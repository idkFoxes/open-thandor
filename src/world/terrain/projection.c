/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/projection.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/projection.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: world/terrain/projection. */

/* Address: 0x00506CD0.
   Ownership: world/terrain/projection.
   Purpose: Converts one world point to the hexagonal field grid, derives the bounded scan radius and row stride,
   seeds the center occupancy mask, and dispatches all six projected-occlusion wedge traces. Storage remains one
   signed 32-bit word. Typed parameters: p3 referenceHeightQ12→Q12. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged. Typed parameters: p4 worldXQ12→Q12, p5 worldYQ12→Q12.
   Local calls: TerrainProjectedOcclusion_TraceWedge0, TerrainProjectedOcclusion_TraceWedge1,
   TerrainProjectedOcclusion_TraceWedge2, TerrainProjectedOcclusion_TraceWedge3,
   TerrainProjectedOcclusion_TraceWedge4, TerrainProjectedOcclusion_TraceWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (ulonglong occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid)

{
  uint baseColumn;
  int rowStrideBytes;
  int referenceHeight;
  uint columnFraction;
  uint fractionSumOrGridWidth;
  uint rowFraction;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint gridRow;
  uint gridColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    referenceHeight = g_TerrainScanReferenceHeight;
    baseColumn = gridCoordinates.columnQ12 >> 0xc;
    gridRow = gridCoordinates.rowQ12 >> 0xc;
    columnFraction = (uint)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff);
    rowFraction = (uint)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff) >> 0x20);
    fractionSumOrGridWidth = rowFraction + columnFraction * 2;
    gridColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFraction + rowFraction * 2) {
        gridRow = gridRow + 1;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      gridColumn = baseColumn + 1;
      if (columnFraction < rowFraction) {
        gridRow = gridRow + 1;
        gridColumn = baseColumn;
      }
    }
    else {
      gridColumn = baseColumn + 1;
      if (0x1fff < columnFraction + rowFraction * 2) {
        gridRow = gridRow + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)gridColumn) && (fractionSumOrGridWidth = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)gridRow)) &&
        (gridRow < fieldGrid->gridHeight)) && (gridColumn < fractionSumOrGridWidth)) {
      centerCellIndex = gridRow * fractionSumOrGridWidth + gridColumn;
      if ((fieldGrid->cells[centerCellIndex].flagsAndMaterial & 0x88006000) == 0) {
        fieldGrid->cells[centerCellIndex].occupancyMask =
             fieldGrid->cells[centerCellIndex].occupancyMask | occupancyMaskBits;
        rowStrideBytes = g_TerrainScanRowStrideBytes;
        wedgeCellA = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 centerCellIndex * 0x80 + -0x28);
        wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge0
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge1
                  (occupancyMaskBits,wedgeCellB[-1].terrainHeight - referenceHeight,0,wedgeCellB);
        wedgeCellA = (FieldGridCell *)((wedgeCellB + -1)[-1].runtime0C_3F + rowStrideBytes + -0xc);
        TerrainProjectedOcclusion_TraceWedge2
                  (occupancyMaskBits,wedgeCellA->terrainHeight - referenceHeight,0,wedgeCellB + -1);
        wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + rowStrideBytes + -0xc);
        TerrainProjectedOcclusion_TraceWedge3
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge4
                  (occupancyMaskBits,wedgeCellB[1].terrainHeight - referenceHeight,0,wedgeCellB);
        TerrainProjectedOcclusion_TraceWedge5
                  (occupancyMaskBits,*(int *)((int)(wedgeCellB + 1) + (200 - rowStrideBytes)) - referenceHeight,0,
                   wedgeCellB + 1);
      }
    }
  }
  return;
}


/* Address: 0x005099D0.
   Ownership: world/terrain/projection.
   Purpose: Terrain-class runtime overlay callback; exact six-stack-argument contract and CF result. No unproved
   bit labels are introduced. Typed parameters: p0 cellFlagMask→FieldCellFlagMask_V338. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   radiusWorldUnits→FieldGridRadiusUnits.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyWedge0, FieldGridTerrainOverlayVariantA_ApplyWedge1,
   FieldGridTerrainOverlayVariantA_ApplyWedge2, FieldGridTerrainOverlayVariantA_ApplyWedge3,
   FieldGridTerrainOverlayVariantA_ApplyWedge4, FieldGridTerrainOverlayVariantA_ApplyWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint baseColumn;
  int rowStrideBytes;
  uint columnFraction;
  uint fractionSumOrGridWidth;
  uint rowFraction;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *fieldCell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint gridRow;
  uint gridColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    baseColumn = gridCoordinates.columnQ12 >> 0xc;
    gridRow = gridCoordinates.rowQ12 >> 0xc;
    columnFraction = (uint)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff);
    rowFraction = (uint)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff) >> 0x20);
    fractionSumOrGridWidth = rowFraction + columnFraction * 2;
    gridColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFraction + rowFraction * 2) {
        gridRow = gridRow + 1;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      gridColumn = baseColumn + 1;
      if (columnFraction < rowFraction) {
        gridRow = gridRow + 1;
        gridColumn = baseColumn;
      }
    }
    else {
      gridColumn = baseColumn + 1;
      if (0x1fff < columnFraction + rowFraction * 2) {
        gridRow = gridRow + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)gridColumn) && (fractionSumOrGridWidth = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)gridRow)) &&
        (gridRow < fieldGrid->gridHeight)) &&
       ((gridColumn < fractionSumOrGridWidth &&
        (centerCellIndex = gridRow * fractionSumOrGridWidth + gridColumn,
        (fieldGrid->cells[centerCellIndex].flagsAndMaterial & 0x88006000) == 0)))) {
      if (((fieldGrid->cells[centerCellIndex].flagsAndMaterial & cellFlagMask) != 0) &&
         (fieldGrid->cells[centerCellIndex].waterSurfaceDelta < 0)) {
        fieldGrid->cells[centerCellIndex].runtimeOverlayOrHeightValue04 = cellValue;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      wedgeCellA = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               centerCellIndex * 0x80 + -0x28);
      wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyWedge0(0,wedgeCellA);
      fieldCell = wedgeCellB + -1;
      FieldGridTerrainOverlayVariantA_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + rowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantA_ApplyWedge2(0,fieldCell);
      wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + rowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantA_ApplyWedge3(0,wedgeCellA);
      FieldGridTerrainOverlayVariantA_ApplyWedge4(0,wedgeCellB);
      FieldGridTerrainOverlayVariantA_ApplyWedge5(0,wedgeCellB + 1);
      return false;
    }
  }
  return true;
}


/* Address: 0x0050A190.
   Ownership: world/terrain/projection.
   Purpose: Terrain-class runtime overlay callback; exact six-stack-argument contract and CF result. No unproved
   bit labels are introduced. Typed parameters: p0 cellFlagMask→FieldCellFlagMask_V338. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   radiusWorldUnits→FieldGridRadiusUnits.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyWedge0, FieldGridTerrainOverlayVariantB_ApplyWedge1,
   FieldGridTerrainOverlayVariantB_ApplyWedge2, FieldGridTerrainOverlayVariantB_ApplyWedge3,
   FieldGridTerrainOverlayVariantB_ApplyWedge4, FieldGridTerrainOverlayVariantB_ApplyWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint baseColumn;
  int rowStrideBytes;
  uint columnFraction;
  uint fractionSumOrGridWidth;
  uint rowFraction;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *fieldCell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint gridRow;
  uint gridColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    baseColumn = gridCoordinates.columnQ12 >> 0xc;
    gridRow = gridCoordinates.rowQ12 >> 0xc;
    columnFraction = (uint)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff);
    rowFraction = (uint)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff) >> 0x20);
    fractionSumOrGridWidth = rowFraction + columnFraction * 2;
    gridColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFraction + rowFraction * 2) {
        gridRow = gridRow + 1;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      gridColumn = baseColumn + 1;
      if (columnFraction < rowFraction) {
        gridRow = gridRow + 1;
        gridColumn = baseColumn;
      }
    }
    else {
      gridColumn = baseColumn + 1;
      if (0x1fff < columnFraction + rowFraction * 2) {
        gridRow = gridRow + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)gridColumn) && (fractionSumOrGridWidth = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)gridRow)) &&
        (gridRow < fieldGrid->gridHeight)) &&
       ((gridColumn < fractionSumOrGridWidth &&
        (centerCellIndex = gridRow * fractionSumOrGridWidth + gridColumn,
        (fieldGrid->cells[centerCellIndex].flagsAndMaterial & 0x88006000) == 0)))) {
      if (((fieldGrid->cells[centerCellIndex].flagsAndMaterial & cellFlagMask) != 0) &&
         (0 < fieldGrid->cells[centerCellIndex].waterSurfaceDelta)) {
        fieldGrid->cells[centerCellIndex].runtimeOverlayOrHeightValue04 = cellValue;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      wedgeCellA = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               centerCellIndex * 0x80 + -0x28);
      wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyWedge0(0,wedgeCellA);
      fieldCell = wedgeCellB + -1;
      FieldGridTerrainOverlayVariantB_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + rowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantB_ApplyWedge2(0,fieldCell);
      wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + rowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantB_ApplyWedge3(0,wedgeCellA);
      FieldGridTerrainOverlayVariantB_ApplyWedge4(0,wedgeCellB);
      FieldGridTerrainOverlayVariantB_ApplyWedge5(0,wedgeCellB + 1);
      return false;
    }
  }
  return true;
}


/* Address: 0x00500F50.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected grid transform shade and queue.
   Local calls: TerrainProjectedGrid_ClipRowSpansAgainstPlane,
   TerrainProjectedVertex_TransformProjectAndShadeVariantB,
   TerrainProjectedVertex_TransformProjectAndShadeVariantA, TerrainProjectedQuad_QueueAsTwoTrianglesRegs.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedGrid_TransformShadeAndQueue
          (FieldGridAsset *fieldGrid,FrontendModelPointerContextRuntimeState17C *renderContext)

{
  FieldGridDimension gridWidth;
  int spanFirstColumn;
  int spanEndColumn;
  int columnOrVertexCount;
  FieldGridDimension rowCount;
  FieldGridDimension rowsRemaining;
  int firstColumnOrRowsLeft;
  FieldGridDimension columnsRemaining;
  FieldGridCell *rowCells;
  TerrainProjectedVertexWorkRecord *vertexCursor;
  TerrainProjectedRowSpan *rowSpan;
  int remainingCount;
  
  if ((renderContext->contextFlags & 0x800) == 0) {
    rowSpan = g_TerrainProjectedRowSpans;
    gridWidth = fieldGrid->gridWidth;
    rowCount = fieldGrid->gridHeight;
    do {
      rowSpan->firstColumn = 0;
      rowSpan->endColumnExclusive = gridWidth;
      rowSpan = rowSpan + 1;
      rowCount = rowCount - 1;
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
        rowCells->flagsAndMaterial = rowCells->flagsAndMaterial | 0x4200000;
        rowCells = rowCells + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridWidth;
    } while (rowsRemaining != 0);
    rowSpan = g_TerrainProjectedRowSpans;
    remainingCount = rowCount - 1;
    firstColumnOrRowsLeft = g_TerrainProjectedRowSpans[0].firstColumn;
    columnOrVertexCount = g_TerrainProjectedRowSpans[0].endColumnExclusive;
    do {
      rowSpan = rowSpan + 1;
      spanFirstColumn = rowSpan->firstColumn;
      spanEndColumn = rowSpan->endColumnExclusive;
      if (columnOrVertexCount == 0) {
        rowSpan[-1].firstColumn = spanFirstColumn;
        rowSpan[-1].endColumnExclusive = spanEndColumn;
      }
      else if (spanEndColumn == 0) {
        rowSpan->firstColumn = firstColumnOrRowsLeft;
        rowSpan->endColumnExclusive = columnOrVertexCount;
      }
      else {
        if (firstColumnOrRowsLeft < spanFirstColumn) {
          rowSpan->firstColumn = firstColumnOrRowsLeft;
        }
        else if (spanFirstColumn < rowSpan[-1].firstColumn) {
          rowSpan[-1].firstColumn = spanFirstColumn;
        }
        if (spanEndColumn < columnOrVertexCount) {
          rowSpan->endColumnExclusive = columnOrVertexCount;
        }
        else if (rowSpan[-1].endColumnExclusive < spanEndColumn) {
          rowSpan[-1].endColumnExclusive = spanEndColumn;
        }
      }
      remainingCount = remainingCount + -1;
      firstColumnOrRowsLeft = spanFirstColumn;
      columnOrVertexCount = spanEndColumn;
    } while (remainingCount != 0);
  }
  rowSpan = g_TerrainProjectedRowSpans;
  gridWidth = fieldGrid->gridWidth;
  rowCount = fieldGrid->gridHeight;
  if (((fieldGrid->runtimeStateFlags & 1) == 0) && ((renderContext->contextFlags & 0x800) != 0)) {
    rowCells = fieldGrid->cells;
    do {
      firstColumnOrRowsLeft = rowSpan->firstColumn;
      columnOrVertexCount = rowSpan->endColumnExclusive - firstColumnOrRowsLeft;
      if (columnOrVertexCount != 0 && firstColumnOrRowsLeft <= rowSpan->endColumnExclusive) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + firstColumnOrRowsLeft);
        do {
          TerrainProjectedVertex_TransformProjectAndShadeVariantB(vertexCursor);
          vertexCursor = vertexCursor + 1;
          columnOrVertexCount = columnOrVertexCount + -1;
        } while (columnOrVertexCount != 0);
      }
      rowSpan = rowSpan + 1;
      rowCells = rowCells + gridWidth;
      rowCount = rowCount - 1;
    } while (rowCount != 0);
  }
  else {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags & 0xfffffffe;
    renderContext->contextFlags = renderContext->contextFlags & 0xfffff7ff;
    rowCells = fieldGrid->cells;
    do {
      firstColumnOrRowsLeft = rowSpan->firstColumn;
      columnOrVertexCount = rowSpan->endColumnExclusive - firstColumnOrRowsLeft;
      if (columnOrVertexCount != 0 && firstColumnOrRowsLeft <= rowSpan->endColumnExclusive) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + firstColumnOrRowsLeft);
        do {
          TerrainProjectedVertex_TransformProjectAndShadeVariantA(vertexCursor);
          vertexCursor = vertexCursor + 1;
          columnOrVertexCount = columnOrVertexCount + -1;
        } while (columnOrVertexCount != 0);
      }
      rowSpan = rowSpan + 1;
      rowCells = rowCells + gridWidth;
      rowCount = rowCount - 1;
    } while (rowCount != 0);
  }
  gridWidth = fieldGrid->gridWidth;
  rowCells = fieldGrid->cells;
  rowSpan = g_TerrainProjectedRowSpans;
  firstColumnOrRowsLeft = fieldGrid->gridHeight - 1;
  do {
    columnOrVertexCount = rowSpan->firstColumn;
    remainingCount = rowSpan->endColumnExclusive - columnOrVertexCount;
    if (remainingCount != 0 && columnOrVertexCount <= rowSpan->endColumnExclusive) {
      remainingCount = remainingCount + -1;
      if (remainingCount != 0) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + columnOrVertexCount);
        do {
          TerrainProjectedQuad_QueueAsTwoTrianglesRegs(gridWidth * 0x80,vertexCursor,renderContext);
          vertexCursor = vertexCursor + 1;
          remainingCount = remainingCount + -1;
        } while (remainingCount != 0);
      }
    }
    rowSpan = rowSpan + 1;
    rowCells = rowCells + gridWidth;
    firstColumnOrRowsLeft = firstColumnOrRowsLeft + -1;
  } while (firstColumnOrRowsLeft != 0);
  return;
}


/* Address: 0x005066D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 0, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection0, TerrainProjectedOcclusion_ScanDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge0
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int heightOrRowStride;
  int neighborHeight;
  uint projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightOrRowStride = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        heightOrRowStride = heightOrRowStride + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(heightOrRowStride - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      heightOrRowStride = g_TerrainScanRowStrideBytes;
      adjacentCell = cell + 1;
      projectedHeightOrStep = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,adjacentCell);
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((*(uint *)((int)adjacentCell + (0x50 - heightOrRowStride)) & 0x88006000) != 0) {
        return;
      }
      neighborHeight = *(int *)((int)adjacentCell + (0x48 - heightOrRowStride));
      if (0 < *(int *)((int)adjacentCell + (0x4c - heightOrRowStride))) {
        neighborHeight = neighborHeight + *(int *)((int)adjacentCell + (0x4c - heightOrRowStride));
      }
      scaledHeightProduct = (longlong)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(ulonglong *)((int)adjacentCell + (0x70 - heightOrRowStride)) =
             *(ulonglong *)((int)adjacentCell + (0x70 - heightOrRowStride)) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      cell = (FieldGridCell *)((int)adjacentCell + (0x80 - heightOrRowStride));
      scanStep = scanStep + 7;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005067D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 1, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection1, TerrainProjectedOcclusion_ScanDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge1
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int heightOrRowStride;
  int neighborHeight;
  uint projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightOrRowStride = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        heightOrRowStride = heightOrRowStride + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(heightOrRowStride - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      heightOrRowStride = g_TerrainScanRowStrideBytes;
      projectedHeightOrStep = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((*(uint *)((int)cell + (0x50 - heightOrRowStride)) & 0x88006000) != 0) {
        return;
      }
      neighborHeight = *(int *)((int)cell + (0x48 - heightOrRowStride));
      if (0 < *(int *)((int)cell + (0x4c - heightOrRowStride))) {
        neighborHeight = neighborHeight + *(int *)((int)cell + (0x4c - heightOrRowStride));
      }
      scaledHeightProduct = (longlong)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(ulonglong *)((int)cell + (0x70 - heightOrRowStride)) =
             *(ulonglong *)((int)cell + (0x70 - heightOrRowStride)) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      adjacentCell = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - heightOrRowStride));
      scanStep = scanStep + 7;
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


/* Address: 0x005068D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 2, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection2, TerrainProjectedOcclusion_ScanDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge2
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      projectedHeightOrStep = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell[-1].terrainHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        cellHeight = cellHeight + cell[-1].waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        cell[-1].occupancyMask = cell[-1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      adjacentCell = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
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


/* Address: 0x005069D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 3, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection3, TerrainProjectedOcclusion_ScanDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge3
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int rowStrideBytes;
  FieldCellPersistedAux cellHeight;
  int neighborHeight;
  uint projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(int)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = cell + -1;
      projectedHeightOrStep = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,adjacentCell);
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((*(uint *)(adjacentCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      neighborHeight = *(int *)(adjacentCell->runtime60_6B + rowStrideBytes + -0x18);
      if (0 < *(int *)(adjacentCell->runtime60_6B + rowStrideBytes + -0x14)) {
        neighborHeight = neighborHeight + *(int *)(adjacentCell->runtime60_6B + rowStrideBytes + -0x14);
      }
      scaledHeightProduct = (longlong)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(ulonglong *)(adjacentCell->runtime60_6B + rowStrideBytes + 0x10) =
             *(ulonglong *)(adjacentCell->runtime60_6B + rowStrideBytes + 0x10) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      cell = (FieldGridCell *)(adjacentCell[-1].runtime0C_3F + rowStrideBytes + -0xc);
      scanStep = scanStep + 7;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506AD0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 4, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection4, TerrainProjectedOcclusion_ScanDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge4
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  byte *cellRuntimeBase;
  int heightOrRowStride;
  int neighborHeight;
  uint projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightOrRowStride = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        heightOrRowStride = heightOrRowStride + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(heightOrRowStride - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      heightOrRowStride = g_TerrainScanRowStrideBytes;
      projectedHeightOrStep = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((*(uint *)(cell->runtime60_6B + heightOrRowStride + -0x10) & 0x88006000) != 0) {
        return;
      }
      neighborHeight = *(int *)(cell->runtime60_6B + heightOrRowStride + -0x18);
      if (0 < *(int *)(cell->runtime60_6B + heightOrRowStride + -0x14)) {
        neighborHeight = neighborHeight + *(int *)(cell->runtime60_6B + heightOrRowStride + -0x14);
      }
      scaledHeightProduct = (longlong)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(ulonglong *)(cell->runtime60_6B + heightOrRowStride + 0x10) =
             *(ulonglong *)(cell->runtime60_6B + heightOrRowStride + 0x10) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      cellRuntimeBase = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cellRuntimeBase + g_TerrainScanRowStrideBytes + heightOrRowStride + -0xc) + -1;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)(cellRuntimeBase + g_TerrainScanRowStrideBytes + heightOrRowStride + -0xc));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506BD0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 5, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection5, TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge5
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      projectedHeightOrStep = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell[1].terrainHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        cellHeight = cellHeight + cell[1].waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        cell[1].occupancyMask = cell[1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      adjacentCell = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
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


/* Address: 0x00509580.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection0, FieldGridTerrainOverlayVariantA_ApplyDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep + 4,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)adjacentCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)((int)adjacentCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)adjacentCell + (0x4c - rowStrideBytes)) < 0)) {
        *(dword *)((int)adjacentCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)adjacentCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509640.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection1, FieldGridTerrainOverlayVariantA_ApplyDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep + 4,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell + (0x4c - rowStrideBytes)) < 0)) {
        *(dword *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      adjacentCell = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
      fieldCell = adjacentCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection2(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005096F0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection2, FieldGridTerrainOverlayVariantA_ApplyDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell[-1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) !=
           0) && (fieldCell[-1].waterSurfaceDelta < 0)) {
        fieldCell[-1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + -2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)((int)fieldCell + (-0x80 - g_TerrainScanRowStrideBytes));
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005097A0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection3, FieldGridTerrainOverlayVariantA_ApplyDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  int scanRowStrideBytes;
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanRowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + -1;
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep + 4,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x14) < 0)) {
        *(dword *)(adjacentCell->runtime0C_3F + scanRowStrideBytes + -8) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(adjacentCell[-1].runtime0C_3F + scanRowStrideBytes + -0xc);
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep,(FieldGridCell *)
                          (fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509860.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection4, FieldGridTerrainOverlayVariantA_ApplyDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  byte *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell->runtime60_6B + rowStrideBytes + -0x14) < 0)) {
        *(dword *)(fieldCell->runtime0C_3F + rowStrideBytes + -8) = g_TerrainScanReferenceHeight;
      }
      currentCellRuntimeBase = fieldCell->runtime0C_3F;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) +
                  -1;
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase +
                          g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509910.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection5, FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell[1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0
          ) && (fieldCell[1].waterSurfaceDelta < 0)) {
        fieldCell[1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + 2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509DD0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection0, FieldGridTerrainOverlayVariantB_ApplyDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep + 4,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)adjacentCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)adjacentCell + (0x4c - rowStrideBytes))) {
        *(dword *)((int)adjacentCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)adjacentCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509E70.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection1, FieldGridTerrainOverlayVariantB_ApplyDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep + 4,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell + (0x4c - rowStrideBytes))) {
        *(dword *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      adjacentCell = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
      fieldCell = adjacentCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection2(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509F10.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection2, FieldGridTerrainOverlayVariantB_ApplyDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell[-1].waterSurfaceDelta) {
        fieldCell[-1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + -2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)((int)fieldCell + (-0x80 - g_TerrainScanRowStrideBytes));
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509FB0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection3, FieldGridTerrainOverlayVariantB_ApplyDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  int scanRowStrideBytes;
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanRowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + -1;
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep + 4,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x14)) {
        *(dword *)(adjacentCell->runtime0C_3F + scanRowStrideBytes + -8) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(adjacentCell[-1].runtime0C_3F + scanRowStrideBytes + -0xc);
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep,(FieldGridCell *)
                          (fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x0050A050.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection4, FieldGridTerrainOverlayVariantB_ApplyDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  byte *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell->runtime60_6B + rowStrideBytes + -0x14)) {
        *(dword *)(fieldCell->runtime0C_3F + rowStrideBytes + -8) = g_TerrainScanReferenceHeight;
      }
      currentCellRuntimeBase = fieldCell->runtime0C_3F;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) +
                  -1;
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase +
                          g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x0050A0F0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection5, FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell[1].waterSurfaceDelta) {
        fieldCell[1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + 2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00500CE0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected quad queue as two triangles register result.
   Local calls: TerrainProjectedTriangle_ClipInterpolateAndQueueTextured.
*/
void __thandor_void_preserve_ecx_edx
TerrainProjectedQuad_QueueAsTwoTrianglesRegs
          (dword rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             (TerrainProjectedVertexWorkRecord *)
             (topLeftVertex->reserved08_0B + (rowStrideBytes - 8)),topLeftVertex,renderContext);
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             (TerrainProjectedVertexWorkRecord *)
             (topLeftVertex[1].reserved08_0B + (rowStrideBytes - 8)),
             (TerrainProjectedVertexWorkRecord *)
             (topLeftVertex->reserved08_0B + (rowStrideBytes - 8)),renderContext);
  return;
}


