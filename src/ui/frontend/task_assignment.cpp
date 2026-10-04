/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/task_assignment.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/task_assignment.h>
#include <thandor/thandor.h>

/* Module data. */

FrontendTaskAssignmentControlOffsetTables g_FrontendTaskAssignmentControlOffsets = {
    .assignmentControls = {.offsets = {3264, 3356, 3448, 3540, 3632, 3724, 3816}},
    .playerControls = {.offsets = {4580, 4676, 4772, 4868, 4964, 5060, 5156}},
    .factionControls = {.offsets = {3908, 4004, 4100, 4196, 4292, 4388, 4484}},
    .selectionRows = {.offsets = {5252, 5348, 5444, 5540, 5636, 5732, 5828}},
    .statusRows = {.offsets = {5924, 6016, 6108, 6200, 6292, 6384, 6476}}};

/* Implementation ownership: ui/frontend/task_assignment. */

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
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t rowControlOffset;
  UiNodeVtable *rootVtable;
  int localPlayerRuntimeId;
  FrontendLoadedLevelAsset *loadedLevel;
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
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  loadedLevel = g_FrontendLoadedLevelAsset;
  activeFactionsLeft = g_FrontendLoadedLevelAsset->worldSettings.activeFactionCount;
  assignableFactionsLeft = g_FrontendLoadedLevelAsset->worldSettings.assignableFactionCount;
  /* Rows 1..assignable count: factions a player may take; mode button active, caption "Computer".
     Original quirk: at least one row is always set up (a level without assignable factions would wrap the
     counter). */
  rowCursor = 0;
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_COMPUTER;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~(FRONTEND_CONTROL_INACTIVE | UI_SELECTABLE_SELECTED_OR_CHECKED);
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor++;
    activeFactionsLeft--;
    assignableFactionsLeft--;
  } while (assignableFactionsLeft != 0);
  /* Further active factions (computer only): mode button active, the rest of the row hidden and inactive. */
  for (; activeFactionsLeft != 0; activeFactionsLeft--) {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags &=
         ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_COMPUTER;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags &= ~UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] =
         FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    rowCursor++;
  }
  /* Unused rows up to 7: everything hidden and inactive, caption "No-one", faction slot cleared. */
  for (; rowCursor < 7; rowCursor++) {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->textResourceId;
    *rowTextId = TEXT_ID_FACTION_MODE_NOBODY;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowCursor];
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags |=
         FRONTEND_CONTROL_INACTIVE;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.statusRows.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags |= UI_LABEL_HIDE_WHILE_SUPPRESSED;
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[rowCursor];
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->base.nodeFlags |=
         UI_NODE_SUPPRESSED;
    ((UiSingleLineTextControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->labelFlags |= UI_LABEL_HIDE_WHILE_SUPPRESSED;
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[rowCursor + 1] = 0;
  }
  /* Colour buttons of rows 7..1 show the faction name of the level's player slot; none is selected. */
  do {
    rowControlOffset = g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowCursor - 1];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->textResourceId =
         ((LevelPlayerSlotRecord *)((uint8_t *)loadedLevel->playerSlots +
                                    g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowCursor - 1]))->aiClassOrMode +
         TEXT_ID_FACTION_NAME_BASE + rowCursor;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->selectable.stateFlags &=
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
    rowTextId = &((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendRootPage,rowControlOffset))->textResourceId;
    playerRecord->factionAssignment.readyOrWaitState = 0;
    playerRecord->factionAssignment.consensusValue = 0;
    *rowTextId = TEXT_ID_FACTION_MODE_PLAYER;
    if (localPlayerRuntimeId == playerRecord->playerRuntimeId) {
      localPlayerRow = assignmentIndex - 1;
    }
    assignmentIndex++;
    playerRecord++;
    if (loadedLevel->worldSettings.assignableFactionCount < assignmentIndex) {
      assignmentIndex = assignmentIndex - loadedLevel->worldSettings.assignableFactionCount;
    }
    remainingPlayerRecords--;
  } while (remainingPlayerRecords != 0);
  /* tick and show the local player's play checkbox */
  ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[localPlayerRow]))
       ->selectable.stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
  ((UiTextButtonControl *)THANDOR_UI_AT(frontendRootPage,
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[localPlayerRow]))
       ->selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls((UiRootNode *)frontendRootPage);
  rootVtable = frontendRootPage->rootNode.vtable;
  /* frontendRootPage is the frontend root (FrontendUiImage). */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    /* local game: roster left/right offsets 96 (no participant column); Back and Next, no Finish */
    FRONTEND_UI(frontendRootPage,factionRosterTable)->leftOffset = 96;
    FRONTEND_UI(frontendRootPage,factionRosterTable)->rightOffset = 96;
    FRONTEND_UI(frontendRootPage,factionSetupFinishButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    FRONTEND_UI(frontendRootPage,factionSetupNextButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    FRONTEND_UI(frontendRootPage,factionSetupBackButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupBackButton))->selectable.stateFlags &=
         ~FRONTEND_CONTROL_INACTIVE;
  }
  else {
    /* network game: full-width roster, Finish instead of Next; a client cannot go back */
    FRONTEND_UI(frontendRootPage,factionRosterTable)->leftOffset = 0;
    FRONTEND_UI(frontendRootPage,factionRosterTable)->rightOffset = 0;
    FRONTEND_UI(frontendRootPage,factionSetupFinishButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
    ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupFinishButton))->selectable.stateFlags &=
         ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    FRONTEND_UI(frontendRootPage,factionSetupNextButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupNextButton))->selectable.stateFlags |=
           FRONTEND_CONTROL_INACTIVE;
      FRONTEND_UI(frontendRootPage,factionSetupBackButton)->nodeFlags |= UI_NODE_SUPPRESSED;
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupBackButton))->selectable.stateFlags |=
           FRONTEND_CONTROL_INACTIVE;
    }
    else {
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupNextButton))->selectable.stateFlags &=
           ~FRONTEND_CONTROL_INACTIVE;
      FRONTEND_UI(frontendRootPage,factionSetupBackButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
      ((UiFramedTextButtonControl *)FRONTEND_UI(frontendRootPage,factionSetupBackButton))->selectable.stateFlags &=
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
  uint32_t *controlFlags;

  controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,controlOffset + offsetof(UiNodeBase,nodeFlags),uint32_t);
  *controlFlags = *controlFlags | controlSetMask;
  controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,controlOffset + offsetof(UiNodeBase,nodeFlags),uint32_t);
  *controlFlags = *controlFlags & controlClearMask;
  controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
  *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
}

