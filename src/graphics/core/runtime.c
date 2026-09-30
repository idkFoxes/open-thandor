/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/core/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>

/* Implementation ownership: graphics/core/runtime. */

/* Address: 0x00576C30.
   Periodic cursor timer callback: keeps the software mouse cursor animated and in place independently of the
   game's frame rate. Every second tick it steps the idle and active animation subresources of the current cursor
   frame (wrapping to the first one); when the frame changed or mouse events moved the cursor, and the graphics
   backend is not in use, the cursor is redrawn directly on the primary surface.
*/
void GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void)

{
  GraphicsCursorFrameRecord *frameRecords;
  GraphicsCursorFrameIndex activeFrameIndex;
  int32_t previousAccessState;
  GraphicsSubresourceIndex nextIdleSubresource;
  GraphicsSubresourceIndex nextActiveSubresource;
  bool frameAdvanced;

  frameAdvanced = false;
  activeFrameIndex = g_CursorFrameIndex;
  frameRecords = g_CursorFrameRecords;
  if (g_CursorVisibilityToken < 0) {
    return;
  }
  g_CursorInputClockValue++;
  g_GraphicsCursorAnimationCountdown--;
  if (g_GraphicsCursorAnimationCountdown == 0) {
    g_GraphicsCursorAnimationCountdown = 2;
    nextIdleSubresource = g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex + 1;
    nextActiveSubresource = g_CursorFrameRecords[g_CursorFrameIndex].activeSubresourceIndex + 1;
    if (g_CursorFrameRecords[g_CursorFrameIndex].idleAnimationLastSubresourceIndex < nextIdleSubresource) {
      nextIdleSubresource = g_CursorFrameRecords[g_CursorFrameIndex].idleAnimationFirstSubresourceIndex;
    }
    if (g_CursorFrameRecords[g_CursorFrameIndex].activeAnimationLastSubresourceIndex < nextActiveSubresource) {
      nextActiveSubresource = g_CursorFrameRecords[g_CursorFrameIndex].activeAnimationFirstSubresourceIndex;
    }
    if (nextIdleSubresource != g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex) {
      g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex = nextIdleSubresource;
      frameRecords[activeFrameIndex].activeSubresourceIndex = nextActiveSubresource;
      frameAdvanced = true;
    }
  }
  /* Recompose the cursor when its animation frame changed or the mouse moved. */
  if ((!frameAdvanced) && (g_MouseEventsProcessed == 0)) {
    return;
  }
#ifdef THANDOR_TEST_AIDS
  /* Windowed test aid (not in the original): the primary surface is the whole desktop, so drawing the cursor
     at framebuffer coordinates would paint over the desktop's top left corner. GraphicsFramebuffer_Present
     composes the cursor into the back surface every frame, so the timer refresh is skipped. */
  if (Thandor_TestAidWindowed()) {
    return;
  }
#endif
  /* try-lock: the original swaps 1 into the access state (XCHG) and only draws when it was 0 */
  if (g_CursorSourceAsset != NULL) {
    previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
    if (previousAccessState == 0) {
      g_MouseEventsProcessed = 0;
      GraphicsCursor_RestoreAfterPresent(g_PrimarySurface3);
      GraphicsCursor_ComposeBeforePresent(g_PrimarySurface3);
      g_GraphicsBackendAccessState--;
    }
  }
  return;
}


/* Address: 0x004168B0.
   Selects the software cursor frame (GRAPHICS_CURSOR_FRAME_*) that the cursor timer animates and draws.
   An index at or above g_CursorFrameCount is rejected with CF set. EAX holds FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE
   on both paths. Installed in g_GraphicsCursorSetFrame (image slot 0x00416848).
*/
CursorFrameResult GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex)

{
  CursorFrameResult successResult;
  CursorFrameResult failureResult;

  if (frameIndex < g_CursorFrameCount) {
    g_CursorFrameIndex = frameIndex;
    successResult.errorCode = FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE;
    successResult.failed = false;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.errorCode = FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE;
  return failureResult;
}


/* Address: 0x004168E0.
   Takes the next mouse event from the 256-entry ring the mouse input code fills (CF set when it is empty)
   and publishes it: button state, cursor position and wheel delta go to the g_Cursor* globals, a release stores
   its clock per button, a press its position as the last click. Installed in g_GraphicsCursorConsumeEvent
   (image slot 0x00416850). A press less than 16 clock ticks after the release of the same button and within
   +-4 pixels of the last click also sets bit 31 (double click) in the returned button state (EBX;
   g_CursorButtonState keeps the raw value), which UiPointer_DispatchPendingEvents passes on to the press
   dispatchers as UI_POINTER_BUTTON_REPEAT_CLICK.
   Original register convention: the event is returned in registers (see the result type), CF flag.
*/
CursorEventResult GraphicsCursor_ConsumeNextInputEvent(void)

{
  GraphicsCursorEventType consumedEventType;
  uint32_t nextReadIndex;
  CursorEventResult eventResult;
  CursorEventResult emptyResult;
  GraphicsCursorClockValue eventClock;
  uint32_t eventIndex;
  uint32_t rawButtonState;
  uint32_t ticksSinceRelease;
  UiPixelCoordinate clickDeltaX;
  UiPixelCoordinate clickDeltaY;

  eventIndex = g_CursorInputReadIndex;
  nextReadIndex = g_CursorInputReadIndex + 1;
  if (g_CursorInputReadIndex == g_CursorInputWriteIndex) {
    memset(&emptyResult, 0, sizeof emptyResult);
    emptyResult.queueEmpty = true;
    return emptyResult;
  }
  if (GRAPHICS_CURSOR_INPUT_EVENT_CAPACITY - 1 < nextReadIndex) { /* wrap around the ring */
    nextReadIndex = 0;
  }
  g_CursorInputReadIndex = nextReadIndex;
  consumedEventType = g_CursorInputEvents[eventIndex].eventType;
  eventClock = g_CursorInputEvents[eventIndex].clockValue;
  rawButtonState = g_CursorInputEvents[eventIndex].buttonState;
  g_CursorButtonState = rawButtonState;
  /* ESI: ticks since the release of the pressed button; releases (and motion) leave the limit itself, which
     never counts as a double click. The compare is unsigned (JNC at 0x00416978). */
  ticksSinceRelease = GRAPHICS_CURSOR_DOUBLE_CLICK_TICKS;
  if (consumedEventType == LEFT_PRESS) {
    ticksSinceRelease = eventClock - g_CursorButtonReleaseClock[0];
  }
  else if (consumedEventType == MIDDLE_PRESS) {
    ticksSinceRelease = eventClock - g_CursorButtonReleaseClock[1];
  }
  else if (consumedEventType == RIGHT_PRESS) {
    ticksSinceRelease = eventClock - g_CursorButtonReleaseClock[2];
  }
  else if (consumedEventType == LEFT_RELEASE) {
    g_CursorButtonReleaseClock[0] = eventClock;
  }
  else if (consumedEventType == MIDDLE_RELEASE) {
    g_CursorButtonReleaseClock[1] = eventClock;
  }
  else if (consumedEventType == RIGHT_RELEASE) {
    g_CursorButtonReleaseClock[2] = eventClock;
  }
  /* The returned state (EBX) gets bit 31 for a double click; g_CursorButtonState above stays raw. The distance
     is measured to the previous press, before this press becomes the last click below. */
  rawButtonState = rawButtonState & ~GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK;
  if (ticksSinceRelease < GRAPHICS_CURSOR_DOUBLE_CLICK_TICKS) {
    clickDeltaX = g_CursorInputEvents[eventIndex].pointerX - g_CursorLastClickX;
    clickDeltaY = g_CursorInputEvents[eventIndex].pointerY - g_CursorLastClickY;
    if ((-GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE <= clickDeltaX) &&
        (clickDeltaX <= GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE) &&
        (-GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE <= clickDeltaY) &&
        (clickDeltaY <= GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE)) {
      rawButtonState = rawButtonState | GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK;
    }
  }
  g_CursorOverrideX = g_CursorInputEvents[eventIndex].pointerX;
  g_CursorOverrideY = g_CursorInputEvents[eventIndex].pointerY;
  g_CursorWheelDelta = g_CursorInputEvents[eventIndex].wheelDelta;
  if ((consumedEventType != MOTION_OR_WHEEL) && (consumedEventType < 4)) { /* a press: LEFT/MIDDLE/RIGHT_PRESS */
    g_CursorLastClickX = g_CursorInputEvents[eventIndex].pointerX;
    g_CursorLastClickY = g_CursorInputEvents[eventIndex].pointerY;
  }
  /* Called through GraphicsCursorConsumeEventProc: the event also leaves the button state in EBX,
     position in ECX/EDX and wheel delta in ESI, which Ghidra's EAX/CF view of this function dropped. */
  eventResult.eventType = consumedEventType;
  eventResult.buttonState = (GraphicsCursorButtonState)rawButtonState;
  eventResult.pointerX = g_CursorInputEvents[eventIndex].pointerX;
  eventResult.pointerY = g_CursorInputEvents[eventIndex].pointerY;
  eventResult.wheelDelta = g_CursorInputEvents[eventIndex].wheelDelta;
  eventResult.queueEmpty = false;
  return eventResult;
}


/* Address: 0x00486430.
   Perspective-projects one view-space Q12 point to screen coordinates (EAX = x, EDX = y): the perspective scale
   is the 64-bit projection numerator divided by z, x and y are scaled by it and offset by the projection
   centre. Points with z not above the numerator's high dword (behind or too close to the eye, where the
   32-bit IDIV would overflow) project to (0,0).
*/
GraphicsProjectedPointPair Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint)

