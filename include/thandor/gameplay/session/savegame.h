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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056C030 */
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameList_SelectAndRefreshDetail(InGameCatalogDetailPageCatalogListPtr catalogList);

/* 0x0056C190 */
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog
          (InGameSaveGamePageControlAddress32 saveGamePageControl);

/* 0x0056BDD0 */
void __thandor_void_preserve_eax_ecx_edx InGameSaveGamePage_RebuildCatalog(UiRootNode *savePageRoot);

/* 0x0056C230 */
void __thandor_void_preserve_eax_ecx_edx InGameSaveGame_SaveSelectedOrTypedName(void *source);

/* 0x0056C2F0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl);

#endif /* THANDOR_GAMEPLAY_SESSION_SAVEGAME_H */
