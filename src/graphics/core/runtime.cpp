/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/core/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

__declspec(align(8)) GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame = THANDOR_FN(GraphicsCursor_SetFrameIndex);

__declspec(align(4)) GraphicsFixedVec3 g_ViewOriginFixed = {0};

int32_t g_ProjectionScaleFixed = 0;

GraphicsFixedMatrix3x4 g_ViewProjectionMatrixFixed = {0};

GraphicsFixedMatrix3x4 g_AuxiliaryRotationMatrixFixed = {0};

SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize = 0;

int32_t g_CursorCurrentVisibilityToken = 0;

int32_t g_GraphicsBackendAccessState = -0x1;

/* allocated by Graphics_Init but no longer read (see there) */
static DirectDrawPaletteEntry *g_TexturePaletteEntries = 0;

SoftwareFramebufferAccess *g_CursorAlternateSavedBackground = 0;

/* uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0 in graphics/core/runtime.c). */
static uint32_t g_GraphicsCursorAnimationCountdown = 2;

static uint32_t g_CursorButtonReleaseClock[3] = {0};

static GraphicsCursorFrameIndex g_CursorFrameIndex = 0;

static UiPixelCoordinate g_CursorLastClickX = 0;

static UiPixelCoordinate g_CursorLastClickY = 0;

static GraphicsViewAngle16 g_ViewAngle0 = 0;

static GraphicsViewAngle16 g_ViewAngle1 = 0;

static uint32_t g_ProjectionShift = 0;

static uint32_t g_ProjectionScaleProduct = 0;

static GraphicsWideFixed g_ProjectionNumerator = {0};

static GraphicsFixedVec2 g_ProjectionCenterFixed = {0};

static GraphicsFixedMatrix3x4 g_ViewRotationMatrixFixed = {0};

static GraphicsFixedMatrix3x4 g_CameraTransformMatrixFixed = {0};

static GraphicsWideFixed g_ProjectionAngleFactors[2] = {0};

static GraphicsFixedVec2 g_AuxiliaryOrientation = {0};

static GraphicsFixedVec3 g_FrustumCornerRayFixed_0[4] = {0};

static DirectDrawEnumerateA *pDirectDrawEnumerateA = 0;

static char sz_DDRAW[6] = "DDRAW";

static char sz_DirectDrawCreate[17] = "DirectDrawCreate";

static char sz_DirectDrawEnumerateA[21] = "DirectDrawEnumerateA";

/* scratch descriptor the cursor save/restore Lock fills (lPitch, lpSurface) */
static DDSURFACEDESC_DX6 g_GraphicsCursorSurfaceDesc = {0};

static int32_t g_CursorCurrentDrawX = 0;

static int32_t g_CursorCurrentDrawY = 0;

GraphicsCursorInputEvent18 g_CursorInputEvents[256] = {0};

uint32_t g_CursorInputReadIndex = 0;

uint32_t g_CursorInputClockValue = 0;

GraphicsTextureSourceAsset *g_CursorSourceAsset = 0;

GraphicsCursorFrameRecord *g_CursorFrameRecords = 0;

GraphicsCursorFrameCount g_CursorFrameCount = 0;

GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent = THANDOR_FN(GraphicsCursor_ConsumeNextInputEvent);

GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed = {0};

GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4] = {0};

GraphicsSceneBounds8 g_SceneBoundsFixed = {0};

SoftwareFramebufferAccess *g_CursorSavedBackground = 0;

SoftwareFramebufferAccess *g_CursorCompositeBuffer = 0;

DirectDrawCreate *pDirectDrawCreate = 0;

uint32_t g_MouseEventsProcessed = 0;

/* Implementation ownership: graphics/core/runtime. */

/* Periodic cursor timer callback: keeps the software mouse cursor animated and in place independently of the
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
  Bool8 frameAdvanced;

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
#ifdef THANDOR_PLATFORM_SDL3
  /* SDL3 backend: there is no primary surface to draw on from this (timer) thread; SdlVideo_Present composes
     the cursor into every presented frame. */
  return;
#endif
  /* Windowed mode (developer tools, not in the original): the primary surface is the whole desktop, so drawing
     the cursor at framebuffer coordinates would paint over the desktop's top left corner.
     GraphicsFramebuffer_Present composes the cursor into the back surface every frame, so the timer refresh is
     skipped. */
  if (DebugHook_Windowed()) {
    return;
  }
  /* try-lock: the original atomically swaps 1 into the access state and only draws when it was 0 */
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


