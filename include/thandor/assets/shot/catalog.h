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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052B4D0 */
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx ShotAsset_PrepareEntries(ShotAssetHeader *asset);

/* 0x0052B7E0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotDefinitions_ValidateTerrainMaterialReferences(void);

/* 0x0052B860 */
ShotDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotDefinitionRegistry_FindByIdWithErrorCf(PckShotDefinitionIdCatalog definitionId);

/* 0x0052B8C0 */
ShotLaunchAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
ShotDefinition_ComputeLaunchAnglesRegs
          (Q12 point0X,Q12 point0Y,Q12 point0Z,Q12 point1X,Q12 point1Y,Q12 point1Z,
          ShotDefinition *definition);

/* 0x0052BCE0 */
dword ShotDefinition_ComputeSelectionRange(ShotDefinition *definition);

/* 0x0052BD50 */
ShotModeRangeLimitEbxCf5 __thandor_ebx_cf_preserve_eax_ecx_edx
ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition);

/* 0x0052BD80 */
dword __thandor_eax_preserve_ecx_edx
ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition);

/* 0x0052B350 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ShotDefinition_RegisterAndResolveReferencesCf(ShotDefinition *definition);

#endif /* THANDOR_ASSETS_SHOT_CATALOG_H */
