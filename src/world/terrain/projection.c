/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/projection.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/projection.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* per-row visible column spans of the terrain projection; entries 0..258 start zeroed, entry 259 keeps the
   0x90 fill bytes the original image held there */
static TerrainProjectedRowSpan g_TerrainProjectedRowSpans[260] = {
    /*   0 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  10 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  20 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  30 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  40 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  50 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  60 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  70 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  80 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /*  90 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 100 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 110 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 120 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 130 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 140 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 150 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 160 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 170 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 180 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 190 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 200 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 210 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 220 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 230 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 240 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 250 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    /* 259 */ {.firstColumn = (int)0x90909090, .endColumnExclusive = (int)0x90909090}};

/* Q12 height-delta scale per scan step: entry n = 4 / (n + 4) in Q12, rounded to nearest
   ((16384 + (n + 4) / 2) / (n + 4); checked against every entry) */
static const int32_t g_TerrainHeightDeltaScaleByStepQ12[256] = {
    /*   0 */ 4096, 3277, 2731, 2341, 2048, 1820, 1638, 1489, 1365, 1260, 1170, 1092, 1024, 964, 910, 862,
    /*  16 */ 819, 780, 745, 712, 683, 655, 630, 607, 585, 565, 546, 529, 512, 496, 482, 468,
    /*  32 */ 455, 443, 431, 420, 410, 400, 390, 381, 372, 364, 356, 349, 341, 334, 328, 321,
    /*  48 */ 315, 309, 303, 298, 293, 287, 282, 278, 273, 269, 264, 260, 256, 252, 248, 245,
    /*  64 */ 241, 237, 234, 231, 228, 224, 221, 218, 216, 213, 210, 207, 205, 202, 200, 197,
    /*  80 */ 195, 193, 191, 188, 186, 184, 182, 180, 178, 176, 174, 172, 171, 169, 167, 165,
    /*  96 */ 164, 162, 161, 159, 158, 156, 155, 153, 152, 150, 149, 148, 146, 145, 144, 142,
    /* 112 */ 141, 140, 139, 138, 137, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125,
    /* 128 */ 124, 123, 122, 121, 120, 120, 119, 118, 117, 116, 115, 115, 114, 113, 112, 111,
    /* 144 */ 111, 110, 109, 109, 108, 107, 106, 106, 105, 104, 104, 103, 102, 102, 101, 101,
    /* 160 */ 100, 99, 99, 98, 98, 97, 96, 96, 95, 95, 94, 94, 93, 93, 92, 92,
    /* 176 */ 91, 91, 90, 90, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84,
    /* 192 */ 84, 83, 83, 82, 82, 82, 81, 81, 80, 80, 80, 79, 79, 78, 78, 78,
    /* 208 */ 77, 77, 77, 76, 76, 76, 75, 75, 74, 74, 74, 73, 73, 73, 72, 72,
    /* 224 */ 72, 72, 71, 71, 71, 70, 70, 70, 69, 69, 69, 69, 68, 68, 68, 67,
    /* 240 */ 67, 67, 67, 66, 66, 66, 66, 65, 65, 65, 65, 64, 64, 64, 64, 63};

uint32_t g_TerrainScanRowStrideBytes = 0;

uint32_t g_TerrainScanStepLimit = 0;

TerrainScanSelectorUnion g_TerrainScanSharedSelectorValue = {0};

uint32_t g_TerrainScanReferenceHeight = 0;

/* Implementation ownership: world/terrain/projection. */

/* Not a function of its own in the original: PUNPCKLBW mm,mm then PSRLW mm,shift, i.e. the four bytes b of value
   as the words ((b << 8) | b) >> shift (the MMX colour unpack of the terrain shading). */
static __inline uint64_t TerrainProjection_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
  return lanes.q;
}

/* Not a function of its own in the original: PACKUSWB mm,mm (low dword), the four signed words saturated to
   unsigned bytes. */
static __inline uint32_t TerrainProjection_PackWordsUnsignedSaturate(uint64_t words)

{
  ThandorMmx lanes;
  uint32_t packed;
  int lane;

  lanes.q = words;
  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    packed = packed |
             (uint32_t)(lanes.sw[lane] < 0 ? 0 : (0xff < lanes.sw[lane] ? 0xff : lanes.sw[lane])) << (lane * 8);
  }
  return packed;
}

/* Shared start of the three world-point scans below: sets g_TerrainScanStepLimit from the radius (one step per
   TERRAIN_SCAN_RADIUS_PER_STEP world units, at least 1, at most TERRAIN_SCAN_STEP_LIMIT_MAX). */
static void TerrainProjectedScan_SetStepLimitFromRadius(FieldGridRadiusUnits radiusWorldUnits)

{
  g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
  if (g_TerrainScanStepLimit == 0) {
    g_TerrainScanStepLimit = 1;
  }
  else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
    g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
  }
}

/* Shared by the three world-point scans below: picks the vertex of the triangulated grid cell nearest to the Q12
   grid position from its Q12 fractions (0x1000 = one cell). */
static void TerrainProjectedScan_SelectNearestGridVertex
          (FieldGridCoordinates gridCoordinates,uint32_t *gridRowOut,uint32_t *gridColumnOut)

{
  uint32_t baseColumn;
  uint32_t columnFraction;
  uint32_t rowFraction;
  uint32_t fractionSum;
  uint32_t gridRow;
  uint32_t gridColumn;

  baseColumn = gridCoordinates.columnQ12 >> 12;
  gridRow = gridCoordinates.rowQ12 >> 12;
  columnFraction = (uint32_t)(gridCoordinates.columnQ12 & Q12_FRACTION_MASK);
  rowFraction = (uint32_t)(gridCoordinates.rowQ12 & Q12_FRACTION_MASK);
  fractionSum = rowFraction + columnFraction * 2;
  gridColumn = baseColumn;
  if (fractionSum < FIELD_GRID_CELL_Q12) {
    if (FIELD_GRID_CELL_Q12 - 1 < columnFraction + rowFraction * 2) {
      gridRow++;
    }
  }
  else if (fractionSum < FIELD_GRID_TWO_CELLS_Q12 + 1) {
    gridColumn = baseColumn + 1;
    if (columnFraction < rowFraction) {
      gridRow++;
      gridColumn = baseColumn;
    }
  }
  else {
    gridColumn = baseColumn + 1;
    if (FIELD_GRID_TWO_CELLS_Q12 - 1 < columnFraction + rowFraction * 2) {
      gridRow++;
    }
  }
  *gridRowOut = gridRow;
  *gridColumnOut = gridColumn;
}