/* Address: 0x005004A0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected vertex transform project and shade variant a.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime], GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
   [graphics/render/shading].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedVertex_TransformProjectAndShadeVariantA(TerrainProjectedVertexWorkRecord *vertex)

{
  GraphicsWorldCoordinateQ12 *sourceCoordinate;
  PackedArgb32 vertexColor;
  PackedArgb32 baseColor;
  GraphicsFixedVec3 *offsetVector;
  int offsetX;
  int offsetY;
  short shadedBlue;
  short shadedGreen;
  short shadedRed;
  short shadedAlpha;
  ushort vertexAlphaPair;
  ushort baseAlphaPair;
  int offsetZ;
  uint resultFlags;
  uint pointAFlags;
  undefined1 vertexAlphaOrGreen;
  undefined1 vertexRed;
  MmxPackedValue64 lightingFactors;
  undefined8 shadedProduct;
  undefined1 baseAlphaOrGreen;
  undefined1 baseRed;
  GraphicsProjectedPointPair projectedPoint;
  
  resultFlags = vertex->projectionFlags & 0xe801ffff;
  if ((vertex->projectionFlags & 0xff) != 0xff) {
    pointAFlags = resultFlags | 0x200000;
    FixedTransform_ApplyPoint(&vertex->viewPointA,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed)
    ;
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointA).z) {
      projectedPoint = Graphics_ProjectViewPoint(&vertex->viewPointA);
      vertex->projectedPointA = projectedPoint;
      pointAFlags = resultFlags;
      if (g_ProjectionClipRect.minX <= projectedPoint.projectedX) {
        pointAFlags = resultFlags | 0x20000;
      }
      if (projectedPoint.projectedX < g_ProjectionClipRect.maxX) {
        pointAFlags = pointAFlags | 0x80000;
      }
      if (g_ProjectionClipRect.minY <= projectedPoint.projectedY) {
        pointAFlags = pointAFlags | 0x40000;
      }
      if (projectedPoint.projectedY < g_ProjectionClipRect.maxY) {
        pointAFlags = pointAFlags | 0x100000;
      }
    }
    vertexColor = vertex->packedColorA;
    baseColor = vertex->basePackedColor;
    vertexAlphaOrGreen = (undefined1)(vertexColor >> 0x18);
    vertexAlphaPair = CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen);
    vertexRed = (undefined1)(vertexColor >> 0x10);
    vertexAlphaOrGreen = (undefined1)(vertexColor >> 8);
    baseAlphaOrGreen = (undefined1)(baseColor >> 0x18);
    baseAlphaPair = CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen);
    baseRed = (undefined1)(baseColor >> 0x10);
    baseAlphaOrGreen = (undefined1)(baseColor >> 8);
    lightingFactors = CONCAT26(vertexAlphaPair >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(vertexAlphaPair,vertexRed),CONCAT14(vertexRed,vertexColor)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen) >> 6,
                                        CONCAT11((char)vertexColor,(char)vertexColor) >> 6)));
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointA,lightingFactors);
    }
    shadedProduct = pmulhw(lightingFactors,CONCAT26(baseAlphaPair >> 2,
                                    CONCAT24((ushort)(CONCAT35(CONCAT21(baseAlphaPair,baseRed),
                                                               CONCAT14(baseRed,baseColor)) >> 0x20) >> 2
                                             ,CONCAT22(CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen) >> 2,
                                                       CONCAT11((char)baseColor,(char)baseColor) >> 2))));
    shadedBlue = (short)shadedProduct;
    shadedGreen = (short)((ulonglong)shadedProduct >> 0x10);
    shadedRed = (short)((ulonglong)shadedProduct >> 0x20);
    shadedAlpha = (short)((ulonglong)shadedProduct >> 0x30);
    offsetVector = vertex->secondaryOffset;
    vertex->shadedColorA =
         CONCAT13((0 < shadedAlpha) * (shadedAlpha < 0x100) * (char)((ulonglong)shadedProduct >> 0x30) -
                  (0xff < shadedAlpha),
                  CONCAT12((0 < shadedRed) * (shadedRed < 0x100) * (char)((ulonglong)shadedProduct >> 0x20) -
                           (0xff < shadedRed),
                           CONCAT11((0 < shadedGreen) * (shadedGreen < 0x100) *
                                    (char)((ulonglong)shadedProduct >> 0x10) - (0xff < shadedGreen),
                                    (0 < shadedBlue) * (shadedBlue < 0x100) * (char)shadedProduct - (0xff < shadedBlue))))
    ;
    resultFlags = pointAFlags | 0x4000000;
    offsetX = offsetVector->x;
    offsetY = offsetVector->y;
    offsetZ = offsetVector->z;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x + offsetX;
    offsetZ = offsetZ + vertex->secondaryProjectionDepthQ12;
    sourceCoordinate = &(vertex->sourcePoint).y;
    *sourceCoordinate = *sourceCoordinate + offsetY;
    sourceCoordinate = &(vertex->sourcePoint).z;
    *sourceCoordinate = *sourceCoordinate + offsetZ;
    FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed)
    ;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x - offsetX;
    sourceCoordinate = &(vertex->sourcePoint).y;
    *sourceCoordinate = *sourceCoordinate - offsetY;
    sourceCoordinate = &(vertex->sourcePoint).z;
    *sourceCoordinate = *sourceCoordinate - offsetZ;
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointB).z) {
      projectedPoint = Graphics_ProjectViewPoint(&vertex->viewPointB);
      vertex->projectedPointB = projectedPoint;
      resultFlags = pointAFlags;
      if (g_ProjectionClipRect.minX <= projectedPoint.projectedX) {
        resultFlags = pointAFlags | 0x400000;
      }
      if (projectedPoint.projectedX < g_ProjectionClipRect.maxX) {
        resultFlags = resultFlags | 0x1000000;
      }
      if (g_ProjectionClipRect.minY <= projectedPoint.projectedY) {
        resultFlags = resultFlags | 0x800000;
      }
      if (projectedPoint.projectedY < g_ProjectionClipRect.maxY) {
        resultFlags = resultFlags | 0x2000000;
      }
    }
    vertexColor = vertex->packedColorB;
    baseColor = vertex->basePackedColor;
    vertexAlphaOrGreen = (undefined1)(vertexColor >> 0x18);
    vertexAlphaPair = CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen);
    vertexRed = (undefined1)(vertexColor >> 0x10);
    vertexAlphaOrGreen = (undefined1)(vertexColor >> 8);
    baseAlphaOrGreen = (undefined1)(baseColor >> 0x18);
    baseAlphaPair = CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen);
    baseRed = (undefined1)(baseColor >> 0x10);
    baseAlphaOrGreen = (undefined1)(baseColor >> 8);
    lightingFactors = CONCAT26(vertexAlphaPair >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(vertexAlphaPair,vertexRed),CONCAT14(vertexRed,vertexColor)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen) >> 6,
                                        CONCAT11((char)vertexColor,(char)vertexColor) >> 6)));
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointB,lightingFactors);
    }
    shadedProduct = pmulhw(lightingFactors,CONCAT26(baseAlphaPair >> 2,
                                    CONCAT24((ushort)(CONCAT35(CONCAT21(baseAlphaPair,baseRed),
                                                               CONCAT14(baseRed,baseColor)) >> 0x20) >> 2
                                             ,CONCAT22(CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen) >> 2,
                                                       CONCAT11((char)baseColor,(char)baseColor) >> 2))));
    shadedBlue = (short)shadedProduct;
    shadedGreen = (short)((ulonglong)shadedProduct >> 0x10);
    shadedRed = (short)((ulonglong)shadedProduct >> 0x20);
    shadedAlpha = (short)((ulonglong)shadedProduct >> 0x30);
    vertex->projectionFlags = resultFlags;
    vertex->shadedColorB =
         CONCAT13((0 < shadedAlpha) * (shadedAlpha < 0x100) * (char)((ulonglong)shadedProduct >> 0x30) -
                  (0xff < shadedAlpha),
                  CONCAT12((0 < shadedRed) * (shadedRed < 0x100) * (char)((ulonglong)shadedProduct >> 0x20) -
                           (0xff < shadedRed),
                           CONCAT11((0 < shadedGreen) * (shadedGreen < 0x100) *
                                    (char)((ulonglong)shadedProduct >> 0x10) - (0xff < shadedGreen),
                                    (0 < shadedBlue) * (shadedBlue < 0x100) * (char)shadedProduct - (0xff < shadedBlue))))
    ;
  }
  return;
}


/* Address: 0x005006A0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected vertex transform project and shade variant b.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime], GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
   [graphics/render/shading].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedVertex_TransformProjectAndShadeVariantB(TerrainProjectedVertexWorkRecord *vertex)

{
  GraphicsWorldCoordinateQ12 *sourceCoordinate;
  GraphicsFixedVec3 *offsetVector;
  int offsetX;
  int offsetY;
  PackedArgb32 vertexColor;
  PackedArgb32 baseColor;
  short shadedBlue;
  short shadedGreen;
  short shadedRed;
  short shadedAlpha;
  ushort vertexAlphaPair;
  ushort baseAlphaPair;
  int offsetZ;
  uint maskedFlags;
  uint resultFlags;
  undefined1 vertexAlphaOrGreen;
  undefined1 vertexRed;
  MmxPackedValue64 lightingFactors;
  undefined8 shadedProduct;
  undefined1 baseAlphaOrGreen;
  undefined1 baseRed;
  GraphicsProjectedPointPair projectedPoint;
  
  resultFlags = vertex->projectionFlags;
  offsetVector = vertex->secondaryOffset;
  if ((resultFlags & 0x10000000) != 0) {
    maskedFlags = resultFlags & 0xf83fffff;
    resultFlags = maskedFlags | 0x4000000;
    offsetX = offsetVector->x;
    offsetY = offsetVector->y;
    offsetZ = offsetVector->z;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x + offsetX;
    offsetZ = offsetZ + vertex->secondaryProjectionDepthQ12;
    sourceCoordinate = &(vertex->sourcePoint).y;
    *sourceCoordinate = *sourceCoordinate + offsetY;
    sourceCoordinate = &(vertex->sourcePoint).z;
    *sourceCoordinate = *sourceCoordinate + offsetZ;
    FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed)
    ;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x - offsetX;
    sourceCoordinate = &(vertex->sourcePoint).y;
    *sourceCoordinate = *sourceCoordinate - offsetY;
    sourceCoordinate = &(vertex->sourcePoint).z;
    *sourceCoordinate = *sourceCoordinate - offsetZ;
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointB).z) {
      projectedPoint = Graphics_ProjectViewPoint(&vertex->viewPointB);
      vertex->projectedPointB = projectedPoint;
      resultFlags = maskedFlags;
      if (g_ProjectionClipRect.minX <= projectedPoint.projectedX) {
        resultFlags = maskedFlags | 0x400000;
      }
      if (projectedPoint.projectedX < g_ProjectionClipRect.maxX) {
        resultFlags = resultFlags | 0x1000000;
      }
      if (g_ProjectionClipRect.minY <= projectedPoint.projectedY) {
        resultFlags = resultFlags | 0x800000;
      }
      if (projectedPoint.projectedY < g_ProjectionClipRect.maxY) {
        resultFlags = resultFlags | 0x2000000;
      }
    }
    vertexColor = vertex->packedColorB;
    baseColor = vertex->basePackedColor;
    vertexAlphaOrGreen = (undefined1)(vertexColor >> 0x18);
    vertexAlphaPair = CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen);
    vertexRed = (undefined1)(vertexColor >> 0x10);
    vertexAlphaOrGreen = (undefined1)(vertexColor >> 8);
    baseAlphaOrGreen = (undefined1)(baseColor >> 0x18);
    baseAlphaPair = CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen);
    baseRed = (undefined1)(baseColor >> 0x10);
    baseAlphaOrGreen = (undefined1)(baseColor >> 8);
    lightingFactors = CONCAT26(vertexAlphaPair >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(vertexAlphaPair,vertexRed),CONCAT14(vertexRed,vertexColor)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen) >> 6,
                                        CONCAT11((char)vertexColor,(char)vertexColor) >> 6)));
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointB,lightingFactors);
    }
    shadedProduct = pmulhw(lightingFactors,CONCAT26(baseAlphaPair >> 2,
                                    CONCAT24((ushort)(CONCAT35(CONCAT21(baseAlphaPair,baseRed),
                                                               CONCAT14(baseRed,baseColor)) >> 0x20) >> 2
                                             ,CONCAT22(CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen) >> 2,
                                                       CONCAT11((char)baseColor,(char)baseColor) >> 2))));
    shadedBlue = (short)shadedProduct;
    shadedGreen = (short)((ulonglong)shadedProduct >> 0x10);
    shadedRed = (short)((ulonglong)shadedProduct >> 0x20);
    shadedAlpha = (short)((ulonglong)shadedProduct >> 0x30);
    vertex->shadedColorB =
         CONCAT13((0 < shadedAlpha) * (shadedAlpha < 0x100) * (char)((ulonglong)shadedProduct >> 0x30) -
                  (0xff < shadedAlpha),
                  CONCAT12((0 < shadedRed) * (shadedRed < 0x100) * (char)((ulonglong)shadedProduct >> 0x20) -
                           (0xff < shadedRed),
                           CONCAT11((0 < shadedGreen) * (shadedGreen < 0x100) *
                                    (char)((ulonglong)shadedProduct >> 0x10) - (0xff < shadedGreen),
                                    (0 < shadedBlue) * (shadedBlue < 0x100) * (char)shadedProduct - (0xff < shadedBlue))))
    ;
  }
  vertexColor = vertex->packedColorA;
  baseColor = vertex->basePackedColor;
  vertexAlphaOrGreen = (undefined1)(vertexColor >> 0x18);
  vertexAlphaPair = CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen);
  vertexRed = (undefined1)(vertexColor >> 0x10);
  vertexAlphaOrGreen = (undefined1)(vertexColor >> 8);
  baseAlphaOrGreen = (undefined1)(baseColor >> 0x18);
  baseAlphaPair = CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen);
  baseRed = (undefined1)(baseColor >> 0x10);
  baseAlphaOrGreen = (undefined1)(baseColor >> 8);
  lightingFactors = CONCAT26(vertexAlphaPair >> 6,
                    CONCAT24((ushort)(CONCAT35(CONCAT21(vertexAlphaPair,vertexRed),CONCAT14(vertexRed,vertexColor)) >>
                                     0x20) >> 6,
                             CONCAT22(CONCAT11(vertexAlphaOrGreen,vertexAlphaOrGreen) >> 6,
                                      CONCAT11((char)vertexColor,(char)vertexColor) >> 6)));
  if (vertex->lightingLookupIndexOrSentinel == 0xff) {
    lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                       (&vertex->viewPointA,lightingFactors);
  }
  shadedProduct = pmulhw(lightingFactors,CONCAT26(baseAlphaPair >> 2,
                                  CONCAT24((ushort)(CONCAT35(CONCAT21(baseAlphaPair,baseRed),
                                                             CONCAT14(baseRed,baseColor)) >> 0x20) >> 2,
                                           CONCAT22(CONCAT11(baseAlphaOrGreen,baseAlphaOrGreen) >> 2,
                                                    CONCAT11((char)baseColor,(char)baseColor) >> 2))));
  shadedBlue = (short)shadedProduct;
  shadedGreen = (short)((ulonglong)shadedProduct >> 0x10);
  shadedRed = (short)((ulonglong)shadedProduct >> 0x20);
  shadedAlpha = (short)((ulonglong)shadedProduct >> 0x30);
  vertex->shadedColorA =
       CONCAT13((0 < shadedAlpha) * (shadedAlpha < 0x100) * (char)((ulonglong)shadedProduct >> 0x30) -
                (0xff < shadedAlpha),
                CONCAT12((0 < shadedRed) * (shadedRed < 0x100) * (char)((ulonglong)shadedProduct >> 0x20) -
                         (0xff < shadedRed),
                         CONCAT11((0 < shadedGreen) * (shadedGreen < 0x100) * (char)((ulonglong)shadedProduct >> 0x10)
                                  - (0xff < shadedGreen),
                                  (0 < shadedBlue) * (shadedBlue < 0x100) * (char)shadedProduct - (0xff < shadedBlue))));
  vertex->projectionFlags = resultFlags;
  return;
}


/* Address: 0x00500820.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected triangle clip interpolate and queue textured.
   Cross-module calls: Triangle2D_ComputeBarycentricWeightsQ12Packed [core/math/geometry],
   GraphicsPrimitiveQueue_AppendTexturedTriangleRegs [graphics/render/primitives],
   GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
          (int surfacePacketIndex,TerrainProjectedVertexWorkRecord *vertex2,
          TerrainProjectedVertexWorkRecord *vertex1,TerrainProjectedVertexWorkRecord *vertex0,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  GraphicsPrimitiveDispatchFlags *packetRenderFlags;
  dword vertex0ViewDepth;
  short channelWord0;
  short channelWord1;
  short channelWord2;
  short channelWord3;
  short channelWord4;
  short channelWord5;
  short channelWord6;
  short channelWord7;
  short channelWord8;
  short channelWord9;
  short channelWord10;
  short channelWord11;
  ushort color0AlphaPair;
  ushort color1AlphaPair;
  ushort color2AlphaPair;
  void *soilPacketTable;
  uint flagsOrClampedDepth0;
  int yOrTableIndexC;
  int yOrTableIndexA;
  uint clampedDepth2;
  int yOrTableIndexB;
  uint clampedDepth1;
  int materialOffset1;
  int materialOffset0;
  bool outsideTriangle;
  PackedArgb32 vertex0Color;
  undefined1 color0AlphaOrGreen;
  undefined1 color0Red;
  undefined8 litProduct0;
  PackedArgb32 vertex1Color;
  undefined1 color1AlphaOrGreen;
  undefined1 color1Red;
  undefined8 litProduct1;
  PackedArgb32 vertex2Color;
  undefined1 color2AlphaOrGreen;
  undefined1 color2Red;
  undefined8 litProduct2;
  TriangleBarycentricWeightsQ12 barycentricWeights;
  GraphicsPrimitivePacketEaxCf5 queuedPacket;
  TerrainProjectedVertexWorkRecord *vertex2Projected;
  TerrainProjectedVertexWorkRecord *vertex1Projected;
  TerrainProjectedVertexWorkRecord *vertex0Projected;
  FrontendModelPointerContextRuntimeState17C *savedRenderContext;
  
  flagsOrClampedDepth0 = vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags;
  if ((flagsOrClampedDepth0 & 0xff) != 0xff) {
    if ((flagsOrClampedDepth0 & 0x3e0000) == 0x1e0000) {
      outsideTriangle = false;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) == 0) {
        barycentricWeights = Triangle2D_ComputeBarycentricWeightsQ12Packed
                           ((vertex2->projectedPointA).projectedY,
                            (vertex2->projectedPointA).projectedX,
                            (vertex1->projectedPointA).projectedY,
                            (vertex1->projectedPointA).projectedX,
                            (vertex0->projectedPointA).projectedY,
                            (vertex0->projectedPointA).projectedX,renderContext->cursorWorldYQ12,
                            renderContext->cursorWorldXQ12);
        outsideTriangle = g_Triangle2DBarycentricOutside; /* the original's JC after the call */
        vertex0ViewDepth = (vertex0->viewPointA).z;
        if ((!outsideTriangle) && ((int)vertex0ViewDepth < (int)renderContext->callbackArgumentF0)) {
          renderContext->callbackArgumentF0 = vertex0ViewDepth;
          yOrTableIndexA = (vertex1->sourcePoint).y;
          yOrTableIndexB = (vertex0->sourcePoint).y;
          yOrTableIndexC = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               (((vertex1->sourcePoint).x - (vertex0->sourcePoint).x) * barycentricWeights.weightVertexB_Q12 >>
               0xc) + (vertex0->sourcePoint).x;
          renderContext->callbackArgumentEC =
               ((yOrTableIndexA - yOrTableIndexB) * barycentricWeights.weightVertexB_Q12 >> 0xc) + yOrTableIndexC;
          yOrTableIndexA = (vertex2->sourcePoint).y;
          yOrTableIndexB = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               renderContext->callbackArgumentE8 +
               (((vertex2->sourcePoint).x - (vertex0->sourcePoint).x) * barycentricWeights.weightVertexA_Q12 >>
               0xc);
          renderContext->callbackArgumentEC =
               renderContext->callbackArgumentEC +
               ((yOrTableIndexA - yOrTableIndexB) * barycentricWeights.weightVertexA_Q12 >> 0xc);
        }
      }
      soilPacketTable = g_TerrainSoilPacketTablePayload;
      vertex0Color = vertex0->shadedColorA;
      vertex1Color = vertex1->shadedColorA;
      vertex2Color = vertex2->shadedColorA;
      color0AlphaOrGreen = (undefined1)(vertex0Color >> 0x18);
      color0AlphaPair = CONCAT11(color0AlphaOrGreen,color0AlphaOrGreen);
      color0Red = (undefined1)(vertex0Color >> 0x10);
      color0AlphaOrGreen = (undefined1)(vertex0Color >> 8);
      color1AlphaOrGreen = (undefined1)(vertex1Color >> 0x18);
      color1AlphaPair = CONCAT11(color1AlphaOrGreen,color1AlphaOrGreen);
      color1Red = (undefined1)(vertex1Color >> 0x10);
      color1AlphaOrGreen = (undefined1)(vertex1Color >> 8);
      color2AlphaOrGreen = (undefined1)(vertex2Color >> 0x18);
      color2AlphaPair = CONCAT11(color2AlphaOrGreen,color2AlphaOrGreen);
      color2Red = (undefined1)(vertex2Color >> 0x10);
      color2AlphaOrGreen = (undefined1)(vertex2Color >> 8);
      flagsOrClampedDepth0 = vertex0->secondaryProjectionDepthQ12;
      clampedDepth1 = vertex1->secondaryProjectionDepthQ12;
      clampedDepth2 = vertex2->secondaryProjectionDepthQ12;
      if ((int)flagsOrClampedDepth0 < 0) {
        flagsOrClampedDepth0 = 0;
      }
      if ((int)clampedDepth1 < 0) {
        clampedDepth1 = 0;
      }
      if ((int)clampedDepth2 < 0) {
        clampedDepth2 = 0;
      }
      yOrTableIndexA = vertex0->lightingLookupIndexOrSentinel - (flagsOrClampedDepth0 >> 1);
      if (yOrTableIndexA < 0) {
        yOrTableIndexA = 0;
      }
      yOrTableIndexB = vertex1->lightingLookupIndexOrSentinel - (clampedDepth1 >> 1);
      if (yOrTableIndexB < 0) {
        yOrTableIndexB = 0;
      }
      yOrTableIndexC = vertex2->lightingLookupIndexOrSentinel - (clampedDepth2 >> 1);
      if (yOrTableIndexC < 0) {
        yOrTableIndexC = 0;
      }
      litProduct0 = pmulhw(CONCAT26(color0AlphaPair >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(color0AlphaPair,color0Red),
                                                          CONCAT14(color0Red,vertex0Color)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(color0AlphaOrGreen,color0AlphaOrGreen) >> 4,
                                                 CONCAT11((char)vertex0Color,(char)vertex0Color) >> 4))),
                      g_PackedLightingLookupTable[yOrTableIndexA]);
      litProduct1 = pmulhw(CONCAT26(color1AlphaPair >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(color1AlphaPair,color1Red),
                                                          CONCAT14(color1Red,vertex1Color)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(color1AlphaOrGreen,color1AlphaOrGreen) >> 4,
                                                 CONCAT11((char)vertex1Color,(char)vertex1Color) >> 4))),
                      g_PackedLightingLookupTable[yOrTableIndexB]);
      litProduct2 = pmulhw(CONCAT26(color2AlphaPair >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(color2AlphaPair,color2Red),
                                                          CONCAT14(color2Red,vertex2Color)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(color2AlphaOrGreen,color2AlphaOrGreen) >> 4,
                                                 CONCAT11((char)vertex2Color,(char)vertex2Color) >> 4))),
                      g_PackedLightingLookupTable[yOrTableIndexC]);
      channelWord0 = (short)litProduct0;
      channelWord1 = (short)((ulonglong)litProduct0 >> 0x10);
      channelWord2 = (short)((ulonglong)litProduct0 >> 0x20);
      channelWord3 = (short)((ulonglong)litProduct0 >> 0x30);
      vertex0Color = CONCAT13((0 < channelWord3) * (channelWord3 < 0x100) * (char)((ulonglong)litProduct0 >> 0x30) -
                        (0xff < channelWord3),
                        CONCAT12((0 < channelWord2) * (channelWord2 < 0x100) * (char)((ulonglong)litProduct0 >> 0x20) -
                                 (0xff < channelWord2),
                                 CONCAT11((0 < channelWord1) * (channelWord1 < 0x100) *
                                          (char)((ulonglong)litProduct0 >> 0x10) - (0xff < channelWord1),
                                          (0 < channelWord0) * (channelWord0 < 0x100) * (char)litProduct0 -
                                          (0xff < channelWord0))));
      channelWord0 = (short)litProduct1;
      channelWord1 = (short)((ulonglong)litProduct1 >> 0x10);
      channelWord2 = (short)((ulonglong)litProduct1 >> 0x20);
      channelWord3 = (short)((ulonglong)litProduct1 >> 0x30);
      vertex1Color = CONCAT13((0 < channelWord3) * (channelWord3 < 0x100) * (char)((ulonglong)litProduct1 >> 0x30) -
                        (0xff < channelWord3),
                        CONCAT12((0 < channelWord2) * (channelWord2 < 0x100) * (char)((ulonglong)litProduct1 >> 0x20) -
                                 (0xff < channelWord2),
                                 CONCAT11((0 < channelWord1) * (channelWord1 < 0x100) *
                                          (char)((ulonglong)litProduct1 >> 0x10) - (0xff < channelWord1),
                                          (0 < channelWord0) * (channelWord0 < 0x100) * (char)litProduct1 -
                                          (0xff < channelWord0))));
      channelWord0 = (short)litProduct2;
      channelWord1 = (short)((ulonglong)litProduct2 >> 0x10);
      channelWord2 = (short)((ulonglong)litProduct2 >> 0x20);
      channelWord3 = (short)((ulonglong)litProduct2 >> 0x30);
      vertex2Color = CONCAT13((0 < channelWord3) * (channelWord3 < 0x100) * (char)((ulonglong)litProduct2 >> 0x30) -
                        (0xff < channelWord3),
                        CONCAT12((0 < channelWord2) * (channelWord2 < 0x100) * (char)((ulonglong)litProduct2 >> 0x20) -
                                 (0xff < channelWord2),
                                 CONCAT11((0 < channelWord1) * (channelWord1 < 0x100) *
                                          (char)((ulonglong)litProduct2 >> 0x10) - (0xff < channelWord1),
                                          (0 < channelWord0) * (channelWord0 < 0x100) * (char)litProduct2 -
                                          (0xff < channelWord0))));
      materialOffset0 = (vertex0->projectionFlags & 0xff) * 0x800;
      materialOffset1 = (vertex1->projectionFlags & 0xff) * 0x800;
      yOrTableIndexC = (vertex2->projectionFlags & 0xff) * 0x800;
      yOrTableIndexA = materialOffset1 + (vertex1->projectionFlags & 0x700);
      yOrTableIndexB = yOrTableIndexC + (vertex2->projectionFlags & 0x700);
      vertex2Projected = vertex2;
      vertex1Projected = vertex1;
      vertex0Projected = vertex0;
      savedRenderContext = renderContext;
      queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                         ((dword *)((int)g_TerrainSoilPacketTablePayload +
                                   materialOffset0 + (vertex0->projectionFlags & 0x700)),vertex2Color,vertex1Color,vertex0Color
                          ,(GraphicsProjectedVertexSource *)vertex2,
                          (GraphicsProjectedVertexSource *)vertex1,
                          (GraphicsProjectedVertexSource *)vertex0,renderContext);
      if (!queuedPacket.carry) {
        if (materialOffset0 == materialOffset1) {
          if (materialOffset0 != yOrTableIndexC) {
            queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               ((dword *)((int)soilPacketTable + yOrTableIndexB + 0x20),vertex2Color,vertex1Color,vertex0Color,
                                (GraphicsProjectedVertexSource *)vertex2Projected,
                                (GraphicsProjectedVertexSource *)vertex1Projected,
                                (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
            if (!queuedPacket.carry) {
              packetRenderFlags = &(queuedPacket.packet)->renderFlags;
              *packetRenderFlags = *packetRenderFlags | 0x10020000;
            }
          }
        }
        else if (materialOffset0 == yOrTableIndexC) {
          queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((dword *)((int)soilPacketTable + yOrTableIndexA + 0x40),vertex2Color,vertex1Color,vertex0Color,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
          if (!queuedPacket.carry) {
            packetRenderFlags = &(queuedPacket.packet)->renderFlags;
            *packetRenderFlags = *packetRenderFlags | 0x10020000;
          }
        }
        else if (materialOffset1 == yOrTableIndexC) {
          queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((dword *)((int)soilPacketTable + yOrTableIndexA + 0x60),vertex2Color,vertex1Color,vertex0Color,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
          if (!queuedPacket.carry) {
            packetRenderFlags = &(queuedPacket.packet)->renderFlags;
            *packetRenderFlags = *packetRenderFlags | 0x10020000;
          }
        }
        else {
          queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((dword *)((int)soilPacketTable + yOrTableIndexA + 0x80),vertex2Color,vertex1Color,vertex0Color,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
          if (!queuedPacket.carry) {
            packetRenderFlags = &(queuedPacket.packet)->renderFlags;
            *packetRenderFlags = *packetRenderFlags | 0x10020000;
            queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               ((dword *)((int)soilPacketTable + yOrTableIndexB + 0xa0),vertex2Color,vertex1Color,vertex0Color,
                                (GraphicsProjectedVertexSource *)vertex2Projected,
                                (GraphicsProjectedVertexSource *)vertex1Projected,
                                (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
            if (!queuedPacket.carry) {
              packetRenderFlags = &(queuedPacket.packet)->renderFlags;
              *packetRenderFlags = *packetRenderFlags | 0x20020000;
            }
          }
        }
      }
    }
    if ((((((renderContext->contextFlags & 0x1000000) != 0) ||
          (0 < vertex0->secondaryProjectionDepthQ12)) || (0 < vertex1->secondaryProjectionDepthQ12))
        || (0 < vertex2->secondaryProjectionDepthQ12)) &&
       (((vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags) &
        0x7c00000) == 0x3c00000)) {
      vertex0->projectionFlags = vertex0->projectionFlags | 0x10000000;
      vertex1->projectionFlags = vertex1->projectionFlags | 0x10000000;
      vertex2->projectionFlags = vertex2->projectionFlags | 0x10000000;
      outsideTriangle = false;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
        barycentricWeights = Triangle2D_ComputeBarycentricWeightsQ12Packed
                           ((vertex2->projectedPointB).projectedY,
                            (vertex2->projectedPointB).projectedX,
                            (vertex1->projectedPointB).projectedY,
                            (vertex1->projectedPointB).projectedX,
                            (vertex0->projectedPointB).projectedY,
                            (vertex0->projectedPointB).projectedX,renderContext->cursorWorldYQ12,
                            renderContext->cursorWorldXQ12);
        outsideTriangle = g_Triangle2DBarycentricOutside; /* the original's JC after the call */
        vertex0ViewDepth = (vertex0->viewPointB).z;
        if ((!outsideTriangle) && ((int)vertex0ViewDepth < (int)renderContext->callbackArgumentF0)) {
          renderContext->callbackArgumentF0 = vertex0ViewDepth;
          yOrTableIndexA = (vertex1->sourcePoint).y;
          yOrTableIndexB = (vertex0->sourcePoint).y;
          yOrTableIndexC = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               (((vertex1->sourcePoint).x - (vertex0->sourcePoint).x) * barycentricWeights.weightVertexB_Q12 >>
               0xc) + (vertex0->sourcePoint).x;
          renderContext->callbackArgumentEC =
               ((yOrTableIndexA - yOrTableIndexB) * barycentricWeights.weightVertexB_Q12 >> 0xc) + yOrTableIndexC;
          yOrTableIndexA = (vertex2->sourcePoint).y;
          yOrTableIndexB = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               renderContext->callbackArgumentE8 +
               (((vertex2->sourcePoint).x - (vertex0->sourcePoint).x) * barycentricWeights.weightVertexA_Q12 >>
               0xc);
          renderContext->callbackArgumentEC =
               renderContext->callbackArgumentEC +
               ((yOrTableIndexA - yOrTableIndexB) * barycentricWeights.weightVertexA_Q12 >> 0xc);
        }
      }
      vertex0Color = vertex0->shadedColorB;
      vertex1Color = vertex1->shadedColorB;
      vertex2Color = vertex2->shadedColorB;
      color0AlphaOrGreen = (undefined1)(vertex0Color >> 0x18);
      color0AlphaPair = CONCAT11(color0AlphaOrGreen,color0AlphaOrGreen);
      color0Red = (undefined1)(vertex0Color >> 0x10);
      color0AlphaOrGreen = (undefined1)(vertex0Color >> 8);
      color1AlphaOrGreen = (undefined1)(vertex1Color >> 0x18);
      color1AlphaPair = CONCAT11(color1AlphaOrGreen,color1AlphaOrGreen);
      color1Red = (undefined1)(vertex1Color >> 0x10);
      color1AlphaOrGreen = (undefined1)(vertex1Color >> 8);
      color2AlphaOrGreen = (undefined1)(vertex2Color >> 0x18);
      color2AlphaPair = CONCAT11(color2AlphaOrGreen,color2AlphaOrGreen);
      color2Red = (undefined1)(vertex2Color >> 0x10);
      color2AlphaOrGreen = (undefined1)(vertex2Color >> 8);
      litProduct0 = pmulhw(CONCAT26(color0AlphaPair >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(color0AlphaPair,color0Red),
                                                          CONCAT14(color0Red,vertex0Color)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(color0AlphaOrGreen,color0AlphaOrGreen) >> 4,
                                                 CONCAT11((char)vertex0Color,(char)vertex0Color) >> 4))),
                      g_PackedLightingLookupTable[vertex0->lightingLookupIndexOrSentinel]);
      litProduct1 = pmulhw(CONCAT26(color1AlphaPair >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(color1AlphaPair,color1Red),
                                                          CONCAT14(color1Red,vertex1Color)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(color1AlphaOrGreen,color1AlphaOrGreen) >> 4,
                                                 CONCAT11((char)vertex1Color,(char)vertex1Color) >> 4))),
                      g_PackedLightingLookupTable[vertex1->lightingLookupIndexOrSentinel]);
      litProduct2 = pmulhw(CONCAT26(color2AlphaPair >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(color2AlphaPair,color2Red),
                                                          CONCAT14(color2Red,vertex2Color)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(color2AlphaOrGreen,color2AlphaOrGreen) >> 4,
                                                 CONCAT11((char)vertex2Color,(char)vertex2Color) >> 4))),
                      g_PackedLightingLookupTable[vertex2->lightingLookupIndexOrSentinel]);
      channelWord0 = (short)litProduct0;
      channelWord3 = (short)((ulonglong)litProduct0 >> 0x10);
      channelWord6 = (short)((ulonglong)litProduct0 >> 0x20);
      channelWord9 = (short)((ulonglong)litProduct0 >> 0x30);
      channelWord1 = (short)litProduct1;
      channelWord4 = (short)((ulonglong)litProduct1 >> 0x10);
      channelWord7 = (short)((ulonglong)litProduct1 >> 0x20);
      channelWord10 = (short)((ulonglong)litProduct1 >> 0x30);
      channelWord2 = (short)litProduct2;
      channelWord5 = (short)((ulonglong)litProduct2 >> 0x10);
      channelWord8 = (short)((ulonglong)litProduct2 >> 0x20);
      channelWord11 = (short)((ulonglong)litProduct2 >> 0x30);
      GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangleCf
                ((dword *)(surfacePacketIndex * 0x20 + (int)g_TerrainSurfacePacketTablePayload),
                 CONCAT13((0 < channelWord11) * (channelWord11 < 0x100) * (char)((ulonglong)litProduct2 >> 0x30) -
                          (0xff < channelWord11),
                          CONCAT12((0 < channelWord8) * (channelWord8 < 0x100) *
                                   (char)((ulonglong)litProduct2 >> 0x20) - (0xff < channelWord8),
                                   CONCAT11((0 < channelWord5) * (channelWord5 < 0x100) *
                                            (char)((ulonglong)litProduct2 >> 0x10) - (0xff < channelWord5),
                                            (0 < channelWord2) * (channelWord2 < 0x100) * (char)litProduct2 -
                                            (0xff < channelWord2)))),
                 CONCAT13((0 < channelWord10) * (channelWord10 < 0x100) * (char)((ulonglong)litProduct1 >> 0x30) -
                          (0xff < channelWord10),
                          CONCAT12((0 < channelWord7) * (channelWord7 < 0x100) *
                                   (char)((ulonglong)litProduct1 >> 0x20) - (0xff < channelWord7),
                                   CONCAT11((0 < channelWord4) * (channelWord4 < 0x100) *
                                            (char)((ulonglong)litProduct1 >> 0x10) - (0xff < channelWord4),
                                            (0 < channelWord1) * (channelWord1 < 0x100) * (char)litProduct1 -
                                            (0xff < channelWord1)))),
                 CONCAT13((0 < channelWord9) * (channelWord9 < 0x100) * (char)((ulonglong)litProduct0 >> 0x30) -
                          (0xff < channelWord9),
                          CONCAT12((0 < channelWord6) * (channelWord6 < 0x100) * (char)((ulonglong)litProduct0 >> 0x20)
                                   - (0xff < channelWord6),
                                   CONCAT11((0 < channelWord3) * (channelWord3 < 0x100) *
                                            (char)((ulonglong)litProduct0 >> 0x10) - (0xff < channelWord3),
                                            (0 < channelWord0) * (channelWord0 < 0x100) * (char)litProduct0 -
                                            (0xff < channelWord0)))),
                 (GraphicsProjectedVertexSource *)vertex2,(GraphicsProjectedVertexSource *)vertex1,
                 (GraphicsProjectedVertexSource *)vertex0,renderContext);
    }
  }
  return;
}


