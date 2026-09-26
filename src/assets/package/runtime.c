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
bool __thandor_cf_preserve_eax_ecx_edx LevelPackage_ValidateAndMount(word *levelPathUtf16)

{
  dword aliasAddressBase;
  EngineFileHandle fileHandle;
  int *allocation;
  bool failed;
  StatusValueEaxCf5 mountResult;
  PackageLoadEntryEaxCf5 loadResult;
  PackageFindEntryEaxEcxCf9 findResult;
  
  mountResult = Package_Mount(levelPathUtf16);
  failed = mountResult.carry;
  fileHandle = mountResult.valueOrError;
  if (!failed) {
    findResult = Package_FindEntry(0x200,(PckEntryHeader *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                              (word *)u_level___lev_005460a6,fileHandle);
    if ((!findResult.carry) && (findResult.matchCount != 0)) {
      loadResult = Package_LoadEntry((word *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15));
      allocation = loadResult.bufferOrError;
      if (!loadResult.carry) {
        if ((*allocation == 0x76656c) && (allocation[3] == 0x70001)) {
          aliasAddressBase = allocation[0x5c];
          Resource_Release(allocation);
          findResult = Package_FindEntry(0x200,(PckEntryHeader *)
                                          (s_NAME__CLIENT__KARTE___00545e91 + 0x15),
                                    (word *)u_level___str_005460be,fileHandle);
          if (((!findResult.carry) && (findResult.matchCount != 0)) &&
             (failed = TextResourcePage_LoadCompatibilityAliases
                                (aliasAddressBase,(word *)(s_NAME__CLIENT__KARTE___00545e91 + 0x15))
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   dword *sourceData,word *path,EngineFileHandle fileHandle)

{
  byte *destination;
  dword errorCode;
  FileIoByteCount byteCount;
  uint alignedByteCount;
  int dwordsRemaining;
  byte *nameDestination;
  PackageEntryEaxCf5 findResult;
  StatusValueEaxCf5 statusResult;
  FileSystemSeekEaxCf5 seekResult;
  FileSystemReadEaxCf5 readResult;
  PckCodecEaxCf5 encodeResult;
  FileSystemWriteEaxCf5 writeResult;
  
  destination = g_PackageScratchBuffer;
  findResult = Package_FindEntryInMount(path,fileHandle);
  if (!findResult.carry) {
    statusResult = Package_DeleteEntry(path,fileHandle);
    errorCode = statusResult.valueOrError;
    if (statusResult.carry) goto LAB_0040ea0c;
  }
  seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  errorCode = seekResult.eax;
  if (!seekResult.carry) {
    readResult = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
    errorCode = readResult.eax;
    if (!readResult.carry) {
      seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      errorCode = seekResult.eax;
      if (!seekResult.carry) {
        *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + 1;
        if (compressionMethod == PCK_COMPRESSION_STORED) {
          alignedByteCount = unpackedSize + 3 & 0xfffffffc;
          *(uint *)(destination + 0x3f8) = alignedByteCount;
          destination[0x3fc] = 1;
          destination[0x3fd] = 0;
          destination[0x3fe] = 0;
          destination[0x3ff] = 0;
          *(uint *)(destination + 4) = *(int *)(destination + 4) + alignedByteCount + 0x200;
          destination[0x3ec] = 0;
          destination[0x3ed] = 0;
          destination[0x3ee] = 0;
          destination[0x3ef] = 0;
          *(dword *)(destination + 0x3f4) = *sourceData;
          *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
          writeResult = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
          errorCode = writeResult.eax;
          if (writeResult.carry) goto LAB_0040ea0c;
          nameDestination = destination + 0x200;
          for (dwordsRemaining = 0x7b; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
            *(undefined4 *)nameDestination = *(undefined4 *)path;
            path = path + 2;
            nameDestination = nameDestination + 4;
          }
          seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          errorCode = seekResult.eax;
          if (seekResult.carry) goto LAB_0040ea0c;
          writeResult = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination + 0x200,(void *)fileHandle);
          errorCode = writeResult.eax;
          if (writeResult.carry) goto LAB_0040ea0c;
          writeResult = (*g_FileSystemWriteExactOrFlushCf)(alignedByteCount,sourceData,(void *)fileHandle);
          errorCode = writeResult.eax;
          if (writeResult.carry) goto LAB_0040ea0c;
        }
        else {
          encodeResult = (*g_PckEncoderTable[compressionMethod])
                            (0x7ffc00,destination + 0x400,unpackedSize,(byte *)sourceData);
          errorCode = encodeResult.eax;
          if (encodeResult.carry) goto LAB_0040ea0c;
          *(dword *)(destination + 0x3f8) = errorCode;
          byteCount = errorCode + 0x200;
          *(PckCompressionMethod *)(destination + 0x3fc) = compressionMethod;
          *(FileIoByteCount *)(destination + 4) = *(int *)(destination + 4) + byteCount;
          destination[0x3ec] = 0;
          destination[0x3ed] = 0;
          destination[0x3ee] = 0;
          destination[0x3ef] = 0;
          *(dword *)(destination + 0x3f4) = *sourceData;
          *(PckDecodedByteCount *)(destination + 0x3f0) = unpackedSize;
          writeResult = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
          errorCode = writeResult.eax;
          if (writeResult.carry) goto LAB_0040ea0c;
          nameDestination = destination + 0x200;
          for (dwordsRemaining = 0x7b; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
            *(undefined4 *)nameDestination = *(undefined4 *)path;
            path = path + 2;
            nameDestination = nameDestination + 4;
          }
          seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_END,0,(void *)fileHandle);
          errorCode = seekResult.eax;
          if (seekResult.carry) goto LAB_0040ea0c;
          writeResult = (*g_FileSystemWriteExactOrFlushCf)
                            (byteCount,destination + 0x200,(void *)fileHandle);
          errorCode = writeResult.eax;
          if (writeResult.carry) goto LAB_0040ea0c;
        }
        statusResult = Package_ReadDirectory(fileHandle);
        errorCode = statusResult.valueOrError;
        if (!statusResult.carry) {
          return THANDOR_BITCAST(qword, StatusValueEaxCf5, ((THANDOR_BITCAST(StatusValueEaxCf5, qword, statusResult) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
      }
    }
  }
LAB_0040ea0c:
  statusResult.carry = true;
  statusResult.valueOrError = errorCode;
  return statusResult;
}


/* Address: 0x0040ED00.
   Ownership: assets/package/runtime.
   Purpose: Loads a package entry into a caller-provided buffer when allowed by flags and capacity, otherwise falls
   back to the loose-file path. The high two bits of capacityAndFlags control package bypass and alternate loose-
   path handling.
   Local calls: Package_FindEntryAcrossMounts, Package_DecodeEntryInto, Package_SetLastErrorPath.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,byte *destination,word *path)

{
  void *bufferCapacity;
  PckEntryHeader *entry;
  void *handle;
  void *byteCount;
  PackageDecodeEaxCf5 decodeResult;
  StatusValueEaxCf5 decodeStatus;
  FileSystemOpenEaxCf5 openResult;
  FileSystemSizeEaxCf5 sizeResult;
  FileSystemReadEaxCf5 readResult;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  PackageFindEntryEaxEbxCf9 findResult;
  
  bufferCapacity = (void *)(bufferCapacityAndLoadFlags & 0x3fffffff);
  if ((bufferCapacityAndLoadFlags & 0x80000000) == 0) {
    findResult = Package_FindEntryAcrossMounts(path);
    entry = (PckEntryHeader *)findResult.eax;
    if (!findResult.carry) {
      handle = (void *)0x5;
      if ((((void *)entry->unpackedSize <= bufferCapacity) && (entry->packedSize < 0x800001)) &&
         (destination != g_PackageScratchBuffer)) {
        decodeResult = Package_DecodeEntryInto(destination,entry,findResult.ebx);
        decodeStatus.valueOrError = decodeResult.eax;
        decodeStatus.carry = decodeResult.carry;
        return decodeStatus;
      }
      Package_SetLastErrorPath(path);
      goto LAB_0040ee1c;
    }
  }
  if ((bufferCapacityAndLoadFlags & 0x40000000) == 0) {
    openResult = (*g_FileSystemOpenCf)(0,path);
    handle = (void *)openResult.eax;
    if (openResult.carry) goto LAB_0040ee1c;
  }
  else {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (void *)openResult.eax;
    if (openResult.carry) {
      openResult = (*g_FileSystemOpenCf)(0,path);
      handle = (void *)openResult.eax;
      if (openResult.carry) goto LAB_0040ee1c;
    }
  }
  sizeResult = (*g_FileSystemGetSizeCf)(handle);
  byteCount = (void *)sizeResult.eax;
  if (!sizeResult.carry) {
    if ((bufferCapacity < byteCount) && (byteCount = bufferCapacity, (void *)0x7fffff < bufferCapacity)) {
      byteCount = (void *)0x5;
    }
    else {
      readResult = (*g_FileSystemReadExactCf)((FileIoByteCount)byteCount,destination,handle);
      byteCount = (void *)readResult.eax;
      if (!readResult.carry) {
        (*g_FileSystemClose)(handle);
        successResult.carry = false;
        successResult.valueOrError = (dword)byteCount;
        return successResult;
      }
    }
  }
  (*g_FileSystemClose)(handle);
  handle = byteCount;
LAB_0040ee1c:
  failureResult.carry = true;
  failureResult.valueOrError = (dword)handle;
  return failureResult;
}


/* Address: 0x0040E450.
   Ownership: assets/package/runtime.
   Purpose: Mounts a package into the last free slot while scanning the 1,024-slot mount table backward. Because
   lookups scan forward, this archive has the lowest precedence. Startup uses this path for engine.pck.
   Local calls: Package_ReadDirectory.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_MountLowPriority(word *path)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int slotsRemaining;
  PckMountSlot *mountSlot;
  StatusValueEaxCf5 failureResult;
  FileSystemOpenEaxCf5 openResult;
  ArenaAllocEaxCf5 allocResult;
  StatusValueEaxCf5 successResult;
  
  mountSlot = g_PackageMountSlots + 0x3ff;
  slotsRemaining = 0x400;
  do {
    if (mountSlot->fileHandle == 0) {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      openResult = (*g_FileSystemOpenCf)
                        (FILESYSTEM_OPEN_WRITE_ACCESS,(word *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (PckEntryHeader *)openResult.eax;
      if (openResult.carry) {
        openResult = (*g_FileSystemOpenCf)(FILESYSTEM_OPEN_WRITE_ACCESS,path);
        handle = (PckEntryHeader *)openResult.eax;
        if (openResult.carry) goto LAB_0040e480;
      }
      allocResult = (*g_MemoryApi.alloc)(0x80000);
      allocatedEntryHeaders = (PckEntryHeader *)allocResult.eax;
      if (!allocResult.carry) {
        mountSlot->fileHandle = (EngineFileHandle)handle;
        mountSlot->entryHeaders = allocatedEntryHeaders;
        mountSlot->entryCount = 0;
        Package_ReadDirectory((EngineFileHandle)handle);
        successResult.carry = false;
        successResult.valueOrError = (dword)handle;
        return successResult;
      }
      (*g_FileSystemClose)(handle);
      handle = allocatedEntryHeaders;
      goto LAB_0040e480;
    }
    mountSlot = mountSlot + -1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  handle = (PckEntryHeader *)0x14;
LAB_0040e480:
  failureResult.carry = true;
  failureResult.valueOrError = (dword)handle;
  return failureResult;
}


/* Address: 0x0040E6F0.
   Ownership: assets/package/runtime.
   Purpose: Deletes one exact path from a writable mounted PCK. Missing entries are treated as success. The routine
   rewrites the archive header, compacts all following bytes through the shared scratch buffer, truncates the file,
   and reloads the in-memory directory. CF reports file or rewrite failure.
   Local calls: Package_FindEntryInMount, Package_ReadDirectory.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_DeleteEntry(word *path,EngineFileHandle fileHandle)

{
  PckStoredByteCount entryPackedSize;
  int archiveEndOffset;
  byte *destination;
  PckEntryHeader *foundEntry;
  FileSystemFilePosition distance;
  PckEntryHeader *statusOrError;
  uint byteCount;
  PackageEntryEaxCf5 findResult;
  FileSystemSeekEaxCf5 seekResult;
  FileSystemReadEaxCf5 readResult;
  FileSystemWriteEaxCf5 writeResult;
  StatusValueEaxCf5 statusResult;
  StatusValueEaxCf5 successResult;
  
  destination = g_PackageScratchBuffer;
  findResult = Package_FindEntryInMount(path,fileHandle);
  foundEntry = findResult.entry;
  statusOrError = foundEntry;
  if (findResult.carry) {
LAB_0040e81c:
    successResult.carry = false;
    successResult.valueOrError = (dword)statusOrError;
    return successResult;
  }
  seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
  statusOrError = (PckEntryHeader *)seekResult.eax;
  if (!seekResult.carry) {
    entryPackedSize = foundEntry->packedSize;
    readResult = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
    statusOrError = (PckEntryHeader *)readResult.eax;
    if (!readResult.carry) {
      archiveEndOffset = *(int *)(destination + 4);
      *(int *)(destination + 0xb0) = *(int *)(destination + 0xb0) + -1;
      *(PckStoredByteCount *)(destination + 4) = *(int *)(destination + 4) - (entryPackedSize + 0x200);
      seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      statusOrError = (PckEntryHeader *)seekResult.eax;
      if (!seekResult.carry) {
        writeResult = (*g_FileSystemWriteExactOrFlushCf)(0x200,destination,(void *)fileHandle);
        statusOrError = (PckEntryHeader *)writeResult.eax;
        if (!writeResult.carry) {
          distance = foundEntry->runtimePayloadOffset + foundEntry->packedSize + 0x200;
          byteCount = archiveEndOffset - distance;
          if (byteCount == 0) {
            seekResult = (*g_FileSystemSeekCf)
                              (FILESYSTEM_SEEK_BEGIN,foundEntry->runtimePayloadOffset,(void *)fileHandle
                              );
            statusOrError = (PckEntryHeader *)seekResult.eax;
            if (seekResult.carry) goto LAB_0040e828;
            writeResult = (*g_FileSystemWriteExactOrFlushCf)(0,(void *)0x0,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.eax;
            if (writeResult.carry) goto LAB_0040e828;
          }
          else {
            seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,distance,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)seekResult.eax;
            if ((seekResult.carry) || (statusOrError = (PckEntryHeader *)0x14, 0x800000 < byteCount))
            goto LAB_0040e828;
            readResult = (*g_FileSystemReadExactCf)(byteCount,g_PackageScratchBuffer,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)readResult.eax;
            if (readResult.carry) goto LAB_0040e828;
            seekResult = (*g_FileSystemSeekCf)
                              (FILESYSTEM_SEEK_BEGIN,foundEntry->runtimePayloadOffset,(void *)fileHandle
                              );
            statusOrError = (PckEntryHeader *)seekResult.eax;
            if (seekResult.carry) goto LAB_0040e828;
            writeResult = (*g_FileSystemWriteExactOrFlushCf)
                              (byteCount,g_PackageScratchBuffer,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.eax;
            if (writeResult.carry) goto LAB_0040e828;
            writeResult = (*g_FileSystemWriteExactOrFlushCf)(0,(void *)0x0,(void *)fileHandle);
            statusOrError = (PckEntryHeader *)writeResult.eax;
            if (writeResult.carry) goto LAB_0040e828;
          }
          statusResult = Package_ReadDirectory(fileHandle);
          statusOrError = (PckEntryHeader *)statusResult.valueOrError;
          if (!statusResult.carry) goto LAB_0040e81c;
        }
      }
    }
  }
LAB_0040e828:
  statusResult.carry = true;
  statusResult.valueOrError = (dword)statusOrError;
  return statusResult;
}


/* Address: 0x0040EE30.
   Ownership: assets/package/runtime.
   Purpose: Loads any package or loose-file asset into a newly allocated buffer. The returned allocation is untyped
   at this layer; callers cast it to gfx, fld, lev, mdl, sound, text, and other asset types. CF reports success or
   failure.
   Local calls: Package_FindEntryAcrossMounts, Package_DecodeEntryInto.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
PackageLoadEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_LoadEntry(word *path)

{
  PckEntryHeader *entry;
  byte *destination;
  byte *byteCountOrError;
  ArenaAllocEaxCf5 allocResult;
  PackageDecodeEaxCf5 decodeResult;
  FileSystemOpenEaxCf5 openResult;
  FileSystemSizeEaxCf5 sizeResult;
  FileSystemReadEaxCf5 readResult;
  PackageLoadEntryEaxCf5 failureResult;
  PackageFindEntryEaxEbxCf9 findResult;
  
  findResult = Package_FindEntryAcrossMounts(path);
  entry = (PckEntryHeader *)findResult.eax;
  if (findResult.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    destination = (byte *)openResult.eax;
    if (openResult.carry) {
      openResult = (*g_FileSystemOpenCf)(0,path);
      destination = (byte *)openResult.eax;
      if (openResult.carry) goto LAB_0040ef3f;
    }
    sizeResult = (*g_FileSystemGetSizeCf)(destination);
    byteCountOrError = (byte *)sizeResult.eax;
    if (!sizeResult.carry) {
      allocResult = (*g_MemoryApi.alloc)((dword)byteCountOrError);
      if (allocResult.carry) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)byteCountOrError,g_FatalErrorDetail1Utf16);
        byteCountOrError = (byte *)0x5;
      }
      else {
        readResult = (*g_FileSystemReadExactCf)((FileIoByteCount)byteCountOrError,(void *)allocResult.eax,destination);
        byteCountOrError = (byte *)readResult.eax;
        if (!readResult.carry) {
          (*g_FileSystemClose)(destination);
          return THANDOR_BITCAST(qword, PackageLoadEntryEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
        (*g_MemoryApi.free)((void *)allocResult.eax);
      }
    }
    (*g_FileSystemClose)(destination);
    destination = byteCountOrError;
  }
  else {
    destination = (byte *)0x5;
    if (entry->packedSize < 0x800001) {
      allocResult = (*g_MemoryApi.alloc)(entry->unpackedSize);
      destination = (byte *)allocResult.eax;
      if (!allocResult.carry) {
        decodeResult = Package_DecodeEntryInto(destination,entry,findResult.ebx);
        if (!decodeResult.carry) {
          return THANDOR_BITCAST(qword, PackageLoadEntryEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
        byteCountOrError = (byte *)decodeResult.eax;
        (*g_MemoryApi.free)(destination);
        destination = byteCountOrError;
      }
    }
  }
LAB_0040ef3f:
  {
    /* open-thandor diagnostics: first failed loads with their caller stack */
    static int loggedFailures;
    if (loggedFailures++ < 8) {
      Thandor_Log("Package_LoadEntry failed: \"%ls\" (error 0x%08X)", (wchar_t *)path, (dword)destination);
      Thandor_LogStack("  load failure stack", (dword)destination);
    }
  }
  failureResult.carry = true;
  failureResult.bufferOrError = destination;
  return failureResult;
}


/* Address: 0x0040E3A0.
   Ownership: assets/package/runtime.
   Purpose: Mounts a package in the first free slot. Lookup scans the table in the same direction, so earlier
   normal mounts have higher precedence.
   Local calls: Package_ReadDirectory.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Package_Mount(word *path)

{
  PckEntryHeader *handle;
  PckEntryHeader *allocatedEntryHeaders;
  int slotsRemaining;
  PckMountSlot *mountSlot;
  StatusValueEaxCf5 failureResult;
  FileSystemOpenEaxCf5 openResult;
  ArenaAllocEaxCf5 allocResult;
  StatusValueEaxCf5 successResult;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = 0x400;
  do {
    if (mountSlot->fileHandle == 0) {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
                 (word *)&g_ExecutableDirectoryUtf16);
      openResult = (*g_FileSystemOpenCf)
                        (FILESYSTEM_OPEN_WRITE_ACCESS,(word *)&g_FileSystemCombinedPathScratchUtf16);
      handle = (PckEntryHeader *)openResult.eax;
      if (openResult.carry) {
        openResult = (*g_FileSystemOpenCf)(FILESYSTEM_OPEN_WRITE_ACCESS,path);
        handle = (PckEntryHeader *)openResult.eax;
        if (openResult.carry) goto LAB_0040e3d0;
      }
      allocResult = (*g_MemoryApi.alloc)(0x80000);
      allocatedEntryHeaders = (PckEntryHeader *)allocResult.eax;
      if (!allocResult.carry) {
        mountSlot->fileHandle = (EngineFileHandle)handle;
        mountSlot->entryHeaders = allocatedEntryHeaders;
        mountSlot->entryCount = 0;
        Package_ReadDirectory((EngineFileHandle)handle);
        successResult.carry = false;
        successResult.valueOrError = (dword)handle;
        return successResult;
      }
      (*g_FileSystemClose)(handle);
      handle = allocatedEntryHeaders;
      goto LAB_0040e3d0;
    }
    mountSlot = mountSlot + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  handle = (PckEntryHeader *)0x14;
LAB_0040e3d0:
  failureResult.carry = true;
  failureResult.valueOrError = (dword)handle;
  return failureResult;
}


/* Address: 0x0040EB70.
   Ownership: assets/package/runtime.
   Purpose: Collects entry paths in one mounted package that match the package wildcard grammar, copies them into
   0x200-byte output slots, and sorts the results lexicographically. Returns 0x200 with CF clear or error 0x14 with
   CF set.
   Local calls: Package_WildcardPathMatches.
*/
PackageFindEntryEaxEcxCf9 __thandor_eax_cf_preserve_edx
Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                 word *pattern,EngineFileHandle fileHandle)

{
  EngineFileHandle handleOrRemaining;
  word *firstPathCursor;
  word *secondPathCursor;
  undefined4 in_ECX;
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
  PackageFindEntryEaxEcxCf9 failureResult;
  PackageFindEntryEaxEcxCf9 successResult;
  dword swappedDword;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = 0x400;
  handleOrRemaining = fileHandle;
  while( true ) {
    if (handleOrRemaining == 0) {
      failureResult.matchCount = in_ECX;
      failureResult.recordSizeOrError = 0x14;
      failureResult.carry = true;
      return failureResult;
    }
    if (fileHandle == mountSlot->fileHandle) break;
    mountSlot = mountSlot + 1;
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
        carryFlag = outputCapacityBytes < 0x200;
        outputCapacityBytes = outputCapacityBytes - 0x200;
        if (carryFlag) goto LAB_0040ec93;
        sourceCursor = entryCursor;
        copyDestination = targetCursor;
        for (remainingCount = 0xf6; remainingCount != 0; remainingCount = remainingCount + -1) {
          copyDestination->path[0] = sourceCursor->path[0];
          sourceCursor = (PckEntryHeader *)(sourceCursor->path + 1);
          copyDestination = (PckEntryHeader *)(copyDestination->path + 1);
        }
        targetCursor = targetCursor + 1;
        matchedCount = matchedCount + 1;
      }
      entryCursor = entryCursor + 1;
      entriesRemaining = entriesRemaining - 1;
    } while (entriesRemaining != 0);
    if (matchedCount != 0) {
      remainingCount = matchedCount + -1;
      carryFlag = false;
      if (remainingCount != 0) {
        entryCursor = outputEntries + 1;
        unsortedCount = matchedCount;
        do {
          do {
            unitsRemaining = 0x100;
            targetCursor = entryCursor;
            sourceCursor = outputEntries;
            do {
              if (unitsRemaining == 0) break;
              unitsRemaining = unitsRemaining + -1;
              firstPathCursor = sourceCursor->path;
              secondPathCursor = targetCursor->path;
              carryFlag = *secondPathCursor < *firstPathCursor;
              targetCursor = (PckEntryHeader *)(targetCursor->path + 1);
              sourceCursor = (PckEntryHeader *)(sourceCursor->path + 1);
            } while (*secondPathCursor == *firstPathCursor);
            if (carryFlag) {
              unitsRemaining = 0x80;
              do {
                sourceCursor = outputEntries;
                targetCursor = entryCursor;
                LOCK();
                swappedDword = *(dword *)sourceCursor->path;
                *(undefined4 *)sourceCursor->path = *(undefined4 *)targetCursor->path;
                UNLOCK();
                *(dword *)targetCursor->path = swappedDword;
                unitsRemaining = unitsRemaining + -1;
                entryCursor = (PckEntryHeader *)(targetCursor->path + 2);
                outputEntries = (PckEntryHeader *)(sourceCursor->path + 2);
              } while (unitsRemaining != 0);
              entryCursor = (PckEntryHeader *)(targetCursor[-1].path + 2);
              outputEntries = (PckEntryHeader *)(sourceCursor[-1].path + 2);
            }
            carryFlag = (PckEntryHeader *)0xfffffdff < entryCursor;
            entryCursor = entryCursor + 1;
            remainingCount = remainingCount + -1;
          } while (remainingCount != 0);
          remainingCount = unsortedCount + -2;
          unsortedCount = unsortedCount + -1;
          entryCursor = outputEntries + 2;
          carryFlag = false;
          outputEntries = outputEntries + 1;
        } while (remainingCount != 0);
      }
    }
  }
LAB_0040ec93:
  successResult.matchCount = matchedCount;
  successResult.recordSizeOrError = 0x200;
  successResult.carry = false;
  return successResult;
}


/* Address: 0x0040E500.
   Ownership: assets/package/runtime.
   Purpose: Finds the mount slot by file handle, frees its entry-header array, closes the file, and clears all
   three slot fields.
*/
void __thandor_preserve_eax_edx Package_Unmount(EngineFileHandle fileHandle)

{
  EngineFileHandle handleOrRemaining;
  EngineFileHandle mountSlotsRemaining;
  PckMountSlot *mountSlotCursor;
  
  mountSlotCursor = g_PackageMountSlots;
  mountSlotsRemaining = 0x400;
  handleOrRemaining = fileHandle;
  while( true ) {
    if (handleOrRemaining == 0) {
      return;
    }
    if (fileHandle == mountSlotCursor->fileHandle) break;
    mountSlotCursor = mountSlotCursor + 1;
    mountSlotsRemaining = mountSlotsRemaining - 1;
    handleOrRemaining = mountSlotsRemaining;
  }
  (*g_MemoryApi.free)(mountSlotCursor->entryHeaders);
  (*g_FileSystemClose)((void *)fileHandle);
  mountSlotCursor->fileHandle = 0;
  mountSlotCursor->entryHeaders = (PckEntryHeader *)0x0;
  mountSlotCursor->entryCount = 0;
  return;
}


/* Address: 0x0040ECA0.
   Ownership: assets/package/runtime.
   Purpose: Compares a UTF-16 archive path against a pattern. '?' matches one word. '*' advances the candidate to
   the next dot or terminator rather than implementing unrestricted globbing. CF clear means match.
*/
bool __thandor_cf_preserve_eax_ecx_edx Package_WildcardPathMatches(word *pattern,word *candidate)

{
  word patternCodeUnit;
  
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
   Ownership: assets/package/runtime.
   Purpose: Seeks to runtimePayloadOffset + 0x200, reads packedSize bytes into g_PackageScratchBuffer, and
   dispatches compressionMethod through g_PckDecoderTable into destination. CF reports failure.
   Local calls: Package_SetLastErrorPath.
*/
PackageDecodeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_DecodeEntryInto(byte *destination,PckEntryHeader *entry,EngineFileHandle fileHandle)

{
  PckCompressionMethod entryCompression;
  dword decoderStatusCode;
  FileSystemSeekEaxCf5 seekResult;
  FileSystemReadEaxCf5 readResult;
  PckCodecEaxCf5 decodeResult;
  PackageDecodeEaxCf5 successResult;
  PackageDecodeEaxCf5 failureResult;
  
  seekResult = (*g_FileSystemSeekCf)
                    (FILESYSTEM_SEEK_BEGIN,entry->runtimePayloadOffset + 0x200,(void *)fileHandle);
  decoderStatusCode = seekResult.eax;
  if (!seekResult.carry) {
    entryCompression = entry->compressionMethod;
    readResult = (*g_FileSystemReadExactCf)(entry->packedSize,g_PackageScratchBuffer,(void *)fileHandle);
    decoderStatusCode = readResult.eax;
    if (!readResult.carry) {
      decodeResult = (*g_PckDecoderTable[entryCompression])
                        (entry->unpackedSize,destination,entry->packedSize,g_PackageScratchBuffer);
      decoderStatusCode = decodeResult.eax;
      if (!decodeResult.carry) {
        successResult.eax = decodeResult.eax;
        successResult.carry = decodeResult.carry;
        return successResult;
      }
    }
  }
  Package_SetLastErrorPath(entry->path);
  failureResult.carry = true;
  failureResult.eax = decoderStatusCode;
  return failureResult;
}


/* Address: 0x0040E2B0.
   Ownership: assets/package/runtime.
   Purpose: Copies up to 256 UTF-16 words from path into g_PackageLastErrorPath. The buffer is used by package and
   loose-file load failures.
*/
void __thandor_void_preserve_eax_ecx_edx Package_SetLastErrorPath(word *path)

{
  word codeUnit;
  int remainingCount;
  word *scanEnd;
  word *wordCursor;
  
  remainingCount = 0x100;
  wordCursor = path;
  do {
    scanEnd = wordCursor;
    if (remainingCount == 0) break;
    remainingCount = remainingCount + -1;
    scanEnd = wordCursor + 1;
    codeUnit = *wordCursor;
    wordCursor = scanEnd;
  } while (codeUnit != 0);
  remainingCount = (int)scanEnd - (int)path;
  wordCursor = g_PackageLastErrorPath;
  for (; remainingCount != 0; remainingCount = remainingCount + -1) {
    *wordCursor = *path;
    path = path + 1;
    wordCursor = wordCursor + 1;
  }
  return;
}


/* Address: 0x0040E640.
   Ownership: assets/package/runtime.
   Purpose: Finds one exact lowercase UTF-16 path in the selected mounted archive. CF clear returns a
   PckEntryHeader pointer; CF set indicates failure.
*/
PackageEntryEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_FindEntryInMount(word *path,EngineFileHandle fileHandle)

{
  int lengthRemaining;
  int remainingCount;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  word *pathCursor;
  PckEntryHeader *entryCursor;
  bool matched;
  PackageEntryEaxCf5 notFoundResult;
  PackageEntryEaxCf5 foundResult;
  PackageEntryEaxCf5 emptyMountResult;
  PackageEntryEaxCf5 noMountResult;
  PackageEntryEaxCf5 pathTooLongResult;
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
    pathTooLongResult.carry = true;
    return pathTooLongResult;
  }
  mountSlot = g_PackageMountSlots;
  remainingCount = 0x400;
  while (fileHandle != mountSlot->fileHandle) {
    mountSlot = mountSlot + 1;
    remainingCount = remainingCount + -1;
    if (remainingCount == 0) {
      noMountResult.entry = (PckEntryHeader *)0x0;
      noMountResult.carry = true;
      return noMountResult;
    }
  }
  entryCursor = mountSlot->entryHeaders;
  entriesRemaining = mountSlot->entryCount;
  matched = false;
  remainingCount = -(lengthRemaining + -0xf6);
  pathCursor = path;
  currentEntry = entryCursor;
  if (entriesRemaining == 0) {
    emptyMountResult.entry = (PckEntryHeader *)0x0;
    emptyMountResult.carry = true;
    return emptyMountResult;
  }
code_r0x0040e6b4:
  do {
    if (remainingCount != 0) {
      matched = *pathCursor == entryCursor->path[0];
      remainingCount = remainingCount + -1;
      pathCursor = pathCursor + 1;
      entryCursor = (PckEntryHeader *)(entryCursor->path + 1);
      if (matched) goto code_r0x0040e6b4;
    }
    if (matched) {
      foundResult.carry = false;
      foundResult.entry = currentEntry;
      return foundResult;
    }
    entryCursor = currentEntry + 1;
    entriesRemaining = entriesRemaining - 1;
    matched = false;
    remainingCount = -(lengthRemaining + -0xf6);
    pathCursor = path;
    currentEntry = entryCursor;
    if (entriesRemaining == 0) {
      notFoundResult.entry = (PckEntryHeader *)0x0;
      notFoundResult.carry = true;
      return notFoundResult;
    }
  } while( true );
}


/* Address: 0x0040EA20.
   Ownership: assets/package/runtime.
   Purpose: Lowercases the caller's path in place and searches mounted archives from slot 0 upward, establishing
   first-mounted-wins precedence. CF clear returns the matching entry header.
*/
PackageFindEntryEaxEbxCf9 __thandor_eax_ebx_cf_preserve_ecx_edx
Package_FindEntryAcrossMounts(word *path)

{
  uint codeUnit;
  int remainingOrLength;
  int compareRemaining;
  int slotsRemaining;
  PckEntryCount in_EBX;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  word *pathCursor;
  PckEntryHeader *entryCursor;
  bool matched;
  PackageFindEntryEaxEbxCf9 foundResult;
  PackageFindEntryEaxEbxCf9 notFoundResult;
  PckEntryHeader *currentEntry;
  
  codeUnit = 0;
  remainingOrLength = 0xf6;
  pathCursor = path;
  do {
    compareRemaining = remainingOrLength;
    codeUnit = CONCAT22((short)(codeUnit >> 0x10),*pathCursor);
    if ((0x40 < codeUnit) && (codeUnit < 0x5b)) {
      codeUnit = codeUnit + 0x20;
    }
    *pathCursor = (word)codeUnit;
    remainingOrLength = compareRemaining + -1;
    if (remainingOrLength == 0) goto LAB_0040ead6;
    pathCursor = pathCursor + 1;
  } while (codeUnit != 0);
  mountSlot = g_PackageMountSlots;
  slotsRemaining = 0x400;
  remainingOrLength = -(compareRemaining + -0xf7);
  do {
    entryCursor = mountSlot->entryHeaders;
    in_EBX = mountSlot->entryCount;
    if ((entryCursor != (PckEntryHeader *)0x0) && (in_EBX != 0)) {
      matched = entryCursor == (PckEntryHeader *)0x0;
      compareRemaining = remainingOrLength;
      pathCursor = path;
      currentEntry = entryCursor;
      entriesRemaining = in_EBX;
code_r0x0040eaa7:
      do {
        if (compareRemaining != 0) {
          matched = *pathCursor == entryCursor->path[0];
          compareRemaining = compareRemaining + -1;
          pathCursor = pathCursor + 1;
          entryCursor = (PckEntryHeader *)(entryCursor->path + 1);
          if (matched) goto code_r0x0040eaa7;
        }
        if (matched) {
          foundResult.ebx = mountSlot->fileHandle;
          foundResult.eax = (dword)currentEntry;
          foundResult.carry = false;
          return foundResult;
        }
        entryCursor = currentEntry + 1;
        entriesRemaining = entriesRemaining - 1;
        matched = entriesRemaining == 0;
        in_EBX = 0;
        compareRemaining = remainingOrLength;
        pathCursor = path;
        currentEntry = entryCursor;
      } while (!matched);
    }
    mountSlot = mountSlot + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
LAB_0040ead6:
  notFoundResult.ebx = in_EBX;
  notFoundResult.eax = codeUnit;
  notFoundResult.carry = true;
  return notFoundResult;
}


/* Address: 0x0040E570.
   Ownership: assets/package/runtime.
   Purpose: Reads the 0x200-byte archive header, caches entryCount, reads every 0x200-byte entry header, and fills
   runtimePayloadOffset while skipping each packed payload.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Package_ReadDirectory(EngineFileHandle fileHandle)

{
  PckStoredByteCount *packedSizeField;
  byte *archiveHeader;
  uint statusCode;
  PckEntryCount entriesRemaining;
  int slotsRemaining;
  FileSystemFilePosition distance;
  PckMountSlot *mountSlot;
  PckEntryHeader *destination;
  StatusValueEaxCf5 failureResult;
  FileSystemSeekEaxCf5 seekResult;
  FileSystemReadEaxCf5 readResult;
  StatusValueEaxCf5 successResult;
  
  mountSlot = g_PackageMountSlots;
  slotsRemaining = 0x400;
  do {
    if (fileHandle == mountSlot->fileHandle) {
      destination = mountSlot->entryHeaders;
      seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,(void *)fileHandle);
      archiveHeader = g_PackageScratchBuffer;
      statusCode = seekResult.eax;
      if (seekResult.carry) goto LAB_0040e59f;
      readResult = (*g_FileSystemReadExactCf)(0x200,g_PackageScratchBuffer,(void *)fileHandle);
      statusCode = readResult.eax;
      if (readResult.carry) goto LAB_0040e59f;
      entriesRemaining = *(PckEntryCount *)(archiveHeader + 0xb0);
      mountSlot->entryCount = entriesRemaining;
      distance = 0x200;
      if (entriesRemaining != 0) goto LAB_0040e5f0;
      goto LAB_0040e62b;
    }
    mountSlot = mountSlot + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  statusCode = 0x14;
LAB_0040e59f:
  failureResult.carry = true;
  failureResult.valueOrError = statusCode;
  return failureResult;
LAB_0040e5f0:
  readResult = (*g_FileSystemReadExactCf)(0x200,destination,(void *)fileHandle);
  statusCode = readResult.eax;
  if (readResult.carry) goto LAB_0040e59f;
  packedSizeField = &destination->packedSize;
  destination->runtimePayloadOffset = distance;
  destination = destination + 1;
  distance = distance + *packedSizeField + 0x200;
  seekResult = (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,distance,(void *)fileHandle);
  statusCode = seekResult.eax;
  if (seekResult.carry) goto LAB_0040e59f;
  entriesRemaining = entriesRemaining - 1;
  if (entriesRemaining == 0) {
LAB_0040e62b:
    successResult.carry = false;
    successResult.valueOrError = statusCode;
    return successResult;
  }
  goto LAB_0040e5f0;
}

