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

/* Technology window actions (InGameUiImage.technologyResearchButton, technologyAreaTab1..7) */
#define INGAME_ACTION_TECHNOLOGY_RESEARCH 0x1013
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB1 0x1014
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB2 0x1015
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB3 0x1016
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB4 0x1017
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB5 0x1018
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB6 0x1019
#define INGAME_ACTION_TECHNOLOGY_AREA_TAB7 0x101A
/* Technology texts: name 0x300000 + 2 * technology id, description the id after it */
#define TECHNOLOGY_TEXT_ID_BASE 0x300000
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
