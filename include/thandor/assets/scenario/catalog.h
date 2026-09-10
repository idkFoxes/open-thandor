#ifndef THANDOR_ASSETS_SCENARIO_CATALOG_H
#define THANDOR_ASSETS_SCENARIO_CATALOG_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/scenario/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00549E50 */
undefined4 FrontendScenarioSelection_ApplyLocalizedTextSelection(UiPointerListControl *listControl);

/* 0x00549EB0 */
undefined4 FrontendScenarioSelection_ApplyField70Selection(UiPointerListControl *listControl);

/* 0x00549F10 */
undefined4 FrontendScenarioSelection_ApplyField50Selection(UiPointerListControl *listControl);

/* 0x0054A280 */
void FrontendScenarioSelectionPage_InitializeAndApplyMapOption(byte *scenarioSelectionRoot);

/* 0x0054A610 */
undefined8 FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode);

/* 0x0054A690 */
undefined1 FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode);

/* 0x0054A710 */
undefined1 FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode);

/* 0x00549A70 */
undefined8 FrontendScenarioAction_StartFieldGridLoad(void *source);

/* 0x00549FD0 */
void ScenarioCatalog_Rebuild(void);

/* 0x00545290 */
void ScenarioCatalog_RequestRomTransitionStopCallback(dword arg0,dword arg1,dword arg2,dword arg3);

/* 0x00547860 */
void __fastcall FrontendScenarioTransfer_ProcessReceivedAsset(void);

/* 0x005443B0 */
void __fastcall FrontendScenarioSession_LoadOrRequestFieldGrid (undefined4 param_1,undefined4 param_2,UiListRowIndex selectedLevelIndex);

/* 0x00544AC0 */
void __fastcall FrontendScenarioSession_LoadOrRequestCampaignBundle(undefined4 param_1,undefined4 param_2);

/* 0x00544DC0 */
undefined8 ScenarioCatalog_RebuildSaveRecordListPage (dword argument1,dword argument2,dword argument3,dword argument4);

/* 0x00544EA0 */
void ScenarioCatalog_RebuildLevelRecordListPage (dword argument1,dword argument2,dword argument3,dword argument4);

/* 0x00545020 */
undefined8 ScenarioCatalog_RebuildCampaignRecordListPage (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3);

/* 0x00549F70 */
undefined8 ScenarioCatalog_MergeRecordsByName (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords, ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords);

/* 0x00544870 */
void __fastcall FrontendScenarioSession_LoadOrRequestLevelAsset(undefined4 param_1,undefined4 param_2);

/* 0x00545140 */
void ScenarioCatalog_RefreshSelectedRecordLocalizedText (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex);

/* 0x00545240 */
void ScenarioCatalog_RefreshSelectedRecordField50DisplayId (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex);

/* 0x00549CC0 */
undefined8 FrontendScenarioSelection_ActivateSelectedRecord (FrontendScenarioSelectionControlAddress32 selectionControl);

/* 0x005451F0 */
undefined8 ScenarioCatalog_RefreshSelectedRecordField70DisplayId (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex);

#endif /* THANDOR_ASSETS_SCENARIO_CATALOG_H */
