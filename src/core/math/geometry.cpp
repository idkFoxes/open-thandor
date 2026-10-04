/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/geometry.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/geometry.h>
#include <thandor/thandor.h>

/* Two-bone joint solver for the leg suspension: for a triangle with sides s0, s1 and base s2 it returns
   jointAngle0 = the angle between s2 and s1 and jointAngle1 = that angle plus the one between s2 and s0 (the
   bend at the joint of s0 and s1). The height over the base comes from 64-bit sums of squares; when the
   triangle cannot close or the base is at most 0x10, both angles are 0 when s0 < s2 (unsigned) and 0x8000
   (half turn) otherwise.
*/
FixedTriangleJointAngles FixedGeometry_SolveTriangleJointAngles(Q12 sideLength0Q12,Q12 sideLength1Q12,Q12 sideLength2Q12)

{
  int64_t projectionSquared;
  int64_t doubleHeightSquared;
  int64_t side2Squared;
  int64_t side1Squared;
  int64_t side0Squared;
  int projection;
  uint32_t triangleHeight;
  uint32_t firstAngle16;
  FixedTriangleJointAngles solvedAngles;
  FixedTriangleJointAngles fallbackAngles;
  int64_t cosineNumerator0;

  /* The original divides by s2 here, before the s2 > 16 check below, so s2 == 0 faults; bounded here
     because of that: s2 == 0 takes the fallback of a base of at most 16 (s0 < s2 cannot hold unsigned, so
     the half turn). Every caller passes the leg geometry, never 0, so valid results are unchanged. */
  if (sideLength2Q12 == 0) {
    fallbackAngles.jointAngle0 = FIXED_ANGLE16_HALF_TURN;
    fallbackAngles.jointAngle1 = FIXED_ANGLE16_HALF_TURN;
    return fallbackAngles;
  }
  projection = (int)(((int64_t)sideLength0Q12 * (int64_t)sideLength0Q12 -
                (int64_t)sideLength1Q12 * (int64_t)sideLength1Q12) / (int64_t)sideLength2Q12);
  projectionSquared = (int64_t)projection * (int64_t)projection;
  side2Squared = (int64_t)sideLength2Q12 * (int64_t)sideLength2Q12;
  side1Squared = (int64_t)sideLength1Q12 * (int64_t)sideLength1Q12;
  side0Squared = (int64_t)sideLength0Q12 * (int64_t)sideLength0Q12;
  cosineNumerator0 = (side2Squared - side1Squared) + side0Squared;
  /* 64-bit -p^2 - s2^2 + 2*s1^2 + 2*s0^2, in the original's order; with p = (s0^2 - s1^2) / s2
     this is (2 * height)^2, so the square root is halved below like the two base projections */
  doubleHeightSquared = (((0 - projectionSquared) - side2Squared) + side1Squared * 2) + side0Squared * 2;
  if (doubleHeightSquared > -1 && sideLength2Q12 > 16) {
    triangleHeight = FIXED_UINT64_SQRT(doubleHeightSquared);
    firstAngle16 = FixedMath_Atan2Angle16((int)triangleHeight >> 1,
                                          (int)(cosineNumerator0 / (int64_t)sideLength2Q12) >> 1);
    solvedAngles.jointAngle0 =
         FixedMath_Atan2Angle16((int)triangleHeight >> 1,
                                (int)(((side1Squared + side2Squared) - side0Squared) / (int64_t)sideLength2Q12) >> 1);
    solvedAngles.jointAngle1 = firstAngle16 + solvedAngles.jointAngle0;
    return solvedAngles;
  }
  if ((uint32_t)sideLength0Q12 < (uint32_t)sideLength2Q12) {
    fallbackAngles.jointAngle0 = 0;
    fallbackAngles.jointAngle1 = 0;
  }
  else {
    fallbackAngles.jointAngle0 = FIXED_ANGLE16_HALF_TURN;
    fallbackAngles.jointAngle1 = FIXED_ANGLE16_HALF_TURN;
  }
  return fallbackAngles;
}
