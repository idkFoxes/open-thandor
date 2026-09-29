/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/interpolation.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/interpolation.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/math/interpolation. */

/* Address: 0x0053CA30.
   Plays a six-channel keyframe spline at timeQ12: finds the first keyframe later than the time, evaluates
   the cubic segment before it and applies channels 0..2 as the position and 3..5 as magnitude/yaw/pitch
   to worldRuntime, caching the six derivatives. Returns true (CF set) while the spline runs; past the last
   keyframe it applies that keyframe, clears the derivatives and returns false.
*/
bool WorldMotionSpline_EvaluateAndApplyAtTime
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

  /* Every channel evaluates the same segment (keyframeIndex - 1); Ghidra showed the re-pushed register
     as uninitialized segmentIndex_NN locals. A time before the first keyframe gives segment -1. */
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


/* Address: 0x0053CBB0.
   Orbit variant of WorldMotionSpline_EvaluateAndApplyAtTime: evaluates the six-channel keyframe spline at
   timeQ12 and applies channels 0..2 as the orbit origin (position80) and 3..5 as distance/yaw/pitch, from
   which the world runtime rebuilds position60; the six derivatives are cached. Returns 1 (CF set) while the
   spline runs; past the last keyframe it applies that keyframe, clears the derivatives and returns 0.
   No caller, function-pointer table or data reference to 0x0053CBB0 was found in the port or the image data.
*/
uint8_t WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime)

{
  int32_t originX;
  int32_t originY;
  int32_t originZ;
  UQ12 distance;
  uint32_t yawAngle;
  AngleTurn32 pitchAngle;
  int keyframeIndex; /* EDX: the segment index is passed (minus one) to every evaluation */
  WorldMotionSplineKeyframe *currentKeyframe;
  
  keyframeIndex = 0;
  do {
    currentKeyframe = keyframes;
    if ((uint32_t)timeQ12 < (uint32_t)currentKeyframe->timeQ12) {
      originX = CubicSpline_EvaluateValueQ12
                          (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[0]);
      originY = CubicSpline_EvaluateValueQ12
                          (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[1]);
      originZ = CubicSpline_EvaluateValueQ12
                          (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[2]);
      distance = CubicSpline_EvaluateValueQ12
                           (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[3]);
      yawAngle = CubicSpline_EvaluateValueQ12
                        (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[4]);
      yawAngle = yawAngle & FIXED_ANGLE16_MASK; /* 16-bit angle: wrap to one turn */
      pitchAngle = CubicSpline_EvaluateValueQ12
                             (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[5]);
      WorldRuntime_PointCameraAtTarget
                (pitchAngle,yawAngle,distance,originZ,originY,originX,worldRuntime);
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
      return 1;
    }
    keyframeIndex++;
    keyframeCount--;
    keyframes = currentKeyframe + 1;
  } while (keyframeCount != 0);
  /* past the end: hold the last keyframe */
  WorldRuntime_PointCameraAtTarget
            (currentKeyframe->channel5Q12,currentKeyframe->channel4Q12 & FIXED_ANGLE16_MASK,currentKeyframe->channel3Q12,
             currentKeyframe->channel2Q12,currentKeyframe->channel1Q12,currentKeyframe->channel0Q12,worldRuntime);
  WorldMotionSpline_ClearCachedDerivatives();
  return 0;
}


/* Address: 0x0053CD10.
   Prepares a world motion path (the frontend ROM transition's view flight): makes the yaw channel (4) continuous, so the spline turns the
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


/* Address: 0x004CCC00.
   Starts fading out a dynamic light (shading record) over fadeOutTicks: a negative transition duration
   makes InterpolationStateTable_Advance256ByTicks shrink the radius to zero and then free the light. A
   light still fading in keeps its current fraction; a zero duration switches the light off at once.
*/
void InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount fadeOutTicks,GraphicsShadingRuntimeRecord *shadingRecord)

