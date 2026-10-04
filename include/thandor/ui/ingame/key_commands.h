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

/* Submodule: ui/ingame/key_commands. */

/* Slots of the in-game notification queue (notificationQueue, InGameNotificationQueue_InsertPriorityRecord) */
#define INGAME_NOTIFICATION_QUEUE_SLOTS 4

/* Notification target button: cursor frame after a jump (the next click cancels), and the panel subresource shown
   when no notification movie plays */
#define INGAME_NOTIFICATION_CURSOR_CANCEL 0x1B

/* Queued player command codes of the selection hotkeys (network games; see the key table in ui/ingame/key_commands.cpp) */
#define INGAME_COMMAND_SELECT_OWN_AIRCRAFT_PADS 0x8F0 /* InGameSelection_SelectAllOwnAircraftPads */
#define INGAME_COMMAND_SELECTION_RESET_MOVEMENT 0xE10 /* S: PlayerSelection_ResetMovementPruneAndRecenterEntries */
#define INGAME_COMMAND_SELECTION_STOP_MOVEMENT 0xE30 /* Shift+S: PlayerSelection_StopMovement (stay where they are) */
#define INGAME_COMMAND_SELECTION_CANCEL_TARGETS 0xE50 /* Alt+S: PlayerSelection_CancelTargets */
#define INGAME_COMMAND_SELECTION_SELF_DESTRUCT 0xE70 /* Alt+D: PlayerSelection_SelfDestruct */

/* Functions are grouped by semantic ownership. */

Bool8 InGameUiRuntime_DispatchCommandByCodeAndModifierFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *world);

void InGameUiRuntime_ResetNotificationButtonCursor(void *worldView);

void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *world);

void InGameNotificationQueue_InsertPriorityRecord(InGameNotificationPayloadKind payloadKind,uint32_t payloadReserved,
          uint32_t orientationValue,AngleTurn32 orientationAngle,
          Q12 secondaryWorldCoordinateQ12,Q12 primaryWorldCoordinateQ12,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId);

extern uint32_t g_UiCommandRuntimeFlags;

#endif /* THANDOR_UI_INGAME_KEY_COMMANDS_H */
