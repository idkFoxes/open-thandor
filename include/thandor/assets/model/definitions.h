#ifndef THANDOR_ASSETS_MODEL_DEFINITIONS_H
#define THANDOR_ASSETS_MODEL_DEFINITIONS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/model/definitions. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051B3C0 */
ModelDefinitionRecordPrefix * ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

/* 0x0051DB00 */
void __fastcall ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology (int param_1,undefined4 param_2,FactionRuntimeIndex factionIndex, ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051DA60 */
ModelTechnologyHierarchyCfVolatileContinuityResult ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00528950 */
dword ModelAsset_PrepareRecords(ModelAssetHeader *asset);

/* 0x004BE670 */
undefined8 ModelLookupTable_FindPackedKeyEntryRegsCf (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass, ModelResourceHitTestAndRenderView210 *modelDefinition);

/* 0x004BE6F0 */
void ModelLookupTable_ContainsPackedKeyCf (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass, ModelResourceHitTestAndRenderView210 *modelDefinition);

/* 0x0050AEA0 */
Q12 ModelMesh_IntersectTriangleRayDistanceCf(ModelRaycastTriangleDescriptor *triangle);

/* 0x005289C0 */
void * ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(PckModelDefinitionIdCatalog definitionId);

/* 0x0053BA00 */
ModelDefinitionRecordPrefix * ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId);

/* 0x0051B430 */
PckModelDefinitionIdCatalog ModelDefinition_SelectFactionUnlockedLinkedIdCf (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

/* 0x00528600 */
undefined8 ModelDefinition_RegisterAndResolveReferencesCf (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset);

/* 0x0052ADE0 */
void ModelDefinition_UnlockLinkedTechnologyForFactionCf (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId);

/* 0x0052AD90 */
undefined4 ModelDefinition_IsFactionTechnologyUnlockedCf (dword *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId);

/* 0x00528E20 */
ModelDefinitionRecordPrefix * ModelDefinitionRegistry_FindByIdWithErrorCf(PckModelDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_MODEL_DEFINITIONS_H */
