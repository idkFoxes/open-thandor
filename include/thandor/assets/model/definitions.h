/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/model/definitions.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_MODEL_DEFINITIONS_H
#define THANDOR_ASSETS_MODEL_DEFINITIONS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/model/definitions. */

/* Linked model-definition ids of an army model-tree node (ArmyModelTreeNode.linkedDefinitionIds, +0x20):
   [0] is the default, the others are upgrade stages that need a technology. */
#define MODEL_LINKED_DEFINITION_COUNT 8
/* Key classes of a model resource's packed points (ModelPackedPointRecord.packedLookupKey = keyIndex << 4 |
   keyClass; ModelLookupTable_FindPackedPoint / GetPackedPointPosition). */
#define MODEL_POINT_CLASS_ATTACHMENT 0 /* child node attachment point, keyIndex = child index */
#define MODEL_POINT_CLASS_SHOT 2 /* shot launch point, keyIndex = weapon / emitter index */
#define MODEL_POINT_CLASS_EFFECT 3 /* effect spawn point (0 linked effect, 1 periodic effect) */
#define MODEL_POINT_CLASS_LIGHT 4 /* shading light position */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

ModelDefinitionRecordPrefix *ModelDefinition_SelectFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

void ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

bool ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

bool ModelAsset_PrepareRecords(ModelAssetHeader *asset,uint32_t *outError);

bool ModelLookupTable_GetPackedPointPosition
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,GraphicsFixedVec3 *outLocalPosition);

bool ModelLookupTable_FindPackedPoint(ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,ModelPackedPointRecord **outEntry);

bool ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle,Q12 *outDistanceQ12);

uint32_t ModelDefinitionRegistry_FindBuildCostsById
          (PckModelDefinitionIdCatalog definitionId,uint32_t *outEnergyLoadQ4,uint32_t *outBuildTicks,
           uint32_t *outXeniteCostQ4);

ModelDefinitionRecordPrefix * ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId);

PckModelDefinitionIdCatalog ModelDefinition_SelectFactionUnlockedLinkedId
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

bool ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolveView *definition,ModelAssetHeader *asset,uint32_t *outError);

void ModelDefinition_UnlockLinkedTechnologyForFaction
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId);

bool ModelDefinition_IsFactionTechnologyLocked
          (uint32_t *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId);

ModelDefinitionRecordPrefix *ModelDefinitionRegistry_FindById(PckModelDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_MODEL_DEFINITIONS_H */
