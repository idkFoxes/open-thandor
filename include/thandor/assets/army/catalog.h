/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/army/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ARMY_CATALOG_H
#define THANDOR_ASSETS_ARMY_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/army/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005719F0 */
ArmyRegistryIdEaxCf5_5719f0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(PckArmyAssetIdCatalog recordId);

/* 0x00571A10 */
ArmyRegistryIdEaxCf5_571a10 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(ArmyAssetId recordId);

/* 0x00571A60 */
ArmyRegistryIdEaxCf5_571a60 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(ArmyAssetId recordId);

/* 0x00571B00 */
ArmyRegistryIdEaxCf5_571b00 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf(ArmyAssetId recordId);

/* 0x00571C30 */
ArmyRegistryIdEaxCf5_571c30 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(PckArmyAssetIdCatalog recordId);

/* 0x00571C50 */
ArmyRegistryIdEaxCf5_571c50 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlags0100And0200Cf(ArmyAssetId recordId);

/* 0x00571CA0 */
ArmyRegistryIdEaxCf5_571ca0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(ArmyAssetId recordId);

/* 0x00571D40 */
ArmyRegistryIdEaxCf5_571d40 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf(ArmyAssetId recordId);

/* 0x0051B5E0 */
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx ArmyAsset_PrepareRecords(ArmyAssetHeader *asset);

/* 0x0051B740 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_FindEnabledByIdCf(PckArmyAssetIdCatalog recordId);

/* 0x0051B770 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
          (FactionRuntimeIndex factionIndex,dword requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x00571E40 */
void __thandor_void_preserve_eax_ecx
ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(dword selectedArmyAssetRegistryId);

/* 0x0051C170 */
dword __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051C2C0 */
EnergyDemandQ4 __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00571AB0 */
ArmyRegistryIdEaxCf5_571ab0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(ArmyAssetId recordId);

/* 0x00571CF0 */
ArmyRegistryIdEaxCf5_571cf0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(ArmyAssetId recordId);

/* 0x0051B4A0 */
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx
ArmyAssetRecord_RegisterAndRelocate
          (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase);

/* 0x00571D90 */
dword ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(dword armyAssetRegistryId);

/* 0x0051B6D0 */
ArmyRegistryEaxCf5_51b6d0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindByIdCf(PckArmyAssetIdCatalog registryId);

/* 0x00571910 */
byte __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithoutFlag0200Cf(ArmyAssetId recordId);

/* 0x00571B50 */
byte __thandor_cf_preserve_eax_ecx_edx ArmyAssetRegistry_HasIdWithFlag0200Cf(ArmyAssetId recordId);

/* 0x00571980 */
byte __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(ArmyAssetId recordId);

/* 0x00571BC0 */
byte __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(ArmyAssetId recordId);

#endif /* THANDOR_ASSETS_ARMY_CATALOG_H */
