/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: assets/package/runtime. */

/* One-entry output buffer (PCK_ENTRY_HEADER_BYTES) for Package_FindEntry, placed behind the string at
   0x00545E91; the found entry's path is its first field and is passed on as a UTF-16 path. */
#define LEVEL_PACKAGE_FOUND_ENTRY (s_NAME__CLIENT__KARTE___00545e91 + 21)

/* Address: 0x005460E0.
   Mounts the level package levelPathUtf16 and checks that it holds a valid level: its level\*.lev must be a
   'lev' asset of converter version 0x70001, and the level\*.str text page must load as the level's text
   aliases (keyed by the level's title text id). CF clear means the package stays mounted; on any failure it is
   unmounted again and CF is set (a failed mount returns without unmounting).
*/
bool LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16)

{
  uint32_t levelTitleTextId;
  EngineFileHandle fileHandle;
  int *levelAsset;
  bool failed;
  PackageLoadResult loadResult;
  uint32_t matchCount;

  /* on failure fileHandle holds the error code but is not used */
  failed = !Package_Mount(levelPathUtf16,&fileHandle);
  if (!failed) {
    if (Package_FindEntry(PCK_ENTRY_HEADER_BYTES,(PckEntryHeader *)LEVEL_PACKAGE_FOUND_ENTRY,
                          (uint16_t *)u_level___lev_005460a6,fileHandle,&matchCount) &&
        matchCount != 0) {
      loadResult = Package_LoadEntry((uint16_t *)LEVEL_PACKAGE_FOUND_ENTRY);
      levelAsset = loadResult.bufferOrError;
      if (!loadResult.failed) {
        /* dword 0: asset magic, dword 3: converter version */
        if (*levelAsset == ASSET_MAGIC_LEV && levelAsset[3] == PCK_CONVERTER_LEV_00070001) {
          levelTitleTextId = levelAsset[92]; /* LEV +0x170 */
          Resource_Release(levelAsset);
          if (Package_FindEntry(PCK_ENTRY_HEADER_BYTES,(PckEntryHeader *)LEVEL_PACKAGE_FOUND_ENTRY,
                                (uint16_t *)u_level___str_005460be,fileHandle,&matchCount) &&
              matchCount != 0 &&
              (failed = TextResourcePage_LoadCompatibilityAliases
                                 (levelTitleTextId,(uint16_t *)LEVEL_PACKAGE_FOUND_ENTRY),
               !failed)) {
            return failed;
          }
        }
        else {
          Resource_Release(levelAsset);
        }
      }
    }
    Package_Unmount(fileHandle);
    failed = true;
  }
  return failed;
}


/* Address: 0x0040E840.
   Writes path into the writable mounted package fileHandle, replacing an existing entry of that name: the
   archive header in g_PackageScratchBuffer gets one more entry and the new size, then the entry header and
   its payload are appended at the end of the file. compressionMethod indexes g_PckEncoderTable, except
   PCK_COMPRESSION_STORED, which appends the source dword-aligned as it is; the first source dword becomes
   the entry's typeTag. The in-memory directory is reloaded afterwards. Returns true on success, false when
   deleting the old entry, a seek/read/write, the encoder or the directory reload fails (the error code is
   dropped: no caller uses it).
   Called directly by the save-game writer in ui/ingame/runtime.c (no callback table).
*/
bool Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle)