/* Selects the software cursor frame (GRAPHICS_CURSOR_FRAME_*) that the cursor timer animates and draws.
   Returns true when the frame was selected, false (frame unchanged) for an index at or above g_CursorFrameCount;
   the error to report for that is FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE. Installed in g_GraphicsCursorSetFrame.
*/
Bool8 GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex)

{
  if (frameIndex < g_CursorFrameCount) {
    g_CursorFrameIndex = frameIndex;
    return true;
  }
  return false;
}


/* The software cursor frame selected by GraphicsCursor_SetFrameIndex (for backends that compose the cursor
   themselves, such as the SDL3 backend). */
GraphicsCursorFrameIndex GraphicsCursor_GetFrameIndex(void)

{
  return g_CursorFrameIndex;
}


/* Takes the next mouse event from the 256-entry ring the mouse input code fills (returns false and leaves
   *outEvent untouched when it is empty; true with the event in *outEvent otherwise) and publishes it: button state, cursor position and wheel delta go to the g_Cursor* globals, a release stores
   its clock per button, a press its position as the last click. Installed in g_GraphicsCursorConsumeEvent.
   A press less than 16 clock ticks after the release of the same button and within
   +-4 pixels of the last click also sets bit 31 (double click) in the returned button state (outEvent->buttonState;
   g_CursorButtonState keeps the raw value), which UiPointer_DispatchPendingEvents passes on to the press
   dispatchers as UI_POINTER_BUTTON_REPEAT_CLICK.
*/
Bool8 GraphicsCursor_ConsumeNextInputEvent(CursorPointerEvent *outEvent)

{
  GraphicsCursorEventType consumedEventType;
  uint32_t nextReadIndex;
  GraphicsCursorClockValue eventClock;
  uint32_t eventIndex;
  uint32_t rawButtonState;
  uint32_t ticksSinceRelease;
  UiPixelCoordinate clickDeltaX;
  UiPixelCoordinate clickDeltaY;

  eventIndex = g_CursorInputReadIndex;
  nextReadIndex = g_CursorInputReadIndex + 1;
  if (g_CursorInputReadIndex == g_CursorInputWriteIndex) {
    return false;
  }
  if (GRAPHICS_CURSOR_INPUT_EVENT_CAPACITY - 1 < nextReadIndex) { /* wrap around the ring */
    nextReadIndex = 0;
  }
  g_CursorInputReadIndex = nextReadIndex;
  consumedEventType = g_CursorInputEvents[eventIndex].eventType;
  eventClock = g_CursorInputEvents[eventIndex].clockValue;
  rawButtonState = g_CursorInputEvents[eventIndex].buttonState;
  g_CursorButtonState = rawButtonState;
  /* ticksSinceRelease: ticks since the release of the pressed button; releases (and motion) leave the limit
     itself, which never counts as a double click. The compare is unsigned. */
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
  /* The returned state gets bit 31 for a double click; g_CursorButtonState above stays raw. The distance
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
  outEvent->eventType = consumedEventType;
  outEvent->buttonState = (GraphicsCursorButtonState)rawButtonState;
  outEvent->pointerX = g_CursorInputEvents[eventIndex].pointerX;
  outEvent->pointerY = g_CursorInputEvents[eventIndex].pointerY;
  outEvent->wheelDelta = g_CursorInputEvents[eventIndex].wheelDelta;
  return true;
}


/* Perspective-projects one view-space Q12 point to screen coordinates (returned as an x/y pair): the perspective
   scale is the 64-bit projection numerator divided by z, x and y are scaled by it and offset by the projection
   centre. Points with z not above the numerator's high dword (behind or too close to the eye, where the
   32-bit division would overflow) project to (0,0).
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
         (int)((int64_t)((uint64_t)(uint32_t)g_ProjectionNumerator.high << 32 | g_ProjectionNumerator.low) /
               (int64_t)viewPoint->z);
    projectedXProduct = (int64_t)viewPoint->x * (int64_t)perspectiveScaleQ12;
    projectedYProduct = (int64_t)viewPoint->y * (int64_t)perspectiveScaleQ12;
    /* bits 12..43 of the 64-bit product, i.e. the Q12 product shifted back by 12 */
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


/* Sets the screen rectangle projected geometry is clipped against, converted from pixels to Q12 (20.12 fixed
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


/* Sets up the camera for the next scene: stores the eye position (Q12 world coordinates), projection scale and
   view angles, builds the view rotation, the camera matrix (identity rotation, translation to the eye) and
   their composition g_ViewProjectionMatrixFixed, and stores the sin/cos pairs of the view azimuth plus and minus
   the half view angle atan2(1 << (12 - projectionShift), projectionScale).
   The azimuth/elevation names follow FixedMath_DirectionFromAnglesScaled, which
   Graphics_RebuildFrustumPlanes feeds with the same two angles.
*/
void Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewElevationAngle,
          GraphicsViewAngle16 viewAzimuthAngle,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX)

