/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/archive_write.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/archive_write.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>

/* Writes path into the writable mounted package fileHandle, replacing an existing entry of that name: the
   archive header in g_PackageScratchBuffer gets one more entry and the new size, then the entry header and
   its payload are appended at the end of the file. compressionMethod indexes g_PckEncoderTable, except
   PCK_COMPRESSION_STORED, which appends the source dword-aligned as it is; the first source dword becomes
   the entry's typeTag. The in-memory directory is reloaded afterwards. Returns true on success, false when
   deleting the old entry, a seek/read/write, the encoder or the directory reload fails (the error code is
   dropped: no caller uses it).
   Called directly by the save-game writers in gameplay/session/savegame.cpp and campaign_carryover.cpp (no
   callback table); all of them use PCK_COMPRESSION_HUFFMAN_RLE, so no caller reaches the stored branch.
   The original wrote a stored payload as unpackedSize rounded up to whole dwords straight from sourceData, reading
   up to 3 bytes behind the source; bounded here because the source need not extend that far: the payload is
   written as its unpackedSize bytes plus zero padding to the dword boundary, and a size that would wrap when
   rounded up is rejected (one log line). The entry header and the file size are unchanged.
*/
bool Package_UpsertEntry(PckCompressionMethod compressionMethod,PckDecodedByteCount unpackedSize,
                   uint32_t *sourceData,uint16_t *path,EngineFileHandle fileHandle)

