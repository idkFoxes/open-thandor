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
bool __thandor_cf_preserve_eax_ecx_edx
WorldMotionSpline_EvaluateAndApplyAtTime
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
      WorldRuntime_SetPosition60AndDistanceFromPosition80(positionZ,positionY,positionX,runtimeCopy);
      runtimeCopy = worldRuntime;
      magnitude = CubicSpline_EvaluateValueQ12
                            (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[3]);
      yawAngle = CubicSpline_EvaluateValueQ12
                        (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[4]);
      yawAngle = yawAngle & 0xffff; /* 16-bit angle: wrap to one turn */
      pitchAngle = CubicSpline_EvaluateValueQ12
                             (timeQ12,keyframeIndex - 1,g_WorldMotionSplineCoefficientTables[5]);
      WorldRuntime_SetMotionParameters6CThrough78Clamped
                ((worldRuntime->motion).motionValue78,pitchAngle,yawAngle,magnitude,runtimeCopy);
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
  WorldRuntime_SetPosition60AndDistanceFromPosition80
            (currentKeyframe->channel2Q12,currentKeyframe->channel1Q12,currentKeyframe->channel0Q12,worldRuntime);
  WorldRuntime_SetMotionParameters6CThrough78Clamped
            ((worldRuntime->motion).motionValue78,currentKeyframe->channel5Q12,yawAngle & 0xffff,
             currentKeyframe->channel3Q12,worldRuntime);
  WorldMotionSpline_ClearCachedDerivatives();
  return false;
}


/* Address: 0x0053CBB0.
   Ownership: core/math/interpolation.
   Purpose: Handles world motion spline evaluate and apply origin distance at time carry-flag result.
   Local calls: CubicSpline_EvaluateValueQ12, CubicSpline_EvaluateDerivativeQ12,
   WorldMotionSpline_ClearCachedDerivatives.
   Cross-module calls: WorldRuntime_SetPosition80AndRebuildPosition60FromAngles [world/runtime/core].
*/
uint8_t __thandor_cf_preserve_eax_ecx_edx
WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTime
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
                          (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[0]);
      originY = CubicSpline_EvaluateValueQ12
                          (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[1]);
      originZ = CubicSpline_EvaluateValueQ12
                          (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[2]);
      distance = CubicSpline_EvaluateValueQ12
                           (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[3]);
      yawAngle = CubicSpline_EvaluateValueQ12
                        (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[4]);
      yawAngle = yawAngle & 0xffff;
      pitchAngle = CubicSpline_EvaluateValueQ12
                             (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[5]);
      WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                (pitchAngle,yawAngle,distance,originZ,originY,originX,worldRuntime);
      g_WorldMotionSplineCachedDerivatives[0] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[0]);
      g_WorldMotionSplineCachedDerivatives[1] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[1]);
      g_WorldMotionSplineCachedDerivatives[2] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[2]);
      g_WorldMotionSplineCachedDerivatives[3] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[3]);
      g_WorldMotionSplineCachedDerivatives[4] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[4]);
      g_WorldMotionSplineCachedDerivatives[5] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,keyframeIndex + -1,g_WorldMotionSplineCoefficientTables[5]);
      return 1;
    }
    keyframeIndex = keyframeIndex + 1;
    keyframeCount = keyframeCount + -1;
    keyframes = currentKeyframe + 1;
  } while (keyframeCount != 0);
  WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
            (currentKeyframe->channel5Q12,currentKeyframe->channel4Q12 & 0xffff,currentKeyframe->channel3Q12,
             currentKeyframe->channel2Q12,currentKeyframe->channel1Q12,currentKeyframe->channel0Q12,worldRuntime);
  WorldMotionSpline_ClearCachedDerivatives();
  return 0;
}


