/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/technology.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_TECHNOLOGY_H
#define THANDOR_GAMEPLAY_AI_TECHNOLOGY_H

#include <thandor/assets/model/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/ai/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/technology. */

/* Score factor of AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue:
   score = (relationScaleQ8 * 40000 >> 8) * baseCandidateScore >> 8. */
#define AI_TECHNOLOGY_RELATION_SCORE_FACTOR 40000
/* Functions are grouped by semantic ownership. */

Bool8 AiTechnologyCandidate_IsCurrentlyAvailable
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          );

void AiTechnologyPlanning_AddCandidateRecord(ModelRuntimeSlot *sourceModelRuntime,PckTechnologyIdCatalog technologyId);

void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_TECHNOLOGY_H */
