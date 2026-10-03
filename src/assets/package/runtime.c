/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static PckMountSlot g_PackageMountSlots[1024] = {0};

/* LevelPackage_ValidateAndMount's one-entry Package_FindEntry output buffer (PCK_ENTRY_HEADER_BYTES) */
static PckEntryHeader g_LevelPackageFoundEntry = {0};

static uint16_t g_LevelLevPatternUtf16[12] = L"level\\*.lev";

static uint16_t g_LevelStrPatternUtf16[12] = L"level\\*.str";

uint8_t *g_PackageScratchBuffer = 0;

/* Implementation ownership: assets/package/runtime. */

/* Mounts the level package levelPathUtf16 and checks that it holds a valid level: its level\*.lev must be a
   'lev' asset of converter version 0x70001, and the level\*.str text page must load as the level's text
   aliases (keyed by the level's title text id). Returns false when the package stays mounted; on any failure
   it is unmounted again and true is returned (a failed mount returns without unmounting).
*/
Bool8 LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16)

{
  uint32_t levelTitleTextId;
  EngineFileHandle fileHandle;
  int *levelAsset;
  uint32_t matchCount;

  /* on failure fileHandle holds the error code but is not used */
  if (!Package_Mount(levelPathUtf16,&fileHandle)) {
    return true; /* nothing mounted, nothing to unmount */
  }
  if (Package_FindEntry(PCK_ENTRY_HEADER_BYTES,&g_LevelPackageFoundEntry,
                        (uint16_t *)g_LevelLevPatternUtf16,fileHandle,&matchCount) &&
      matchCount != 0) {
    levelAsset = Package_LoadEntry(g_LevelPackageFoundEntry.path,NULL);
    if (levelAsset != NULL) {
      /* dword 0: asset magic, dword 3: converter version */
      if (*levelAsset == ASSET_MAGIC_LEV && levelAsset[3] == PCK_CONVERTER_LEV_00070001) {
        levelTitleTextId = levelAsset[92]; /* LEV +0x170 */
        Resource_Release(levelAsset);
        if (Package_FindEntry(PCK_ENTRY_HEADER_BYTES,&g_LevelPackageFoundEntry,
                              (uint16_t *)g_LevelStrPatternUtf16,fileHandle,&matchCount) &&
            matchCount != 0 &&
            !TextResourcePage_LoadCompatibilityAliases(levelTitleTextId,g_LevelPackageFoundEntry.path)) {
          return false; /* valid level: the package stays mounted */
        }
      }
      else {
        Resource_Release(levelAsset);
      }
    }
  }
  Package_Unmount(fileHandle);
  return true;
}


/* Package_UpsertEntry: copies the PCK_ENTRY_PATH_UNITS code units of path (PckEntryHeader.path) to
   nameDestination, two code units per dword.
   Original quirk: the full field is copied (0x7B whole dwords = 492 bytes) whatever the path's length,
   so for the short save-game entry names (g_ArmyHexPathUtf16 ... g_OldunitHexPathUtf16, 0x12-0x1A bytes
   each) it reads up to 0x1EC bytes past the string: through the following names and on into
   g_InGameResourceRegistrationBusyCount and the variables after it, which change at run time, so the
   original bytes cannot be kept by making the names one table. The extra bytes only land behind the
   terminator in the path field of the entry header written to the save file; every reader stops at the
   terminator (Package_FindEntryInMount / Package_FindEntryAcrossMounts compare up to it, Package_FindEntry's
   copy is then used as a string), so they never reach a result. */
static THANDOR_ALLOWS_OVERREAD void Package_CopyEntryPathDwords(uint8_t *nameDestination,uint16_t *path)

{
  int dwordsRemaining;

  for (dwordsRemaining = PCK_ENTRY_PATH_UNITS / 2; dwordsRemaining != 0; dwordsRemaining--) {
    *(uint32_t *)nameDestination = *(uint32_t *)path;
    path = path + 2;
    nameDestination = nameDestination + 4;
  }
}


/* Writes path into the writable mounted package fileHandle, replacing an existing entry of that name: the
   archive header in g_PackageScratchBuffer gets one more entry and the new size, then the entry header and
   its payload are appended at the end of the file. compressionMethod indexes g_PckEncoderTable, except
   PCK_COMPRESSION_STORED, which appends the source dword-aligned as it is; the first source dword becomes
   the entry's typeTag. The in-memory directory is reloaded afterwards. Returns true on success, false when
   deleting the old entry, a seek/read/write, the encoder or the directory reload fails (the error code is
   dropped: no caller uses it).
   Called directly by the save-game writer in ui/ingame/runtime.c (no callback table).
*/
Bool8 Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle)

