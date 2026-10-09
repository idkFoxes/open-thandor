/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/savegame_page.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/savegame_page.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* L"save" */
uint16_t g_SaveDirectoryUtf16[5] = {'s', 'a', 'v', 'e', 0};

/* <exe dir>\save\<name>.sve and the save\*.sve pattern. open-thandor: THANDOR_PATH_CAPACITY units (the original's
   0x100 units overflowed with a long game directory and a long save name). */
uint16_t g_ScenarioCatalogPathScratchUtf16[THANDOR_PATH_CAPACITY] = {};

static_assert(offsetof(InGameUiImage, saveGameDeleteButton) - offsetof(InGameUiImage, gameMenuSaveButton) == 0x760,
              "the delete handler's node is 0x760 bytes behind gameMenuSaveButton");
/* The save name of a catalog record runs from identifier up to levelTitleTextId (the header's 0x38-unit
   saveNameUtf16 field); its last code unit is forced to NUL for records read from a file. */
#define SAVE_RECORD_NAME_LAST_UNIT (offsetof(ScenarioCatalogSaveRecord, levelTitleTextId) / sizeof(uint16_t) - 1)
static_assert(SAVE_RECORD_NAME_LAST_UNIT == 0x37, "save name field of 0x38 code units");
static_assert(sizeof(ScenarioCatalogSaveRecord) == 256, "the catalog record is 0x100 bytes of the .sve");

/* Resolves a save title text id read from a .sve. open-thandor: the original resolved any id and wrote through the
   result; an id outside the compact range indexes past the page table and a missing text resolves to the sentinel
   pointer 0x33. Such an id (from a crafted, truncated or unreadable save) gives NULL here instead. */
static uint16_t *InGameSaveGame_ResolveTitleText(int32_t titleTextId)
{
  uint16_t *text;

  /* compact ids (page << 8 | index) below 0x10000 keep the page index inside the 256-entry page table */
  if ((uint32_t)titleTextId >= 0x10000) {
    Thandor_Log("save page: title text id 0x%08X of a save is out of range; no description shown",
                (uint32_t)titleTextId);
    return nullptr;
  }
  /* TextResource_TryResolve logs a missing text itself */
  return TextResource_TryResolve((TextResourceId)titleTextId,&text) ? text : nullptr;
}

/* Shows an existing save's description: its level title text alone, or, while a campaign is loaded, the level and
   campaign titles patched into the saved-game template text. The text box holds a text id, not a string. Shared by
   InGameSaveGameList_SelectAndRefreshDetail and InGameSaveGamePage_RebuildCatalog, which set the empty
   description first; a save with an invalid title id (see InGameSaveGame_ResolveTitleText) keeps it. */
