/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/menu_room.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/menu_room.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(8) GraphicsTextureSourceBlitProc *g_SelectionPanelBlitOpaque = nullptr;

THANDOR_ALIGN(4) GraphicsTextureSourceTiledBlitProc *g_SelectionPanelBlitClipped = nullptr;

static const UQ12 g_WorldMotionTargetDistanceConvergenceStepQ12 = 512;

static const int g_WorldMotionPointerWheelInputScale = -64;

/* hit priority of a model under the pointer, by its model runtime class id (24 classes; the original laid it
   out as RuntimeModelClassPriorityTable24) */
static const RuntimeModelClassPriority g_RuntimeModelClassPriorityByModelClassId[24] = {
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /*  0 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  1 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  2 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  3 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /*  4 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  5 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  6 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  7 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  8 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /*  9 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 10 */
    RUNTIME_MODEL_CLASS_PRIORITY_MEDIUM, /* 11 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 12 */
    RUNTIME_MODEL_CLASS_PRIORITY_MEDIUM, /* 13 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 14 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 15 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 16 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /* 17 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /* 18 */
    RUNTIME_MODEL_CLASS_PRIORITY_HIGH, /* 19 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 20 */
    RUNTIME_MODEL_CLASS_PRIORITY_LOW, /* 21 */
    RUNTIME_MODEL_CLASS_PRIORITY_MEDIUM, /* 22 */
    RUNTIME_MODEL_CLASS_PRIORITY_MEDIUM  /* 23 */
};

/* two spline keyframes (0 = current camera, 1 = target record's camera), passed as an array to
   WorldMotionSpline_BuildSixChannelCurves */
static WorldMotionSplineKeyframe g_FrontendRomTransitionKeyframes[2] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, /* keyframe 0 */
    {0, 0, 0, 0, 0, 0, 0, 0}, /* keyframe 1 */
};

/* Pointer-move handler of the model pointer context (pointerMove of g_FrontendModelPointerContextVtable): stores
   the best model hit under the pointer, then returns the cursor frame. While a non-right button is held
   (ROUTE_TO_SECONDARY_CALLBACK) heldButtonCursorCallback decides it, with no button hoverCursorCallback;
   while the right button drags the camera (ROUTE_TO_BUILTIN_ACTION_RESOLUTION) the frame shows the camera
   motion FrontendModelPointerContext_DispatchWorldCameraPointerInput will perform for the camera scheme bits,
   the left button and the modifier keys (1 move, 0x0F pitch, 0x10 heading and pitch, 0x11 distance, 0x25
   heading, 0x0E heading and distance, 0x12..0x14 the variants of scheme 0x8000).
*/
GraphicsCursorFrameIndex
FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction
          (int pointerY,int pointerX,FrontendModelPointerHitContext *context)

{
  uint32_t callbackResult;
  GraphicsCursorFrameIndex cursorFrameIndex;
  uint64_t bestHit;

  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget(pointerY,pointerX,context);
  context->selectedHitMetric = (int)bestHit;
  context->selectedModelNode = reinterpret_cast<ModelRuntimeNode *>(bestHit >> 32);
  if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK)) {
    if (context->heldButtonCursorCallback != nullptr)
    {
      callbackResult = context->heldButtonCursorCallback
                        (context->surfaceHitDepth,context->surfaceHitWorldY,
                         context->surfaceHitWorldX,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
    }
    return 0;
  }
  if (!Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION)) {
    if (context->hoverCursorCallback != nullptr)
    {
      callbackResult = context->hoverCursorCallback
                        (context->surfaceHitDepth,context->surfaceHitWorldY,
                         context->surfaceHitWorldX,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
    }
    return 0;
  }
  if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION)) {
    return 0;
  }
  if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT))
  {
    if (Any(g_CursorButtonState & LEFT)) {
      return 17;
    }
    return 16;
  }
  if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN))
  {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      if (Any(g_CursorButtonState & LEFT)) {
        return 17;
      }
      return 18;
    }
    if (Any(g_CursorButtonState & LEFT)) {
      return 20;
    }
    return 19;
  }
  if (!Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE))
  {
    return 0;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    return 15;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) == 0) {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      return 37;
    }
    if (!Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_HIDE_PANEL)) {
      if (!Any(g_CursorButtonState & LEFT)) {
        return 1;
      }
      if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)) {
        return 14;
      }
      if (!Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)) {
        return 37;
      }
    }
    else {
      if (Any(g_CursorButtonState & LEFT)) {
        if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)) {
          return 15;
        }
        if (!Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)) {
          return 0;
        }
        return 17;
      }
      if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)) {
        return 14;
      }
      if (!Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)) {
        return 37;
      }
    }
    cursorFrameIndex = 16;
  }
  else {
    cursorFrameIndex = 17;
  }
  return cursorFrameIndex;
}

/* Press of a non-right button on the model pointer context (nonRightPress of
   g_FrontendModelPointerContextVtable): remembers the press point (corner of the drag frame), stores the best
   model hit, routes the following pointer moves to heldButtonCursorCallback and reports the press to
   buttonPressCallback.
*/
void FrontendModelPointerContext_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  uint64_t bestHit;
  
  callbackContext->dragFrameStartX = pointerX;
  callbackContext->dragFrameStartY = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,UiNode_As<FrontendModelPointerHitContext>(&callbackContext->base)
                    );
  callbackContext->selectedModelNode = reinterpret_cast<ModelRuntimeNode *>(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  callbackContext->contextFlags =
       callbackContext->contextFlags | FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->buttonPressCallback !=
      nullptr) {
    callbackContext->buttonPressCallback
              (callbackContext->surfaceHitDepth,callbackContext->surfaceHitWorldY,
               callbackContext->surfaceHitWorldX,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               UiNode_As<FrontendModelPointerHitContext>(&callbackContext->base));
  }
}

