#ifndef THANDOR_ASSETS_SHOT_CATALOG_H
#define THANDOR_ASSETS_SHOT_CATALOG_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/shot/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052B4D0 */
dword ShotAsset_PrepareEntries(ShotAssetHeader *asset);

/* 0x0052B7E0 */
dword __cdecl ShotDefinitions_ValidateTerrainMaterialReferences(void);

/* 0x0052B860 */
ShotDefinition * ShotDefinitionRegistry_FindByIdWithErrorCf(PckShotDefinitionIdCatalog definitionId);

/* 0x0052B8C0 */
undefined8 ShotDefinition_ComputeLaunchAnglesRegs (Q12 point0X,Q12 point0Y,Q12 point0Z,Q12 point1X,Q12 point1Y,Q12 point1Z, ShotDefinition *definition);

/* 0x0052BCE0 */
dword ShotDefinition_ComputeSelectionRange(ShotDefinition *definition);

/* 0x0052BD50 */
void ShotDefinition_GetModeRangeLimitEbx(ShotDefinition *definition);

/* 0x0052BD80 */
dword ShotDefinition_ComputeMode3LeadAdjustment(ShotDefinition *definition);

/* 0x0052B350 */
undefined8 ShotDefinition_RegisterAndResolveReferencesCf(ShotDefinition *definition);

#endif /* THANDOR_ASSETS_SHOT_CATALOG_H */
