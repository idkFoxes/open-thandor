#ifndef THANDOR_WORLD_MOTION_RUNTIME_H
#define THANDOR_WORLD_MOTION_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/motion/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0050D050 */
dword WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime);

/* 0x0050C7F0 */
void __fastcall WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn (undefined4 param_1,AngleTurn32 elevationAngle,int screenDelta, WorldRuntimeContext *worldRuntime);

/* 0x0050C850 */
void WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn (int screenDelta,WorldRuntimeContext *worldRuntime);

/* 0x0050C8C0 */
void WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading (int screenDelta,WorldRuntimeContext *worldRuntime);

/* 0x0050C920 */
void WorldMotion_AdjustHeadingAndRecomputePosition (int headingDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050C990 */
undefined4 WorldMotion_AdjustHeadingAndClearFieldGridDirty(int param_1,WorldRuntimeContext *param_2);

/* 0x0050C9C0 */
void WorldMotion_AdjustDistanceClampAndRecomputePosition(int param_1,WorldRuntimeContext *param_2);

/* 0x0050CA80 */
void WorldMotion_AdjustPositionMagnitudeClamp (int magnitudeDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050CB10 */
void WorldMotion_AdjustPitchClampAndRecomputePosition (int pitchDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050CBE0 */
void WorldMotion_AdjustPitchClampAndClearFieldGridDirty (int pitchDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050C770 */
void WorldRuntime_TranslateCameraByScreenDelta (CameraScreenDeltaPixels screenDeltaX,dword screenDeltaY, WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_MOTION_RUNTIME_H */