static void InGameSaveGame_ShowRecordDescription(UiWrappedTextControl *descriptionBox,
                                                 const ScenarioCatalogSaveRecord *record)
{
  TextResourceId levelTitleId;
  uint16_t *templateText;
  uint16_t *levelTitleText;
  uint16_t *campaignTitleText;

  levelTitleText = InGameSaveGame_ResolveTitleText(record->levelTitleTextId);
  if (levelTitleText == nullptr) {
    return;
  }
  if (g_FrontendLoadedCampaignAsset == nullptr) {
    levelTitleId = record->levelTitleTextId;
    /* Original quirk: the colour command is written into the shared resolved title text. */
    *levelTitleText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
    descriptionBox->text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(levelTitleId)); /* a text id in the pointer field */
  }
  else {
    campaignTitleText = InGameSaveGame_ResolveTitleText(record->campaignTitleTextId);
    if (campaignTitleText == nullptr) {
      return;
    }
    templateText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
    *levelTitleText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
    RichTextCommandStream_PatchPayloadBySelector(1,levelTitleText,templateText);
    RichTextCommandStream_PatchPayloadBySelector(0,campaignTitleText,templateText);
    descriptionBox->text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE));
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
  Ptr32<void> *rowSlotArray;
  ScenarioCatalogSaveRecord *selectedRowRecord;
  UiListRowIndex selectedIndex;
  UiNodeBase *rootNode;
  UiListRowIndex lastRowIndex;
  bool selectionConfirmed;
  bool isNewSaveRow;

  InGameUiImage *image = THANDOR_CONTAINER_OF(catalogList, InGameUiImage, saveGameList);
  saveNameEntryStack = UiLayoutContainerControl_AsPageStack(&image->saveNameEntryStack);
  descriptionBox = &image->saveGameDescriptionText;
  rowSlotArray = catalogList->rowSlots;
  lastRowIndex = catalogList->rowCount - 1;
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(catalogList,&selectionConfirmed);
  selectedRowRecord = static_cast<ScenarioCatalogSaveRecord *>(rowSlotArray[selectedIndex]);
  isNewSaveRow = selectedIndex == lastRowIndex;
  /* page 1 of the stack is the name edit, shown only for the "new savegame" row */
  UiPageStack_SetActiveIndex((uint32_t)isNewSaveRow,saveNameEntryStack);
  if (selectionConfirmed) {
    if (!isNewSaveRow) {
      /* a double-clicked existing save is overwritten at once */
      InGameSaveGame_SaveSelectedOrTypedName(&image->saveGameSaveButton.selectable.base);
      return;
    }
  }
  else {
    /* the text box holds a text id here, not a string */
    descriptionBox->text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(TEXT_ID_SCENARIO_DESCRIPTION_EMPTY));
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
  if (!Any(image->saveNameEdit.editStateFlags & UI_REQUIRED_TEXT_VALUE_VALID)) {
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

  /* deleteButton is the save page's saveGameDeleteButton node of the in-game UI copy */
  InGameUiImage *image =
       THANDOR_CONTAINER_OF(reinterpret_cast<UiNodeBase *>(deleteButton), InGameUiImage, saveGameDeleteButton);

  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  /* the list's 0x64-byte prefix view */
  saveList = reinterpret_cast<UiPointerListControl *>(&image->saveGameList);
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(saveList,nullptr);
  /* the trailing "new savegame" row has no file */
  if (selectedIndex + 1 != saveList->rowCount) {
    /* the row record starts with the save's name, used as the file name */
    leaf = static_cast<uint16_t *>(saveList->rowSlots[selectedIndex]);
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,g_SaveDirectoryUtf16,
               g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,leaf,
               g_ScenarioCatalogPathScratchUtf16);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,g_ScenarioCatalogPathScratchUtf16);
    /* a failed delete is reported through the fatal-error dispatch */
    deleteError = g_FileSystemDelete(0,g_ScenarioCatalogPathScratchUtf16);
    FatalError_ReportIfFailed(deleteError,deleteError != 0);
    /* gameMenuSaveButton (0x760 bytes before deleteButton), the node RebuildCatalog expects */
    InGameSaveGamePage_RebuildCatalog(&image->gameMenuSaveButton.selectable.base);
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
  Ptr32<void> *rowSlots;
  ScenarioCatalogSaveRecord *selectedRecord;
  Ptr32<void> *rowSlot;
  uint32_t *record;
  uint8_t *enumRecord;
  ScenarioCatalogSaveRecord *saveRecord;
  void *handle;
  uint32_t rowCount;
  uint32_t remainingCount;
  int clearIndex;
  uint32_t allocError;
  uint32_t openError;
  uint32_t readError;
  uint16_t *newRowText;
  UiListRowIndex selectedIndex;

  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,g_SaveSvePatternUtf16,
             g_ExecutableDirectoryUtf16);
  rowCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
               (FileSystemEnumerationMode::FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                reinterpret_cast<uint8_t *>(g_ScenarioCatalogPathScratchUtf16));
  g_MemoryApi.free(g_ScenarioCatalog);
  g_ScenarioCatalog = nullptr;
  /* per row a pointer and a 0x100-byte record: the row pointers first, then the records */
  allocError = g_MemoryApi.alloc((uint32_t)((rowCount + 1) * (sizeof(Ptr32<void>) + 256)),reinterpret_cast<void **>(&rowSlot));
  if (allocError != 0) {
    return;
  }
  record = reinterpret_cast<uint32_t *>(rowSlot + rowCount + 1); /* behind the rowCount + 1 row pointers */
  g_ScenarioCatalog = reinterpret_cast<ScenarioCatalogHeader *>(rowSlot);
  enumRecord = g_PackageScratchBuffer; /* each enumeration record starts with the file name */
  for (remainingCount = rowCount; remainingCount != 0; remainingCount--) {
    *rowSlot = record;
    *record = 0;
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,g_SaveDirectoryUtf16,
               g_ExecutableDirectoryUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_ScenarioCatalogPathScratchUtf16,reinterpret_cast<uint16_t *>(enumRecord),
               g_ScenarioCatalogPathScratchUtf16);
    openError = g_FileSystemOpen
                      (FileSystemOpenFlags::FILESYSTEM_OPEN_EXCLUSIVE_SHARE,g_ScenarioCatalogPathScratchUtf16,&handle);
    /* the catalog record is the second 0x100 bytes of the .sve; for a save that cannot be opened only the first
       dword is cleared */
    if (openError == 0) {
      saveRecord = reinterpret_cast<ScenarioCatalogSaveRecord *>(record);
      readError = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,256,handle);
      if (readError == 0) {
        readError = g_FileSystemReadExact(sizeof(ScenarioCatalogSaveRecord),record,handle);
      }
      g_FileSystemClose(handle);
      if (readError == 0) {
        /* open-thandor: the name is used as file name leaf and list text; a crafted save may lack its NUL */
        reinterpret_cast<uint16_t *>(saveRecord)[SAVE_RECORD_NAME_LAST_UNIT] = 0;
        /* level index -> level title text, campaign index -> campaign title text (unsigned: a crafted index
           must not overflow; InGameSaveGame_ResolveTitleText rejects out-of-range ids) */
        saveRecord->levelTitleTextId =
             (int)((uint32_t)saveRecord->levelTitleTextId + TEXT_ID_LEVEL_TITLE_BASE);
        saveRecord->campaignTitleTextId =
             (int)((uint32_t)saveRecord->campaignTitleTextId + TEXT_ID_CAMPAIGN_TITLE_BASE);
      }
      else {
        /* open-thandor: the original ignored a failed read and kept a partly filled record; a truncated save is
           listed with an empty name and no description here. */
        Thandor_Log("save page: catalog record of a save could not be read (error %u)",readError);
        memset(saveRecord,0,sizeof(ScenarioCatalogSaveRecord));
        saveRecord->levelTitleTextId = (int)TEXT_RESOURCE_ID_NONE;
        saveRecord->campaignTitleTextId = (int)TEXT_RESOURCE_ID_NONE;
      }
    }
    rowSlot++;
    record += 64; /* 0x100 bytes */
    enumRecord += FILESYSTEM_ENUMERATION_RECORD_BYTES;
  }
  /* the trailing "new savegame" row: a cleared record holding text 0x2151 */
  *rowSlot = record;
  for (clearIndex = 0; clearIndex < 64; clearIndex++) {
    record[clearIndex] = 0;
  }
  newRowText = TextResource_Resolve(TEXT_ID_SAVE_GAME_NEW_ROW);
  RichTextCommandStream_CopyExpanded(256,reinterpret_cast<uint16_t *>(record),newRowText,nullptr);
  /* The action source is the game menu's Save button (InGameUiImage.gameMenuSaveButton). */
  inGameUi = THANDOR_CONTAINER_OF(saveMenuButton, InGameUiImage, gameMenuSaveButton);
  saveList = reinterpret_cast<UiPointerListControl *>(&inGameUi->saveGameList); /* the list's 0x64-byte prefix view */
  descriptionText = &inGameUi->saveGameDescriptionText;
  /* sort only the saves, then append the new row and select it */
  UiPointerList_InitializeColumnLayout(rowCount,reinterpret_cast<Ptr32<void> *>(g_ScenarioCatalog),saveList);
  UiPointerList_SortByDwordPairFieldDescending(240,saveList);
  UiPointerList_InitializeColumnLayout(rowCount + 1,reinterpret_cast<Ptr32<void> *>(g_ScenarioCatalog),saveList);
  UiPointerList_SelectColumnListIndex(rowCount,saveList);
  UiPageStack_SetActiveIndex(5,UiLayoutContainerControl_AsPageStack(&inGameUi->gameWindowPageStack));
  UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&inGameUi->saveNameEntryStack));
  /* up to the root node (its parent is -1) */
  rootNode = saveMenuButton;
  while (rootNode->parent != UI_NODE_NONE) {
    rootNode = rootNode->parent;
  }
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,rootNode);
  UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_DELETE,rootNode);
  UiTextControl_UpdateNonEmptyValidity(reinterpret_cast<UiTextEditControl *>(&InGameUi_Image(rootNode)->saveNameEdit));
  InGameSaveName_UpdateSaveActionValidity(&InGameUi_Image(rootNode)->saveNameEdit.base);
  listRowCount = saveList->rowCount;
  rowSlots = saveList->rowSlots;
  selectedIndex = UiPointerList_GetSelectedIndexAndConfirmed(saveList,nullptr);
  selectedRecord = static_cast<ScenarioCatalogSaveRecord *>(rowSlots[selectedIndex]);
  /* The description text holds a TextResourceId (labelFlags & 0x10 clear); the last row is the new save. */
  descriptionText->text = reinterpret_cast<uint16_t *>(static_cast<uintptr_t>(TEXT_ID_SCENARIO_DESCRIPTION_EMPTY));
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
  uint32_t saveError;

  InGameUiImage *image = THANDOR_CONTAINER_OF(saveButton, InGameUiImage, saveGameSaveButton);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  saveList = reinterpret_cast<UiPointerListControl *>(&image->saveGameList); /* the list's prefix view */
  rowOrdinal = UiPointerList_GetSelectedIndexAndConfirmed(saveList,nullptr) + 1;
  /* the typed name of the trailing new-save row, else the selected row's file name */
  leaf = UiTextEdit_Text(&image->saveNameEdit).data();
  if (rowOrdinal != saveList->rowCount) {
    leaf = static_cast<uint16_t *>(saveList->rowSlots[rowOrdinal - 1]);
  }
  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,g_SaveDirectoryUtf16,
             g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,leaf,
             g_ScenarioCatalogPathScratchUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,g_ScenarioCatalogPathScratchUtf16);
  saveError = InGameSaveGame_WritePackage
                    (&image->worldView.base,
                     g_ScenarioCatalogPathScratchUtf16);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  /* The original reported the selected row ordinal (selected index + 1) as the error of a failed save, which
     showed an unrelated error text; bounded here because an ordinal of 0x100 or more is no error code and the
     dialog reads it as a rich-text address. The save's own error code is reported (with the save path in the
     message, e.g. FATAL_ERROR_FILE_ACCESS_FAILED for a locked or read-only file) and the game carries on. */
  FatalError_ReportIfFailed(saveError,saveError != 0);
  UiSelectableControl_SetSelected(0,&image->inGameMenuButton.selectable);
  InGameSettingsPage_ToggleAndSynchronizeControls(&image->inGameMenuButton.selectable);
}


