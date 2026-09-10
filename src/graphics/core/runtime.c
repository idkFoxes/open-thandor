#include <thandor/graphics/core/runtime.h>

/* Implementation ownership: graphics/core/runtime. */

/* Address: 0x00576C30.
   Ownership: graphics/core/runtime.
   Purpose: 20 ms graphics cursor timer. Advances animation coordinates, marks the cursor dirty, acquires the
   graphics guard, restores/redraws cursor background on the primary surface, and releases the guard.
   Local calls: GraphicsCursor_RestoreAfterPresent, GraphicsCursor_ComposeBeforePresent.
*/
void __cdecl GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void)

{
  GraphicsCursorFrameRecord *pGVar1;
  GraphicsCursorFrameIndex GVar2;
  sdword sVar3;
  GraphicsSubresourceIndex GVar4;
  GraphicsSubresourceIndex GVar5;
  
  GVar2 = g_CursorFrameIndex;
  pGVar1 = g_CursorFrameRecords;
  if (g_CursorVisibilityToken < 0) {
    return;
  }
  g_CursorInputClockValue = g_CursorInputClockValue + 1;
  g_GraphicsCursorAnimationCountdown = g_GraphicsCursorAnimationCountdown + -1;
  if (g_GraphicsCursorAnimationCountdown == 0) {
    g_GraphicsCursorAnimationCountdown = 2;
    GVar4 = g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex + 1;
    GVar5 = g_CursorFrameRecords[g_CursorFrameIndex].activeSubresourceIndex + 1;
    if (g_CursorFrameRecords[g_CursorFrameIndex].idleAnimationLastSubresourceIndex < GVar4) {
      GVar4 = g_CursorFrameRecords[g_CursorFrameIndex].idleAnimationFirstSubresourceIndex;
    }
    if (g_CursorFrameRecords[g_CursorFrameIndex].activeAnimationLastSubresourceIndex < GVar5) {
      GVar5 = g_CursorFrameRecords[g_CursorFrameIndex].activeAnimationFirstSubresourceIndex;
    }
    if (GVar4 != g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex) {
      g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex = GVar4;
      pGVar1[GVar2].activeSubresourceIndex = GVar5;
      goto GraphicsCursor_RefreshComposedCursorIfNeeded;
    }
  }
  if (g_MouseEventsProcessed == 0) {
    return;
  }
GraphicsCursor_RefreshComposedCursorIfNeeded:
  sVar3 = g_GraphicsBackendAccessState;
  if (g_CursorSourceAsset != (GraphicsTextureSourceAsset *)0x0) {
    LOCK();
    g_GraphicsBackendAccessState = 1;
    UNLOCK();
    if (sVar3 == 0) {
      g_MouseEventsProcessed = 0;
      GraphicsCursor_RestoreAfterPresent(g_PrimarySurface3);
      GraphicsCursor_ComposeBeforePresent(g_PrimarySurface3);
      g_GraphicsBackendAccessState = g_GraphicsBackendAccessState + -1;
    }
  }
  return;
}

/* Address: 0x004168B0.
   Ownership: graphics/core/runtime.
   Purpose: Sets g_CursorFrameIndex when frameIndex is below g_CursorFrameCount. EAX is the engine code 0x2D on
   both paths. CF clear means success; CF set means the index was out of range.
*/
dword GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex)

{
  if (frameIndex < g_CursorFrameCount) {
    g_CursorFrameIndex = frameIndex;
    return 0x2d;
  }
  return 0x2d;
}

/* Address: 0x004168E0.
   Ownership: graphics/core/runtime.
   Purpose: Consumes one GraphicsCursorInputEvent from the 256-entry ring. CF clear means an event was consumed; CF
   set means the ring was empty. The function publishes button state, cursor position, wheel delta, and release
   clocks. A press within 16 clock units and within plus/minus four pixels of the previous press sets bit 31 in
   g_CursorButtonState.
*/
void __cdecl GraphicsCursor_ConsumeNextInputEvent(void)

{
  uint nextReadIndex;
  GraphicsCursorEventType eventType;
  GraphicsCursorClockValue eventClock;
  dword eventIndex;
  dword leftReleaseClock;
  dword middleReleaseClock;
  
  eventIndex = g_CursorInputReadIndex;
  nextReadIndex = g_CursorInputReadIndex + 1;
  if (g_CursorInputReadIndex == g_CursorInputWriteIndex) {
    return;
  }
  if (0xff < nextReadIndex) {
    nextReadIndex = 0;
  }
  g_CursorInputReadIndex = nextReadIndex;
  eventType = g_CursorInputEvents[eventIndex].eventType00;
  eventClock = g_CursorInputEvents[eventIndex].clockValue14;
  g_CursorButtonState = g_CursorInputEvents[eventIndex].buttonState04;
  leftReleaseClock = g_CursorButtonReleaseClock[0];
  middleReleaseClock = g_CursorButtonReleaseClock[1];
  if (((((eventType != LEFT_PRESS) && (eventType != MIDDLE_PRESS)) && (eventType != RIGHT_PRESS)) &&
      ((leftReleaseClock = eventClock, eventType != LEFT_RELEASE &&
       (leftReleaseClock = g_CursorButtonReleaseClock[0], middleReleaseClock = eventClock,
       eventType != MIDDLE_RELEASE)))) &&
     (middleReleaseClock = g_CursorButtonReleaseClock[1], eventType == RIGHT_RELEASE)) {
    g_CursorButtonReleaseClock[2] = eventClock;
  }
  g_CursorButtonReleaseClock[1] = middleReleaseClock;
  g_CursorButtonReleaseClock[0] = leftReleaseClock;
  g_CursorOverrideX = g_CursorInputEvents[eventIndex].pointerX08;
  g_CursorOverrideY = g_CursorInputEvents[eventIndex].pointerY0C;
  g_CursorWheelDelta = g_CursorInputEvents[eventIndex].wheelDelta10;
  if ((eventType != MOTION_OR_WHEEL) && (eventType < 4)) {
    g_CursorLastClickX = g_CursorInputEvents[eventIndex].pointerX08;
    g_CursorLastClickY = g_CursorInputEvents[eventIndex].pointerY0C;
  }
  return;
}

/* Address: 0x00486430.
   Ownership: graphics/core/runtime.
   Purpose: Projects one view-space fixed-point point.
*/
GraphicsProjectedPointEdxEax8 Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint)

