/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/scenario/catalog.h>

/* Implementation ownership: assets/scenario/catalog. */

/* Address: 0x00549E50.
   Ownership: assets/scenario/catalog.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[57]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[57] (0x2039). Return datatype is preserved for non-queue direct callers.
   Local calls: ScenarioCatalog_RefreshSelectedRecordLocalizedText,
   FrontendScenarioSelection_ActivateSelectedRecord.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyLocalizedTextSelection(UiPointerListControl *listControl)

{
  UiListRowIndexEaxCf5 UVar1;
  
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf(listControl);
  if (!UVar1.carry) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordLocalizedText(g_LocalPlayerRuntimeId,0,0,UVar1.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x11f0,0,0,UVar1.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)&listControl[-8].rowHeight);
  return;
}


/* Address: 0x00549EB0.
   Ownership: assets/scenario/catalog.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[58]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[58] (0x203A). Return datatype is preserved for non-queue direct callers.
   Local calls: ScenarioCatalog_RefreshSelectedRecordField70DisplayId,
   FrontendScenarioSelection_ActivateSelectedRecord.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyField70Selection(UiPointerListControl *listControl)

{
  UiListRowIndexEaxCf5 UVar1;
  
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf(listControl);
  if (!UVar1.carry) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordField70DisplayId
                (g_LocalPlayerRuntimeId,0,0,UVar1.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,UVar1.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)&listControl[-0xc].base.leftOffset);
  return;
}


/* Address: 0x00549F10.
   Ownership: assets/scenario/catalog.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[59]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[59] (0x203B). Return datatype is preserved for non-queue direct callers.
   Local calls: ScenarioCatalog_RefreshSelectedRecordField50DisplayId,
   FrontendScenarioSelection_ActivateSelectedRecord.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyField50Selection(UiPointerListControl *listControl)

{
  UiListRowIndexEaxCf5 UVar1;
  
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf(listControl);
  if (!UVar1.carry) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordField50DisplayId
                (g_LocalPlayerRuntimeId,0,0,UVar1.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12f0,0,0,UVar1.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)&listControl[-0x11].base.topAnchorQ31);
  return;
}


/* Address: 0x0054A280.
   Ownership: assets/scenario/catalog.
   Purpose: Initializes the scenario-selection page, chooses the save, level, or campaign catalog, applies the
   command-line map option when it matches an existing level, and updates mode-dependent control visibility.
   Local calls: ScenarioCatalog_RefreshSelectedRecordField70DisplayId,
   FrontendScenarioSession_LoadOrRequestLevelAsset.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], UiSelectableGroup_NoneVisibleSelectedCf
   [ui/controls/lists], UiSelectableGroup_SelectExclusive [ui/controls/lists], Text_CopyNarrowToUtf16Cf
   [core/text/string], WidePath_SetExtensionCode [core/text/path], Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView26C4 *scenarioSelectionPage)

{
  UiNodeFlags *pUVar1;
  short sVar2;
  CommandPayloadDword04 selectionIndex;
  UiControlCount UVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  short *psVar8;
  bool bVar9;
  UiSelectableNodeEaxEcxCf9 UVar10;
  CommandLineFindOptionEbxCf5 CVar11;
  dword zeroDispatchArg6;
  dword zeroDispatchArg5;
  dword zeroDispatchArg4;
  
  UiPageStack_SetActiveIndex(10,&scenarioSelectionPage->primaryPageStack);
  if ((int)g_FramebufferWidth < 0x281) {
    pUVar1 = &(scenarioSelectionPage->compactLayoutControl).nodeFlags;
    *pUVar1 = *pUVar1 | 0x2000;
  }
  UVar10 = UiSelectableGroup_NoneVisibleSelectedCf(3);
  UVar3 = UVar10.controlIndexOrCount;
  if (UVar10.carry) {
    UVar3 = 1;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&scenarioSelectionPage->scenarioOptionRow3);
    UVar3 = 1;
  }
  CVar11 = (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
  if (!CVar11.carry) {
    UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&scenarioSelectionPage->scenarioOptionRow3);
    UVar3 = 1;
  }
  (*g_FrontendUiActionHandlersPage20.scenarioCatalogRebuildCallbacks[UVar3])(0,0,0,0);
  (*g_FrontendScenarioMapOptionHandlerTable[UVar3])(0,0,0,0);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow1).control.base.nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
    *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
    return;
  }
  CVar11 = (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
  pbVar7 = CVar11.ebx;
  if (!CVar11.carry) {
    iVar4 = 0x3fffff;
    for (pbVar6 = pbVar7 + 7; *pbVar6 != 0x22; pbVar6 = pbVar6 + 1) {
      if ((*pbVar6 < 0x20) || (iVar4 = iVar4 + -1, iVar4 == 0))
      goto 
      FrontendScenarioSelectionPage_InitializeAndApplyMapOption_UpdateNetworkRoleActionAvailabilityAndReturn
      ;
    }
    *pbVar6 = 0;
    if (pbVar6[1] == 0) {
      Text_CopyNarrowToUtf16Cf(0x800000,(word *)g_PackageScratchBuffer,pbVar7 + 7);
      WidePath_SetExtensionCode(0,(word *)g_PackageScratchBuffer);
      *pbVar6 = 0x22;
      *pbVar7 = 0x6b;
      iVar4 = 0x400000;
      pbVar7 = g_PackageScratchBuffer;
      do {
        pbVar6 = pbVar7;
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        pbVar6 = pbVar7 + 2;
        sVar2 = *(short *)pbVar7;
        pbVar7 = pbVar6;
      } while (sVar2 != 0);
      iVar4 = *(int *)(g_FrontendRootNode + 0x21d8);
      if (iVar4 != 0) {
        selectionIndex = 0;
        bVar9 = true;
        do {
          uVar5 = (uint)((int)pbVar6 - (int)g_PackageScratchBuffer) >> 1;
          pbVar7 = g_PackageScratchBuffer;
          psVar8 = *(short **)(*(int *)(g_FrontendRootNode + 0x21d4) + selectionIndex * 4);
          do {
            if (uVar5 == 0) break;
            uVar5 = uVar5 - 1;
            bVar9 = *(short *)pbVar7 == *psVar8;
            pbVar7 = pbVar7 + 2;
            psVar8 = psVar8 + 1;
          } while (bVar9);
          if (bVar9) {
            pUVar1 = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
            *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
            pUVar1 = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
            *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
            pUVar1 = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
            *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
            pUVar1 = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
            *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              pUVar1 = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
              *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
            }
            Resource_Release(g_FrontendLoadedCampaignAsset);
            g_FrontendLoadedCampaignAsset = (void *)0x0;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              ScenarioCatalog_RefreshSelectedRecordField70DisplayId
                        (g_LocalPlayerRuntimeId,0,0,selectionIndex);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,selectionIndex);
            }
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendScenarioSession_LoadOrRequestLevelAsset
                        (g_LocalPlayerRuntimeId,0,0,selectionIndex);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x920,0,0,selectionIndex);
            }
            return;
          }
          selectionIndex = selectionIndex + 1;
          iVar4 = iVar4 + -1;
          bVar9 = iVar4 == 0;
        } while (!bVar9);
      }
    }
  }

  FrontendScenarioSelectionPage_InitializeAndApplyMapOption_UpdateNetworkRoleActionAvailabilityAndReturn
  :
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
    pUVar1 = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
    *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
    return;
  }
  pUVar1 = &(scenarioSelectionPage->scenarioOptionRow0).control.base.nodeFlags;
  *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
  pUVar1 = &(scenarioSelectionPage->scenarioOptionRow3).control.base.nodeFlags;
  *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
  pUVar1 = &(scenarioSelectionPage->scenarioOptionRow2).control.base.nodeFlags;
  *pUVar1 = *pUVar1 | UI_NODE_SUPPRESSED;
  pUVar1 = &(scenarioSelectionPage->scenarioOptionRow4).control.base.nodeFlags;
  *pUVar1 = *pUVar1 & ~UI_NODE_SUPPRESSED;
  return;
}


/* Address: 0x0054A610.
   Ownership: assets/scenario/catalog.
   Purpose: Walks to the scenario-page root, selects the save-record heading, and either rebuilds and refreshes
   locally or queues commands 0x0E70 and 0x11F0. Queued UI action handler for
   FRONTEND_PAGE20[53],FRONTEND_PAGE20[82] (0x2035,0x2052). Return datatype is preserved for non-queue direct
   callers.
   Local calls: ScenarioCatalog_RebuildSaveRecordListPage, ScenarioCatalog_RefreshSelectedRecordLocalizedText.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode)

{
  UiNodeBase *parentCursor;
  
  parentCursor = sourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    sourceNode = sourceNode->parent;
    parentCursor = sourceNode->parent;
  }
  sourceNode[0x6e].layoutHeight = 0x215d;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildSaveRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xe70,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RefreshSelectedRecordLocalizedText(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x11f0,0,0,0);
  }
  return;
}


/* Address: 0x0054A690.
   Ownership: assets/scenario/catalog.
   Purpose: Walks to the scenario-page root, selects the level-record heading, and either rebuilds and refreshes
   locally or queues commands 0x0F50 and 0x12A0. Queued UI action handler for
   FRONTEND_PAGE20[54],FRONTEND_PAGE20[83] (0x2036,0x2053). Return datatype is preserved for non-queue direct
   callers.
   Local calls: ScenarioCatalog_RebuildLevelRecordListPage, ScenarioCatalog_RefreshSelectedRecordField70DisplayId.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode)

{
  UiNodeBase *parentCursor;
  
  parentCursor = sourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    sourceNode = sourceNode->parent;
    parentCursor = sourceNode->parent;
  }
  sourceNode[0x75].left = 0x215d;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildLevelRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xf50,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RefreshSelectedRecordField70DisplayId(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,0);
  }
  return;
}


/* Address: 0x0054A710.
   Ownership: assets/scenario/catalog.
   Purpose: Walks to the scenario-page root, selects the campaign-record heading, and either rebuilds and refreshes
   locally or queues commands 0x10D0 and 0x12F0. Queued UI action handler for
   FRONTEND_PAGE20[55],FRONTEND_PAGE20[84] (0x2037,0x2054). Return datatype is preserved for non-queue direct
   callers.
   Local calls: ScenarioCatalog_RebuildCampaignRecordListPage,
   ScenarioCatalog_RefreshSelectedRecordField50DisplayId.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode)

{
  UiNodeBase *parentCursor;
  
  parentCursor = sourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    sourceNode = sourceNode->parent;
    parentCursor = sourceNode->parent;
  }
  sourceNode[0x7b].right = 0x215d;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x10d0,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RefreshSelectedRecordField50DisplayId(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12f0,0,0,0);
  }
  return;
}


/* Address: 0x00549A70.
   Ownership: assets/scenario/catalog.
   Purpose: Starts the field-grid load directly in local mode or queues command 0x460 in shared frontend modes.
   Queued UI action handler for FRONTEND_PAGE20[65] (0x2041). Return datatype is preserved for non-queue direct
   callers.
   Local calls: FrontendScenarioSession_LoadOrRequestFieldGrid.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendScenarioAction_StartFieldGridLoad(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendScenarioSession_LoadOrRequestFieldGrid(g_LocalPlayerRuntimeId);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x460,0,0,0);
  }
  return;
}


/* Address: 0x00549FD0.
   Ownership: assets/scenario/catalog.
   Purpose: Recreates the 0x30000-byte frontend catalog. It imports level\level.dat and level\campagne.dat, merges
   numbered levelNN.dat and campagneNN.dat overrides by their 0x40-byte identifiers, enumerates save\*.sve, loads
   each 0x100-byte save record, relocates localized string IDs at +0x70 and optional +0x90, and updates the six-
   field header. The extra four bytes reserved per counted record remain intentionally untyped until their consumer
   is recovered.
   Local calls: ScenarioCatalog_MergeRecordsByName.
   Cross-module calls: Resource_Load [assets/resource/runtime], Resource_Release [assets/resource/runtime],
   WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
void __thandor_void_preserve_eax_ecx_edx ScenarioCatalog_Rebuild(void)

{
  ScenarioCatalogByteOffset *pSVar1;
  ScenarioCatalogRecordCount *pSVar2;
  ScenarioCatalogHeader *pSVar3;
  uint uVar4;
  void *handle;
  uint uVar5;
  dword dVar6;
  word *leaf;
  dword *recordCopyDwordsLevel;
  dword *recordCopyDwordsCampaign;
  ScenarioCatalogRecord *pSVar7;
  ScenarioCatalogRecord *pSVar8;
  ScenarioCatalogSaveRecord *saveRecord;
  ArenaAllocEaxCf5 AVar9;
  FatalErrorEaxCf5 FVar10;
  FileSystemOpenEaxCf5 FVar11;
  ResourceLoadEaxEcxCf9 RVar12;
  FileSystemEnumerationEaxEcxCf9 FVar13;
  void *handle_00;
  
  (*g_MemoryApi.free)(g_ScenarioCatalog);
  AVar9 = (*g_MemoryApi.alloc)(0x30000);
  FVar10 = (*g_FatalErrorPrimaryDispatchCf)(AVar9.eax,AVar9.carry);
  pSVar3 = (ScenarioCatalogHeader *)FVar10.eax;
  g_ScenarioCatalogUsedBytes = 0x18;
  g_ScenarioCatalog = pSVar3;
  pSVar3->levelRecordsOffset = 0x18;
  pSVar3->campaignRecordsOffset = 0x18;
  pSVar3->saveRecordsOffset = 0x18;
  pSVar3->levelRecordCount = 0;
  pSVar3->campaignRecordCount = 0;
  pSVar3->saveRecordCount = 0;
  RVar12 = Resource_Load((word *)u_level_level_dat_0050da0e);
  pSVar3 = g_ScenarioCatalog;
  if (!RVar12.carry) {
    uVar4 = RVar12.ecx / 0x100;
    pSVar7 = (ScenarioCatalogRecord *)
             ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset);
    recordCopyDwordsLevel = (dword *)RVar12.eax;
    pSVar8 = pSVar7;
    for (uVar5 = RVar12.ecx >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(dword *)pSVar8->identifier = *recordCopyDwordsLevel;
      recordCopyDwordsLevel = recordCopyDwordsLevel + 1;
      pSVar8 = (ScenarioCatalogRecord *)(pSVar8->identifier + 2);
    }
    Resource_Release((dword *)RVar12.eax);
    g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
    do {
      RVar12 = Resource_Load(g_ScenarioLevelDataPathTemplateUtf16.prefixCodeUnits);
      if (!RVar12.carry) {
        uVar4 = ScenarioCatalog_MergeRecordsByName
                          (RVar12.ecx,(ScenarioCatalogRecord *)RVar12.eax,uVar4,pSVar7);
        Resource_Release((ScenarioCatalogRecord *)RVar12.eax);
      }
      g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] + 1;
    } while ((g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] < 0x3a) ||
            (g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits =
                  g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits - 0x9ffff,
            g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[0] < 0x3a));
    do {
      pSVar3->campaignRecordsOffset = pSVar3->campaignRecordsOffset + 0x104;
      pSVar3->saveRecordsOffset = pSVar3->saveRecordsOffset + 0x104;
      pSVar3->levelRecordCount = pSVar3->levelRecordCount + 1;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + 0x104;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
  }
  RVar12 = Resource_Load((word *)u_level_campagne_dat_0050da52);
  pSVar3 = g_ScenarioCatalog;
  if (!RVar12.carry) {
    uVar4 = RVar12.ecx / 0x100;
    pSVar7 = (ScenarioCatalogRecord *)
             ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->campaignRecordsOffset
             );
    recordCopyDwordsCampaign = (dword *)RVar12.eax;
    pSVar8 = pSVar7;
    for (uVar5 = RVar12.ecx >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(dword *)pSVar8->identifier = *recordCopyDwordsCampaign;
      recordCopyDwordsCampaign = recordCopyDwordsCampaign + 1;
      pSVar8 = (ScenarioCatalogRecord *)(pSVar8->identifier + 2);
    }
    Resource_Release((dword *)RVar12.eax);
    g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
    do {
      RVar12 = Resource_Load(g_ScenarioCampaignDataPathTemplateUtf16.prefixCodeUnits);
      if (!RVar12.carry) {
        uVar4 = ScenarioCatalog_MergeRecordsByName
                          (RVar12.ecx,(ScenarioCatalogRecord *)RVar12.eax,uVar4,pSVar7);
        Resource_Release((ScenarioCatalogRecord *)RVar12.eax);
      }
      g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] =
           g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] + 1;
    } while ((g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] < 0x3a) ||
            (g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits =
                  g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits - 0x9ffff,
            g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[0] < 0x3a));
    do {
      pSVar1 = &pSVar3->saveRecordsOffset;
      *pSVar1 = *pSVar1 + 0x104;
      pSVar2 = &pSVar3->campaignRecordCount;
      *pSVar2 = *pSVar2 + 1;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + 0x104;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save___sve_0050d9c8,
             (word *)&g_ExecutableDirectoryUtf16);
  FVar13 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                     (FILESYSTEM_ENUMERATE_FILES,0xffffffff,0x800000,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  pSVar3 = g_ScenarioCatalog;
  dVar6 = FVar13.entryCount;
  if ((!FVar13.carry) && (dVar6 != 0)) {
    saveRecord = (ScenarioCatalogSaveRecord *)
                 ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->saveRecordsOffset
                 );
    leaf = (word *)g_PackageScratchBuffer;
    do {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
                 (word *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
                 (word *)&g_ScenarioCatalogPathScratchUtf16);
      FVar11 = (*g_FileSystemOpenCf)
                         (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,(word *)&g_ScenarioCatalogPathScratchUtf16
                         );
      FVar10 = (*g_FatalErrorPrimaryDispatchCf)(FVar11.eax,FVar11.carry);
      handle = (void *)FVar10.eax;
      handle_00 = handle;
      (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0x100,handle);
      (*g_FileSystemReadExactCf)(0x100,saveRecord,handle);
      (*g_FileSystemClose)(handle_00);
      saveRecord->localizedStringId70 = saveRecord->localizedStringId70 + 0x2230;
      if (-1 < saveRecord->optionalLocalizedStringId90) {
        saveRecord->optionalLocalizedStringId90 = saveRecord->optionalLocalizedStringId90 + 0x2220;
      }
      saveRecord = saveRecord + 1;
      pSVar3->saveRecordCount = pSVar3->saveRecordCount + 1;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + 0x104;
      leaf = (word *)((int)leaf + FVar13.recordSizeBytes);
      dVar6 = dVar6 - 1;
    } while (dVar6 != 0);
  }
  return;
}


/* Address: 0x00545290.
   Ownership: assets/scenario/catalog.
   Purpose: Exact four-argument callback wrapper that ignores its arguments and invokes
   FrontendRomTransition_RequestStop.
   Cross-module calls: FrontendRomTransition_RequestStop [assets/rom/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RequestRomTransitionStopCallback(dword arg0,dword arg1,dword arg2,dword arg3)

{
  FrontendRomTransition_RequestStop();
  return;
}


/* Address: 0x00547860.
   Ownership: assets/scenario/catalog.
   Purpose: Consumes the current frontend transfer-mailbox payload according to the pending asset state, decodes
   scenario catalog, level, field-grid, or campaign data, updates player readiness flags, and queues the matching
   local command.
   Cross-module calls: UiTransferMailbox_GetReceivedBufferCf [network/protocol/transfer], PckCodec_DecodeHuffmanRle
   [assets/package/codec], UiTransferMailbox_ClearReceivedState [network/protocol/transfer],
   DwordBlock64Array_ContainsExactRecordCf [core/memory/allocator], FrontendCommandQueue_EnqueueLocalPlayerCommand
   [network/protocol/commands], Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx FrontendScenarioTransfer_ProcessReceivedAsset(void)

{
  dword dVar1;
  ScenarioCatalogRecordCount recordCount;
  ScenarioCatalogByteOffset SVar2;
  ScenarioCatalogByteOffset *pSVar3;
  dword *pdVar4;
  FrontendLoadedLevelRuntimeImage370 *pFVar5;
  byte *pbVar6;
  uint uVar7;
  int iVar8;
  ScenarioCatalogRecordCount SVar9;
  byte *pbVar10;
  bool bVar11;
  ArenaAllocEaxCf5 AVar12;
  FatalErrorEaxCf5 FVar13;
  UiTransferMailboxReceivedEaxEcxCf9 UVar14;
  ScenarioCatalogHeader *memory;
  CommandPayloadDword0C payloadDword0C;
  CommandPayloadDword08 payloadDword08;
  CommandPayloadDword04 payloadDword04;
  
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) &&
     (g_FrontendScenarioTransferState != 0)) {
    if (g_FrontendScenarioTransferState == 1) {
      UVar14 = UiTransferMailbox_GetReceivedBufferCf();
      pdVar4 = (dword *)UVar14.eax;
      if (!UVar14.carry) {
        dVar1 = *pdVar4;
        AVar12 = (*g_MemoryApi.alloc)(dVar1);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        payloadDword04 = 0;
        payloadDword08 = 0;
        payloadDword0C = 0;
        memory = g_ScenarioCatalog;
        g_ScenarioCatalog = (ScenarioCatalogHeader *)FVar13.eax;
        g_ScenarioCatalogUsedBytes = dVar1;
        PckCodec_DecodeHuffmanRle(dVar1,(byte *)FVar13.eax,UVar14.ecx - 4,(byte *)(pdVar4 + 1));
        (*g_MemoryApi.free)(pdVar4);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = 0;
        SVar9 = g_ScenarioCatalog->levelRecordCount;
        if (memory != (ScenarioCatalogHeader *)0x0) {
          recordCount = memory->levelRecordCount;
          pdVar4 = (dword *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                            g_ScenarioCatalog->levelRecordsOffset);
          SVar2 = memory->levelRecordsOffset;
          pSVar3 = &memory->levelRecordsOffset;
          if ((SVar9 != 0) && (recordCount != 0)) {
            uVar7 = 1;
            iVar8 = 3;
            do {
              bVar11 = DwordBlock64Array_ContainsExactRecordCf
                                 (recordCount,(dword *)((int)pSVar3 + SVar2),pdVar4);
              if (!bVar11) {
                *(uint *)(&stack0xffffffd8 + iVar8 * 4) =
                     *(uint *)(&stack0xffffffd8 + iVar8 * 4) | uVar7;
              }
              pdVar4 = pdVar4 + 0x40;
              uVar7 = uVar7 * 2;
              if (uVar7 == 0) {
                uVar7 = 1;
                iVar8 = iVar8 + -1;
                if (iVar8 == 0) break;
              }
              SVar9 = SVar9 - 1;
            } while (SVar9 != 0);
          }
        }
        (*g_MemoryApi.free)(memory);
        FrontendCommandQueue_EnqueueLocalPlayerCommand
                  (0xe00,payloadDword0C,payloadDword08,payloadDword04);
      }
    }
    else if (g_FrontendScenarioTransferState < 3) {
      UVar14 = UiTransferMailbox_GetReceivedBufferCf();
      pdVar4 = (dword *)UVar14.eax;
      if (!UVar14.carry) {
        dVar1 = *pdVar4;
        if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
           (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid
           )) {
          Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                   levelPathOffsetOrLoadedFieldGrid);
        }
        Resource_Release(g_FrontendLoadedLevelAsset);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
        AVar12 = (*g_MemoryApi.alloc)(dVar1);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)FVar13.eax;
        PckCodec_DecodeHuffmanRle
                  (dVar1,(byte *)g_FrontendLoadedLevelAsset,UVar14.ecx - 4,(byte *)(pdVar4 + 1));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkFlag02ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x8d0,0,0,0);
        }
        (*g_MemoryApi.free)(pdVar4);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = 0;
      }
    }
    else if (g_FrontendScenarioTransferState == 3) {
      UVar14 = UiTransferMailbox_GetReceivedBufferCf();
      pdVar4 = (dword *)UVar14.eax;
      if (!UVar14.carry) {
        dVar1 = *pdVar4;
        AVar12 = (*g_MemoryApi.alloc)(dVar1);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = FVar13.eax
        ;
        PckCodec_DecodeFieldGrid
                  (dVar1,(FieldGridAsset *)FVar13.eax,UVar14.ecx - 4,(byte *)(pdVar4 + 1));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkFlag08ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x360,0,0,0);
        }
        (*g_MemoryApi.free)(pdVar4);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = 0;
      }
    }
    else if (g_FrontendScenarioTransferState < 5) {
      UVar14 = UiTransferMailbox_GetReceivedBufferCf();
      pdVar4 = (dword *)UVar14.eax;
      if (!UVar14.carry) {
        if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
           (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid
           )) {
          Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                   levelPathOffsetOrLoadedFieldGrid);
        }
        Resource_Release(g_FrontendLoadedLevelAsset);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
        AVar12 = (*g_MemoryApi.alloc)(*pdVar4);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)FVar13.eax;
        PckCodec_DecodeHuffmanRle
                  (*pdVar4,(byte *)g_FrontendLoadedLevelAsset,pdVar4[3],(byte *)(pdVar4 + 6));
        pbVar10 = (byte *)((int)(pdVar4 + 6) + pdVar4[3]);
        AVar12 = (*g_MemoryApi.alloc)(pdVar4[1]);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        g_FrontendLoadedCampaignAsset = (byte *)FVar13.eax;
        PckCodec_DecodeHuffmanRle(pdVar4[1],g_FrontendLoadedCampaignAsset,pdVar4[4],pbVar10);
        pFVar5 = g_FrontendLoadedLevelAsset;
        dVar1 = pdVar4[4];
        pbVar6 = (g_FrontendLoadedLevelAsset->header).common.buildMetadata.
                 assetRelativeAddressAnchor28 +
                 ((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid -
                 0x28);
        WidePath_SetExtensionCode(0x646c66,(word *)pbVar6);
        WidePath_CombineDirectoryAndLeaf
                  ((word *)&g_LevelResourcePathScratchUtf16,(word *)pbVar6,
                   (word *)&g_ExecutableDirectoryUtf16);
        AVar12 = (*g_MemoryApi.alloc)(pdVar4[2]);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        (pFVar5->header).pathState.levelPathOffsetOrLoadedFieldGrid = FVar13.eax;
        PckCodec_DecodeFieldGrid(pdVar4[2],(FieldGridAsset *)FVar13.eax,pdVar4[5],pbVar10 + dVar1);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkFlag04ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x410,0,0,0);
        }
        (*g_MemoryApi.free)(pdVar4);
        UiTransferMailbox_ClearReceivedState();
        pbVar10 = g_FrontendLoadedCampaignAsset;
        g_FrontendScenarioTransferState = 0;
        pbVar6 = g_FrontendLoadedCampaignAsset + 0xb4;
        iVar8 = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
        *(int *)(g_FrontendLoadedCampaignAsset + 0xc4) = *(int *)pbVar6;
        do {
          if (*(int *)pbVar6 == *(int *)(pbVar10 + 0x300)) break;
          pbVar10 = pbVar10 + 0x180;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
        WidePath_CombineDirectoryAndLeaf
                  (&g_FrontendScenarioPathScratchUtf16,(word *)(pbVar10 + 0x30c),
                   (word *)u_level_0050daac);
        WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
        FrontendPlayerRuntime_InitializeFactionAssignments();
      }
    }
    else {
      UVar14 = UiTransferMailbox_GetReceivedBufferCf();
      pdVar4 = (dword *)UVar14.eax;
      if (!UVar14.carry) {
        if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
           (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid
           )) {
          Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                   levelPathOffsetOrLoadedFieldGrid);
        }
        Resource_Release(g_FrontendLoadedLevelAsset);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
        AVar12 = (*g_MemoryApi.alloc)(*pdVar4);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        pFVar5 = (FrontendLoadedLevelRuntimeImage370 *)FVar13.eax;
        g_FrontendLoadedLevelAsset = pFVar5;
        PckCodec_DecodeHuffmanRle(*pdVar4,(byte *)pFVar5,pdVar4[2],(byte *)(pdVar4 + 4));
        dVar1 = pdVar4[2];
        pbVar6 = (pFVar5->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((pFVar5->header).pathState.levelPathOffsetOrLoadedFieldGrid - 0x28);
        WidePath_SetExtensionCode(0x646c66,(word *)pbVar6);
        WidePath_CombineDirectoryAndLeaf
                  ((word *)&g_LevelResourcePathScratchUtf16,(word *)pbVar6,
                   (word *)&g_ExecutableDirectoryUtf16);
        AVar12 = (*g_MemoryApi.alloc)(pdVar4[1]);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
        (pFVar5->header).pathState.levelPathOffsetOrLoadedFieldGrid = FVar13.eax;
        PckCodec_DecodeFieldGrid
                  (pdVar4[1],(FieldGridAsset *)FVar13.eax,pdVar4[3],
                   (byte *)((int)(pdVar4 + 4) + dVar1));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkFlag04ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x410,0,0,0);
        }
        (*g_MemoryApi.free)(pdVar4);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = 0;
        FrontendPlayerRuntime_InitializeFactionAssignments();
      }
    }
  }
  return;
}


/* Address: 0x005443B0.
   Ownership: assets/scenario/catalog.
   Purpose: Loads the selected level field-grid locally or obtains it through the frontend transfer mailbox,
   optionally publishes an encoded host copy, updates player readiness fields, and returns to the main frontend
   page. Typed parameters: p2 selectedLevelIndex→UiListRowIndex_V300. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], WidePath_CombineDirectoryAndLeaf
   [core/text/path], Package_LoadEntry [assets/package/runtime], PckCodec_EncodeFieldGrid [assets/package/codec],
   UiTransferMailbox_SetOutgoingBuffer [network/protocol/transfer], UiTransferMailbox_MarkUnavailable
   [network/protocol/transfer].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestFieldGrid(UiListRowIndex selectedLevelIndex)

{
  FrontendRoleStateFlags *pFVar1;
  uint bytes;
  dword dVar2;
  PckDecodedByteCount sourceImageSizeBytes;
  FrontendPlayerRuntimeBlockCount FVar3;
  FrontendLoadedLevelRuntimeImage370 *pFVar4;
  FrontendLoadedLevelRuntimeImage370 *pFVar5;
  FrontendPlayerRuntimeRecord *pFVar6;
  FieldGridAsset *sourceGrid;
  FrontendPlayerRuntimeBlockCount FVar7;
  uint uVar8;
  int iVar9;
  byte *pbVar10;
  dword *encodedSourceDwords;
  FrontendPlayerRuntimeRecord *pFVar11;
  dword *outgoingDwordCursor;
  PackageLoadEntryEaxCf5 PVar12;
  FatalErrorEaxCf5 FVar13;
  PckCodecEaxCf5 PVar14;
  ArenaAllocEaxCf5 AVar15;
  
  pFVar4 = g_FrontendLoadedLevelAsset;
  FVar7 = g_FrontendPlayerRuntimeBlockCount;
  pFVar11 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (((pFVar11->factionAssignment).roleStateFlags & 8) == 0) {
      dVar2 = (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
      pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
      *pFVar1 = *pFVar1 | 8;
      pbVar10 = (pFVar4->header).common.buildMetadata.assetRelativeAddressAnchor28 + (dVar2 - 0x28);
      WidePath_SetExtensionCode(0x646c66,(word *)pbVar10);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_LevelResourcePathScratchUtf16,(word *)pbVar10,
                 (word *)&g_ExecutableDirectoryUtf16);
      pFVar5 = g_FrontendLoadedLevelAsset;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
        pFVar11 = g_FrontendPlayerRuntimeBlocks + 1;
        iVar9 = g_FrontendPlayerRuntimeBlockCount - 1;
        goto FrontendScenarioSession_CheckNextPlayerForFieldGridRequest;
      }
      PVar12 = Package_LoadEntry((word *)pbVar10);
      FVar13 = (*g_FatalErrorPrimaryDispatchCf)((dword)PVar12.bufferOrError,PVar12.carry);
      sourceGrid = (FieldGridAsset *)FVar13.eax;
      (pFVar4->header).pathState.levelPathOffsetOrLoadedFieldGrid = (dword)sourceGrid;
      encodedSourceDwords = (dword *)g_PackageScratchBuffer;
      pFVar6 = g_FrontendPlayerRuntimeBlocks;
      FVar3 = g_FrontendPlayerRuntimeBlockCount;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        sourceImageSizeBytes = (sourceGrid->common).allocationSizeBytes;
        pbVar10 = g_PackageScratchBuffer + 4;
        *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceImageSizeBytes;
        PVar14 = PckCodec_EncodeFieldGrid(0x7ffffc,pbVar10,sourceImageSizeBytes,sourceGrid);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(PVar14.eax,PVar14.carry);
        bytes = FVar13.eax + 4;
        AVar15 = (*g_MemoryApi.alloc)(bytes);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)(AVar15.eax,AVar15.carry);
        outgoingDwordCursor = (dword *)FVar13.eax;
        for (uVar8 = bytes >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
          *outgoingDwordCursor = *encodedSourceDwords;
          encodedSourceDwords = encodedSourceDwords + 1;
          outgoingDwordCursor = outgoingDwordCursor + 1;
        }
        UiTransferMailbox_SetOutgoingBuffer(bytes,(dword *)FVar13.eax);
        pFVar6 = g_FrontendPlayerRuntimeBlocks;
        FVar3 = g_FrontendPlayerRuntimeBlockCount;
      }
      break;
    }
    pFVar11 = pFVar11 + 1;
    FVar7 = FVar7 - 1;
    pFVar6 = g_FrontendPlayerRuntimeBlocks;
    FVar3 = g_FrontendPlayerRuntimeBlockCount;
  } while (FVar7 != 0);
  goto joined_r0x00544553;
  while( true ) {
    pFVar11 = pFVar11 + 1;
    iVar9 = iVar9 + -1;
    if (iVar9 == 0) break;
FrontendScenarioSession_CheckNextPlayerForFieldGridRequest:
    if (g_LocalPlayerRuntimeId == pFVar11->playerRuntimeId) {
      if (((pFVar11->factionAssignment).roleStateFlags & 0x10) != 0) {
        PVar12 = Package_LoadEntry((word *)pbVar10);
        FVar13 = (*g_FatalErrorPrimaryDispatchCf)((dword)PVar12.bufferOrError,PVar12.carry);
        (pFVar5->header).pathState.levelPathOffsetOrLoadedFieldGrid = FVar13.eax;
        pFVar6 = g_FrontendPlayerRuntimeBlocks;
        FVar3 = g_FrontendPlayerRuntimeBlockCount;
        goto joined_r0x00544553;
      }
      break;
    }
  }
  pFVar11->snapshotPayloadB0_13AF[0] = 0;
  pFVar11->snapshotPayloadB0_13AF[1] = 0;
  pFVar11->snapshotPayloadB0_13AF[2] = 0;
  pFVar11->snapshotPayloadB0_13AF[3] = 0;
  UiTransferMailbox_MarkUnavailable();
  g_FrontendScenarioTransferState = 3;
  pFVar6 = g_FrontendPlayerRuntimeBlocks;
  FVar3 = g_FrontendPlayerRuntimeBlockCount;
joined_r0x00544553:
  while (pFVar11 = pFVar6, FVar3 = FVar3 - 1, FVar3 != 0) {
    pFVar6 = pFVar11 + 1;
    if ((pFVar11[1].factionAssignment.roleStateFlags & 0x10) != 0) {
      pFVar1 = &pFVar11[1].factionAssignment.roleStateFlags;
      *pFVar1 = *pFVar1 | 8;
      pFVar11[1].runtimeState70 = 0x7fffffff;
    }
  }
  FrontendSession_ReturnToMainPage(selectedLevelIndex,0,0,1);
  return;
}


/* Address: 0x00544AC0.
   Ownership: assets/scenario/catalog.
   Purpose: Loads the selected campaign image, resolves its level and field-grid assets, optionally publishes the
   three encoded images as one transfer bundle, initializes faction assignments, and switches the frontend state to
   the campaign path.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime],
   PckCodec_EncodeHuffmanRle [assets/package/codec], PckCodec_EncodeFieldGrid [assets/package/codec].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestCampaignBundle
          (dword arg0,dword arg1,dword arg2,dword selectedRecordIndex)

{
  FrontendRoleStateFlags *pFVar1;
  FieldGridAsset *sourceGrid;
  dword dVar2;
  dword dVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  int iVar7;
  byte *pbVar8;
  byte *transferBundleBytes;
  FrontendLoadedLevelRuntimeImage370 *source;
  dword *transferCopyDestination;
  PackageLoadEntryEaxCf5 PVar9;
  FatalErrorEaxCf5 FVar10;
  PckCodecEaxCf5 PVar11;
  ArenaAllocEaxCf5 AVar12;
  PckDecodedByteCount campaignDecodedSizeBytes;
  
  iVar7 = g_FrontendRootNode;
  pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *pFVar1 = *pFVar1 | 4;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,
               *(word **)(*(int *)(iVar7 + 0x23b4) + selectedRecordIndex * 4),
               (word *)u_level_0050dab8);
    WidePath_SetExtensionCode(0x6e6763,&g_FrontendScenarioPathScratchUtf16);
    PVar9 = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    FVar10 = (*g_FatalErrorPrimaryDispatchCf)((dword)PVar9.bufferOrError,PVar9.carry);
    pbVar8 = (byte *)FVar10.eax;
    pbVar6 = pbVar8 + 0xb4;
    iVar4 = *(int *)(pbVar8 + 0xb8);
    g_FrontendLoadedCampaignAsset = pbVar8;
    *(int *)(pbVar8 + 0xc4) = *(int *)pbVar6;
    do {
      if (*(int *)pbVar6 == *(int *)(pbVar8 + 0x300))
      goto FrontendScenarioSession_UseSelectedCampaignLevelRecord;
      pbVar8 = pbVar8 + 0x180;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    (*g_FatalErrorPrimaryDispatchCf)(0,false);
FrontendScenarioSession_UseSelectedCampaignLevelRecord:
    if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
       (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
      Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                               levelPathOffsetOrLoadedFieldGrid);
    }
    Resource_Release(g_FrontendLoadedLevelAsset);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,(word *)(pbVar8 + 0x30c),(word *)u_level_0050daac
              );
    WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
    PVar9 = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    FVar10 = (*g_FatalErrorPrimaryDispatchCf)((dword)PVar9.bufferOrError,PVar9.carry);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)FVar10.eax;
    pbVar6 = (g_FrontendLoadedLevelAsset->header).common.buildMetadata.assetRelativeAddressAnchor28
             + ((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid -
               0x28);
    WidePath_SetExtensionCode(0x646c66,(word *)pbVar6);
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_LevelResourcePathScratchUtf16,(word *)pbVar6,
               (word *)&g_ExecutableDirectoryUtf16);
    PVar9 = Package_LoadEntry((word *)pbVar6);
    pbVar6 = g_FrontendLoadedCampaignAsset;
    source = g_FrontendLoadedLevelAsset;
    transferBundleBytes = g_PackageScratchBuffer;
    sourceGrid = PVar9.bufferOrError;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      *(AssetAllocationSizeBytes *)g_PackageScratchBuffer =
           (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
      campaignDecodedSizeBytes = *(PckDecodedByteCount *)(pbVar6 + 4);
      *(AssetAllocationSizeBytes *)(transferBundleBytes + 8) =
           (sourceGrid->common).allocationSizeBytes;
      *(PckDecodedByteCount *)(transferBundleBytes + 4) = campaignDecodedSizeBytes;
      pbVar8 = transferBundleBytes + 0x18;
      PVar11 = PckCodec_EncodeHuffmanRle
                         (0x7fffe8,pbVar8,(source->header).common.allocationSizeBytes,(byte *)source
                         );
      FVar10 = (*g_FatalErrorPrimaryDispatchCf)(PVar11.eax,PVar11.carry);
      dVar2 = FVar10.eax;
      *(dword *)(transferBundleBytes + 0xc) = dVar2;
      pbVar8 = pbVar8 + dVar2;
      PVar11 = PckCodec_EncodeHuffmanRle
                         (0x7fffe8 - dVar2,pbVar8,*(PckDecodedByteCount *)(pbVar6 + 4),pbVar6);
      FVar10 = (*g_FatalErrorPrimaryDispatchCf)(PVar11.eax,PVar11.carry);
      dVar3 = FVar10.eax;
      *(dword *)(transferBundleBytes + 0x10) = dVar3;
      PVar11 = PckCodec_EncodeFieldGrid
                         ((0x7fffe8 - dVar2) - dVar3,pbVar8 + dVar3,
                          (sourceGrid->common).allocationSizeBytes,sourceGrid);
      FVar10 = (*g_FatalErrorPrimaryDispatchCf)(PVar11.eax,PVar11.carry);
      *(dword *)(transferBundleBytes + 0x14) = FVar10.eax;
      pbVar6 = pbVar8 + dVar3 + (FVar10.eax - (int)transferBundleBytes);
      AVar12 = (*g_MemoryApi.alloc)((dword)pbVar6);
      FVar10 = (*g_FatalErrorPrimaryDispatchCf)(AVar12.eax,AVar12.carry);
      transferCopyDestination = (dword *)FVar10.eax;
      for (uVar5 = (uint)pbVar6 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
        *transferCopyDestination = *(dword *)transferBundleBytes;
        transferBundleBytes = transferBundleBytes + 4;
        transferCopyDestination = transferCopyDestination + 1;
      }
      UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)pbVar6,(dword *)FVar10.eax);
    }
    (source->header).pathState.levelPathOffsetOrLoadedFieldGrid = (dword)sourceGrid;
    FrontendPlayerRuntime_InitializeFactionAssignments();
  }
  else {
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = 4;
  }
  *(uint *)(iVar7 + 0x3b4) = *(uint *)(iVar7 + 0x3b4) & 0xffffdfff;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(iVar7 + 0x508));
  OldUnitRuntime_ResetPendingTables();
  FrontendState_DispatchCode(3);
  return;
}


/* Address: 0x00544DC0.
   Ownership: assets/scenario/catalog.
   Purpose: Four-argument frontend callback. It selects the save-record page, constructs a pointer list over exact
   0x100-byte save records from ScenarioCatalogHeader.saveRecordsOffset, rebuilds the list control, and enables
   action 0x2038 only when at least one save record exists. EAX is preserved. Special action-table slot
   FRONTEND_PAGE20[85] uses four stack arguments and RET 0x10; it is deliberately partitioned from the generic
   queued handler ABI.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiPointerList_SortByDwordPairFieldDescending [ui/controls/lists], UiPointerList_SelectIndexVariantB
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildSaveRecordListPage
          (dword argument1,dword argument2,dword argument3,dword argument4)

{
  sdword *control;
  UiListRowCount rowCount;
  UiNodeBase *firstNode;
  UiListRowCount UVar1;
  void *pvVar2;
  void **rowPointers;
  void **ppvVar3;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&g_FrontendRootNode[0x62].bottomAnchorQ31);
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&firstNode[0x67].bottomAnchorQ31);
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    rowCount = g_ScenarioCatalog->saveRecordCount;
    pvVar2 = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                     g_ScenarioCatalog->saveRecordsOffset);
    rowPointers = (void **)(rowCount * 0x100 + (int)pvVar2);
    ppvVar3 = rowPointers;
    for (UVar1 = rowCount; UVar1 != 0; UVar1 = UVar1 - 1) {
      *ppvVar3 = pvVar2;
      ppvVar3 = ppvVar3 + 1;
      pvVar2 = (void *)((int)pvVar2 + 0x100);
    }
    control = &firstNode[0x6a].layoutHeight;
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByDwordPairFieldDescending(0xf0,(UiPointerListControl *)control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)control);
      UiNodeList_UnsuppressActionId(0x2038,firstNode);
      return;
    }
  }
  UiPointerList_InitializeColumnLayout
            (0,(void **)0x0,(UiPointerListControl *)&firstNode[0x6a].layoutHeight);
  UiNodeList_SuppressActionId(0x2038,firstNode);
  return;
}


/* Address: 0x00544EA0.
   Ownership: assets/scenario/catalog.
   Purpose: Four-argument frontend callback. It selects the level-record page, constructs a pointer list over exact
   0x100-byte level records, resolves four localized display fields per record, rebuilds the list control, and
   toggles actions 0x2038 and 0x203A according to availability and mode. EAX is preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], UiPointerList_InitializeColumnLayout
   [ui/controls/lists], UiPointerList_SortByExpandedTextFieldAscending [ui/controls/text],
   UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildLevelRecordListPage
          (dword argument1,dword argument2,dword argument3,dword argument4)

{
  sdword *control;
  UiNodeBase *firstNode;
  UiListRowCount UVar1;
  ScenarioCatalogRuntimeExpandedRecord100 *scenarioRecord;
  ScenarioCatalogRuntimeExpandedRecord100 **rowPointerCursor;
  TextResourceResolveEaxCf5 TVar2;
  UiListRowCount rowCount;
  ScenarioCatalogRuntimeExpandedRecord100 **rowPointers;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&g_FrontendRootNode[100].firstChild);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&firstNode[0x67].bottomAnchorQ31);
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    UVar1 = g_ScenarioCatalog->levelRecordCount;
    scenarioRecord =
         (ScenarioCatalogRuntimeExpandedRecord100 *)
         ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset);
    rowPointerCursor = (ScenarioCatalogRuntimeExpandedRecord100 **)(scenarioRecord + UVar1);
    rowCount = UVar1;
    rowPointers = rowPointerCursor;
    for (; UVar1 != 0; UVar1 = UVar1 - 1) {
      *rowPointerCursor = scenarioRecord;
      TVar2 = TextResource_Resolve(scenarioRecord->titleTextResourceId + 0x220a);
      scenarioRecord->titleDisplayTag = 0x8019;
      scenarioRecord->titleResolvedText = TVar2.eax;
      TVar2 = TextResource_Resolve(scenarioRecord->subtitleTextResourceId + 0x2200);
      scenarioRecord->subtitleDisplayTag = 0x8019;
      scenarioRecord->subtitleResolvedText = TVar2.eax;
      TVar2 = TextResource_Resolve(scenarioRecord->scenarioTextResourceId + 0x2230);
      *TVar2.eax = 0x8001;
      scenarioRecord->scenarioDisplayTag = 0x8019;
      scenarioRecord->scenarioResolvedText = TVar2.eax;
      TVar2 = TextResource_Resolve(scenarioRecord->modeTextResourceId + 0x2205);
      scenarioRecord->modeDisplayTag = 0x8019;
      scenarioRecord->modeResolvedText = TVar2.eax;
      rowPointerCursor = rowPointerCursor + 1;
      scenarioRecord = scenarioRecord + 1;
    }
    control = &firstNode[0x70].layoutHeight;
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByExpandedTextFieldAscending(0x74,(UiPointerListControl *)control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
        UiNodeList_UnsuppressActionId(0x2038,firstNode);
        UiNodeList_UnsuppressActionId(0x203a,firstNode);
        return;
      }
      goto LAB_00544ffc;
    }
  }
  UiPointerList_InitializeColumnLayout
            (0,(void **)0x0,(UiPointerListControl *)&firstNode[0x70].layoutHeight);
LAB_00544ffc:
  UiNodeList_SuppressActionId(0x2038,firstNode);
  UiNodeList_SuppressActionId(0x203a,firstNode);
  return;
}


/* Address: 0x00545020.
   Ownership: assets/scenario/catalog.
   Purpose: Selects the campaign scenario page, builds its record-pointer list, resolves localized campaign labels,
   sorts by record field 0x50, and updates actions 0x2038 and 0x203B.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiPointerList_InitializeColumnLayout [ui/controls/lists], TextResource_Resolve
   [assets/text/resources], UiPointerList_SortByDwordFieldAscending [ui/controls/lists],
   UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildCampaignRecordListPage
          (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3)

{
  sdword *control;
  UiNodeBase *firstNode;
  UiListRowCount UVar1;
  void *pvVar2;
  void **ppvVar3;
  TextResourceResolveEaxCf5 TVar4;
  UiListRowCount rowCount;
  void **rowPointers;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&g_FrontendRootNode[0x65].right);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&firstNode[0x67].bottomAnchorQ31);
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    UVar1 = g_ScenarioCatalog->campaignRecordCount;
    pvVar2 = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                     g_ScenarioCatalog->campaignRecordsOffset);
    ppvVar3 = (void **)(UVar1 * 0x100 + (int)pvVar2);
    rowCount = UVar1;
    rowPointers = ppvVar3;
    for (; UVar1 != 0; UVar1 = UVar1 - 1) {
      *ppvVar3 = pvVar2;
      TVar4 = TextResource_Resolve(*(int *)((int)pvVar2 + 0x50) + 0x2220);
      *(undefined2 *)((int)pvVar2 + 0x54) = 0x8019;
      *(word **)((int)pvVar2 + 0x56) = TVar4.eax;
      ppvVar3 = ppvVar3 + 1;
      pvVar2 = (void *)((int)pvVar2 + 0x100);
    }
    control = &firstNode[0x77].left;
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByDwordFieldAscending(0x50,(UiPointerListControl *)control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
        UiNodeList_UnsuppressActionId(0x2038,firstNode);
        UiNodeList_UnsuppressActionId(0x203b,firstNode);
        return;
      }
      goto LAB_00545113;
    }
  }
  UiPointerList_InitializeColumnLayout(0,(void **)0x0,(UiPointerListControl *)&firstNode[0x77].left)
  ;
LAB_00545113:
  UiNodeList_SuppressActionId(0x2038,firstNode);
  UiNodeList_SuppressActionId(0x203b,firstNode);
  return;
}


/* Address: 0x00549F70.
   Ownership: assets/scenario/catalog.
   Purpose: Merges source records of exactly 0x100 bytes into a destination array. It compares the first 0x40
   identifier bytes, replaces matching records, appends new identifiers, and returns the updated destination count
   in EDX; EAX is preserved by the original routine.
*/
ScenarioCatalogRecordCount __thandor_void_preserve_eax_ecx
ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords)

