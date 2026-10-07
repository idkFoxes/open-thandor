/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/selection_overlay.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/selection_overlay.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

static ModelProjectedBoundsPixels g_ModelProjectedBoundsPixels = {};

/* Draws the metric bars (SelectionPanel_RenderArmyRuntimeMetrics) over every selected entity whose model node
   carries flag 4, or flag 8 without 0x10, at the screen bounds of its projected model hierarchy; entities whose
   bounds come out empty are skipped. Called by FrontendModelPointerContext_RenderWorldViewQueuesClipped when
   context flag 0x400 is set.
*/
void SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft)

{
  ModelRuntimeNode *modelNode;
  GameEntityRuntime *selectedEntity;
  Ptr32<GameEntityRuntime> *selectionSlots;
  int slotIndex;

  selectionSlots = g_SelectionInfoEntitySlots->entries;
  for (slotIndex = 0; slotIndex < SELECTION_ENTRY_CAPACITY; slotIndex++) {
    selectedEntity = selectionSlots[slotIndex];
    if (selectedEntity == nullptr) {
      continue;
    }
    modelNode = (selectedEntity->common).ownership.modelNode;
    if (!(Any(modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) ||
          (!Any(modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) &&
           (Any(modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE))))) {
      continue;
    }
    g_ModelProjectedBoundsPixels.minX = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
    g_ModelProjectedBoundsPixels.minY = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
    g_ModelProjectedBoundsPixels.maxX = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
    g_ModelProjectedBoundsPixels.maxY = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
    ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
    if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
       (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
      SelectionPanel_RenderArmyRuntimeMetrics
                (clipBottom,clipRight,clipTop,clipLeft,g_ModelProjectedBoundsPixels.maxY,
                 g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                 g_ModelProjectedBoundsPixels.minX,ModelView_Cast<RuntimeModelFactionPrefix>(selectedEntity));
    }
  }
}

/* Draws the metric bars of one entity like SelectionOverlay_RenderSelectedArmyMetrics, but with the info-panel
   texture and data (g_InfoPanelTextureSource/g_InfoPanelData) swapped in for the call. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped for its selectedOverlayEntity when that entity is in
   the selection info.
*/
void SelectionOverlay_RenderArmyMetricsForEntity
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GameEntityRuntime *entity)

{
  ModelRuntimeNode *modelNode;
  GraphicsTextureSourceAsset *savedTextureSource;
  void *savedPanelData;
  
  modelNode = (entity->common).ownership.modelNode;
  if (Any(modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) ||
     ((!Any(modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) && Any(modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE)))) {
    g_ModelProjectedBoundsPixels.minX = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
    g_ModelProjectedBoundsPixels.minY = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
    g_ModelProjectedBoundsPixels.maxX = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
    g_ModelProjectedBoundsPixels.maxY = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
    ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
    savedPanelData = g_SelectionPanelData;
    savedTextureSource = g_SelectionPanelTextureSource;
    /* no-op write-back; the original saves both only inside the if below */
    g_SelectionPanelTextureSource = savedTextureSource;
    g_SelectionPanelData = savedPanelData;
    if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
       (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
      g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
      g_SelectionPanelData = g_InfoPanelData;
      SelectionPanel_RenderArmyRuntimeMetrics
                (clipBottom,clipRight,clipTop,clipLeft,g_ModelProjectedBoundsPixels.maxY,
                 g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                 g_ModelProjectedBoundsPixels.minX,
                 ModelView_Cast<RuntimeModelFactionPrefix>(entity));
      g_SelectionPanelTextureSource = savedTextureSource;
      g_SelectionPanelData = savedPanelData;
    }
  }
}

/* Draws a frame around the screen rectangle spanned by corners A and B (either order; nothing when it is empty in
   either direction): four corner pieces outside the rectangle and the four edges tiled between them. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x80 is set (the drag-selection
   rectangle, WORLD_RUNTIME_FLAG_DRAG_SELECTING in the in-game world view).
*/
void SelectionOverlay_DrawBoundsFrame(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate cornerAY,UiPixelCoordinate cornerAX,
          UiPixelCoordinate cornerBY,UiPixelCoordinate cornerBX)

