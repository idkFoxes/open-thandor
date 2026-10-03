/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/savegame.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/savegame.h>
#include <thandor/thandor.h>

/* Module data. */

/* L"save" */
uint16_t g_SaveDirectoryUtf16[5] = {'s', 'a', 'v', 'e', 0};

uint16_t g_ScenarioCatalogPathScratchUtf16[256] = {0};

/* Implementation ownership: gameplay/session/savegame. */

/* Shows an existing save's description: its level title text alone, or, while a campaign is loaded, the level and
   campaign titles patched into the saved-game template text. The text box holds a text id, not a string. Shared by
   InGameSaveGameList_SelectAndRefreshDetail and InGameSaveGamePage_RebuildCatalog. */
static void InGameSaveGame_ShowRecordDescription(UiWrappedTextControl *descriptionBox,
                                                 const ScenarioCatalogSaveRecord *record)
{
  TextResourceId levelTitleId;
  uint16_t *templateText;
  uint16_t *fieldText;

  if (g_FrontendLoadedCampaignAsset == 0) {
    levelTitleId = record->levelTitleTextId;
    fieldText = TextResource_Resolve(levelTitleId);
    *fieldText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
    descriptionBox->text = (uint16_t *)levelTitleId;
  }
  else {
    templateText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
    fieldText = TextResource_Resolve(record->levelTitleTextId);
    *fieldText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
    RichTextCommandStream_PatchPayloadBySelector(1,fieldText,templateText);
    fieldText = TextResource_Resolve(record->campaignTitleTextId);
    RichTextCommandStream_PatchPayloadBySelector(0,fieldText,templateText);
    descriptionBox->text = (uint16_t *)TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE;
  }
}


/* Row handler of the in-game save list (action INGAME_ACTION_SAVE_GAME_SELECT, g_InGameUiActionHandlersPage12
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
  ScenarioCatalogSaveRecord *selectedRowRecord;
  UiListRowIndex selectedIndex;
  UiNodeBase *rootNode;
  UiListRowIndex lastRowIndex;
  Bool8 selectionConfirmed;
  Bool8 isNewSaveRow;

  saveNameEntryStack =
       (UiPageStackControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveNameEntryStack);
  descriptionBox =
       (UiWrappedTextControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveGameDescriptionText);
  rowSlotArray = catalogList->rowSlots;
  lastRowIndex = catalogList->rowCount - 1;
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(catalogList,&selectionConfirmed);
  selectedRowRecord = (ScenarioCatalogSaveRecord *)rowSlotArray[selectedIndex];
  isNewSaveRow = selectedIndex == lastRowIndex;
  /* page 1 of the stack is the name edit, shown only for the "new savegame" row */
  UiPageStack_SetActiveIndex((uint32_t)isNewSaveRow,saveNameEntryStack);
  if (selectionConfirmed) {
    if (!isNewSaveRow) {
      /* a double-clicked existing save is overwritten at once */
      InGameSaveGame_SaveSelectedOrTypedName
                (THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveGameSaveButton));
      return;
    }
  }
  else {
    /* the text box holds a text id here, not a string */
    descriptionBox->text = (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
    if (!isNewSaveRow) {
      InGameSaveGame_ShowRecordDescription(descriptionBox,selectedRowRecord);
      rootNode = UiNode_GetRoot(&saveNameEntryStack->base);
      UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,rootNode);
      UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,rootNode);
      return;
    }
  }
  /* the "new savegame" row: no Delete, Save only for a valid typed name */
  rootNode = UiNode_GetRoot(&catalogList->base);
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,rootNode);
  if ((((UiTextEditControl *)THANDOR_UI_SIBLING(catalogList,InGameUiImage,saveGameList,saveNameEdit))->
       editStateFlags & UI_TEXT_EDIT_VALUE_VALID) == 0) {
    UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,rootNode);
  }
  else {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,rootNode);
  }
}


/* Delete button of the in-game save page (action INGAME_ACTION_SAVE_GAME_DELETE, g_InGameUiActionHandlersPage12
   slot 25): deletes save\<name>.sve of the selected save list row (not the trailing "new savegame" row) and
   rebuilds the page with InGameSaveGamePage_RebuildCatalog. A failed delete is reported, not fatal.
*/
void InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog(InGameSaveGamePageControlAddress32 deleteButton)