/* Release of a non-right button on the model pointer context (nonRightRelease of
   g_FrontendModelPointerContextVtable): stores the best model hit, routes pointer moves back to the hover
   callback and reports the release to buttonReleaseCallback.
*/
void FrontendModelPointerContext_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerHitContext *callbackContext)

{
  uint64_t bestHit;
  
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = reinterpret_cast<ModelRuntimeNode *>(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  callbackContext->contextFlags =
       callbackContext->contextFlags & ~FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->buttonReleaseCallback !=
      nullptr) {
    callbackContext->buttonReleaseCallback
              (callbackContext->surfaceHitDepth,callbackContext->surfaceHitWorldY,
               callbackContext->surfaceHitWorldX,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,callbackContext);
  }
}

/* Drag with a non-right button on the model pointer context (nonRightDrag of
   g_FrontendModelPointerContextVtable): remembers the current point (the other corner of the drag frame),
   stores the best model hit and reports the drag to buttonDragCallback.
*/
void FrontendModelPointerContext_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  uint64_t bestHit;
  
  callbackContext->dragFrameEndX = pointerX;
  callbackContext->dragFrameEndY = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,UiNode_As<FrontendModelPointerHitContext>(&callbackContext->base)
                    );
  callbackContext->selectedModelNode = reinterpret_cast<ModelRuntimeNode *>(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  if (callbackContext->buttonDragCallback !=
      nullptr) {
    callbackContext->buttonDragCallback
              (callbackContext->surfaceHitDepth,callbackContext->surfaceHitWorldY,
               callbackContext->surfaceHitWorldX,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               UiNode_As<FrontendModelPointerHitContext>(&callbackContext->base));
  }
}

/* Relocate method of the model pointer context (relocate of g_FrontendModelPointerContextVtable), run when the
   control is built from its template: puts the camera target at the origin, gives camera limits the template
   left at 0 their defaults, starts with no candidate models and no overlay entity, then relocates the children.
*/
void FrontendModelPointerContext_Relocate
               (UiSerializedRelocationDelta relocationDelta,
               FrontendModelPointerContext *control)

{
  control->targetPositionXQ12 = 0;
  control->targetPositionYQ12 = 0;
  control->targetPositionZQ12 = 0;
  /* pitch -90..+90 degrees */
  if (control->minimumPitchAngle == 0) {
    control->minimumPitchAngle = (uint32_t)-FIXED_ANGLE16_QUARTER_TURN;
  }
  if (control->maximumPitchAngle == 0) {
    control->maximumPitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  /* camera distance 0.25..127 (Q12) */
  if (control->minimumDistanceQ12 == 0) {
    control->minimumDistanceQ12 = Q12_ONE / 4;
  }
  if (control->maximumDistanceOrSurfaceLimitQ12 == 0) {
    control->maximumDistanceOrSurfaceLimitQ12 = 127 * Q12_ONE;
  }
  control->worldObjectArray = 0;
  control->worldObjectCount = 0;
  control->candidateModelListHead = nullptr;
  control->selectedOverlayEntity = nullptr;
  UiContainer_RelocateChildren(relocationDelta,&control->base);
}

/* Layout method of the model pointer context (layout of g_FrontendModelPointerContextVtable): a new size
   invalidates the reusable terrain projection (WorldRuntime_ClearFieldGridDirtyFlag clears
   TERRAIN_RENDER_REUSE_PROJECTION), then the children are laid out.
*/
void FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext)

{
  WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  /* the world runtime view starts with the view control's UiNodeBase (WorldRuntimeInteractionState) */
  UiContainer_LayoutChildren(reinterpret_cast<UiNodeBase *>(callbackContext));
}

/* Hierarchy renderer for the model passes of FrontendModelPointerContext_RenderWorldViewQueuesClipped. */
static void (*FrontendModelPointerContext_SelectRenderHierarchyProc(const FrontendModelPointerContext *control))
          (ModelRuntimeNode *)
{
  if (Any(control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY)) {
    return ModelRuntime_RenderHierarchyRecursiveAlternatePath;
  }
  return ModelRuntime_CullAndRenderHierarchyRecursive;
}

/* End of one render pass: sorts and draws the active primitive queue into the clip rectangle and adds its
   primitive count to renderedPrimitiveCount. */
static void FrontendModelPointerContext_DrawActiveQueue
          (FrontendModelPointerContext *control,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft)
{
  uint32_t queuedPrimitiveCount;

  g_GraphicsPrimitiveQueueRadixSortProc
            (ToBits(control->base.nodeFlags & UI_NODE_SUPPRESSED),control->activePrimitiveQueue);
  g_GraphicsDrawPrimitiveQueue
            (clipBottom,clipRight,clipTop,clipLeft,control->activePrimitiveQueue);
  queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
  control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
}

/* Draw method of the model pointer context (drawClipped of g_FrontendModelPointerContextVtable), the 3D view of
   the menu room and of the in-game world: clamps the clip rectangle to the control, sets up camera, projection
   and (optionally) the sound listener, then renders the candidate models in up to four primitive-queue passes
   (models with flag 0x200, the terrain, the shading pass of models with flag 0x100, the remaining models),
   releasing and re-acquiring the render spin lock between passes. Afterwards the enabled selection
   overlays are drawn and the child controls on top. Nothing is drawn while FRONTEND_MENU_ROOM_RENDER_SUPPRESSED
   is set (a dialog page covers the room).
*/
void FrontendModelPointerContext_RenderWorldViewQueuesClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FrontendModelPointerContext *control)

