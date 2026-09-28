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

/* Movie_Open flags. Intro and end movies are streamed; in-game and briefing movies are loaded from the packages
   only. Movie_Open clears MOVIE_OPEN_PACKAGE_ONLY when the loose path was not used, so a nonzero
   MovieRuntime.openFlags means streaming. */
#define MOVIE_OPEN_STREAM 0x00000001 /* load at most MOVIE_INITIAL_VIDEO_MAX_BYTES, the worker thread streams the rest */
#define MOVIE_OPEN_PACKAGE_ONLY 0x80000000 /* skip the g_LooseMoviePathPrefix directory */
/* FLM layout and the streaming buffer (Movie_Open, Movie_AdvanceFrame, Movie_StreamWorkerThread) */
#define MOVIE_FILE_HEADER_BYTES 0x200 /* MovieFileHeader; the video stream follows it */
#define MOVIE_FLM_CONVERTER_VERSION 0x20001
#define MOVIE_RUNTIME_PIXELS_OFFSET 0x220 /* MovieRuntime: 0x200-byte gfx header and one 0x20-byte subresource
                                             entry, then the ARGB frame */
#define MOVIE_STREAM_BUFFER_MAX_BYTES 0x3C0000 /* allocation cap of the FLM buffer when streaming */
#define MOVIE_INITIAL_VIDEO_MAX_BYTES 0x3A2000 /* video bytes Movie_Open reads when streaming */
#define MOVIE_REFILL_LIMIT_BYTES 0x3A2200 /* refills only while header + loaded video stay below this */
#define MOVIE_REFILL_CHUNK_BYTES 0x1E000 /* bytes per refill; also the minimum buffered ahead of a decode */
#define MOVIE_COMPACT_SHIFT_BYTES 0x1E0000 /* played bytes dropped from the buffer front at once */
#define MOVIE_MAX_AUDIO_TRACKS 14 /* MovieFileHeader.audioTrackBytes; Movie_Open plays no audio for larger counts */
/* FLM frame tokens (low 5 bits of the next stream dword, Movie_DecodeFrame4x4Delta / Movie_EncodeFrame4x4*):
   0..24 start an 8-byte colour block with that base luma; the skip tokens keep blocks of the previous frame,
   the count (minus its bias) sits in the bits above the token */
#define MOVIE_TOKEN_SKIP_SHORT 0x19 /* 1 byte: skip 1..8 blocks */
#define MOVIE_TOKEN_SKIP_MEDIUM 0x1A /* 2 bytes: skip 9..0x808 blocks */
#define MOVIE_TOKEN_SKIP_LONG 0x1B /* 4 bytes: skip 0x809 or more blocks */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004A8040 */
StatusResult Movie_EncodeFlmBufferFromFrameProvider
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *outputBuffer,MovieFrameProviderProc *frameProvider);

/* 0x00563FF0 */
void MoviePlayback_AdvanceScheduledFrameAndTick(void);

/* 0x004A8590 */
MovieOpenResult Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path);

/* 0x004A8A20 */
MovieFrameDimensionsEdxEax8 Movie_GetFrameDimensions(void);

/* 0x004A8A40 */
void Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15);

/* 0x004A8C00 */
uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext);

/* 0x004A8D50 */
void Movie_Rewind(void);

/* 0x004A8D90 */
void Movie_Close(void);

/* 0x005657D0 */
void EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime);

/* 0x00565810 */
void EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

/* 0x005739C0 */
void IntroMovie_TimerTick(void);

/* 0x004A7030 */
uint32_t Movie_EncodeFrame4x4Keyframe(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *sourcePixels);

/* 0x004A7770 */
uint32_t Movie_EncodeFrame4x4Delta(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *previousFramePixels,uint32_t *currentFramePixels);

/* 0x004A8A60 */
MovieFrameResult Movie_AdvanceFrame(void);

/* 0x00564080 */
void MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

/* 0x004A81C0 */
uint32_t Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          uint8_t *encodedFrame);

/* 0x004A6FB0 */
uint32_t MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888);

/* 0x004A7000 */
uint32_t MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888);

/* Not in the original: fills g_MovieChromaLumaToArgb (the original shipped it precomputed). */
void Movie_BuildChromaLumaTable(void);

#endif /* THANDOR_MOVIE_RUNTIME_PLAYBACK_H */
