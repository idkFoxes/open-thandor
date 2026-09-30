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

/* Pointer slots in g_ArmyAssetRecordRegistry (the global is declared as an array of 768). */
#define ARMY_ASSET_REGISTRY_SLOT_COUNT 768
/* Number of linked army-asset ids stored from record offset +0x30 (ArmyAssetRecord_HasFactionUnlockedLinkedDefinition). */
#define ARMY_ASSET_LINKED_ID_COUNT 16
/* Bits of an army record's flags dword (+0x14) that pick the map-editor placement lists: unit placement
   (g_UiCommandModeG 3, g_UiCommandModeGArmyAssetId) cycles records with 0x0100 set and 0x0200 clear, object
   placement (mode 4, g_UiCommandMode4ArmyAssetId) records with both set (ArmyAssetRegistry_HasNo*WithId). */
/* Bit 0 of the flags: the record is enabled (buildable); ArmyAssetRegistry_FindEnabledById. */
#define ARMY_ASSET_FLAG_ENABLED 0x1
#define ARMY_ASSET_FLAG_EDITOR_PLACEABLE 0x100
#define ARMY_ASSET_FLAG_EDITOR_OBJECT 0x200
/* The placement lists search ids 0..0xFFF and wrap around at 0x1000. */
#define ARMY_ASSET_EDITOR_ID_LIMIT 0x1000
/* Army ids from 400 up are rendered in their preview with faction 0 (ArmyAssetRegistry_ResolveOrCreatePreviewTexture). */
#define ARMY_ASSET_NEUTRAL_PREVIEW_FIRST_ID 400

/* A node of an army record's model tree (root at ArmyAssetRecordPrefix.rootNodeOffsetOrPointer, which is
   also the ModelLinkedDefinitionListAddress32 of ModelDefinition_SelectFactionUnlockedLinkedDefinition).
   The tree walkers index children[0..childCount-1]. */
typedef struct ArmyModelTreeNode {
    uint8_t unknown00_07[8];
    uint32_t childCount;                      /* +0x08 */
    struct ArmyModelTreeNode *children[5];    /* +0x0C */
    enum PckModelDefinitionIdCatalog linkedDefinitionIds[8]; /* +0x20 [0] default, others need a technology; 0 = none */
} ArmyModelTreeNode;
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005719F0 */
ArmyAssetId ArmyAssetRegistry_NormalizeIdToPlaceableUnit(PckArmyAssetIdCatalog recordId);

/* 0x00571A10 */
ArmyAssetId ArmyAssetRegistry_StepForwardPlaceableUnit(ArmyAssetId recordId);

/* 0x00571A60 */
ArmyAssetId ArmyAssetRegistry_StepBackwardPlaceableUnit(ArmyAssetId recordId);

/* 0x00571B00 */
ArmyAssetId ArmyAssetRegistry_FindPreviousPlaceableUnitWrapped(ArmyAssetId recordId);

/* 0x00571C30 */
ArmyAssetId ArmyAssetRegistry_NormalizeIdToPlaceableObject(PckArmyAssetIdCatalog recordId);

/* 0x00571C50 */
ArmyAssetId ArmyAssetRegistry_StepForwardPlaceableObject(ArmyAssetId recordId);

/* 0x00571CA0 */
ArmyAssetId ArmyAssetRegistry_StepBackwardPlaceableObject(ArmyAssetId recordId);

/* 0x00571D40 */
ArmyAssetId ArmyAssetRegistry_FindPreviousPlaceableObjectWrapped(ArmyAssetId recordId);

/* 0x0051B5E0 */
uint32_t ArmyAsset_PrepareRecords(ArmyAssetHeader *asset);

/* 0x0051B740 */
bool ArmyAssetRegistry_FindEnabledById(PckArmyAssetIdCatalog recordId);

/* 0x0051B770 */
bool ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord);

/* 0x00571E40 */
void ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(uint32_t uiRootAddress);

/* 0x0051C170 */
uint32_t ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x0051C2C0 */
EnergyDemandQ4 ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0x00571AB0 */
ArmyAssetId ArmyAssetRegistry_FindNextPlaceableUnitWrapped(ArmyAssetId recordId);

/* 0x00571CF0 */
ArmyAssetId ArmyAssetRegistry_FindNextPlaceableObjectWrapped(ArmyAssetId recordId);

/* 0x0051B4A0: 0 on success, otherwise a FATAL_ERROR_* code */
uint32_t ArmyAssetRecord_RegisterAndRelocate(ArmyAssetRecord *record,ArmyAssetHeader *assetBase);

/* 0x00571D90 */
uint32_t ArmyAssetRegistry_ResolveOrCreatePreviewTexture(uint32_t armyAssetRegistryId);

/* 0x0051B6D0: 0 and the record in *outRecord, or FATAL_ERROR_ARMY_ID_NOT_FOUND (then *outRecord holds that code
   cast to a pointer, see the definition) */
uint32_t ArmyAssetRegistry_FindById(PckArmyAssetIdCatalog registryId,ArmyAssetRecordPrefix **outRecord);

/* 0x00571910 */
uint8_t ArmyAssetRegistry_HasNoUnitWithId(ArmyAssetId recordId);

/* 0x00571B50 */
uint8_t ArmyAssetRegistry_HasNoObjectWithId(ArmyAssetId recordId);

/* 0x00571980 */
uint8_t ArmyAssetRegistry_HasNoPlaceableUnitWithId(ArmyAssetId recordId);

/* 0x00571BC0 */
uint8_t ArmyAssetRegistry_HasNoPlaceableObjectWithId(ArmyAssetId recordId);

#endif /* THANDOR_ASSETS_ARMY_CATALOG_H */