{
  UiPixelCoordinate cursorOverrideX;
  UiPixelCoordinate cursorOverrideY;
  GraphicsWorldCoordinateQ12 listenerY;
  GraphicsWorldCoordinateQ12 listenerZ;
  void (*renderHierarchyProc)(ModelRuntimeNode *);
  ModelRuntimeNode *modelNode;
  GraphicsPrimitiveQueue *frameQueue;
  /* The original would jump straight to the end when a primitive-queue reset failed, handing the selection
     overlays a stale rectangle; GraphicsPrimitiveQueue_ResetGlobal never fails, so the overlays always get the
     clipped rectangle. */

  if (Any(control->contextFlags & FRONTEND_MENU_ROOM_RENDER_SUPPRESSED)) {
    return;
  }
  if (clipLeft < control->base.left) {
    clipLeft = control->base.left;
  }
  if (control->base.right < clipRight) {
    clipRight = control->base.right;
  }
  if (clipTop < control->base.top) {
    clipTop = control->base.top;
  }
  if (control->base.bottom < clipBottom) {
    clipBottom = control->base.bottom;
  }
  g_GraphicsSetViewportAndClearDepth(clipBottom,clipRight,clipTop,clipLeft);
  g_SpinLockAcquire(control->renderSpinLock);
  cursorOverrideY = g_CursorOverrideY;
  cursorOverrideX = g_CursorOverrideX;
  control->selectedModelNode = nullptr;
  control->selectedHitMetric = WORLD_POINTER_NO_HIT;
  control->surfaceHitWorldX = WORLD_POINTER_NO_HIT;
  control->surfaceHitWorldY = WORLD_POINTER_NO_HIT;
  control->surfaceHitDepth = WORLD_POINTER_NO_HIT;
  control->cursorWorldXQ12 = cursorOverrideX << 12;
  control->cursorWorldYQ12 = cursorOverrideY << 12;
  control->renderedPrimitiveCount = 0;
  Graphics_SetProjectionClipRect(clipBottom,clipRight,clipTop,clipLeft);
  Graphics_SetViewProjectionParameters
            (control->projectionShift,control->viewAngle1,control->viewAngle0,
             control->projectionScale,control->hitReferenceWorldZQ12,control->hitReferenceWorldYQ12,
             control->hitReferenceWorldXQ12);
  if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_SOUND_LISTENER)) {
    listenerY = control->targetPositionYQ12;
    listenerZ = ((int)control->committedDistanceOrSoundZOffset >> 2) + control->targetPositionZQ12;
    SpatialSound_RebuildListenerTransformFromPose
              (control->viewAngle1,control->viewAngle0,listenerZ,listenerY,control->targetPositionXQ12);
  }
  Graphics_SetProjectionViewport
            (control->base.bottom,control->base.right,control->base.top,control->base.left);
  Graphics_SetAuxiliaryOrientation
            (control->auxiliaryOrientationAngle1,control->auxiliaryOrientationAngle0);
  Graphics_SetSceneBoundsAndColors
            (control->sceneBound7,control->sceneBound6,control->sceneBound5,control->sceneBound4,
             control->sceneBound3,control->sceneBound2,control->sceneBound1,control->sceneBound0);
  Graphics_RebuildFrustumPlanes();
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  GraphicsShadingRuntime_RebuildCompactLightingRecords();
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
  Graphics_SetActivePrimitiveQueue(frameQueue);
  control->activePrimitiveQueue = frameQueue;
  if (control->renderPhaseCallback != nullptr) {
    control->renderPhaseCallback(GRAPHICS_STATE_DISABLED,FrontendModelPointerContext_AsWorldRuntime(control));
  }
  renderHierarchyProc = FrontendModelPointerContext_SelectRenderHierarchyProc(control);
  for (modelNode = control->candidateModelListHead; modelNode != nullptr;
      modelNode = WorldNode_View<ModelRuntimeNode>(modelNode->common.nextNode.get())) {
    if (!Any(modelNode->runtimeFlags & MODEL_NODE_FLAG_HIDDEN) &&
        (Any(modelNode->runtimeFlags & MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN))) {
      modelNode->runtimeFlags = modelNode->runtimeFlags & ~MODEL_NODE_FLAG_RENDERED;
      if ((modelNode->tintArgb & ARGB8888_ALPHA_MASK) != 0) {
        renderHierarchyProc(modelNode);
      }
    }
  }
  if (control->renderPhaseCallback != nullptr) {
    control->renderPhaseCallback(GRAPHICS_STATE_ENABLED,FrontendModelPointerContext_AsWorldRuntime(control));
  }
  FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  if ((Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN)) && (control->fieldGrid != nullptr)) {
    frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
    Graphics_SetActivePrimitiveQueue(frameQueue);
    control->activePrimitiveQueue = frameQueue;
    TerrainProjectedGrid_TransformShadeAndQueue(control->fieldGrid,control);
    FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  }
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_SHADING_ENABLED)) {
    modelNode = control->candidateModelListHead;
    frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
    Graphics_SetActivePrimitiveQueue(frameQueue);
    control->activePrimitiveQueue = frameQueue;
    if (modelNode != nullptr) {
      GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();
      for (; modelNode != nullptr; modelNode = WorldNode_View<ModelRuntimeNode>(modelNode->common.nextNode.get())) {
        if (!Any(modelNode->runtimeFlags & MODEL_NODE_FLAG_HIDDEN) &&
            (Any(modelNode->runtimeFlags & MODEL_NODE_FLAG_SHADING_PASS)) &&
            ((modelNode->tintArgb & ARGB8888_ALPHA_MASK) != 0)) {
          GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
                    (modelNode,reinterpret_cast<GeneratedTextureRenderContextView *>(control)); /* the shading view of this context's bytes */
        }
      }
      GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();
    }
    FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  }
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
  Graphics_SetActivePrimitiveQueue(frameQueue);
  control->activePrimitiveQueue = frameQueue;
  if (control->renderPhaseCallback != nullptr) {
    control->renderPhaseCallback(GRAPHICS_STATE_DISABLED,FrontendModelPointerContext_AsWorldRuntime(control));
  }
  renderHierarchyProc = FrontendModelPointerContext_SelectRenderHierarchyProc(control);
  for (modelNode = control->candidateModelListHead; modelNode != nullptr;
      modelNode = WorldNode_View<ModelRuntimeNode>(modelNode->common.nextNode.get())) {
    if (!Any(modelNode->runtimeFlags & (MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN | MODEL_NODE_FLAG_HIDDEN))) {
      modelNode->runtimeFlags = modelNode->runtimeFlags & ~MODEL_NODE_FLAG_RENDERED;
      if ((modelNode->tintArgb & ARGB8888_ALPHA_MASK) != 0) {
        renderHierarchyProc(modelNode);
      }
    }
  }
  if (control->renderPhaseCallback != nullptr) {
    control->renderPhaseCallback(GRAPHICS_STATE_ENABLED,FrontendModelPointerContext_AsWorldRuntime(control));
  }
  FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  if ((((clipLeft == control->base.left) && (clipRight == control->base.right)) &&
      (clipTop == control->base.top)) && (clipBottom == control->base.bottom)) {
    /* the whole view was drawn: the next terrain pass may reuse this projection */
    control->contextFlags = control->contextFlags | TERRAIN_RENDER_REUSE_PROJECTION;
  }
  g_GraphicsEndScene();
  /* the 2D draw list: the 3D scene goes here (GPU_RECORD only) */
  Draw2D_MarkExternal3D(clipBottom,clipRight,clipTop,clipLeft);
  g_RenderedFrameCountSinceDebugRefresh++;
  if (!Any(control->base.nodeFlags & UI_NODE_SUPPRESSED)) {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  }
  else {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitHalfSourceRgb;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledHalfSourceRgb;
  }
  if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_OVERLAYS)) {
    if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS)) {
      SelectionOverlay_RenderSelectedArmyMetrics(clipBottom,clipRight,clipTop,clipLeft);
      if ((control->selectedOverlayEntity != nullptr) &&
          SelectionInfo_IsEntryAbsent(control->selectedOverlayEntity)) {
        SelectionOverlay_RenderArmyMetricsForEntity
                  (clipBottom,clipRight,clipTop,clipLeft,control->selectedOverlayEntity);
      }
    }
    if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING)) {
      SelectionOverlay_DrawBoundsFrame
                (clipBottom,clipRight,clipTop,clipLeft,control->dragFrameEndY,
                 control->dragFrameEndX,control->dragFrameStartY,
                 control->dragFrameStartX);
    }
    if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS)) {
      SelectionOverlay_DrawTerrainPointMarkers
                (clipBottom,clipRight,clipTop,clipLeft,control->terrainMarkerPointCount,
                 control->terrainMarkerCoordinatePairs,control->fieldGrid);
    }
    if ((Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER)) && (control->surfaceHitDepth != WORLD_POINTER_NO_HIT)) {
      SelectionOverlay_DrawWorldPointMarker
                (clipBottom,clipRight,clipTop,clipLeft,
                 (uint32_t)((g_UiCommandModeGColorVariantLimit & ARGB8888_ALPHA_MASK) != 0),
                 control->surfaceHitWorldY,control->surfaceHitWorldX,control->fieldGrid);
    }
    if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS)) {
      SelectionOverlay_DrawGridVertexMarkers(clipBottom,clipRight,clipTop,clipLeft,control->fieldGrid);
    }
    if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY)) {
      SelectionOverlay_DrawFluidExclusionMarkers(clipBottom,clipRight,clipTop,clipLeft,control->fieldGrid);
    }
    if (Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS)) {
      SelectionOverlay_DrawResourceCellMarkers
                (clipBottom,clipRight,clipTop,clipLeft,(uint8_t)control->selectedResourceMarkerIndex,
                 control->fieldGrid);
    }
    if (((Any(control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN)) && (control->fieldGrid != nullptr))
       && (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_DRAW_DEBUG_CELL_MARKERS))) {
      SelectionOverlay_DrawDebugMarkedCellMarkers(clipBottom,clipRight,clipTop,clipLeft,control->fieldGrid);
    }
  }
  g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
  g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  if ((control->selectedModelNode != nullptr) &&
     ((int)control->surfaceHitDepth < control->selectedHitMetric)) {
    control->selectedModelNode = nullptr;
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
}

