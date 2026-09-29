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
   Row handler of the in-game save list (action INGAME_ACTION_SAVE_GAME_SELECT, g_InGameUiActionHandlersPage12
   slot 15). Shows the name edit only for the trailing "new savegame" row. A confirmed (double-clicked) existing
   save is overwritten at once; a plain selection shows the save's description (see InGameSaveGamePage_RebuildCatalog)
   and enables Delete, while the new row shows the empty description, disables Delete and enables Save only for a
   valid typed name.
*/
void InGameSaveGameList_SelectAndRefreshDetail(UiPointerListControl *catalogList)

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
  selectionResult = UiPointerList_GetSelectedIndexAndConfirmed(catalogList);
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
    /* the text box holds a text id here, not a string */
    descriptionBox->text = (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
    if (selectedIndex != lastRowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = ((ScenarioCatalogSaveRecord *)selectedRowRecord)->levelTitleTextId;
        descriptionText = TextResource_Resolve(resourceId);
        *descriptionText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
        descriptionBox->text = (uint16_t *)resourceId;
      }
      else {
        descriptionText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
        fieldText = TextResource_Resolve(((ScenarioCatalogSaveRecord *)selectedRowRecord)->levelTitleTextId);
        *fieldText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
        RichTextCommandStream_PatchPayloadBySelector(1,fieldText.text,descriptionText.text);
        fieldText = TextResource_Resolve(((ScenarioCatalogSaveRecord *)selectedRowRecord)->campaignTitleTextId);
        RichTextCommandStream_PatchPayloadBySelector(0,fieldText.text,descriptionText.text);
        descriptionBox->text = (uint16_t *)TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE;
      }
      firstNode = UiNode_GetRoot(&saveNameEntryStack->base);
      UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,firstNode);
      goto enableSave;
    }
  }
  firstNode = UiNode_GetRoot(&catalogList->base);
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,firstNode);
  if ((((UiTextEditControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveNameEdit))->
       editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
    return;
  }
enableSave:
  UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
}


/* Address: 0x0056C190.
   Delete button of the in-game save page (action INGAME_ACTION_SAVE_GAME_DELETE, g_InGameUiActionHandlersPage12
   slot 25): deletes save\<name>.sve of the selected save list row (not the trailing "new savegame" row) and
   rebuilds the page with InGameSaveGamePage_RebuildCatalog. A failed delete is reported, not fatal.
*/
void InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog(InGameSaveGamePageControlAddress32 deleteButton)

