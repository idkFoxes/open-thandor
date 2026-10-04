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

#endif /* THANDOR_UI_INGAME_PAGES_H */
