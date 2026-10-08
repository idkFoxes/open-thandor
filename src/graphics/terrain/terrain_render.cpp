/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/terrain/terrain_render.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/terrain/terrain_render.h>
#include <thandor/thandor.h>
#include <thandor/core/color_lanes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/record_bytes.h>

/* The projection work record of a field grid cell: the terrain pass reuses the 0x80-byte cells of the field grid
   as its vertex records. */
static inline TerrainProjectedVertexWorkRecord *TerrainVertex_OfCell(FieldGridCell *cell)
{
  return reinterpret_cast<TerrainProjectedVertexWorkRecord *>(cell);
}

/* The vertex one grid row below (rowStrideBytes: one grid row of vertex records). */
static inline TerrainProjectedVertexWorkRecord *TerrainVertex_RowBelow(TerrainProjectedVertexWorkRecord *vertex,
                                                                       uint32_t rowStrideBytes)
{
  return reinterpret_cast<TerrainProjectedVertexWorkRecord *>(reinterpret_cast<uint8_t *>(vertex) + rowStrideBytes);
}

/* Module data. */

/* Entries of g_TerrainProjectedRowSpans, and the most grid rows TerrainProjectedGrid_TransformShadeAndQueue draws
   (the clip pass empties rows up to gridHeight + 1; 257 is the limit the table was sized for). */
static constexpr int TERRAIN_PROJECTED_ROW_SPAN_COUNT = 260;
static constexpr int TERRAIN_PROJECTED_GRID_MAX_ROWS = 257;

/* per-row visible column spans of the terrain projection, all zero at start (the original held 0x90 fill bytes
   in entry 259; the read loops stop at row gridHeight - 1 <= 256, only the clip pass's emptying writes reach it) */