/* Line-of-sight marking for one army (occupancy rebuild): from the grid vertex nearest to the world point, ORs
   occupancyMaskBits (the bits of the factions that share the army's sight) into every cell the terrain does not
   hide from an eye at referenceHeightQ12 within the radius. The centre cell is marked here, the rest by the six
   sector traces, each seeded with the height of the next sector's first cell. Nothing happens outside the grid or
   on a map-edge cell.
*/
void TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (uint64_t occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int rowStrideBytes;
  int referenceHeight;
  uint32_t gridWidth;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCoordinates gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != NULL) {
    TerrainProjectedScan_SetStepLimitFromRadius(radiusWorldUnits);
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    referenceHeight = g_TerrainScanReferenceHeight;
    TerrainProjectedScan_SelectNearestGridVertex(gridCoordinates,&gridRow,&gridColumn);
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    gridWidth = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
    if ((-1 < (int)gridColumn) && (-1 < (int)gridRow) && (gridRow < fieldGrid->gridHeight) &&
        (gridColumn < gridWidth)) {
      centerCellIndex = gridRow * gridWidth + gridColumn;
      if ((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        fieldGrid->cells[centerCellIndex].occupancyMask =
             fieldGrid->cells[centerCellIndex].occupancyMask | occupancyMaskBits;
        rowStrideBytes = g_TerrainScanRowStrideBytes;
        /* the six neighbours of centre cell C, one per sector: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width;
           the first address is cells[centerCellIndex + 1]) */
        wedgeCellA = &fieldGrid->cells[centerCellIndex + 1];
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,-g_TerrainScanRowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge0
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge1
                  (occupancyMaskBits,wedgeCellB[-1].terrainHeight - referenceHeight,0,wedgeCellB);
        wedgeCellA = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellB - 2,rowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge2
                  (occupancyMaskBits,wedgeCellA->terrainHeight - referenceHeight,0,wedgeCellB - 1);
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,rowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge3
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge4
                  (occupancyMaskBits,wedgeCellB[1].terrainHeight - referenceHeight,0,wedgeCellB);
        /* the terrain height of C+1 (one row up from C+W+1), the first cell of sector 0 */
        TerrainProjectedOcclusion_TraceWedge5
                  (occupancyMaskBits,
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellB + 2,-rowStrideBytes)->terrainHeight - referenceHeight,0,
                   wedgeCellB + 1);
      }
    }
  }
}


/* Terrain-class overlay callback for land classes (g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
   slots 0, 2, 3 and 4, called by WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries): stores cellValue into
   overlayColor of every cell within the radius around the world point that has a bit of
   cellFlagMask and no water above it; the centre cell here, the rest by the six sector walks (the same hexagon
   as TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint, without line of sight). Marks the field grid
   surface dirty. Returns true (nothing applied) without a grid, outside it or on a map-edge cell.
*/
Bool8 FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  int rowStrideBytes;
  uint32_t gridWidth;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *fieldCell;
  FieldGridCoordinates gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != NULL) {
    TerrainProjectedScan_SetStepLimitFromRadius(radiusWorldUnits);
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    TerrainProjectedScan_SelectNearestGridVertex(gridCoordinates,&gridRow,&gridColumn);
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    gridWidth = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
    if ((-1 < (int)gridColumn) && (-1 < (int)gridRow) && (gridRow < fieldGrid->gridHeight) &&
        (gridColumn < gridWidth)) {
      centerCellIndex = gridRow * gridWidth + gridColumn;
      if ((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        if (((fieldGrid->cells[centerCellIndex].flagsAndMaterial & cellFlagMask) != 0) &&
           (fieldGrid->cells[centerCellIndex].waterSurfaceDelta < 0)) {
          fieldGrid->cells[centerCellIndex].overlayColor = cellValue;
        }
        rowStrideBytes = g_TerrainScanRowStrideBytes;
        /* the first cell of each sector walk: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width; the first
           address is cells[centerCellIndex + 1]) */
        wedgeCellA = &fieldGrid->cells[centerCellIndex + 1];
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,-g_TerrainScanRowStrideBytes);
        FieldGridTerrainOverlayVariantA_ApplyWedge0(0,wedgeCellA);
        fieldCell = wedgeCellB - 1;
        FieldGridTerrainOverlayVariantA_ApplyWedge1(0,wedgeCellB);
        wedgeCellA = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,rowStrideBytes);
        FieldGridTerrainOverlayVariantA_ApplyWedge2(0,fieldCell);
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,rowStrideBytes);
        FieldGridTerrainOverlayVariantA_ApplyWedge3(0,wedgeCellA);
        FieldGridTerrainOverlayVariantA_ApplyWedge4(0,wedgeCellB);
        FieldGridTerrainOverlayVariantA_ApplyWedge5(0,wedgeCellB + 1);
        return false;
      }
    }
  }
  return true;
}


/* Terrain-class overlay callback for the water class (g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
   slot 1, called by WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries): like
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, but only for cells with water above them; the centre
   cell also needs a bit of cellFlagMask, the sector walks ignore the mask.
*/
Bool8 FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  int rowStrideBytes;
  uint32_t gridWidth;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *fieldCell;
  FieldGridCoordinates gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != NULL) {
    TerrainProjectedScan_SetStepLimitFromRadius(radiusWorldUnits);
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    TerrainProjectedScan_SelectNearestGridVertex(gridCoordinates,&gridRow,&gridColumn);
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    gridWidth = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
    if ((-1 < (int)gridColumn) && (-1 < (int)gridRow) && (gridRow < fieldGrid->gridHeight) &&
        (gridColumn < gridWidth)) {
      centerCellIndex = gridRow * gridWidth + gridColumn;
      if ((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        if (((fieldGrid->cells[centerCellIndex].flagsAndMaterial & cellFlagMask) != 0) &&
           (0 < fieldGrid->cells[centerCellIndex].waterSurfaceDelta)) {
          fieldGrid->cells[centerCellIndex].overlayColor = cellValue;
        }
        rowStrideBytes = g_TerrainScanRowStrideBytes;
        /* the first cell of each sector walk: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width; the first
           address is cells[centerCellIndex + 1]) */
        wedgeCellA = &fieldGrid->cells[centerCellIndex + 1];
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,-g_TerrainScanRowStrideBytes);
        FieldGridTerrainOverlayVariantB_ApplyWedge0(0,wedgeCellA);
        fieldCell = wedgeCellB - 1;
        FieldGridTerrainOverlayVariantB_ApplyWedge1(0,wedgeCellB);
        wedgeCellA = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,rowStrideBytes);
        FieldGridTerrainOverlayVariantB_ApplyWedge2(0,fieldCell);
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,rowStrideBytes);
        FieldGridTerrainOverlayVariantB_ApplyWedge3(0,wedgeCellA);
        FieldGridTerrainOverlayVariantB_ApplyWedge4(0,wedgeCellB);
        FieldGridTerrainOverlayVariantB_ApplyWedge5(0,wedgeCellB + 1);
        return false;
      }
    }
  }
  return true;
}


/* Terrain pass of the world view (called by FrontendModelPointerContext_RenderWorldViewQueuesClipped): unless
   the previous projection can be reused (TERRAIN_RENDER_REUSE_PROJECTION), rebuilds the visible column span of
   every grid row from the four frustum side planes, marks all vertices as not projected and widens each span to
   cover its neighbour rows. Then projects and shades the vertices inside the spans, fully (VariantA) when the
   grid surface changed or the projection is not reusable, else only the parts that can change (VariantB), and
   queues every grid quad between two rows of the spans as two triangles.
*/
void TerrainProjectedGrid_TransformShadeAndQueue
          (FieldGridAsset *fieldGrid,FrontendModelPointerContext *renderContext)