{
  uint8_t *destination; /* archive header, then the new entry header, then (encoded) its payload */
  uint32_t packedByteCount;
  FileIoByteCount byteCount;
  uint32_t alignedByteCount;

  destination = g_PackageScratchBuffer;
  if (Package_FindEntryInMount(path,fileHandle) != NULL) {
    if (!Package_DeleteEntry(path,fileHandle,NULL)) {
      return false;
    }
  }
  if (g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle)) != 0) {
    return false;
  }
  if (g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,destination,THANDOR_PTR(fileHandle)) != 0) {
    return false;
  }
  if (g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle)) != 0) {
    return false;
  }
  ((PckArchiveHeader *)destination)->entryCount++;
  if (compressionMethod == PCK_COMPRESSION_STORED) {
    alignedByteCount = unpackedSize + 3 & PACKAGE_DWORD_ALIGN_MASK;
    *(uint32_t *)(destination + PCK_NEW_ENTRY_PACKED_SIZE) = alignedByteCount;
    /* compressionMethod = PCK_COMPRESSION_STORED and runtimePayloadOffset = 0, byte by byte (a dword
       store schedules differently) */
    destination[PCK_NEW_ENTRY_COMPRESSION_METHOD] = PCK_COMPRESSION_STORED;
    destination[PCK_NEW_ENTRY_COMPRESSION_METHOD + 1] = 0;
    destination[PCK_NEW_ENTRY_COMPRESSION_METHOD + 2] = 0;
    destination[PCK_NEW_ENTRY_COMPRESSION_METHOD + 3] = 0;
    *(uint32_t *)(destination + PCK_ARCHIVE_SIZE) =
         *(int *)(destination + PCK_ARCHIVE_SIZE) + alignedByteCount + PCK_ENTRY_HEADER_BYTES;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET] = 0;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET + 1] = 0;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET + 2] = 0;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET + 3] = 0;
    *(uint32_t *)(destination + PCK_NEW_ENTRY_TYPE_TAG) = *sourceData;
    *(PckDecodedByteCount *)(destination + PCK_NEW_ENTRY_UNPACKED_SIZE) = unpackedSize;
    if (g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
    Package_CopyEntryPathDwords(destination + PCK_ENTRY_HEADER_BYTES,path);
    if (g_FileSystemSeek(FILESYSTEM_SEEK_END,0,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
    if (g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination + PCK_ENTRY_HEADER_BYTES,
                                      THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
    if (g_FileSystemWriteExactOrFlush(alignedByteCount,sourceData,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
  }
  else {
    /* encode straight behind the new entry header, so both are written in one go */
    if (!g_PckEncoderTable[compressionMethod]
            (PACKAGE_SCRATCH_BUFFER_BYTES - 2 * PCK_ENTRY_HEADER_BYTES,
             destination + 2 * PCK_ENTRY_HEADER_BYTES,unpackedSize,(uint8_t *)sourceData,&packedByteCount,NULL)) {
      return false;
    }
    *(uint32_t *)(destination + PCK_NEW_ENTRY_PACKED_SIZE) = packedByteCount;
    byteCount = packedByteCount + PCK_ENTRY_HEADER_BYTES;
    *(PckCompressionMethod *)(destination + PCK_NEW_ENTRY_COMPRESSION_METHOD) = compressionMethod;
    *(FileIoByteCount *)(destination + PCK_ARCHIVE_SIZE) = *(int *)(destination + PCK_ARCHIVE_SIZE) + byteCount;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET] = 0;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET + 1] = 0;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET + 2] = 0;
    destination[PCK_NEW_ENTRY_PAYLOAD_OFFSET + 3] = 0;
    *(uint32_t *)(destination + PCK_NEW_ENTRY_TYPE_TAG) = *sourceData;
    *(PckDecodedByteCount *)(destination + PCK_NEW_ENTRY_UNPACKED_SIZE) = unpackedSize;
    if (g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
    Package_CopyEntryPathDwords(destination + PCK_ENTRY_HEADER_BYTES,path);
    if (g_FileSystemSeek(FILESYSTEM_SEEK_END,0,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
    if (g_FileSystemWriteExactOrFlush
          (byteCount,destination + PCK_ENTRY_HEADER_BYTES,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
  }
  return Package_ReadDirectory(fileHandle,NULL);
}


/* Loads path into a caller buffer of the given capacity: from the first mounted package that has it, otherwise
   from the loose file (PACKAGE_LOAD_* flags in the top two bits of the capacity select loose-only loading and a
   first try next to the executable). Returns true and stores the byte count in *outByteCountOrError; returns
   false with an error code there instead, FATAL_ERROR_OUT_OF_MEMORY when the entry does not fit (or is to be
   decoded into g_PackageScratchBuffer, which holds the packed data). outByteCountOrError may be NULL.
*/
Bool8 Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path,
           uint32_t *outByteCountOrError)

{
  uint32_t bufferCapacity;
  PckEntryHeader *entry;
  EngineFileHandle entryFileHandle;
  void *handle;
  uint32_t byteCount;
  uint32_t decodedByteCount;
  uint32_t decodeErrorCode;
  Bool8 decoded;
  uint32_t statusCode;
  uint32_t errorCode;

  bufferCapacity = bufferCapacityAndLoadFlags & PACKAGE_LOAD_CAPACITY_MASK;
  entry = NULL;
  if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_SKIP_PACKAGES) == 0) {
    entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  }
  if (entry != NULL) {
    if (entry->unpackedSize <= bufferCapacity && entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1 &&
        destination != g_PackageScratchBuffer) {
      decoded = Package_DecodeEntryInto(destination,entry,entryFileHandle,&decodedByteCount,&decodeErrorCode);
      if (outByteCountOrError != NULL) {
        *outByteCountOrError = decoded ? decodedByteCount : decodeErrorCode;
      }
      return decoded;
    }
    Package_SetLastErrorPath(path);
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
  }
  else {
    /* not in a mounted package (or packages skipped): load the loose file */
    if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_EXECUTABLE_DIRECTORY_FIRST) == 0) {
      statusCode = g_FileSystemOpen(0,path,&handle);
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      statusCode = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle);
      if (statusCode != 0) {
        statusCode = g_FileSystemOpen(0,path,&handle);
      }
    }
    if (statusCode != 0) {
      errorCode = statusCode; /* the open error code */
    }
    else {
      if (!g_FileSystemGetSize(handle,&byteCount)) {
        errorCode = byteCount; /* a failed size query leaves 0 in byteCount, which becomes the error code */
      }
      /* a file larger than the buffer is truncated to the capacity, unless that exceeds 8 MiB */
      else if (bufferCapacity < byteCount && PACKAGE_SCRATCH_BUFFER_BYTES - 1 < bufferCapacity) {
        errorCode = FATAL_ERROR_OUT_OF_MEMORY;
      }
      else {
        if (bufferCapacity < byteCount) {
          byteCount = bufferCapacity;
        }
        statusCode = g_FileSystemReadExact((FileIoByteCount)byteCount,destination,handle);
        if (statusCode == 0) {
          g_FileSystemClose(handle);
          if (outByteCountOrError != NULL) {
            *outByteCountOrError = byteCount;
          }
          return true;
        }
        errorCode = statusCode;
      }
      g_FileSystemClose(handle);
    }
  }
  if (outByteCountOrError != NULL) {
    *outByteCountOrError = errorCode;
  }
  return false;
}


