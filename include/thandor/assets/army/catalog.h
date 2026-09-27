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
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlag0100Without0200(PckArmyAssetIdCatalog recordId);

/* 0x00571A10 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlag0100Without0200(ArmyAssetId recordId);

/* 0x00571A60 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlag0100Without0200(ArmyAssetId recordId);

/* 0x00571B00 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlag0100Without0200Wrapped(ArmyAssetId recordId);

/* 0x00571C30 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlags0100And0200(PckArmyAssetIdCatalog recordId);

/* 0x00571C50 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlags0100And0200(ArmyAssetId recordId);

/* 0x00571CA0 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlags0100And0200(ArmyAssetId recordId);

/* 0x00571D40 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlags0100And0200Wrapped(ArmyAssetId recordId);

/* 0x0051B5E0 */
StatusResult __thandor_void_preserve_ecx_edx ArmyAsset_PrepareRecords(ArmyAssetHeader *asset);

/* 0x0051B740 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_FindEnabledById(PckArmyAssetIdCatalog recordId);

/* 0x0051B770 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x00571E40 */
void __thandor_void_preserve_eax_ecx
ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(uint32_t selectedArmyAssetRegistryId);

/* 0x0051C170 */
uint32_t __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051C2C0 */
EnergyDemandQ4 __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00571AB0 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlag0100Without0200Wrapped(ArmyAssetId recordId);

/* 0x00571CF0 */
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlags0100And0200Wrapped(ArmyAssetId recordId);

/* 0x0051B4A0 */
StatusResult __thandor_void_preserve_ecx_edx
ArmyAssetRecord_RegisterAndRelocate
          (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase);

/* 0x00571D90 */
uint32_t ArmyAssetRegistry_ResolveOrCreatePreviewTexture(uint32_t armyAssetRegistryId);

/* 0x0051B6D0 */
ArmyAssetLookupResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindById(PckArmyAssetIdCatalog registryId);

/* 0x00571910 */
uint8_t __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithoutFlag0200(ArmyAssetId recordId);

/* 0x00571B50 */
uint8_t __thandor_cf_preserve_eax_ecx_edx ArmyAssetRegistry_HasIdWithFlag0200(ArmyAssetId recordId);

/* 0x00571980 */
uint8_t __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithFlag0100Without0200(ArmyAssetId recordId);

/* 0x00571BC0 */
uint8_t __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithFlags0100And0200(ArmyAssetId recordId);

#endif /* THANDOR_ASSETS_ARMY_CATALOG_H */
