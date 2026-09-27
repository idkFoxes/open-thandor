/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/core/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: graphics/core/runtime. */

/* Address: 0x00576C30.
   Periodic cursor timer callback: keeps the software mouse cursor animated and in place independently of the
   game's frame rate. Every second tick it steps the idle and active animation subresources of the current cursor
   frame (wrapping to the first one); when the frame changed or mouse events moved the cursor, and the graphics
   backend is not in use, the cursor is redrawn directly on the primary surface.
*/
void __thandor_preserve_eax_edx GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void)

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
  /* try-lock: the original swaps 1 into the access state (XCHG) and only draws when it was 0 */
  previousAccessState = g_GraphicsBackendAccessState;
  if (g_CursorSourceAsset != NULL) {
    LOCK();
    g_GraphicsBackendAccessState = 1;
    UNLOCK();
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
   Ownership: graphics/core/runtime.
   Purpose: Sets g_CursorFrameIndex when frameIndex is below g_CursorFrameCount. EAX is the engine code 0x2D on
   both paths. CF clear means success; CF set means the index was out of range.
*/
CursorFrameResult __thandor_eax_cf_preserve_ecx_edx
GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex)

{
  CursorFrameResult successResult;
  CursorFrameResult failureResult;
  
  if (frameIndex < g_CursorFrameCount) {
    g_CursorFrameIndex = frameIndex;
    successResult.errorCode = 0x2d;
    successResult.failed = false;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.errorCode = 0x2d;
  return failureResult;
}


/* Address: 0x004168E0.
   Ownership: graphics/core/runtime.
   Purpose: Consumes one GraphicsCursorInputEvent from the 256-entry ring. CF clear means an event was consumed; CF
   set means the ring was empty. The function publishes button state, cursor position, wheel delta, and release
   clocks. A press within 16 clock units and within plus/minus four pixels of the previous press sets bit 31 in
   g_CursorButtonState.
*/
CursorEventResult __thandor_input_event_regs_cf GraphicsCursor_ConsumeNextInputEvent(void)

{
  GraphicsCursorEventType consumedEventType;
  uint32_t nextReadIndex;
  CursorEventResult eventResult;
  CursorEventResult emptyResult;
  GraphicsCursorEventType eventType;
  GraphicsCursorClockValue eventClock;
  uint32_t eventIndex;
  uint32_t leftReleaseClock;
  uint32_t middleReleaseClock;
  
  eventIndex = g_CursorInputReadIndex;
  nextReadIndex = g_CursorInputReadIndex + 1;
  if (g_CursorInputReadIndex == g_CursorInputWriteIndex) {
    memset(&emptyResult, 0, sizeof emptyResult);
    emptyResult.queueEmpty = true;
    return emptyResult;
  }
  if (0xff < nextReadIndex) {
    nextReadIndex = 0;
  }
  g_CursorInputReadIndex = nextReadIndex;
  consumedEventType = g_CursorInputEvents[eventIndex].eventType00;
  eventClock = g_CursorInputEvents[eventIndex].clockValue14;
  g_CursorButtonState = g_CursorInputEvents[eventIndex].buttonState04;
  leftReleaseClock = g_CursorButtonReleaseClock[0];
  middleReleaseClock = g_CursorButtonReleaseClock[1];
  if (((((consumedEventType != LEFT_PRESS) && (consumedEventType != MIDDLE_PRESS)) && (consumedEventType != RIGHT_PRESS)) &&
      ((leftReleaseClock = eventClock, consumedEventType != LEFT_RELEASE &&
       (leftReleaseClock = g_CursorButtonReleaseClock[0], middleReleaseClock = eventClock,
       consumedEventType != MIDDLE_RELEASE)))) &&
     (middleReleaseClock = g_CursorButtonReleaseClock[1], consumedEventType == RIGHT_RELEASE)) {
    g_CursorButtonReleaseClock[2] = eventClock;
  }
  g_CursorButtonReleaseClock[1] = middleReleaseClock;
  g_CursorButtonReleaseClock[0] = leftReleaseClock;
  g_CursorOverrideX = g_CursorInputEvents[eventIndex].pointerX08;
  g_CursorOverrideY = g_CursorInputEvents[eventIndex].pointerY0C;
  g_CursorWheelDelta = g_CursorInputEvents[eventIndex].wheelDelta10;
  if ((consumedEventType != MOTION_OR_WHEEL) && (consumedEventType < 4)) {
    g_CursorLastClickX = g_CursorInputEvents[eventIndex].pointerX08;
    g_CursorLastClickY = g_CursorInputEvents[eventIndex].pointerY0C;
  }
  /* Called through GraphicsCursorConsumeEventProc: the event also leaves the button state in EBX,
     position in ECX/EDX and wheel delta in ESI, which Ghidra's EAX/CF view of this function dropped. */
  eventResult.eventType = consumedEventType;
  eventResult.buttonState = g_CursorButtonState;
  eventResult.pointerX = g_CursorInputEvents[eventIndex].pointerX08;
  eventResult.pointerY = g_CursorInputEvents[eventIndex].pointerY0C;
  eventResult.wheelDelta = g_CursorInputEvents[eventIndex].wheelDelta10;
  eventResult.queueEmpty = false;
  return eventResult;
}


/* Address: 0x00486430.
   Perspective-projects one view-space Q12 point to screen coordinates (EAX = x, EDX = y): the perspective scale
   is the 64-bit projection numerator divided by z, x and y are scaled by it and offset by the projection
   centre. Points with z not above the numerator's high dword (behind or too close to the eye, where the
   32-bit IDIV would overflow) project to (0,0).
*/
GraphicsProjectedPointPair __thandor_eax_edx_cf_preserve_ecx
Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint)

