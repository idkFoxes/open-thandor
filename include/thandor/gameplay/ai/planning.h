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

/* g_AiActiveGridMaskClasses[0..3] (AiPlanning_CollectActiveGridMaskClasses): an unused slot */
#define AI_GRID_MASK_CLASS_FREE 0xffffffff
/* Random terms of the strategic building choice: 14 bits of g_RandomGeneratorState.next added to each
   terrain score as a tie breaker (AiStrategicClass_SelectTerrainSuitedBuilding), and 7 bits added to each
   pressure score as jitter (AiStrategicClass_SelectPressureWeightedBuilding) */
#define AI_STRATEGIC_TIE_BREAK_MASK 0x3fff
#define AI_STRATEGIC_SCORE_JITTER_MASK 0x7f
/* Functions are grouped by semantic ownership. */

void AiFactionRuntime_RebuildPlanningCapacityState(void);

AiCandidateScore32 AiArmyCandidate_ComputeAverageCompatibleAssetScore (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex, ModelRuntimeClassId runtimeClassId);

void AiPlanning_CollectActiveGridMaskClasses(void);

void AiRuntime_DispatchFactionPlanningPhase(FactionRuntimeIndex factionIndex,InGameRuntimeRoot *inGameRoot);

Bool8 AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

Bool8 AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex);

void AiConstructionPlanner_PlaceTritiumStorageNearResourceSite(PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiArmyCandidate_AddBestDefenseAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiStrategicClass_AddArmsFactoriesStageBuilding(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiStrategicClass_AddWeightedClassCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiArmyCandidate_AddBestExplorationAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiArmyCandidate_AddBestAttackAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

Bool8 AiPurchaseCandidate_HasEligibleProducer
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

void AiPurchaseCandidate_ApplyToFaction(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

void AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex);

void AiStructureCandidate_AddResourceStorage
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiResourceCandidate_AddPowerPlant(FactionRuntimeIndex factionIndex);

AiStrategicClassSelection AiStrategicClass_SelectTerrainSuitedBuilding
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

AiStrategicClassSelection AiStrategicClass_SelectPressureWeightedBuilding
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

Bool8 AiFactionRuntime_TestPlanningCapacityExceeded(uint32_t additionalEnergyDemand,FactionRuntimeIndex factionIndex);

AiCandidateScore32 AiArmyCandidate_ComputeFactionWeightedScore
          (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRecord *armyAssetRecord);

extern uint32_t g_AiActiveGridMaskClasses[4];
extern ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime;
extern const AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15;

#endif /* THANDOR_GAMEPLAY_AI_PLANNING_H */
