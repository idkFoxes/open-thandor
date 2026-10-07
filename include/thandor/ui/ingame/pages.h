/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/pages.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_PAGES_H
#define THANDOR_UI_INGAME_PAGES_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Pages of the in-game window page stack (InGameUiImage.gameWindowPageStack) */
inline constexpr int32_t INGAME_WINDOW_PAGE_NONE = 0; /* no window open, the world view is shown */
inline constexpr int32_t INGAME_WINDOW_PAGE_TECHNOLOGY = 2;
inline constexpr int32_t INGAME_WINDOW_PAGE_GAME_MENU = 3;
inline constexpr int32_t INGAME_WINDOW_PAGE_QUIT_MENU = 4;
inline constexpr int32_t INGAME_WINDOW_PAGE_SAVE_GAME = 5;
inline constexpr int32_t INGAME_WINDOW_PAGE_GRAPHICS_SETTINGS = 6;
inline constexpr int32_t INGAME_WINDOW_PAGE_SOUND_SETTINGS = 7;
inline constexpr int32_t INGAME_WINDOW_PAGE_MISSION_HELP = 8;

/* Mission help text: TEXT_ID_LEVEL_DESCRIPTION_BASE + 7 + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * level title index +
   active faction (InGameMissionHelpPage_Toggle) */
inline constexpr int32_t TEXT_ID_MISSION_HELP_BASE = 0x230017;

void InGameMissionHelpPage_Toggle(UiNodeBase *source);

void InGameResultsScreen_SelectChartTab(UiSelectableControl *selectableControl);

void InGameTechnologyPanel_ToggleForSelection(UiNodeBase *source);

extern InGameUiActionHandlerPage10Prefix40 g_InGameUiActionHandlersPage10;

/* SelectionPlayerRuntimeBlock.sessionFlags bit: the player asks for a pause (shown as "P" in the player roster;
   toggled by InGameCommand_TogglePauseRequest) */
inline constexpr int32_t PLAYER_SESSION_FLAG_PAUSE_REQUESTED = 0x01;


/* Buttons of the quit game window (InGameUiImage.quitMenuSurrenderButton / quitMenuRestartMissionButton) */
inline constexpr int32_t INGAME_ACTION_QUIT_SURRENDER = 0x101E; /* command 150 mode 1: destroys the local faction's armies */
inline constexpr int32_t INGAME_ACTION_QUIT_RESTART_MISSION = 0x1027; /* command 150 mode 2 (label unverified) */

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
