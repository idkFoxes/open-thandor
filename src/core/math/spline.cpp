/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/spline.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/spline.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static const float g_Q12FloatScale4096 = 4096.0f;

/* Solves the spline equation system built by CubicSpline_BuildNaturalCoefficientSystem in place: an LU
   (Doolittle) decomposition of the matrix without pivoting, whose unit-L forward substitution runs along row
   by row, followed by back substitution with U. rhsVector then holds the four coefficients of every segment.
*/
void CubicSpline_SolveCoefficientSystem(CubicSplineEquationCount equationCount,float *rhsVector,float *matrix32x32)

{
  float pivot;
  CubicSplineMatrixIndex rowIndex;
  CubicSplineMatrixIndex columnIndex;
  CubicSplineMatrixIndex lowerRowIndex;
  CubicSplineMatrixIndex backRowIndex;

  /* The do-while loops run their body at least once, as in the original (only relevant for an empty system,
     which the callers never build: there are always at least 4 equations). */
  rowIndex = 0;
  do {
    /* row rowIndex of U (columns rowIndex..n-1), then the forward-substituted right-hand side */
    columnIndex = rowIndex;
    do {
      CubicSpline_ForwardEliminateColumn(1.0,rowIndex - 1,columnIndex,rowIndex,matrix32x32);
      columnIndex++;
    } while (columnIndex < equationCount);
    CubicSpline_BackSubstituteRow(1.0,rowIndex - 1,0,rowIndex,rhsVector,matrix32x32);
    if (rowIndex + 1 < equationCount) {
      /* column rowIndex of L below the diagonal, divided by the pivot U[rowIndex][rowIndex] */
      pivot = matrix32x32[(int32_t)(rowIndex * (CUBIC_SPLINE_MATRIX_ORDER + 1))];
      for (lowerRowIndex = rowIndex + 1; lowerRowIndex < equationCount; lowerRowIndex++) {
        CubicSpline_ForwardEliminateColumn(pivot,rowIndex - 1,rowIndex,lowerRowIndex,matrix32x32);
      }
    }
    rowIndex++;
  } while (rowIndex < equationCount);
  /* back substitution from the last row up */
  backRowIndex = equationCount - 1;
  do {
    CubicSpline_BackSubstituteRow
              (matrix32x32[(int32_t)(backRowIndex * (CUBIC_SPLINE_MATRIX_ORDER + 1))],equationCount - 1,
               backRowIndex + 1,backRowIndex,rhsVector,matrix32x32);
    backRowIndex--;
  } while (-1 < (int)backRowIndex);
}

/* Builds the equation system of a piecewise cubic spline through one channel of the keyframes (times and values
   Q12, converted to float seconds/units): segment s has the unknowns a + b*t + c*t^2 + d*t^3 in columns 4s..4s+3.
   Per segment, row 4s and 4s+3 fix the values at both keyframes, row 4s+2 makes the slope continuous and row
   4s+5 the curvature; row 1 sets the start slope to startDerivative and the last segment's row 4s+2 the end slope
   to 0 (so the spline is clamped, not natural). The right-hand side goes to outCoefficients.
*/
void CubicSpline_BuildNaturalCoefficientSystem(float startDerivative,CubicSplineEquationCount *outEquationCount,
          float *outCoefficients,float *matrix32x32,WorldMotionSplineKeyframeCount keyframeCount,
          WorldMotionSplineChannelByteOffset channelByteOffset,WorldMotionSplineKeyframe *keyframes)

