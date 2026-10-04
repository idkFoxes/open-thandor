/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/camera/camera.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_CAMERA_CAMERA_H
#define THANDOR_WORLD_CAMERA_CAMERA_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Smallest camera magnitude WorldRuntime_SetCameraAnglesAndMagnitudeClamped accepts (0.25 in Q12) */
#define WORLD_MOTION_MINIMUM_MAGNITUDE_Q12 0x400

void WorldRuntime_SetCameraPositionKeepingTarget
          (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime);

void WorldRuntime_SetCameraAnglesAndMagnitudeClamped
          (WorldMotionValue78 projectionShift,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime);

void WorldRuntime_PointCameraAtTarget
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime);

void WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime);

void WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime);

void WorldRuntime_CommitCameraTargetDistance(WorldRuntimeContext *world);

WorldCameraPosition WorldRuntime_GetCameraPosition(WorldRuntimeContext *world);

WorldCameraOrientation WorldRuntime_GetCameraOrientation(WorldRuntimeContext *world);

void WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime);

void WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world);

#endif /* THANDOR_WORLD_CAMERA_CAMERA_H */
