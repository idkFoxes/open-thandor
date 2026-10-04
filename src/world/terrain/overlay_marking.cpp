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