{
  int perspectiveScaleQ12;
  GraphicsProjectedPointPair projectedPoint;
  GraphicsProjectedPointPair offscreenPoint;
  int64_t projectedXProduct;
  int64_t projectedYProduct;
  
  if (g_ProjectionNumerator.high < viewPoint->z) {
    perspectiveScaleQ12 =
         (int)(THANDOR_BITCAST(GraphicsWideFixed, int64_t, g_ProjectionNumerator) / (int64_t)viewPoint->z);
    projectedXProduct = (int64_t)viewPoint->x * (int64_t)perspectiveScaleQ12;
    projectedYProduct = (int64_t)viewPoint->y * (int64_t)perspectiveScaleQ12;
    /* SHLD EDX,EAX,20: bits 12..43 of the 64-bit product, i.e. the Q12 product shifted back by 12 */
    projectedPoint.projectedY =
         (FIXED_PRODUCT_SHR(projectedYProduct, 12)) +
         g_ProjectionCenterFixed.component1;
    projectedPoint.projectedX =
         g_ProjectionCenterFixed.component0 +
         (FIXED_PRODUCT_SHR(projectedXProduct, 12));
    return projectedPoint;
  }
  offscreenPoint.projectedX = 0;
  offscreenPoint.projectedY = 0;
  return offscreenPoint;
}


/* Address: 0x00486490.
   Sets the screen rectangle projected geometry is clipped against, converted from pixels to Q12 (20.12 fixed
   point). First step of a scene setup, before the view parameters and the viewport (called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped and GraphicsOffscreen_RenderModelListToTextureSource).
*/
void Graphics_SetProjectionClipRect
          (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,GraphicsScreenCoordinate minY
          ,GraphicsScreenCoordinate minX)

{
  g_ProjectionClipRect.minX = minX << Q12_SHIFT;
  g_ProjectionClipRect.minY = minY << Q12_SHIFT;
  g_ProjectionClipRect.maxX = maxX << Q12_SHIFT;
  g_ProjectionClipRect.maxY = maxY << Q12_SHIFT;
}


/* Address: 0x004864D0.
   Sets up the camera for the next scene: stores the eye position (Q12 world coordinates), projection scale and
   view angles, builds the view rotation, the camera matrix (identity rotation, translation to the eye) and
   their composition g_ViewProjectionMatrixFixed, and stores the sin/cos pairs of the view azimuth plus and minus
   the half view angle atan2(1 << (12 - projectionShift), projectionScale).
   The azimuth/elevation names follow FixedMath_DirectionFromAnglesScaledRegs, which
   Graphics_RebuildFrustumPlanes feeds with the same two angles.
*/
void Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewElevationAngle,
          GraphicsViewAngle16 viewAzimuthAngle,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX)

{
  uint32_t halfViewAngle16;

  g_ViewOriginFixed.x = originX;
  g_ViewOriginFixed.y = originY;
  g_ViewOriginFixed.z = originZ;
  g_ProjectionScaleFixed = projectionScale;
  g_ViewAngle0 = viewAzimuthAngle;
  g_ViewAngle1 = viewElevationAngle;
  FixedTransform_BuildRotationBasis
            (&g_ViewRotationMatrixFixed,FIXED_ANGLE16_QUARTER_TURN - viewAzimuthAngle & FIXED_ANGLE16_MASK,viewElevationAngle,
             FIXED_ANGLE16_THREE_QUARTER_TURN);
  g_ViewRotationMatrixFixed.translation.x = 0;
  g_ViewRotationMatrixFixed.translation.y = 0;
  g_ViewRotationMatrixFixed.translation.z = 0;
  g_CameraTransformMatrixFixed.translation.x = -originX;
  g_CameraTransformMatrixFixed.translation.y = -originY;
  g_CameraTransformMatrixFixed.translation.z = -originZ;
  g_CameraTransformMatrixFixed.basisRow0[0] = Q28_ONE;
  g_CameraTransformMatrixFixed.basisRow0[1] = 0;
  g_CameraTransformMatrixFixed.basisRow0[2] = 0;
  g_CameraTransformMatrixFixed.basisRow1[0] = 0;
  g_CameraTransformMatrixFixed.basisRow1[1] = Q28_ONE;
  g_CameraTransformMatrixFixed.basisRow1[2] = 0;
  g_CameraTransformMatrixFixed.basisRow2[0] = 0;
  g_CameraTransformMatrixFixed.basisRow2[1] = 0;
  g_CameraTransformMatrixFixed.basisRow2[2] = Q28_ONE;
  FixedTransform_Compose
            (&g_ViewProjectionMatrixFixed,&g_CameraTransformMatrixFixed,&g_ViewRotationMatrixFixed);
  g_ProjectionShift = projectionShift;
  halfViewAngle16 =
       FixedMath_Atan2Angle16(1 << (12U - (char)projectionShift & SHIFT_COUNT_MASK),projectionScale);
  g_ProjectionAngleFactors[0] =
       THANDOR_BITCAST(FixedSinCosEdxEax8, GraphicsWideFixed,
                       FixedMath_SinCosQ28(halfViewAngle16 + viewAzimuthAngle & FIXED_ANGLE16_MASK));
  g_ProjectionAngleFactors[1] =
       THANDOR_BITCAST(FixedSinCosEdxEax8, GraphicsWideFixed,
                       FixedMath_SinCosQ28(viewAzimuthAngle - halfViewAngle16 & FIXED_ANGLE16_MASK));
}


/* Address: 0x00486640.
   Maps the view onto a screen rectangle in pixels: the projection centre
   is the rectangle's midpoint in Q12, and the perspective numerator that Graphics_ProjectViewPoint divides by z
   is width * projection scale, shifted by g_ProjectionShift - 1 and widened to a signed 64-bit value << 12.
   Must follow Graphics_SetViewProjectionParameters, whose scale and shift it reads.
*/
void Graphics_SetProjectionViewport(GraphicsScreenCoordinate bottom,GraphicsScreenCoordinate right,
          GraphicsScreenCoordinate top,GraphicsScreenCoordinate left)

