/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/camera/motion_spline.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/camera/motion_spline.h>
#include <thandor/thandor.h>

/* Module data. */

static float g_WorldMotionSplineCachedDerivatives[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

static int32_t g_WorldMotionSplineEquationCounts[6] = {0};

float *g_WorldMotionSplineMatrixWorkspaces[6] = {};

float *g_WorldMotionSplineCoefficientTables[6] = {};

/* Implementation ownership: world/camera/motion_spline. */

/* Plays a six-channel keyframe spline at timeQ12: finds the first keyframe later than the time, evaluates
   the cubic segment before it and applies channels 0..2 as the position and 3..5 as magnitude/yaw/pitch
   to worldRuntime, caching the six derivatives. Returns true while the spline runs; past the last
   keyframe it applies that keyframe, clears the derivatives and returns false.
*/
Bool8 WorldMotionSpline_EvaluateAndApplyAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime)

{
  int32_t positionX;
  int32_t positionY;
  int32_t positionZ;
  UQ12 magnitude;
  uint32_t yawAngle;
  AngleTurn32 pitchAngle;
  int keyframeIndex;
  WorldMotionSplineKeyframe *currentKeyframe;
  WorldRuntimeContext *runtimeCopy;

  /* Every channel evaluates the same segment (keyframeIndex - 1). A time before the first keyframe gives
     segment -1. */
  keyframeIndex = 0;
  do {
    currentKeyframe = keyframes;
    if ((uint32_t)timeQ12 < (uint32_t)currentKeyframe->timeQ12) {
      runtimeCopy = worldRuntime;
      positionX = CubicSpline_EvaluateValueQ12
                            (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[0]);
      positionY = CubicSpline_EvaluateValueQ12
                            (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[1]);
      positionZ = CubicSpline_EvaluateValueQ12
                            (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[2]);
      WorldRuntime_SetCameraPositionKeepingTarget(positionZ,positionY,positionX,runtimeCopy);
      runtimeCopy = worldRuntime;
      magnitude = CubicSpline_EvaluateValueQ12
                            (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[3]);
      yawAngle = CubicSpline_EvaluateValueQ12
                        (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[4]);
      yawAngle = yawAngle & FIXED_ANGLE16_MASK; /* 16-bit angle: wrap to one turn */
      pitchAngle = CubicSpline_EvaluateValueQ12
                             (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[5]);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                ((worldRuntime->motion).projectionShift,pitchAngle,yawAngle,magnitude,runtimeCopy);
      g_WorldMotionSplineCachedDerivatives[0] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[0]);
      g_WorldMotionSplineCachedDerivatives[1] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[1]);
      g_WorldMotionSplineCachedDerivatives[2] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[2]);
      g_WorldMotionSplineCachedDerivatives[3] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[3]);
      g_WorldMotionSplineCachedDerivatives[4] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[4]);
      g_WorldMotionSplineCachedDerivatives[5] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[5]);
      return true;
    }
    keyframeIndex++;
    keyframeCount--;
    keyframes = currentKeyframe + 1;
  } while (keyframeCount != 0);
  /* past the end: hold the last keyframe */
  yawAngle = currentKeyframe->channel4Q12;
  WorldRuntime_SetCameraPositionKeepingTarget
            (currentKeyframe->channel2Q12,currentKeyframe->channel1Q12,currentKeyframe->channel0Q12,worldRuntime);
  WorldRuntime_SetCameraAnglesAndMagnitudeClamped
            ((worldRuntime->motion).projectionShift,currentKeyframe->channel5Q12,yawAngle & FIXED_ANGLE16_MASK,
             currentKeyframe->channel3Q12,worldRuntime);
  WorldMotionSpline_ClearCachedDerivatives();
  return false;
}

/* Prepares a world motion path (the frontend ROM transition's view flight): makes the yaw channel (4) continuous, so the spline turns the
   short way across the 0/0x10000 wrap, then builds and solves a natural cubic spline for each of the six
   keyframe channels into the global coefficient tables the evaluators read.
*/
void WorldMotionSpline_BuildSixChannelCurves
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes)

