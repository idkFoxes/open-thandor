/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/interpolation.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/interpolation.h>

/* Implementation ownership: core/math/interpolation. */

/* Address: 0x0053CA30.
   Ownership: core/math/interpolation.
   Purpose: Evaluates six world-motion cubic channels and applies them to one world runtime. RET 0x10 proves four
   stack arguments. Role: Evaluates six cubic channels at a Q12 time and applies position/motion parameters to the
   world runtime. Inputs: 0x20-byte keyframes, coefficient tables, keyframe count and Q12 time. Outputs: World
   position from channels 0..2, motion parameters from 3..5, plus six cached derivatives.
   Local calls: CubicSpline_EvaluateValueQ12, CubicSpline_EvaluateDerivativeQ12,
   WorldMotionSpline_ClearCachedDerivatives.
   Cross-module calls: WorldRuntime_SetPosition60AndDistanceFromPosition80 [world/runtime/core],
   WorldRuntime_SetMotionParameters6CThrough78Clamped [world/runtime/core].
*/
bool __thandor_cf_preserve_eax_ecx_edx
WorldMotionSpline_EvaluateAndApplyAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime)

{
  sdword positionX;
  sdword positionY;
  sdword positionZ;
  UQ12 magnitude;
  uint uVar1;
  AngleTurn32 pitchAngle;
  int iVar2;
  CubicSplineSegmentIndex segmentIndex;
  CubicSplineSegmentIndex segmentIndex_00;
  CubicSplineSegmentIndex segmentIndex_01;
  CubicSplineSegmentIndex segmentIndex_02;
  CubicSplineSegmentIndex segmentIndex_03;
  CubicSplineSegmentIndex segmentIndex_04;
  CubicSplineSegmentIndex segmentIndex_05;
  CubicSplineSegmentIndex segmentIndex_06;
  CubicSplineSegmentIndex segmentIndex_07;
  CubicSplineSegmentIndex segmentIndex_08;
  CubicSplineSegmentIndex segmentIndex_09;
  WorldMotionSplineKeyframe *pWVar3;
  WorldRuntimeContext *pWVar4;
  
  iVar2 = 0;
  do {
    pWVar3 = keyframes;
    if ((uint)timeQ12 < (uint)pWVar3->timeQ12) {
      pWVar4 = worldRuntime;
      positionX = CubicSpline_EvaluateValueQ12
                            (timeQ12,iVar2 + -1,g_WorldMotionSplineCoefficientTables[0]);
      positionY = CubicSpline_EvaluateValueQ12
                            (timeQ12,segmentIndex,g_WorldMotionSplineCoefficientTables[1]);
      positionZ = CubicSpline_EvaluateValueQ12
                            (timeQ12,segmentIndex_00,g_WorldMotionSplineCoefficientTables[2]);
      WorldRuntime_SetPosition60AndDistanceFromPosition80(positionZ,positionY,positionX,pWVar4);
      pWVar4 = worldRuntime;
      magnitude = CubicSpline_EvaluateValueQ12
                            (timeQ12,segmentIndex_01,g_WorldMotionSplineCoefficientTables[3]);
      uVar1 = CubicSpline_EvaluateValueQ12
                        (timeQ12,segmentIndex_02,g_WorldMotionSplineCoefficientTables[4]);
      uVar1 = uVar1 & 0xffff;
      pitchAngle = CubicSpline_EvaluateValueQ12
                             (timeQ12,segmentIndex_03,g_WorldMotionSplineCoefficientTables[5]);
      WorldRuntime_SetMotionParameters6CThrough78Clamped
                ((worldRuntime->motion).motionValue78,pitchAngle,uVar1,magnitude,pWVar4);
      g_WorldMotionSplineCachedDerivatives[0] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_04,g_WorldMotionSplineCoefficientTables[0]);
      g_WorldMotionSplineCachedDerivatives[1] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_05,g_WorldMotionSplineCoefficientTables[1]);
      g_WorldMotionSplineCachedDerivatives[2] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_06,g_WorldMotionSplineCoefficientTables[2]);
      g_WorldMotionSplineCachedDerivatives[3] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_07,g_WorldMotionSplineCoefficientTables[3]);
      g_WorldMotionSplineCachedDerivatives[4] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_08,g_WorldMotionSplineCoefficientTables[4]);
      g_WorldMotionSplineCachedDerivatives[5] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_09,g_WorldMotionSplineCoefficientTables[5]);
      return true;
    }
    iVar2 = iVar2 + 1;
    keyframeCount = keyframeCount + -1;
    keyframes = pWVar3 + 1;
  } while (keyframeCount != 0);
  uVar1 = pWVar3->channel4Q12;
  WorldRuntime_SetPosition60AndDistanceFromPosition80
            (pWVar3->channel2Q12,pWVar3->channel1Q12,pWVar3->channel0Q12,worldRuntime);
  WorldRuntime_SetMotionParameters6CThrough78Clamped
            ((worldRuntime->motion).motionValue78,pWVar3->channel5Q12,uVar1 & 0xffff,
             pWVar3->channel3Q12,worldRuntime);
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
byte __thandor_cf_preserve_eax_ecx_edx
WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime)

