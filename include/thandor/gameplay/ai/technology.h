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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053BD80 */
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x00538000 */
bool __thandor_cf_preserve_eax_ecx_edx
AiTechnologyCandidate_IsCurrentlyAvailableCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          );

/* 0x00538140 */
AiTechnologyPlanningLoopRegisterContinuityResult
AiTechnologyPlanning_AddCandidateRecord
          (dword technologyPanelIndex,dword sourceArmyEntriesRemaining,dword factionRecordOffset,
          ArmyRuntimeSlot *sourceArmyRuntime,PckTechnologyIdCatalog technologyId);

/* 0x0053BCC0 */
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeFactionScaledCandidateValue
          (AiKnowledgeDataImage *knowledgeData,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

/* 0x0053BD60 */
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ReturnBaseCandidateValueForKind2
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BEA0 */
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ReturnBaseCandidateValueForKind4
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BEC0 */
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

/* 0x0053BC00 */
bool __thandor_cf_preserve_eax_ecx_edx
AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf
          (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition);

/* 0x0053BC20 */
UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8 (ModelDefinitionRecordPrefix *candidateDefinition);

#endif /* THANDOR_GAMEPLAY_AI_TECHNOLOGY_H */
