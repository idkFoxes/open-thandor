/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/interpolation.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_INTERPOLATION_H
#define THANDOR_CORE_MATH_INTERPOLATION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/interpolation. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053CA30 */
bool __thandor_cf_preserve_eax_ecx_edx
WorldMotionSpline_EvaluateAndApplyAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

/* 0x0053CBB0 */
byte __thandor_cf_preserve_eax_ecx_edx
WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

/* 0x0053CD10 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotionSpline_BuildSixChannelCurves
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes);

/* 0x004CCC00 */
void __thandor_void_preserve_eax_ecx_edx
InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount transitionDurationTicks,
          GraphicsShadingRuntimeRecord *interpolationState);

/* 0x004CCC80 */
void __thandor_void_preserve_eax_ecx_edx
InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks);

/* 0x0053D230 */
void __thandor_void_preserve_eax_ecx_edx
CubicSpline_SolveCoefficientSystem
          (CubicSplineEquationCount equationCount,float *rhsVector,float *matrix32x32);

/* 0x0053CF10 */
void __thandor_void_preserve_eax_ecx_edx
CubicSpline_BuildNaturalCoefficientSystem
          (float endpointDerivative,CubicSplineEquationCount *outEquationCount,
          float *outCoefficients,float *matrix32x32,WorldMotionSplineKeyframeCount keyframeCount,
          WorldMotionSplineChannelByteOffset channelByteOffset,WorldMotionSplineKeyframe *keyframes);

/* 0x0053D160 */
void __thandor_void_preserve_ecx_edx
CubicSpline_ForwardEliminateColumn
          (float pivot,CubicSplineMatrixIndex lastPriorIndex,CubicSplineMatrixIndex columnIndex,
          CubicSplineMatrixIndex rowIndex,float *matrix32x32);

/* 0x0053D1C0 */
void __thandor_void_preserve_ecx_edx
CubicSpline_BackSubstituteRow
          (float pivot,CubicSplineMatrixIndex lastSolvedIndex,
          CubicSplineMatrixIndex firstSolvedIndex,CubicSplineMatrixIndex targetIndex,
          float *rhsVector,float *matrix32x32);

/* 0x0053CA10 */
void __thandor_void_preserve_eax_ecx WorldMotionSpline_ClearCachedDerivatives(void);

/* 0x0053D2E0 */
sdword CubicSpline_EvaluateValueQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

/* 0x0053D320 */
float CubicSpline_EvaluateDerivativeQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

#endif /* THANDOR_CORE_MATH_INTERPOLATION_H */