{
  int edgeX;
  int edgeY;
  UiPixelCoordinate originalCornerBY;
  UiPixelCoordinate originalCornerBX;
  uint32_t cornerWidth;
  bool accessFailed;
  GraphicsTextureLogicalSize cornerSize;
  
  /* order the corners: A becomes bottom-right (maximum), B top-left (minimum) */
  originalCornerBX = cornerBX;
  originalCornerBY = cornerBY;
  if (cornerAX <= cornerBX) {
    if (cornerBX == cornerAX) {
      return;
    }
    cornerBX = cornerAX;
    cornerAX = originalCornerBX;
  }
  if (cornerAY <= cornerBY) {
    if (cornerBY == cornerAY) {
      return;
    }
    cornerBY = cornerAY;
    cornerAY = originalCornerBY;
  }
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    cornerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_FRAME_TOP_LEFT,g_SelectionPanelTextureSource);
    cornerWidth = cornerSize.logicalWidthPixels;
    edgeX = cornerBX - cornerWidth;
    edgeY = cornerBY - cornerSize.logicalHeightPixels;
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,edgeY,edgeX,SELECTION_OVERLAY_FRAME_TOP_LEFT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,edgeY,cornerAX,SELECTION_OVERLAY_FRAME_TOP_RIGHT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,edgeX,SELECTION_OVERLAY_FRAME_BOTTOM_LEFT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,cornerAX,SELECTION_OVERLAY_FRAME_BOTTOM_RIGHT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    edgeX = edgeX + cornerWidth;
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,cornerAX,edgeY,edgeX,
               SELECTION_OVERLAY_FRAME_TOP,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,cornerAX,cornerAY,edgeX,
               SELECTION_OVERLAY_FRAME_BOTTOM,g_SelectionPanelTextureSource,g_FramebufferAccess);
    edgeY = edgeY + cornerSize.logicalHeightPixels;
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,GRAPHICS_TILED_BLIT_ONE_TILE,edgeY,
               edgeX - cornerWidth,SELECTION_OVERLAY_FRAME_LEFT,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,GRAPHICS_TILED_BLIT_ONE_TILE,edgeY,
               cornerAX,SELECTION_OVERLAY_FRAME_RIGHT,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
}

/* Draws the SELECTION_OVERLAY_MARKER_GRID_POINT marker centred on the screen position of each of markerPointCount
   grid coordinate pairs: each pair is converted to a world point, snapped to the nearest terrain point and
   projected; points off the field or not beyond the near plane are skipped. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped with its terrainMarkerCoordinatePairs when context
   flag 0x200000 is set.
*/
void SelectionOverlay_DrawTerrainPointMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markerPointCount,int *gridCoordinatePairs,
          FieldGridAsset *fieldGrid)

{
  int64_t worldXProduct;
  int64_t worldYProduct;
  int screenX;
  int screenY;
  bool accessFailed;
  GraphicsProjectedPointPair projectedPoint;
  GraphicsTextureLogicalSize markerSize;
  FixedVectorQ12 terrainPoint;
  uint32_t blitTextureId;
  GraphicsTextureSourceAsset *blitTextureSource;
  SoftwareFramebufferAccess *blitFramebuffer;
  
  if (markerPointCount != 0) {
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      while (markerPointCount != 0) {
        /* pair (a, b) -> world point: x = (2a + b) * 0x901 / 2^13, y = -b * 1999 / 2^12 (SHRD of the 64-bit
           products); 0x901 and 1999 are about one cell width 0x900 and 0x900 * sqrt(3) / 2, so the pairs look
           like triangular-lattice coordinates in Q12 */
        worldXProduct = (int64_t)(gridCoordinatePairs[1] + *gridCoordinatePairs * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
        worldYProduct = (int64_t)gridCoordinatePairs[1] * -1999;
        if (FieldGrid_GetNearestTerrainPoint
                      (FIXED_PRODUCT_SHR(worldYProduct,Q12_SHIFT),
                       FIXED_PRODUCT_SHR(worldXProduct,13),fieldGrid,&terrainPoint)) {
          g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = terrainPoint.xQ12;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = terrainPoint.yQ12;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = terrainPoint.zQ12;
          FixedTransform_ApplyPoint
                    (&g_GraphicsTransformInputScratchVec3,
                     reinterpret_cast<GraphicsFixedVec3 *>(&g_GraphicsTransformScratchMatrix3x4) /* basisRow0: the point */,
                     &g_ViewProjectionMatrixFixed);
          if (16 < g_GraphicsTransformInputScratchVec3.z) { /* in front of the near plane */
            projectedPoint = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
            screenX = projectedPoint.projectedX >> 12;
            screenY = projectedPoint.projectedY >> 12;
            blitTextureId = SELECTION_OVERLAY_MARKER_GRID_POINT;
            blitTextureSource = g_SelectionPanelTextureSource;
            blitFramebuffer = g_FramebufferAccess;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_GRID_POINT,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
          }
        }
        gridCoordinatePairs = gridCoordinatePairs + 2;
        markerPointCount--;
      }
      g_GraphicsFramebufferEndAccess();
    }
  }
}

