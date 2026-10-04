/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_PACKAGE_TYPES_H
#define THANDOR_ASSETS_PACKAGE_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

typedef union PckHuffmanSymbolState PckHuffmanSymbolState, *PPckHuffmanSymbolState;
typedef struct PckHuffmanNode PckHuffmanNode, *PPckHuffmanNode;
typedef struct PckEntryHeader PckEntryHeader, *PPckEntryHeader;
typedef struct PckMountSlot PckMountSlot, *PPckMountSlot;

using PckHuffmanFrequencyCount = uint32_t;

using PckHuffmanPackedCodeAndBitLength = uint32_t;

union PckHuffmanSymbolState {
    PckHuffmanFrequencyCount frequencyCount; 
    PckHuffmanPackedCodeAndBitLength packedCodeAndBitLength; 
};

enum {
    PCK_COMPRESSION_HUFFMAN_RLE=0,
    PCK_COMPRESSION_STORED=1,
    PCK_COMPRESSION_FIELD_GRID=2
};
using PckCompressionMethod = int;

enum {
    PCK_ASSET_TYPE_TEC=6514036,
    PCK_ASSET_TYPE_FNC=6516326,
    PCK_ASSET_TYPE_FLD=6581350,
    PCK_ASSET_TYPE_EFF=6710885,
    PCK_ASSET_TYPE_PAL=7102832,
    PCK_ASSET_TYPE_MDL=7103597,
    PCK_ASSET_TYPE_SAM=7168371,
    PCK_ASSET_TYPE_FLM=7171174,
    PCK_ASSET_TYPE_ROM=7171954,
    PCK_ASSET_TYPE_ARM=7172705,
    PCK_ASSET_TYPE_CGN=7235427,
    PCK_ASSET_TYPE_SPR=7499891,
    PCK_ASSET_TYPE_STR=7500915,
    PCK_ASSET_TYPE_SHT=7628915,
    PCK_ASSET_TYPE_LEV=7759212,
    PCK_ASSET_TYPE_GFX=7890535
};
using PckAssetTypeTag = int;

using PckDecodedByteCount = uint32_t;

using PckEntryCount = uint32_t;

using PckStoredByteCount = uint32_t;

using PckOutputCapacityBytes = uint32_t;

using PckHuffmanRunLength = uint32_t;

using PckHeaderDwordCount = uint32_t;

using EngineFileHandle = uintptr_t; /* Win32 HANDLE (pointer-sized, 5f) */

using PckDwordCopyCount = uint32_t;

using PckCompactFieldImageByteCount = uint32_t;

typedef struct PckHuffmanNode *PckHuffmanNodePtr;

using PckHuffmanWeight = uint32_t;

struct PckHuffmanNode {
    PckHuffmanWeight weight; 
    PckHuffmanNodePtr zeroChild; 
    PckHuffmanNodePtr oneChild; 
    PckHuffmanNodePtr parent; 
};

using PckRuntimePayloadOffset = uint32_t;

using PckLoadCapacityFlags = uint32_t;

using PckHuffmanBitOffset = uint8_t;

struct PckEntryHeader {
    uint16_t path[246]; 
    PckRuntimePayloadOffset runtimePayloadOffset; 
    PckDecodedByteCount unpackedSize; 
    PckAssetTypeTag typeTag; 
    PckStoredByteCount packedSize; 
    PckCompressionMethod compressionMethod; 
};

struct PckMountSlot {
    EngineFileHandle fileHandle; 
    struct PckEntryHeader *entryHeaders; 
    PckEntryCount entryCount; 
};
using PckCodecProc = Bool8 (uint32_t destinationCapacityOrOutputSize, uint8_t * destination, uint32_t sourceSize, uint8_t * source, uint32_t * outByteCount, uint32_t * outErrorCode);

#endif /* THANDOR_ASSETS_PACKAGE_TYPES_H */