{
  UiPointerListControl *saveList;
  uint16_t *leaf;
  UiListRowIndex selectedIndex;
  uint32_t deleteError;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  saveList =
       (UiPointerListControl *)THANDOR_UI_SIBLING(deleteButton,InGameUiImage,saveGameDeleteButton,saveGameList);
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(saveList,NULL);
  /* the trailing "new savegame" row has no file */
  if (selectedIndex + 1 != saveList->rowCount) {
    /* the row record starts with the save's name, used as the file name */
    leaf = (uint16_t *)saveList->rowSlots[selectedIndex];
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveDirectoryUtf16,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,leaf,
               g_ScenarioCatalogPathScratchUtf16);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,g_ScenarioCatalogPathScratchUtf16);
    /* a failed delete is reported through the fatal-error dispatch */
    deleteError = g_FileSystemDelete(0,g_ScenarioCatalogPathScratchUtf16);
    FatalError_ReportIfFailed(deleteError,deleteError != 0);
    /* deleteButton - 0x760 = gameMenuSaveButton, the node RebuildCatalog expects */
    InGameSaveGamePage_RebuildCatalog((UiNodeBase *)(deleteButton - 1888));
  }
}


/* Opens the in-game save page (action 0x120E, from the game menu's Save button): rebuilds g_ScenarioCatalog
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
  UiNodeBase *rootNode;
  UiListRowCount listRowCount;
  void **rowSlots;
  ScenarioCatalogSaveRecord *selectedRecord;
  ScenarioCatalogByteOffset *rowSlot;
  uint32_t *record;
  uint8_t *enumRecord;
  ScenarioCatalogSaveRecord *saveRecord;
  void *handle;
  uint32_t rowCount;
  uint32_t remainingCount;
  int clearIndex;
  uint32_t allocError;
  uint32_t openError;
  uint16_t *newRowText;
  UiListRowIndex selectedIndex;

  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveSvePatternUtf16,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  rowCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
               (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                (uint8_t *)g_ScenarioCatalogPathScratchUtf16);
  g_MemoryApi.free(g_ScenarioCatalog);
  g_ScenarioCatalog = NULL;
  /* per row a pointer and a 0x100-byte record: the row pointers first, then the records */
  allocError = g_MemoryApi.alloc((rowCount + 1) * 260,(void **)&rowSlot);
  if (allocError != 0) {
    return;
  }
  record = (uint32_t *)(rowSlot + rowCount + 1); /* behind the rowCount + 1 row pointers */
  g_ScenarioCatalog = (ScenarioCatalogHeader *)rowSlot;
  enumRecord = g_PackageScratchBuffer; /* each enumeration record starts with the file name */
  for (remainingCount = rowCount; remainingCount != 0; remainingCount--) {
    *rowSlot = (ScenarioCatalogByteOffset)record;
    *record = 0;
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveDirectoryUtf16,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)enumRecord,
               g_ScenarioCatalogPathScratchUtf16);
    openError = g_FileSystemOpen
                      (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,g_ScenarioCatalogPathScratchUtf16,&handle);
    /* the catalog record is the second 0x100 bytes of the .sve; for a save that cannot be opened only the first
       dword is cleared */
    if (openError == 0) {
      g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,256,handle);
      g_FileSystemReadExact(256,record,handle);
      g_FileSystemClose(handle);
      /* level index -> level title text, campaign index -> campaign title text */
      saveRecord = (ScenarioCatalogSaveRecord *)record;
      saveRecord->levelTitleTextId = saveRecord->levelTitleTextId + TEXT_ID_LEVEL_TITLE_BASE;
      saveRecord->campaignTitleTextId = saveRecord->campaignTitleTextId + TEXT_ID_CAMPAIGN_TITLE_BASE;
    }
    rowSlot++;
    record += 64; /* 0x100 bytes */
    enumRecord += FILESYSTEM_ENUMERATION_RECORD_BYTES;
  }
  /* the trailing "new savegame" row: a cleared record holding text 0x2151 */
  *rowSlot = (ScenarioCatalogByteOffset)record;
  for (clearIndex = 0; clearIndex < 64; clearIndex++) {
    record[clearIndex] = 0;
  }
  newRowText = TextResource_Resolve(TEXT_ID_SAVE_GAME_NEW_ROW);
  RichTextCommandStream_CopyExpanded(256,(uint16_t *)record,newRowText,NULL);
  /* The action source is the game menu's Save button (InGameUiImage.gameMenuSaveButton). */
  inGameUi = THANDOR_CONTAINER_OF(saveMenuButton, InGameUiImage, gameMenuSaveButton);
  saveList = (UiPointerListControl *)INGAME_UI(inGameUi, saveGameList);
  descriptionText = (UiWrappedTextControl *)INGAME_UI(inGameUi, saveGameDescriptionText);
  /* sort only the saves, then append the new row and select it */
  UiPointerList_InitializeColumnLayout(rowCount,(void **)g_ScenarioCatalog,saveList);
  UiPointerList_SortByDwordPairFieldDescending(240,saveList);
  UiPointerList_InitializeColumnLayout(rowCount + 1,(void **)g_ScenarioCatalog,saveList);
  UiPointerList_SelectColumnListIndex(rowCount,saveList);
  UiPageStack_SetActiveIndex(5,(UiPageStackControl *)INGAME_UI(inGameUi, gameWindowPageStack));
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(inGameUi, saveNameEntryStack));
  /* up to the root node (its parent is -1) */
  rootNode = saveMenuButton;
  while (rootNode->parent != UI_NODE_NONE) {
    rootNode = rootNode->parent;
  }
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,rootNode);
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,rootNode);
  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)INGAME_UI(rootNode, saveNameEdit));
  InGameSaveName_UpdateSaveActionValidity(INGAME_UI(rootNode, saveNameEdit));
  listRowCount = saveList->rowCount;
  rowSlots = saveList->rowSlots;
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(saveList,NULL);
  selectedRecord = (ScenarioCatalogSaveRecord *)rowSlots[selectedIndex];
  /* The description text holds a TextResourceId (labelFlags & 0x10 clear); the last row is the new save. */
  descriptionText->text = (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (listRowCount - 1 != selectedIndex) {
    InGameSaveGame_ShowRecordDescription(descriptionText,selectedRecord);
  }
}


