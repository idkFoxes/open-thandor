#ifndef THANDOR_GAMEPLAY_FACTION_RUNTIME_H
#define THANDOR_GAMEPLAY_FACTION_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0055F790 */
void GameFactionRuntime_AdvancePairwiseRelationState (undefined4 unusedRelationArgument0,undefined4 unusedRelationArgument1, FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0055F910 */
void GameFactionRuntime_ResetPairwiseRelationState (undefined4 unusedRelationArgument0,undefined4 unusedRelationArgument1, FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x00565320 */
void __fastcall OldUnitRuntime_RebuildScenarioReplayTables(void);

/* 0x005130B0 */
void __fastcall GameFactionRuntime_RebaseLoadedArmyReferences(void);

/* 0x00513960 */
void GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(void *runtimeGroupMember);

/* 0x00513CA0 */
GameFactionCapabilityCfVolatileContinuityResult GameFactionRuntime_TestCapabilityBitClearCf (dword capabilityBitIndex,FactionRuntimeIndex factionIndex);

/* 0x00513CD0 */
FactionRelationState GameFactionRuntime_GetPackedStateNibble (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00513D70 */
undefined8 __cdecl GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void);

/* 0x00514510 */
void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify (ArmyRuntimeSlot *targetArmyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00514730 */
void GameFactionRuntime_RecomputeProgressAndScoreMetrics (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00514900 */
dword GameFactionRuntime_FindRuntimeGroupIndexCf(ArmyRuntimeSlot *runtimeGroupMember);

/* 0x0051B800 */
undefined8 FactionRuntime_HasArmyAssetOrActiveStructureCf (FactionRuntimeIndex factionIndex,ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x0051C4C0 */
void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(int entityRuntime);

/* 0x0051C680 */
void GameEntityRuntime_ResolveCommandTargetPositionCf(GameEntityRuntime *targetState);

/* 0x0052A4D0 */
void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex, ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime);

/* 0x00560110 */
void GameFactionRuntime_RegisterArmyAssetPointers (undefined4 reservedDword0,FactionArmyAssetCount repetitionCount, PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560160 */
void GameFactionRuntime_CancelQueuedArmyAssetsAndRefund (undefined4 reservedDword0,FactionArmyAssetCount requestedCount, PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560400 */
void GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer (PlayerRuntimeId playerRuntimeId,undefined4 reservedDword04, PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00560620 */
void GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid (PlayerRuntimeId playerRuntimeId,undefined4 unusedConsumeArgument0, undefined4 unusedConsumeArgument1,FactionRuntimeIndex factionIndex);

/* 0x005606A0 */
void GameFactionRuntime_SellArmyAssetAndRefundSevenEighths (undefined4 unusedSaleArgument0,undefined4 unusedSaleArgument1, PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x00561F80 */
undefined8 PlayerRuntime_ResolveAndStoreState8094 (PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 lookupValue0, PlayerStateLookupValue1 lookupValue1,RuntimeToken lookupToken);

/* 0x00561FF0 */
void PlayerRuntime_SetState8090 (PlayerRuntimeId playerRuntimeId,undefined4 param_2,undefined4 param_3, PlayerState8090Value stateValue);

/* 0x00562020 */
void PlayerRuntime_SetState8094 (PlayerRuntimeId playerRuntimeId,undefined4 param_2,undefined4 param_3, PlayerState8094Value stateValue);

/* 0x005622C0 */
void __fastcall PlayerRuntime_ClearState8094(undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex);

/* 0x00565590 */
void __cdecl OldUnitRuntime_MergeMasksAndReplayRecords(void);

/* 0x00513D00 */
void GameFactionRuntime_IsRecentTimedRelationStateCf (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

/* 0x00565650 */
undefined4 __cdecl OldUnitRuntime_ResetPendingTables(void);

/* 0x00513EE0 */
undefined8 GameFactionRuntime_ApplyPairwiseRelationTransition (FactionNotificationCodeBase activeFactionCodeForFirst, FactionNotificationCodeBase activeFactionCodeForSecond, FactionRelationStateNibble stateFirstTowardSecond, FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex, FactionRuntimeIndex secondFactionIndex);

#endif /* THANDOR_GAMEPLAY_FACTION_RUNTIME_H */
