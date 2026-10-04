/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/fixed_transform.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_FIXED_TRANSFORM_H
#define THANDOR_CORE_MATH_FIXED_TRANSFORM_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/fixed_transform. */

/* Functions are grouped by semantic ownership. */

FixedAzimuthElevationRoll FixedTransform_ComposeEulerAngles
          (AngleTurn32 inputAngle0,AngleTurn32 inputAngle1,AngleTurn32 inputAngle2,
          AngleTurn32 basisAngle0,AngleTurn32 basisAngle1,AngleTurn32 basisAngle2);

FixedVectorQ12
FixedTransform_RotateVectorByEulerAngles
          (Q12 inputXQ12,Q12 inputYQ12,Q12 inputZQ12,AngleTurn32 rotationAngle0,
          AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

FixedVectorQ12
FixedTransform_RotateScaledDirection
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

void FixedTransform_ApplyDirection
          (GraphicsFixedVec3 *output,GraphicsFixedVec3 *direction,GraphicsFixedMatrix3x4 *transform);

void FixedTransform_ApplyTransposeDirection
          (GraphicsFixedVec3 *output,GraphicsFixedMatrix3x4 *transform,GraphicsFixedVec3 *direction);

void FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input);

FixedVectorQ12 FixedTransform_RotateScaledDirectionCore
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

FixedRollAzimuthElevation FixedTransform_ExtractEulerAngles(GraphicsFixedMatrix3x4 *transform);

void FixedTransform_Compose(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *innerTransform,
          GraphicsFixedMatrix3x4 *outerTransform);

void FixedTransform_ApplyPoint(GraphicsFixedVec3 *output,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

void FixedTransform_BuildRotationBasis(GraphicsFixedMatrix3x4 *output,AngleTurn32 rollAngle,AngleTurn32 elevationAngle,
          AngleTurn32 azimuthAngle);

extern GraphicsFixedVec3 g_ModelTransformOutput;

#endif /* THANDOR_CORE_MATH_FIXED_TRANSFORM_H */
