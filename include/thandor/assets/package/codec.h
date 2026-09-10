#ifndef THANDOR_ASSETS_PACKAGE_CODEC_H
#define THANDOR_ASSETS_PACKAGE_CODEC_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/package/codec. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040A9C0 */
dword PckCodec_EncodeFieldGrid (PckOutputCapacityBytes destinationCapacityBytes,byte *destination, PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid);

/* 0x0040AAA0 */
dword PckCodec_DecodeFieldGrid (PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid, PckStoredByteCount sourceSizeBytes,byte *source);

/* 0x0040A960 */
dword PckCodec_EncodeStored (PckOutputCapacityBytes destinationCapacityBytes,byte *destination, PckDecodedByteCount sourceSizeBytes,byte *source);

/* 0x0040A9A0 */
dword PckCodec_DecodeStored (PckOutputCapacityBytes destinationCapacityBytes,byte *destination, PckStoredByteCount sourceSizeBytes,byte *source);

/* 0x0040A4C0 */
dword PckCodec_EncodeHuffmanRle (PckOutputCapacityBytes destinationCapacityBytes,byte *destination, PckDecodedByteCount sourceSizeBytes,byte *source);

/* 0x0040A790 */
dword PckCodec_DecodeHuffmanRle (PckDecodedByteCount outputSizeBytes,byte *destination, PckStoredByteCount sourceSizeBytes,byte *source);

#endif /* THANDOR_ASSETS_PACKAGE_CODEC_H */
