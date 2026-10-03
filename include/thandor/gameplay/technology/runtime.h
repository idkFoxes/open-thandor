/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/technology/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H
#define THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/technology/runtime. */

/* GameEntityRuntime.common.runtimeFlags research bits. Technology_IsAvailableForFaction treats an army with
   RESEARCH_RUNNING whose researchTechnologyId holds the technology as already researching it;
   Technology_ApplyRecordToEntity stores the technology in common.commandState (the same slot) and sets
   RESEARCH_ASSIGNED, and does nothing while either bit is set. */
#define ENTITY_RUNTIME_FLAG_RESEARCH_RUNNING 0x40
#define ENTITY_RUNTIME_FLAG_RESEARCH_ASSIGNED 0x80
/* tech.tec holds 256 technology records; a faction's unlock mask has one bit per record (8 dwords). */
#define TECHNOLOGY_RECORD_COUNT 256
/* 1.0 in Q24: dividend of g_TechnologyCategoryMaximumReciprocalQ24Table8 (1 / category maximum) */
#define TECHNOLOGY_RECIPROCAL_Q24_ONE 0x1000000u
/* Functions are grouped by semantic ownership. */

void Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex);

Bool8 Technology_IsUnlockedForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

Bool8 Technology_IsAvailableForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

void Technology_ApplyRecordToEntity(PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity);

void TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void);

extern TechnologyAsset *g_TechnologyAsset;

extern int32_t g_TechnologyCategoryMaximums[8];
extern int32_t g_AiArmyCandidateFlaggedDefinitionValueMaximum;
extern TechnologyCategoryMasks g_TechnologyCategoryMasks;

#endif /* THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H */
