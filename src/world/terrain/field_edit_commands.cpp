/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_edit_commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid editor commands (PlayerRuntimeId commands of the map editor): cell delta drags, encoded cell
   updates, flag setters, the player scratch plane and the masked/rectangular transitions. */

#include <thandor/world/terrain/field_edit_commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* The selection block of the player a map-editor command names, or NULL (logged once) when there is none.
   The original indexes g_SelectionPlayerRuntimeBlockPointers with the command's player id and dereferences
   the entry unchecked; bounded here because editor commands arrive from any network peer. */
static SelectionPlayerRuntimeBlock *FieldGridEdit_PlayerBlock(PlayerRuntimeId playerRuntimeId)
{
  static Bool8 s_missingBlockLogged = false;
  SelectionPlayerRuntimeBlock *playerBlock;

  playerBlock = nullptr;
  if (playerRuntimeId <
      sizeof(g_SelectionPlayerRuntimeBlockPointers) / sizeof(g_SelectionPlayerRuntimeBlockPointers[0])) {
    playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  }
  if ((playerBlock == nullptr) && !s_missingBlockLogged) {
    s_missingBlockLogged = true;
    Thandor_Log("terrain editor: command for player %u without a selection block, ignored",
                (unsigned)playerRuntimeId);
  }
  return playerBlock;
}

/* The player's terrain height scratch plane (SelectionPlayerRuntimeBlock.terrainHeightScratchPlane) for a
   map-editor command, or NULL (logged once). The original writes through the field unchecked; it is never
   assigned in this code base, so the editor commands that use it do nothing instead of writing through NULL. */
int *FieldGridEdit_PlayerHeightPlane(PlayerRuntimeId playerRuntimeId)
{
  static Bool8 s_missingPlaneLogged = false;
  SelectionPlayerRuntimeBlock *playerBlock;
  int *heightPlane;

  playerBlock = FieldGridEdit_PlayerBlock(playerRuntimeId);
  if (playerBlock == nullptr) {
    return nullptr;
  }
  heightPlane = playerBlock->terrainHeightScratchPlane;
  if ((heightPlane == nullptr) && !s_missingPlaneLogged) {
    s_missingPlaneLogged = true;
    Thandor_Log("terrain editor: player %u has no terrain height plane, height edit ignored",
                (unsigned)playerRuntimeId);
  }
  return heightPlane;
}

/* The same for the player's material edit plane (SelectionPlayerRuntimeBlock.terrainMaterialEditPlane). */
uint32_t *FieldGridEdit_PlayerMaterialPlane(PlayerRuntimeId playerRuntimeId)
{
  static Bool8 s_missingPlaneLogged = false;
  SelectionPlayerRuntimeBlock *playerBlock;
  uint32_t *materialPlane;

  playerBlock = FieldGridEdit_PlayerBlock(playerRuntimeId);
  if (playerBlock == nullptr) {
    return nullptr;
  }
  materialPlane = playerBlock->terrainMaterialEditPlane;
  if ((materialPlane == nullptr) && !s_missingPlaneLogged) {
    s_missingPlaneLogged = true;
    Thandor_Log("terrain editor: player %u has no material edit plane, material edit ignored",
                (unsigned)playerRuntimeId);
  }
  return materialPlane;
}

/* Whether the Q12 grid row/column lies on a cell of the grid, so cells[row * width + column] is inside the
   array. Checked on the flat index (as the original computes it), so every in-array position stays as it was. */
static Bool8 FieldGrid_IsCellIndexInGrid(Q12 gridRowQ12,Q12 gridColumnQ12,const FieldGridAsset *fieldGrid)
{
  int64_t cellIndex;

  cellIndex = (int64_t)(gridRowQ12 >> Q12_SHIFT) * (int64_t)fieldGrid->gridWidth + (gridColumnQ12 >> Q12_SHIFT);
  return (0 <= cellIndex) && (cellIndex < (int64_t)fieldGrid->gridWidth * (int64_t)fieldGrid->gridHeight);
}

/* Shared by FieldGrid_ApplyPositiveCellDeltas and FieldGrid_ApplyNegativeCellDeltas after a cell's height changed:
   unless the cell lies on the grid edge, refreshes the normals and light of the cell and its six neighbours (the
   left and right one only while their scratch entry is 0). scratchEntry is the cell's entry in the scratch plane. */
static void FieldGridCell_RefreshChangedCellAndNeighbours(int rowStrideBytes,FieldGridCell *cell,
          const int *scratchEntry)