{
  int perspectiveScaleQ12;
  GraphicsProjectedPointPair projectedPoint;
  GraphicsProjectedPointPair offscreenPoint;
  int64_t projectedXProduct;
  int64_t projectedYProduct;
  
  if (g_ProjectionNumerator.high < viewPoint->z) {
    perspectiveScaleQ12 = (int)(THANDOR_BITCAST(GraphicsWideFixed, int64_t, g_ProjectionNumerator) / (int64_t)viewPoint->z);
    projectedXProduct = (int64_t)viewPoint->x * (int64_t)perspectiveScaleQ12;
    projectedYProduct = (int64_t)viewPoint->y * (int64_t)perspectiveScaleQ12;
    /* SHLD EDX,EAX,20: bits 12..43 of the 64-bit product, i.e. the Q12 product shifted back by 12 */
    projectedPoint.projectedY =
         ((int)((uint64_t)projectedYProduct >> 0x20) << 0x14 | (uint32_t)projectedYProduct >> 0xc) +
         g_ProjectionCenterFixed.component1;
    projectedPoint.projectedX =
         g_ProjectionCenterFixed.component0 +
         ((int)((uint64_t)projectedXProduct >> 0x20) << 0x14 | (uint32_t)projectedXProduct >> 0xc);
    return projectedPoint;
  }
  offscreenPoint.projectedX = 0;
  offscreenPoint.projectedY = 0;
  return offscreenPoint;
}


