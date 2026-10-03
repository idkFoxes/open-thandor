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
/* Technology window texts (InGameTechnologyPanel_Rebuild) */
#define TEXT_ID_TECHNOLOGY_WINDOW_TITLE 0x217C       /* rich text: selector 0 = unit name */
#define TEXT_ID_TECHNOLOGY_BUTTON_AREA_SELECTED 0x217E /* research button caption with an area tab selected */
#define TEXT_ID_TECHNOLOGY_GENERAL_DESCRIPTION 0x217F /* description shown with no area tab selected */
#define TEXT_ID_TECHNOLOGY_BUTTON_NO_AREA 0x2180     /* research button caption with no area tab selected */
#define TEXT_ID_TECHNOLOGY_AREA_TAB_LABEL 0x2181     /* rich text: selector 0 = name, selector 1 = Xenite cost */
/* Model name texts: 0x18004F + the definition's name index (ModelDefinitionRecordPrefix.nameTextIndex) */
#define TEXT_ID_MODEL_NAME_BASE 0x18004F
/* Technology slots of a model definition (researchTechnologyIds[1..28]), dealt out over the seven area tabs */
#define TECHNOLOGY_DEFINITION_SLOT_COUNT 28
#define TECHNOLOGY_AREA_TAB_COUNT 7
/* Build catalog (InGameBuildCatalog_RebuildGrid) and special build catalog (InGameSpecialBuildCatalog_RebuildGrid) */
#define BUILD_CATALOG_ENTRY_COUNT 48
#define BUILD_CATALOG_MAX_COLUMNS 8
#define SPECIAL_BUILD_CATALOG_ENTRY_COUNT 42
#define SPECIAL_BUILD_CATALOG_MAX_COLUMNS 6
/* assetFlags14 bits of a catalog record */
#define BUILD_CATALOG_ASSET_FLAG_BUILDABLE 0x1
#define BUILD_CATALOG_ASSET_FLAG_CAPABILITY_8 0x8    /* capability every model of runtime class 22 provides */
#define BUILD_CATALOG_ASSET_FLAG_SPECIAL 0x10        /* offered in the special build catalog */
#define BUILD_CATALOG_ASSET_CAPABILITY_MASK 0xEE     /* all capability bits of the normal build catalog */
/* UiTechnologyAreaTabPrefix (the two dwords in front of each technology area tab, INGAME_UI(root,
   technologyAreaTabN_prefix)) is generated with the template: thandor/generated/ui_templates.h. */
/* The UiTechnologyAreaTabPrefix in front of a technology area tab the code has only as a node pointer. */
#define TECHNOLOGY_AREA_TAB_PREFIX(tab) UI_TEMPLATE_NODE_PREFIX(UiTechnologyAreaTabPrefix,tab)
/* Functions are grouped by semantic ownership. */

void InGameTechnologyAreaTab_SelectAndRebuild(UiSelectableControl *selectableControl);

void InGameTechnologyPanel_ResetAndSelectCurrentArea(UiRootNode *inGameRoot);

void InGameBuildCatalog_RebuildGrid(UiNodeBase *node);

void InGameSpecialBuildCatalog_RebuildGrid(UiNodeBase *node);

void InGameTechnologyResearch_StartSelected(void *source);

void InGameTechnologyPanel_Rebuild(UiRootNode *inGameRoot);

#endif /* THANDOR_UI_INGAME_TECHNOLOGY_H */
