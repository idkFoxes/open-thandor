/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/scratch_grid.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* The pathing scratch grid: allocation for a field grid, terrain and runtime classification of its cells,
   occupancy propagation, flood fills and the primary/secondary copy and swap. */

#include <thandor/world/pathing/scratch_grid.h>
#include <thandor/thandor.h>

/* Module data. */

static GridScratchCell *g_GridScratchSecondary = nullptr;

GridScratchCell *g_GridScratchPrimary = nullptr;

uint32_t g_GridScratchWidth = 0;

int32_t g_GridScratchHeight = 0;

/* int32_t[17] terrain-class thresholds, one table in the original
   (indexed by GRID_TERRAIN_THRESHOLD_*). GridScratch classification reads each entry by name;
   ModelDefinition_CopyTerrainClassValues indexes from several entries into their neighbours by the model's
   terrainTraversalClass (assets/model/definitions.cpp). Followed by 12 bytes of 0x90 padding in the original. */
const int32_t g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_COUNT] = {
    0, /* [0] bit 24 max water surface delta (Q12) */
    11500, /* [1] bit 24 max triangle 1 normal angle (high 16) */
    10500, /* [2] bit 25 max selected normal angle (high 16) */
    10500, /* [3] bit 26 max selected normal angle (high 16) */
    10500, /* [4] bit 27 max selected normal angle (high 16) */
    14000, /* [5] contact kind 4 traversal secondary threshold, class 1 */
    15000, /* [6] contact kind 4 traversal secondary threshold, class 2 */
    15500, /* [7] contact kind 4 traversal secondary threshold, class 3 */
    500 /* 0.12207 */, /* [8] bit 28 min water surface delta (Q12) */
    500 /* 0.12207 */, /* [9] bit 29 min water surface delta (Q12) */
    500 /* 0.12207 */, /* [10] bit 30 min water surface delta (Q12) */
    10500, /* [11] bit 28 max triangle 0 normal angle (high 16) */
    10500, /* [12] bit 29 max triangle 0 normal angle (high 16) */
    10500, /* [13] bit 30 max triangle 0 normal angle (high 16) */
    12500, /* [14] fallback traversal secondary threshold, class 4 */
    13500, /* [15] fallback traversal secondary threshold, class 5 */
    14500, /* [16] fallback traversal secondary threshold, class 6 */
};

/* Class bits of one field cell for its 4x4 scratch block: faction presence bits 1..7 from the occupancy
   bytes, terrain class bits 24..30 from water depth and slope, GRID_SCRATCH_BLOCKED on the map edge. */
static GridScratchStateMask GridScratch_ClassifyFieldCell(FieldGridCell *fieldCell)
{
  GridScratchStateMask cellClassMask;
  int waterSurfaceDelta;
  int triangle0Angle;
  int triangle1Angle;
  int selectedNormalAngle;

  /* bit n set when faction slot n (1..7) is present on this field cell */
  cellClassMask =
       ((uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,1)) != 0) +
       ((uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,2)) != 0) +
       ((uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,3)) != 0) +
       ((uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,4)) != 0) +
       ((uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,5)) != 0) +
       ((uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,6)) != 0) +
       (uint32_t)((fieldCell->occupancyMask &
                   FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,7)) != 0) * 2) * 2) * 2) * 2) *
       2) * 2) * 2;
  waterSurfaceDelta = fieldCell->waterSurfaceDelta;
  triangle0Angle = (int)fieldCell->triangle0NormalAngles >> 16;
  triangle1Angle = (int)fieldCell->triangle1NormalAngles >> 16;
  if (waterSurfaceDelta <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT24_MAX_WATER_SURFACE_DELTA]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
  }
  if (triangle1Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT24_MAX_TRIANGLE1_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
  }
  /* below the water surface the first triangle's slope counts */
  selectedNormalAngle = triangle1Angle;
  if (waterSurfaceDelta < 0) {
    selectedNormalAngle = triangle0Angle;
  }
  if (selectedNormalAngle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT25_MAX_SELECTED_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT25;
  }
  if (selectedNormalAngle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT26_MAX_SELECTED_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT26;
  }
  if (selectedNormalAngle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT27_MAX_SELECTED_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT27;
  }
  if (g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MIN_WATER_SURFACE_DELTA] <= waterSurfaceDelta) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
  }
  if (g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT29_MIN_WATER_SURFACE_DELTA] <= waterSurfaceDelta) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
  }
  if (g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT30_MIN_WATER_SURFACE_DELTA] <= waterSurfaceDelta) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
  }
  if (triangle0Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
  }
  if (triangle0Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT29_MAX_TRIANGLE0_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
  }
  if (triangle0Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT30_MAX_TRIANGLE0_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
  }
  if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    cellClassMask = cellClassMask | GRID_SCRATCH_BLOCKED;
  }
  return cellClassMask;
}

