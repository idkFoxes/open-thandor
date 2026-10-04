/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/army_stock.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/army_stock.h>
#include <thandor/thandor.h>

/* Module data. */

GameFactionRuntimeImage g_GameFactionRuntimeImage = {.tail = {.factionLifecycleStates = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1}}};

/* Implementation ownership: gameplay/faction/army_stock. */

/* Called when an army is destroyed: clears every slot of the eight factions' 256-entry runtime group member
   tables that still points to it, so no group keeps a dangling pointer to the freed army slot.
*/
void GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember)

{
  int factionsRemaining;
  int groupSlotsRemaining;
  int groupSlotIndex;
  GameFactionRuntimeImage *factionRecordCursor;
  
  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionsRemaining = 8;
  groupSlotsRemaining = 256;
  groupSlotIndex = 0;
  do {
    do {
      if (runtimeGroupMember ==
          factionRecordCursor->records[0].runtimeGroupMembers8x32[groupSlotIndex]) {
        factionRecordCursor->records[0].runtimeGroupMembers8x32[groupSlotIndex] = nullptr;
      }
      groupSlotIndex++;
      groupSlotsRemaining--;
    } while (groupSlotsRemaining != 0);
    /* the cursor steps one faction record (0x740 bytes) at a time */
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    groupSlotsRemaining = 256;
    groupSlotIndex = 0;
    factionsRemaining--;
  } while (factionsRemaining != 0);
  return;
}