static TerrainProjectedRowSpan g_TerrainProjectedRowSpans[TERRAIN_PROJECTED_ROW_SPAN_COUNT] = {
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
    /* 250 */ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

bool g_Triangle2DBarycentricOutside;

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
  static bool loggedGridRejected;

  /* The original trusted the FLD grid size; a grid that does not fit the span table (rows 0..256 plus the
     emptied rows past the grid) or has fewer than two rows or no columns is not drawn here, because the span
     loops below then ran outside g_TerrainProjectedRowSpans or wrapped around. */
  if (fieldGrid->gridHeight < 2 || fieldGrid->gridHeight > TERRAIN_PROJECTED_GRID_MAX_ROWS ||
      fieldGrid->gridWidth == 0) {
    if (!loggedGridRejected) {
      loggedGridRejected = true;
      Thandor_Log("TerrainProjectedGrid_TransformShadeAndQueue: grid %ux%u not drawn",
                  fieldGrid->gridWidth,fieldGrid->gridHeight);
    }
    return;
  }
  if (!Any(renderContext->contextFlags & TERRAIN_RENDER_REUSE_PROJECTION)) {
    rowSpan = g_TerrainProjectedRowSpans;
    gridWidth = fieldGrid->gridWidth;
    rowCount = fieldGrid->gridHeight;
    /* count = gridHeight >= 2 (the grid check above) */
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
    /* counts = gridHeight >= 2 and gridWidth >= 1 (the grid check above) */
    do {
      do {
        rowCells->flagsAndMaterial =
             rowCells->flagsAndMaterial | (FIELD_CELL_VERTEX_POINT_A_NOT_PROJECTED | FIELD_CELL_VERTEX_POINT_B_NOT_PROJECTED);
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
    /* count = gridHeight - 1 >= 1 (the grid check above) */
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
  if (!Any(fieldGrid->runtimeStateFlags & FIELD_GRID_RUNTIME_SURFACE_DIRTY) &&
     (Any(renderContext->contextFlags & TERRAIN_RENDER_REUSE_PROJECTION))) {
    rowCells = fieldGrid->cells;
    /* count = gridHeight >= 2 (the grid check above) */
    do {
      spanFirstColumn = rowSpan->firstColumn;
      vertexCount = rowSpan->endColumnExclusive - spanFirstColumn;
      if (vertexCount != 0 && spanFirstColumn <= rowSpan->endColumnExclusive) {
        vertexCursor = TerrainVertex_OfCell(rowCells + spanFirstColumn);
        for (; vertexCount != 0; vertexCount = vertexCount - 1) {
          TerrainProjectedVertex_ReshadeKeepingProjection(vertexCursor);
          vertexCursor = vertexCursor + 1;
        }
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
    /* count = gridHeight >= 2 (the grid check above) */
    do {
      spanFirstColumn = rowSpan->firstColumn;
      vertexCount = rowSpan->endColumnExclusive - spanFirstColumn;
      if (vertexCount != 0 && spanFirstColumn <= rowSpan->endColumnExclusive) {
        vertexCursor = TerrainVertex_OfCell(rowCells + spanFirstColumn);
        for (; vertexCount != 0; vertexCount = vertexCount - 1) {
          TerrainProjectedVertex_TransformProjectAndShade(vertexCursor);
          vertexCursor = vertexCursor + 1;
        }
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
  /* count = gridHeight - 1 >= 1 (the grid check above) */
  do {
    spanFirstColumn = rowSpan->firstColumn;
    vertexCount = rowSpan->endColumnExclusive - spanFirstColumn;
    if (vertexCount != 0 && spanFirstColumn <= rowSpan->endColumnExclusive) {
      quadCount = vertexCount - 1;
      if (quadCount != 0) {
        vertexCursor = TerrainVertex_OfCell(rowCells + spanFirstColumn);
        for (; quadCount != 0; quadCount = quadCount - 1) {
          TerrainProjectedQuad_QueueAsTwoTriangles(gridWidth * sizeof(FieldGridCell),vertexCursor,renderContext);
          vertexCursor = vertexCursor + 1;
        }
      }
    }
    rowSpan = rowSpan + 1;
    rowCells = rowCells + gridWidth;
    quadRowsLeft = quadRowsLeft - 1;
  } while (quadRowsLeft != 0);
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
             TerrainVertex_RowBelow(topLeftVertex,rowStrideBytes),topLeftVertex,
             renderContext);
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             TerrainVertex_RowBelow(topLeftVertex + 1,rowStrideBytes),
             TerrainVertex_RowBelow(topLeftVertex,rowStrideBytes),renderContext);
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
  lightingFactors = ColorLanes_UnpackBytesShiftRight(vertexColor,6);
  if (vertex->lightingLookupIndexOrSentinel == FIELD_CELL_LIGHTING_VISIBLE) {
    lightingFactors = GraphicsShadingRuntime_AccumulateCompactLightingAtPoint(viewPoint,lightingFactors);
  }
  shadedProduct = pmulhw(lightingFactors,ColorLanes_UnpackBytesShiftRight(baseColor,2));
  return ColorLanes_PackWordsUnsignedSaturate(shadedProduct);
}

/* Point B of a terrain vertex (shared by the two vertex updates below): point B = terrain point + secondaryOffset
   + (0, 0, secondaryProjectionDepthQ12), built in sourcePoint and undone after the transform into viewPointB.
   When it lies beyond the near plane it is projected into projectedPointB. Returns flagsWithoutPointB (point-B
   bits clear) with the point-B clip side bits, or with TERRAIN_VERTEX_POINT_B_NOT_PROJECTED. */
static uint32_t TerrainProjectedVertex_TransformAndProjectPointB
          (TerrainProjectedVertexWorkRecord *vertex,uint32_t flagsWithoutPointB)

{
  const TerrainDirectionRecord *offsetRecord;
  int offsetX;
  int offsetY;
  int offsetZ;
  uint32_t resultFlags;
  GraphicsProjectedPointPair projectedPoint;

  /* the offset is the first three dwords of the cell's direction record; secondaryOffset is its index, 0..255 by
     construction (FieldGrid_InitializeRuntimeCellsAndBoundaryFlags), the mask keeps any other value in the table */
  offsetRecord = &g_TerrainDirectionRecordTable256[vertex->secondaryOffset & 0xffU];
  resultFlags = flagsWithoutPointB | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED;
  offsetX = (int)offsetRecord->angleAComponent0ScaledQ28;
  offsetY = (int)offsetRecord->angleAComponent1ScaledQ28;
  offsetZ = (int)offsetRecord->angleBComponent0ScaledQ28 + vertex->secondaryProjectionDepthQ12;
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
  bool outsideTriangle;
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
                     (Asset_RecordAt<uint32_t>(soilPacketTable,packetOffset),vertex2Color,vertex1Color,vertex0Color,
                      TerrainVertex_AsProjectedSource(vertex2),TerrainVertex_AsProjectedSource(vertex1),
                      TerrainVertex_AsProjectedSource(vertex0),renderContext);
  if (queuedPacket != nullptr) {
    queuedPacket->renderFlags = queuedPacket->renderFlags | layerFlags;
  }
  return queuedPacket;
}

/* True when the vertex's material block (TERRAIN_SOIL_PACKET_MATERIAL_BYTES, every packet of its variants and
   blends) lies inside the loaded soil packet table. */
static bool TerrainProjectedTriangle_SoilMaterialInTable(const TerrainProjectedVertexWorkRecord *vertex)
{
  return ((vertex->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) + 1) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES <=
         g_TerrainSoilPacketTablePayloadBytes;
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
  static bool loggedMaterialOutsideTable;

  soilPacketTable = g_TerrainSoilPacketTablePayload;
  /* The original indexed the soil packet table by the vertex materials unchecked; a triangle with a material
     whose 0x800-byte block lies past the loaded table is skipped here, because the reads ran past the table. */
  if (!TerrainProjectedTriangle_SoilMaterialInTable(vertex0) ||
      !TerrainProjectedTriangle_SoilMaterialInTable(vertex1) ||
      !TerrainProjectedTriangle_SoilMaterialInTable(vertex2)) {
    if (!loggedMaterialOutsideTable) {
      loggedMaterialOutsideTable = true;
      Thandor_Log("TerrainProjectedTriangle_QueueSoilTriangles: skipped triangle, material outside the soil table "
                  "(%u bytes)",g_TerrainSoilPacketTablePayloadBytes);
    }
    return;
  }
  vertex0Color = vertex0->shadedColorA;
  vertex1Color = vertex1->shadedColorA;
  vertex2Color = vertex2->shadedColorA;
  /* PUNPCKLBW/PSRLW 4 of each color, PMULHW by its lighting level, PACKUSWB */
  litProduct0 = pmulhw(ColorLanes_UnpackBytesShiftRight(vertex0Color,4),
                       g_PackedLightingLookupTable[TerrainProjectedTriangle_SoilLightingLevel(vertex0)]);
  litProduct1 = pmulhw(ColorLanes_UnpackBytesShiftRight(vertex1Color,4),
                       g_PackedLightingLookupTable[TerrainProjectedTriangle_SoilLightingLevel(vertex1)]);
  litProduct2 = pmulhw(ColorLanes_UnpackBytesShiftRight(vertex2Color,4),
                       g_PackedLightingLookupTable[TerrainProjectedTriangle_SoilLightingLevel(vertex2)]);
  vertex0Color = ColorLanes_PackWordsUnsignedSaturate(litProduct0);
  vertex1Color = ColorLanes_PackWordsUnsignedSaturate(litProduct1);
  vertex2Color = ColorLanes_PackWordsUnsignedSaturate(litProduct2);
  /* soil packet table: 0x800 bytes per material, 0x100 per variant (flag bits 8..10); +0x20..+0xA0 are the
     blend packets towards other materials */
  materialOffset0 = (vertex0->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES;
  materialOffset1 = (vertex1->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES;
  materialOffset2 = (vertex2->projectionFlags & TERRAIN_VERTEX_MATERIAL_MASK) * TERRAIN_SOIL_PACKET_MATERIAL_BYTES;
  packetOffset1 = materialOffset1 + (vertex1->projectionFlags & TERRAIN_VERTEX_VARIANT_OFFSET_MASK);
  packetOffset2 = materialOffset2 + (vertex2->projectionFlags & TERRAIN_VERTEX_VARIANT_OFFSET_MASK);
  queuedPacket = GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle
                     (reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(soilPacketTable) +
                               materialOffset0 + (vertex0->projectionFlags & TERRAIN_VERTEX_VARIANT_OFFSET_MASK)),
                      vertex2Color,vertex1Color,vertex0Color,TerrainVertex_AsProjectedSource(vertex2),
                      TerrainVertex_AsProjectedSource(vertex1),TerrainVertex_AsProjectedSource(vertex0),
                      renderContext);
  if (queuedPacket == nullptr) {
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
    if (queuedPacket != nullptr) {
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
    if (((((Any(renderContext->contextFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY)) ||
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
      litProduct0 = pmulhw(ColorLanes_UnpackBytesShiftRight(vertex0Color,4),
                           g_PackedLightingLookupTable[vertex0->lightingLookupIndexOrSentinel]);
      litProduct1 = pmulhw(ColorLanes_UnpackBytesShiftRight(vertex1Color,4),
                           g_PackedLightingLookupTable[vertex1->lightingLookupIndexOrSentinel]);
      litProduct2 = pmulhw(ColorLanes_UnpackBytesShiftRight(vertex2Color,4),
                           g_PackedLightingLookupTable[vertex2->lightingLookupIndexOrSentinel]);
      GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
                (Asset_RecordAt<uint32_t>(g_TerrainSurfacePacketTablePayload,
                              surfacePacketIndex * TERRAIN_SURFACE_PACKET_BYTES),
                 ColorLanes_PackWordsUnsignedSaturate(litProduct2),
                 ColorLanes_PackWordsUnsignedSaturate(litProduct1),
                 ColorLanes_PackWordsUnsignedSaturate(litProduct0),
                 TerrainVertex_AsProjectedSource(vertex2),TerrainVertex_AsProjectedSource(vertex1),
                 TerrainVertex_AsProjectedSource(vertex0),renderContext);
    }
  }
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
  int firstRowToEmpty;
  int endRowToEmpty;
  int rowIndex;
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
          /* rows cutoffRow + 3 .. gridHeight + 1 are emptied, so the last two emptied rows lie past the grid.
             The original wrote before the table for a camera more than three rows south of the grid (cutoffRow
             <= -4); the range is clamped to the table here because those writes hit other data. */
          firstRowToEmpty = cutoffRow + 3;
          endRowToEmpty = cutoffRow + 3 + (rowsFromCutoff - 1);
          if (firstRowToEmpty < 0) {
            firstRowToEmpty = 0;
          }
          if (endRowToEmpty > TERRAIN_PROJECTED_ROW_SPAN_COUNT) {
            endRowToEmpty = TERRAIN_PROJECTED_ROW_SPAN_COUNT;
          }
          for (rowIndex = firstRowToEmpty; rowIndex < endRowToEmpty; rowIndex++) {
            g_TerrainProjectedRowSpans[rowIndex].firstColumn = 0;
            g_TerrainProjectedRowSpans[rowIndex].endColumnExclusive = 0;
          }
        }
      }
      else if (-1 < cutoffRow) {
        /* rows 0..cutoffRow-1 are emptied; the original ran past the table for a camera far north of the grid
           (cutoffRow > 260), clamped to the table here */
        endRowToEmpty = cutoffRow;
        if (endRowToEmpty > TERRAIN_PROJECTED_ROW_SPAN_COUNT) {
          endRowToEmpty = TERRAIN_PROJECTED_ROW_SPAN_COUNT;
        }
        for (rowIndex = 0; rowIndex < endRowToEmpty; rowIndex++) {
          g_TerrainProjectedRowSpans[rowIndex].firstColumn = 0;
          g_TerrainProjectedRowSpans[rowIndex].endColumnExclusive = 0;
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
    /* count = gridHeight >= 2: only called from TerrainProjectedGrid_TransformShadeAndQueue after its grid check */
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
    /* count = gridHeight >= 2: only called from TerrainProjectedGrid_TransformShadeAndQueue after its grid check */
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
}

/* Fills one packet vertex from a terrain vertex's second screen/depth block for
   GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle; the colour is masked with
   g_UiCommandModeGColorVariantLimit when the vertex's secondary projection depth is negative. */
static void GraphicsPrimitiveVertex_SetFromTerrainSecondarySurface(GraphicsPrimitiveVertexRaw *vertex,
                                                                   const TerrainProjectedVertexWorkRecord *source,
                                                                   PackedArgb32 diffuseColor)
{
  if (source->secondaryProjectionDepthQ12 < 0) {
    diffuseColor = diffuseColor & g_UiCommandModeGColorVariantLimit;
  }
  vertex->screenX = source->projectedPointB.projectedX;
  vertex->screenY = source->projectedPointB.projectedY;
  vertex->diffuseColor = diffuseColor;
  vertex->backendCoord0 = source->viewPointB.x;
  vertex->backendCoord1 = source->viewPointB.y;
  vertex->depth = source->viewPointB.z;
}

/* Terrain counterpart of GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle for the second projected surface:
   appends a packet from the terrain vertices' second screen/depth block (projectedPointB, viewPointB), the per-vertex
   colours (masked with g_UiCommandModeGColorVariantLimit for vertices whose secondaryProjectionDepthQ12 is
   negative) and the texture
   coordinates of terrainPacketRecord (u0,v0,u1,v1,u2,v2, texture index, palette entry). Blend mode 6; textured
   with g_TerrainPrimaryTextureSet when the index is in range, modulated by g_TerrainPrimaryPalette. Returns the
   packet, or NULL when the queue is full (one slot is always left unused). Called by
   TerrainProjectedTriangle_ClipInterpolateAndQueueTextured.
*/
GraphicsPrimitivePacket *GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
          (uint32_t *terrainPacketRecord,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContext *renderContext)

{
  GraphicsPrimitiveQueue *primitiveQueue;
  uint32_t packetIndex;
  uint32_t textureEntryIndex;
  GraphicsTextureSet *terrainTextureSet;
  PackedArgb32 paletteModulationColor;
  GraphicsPrimitivePacket *newPacket;

  primitiveQueue = renderContext->activePrimitiveQueue;
  packetIndex = primitiveQueue->count;
  if (packetIndex + 1 >= primitiveQueue->capacity) {
    /* Queue full (the original returned no packet here; no caller reads the result) */
    return nullptr;
  }
  primitiveQueue->count = packetIndex + 1;
  newPacket = primitiveQueue->packetPool + packetIndex;
  primitiveQueue->primaryNodes[packetIndex].packet = newPacket;
  /* the vertices are TerrainProjectedVertexWorkRecords (passed with the GraphicsProjectedVertexSource type) */
  GraphicsPrimitiveVertex_SetFromTerrainSecondarySurface(&newPacket->vertices[0],
          TerrainVertex_FromProjectedSource(vertex0Projected),vertex0DiffuseColor);
  GraphicsPrimitiveVertex_SetFromTerrainSecondarySurface(&newPacket->vertices[1],
          TerrainVertex_FromProjectedSource(vertex1Projected),vertex1DiffuseColor);
  GraphicsPrimitiveVertex_SetFromTerrainSecondarySurface(&newPacket->vertices[2],
          TerrainVertex_FromProjectedSource(vertex2Projected),vertex2DiffuseColor);
  newPacket->vertices[0].textureU = terrainPacketRecord[0];
  newPacket->vertices[1].textureU = terrainPacketRecord[2];
  newPacket->vertices[2].textureU = terrainPacketRecord[4];
  newPacket->vertices[0].textureV = terrainPacketRecord[1];
  newPacket->vertices[1].textureV = terrainPacketRecord[3];
  newPacket->vertices[2].textureV = terrainPacketRecord[5];
  paletteModulationColor = 0;
  /* the palette index is bounded here (the original read past the palette for an index past its entries) */
  if (g_TerrainPrimaryPalette != nullptr && terrainPacketRecord[7] < g_TerrainPrimaryPalette->paletteBankCount) {
    paletteModulationColor = g_TerrainPrimaryPalette->paletteEntries[terrainPacketRecord[7]].
            alternateModulationColorArgb;
  }
  newPacket->renderFlags = GRAPHICS_PRIMITIVE_BLEND_ALPHA_DEPTH_WRITE;
  newPacket->modulationColor = paletteModulationColor;
  terrainTextureSet = g_TerrainPrimaryTextureSet;
  textureEntryIndex = terrainPacketRecord[6];
  newPacket->textureEntry = nullptr;
  if ((terrainTextureSet != nullptr) && (textureEntryIndex < terrainTextureSet->subresourceCount)) {
    newPacket->renderFlags = newPacket->renderFlags | GRAPHICS_PRIMITIVE_FLAG_TEXTURED;
    newPacket->textureEntry = terrainTextureSet->entries + textureEntryIndex;
  }
  return newPacket;
}

/* Fills one packet vertex for GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle: the five projected
   attribute dwords (texturedPacketAttributes[0..4]) are screenX, screenY, backendCoord0, backendCoord1 and depth. */
static void GraphicsPrimitiveVertex_SetFromProjectedAttributes(GraphicsPrimitiveVertexRaw *vertex,
                                                               const GraphicsProjectedVertexSource *source,
                                                               PackedArgb32 diffuseColor)
{
  vertex->screenX = (GraphicsPrimitiveScreenCoordinate)source->texturedPacketAttributes[0];
  vertex->screenY = (GraphicsPrimitiveScreenCoordinate)source->texturedPacketAttributes[1];
  vertex->diffuseColor = diffuseColor;
  vertex->backendCoord0 = (GraphicsPrimitiveBackendCoordinate)source->texturedPacketAttributes[2];
  vertex->backendCoord1 = (GraphicsPrimitiveBackendCoordinate)source->texturedPacketAttributes[3];
  vertex->depth = (GraphicsPrimitiveDepthFixed)source->texturedPacketAttributes[4];
}

/* Appends a textured terrain triangle: copies each terrain vertex's screen position, backend coordinates and
   depth (texturedPacketAttributes) and its colour, the texture coordinates of terrainPacketRecord (u0,v0,u1,v1,u2,v2,
   texture-set index, palette entry), the modulation colour from g_TerrainSecondaryPalette, the first texture
   of g_TerrainMaterialTextureSets[index] and the render flags g_UiCommandModeGColorVariantFlags. Returns the
   packet, or NULL when the queue is full (one slot is always left unused). Called by
   TerrainProjectedTriangle_ClipInterpolateAndQueueTextured.
*/
GraphicsPrimitivePacket *GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle
          (uint32_t *terrainPacketRecord,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContext *renderContext)

{
  GraphicsPrimitiveQueue *primitiveQueue;
  uint32_t packetIndex;
  GraphicsTextureSet *materialTextureSet;
  PackedArgb32 paletteModulationColor;
  GraphicsPrimitivePacket *newPacket;
  static bool loggedMissingMaterial;

  /* The original dereferenced the material's texture set unchecked; a packet whose material index is out of
     range or names an optional material that was not loaded (a NULL entry) is skipped here, as if the queue
     were full, because the original crashed on it. */
  if (terrainPacketRecord[6] >= sizeof(g_TerrainMaterialTextureSets) / sizeof(g_TerrainMaterialTextureSets[0]) ||
      g_TerrainMaterialTextureSets[terrainPacketRecord[6]] == nullptr) {
    if (!loggedMissingMaterial) {
      loggedMissingMaterial = true;
      Thandor_Log("GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle: skipped packets of missing material %u",
                  terrainPacketRecord[6]);
    }
    return nullptr;
  }
  primitiveQueue = renderContext->activePrimitiveQueue;
  packetIndex = primitiveQueue->count;
  if (packetIndex + 1 >= primitiveQueue->capacity) {
    return nullptr;
  }
  primitiveQueue->count = packetIndex + 1;
  newPacket = primitiveQueue->packetPool + packetIndex;
  primitiveQueue->primaryNodes[packetIndex].packet = newPacket;
  GraphicsPrimitiveVertex_SetFromProjectedAttributes(&newPacket->vertices[0],vertex0Projected,vertex0DiffuseColor);
  GraphicsPrimitiveVertex_SetFromProjectedAttributes(&newPacket->vertices[1],vertex1Projected,vertex1DiffuseColor);
  GraphicsPrimitiveVertex_SetFromProjectedAttributes(&newPacket->vertices[2],vertex2Projected,vertex2DiffuseColor);
  newPacket->vertices[0].textureU = (GraphicsPrimitiveTextureCoordinateFixed)terrainPacketRecord[0];
  newPacket->vertices[1].textureU = (GraphicsPrimitiveTextureCoordinateFixed)terrainPacketRecord[2];
  newPacket->vertices[2].textureU = (GraphicsPrimitiveTextureCoordinateFixed)terrainPacketRecord[4];
  newPacket->vertices[0].textureV = (GraphicsPrimitiveTextureCoordinateFixed)terrainPacketRecord[1];
  newPacket->vertices[1].textureV = (GraphicsPrimitiveTextureCoordinateFixed)terrainPacketRecord[3];
  newPacket->vertices[2].textureV = (GraphicsPrimitiveTextureCoordinateFixed)terrainPacketRecord[5];
  paletteModulationColor = 0;
  /* the palette index is bounded here (the original read past the palette for an index past its entries) */
  if (g_TerrainSecondaryPalette != nullptr && terrainPacketRecord[7] < g_TerrainSecondaryPalette->paletteBankCount) {
    paletteModulationColor = g_TerrainSecondaryPalette->paletteEntries[terrainPacketRecord[7]].
            alternateModulationColorArgb;
  }
  newPacket->modulationColor = paletteModulationColor;
  materialTextureSet = g_TerrainMaterialTextureSets[terrainPacketRecord[6]];
  newPacket->renderFlags = FromBits<GraphicsPrimitiveDispatchFlags>(g_UiCommandModeGColorVariantFlags);
  newPacket->textureEntry = materialTextureSet->entries;
  return newPacket;
}

/* Computes the barycentric weights of a screen point for vertices A and B of a projected triangle
   (C's weight is the remainder to 1.0), used to interpolate texture/shade values when a clipped
   terrain triangle is queued. Returns both weights in Q12 packed in one struct and publishes "point outside
   the triangle" (a second result in the original) in g_Triangle2DBarycentricOutside.
   Called directly by the terrain projection code (TerrainProjectedTriangle_ClipInterpolateAndQueueTextured).
*/
TriangleBarycentricWeightsQ12
Triangle2D_ComputeBarycentricWeightsQ12Packed
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  /* "Point outside the triangle" is published in g_Triangle2DBarycentricOutside. Weights are Q16
     internally and returned >> 4. */
  int64_t denominator;
  int64_t numerator;
  int denominatorShifted;
  int denominatorHigh;
  int weightA;
  int weightB;
  TriangleBarycentricWeightsQ12 result;

  g_Triangle2DBarycentricOutside = true;
  result.weightVertexA_Q12 = 0;
  result.weightVertexB_Q12 = 0;
  if ((pointX > vertexCX && pointX > vertexBX && pointX > vertexAX) ||
      (pointX < vertexCX && pointX < vertexBX && pointX < vertexAX) ||
      (pointY > vertexCY && pointY > vertexBY && pointY > vertexAY) ||
      (pointY < vertexCY && pointY < vertexBY && pointY < vertexAY)) {
    return result;
  }
  denominator = (int64_t)vertexCX * (vertexAY - vertexBY) + (int64_t)vertexBX * (vertexCY - vertexAY) +
                (int64_t)vertexAX * (vertexBY - vertexCY);
  denominatorHigh = (int)(denominator >> 32);
  denominatorShifted = (int)(denominator >> 16);
  if (denominatorShifted == 0) {
    return result;
  }
  numerator = (int64_t)(pointY - vertexBY) * vertexCX + (int64_t)(vertexCY - pointY) * vertexBX +
              (int64_t)(vertexBY - vertexCY) * pointX;
  if (denominatorHigh >= 0 ? (int)(numerator >> 32) > denominatorHigh
                           : (int)(numerator >> 32) < denominatorHigh) {
    return result;
  }
  weightA = (int)(numerator / denominatorShifted);
  if (weightA < 0 || weightA > TRIANGLE_BARYCENTRIC_WEIGHT_ONE_Q16) {
    return result;
  }
  numerator = (int64_t)(vertexAY - pointY) * vertexCX + (int64_t)(vertexCY - vertexAY) * pointX +
              (int64_t)(pointY - vertexCY) * vertexAX;
  if (denominatorHigh >= 0 ? (int)(numerator >> 32) > denominatorHigh
                           : (int)(numerator >> 32) < denominatorHigh) {
    return result;
  }
  weightB = (int)(numerator / denominatorShifted);
  if (weightB < 0 || weightA + weightB > TRIANGLE_BARYCENTRIC_WEIGHT_ONE_Q16) {
    return result;
  }
  result.weightVertexB_Q12 = weightB >> 4;
  result.weightVertexA_Q12 = weightA >> 4;
  g_Triangle2DBarycentricOutside = false;
  return result;
}