/* Flood-fills (GridScratch_FloodFillConnectedCells with wallMask) from every placed runtime model in the owner
   list starting at ownerNode (not NULL): its owner army must have a non-zero factionIndex, and its model
   definition's placement contact kind (placementContactKindIndex, the ArmyPlacementContact dispatch index) must
   not be 1.
   The world point is projected into the skewed scratch grid: row from Y, column from X minus half the row
   term, both rounded (+ GRID_SCRATCH_INDEX_BIAS_Q12 >> GRID_SCRATCH_CELL_SHIFT); points off the grid are
   skipped. */
static void GridScratch_FloodFillFromPlacedRuntimeModels(WorldOwnerListNode *ownerNode,GridScratchStateMask wallMask)
{
  int64_t wideProductX;
  int64_t wideProductY;
  uint32_t scaledRowTerm;
  int modelColumn;
  int modelRow;

  do {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
        (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != 0) &&
        ((int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->placementContactKindIndex != 1)) {
      wideProductX = (int64_t)ownerNode->worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      wideProductY = (int64_t)ownerNode->worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      scaledRowTerm = FIXED_PRODUCT_SHR(wideProductY, Q20_SHIFT + 1);
      modelColumn = (int)(((FIXED_PRODUCT_SHR(wideProductX, Q20_SHIFT)) - scaledRowTerm) +
                   GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
      modelRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
      if ((-1 < modelColumn) && (-1 < modelRow) && (modelColumn < (int)g_GridScratchWidth) &&
          (modelRow < (int)g_GridScratchHeight)) {
        GridScratch_FloodFillConnectedCells
                  (wallMask,g_GridScratchWidth << 3,g_GridScratchPrimary + (int32_t)(modelRow * g_GridScratchWidth) + modelColumn);
      }
    }
    ownerNode = ownerNode->nextNode;
  } while (ownerNode != nullptr);
}

/* Rebuilds the terrain classification of the scratch grid (4x4 scratch cells per field cell): clears the
   faction and class bits, derives terrain class bits 24..30 from water depth and slope of each field cell
   and faction presence bits 1..7 from its occupancy bytes, and marks map-edge cells GRID_SCRATCH_BLOCKED.
   Then every class-24 area is grown by one field cell, and each runtime model flood-fills the region it stands
   in; cells no model can reach get classes 28..30 resp. 25..27 so they count as unreachable. The class
   bits are a scratch-only namespace and never written back to FieldGridCell.flagsAndMaterial.
   The dilation pass reads four scratch rows before the first and after the last row, as in the original.
*/
void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime)

