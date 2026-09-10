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
undefined4 FrontendScenarioSelection_ApplyLocalizedTextSelection(UiPointerListControl *listControl)

{
  undefined4 in_EAX;
  UiListRowIndex selectionIndex;
  undefined1 in_CF;
  
  selectionIndex = UiPointerList_GetSelectedIndexVariantBCf(listControl);
  if (!(bool)in_CF) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordLocalizedText(g_LocalPlayerRuntimeId,0,0,selectionIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x11f0,0,0,selectionIndex);
    }
    return in_EAX;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)&listControl[-8].rowHeight);
  return in_EAX;
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
undefined4 FrontendScenarioSelection_ApplyField70Selection(UiPointerListControl *listControl)

{
  undefined4 in_EAX;
  UiListRowIndex selectionIndex;
  undefined1 in_CF;
  
  selectionIndex = UiPointerList_GetSelectedIndexVariantBCf(listControl);
  if (!(bool)in_CF) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordField70DisplayId
                (g_LocalPlayerRuntimeId,0,0,selectionIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,selectionIndex);
    }
    return in_EAX;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)&listControl[-0xc].base.leftOffset);
  return in_EAX;
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
undefined4 FrontendScenarioSelection_ApplyField50Selection(UiPointerListControl *listControl)

{
  undefined4 in_EAX;
  UiListRowIndex selectionIndex;
  undefined1 in_CF;
  
  selectionIndex = UiPointerList_GetSelectedIndexVariantBCf(listControl);
  if (!(bool)in_CF) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordField50DisplayId
                (g_LocalPlayerRuntimeId,0,0,selectionIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12f0,0,0,selectionIndex);
    }
    return in_EAX;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)&listControl[-0x11].base.topAnchorQ31);
  return in_EAX;
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
void FrontendScenarioSelectionPage_InitializeAndApplyMapOption(byte *scenarioSelectionRoot)

{
  short sVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  int extraout_ECX_00;
  uint uVar4;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 uVar5;
  byte *pbVar6;
  undefined1 *extraout_EDX;
  CommandPayloadDword04 selectionIndex;
  CommandPayloadDword04 extraout_EDX_00;
  CommandPayloadDword04 payloadDword04;
  byte *pbVar7;
  short *psVar8;
  bool bVar9;
  undefined8 uVar10;
  byte *pbVar11;
  
  UiPageStack_SetActiveIndex(10,(UiPageStackControl *)(scenarioSelectionRoot + 0x508));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)(scenarioSelectionRoot + 0x3b4) = *(uint *)(scenarioSelectionRoot + 0x3b4) | 0x2000;
  }
  UiSelectableGroup_NoneVisibleSelectedCf(3);
  bVar9 = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL;
  if (bVar9) {
    UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)(scenarioSelectionRoot + 0x1db4));
  }
  bVar9 = bVar9 && (undefined1 *)0xfffffff3 < &stack0xffffffcc;
  (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
  iVar3 = extraout_ECX;
  if (!bVar9) {
    UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)(scenarioSelectionRoot + 0x1db4));
    iVar3 = 1;
  }
  (*g_FrontendUiActionHandlersPage20.handlers00_54[iVar3 + 0x55])((void *)0x0);
  (**(code **)(&g_FrontendScenarioMapOptionHandlerTable + extraout_ECX_00 * 4))(0);
  bVar9 = false;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    *(uint *)(scenarioSelectionRoot + 0x1d3c) = *(uint *)(scenarioSelectionRoot + 0x1d3c) | 8;
    *(uint *)(scenarioSelectionRoot + 0x1cdc) = *(uint *)(scenarioSelectionRoot + 0x1cdc) | 8;
    *(uint *)(scenarioSelectionRoot + 0x1dfc) = *(uint *)(scenarioSelectionRoot + 0x1dfc) | 8;
    *(uint *)(scenarioSelectionRoot + 0x1d9c) = *(uint *)(scenarioSelectionRoot + 0x1d9c) | 8;
    *(uint *)(scenarioSelectionRoot + 0x1e5c) = *(uint *)(scenarioSelectionRoot + 0x1e5c) | 8;
    return;
  }
  pbVar11 = scenarioSelectionRoot;
  (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
  if (!bVar9) {
    iVar3 = 0x3fffff;
    for (pbVar6 = scenarioSelectionRoot + 7; *pbVar6 != 0x22; pbVar6 = pbVar6 + 1) {
      if ((*pbVar6 < 0x20) || (iVar3 = iVar3 + -1, iVar3 == 0))
      goto 
      FrontendScenarioSelectionPage_InitializeAndApplyMapOption_UpdateNetworkRoleActionAvailabilityAndReturn
      ;
    }
    *pbVar6 = 0;
    if (pbVar6[1] == 0) {
      Text_CopyNarrowToUtf16Cf(0x800000,(word *)g_PackageScratchBuffer,scenarioSelectionRoot + 7);
      WidePath_SetExtensionCode(0,(word *)g_PackageScratchBuffer);
      *extraout_EDX = 0x22;
      *scenarioSelectionRoot = 0x6b;
      iVar3 = 0x400000;
      pbVar6 = g_PackageScratchBuffer;
      do {
        pbVar7 = pbVar6;
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        pbVar7 = pbVar6 + 2;
        sVar1 = *(short *)pbVar6;
        pbVar6 = pbVar7;
      } while (sVar1 != 0);
      iVar3 = *(int *)(g_FrontendRootNode + 0x21d8);
      if (iVar3 != 0) {
        iVar2 = 0;
        bVar9 = true;
        do {
          uVar4 = (uint)((int)pbVar7 - (int)g_PackageScratchBuffer) >> 1;
          pbVar6 = g_PackageScratchBuffer;
          psVar8 = *(short **)(*(int *)(g_FrontendRootNode + 0x21d4) + iVar2 * 4);
          do {
            if (uVar4 == 0) break;
            uVar4 = uVar4 - 1;
            bVar9 = *(short *)pbVar6 == *psVar8;
            pbVar6 = pbVar6 + 2;
            psVar8 = psVar8 + 1;
          } while (bVar9);
          if (bVar9) {
            *(uint *)(pbVar11 + 0x1cdc) = *(uint *)(pbVar11 + 0x1cdc) & 0xfffffff7;
            *(uint *)(pbVar11 + 0x1dfc) = *(uint *)(pbVar11 + 0x1dfc) & 0xfffffff7;
            *(uint *)(pbVar11 + 0x1d9c) = *(uint *)(pbVar11 + 0x1d9c) & 0xfffffff7;
            *(uint *)(pbVar11 + 0x1e5c) = *(uint *)(pbVar11 + 0x1e5c) & 0xfffffff7;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              *(uint *)(pbVar11 + 0x1d9c) = *(uint *)(pbVar11 + 0x1d9c) | 8;
            }
            Resource_Release(g_FrontendLoadedCampaignAsset);
            g_FrontendLoadedCampaignAsset = (void *)0x0;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              uVar10 = ScenarioCatalog_RefreshSelectedRecordField70DisplayId
                                 (g_LocalPlayerRuntimeId,0,0,selectionIndex);
              payloadDword04 = (CommandPayloadDword04)((ulonglong)uVar10 >> 0x20);
              uVar5 = extraout_ECX_02;
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,selectionIndex);
              uVar5 = extraout_ECX_01;
              payloadDword04 = extraout_EDX_00;
            }
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendScenarioSession_LoadOrRequestLevelAsset(uVar5,payloadDword04);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x920,0,0,payloadDword04);
            }
            return;
          }
          iVar2 = iVar2 + 1;
          iVar3 = iVar3 + -1;
          bVar9 = iVar3 == 0;
        } while (!bVar9);
      }
    }
  }

  FrontendScenarioSelectionPage_InitializeAndApplyMapOption_UpdateNetworkRoleActionAvailabilityAndReturn
  :
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
    *(uint *)(pbVar11 + 0x1cdc) = *(uint *)(pbVar11 + 0x1cdc) & 0xfffffff7;
    *(uint *)(pbVar11 + 0x1dfc) = *(uint *)(pbVar11 + 0x1dfc) & 0xfffffff7;
    *(uint *)(pbVar11 + 0x1d9c) = *(uint *)(pbVar11 + 0x1d9c) & 0xfffffff7;
    *(uint *)(pbVar11 + 0x1e5c) = *(uint *)(pbVar11 + 0x1e5c) & 0xfffffff7;
    return;
  }
  *(uint *)(pbVar11 + 0x1cdc) = *(uint *)(pbVar11 + 0x1cdc) & 0xfffffff7;
  *(uint *)(pbVar11 + 0x1dfc) = *(uint *)(pbVar11 + 0x1dfc) & 0xfffffff7;
  *(uint *)(pbVar11 + 0x1d9c) = *(uint *)(pbVar11 + 0x1d9c) | 8;
  *(uint *)(pbVar11 + 0x1e5c) = *(uint *)(pbVar11 + 0x1e5c) & 0xfffffff7;
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
undefined8 FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode)