/* Draws the SELECTION_OVERLAY_MARKER_WORLD_POINT marker centred on the projected nearest terrain point (or top
   surface point when useTopSurface is nonzero) of a world position; nothing when it is off the field. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped with the point in its surfaceHitWorldY/X when
   context flag 0x100000 is set and surfaceHitDepth is not WORLD_POINTER_NO_HIT; useTopSurface is set when the
   high byte of g_UiCommandModeGColorVariantLimit is nonzero.
*/
void SelectionOverlay_DrawWorldPointMarker
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int useTopSurface,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  bool accessFailed;
  GraphicsProjectedPointPair projectedPoint;
  GraphicsTextureLogicalSize markerSize;
  FixedVectorQ12 markerPoint;

  if (useTopSurface == 0) {
    if (!FieldGrid_GetNearestTerrainPoint(worldYQ12,worldXQ12,fieldGrid,&markerPoint)) {
      return;
    }
  }
  else {
    if (!FieldGrid_GetNearestTopSurfacePoint(worldYQ12,worldXQ12,fieldGrid,&markerPoint)) {
      return;
    }
  }
  g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = markerPoint.xQ12;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = markerPoint.yQ12;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = markerPoint.zQ12;
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformInputScratchVec3,
             reinterpret_cast<GraphicsFixedVec3 *>(&g_GraphicsTransformScratchMatrix3x4) /* basisRow0: the point */,&g_ViewProjectionMatrixFixed)
  ;
  projectedPoint = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_WORLD_POINT,
                                                       g_SelectionPanelTextureSource);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,
               (projectedPoint.projectedY >> 12) - ((int)markerSize.logicalHeightPixels >> 1),
               (projectedPoint.projectedX >> 12) - ((int)markerSize.logicalWidthPixels >> 1),
               SELECTION_OVERLAY_MARKER_WORLD_POINT,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
}

/* Draws the SELECTION_OVERLAY_MARKER_GRID_VERTEX marker at the projected position of every fourth field cell in
   both directions (rows and columns 1, 5, 9, ...), using the cells' projected point B instead of A
   when the high byte of g_UiCommandModeGColorVariantLimit is nonzero; cells whose point A was not projected are
   skipped. Called by FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x800000 is set.
*/
void SelectionOverlay_DrawGridVertexMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid)

