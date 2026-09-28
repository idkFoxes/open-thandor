/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_PACKAGE_RUNTIME_H
#define THANDOR_ASSETS_PACKAGE_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/package/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Size of g_PackageScratchBuffer (8 MiB), allocated once by FileSystem_Init and used as the default
   load/enumeration buffer (the original repeats the literal 0x800000 at every use). */
#define PACKAGE_SCRATCH_BUFFER_BYTES 0x800000
/* Mount table and archive layout (Package_Mount, Package_FindEntry*, Package_Unmount, Package_DecodeEntryInto) */
#define PACKAGE_MOUNT_SLOT_COUNT 0x400 /* g_PackageMountSlots */
#define PACKAGE_DIRECTORY_BYTES 0x80000 /* entry-header array allocated per mount (0x400 headers) */
#define PCK_ENTRY_HEADER_BYTES 0x200 /* sizeof(PckEntryHeader); the archive header has the same size */
#define PCK_ENTRY_PATH_UNITS 0xF6 /* UTF-16 code units of PckEntryHeader.path, terminator included */
/* Byte offsets into g_PackageScratchBuffer while Package_UpsertEntry/Package_DeleteEntry rewrite an archive:
   the archive header is read to offset 0, the entry header being appended follows it. */
#define PCK_ARCHIVE_SIZE offsetof(PckArchiveHeader,archiveSize)
#define PCK_NEW_ENTRY_PAYLOAD_OFFSET (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,runtimePayloadOffset))
#define PCK_NEW_ENTRY_UNPACKED_SIZE (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,unpackedSize))
#define PCK_NEW_ENTRY_TYPE_TAG (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,typeTag))
#define PCK_NEW_ENTRY_PACKED_SIZE (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,packedSize))
#define PCK_NEW_ENTRY_COMPRESSION_METHOD (PCK_ENTRY_HEADER_BYTES + offsetof(PckEntryHeader,compressionMethod))
/* High bits of the Package_LoadEntryIntoBuffer capacity argument */
#define PACKAGE_LOAD_CAPACITY_MASK 0x3FFFFFFF
#define PACKAGE_LOAD_SKIP_PACKAGES 0x80000000 /* load only the loose file */
#define PACKAGE_LOAD_EXECUTABLE_DIRECTORY_FIRST 0x40000000 /* try the loose file next to the executable first */

/* 0x005460E0 */
bool LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16);

/* 0x0040E840 */
StatusResult Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle);

/* 0x0040ED00 */
StatusResult Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path);

/* 0x0040E450 */
StatusResult Package_MountLowPriority(uint16_t *path);

/* 0x0040E6F0 */
StatusResult Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle);

/* 0x0040EE30 */
PackageLoadResult Package_LoadEntry(uint16_t *path);

/* 0x0040E3A0 */
StatusResult Package_Mount(uint16_t *path);

/* 0x0040EB70 */
PackageFindResult Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                 uint16_t *pattern,EngineFileHandle fileHandle);

/* 0x0040E500 */
void Package_Unmount(EngineFileHandle fileHandle);

/* 0x0040ECA0 */
bool Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate);

/* 0x0040EAF0 */
PackageDecodeResult Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle);

/* 0x0040E2B0 */
void Package_SetLastErrorPath(uint16_t *path);

/* 0x0040E640 */
PackageMountEntryResult Package_FindEntryInMount(uint16_t *path,EngineFileHandle fileHandle);

/* 0x0040EA20 */
PackageEntryLookupResult Package_FindEntryAcrossMounts(uint16_t *path);

/* 0x0040E570 */
StatusResult Package_ReadDirectory(EngineFileHandle fileHandle);

#endif /* THANDOR_ASSETS_PACKAGE_RUNTIME_H */