{
  undefined4 extraout_EAX;
  undefined4 extraout_EAX_00;
  undefined4 uVar1;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar2;
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
    uVar1 = extraout_EAX_00;
    uVar2 = extraout_EDX_00;
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x11f0,0,0,0);
    uVar1 = extraout_EAX;
    uVar2 = extraout_EDX;
  }
  return CONCAT44(uVar2,uVar1);
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
undefined1 FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode)

{
  undefined1 in_AL;
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
  return in_AL;
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
undefined1 FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode)

{
  undefined1 in_AL;
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
  return in_AL;
}

/* Address: 0x00549A70.
   Ownership: assets/scenario/catalog.
   Purpose: Starts the field-grid load directly in local mode or queues command 0x460 in shared frontend modes.
   Queued UI action handler for FRONTEND_PAGE20[65] (0x2041). Return datatype is preserved for non-queue direct
   callers.
   Local calls: FrontendScenarioSession_LoadOrRequestFieldGrid.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined8 FrontendScenarioAction_StartFieldGridLoad(void *source)

{
  undefined4 extraout_EAX;
  undefined4 extraout_EAX_00;
  undefined4 uVar1;
  undefined4 in_ECX;
  undefined4 in_EDX;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar2;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendScenarioSession_LoadOrRequestFieldGrid(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
    uVar1 = extraout_EAX_00;
    uVar2 = extraout_EDX_00;
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x460,0,0,0);
    uVar1 = extraout_EAX;
    uVar2 = extraout_EDX;
  }
  return CONCAT44(uVar2,uVar1);
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
void ScenarioCatalog_Rebuild(void)

{
  ScenarioCatalogByteOffset *pSVar1;
  ScenarioCatalogRecordCount *pSVar2;
  ScenarioCatalogHeader *pSVar3;
  ScenarioCatalogRecordCount SVar4;
  void *handle;
  undefined4 extraout_ECX;
  uint extraout_ECX_00;
  uint uVar5;
  uint extraout_ECX_01;
  ScenarioCatalogSourceByteCount sourceByteCount;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  ScenarioCatalogSourceByteCount extraout_ECX_04;
  ScenarioCatalogSourceByteCount sourceByteCount_00;
  ScenarioCatalogSourceByteCount extraout_ECX_05;
  ScenarioCatalogSourceByteCount SVar6;
  int extraout_ECX_06;
  int extraout_ECX_07;
  undefined4 uVar7;
  ScenarioCatalogRecordCount extraout_EDX;
  ScenarioCatalogRecordCount extraout_EDX_00;
  int extraout_EDX_01;
  word *leaf;
  undefined4 *puVar8;
  ScenarioCatalogRecord *pSVar9;
  ScenarioCatalogRecord *pSVar10;
  void *destination;
  undefined1 in_CF;
  undefined1 uVar11;
  undefined8 uVar12;
  void *handle_00;
  
  (*g_MemoryApi.free)(g_ScenarioCatalog);
  (*g_MemoryApi.alloc)(0x30000);
  uVar12 = (*g_FatalErrorPrimaryDispatchCf)();
  pSVar3 = (ScenarioCatalogHeader *)uVar12;
  g_ScenarioCatalogUsedBytes = 0x18;
  g_ScenarioCatalog = pSVar3;
  pSVar3->levelRecordsOffset = 0x18;
  pSVar3->campaignRecordsOffset = 0x18;
  pSVar3->saveRecordsOffset = 0x18;
  pSVar3->levelRecordCount = 0;
  pSVar3->campaignRecordCount = 0;
  pSVar3->saveRecordCount = 0;
  uVar12 = Resource_Load(extraout_ECX,(int)((ulonglong)uVar12 >> 0x20),
                         (word *)u_level_level_dat_0050da0e);
  pSVar3 = g_ScenarioCatalog;
  uVar7 = (undefined4)((ulonglong)uVar12 >> 0x20);
  SVar6 = extraout_ECX_00;
  if (!(bool)in_CF) {
    uVar11 = CARRY4(g_ScenarioCatalog->levelRecordsOffset,(uint)g_ScenarioCatalog);
    pSVar10 = (ScenarioCatalogRecord *)
              ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset);
    puVar8 = (undefined4 *)uVar12;
    pSVar9 = pSVar10;
    for (uVar5 = extraout_ECX_00 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined4 *)pSVar9->identifier = *puVar8;
      puVar8 = puVar8 + 1;
      pSVar9 = (ScenarioCatalogRecord *)(pSVar9->identifier + 2);
    }
    SVar4 = Resource_Release((undefined4 *)uVar12);
    g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
    SVar6 = extraout_ECX_01;
    do {
      do {
        uVar12 = Resource_Load(SVar6,SVar4,g_ScenarioLevelDataPathTemplateUtf16.prefixCodeUnits);
        SVar4 = (ScenarioCatalogRecordCount)((ulonglong)uVar12 >> 0x20);
        SVar6 = sourceByteCount;
        if (!(bool)uVar11) {
          uVar12 = ScenarioCatalog_MergeRecordsByName
                             (sourceByteCount,(ScenarioCatalogRecord *)uVar12,SVar4,pSVar10);
          Resource_Release((void *)uVar12);
          SVar6 = extraout_ECX_02;
          SVar4 = extraout_EDX;
        }
        g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] =
             g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] + 1;
        uVar11 = 1;
      } while (g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] < 0x3a);
      g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits =
           g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits - 0x9ffff;
      uVar11 = 1;
    } while (g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[0] < 0x3a);
    do {
      pSVar3->campaignRecordsOffset = pSVar3->campaignRecordsOffset + 0x104;
      pSVar3->saveRecordsOffset = pSVar3->saveRecordsOffset + 0x104;
      pSVar3->levelRecordCount = pSVar3->levelRecordCount + 1;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + 0x104;
      in_CF = (ScenarioCatalogRecord *)0xfffffeff < pSVar10;
      pSVar10 = pSVar10 + 1;
      SVar4 = SVar4 - 1;
      uVar7 = 0;
    } while (SVar4 != 0);
  }
  uVar12 = Resource_Load(SVar6,uVar7,(word *)u_level_campagne_dat_0050da52);
  pSVar3 = g_ScenarioCatalog;
  if (!(bool)in_CF) {
    uVar11 = CARRY4(g_ScenarioCatalog->campaignRecordsOffset,(uint)g_ScenarioCatalog);
    pSVar10 = (ScenarioCatalogRecord *)
              ((int)&g_ScenarioCatalog->levelRecordsOffset +
              g_ScenarioCatalog->campaignRecordsOffset);
    puVar8 = (undefined4 *)uVar12;
    pSVar9 = pSVar10;
    for (uVar5 = extraout_ECX_03 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined4 *)pSVar9->identifier = *puVar8;
      puVar8 = puVar8 + 1;
      pSVar9 = (ScenarioCatalogRecord *)(pSVar9->identifier + 2);
    }
    SVar4 = Resource_Release((undefined4 *)uVar12);
    g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
    SVar6 = extraout_ECX_04;
    do {
      do {
        uVar12 = Resource_Load(SVar6,SVar4,g_ScenarioCampaignDataPathTemplateUtf16.prefixCodeUnits);
        SVar4 = (ScenarioCatalogRecordCount)((ulonglong)uVar12 >> 0x20);
        SVar6 = sourceByteCount_00;
        if (!(bool)uVar11) {
          uVar12 = ScenarioCatalog_MergeRecordsByName
                             (sourceByteCount_00,(ScenarioCatalogRecord *)uVar12,SVar4,pSVar10);
          Resource_Release((void *)uVar12);
          SVar6 = extraout_ECX_05;
          SVar4 = extraout_EDX_00;
        }
        g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] =
             g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] + 1;
        uVar11 = 1;
      } while (g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] < 0x3a);
      g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits =
           g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits - 0x9ffff;
      uVar11 = 1;
    } while (g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[0] < 0x3a);
    do {
      pSVar1 = &pSVar3->saveRecordsOffset;
      *pSVar1 = *pSVar1 + 0x104;
      pSVar2 = &pSVar3->campaignRecordCount;
      *pSVar2 = *pSVar2 + 1;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + 0x104;
      in_CF = (ScenarioCatalogRecord *)0xfffffeff < pSVar10;
      pSVar10 = pSVar10 + 1;
      SVar4 = SVar4 - 1;
    } while (SVar4 != 0);
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save___sve_0050d9c8,
             (word *)&g_ExecutableDirectoryUtf16);
  (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
            (FILESYSTEM_ENUMERATE_FILES,0xffffffff,0x800000,g_PackageScratchBuffer,
             &g_ScenarioCatalogPathScratchUtf16);
  pSVar3 = g_ScenarioCatalog;
  if ((!(bool)in_CF) && (extraout_ECX_06 != 0)) {
    destination = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                          g_ScenarioCatalog->saveRecordsOffset);
    leaf = (word *)g_PackageScratchBuffer;
    do {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
                 (word *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
                 (word *)&g_ScenarioCatalogPathScratchUtf16);
      (*g_FileSystemOpenCf)
                (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,(word *)&g_ScenarioCatalogPathScratchUtf16);
      handle = (void *)(*g_FatalErrorPrimaryDispatchCf)();
      handle_00 = handle;
      (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0x100,handle);
      (*g_FileSystemReadExactCf)(0x100,destination,handle_00);
      (*g_FileSystemClose)(handle);
      *(int *)((int)destination + 0x70) = *(int *)((int)destination + 0x70) + 0x2230;
      if (-1 < *(int *)((int)destination + 0x90)) {
        *(int *)((int)destination + 0x90) = *(int *)((int)destination + 0x90) + 0x2220;
      }
      destination = (void *)((int)destination + 0x100);
      pSVar3->saveRecordCount = pSVar3->saveRecordCount + 1;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + 0x104;
      leaf = (word *)((int)leaf + extraout_EDX_01);
    } while (extraout_ECX_07 != 1);
  }
  return;
}

/* Address: 0x00545290.
   Ownership: assets/scenario/catalog.
   Purpose: Exact four-argument callback wrapper that ignores its arguments and invokes
   FrontendRomTransition_RequestStop.
   Cross-module calls: FrontendRomTransition_RequestStop [assets/rom/runtime].
*/
void ScenarioCatalog_RequestRomTransitionStopCallback(dword arg0,dword arg1,dword arg2,dword arg3)

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
void __fastcall FrontendScenarioTransfer_ProcessReceivedAsset(void)