{
  uint32_t gridColumns;
  int screenX;
  uint32_t columnCount;
  uint32_t columnsRemaining;
  uint32_t rowsRemaining;
  int screenY;
  uint8_t *vertexCursor;
  int coordinateOffset;
  bool accessFailed;
  GraphicsTextureLogicalSize markerSize;
  uint32_t blitTextureId;
  GraphicsTextureSourceAsset *blitTextureSource;
  SoftwareFramebufferAccess *blitFramebuffer;
  uint8_t *rowStart;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    coordinateOffset = 0;
    gridColumns = fieldGrid->gridWidth;
    columnCount = gridColumns >> 2;
    rowsRemaining = fieldGrid->gridHeight >> 2;
    /* row 1, column 1 */
    vertexCursor = Thandor_Bytes(&fieldGrid->cells[gridColumns + 1]);
    columnsRemaining = columnCount;
    rowStart = vertexCursor;
    if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
      coordinateOffset = 32; /* projectedPointB instead of projectedPointA (32 bytes further) */
    }
    while (-1 < (int)rowsRemaining) { /* gridHeight / 4 + 1 rows (rowsRemaining < 2^30) */
      while (-1 < (int)columnsRemaining) {
        if ((*Thandor_At<uint32_t>(vertexCursor,80) & TERRAIN_VERTEX_POINT_A_NOT_PROJECTED) == 0) {
          screenX = *Thandor_At<int>(vertexCursor + coordinateOffset,12) >> Q12_SHIFT;
          screenY = *Thandor_At<int>(vertexCursor + coordinateOffset,16) >> Q12_SHIFT;
          blitTextureId = SELECTION_OVERLAY_MARKER_GRID_VERTEX;
          blitTextureSource = g_SelectionPanelTextureSource;
          blitFramebuffer = g_FramebufferAccess;
          markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_GRID_VERTEX,
                                                             g_SelectionPanelTextureSource);
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,
                     screenY - ((int)markerSize.logicalHeightPixels >> 1),
                     screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
        }
        vertexCursor = vertexCursor + 4 * sizeof(FieldGridCell); /* four cells on */
        columnsRemaining = columnsRemaining - 1;
      } /* gridWidth / 4 + 1 cells per row */
      vertexCursor = rowStart + gridColumns * 4 * sizeof(FieldGridCell); /* four rows on */
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = columnCount;
      rowStart = vertexCursor;
    }
    g_GraphicsFramebufferEndAccess();
  }
}

/* Marks the field cells excluded from the fluid simulation: SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED and/or
   SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED at the cell's projected point B, skipping cells whose point B was
   not projected. Called by FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x1000000
   is set.
*/
void SelectionOverlay_DrawFluidExclusionMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid)

{
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  bool accessFailed;
  GraphicsTextureLogicalSize markerSize;
  uint32_t receiverTextureId;
  GraphicsTextureSourceAsset *receiverTextureSource;
  SoftwareFramebufferAccess *receiverFramebuffer;
  int savedScreenY;
  int savedScreenX;
  uint32_t sourceTextureId;
  GraphicsTextureSourceAsset *sourceTextureSource;
  SoftwareFramebufferAccess *sourceFramebuffer;
  FieldGridCell *rowStartCell;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    /* Original quirk: a do/while, so a count of 0 runs it 2^32 times (kept as in the original; step 11). */
    do {
      do {
        if ((!Any(cellCursor->flagsAndMaterial & FIELD_CELL_VERTEX_POINT_B_NOT_PROJECTED)) &&
           (Any(cellCursor->flagsAndMaterial & (FIELD_CELL_FLUID_SOURCE_EXCLUDED|FIELD_CELL_FLUID_RECEIVER_EXCLUDED)))) {
          screenX = cellCursor->secondarySurfaceScreenPoint.projectedX >> 12;
          screenY = cellCursor->secondarySurfaceScreenPoint.projectedY >> 12;
          sourceTextureId = SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED;
          receiverTextureId = SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED;
          sourceTextureSource = g_SelectionPanelTextureSource;
          sourceFramebuffer = g_FramebufferAccess;
          if (Any(cellCursor->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)) {
            receiverTextureSource = g_SelectionPanelTextureSource;
            receiverFramebuffer = g_FramebufferAccess;
            savedScreenY = screenY;
            savedScreenX = screenX;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),receiverTextureId,receiverTextureSource,receiverFramebuffer);
            screenY = savedScreenY;
            screenX = savedScreenX;
          }
          if (Any(cellCursor->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED)) {
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),sourceTextureId,sourceTextureSource,sourceFramebuffer);
          }
        }
        cellCursor = cellCursor + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      cellCursor = rowStartCell + gridColumns;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridColumns;
      rowStartCell = cellCursor;
    } while (rowsRemaining != 0);
    g_GraphicsFramebufferEndAccess();
  }
}

