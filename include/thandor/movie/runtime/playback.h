#ifndef THANDOR_MOVIE_RUNTIME_PLAYBACK_H
#define THANDOR_MOVIE_RUNTIME_PLAYBACK_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: movie/runtime/playback. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004A8040 */
undefined8 __fastcall Movie_EncodeFlmBufferFromFrameProviderCf (undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,uint *param_5, undefined *param_6);

/* 0x00563FF0 */
undefined8 __cdecl MoviePlayback_AdvanceScheduledFrameAndTick(void);

/* 0x004A8590 */
dword Movie_Open(MovieOpenFlags movieOpenFlags,word *path);

/* 0x004A8A20 */
MovieFrameDimensionsEdxEax8 Movie_GetFrameDimensions(void);

/* 0x004A8A40 */
void Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15);

/* 0x004A8C00 */
dword Movie_StreamWorkerThread(void *unusedThreadContext);

/* 0x004A8D50 */
void Movie_Rewind(void);

/* 0x004A8D90 */
void Movie_Close(void);

/* 0x005657D0 */
void EndMovieUiRuntime_HandleModeTransitionCf(void *endMovieRuntime);

/* 0x00565810 */
void EndMovieUiRuntime_DispatchCommandByFlagsCf (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

/* 0x005739C0 */
void IntroMovie_TimerTick(void);

/* 0x004A7030 */
undefined8 __fastcall Movie_EncodeFrame4x4Keyframe (undefined4 param_1,undefined4 param_2,uint param_3,uint param_4,uint *param_5, uint *param_6);

/* 0x004A7770 */
ulonglong __fastcall Movie_EncodeFrame4x4Delta (undefined4 param_1,undefined4 param_2,uint param_3,uint param_4,uint *param_5,int param_6 ,ulonglong *param_7);

/* 0x004A8A60 */
MovieRuntime * Movie_AdvanceFrame(void);

/* 0x00564080 */
undefined8 MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

/* 0x004A81C0 */
dword Movie_DecodeFrame4x4Delta (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels, dword *destinationArgb,byte *encodedFrame);

/* 0x004A6FB0 */
undefined8 MovieColor_ComputeChromaCodeFromRgb888(uint param_1);

/* 0x004A7000 */
undefined8 MovieColor_ComputeLuma5FromRgb888(uint param_1);

#endif /* THANDOR_MOVIE_RUNTIME_PLAYBACK_H */
