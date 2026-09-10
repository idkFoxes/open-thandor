#ifndef THANDOR_GAMEPLAY_SESSION_SAVEGAME_H
#define THANDOR_GAMEPLAY_SESSION_SAVEGAME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/savegame. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056C030 */
void InGameSaveGameList_SelectAndRefreshDetail(InGameCatalogDetailPageCatalogListPtr catalogList);

/* 0x0056C190 */
undefined8 InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog (InGameSaveGamePageControlAddress32 saveGamePageControl);

/* 0x0056BDD0 */
undefined8 InGameSaveGamePage_RebuildCatalog(UiRootNode *savePageRoot);

/* 0x0056C230 */
void InGameSaveGame_SaveSelectedOrTypedName(void *source);

/* 0x0056C2F0 */
undefined8 InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl);

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_H */
