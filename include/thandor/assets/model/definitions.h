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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051B3C0 */
ModelDefinitionResult ModelDefinition_SelectFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

/* 0x0051DB00 */
void ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051DA60 */
bool ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00528950 */
StatusResult ModelAsset_PrepareRecords(ModelAssetHeader *asset);

/* 0x004BE670 */
ModelLookupPayloadResult
ModelLookupTable_FindPackedKeyEntryRegs
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition);

/* 0x004BE6F0 */
ModelLookupEntryResult ModelLookupTable_ContainsPackedKey(ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition);

/* 0x0050AEA0 */
MeshRayTriangleResult ModelMesh_IntersectTriangleRayDistance(ModelRaycastTriangleDescriptor *triangle);

/* 0x005289C0 */
BuildMetricResult
ModelDefinitionRegistry_FindBuildMetricTupleById(PckModelDefinitionIdCatalog definitionId);

/* 0x0053BA00 */
ModelDefinitionRecordPrefix * ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId);

/* 0x0051B430 */
PckModelDefinitionIdCatalog ModelDefinition_SelectFactionUnlockedLinkedId
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

/* 0x00528600 */
StatusResult ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset);

/* 0x0052ADE0 */
void ModelDefinition_UnlockLinkedTechnologyForFaction
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId);

/* 0x0052AD90 */
bool ModelDefinition_IsFactionTechnologyUnlocked
          (uint32_t *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId);

/* 0x00528E20 */
ModelDefinitionResult ModelDefinitionRegistry_FindByIdWithError(PckModelDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_MODEL_DEFINITIONS_H */
