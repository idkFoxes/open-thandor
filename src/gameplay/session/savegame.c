/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/savegame.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/savegame.h>
#include <thandor/thandor.h>

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
  void **rowSlotArray;
  void *selectedRowRecord;
  TextResourceId resourceId;
  UiListRowIndex selectedIndex;
  UiNodeBase *firstNode;
  UiListRowIndex lastRowIndex;
  UiListRowIndexEaxCf5 selectionResult;
  TextResourceResolveEaxCf5 descriptionText;
  TextResourceResolveEaxCf5 fieldText;
  
  rowSlotArray = catalogList->rowSlots;
  lastRowIndex = catalogList->rowCount - 1;
  selectionResult = UiPointerList_GetSelectedIndexVariantBCf(catalogList);
  selectedIndex = selectionResult.rowIndex;
  selectedRowRecord = rowSlotArray[selectedIndex];
  if (selectionResult.carry) {
    UiPageStack_SetActiveIndex((uint)(selectedIndex == lastRowIndex),&THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->detailPageStack);
    if (selectedIndex != lastRowIndex) {
      InGameSaveGame_SaveSelectedOrTypedName(THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList));
      return;
    }
  }
  else {
    UiPageStack_SetActiveIndex((uint)(selectedIndex == lastRowIndex),&THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->detailPageStack);
    THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->activeDetailTextResourceId = 0x215d;
    if (selectedIndex != lastRowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = *(TextResourceId *)((int)selectedRowRecord + 0x70);
        descriptionText = TextResource_Resolve(resourceId);
        *descriptionText.eax = 0x8000;
        THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->activeDetailTextResourceId = resourceId;
      }
      else {
        descriptionText = TextResource_Resolve(0x215e);
        fieldText = TextResource_Resolve(*(TextResourceId *)((int)selectedRowRecord + 0x70));
        *fieldText.eax = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,fieldText.eax,descriptionText.eax);
        fieldText = TextResource_Resolve(*(TextResourceId *)((int)selectedRowRecord + 0x90));
        RichTextCommandStream_PatchPayloadBySelector(0,fieldText.eax,descriptionText.eax);
        THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->activeDetailTextResourceId = 0x215e;
      }
      firstNode = UiNode_GetRoot(&(THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->detailPageStack).base);
      UiNodeList_UnsuppressActionId(0x1219,firstNode);
      goto InGameUiAction120F_Handler_UnsuppressAction1210AndReturn;
    }
  }
  firstNode = UiNode_GetRoot(&catalogList->base);
  UiNodeList_SuppressActionId(0x1219,firstNode);
  if (((THANDOR_CONTAINER_OF(catalogList, InGameCatalogDetailPage32C, catalogList)->action1210Control).nodeFlags & 1) == 0) {
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
  int rowOrdinal;
  dword errorOrValue;
  bool carryIn;
  UiListRowIndexEaxCf5 selectionResult;
  
  (*g_GraphicsCursorSetFrame)(6);
  selectionResult = UiPointerList_GetSelectedIndexVariantBCf
                    ((UiPointerListControl *)(saveGamePageControl + 0xf0));
  rowOrdinal = selectionResult.rowIndex + 1;
  if (rowOrdinal != *(int *)(saveGamePageControl + 0x144)) {
    leaf = *(word **)(*(int *)(saveGamePageControl + 0x140) + -4 + rowOrdinal * 4);
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
  UiNodeBase *parentOrSelectedRow;
  UiRootNode *firstNode;
  UiNodeVtable *listRowCount;
  TextResourceId resourceId;
  ScenarioCatalogHeader *rowPointerCursor;
  void *handle;
  sdword *controlPtr;
  dword remainingCount;
  int clearCount;
  word *leaf;
  ScenarioCatalogByteOffset *destination;
  ScenarioCatalogByteOffset *clearCursor;
  ArenaAllocEaxCf5 allocResult;
  FileSystemOpenEaxCf5 openResult;
  TextResourceResolveEaxCf5 resolvedText;
  UiListRowIndexEaxCf5 selectionResult;
  TextResourceResolveEaxCf5 fieldText;
  FileSystemEnumerationEaxEcxCf9 enumResult;
  void *closeHandle;
  dword rowCount;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save___sve_0050d9c8,
             (word *)&g_ExecutableDirectoryUtf16);
  enumResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                     (FILESYSTEM_ENUMERATE_FILES,0xffffffff,0x800000,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  remainingCount = enumResult.entryCount;
  if (enumResult.carry) {
    remainingCount = 0;
  }
  (*g_MemoryApi.free)(g_ScenarioCatalog);
  g_ScenarioCatalog = (ScenarioCatalogHeader *)0x0;
  allocResult = (*g_MemoryApi.alloc)((remainingCount + 1) * 0x104);
  rowPointerCursor = (ScenarioCatalogHeader *)allocResult.eax;
  if (!allocResult.carry) {
    destination = &rowPointerCursor->campaignRecordsOffset + remainingCount;
    g_ScenarioCatalog = rowPointerCursor;
    rowCount = remainingCount;
    leaf = (word *)g_PackageScratchBuffer;
    for (; remainingCount != 0; remainingCount = remainingCount - 1) {
      rowPointerCursor->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
      *destination = 0;
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,(word *)u_save_0050daa2,
                 (word *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_ScenarioCatalogPathScratchUtf16,leaf,
                 (word *)&g_ScenarioCatalogPathScratchUtf16);
      openResult = (*g_FileSystemOpenCf)
                        (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,(word *)&g_ScenarioCatalogPathScratchUtf16);
      handle = (void *)openResult.eax;
      if (!openResult.carry) {
        closeHandle = handle;
        (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0x100,handle);
        (*g_FileSystemReadExactCf)(0x100,destination,handle);
        (*g_FileSystemClose)(closeHandle);
        destination[0x1c] = destination[0x1c] + 0x2230;
        destination[0x24] = destination[0x24] + 0x2220;
      }
      rowPointerCursor = (ScenarioCatalogHeader *)&rowPointerCursor->campaignRecordsOffset;
      destination = destination + 0x40;
      leaf = (word *)((int)leaf + enumResult.recordSizeBytes);
    }
    rowPointerCursor->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
    clearCursor = destination;
    for (clearCount = 0x40; clearCount != 0; clearCount = clearCount + -1) {
      *clearCursor = 0;
      clearCursor = clearCursor + 1;
    }
    resolvedText = TextResource_Resolve(0x2151);
    RichTextCommandStream_CopyExpandedCf(0x100,(word *)destination,resolvedText.eax);
    controlPtr = &savePageRoot[0x18].base.left;
    UiPointerList_InitializeColumnLayout
              (rowCount,(void **)g_ScenarioCatalog,(UiPointerListControl *)controlPtr);
    UiPointerList_SortByDwordPairFieldDescending(0xf0,(UiPointerListControl *)controlPtr);
    UiPointerList_InitializeColumnLayout
              (rowCount + 1,(void **)g_ScenarioCatalog,(UiPointerListControl *)controlPtr);
    UiPointerList_SelectIndexVariantB(rowCount,(UiPointerListControl *)controlPtr);
    UiPageStack_SetActiveIndex(5,(UiPageStackControl *)&savePageRoot[-0x4b].base.nodeFlags);
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&savePageRoot[0x1b].base.bottomAnchorQ31);
    parentOrSelectedRow = (savePageRoot->base).parent;
    firstNode = savePageRoot;
    while (parentOrSelectedRow != (UiNodeBase *)0xffffffff) {
      firstNode = (UiRootNode *)(firstNode->base).parent;
      parentOrSelectedRow = (firstNode->base).parent;
    }
    UiNodeList_SuppressActionId(0x1210,&firstNode->base);
    UiNodeList_SuppressActionId(0x1219,&firstNode->base);
    controlPtr = &firstNode[0x89].base.top;
    UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)controlPtr);
    InGameSaveName_UpdateSaveActionValidity((UiNodeBase *)controlPtr);
    listRowCount = savePageRoot[0x19].base.vtable;
    parentOrSelectedRow = savePageRoot[0x19].base.parent;
    selectionResult = UiPointerList_GetSelectedIndexVariantBCf
                       ((UiPointerListControl *)&savePageRoot[0x18].base.left);
    parentOrSelectedRow = (&parentOrSelectedRow->nextSibling)[(int)selectionResult.rowIndex];
    savePageRoot[0x1b].base.topAnchorQ31 = 0x215d;
    if ((undefined1 *)((int)&listRowCount[-1].pointerWheel + 3U) != (undefined1 *)selectionResult.rowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = parentOrSelectedRow[1].topOffset;
        resolvedText = TextResource_Resolve(resourceId);
        *resolvedText.eax = 0x8000;
        savePageRoot[0x1b].base.topAnchorQ31 = resourceId;
      }
      else {
        resolvedText = TextResource_Resolve(0x215e);
        fieldText = TextResource_Resolve(parentOrSelectedRow[1].topOffset);
        *fieldText.eax = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,fieldText.eax,resolvedText.eax);
        fieldText = TextResource_Resolve(parentOrSelectedRow[1].layoutHeight);
        RichTextCommandStream_PatchPayloadBySelector(0,fieldText.eax,resolvedText.eax);
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
  byte saveStatus;
  UiListRowIndexEaxCf5 selectionResult;
  uint saveCarry;
  
  (*g_GraphicsCursorSetFrame)(6);
  selectionResult = UiPointerList_GetSelectedIndexVariantBCf((UiPointerListControl *)((int)source + 0x150));
  errorOrValue = selectionResult.rowIndex + 1;
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
  WidePath_SetExtensionCode(0x657673,(word *)&g_ScenarioCatalogPathScratchUtf16);
  /* The error check below uses the save routine's CF, not the extension helper's. */
  saveStatus = InGameUiAction1210_ResourceRegistrationHelper
                    ((void *)((int)source + -0x2220),&g_ScenarioCatalogPathScratchUtf16);
  saveCarry = (uint)(saveStatus & 1);
  (*g_GraphicsCursorSetFrame)(0);
  (*g_FatalErrorRuntimeDispatchCf)(errorOrValue,(saveCarry & 1) != 0);
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
  UiNodeBase *parentWalk;
  UiNodeBase *firstNode;
  UiNodeVtable *remainingLength;
  uint scanRemaining;
  uint nameLength;
  sdword *scanEnd;
  sdword *charCursor;
  bool matched;
  
  parentWalk = nameControl->parent;
  firstNode = nameControl;
  while (parentWalk != (UiNodeBase *)0xffffffff) {
    firstNode = firstNode->parent;
    parentWalk = firstNode->parent;
  }
  if (((uint)nameControl[1].nextSibling & 1) != 0) {
    remainingLength = nameControl[1].vtable;
    matched = true;
    charCursor = &nameControl[1].leftOffset;
    do {
      scanEnd = charCursor;
      if (remainingLength == (UiNodeVtable *)0x0) break;
      remainingLength = (UiNodeVtable *)((int)&remainingLength[-1].pointerWheel + 3);
      scanEnd = (sdword *)((int)charCursor + 2);
      matched = (short)*charCursor == 0;
      charCursor = scanEnd;
    } while (!matched);
    if (matched) {
      nameLength = (uint)-((int)&nameControl[1].leftOffset - (int)scanEnd) >> 1;
      matched = nameLength == 0;
      scanRemaining = nameLength;
      charCursor = &nameControl[1].leftOffset;
      do {
        if (scanRemaining == 0) break;
        scanRemaining = scanRemaining - 1;
        matched = (short)*charCursor == 0x2a;
        charCursor = (sdword *)((int)charCursor + 2);
      } while (!matched);
      if (!matched) {
        scanRemaining = nameLength;
        charCursor = &nameControl[1].leftOffset;
        do {
          if (scanRemaining == 0) break;
          scanRemaining = scanRemaining - 1;
          matched = (short)*charCursor == 0x2e;
          charCursor = (sdword *)((int)charCursor + 2);
        } while (!matched);
        if (!matched) {
          scanRemaining = nameLength;
          charCursor = &nameControl[1].leftOffset;
          do {
            if (scanRemaining == 0) break;
            scanRemaining = scanRemaining - 1;
            matched = (short)*charCursor == 0x5c;
            charCursor = (sdword *)((int)charCursor + 2);
          } while (!matched);
          if (!matched) {
            scanRemaining = nameLength;
            charCursor = &nameControl[1].leftOffset;
            do {
              if (scanRemaining == 0) break;
              scanRemaining = scanRemaining - 1;
              matched = (short)*charCursor == 0x3f;
              charCursor = (sdword *)((int)charCursor + 2);
            } while (!matched);
            if (!matched) {
              scanRemaining = nameLength;
              charCursor = &nameControl[1].leftOffset;
              do {
                if (scanRemaining == 0) break;
                scanRemaining = scanRemaining - 1;
                matched = (short)*charCursor == 0x3c;
                charCursor = (sdword *)((int)charCursor + 2);
              } while (!matched);
              if (!matched) {
                scanRemaining = nameLength;
                charCursor = &nameControl[1].leftOffset;
                do {
                  if (scanRemaining == 0) break;
                  scanRemaining = scanRemaining - 1;
                  matched = (short)*charCursor == 0x3e;
                  charCursor = (sdword *)((int)charCursor + 2);
                } while (!matched);
                if (!matched) {
                  scanRemaining = nameLength;
                  charCursor = &nameControl[1].leftOffset;
                  do {
                    if (scanRemaining == 0) break;
                    scanRemaining = scanRemaining - 1;
                    matched = (short)*charCursor == 0x3a;
                    charCursor = (sdword *)((int)charCursor + 2);
                  } while (!matched);
                  if (!matched) {
                    scanRemaining = nameLength;
                    charCursor = &nameControl[1].leftOffset;
                    do {
                      if (scanRemaining == 0) break;
                      scanRemaining = scanRemaining - 1;
                      matched = (short)*charCursor == 0x22;
                      charCursor = (sdword *)((int)charCursor + 2);
                    } while (!matched);
                    if (!matched) {
                      scanRemaining = nameLength;
                      charCursor = &nameControl[1].leftOffset;
                      do {
                        if (scanRemaining == 0) break;
                        scanRemaining = scanRemaining - 1;
                        matched = (short)*charCursor == 0x7c;
                        charCursor = (sdword *)((int)charCursor + 2);
                      } while (!matched);
                      if (!matched) {
                        charCursor = &nameControl[1].leftOffset;
                        do {
                          if (nameLength == 0) break;
                          nameLength = nameLength - 1;
                          matched = (short)*charCursor == 0x2f;
                          charCursor = (sdword *)((int)charCursor + 2);
                        } while (!matched);
                        if (!matched) {
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

