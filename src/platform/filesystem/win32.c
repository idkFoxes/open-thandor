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
FileSystemStringTableEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
FileSystem_BuildEnumerationStringTableCf
          (FileSystemEnumerationMode enumerationMode,dword reserved,byte *pathOrVolumeText)

{
  short codeUnit;
  byte *outputRecords;
  byte *recordStride;
  byte *memory;
  FileSystemOutputCapacityBytes foundEntryCount;
  FileSystemOutputCapacityBytes outputCapacityBytes;
  FileSystemOutputCapacityBytes remainingEntries;
  byte *pointerSlot;
  byte *sourceChar;
  byte *sourceRecord;
  byte *stringCursor;
  bool capacityCheck;
  ArenaShrinkEaxCf5 shrinkResult;
  ArenaFreeEaxCf5 freeResult;
  ArenaLargestAllocationEaxEcxCf9 largestBlock;
  FileSystemEnumerationEaxEcxCf9 enumerationResult;
  FileSystemStringTableEaxEcxCf9 successResult;
  FileSystemStringTableEaxEcxCf9 failureResult;
  
  largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
  outputCapacityBytes = largestBlock.blockSizeOrSentinel;
  outputRecords = (byte *)largestBlock.allocationOrError;
  if (!largestBlock.carry) {
    enumerationResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                       (enumerationMode,reserved,outputCapacityBytes,outputRecords,pathOrVolumeText)
    ;
    foundEntryCount = enumerationResult.entryCount;
    recordStride = (byte *)enumerationResult.recordSizeBytes;
    memory = recordStride;
    outputCapacityBytes = foundEntryCount;
    if (!enumerationResult.carry) {
      if (foundEntryCount == 0) {
        (*g_MemoryApi.free)(outputRecords);
        return THANDOR_BITCAST(unkuint9, FileSystemStringTableEaxEcxCf9, (unkuint9)0);
      }
      outputCapacityBytes = foundEntryCount * (int)recordStride;
      shrinkResult = (*g_MemoryApi.shrinkInPlace)(outputCapacityBytes,outputRecords);
      memory = (byte *)shrinkResult.scratchOrError;
      if (!shrinkResult.carry) {
        largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
        outputCapacityBytes = largestBlock.blockSizeOrSentinel;
        memory = (byte *)largestBlock.allocationOrError;
        if (!largestBlock.carry) {
          stringCursor = memory + foundEntryCount * 4;
          capacityCheck = foundEntryCount * 4 <= outputCapacityBytes;
          outputCapacityBytes = outputCapacityBytes + foundEntryCount * -4;
          remainingEntries = foundEntryCount;
          pointerSlot = memory;
          sourceRecord = outputRecords;
          if (capacityCheck && outputCapacityBytes != 0) {
            do {
              *(byte **)pointerSlot = stringCursor;
              sourceChar = sourceRecord;
              do {
                codeUnit = *(short *)sourceChar;
                *(short *)stringCursor = codeUnit;
                sourceChar = sourceChar + 2;
                stringCursor = stringCursor + 2;
                capacityCheck = outputCapacityBytes < 2;
                outputCapacityBytes = outputCapacityBytes - 2;
                if (capacityCheck || outputCapacityBytes == 0) goto LAB_0040f509;
              } while (codeUnit != 0);
              pointerSlot = pointerSlot + 4;
              sourceRecord = sourceRecord + (int)recordStride;
              remainingEntries = remainingEntries - 1;
              if (remainingEntries == 0) {
                (*g_MemoryApi.shrinkInPlace)((int)stringCursor - (int)memory,memory);
                (*g_MemoryApi.free)(outputRecords);
                successResult.entryCountOrScratch = foundEntryCount;
                successResult.tableOrError = (dword)memory;
                successResult.carry = false;
                return successResult;
              }
            } while( true );
          }
LAB_0040f509:
          freeResult = (*g_MemoryApi.free)(memory);
          memory = (byte *)freeResult.eax;
        }
      }
    }
    (*g_MemoryApi.free)(outputRecords);
    outputRecords = memory;
  }
  failureResult.entryCountOrScratch = outputCapacityBytes;
  failureResult.tableOrError = (dword)outputRecords;
  failureResult.carry = true;
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
dword __cdecl FileSystem_Init(void)

{
  byte configByte;
  dword remainingBytesSnapshot;
  byte *configCursor;
  BOOL computerNameFound;
  void *handle;
  int clearCount;
  ArenaPayloadByteCount bytes;
  byte *cursorSnapshot;
  word *labelCursor;
  ArenaAllocEaxCf5 allocResult;
  Win32FileOpenEaxCf5 openResult;
  Win32FileSizeEaxCf5 sizeResult;
  Win32FileReadEaxCf5 readResult;
  StatusValueEaxCf5 mountResult;
  
  /* open-thandor: the original took the executable path from the first command-line token, which
     is only a bare "thandor.exe" when started from a shell or batch file; the executable
     directory then came out empty. Use the module path instead. */
  Thandor_GetExecutablePathA((char *)g_Win32PathScratchA,sizeof g_Win32PathScratchA);
  Text_CopyNarrowToUtf16Cf(0x200,g_PackageLastErrorPath,g_Win32PathScratchA);
  WidePath_SplitParentAndLeaf
            ((word *)g_Win32PathScratchA,(word *)&g_ExecutableDirectoryUtf16,g_PackageLastErrorPath)
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
  g_FileSystemInitComputerNameCapacityOrConfigCursor = (undefined *)0x100;
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
  if (allocResult.carry) {
                    // WARNING: Subroutine does not return
    FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),true);
  }
  g_PackageScratchBuffer = (byte *)allocResult.eax;
  openResult = Win32File_OpenCf(0,(word *)u_THANDOR_cfg_0040e23d);
  handle = (void *)openResult.eax;
  if (openResult.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,(word *)u_THANDOR_cfg_0040e23d,
               (word *)&g_ExecutableDirectoryUtf16);
    openResult = Win32File_OpenCf(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (void *)openResult.eax;
    if (openResult.carry) goto FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage;
  }
  sizeResult = Win32File_GetSizeCf(handle);
  bytes = sizeResult.eax;
  if ((!sizeResult.carry) && (bytes != 0)) {
    allocResult = ArenaHeap_Alloc(bytes);
    configCursor = (byte *)allocResult.eax;
    if (!allocResult.carry) {
      readResult = Win32File_ReadExactCf(bytes,configCursor,handle);
      cursorSnapshot = configCursor;
      remainingBytesSnapshot = bytes;
      if (readResult.carry) {
        ArenaHeap_Free(configCursor);
      }
      else {
        do {
          while( true ) {
            g_FileSystemConfigRemainingBytes = remainingBytesSnapshot;
            g_FileSystemInitComputerNameCapacityOrConfigCursor = cursorSnapshot;
            configByte = *configCursor;
            if (0x20 < configByte) break;
FileSystemConfig_TerminateSeparatorOrComment:
            *configCursor = 0;
            configCursor = configCursor + 1;
            bytes = bytes - 1;
            cursorSnapshot = g_FileSystemInitComputerNameCapacityOrConfigCursor;
            remainingBytesSnapshot = g_FileSystemConfigRemainingBytes;
            if (bytes == 0) goto FileSystemConfig_CloseInput;
          }
          if (configByte == 0x5b) {
            do {
              *configCursor = 0;
              configCursor = configCursor + 1;
              bytes = bytes - 1;
              if (bytes == 0) goto FileSystemConfig_CloseInput;
            } while (*configCursor != 0x5d);
            goto FileSystemConfig_TerminateSeparatorOrComment;
          }
          *configCursor = (&g_FileSystemConfigCharacterNormalizationMap)[configByte];
          configCursor = configCursor + 1;
          bytes = bytes - 1;
          cursorSnapshot = g_FileSystemInitComputerNameCapacityOrConfigCursor;
          remainingBytesSnapshot = g_FileSystemConfigRemainingBytes;
        } while (bytes != 0);
      }
    }
  }
