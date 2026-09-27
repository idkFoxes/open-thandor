/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/filesystem/win32.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/filesystem/win32.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: platform/filesystem/win32. */

/* Address: 0x0040F430.
   Ownership: platform/filesystem/win32.
   Purpose: Builds the file-enumeration string table with the recovered carry/error result.
*/
EnumerationStringTableResult __thandor_eax_ecx_cf_preserve_edx
FileSystem_BuildEnumerationStringTableCf
          (FileSystemEnumerationMode enumerationMode,uint32_t reserved,uint8_t *pathOrVolumeText)

{
  short codeUnit;
  uint8_t *outputRecords;
  uint8_t *recordStride;
  uint8_t *memory;
  FileSystemOutputCapacityBytes foundEntryCount;
  FileSystemOutputCapacityBytes outputCapacityBytes;
  FileSystemOutputCapacityBytes remainingEntries;
  uint8_t *pointerSlot;
  uint8_t *sourceChar;
  uint8_t *sourceRecord;
  uint8_t *stringCursor;
  bool capacityCheck;
  ArenaShrinkResult shrinkResult;
  ArenaFreeResult freeResult;
  ArenaLargestAllocResult largestBlock;
  DirectoryEnumerationResult enumerationResult;
  EnumerationStringTableResult successResult;
  EnumerationStringTableResult failureResult;
  
  largestBlock = g_MemoryApi.allocLargestFreeBlock();
  outputCapacityBytes = largestBlock.blockSizeOrSentinel;
  outputRecords = (uint8_t *)largestBlock.allocationOrError;
  if (!largestBlock.failed) {
    enumerationResult = g_FileSystemEnumerateDirectoryOrVolumeEntriesCf
                       (enumerationMode,reserved,outputCapacityBytes,outputRecords,pathOrVolumeText)
    ;
    foundEntryCount = enumerationResult.entryCount;
    recordStride = (uint8_t *)enumerationResult.recordSizeBytes;
    memory = recordStride;
    outputCapacityBytes = foundEntryCount;
    if (!enumerationResult.failed) {
      if (foundEntryCount == 0) {
        g_MemoryApi.free(outputRecords);
        successResult.tableOrError = 0;
        successResult.entryCountOrScratch = 0;
        successResult.failed = false;
        return successResult;
      }
      outputCapacityBytes = foundEntryCount * (int)recordStride;
      shrinkResult = g_MemoryApi.shrinkInPlace(outputCapacityBytes,outputRecords);
      memory = (uint8_t *)shrinkResult.scratchOrError;
      if (!shrinkResult.failed) {
        largestBlock = g_MemoryApi.allocLargestFreeBlock();
        outputCapacityBytes = largestBlock.blockSizeOrSentinel;
        memory = (uint8_t *)largestBlock.allocationOrError;
        if (!largestBlock.failed) {
          stringCursor = memory + foundEntryCount * 4;
          capacityCheck = foundEntryCount * 4 <= outputCapacityBytes;
          outputCapacityBytes = outputCapacityBytes + foundEntryCount * -4;
          remainingEntries = foundEntryCount;
          pointerSlot = memory;
          sourceRecord = outputRecords;
          if (capacityCheck && outputCapacityBytes != 0) {
            do {
              *(uint8_t **)pointerSlot = stringCursor;
              sourceChar = sourceRecord;
              do {
                codeUnit = *(short *)sourceChar;
                *(short *)stringCursor = codeUnit;
                sourceChar = sourceChar + 2;
                stringCursor = stringCursor + 2;
                capacityCheck = outputCapacityBytes < 2;
                outputCapacityBytes = outputCapacityBytes - 2;
                if (capacityCheck || outputCapacityBytes == 0) goto FileSystem_BuildEnumerationStringTable_FreeOnOverflow;
              } while (codeUnit != 0);
              pointerSlot = pointerSlot + 4;
              sourceRecord = sourceRecord + (int)recordStride;
              remainingEntries = remainingEntries - 1;
              if (remainingEntries == 0) {
                g_MemoryApi.shrinkInPlace((int)stringCursor - (int)memory,memory);
                g_MemoryApi.free(outputRecords);
                successResult.entryCountOrScratch = foundEntryCount;
                successResult.tableOrError = (uint32_t)memory;
                successResult.failed = false;
                return successResult;
              }
            } while( true );
          }
FileSystem_BuildEnumerationStringTable_FreeOnOverflow:
          freeResult = g_MemoryApi.free(memory);
          memory = (uint8_t *)freeResult.valueOrError;
        }
      }
    }
    g_MemoryApi.free(outputRecords);
    outputRecords = memory;
  }
  failureResult.entryCountOrScratch = outputCapacityBytes;
  failureResult.tableOrError = (uint32_t)outputRecords;
  failureResult.failed = true;
  return failureResult;
}

/* Address: 0x00575CB0.
   Ownership: platform/filesystem/win32.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Local calls: Win32File_OpenCf, Win32File_GetSizeCf, Win32File_ReadExactCf, Win32File_Close,
   Win32File_GetCurrentDirectoryCf.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string], WidePath_SplitParentAndLeaf [core/text/path],
   ArenaHeap_Alloc [core/memory/allocator], FatalError_Exit [core/error/runtime], WidePath_CombineDirectoryAndLeaf
   [core/text/path], ArenaHeap_Free [core/memory/allocator].
*/
uint32_t __cdecl FileSystem_Init(void)

