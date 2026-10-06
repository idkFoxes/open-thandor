/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/task_assignment.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/task_assignment.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* Node offsets in FrontendUiImage of the seven faction rows' controls: row number labels, mode buttons,
   colour buttons, play checkboxes and participant labels. */
FrontendTaskAssignmentControlOffsetTables g_FrontendTaskAssignmentControlOffsets = {
    .assignmentControls = {.offsets = {
        offsetof(FrontendUiImage,factionRow1NumberLabel),
        offsetof(FrontendUiImage,factionRow2NumberLabel),
        offsetof(FrontendUiImage,factionRow3NumberLabel),
        offsetof(FrontendUiImage,factionRow4NumberLabel),
        offsetof(FrontendUiImage,factionRow5NumberLabel),
        offsetof(FrontendUiImage,factionRow6NumberLabel),
        offsetof(FrontendUiImage,factionRow7NumberLabel)
    }},
    .playerControls = {.offsets = {
        offsetof(FrontendUiImage,factionRow1ModeButton),
        offsetof(FrontendUiImage,factionRow2ModeButton),
        offsetof(FrontendUiImage,factionRow3ModeButton),
        offsetof(FrontendUiImage,factionRow4ModeButton),
        offsetof(FrontendUiImage,factionRow5ModeButton),
        offsetof(FrontendUiImage,factionRow6ModeButton),
        offsetof(FrontendUiImage,factionRow7ModeButton)
    }},
    .factionControls = {.offsets = {
        offsetof(FrontendUiImage,factionRow1ColourButton),
        offsetof(FrontendUiImage,factionRow2ColourButton),
        offsetof(FrontendUiImage,factionRow3ColourButton),
        offsetof(FrontendUiImage,factionRow4ColourButton),
        offsetof(FrontendUiImage,factionRow5ColourButton),
        offsetof(FrontendUiImage,factionRow6ColourButton),
        offsetof(FrontendUiImage,factionRow7ColourButton)
    }},
    .selectionRows = {.offsets = {
        offsetof(FrontendUiImage,factionRow1PlayCheckbox),
        offsetof(FrontendUiImage,factionRow2PlayCheckbox),
        offsetof(FrontendUiImage,factionRow3PlayCheckbox),
        offsetof(FrontendUiImage,factionRow4PlayCheckbox),
        offsetof(FrontendUiImage,factionRow5PlayCheckbox),
        offsetof(FrontendUiImage,factionRow6PlayCheckbox),
        offsetof(FrontendUiImage,factionRow7PlayCheckbox)
    }},
    .statusRows = {.offsets = {
        offsetof(FrontendUiImage,factionRow1ParticipantsLabel),
        offsetof(FrontendUiImage,factionRow2ParticipantsLabel),
        offsetof(FrontendUiImage,factionRow3ParticipantsLabel),
        offsetof(FrontendUiImage,factionRow4ParticipantsLabel),
        offsetof(FrontendUiImage,factionRow5ParticipantsLabel),
        offsetof(FrontendUiImage,factionRow6ParticipantsLabel),
        offsetof(FrontendUiImage,factionRow7ParticipantsLabel)
    }}};

/* Faction assignment indices of player records are 1..7 (row index + 1). The original indexes the row tables
   with index - 1 (and the roster texts with the index) without a check; bounded here because the records are
   filled from peers' commands in a network game. An index outside 1..7 is skipped (logged once). */
static bool FrontendTaskAssignmentPage_IsValidFactionIndex(FrontendFactionAssignmentIndex factionIndex)

{
  static int s_loggedInvalidFactionIndex;

  if (factionIndex >= 1 && factionIndex <= 7) {
    return true;
  }
  if (s_loggedInvalidFactionIndex == 0) {
    s_loggedInvalidFactionIndex = 1;
    Thandor_Log("faction setup: player faction index %d out of range, skipped",factionIndex);
  }
  return false;
}

/* Opens the "Choose faction" page (FRONTEND_PAGE_ACTION_TASK_ASSIGNMENT_PAGE) for the loaded level. The seven
   roster rows are set up from the level: assignable factions get an active mode button ("Computer"), the other
   active factions a visible but inactive row, unused rows are hidden and their faction slot cleared. The players
   are then spread round-robin over the assignable factions (mode "Player"), the local player's play checkbox is
   ticked, and the page's buttons are arranged for a local game, a network host or a client.
   The row controls are reached through g_FrontendTaskAssignmentControlOffsets, node offsets from the frontend
   root: mode and colour buttons (UiFramedTextButtonControl), play checkboxes (UiTextButtonControl), row number
   and participant labels (UiSingleLineTextControl).
*/

void FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView *frontendRootPage)

{
  FrontendUiImage *ui = FrontendUi_Image(frontendRootPage);
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t rowControlOffset;
  UiNodeVtable *rootVtable;
  int localPlayerRuntimeId;
  FrontendLoadedLevelAsset *loadedLevel;
  uint32_t activeFactionCount;
  uint32_t assignableFactionCount;
  uint32_t activeFactionsLeft;
  uint32_t assignableFactionsLeft;
  uint32_t localPlayerRow;
  uint32_t assignmentIndex;
  uint32_t rowCursor;
  UiTextResourceId *rowTextId;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint16_t *titleText;
  uint16_t *templateText;
  FrontendPlayerRuntimeBlockCount remainingPlayerRecords;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_FACTION_SETUP,&frontendRootPage->primaryPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &ui->menuRoomModelView.contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  activeFactionCount = g_FrontendLoadedLevelAsset->worldSettings.activeFactionCount;
  assignableFactionCount = g_FrontendLoadedLevelAsset->worldSettings.assignableFactionCount;
  /* The original trusts the level's counts (valid levels have 1 <= assignable <= active <= 7); bounded here
     because an assignable count of 0 wraps the do-while counter below, an active count below the assignable one
     underflows activeFactionsLeft, and more than 7 factions run past the seven-entry row tables. Such counts
     are clamped into that range so the page still opens. */
  if (assignableFactionCount == 0 || activeFactionCount > 7 || assignableFactionCount > activeFactionCount) {
    Thandor_Log("faction setup: invalid level counts (assignable %u, active %u), bounded",
                (unsigned)assignableFactionCount,(unsigned)activeFactionCount);
    if (activeFactionCount > 7) {
      activeFactionCount = 7;
    }
    if (assignableFactionCount == 0) {
      assignableFactionCount = 1;
    }
    if (assignableFactionCount > activeFactionCount) {
      assignableFactionCount = activeFactionCount != 0 ? activeFactionCount : 1;
      activeFactionCount = assignableFactionCount;
    }
  }
  activeFactionsLeft = activeFactionCount;
  assignableFactionsLeft = assignableFactionCount;
  /* Rows 1..assignable count: factions a player may take; mode button active, caption "Computer". */
  rowCursor = 0;
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowTextId = &FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_COMPUTER;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~(FRONTEND_CONTROL_INACTIVE | UI_SELECTABLE_SELECTED_OR_CHECKED);
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor++;
    activeFactionsLeft--;
    assignableFactionsLeft--;
  } while (assignableFactionsLeft != 0);
  /* Further active factions (computer only): mode button active, the rest of the row hidden and inactive. */
  for (; activeFactionsLeft != 0; activeFactionsLeft--) {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowTextId = &FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_COMPUTER;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor++;
  }
  /* Unused rows up to 7: everything hidden and inactive, caption "No-one", faction slot cleared. */
  for (; rowCursor < 7; rowCursor++) {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowTextId = &FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_NOBODY;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(rowControlOffset)->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->labelFlags |= UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiSingleLineTextControl>(rowControlOffset)->labelFlags |= UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] = 0;
  }
  /* Colour buttons of rows 7..1 show the faction name of the level's player slot; none is selected. */
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor - 1];
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->textResourceId =
         ((LevelPlayerSlotRecord *)((uint8_t *)loadedLevel->playerSlots +
                                    g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowCursor - 1]))->aiClassOrMode +
         TEXT_ID_FACTION_NAME_BASE + rowCursor;
    FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    rowCursor--;
  } while (rowCursor != 0);
  /* Players round-robin over the assignable factions (as FrontendPlayerRuntime_InitializeFactionAssignments);
     their rows switch to "Player". localPlayerRow ends as the local player's zero-based row. */
  localPlayerRuntimeId = g_LocalPlayerRuntimeId;
  assignmentIndex = 1;
  localPlayerRow = 0;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingPlayerRecords = g_FrontendPlayerRuntimeBlockCount;
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[assignmentIndex - 1];
    playerRecord->factionAssignment.factionAssignmentIndex = assignmentIndex;
    rowTextId = &FrontendUi_Image(frontendRootPage)->NodeAt<UiFramedTextButtonControl>(rowControlOffset)->textResourceId;
    playerRecord->factionAssignment.readyOrWaitState = 0;
    playerRecord->factionAssignment.consensusValue = 0;
    *rowTextId = TEXT_ID_FACTION_MODE_PLAYER;
    if (localPlayerRuntimeId == playerRecord->playerRuntimeId) {
      localPlayerRow = assignmentIndex - 1;
    }
    assignmentIndex++;
    playerRecord++;
    if (assignableFactionCount < assignmentIndex) {
      assignmentIndex = assignmentIndex - assignableFactionCount;
    }
    remainingPlayerRecords--;
  } while (remainingPlayerRecords != 0);
  /* tick and show the local player's play checkbox */
  FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[localPlayerRow])
       ->selectable.stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
  FrontendUi_Image(frontendRootPage)->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[localPlayerRow])
       ->selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls((UiRootNode *)frontendRootPage);
  rootVtable = frontendRootPage->rootNode.vtable;
  /* frontendRootPage is the frontend root (FrontendUiImage). */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    /* local game: roster left/right offsets 96 (no participant column); Back and Next, no Finish */
    ui->factionRosterTable.base.leftOffset = 96;
    ui->factionRosterTable.base.rightOffset = 96;
    ui->factionSetupFinishButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    ui->factionSetupNextButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ui->factionSetupBackButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ui->factionSetupBackButton.selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    /* network game: full-width roster, Finish instead of Next; a client cannot go back */
    ui->factionRosterTable.base.leftOffset = 0;
    ui->factionRosterTable.base.rightOffset = 0;
    ui->factionSetupFinishButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ui->factionSetupFinishButton.selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    ui->factionSetupNextButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      ui->factionSetupNextButton.selectable.stateFlags |=
           FRONTEND_CONTROL_INACTIVE;
      ui->factionSetupBackButton.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
      ui->factionSetupBackButton.selectable.stateFlags |=
           FRONTEND_CONTROL_INACTIVE;
    }
    else {
      ui->factionSetupNextButton.selectable.stateFlags &=
           ~FRONTEND_CONTROL_INACTIVE;
      ui->factionSetupBackButton.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
      ui->factionSetupBackButton.selectable.stateFlags &=
           ~FRONTEND_CONTROL_INACTIVE;
    }
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  rootVtable->layout(&frontendRootPage->rootNode);
  titleText = TextResource_Resolve(loadedLevel->header.titleTextResourceIndex + TEXT_ID_LEVEL_TITLE_BASE);
  *titleText = FRONTEND_TEXT_STYLE_NORMAL;
  templateText = TextResource_Resolve(TEXT_ID_FACTION_SETUP_TASK_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,titleText,templateText);
}