{
  PackedRgb24 negatedDurationOrZero; /* EAX, also the value the switch-off path stores */
  int durationOrElapsed;

  if (shadingRecord == NULL) {
    return;
  }
  negatedDurationOrZero = -fadeOutTicks;
  if (((int)negatedDurationOrZero < 0) && (-1 < shadingRecord->radiusTransitionDurationTicks)) {
    if (shadingRecord->radiusTransitionDurationTicks == 0) {
      /* steady light: elapsed runs from -fadeOutTicks up to 0 */
      shadingRecord->radiusTransitionDurationTicks = negatedDurationOrZero;
      shadingRecord->radiusTransitionElapsedTicks = negatedDurationOrZero;
      return;
    }
    /* fading in: XCHG in the new duration, rescale elapsed to keep the reached radius fraction */
    LOCK();
    durationOrElapsed = shadingRecord->radiusTransitionDurationTicks;
    shadingRecord->radiusTransitionDurationTicks = negatedDurationOrZero;
    UNLOCK();
    durationOrElapsed = (int)(((int64_t)(int)negatedDurationOrZero * (int64_t)shadingRecord->radiusTransitionElapsedTicks
                  ) / (int64_t)durationOrElapsed);
    shadingRecord->radiusTransitionElapsedTicks = durationOrElapsed;
    negatedDurationOrZero = 0;
    if (durationOrElapsed != 0) {
      return;
    }
  }
  /* Switch-off path. The original stores EAX here, which is -fadeOutTicks (not 0) when the light
     was already fading out (0x004CCC19). */
  ((PackedRgb24 *)&shadingRecord->squaredRadiusQ24)[1] = negatedDurationOrZero; /* high dword */
  ((PackedRgb24 *)&shadingRecord->squaredRadiusQ24)[0] = negatedDurationOrZero;
  shadingRecord->packedColorRgbActive = negatedDurationOrZero;
  shadingRecord->targetRadiusQ12 = negatedDurationOrZero;
}


/* Address: 0x004CCC80.
   Advances the radius transitions of all 256 dynamic lights (shading records) by elapsedTicks. An active
   light in transition gets radius = target * elapsed / duration (squared for the shading pass); a finished
   fade-in becomes steady, a finished fade-out (negative duration) frees the light.
*/
void InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks)

{
  int durationTicks;
  int currentRadius;
  int remainingCount;
  GraphicsShadingRuntimeRecord *shadingRecord;

  shadingRecord = g_GraphicsShadingRuntimeRecords;
  for (remainingCount = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT; remainingCount != 0; remainingCount--) {
    durationTicks = shadingRecord->radiusTransitionDurationTicks;
    if ((shadingRecord->packedColorRgbActive != 0) && (durationTicks != 0)) {
      /* the radius uses the elapsed time from before this step */
      currentRadius = (int)(((int64_t)shadingRecord->targetRadiusQ12 *
                    (int64_t)shadingRecord->radiusTransitionElapsedTicks) / (int64_t)durationTicks);
      shadingRecord->radiusTransitionElapsedTicks = shadingRecord->radiusTransitionElapsedTicks + elapsedTicks;
      shadingRecord->squaredRadiusQ24 = (int64_t)currentRadius * (int64_t)currentRadius;
      if (durationTicks < 0) {
        if ((uint32_t)shadingRecord->radiusTransitionElapsedTicks < 0x80000000) { /* elapsed >= 0 */
          /* fade-out finished: free the light */
          shadingRecord->squaredRadiusQ24 = 0;
          shadingRecord->targetRadiusQ12 = 0;
          shadingRecord->packedColorRgbActive = 0;
          shadingRecord->radiusTransitionElapsedTicks = 0;
          shadingRecord->radiusTransitionDurationTicks = 0;
        }
      }
      else if (durationTicks < shadingRecord->radiusTransitionElapsedTicks) {
        /* fade-in complete: the light stays at its last radius */
        shadingRecord->radiusTransitionElapsedTicks = 0;
        shadingRecord->radiusTransitionDurationTicks = 0;
      }
    }
    shadingRecord++;
  }
}


/* Address: 0x0053D230.
   Solves the spline equation system built by CubicSpline_BuildNaturalCoefficientSystem in place: an LU
   (Doolittle) decomposition of the matrix without pivoting, whose unit-L forward substitution runs along row
   by row, followed by back substitution with U. rhsVector then holds the four coefficients of every segment.
*/
void CubicSpline_SolveCoefficientSystem(CubicSplineEquationCount equationCount,float *rhsVector,float *matrix32x32)