FileSystemConfig_CloseInput:
  Win32File_Close(handle);
FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage:
  Win32File_GetCurrentDirectoryCf(g_InitialWorkingDirectory.codeUnits);
  mountResult = Package_MountLowPriority((word *)u_engine_pck_0040e255);
  if (!mountResult.carry) {
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteDosDateCf(word *path)

{
  BOOL fileTimeQuerySucceeded;
  HANDLE hFile;
  Win32FileOpenEaxCf5 openResult;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  openResult = Win32File_OpenCf(0,path);
  hFile = (HANDLE)openResult.eax;
  if ((!openResult.carry) && (hFile != (HANDLE)0xffffffff)) {
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
      successResult.carry = false;
      successResult.valueOrError = g_Win32FileCreationTimeOrDosDateScratch;
      return successResult;
    }
  }
  failureResult.carry = true;
  failureResult.valueOrError = (dword)hFile;
  return failureResult;
}


/* Address: 0x00576360.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a path and returns the high dword of its last-write FILETIME. CF reports open or GetFileTime
   failure.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteTimeHighCf(word *path)

{
  BOOL fileTimeQuerySucceeded;
  HANDLE hFile;
  Win32FileOpenEaxCf5 openResult;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  openResult = Win32File_OpenCf(0,path);
  hFile = (HANDLE)openResult.eax;
  if (!openResult.carry) {
    fileTimeQuerySucceeded =
         GetFileTime(hFile,(LPFILETIME)0x0,(LPFILETIME)0x0,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(hFile);
    hFile = (HANDLE)0x1;
    if (fileTimeQuerySucceeded != 0) {
      successResult.carry = false;
      successResult.valueOrError = g_Win32FileLastWriteTimeHighScratch;
      return successResult;
    }
  }
  failureResult.carry = true;
  failureResult.valueOrError = (dword)hFile;
  return failureResult;
}


/* Address: 0x005763C0.
   Ownership: platform/filesystem/win32.
   Purpose: Filesystem service-table callback that normalizes a path, queries Win32 volume information, returns the
   captured volume serial number in EAX, and reports failure through CF.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
dword Win32Drive_GetVolumeSerialNumberCf(byte *outputLabel,char *path)

{
  BOOL volumeInformationQuerySucceeded;
  HANDLE hFile;
  dword volumeSerialNumber;
  Win32FileOpenEaxCf5 openResult;
  
  openResult = Win32File_OpenCf(0,(word *)path);
  hFile = (HANDLE)openResult.eax;
  if (!openResult.carry) {
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
  return (dword)hFile;
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
  dword driveTypeCode;
  
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
FileBufferEaxCf5 __thandor_eax_cf_preserve_ecx_edx FileSystem_LoadWholeFileCf(word *pathUtf16)

{
  void *handle;
  void *bytes;
  FileSystemOpenEaxCf5 openResult;
  FileSystemSizeEaxCf5 sizeResult;
  ArenaAllocEaxCf5 allocResult;
  FileSystemReadEaxCf5 readResult;
  FileBufferEaxCf5 failureResult;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             (word *)&g_ExecutableDirectoryUtf16);
  openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
  handle = (void *)openResult.eax;
  if (openResult.carry) {
    openResult = (*g_FileSystemOpenCf)(0,pathUtf16);
    handle = (void *)openResult.eax;
    if (openResult.carry) goto LAB_0040eff4;
  }
  sizeResult = (*g_FileSystemGetSizeCf)(handle);
  bytes = (void *)sizeResult.eax;
  if (!sizeResult.carry) {
    allocResult = (*g_MemoryApi.alloc)((dword)bytes);
    if (allocResult.carry) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)bytes,g_FatalErrorDetail1Utf16);
      bytes = (void *)0x5;
    }
    else {
      readResult = (*g_FileSystemReadExactCf)((FileIoByteCount)bytes,(void *)allocResult.eax,handle);
      bytes = (void *)readResult.eax;
      if (!readResult.carry) {
        (*g_FileSystemClose)(handle);
        return THANDOR_BITCAST(qword, FileBufferEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
      (*g_MemoryApi.free)((void *)allocResult.eax);
    }
  }
  (*g_FileSystemClose)(handle);
  handle = bytes;
LAB_0040eff4:
  failureResult.carry = true;
  failureResult.bufferOrError = handle;
  return failureResult;
}

/* Address: 0x0040F120.
   Ownership: platform/filesystem/win32.
   Purpose: Loads a whole file through the alternate recovered path and returns the carry/error contract.
*/
FileBufferEaxCf5 __thandor_eax_cf_preserve_edx
FileSystem_LoadWholeFileAlternatePathCf(word *pathUtf16)