/* Marks the field cells that support Xenite or Tritium: SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE when the cell
   supports the resource selectedResourceIndex picks (0 Xenite, 1 Tritium), SELECTION_OVERLAY_MARKER_OTHER_RESOURCE
   when it supports the other one, at the cell's projected point A. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped with the low byte of its selectedResourceMarkerIndex when
   context flag 0x2000000 is set.
*/
void SelectionOverlay_DrawResourceCellMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,uint8_t selectedResourceIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  FieldCellPackedFlagsAndMaterial selectedResourceFlag;
  bool accessFailed;
  GraphicsTextureLogicalSize markerSize;
  uint32_t flaggedTextureId;
  GraphicsTextureSourceAsset *flaggedTextureSource;
  SoftwareFramebufferAccess *flaggedFramebuffer;
  int savedScreenY;
  int savedScreenX;
  uint32_t otherTextureId;
  GraphicsTextureSourceAsset *otherTextureSource;
  SoftwareFramebufferAccess *otherFramebuffer;
  FieldGridCell *rowStartCell;
  
  selectedResourceFlag = FieldCell_ResourceSupportBit(selectedResourceIndex & 31);
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    /* Original quirk: a do/while, so a count of 0 runs it 2^32 times (kept as in the original; step 11). */
    do {
      do {
        if ((!Any(cellCursor->flagsAndMaterial & FIELD_CELL_VERTEX_POINT_A_NOT_PROJECTED)) &&
           (Any(cellCursor->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK))) {
          screenX = cellCursor->groundScreenPoint.projectedX >> 12;
          screenY = cellCursor->groundScreenPoint.projectedY >> 12;
          otherTextureId = SELECTION_OVERLAY_MARKER_OTHER_RESOURCE;
          flaggedTextureId = SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE;
          otherTextureSource = g_SelectionPanelTextureSource;
          otherFramebuffer = g_FramebufferAccess;
          if (Any(cellCursor->flagsAndMaterial & selectedResourceFlag)) {
            flaggedTextureSource = g_SelectionPanelTextureSource;
            flaggedFramebuffer = g_FramebufferAccess;
            savedScreenY = screenY;
            savedScreenX = screenX;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),flaggedTextureId,flaggedTextureSource,flaggedFramebuffer);
            screenY = savedScreenY;
            screenX = savedScreenX;
          }
          if (Any(cellCursor->flagsAndMaterial & (selectedResourceFlag ^ FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)))
          {
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_OTHER_RESOURCE,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),otherTextureId,otherTextureSource,otherFramebuffer);
          }
        }
        cellCursor = cellCursor + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      cellCursor = rowStartCell + gridColumns;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridColumns;
      rowStartCell = cellCursor;
    } while (rowsRemaining != 0);
    g_GraphicsFramebufferEndAccess();
  }
}

/* Debug overlay: draws the SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED marker at the projected point A of
   every field cell with FIELD_CELL_DEBUG_MARKED (which no code in the game sets). Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x4000 and g_UiCommandRuntimeFlags
   bit 0x40 are set and a field grid is attached.
*/
void SelectionOverlay_DrawDebugMarkedCellMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid)

{
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  bool accessFailed;
  GraphicsTextureLogicalSize markerSize;
  uint32_t blitTextureId;
  GraphicsTextureSourceAsset *blitTextureSource;
  SoftwareFramebufferAccess *blitFramebuffer;
  FieldGridCell *rowStartCell;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    /* Original quirk: a do/while, so a count of 0 runs it 2^32 times (kept as in the original; step 11). */
    do {
      do {
        if ((Any(cellCursor->flagsAndMaterial & FIELD_CELL_DEBUG_MARKED)) &&
           (!Any(cellCursor->flagsAndMaterial & FIELD_CELL_VERTEX_POINT_A_NOT_PROJECTED))) {
          screenX = cellCursor->groundScreenPoint.projectedX >> 12;
          screenY = cellCursor->groundScreenPoint.projectedY >> 12;
          blitTextureId = SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED;
          blitTextureSource = g_SelectionPanelTextureSource;
          blitFramebuffer = g_FramebufferAccess;
          markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED,
                                                             g_SelectionPanelTextureSource);
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,
                     screenY - ((int)markerSize.logicalHeightPixels >> 1),
                     screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
        }
        cellCursor = cellCursor + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      cellCursor = rowStartCell + gridColumns;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridColumns;
      rowStartCell = cellCursor;
    } while (rowsRemaining != 0);
    g_GraphicsFramebufferEndAccess();
  }
}