/* Package_Mount and Package_MountLowPriority, once a free slot is chosen: opens path for writing (next to the
   executable first, then as given), allocates the entry-header array and reads the directory into
   mountSlot. Stores the file handle or the open/allocation error code in *outFileHandleOrError (may be
   NULL); returns true on success. */
static Bool8 Package_MountIntoSlot(PckMountSlot *mountSlot,uint16_t *path,uint32_t *outFileHandleOrError)

{
  void *handle;
  PckEntryHeader *allocatedEntryHeaders;
  uint32_t openError;
  uint32_t allocError;
  uint32_t errorCode;

  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  openError = g_FileSystemOpen
                    (FILESYSTEM_OPEN_WRITE_ACCESS,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle);
  if (openError != 0) {
    openError = g_FileSystemOpen(FILESYSTEM_OPEN_WRITE_ACCESS,path,&handle);
  }
  if (openError != 0) {
    errorCode = openError;
  }
  else {
    allocError = g_MemoryApi.alloc(PACKAGE_DIRECTORY_BYTES,(void **)&allocatedEntryHeaders);
    if (allocError == 0) {
      mountSlot->fileHandle = (EngineFileHandle)handle;
      mountSlot->entryHeaders = allocatedEntryHeaders;
      mountSlot->entryCount = 0;
      Package_ReadDirectory((EngineFileHandle)handle,NULL); /* its result is ignored */
      if (outFileHandleOrError != NULL) {
        *outFileHandleOrError = (uint32_t)handle;
      }
      return true;
    }
    g_FileSystemClose(handle);
    errorCode = allocError;
  }
  if (outFileHandleOrError != NULL) {
    *outFileHandleOrError = errorCode;
  }
  return false;
}


/* Like Package_Mount, but takes the last free mount slot: lookups scan the table from the front, so this
   archive loses against every other one. FileSystem_Init mounts engine.pck this way. Same result as
   Package_Mount.
*/
Bool8 Package_MountLowPriority(uint16_t *path,uint32_t *outFileHandleOrError)

{
  int slotsRemaining;
  PckMountSlot *mountSlot;

  mountSlot = g_PackageMountSlots + (PACKAGE_MOUNT_SLOT_COUNT - 1);
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && mountSlot->fileHandle != 0) {
    mountSlot--;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    if (outFileHandleOrError != NULL) {
      *outFileHandleOrError = FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
    }
    return false;
  }
  return Package_MountIntoSlot(mountSlot,path,outFileHandleOrError);
}


