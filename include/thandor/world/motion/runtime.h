/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/motion/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MOTION_RUNTIME_H
#define THANDOR_WORLD_MOTION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/motion/runtime. */

/* Scroll-arrow cursor frames returned by WorldRuntime_ApplyEdgeScrollAndGetCursorFrame (clockwise from up). */
#define WORLD_CURSOR_SCROLL_UP 0x2F
#define WORLD_CURSOR_SCROLL_UP_RIGHT 0x30
#define WORLD_CURSOR_SCROLL_RIGHT 0x31
#define WORLD_CURSOR_SCROLL_DOWN_RIGHT 0x32
#define WORLD_CURSOR_SCROLL_DOWN 0x33
#define WORLD_CURSOR_SCROLL_DOWN_LEFT 0x34
#define WORLD_CURSOR_SCROLL_LEFT 0x35
#define WORLD_CURSOR_SCROLL_UP_LEFT 0x36
/* Hard lower limit (0.25 in Q12) of the camera distance and position magnitude set by the WorldMotion_Adjust*
   functions, applied after the configurable limits. */
#define WORLD_MOTION_MINIMUM_DISTANCE_Q12 0x400
/* Functions are grouped by semantic ownership. */

uint32_t WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime);

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

#endif /* THANDOR_WORLD_MOTION_RUNTIME_H */
