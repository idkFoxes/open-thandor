/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/savegame.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/savegame.h>

/* Implementation ownership: gameplay/session/savegame. */

/* Address: 0x0056C030.
   Ownership: gameplay/session/savegame.
   Purpose: Recovered action-table target INGAME_PAGE12[15] (0x120F).
   Local calls: InGameSaveGame_SaveSelectedOrTypedName.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], UiNode_GetRoot [ui/core/runtime], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameList_SelectAndRefreshDetail(InGameCatalogDetailPageCatalogListPtr catalogList)

{
  void **ppvVar1;
  void *pvVar2;
  TextResourceId resourceId;
  UiListRowIndex UVar3;
  UiNodeBase *firstNode;
  UiListRowIndex UVar4;
  UiListRowIndexEaxCf5 UVar5;
  TextResourceResolveEaxCf5 TVar6;
  TextResourceResolveEaxCf5 TVar7;
  
  ppvVar1 = catalogList->rowSlots;
  UVar4 = catalogList->rowCount - 1;
  UVar5 = UiPointerList_GetSelectedIndexVariantBCf(catalogList);
  UVar3 = UVar5.rowIndex;
  pvVar2 = ppvVar1[UVar3];
  if (UVar5.carry) {
    UiPageStack_SetActiveIndex((uint)(UVar3 == UVar4),&ADJ(catalogList)->detailPageStack);
    if (UVar3 != UVar4) {
      InGameSaveGame_SaveSelectedOrTypedName(ADJ(catalogList));
      return;
    }
  }
  else {
    UiPageStack_SetActiveIndex((uint)(UVar3 == UVar4),&ADJ(catalogList)->detailPageStack);
    ADJ(catalogList)->activeDetailTextResourceId = 0x215d;
    if (UVar3 != UVar4) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = *(TextResourceId *)((int)pvVar2 + 0x70);
        TVar6 = TextResource_Resolve(resourceId);
        *TVar6.eax = 0x8000;
        ADJ(catalogList)->activeDetailTextResourceId = resourceId;
      }
      else {
        TVar6 = TextResource_Resolve(0x215e);
        TVar7 = TextResource_Resolve(*(TextResourceId *)((int)pvVar2 + 0x70));
        *TVar7.eax = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,TVar7.eax,TVar6.eax);
        TVar7 = TextResource_Resolve(*(TextResourceId *)((int)pvVar2 + 0x90));
        RichTextCommandStream_PatchPayloadBySelector(0,TVar7.eax,TVar6.eax);
        ADJ(catalogList)->activeDetailTextResourceId = 0x215e;
      }
      firstNode = UiNode_GetRoot(&(ADJ(catalogList)->detailPageStack).base);
      UiNodeList_UnsuppressActionId(0x1219,firstNode);
      goto InGameUiAction120F_Handler_UnsuppressAction1210AndReturn;
    }
  }
  firstNode = UiNode_GetRoot(&catalogList->base);
  UiNodeList_SuppressActionId(0x1219,firstNode);
  if (((ADJ(catalogList)->action1210Control).nodeFlags & 1) == 0) {
    UiNodeList_SuppressActionId(0x1210,firstNode);
    return;
  }
InGameUiAction120F_Handler_UnsuppressAction1210AndReturn:
  UiNodeList_UnsuppressActionId(0x1210,firstNode);
  return;
}


/* Address: 0x0056C190.
   Ownership: gameplay/session/savegame.
   Purpose: Binary entry is anchored by g_UiActionPage12InitializedHandlers[25]@005625B8. Queued UI action handler
   for INGAME_PAGE12[25] (0x1219). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   saveGamePageControl→InGameSaveGamePageControlAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: InGameSaveGamePage_RebuildCatalog.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode [core/text/path].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog
          (InGameSaveGamePageControlAddress32 saveGamePageControl)

{
  word *leaf;
  int iVar1;
  dword errorOrValue;
  bool carryIn;
  UiListRowIndexEaxCf5 UVar2;
  
  (*g_GraphicsCursorSetFrame)(6);
  UVar2 = UiPointerList_GetSelectedIndexVariantBCf
                    ((UiPointerListControl *)(saveGamePageControl + 0xf0));
  iVar1 = UVar2.rowIndex + 1;
  if (iVar1 != *(int *)(saveGamePageControl + 0x144)) {
    leaf = *(word **)(*(int *)(saveGamePageControl + 0x140) + -4 + iVar1 * 4);
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
               (word *)&g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
               (word *)&g_ScenarioCatalogPathScratchUtf16);
    carryIn = WidePath_SetExtensionCode(0x657673,(word *)&g_ScenarioCatalogPathScratchUtf16);
    errorOrValue = (*g_FileSystemDeleteCf)(0,(word *)&g_ScenarioCatalogPathScratchUtf16);
    (*g_FatalErrorRuntimeDispatchCf)(errorOrValue,carryIn);
    InGameSaveGamePage_RebuildCatalog((UiRootNode *)(saveGamePageControl + -0x760));
  }
  return;
}


/* Address: 0x0056BDD0.
   Ownership: gameplay/session/savegame.
   Purpose: Enumerates save/*.sve, allocates and fills 0x104-byte catalog records, adds the localized New score row
   0x2151, sorts by the persisted dword pair, selects the current row, and updates save-page actions and
   description 0x215E. Queued UI action handler for INGAME_PAGE12[14] (0x120E). Return datatype is preserved for
   non-queue direct callers.
   Local calls: InGameSaveName_UpdateSaveActionValidity.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_CopyExpandedCf [assets/text/richtext],
   UiPointerList_InitializeColumnLayout [ui/controls/lists], UiPointerList_SortByDwordPairFieldDescending
   [ui/controls/lists], UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSaveGamePage_RebuildCatalog(UiRootNode *savePageRoot)

{
  UiNodeBase *pUVar1;
  UiRootNode *firstNode;
  UiNodeVtable *pUVar2;
  TextResourceId resourceId;
  ScenarioCatalogHeader *pSVar3;
  void *handle;
  sdword *psVar4;
  dword dVar5;
  int iVar6;
  word *leaf;
  ScenarioCatalogByteOffset *destination;
  ScenarioCatalogByteOffset *pSVar7;
  ArenaAllocEaxCf5 AVar8;
  FileSystemOpenEaxCf5 FVar9;
  TextResourceResolveEaxCf5 TVar10;
  UiListRowIndexEaxCf5 UVar11;
  TextResourceResolveEaxCf5 TVar12;
  FileSystemEnumerationEaxEcxCf9 FVar13;
  void *handle_00;
  dword rowCount;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save___sve_0050d9c8,
             (word *)&g_ExecutableDirectoryUtf16);
  FVar13 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                     (FILESYSTEM_ENUMERATE_FILES,0xffffffff,0x800000,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  dVar5 = FVar13.entryCount;
  if (FVar13.carry) {
    dVar5 = 0;
  }
  (*g_MemoryApi.free)(g_ScenarioCatalog);
  g_ScenarioCatalog = (ScenarioCatalogHeader *)0x0;
  AVar8 = (*g_MemoryApi.alloc)((dVar5 + 1) * 0x104);
  pSVar3 = (ScenarioCatalogHeader *)AVar8.eax;
  if (!AVar8.carry) {
    destination = &pSVar3->campaignRecordsOffset + dVar5;
    g_ScenarioCatalog = pSVar3;
    rowCount = dVar5;
    leaf = (word *)g_PackageScratchBuffer;
    for (; dVar5 != 0; dVar5 = dVar5 - 1) {
      pSVar3->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
      *destination = 0;
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
                 (word *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
                 (word *)&g_ScenarioCatalogPathScratchUtf16);
      FVar9 = (*g_FileSystemOpenCf)
                        (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,(word *)&g_ScenarioCatalogPathScratchUtf16)
      ;
      handle = (void *)FVar9.eax;
      if (!FVar9.carry) {
        handle_00 = handle;
        (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0x100,handle);
        (*g_FileSystemReadExactCf)(0x100,destination,handle);
        (*g_FileSystemClose)(handle_00);
        destination[0x1c] = destination[0x1c] + 0x2230;
        destination[0x24] = destination[0x24] + 0x2220;
      }
      pSVar3 = (ScenarioCatalogHeader *)&pSVar3->campaignRecordsOffset;
      destination = destination + 0x40;
      leaf = (word *)((int)leaf + FVar13.recordSizeBytes);
    }
    pSVar3->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
    pSVar7 = destination;
    for (iVar6 = 0x40; iVar6 != 0; iVar6 = iVar6 + -1) {
      *pSVar7 = 0;
      pSVar7 = pSVar7 + 1;
    }
    TVar10 = TextResource_Resolve(0x2151);
    RichTextCommandStream_CopyExpandedCf(0x100,(word *)destination,TVar10.eax);
    psVar4 = &savePageRoot[0x18].base.left;
    UiPointerList_InitializeColumnLayout
              (rowCount,(void **)g_ScenarioCatalog,(UiPointerListControl *)psVar4);
    UiPointerList_SortByDwordPairFieldDescending(0xf0,(UiPointerListControl *)psVar4);
    UiPointerList_InitializeColumnLayout
              (rowCount + 1,(void **)g_ScenarioCatalog,(UiPointerListControl *)psVar4);
    UiPointerList_SelectIndexVariantB(rowCount,(UiPointerListControl *)psVar4);
    UiPageStack_SetActiveIndex(5,(UiPageStackControl *)&savePageRoot[-0x4b].base.nodeFlags);
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&savePageRoot[0x1b].base.bottomAnchorQ31);
    pUVar1 = (savePageRoot->base).parent;
    firstNode = savePageRoot;
    while (pUVar1 != (UiNodeBase *)0xffffffff) {
      firstNode = (UiRootNode *)(firstNode->base).parent;
      pUVar1 = (firstNode->base).parent;
    }
    UiNodeList_SuppressActionId(0x1210,&firstNode->base);
    UiNodeList_SuppressActionId(0x1219,&firstNode->base);
    psVar4 = &firstNode[0x89].base.top;
    UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)psVar4);
    InGameSaveName_UpdateSaveActionValidity((UiNodeBase *)psVar4);
    pUVar2 = savePageRoot[0x19].base.vtable;
    pUVar1 = savePageRoot[0x19].base.parent;
    UVar11 = UiPointerList_GetSelectedIndexVariantBCf
                       ((UiPointerListControl *)&savePageRoot[0x18].base.left);
    pUVar1 = (&pUVar1->nextSibling)[(int)UVar11.rowIndex];
    savePageRoot[0x1b].base.topAnchorQ31 = 0x215d;
    if ((undefined1 *)((int)&pUVar2[-1].pointerWheel + 3U) != (undefined1 *)UVar11.rowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = pUVar1[1].topOffset;
        TVar10 = TextResource_Resolve(resourceId);
        *TVar10.eax = 0x8000;
        savePageRoot[0x1b].base.topAnchorQ31 = resourceId;
      }
      else {
        TVar10 = TextResource_Resolve(0x215e);
        TVar12 = TextResource_Resolve(pUVar1[1].topOffset);
        *TVar12.eax = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,TVar12.eax,TVar10.eax);
        TVar12 = TextResource_Resolve(pUVar1[1].layoutHeight);
        RichTextCommandStream_PatchPayloadBySelector(0,TVar12.eax,TVar10.eax);
        savePageRoot[0x1b].base.topAnchorQ31 = 0x215e;
      }
    }
  }
  return;
}


/* Address: 0x0056C230.
   Ownership: gameplay/session/savegame.
   Purpose: Recovered action-table target INGAME_PAGE12[16] (0x1210).
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode [core/text/path],
   InGameUiAction1210_ResourceRegistrationHelper [ui/ingame/runtime], UiSelectableControl_SetSelected
   [ui/controls/lists], InGameSettingsPage_ToggleAndSynchronizeControls [ui/ingame/settings].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSaveGame_SaveSelectedOrTypedName(void *source)

{
  dword errorOrValue;
  word *leaf;
  byte bVar1;
  UiListRowIndexEaxCf5 UVar2;
  uint uVar3;
  
  (*g_GraphicsCursorSetFrame)(6);
  UVar2 = UiPointerList_GetSelectedIndexVariantBCf((UiPointerListControl *)((int)source + 0x150));
  errorOrValue = UVar2.rowIndex + 1;
  leaf = (word *)((int)source + 0x348);
  if (errorOrValue != *(dword *)((int)source + 0x1a4)) {
    leaf = *(word **)(*(int *)((int)source + 0x1a0) + -4 + errorOrValue * 4);
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
             (word *)&g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
             (word *)&g_ScenarioCatalogPathScratchUtf16);
  bVar1 = WidePath_SetExtensionCode(0x657673,(word *)&g_ScenarioCatalogPathScratchUtf16);
  InGameUiAction1210_ResourceRegistrationHelper
            ((void *)((int)source + -0x2220),&g_ScenarioCatalogPathScratchUtf16);
  uVar3 = (uint)(bVar1 & 1);
  (*g_GraphicsCursorSetFrame)(0);
  (*g_FatalErrorRuntimeDispatchCf)(errorOrValue,(uVar3 & 1) != 0);
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)((int)source + 0x1738));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)((int)source + 0x1738));
  return;
}


/* Address: 0x0056C2F0.
   Ownership: gameplay/session/savegame.
   Purpose: Walks to the page root, validates the current UTF-16 save name, and enables action 0x1210 only for
   nonempty names that contain none of the forbidden path characters. Queued UI action handler for
   INGAME_PAGE12[17] (0x1211). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiNodeList_UnsuppressActionId [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl)

{
  UiNodeBase *pUVar1;
  UiNodeBase *firstNode;
  UiNodeVtable *pUVar2;
  uint uVar3;
  uint uVar4;
  sdword *psVar5;
  sdword *psVar6;
  bool bVar7;
  
  pUVar1 = nameControl->parent;
  firstNode = nameControl;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    firstNode = firstNode->parent;
    pUVar1 = firstNode->parent;
  }
  if (((uint)nameControl[1].nextSibling & 1) != 0) {
    pUVar2 = nameControl[1].vtable;
    bVar7 = true;
    psVar6 = &nameControl[1].leftOffset;
    do {
      psVar5 = psVar6;
      if (pUVar2 == (UiNodeVtable *)0x0) break;
      pUVar2 = (UiNodeVtable *)((int)&pUVar2[-1].pointerWheel + 3);
      psVar5 = (sdword *)((int)psVar6 + 2);
      bVar7 = (short)*psVar6 == 0;
      psVar6 = psVar5;
    } while (!bVar7);
    if (bVar7) {
      uVar4 = (uint)-((int)&nameControl[1].leftOffset - (int)psVar5) >> 1;
      bVar7 = uVar4 == 0;
      uVar3 = uVar4;
      psVar6 = &nameControl[1].leftOffset;
      do {
        if (uVar3 == 0) break;
        uVar3 = uVar3 - 1;
        bVar7 = (short)*psVar6 == 0x2a;
        psVar6 = (sdword *)((int)psVar6 + 2);
      } while (!bVar7);
      if (!bVar7) {
        uVar3 = uVar4;
        psVar6 = &nameControl[1].leftOffset;
        do {
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          bVar7 = (short)*psVar6 == 0x2e;
          psVar6 = (sdword *)((int)psVar6 + 2);
        } while (!bVar7);
        if (!bVar7) {
          uVar3 = uVar4;
          psVar6 = &nameControl[1].leftOffset;
          do {
            if (uVar3 == 0) break;
            uVar3 = uVar3 - 1;
            bVar7 = (short)*psVar6 == 0x5c;
            psVar6 = (sdword *)((int)psVar6 + 2);
          } while (!bVar7);
          if (!bVar7) {
            uVar3 = uVar4;
            psVar6 = &nameControl[1].leftOffset;
            do {
              if (uVar3 == 0) break;
              uVar3 = uVar3 - 1;
              bVar7 = (short)*psVar6 == 0x3f;
              psVar6 = (sdword *)((int)psVar6 + 2);
            } while (!bVar7);
            if (!bVar7) {
              uVar3 = uVar4;
              psVar6 = &nameControl[1].leftOffset;
              do {
                if (uVar3 == 0) break;
                uVar3 = uVar3 - 1;
                bVar7 = (short)*psVar6 == 0x3c;
                psVar6 = (sdword *)((int)psVar6 + 2);
              } while (!bVar7);
              if (!bVar7) {
                uVar3 = uVar4;
                psVar6 = &nameControl[1].leftOffset;
                do {
                  if (uVar3 == 0) break;
                  uVar3 = uVar3 - 1;
                  bVar7 = (short)*psVar6 == 0x3e;
                  psVar6 = (sdword *)((int)psVar6 + 2);
                } while (!bVar7);
                if (!bVar7) {
                  uVar3 = uVar4;
                  psVar6 = &nameControl[1].leftOffset;
                  do {
                    if (uVar3 == 0) break;
                    uVar3 = uVar3 - 1;
                    bVar7 = (short)*psVar6 == 0x3a;
                    psVar6 = (sdword *)((int)psVar6 + 2);
                  } while (!bVar7);
                  if (!bVar7) {
                    uVar3 = uVar4;
                    psVar6 = &nameControl[1].leftOffset;
                    do {
                      if (uVar3 == 0) break;
                      uVar3 = uVar3 - 1;
                      bVar7 = (short)*psVar6 == 0x22;
                      psVar6 = (sdword *)((int)psVar6 + 2);
                    } while (!bVar7);
                    if (!bVar7) {
                      uVar3 = uVar4;
                      psVar6 = &nameControl[1].leftOffset;
                      do {
                        if (uVar3 == 0) break;
                        uVar3 = uVar3 - 1;
                        bVar7 = (short)*psVar6 == 0x7c;
                        psVar6 = (sdword *)((int)psVar6 + 2);
                      } while (!bVar7);
                      if (!bVar7) {
                        psVar6 = &nameControl[1].leftOffset;
                        do {
                          if (uVar4 == 0) break;
                          uVar4 = uVar4 - 1;
                          bVar7 = (short)*psVar6 == 0x2f;
                          psVar6 = (sdword *)((int)psVar6 + 2);
                        } while (!bVar7);
                        if (!bVar7) {
                          UiNodeList_UnsuppressActionId(0x1210,firstNode);
                          return;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  UiNodeList_SuppressActionId(0x1210,firstNode);
  return;
}

