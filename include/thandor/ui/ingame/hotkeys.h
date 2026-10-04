/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/hotkeys.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_HOTKEYS_H
#define THANDOR_UI_INGAME_HOTKEYS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/hotkeys. */

/* Command codes the hotkeys queue in network games (InGameHotkeys_DispatchCommandByFlags); a local game calls the
   handler directly. */
#define INGAME_COMMAND_TOGGLE_PAUSE 0x370 /* InGameCommand_TogglePauseRequest */
#define INGAME_COMMAND_ADJUST_GAME_SPEED 0x3F0 /* InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks */

/* Functions are grouped by semantic ownership. */

Bool8 InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot);

#endif /* THANDOR_UI_INGAME_HOTKEYS_H */