{
  FieldGridDimension fieldGridWidth;
  FieldCellPackedFlagsAndMaterial cellFlags;
  uint32_t scratchWidth;
  uint32_t scratchStride;
  int blocksToClear;
  int cellsRemaining;
  int cellsToPromote;
  GridScratchStateMask cellClassMask;
  FieldGridDimension columnsRemaining;
  FieldGridCell *fieldCell;
  GridScratchCell *promoteCursor;
  GridScratchCell *scratchCursor;
  WorldOwnerListNode *ownerNode;
  FieldGridDimension rowsRemaining;
  FieldGridAsset *fieldGridAsset;

  fieldGridAsset = worldRuntime->fieldGrid;
  fieldGridWidth = fieldGridAsset->gridWidth;
  rowsRemaining = fieldGridAsset->gridHeight;
  /* one 16-cell block per field cell */
  blocksToClear = fieldGridWidth * rowsRemaining;
  scratchCursor = g_GridScratchPrimary;
  do {
    scratchCursor->stateMask = scratchCursor->stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[1].stateMask = scratchCursor[1].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[2].stateMask = scratchCursor[2].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[3].stateMask = scratchCursor[3].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[4].stateMask = scratchCursor[4].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[5].stateMask = scratchCursor[5].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[6].stateMask = scratchCursor[6].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[7].stateMask = scratchCursor[7].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[8].stateMask = scratchCursor[8].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[9].stateMask = scratchCursor[9].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[10].stateMask = scratchCursor[10].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[11].stateMask = scratchCursor[11].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[12].stateMask = scratchCursor[12].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[13].stateMask = scratchCursor[13].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[14].stateMask = scratchCursor[14].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[15].stateMask = scratchCursor[15].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchWidth = g_GridScratchWidth;
    scratchCursor = scratchCursor + 16;
    blocksToClear--;
  } while (blocksToClear != 0);
  fieldCell = fieldGridAsset->cells;
  columnsRemaining = fieldGridWidth;
  scratchCursor = g_GridScratchPrimary;
  do {
    do {
      cellClassMask = GridScratch_ClassifyFieldCell(fieldCell);
      cellFlags = fieldCell->flagsAndMaterial;
      if ((cellFlags & FIELD_CELL_FIRST_ROW_BOUNDARY) == 0) {
        scratchCursor[(int32_t)(scratchWidth * -2 + 2)].stateMask = scratchCursor[(int32_t)(scratchWidth * -2 + 2)].stateMask | cellClassMask;
        scratchCursor[(int32_t)(scratchWidth * -2 + 3)].stateMask = scratchCursor[(int32_t)(scratchWidth * -2 + 3)].stateMask | cellClassMask;
        scratchCursor[(int32_t)(1 - scratchWidth)].stateMask = scratchCursor[(int32_t)(1 - scratchWidth)].stateMask | cellClassMask;
        scratchCursor[(int32_t)(2 - scratchWidth)].stateMask = scratchCursor[(int32_t)(2 - scratchWidth)].stateMask | cellClassMask;
        scratchCursor[(int32_t)(3 - scratchWidth)].stateMask = scratchCursor[(int32_t)(3 - scratchWidth)].stateMask | cellClassMask;
        if ((cellFlags & FIELD_CELL_LAST_COLUMN_BOUNDARY) == 0) {
          scratchCursor[(int32_t)(scratchWidth * -2 + 4)].stateMask = scratchCursor[(int32_t)(scratchWidth * -2 + 4)].stateMask | cellClassMask;
          scratchCursor[(int32_t)(4 - scratchWidth)].stateMask = scratchCursor[(int32_t)(4 - scratchWidth)].stateMask | cellClassMask;
          scratchCursor[(int32_t)(5 - scratchWidth)].stateMask = scratchCursor[(int32_t)(5 - scratchWidth)].stateMask | cellClassMask;
        }
      }
      if ((cellFlags & FIELD_CELL_LAST_COLUMN_BOUNDARY) == 0) {
        scratchCursor[4].stateMask = scratchCursor[4].stateMask | cellClassMask;
        scratchCursor[5].stateMask = scratchCursor[5].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 4].stateMask = scratchCursor[scratchWidth + 4].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 5].stateMask = scratchCursor[scratchWidth + 5].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + 4].stateMask = scratchCursor[scratchWidth * 2 + 4].stateMask | cellClassMask;
      }
      scratchCursor->stateMask = scratchCursor->stateMask | cellClassMask;
      scratchCursor[1].stateMask = scratchCursor[1].stateMask | cellClassMask;
      scratchCursor[2].stateMask = scratchCursor[2].stateMask | cellClassMask;
      scratchCursor[3].stateMask = scratchCursor[3].stateMask | cellClassMask;
      scratchCursor = scratchCursor + scratchWidth;
      if ((cellFlags & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
        scratchCursor[-1].stateMask = scratchCursor[-1].stateMask | cellClassMask;
        scratchCursor[scratchWidth - 1].stateMask = scratchCursor[scratchWidth - 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth - 2].stateMask = scratchCursor[scratchWidth - 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 - 1].stateMask = scratchCursor[scratchWidth * 2 - 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 - 2].stateMask = scratchCursor[scratchWidth * 2 - 2].stateMask | cellClassMask;
      }
      scratchCursor->stateMask = scratchCursor->stateMask | cellClassMask;
      scratchCursor[1].stateMask = scratchCursor[1].stateMask | cellClassMask;
      scratchCursor[2].stateMask = scratchCursor[2].stateMask | cellClassMask;
      scratchCursor[3].stateMask = scratchCursor[3].stateMask | cellClassMask;
      scratchCursor[scratchWidth].stateMask = scratchCursor[scratchWidth].stateMask | cellClassMask;
      scratchCursor[scratchWidth + 1].stateMask = scratchCursor[scratchWidth + 1].stateMask | cellClassMask;
      scratchCursor[scratchWidth + 2].stateMask = scratchCursor[scratchWidth + 2].stateMask | cellClassMask;
      scratchCursor[scratchWidth + 3].stateMask = scratchCursor[scratchWidth + 3].stateMask | cellClassMask;
      scratchCursor = scratchCursor + scratchWidth * 2;
      scratchCursor->stateMask = scratchCursor->stateMask | cellClassMask;
      scratchCursor[1].stateMask = scratchCursor[1].stateMask | cellClassMask;
      scratchCursor[2].stateMask = scratchCursor[2].stateMask | cellClassMask;
      scratchCursor[3].stateMask = scratchCursor[3].stateMask | cellClassMask;
      if ((cellFlags & FIELD_CELL_LAST_ROW_BOUNDARY) == 0) {
        scratchCursor[scratchWidth].stateMask = scratchCursor[scratchWidth].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 1].stateMask = scratchCursor[scratchWidth + 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 2].stateMask = scratchCursor[scratchWidth + 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2].stateMask = scratchCursor[scratchWidth * 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + 1].stateMask = scratchCursor[scratchWidth * 2 + 1].stateMask | cellClassMask;
        if ((cellFlags & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
          scratchCursor[scratchWidth - 1].stateMask = scratchCursor[scratchWidth - 1].stateMask | cellClassMask;
          scratchCursor[scratchWidth - 2].stateMask = scratchCursor[scratchWidth - 2].stateMask | cellClassMask;
          scratchCursor[scratchWidth * 2 - 1].stateMask = scratchCursor[scratchWidth * 2 - 1].stateMask | cellClassMask;
        }
      }
      scratchStride = g_GridScratchWidth;
      scratchCursor = scratchCursor + (int32_t)(scratchWidth * -3) + 4;
      fieldCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    scratchCursor = scratchCursor + scratchWidth * 3;
    rowsRemaining--;
    columnsRemaining = fieldGridWidth;
  } while (rowsRemaining != 0);
  /* grow class 24 by one field cell: mark (visited) every open cell with a class-24 cell four scratch cells
     away in one of the six hex directions, then promote the marks */
  cellsToPromote = g_GridScratchHeight * g_GridScratchWidth;
  scratchCursor = g_GridScratchPrimary + (int32_t)(g_GridScratchWidth * -4);
  cellsRemaining = cellsToPromote;
  do {
    scratchCursor[scratchStride * 4].stateMask =
         scratchCursor[scratchStride * 4].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    if (((scratchCursor[scratchStride * 4].stateMask & GRID_SCRATCH_BLOCKED) == 0) &&
       (((scratchCursor[scratchStride * 4].stateMask | scratchCursor->stateMask | scratchCursor[4].stateMask |
          scratchCursor[scratchStride * 4 - 4].stateMask | scratchCursor[scratchStride * 4 + 4].stateMask |
          scratchCursor[scratchStride * 8 - 4].stateMask | scratchCursor[scratchStride * 8].stateMask) &
        GRID_SCRATCH_TERRAIN_CLASS_BIT24) != 0)) {
      scratchCursor[scratchStride * 4].stateMask =
           scratchCursor[scratchStride * 4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    scratchCursor++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  promoteCursor = g_GridScratchPrimary;
  do {
    if ((promoteCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) {
      promoteCursor->stateMask = promoteCursor->stateMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      promoteCursor->stateMask = promoteCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    cellsToPromote--;
    promoteCursor++;
  } while (cellsToPromote != 0);
  ownerNode = worldRuntime->ownerListHead;
  if (ownerNode != nullptr) {
    /* flood-fill from every placed runtime model through cells open for classes 28..30; every cell left
       unreached gets those classes */
    GridScratch_FloodFillFromPlacedRuntimeModels
              (ownerNode,GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT30 | GRID_SCRATCH_TERRAIN_CLASS_BIT29 |
               GRID_SCRATCH_TERRAIN_CLASS_BIT28 | GRID_SCRATCH_TRAVERSAL_VISITED);
    cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
    scratchCursor = g_GridScratchPrimary;
    do {
      if ((scratchCursor->stateMask &
          (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
           GRID_SCRATCH_TERRAIN_CLASS_BIT28|GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
        scratchCursor->stateMask =
             scratchCursor->stateMask |
             (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
             GRID_SCRATCH_TERRAIN_CLASS_BIT28);
      }
      scratchCursor->stateMask = scratchCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      scratchCursor++;
      cellsRemaining--;
    } while (cellsRemaining != 0);
    /* second flood fill from the same models, this time for classes 25..27 */
    GridScratch_FloodFillFromPlacedRuntimeModels
              (worldRuntime->ownerListHead,GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT27 |
               GRID_SCRATCH_TERRAIN_CLASS_BIT26 | GRID_SCRATCH_TERRAIN_CLASS_BIT25 | GRID_SCRATCH_TRAVERSAL_VISITED);
    cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
    scratchCursor = g_GridScratchPrimary;
    do {
      if ((scratchCursor->stateMask &
          (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
           GRID_SCRATCH_TERRAIN_CLASS_BIT25|GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
        scratchCursor->stateMask =
             scratchCursor->stateMask |
             (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
             GRID_SCRATCH_TERRAIN_CLASS_BIT25);
      }
      scratchCursor++;
      cellsRemaining--;
    } while (cellsRemaining != 0);
  }
}

/* Sizes the pathing scratch grids for a field grid (4x4 scratch cells per field cell, 8-byte GridScratchCell
   records): allocates the primary and secondary scratch grids and the GRID_PATH_COST_QUEUE_BYTES path-cost
   pointer queue (g_GridPathCostQueueBegin..End; 0x180000 bytes in the original, 0x300000 with 8-byte pointers), each replacing and freeing the previous buffer. Returns true on success;
   on failure returns false and writes the allocator error to *outError (untouched on success).
*/
Bool8 GridScratch_AllocateForFieldGrid(FieldGridAsset *fieldGrid,uint32_t *outError)

{
  GridScratchCell *previousSecondaryScratchBuffer;
  GridScratchCell **previousCostQueueBuffer;
  uint32_t *newScratchBuffer;
  uint32_t *newSecondaryScratchBuffer;
  void *newAuxiliaryBuffer;
  uint32_t bytes;
  uint32_t allocError;
  GridScratchCell *previousScratchBuffer;

  g_GridScratchWidth = fieldGrid->gridWidth * 4;
  g_GridScratchHeight = fieldGrid->gridHeight * 4;
  bytes = fieldGrid->gridWidth * (4 * sizeof(GridScratchCell)) * g_GridScratchHeight; /* scratch width * height * 8 */
  allocError = g_MemoryApi.alloc(bytes,(void **)&newScratchBuffer);
  previousScratchBuffer = g_GridScratchPrimary;
  if (allocError == 0) {
    /* the original swaps the pointers atomically */
    LOCK();
    UNLOCK();
    g_GridScratchPrimary = (GridScratchCell *)newScratchBuffer;
    g_MemoryApi.free(previousScratchBuffer);
    allocError = g_MemoryApi.alloc(bytes,(void **)&newSecondaryScratchBuffer);
    previousSecondaryScratchBuffer = g_GridScratchSecondary;
    if (allocError == 0) {
      LOCK();
      UNLOCK();
      g_GridScratchSecondary = (GridScratchCell *)newSecondaryScratchBuffer;
      g_MemoryApi.free(previousSecondaryScratchBuffer);
      allocError = g_MemoryApi.alloc(GRID_PATH_COST_QUEUE_BYTES,&newAuxiliaryBuffer);
      previousCostQueueBuffer = g_GridPathCostQueueBegin;
      if (allocError == 0) {
        g_GridPathCostQueueEnd = (GridScratchCell **)((uintptr_t)newAuxiliaryBuffer + GRID_PATH_COST_QUEUE_BYTES);
        g_GridPathCostQueueBegin = (GridScratchCell **)newAuxiliaryBuffer;
        g_MemoryApi.free(previousCostQueueBuffer);
        return true;
      }
    }
  }
  *outError = allocError;
  return false;
}

/* Frees the path-cost queue and both scratch grids allocated by GridScratch_AllocateForFieldGrid and
   clears the three pointers, so a later session starts without stale buffers.
*/
void GridScratch_ReleaseBuffers()

{
  g_MemoryApi.free(g_GridPathCostQueueBegin);
  g_MemoryApi.free(g_GridScratchPrimary);
  g_MemoryApi.free(g_GridScratchSecondary);
  g_GridPathCostQueueBegin = nullptr;
  g_GridScratchPrimary = nullptr;
  g_GridScratchSecondary = nullptr;
}

/* Tail of tick-wheel case 7: after the occupancy rebuild, turns each non-edge field cell's occupancy bytes
   of faction slots 1..7 into scratch bits 1..7 and ORs them into the cell's 4x4 scratch block and a ring of
   surrounding scratch cells, so faction presence is dilated into the scratch grid used by pathing.
*/
void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid)

{
  uint32_t *lowerScratchCursor;
  FieldGridDimension columnsRemaining;
  FieldGridCell *currentFieldCell;
  uint32_t *scratchCellCursor; /* dword view of the 8-byte scratch cells: [2n] = stateMask of cell n */
  uint32_t *propagatedScratchCursor;
  FieldGridDimension rowsRemaining;
  FieldGridDimension gridWidth;
  uint32_t factionPresenceMask;
  uint32_t scratchWidth;
  uint32_t *nextScratchCellCursor;
  
  scratchWidth = g_GridScratchWidth;
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  columnsRemaining = gridWidth;
  currentFieldCell = fieldGrid->cells;
  scratchCellCursor = (uint32_t *)&g_GridScratchPrimary->stateMask;
  do {
    do {
      /* the original advances first and then tests the flags of the current cell */
      nextScratchCellCursor = scratchCellCursor + 8;
      if ((currentFieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        factionPresenceMask =
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,1)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,2)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,3)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,4)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,5)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,6)) != 0) +
             (uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,7)) != 0) * 2) * 2) * 2) * 2)
             * 2) * 2) * 2;
        scratchCellCursor[(int32_t)(scratchWidth * -4 + 4)] =
             scratchCellCursor[(int32_t)(scratchWidth * -4 + 4)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -4 + 6)] =
             scratchCellCursor[(int32_t)(scratchWidth * -4 + 6)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -2 + 2)] =
             scratchCellCursor[(int32_t)(scratchWidth * -2 + 2)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -2 + 4)] =
             scratchCellCursor[(int32_t)(scratchWidth * -2 + 4)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -2 + 6)] =
             scratchCellCursor[(int32_t)(scratchWidth * -2 + 6)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -4 + 8)] =
             scratchCellCursor[(int32_t)(scratchWidth * -4 + 8)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -2 + 8)] =
             scratchCellCursor[(int32_t)(scratchWidth * -2 + 8)] | factionPresenceMask;
        scratchCellCursor[(int32_t)(scratchWidth * -2 + 10)] =
             scratchCellCursor[(int32_t)(scratchWidth * -2 + 10)] | factionPresenceMask;
        scratchCellCursor[8] = scratchCellCursor[8] | factionPresenceMask;
        scratchCellCursor[10] = scratchCellCursor[10] | factionPresenceMask;
        scratchCellCursor[scratchWidth * 2 + 8] =
             scratchCellCursor[scratchWidth * 2 + 8] | factionPresenceMask;
        scratchCellCursor[scratchWidth * 2 + 10] =
             scratchCellCursor[scratchWidth * 2 + 10] | factionPresenceMask;
        scratchCellCursor[scratchWidth * 4 + 8] =
             scratchCellCursor[scratchWidth * 4 + 8] | factionPresenceMask;
        *scratchCellCursor = *scratchCellCursor | factionPresenceMask;
        scratchCellCursor[2] = scratchCellCursor[2] | factionPresenceMask;
        scratchCellCursor[4] = scratchCellCursor[4] | factionPresenceMask;
        scratchCellCursor[6] = scratchCellCursor[6] | factionPresenceMask;
        propagatedScratchCursor = scratchCellCursor + scratchWidth * 2;
        propagatedScratchCursor[-2] = propagatedScratchCursor[-2] | factionPresenceMask;
        propagatedScratchCursor[(int32_t)(scratchWidth * 2 + -2)] =
             propagatedScratchCursor[(int32_t)(scratchWidth * 2 + -2)] | factionPresenceMask;
        propagatedScratchCursor[(int32_t)(scratchWidth * 2 + -4)] =
             propagatedScratchCursor[(int32_t)(scratchWidth * 2 + -4)] | factionPresenceMask;
        propagatedScratchCursor[(int32_t)(scratchWidth * 4 + -2)] =
             propagatedScratchCursor[(int32_t)(scratchWidth * 4 + -2)] | factionPresenceMask;
        propagatedScratchCursor[(int32_t)(scratchWidth * 4 + -4)] =
             propagatedScratchCursor[(int32_t)(scratchWidth * 4 + -4)] | factionPresenceMask;
        *propagatedScratchCursor = *propagatedScratchCursor | factionPresenceMask;
        propagatedScratchCursor[2] = propagatedScratchCursor[2] | factionPresenceMask;
        propagatedScratchCursor[4] = propagatedScratchCursor[4] | factionPresenceMask;
        propagatedScratchCursor[6] = propagatedScratchCursor[6] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2] =
             propagatedScratchCursor[scratchWidth * 2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + 2] =
             propagatedScratchCursor[scratchWidth * 2 + 2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + 4] =
             propagatedScratchCursor[scratchWidth * 2 + 4] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + 6] =
             propagatedScratchCursor[scratchWidth * 2 + 6] | factionPresenceMask;
        lowerScratchCursor = propagatedScratchCursor + scratchWidth * 4;
        *lowerScratchCursor = *lowerScratchCursor | factionPresenceMask;
        lowerScratchCursor[2] = lowerScratchCursor[2] | factionPresenceMask;
        lowerScratchCursor[4] = lowerScratchCursor[4] | factionPresenceMask;
        lowerScratchCursor[6] = lowerScratchCursor[6] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2] = lowerScratchCursor[scratchWidth * 2] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2 + 2] = lowerScratchCursor[scratchWidth * 2 + 2] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2 + 4] = lowerScratchCursor[scratchWidth * 2 + 4] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 4] = lowerScratchCursor[scratchWidth * 4] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 4 + 2] = lowerScratchCursor[scratchWidth * 4 + 2] | factionPresenceMask;
        lowerScratchCursor[(int32_t)(scratchWidth * 2 + -2)] = lowerScratchCursor[(int32_t)(scratchWidth * 2 + -2)] | factionPresenceMask
        ;
        lowerScratchCursor[(int32_t)(scratchWidth * 2 + -4)] = lowerScratchCursor[(int32_t)(scratchWidth * 2 + -4)] | factionPresenceMask
        ;
        lowerScratchCursor[(int32_t)(scratchWidth * 4 + -2)] = lowerScratchCursor[(int32_t)(scratchWidth * 4 + -2)] | factionPresenceMask
        ;
        nextScratchCellCursor = lowerScratchCursor + (int32_t)(scratchWidth * -6) + 8;
      }
      scratchCellCursor = nextScratchCellCursor;
      columnsRemaining--;
      currentFieldCell++;
    } while (columnsRemaining != 0);
    scratchCellCursor = scratchCellCursor + scratchWidth * 6;
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
}

