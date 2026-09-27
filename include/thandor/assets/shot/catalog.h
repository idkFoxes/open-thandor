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

/* Slots of g_ShotDefinitionRegistry (0x0052AF50, 256 pointers; a null slot is free). */
#define SHOT_DEFINITION_REGISTRY_SLOT_COUNT 256
/* Entries of ShotDefinition.terrainMaterialIndices31 (one per terrain-material reference of a shot). */
#define SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT 31
/* Entries of g_TerrainMaterialTextureSets (0x005039DC): valid terrain-material indices are 0..25. */
#ifndef TERRAIN_MATERIAL_COUNT
#define TERRAIN_MATERIAL_COUNT 26
#endif
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052B4D0 */
StatusResult __thandor_void_preserve_ecx_edx ShotAsset_PrepareEntries(ShotAssetHeader *asset);

/* 0x0052B7E0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinitions_ValidateTerrainMaterialReferences(void);

/* 0x0052B860 */
ShotDefinitionResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinitionRegistry_FindByIdWithError(PckShotDefinitionIdCatalog definitionId);

/* 0x0052B8C0 */
ShotLaunchAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
ShotDefinition_ComputeLaunchAnglesRegs
          (Q12 point0X,Q12 point0Y,Q12 point0Z,Q12 point1X,Q12 point1Y,Q12 point1Z,
          ShotDefinition *definition);

/* 0x0052BCE0 */
uint32_t ShotDefinition_ComputeSelectionRange(ShotDefinition *definition);

/* 0x0052BD50 */
ShotRangeLimitResult __thandor_ebx_cf_preserve_eax_ecx_edx
ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition);

/* 0x0052BD80 */
uint32_t __thandor_eax_preserve_ecx_edx
ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition);

/* 0x0052B350 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition);

#endif /* THANDOR_ASSETS_SHOT_CATALOG_H */