{
  FieldGridDimension gridWidth;
  int spanFirstColumn;
  int spanEndColumn;
  int previousFirstColumn;
  int previousEndColumn;
  int vertexCount;
  int quadCount;
  int quadRowsLeft;
  FieldGridDimension rowCount;
  FieldGridDimension rowsRemaining;
  FieldGridDimension columnsRemaining;
  FieldGridCell *rowCells;
  TerrainProjectedVertexWorkRecord *vertexCursor;
  TerrainProjectedRowSpan *rowSpan;
  int spanPairsLeft;

  if ((renderContext->contextFlags & TERRAIN_RENDER_REUSE_PROJECTION) == 0) {
    rowSpan = g_TerrainProjectedRowSpans;
    gridWidth = fieldGrid->gridWidth;
    rowCount = fieldGrid->gridHeight;
    do {
      rowSpan->firstColumn = 0;
      rowSpan->endColumnExclusive = gridWidth;
      rowSpan = rowSpan + 1;
      rowCount--;
    } while (rowCount != 0);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0 + 1);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0 + 2);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0 + 3);
    gridWidth = fieldGrid->gridWidth;
    rowCount = fieldGrid->gridHeight;
    rowCells = fieldGrid->cells;
    rowsRemaining = rowCount;
    columnsRemaining = gridWidth;
    do {
      do {
        rowCells->flagsAndMaterial =
             rowCells->flagsAndMaterial | (TERRAIN_VERTEX_POINT_A_NOT_PROJECTED | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED);
        rowCells = rowCells + 1;
        columnsRemaining--;
      } while (columnsRemaining != 0);
      rowsRemaining--;
      columnsRemaining = gridWidth;
    } while (rowsRemaining != 0);
    /* widen each span to its neighbour row's, so every quad between two rows has all four vertices */
    rowSpan = g_TerrainProjectedRowSpans;
    spanPairsLeft = rowCount - 1;
    previousFirstColumn = g_TerrainProjectedRowSpans[0].firstColumn;
    previousEndColumn = g_TerrainProjectedRowSpans[0].endColumnExclusive;
    do {
      rowSpan = rowSpan + 1;
      spanFirstColumn = rowSpan->firstColumn;
      spanEndColumn = rowSpan->endColumnExclusive;
      if (previousEndColumn == 0) {
        rowSpan[-1].firstColumn = spanFirstColumn;
        rowSpan[-1].endColumnExclusive = spanEndColumn;
      }
      else if (spanEndColumn == 0) {
        rowSpan->firstColumn = previousFirstColumn;
        rowSpan->endColumnExclusive = previousEndColumn;
      }
      else {
        if (previousFirstColumn < spanFirstColumn) {
          rowSpan->firstColumn = previousFirstColumn;
        }
        else if (spanFirstColumn < rowSpan[-1].firstColumn) {
          rowSpan[-1].firstColumn = spanFirstColumn;
        }
        if (spanEndColumn < previousEndColumn) {
          rowSpan->endColumnExclusive = previousEndColumn;
        }
        else if (rowSpan[-1].endColumnExclusive < spanEndColumn) {
          rowSpan[-1].endColumnExclusive = spanEndColumn;
        }
      }
      spanPairsLeft = spanPairsLeft - 1;
      /* the next pair compares against this row's spans as read, before any widening */
      previousFirstColumn = spanFirstColumn;
      previousEndColumn = spanEndColumn;
    } while (spanPairsLeft != 0);
  }
  rowSpan = g_TerrainProjectedRowSpans;
  gridWidth = fieldGrid->gridWidth;
  rowCount = fieldGrid->gridHeight;
  if (((fieldGrid->runtimeStateFlags & FIELD_GRID_RUNTIME_SURFACE_DIRTY) == 0) &&
     ((renderContext->contextFlags & TERRAIN_RENDER_REUSE_PROJECTION) != 0)) {
    rowCells = fieldGrid->cells;
    do {
      spanFirstColumn = rowSpan->firstColumn;
      vertexCount = rowSpan->endColumnExclusive - spanFirstColumn;
      if (vertexCount != 0 && spanFirstColumn <= rowSpan->endColumnExclusive) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + spanFirstColumn);
        do {
          TerrainProjectedVertex_ReshadeKeepingProjection(vertexCursor);
          vertexCursor = vertexCursor + 1;
          vertexCount = vertexCount - 1;
        } while (vertexCount != 0);
      }
      rowSpan = rowSpan + 1;
      rowCells = rowCells + gridWidth;
      rowCount--;
    } while (rowCount != 0);
  }
  else {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags & ~FIELD_GRID_RUNTIME_SURFACE_DIRTY;
    renderContext->contextFlags = renderContext->contextFlags & ~TERRAIN_RENDER_REUSE_PROJECTION;
    rowCells = fieldGrid->cells;
    do {
      spanFirstColumn = rowSpan->firstColumn;
      vertexCount = rowSpan->endColumnExclusive - spanFirstColumn;
      if (vertexCount != 0 && spanFirstColumn <= rowSpan->endColumnExclusive) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + spanFirstColumn);
        do {
          TerrainProjectedVertex_TransformProjectAndShade(vertexCursor);
          vertexCursor = vertexCursor + 1;
          vertexCount = vertexCount - 1;
        } while (vertexCount != 0);
      }
      rowSpan = rowSpan + 1;
      rowCells = rowCells + gridWidth;
      rowCount--;
    } while (rowCount != 0);
  }
  gridWidth = fieldGrid->gridWidth;
  rowCells = fieldGrid->cells;
  rowSpan = g_TerrainProjectedRowSpans;
  /* quads: rows 0..height-2, columns firstColumn..endColumnExclusive-2 */
  quadRowsLeft = fieldGrid->gridHeight - 1;
  do {
    spanFirstColumn = rowSpan->firstColumn;
    vertexCount = rowSpan->endColumnExclusive - spanFirstColumn;
    if (vertexCount != 0 && spanFirstColumn <= rowSpan->endColumnExclusive) {
      quadCount = vertexCount - 1;
      if (quadCount != 0) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + spanFirstColumn);
        do {
          TerrainProjectedQuad_QueueAsTwoTriangles(gridWidth * sizeof(FieldGridCell),vertexCursor,renderContext);
          vertexCursor = vertexCursor + 1;
          quadCount = quadCount - 1;
        } while (quadCount != 0);
      }
    }
    rowSpan = rowSpan + 1;
    rowCells = rowCells + gridWidth;
    quadRowsLeft = quadRowsLeft - 1;
  } while (quadRowsLeft != 0);
  return;
}