{
  dword dVar1;
  ScenarioCatalogByteOffset SVar2;
  ScenarioCatalogByteOffset *pSVar3;
  dword *pdVar4;
  word *pwVar5;
  void *memory;
  uint uVar6;
  undefined4 extraout_ECX;
  int iVar7;
  undefined4 extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  dword bytes;
  ScenarioCatalogRecordCount recordCount;
  byte *pbVar8;
  byte *pbVar9;
  bool bVar10;
  undefined8 uVar11;
  ulonglong uVar12;
  ScenarioCatalogHeader *memory_00;
  CommandPayloadDword0C payloadDword0C;
  CommandPayloadDword08 payloadDword08;
  CommandPayloadDword04 payloadDword04;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    bVar10 = g_FrontendScenarioTransferState == 0;
    if (!bVar10) {
      if (g_FrontendScenarioTransferState == 1) {
        pdVar4 = UiTransferMailbox_GetReceivedBufferCf();
        if (!bVar10) {
          (*g_MemoryApi.alloc)(*pdVar4);
          uVar11 = (*g_FatalErrorPrimaryDispatchCf)();
          g_ScenarioCatalogUsedBytes = (dword)((ulonglong)uVar11 >> 0x20);
          payloadDword04 = 0;
          payloadDword08 = 0;
          payloadDword0C = 0;
          memory_00 = g_ScenarioCatalog;
          g_ScenarioCatalog = (ScenarioCatalogHeader *)uVar11;
          PckCodec_DecodeHuffmanRle
                    (g_ScenarioCatalogUsedBytes,(byte *)(ScenarioCatalogHeader *)uVar11,
                     extraout_ECX_03 - 4,(byte *)(pdVar4 + 1));
          (*g_MemoryApi.free)(pdVar4);
          UiTransferMailbox_ClearReceivedState();
          g_FrontendScenarioTransferState = 0;
          if (memory_00 != (ScenarioCatalogHeader *)0x0) {
            recordCount = memory_00->levelRecordCount;
            pdVar4 = (dword *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                              g_ScenarioCatalog->levelRecordsOffset);
            SVar2 = memory_00->levelRecordsOffset;
            pSVar3 = &memory_00->levelRecordsOffset;
            if ((g_ScenarioCatalog->levelRecordCount != 0) && (bVar10 = false, recordCount != 0)) {
              iVar7 = 3;
              do {
                uVar12 = DwordBlock64Array_ContainsExactRecordCf
                                   (recordCount,(dword *)((int)pSVar3 + SVar2),pdVar4);
                recordCount = (ScenarioCatalogRecordCount)(uVar12 >> 0x20);
                uVar6 = (uint)uVar12;
                if (!bVar10) {
                  *(uint *)(&stack0xffffffd8 + iVar7 * 4) =
                       *(uint *)(&stack0xffffffd8 + iVar7 * 4) | uVar6;
                }
                pdVar4 = pdVar4 + 0x40;
                bVar10 = CARRY4(uVar6,uVar6);
              } while ((((uVar12 & 0x7fffffff) != 0) || (iVar7 = iVar7 + -1, iVar7 != 0)) &&
                      (extraout_ECX_04 != 1));
            }
          }
          (*g_MemoryApi.free)(memory_00);
          FrontendCommandQueue_EnqueueLocalPlayerCommand
                    (0xe00,payloadDword0C,payloadDword08,payloadDword04);
        }
      }
      else {
        bVar10 = g_FrontendScenarioTransferState < 3;
        if (bVar10) {
          memory = UiTransferMailbox_GetReceivedBufferCf();
          if (!bVar10) {
            if ((g_FrontendLoadedLevelAsset != (byte *)0x0) &&
               (0xffff < *(uint *)(g_FrontendLoadedLevelAsset + 0xb0))) {
              Resource_Release(*(void **)(g_FrontendLoadedLevelAsset + 0xb0));
            }
            Resource_Release(g_FrontendLoadedLevelAsset);
            g_FrontendLoadedLevelAsset = (byte *)0x0;
            (*g_MemoryApi.alloc)(bytes);
            uVar11 = (*g_FatalErrorPrimaryDispatchCf)();
            g_FrontendLoadedLevelAsset = (byte *)uVar11;
            PckCodec_DecodeHuffmanRle
                      ((PckDecodedByteCount)((ulonglong)uVar11 >> 0x20),g_FrontendLoadedLevelAsset,
                       extraout_ECX_02 - 4,(byte *)((int)memory + 4));
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendPlayerRuntime_MarkFlag02ById(g_LocalPlayerRuntimeId,0,0,0);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x8d0,0,0,0);
            }
            (*g_MemoryApi.free)(memory);
            UiTransferMailbox_ClearReceivedState();
            g_FrontendScenarioTransferState = 0;
          }
        }
        else if (g_FrontendScenarioTransferState == 3) {
          pdVar4 = UiTransferMailbox_GetReceivedBufferCf();
          if (!bVar10) {
            (*g_MemoryApi.alloc)(*pdVar4);
            uVar11 = (*g_FatalErrorPrimaryDispatchCf)();
            *(FieldGridAsset **)(g_FrontendLoadedLevelAsset + 0xb0) = (FieldGridAsset *)uVar11;
            PckCodec_DecodeFieldGrid
                      ((PckOutputCapacityBytes)((ulonglong)uVar11 >> 0x20),(FieldGridAsset *)uVar11,
                       extraout_ECX_01 - 4,(byte *)(pdVar4 + 1));
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
        else {
          bVar10 = g_FrontendScenarioTransferState < 5;
          if (bVar10) {
            pdVar4 = UiTransferMailbox_GetReceivedBufferCf();
            if (!bVar10) {
              if ((g_FrontendLoadedLevelAsset != (byte *)0x0) &&
                 (0xffff < *(uint *)(g_FrontendLoadedLevelAsset + 0xb0))) {
                Resource_Release(*(void **)(g_FrontendLoadedLevelAsset + 0xb0));
              }
              Resource_Release(g_FrontendLoadedLevelAsset);
              g_FrontendLoadedLevelAsset = (byte *)0x0;
              (*g_MemoryApi.alloc)(*pdVar4);
              g_FrontendLoadedLevelAsset = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
              PckCodec_DecodeHuffmanRle
                        (*pdVar4,g_FrontendLoadedLevelAsset,pdVar4[3],(byte *)(pdVar4 + 6));
              pbVar9 = (byte *)((int)(pdVar4 + 6) + pdVar4[3]);
              (*g_MemoryApi.alloc)(pdVar4[1]);
              g_FrontendLoadedCampaignAsset = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
              PckCodec_DecodeHuffmanRle(pdVar4[1],g_FrontendLoadedCampaignAsset,pdVar4[4],pbVar9);
              dVar1 = pdVar4[4];
              pwVar5 = (word *)WidePath_SetExtensionCode
                                         (0x646c66,(word *)(g_FrontendLoadedLevelAsset +
                                                           *(int *)(g_FrontendLoadedLevelAsset +
                                                                   0xb0)));
              WidePath_CombineDirectoryAndLeaf
                        ((word *)&g_LevelResourcePathScratchUtf16,pwVar5,
                         (word *)&g_ExecutableDirectoryUtf16);
              (*g_MemoryApi.alloc)(pdVar4[2]);
              uVar11 = (*g_FatalErrorPrimaryDispatchCf)();
              *(FieldGridAsset **)((int)((ulonglong)uVar11 >> 0x20) + 0xb0) =
                   (FieldGridAsset *)uVar11;
              PckCodec_DecodeFieldGrid(pdVar4[2],(FieldGridAsset *)uVar11,pdVar4[5],pbVar9 + dVar1);
              if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                  SESSION_NETWORK_ROLE_LOCAL) {
                FrontendPlayerRuntime_MarkFlag04ById(g_LocalPlayerRuntimeId,0,0,0);
              }
              else {
                FrontendCommandQueue_EnqueueLocalPlayerCommand(0x410,0,0,0);
              }
              (*g_MemoryApi.free)(pdVar4);
              UiTransferMailbox_ClearReceivedState();
              pbVar8 = g_FrontendLoadedCampaignAsset;
              g_FrontendScenarioTransferState = 0;
              pbVar9 = g_FrontendLoadedCampaignAsset + 0xb4;
              iVar7 = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
              *(int *)(g_FrontendLoadedCampaignAsset + 0xc4) = *(int *)pbVar9;
              do {
                if (*(int *)pbVar9 == *(int *)(pbVar8 + 0x300)) break;
                pbVar8 = pbVar8 + 0x180;
                iVar7 = iVar7 + -1;
              } while (iVar7 != 0);
              WidePath_CombineDirectoryAndLeaf
                        (&g_FrontendScenarioPathScratchUtf16,(word *)(pbVar8 + 0x30c),
                         (word *)u_level_0050daac);
              WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
              FrontendPlayerRuntime_InitializeFactionAssignments(extraout_ECX_00,extraout_EDX_01);
            }
          }
          else {
            pdVar4 = UiTransferMailbox_GetReceivedBufferCf();
            if (!bVar10) {
              if ((g_FrontendLoadedLevelAsset != (byte *)0x0) &&
                 (0xffff < *(uint *)(g_FrontendLoadedLevelAsset + 0xb0))) {
                Resource_Release(*(void **)(g_FrontendLoadedLevelAsset + 0xb0));
              }
              Resource_Release(g_FrontendLoadedLevelAsset);
              g_FrontendLoadedLevelAsset = (byte *)0x0;
              (*g_MemoryApi.alloc)(*pdVar4);
              g_FrontendLoadedLevelAsset = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
              PckCodec_DecodeHuffmanRle
                        (*pdVar4,g_FrontendLoadedLevelAsset,pdVar4[2],(byte *)(pdVar4 + 4));
              dVar1 = pdVar4[2];
              pwVar5 = (word *)WidePath_SetExtensionCode
                                         (0x646c66,(word *)(*(int *)(extraout_EDX + 0xb0) +
                                                           extraout_EDX));
              WidePath_CombineDirectoryAndLeaf
                        ((word *)&g_LevelResourcePathScratchUtf16,pwVar5,
                         (word *)&g_ExecutableDirectoryUtf16);
              (*g_MemoryApi.alloc)(pdVar4[1]);
              uVar11 = (*g_FatalErrorPrimaryDispatchCf)();
              *(FieldGridAsset **)((int)((ulonglong)uVar11 >> 0x20) + 0xb0) =
                   (FieldGridAsset *)uVar11;
              PckCodec_DecodeFieldGrid
                        (pdVar4[1],(FieldGridAsset *)uVar11,pdVar4[3],
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
              FrontendPlayerRuntime_InitializeFactionAssignments(extraout_ECX,extraout_EDX_00);
            }
          }
        }
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
void __fastcall
FrontendScenarioSession_LoadOrRequestFieldGrid
          (undefined4 param_1,undefined4 param_2,UiListRowIndex selectedLevelIndex)

{
  FrontendRoleStateFlags *pFVar1;
  int iVar2;
  PckDecodedByteCount sourceImageSizeBytes;
  FrontendPlayerRuntimeBlockCount FVar3;
  FrontendPlayerRuntimeRecord *pFVar4;
  FieldGridAsset *sourceGrid;
  int iVar5;
  undefined4 uVar6;
  FrontendPlayerRuntimeBlockCount FVar7;
  uint uVar8;
  uint byteCount;
  byte *destination;
  byte *pbVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  word *path;
  undefined4 *puVar11;
  undefined8 uVar12;
  
  iVar5 = g_FrontendLoadedLevelAsset;
  FVar7 = g_FrontendPlayerRuntimeBlockCount;
  pFVar10 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (((pFVar10->factionAssignment).roleStateFlags & 8) == 0) {
      iVar2 = *(int *)(g_FrontendLoadedLevelAsset + 0xb0);
      pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
      *pFVar1 = *pFVar1 | 8;
      path = (word *)(iVar2 + iVar5);
      WidePath_SetExtensionCode(0x646c66,path);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_LevelResourcePathScratchUtf16,path,(word *)&g_ExecutableDirectoryUtf16);
      iVar2 = g_FrontendLoadedLevelAsset;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
        pFVar10 = g_FrontendPlayerRuntimeBlocks + 1;
        iVar5 = g_FrontendPlayerRuntimeBlockCount - 1;
        goto FrontendScenarioSession_CheckNextPlayerForFieldGridRequest;
      }
      Package_LoadEntry(path);
      sourceGrid = (FieldGridAsset *)(*g_FatalErrorPrimaryDispatchCf)();
      *(FieldGridAsset **)(iVar5 + 0xb0) = sourceGrid;
      pbVar9 = g_PackageScratchBuffer;
      pFVar4 = g_FrontendPlayerRuntimeBlocks;
      FVar3 = g_FrontendPlayerRuntimeBlockCount;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        sourceImageSizeBytes = (sourceGrid->common).allocationSizeBytes;
        destination = g_PackageScratchBuffer + 4;
        *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceImageSizeBytes;
        PckCodec_EncodeFieldGrid(0x7ffffc,destination,sourceImageSizeBytes,sourceGrid);
        iVar5 = (*g_FatalErrorPrimaryDispatchCf)();
        (*g_MemoryApi.alloc)(iVar5 + 4);
        uVar12 = (*g_FatalErrorPrimaryDispatchCf)();
        byteCount = (uint)((ulonglong)uVar12 >> 0x20);
        puVar11 = (undefined4 *)uVar12;
        for (uVar8 = byteCount >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
          *puVar11 = *(undefined4 *)pbVar9;
          pbVar9 = pbVar9 + 4;
          puVar11 = puVar11 + 1;
        }
        UiTransferMailbox_SetOutgoingBuffer(byteCount,(undefined4 *)uVar12);
        pFVar4 = g_FrontendPlayerRuntimeBlocks;
        FVar3 = g_FrontendPlayerRuntimeBlockCount;
      }
      break;
    }
    pFVar10 = pFVar10 + 1;
    FVar7 = FVar7 - 1;
    pFVar4 = g_FrontendPlayerRuntimeBlocks;
    FVar3 = g_FrontendPlayerRuntimeBlockCount;
  } while (FVar7 != 0);
  goto joined_r0x00544553;
  while( true ) {
    pFVar10 = pFVar10 + 1;
    iVar5 = iVar5 + -1;
    if (iVar5 == 0) break;
FrontendScenarioSession_CheckNextPlayerForFieldGridRequest:
    if (g_LocalPlayerRuntimeId == pFVar10->playerRuntimeId) {
      if (((pFVar10->factionAssignment).roleStateFlags & 0x10) != 0) {
        Package_LoadEntry(path);
        uVar6 = (*g_FatalErrorPrimaryDispatchCf)();
        *(undefined4 *)(iVar2 + 0xb0) = uVar6;
        pFVar4 = g_FrontendPlayerRuntimeBlocks;
        FVar3 = g_FrontendPlayerRuntimeBlockCount;
        goto joined_r0x00544553;
      }
      break;
    }
  }
  pFVar10->snapshotPayloadB0_13AF[0] = 0;
  pFVar10->snapshotPayloadB0_13AF[1] = 0;
  pFVar10->snapshotPayloadB0_13AF[2] = 0;
  pFVar10->snapshotPayloadB0_13AF[3] = 0;
  UiTransferMailbox_MarkUnavailable();
  g_FrontendScenarioTransferState = 3;
  pFVar4 = g_FrontendPlayerRuntimeBlocks;
  FVar3 = g_FrontendPlayerRuntimeBlockCount;
