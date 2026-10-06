/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/archive_write.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H
#define THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H

#include <thandor/assets/package/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

Bool8 Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle);

Bool8 Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle,uint32_t *outErrorCode);

#endif /* THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H */