{
  uint8_t configByte;
  uint8_t *configCursor;
  BOOL computerNameFound;
  void *handle;
  int clearCount;
  ArenaPayloadByteCount bytes;
  uint16_t *labelCursor;
  ArenaAllocResult allocResult;
  Win32FileOpenResult openResult;
  Win32FileSizeResult sizeResult;
  Win32FileReadResult readResult;
  StatusResult mountResult;
  
  /* open-thandor: the original took the executable path from the first command-line token, which
     is only a bare "thandor.exe" when started from a shell or batch file; the executable
     directory then came out empty. Use the module path instead. */
  Thandor_GetExecutablePathA((char *)g_Win32PathScratchA,sizeof g_Win32PathScratchA);
  Text_CopyNarrowToUtf16Cf(0x200,g_PackageLastErrorPath,g_Win32PathScratchA);
  WidePath_SplitParentAndLeaf
            ((uint16_t *)g_Win32PathScratchA,(uint16_t *)&g_ExecutableDirectoryUtf16,g_PackageLastErrorPath)
  ;
  g_FileSystemOpenCf = Win32File_OpenCf;
  g_FileSystemClose = Win32File_Close;
  g_FileSystemReadExactCf = Win32File_ReadExactCf;
  g_FileSystemWriteExactOrFlushCf = Win32File_WriteExactOrFlushCf;
  g_FileSystemGetSizeCf = Win32File_GetSizeCf;
  g_FileSystemGetPositionCf = Win32File_GetPositionCf;
  g_FileSystemSeekCf = Win32File_SeekCf;
  g_FileSystemDeleteCf = Win32File_DeleteCf;
  g_FileSystemGetCurrentDirectoryCf = Win32File_GetCurrentDirectoryCf;
  g_FileSystemSetCurrentDirectoryCf = Win32File_SetCurrentDirectoryCf;
  g_FileSystemRemoveDirectoryCf = Win32File_RemoveDirectoryCf;
  g_FileSystemCreateDirectoryRecursiveCf = Win32File_CreateDirectoryRecursiveCf;
  g_FileSystemEnumerateDriveLetters = Win32Drive_EnumerateLetters;
  g_FileSystemGetDriveTypeCode = Win32Drive_GetEngineTypeCode;
  g_FileSystemCheckDriveMediaReady = Win32Drive_CheckMediaReadyCf;
  g_FileSystemGetFreeAndTotalBytesRegs = Win32Drive_GetFreeAndTotalBytesRegs;
  g_FileSystemGetLastWriteDosDateCf = Win32File_GetLastWriteDosDateCf;
  g_FileSystemGetLastWriteTimeHighCf = Win32File_GetLastWriteTimeHighCf;
  g_FileSystemGetVolumeSerialNumberCf = Win32Drive_GetVolumeSerialNumberCf;
  g_FileSystemMoveCf = Win32File_MoveCf;
  g_FileSystemCopyCf = Win32File_CopyCf;
  g_FileSystemEnumerateDirectoryOrVolumeEntriesCf =
       Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf;
  g_FileSystemValidateDos83Path = Win32Path_ValidateDos83Cf;
  g_FileSystemInitComputerNameCapacityOrConfigCursor = (pointer)0x100; /* GetComputerNameA size in/out */
  computerNameFound = GetComputerNameA((LPSTR)g_Win32PathScratchA,
                           (LPDWORD)&g_FileSystemInitComputerNameCapacityOrConfigCursor);
  if (computerNameFound != 0) {
    labelCursor = g_DefaultComputerLabelUtf16;
    for (clearCount = 0x10; clearCount != 0; clearCount = clearCount + -1) {
      labelCursor[0] = 0;
      labelCursor[1] = 0;
      labelCursor = labelCursor + 2;
    }
    Text_CopyNarrowToUtf16Cf(0x40,g_DefaultComputerLabelUtf16,g_Win32PathScratchA);
  }
  allocResult = ArenaHeap_Alloc(0x800000);
  if (allocResult.failed) {
                    // WARNING: Subroutine does not return
    FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),true);
  }
  g_PackageScratchBuffer = (uint8_t *)allocResult.payloadOrError;
  openResult = Win32File_OpenCf(0,(uint16_t *)u_THANDOR_cfg_0040e23d);
  handle = (void *)openResult.handleOrError;
  if (openResult.failed) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,(uint16_t *)u_THANDOR_cfg_0040e23d,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = Win32File_OpenCf(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (void *)openResult.handleOrError;
    if (openResult.failed) goto FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage;
  }
  sizeResult = Win32File_GetSizeCf(handle);
  bytes = sizeResult.sizeOrError;
  if ((!sizeResult.failed) && (bytes != 0)) {
    allocResult = ArenaHeap_Alloc(bytes);
    configCursor = (uint8_t *)allocResult.payloadOrError;
    if (!allocResult.failed) {
      readResult = Win32File_ReadExactCf(bytes,configCursor,handle);
      if (readResult.failed) {
        ArenaHeap_Free(configCursor);
      }
      else {
        g_FileSystemInitComputerNameCapacityOrConfigCursor = configCursor;
        g_FileSystemConfigRemainingBytes = bytes;
        /* Normalize the text in place: separators (<= 0x20) and [comments] become NUL, other
           characters go through the normalization map. */
        do {
          configByte = *configCursor;
          if (configByte == 0x5b) {
            /* blank the comment up to its closing ']', which is then blanked as a separator */
            do {
              *configCursor = 0;
              configCursor = configCursor + 1;
              bytes = bytes - 1;
            } while ((bytes != 0) && (*configCursor != 0x5d));
            if (bytes == 0) break;
            configByte = 0;
          }
          if (configByte <= 0x20) {
            *configCursor = 0;
          }
          else {
            *configCursor = (&g_FileSystemConfigCharacterNormalizationMap)[configByte];
          }
          configCursor = configCursor + 1;
          bytes = bytes - 1;
        } while (bytes != 0);
      }
    }
  }
  Win32File_Close(handle);
FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage:
  Win32File_GetCurrentDirectoryCf(g_InitialWorkingDirectory.codeUnits);
  mountResult = Package_MountLowPriority((uint16_t *)u_engine_pck_0040e255);
  if (!mountResult.failed) {
    g_EnginePackageLowPriorityMountHandle = mountResult.valueOrError;
  }
  return mountResult.valueOrError;
}


