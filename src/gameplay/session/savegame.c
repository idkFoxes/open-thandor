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
void InGameSaveGameList_SelectAndRefreshDetail(InGameCatalogDetailPageCatalogListPtr catalogList)

{
  void **ppvVar1;
  void *pvVar2;
  UiListRowIndex UVar3;
  int extraout_EAX;
  word *pwVar4;
  UiNodeBase *pUVar5;
  UiNodeBase *extraout_EAX_00;
  int extraout_EAX_01;
  UiNodeBase *firstNode;
  UiListRowIndex extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  UiPageIndex extraout_EDX;
  UiPageIndex UVar6;
  word *stream;
  word *stream_00;
  UiTextResourceId extraout_EDX_00;
  bool bVar7;
  
  ppvVar1 = catalogList->rowSlots;
  bVar7 = false;
  UVar3 = UiPointerList_GetSelectedIndexVariantBCf(catalogList);
  pvVar2 = ppvVar1[UVar3];
  if (bVar7) {
    UVar6 = extraout_EDX;
    if (UVar3 == extraout_ECX) {
      UVar6 = extraout_EDX + 1;
    }
    UiPageStack_SetActiveIndex(UVar6,&ADJ(catalogList)->detailPageStack);
    if (extraout_EAX_01 != extraout_ECX_01) {
      InGameSaveGame_SaveSelectedOrTypedName(ADJ(catalogList));
      return;
    }
  }
  else {
    UVar6 = extraout_EDX;
    if (UVar3 == extraout_ECX) {
      UVar6 = extraout_EDX + 1;
    }
    UiPageStack_SetActiveIndex(UVar6,&ADJ(catalogList)->detailPageStack);
    ADJ(catalogList)->activeDetailTextResourceId = 0x215d;
    if (extraout_EAX != extraout_ECX_00) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        pwVar4 = TextResource_Resolve(*(TextResourceId *)((int)pvVar2 + 0x70));
        *pwVar4 = 0x8000;
        ADJ(catalogList)->activeDetailTextResourceId = extraout_EDX_00;
      }
      else {
        TextResource_Resolve(0x215e);
        pwVar4 = TextResource_Resolve(*(TextResourceId *)((int)pvVar2 + 0x70));
        *pwVar4 = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,pwVar4,stream);
        pwVar4 = TextResource_Resolve(*(TextResourceId *)((int)pvVar2 + 0x90));
        RichTextCommandStream_PatchPayloadBySelector(0,pwVar4,stream_00);
        ADJ(catalogList)->activeDetailTextResourceId = 0x215e;
      }
      pUVar5 = UiNode_GetRoot(&(ADJ(catalogList)->detailPageStack).base);
      UiNodeList_UnsuppressActionId(0x1219,pUVar5);
      pUVar5 = extraout_EAX_00;
      goto InGameUiAction120F_Handler_UnsuppressAction1210AndReturn;
    }
  }
  pUVar5 = UiNode_GetRoot(&catalogList->base);
  UiNodeList_SuppressActionId(0x1219,pUVar5);
  pUVar5 = firstNode;
  if (((ADJ(catalogList)->action1210Control).nodeFlags & 1) == 0) {
    UiNodeList_SuppressActionId(0x1210,firstNode);
    return;
  }
InGameUiAction120F_Handler_UnsuppressAction1210AndReturn:
  UiNodeList_UnsuppressActionId(0x1210,pUVar5);
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
undefined8
InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog
          (InGameSaveGamePageControlAddress32 saveGamePageControl)

{
  word *leaf;
  undefined4 in_EAX;
  UiListRowIndex UVar1;
  undefined4 in_EDX;
  int extraout_EDX;
  
  (*g_GraphicsCursorSetFrame)(6);
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf
                    ((UiPointerListControl *)(saveGamePageControl + 0xf0));
  if (UVar1 + 1 != *(int *)(extraout_EDX + 0x54)) {
    leaf = *(word **)(*(int *)(extraout_EDX + 0x50) + -4 + (UVar1 + 1) * 4);
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
               (word *)&g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
               (word *)&g_ScenarioCatalogPathScratchUtf16);
    WidePath_SetExtensionCode(0x657673,(word *)&g_ScenarioCatalogPathScratchUtf16);
    (*g_FileSystemDeleteCf)(0,(word *)&g_ScenarioCatalogPathScratchUtf16);
    (*g_FatalErrorRuntimeDispatchCf)();
    InGameSaveGamePage_RebuildCatalog((UiRootNode *)(saveGamePageControl + -0x760));
  }
  return CONCAT44(in_EDX,in_EAX);
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
undefined8 InGameSaveGamePage_RebuildCatalog(UiRootNode *savePageRoot)

