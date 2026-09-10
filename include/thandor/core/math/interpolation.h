#ifndef THANDOR_CORE_MATH_INTERPOLATION_H
#define THANDOR_CORE_MATH_INTERPOLATION_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/interpolation. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053CA30 */
void WorldMotionSpline_EvaluateAndApplyAtTime (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes, WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

/* 0x0053CBB0 */
undefined8 WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes, WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

/* 0x0053CD10 */
void WorldMotionSpline_BuildSixChannelCurves (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes);

/* 0x004CCC00 */
void InterpolationState_SetNegatedTargetAndRescaleProgress (GraphicsTransitionTickCount transitionDurationTicks, GraphicsShadingRuntimeRecord *interpolationState);

/* 0x004CCC80 */
void InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks);

/* 0x0053D230 */
void CubicSpline_SolveCoefficientSystem (CubicSplineEquationCount equationCount,float *rhsVector,float *matrix32x32);

/* 0x0053CF10 */
void CubicSpline_BuildNaturalCoefficientSystem (float endpointDerivative,CubicSplineEquationCount *outEquationCount, float *outCoefficients,float *matrix32x32, WorldMotionSplineKeyframeCount keyframeCount, WorldMotionSplineChannelByteOffset channelByteOffset, WorldMotionSplineKeyframe *keyframes);

/* 0x0053D160 */
void CubicSpline_ForwardEliminateColumn (float pivot,CubicSplineMatrixIndex lastPriorIndex,CubicSplineMatrixIndex columnIndex ,CubicSplineMatrixIndex rowIndex,float *matrix32x32);

/* 0x0053D1C0 */
void CubicSpline_BackSubstituteRow (float pivot,CubicSplineMatrixIndex lastSolvedIndex, CubicSplineMatrixIndex firstSolvedIndex,CubicSplineMatrixIndex targetIndex, float *rhsVector,float *matrix32x32);

/* 0x0053CA10 */
void WorldMotionSpline_ClearCachedDerivatives(void);

/* 0x0053D2E0 */
sdword CubicSpline_EvaluateValueQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

/* 0x0053D320 */
float CubicSpline_EvaluateDerivativeQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

#endif /* THANDOR_CORE_MATH_INTERPOLATION_H */