/* Address: 0x005762F0.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a path, reads its last-write FILETIME, converts it to DOS date/time, and returns the packed DOS
   date with CF clear.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteDosDateCf(uint16_t *path)

{
  BOOL fileTimeQuerySucceeded;
  HANDLE hFile;
  Win32FileOpenResult openResult;
  StatusResult successResult;
  StatusResult failureResult;
  
  openResult = Win32File_OpenCf(0,path);
  hFile = (HANDLE)openResult.handleOrError;
  if ((!openResult.failed) && (hFile != (HANDLE)0xffffffff)) {
    fileTimeQuerySucceeded =
         GetFileTime(hFile,(LPFILETIME)0x0,(LPFILETIME)0x0,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(hFile);
    hFile = (HANDLE)0x1;
    if (fileTimeQuerySucceeded != 0) {
      FileTimeToDosDateTime
                ((FILETIME *)&g_Win32FileLastWriteTimeScratch,
                 (LPWORD)((int)&g_Win32FileCreationTimeOrDosDateScratch + 2),
                 (LPWORD)&g_Win32FileCreationTimeOrDosDateScratch);
      successResult.failed = false;
      successResult.valueOrError = g_Win32FileCreationTimeOrDosDateScratch;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)hFile;
  return failureResult;
}


/* Address: 0x00576360.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a path and returns the high dword of its last-write FILETIME. CF reports open or GetFileTime
   failure.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteTimeHighCf(uint16_t *path)

{
  BOOL fileTimeQuerySucceeded;
  HANDLE hFile;
  Win32FileOpenResult openResult;
  StatusResult successResult;
  StatusResult failureResult;
  
  openResult = Win32File_OpenCf(0,path);
  hFile = (HANDLE)openResult.handleOrError;
  if (!openResult.failed) {
    fileTimeQuerySucceeded =
         GetFileTime(hFile,(LPFILETIME)0x0,(LPFILETIME)0x0,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(hFile);
    hFile = (HANDLE)0x1;
    if (fileTimeQuerySucceeded != 0) {
      successResult.failed = false;
      successResult.valueOrError = g_Win32FileLastWriteTimeHighScratch;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)hFile;
  return failureResult;
}


/* Address: 0x005763C0.
   Ownership: platform/filesystem/win32.
   Purpose: Filesystem service-table callback that normalizes a path, queries Win32 volume information, returns the
   captured volume serial number in EAX, and reports failure through CF.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
uint32_t Win32Drive_GetVolumeSerialNumberCf(uint8_t *outputLabel,char *path)

{
  BOOL volumeInformationQuerySucceeded;
  HANDLE hFile;
  uint32_t volumeSerialNumber;
  Win32FileOpenResult openResult;
  
  openResult = Win32File_OpenCf(0,(uint16_t *)path);
  hFile = (HANDLE)openResult.handleOrError;
  if (!openResult.failed) {
    volumeInformationQuerySucceeded =
         GetFileTime(hFile,(LPFILETIME)&g_Win32FileCreationTimeOrDosDateScratch,
                     (LPFILETIME)&g_Win32FileLastAccessTimeScratch,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(hFile);
    hFile = (HANDLE)0x1;
    if (volumeInformationQuerySucceeded != 0) {
      volumeSerialNumber = g_Win32FileLastWriteTimeHighScratch;
      *outputLabel = 0;
      return volumeSerialNumber;
    }
  }
  return (uint32_t)hFile;
}


/* Address: 0x00575F40.
   Ownership: platform/filesystem/win32.
   Purpose: Restores the UTF-16 working directory captured by FileSystem_Init when the buffer is nonempty.
   Local calls: Win32File_SetCurrentDirectoryCf.
*/
void __cdecl Win32FileSystem_RestoreInitialDirectory(void)

{
  if (g_InitialWorkingDirectory.firstTwoCodeUnits != 0) {
    Win32File_SetCurrentDirectoryCf(g_InitialWorkingDirectory.codeUnits);
  }
  return;
}

/* Address: 0x005766F0.
   Ownership: platform/filesystem/win32.
   Purpose: Fixed and network drives return CF clear immediately. Removable and CD-ROM drives are opened through
   the \\.\X: device path and checked with IOCTL_STORAGE_CHECK_VERIFY. CF set means media is unavailable. Typed
   parameters: p0 driveLetter→DosDriveLetterCode32_V342. Calling convention, exact VariableStorage serialization,
   function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: Win32Drive_GetEngineTypeCode.
*/
bool __thandor_cf_preserve_eax_ecx_edx
Win32Drive_CheckMediaReadyCf(DosDriveLetterCode32 driveLetter)

{
  uint32_t driveTypeCode;
  
  driveTypeCode = Win32Drive_GetEngineTypeCode(driveLetter);
  if ((driveTypeCode != 0x28) && (driveTypeCode != 0x2b)) {
    return false;
  }
  return true;
}


/* Address: 0x0040EF50.
   Ownership: platform/filesystem/win32.
   Purpose: Loads a whole file through the recovered file-system path and returns the carry/error contract.
*/
FileLoadResult __thandor_eax_cf_preserve_ecx_edx FileSystem_LoadWholeFileCf(uint16_t *pathUtf16)

