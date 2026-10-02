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
   Edge scrolling: while the cursor presses against a screen edge (g_CursorOverflow*), moves the camera by the
   configured scroll step in that direction and returns the matching scroll-arrow cursor frame
   (WORLD_CURSOR_SCROLL_*), or 0 when no edge is touched.
*/
uint32_t WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime)

{
  uint32_t edgeScrollStep;
  uint32_t bottomStepOrCursorFrame;
  uint32_t rightStepOrDeltaDown;
  uint32_t screenDeltaRight;

  edgeScrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  rightStepOrDeltaDown = 0;
  if (g_CursorOverflowRight != 0) {
    rightStepOrDeltaDown = edgeScrollStep;
  }
  bottomStepOrCursorFrame = 0;
  if (g_CursorOverflowBottom != 0) {
    bottomStepOrCursorFrame = edgeScrollStep;
  }
  /* delta = right/bottom step - left/top overflow; a negative result becomes -step */
  screenDeltaRight = rightStepOrDeltaDown - g_CursorOverflowLeft;
  if ((int)(rightStepOrDeltaDown - g_CursorOverflowLeft) < 0) {
    screenDeltaRight = -edgeScrollStep;
  }
  rightStepOrDeltaDown = bottomStepOrCursorFrame - g_CursorOverflowTop;
  if ((int)(bottomStepOrCursorFrame - g_CursorOverflowTop) < 0) {
    rightStepOrDeltaDown = -edgeScrollStep;
  }
  WorldRuntime_TranslateCameraByScreenDelta(rightStepOrDeltaDown,screenDeltaRight,worldRuntime);
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_UP;
  if ((screenDeltaRight == 0) && (rightStepOrDeltaDown == 0)) {
    bottomStepOrCursorFrame = 0;
  }
  else if (screenDeltaRight == 0) {
    if (-1 < (int)rightStepOrDeltaDown) {
      bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_DOWN;
    }
  }
  else if ((int)screenDeltaRight < 0) {
    bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_LEFT;
    if (rightStepOrDeltaDown != 0) {
      if ((int)rightStepOrDeltaDown < 0) {
        bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_UP_LEFT;
      }
      else {
        bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_DOWN_LEFT;
      }
    }
  }
  else {
    bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_RIGHT;
    if (rightStepOrDeltaDown != 0) {
      if ((int)rightStepOrDeltaDown < 0) {
        bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_UP_RIGHT;
      }
      else {
        bottomStepOrCursorFrame = WORLD_CURSOR_SCROLL_DOWN_RIGHT;
      }
    }
  }
  return bottomStepOrCursorFrame;
}


/* Address: 0x0050C7F0.
   Camera drag sideways (right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/runtime.c): moves camera position and target together by screenDelta scaled with
   k_CameraScreenDeltaDistanceScaleQ16 along the heading minus a quarter turn, at the elevation passed in EDX.
*/
void WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
          (AngleTurn32 elevationAngle,int screenDelta,WorldRuntimeContext *worldRuntime)