{
  uint sourceRecordsRemaining;
  int dwordsRemaining;
  int iVar1;
  ScenarioCatalogRecordCount destinationRecordsRemaining;
  ScenarioCatalogRecord *sourceNameCursor;
  ScenarioCatalogRecord *destinationRecordCursor;
  ScenarioCatalogRecord *destinationNameCursor;
  bool recordNamesEqual;
  
  recordNamesEqual = true;
  sourceRecordsRemaining = sourceByteCount / 0x100;
  destinationRecordsRemaining = existingRecordCount;
  destinationRecordCursor = destinationRecords;
ScenarioCatalog_MergeRecordsByName_ScanDestinationForMatchingRecordName:
  do {
    dwordsRemaining = 0x10;
    sourceNameCursor = sourceRecords;
    destinationNameCursor = destinationRecordCursor;
    do {
      if (dwordsRemaining == 0) break;
      dwordsRemaining = dwordsRemaining + -1;
      recordNamesEqual =
           *(int *)sourceNameCursor->identifier == *(int *)destinationNameCursor->identifier;
      sourceNameCursor = (ScenarioCatalogRecord *)(sourceNameCursor->identifier + 2);
      destinationNameCursor = (ScenarioCatalogRecord *)(destinationNameCursor->identifier + 2);
    } while (recordNamesEqual);
    if (!recordNamesEqual) {
      destinationRecordCursor = destinationRecordCursor + 1;
      destinationRecordsRemaining = destinationRecordsRemaining - 1;
      recordNamesEqual = destinationRecordsRemaining == 0;
      if (!recordNamesEqual)
      goto ScenarioCatalog_MergeRecordsByName_ScanDestinationForMatchingRecordName;
      existingRecordCount = existingRecordCount + 1;
    }
    for (iVar1 = 0x40; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined4 *)destinationRecordCursor->identifier = *(undefined4 *)sourceRecords->identifier;
      sourceRecords = (ScenarioCatalogRecord *)(sourceRecords->identifier + 2);
      destinationRecordCursor = (ScenarioCatalogRecord *)(destinationRecordCursor->identifier + 2);
    }
    sourceRecordsRemaining = sourceRecordsRemaining - 1;
    recordNamesEqual = false;
    destinationRecordsRemaining = existingRecordCount;
    destinationRecordCursor = destinationRecords;
    if (sourceRecordsRemaining == 0) {
      return existingRecordCount;
    }
  } while( true );
}


