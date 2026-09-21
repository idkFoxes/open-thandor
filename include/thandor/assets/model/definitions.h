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
ModelDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

/* 0x0051DB00 */
void __thandor_void_preserve_eax_ecx_edx
ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051DA60 */
bool __thandor_cf_preserve_eax_ecx_edx
ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00528950 */
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx ModelAsset_PrepareRecords(ModelAssetHeader *asset);

/* 0x004BE670 */
ModelLookupPayloadEaxEcxEdxCf13
ModelLookupTable_FindPackedKeyEntryRegsCf
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition);

/* 0x004BE6F0 */
ModelLookupEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelLookupTable_ContainsPackedKeyCf
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResourceHitTestAndRenderView210 *modelDefinition);

/* 0x0050AEA0 */
TerrainDistanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelMesh_IntersectTriangleRayDistanceCf(ModelRaycastTriangleDescriptor *triangle);

/* 0x005289C0 */
ModelBuildMetricEaxEcxEdxCf13
ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(PckModelDefinitionIdCatalog definitionId);

/* 0x0053BA00 */
ModelDefinitionRecordPrefix * ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId);

/* 0x0051B430 */
PckModelDefinitionIdCatalog __thandor_eax_preserve_ecx_edx
ModelDefinition_SelectFactionUnlockedLinkedIdCf
          (FactionRuntimeIndex factionIndex,ModelLinkedDefinitionListAddress32 linkedDefinitionList);

/* 0x00528600 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelDefinition_RegisterAndResolveReferencesCf
          (ModelDefinitionResolvePhaseView280 *definition,ModelAssetHeader *asset);

/* 0x0052ADE0 */
void __thandor_preserve_eax
ModelDefinition_UnlockLinkedTechnologyForFactionCf
          (FactionRuntimeIndex factionIndex,PckModelDefinitionIdCatalog modelDefinitionId);

/* 0x0052AD90 */
bool __thandor_cf_preserve_eax_ecx_edx
ModelDefinition_IsFactionTechnologyUnlockedCf
          (dword *factionTechnologyMasks,PckModelDefinitionIdCatalog modelDefinitionId);

/* 0x00528E20 */
ModelDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelDefinitionRegistry_FindByIdWithErrorCf(PckModelDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_MODEL_DEFINITIONS_H */