{
  WorldMotionSplineKeyframe *keyframeCursor;
  uint32_t unwrappedAngle;
  int remainingCount;
  uint32_t previousAngle;
  int unwrapDelta;

  if (1 < keyframeCount) {
    unwrappedAngle = keyframes->channel4Q12;
    remainingCount = keyframeCount - 1;
    previousAngle = unwrappedAngle;
    keyframeCursor = keyframes;
    do {
      /* shortest signed turn from the previous yaw to the next one */
      unwrapDelta = (keyframeCursor[1].channel4Q12 & 0xffffU) - (previousAngle & FIXED_ANGLE16_MASK);
      if (FIXED_ANGLE16_HALF_TURN < unwrapDelta) {
        unwrapDelta = unwrapDelta - FIXED_ANGLE16_FULL_TURN;
      }
      if (unwrapDelta < -FIXED_ANGLE16_HALF_TURN) {
        unwrapDelta = unwrapDelta + FIXED_ANGLE16_FULL_TURN;
      }
      unwrappedAngle = unwrappedAngle + unwrapDelta;
      previousAngle = (previousAngle & FIXED_ANGLE16_MASK) + unwrapDelta;
      keyframeCursor[1].channel4Q12 = unwrappedAngle;
      remainingCount--;
      keyframeCursor++;
    } while (remainingCount != 0);
  }
  /* the sixth argument is the channel's byte offset in the keyframe (channel n at n * 4) */
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[0],
             (CubicSplineEquationCount *)g_WorldMotionSplineEquationCounts,
             g_WorldMotionSplineCoefficientTables[0],g_WorldMotionSplineMatrixWorkspaces[0],
             keyframeCount,0,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[1],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 1),
             g_WorldMotionSplineCoefficientTables[1],g_WorldMotionSplineMatrixWorkspaces[1],
             keyframeCount,4,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[2],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 2),
             g_WorldMotionSplineCoefficientTables[2],g_WorldMotionSplineMatrixWorkspaces[2],
             keyframeCount,8,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[3],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 3),
             g_WorldMotionSplineCoefficientTables[3],g_WorldMotionSplineMatrixWorkspaces[3],
             keyframeCount,12,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[4],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 4),
             g_WorldMotionSplineCoefficientTables[4],g_WorldMotionSplineMatrixWorkspaces[4],
             keyframeCount,16,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[5],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 5),
             g_WorldMotionSplineCoefficientTables[5],g_WorldMotionSplineMatrixWorkspaces[5],
             keyframeCount,20,keyframes);
  CubicSpline_SolveCoefficientSystem
            (g_WorldMotionSplineEquationCounts[0],g_WorldMotionSplineCoefficientTables[0],
             g_WorldMotionSplineMatrixWorkspaces[0]);
  CubicSpline_SolveCoefficientSystem
            (g_WorldMotionSplineEquationCounts[1],g_WorldMotionSplineCoefficientTables[1],
             g_WorldMotionSplineMatrixWorkspaces[1]);
  CubicSpline_SolveCoefficientSystem
            (g_WorldMotionSplineEquationCounts[2],g_WorldMotionSplineCoefficientTables[2],
             g_WorldMotionSplineMatrixWorkspaces[2]);
  CubicSpline_SolveCoefficientSystem
            (g_WorldMotionSplineEquationCounts[3],g_WorldMotionSplineCoefficientTables[3],
             g_WorldMotionSplineMatrixWorkspaces[3]);
  CubicSpline_SolveCoefficientSystem
            (g_WorldMotionSplineEquationCounts[4],g_WorldMotionSplineCoefficientTables[4],
             g_WorldMotionSplineMatrixWorkspaces[4]);
  CubicSpline_SolveCoefficientSystem
            (g_WorldMotionSplineEquationCounts[5],g_WorldMotionSplineCoefficientTables[5],
             g_WorldMotionSplineMatrixWorkspaces[5]);
}

/* Zeroes the six derivatives cached by the world-motion spline evaluators, so a finished or newly built
   spline reports no motion.
*/
void WorldMotionSpline_ClearCachedDerivatives()

{
  int derivativesRemaining;
  float *derivativeCursor;

  derivativeCursor = g_WorldMotionSplineCachedDerivatives;
  for (derivativesRemaining = WORLD_MOTION_SPLINE_CHANNEL_COUNT; derivativesRemaining != 0; derivativesRemaining--) {
    *derivativeCursor = 0.0;
    derivativeCursor++;
  }
  return;
}
