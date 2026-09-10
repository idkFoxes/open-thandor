#ifndef THANDOR_ASSETS_PACKAGE_RUNTIME_H
#define THANDOR_ASSETS_PACKAGE_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/package/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005460E0 */
undefined8 LevelPackage_ValidateAndMount(word *levelPathUtf16);

/* 0x0040E840 */
dword Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize, void *sourceData,word *path,EngineFileHandle fileHandle);

/* 0x0040ED00 */
dword Package_LoadEntryIntoBuffer (PckLoadCapacityFlags bufferCapacityAndLoadFlags,byte *destination,word *path);

/* 0x0040E450 */
dword Package_MountLowPriority(word *path);

/* 0x0040E6F0 */
dword Package_DeleteEntry(word *path,EngineFileHandle fileHandle);

/* 0x0040EE30 */
void * Package_LoadEntry(word *path);

/* 0x0040E3A0 */
dword Package_Mount(word *path);

/* 0x0040EB70 */
dword Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries, word *pattern,EngineFileHandle fileHandle);

/* 0x0040E500 */
void Package_Unmount(EngineFileHandle fileHandle);

/* 0x0040ECA0 */
dword Package_WildcardPathMatches(word *pattern,word *candidate);

/* 0x0040EAF0 */
dword Package_DecodeEntryInto(byte *destination,PckEntryHeader *entry,EngineFileHandle fileHandle);

/* 0x0040E2B0 */
void Package_SetLastErrorPath(word *path);

/* 0x0040E640 */
PckEntryHeader * Package_FindEntryInMount(word *path,EngineFileHandle fileHandle);

/* 0x0040EA20 */
PckEntryHeader * Package_FindEntryAcrossMounts(word *path);

/* 0x0040E570 */
dword Package_ReadDirectory(EngineFileHandle fileHandle);

#endif /* THANDOR_ASSETS_PACKAGE_RUNTIME_H */
