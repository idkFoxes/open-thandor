#ifndef THANDOR_GAMEPLAY_AI_TECHNOLOGY_H
#define THANDOR_GAMEPLAY_AI_TECHNOLOGY_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/technology. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053BD80 */
AiTechnologyCandidateScore AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId, WorldRuntimeContext *worldRuntime);

/* 0x00538000 */
AiTechnologyPlanningLoopRegisterContinuityResult AiTechnologyCandidate_IsCurrentlyAvailableCf (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset );

/* 0x00538140 */
AiTechnologyPlanningLoopRegisterContinuityResult AiTechnologyPlanning_AddCandidateRecord (ArmyRuntimeSlot *sourceArmyRuntime,PckTechnologyIdCatalog technologyId);

/* 0x0053BCC0 */
AiTechnologyCandidateScore AiTechnologyScore_ComputeFactionScaledCandidateValue (void *knowledgeContextEdi,FactionRuntimeIndex factionIndex, PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

/* 0x0053BD60 */
AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind2 (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId, WorldRuntimeContext *worldRuntime);

/* 0x0053BEA0 */
AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind4 (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId, WorldRuntimeContext *worldRuntime);

/* 0x0053BEC0 */
AiTechnologyCandidateScore AiTechnologyScore_ComputeCategoryCompatibleCandidateValue (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex, PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

/* 0x0053BC00 */
ModelDefinitionRecordPrefix * AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition);

/* 0x0053BC20 */
UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8 (ModelDefinitionRecordPrefix *candidateDefinition);

#endif /* THANDOR_GAMEPLAY_AI_TECHNOLOGY_H */
