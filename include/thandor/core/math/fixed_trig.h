/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/fixed_trig.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_FIXED_TRIG_H
#define THANDOR_CORE_MATH_FIXED_TRIG_H

#include <thandor/core/math/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/core/contracts.h>

/* FixedMath_Atan2Angle16: odd polynomial atan(t) ~ t * (C1 - t^2 * (C3 - t^2 * C5)) in angle16 units, each
   step a FIXED_MUL_HIGH; C1 = 20861 = 2 * 65536 / (2 * pi) */
inline constexpr auto FIXED_ATAN_ANGLE16_C1 = 0x517d;
inline constexpr auto FIXED_ATAN_ANGLE16_C3 = 0x6ca6;
inline constexpr auto FIXED_ATAN_ANGLE16_C5 = 0x104c2;

FixedLengthAzimuthElevation
FixedMath_VectorToAnglesAndLength(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z);

FixedLengthAzimuthElevation FixedMath_VectorToAnglesAndLengthVec3(GraphicsFixedVec3 *vector);

FixedLengthAngle FixedMath_Vector2AngleAndLength
          (FixedMathVectorComponent32 component0,FixedMathVectorComponent32 component1);

void FixedMath_WriteDirectionQ28(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

FixedSinCos FixedMath_SinCosScaled(AngleTurn32 angle,FixedMathScale32 scale);

FixedPlanarPointQ12 FixedTrig_ProjectPlanarPoint(Q12 baseX,Q12 baseY,Q12 distance,AngleTurn32 angle16);

FixedElevationAzimuth FixedMath_VectorToAnglesVec3(GraphicsFixedVec3 *vector);

FixedDirection FixedMath_DirectionFromAnglesScaled(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale);

FixedDirection FixedMath_DirectionFromAnglesQ28(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

void FixedMath_WriteDirectionScaled(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale);

FixedVectorAngles FixedMath_VectorToAngles
          (FixedMathVectorComponent32 z,FixedMathVectorComponent32 y,FixedMathVectorComponent32 x);

uint32_t FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x);

/* Not in the original: fills g_FixedSineQ28 (the original shipped it precomputed). */
void FixedMath_BuildSinCosTables();

/* one sine over 1.5 turns in Q28, from a quarter turn before angle 0: sin(-16384..-1), then
   sin(0..16383) (FIXED_SINE_TABLE_SIN), then cos(0..65535) (FIXED_SINE_TABLE_COS); signed and full-turn lookups run on from one part into the next */
extern int32_t g_FixedSineQ28[98304];

#endif /* THANDOR_CORE_MATH_FIXED_TRIG_H */