/* Right-button press on the model pointer context (rightPress of g_FrontendModelPointerContextVtable): starts a
   camera drag. Remembers the press point (the pointer is put back there after every drag step), routes pointer
   moves to the camera cursor resolution, restarts the held-tick counter (rightButtonHeldTicks, counted by
   FrontendModelPointerContext_Tick) and pins the drawn cursor.
*/
void FrontendModelPointerContext_RightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  callbackContext->capturedPointerX = pointerX;
  callbackContext->capturedPointerY = pointerY;
  callbackContext->capturedWheelDelta = wheelDelta;
  callbackContext->contextFlags =
       callbackContext->contextFlags |
       FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION;
  callbackContext->rightButtonHeldTicks = 0;
  g_CursorUseOverridePosition++;
}

/* Right-button release on the model pointer context (rightRelease of g_FrontendModelPointerContextVtable): ends
   the camera drag and unpins the cursor. A release within 7 ticks of the press counts as a click and is
   reported to rightClickCallback (the menu room stops its camera flight with it).
*/
void FrontendModelPointerContext_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  g_CursorUseOverridePosition = 0;
  /* clears ROUTE_TO_BUILTIN_ACTION_RESOLUTION and the camera motion bits 0..3 */
  callbackContext->contextFlags = callbackContext->contextFlags &
       ~(FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION | FRONTEND_CAMERA_MOTION_MASK);
  if ((callbackContext->rightButtonHeldTicks < 7) &&
     (callbackContext->rightClickCallback != nullptr)) {
    callbackContext->rightClickCallback(callbackContext);
  }
}