{
  int projectionShiftDelta;
  int64_t projectionScaleProduct;
  uint8_t rightShiftAmount;

  /* (a + b) * 0x800 = the midpoint (a + b) / 2 in Q12 */
  g_ProjectionCenterFixed.component0 = (left + right) * (Q12_ONE / 2);
  g_ProjectionCenterFixed.component1 = (top + bottom) * (Q12_ONE / 2);
  projectionShiftDelta = g_ProjectionShift - 1;
  projectionScaleProduct = (int64_t)(right - left) * (int64_t)(int)g_ProjectionScaleFixed;
  g_ProjectionScaleProduct = (uint32_t)projectionScaleProduct;
  if (projectionShiftDelta != 0) {
    if (projectionShiftDelta < 0) {
      rightShiftAmount = -(uint8_t)projectionShiftDelta & SHIFT_COUNT_MASK;
      g_ProjectionScaleProduct =
           g_ProjectionScaleProduct >> rightShiftAmount |
           (int)((uint64_t)projectionScaleProduct >> 32) << (32 - rightShiftAmount);
    }
    else {
      g_ProjectionScaleProduct = g_ProjectionScaleProduct << ((uint8_t)projectionShiftDelta & SHIFT_COUNT_MASK);
    }
  }
  /* 64-bit numerator = sign-extended product << 12 */
  g_ProjectionNumerator.low = g_ProjectionScaleProduct << Q12_SHIFT;
  g_ProjectionNumerator.high = (int)g_ProjectionScaleProduct >> (32 - Q12_SHIFT);
}


/* Address: 0x004866C0.
   Sets the scene's second direction (elevation/azimuth): stores the angles, their unit direction
   g_AuxiliaryForwardDirectionFixed and a rotation built like the view rotation. The model renderer transforms the
   direction into each model's space and passes it to ModelRender_ComputeVertexIntensity* as the light direction;
   the rotation is used by the generated-texture shading code.
*/
void Graphics_SetAuxiliaryOrientation(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  g_AuxiliaryOrientation.component0 = azimuthAngle;
  g_AuxiliaryOrientation.component1 = elevationAngle;
  FixedMath_WriteDirectionQ28(&g_AuxiliaryForwardDirectionFixed,elevationAngle,azimuthAngle);
  FixedTransform_BuildRotationBasis
            (&g_AuxiliaryRotationMatrixFixed,FIXED_ANGLE16_QUARTER_TURN - azimuthAngle & FIXED_ANGLE16_MASK,elevationAngle,
             FIXED_ANGLE16_THREE_QUARTER_TURN);
  g_AuxiliaryRotationMatrixFixed.translation.x = 0;
  g_AuxiliaryRotationMatrixFixed.translation.y = 0;
  g_AuxiliaryRotationMatrixFixed.translation.z = 0;
}


/* Address: 0x00486730.
   Stores the eight per-scene values in g_SceneBoundsFixed. bound4..bound7 are packed ARGB
   colours: the model renderer passes bound5/bound4 and bound7/bound6 as the scene colour pairs of
   ModelRender_ComputeVertexIntensityDefaultPath and ...ScaledPath. No reader of bound0..bound3 is known.
*/
void Graphics_SetSceneBoundsAndColors(GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6,
          GraphicsSceneExtentFixed bound5,GraphicsSceneExtentFixed bound4,
          GraphicsSceneExtentFixed bound3,GraphicsSceneExtentFixed bound2,
          GraphicsSceneExtentFixed bound1,GraphicsSceneExtentFixed bound0)

{
  g_SceneBoundsFixed.bound0 = bound0;
  g_SceneBoundsFixed.bound1 = bound1;
  g_SceneBoundsFixed.bound2 = bound2;
  g_SceneBoundsFixed.bound3 = bound3;
  g_SceneBoundsFixed.bound4 = bound4;
  g_SceneBoundsFixed.bound5 = bound5;
  g_SceneBoundsFixed.bound6 = bound6;
  g_SceneBoundsFixed.bound7 = bound7;
}


/* Address: 0x00486790.
   Selects the primitive queue the model renderer appends its triangles to (g_ActivePrimitiveQueue); the scene
   setup calls it with the queue freshly reset by GraphicsPrimitiveQueue_ResetGlobal.
*/
void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue)

{
  g_ActivePrimitiveQueue = queue;
}


/* Address: 0x004867B0.
   Rebuilds the four side planes of the view frustum from the current view angles, projection scale and shift
   (call after Graphics_SetViewProjectionParameters). Two edge rays are forward + / - a sideways vector of length
   1 << (12 - shift), two are forward + / - an up/down vector of that length; the plane normals are cross
   products of neighbouring rays, normalised to Q28 in g_FrustumPlaneNormalFixed_0[0..3].
*/
void Graphics_RebuildFrustumPlanes(void)

{
  uint32_t forwardX;
  uint32_t forwardY;
  uint32_t forwardZ;
  uint32_t sideAzimuthAngle16;
  uint32_t edgeAzimuthAngle16;
  int scale;
  uint32_t upElevationAngle16;
  FixedDirection viewDirection;
  uint32_t viewElevationAngle16;
  uint32_t viewAzimuthAngle16;
  
  viewElevationAngle16 = g_ViewAngle1;
  viewAzimuthAngle16 = g_ViewAngle0;
  scale = 1 << (12U - (char)g_ProjectionShift & SHIFT_COUNT_MASK);
  viewDirection = FixedMath_DirectionFromAnglesScaledRegs(g_ViewAngle1,g_ViewAngle0,g_ProjectionScaleFixed);
  forwardZ = viewDirection.z;
  forwardY = viewDirection.y;
  forwardX = viewDirection.x;
  /* rays 0 and 1: horizontal vectors of length scale, a quarter turn to either side */
  sideAzimuthAngle16 = viewAzimuthAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0,0,sideAzimuthAngle16,scale);
  edgeAzimuthAngle16 = sideAzimuthAngle16 - FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 1,0,edgeAzimuthAngle16,scale);
  g_FrustumCornerRayFixed_0[0].x = g_FrustumCornerRayFixed_0[0].x + forwardX;
  g_FrustumCornerRayFixed_0[0].y = g_FrustumCornerRayFixed_0[0].y + forwardY;
  g_FrustumCornerRayFixed_0[0].z = g_FrustumCornerRayFixed_0[0].z + forwardZ;
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x + forwardX;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y + forwardY;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z + forwardZ;
  /* rays 2 and 3: up and down (elevation + / - a quarter turn) at the view azimuth again */
  edgeAzimuthAngle16 = edgeAzimuthAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  upElevationAngle16 = viewElevationAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 2,upElevationAngle16,edgeAzimuthAngle16,scale);
  FixedMath_WriteDirectionScaled
            (g_FrustumCornerRayFixed_0 + 3,upElevationAngle16 - FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,edgeAzimuthAngle16,
             scale);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0,g_FrustumCornerRayFixed_0,
                     g_FrustumCornerRayFixed_0 + 2);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 1,g_FrustumCornerRayFixed_0 + 2,
                     g_FrustumCornerRayFixed_0 + 1);
  /* ray 1 back to the pure sideways vector, rays 2 and 3 tilted forward */
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x - forwardX;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y - forwardY;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z - forwardZ;
  g_FrustumCornerRayFixed_0[2].x = g_FrustumCornerRayFixed_0[2].x + forwardX;
  g_FrustumCornerRayFixed_0[2].y = g_FrustumCornerRayFixed_0[2].y + forwardY;
  g_FrustumCornerRayFixed_0[2].z = g_FrustumCornerRayFixed_0[2].z + forwardZ;
  g_FrustumCornerRayFixed_0[3].x = g_FrustumCornerRayFixed_0[3].x + forwardX;
  g_FrustumCornerRayFixed_0[3].y = g_FrustumCornerRayFixed_0[3].y + forwardY;
  g_FrustumCornerRayFixed_0[3].z = g_FrustumCornerRayFixed_0[3].z + forwardZ;
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 2,g_FrustumCornerRayFixed_0 + 2,
                     g_FrustumCornerRayFixed_0 + 1);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 3,g_FrustumCornerRayFixed_0 + 1,
                     g_FrustumCornerRayFixed_0 + 3);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0,g_FrustumPlaneNormalFixed_0);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0 + 1,g_FrustumPlaneNormalFixed_0 + 1);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0 + 2,g_FrustumPlaneNormalFixed_0 + 2);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0 + 3,g_FrustumPlaneNormalFixed_0 + 3);
}