{
  float pivot;
  CubicSplineMatrixIndex rowIndex;
  CubicSplineMatrixIndex columnOrRowIndex;
  uint32_t followingIndex;
  uint32_t nextRowIndex;

  rowIndex = 0;
  columnOrRowIndex = 0;
  do {
    /* row rowIndex of U (columns rowIndex..n-1), then the forward-substituted right-hand side */
    do {
      followingIndex = columnOrRowIndex + 1;
      CubicSpline_ForwardEliminateColumn(1.0,rowIndex - 1,columnOrRowIndex,rowIndex,matrix32x32);
      columnOrRowIndex = followingIndex;
    } while (followingIndex < equationCount);
    CubicSpline_BackSubstituteRow(1.0,rowIndex - 1,0,rowIndex,rhsVector,matrix32x32);
    if (rowIndex + 1 < equationCount) {
      /* column rowIndex of L below the diagonal, divided by the pivot U[rowIndex][rowIndex] */
      pivot = matrix32x32[rowIndex * (CUBIC_SPLINE_MATRIX_ORDER + 1)];
      followingIndex = rowIndex + 1;
      do {
        nextRowIndex = followingIndex + 1;
        CubicSpline_ForwardEliminateColumn(pivot,rowIndex - 1,rowIndex,followingIndex,matrix32x32);
        followingIndex = nextRowIndex;
      } while (nextRowIndex < equationCount);
    }
    rowIndex++;
    columnOrRowIndex = rowIndex;
  } while (rowIndex < equationCount);
  /* back substitution from the last row up */
  columnOrRowIndex = equationCount - 1;
  do {
    CubicSpline_BackSubstituteRow
              (matrix32x32[columnOrRowIndex * (CUBIC_SPLINE_MATRIX_ORDER + 1)],equationCount - 1,
               columnOrRowIndex + 1,columnOrRowIndex,rhsVector,matrix32x32);
    columnOrRowIndex--;
  } while (-1 < (int)columnOrRowIndex);
}


/* Address: 0x0053CF10.
   Builds the equation system of a piecewise cubic spline through one channel of the keyframes (times and values
   Q12, converted to float seconds/units): segment s has the unknowns a + b*t + c*t^2 + d*t^3 in columns 4s..4s+3.
   Per segment, row 4s and 4s+3 fix the values at both keyframes, row 4s+2 makes the slope continuous and row
   4s+5 the curvature; row 1 sets the start slope to startDerivative and the last segment's row 4s+2 the end slope
   to 0 (so the spline is clamped, not natural). The right-hand side goes to outCoefficients.
*/
void CubicSpline_BuildNaturalCoefficientSystem(float startDerivative,CubicSplineEquationCount *outEquationCount,
          float *outCoefficients,float *matrix32x32,WorldMotionSplineKeyframeCount keyframeCount,
          WorldMotionSplineChannelByteOffset channelByteOffset,WorldMotionSplineKeyframe *keyframes)