{
  uint8_t *destination; /* archive header, then the new entry header, then (encoded) its payload */
  uint32_t packedByteCount;
  FileIoByteCount byteCount;
  uint32_t alignedByteCount;
  int dwordsRemaining;
  uint8_t *nameDestination;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  PckCodecResult encodeResult;
  FileSystemWriteResult writeResult;

  destination = g_PackageScratchBuffer;
  if (Package_FindEntryInMount(path,fileHandle) != NULL) {
    if (!Package_DeleteEntry(path,fileHandle,NULL)) {
      return false;
    }
  }
  seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  if (!seekResult.failed) {
    readResult = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,destination,(void *)fileHandle);
    if (!readResult.failed) {
      seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      if (!seekResult.failed) {
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
          writeResult = g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination,(void *)fileHandle);
          if (writeResult.failed) {
            return false;
          }
          /* PckEntryHeader.path, two code units per dword */
          nameDestination = destination + PCK_ENTRY_HEADER_BYTES;
          for (dwordsRemaining = PCK_ENTRY_PATH_UNITS / 2; dwordsRemaining != 0; dwordsRemaining--) {
            *(uint32_t *)nameDestination = *(uint32_t *)path;
            path = path + 2;
            nameDestination = nameDestination + 4;
          }
          seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          if (seekResult.failed) {
            return false;
          }
          writeResult = g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination + PCK_ENTRY_HEADER_BYTES,
                                                      (void *)fileHandle);
          if (writeResult.failed) {
            return false;
          }
          writeResult = g_FileSystemWriteExactOrFlush(alignedByteCount,sourceData,(void *)fileHandle);
          if (writeResult.failed) {
            return false;
          }
        }
        else {
          /* encode straight behind the new entry header, so both are written in one go */
          encodeResult = g_PckEncoderTable[compressionMethod]
                            (PACKAGE_SCRATCH_BUFFER_BYTES - 2 * PCK_ENTRY_HEADER_BYTES,
                             destination + 2 * PCK_ENTRY_HEADER_BYTES,unpackedSize,(uint8_t *)sourceData);
          if (encodeResult.failed) {
            return false;
          }
          packedByteCount = encodeResult.byteCountOrError;
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
          writeResult = g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination,(void *)fileHandle);
          if (writeResult.failed) {
            return false;
          }
          nameDestination = destination + PCK_ENTRY_HEADER_BYTES;
          for (dwordsRemaining = PCK_ENTRY_PATH_UNITS / 2; dwordsRemaining != 0; dwordsRemaining--) {
            *(uint32_t *)nameDestination = *(uint32_t *)path;
            path = path + 2;
            nameDestination = nameDestination + 4;
          }
          seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          if (seekResult.failed) {
            return false;
          }
          writeResult = g_FileSystemWriteExactOrFlush
                            (byteCount,destination + PCK_ENTRY_HEADER_BYTES,(void *)fileHandle);
          if (writeResult.failed) {
            return false;
          }
        }
        if (Package_ReadDirectory(fileHandle,NULL)) {
          return true;
        }
      }
    }
  }
  return false;
}


/* Address: 0x0040ED00.
   Loads path into a caller buffer of the given capacity: from the first mounted package that has it, otherwise
   from the loose file (PACKAGE_LOAD_* flags in the top two bits of the capacity select loose-only loading and a
   first try next to the executable). Returns true and stores the byte count in *outByteCountOrError; returns
   false with an error code there instead, FATAL_ERROR_OUT_OF_MEMORY when the entry does not fit (or is to be
   decoded into g_PackageScratchBuffer, which holds the packed data). outByteCountOrError may be NULL.
*/
bool Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path,
           uint32_t *outByteCountOrError)

{
  void *bufferCapacity;
  PckEntryHeader *entry;
  EngineFileHandle entryFileHandle;
  void *handle;
  void *byteCount;
  uint32_t decodedByteCount;
  uint32_t decodeErrorCode;
  bool decoded;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  FileSystemReadResult readResult;

  bufferCapacity = (void *)(bufferCapacityAndLoadFlags & PACKAGE_LOAD_CAPACITY_MASK);
  if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_SKIP_PACKAGES) == 0 &&
      (entry = Package_FindEntryAcrossMounts(path,&entryFileHandle), entry != NULL)) {
    handle = (void *)FATAL_ERROR_OUT_OF_MEMORY;
    if ((((void *)entry->unpackedSize <= bufferCapacity) &&
         (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1)) &&
       (destination != g_PackageScratchBuffer)) {
      decoded = Package_DecodeEntryInto(destination,entry,entryFileHandle,&decodedByteCount,&decodeErrorCode);
      if (outByteCountOrError != NULL) {
        *outByteCountOrError = decoded ? decodedByteCount : decodeErrorCode;
      }
      return decoded;
    }
    Package_SetLastErrorPath(path);
  }
  else {
    /* not in a mounted package (or packages skipped): load the loose file */
    if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_EXECUTABLE_DIRECTORY_FIRST) == 0) {
      openResult = g_FileSystemOpen(0,path);
      handle = (void *)openResult.handleOrError;
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      openResult = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (void *)openResult.handleOrError;
      if (openResult.failed) {
        openResult = g_FileSystemOpen(0,path);
        handle = (void *)openResult.handleOrError;
      }
    }
    if (!openResult.failed) {
      sizeResult = g_FileSystemGetSize(handle);
      byteCount = (void *)sizeResult.sizeOrError;
      if (!sizeResult.failed) {
        /* a file larger than the buffer is truncated to the capacity, unless that exceeds 8 MiB */
        if ((bufferCapacity < byteCount) &&
            (byteCount = bufferCapacity, (void *)(PACKAGE_SCRATCH_BUFFER_BYTES - 1) < bufferCapacity)) {
          byteCount = (void *)FATAL_ERROR_OUT_OF_MEMORY;
        }
        else {
          readResult = g_FileSystemReadExact((FileIoByteCount)byteCount,destination,handle);
          byteCount = (void *)readResult.valueOrError;
          if (!readResult.failed) {
            g_FileSystemClose(handle);
            if (outByteCountOrError != NULL) {
              *outByteCountOrError = (uint32_t)byteCount;
            }
            return true;
          }
        }
      }
      g_FileSystemClose(handle);
      handle = byteCount;
    }
  }
  if (outByteCountOrError != NULL) {
    *outByteCountOrError = (uint32_t)handle;
  }
  return false;
}


