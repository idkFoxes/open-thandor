/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/hotkeys.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_HOTKEYS_H
#define THANDOR_UI_INGAME_HOTKEYS_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

bool InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot);

/* Not in the original: the action number (InGameHotkeyAction, 1..15) the in-game hotkey table gives keyCode with the
   held modifiers heldFlags, 0 when no record matches. Read-only, for the key binding list (ui/core/key_bindings.h). */
uint32_t InGameHotkeys_FindBinding(uint32_t keyCode,UiKeyboardStateMask heldFlags);

#endif /* THANDOR_UI_INGAME_HOTKEYS_H */
