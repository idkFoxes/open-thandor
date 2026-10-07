/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/spline.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_SPLINE_H
#define THANDOR_CORE_MATH_SPLINE_H

#include <thandor/core/math/types.h>
#include <thandor/world/camera/types.h>
#include <thandor/core/contracts.h>

/* Order of the cubic-spline equation matrix (32x32 floats, row-major): 4 coefficients per segment, so at most
   8 segments / 9 keyframes (CubicSpline_BuildNaturalCoefficientSystem, CubicSpline_SolveCoefficientSystem). */
inline constexpr auto CUBIC_SPLINE_MATRIX_ORDER = 32;

/* Floats of one equation matrix (g_WorldMotionSplineMatrixWorkspaces[channel], 0x400) */
#define CUBIC_SPLINE_MATRIX_FLOATS (CUBIC_SPLINE_MATRIX_ORDER * CUBIC_SPLINE_MATRIX_ORDER)

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

int32_t CubicSpline_EvaluateValueQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

float CubicSpline_EvaluateDerivativeQ12 (WorldMotionSplineTimeQ12 timeQ12,CubicSplineSegmentIndex segmentIndex, float *coefficients);

#endif /* THANDOR_CORE_MATH_SPLINE_H */