/* Address: 0x0040E450.
   Like Package_Mount, but takes the last free mount slot: lookups scan the table from the front, so this
   archive loses against every other one. FileSystem_Init mounts engine.pck this way. Same result as
   Package_Mount.
*/
bool Package_MountLowPriority(uint16_t *path,uint32_t *outFileHandleOrError)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int slotsRemaining;
  PckMountSlot *mountSlot;
  FileSystemOpenResult openResult;
  ArenaAllocResult allocResult;

  mountSlot = g_PackageMountSlots + (PACKAGE_MOUNT_SLOT_COUNT - 1);
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && mountSlot->fileHandle != 0) {
    mountSlot--;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    handle = (PckEntryHeader *)FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
  }
  else {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = g_FileSystemOpen
                      (FILESYSTEM_OPEN_WRITE_ACCESS,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (PckEntryHeader *)openResult.handleOrError;
    if (openResult.failed) {
      openResult = g_FileSystemOpen(FILESYSTEM_OPEN_WRITE_ACCESS,path);
      handle = (PckEntryHeader *)openResult.handleOrError;
    }
    if (!openResult.failed) {
      allocResult = g_MemoryApi.alloc(PACKAGE_DIRECTORY_BYTES);
      allocatedEntryHeaders = (PckEntryHeader *)allocResult.payloadOrError;
      if (!allocResult.failed) {
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
      handle = allocatedEntryHeaders;
    }
  }
  if (outFileHandleOrError != NULL) {
    *outFileHandleOrError = (uint32_t)handle;
  }
  return false;
}


/* Address: 0x0040E6F0.
   Deletes the entry named path from the writable mounted package fileHandle (a missing entry counts as
   deleted): the archive header loses one entry and its size, everything behind the entry is moved down over
   it through g_PackageScratchBuffer, the file is truncated there and the in-memory directory is reloaded.
   Returns true on success; on failure returns false with the file-system error code (or
   FATAL_ERROR_GENERAL_FAILURE when the tail does not fit the scratch buffer) in *outErrorCode, which may be
   NULL. Called directly by Package_UpsertEntry and the save-game writer in ui/ingame/runtime.c (no callback
   table).
*/
bool Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle,uint32_t *outErrorCode)