/* Package_DeleteEntry, step 1: rewrites the archive header of fileHandle with one entry less and its size
   reduced by the deleted entry (header plus entryPackedSize payload bytes), staged in destination. Stores the
   archive size before the delete in *outArchiveEndOffset. Returns 0 or the file-system error code. */
static uint32_t Package_ShrinkArchiveHeader(PckStoredByteCount entryPackedSize,uint8_t *destination,
                                            EngineFileHandle fileHandle,int *outArchiveEndOffset)
{
  uint32_t statusCode;

  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  statusCode = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,destination,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  *outArchiveEndOffset = *(int *)(destination + PCK_ARCHIVE_SIZE);
  ((PckArchiveHeader *)destination)->entryCount--;
  *(PckStoredByteCount *)(destination + PCK_ARCHIVE_SIZE) =
       *(int *)(destination + PCK_ARCHIVE_SIZE) - (entryPackedSize + PCK_ENTRY_HEADER_BYTES);
  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  return g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination,THANDOR_PTR(fileHandle));
}


/* Package_DeleteEntry, step 2: moves the tailByteCount bytes from tailOffset down to entryOffset (the deleted
   entry's header) through g_PackageScratchBuffer and truncates the file behind them. Returns 0, the
   file-system error code, or FATAL_ERROR_GENERAL_FAILURE when the tail does not fit the scratch buffer. */
static uint32_t Package_MoveTailOverEntry(FileSystemFilePosition entryOffset,FileSystemFilePosition tailOffset,
                                          uint32_t tailByteCount,EngineFileHandle fileHandle)
{
  uint32_t statusCode;

  if (tailByteCount == 0) {
    statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,entryOffset,THANDOR_PTR(fileHandle));
    if (statusCode != 0) {
      return statusCode;
    }
    /* a zero-byte write truncates the file at the current position */
    return g_FileSystemWriteExactOrFlush(0,NULL,THANDOR_PTR(fileHandle));
  }
  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,tailOffset,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  if (PACKAGE_SCRATCH_BUFFER_BYTES < tailByteCount) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  statusCode = g_FileSystemReadExact(tailByteCount,g_PackageScratchBuffer,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,entryOffset,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  statusCode = g_FileSystemWriteExactOrFlush(tailByteCount,g_PackageScratchBuffer,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  return g_FileSystemWriteExactOrFlush(0,NULL,THANDOR_PTR(fileHandle));
}


/* Deletes the entry named path from the writable mounted package fileHandle (a missing entry counts as
   deleted): the archive header loses one entry and its size, everything behind the entry is moved down over
   it through g_PackageScratchBuffer, the file is truncated there and the in-memory directory is reloaded.
   Returns true on success; on failure returns false with the file-system error code (or
   FATAL_ERROR_GENERAL_FAILURE when the tail does not fit the scratch buffer) in *outErrorCode, which may be
   NULL. Called directly by Package_UpsertEntry and the save-game writer in ui/ingame/runtime.c (no callback
   table).
*/
Bool8 Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle,uint32_t *outErrorCode)

{
  PckStoredByteCount entryPackedSize;
  int archiveEndOffset; /* archive size before the delete */
  uint8_t *destination;
  PckEntryHeader *foundEntry;
  FileSystemFilePosition tailOffset; /* first byte behind the deleted entry */
  uint32_t statusCode;
  uint32_t tailByteCount;
  uint32_t directoryErrorCode;

  destination = g_PackageScratchBuffer;
  foundEntry = Package_FindEntryInMount(path,fileHandle);
  if (foundEntry == NULL) {
    /* a missing entry counts as deleted */
    return true;
  }
  entryPackedSize = foundEntry->packedSize;
  statusCode = Package_ShrinkArchiveHeader(entryPackedSize,destination,fileHandle,&archiveEndOffset);
  if (statusCode == 0) {
    /* runtimePayloadOffset is the file offset of the entry header */
    tailOffset = foundEntry->runtimePayloadOffset + foundEntry->packedSize + PCK_ENTRY_HEADER_BYTES;
    tailByteCount = archiveEndOffset - tailOffset;
    statusCode = Package_MoveTailOverEntry(foundEntry->runtimePayloadOffset,tailOffset,tailByteCount,fileHandle);
    if (statusCode == 0) {
      if (Package_ReadDirectory(fileHandle,&directoryErrorCode)) {
        return true;
      }
      statusCode = directoryErrorCode;
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = statusCode;
  }
  return false;
}


/* Package_LoadEntry for a path in no mounted package: opens the loose file (relative to the executable
   directory first, then as given) and reads it whole into a newly allocated buffer. Returns the buffer, or
   NULL with the open, size, allocation or read error code in *outErrorCode. */
static void *Package_LoadLooseFile(uint16_t *path,uint32_t *outErrorCode)

{
  void *handle;
  void *fileBuffer;
  uint32_t byteCount;
  uint32_t openError;
  uint32_t readError;
  uint32_t errorCode;

  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  openError = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle);
  if (openError != 0) {
    openError = g_FileSystemOpen(0,path,&handle);
    if (openError != 0) {
      *outErrorCode = openError;
      return NULL;
    }
  }
  if (!g_FileSystemGetSize(handle,&byteCount)) {
    errorCode = byteCount; /* a failed size query leaves 0 in byteCount, which becomes the error code */
  }
  else if (g_MemoryApi.alloc(byteCount,&fileBuffer) != 0) {
    /* the requested size becomes the detail of the out-of-memory message */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)byteCount,g_FatalErrorDetail1Utf16);
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
  }
  else {
    readError = g_FileSystemReadExact((FileIoByteCount)byteCount,fileBuffer,handle);
    if (readError == 0) {
      g_FileSystemClose(handle);
      return fileBuffer;
    }
    g_MemoryApi.free(fileBuffer);
    errorCode = readError;
  }
  g_FileSystemClose(handle);
  *outErrorCode = errorCode;
  return NULL;
}