{
  void *handle;
  void *bytes;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  ArenaAllocResult allocResult;
  FileSystemReadResult readResult;
  FileLoadResult failureResult;
  
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  openResult = g_FileSystemOpenCf(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
  handle = (void *)openResult.handleOrError;
  if (openResult.failed) {
    openResult = g_FileSystemOpenCf(0,pathUtf16);
    handle = (void *)openResult.handleOrError;
    if (openResult.failed) {
      failureResult.failed = true;
      failureResult.bufferOrError = handle; /* the open error code */
      return failureResult;
    }
  }
  sizeResult = g_FileSystemGetSizeCf(handle);
  bytes = (void *)sizeResult.sizeOrError;
  if (!sizeResult.failed) {
    allocResult = g_MemoryApi.alloc((uint32_t)bytes);
    if (allocResult.failed) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)bytes,g_FatalErrorDetail1Utf16);
      bytes = (void *)0x5;
    }
    else {
      readResult = g_FileSystemReadExactCf((FileIoByteCount)bytes,(void *)allocResult.payloadOrError,handle);
      bytes = (void *)readResult.valueOrError;
      if (!readResult.failed) {
        g_FileSystemClose(handle);
        return THANDOR_BITCAST(uint64_t, FileLoadResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
      g_MemoryApi.free((void *)allocResult.payloadOrError);
    }
  }
  g_FileSystemClose(handle);
  handle = bytes;
  failureResult.failed = true;
  failureResult.bufferOrError = handle;
  return failureResult;
}

/* Address: 0x0040F120.
   Ownership: platform/filesystem/win32.
   Purpose: Loads a whole file through the alternate recovered path and returns the carry/error contract.
*/
FileLoadResult __thandor_eax_cf_preserve_edx
FileSystem_LoadWholeFileAlternatePathCf(uint16_t *pathUtf16)

{
  void *handle;
  void *bytes;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  ArenaAllocResult allocResult;
  FileSystemReadResult readResult;
  FileLoadResult failureResult;
  
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  openResult = g_FileSystemOpenCf(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
  handle = (void *)openResult.handleOrError;
  if (openResult.failed) {
    openResult = g_FileSystemOpenCf(0,pathUtf16);
    handle = (void *)openResult.handleOrError;
    if (openResult.failed) {
      failureResult.failed = true;
      failureResult.bufferOrError = handle; /* the open error code */
      return failureResult;
    }
  }
  sizeResult = g_FileSystemGetSizeCf(handle);
  bytes = (void *)sizeResult.sizeOrError;
  if (!sizeResult.failed) {
    allocResult = g_MemoryApi.alloc((uint32_t)bytes);
    if (allocResult.failed) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)bytes,g_FatalErrorDetail1Utf16);
      bytes = (void *)0x5;
    }
    else {
      readResult = g_FileSystemReadExactCf((FileIoByteCount)bytes,(void *)allocResult.payloadOrError,handle);
      bytes = (void *)readResult.valueOrError;
      if (!readResult.failed) {
        g_FileSystemClose(handle);
        return THANDOR_BITCAST(uint64_t, FileLoadResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
      g_MemoryApi.free((void *)allocResult.payloadOrError);
    }
  }
  g_FileSystemClose(handle);
  handle = bytes;
  failureResult.failed = true;
  failureResult.bufferOrError = handle;
  return failureResult;
}

/* Address: 0x0040F1F0.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a UTF-16 path with engine mode 3, writes exactly byteCount bytes, and closes the handle. On write
   failure it closes and deletes the partial file. CF clear returns EAX zero; CF set preserves the backend error.
*/
StatusResult FileSystem_WriteBufferToPathCf(FileIoByteCount byteCount,void *source,uint16_t *path)

{
  void *handle;
  void *writeFailureStatusCode;
  FileSystemOpenResult openResult;
  FileSystemWriteResult writeResult;
  StatusResult successResult;
  StatusResult failureResult;
  
  openResult = g_FileSystemOpenCf
                    (FILESYSTEM_OPEN_EXCLUSIVE_SHARE|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE,path);
  handle = (void *)openResult.handleOrError;
  if (!openResult.failed) {
    writeResult = g_FileSystemWriteExactOrFlushCf(byteCount,source,handle);
    writeFailureStatusCode = (void *)writeResult.valueOrError;
    if (!writeResult.failed) {
      g_FileSystemClose(handle);
      successResult.valueOrError = 0;
      successResult.failed = false;
      return successResult;
    }
    g_FileSystemClose(handle);
    handle = writeFailureStatusCode;
    g_FileSystemDeleteCf(1,path);
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)handle;
  return failureResult;
}


/* Address: 0x00576070.
   Ownership: platform/filesystem/win32.
   Purpose: Writes exactly byteCount bytes, or flushes the handle when byteCount is zero. CF set returns engine
   error 7 or 8.
*/
Win32FileWriteResult __thandor_eax_cf_preserve_ecx_edx
Win32File_WriteExactOrFlushCf(FileIoByteCount byteCount,void *source,void *handle)

