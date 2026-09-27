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
   Cross-module calls: UiPointerList_GetSelectedIndexVariantB [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], UiNode_GetRoot [ui/core/runtime], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameList_SelectAndRefreshDetail(UiPointerListControl *catalogList)

{
  /* catalogList is the save page's saveGameList node of the in-game UI copy. */
  UiPageStackControl *saveNameEntryStack;
  UiWrappedTextControl *descriptionBox;
  void **rowSlotArray;
  void *selectedRowRecord;
  TextResourceId resourceId;
  UiListRowIndex selectedIndex;
  UiNodeBase *firstNode;
  UiListRowIndex lastRowIndex;
  ListSelectionResult selectionResult;
  TextResolveResult descriptionText;
  TextResolveResult fieldText;
  
  saveNameEntryStack =
       (UiPageStackControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveNameEntryStack);
  descriptionBox =
       (UiWrappedTextControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveGameDescriptionText);
  rowSlotArray = catalogList->rowSlots;
  lastRowIndex = catalogList->rowCount - 1;
  selectionResult = UiPointerList_GetSelectedIndexVariantB(catalogList);
  selectedIndex = selectionResult.rowIndex;
  selectedRowRecord = rowSlotArray[selectedIndex];
  if (selectionResult.confirmed) {
    UiPageStack_SetActiveIndex((uint32_t)(selectedIndex == lastRowIndex),saveNameEntryStack);
    if (selectedIndex != lastRowIndex) {
      InGameSaveGame_SaveSelectedOrTypedName
                (THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveGameSaveButton));
      return;
    }
  }
  else {
    UiPageStack_SetActiveIndex((uint32_t)(selectedIndex == lastRowIndex),saveNameEntryStack);
    descriptionBox->text = (uint16_t *)0x215d;
    if (selectedIndex != lastRowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = *(TextResourceId *)((int)selectedRowRecord + 0x70);
        descriptionText = TextResource_Resolve(resourceId);
        *descriptionText.text = 0x8000;
        descriptionBox->text = (uint16_t *)resourceId;
      }
      else {
        descriptionText = TextResource_Resolve(0x215e);
        fieldText = TextResource_Resolve(*(TextResourceId *)((int)selectedRowRecord + 0x70));
        *fieldText.text = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,fieldText.text,descriptionText.text);
        fieldText = TextResource_Resolve(*(TextResourceId *)((int)selectedRowRecord + 0x90));
        RichTextCommandStream_PatchPayloadBySelector(0,fieldText.text,descriptionText.text);
        descriptionBox->text = (uint16_t *)0x215e;
      }
      firstNode = UiNode_GetRoot(&saveNameEntryStack->base);
      UiNodeList_UnsuppressActionId(0x1219,firstNode);
      goto InGameUiAction120F_Handler_UnsuppressAction1210AndReturn;
    }
  }
  firstNode = UiNode_GetRoot(&catalogList->base);
  UiNodeList_SuppressActionId(0x1219,firstNode);
  if ((((UiTextEditControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveNameEdit))->
       editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
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
   Cross-module calls: UiPointerList_GetSelectedIndexVariantB [ui/controls/lists],
   WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode [core/text/path].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog
          (InGameSaveGamePageControlAddress32 saveGamePageControl)

{
  uint16_t *leaf;
  int rowOrdinal;
  uint32_t errorOrValue;
  bool carryIn;
  ListSelectionResult selectionResult;
  
  g_GraphicsCursorSetFrame(6);
  selectionResult = UiPointerList_GetSelectedIndexVariantB
                    ((UiPointerListControl *)(saveGamePageControl + 0xf0));
  rowOrdinal = selectionResult.rowIndex + 1;
  if (rowOrdinal != *(int *)(saveGamePageControl + 0x144)) {
    leaf = *(uint16_t **)(*(int *)(saveGamePageControl + 0x140) + -4 + rowOrdinal * 4);
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save_0050daa2,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,leaf,
               (uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
    carryIn = WidePath_SetExtensionCode(0x657673,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
    errorOrValue = g_FileSystemDelete(0,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
    FatalError_ReportIfFailed(errorOrValue,carryIn);
    InGameSaveGamePage_RebuildCatalog((UiNodeBase *)(saveGamePageControl + -0x760));
  }
  return;
}


/* Address: 0x0056BDD0.
   Opens the in-game save page (action 0x120E, from the game menu's Save button): rebuilds g_ScenarioCatalog
   from the headers of save\*.sve plus a final "new savegame" row (text 0x2151), sorts the saves by the dword pair
   at record offset 0xF0 (descending), selects the new row and shows the page with the save-name entry. The
   description shows the selected save's title texts (alone, or patched into text 0x215E while a campaign is
   loaded), or text 0x215D for the new row.
*/
void __thandor_void_preserve_eax_ecx_edx InGameSaveGamePage_RebuildCatalog(UiNodeBase *saveMenuButton)

{
  InGameUiImage *inGameUi;
  UiPointerListControl *saveList;
  UiWrappedTextControl *descriptionText;
  UiNodeBase *parentCursor;
  UiNodeBase *firstNode;
  UiListRowCount listRowCount;
  void **rowSlots;
  ScenarioCatalogSaveRecord *selectedRecord;
  TextResourceId resourceId;
  ScenarioCatalogHeader *rowPointerCursor;
  void *handle;
  uint32_t remainingCount;
  int clearCount;
  uint16_t *leaf;
  ScenarioCatalogByteOffset *destination;
  ScenarioCatalogByteOffset *clearCursor;
  ArenaAllocResult allocResult;
  FileSystemOpenResult openResult;
  TextResolveResult resolvedText;
  ListSelectionResult selectionResult;
  TextResolveResult fieldText;
  DirectoryEnumerationResult enumResult;
  void *closeHandle;
  uint32_t rowCount;
  
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save___sve_0050d9c8,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  enumResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                     (FILESYSTEM_ENUMERATE_FILES,0xffffffff,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  remainingCount = enumResult.entryCount;
  if (enumResult.failed) {
    remainingCount = 0;
  }
  g_MemoryApi.free(g_ScenarioCatalog);
  g_ScenarioCatalog = NULL;
  /* per row a pointer and a 0x100-byte record: the row pointers first, then the records */
  allocResult = g_MemoryApi.alloc((remainingCount + 1) * 0x104);
  rowPointerCursor = (ScenarioCatalogHeader *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    destination = &rowPointerCursor->campaignRecordsOffset + remainingCount; /* behind count + 1 pointers */
    g_ScenarioCatalog = rowPointerCursor;
    rowCount = remainingCount;
    leaf = (uint16_t *)g_PackageScratchBuffer;
    for (; remainingCount != 0; remainingCount = remainingCount - 1) {
      rowPointerCursor->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
      *destination = 0;
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save_0050daa2,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,leaf,
                 (uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
      openResult = g_FileSystemOpen
                        (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
      handle = (void *)openResult.handleOrError;
      /* the catalog record is the second 0x100 bytes of the .sve; a save that cannot be opened stays empty */
      if (!openResult.failed) {
        closeHandle = handle;
        g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0x100,handle);
        g_FileSystemReadExact(0x100,destination,handle);
        g_FileSystemClose(closeHandle);
        destination[0x1c] = destination[0x1c] + 0x2230; /* localizedStringId70: level index -> level title text */
        destination[0x24] = destination[0x24] + 0x2220; /* optionalLocalizedStringId90 -> text id */
      }
      rowPointerCursor = (ScenarioCatalogHeader *)&rowPointerCursor->campaignRecordsOffset;
      destination = destination + 0x40;
      leaf = (uint16_t *)((int)leaf + enumResult.recordSizeBytes); /* next enumerated file name */
    }
    rowPointerCursor->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
    clearCursor = destination;
    for (clearCount = 0x40; clearCount != 0; clearCount--) {
      *clearCursor = 0;
      clearCursor++;
    }
    resolvedText = TextResource_Resolve(0x2151);
    RichTextCommandStream_CopyExpanded(0x100,(uint16_t *)destination,resolvedText.text);
    /* The action source is the game menu's Save button (in-game template +0x2550). */
    inGameUi = THANDOR_CONTAINER_OF(saveMenuButton, InGameUiImage, gameMenuSaveButton);
    saveList = (UiPointerListControl *)INGAME_UI(inGameUi, saveGameList);
    descriptionText = (UiWrappedTextControl *)INGAME_UI(inGameUi, saveGameDescriptionText);
    UiPointerList_InitializeColumnLayout(rowCount,(void **)g_ScenarioCatalog,saveList);
    UiPointerList_SortByDwordPairFieldDescending(0xf0,saveList);
    UiPointerList_InitializeColumnLayout(rowCount + 1,(void **)g_ScenarioCatalog,saveList);
    UiPointerList_SelectIndexVariantB(rowCount,saveList);
    UiPageStack_SetActiveIndex(5,(UiPageStackControl *)INGAME_UI(inGameUi, gameWindowPageStack));
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(inGameUi, saveNameEntryStack));
    parentCursor = saveMenuButton->parent;
    firstNode = saveMenuButton;
    /* up to the root node (its parent is -1) */
    while (parentCursor != (UiNodeBase *)0xffffffff) {
      firstNode = firstNode->parent;
      parentCursor = firstNode->parent;
    }
    UiNodeList_SuppressActionId(0x1210,firstNode);
    UiNodeList_SuppressActionId(0x1219,firstNode);
    UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)INGAME_UI(firstNode, saveNameEdit));
    InGameSaveName_UpdateSaveActionValidity(INGAME_UI(firstNode, saveNameEdit));
    listRowCount = saveList->rowCount;
    rowSlots = saveList->rowSlots;
    selectionResult = UiPointerList_GetSelectedIndexVariantB(saveList);
    selectedRecord = (ScenarioCatalogSaveRecord *)rowSlots[selectionResult.rowIndex];
    /* The description text holds a TextResourceId (labelFlags & 0x10 clear); the last row is the new save. */
    descriptionText->text = (uint16_t *)0x215d;
    if (listRowCount - 1 != selectionResult.rowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = selectedRecord->localizedStringId70;
        resolvedText = TextResource_Resolve(resourceId);
        *resolvedText.text = 0x8000;
        descriptionText->text = (uint16_t *)resourceId;
      }
      else {
        resolvedText = TextResource_Resolve(0x215e);
        fieldText = TextResource_Resolve(selectedRecord->localizedStringId70);
        *fieldText.text = 0x8000;
        RichTextCommandStream_PatchPayloadBySelector(1,fieldText.text,resolvedText.text);
        fieldText = TextResource_Resolve(selectedRecord->optionalLocalizedStringId90);
        RichTextCommandStream_PatchPayloadBySelector(0,fieldText.text,resolvedText.text);
        descriptionText->text = (uint16_t *)0x215e;
      }
    }
  }
  return;
}


/* Address: 0x0056C230.
   Ownership: gameplay/session/savegame.
   Purpose: Recovered action-table target INGAME_PAGE12[16] (0x1210).
   Cross-module calls: UiPointerList_GetSelectedIndexVariantB [ui/controls/lists],
   WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode [core/text/path],
   InGameUiAction1210_ResourceRegistrationHelper [ui/ingame/runtime], UiSelectableControl_SetSelected
   [ui/controls/lists], InGameSettingsPage_ToggleAndSynchronizeControls [ui/ingame/settings].
*/
void __thandor_void_preserve_eax_ecx_edx InGameSaveGame_SaveSelectedOrTypedName(UiNodeBase *saveButton)

{
  /* saveButton is the save page's saveGameSaveButton node of the in-game UI copy. */
  UiPointerListControl *saveList;
  uint32_t errorOrValue;
  uint16_t *leaf;
  uint8_t saveStatus;
  ListSelectionResult selectionResult;
  uint32_t saveCarry;
  
  g_GraphicsCursorSetFrame(6);
  saveList = (UiPointerListControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,saveGameList);
  selectionResult = UiPointerList_GetSelectedIndexVariantB(saveList);
  errorOrValue = selectionResult.rowIndex + 1;
  /* the typed name of the trailing new-save row, else the selected row's file name */
  leaf = ((UiTextEditControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,saveNameEdit))->
         textPrefix6C;
  if (errorOrValue != saveList->rowCount) {
    leaf = (uint16_t *)saveList->rowSlots[errorOrValue - 1];
  }
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save_0050daa2,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,leaf,
             (uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
  WidePath_SetExtensionCode(0x657673,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
  /* The error check below uses the save routine's CF, not the extension helper's. */
  saveStatus = InGameUiAction1210_ResourceRegistrationHelper
                    (THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,worldView),
                     &g_ScenarioCatalogPathScratchUtf16);
  saveCarry = (uint32_t)(saveStatus & 1);
  g_GraphicsCursorSetFrame(0);
  FatalError_ReportIfFailed(errorOrValue,(saveCarry & 1) != 0);
  UiSelectableControl_SetSelected
            (0,(UiSelectableControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls
            ((UiSelectableControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,inGameMenuButton));
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
  uint32_t remainingLength;
  uint32_t scanRemaining;
  uint32_t nameLength;
  int32_t *scanEnd;
  int32_t *charCursor;
  bool matched;
  
  parentWalk = nameControl->parent;
  firstNode = nameControl;
  while (parentWalk != (UiNodeBase *)0xffffffff) {
    firstNode = firstNode->parent;
    parentWalk = firstNode->parent;
  }
  if ((((UiTextEditControl *)nameControl)->editStateFlags & 1) != 0) {
    remainingLength = ((UiTextEditControl *)nameControl)->valueOrCapacity58;
    matched = true;
    charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
    do {
      scanEnd = charCursor;
      if (remainingLength == 0) break;
      remainingLength = remainingLength - 1;
      scanEnd = (int32_t *)((int)charCursor + 2);
      matched = (short)*charCursor == 0;
      charCursor = scanEnd;
    } while (!matched);
    if (matched) {
      nameLength = (uint32_t)-((int)(int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C - (int)scanEnd) >> 1;
      matched = nameLength == 0;
      scanRemaining = nameLength;
      charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
      do {
        if (scanRemaining == 0) break;
        scanRemaining = scanRemaining - 1;
        matched = (short)*charCursor == 0x2a;
        charCursor = (int32_t *)((int)charCursor + 2);
      } while (!matched);
      if (!matched) {
        scanRemaining = nameLength;
        charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
        do {
          if (scanRemaining == 0) break;
          scanRemaining = scanRemaining - 1;
          matched = (short)*charCursor == 0x2e;
          charCursor = (int32_t *)((int)charCursor + 2);
        } while (!matched);
        if (!matched) {
          scanRemaining = nameLength;
          charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
          do {
            if (scanRemaining == 0) break;
            scanRemaining = scanRemaining - 1;
            matched = (short)*charCursor == 0x5c;
            charCursor = (int32_t *)((int)charCursor + 2);
          } while (!matched);
          if (!matched) {
            scanRemaining = nameLength;
            charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
            do {
              if (scanRemaining == 0) break;
              scanRemaining = scanRemaining - 1;
              matched = (short)*charCursor == 0x3f;
              charCursor = (int32_t *)((int)charCursor + 2);
            } while (!matched);
            if (!matched) {
              scanRemaining = nameLength;
              charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
              do {
                if (scanRemaining == 0) break;
                scanRemaining = scanRemaining - 1;
                matched = (short)*charCursor == 0x3c;
                charCursor = (int32_t *)((int)charCursor + 2);
              } while (!matched);
              if (!matched) {
                scanRemaining = nameLength;
                charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
                do {
                  if (scanRemaining == 0) break;
                  scanRemaining = scanRemaining - 1;
                  matched = (short)*charCursor == 0x3e;
                  charCursor = (int32_t *)((int)charCursor + 2);
                } while (!matched);
                if (!matched) {
                  scanRemaining = nameLength;
                  charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
                  do {
                    if (scanRemaining == 0) break;
                    scanRemaining = scanRemaining - 1;
                    matched = (short)*charCursor == 0x3a;
                    charCursor = (int32_t *)((int)charCursor + 2);
                  } while (!matched);
                  if (!matched) {
                    scanRemaining = nameLength;
                    charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
                    do {
                      if (scanRemaining == 0) break;
                      scanRemaining = scanRemaining - 1;
                      matched = (short)*charCursor == 0x22;
                      charCursor = (int32_t *)((int)charCursor + 2);
                    } while (!matched);
                    if (!matched) {
                      scanRemaining = nameLength;
                      charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
                      do {
                        if (scanRemaining == 0) break;
                        scanRemaining = scanRemaining - 1;
                        matched = (short)*charCursor == 0x7c;
                        charCursor = (int32_t *)((int)charCursor + 2);
                      } while (!matched);
                      if (!matched) {
                        charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textPrefix6C;
                        do {
                          if (nameLength == 0) break;
                          nameLength = nameLength - 1;
                          matched = (short)*charCursor == 0x2f;
                          charCursor = (int32_t *)((int)charCursor + 2);
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

