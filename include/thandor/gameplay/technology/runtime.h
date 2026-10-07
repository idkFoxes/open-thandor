/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/technology/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H
#define THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H

#include <thandor/assets/model/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/ai/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/gameplay/technology/types.h>
#include <thandor/core/contracts.h>

/* The research bits ENTITY_RUNTIME_FLAG_RESEARCH_* are ArmyRuntimeFlags (gameplay/army/types.h). */
/* tech.tec holds 256 technology records; a faction's unlock mask has one bit per record (8 dwords). */
inline constexpr int TECHNOLOGY_RECORD_COUNT = 256;

void Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex);

bool Technology_IsUnlockedForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

bool Technology_IsAvailableForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

void Technology_ApplyRecordToEntity(PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity);

void TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();

extern TechnologyAsset *g_TechnologyAsset;

extern int32_t g_TechnologyCategoryMaximums[8];
extern int32_t g_AiArmyCandidateFlaggedDefinitionValueMaximum;
extern TechnologyCategoryMasks g_TechnologyCategoryMasks;

ModelDefinitionRecordPrefix *ModelDefinition_SelectFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uintptr_t linkedDefinitionList);

void ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

bool ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

PckModelDefinitionIdCatalog ModelDefinition_SelectFactionUnlockedLinkedId
          (FactionRuntimeIndex factionIndex,uintptr_t linkedDefinitionList);

void ModelDefinition_UnlockLinkedTechnologyForFaction
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId);

bool ModelDefinition_IsFactionTechnologyLocked
          (uint32_t *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId);

#endif /* THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H */
