/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/notifications.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_NOTIFICATIONS_H
#define THANDOR_UI_INGAME_NOTIFICATIONS_H

#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/notifications. */

/* Functions are grouped by semantic ownership. */

void InGameRuntime_ProcessQueuedSessionNotificationTimer();

extern uint32_t g_InGameSessionNotificationTimeoutTicks;

#endif /* THANDOR_UI_INGAME_NOTIFICATIONS_H */