{
  void *handle;
  void *bytes;
  FileSystemOpenEaxCf5 openResult;
  FileSystemSizeEaxCf5 sizeResult;
  ArenaAllocEaxCf5 allocResult;
  FileSystemReadEaxCf5 readResult;
  FileBufferEaxCf5 failureResult;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             (word *)&g_ExecutableDirectoryUtf16);
  openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
  handle = (void *)openResult.eax;
  if (openResult.carry) {
    openResult = (*g_FileSystemOpenCf)(0,pathUtf16);
    handle = (void *)openResult.eax;
    if (openResult.carry) goto LAB_0040f1c3;
  }
  sizeResult = (*g_FileSystemGetSizeCf)(handle);
  bytes = (void *)sizeResult.eax;
  if (!sizeResult.carry) {
    allocResult = (*g_MemoryApi.alloc)((dword)bytes);
    if (allocResult.carry) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)bytes,g_FatalErrorDetail1Utf16);
      bytes = (void *)0x5;
    }
    else {
      readResult = (*g_FileSystemReadExactCf)((FileIoByteCount)bytes,(void *)allocResult.eax,handle);
      bytes = (void *)readResult.eax;
      if (!readResult.carry) {
        (*g_FileSystemClose)(handle);
        return THANDOR_BITCAST(qword, FileBufferEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
      (*g_MemoryApi.free)((void *)allocResult.eax);
    }
  }
  (*g_FileSystemClose)(handle);
  handle = bytes;
LAB_0040f1c3:
  failureResult.carry = true;
  failureResult.bufferOrError = handle;
  return failureResult;
}