/* Address: 0x00486490.
   Ownership: graphics/core/runtime.
   Purpose: Stores four integer bounds as 20.12 fixed-point values. The callee writes them in reverse stack order
   into minX, minY, maxX, and maxY. Typed parameters: p0 maxY→GraphicsScreenCoordinate_V307, p1
   maxX→GraphicsScreenCoordinate_V307, p2 minY→GraphicsScreenCoordinate_V307, p3
   minX→GraphicsScreenCoordinate_V307. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetProjectionClipRect
          (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,GraphicsScreenCoordinate minY
          ,GraphicsScreenCoordinate minX)

{
  g_ProjectionClipRect.minX = minX << 0xc;
  g_ProjectionClipRect.minY = minY << 0xc;
  g_ProjectionClipRect.maxX = maxX << 0xc;
  g_ProjectionClipRect.maxY = maxY << 0xc;
  return;
}


/* Address: 0x004864D0.
   Ownership: graphics/core/runtime.
   Purpose: Copies the seven-value camera/projection parameter block, builds the primary orientation and camera
   matrices, and computes two projection factors.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_Compose
   [core/math/fixed], FixedMath_Atan2Angle16 [core/math/fixed], FixedMath_SinCosQ28 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewAngle1,
          GraphicsViewAngle16 viewAngle0,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX)

{
  uint32_t projectionAngle16;
  
  g_ViewOriginFixed.x = originX;
  g_ViewOriginFixed.y = originY;
  g_ViewOriginFixed.z = originZ;
  g_ProjectionScaleFixed = projectionScale;
  g_ViewAngle0 = viewAngle0;
  g_ViewAngle1 = viewAngle1;
  FixedTransform_BuildRotationBasis
            (&g_ViewRotationMatrixFixed,0x4000 - viewAngle0 & 0xffff,viewAngle1,0xc000);
  g_ViewRotationMatrixFixed.translation.x = 0;
  g_ViewRotationMatrixFixed.translation.y = 0;
  g_ViewRotationMatrixFixed.translation.z = 0;
  g_CameraTransformMatrixFixed.translation.x = -originX;
  g_CameraTransformMatrixFixed.translation.y = -originY;
  g_CameraTransformMatrixFixed.translation.z = -originZ;
  g_CameraTransformMatrixFixed.basisRow0[0] = 0x10000000;
  g_CameraTransformMatrixFixed.basisRow0[1] = 0;
  g_CameraTransformMatrixFixed.basisRow0[2] = 0;
  g_CameraTransformMatrixFixed.basisRow1[0] = 0;
  g_CameraTransformMatrixFixed.basisRow1[1] = 0x10000000;
  g_CameraTransformMatrixFixed.basisRow1[2] = 0;
  g_CameraTransformMatrixFixed.basisRow2[0] = 0;
  g_CameraTransformMatrixFixed.basisRow2[1] = 0;
  g_CameraTransformMatrixFixed.basisRow2[2] = 0x10000000;
  FixedTransform_Compose
            (&g_ViewProjectionMatrixFixed,&g_CameraTransformMatrixFixed,&g_ViewRotationMatrixFixed);
  g_ProjectionShift = projectionShift;
  projectionAngle16 =
       FixedMath_Atan2Angle16(1 << (0xcU - (char)projectionShift & 0x1f),projectionScale);
  g_ProjectionAngleFactors[0] =
       THANDOR_BITCAST(FixedSinCosEdxEax8, GraphicsWideFixed, FixedMath_SinCosQ28(projectionAngle16 + viewAngle0 & 0xffff));
  g_ProjectionAngleFactors[1] = THANDOR_BITCAST(FixedSinCosEdxEax8, GraphicsWideFixed, FixedMath_SinCosQ28(viewAngle0 - projectionAngle16 & 0xffff));
  return;
}


/* Address: 0x00486640.
   Ownership: graphics/core/runtime.
   Purpose: Calculates the fixed-point projection center and the signed 64-bit perspective numerator from four
   viewport bounds. Typed parameters: p0 bound0→GraphicsScreenCoordinate_V307, p1
   bound1→GraphicsScreenCoordinate_V307, p2 bound2→GraphicsScreenCoordinate_V307, p3
   bound3→GraphicsScreenCoordinate_V307. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetProjectionViewport
          (GraphicsScreenCoordinate bound0,GraphicsScreenCoordinate bound1,
          GraphicsScreenCoordinate bound2,GraphicsScreenCoordinate bound3)

{
  int projectionShiftDelta;
  int64_t projectionScaleProduct;
  uint8_t rightShiftAmount;
  
  g_ProjectionCenterFixed.component0 = (bound3 + bound1) * 0x800;
  g_ProjectionCenterFixed.component1 = (bound2 + bound0) * 0x800;
  projectionShiftDelta = g_ProjectionShift - 1;
  projectionScaleProduct = (int64_t)(bound1 - bound3) * (int64_t)(int)g_ProjectionScaleFixed;
  g_ProjectionScaleProduct = (uint32_t)projectionScaleProduct;
  if (projectionShiftDelta != 0) {
    if (projectionShiftDelta < 0) {
      rightShiftAmount = -(uint8_t)projectionShiftDelta & 0x1f;
      g_ProjectionScaleProduct =
           g_ProjectionScaleProduct >> rightShiftAmount |
           (int)((uint64_t)projectionScaleProduct >> 0x20) << 0x20 - rightShiftAmount;
    }
    else {
      g_ProjectionScaleProduct = g_ProjectionScaleProduct << ((uint8_t)projectionShiftDelta & 0x1f);
    }
  }
  g_ProjectionNumerator.low = g_ProjectionScaleProduct << 0xc;
  g_ProjectionNumerator.high = (int)g_ProjectionScaleProduct >> 0x14;
  return;
}


/* Address: 0x004866C0.
   Ownership: graphics/core/runtime.
   Purpose: Stores two auxiliary engine angles and rebuilds the secondary fixed-point rotation matrix. Kept
   distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed
   parameters: p0 angle1→AngleTurn32, p1 angle0→AngleTurn32. Calling convention, storage, body bytes, control flow,
   and executable data remain unchanged.
   Cross-module calls: FixedMath_WriteDirectionQ28 [core/math/fixed], FixedTransform_BuildRotationBasis
   [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetAuxiliaryOrientation(AngleTurn32 angle1,AngleTurn32 angle0)

{
  g_AuxiliaryOrientation.component0 = angle0;
  g_AuxiliaryOrientation.component1 = angle1;
  FixedMath_WriteDirectionQ28(&g_AuxiliaryForwardDirectionFixed,angle1,angle0);
  FixedTransform_BuildRotationBasis
            (&g_AuxiliaryRotationMatrixFixed,0x4000 - angle0 & 0xffff,angle1,0xc000);
  g_AuxiliaryRotationMatrixFixed.translation.x = 0;
  g_AuxiliaryRotationMatrixFixed.translation.y = 0;
  g_AuxiliaryRotationMatrixFixed.translation.z = 0;
  return;
}


/* Address: 0x00486730.
   Ownership: graphics/core/runtime.
   Purpose: Copies eight scene-bound values. Exact axis ordering remains unresolved.
*/
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetSceneBounds
          (GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6,
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
  return;
}


