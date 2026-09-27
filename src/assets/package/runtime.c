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

/* Address: 0x005460E0.
   Ownership: assets/package/runtime.
   Purpose: Handles level package validate and mount.
   Local calls: Package_Mount, Package_FindEntry, Package_LoadEntry, Package_Unmount.
   Cross-module calls: Resource_Release [assets/resource/runtime], TextResourcePage_LoadCompatibilityAliases
   [assets/text/resources].
*/
bool __thandor_cf_preserve_eax_ecx_edx LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16)

{
  uint32_t aliasAddressBase;
  EngineFileHandle fileHandle;
  int *allocation;
  bool failed;
  StatusResult mountResult;
  PackageLoadResult loadResult;
  PackageFindResult findResult;
  
  mountResult = Package_Mount(levelPathUtf16);
  failed = mountResult.failed;
  fileHandle = mountResult.valueOrError;
  if (!failed) {
    findResult = Package_FindEntry(0x200,(PckEntryHeader *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                              (uint16_t *)u_level___lev_005460a6,fileHandle);
    if ((!findResult.failed) && (findResult.matchCount != 0)) {
      loadResult = Package_LoadEntry((uint16_t *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15));
      allocation = loadResult.bufferOrError;
      if (!loadResult.failed) {
        if ((*allocation == 0x76656c) && (allocation[3] == 0x70001)) {
          aliasAddressBase = allocation[0x5c];
          Resource_Release(allocation);
          findResult = Package_FindEntry(0x200,(PckEntryHeader *)
                                          (s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                                    (uint16_t *)u_level___str_005460be,fileHandle);
          if (((!findResult.failed) && (findResult.matchCount != 0)) &&
             (failed = TextResourcePage_LoadCompatibilityAliases
                                (aliasAddressBase,(uint16_t *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15))
             , !failed)) {
            return failed;
          }
        }
        else {
          Resource_Release(allocation);
        }
      }
    }
    Package_Unmount(fileHandle);
    failed = true;
  }
  return failed;
}


/* Address: 0x0040E840.
   Ownership: assets/package/runtime.
   Purpose: Replaces an existing path when present, then appends a new 0x200-byte entry header and encoded payload.
   compressionMethod indexes the verified three-entry encoder table; method 1 writes a four-byte-aligned stored
   payload directly. The first payload dword becomes typeTag. CF reports failure and the mounted directory is
   refreshed after success.
   Local calls: Package_FindEntryInMount, Package_DeleteEntry, Package_ReadDirectory.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle)

{
  uint8_t *destination;
  uint32_t errorCode;
  FileIoByteCount byteCount;
  uint32_t alignedByteCount;
  int dwordsRemaining;
  uint8_t *nameDestination;
  PackageMountEntryResult findResult;
  StatusResult statusResult;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  PckCodecResult encodeResult;
  FileSystemWriteResult writeResult;
  
  destination = g_PackageScratchBuffer;
  findResult = Package_FindEntryInMount(path,fileHandle);
  if (!findResult.notFound) {
    statusResult = Package_DeleteEntry(path,fileHandle);
    errorCode = statusResult.valueOrError;
    if (statusResult.failed) goto Package_UpsertEntry_Fail;
  }
  seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  errorCode = seekResult.positionOrError;
  if (!seekResult.failed) {
    readResult = g_FileSystemReadExact(0x200,destination,(void *)fileHandle);
    errorCode = readResult.valueOrError;
    if (!readResult.failed) {
      seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      errorCode = seekResult.positionOrError;
      if (!seekResult.failed) {
        *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + 1;
        if (compressionMethod == PCK_COMPRESSION_STORED) {
          alignedByteCount = unpackedSize + 3 & 0xfffffffc;
          *(uint32_t *)(destination + 0x3f8) = alignedByteCount;
          destination[0x3fc] = 1;
          destination[0x3fd] = 0;
          destination[0x3fe] = 0;
          destination[0x3ff] = 0;
          *(uint32_t *)(destination + 4) = *(int *)(destination + 4) + alignedByteCount + 0x200;
          destination[0x3ec] = 0;
          destination[0x3ed] = 0;
          destination[0x3ee] = 0;
          destination[0x3ef] = 0;
          *(uint32_t *)(destination + 0x3f4) = *sourceData;
          *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
          writeResult = g_FileSystemWriteExactOrFlush(0x200,destination,(void *)fileHandle);
          errorCode = writeResult.valueOrError;
          if (writeResult.failed) goto Package_UpsertEntry_Fail;
          nameDestination = destination + 0x200;
          for (dwordsRemaining = 0x7b; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
            *(uint32_t *)nameDestination = *(uint32_t *)path;
            path = path + 2;
            nameDestination = nameDestination + 4;
          }
          seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          errorCode = seekResult.positionOrError;
          if (seekResult.failed) goto Package_UpsertEntry_Fail;
          writeResult = g_FileSystemWriteExactOrFlush(0x200,destination + 0x200,(void *)fileHandle);
          errorCode = writeResult.valueOrError;
          if (writeResult.failed) goto Package_UpsertEntry_Fail;
          writeResult = g_FileSystemWriteExactOrFlush(alignedByteCount,sourceData,(void *)fileHandle);
          errorCode = writeResult.valueOrError;
          if (writeResult.failed) goto Package_UpsertEntry_Fail;
        }
        else {
          encodeResult = g_PckEncoderTable[compressionMethod]
                            (0x7ffc00,destination + 0x400,unpackedSize,(uint8_t *)sourceData);
          errorCode = encodeResult.byteCountOrError;
          if (encodeResult.failed) goto Package_UpsertEntry_Fail;
          *(uint32_t *)(destination + 0x3f8) = errorCode;
          byteCount = errorCode + 0x200;
          *(PckCompressionMethod *)(destination + 0x3fc) = compressionMethod;
          *(FileIoByteCount *)(destination + 4) = *(int *)(destination + 4) + byteCount;
          destination[0x3ec] = 0;
          destination[0x3ed] = 0;
          destination[0x3ee] = 0;
          destination[0x3ef] = 0;
          *(uint32_t *)(destination + 0x3f4) = *sourceData;
          *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
          writeResult = g_FileSystemWriteExactOrFlush(0x200,destination,(void *)fileHandle);
          errorCode = writeResult.valueOrError;
          if (writeResult.failed) goto Package_UpsertEntry_Fail;
          nameDestination = destination + 0x200;
          for (dwordsRemaining = 0x7b; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
            *(uint32_t *)nameDestination = *(uint32_t *)path;
            path = path + 2;
            nameDestination = nameDestination + 4;
          }
          seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          errorCode = seekResult.positionOrError;
          if (seekResult.failed) goto Package_UpsertEntry_Fail;
          writeResult = g_FileSystemWriteExactOrFlush
                            (byteCount,destination + 0x200,(void *)fileHandle);
          errorCode = writeResult.valueOrError;
          if (writeResult.failed) goto Package_UpsertEntry_Fail;
        }
        statusResult = Package_ReadDirectory(fileHandle);
        errorCode = statusResult.valueOrError;
        if (!statusResult.failed) {
          return statusResult;
        }
      }
    }
  }
Package_UpsertEntry_Fail:
  statusResult.failed = true;
  statusResult.valueOrError = errorCode;
  return statusResult;
}


/* Address: 0x0040ED00.
   Loads path into a caller buffer of the given capacity: from the first mounted package that has it, otherwise
   from the loose file (PACKAGE_LOAD_* flags in the top two bits of the capacity select loose-only loading and a
   first try next to the executable). Returns the byte count; CF set with an error code, FATAL_ERROR_OUT_OF_MEMORY
   when the entry does not fit (or is to be decoded into g_PackageScratchBuffer, which holds the packed data).
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path)

{
  void *bufferCapacity;
  PckEntryHeader *entry;
  void *handle;
  void *byteCount;
  PackageDecodeResult decodeResult;
  StatusResult decodeStatus;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  FileSystemReadResult readResult;
  StatusResult successResult;
  StatusResult failureResult;
  PackageEntryLookupResult findResult;
  
  bufferCapacity = (void *)(bufferCapacityAndLoadFlags & PACKAGE_LOAD_CAPACITY_MASK);
  if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_SKIP_PACKAGES) == 0) {
    findResult = Package_FindEntryAcrossMounts(path);
    entry = (PckEntryHeader *)findResult.entry;
    if (!findResult.notFound) {
      handle = (void *)FATAL_ERROR_OUT_OF_MEMORY;
      if ((((void *)entry->unpackedSize <= bufferCapacity) &&
           (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1)) &&
         (destination != g_PackageScratchBuffer)) {
        decodeResult = Package_DecodeEntryInto(destination,entry,findResult.fileHandle);
        decodeStatus.valueOrError = decodeResult.valueOrError;
        decodeStatus.failed = decodeResult.failed;
        return decodeStatus;
      }
      Package_SetLastErrorPath(path);
      goto Package_LoadEntryIntoBuffer_Fail;
    }
  }
  if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_EXECUTABLE_DIRECTORY_FIRST) == 0) {
    openResult = g_FileSystemOpen(0,path);
    handle = (void *)openResult.handleOrError;
    if (openResult.failed) goto Package_LoadEntryIntoBuffer_Fail;
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
      if (openResult.failed) goto Package_LoadEntryIntoBuffer_Fail;
    }
  }
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
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)byteCount;
        return successResult;
      }
    }
  }
  g_FileSystemClose(handle);
  handle = byteCount;
Package_LoadEntryIntoBuffer_Fail:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)handle;
  return failureResult;
}


/* Address: 0x0040E450.
   Like Package_Mount, but takes the last free mount slot: lookups scan the table from the front, so this
   archive loses against every other one. FileSystem_Init mounts engine.pck this way.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx Package_MountLowPriority(uint16_t *path)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int slotsRemaining;
  PckMountSlot *mountSlot;
  StatusResult failureResult;
  FileSystemOpenResult openResult;
  ArenaAllocResult allocResult;
  StatusResult successResult;
  
  mountSlot = g_PackageMountSlots + (PACKAGE_MOUNT_SLOT_COUNT - 1);
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  do {
    if (mountSlot->fileHandle == 0) {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      openResult = g_FileSystemOpen
                        (FILESYSTEM_OPEN_WRITE_ACCESS,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (PckEntryHeader *)openResult.handleOrError;
      if (openResult.failed) {
        openResult = g_FileSystemOpen(FILESYSTEM_OPEN_WRITE_ACCESS,path);
        handle = (PckEntryHeader *)openResult.handleOrError;
        if (openResult.failed) goto Package_MountLowPriority_Fail;
      }
      allocResult = g_MemoryApi.alloc(PACKAGE_DIRECTORY_BYTES);
      allocatedEntryHeaders = (PckEntryHeader *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        mountSlot->fileHandle = (EngineFileHandle)handle;
        mountSlot->entryHeaders = allocatedEntryHeaders;
        mountSlot->entryCount = 0;
        Package_ReadDirectory((EngineFileHandle)handle);
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)handle;
        return successResult;
      }
      g_FileSystemClose(handle);
      handle = allocatedEntryHeaders;
      goto Package_MountLowPriority_Fail;
    }
    mountSlot--;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  handle = (PckEntryHeader *)FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
Package_MountLowPriority_Fail:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)handle;
  return failureResult;
}


/* Address: 0x0040E6F0.
   Ownership: assets/package/runtime.
   Purpose: Deletes one exact path from a writable mounted PCK. Missing entries are treated as success. The routine
   rewrites the archive header, compacts all following bytes through the shared scratch buffer, truncates the file,
   and reloads the in-memory directory. CF reports file or rewrite failure.
   Local calls: Package_FindEntryInMount, Package_ReadDirectory.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle)

{
  PckStoredByteCount entryPackedSize;
  int archiveEndOffset;
  uint8_t *destination;
  PckEntryHeader *foundEntry;
  FileSystemFilePosition distance;
  PckEntryHeader *statusOrError;
  uint32_t byteCount;
  PackageMountEntryResult findResult;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  FileSystemWriteResult writeResult;
  StatusResult statusResult;
  StatusResult successResult;
  
  destination = g_PackageScratchBuffer;
  findResult = Package_FindEntryInMount(path,fileHandle);
  foundEntry = findResult.entry;
  statusOrError = foundEntry;
  if (findResult.notFound) {
    /* A missing entry counts as deleted. */
    successResult.failed = false;
    successResult.valueOrError = (uint32_t)statusOrError;
    return successResult;
  }
  seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  statusOrError = (PckEntryHeader *)seekResult.positionOrError;
  if (!seekResult.failed) {
    entryPackedSize = foundEntry->packedSize;
    readResult = g_FileSystemReadExact(0x200,destination,(void *)fileHandle);
    statusOrError = (PckEntryHeader *)readResult.valueOrError;
    if (!readResult.failed) {
      archiveEndOffset = *(int *)(destination + 4);
      *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + -1;
      *(PckStoredByteCount *)(destination + 4) = *(int *)(destination + 4) - (entryPackedSize + 0x200);
      seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      statusOrError = (PckEntryHeader *)seekResult.positionOrError;
      if (!seekResult.failed) {
        writeResult = g_FileSystemWriteExactOrFlush(0x200,destination,(void *)fileHandle);
        statusOrError = (PckEntryHeader *)writeResult.valueOrError;
        if (!writeResult.failed) {
          distance = foundEntry->runtimePayloadOffset + foundEntry->packedSize + 0x200;
          byteCount = archiveEndOffset - distance;
          if (byteCount == 0) {
            seekResult = g_FileSystemSeek
                              (FILESYSTEM_SEEK_BEGIN,foundEntry->runtimePayloadOffset,(void *)fileHandle
                              );
            statusOrError = (PckEntryHeader *)seekResult.positionOrError;
            if (seekResult.failed) goto Package_DeleteEntry_Fail;
            writeResult = g_FileSystemWriteExactOrFlush(0,(void *)0x0,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.valueOrError;
            if (writeResult.failed) goto Package_DeleteEntry_Fail;
          }
          else {
            seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,distance,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)seekResult.positionOrError;
            if ((seekResult.failed) || (statusOrError = (PckEntryHeader *)0x14, 0x800000 < byteCount))
            goto Package_DeleteEntry_Fail;
            readResult = g_FileSystemReadExact(byteCount,g_PackageScratchBuffer,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)readResult.valueOrError;
            if (readResult.failed) goto Package_DeleteEntry_Fail;
            seekResult = g_FileSystemSeek
                              (FILESYSTEM_SEEK_BEGIN,foundEntry->runtimePayloadOffset,(void *)fileHandle
                              );
            statusOrError = (PckEntryHeader *)seekResult.positionOrError;
            if (seekResult.failed) goto Package_DeleteEntry_Fail;
            writeResult = g_FileSystemWriteExactOrFlush
                              (byteCount,g_PackageScratchBuffer,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.valueOrError;
            if (writeResult.failed) goto Package_DeleteEntry_Fail;
            writeResult = g_FileSystemWriteExactOrFlush(0,(void *)0x0,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.valueOrError;
            if (writeResult.failed) goto Package_DeleteEntry_Fail;
          }
          statusResult = Package_ReadDirectory(fileHandle);
          statusOrError = (PckEntryHeader *)statusResult.valueOrError;
          if (!statusResult.failed) {
            successResult.failed = false;
            successResult.valueOrError = (uint32_t)statusOrError;
            return successResult;
          }
        }
      }
    }
  }
Package_DeleteEntry_Fail:
  statusResult.failed = true;
  statusResult.valueOrError = (uint32_t)statusOrError;
  return statusResult;
}


/* Address: 0x0040EE30.
   Loads an asset into a newly allocated buffer: from the first mounted package that has the path, otherwise as
   a loose file (first relative to the executable directory, then as given). The buffer is untyped here;
   callers cast it to their gfx, fld, lev, mdl, sound, text, ... layout. CF set: bufferOrError is an error code.
*/
PackageLoadResult __thandor_eax_cf_preserve_ecx_edx Package_LoadEntry(uint16_t *path)

{
  PckEntryHeader *entry;
  /* the loose file's handle or the package entry's buffer; on failure the error code */
  uint8_t *handleBufferOrError;
  /* the loose file's size, then the error code of the failed step */
  uint8_t *byteCountOrError;
  ArenaAllocResult allocResult;
  PackageDecodeResult decodeResult;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  FileSystemReadResult readResult;
  PackageLoadResult failureResult;
  PackageLoadResult successResult;
  PackageEntryLookupResult findResult;

  findResult = Package_FindEntryAcrossMounts(path);
  entry = (PckEntryHeader *)findResult.entry;
  if (findResult.notFound) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    handleBufferOrError = (uint8_t *)openResult.handleOrError;
    if (openResult.failed) {
      openResult = g_FileSystemOpen(0,path);
      handleBufferOrError = (uint8_t *)openResult.handleOrError;
      if (openResult.failed) goto Package_LoadEntry_Fail;
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
        decodeResult = Package_DecodeEntryInto(handleBufferOrError,entry,findResult.fileHandle);
        if (!decodeResult.failed) {
          successResult.failed = false;
          successResult.bufferOrError = handleBufferOrError;
          return successResult;
        }
        byteCountOrError = (uint8_t *)decodeResult.valueOrError;
        g_MemoryApi.free(handleBufferOrError);
        handleBufferOrError = byteCountOrError;
      }
    }
  }
Package_LoadEntry_Fail:
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
   the same order, so earlier mounts win. Returns the file handle; CF set with an error code when no slot is
   free, the file cannot be opened or the allocation fails.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx Package_Mount(uint16_t *path)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int slotsRemaining;
  PckMountSlot *mountSlot;
  StatusResult failureResult;
  FileSystemOpenResult openResult;
  ArenaAllocResult allocResult;
  StatusResult successResult;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  do {
    if (mountSlot->fileHandle == 0) {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      openResult = g_FileSystemOpen
                        (FILESYSTEM_OPEN_WRITE_ACCESS,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (PckEntryHeader *)openResult.handleOrError;
      if (openResult.failed) {
        openResult = g_FileSystemOpen(FILESYSTEM_OPEN_WRITE_ACCESS,path);
        handle = (PckEntryHeader *)openResult.handleOrError;
        if (openResult.failed) goto Package_Mount_Fail;
      }
      allocResult = g_MemoryApi.alloc(PACKAGE_DIRECTORY_BYTES);
      allocatedEntryHeaders = (PckEntryHeader *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        mountSlot->fileHandle = (EngineFileHandle)handle;
        mountSlot->entryHeaders = allocatedEntryHeaders;
        mountSlot->entryCount = 0;
        Package_ReadDirectory((EngineFileHandle)handle);
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)handle;
        return successResult;
      }
      g_FileSystemClose(handle);
      handle = allocatedEntryHeaders;
      goto Package_Mount_Fail;
    }
    mountSlot++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  handle = (PckEntryHeader *)FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
Package_Mount_Fail:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)handle;
  return failureResult;
}


