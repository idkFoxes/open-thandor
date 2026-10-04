/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/sprite/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SPRITE_TYPES_H
#define THANDOR_ASSETS_SPRITE_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/graphics/resources/types.h>

typedef struct SpriteAssetHeader SpriteAssetHeader, *PSpriteAssetHeader;
typedef struct GeneratedAssetRegistryHeader GeneratedAssetRegistryHeader, *PGeneratedAssetRegistryHeader;
typedef struct SprRelocationBlockHeader SprRelocationBlockHeader, *PSprRelocationBlockHeader;
typedef struct SprPointerRelocationRecord SprPointerRelocationRecord, *PSprPointerRelocationRecord;
typedef struct SprGroupRelocationHeader SprGroupRelocationHeader, *PSprGroupRelocationHeader;

using SpriteAssetId = uint32_t;

struct GeneratedAssetRegistryHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount groupCount;
    Ptr32<struct SpriteAssetHeader> previousRegistryAsset;
    SpriteAssetId registryId; 
};

struct SpriteAssetHeader {
    struct GeneratedAssetRegistryHeader registryHeader;
    uint8_t reservedBC_1FF[324];
};

using SerializedRelativeByteOffset = uint32_t;

using SprRelocationCount = uint32_t;

struct SprRelocationBlockHeader {
    SerializedRelativeByteOffset blockByteSize;
    uint32_t reserved04;
    SprRelocationCount fixedRecordCount; 
    SprRelocationCount pointerRelocationCount;
    uint8_t reserved10_1F[16];
};

struct SprPointerRelocationRecord {
    uint32_t pointerOrSerializedOffset00;
    uint8_t reserved04_0B[8];
    uint32_t pointerOrSerializedOffset0C;
    uint8_t reserved10_17[8];
    uint32_t pointerOrSerializedOffset18;
    uint8_t reserved1C_3F[36];
};

struct SprGroupRelocationHeader {
    SerializedRelativeByteOffset nextGroupByteOffset; 
    SprRelocationCount relocationBlockCount;
    uint8_t reserved08_1F[24];
};

#endif /* THANDOR_ASSETS_SPRITE_TYPES_H */