{
  PckStoredByteCount entryPackedSize;
  int archiveEndOffset; /* archive size before the delete */
  uint8_t *destination;
  PckEntryHeader *foundEntry;
  FileSystemFilePosition tailOffset; /* first byte behind the deleted entry */
  PckEntryHeader *statusOrError;
  uint32_t tailByteCount;
  uint32_t directoryErrorCode;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  FileSystemWriteResult writeResult;

  destination = g_PackageScratchBuffer;
  foundEntry = Package_FindEntryInMount(path,fileHandle);
  statusOrError = foundEntry;
  if (foundEntry == NULL) {
    /* a missing entry counts as deleted */
    return true;
  }
  seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  statusOrError = (PckEntryHeader *)seekResult.positionOrError;
  if (!seekResult.failed) {
    entryPackedSize = foundEntry->packedSize;
    readResult = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,destination,(void *)fileHandle);
    statusOrError = (PckEntryHeader *)readResult.valueOrError;
    if (!readResult.failed) {
      archiveEndOffset = *(int *)(destination + PCK_ARCHIVE_SIZE);
      ((PckArchiveHeader *)destination)->entryCount--;
      *(PckStoredByteCount *)(destination + PCK_ARCHIVE_SIZE) =
           *(int *)(destination + PCK_ARCHIVE_SIZE) - (entryPackedSize + PCK_ENTRY_HEADER_BYTES);
      seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      statusOrError = (PckEntryHeader *)seekResult.positionOrError;
      if (!seekResult.failed) {
        writeResult = g_FileSystemWriteExactOrFlush(PCK_ENTRY_HEADER_BYTES,destination,(void *)fileHandle);
        statusOrError = (PckEntryHeader *)writeResult.valueOrError;
        if (!writeResult.failed) {
          /* runtimePayloadOffset is the file offset of the entry header */
          tailOffset = foundEntry->runtimePayloadOffset + foundEntry->packedSize + PCK_ENTRY_HEADER_BYTES;
          tailByteCount = archiveEndOffset - tailOffset;
          if (tailByteCount == 0) {
            seekResult = g_FileSystemSeek
                              (FILESYSTEM_SEEK_BEGIN,foundEntry->runtimePayloadOffset,(void *)fileHandle
                              );
            statusOrError = (PckEntryHeader *)seekResult.positionOrError;
            if (seekResult.failed) goto fail;
            /* a zero-byte write truncates the file at the current position */
            writeResult = g_FileSystemWriteExactOrFlush(0,NULL,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.valueOrError;
            if (writeResult.failed) goto fail;
          }
          else {
            seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,tailOffset,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)seekResult.positionOrError;
            if ((seekResult.failed) || (statusOrError = (PckEntryHeader *)FATAL_ERROR_GENERAL_FAILURE,
                                         PACKAGE_SCRATCH_BUFFER_BYTES < tailByteCount))
            goto fail;
            readResult = g_FileSystemReadExact(tailByteCount,g_PackageScratchBuffer,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)readResult.valueOrError;
            if (readResult.failed) goto fail;
            seekResult = g_FileSystemSeek
                              (FILESYSTEM_SEEK_BEGIN,foundEntry->runtimePayloadOffset,(void *)fileHandle
                              );
            statusOrError = (PckEntryHeader *)seekResult.positionOrError;
            if (seekResult.failed) goto fail;
            writeResult = g_FileSystemWriteExactOrFlush
                              (tailByteCount,g_PackageScratchBuffer,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.valueOrError;
            if (writeResult.failed) goto fail;
            writeResult = g_FileSystemWriteExactOrFlush(0,NULL,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.valueOrError;
            if (writeResult.failed) goto fail;
          }
          if (Package_ReadDirectory(fileHandle,&directoryErrorCode)) {
            return true;
          }
          statusOrError = (PckEntryHeader *)directoryErrorCode;
        }
      }
    }
  }
fail:
  if (outErrorCode != NULL) {
    *outErrorCode = (uint32_t)statusOrError;
  }
  return false;
}


/* Address: 0x0040EE30.
   Loads an asset into a newly allocated buffer: from the first mounted package that has the path, otherwise as
   a loose file (first relative to the executable directory, then as given). The buffer is untyped here;
   callers cast it to their gfx, fld, lev, mdl, sound, text, ... layout. CF set: bufferOrError is an error code.
*/
PackageLoadResult Package_LoadEntry(uint16_t *path)

{
  PckEntryHeader *entry;
  /* the loose file's handle or the package entry's buffer; on failure the error code */
  uint8_t *handleBufferOrError;
  /* the loose file's size, then the error code of the failed step */
  uint8_t *byteCountOrError;
  ArenaAllocResult allocResult;
  uint32_t decodeErrorCode;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  FileSystemReadResult readResult;
  PackageLoadResult failureResult;
  PackageLoadResult successResult;
  EngineFileHandle entryFileHandle;

  entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  if (entry == NULL) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    handleBufferOrError = (uint8_t *)openResult.handleOrError;
    if (openResult.failed) {
      openResult = g_FileSystemOpen(0,path);
      handleBufferOrError = (uint8_t *)openResult.handleOrError;
      if (openResult.failed) goto fail;
    }
    sizeResult = g_FileSystemGetSize(handleBufferOrError);
    byteCountOrError = (uint8_t *)sizeResult.sizeOrError;
    if (!sizeResult.failed) {
      allocResult = g_MemoryApi.alloc((uint32_t)byteCountOrError);
      if (allocResult.failed) {
        /* the requested size becomes the detail of the out-of-memory message */
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)byteCountOrError,g_FatalErrorDetail1Utf16);
        byteCountOrError = (uint8_t *)FATAL_ERROR_OUT_OF_MEMORY;
      }
      else {
        readResult = g_FileSystemReadExact((FileIoByteCount)byteCountOrError,(void *)allocResult.payloadOrError,handleBufferOrError);
        byteCountOrError = (uint8_t *)readResult.valueOrError;
        if (!readResult.failed) {
          g_FileSystemClose(handleBufferOrError);
          successResult.failed = false;
          successResult.bufferOrError = (uint8_t *)allocResult.payloadOrError;
          return successResult;
        }
        g_MemoryApi.free((void *)allocResult.payloadOrError);
      }
    }
    g_FileSystemClose(handleBufferOrError);
    handleBufferOrError = byteCountOrError;
  }
  else {
    handleBufferOrError = (uint8_t *)FATAL_ERROR_OUT_OF_MEMORY;
    /* the packed data is staged in the package scratch buffer, so it must fit there */
    if (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1) {
      allocResult = g_MemoryApi.alloc(entry->unpackedSize);
      handleBufferOrError = (uint8_t *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        if (Package_DecodeEntryInto(handleBufferOrError,entry,entryFileHandle,NULL,&decodeErrorCode)) {
          successResult.failed = false;
          successResult.bufferOrError = handleBufferOrError;
          return successResult;
        }
        byteCountOrError = (uint8_t *)decodeErrorCode;
        g_MemoryApi.free(handleBufferOrError);
        handleBufferOrError = byteCountOrError;
      }
    }
  }