{
  sdword originX;
  sdword originY;
  sdword originZ;
  UQ12 distance;
  uint uVar1;
  AngleTurn32 pitchAngle;
  int iVar2;
  CubicSplineSegmentIndex segmentIndex;
  CubicSplineSegmentIndex segmentIndex_00;
  CubicSplineSegmentIndex segmentIndex_01;
  CubicSplineSegmentIndex segmentIndex_02;
  CubicSplineSegmentIndex segmentIndex_03;
  CubicSplineSegmentIndex segmentIndex_04;
  CubicSplineSegmentIndex segmentIndex_05;
  CubicSplineSegmentIndex segmentIndex_06;
  CubicSplineSegmentIndex segmentIndex_07;
  CubicSplineSegmentIndex segmentIndex_08;
  CubicSplineSegmentIndex segmentIndex_09;
  WorldMotionSplineKeyframe *pWVar3;
  
  iVar2 = 0;
  do {
    pWVar3 = keyframes;
    if ((uint)timeQ12 < (uint)pWVar3->timeQ12) {
      originX = CubicSpline_EvaluateValueQ12
                          (timeQ12,iVar2 + -1,g_WorldMotionSplineCoefficientTables[0]);
      originY = CubicSpline_EvaluateValueQ12
                          (timeQ12,segmentIndex,g_WorldMotionSplineCoefficientTables[1]);
      originZ = CubicSpline_EvaluateValueQ12
                          (timeQ12,segmentIndex_00,g_WorldMotionSplineCoefficientTables[2]);
      distance = CubicSpline_EvaluateValueQ12
                           (timeQ12,segmentIndex_01,g_WorldMotionSplineCoefficientTables[3]);
      uVar1 = CubicSpline_EvaluateValueQ12
                        (timeQ12,segmentIndex_02,g_WorldMotionSplineCoefficientTables[4]);
      uVar1 = uVar1 & 0xffff;
      pitchAngle = CubicSpline_EvaluateValueQ12
                             (timeQ12,segmentIndex_03,g_WorldMotionSplineCoefficientTables[5]);
      WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                (pitchAngle,uVar1,distance,originZ,originY,originX,worldRuntime);
      g_WorldMotionSplineCachedDerivatives[0] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_04,g_WorldMotionSplineCoefficientTables[0]);
      g_WorldMotionSplineCachedDerivatives[1] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_05,g_WorldMotionSplineCoefficientTables[1]);
      g_WorldMotionSplineCachedDerivatives[2] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_06,g_WorldMotionSplineCoefficientTables[2]);
      g_WorldMotionSplineCachedDerivatives[3] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_07,g_WorldMotionSplineCoefficientTables[3]);
      g_WorldMotionSplineCachedDerivatives[4] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_08,g_WorldMotionSplineCoefficientTables[4]);
      g_WorldMotionSplineCachedDerivatives[5] =
           CubicSpline_EvaluateDerivativeQ12
                     (timeQ12,segmentIndex_09,g_WorldMotionSplineCoefficientTables[5]);
      return 1;
    }
    iVar2 = iVar2 + 1;
    keyframeCount = keyframeCount + -1;
    keyframes = pWVar3 + 1;
  } while (keyframeCount != 0);
  WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
            (pWVar3->channel5Q12,pWVar3->channel4Q12 & 0xffff,pWVar3->channel3Q12,
             pWVar3->channel2Q12,pWVar3->channel1Q12,pWVar3->channel0Q12,worldRuntime);
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
  WorldMotionSplineKeyframe *pWVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  if (1 < keyframeCount) {
    uVar2 = keyframes->channel4Q12;
    iVar3 = keyframeCount + -1;
    uVar4 = uVar2;
    pWVar1 = keyframes;
    do {
      iVar5 = (pWVar1[1].channel4Q12 & 0xffffU) - (uVar4 & 0xffff);
      if (0x8000 < iVar5) {
        iVar5 = iVar5 + -0x10000;
      }
      if (iVar5 < -0x8000) {
        iVar5 = iVar5 + 0x10000;
      }
      uVar2 = uVar2 + iVar5;
      uVar4 = (uVar4 & 0xffff) + iVar5;
      pWVar1[1].channel4Q12 = uVar2;
      iVar3 = iVar3 + -1;
      pWVar1 = pWVar1 + 1;
    } while (iVar3 != 0);
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
  PackedRgb24 PVar1;
  int iVar2;
  
  if (interpolationState == (GraphicsShadingRuntimeRecord *)0x0) {
    return;
  }
  PVar1 = -transitionDurationTicks;
  if (((int)PVar1 < 0) && (-1 < interpolationState->radiusTransitionDurationTicks)) {
    if (interpolationState->radiusTransitionDurationTicks == 0) {
      interpolationState->radiusTransitionDurationTicks = PVar1;
      interpolationState->radiusTransitionElapsedTicks = PVar1;
      return;
    }
    LOCK();
    iVar2 = interpolationState->radiusTransitionDurationTicks;
    interpolationState->radiusTransitionDurationTicks = PVar1;
    UNLOCK();
    iVar2 = (int)(((longlong)(int)PVar1 * (longlong)interpolationState->radiusTransitionElapsedTicks
                  ) / (longlong)iVar2);
    interpolationState->radiusTransitionElapsedTicks = iVar2;
    PVar1 = 0;
    if (iVar2 != 0) {
      return;
    }
  }
  *(PackedRgb24 *)((int)&interpolationState->squaredRadiusQ24 + 4) = PVar1;
  *(PackedRgb24 *)&interpolationState->squaredRadiusQ24 = PVar1;
  interpolationState->packedColorRgbActive = PVar1;
  interpolationState->targetRadiusQ12 = PVar1;
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
  int iVar1;
  int iVar2;
  int iVar3;
  GraphicsShadingRuntimeRecord *pGVar4;
  
  pGVar4 = g_GraphicsShadingRuntimeRecords;
  iVar3 = 0x100;
  do {
    iVar1 = pGVar4->radiusTransitionDurationTicks;
    if ((pGVar4->packedColorRgbActive != 0) && (iVar1 != 0)) {
      iVar2 = (int)(((longlong)pGVar4->targetRadiusQ12 *
                    (longlong)pGVar4->radiusTransitionElapsedTicks) / (longlong)iVar1);
      pGVar4->radiusTransitionElapsedTicks = pGVar4->radiusTransitionElapsedTicks + elapsedTicks;
      pGVar4->squaredRadiusQ24 = (longlong)iVar2 * (longlong)iVar2;
      if (iVar1 < 0) {
        if ((uint)pGVar4->radiusTransitionElapsedTicks < 0x80000000) {
          *(undefined4 *)((int)&pGVar4->squaredRadiusQ24 + 4) = 0;
          *(undefined4 *)&pGVar4->squaredRadiusQ24 = 0;
          pGVar4->targetRadiusQ12 = 0;
          pGVar4->packedColorRgbActive = 0;
InterpolationStateTable_Advance256ByTicks_ClearTransitionTimingAfterCompletionOrDeactivation:
          pGVar4->radiusTransitionElapsedTicks = 0;
          pGVar4->radiusTransitionDurationTicks = 0;
        }
      }
      else if (iVar1 < pGVar4->radiusTransitionElapsedTicks)
      goto 
      InterpolationStateTable_Advance256ByTicks_ClearTransitionTimingAfterCompletionOrDeactivation;
    }
    pGVar4 = pGVar4 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
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
  CubicSplineMatrixIndex CVar1;
  uint uVar2;
  uint uVar3;
  
  rowIndex = 0;
  CVar1 = 0;
  do {
    do {
      uVar2 = CVar1 + 1;
      CubicSpline_ForwardEliminateColumn(1.0,rowIndex - 1,CVar1,rowIndex,matrix32x32);
      CVar1 = uVar2;
    } while (uVar2 < equationCount);
    CubicSpline_BackSubstituteRow(1.0,rowIndex - 1,0,rowIndex,rhsVector,matrix32x32);
    if (rowIndex + 1 < equationCount) {
      pivot = matrix32x32[rowIndex * 0x21];
      uVar2 = rowIndex + 1;
      do {
        uVar3 = uVar2 + 1;
        CubicSpline_ForwardEliminateColumn(pivot,rowIndex - 1,rowIndex,uVar2,matrix32x32);
        uVar2 = uVar3;
      } while (uVar3 < equationCount);
    }
    rowIndex = rowIndex + 1;
    CVar1 = rowIndex;
  } while (rowIndex < equationCount);
  CVar1 = equationCount - 1;
  do {
    CubicSpline_BackSubstituteRow
              (matrix32x32[CVar1 * 0x21],equationCount - 1,CVar1 + 1,CVar1,rhsVector,matrix32x32);
    CVar1 = CVar1 - 1;
  } while (-1 < (int)CVar1);
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
  float fVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  WorldMotionSplineKeyframe *pWVar5;
  float *pfVar6;
  
  piVar3 = (int *)((int)&keyframes->channel0Q12 + channelByteOffset);
  *outEquationCount = keyframeCount * 4 - 4;
  pfVar6 = matrix32x32;
  for (iVar2 = 0x400; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pfVar6 = 0.0;
    pfVar6 = pfVar6 + 1;
  }
  iVar2 = keyframeCount + -1;
  pWVar5 = keyframes;
  pfVar6 = matrix32x32;
  do {
    fVar1 = (float)pWVar5->timeQ12 / g_Q12FloatScale4096;
    *pfVar6 = 1.0;
    pfVar6[1] = fVar1;
    pfVar6[2] = fVar1 * fVar1;
    pfVar6[3] = fVar1 * fVar1 * fVar1;
    pWVar5 = pWVar5 + 1;
    pfVar6 = pfVar6 + 0x84;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  iVar2 = keyframeCount + -1;
  pfVar6 = matrix32x32;
  pWVar5 = keyframes;
  do {
    fVar1 = (float)pWVar5[1].timeQ12 / g_Q12FloatScale4096;
    pfVar6[0x60] = 1.0;
    pfVar6[0x61] = fVar1;
    pfVar6[0x62] = fVar1 * fVar1;
    pfVar6[99] = fVar1 * fVar1 * fVar1;
    pfVar6 = pfVar6 + 0x84;
    iVar2 = iVar2 + -1;
    pWVar5 = pWVar5 + 1;
  } while (iVar2 != 0);
  pfVar6 = matrix32x32;
  pWVar5 = keyframes;
  for (iVar2 = keyframeCount + -2; iVar2 != 0; iVar2 = iVar2 + -1) {
    fVar1 = (float)pWVar5[1].timeQ12 / g_Q12FloatScale4096;
    pfVar6[0x41] = 1.0;
    pfVar6[0x45] = -1.0;
    pfVar6[0x42] = fVar1 + fVar1;
    pfVar6[0x46] = -(fVar1 + fVar1);
    fVar1 = fVar1 * fVar1;
    fVar1 = fVar1 + fVar1 + fVar1;
    pfVar6[0x43] = fVar1;
    pfVar6[0x47] = -fVar1;
    pfVar6 = pfVar6 + 0x84;
    pWVar5 = pWVar5 + 1;
  }
  pfVar6 = matrix32x32;
  pWVar5 = keyframes;
  for (iVar2 = keyframeCount + -2; pWVar5 = pWVar5 + 1, iVar2 != 0; iVar2 = iVar2 + -1) {
    fVar1 = (float)pWVar5->timeQ12 / g_Q12FloatScale4096;
    pfVar6[0xa6] = 2.0;
    pfVar6[0xa2] = -2.0;
    fVar1 = fVar1 + fVar1 + fVar1;
    fVar1 = fVar1 + fVar1;
    pfVar6[0xa7] = fVar1;
    pfVar6[0xa3] = -fVar1;
    pfVar6 = pfVar6 + 0x84;
  }
  fVar1 = (float)pWVar5->timeQ12 / g_Q12FloatScale4096;
  pfVar6[0x41] = 1.0;
  pfVar6[0x42] = fVar1 + fVar1;
  fVar1 = fVar1 * fVar1;
  pfVar6[0x43] = fVar1 + fVar1 + fVar1;
  fVar1 = (float)keyframes->timeQ12 / g_Q12FloatScale4096;
  matrix32x32[0x21] = 1.0;
  matrix32x32[0x22] = fVar1 + fVar1;
  fVar1 = fVar1 * fVar1;
  matrix32x32[0x23] = fVar1 + fVar1 + fVar1;
  iVar2 = keyframeCount + -1;
  piVar4 = piVar3;
  pfVar6 = outCoefficients;
  do {
    *pfVar6 = (float)*piVar4 / g_Q12FloatScale4096;
    pfVar6[1] = 0.0;
    pfVar6[2] = 0.0;
    piVar4 = piVar4 + 8;
    pfVar6 = pfVar6 + 4;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  iVar2 = keyframeCount + -1;
  pfVar6 = outCoefficients;
  do {
    piVar3 = piVar3 + 8;
    pfVar6[3] = (float)*piVar3 / g_Q12FloatScale4096;
    pfVar6 = pfVar6 + 4;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
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
  float *pfVar1;
  float fVar2;
  int iVar3;
  float *pfVar4;
  float *pfVar5;
  
  pfVar5 = matrix32x32 + rowIndex * 0x20;
  iVar3 = 0;
  pfVar1 = pfVar5 + columnIndex;
  pfVar4 = matrix32x32 + columnIndex;
  fVar2 = *pfVar1;
  if (lastPriorIndex < 0x80000000) {
    do {
      fVar2 = fVar2 - *pfVar5 * *pfVar4;
      iVar3 = iVar3 + 1;
      pfVar5 = pfVar5 + 1;
      pfVar4 = pfVar4 + 0x20;
    } while (iVar3 <= (int)lastPriorIndex);
  }
  *pfVar1 = fVar2 / pivot;
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
   Ownership: core/math/interpolation.
   Purpose: Clears the six cached derivative values used by the world-motion spline evaluator. Role: Clears the six
   cached derivative values used by world-motion spline evaluation. Inputs: Global spline derivative cache.
   Outputs: Zeroed six-channel derivative cache. Edges: Precedes building/evaluating a new motion spline.
*/
void __thandor_void_preserve_eax_ecx WorldMotionSpline_ClearCachedDerivatives(void)

{
  int derivativesRemaining;
  float *derivativeCursor;
  
  derivativeCursor = g_WorldMotionSplineCachedDerivatives;
  for (derivativesRemaining = 6; derivativesRemaining != 0;
      derivativesRemaining = derivativesRemaining + -1) {
    *derivativeCursor = 0.0;
    derivativeCursor = derivativeCursor + 1;
  }
  return;
}


/* Address: 0x0053D2E0.
   Ownership: core/math/interpolation.
   Purpose: Evaluates one cubic segment and returns a Q12 scalar. RET 0x0C proves three stack arguments. Role:
   Evaluates one cubic segment value at Q12 time. Inputs: Time, segment index and solved coefficient table.
   Outputs: Q12 channel value.
*/
sdword CubicSpline_EvaluateValueQ12
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
  float fVar1;
  float fVar2;
  
  fVar1 = ((float)timeQ12 / g_Q12FloatScale4096) * coefficients[segmentIndex * 4 + 3];
  fVar2 = fVar1 + coefficients[segmentIndex * 4 + 2];
  return (fVar2 + fVar2 + fVar1) * ((float)timeQ12 / g_Q12FloatScale4096) +
         coefficients[segmentIndex * 4 + 1];
}