{
  float knotTime;
  float knotTimeSquared;
  float knotTimeTripled;
  float slopeCubicTerm;
  float curvatureCubicTerm;
  int remainingCount;
  int *firstChannelValue;
  int *endValueCursor;
  int *startValueCursor;
  WorldMotionSplineKeyframe *keyframeCursor;
  float *floatCursor;

  /* The original requires 2 <= keyframeCount <= 9 unchecked (1 or 0 makes the do-while loops below run ~2^32
     times, 10 or more needs more than the 32 rows); clamped here because both write outside the 32x32
     workspace. The only caller (world/camera/motion_spline.cpp) passes 2. */
  if (keyframeCount < 2 || keyframeCount > (CUBIC_SPLINE_MATRIX_ORDER + 4) / 4) {
    static Bool8 s_KeyframeCountLogged = false;
    if (!s_KeyframeCountLogged) {
      s_KeyframeCountLogged = true;
      Thandor_Log("spline: keyframe count %d out of 2..%d, clamped",(int)keyframeCount,
                  (CUBIC_SPLINE_MATRIX_ORDER + 4) / 4);
    }
    keyframeCount = (keyframeCount < 2) ? 2 : (CUBIC_SPLINE_MATRIX_ORDER + 4) / 4;
  }
  /* In the matrix loops floatCursor points at the top-left of segment s's 4x4 diagonal block and steps one
     block (4 rows and 4 columns) per segment; indices below are written as row * order + column. */
  firstChannelValue = (int *)((uint8_t *)&keyframes->channel0Q12 + channelByteOffset);
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
    knotTime = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
    *floatCursor = 1.0;
    floatCursor[1] = knotTime;
    floatCursor[2] = knotTime * knotTime;
    floatCursor[3] = knotTime * knotTime * knotTime;
    keyframeCursor = keyframeCursor + 1;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    remainingCount--;
  } while (remainingCount != 0);
  /* row 4s+3: the segment's value at its end keyframe */
  remainingCount = keyframeCount - 1;
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  do {
    knotTime = (float)keyframeCursor[1].timeQ12 / g_Q12FloatScale4096;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 0] = 1.0;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 1] = knotTime;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTime * knotTime;
    floatCursor[3 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTime * knotTime * knotTime;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    remainingCount--;
    keyframeCursor = keyframeCursor + 1;
  } while (remainingCount != 0);
  /* row 4s+2 (all but the last segment): slope of segment s minus slope of segment s+1 at their shared knot */
  floatCursor = matrix32x32;
  keyframeCursor = keyframes;
  for (remainingCount = keyframeCount - 2; remainingCount != 0; remainingCount--) {
    knotTime = (float)keyframeCursor[1].timeQ12 / g_Q12FloatScale4096;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 1] = 1.0;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 5] = -1.0;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTime + knotTime;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 6] = -(knotTime + knotTime);
    knotTimeSquared = knotTime * knotTime;
    slopeCubicTerm = knotTimeSquared + knotTimeSquared + knotTimeSquared;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 3] = slopeCubicTerm;
    floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 7] = -slopeCubicTerm;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    keyframeCursor = keyframeCursor + 1;
  }
  /* row 4s+5: curvature of segment s+1 minus curvature of segment s at their shared knot (keyframe s+1); the
     loop leaves keyframeCursor on the last keyframe and floatCursor on the last segment's block */
  floatCursor = matrix32x32;
  keyframeCursor = keyframes + 1;
  for (remainingCount = keyframeCount - 2; remainingCount != 0; remainingCount--) {
    knotTime = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 6] = 2.0;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 2] = -2.0;
    knotTimeTripled = knotTime + knotTime + knotTime;
    curvatureCubicTerm = knotTimeTripled + knotTimeTripled;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 7] = curvatureCubicTerm;
    floatCursor[5 * CUBIC_SPLINE_MATRIX_ORDER + 3] = -curvatureCubicTerm;
    floatCursor = floatCursor + 4 * CUBIC_SPLINE_MATRIX_ORDER + 4;
    keyframeCursor = keyframeCursor + 1;
  }
  /* last segment, row 4s+2: slope at the last keyframe (right-hand side 0) */
  knotTime = (float)keyframeCursor->timeQ12 / g_Q12FloatScale4096;
  floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 1] = 1.0;
  floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTime + knotTime;
  knotTimeSquared = knotTime * knotTime;
  floatCursor[2 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTimeSquared + knotTimeSquared + knotTimeSquared;
  /* row 1: slope at the first keyframe (right-hand side startDerivative) */
  knotTime = (float)keyframes->timeQ12 / g_Q12FloatScale4096;
  matrix32x32[1 * CUBIC_SPLINE_MATRIX_ORDER + 1] = 1.0;
  matrix32x32[1 * CUBIC_SPLINE_MATRIX_ORDER + 2] = knotTime + knotTime;
  knotTimeSquared = knotTime * knotTime;
  matrix32x32[1 * CUBIC_SPLINE_MATRIX_ORDER + 3] = knotTimeSquared + knotTimeSquared + knotTimeSquared;
  /* right-hand side per segment: start value, 0, 0, end value (one keyframe apart) */
  remainingCount = keyframeCount - 1;
  startValueCursor = firstChannelValue;
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
  endValueCursor = firstChannelValue;
  floatCursor = outCoefficients;
  do {
    endValueCursor = endValueCursor + sizeof(WorldMotionSplineKeyframe) / sizeof(int);
    floatCursor[3] = (float)*endValueCursor / g_Q12FloatScale4096;
    floatCursor = floatCursor + 4;
    remainingCount--;
  } while (remainingCount != 0);
  outCoefficients[1] = startDerivative;
}

/* One LU-decomposition step of CubicSpline_SolveCoefficientSystem, in place on the 32x32 matrix M:
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
  if (lastPriorIndex < INTERPOLATION_SIGN_BIT) { /* signed lastPriorIndex >= 0; -1 on the first row means no terms */
    do {
      reducedValue = reducedValue - *rowCursor * *columnCursor;
      priorIndex++;
      rowCursor++;
      columnCursor = columnCursor + CUBIC_SPLINE_MATRIX_ORDER;
    } while (priorIndex <= (int)lastPriorIndex);
  }
  *targetElement = reducedValue / pivot;
}

/* One substitution step of CubicSpline_SolveCoefficientSystem on the right-hand side b with the 32x32 matrix M:
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
}

/* Value of one solved spline segment at a Q12 time: the segment's four float coefficients a + b*t +
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

/* First derivative b + 2c*t + 3d*t^2 of one spline segment at a Q12 time. Unlike the value it stays a
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
