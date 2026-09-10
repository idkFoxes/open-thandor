#include <thandor/world/motion/runtime.h>

/* Implementation ownership: world/motion/runtime. */

/* Address: 0x0050D050.
   Ownership: world/motion/runtime.
   Purpose: Reads the edge-scroll settings and pointer-edge state, applies the corresponding camera translation,
   refreshes the view, and returns the directional cursor frame or zero.
   Local calls: WorldRuntime_TranslateCameraByScreenDelta.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent],
   WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface [world/runtime/core].
*/
dword WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime)

{
  dword edgeScrollStep;
  dword dVar1;
  dword screenDeltaY;
  int extraout_ECX;
  dword dVar2;
  int extraout_EDX;
  
  edgeScrollStep = PersistentSettings_ReadDword(0x20,0x48);
  dVar1 = 0;
  if (g_CursorOverflowRight != 0) {
    dVar1 = edgeScrollStep;
  }
  dVar2 = 0;
  if (g_CursorOverflowBottom != 0) {
    dVar2 = edgeScrollStep;
  }
  screenDeltaY = dVar1 - g_CursorOverflowLeft;
  if ((int)(dVar1 - g_CursorOverflowLeft) < 0) {
    screenDeltaY = -edgeScrollStep;
  }
  dVar1 = dVar2 - g_CursorOverflowTop;
  if ((int)(dVar2 - g_CursorOverflowTop) < 0) {
    dVar1 = -edgeScrollStep;
  }
  WorldRuntime_TranslateCameraByScreenDelta(dVar1,screenDeltaY,worldRuntime);
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  dVar1 = 0x2f;
  if ((extraout_ECX == 0) && (extraout_EDX == 0)) {
    dVar1 = 0;
  }
  else if (extraout_ECX == 0) {
    if (-1 < extraout_EDX) {
      dVar1 = 0x33;
    }
  }
  else if (extraout_ECX < 0) {
    dVar1 = 0x35;
    if (extraout_EDX != 0) {
      if (extraout_EDX < 0) {
        dVar1 = 0x36;
      }
      else {
        dVar1 = 0x34;
      }
    }
  }
  else {
    dVar1 = 0x31;
    if (extraout_EDX != 0) {
      if (extraout_EDX < 0) {
        dVar1 = 0x30;
      }
      else {
        dVar1 = 0x32;
      }
    }
  }
  return dVar1;
}