/* Save button of the in-game save page (action INGAME_ACTION_SAVE_GAME_SAVE, g_InGameUiActionHandlersPage12
   slot 16; also called by InGameSaveGameList_SelectAndRefreshDetail for a double-clicked row): writes the game to
   save\<name>.sve, named by the typed name for the trailing "new savegame" row or by the selected save's file
   name (overwriting it), reports a failed save and closes the in-game menu.
*/
void InGameSaveGame_SaveSelectedOrTypedName(UiNodeBase *saveButton)

{
  /* saveButton is the save page's saveGameSaveButton node of the in-game UI copy. */
  UiPointerListControl *saveList;
  uint32_t rowOrdinal;
  uint16_t *leaf;
  Bool8 saveFailed;

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  saveList = (UiPointerListControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,saveGameList);
  rowOrdinal = UiPointerList_GetSelectedIndexAndConfirmed(saveList,NULL) + 1;
  /* the typed name of the trailing new-save row, else the selected row's file name */
  leaf = ((UiTextEditControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,saveNameEdit))->
         textBuffer;
  if (rowOrdinal != saveList->rowCount) {
    leaf = (uint16_t *)saveList->rowSlots[rowOrdinal - 1];
  }
  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveDirectoryUtf16,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,leaf,
             g_ScenarioCatalogPathScratchUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,g_ScenarioCatalogPathScratchUtf16);
  saveFailed = InGameSaveGame_WritePackage
                    (THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,worldView),
                     g_ScenarioCatalogPathScratchUtf16);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  /* Original quirk: the error code reported for a failed save is the selected row ordinal (selected index + 1),
     not an error from the save routine; the original never replaces that value before the report. */
  FatalError_ReportIfFailed(rowOrdinal,saveFailed);
  UiSelectableControl_SetSelected
            (0,(UiSelectableControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls
            ((UiSelectableControl *)THANDOR_UI_SIBLING(saveButton,InGameUiImage,saveGameSaveButton,inGameMenuButton));
}


/* Handler of the save-name edit (action 0x1211): enables the Save button (INGAME_ACTION_SAVE_GAME_SAVE) only
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
  Bool8 matched;

  /* up to the root node (its parent is -1) */
  parentWalk = nameControl->parent;
  firstNode = nameControl;
  while (parentWalk != UI_NODE_NONE) {
    firstNode = firstNode->parent;
    parentWalk = firstNode->parent;
  }
  if ((((UiTextEditControl *)nameControl)->editStateFlags & 1) != 0) {
    /* scan for the NUL; nameLength then counts the characters including the NUL. Each forbidden
       character is searched by its own scan pass below. */
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