{
  UiRootNode *firstNode;
  longlong lVar1;
  undefined4 in_EAX;
  ScenarioCatalogHeader *pSVar2;
  void *handle;
  UiPointerListControl *control;
  UiPointerListControl *control_00;
  UiNodeBase *firstNode_00;
  int extraout_EAX;
  UiNodeBase *pUVar3;
  UiListRowIndex UVar4;
  int extraout_ECX;
  int iVar5;
  UiListRowCount extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  UiListRowIndex extraout_ECX_05;
  undefined4 in_EDX;
  uint extraout_EDX;
  uint extraout_EDX_00;
  uint uVar6;
  word *stream;
  word *stream_00;
  UiAnchorFractionQ31 extraout_EDX_01;
  word *pwVar7;
  ScenarioCatalogByteOffset *destination;
  dword bytes;
  ScenarioCatalogByteOffset *pSVar8;
  undefined1 in_CF;
  undefined1 uVar9;
  undefined8 uVar10;
  void *handle_00;
  UiListRowCount rowCount;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save___sve_0050d9c8,
             (word *)&g_ExecutableDirectoryUtf16);
  (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
            (FILESYSTEM_ENUMERATE_FILES,0xffffffff,0x800000,g_PackageScratchBuffer,
             &g_ScenarioCatalogPathScratchUtf16);
  iVar5 = extraout_ECX;
  if ((bool)in_CF) {
    iVar5 = 0;
  }
  lVar1 = (longlong)(iVar5 + 1) * 0x104;
  bytes = (dword)lVar1;
  uVar9 = (int)bytes != lVar1;
  (*g_MemoryApi.free)(g_ScenarioCatalog);
  g_ScenarioCatalog = (ScenarioCatalogHeader *)0x0;
  pSVar2 = (*g_MemoryApi.alloc)(bytes);
  if (!(bool)uVar9) {
    destination = &pSVar2->campaignRecordsOffset + extraout_ECX_00;
    uVar9 = false;
    pwVar7 = (word *)g_PackageScratchBuffer;
    g_ScenarioCatalog = pSVar2;
    rowCount = extraout_ECX_00;
    if (extraout_ECX_00 != 0) {
      do {
        pSVar2->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
        *destination = 0;
        WidePath_CombineDirectoryAndLeaf
                  ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
                   (word *)&g_ExecutableDirectoryUtf16);
        WidePath_CombineDirectoryAndLeaf
                  ((word *)&g_ScenarioCatalogPathScratchUtf16,pwVar7,
                   (word *)&g_ScenarioCatalogPathScratchUtf16);
        handle = (void *)(*g_FileSystemOpenCf)
                                   (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,
                                    (word *)&g_ScenarioCatalogPathScratchUtf16);
        iVar5 = extraout_ECX_01;
        uVar6 = extraout_EDX;
        if (!(bool)uVar9) {
          handle_00 = handle;
          (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0x100,handle);
          (*g_FileSystemReadExactCf)(0x100,destination,handle);
          (*g_FileSystemClose)(handle_00);
          destination[0x1c] = destination[0x1c] + 0x2230;
          destination[0x24] = destination[0x24] + 0x2220;
          iVar5 = extraout_ECX_02;
          uVar6 = extraout_EDX_00;
        }
        pSVar2 = (ScenarioCatalogHeader *)&pSVar2->campaignRecordsOffset;
        destination = destination + 0x40;
        uVar9 = CARRY4((uint)pwVar7,uVar6);
        pwVar7 = (word *)((int)pwVar7 + uVar6);
      } while (iVar5 != 1);
    }
    pSVar2->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
    pSVar8 = destination;
    for (iVar5 = 0x40; iVar5 != 0; iVar5 = iVar5 + -1) {
      *pSVar8 = 0;
      pSVar8 = pSVar8 + 1;
    }
    pwVar7 = TextResource_Resolve(0x2151);
    RichTextCommandStream_CopyExpandedCf(0x100,(word *)destination,pwVar7);
    UiPointerList_InitializeColumnLayout
              (rowCount,(void **)g_ScenarioCatalog,
               (UiPointerListControl *)&savePageRoot[0x18].base.left);
    uVar10 = UiPointerList_SortByDwordPairFieldDescending(0xf0,control);
    UiPointerList_InitializeColumnLayout
              (extraout_ECX_03 + 1,(void **)g_ScenarioCatalog,(UiPointerListControl *)uVar10);
    UiPointerList_SelectIndexVariantB(extraout_ECX_04 - 1,control_00);
    UiPageStack_SetActiveIndex(5,(UiPageStackControl *)&savePageRoot[-0x4b].base.nodeFlags);
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&savePageRoot[0x1b].base.bottomAnchorQ31);
    pUVar3 = (savePageRoot->base).parent;
    firstNode = savePageRoot;
    while (pUVar3 != (UiNodeBase *)0xffffffff) {
      firstNode = (UiRootNode *)(firstNode->base).parent;
      pUVar3 = (firstNode->base).parent;
    }
    UiNodeList_SuppressActionId(0x1210,&firstNode->base);
    UiNodeList_SuppressActionId(0x1219,firstNode_00);
    pUVar3 = (UiNodeBase *)
             UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)(extraout_EAX + 0x2f2c));
    InGameSaveName_UpdateSaveActionValidity(pUVar3);
    pUVar3 = savePageRoot[0x19].base.parent;
    UVar4 = UiPointerList_GetSelectedIndexVariantBCf
                      ((UiPointerListControl *)&savePageRoot[0x18].base.left);
    pUVar3 = (&pUVar3->nextSibling)[UVar4];
    savePageRoot[0x1b].base.topAnchorQ31 = 0x215d;
    if (extraout_ECX_05 != UVar4) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        pwVar7 = TextResource_Resolve(pUVar3[1].topOffset);
        *pwVar7 = 0x8000;
        savePageRoot[0x1b].base.topAnchorQ31 = extraout_EDX_01;
      }
      else {
        TextResource_Resolve(0x215e);
        pwVar7 = TextResource_Resolve(pUVar3[1].topOffset);
        *pwVar7 = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,pwVar7,stream);
        pwVar7 = TextResource_Resolve(pUVar3[1].layoutHeight);
        RichTextCommandStream_PatchPayloadBySelector(0,pwVar7,stream_00);
        savePageRoot[0x1b].base.topAnchorQ31 = 0x215e;
      }
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0056C230.
   Ownership: gameplay/session/savegame.
   Purpose: Recovered action-table target INGAME_PAGE12[16] (0x1210).
   Cross-module calls: UiPointerList_GetSelectedIndexVariantBCf [ui/controls/lists],
   WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode [core/text/path],
   InGameUiAction1210_ResourceRegistrationHelper [ui/ingame/runtime], UiSelectableControl_SetSelected
   [ui/controls/lists], InGameSettingsPage_ToggleAndSynchronizeControls [ui/ingame/settings].