/* Right-button drag on the model pointer context (rightDrag of g_FrontendModelPointerContextVtable): moves the
   camera by the pointer's offset from the press point, then puts the pointer back there, snapshots the camera
   state and calls the view's clearTransientStateCallback. The motion depends on the camera scheme bit of the
   view (0x100, 0x8000 or 0x200), the left button and the modifier keys: move, heading, pitch, distance or a
   combination; 0x10 blocks camera input. FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction
   shows the matching cursor.
*/
void FrontendModelPointerContext_DispatchWorldCameraPointerInput
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  InGameWorldTransientStateClearCallbackProc *clearTransientCallback;
  uint32_t pointerDeltaX;
  AngleTurn32 pointerDeltaY;

  /* The low four runtimeFlags bits record the motion in progress: 1 move, 2 heading, 4 distance, 8 pitch.
     The pointer's X offset drives the heading, its Y offset pitch, distance and moves. */
  pointerDeltaX = pointerX - callbackContext->pointerCaptureX;
  pointerDeltaY = pointerY - callbackContext->pointerCaptureY;
  if (Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO)) {
    return;
  }
  if (Any(callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT)) {
    if (!Any(g_CursorButtonState & LEFT)) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | (FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
      WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
  }
  else if (Any(callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN)) {
    if (!Any(g_CursorButtonState & LEFT)) {
      if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_MOVE;
        WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
                  (pointerDeltaY,pointerDeltaX,callbackContext);
        WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
                  (pointerDeltaY,callbackContext);
      }
      else {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE);
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | (FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
        WorldMotion_AdjustHeadingAndClearFieldGridDirty(pointerDeltaX,callbackContext);
        /* the pointer Y delta, as in the other pitch branches */
        WorldMotion_AdjustPitchClampAndClearFieldGridDirty(pointerDeltaY,callbackContext);
      }
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_MOVE;
      WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn(pointerDeltaY,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustPositionMagnitudeClamp(pointerDeltaY,callbackContext);
    }
  }
  else {
    if (!Any(callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE)) {
      return;
    }
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
      WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_HEADING;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
    }
    else if (Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_HIDE_PANEL) && Any(g_CursorButtonState & LEFT)) {
      /* Flag 0x4000000 with the button held: 0x40000000 selects pitch, 0x80000000 distance. */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~FRONTEND_CAMERA_MOTION_MASK;
      if (Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM)) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
        WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
      else if (Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT)) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
    }
    else if (!Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_HIDE_PANEL) && !Any(g_CursorButtonState & LEFT)) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_MOVE;
      WorldRuntime_TranslateCameraByScreenDelta(pointerDeltaY,pointerDeltaX,callbackContext);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(callbackContext);
    }
    else {
      /* Heading drag (button state opposite to flag 0x4000000); afterwards 0x40000000 adds a distance step and
         0x80000000 a pitch step (flags re-read after the heading call). */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_HEADING;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
      if (Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM)) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
      else if (Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT)) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
        WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
    }
  }
  g_PointerSetPosition(callbackContext->pointerCaptureY,callbackContext->pointerCaptureX);
  clearTransientCallback = callbackContext->fieldRegion.clearTransientStateCallback;
  WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
  if (clearTransientCallback != nullptr) {
    clearTransientCallback(callbackContext);
  }
}

/* Wheel handler of the model pointer context (pointerWheel of g_FrontendModelPointerContextVtable): unless
   camera input is blocked (0x10) or the view has no camera scheme (0x100/0x200/0x8000), the scaled wheel
   delta changes the camera distance, with Ctrl the pitch, and the camera state is snapshotted.
*/
void FrontendModelPointerContext_PointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  int scaledWheelDelta;
  
  if ((!Any(callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO)) &&
     (Any(callbackContext->runtimeFlags &
      (FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN | FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE |
       FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT)))) {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
      scaledWheelDelta = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(scaledWheelDelta,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
    else {
      scaledWheelDelta = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
      WorldMotion_AdjustPitchClampAndRecomputePosition(scaledWheelDelta,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
  }
}

/* Keyboard handler of the model pointer context (keyboardEvent of g_FrontendModelPointerContextVtable): offers
   the key to the view's keyboardFallback first; when there is none or it returns true, the default handling
   (UiNode_DefaultKeyboardEventMoveFocusNext) decides and its result is returned.
*/
Bool8 FrontendModelPointerContext_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          FrontendModelPointerHitContext *control)

{
  if (control->keyboardFallback != nullptr &&
      !control->keyboardFallback(keyboardStateMask,keyCode,UiNode_As<UiRootNode>(&control->base))) {
    return false;
  }
  return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
}

/* Tick method of the model pointer context (tick of g_FrontendModelPointerContextVtable): counts the ticks the
   right button is held (rightButtonHeldTicks, read by FrontendModelPointerContext_RightRelease) and, in a view
   without camera scheme 0x100/0x8000, camera input block (0x10) and WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA, eases
   the camera distance by one convergence step per tick until it is within 15/16..17/16 of the clamped committed
   distance, placing the camera behind the target.
*/
void FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext)