/* Address: 0x0053CD10.
   Ownership: core/math/interpolation.
   Purpose: Builds and solves six cubic channels for an exact 0x20-byte keyframe array. RET 8 proves two stack
   arguments. Role: Unwraps the angle-like channel and builds/solves six natural cubic splines. Inputs: Keyframe
   count and array of 0x20-byte WorldMotionSplineKeyframe records. Outputs: Six coefficient tables used by runtime
   evaluation.
   Local calls: CubicSpline_BuildNaturalCoefficientSystem, CubicSpline_SolveCoefficientSystem.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldMotionSpline_BuildSixChannelCurves
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes)

{
  WorldMotionSplineKeyframe *keyframeCursor;
  uint32_t unwrappedAngle;
  int remainingCount;
  uint32_t previousAngle;
  int unwrapDelta;
  
  if (1 < keyframeCount) {
    unwrappedAngle = keyframes->channel4Q12;
    remainingCount = keyframeCount + -1;
    previousAngle = unwrappedAngle;
    keyframeCursor = keyframes;
    do {
      unwrapDelta = (keyframeCursor[1].channel4Q12 & 0xffffU) - (previousAngle & 0xffff);
      if (0x8000 < unwrapDelta) {
        unwrapDelta = unwrapDelta + -0x10000;
      }
      if (unwrapDelta < -0x8000) {
        unwrapDelta = unwrapDelta + 0x10000;
      }
      unwrappedAngle = unwrappedAngle + unwrapDelta;
      previousAngle = (previousAngle & 0xffff) + unwrapDelta;
      keyframeCursor[1].channel4Q12 = unwrappedAngle;
      remainingCount = remainingCount + -1;
      keyframeCursor = keyframeCursor + 1;
    } while (remainingCount != 0);
  }
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
             keyframeCount,0xc,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[4],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 4),
             g_WorldMotionSplineCoefficientTables[4],g_WorldMotionSplineMatrixWorkspaces[4],
             keyframeCount,0x10,keyframes);
  CubicSpline_BuildNaturalCoefficientSystem
            (g_WorldMotionSplineCachedDerivatives[5],
             (CubicSplineEquationCount *)(g_WorldMotionSplineEquationCounts + 5),
             g_WorldMotionSplineCoefficientTables[5],g_WorldMotionSplineMatrixWorkspaces[5],
             keyframeCount,0x14,keyframes);
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
  return;
}


/* Address: 0x004CCC00.
   Ownership: core/math/interpolation.
   Purpose: Stores a negated transition duration in a GraphicsShadingRuntimeRecord or rescales its elapsed
   progress, then clears color/radius state when the transition reaches zero.
*/
void __thandor_void_preserve_eax_ecx_edx
InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount transitionDurationTicks,
          GraphicsShadingRuntimeRecord *interpolationState)

{
  PackedRgb24 negatedDurationOrZero;
  int durationOrElapsed;
  
  if (interpolationState == (GraphicsShadingRuntimeRecord *)0x0) {
    return;
  }
  negatedDurationOrZero = -transitionDurationTicks;
  if (((int)negatedDurationOrZero < 0) && (-1 < interpolationState->radiusTransitionDurationTicks)) {
    if (interpolationState->radiusTransitionDurationTicks == 0) {
      interpolationState->radiusTransitionDurationTicks = negatedDurationOrZero;
      interpolationState->radiusTransitionElapsedTicks = negatedDurationOrZero;
      return;
    }
    LOCK();
    durationOrElapsed = interpolationState->radiusTransitionDurationTicks;
    interpolationState->radiusTransitionDurationTicks = negatedDurationOrZero;
    UNLOCK();
    durationOrElapsed = (int)(((int64_t)(int)negatedDurationOrZero * (int64_t)interpolationState->radiusTransitionElapsedTicks
                  ) / (int64_t)durationOrElapsed);
    interpolationState->radiusTransitionElapsedTicks = durationOrElapsed;
    negatedDurationOrZero = 0;
    if (durationOrElapsed != 0) {
      return;
    }
  }
  *(PackedRgb24 *)((int)&interpolationState->squaredRadiusQ24 + 4) = negatedDurationOrZero;
  *(PackedRgb24 *)&interpolationState->squaredRadiusQ24 = negatedDurationOrZero;
  interpolationState->packedColorRgbActive = negatedDurationOrZero;
  interpolationState->targetRadiusQ12 = negatedDurationOrZero;
  return;
}