/* Line-of-sight marking for the sector between directions 0 (C+1) and 1 (C+1-W) of
   TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint. Walks the sector's spine (step C+2-W, scan step +7)
   with a running horizon like TerrainProjectedOcclusion_ScanDirection0, tests the direction-1 neighbour between
   two spine cells, and hands the current horizon to a straight leg along each bounding direction. The first spine
   cell is tested against its own unscaled height above the eye; unless it is visible, the legs start from the
   average of that height and the caller's projectedHeightThresholdQ20.
*/
void TerrainProjectedOcclusion_TraceWedge0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  int rowStrideBytes;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = cell + 1;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,adjacentCell);
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-1 neighbour C+1-W */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,-rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      cell = neighborCell + 1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Line-of-sight marking for the sector between directions 1 (C+1-W) and 2 (C-W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C+1-2W (scan step +7), the direction-2 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  int rowStrideBytes;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-2 neighbour C-W */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      adjacentCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = adjacentCell + 1;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,adjacentCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Line-of-sight marking for the sector between directions 2 (C-W) and 3 (C-1), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-1-W (scan step +7), the direction-3 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  int64_t scaledHeightProduct;
  int cellHeight;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-3 neighbour C-1 */
      if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = cell[-1].terrainHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        neighborHeight = neighborHeight + cell[-1].waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        cell[-1].occupancyMask = cell[-1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      adjacentCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes);
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,adjacentCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Line-of-sight marking for the sector between directions 3 (C-1) and 4 (C-1+W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-2+W (scan step +7), the direction-4 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int rowStrideBytes;
  FieldCellPersistedAux cellHeight;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(int)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = cell - 1;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,adjacentCell);
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-4 neighbour C-1+W, one row stride on */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      /* C-2+W */
      cell = neighborCell - 1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Line-of-sight marking for the sector between directions 4 (C-1+W) and 5 (C+W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-1+2W (scan step +7), the direction-5 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  int rowStrideBytes;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *neighborCell;
  FieldGridCell *twoRowsDownCell;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-5 neighbour C+W, one row stride on */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = neighborCell->terrainHeight;
      if (0 < neighborCell->waterSurfaceDelta) {
        neighborHeight = neighborHeight + neighborCell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        neighborCell->occupancyMask = neighborCell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      /* two rows down: C+2W, the spine continues at C-1+2W */
      twoRowsDownCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(neighborCell,g_TerrainScanRowStrideBytes);
      cell = twoRowsDownCell - 1;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,twoRowsDownCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Line-of-sight marking for the sector between directions 5 (C+W) and 0 (C+1), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C+1+W (scan step +7), the direction-0 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void TerrainProjectedOcclusion_TraceWedge5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  int64_t scaledHeightProduct;
  int cellHeight;
  int neighborHeight;
  uint32_t projectedHeight;
  TerrainDirectionalScanStep legStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;

  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)cellThreshold <= (int)projectedHeight) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      legStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,legStep,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= legStep) {
        return;
      }
      /* the direction-0 neighbour C+1 */
      if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = cell[1].terrainHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        neighborHeight = neighborHeight + cell[1].waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[legStep];
      projectedHeight = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeight) {
        cell[1].occupancyMask = cell[1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeight;
      }
      adjacentCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes);
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,adjacentCell);
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 0 (C+1, right) and 1 (C+1-W, up and right) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge0
   but without a horizon: spine step C+2-W (scan step +7), each spine cell and the direction-1 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;
  int rowStrideBytes;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row up */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,-rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((neighborCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (neighborCell->waterSurfaceDelta < 0)) {
        neighborCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      fieldCell = neighborCell + 1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 1 (C+1-W, up and right) and 2 (C-W, up) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge1
   but without a horizon: spine step C+1-2W (scan step +7), each spine cell and the direction-2 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;
  int rowStrideBytes;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell + 1,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row up */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((neighborCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (neighborCell->waterSurfaceDelta < 0)) {
        neighborCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      adjacentCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = adjacentCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection2(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 2 (C-W, up) and 3 (C-1, left) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge2
   but without a horizon: spine step C-1-W (scan step +7), each spine cell and the direction-3 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection2
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell[-1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) !=
           0) && (fieldCell[-1].waterSurfaceDelta < 0)) {
        fieldCell[-1].overlayColor = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,-g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 3 (C-1, left) and 4 (C-1+W, down and left) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge3
   but without a horizon: spine step C-2+W (scan step +7), each spine cell and the direction-4 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  FieldGridCell *neighborCell;
  int rowStrideBytes;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell - 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row down */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((neighborCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (neighborCell->waterSurfaceDelta < 0)) {
        neighborCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      fieldCell = neighborCell - 1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 4 (C-1+W, down and left) and 5 (C+W, down) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge4
   but without a horizon: spine step C-1+2W (scan step +7), each spine cell and the direction-5 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *neighborCell;
  FieldGridCell *twoRowsDownCell;
  int rowStrideBytes;

  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row down */
      neighborCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,rowStrideBytes);
      if ((neighborCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((neighborCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (neighborCell->waterSurfaceDelta < 0)) {
        neighborCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      /* two rows down: C+2W, the spine continues at C-1+2W */
      twoRowsDownCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(neighborCell,g_TerrainScanRowStrideBytes);
      fieldCell = twoRowsDownCell - 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection5(scanStep,twoRowsDownCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 5 (C+W, down) and 0 (C+1, right) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge5
   but without a horizon: spine step C+1+W (scan step +7), each spine cell and the direction-0 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell[1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0
          ) && (fieldCell[1].waterSurfaceDelta < 0)) {
        fieldCell[1].overlayColor = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell + 1,g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 0 (C+1, right) and 1 (C+1-W, up and right) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge0
   but without a horizon: spine step C+2-W (scan step +7), each spine cell and the direction-1 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row up */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,-rowStrideBytes)->waterSurfaceDelta) {
        FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,-rowStrideBytes)->overlayColor = g_TerrainScanReferenceHeight;
      }
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell + 1,-rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 1 (C+1-W, up and right) and 2 (C-W, up) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge1
   but without a horizon: spine step C+1-2W (scan step +7), each spine cell and the direction-2 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell + 1,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row up */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-rowStrideBytes)->waterSurfaceDelta) {
        FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-rowStrideBytes)->overlayColor = g_TerrainScanReferenceHeight;
      }
      adjacentCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = adjacentCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection2(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 2 (C-W, up) and 3 (C-1, left) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge2
   but without a horizon: spine step C-1-W (scan step +7), each spine cell and the direction-3 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection2
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell[-1].waterSurfaceDelta) {
        fieldCell[-1].overlayColor = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,-g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 3 (C-1, left) and 4 (C-1+W, down and left) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge3
   but without a horizon: spine step C-2+W (scan step +7), each spine cell and the direction-4 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  int scanRowStrideBytes;
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanRowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell - 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row down */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,scanRowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,scanRowStrideBytes)->waterSurfaceDelta) {
        FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell,scanRowStrideBytes)->overlayColor = g_TerrainScanReferenceHeight;
      }
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(adjacentCell - 1,scanRowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 4 (C-1+W, down and left) and 5 (C+W, down) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge4
   but without a horizon: spine step C-1+2W (scan step +7), each spine cell and the direction-5 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint8_t *currentCellBytes;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the neighbour one row down */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,rowStrideBytes)->waterSurfaceDelta) {
        FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,rowStrideBytes)->overlayColor = g_TerrainScanReferenceHeight;
      }
      currentCellBytes = (uint8_t *)fieldCell;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = (FieldGridCell *)
                  (currentCellBytes + g_TerrainScanRowStrideBytes + rowStrideBytes) - 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellBytes + g_TerrainScanRowStrideBytes + rowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Overlay sector between directions 5 (C+W, down) and 0 (C+1, right) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge5
   but without a horizon: spine step C+1+W (scan step +7), each spine cell and the direction-0 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell[1].waterSurfaceDelta) {
        fieldCell[1].overlayColor = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell + 1,g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Queues the grid quad whose top-left vertex is topLeftVertex as two triangles, (top-left, bottom-left,
   top-right) and (bottom-left, bottom-right, top-right) as vertex0..2, both with the top-left vertex's
   secondary-surface packet; rowStrideBytes is one grid row of vertex records. Called for every quad of the
   visible spans by TerrainProjectedGrid_TransformShadeAndQueue.
*/
void TerrainProjectedQuad_QueueAsTwoTriangles
          (uint32_t rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContext *renderContext)

{
  /* + rowStrideBytes: the vertex one row down */
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             (TerrainProjectedVertexWorkRecord *)((uint8_t *)topLeftVertex + rowStrideBytes),topLeftVertex,
             renderContext);
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             (TerrainProjectedVertexWorkRecord *)((uint8_t *)(topLeftVertex + 1) + rowStrideBytes),
             (TerrainProjectedVertexWorkRecord *)((uint8_t *)topLeftVertex + rowStrideBytes),renderContext);
  return;
}