fail:
  {
    /* open-thandor diagnostics: first failed loads with their caller stack */
    static int loggedFailures;
    if (loggedFailures++ < 8) {
      Thandor_Log("Package_LoadEntry failed: \"%ls\" (error 0x%08X)", (wchar_t *)path, (uint32_t)handleBufferOrError);
      Thandor_LogStack("  load failure stack", (uint32_t)handleBufferOrError);
    }
  }
  failureResult.failed = true;
  failureResult.bufferOrError = handleBufferOrError;
  return failureResult;
}


/* Address: 0x0040E3A0.
   Mounts the package archive path (next to the executable first, then as given) in the first free mount slot
   and reads its directory into a fresh PACKAGE_DIRECTORY_BYTES entry-header array. Lookups scan the slots in
   the same order, so earlier mounts win. Returns true and stores the file handle in *outFileHandleOrError;
   returns false with an error code there instead when no slot is free, the file cannot be opened or the
   allocation fails. outFileHandleOrError may be NULL.
*/
bool Package_Mount(uint16_t *path,uint32_t *outFileHandleOrError)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int slotsRemaining;
  PckMountSlot *mountSlot;
  FileSystemOpenResult openResult;
  ArenaAllocResult allocResult;

  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && mountSlot->fileHandle != 0) {
    mountSlot++;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    handle = (PckEntryHeader *)FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
  }
  else {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = g_FileSystemOpen
                      (FILESYSTEM_OPEN_WRITE_ACCESS,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (PckEntryHeader *)openResult.handleOrError;
    if (openResult.failed) {
      openResult = g_FileSystemOpen(FILESYSTEM_OPEN_WRITE_ACCESS,path);
      handle = (PckEntryHeader *)openResult.handleOrError;
    }
    if (!openResult.failed) {
      allocResult = g_MemoryApi.alloc(PACKAGE_DIRECTORY_BYTES);
      allocatedEntryHeaders = (PckEntryHeader *)allocResult.payloadOrError;
      if (!allocResult.failed) {
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
      handle = allocatedEntryHeaders;
    }
  }
  if (outFileHandleOrError != NULL) {
    *outFileHandleOrError = (uint32_t)handle;
  }
  return false;
}


/* Address: 0x0040EB70.
   Lists the entries of the mounted package fileHandle whose path matches pattern (Package_WildcardPathMatches):
   copies each path into a PCK_ENTRY_HEADER_BYTES output record while the capacity lasts and sorts the records
   by path (UTF-16 code-unit order). Returns true with the match count in *outMatchCount (each record is
   PCK_ENTRY_HEADER_BYTES); returns false when the handle is not mounted, leaving *outMatchCount unchanged
   (the original returned FATAL_ERROR_GENERAL_FAILURE with the caller's ECX, which no caller reads).
*/
bool Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                       uint16_t *pattern,EngineFileHandle fileHandle,uint32_t *outMatchCount)