/* Tests whether a world point may be used for pathing: projects it onto the grid-scratch cells (the same
   skewed field projection as the placement tests, with 10 instead of 12 fraction bits) and rejects it (returns true)
   when it lies outside the scratch grid, the cell is GRID_SCRATCH_BLOCKED, or the cell has distance band bit
   8 + lowBandIndex or bit 24 + highBandIndex set.
*/
Bool8 GridScratch_TestProjectedCellMaskBands(Q12 worldYQ12,Q12 worldXQ12,uint8_t lowBandIndex,uint8_t highBandIndex)

{
  GridScratchStateMask cellStateMask;
  int cellColumn;
  uint32_t scaledRowTerm;
  int cellRow;

  /* 64-bit products shifted back by 21 / 20 bits; column = x term - row term, row = 2 * row term,
     both biased by GRID_SCRATCH_INDEX_BIAS_Q12 and taken in scratch cells */
  scaledRowTerm = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  cellColumn = (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  cellRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (cellColumn < 0 || cellRow < 0 ||
      cellColumn >= (int)g_GridScratchWidth || cellRow >= (int)g_GridScratchHeight) {
    return true;
  }
  cellStateMask = g_GridScratchPrimary[(int32_t)(cellRow * g_GridScratchWidth + cellColumn)].stateMask;
  /* sign bit: GRID_SCRATCH_BLOCKED */
  if ((int)cellStateMask < 0) {
    return true;
  }
  if ((GRID_SCRATCH_LOW_BAND0 << (lowBandIndex & 31) & cellStateMask) != 0) {
    return true;
  }
  return (GRID_SCRATCH_TERRAIN_CLASS_BIT24 << (highBandIndex & 31) & cellStateMask) != 0;
}

/* Copies the whole primary scratch grid into the secondary one (two dwords per 8-byte GridScratchCell), so pathing can plan on a copy and swap back afterwards.
*/
void __cdecl GridScratch_CopyPrimaryToSecondary()

{
  int scratchDwordsRemaining;
  uint32_t *primaryReadCursor;
  uint32_t *secondaryWriteCursor;

  scratchDwordsRemaining = g_GridScratchWidth * g_GridScratchHeight * 2;
  primaryReadCursor = (uint32_t *)&g_GridScratchPrimary->stateMask;
  secondaryWriteCursor = (uint32_t *)&g_GridScratchSecondary->stateMask;
  for (; scratchDwordsRemaining != 0; scratchDwordsRemaining--) {
    *secondaryWriteCursor = *primaryReadCursor;
    primaryReadCursor++;
    secondaryWriteCursor++;
  }
}

/* Swaps the primary and secondary scratch grid pointers, making the copy made by
   GridScratch_CopyPrimaryToSecondary the working grid, or restoring the original afterwards.
*/
void GridScratch_SwapPrimarySecondary()

{
  GridScratchCell *previousSecondaryBuffer;
  
  previousSecondaryBuffer = g_GridScratchSecondary;
  LOCK();
  g_GridScratchSecondary = g_GridScratchPrimary;
  UNLOCK();
  g_GridScratchPrimary = previousSecondaryBuffer;
}

/* Scanline flood fill over the scratch grid: marks the horizontal run of cells around currentCell that have no
   traversalMask bit as visited, then recurses into every such cell of the row above and the row below that span.
   A blocked or already visited start cell does nothing.
*/
void GridScratch_FloodFillConnectedCells
          (GridScratchStateMask traversalMask,uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *leftStopCell;
  GridScratchCell *rightStopCell;
  GridScratchCell *rowAboveCell;
  GridScratchCell *rowAboveLastCell;
  GridScratchCell *rowBelowCell;
  GridScratchCell *rowBelowEndCell;

  if ((currentCell->stateMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) != 0) {
    return;
  }
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  for (leftStopCell = currentCell - 1; (leftStopCell->stateMask & traversalMask) == 0; leftStopCell--) {
    leftStopCell->stateMask = leftStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  for (rightStopCell = currentCell + 1; (rightStopCell->stateMask & traversalMask) == 0; rightStopCell++) {
    rightStopCell->stateMask = rightStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* the row above is scanned from the span's first cell up to the column of the right stopping cell, the row
     below from the column of the left stopping cell up to the span's last cell (addresses as in the original).
     Both ranges are never empty, so testing before the first cell is the same as the original's do-while. */
  rowAboveLastCell = (GridScratchCell *)((uint8_t *)rightStopCell - rowStrideBytes);
  for (rowAboveCell = (GridScratchCell *)((uint8_t *)(leftStopCell + 1) - rowStrideBytes);
       rowAboveCell <= rowAboveLastCell; rowAboveCell++) {
    if ((rowAboveCell->stateMask & traversalMask) == 0) {
      GridScratch_FloodFillConnectedCells(traversalMask,rowStrideBytes,rowAboveCell);
    }
  }
  rowBelowEndCell = (GridScratchCell *)((uint8_t *)rightStopCell + rowStrideBytes);
  for (rowBelowCell = (GridScratchCell *)((uint8_t *)leftStopCell + rowStrideBytes);
       rowBelowCell < rowBelowEndCell; rowBelowCell++) {
    if ((rowBelowCell->stateMask & traversalMask) == 0) {
      GridScratch_FloodFillConnectedCells(traversalMask,rowStrideBytes,rowBelowCell);
    }
  }
}

/* Prepares the scratch grid for a cost propagation: clears the visited bit and sets pathCost to
   GRID_PATH_COST_UNREACHED in every cell, sixteen cells per unrolled iteration.
*/
void GridScratch_ResetTraversalFlagsAndCosts()

{
  uint32_t cellsRemaining;
  uint32_t *scratchRecordCursor; /* dword view of the 8-byte cells: [2n] = stateMask, [2n + 1] = pathCost */
  Bool8 fullRecordBlockRemaining;

  cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
  scratchRecordCursor = (uint32_t *)&g_GridScratchPrimary->stateMask;
  do {
    *scratchRecordCursor = *scratchRecordCursor & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[1] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[2] = scratchRecordCursor[2] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[3] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[4] = scratchRecordCursor[4] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[5] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[6] = scratchRecordCursor[6] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[7] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[8] = scratchRecordCursor[8] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[9] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[10] = scratchRecordCursor[10] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[11] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[12] = scratchRecordCursor[12] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[13] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[14] = scratchRecordCursor[14] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[15] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[16] = scratchRecordCursor[16] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[17] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[18] = scratchRecordCursor[18] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[19] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[20] = scratchRecordCursor[20] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[21] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[22] = scratchRecordCursor[22] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[23] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[24] = scratchRecordCursor[24] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[25] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[26] = scratchRecordCursor[26] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[27] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[28] = scratchRecordCursor[28] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[29] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[30] = scratchRecordCursor[30] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[31] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor = scratchRecordCursor + 32;
    fullRecordBlockRemaining = 15 < cellsRemaining;
    cellsRemaining = cellsRemaining - 16;
  } while (fullRecordBlockRemaining && cellsRemaining != 0);
}