/* Shading of one terrain vertex colour (shared by the two vertex updates below): vertexColor PUNPCKLBW/PSRLW 6,
   lit by the dynamic lights at viewPoint when the vertex's lighting selector is FIELD_CELL_LIGHTING_VISIBLE, PMULHW
   by the vertex's base colour PUNPCKLBW/PSRLW 2, PACKUSWB. */
static PackedArgb32 TerrainProjectedVertex_ShadeColor
          (TerrainProjectedVertexWorkRecord *vertex,PackedArgb32 vertexColor,GraphicsFixedVec3 *viewPoint)

{
  PackedArgb32 baseColor;
  MmxPackedValue64 lightingFactors;
  uint64_t shadedProduct;

  baseColor = vertex->basePackedColor;
  lightingFactors = TerrainProjection_UnpackBytesShiftRight(vertexColor,6);
  if (vertex->lightingLookupIndexOrSentinel == FIELD_CELL_LIGHTING_VISIBLE) {
    lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPoint(viewPoint,lightingFactors);
  }
  shadedProduct = pmulhw(lightingFactors,TerrainProjection_UnpackBytesShiftRight(baseColor,2));
  return TerrainProjection_PackWordsUnsignedSaturate(shadedProduct);
}

/* Point B of a terrain vertex (shared by the two vertex updates below): point B = terrain point + secondaryOffset
   + (0, 0, secondaryProjectionDepthQ12), built in sourcePoint and undone after the transform into viewPointB.
   When it lies beyond the near plane it is projected into projectedPointB. Returns flagsWithoutPointB (point-B
   bits clear) with the point-B clip side bits, or with TERRAIN_VERTEX_POINT_B_NOT_PROJECTED. */
static uint32_t TerrainProjectedVertex_TransformAndProjectPointB
          (TerrainProjectedVertexWorkRecord *vertex,uint32_t flagsWithoutPointB)

{
  GraphicsFixedVec3 *offsetVector;
  int offsetX;
  int offsetY;
  int offsetZ;
  uint32_t resultFlags;
  GraphicsProjectedPointPair projectedPoint;

  offsetVector = vertex->secondaryOffset;
  resultFlags = flagsWithoutPointB | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED;
  offsetX = offsetVector->x;
  offsetY = offsetVector->y;
  offsetZ = offsetVector->z + vertex->secondaryProjectionDepthQ12;
  (vertex->sourcePoint).x = (vertex->sourcePoint).x + offsetX;
  (vertex->sourcePoint).y = (vertex->sourcePoint).y + offsetY;
  (vertex->sourcePoint).z = (vertex->sourcePoint).z + offsetZ;
  FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed);
  (vertex->sourcePoint).x = (vertex->sourcePoint).x - offsetX;
  (vertex->sourcePoint).y = (vertex->sourcePoint).y - offsetY;
  (vertex->sourcePoint).z = (vertex->sourcePoint).z - offsetZ;
  if ((int)g_ProjectionScaleFixed < (vertex->viewPointB).z) {
    projectedPoint = Graphics_ProjectViewPoint(&vertex->viewPointB);
    vertex->projectedPointB = projectedPoint;
    resultFlags = flagsWithoutPointB;
    if (g_ProjectionClipRect.minX <= projectedPoint.projectedX) {
      resultFlags = resultFlags | TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X;
    }
    if (projectedPoint.projectedX < g_ProjectionClipRect.maxX) {
      resultFlags = resultFlags | TERRAIN_VERTEX_POINT_B_INSIDE_MAX_X;
    }
    if (g_ProjectionClipRect.minY <= projectedPoint.projectedY) {
      resultFlags = resultFlags | TERRAIN_VERTEX_POINT_B_INSIDE_MIN_Y;
    }
    if (projectedPoint.projectedY < g_ProjectionClipRect.maxY) {
      resultFlags = resultFlags | TERRAIN_VERTEX_POINT_B_INSIDE_MAX_Y;
    }
  }
  return resultFlags;
}

/* Full per-frame update of one terrain vertex (a field cell): transforms the terrain point to view space,
   projects it when it lies beyond the near plane and records on which sides of the clip rectangle it lies,
   and shades its colour with the base colour (plus the dynamic lights when lightingLookupIndexOrSentinel is
   0xFF). Does the same for point B, the secondary surface point (terrain point + secondaryOffset, raised by
   secondaryProjectionDepthQ12). Vertices without terrain (material 0xFF) are skipped.
*/
void TerrainProjectedVertex_TransformProjectAndShade(TerrainProjectedVertexWorkRecord *vertex)

{
  uint32_t keptFlags;
  uint32_t pointAFlags;
  uint32_t resultFlags;
  PackedArgb32 shadedColorB;
  GraphicsProjectedPointPair projectedPoint;

  /* keeps the material and bits 8..16, 27 and 29..31 (0xe801ffff); the clip bits and SECONDARY_VISIBLE start
     cleared */
  keptFlags = vertex->projectionFlags &
              ~(TERRAIN_VERTEX_POINT_A_BITS | TERRAIN_VERTEX_POINT_B_BITS | TERRAIN_VERTEX_SECONDARY_VISIBLE);
  if ((vertex->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) != TERRAIN_VERTEX_MATERIAL_NONE) {
    pointAFlags = keptFlags | TERRAIN_VERTEX_POINT_A_NOT_PROJECTED;
    FixedTransform_ApplyPoint(&vertex->viewPointA,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed);
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointA).z) {
      projectedPoint = Graphics_ProjectViewPoint(&vertex->viewPointA);
      vertex->projectedPointA = projectedPoint;
      pointAFlags = keptFlags;
      if (g_ProjectionClipRect.minX <= projectedPoint.projectedX) {
        pointAFlags = keptFlags | TERRAIN_VERTEX_POINT_A_INSIDE_MIN_X;
      }
      if (projectedPoint.projectedX < g_ProjectionClipRect.maxX) {
        pointAFlags = pointAFlags | TERRAIN_VERTEX_POINT_A_INSIDE_MAX_X;
      }
      if (g_ProjectionClipRect.minY <= projectedPoint.projectedY) {
        pointAFlags = pointAFlags | TERRAIN_VERTEX_POINT_A_INSIDE_MIN_Y;
      }
      if (projectedPoint.projectedY < g_ProjectionClipRect.maxY) {
        pointAFlags = pointAFlags | TERRAIN_VERTEX_POINT_A_INSIDE_MAX_Y;
      }
    }
    vertex->shadedColorA = TerrainProjectedVertex_ShadeColor(vertex,vertex->packedColorA,&vertex->viewPointA);
    resultFlags = TerrainProjectedVertex_TransformAndProjectPointB(vertex,pointAFlags);
    shadedColorB = TerrainProjectedVertex_ShadeColor(vertex,vertex->packedColorB,&vertex->viewPointB);
    vertex->projectionFlags = resultFlags;
    vertex->shadedColorB = shadedColorB;
  }
  return;
}


/* Cheap per-frame update of one terrain vertex while the view and grid are unchanged: keeps the projection of
   the terrain point and only re-shades it; point B (the secondary surface point) is projected and shaded again
   only when the vertex belonged to a visible secondary-surface triangle last frame.
*/
void TerrainProjectedVertex_ReshadeKeepingProjection(TerrainProjectedVertexWorkRecord *vertex)

{
  uint32_t resultFlags;

  resultFlags = vertex->projectionFlags;
  if ((resultFlags & TERRAIN_VERTEX_SECONDARY_VISIBLE) != 0) {
    resultFlags = TerrainProjectedVertex_TransformAndProjectPointB(vertex,resultFlags & ~TERRAIN_VERTEX_POINT_B_BITS);
    vertex->shadedColorB = TerrainProjectedVertex_ShadeColor(vertex,vertex->packedColorB,&vertex->viewPointB);
  }
  vertex->shadedColorA = TerrainProjectedVertex_ShadeColor(vertex,vertex->packedColorA,&vertex->viewPointA);
  vertex->projectionFlags = resultFlags;
  return;
}