{
  EngineFileHandle handleOrRemaining;
  uint16_t *firstPathCursor;
  uint16_t *secondPathCursor;
  int matchedCount;
  int remainingCount;
  int unitsRemaining;
  EngineFileHandle slotsRemaining;
  PckEntryCount entriesRemaining;
  int unsortedCount;
  PckMountSlot *mountSlot;
  PckEntryHeader *sourceCursor;
  PckEntryHeader *targetCursor;
  PckEntryHeader *entryCursor;
  PckEntryHeader *copyDestination;
  bool carryFlag;
  uint32_t swappedDword;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  handleOrRemaining = fileHandle; /* a zero handle fails at once, like running out of slots */
  while (handleOrRemaining != 0) {
    if (fileHandle == mountSlot->fileHandle) break;
    mountSlot++;
    slotsRemaining = slotsRemaining - 1;
    handleOrRemaining = slotsRemaining;
  }
  if (handleOrRemaining == 0) {
    return false;
  }
  entriesRemaining = mountSlot->entryCount;
  entryCursor = mountSlot->entryHeaders;
  matchedCount = 0;
  targetCursor = outputEntries;
  if (entriesRemaining != 0) {
    do {
      carryFlag = Package_WildcardPathMatches(pattern,entryCursor->path);
      if (!carryFlag) {
        /* output full: stop with the matches so far */
        carryFlag = outputCapacityBytes < PCK_ENTRY_HEADER_BYTES;
        outputCapacityBytes = outputCapacityBytes - PCK_ENTRY_HEADER_BYTES;
        if (carryFlag) goto returnMatches; /* Original quirk: these matches are returned unsorted */
        /* only the path is copied; the rest of the output record is left as it was */
        sourceCursor = entryCursor;
        copyDestination = targetCursor;
        for (remainingCount = PCK_ENTRY_PATH_UNITS; remainingCount != 0; remainingCount--) {
          copyDestination->path[0] = sourceCursor->path[0];
          sourceCursor = (PckEntryHeader *)(sourceCursor->path + 1);
          copyDestination = (PckEntryHeader *)(copyDestination->path + 1);
        }
        targetCursor++;
        matchedCount++;
      }
      entryCursor++;
      entriesRemaining = entriesRemaining - 1;
    } while (entriesRemaining != 0);
    /* exchange sort: every record is compared with each later one and swapped (0x80 dwords) when the later
       path is smaller; outputEntries walks down the list as the front becomes sorted */
    if (matchedCount != 0) {
      remainingCount = matchedCount - 1;
      carryFlag = false;
      if (remainingCount != 0) {
        entryCursor = outputEntries + 1;
        unsortedCount = matchedCount;
        do {
          do {
            unitsRemaining = PCK_ENTRY_HEADER_BYTES / 2; /* REPE CMPSW over the whole record */
            targetCursor = entryCursor;
            sourceCursor = outputEntries;
            do {
              if (unitsRemaining == 0) break;
              unitsRemaining--;
              firstPathCursor = sourceCursor->path;
              secondPathCursor = targetCursor->path;
              carryFlag = *secondPathCursor < *firstPathCursor;
              targetCursor = (PckEntryHeader *)(targetCursor->path + 1);
              sourceCursor = (PckEntryHeader *)(sourceCursor->path + 1);
            } while (*secondPathCursor == *firstPathCursor);
            if (carryFlag) {
              unitsRemaining = PCK_ENTRY_HEADER_BYTES / 4;
              do {
                sourceCursor = outputEntries;
                targetCursor = entryCursor;
                LOCK();
                swappedDword = *(uint32_t *)sourceCursor->path;
                *(uint32_t *)sourceCursor->path = *(uint32_t *)targetCursor->path;
                UNLOCK();
                *(uint32_t *)targetCursor->path = swappedDword;
                unitsRemaining--;
                entryCursor = (PckEntryHeader *)(targetCursor->path + 2);
                outputEntries = (PckEntryHeader *)(sourceCursor->path + 2);
              } while (unitsRemaining != 0);
              entryCursor = (PckEntryHeader *)(targetCursor[-1].path + 2);
              outputEntries = (PckEntryHeader *)(sourceCursor[-1].path + 2);
            }
            /* carry of entryCursor + sizeof(PckEntryHeader) */
            carryFlag = (PckEntryHeader *)(0xffffffffU - PCK_ENTRY_HEADER_BYTES) < entryCursor;
            entryCursor++;
            remainingCount--;
          } while (remainingCount != 0);
          remainingCount = unsortedCount - 2;
          unsortedCount--;
          entryCursor = outputEntries + 2;
          carryFlag = false;
          outputEntries = outputEntries + 1;
        } while (remainingCount != 0);
      }
    }
  }
returnMatches:
  *outMatchCount = matchedCount;
  return true;
}