{
  int perspectiveScaleQ12;
  longlong projectedXProduct;
  longlong projectedYProduct;
  
  if (g_ProjectionNumerator.high < viewPoint->z) {
    perspectiveScaleQ12 = (int)((longlong)g_ProjectionNumerator / (longlong)viewPoint->z);
    projectedXProduct = (longlong)viewPoint->x * (longlong)perspectiveScaleQ12;
    projectedYProduct = (longlong)viewPoint->y * (longlong)perspectiveScaleQ12;
    return CONCAT44(((int)((ulonglong)projectedYProduct >> 0x20) << 0x14 |
                    (uint)projectedYProduct >> 0xc) + g_ProjectionCenterFixed.component1,
                    g_ProjectionCenterFixed.component0 +
                    ((int)((ulonglong)projectedXProduct >> 0x20) << 0x14 |
                    (uint)projectedXProduct >> 0xc));
  }
  return 0;
}

/* Address: 0x00486490.
   Ownership: graphics/core/runtime.
   Purpose: Stores four integer bounds as 20.12 fixed-point values. The callee writes them in reverse stack order
   into minX, minY, maxX, and maxY. Typed parameters: p0 maxY→GraphicsScreenCoordinate_V307, p1
   maxX→GraphicsScreenCoordinate_V307, p2 minY→GraphicsScreenCoordinate_V307, p3
   minX→GraphicsScreenCoordinate_V307. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void Graphics_SetProjectionClipRect
               (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,
               GraphicsScreenCoordinate minY,GraphicsScreenCoordinate minX)

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
void Graphics_SetViewProjectionParameters
               (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewAngle1,
               GraphicsViewAngle16 viewAngle0,GraphicsProjectionScale projectionScale,
               GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
               GraphicsWorldCoordinateQ12 originX)

{
  dword projectionAngle16;
  AngleTurn32 angle;
  
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
       (GraphicsWideFixed)FixedMath_SinCosQ28(projectionAngle16 + viewAngle0 & 0xffff);
  g_ProjectionAngleFactors[1] = (GraphicsWideFixed)FixedMath_SinCosQ28(angle);
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
void Graphics_SetProjectionViewport
               (GraphicsScreenCoordinate bound0,GraphicsScreenCoordinate bound1,
               GraphicsScreenCoordinate bound2,GraphicsScreenCoordinate bound3)

{
  int projectionShiftDelta;
  longlong projectionScaleProduct;
  byte rightShiftAmount;
  
  g_ProjectionCenterFixed.component0 = (bound3 + bound1) * 0x800;
  g_ProjectionCenterFixed.component1 = (bound2 + bound0) * 0x800;
  projectionShiftDelta = g_ProjectionShift - 1;
  projectionScaleProduct = (longlong)(bound1 - bound3) * (longlong)(int)g_ProjectionScaleFixed;
  g_ProjectionScaleProduct = (dword)projectionScaleProduct;
  if (projectionShiftDelta != 0) {
    if (projectionShiftDelta < 0) {
      rightShiftAmount = -(byte)projectionShiftDelta & 0x1f;
      g_ProjectionScaleProduct =
           g_ProjectionScaleProduct >> rightShiftAmount |
           (int)((ulonglong)projectionScaleProduct >> 0x20) << 0x20 - rightShiftAmount;
    }
    else {
      g_ProjectionScaleProduct = g_ProjectionScaleProduct << ((byte)projectionShiftDelta & 0x1f);
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
void Graphics_SetAuxiliaryOrientation(AngleTurn32 angle1,AngleTurn32 angle0)

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
void Graphics_SetSceneBounds
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
void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue)

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
void __cdecl Graphics_RebuildFrustumPlanes(void)

{
  int extraout_EAX;
  int extraout_EAX_00;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  uint cornerAzimuthAngle16;
  uint uVar1;
  int scale;
  uint elevationAngle;
  dword viewElevationAngle16;
  dword viewAzimuthAngle16;
  
  viewElevationAngle16 = g_ViewAngle1;
  viewAzimuthAngle16 = g_ViewAngle0;
  scale = 1 << (0xcU - (char)g_ProjectionShift & 0x1f);
  FixedMath_DirectionFromAnglesScaledRegs(g_ViewAngle1,g_ViewAngle0,g_ProjectionScaleFixed);
  cornerAzimuthAngle16 = viewAzimuthAngle16 + 0x4000 & 0xffff;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0,0,cornerAzimuthAngle16,scale);
  uVar1 = cornerAzimuthAngle16 - 0x8000 & 0xffff;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 1,0,uVar1,scale);
  g_FrustumCornerRayFixed_0[0].x = g_FrustumCornerRayFixed_0[0].x + extraout_EAX;
  g_FrustumCornerRayFixed_0[0].y = g_FrustumCornerRayFixed_0[0].y + extraout_ECX;
  g_FrustumCornerRayFixed_0[0].z = g_FrustumCornerRayFixed_0[0].z + extraout_EDX;
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x + extraout_EAX;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y + extraout_ECX;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z + extraout_EDX;
  uVar1 = uVar1 + 0x4000 & 0xffff;
  elevationAngle = viewElevationAngle16 + 0x4000 & 0xffff;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 2,elevationAngle,uVar1,scale);
  FixedMath_WriteDirectionScaled
            (g_FrustumCornerRayFixed_0 + 3,elevationAngle - 0x8000 & 0xffff,uVar1,scale);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0,g_FrustumCornerRayFixed_0,
                     g_FrustumCornerRayFixed_0 + 2);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 1,g_FrustumCornerRayFixed_0 + 2,
                     g_FrustumCornerRayFixed_0 + 1);
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x - extraout_EAX_00;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y - extraout_ECX_00;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z - extraout_EDX_00;
  g_FrustumCornerRayFixed_0[2].x = g_FrustumCornerRayFixed_0[2].x + extraout_EAX_00;
  g_FrustumCornerRayFixed_0[2].y = g_FrustumCornerRayFixed_0[2].y + extraout_ECX_00;
  g_FrustumCornerRayFixed_0[2].z = g_FrustumCornerRayFixed_0[2].z + extraout_EDX_00;
  g_FrustumCornerRayFixed_0[3].x = g_FrustumCornerRayFixed_0[3].x + extraout_EAX_00;
  g_FrustumCornerRayFixed_0[3].y = g_FrustumCornerRayFixed_0[3].y + extraout_ECX_00;
  g_FrustumCornerRayFixed_0[3].z = g_FrustumCornerRayFixed_0[3].z + extraout_EDX_00;
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
FixedEulerPairEdxEax8
GraphicsObject_ExtractTransformEulerAnglesRegs(GraphicsObjectAddress32 graphicsObject)

{
  FixedEulerPairEdxEax8 eulerAnglePair;
  
  eulerAnglePair =
       FixedTransform_ExtractEulerAnglesRegs((GraphicsFixedMatrix3x4 *)(graphicsObject + 0x10));
  return eulerAnglePair;
}

/* Address: 0x004BD000.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object convert world direction angles to local angles register result.
   Cross-module calls: FixedTransform_InvertRigidQ28 [core/math/fixed], FixedMath_WriteDirectionQ28
   [core/math/fixed], FixedTransform_ApplyPoint [core/math/fixed], FixedMath_VectorToAnglesVec3Regs
   [core/math/fixed].
*/
undefined4
GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs
          (AngleTurn32 param_1,AngleTurn32 param_2,int param_3)