/* Cursor picking of TerrainProjectedTriangle_ClipInterpolateAndQueueTextured for one surface (the projected
   points A or B of the three vertices): when the cursor lies inside the projected triangle and vertex0's view
   depth (vertex0ViewPoint->z) is nearer than the best hit so far, stores that depth and the world X/Y of the
   cursor, interpolated from the vertices' source points with the barycentric weights. */
static void TerrainProjectedTriangle_PickCursor
          (GraphicsProjectedPointPair *projected2,GraphicsProjectedPointPair *projected1,
          GraphicsProjectedPointPair *projected0,GraphicsFixedVec3 *vertex0ViewPoint,
          TerrainProjectedVertexWorkRecord *vertex2,TerrainProjectedVertexWorkRecord *vertex1,
          TerrainProjectedVertexWorkRecord *vertex0,FrontendModelPointerContext *renderContext)

{
  TriangleBarycentricWeightsQ12 barycentricWeights;
  Bool8 outsideTriangle;
  uint32_t vertex0ViewDepth;

  barycentricWeights = Triangle2D_ComputeBarycentricWeightsQ12Packed
                     (projected2->projectedY,projected2->projectedX,projected1->projectedY,projected1->projectedX,
                      projected0->projectedY,projected0->projectedX,renderContext->cursorWorldYQ12,
                      renderContext->cursorWorldXQ12);
  outsideTriangle = g_Triangle2DBarycentricOutside; /* the original tests the outside flag right after the call */
  vertex0ViewDepth = vertex0ViewPoint->z;
  if ((!outsideTriangle) && ((int)vertex0ViewDepth < (int)renderContext->surfaceHitDepth)) {
    renderContext->surfaceHitDepth = vertex0ViewDepth;
    renderContext->surfaceHitWorldX =
         (((vertex1->sourcePoint).x - (vertex0->sourcePoint).x) * barycentricWeights.weightVertexB_Q12 >>
         Q12_SHIFT) + (vertex0->sourcePoint).x;
    renderContext->surfaceHitWorldY =
         (((vertex1->sourcePoint).y - (vertex0->sourcePoint).y) * barycentricWeights.weightVertexB_Q12 >>
         Q12_SHIFT) + (vertex0->sourcePoint).y;
    renderContext->surfaceHitWorldX =
         renderContext->surfaceHitWorldX +
         (((vertex2->sourcePoint).x - (vertex0->sourcePoint).x) * barycentricWeights.weightVertexA_Q12 >> Q12_SHIFT);
    renderContext->surfaceHitWorldY =
         renderContext->surfaceHitWorldY +
         (((vertex2->sourcePoint).y - (vertex0->sourcePoint).y) * barycentricWeights.weightVertexA_Q12 >> Q12_SHIFT);
  }
}

/* Lighting level of a soil vertex: the deeper under water, the darker; the vertex's lighting level minus half
   its (non-negative) secondary depth, at least 0. */
static int TerrainProjectedTriangle_SoilLightingLevel(TerrainProjectedVertexWorkRecord *vertex)

{
  uint32_t clampedDepth;
  int lightingLevel;

  clampedDepth = vertex->secondaryProjectionDepthQ12;
  if ((int)clampedDepth < 0) {
    clampedDepth = 0;
  }
  lightingLevel = vertex->lightingLookupIndexOrSentinel - (clampedDepth >> 1);
  if (lightingLevel < 0) {
    lightingLevel = 0;
  }
  return lightingLevel;
}

/* Queues one textured terrain triangle with the soil packet at packetOffset bytes into the soil packet table
   and ORs layerFlags into its render flags; returns the packet (NULL when it was not queued). */
static GraphicsPrimitivePacket *TerrainProjectedTriangle_QueueSoilPacket
          (void *soilPacketTable,int packetOffset,GraphicsPrimitiveDispatchFlags layerFlags,
          PackedArgb32 vertex2Color,PackedArgb32 vertex1Color,PackedArgb32 vertex0Color,
          TerrainProjectedVertexWorkRecord *vertex2,TerrainProjectedVertexWorkRecord *vertex1,
          TerrainProjectedVertexWorkRecord *vertex0,FrontendModelPointerContext *renderContext)

{
  GraphicsPrimitivePacket *queuedPacket;

  queuedPacket = GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle
                     ((uint32_t *)((uint8_t *)soilPacketTable + packetOffset),vertex2Color,vertex1Color,vertex0Color,
                      (GraphicsProjectedVertexSource *)vertex2,(GraphicsProjectedVertexSource *)vertex1,
                      (GraphicsProjectedVertexSource *)vertex0,renderContext);
  if (queuedPacket != NULL) {
    queuedPacket->renderFlags = queuedPacket->renderFlags | layerFlags;
  }
  return queuedPacket;
}

/* Terrain (point A) part of TerrainProjectedTriangle_ClipInterpolateAndQueueTextured: darkens the shaded vertex
   colours by the water depth and queues the triangle with the soil texture of vertex0's material; when the
   vertices have different materials, one or two blend triangles towards the other materials follow. */
static void TerrainProjectedTriangle_QueueSoilTriangles
          (TerrainProjectedVertexWorkRecord *vertex2,TerrainProjectedVertexWorkRecord *vertex1,
          TerrainProjectedVertexWorkRecord *vertex0,FrontendModelPointerContext *renderContext)