joined_r0x00544553:
  while (pFVar10 = pFVar4, FVar3 = FVar3 - 1, FVar3 != 0) {
    pFVar4 = pFVar10 + 1;
    if ((pFVar10[1].factionAssignment.roleStateFlags & 0x10) != 0) {
      pFVar1 = &pFVar10[1].factionAssignment.roleStateFlags;
      *pFVar1 = *pFVar1 | 8;
      pFVar10[1].runtimeState70 = 0x7fffffff;
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
void __fastcall
FrontendScenarioSession_LoadOrRequestCampaignBundle(undefined4 param_1,undefined4 param_2)

{
  FrontendRoleStateFlags *pFVar1;
  byte *source;
  byte *pbVar2;
  word *pwVar3;
  FieldGridAsset *sourceGrid;
  int iVar4;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint extraout_ECX_02;
  uint uVar5;
  undefined4 extraout_ECX_03;
  undefined4 uVar6;
  int iVar7;
  byte *pbVar8;
  undefined4 *puVar9;
  undefined8 uVar10;
  int in_stack_00000010;
  
  iVar7 = g_FrontendRootNode;
  pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *pFVar1 = *pFVar1 | 4;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,
               *(word **)(*(int *)(iVar7 + 0x23b4) + in_stack_00000010 * 4),(word *)u_level_0050dab8
              );
    WidePath_SetExtensionCode(0x6e6763,&g_FrontendScenarioPathScratchUtf16);
    Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    pbVar2 = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
    pbVar8 = pbVar2 + 0xb4;
    iVar4 = *(int *)(pbVar2 + 0xb8);
    g_FrontendLoadedCampaignAsset = pbVar2;
    *(int *)(pbVar2 + 0xc4) = *(int *)pbVar8;
    do {
      if (*(int *)pbVar8 == *(int *)(pbVar2 + 0x300))
      goto FrontendScenarioSession_UseSelectedCampaignLevelRecord;
      pbVar2 = pbVar2 + 0x180;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    (*g_FatalErrorPrimaryDispatchCf)();
FrontendScenarioSession_UseSelectedCampaignLevelRecord:
    if ((g_FrontendLoadedLevelAsset != (byte *)0x0) &&
       (0xffff < *(uint *)(g_FrontendLoadedLevelAsset + 0xb0))) {
      Resource_Release(*(void **)(g_FrontendLoadedLevelAsset + 0xb0));
    }
    Resource_Release(g_FrontendLoadedLevelAsset);
    g_FrontendLoadedLevelAsset = (byte *)0x0;
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,(word *)(pbVar2 + 0x30c),(word *)u_level_0050daac
              );
    WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
    Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    g_FrontendLoadedLevelAsset = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
    pwVar3 = (word *)WidePath_SetExtensionCode
                               (0x646c66,(word *)(g_FrontendLoadedLevelAsset +
                                                 *(int *)(g_FrontendLoadedLevelAsset + 0xb0)));
    pwVar3 = (word *)WidePath_CombineDirectoryAndLeaf
                               ((word *)&g_LevelResourcePathScratchUtf16,pwVar3,
                                (word *)&g_ExecutableDirectoryUtf16);
    sourceGrid = Package_LoadEntry(pwVar3);
    source = g_FrontendLoadedCampaignAsset;
    pbVar2 = g_FrontendLoadedLevelAsset;
    pbVar8 = g_PackageScratchBuffer;
    uVar6 = extraout_ECX;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      *(undefined4 *)g_PackageScratchBuffer = *(undefined4 *)(g_FrontendLoadedLevelAsset + 4);
      uVar6 = *(undefined4 *)(source + 4);
      *(AssetAllocationSizeBytes *)(pbVar8 + 8) = (sourceGrid->common).allocationSizeBytes;
      *(undefined4 *)(pbVar8 + 4) = uVar6;
      PckCodec_EncodeHuffmanRle(0x7fffe8,pbVar8 + 0x18,*(PckDecodedByteCount *)(pbVar2 + 4),pbVar2);
      uVar10 = (*g_FatalErrorPrimaryDispatchCf)();
      iVar4 = (int)uVar10;
      *(int *)(pbVar8 + 0xc) = iVar4;
      PckCodec_EncodeHuffmanRle
                (extraout_ECX_00 - iVar4,(byte *)((int)((ulonglong)uVar10 >> 0x20) + iVar4),
                 *(PckDecodedByteCount *)(source + 4),source);
      uVar10 = (*g_FatalErrorPrimaryDispatchCf)();
      iVar4 = (int)uVar10;
      *(int *)(pbVar8 + 0x10) = iVar4;
      PckCodec_EncodeFieldGrid
                (extraout_ECX_01 - iVar4,(byte *)((int)((ulonglong)uVar10 >> 0x20) + iVar4),
                 (sourceGrid->common).allocationSizeBytes,sourceGrid);
      uVar10 = (*g_FatalErrorPrimaryDispatchCf)();
      *(int *)(pbVar8 + 0x14) = (int)uVar10;
      (*g_MemoryApi.alloc)(((int)((ulonglong)uVar10 >> 0x20) + (int)uVar10) - (int)pbVar8);
      uVar10 = (*g_FatalErrorPrimaryDispatchCf)();
      puVar9 = (undefined4 *)uVar10;
      for (uVar5 = extraout_ECX_02 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
        *puVar9 = *(undefined4 *)pbVar8;
        pbVar8 = pbVar8 + 4;
        puVar9 = puVar9 + 1;
      }
      UiTransferMailbox_SetOutgoingBuffer
                ((UiTransferPayloadByteCount)((ulonglong)uVar10 >> 0x20),(undefined4 *)uVar10);
      uVar6 = extraout_ECX_03;
    }
    *(FieldGridAsset **)(pbVar2 + 0xb0) = sourceGrid;
    FrontendPlayerRuntime_InitializeFactionAssignments(uVar6,sourceGrid);
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
undefined8
ScenarioCatalog_RebuildSaveRecordListPage
          (dword argument1,dword argument2,dword argument3,dword argument4)

{
  UiListRowCount rowCount;
  UiNodeBase *firstNode;
  undefined4 in_EAX;
  UiPointerListControl *control;
  UiListRowCount UVar1;
  undefined4 in_EDX;
  void *pvVar2;
  void **rowPointers;
  void **ppvVar3;
  undefined8 uVar4;
  
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
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout
                (rowCount,rowPointers,(UiPointerListControl *)&firstNode[0x6a].layoutHeight);
      uVar4 = UiPointerList_SortByDwordPairFieldDescending(0xf0,control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)uVar4);
      UiNodeList_UnsuppressActionId(0x2038,firstNode);
      goto ScenarioCatalog_RebuildSaveRecordListPage_ReturnAfterListAndActionUpdate;
    }
  }
  UiPointerList_InitializeColumnLayout
            (0,(void **)0x0,(UiPointerListControl *)&firstNode[0x6a].layoutHeight);
  UiNodeList_SuppressActionId(0x2038,firstNode);