/* Address: 0x00486790.
   Ownership: graphics/core/runtime.
   Purpose: Sets the primitive queue used by the scene/mesh submission code.
*/
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue)

{
  g_ActivePrimitiveQueue = queue;
  return;
}


/* Address: 0x004867B0.
   Ownership: graphics/core/runtime.
   Purpose: Builds four fixed-point corner rays from the current projection parameters, derives four side-plane
   normals with cross products, and normalizes them.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_WriteDirectionScaled
   [core/math/fixed], FixedVec3_CrossQ12 [core/math/fixed], FixedVec3_NormalizeQ28 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx Graphics_RebuildFrustumPlanes(void)

{
  uint32_t forwardX;
  uint32_t forwardY;
  uint32_t forwardZ;
  uint32_t cornerAzimuthAngle16;
  uint32_t sideAzimuthAngle16;
  int scale;
  uint32_t elevationAngle;
  FixedDirection viewDirection;
  uint32_t viewElevationAngle16;
  uint32_t viewAzimuthAngle16;
  
  viewElevationAngle16 = g_ViewAngle1;
  viewAzimuthAngle16 = g_ViewAngle0;
  scale = 1 << (0xcU - (char)g_ProjectionShift & 0x1f);
  viewDirection = FixedMath_DirectionFromAnglesScaledRegs(g_ViewAngle1,g_ViewAngle0,g_ProjectionScaleFixed);
  forwardZ = viewDirection.z;
  forwardY = viewDirection.y;
  forwardX = viewDirection.x;
  cornerAzimuthAngle16 = viewAzimuthAngle16 + 0x4000 & 0xffff;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0,0,cornerAzimuthAngle16,scale);
  sideAzimuthAngle16 = cornerAzimuthAngle16 - 0x8000 & 0xffff;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 1,0,sideAzimuthAngle16,scale);
  g_FrustumCornerRayFixed_0[0].x = g_FrustumCornerRayFixed_0[0].x + forwardX;
  g_FrustumCornerRayFixed_0[0].y = g_FrustumCornerRayFixed_0[0].y + forwardY;
  g_FrustumCornerRayFixed_0[0].z = g_FrustumCornerRayFixed_0[0].z + forwardZ;
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x + forwardX;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y + forwardY;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z + forwardZ;
  sideAzimuthAngle16 = sideAzimuthAngle16 + 0x4000 & 0xffff;
  elevationAngle = viewElevationAngle16 + 0x4000 & 0xffff;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 2,elevationAngle,sideAzimuthAngle16,scale);
  FixedMath_WriteDirectionScaled
            (g_FrustumCornerRayFixed_0 + 3,elevationAngle - 0x8000 & 0xffff,sideAzimuthAngle16,scale);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0,g_FrustumCornerRayFixed_0,
                     g_FrustumCornerRayFixed_0 + 2);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 1,g_FrustumCornerRayFixed_0 + 2,
                     g_FrustumCornerRayFixed_0 + 1);
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
  return;
}


/* Address: 0x004A9100.
   Ownership: graphics/core/runtime.
   Purpose: Default no-op callback stored at 004A8ED4 before graphics backend initialization installs
   GraphicsBackend_RefreshActiveAdapterIfReady.
*/
void GraphicsBackend_RefreshActiveAdapterNoOp(void)