{
  void *soilPacketTable;
  PackedArgb32 vertex0Color;
  PackedArgb32 vertex1Color;
  PackedArgb32 vertex2Color;
  uint64_t litProduct0;
  uint64_t litProduct1;
  uint64_t litProduct2;
  int materialOffset0;
  int materialOffset1;
  int materialOffset2;
  int packetOffset1;
  int packetOffset2;
  GraphicsPrimitivePacket *queuedPacket;

  soilPacketTable = g_TerrainSoilPacketTablePayload;
  vertex0Color = vertex0->shadedColorA;
  vertex1Color = vertex1->shadedColorA;
  vertex2Color = vertex2->shadedColorA;
  /* PUNPCKLBW/PSRLW 4 of each color, PMULHW by its lighting level, PACKUSWB */
  litProduct0 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex0Color,4),
                       g_PackedLightingLookupTable[TerrainProjectedTriangle_SoilLightingLevel(vertex0)]);
  litProduct1 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex1Color,4),
                       g_PackedLightingLookupTable[TerrainProjectedTriangle_SoilLightingLevel(vertex1)]);
  litProduct2 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex2Color,4),
                       g_PackedLightingLookupTable[TerrainProjectedTriangle_SoilLightingLevel(vertex2)]);
  vertex0Color = TerrainProjection_PackWordsUnsignedSaturate(litProduct0);
  vertex1Color = TerrainProjection_PackWordsUnsignedSaturate(litProduct1);
  vertex2Color = TerrainProjection_PackWordsUnsignedSaturate(litProduct2);
  /* soil packet table: 0x800 bytes per material, 0x100 per variant (flag bits 8..10); +0x20..+0xA0 are the
     blend packets towards other materials */
  materialOffset0 = (vertex0->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES;
  materialOffset1 = (vertex1->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES;
  materialOffset2 = (vertex2->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES;
  packetOffset1 = materialOffset1 + (vertex1->projectionFlags & TERRAIN_VERTEX_VARIANT_OFFSET_MASK);
  packetOffset2 = materialOffset2 + (vertex2->projectionFlags & TERRAIN_VERTEX_VARIANT_OFFSET_MASK);
  queuedPacket = GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle
                     ((uint32_t *)((uint8_t *)soilPacketTable +
                               materialOffset0 + (vertex0->projectionFlags & TERRAIN_VERTEX_VARIANT_OFFSET_MASK)),
                      vertex2Color,vertex1Color,vertex0Color,(GraphicsProjectedVertexSource *)vertex2,
                      (GraphicsProjectedVertexSource *)vertex1,(GraphicsProjectedVertexSource *)vertex0,
                      renderContext);
  if (queuedPacket == NULL) {
    return;
  }
  if (materialOffset0 == materialOffset1) {
    if (materialOffset0 != materialOffset2) {
      TerrainProjectedTriangle_QueueSoilPacket
                (soilPacketTable,packetOffset2 + TERRAIN_SURFACE_PACKET_BYTES,TERRAIN_BLEND_PACKET_FIRST_LAYER_FLAGS,
                 vertex2Color,vertex1Color,vertex0Color,vertex2,vertex1,vertex0,renderContext);
    }
  }
  else if (materialOffset0 == materialOffset2) {
    TerrainProjectedTriangle_QueueSoilPacket
              (soilPacketTable,packetOffset1 + 2 * TERRAIN_SURFACE_PACKET_BYTES,TERRAIN_BLEND_PACKET_FIRST_LAYER_FLAGS,
               vertex2Color,vertex1Color,vertex0Color,vertex2,vertex1,vertex0,renderContext);
  }
  else if (materialOffset1 == materialOffset2) {
    TerrainProjectedTriangle_QueueSoilPacket
              (soilPacketTable,packetOffset1 + 3 * TERRAIN_SURFACE_PACKET_BYTES,TERRAIN_BLEND_PACKET_FIRST_LAYER_FLAGS,
               vertex2Color,vertex1Color,vertex0Color,vertex2,vertex1,vertex0,renderContext);
  }
  else {
    /* three different materials: two blend layers, the second only when the first was queued */
    queuedPacket = TerrainProjectedTriangle_QueueSoilPacket
                       (soilPacketTable,packetOffset1 + 4 * TERRAIN_SURFACE_PACKET_BYTES,
                        TERRAIN_BLEND_PACKET_FIRST_LAYER_FLAGS,vertex2Color,vertex1Color,vertex0Color,vertex2,
                        vertex1,vertex0,renderContext);
    if (queuedPacket != NULL) {
      TerrainProjectedTriangle_QueueSoilPacket
                (soilPacketTable,packetOffset2 + 5 * TERRAIN_SURFACE_PACKET_BYTES,
                 TERRAIN_BLEND_PACKET_SECOND_LAYER_FLAGS,vertex2Color,vertex1Color,vertex0Color,vertex2,vertex1,
                 vertex0,renderContext);
    }
  }
}

/* Queues one terrain triangle. When its screen bounds can overlap the clip rectangle, the terrain triangle is
   queued with the soil texture of vertex0's material (vertex colours darkened by the water depth); if the
   vertices have different materials, one or two blend triangles of the other materials follow. When any vertex
   lies under water (or WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY is set) and the secondary points can be
   visible, the secondary-surface triangle is queued as well. Along the way the cursor is picked: when it lies
   inside the projected triangle and nearer than the best hit so far, the hit depth (surfaceHitDepth) and the
   interpolated world X/Y (surfaceHitWorldX/EC) are stored; the top byte of g_UiCommandModeGColorVariantLimit
   chooses between the terrain (0) and the secondary surface. Skipped entirely when the OR of the three material
   bytes is 0xFF, which it always is when a vertex has no terrain.
*/
void TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
          (int surfacePacketIndex,TerrainProjectedVertexWorkRecord *vertex2,
          TerrainProjectedVertexWorkRecord *vertex1,TerrainProjectedVertexWorkRecord *vertex0,
          FrontendModelPointerContext *renderContext)

{
  uint32_t combinedFlags;
  PackedArgb32 vertex0Color;
  uint64_t litProduct0;
  PackedArgb32 vertex1Color;
  uint64_t litProduct1;
  PackedArgb32 vertex2Color;
  uint64_t litProduct2;

  combinedFlags = vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags;
  if ((combinedFlags & TERRAIN_VERTEX_MATERIAL_MASK) != TERRAIN_VERTEX_MATERIAL_NONE) {
    /* all four point-A side bits set by some vertex and every point A projected */
    if ((combinedFlags & TERRAIN_VERTEX_POINT_A_BITS) == TERRAIN_VERTEX_POINT_A_SIDE_BITS) {
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) == 0) {
        TerrainProjectedTriangle_PickCursor
                  (&vertex2->projectedPointA,&vertex1->projectedPointA,&vertex0->projectedPointA,
                   &vertex0->viewPointA,vertex2,vertex1,vertex0,renderContext);
      }
      TerrainProjectedTriangle_QueueSoilTriangles(vertex2,vertex1,vertex0,renderContext);
    }
    if ((((((renderContext->contextFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY) != 0) ||
          (0 < vertex0->secondaryProjectionDepthQ12)) || (0 < vertex1->secondaryProjectionDepthQ12))
        || (0 < vertex2->secondaryProjectionDepthQ12)) &&
       (((vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags) &
        TERRAIN_VERTEX_POINT_B_BITS) == TERRAIN_VERTEX_POINT_B_SIDE_BITS)) { /* the same test for point B */
      vertex0->projectionFlags = vertex0->projectionFlags | TERRAIN_VERTEX_SECONDARY_VISIBLE;
      vertex1->projectionFlags = vertex1->projectionFlags | TERRAIN_VERTEX_SECONDARY_VISIBLE;
      vertex2->projectionFlags = vertex2->projectionFlags | TERRAIN_VERTEX_SECONDARY_VISIBLE;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
        TerrainProjectedTriangle_PickCursor
                  (&vertex2->projectedPointB,&vertex1->projectedPointB,&vertex0->projectedPointB,
                   &vertex0->viewPointB,vertex2,vertex1,vertex0,renderContext);
      }
      vertex0Color = vertex0->shadedColorB;
      vertex1Color = vertex1->shadedColorB;
      vertex2Color = vertex2->shadedColorB;
      litProduct0 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex0Color,4),
                           g_PackedLightingLookupTable[vertex0->lightingLookupIndexOrSentinel]);
      litProduct1 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex1Color,4),
                           g_PackedLightingLookupTable[vertex1->lightingLookupIndexOrSentinel]);
      litProduct2 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex2Color,4),
                           g_PackedLightingLookupTable[vertex2->lightingLookupIndexOrSentinel]);
      GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
                ((uint32_t *)((uint8_t *)g_TerrainSurfacePacketTablePayload +
                              surfacePacketIndex * TERRAIN_SURFACE_PACKET_BYTES),
                 TerrainProjection_PackWordsUnsignedSaturate(litProduct2),
                 TerrainProjection_PackWordsUnsignedSaturate(litProduct1),
                 TerrainProjection_PackWordsUnsignedSaturate(litProduct0),
                 (GraphicsProjectedVertexSource *)vertex2,(GraphicsProjectedVertexSource *)vertex1,
                 (GraphicsProjectedVertexSource *)vertex0,renderContext);
    }
  }
  return;
}


