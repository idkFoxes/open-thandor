/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/faction_setup.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/faction_setup.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

FrontendUiImage *g_FrontendRootNode = nullptr;

static uint32_t g_FrontendFactionAssignmentReadyStateGeneration = 0;

/* The original indexes the seven-entry row tables (and factionLifecycleStates[rowIndex + 1]) with the row index
   of a frontend command without a check; bounded here because in a network game the command (and its row
   index) comes from a peer. Rows 0..6 pass; anything else is dropped (logged once). */
static bool FrontendFactionSetup_IsValidRowIndex(FrontendFactionAssignmentIndex rowIndex,const char *commandName)

{
  static int s_loggedInvalidRowIndex;

  if (rowIndex >= 0 && rowIndex < 7) {
    return true;
  }
  if (s_loggedInvalidRowIndex == 0) {
    s_loggedInvalidRowIndex = 1;
    Thandor_Log("faction setup: %s row index %d out of range, command dropped",commandName,rowIndex);
  }
  return false;
}

/* Handler of action 0x2044 (slot 68 of g_FrontendUiActionHandlersPage20.handlers00_54), the colour buttons of
   the faction setup page: finds the row of the pressed button in the factionControls offset table and cycles
   that faction's colour (FrontendFactionSetup_CycleFactionColour directly in a local game,
   FRONTEND_COMMAND_CYCLE_FACTION_COLOUR in a network game).
*/
void FrontendFactionSetupAction_CycleFactionColour(UiNodeBase *factionControl)

{
  CommandPayload rowIndex;
  
  for (rowIndex = 0; rowIndex < 7; rowIndex++) {
    if ((int)Thandor_ByteDistance(factionControl,g_FrontendRootNode) ==
        g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndex]) {
      FrontendCommand_Issue<FrontendFactionSetup_CycleFactionColour>(0,0,rowIndex);
      return;
    }
  }
}

/* Handler of action 0x2045 (slot 69 of g_FrontendUiActionHandlersPage20.handlers00_54), the mode buttons of
   the faction setup page: finds the row of the pressed button in the playerControls offset table and toggles
   whether that faction takes part (FrontendFactionSetup_ToggleFactionActive directly in a local game,
   FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE in a network game).
*/
void FrontendFactionSetupAction_ToggleFactionActive(UiNodeBase *playerControl)

{
  CommandPayload rowIndex;
  
  for (rowIndex = 0; rowIndex < 7; rowIndex++) {
    if ((int)Thandor_ByteDistance(playerControl,g_FrontendRootNode) ==
        g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndex]) {
      FrontendCommand_Issue<FrontendFactionSetup_ToggleFactionActive>(0,0,rowIndex);
      return;
    }
  }
}

/* Handler of action 0x2046 (slot 70 of g_FrontendUiActionHandlersPage20.handlers00_54), the "play" checkboxes
   of the faction setup page: finds the row of the pressed checkbox in the selectionRows offset table and makes
   that faction the local player's (FrontendFactionSetup_ChooseFaction directly in a local game,
   FRONTEND_COMMAND_CHOOSE_FACTION in a network game).
*/
void FrontendFactionSetupAction_ChooseFaction(UiNodeBase *selectionRowControl)

{
  CommandPayload rowIndex;
  
  for (rowIndex = 0; rowIndex < 7; rowIndex++) {
    if ((int)Thandor_ByteDistance(selectionRowControl,g_FrontendRootNode) ==
        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndex]) {
      FrontendCommand_Issue<FrontendFactionSetup_ChooseFaction>(0,0,rowIndex);
      return;
    }
  }
}

/* Handler of action 0x2040 (slot 64 of g_FrontendUiActionHandlersPage20.handlers00_54), the faction setup
   page's "Back" button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE
   in a network game).
*/
void FrontendFactionSetupAction_ReturnToMainPage(uint32_t callbackArgument)

{
  FrontendCommand_Issue<FrontendSession_ReturnToMainPage>(0,0,0);
}