{
  undefined4 in_EAX;
  
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)0x4bcf20,(GraphicsFixedMatrix3x4 *)(param_3 + 0x10));
  FixedMath_WriteDirectionQ28((GraphicsFixedVec3 *)0x4bcf50,param_1,param_2);
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)0x4bcf5c,(GraphicsFixedVec3 *)0x4bcf50,
             (GraphicsFixedMatrix3x4 *)0x4bcf20);
  FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)0x4bcf5c);
  return in_EAX;
}

/* Address: 0x004BD050.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object set translation direction packed angles and scale.
*/
undefined8
GraphicsObject_SetTranslationDirectionPackedAnglesAndScale
          (int param_1,uint param_2,undefined4 param_3,int param_4)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  *(undefined4 *)(param_4 + 0x40) = param_3;
  *(uint *)(param_4 + 0x44) = param_2 | param_1 << 0x10;
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BD080.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object set rotation euler angles packed.
*/
undefined8
GraphicsObject_SetRotationEulerAnglesPacked(undefined4 param_1,int param_2,uint param_3,int param_4)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  *(undefined4 *)(param_4 + 0x48) = param_1;
  *(uint *)(param_4 + 0x4c) = param_3 | param_2 << 0x10;
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BD0B0.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics object rebuild transform hierarchy recursive.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedMath_DirectionFromAnglesScaledRegs
   [core/math/fixed], FixedTransform_Compose [core/math/fixed].
*/
undefined8 GraphicsObject_RebuildTransformHierarchyRecursive(int param_1)