/* Address: 0x0040EB70.
   Lists the entries of the mounted package fileHandle whose path matches pattern (Package_WildcardPathMatches):
   copies each path into a PCK_ENTRY_HEADER_BYTES output record while the capacity lasts and sorts the records
   by path (UTF-16 code-unit order). Returns the record size with the match count in ECX; CF set with
   FATAL_ERROR_GENERAL_FAILURE when the handle is not mounted.
*/
PackageFindResult __thandor_eax_cf_preserve_edx
Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                 uint16_t *pattern,EngineFileHandle fileHandle)

{
  EngineFileHandle handleOrRemaining;
  uint16_t *firstPathCursor;
  uint16_t *secondPathCursor;
  /* Handle not mounted (CF set): the original leaves the caller's ECX as the match count; every caller ignores
     it then, so zero stands in for it. */
  uint32_t unmountedMatchCount = 0;
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
  PackageFindResult failureResult;
  PackageFindResult successResult;
  uint32_t swappedDword;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  handleOrRemaining = fileHandle; /* a zero handle fails at once, like running out of slots */
  while( true ) {
    if (handleOrRemaining == 0) {
      failureResult.matchCount = unmountedMatchCount;
      failureResult.recordSizeOrError = FATAL_ERROR_GENERAL_FAILURE;
      failureResult.failed = true;
      return failureResult;
    }
    if (fileHandle == mountSlot->fileHandle) break;
    mountSlot++;
    slotsRemaining = slotsRemaining - 1;
    handleOrRemaining = slotsRemaining;
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
        if (carryFlag) goto Package_FindEntry_ReturnMatches;
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
            carryFlag = (PckEntryHeader *)0xfffffdff < entryCursor;
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
Package_FindEntry_ReturnMatches:
  successResult.matchCount = matchedCount;
  successResult.recordSizeOrError = PCK_ENTRY_HEADER_BYTES;
  successResult.failed = false;
  return successResult;
}


/* Address: 0x0040E500.
   Unmounts the package fileHandle: frees its entry-header array, closes the file and clears the mount slot.
   Does nothing for a zero or unknown handle.
*/
void __thandor_preserve_eax_edx Package_Unmount(EngineFileHandle fileHandle)

{
  EngineFileHandle handleOrRemaining;
  EngineFileHandle mountSlotsRemaining;
  PckMountSlot *mountSlotCursor;
  
  mountSlotCursor = g_PackageMountSlots;
  mountSlotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  handleOrRemaining = fileHandle; /* a zero handle returns at once, like running out of slots */
  while( true ) {
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
   Ownership: assets/package/runtime.
   Purpose: Compares a UTF-16 archive path against a pattern. '?' matches one word. '*' advances the candidate to
   the next dot or terminator rather than implementing unrestricted globbing. CF clear means match.
*/
bool __thandor_cf_preserve_eax_ecx_edx Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate)

{
  uint16_t patternCodeUnit;
  
  while( true ) {
    while( true ) {
      patternCodeUnit = *pattern;
      pattern = pattern + 1;
      if (patternCodeUnit != 0x2a) break;
      for (; (*candidate != 0x2e && (*candidate != 0)); candidate = candidate + 1) {
      }
    }
    if ((patternCodeUnit != 0x3f) && (patternCodeUnit != *candidate)) break;
    candidate = candidate + 1;
    if (patternCodeUnit == 0) {
      return false;
    }
  }
  return true;
}


/* Address: 0x0040EAF0.
   Reads the packed data of entry from the package fileHandle into g_PackageScratchBuffer and unpacks it into
   destination with the decoder of its compression method (g_PckDecoderTable). Returns the decoder result; on
   failure the entry path is left in g_PackageLastErrorPath and CF is set.
*/
PackageDecodeResult __thandor_eax_cf_preserve_ecx_edx
Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle)

{
  PckCompressionMethod entryCompression;
  uint32_t decoderStatusCode;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  PckCodecResult decodeResult;
  PackageDecodeResult successResult;
  PackageDecodeResult failureResult;
  
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
        successResult.valueOrError = decodeResult.byteCountOrError;
        successResult.failed = decodeResult.failed;
        return successResult;
      }
    }
  }
  Package_SetLastErrorPath(entry->path);
  failureResult.failed = true;
  failureResult.valueOrError = decoderStatusCode;
  return failureResult;
}


