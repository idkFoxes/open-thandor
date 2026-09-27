/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SCENARIO_CATALOG_H
#define THANDOR_ASSETS_SCENARIO_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/scenario/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Scenario catalog (g_ScenarioCatalog): ScenarioCatalogHeader followed by the level, campaign and save
   records (0x100 bytes each). Every counted record advances the following section offsets and
   g_ScenarioCatalogUsedBytes by 0x104, although the records are packed 0x100 apart. */
#define SCENARIO_CATALOG_CAPACITY 0x30000
#define SCENARIO_CATALOG_HEADER_SIZE 0x18
#define SCENARIO_CATALOG_RECORD_SIZE 0x100
#define SCENARIO_CATALOG_RECORD_STRIDE 0x104
/* Text resource ids: a level title is 0x2230 + its title index, a campaign title 0x2220 + its index (the
   level text page aliases, see TextResourcePage_LoadCompatibilityAliases). */
#define TEXT_ID_LEVEL_TITLE_BASE 0x2230
#define TEXT_ID_CAMPAIGN_TITLE_BASE 0x2220
/* A level's description text is 0x230010 + 0x10 * its title index (record +0x70), registered by
   TextResourcePage_LoadCompatibilityAliases. 0x215D fills a scenario description box while no row is selected. */
#define TEXT_ID_LEVEL_DESCRIPTION_BASE 0x230010
#define TEXT_ID_SCENARIO_DESCRIPTION_EMPTY 0x215D

/* Index of the "Choose game" page (gameSelectPage) in the frontend page stack. */
#define FRONTEND_PAGE_STACK_CHOOSE_GAME 10

/* Tabs of the scenario-selection page (index into scenarioCatalogRebuildCallbacks and
   g_FrontendScenarioMapOptionHandlerTable). */
#define SCENARIO_SELECTION_TAB_SAVED_GAMES 0
#define SCENARIO_SELECTION_TAB_SINGLE_GAMES 1
#define SCENARIO_SELECTION_TAB_CAMPAIGNS 2

/* g_FrontendScenarioTransferState: which asset a network client expects next in the transfer mailbox
   (handled by FrontendScenarioTransfer_ProcessReceivedAsset). */
#define SCENARIO_TRANSFER_NONE 0
#define SCENARIO_TRANSFER_CATALOG 1            /* scenario catalog */
#define SCENARIO_TRANSFER_LEVEL 2              /* level asset */
#define SCENARIO_TRANSFER_FIELD_GRID 3         /* field grid of the loaded level */
#define SCENARIO_TRANSFER_CAMPAIGN_BUNDLE 4    /* level + campaign + field grid */
#define SCENARIO_TRANSFER_LEVEL_BUNDLE 5       /* level + field grid (every value >= 5) */

/* 0x00549E50 */
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyLocalizedTextSelection(UiPointerListControl *listControl);

/* 0x00549EB0 */
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyField70Selection(UiPointerListControl *listControl);

/* 0x00549F10 */
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyField50Selection(UiPointerListControl *listControl);

/* 0x0054A280 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView26C4 *scenarioSelectionPage);

/* 0x0054A610 */
void FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode);

/* 0x0054A690 */
void __thandor_preserve_eax FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode);

/* 0x0054A710 */
void __thandor_preserve_eax
FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode);

/* 0x00549A70 */
void FrontendScenarioAction_StartFieldGridLoad(void *source);

/* 0x00549FD0 */
void __thandor_void_preserve_eax_ecx_edx ScenarioCatalog_Rebuild(void);

/* 0x00545290 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
                                                 uint32_t unusedArg3);

/* 0x00547860 */
void __thandor_void_preserve_eax_ecx_edx FrontendScenarioTransfer_ProcessReceivedAsset(void);

/* 0x005443B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestFieldGrid(UiListRowIndex selectedLevelIndex);

/* 0x00544AC0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestCampaignBundle
          (uint32_t arg0,uint32_t arg1,uint32_t arg2,uint32_t selectedRecordIndex);

/* 0x00544DC0 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildSaveRecordListPage
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4);

/* 0x00544EA0 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildLevelRecordListPage
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4);

/* 0x00545020 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildCampaignRecordListPage
          (uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

/* 0x00549F70 */
ScenarioCatalogRecordCount __thandor_void_preserve_eax_ecx
ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords);

/* 0x00544870 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestLevelAsset
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRowIndex);

/* 0x00545140 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordLocalizedText
          (uint32_t arg0,uint32_t arg1,uint32_t arg2,UiListRowIndex selectionIndex);

/* 0x00545240 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField50DisplayId
          (uint32_t arg0,uint32_t arg1,uint32_t arg2,UiListRowIndex selectionIndex);

/* 0x00549CC0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelection_ActivateSelectedRecord
          (FrontendScenarioSelectionControlAddress32 selectionControl);

/* 0x005451F0 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField70DisplayId
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex);

#endif /* THANDOR_ASSETS_SCENARIO_CATALOG_H */
