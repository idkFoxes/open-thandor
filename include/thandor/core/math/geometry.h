/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/geometry.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_GEOMETRY_H
#define THANDOR_CORE_MATH_GEOMETRY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/geometry. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Weight 1.0 of Triangle2D_ComputeBarycentricWeightsQ12Packed's internal Q16 weights (returned >> 4 as Q12) */
#define TRIANGLE_BARYCENTRIC_WEIGHT_ONE_Q16 0x10000

/* CF result of Triangle2D_ComputeBarycentricWeightsQ12Packed: the point is outside the triangle. */
extern bool g_Triangle2DBarycentricOutside;

/* 0x004869B0 */
TriangleBarycentricWeightsQ12
Triangle2D_ComputeBarycentricWeightsQ12Packed
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX);

#endif /* THANDOR_CORE_MATH_GEOMETRY_H */