/* Address: 0x0040F1F0.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a UTF-16 path with engine mode 3, writes exactly byteCount bytes, and closes the handle. On write
   failure it closes and deletes the partial file. CF clear returns EAX zero; CF set preserves the backend error.
*/
StatusValueEaxCf5 FileSystem_WriteBufferToPathCf(FileIoByteCount byteCount,void *source,word *path)

{
  void *handle;
  void *writeFailureStatusCode;
  FileSystemOpenEaxCf5 openResult;
  FileSystemWriteEaxCf5 writeResult;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  openResult = (*g_FileSystemOpenCf)
                    (FILESYSTEM_OPEN_EXCLUSIVE_SHARE|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE,path);
  handle = (void *)openResult.eax;
  if (!openResult.carry) {
    writeResult = (*g_FileSystemWriteExactOrFlushCf)(byteCount,source,handle);
    writeFailureStatusCode = (void *)writeResult.eax;
    if (!writeResult.carry) {
      (*g_FileSystemClose)(handle);
      successResult.valueOrError = 0;
      successResult.carry = false;
      return successResult;
    }
    (*g_FileSystemClose)(handle);
    handle = writeFailureStatusCode;
    (*g_FileSystemDeleteCf)(1,path);
  }
  failureResult.carry = true;
  failureResult.valueOrError = (dword)handle;
  return failureResult;
}


/* Address: 0x00576070.
   Ownership: platform/filesystem/win32.
   Purpose: Writes exactly byteCount bytes, or flushes the handle when byteCount is zero. CF set returns engine
   error 7 or 8.
*/
Win32FileWriteEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_WriteExactOrFlushCf(FileIoByteCount byteCount,void *source,void *handle)