/* Address: 0x0040E500.
   Unmounts the package fileHandle: frees its entry-header array, closes the file and clears the mount slot.
   Does nothing for a zero or unknown handle.
*/
void Package_Unmount(EngineFileHandle fileHandle)

{
  EngineFileHandle handleOrRemaining;
  EngineFileHandle mountSlotsRemaining;
  PckMountSlot *mountSlotCursor;
  
  mountSlotCursor = g_PackageMountSlots;
  mountSlotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  handleOrRemaining = fileHandle; /* a zero handle returns at once, like running out of slots */
  for (;;) {
    if (handleOrRemaining == 0) {
      return;
    }
    if (fileHandle == mountSlotCursor->fileHandle) break;
    mountSlotCursor++;
    mountSlotsRemaining = mountSlotsRemaining - 1;
    handleOrRemaining = mountSlotsRemaining;
  }
  g_MemoryApi.free(mountSlotCursor->entryHeaders);
  g_FileSystemClose((void *)fileHandle);
  mountSlotCursor->fileHandle = 0;
  mountSlotCursor->entryHeaders = NULL;
  mountSlotCursor->entryCount = 0;
  return;
}


/* Address: 0x0040ECA0.
   Compares a UTF-16 archive path against a pattern for the package entry search. '?' matches any one code
   unit; '*' only skips the candidate to its next dot or terminator (no full globbing), which is enough for
   patterns like "level\*.lev". The comparison is case-sensitive. CF clear means match.
*/
bool Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate)

{
  uint16_t patternCodeUnit;

  do {
    while (patternCodeUnit = *pattern, pattern++, patternCodeUnit == '*') {
      while (*candidate != '.' && *candidate != 0) {
        candidate++;
      }
    }
    if (patternCodeUnit != '?' && patternCodeUnit != *candidate) {
      return true; /* mismatch */
    }
    candidate++;
  } while (patternCodeUnit != 0);
  return false; /* both ended together */
}


