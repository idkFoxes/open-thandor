/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/session_schedule.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_MOVIE_RUNTIME_SESSION_SCHEDULE_H
#define THANDOR_MOVIE_RUNTIME_SESSION_SCHEDULE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: movie/runtime/session_schedule. */

/* Functions are grouped by semantic ownership. */

void MoviePlayback_AdvanceScheduledFrameAndTick(void);

void MoviePlayback_AdvanceToFrameAndPresent(MovieFrameIndex targetFrame);

extern uint32_t g_MoviePlaybackCurrentFrame;

#endif /* THANDOR_MOVIE_RUNTIME_SESSION_SCHEDULE_H */
