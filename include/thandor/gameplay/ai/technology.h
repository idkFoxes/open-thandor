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
   score = (relationScaleQ8 * 40000 >> 8) * baseCandidateScore >> 8 (IMUL EAX,EAX,0x9C40 at 0x0053BDE5). */
#define AI_TECHNOLOGY_RELATION_SCORE_FACTOR 40000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053BD80 */
AiTechnologyCandidateScore AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x00538000 */
bool AiTechnologyCandidate_IsCurrentlyAvailable
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          );

/* 0x00538140 */
void AiTechnologyPlanning_AddCandidateRecord(ModelRuntimeSlot *sourceModelRuntime,PckTechnologyIdCatalog technologyId);

/* 0x0053BCC0 */
AiTechnologyCandidateScore AiTechnologyScore_ComputeFactionScaledCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BD60 */
AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind2
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BEA0 */
AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind4
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BEC0 */
AiTechnologyCandidateScore AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* Extra input of the category score callback: the category mask the only caller
   (AiTechnologyCandidate_AddBestResearch) computes before its loop. */
extern AiTechnologyCategoryMask g_AiTechnologyScoreCategoryMask;

/* 0x0053BC00 */
bool AiTechnologyCompatibility_AcceptRuntimeClassCandidate
          (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition);

/* 0x0053BC20 */
UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8 (ModelDefinitionRecordPrefix *candidateDefinition);

#endif /* THANDOR_GAMEPLAY_AI_TECHNOLOGY_H */