/* Address: 0x0040E2B0.
   Stores path in g_PackageLastErrorPath for the fatal-error message of a failed load. The length is measured
   in code units (at most 0x100, terminator included) but used as a byte count: the original copies twice as
   many code units as the path has (SUB EDI,ESI then REP MOVSW), running past the terminator and, for paths
   over 0x80 units, into g_FatalErrorDetail1Utf16 behind the 0x100-unit buffer.
*/
void __thandor_void_preserve_eax_ecx_edx Package_SetLastErrorPath(uint16_t *path)

{
  uint16_t codeUnit;
  int remainingCount;
  uint16_t *scanEnd;
  uint16_t *wordCursor;
  
  remainingCount = 0x100;
  wordCursor = path;
  do {
    scanEnd = wordCursor;
    if (remainingCount == 0) break;
    remainingCount--;
    scanEnd = wordCursor + 1;
    codeUnit = *wordCursor;
    wordCursor = scanEnd;
  } while (codeUnit != 0);
  remainingCount = (int)scanEnd - (int)path; /* bytes, used as a code-unit count below */
  wordCursor = g_PackageLastErrorPath;
  for (; remainingCount != 0; remainingCount--) {
    *wordCursor = *path;
    path++;
    wordCursor++;
  }
  return;
}


/* Address: 0x0040E640.
   Ownership: assets/package/runtime.
   Purpose: Finds one exact lowercase UTF-16 path in the selected mounted archive. CF clear returns a
   PckEntryHeader pointer; CF set indicates failure.
*/
PackageMountEntryResult __thandor_eax_cf_preserve_ecx_edx
Package_FindEntryInMount(uint16_t *path,EngineFileHandle fileHandle)

