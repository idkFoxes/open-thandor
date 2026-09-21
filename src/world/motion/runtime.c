/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/motion/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

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
dword __thandor_eax_preserve_ecx_edx
WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime)

{
  dword edgeScrollStep;
  dword dVar1;
  dword dVar2;
  dword screenDeltaY;
  
  edgeScrollStep = PersistentSettings_ReadDword(0x20,0x48);
  dVar2 = 0;
  if (g_CursorOverflowRight != 0) {
    dVar2 = edgeScrollStep;
  }
  dVar1 = 0;
  if (g_CursorOverflowBottom != 0) {
    dVar1 = edgeScrollStep;
  }
  screenDeltaY = dVar2 - g_CursorOverflowLeft;
  if ((int)(dVar2 - g_CursorOverflowLeft) < 0) {
    screenDeltaY = -edgeScrollStep;
  }
  dVar2 = dVar1 - g_CursorOverflowTop;
  if ((int)(dVar1 - g_CursorOverflowTop) < 0) {
    dVar2 = -edgeScrollStep;
  }
  WorldRuntime_TranslateCameraByScreenDelta(dVar2,screenDeltaY,worldRuntime);
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  dVar1 = 0x2f;
  if ((screenDeltaY == 0) && (dVar2 == 0)) {
    dVar1 = 0;
  }
  else if (screenDeltaY == 0) {
    if (-1 < (int)dVar2) {
      dVar1 = 0x33;
    }
  }
  else if ((int)screenDeltaY < 0) {
    dVar1 = 0x35;
    if (dVar2 != 0) {
      if ((int)dVar2 < 0) {
        dVar1 = 0x36;
      }
      else {
        dVar1 = 0x34;
      }
    }
  }
  else {
    dVar1 = 0x31;
    if (dVar2 != 0) {
      if ((int)dVar2 < 0) {
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
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
          (AngleTurn32 elevationAngle,int screenDelta,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  FixedDirectionXyzRegs12 FVar2;
  
  FVar2 = FixedMath_DirectionFromAnglesScaledRegs
                    (elevationAngle,(worldRuntime->motion).headingAngle + 0xc000 & 0xffff,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + FVar2.eax;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 + FVar2.ecx;
  pQVar1 = &(worldRuntime->motion).positionZQ12;
  *pQVar1 = *pQVar1 + FVar2.edx;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 + FVar2.eax;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 + FVar2.ecx;
  pQVar1 = &(worldRuntime->motion).targetPositionZQ12;
  *pQVar1 = *pQVar1 + FVar2.edx;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C850.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion translate current and target by pitch quarter turn.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn
          (int screenDelta,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  AngleTurn32 azimuthAngle;
  AngleTurn32 elevationAngle;
  FixedDirectionXyzRegs12 FVar2;
  
  azimuthAngle = (worldRuntime->motion).headingAngle;
  elevationAngle = (worldRuntime->motion).pitchAngle - 0x4000;
  if ((int)elevationAngle < -0x4000) {
    elevationAngle = -(worldRuntime->motion).pitchAngle - 0x4000;
    azimuthAngle = azimuthAngle + 0x8000 & 0xffff;
  }
  FVar2 = FixedMath_DirectionFromAnglesScaledRegs
                    (elevationAngle,azimuthAngle,screenDelta * _k_CameraScreenDeltaDistanceScaleQ16)
  ;
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + FVar2.eax;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 + FVar2.ecx;
  pQVar1 = &(worldRuntime->motion).positionZQ12;
  *pQVar1 = *pQVar1 + FVar2.edx;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 + FVar2.eax;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 + FVar2.ecx;
  pQVar1 = &(worldRuntime->motion).targetPositionZQ12;
  *pQVar1 = *pQVar1 + FVar2.edx;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C8C0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion translate current and target by negated pitch reverse heading.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
          (int screenDelta,WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  FixedDirectionXyzRegs12 FVar2;
  
  FVar2 = FixedMath_DirectionFromAnglesScaledRegs
                    (-(worldRuntime->motion).pitchAngle,
                     (worldRuntime->motion).headingAngle + 0x8000 & 0xffff,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + FVar2.eax;
  pQVar1 = &(worldRuntime->motion).positionYQ12;
  *pQVar1 = *pQVar1 + FVar2.ecx;
  pQVar1 = &(worldRuntime->motion).positionZQ12;
  *pQVar1 = *pQVar1 + FVar2.edx;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 + FVar2.eax;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 + FVar2.ecx;
  pQVar1 = &(worldRuntime->motion).targetPositionZQ12;
  *pQVar1 = *pQVar1 + FVar2.edx;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C920.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust heading and recompute position.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustHeadingAndRecomputePosition
          (int headingDeltaInput,WorldRuntimeContext *worldRuntime)

{
  uint azimuthAngle;
  FixedDirectionXyzRegs12 FVar1;
  
  azimuthAngle = (worldRuntime->motion).headingAngle +
                 headingDeltaInput * g_WorldMotionHeadingInputScale & 0xffff;
  (worldRuntime->motion).headingAngle = azimuthAngle;
  FVar1 = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,azimuthAngle,
                     (worldRuntime->motion).targetDistanceQ12);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - FVar1.eax;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - FVar1.ecx;
  (worldRuntime->motion).positionZQ12 = (worldRuntime->motion).targetPositionZQ12 - FVar1.edx;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C990.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust heading and clear field grid dirty.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx
WorldMotion_AdjustHeadingAndClearFieldGridDirty
          (int headingDeltaInput,WorldRuntimeContext *worldRuntime)

{
  (worldRuntime->motion).headingAngle =
       (worldRuntime->motion).headingAngle - headingDeltaInput * g_WorldMotionHeadingInputScale &
       0xffff;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C9C0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust distance clamp and recompute position.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustDistanceClampAndRecomputePosition
          (int distanceDeltaInput,WorldRuntimeContext *worldRuntime)

{
  UQ12 UVar1;
  UQ12 UVar2;
  FixedDirectionXyzRegs12 FVar3;
  
  UVar1 = distanceDeltaInput * g_WorldMotionDistanceInputScaleQ12 +
          (worldRuntime->motion).committedDistanceQ12;
  UVar2 = UVar1;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)UVar1) {
      UVar2 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)UVar1 < (int)worldRuntime->minimumCameraDistanceQ12) {
      UVar2 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (UVar2 = g_WorldMotionAlternateMaximumDistanceQ12,
           (int)UVar1 <= (int)g_WorldMotionAlternateMaximumDistanceQ12)) &&
          (UVar2 = UVar1, (int)UVar1 < (int)g_WorldMotionAlternateMinimumDistanceQ12)) {
    UVar2 = g_WorldMotionAlternateMinimumDistanceQ12;
  }
  if ((int)UVar2 < 0x400) {
    UVar2 = 0x400;
  }
  (worldRuntime->motion).targetDistanceQ12 = UVar2;
  (worldRuntime->motion).committedDistanceQ12 = UVar2;
  FVar3 = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,UVar2);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - FVar3.eax;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - FVar3.ecx;
  (worldRuntime->motion).positionZQ12 = (worldRuntime->motion).targetPositionZQ12 - FVar3.edx;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050CA80.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust position magnitude clamp.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx
WorldMotion_AdjustPositionMagnitudeClamp(int magnitudeDeltaInput,WorldRuntimeContext *worldRuntime)

{
  UQ12 UVar1;
  UQ12 UVar2;
  
  UVar1 = (worldRuntime->motion).positionMagnitudeQ12 -
          magnitudeDeltaInput * g_WorldMotionPositionMagnitudeInputScaleQ12;
  UVar2 = UVar1;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)UVar1) {
      UVar2 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)UVar1 < (int)worldRuntime->minimumCameraDistanceQ12) {
      UVar2 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (UVar2 = g_WorldMotionAlternateMaximumDistanceQ12,
           (int)UVar1 <= (int)g_WorldMotionAlternateMaximumDistanceQ12)) &&
          (UVar2 = UVar1, (int)UVar1 < (int)g_WorldMotionAlternateMinimumDistanceQ12)) {
    UVar2 = g_WorldMotionAlternateMinimumDistanceQ12;
  }
  if ((int)UVar2 < 0x400) {
    UVar2 = 0x400;
  }
  (worldRuntime->motion).positionMagnitudeQ12 = UVar2;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050CB10.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust pitch clamp and recompute position.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustPitchClampAndRecomputePosition
          (int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  FixedDirectionXyzRegs12 FVar3;
  
  AVar1 = (worldRuntime->motion).pitchAngle + pitchDeltaInput * g_WorldMotionPitchInputScale;
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
           (AVar2 = g_WorldMotionAlternateMaximumPitchAngle,
           (int)AVar1 <= (int)g_WorldMotionAlternateMaximumPitchAngle)) &&
          (AVar2 = AVar1, (int)AVar1 < (int)g_WorldMotionAlternateMinimumPitchAngle)) {
    AVar2 = g_WorldMotionAlternateMinimumPitchAngle;
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
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - FVar3.eax;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - FVar3.ecx;
  (worldRuntime->motion).positionZQ12 = (worldRuntime->motion).targetPositionZQ12 - FVar3.edx;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050CBE0.
   Ownership: world/motion/runtime.
   Purpose: Handles world motion adjust pitch clamp and clear field grid dirty.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustPitchClampAndClearFieldGridDirty
          (int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  
  AVar1 = (worldRuntime->motion).pitchAngle - pitchDeltaInput * g_WorldMotionPitchInputScale;
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
           (AVar2 = g_WorldMotionAlternateMaximumPitchAngle,
           (int)AVar1 <= (int)g_WorldMotionAlternateMaximumPitchAngle)) &&
          (AVar2 = AVar1, (int)AVar1 < (int)g_WorldMotionAlternateMinimumPitchAngle)) {
    AVar2 = g_WorldMotionAlternateMinimumPitchAngle;
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
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_TranslateCameraByScreenDelta
          (CameraScreenDeltaPixels screenDeltaX,dword screenDeltaY,WorldRuntimeContext *worldRuntime
          )

{
  Q12 *pQVar1;
  AngleTurn32 angle;
  int iVar2;
  FixedSinCosEdxEax8 movementDeltaXYQ12;
  FixedSinCosEdxEax8 FVar3;
  Q12 *motionCoordinateField;
  
  iVar2 = (int)(_k_CameraScreenDeltaDistanceScaleQ16 * (worldRuntime->motion).targetDistanceQ12) >>
          0x10;
  angle = (worldRuntime->motion).headingAngle;
  movementDeltaXYQ12 = FixedMath_SinCosScaled(angle,screenDeltaX * iVar2);
  movementDeltaXYQ12._4_4_ = (int)(movementDeltaXYQ12 >> 0x20);
  (worldRuntime->motion).positionXQ12 =
       (worldRuntime->motion).positionXQ12 - (int)movementDeltaXYQ12;
  motionCoordinateField = &(worldRuntime->motion).positionYQ12;
  *motionCoordinateField = *motionCoordinateField - movementDeltaXYQ12._4_4_;
  pQVar1 = &(worldRuntime->motion).targetPositionXQ12;
  *pQVar1 = *pQVar1 - (int)movementDeltaXYQ12;
  pQVar1 = &(worldRuntime->motion).targetPositionYQ12;
  *pQVar1 = *pQVar1 - movementDeltaXYQ12._4_4_;
  FVar3 = FixedMath_SinCosScaled(angle + 0x4000 & 0xffff,screenDeltaY * iVar2);
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

