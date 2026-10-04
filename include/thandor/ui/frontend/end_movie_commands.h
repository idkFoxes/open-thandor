/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/end_movie_commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_END_MOVIE_COMMANDS_H
#define THANDOR_UI_FRONTEND_END_MOVIE_COMMANDS_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

void EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime);

void EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime);

#endif /* THANDOR_UI_FRONTEND_END_MOVIE_COMMANDS_H */