{
  float knotTimeOrTerm;
  int remainingCount;
  int *endValueCursor;
  int *startValueCursor;
  WorldMotionSplineKeyframe *keyframeCursor;
  float *floatCursor;

  /* In the matrix loops floatCursor points at the top-left of segment s's 4x4 diagonal block and steps one
     block (4 rows and 4 columns) per segment; indices below are written as row * order + column. */
  endValueCursor = (int *)((uint8_t *)&keyframes->channel0Q12 + channelByteOffset);
  *outEquationCount = keyframeCount * 4 - 4;
  floatCursor = matrix32x32;
  for (remainingCount = CUBIC_SPLINE_MATRIX_ORDER * CUBIC_SPLINE_MATRIX_ORDER; remainingCount != 0;
      remainingCount--) {
    *floatCursor = 0.0;
    floatCursor = floatCursor + 1;
  }
  /* row 4s: the segment's value at its start keyframe */
  remainingCount = keyframeCount - 1;
  keyframeCursor = keyframes;
  floatCursor = matrix32x32;
  do {
    knotTimeOrTerm = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
    *floatCursor = 1.0;
    floatCursor[1] = knotTimeOrTerm;
    floatCursor[2] = knotTimeOrTerm * knotTimeOrTerm;
    floatCursor[3] = knotTimeOrTerm * knotTimeOrTerm * knotTimeOrTerm;
    keyframeCursor = keyframeCursor + 1;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    remainingCount--;
  } while (remainingCount != 0);
  /* row 4s+3: the segment's value at its end keyframe */
  remainingCount = keyframeCount - 1;
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  do {
    knotTimeOrTerm = (float)keyframeCursor[1].timeQ12 / g_Q12FloatScale4096;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 0] = 1.0;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 1] = knotTimeOrTerm;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTimeOrTerm * knotTimeOrTerm;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTimeOrTerm * knotTimeOrTerm * knotTimeOrTerm;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    remainingCount--;
    keyframeCursor = keyframeCursor + 1;
  } while (remainingCount != 0);
  /* row 4s+2 (all but the last segment): slope of segment s minus slope of segment s+1 at their shared knot */
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  for (remainingCount = keyframeCount - 2; remainingCount != 0; remainingCount--) {
    knotTimeOrTerm = (float)keyframeCursor[1].timeQ12 / g_Q12FloatScale4096;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 1] = 1.0;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 5] = -1.0;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTimeOrTerm + knotTimeOrTerm;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 6] = -(knotTimeOrTerm + knotTimeOrTerm);
    knotTimeOrTerm = knotTimeOrTerm * knotTimeOrTerm;
    knotTimeOrTerm = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTimeOrTerm;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 7] = -knotTimeOrTerm;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    keyframeCursor = keyframeCursor + 1;
  }
  /* row 4s+5: curvature of segment s+1 minus curvature of segment s at their shared knot; the loop leaves
     keyframeCursor on the last keyframe and floatCursor on the last segment's block */
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  for (remainingCount = keyframeCount - 2; keyframeCursor = keyframeCursor + 1, remainingCount != 0;
      remainingCount--) {
    knotTimeOrTerm = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 6] = 2.0;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 2] = -2.0;
    knotTimeOrTerm = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
    knotTimeOrTerm = knotTimeOrTerm + knotTimeOrTerm;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 7] = knotTimeOrTerm;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 3] = -knotTimeOrTerm;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
  }
  /* last segment, row 4s+2: slope at the last keyframe (right-hand side 0) */
  knotTimeOrTerm = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
  floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 1] = 1.0;
  floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTimeOrTerm + knotTimeOrTerm;
  knotTimeOrTerm = knotTimeOrTerm * knotTimeOrTerm;
  floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
  /* row 1: slope at the first keyframe (right-hand side startDerivative) */
  knotTimeOrTerm = (float)keyframes->timeQ12 / g_Q12FloatScale4096;
  matrix32x32[1 * CUBIC_SPLINE_MATRIX_ORDER + 1] = 1.0;
  matrix32x32[1 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTimeOrTerm + knotTimeOrTerm;
  knotTimeOrTerm = knotTimeOrTerm * knotTimeOrTerm;
  matrix32x32[1 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
  /* right-hand side per segment: start value, 0, 0, end value (one keyframe apart) */
  remainingCount = keyframeCount - 1;
  startValueCursor = endValueCursor;
  floatCursor = outCoefficients;
  do {
    *floatCursor = (float)*startValueCursor / g_Q12FloatScale4096;
    floatCursor[1] = 0.0;
    floatCursor[2] = 0.0;
    startValueCursor = startValueCursor + sizeof(WorldMotionSplineKeyframe) / sizeof(int);
    floatCursor = floatCursor + 4;
    remainingCount--;
  } while (remainingCount != 0);
  remainingCount = keyframeCount - 1;
  floatCursor = outCoefficients;
  do {
    endValueCursor = endValueCursor + sizeof(WorldMotionSplineKeyframe) / sizeof(int);
    floatCursor[3] = (float)*endValueCursor / g_Q12FloatScale4096;
    floatCursor = floatCursor + 4;
    remainingCount--;
  } while (remainingCount != 0);
  outCoefficients[1] = startDerivative;
}


/* Address: 0x0053D160.
   One LU-decomposition step of CubicSpline_SolveCoefficientSystem, in place on the 32x32 matrix M:
   M[row][column] = (M[row][column] - sum over k = 0..lastPriorIndex of M[row][k] * M[k][column]) / pivot.
   With pivot 1 this yields an element of U, with the diagonal element of U as pivot an element of L.
*/
void CubicSpline_ForwardEliminateColumn
          (float pivot,CubicSplineMatrixIndex lastPriorIndex,CubicSplineMatrixIndex columnIndex,
          CubicSplineMatrixIndex rowIndex,float *matrix32x32)

{
  float *targetElement;
  float reducedValue;
  int priorIndex;
  float *columnCursor;
  float *rowCursor;

  rowCursor = matrix32x32 + rowIndex * CUBIC_SPLINE_MATRIX_ORDER;
  priorIndex = 0;
  targetElement = rowCursor + columnIndex;
  columnCursor = matrix32x32 + columnIndex;
  reducedValue = *targetElement;
  if (lastPriorIndex < 0x80000000) { /* signed lastPriorIndex >= 0; -1 on the first row means no terms */
    do {
      reducedValue = reducedValue - *rowCursor * *columnCursor;
      priorIndex++;
      rowCursor++;
      columnCursor = columnCursor + CUBIC_SPLINE_MATRIX_ORDER;
    } while (priorIndex <= (int)lastPriorIndex);
  }
  *targetElement = reducedValue / pivot;
  return;
}


/* Address: 0x0053D1C0.
   One substitution step of CubicSpline_SolveCoefficientSystem on the right-hand side b with the 32x32 matrix M:
   b[target] = (b[target] - sum over k = firstSolvedIndex..lastSolvedIndex of M[target][k] * b[k]) / pivot.
   Used with pivot 1 for the forward (unit-L) pass and with the diagonal of U for the back substitution.
*/
void CubicSpline_BackSubstituteRow(float pivot,CubicSplineMatrixIndex lastSolvedIndex,
          CubicSplineMatrixIndex firstSolvedIndex,CubicSplineMatrixIndex targetIndex,
          float *rhsVector,float *matrix32x32)

{
  float *solvedRhsCursor;
  float *matrixCoefficientCursor;
  float targetSolutionValue;

  solvedRhsCursor = rhsVector + firstSolvedIndex;
  matrixCoefficientCursor = matrix32x32 + firstSolvedIndex + targetIndex * CUBIC_SPLINE_MATRIX_ORDER;
  targetSolutionValue = rhsVector[targetIndex];
  for (; (int)firstSolvedIndex <= (int)lastSolvedIndex; firstSolvedIndex++) {
    targetSolutionValue = targetSolutionValue - *matrixCoefficientCursor * *solvedRhsCursor;
    matrixCoefficientCursor++;
    solvedRhsCursor++;
  }
  rhsVector[targetIndex] = targetSolutionValue / pivot;
  return;
}


/* Address: 0x0053CA10.
   Zeroes the six derivatives cached by the world-motion spline evaluators, so a finished or newly built
   spline reports no motion.
*/
void WorldMotionSpline_ClearCachedDerivatives(void)

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


/* Address: 0x0053D2E0.
   Value of one solved spline segment at a Q12 time: the segment's four float coefficients a + b*t +
   c*t^2 + d*t^3 are evaluated (Horner) at t = time / 4096 and the result is rounded back to Q12.
*/
int32_t CubicSpline_EvaluateValueQ12
                 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex,
                 float *coefficients)