{
  uint32_t *callbackStateCounter;
  UQ12 targetDistance;
  UQ12 convergenceStep;
  UQ12 clampedCommittedDistance;
  FixedDirection cameraOffset;
  
  /* 0x40: right button held (ROUTE_TO_BUILTIN_ACTION_RESOLUTION); the counter is rightButtonHoldTicks of the
     pointer-context view of this record */
  if (Any(callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION)) {
    callbackStateCounter = &callbackContext->selection.rightButtonHoldTicks;
    *callbackStateCounter = *callbackStateCounter + 1;
  }
  if (!Any(callbackContext->runtimeFlags &
       (WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA | FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN |
        FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT | WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO))) {
    clampedCommittedDistance = callbackContext->motion.committedDistanceQ12;
    if ((int)clampedCommittedDistance < (int)callbackContext->minimumCameraDistanceQ12) {
      clampedCommittedDistance = callbackContext->minimumCameraDistanceQ12;
    }
    if ((int)callbackContext->maximumCameraDistanceQ12 < (int)clampedCommittedDistance) {
      clampedCommittedDistance = callbackContext->maximumCameraDistanceQ12;
    }
    targetDistance = callbackContext->motion.targetDistanceQ12;
    convergenceStep = g_WorldMotionTargetDistanceConvergenceStepQ12;
    if ((int)(clampedCommittedDistance * 15) >> 4 <= (int)targetDistance) {
      if ((int)targetDistance <= (int)(clampedCommittedDistance * 17) >> 4) {
        return;
      }
      convergenceStep = -g_WorldMotionTargetDistanceConvergenceStepQ12;
    }
    callbackContext->motion.targetDistanceQ12 = targetDistance + convergenceStep;
    /* heading ^ 0x8000 turns half round: the camera sits behind the target */
    cameraOffset = FixedMath_DirectionFromAnglesScaled
                      (-callbackContext->motion.pitchAngle,
                       callbackContext->motion.headingAngle ^ FIXED_ANGLE16_HALF_TURN,targetDistance + convergenceStep);
    callbackContext->motion.positionXQ12 =
         cameraOffset.x + callbackContext->motion.targetPositionXQ12;
    callbackContext->motion.positionYQ12 =
         cameraOffset.y + callbackContext->motion.targetPositionYQ12;
    callbackContext->motion.positionZQ12 =
         cameraOffset.z + callbackContext->motion.targetPositionZQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  }
}

/* Hover handler of the menu room's pointer context (hoverCursorCallback/108). On the main page (not on a
   network client) an object of the room that has a usable ROM action record starts a camera flight towards
   the record's keyframe and makes the pointer cursor frame 7; the record's hint text (text id 0x2000 + hint)
   is shown in the hint box, hint 1 while a page action is still being processed, none otherwise.
   Actions 3, 4, 9 and negative ones are not offered in a network session, the network page (2) not without
   a network backend (the same rule as FrontendRomActionTable_ExecuteRecord). Returns the cursor frame index.
*/
uint32_t FrontendRuntime_UpdatePointerContextAndSceneView
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          void *pointedModelNode,FrontendPointerSceneRuntimeView *frontendRuntime)

