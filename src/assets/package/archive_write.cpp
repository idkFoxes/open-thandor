/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/archive_write.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/archive_write.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: assets/package/archive_write. */

/* Writes path into the writable mounted package fileHandle, replacing an existing entry of that name: the
   archive header in g_PackageScratchBuffer gets one more entry and the new size, then the entry header and
   its payload are appended at the end of the file. compressionMethod indexes g_PckEncoderTable, except
   PCK_COMPRESSION_STORED, which appends the source dword-aligned as it is; the first source dword becomes
   the entry's typeTag. The in-memory directory is reloaded afterwards. Returns true on success, false when
   deleting the old entry, a seek/read/write, the encoder or the directory reload fails (the error code is
   dropped: no caller uses it).
   Called directly by the save-game writer in gameplay/session/savegame.cpp (no callback table).
*/
Bool8 Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle)

{
  uint8_t *destination; /* archive header, then the new entry header, then (encoded) its payload */
  uint32_t packedByteCount;
  FileIoByteCount byteCount;
  uint32_t alignedByteCount;

  destination = g_PackageScratchBuffer;
  if (Package_FindEntryInMount(path,fileHandle) != nullptr) {
    if (!Package_DeleteEntry(path,fileHandle,nullptr)) {
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
             destination + 2 * PCK_ENTRY_HEADER_BYTES,unpackedSize,(uint8_t *)sourceData,&packedByteCount,nullptr)) {
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
  return Package_ReadDirectory(fileHandle,nullptr);
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
    return g_FileSystemWriteExactOrFlush(0,nullptr,THANDOR_PTR(fileHandle));
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
  return g_FileSystemWriteExactOrFlush(0,nullptr,THANDOR_PTR(fileHandle));
}

/* Deletes the entry named path from the writable mounted package fileHandle (a missing entry counts as
   deleted): the archive header loses one entry and its size, everything behind the entry is moved down over
   it through g_PackageScratchBuffer, the file is truncated there and the in-memory directory is reloaded.
   Returns true on success; on failure returns false with the file-system error code (or
   FATAL_ERROR_GENERAL_FAILURE when the tail does not fit the scratch buffer) in *outErrorCode, which may be
   NULL. Called directly by Package_UpsertEntry and the save-game writer in gameplay/session/savegame.cpp (no callback
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
  if (foundEntry == nullptr) {
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
  if (outErrorCode != nullptr) {
    *outErrorCode = statusCode;
  }
  return false;
}
