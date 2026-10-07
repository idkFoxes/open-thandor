/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/camera/camera_input.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/camera/camera_input.h>
#include <thandor/thandor.h>

/* Module data. */

static const Q12 g_WorldMotionPositionMagnitudeInputScaleQ12 = 16 /* 0.003906 */;

static const uint32_t k_CameraScreenDeltaDistanceScaleQ16 = 32;

static const Q12 g_WorldMotionDistanceInputScaleQ12 = 64 /* 0.015625 */;

static const AngleTurn32 g_WorldMotionHeadingInputScale = 16;

static const AngleTurn32 g_WorldMotionPitchInputScale = 16;

static const AngleTurn32 g_WorldMotionAlternateMinimumPitchAngle = 0xFFFFC400;

static const AngleTurn32 g_WorldMotionAlternateMaximumPitchAngle = 0xFFFFF600;

static const UQ12 g_WorldMotionAlternateMinimumDistanceQ12 = 32768;

static const UQ12 g_WorldMotionAlternateMaximumDistanceQ12 = 131072;

/* Camera drag sideways (right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/menu_room.cpp): moves camera position and target together by screenDelta scaled with
   k_CameraScreenDeltaDistanceScaleQ16 along the heading minus a quarter turn, at elevationAngle.
*/
void WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
          (AngleTurn32 elevationAngle,int screenDelta,WorldRuntimeContext *worldRuntime)

{
  FixedDirection translationDelta;

  translationDelta = FixedMath_DirectionFromAnglesScaled
                    (elevationAngle,worldRuntime->motion.headingAngle + FIXED_ANGLE16_THREE_QUARTER_TURN & FIXED_ANGLE16_MASK,
                     screenDelta * k_CameraScreenDeltaDistanceScaleQ16);
  worldRuntime->motion.positionXQ12 += translationDelta.x;
  worldRuntime->motion.positionYQ12 += translationDelta.y;
  worldRuntime->motion.positionZQ12 += translationDelta.z;
  worldRuntime->motion.targetPositionXQ12 += translationDelta.x;
  worldRuntime->motion.targetPositionYQ12 += translationDelta.y;
  worldRuntime->motion.targetPositionZQ12 += translationDelta.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Camera drag up/down (left+right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/menu_room.cpp): moves camera position and target together by the scaled screenDelta along the camera's
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
                    (elevationAngle,azimuthAngle,screenDelta * k_CameraScreenDeltaDistanceScaleQ16)
  ;
  worldRuntime->motion.positionXQ12 += translationDelta.x;
  worldRuntime->motion.positionYQ12 += translationDelta.y;
  worldRuntime->motion.positionZQ12 += translationDelta.z;
  worldRuntime->motion.targetPositionXQ12 += translationDelta.x;
  worldRuntime->motion.targetPositionYQ12 += translationDelta.y;
  worldRuntime->motion.targetPositionZQ12 += translationDelta.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Camera drag forward/back (right-button drag of the model pointer context in camera scheme 0x8000, together with
   the sideways move; ui/frontend/menu_room.cpp): moves camera position and target together by the scaled
   screenDelta against the viewing direction (negated pitch, heading plus half a turn).
*/
void WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
          (int screenDelta,WorldRuntimeContext *worldRuntime)

{
  FixedDirection translationDelta;

  translationDelta = FixedMath_DirectionFromAnglesScaled
                    (-worldRuntime->motion.pitchAngle,
                     worldRuntime->motion.headingAngle + FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,
                     screenDelta * k_CameraScreenDeltaDistanceScaleQ16);
  worldRuntime->motion.positionXQ12 += translationDelta.x;
  worldRuntime->motion.positionYQ12 += translationDelta.y;
  worldRuntime->motion.positionZQ12 += translationDelta.z;
  worldRuntime->motion.targetPositionXQ12 += translationDelta.x;
  worldRuntime->motion.targetPositionYQ12 += translationDelta.y;
  worldRuntime->motion.targetPositionZQ12 += translationDelta.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Orbits the camera around its target (right-button drag of the model pointer context in camera schemes 0x100 and
   0x200, ui/frontend/menu_room.cpp): turns the heading by headingDeltaInput * g_WorldMotionHeadingInputScale and puts
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
}

/* Turns the camera in place (Ctrl + right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/menu_room.cpp): changes only the heading, in the opposite sense of
   WorldMotion_AdjustHeadingAndRecomputePosition; position and target stay.
*/
void WorldMotion_AdjustHeadingAndClearFieldGridDirty(int headingDeltaInput,WorldRuntimeContext *worldRuntime)

{
  worldRuntime->motion.headingAngle =
       worldRuntime->motion.headingAngle - headingDeltaInput * g_WorldMotionHeadingInputScale &
       FIXED_ANGLE16_MASK;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Camera zoom (mouse wheel and camera drags of the model pointer context, ui/frontend/menu_room.cpp): changes
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
  if (!Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA)) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)requestedDistanceQ12) {
      clampedDistanceQ12 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)requestedDistanceQ12 < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedDistanceQ12 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if (Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE)) {
    if ((int)g_WorldMotionAlternateMaximumDistanceQ12 < (int)requestedDistanceQ12) {
      clampedDistanceQ12 = g_WorldMotionAlternateMaximumDistanceQ12;
    }
    else if ((int)requestedDistanceQ12 < (int)g_WorldMotionAlternateMinimumDistanceQ12) {
      clampedDistanceQ12 = g_WorldMotionAlternateMinimumDistanceQ12;
    }
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
}

