/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_COMMANDS_H
#define THANDOR_UI_INGAME_COMMANDS_H

#include <thandor/gameplay/army/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* g_UiCommandRuntimeFlags bits that end the in-game session loop (InGameRuntime_RunSessionUntilExit) */
inline constexpr int32_t UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING = 0x800; /* an end trigger fired and chose the end movie
                                                           (set in gameplay/session/level_script.cpp) */
inline constexpr int32_t UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED = 0x10000; /* command 150 with flag bit 1: the session is closed
                                                          (InGameCommand_HandlePlayerDeparture) */
inline constexpr int32_t UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT = 0x20000; /* command 150 reported the local player's departure */

/* sessionFlags bit: the player's machine renders too few frames (set/cleared through command 0x340 by
   InGameHud_UpdateStatusCountersAndSessionPrompts; shown as a highlighted "W" in the player roster) */
inline constexpr int32_t PLAYER_SESSION_FLAG_SLOW_RENDERING = 0x02;

/* UI action toggling the technology window (InGameTechnologyPanel_ToggleForSelection); suppressed by
   InGameSelectionDetailPanel_Rebuild when the single selected army has no available technology */
inline constexpr int32_t INGAME_ACTION_TECHNOLOGY_WINDOW = 0x1010;

/* flags of InGameCommand_HandlePlayerDeparture; neither bit: the player left the session */
inline constexpr int32_t INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER = 0x01; /* destroy every army of the player's faction */
inline constexpr int32_t INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION = 0x02; /* sets UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED */

/* Action id of the results screen's resultsContinueButton (InGameResultsScreen_ContinueOrMarkReady);
   a network host only shows it once every client has pressed its own
   (FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton). */
inline constexpr int32_t INGAME_ACTION_RESULTS_CONTINUE = 0x101B;

/* Bit of the last argument of InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState (command
   INGAME_COMMAND_EDITOR_ACTIVE_STATE): set leaves the editor, clear enters it. */
inline constexpr int32_t EDITOR_ACTIVE_STATE_LEAVE = 0x04;

/* Message history text of another player's departure (rich text: selector 0 = player name) */
inline constexpr int32_t TEXT_ID_PLAYER_DEPARTED = 0xFF08;

void InGameCommand_TogglePauseRequest
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

void InGameCommand_ExecuteLocalPlacementFromSelection(PlayerRuntimeId playerId,CommandPayload headingAngle,
          CommandPayload worldXQ12,CommandPayload worldYQ12);

void InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control);

void InGameSelectionGroupButton_RecallOrStoreGroup(UiCommandSpriteButtonControl *control);

void UiCommandRuntime_CallbackNoOp();

void InGameCommand_HandlePlayerDeparture
          (PlayerOrFactionRuntimeId32 playerOrFactionId,uint32_t value1,uint32_t value2,
          GameEntityCommandFlags flags);

void UiCommandRuntimeFlags_ApplyClearSetToggleMasks(PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
          UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask);

extern InGameUiCommandModeActionHandlerPage11 g_InGameUiActionHandlersPage11;

#endif /* THANDOR_UI_INGAME_COMMANDS_H */
