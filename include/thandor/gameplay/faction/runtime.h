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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0055F790 */
void GameFactionRuntime_AdvancePairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0055F910 */
void GameFactionRuntime_ResetPairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x00565320 */
void __fastcall OldUnitRuntime_RebuildScenarioReplayTables(void);

/* 0x005130B0 */
void __fastcall GameFactionRuntime_RebaseLoadedArmyReferences(void);

/* 0x00513960 */
void GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember);

/* 0x00513CA0 */
bool GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00513CD0 */
FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00513D70 */
void GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void);

/* 0x00514510 */
void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ArmyRuntimeSlot *targetArmyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00514730 */
void GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00514900 */
RuntimeGroupIndexResult GameFactionRuntime_FindRuntimeGroupIndex(RuntimeModelFactionPrefix *runtimeEntry);

/* 0x0051B800 */
bool FactionRuntime_HasArmyAssetOrActiveStructure
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x0051C4C0 */
void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime);

/* 0x0051C680 */
WorldPositionResult
GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState);

/* 0x0052A4D0 */
void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime);

/* 0x00560110 */
void GameFactionRuntime_RegisterArmyAssetPointers(uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560160 */
void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t unusedPlayerRuntimeId,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560400 */
void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

/* 0x00560620 */
void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex);

/* 0x005606A0 */
void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedPlayerRuntimeId,uint32_t unusedZero,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

/* 0x00561F80 */
void PlayerRuntime_CreatePlacementArmy(PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 worldXQ12,
          PlayerStateLookupValue1 worldYQ12,RuntimeToken armyAssetId);

/* 0x00561FF0 */
void PlayerRuntime_SetPlacementFaction(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacementFactionIndex placementFactionIndex);

/* 0x00562020 */
void PlayerRuntime_SetPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacedArmyToken armyToken);

/* 0x005622C0 */
void PlayerRuntime_ClearPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          uint32_t unusedZero2);

/* 0x00565590 */
void OldUnitRuntime_MergeMasksAndReplayRecords(void);

/* 0x00513D00 */
bool GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00565650 */
void OldUnitRuntime_ResetPendingTables(void);

/* 0x00513EE0 */
void GameFactionRuntime_ApplyPairwiseRelationTransition(FactionNotificationCodeBase activeFactionCodeForFirst,
          FactionNotificationCodeBase activeFactionCodeForSecond,
          FactionRelationStateNibble stateFirstTowardSecond,
          FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex,
          FactionRuntimeIndex secondFactionIndex);

#endif /* THANDOR_GAMEPLAY_FACTION_RUNTIME_H */
