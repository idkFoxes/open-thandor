/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/technology.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_TECHNOLOGY_H
#define THANDOR_UI_INGAME_TECHNOLOGY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/technology. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056AE70 */
void __thandor_void_preserve_eax_ecx_edx
InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl);

/* 0x0056B450 */
void __thandor_void_preserve_eax_ecx_edx
InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot);

/* 0x00569DF0 */
void __thandor_void_preserve_eax_ecx_edx UiCatalogGroup48_RebuildGrid(UiNodeBase *node);

/* 0x0056A050 */
void __thandor_void_preserve_eax_ecx_edx UiCatalogGroup42_RebuildGrid(UiNodeBase *node);

/* 0x0056AEF0 */
void __thandor_void_preserve_eax_ecx_edx InGameTechnologyResearch_StartSelected(void *source);

/* 0x0056B050 */
void __thandor_void_preserve_eax_ecx_edx InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot);

#endif /* THANDOR_UI_INGAME_TECHNOLOGY_H */
