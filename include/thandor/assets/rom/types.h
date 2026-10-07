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

struct WorldRuntimeNodeCommon;
struct WorldRuntimeNodeModelPayload;
struct WorldRuntimeNode;
union WorldRuntimeNodePayload;
struct RomAssetHeader;
struct RomRegistrySlot;
struct RomAssetRecordPrefix;
struct RomSerializedNodeHeader;
union RomSerializedNodeReferenceOrSavedOffset4;
struct GraphicsPaletteAsset;
struct GraphicsTextureSet;
struct WorldRuntimeContext;

using ModelMeshGroupMask = uint32_t;

using WorldRuntimeNodeFlags = uint32_t;

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

using RomRegistrySlotValue = uintptr_t; /* a model node address (5f) */

using RomRecordByteSize = uint32_t;

using RomRecordId = uint32_t;

using RomRecordTableIndex = uint32_t;

using RomRecordTableCount = uint32_t;

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