{
  int lengthRemaining;
  int remainingCount;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  bool matched;
  PackageMountEntryResult notFoundResult;
  PackageMountEntryResult foundResult;
  PackageMountEntryResult emptyMountResult;
  PackageMountEntryResult noMountResult;
  PackageMountEntryResult pathTooLongResult;
  PckEntryHeader *currentEntry;
  
  matched = true;
  lengthRemaining = 0xf6;
  pathCursor = path;
  do {
    if (lengthRemaining == 0) break;
    lengthRemaining = lengthRemaining + -1;
    matched = *pathCursor == 0;
    pathCursor = pathCursor + 1;
  } while (!matched);
  if (!matched) {
    pathTooLongResult.entry = (PckEntryHeader *)0x0;
    pathTooLongResult.notFound = true;
    return pathTooLongResult;
  }
  mountSlot = g_PackageMountSlots;
  remainingCount = 0x400;
  while (fileHandle != mountSlot->fileHandle) {
    mountSlot = mountSlot + 1;
    remainingCount = remainingCount + -1;
    if (remainingCount == 0) {
      noMountResult.entry = (PckEntryHeader *)0x0;
      noMountResult.notFound = true;
      return noMountResult;
    }
  }
  currentEntry = mountSlot->entryHeaders;
  entriesRemaining = mountSlot->entryCount;
  if (entriesRemaining == 0) {
    emptyMountResult.entry = (PckEntryHeader *)0x0;
    emptyMountResult.notFound = true;
    return emptyMountResult;
  }
  do {
    /* REPE CMPSW over the path length including its terminator. */
    matched = false;
    remainingCount = -(lengthRemaining + -0xf6);
    pathCursor = path;
    nameCursor = currentEntry->path;
    while (remainingCount != 0) {
      matched = *pathCursor == *nameCursor;
      remainingCount = remainingCount + -1;
      pathCursor = pathCursor + 1;
      nameCursor = nameCursor + 1;
      if (!matched) break;
    }
    if (matched) {
      foundResult.notFound = false;
      foundResult.entry = currentEntry;
      return foundResult;
    }
    currentEntry = currentEntry + 1;
    entriesRemaining = entriesRemaining - 1;
  } while (entriesRemaining != 0);
  notFoundResult.entry = (PckEntryHeader *)0x0;
  notFoundResult.notFound = true;
  return notFoundResult;
}


