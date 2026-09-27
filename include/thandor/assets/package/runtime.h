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

/* 0x005460E0 */
bool __thandor_cf_preserve_eax_ecx_edx LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16);

/* 0x0040E840 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle);

/* 0x0040ED00 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path);

/* 0x0040E450 */
StatusResult __thandor_eax_cf_preserve_ecx_edx Package_MountLowPriority(uint16_t *path);

/* 0x0040E6F0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle);

/* 0x0040EE30 */
PackageLoadResult __thandor_eax_cf_preserve_ecx_edx Package_LoadEntry(uint16_t *path);

/* 0x0040E3A0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx Package_Mount(uint16_t *path);

/* 0x0040EB70 */
PackageFindResult __thandor_eax_cf_preserve_edx
Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                 uint16_t *pattern,EngineFileHandle fileHandle);

/* 0x0040E500 */
void __thandor_preserve_eax_edx Package_Unmount(EngineFileHandle fileHandle);

/* 0x0040ECA0 */
bool __thandor_cf_preserve_eax_ecx_edx Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate);

/* 0x0040EAF0 */
PackageDecodeResult __thandor_eax_cf_preserve_ecx_edx
Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle);

/* 0x0040E2B0 */
void __thandor_void_preserve_eax_ecx_edx Package_SetLastErrorPath(uint16_t *path);

/* 0x0040E640 */
PackageMountEntryResult __thandor_eax_cf_preserve_ecx_edx
Package_FindEntryInMount(uint16_t *path,EngineFileHandle fileHandle);

/* 0x0040EA20 */
PackageEntryLookupResult __thandor_eax_ebx_cf_preserve_ecx_edx
Package_FindEntryAcrossMounts(uint16_t *path);

/* 0x0040E570 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_ReadDirectory(EngineFileHandle fileHandle);

#endif /* THANDOR_ASSETS_PACKAGE_RUNTIME_H */
