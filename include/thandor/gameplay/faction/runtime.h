/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_RUNTIME_H
#define THANDOR_GAMEPLAY_FACTION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/runtime. */

/* GameFactionRuntimeRecord army-asset lists (64 entries each): secondaryArmyAssetPointersOrIds is the production
   queue, primaryArmyAssetPointersOrIds the finished armies waiting for placement. */
#define FACTION_ARMY_ASSET_LIST_CAPACITY 64
/* GameFactionRuntime_IsRecentTimedRelationState: a pending relation state (2, 5, 9) does not advance again
   within this many simulation ticks of the pair's last change. */
#define FACTION_RELATION_CHANGE_COOLDOWN_TICKS 600
/* g_OldUnitPrimaryTable: 0x20-byte carry-over unit records (OLD_UNIT_PRIMARY_TABLE_BYTES / 0x20). */
#define OLD_UNIT_PRIMARY_RECORD_CAPACITY 0x200
/* Faction merge (relation state 11) without a clear survivor: this random bit clear = the second faction survives */
#define FACTION_MERGE_RANDOM_DIRECTION_BIT 0x2000
/* Functions are grouped by semantic ownership. */

void GameFactionRuntime_AdvancePairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void GameFactionRuntime_ResetPairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void OldUnitRuntime_RebuildScenarioReplayTables(void);

void GameFactionRuntime_RebaseLoadedArmyReferences(void);

void GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember);

Bool8 GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex);

FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void);

void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ModelRuntimeSlot *hitModelRuntime,WorldRuntimeContext *worldRuntime);

void GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

uint32_t GameFactionRuntime_FindRuntimeGroupNumber(RuntimeModelFactionPrefix *runtimeEntry);

Bool8 FactionRuntime_IsArmyAssetNotPending
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord);

void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime);

Bool8 GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState,FixedVectorQ12 *outPosition);

void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime);

void GameFactionRuntime_RegisterArmyAssetPointers(uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedPlayerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

void PlayerRuntime_CreatePlacementArmy(PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 worldXQ12,
          PlayerStateLookupValue1 worldYQ12,RuntimeToken armyAssetId);

void PlayerRuntime_SetPlacementFaction(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacementFactionIndex placementFactionIndex);

void PlayerRuntime_SetPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacedArmyToken armyToken);

void PlayerRuntime_ClearPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          uint32_t unusedZero2);

void OldUnitRuntime_MergeMasksAndReplayRecords(void);

Bool8 GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

void OldUnitRuntime_ResetPendingTables(void);

void GameFactionRuntime_ApplyPairwiseRelationTransition(FactionNotificationCodeBase activeFactionCodeForFirst,
          FactionNotificationCodeBase activeFactionCodeForSecond,
          FactionRelationStateNibble stateFirstTowardSecond,
          FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex,
          FactionRuntimeIndex secondFactionIndex);

extern GameFactionRuntimeImage g_GameFactionRuntimeImage;

extern uint32_t *g_OldUnitSecondaryTable;
extern uint32_t *g_OldUnitPrimaryTable;
extern OldUnitRecordCount g_OldUnitRecordCount; /* followed by 8 bytes 0x90 fill (dropped) */

#endif /* THANDOR_GAMEPLAY_FACTION_RUNTIME_H */