{
  uint8_t *destination; /* archive header, then the new entry header, then (encoded) its payload */
  PckArchiveHeader *archiveHeader; /* the archive header read to offset 0 of destination */
  PckEntryHeader *newEntry; /* the entry header behind the archive header */
  uint32_t packedByteCount;
  FileIoByteCount byteCount;
  uint32_t alignedByteCount;

  if (compressionMethod == PCK_COMPRESSION_STORED && unpackedSize > UINT32_MAX - 3) {
    Thandor_Log("Package_UpsertEntry: rejected stored entry of %u bytes",unpackedSize);
    return false;
  }
  destination = g_PackageScratchBuffer;
  archiveHeader = reinterpret_cast<PckArchiveHeader *>(destination);
  newEntry = Asset_RecordAt<PckEntryHeader>(destination,PCK_ENTRY_HEADER_BYTES);
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
  archiveHeader->entryCount++;
  if (compressionMethod == PCK_COMPRESSION_STORED) {
    std::array<uint8_t,3> storedPadding = {}; /* zero padding behind the payload */

    alignedByteCount = unpackedSize + 3 & PACKAGE_DWORD_ALIGN_MASK;
    newEntry->packedSize = alignedByteCount;
    newEntry->compressionMethod = PCK_COMPRESSION_STORED;
    archiveHeader->archiveSize = archiveHeader->archiveSize + alignedByteCount + PCK_ENTRY_HEADER_BYTES;
    newEntry->runtimePayloadOffset = 0;
    newEntry->typeTag = static_cast<PckAssetTypeTag>(*sourceData);
    newEntry->unpackedSize = unpackedSize;
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
    if (g_FileSystemWriteExactOrFlush(unpackedSize,sourceData,THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
    if (alignedByteCount != unpackedSize &&
        g_FileSystemWriteExactOrFlush(alignedByteCount - unpackedSize,storedPadding.data(),THANDOR_PTR(fileHandle)) != 0) {
      return false;
    }
  }
  else {
    /* encode straight behind the new entry header, so both are written in one go */
    if (!g_PckEncoderTable[compressionMethod]
            (PACKAGE_SCRATCH_BUFFER_BYTES - 2 * PCK_ENTRY_HEADER_BYTES,
             destination + 2 * PCK_ENTRY_HEADER_BYTES,unpackedSize,reinterpret_cast<uint8_t *>(sourceData),&packedByteCount,
             nullptr)) { /* the encoder reads the payload as bytes */
      return false;
    }
    newEntry->packedSize = packedByteCount;
    byteCount = packedByteCount + PCK_ENTRY_HEADER_BYTES;
    newEntry->compressionMethod = compressionMethod;
    archiveHeader->archiveSize = archiveHeader->archiveSize + byteCount;
    newEntry->runtimePayloadOffset = 0;
    newEntry->typeTag = static_cast<PckAssetTypeTag>(*sourceData);
    newEntry->unpackedSize = unpackedSize;
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
  PckArchiveHeader *archiveHeader = reinterpret_cast<PckArchiveHeader *>(destination); /* read to offset 0 */

  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  statusCode = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,destination,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  *outArchiveEndOffset = static_cast<int>(archiveHeader->archiveSize);
  archiveHeader->entryCount--;
  archiveHeader->archiveSize = archiveHeader->archiveSize - (entryPackedSize + PCK_ENTRY_HEADER_BYTES);
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
bool Package_DeleteEntry(uint16_t *path,EngineFileHandle fileHandle,uint32_t *outErrorCode)

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

/* open-thandor: transactional package writes. The original built a save package directly in the target
   file (created empty, then filled entry by entry through Package_UpsertEntry / Package_DeleteEntry), so a
   failed write (disk full, a crash) left a truncated or inconsistent package in place of the old one. A
   writer instead creates and mounts the package at the temporary path from Package_MakeTemporaryPath (the
   target path plus ".tmp", so the same directory and volume), writes it exactly as before and then calls
   Package_CommitTemporary, which unmounts it and replaces the target in one step; on any failure
   Package_DiscardTemporary unmounts and deletes it and the old target stays intact. The package bytes are
   unchanged: no entry or header field depends on the file name the package is built under. */

/* ".tmp", appended to the target path */
static constexpr std::array<uint16_t,5> PACKAGE_TEMPORARY_SUFFIX = {'.','t','m','p',0};

/* Writes targetPath plus ".tmp" to temporaryPath (THANDOR_PATH_CAPACITY code units). Returns false (one log
   line, temporaryPath unspecified) when the result does not fit. */
bool Package_MakeTemporaryPath(uint16_t *temporaryPath,const uint16_t *targetPath)

{
  size_t pathLength = 0;

  while (pathLength < THANDOR_PATH_CAPACITY && targetPath[pathLength] != 0) {
    temporaryPath[pathLength] = targetPath[pathLength];
    pathLength++;
  }
  if (pathLength + PACKAGE_TEMPORARY_SUFFIX.size() > THANDOR_PATH_CAPACITY) {
    Thandor_Log("Package_MakeTemporaryPath: path too long for the temporary package");
    return false;
  }
  std::copy(PACKAGE_TEMPORARY_SUFFIX.begin(),PACKAGE_TEMPORARY_SUFFIX.end(),temporaryPath + pathLength);
  return true;
}

/* Commits a package written at temporaryPath and mounted as fileHandle: unmounts it and replaces targetPath
   with it (Win32File_Replace). Returns true on success; on failure the temporary file is deleted, targetPath
   is unchanged and one line is logged. */
bool Package_CommitTemporary(EngineFileHandle fileHandle,uint16_t *temporaryPath,uint16_t *targetPath)

{
  uint32_t replaceError;

  Package_Unmount(fileHandle);
  replaceError = Win32File_Replace(targetPath,temporaryPath);
  if (replaceError == 0) {
    return true;
  }
  Thandor_Log("Package_CommitTemporary: replacing \"%ls\" failed (error 0x%08X), the old file is kept",
              reinterpret_cast<wchar_t *>(targetPath),replaceError); /* UTF-16 for %ls */
  g_FileSystemDelete(1,temporaryPath); /* the first argument is unused by Win32File_Delete */
  return false;
}

/* Abandons a failed package write at temporaryPath: unmounts fileHandle (0, or a handle that is no longer
   mounted, is skipped) and deletes the temporary file (a missing one is fine). The target is not touched. */
void Package_DiscardTemporary(EngineFileHandle fileHandle,uint16_t *temporaryPath)

{
  Package_Unmount(fileHandle);
  g_FileSystemDelete(1,temporaryPath); /* the first argument is unused by Win32File_Delete */
}
