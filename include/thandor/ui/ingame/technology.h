#ifndef THANDOR_UI_INGAME_TECHNOLOGY_H
#define THANDOR_UI_INGAME_TECHNOLOGY_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/technology. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056AE70 */
void InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl);

/* 0x0056B450 */
undefined8 InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot);

/* 0x00569DF0 */
void UiCatalogGroup48_RebuildGrid(UiNodeBase *node);

/* 0x0056A050 */
void UiCatalogGroup42_RebuildGrid(UiNodeBase *node);

/* 0x0056AEF0 */
void InGameTechnologyResearch_StartSelected(void *source);

/* 0x0056B050 */
void InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot);

#endif /* THANDOR_UI_INGAME_TECHNOLOGY_H */