/* Address: 0x0040EAF0.
   Reads the packed data of entry from the package fileHandle into g_PackageScratchBuffer and unpacks it into
   destination with the decoder of its compression method (g_PckDecoderTable). Returns true on success with the
   decoder's byte count in *outByteCount; on failure returns false with the seek, read or decoder error code in
   *outErrorCode and leaves the entry path in g_PackageLastErrorPath. Either out pointer may be NULL.
*/
bool Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle,
                             uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckCompressionMethod entryCompression;
  uint32_t decoderStatusCode;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  PckCodecResult decodeResult;

  seekResult = g_FileSystemSeek
                    (FILESYSTEM_SEEK_BEGIN,entry->runtimePayloadOffset + PCK_ENTRY_HEADER_BYTES,(void *)fileHandle);
  decoderStatusCode = seekResult.positionOrError;
  if (!seekResult.failed) {
    entryCompression = entry->compressionMethod;
    readResult = g_FileSystemReadExact(entry->packedSize,g_PackageScratchBuffer,(void *)fileHandle);
    decoderStatusCode = readResult.valueOrError;
    if (!readResult.failed) {
      decodeResult = g_PckDecoderTable[entryCompression]
                        (entry->unpackedSize,destination,entry->packedSize,g_PackageScratchBuffer);
      decoderStatusCode = decodeResult.byteCountOrError;
      if (!decodeResult.failed) {
        if (outByteCount != NULL) {
          *outByteCount = decodeResult.byteCountOrError;
        }
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


/* Address: 0x0040E2B0.
   Stores path in g_PackageLastErrorPath for the fatal-error message of a failed load. The length is measured
   in code units (at most 0x100, terminator included) but used as a byte count: the original copies twice as
   many code units as the path has (SUB EDI,ESI then REP MOVSW), running past the terminator and, for paths
   over 0x80 units, into g_FatalErrorDetail1Utf16 behind the 0x100-unit buffer.
*/
void Package_SetLastErrorPath(uint16_t *path)

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


/* Address: 0x0040E640.
   Finds the entry whose name equals path exactly (no wildcards, case-sensitive: package paths are stored in
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
  bool matched;
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
    /* REPE CMPSW over the path length including its terminator */
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


/* Address: 0x0040EA20.
   Finds path in the mounted packages, scanning the mount slots from the front so that the first mounted
   package that has the entry wins. The path is lowercased in place first (package paths are stored in lower
   case). Returns the entry header and stores the package's handle in *outFileHandle; returns NULL (leaving
   *outFileHandle unchanged) when the path is too long for an entry or no package has it. A found entry is
   never NULL.
   Original register convention: entry in EAX, handle in EBX, CF set on failure; ECX and EDX preserved.
*/
PckEntryHeader *Package_FindEntryAcrossMounts(uint16_t *path,EngineFileHandle *outFileHandle)

{
  uint32_t codeUnit;
  int remainingOrLength;
  int compareRemaining;
  int slotsRemaining;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  bool matched;
  PckEntryHeader *currentEntry;

  remainingOrLength = PCK_ENTRY_PATH_UNITS;
  pathCursor = path;
  do {
    compareRemaining = remainingOrLength;
    codeUnit = *pathCursor;
    if ('A' - 1 < codeUnit && codeUnit < 'Z' + 1) {
      codeUnit = codeUnit + ('a' - 'A');
    }
    *pathCursor = (uint16_t)codeUnit;
    remainingOrLength = compareRemaining - 1;
    if (remainingOrLength == 0) return NULL; /* path too long */
    pathCursor = pathCursor + 1;
  } while (codeUnit != 0);
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  /* the path length in code units, terminator included */
  remainingOrLength = -(compareRemaining + -(PCK_ENTRY_PATH_UNITS + 1));
  do {
    currentEntry = mountSlot->entryHeaders;
    entriesRemaining = mountSlot->entryCount;
    if ((currentEntry != NULL) && (entriesRemaining != 0)) {
      do {
        /* REPE CMPSW over the lowercased path length including its terminator. */
        matched = false;
        compareRemaining = remainingOrLength;
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
        entriesRemaining = entriesRemaining - 1;
      } while (entriesRemaining != 0);
    }
    mountSlot++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return NULL;
}


/* Address: 0x0040E570.
   Loads the directory of the mounted package fileHandle into its mount slot: reads the archive header for the
   entry count, then every entry header, recording the file offset of the entry (its header; the packed
   payload follows it) and seeking past the payload to the next header. Returns true on success; false with
   the file-system error, or FATAL_ERROR_GENERAL_FAILURE when fileHandle is not mounted, in *outErrorCode
   (which may be NULL). The original also returned the last seek position on success; no caller used it.
*/
bool Package_ReadDirectory(EngineFileHandle fileHandle,uint32_t *outErrorCode)

{
  PckStoredByteCount *packedSizeField;
  uint8_t *archiveHeader;
  uint32_t statusCode;
  PckEntryCount entriesRemaining;
  int slotsRemaining;
  FileSystemFilePosition entryHeaderOffset;
  PckMountSlot *mountSlot;
  PckEntryHeader *entryHeader;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;

  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  do {
    if (fileHandle == mountSlot->fileHandle) {
      entryHeader = mountSlot->entryHeaders;
      seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      archiveHeader = g_PackageScratchBuffer;
      statusCode = seekResult.positionOrError;
      if (seekResult.failed) goto fail;
      readResult = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,g_PackageScratchBuffer,(void *)fileHandle);
      statusCode = readResult.valueOrError;
      if (readResult.failed) goto fail;
      entriesRemaining = ((PckArchiveHeader *)archiveHeader)->entryCount;
      mountSlot->entryCount = entriesRemaining;
      /* each entry is its header followed directly by its packed payload */
      entryHeaderOffset = PCK_ENTRY_HEADER_BYTES;
      for (; entriesRemaining != 0; entriesRemaining = entriesRemaining - 1) {
        readResult = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,entryHeader,(void *)fileHandle);
        statusCode = readResult.valueOrError;
        if (readResult.failed) goto fail;
        packedSizeField = &entryHeader->packedSize;
        entryHeader->runtimePayloadOffset = entryHeaderOffset;
        entryHeader++;
        entryHeaderOffset = entryHeaderOffset + *packedSizeField + PCK_ENTRY_HEADER_BYTES;
        seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,entryHeaderOffset,(void *)fileHandle);
        statusCode = seekResult.positionOrError;
        if (seekResult.failed) goto fail;
      }
      return true;
    }
    mountSlot++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  statusCode = FATAL_ERROR_GENERAL_FAILURE;
fail:
  if (outErrorCode != NULL) {
    *outErrorCode = statusCode;
  }
  return false;
}

