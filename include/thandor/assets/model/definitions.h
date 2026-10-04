/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/model/definitions.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_MODEL_DEFINITIONS_H
#define THANDOR_ASSETS_MODEL_DEFINITIONS_H

#include <thandor/assets/model/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/core/contracts.h>

/* Linked model-definition ids of an army model-tree node (ArmyModelTreeNode.linkedDefinitionIds, +0x20):
   [0] is the default, the others are upgrade stages that need a technology. */
#define MODEL_LINKED_DEFINITION_COUNT 8
/* Key classes of a model resource's packed points (ModelPackedPointRecord.packedLookupKey = keyIndex << 4 |
   keyClass; ModelLookupTable_FindPackedPoint / GetPackedPointPosition). */
#define MODEL_POINT_CLASS_ATTACHMENT 0 /* child node attachment point, keyIndex = child index */
#define MODEL_POINT_CLASS_SHOT 2 /* shot launch point, keyIndex = weapon / emitter index */
#define MODEL_POINT_CLASS_EFFECT 3 /* effect spawn point (0 linked effect, 1 periodic effect) */
#define MODEL_POINT_CLASS_LIGHT 4 /* shading light position */

Bool8 ModelAsset_PrepareRecords(ModelAssetHeader *asset,uint32_t *outError);

Bool8 ModelLookupTable_GetPackedPointPosition
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,GraphicsFixedVec3 *outLocalPosition);

Bool8 ModelLookupTable_FindPackedPoint(ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,ModelPackedPointRecord **outEntry);

uint32_t ModelDefinitionRegistry_FindBuildCostsById
          (PckModelDefinitionIdCatalog definitionId,uint32_t *outEnergyLoadQ4,uint32_t *outBuildTicks,
           uint32_t *outXeniteCostQ4);

ModelDefinitionRecordPrefix * ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId);

Bool8 ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolveView *definition,ModelAssetHeader *asset,uint32_t *outError);

ModelDefinitionRecordPrefix *ModelDefinitionRegistry_LookupById(PckModelDefinitionIdCatalog definitionId);

ModelDefinitionRecordPrefix *ModelDefinitionRegistry_FindById(PckModelDefinitionIdCatalog definitionId);

extern ModelDefinitionRecordPrefix *g_ModelDefinitionRegistry[768];

#endif /* THANDOR_ASSETS_MODEL_DEFINITIONS_H */