/* Ctrl + left+right-button drag of the model pointer context in camera scheme 0x8000 (ui/frontend/menu_room.cpp):
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
  if (!Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA)) {
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)requestedMagnitudeQ12) {
      clampedMagnitudeQ12 = worldRuntime->maximumCameraDistanceQ12;
    }
    else if ((int)requestedMagnitudeQ12 < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedMagnitudeQ12 = worldRuntime->minimumCameraDistanceQ12;
    }
  }
  else if (Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE)) {
    if ((int)g_WorldMotionAlternateMaximumDistanceQ12 < (int)requestedMagnitudeQ12) {
      clampedMagnitudeQ12 = g_WorldMotionAlternateMaximumDistanceQ12;
    }
    else if ((int)requestedMagnitudeQ12 < (int)g_WorldMotionAlternateMinimumDistanceQ12) {
      clampedMagnitudeQ12 = g_WorldMotionAlternateMinimumDistanceQ12;
    }
  }
  if ((int)clampedMagnitudeQ12 < WORLD_MOTION_MINIMUM_DISTANCE_Q12) {
    clampedMagnitudeQ12 = WORLD_MOTION_MINIMUM_DISTANCE_Q12;
  }
  worldRuntime->motion.positionMagnitudeQ12 = clampedMagnitudeQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Camera tilt around its target (Ctrl + mouse wheel and camera drags of the model pointer context,
   ui/frontend/menu_room.cpp): changes the pitch by pitchDeltaInput * g_WorldMotionPitchInputScale, clamps it like
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
  if (!Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA)) {
    if ((int)worldRuntime->motion.maximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.maximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)worldRuntime->motion.minimumPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.minimumPitchAngle;
    }
  }
  else if (Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE)) {
    if ((int)g_WorldMotionAlternateMaximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = g_WorldMotionAlternateMaximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)g_WorldMotionAlternateMinimumPitchAngle) {
      clampedPitchAngle = g_WorldMotionAlternateMinimumPitchAngle;
    }
  }
  if ((int)clampedPitchAngle > FIXED_ANGLE16_QUARTER_TURN) {
    clampedPitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  else if ((int)clampedPitchAngle < -FIXED_ANGLE16_QUARTER_TURN) {
    clampedPitchAngle = -FIXED_ANGLE16_QUARTER_TURN;
  }
  worldRuntime->motion.pitchAngle = clampedPitchAngle;
  cameraOffset = FixedMath_DirectionFromAnglesScaled
                    (clampedPitchAngle,worldRuntime->motion.headingAngle,
                     worldRuntime->motion.targetDistanceQ12);
  worldRuntime->motion.positionXQ12 = worldRuntime->motion.targetPositionXQ12 - cameraOffset.x;
  worldRuntime->motion.positionYQ12 = worldRuntime->motion.targetPositionYQ12 - cameraOffset.y;
  worldRuntime->motion.positionZQ12 = worldRuntime->motion.targetPositionZQ12 - cameraOffset.z;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Tilts the camera in place (Ctrl + right-button drag of the model pointer context in camera scheme 0x8000,
   ui/frontend/menu_room.cpp): changes only the pitch, in the opposite sense of
   WorldMotion_AdjustPitchClampAndRecomputePosition and with the same clamps; position and target stay.
*/
void WorldMotion_AdjustPitchClampAndClearFieldGridDirty(int pitchDeltaInput,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 requestedPitchAngle;
  AngleTurn32 clampedPitchAngle;

  requestedPitchAngle = worldRuntime->motion.pitchAngle - pitchDeltaInput * g_WorldMotionPitchInputScale;
  clampedPitchAngle = requestedPitchAngle;
  if (!Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA)) {
    if ((int)worldRuntime->motion.maximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.maximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)worldRuntime->motion.minimumPitchAngle) {
      clampedPitchAngle = worldRuntime->motion.minimumPitchAngle;
    }
  }
  else if (Any(worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_ALTERNATE_CAMERA_RANGE)) {
    if ((int)g_WorldMotionAlternateMaximumPitchAngle < (int)requestedPitchAngle) {
      clampedPitchAngle = g_WorldMotionAlternateMaximumPitchAngle;
    }
    else if ((int)requestedPitchAngle < (int)g_WorldMotionAlternateMinimumPitchAngle) {
      clampedPitchAngle = g_WorldMotionAlternateMinimumPitchAngle;
    }
  }
  if ((int)clampedPitchAngle > FIXED_ANGLE16_QUARTER_TURN) {
    clampedPitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  else if ((int)clampedPitchAngle < -FIXED_ANGLE16_QUARTER_TURN) {
    clampedPitchAngle = -FIXED_ANGLE16_QUARTER_TURN;
  }
  worldRuntime->motion.pitchAngle = clampedPitchAngle;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}

