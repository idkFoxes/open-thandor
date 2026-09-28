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

/* Not a function of its own in the original: PUNPCKLBW mm,mm then PSRLW mm,shift, i.e. the four bytes b of value
   as the words ((b << 8) | b) >> shift (the MMX colour unpack of the terrain shading). */
static __inline uint64_t TerrainProjection_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
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

/* Address: 0x00506CD0.
   Line-of-sight marking for one army (occupancy rebuild): from the grid vertex nearest to the world point, ORs
   occupancyMaskBits (the bits of the factions that share the army's sight) into every cell the terrain does not
   hide from an eye at referenceHeightQ12 within the radius. The centre cell is marked here, the rest by the six
   sector traces, each seeded with the height of the next sector's first cell. Nothing happens outside the grid or
   on a map-edge cell.
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
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
   Terrain-class overlay callback for land classes (g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
   slots 0, 2, 3 and 4, called by WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries): stores cellValue into
   runtimeOverlayOrHeightValue04 of every cell within the radius around the world point that has a bit of
   cellFlagMask and no water above it; the centre cell here, the rest by the six sector walks (the same hexagon
   as TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint, without line of sight). Marks the field grid
   surface dirty. CF set (nothing applied) without a grid, outside it or on a map-edge cell.
*/
bool FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
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
  
  if (fieldGrid != NULL) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
    }
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
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
        (gridRow < fieldGrid->gridHeight)) &&
       ((gridColumn < fractionSumOrGridWidth &&
        (centerCellIndex = gridRow * fractionSumOrGridWidth + gridColumn,
        (fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)))) {
      if (((fieldGrid->cells[centerCellIndex].flagsAndMaterial & cellFlagMask) != 0) &&
         (fieldGrid->cells[centerCellIndex].waterSurfaceDelta < 0)) {
        fieldGrid->cells[centerCellIndex].runtimeOverlayOrHeightValue04 = cellValue;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      /* the first cell of each sector walk: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width; the first address
         is cells[centerCellIndex + 1], and runtime0C_3F - 0xC is a cell's own address) */
      wedgeCellA = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               centerCellIndex * 0x80 - 0x28);
      wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyWedge0(0,wedgeCellA);
      fieldCell = wedgeCellB - 1;
      FieldGridTerrainOverlayVariantA_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + rowStrideBytes - 0xc);
      FieldGridTerrainOverlayVariantA_ApplyWedge2(0,fieldCell);
      wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + rowStrideBytes - 0xc);
      FieldGridTerrainOverlayVariantA_ApplyWedge3(0,wedgeCellA);
      FieldGridTerrainOverlayVariantA_ApplyWedge4(0,wedgeCellB);
      FieldGridTerrainOverlayVariantA_ApplyWedge5(0,wedgeCellB + 1);
      return false;
    }
  }
  return true;
}


/* Address: 0x0050A190.
   Terrain-class overlay callback for the water class (g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
   slot 1, called by WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries): like
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, but only for cells with water above them; the centre
   cell also needs a bit of cellFlagMask, the sector walks ignore the mask.
*/
bool FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
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
  
  if (fieldGrid != NULL) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
    }
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
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
        (gridRow < fieldGrid->gridHeight)) &&
       ((gridColumn < fractionSumOrGridWidth &&
        (centerCellIndex = gridRow * fractionSumOrGridWidth + gridColumn,
        (fieldGrid->cells[centerCellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)))) {
      if (((fieldGrid->cells[centerCellIndex].flagsAndMaterial & cellFlagMask) != 0) &&
         (0 < fieldGrid->cells[centerCellIndex].waterSurfaceDelta)) {
        fieldGrid->cells[centerCellIndex].runtimeOverlayOrHeightValue04 = cellValue;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      /* the first cell of each sector walk: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width; the first address
         is cells[centerCellIndex + 1], and runtime0C_3F - 0xC is a cell's own address) */
      wedgeCellA = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               centerCellIndex * 0x80 - 0x28);
      wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyWedge0(0,wedgeCellA);
      fieldCell = wedgeCellB - 1;
      FieldGridTerrainOverlayVariantB_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + rowStrideBytes - 0xc);
      FieldGridTerrainOverlayVariantB_ApplyWedge2(0,fieldCell);
      wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + rowStrideBytes - 0xc);
      FieldGridTerrainOverlayVariantB_ApplyWedge3(0,wedgeCellA);
      FieldGridTerrainOverlayVariantB_ApplyWedge4(0,wedgeCellB);
      FieldGridTerrainOverlayVariantB_ApplyWedge5(0,wedgeCellB + 1);
      return false;
    }
  }
  return true;
}


