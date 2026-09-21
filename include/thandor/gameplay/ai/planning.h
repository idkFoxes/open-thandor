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
void __thandor_void_preserve_eax_ecx_edx AiPlanning_CollectActiveGridMaskClasses(void);

/* 0x0053C810 */
void __thandor_void_preserve_eax_ecx_edx
AiRuntime_DispatchFactionPlanningPhase
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *inGameRuntime);

/* 0x00539070 */
bool __thandor_cf_preserve_eax_ecx_edx
AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x005378C0 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPurchasePlanner_ExecuteAffordableCandidates(FactionRuntimeIndex factionIndex);

/* 0x005393F0 */
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceDerivedAsset14D
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053A6E0 */
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantA
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AC20 */
void __thandor_void_preserve_eax_ecx_edx
AiStrategicClass_AddCandidate12DOr12FTo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B070 */
void __thandor_void_preserve_eax_ecx_edx
AiStrategicClass_AddWeightedClassCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x005397C0 */
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053A800 */
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantB
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053A8D0 */
void __thandor_void_preserve_eax_ecx_edx
AiArmyCandidate_AddBestScoredVariantC
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537630 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPurchaseCandidate_HasEligibleProducerCf
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

/* 0x00537800 */
void __thandor_void_preserve_eax_ecx_edx
AiPurchaseCandidate_ApplyToFaction
          (AiCandidateWorkspaceEntry *candidateEntry,FactionRuntimeIndex factionIndex);

/* 0x00539A40 */
void __thandor_void_preserve_eax_ecx_edx
AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex);

/* 0x00539D20 */
void __thandor_void_preserve_eax_ecx_edx
AiStructureCandidate_AddWeightedId14BOr14CCandidate
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog candidateArmyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539E60 */
void __thandor_void_preserve_eax_ecx_edx
AiResourceCandidate_AddWeightedId136(FactionRuntimeIndex factionIndex);

/* 0x0053A9D0 */
AiStrategicClassSelectionRegs8 __thandor_regs_ebx_ecx_preserve_eax_edx
AiStrategicClass_SelectBestCandidate12FTo132
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AF00 */
AiStrategicClassSelectionRegs8 __thandor_regs_ebx_ecx_preserve_eax_edx
AiStrategicClass_SelectWeightedClass141To143
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539600 */
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x00539190 */
void __thandor_void_preserve_eax_ecx
AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex);

/* 0x0053A980 */
bool __thandor_cf_preserve_eax_ecx_edx
AiFactionRuntime_TestPlanningCapacityExceededCf
          (dword additionalPlanningCapacity,FactionRuntimeIndex factionIndex);

/* 0x0053A2A0 */
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiArmyCandidate_ComputeFactionWeightedScore
          (AiArmyScoreWeights *scoreWeights,FactionRuntimeIndex factionIndex,
          ArmyAssetRuntimeSemanticView80 *armyAssetRecord);

#endif /* THANDOR_GAMEPLAY_AI_PLANNING_H */
