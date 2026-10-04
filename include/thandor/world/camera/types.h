/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/camera/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_CAMERA_TYPES_H
#define THANDOR_WORLD_CAMERA_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct WorldMotionSplineKeyframe WorldMotionSplineKeyframe, *PWorldMotionSplineKeyframe;
typedef struct WorldCameraOrientation WorldCameraOrientation, *PWorldCameraOrientation;
typedef struct WorldCameraPosition WorldCameraPosition, *PWorldCameraPosition;

using WorldMotionSplineKeyframeCount = int;

using WorldMotionSplineValueQ12 = int;

using WorldMotionSplineTimeQ12 = int;

using WorldMotionValue78 = uint32_t;

using CameraScreenDeltaPixels = int;

struct WorldMotionSplineKeyframe {
    WorldMotionSplineValueQ12 channel0Q12; 
    WorldMotionSplineValueQ12 channel1Q12; 
    WorldMotionSplineValueQ12 channel2Q12; 
    WorldMotionSplineValueQ12 channel3Q12; 
    WorldMotionSplineValueQ12 channel4Q12; 
    WorldMotionSplineValueQ12 channel5Q12; 
    WorldMotionSplineTimeQ12 timeQ12; 
    uint32_t reserved1C;
};

struct WorldCameraOrientation {
    UQ12 magnitudeQ12; // world motion magnitude
    AngleTurn32 headingAngle; // world motion heading
    AngleTurn32 pitchAngle; // world motion pitch
};

struct WorldCameraPosition {
    Q12 xQ12; // world motion X
    Q12 yQ12; // world motion Y
    Q12 zQ12; // world motion Z
};

#endif /* THANDOR_WORLD_CAMERA_TYPES_H */
