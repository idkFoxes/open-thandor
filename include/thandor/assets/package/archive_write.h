/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/archive_write.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H
#define THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/package/archive_write. */

/* Byte offsets into g_PackageScratchBuffer while Package_UpsertEntry/Package_DeleteEntry rewrite an archive:
   the archive header is read to offset 0, the entry header being appended follows it. */
#define PCK_ARCHIVE_SIZE offsetof(PckArchiveHeader,archiveSize)
#define PCK_NEW_ENTRY_PAYLOAD_OFFSET (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,runtimePayloadOffset))
#define PCK_NEW_ENTRY_UNPACKED_SIZE (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,unpackedSize))
#define PCK_NEW_ENTRY_TYPE_TAG (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,typeTag))
#define PCK_NEW_ENTRY_PACKED_SIZE (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,packedSize))
#define PCK_NEW_ENTRY_COMPRESSION_METHOD (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,compressionMethod))

/* Functions are grouped by semantic ownership. */

Bool8 Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle);

Bool8 Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle,uint32_t *outErrorCode);

#endif /* THANDOR_ASSETS_PACKAGE_ARCHIVE_WRITE_H */