{
  FieldGridCell *rowAboveCell;
  FieldGridCell *rowBelowLeftCell;

  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return;
  }
  FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
  FieldGridCell_ComputeDirectionalLightColor(cell);
  if (((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchEntry[-1] == 0)) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell - 1);
    FieldGridCell_ComputeDirectionalLightColor(cell - 1);
  }
  if (((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchEntry[1] == 0)) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
    FieldGridCell_ComputeDirectionalLightColor(cell + 1);
  }
  rowAboveCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes);
  if ((rowAboveCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowAboveCell);
    FieldGridCell_ComputeDirectionalLightColor(rowAboveCell);
  }
  if ((rowAboveCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowAboveCell + 1);
    FieldGridCell_ComputeDirectionalLightColor(rowAboveCell + 1);
  }
  rowBelowLeftCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,rowStrideBytes);
  if ((rowBelowLeftCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowBelowLeftCell);
    FieldGridCell_ComputeDirectionalLightColor(rowBelowLeftCell);
  }
  if ((rowBelowLeftCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,rowBelowLeftCell + 1);
    FieldGridCell_ComputeDirectionalLightColor(rowBelowLeftCell + 1);
  }
}

/* In-game command INGAME_COMMAND_EDITOR_RAISE_HEIGHTS (0x1F70, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor drag (mode G 0, C 0): pulls the terrain towards the anchor cell's height moved by the vertical
   drag distance, with a cosine falloff over the horizontal drag distance. The player's scratch plane holds the
   height change of the previous drag step: it is undone first, the plane is refilled with the current heights
   and FieldGrid_ProcessHorizontalSpan moves them towards the target around the anchor (or around every selected
   pair); the difference is applied again and kept in the plane. Water surfaces stay at their level
   (waterSurfaceDelta moves opposite to the terrain). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_ApplyPositiveCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  int cellDelta;
  int targetHeight;
  uint32_t remainingPairCount;
  int cellCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *firstCell;
  FieldGridCell *cell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  Bool8 containsAnchorPair;
  int *accumulatorPlane;

  accumulatorPlane = FieldGridEdit_PlayerHeightPlane(playerRuntimeId);
  if (accumulatorPlane == nullptr) {
    return;
  }
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  rowLength = fieldGrid->gridWidth;
  cellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  firstCell = fieldGrid->cells;
  /* undo the previous step and refill the plane with the current heights */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight - cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta + cellDelta;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    *scratchHeightCursor = cell->terrainHeight;
    cell++;
    scratchHeightCursor++;
  }
  /* the drag distances are packed as vertical << 16 | horizontal (signed 16-bit each) */
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorRowQ12,anchorColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessHorizontalSpan
              (anchorRowQ12,anchorColumnQ12,(int)packedDragDeltaXY16 >> 16,
               (int)(short)packedDragDeltaXY16,anchorRowQ12,anchorColumnQ12,accumulatorPlane,
               fieldGrid);
  }
  else {
    pairRecord = FieldGridEdit_PlayerBlock(playerRuntimeId)->markedCells;
    for (remainingPairCount = FieldGridEdit_PlayerBlock(playerRuntimeId)->markedCellCount;
        remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ProcessHorizontalSpan
                (anchorRowQ12,anchorColumnQ12,(int)packedDragDeltaXY16 >> 16,
                 (int)(short)packedDragDeltaXY16,pairRecord->pairValue,pairRecord->pairKey,accumulatorPlane,
                 fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  /* apply the new heights and keep their difference in the plane for the next step */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    LOCK();
    targetHeight = *scratchHeightCursor;
    *scratchHeightCursor = 0;
    UNLOCK();
    cellDelta = targetHeight - cell->terrainHeight;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight + cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - cellDelta;
      *scratchHeightCursor = cellDelta;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    cell++;
    scratchHeightCursor++;
  }
}

/* In-game command INGAME_COMMAND_EDITOR_LOWER_HEIGHTS (0x2290, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor drag (mode G 0, C 1): raises or lowers the terrain by the vertical drag distance, with a cosine
   falloff over the horizontal drag distance. The player's scratch plane holds the height change of the previous
   drag step: it is undone and cleared, then FieldGrid_ProcessVerticalSpan writes the new per-cell offsets around
   the anchor (or around every selected pair) and they are added to the terrain; water surfaces stay at their
   level. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_ApplyNegativeCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  int cellDelta;
  uint32_t remainingPairCount;
  int cellCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *firstCell;
  FieldGridCell *cell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  Bool8 containsAnchorPair;
  int *accumulatorPlane;

  accumulatorPlane = FieldGridEdit_PlayerHeightPlane(playerRuntimeId);
  if (accumulatorPlane == nullptr) {
    return;
  }
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  rowLength = fieldGrid->gridWidth;
  cellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  firstCell = fieldGrid->cells;
  /* undo and clear the previous step */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight - cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta + cellDelta;
      *scratchHeightCursor = 0;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    cell++;
    scratchHeightCursor++;
  }
  /* the drag distances are packed as vertical << 16 | horizontal (signed 16-bit each) */
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorRowQ12,anchorColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessVerticalSpan
              ((int)packedDragDeltaXY16 >> 16,(int)(short)packedDragDeltaXY16,anchorRowQ12,
               anchorColumnQ12,accumulatorPlane,fieldGrid);
  }
  else {
    pairRecord = FieldGridEdit_PlayerBlock(playerRuntimeId)->markedCells;
    for (remainingPairCount = FieldGridEdit_PlayerBlock(playerRuntimeId)->markedCellCount;
        remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ProcessVerticalSpan
                ((int)packedDragDeltaXY16 >> 16,(int)(short)packedDragDeltaXY16,pairRecord->pairValue,
                 pairRecord->pairKey,accumulatorPlane,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  /* apply the new offsets; they stay in the plane for the next step */
  cell = firstCell;
  scratchHeightCursor = accumulatorPlane;
  for (remainingCellCount = cellCount; remainingCellCount != 0; remainingCellCount--) {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      cell->terrainHeight = cell->terrainHeight + cellDelta;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - cellDelta;
      FieldGridCell_RefreshChangedCellAndNeighbours(rowStrideBytes,cell,scratchHeightCursor);
    }
    cell++;
    scratchHeightCursor++;
  }
}

/* In-game command INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE (0x2AE0, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor smoothing brush (mode G 0, C 2): smooths the cell at the given grid position (or at every
   selected pair) with FieldGrid_ApplyRectangularTransition, then refreshes normals and light wherever the height
   now differs from the player's scratch plane (filled with the heights by FieldGrid_ResetLocalInfluenceState
   when the stroke began). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 gridRowQ12,Q12 gridColumnQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  uint32_t remainingPairCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *scanCell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  Bool8 containsAnchorPair;
  
  if (FieldGridEdit_PlayerHeightPlane(playerRuntimeId) == nullptr) {
    return;
  }
  playerBlock = FieldGridEdit_PlayerBlock(playerRuntimeId);
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(gridRowQ12,gridColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplyRectangularTransition(gridRowQ12,gridColumnQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->markedCells;
    for (remainingPairCount = playerBlock->markedCellCount; remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ApplyRectangularTransition(pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  scratchHeightCursor = playerBlock->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  scanCell = fieldGrid->cells;
  /* neighbour refresh as in FieldGrid_ApplyPositiveCellDeltas */
  do {
    if ((*scratchHeightCursor != scanCell->terrainHeight) && ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
      FieldGridCell_ComputeDirectionalLightColor(scanCell);
      if (((scanCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell - 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell - 1);
      }
      if (((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell - rowLength; /* row above */
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell + rowLength * 2 - 1; /* row below, one to the left */
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      cell = scanCell + 1;
      if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        FieldGridCell_ComputeDirectionalLightColor(cell);
      }
      scanCell = cell - rowLength; /* back to the changed cell */
    }
    scanCell++;
    scratchHeightCursor++;
    remainingCellCount--;
  } while (remainingCellCount != 0);
}

/* In-game command INGAME_COMMAND_EDITOR_PAINT_MATERIAL (0x2770, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor material brush (mode G 1): writes the material index transitionValue into the cell at the Q12
   grid row/column (or into the cell of every selected pair) with FieldGrid_ApplySingleCellTransition and marks
   the surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 gridRowQ12,
          Q12 gridColumnQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  uint32_t remainingPairCount;
  SelectionPlayerPairRecord *pairRecord;
  Bool8 containsAnchorPair;
  
  playerBlock = FieldGridEdit_PlayerBlock(playerRuntimeId);
  if (playerBlock == nullptr) {
    return;
  }
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(gridRowQ12,gridColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplySingleCellTransition(transitionValue,gridRowQ12,gridColumnQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->markedCells;
    for (remainingPairCount = playerBlock->markedCellCount; remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ApplySingleCellTransition
                (transitionValue,pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}

/* In-game command INGAME_COMMAND_EDITOR_SMOOTH (0x3260, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor water drag (mode G 2, E 0): marks the surface dirty and lets FieldGrid_ApplyEncodedUpdateCore
   move the water level of the cell at the Q12 grid row/column by the vertical drag distance (the signed high 16
   bits of packedDragDeltaXY16). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode. (The command constant's name notwithstanding, nothing
   is smoothed here; FieldGrid_RebuildLocalInfluenceState is the smoothing brush.)
*/
void FieldGrid_ApplyEncodedCellUpdate(PlayerRuntimeId playerRuntimeId,Q12 gridRowQ12,Q12 gridColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  FieldGrid_ApplyEncodedUpdateCore((int)packedDragDeltaXY16 >> 16,gridRowQ12,gridColumnQ12,fieldGrid);
}

/* In-game command INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED (0x32A0, handler at INGAME_COMMAND_CODE_BASE +
   code), terrain-editor flag toggle (mode G 2, E 1): replaces FIELD_CELL_FLUID_RECEIVER_EXCLUDED of the cell at
   the Q12 grid row/column with setMask (0 or the flag, g_UiCommandTerrainMaskToggleValue) and marks the surface
   dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_SetCellFluidReceiverExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  /* The original ORs setMask in unmasked; bounded here to the one flag because setMask comes with the command */
  FieldGrid_ApplyMaskedRegionCore(~FIELD_CELL_FLUID_RECEIVER_EXCLUDED,setMask & FIELD_CELL_FLUID_RECEIVER_EXCLUDED,
                                  gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}

/* In-game command INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED (0x32E0, handler at INGAME_COMMAND_CODE_BASE +
   code), terrain-editor flag toggle (mode G 2, E 2 and up): replaces FIELD_CELL_FLUID_SOURCE_EXCLUDED of the
   cell at the Q12 grid row/column with setMask (0 or the flag, g_UiCommandTerrainMaskToggleValue) and marks the
   surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_SetCellFluidSourceExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  /* The original ORs setMask in unmasked; bounded here to the one flag because setMask comes with the command */
  FieldGrid_ApplyMaskedRegionCore(~FIELD_CELL_FLUID_SOURCE_EXCLUDED,setMask & FIELD_CELL_FLUID_SOURCE_EXCLUDED,
                                  gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}

/* In-game command INGAME_COMMAND_EDITOR_APPLY_REGION_MASK (0x3320, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor resource brush (mode G 5): sets cell flag bit 11 + materialBitIndex (FIELD_CELL_XENITE_SUPPORT
   for 0, FIELD_CELL_TRITIUM_SUPPORT for 1) in the cell at the Q12 grid row/column, or clears it when bit 31 of
   materialBitIndex is set (g_UiCommandCallerMaskHighBit; the shift count only uses the low 5 bits), and marks
   the surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode.
*/
void FieldGrid_SetCellResourceSupportFlag
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 gridRowQ12,
          Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRegionMask setMask;
  uint32_t preserveMask;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  setMask = FIELD_CELL_XENITE_SUPPORT << ((uint8_t)materialBitIndex & 31);
  /* The original takes any bit index 0..31 (others than 0 and 1 replace an unrelated flag, e.g. the edge
     ring); bounded here to the two support flags because the index comes with the command. */
  if ((setMask & ~(FieldGridRegionMask)FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0) {
    static Bool8 s_bitIndexLogged = false;
    if (!s_bitIndexLogged) {
      s_bitIndexLogged = true;
      Thandor_Log("terrain editor: resource flag bit index %d outside 0..1, ignored",(int)materialBitIndex);
    }
    return;
  }
  preserveMask = setMask ^ 0xffffffff;
  if (materialBitIndex < 0) {
    setMask = 0;
  }
  FieldGrid_ApplyMaskedRegionCore(preserveMask,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}

/* In-game command INGAME_COMMAND_EDITOR_CLEAR_SCRATCH (0x1F20, handler at INGAME_COMMAND_CODE_BASE + code):
   zeroes the player's terrain scratch plane (one dword per cell) at the start of a height drag (mode G 0,
   C 0/1), so that FieldGrid_ApplyPositiveCellDeltas / ..NegativeCellDeltas have no previous step to undo. The
   other payload dwords are unused. Called directly or through the command queue by
   InGameUiCommand_BeginInteractionByMode.
*/
void FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  scratchHeightCursor = FieldGridEdit_PlayerHeightPlane(playerRuntimeId);
  if (scratchHeightCursor == nullptr) {
    return;
  }
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  for (cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight; cellsRemaining != 0;
      cellsRemaining--) {
    *scratchHeightCursor = 0;
    scratchHeightCursor++;
  }
}

/* In-game command INGAME_COMMAND_EDITOR_RESET_INFLUENCE (0x2A80, handler at INGAME_COMMAND_CODE_BASE + code):
   copies every cell's terrain height into the player's scratch plane at the start of a smoothing stroke
   (mode G 0, C 2), the reference FieldGrid_RebuildLocalInfluenceState compares against. The other payload
   dwords are unused. Called directly or through the command queue by InGameUiCommand_BeginInteractionByMode.
*/
void FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  scratchHeightCursor = FieldGridEdit_PlayerHeightPlane(playerRuntimeId);
  if (scratchHeightCursor == nullptr) {
    return;
  }
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    *scratchHeightCursor = currentCell->terrainHeight;
    currentCell++;
    scratchHeightCursor++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}

/* Moves the water level of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) by -64 per
   heightDeltaUnits (the vertical drag distance, so dragging up raises it) and refreshes the normals and light of
   the cell and of its six neighbours that are not border cells. Called by FieldGrid_ApplyEncodedCellUpdate.
*/
void FieldGrid_ApplyEncodedUpdateCore(FieldGridHeightDeltaUnits heightDeltaUnits,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnIndex;
  int rowIndex;
  int rowStrideBytes;
  FieldGridCell *cell;
  
  rowLength = fieldGrid->gridWidth;
  columnIndex = gridColumnQ12 >> Q12_SHIFT;
  rowIndex = gridRowQ12 >> Q12_SHIFT;
  if ((-1 < columnIndex) && (-1 < rowIndex) && (columnIndex < (int)rowLength) &&
      (rowIndex < (int)fieldGrid->gridHeight)) {
    rowStrideBytes = rowLength * sizeof(FieldGridCell);
    cell = fieldGrid->cells + columnIndex + (int32_t)(rowIndex * rowLength);
    cell->waterSurfaceDelta = cell->waterSurfaceDelta + heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12; /* 64 per unit */
    /* The original also refreshes an edge-ring cell and its neighbours, which lie outside the grid there;
       bounded here because the cell comes with the editor command (any network peer): the water change stays,
       the refresh is skipped as in FieldGridCell_RefreshChangedCellAndNeighbours. */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      return;
    }
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
    FieldGridCell_ComputeDirectionalLightColor(cell);
    if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell - 1);
      FieldGridCell_ComputeDirectionalLightColor(cell - 1);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
    cell = cell - rowLength; /* row above */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
      FieldGridCell_ComputeDirectionalLightColor(cell);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
    cell = cell + rowLength * 2 - 1; /* row below, one to the left */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
      FieldGridCell_ComputeDirectionalLightColor(cell);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
  }
}

