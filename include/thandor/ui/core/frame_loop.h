/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/frame_loop.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CORE_FRAME_LOOP_H
#define THANDOR_UI_CORE_FRAME_LOOP_H

#include <thandor/core/types.h>
#include <thandor/ui/core/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/core/frame_loop. */

/* Functions are grouped by semantic ownership. */

void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void);

void UiFrame_ProcessAndPresent(void);

void UiFrame_FlushInputAndResetPendingTicks(void);

void UiFrame_Update(UiStopMessageCode stopMessageCode);

void UiFrame_Draw(void);

extern uint32_t g_UiPendingFrameTicks;

#endif /* THANDOR_UI_CORE_FRAME_LOOP_H */