{
  BOOL operationSucceeded;
  uint32_t writeCompletionStatusCode;
  BOOL setEndOfFileSucceeded;
  Win32FileWriteResult successResult;
  Win32FileWriteResult flushResult;
  Win32FileWriteResult failureResult;
  
  g_Win32FileBytesTransferred = 0;
  if (byteCount == 0) {
    setEndOfFileSucceeded = SetEndOfFile(handle);
    flushResult.failed = false;
    flushResult.valueOrError = setEndOfFileSucceeded;
    return flushResult;
  }
  operationSucceeded =
       WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  writeCompletionStatusCode = 8;
  if ((operationSucceeded != 0) &&
     (writeCompletionStatusCode = 7, byteCount == g_Win32FileBytesTransferred)) {
    successResult.valueOrError = 7;
    successResult.failed = false;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = writeCompletionStatusCode;
  return failureResult;
}


/* Address: 0x00576140.
   Ownership: platform/filesystem/win32.
   Purpose: Queries the current file position with SetFilePointer(FILE_CURRENT). CF reports failure.
*/
uint32_t Win32File_GetPositionCf(void *handle)

{
  DWORD filePosition;
  
  filePosition = SetFilePointer(handle,0,(PLONG)0x0,1);
  if (filePosition != 0xffffffff) {
    return filePosition;
  }
  return 0;
}

/* Address: 0x00576180.
   Ownership: platform/filesystem/win32.
   Purpose: Error 9 is returned with CF set. Typed parameters: p0 moveMethod→FileSystemSeekOrigin_V331, p1
   distance→FileSystemFilePosition_V331. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
Win32FileSeekResult __thandor_eax_cf_preserve_ecx_edx
Win32File_SeekCf(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle)

{
  DWORD newFilePosition;
  Win32FileSeekResult successResult;
  Win32FileSeekResult failureResult;
  
  newFilePosition = SetFilePointer(handle,distance,(PLONG)0x0,moveMethod);
  if (newFilePosition != 0xffffffff) {
    successResult.failed = false;
    successResult.positionOrError = newFilePosition;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.positionOrError = 9;
  return failureResult;
}


/* Address: 0x005761C0.
   Ownership: platform/filesystem/win32.
   Purpose: Converts and deletes one UTF-16 path. The first argument is an unused backend flags slot; CF set
   returns error 1.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
uint32_t Win32File_DeleteCf(uint32_t unusedFlags,uint16_t *path)

{
  BOOL operationSucceeded;
  
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = DeleteFileA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    return operationSucceeded;
  }
  return 1;
}

/* Address: 0x00576210.
   Ownership: platform/filesystem/win32.
   Purpose: Converts source and destination UTF-16 paths and calls MoveFileA. CF set returns error 1.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_MoveCf(uint16_t *destinationPath,uint16_t *sourcePath)

{
  BOOL operationSucceeded;
  StatusResult successResult;
  StatusResult failureResult;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = MoveFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB);
  if (operationSucceeded != 0) {
    successResult.failed = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = 1;
  return failureResult;
}


/* Address: 0x00576280.
   Ownership: platform/filesystem/win32.
   Purpose: Converts source and destination UTF-16 paths and calls CopyFileA with fail-if-exists enabled. CF set
   returns error 1.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_CopyCf(uint16_t *destinationPath,uint16_t *sourcePath)

{
  BOOL operationSucceeded;
  StatusResult successResult;
  StatusResult failureResult;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = CopyFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB,1);
  if (operationSucceeded != 0) {
    successResult.failed = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = 1;
  return failureResult;
}


/* Address: 0x005764E0.
   Ownership: platform/filesystem/win32.
   Purpose: Creates a directory and, when flag bit zero is set, recursively creates the parent derived by
   WidePath_SplitParentAndLeaf. Error 8 is returned with CF set. Typed parameters: p0
   flags→FileSystemCreateDirectoryFlags_V331. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext], WidePath_SplitParentAndLeaf [core/text/path].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_CreateDirectoryRecursiveCf(FileSystemCreateDirectoryFlags flags,uint16_t *path)

{
  uint32_t createSucceeded;
  StatusResult parentOrSuccessResult;
  StatusResult failureResult;
  uint16_t parentPath [256];
  uint16_t leafName [248];

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  createSucceeded = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
  if (createSucceeded == 0) {
    if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
      WidePath_SplitParentAndLeaf(leafName,parentPath,path);
      parentOrSuccessResult = Win32File_CreateDirectoryRecursiveCf(flags,parentPath);
      if (!parentOrSuccessResult.failed) {
        RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
        createSucceeded = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
        if (createSucceeded != 0) {
          /* created after its parents */
          parentOrSuccessResult.failed = false;
          parentOrSuccessResult.valueOrError = createSucceeded;
          return parentOrSuccessResult;
        }
      }
    }
    Package_SetLastErrorPath(path);
    failureResult.failed = true;
    failureResult.valueOrError = 8;
    return failureResult;
  }
  parentOrSuccessResult.failed = false;
  parentOrSuccessResult.valueOrError = createSucceeded;
  return parentOrSuccessResult;
}


/* Address: 0x005765A0.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls RemoveDirectoryA. Error 11 is returned with CF set.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_RemoveDirectoryCf(uint16_t *path)

{
  BOOL operationSucceeded;
  StatusResult successResult;
  StatusResult failureResult;
  
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = RemoveDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    successResult.failed = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = 0xb;
  return failureResult;
}


/* Address: 0x005765F0.
   Ownership: platform/filesystem/win32.
   Purpose: Queries GetDiskFreeSpaceA for one drive letter. EAX receives free bytes and EDX receives total bytes;
   both are zero on failure. Typed parameters: p0 driveLetter→DosDriveLetterCode32_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
Win32DriveCapacityEdxEax8 Win32Drive_GetFreeAndTotalBytesRegs(DosDriveLetterCode32 driveLetter)

{
  BOOL querySucceeded;
  int freeBytes;
  int totalBytes;
  
  g_Win32DriveRootPathScratchA = (uint8_t)driveLetter; /* "X:\" root path scratch */
  querySucceeded =
       GetDiskFreeSpaceA(&g_Win32DriveRootPathScratchA,(LPDWORD)&g_Win32DiskSectorsPerClusterScratch
                         ,(LPDWORD)&g_Win32DiskBytesPerSectorScratch,
                         (LPDWORD)&g_Win32DiskFreeClustersScratch,
                         (LPDWORD)&g_Win32DiskTotalClustersScratch);
  totalBytes = 0;
  freeBytes = 0;
  if (querySucceeded != 0) {
    freeBytes = g_Win32DiskFreeClustersScratch *
                g_Win32DiskBytesPerSectorScratch * g_Win32DiskSectorsPerClusterScratch;
    totalBytes = g_Win32DiskTotalClustersScratch *
                 g_Win32DiskBytesPerSectorScratch * g_Win32DiskSectorsPerClusterScratch;
  }
  return (uint64_t)(uint32_t)totalBytes << 0x20 | (uint64_t)(uint32_t)freeBytes; /* EDX = total, EAX = free */
}

/* Address: 0x005766B0.
   Ownership: platform/filesystem/win32.
   Purpose: Writes uppercase letters for every bit returned by GetLogicalDrives and returns the number of letters
   written.
*/
DriveLetterEnumerationEaxEcx8 __thandor_eax_ecx_preserve_edx
Win32Drive_EnumerateLetters(uint8_t *lettersOut)

