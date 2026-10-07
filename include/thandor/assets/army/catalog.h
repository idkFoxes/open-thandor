/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/army/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ARMY_CATALOG_H
#define THANDOR_ASSETS_ARMY_CATALOG_H

#include <thandor/assets/army/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/core/contracts.h>

/* Pointer slots in g_ArmyAssetRecordRegistry (the global is declared as an array of 768). */
inline constexpr int ARMY_ASSET_REGISTRY_SLOT_COUNT = 768;
/* Length of ArmyAssetRecord.linkedArmyAssetIds (ArmyAssetRecord_HasFactionUnlockedLinkedDefinition). */
inline constexpr int ARMY_ASSET_LINKED_ID_COUNT = 16;
/* Bits of an army record's flags (ArmyAssetRecord.flags) that pick the map-editor placement lists: unit placement
   (g_UiCommandModeG 3, g_UiCommandModeGArmyAssetId) cycles records with 0x0100 set and 0x0200 clear, object
   placement (mode 4, g_UiCommandMode4ArmyAssetId) records with both set (ArmyAssetRegistry_HasNo*WithId). */
/* Bit 0 of the flags: the record is enabled (buildable); ArmyAssetRegistry_FindEnabledById. */
inline constexpr int ARMY_ASSET_FLAG_ENABLED = 0x1;
inline constexpr int ARMY_ASSET_FLAG_EDITOR_PLACEABLE = 0x100;
inline constexpr int ARMY_ASSET_FLAG_EDITOR_OBJECT = 0x200;

/* A node of an army record's model tree (root at ArmyAssetRecordPrefix.rootNodeOffsetOrPointer, which is
   also the ModelLinkedDefinitionListAddress32 of ModelDefinition_SelectFactionUnlockedLinkedDefinition).
   The tree walkers index children[0..childCount-1]. */
typedef struct ArmyModelTreeNode {
    uint8_t unknown00_07[8];
    uint32_t childCount;                      /* +0x08 */
    Ptr32<struct ArmyModelTreeNode> children[5]; /* +0x0C */
    PckModelDefinitionIdCatalog linkedDefinitionIds[8]; /* +0x20 [0] default, others need a technology; 0 = none */
} ArmyModelTreeNode;

/* Weapon slots of a unit: the root's children[i] of the model tree, created in that order as the chassis model's
   ModelRuntimeSlot.attachments[i] (ModelNodeRuntime_InstantiateLinkedChildrenRecursive). In the stock unit data
   (docs/reference/army_units.md, tools/data/army_table.py) slot 0 holds the primary weapon and slot 1 the
   secondary one (walkers and gliders: the lighter weapon or a second copy); no unit fills a third slot, which only
   the selection panel reads. Buildings use the same slots for turrets (the Kraftwerk for four non-weapon parts). */
inline constexpr int ARMY_WEAPON_SLOT_PRIMARY = 0;
inline constexpr int ARMY_WEAPON_SLOT_SECONDARY = 1;
inline constexpr int ARMY_WEAPON_SLOT_TERTIARY = 2;

uint32_t ArmyAsset_PrepareRecords(ArmyAssetHeader *asset,uint32_t assetByteCount);

Bool8 ArmyAssetRegistry_FindEnabledById(PckArmyAssetIdCatalog recordId);

Bool8 ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord);

uint32_t ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

EnergyDemandQ4 ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode);

/* 0 on success, otherwise a FATAL_ERROR_* code */
uint32_t ArmyAssetRecord_RegisterAndRelocate(ArmyAssetRecord *record,ArmyAssetHeader *assetBase);

/* 0 and the record in *outRecord, or FATAL_ERROR_ARMY_ID_NOT_FOUND (then *outRecord holds that code
   cast to a pointer, see the definition) */
ArmyAssetRecordPrefix *ArmyAssetRegistry_FindRecordById(PckArmyAssetIdCatalog registryId);

uint32_t ArmyAssetRegistry_FindById(PckArmyAssetIdCatalog registryId,ArmyAssetRecordPrefix **outRecord);

uint8_t ArmyAssetRegistry_HasNoUnitWithId(ArmyAssetId recordId);

uint8_t ArmyAssetRegistry_HasNoObjectWithId(ArmyAssetId recordId);

uint8_t ArmyAssetRegistry_HasNoPlaceableUnitWithId(ArmyAssetId recordId);

uint8_t ArmyAssetRegistry_HasNoPlaceableObjectWithId(ArmyAssetId recordId);

extern ArmyAssetRecordPrefix *g_ArmyAssetRecordRegistry[768];

#endif /* THANDOR_ASSETS_ARMY_CATALOG_H */