/* Handler of frontend command FRONTEND_COMMAND_CYCLE_FACTION_COLOUR (0x650), called directly by
   FrontendFactionSetupAction_CycleFactionColour in a local game: advances the colour of faction row rowIndex + 1 (0-based index)
   by one, wrapping after 7 colours (8 when the requesting player has colourCycleFlags bit 0); the row's caption
   (faction colour name) and the level player slot's colour index (the field typed aiClassOrMode) move together.
   Nothing happens for an unknown player id.
*/
void FrontendFactionSetup_CycleFactionColour
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  int *levelCycleCounterField;
  LevelPlayerSlotByteOffset32 playerSlotOffset;
  FrontendLoadedLevelAsset *loadedLevelAsset;
  uint32_t nextSelectionTextId;
  uint32_t playerRecordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  UiFramedTextButtonControl *selectionControl;
  int selectionTextCycleLength;
  int *selectionCycleCounterField;
  
  if (!FrontendFactionSetup_IsValidRowIndex(rowIndex,"cycle colour")) {
    return;
  }
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  selectionTextCycleLength = 7;
  playerRecordsRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    if (playerRuntimeId == playerRecordCursor->playerRuntimeId) {
      if ((playerRecordCursor->colourCycleFlags & 1) != 0) {
        selectionTextCycleLength = 8;
      }
      playerSlotOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowIndex];
      selectionControl = Thandor_At<UiFramedTextButtonControl>
           (g_FrontendRootNode,g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndex]);
      nextSelectionTextId = selectionControl->textResourceId + 1;
      selectionCycleCounterField =
           reinterpret_cast<int *>(&Thandor_At<LevelPlayerSlotRecord>(g_FrontendLoadedLevelAsset->playerSlots,playerSlotOffset)->aiClassOrMode);
      *selectionCycleCounterField = *selectionCycleCounterField + 1;
      if (selectionTextCycleLength + (TEXT_ID_FACTION_NAME_BASE + 1U) <= nextSelectionTextId) {
        nextSelectionTextId = TEXT_ID_FACTION_NAME_BASE + 1;
        levelCycleCounterField = reinterpret_cast<int *>(&Thandor_At<LevelPlayerSlotRecord>(loadedLevelAsset->playerSlots,playerSlotOffset)->aiClassOrMode);
        *levelCycleCounterField = *levelCycleCounterField - selectionTextCycleLength;
      }
      selectionControl->textResourceId = nextSelectionTextId;
      return;
    }
    playerRecordCursor++;
    playerRecordsRemaining--;
  } while (playerRecordsRemaining != 0);
}

/* Handler of frontend command FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE (0x6F0), called directly by
   FrontendFactionSetupAction_ToggleFactionActive in a local game: unless a player has chosen faction rowIndex + 1, toggles
   whether that faction takes part (FACTION_RUNTIME_LIFECYCLE_ACTIVE: computer or nobody) and refreshes the
   faction setup page.
*/
void FrontendFactionSetup_ToggleFactionActive
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  FactionRuntimeLifecycleObservedState *lifecycleState;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  if (!FrontendFactionSetup_IsValidRowIndex(rowIndex,"toggle active")) {
    return;
  }
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    if (rowIndex + 1 == playerBlock->factionAssignment.factionAssignmentIndex) {
      return;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates + rowIndex + 1;
  *lifecycleState = *lifecycleState ^ FACTION_RUNTIME_LIFECYCLE_ACTIVE;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(&g_FrontendRootNode->frontendRoot.root);
}

/* Handler of frontend command FRONTEND_COMMAND_CHOOSE_FACTION (0x750), called directly by
   FrontendFactionSetupAction_ChooseFaction in a local game: unless the row is inactive (FRONTEND_CONTROL_INACTIVE), the
   player chooses faction rowIndex + 1. For the local player the row's checkbox becomes the only one checked.
   The player's record (the first one in a local game) gets the faction and the next ready-state generation,
   which orders the choices, then the faction setup page is refreshed.
*/
void FrontendFactionSetup_ChooseFaction
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  FrontendPlayerRuntimeBlockCount remainingBlockCount;
  uint32_t readyStateGeneration;
  UiTextButtonControl *selectedControl;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeRecord *matchedPlayerBlock;

  if (!FrontendFactionSetup_IsValidRowIndex(rowIndex,"choose faction")) {
    return;
  }
  selectedControl =
       g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndex]);
  if ((selectedControl->selectable.stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      UiSelectableGroup_SelectExclusive(7,&selectedControl->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[0])->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[1])->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[2])->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[3])->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[4])->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[5])->selectable.base,
          &g_FrontendRootNode->NodeAt<UiTextButtonControl>(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[6])->selectable.base);
    }
    readyStateGeneration = g_FrontendFactionAssignmentReadyStateGeneration;
    /* network game: search the player's record; when the count runs out first, the first record is used
       (as it is in a local game) */
    matchedPlayerBlock = g_FrontendPlayerRuntimeBlocks;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
      remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
      playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
      /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
      do {
        if (playerRuntimeId == playerBlockCursor->playerRuntimeId) {
          matchedPlayerBlock = playerBlockCursor;
          break;
        }
        remainingBlockCount--;
        playerBlockCursor++;
      } while (remainingBlockCount != 0);
    }
    matchedPlayerBlock->factionAssignment.factionAssignmentIndex = rowIndex + 1;
    matchedPlayerBlock->factionAssignment.readyOrWaitState = readyStateGeneration;
    g_FrontendFactionAssignmentReadyStateGeneration++;
    FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(&g_FrontendRootNode->frontendRoot.root);
  }
}