{
  uint32_t logicalDriveMask;
  uint32_t enumeratedDriveCount;
  uint8_t currentDriveLetter;
  int driveLettersRemaining;
  DriveLetterEnumerationEaxEcx8 enumerationResult;
  
  logicalDriveMask = GetLogicalDrives();
  enumeratedDriveCount = 0;
  driveLettersRemaining = 0x1a;
  currentDriveLetter = 0x41;
  do {
    if ((logicalDriveMask & 1) != 0) {
      *lettersOut = currentDriveLetter;
      enumeratedDriveCount = enumeratedDriveCount + 1;
      lettersOut = lettersOut + 1;
    }
    logicalDriveMask = logicalDriveMask >> 1;
    currentDriveLetter = currentDriveLetter + 1;
    driveLettersRemaining = driveLettersRemaining + -1;
  } while (driveLettersRemaining != 0);
  enumerationResult.driveCountMirror = enumeratedDriveCount;
  enumerationResult.driveCount = enumeratedDriveCount;
  return enumerationResult;
}


/* Address: 0x00576790.
   Ownership: platform/filesystem/win32.
   Purpose: Validates an ANSI path as DOS-style 8.3 components. Flag bit 0 permits '*' and '?', bit 1 selects one-
   component recursion, and bit 2 permits directory separators and a trailing path. CF clear means valid; CF set
   means rejected. Typed parameters: p0 flags→FileSystemDos83ValidationFlags_V331. Nearby but non-identical
   semantic domains were explicitly deferred.
*/
bool __thandor_cf_preserve_eax_ecx_edx
Win32Path_ValidateDos83Cf(FileSystemDos83ValidationFlags flags,uint8_t *pathAnsi)

{
  uint8_t pathChar;
  int charsRemaining;
  uint8_t *previousCursor;
  bool componentRejected;
  
  if ((flags & FILESYSTEM_DOS83_COMPONENT_ONLY) == 0) {
    if (pathAnsi[1] == 0x3a) {
      pathChar = *pathAnsi;
      if (pathChar < 0x41) {
        return true;
      }
      if (0x7a < pathChar) {
        return true;
      }
      if ((pathChar < 0x61) && (0x5a < pathChar)) {
        return true;
      }
      pathAnsi = pathAnsi + 2;
    }
    if (*pathAnsi == 0x5c) {
      pathAnsi = pathAnsi + 1;
    }
    while (componentRejected = Win32Path_ValidateDos83Cf
                             (flags | (FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION|
                                      FILESYSTEM_DOS83_COMPONENT_ONLY),pathAnsi), !componentRejected) {
      while( true ) {
        pathChar = *pathAnsi;
        pathAnsi = pathAnsi + 1;
        if (pathChar == 0x5c) break;
        if (pathChar == 0) {
          return false;
        }
      }
    }
  }
  else {
    charsRemaining = 8;
    do {
      pathChar = *pathAnsi;
      if (pathChar == 0) break;
      if (pathChar == 0x2a) {
        pathAnsi = pathAnsi + 1;
        if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
          return true;
        }
        break;
      }
      if (0x2c < pathChar) {
        if (pathChar < 0x2f) break;
        if (pathChar == 0x2f) {
          return true;
        }
        if (0x39 < pathChar) {
          if (pathChar == 0x3f) {
            if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
              return true;
            }
          }
          else {
            if (pathChar < 0x41) {
              return true;
            }
            if (0x5a < pathChar) {
              if (pathChar == 0x5c) break;
              if (pathChar < 0x61) {
                return true;
              }
              if (0x7a < pathChar) {
                return true;
              }
            }
          }
        }
      }
      pathAnsi = pathAnsi + 1;
      charsRemaining = charsRemaining + -1;
    } while (charsRemaining != 0);
    if (charsRemaining != 8) {
      pathChar = *pathAnsi;
      charsRemaining = 3;
      if (pathChar != 0) {
        if (pathChar == 0x2e) {
          do {
            previousCursor = pathAnsi;
            pathAnsi = previousCursor + 1;
            pathChar = *pathAnsi;
            if (pathChar == 0) {
              return false;
            }
            if (pathChar == 0x2a) {
              if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                return true;
              }
              break;
            }
            if (0x2c < pathChar) {
              if (pathChar < 0x30) {
                return true;
              }
              if (0x39 < pathChar) {
                if (pathChar == 0x3f) {
                  if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                    return true;
                  }
                }
                else {
                  if (pathChar < 0x41) {
                    return true;
                  }
                  if (0x5a < pathChar) {
                    if (pathChar == 0x5c) {
                      return false;
                    }
                    if (pathChar < 0x61) {
                      return true;
                    }
                    if (0x7a < pathChar) {
                      return true;
                    }
                  }
                }
              }
            }
            charsRemaining = charsRemaining + -1;
          } while (charsRemaining != 0);
          pathChar = previousCursor[2];
          if (pathChar == 0) {
            return false;
          }
        }
        if ((flags & FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION) == 0) {
          return true;
        }
        if (pathChar != 0x5c) {
          return true;
        }
      }
      return false;
    }
  }
  return true;
}


/* Address: 0x00576910.
   Ownership: platform/filesystem/win32.
   Purpose: Win32 file-system callback. Mode 0/2 enumerates directory entries into fixed 0x200-byte records and
   sorts them; mode 1 queries volume information. Returns 0x200 in EAX and the produced record count in ECX; carry
   remains clear on archived exits. Typed parameters: p0 mode→FileSystemEnumerationMode_V331. Nearby but non-
   identical semantic domains were explicitly deferred.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string], Package_SetLastErrorPath
   [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf [assets/text/richtext],
   Utf16String_CompareAsciiCaseInsensitiveFlags [core/text/string].
*/
DirectoryEnumerationResult __thandor_eax_ecx_cf_preserve_edx
Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText)

