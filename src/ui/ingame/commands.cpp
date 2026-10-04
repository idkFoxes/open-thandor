/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/commands.h>
#include <thandor/thandor.h>

/* Module data. */

static int32_t g_UiAction100AControlOffsets[8] = {45584, 45712, 45840, 45968, 46096, 46224, 46352, 46480};

/* Implementation ownership: ui/ingame/commands. */

/* In-game command handler 0x370 (key P): toggles the player's pause request, then toggles the global pause once
   every player agrees - the game pauses when all players request it and resumes when none does any more.
*/
void InGameCommand_TogglePauseRequest
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

{
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;

  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->sessionFlags =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->sessionFlags ^ PLAYER_SESSION_FLAG_PAUSE_REQUESTED;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) == 0) {
      if ((g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->sessionFlags &
           PLAYER_SESSION_FLAG_PAUSE_REQUESTED) == 0) {
        return;
      }
    }
    else if ((g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->sessionFlags &
              PLAYER_SESSION_FLAG_PAUSE_REQUESTED) != 0) {
      return;
    }
    playerRecord++;
    remainingPlayers--;
  } while (remainingPlayers != 0);
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_PAUSED;
  return;
}

/* In-game command handler INGAME_COMMAND_PLACE_ARMY: takes the player's pending army asset (stock entry chosen
   in the army stock panel), validates the placement at the clicked point and creates the army there with the
   given heading, counts it for the faction and spawns the asset's placement effect. A rejected placement leaves
   the asset pending; a successful one ends the local placement mode.
*/
void InGameCommand_ExecuteLocalPlacementFromSelection(PlayerRuntimeId playerId,CommandPayload headingAngle,
          CommandPayload worldXQ12,CommandPayload worldYQ12)

{
  FactionRelationCounter *relationCounter;
  uintptr_t pendingEntry;
  uint32_t ownerFactionIndex;
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *armySlot;
  ModelRuntimeSlot *slotModelRuntime;
  InGameRuntimeRoot *runtimeRoot;
  Ptr32<ArmyRuntimeSlot> *createdArmySlots;
  WorldRuntimeContext *worldRuntime;
  Bool8 placementRejected;

  runtimeRoot = g_InGameRuntimeRoot;
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerId];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  /* take the pending entry and clear it atomically, as in the original */
  LOCK();
  pendingEntry = playerBlock->pendingPlacementArmyAsset;
  playerBlock->pendingPlacementArmyAsset = 0;
  UNLOCK();
  if (pendingEntry != 0) {
    /* the pending entry is the chosen army asset record */
    placementRejected = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                      (0,headingAngle,worldXQ12,worldYQ12,
                       (ArmyPlacementContext)((ArmyAssetRecordPrefix *)pendingEntry)->registryId,playerBlock->factionIndex,
                       worldRuntime);
    if (!placementRejected) {
      /* the validator leaves the accepted (possibly snapped) point in g_ArmyPlacementValidatedWorldX/YQ12 */
      createdArmySlots = (Ptr32<ArmyRuntimeSlot> *)ArmyRuntime_CreateInstanceFromAsset
                        (4,headingAngle,g_ArmyPlacementValidatedWorldYQ12,
                         g_ArmyPlacementValidatedWorldXQ12,
                         playerBlock->factionIndex,
                         ((ArmyAssetRecordPrefix *)pendingEntry)->registryId,worldRuntime,nullptr);
      if (createdArmySlots != nullptr) {
        ownerFactionIndex = playerBlock->factionIndex;
        modelNodeRuntime = createdArmySlots[1];
        armySlot = *createdArmySlots;
        modelNodeRuntime->movementPosition0Q12 = 0;
        if (ownerFactionIndex == (runtimeRoot->worldRuntime).activeFactionRuntimeIndex) {
          modelNodeRuntime->movementPosition0Q12 = INT32_MAX;
        }
        relationCounter = &g_GameFactionRuntimeImage.records[ownerFactionIndex].relationCounterB;
        *relationCounter = *relationCounter + 1;
        slotModelRuntime = (armySlot->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdArmySlots,worldRuntime); /* the created army */
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },
                   ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle2,
                   ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle1,
                   ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle0,
                   ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.z,
                   ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.y,
                   ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.x,
                   Thandor_U32ToPointer<EffectDefinition>(slotModelRuntime->attachments[2].childLocalRotationAngle0), /* 5f-format: ModelRuntimeSlot.attachments[2].childLocalRotationAngle0 (saved model pool) */
                   worldRuntime);
        InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
        InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
        if (playerId != g_LocalPlayerRuntimeId) {
          return;
        }
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
        g_InGamePendingPlacementArmyAsset = 0;
        return;
      }
    }
    g_SelectionPlayerRuntimeBlockPointers[playerId]->pendingPlacementArmyAsset = pendingEntry;
  }
  return;
}

