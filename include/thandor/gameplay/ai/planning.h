/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/planning.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_PLANNING_H
#define THANDOR_GAMEPLAY_AI_PLANNING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/planning. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00514350 */
void __fastcall AiFactionRuntime_RebuildPlanningCapacityState(void);

/* 0x0053BA50 */
AiCandidateScore32 AiArmyCandidate_ComputeAverageCompatibleAssetScore (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex, ModelRuntimeClassId runtimeClassId);

/* 0x005379E0 */
void AiPlanning_CollectActiveGridMaskClasses(void);

/* 0x0053C810 */
void AiRuntime_DispatchFactionPlanningPhase(FactionRuntimeIndex factionIndex,WorldRuntimeContext *inGameRuntime);

/* 0x00539070 */
bool AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x005378C0 */
bool AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex);

/* 0x005393F0 */
void AiConstructionPlanner_PlaceDerivedAsset14D(PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053A6E0 */
void AiArmyCandidate_AddBestScoredVariantA(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AC20 */
void AiStrategicClass_AddCandidate12DOr12FTo132(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B070 */
void AiStrategicClass_AddWeightedClassCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x005397C0 */
void AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053A800 */
void AiArmyCandidate_AddBestScoredVariantB(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053A8D0 */
void AiArmyCandidate_AddBestScoredVariantC(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537630 */
bool AiPurchaseCandidate_HasEligibleProducer
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

/* 0x00537800 */
void AiPurchaseCandidate_ApplyToFaction(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

/* 0x00539A40 */
void AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex);

/* 0x00539D20 */
void AiStructureCandidate_AddWeightedId14BOr14CCandidate
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539E60 */
void AiResourceCandidate_AddWeightedId136(FactionRuntimeIndex factionIndex);

/* 0x0053A9D0 */
AiStrategicClassSelectionRegs8 AiStrategicClass_SelectBestCandidate12ETo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AF00 */
AiStrategicClassSelectionRegs8 AiStrategicClass_SelectWeightedClass141To143
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539600 */
void AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x00539190 */
void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x0053A980 */
bool AiFactionRuntime_TestPlanningCapacityExceeded(uint32_t additionalEnergyDemand,FactionRuntimeIndex factionIndex);

/* 0x0053A2A0 */
AiCandidateScore32 AiArmyCandidate_ComputeFactionWeightedScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRecord *armyAssetRecord);

#endif /* THANDOR_GAMEPLAY_AI_PLANNING_H */