/* Height-drag brush of FieldGrid_ApplyPositiveCellDeltas around one centre cell (Q12 grid row/column
   centerRowQ12/centerColumnQ12): the target height is the source cell's terrainHeight - 64 * heightDeltaUnits;
   every cell of accumulatorPlane (one height per cell) within radius = min(|64 * radiusUnits|, 0x5000) world
   units of the centre cell moves towards it by (1 + cos(pi * distance / (radius + 1))) / 2 of the difference,
   but never away from it (so overlapping brushes keep the strongest pull). The scanned box is the centre +/- 4 *
   radius in grid Q12 coordinates, clipped to the grid.
*/
void FieldGrid_ProcessHorizontalSpan(Q12 sourceRowQ12,Q12 sourceColumnQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerRowQ12,Q12 centerColumnQ12,
          FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int64_t falloffProduct;
  uint32_t spanRadiusQ12;
  int boxMinColumnQ12;
  int boxMinRowQ12;
  int minColumn;
  int minRow;
  int maxColumn;
  int maxRow;
  int spanColumnCount;
  int firstCellIndex;
  int centerCellIndex;
  int centerX;
  int centerY;
  int sourceHeight;
  uint32_t cellDistance;
  int blendedHeight;
  int heightDifference;
  FieldGridCell *spanCell;
  int *accumulatorCursor;
  FieldGridCell *rowStartCell;
  int *rowStartAccumulator;
  int rowsRemaining;
  int columnsLeft;

  spanRadiusQ12 = radiusUnits * FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  }
  if (FIELD_GRID_EDIT_BRUSH_RADIUS_MAX < spanRadiusQ12) {
    spanRadiusQ12 = FIELD_GRID_EDIT_BRUSH_RADIUS_MAX;
  }
  boxMinColumnQ12 = centerColumnQ12 + spanRadiusQ12 * -4;
  boxMinRowQ12 = centerRowQ12 + spanRadiusQ12 * -4;
  maxColumn = (int)(boxMinColumnQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 8) >> Q12_SHIFT;
  maxRow = (int)(boxMinRowQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 8) >> Q12_SHIFT;
  minColumn = boxMinColumnQ12 >> Q12_SHIFT;
  if (minColumn < 0) {
    minColumn = 0;
  }
  minRow = boxMinRowQ12 >> Q12_SHIFT;
  if (minRow < 0) {
    minRow = 0;
  }
  if ((int)fieldGrid->gridWidth <= maxColumn) {
    maxColumn = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRow) {
    maxRow = fieldGrid->gridHeight - 1;
  }
  if (maxColumn < minColumn || maxRow < minRow) {
    return;
  }
  /* The original reads the centre and source cells unchecked; bounded here because both positions come with
     the editor command (any network peer): an index outside the cell array leaves the plane unchanged. */
  if (!FieldGrid_IsCellIndexInGrid(centerRowQ12,centerColumnQ12,fieldGrid) ||
      !FieldGrid_IsCellIndexInGrid(sourceRowQ12,sourceColumnQ12,fieldGrid)) {
    return;
  }
  spanColumnCount = (maxColumn - minColumn) + 1;
  firstCellIndex = minRow * fieldGrid->gridWidth + minColumn;
  rowLength = fieldGrid->gridWidth;
  centerCellIndex = (centerRowQ12 >> Q12_SHIFT) * rowLength + (centerColumnQ12 >> Q12_SHIFT);
  centerX = fieldGrid->cells[centerCellIndex].worldX;
  centerY = fieldGrid->cells[centerCellIndex].worldY;
  sourceHeight = fieldGrid->cells
          [(int32_t)((sourceRowQ12 >> Q12_SHIFT) * fieldGrid->gridWidth + (sourceColumnQ12 >> Q12_SHIFT))].terrainHeight;
  rowStartCell = fieldGrid->cells + firstCellIndex;
  rowStartAccumulator = accumulatorPlane + firstCellIndex;
  for (rowsRemaining = (maxRow - minRow) + 1; rowsRemaining != 0; rowsRemaining--) {
    spanCell = rowStartCell;
    accumulatorCursor = rowStartAccumulator;
    for (columnsLeft = spanColumnCount; columnsLeft != 0; columnsLeft--) {
      cellDistance = FixedMath_Length2(spanCell->worldY - centerY,spanCell->worldX - centerX);
      if (cellDistance <= spanRadiusQ12 + 1) {
        heightDifference = (sourceHeight + heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12) - *accumulatorCursor;
        falloffProduct = (int64_t)
                (g_FixedSineQ28
                 [FIXED_SINE_TABLE_COS +
                  (int)((int64_t)
                        ((((int64_t)(int)cellDistance & FIELD_GRID_ANGLE_PRODUCT_HIGH_BITS_MASK) >> 17) << 32 |
                        (int64_t)(int)cellDistance * FIXED_ANGLE16_HALF_TURN & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                 ] + Q28_ONE) * (int64_t)heightDifference;
        blendedHeight = (FIXED_PRODUCT_SHR(falloffProduct, Q28_SHIFT + 1)) + *accumulatorCursor;
        /* only move towards the target, never back */
        if (heightDifference < 0) {
          if (blendedHeight < *accumulatorCursor) {
            *accumulatorCursor = blendedHeight;
          }
        }
        else if (0 < heightDifference && *accumulatorCursor < blendedHeight) {
          *accumulatorCursor = blendedHeight;
        }
      }
      spanCell++;
      accumulatorCursor++;
    }
    rowStartCell = rowStartCell + rowLength;
    rowStartAccumulator = rowStartAccumulator + rowLength;
  }
}

/* Raise/lower brush of FieldGrid_ApplyNegativeCellDeltas around one centre cell (Q12 grid row/column
   centerRowQ12/centerColumnQ12): adds (1 + cos(pi * distance / (radius + 1))) / 2 * offset, offset = -64 *
   heightDeltaUnits, to every accumulatorPlane entry within radius = min(|64 * radiusUnits|, 0x5000) world units
   of the centre cell, capping the sum at offset (so overlapping brushes do not add up beyond it). The scanned
   box is the centre +/- 2 * radius in grid Q12 coordinates, clipped to the grid.
*/
void FieldGrid_ProcessVerticalSpan(FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerRowQ12,Q12 centerColumnQ12,FieldGridAccumulatorValue *accumulatorPlane,
          FieldGridAsset *fieldGrid)

{
  int targetOffset;
  FieldGridDimension rowLength;
  int64_t falloffProduct;
  uint32_t spanRadiusQ12;
  int boxMinColumnQ12;
  int boxMinRowQ12;
  int minColumn;
  int minRow;
  int maxColumn;
  int maxRow;
  int spanColumnCount;
  int firstCellIndex;
  int centerCellIndex;
  int centerX;
  int centerY;
  uint32_t cellDistance;
  int blendedValue;
  FieldGridCell *spanCell;
  int *accumulatorCursor;
  FieldGridCell *rowStartCell;
  int *rowStartAccumulator;
  int rowsRemaining;
  int columnsLeft;

  spanRadiusQ12 = radiusUnits * FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  }
  if (FIELD_GRID_EDIT_BRUSH_RADIUS_MAX < spanRadiusQ12) {
    spanRadiusQ12 = FIELD_GRID_EDIT_BRUSH_RADIUS_MAX;
  }
  targetOffset = heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  boxMinColumnQ12 = centerColumnQ12 + spanRadiusQ12 * -2;
  boxMinRowQ12 = centerRowQ12 + spanRadiusQ12 * -2;
  maxColumn = (int)(boxMinColumnQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 4) >> Q12_SHIFT;
  maxRow = (int)(boxMinRowQ12 + Q12_FRACTION_MASK + spanRadiusQ12 * 4) >> Q12_SHIFT;
  minColumn = boxMinColumnQ12 >> Q12_SHIFT;
  if (minColumn < 0) {
    minColumn = 0;
  }
  minRow = boxMinRowQ12 >> Q12_SHIFT;
  if (minRow < 0) {
    minRow = 0;
  }
  if ((int)fieldGrid->gridWidth <= maxColumn) {
    maxColumn = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRow) {
    maxRow = fieldGrid->gridHeight - 1;
  }
  if (maxColumn < minColumn || maxRow < minRow) {
    return;
  }
  /* The original reads the centre cell unchecked; bounded here because the position comes with the editor
     command (any network peer): an index outside the cell array leaves the plane unchanged. */
  if (!FieldGrid_IsCellIndexInGrid(centerRowQ12,centerColumnQ12,fieldGrid)) {
    return;
  }
  spanColumnCount = (maxColumn - minColumn) + 1;
  firstCellIndex = minRow * fieldGrid->gridWidth + minColumn;
  rowLength = fieldGrid->gridWidth;
  centerCellIndex = (centerRowQ12 >> Q12_SHIFT) * rowLength + (centerColumnQ12 >> Q12_SHIFT);
  centerX = fieldGrid->cells[centerCellIndex].worldX;
  centerY = fieldGrid->cells[centerCellIndex].worldY;
  rowStartCell = fieldGrid->cells + firstCellIndex;
  rowStartAccumulator = accumulatorPlane + firstCellIndex;
  for (rowsRemaining = (maxRow - minRow) + 1; rowsRemaining != 0; rowsRemaining--) {
    spanCell = rowStartCell;
    accumulatorCursor = rowStartAccumulator;
    for (columnsLeft = spanColumnCount; columnsLeft != 0; columnsLeft--) {
      cellDistance = FixedMath_Length2(spanCell->worldY - centerY,spanCell->worldX - centerX);
      if (cellDistance <= spanRadiusQ12 + 1) {
        falloffProduct = (int64_t)
                (g_FixedSineQ28
                 [FIXED_SINE_TABLE_COS +
                  (int)((int64_t)
                        ((((int64_t)(int)cellDistance & FIELD_GRID_ANGLE_PRODUCT_HIGH_BITS_MASK) >> 17) << 32 |
                        (int64_t)(int)cellDistance * FIXED_ANGLE16_HALF_TURN & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                 ] + Q28_ONE) * (int64_t)targetOffset;
        blendedValue = (FIXED_PRODUCT_SHR(falloffProduct, Q28_SHIFT + 1)) + *accumulatorCursor;
        /* cap the sum at targetOffset */
        if (targetOffset < 0) {
          if (blendedValue < targetOffset) {
            blendedValue = targetOffset;
          }
        }
        else if (targetOffset < blendedValue) {
          blendedValue = targetOffset;
        }
        *accumulatorCursor = blendedValue;
      }
      spanCell++;
      accumulatorCursor++;
    }
    rowStartCell = rowStartCell + rowLength;
    rowStartAccumulator = rowStartAccumulator + rowLength;
  }
}

/* Replaces the material byte of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) with transitionValue,
   when the cell is inside the grid. Called by FieldGrid_ApplyLocalCellUpdate.
*/
void FieldGrid_ApplySingleCellTransition(FieldGridTransitionValue transitionValue,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int gridColumnIndex;
  int cellIndex;

  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  gridColumnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((-1 < gridRowIndex) && (-1 < gridColumnIndex) && (gridRowIndex < (int)fieldGrid->gridHeight) &&
      (gridColumnIndex < (int)fieldGrid->gridWidth)) {
    cellIndex = gridRowIndex * fieldGrid->gridWidth + gridColumnIndex;
    /* The original ORs transitionValue in unmasked; bounded here to the material byte because the value comes
       with the editor command (any network peer) and must not set flags such as the edge ring. */
    fieldGrid->cells[cellIndex].flagsAndMaterial =
         (fieldGrid->cells[cellIndex].flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK) |
         (transitionValue & FIELD_CELL_MATERIAL_ID_MASK);
  }
}

/* Smooths one cell: sets the terrain height of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) to the
   average of its six neighbours (the water level stays); only rows 2..height-2 and columns 2..width-2 are
   touched. Called by FieldGrid_RebuildLocalInfluenceState.
*/
void FieldGrid_ApplyRectangularTransition(Q12 gridRowQ12,Q12 gridColumnQ12,FieldGridAsset *fieldGrid)

{
  Q12 *heightField;
  int gridRowIndex;
  int gridColumnIndex;
  int aboveCellIndex;
  int heightDelta;
  FieldGridDimension gridWidth;

  gridWidth = fieldGrid->gridWidth;
  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  gridColumnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((1 < gridRowIndex) && (1 < gridColumnIndex) && (gridRowIndex + 1 < (int)fieldGrid->gridHeight) &&
      (gridColumnIndex + 1 < (int)gridWidth)) {
    /* index of the neighbour above; the six terms are the neighbours above, above right, left, right, below
       left and below, the centre is cells[aboveCellIndex + gridWidth] */
    aboveCellIndex = (gridRowIndex - 1) * gridWidth + gridColumnIndex;
    heightDelta = (fieldGrid->cells[aboveCellIndex].terrainHeight +
             fieldGrid->cells[aboveCellIndex + 1].terrainHeight +
             fieldGrid->cells[(int32_t)(aboveCellIndex + (gridWidth - 1))].terrainHeight +
             fieldGrid->cells[(int32_t)(aboveCellIndex + gridWidth + 1)].terrainHeight +
             fieldGrid->cells[(int32_t)(aboveCellIndex + gridWidth * 2 - 1)].terrainHeight
            + fieldGrid->cells[(int32_t)(aboveCellIndex + gridWidth * 2)].terrainHeight) / 6 -
            fieldGrid->cells[(int32_t)(aboveCellIndex + gridWidth)].terrainHeight;
    heightField = &fieldGrid->cells[(int32_t)(aboveCellIndex + gridWidth)].terrainHeight;
    *heightField = *heightField + heightDelta;
    heightField = &fieldGrid->cells[(int32_t)(aboveCellIndex + gridWidth)].waterSurfaceDelta;
    *heightField = *heightField - heightDelta;
  }
}

/* Sets the flags of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) to (flags & preserveMask) |
   setMask, when the cell is inside the grid. Shared by FieldGrid_SetCellFluidReceiverExcluded,
   FieldGrid_SetCellFluidSourceExcluded and FieldGrid_SetCellResourceSupportFlag.
*/
void FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int gridColumnIndex;
  int cellIndex;
  uint32_t oldFlags;

  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  gridColumnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((-1 < gridRowIndex) && (-1 < gridColumnIndex) && (gridRowIndex < (int)fieldGrid->gridHeight) &&
      (gridColumnIndex < (int)fieldGrid->gridWidth)) {
    cellIndex = gridRowIndex * fieldGrid->gridWidth + gridColumnIndex;
    /* The original lets the masks change any bit; the edge-ring bits are kept here because the neighbour loops
       rely on them to stay inside the grid (the callers only touch fluid and resource flags). */
    oldFlags = (uint32_t)fieldGrid->cells[cellIndex].flagsAndMaterial;
    fieldGrid->cells[cellIndex].flagsAndMaterial =
         (((preserveMask & oldFlags) | setMask) & ~FIELD_CELL_GRID_EDGE_MASK) | (oldFlags & FIELD_CELL_GRID_EDGE_MASK);
  }
}
