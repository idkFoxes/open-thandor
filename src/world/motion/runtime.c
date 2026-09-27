/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/motion/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/motion/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/motion/runtime. */

/* Address: 0x0050D050.
   Ownership: world/motion/runtime.
   Purpose: Reads the edge-scroll settings and pointer-edge state, applies the corresponding camera translation,
   refreshes the view, and returns the directional cursor frame or zero.
   Local calls: WorldRuntime_TranslateCameraByScreenDelta.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent],
   WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface [world/runtime/core].
*/
uint32_t __thandor_eax_preserve_ecx_edx
WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime)

{
  uint32_t edgeScrollStep;
  uint32_t bottomStepOrCursorFrame;
  uint32_t rightStepOrScreenDeltaX;
  uint32_t screenDeltaY;
  
  edgeScrollStep = PersistentSettings_ReadDword(0x20,0x48);
  rightStepOrScreenDeltaX = 0;
  if (g_CursorOverflowRight != 0) {
    rightStepOrScreenDeltaX = edgeScrollStep;
  }
  bottomStepOrCursorFrame = 0;
  if (g_CursorOverflowBottom != 0) {
    bottomStepOrCursorFrame = edgeScrollStep;
  }
  screenDeltaY = rightStepOrScreenDeltaX - g_CursorOverflowLeft;
  if ((int)(rightStepOrScreenDeltaX - g_CursorOverflowLeft) < 0) {
    screenDeltaY = -edgeScrollStep;
  }
  rightStepOrScreenDeltaX = bottomStepOrCursorFrame - g_CursorOverflowTop;
  if ((int)(bottomStepOrCursorFrame - g_CursorOverflowTop) < 0) {
    rightStepOrScreenDeltaX = -edgeScrollStep;
  }
  WorldRuntime_TranslateCameraByScreenDelta(rightStepOrScreenDeltaX,screenDeltaY,worldRuntime);
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  bottomStepOrCursorFrame = 0x2f;
  if ((screenDeltaY == 0) && (rightStepOrScreenDeltaX == 0)) {
    bottomStepOrCursorFrame = 0;
  }
  else if (screenDeltaY == 0) {
    if (-1 < (int)rightStepOrScreenDeltaX) {
      bottomStepOrCursorFrame = 0x33;
    }
  }
  else if ((int)screenDeltaY < 0) {
    bottomStepOrCursorFrame = 0x35;
    if (rightStepOrScreenDeltaX != 0) {
      if ((int)rightStepOrScreenDeltaX < 0) {
        bottomStepOrCursorFrame = 0x36;
      }
      else {
        bottomStepOrCursorFrame = 0x34;
      }
    }
  }
  else {
    bottomStepOrCursorFrame = 0x31;
    if (rightStepOrScreenDeltaX != 0) {
      if ((int)rightStepOrScreenDeltaX < 0) {
        bottomStepOrCursorFrame = 0x30;
      }
      else {
        bottomStepOrCursorFrame = 0x32;
      }
    }
  }
  return bottomStepOrCursorFrame;
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
  Q12 *coordinateField;
  FixedDirectionXyzRegs12 translationDelta;
  
  translationDelta = FixedMath_DirectionFromAnglesScaledRegs
                    (elevationAngle,(worldRuntime->motion).headingAngle + 0xc000 & 0xffff,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + translationDelta.eax;
  coordinateField = &(worldRuntime->motion).positionYQ12;
  *coordinateField = *coordinateField + translationDelta.ecx;
  coordinateField = &(worldRuntime->motion).positionZQ12;
  *coordinateField = *coordinateField + translationDelta.edx;
  coordinateField = &(worldRuntime->motion).targetPositionXQ12;
  *coordinateField = *coordinateField + translationDelta.eax;
  coordinateField = &(worldRuntime->motion).targetPositionYQ12;
  *coordinateField = *coordinateField + translationDelta.ecx;
  coordinateField = &(worldRuntime->motion).targetPositionZQ12;
  *coordinateField = *coordinateField + translationDelta.edx;
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
  Q12 *coordinateField;
  AngleTurn32 azimuthAngle;
  AngleTurn32 elevationAngle;
  FixedDirectionXyzRegs12 translationDelta;
  
  azimuthAngle = (worldRuntime->motion).headingAngle;
  elevationAngle = (worldRuntime->motion).pitchAngle - 0x4000;
  if ((int)elevationAngle < -0x4000) {
    elevationAngle = -(worldRuntime->motion).pitchAngle - 0x4000;
    azimuthAngle = azimuthAngle + 0x8000 & 0xffff;
  }
  translationDelta = FixedMath_DirectionFromAnglesScaledRegs
                    (elevationAngle,azimuthAngle,screenDelta * _k_CameraScreenDeltaDistanceScaleQ16)
  ;
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + translationDelta.eax;
  coordinateField = &(worldRuntime->motion).positionYQ12;
  *coordinateField = *coordinateField + translationDelta.ecx;
  coordinateField = &(worldRuntime->motion).positionZQ12;
  *coordinateField = *coordinateField + translationDelta.edx;
  coordinateField = &(worldRuntime->motion).targetPositionXQ12;
  *coordinateField = *coordinateField + translationDelta.eax;
  coordinateField = &(worldRuntime->motion).targetPositionYQ12;
  *coordinateField = *coordinateField + translationDelta.ecx;
  coordinateField = &(worldRuntime->motion).targetPositionZQ12;
  *coordinateField = *coordinateField + translationDelta.edx;
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
  Q12 *coordinateField;
  FixedDirectionXyzRegs12 translationDelta;
  
  translationDelta = FixedMath_DirectionFromAnglesScaledRegs
                    (-(worldRuntime->motion).pitchAngle,
                     (worldRuntime->motion).headingAngle + 0x8000 & 0xffff,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 + translationDelta.eax;
  coordinateField = &(worldRuntime->motion).positionYQ12;
  *coordinateField = *coordinateField + translationDelta.ecx;
  coordinateField = &(worldRuntime->motion).positionZQ12;
  *coordinateField = *coordinateField + translationDelta.edx;
  coordinateField = &(worldRuntime->motion).targetPositionXQ12;
  *coordinateField = *coordinateField + translationDelta.eax;
  coordinateField = &(worldRuntime->motion).targetPositionYQ12;
  *coordinateField = *coordinateField + translationDelta.ecx;
  coordinateField = &(worldRuntime->motion).targetPositionZQ12;
  *coordinateField = *coordinateField + translationDelta.edx;
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
  uint32_t azimuthAngle;
  FixedDirectionXyzRegs12 cameraOffset;
  
  azimuthAngle = (worldRuntime->motion).headingAngle +
                 headingDeltaInput * g_WorldMotionHeadingInputScale & 0xffff;
  (worldRuntime->motion).headingAngle = azimuthAngle;
  cameraOffset = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,azimuthAngle,
                     (worldRuntime->motion).targetDistanceQ12);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - cameraOffset.eax;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - cameraOffset.ecx;
  (worldRuntime->motion).positionZQ12 = (worldRuntime->motion).targetPositionZQ12 - cameraOffset.edx;
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
  UQ12 requestedDistanceQ12;
  UQ12 clampedDistanceQ12;
  FixedDirectionXyzRegs12 cameraOffset;
  
  requestedDistanceQ12 = distanceDeltaInput * g_WorldMotionDistanceInputScaleQ12 +
          (worldRuntime->motion).committedDistanceQ12;
  clampedDistanceQ12 = requestedDistanceQ12;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)requestedDistanceQ12) {
      clampedDistanceQ12 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)requestedDistanceQ12 < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedDistanceQ12 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (clampedDistanceQ12 = g_WorldMotionAlternateMaximumDistanceQ12,
           (int)requestedDistanceQ12 <= (int)g_WorldMotionAlternateMaximumDistanceQ12)) &&
          (clampedDistanceQ12 = requestedDistanceQ12, (int)requestedDistanceQ12 < (int)g_WorldMotionAlternateMinimumDistanceQ12)) {
    clampedDistanceQ12 = g_WorldMotionAlternateMinimumDistanceQ12;
  }
  if ((int)clampedDistanceQ12 < 0x400) {
    clampedDistanceQ12 = 0x400;
  }
  (worldRuntime->motion).targetDistanceQ12 = clampedDistanceQ12;
  (worldRuntime->motion).committedDistanceQ12 = clampedDistanceQ12;
  cameraOffset = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,clampedDistanceQ12);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - cameraOffset.eax;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - cameraOffset.ecx;
  (worldRuntime->motion).positionZQ12 = (worldRuntime->motion).targetPositionZQ12 - cameraOffset.edx;
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
  UQ12 requestedMagnitudeQ12;
  UQ12 clampedMagnitudeQ12;
  
  requestedMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12 -
          magnitudeDeltaInput * g_WorldMotionPositionMagnitudeInputScaleQ12;
  clampedMagnitudeQ12 = requestedMagnitudeQ12;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)requestedMagnitudeQ12) {
      clampedMagnitudeQ12 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)requestedMagnitudeQ12 < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedMagnitudeQ12 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (clampedMagnitudeQ12 = g_WorldMotionAlternateMaximumDistanceQ12,
           (int)requestedMagnitudeQ12 <= (int)g_WorldMotionAlternateMaximumDistanceQ12)) &&
          (clampedMagnitudeQ12 = requestedMagnitudeQ12, (int)requestedMagnitudeQ12 < (int)g_WorldMotionAlternateMinimumDistanceQ12)) {
    clampedMagnitudeQ12 = g_WorldMotionAlternateMinimumDistanceQ12;
  }
  if ((int)clampedMagnitudeQ12 < 0x400) {
    clampedMagnitudeQ12 = 0x400;
  }
  (worldRuntime->motion).positionMagnitudeQ12 = clampedMagnitudeQ12;
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
  AngleTurn32 requestedPitchAngle;
  AngleTurn32 clampedPitchAngle;
  FixedDirectionXyzRegs12 cameraOffset;
  
  requestedPitchAngle = (worldRuntime->motion).pitchAngle + pitchDeltaInput * g_WorldMotionPitchInputScale;
  clampedPitchAngle = requestedPitchAngle;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)(worldRuntime->motion).maximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = (worldRuntime->motion).maximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)(worldRuntime->motion).minimumPitchAngle) {
      clampedPitchAngle = (worldRuntime->motion).minimumPitchAngle;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (clampedPitchAngle = g_WorldMotionAlternateMaximumPitchAngle,
           (int)requestedPitchAngle <= (int)g_WorldMotionAlternateMaximumPitchAngle)) &&
          (clampedPitchAngle = requestedPitchAngle, (int)requestedPitchAngle < (int)g_WorldMotionAlternateMinimumPitchAngle)) {
    clampedPitchAngle = g_WorldMotionAlternateMinimumPitchAngle;
  }
  if ((int)clampedPitchAngle < 0x4001) {
    if ((int)clampedPitchAngle < -0x4000) {
      clampedPitchAngle = 0xffffc000;
    }
  }
  else {
    clampedPitchAngle = 0x4000;
  }
  (worldRuntime->motion).pitchAngle = clampedPitchAngle;
  cameraOffset = FixedMath_DirectionFromAnglesScaledRegs
                    (clampedPitchAngle,(worldRuntime->motion).headingAngle,
                     (worldRuntime->motion).targetDistanceQ12);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).targetPositionXQ12 - cameraOffset.eax;
  (worldRuntime->motion).positionYQ12 = (worldRuntime->motion).targetPositionYQ12 - cameraOffset.ecx;
  (worldRuntime->motion).positionZQ12 = (worldRuntime->motion).targetPositionZQ12 - cameraOffset.edx;
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
  AngleTurn32 requestedPitchAngle;
  AngleTurn32 clampedPitchAngle;
  
  requestedPitchAngle = (worldRuntime->motion).pitchAngle - pitchDeltaInput * g_WorldMotionPitchInputScale;
  clampedPitchAngle = requestedPitchAngle;
  if ((worldRuntime->runtimeFlags & 0x40000) == 0) {
    if ((int)(worldRuntime->motion).maximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = (worldRuntime->motion).maximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)(worldRuntime->motion).minimumPitchAngle) {
      clampedPitchAngle = (worldRuntime->motion).minimumPitchAngle;
    }
  }
  else if ((((worldRuntime->runtimeFlags & 0x200) != 0) &&
           (clampedPitchAngle = g_WorldMotionAlternateMaximumPitchAngle,
           (int)requestedPitchAngle <= (int)g_WorldMotionAlternateMaximumPitchAngle)) &&
          (clampedPitchAngle = requestedPitchAngle, (int)requestedPitchAngle < (int)g_WorldMotionAlternateMinimumPitchAngle)) {
    clampedPitchAngle = g_WorldMotionAlternateMinimumPitchAngle;
  }
  if ((int)clampedPitchAngle < 0x4001) {
    if ((int)clampedPitchAngle < -0x4000) {
      clampedPitchAngle = 0xffffc000;
    }
  }
  else {
    clampedPitchAngle = 0x4000;
  }
  (worldRuntime->motion).pitchAngle = clampedPitchAngle;
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
          (CameraScreenDeltaPixels screenDeltaX,uint32_t screenDeltaY,WorldRuntimeContext *worldRuntime
          )

