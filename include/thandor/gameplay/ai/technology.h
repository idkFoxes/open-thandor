/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/technology.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_TECHNOLOGY_H
#define THANDOR_GAMEPLAY_AI_TECHNOLOGY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/technology. */

/* Score factor of AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue:
   score = (relationScaleQ8 * 40000 >> 8) * baseCandidateScore >> 8. */
#define AI_TECHNOLOGY_RELATION_SCORE_FACTOR 40000
/* Functions are grouped by semantic ownership. */

AiTechnologyCandidateScore AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

Bool8 AiTechnologyCandidate_IsCurrentlyAvailable
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          );

void AiTechnologyPlanning_AddCandidateRecord(ModelRuntimeSlot *sourceModelRuntime,PckTechnologyIdCatalog technologyId);

AiTechnologyCandidateScore AiTechnologyScore_ComputeFactionScaledCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind2
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind4
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

AiTechnologyCandidateScore AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* Extra input of the category score callback: the category mask the only caller
   (AiTechnologyCandidate_AddBestResearch) computes before its loop. */
extern AiTechnologyCategoryMask g_AiTechnologyScoreCategoryMask;

Bool8 AiTechnologyCompatibility_AcceptRuntimeClassCandidate
          (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition);

UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8 (ModelDefinitionRecordPrefix *candidateDefinition);

extern AiTechnologyCandidateScoreCallback *g_AiTechnologyCandidateScoreCallbackTable[6];

void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

AiTechnologyCandidateScore AiTechnologyScore_AlwaysZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_TECHNOLOGY_H */