/* Technology window close button (action 0x1011, g_InGameUiActionHandlersPage10[17]): shows the world view again,
   closes the window (page 0 of the game window page stack) and, if something is selected, sends
   INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE with -1 (cancel) for the first selected building, which gives back what
   opening the page took away.
*/
void InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control)

{
  GameEntityRuntime *firstSelectedEntity;
  CommandPayload modelOffset;

  /* control becomes the in-game UI root */
  while (control->parent != UI_NODE_NONE) {
    control = control->parent;
  }
  INGAME_UI(control,worldView)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,(UiPageStackControl *)INGAME_UI(control,gameWindowPageStack));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != nullptr) {
    modelOffset = (int)((intptr_t)(firstSelectedEntity->common).ownership.definitionOrClassRecord -
                        (intptr_t)g_ModelRuntimeRebaseDelta);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
                (g_LocalPlayerRuntimeId,0,0xffffffff,modelOffset);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE,0,0xffffffff,modelOffset);
    }
  }
  return;
}

/* Selection group button click (action 0x100A, g_InGameUiActionHandlersPage10[10]; the 8 buttons of
   g_UiAction100AControlOffsets): the mouse version of the 1..8 group keys. A plain click recalls the group, a
   modifier key merges (SELECTION_TRANSFER_MERGE), the right button stores the selection into the group
   (SELECTION_TRANSFER_TO_GROUP) and a double click also centres the view. Every variant except the plain recall
   is refused when SelectionInfo_AllEntriesEmptyOrMatchOwner reports so for the active faction.
*/
void InGameSelectionGroupButton_RecallOrStoreGroup(UiCommandSpriteButtonControl *control)

{
  UiCommandSpriteButtonControl *root;
  FactionRuntimeIndex factionIndex;
  CommandPayload groupIndex;
  CommandPayload transferModeFlags;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) != 0) {
    return;
  }
  root = control;
  while ((root->sprite).selectable.base.parent != UI_NODE_NONE) {
    root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
  }
  /* find the group of the clicked button */
  groupIndex = SELECTION_GROUP_COUNT - 1;
  while ((int)((uintptr_t)control - (uintptr_t)root) != g_UiAction100AControlOffsets[groupIndex]) {
    groupIndex--;
    if ((int)groupIndex < 0) {
      return;
    }
  }
  transferModeFlags = 0;
  if ((control->activationInputState & UI_COMMAND_ACTIVATION_LOW_INPUT_NIBBLE_MASK) != 0) {
    transferModeFlags = SELECTION_TRANSFER_MERGE;
  }
  if ((control->activationInputState & UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON) != 0) {
    transferModeFlags = transferModeFlags | SELECTION_TRANSFER_TO_GROUP;
  }
  if ((control->activationInputState & UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK) != 0) {
    transferModeFlags = transferModeFlags | SELECTION_TRANSFER_CENTER_VIEW;
  }
  if ((transferModeFlags != 0) &&
      SelectionInfo_AllEntriesEmptyOrMatchOwner
           ((FactionRuntimeIndex)((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex)) {
    return;
  }
  factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_SELECTION_GROUP,(CommandPayload)factionIndex,transferModeFlags,groupIndex);
    return;
  }
  FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
            (g_LocalPlayerRuntimeId,factionIndex,transferModeFlags,groupIndex);
  return;
}

/* Empty callback: InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState installs it as
   fieldRegion.clearTransientStateCallback of the world runtime while the editor is active.
*/
void UiCommandRuntime_CallbackNoOp()

{
  return;
}

/* In-game command handler 0x150 (quit game window and player departure): CLOSE_SESSION ends the session,
   SURRENDER destroys every army of the player's faction. Without flags the player has left: another player's
   departure is announced in the message history (text 0xFF08); the local player's own departure marks the
   session as left and, in a network game, shuts the network backend down and falls back to a one-player setup.
*/
void InGameCommand_HandlePlayerDeparture
          (PlayerOrFactionRuntimeId32 playerOrFactionId,uint32_t value1,uint32_t value2,
          GameEntityCommandFlags flags)