ScenarioCatalog_RebuildSaveRecordListPage_ReturnAfterListAndActionUpdate:
  return CONCAT44(in_EDX,in_EAX);
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
void ScenarioCatalog_RebuildLevelRecordListPage
               (dword argument1,dword argument2,dword argument3,dword argument4)

{
  UiNodeBase *firstNode;
  word *pwVar1;
  UiPointerListControl *control;
  UiPointerListControl *control_00;
  int extraout_ECX;
  ScenarioCatalogRecordCount rowCount;
  void *pvVar2;
  void **rowPointers;
  void **ppvVar3;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&g_FrontendRootNode[100].firstChild);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&firstNode[0x67].bottomAnchorQ31);
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    rowCount = g_ScenarioCatalog->levelRecordCount;
    pvVar2 = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                     g_ScenarioCatalog->levelRecordsOffset);
    rowPointers = (void **)(rowCount * 0x100 + (int)pvVar2);
    ppvVar3 = rowPointers;
    if (rowCount != 0) {
      do {
        *ppvVar3 = pvVar2;
        pwVar1 = TextResource_Resolve(*(int *)((int)pvVar2 + 0x50) + 0x220a);
        *(undefined2 *)((int)pvVar2 + 0x54) = 0x8019;
        *(word **)((int)pvVar2 + 0x56) = pwVar1;
        pwVar1 = TextResource_Resolve(*(int *)((int)pvVar2 + 0x60) + 0x2200);
        *(undefined2 *)((int)pvVar2 + 100) = 0x8019;
        *(word **)((int)pvVar2 + 0x66) = pwVar1;
        pwVar1 = TextResource_Resolve(*(int *)((int)pvVar2 + 0x70) + 0x2230);
        *pwVar1 = 0x8001;
        *(undefined2 *)((int)pvVar2 + 0x74) = 0x8019;
        *(word **)((int)pvVar2 + 0x76) = pwVar1;
        pwVar1 = TextResource_Resolve(*(int *)((int)pvVar2 + 0x80) + 0x2205);
        *(undefined2 *)((int)pvVar2 + 0x84) = 0x8019;
        *(word **)((int)pvVar2 + 0x86) = pwVar1;
        pvVar2 = (void *)((int)pvVar2 + 0x100);
        ppvVar3 = ppvVar3 + 1;
      } while (extraout_ECX != 1);
    }
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout
                (rowCount,rowPointers,(UiPointerListControl *)&firstNode[0x70].layoutHeight);
      UiPointerList_SortByExpandedTextFieldAscending(0x74,control);
      UiPointerList_SelectIndexVariantB(0,control_00);
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
undefined8
ScenarioCatalog_RebuildCampaignRecordListPage
          (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3)

