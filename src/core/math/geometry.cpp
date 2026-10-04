/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/geometry.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/geometry.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/math/geometry. */

Bool8 g_Triangle2DBarycentricOutside;

/* Computes the barycentric weights of a screen point for vertices A and B of a projected triangle
   (C's weight is the remainder to 1.0), used to interpolate texture/shade values when a clipped
   terrain triangle is queued. Returns both weights in Q12 packed in one struct and publishes "point outside
   the triangle" (a second result in the original) in g_Triangle2DBarycentricOutside.
   Called directly by the terrain projection code (TerrainProjectedTriangle_ClipInterpolateAndQueueTextured).
*/
TriangleBarycentricWeightsQ12
Triangle2D_ComputeBarycentricWeightsQ12Packed
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  /* "Point outside the triangle" is published in g_Triangle2DBarycentricOutside. Weights are Q16
     internally and returned >> 4. */
  int64_t denominator;
  int64_t numerator;
  int denominatorShifted;
  int denominatorHigh;
  int weightA;
  int weightB;
  TriangleBarycentricWeightsQ12 result;

  g_Triangle2DBarycentricOutside = true;
  result.weightVertexA_Q12 = 0;
  result.weightVertexB_Q12 = 0;
  if ((pointX > vertexCX && pointX > vertexBX && pointX > vertexAX) ||
      (pointX < vertexCX && pointX < vertexBX && pointX < vertexAX) ||
      (pointY > vertexCY && pointY > vertexBY && pointY > vertexAY) ||
      (pointY < vertexCY && pointY < vertexBY && pointY < vertexAY)) {
    return result;
  }
  denominator = (int64_t)vertexCX * (vertexAY - vertexBY) + (int64_t)vertexBX * (vertexCY - vertexAY) +
                (int64_t)vertexAX * (vertexBY - vertexCY);
  denominatorHigh = (int)(denominator >> 32);
  denominatorShifted = (int)(denominator >> 16);
  if (denominatorShifted == 0) {
    return result;
  }
  numerator = (int64_t)(pointY - vertexBY) * vertexCX + (int64_t)(vertexCY - pointY) * vertexBX +
              (int64_t)(vertexBY - vertexCY) * pointX;
  if (denominatorHigh >= 0 ? (int)(numerator >> 32) > denominatorHigh
                           : (int)(numerator >> 32) < denominatorHigh) {
    return result;
  }
  weightA = (int)(numerator / denominatorShifted);
  if (weightA < 0 || weightA > TRIANGLE_BARYCENTRIC_WEIGHT_ONE_Q16) {
    return result;
  }
  numerator = (int64_t)(vertexAY - pointY) * vertexCX + (int64_t)(vertexCY - vertexAY) * pointX +
              (int64_t)(pointY - vertexCY) * vertexAX;
  if (denominatorHigh >= 0 ? (int)(numerator >> 32) > denominatorHigh
                           : (int)(numerator >> 32) < denominatorHigh) {
    return result;
  }
  weightB = (int)(numerator / denominatorShifted);
  if (weightB < 0 || weightA + weightB > TRIANGLE_BARYCENTRIC_WEIGHT_ONE_Q16) {
    return result;
  }
  result.weightVertexB_Q12 = weightB >> 4;
  result.weightVertexA_Q12 = weightA >> 4;
  g_Triangle2DBarycentricOutside = false;
  return result;
}

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

  /* Original quirk: this divides by s2 before the s2 > 16 check below, so s2 == 0 would fault. */
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
