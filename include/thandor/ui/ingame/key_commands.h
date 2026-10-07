/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/key_commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_KEY_COMMANDS_H
#define THANDOR_UI_INGAME_KEY_COMMANDS_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Slots of the in-game notification queue (notificationQueue, InGameNotificationQueue_InsertPriorityRecord) */
inline constexpr int32_t INGAME_NOTIFICATION_QUEUE_SLOTS = 4;

/* Notification target button: cursor frame after a jump (the next click cancels), and the panel subresource shown
   when no notification movie plays */
inline constexpr int32_t INGAME_NOTIFICATION_CURSOR_CANCEL = 0x1B;

bool InGameUiRuntime_DispatchCommandByCodeAndModifierFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *world);

void InGameWorldView_ShowNextInfoText(UiSingleLineTextControl *infoText);

void InGameUiRuntime_ResetNotificationButtonCursor(void *worldView);

void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *world);

void InGameNotificationQueue_InsertPriorityRecord(InGameNotificationPayloadKind payloadKind,uint32_t payloadReserved,
          uint32_t orientationValue,AngleTurn32 orientationAngle,
          Q12 secondaryWorldCoordinateQ12,Q12 primaryWorldCoordinateQ12,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId);

extern UiCommandRuntimeFlagMask g_UiCommandRuntimeFlags;

#endif /* THANDOR_UI_INGAME_KEY_COMMANDS_H */
