/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/scenario_selection.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_SCENARIO_SELECTION_H
#define THANDOR_UI_FRONTEND_SCENARIO_SELECTION_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* A level's description text is 0x230010 + 0x10 * its title index (record +0x70), registered by
   TextResourcePage_LoadCompatibilityAliases. 0x215D fills a scenario description box while no row is selected. */
inline constexpr int32_t TEXT_ID_LEVEL_DESCRIPTION_BASE = 0x230010;
inline constexpr int32_t TEXT_ID_LEVEL_DESCRIPTION_STRIDE = 0x10; /* description + TEXT_LEVEL_EXTRA_LINE_COUNT lines per level */
inline constexpr int32_t TEXT_ID_SCENARIO_DESCRIPTION_EMPTY = 0x215D;
/* A campaign's description text is 0x230000 + its title index (record +0x50). */
inline constexpr int32_t TEXT_ID_CAMPAIGN_DESCRIPTION_BASE = 0x230000;
/* Saved-game description: a template whose rich-text payload selectors 0 and 1 receive the texts of the save
   record's +0x90 and +0x70 ids (ScenarioCatalog_SelectSavedGameAndShowDescription). */
inline constexpr int32_t TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE = 0x215E;
/* Text id bases of the other single-game list columns (index at level record +0x50, +0x60 and +0x80). */
inline constexpr int32_t TEXT_ID_LEVEL_COLUMN50_BASE = 0x220A;
inline constexpr int32_t TEXT_ID_LEVEL_COLUMN60_BASE = 0x2200;
inline constexpr int32_t TEXT_ID_LEVEL_COLUMN80_BASE = 0x2205;

/* Index of the "Choose game" page (gameSelectPage) in the frontend page stack. */
inline constexpr int32_t FRONTEND_PAGE_STACK_CHOOSE_GAME = 10;

/* Tabs of the scenario-selection page (index into scenarioCatalogRebuildCallbacks and
   g_FrontendScenarioMapOptionHandlerTable). */
inline constexpr int32_t SCENARIO_SELECTION_TAB_SAVED_GAMES = 0;
inline constexpr int32_t SCENARIO_SELECTION_TAB_SINGLE_GAMES = 1;
inline constexpr int32_t SCENARIO_SELECTION_TAB_CAMPAIGNS = 2;

void FrontendScenarioSelection_SelectOrStartSavedGame(UiPointerListControl *listControl);

void FrontendScenarioSelection_SelectOrStartLevel(UiPointerListControl *listControl);

void FrontendScenarioSelection_SelectOrStartCampaign(UiPointerListControl *listControl);

void FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView *scenarioSelectionPage);

void FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode);

void FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode);

void FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode);

void ScenarioCatalog_RebuildSaveRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void ScenarioCatalog_RebuildLevelRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void ScenarioCatalog_RebuildCampaignRecordListPage
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void ScenarioCatalog_SelectSavedGameAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex);

/* levelId (unused by the original): the mission an open-thandor host selected on its campaign map, 0 none. */
void ScenarioCatalog_SelectCampaignAndShowDescription
          (uint32_t playerRuntimeId,uint32_t levelId,uint32_t unusedArg2,UiListRowIndex selectionIndex);

void FrontendScenarioSelection_ActivateSelectedRecord(UiFramedTextButtonControl *selectionControl);

void ScenarioCatalog_SelectLevelAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex);

/* Not in the original (open-thandor): the link "Map Editor auf GitHub" / "Map Editor on GitHub"
   (TEXT_ID_CUSTOM_MAPS_HINT_LINK) below the description text while the hint row of an empty custom map list
   (custom_maps.h) is the selected row of the visible "Single game" tab. Draw is called by UiFrame_Draw after the
   roots; HandleLeftPress by UiPointer_DispatchLeftPress for a left press while no pointer capture is held: on the
   link it opens CUSTOM_MAPS_EDITOR_URL (SdlPlatform_OpenUrl) and returns true, and the press goes to no node;
   elsewhere it returns false. */
void FrontendCustomMapsHintLink_Draw();
bool FrontendCustomMapsHintLink_HandleLeftPress(int pointerY,int pointerX);

#endif /* THANDOR_UI_FRONTEND_SCENARIO_SELECTION_H */