*/
void InGameSaveGame_SaveSelectedOrTypedName(void *source)

{
  UiListRowIndex UVar1;
  int extraout_EDX;
  word *leaf;
  
  (*g_GraphicsCursorSetFrame)(6);
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf((UiPointerListControl *)((int)source + 0x150));
  leaf = (word *)((int)source + 0x348);
  if (UVar1 + 1 != *(int *)(extraout_EDX + 0x54)) {
    leaf = *(word **)(*(int *)(extraout_EDX + 0x50) + -4 + (UVar1 + 1) * 4);
  }
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
             (word *)&g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
             (word *)&g_ScenarioCatalogPathScratchUtf16);
  WidePath_SetExtensionCode(0x657673,(word *)&g_ScenarioCatalogPathScratchUtf16);
  InGameUiAction1210_ResourceRegistrationHelper
            ((void *)((int)source + -0x2220),&g_ScenarioCatalogPathScratchUtf16);
  (*g_GraphicsCursorSetFrame)(0);
  (*g_FatalErrorRuntimeDispatchCf)();
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
undefined8 InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl)

{
  UiNodeBase *pUVar1;
  UiNodeBase *firstNode;
  undefined4 in_EAX;
  UiNodeVtable *pUVar2;
  uint uVar3;
  undefined4 in_EDX;
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
                          goto 
                          FrontendSaveName_UpdateActionValidity_ReturnAfterAction1210ValidityUpdate;
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
FrontendSaveName_UpdateActionValidity_ReturnAfterAction1210ValidityUpdate:
  return CONCAT44(in_EDX,in_EAX);
}
