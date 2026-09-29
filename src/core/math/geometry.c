/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/geometry.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/geometry.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/math/geometry. */

bool g_Triangle2DBarycentricOutside;

/* Address: 0x004869B0.
   Computes the barycentric weights of a screen point for vertices A and B of a projected triangle
   (C's weight is the remainder to 1.0), used to interpolate texture/shade values when a clipped
   terrain triangle is queued. Returns both weights in Q12 packed in EDX:EAX and publishes "point outside
   the triangle" (the original CF) in g_Triangle2DBarycentricOutside.
   Called directly by the terrain projection code (TerrainProjectedTriangle_ClipInterpolateAndQueueTextured).
*/
TriangleBarycentricWeightsQ12
Triangle2D_ComputeBarycentricWeightsQ12Packed
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  /* Rewritten from the assembly (0x004869B0-0x00486AFC). The original reports "point outside the
     triangle" through CF, which the decompiler dropped; it is published in
     g_Triangle2DBarycentricOutside. Weights are Q16 internally and returned >> 4. */
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
  result.weightVertexB_Q12 = weightB >> 4; /* EAX */
  result.weightVertexA_Q12 = weightA >> 4; /* EDX */
  g_Triangle2DBarycentricOutside = false;
  return result;
}

