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

/* Order of the cubic-spline equation matrix (32x32 floats, row-major): 4 coefficients per segment, so at most
   8 segments / 9 keyframes (CubicSpline_BuildNaturalCoefficientSystem, CubicSpline_SolveCoefficientSystem). */
#define CUBIC_SPLINE_MATRIX_ORDER 32
/* Channels of a world-motion keyframe (position/origin x, y, z, magnitude/distance, yaw, pitch) */
#define WORLD_MOTION_SPLINE_CHANNEL_COUNT 6
/* Floats of one equation matrix (g_WorldMotionSplineMatrixWorkspaces[channel], 0x400) */
#define CUBIC_SPLINE_MATRIX_FLOATS (CUBIC_SPLINE_MATRIX_ORDER * CUBIC_SPLINE_MATRIX_ORDER)
/* Bit 31 as an unsigned int: (uint32_t)value < INTERPOLATION_SIGN_BIT tests a signed value for >= 0 (the
   original's unsigned compare) */
#define INTERPOLATION_SIGN_BIT 0x80000000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

bool WorldMotionSpline_EvaluateAndApplyAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

uint8_t WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

void WorldMotionSpline_BuildSixChannelCurves
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes);

void InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount fadeOutTicks,GraphicsShadingRuntimeRecord *shadingRecord);

void InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks);

void CubicSpline_SolveCoefficientSystem(CubicSplineEquationCount equationCount,float *rhsVector,float *matrix32x32);

void CubicSpline_BuildNaturalCoefficientSystem(float startDerivative,CubicSplineEquationCount *outEquationCount,
          float *outCoefficients,float *matrix32x32,WorldMotionSplineKeyframeCount keyframeCount,
          WorldMotionSplineChannelByteOffset channelByteOffset,WorldMotionSplineKeyframe *keyframes);

void CubicSpline_ForwardEliminateColumn
          (float pivot,CubicSplineMatrixIndex lastPriorIndex,CubicSplineMatrixIndex columnIndex,
          CubicSplineMatrixIndex rowIndex,float *matrix32x32);

void CubicSpline_BackSubstituteRow(float pivot,CubicSplineMatrixIndex lastSolvedIndex,
          CubicSplineMatrixIndex firstSolvedIndex,CubicSplineMatrixIndex targetIndex,
          float *rhsVector,float *matrix32x32);

void WorldMotionSpline_ClearCachedDerivatives(void);

int32_t CubicSpline_EvaluateValueQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

float CubicSpline_EvaluateDerivativeQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

#endif /* THANDOR_CORE_MATH_INTERPOLATION_H */