{
  Q12 *coordinateField;
  AngleTurn32 angle;
  int distanceScaleOrSideDeltaY;
  FixedSinCosEdxEax8 movementDeltaXYQ12;
  FixedSinCosEdxEax8 sideMovementDeltaXYQ12;
  Q12 *motionCoordinateField;
  
  distanceScaleOrSideDeltaY = (int)(_k_CameraScreenDeltaDistanceScaleQ16 * (worldRuntime->motion).targetDistanceQ12) >>
          0x10;
  angle = (worldRuntime->motion).headingAngle;
  movementDeltaXYQ12 = FixedMath_SinCosScaled(angle,screenDeltaX * distanceScaleOrSideDeltaY);
  THANDOR_PART(uint32_t, movementDeltaXYQ12, 4) = (int)(movementDeltaXYQ12 >> 0x20);
  (worldRuntime->motion).positionXQ12 =
       (worldRuntime->motion).positionXQ12 - (int)movementDeltaXYQ12;
  motionCoordinateField = &(worldRuntime->motion).positionYQ12;
  *motionCoordinateField = *motionCoordinateField - THANDOR_PART(uint32_t, movementDeltaXYQ12, 4);
  coordinateField = &(worldRuntime->motion).targetPositionXQ12;
  *coordinateField = *coordinateField - (int)movementDeltaXYQ12;
  coordinateField = &(worldRuntime->motion).targetPositionYQ12;
  *coordinateField = *coordinateField - THANDOR_PART(uint32_t, movementDeltaXYQ12, 4);
  sideMovementDeltaXYQ12 = FixedMath_SinCosScaled(angle + 0x4000 & 0xffff,screenDeltaY * distanceScaleOrSideDeltaY);
  distanceScaleOrSideDeltaY = (int)(sideMovementDeltaXYQ12 >> 0x20);
  (worldRuntime->motion).positionXQ12 = (worldRuntime->motion).positionXQ12 - (int)sideMovementDeltaXYQ12;
  coordinateField = &(worldRuntime->motion).positionYQ12;
  *coordinateField = *coordinateField - distanceScaleOrSideDeltaY;
  coordinateField = &(worldRuntime->motion).targetPositionXQ12;
  *coordinateField = *coordinateField - (int)sideMovementDeltaXYQ12;
  coordinateField = &(worldRuntime->motion).targetPositionYQ12;
  *coordinateField = *coordinateField - distanceScaleOrSideDeltaY;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

