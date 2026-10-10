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

/* Not in the original: the action number (EndMovieCommandAction, 1..2) the end-movie command table gives keyCode
   with the held modifiers heldFlags, 0 when no record matches. Its records hold character codes (0x71 'q',
   0x70 'p': the character events of Keyboard_OnChar), not the 0x30000 key-down codes. Read-only, for the key
   binding list (ui/core/key_bindings.h). */
uint32_t EndMovieCommands_FindBinding(uint32_t keyCode,UiKeyboardStateMask heldFlags);

#endif /* THANDOR_UI_FRONTEND_END_MOVIE_COMMANDS_H */