/* Faction setup page: shows the row control at controlOffset with the page's nodeFlags set/clear masks
   (hidden once the local player has confirmed) and makes it active. */
static void FrontendTaskAssignmentPage_ShowRowControl(UiRootNode *taskAssignmentRoot, uint32_t controlOffset,
                                                      uint32_t controlSetMask, uint32_t controlClearMask)
{
  UiNodeFlags *controlFlags; /* a nodeFlags or stateFlags field (both int) */

  controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(controlOffset + offsetof(UiNodeBase,nodeFlags));
  *controlFlags = *controlFlags | controlSetMask;
  controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(controlOffset + offsetof(UiNodeBase,nodeFlags));
  *controlFlags = *controlFlags & controlClearMask;
  controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSelectableControl>(controlOffset)->stateFlags;
  *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
}

/* Faction setup page: hides the row control at controlOffset and marks it inactive. */
static void FrontendTaskAssignmentPage_DisableRowControl(UiRootNode *taskAssignmentRoot, uint32_t controlOffset)
{
  UiNodeFlags *controlFlags; /* a nodeFlags or stateFlags field (both int) */

  controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(controlOffset + offsetof(UiNodeBase,nodeFlags));
  *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSelectableControl>(controlOffset)->stateFlags;
  *controlFlags = *controlFlags | FRONTEND_CONTROL_INACTIVE;
}

