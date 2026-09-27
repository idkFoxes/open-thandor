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

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift. */
static __inline uint64_t TerrainProjection_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm (low dword): the four signed words saturated to unsigned bytes. */
static __inline uint32_t TerrainProjection_PackWordsUnsignedSaturate(uint64_t words)

{
  ThandorMmx lanes;
  uint32_t packed;
  int lane;

  lanes.q = words;
  packed = 0;
  for (lane = 0; lane < 4; lane = lane + 1) {
    packed = packed |
             (uint32_t)(lanes.sw[lane] < 0 ? 0 : (0xff < lanes.sw[lane] ? 0xff : lanes.sw[lane])) << (lane * 8);
  }
  return packed;
}

/* Address: 0x00506CD0.
   Line-of-sight marking for one army (occupancy rebuild): from the grid vertex nearest to the world point, ORs
   occupancyMaskBits (the bits of the factions that share the army's sight) into every cell the terrain does not
   hide from an eye at referenceHeightQ12 within the radius. The centre cell is marked here, the rest by the six
   sector traces, each seeded with the height of the next sector's first cell. Nothing happens outside the grid or
   on a map-edge cell.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (uint64_t occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  uint32_t baseColumn;
  int rowStrideBytes;
  int referenceHeight;
  uint32_t columnFraction;
  uint32_t fractionSumOrGridWidth;
  uint32_t rowFraction;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;

  if (fieldGrid != NULL) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    referenceHeight = g_TerrainScanReferenceHeight;
    baseColumn = gridCoordinates.columnQ12 >> 12;
    gridRow = gridCoordinates.rowQ12 >> 12;
    columnFraction = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFraction = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 32);
    /* pick the nearest vertex of the triangulated cell from the Q12 fractions (0x1000 = one cell) */
    fractionSumOrGridWidth = rowFraction + columnFraction * 2;
    gridColumn = baseColumn;
    if (fractionSumOrGridWidth < 0x1000) {
      if (0xfff < columnFraction + rowFraction * 2) {
        gridRow++;
      }
    }
    else if (fractionSumOrGridWidth < 0x2001) {
      gridColumn = baseColumn + 1;
      if (columnFraction < rowFraction) {
        gridRow++;
        gridColumn = baseColumn;
      }
    }
    else {
      gridColumn = baseColumn + 1;
      if (0x1fff < columnFraction + rowFraction * 2) {
        gridRow++;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    if ((((-1 < (int)gridColumn) && (fractionSumOrGridWidth = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)gridRow)) &&
        (gridRow < fieldGrid->gridHeight)) && (gridColumn < fractionSumOrGridWidth)) {
      centerCellIndex = gridRow * fractionSumOrGridWidth + gridColumn;
      if ((fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        fieldGrid->cells[centerCellIndex].occupancyMask =
             fieldGrid->cells[centerCellIndex].occupancyMask | occupancyMaskBits;
        rowStrideBytes = g_TerrainScanRowStrideBytes;
        /* the six neighbours of centre cell C, one per sector: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width;
           the first address is cells[centerCellIndex + 1], and runtime0C_3F - 0xC is a cell's own address) */
        wedgeCellA = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 centerCellIndex * 0x80 - 0x28);
        wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge0
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge1
                  (occupancyMaskBits,wedgeCellB[-1].terrainHeight - referenceHeight,0,wedgeCellB);
        wedgeCellA = (FieldGridCell *)((wedgeCellB - 1)[-1].runtime0C_3F + rowStrideBytes - 0xc);
        TerrainProjectedOcclusion_TraceWedge2
                  (occupancyMaskBits,wedgeCellA->terrainHeight - referenceHeight,0,wedgeCellB - 1);
        wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + rowStrideBytes - 0xc);
        TerrainProjectedOcclusion_TraceWedge3
                  (occupancyMaskBits,wedgeCellB->terrainHeight - referenceHeight,0,wedgeCellA);
        TerrainProjectedOcclusion_TraceWedge4
                  (occupancyMaskBits,wedgeCellB[1].terrainHeight - referenceHeight,0,wedgeCellB);
        /* (C+W) + 0x80 - stride + 0x48: terrainHeight of C+1, the first cell of sector 0 */
        TerrainProjectedOcclusion_TraceWedge5
                  (occupancyMaskBits,*(int *)((int)(wedgeCellB + 1) + (0xc8 - rowStrideBytes)) - referenceHeight,0,
                   wedgeCellB + 1);
      }
    }
  }
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
FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint32_t baseColumn;
  int rowStrideBytes;
  uint32_t columnFraction;
  uint32_t fractionSumOrGridWidth;
  uint32_t rowFraction;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *fieldCell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / 0x240;
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
    columnFraction = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFraction = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 0x20);
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
FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint32_t baseColumn;
  int rowStrideBytes;
  uint32_t columnFraction;
  uint32_t fractionSumOrGridWidth;
  uint32_t rowFraction;
  int centerCellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *fieldCell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t gridRow;
  uint32_t gridColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / 0x240;
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
    columnFraction = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFraction = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 0x20);
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
   Line-of-sight marking for the sector between directions 0 (C+1) and 1 (C+1-W) of
   TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint. Walks the sector's spine (step C+2-W, scan step +7)
   with a running horizon like TerrainProjectedOcclusion_ScanDirection0, tests the direction-1 neighbour between
   two spine cells, and hands the current horizon to a straight leg along each bounding direction. The first spine
   cell is tested against its own unscaled height above the eye; unless it is visible, the legs start from the
   average of that height and the caller's projectedHeightThresholdQ20.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge0
          (uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int heightOrRowStride;
  int neighborHeight;
  uint32_t projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightOrRowStride = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        heightOrRowStride = heightOrRowStride + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(heightOrRowStride - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      heightOrRowStride = g_TerrainScanRowStrideBytes;
      adjacentCell = cell + 1;
      projectedHeightOrStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,adjacentCell);
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      /* the direction-1 neighbour C+1-W: +0x48 terrainHeight, +0x4C waterSurfaceDelta, +0x50 flags,
         +0x70 occupancyMask, 0x80 = one cell */
      if ((*(uint32_t *)((int)adjacentCell + (0x50 - heightOrRowStride)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = *(int *)((int)adjacentCell + (0x48 - heightOrRowStride));
      if (0 < *(int *)((int)adjacentCell + (0x4c - heightOrRowStride))) {
        neighborHeight = neighborHeight + *(int *)((int)adjacentCell + (0x4c - heightOrRowStride));
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(uint64_t *)((int)adjacentCell + (0x70 - heightOrRowStride)) =
             *(uint64_t *)((int)adjacentCell + (0x70 - heightOrRowStride)) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      cell = (FieldGridCell *)((int)adjacentCell + (0x80 - heightOrRowStride));
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
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
   Line-of-sight marking for the sector between directions 1 (C+1-W) and 2 (C-W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C+1-2W (scan step +7), the direction-2 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge1
          (uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int heightOrRowStride;
  int neighborHeight;
  uint32_t projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightOrRowStride = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        heightOrRowStride = heightOrRowStride + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(heightOrRowStride - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      heightOrRowStride = g_TerrainScanRowStrideBytes;
      projectedHeightOrStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      /* the direction-2 neighbour C-W: +0x48 terrainHeight, +0x4C waterSurfaceDelta, +0x50 flags,
         +0x70 occupancyMask */
      if ((*(uint32_t *)((int)cell + (0x50 - heightOrRowStride)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = *(int *)((int)cell + (0x48 - heightOrRowStride));
      if (0 < *(int *)((int)cell + (0x4c - heightOrRowStride))) {
        neighborHeight = neighborHeight + *(int *)((int)cell + (0x4c - heightOrRowStride));
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(uint64_t *)((int)cell + (0x70 - heightOrRowStride)) =
             *(uint64_t *)((int)cell + (0x70 - heightOrRowStride)) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      adjacentCell = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - heightOrRowStride));
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


/* Address: 0x005068D0.
   Line-of-sight marking for the sector between directions 2 (C-W) and 3 (C-1), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-1-W (scan step +7), the direction-3 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge2
          (uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightOrStep;
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      projectedHeightOrStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell[-1].terrainHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        cellHeight = cellHeight + cell[-1].waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        cell[-1].occupancyMask = cell[-1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      adjacentCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
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
   Line-of-sight marking for the sector between directions 3 (C-1) and 4 (C-1+W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-2+W (scan step +7), the direction-4 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge3
          (uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  int rowStrideBytes;
  FieldCellPersistedAux cellHeight;
  int neighborHeight;
  uint32_t projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  FieldGridCell *adjacentCell;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      cellHeight = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        cellHeight = cellHeight + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(int)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = cell - 1;
      projectedHeightOrStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,adjacentCell);
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      /* the direction-4 neighbour C-1+W, addressed from runtime60_6B (+0x60): -0x18 terrainHeight,
         -0x14 waterSurfaceDelta, -0x10 flags, +0x10 occupancyMask */
      if ((*(uint32_t *)(adjacentCell->runtime60_6B + rowStrideBytes - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = *(int *)(adjacentCell->runtime60_6B + rowStrideBytes - 0x18);
      if (0 < *(int *)(adjacentCell->runtime60_6B + rowStrideBytes - 0x14)) {
        neighborHeight = neighborHeight + *(int *)(adjacentCell->runtime60_6B + rowStrideBytes - 0x14);
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(uint64_t *)(adjacentCell->runtime60_6B + rowStrideBytes + 0x10) =
             *(uint64_t *)(adjacentCell->runtime60_6B + rowStrideBytes + 0x10) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address: C-2+W */
      cell = (FieldGridCell *)(adjacentCell[-1].runtime0C_3F + rowStrideBytes - 0xc);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506AD0.
   Line-of-sight marking for the sector between directions 4 (C-1+W) and 5 (C+W), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C-1+2W (scan step +7), the direction-5 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge4
          (uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int64_t scaledHeightProduct;
  uint8_t *cellRuntimeBase;
  int heightOrRowStride;
  int neighborHeight;
  uint32_t projectedHeightOrStep;
  TerrainProjectedHeightThresholdQ20 cellThreshold;
  
  cellThreshold = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + cellThreshold) >> 1;
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      heightOrRowStride = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        heightOrRowStride = heightOrRowStride + cell->waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(heightOrRowStride - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      heightOrRowStride = g_TerrainScanRowStrideBytes;
      projectedHeightOrStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address */
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      /* the direction-5 neighbour C+W, addressed from runtime60_6B (+0x60): -0x18 terrainHeight,
         -0x14 waterSurfaceDelta, -0x10 flags, +0x10 occupancyMask */
      if ((*(uint32_t *)(cell->runtime60_6B + heightOrRowStride - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      neighborHeight = *(int *)(cell->runtime60_6B + heightOrRowStride - 0x18);
      if (0 < *(int *)(cell->runtime60_6B + heightOrRowStride - 0x14)) {
        neighborHeight = neighborHeight + *(int *)(cell->runtime60_6B + heightOrRowStride - 0x14);
      }
      scaledHeightProduct = (int64_t)(neighborHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        *(uint64_t *)(cell->runtime60_6B + heightOrRowStride + 0x10) =
             *(uint64_t *)(cell->runtime60_6B + heightOrRowStride + 0x10) | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      cellRuntimeBase = cell->runtime0C_3F;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = (FieldGridCell *)(cellRuntimeBase + g_TerrainScanRowStrideBytes + heightOrRowStride - 0xc) - 1;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)(cellRuntimeBase + g_TerrainScanRowStrideBytes + heightOrRowStride - 0xc));
      cellThreshold = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506BD0.
   Line-of-sight marking for the sector between directions 5 (C+W) and 0 (C+1), built like
   TerrainProjectedOcclusion_TraceWedge0: spine step C+1+W (scan step +7), the direction-0 neighbour between two
   spine cells, a straight leg along each bounding direction.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge5
          (uint64_t occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *adjacentCell;
  int64_t scaledHeightProduct;
  int cellHeight;
  uint32_t projectedHeightOrStep;
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)cellThreshold <= (int)projectedHeightOrStep) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      projectedHeightOrStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address */
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,projectedHeightOrStep,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= projectedHeightOrStep) {
        return;
      }
      if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      cellHeight = cell[1].terrainHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        cellHeight = cellHeight + cell[1].waterSurfaceDelta;
      }
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + projectedHeightOrStep * 4);
      projectedHeightOrStep = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightOrStep) {
        cell[1].occupancyMask = cell[1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightOrStep;
      }
      adjacentCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
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
      if ((*(uint32_t *)((int)adjacentCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint32_t *)((int)adjacentCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)adjacentCell + (0x4c - rowStrideBytes)) < 0)) {
        *(uint32_t *)((int)adjacentCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
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
      if ((*(uint32_t *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint32_t *)((int)fieldCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell + (0x4c - rowStrideBytes)) < 0)) {
        *(uint32_t *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
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
      if ((*(uint32_t *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint32_t *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x14) < 0)) {
        *(uint32_t *)(adjacentCell->runtime0C_3F + scanRowStrideBytes + -8) = g_TerrainScanReferenceHeight;
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
  uint8_t *currentCellRuntimeBase;
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
      if ((*(uint32_t *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint32_t *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell->runtime60_6B + rowStrideBytes + -0x14) < 0)) {
        *(uint32_t *)(fieldCell->runtime0C_3F + rowStrideBytes + -8) = g_TerrainScanReferenceHeight;
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
      if ((*(uint32_t *)((int)adjacentCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)adjacentCell + (0x4c - rowStrideBytes))) {
        *(uint32_t *)((int)adjacentCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
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
      if ((*(uint32_t *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell + (0x4c - rowStrideBytes))) {
        *(uint32_t *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
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
      if ((*(uint32_t *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(adjacentCell->runtime60_6B + scanRowStrideBytes + -0x14)) {
        *(uint32_t *)(adjacentCell->runtime0C_3F + scanRowStrideBytes + -8) = g_TerrainScanReferenceHeight;
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
  uint8_t *currentCellRuntimeBase;
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
      if ((*(uint32_t *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell->runtime60_6B + rowStrideBytes + -0x14)) {
        *(uint32_t *)(fieldCell->runtime0C_3F + rowStrideBytes + -8) = g_TerrainScanReferenceHeight;
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
          (uint32_t rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
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
  int offsetZ;
  uint32_t resultFlags;
  uint32_t pointAFlags;
  MmxPackedValue64 lightingFactors;
  uint64_t shadedProduct;
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
    /* vertex color PUNPCKLBW/PSRLW 6 (optionally lit), base color PUNPCKLBW/PSRLW 2, PMULHW, PACKUSWB */
    lightingFactors = TerrainProjection_UnpackBytesShiftRight(vertexColor,6);
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointA,lightingFactors);
    }
    shadedProduct = pmulhw(lightingFactors,TerrainProjection_UnpackBytesShiftRight(baseColor,2));
    offsetVector = vertex->secondaryOffset;
    vertex->shadedColorA = TerrainProjection_PackWordsUnsignedSaturate(shadedProduct);
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
    lightingFactors = TerrainProjection_UnpackBytesShiftRight(vertexColor,6);
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointB,lightingFactors);
    }
    shadedProduct = pmulhw(lightingFactors,TerrainProjection_UnpackBytesShiftRight(baseColor,2));
    vertex->projectionFlags = resultFlags;
    vertex->shadedColorB = TerrainProjection_PackWordsUnsignedSaturate(shadedProduct);
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
  int offsetZ;
  uint32_t maskedFlags;
  uint32_t resultFlags;
  MmxPackedValue64 lightingFactors;
  uint64_t shadedProduct;
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
    lightingFactors = TerrainProjection_UnpackBytesShiftRight(vertexColor,6);
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointB,lightingFactors);
    }
    shadedProduct = pmulhw(lightingFactors,TerrainProjection_UnpackBytesShiftRight(baseColor,2));
    vertex->shadedColorB = TerrainProjection_PackWordsUnsignedSaturate(shadedProduct);
  }
  vertexColor = vertex->packedColorA;
  baseColor = vertex->basePackedColor;
  lightingFactors = TerrainProjection_UnpackBytesShiftRight(vertexColor,6);
  if (vertex->lightingLookupIndexOrSentinel == 0xff) {
    lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                       (&vertex->viewPointA,lightingFactors);
  }
  shadedProduct = pmulhw(lightingFactors,TerrainProjection_UnpackBytesShiftRight(baseColor,2));
  vertex->shadedColorA = TerrainProjection_PackWordsUnsignedSaturate(shadedProduct);
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
  uint32_t vertex0ViewDepth;
  void *soilPacketTable;
  uint32_t flagsOrClampedDepth0;
  int yOrTableIndexC;
  int yOrTableIndexA;
  uint32_t clampedDepth2;
  int yOrTableIndexB;
  uint32_t clampedDepth1;
  int materialOffset1;
  int materialOffset0;
  bool outsideTriangle;
  PackedArgb32 vertex0Color;
  uint64_t litProduct0;
  PackedArgb32 vertex1Color;
  uint64_t litProduct1;
  PackedArgb32 vertex2Color;
  uint64_t litProduct2;
  TriangleBarycentricWeightsQ12 barycentricWeights;
  PrimitivePacketResult queuedPacket;
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
      /* PUNPCKLBW/PSRLW 4 of each color, PMULHW by its lighting level, PACKUSWB */
      litProduct0 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex0Color,4),
                           g_PackedLightingLookupTable[yOrTableIndexA]);
      litProduct1 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex1Color,4),
                           g_PackedLightingLookupTable[yOrTableIndexB]);
      litProduct2 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex2Color,4),
                           g_PackedLightingLookupTable[yOrTableIndexC]);
      vertex0Color = TerrainProjection_PackWordsUnsignedSaturate(litProduct0);
      vertex1Color = TerrainProjection_PackWordsUnsignedSaturate(litProduct1);
      vertex2Color = TerrainProjection_PackWordsUnsignedSaturate(litProduct2);
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
                         ((uint32_t *)((int)g_TerrainSoilPacketTablePayload +
                                   materialOffset0 + (vertex0->projectionFlags & 0x700)),vertex2Color,vertex1Color,vertex0Color
                          ,(GraphicsProjectedVertexSource *)vertex2,
                          (GraphicsProjectedVertexSource *)vertex1,
                          (GraphicsProjectedVertexSource *)vertex0,renderContext);
      if (!queuedPacket.noPacket) {
        if (materialOffset0 == materialOffset1) {
          if (materialOffset0 != yOrTableIndexC) {
            queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               ((uint32_t *)((int)soilPacketTable + yOrTableIndexB + 0x20),vertex2Color,vertex1Color,vertex0Color,
                                (GraphicsProjectedVertexSource *)vertex2Projected,
                                (GraphicsProjectedVertexSource *)vertex1Projected,
                                (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
            if (!queuedPacket.noPacket) {
              packetRenderFlags = &(queuedPacket.packet)->renderFlags;
              *packetRenderFlags = *packetRenderFlags | 0x10020000;
            }
          }
        }
        else if (materialOffset0 == yOrTableIndexC) {
          queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((uint32_t *)((int)soilPacketTable + yOrTableIndexA + 0x40),vertex2Color,vertex1Color,vertex0Color,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
          if (!queuedPacket.noPacket) {
            packetRenderFlags = &(queuedPacket.packet)->renderFlags;
            *packetRenderFlags = *packetRenderFlags | 0x10020000;
          }
        }
        else if (materialOffset1 == yOrTableIndexC) {
          queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((uint32_t *)((int)soilPacketTable + yOrTableIndexA + 0x60),vertex2Color,vertex1Color,vertex0Color,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
          if (!queuedPacket.noPacket) {
            packetRenderFlags = &(queuedPacket.packet)->renderFlags;
            *packetRenderFlags = *packetRenderFlags | 0x10020000;
          }
        }
        else {
          queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((uint32_t *)((int)soilPacketTable + yOrTableIndexA + 0x80),vertex2Color,vertex1Color,vertex0Color,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
          if (!queuedPacket.noPacket) {
            packetRenderFlags = &(queuedPacket.packet)->renderFlags;
            *packetRenderFlags = *packetRenderFlags | 0x10020000;
            queuedPacket = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               ((uint32_t *)((int)soilPacketTable + yOrTableIndexB + 0xa0),vertex2Color,vertex1Color,vertex0Color,
                                (GraphicsProjectedVertexSource *)vertex2Projected,
                                (GraphicsProjectedVertexSource *)vertex1Projected,
                                (GraphicsProjectedVertexSource *)vertex0Projected,savedRenderContext);
            if (!queuedPacket.noPacket) {
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
      litProduct0 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex0Color,4),
                           g_PackedLightingLookupTable[vertex0->lightingLookupIndexOrSentinel]);
      litProduct1 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex1Color,4),
                           g_PackedLightingLookupTable[vertex1->lightingLookupIndexOrSentinel]);
      litProduct2 = pmulhw(TerrainProjection_UnpackBytesShiftRight(vertex2Color,4),
                           g_PackedLightingLookupTable[vertex2->lightingLookupIndexOrSentinel]);
      GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
                ((uint32_t *)(surfacePacketIndex * 0x20 + (int)g_TerrainSurfacePacketTablePayload),
                 TerrainProjection_PackWordsUnsignedSaturate(litProduct2),
                 TerrainProjection_PackWordsUnsignedSaturate(litProduct1),
                 TerrainProjection_PackWordsUnsignedSaturate(litProduct0),
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
  int64_t fixedProduct;
  int boundOrCount;
  uint32_t columnEdgeQ12;
  int cutoffRow;
  FieldGridDimension rowsRemaining;
  int *spanBoundCursor;
  TerrainProjectedRowSpan *spanCursor;
  
  if (planeNormal->x == 0) {
    if (planeNormal->y != 0) {
      if (planeNormal->y < 0) {
        boundOrCount = 0;
        if (-1 < planeNormal->z) {
          boundOrCount = (int)(((int64_t)g_ViewOriginFixed.z * (int64_t)planeNormal->z) /
                       (int64_t)planeNormal->y);
        }
        fixedProduct = (int64_t)(boundOrCount + g_ViewOriginFixed.y) * -0x20c8cc;
        cutoffRow = (int)((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) >> 0xc;
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
          boundOrCount = (int)(((int64_t)g_ViewOriginFixed.z * (int64_t)planeNormal->z) /
                       (int64_t)planeNormal->y);
        }
        fixedProduct = (int64_t)(boundOrCount + g_ViewOriginFixed.y) * -0x20c8cc;
        boundOrCount = (int)((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) >> 0xc;
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
    fixedProduct = (int64_t)planeNormal->y * (int64_t)g_ViewOriginFixed.y +
            (int64_t)planeNormal->x * (int64_t)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      fixedProduct = fixedProduct + (int64_t)planeNormal->z * (int64_t)g_ViewOriginFixed.z;
    }
    fixedProduct = (int64_t)(int)(fixedProduct / (int64_t)planeNormal->x) * 0x1c6e9c;
    columnEdgeQ12 = (int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14;
    fixedProduct = (int64_t)(int)(((int64_t)planeNormal->y * 1999) / (int64_t)planeNormal->x) * 0x1c6e9c
    ;
    spanBoundCursor = (int *)THANDOR_ADDR(g_TerrainProjectedRowSpans,-8);
    rowsRemaining = fieldGrid->gridHeight;
    do {
      spanBoundCursor = spanBoundCursor + 2;
      boundOrCount = (int)(columnEdgeQ12 - 0x1000) >> 0xc;
      columnEdgeQ12 = columnEdgeQ12 + (((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) - 0x800);
      if (*spanBoundCursor < boundOrCount) {
        *spanBoundCursor = boundOrCount;
      }
      rowsRemaining = rowsRemaining - 1;
    } while (rowsRemaining != 0);
  }
  else {
    fixedProduct = (int64_t)planeNormal->y * (int64_t)g_ViewOriginFixed.y +
            (int64_t)planeNormal->x * (int64_t)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      fixedProduct = fixedProduct + (int64_t)planeNormal->z * (int64_t)g_ViewOriginFixed.z;
    }
    fixedProduct = (int64_t)(int)(fixedProduct / (int64_t)planeNormal->x) * 0x1c6e9c;
    columnEdgeQ12 = (int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14;
    fixedProduct = (int64_t)(int)(((int64_t)planeNormal->y * 1999) / (int64_t)planeNormal->x) * 0x1c6e9c
    ;
    spanBoundCursor = (int *)THANDOR_ADDR(g_TerrainProjectedRowSpans,-4);
    rowsRemaining = fieldGrid->gridHeight;
    do {
      spanBoundCursor = spanBoundCursor + 2;
      boundOrCount = (int)(columnEdgeQ12 + 0x1fff) >> 0xc;
      columnEdgeQ12 = columnEdgeQ12 + (((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) - 0x800);
      if (boundOrCount < *spanBoundCursor) {
        *spanBoundCursor = boundOrCount;
      }
      rowsRemaining = rowsRemaining - 1;
    } while (rowsRemaining != 0);
  }
  return;
}


/* Address: 0x005063B0.
   Line-of-sight leg along direction 0 (C+1, right): each cell's surface height (terrain plus positive water)
   above the eye (g_TerrainScanReferenceHeight) is scaled by the per-step table g_TerrainHeightDeltaScaleByStepQ12;
   a cell whose value reaches the highest value seen so far on this line is visible and gets occupancyMaskBits,
   and its value becomes the new horizon. 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection0
          (uint64_t occupancyMaskBits,
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
      /* the table holds one int per scan step; the product keeps bits 12..43 (SHLD EDX,EAX,20) */
      scaledHeightProduct = (int64_t)(cellHeight - (int)g_TerrainScanReferenceHeight) *
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
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


/* Address: 0x00506430.
   Line-of-sight leg along direction 1 (C+1-W, up and right); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection1
          (uint64_t occupancyMaskBits,
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)); /* 0x80 = one cell */
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005064C0.
   Line-of-sight leg along direction 2 (C-W, up); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection2
          (uint64_t occupancyMaskBits,
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506540.
   Line-of-sight leg along direction 3 (C-1, left); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection3
          (uint64_t occupancyMaskBits,
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
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


/* Address: 0x005065C0.
   Line-of-sight leg along direction 4 (C-1+W, down and left); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection4
          (uint64_t occupancyMaskBits,
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address */
      cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506650.
   Line-of-sight leg along direction 5 (C+W, down); works like TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection5
          (uint64_t occupancyMaskBits,
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
              (int64_t)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      projectedHeightQ20 = (int)((uint64_t)scaledHeightProduct >> 0x20) << 0x14 | (uint32_t)scaledHeightProduct >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)projectedHeightQ20) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = projectedHeightQ20;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      /* runtime0C_3F (+0x0C) - 0xC is a cell's own address */
      cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
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

