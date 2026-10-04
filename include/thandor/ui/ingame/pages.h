/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/pages.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_PAGES_H
#define THANDOR_UI_INGAME_PAGES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/pages. */

/* Pages of the in-game window page stack (InGameUiImage.gameWindowPageStack) */
#define INGAME_WINDOW_PAGE_NONE 0 /* no window open, the world view is shown */
#define INGAME_WINDOW_PAGE_TECHNOLOGY 2
#define INGAME_WINDOW_PAGE_GAME_MENU 3
#define INGAME_WINDOW_PAGE_QUIT_MENU 4
#define INGAME_WINDOW_PAGE_SAVE_GAME 5
#define INGAME_WINDOW_PAGE_GRAPHICS_SETTINGS 6
#define INGAME_WINDOW_PAGE_SOUND_SETTINGS 7
#define INGAME_WINDOW_PAGE_MISSION_HELP 8

/* Mission help text: TEXT_ID_LEVEL_DESCRIPTION_BASE + 7 + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * level title index +
   active faction (InGameMissionHelpPage_Toggle) */
#define TEXT_ID_MISSION_HELP_BASE 0x230017

/* Functions are grouped by semantic ownership. */

void InGameMissionHelpPage_Toggle(UiNodeBase *source);

void InGameResultsScreen_SelectChartTab(UiSelectableControl *selectableControl);

void InGameTechnologyPanel_ToggleForSelection(UiNodeBase *source);

extern InGameUiActionHandlerPage10Prefix40 g_InGameUiActionHandlersPage10;

/* g_UiCommandRuntimeFlags bits that gate the simulation step (InGameRuntime_UpdateSimulationAndNetworkTick) */
#define UI_COMMAND_RUNTIME_FLAG_PAUSED 0x01 /* toggled once every player agrees

                                               (InGameCommand_TogglePauseRequest); set at session start */
#define UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED 0x08 /* an end trigger ended the local faction

                                                            (InGameConditionRuntime_UpdateScheduledRecords);
                                                            the step then sets occupancy bit 0 on every cell */
#define UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS 0x10 /* set with PAUSED at session start, cleared with it when

                                                            every player is ready
                                                            (FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus) */
#define UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED 0x100 /* set with LOCAL_FACTION_ENDED; the world input

                                                              handlers (gameplay/input/world.c) then ignore the map */
/* g_UiCommandRuntimeFlags bit that ends the results screen after the end movie (Frontend_PlaySelectedEndMovie) */
#define UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED 0x1000 /* set by the results buttons (actions 0x101B and 0x1025,

                                                         ui/ingame/commands.c) */
/* further g_UiCommandRuntimeFlags bits (gameplay/session/runtime.c, gameplay/ai/planning.c) */
#define UI_COMMAND_RUNTIME_FLAG_AI_PLANNING_OFF 0x02 /* skips the AI planning phase in local games; no writer

                                                        with a constant mask in the original, so it can only come
                                                        from UiCommandRuntimeFlags_ApplyClearSetToggleMasks */
#define UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE 0x04 /* set with PAUSED by

                                                                     InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState;
                                                                     world sounds, camera keys and the full
                                                                     simulation step are skipped meanwhile */
#define UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING 0x20 /* an army asset waits for placement on the map

                                                          (InGameCommand_ExecuteLocalPlacementFromSelection) */
#define UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN 0x2000 /* the placement overlay was drawn onto the field

                                                                  grid (InGameUiRoot_UpdateFrame) */
#define UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED 0x40000 /* toggled by typing the cheat code into the chat line

                                                          (InGameChatInput_SendLineOrCheckCheatPhrase) */
#define UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD 0x100000 /* cheat hotkey: build and research times / 16 */
#define UI_COMMAND_RUNTIME_FLAG_CHEAT_PHRASE_ENTERED 0x80000 /* set with every cheat toggle by the chat phrase;

                                                                no reader found */
/* SelectionPlayerRuntimeBlock.sessionFlags bit: the player asks for a pause (shown as "P" in the player roster;
   toggled by InGameCommand_TogglePauseRequest) */
#define PLAYER_SESSION_FLAG_PAUSE_REQUESTED 0x01

/* g_UiCommandRuntimeFlags bits of windows that pause a local game while open (mission help:
   InGameMissionHelpPage_Toggle, settings: InGameSettingsPage_ToggleAndSynchronizeControls) */
#define UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE 0x4000 /* an open window paused the game */
#define UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW 0x400 /* the game was already paused when it opened */

/* Buttons of the quit game window (InGameUiImage.quitMenuSurrenderButton / quitMenuRestartMissionButton) */
#define INGAME_ACTION_QUIT_SURRENDER 0x101E /* command 150 mode 1: destroys the local faction's armies */
#define INGAME_ACTION_QUIT_RESTART_MISSION 0x1027 /* command 150 mode 2 (label unverified) */

void InGameResultsScreen_ContinueOrMarkReady(void *source);

void InGameEndMovie_Skip(void *source);

void InGameQuitMenu_RestartMission(UiNodeBase *source);

void InGameQuitMenu_OpenAndRefreshButtons(InGameCommandPanelSourceAddress32 source);

void InGameResultsScreen_CloseLocally(UiNodeBase *source);

void InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source);

void InGameQuitMenu_AbortMission(UiNodeBase *source);

void InGameQuitMenu_Surrender(UiNodeBase *source);

void InGameMissionHelpPage_SelectBriefingTab(UiNodeBase *sourceNode);

void InGameMissionHelpPage_SelectKeyboardTab(UiNodeBase *sourceNode);

void InGameMissionHelpPage_SelectMouseTab(UiNodeBase *sourceNode);

#endif /* THANDOR_UI_INGAME_PAGES_H */