/* Address: 0x0040EA20.
   Finds path in the mounted packages, scanning the mount slots from the front so that the first mounted
   package that has the entry wins. The path is lowercased in place first (package paths are stored in lower
   case). Returns the entry header and, in EBX, the package handle; CF set when the path is too long for an
   entry or no package has it.
*/
PackageEntryLookupResult __thandor_eax_ebx_cf_preserve_ecx_edx
Package_FindEntryAcrossMounts(uint16_t *path)

{
  uint32_t codeUnit;
  int remainingOrLength;
  int compareRemaining;
  int slotsRemaining;
  /* EBX on failure: the last slot's entry count (0 once a slot was scanned to the end). When the path is too
     long the original leaves the caller's EBX; callers never read EBX with CF set, so zero stands in for it. */
  PckEntryCount failureEntryCount = 0;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  bool matched;
  PackageEntryLookupResult foundResult;
  PackageEntryLookupResult notFoundResult;
  PckEntryHeader *currentEntry;

  remainingOrLength = PCK_ENTRY_PATH_UNITS;
  pathCursor = path;
  do {
    compareRemaining = remainingOrLength;
    codeUnit = *pathCursor;
    if ((0x40 < codeUnit) && (codeUnit < 0x5b)) { /* 'A'..'Z' */
      codeUnit = codeUnit + 0x20;
    }
    *pathCursor = (uint16_t)codeUnit;
    remainingOrLength = compareRemaining - 1;
    if (remainingOrLength == 0) goto Package_FindEntryAcrossMounts_NotFound;
    pathCursor = pathCursor + 1;
  } while (codeUnit != 0);
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  /* the path length in code units, terminator included */
  remainingOrLength = -(compareRemaining + -(PCK_ENTRY_PATH_UNITS + 1));
  do {
    currentEntry = mountSlot->entryHeaders;
    entriesRemaining = mountSlot->entryCount;
    failureEntryCount = entriesRemaining;
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
          foundResult.fileHandle = mountSlot->fileHandle;
          foundResult.entry = (uint32_t)currentEntry;
          foundResult.notFound = false;
          return foundResult;
        }
        currentEntry++;
        entriesRemaining = entriesRemaining - 1;
      } while (entriesRemaining != 0);
      failureEntryCount = 0;
    }
    mountSlot++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