{
  undefined4 in_EAX;
  GraphicsWorldCoordinateQ12 extraout_ECX;
  int extraout_ECX_00;
  int iVar1;
  int extraout_ECX_01;
  undefined4 in_EDX;
  int iVar2;
  GraphicsFixedMatrix3x4 *output;
  FixedDirectionXZEdxEax8 FVar3;
  
  output = (GraphicsFixedMatrix3x4 *)0x4bcf20;
  iVar2 = *(int *)(param_1 + 100);
  if (iVar2 == 0) {
    output = (GraphicsFixedMatrix3x4 *)(param_1 + 0x10);
  }
  FixedTransform_BuildRotationBasis
            (output,(int)*(uint *)(param_1 + 0x4c) >> 0x10,*(uint *)(param_1 + 0x4c) & 0xffff,
             *(AngleTurn32 *)(param_1 + 0x48));
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    ((int)*(uint *)(param_1 + 0x44) >> 0x10,*(uint *)(param_1 + 0x44) & 0xffff,
                     *(FixedMathScale32 *)(param_1 + 0x40));
  (output->translation).x = (GraphicsWorldCoordinateQ12)FVar3;
  (output->translation).y = extraout_ECX;
  (output->translation).z = (GraphicsWorldCoordinateQ12)(FVar3 >> 0x20);
  iVar1 = *(int *)(param_1 + 0xc);
  if (iVar2 != 0) {
    FixedTransform_Compose
              ((GraphicsFixedMatrix3x4 *)(param_1 + 0x10),output,
               (GraphicsFixedMatrix3x4 *)(iVar2 + 0x10));
    iVar1 = extraout_ECX_00;
  }
  if (iVar1 != 0) {
    do {
      GraphicsObject_RebuildTransformHierarchyRecursive(*(int *)(iVar2 + 0x78));
      iVar2 = iVar2 + 4;
    } while (extraout_ECX_01 != 1);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00578560.
   Ownership: graphics/core/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: CommandLine_FindOption [platform/bootstrap/runtime], Glide3_InitAndEnumerate
   [graphics/backend/glide], DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime].
*/
dword __cdecl Graphics_Init(void)

{
  GraphicsAdapterRecord *pGVar1;
  GraphicsAdapterRecord *pGVar2;
  TH_LEGACY_HRESULT TVar3;
  SoftwareDisplayModeHookProc *pSVar4;
  int iVar5;
  dword dVar6;
  dword arg3;
  bool bVar7;
  byte bVar8;
  undefined1 uVar9;
  IDirect3D2 *direct3D2;
  IDirectDraw *directDraw;
  
  bVar7 = &stack0xffffffe8 < (undefined1 *)0x8;
  pGVar1 = (*g_MemoryApi.alloc)(0x4000);
  if (!bVar7) {
    bVar7 = false;
    g_GraphicsTextureSlots = (GraphicsTextureResource **)pGVar1;
    for (iVar5 = 0x1000; iVar5 != 0; iVar5 = iVar5 + -1) {
      (pGVar1->adapterGuid).Data1 = 0;
      pGVar1 = (GraphicsAdapterRecord *)&(pGVar1->adapterGuid).Data2;
    }
    pGVar1 = (*g_MemoryApi.alloc)(0x400);
    if (!bVar7) {
      bVar8 = 0;
      g_TexturePaletteEntries = (DirectDrawPaletteEntry *)pGVar1;
      for (iVar5 = 0x100; iVar5 != 0; iVar5 = iVar5 + -1) {
        (pGVar1->adapterGuid).Data1 = 0;
        pGVar1 = (GraphicsAdapterRecord *)&(pGVar1->adapterGuid).Data2;
      }
      CommandLine_FindOption(7,s_D3DALL_00578078);
      uVar9 = CARRY4(g_GraphicsEnumerateAllDevicesFlag,(uint)bVar8);
      g_GraphicsEnumerateAllDevicesFlag = g_GraphicsEnumerateAllDevicesFlag + bVar8;
      pGVar1 = (*g_MemoryApi.alloc)(0x800);
      if (!(bool)uVar9) {
        g_GraphicsAdapterCount = 0;
        g_GraphicsAdapters = pGVar1;
        pGVar1 = (*g_MemoryApi.alloc)(0x1000);
        if (!(bool)uVar9) {
          g_GraphicsDisplayModeCount = 0;
          g_GraphicsDisplayModes = (GraphicsDisplayMode *)pGVar1;
          Glide3_InitAndEnumerate();
          if (((bool)uVar9) && (CommandLine_FindOption(6,s_GLIDE_0057ee84), !(bool)uVar9)) {
            uVar9 = 1;
            (*g_FatalErrorPrimaryDispatchCf)();
          }
          pGVar2 = (GraphicsAdapterRecord *)DynDLL_Load(dynapi_2);
          pGVar1 = pGVar2;
          if (((!(bool)uVar9) &&
              (pGVar1 = DynAPI_Resolve(&pDirectDrawCreate,(HINSTANCE)pGVar2,dynapi_17), !(bool)uVar9
              )) && (pGVar1 = DynAPI_Resolve(&pDirectDrawEnumerateA,(HINSTANCE)pGVar2,dynapi_18),
                    !(bool)uVar9)) {
            TVar3 = (*pDirectDrawEnumerateA)(DirectDraw_EnumAdapterCallback,(TH_LEGACY_LPVOID)0x0);
            pGVar1 = (GraphicsAdapterRecord *)&k_LowAddressLiteral00000017;
            if ((TVar3 == 0) &&
               (dVar6 = g_GraphicsAdapterCount, pGVar2 = g_GraphicsAdapters,
               g_GraphicsAdapterCount != 0)) {
              do {
                if ((pGVar2->adapterGuid).Data1 != 1) {
                  pGVar1 = pGVar2;
                  if ((pGVar2->adapterGuid).Data1 == 0) {
                    pGVar1 = (GraphicsAdapterRecord *)0x0;
                  }
                  TVar3 = (*pDirectDrawCreate)
                                    (&pGVar1->adapterGuid,&directDraw,(TH_LEGACY_LPVOID)0x0);
                  if (TVar3 == 0) {
                    pGVar1 = (GraphicsAdapterRecord *)directDraw;
                    TVar3 = (*directDraw->lpVtbl->QueryInterface)
                                      (directDraw,&IID_IDirect3D2_Local,&direct3D2);
                    if (TVar3 == 0) {
                      (*direct3D2->lpVtbl->EnumDevices)
                                (direct3D2,Direct3D_EnumDeviceCallback,pGVar1);
                      (*direct3D2->lpVtbl->Release)(direct3D2);
                    }
                    (*directDraw->lpVtbl->Release)(directDraw);
                  }
                }
                dVar6 = dVar6 - 1;
                pGVar2 = pGVar2 + 1;
              } while (dVar6 != 0);
              arg3 = 0;
              dVar6 = g_GraphicsAdapterCount;
              pGVar1 = g_GraphicsAdapters;
              do {
                if ((pGVar1->adapterGuid).Data1 != 1) {
                  pGVar2 = pGVar1;
                  if ((pGVar1->adapterGuid).Data1 == 0) {
                    pGVar2 = (GraphicsAdapterRecord *)0x0;
                  }
                  TVar3 = (*pDirectDrawCreate)
                                    (&pGVar2->adapterGuid,&directDraw,(TH_LEGACY_LPVOID)0x0);
                  if (TVar3 == 0) {
                    (*directDraw->lpVtbl->EnumDisplayModes)
                              (directDraw,0,(DDSURFACEDESC_DX6 *)0x0,arg3,
                               DirectDraw_EnumDisplayModeCallback);
                    (*directDraw->lpVtbl->Release)(directDraw);
                  }
                }
                pSVar4 = g_GraphicsDisplayModeHook;
                arg3 = arg3 + 1;
                pGVar1 = pGVar1 + 1;
                dVar6 = dVar6 - 1;
              } while (dVar6 != 0);
              pGVar1 = (GraphicsAdapterRecord *)0x18;
              if (g_GraphicsDisplayModeCount != 0) {
                g_GraphicsBackendRefreshActiveAdapterCf =
                     GraphicsBackend_RefreshActiveAdapterIfReady;
                g_GraphicsDisplayModeHook = GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf;
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
                g_GraphicsDisplayModeFinalizeCf = pSVar4;
                return (dword)pSVar4;
              }
            }
          }
        }
      }
    }
  }
  return (dword)pGVar1;
}

/* Address: 0x005794E0.
   Ownership: graphics/core/runtime.
   Purpose: Graphics backend callback that checks the selected adapter record and invokes the active-backend
   refresh routine only when graphics state and the adapter are ready.
   Cross-module calls: Glide3_Shutdown [graphics/backend/glide].
*/
void __cdecl GraphicsBackend_RefreshActiveAdapterIfReady(void)

{
  if (((g_GlideRuntimeActiveCount != 0) && (g_ActiveGraphicsAdapterIndex != -1)) &&
     (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1)) {
    Glide3_Shutdown();
  }
  return;
}

/* Address: 0x00579520.
   Ownership: graphics/core/runtime.
   Purpose: Handles graphics shutdown.
   Cross-module calls: GlideBackend_ShutdownWrapper [graphics/backend/glide], GraphicsTexture_ReleaseObjects
   [graphics/resources/texture].
*/
void __cdecl Graphics_Shutdown(void)

{
  GraphicsTextureResource **ppGVar1;
  GraphicsTextureResource **ppGVar2;
  GraphicsTextureResource **extraout_ECX;
  GraphicsTextureResource **ppGVar3;
  
  g_GraphicsBackendAccessState = -1;
  (*g_MemoryApi.free)(g_CursorSavedBackground);
  (*g_MemoryApi.free)(g_CursorCompositeBuffer);
  (*g_MemoryApi.free)(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = (SoftwareFramebufferAccess *)0x0;
  g_CursorCompositeBuffer = (SoftwareFramebufferAccess *)0x0;
  g_CursorAlternateSavedBackground = (SoftwareFramebufferAccess *)0x0;
  GlideBackend_ShutdownWrapper();
  ppGVar2 = (GraphicsTextureResource **)0x1000;
  ppGVar3 = g_GraphicsTextureSlots;
  ppGVar1 = g_GraphicsTextureSlots;
  while (ppGVar1 != (GraphicsTextureResource **)0x0) {
    if (*ppGVar3 != (GraphicsTextureResource *)0x0) {
      GraphicsTexture_ReleaseObjects(*ppGVar3);
      ppGVar2 = extraout_ECX;
    }
    ppGVar3 = ppGVar3 + 1;
    ppGVar2 = (GraphicsTextureResource **)((int)ppGVar2 + -1);
    ppGVar1 = ppGVar2;
  }
  g_LastViewportRect.x1 = 0;
  g_LastViewportRect.y1 = 0;
  g_LastViewportRect.x2 = 0;
  g_LastViewportRect.y2 = 0;
  if (g_Direct3DViewport2 != (IDirect3DViewport2 *)0x0) {
    (*g_Direct3DViewport2->lpVtbl->Release)(g_Direct3DViewport2);
    g_Direct3DViewport2 = (IDirect3DViewport2 *)0x0;
  }
  if (g_ZSurface3 != (IDirectDrawSurface3 *)0x0) {
    (*g_ZSurface3->lpVtbl->Release)(g_ZSurface3);
    g_ZSurface3 = (IDirectDrawSurface3 *)0x0;
  }
  if (g_ZSurfaceBase != (IDirectDrawSurface *)0x0) {
    (*g_ZSurfaceBase->lpVtbl->Release)(g_ZSurfaceBase);
    g_ZSurfaceBase = (IDirectDrawSurface *)0x0;
  }
  if (g_Direct3DDevice2 != (IDirect3DDevice2 *)0x0) {
    (*g_Direct3DDevice2->lpVtbl->Release)(g_Direct3DDevice2);
    g_Direct3DDevice2 = (IDirect3DDevice2 *)0x0;
  }
  if (g_Direct3D2 != (IDirect3D2 *)0x0) {
    (*g_Direct3D2->lpVtbl->Release)(g_Direct3D2);
    g_Direct3D2 = (IDirect3D2 *)0x0;
  }
  if (g_BackSurface3 != (IDirectDrawSurface3 *)0x0) {
    (*g_BackSurface3->lpVtbl->Release)(g_BackSurface3);
    g_BackSurface3 = (IDirectDrawSurface3 *)0x0;
  }
  if (g_BackSurfaceBase != (IDirectDrawSurface *)0x0) {
    (*g_BackSurfaceBase->lpVtbl->Release)(g_BackSurfaceBase);
    g_BackSurfaceBase = (IDirectDrawSurface *)0x0;
  }
  if (g_PrimarySurface3 != (IDirectDrawSurface3 *)0x0) {
    (*g_PrimarySurface3->lpVtbl->Release)(g_PrimarySurface3);
    g_PrimarySurface3 = (IDirectDrawSurface3 *)0x0;
  }
  if (g_PrimarySurfaceBase != (IDirectDrawSurface *)0x0) {
    (*g_PrimarySurfaceBase->lpVtbl->Release)(g_PrimarySurfaceBase);
    g_PrimarySurfaceBase = (IDirectDrawSurface *)0x0;
  }
  return;
}

/* Address: 0x0057A5C0.
   Ownership: graphics/core/runtime.
   Purpose: Transforms four engine coordinates, updates IDirect3DViewport2 when the rectangle changes, and clears
   the depth buffer inside the resulting D3DRECT. Typed parameters: p0 coordinate0→GraphicsScreenCoordinate_V307,
   p1 coordinate1→GraphicsScreenCoordinate_V307, p2 coordinate2→GraphicsScreenCoordinate_V307, p3
   coordinate3→GraphicsScreenCoordinate_V307. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: SoftwareRenderer_ClearViewport [graphics/backend/software], Memory_ZeroDwords
   [core/memory/allocator], Glide3_ClearViewport [graphics/backend/glide].
*/
void Graphics_SetViewportAndClearDepth
               (GraphicsScreenCoordinate coordinate0,GraphicsScreenCoordinate coordinate1,
               GraphicsScreenCoordinate coordinate2,GraphicsScreenCoordinate coordinate3)

{
  TH_LEGACY_DWORD TVar1;
  int extraout_EAX;
  TH_LEGACY_DWORD extraout_EAX_00;
  TH_LEGACY_HRESULT TVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar3;
  int iVar4;
  GraphicsScreenCoordinate GVar5;
  int iVar6;
  
  TVar1 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  if (TVar1 == 0) {
    SoftwareRenderer_ClearViewport(coordinate0,coordinate1,coordinate2,coordinate3);
    return;
  }
  if (TVar1 != 1) {
    SoftwareRenderer_ClearViewport(coordinate0,coordinate1,coordinate2,coordinate3);
    g_CurrentClearRect.y1 = coordinate2;
    g_CurrentClearRect.x1 = extraout_EAX;
    g_CurrentClearRect.x2 = extraout_ECX;
    g_CurrentClearRect.y2 = extraout_EDX;
    if ((((extraout_EAX != g_LastViewportRect.x1) || (coordinate2 != g_LastViewportRect.y1)) ||
        (extraout_ECX != g_LastViewportRect.x2)) || (extraout_EDX != g_LastViewportRect.y2)) {
      iVar3 = extraout_EDX;
      iVar4 = extraout_ECX;
      GVar5 = coordinate2;
      iVar6 = extraout_EAX;
      Memory_ZeroDwords(0x2c,&g_Direct3DViewportState);
      g_Direct3DViewportState.dwSize = 0x2c;
      g_Direct3DViewportState.dvMinZ = 0.0;
      g_Direct3DViewportState.dwY = coordinate2;
      g_Direct3DViewportState.dwWidth = extraout_ECX_00 - extraout_EAX_00;
      g_Direct3DViewportState.dwHeight = extraout_EDX_00 - coordinate2;
      g_Direct3DViewportState.dvMaxZ = 1.0;
      g_Direct3DViewportState.dvClipX = (float)(int)extraout_EAX_00;
      g_Direct3DViewportState.dvClipY = (float)coordinate2;
      g_Direct3DViewportState.dvClipWidth = (float)(int)g_Direct3DViewportState.dwWidth;
      g_Direct3DViewportState.dvClipHeight = (float)(int)g_Direct3DViewportState.dwHeight;
      g_Direct3DViewportState.dwX = extraout_EAX_00;
      TVar2 = (*g_Direct3DViewport2->lpVtbl->SetViewport2)
                        (g_Direct3DViewport2,&g_Direct3DViewportState);
      if (TVar2 == 0) {
        g_LastViewportRect.x1 = iVar6;
        g_LastViewportRect.y1 = GVar5;
        g_LastViewportRect.x2 = iVar4;
        g_LastViewportRect.y2 = iVar3;
      }
    }
    (*g_Direct3DViewport2->lpVtbl->Clear)(g_Direct3DViewport2,1,&g_CurrentClearRect,2);
    return;
  }
  Glide3_ClearViewport(coordinate0,coordinate1,coordinate2,coordinate3);
  return;
}

/* Address: 0x0057E6D0.
   Ownership: graphics/core/runtime.
   Purpose: Restores the back surface when lost and begins the Direct3D scene. Alternate backends are dispatched
   separately. ABI: CF clear means success. CF set means failure; EAX may contain an engine error code.
   Cross-module calls: GlideBackend_BeginSceneNoOp [graphics/backend/glide].
*/
void __cdecl Graphics_BeginScene(void)

{
  TH_LEGACY_DWORD TVar1;
  TH_LEGACY_HRESULT TVar2;
  int iVar3;
  bool bVar4;
  
  TVar1 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  bVar4 = TVar1 == 0;
  if (bVar4) {
    return;
  }
  if (TVar1 == 1) {
    GlideBackend_BeginSceneNoOp();
    if (!bVar4) {
      return;
    }
  }
  else {
    TVar2 = (*g_BackSurface3->lpVtbl->IsLost)(g_BackSurface3);
    iVar3 = 0;
    if (TVar2 != 0) {
      iVar3 = (*g_BackSurface3->lpVtbl->Restore)(g_BackSurface3);
    }
    if ((iVar3 == 0) &&
       (TVar2 = (*g_Direct3DDevice2->lpVtbl->BeginScene)(g_Direct3DDevice2), TVar2 == 0)) {
      return;
    }
  }
  return;
}

/* Address: 0x0057E750.
   Ownership: graphics/core/runtime.
   Purpose: Ends the current Direct3D scene. Alternate backends are dispatched separately.
   Cross-module calls: GlideBackend_EndSceneNoOp [graphics/backend/glide].
*/
void __cdecl Graphics_EndScene(void)

{
  TH_LEGACY_DWORD graphicsBackendId;
  
  graphicsBackendId = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  if (graphicsBackendId != 0) {
    if (graphicsBackendId == 1) {
      GlideBackend_EndSceneNoOp();
    }
    else {
      (*g_Direct3DDevice2->lpVtbl->EndScene)(g_Direct3DDevice2);
    }
  }
  return;
}

/* Address: 0x0057E7A0.
   Ownership: graphics/core/runtime.
   Purpose: Traverses the next-linked render list created by GraphicsPrimitiveQueue_RadixSortForRendering. Typed
   parameters: p0 coordinate0→GraphicsScreenCoordinate_V307, p1 coordinate1→GraphicsScreenCoordinate_V307, p2
   coordinate2→GraphicsScreenCoordinate_V307, p3 coordinate3→GraphicsScreenCoordinate_V307. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Cross-module calls: SoftwareRenderer_DrawPrimitiveQueueBridge [graphics/backend/software],
   GraphicsPrimitiveQueue_Begin [graphics/render/primitives], GraphicsPrimitiveQueue_Next
   [graphics/render/primitives], Glide3_DrawPrimitiveQueue [graphics/backend/glide].
*/
void Graphics_DrawPrimitiveQueue
               (GraphicsScreenCoordinate coordinate0,GraphicsScreenCoordinate coordinate1,
               GraphicsScreenCoordinate coordinate2,GraphicsScreenCoordinate coordinate3,
               GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *currentPacket;
  undefined1 uVar1;
  TH_LEGACY_DWORD graphicsBackendSelector;
  
  graphicsBackendSelector = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
  uVar1 = graphicsBackendSelector == 0;
  if ((bool)uVar1) {
    SoftwareRenderer_DrawPrimitiveQueueBridge(coordinate0,coordinate1,coordinate2,coordinate3,queue)
    ;
    return;
  }
  if (graphicsBackendSelector != 1) {
    currentPacket = GraphicsPrimitiveQueue_Begin(queue);
    while (!(bool)uVar1) {
      uVar1 = 0;
      (**(code **)((int)g_GraphicsDispatchTable.primitive +
                  ((currentPacket->renderFlags & 0x3f000) >> 10)))(currentPacket);
      (*g_Direct3DDevice2->lpVtbl->DrawPrimitive)
                (g_Direct3DDevice2,6,3,g_ImmediateTLVertices,g_ImmediateVertexCount,8);
      g_PrimitiveDrawCallCount = g_PrimitiveDrawCallCount + 1;
      currentPacket = GraphicsPrimitiveQueue_Next(queue);
    }
    return;
  }
  Glide3_DrawPrimitiveQueue(coordinate0,coordinate1,coordinate2,coordinate3,queue);
  return;
}

/* Address: 0x0057A330.
   Ownership: graphics/core/runtime.
   Purpose: Glide delegates to direct framebuffer composition.
   Local calls: GraphicsCursor_SaveSurfaceBackground, GraphicsCursor_RestoreSurfaceBackground.
   Cross-module calls: Glide3_Cursor_ComposeBeforePresent [graphics/backend/glide].
*/
void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface)

{
  UiPixelCoordinate cursorX;
  GraphicsScreenCoordinate drawX;
  GraphicsScreenCoordinate drawX_00;
  dword arg6;
  GraphicsCursorFrameRecord *cursorFrame;
  UiPixelCoordinate cursorY;
  int drawY;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 != 1) {
    g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
    if (-1 < g_CursorVisibilityToken) {
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
      GraphicsCursor_SaveSurfaceBackground(g_CursorSavedBackground,drawY,drawX,backSurface);
      arg6 = cursorFrame->activeSubresourceIndex;
      if ((g_CursorButtonState & 7) == 0) {
        arg6 = cursorFrame->idleSubresourceIndex;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,0,0,arg6,g_CursorSourceAsset,
                 g_CursorCompositeBuffer);
      GraphicsCursor_RestoreSurfaceBackground(g_CursorCompositeBuffer,drawY,drawX_00,backSurface);
    }
    return;
  }
  Glide3_Cursor_ComposeBeforePresent(backSurface);
  return;
}

