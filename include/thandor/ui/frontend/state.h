/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/state.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_STATE_H
#define THANDOR_UI_FRONTEND_STATE_H

#include <thandor/core/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

#include <atomic>

void FrontendRuntime_TimerCountdownTick();

void FrontendRomTransition_AdvanceElapsedTicks();

bool FrontendRuntime_DispatchCommandByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime);

/* Not in the original: the action number (FrontendHotkeyAction, 1..2) the frontend hotkey table gives keyCode with
   the held modifiers heldFlags, 0 when no record matches. Read-only, for the key binding list
   (ui/core/key_bindings.h). */
uint32_t FrontendHotkeys_FindBinding(uint32_t keyCode,UiKeyboardStateMask heldFlags);

void FrontendState_DispatchCode(FrontendStatusCode romRecordIndex);

void Frontend_StateTick();

extern uint32_t g_FrontendNetworkTickCounter;
extern std::atomic<uint32_t> g_FrontendTimerCountdownTicks; /* counted down by the 80 Hz timer thread */

/* First code unit of a level title: rich-text style code, normal or highlighted (a level that some other player
   of the session does not have). */
inline constexpr int32_t FRONTEND_TEXT_STYLE_NORMAL = 0x8000;

inline constexpr int32_t FRONTEND_TEXT_STYLE_HIGHLIGHTED = 0x8001;

void FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext);

#endif /* THANDOR_UI_FRONTEND_STATE_H */