{
  UiNodeBase *firstNode;
  undefined4 in_EAX;
  word *pwVar1;
  UiPointerListControl *control;
  int extraout_ECX;
  ScenarioCatalogRecordCount rowCount;
  undefined4 in_EDX;
  void *pvVar2;
  void **rowPointers;
  void **ppvVar3;
  undefined8 uVar4;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)&g_FrontendRootNode[0x65].right);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&firstNode[0x67].bottomAnchorQ31);
  if (g_ScenarioCatalog == (ScenarioCatalogHeader *)0x0) {
ScenarioCatalog_RebuildCampaignRecordListPage_InitializeEmptyCampaignList:
    UiPointerList_InitializeColumnLayout
              (0,(void **)0x0,(UiPointerListControl *)&firstNode[0x77].left);
  }
  else {
    rowCount = g_ScenarioCatalog->campaignRecordCount;
    pvVar2 = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                     g_ScenarioCatalog->campaignRecordsOffset);
    rowPointers = (void **)(rowCount * 0x100 + (int)pvVar2);
    ppvVar3 = rowPointers;
    if (rowCount != 0) {
      do {
        *ppvVar3 = pvVar2;
        pwVar1 = TextResource_Resolve(*(int *)((int)pvVar2 + 0x50) + 0x2220);
        *(undefined2 *)((int)pvVar2 + 0x54) = 0x8019;
        *(word **)((int)pvVar2 + 0x56) = pwVar1;
        pvVar2 = (void *)((int)pvVar2 + 0x100);
        ppvVar3 = ppvVar3 + 1;
      } while (extraout_ECX != 1);
    }
    if (rowCount == 0)
    goto ScenarioCatalog_RebuildCampaignRecordListPage_InitializeEmptyCampaignList;
    UiPointerList_InitializeColumnLayout
              (rowCount,rowPointers,(UiPointerListControl *)&firstNode[0x77].left);
    uVar4 = UiPointerList_SortByDwordFieldAscending(0x50,control);
    UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)uVar4);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
      UiNodeList_UnsuppressActionId(0x2038,firstNode);
      UiNodeList_UnsuppressActionId(0x203b,firstNode);
      goto ScenarioCatalog_RebuildCampaignRecordListPage_ReturnAfterActionAvailabilityUpdate;
    }
  }
  UiNodeList_SuppressActionId(0x2038,firstNode);
  UiNodeList_SuppressActionId(0x203b,firstNode);