/* Address: 0x00500D30.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected grid clip row spans against plane.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedGrid_ClipRowSpansAgainstPlane
          (FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal)

{
  longlong fixedProduct;
  int boundOrCount;
  uint columnEdgeQ12;
  int cutoffRow;
  FieldGridDimension rowsRemaining;
  int *spanBoundCursor;
  TerrainProjectedRowSpan *spanCursor;
  
  if (planeNormal->x == 0) {
    if (planeNormal->y != 0) {
      if (planeNormal->y < 0) {
        boundOrCount = 0;
        if (-1 < planeNormal->z) {
          boundOrCount = (int)(((longlong)g_ViewOriginFixed.z * (longlong)planeNormal->z) /
                       (longlong)planeNormal->y);
        }
        fixedProduct = (longlong)(boundOrCount + g_ViewOriginFixed.y) * -0x20c8cc;
        cutoffRow = (int)((int)((ulonglong)fixedProduct >> 0x20) << 0xc | (uint)fixedProduct >> 0x14) >> 0xc;
        boundOrCount = fieldGrid->gridHeight - cutoffRow;
        if ((boundOrCount != 0 && cutoffRow <= (int)fieldGrid->gridHeight) && (boundOrCount = boundOrCount + -1, boundOrCount != 0))
        {
          spanCursor = g_TerrainProjectedRowSpans + cutoffRow + 3;
          for (boundOrCount = boundOrCount * 2; boundOrCount != 0; boundOrCount = boundOrCount + -1) {
            spanCursor->firstColumn = 0;
            spanCursor = (TerrainProjectedRowSpan *)&spanCursor->endColumnExclusive;
          }
        }
      }
      else {
        boundOrCount = 0;
        if (-1 < planeNormal->z) {
          boundOrCount = (int)(((longlong)g_ViewOriginFixed.z * (longlong)planeNormal->z) /
                       (longlong)planeNormal->y);
        }
        fixedProduct = (longlong)(boundOrCount + g_ViewOriginFixed.y) * -0x20c8cc;
        boundOrCount = (int)((int)((ulonglong)fixedProduct >> 0x20) << 0xc | (uint)fixedProduct >> 0x14) >> 0xc;
        if ((-1 < boundOrCount) && (boundOrCount != 0)) {
          spanCursor = g_TerrainProjectedRowSpans;
          for (boundOrCount = boundOrCount * 2; boundOrCount != 0; boundOrCount = boundOrCount + -1) {
            spanCursor->firstColumn = 0;
            spanCursor = (TerrainProjectedRowSpan *)&spanCursor->endColumnExclusive;
          }
        }
      }
    }
  }
  else if (planeNormal->x < 0) {
    fixedProduct = (longlong)planeNormal->y * (longlong)g_ViewOriginFixed.y +
            (longlong)planeNormal->x * (longlong)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      fixedProduct = fixedProduct + (longlong)planeNormal->z * (longlong)g_ViewOriginFixed.z;
    }
    fixedProduct = (longlong)(int)(fixedProduct / (longlong)planeNormal->x) * 0x1c6e9c;
    columnEdgeQ12 = (int)((ulonglong)fixedProduct >> 0x20) << 0xc | (uint)fixedProduct >> 0x14;
    fixedProduct = (longlong)(int)(((longlong)planeNormal->y * 1999) / (longlong)planeNormal->x) * 0x1c6e9c
    ;
    spanBoundCursor = (int *)THANDOR_ADDR(g_TerrainProjectedRowSpans,-8);
    rowsRemaining = fieldGrid->gridHeight;
    do {
      spanBoundCursor = spanBoundCursor + 2;
      boundOrCount = (int)(columnEdgeQ12 - 0x1000) >> 0xc;
      columnEdgeQ12 = columnEdgeQ12 + (((int)((ulonglong)fixedProduct >> 0x20) << 0xc | (uint)fixedProduct >> 0x14) - 0x800);
      if (*spanBoundCursor < boundOrCount) {
        *spanBoundCursor = boundOrCount;
      }
      rowsRemaining = rowsRemaining - 1;
    } while (rowsRemaining != 0);
  }
  else {
    fixedProduct = (longlong)planeNormal->y * (longlong)g_ViewOriginFixed.y +
            (longlong)planeNormal->x * (longlong)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      fixedProduct = fixedProduct + (longlong)planeNormal->z * (longlong)g_ViewOriginFixed.z;
    }
    fixedProduct = (longlong)(int)(fixedProduct / (longlong)planeNormal->x) * 0x1c6e9c;
    columnEdgeQ12 = (int)((ulonglong)fixedProduct >> 0x20) << 0xc | (uint)fixedProduct >> 0x14;
    fixedProduct = (longlong)(int)(((longlong)planeNormal->y * 1999) / (longlong)planeNormal->x) * 0x1c6e9c
    ;
    spanBoundCursor = (int *)THANDOR_ADDR(g_TerrainProjectedRowSpans,-4);
    rowsRemaining = fieldGrid->gridHeight;
    do {
      spanBoundCursor = spanBoundCursor + 2;
      boundOrCount = (int)(columnEdgeQ12 + 0x1fff) >> 0xc;
      columnEdgeQ12 = columnEdgeQ12 + (((int)((ulonglong)fixedProduct >> 0x20) << 0xc | (uint)fixedProduct >> 0x14) - 0x800);
      if (boundOrCount < *spanBoundCursor) {
        *spanBoundCursor = boundOrCount;
      }
      rowsRemaining = rowsRemaining - 1;
    } while (rowsRemaining != 0);
  }
  return;
}


/* Address: 0x005063B0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 0, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection0
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + 4;
      cell = cell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506430.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 1, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection1
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005064C0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 2, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection2
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506540.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 3, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection3
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + 4;
      cell = cell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005065C0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 4, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection4
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506650.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 5, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection5
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong scaledHeightProduct;
  int cellHeight;
  uint projectedHeightQ20;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (longlong)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((ulonglong)scaledHeightProduct >> 0x20) << 0x14 | (uint)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509320.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509380.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005093F0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509450.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005094B0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc)
      ;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509520.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509B90.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509BF0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509C50.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509CB0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509D10.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc)
      ;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509D70.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