{
  uint16_t *previousCommandStream;
  UiNodeBase *control;
  RomAssetRecordPrefix *pointedRomRecord;
  int *transitionRecord; /* keyframe channels 0..5, [6] hint text, [8] FRONTEND_PAGE_ACTION_* */
  uint32_t resultCode;
  uint16_t *commandStream;
  RomRecordId recordId;
  int hintValue;
  int keyframeChannel3;
  int keyframeChannel4;
  int keyframeChannel5;
  int halfWidth;
  int halfHeight;
  int frameWidth;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize windowTextureSize;

  resultCode = 0;
  hintValue = 0;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
     (UiPageStack_ActivePageIndex(&frontendRuntime->activePageStack) == 0)) {
    pointedRomRecord = RomRegistry_FindRecordBySlotValue(reinterpret_cast<RomRegistrySlotValue>(pointedModelNode));
    recordId = FRONTEND_ROM_RECORD_ID_NONE;
    if (pointedRomRecord != nullptr) {
      recordId = pointedRomRecord->recordId;
    }
    transitionRecord = static_cast<int *>(RomRecordTable_FindRecordById(recordId,g_FrontendActiveRomRecord));
    /* Skip records without a transition, network-only pages (3/4/9/negative) in a networked session and the
       network page (2) when no backend exists. */
    if ((transitionRecord != nullptr) &&
       ((((static_cast<FrontendPageAction>(transitionRecord[8]) != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE &&
          (static_cast<FrontendPageAction>(transitionRecord[8]) != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE)) &&
         (static_cast<FrontendPageAction>(transitionRecord[8]) != FRONTEND_PAGE_ACTION_CREDITS)) &&
         (transitionRecord[8] >= 0)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL)) &&
       ((static_cast<FrontendPageAction>(transitionRecord[8]) != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) || (g_NetworkBackendInstanceCount != 0))) {
      keyframeChannel3 = transitionRecord[3];
      keyframeChannel4 = transitionRecord[4];
      keyframeChannel5 = transitionRecord[5];
      /* Rebuild the camera spline only when the target keyframe changed. (When channels 0-2 already match
         the original skips storing them; storing the equal values here is equivalent.) */
      if ((((*transitionRecord != g_FrontendRomTransitionKeyframes[1].channel0Q12) ||
           (transitionRecord[1] != g_FrontendRomTransitionKeyframes[1].channel1Q12)) ||
          (transitionRecord[2] != g_FrontendRomTransitionKeyframes[1].channel2Q12)) ||
         (((keyframeChannel3 != g_FrontendRomTransitionKeyframes[1].channel3Q12) ||
          (keyframeChannel4 != g_FrontendRomTransitionKeyframes[1].channel4Q12)) ||
          (keyframeChannel5 != g_FrontendRomTransitionKeyframes[1].channel5Q12))) {
        /* fly from the current camera (keyframe 0) to the record's camera (keyframe 1) in 192 ticks of
           FrontendRomTransition_AdvanceElapsedTicks; pending -1 = no record to activate at the end */
        g_FrontendRomTransitionKeyframes[1].channel0Q12 = *transitionRecord;
        g_FrontendRomTransitionKeyframes[1].channel1Q12 = transitionRecord[1];
        g_FrontendRomTransitionKeyframes[1].channel2Q12 = transitionRecord[2];
        g_FrontendRomTransitionKeyframes[0].channel0Q12 = frontendRuntime->hitReferenceWorldXQ12;
        g_FrontendRomTransitionKeyframes[0].channel1Q12 = frontendRuntime->hitReferenceWorldYQ12;
        g_FrontendRomTransitionKeyframes[0].channel2Q12 = frontendRuntime->hitReferenceWorldZQ12;
        g_FrontendRomTransitionKeyframes[0].channel3Q12 = frontendRuntime->projectionScale;
        g_FrontendRomTransitionKeyframes[0].channel4Q12 = frontendRuntime->viewAngle0;
        g_FrontendRomTransitionKeyframes[0].channel5Q12 = frontendRuntime->viewAngle1;
        g_FrontendRomTransitionKeyframes[0].timeQ12 = 0;
        g_FrontendRomTransitionKeyframes[1].timeQ12 = 192;
        g_FrontendRomTransitionElapsedTicks = 0;
        g_FrontendRomTransitionTargetRecordId = FRONTEND_ROM_TRANSITION_NO_TARGET;
        g_FrontendRomTransitionSplineKeyframeCount = 2;
        g_FrontendRomTransitionSplineKeyframes = g_FrontendRomTransitionKeyframes;
        g_FrontendRomTransitionKeyframes[1].channel3Q12 = keyframeChannel3;
        g_FrontendRomTransitionKeyframes[1].channel4Q12 = keyframeChannel4;
        g_FrontendRomTransitionKeyframes[1].channel5Q12 = keyframeChannel5;
        WorldMotionSpline_BuildSixChannelCurves(2,g_FrontendRomTransitionKeyframes);
      }
      hintValue = transitionRecord[6];
      resultCode = 7;
    }
  }
  if (hintValue == 0) {
    if (g_FrontendPendingPageActionDepth == 0) {
      frontendRuntime->hintBox.hintActive = 0;
      frontendRuntime->hintBox.commandStream = nullptr;
      return resultCode;
    }
    hintValue = 1;
  }
  /* only when the text changed: size the hint box around it plus the window frame (texture frame UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT) */
  previousCommandStream = frontendRuntime->hintBox.commandStream;
  commandStream = TextResource_Resolve(hintValue + TEXT_ID_MENU_HINT_BASE);
  if (commandStream != previousCommandStream) {
    frontendRuntime->hintBox.commandStream = commandStream;
    textExtent = RichTextCommandStream_MeasureLine(g_UiTextStyleNormal,commandStream);
    halfWidth = (int)(textExtent.widthPixels + 1) >> 1;
    halfHeight = (int)(textExtent.heightPixels + 1) >> 1;
    frontendRuntime->hintBox.base.leftOffset = halfWidth;
    frontendRuntime->hintBox.base.topOffset = halfHeight;
    frontendRuntime->hintBox.base.right = -halfWidth;
    frontendRuntime->hintBox.base.bottom = -halfHeight;
    windowTextureSize = g_GraphicsTextureSourceGetLogicalSize(UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT,g_UiWindowTextureSource);
    frameWidth = windowTextureSize.logicalWidthPixels + 3;
    control = frontendRuntime->hintBox.base.nextSibling;
    frontendRuntime->hintBox.hintActive = 1;
    frontendRuntime->hintBox.base.leftOffset = frontendRuntime->hintBox.base.leftOffset + frameWidth;
    frontendRuntime->hintBox.base.topOffset =
         frontendRuntime->hintBox.base.topOffset + windowTextureSize.logicalHeightPixels;
    frontendRuntime->hintBox.base.right = frontendRuntime->hintBox.base.right - frameWidth;
    frontendRuntime->hintBox.base.bottom =
         frontendRuntime->hintBox.base.bottom - windowTextureSize.logicalHeightPixels;
    control->vtable->layout(control);
  }
  return resultCode;
}

/* Button-press handler of the menu room's pointer context (buttonPressCallback): the frontend does nothing
   on press, it acts on release (FrontendMenuRoom_ExecuteClickedRomAction).
*/
void FrontendMenuRoom_PressNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext)

{
}

/* Drag handler of the menu room's pointer context (buttonDragCallback); dragging does nothing in the
   frontend.
*/
void FrontendMenuRoom_DragNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext)

{
}

/* Button-release handler of the menu room's pointer context (buttonReleaseCallback): clicking an object of
   the menu room runs the ROM action record that belongs to it (FrontendRomActionTable_ExecuteRecord). In a
   network game the host sends it as a frontend command so every player follows; clients ignore clicks.
*/
void FrontendMenuRoom_ExecuteClickedRomAction
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          FrontendCallbackArgument5 pointedModelNode,uint32_t pointerContext)

