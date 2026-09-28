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
/* The two dwords in front of a technology area tab (technologyAreaTabN), ((UiTechnologyAreaTabPrefix *)tab)[-1]:
   the name text id of the tab's technology and the tab's tooltip text (the expanded label). */
typedef struct UiTechnologyAreaTabPrefix {
    int32_t nameTextResourceId; /* -8: TECHNOLOGY_TEXT_ID_BASE + 2 * technology id */
    uint16_t *tooltipText;      /* -4 */
} UiTechnologyAreaTabPrefix;
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056AE70 */
void InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl);

/* 0x0056B450 */
void InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot);

/* 0x00569DF0 */
void UiCatalogGroup48_RebuildGrid(UiNodeBase *node);

/* 0x0056A050 */
void UiCatalogGroup42_RebuildGrid(UiNodeBase *node);

/* 0x0056AEF0 */
void InGameTechnologyResearch_StartSelected(void *source);

/* 0x0056B050 */
void InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot);

#endif /* THANDOR_UI_INGAME_TECHNOLOGY_H */
