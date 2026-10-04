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

/* Byte offset into g_PackageScratchBuffer while Package_UpsertEntry/Package_DeleteEntry rewrite an archive:
   the archive header is read to offset 0, the entry header being appended follows it at
   PCK_ENTRY_HEADER_BYTES. */
#define PCK_ARCHIVE_SIZE offsetof(PckArchiveHeader,archiveSize)

Bool8 Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle);

Bool8 Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle,uint32_t *outErrorCode);

#endif /* THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H */