/* Loads an asset into a newly allocated buffer: from the first mounted package that has the path, otherwise as
   a loose file (first relative to the executable directory, then as given). The buffer is untyped here;
   callers cast it to their gfx, fld, lev, mdl, sound, text, ... layout. Returns the buffer (never NULL on
   success); on failure returns NULL and stores the error code in *outErrorCode (outErrorCode may be NULL).
*/
void *Package_LoadEntry(uint16_t *path,uint32_t *outErrorCode)

{
  static int loggedFailures; /* open-thandor diagnostics: first failed loads with their caller stack */
  PckEntryHeader *entry;
  EngineFileHandle entryFileHandle;
  void *buffer;
  uint32_t allocError;
  uint32_t decodeErrorCode;
  uint32_t errorCode;

  entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  if (entry == NULL) {
    buffer = Package_LoadLooseFile(path,&errorCode);
    if (buffer != NULL) {
      return buffer;
    }
  }
  else {
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
    /* the packed data is staged in the package scratch buffer, so it must fit there */
    if (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1) {
      allocError = g_MemoryApi.alloc(entry->unpackedSize,&buffer);
      if (allocError != 0) {
        errorCode = allocError;
      }
      else {
        if (Package_DecodeEntryInto((uint8_t *)buffer,entry,entryFileHandle,NULL,&decodeErrorCode)) {
          return buffer;
        }
        errorCode = decodeErrorCode;
        g_MemoryApi.free(buffer);
      }
    }
  }
  if (loggedFailures++ < 8) {
    Thandor_Log("Package_LoadEntry failed: \"%ls\" (error 0x%08X)", (wchar_t *)path, errorCode);
    Thandor_LogStack("  load failure stack", errorCode);
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return NULL;
}


/* Mounts the package archive path (next to the executable first, then as given) in the first free mount slot
   and reads its directory into a fresh PACKAGE_DIRECTORY_BYTES entry-header array. Lookups scan the slots in
   the same order, so earlier mounts win. Returns true and stores the file handle in *outFileHandleOrError;
   returns false with an error code there instead when no slot is free, the file cannot be opened or the
   allocation fails. outFileHandleOrError may be NULL.
*/
Bool8 Package_Mount(uint16_t *path,uint32_t *outFileHandleOrError)

{
  int slotsRemaining;
  PckMountSlot *mountSlot;

  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && mountSlot->fileHandle != 0) {
    mountSlot++;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    if (outFileHandleOrError != NULL) {
      *outFileHandleOrError = FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
    }
    return false;
  }
  return Package_MountIntoSlot(mountSlot,path,outFileHandleOrError);
}


/* The mount slot holding fileHandle, or NULL when it is not mounted. A zero handle is never found (it would
   otherwise match a free slot). Used by Package_FindEntry and Package_Unmount. */
static PckMountSlot *Package_FindMountSlot(EngineFileHandle fileHandle)

{
  PckMountSlot *mountSlot;
  int slotsRemaining;

  if (fileHandle == 0) {
    return NULL;
  }
  mountSlot = g_PackageMountSlots;
  for (slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (fileHandle == mountSlot->fileHandle) {
      return mountSlot;
    }
    mountSlot++;
  }
  return NULL;
}


/* Package_FindEntry sort order: true when record later is smaller than record front, comparing the whole
   PCK_ENTRY_HEADER_BYTES record as unsigned UTF-16 code units (the path first, then whatever the output
   record held behind it). Equal records are not smaller. */
static Bool8 Package_FoundEntryIsSmaller(const PckEntryHeader *later,const PckEntryHeader *front)

{
  const uint16_t *laterUnits;
  const uint16_t *frontUnits;
  int unitIndex;

  laterUnits = (const uint16_t *)later;
  frontUnits = (const uint16_t *)front;
  for (unitIndex = 0; unitIndex < PCK_ENTRY_HEADER_BYTES / 2; unitIndex++) {
    if (laterUnits[unitIndex] != frontUnits[unitIndex]) {
      return laterUnits[unitIndex] < frontUnits[unitIndex];
    }
  }
  return false;
}


