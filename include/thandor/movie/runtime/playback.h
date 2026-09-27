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
StatusResult __thandor_eax_cf_preserve_ecx_edx
Movie_EncodeFlmBufferFromFrameProvider
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *outputBuffer,MovieFrameProviderProc *frameProvider);

/* 0x00563FF0 */
void __thandor_void_preserve_eax_ecx_edx MoviePlayback_AdvanceScheduledFrameAndTick(void);

/* 0x004A8590 */
MovieOpenResult __thandor_eax_cf_preserve_edx Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path);

/* 0x004A8A20 */
MovieFrameDimensionsEdxEax8 __thandor_eax_edx_cf_preserve_ecx Movie_GetFrameDimensions(void);

/* 0x004A8A40 */
void __thandor_preserve_eax Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15);

/* 0x004A8C00 */
uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext);

/* 0x004A8D50 */
void Movie_Rewind(void);

/* 0x004A8D90 */
void __thandor_void_preserve_eax_ecx_edx Movie_Close(void);

/* 0x005657D0 */
void __thandor_preserve_eax EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime);

/* 0x00565810 */
void __thandor_void_preserve_eax_ecx_edx
EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

/* 0x005739C0 */
void IntroMovie_TimerTick(void);

/* 0x004A7030 */
uint32_t __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Keyframe
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *sourcePixels);

/* 0x004A7770 */
uint32_t __thandor_eax_preserve_ecx_edx
Movie_EncodeFrame4x4Delta
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *previousFramePixels,uint32_t *currentFramePixels);

/* 0x004A8A60 */
MovieFrameResult __thandor_eax_cf_preserve_ecx_edx Movie_AdvanceFrame(void);

/* 0x00564080 */
void __thandor_void_preserve_eax_ecx_edx
MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

/* 0x004A81C0 */
uint32_t __thandor_eax_preserve_ecx_edx
Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          uint8_t *encodedFrame);

/* 0x004A6FB0 */
uint32_t __thandor_eax_preserve_ecx_edx MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888);

/* 0x004A7000 */
uint32_t __thandor_eax_preserve_ecx_edx MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888);

/* Not in the original: fills g_MovieChromaLumaToArgb (the original shipped it precomputed). */
void Movie_BuildChromaLumaTable(void);

#endif /* THANDOR_MOVIE_RUNTIME_PLAYBACK_H */
