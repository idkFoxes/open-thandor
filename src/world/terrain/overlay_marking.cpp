/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/overlay_marking.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Terrain overlay marking around a world point (variants A and B): marks the cells of the hexagon wedges and
   directions within a radius. */

#include <thandor/world/terrain/overlay_marking.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* The cell rules of the two overlay variants, for the hexagon walk of hex_scan.h (formerly 24 functions
   FieldGridTerrainOverlayVariantA/B_ApplyWedge0..5 and _ApplyDirection0..5). The stored value is
   g_TerrainScanReferenceHeight, which the drivers set to the overlay's cell value. */

/* Variant A: cells with a bit of the overlay's flag mask and no water above them. */
static void FieldGridTerrainOverlayVariantA_ApplyToCell(FieldGridCell *fieldCell)

{
  if (((FieldCell_RawBits(fieldCell->flagsAndMaterial) & FieldCell_RawBits(g_TerrainScanSharedSelectorValue.fieldCellFlagMask)) != 0) &&
      (fieldCell->waterSurfaceDelta < 0)) {
    fieldCell->overlayColor = g_TerrainScanReferenceHeight;
  }
}

/* Variant B: cells with water above them, whatever their flags (Original quirk: only the centre cell is
   tested against the flag mask). */
static void FieldGridTerrainOverlayVariantB_ApplyToCell(FieldGridCell *fieldCell)

{
  if (0 < fieldCell->waterSurfaceDelta) {
    fieldCell->overlayColor = g_TerrainScanReferenceHeight;
  }
}

/* Terrain-class overlay callback for land classes (g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
   slots 0, 2, 3 and 4, called by WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries): stores cellValue into
   overlayColor of every cell within the radius around the world point that has a bit of
   cellFlagMask and no water above it; the centre cell here, the rest by the six sector walks (the same hexagon
   as TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint, without line of sight). Marks the field grid
   surface dirty (Original quirk: before the bounds check, so also when nothing is applied). Returns true
   (nothing applied) without a grid, outside it or on a map-edge cell.
*/
bool FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  uint32_t gridWidth;
  int centerCellIndex;
  FieldGridCoordinates gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != nullptr) {
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
      if (!Any(fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
        if (((FieldCell_RawBits(fieldGrid->cells[centerCellIndex].flagsAndMaterial) & FieldCell_RawBits(cellFlagMask)) != 0) &&
           (fieldGrid->cells[centerCellIndex].waterSurfaceDelta < 0)) {
          fieldGrid->cells[centerCellIndex].overlayColor = cellValue;
        }
        /* the six sector walks, starting at C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width) */
        TerrainHexScan_AllSectors(&fieldGrid->cells[centerCellIndex],
                                  TerrainHexScan_MarkPolicy([](FieldGridCell *fieldCell) {
                                    FieldGridTerrainOverlayVariantA_ApplyToCell(fieldCell);
                                  }));
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
bool FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  uint32_t gridWidth;
  int centerCellIndex;
  FieldGridCoordinates gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != nullptr) {
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
      if (!Any(fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
        if (((FieldCell_RawBits(fieldGrid->cells[centerCellIndex].flagsAndMaterial) & FieldCell_RawBits(cellFlagMask)) != 0) &&
           (0 < fieldGrid->cells[centerCellIndex].waterSurfaceDelta)) {
          fieldGrid->cells[centerCellIndex].overlayColor = cellValue;
        }
        /* the six sector walks, starting at C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width) */
        TerrainHexScan_AllSectors(&fieldGrid->cells[centerCellIndex],
                                  TerrainHexScan_MarkPolicy([](FieldGridCell *fieldCell) {
                                    FieldGridTerrainOverlayVariantB_ApplyToCell(fieldCell);
                                  }));
        return false;
      }
    }
  }
  return true;
}
