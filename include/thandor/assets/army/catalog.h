#ifndef THANDOR_ASSETS_ARMY_CATALOG_H
#define THANDOR_ASSETS_ARMY_CATALOG_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/army/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005719F0 */
void ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(PckArmyAssetIdCatalog recordId);

/* 0x00571A10 */
void ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(ArmyAssetId recordId);

/* 0x00571A60 */
void ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(ArmyAssetId recordId);

/* 0x00571B00 */
void ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf(ArmyAssetId recordId);

/* 0x00571C30 */
void ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(PckArmyAssetIdCatalog recordId);

/* 0x00571C50 */
void ArmyAssetRegistry_StepForwardFlags0100And0200Cf(ArmyAssetId recordId);

/* 0x00571CA0 */
void ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(ArmyAssetId recordId);

/* 0x00571D40 */
void ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf(ArmyAssetId recordId);

/* 0x0051B5E0 */
dword ArmyAsset_PrepareRecords(ArmyAssetHeader *asset);

/* 0x0051B740 */
undefined4 ArmyAssetRegistry_FindEnabledByIdCf(PckArmyAssetIdCatalog recordId);

/* 0x0051B770 */
void ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf (FactionRuntimeIndex factionIndex,undefined4 param_2, ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x00571E40 */
void ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(dword selectedArmyAssetRegistryId);

/* 0x0051C170 */
ArmyUnlockedArmourAggregateEaxPreservedEdxCarrier64 ArmyAssetHierarchy_SumFactionUnlockedArmour (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051C2C0 */
EnergyQ4AggregateEaxPreservedEdxCarrier64 ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4 (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00571AB0 */
void ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(ArmyAssetId recordId);

/* 0x00571CF0 */
void ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(ArmyAssetId recordId);

/* 0x0051B4A0 */
longlong ArmyAssetRecord_RegisterAndRelocate (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase);

/* 0x00571D90 */
dword ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(dword armyAssetRegistryId);

/* 0x0051B6D0 */
ArmyAssetRecordPrefix * ArmyAssetRegistry_FindByIdCf(PckArmyAssetIdCatalog registryId);

/* 0x00571910 */
void ArmyAssetRegistry_HasIdWithoutFlag0200Cf(ArmyAssetId recordId);

/* 0x00571B50 */
void ArmyAssetRegistry_HasIdWithFlag0200Cf(ArmyAssetId recordId);

/* 0x00571980 */
void ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(ArmyAssetId recordId);

/* 0x00571BC0 */
void ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(ArmyAssetId recordId);

#endif /* THANDOR_ASSETS_ARMY_CATALOG_H */
