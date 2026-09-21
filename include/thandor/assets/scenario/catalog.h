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
ScenarioCatalog_RequestRomTransitionStopCallback(dword arg0,dword arg1,dword arg2,dword arg3);

/* 0x00547860 */
void __thandor_void_preserve_eax_ecx_edx FrontendScenarioTransfer_ProcessReceivedAsset(void);

/* 0x005443B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestFieldGrid(UiListRowIndex selectedLevelIndex);

/* 0x00544AC0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestCampaignBundle
          (dword arg0,dword arg1,dword arg2,dword selectedRecordIndex);

/* 0x00544DC0 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildSaveRecordListPage
          (dword argument1,dword argument2,dword argument3,dword argument4);

/* 0x00544EA0 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildLevelRecordListPage
          (dword argument1,dword argument2,dword argument3,dword argument4);

/* 0x00545020 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildCampaignRecordListPage
          (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3);

/* 0x00549F70 */
ScenarioCatalogRecordCount __thandor_void_preserve_eax_ecx
ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords);

/* 0x00544870 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestLevelAsset
          (dword arg0,dword arg1,dword arg2,dword selectedRecordIndex);

/* 0x00545140 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordLocalizedText
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex);

/* 0x00545240 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField50DisplayId
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex);

/* 0x00549CC0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelection_ActivateSelectedRecord
          (FrontendScenarioSelectionControlAddress32 selectionControl);

/* 0x005451F0 */
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField70DisplayId
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex);

#endif /* THANDOR_ASSETS_SCENARIO_CATALOG_H */
