/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/rom/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ROM_TYPES_H
#define THANDOR_ASSETS_ROM_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct WorldRuntimeNodeCommon WorldRuntimeNodeCommon, *PWorldRuntimeNodeCommon;
typedef struct WorldRuntimeNodeModelPayload WorldRuntimeNodeModelPayload, *PWorldRuntimeNodeModelPayload;
typedef struct WorldRuntimeNode WorldRuntimeNode, *PWorldRuntimeNode;
typedef union WorldRuntimeNodePayload WorldRuntimeNodePayload, *PWorldRuntimeNodePayload;
typedef struct RomAssetHeader RomAssetHeader, *PRomAssetHeader;
typedef struct RomRegistrySlot RomRegistrySlot, *PRomRegistrySlot;
typedef struct RomAssetRecordPrefix RomAssetRecordPrefix, *PRomAssetRecordPrefix;
typedef struct RomSerializedNodeHeader RomSerializedNodeHeader, *PRomSerializedNodeHeader;
typedef union RomSerializedNodeReferenceOrSavedOffset4 RomSerializedNodeReferenceOrSavedOffset4, *PRomSerializedNodeReferenceOrSavedOffset4;
typedef struct GraphicsPaletteAsset GraphicsPaletteAsset;
typedef struct GraphicsTextureSet GraphicsTextureSet;
typedef struct WorldRuntimeContext WorldRuntimeContext;

typedef uint32_t ModelMeshGroupMask;

typedef uint32_t WorldRuntimeNodeFlags;

struct WorldRuntimeNodeCommon {
    Ptr32<struct WorldRuntimeNode> previousNode; 
    Ptr32<struct WorldRuntimeNode> nextNode; 
    Ptr32<struct WorldRuntimeContext> ownerWorld; 
};

struct WorldRuntimeNodeModelPayload {
    AngleTurn32 worldRotationAngle0; 
    AngleTurn32 worldRotationAngle1; 
    AngleTurn32 worldRotationAngle2; 
    Q12 localTranslationXQ12; 
    Q12 localTranslationYQ12; 
    Q12 localTranslationZQ12; 
    AngleTurn32 localRotationAngle0; 
    AngleTurn32 localRotationAngle1; 
    AngleTurn32 localRotationAngle2; 
    Ptr32<struct GraphicsPaletteAsset> paletteAsset; 
    Ptr32<struct GraphicsTextureSet> textureSet; 
    uint8_t reserved2C_33[8]; 
    Ptr32<struct ModelResource> modelResource; 
    ModelMeshGroupMask meshGroupMask; 
};

union WorldRuntimeNodePayload {
    uint8_t opaque[60]; 
    struct WorldRuntimeNodeModelPayload model; 
};

struct WorldRuntimeNode {
    struct WorldRuntimeNodeCommon common; 
    union WorldRuntimeNodePayload classPayload; 
    Ptr32<void> runtimePayload; 
    WorldRuntimeNodeFlags runtimeFlags; 
};

typedef uintptr_t RomRegistrySlotValue; /* a model node address (5f) */

typedef uint32_t RomRecordByteSize;

typedef uint32_t RomRecordId;

typedef uint32_t RomRecordTableIndex;

typedef uint32_t RomRecordTableCount;

struct RomAssetHeader {
    struct GeneratedAssetRecordCountHeader recordCountHeader;
    uint8_t reservedB4_1FF[332];
};

struct RomRegistrySlot {
    Ptr32<struct RomAssetRecordPrefix> record; 
    Ptr32<struct WorldRuntimeNode> runtimeRootNode; 
};

struct RomAssetRecordPrefix {
    RomRecordByteSize byteSize; 
    uint32_t rootNodeOffsetOrPointer; 
    RomRecordId recordId; 
};

union RomSerializedNodeReferenceOrSavedOffset4 {
    Ptr32<struct RomSerializedNodeHeader> node; // relocated runtime pointer
    AssetRelativeOffset savedOffset; // serialized asset-relative offset before relocation
};

struct RomSerializedNodeHeader {
    uint32_t reserved00; // unclassified leading dword
    AngleTurn32 localRotationAngle0;
    AngleTurn32 localRotationAngle1;
    AngleTurn32 localRotationAngle2;
    uint32_t childCount; // number of child references; runtime code supports six inline slots
    union RomSerializedNodeReferenceOrSavedOffset4 childReferences[6]; // serialized offsets before relocation; pointers afterward
    union SpriteAssetReferenceOrSavedId spriteAssetReference; // sprite/model resource reference after relocation
    OwnedNestedResourceFlag ownedNestedResourcePresent; // incremented when relocation owns loaded nested resource
};

#endif /* THANDOR_ASSETS_ROM_TYPES_H */