Package_FindEntryAcrossMounts_NotFound:
  notFoundResult.fileHandle = failureEntryCount;
  notFoundResult.entry = codeUnit;
  notFoundResult.notFound = true;
  return notFoundResult;
}


/* Address: 0x0040E570.
   Ownership: assets/package/runtime.
   Purpose: Reads the 0x200-byte archive header, caches entryCount, reads every 0x200-byte entry header, and fills
   runtimePayloadOffset while skipping each packed payload.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Package_ReadDirectory(EngineFileHandle fileHandle)

{
  PckStoredByteCount *packedSizeField;
  uint8_t *archiveHeader;
  uint32_t statusCode;
  PckEntryCount entriesRemaining;
  int slotsRemaining;
  FileSystemFilePosition distance;
  PckMountSlot *mountSlot;
  PckEntryHeader *destination;
  StatusResult failureResult;
  FileSystemSeekResult seekResult;
  FileSystemReadResult readResult;
  StatusResult successResult;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = 0x400;
  do {
    if (fileHandle == mountSlot->fileHandle) {
      destination = mountSlot->entryHeaders;
      seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      archiveHeader = g_PackageScratchBuffer;
      statusCode = seekResult.positionOrError;
      if (seekResult.failed) goto Package_ReadDirectory_Fail;
      readResult = g_FileSystemReadExact(0x200,g_PackageScratchBuffer,(void *)fileHandle);
      statusCode = readResult.valueOrError;
      if (readResult.failed) goto Package_ReadDirectory_Fail;
      entriesRemaining = *(PckEntryCount *)(archiveHeader + 0xb0);
      mountSlot->entryCount = entriesRemaining;
      distance = 0x200;
      for (; entriesRemaining != 0; entriesRemaining = entriesRemaining - 1) {
        readResult = g_FileSystemReadExact(0x200,destination,(void *)fileHandle);
        statusCode = readResult.valueOrError;
        if (readResult.failed) goto Package_ReadDirectory_Fail;
        packedSizeField = &destination->packedSize;
        destination->runtimePayloadOffset = distance;
        destination = destination + 1;
        distance = distance + *packedSizeField + 0x200;
        seekResult = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,distance,(void *)fileHandle);
        statusCode = seekResult.positionOrError;
        if (seekResult.failed) goto Package_ReadDirectory_Fail;
      }
      successResult.failed = false;
      successResult.valueOrError = statusCode;
      return successResult;
    }
    mountSlot = mountSlot + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  statusCode = 0x14;
Package_ReadDirectory_Fail:
  failureResult.failed = true;
  failureResult.valueOrError = statusCode;
  return failureResult;
}

