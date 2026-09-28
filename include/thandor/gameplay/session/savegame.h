/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/savegame.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_SAVEGAME_H
#define THANDOR_GAMEPLAY_SESSION_SAVEGAME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/savegame. */

/* In-game save page action (ui_templates.h: saveGameSaveButton); InGameSaveName_UpdateSaveActionValidity
   enables it only for a valid typed save name. */
#define INGAME_ACTION_SAVE_GAME_SAVE 0x1210
/* Row selection in the save list (saveGameList) and the Delete button (saveGameDeleteButton). */
#define INGAME_ACTION_SAVE_GAME_SELECT 0x120F
#define INGAME_ACTION_SAVE_GAME_DELETE 0x1219
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056C030 */
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameList_SelectAndRefreshDetail(UiPointerListControl *catalogList);

/* 0x0056C190 */
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog
          (InGameSaveGamePageControlAddress32 deleteButton);

/* 0x0056BDD0 */
void __thandor_void_preserve_eax_ecx_edx InGameSaveGamePage_RebuildCatalog(UiNodeBase *saveMenuButton);

/* 0x0056C230 */
void __thandor_void_preserve_eax_ecx_edx InGameSaveGame_SaveSelectedOrTypedName(UiNodeBase *saveButton);

/* 0x0056C2F0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl);

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_H */
