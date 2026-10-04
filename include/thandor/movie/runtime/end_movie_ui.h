/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/movie/runtime/end_movie_ui.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_MOVIE_RUNTIME_END_MOVIE_UI_H
#define THANDOR_MOVIE_RUNTIME_END_MOVIE_UI_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: movie/runtime/end_movie_ui. */

/* Functions are grouped by semantic ownership. */

void EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime);

void EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

#endif /* THANDOR_MOVIE_RUNTIME_END_MOVIE_UI_H */