/* Scrolls the camera by a screen-space delta (arrow keys, edge scrolling): screenDeltaDown moves along the
   camera heading, screenDeltaRight along the heading plus a quarter turn, both scaled with the camera distance so
   a scroll step covers the same screen distance at any zoom. Camera position and target move together.
*/
void WorldRuntime_TranslateCameraByScreenDelta
          (CameraScreenDeltaPixels screenDeltaDown,uint32_t screenDeltaRight,WorldRuntimeContext *worldRuntime
          )

{
  AngleTurn32 headingAngle;
  int distanceScale;
  FixedSinCos forwardDeltaXYQ12;
  FixedSinCos sideDeltaXYQ12;

  distanceScale = (int)(k_CameraScreenDeltaDistanceScaleQ16 * worldRuntime->motion.targetDistanceQ12) >> 16;
  headingAngle = worldRuntime->motion.headingAngle;
  forwardDeltaXYQ12 = FixedMath_SinCosScaled(headingAngle,screenDeltaDown * distanceScale);
  worldRuntime->motion.positionXQ12 -= forwardDeltaXYQ12.cosValue;
  worldRuntime->motion.positionYQ12 -= forwardDeltaXYQ12.sinValue;
  worldRuntime->motion.targetPositionXQ12 -= forwardDeltaXYQ12.cosValue;
  worldRuntime->motion.targetPositionYQ12 -= forwardDeltaXYQ12.sinValue;
  sideDeltaXYQ12 = FixedMath_SinCosScaled(headingAngle + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK,
                                          screenDeltaRight * distanceScale);
  worldRuntime->motion.positionXQ12 -= sideDeltaXYQ12.cosValue;
  worldRuntime->motion.positionYQ12 -= sideDeltaXYQ12.sinValue;
  worldRuntime->motion.targetPositionXQ12 -= sideDeltaXYQ12.cosValue;
  worldRuntime->motion.targetPositionYQ12 -= sideDeltaXYQ12.sinValue;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
}