{
  uint32_t halfViewAngle16;
  FixedSinCos sinCosQ28;

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
  /* each factor: low = cos, high = sin (Q28) */
  sinCosQ28 = FixedMath_SinCosQ28(halfViewAngle16 + viewAzimuthAngle & FIXED_ANGLE16_MASK);
  g_ProjectionAngleFactors[0].low = (uint32_t)sinCosQ28.cosValue;
  g_ProjectionAngleFactors[0].high = sinCosQ28.sinValue;
  sinCosQ28 = FixedMath_SinCosQ28(viewAzimuthAngle - halfViewAngle16 & FIXED_ANGLE16_MASK);
  g_ProjectionAngleFactors[1].low = (uint32_t)sinCosQ28.cosValue;
  g_ProjectionAngleFactors[1].high = sinCosQ28.sinValue;
}


/* Maps the view onto a screen rectangle in pixels: the projection centre
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


/* Sets the scene's second direction (elevation/azimuth): stores the angles, their unit direction
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


/* Stores the eight per-scene values in g_SceneBoundsFixed. bound4..bound7 are packed ARGB
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


/* Selects the primitive queue the model renderer appends its triangles to (g_ActivePrimitiveQueue); the scene
   setup calls it with the queue freshly reset by GraphicsPrimitiveQueue_ResetGlobal.
*/
void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue)

{
  g_ActivePrimitiveQueue = queue;
}


/* Rebuilds the four side planes of the view frustum from the current view angles, projection scale and shift
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
  viewDirection = FixedMath_DirectionFromAnglesScaled(g_ViewAngle1,g_ViewAngle0,g_ProjectionScaleFixed);
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


/* Graphics_Init's first step, shared with the SDL3 backend (SdlVideo_Init): allocates and clears the texture-slot
   and palette tables and allocates the empty adapter and display-mode tables, in this order (the arena layout
   the texture-set sort depends on). Returns 0 or the allocator's error code. */
uint32_t Graphics_AllocateTables(void)

