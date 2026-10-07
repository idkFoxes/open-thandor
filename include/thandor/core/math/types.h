/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_TYPES_H
#define THANDOR_CORE_MATH_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

struct FixedVectorAngles;
struct FixedDirection;
struct RandomGeneratorState;
struct FixedRollAzimuthElevation;
struct FixedLengthAzimuthElevation;
struct FixedAzimuthElevationRoll;
struct FixedTriangleJointAngles;

struct FixedVectorAngles {
    uint32_t azimuthAngle; // atan2(y, x), low 16 bits
    uint32_t elevationAngle; // elevation over the horizontal plane, 16-bit angle
};

/* The angle pair of FixedMath_VectorToAnglesVec3 (elevation first). */
struct FixedElevationAzimuth;
struct FixedElevationAzimuth {
    uint32_t elevationAngle; 
    uint32_t azimuthAngle; 
};

struct FixedDirection {
    uint32_t x; // cos(azimuth) * cos(elevation) part, in the format of the scale (Q28 for unit directions)
    uint32_t y; // sin(azimuth) * cos(elevation) part
    uint32_t z; // sin(elevation) part
};

using CubicSplineSegmentIndex = int;

using CubicSplineEquationCount = uint32_t;

using UInt64Half32 = uint32_t;

using FixedMathVectorComponent32 = int;

using CubicSplineMatrixIndex = uint32_t;

using RandomSeed = uint32_t;

using WorldMotionSplineChannelByteOffset = uint32_t;

struct RandomGeneratorState {
    Ptr32<uint32_t ()> next; 
    RandomSeed primarySeed; 
    RandomSeed secondarySeed; 
};

/* Planar Q12 point (x, y), as FixedTrig_ProjectPlanarPoint returns it. */
struct FixedPlanarPointQ12;
struct FixedPlanarPointQ12 {
    Q12 xQ12;
    Q12 yQ12;
};

/* Angles of a rotation basis as FixedTransform_ExtractEulerAngles returns them. */
struct FixedRollAzimuthElevation {
    AngleTurn32 rollAngle;
    AngleTurn32 azimuthAngle;
    AngleTurn32 elevationAngle;
};

struct FixedLengthAzimuthElevation {
    uint32_t lengthQ12; // vector length
    AngleTurn32 azimuthAngle; // azimuth/heading, low 16 bits
    AngleTurn32 elevationAngle; // elevation/pitch
};

/* Angles of a composed rotation as FixedTransform_ComposeEulerAngles returns them; the order of the model
   nodes' worldRotationAngle0..2. */
struct FixedAzimuthElevationRoll {
    AngleTurn32 azimuthAngle;
    AngleTurn32 elevationAngle;
    AngleTurn32 rollAngle;
};

struct FixedTriangleJointAngles {
    AngleTurn32 jointAngle0; // angle between the base and side 1
    AngleTurn32 jointAngle1; // jointAngle0 plus the angle between the base and side 0 (bend at the joint)
};

#endif /* THANDOR_CORE_MATH_TYPES_H */