/* Address: 0x00544870.
   Ownership: assets/scenario/catalog.
   Purpose: Builds the selected level path, releases the previous level asset, loads or requests the level image,
   optionally publishes an encoded host copy, and marks matching player blocks as level-ready.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode
   [core/text/path], UiPageStack_SetActiveIndex [ui/controls/layout], FrontendState_DispatchCode
   [ui/frontend/runtime], Resource_Release [assets/resource/runtime], Package_LoadEntry [assets/package/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestLevelAsset
          (dword arg0,dword arg1,dword arg2,dword selectedRecordIndex)

{
  FrontendRoleStateFlags *pFVar1;
  UiPageStackControl *stack;
  PckDecodedByteCount sourceSizeBytes;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  FrontendLoadedLevelRuntimeImage370 *source;
  uint uVar4;
  uint uVar5;
  int iVar6;
  byte *destination;
  dword *encodedSourceDwords;
  FrontendPlayerRuntimeRecord *pFVar7;
  dword *outgoingDwordCursor;
  PackageLoadEntryEaxCf5 PVar8;
  FatalErrorEaxCf5 FVar9;
  PckCodecEaxCf5 PVar10;
  ArenaAllocEaxCf5 AVar11;
  
  iVar6 = g_FrontendRootNode;
  stack = (UiPageStackControl *)(g_FrontendRootNode + 0x508);
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             *(word **)(*(int *)(g_FrontendRootNode + 0x21d4) + selectedRecordIndex * 4),
             (word *)u_level_0050daac);
  WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
  UiPageStack_SetActiveIndex(0,stack);
  *(uint *)(iVar6 + 0x3b4) = *(uint *)(iVar6 + 0x3b4) & 0xffffdfff;
  FrontendState_DispatchCode(1);
  if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
     (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                             levelPathOffsetOrLoadedFieldGrid);
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
  pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *pFVar1 = *pFVar1 | 2;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    PVar8 = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    FVar9 = (*g_FatalErrorPrimaryDispatchCf)((dword)PVar8.bufferOrError,PVar8.carry);
    encodedSourceDwords = (dword *)g_PackageScratchBuffer;
    source = (FrontendLoadedLevelRuntimeImage370 *)FVar9.eax;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      sourceSizeBytes = (source->header).common.allocationSizeBytes;
      destination = g_PackageScratchBuffer + 4;
      g_FrontendLoadedLevelAsset = source;
      *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceSizeBytes;
      PVar10 = PckCodec_EncodeHuffmanRle(0x7ffffc,destination,sourceSizeBytes,(byte *)source);
      FVar9 = (*g_FatalErrorPrimaryDispatchCf)(PVar10.eax,PVar10.carry);
      uVar5 = FVar9.eax + 4;
      AVar11 = (*g_MemoryApi.alloc)(uVar5);
      FVar9 = (*g_FatalErrorPrimaryDispatchCf)(AVar11.eax,AVar11.carry);
      outgoingDwordCursor = (dword *)FVar9.eax;
      for (uVar4 = uVar5 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        *outgoingDwordCursor = *encodedSourceDwords;
        encodedSourceDwords = encodedSourceDwords + 1;
        outgoingDwordCursor = outgoingDwordCursor + 1;
      }
      UiTransferMailbox_SetOutgoingBuffer(uVar5,(dword *)FVar9.eax);
      source = g_FrontendLoadedLevelAsset;
    }
  }
  else {
    uVar5 = (*(int *)(*(int *)(g_FrontendRootNode + 0x21d4) + selectedRecordIndex * 4) -
            (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
    uVar4 = uVar5 >> 0xd;
    if (uVar4 < 3) {
      iVar6 = g_FrontendPlayerRuntimeBlockCount - 1;
      pFVar7 = g_FrontendPlayerRuntimeBlocks;
      do {
        if (g_LocalPlayerRuntimeId == pFVar7[1].playerRuntimeId) {
          if ((*(uint *)(pFVar7[1].reserved78_7F + uVar4 * 4 + 0xc) &
              1 << ((byte)(uVar5 >> 8) & 0x1f)) != 0) {
            PVar8 = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
            source = PVar8.bufferOrError;
            if (!PVar8.carry) goto FrontendScenarioSession_CommitLoadedLevelAsset;
          }
          break;
        }
        iVar6 = iVar6 + -1;
        pFVar7 = pFVar7 + 1;
      } while (iVar6 != 0);
    }
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = 2;
    source = g_FrontendLoadedLevelAsset;
  }
FrontendScenarioSession_CommitLoadedLevelAsset:
  g_FrontendLoadedLevelAsset = source;
  uVar5 = (*(int *)(*(int *)(g_FrontendRootNode + 0x21d4) + selectedRecordIndex * 4) -
          (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
  uVar4 = uVar5 >> 0xd;
  pFVar7 = g_FrontendPlayerRuntimeBlocks;
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  if (uVar4 < 3) {
    while (pFVar3 = pFVar7, FVar2 = FVar2 - 1, FVar2 != 0) {
      pFVar7 = pFVar3 + 1;
      if ((*(uint *)(pFVar3[1].reserved78_7F + uVar4 * 4 + 0xc) & 1 << ((byte)(uVar5 >> 8) & 0x1f))
          != 0) {
        pFVar1 = &pFVar3[1].factionAssignment.roleStateFlags;
        *pFVar1 = *pFVar1 | 0x12;
        pFVar3[1].runtimeState70 = 0x7fffffff;
      }
    }
  }
  return;
}


/* Address: 0x00545140.
   Ownership: assets/scenario/catalog.
   Purpose: Selection-change callback for the first scenario list. It resolves the selected record, updates the
   output resource ID, and patches localized fields +0x70 and +0x90 when the latter is nonnegative. Typed
   parameters: p3 selectionIndex→UiListRowIndex_V300. Nearby but non-identical semantic domains were explicitly
   deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Cross-module calls: UiPointerList_SelectIndexVariantB [ui/controls/lists], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordLocalizedText
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  int iVar1;
  TextResourceId resourceId;
  int iVar2;
  TextResourceResolveEaxCf5 TVar3;
  TextResourceResolveEaxCf5 TVar4;
  
  iVar2 = g_FrontendRootNode;
  iVar1 = *(int *)(g_FrontendRootNode + 0x200c);
  control = (UiPointerListControl *)(g_FrontendRootNode + 0x1fbc);
  *(undefined4 *)(g_FrontendRootNode + 0x20ec) = 0x215d;
  if (iVar1 != 0) {
    UiPointerList_SelectIndexVariantB(selectionIndex,control);
    iVar1 = *(int *)(iVar1 + selectionIndex * 4);
    if (*(int *)(iVar1 + 0x90) < 0) {
      resourceId = *(TextResourceId *)(iVar1 + 0x70);
      TVar3 = TextResource_Resolve(resourceId);
      *TVar3.eax = 0x8000;
      *(TextResourceId *)(iVar2 + 0x20ec) = resourceId;
    }
    else {
      TVar3 = TextResource_Resolve(0x215e);
      TVar4 = TextResource_Resolve(*(TextResourceId *)(iVar1 + 0x70));
      *TVar4.eax = 0x8000;
      RichTextCommandStream_PatchPayloadBySelector(1,TVar4.eax,TVar3.eax);
      TVar4 = TextResource_Resolve(*(TextResourceId *)(iVar1 + 0x90));
      RichTextCommandStream_PatchPayloadBySelector(0,TVar4.eax,TVar3.eax);
      *(undefined4 *)(iVar2 + 0x20ec) = 0x215e;
    }
  }
  return;
}


/* Address: 0x00545240.
   Ownership: assets/scenario/catalog.
   Purpose: Selection-change callback for the third scenario list. It derives the output display ID as record field
   +0x50 plus 0x230000. Typed parameters: p3 selectionIndex→UiListRowIndex_V300. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField50DisplayId
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  int iVar1;
  int iVar2;
  
  iVar2 = g_FrontendRootNode;
  iVar1 = *(int *)(g_FrontendRootNode + 0x23b4);
  control = (UiPointerListControl *)(g_FrontendRootNode + 0x2364);
  *(undefined4 *)(g_FrontendRootNode + 0x249c) = 0x215d;
  if (iVar1 != 0) {
    UiPointerList_SelectIndexVariantB(selectionIndex,control);
    *(int *)(iVar2 + 0x249c) = *(int *)(*(int *)(iVar1 + selectionIndex * 4) + 0x50) + 0x230000;
  }
  return;
}


/* Address: 0x00549CC0.
   Ownership: assets/scenario/catalog.
   Purpose: Reads the active save, level, or campaign selection group, releases stale campaign state, dispatches
   the matching local loader or network command, and prepares the selected save path when the save group is active.
   Queued UI action handler for FRONTEND_PAGE20[56] (0x2038). Return datatype is preserved for non-queue direct
   callers. Typed parameters: p0 selectionControl→FrontendScenarioSelectionControlAddress32_V345. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: FrontendScenarioSession_LoadOrRequestLevelAsset,
   FrontendScenarioSession_LoadOrRequestCampaignBundle.
   Cross-module calls: UiSelectableGroup_NoneVisibleSelectedCf [ui/controls/lists], Resource_Release
   [assets/resource/runtime], UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands], WidePath_CombineDirectoryAndLeaf
   [core/text/path], WidePath_SetExtensionCode [core/text/path].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelection_ActivateSelectedRecord
          (FrontendScenarioSelectionControlAddress32 selectionControl)

{
  UiListRowIndex selectedRowIndex;
  UiListRowIndexEaxCf5 UVar1;
  UiSelectableNodeEaxEcxCf9 UVar2;
  int scenarioPathPointerTableAddress;
  
  UVar2 = UiSelectableGroup_NoneVisibleSelectedCf(3);
  if (UVar2.carry) {
    return;
  }
  if (UVar2.controlIndexOrCount != 0) {
    if (UVar2.controlIndexOrCount < 2) {
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = (void *)0x0;
      UVar1 = UiPointerList_GetSelectedIndexVariantBCf
                        ((UiPointerListControl *)(selectionControl + 0x490));
      selectedRowIndex = UVar1.rowIndex;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendScenarioSession_LoadOrRequestLevelAsset(g_LocalPlayerRuntimeId,0,0,selectedRowIndex)
        ;
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x920,0,0,selectedRowIndex);
      }
      return;
    }
    Resource_Release(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = (void *)0x0;
    UVar1 = UiPointerList_GetSelectedIndexVariantBCf
                      ((UiPointerListControl *)(selectionControl + 0x670));
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,0,0,UVar1.rowIndex)
      ;
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0xb70,0,0,UVar1.rowIndex);
    }
    return;
  }
  scenarioPathPointerTableAddress = *(int *)(selectionControl + 0x318);
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf
                    ((UiPointerListControl *)(selectionControl + 0x2c8));
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             *(word **)(scenarioPathPointerTableAddress + UVar1.rowIndex * 4),
             (word *)u_save_0050daa2);
  WidePath_SetExtensionCode(0x657673,&g_FrontendScenarioPathScratchUtf16);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,2);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,2);
  }
  return;
}


/* Address: 0x005451F0.
   Ownership: assets/scenario/catalog.
   Purpose: Selection-change callback for the second scenario list. It derives the output display ID as record
   field +0x70 times 0x10 plus 0x230010. Typed parameters: p3 selectionIndex→UiListRowIndex_V300. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField70DisplayId
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  int iVar1;
  int iVar2;
  
  iVar2 = g_FrontendRootNode;
  iVar1 = *(int *)(g_FrontendRootNode + 0x21d4);
  control = (UiPointerListControl *)(g_FrontendRootNode + 0x2184);
  *(undefined4 *)(g_FrontendRootNode + 0x22cc) = 0x215d;
  if (iVar1 != 0) {
    UiPointerList_SelectIndexVariantB(selectionIndex,control);
    *(int *)(iVar2 + 0x22cc) =
         *(int *)(*(int *)(iVar1 + selectionIndex * 4) + 0x70) * 0x10 + 0x230010;
  }
  return;
}

