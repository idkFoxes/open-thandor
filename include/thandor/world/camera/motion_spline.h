/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/camera/motion_spline.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_CAMERA_MOTION_SPLINE_H
#define THANDOR_WORLD_CAMERA_MOTION_SPLINE_H

#include <thandor/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/camera/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/camera/motion_spline. */

/* Channels of a world-motion keyframe (position/origin x, y, z, magnitude/distance, yaw, pitch) */
#define WORLD_MOTION_SPLINE_CHANNEL_COUNT 6

/* Functions are grouped by semantic ownership. */

Bool8 WorldMotionSpline_EvaluateAndApplyAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

uint8_t WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

void WorldMotionSpline_BuildSixChannelCurves
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes);

void WorldMotionSpline_ClearCachedDerivatives(void);

extern float *g_WorldMotionSplineMatrixWorkspaces[6];
extern float *g_WorldMotionSplineCoefficientTables[6];

#endif /* THANDOR_WORLD_CAMERA_MOTION_SPLINE_H */