/* Address: 0x004CCC80.
   Ownership: core/math/interpolation.
   Purpose: Advances radius transitions for all 256 GraphicsShadingRuntimeRecord entries, recomputes
   squaredRadiusQ24, and clears expired active/color/radius state.
*/
void __thandor_void_preserve_eax_ecx_edx
InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks)

{
  int durationTicks;
  int currentRadius;
  int remainingCount;
  GraphicsShadingRuntimeRecord *stateRecord;
  
  stateRecord = g_GraphicsShadingRuntimeRecords;
  remainingCount = 0x100;
  do {
    durationTicks = stateRecord->radiusTransitionDurationTicks;
    if ((stateRecord->packedColorRgbActive != 0) && (durationTicks != 0)) {
      currentRadius = (int)(((int64_t)stateRecord->targetRadiusQ12 *
                    (int64_t)stateRecord->radiusTransitionElapsedTicks) / (int64_t)durationTicks);
      stateRecord->radiusTransitionElapsedTicks = stateRecord->radiusTransitionElapsedTicks + elapsedTicks;
      stateRecord->squaredRadiusQ24 = (int64_t)currentRadius * (int64_t)currentRadius;
      if (durationTicks < 0) {
        if ((uint32_t)stateRecord->radiusTransitionElapsedTicks < 0x80000000) {
          /* deactivation transition finished: clear the light */
          stateRecord->squaredRadiusQ24 = 0;
          stateRecord->targetRadiusQ12 = 0;
          stateRecord->packedColorRgbActive = 0;
          stateRecord->radiusTransitionElapsedTicks = 0;
          stateRecord->radiusTransitionDurationTicks = 0;
        }
      }
      else if (durationTicks < stateRecord->radiusTransitionElapsedTicks) {
        /* transition complete */
        stateRecord->radiusTransitionElapsedTicks = 0;
        stateRecord->radiusTransitionDurationTicks = 0;
      }
    }
    stateRecord = stateRecord + 1;
    remainingCount = remainingCount + -1;
    if (remainingCount == 0) {
      return;
    }
  } while( true );
}


/* Address: 0x0053D230.
   Ownership: core/math/interpolation.
   Purpose: Solves the cubic coefficient system. RET 0x0C proves three stack arguments and removes the false
   fastcall stack-spacebase model. Role: Runs forward elimination and back substitution for one channel. Inputs:
   Natural-cubic coefficient system and output coefficient table. Outputs: Solved segment coefficients.
   Local calls: CubicSpline_ForwardEliminateColumn, CubicSpline_BackSubstituteRow.
*/
void __thandor_void_preserve_eax_ecx_edx
CubicSpline_SolveCoefficientSystem
          (CubicSplineEquationCount equationCount,float *rhsVector,float *matrix32x32)

{
  float pivot;
  CubicSplineMatrixIndex rowIndex;
  CubicSplineMatrixIndex columnOrRowIndex;
  uint32_t followingIndex;
  uint32_t nextRowIndex;
  
  rowIndex = 0;
  columnOrRowIndex = 0;
  do {
    do {
      followingIndex = columnOrRowIndex + 1;
      CubicSpline_ForwardEliminateColumn(1.0,rowIndex - 1,columnOrRowIndex,rowIndex,matrix32x32);
      columnOrRowIndex = followingIndex;
    } while (followingIndex < equationCount);
    CubicSpline_BackSubstituteRow(1.0,rowIndex - 1,0,rowIndex,rhsVector,matrix32x32);
    if (rowIndex + 1 < equationCount) {
      pivot = matrix32x32[rowIndex * 0x21];
      followingIndex = rowIndex + 1;
      do {
        nextRowIndex = followingIndex + 1;
        CubicSpline_ForwardEliminateColumn(pivot,rowIndex - 1,rowIndex,followingIndex,matrix32x32);
        followingIndex = nextRowIndex;
      } while (nextRowIndex < equationCount);
    }
    rowIndex = rowIndex + 1;
    columnOrRowIndex = rowIndex;
  } while (rowIndex < equationCount);
  columnOrRowIndex = equationCount - 1;
  do {
    CubicSpline_BackSubstituteRow
              (matrix32x32[columnOrRowIndex * 0x21],equationCount - 1,columnOrRowIndex + 1,columnOrRowIndex,rhsVector,matrix32x32);
    columnOrRowIndex = columnOrRowIndex - 1;
  } while (-1 < (int)columnOrRowIndex);
  return;
}