/* Address: 0x0057A2C0.
   Ownership: graphics/core/runtime.
   Purpose: Restores the saved DirectDraw background after the cursor-containing back surface has been presented.
   Hidden cursors skip restoration. Glide delegates to a one-argument no-op because its cursor is drawn directly
   into the locked framebuffer.
   Local calls: GraphicsCursor_RestoreSurfaceBackground.
   Cross-module calls: Glide3_Cursor_RestoreAfterPresentNoOp [graphics/backend/glide].
*/
void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface)

{
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1) {
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
   Ownership: graphics/core/runtime.
   Purpose: Copies a clipped rectangle from a locked DirectDraw surface into the origin of a
   SoftwareFramebufferAccess scratch buffer. Supports two-byte and four-byte pixels. Negative draw coordinates
   advance the destination buffer and clip the copied dimensions. Typed parameters: p1
   drawY→GraphicsScreenCoordinate_V307, p2 drawX→GraphicsScreenCoordinate_V307. Calling convention, parameter
   storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void GraphicsCursor_SaveSurfaceBackground
               (SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
               GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface)

{
  int *piVar1;
  GraphicsPixelDimension GVar2;
  undefined *puVar3;
  int iVar4;
  TH_LEGACY_HRESULT TVar5;
  int iVar6;
  int iVar7;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  IDirectDrawSurface3 *pIVar12;
  IDirectDrawSurface3 *pIVar13;
  GraphicsPixelDimension GStack_24;
  GraphicsPixelDimension GStack_20;
  
  GStack_24 = destinationBuffer->height;
  GVar2 = destinationBuffer->width;
  GStack_20 = GVar2;
  if (destinationBuffer->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
    if (drawX < 0) {
      GStack_20 = GVar2 + drawX;
      drawX = 0;
    }
    if (drawY < 0) {
      GStack_24 = GStack_24 + drawY;
      drawY = 0;
    }
    iVar4 = (GStack_20 + drawX) - g_FramebufferWidth;
    if (iVar4 != 0 && (int)g_FramebufferWidth <= (int)(GStack_20 + drawX)) {
      GStack_20 = GStack_20 - iVar4;
    }
    iVar7 = (GStack_24 + drawY) - g_FramebufferHeight;
    if (iVar7 != 0 && (int)g_FramebufferHeight <= (int)(GStack_24 + drawY)) {
      GStack_24 = GStack_24 - iVar7;
    }
    if ((0 < (int)GStack_20) && (0 < (int)GStack_24)) {
      pIVar12 = sourceSurface;
      TVar5 = (*sourceSurface->lpVtbl->IsLost)(sourceSurface);
      iVar6 = 0;
      if (TVar5 != 0) {
        iVar6 = (*sourceSurface->lpVtbl->Restore)(sourceSurface);
      }
      if (iVar6 == 0) {
        Memory_ZeroDwords(0x6c,&g_GraphicsCursorSurfaceDescScratch);
        g_GraphicsCursorSurfaceDescScratch = 0x6c;
        iVar6 = (*sourceSurface->lpVtbl->Lock)
                          (sourceSurface,(TH_LEGACY_RECT *)0x0,
                           (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,0x11,
                           (TH_LEGACY_HANDLE)0x0);
      }
      puVar3 = g_GraphicsCursorSurfacePixels;
      if (iVar6 == 0) {
        puVar8 = (undefined2 *)
                 (g_GraphicsCursorSurfacePixels +
                 GVar2 * g_GraphicsCursorSurfacePitchBytes + drawY * 2);
        do {
          puVar9 = puVar8;
          pIVar13 = pIVar12;
          for (iVar6 = *(int *)(iVar7 + -4); iVar6 != 0; iVar6 = iVar6 + -1) {
            *(undefined2 *)&pIVar13->lpVtbl = *puVar9;
            puVar9 = puVar9 + 1;
            pIVar13 = (IDirectDrawSurface3 *)((int)&pIVar13->lpVtbl + 2);
          }
          pIVar12 = (IDirectDrawSurface3 *)((int)&pIVar12->lpVtbl + (int)&GStack_24 * 2);
          puVar8 = (undefined2 *)((int)puVar8 + g_GraphicsCursorSurfacePitchBytes);
          piVar1 = (int *)(iVar7 + -8);
          *piVar1 = *piVar1 + -1;
        } while (*piVar1 != 0);
        (**(code **)(**(int **)(iVar7 + 0x2c) + 0x80))(*(int **)(iVar7 + 0x2c),puVar3,iVar4);
      }
    }
  }
  else {
    if (drawX < 0) {
      GStack_20 = GVar2 + drawX;
      drawX = 0;
    }
    if (drawY < 0) {
      GStack_24 = GStack_24 + drawY;
      drawY = 0;
    }
    iVar4 = (GStack_20 + drawX) - g_FramebufferWidth;
    if (iVar4 != 0 && (int)g_FramebufferWidth <= (int)(GStack_20 + drawX)) {
      GStack_20 = GStack_20 - iVar4;
    }
    iVar7 = (GStack_24 + drawY) - g_FramebufferHeight;
    if (iVar7 != 0 && (int)g_FramebufferHeight <= (int)(GStack_24 + drawY)) {
      GStack_24 = GStack_24 - iVar7;
    }
    if ((0 < (int)GStack_20) && (0 < (int)GStack_24)) {
      pIVar12 = sourceSurface;
      TVar5 = (*sourceSurface->lpVtbl->IsLost)(sourceSurface);
      iVar6 = 0;
      if (TVar5 != 0) {
        iVar6 = (*sourceSurface->lpVtbl->Restore)(sourceSurface);
      }
      if (iVar6 == 0) {
        Memory_ZeroDwords(0x6c,&g_GraphicsCursorSurfaceDescScratch);
        g_GraphicsCursorSurfaceDescScratch = 0x6c;
        iVar6 = (*sourceSurface->lpVtbl->Lock)
                          (sourceSurface,(TH_LEGACY_RECT *)0x0,
                           (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,0x11,
                           (TH_LEGACY_HANDLE)0x0);
      }
      puVar3 = g_GraphicsCursorSurfacePixels;
      if (iVar6 == 0) {
        puVar10 = (undefined4 *)
                  (g_GraphicsCursorSurfacePixels +
                  GVar2 * g_GraphicsCursorSurfacePitchBytes + drawY * 4);
        do {
          puVar11 = puVar10;
          pIVar13 = pIVar12;
          for (iVar6 = *(int *)(iVar7 + -4); iVar6 != 0; iVar6 = iVar6 + -1) {
            pIVar13->lpVtbl = (IDirectDrawSurface3_Vtbl *)*puVar11;
            puVar11 = puVar11 + 1;
            pIVar13 = pIVar13 + 1;
          }
          pIVar12 = pIVar12 + (int)&GStack_24;
          puVar10 = (undefined4 *)((int)puVar10 + g_GraphicsCursorSurfacePitchBytes);
          piVar1 = (int *)(iVar7 + -8);
          *piVar1 = *piVar1 + -1;
        } while (*piVar1 != 0);
        (**(code **)(**(int **)(iVar7 + 0x2c) + 0x80))(*(int **)(iVar7 + 0x2c),puVar3,iVar4);
      }
    }
  }
  return;
}

/* Address: 0x0057A0C0.
   Ownership: graphics/core/runtime.
   Purpose: Copies a clipped SoftwareFramebufferAccess scratch buffer back into a DirectDraw surface at
   drawX/drawY. Supports two-byte and four-byte pixels and mirrors the save helper's clipping rules. Typed
   parameters: p1 drawY→GraphicsScreenCoordinate_V307, p2 drawX→GraphicsScreenCoordinate_V307. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void GraphicsCursor_RestoreSurfaceBackground
               (SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
               GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface)

{
  byte *pbVar1;
  GraphicsPixelDimension GVar2;
  undefined *puVar3;
  int iVar4;
  TH_LEGACY_HRESULT TVar5;
  int iVar6;
  undefined2 *puVar7;
  byte *pbVar8;
  undefined4 *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  undefined2 *puVar12;
  undefined4 *puVar13;
  undefined4 *puVar14;
  undefined4 *puVar15;
  GraphicsPixelDimension GStack_24;
  GraphicsPixelDimension GStack_20;
  
  GStack_24 = sourceBuffer->height;
  GVar2 = sourceBuffer->width;
  GStack_20 = GVar2;
  if (sourceBuffer->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
    pbVar8 = sourceBuffer->pixels;
    if (drawX < 0) {
      pbVar8 = pbVar8 + drawX * -2;
      GStack_20 = GVar2 + drawX;
      drawX = 0;
    }
    if (drawY < 0) {
      GStack_24 = GStack_24 + drawY;
      pbVar8 = pbVar8 + -drawY * GVar2 * 2;
      drawY = 0;
    }
    iVar4 = (GStack_20 + drawX) - g_FramebufferWidth;
    if (iVar4 != 0 && (int)g_FramebufferWidth <= (int)(GStack_20 + drawX)) {
      GStack_20 = GStack_20 - iVar4;
    }
    puVar10 = (undefined2 *)((GStack_24 + drawY) - g_FramebufferHeight);
    if (puVar10 != (undefined2 *)0x0 && (int)g_FramebufferHeight <= (int)(GStack_24 + drawY)) {
      GStack_24 = GStack_24 - (int)puVar10;
    }
    if ((0 < (int)GStack_20) && (0 < (int)GStack_24)) {
      TVar5 = (*destinationSurface->lpVtbl->IsLost)(destinationSurface);
      iVar6 = 0;
      if (TVar5 != 0) {
        iVar6 = (*destinationSurface->lpVtbl->Restore)(destinationSurface);
      }
      if (iVar6 == 0) {
        Memory_ZeroDwords(0x6c,&g_GraphicsCursorSurfaceDescScratch);
        g_GraphicsCursorSurfaceDescScratch = 0x6c;
        iVar6 = (*destinationSurface->lpVtbl->Lock)
                          (destinationSurface,(TH_LEGACY_RECT *)0x0,
                           (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,0x21,
                           (TH_LEGACY_HANDLE)0x0);
      }
      puVar3 = g_GraphicsCursorSurfacePixels;
      if (iVar6 == 0) {
        puVar11 = (undefined2 *)
                  (g_GraphicsCursorSurfacePixels +
                  GVar2 * g_GraphicsCursorSurfacePitchBytes + drawY * 2);
        do {
          puVar7 = puVar10;
          puVar12 = puVar11;
          for (iVar6 = *(int *)(pbVar8 + -4); iVar6 != 0; iVar6 = iVar6 + -1) {
            *puVar12 = *puVar7;
            puVar7 = puVar7 + 1;
            puVar12 = puVar12 + 1;
          }
          puVar11 = (undefined2 *)((int)puVar11 + g_GraphicsCursorSurfacePitchBytes);
          puVar10 = puVar10 + (int)&GStack_24;
          pbVar1 = pbVar8 + -8;
          *(int *)pbVar1 = *(int *)pbVar1 + -1;
        } while (*(int *)pbVar1 != 0);
        (**(code **)(**(int **)(pbVar8 + 0x2c) + 0x80))(*(int **)(pbVar8 + 0x2c),puVar3,iVar4);
      }
    }
  }
  else {
    pbVar8 = sourceBuffer->pixels;
    if (drawX < 0) {
      pbVar8 = pbVar8 + drawX * -4;
      GStack_20 = GVar2 + drawX;
      drawX = 0;
    }
    if (drawY < 0) {
      GStack_24 = GStack_24 + drawY;
      pbVar8 = pbVar8 + -drawY * GVar2 * 4;
      drawY = 0;
    }
    iVar4 = (GStack_20 + drawX) - g_FramebufferWidth;
    if (iVar4 != 0 && (int)g_FramebufferWidth <= (int)(GStack_20 + drawX)) {
      GStack_20 = GStack_20 - iVar4;
    }
    puVar13 = (undefined4 *)((GStack_24 + drawY) - g_FramebufferHeight);
    if (puVar13 != (undefined4 *)0x0 && (int)g_FramebufferHeight <= (int)(GStack_24 + drawY)) {
      GStack_24 = GStack_24 - (int)puVar13;
    }
    if ((0 < (int)GStack_20) && (0 < (int)GStack_24)) {
      TVar5 = (*destinationSurface->lpVtbl->IsLost)(destinationSurface);
      iVar6 = 0;
      if (TVar5 != 0) {
        iVar6 = (*destinationSurface->lpVtbl->Restore)(destinationSurface);
      }
      if (iVar6 == 0) {
        Memory_ZeroDwords(0x6c,&g_GraphicsCursorSurfaceDescScratch);
        g_GraphicsCursorSurfaceDescScratch = 0x6c;
        iVar6 = (*destinationSurface->lpVtbl->Lock)
                          (destinationSurface,(TH_LEGACY_RECT *)0x0,
                           (DDSURFACEDESC_DX6 *)&g_GraphicsCursorSurfaceDescScratch,0x21,
                           (TH_LEGACY_HANDLE)0x0);
      }
      puVar3 = g_GraphicsCursorSurfacePixels;
      if (iVar6 == 0) {
        puVar14 = (undefined4 *)
                  (g_GraphicsCursorSurfacePixels +
                  GVar2 * g_GraphicsCursorSurfacePitchBytes + drawY * 4);
        do {
          puVar9 = puVar13;
          puVar15 = puVar14;
          for (iVar6 = *(int *)(pbVar8 + -4); iVar6 != 0; iVar6 = iVar6 + -1) {
            *puVar15 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar15 = puVar15 + 1;
          }
          puVar14 = (undefined4 *)((int)puVar14 + g_GraphicsCursorSurfacePitchBytes);
          puVar13 = puVar13 + (int)&GStack_24;
          pbVar1 = pbVar8 + -8;
          *(int *)pbVar1 = *(int *)pbVar1 + -1;
        } while (*(int *)pbVar1 != 0);
        (**(code **)(**(int **)(pbVar8 + 0x2c) + 0x80))(*(int **)(pbVar8 + 0x2c),puVar3,iVar4);
      }
    }
  }
  return;
}
