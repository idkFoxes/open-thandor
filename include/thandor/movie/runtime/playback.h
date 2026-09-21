/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/playback.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_MOVIE_RUNTIME_PLAYBACK_H
#define THANDOR_MOVIE_RUNTIME_PLAYBACK_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: movie/runtime/playback. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004A8040 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Movie_EncodeFlmBufferFromFrameProviderCf
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint *outputBuffer,MovieFrameProviderCfProc *frameProvider);

/* 0x00563FF0 */
void __thandor_void_preserve_eax_ecx_edx MoviePlayback_AdvanceScheduledFrameAndTick(void);

/* 0x004A8590 */
MovieOpenEaxCf5 __thandor_eax_cf_preserve_edx Movie_Open(MovieOpenFlags movieOpenFlags,word *path);

/* 0x004A8A20 */
MovieFrameDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx Movie_GetFrameDimensions(void);

/* 0x004A8A40 */
void __thandor_preserve_eax Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15);

/* 0x004A8C00 */
dword Movie_StreamWorkerThread(void *unusedThreadContext);

/* 0x004A8D50 */
void Movie_Rewind(void);

/* 0x004A8D90 */
void __thandor_void_preserve_eax_ecx_edx Movie_Close(void);

/* 0x005657D0 */
void __thandor_preserve_eax EndMovieUiRuntime_HandleModeTransitionCf(void *endMovieRuntime);

/* 0x00565810 */
void __thandor_void_preserve_eax_ecx_edx
EndMovieUiRuntime_DispatchCommandByFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

/* 0x005739C0 */
void IntroMovie_TimerTick(void);

/* 0x004A7030 */
uint __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Keyframe
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint *encodedOutput,uint *sourcePixels);

/* 0x004A7770 */
uint __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Delta
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint *encodedOutput,uint *previousFramePixels,uint *currentFramePixels);

/* 0x004A8A60 */
MovieAdvanceFrameEaxCf5 __thandor_eax_cf_preserve_ecx_edx Movie_AdvanceFrame(void);

/* 0x00564080 */
void __thandor_void_preserve_eax_ecx_edx
MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

/* 0x004A81C0 */
dword __thandor_eax_preserve_ecx_edx
Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,dword *destinationArgb,
          byte *encodedFrame);

/* 0x004A6FB0 */
uint __thandor_eax_preserve_ecx_edx MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888);

/* 0x004A7000 */
uint __thandor_eax_preserve_ecx_edx MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888);

#endif /* THANDOR_MOVIE_RUNTIME_PLAYBACK_H */