{
  uint16_t *leaf;
  int rowOrdinal;
  StatusResult deleteResult;
  ListSelectionResult selectionResult;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  selectionResult = UiPointerList_GetSelectedIndexAndConfirmed
                    ((UiPointerListControl *)
                     THANDOR_UI_SIBLING(deleteButton,InGameUiImage,saveGameDeleteButton,saveGameList));
  rowOrdinal = selectionResult.rowIndex + 1;
  if (rowOrdinal != (int)((UiPointerListControl *)
                          THANDOR_UI_SIBLING(deleteButton,InGameUiImage,saveGameDeleteButton,saveGameList))->rowCount) {
    leaf = (uint16_t *)((UiPointerListControl *)
                        THANDOR_UI_SIBLING(deleteButton,InGameUiImage,saveGameDeleteButton,saveGameList))->
           rowSlots[rowOrdinal - 1];
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save_0050daa2,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,leaf,
               (uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
    /* the fatal-error dispatch reads EAX and CF of the delete (0x0056C20B) */
    deleteResult = (*(StatusResult (*)(uint32_t,uint16_t *))g_FileSystemDelete)
                             (0,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
    FatalError_ReportIfFailed(deleteResult.valueOrError,deleteResult.failed);
    /* deleteButton - 0x760 = gameMenuSaveButton, the node RebuildCatalog expects */
    InGameSaveGamePage_RebuildCatalog((UiNodeBase *)(deleteButton - 1888));
  }
}


/* Address: 0x0056BDD0.
   Opens the in-game save page (action 0x120E, from the game menu's Save button): rebuilds g_ScenarioCatalog
   from the headers of save\*.sve plus a final "new savegame" row (text 0x2151), sorts the saves by the dword pair
   at record offset 0xF0 (descending), selects the new row and shows the page with the save-name entry. The
   description shows the selected save's title texts (alone, or patched into text 0x215E while a campaign is
   loaded), or text 0x215D for the new row.
*/
void InGameSaveGamePage_RebuildCatalog(UiNodeBase *saveMenuButton)

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
                     (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  remainingCount = enumResult.entryCount;
  if (enumResult.failed) {
    remainingCount = 0;
  }
  g_MemoryApi.free(g_ScenarioCatalog);
  g_ScenarioCatalog = NULL;
  /* per row a pointer and a 0x100-byte record: the row pointers first, then the records */
  allocResult = g_MemoryApi.alloc((remainingCount + 1) * 260);
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
        g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,256,handle);
        g_FileSystemReadExact(256,destination,handle);
        g_FileSystemClose(closeHandle);
        /* level index -> level title text, campaign index -> campaign title text */
        ((ScenarioCatalogSaveRecord *)destination)->levelTitleTextId =
             ((ScenarioCatalogSaveRecord *)destination)->levelTitleTextId + TEXT_ID_LEVEL_TITLE_BASE;
        ((ScenarioCatalogSaveRecord *)destination)->campaignTitleTextId =
             ((ScenarioCatalogSaveRecord *)destination)->campaignTitleTextId + TEXT_ID_CAMPAIGN_TITLE_BASE;
      }
      rowPointerCursor = (ScenarioCatalogHeader *)&rowPointerCursor->campaignRecordsOffset;
      destination = destination + 64;
      leaf = (uint16_t *)((int)leaf + enumResult.recordSizeBytes); /* next enumerated file name */
    }
    rowPointerCursor->levelRecordsOffset = (ScenarioCatalogByteOffset)destination;
    clearCursor = destination;
    for (clearCount = 64; clearCount != 0; clearCount--) {
      *clearCursor = 0;
      clearCursor++;
    }
    resolvedText = TextResource_Resolve(TEXT_ID_SAVE_GAME_NEW_ROW);
    RichTextCommandStream_CopyExpanded(256,(uint16_t *)destination,resolvedText.text);
    /* The action source is the game menu's Save button (in-game template +0x2550). */
    inGameUi = THANDOR_CONTAINER_OF(saveMenuButton, InGameUiImage, gameMenuSaveButton);
    saveList = (UiPointerListControl *)INGAME_UI(inGameUi, saveGameList);
    descriptionText = (UiWrappedTextControl *)INGAME_UI(inGameUi, saveGameDescriptionText);
    UiPointerList_InitializeColumnLayout(rowCount,(void **)g_ScenarioCatalog,saveList);
    UiPointerList_SortByDwordPairFieldDescending(240,saveList);
    UiPointerList_InitializeColumnLayout(rowCount + 1,(void **)g_ScenarioCatalog,saveList);
    UiPointerList_SelectColumnListIndex(rowCount,saveList);
    UiPageStack_SetActiveIndex(5,(UiPageStackControl *)INGAME_UI(inGameUi, gameWindowPageStack));
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(inGameUi, saveNameEntryStack));
    parentCursor = saveMenuButton->parent;
    firstNode = saveMenuButton;
    /* up to the root node (its parent is -1) */
    while (parentCursor != UI_NODE_NONE) {
      firstNode = firstNode->parent;
      parentCursor = firstNode->parent;
    }
    UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
    UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,firstNode);
    UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)INGAME_UI(firstNode, saveNameEdit));
    InGameSaveName_UpdateSaveActionValidity(INGAME_UI(firstNode, saveNameEdit));
    listRowCount = saveList->rowCount;
    rowSlots = saveList->rowSlots;
    selectionResult = UiPointerList_GetSelectedIndexAndConfirmed(saveList);
    selectedRecord = (ScenarioCatalogSaveRecord *)rowSlots[selectionResult.rowIndex];
    /* The description text holds a TextResourceId (labelFlags & 0x10 clear); the last row is the new save. */
    descriptionText->text = (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
    if (listRowCount - 1 != selectionResult.rowIndex) {
      if (g_FrontendLoadedCampaignAsset == 0) {
        resourceId = selectedRecord->levelTitleTextId;
        resolvedText = TextResource_Resolve(resourceId);
        *resolvedText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
        descriptionText->text = (uint16_t *)resourceId;
      }
      else {
        resolvedText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
        fieldText = TextResource_Resolve(selectedRecord->levelTitleTextId);
        *fieldText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
        RichTextCommandStream_PatchPayloadBySelector(1,fieldText.text,resolvedText.text);
        fieldText = TextResource_Resolve(selectedRecord->campaignTitleTextId);
        RichTextCommandStream_PatchPayloadBySelector(0,fieldText.text,resolvedText.text);
        descriptionText->text = (uint16_t *)TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE;
      }
    }
  }
  return;
}