{
  BOOL operationSucceeded;
  dword writeCompletionStatusCode;
  BOOL setEndOfFileSucceeded;
  Win32FileWriteEaxCf5 successResult;
  Win32FileWriteEaxCf5 flushResult;
  Win32FileWriteEaxCf5 failureResult;
  
  g_Win32FileBytesTransferred = 0;
  if (byteCount == 0) {
    setEndOfFileSucceeded = SetEndOfFile(handle);
    flushResult.carry = false;
    flushResult.eax = setEndOfFileSucceeded;
    return flushResult;
  }
  operationSucceeded =
       WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  writeCompletionStatusCode = 8;
  if ((operationSucceeded != 0) &&
     (writeCompletionStatusCode = 7, byteCount == g_Win32FileBytesTransferred)) {
    successResult.eax = 7;
    successResult.carry = false;
    return successResult;
  }
  failureResult.carry = true;
  failureResult.eax = writeCompletionStatusCode;
  return failureResult;
}


/* Address: 0x00576140.
   Ownership: platform/filesystem/win32.
   Purpose: Queries the current file position with SetFilePointer(FILE_CURRENT). CF reports failure.
*/
dword Win32File_GetPositionCf(void *handle)

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
Win32FileSeekEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_SeekCf(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle)

{
  DWORD newFilePosition;
  Win32FileSeekEaxCf5 successResult;
  Win32FileSeekEaxCf5 failureResult;
  
  newFilePosition = SetFilePointer(handle,distance,(PLONG)0x0,moveMethod);
  if (newFilePosition != 0xffffffff) {
    successResult.carry = false;
    successResult.eax = newFilePosition;
    return successResult;
  }
  failureResult.carry = true;
  failureResult.eax = 9;
  return failureResult;
}


/* Address: 0x005761C0.
   Ownership: platform/filesystem/win32.
   Purpose: Converts and deletes one UTF-16 path. The first argument is an unused backend flags slot; CF set
   returns error 1.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
dword Win32File_DeleteCf(dword unusedFlags,word *path)

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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_MoveCf(word *destinationPath,word *sourcePath)

{
  BOOL operationSucceeded;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = MoveFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB);
  if (operationSucceeded != 0) {
    successResult.carry = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.carry = true;
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_CopyCf(word *destinationPath,word *sourcePath)

{
  BOOL operationSucceeded;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = CopyFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB,1);
  if (operationSucceeded != 0) {
    successResult.carry = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.carry = true;
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_CreateDirectoryRecursiveCf(FileSystemCreateDirectoryFlags flags,word *path)

{
  uint createSucceeded;
  StatusValueEaxCf5 parentOrSuccessResult;
  StatusValueEaxCf5 failureResult;
  word parentPath [256];
  word leafName [248];
  undefined4 returnAddressSlot;
  
  Package_SetLastErrorPath(path);
  returnAddressSlot = 0x576502;
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  createSucceeded = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
  if (createSucceeded == 0) {
    if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
      WidePath_SplitParentAndLeaf(leafName,parentPath,path);
      parentOrSuccessResult = Win32File_CreateDirectoryRecursiveCf(flags,parentPath);
      if (!parentOrSuccessResult.carry) {
        RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
        createSucceeded = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
        if (createSucceeded != 0)
        goto 
        Win32File_CreateDirectoryRecursiveCf_ReturnSuccessWithCarryClearAfterDirectOrRecursiveCreate
        ;
      }
    }
    Package_SetLastErrorPath(path);
    failureResult.carry = true;
    failureResult.valueOrError = 8;
    return failureResult;
  }
Win32File_CreateDirectoryRecursiveCf_ReturnSuccessWithCarryClearAfterDirectOrRecursiveCreate:
  parentOrSuccessResult.carry = false;
  parentOrSuccessResult.valueOrError = createSucceeded;
  return parentOrSuccessResult;
}


/* Address: 0x005765A0.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls RemoveDirectoryA. Error 11 is returned with CF set.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_RemoveDirectoryCf(word *path)

{
  BOOL operationSucceeded;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = RemoveDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    successResult.carry = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.carry = true;
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
  
  g_Win32DriveRootPathScratchA = (undefined1)driveLetter;
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
  return CONCAT44(totalBytes,freeBytes);
}

/* Address: 0x005766B0.
   Ownership: platform/filesystem/win32.
   Purpose: Writes uppercase letters for every bit returned by GetLogicalDrives and returns the number of letters
   written.
*/
DriveLetterEnumerationEaxEcx8 __thandor_eax_ecx_preserve_edx
Win32Drive_EnumerateLetters(byte *lettersOut)