/* Address: 0x0053CF10.
   Ownership: core/math/interpolation.
   Purpose: Builds one natural cubic-spline coefficient system. RET 0x1C proves seven stack arguments. Role: Builds
   the tridiagonal coefficient system for one Q12 keyframe channel. Inputs: Keyframe times, selected channel offset
   and output work arrays. Outputs: Natural cubic spline matrix/right-hand-side data.
*/
void __thandor_void_preserve_eax_ecx_edx
CubicSpline_BuildNaturalCoefficientSystem
          (float endpointDerivative,CubicSplineEquationCount *outEquationCount,
          float *outCoefficients,float *matrix32x32,WorldMotionSplineKeyframeCount keyframeCount,
          WorldMotionSplineChannelByteOffset channelByteOffset,WorldMotionSplineKeyframe *keyframes)

{
  float knotTimeOrTerm;
  int remainingCount;
  int *endValueCursor;
  int *startValueCursor;
  WorldMotionSplineKeyframe *keyframeCursor;
  float *floatCursor;
  
  endValueCursor = (int *)((int)&keyframes->channel0Q12 + channelByteOffset);
  *outEquationCount = keyframeCount * 4 - 4;
  floatCursor = matrix32x32;
  for (remainingCount = 0x400; remainingCount != 0; remainingCount = remainingCount + -1) {
    *floatCursor = 0.0;
    floatCursor = floatCursor + 1;
  }
  remainingCount = keyframeCount + -1;
  keyframeCursor = keyframes;
  floatCursor = matrix32x32;
  do {
    knotTimeOrTerm = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
    *floatCursor = 1.0;
    floatCursor[1] = knotTimeOrTerm;
    floatCursor[2] = knotTimeOrTerm * knotTimeOrTerm;
    floatCursor[3] = knotTimeOrTerm * knotTimeOrTerm * knotTimeOrTerm;
    keyframeCursor = keyframeCursor + 1;
    floatCursor = floatCursor + 0x84;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  remainingCount = keyframeCount + -1;
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  do {
    knotTimeOrTerm = (float)keyframeCursor[1].timeQ12 / g_Q12FloatScale4096;
    floatCursor[0x60] = 1.0;
    floatCursor[0x61] = knotTimeOrTerm;
    floatCursor[0x62] = knotTimeOrTerm * knotTimeOrTerm;
    floatCursor[99] = knotTimeOrTerm * knotTimeOrTerm * knotTimeOrTerm;
    floatCursor = floatCursor + 0x84;
    remainingCount = remainingCount + -1;
    keyframeCursor = keyframeCursor + 1;
  } while (remainingCount != 0);
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  for (remainingCount = keyframeCount + -2; remainingCount != 0; remainingCount = remainingCount + -1) {
    knotTimeOrTerm = (float)keyframeCursor[1].timeQ12 / g_Q12FloatScale4096;
    floatCursor[0x41] = 1.0;
    floatCursor[0x45] = -1.0;
    floatCursor[0x42] = knotTimeOrTerm + knotTimeOrTerm;
    floatCursor[0x46] = -(knotTimeOrTerm + knotTimeOrTerm);
    knotTimeOrTerm = knotTimeOrTerm * knotTimeOrTerm;
    knotTimeOrTerm = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
    floatCursor[0x43] = knotTimeOrTerm;
    floatCursor[0x47] = -knotTimeOrTerm;
    floatCursor = floatCursor + 0x84;
    keyframeCursor = keyframeCursor + 1;
  }
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  for (remainingCount = keyframeCount + -2; keyframeCursor = keyframeCursor + 1, remainingCount != 0; remainingCount = remainingCount + -1) {
    knotTimeOrTerm = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
    floatCursor[0xa6] = 2.0;
    floatCursor[0xa2] = -2.0;
    knotTimeOrTerm = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
    knotTimeOrTerm = knotTimeOrTerm + knotTimeOrTerm;
    floatCursor[0xa7] = knotTimeOrTerm;
    floatCursor[0xa3] = -knotTimeOrTerm;
    floatCursor = floatCursor + 0x84;
  }
  knotTimeOrTerm = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
  floatCursor[0x41] = 1.0;
  floatCursor[0x42] = knotTimeOrTerm + knotTimeOrTerm;
  knotTimeOrTerm = knotTimeOrTerm * knotTimeOrTerm;
  floatCursor[0x43] = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
  knotTimeOrTerm = (float)keyframes->timeQ12 / g_Q12FloatScale4096;
  matrix32x32[0x21] = 1.0;
  matrix32x32[0x22] = knotTimeOrTerm + knotTimeOrTerm;
  knotTimeOrTerm = knotTimeOrTerm * knotTimeOrTerm;
  matrix32x32[0x23] = knotTimeOrTerm + knotTimeOrTerm + knotTimeOrTerm;
  remainingCount = keyframeCount + -1;
  startValueCursor = endValueCursor;
  floatCursor = outCoefficients;
  do {
    *floatCursor = (float)*startValueCursor / g_Q12FloatScale4096;
    floatCursor[1] = 0.0;
    floatCursor[2] = 0.0;
    startValueCursor = startValueCursor + 8;
    floatCursor = floatCursor + 4;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  remainingCount = keyframeCount + -1;
  floatCursor = outCoefficients;
  do {
    endValueCursor = endValueCursor + 8;
    floatCursor[3] = (float)*endValueCursor / g_Q12FloatScale4096;
    floatCursor = floatCursor + 4;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  outCoefficients[1] = endpointDerivative;
  return;
}


/* Address: 0x0053D160.
   Ownership: core/math/interpolation.
   Purpose: Performs one forward-elimination column step. RET 0x14 proves five stack arguments. Role: Performs one
   forward-elimination step over the spline coefficient system. Inputs: Coefficient matrix/work column and row
   bounds. Outputs: Partially reduced system.
*/
void __thandor_void_preserve_ecx_edx
CubicSpline_ForwardEliminateColumn
          (float pivot,CubicSplineMatrixIndex lastPriorIndex,CubicSplineMatrixIndex columnIndex,
          CubicSplineMatrixIndex rowIndex,float *matrix32x32)

{
  float *targetElement;
  float reducedValue;
  int priorIndex;
  float *columnCursor;
  float *rowCursor;
  
  rowCursor = matrix32x32 + rowIndex * 0x20;
  priorIndex = 0;
  targetElement = rowCursor + columnIndex;
  columnCursor = matrix32x32 + columnIndex;
  reducedValue = *targetElement;
  if (lastPriorIndex < 0x80000000) {
    do {
      reducedValue = reducedValue - *rowCursor * *columnCursor;
      priorIndex = priorIndex + 1;
      rowCursor = rowCursor + 1;
      columnCursor = columnCursor + 0x20;
    } while (priorIndex <= (int)lastPriorIndex);
  }
  *targetElement = reducedValue / pivot;
  return;
}


/* Address: 0x0053D1C0.
   Ownership: core/math/interpolation.
   Purpose: Performs one back-substitution row step. RET 0x18 proves six stack arguments. Role: Performs one back-
   substitution step over the spline coefficient system. Inputs: Reduced matrix/work row and solved tail values.
   Outputs: Solved coefficient row.
*/
void __thandor_void_preserve_ecx_edx
CubicSpline_BackSubstituteRow
          (float pivot,CubicSplineMatrixIndex lastSolvedIndex,
          CubicSplineMatrixIndex firstSolvedIndex,CubicSplineMatrixIndex targetIndex,
          float *rhsVector,float *matrix32x32)

{
  float *solvedRhsCursor;
  float *matrixCoefficientCursor;
  float targetSolutionValue;
  
  solvedRhsCursor = rhsVector + firstSolvedIndex;
  matrixCoefficientCursor = matrix32x32 + firstSolvedIndex + targetIndex * 0x20;
  targetSolutionValue = rhsVector[targetIndex];
  for (; (int)firstSolvedIndex <= (int)lastSolvedIndex; firstSolvedIndex = firstSolvedIndex + 1) {
    targetSolutionValue = targetSolutionValue - *matrixCoefficientCursor * *solvedRhsCursor;
    matrixCoefficientCursor = matrixCoefficientCursor + 1;
    solvedRhsCursor = solvedRhsCursor + 1;
  }
  rhsVector[targetIndex] = targetSolutionValue / pivot;
  return;
}


/* Address: 0x0053CA10.
   Zeroes the six derivatives cached by the world-motion spline evaluators, so a finished or newly built
   spline reports no motion.
*/
void __thandor_void_preserve_eax_ecx WorldMotionSpline_ClearCachedDerivatives(void)

{
  int derivativesRemaining;
  float *derivativeCursor;

  derivativeCursor = g_WorldMotionSplineCachedDerivatives;
  for (derivativesRemaining = 6; derivativesRemaining != 0; derivativesRemaining--) {
    *derivativeCursor = 0.0;
    derivativeCursor++;
  }
  return;
}


/* Address: 0x0053D2E0.
   Ownership: core/math/interpolation.
   Purpose: Evaluates one cubic segment and returns a Q12 scalar. RET 0x0C proves three stack arguments. Role:
   Evaluates one cubic segment value at Q12 time. Inputs: Time, segment index and solved coefficient table.
   Outputs: Q12 channel value.
*/
int32_t CubicSpline_EvaluateValueQ12
                 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex,
                 float *coefficients)

{
  float *segmentCoefficientCursor;
  float normalizedSplineTime;
  
  normalizedSplineTime = (float)timeQ12 / g_Q12FloatScale4096;
  segmentCoefficientCursor = coefficients + segmentIndex * 4;
  return (int)ROUND((((normalizedSplineTime * segmentCoefficientCursor[3] +
                      segmentCoefficientCursor[2]) * normalizedSplineTime +
                     segmentCoefficientCursor[1]) * normalizedSplineTime + *segmentCoefficientCursor
                    ) * g_Q12FloatScale4096);
}

/* Address: 0x0053D320.
   Ownership: core/math/interpolation.
   Purpose: Evaluates the first derivative of one cubic segment. RET 0x0C proves three stack arguments. Role:
   Evaluates one cubic segment derivative at Q12 time. Inputs: Time, segment index and solved coefficient table.
   Outputs: Floating derivative cached per channel.
*/
float CubicSpline_EvaluateDerivativeQ12
                (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex,
                float *coefficients)

{
  float scaledCubicTerm;
  float partialSum;
  
  scaledCubicTerm = ((float)timeQ12 / g_Q12FloatScale4096) * coefficients[segmentIndex * 4 + 3];
  partialSum = scaledCubicTerm + coefficients[segmentIndex * 4 + 2];
  return (partialSum + partialSum + scaledCubicTerm) * ((float)timeQ12 / g_Q12FloatScale4096) +
         coefficients[segmentIndex * 4 + 1];
}