/* Handler of the save-name edit (action 0x1211): enables the Save button (INGAME_ACTION_SAVE_GAME_SAVE) only
   while the typed name is valid (UI_TEXT_EDIT_VALUE_VALID set, terminated within the capacity) and contains none
   of the characters * . \ ? < > : " | / that would break the file name built from it. The original scanned the
   name once per forbidden character with dword reads at code unit steps (reading one unit past the NUL); this is
   the same check per code unit.
*/
void InGameSaveName_UpdateSaveActionValidity(UiNodeBase *nameControl)

{
  static const uint16_t forbiddenUnits[] = {'*', '.', '\\', '?', '<', '>', ':', '"', '|', '/'};
  UiTextEditControl *nameEdit;
  UiNodeBase *firstNode;
  const uint16_t *name;
  uint32_t capacity;
  uint32_t nameLength;
  uint32_t unitIndex;
  uint32_t forbiddenIndex;
  bool nameValid;

  nameEdit = reinterpret_cast<UiTextEditControl *>(nameControl); /* the save name edit */
  /* up to the root node (its parent is -1) */
  firstNode = nameControl;
  while (firstNode->parent != UI_NODE_NONE) {
    firstNode = firstNode->parent;
  }
  nameValid = false;
  if (Any(nameEdit->editStateFlags & UI_TEXT_EDIT_VALUE_VALID)) {
    name = UiTextEdit_Text(nameEdit).data();
    capacity = nameEdit->bufferCapacityCodeUnits;
    nameLength = 0;
    while (nameLength < capacity && name[nameLength] != 0) {
      nameLength++;
    }
    /* a name without its NUL within the capacity is invalid */
    nameValid = nameLength < capacity;
    for (unitIndex = 0; nameValid && unitIndex < nameLength; unitIndex++) {
      for (forbiddenIndex = 0; forbiddenIndex < sizeof(forbiddenUnits) / sizeof(forbiddenUnits[0]);
           forbiddenIndex++) {
        if (name[unitIndex] == forbiddenUnits[forbiddenIndex]) {
          nameValid = false;
          break;
        }
      }
    }
  }
  if (nameValid) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_SAVE_GAME_SAVE,firstNode);
  }
}