{
  HANDLE hFindFile;
  BOOL apiSucceeded;
  uint32_t recordCount;
  int comparisonsRemaining;
  int dwordsRemaining;
  uint32_t *rightRecordDwords;
  uint32_t *copySource;
  uint16_t *destination;
  uint32_t *leftRecordDwords;
  uint32_t *copyDestination;
  DirectoryEnumerationResult enumerationResult;
  DirectoryEnumerationResult volumeResult;
  TextCompareResult compareFlags;
  int passesRemaining;
  
  if (mode == FILESYSTEM_ENUMERATE_VOLUME_LABEL) {
    g_Win32DriveRootPathScratchA = *pathOrVolumeText;
    apiSucceeded = GetVolumeInformationA
                      ((LPCSTR)&g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratchA,0x80,
                       (LPDWORD)0x0,(LPDWORD)0x0,(LPDWORD)0x0,(LPSTR)0x0,0);
    if (apiSucceeded == 0) {
      enumerationResult.recordSizeBytes = 0x200;
      enumerationResult.entryCount = 0;
      enumerationResult.failed = false;
      return enumerationResult;
    }
    recordCount = 0;
    if (0xff < outputCapacityBytes) {
      Text_CopyNarrowToUtf16Cf(0x200,(uint16_t *)outputRecords,g_Win32PathScratchA);
      volumeResult.entryCount = 1;
      volumeResult.recordSizeBytes = 0x200;
      volumeResult.failed = false;
      return volumeResult;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)pathOrVolumeText);
    RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,(uint16_t *)pathOrVolumeText);
    hFindFile = FindFirstFileA((LPCSTR)g_Win32PathScratchA,
                               (LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    if (hFindFile == (HANDLE)0xffffffff) {
      enumerationResult.recordSizeBytes = 0x200;
      enumerationResult.entryCount = 0;
      enumerationResult.failed = false;
      return enumerationResult;
    }
    recordCount = 0;
    destination = (uint16_t *)outputRecords;
    do {
      /* files: neither directory nor volume label (attributes & 0x18); directories: not "." or ".." */
      if ((mode == FILESYSTEM_ENUMERATE_FILES) ?
          ((g_Win32FileCreationTimeOrDosDateScratch & 0x18) == 0) :
          ((mode == FILESYSTEM_ENUMERATE_DIRECTORIES) &&
           ((g_Win32FileCreationTimeOrDosDateScratch & 0x10) != 0) &&
           ((g_Win32FindDataFileNameA != '.') ||
            ((g_Win32FindDataFileNameSecondCharA != '\0') &&
             ((g_Win32FindDataFileNameSecondCharA != '.') || (g_Win32FindDataFileNameThirdCharA != '\0')))))) {
        if (0x1ff < outputCapacityBytes) {
          Text_CopyNarrowToUtf16Cf(0x200,destination,(uint8_t *)&g_Win32FindDataFileNameA);
          destination = destination + 0x100;
          recordCount = recordCount + 1;
          outputCapacityBytes = outputCapacityBytes - 0x200;
        }
      }
      apiSucceeded = FindNextFileA(hFindFile,(LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    } while (apiSucceeded != 0);
    FindClose(hFindFile);
    if (1 < recordCount) {
      comparisonsRemaining = recordCount - 1;
      rightRecordDwords = (uint32_t *)(outputRecords + 0x200);
      leftRecordDwords = (uint32_t *)outputRecords;
      passesRemaining = comparisonsRemaining;
      do {
        do {
          compareFlags = Utf16String_CompareAsciiCaseInsensitiveFlags
                            ((uint16_t *)rightRecordDwords,(uint16_t *)leftRecordDwords);
          if (!compareFlags.less && !compareFlags.equal) {
            copySource = rightRecordDwords;
            copyDestination = (uint32_t *)g_Win32PathScratchA;
            for (dwordsRemaining = 0x80; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
              *copyDestination = *copySource;
              copySource = copySource + 1;
              copyDestination = copyDestination + 1;
            }
            copySource = leftRecordDwords;
            copyDestination = rightRecordDwords;
            for (dwordsRemaining = 0x80; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
              *copyDestination = *copySource;
              copySource = copySource + 1;
              copyDestination = copyDestination + 1;
            }
            copySource = (uint32_t *)g_Win32PathScratchA;
            copyDestination = leftRecordDwords;
            for (dwordsRemaining = 0x80; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
              *copyDestination = *copySource;
              copySource = copySource + 1;
              copyDestination = copyDestination + 1;
            }
          }
          rightRecordDwords = rightRecordDwords + 0x80;
          leftRecordDwords = leftRecordDwords + 0x80;
          comparisonsRemaining = comparisonsRemaining + -1;
        } while (comparisonsRemaining != 0);
        comparisonsRemaining = passesRemaining + -1;
        rightRecordDwords = (uint32_t *)(outputRecords + 0x200);
        leftRecordDwords = (uint32_t *)outputRecords;
        passesRemaining = comparisonsRemaining;
      } while (comparisonsRemaining != 0);
    }
  }
  enumerationResult.entryCount = recordCount;
  enumerationResult.recordSizeBytes = 0x200;
  enumerationResult.failed = false;
  return enumerationResult;
}


/* Address: 0x00576020.
   Ownership: platform/filesystem/win32.
   Purpose: Reads exactly byteCount bytes. CF clear means the requested count was transferred; CF set returns
   engine error 6.
*/
Win32FileReadResult __thandor_eax_cf_preserve_ecx_edx
Win32File_ReadExactCf(FileIoByteCount byteCount,void *destination,void *handle)

{
  Win32FileReadResult successResult;
  Win32FileReadResult failureResult;
  
  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  if (g_Win32FileBytesTransferred == byteCount) {
    successResult.failed = false;
    successResult.valueOrError = g_Win32FileBytesTransferred;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = 6;
  return failureResult;
}


/* Address: 0x00576100.
   Ownership: platform/filesystem/win32.
   Purpose: Returns the low 32-bit file size with CF clear. GetFileSize failure returns zero with CF set.
*/
Win32FileSizeResult __thandor_eax_cf_preserve_ecx_edx Win32File_GetSizeCf(void *handle)

{
  DWORD fileSize;
  Win32FileSizeResult successResult;
  Win32FileSizeResult failureResult;
  
  fileSize = GetFileSize(handle,(LPDWORD)0x0);
  if (fileSize != 0xffffffff) {
    successResult.failed = false;
    successResult.sizeOrError = fileSize;
    return successResult;
  }
  failureResult.sizeOrError = 0;
  failureResult.failed = true;
  return failureResult;
}


/* Address: 0x00576430.
   Ownership: platform/filesystem/win32.
   Purpose: Gets the current ANSI directory and converts it into a 0x200-byte UTF-16 destination. CF reports Win32
   failure.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_GetCurrentDirectoryCf(uint16_t *destination)

{
  DWORD narrowPathLength;
  int copiedPathByteLength;
  StatusResult copyResult;
  
  narrowPathLength = GetCurrentDirectoryA(0xff,(LPSTR)g_Win32PathScratchA);
  if (narrowPathLength != 0) {
    copyResult = Text_CopyNarrowToUtf16Cf(0x200,destination,g_Win32PathScratchA);
    return THANDOR_BITCAST(uint64_t, StatusResult, ((THANDOR_BITCAST(StatusResult, uint64_t, copyResult) & 0xFFFFFFFFFFull) & 0xffffffff));
  }
  destination[0] = 0;
  destination[1] = 0;
  copyResult.valueOrError = 0;
  copyResult.failed = true;
  return copyResult;
}


/* Address: 0x00576490.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls SetCurrentDirectoryA. Error 10 is returned with CF set.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_SetCurrentDirectoryCf(uint16_t *path)

{
  BOOL operationSucceeded;
  StatusResult successResult;
  StatusResult failureResult;
  
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = SetCurrentDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    successResult.failed = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = 10;
  return failureResult;
}


/* Address: 0x00576650.
   Ownership: platform/filesystem/win32.
   Purpose: Maps GetDriveTypeA to engine codes 0x28 removable, 0x2A remote, 0x2B CD-ROM, or 0x29 otherwise. Typed
   parameters: p0 driveLetter→DosDriveLetterCode32_V342. Calling convention, exact VariableStorage serialization,
   function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
EngineDriveTypeCode __thandor_eax_preserve_ecx_edx
Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter)

{
  UINT driveTypeCode;
  
  g_Win32DriveRootPathScratchA = (uint8_t)driveLetter; /* "X:\" root path scratch */
  driveTypeCode = GetDriveTypeA(&g_Win32DriveRootPathScratchA);
  if (driveTypeCode == 2) {
    return ENGINE_DRIVE_REMOVABLE;
  }
  if (1 < driveTypeCode) {
    if (driveTypeCode == 4) {
      return ENGINE_DRIVE_REMOTE;
    }
    if ((3 < driveTypeCode) && (driveTypeCode < 6)) {
      return ENGINE_DRIVE_CDROM;
    }
  }
  return ENGINE_DRIVE_OTHER;
}


/* Address: 0x00575F60.
   Ownership: platform/filesystem/win32.
   Purpose: Converts a UTF-16 path and maps engine open flags to CreateFileA access, sharing, and creation modes.
   CF clear returns a handle; CF set returns error 1. Typed parameters: p0 openFlags→FileSystemOpenFlags_V331.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
Win32FileOpenResult __thandor_eax_cf_preserve_ecx_edx
Win32File_OpenCf(FileSystemOpenFlags openFlags,uint16_t *path)

{
  HANDLE fileHandle;
  Win32FileOpenResult successResult;
  Win32FileOpenResult failureResult;
  DWORD dwDesiredAccess;
  DWORD dwShareMode;
  DWORD dwCreationDisposition;
  
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  if ((openFlags & FILESYSTEM_OPEN_CREATE_OR_TRUNCATE) == 0) {
    if ((openFlags & FILESYSTEM_OPEN_EXISTING_OR_CREATE) == 0) {
      dwCreationDisposition = 3;
    }
    else {
      dwCreationDisposition = 4;
    }
  }
  else {
    dwCreationDisposition = 2;
  }
  if ((openFlags & FILESYSTEM_OPEN_EXCLUSIVE_SHARE) == 0) {
    if ((openFlags & FILESYSTEM_OPEN_CREATE_OR_TRUNCATE) == 0) {
      dwShareMode = 3;
    }
    else {
      dwShareMode = 1;
    }
  }
  else {
    dwShareMode = 0;
  }
  if ((openFlags & (FILESYSTEM_OPEN_WRITE_ACCESS|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE)) == 0) {
    dwDesiredAccess = 0x80000000;
  }
  else {
    dwDesiredAccess = 0xc0000000;
  }
  fileHandle = CreateFileA((LPCSTR)g_Win32PathScratchA,dwDesiredAccess,dwShareMode,
                           (LPSECURITY_ATTRIBUTES)0x0,dwCreationDisposition,0x80000080,(HANDLE)0x0);
  if (fileHandle != (HANDLE)0xffffffff) {
    successResult.failed = false;
    successResult.handleOrError = (uint32_t)fileHandle;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.handleOrError = 1;
  return failureResult;
}


/* Address: 0x00576000.
   Ownership: platform/filesystem/win32.
   Purpose: Closes one Win32 file handle.
*/
void __thandor_void_preserve_eax_ecx_edx Win32File_Close(void *handle)

{
  CloseHandle(handle);
  return;
}