/* Faction setup page: hides the row control at controlOffset and marks it inactive. */
static void FrontendTaskAssignmentPage_DisableRowControl(UiRootNode *taskAssignmentRoot, uint32_t controlOffset)
{
  uint32_t *controlFlags;

  controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,controlOffset + offsetof(UiNodeBase,nodeFlags),uint32_t);
  *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
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
  uint32_t *controlFlags;
  uint16_t scannedChar;
  uint32_t controlOffset;
  uint32_t lastPlayerFactionIndex;
  FrontendLoadedLevelAsset *loadedLevelAsset;
  FrontendFactionAssignmentIndex localFactionIndex;
  FrontendPlayerRuntimeBlockCount remainingSearchCount;
  int row;
  int remainingDwords;
  int remainingCodeUnits;
  int localPlayerFaction;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerNameUtf16 *playerName;
  FrontendPlayerNameUtf16 *nameCharCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeRecord *otherPlayerRecord;
  FrontendTaskAssignmentFactionTextRow *textRowCursor;
  uint16_t *rosterTextCursor;
  uint16_t *rosterScanCursor;
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
  ((UiWrappedTextControl *)FRONTEND_UI(taskAssignmentRoot,taskDescriptionText))->text =
       (uint16_t *)(uintptr_t)
       (localFactionIndex + TEXT_ID_LEVEL_DESCRIPTION_BASE + g_FrontendLoadedLevelAsset->header.titleTextResourceIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE);
  /* Offsets from the control tables are control offsets in the page: + nodeFlags gives the control's nodeFlags
     (UI_NODE_SUPPRESSED), + rootFlags its stateFlags (UI_SELECTABLE_SELECTED_OR_CHECKED,
     FRONTEND_CONTROL_INACTIVE), + previousRoot its caption text id. Rows 7..1 (entry row - 1).
     Some nodeFlags accesses add offsetof(UiNodeBase,nodeFlags) to the offset first (THANDOR_UI_FIELD): that
     order is the one of the original code there. */
  for (row = 7; row != 0; row--) {
    if ((((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1]))->stateFlags &
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
             ((*(uint32_t *)&THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1])->nodeFlags &
               UI_NODE_SUPPRESSED) != 0)) {
      /* not a client and the row's assignment control is suppressed */
      controlOffset = g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1];
      if ((((UiSingleLineTextControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->labelFlags &
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
      controlFlags = (uint32_t *)&THANDOR_UI_AT(taskAssignmentRoot,controlOffset)->nodeFlags;
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      controlFlags = (uint32_t *)&((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags;
      *controlFlags = *controlFlags & ~FRONTEND_CONTROL_INACTIVE;
    }
    else {
      FrontendTaskAssignmentPage_ShowRowControl(taskAssignmentRoot,
                                                g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[row - 1],
                                                controlSetMask, controlClearMask);
    }
    controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[row - 1] + offsetof(UiNodeBase,nodeFlags),uint32_t);
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
  /* mode captions: nobody, computer for active factions, player where a player record has the faction */
  for (row = 7; row != 0; row--) {
    controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[row - 1];
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->textResourceId = TEXT_ID_FACTION_MODE_NOBODY;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[row] == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      ((UiFramedTextButtonControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->textResourceId = TEXT_ID_FACTION_MODE_COMPUTER;
      if ((*(uint32_t *)&THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1])->nodeFlags &
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
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    lastPlayerFactionIndex = playerRecord->factionAssignment.factionAssignmentIndex;
    ((UiFramedTextButtonControl *)THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[lastPlayerFactionIndex - 1]))->textResourceId = TEXT_ID_FACTION_MODE_PLAYER;
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
  /* Not a client: update the mode buttons, then hide those of factions taken by players. Original quirk: the
     original ORs the last player's faction index and ANDs the address of g_FrontendLoadedLevelAsset here
     (both left over from the loop above), where the set/clear masks were probably meant; kept as in the
     original. */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    for (row = 7; row != 0; row--) {
      if ((((UiSingleLineTextControl *)THANDOR_UI_AT(taskAssignmentRoot,
               g_FrontendTaskAssignmentControlOffsets.assignmentControls.offsets[row - 1]))->labelFlags &
           UI_LABEL_HIDE_WHILE_SUPPRESSED) == 0) {
        controlOffset = g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[row - 1];
        controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,controlOffset + offsetof(UiNodeBase,nodeFlags),uint32_t);
        *controlFlags = *controlFlags | lastPlayerFactionIndex;
        controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,controlOffset + offsetof(UiNodeBase,nodeFlags),uint32_t);
        *controlFlags = *controlFlags & (uint32_t)(uintptr_t)loadedLevelAsset; /* the quirk: low address bits */
      }
    }
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      controlFlags = (uint32_t *)&THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[playerRecord->factionAssignment.factionAssignmentIndex - 1])->nodeFlags;
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
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
  FRONTEND_UI(taskAssignmentRoot,rosterParticipantHeader)->nodeFlags |= UI_NODE_SUPPRESSED;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    return;
  }
  FRONTEND_UI(taskAssignmentRoot,rosterParticipantHeader)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerName = &playerRecord->playerName;
  /* Append every player's name to the roster row of its faction; playerName walks the player records. */
  do {
    rosterTextCursor = g_FrontendUiDisplayModeAndTaskAssignmentScratch.taskAssignmentText.rows
                      [FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.factionAssignmentIndex].textUtf16;
    remainingCodeUnits = 40; /* code units left in the row */
    if (*(int *)rosterTextCursor != 0) {
      /* The faction row already names a player: find its end and append ", " while room is left. */
      do {
        rosterScanCursor = rosterTextCursor;
        if (remainingCodeUnits == 0) break;
        remainingCodeUnits--;
        rosterScanCursor = rosterTextCursor + 1;
        scannedChar = *rosterTextCursor;
        rosterTextCursor = rosterScanCursor;
      } while (scannedChar != 0);
      rosterTextCursor = rosterScanCursor + 1;
      remainingCodeUnits--;
      if (remainingCodeUnits != 0) {
        rosterScanCursor[-1] = ',';
        rosterScanCursor[0] = ' ';
      }
    }
    if (remainingCodeUnits != 0) {
      if (21 < remainingCodeUnits) {
        remainingCodeUnits = 20; /* at most the 20 code units of a player name */
      }
      if (FRONTEND_PLAYER_RECORD_OF_NAME(playerName)->factionAssignment.consensusValue == 0) {
        /* consensusValue 0: the name between rich-text codes 0x8001 and 0x8000, else after 0x8000 */
        *rosterTextCursor = FRONTEND_TEXT_STYLE_HIGHLIGHTED;
        nameCharCursor = playerName;
        for (; remainingCodeUnits != 0; remainingCodeUnits--) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor++;
        }
        rosterTextCursor[0] = FRONTEND_TEXT_STYLE_NORMAL;
        rosterTextCursor[1] = 0;
      }
      else {
        *rosterTextCursor = FRONTEND_TEXT_STYLE_NORMAL;
        nameCharCursor = playerName;
        for (; remainingCodeUnits != 0; remainingCodeUnits--) {
          rosterTextCursor[1] = nameCharCursor->textUtf16[0];
          nameCharCursor = (FrontendPlayerNameUtf16 *)(nameCharCursor->textUtf16 + 1);
          rosterTextCursor++;
        }
        *rosterTextCursor = 0;
      }
    }
    remainingPlayers--;
    /* next player record */
    playerName = playerName + sizeof(FrontendPlayerRuntimeRecord) / sizeof(FrontendPlayerNameUtf16);
  } while (remainingPlayers != 0);
  /* once the local player has pressed Finish, the faction rows are hidden */
  if ((((UiSelectableControl *)FRONTEND_UI(taskAssignmentRoot,factionSetupFinishButton))->stateFlags &
       UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    for (row = 7; row != 0; row--) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1];
      if ((((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
        controlFlags = &THANDOR_UI_FIELD(taskAssignmentRoot,controlOffset + offsetof(UiNodeBase,nodeFlags),uint32_t);
        *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      }
    }
  }
  else {
    for (row = 7; row != 0; row--) {
      controlOffset = g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[row - 1];
      if ((((UiSelectableControl *)THANDOR_UI_AT(taskAssignmentRoot,controlOffset))->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
        controlFlags = (uint32_t *)&THANDOR_UI_AT(taskAssignmentRoot,controlOffset)->nodeFlags;
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
      controlFlags = (uint32_t *)&THANDOR_UI_AT(taskAssignmentRoot,g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[localPlayerFaction - 1])->nodeFlags;
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      return;
    }
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
}
