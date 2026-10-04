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

/* Types (split out by tools/dev/split_types.py). */

typedef union PckHuffmanSymbolState PckHuffmanSymbolState, *PPckHuffmanSymbolState;
typedef struct PckHuffmanNode PckHuffmanNode, *PPckHuffmanNode;
typedef struct PckEntryHeader PckEntryHeader, *PPckEntryHeader;
typedef struct PckMountSlot PckMountSlot, *PPckMountSlot;

typedef uint32_t PckHuffmanFrequencyCount;

typedef uint32_t PckHuffmanPackedCodeAndBitLength;

union PckHuffmanSymbolState {
    PckHuffmanFrequencyCount frequencyCount; 
    PckHuffmanPackedCodeAndBitLength packedCodeAndBitLength; 
};

enum {
    PCK_COMPRESSION_HUFFMAN_RLE=0,
    PCK_COMPRESSION_STORED=1,
    PCK_COMPRESSION_FIELD_GRID=2
};
typedef int PckCompressionMethod;

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
typedef int PckAssetTypeTag;

typedef uint32_t PckDecodedByteCount;

typedef uint32_t PckEntryCount;

typedef uint32_t PckStoredByteCount;

typedef uint32_t PckOutputCapacityBytes;

typedef uint32_t PckHuffmanRunLength;

typedef uint32_t PckHeaderDwordCount;

typedef uintptr_t EngineFileHandle; /* Win32 HANDLE (pointer-sized, 5f) */

typedef uint32_t PckDwordCopyCount;

typedef uint32_t PckCompactFieldImageByteCount;

typedef struct PckHuffmanNode *PckHuffmanNodePtr;

typedef uint32_t PckHuffmanWeight;

struct PckHuffmanNode {
    PckHuffmanWeight weight; 
    PckHuffmanNodePtr zeroChild; 
    PckHuffmanNodePtr oneChild; 
    PckHuffmanNodePtr parent; 
};

typedef uint32_t PckRuntimePayloadOffset;

typedef uint32_t PckLoadCapacityFlags;

typedef uint8_t PckHuffmanBitOffset;

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
typedef Bool8 PckCodecProc(uint32_t destinationCapacityOrOutputSize, uint8_t * destination, uint32_t sourceSize, uint8_t * source, uint32_t * outByteCount, uint32_t * outErrorCode);

#endif /* THANDOR_ASSETS_PACKAGE_TYPES_H */
