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

/* Functions are grouped by semantic ownership. */

Bool8 Movie_Open(MovieOpenFlags movieOpenFlags,uint16_t *path,uint32_t *outPlaybackRateHz,uint32_t *outError);

MovieFrameDimensions Movie_GetFrameDimensions(void);

void Movie_SetAudioGainQ15(MovieAudioGainQ15 gainQ15);

uint32_t __stdcall Movie_StreamWorkerThread(void *unusedThreadContext);

void Movie_Rewind(void);

void Movie_Close(void);

void IntroMovie_TimerTick(void);

Bool8 Movie_AdvanceFrame(MovieRuntime **outMovie,uint32_t *outEndCode);

extern MovieRuntime *g_ActiveMovie;

extern MovieAudioGainQ15 g_MovieDefaultAudioGainQ15;
extern MovieAudioGainQ15 g_MovieAlternateAudioGainQ15;

#endif /* THANDOR_MOVIE_RUNTIME_PLAYBACK_H */
