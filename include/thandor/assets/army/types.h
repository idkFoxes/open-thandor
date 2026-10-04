/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/army/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ARMY_TYPES_H
#define THANDOR_ASSETS_ARMY_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct ArmyAssetRecordPrefix ArmyAssetRecordPrefix, *PArmyAssetRecordPrefix;
typedef struct ArmyAssetRecord ArmyAssetRecord, *PArmyAssetRecord;
typedef struct ArmyAssetHeader ArmyAssetHeader, *PArmyAssetHeader;
typedef struct GeneratedAssetRecordCountHeader GeneratedAssetRecordCountHeader, *PGeneratedAssetRecordCountHeader;

typedef uint32_t AssetRecordByteCount;

typedef uint32_t AssetRecordCount;

typedef uint32_t ArmySelectionDetailTemplateVariantIndex;

typedef uint32_t EnergyDemandQ4;

struct ArmyAssetRecordPrefix {
    AssetRecordByteCount byteSize; 
    ArmySelectionDetailTemplateVariantIndex selectionDetailTemplateVariantIndex; 
    PckArmyAssetIdCatalog registryId; 
    uint32_t rootNodeOffsetOrPointer; 
};

struct ArmyAssetRecord {
    AssetRecordByteCount byteSize;
    ArmySelectionDetailTemplateVariantIndex selectionDetailTemplateVariantIndex;
    PckArmyAssetIdCatalog registryId;
    uint32_t rootNodeOffsetOrPointer;
    Ptr32<void> linkedRuntimeOrRecord10;
    uint32_t flags; /* +0x14 ARMY_ASSET_FLAG_*: 1 enabled/buildable, 0x100 editor-placeable, 0x200 editor object */
    uint8_t reserved018_01B[4];
    uint32_t selectionDetailValue; /* +0x1C copied to InGameRuntimeRoot.selectionDetailArmyAssetValue */
    uint32_t previewTexture; /* +0x20 cached editor preview texture (ArmyAssetRegistry_ResolveOrCreatePreviewTexture), 0 = none */
    /* +0x24..+0x2C: sums over the model tree's definitions, added up by ArmyAssetRecord_RegisterAndRelocate */
    uint32_t buildTicks; /* +0x24 ModelDefinition.buildTicks */
    uint32_t xeniteCostQ4; /* +0x28 ModelDefinition.xeniteValueQ4 */
    uint32_t energyLoadQ4; /* +0x2C ModelDefinition.buildEnergyLoadQ4; added to the builder's energyLoadQ4 while building */
    PckArmyAssetIdCatalog linkedArmyAssetIds[16]; /* +0x30 ARMY_ASSET_LINKED_ID_COUNT linked army ids, 0 = none */
    uint32_t definitionClassValue70; 
    uint32_t definitionClassValue74; 
    uint32_t definitionClassValue78; 
    uint8_t reserved07C_07F[4]; 
};

typedef uint32_t ArmyAssetId;

struct GeneratedAssetRecordCountHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount recordCount;
};

struct ArmyAssetHeader {
    struct GeneratedAssetRecordCountHeader recordCountHeader;
    uint8_t reservedB4_1FF[332];
};

#endif /* THANDOR_ASSETS_ARMY_TYPES_H */