/* Exchanges two whole PCK_ENTRY_HEADER_BYTES records, dword by dword. */
static void Package_SwapFoundEntries(PckEntryHeader *first,PckEntryHeader *second)

{
  uint32_t *firstDwords;
  uint32_t *secondDwords;
  uint32_t swappedDword;
  int dwordIndex;

  firstDwords = (uint32_t *)first;
  secondDwords = (uint32_t *)second;
  for (dwordIndex = 0; dwordIndex < PCK_ENTRY_HEADER_BYTES / 4; dwordIndex++) {
    swappedDword = firstDwords[dwordIndex];
    firstDwords[dwordIndex] = secondDwords[dwordIndex];
    secondDwords[dwordIndex] = swappedDword;
  }
}


/* Package_FindEntry, final step: exchange sort of the entryCount found records. Every record is compared
   with each later one and the two are swapped when the later one is smaller. */
static void Package_SortFoundEntries(PckEntryHeader *entries,int entryCount)

{
  int frontIndex;
  int laterIndex;

  for (frontIndex = 0; frontIndex + 1 < entryCount; frontIndex++) {
    for (laterIndex = frontIndex + 1; laterIndex < entryCount; laterIndex++) {
      if (Package_FoundEntryIsSmaller(&entries[laterIndex],&entries[frontIndex])) {
        Package_SwapFoundEntries(&entries[frontIndex],&entries[laterIndex]);
      }
    }
  }
}


/* Lists the entries of the mounted package fileHandle whose path matches pattern (Package_WildcardPathMatches):
   copies each path into a PCK_ENTRY_HEADER_BYTES output record while the capacity lasts and sorts the records
   by path (UTF-16 code-unit order). Returns true with the match count in *outMatchCount (each record is
   PCK_ENTRY_HEADER_BYTES); returns false when the handle is not mounted, leaving *outMatchCount unchanged
   (the original returned FATAL_ERROR_GENERAL_FAILURE, which no caller reads).
*/
Bool8 Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                       uint16_t *pattern,EngineFileHandle fileHandle,uint32_t *outMatchCount)

{
  PckMountSlot *mountSlot;
  PckEntryHeader *entry;
  PckEntryHeader *outputEntry;
  PckEntryCount entriesRemaining;
  int matchedCount;
  int unitIndex;

  mountSlot = Package_FindMountSlot(fileHandle);
  if (mountSlot == NULL) {
    return false;
  }
  matchedCount = 0;
  outputEntry = outputEntries;
  entry = mountSlot->entryHeaders;
  for (entriesRemaining = mountSlot->entryCount; entriesRemaining != 0; entriesRemaining--) {
    if (!Package_WildcardPathMatches(pattern,entry->path)) { /* false means match */
      if (outputCapacityBytes < PCK_ENTRY_HEADER_BYTES) {
        /* output full: stop with the matches so far.
           Original quirk: these matches are returned unsorted */
        *outMatchCount = matchedCount;
        return true;
      }
      outputCapacityBytes = outputCapacityBytes - PCK_ENTRY_HEADER_BYTES;
      /* only the path is copied; the rest of the output record is left as it was */
      for (unitIndex = 0; unitIndex < PCK_ENTRY_PATH_UNITS; unitIndex++) {
        outputEntry->path[unitIndex] = entry->path[unitIndex];
      }
      outputEntry++;
      matchedCount++;
    }
    entry++;
  }
  Package_SortFoundEntries(outputEntries,matchedCount);
  *outMatchCount = matchedCount;
  return true;
}


/* Unmounts the package fileHandle: frees its entry-header array, closes the file and clears the mount slot.
   Does nothing for a zero or unknown handle.
*/
void Package_Unmount(EngineFileHandle fileHandle)

{
  PckMountSlot *mountSlot;

  mountSlot = Package_FindMountSlot(fileHandle);
  if (mountSlot == NULL) {
    return;
  }
  g_MemoryApi.free(mountSlot->entryHeaders);
  g_FileSystemClose(THANDOR_PTR(fileHandle));
  mountSlot->fileHandle = 0;
  mountSlot->entryHeaders = NULL;
  mountSlot->entryCount = 0;
}


/* Compares a UTF-16 archive path against a pattern for the package entry search. '?' matches any one code
   unit; '*' only skips the candidate to its next dot or terminator (no full globbing), which is enough for
   patterns like "level\*.lev". The comparison is case-sensitive. Returns false on a match.
*/
Bool8 Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate)

