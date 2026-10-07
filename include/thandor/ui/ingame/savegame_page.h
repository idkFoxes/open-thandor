/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/savegame_page.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_SAVEGAME_PAGE_H
#define THANDOR_UI_INGAME_SAVEGAME_PAGE_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>
#include <thandor/core/text/path.h>

/* In-game save page action (InGameUiImage: saveGameSaveButton); InGameSaveName_UpdateSaveActionValidity
   enables it only for a valid typed save name. */
inline constexpr int32_t INGAME_ACTION_SAVE_GAME_SAVE = 0x1210;
/* Row selection in the save list (saveGameList) and the Delete button (saveGameDeleteButton). */
inline constexpr int32_t INGAME_ACTION_SAVE_GAME_SELECT = 0x120F;
inline constexpr int32_t INGAME_ACTION_SAVE_GAME_DELETE = 0x1219;
/* Caption of the last row of the save list, the "new savegame" entry (InGameSaveGamePage_RebuildCatalog). */
inline constexpr int32_t TEXT_ID_SAVE_GAME_NEW_ROW = 0x2151;

void InGameSaveGameList_SelectAndRefreshDetail(UiPointerListControl *catalogList);

void InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog(InGameSaveGamePageControlAddress32 deleteButton);

void InGameSaveGamePage_RebuildCatalog(UiNodeBase *saveMenuButton);

void InGameSaveGame_SaveSelectedOrTypedName(UiNodeBase *saveButton);

void InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl);

extern uint16_t g_SaveDirectoryUtf16[5];
extern uint16_t g_ScenarioCatalogPathScratchUtf16[THANDOR_PATH_CAPACITY];

#endif /* THANDOR_UI_INGAME_SAVEGAME_PAGE_H */