/* Address: 0x0056C230.
   Save button of the in-game save page (action INGAME_ACTION_SAVE_GAME_SAVE, g_InGameUiActionHandlersPage12
   slot 16; also called by InGameSaveGameList_SelectAndRefreshDetail for a double-clicked row): writes the game to
   save\<name>.sve, named by the typed name for the trailing "new savegame" row or by the selected save's file
   name (overwriting it), reports a failed save and closes the in-game menu.
*/
void InGameSaveGame_SaveSelectedOrTypedName(UiNodeBase *saveButton)

{
  /* saveButton is the save page's saveGameSaveButton node of the in-game UI copy. */
  UiPointerListControl *saveList;
  uint32_t errorOrValue;
  uint16_t *leaf;
  uint8_t saveStatus;
  ListSelectionResult selectionResult;
  uint32_t saveCarry;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  saveList = (UiPointerListControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,saveGameList);
  selectionResult = UiPointerList_GetSelectedIndexAndConfirmed(saveList);
  errorOrValue = selectionResult.rowIndex + 1;
  /* the typed name of the trailing new-save row, else the selected row's file name */
  leaf = ((UiTextEditControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,saveNameEdit))->
         textBuffer;
  if (errorOrValue != saveList->rowCount) {
    leaf = (uint16_t *)saveList->rowSlots[errorOrValue - 1];
  }
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save_0050daa2,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,leaf,
             (uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
  /* The error check below uses the save routine's CF, not the extension helper's. */
  saveStatus = InGameSaveGame_WritePackage
                    (THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,worldView),
                     &g_ScenarioCatalogPathScratchUtf16);
  saveCarry = (uint32_t)(saveStatus & 1);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  /* The original passes EAX (kept across the cursor call with PUSHFD/PUSH EAX) as the error code. The save
     routine (0x0050ECE0) and both WidePath helpers push and pop EAX, so EAX is still the row ordinal from
     INC EAX at 0x0056C250. */
  FatalError_ReportIfFailed(errorOrValue,(saveCarry & 1) != 0);
  UiSelectableControl_SetSelected
            (0,(UiSelectableControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls
            ((UiSelectableControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,inGameMenuButton));
}


/* Address: 0x0056C2F0.
   Handler of the save-name edit (action 0x1211): enables the Save button (INGAME_ACTION_SAVE_GAME_SAVE) only
   while the typed name is valid (edit flag 0x1 set, terminated within the capacity) and contains none of the
   characters * . \ ? < > : " | / that would break the file name built from it.
*/
void InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl)

{
  UiNodeBase *parentWalk;
  UiNodeBase *firstNode;
  uint32_t remainingLength;
  uint32_t scanRemaining;
  uint32_t nameLength;
  int32_t *scanEnd;
  int32_t *charCursor;
  bool matched;

  /* up to the root node (its parent is -1) */
  parentWalk = nameControl->parent;
  firstNode = nameControl;
  while (parentWalk != UI_NODE_NONE) {
    firstNode = firstNode->parent;
    parentWalk = firstNode->parent;
  }
  if ((((UiTextEditControl *)nameControl)->editStateFlags & 1) != 0) {
    /* REPNE SCASW for the NUL; nameLength then counts the characters including the NUL. Each forbidden
       character is searched by its own REPNE SCASW pass below. */
    remainingLength = ((UiTextEditControl *)nameControl)->bufferCapacityCodeUnits;
    matched = true;
    charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
    do {
      scanEnd = charCursor;
      if (remainingLength == 0) break;
      remainingLength--;
      scanEnd = (int32_t *)((int)charCursor + 2);
      matched = (short)*charCursor == 0;
      charCursor = scanEnd;
    } while (!matched);
    if (matched) {
      nameLength = (uint32_t)-((int)(int32_t *)((UiTextEditControl *)nameControl)->textBuffer - (int)scanEnd) >> 1;
      matched = nameLength == 0;
      scanRemaining = nameLength;
      charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
      do {
        if (scanRemaining == 0) break;
        scanRemaining--;
        matched = (short)*charCursor == '*';
        charCursor = (int32_t *)((int)charCursor + 2);
      } while (!matched);
      if (!matched) {
        scanRemaining = nameLength;
        charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
        do {
          if (scanRemaining == 0) break;
          scanRemaining--;
          matched = (short)*charCursor == '.';
          charCursor = (int32_t *)((int)charCursor + 2);
        } while (!matched);
        if (!matched) {
          scanRemaining = nameLength;
          charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
          do {
            if (scanRemaining == 0) break;
            scanRemaining--;
            matched = (short)*charCursor == '\\';
            charCursor = (int32_t *)((int)charCursor + 2);
          } while (!matched);
          if (!matched) {
            scanRemaining = nameLength;
            charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
            do {
              if (scanRemaining == 0) break;
              scanRemaining--;
              matched = (short)*charCursor == '?';
              charCursor = (int32_t *)((int)charCursor + 2);
            } while (!matched);
            if (!matched) {
              scanRemaining = nameLength;
              charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
              do {
                if (scanRemaining == 0) break;
                scanRemaining--;
                matched = (short)*charCursor == '<';
                charCursor = (int32_t *)((int)charCursor + 2);
              } while (!matched);
              if (!matched) {
                scanRemaining = nameLength;
                charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
                do {
                  if (scanRemaining == 0) break;
                  scanRemaining--;
                  matched = (short)*charCursor == '>';
                  charCursor = (int32_t *)((int)charCursor + 2);
                } while (!matched);
                if (!matched) {
                  scanRemaining = nameLength;
                  charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
                  do {
                    if (scanRemaining == 0) break;
                    scanRemaining--;
                    matched = (short)*charCursor == ':';
                    charCursor = (int32_t *)((int)charCursor + 2);
                  } while (!matched);
                  if (!matched) {
                    scanRemaining = nameLength;
                    charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
                    do {
                      if (scanRemaining == 0) break;
                      scanRemaining--;
                      matched = (short)*charCursor == '"';
                      charCursor = (int32_t *)((int)charCursor + 2);
                    } while (!matched);
                    if (!matched) {
                      scanRemaining = nameLength;
                      charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
                      do {
                        if (scanRemaining == 0) break;
                        scanRemaining--;
                        matched = (short)*charCursor == '|';
                        charCursor = (int32_t *)((int)charCursor + 2);
                      } while (!matched);
                      if (!matched) {
                        charCursor = (int32_t *)((UiTextEditControl *)nameControl)->textBuffer;
                        do {
                          if (nameLength == 0) break;
                          nameLength--;
                          matched = (short)*charCursor == '/';
                          charCursor = (int32_t *)((int)charCursor + 2);
                        } while (!matched);
                        if (!matched) {
                          UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
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
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
}

