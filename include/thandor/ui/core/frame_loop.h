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

#include <atomic>

void UiFrame_ProcessAndPresentWithLockTransition();

void UiFrame_ProcessAndPresent();

void UiFrame_FlushInputAndResetPendingTicks();

void UiFrame_Update(UiStopMessageCode stopMessageCode);

void UiFrame_Draw();

extern std::atomic<uint32_t> g_UiPendingFrameTicks; /* incremented by the 20 Hz frame-tick timer thread */

#endif /* THANDOR_UI_CORE_FRAME_LOOP_H */