{
  float *segmentCoefficients;
  float splineTime;

  splineTime = (float)timeQ12 / g_Q12FloatScale4096;
  segmentCoefficients = coefficients + segmentIndex * 4;
  return (int)ROUND((((splineTime * segmentCoefficients[3] +
                      segmentCoefficients[2]) * splineTime +
                     segmentCoefficients[1]) * splineTime + *segmentCoefficients
                    ) * g_Q12FloatScale4096);
}

/* Address: 0x0053D320.
   First derivative b + 2c*t + 3d*t^2 of one spline segment at a Q12 time. Unlike the value it stays a
   float (per 1.0 = 4096 units of Q12 time), which the evaluators cache per channel as the current motion.
*/
float CubicSpline_EvaluateDerivativeQ12
                (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex,
                float *coefficients)

{
  float cubicTimesTime;
  float partialSum;

  cubicTimesTime = ((float)timeQ12 / g_Q12FloatScale4096) * coefficients[segmentIndex * 4 + 3];
  partialSum = cubicTimesTime + coefficients[segmentIndex * 4 + 2];
  /* (2 * (d*t + c) + d*t) * t + b */
  return (partialSum + partialSum + cubicTimesTime) * ((float)timeQ12 / g_Q12FloatScale4096) +
         coefficients[segmentIndex * 4 + 1];
}