/* Address: 0x00500F50.
   Terrain pass of the world view (called by FrontendModelPointerContext_RenderWorldViewQueuesClipped): unless
   the previous projection can be reused (TERRAIN_RENDER_REUSE_PROJECTION), rebuilds the visible column span of
   every grid row from the four frustum side planes, marks all vertices as not projected and widens each span to
   cover its neighbour rows. Then projects and shades the vertices inside the spans, fully (VariantA) when the
   grid surface changed or the projection is not reusable, else only the parts that can change (VariantB), and
   queues every grid quad between two rows of the spans as two triangles.
*/
void TerrainProjectedGrid_TransformShadeAndQueue
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
      remainingCount = remainingCount - 1;
      firstColumnOrRowsLeft = spanFirstColumn;
      columnOrVertexCount = spanEndColumn;
    } while (remainingCount != 0);
  }
  rowSpan = g_TerrainProjectedRowSpans;
  gridWidth = fieldGrid->gridWidth;
  rowCount = fieldGrid->gridHeight;
  if (((fieldGrid->runtimeStateFlags & FIELD_GRID_RUNTIME_SURFACE_DIRTY) == 0) &&
     ((renderContext->contextFlags & TERRAIN_RENDER_REUSE_PROJECTION) != 0)) {
    rowCells = fieldGrid->cells;
    do {
      firstColumnOrRowsLeft = rowSpan->firstColumn;
      columnOrVertexCount = rowSpan->endColumnExclusive - firstColumnOrRowsLeft;
      if (columnOrVertexCount != 0 && firstColumnOrRowsLeft <= rowSpan->endColumnExclusive) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + firstColumnOrRowsLeft);
        do {
          TerrainProjectedVertex_TransformProjectAndShadeVariantB(vertexCursor);
          vertexCursor = vertexCursor + 1;
          columnOrVertexCount = columnOrVertexCount - 1;
        } while (columnOrVertexCount != 0);
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
      firstColumnOrRowsLeft = rowSpan->firstColumn;
      columnOrVertexCount = rowSpan->endColumnExclusive - firstColumnOrRowsLeft;
      if (columnOrVertexCount != 0 && firstColumnOrRowsLeft <= rowSpan->endColumnExclusive) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + firstColumnOrRowsLeft);
        do {
          TerrainProjectedVertex_TransformProjectAndShadeVariantA(vertexCursor);
          vertexCursor = vertexCursor + 1;
          columnOrVertexCount = columnOrVertexCount - 1;
        } while (columnOrVertexCount != 0);
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
  firstColumnOrRowsLeft = fieldGrid->gridHeight - 1;
  do {
    columnOrVertexCount = rowSpan->firstColumn;
    remainingCount = rowSpan->endColumnExclusive - columnOrVertexCount;
    if (remainingCount != 0 && columnOrVertexCount <= rowSpan->endColumnExclusive) {
      remainingCount = remainingCount - 1;
      if (remainingCount != 0) {
        vertexCursor = (TerrainProjectedVertexWorkRecord *)(rowCells + columnOrVertexCount);
        do {
          TerrainProjectedQuad_QueueAsTwoTrianglesRegs(gridWidth * 0x80,vertexCursor,renderContext);
          vertexCursor = vertexCursor + 1;
          remainingCount = remainingCount - 1;
        } while (remainingCount != 0);
      }
    }
    rowSpan = rowSpan + 1;
    rowCells = rowCells + gridWidth;
    firstColumnOrRowsLeft = firstColumnOrRowsLeft - 1;
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_TraceWedge0(uint64_t occupancyMaskBits,
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_TraceWedge1(uint64_t occupancyMaskBits,
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_TraceWedge2(uint64_t occupancyMaskBits,
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_TraceWedge3(uint64_t occupancyMaskBits,
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_TraceWedge4(uint64_t occupancyMaskBits,
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
*/
void TerrainProjectedOcclusion_TraceWedge5(uint64_t occupancyMaskBits,
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
   Overlay sector between directions 0 (C+1, right) and 1 (C+1-W, up and right) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge0
   but without a horizon: spine step C+2-W (scan step +7), each spine cell and the direction-1 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* +0x50 flagsAndMaterial, +0x4C waterSurfaceDelta, +4 runtimeOverlayOrHeightValue04 of the cell one row up */
      if ((*(uint32_t *)((int)adjacentCell + (0x50 - rowStrideBytes)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((*(uint32_t *)((int)adjacentCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)adjacentCell + (0x4c - rowStrideBytes)) < 0)) {
        *(uint32_t *)((int)adjacentCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)adjacentCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
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
   Overlay sector between directions 1 (C+1-W, up and right) and 2 (C-W, up) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge1
   but without a horizon: spine step C+1-2W (scan step +7), each spine cell and the direction-2 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* +0x50 flagsAndMaterial, +0x4C waterSurfaceDelta, +4 runtimeOverlayOrHeightValue04 of the cell one row up */
      if ((*(uint32_t *)((int)fieldCell + (0x50 - rowStrideBytes)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((*(uint32_t *)((int)fieldCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell + (0x4c - rowStrideBytes)) < 0)) {
        *(uint32_t *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      adjacentCell = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
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


/* Address: 0x005096F0.
   Overlay sector between directions 2 (C-W, up) and 3 (C-1, left) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection2
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell[-1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) !=
           0) && (fieldCell[-1].waterSurfaceDelta < 0)) {
        fieldCell[-1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
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
   Overlay sector between directions 3 (C-1, left) and 4 (C-1+W, down and left) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge3
   but without a horizon: spine step C-2+W (scan step +7), each spine cell and the direction-4 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  int scanRowStrideBytes;
  FieldGridCell *adjacentCell;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanRowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell - 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* runtime60_6B - 0x10 / - 0x14 and runtime0C_3F - 8: flagsAndMaterial, waterSurfaceDelta and
         runtimeOverlayOrHeightValue04 of the cell one row down */
      if ((*(uint32_t *)(adjacentCell->runtime60_6B + scanRowStrideBytes - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((*(uint32_t *)(adjacentCell->runtime60_6B + scanRowStrideBytes - 0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(adjacentCell->runtime60_6B + scanRowStrideBytes - 0x14) < 0)) {
        *(uint32_t *)(adjacentCell->runtime0C_3F + scanRowStrideBytes - 8) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(adjacentCell[-1].runtime0C_3F + scanRowStrideBytes - 0xc);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep,(FieldGridCell *)
                          (fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509860.
   Overlay sector between directions 4 (C-1+W, down and left) and 5 (C+W, down) of
   FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge4
   but without a horizon: spine step C-1+2W (scan step +7), each spine cell and the direction-5 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantA_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint8_t *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* runtime60_6B - 0x10 / - 0x14 and runtime0C_3F - 8: flagsAndMaterial, waterSurfaceDelta and
         runtimeOverlayOrHeightValue04 of the cell one row down */
      if ((*(uint32_t *)(fieldCell->runtime60_6B + rowStrideBytes - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((*(uint32_t *)(fieldCell->runtime60_6B + rowStrideBytes - 0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell->runtime60_6B + rowStrideBytes - 0x14) < 0)) {
        *(uint32_t *)(fieldCell->runtime0C_3F + rowStrideBytes - 8) = g_TerrainScanReferenceHeight;
      }
      currentCellRuntimeBase = fieldCell->runtime0C_3F;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes - 0xc) - 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase +
                          g_TerrainScanRowStrideBytes + rowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509910.
   Overlay sector between directions 5 (C+W, down) and 0 (C+1, right) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (((fieldCell[1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0
          ) && (fieldCell[1].waterSurfaceDelta < 0)) {
        fieldCell[1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509DD0.
   Overlay sector between directions 0 (C+1, right) and 1 (C+1-W, up and right) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* +0x50 flagsAndMaterial, +0x4C waterSurfaceDelta, +4 runtimeOverlayOrHeightValue04 of the cell one row up */
      if ((*(uint32_t *)((int)adjacentCell + (0x50 - rowStrideBytes)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < *(int *)((int)adjacentCell + (0x4c - rowStrideBytes))) {
        *(uint32_t *)((int)adjacentCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)adjacentCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
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
   Overlay sector between directions 1 (C+1-W, up and right) and 2 (C-W, up) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* +0x50 flagsAndMaterial, +0x4C waterSurfaceDelta, +4 runtimeOverlayOrHeightValue04 of the cell one row up */
      if ((*(uint32_t *)((int)fieldCell + (0x50 - rowStrideBytes)) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell + (0x4c - rowStrideBytes))) {
        *(uint32_t *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      adjacentCell = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
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


/* Address: 0x00509F10.
   Overlay sector between directions 2 (C-W, up) and 3 (C-1, left) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection2
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell[-1].waterSurfaceDelta) {
        fieldCell[-1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
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
   Overlay sector between directions 3 (C-1, left) and 4 (C-1+W, down and left) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanRowStrideBytes = g_TerrainScanRowStrideBytes;
      adjacentCell = fieldCell - 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* runtime60_6B - 0x10 / - 0x14 and runtime0C_3F - 8: flagsAndMaterial, waterSurfaceDelta and
         runtimeOverlayOrHeightValue04 of the cell one row down */
      if ((*(uint32_t *)(adjacentCell->runtime60_6B + scanRowStrideBytes - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < *(int *)(adjacentCell->runtime60_6B + scanRowStrideBytes - 0x14)) {
        *(uint32_t *)(adjacentCell->runtime0C_3F + scanRowStrideBytes - 8) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(adjacentCell[-1].runtime0C_3F + scanRowStrideBytes - 0xc);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep,(FieldGridCell *)
                          (fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x0050A050.
   Overlay sector between directions 4 (C-1+W, down and left) and 5 (C+W, down) of
   FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint, walked like TerrainProjectedOcclusion_TraceWedge4
   but without a horizon: spine step C-1+2W (scan step +7), each spine cell and the direction-5 neighbour
   between two spine cells get the overlay, and a straight leg runs from each along both bounding
   directions, so the whole sector is covered.
*/
void FieldGridTerrainOverlayVariantB_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint8_t *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* runtime60_6B - 0x10 / - 0x14 and runtime0C_3F - 8: flagsAndMaterial, waterSurfaceDelta and
         runtimeOverlayOrHeightValue04 of the cell one row down */
      if ((*(uint32_t *)(fieldCell->runtime60_6B + rowStrideBytes - 0x10) & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell->runtime60_6B + rowStrideBytes - 0x14)) {
        *(uint32_t *)(fieldCell->runtime0C_3F + rowStrideBytes - 8) = g_TerrainScanReferenceHeight;
      }
      currentCellRuntimeBase = fieldCell->runtime0C_3F;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes - 0xc) - 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase +
                          g_TerrainScanRowStrideBytes + rowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x0050A0F0.
   Overlay sector between directions 5 (C+W, down) and 0 (C+1, right) of
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                 (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      if (0 < fieldCell[1].waterSurfaceDelta) {
        fieldCell[1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      adjacentCell = fieldCell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep,adjacentCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00500CE0.
   Queues the grid quad whose top-left vertex is topLeftVertex as two triangles, (top-left, bottom-left,
   top-right) and (bottom-left, bottom-right, top-right) as vertex0..2, both with the top-left vertex's
   secondary-surface packet; rowStrideBytes is one grid row of vertex records. Called for every quad of the
   visible spans by TerrainProjectedGrid_TransformShadeAndQueue.
*/
void TerrainProjectedQuad_QueueAsTwoTrianglesRegs
          (uint32_t rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  /* reserved08_0B + stride - 8 is the vertex one row down */
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
   Full per-frame update of one terrain vertex (a field cell): transforms the terrain point to view space,
   projects it when it lies beyond the near plane and records on which sides of the clip rectangle it lies,
   and shades its colour with the base colour (plus the dynamic lights when lightingLookupIndexOrSentinel is
   0xFF). Does the same for point B, the secondary surface point (terrain point + secondaryOffset, raised by
   secondaryProjectionDepthQ12). Vertices without terrain (material 0xFF) are skipped.
*/
void TerrainProjectedVertex_TransformProjectAndShadeVariantA(TerrainProjectedVertexWorkRecord *vertex)

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
  
  /* keeps the material and bits 8..16, 27 and 29..31; the clip bits and SECONDARY_VISIBLE start cleared */
  resultFlags = vertex->projectionFlags & 0xe801ffff;
  if ((vertex->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) != TERRAIN_VERTEX_MATERIAL_NONE) {
    pointAFlags = resultFlags | TERRAIN_VERTEX_POINT_A_NOT_PROJECTED;
    FixedTransform_ApplyPoint(&vertex->viewPointA,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed);
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointA).z) {
      projectedPoint = Graphics_ProjectViewPoint(&vertex->viewPointA);
      vertex->projectedPointA = projectedPoint;
      pointAFlags = resultFlags;
      if (g_ProjectionClipRect.minX <= projectedPoint.projectedX) {
        pointAFlags = resultFlags | TERRAIN_VERTEX_POINT_A_INSIDE_MIN_X;
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
    resultFlags = pointAFlags | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED;
    /* point B = terrain point + secondaryOffset + (0, 0, secondaryProjectionDepthQ12), built in sourcePoint and undone after
       the transform */
    offsetX = offsetVector->x;
    offsetY = offsetVector->y;
    offsetZ = offsetVector->z;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x + offsetX;
    offsetZ = offsetZ + vertex->secondaryProjectionDepthQ12;
    sourceCoordinate = &(vertex->sourcePoint).y;
    *sourceCoordinate = *sourceCoordinate + offsetY;
    sourceCoordinate = &(vertex->sourcePoint).z;
    *sourceCoordinate = *sourceCoordinate + offsetZ;
    FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed);
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
        resultFlags = pointAFlags | TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X;
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
   Cheap per-frame update of one terrain vertex while the view and grid are unchanged: keeps the projection of
   the terrain point and only re-shades it; point B (the secondary surface point) is projected and shaded again
   only when the vertex belonged to a visible secondary-surface triangle last frame.
*/
void TerrainProjectedVertex_TransformProjectAndShadeVariantB(TerrainProjectedVertexWorkRecord *vertex)

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
  if ((resultFlags & TERRAIN_VERTEX_SECONDARY_VISIBLE) != 0) {
    /* clears the five point-B bits */
    maskedFlags = resultFlags & 0xf83fffff;
    resultFlags = maskedFlags | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED;
    offsetX = offsetVector->x;
    offsetY = offsetVector->y;
    offsetZ = offsetVector->z;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x + offsetX;
    offsetZ = offsetZ + vertex->secondaryProjectionDepthQ12;
    sourceCoordinate = &(vertex->sourcePoint).y;
    *sourceCoordinate = *sourceCoordinate + offsetY;
    sourceCoordinate = &(vertex->sourcePoint).z;
    *sourceCoordinate = *sourceCoordinate + offsetZ;
    FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed);
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
        resultFlags = maskedFlags | TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X;
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
   Queues one terrain triangle. When its screen bounds can overlap the clip rectangle, the terrain triangle is
   queued with the soil texture of vertex0's material (vertex colours darkened by the water depth); if the
   vertices have different materials, one or two blend triangles of the other materials follow. When any vertex
   lies under water (or WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY is set) and the secondary points can be
   visible, the secondary-surface triangle is queued as well. Along the way the cursor is picked: when it lies
   inside the projected triangle and nearer than the best hit so far, the hit depth (callbackArgumentF0) and the
   interpolated world X/Y (callbackArgumentE8/EC) are stored; the top byte of g_UiCommandModeGColorVariantLimit
   chooses between the terrain (0) and the secondary surface. Skipped entirely when the OR of the three material
   bytes is 0xFF, which it always is when a vertex has no terrain.
*/
void TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
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
  if ((flagsOrClampedDepth0 & TERRAIN_VERTEX_MATERIAL_MASK) != TERRAIN_VERTEX_MATERIAL_NONE) {
    /* all four point-A side bits set by some vertex and every point A projected */
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
      /* the deeper under water, the darker: lighting level minus half the (non-negative) secondary depth */
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
      /* soil packet table: 0x800 bytes per material, 0x100 per variant (flag bits 8..10); +0x20..+0xA0 are the
         blend packets towards other materials */
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
    if ((((((renderContext->contextFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY) != 0) ||
          (0 < vertex0->secondaryProjectionDepthQ12)) || (0 < vertex1->secondaryProjectionDepthQ12))
        || (0 < vertex2->secondaryProjectionDepthQ12)) &&
       (((vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags) &
        0x7c00000) == 0x3c00000)) { /* the same test for point B */
      vertex0->projectionFlags = vertex0->projectionFlags | TERRAIN_VERTEX_SECONDARY_VISIBLE;
      vertex1->projectionFlags = vertex1->projectionFlags | TERRAIN_VERTEX_SECONDARY_VISIBLE;
      vertex2->projectionFlags = vertex2->projectionFlags | TERRAIN_VERTEX_SECONDARY_VISIBLE;
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
   Narrows the per-row visible column spans (g_TerrainProjectedRowSpans) by one frustum side plane through the
   view origin: a plane with an x component moves the first or end column of every row to the column where the
   plane crosses that row (skewed by half a column per row); a plane parallel to the columns empties the rows
   on its far side.
*/
void TerrainProjectedGrid_ClipRowSpansAgainstPlane(FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal)

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
        fixedProduct = (int64_t)(boundOrCount + g_ViewOriginFixed.y) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        cutoffRow = (int)((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) >> 0xc;
        boundOrCount = fieldGrid->gridHeight - cutoffRow;
        if ((boundOrCount != 0 && cutoffRow <= (int)fieldGrid->gridHeight) && (boundOrCount = boundOrCount - 1, boundOrCount != 0))
        {
          /* rows cutoffRow + 3 on are emptied, so the last two emptied rows lie past the grid (the table has 260) */
          spanCursor = g_TerrainProjectedRowSpans + cutoffRow + 3;
          for (boundOrCount = boundOrCount * 2; boundOrCount != 0; boundOrCount--) {
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
        fixedProduct = (int64_t)(boundOrCount + g_ViewOriginFixed.y) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        boundOrCount = (int)((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) >> 0xc;
        if ((-1 < boundOrCount) && (boundOrCount != 0)) {
          spanCursor = g_TerrainProjectedRowSpans;
          for (boundOrCount = boundOrCount * 2; boundOrCount != 0; boundOrCount--) {
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
    fixedProduct = (int64_t)(int)(fixedProduct / (int64_t)planeNormal->x) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    columnEdgeQ12 = (int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14;
    /* column shift per row: 1999 = 2^32 / -FIELD_GRID_WORLD_Y_TO_ROW_Q20 is one row in world Y, then half a column of
       skew is subtracted */
    fixedProduct = (int64_t)(int)(((int64_t)planeNormal->y * 1999) / (int64_t)planeNormal->x) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    spanBoundCursor = (int *)THANDOR_ADDR(g_TerrainProjectedRowSpans,-8);
    rowsRemaining = fieldGrid->gridHeight;
    do {
      spanBoundCursor = spanBoundCursor + 2;
      boundOrCount = (int)(columnEdgeQ12 - 0x1000) >> 0xc;
      columnEdgeQ12 = columnEdgeQ12 + (((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) - 0x800);
      if (*spanBoundCursor < boundOrCount) {
        *spanBoundCursor = boundOrCount;
      }
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
    columnEdgeQ12 = (int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14;
    fixedProduct = (int64_t)(int)(((int64_t)planeNormal->y * 1999) / (int64_t)planeNormal->x) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
    spanBoundCursor = (int *)THANDOR_ADDR(g_TerrainProjectedRowSpans,-4);
    rowsRemaining = fieldGrid->gridHeight;
    do {
      spanBoundCursor = spanBoundCursor + 2;
      boundOrCount = (int)(columnEdgeQ12 + 0x1fff) >> 0xc;
      columnEdgeQ12 = columnEdgeQ12 + (((int)((uint64_t)fixedProduct >> 0x20) << 0xc | (uint32_t)fixedProduct >> 0x14) - 0x800);
      if (boundOrCount < *spanBoundCursor) {
        *spanBoundCursor = boundOrCount;
      }
      rowsRemaining--;
    } while (rowsRemaining != 0);
  }
  return;
}


/* Address: 0x005063B0.
   Line-of-sight leg along direction 0 (C+1, right): each cell's surface height (terrain plus positive water)
   above the eye (g_TerrainScanReferenceHeight) is scaled by the per-step table g_TerrainHeightDeltaScaleByStepQ12;
   a cell whose value reaches the highest value seen so far on this line is visible and gets occupancyMaskBits,
   and its value becomes the new horizon. 4 scan steps per cell, until the step limit or a map-edge cell.
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
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
   Original register convention: no result; EAX, ECX, EDX preserved; works on MMX register MM0.
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
   Overlay leg along direction 0 (C+1, right) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint:
   stores the overlay value (g_TerrainScanReferenceHeight) into runtimeOverlayOrHeightValue04 of every cell
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509380.
   Overlay leg along direction 1 (C+1-W, up and right) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005093F0.
   Overlay leg along direction 2 (C-W, up) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509450.
   Overlay leg along direction 3 (C-1, left) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell - 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005094B0.
   Overlay leg along direction 4 (C-1+W, down and left) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509520.
   Overlay leg along direction 5 (C+W, down) of FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509B90.
   Overlay leg along direction 0 (C+1, right) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint:
   stores the overlay value (g_TerrainScanReferenceHeight) into runtimeOverlayOrHeightValue04 of every cell
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509BF0.
   Overlay leg along direction 1 (C+1-W, up and right) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509C50.
   Overlay leg along direction 2 (C-W, up) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509CB0.
   Overlay leg along direction 3 (C-1, left) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = fieldCell - 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509D10.
   Overlay leg along direction 4 (C-1+W, down and left) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509D70.
   Overlay leg along direction 5 (C+W, down) of FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint;
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
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      fieldCell = (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes - 0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

