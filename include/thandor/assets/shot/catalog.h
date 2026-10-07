/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/shot/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SHOT_CATALOG_H
#define THANDOR_ASSETS_SHOT_CATALOG_H

#include <thandor/assets/shot/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/core/contracts.h>

/* Slots of g_ShotDefinitionRegistry (256 pointers; a null slot is free). */
inline constexpr int SHOT_DEFINITION_REGISTRY_SLOT_COUNT = 256;
/* Entries of ShotDefinition.terrainMaterialIndices31 (one per terrain-material reference of a shot). */
inline constexpr int SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT = 31;
/* Entries of ShotDefinition.targetClassImpactEffectDefinitions8 / targetClassImpactDamageQ12. */
inline constexpr int SHOT_TARGET_CLASS_IMPACT_COUNT = 8;
/* Entries of g_TerrainMaterialTextureSets: valid terrain-material indices are 0..25. */
inline constexpr int TERRAIN_MATERIAL_COUNT = 26;

uint32_t ShotAsset_PrepareEntries(ShotAssetHeader *asset);

uint32_t ShotDefinitions_ValidateTerrainMaterialReferences();

ShotDefinition *ShotDefinitionRegistry_LookupById(PckShotDefinitionIdCatalog definitionId);

uint32_t ShotDefinitionRegistry_FindByIdWithError
          (PckShotDefinitionIdCatalog definitionId,ShotDefinition **outDefinition);

uint32_t ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition);

extern ShotDefinition *g_ShotDefinitionRegistry[256];

#endif /* THANDOR_ASSETS_SHOT_CATALOG_H */
