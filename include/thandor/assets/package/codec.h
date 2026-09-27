/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/codec.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_PACKAGE_CODEC_H
#define THANDOR_ASSETS_PACKAGE_CODEC_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/package/codec. */
/* PCK compression method 0 (PckCodec_EncodeHuffmanRle / PckCodec_DecodeHuffmanRle): a 256-byte table of
   8-bit symbol frequencies, then an LSB-first bitstream of tokens. Token = flag bit 0 + Huffman code of a
   literal byte, or flag bit 1 + 4-bit (runLength - 3) + Huffman code of the repeated byte. */
#define PCK_HUFFMAN_SYMBOL_COUNT 256            /* byte symbols = leaf nodes = internal node slots */
#define PCK_HUFFMAN_FREQUENCY_TABLE_BYTES 0x100 /* stored frequency table in front of the bitstream */
#define PCK_HUFFMAN_NODE_COUNT 0x200            /* leaves + internal nodes scanned per tree-building step */
#define PCK_HUFFMAN_MIN_RUN_LENGTH 3            /* shortest run encoded as a run token (runs are 3..18) */
/* PCK compression method 2 (PckCodec_EncodeFieldGrid / PckCodec_DecodeFieldGrid): a 0x10-byte prefix whose
   first dword is the size of the compact image, then the compact image packed with method 0. The compact image
   is the FieldGridAsset header followed by one 0x10-byte record per cell: persistedAux54, terrainHeight,
   waterSurfaceDelta, flagsAndMaterial. */
#define PCK_FIELD_GRID_PREFIX_BYTES 0x10
#define FIELD_GRID_HEADER_BYTES 0x200 /* FieldGridAsset up to cells[] */
#define FIELD_GRID_HEADER_DWORDS 0x80
#define FIELD_GRID_CELL_DWORDS 0x20 /* sizeof(FieldGridCell) / 4 */
#define FIELD_GRID_COMPACT_CELL_BYTES 0x10
/* Generated cell world coordinates (Q12): a triangle lattice, each row shifted by half a column */
#define FIELD_GRID_WORLD_COLUMN_STEP_X 0x901 /* 2305 */
#define FIELD_GRID_WORLD_ROW_STEP_X 0x480 /* 1152, about half a column */
#define FIELD_GRID_WORLD_ROW_STEP_Y (-1999)

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040A9C0 */
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid);

/* 0x0040AAA0 */
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid,
          PckStoredByteCount sourceSizeBytes,uint8_t *source);

/* 0x0040A960 */
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceSizeBytes,uint8_t *source);

/* 0x0040A9A0 */
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckStoredByteCount sourceSizeBytes,uint8_t *source);

/* 0x0040A4C0 */
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeHuffmanRle
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceSizeBytes,uint8_t *source);

/* 0x0040A790 */
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeHuffmanRle
          (PckDecodedByteCount outputSizeBytes,uint8_t *destination,PckStoredByteCount sourceSizeBytes,
          uint8_t *source);

#endif /* THANDOR_ASSETS_PACKAGE_CODEC_H */