/* Address: 0x0050C7F0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion translate current and target by input elevation and heading quarter turn.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __fastcall
WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
          (undefined4 param_1,AngleTurn32 elevationAngle,int screenDelta,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  int extraout_ECX;
  int iVar2;
  FixedDirectionXZEdxEax8 FVar3;
  
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    (elevationAngle,(worldRuntime->motion).headingAngle + 0xc000 & 0xffff,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  iVar2 = (int)(FVar3 >> 0x20);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + (int)FVar3;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 + extraout_ECX;
  pQVar1 = &(worldRuntime->motion).positionZQ12;
  *pQVar1 = *pQVar1 + iVar2;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 + (int)FVar3;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 + extraout_ECX;
  pQVar1 = &(worldRuntime->motion).targetPositionZQ12;
  *pQVar1 = *pQVar1 + iVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050C850.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion translate current and target by pitch quarter turn.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn
               (int screenDelta,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  AngleTurn32 azimuthAngle;
  int extraout_ECX;
  AngleTurn32 elevationAngle;
  int iVar2;
  FixedDirectionXZEdxEax8 FVar3;
  
  azimuthAngle = (worldRuntime->motion).headingAngle;
  elevationAngle = (worldRuntime->motion).pitchAngle - 0x4000;
  if ((int)elevationAngle < -0x4000) {
    elevationAngle = -(worldRuntime->motion).pitchAngle - 0x4000;
    azimuthAngle = azimuthAngle + 0x8000 & 0xffff;
  }
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    (elevationAngle,azimuthAngle,screenDelta * _k_CameraScreenDeltaDistanceScaleQ16)
  ;
  iVar2 = (int)(FVar3 >> 0x20);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + (int)FVar3;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 + extraout_ECX;
  pQVar1 = &(worldRuntime->motion).positionZQ12;
  *pQVar1 = *pQVar1 + iVar2;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 + (int)FVar3;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 + extraout_ECX;
  pQVar1 = &(worldRuntime->motion).targetPositionZQ12;
  *pQVar1 = *pQVar1 + iVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050C8C0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion translate current and target by negated pitch reverse heading.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
               (int screenDelta,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  int extraout_ECX;
  int iVar2;
  FixedDirectionXZEdxEax8 FVar3;
  
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    (-(worldRuntime->motion).pitchAngle,
                     (worldRuntime->motion).headingAngle + 0x8000 & 0xffff,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  iVar2 = (int)(FVar3 >> 0x20);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + (int)FVar3;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 + extraout_ECX;
  pQVar1 = &(worldRuntime->motion).positionZQ12;
  *pQVar1 = *pQVar1 + iVar2;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 + (int)FVar3;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 + extraout_ECX;
  pQVar1 = &(worldRuntime->motion).targetPositionZQ12;
  *pQVar1 = *pQVar1 + iVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050C920.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust heading and recompute position.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_AdjustHeadingAndRecomputePosition
               (int headingDeltaInput,WorldRuntimeContext *worldRuntime)

{
  uint azimuthAngle;
  int extraout_ECX;
  FixedDirectionXZEdxEax8 FVar1;
  
  azimuthAngle = (worldRuntime->motion).headingAngle + headingDeltaInput * DAT_0050baee & 0xffff;
  (worldRuntime->motion).headingAngle = azimuthAngle;
  FVar1 = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,azimuthAngle,
                     (worldRuntime->motion).targetDistanceQ12);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - (int)FVar1;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - extraout_ECX;
  (worldRuntime->motion).positionZQ12 =
       (worldRuntime->motion).targetPositionZQ12 - (int)(FVar1 >> 0x20);
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050C990.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust heading and clear field grid dirty.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
undefined4 WorldMotion_AdjustHeadingAndClearFieldGridDirty(int param_1,WorldRuntimeContext *param_2)

{
  undefined4 in_EAX;
  
  (param_2->motion).headingAngle = (param_2->motion).headingAngle - param_1 * DAT_0050baee & 0xffff;
  WorldRuntime_ClearFieldGridDirtyFlag(param_2);
  return in_EAX;
}

/* Address: 0x0050C9C0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust distance clamp and recompute position.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_AdjustDistanceClampAndRecomputePosition(int param_1,WorldRuntimeContext *param_2)

{
  WorldSurfaceSelectionFlags WVar1;
  WorldSurfaceSelectionFlags WVar2;
  int extraout_ECX;
  FixedDirectionXZEdxEax8 FVar3;
  
  WVar1 = param_1 * DAT_0050baea + (param_2->motion).committedDistanceQ12;
  WVar2 = WVar1;
  if ((param_2->runtimeFlags & 0x40000) == 0) {
    if ((int)param_2->surfaceSelectionFlags < (int)WVar1) {
      WVar2 = param_2->surfaceSelectionFlags;
    }
    else if ((int)WVar1 < (int)param_2->reserved98) {
      WVar2 = param_2->reserved98;
    }
  }
  else if ((((param_2->runtimeFlags & 0x200) != 0) &&
           (WVar2 = DAT_0050bb06, (int)WVar1 <= (int)DAT_0050bb06)) &&
          (WVar2 = WVar1, (int)WVar1 < (int)DAT_0050bb02)) {
    WVar2 = DAT_0050bb02;
  }
  if ((int)WVar2 < 0x400) {
    WVar2 = 0x400;
  }
  (param_2->motion).targetDistanceQ12 = WVar2;
  (param_2->motion).committedDistanceQ12 = WVar2;
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    ((param_2->motion).pitchAngle,(param_2->motion).headingAngle,WVar2);
  (param_2->motion).positionXQ12 = (param_2->motion).targetPositionXQ12 - (int)FVar3;
  (param_2->motion).positionYQ12 = (param_2->motion).targetPositionYQ12 - extraout_ECX;
  (param_2->motion).positionZQ12 = (param_2->motion).targetPositionZQ12 - (int)(FVar3 >> 0x20);
  WorldRuntime_ClearFieldGridDirtyFlag(param_2);
  return;
}

/* Address: 0x0050CA80.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust position magnitude clamp.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_AdjustPositionMagnitudeClamp
               (int magnitudeDeltaInput,WorldRuntimeContext *worldRuntime)

{
  WorldSurfaceSelectionFlags WVar1;
  WorldSurfaceSelectionFlags WVar2;
  
  WVar1 = (worldRuntime->motion).positionMagnitudeQ12 - magnitudeDeltaInput * DAT_0050bae2;
  WVar2 = WVar1;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)worldRuntime->surfaceSelectionFlags < (int)WVar1) {
      WVar2 = worldRuntime->surfaceSelectionFlags;
    }
    else if ((int)WVar1 < (int)worldRuntime->reserved98) {
      WVar2 = worldRuntime->reserved98;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (WVar2 = DAT_0050bb06, (int)WVar1 <= (int)DAT_0050bb06)) &&
          (WVar2 = WVar1, (int)WVar1 < (int)DAT_0050bb02)) {
    WVar2 = DAT_0050bb02;
  }
  if ((int)WVar2 < 0x400) {
    WVar2 = 0x400;
  }
  (worldRuntime->motion).positionMagnitudeQ12 = WVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050CB10.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust pitch clamp and recompute position.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_AdjustPitchClampAndRecomputePosition
               (int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  int extraout_ECX;
  FixedDirectionXZEdxEax8 FVar3;
  
  AVar1 = (worldRuntime->motion).pitchAngle + pitchDeltaInput * DAT_0050baf2;
  AVar2 = AVar1;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)(worldRuntime->motion).maximumPitchAngle < (int)AVar1) {
      AVar2 = (worldRuntime->motion).maximumPitchAngle;
    }
    else if ((int)AVar1 < (int)(worldRuntime->motion).minimumPitchAngle) {
      AVar2 = (worldRuntime->motion).minimumPitchAngle;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (AVar2 = DAT_0050bafe, (int)AVar1 <= (int)DAT_0050bafe)) &&
          (AVar2 = AVar1, (int)AVar1 < (int)DAT_0050bafa)) {
    AVar2 = DAT_0050bafa;
  }
  if ((int)AVar2 < 0x4001) {
    if ((int)AVar2 < -0x4000) {
      AVar2 = 0xffffc000;
    }
  }
  else {
    AVar2 = 0x4000;
  }
  (worldRuntime->motion).pitchAngle = AVar2;
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    (AVar2,(worldRuntime->motion).headingAngle,
                     (worldRuntime->motion).targetDistanceQ12);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - (int)FVar3;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - extraout_ECX;
  (worldRuntime->motion).positionZQ12 =
       (worldRuntime->motion).targetPositionZQ12 - (int)(FVar3 >> 0x20);
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050CBE0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust pitch clamp and clear field grid dirty.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void WorldMotion_AdjustPitchClampAndClearFieldGridDirty
               (int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  
  AVar1 = (worldRuntime->motion).pitchAngle - pitchDeltaInput * DAT_0050baf2;
  AVar2 = AVar1;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)(worldRuntime->motion).maximumPitchAngle < (int)AVar1) {
      AVar2 = (worldRuntime->motion).maximumPitchAngle;
    }
    else if ((int)AVar1 < (int)(worldRuntime->motion).minimumPitchAngle) {
      AVar2 = (worldRuntime->motion).minimumPitchAngle;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (AVar2 = DAT_0050bafe, (int)AVar1 <= (int)DAT_0050bafe)) &&
          (AVar2 = AVar1, (int)AVar1 < (int)DAT_0050bafa)) {
    AVar2 = DAT_0050bafa;
  }
  if ((int)AVar2 < 0x4001) {
    if ((int)AVar2 < -0x4000) {
      AVar2 = 0xffffc000;
    }
  }
  else {
    AVar2 = 0x4000;
  }
  (worldRuntime->motion).pitchAngle = AVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050C770.
   Ownership: world/motion/runtime.
   Purpose: Transforms a screen-space delta through the current camera angle and scale, updates both camera
   coordinate pairs, and refreshes the world view. Typed parameters: p0 screenDeltaX→CameraScreenDeltaPixels_V344.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], WorldRuntime_ClearFieldGridDirtyFlag
   [world/runtime/core].
*/
void WorldRuntime_TranslateCameraByScreenDelta
               (CameraScreenDeltaPixels screenDeltaX,dword screenDeltaY,
               WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  AngleTurn32 angle;
  FixedMathScale32 scale;
  int iVar2;
  FixedSinCosEdxEax8 movementDeltaXYQ12;
  FixedSinCosEdxEax8 FVar3;
  Q12 *motionCoordinateField;
  
  angle = (worldRuntime->motion).headingAngle;
  movementDeltaXYQ12 =
       FixedMath_SinCosScaled
                 (angle,screenDeltaX *
                        ((int)(_k_CameraScreenDeltaDistanceScaleQ16 *
                              (worldRuntime->motion).targetDistanceQ12) >> 0x10));
  movementDeltaXYQ12._4_4_ = (int)(movementDeltaXYQ12 >> 0x20);
  (worldRuntime->motion).positionXQ12 =
       (worldRuntime->motion).positionXQ12 - (int)movementDeltaXYQ12;
  motionCoordinateField = &(worldRuntime->motion).positionYQ12;
  *motionCoordinateField = *motionCoordinateField - movementDeltaXYQ12._4_4_;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 - (int)movementDeltaXYQ12;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 - movementDeltaXYQ12._4_4_;
  FVar3 = FixedMath_SinCosScaled(angle + 0x4000 & 0xffff,scale);
  iVar2 = (int)(FVar3 >> 0x20);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 - (int)FVar3;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 - iVar2;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 - (int)FVar3;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 - iVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}
