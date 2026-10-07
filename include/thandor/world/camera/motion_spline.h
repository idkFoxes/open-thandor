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

/* Channels of a world-motion keyframe (position/origin x, y, z, magnitude/distance, yaw, pitch) */
inline constexpr int WORLD_MOTION_SPLINE_CHANNEL_COUNT = 6;

bool WorldMotionSpline_EvaluateAndApplyAtTime
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes,
          WorldMotionSplineTimeQ12 timeQ12,WorldRuntimeContext *worldRuntime);

void WorldMotionSpline_BuildSixChannelCurves
          (WorldMotionSplineKeyframeCount keyframeCount,WorldMotionSplineKeyframe *keyframes);

void WorldMotionSpline_ClearCachedDerivatives();

extern float *g_WorldMotionSplineMatrixWorkspaces[6];
extern float *g_WorldMotionSplineCoefficientTables[6];

#endif /* THANDOR_WORLD_CAMERA_MOTION_SPLINE_H */
