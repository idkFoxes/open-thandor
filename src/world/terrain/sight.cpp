/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/sight.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Line of sight over the terrain: the occlusion mask around a world point, traced along the six hexagon
   wedges and directions (fog-of-war visibility, not screen projection). */

#include <thandor/world/terrain/sight.h>
#include <thandor/world/terrain/hex_scan.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

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


/* The line-of-sight cell policy of the hexagon walk (TerrainHexScan_Sector / TerrainHexScan_Leg): each cell's
   surface height (terrain plus positive water) above the eye (g_TerrainScanReferenceHeight) is scaled by
   g_TerrainHeightDeltaScaleByStepQ12 at the cell's scan step (a leg cell its own step, a spine cell the spine step
   s, the cell between two spine cells s+4); a cell whose value reaches the threshold is visible, gets
   occupancyMaskBits and its value becomes the new horizon. The threshold is the horizon, except for the first
   spine cell of a sector, which is compared against its own unscaled height above the eye (original quirk).
   The walk passes the policy by value, so a leg works on a copy of the horizon and its updates never flow back
   into the sector. Comparisons are signed on the uint32 values, as in the original. */
struct TerrainProjectedOcclusion_SightPolicy {
  static constexpr bool IsTest = false;
  uint64_t occupancyMaskBits;
  TerrainProjectedHeightThresholdQ20 horizonQ20;
  TerrainProjectedHeightThresholdQ20 thresholdQ20;

  void visit(FieldGridCell *cell,TerrainDirectionalScanStep scanStep)
  {
    int64_t scaledHeightProduct;
    int cellHeight;
    uint32_t projectedHeightQ20;

    cellHeight = cell->terrainHeight;
    if (0 < cell->waterSurfaceDelta) {
      cellHeight = cellHeight + cell->waterSurfaceDelta;
    }
    /* the table holds one int per scan step; the product keeps bits 12..43 */
    scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
            (int64_t)g_TerrainHeightDeltaScaleByStepQ12[scanStep];
    projectedHeightQ20 = FIXED_PRODUCT_SHR(scaledHeightProduct, 12);
    if ((int)thresholdQ20 <= (int)projectedHeightQ20) {
      cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
      horizonQ20 = projectedHeightQ20;
    }
    thresholdQ20 = horizonQ20;
  }
};

/* One sector of the line-of-sight marking from its first cell (scan step 0). The first cell's unscaled height above
   the eye is read first; the starting horizon is the average of it and seedHeightQ20 (the caller passes the
   unscaled height of the next sector's first cell), summed in uint32 and shifted signed (original quirk). */
template <int Sector>
static void TerrainProjectedOcclusion_TraceSector
          (uint64_t occupancyMaskBits,TerrainProjectedHeightThresholdQ20 seedHeightQ20,FieldGridCell *firstCell)
{
  TerrainProjectedHeightThresholdQ20 ownHeightQ20;
  TerrainProjectedOcclusion_SightPolicy policy;

  ownHeightQ20 = firstCell->terrainHeight - g_TerrainScanReferenceHeight;
  policy.occupancyMaskBits = occupancyMaskBits;
  policy.horizonQ20 = (int)(seedHeightQ20 + ownHeightQ20) >> 1;
  policy.thresholdQ20 = ownHeightQ20;
  TerrainHexScan_Sector<Sector>(0,firstCell,policy);
}

/* Line-of-sight marking for one army (occupancy rebuild): from the grid vertex nearest to the world point, ORs
   occupancyMaskBits (the bits of the factions that share the army's sight) into every cell the terrain does not
   hide from an eye at referenceHeightQ12 within the radius. The centre cell is marked here, the rest by the six
   sector traces (TerrainHexScan_Sector with TerrainProjectedOcclusion_SightPolicy), each seeded with the height of
   the next sector's first cell. Nothing happens outside the grid or on a map-edge cell.
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

  if (fieldGrid != nullptr) {
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
        TerrainProjectedOcclusion_TraceSector<0>
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,wedgeCellA);
        TerrainProjectedOcclusion_TraceSector<1>
                  (occupancyMaskBits,wedgeCellB[-1].terrainHeight - referenceHeight,wedgeCellB);
        wedgeCellA = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellB - 2,rowStrideBytes);
        TerrainProjectedOcclusion_TraceSector<2>
                  (occupancyMaskBits,wedgeCellA->terrainHeight - referenceHeight,wedgeCellB - 1);
        wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,rowStrideBytes);
        TerrainProjectedOcclusion_TraceSector<3>
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,wedgeCellA);
        TerrainProjectedOcclusion_TraceSector<4>
                  (occupancyMaskBits,wedgeCellB[1].terrainHeight - referenceHeight,wedgeCellB);
        /* the terrain height of C+1 (one row up from C+W+1), the first cell of sector 0 */
        TerrainProjectedOcclusion_TraceSector<5>
                  (occupancyMaskBits,
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellB + 2,-rowStrideBytes)->terrainHeight - referenceHeight,
                   wedgeCellB + 1);
      }
    }
  }
}
