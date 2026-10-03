/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/shot/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SHOT_CATALOG_H
#define THANDOR_ASSETS_SHOT_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/shot/catalog. */

/* Slots of g_ShotDefinitionRegistry (256 pointers; a null slot is free). */
#define SHOT_DEFINITION_REGISTRY_SLOT_COUNT 256
/* Entries of ShotDefinition.terrainMaterialIndices31 (one per terrain-material reference of a shot). */
#define SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT 31
/* Entries of ShotDefinition.targetClassImpactEffectDefinitions8 / targetClassImpactDamageQ12. */
#define SHOT_TARGET_CLASS_IMPACT_COUNT 8
/* Entries of g_TerrainMaterialTextureSets: valid terrain-material indices are 0..25. */
#ifndef TERRAIN_MATERIAL_COUNT
#define TERRAIN_MATERIAL_COUNT 26
#endif
/* Functions are grouped by semantic ownership. */

uint32_t ShotAsset_PrepareEntries(ShotAssetHeader *asset);

uint32_t ShotDefinitions_ValidateTerrainMaterialReferences(void);

uint32_t ShotDefinitionRegistry_FindByIdWithError
          (PckShotDefinitionIdCatalog definitionId,ShotDefinition **outDefinition);

ShotLaunchAngles ShotDefinition_ComputeLaunchAngles
          (Q12 targetZ,Q12 targetY,Q12 targetX,Q12 launchZ,Q12 launchY,Q12 launchX,
          ShotDefinition *definition);

uint32_t ShotDefinition_ComputeSelectionRange(ShotDefinition *definition);

Q12 ShotDefinition_GetLeadSpeed(ShotDefinition *definition);

uint32_t ShotDefinition_ComputeRampUpLeadTime(ShotDefinition *definition);

uint32_t ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition);

#endif /* THANDOR_ASSETS_SHOT_CATALOG_H */