{
  uint16_t patternCodeUnit;

  do {
    patternCodeUnit = *pattern++;
    while (patternCodeUnit == '*') {
      while (*candidate != '.' && *candidate != 0) {
        candidate++;
      }
      patternCodeUnit = *pattern++;
    }
    if (patternCodeUnit != '?' && patternCodeUnit != *candidate) {
      return true; /* mismatch */
    }
    candidate++;
  } while (patternCodeUnit != 0);
  return false; /* both ended together */
}


/* Reads the packed data of entry from the package fileHandle into g_PackageScratchBuffer and unpacks it into
   destination with the decoder of its compression method (g_PckDecoderTable). Returns true on success with the
   decoder's byte count in *outByteCount; on failure returns false with the seek, read or decoder error code in
   *outErrorCode and leaves the entry path in g_PackageLastErrorPath. Either out pointer may be NULL.
*/
Bool8 Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle,
                             uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckCompressionMethod entryCompression;
  uint32_t decoderStatusCode;

  decoderStatusCode = g_FileSystemSeek
                    (FILESYSTEM_SEEK_BEGIN,entry->runtimePayloadOffset + PCK_ENTRY_HEADER_BYTES,THANDOR_PTR(fileHandle));
  if (decoderStatusCode == 0) {
    entryCompression = entry->compressionMethod;
    decoderStatusCode = g_FileSystemReadExact(entry->packedSize,g_PackageScratchBuffer,THANDOR_PTR(fileHandle));
    if (decoderStatusCode == 0) {
      /* the decoder stores its byte count straight into *outByteCount (NULL is allowed) */
      if (g_PckDecoderTable[entryCompression]
              (entry->unpackedSize,destination,entry->packedSize,g_PackageScratchBuffer,outByteCount,
               &decoderStatusCode)) {
        return true;
      }
    }
  }
  Package_SetLastErrorPath(entry->path);
  if (outErrorCode != NULL) {
    *outErrorCode = decoderStatusCode;
  }
  return false;
}


/* Stores path in g_PackageLastErrorPath for the fatal-error message of a failed load. The length is measured
   in code units (at most 0x100, terminator included) but used as a byte count: the original copies twice as
   many code units as the path has, running past the terminator and, for paths
   over 0x80 units, into g_FatalErrorDetail1Utf16 behind the 0x100-unit buffer.
*/
THANDOR_ALLOWS_OVERREAD void Package_SetLastErrorPath(uint16_t *path)

{
  uint16_t codeUnit;
  int remainingCount;
  uint16_t *scanEnd;
  uint16_t *wordCursor;
  
  remainingCount = 256;
  wordCursor = path;
  do {
    scanEnd = wordCursor;
    if (remainingCount == 0) break;
    remainingCount--;
    scanEnd = wordCursor + 1;
    codeUnit = *wordCursor;
    wordCursor = scanEnd;
  } while (codeUnit != 0);
  remainingCount = (int)((uint8_t *)scanEnd - (uint8_t *)path); /* bytes, used as a code-unit count below */
  wordCursor = g_PackageLastErrorPath;
  for (; remainingCount != 0; remainingCount--) {
    *wordCursor = *path;
    path++;
    wordCursor++;
  }
  return;
}


/* Finds the entry whose name equals path exactly (no wildcards, case-sensitive: package paths are stored in
   lower case) in the package mounted as fileHandle and returns its entry header. Returns NULL when no entry
   matches, and also for a path longer than an entry name, an unmounted handle or an empty package. Called
   directly by Package_UpsertEntry and Package_DeleteEntry (no callback table).
*/
PckEntryHeader *Package_FindEntryInMount(uint16_t *path,EngineFileHandle fileHandle)

