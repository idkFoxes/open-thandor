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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0050D050 */
dword __thandor_eax_preserve_ecx_edx
WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime);

/* 0x0050C7F0 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
          (AngleTurn32 elevationAngle,int screenDelta,WorldRuntimeContext *worldRuntime);

/* 0x0050C850 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn
          (int screenDelta,WorldRuntimeContext *worldRuntime);

/* 0x0050C8C0 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
          (int screenDelta,WorldRuntimeContext *worldRuntime);

/* 0x0050C920 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustHeadingAndRecomputePosition
          (int headingDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050C990 */
void __thandor_void_preserve_eax_ecx
WorldMotion_AdjustHeadingAndClearFieldGridDirty
          (int headingDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050C9C0 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustDistanceClampAndRecomputePosition
          (int distanceDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050CA80 */
void __thandor_void_preserve_eax_ecx
WorldMotion_AdjustPositionMagnitudeClamp(int magnitudeDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050CB10 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustPitchClampAndRecomputePosition
          (int pitchDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050CBE0 */
void __thandor_void_preserve_eax_ecx_edx
WorldMotion_AdjustPitchClampAndClearFieldGridDirty
          (int pitchDeltaInput,WorldRuntimeContext *worldRuntime);

/* 0x0050C770 */
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_TranslateCameraByScreenDelta
          (CameraScreenDeltaPixels screenDeltaX,dword screenDeltaY,WorldRuntimeContext *worldRuntime
          );

#endif /* THANDOR_WORLD_MOTION_RUNTIME_H */
