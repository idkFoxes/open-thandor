/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/state.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_STATE_H
#define THANDOR_UI_FRONTEND_STATE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/state. */

/* Functions are grouped by semantic ownership. */

void __cdecl FrontendRuntime_TimerCountdownTick(void);

void __cdecl FrontendRomTransition_AdvanceElapsedTicks(void);

Bool8 FrontendRuntime_DispatchCommandByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime);

void FrontendState_DispatchCode(FrontendStatusCode romRecordIndex);

void Frontend_StateTick(void);

extern uint32_t g_FrontendNetworkTickCounter;
extern uint32_t g_FrontendTimerCountdownTicks;

#endif /* THANDOR_UI_FRONTEND_STATE_H */