{
  FixedDirection translationDelta;

  translationDelta = FixedMath_DirectionFromAnglesScaled
                    (elevationAngle,worldRuntime->motion.headingAngle + FIXED_ANGLE16_THREE_QUARTER_TURN & FIXED_ANGLE16_MASK,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  worldRuntime->motion.positionXQ12 += translationDelta.x;
  worldRuntime->motion.positionYQ12 += translationDelta.y;
  worldRuntime->motion.positionZQ12 += translationDelta.z;
  worldRuntime->motion.targetPositionXQ12 += translationDelta.x;
  worldRuntime->motion.targetPositionYQ12 += translationDelta.y;
  worldRuntime->motion.targetPositionZQ12 += translationDelta.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C850.
   Camera drag up/down (left+right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/runtime.c): moves camera position and target together by the scaled screenDelta along the camera's
   up direction (pitch minus a quarter turn; past straight down the direction is mirrored with the heading turned
   by half a turn).
*/
void WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn(int screenDelta,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 azimuthAngle;
  AngleTurn32 elevationAngle;
  FixedDirection translationDelta;

  azimuthAngle = worldRuntime->motion.headingAngle;
  elevationAngle = worldRuntime->motion.pitchAngle - FIXED_ANGLE16_QUARTER_TURN;
  if ((int)elevationAngle < -FIXED_ANGLE16_QUARTER_TURN) {
    elevationAngle = -worldRuntime->motion.pitchAngle - FIXED_ANGLE16_QUARTER_TURN;
    azimuthAngle = azimuthAngle + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  }
  translationDelta = FixedMath_DirectionFromAnglesScaled
                    (elevationAngle,azimuthAngle,screenDelta * _k_CameraScreenDeltaDistanceScaleQ16)
  ;
  worldRuntime->motion.positionXQ12 += translationDelta.x;
  worldRuntime->motion.positionYQ12 += translationDelta.y;
  worldRuntime->motion.positionZQ12 += translationDelta.z;
  worldRuntime->motion.targetPositionXQ12 += translationDelta.x;
  worldRuntime->motion.targetPositionYQ12 += translationDelta.y;
  worldRuntime->motion.targetPositionZQ12 += translationDelta.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C8C0.
   Camera drag forward/back (right-button drag of the model pointer context in camera scheme 0x8000, together with
   the sideways move; ui/frontend/runtime.c): moves camera position and target together by the scaled
   screenDelta against the viewing direction (negated pitch, heading plus half a turn).
*/
void WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
          (int screenDelta,WorldRuntimeContext *worldRuntime)

{
  FixedDirection translationDelta;

  translationDelta = FixedMath_DirectionFromAnglesScaled
                    (-worldRuntime->motion.pitchAngle,
                     worldRuntime->motion.headingAngle + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,
                     screenDelta * _k_CameraScreenDeltaDistanceScaleQ16);
  worldRuntime->motion.positionXQ12 += translationDelta.x;
  worldRuntime->motion.positionYQ12 += translationDelta.y;
  worldRuntime->motion.positionZQ12 += translationDelta.z;
  worldRuntime->motion.targetPositionXQ12 += translationDelta.x;
  worldRuntime->motion.targetPositionYQ12 += translationDelta.y;
  worldRuntime->motion.targetPositionZQ12 += translationDelta.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C920.
   Orbits the camera around its target (right-button drag of the model pointer context in camera schemes 0x100 and
   0x200, ui/frontend/runtime.c): turns the heading by headingDeltaInput * g_WorldMotionHeadingInputScale and puts
   the camera back at targetDistanceQ12 from the unchanged target.
*/
void WorldMotion_AdjustHeadingAndRecomputePosition(int headingDeltaInput,WorldRuntimeContext *worldRuntime)

{
  uint32_t azimuthAngle;
  FixedDirection cameraOffset;
  
  azimuthAngle = worldRuntime->motion.headingAngle +
                 headingDeltaInput * g_WorldMotionHeadingInputScale & FIXED_ANGLE16_MASK;
  worldRuntime->motion.headingAngle = azimuthAngle;
  cameraOffset = FixedMath_DirectionFromAnglesScaled
                    (worldRuntime->motion.pitchAngle,azimuthAngle,
                     worldRuntime->motion.targetDistanceQ12);
  worldRuntime->motion.positionXQ12 = worldRuntime->motion.targetPositionXQ12 - cameraOffset.x;
  worldRuntime->motion.positionYQ12 = worldRuntime->motion.targetPositionYQ12 - cameraOffset.y;
  worldRuntime->motion.positionZQ12 = worldRuntime->motion.targetPositionZQ12 - cameraOffset.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C990.
   Turns the camera in place (Ctrl + right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/runtime.c): changes only the heading, in the opposite sense of
   WorldMotion_AdjustHeadingAndRecomputePosition; position and target stay.
*/
void WorldMotion_AdjustHeadingAndClearFieldGridDirty(int headingDeltaInput,WorldRuntimeContext *worldRuntime)

{
  worldRuntime->motion.headingAngle =
       worldRuntime->motion.headingAngle - headingDeltaInput * g_WorldMotionHeadingInputScale &
       FIXED_ANGLE16_MASK;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C9C0.
   Camera zoom (mouse wheel and camera drags of the model pointer context, ui/frontend/runtime.c): changes
   the camera distance by distanceDeltaInput * g_WorldMotionDistanceInputScaleQ12, clamps it to the world's camera
   distance range (with WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA only with flag 0x200, to the alternate range) and to at
   least WORLD_MOTION_MINIMUM_DISTANCE_Q12, and puts the camera at that distance from its target.
*/
void WorldMotion_AdjustDistanceClampAndRecomputePosition(int distanceDeltaInput,WorldRuntimeContext *worldRuntime)

{
  UQ12 requestedDistanceQ12;
  UQ12 clampedDistanceQ12;
  FixedDirection cameraOffset;

  requestedDistanceQ12 = distanceDeltaInput * g_WorldMotionDistanceInputScaleQ12 +
          worldRuntime->motion.committedDistanceQ12;
  clampedDistanceQ12 = requestedDistanceQ12;
  if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA) == 0) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)requestedDistanceQ12) {
      clampedDistanceQ12 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)requestedDistanceQ12 < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedDistanceQ12 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if ((((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE) != 0) &&
           (clampedDistanceQ12 = g_WorldMotionAlternateMaximumDistanceQ12,
           (int)requestedDistanceQ12 <= (int)g_WorldMotionAlternateMaximumDistanceQ12)) &&
          (clampedDistanceQ12 = requestedDistanceQ12,
           (int)requestedDistanceQ12 < (int)g_WorldMotionAlternateMinimumDistanceQ12)) {
    clampedDistanceQ12 = g_WorldMotionAlternateMinimumDistanceQ12;
  }
  if ((int)clampedDistanceQ12 < WORLD_MOTION_MINIMUM_DISTANCE_Q12) {
    clampedDistanceQ12 = WORLD_MOTION_MINIMUM_DISTANCE_Q12;
  }
  worldRuntime->motion.targetDistanceQ12 = clampedDistanceQ12;
  worldRuntime->motion.committedDistanceQ12 = clampedDistanceQ12;
  cameraOffset = FixedMath_DirectionFromAnglesScaled
                    (worldRuntime->motion.pitchAngle,worldRuntime->motion.headingAngle,clampedDistanceQ12);
  worldRuntime->motion.positionXQ12 = worldRuntime->motion.targetPositionXQ12 - cameraOffset.x;
  worldRuntime->motion.positionYQ12 = worldRuntime->motion.targetPositionYQ12 - cameraOffset.y;
  worldRuntime->motion.positionZQ12 = worldRuntime->motion.targetPositionZQ12 - cameraOffset.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050CA80.
   Ctrl + left+right-button drag of the model pointer context in camera scheme 0x8000 (ui/frontend/runtime.c):
   changes positionMagnitudeQ12 against magnitudeDeltaInput with the same clamps as
   WorldMotion_AdjustDistanceClampAndRecomputePosition, without moving the camera.
*/
void WorldMotion_AdjustPositionMagnitudeClamp(int magnitudeDeltaInput,WorldRuntimeContext *worldRuntime)

{
  UQ12 requestedMagnitudeQ12;
  UQ12 clampedMagnitudeQ12;

  requestedMagnitudeQ12 = worldRuntime->motion.positionMagnitudeQ12 -
          magnitudeDeltaInput * g_WorldMotionPositionMagnitudeInputScaleQ12;
  clampedMagnitudeQ12 = requestedMagnitudeQ12;
  if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA) == 0) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)requestedMagnitudeQ12) {
      clampedMagnitudeQ12 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)requestedMagnitudeQ12 < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedMagnitudeQ12 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if ((((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE) != 0) &&
           (clampedMagnitudeQ12 = g_WorldMotionAlternateMaximumDistanceQ12,
           (int)requestedMagnitudeQ12 <= (int)g_WorldMotionAlternateMaximumDistanceQ12)) &&
          (clampedMagnitudeQ12 = requestedMagnitudeQ12,
           (int)requestedMagnitudeQ12 < (int)g_WorldMotionAlternateMinimumDistanceQ12)) {
    clampedMagnitudeQ12 = g_WorldMotionAlternateMinimumDistanceQ12;
  }
  if ((int)clampedMagnitudeQ12 < WORLD_MOTION_MINIMUM_DISTANCE_Q12) {
    clampedMagnitudeQ12 = WORLD_MOTION_MINIMUM_DISTANCE_Q12;
  }
  worldRuntime->motion.positionMagnitudeQ12 = clampedMagnitudeQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050CB10.
   Camera tilt around its target (Ctrl + mouse wheel and camera drags of the model pointer context,
   ui/frontend/runtime.c): changes the pitch by pitchDeltaInput * g_WorldMotionPitchInputScale, clamps it like
   the distance in WorldMotion_AdjustDistanceClampAndRecomputePosition (world pitch range or, unlimited with flag
   0x200, the alternate range) and always to +-a quarter turn, and puts the camera back around the target.