{
  RomAssetRecordPrefix *slotRecord;
  RomRecordTableIndex recordIndex;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    slotRecord = RomRegistry_FindRecordBySlotValue(pointedModelNode);
    if (slotRecord != nullptr) {
      recordIndex = RomRecordTable_FindIndexById(slotRecord->recordId,g_FrontendActiveRomRecord);
      if (-1 < (int)recordIndex) {
        FrontendCommand_Issue<FrontendRomActionTable_ExecuteRecord>(0,0,recordIndex);
      }
    }
  }
}

/* Right-button release handler of the menu room's pointer context (rightClickCallback): stops the running
   camera flight (ScenarioCatalog_RequestRomTransitionStopCallback), in a network game as a frontend command
   sent by the host; clients ignore it.
*/
void FrontendMenuRoom_StopCameraFlight(uint32_t pointerContext)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendCommand_Issue<ScenarioCatalog_RequestRomTransitionStopCallback>(0,0,0);
  }
}

/* FrontendModelPointerContext_FindBestEligibleModelHitTarget: the hit priority of model runtime class
   modelClassId. The original indexed the 24-entry table unchecked; bounded here because the class id comes from
   the model's runtime definition: an id outside the table counts as RUNTIME_MODEL_CLASS_PRIORITY_LOW (logged once). */
static RuntimeModelClassPriority FrontendModelPointerContext_ModelClassPriority(ModelRuntimeClassId modelClassId)
{
  static Bool8 s_loggedClassIdOutOfRange;

  if ((uint32_t)modelClassId < sizeof(g_RuntimeModelClassPriorityByModelClassId) /
                                 sizeof(g_RuntimeModelClassPriorityByModelClassId[0])) {
    return g_RuntimeModelClassPriorityByModelClassId[modelClassId];
  }
  if (!s_loggedClassIdOutOfRange) {
    s_loggedClassIdOutOfRange = true;
    Thandor_Log("FrontendModelPointerContext: model class id %d outside the priority table, taken as low",modelClassId);
  }
  return RUNTIME_MODEL_CLASS_PRIORITY_LOW;
}

/* Finds the model under the pointer for the model pointer context's press, release, drag and move handlers:
   hit-tests every candidate model node with flag 2 that is a runtime model (and has flag 0x20 unless the
   context allows models without it). The winner is the nearest hit, or, unless the context compares by metric
   only, the hit whose model class has the highest priority, the nearer one on equal priority. Returns the
   node in the high 32 bits and its hit metric in the low 32 bits (NULL and WORLD_POINTER_NO_HIT without a hit).
*/
uint64_t FrontendModelPointerContext_FindBestEligibleModelHitTarget
                (int pointerY,int pointerX,FrontendModelPointerHitContext *context)

{
  ModelRuntimeNode *modelNode;
  uint32_t bestHitMetric;
  ModelRuntimeNode *bestModelNode;
  uint32_t hitDistanceQ12;
  RuntimeModelClassPriority candidatePriority;
  RuntimeModelClassPriority bestPriority;

  bestModelNode = nullptr;
  bestHitMetric = WORLD_POINTER_NO_HIT;
  for (modelNode = context->candidateModelListHead; modelNode != nullptr;
      modelNode = WorldNode_View<ModelRuntimeNode>(modelNode->common.nextNode.get())) {
    if (Any(modelNode->runtimeFlags & MODEL_NODE_FLAG_RENDERED) && modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL &&
        (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_NON_FACTION_MODELS) ||
         Any(modelNode->runtimeFlags & MODEL_NODE_FLAG_FACTION_OWNED))) {
      if (!ModelRuntimeNode_HitTestProjectedBoundsAndChildren
                        (pointerY,pointerX,modelNode,context,&hitDistanceQ12)) continue;
      if (Any(context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY)) {
        if ((int)bestHitMetric <= (int)hitDistanceQ12) continue;
      }
      else if (bestModelNode != nullptr) {
        /* Higher model-class priority wins; equal priority falls back to the smaller hit metric. */
        candidatePriority = FrontendModelPointerContext_ModelClassPriority
                                 (modelNode->runtimePayload.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId);
        bestPriority = FrontendModelPointerContext_ModelClassPriority
                            (bestModelNode->runtimePayload.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId);
        if (candidatePriority < bestPriority) continue;
        if ((candidatePriority == bestPriority) && ((int)bestHitMetric <= (int)hitDistanceQ12))
        continue;
      }
      bestHitMetric = hitDistanceQ12;
      bestModelNode = modelNode;
    }
  }
  return (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(bestModelNode)) << 32) | (uint64_t)bestHitMetric;
}

/* unaligned in the original; one NOP byte after it dropped */
UiNodeVtable g_FrontendModelPointerContextVtable = {
        .relocate = UI_SLOT(FrontendModelPointerContext_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(FrontendModelPointerContext_RenderWorldViewQueuesClipped),
        .layout = UI_SLOT(FrontendModelPointerContext_Layout),
        .nonRightPress = UI_SLOT(FrontendModelPointerContext_NonRightPress),
        .nonRightRelease = UI_SLOT(FrontendModelPointerContext_NonRightRelease),
        .rightPress = UI_SLOT(FrontendModelPointerContext_RightPress),
        .rightRelease = UI_SLOT(FrontendModelPointerContext_RightRelease),
        .nonRightDrag = UI_SLOT(FrontendModelPointerContext_NonRightDrag),
        .rightDrag = UI_SLOT(FrontendModelPointerContext_DispatchWorldCameraPointerInput),
        .pointerMove = UI_SLOT(FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(FrontendModelPointerContext_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(FrontendModelPointerContext_Tick),
        .pointerWheel = UI_SLOT(FrontendModelPointerContext_PointerWheel)};