/* Narrows the per-row visible column spans (g_TerrainProjectedRowSpans) by one frustum side plane through the
   view origin: a plane with an x component moves the first or end column of every row to the column where the
   plane crosses that row (skewed by half a column per row); a plane parallel to the columns empties the rows
   on its far side.
*/
void TerrainProjectedGrid_ClipRowSpansAgainstPlane(FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal)

{
  int64_t fixedProduct;
  int64_t columnStepProduct;
  int planeYOffset;
  int columnBound;
  uint32_t columnEdgeQ12;
  int cutoffRow;
  int rowsFromCutoff;
  int rowsToEmpty;
  FieldGridDimension rowsRemaining;
  TerrainProjectedRowSpan *rowSpan;

  if (planeNormal->x == 0) {
    if (planeNormal->y != 0) {
      planeYOffset = 0;
      if (-1 < planeNormal->z) {
        planeYOffset = (int)(((int64_t)g_ViewOriginFixed.z * (int64_t)planeNormal->z) / (int64_t)planeNormal->y);
      }
      fixedProduct = (int64_t)(planeYOffset + g_ViewOriginFixed.y) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      cutoffRow = (int)(FIXED_PRODUCT_SHR(fixedProduct, Q20_SHIFT)) >> Q12_SHIFT;
      if (planeNormal->y < 0) {
        rowsFromCutoff = fieldGrid->gridHeight - cutoffRow;
        if (rowsFromCutoff != 0 && cutoffRow <= (int)fieldGrid->gridHeight) {
          rowsToEmpty = rowsFromCutoff - 1;
          /* rows cutoffRow + 3 on are emptied, so the last two emptied rows lie past the grid (the table has 260) */
          for (rowSpan = g_TerrainProjectedRowSpans + cutoffRow + 3; rowsToEmpty != 0; rowsToEmpty--) {
            rowSpan->firstColumn = 0;
            rowSpan->endColumnExclusive = 0;
            rowSpan = rowSpan + 1;
          }
        }
      }
      else if (-1 < cutoffRow) {
        /* rows 0..cutoffRow-1 are emptied */
        rowSpan = g_TerrainProjectedRowSpans;
        for (rowsToEmpty = cutoffRow; rowsToEmpty != 0; rowsToEmpty--) {
          rowSpan->firstColumn = 0;
          rowSpan->endColumnExclusive = 0;
          rowSpan = rowSpan + 1;
        }
      }
    }
  }
  else if (planeNormal->x < 0) {
    fixedProduct = (int64_t)planeNormal->y * (int64_t)g_ViewOriginFixed.y +
            (int64_t)planeNormal->x * (int64_t)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      fixedProduct = fixedProduct + (int64_t)planeNormal->z * (int64_t)g_ViewOriginFixed.z;
    }
    fixedProduct = (int64_t)(int)(fixedProduct / (int64_t)planeNormal->x) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    columnEdgeQ12 = FIXED_PRODUCT_SHR(fixedProduct, Q20_SHIFT);
    /* column shift per row: 1999 = 2^32 / -FIELD_GRID_WORLD_Y_TO_ROW_Q20 is one row in world Y, then half a column of
       skew is subtracted */
    columnStepProduct = (int64_t)(int)(((int64_t)planeNormal->y * 1999) / (int64_t)planeNormal->x) *
                        FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    /* raises every row's first column to the plane's crossing column */
    rowSpan = g_TerrainProjectedRowSpans;
    rowsRemaining = fieldGrid->gridHeight;
    do {
      columnBound = (int)(columnEdgeQ12 - FIELD_GRID_CELL_Q12) >> Q12_SHIFT;
      columnEdgeQ12 = columnEdgeQ12 + ((FIXED_PRODUCT_SHR(columnStepProduct, Q20_SHIFT)) - FIELD_GRID_CELL_Q12 / 2);
      if (rowSpan->firstColumn < columnBound) {
        rowSpan->firstColumn = columnBound;
      }
      rowSpan = rowSpan + 1;
      rowsRemaining--;
    } while (rowsRemaining != 0);
  }
  else {
    fixedProduct = (int64_t)planeNormal->y * (int64_t)g_ViewOriginFixed.y +
            (int64_t)planeNormal->x * (int64_t)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      fixedProduct = fixedProduct + (int64_t)planeNormal->z * (int64_t)g_ViewOriginFixed.z;
    }
    fixedProduct = (int64_t)(int)(fixedProduct / (int64_t)planeNormal->x) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    columnEdgeQ12 = FIXED_PRODUCT_SHR(fixedProduct, Q20_SHIFT);
    columnStepProduct = (int64_t)(int)(((int64_t)planeNormal->y * 1999) / (int64_t)planeNormal->x) *
                        FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    /* lowers every row's end column to the plane's crossing column */
    rowSpan = g_TerrainProjectedRowSpans;
    rowsRemaining = fieldGrid->gridHeight;
    do {
      columnBound = (int)(columnEdgeQ12 + (FIELD_GRID_TWO_CELLS_Q12 - 1)) >> Q12_SHIFT;
      columnEdgeQ12 = columnEdgeQ12 + ((FIXED_PRODUCT_SHR(columnStepProduct, Q20_SHIFT)) - FIELD_GRID_CELL_Q12 / 2);
      if (columnBound < rowSpan->endColumnExclusive) {
        rowSpan->endColumnExclusive = columnBound;
      }
      rowSpan = rowSpan + 1;
      rowsRemaining--;
    } while (rowsRemaining != 0);
  }
  return;
}


/* Line-of-sight leg along direction 0 (C+1, right): each cell's surface height (terrain plus positive water)
   above the eye (g_TerrainScanReferenceHeight) is scaled by the per-step table g_TerrainHeightDeltaScaleByStepQ12;
   a cell whose value reaches the highest value seen so far on this line is visible and gets occupancyMaskBits,
   and its value becomes the new horizon. 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainProjectedOcclusion_ScanDirection0(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      /* the table holds one int per scan step; the product keeps bits 12..43 */
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell++;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Line-of-sight leg along direction 1 (C+1-W, up and right); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection1(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Line-of-sight leg along direction 2 (C-W, up); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection2(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Line-of-sight leg along direction 3 (C-1, left); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection3(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell--;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Line-of-sight leg along direction 4 (C-1+W, down and left); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection4(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Line-of-sight leg along direction 5 (C+W, down); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void TerrainProjectedOcclusion_ScanDirection5(uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightQ20;

  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
      projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 0 (C+1, right) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint:
   stores the overlay value (g_TerrainScanReferenceHeight) into overlayColor of every cell
   that has a bit of the overlay's cell flag mask and no water above it (waterSurfaceDelta < 0), 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void FieldGridTerrainOverlayVariantA_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 1 (C+1-W, up and right) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantA_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell + 1,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 2 (C-W, up) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantA_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 3 (C-1, left) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantA_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell - 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 4 (C-1+W, down and left) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantA_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 5 (C+W, down) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantA_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 0 (C+1, right) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint:
   stores the overlay value (g_TerrainScanReferenceHeight) into overlayColor of every cell
   with water above it (waterSurfaceDelta > 0), whatever its flags, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void FieldGridTerrainOverlayVariantB_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 1 (C+1-W, up and right) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantB_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell + 1,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 2 (C-W, up) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantB_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 3 (C-1, left) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantB_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell - 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 4 (C-1+W, down and left) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantB_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell - 1,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Overlay leg along direction 5 (C+W, down) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
   works like FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void FieldGridTerrainOverlayVariantB_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->overlayColor = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldCell,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

