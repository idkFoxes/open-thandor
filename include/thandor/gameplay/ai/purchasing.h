/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/purchasing.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_PURCHASING_H
#define THANDOR_GAMEPLAY_AI_PURCHASING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/purchasing. */

/* Functions are grouped by semantic ownership. */

AiCandidateScore32 AiArmyCandidate_ComputeAverageCompatibleAssetScore (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex, ModelRuntimeClassId runtimeClassId);

Bool8 AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex);

void AiArmyCandidate_AddBestDefenseAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiArmyCandidate_AddBestExplorationAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiArmyCandidate_AddBestAttackAsset(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

Bool8 AiPurchaseCandidate_HasEligibleProducer
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

void AiPurchaseCandidate_ApplyToFaction(AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

void AiStructureCandidate_AddResourceStorage
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiResourceCandidate_AddPowerPlant(FactionRuntimeIndex factionIndex);

AiCandidateScore32 AiArmyCandidate_ComputeFactionWeightedScore
          (const AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRecord *armyAssetRecord);

extern const AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15;

#endif /* THANDOR_GAMEPLAY_AI_PURCHASING_H */