{
  int remainingDwords;
  void *allocation;
  uint32_t *zeroCursor;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(GRAPHICS_TEXTURE_SLOT_CAPACITY * sizeof(GraphicsTextureResource *),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsTextureSlots = (GraphicsTextureResource **)allocation;
  zeroCursor = (uint32_t *)allocation;
  for (remainingDwords = GRAPHICS_TEXTURE_SLOT_CAPACITY; remainingDwords != 0; remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  allocError = g_MemoryApi.alloc(256 * sizeof(DirectDrawPaletteEntry),&allocation); /* 256 palette entries */
  if (allocError != 0) {
    return allocError;
  }
  g_TexturePaletteEntries = (DirectDrawPaletteEntry *)allocation;
  zeroCursor = (uint32_t *)allocation;
  for (remainingDwords = 256; remainingDwords != 0; remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  allocError = g_MemoryApi.alloc(GRAPHICS_ADAPTER_CAPACITY * sizeof(GraphicsAdapterRecord),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsAdapterCount = 0;
  g_GraphicsAdapters = (GraphicsAdapterRecord *)allocation;
  allocError = g_MemoryApi.alloc(GRAPHICS_DISPLAY_MODE_CAPACITY * sizeof(GraphicsDisplayMode),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsDisplayModeCount = 0;
  g_GraphicsDisplayModes = (GraphicsDisplayMode *)allocation;
  return 0;
}


/* Allocates the texture-slot, palette, adapter and display-mode tables, enumerates the DirectDraw adapters
   (every one is a software renderer device) and their display modes and installs the DirectDraw surface
   backend in the g_Graphics* slots. The display-mode hook installed before (the software renderer's) is kept
   as g_GraphicsDisplayModeFinalize. Returns 0 on success, otherwise the error code of the failing step (never
   0). The palette table is no longer read (only the original's hardware texture upload used it); it is still
   allocated, like the texture slots, so the arena layout and with it the texture-set addresses that
   GraphicsPrimitiveQueue_RadixSortForRendering sorts opaque packets by stay as they were.
*/
uint32_t __cdecl Graphics_Init(void)

{
  TH_LEGACY_HRESULT hresult;
  SoftwareDisplayModeHookProc *displayModeHook;
  uint32_t remainingAdapters;
  uint32_t displayAdapterIndex;
  GraphicsAdapterRecord *adapter;
  struct TH_LEGACY_GUID *driverGuid;
  HINSTANCE ddrawModule;
  uint32_t allocError;
  uint32_t resolveError;
  IDirectDraw *directDraw;

  allocError = Graphics_AllocateTables();
  if (allocError != 0) {
    return allocError;
  }
  ddrawModule = DynDLL_Load(sz_DDRAW);
  if (ddrawModule == NULL) {
    return FATAL_ERROR_DLL_LOAD_FAILED;
  }
  resolveError = DynAPI_Resolve((void **)&pDirectDrawCreate,ddrawModule,sz_DirectDrawCreate);
  if (resolveError != 0) {
    return resolveError;
  }
  resolveError = DynAPI_Resolve((void **)&pDirectDrawEnumerateA,ddrawModule,sz_DirectDrawEnumerateA);
  if (resolveError != 0) {
    return resolveError;
  }
  hresult = pDirectDrawEnumerateA(DirectDraw_EnumAdapterCallback,NULL);
  if ((hresult != 0) || (g_GraphicsAdapterCount == 0)) {
    return FATAL_ERROR_DIRECTDRAW_NO_ADAPTER;
  }
  /* collect the display modes, tagged with the adapter index */
  displayAdapterIndex = 0;
  remainingAdapters = g_GraphicsAdapterCount;
  adapter = g_GraphicsAdapters;
  do {
    driverGuid = &adapter->adapterGuid;
    if ((adapter->adapterGuid).Data1 == 0) {
      driverGuid = NULL; /* primary display driver: NULL GUID */
    }
    hresult = pDirectDrawCreate(driverGuid,&directDraw,NULL);
    if (hresult == 0) {
      directDraw->lpVtbl->EnumDisplayModes
                (directDraw,0,NULL,displayAdapterIndex,
                 /* signature differs: the callback takes the context as FrontendDisplayAdapterIndex (int) */
                 (int32_t (__stdcall *)(DDSURFACEDESC_DX6 *,uint32_t))DirectDraw_EnumDisplayModeCallback);
      directDraw->lpVtbl->Release(directDraw);
    }
    displayAdapterIndex++;
    adapter++;
    remainingAdapters--;
  } while (remainingAdapters != 0);
  /* the display-mode hook installed before (the software renderer's) */
  displayModeHook = g_GraphicsSetDisplayMode;
  if (g_GraphicsDisplayModeCount == 0) {
    return FATAL_ERROR_DIRECTDRAW_NO_DISPLAY_MODE;
  }
  /* g_GraphicsSetViewportAndClearDepth, g_GraphicsDrawPrimitiveQueue, g_GraphicsBeginScene/EndScene and the
     texture refresh/rebuild slots keep their software renderer defaults */
  /* signature differs: the adapter index is FrontendDisplayAdapterIndex (int), not uint32_t */
  g_GraphicsSetDisplayMode = (SoftwareDisplayModeHookProc *)GraphicsDirectDraw_ApplyDisplayModeAndCreateResources;
  g_GraphicsFramebufferBeginAccess = GraphicsFramebuffer_BeginAccess;
  g_GraphicsFramebufferEndAccess = GraphicsFramebuffer_EndAccess;
  g_GraphicsCreateTextureSet = GraphicsTextureSet_Create;
  g_GraphicsDestroyTextureSet = GraphicsTextureSet_Destroy;
  g_GraphicsDisplayModeFinalize = displayModeHook;
  return 0;
}


/* Tears the graphics backend down at exit (Runtime_Shutdown): blocks the cursor timer, frees the software
   cursor buffers and releases every DirectDraw surface, primary surface last.
*/
void Graphics_Shutdown(void)

{
  /* nonzero: GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer draws nothing */
  g_GraphicsBackendAccessState = -1;
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = NULL;
  g_CursorCompositeBuffer = NULL;
  g_CursorAlternateSavedBackground = NULL;
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


/* Draws the software mouse cursor into backSurface before it is presented (the animation timer also redraws it
   on the primary surface when it moved): the background under the cursor is saved twice (once to draw on, once
   for GraphicsCursor_RestoreAfterPresent), the cursor frame is blended onto the first copy (the pressed image
   while a mouse button is down) and that copy is written back.
   The visibility token is latched so the restore matches what was drawn.
*/
void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface)

{
  UiPixelCoordinate cursorX;
  uint32_t cursorSubresourceIndex;
  GraphicsCursorFrameRecord *cursorFrame;
  UiPixelCoordinate cursorY;
  int drawY;

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


/* Removes the software cursor from backSurface again by writing back the background that
   GraphicsCursor_ComposeBeforePresent saved, so the surface is clean again (for the next frame or for drawing
   the cursor at its new position). Skipped when the cursor was hidden at compose time.
*/
void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface)

{
  if (-1 < g_CursorCurrentVisibilityToken) {
    GraphicsCursor_RestoreSurfaceBackground
              (g_CursorSavedBackground,g_CursorCurrentDrawY,g_CursorCurrentDrawX,backSurface);
  }
  return;
}


/* Saves the screen rectangle under the software cursor: copies the part of sourceSurface at (drawX, drawY)
   that lies on screen into destinationBuffer (same layout, 16 or 32 bits per pixel), so the cursor can later
   be removed again with GraphicsCursor_RestoreSurfaceBackground. The surface is restored first if it was lost.
*/
void GraphicsCursor_SaveSurfaceBackground(SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface)

{
  /* Copies a clipped rectangle of the locked surface into the buffer origin. */
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
    /* the scratch DDSURFACEDESC: cleared, then dwSize set */
    Memory_ZeroDwords(sizeof(DDSURFACEDESC_DX6),&g_GraphicsCursorSurfaceDesc);
    g_GraphicsCursorSurfaceDesc.dwSize = sizeof(DDSURFACEDESC_DX6);
    result = sourceSurface->lpVtbl->Lock
                       (sourceSurface,NULL,&g_GraphicsCursorSurfaceDesc,DDLOCK_WAIT | DDLOCK_READONLY,NULL);
  }
  if (result != 0) {
    return;
  }
  surfacePixels = (uint8_t *)g_GraphicsCursorSurfaceDesc.lpSurface;
  source = surfacePixels + drawY * (int)g_GraphicsCursorSurfaceDesc.lPitch + drawX * bytesPerPixel;
  for (; copyHeight != 0; copyHeight--) {
    memcpy(destination,source,(size_t)(copyWidth * bytesPerPixel));
    destination = destination + rowPixels * bytesPerPixel;
    source = source + (int)g_GraphicsCursorSurfaceDesc.lPitch;
  }
  sourceSurface->lpVtbl->Unlock(sourceSurface,surfacePixels);
}


/* Writes a buffer filled by GraphicsCursor_SaveSurfaceBackground (or the composed cursor image) back into
   destinationSurface at (drawX, drawY), clipped to the screen exactly like the save. Used to draw the
   composed cursor and to remove it again after the present.
*/
void GraphicsCursor_RestoreSurfaceBackground(SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface)

{
  /* Mirror of GraphicsCursor_SaveSurfaceBackground: buffer rows back into the locked surface. */
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
    /* the scratch DDSURFACEDESC: cleared, then dwSize set */
    Memory_ZeroDwords(sizeof(DDSURFACEDESC_DX6),&g_GraphicsCursorSurfaceDesc);
    g_GraphicsCursorSurfaceDesc.dwSize = sizeof(DDSURFACEDESC_DX6);
    /* write-only lock (DDLOCK_WAIT | DDLOCK_WRITEONLY), as in the original */
    result = destinationSurface->lpVtbl->Lock
                       (destinationSurface,NULL,&g_GraphicsCursorSurfaceDesc,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
  }
  if (result != 0) {
    return;
  }
  surfacePixels = (uint8_t *)g_GraphicsCursorSurfaceDesc.lpSurface;
  destination = surfacePixels + drawY * (int)g_GraphicsCursorSurfaceDesc.lPitch + drawX * bytesPerPixel;
  for (; copyHeight != 0; copyHeight--) {
    memcpy(destination,source,(size_t)(copyWidth * bytesPerPixel));
    destination = destination + (int)g_GraphicsCursorSurfaceDesc.lPitch;
    source = source + rowPixels * bytesPerPixel;
  }
  destinationSurface->lpVtbl->Unlock(destinationSurface,surfacePixels);
}

