/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/loading_movie.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LOADING_MOVIE_H
#define THANDOR_GAMEPLAY_SESSION_LOADING_MOVIE_H

#include <thandor/movie/runtime/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/loading_movie. */

/* Functions are grouped by semantic ownership. */

void MoviePlayback_AdvanceScheduledFrameAndTick(void);

void MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

extern uint32_t g_MoviePlaybackCurrentFrame;

#endif /* THANDOR_GAMEPLAY_SESSION_LOADING_MOVIE_H */