/* Refreshes the faction setup page (FRONTEND_PAGE_FACTION_SETUP) from the player records: the task description
   of the local player's faction, which of the seven faction rows can be chosen (only factions that are active
   in the level; a player who has confirmed is locked), the mode caption per row (player / nobody / computer),
   and in a network game the roster text naming the players of every faction plus the lock of the rows once the
   Finish button is pressed. When another player with a lower readyOrWaitState has the local player's faction,
   the local player's faction control is hidden.
*/
void
FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(UiRootNode *taskAssignmentRoot)

{
  UiNodeFlags *controlFlags; /* a nodeFlags or stateFlags field (both int) */
  uint32_t controlOffset;
  FrontendFactionAssignmentIndex rosterFactionIndex;
  FrontendFactionAssignmentIndex localFactionIndex;
  FrontendPlayerRuntimeBlockCount remainingSearchCount;
  int row;
  int remainingDwords;
  int nameCodeUnits;
  int nameUnitIndex;
  int localPlayerFaction;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerNameUtf16 *playerName;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeRecord *otherPlayerRecord;
  FrontendTaskAssignmentFactionTextRow *textRowCursor;
  uint16_t *rosterTextCursor;
  uint16_t *rosterRowStart;
  uint16_t *rosterRowEnd;
  uint32_t controlClearMask;
  uint32_t controlSetMask;
  
  /* the rows get these nodeFlags masks: shown, or hidden once the local player has confirmed */
  controlSetMask = 0;
  controlClearMask = ~UI_NODE_SUPPRESSED;
  localFactionIndex = g_FrontendPlayerRuntimeBlocks->factionAssignment.factionAssignmentIndex;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingSearchCount = g_FrontendPlayerRuntimeBlockCount;
  /* network game: find the local player's record (the first record is tested without checking the count) */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
        localFactionIndex = playerRecord->factionAssignment.factionAssignmentIndex;
        if (playerRecord->factionAssignment.consensusValue != 0) {
          controlSetMask = UI_NODE_SUPPRESSED;
          controlClearMask = 0xffffffff;
        }
        break;
      }
      playerRecord++;
      remainingSearchCount--;
    } while (remainingSearchCount != 0);
  }
  /* 0x230010 + 0x10 * level title + faction selects the task description of the local player's faction */
  FrontendUi_Image(taskAssignmentRoot)->taskDescriptionText.text =
       (uint16_t *)(uintptr_t)
       (localFactionIndex + TEXT_ID_LEVEL_DESCRIPTION_BASE + g_FrontendLoadedLevelAsset->header.titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE);
  /* The control tables hold node offsets in the page (FrontendUiImage::NodeAt): nodeFlags (UI_NODE_SUPPRESSED), the
     selectable stateFlags (UI_SELECTABLE_SELECTED_OR_CHECKED, FRONTEND_CONTROL_INACTIVE) and the mode buttons'
     caption text id. Rows 7..1 (entry row - 1). Some nodeFlags accesses add offsetof(UiNodeBase,nodeFlags) to
     the offset first (NodeAt<UiNodeFlags>): that order is the one of the original code there. */
  for (row = 7; row != 0; row--) {
    if ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSelectableControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1])->stateFlags &
         UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      FrontendTaskAssignmentPage_ShowRowControl(taskAssignmentRoot,
                                                g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1],
                                                controlSetMask, controlClearMask);
    }
    else if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[row] != FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      FrontendTaskAssignmentPage_DisableRowControl(taskAssignmentRoot,
                                                   g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1]);
    }
    else if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
             ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeBase>(g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1])->nodeFlags &
               UI_NODE_SUPPRESSED) != 0)) {
      /* not a client and the row's assignment control is suppressed */
      controlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1];
      if ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSingleLineTextControl>(controlOffset)->labelFlags &
           UI_LABEL_HIDE_WHILE_SUPPRESSED) != 0) {
        FrontendTaskAssignmentPage_DisableRowControl(taskAssignmentRoot,
                                                     g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1]);
      }
      else {
        FrontendTaskAssignmentPage_ShowRowControl(taskAssignmentRoot,
                                                  g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1],
                                                  controlSetMask, controlClearMask);
      }
    }
    else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
      /* network game (a client, or the row's assignment control is shown): hidden but active */
      controlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1];
      controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeBase>(controlOffset)->nodeFlags;
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSelectableControl>(controlOffset)->stateFlags;
      *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
    }
    else {
      FrontendTaskAssignmentPage_ShowRowControl(taskAssignmentRoot,
                                                g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1],
                                                controlSetMask, controlClearMask);
    }
    controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[row - 1] + offsetof(UiNodeBase,nodeFlags));
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
  /* mode captions: nobody, computer for active factions, player where a player record has the faction */
  for (row = 7; row != 0; row--) {
    controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[row - 1];
    FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiFramedTextButtonControl>(controlOffset)->textResourceId = TEXT_ID_FACTION_MODE_NOBODY;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[row] == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiFramedTextButtonControl>(controlOffset)->textResourceId = TEXT_ID_FACTION_MODE_COMPUTER;
      if ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeBase>(g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1])->nodeFlags &
           UI_NODE_SUPPRESSED) == 0) {
        FrontendTaskAssignmentPage_ShowRowControl(taskAssignmentRoot,
                                                  g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1],
                                                  controlSetMask, controlClearMask);
      }
      else {
        FrontendTaskAssignmentPage_DisableRowControl(taskAssignmentRoot,
                                                     g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1]);
      }
    }
    else {
      FrontendTaskAssignmentPage_DisableRowControl(taskAssignmentRoot,
                                                   g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1]);
    }
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (FrontendTaskAssignmentPage_IsValidFactionIndex(playerRecord->factionAssignment.factionAssignmentIndex)) {
      FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiFramedTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[playerRecord->factionAssignment.factionAssignmentIndex - 1])->textResourceId = TEXT_ID_FACTION_MODE_PLAYER;
    }
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
  /* Not a client: update the mode buttons, then hide those of factions taken by players. The original ORs
     the last player's faction index and ANDs the (low 32 bits of the) address of g_FrontendLoadedLevelAsset
     here (both left over in registers from the loop above) where the page's set/clear masks were meant (as
     FrontendTaskAssignmentPage_ShowRowControl applies them: shown, or kept hidden once the local player has
     confirmed); the masks are applied here because the original makes the flags depend on the heap layout. */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    for (row = 7; row != 0; row--) {
      if ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSingleLineTextControl>(g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1])->labelFlags &
           UI_LABEL_HIDE_WHILE_SUPPRESSED) == 0) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[row - 1];
        controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(controlOffset + offsetof(UiNodeBase,nodeFlags));
        *controlFlags = *controlFlags | controlSetMask;
        controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(controlOffset + offsetof(UiNodeBase,nodeFlags));
        *controlFlags = *controlFlags & controlClearMask;
      }
    }
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if (FrontendTaskAssignmentPage_IsValidFactionIndex(playerRecord->factionAssignment.factionAssignmentIndex)) {
        controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeBase>(g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[playerRecord->factionAssignment.factionAssignmentIndex - 1])->nodeFlags;
        *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      }
      remainingPlayers--;
      playerRecord++;
    } while (remainingPlayers != 0);
  }
  /* clear the roster texts (0x8C dwords = seven 0x50-byte rows) */
  textRowCursor = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows + 1;
  for (remainingDwords = 140; remainingDwords != 0; remainingDwords--) {
    textRowCursor->textUtf16[0] = 0;
    textRowCursor->textUtf16[1] = 0;
    textRowCursor = (FrontendTaskAssignmentFactionTextRow *)(textRowCursor->textUtf16 + 2); /* one dword */
  }
  /* the participant column exists only in a network game */
  FrontendUi_Image(taskAssignmentRoot)->rosterParticipantHeader.base.nodeFlags |= UI_NODE_SUPPRESSED;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    return;
  }
  FrontendUi_Image(taskAssignmentRoot)->rosterParticipantHeader.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerName = &playerRecord->playerName;
  /* Append every player's name to the roster row of its faction; playerName walks the player records.
     A name is written as the style code (0x8001 highlighted while consensusValue is 0, else 0x8000) and up to
     20 code units of the name (copied past its terminator); then, highlighted, 0x8000 over the last copied
     unit and a 0, else a 0 over the last copied unit. A further name follows the row's first terminator after
     ", ". The original copies 20 units, but all units left in the row when no more than 21 are left (21 of
     them read past the name), and writes the closing codes after them: with two names of 16 or more code units on one faction it writes up
     to 2 code units past the 40-unit row, and a third name then finds no terminator and copies about 2^32
     units. Bounded here because the names come from peers: the copied units are cut so that the closing
     codes stay in the row, and a name without room for one unit is left out (without ", "). Rows that fit
     get the same text as in the original. */
  do {
    rosterFactionIndex = FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.factionAssignmentIndex;
    if (FrontendTaskAssignmentPage_IsValidFactionIndex(rosterFactionIndex)) {
      rosterRowStart = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows
                       [rosterFactionIndex].textUtf16;
      rosterRowEnd = rosterRowStart + 40;
      rosterTextCursor = rosterRowStart;
      if (rosterTextCursor[0] != 0 || rosterTextCursor[1] != 0) {
        /* The faction row already names a player: find its end; ", " follows there. */
        while (rosterTextCursor < rosterRowEnd && *rosterTextCursor != 0) {
          rosterTextCursor++;
        }
        if (2 <= rosterRowEnd - rosterTextCursor) {
          rosterTextCursor += 2;
        }
        else {
          rosterTextCursor = rosterRowEnd; /* no room: the name is left out below */
        }
      }
      /* code units left for the name between the style code and the closing code(s) */
      nameCodeUnits = (int)(rosterRowEnd - rosterTextCursor) -
                      (FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.consensusValue == 0 ? 2 : 1);
      if (20 < nameCodeUnits) {
        nameCodeUnits = 20; /* at most the 20 code units of a player name */
      }
      if (0 < nameCodeUnits) {
        if (rosterTextCursor != rosterRowStart) {
          rosterTextCursor[-2] = ',';
          rosterTextCursor[-1] = ' ';
        }
        /* consensusValue 0: the name between rich-text codes 0x8001 and 0x8000, else after 0x8000 */
        if (FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.consensusValue == 0) {
          rosterTextCursor[0] = FRONTEND_TEXT_STYLE_HIGHLIGHTED;
        }
        else {
          rosterTextCursor[0] = FRONTEND_TEXT_STYLE_NORMAL;
        }
        for (nameUnitIndex = 0; nameUnitIndex < nameCodeUnits; nameUnitIndex++) {
          rosterTextCursor[nameUnitIndex + 1] = playerName->textUtf16[nameUnitIndex];
        }
        rosterTextCursor += nameCodeUnits;
        if (FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.consensusValue == 0) {
          rosterTextCursor[0] = FRONTEND_TEXT_STYLE_NORMAL;
          rosterTextCursor[1] = 0;
        }
        else {
          rosterTextCursor[0] = 0;
        }
      }
    }
    remainingPlayers--;
    /* next player record */
    playerName = playerName + sizeof(FrontendPlayerRuntimeRecord) / sizeof(FrontendPlayerNameUtf16);
  } while (remainingPlayers != 0);
  /* once the local player has pressed Finish, the faction rows are hidden */
  if ((FrontendUi_Image(taskAssignmentRoot)->factionSetupFinishButton.selectable.stateFlags &
       UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    for (row = 7; row != 0; row--) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1];
      if ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSelectableControl>(controlOffset)->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
        controlFlags = FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeFlags>(controlOffset + offsetof(UiNodeBase,nodeFlags));
        *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      }
    }
  }
  else {
    for (row = 7; row != 0; row--) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1];
      if ((FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiSelectableControl>(controlOffset)->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
        controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeBase>(controlOffset)->nodeFlags;
        *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      }
    }
  }
  /* Another player with the local player's faction and a lower readyOrWaitState: hide the faction.
     Original quirk: the search for that player keeps counting down remainingPlayers of the outer loop. */
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
      localPlayerFaction = playerRecord->factionAssignment.factionAssignmentIndex;
      if (!FrontendTaskAssignmentPage_IsValidFactionIndex(localPlayerFaction)) {
        return;
      }
      otherPlayerRecord = g_FrontendPlayerRuntimeBlocks;
      while ((localPlayerFaction != otherPlayerRecord->factionAssignment.factionAssignmentIndex ||
             ((int)(playerRecord->factionAssignment).readyOrWaitState <=
              (int)(otherPlayerRecord->factionAssignment).readyOrWaitState))) {
        otherPlayerRecord++;
        remainingPlayers--;
        if (remainingPlayers == 0) {
          return;
        }
      }
      controlFlags = &FrontendUi_Image(taskAssignmentRoot)->NodeAt<UiNodeBase>(g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[localPlayerFaction - 1])->nodeFlags;
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      return;
    }
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
}
