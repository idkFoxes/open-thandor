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
/* Largest run of kept blocks the short and the medium skip token encode */
#define MOVIE_SKIP_SHORT_MAX_BLOCKS 8
#define MOVIE_SKIP_MEDIUM_MAX_BLOCKS 0x808
/* Low 5 bits of a stream dword: the frame token (MOVIE_TOKEN_SKIP_*, or the base luma of a colour block) */
#define MOVIE_TOKEN_MASK 0x1f
/* Highest base luma of a colour block (tokens 0..24) */
#define MOVIE_TOKEN_BASE_LUMA_MAX 24
/* FLM colour: 5-bit luma in bits 0-4 below the 10-bit chroma code (hue in bits 5-9, saturation in 10-14);
   a colour block stores the chroma code in bits 21-30 of its second dword */
#define MOVIE_COLOR_LUMA_MASK 0x1f
#define MOVIE_COLOR_HUE_MASK 0x3e0
#define MOVIE_COLOR_SATURATION_MASK 0x7c00
#define MOVIE_COLOR_CHROMA_MASK 0x7fe0
/* Bit 31 of a colour block's second dword: every luma step counts twice (4-bit levels 0..15 halved) */
#define MOVIE_BLOCK_DOUBLE_STEPS 0x80000000
/* Largest luma level of a colour block with doubled steps */
#define MOVIE_BLOCK_WIDE_LEVEL_MAX 15
/* 0x8000 * sqrt(3): the blue-green axis of the chroma vector (MovieColor_ComputeChromaCodeFromRgb888) */
#define MOVIE_CHROMA_SQRT3_Q15 0xddb4
/* 2^16 / 3: (r + g + b) * this >> 19 is the channel average scaled to 5 bits (MovieColor_ComputeLuma5FromRgb888) */
#define MOVIE_LUMA_THIRD_Q16 0x5555
/* Functions are grouped by semantic ownership. */

bool Movie_EncodeFlmBufferFromFrameProvider
          (MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *outputBuffer,MovieFrameProviderProc *frameProvider,uint32_t *outByteCount);

void MoviePlayback_AdvanceScheduledFrameAndTick(void);

bool Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path,uint32_t *outPlaybackRateHz,uint32_t *outError);

MovieFrameDimensions Movie_GetFrameDimensions(void);

void Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15);

uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext);

void Movie_Rewind(void);

void Movie_Close(void);

void EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime);

void EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

void IntroMovie_TimerTick(void);

uint32_t Movie_EncodeFrame4x4Keyframe(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *sourcePixels);

uint32_t Movie_EncodeFrame4x4Delta(MoviePixelDimension frameHeightPixels,MoviePixelDimension frameWidthPixels,
          uint32_t *encodedOutput,uint32_t *previousFramePixels,uint32_t *currentFramePixels);

bool Movie_AdvanceFrame(MovieRuntime **outMovie,uint32_t *outEndCode);

void MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

uint32_t Movie_DecodeFrame4x4Delta
          (MoviePixelDimension heightPixels,MoviePixelDimension widthPixels,uint32_t *destinationArgb,
          uint8_t *encodedFrame);

uint32_t MovieColor_ComputeChromaCodeFromRgb888(PackedRgb24 rgb888);

uint32_t MovieColor_ComputeLuma5FromRgb888(PackedRgb24 rgb888);

/* Not in the original: fills g_MovieChromaLumaToArgb (the original shipped it precomputed). */
void Movie_BuildChromaLumaTable(void);

#endif /* THANDOR_MOVIE_RUNTIME_PLAYBACK_H */
