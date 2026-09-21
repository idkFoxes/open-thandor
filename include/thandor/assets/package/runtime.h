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

/* 0x005460E0 */
bool __thandor_cf_preserve_eax_ecx_edx LevelPackage_ValidateAndMount(word *levelPathUtf16);

/* 0x0040E840 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   dword *sourceData,word *path,EngineFileHandle fileHandle);

/* 0x0040ED00 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,byte *destination,word *path);

/* 0x0040E450 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_MountLowPriority(word *path);

/* 0x0040E6F0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_DeleteEntry(word *path,EngineFileHandle fileHandle);

/* 0x0040EE30 */
PackageLoadEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_LoadEntry(word *path);

/* 0x0040E3A0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_Mount(word *path);

/* 0x0040EB70 */
PackageFindEntryEaxEcxCf9 __thandor_eax_cf_preserve_edx
Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                 word *pattern,EngineFileHandle fileHandle);

/* 0x0040E500 */
void __thandor_preserve_eax_edx Package_Unmount(EngineFileHandle fileHandle);

/* 0x0040ECA0 */
bool __thandor_cf_preserve_eax_ecx_edx Package_WildcardPathMatches(word *pattern,word *candidate);

/* 0x0040EAF0 */
PackageDecodeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_DecodeEntryInto(byte *destination,PckEntryHeader *entry,EngineFileHandle fileHandle);

/* 0x0040E2B0 */
void __thandor_void_preserve_eax_ecx_edx Package_SetLastErrorPath(word *path);

/* 0x0040E640 */
PackageEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_FindEntryInMount(word *path,EngineFileHandle fileHandle);

/* 0x0040EA20 */
PackageFindEntryEaxEbxCf9 __thandor_eax_ebx_cf_preserve_ecx_edx
Package_FindEntryAcrossMounts(word *path);

/* 0x0040E570 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_ReadDirectory(EngineFileHandle fileHandle);

#endif /* THANDOR_ASSETS_PACKAGE_RUNTIME_H */
