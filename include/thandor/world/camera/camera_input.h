/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/camera/camera_input.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_CAMERA_CAMERA_INPUT_H
#define THANDOR_WORLD_CAMERA_CAMERA_INPUT_H

#include <thandor/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/camera/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/camera/camera_input. */

/* Hard lower limit (0.25 in Q12) of the camera distance and position magnitude set by the WorldMotion_Adjust*
   functions, applied after the configurable limits. */
#define WORLD_MOTION_MINIMUM_DISTANCE_Q12 0x400

/* Functions are grouped by semantic ownership. */

void WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
          (AngleTurn32 elevationAngle,int screenDelta,WorldRuntimeContext *worldRuntime);

void WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn(int screenDelta,WorldRuntimeContext *worldRuntime);

void WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
          (int screenDelta,WorldRuntimeContext *worldRuntime);

void WorldMotion_AdjustHeadingAndRecomputePosition(int headingDeltaInput,WorldRuntimeContext *worldRuntime);

void WorldMotion_AdjustHeadingAndClearFieldGridDirty(int headingDeltaInput,WorldRuntimeContext *worldRuntime);

void WorldMotion_AdjustDistanceClampAndRecomputePosition(int distanceDeltaInput,WorldRuntimeContext *worldRuntime);

void WorldMotion_AdjustPositionMagnitudeClamp(int magnitudeDeltaInput,WorldRuntimeContext *worldRuntime);

void WorldMotion_AdjustPitchClampAndRecomputePosition(int pitchDeltaInput,WorldRuntimeContext *worldRuntime);

void WorldMotion_AdjustPitchClampAndClearFieldGridDirty(int pitchDeltaInput,WorldRuntimeContext *worldRuntime);

void WorldRuntime_TranslateCameraByScreenDelta
          (CameraScreenDeltaPixels screenDeltaDown,uint32_t screenDeltaRight,WorldRuntimeContext *worldRuntime
          );

#endif /* THANDOR_WORLD_CAMERA_CAMERA_INPUT_H */