{
  return;
}

/* Address: 0x004BCFE0.
   Ownership: graphics/core/runtime.
   Purpose: Passes the fixed transform stored at object offset 0x10 to the shared Euler-angle extractor and returns
   the packed register results. Typed parameters: p0 graphicsObject→GraphicsObjectAddress32_V345. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: FixedTransform_ExtractEulerAnglesRegs [core/math/fixed].
*/
FixedEulerAnglesEaxEcxEdx12
GraphicsObject_ExtractTransformEulerAnglesRegs(GraphicsObjectAddress32 graphicsObject)

{
  FixedEulerPairEdxEax8 eulerAnglePair;
  FixedEulerAnglesEaxEcxEdx12 eulerAngles;
  
  eulerAngles = FixedTransform_ExtractEulerAnglesRegs((GraphicsFixedMatrix3x4 *)(graphicsObject + 0x10));
  return eulerAngles;
}


/* Address: 0x004BD000.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object convert world direction angles to local angles register result.
   Cross-module calls: FixedTransform_InvertRigidQ28 [core/math/fixed], FixedMath_WriteDirectionQ28
   [core/math/fixed], FixedTransform_ApplyPoint [core/math/fixed], FixedMath_VectorToAnglesVec3Regs
   [core/math/fixed].
*/
FixedElevationAzimuth __thandor_preserve_eax
GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          GraphicsObjectAddress32 graphicsObject)

{
  FixedElevationAzimuth localAngles;
  
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)THANDOR_ADDR(g_GraphicsDirectionInverseTransform,0),(GraphicsFixedMatrix3x4 *)(graphicsObject + 0x10));
  FixedMath_WriteDirectionQ28((GraphicsFixedVec3 *)THANDOR_ADDR(g_GraphicsDirectionWorld,0),elevationAngle,azimuthAngle);
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)THANDOR_ADDR(g_GraphicsDirectionLocal,0),(GraphicsFixedVec3 *)THANDOR_ADDR(g_GraphicsDirectionWorld,0),
             (GraphicsFixedMatrix3x4 *)THANDOR_ADDR(g_GraphicsDirectionInverseTransform,0));
  localAngles = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)THANDOR_ADDR(g_GraphicsDirectionLocal,0));
  return localAngles;
}