{
  WorldRuntimeContext *worldRuntime;
  uint32_t factionToken;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRoot *runtimeRoot;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint16_t *departureText;
  WorldOwnerListNode *ownerNode;
  
  runtimeRoot = g_InGameRuntimeRoot;
  if ((flags & INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION) != 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED;
    return;
  }
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  if ((flags & INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER) != 0) {
    factionToken = g_SelectionPlayerRuntimeBlockPointers[playerOrFactionId]->factionIndex;
    for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        entityRuntime = (GameEntityRuntime *)
             (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset).armyRuntime;
        if (factionToken == (entityRuntime->common).ownership.ownerIndex) {
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
        }
      }
    }
    return;
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerOrFactionId == playerRecord->playerRuntimeId) {
      playerRecord->heartbeatExpiryTicks = 0;
      if (playerRecord == g_FrontendPlayerRuntimeBlocks) {
        g_SessionTransferTimeoutTicks = 0;
      }
      if (playerOrFactionId == (runtimeRoot->worldRuntime).selection.activePlayerRuntimeId) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT;
        Resource_Release((void *)(uintptr_t)g_FrontendLoadedCampaignAsset);
        g_FrontendLoadedCampaignAsset = 0;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          g_FrontendLoadedCampaignAsset = 0; /* stored twice, as in the original */
          return;
        }
        g_SessionNetworkRoleFlags =
             g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
        g_SessionTransferTimeoutTicks = 0;
        g_NetworkBackendSlot3();
        g_NetworkBackendSlot1();
        playerRecord = g_FrontendPlayerRuntimeBlocks;
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        (playerRecord->playerName).textUtf16[0] = 0;
        (playerRecord->playerName).textUtf16[1] = 0;
        playerRecord->playerRuntimeId = 0;
        (playerRecord->factionAssignment).roleStateFlags = 0;
        return;
      }
      /* departure message with the player name patched in */
      departureText = TextResource_Resolve(TEXT_ID_PLAYER_DEPARTED);
      RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,departureText);
      InGameRecentTextHistory_InsertAndRebuild8(departureText);
      return;
    }
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
  return;
}

/* Changes the global g_UiCommandRuntimeFlags: first clears clearMask, then sets setMask, then toggles toggleMask
   (the masks come in the reverse order as arguments). Local games call it directly, network games send the
   same masks as player command 0x310. playerRuntimeId is not used: the flags are not per player.
*/
void UiCommandRuntimeFlags_ApplyClearSetToggleMasks(PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
          UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~clearMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | setMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ toggleMask;
}

InGameUiCommandModeActionHandlerPage11 g_InGameUiActionHandlersPage11 = {
        .handlers = {
            /*  0 */ UI_SLOT(InGameCommandModeG_Select0),
            /*  1 */ UI_SLOT(InGameCommandModeG_Select1),
            /*  2 */ UI_SLOT(InGameCommandModeG_Select2),
            /*  3 */ nullptr,
            /*  4 */ UI_SLOT(InGameCommandModeG_Select5),
            /*  5 */ UI_SLOT(InGameCommandModeG_Select3),
            /*  6 */ UI_SLOT(InGameCommandModeG_Select4),
            /*  7 */ nullptr,
            /*  8 */ UI_SLOT(InGameCommandModeC_Select0),
            /*  9 */ UI_SLOT(InGameCommandModeC_Select1),
            /* 10 */ UI_SLOT(InGameCommandModeC_Select2),
            /* 11 */ UI_SLOT(InGameCommandModeC_Select3),
            /* 12 */ UI_SLOT(InGameCommandModeD_Select0),
            /* 13 */ UI_SLOT(InGameCommandModeD_Select1),
            /* 14 */ UI_SLOT(InGameCommandModeD_Select2),
            /* 15 */ UI_SLOT(InGameCommandModeD_Select3),
            /* 16 */ UI_SLOT(InGameCommandMatrix_SelectMappedControl),
            /* 17 */ UI_SLOT(InGameCommandModeA_Select0),
            /* 18 */ UI_SLOT(InGameCommandModeA_Select1),
            /* 19 */ UI_SLOT(InGameCommandModeA_Select2),
            /* 20 */ UI_SLOT(InGameCommandModeB_Select0),
            /* 21 */ UI_SLOT(InGameCommandModeB_Select1),
            /* 22 */ UI_SLOT(InGameCommandModeB_Select2),
            /* 23 */ UI_SLOT(InGameCommandModeE_Select0),
            /* 24 */ UI_SLOT(InGameCommandModeE_Select1),
            /* 25 */ UI_SLOT(InGameCommandModeE_Select2),
            /* 26 */ UI_SLOT(InGameCommandRange_DispatchState0),
            /* 27 */ UI_SLOT(InGameCommandRange_DispatchState1),
            /* 28 */ UI_SLOT(InGameCommandModeF_Select0),
            /* 29 */ UI_SLOT(InGameCommandModeF_Select1)
        }};