/* Checks whether the faction already has armyAssetRecord pending: in its secondary army-asset list, or in
   production in one of its class 0x0B/0x0D structures (state word 0x2E == 1). The result is
   false when found, true when not.
*/
Bool8 FactionRuntime_IsArmyAssetNotPending
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *activeAssetRecord;
  WorldOwnerListNode *ownerNode;
  int *modelPayload;
  FactionArmyAssetCount assetIndex;

  for (assetIndex = 0; assetIndex < g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
       assetIndex++) {
    if (armyAssetRecord ==
        Thandor_U32ToPointer<ArmyAssetRecordPrefix>(g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds[assetIndex])) { /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
      return false;
    }
  }
  /* Structures of class 0xB or 0xD in state 1 that are currently producing armyAssetRecord. */
  for (ownerNode = g_InGameRuntimeRoot->worldRuntime.ownerListHead;
      ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelPayload = (int *)ownerNode->runtimePayload;
    if ((factionIndex == ((ModelRuntimeSlot *)modelPayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
        ((((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) ||
         (((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13)) &&
        (modelPayload[46] == 1)) {
      activeAssetRecord = Thandor_U32ToPointer<ArmyAssetRecordPrefix>(modelPayload[24]); /* 5f-format: ModelRuntimeSlot class state word 24 (asset in production) */
      if (armyAssetRecord == activeAssetRecord) {
        return false;
      }
    }
  }
  return true;
}

/* Queues repetitionCount units of an army record for a faction (the build buttons of the in-game catalog and
   the AI): appends the registry pointer of armyAssetId that many times to the faction's secondary army-asset
   list, stopping when its 64 entries are full. An unknown id queues nothing.
*/
void GameFactionRuntime_RegisterArmyAssetPointers(uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  GameFactionRuntimeRecord *factionRecord;
  ArmyAssetRecordPrefix *resolvedAsset;

  if (ArmyAssetRegistry_FindById(armyAssetId,&resolvedAsset) != 0) {
    return;
  }
  factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
  /* Original quirk: the count is tested after the first append, so repetitionCount 0 wraps around and fills
     the list up to its capacity. */
  do {
    if (factionRecord->secondaryArmyAssetCount >= FACTION_ARMY_ASSET_LIST_CAPACITY) {
      return;
    }
    factionRecord->secondaryArmyAssetPointersOrIds[factionRecord->secondaryArmyAssetCount] = Thandor_PointerToU32(resolvedAsset); /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
    factionRecord->secondaryArmyAssetCount++;
    repetitionCount--;
  } while (repetitionCount != 0);
}

/* Whether a producing structure of class producerClassId is building armyDefinition right now: payload [0x18]
   holds the army in production; a class 0x0D structure must also be able to build the army (its definition's
   classParameterC4 against the army's flags) and have production state [0x2E] == 1. */
static Bool8 GameFactionRuntime_StructureProducesArmy(const int *modelPayload,int producerClassId,
          const ArmyAssetRecordPrefix *armyDefinition)
{
  if (producerClassId == MODEL_RUNTIME_CLASS_13) {
    return (((uint32_t)((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->classParameterC4 &
             armyDefinition[1].selectionDetailTemplateVariantIndex) != 0) &&
           (armyDefinition->registryId == modelPayload[24]) && (modelPayload[46] == 1);
  }
  return armyDefinition->registryId == modelPayload[24];
}

/* Stops a structure's production: clears the army in production [0x18], its state word (stateWordIndex: [0x2E],
   or [0x2B] for the aircraft pad) and flag 0x100 of [0x3B], takes its share [0x1D] back out of [0x3D] and refunds
   the full price to the faction's xenite. */
static void GameFactionRuntime_StopProductionAndRefund(int *modelPayload,int stateWordIndex,
          const ArmyAssetRecordPrefix *armyDefinition,FactionRuntimeIndex factionIndex)
{
  PckArmyAssetIdCatalog refundAmount;

  modelPayload[61] = modelPayload[61] - modelPayload[29];
  refundAmount = armyDefinition[2].registryId;
  modelPayload[29] = 0;
  modelPayload[stateWordIndex] = 0;
  modelPayload[59] = modelPayload[59] & ~(uint32_t)ARMY_MODEL_STATE_PRODUCING;
  modelPayload[24] = 0;
  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + refundAmount;
}

/* In-game command INGAME_COMMAND_CANCEL_QUEUED_ARMY (the cancel click on a build button of the in-game catalog,
   the reverse of GameFactionRuntime_RegisterArmyAssetPointers): cancels up to requestedCount orders of an army
   record. Waiting orders are taken out of the faction's production queue first (without a refund); what is left is cancelled in the faction's producing structures (class 0x0B, 0x16 or 0x0D, chosen by the
   army's flags 0x10 / 0x08), which stop production and refund the full price to the faction's xenite.
*/
void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionArmyAssetCount *secondaryCount;
  uint32_t *queuedAssets;
  int *modelPayload;
  ArmyAssetRecordPrefix *armyDefinition;
  FactionArmyAssetCount assetsRemaining;
  int writeIndex;
  int readIndex;
  int producerClassId;
  WorldOwnerListNode *ownerNode;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyDefinition) != 0) {
    return;
  }
  /* compact the queue (secondaryArmyAssetPointersOrIds), dropping the first matching entries */
  queuedAssets = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  readIndex = 0;
  writeIndex = 0;
  assetsRemaining = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  while (assetsRemaining != 0) {
    if ((Thandor_PointerToU32(armyDefinition) == queuedAssets[readIndex]) && (0 < (int)requestedCount)) { /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
      readIndex++;
      secondaryCount = &g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
      *secondaryCount = *secondaryCount - 1;
      requestedCount--;
    }
    else {
      queuedAssets[writeIndex] = queuedAssets[readIndex];
      readIndex++;
      writeIndex++;
    }
    assetsRemaining--;
  }
  if (requestedCount == 0) {
    return;
  }
  /* the structure class that produces this army */
  ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
  if ((armyDefinition[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) != 0) {
    producerClassId = MODEL_RUNTIME_CLASS_11;
  }
  else if ((armyDefinition[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) != 0) {
    producerClassId = MODEL_RUNTIME_CLASS_22;
  }
  else {
    producerClassId = MODEL_RUNTIME_CLASS_13;
  }
  for (; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelPayload = (int *)ownerNode->runtimePayload;
    if ((factionIndex != ((ModelRuntimeSlot *)modelPayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) ||
        (producerClassId != ((ModelRuntimeSlot *)modelPayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId)) {
      continue;
    }
    if (GameFactionRuntime_StructureProducesArmy(modelPayload,producerClassId,armyDefinition)) {
      GameFactionRuntime_StopProductionAndRefund
                (modelPayload,producerClassId == MODEL_RUNTIME_CLASS_22 ? 43 : 46,armyDefinition,factionIndex);
      requestedCount--;
      if (requestedCount == 0) {
        return;
      }
    }
  }
}

/* Takes the first entry equal to armyDefinition out of the faction's primary army-asset list
   (primaryArmyAssetPointersOrIds),
   moving the later entries down one slot, and returns true; returns false when there is none.
   Original quirk: the last move reads the entry one past the count (with a full list of 64 that is the first
   runtime group member pointer that follows the list). */
static Bool8 GameFactionRuntime_RemoveFirstPrimaryArmyAsset(GameFactionRuntimeRecord *factionRecord,
          const ArmyAssetRecordPrefix *armyDefinition)
{
  uint32_t *primaryAssets;
  FactionArmyAssetCount assetIndex;

  primaryAssets = factionRecord->primaryArmyAssetPointersOrIds;
  for (assetIndex = 0; assetIndex < factionRecord->primaryArmyAssetCount; assetIndex++) {
    if (armyDefinition == Thandor_U32ToPointer<const ArmyAssetRecordPrefix>(primaryAssets[assetIndex])) { /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      /* close the gap */
      for (; assetIndex < factionRecord->primaryArmyAssetCount; assetIndex++) {
        primaryAssets[assetIndex] = primaryAssets[assetIndex + 1];
      }
      factionRecord->primaryArmyAssetCount--;
      return true;
    }
  }
  return false;
}

/* In-game command INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT (clicking a finished army in the in-game catalog):
   takes the first entry of the army record out of the faction's primary army-asset list
   (primaryArmyAssetPointersOrIds) and stages it in the player's pending slot (pendingPlacementArmyAsset) for
   placement on the map. When the faction is the one shown
   the command sprite grid is rebuilt, and for the local player the placement cursor is armed. If the faction
   has no such entry the pending slot is cleared again.
*/
void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyAssetRecordPrefix *armyDefinition;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyDefinition) != 0) {
    return;
  }
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  playerBlock->pendingPlacementArmyAsset = (uintptr_t)armyDefinition;
  if (!GameFactionRuntime_RemoveFirstPrimaryArmyAsset(&g_GameFactionRuntimeImage.records[factionIndex],
                                                      armyDefinition)) {
    playerBlock->pendingPlacementArmyAsset = 0;
    return;
  }
  if (factionIndex != (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex) {
    return;
  }
  InGameArmyStock_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  if (playerRuntimeId != g_LocalPlayerRuntimeId) {
    return;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
  g_InGamePendingPlacementArmyAsset = (intptr_t)armyDefinition;
  g_InGamePlacementSurfaceHeightQ12OrSentinel = INT32_MAX; /* no surface picked yet */
}

/* In-game command handler, the reverse of GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer: takes the
   army asset staged for the player's placement (exchanged with 0) back into the faction's primary army-asset list
   (at most 64 entries). When the faction is the one shown it rebuilds the command sprite grid, and for the local
   player it ends the pending placement.
*/
void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex)

{
  uintptr_t pendingAsset;
  uint32_t assetCount;
  InGameRuntimeRoot *runtimeRoot;

  LOCK();
  pendingAsset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingPlacementArmyAsset;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pendingPlacementArmyAsset = 0;
  runtimeRoot = g_InGameRuntimeRoot;
  UNLOCK();
  if (pendingAsset == 0) {
    return;
  }
  assetCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  if (assetCount < FACTION_ARMY_ASSET_LIST_CAPACITY) {
    /* primaryArmyAssetPointersOrIds[assetCount] */
    g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[assetCount] =
         (uint32_t)pendingAsset; /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
    g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount++;
  }
  if (factionIndex == runtimeRoot->worldRuntime.activeFactionRuntimeIndex) {
    InGameArmyStock_RebuildGrid((UiNodeBase *)runtimeRoot);
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
      g_InGamePendingPlacementArmyAsset = 0;
    }
  }
}

/* In-game command INGAME_COMMAND_SELL_ARMY (the sell click on a finished army in the in-game catalog): takes the
   first entry of the army record out of the faction's primary army-asset list (primaryArmyAssetPointersOrIds)
   and credits 7/8 of its price to the faction's xenite. The command sprite grid is rebuilt when the faction is
   the one shown.
*/
void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedPlayerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex)

{
  PckArmyAssetIdCatalog price;
  ArmyAssetRecordPrefix *resolvedAsset;

  if (ArmyAssetRegistry_FindById(armyAssetId,&resolvedAsset) != 0) {
    return;
  }
  if (!GameFactionRuntime_RemoveFirstPrimaryArmyAsset(&g_GameFactionRuntimeImage.records[factionIndex],
                                                      resolvedAsset)) {
    return;
  }
  price = resolvedAsset[2].registryId;
  g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
       g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + ((int)(price * 7) >> 3);
  if ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex == factionIndex) {
    InGameArmyStock_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
}