{
  int lengthRemaining; /* PCK_ENTRY_PATH_UNITS minus the path length, terminator included */
  int remainingCount;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  Bool8 matched;
  PckEntryHeader *currentEntry;

  matched = true;
  lengthRemaining = PCK_ENTRY_PATH_UNITS;
  pathCursor = path;
  do {
    if (lengthRemaining == 0) break;
    lengthRemaining--;
    matched = *pathCursor == 0;
    pathCursor++;
  } while (!matched);
  if (!matched) {
    return NULL; /* path too long */
  }
  mountSlot = g_PackageMountSlots;
  remainingCount = PACKAGE_MOUNT_SLOT_COUNT;
  while (fileHandle != mountSlot->fileHandle) {
    mountSlot++;
    remainingCount--;
    if (remainingCount == 0) {
      return NULL; /* not mounted */
    }
  }
  currentEntry = mountSlot->entryHeaders;
  entriesRemaining = mountSlot->entryCount;
  if (entriesRemaining == 0) {
    return NULL; /* empty package */
  }
  do {
    /* compare code units over the path length including its terminator, stopping at the first difference */
    matched = false;
    remainingCount = -(lengthRemaining - PCK_ENTRY_PATH_UNITS);
    pathCursor = path;
    nameCursor = currentEntry->path;
    while (remainingCount != 0) {
      matched = *pathCursor == *nameCursor;
      remainingCount--;
      pathCursor++;
      nameCursor++;
      if (!matched) break;
    }
    if (matched) {
      return currentEntry;
    }
    currentEntry++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return NULL;
}


/* Finds path in the mounted packages, scanning the mount slots from the front so that the first mounted
   package that has the entry wins. The path is lowercased in place first (package paths are stored in lower
   case). Returns the entry header and stores the package's handle in *outFileHandle; returns NULL (leaving
   *outFileHandle unchanged) when the path is too long for an entry or no package has it. A found entry is
   never NULL.
*/
PckEntryHeader *Package_FindEntryAcrossMounts(uint16_t *path,EngineFileHandle *outFileHandle)

{
  uint32_t codeUnit;
  int pathLength;
  int compareRemaining;
  int slotsRemaining;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  Bool8 matched;
  PckEntryHeader *currentEntry;

  /* Lowercase the path in place and count its code units, terminator included. A path that reaches
     PCK_ENTRY_PATH_UNITS code units (terminator included) is rejected after its last unit was written. */
  pathLength = 0;
  do {
    codeUnit = path[pathLength];
    if ('A' - 1 < codeUnit && codeUnit < 'Z' + 1) {
      codeUnit = codeUnit + ('a' - 'A');
    }
    path[pathLength] = (uint16_t)codeUnit;
    pathLength++;
    if (pathLength == PCK_ENTRY_PATH_UNITS) return NULL; /* path too long */
  } while (codeUnit != 0);
  mountSlot = g_PackageMountSlots;
  for (slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    currentEntry = mountSlot->entryHeaders;
    if (currentEntry != NULL) {
      for (entriesRemaining = mountSlot->entryCount; entriesRemaining != 0;
           entriesRemaining = entriesRemaining - 1) {
        /* compare the lowercased path, terminator included, with the entry's path */
        matched = false;
        compareRemaining = pathLength;
        pathCursor = path;
        nameCursor = currentEntry->path;
        while (compareRemaining != 0) {
          matched = *pathCursor == *nameCursor;
          compareRemaining--;
          pathCursor++;
          nameCursor++;
          if (!matched) break;
        }
        if (matched) {
          *outFileHandle = mountSlot->fileHandle;
          return currentEntry;
        }
        currentEntry++;
      }
    }
    mountSlot++;
  }
  return NULL;
}


/* Package_ReadDirectory, once the slot is found: reads the archive header and every entry header of
   fileHandle into mountSlot. Returns 0 or the file-system error code of the failed seek/read. */
static uint32_t Package_ReadDirectoryIntoSlot(PckMountSlot *mountSlot,EngineFileHandle fileHandle)

{
  uint32_t statusCode;
  PckEntryCount entriesRemaining;
  FileSystemFilePosition entryHeaderOffset;
  PckEntryHeader *entryHeader;

  entryHeader = mountSlot->entryHeaders;
  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  statusCode = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,g_PackageScratchBuffer,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  entriesRemaining = ((PckArchiveHeader *)g_PackageScratchBuffer)->entryCount;
  mountSlot->entryCount = entriesRemaining;
  /* each entry is its header followed directly by its packed payload */
  entryHeaderOffset = PCK_ENTRY_HEADER_BYTES;
  for (; entriesRemaining != 0; entriesRemaining--) {
    statusCode = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,entryHeader,THANDOR_PTR(fileHandle));
    if (statusCode != 0) {
      return statusCode;
    }
    entryHeader->runtimePayloadOffset = entryHeaderOffset;
    entryHeaderOffset = entryHeaderOffset + entryHeader->packedSize + PCK_ENTRY_HEADER_BYTES;
    entryHeader++;
    statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,entryHeaderOffset,THANDOR_PTR(fileHandle));
    if (statusCode != 0) {
      return statusCode;
    }
  }
  return 0;
}


/* Loads the directory of the mounted package fileHandle into its mount slot: reads the archive header for the
   entry count, then every entry header, recording the file offset of the entry (its header; the packed
   payload follows it) and seeking past the payload to the next header. Returns true on success; false with
   the file-system error, or FATAL_ERROR_GENERAL_FAILURE when fileHandle is not mounted, in *outErrorCode
   (which may be NULL). The original also returned the last seek position on success; no caller used it.
*/
Bool8 Package_ReadDirectory(EngineFileHandle fileHandle,uint32_t *outErrorCode)

{
  uint32_t statusCode;
  int slotsRemaining;
  PckMountSlot *mountSlot;

  /* no zero-handle check here: a zero handle matches the first free slot */
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && fileHandle != mountSlot->fileHandle) {
    mountSlot++;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    statusCode = FATAL_ERROR_GENERAL_FAILURE; /* not mounted */
  }
  else {
    statusCode = Package_ReadDirectoryIntoSlot(mountSlot,fileHandle);
    if (statusCode == 0) {
      return true;
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = statusCode;
  }
  return false;
}