/* Address: 0x004BD050.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object set translation direction packed angles and scale.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsObject_SetTranslationDirectionPackedAnglesAndScale
          (AngleTurn16Stored32 directionAngle0Stored16,AngleTurn16Stored32 directionAngle1Stored16,
          FixedMathScale32 translationScale,GraphicsObjectAddress32 graphicsObjectAddress)

{
  *(FixedMathScale32 *)(graphicsObjectAddress + 0x40) = translationScale;
  *(AngleTurn16Stored32 *)(graphicsObjectAddress + 0x44) =
       directionAngle1Stored16 | directionAngle0Stored16 << 0x10;
  return;
}


/* Address: 0x004BD080.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object set rotation euler angles packed.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsObject_SetRotationEulerAnglesPacked
          (AngleTurn32 rotationAngle2,AngleTurn16Stored32 rotationAngle0Stored16,
          AngleTurn16Stored32 rotationAngle1Stored16,GraphicsObjectAddress32 graphicsObjectAddress)

{
  *(AngleTurn32 *)(graphicsObjectAddress + 0x48) = rotationAngle2;
  *(AngleTurn16Stored32 *)(graphicsObjectAddress + 0x4c) =
       rotationAngle1Stored16 | rotationAngle0Stored16 << 0x10;
  return;
}


/* Address: 0x004BD0B0.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object rebuild transform hierarchy recursive.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedMath_DirectionFromAnglesScaledRegs
   [core/math/fixed], FixedTransform_Compose [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsObject_RebuildTransformHierarchyRecursive(GraphicsObjectAddress32 graphicsObjectAddress)

{
  int remainingChildCount;
  int parentObjectOrCursor;
  GraphicsFixedMatrix3x4 *output;
  FixedDirection translationDirection;
  
  output = (GraphicsFixedMatrix3x4 *)THANDOR_ADDR(g_GraphicsDirectionInverseTransform,0);
  parentObjectOrCursor = *(int *)(graphicsObjectAddress + 100);
  if (parentObjectOrCursor == 0) {
    output = (GraphicsFixedMatrix3x4 *)(graphicsObjectAddress + 0x10);
  }
  FixedTransform_BuildRotationBasis
            (output,(int)*(uint32_t *)(graphicsObjectAddress + 0x4c) >> 0x10,
             *(uint32_t *)(graphicsObjectAddress + 0x4c) & 0xffff,
             *(AngleTurn32 *)(graphicsObjectAddress + 0x48));
  translationDirection = FixedMath_DirectionFromAnglesScaledRegs
                    ((int)*(uint32_t *)(graphicsObjectAddress + 0x44) >> 0x10,
                     *(uint32_t *)(graphicsObjectAddress + 0x44) & 0xffff,
                     *(FixedMathScale32 *)(graphicsObjectAddress + 0x40));
  (output->translation).x = translationDirection.x;
  (output->translation).y = translationDirection.y;
  (output->translation).z = translationDirection.z;
  remainingChildCount = *(int *)(graphicsObjectAddress + 0xc);
  if (parentObjectOrCursor != 0) {
    FixedTransform_Compose
              ((GraphicsFixedMatrix3x4 *)(graphicsObjectAddress + 0x10),output,
               (GraphicsFixedMatrix3x4 *)(parentObjectOrCursor + 0x10));
  }
  for (; remainingChildCount != 0; remainingChildCount = remainingChildCount + -1) {
    GraphicsObject_RebuildTransformHierarchyRecursive(*(GraphicsObjectAddress32 *)(parentObjectOrCursor + 0x78));
    parentObjectOrCursor = parentObjectOrCursor + 4;
  }
  return;
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
  GraphicsAdapterRecord *cursorOrResult; /* also the fill cursor of the zeroing loops, and the error code */
  uint32_t displayAdapterIndex;
  GraphicsAdapterRecord *adapterOrModule;
  StatusResult glideResult;
  ArenaAllocResult allocResult;
  DllLoadResult moduleLoad;
  DynApiResolveResult procResolve;
  CommandLineOptionResult optionResult;
  IDirect3D2 *direct3D2;
  IDirectDraw *directDraw;

  allocResult = g_MemoryApi.alloc(0x4000); /* 4096 texture-slot pointers */
  cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    g_GraphicsTextureSlots = (GraphicsTextureResource **)cursorOrResult;
    for (remainingDwords = 0x1000; remainingDwords != 0; remainingDwords--) {
      (cursorOrResult->adapterGuid).Data1 = 0;
      cursorOrResult = (GraphicsAdapterRecord *)&(cursorOrResult->adapterGuid).Data2;
    }
    allocResult = g_MemoryApi.alloc(0x400); /* 256 palette entries */
    cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
    if (!allocResult.failed) {
      g_TexturePaletteEntries = (DirectDrawPaletteEntry *)cursorOrResult;
      for (remainingDwords = 0x100; remainingDwords != 0; remainingDwords--) {
        (cursorOrResult->adapterGuid).Data1 = 0;
        cursorOrResult = (GraphicsAdapterRecord *)&(cursorOrResult->adapterGuid).Data2;
      }
      /* ADC of the not-found carry: the flag becomes nonzero without -D3DALL, and then
         Direct3D_EnumDeviceCallback accepts only hardware devices with the required caps */
      optionResult = CommandLine_FindOption(sizeof g_CommandLineOptionD3dAll,g_CommandLineOptionD3dAll);
      g_GraphicsEnumerateAllDevicesFlag = g_GraphicsEnumerateAllDevicesFlag + optionResult.notFound;
      allocResult = g_MemoryApi.alloc(0x800); /* 16 adapter records of 0x80 bytes */
      cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        g_GraphicsAdapterCount = 0;
        g_GraphicsAdapters = (GraphicsAdapterRecord *)allocResult.payloadOrError;
        allocResult = g_MemoryApi.alloc(0x1000);
        cursorOrResult = (GraphicsAdapterRecord *)allocResult.payloadOrError;
        if (!allocResult.failed) {
          g_GraphicsDisplayModeCount = 0;
          g_GraphicsDisplayModes = (GraphicsDisplayMode *)allocResult.payloadOrError;
          /* Glide is optional, unless -GLIDE asks for it */
          glideResult = Glide3_InitAndEnumerate();
          if ((glideResult.failed) && (optionResult = CommandLine_FindOption(sizeof g_CommandLineOptionGlide,g_CommandLineOptionGlide), !optionResult.notFound)) {
            FatalError_ExitIfFailed(glideResult.valueOrError,true);
          }
          moduleLoad = DynDLL_Load(dynapi_2);
          adapterOrModule = (GraphicsAdapterRecord *)moduleLoad.moduleOrError;
          cursorOrResult = adapterOrModule;
          if (!moduleLoad.failed) {
            procResolve = DynAPI_Resolve(&pDirectDrawCreate,(HINSTANCE)adapterOrModule,dynapi_17);
            cursorOrResult = procResolve.procedureOrError;
            if (!procResolve.failed) {
              procResolve = DynAPI_Resolve(&pDirectDrawEnumerateA,(HINSTANCE)adapterOrModule,dynapi_18);
              cursorOrResult = procResolve.procedureOrError;
              if (!procResolve.failed) {
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
                          direct3D2->lpVtbl->EnumDevices
                                    (direct3D2,Direct3D_EnumDeviceCallback,adapterOrModule);
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
                         GraphicsBackend_RefreshActiveAdapterIfReady;
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
   application is deactivated (WM_ACTIVATEAPP): despite its name it shuts Glide down when the active adapter
   is the running 3dfx Glide adapter, so the full-screen Glide display is released while the game is in the
   background.
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsBackend_RefreshActiveAdapterIfReady(void)

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
void __thandor_void_preserve_eax_ecx_edx Graphics_Shutdown(void)

{
  GraphicsTextureResource **slotsOrRemaining;
  GraphicsTextureResource **remainingSlots;
  GraphicsTextureResource **slotCursor;

  g_GraphicsBackendAccessState = -1; /* nonzero: GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer draws nothing */
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = NULL;
  g_CursorCompositeBuffer = NULL;
  g_CursorAlternateSavedBackground = NULL;
  GlideBackend_ShutdownWrapper();
  /* 0x1000 texture slots; slotsOrRemaining first carries the table pointer (skipped when it is NULL), then
     the number of slots still to visit */
  remainingSlots = (GraphicsTextureResource **)0x1000;
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
void __thandor_void_preserve_eax_ecx_edx
Graphics_SetViewportAndClearDepth
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
      Memory_ZeroDwords(0x2c,&g_Direct3DViewportState); /* sizeof(D3DVIEWPORT2) */
      g_Direct3DViewportState.dwSize = 0x2c;
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
   failed restore or BeginScene with CF set; this C version returns nothing.
*/
void __thandor_void_preserve_eax_ecx_edx Graphics_BeginScene(void)

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
void __thandor_void_preserve_eax_ecx_edx Graphics_EndScene(void)

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
void __thandor_void_preserve_eax_ecx_edx
Graphics_DrawPrimitiveQueue
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
      g_GraphicsDispatchTable.primitive[(packetResult.packet->renderFlags & 0x3f000) >> 12]
                (packetResult.packet);
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
   on the primary surface when it moved): the background under the cursor is saved twice (once to draw on, once for GraphicsCursor_RestoreAfterPresent), the cursor frame is
   blended onto the first copy (the pressed image while a mouse button is down) and that copy is written back.
   The visibility token is latched so the restore matches what was drawn. Glide draws its cursor itself.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface)

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
      if ((g_CursorButtonState & 7) == 0) { /* none of the three mouse buttons is down */
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
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface)

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
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_SaveSurfaceBackground
          (SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
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
    /* 0x6C = sizeof(DDSURFACEDESC): cleared, then dwSize set */
    Memory_ZeroDwords(0x6c,&g_GraphicsCursorSurfaceDescScratch);
    g_GraphicsCursorSurfaceDescScratch = 0x6c;
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
void __thandor_void_preserve_eax_ecx_edx
GraphicsCursor_RestoreSurfaceBackground
          (SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
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
    /* 0x6C = sizeof(DDSURFACEDESC): cleared, then dwSize set */
    Memory_ZeroDwords(0x6c,&g_GraphicsCursorSurfaceDescScratch);
    g_GraphicsCursorSurfaceDescScratch = 0x6c;
    /* differs from the original: 0x0057A176/0x0057A266 push 0x21 (DDLOCK_WAIT | DDLOCK_WRITEONLY) here */
    result = destinationSurface->lpVtbl->Lock
                       (destinationSurface,NULL,
                        (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,DDLOCK_WAIT | DDLOCK_READONLY,
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