/* Address: 0x004A9100.
   Initial value of g_GraphicsBackendRefreshActiveAdapter (image slot 0x004A8ED4), which MainWindowProc calls on
   WM_ACTIVATEAPP deactivation: does nothing until Graphics_Init installs
   GraphicsBackend_ShutdownGlideOnDeactivate.
*/
void GraphicsBackend_RefreshActiveAdapterNoOp(void)

{
}

/* Address: 0x004BCFE0.
   Returns the Euler angles (FixedTransform_ExtractEulerAnglesRegs, EAX/ECX/EDX) of the object's world transform
   at +0x10. Part of an object-transform helper family (0x004BCFE0-0x004BD0B0) that nothing in the executable
   calls; it is only listed in g_ThandorFunctionMap.
*/
FixedRollAzimuthElevation
GraphicsObject_ExtractTransformEulerAnglesRegs(GraphicsObjectAddress32 graphicsObject)

{
  FixedRollAzimuthElevation eulerAngles;
  
  eulerAngles = FixedTransform_ExtractEulerAnglesRegs(&((GraphicsObject *)graphicsObject)->worldTransform);
  return eulerAngles;
}


/* Address: 0x004BD000.
   Converts a world direction (elevation/azimuth) into the object's local frame: inverts the object's world
   transform at +0x10 into the shared scratch matrix, applies it to the direction's unit vector and returns the
   resulting angles. No caller in the executable (only in g_ThandorFunctionMap).
*/
FixedElevationAzimuth GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          GraphicsObjectAddress32 graphicsObject)

{
  FixedElevationAzimuth localAngles;
  
  FixedTransform_InvertRigidQ28
            (&g_GraphicsDirectionInverseTransform,&((GraphicsObject *)graphicsObject)->worldTransform);
  FixedMath_WriteDirectionQ28(&g_GraphicsDirectionWorld,elevationAngle,azimuthAngle);
  FixedTransform_ApplyPoint(&g_GraphicsDirectionLocal,&g_GraphicsDirectionWorld,&g_GraphicsDirectionInverseTransform);
  localAngles = FixedMath_VectorToAnglesVec3Regs(&g_GraphicsDirectionLocal);
  return localAngles;
}


/* Address: 0x004BD050.
   Sets the object's offset from its parent in polar form: the distance at +0x40 and the 16-bit elevation and
   azimuth packed into +0x44 (elevation in the high word), which GraphicsObject_RebuildTransformHierarchyRecursive
   turns into the translation with FixedMath_DirectionFromAnglesScaledRegs. No caller in the executable (only in
   g_ThandorFunctionMap).
*/
void GraphicsObject_SetTranslationDirectionPackedAnglesAndScale
          (AngleTurn16Stored32 elevationAngle16,AngleTurn16Stored32 azimuthAngle16,
          FixedMathScale32 distance,GraphicsObjectAddress32 graphicsObjectAddress)

{
  GraphicsObject *object;

  object = (GraphicsObject *)graphicsObjectAddress;
  object->translationDistance = distance;
  object->translationAnglesPacked = azimuthAngle16 | elevationAngle16 << 16;
}


/* Address: 0x004BD080.
   Sets the object's local rotation: the azimuth at +0x48 and the 16-bit roll and elevation packed into +0x4C
   (roll in the high word), which GraphicsObject_RebuildTransformHierarchyRecursive passes to
   FixedTransform_BuildRotationBasis (the angle names follow that function's parameters). No caller in the
   executable (only in g_ThandorFunctionMap).
*/
void GraphicsObject_SetRotationEulerAnglesPacked(AngleTurn32 azimuthAngle,AngleTurn16Stored32 rollAngle16,
          AngleTurn16Stored32 elevationAngle16,GraphicsObjectAddress32 graphicsObjectAddress)

{
  GraphicsObject *object;

  object = (GraphicsObject *)graphicsObjectAddress;
  object->rotationAzimuth = azimuthAngle;
  object->rotationAnglesPacked = elevationAngle16 | rollAngle16 << 16;
}


/* Address: 0x004BD0B0.
   Rebuilds the world transform at +0x10 from the packed rotation (+0x48/+0x4C) and polar translation
   (+0x40/+0x44): a root object (no parent at +0x64) gets the local transform directly, a child gets it composed
   with the parent's world transform; then the children are rebuilt recursively. No caller in the executable
   besides itself (only in g_ThandorFunctionMap).
   As in the original, the child loop takes its count from this object (+0x0C) but reads the child pointers from
   the parent's list at +0x78 (EBX = parent), so a root object with children would read from address 0x78.
*/
void GraphicsObject_RebuildTransformHierarchyRecursive(GraphicsObjectAddress32 graphicsObjectAddress)

{
  GraphicsObject *object;
  int remainingChildCount;
  int parentObjectOrCursor;
  GraphicsFixedMatrix3x4 *output;
  FixedDirection translationDirection;
  
  object = (GraphicsObject *)graphicsObjectAddress;
  /* a child builds its local transform in the scratch matrix shared with
     GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs */
  output = &g_GraphicsDirectionInverseTransform;
  parentObjectOrCursor = object->parentObject;
  if (parentObjectOrCursor == 0) {
    output = &object->worldTransform;
  }
  FixedTransform_BuildRotationBasis
            (output,(int)object->rotationAnglesPacked >> 16,object->rotationAnglesPacked & FIXED_ANGLE16_MASK,
             object->rotationAzimuth);
  translationDirection = FixedMath_DirectionFromAnglesScaledRegs
                    ((int)object->translationAnglesPacked >> 16,object->translationAnglesPacked & FIXED_ANGLE16_MASK,
                     object->translationDistance);
  (output->translation).x = translationDirection.x;
  (output->translation).y = translationDirection.y;
  (output->translation).z = translationDirection.z;
  remainingChildCount = object->childCount;
  if (parentObjectOrCursor != 0) {
    FixedTransform_Compose
              (&object->worldTransform,output,&((GraphicsObject *)parentObjectOrCursor)->worldTransform);
  }
  for (; remainingChildCount != 0; remainingChildCount--) {
    /* the cursor advances 4 bytes per child: childObjects[i] of the parent */
    GraphicsObject_RebuildTransformHierarchyRecursive(((GraphicsObject *)parentObjectOrCursor)->childObjects[0]);
    parentObjectOrCursor = parentObjectOrCursor + 4;
  }
}


/* Address: 0x00578560.
   Allocates the texture-slot, palette, adapter and display-mode tables, enumerates the adapters (Glide
   first, which is optional unless -GLIDE is given, then DirectDraw with their Direct3D devices and display
   modes) and installs the DirectDraw backend in the g_Graphics* slots. The display-mode hook installed
   before (the software renderer's) is kept as g_GraphicsDisplayModeFinalize and returned in EAX.
*/
StatusResult __cdecl Graphics_Init(void)

