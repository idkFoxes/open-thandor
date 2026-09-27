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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0055F790 */
void __thandor_void_preserve_eax_ecx
GameFactionRuntime_AdvancePairwiseRelationState
          (uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0055F910 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ResetPairwiseRelationState
          (uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x00565320 */
void __fastcall OldUnitRuntime_RebuildScenarioReplayTables(void);

/* 0x005130B0 */
void __fastcall GameFactionRuntime_RebaseLoadedArmyReferences(void);

/* 0x00513960 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember);

/* 0x00513CA0 */
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRuntime_TestCapabilityBitClear
          (uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00513CD0 */
FactionRelationState __thandor_eax_preserve_ecx_edx
GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00513D70 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void);

/* 0x00514510 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ArmyRuntimeSlot *targetArmyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00514730 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00514900 */
RuntimeGroupIndexResult __thandor_eax_cf_preserve_ecx_edx
GameFactionRuntime_FindRuntimeGroupIndex(RuntimeModelFactionPrefix10 *runtimeEntry);

/* 0x0051B800 */
bool __thandor_cf_preserve_eax_ecx_edx
FactionRuntime_HasArmyAssetOrActiveStructure
          (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x0051C4C0 */
void __thandor_void_preserve_eax_ecx
GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime);

/* 0x0051C680 */
WorldPositionResult
GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState);

/* 0x0052A4D0 */
void __thandor_void_preserve_eax_ecx_edx
GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime);

/* 0x00560110 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RegisterArmyAssetPointers
          (uint32_t reservedDword0,FactionArmyAssetCount repetitionCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560160 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
          (uint32_t reservedDword0,FactionArmyAssetCount requestedCount,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560400 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedDword04,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

/* 0x00560620 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedConsumeArgument0,uint32_t unusedConsumeArgument1
          ,FactionRuntimeIndex factionIndex);

/* 0x005606A0 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
          (uint32_t unusedSaleArgument0,uint32_t unusedSaleArgument1,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex);

/* 0x00561F80 */
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_ResolveAndStoreState8094
          (PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 lookupValue0,
          PlayerStateLookupValue1 lookupValue1,RuntimeToken lookupToken);

/* 0x00561FF0 */
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_SetState8090
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          PlayerState8090Value stateValue);

/* 0x00562020 */
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_SetState8094
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          PlayerState8094Value stateValue);

/* 0x005622C0 */
void __thandor_void_preserve_eax_ecx_edx
PlayerRuntime_ClearState8094
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

/* 0x00565590 */
void __thandor_void_preserve_eax_ecx_edx OldUnitRuntime_MergeMasksAndReplayRecords(void);

/* 0x00513D00 */
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00565650 */
void __thandor_void_preserve_eax_ecx OldUnitRuntime_ResetPendingTables(void);

/* 0x00513EE0 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRuntime_ApplyPairwiseRelationTransition
          (FactionNotificationCodeBase activeFactionCodeForFirst,
          FactionNotificationCodeBase activeFactionCodeForSecond,
          FactionRelationStateNibble stateFirstTowardSecond,
          FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex,
          FactionRuntimeIndex secondFactionIndex);

#endif /* THANDOR_GAMEPLAY_FACTION_RUNTIME_H */
