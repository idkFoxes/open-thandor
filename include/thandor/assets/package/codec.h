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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040A9C0 */
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid);

/* 0x0040AAA0 */
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid,
          PckStoredByteCount sourceSizeBytes,byte *source);

/* 0x0040A960 */
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckDecodedByteCount sourceSizeBytes,byte *source);

/* 0x0040A9A0 */
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckStoredByteCount sourceSizeBytes,byte *source);

/* 0x0040A4C0 */
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeHuffmanRle
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckDecodedByteCount sourceSizeBytes,byte *source);

/* 0x0040A790 */
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeHuffmanRle
          (PckDecodedByteCount outputSizeBytes,byte *destination,PckStoredByteCount sourceSizeBytes,
          byte *source);

#endif /* THANDOR_ASSETS_PACKAGE_CODEC_H */