*/
void WorldMotion_AdjustPitchClampAndRecomputePosition(int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 requestedPitchAngle;
  AngleTurn32 clampedPitchAngle;
  FixedDirection cameraOffset;

  requestedPitchAngle = worldRuntime->motion.pitchAngle + pitchDeltaInput * g_WorldMotionPitchInputScale;
  clampedPitchAngle = requestedPitchAngle;
  if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA) == 0) {
    if ((int)(worldRuntime->motion).maximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.maximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)(worldRuntime->motion).minimumPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.minimumPitchAngle;
    }
  }
  else if ((((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE) != 0) &&
           (clampedPitchAngle = g_WorldMotionAlternateMaximumPitchAngle,
           (int)requestedPitchAngle <= (int)g_WorldMotionAlternateMaximumPitchAngle)) &&
          (clampedPitchAngle = requestedPitchAngle,
           (int)requestedPitchAngle < (int)g_WorldMotionAlternateMinimumPitchAngle)) {
    clampedPitchAngle = g_WorldMotionAlternateMinimumPitchAngle;
  }
  if ((int)clampedPitchAngle < FIXED_ANGLE16_QUARTER_TURN + 1) {
    if ((int)clampedPitchAngle < -FIXED_ANGLE16_QUARTER_TURN) {
      clampedPitchAngle = -FIXED_ANGLE16_QUARTER_TURN;
    }
  }
  else {
    clampedPitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  worldRuntime->motion.pitchAngle = clampedPitchAngle;
  cameraOffset = FixedMath_DirectionFromAnglesScaled
                    (clampedPitchAngle,worldRuntime->motion.headingAngle,
                     worldRuntime->motion.targetDistanceQ12);
  worldRuntime->motion.positionXQ12 = worldRuntime->motion.targetPositionXQ12 - cameraOffset.x;
  worldRuntime->motion.positionYQ12 = worldRuntime->motion.targetPositionYQ12 - cameraOffset.y;
  worldRuntime->motion.positionZQ12 = worldRuntime->motion.targetPositionZQ12 - cameraOffset.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050CBE0.
   Tilts the camera in place (Ctrl + right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/runtime.c): changes only the pitch, in the opposite sense of
   WorldMotion_AdjustPitchClampAndRecomputePosition and with the same clamps; position and target stay.
*/
void WorldMotion_AdjustPitchClampAndClearFieldGridDirty(int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 requestedPitchAngle;
  AngleTurn32 clampedPitchAngle;

  requestedPitchAngle = worldRuntime->motion.pitchAngle - pitchDeltaInput * g_WorldMotionPitchInputScale;
  clampedPitchAngle = requestedPitchAngle;
  if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA) == 0) {
    if ((int)(worldRuntime->motion).maximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.maximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)(worldRuntime->motion).minimumPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.minimumPitchAngle;
    }
  }
  else if ((((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE) != 0) &&
           (clampedPitchAngle = g_WorldMotionAlternateMaximumPitchAngle,
           (int)requestedPitchAngle <= (int)g_WorldMotionAlternateMaximumPitchAngle)) &&
          (clampedPitchAngle = requestedPitchAngle,
           (int)requestedPitchAngle < (int)g_WorldMotionAlternateMinimumPitchAngle)) {
    clampedPitchAngle = g_WorldMotionAlternateMinimumPitchAngle;
  }
  if ((int)clampedPitchAngle < FIXED_ANGLE16_QUARTER_TURN + 1) {
    if ((int)clampedPitchAngle < -FIXED_ANGLE16_QUARTER_TURN) {
      clampedPitchAngle = -FIXED_ANGLE16_QUARTER_TURN;
    }
  }
  else {
    clampedPitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  worldRuntime->motion.pitchAngle = clampedPitchAngle;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050C770.
   Scrolls the camera by a screen-space delta (arrow keys, edge scrolling): screenDeltaDown moves along the
   camera heading, screenDeltaRight along the heading plus a quarter turn, both scaled with the camera distance so
   a scroll step covers the same screen distance at any zoom. Camera position and target move together.
*/
void WorldRuntime_TranslateCameraByScreenDelta
          (CameraScreenDeltaPixels screenDeltaDown,uint32_t screenDeltaRight,WorldRuntimeContext *worldRuntime
          )

{
  Q12 *coordinateField;
  AngleTurn32 angle;
  int distanceScaleOrSideDeltaY;
  FixedSinCos movementDeltaXYQ12;
  FixedSinCos sideMovementDeltaXYQ12;
  Q12 movementDeltaXQ12;
  Q12 movementDeltaYQ12;
  Q12 *motionCoordinateField;

  distanceScaleOrSideDeltaY =
       (int)(_k_CameraScreenDeltaDistanceScaleQ16 * worldRuntime->motion.targetDistanceQ12) >> 16;
  angle = worldRuntime->motion.headingAngle;
  movementDeltaXYQ12 = FixedMath_SinCosScaled(angle,screenDeltaDown * distanceScaleOrSideDeltaY);
  movementDeltaXQ12 = movementDeltaXYQ12.cosValue;
  movementDeltaYQ12 = movementDeltaXYQ12.sinValue;
  worldRuntime->motion.positionXQ12 =
       worldRuntime->motion.positionXQ12 - movementDeltaXQ12;
  motionCoordinateField = &worldRuntime->motion.positionYQ12;
  *motionCoordinateField = *motionCoordinateField - movementDeltaYQ12;
  coordinateField = &worldRuntime->motion.targetPositionXQ12;
  *coordinateField = *coordinateField - movementDeltaXQ12;
  coordinateField = &worldRuntime->motion.targetPositionYQ12;
  *coordinateField = *coordinateField - movementDeltaYQ12;
  sideMovementDeltaXYQ12 = FixedMath_SinCosScaled(angle + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK,
                                                  screenDeltaRight * distanceScaleOrSideDeltaY);
  distanceScaleOrSideDeltaY = sideMovementDeltaXYQ12.sinValue;
  worldRuntime->motion.positionXQ12 -= sideMovementDeltaXYQ12.cosValue;
  worldRuntime->motion.positionYQ12 -= distanceScaleOrSideDeltaY;
  worldRuntime->motion.targetPositionXQ12 -= sideMovementDeltaXYQ12.cosValue;
  worldRuntime->motion.targetPositionYQ12 -= distanceScaleOrSideDeltaY;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