{
  uint logicalDriveMask;
  dword enumeratedDriveCount;
  byte currentDriveLetter;
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
Win32Path_ValidateDos83Cf(FileSystemDos83ValidationFlags flags,byte *pathAnsi)

{
  byte pathChar;
  int charsRemaining;
  byte *previousCursor;
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
FileSystemEnumerationEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf
          (FileSystemEnumerationMode mode,dword reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,byte *outputRecords,
          byte *pathOrVolumeText)

{
  HANDLE hFindFile;
  BOOL apiSucceeded;
  uint recordCount;
  int comparisonsRemaining;
  int dwordsRemaining;
  dword *rightRecordDwords;
  dword *copySource;
  word *destination;
  dword *leftRecordDwords;
  dword *copyDestination;
  FileSystemEnumerationEaxEcxCf9 enumerationResult;
  FileSystemEnumerationEaxEcxCf9 volumeResult;
  CompareFlagsCfZf2 compareFlags;
  int passesRemaining;
  
  if (mode == FILESYSTEM_ENUMERATE_VOLUME_LABEL) {
    g_Win32DriveRootPathScratchA = *pathOrVolumeText;
    apiSucceeded = GetVolumeInformationA
                      ((LPCSTR)&g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratchA,0x80,
                       (LPDWORD)0x0,(LPDWORD)0x0,(LPDWORD)0x0,(LPSTR)0x0,0);
    if (apiSucceeded == 0) goto LAB_00576af2;
    recordCount = 0;
    if (0xff < outputCapacityBytes) {
      Text_CopyNarrowToUtf16Cf(0x200,(word *)outputRecords,g_Win32PathScratchA);
      volumeResult.entryCount = 1;
      volumeResult.recordSizeBytes = 0x200;
      volumeResult.carry = false;
      return volumeResult;
    }
  }
  else {
    Package_SetLastErrorPath((word *)pathOrVolumeText);
    RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,(word *)pathOrVolumeText);
    hFindFile = FindFirstFileA((LPCSTR)g_Win32PathScratchA,
                               (LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    if (hFindFile == (HANDLE)0xffffffff) {
LAB_00576af2:
      return THANDOR_BITCAST(unkuint9, FileSystemEnumerationEaxEcxCf9, (unkuint9)0x200);
    }
    recordCount = 0;
    destination = (word *)outputRecords;
    do {
      if (mode == FILESYSTEM_ENUMERATE_FILES) {
        if ((g_Win32FileCreationTimeOrDosDateScratch & 0x18) == 0)
        goto Win32FileSystem_AppendCurrentFindEntry;
      }
      else if (((mode == FILESYSTEM_ENUMERATE_DIRECTORIES) &&
               ((g_Win32FileCreationTimeOrDosDateScratch & 0x10) != 0)) &&
              ((g_Win32FindDataFileNameA != '.' ||
               ((g_Win32FindDataFileNameSecondCharA != '\0' &&
                ((g_Win32FindDataFileNameSecondCharA != '.' ||
                 (g_Win32FindDataFileNameThirdCharA != '\0')))))))) {
Win32FileSystem_AppendCurrentFindEntry:
        if (0x1ff < outputCapacityBytes) {
          Text_CopyNarrowToUtf16Cf(0x200,destination,(byte *)&g_Win32FindDataFileNameA);
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
      rightRecordDwords = (dword *)(outputRecords + 0x200);
      leftRecordDwords = (dword *)outputRecords;
      passesRemaining = comparisonsRemaining;
      do {
        do {
          compareFlags = Utf16String_CompareAsciiCaseInsensitiveFlags
                            ((word *)rightRecordDwords,(word *)leftRecordDwords);
          if (!compareFlags.carry && !compareFlags.zero) {
            copySource = rightRecordDwords;
            copyDestination = (dword *)g_Win32PathScratchA;
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
            copySource = (dword *)g_Win32PathScratchA;
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
        rightRecordDwords = (dword *)(outputRecords + 0x200);
        leftRecordDwords = (dword *)outputRecords;
        passesRemaining = comparisonsRemaining;
      } while (comparisonsRemaining != 0);
    }
  }
  enumerationResult.entryCount = recordCount;
  enumerationResult.recordSizeBytes = 0x200;
  enumerationResult.carry = false;
  return enumerationResult;
}


/* Address: 0x00576020.
   Ownership: platform/filesystem/win32.
   Purpose: Reads exactly byteCount bytes. CF clear means the requested count was transferred; CF set returns
   engine error 6.
*/
Win32FileReadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_ReadExactCf(FileIoByteCount byteCount,void *destination,void *handle)

{
  Win32FileReadEaxCf5 successResult;
  Win32FileReadEaxCf5 failureResult;
  
  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  if (g_Win32FileBytesTransferred == byteCount) {
    successResult.carry = false;
    successResult.eax = g_Win32FileBytesTransferred;
    return successResult;
  }
  failureResult.carry = true;
  failureResult.eax = 6;
  return failureResult;
}


/* Address: 0x00576100.
   Ownership: platform/filesystem/win32.
   Purpose: Returns the low 32-bit file size with CF clear. GetFileSize failure returns zero with CF set.
*/
Win32FileSizeEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetSizeCf(void *handle)

{
  DWORD fileSize;
  Win32FileSizeEaxCf5 successResult;
  Win32FileSizeEaxCf5 failureResult;
  
  fileSize = GetFileSize(handle,(LPDWORD)0x0);
  if (fileSize != 0xffffffff) {
    successResult.carry = false;
    successResult.eax = fileSize;
    return successResult;
  }
  failureResult.eax = 0;
  failureResult.carry = true;
  return failureResult;
}


/* Address: 0x00576430.
   Ownership: platform/filesystem/win32.
   Purpose: Gets the current ANSI directory and converts it into a 0x200-byte UTF-16 destination. CF reports Win32
   failure.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_GetCurrentDirectoryCf(word *destination)

{
  DWORD narrowPathLength;
  int copiedPathByteLength;
  StatusValueEaxCf5 copyResult;
  
  narrowPathLength = GetCurrentDirectoryA(0xff,(LPSTR)g_Win32PathScratchA);
  if (narrowPathLength != 0) {
    copyResult = Text_CopyNarrowToUtf16Cf(0x200,destination,g_Win32PathScratchA);
    return THANDOR_BITCAST(qword, StatusValueEaxCf5, ((THANDOR_BITCAST(StatusValueEaxCf5, qword, copyResult) & 0xFFFFFFFFFFull) & 0xffffffff));
  }
  destination[0] = 0;
  destination[1] = 0;
  copyResult.valueOrError = 0;
  copyResult.carry = true;
  return copyResult;
}


/* Address: 0x00576490.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls SetCurrentDirectoryA. Error 10 is returned with CF set.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_SetCurrentDirectoryCf(word *path)

{
  BOOL operationSucceeded;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = SetCurrentDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    successResult.carry = false;
    successResult.valueOrError = operationSucceeded;
    return successResult;
  }
  failureResult.carry = true;
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
  
  g_Win32DriveRootPathScratchA = (undefined1)driveLetter;
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
Win32FileOpenEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_OpenCf(FileSystemOpenFlags openFlags,word *path)

{
  HANDLE fileHandle;
  Win32FileOpenEaxCf5 successResult;
  Win32FileOpenEaxCf5 failureResult;
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
    successResult.carry = false;
    successResult.eax = (dword)fileHandle;
    return successResult;
  }
  failureResult.carry = true;
  failureResult.eax = 1;
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