{
  TH_LEGACY_HRESULT hresult;
  SoftwareDisplayModeHookProc *displayModeHook;
  int remainingDwords;
  uint32_t remainingAdapters;
  GraphicsAdapterRecord *cursorOrResult; /* allocation, adapter cursor, and the error code */
  uint32_t *zeroCursor;
  uint32_t displayAdapterIndex;
  GraphicsAdapterRecord *adapterOrModule;
  StatusResult glideResult;
  ArenaAllocResult allocResult;
  DllLoadResult moduleLoad;
  uint32_t resolveError;
  CommandLineOptionResult optionResult;
  IDirect3D2 *direct3D2;
  IDirectDraw *directDraw;

  allocResult = g_MemoryApi.alloc(GRAPHICS_TEXTURE_SLOT_CAPACITY * sizeof(GraphicsTextureResource *));
  cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    g_GraphicsTextureSlots = (GraphicsTextureResource **)cursorOrResult;
    zeroCursor = (uint32_t *)cursorOrResult;
    for (remainingDwords = GRAPHICS_TEXTURE_SLOT_CAPACITY; remainingDwords != 0; remainingDwords--) {
      *zeroCursor = 0;
      zeroCursor++;
    }
    allocResult = g_MemoryApi.alloc(256 * sizeof(DirectDrawPaletteEntry)); /* 256 palette entries */
    cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
    if (!allocResult.failed) {
      g_TexturePaletteEntries = (DirectDrawPaletteEntry *)cursorOrResult;
      zeroCursor = (uint32_t *)cursorOrResult;
      for (remainingDwords = 256; remainingDwords != 0; remainingDwords--) {
        *zeroCursor = 0;
        zeroCursor++;
      }
      /* ADC of the not-found carry: the flag becomes nonzero without -D3DALL, and then
         Direct3D_EnumDeviceCallback accepts only hardware devices with the required caps */
      optionResult = CommandLine_FindOption(sizeof g_CommandLineOptionD3dAll,g_CommandLineOptionD3dAll);
      g_GraphicsEnumerateAllDevicesFlag = g_GraphicsEnumerateAllDevicesFlag + optionResult.notFound;
      allocResult = g_MemoryApi.alloc(GRAPHICS_ADAPTER_CAPACITY * sizeof(GraphicsAdapterRecord));
      cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        g_GraphicsAdapterCount = 0;
        g_GraphicsAdapters = (GraphicsAdapterRecord *)allocResult.payloadOrError;
        allocResult = g_MemoryApi.alloc(GRAPHICS_DISPLAY_MODE_CAPACITY * sizeof(GraphicsDisplayMode));
        cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
        if (!allocResult.failed) {
          g_GraphicsDisplayModeCount = 0;
          g_GraphicsDisplayModes = (GraphicsDisplayMode *)allocResult.payloadOrError;
#ifdef THANDOR_TEST_AIDS
          /* Glide is optional, unless -GLIDE asks for it. The windowed test aid (OPEN_THANDOR_WINDOWED, not in
             the original) runs only the software renderer, so it enumerates neither Glide nor Direct3D. */
          if (!Thandor_TestAidWindowed()) {
            glideResult = Glide3_InitAndEnumerate();
            if ((glideResult.failed) &&
                (optionResult = CommandLine_FindOption(sizeof g_CommandLineOptionGlide,g_CommandLineOptionGlide),
                 !optionResult.notFound)) {
              FatalError_ExitIfFailed(glideResult.valueOrError,true);
            }
          }
#else
          /* Glide is optional, unless -GLIDE asks for it */
          glideResult = Glide3_InitAndEnumerate();
          if ((glideResult.failed) &&
              (optionResult = CommandLine_FindOption(sizeof g_CommandLineOptionGlide,g_CommandLineOptionGlide),
               !optionResult.notFound)) {
            FatalError_ExitIfFailed(glideResult.valueOrError,true);
          }
#endif
          moduleLoad = DynDLL_Load(sz_DDRAW);
          adapterOrModule = (GraphicsAdapterRecord *)moduleLoad.moduleOrError;
          cursorOrResult = adapterOrModule;
          if (!moduleLoad.failed) {
            resolveError = DynAPI_Resolve(&pDirectDrawCreate,(HINSTANCE)adapterOrModule,sz_DirectDrawCreate);
            cursorOrResult = (GraphicsAdapterRecord *)resolveError;
            if (resolveError == 0) {
              resolveError = DynAPI_Resolve(&pDirectDrawEnumerateA,(HINSTANCE)adapterOrModule,sz_DirectDrawEnumerateA);
              cursorOrResult = (GraphicsAdapterRecord *)resolveError;
              if (resolveError == 0) {
                hresult = pDirectDrawEnumerateA(DirectDraw_EnumAdapterCallback,NULL);
                cursorOrResult = (GraphicsAdapterRecord *)FATAL_ERROR_DIRECTDRAW_NO_ADAPTER;
                if ((hresult == 0) &&
                   (remainingAdapters = g_GraphicsAdapterCount, adapterOrModule = g_GraphicsAdapters,
                   g_GraphicsAdapterCount != 0)) {
                  /* collect the Direct3D devices of every DirectDraw adapter */
                  do {
                    if ((adapterOrModule->adapterGuid).Data1 != GRAPHICS_ADAPTER_GUID_GLIDE) {
                      cursorOrResult = adapterOrModule;
                      if ((adapterOrModule->adapterGuid).Data1 == 0) {
                        cursorOrResult = NULL; /* primary display driver: NULL GUID */
                      }
                      hresult = pDirectDrawCreate(&cursorOrResult->adapterGuid,&directDraw,NULL);
                      if (hresult == 0) {
                        hresult = directDraw->lpVtbl->QueryInterface
                                          (directDraw,&IID_IDirect3D2_Local,&direct3D2);
                        if (hresult == 0) {
#ifdef THANDOR_TEST_AIDS
                          if (!Thandor_TestAidWindowed()) {
                            direct3D2->lpVtbl->EnumDevices
                                      (direct3D2,Direct3D_EnumDeviceCallback,adapterOrModule);
                          }
#else
                          direct3D2->lpVtbl->EnumDevices
                                    (direct3D2,Direct3D_EnumDeviceCallback,adapterOrModule);
#endif
                          direct3D2->lpVtbl->Release(direct3D2);
                        }
                        directDraw->lpVtbl->Release(directDraw);
                      }
                    }
                    remainingAdapters--;
                    adapterOrModule++;
                  } while (remainingAdapters != 0);
                  /* collect the display modes, tagged with the adapter index */
                  displayAdapterIndex = 0;
                  remainingAdapters = g_GraphicsAdapterCount;
                  cursorOrResult = g_GraphicsAdapters;
                  do {
                    if ((cursorOrResult->adapterGuid).Data1 != GRAPHICS_ADAPTER_GUID_GLIDE) {
                      adapterOrModule = cursorOrResult;
                      if ((cursorOrResult->adapterGuid).Data1 == 0) {
                        adapterOrModule = NULL; /* primary display driver: NULL GUID */
                      }
                      hresult = pDirectDrawCreate(&adapterOrModule->adapterGuid,&directDraw,NULL);
                      if (hresult == 0) {
                        directDraw->lpVtbl->EnumDisplayModes
                                  (directDraw,0,NULL,displayAdapterIndex,DirectDraw_EnumDisplayModeCallback);
                        directDraw->lpVtbl->Release(directDraw);
                      }
                    }
                    displayModeHook = g_GraphicsSetDisplayMode;
                    displayAdapterIndex++;
                    cursorOrResult++;
                    remainingAdapters--;
                  } while (remainingAdapters != 0);
                  cursorOrResult = (GraphicsAdapterRecord *)FATAL_ERROR_DIRECTDRAW_NO_DISPLAY_MODE;
                  if (g_GraphicsDisplayModeCount != 0) {
                    g_GraphicsBackendRefreshActiveAdapter =
                         GraphicsBackend_ShutdownGlideOnDeactivate;
                    g_GraphicsSetDisplayMode =
                         GraphicsDirectDraw_ApplyDisplayModeAndCreateResources;
                    g_GraphicsFramebufferBeginAccess = GraphicsFramebuffer_BeginAccess;
                    g_GraphicsFramebufferEndAccess = GraphicsFramebuffer_EndAccess;
                    g_GraphicsSetViewportAndClearDepth = Graphics_SetViewportAndClearDepth;
                    g_GraphicsDrawPrimitiveQueue = Graphics_DrawPrimitiveQueue;
                    g_GraphicsBeginScene = Graphics_BeginScene;
                    g_GraphicsEndScene = Graphics_EndScene;
                    g_GraphicsCreateTextureSet = GraphicsTextureSet_Create;
                    g_GraphicsDestroyTextureSet = GraphicsTextureSet_Destroy;
                    g_GraphicsRefreshTextureColor = GraphicsTextureSet_RefreshColor;
                    g_GraphicsRefreshTextureAlpha = GraphicsTextureSet_RefreshAlpha;
                    g_GraphicsRebuildAllStagingTextures = GraphicsTexture_RebuildAllStagingTextures;
                    g_GraphicsDisplayModeFinalize = displayModeHook;
                    return StatusValue_Ok((uint32_t)displayModeHook);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return StatusValue_Fail((uint32_t)cursorOrResult);
}


/* Address: 0x005794E0.
   Installed by Graphics_Init as g_GraphicsBackendRefreshActiveAdapter, which MainWindowProc calls when the
   application is deactivated (WM_ACTIVATEAPP): it shuts Glide down when the active adapter
   is the running 3dfx Glide adapter, so the full-screen Glide display is released while the game is in the
   background.
*/
void GraphicsBackend_ShutdownGlideOnDeactivate(void)

{
  if (((g_GlideRuntimeActiveCount != 0) && (g_ActiveGraphicsAdapterIndex != GRAPHICS_ADAPTER_INDEX_NONE)) &&
     (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE)) {
    Glide3_Shutdown();
  }
  return;
}


/* Address: 0x00579520.
   Tears the graphics backend down at exit (Runtime_Shutdown): blocks the cursor timer, frees the software
   cursor buffers, shuts Glide down, releases the objects of every texture slot and then every Direct3D and
   DirectDraw object, viewport first and primary surface last.
*/
void Graphics_Shutdown(void)

{
  GraphicsTextureResource **slotsOrRemaining;
  GraphicsTextureResource **remainingSlots;
  GraphicsTextureResource **slotCursor;

  /* nonzero: GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer draws nothing */
  g_GraphicsBackendAccessState = -1;
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = NULL;
  g_CursorCompositeBuffer = NULL;
  g_CursorAlternateSavedBackground = NULL;
  GlideBackend_ShutdownWrapper();
  /* GRAPHICS_TEXTURE_SLOT_CAPACITY texture slots; slotsOrRemaining first carries the table pointer (skipped
     when it is NULL), then the number of slots still to visit */
  remainingSlots = (GraphicsTextureResource **)GRAPHICS_TEXTURE_SLOT_CAPACITY;
  slotCursor = g_GraphicsTextureSlots;
  slotsOrRemaining = g_GraphicsTextureSlots;
  while (slotsOrRemaining != NULL) {
    if (*slotCursor != NULL) {
      GraphicsTexture_ReleaseObjects(*slotCursor);
    }
    slotCursor++;
    remainingSlots = (GraphicsTextureResource **)((int)remainingSlots - 1);
    slotsOrRemaining = remainingSlots;
  }
  g_LastViewportRect.x1 = 0;
  g_LastViewportRect.y1 = 0;
  g_LastViewportRect.x2 = 0;
  g_LastViewportRect.y2 = 0;
  if (g_Direct3DViewport2 != NULL) {
    g_Direct3DViewport2->lpVtbl->Release(g_Direct3DViewport2);
    g_Direct3DViewport2 = NULL;
  }
  if (g_ZSurface3 != NULL) {
    g_ZSurface3->lpVtbl->Release(g_ZSurface3);
    g_ZSurface3 = NULL;
  }
  if (g_ZSurfaceBase != NULL) {
    g_ZSurfaceBase->lpVtbl->Release(g_ZSurfaceBase);
    g_ZSurfaceBase = NULL;
  }
  if (g_Direct3DDevice2 != NULL) {
    g_Direct3DDevice2->lpVtbl->Release(g_Direct3DDevice2);
    g_Direct3DDevice2 = NULL;
  }
  if (g_Direct3D2 != NULL) {
    g_Direct3D2->lpVtbl->Release(g_Direct3D2);
    g_Direct3D2 = NULL;
  }
  if (g_BackSurface3 != NULL) {
    g_BackSurface3->lpVtbl->Release(g_BackSurface3);
    g_BackSurface3 = NULL;
  }
  if (g_BackSurfaceBase != NULL) {
    g_BackSurfaceBase->lpVtbl->Release(g_BackSurfaceBase);
    g_BackSurfaceBase = NULL;
  }
  if (g_PrimarySurface3 != NULL) {
    g_PrimarySurface3->lpVtbl->Release(g_PrimarySurface3);
    g_PrimarySurface3 = NULL;
  }
  if (g_PrimarySurfaceBase != NULL) {
    g_PrimarySurfaceBase->lpVtbl->Release(g_PrimarySurfaceBase);
    g_PrimarySurfaceBase = NULL;
  }
  return;
}


/* Address: 0x0057A5C0.
   Sets the rectangle the scene is drawn into and clears its depth buffer. The software rasterizer only takes
   the rectangle as its clip rectangle; Direct3D also gets it as its viewport (SetViewport2 only when the
   rectangle changed since the last successful call) and clears the Z-buffer inside it; Glide does its own.
*/
void Graphics_SetViewportAndClearDepth(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX)

{
  TH_LEGACY_DWORD deviceKind;
  TH_LEGACY_HRESULT hresult;
  GraphicsScreenCoordinate savedMaxY;
  GraphicsScreenCoordinate savedMaxX;
  GraphicsScreenCoordinate savedMinY;
  GraphicsScreenCoordinate savedMinX;

  deviceKind = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  if (deviceKind == GRAPHICS_DEVICE_GUID_SOFTWARE) {
    SoftwareRenderer_ClearViewport(clipMaxY,clipMaxX,clipMinY,clipMinX);
    return;
  }
  if (deviceKind != GRAPHICS_DEVICE_GUID_GLIDE) {
    /* Direct3D: the software clip rectangle is kept in step as well */
    SoftwareRenderer_ClearViewport(clipMaxY,clipMaxX,clipMinY,clipMinX);
    g_CurrentClearRect.x1 = clipMinX;
    g_CurrentClearRect.y1 = clipMinY;
    g_CurrentClearRect.x2 = clipMaxX;
    g_CurrentClearRect.y2 = clipMaxY;
    if ((((clipMinX != g_LastViewportRect.x1) || (clipMinY != g_LastViewportRect.y1)) ||
        (clipMaxX != g_LastViewportRect.x2)) || (clipMaxY != g_LastViewportRect.y2)) {
      savedMaxY = clipMaxY;
      savedMaxX = clipMaxX;
      savedMinY = clipMinY;
      savedMinX = clipMinX;
      Memory_ZeroDwords(sizeof g_Direct3DViewportState,&g_Direct3DViewportState);
      g_Direct3DViewportState.dwSize = sizeof g_Direct3DViewportState;
      g_Direct3DViewportState.dvMinZ = 0.0;
      g_Direct3DViewportState.dwX = clipMinX;
      g_Direct3DViewportState.dwY = clipMinY;
      g_Direct3DViewportState.dwWidth = clipMaxX - clipMinX;
      g_Direct3DViewportState.dwHeight = clipMaxY - clipMinY;
      g_Direct3DViewportState.dvMaxZ = 1.0;
      g_Direct3DViewportState.dvClipX = (float)clipMinX;
      g_Direct3DViewportState.dvClipY = (float)clipMinY;
      g_Direct3DViewportState.dvClipWidth = (float)(int)g_Direct3DViewportState.dwWidth;
      g_Direct3DViewportState.dvClipHeight = (float)(int)g_Direct3DViewportState.dwHeight;
      hresult = g_Direct3DViewport2->lpVtbl->SetViewport2
                        (g_Direct3DViewport2,&g_Direct3DViewportState);
      if (hresult == 0) {
        g_LastViewportRect.x1 = savedMinX;
        g_LastViewportRect.y1 = savedMinY;
        g_LastViewportRect.x2 = savedMaxX;
        g_LastViewportRect.y2 = savedMaxY;
      }
    }
    g_Direct3DViewport2->lpVtbl->Clear(g_Direct3DViewport2,1,&g_CurrentClearRect,D3DCLEAR_ZBUFFER);
    return;
  }
  Glide3_ClearViewport(clipMaxY,clipMaxX,clipMinY,clipMinX);
  return;
}


/* Address: 0x0057E6D0.
   Starts a frame on the active renderer: nothing for the software rasterizer, the (empty) Glide hook, or for
   Direct3D a restore of a lost back surface followed by IDirect3DDevice2::BeginScene. The original reports a
   failed restore or BeginScene with CF set (STC at 0x0057E700); this C version returns nothing, since the only
   caller (g_GraphicsBeginScene in FrontendModelPointerContext_RenderWorldViewQueuesClipped, 0x0050BDF7) never
   reads CF.
*/
void Graphics_BeginScene(void)

{
  TH_LEGACY_DWORD deviceKind;
  TH_LEGACY_HRESULT hresult;
  int restoreResult;
  bool isSoftwareBackend;

  deviceKind = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  isSoftwareBackend = deviceKind == GRAPHICS_DEVICE_GUID_SOFTWARE;
  if (isSoftwareBackend) {
    return;
  }
  if (deviceKind == GRAPHICS_DEVICE_GUID_GLIDE) {
    GlideBackend_BeginSceneNoOp();
    if (!isSoftwareBackend) {
      return;
    }
  }
  else {
    hresult = g_BackSurface3->lpVtbl->IsLost(g_BackSurface3);
    restoreResult = 0;
    if (hresult != 0) {
      restoreResult = g_BackSurface3->lpVtbl->Restore(g_BackSurface3);
    }
    if ((restoreResult == 0) &&
       (hresult = g_Direct3DDevice2->lpVtbl->BeginScene(g_Direct3DDevice2), hresult == 0)) {
      return;
    }
  }
  return;
}


/* Address: 0x0057E750.
   Ends the frame started by Graphics_BeginScene: IDirect3DDevice2::EndScene for Direct3D, the (empty) Glide
   hook for Glide, nothing for the software rasterizer.
*/
void Graphics_EndScene(void)

{
  TH_LEGACY_DWORD deviceKind;

  deviceKind = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  if (deviceKind != GRAPHICS_DEVICE_GUID_SOFTWARE) {
    if (deviceKind == GRAPHICS_DEVICE_GUID_GLIDE) {
      GlideBackend_EndSceneNoOp();
    }
    else {
      g_Direct3DDevice2->lpVtbl->EndScene(g_Direct3DDevice2);
    }
  }
  return;
}


/* Address: 0x0057E7A0.
   Draws a sorted primitive queue (see GraphicsPrimitiveQueue_RadixSortForRendering) inside the given clip
   rectangle. Software and Glide have their own queue walkers; for Direct3D every packet is turned into
   transformed vertices by the primitive handler its render flags select and drawn as one triangle fan.
*/
void Graphics_DrawPrimitiveQueue(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  PrimitivePacketResult packetResult;
  TH_LEGACY_DWORD deviceKind;

  deviceKind = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  if (deviceKind == GRAPHICS_DEVICE_GUID_SOFTWARE) {
    SoftwareRenderer_DrawPrimitiveQueueBridge(clipMaxY,clipMaxX,clipMinY,clipMinX,queue);
    return;
  }
  if (deviceKind != GRAPHICS_DEVICE_GUID_GLIDE) {
    packetResult = GraphicsPrimitiveQueue_Begin(queue);
    while (!packetResult.noPacket) {
      /* render-flag bits 12..17 select the handler that fills g_ImmediateTLVertices */
      g_GraphicsDispatchTable.primitive[(packetResult.packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >>
                                        12](packetResult.packet);
      g_Direct3DDevice2->lpVtbl->DrawPrimitive
                (g_Direct3DDevice2,D3DPT_TRIANGLEFAN,D3DVT_TLVERTEX,g_ImmediateTLVertices,
                 g_ImmediateVertexCount,D3DDP_DONOTUPDATEEXTENTS);
      g_PrimitiveDrawCallCount++;
      packetResult = GraphicsPrimitiveQueue_Next(queue);
    }
    return;
  }
  Glide3_DrawPrimitiveQueue(clipMaxY,clipMaxX,clipMinY,clipMinX,queue);
  return;
}


/* Address: 0x0057A330.
   Draws the software mouse cursor into backSurface before it is presented (the animation timer also redraws it
   on the primary surface when it moved): the background under the cursor is saved twice (once to draw on, once
   for GraphicsCursor_RestoreAfterPresent), the cursor frame is blended onto the first copy (the pressed image
   while a mouse button is down) and that copy is written back.
   The visibility token is latched so the restore matches what was drawn. Glide draws its cursor itself.
*/
void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface)

{
  UiPixelCoordinate cursorX;
  uint32_t cursorSubresourceIndex;
  GraphicsCursorFrameRecord *cursorFrame;
  UiPixelCoordinate cursorY;
  int drawY;

  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 != GRAPHICS_ADAPTER_GUID_GLIDE) {
    g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
    if (-1 < g_CursorVisibilityToken) { /* a negative token hides the cursor */
      cursorX = g_MouseX;
      cursorY = g_MouseY;
      if (g_CursorUseOverridePosition != 0) {
        cursorX = g_CursorOverrideX;
        cursorY = g_CursorOverrideY;
      }
      cursorFrame = g_CursorFrameRecords + g_CursorFrameIndex;
      cursorX = cursorX - cursorFrame->hotspotX;
      drawY = cursorY - cursorFrame->hotspotY;
      g_CursorCurrentDrawX = cursorX;
      g_CursorCurrentDrawY = drawY;
      GraphicsCursor_SaveSurfaceBackground(g_CursorCompositeBuffer,drawY,cursorX,backSurface);
      GraphicsCursor_SaveSurfaceBackground(g_CursorSavedBackground,drawY,cursorX,backSurface);
      cursorSubresourceIndex = cursorFrame->activeSubresourceIndex;
      if ((g_CursorButtonState & LEFT_MIDDLE_RIGHT) == 0) { /* none of the three mouse buttons is down */
        cursorSubresourceIndex = cursorFrame->idleSubresourceIndex;
      }
      /* the composite buffer holds the saved rectangle at its origin, so the cursor is drawn at (0,0) */
      g_GraphicsTextureSourceBlitSourceAlpha
                (g_FramebufferHeight,g_FramebufferWidth,0,0,0,0,cursorSubresourceIndex,g_CursorSourceAsset,
                 g_CursorCompositeBuffer);
      GraphicsCursor_RestoreSurfaceBackground(g_CursorCompositeBuffer,drawY,cursorX,backSurface);
    }
    return;
  }
  Glide3_Cursor_ComposeBeforePresent(backSurface);
  return;
}


/* Address: 0x0057A2C0.
   Removes the software cursor from backSurface again by writing back the background that
   GraphicsCursor_ComposeBeforePresent saved, so the surface is clean again (for the next frame or for drawing
   the cursor at its new position). Skipped when the
   cursor was hidden at compose time; Glide calls a no-op because it draws the cursor directly into the
   locked framebuffer.
*/
void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface)

{
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE) {
    Glide3_Cursor_RestoreAfterPresentNoOp(backSurface);
    return;
  }
  if (-1 < g_CursorCurrentVisibilityToken) {
    GraphicsCursor_RestoreSurfaceBackground
              (g_CursorSavedBackground,g_CursorCurrentDrawY,g_CursorCurrentDrawX,backSurface);
  }
  return;
}


/* Address: 0x00579EC0.
   Saves the screen rectangle under the software cursor: copies the part of sourceSurface at (drawX, drawY)
   that lies on screen into destinationBuffer (same layout, 16 or 32 bits per pixel), so the cursor can later
   be removed again with GraphicsCursor_RestoreSurfaceBackground. The surface is restored first if it was lost.
*/
void GraphicsCursor_SaveSurfaceBackground(SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface)

{
  /* Rewritten from the assembly (0x00579EC0-0x0057A0BD); Ghidra's output confused the frame pointer
     with the copy cursors. Copies a clipped rectangle of the locked surface into the buffer origin. */
  int bytesPerPixel;
  int rowPixels;
  int copyWidth;
  int copyHeight;
  uint8_t *destination;
  uint8_t *source;
  uint8_t *surfacePixels;
  TH_LEGACY_HRESULT result;

  bytesPerPixel = destinationBuffer->bytesPerPixel == 2 ? 2 : 4;
  rowPixels = (int)destinationBuffer->width;
  copyWidth = (int)destinationBuffer->width;
  copyHeight = (int)destinationBuffer->height;
  destination = destinationBuffer->pixels;
  if (drawX < 0) {
    destination = destination + -drawX * bytesPerPixel;
    copyWidth = copyWidth + drawX;
    drawX = 0;
  }
  if (drawY < 0) {
    copyHeight = copyHeight + drawY;
    destination = destination + -drawY * rowPixels * bytesPerPixel;
    drawY = 0;
  }
  if (copyWidth + drawX - (int)g_FramebufferWidth > 0) {
    copyWidth = copyWidth - (copyWidth + drawX - (int)g_FramebufferWidth);
  }
  if (copyHeight + drawY - (int)g_FramebufferHeight > 0) {
    copyHeight = copyHeight - (copyHeight + drawY - (int)g_FramebufferHeight);
  }
  if (copyWidth <= 0 || copyHeight <= 0) {
    return;
  }
  result = sourceSurface->lpVtbl->IsLost(sourceSurface);
  if (result != 0) {
    result = sourceSurface->lpVtbl->Restore(sourceSurface);
  }
  if (result == 0) {
    /* the scratch DDSURFACEDESC (its first dword is dwSize): cleared, then dwSize set */
    Memory_ZeroDwords(sizeof(DDSURFACEDESC_DX6),&g_GraphicsCursorSurfaceDescScratch);
    g_GraphicsCursorSurfaceDescScratch = sizeof(DDSURFACEDESC_DX6);
    result = sourceSurface->lpVtbl->Lock
                       (sourceSurface,NULL,
                        (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,DDLOCK_WAIT | DDLOCK_READONLY,
                        NULL);
  }
  if (result != 0) {
    return;
  }
  /* g_GraphicsCursorSurfacePixels/PitchBytes are the lpSurface/lPitch fields of the locked descriptor */
  surfacePixels = (uint8_t *)g_GraphicsCursorSurfacePixels;
  source = surfacePixels + drawY * (int)g_GraphicsCursorSurfacePitchBytes + drawX * bytesPerPixel;
  for (; copyHeight != 0; copyHeight--) {
    memcpy(destination,source,(size_t)(copyWidth * bytesPerPixel));
    destination = destination + rowPixels * bytesPerPixel;
    source = source + (int)g_GraphicsCursorSurfacePitchBytes;
  }
  sourceSurface->lpVtbl->Unlock(sourceSurface,surfacePixels);
}


/* Address: 0x0057A0C0.
   Writes a buffer filled by GraphicsCursor_SaveSurfaceBackground (or the composed cursor image) back into
   destinationSurface at (drawX, drawY), clipped to the screen exactly like the save. Used to draw the
   composed cursor and to remove it again after the present.
*/
void GraphicsCursor_RestoreSurfaceBackground(SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface)

{
  /* Rewritten from the assembly (0x0057A0C0-0x0057A2BD), mirror of
     GraphicsCursor_SaveSurfaceBackground: buffer rows back into the locked surface. */
  int bytesPerPixel;
  int rowPixels;
  int copyWidth;
  int copyHeight;
  uint8_t *source;
  uint8_t *destination;
  uint8_t *surfacePixels;
  TH_LEGACY_HRESULT result;

  bytesPerPixel = sourceBuffer->bytesPerPixel == 2 ? 2 : 4;
  rowPixels = (int)sourceBuffer->width;
  copyWidth = (int)sourceBuffer->width;
  copyHeight = (int)sourceBuffer->height;
  source = sourceBuffer->pixels;
  if (drawX < 0) {
    source = source + -drawX * bytesPerPixel;
    copyWidth = copyWidth + drawX;
    drawX = 0;
  }
  if (drawY < 0) {
    copyHeight = copyHeight + drawY;
    source = source + -drawY * rowPixels * bytesPerPixel;
    drawY = 0;
  }
  if (copyWidth + drawX - (int)g_FramebufferWidth > 0) {
    copyWidth = copyWidth - (copyWidth + drawX - (int)g_FramebufferWidth);
  }
  if (copyHeight + drawY - (int)g_FramebufferHeight > 0) {
    copyHeight = copyHeight - (copyHeight + drawY - (int)g_FramebufferHeight);
  }
  if (copyWidth <= 0 || copyHeight <= 0) {
    return;
  }
  result = destinationSurface->lpVtbl->IsLost(destinationSurface);
  if (result != 0) {
    result = destinationSurface->lpVtbl->Restore(destinationSurface);
  }
  if (result == 0) {
    /* the scratch DDSURFACEDESC (its first dword is dwSize): cleared, then dwSize set */
    Memory_ZeroDwords(sizeof(DDSURFACEDESC_DX6),&g_GraphicsCursorSurfaceDescScratch);
    g_GraphicsCursorSurfaceDescScratch = sizeof(DDSURFACEDESC_DX6);
    /* write lock, as the original (PUSH 0x21 at 0x0057A176 / 0x0057A266) */
    result = destinationSurface->lpVtbl->Lock
                       (destinationSurface,NULL,
                        (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,DDLOCK_WAIT | DDLOCK_WRITEONLY,
                        NULL);
  }
  if (result != 0) {
    return;
  }
  surfacePixels = (uint8_t *)g_GraphicsCursorSurfacePixels;
  destination = surfacePixels + drawY * (int)g_GraphicsCursorSurfacePitchBytes + drawX * bytesPerPixel;
  for (; copyHeight != 0; copyHeight--) {
    memcpy(destination,source,(size_t)(copyWidth * bytesPerPixel));
    destination = destination + (int)g_GraphicsCursorSurfacePitchBytes;
    source = source + rowPixels * bytesPerPixel;
  }
  destinationSurface->lpVtbl->Unlock(destinationSurface,surfacePixels);
}