ScenarioCatalog_RebuildCampaignRecordListPage_ReturnAfterActionAvailabilityUpdate:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00549F70.
   Ownership: assets/scenario/catalog.
   Purpose: Merges source records of exactly 0x100 bytes into a destination array. It compares the first 0x40
   identifier bytes, replaces matching records, appends new identifiers, and returns the updated destination count
   in EDX; EAX is preserved by the original routine.
*/
undefined8
ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords)

{
  undefined4 in_EAX;
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
      return CONCAT44(existingRecordCount,in_EAX);
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
void __fastcall
FrontendScenarioSession_LoadOrRequestLevelAsset(undefined4 param_1,undefined4 param_2)

{
  FrontendRoleStateFlags *pFVar1;
  PckDecodedByteCount sourceSizeBytes;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  byte *source;
  int iVar4;
  uint uVar5;
  uint uVar6;
  UiPageStackControl *stack;
  byte *destination;
  byte *pbVar7;
  FrontendPlayerRuntimeRecord *pFVar8;
  undefined4 *puVar9;
  bool bVar10;
  undefined8 uVar11;
  int in_stack_00000010;
  
  iVar4 = g_FrontendRootNode;
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             *(word **)(*(int *)(g_FrontendRootNode + 0x21d4) + in_stack_00000010 * 4),
             (word *)u_level_0050daac);
  WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
  UiPageStack_SetActiveIndex(0,stack);
  *(uint *)(iVar4 + 0x3b4) = *(uint *)(iVar4 + 0x3b4) & 0xffffdfff;
  FrontendState_DispatchCode(1);
  if ((g_FrontendLoadedLevelAsset != (void *)0x0) &&
     (0xffff < *(uint *)((int)g_FrontendLoadedLevelAsset + 0xb0))) {
    Resource_Release(*(void **)((int)g_FrontendLoadedLevelAsset + 0xb0));
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = (byte *)0x0;
  pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *pFVar1 = *pFVar1 | 2;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    source = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
    pbVar7 = g_PackageScratchBuffer;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      sourceSizeBytes = *(PckDecodedByteCount *)(source + 4);
      destination = g_PackageScratchBuffer + 4;
      g_FrontendLoadedLevelAsset = source;
      *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceSizeBytes;
      PckCodec_EncodeHuffmanRle(0x7ffffc,destination,sourceSizeBytes,source);
      iVar4 = (*g_FatalErrorPrimaryDispatchCf)();
      (*g_MemoryApi.alloc)(iVar4 + 4);
      uVar11 = (*g_FatalErrorPrimaryDispatchCf)();
      uVar6 = (uint)((ulonglong)uVar11 >> 0x20);
      puVar9 = (undefined4 *)uVar11;
      for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
        *puVar9 = *(undefined4 *)pbVar7;
        pbVar7 = pbVar7 + 4;
        puVar9 = puVar9 + 1;
      }
      UiTransferMailbox_SetOutgoingBuffer(uVar6,(undefined4 *)uVar11);
      source = g_FrontendLoadedLevelAsset;
    }
  }
  else {
    uVar5 = (*(int *)(*(int *)(g_FrontendRootNode + 0x21d4) + in_stack_00000010 * 4) -
            (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
    uVar6 = uVar5 >> 0xd;
    if (uVar6 < 3) {
      iVar4 = g_FrontendPlayerRuntimeBlockCount - 1;
      pFVar8 = g_FrontendPlayerRuntimeBlocks;
      do {
        if (g_LocalPlayerRuntimeId == pFVar8[1].playerRuntimeId) {
          bVar10 = false;
          if (((*(uint *)(pFVar8[1].reserved78_7F + uVar6 * 4 + 0xc) &
               1 << ((byte)(uVar5 >> 8) & 0x1f)) != 0) &&
             (source = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16), !bVar10))
          goto FrontendScenarioSession_CommitLoadedLevelAsset;
          break;
        }
        iVar4 = iVar4 + -1;
        pFVar8 = pFVar8 + 1;
      } while (iVar4 != 0);
    }
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = 2;
    source = g_FrontendLoadedLevelAsset;
  }
FrontendScenarioSession_CommitLoadedLevelAsset:
  g_FrontendLoadedLevelAsset = source;
  uVar5 = (*(int *)(*(int *)(g_FrontendRootNode + 0x21d4) + in_stack_00000010 * 4) -
          (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
  uVar6 = uVar5 >> 0xd;
  pFVar8 = g_FrontendPlayerRuntimeBlocks;
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  if (uVar6 < 3) {
    while (pFVar3 = pFVar8, FVar2 = FVar2 - 1, FVar2 != 0) {
      pFVar8 = pFVar3 + 1;
      if ((*(uint *)(pFVar3[1].reserved78_7F + uVar6 * 4 + 0xc) & 1 << ((byte)(uVar5 >> 8) & 0x1f))
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
void ScenarioCatalog_RefreshSelectedRecordLocalizedText
               (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex)

{
  int *piVar1;
  UiPointerListControl *control;
  int iVar2;
  int iVar3;
  word *pwVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  word *stream;
  word *stream_00;
  undefined4 extraout_EDX;
  undefined8 uVar5;
  
  iVar3 = g_FrontendRootNode;
  piVar1 = (int *)(g_FrontendRootNode + 0x200c);
  control = (UiPointerListControl *)(g_FrontendRootNode + 0x1fbc);
  *(undefined4 *)(g_FrontendRootNode + 0x20ec) = 0x215d;
  if (*piVar1 != 0) {
    uVar5 = UiPointerList_SelectIndexVariantB(selectionIndex,control);
    iVar2 = *(int *)((int)((ulonglong)uVar5 >> 0x20) + extraout_ECX * 4);
    if (*(int *)(iVar2 + 0x90) < 0) {
      pwVar4 = TextResource_Resolve(*(TextResourceId *)(iVar2 + 0x70));
      *pwVar4 = 0x8000;
      *(undefined4 *)(iVar3 + 0x20ec) = extraout_EDX;
    }
    else {
      TextResource_Resolve(0x215e);
      pwVar4 = TextResource_Resolve(*(TextResourceId *)(extraout_ECX_00 + 0x70));
      *pwVar4 = 0x8000;
      RichTextCommandStream_PatchPayloadBySelector(1,pwVar4,stream);
      pwVar4 = TextResource_Resolve(*(TextResourceId *)(extraout_ECX_01 + 0x90));
      RichTextCommandStream_PatchPayloadBySelector(0,pwVar4,stream_00);
      *(undefined4 *)(iVar3 + 0x20ec) = 0x215e;
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
void ScenarioCatalog_RefreshSelectedRecordField50DisplayId
               (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex)

{
  int *piVar1;
  UiPointerListControl *control;
  int iVar2;
  int extraout_ECX;
  undefined8 uVar3;
  
  iVar2 = g_FrontendRootNode;
  piVar1 = (int *)(g_FrontendRootNode + 0x23b4);
  control = (UiPointerListControl *)(g_FrontendRootNode + 0x2364);
  *(undefined4 *)(g_FrontendRootNode + 0x249c) = 0x215d;
  if (*piVar1 != 0) {
    uVar3 = UiPointerList_SelectIndexVariantB(selectionIndex,control);
    *(int *)(iVar2 + 0x249c) =
         *(int *)(*(int *)((int)((ulonglong)uVar3 >> 0x20) + extraout_ECX * 4) + 0x50) + 0x230000;
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
undefined8
FrontendScenarioSelection_ActivateSelectedRecord
          (FrontendScenarioSelectionControlAddress32 selectionControl)

{
  undefined4 in_EAX;
  UiListRowIndex selectedRowIndex;
  UiListRowIndex UVar1;
  uint extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 in_EDX;
  UiPointerListControl *control;
  undefined4 extraout_EDX;
  UiPointerListControl *control_00;
  undefined4 extraout_EDX_00;
  UiPointerListControl *control_01;
  undefined1 in_CF;
  int scenarioPathPointerTableAddress;
  
  UiSelectableGroup_NoneVisibleSelectedCf(3);
  if ((bool)in_CF) {
    return CONCAT44(in_EDX,in_EAX);
  }
  if (extraout_ECX != 0) {
    if (extraout_ECX < 2) {
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = (void *)0x0;
      selectedRowIndex = UiPointerList_GetSelectedIndexVariantBCf(control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendScenarioSession_LoadOrRequestLevelAsset(extraout_ECX_00,extraout_EDX);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x920,0,0,selectedRowIndex);
      }
      return CONCAT44(in_EDX,in_EAX);
    }
    Resource_Release(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = (void *)0x0;
    UVar1 = UiPointerList_GetSelectedIndexVariantBCf(control_00);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendScenarioSession_LoadOrRequestCampaignBundle(extraout_ECX_01,extraout_EDX_00);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0xb70,0,0,UVar1);
    }
    return CONCAT44(in_EDX,in_EAX);
  }
  scenarioPathPointerTableAddress = *(int *)(selectionControl + 0x318);
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf(control_01);
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             *(word **)(scenarioPathPointerTableAddress + UVar1 * 4),(word *)u_save_0050daa2);
  WidePath_SetExtensionCode(0x657673,&g_FrontendScenarioPathScratchUtf16);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,2);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,2);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005451F0.
   Ownership: assets/scenario/catalog.
   Purpose: Selection-change callback for the second scenario list. It derives the output display ID as record
   field +0x70 times 0x10 plus 0x230010. Typed parameters: p3 selectionIndex→UiListRowIndex_V300. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
undefined8
ScenarioCatalog_RefreshSelectedRecordField70DisplayId
          (dword arg0,dword arg1,dword arg2,UiListRowIndex selectionIndex)

{
  int *piVar1;
  UiPointerListControl *control;
  int iVar2;
  undefined4 in_EAX;
  int extraout_ECX;
  undefined4 in_EDX;
  undefined8 uVar3;
  
  iVar2 = g_FrontendRootNode;
  piVar1 = (int *)(g_FrontendRootNode + 0x21d4);
  control = (UiPointerListControl *)(g_FrontendRootNode + 0x2184);
  *(undefined4 *)(g_FrontendRootNode + 0x22cc) = 0x215d;
  if (*piVar1 != 0) {
    uVar3 = UiPointerList_SelectIndexVariantB(selectionIndex,control);
    *(int *)(iVar2 + 0x22cc) =
         *(int *)(*(int *)((int)((ulonglong)uVar3 >> 0x20) + extraout_ECX * 4) + 0x70) * 0x10 +
         0x230010;
  }
  return CONCAT44(in_EDX,in_EAX);
}
