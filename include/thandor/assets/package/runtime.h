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

/* Size of g_PackageScratchBuffer (8 MiB), allocated once by FileSystem_Init and used as the default
   load/enumeration buffer (the original repeats the literal 0x800000 at every use). */
#define PACKAGE_SCRATCH_BUFFER_BYTES 0x800000
/* Mount table and archive layout (Package_Mount, Package_FindEntry*, Package_Unmount, Package_DecodeEntryInto) */
#define PACKAGE_MOUNT_SLOT_COUNT 0x400 /* g_PackageMountSlots */
#define PACKAGE_DIRECTORY_BYTES 0x80000 /* entry-header array allocated per mount (0x400 headers) */
#define PCK_ENTRY_HEADER_BYTES 0x200 /* sizeof(PckEntryHeader); the archive header has the same size */
#define PACKAGE_DWORD_ALIGN_MASK 0xFFFFFFFCU /* ~3: (byteCount + 3) & mask rounds up to whole dwords */
#define PCK_ENTRY_PATH_UNITS 0xF6 /* UTF-16 code units of PckEntryHeader.path, terminator included */

/* High bits of the Package_LoadEntryIntoBuffer capacity argument */
#define PACKAGE_LOAD_CAPACITY_MASK 0x3FFFFFFF
#define PACKAGE_LOAD_SKIP_PACKAGES 0x80000000 /* load only the loose file */
#define PACKAGE_LOAD_EXECUTABLE_DIRECTORY_FIRST 0x40000000 /* try the loose file next to the executable first */

/* Functions are grouped by semantic ownership. */

Bool8 LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16);

Bool8 Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path,
           uint32_t *outByteCountOrError);

Bool8 Package_MountLowPriority(uint16_t *path,uintptr_t *outFileHandleOrError);

void *Package_LoadEntry(uint16_t *path,uint32_t *outErrorCode);

Bool8 Package_Mount(uint16_t *path,uintptr_t *outFileHandleOrError);

Bool8 Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                       uint16_t *pattern,EngineFileHandle fileHandle,uint32_t *outMatchCount);

void Package_Unmount(EngineFileHandle fileHandle);

Bool8 Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate);

Bool8 Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle,
                             uint32_t *outByteCount,uint32_t *outErrorCode);

void Package_SetLastErrorPath(uint16_t *path);

PckEntryHeader *Package_FindEntryInMount(uint16_t *path,EngineFileHandle fileHandle);

PckEntryHeader *Package_FindEntryAcrossMounts(uint16_t *path,EngineFileHandle *outFileHandle);

Bool8 Package_ReadDirectory(EngineFileHandle fileHandle,uint32_t *outErrorCode);

THANDOR_ALLOWS_OVERREAD void Package_CopyEntryPathDwords(uint8_t *nameDestination,uint16_t *path);

extern uint8_t *g_PackageScratchBuffer;

extern uint16_t g_PackageLastErrorPath[256];

#endif /* THANDOR_ASSETS_PACKAGE_RUNTIME_H */
