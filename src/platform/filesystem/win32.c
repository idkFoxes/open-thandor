/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/filesystem/win32.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/filesystem/win32.h>

/* Implementation ownership: platform/filesystem/win32. */

/* Address: 0x0040F430.
   Ownership: platform/filesystem/win32.
   Purpose: Builds the file-enumeration string table with the recovered carry/error result.
*/
FileSystemStringTableEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
FileSystem_BuildEnumerationStringTableCf
          (FileSystemEnumerationMode enumerationMode,dword reserved,byte *pathOrVolumeText)

{
  short sVar1;
  byte *outputRecords;
  byte *pbVar2;
  byte *memory;
  FileSystemOutputCapacityBytes FVar3;
  FileSystemOutputCapacityBytes outputCapacityBytes;
  FileSystemOutputCapacityBytes FVar4;
  byte *pbVar5;
  byte *pbVar6;
  byte *pbVar7;
  byte *pbVar8;
  bool bVar9;
  ArenaShrinkEaxCf5 AVar10;
  ArenaFreeEaxCf5 AVar11;
  ArenaLargestAllocationEaxEcxCf9 AVar12;
  FileSystemEnumerationEaxEcxCf9 FVar13;
  FileSystemStringTableEaxEcxCf9 FVar14;
  FileSystemStringTableEaxEcxCf9 FVar15;
  
  AVar12 = (*g_MemoryApi.allocLargestFreeBlock)();
  outputCapacityBytes = AVar12.blockSizeOrSentinel;
  outputRecords = (byte *)AVar12.allocationOrError;
  if (!AVar12.carry) {
    FVar13 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                       (enumerationMode,reserved,outputCapacityBytes,outputRecords,pathOrVolumeText)
    ;
    FVar3 = FVar13.entryCount;
    pbVar2 = (byte *)FVar13.recordSizeBytes;
    memory = pbVar2;
    outputCapacityBytes = FVar3;
    if (!FVar13.carry) {
      if (FVar3 == 0) {
        (*g_MemoryApi.free)(outputRecords);
        return (FileSystemStringTableEaxEcxCf9)(unkuint9)0;
      }
      outputCapacityBytes = FVar3 * (int)pbVar2;
      AVar10 = (*g_MemoryApi.shrinkInPlace)(outputCapacityBytes,outputRecords);
      memory = (byte *)AVar10.scratchOrError;
      if (!AVar10.carry) {
        AVar12 = (*g_MemoryApi.allocLargestFreeBlock)();
        outputCapacityBytes = AVar12.blockSizeOrSentinel;
        memory = (byte *)AVar12.allocationOrError;
        if (!AVar12.carry) {
          pbVar8 = memory + FVar3 * 4;
          bVar9 = FVar3 * 4 <= outputCapacityBytes;
          outputCapacityBytes = outputCapacityBytes + FVar3 * -4;
          FVar4 = FVar3;
          pbVar5 = memory;
          pbVar7 = outputRecords;
          if (bVar9 && outputCapacityBytes != 0) {
            do {
              *(byte **)pbVar5 = pbVar8;
              pbVar6 = pbVar7;
              do {
                sVar1 = *(short *)pbVar6;
                *(short *)pbVar8 = sVar1;
                pbVar6 = pbVar6 + 2;
                pbVar8 = pbVar8 + 2;
                bVar9 = outputCapacityBytes < 2;
                outputCapacityBytes = outputCapacityBytes - 2;
                if (bVar9 || outputCapacityBytes == 0) goto LAB_0040f509;
              } while (sVar1 != 0);
              pbVar5 = pbVar5 + 4;
              pbVar7 = pbVar7 + (int)pbVar2;
              FVar4 = FVar4 - 1;
              if (FVar4 == 0) {
                (*g_MemoryApi.shrinkInPlace)((int)pbVar8 - (int)memory,memory);
                (*g_MemoryApi.free)(outputRecords);
                FVar14.entryCountOrScratch = FVar3;
                FVar14.tableOrError = (dword)memory;
                FVar14.carry = false;
                return FVar14;
              }
            } while( true );
          }
LAB_0040f509:
          AVar11 = (*g_MemoryApi.free)(memory);
          memory = (byte *)AVar11.eax;
        }
      }
    }
    (*g_MemoryApi.free)(outputRecords);
    outputRecords = memory;
  }
  FVar15.entryCountOrScratch = outputCapacityBytes;
  FVar15.tableOrError = (dword)outputRecords;
  FVar15.carry = true;
  return FVar15;
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
  byte bVar1;
  dword dVar2;
  byte *pbVar3;
  BOOL BVar4;
  void *handle;
  int iVar5;
  ArenaPayloadByteCount bytes;
  byte *pbVar6;
  word *pwVar7;
  ArenaAllocEaxCf5 AVar8;
  Win32FileOpenEaxCf5 WVar9;
  Win32FileSizeEaxCf5 WVar10;
  Win32FileReadEaxCf5 WVar11;
  StatusValueEaxCf5 SVar12;
  
  pbVar3 = (byte *)GetCommandLineA();
  pbVar6 = g_Win32PathScratchA;
  if (*pbVar3 == 0x22) {
    do {
      pbVar3 = pbVar3 + 1;
      bVar1 = *pbVar3;
      *pbVar6 = bVar1;
      pbVar6 = pbVar6 + 1;
    } while (bVar1 != 0x22);
  }
  else {
    do {
      bVar1 = *pbVar3;
      *pbVar6 = bVar1;
      pbVar3 = pbVar3 + 1;
      pbVar6 = pbVar6 + 1;
      if (bVar1 == 0) break;
    } while (bVar1 != 0x20);
  }
  pbVar6[-1] = 0;
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
  BVar4 = GetComputerNameA((LPSTR)g_Win32PathScratchA,
                           (LPDWORD)&g_FileSystemInitComputerNameCapacityOrConfigCursor);
  if (BVar4 != 0) {
    pwVar7 = g_DefaultComputerLabelUtf16;
    for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
      pwVar7[0] = 0;
      pwVar7[1] = 0;
      pwVar7 = pwVar7 + 2;
    }
    Text_CopyNarrowToUtf16Cf(0x40,g_DefaultComputerLabelUtf16,g_Win32PathScratchA);
  }
  AVar8 = ArenaHeap_Alloc(0x800000);
  if (AVar8.carry) {
                    // WARNING: Subroutine does not return
    FatalError_Exit(0x407d40,true);
  }
  g_PackageScratchBuffer = (byte *)AVar8.eax;
  WVar9 = Win32File_OpenCf(0,(word *)u_THANDOR_cfg_0040e23d);
  handle = (void *)WVar9.eax;
  if (WVar9.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,(word *)u_THANDOR_cfg_0040e23d,
               (word *)&g_ExecutableDirectoryUtf16);
    WVar9 = Win32File_OpenCf(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    handle = (void *)WVar9.eax;
    if (WVar9.carry) goto FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage;
  }
  WVar10 = Win32File_GetSizeCf(handle);
  bytes = WVar10.eax;
  if ((!WVar10.carry) && (bytes != 0)) {
    AVar8 = ArenaHeap_Alloc(bytes);
    pbVar3 = (byte *)AVar8.eax;
    if (!AVar8.carry) {
      WVar11 = Win32File_ReadExactCf(bytes,pbVar3,handle);
      pbVar6 = pbVar3;
      dVar2 = bytes;
      if (WVar11.carry) {
        ArenaHeap_Free(pbVar3);
      }
      else {
        do {
          while( true ) {
            g_FileSystemConfigRemainingBytes = dVar2;
            g_FileSystemInitComputerNameCapacityOrConfigCursor = pbVar6;
            bVar1 = *pbVar3;
            if (0x20 < bVar1) break;
FileSystemConfig_TerminateSeparatorOrComment:
            *pbVar3 = 0;
            pbVar3 = pbVar3 + 1;
            bytes = bytes - 1;
            pbVar6 = g_FileSystemInitComputerNameCapacityOrConfigCursor;
            dVar2 = g_FileSystemConfigRemainingBytes;
            if (bytes == 0) goto FileSystemConfig_CloseInput;
          }
          if (bVar1 == 0x5b) {
            do {
              *pbVar3 = 0;
              pbVar3 = pbVar3 + 1;
              bytes = bytes - 1;
              if (bytes == 0) goto FileSystemConfig_CloseInput;
            } while (*pbVar3 != 0x5d);
            goto FileSystemConfig_TerminateSeparatorOrComment;
          }
          *pbVar3 = (&g_FileSystemConfigCharacterNormalizationMap)[bVar1];
          pbVar3 = pbVar3 + 1;
          bytes = bytes - 1;
          pbVar6 = g_FileSystemInitComputerNameCapacityOrConfigCursor;
          dVar2 = g_FileSystemConfigRemainingBytes;
        } while (bytes != 0);
      }
    }
  }
FileSystemConfig_CloseInput:
  Win32File_Close(handle);
FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage:
  Win32File_GetCurrentDirectoryCf(g_InitialWorkingDirectory.codeUnits);
  SVar12 = Package_MountLowPriority((word *)u_engine_pck_0040e255);
  if (!SVar12.carry) {
    g_EnginePackageLowPriorityMountHandle = SVar12.valueOrError;
  }
  return SVar12.valueOrError;
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
  Win32FileOpenEaxCf5 WVar1;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  WVar1 = Win32File_OpenCf(0,path);
  hFile = (HANDLE)WVar1.eax;
  if ((!WVar1.carry) && (hFile != (HANDLE)0xffffffff)) {
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
      SVar2.carry = false;
      SVar2.valueOrError = g_Win32FileCreationTimeOrDosDateScratch;
      return SVar2;
    }
  }
  SVar3.carry = true;
  SVar3.valueOrError = (dword)hFile;
  return SVar3;
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
  Win32FileOpenEaxCf5 WVar1;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  WVar1 = Win32File_OpenCf(0,path);
  hFile = (HANDLE)WVar1.eax;
  if (!WVar1.carry) {
    fileTimeQuerySucceeded =
         GetFileTime(hFile,(LPFILETIME)0x0,(LPFILETIME)0x0,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(hFile);
    hFile = (HANDLE)0x1;
    if (fileTimeQuerySucceeded != 0) {
      SVar2.carry = false;
      SVar2.valueOrError = g_Win32FileLastWriteTimeHighScratch;
      return SVar2;
    }
  }
  SVar3.carry = true;
  SVar3.valueOrError = (dword)hFile;
  return SVar3;
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
  Win32FileOpenEaxCf5 WVar1;
  
  WVar1 = Win32File_OpenCf(0,(word *)path);
  hFile = (HANDLE)WVar1.eax;
  if (!WVar1.carry) {
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
  FileSystemOpenEaxCf5 FVar1;
  FileSystemSizeEaxCf5 FVar2;
  ArenaAllocEaxCf5 AVar3;
  FileSystemReadEaxCf5 FVar4;
  FileBufferEaxCf5 FVar5;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             (word *)&g_ExecutableDirectoryUtf16);
  FVar1 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
  handle = (void *)FVar1.eax;
  if (FVar1.carry) {
    FVar1 = (*g_FileSystemOpenCf)(0,pathUtf16);
    handle = (void *)FVar1.eax;
    if (FVar1.carry) goto LAB_0040eff4;
  }
  FVar2 = (*g_FileSystemGetSizeCf)(handle);
  bytes = (void *)FVar2.eax;
  if (!FVar2.carry) {
    AVar3 = (*g_MemoryApi.alloc)((dword)bytes);
    if (AVar3.carry) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)bytes,g_FatalErrorDetail1Utf16);
      bytes = (void *)0x5;
    }
    else {
      FVar4 = (*g_FileSystemReadExactCf)((FileIoByteCount)bytes,(void *)AVar3.eax,handle);
      bytes = (void *)FVar4.eax;
      if (!FVar4.carry) {
        (*g_FileSystemClose)(handle);
        return (FileBufferEaxCf5)((uint5)AVar3 & 0xffffffff);
      }
      (*g_MemoryApi.free)((void *)AVar3.eax);
    }
  }
  (*g_FileSystemClose)(handle);
  handle = bytes;
LAB_0040eff4:
  FVar5.carry = true;
  FVar5.bufferOrError = handle;
  return FVar5;
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
  FileSystemOpenEaxCf5 FVar1;
  FileSystemSizeEaxCf5 FVar2;
  ArenaAllocEaxCf5 AVar3;
  FileSystemReadEaxCf5 FVar4;
  FileBufferEaxCf5 FVar5;
  
  WidePath_CombineDirectoryAndLeaf
            ((word *)&g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             (word *)&g_ExecutableDirectoryUtf16);
  FVar1 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
  handle = (void *)FVar1.eax;
  if (FVar1.carry) {
    FVar1 = (*g_FileSystemOpenCf)(0,pathUtf16);
    handle = (void *)FVar1.eax;
    if (FVar1.carry) goto LAB_0040f1c3;
  }
  FVar2 = (*g_FileSystemGetSizeCf)(handle);
  bytes = (void *)FVar2.eax;
  if (!FVar2.carry) {
    AVar3 = (*g_MemoryApi.alloc)((dword)bytes);
    if (AVar3.carry) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)bytes,g_FatalErrorDetail1Utf16);
      bytes = (void *)0x5;
    }
    else {
      FVar4 = (*g_FileSystemReadExactCf)((FileIoByteCount)bytes,(void *)AVar3.eax,handle);
      bytes = (void *)FVar4.eax;
      if (!FVar4.carry) {
        (*g_FileSystemClose)(handle);
        return (FileBufferEaxCf5)((uint5)AVar3 & 0xffffffff);
      }
      (*g_MemoryApi.free)((void *)AVar3.eax);
    }
  }
  (*g_FileSystemClose)(handle);
  handle = bytes;
LAB_0040f1c3:
  FVar5.carry = true;
  FVar5.bufferOrError = handle;
  return FVar5;
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
  FileSystemOpenEaxCf5 FVar1;
  FileSystemWriteEaxCf5 FVar2;
  StatusValueEaxCf5 SVar3;
  StatusValueEaxCf5 SVar4;
  
  FVar1 = (*g_FileSystemOpenCf)
                    (FILESYSTEM_OPEN_EXCLUSIVE_SHARE|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE,path);
  handle = (void *)FVar1.eax;
  if (!FVar1.carry) {
    FVar2 = (*g_FileSystemWriteExactOrFlushCf)(byteCount,source,handle);
    writeFailureStatusCode = (void *)FVar2.eax;
    if (!FVar2.carry) {
      (*g_FileSystemClose)(handle);
      SVar3.valueOrError = 0;
      SVar3.carry = false;
      return SVar3;
    }
    (*g_FileSystemClose)(handle);
    handle = writeFailureStatusCode;
    (*g_FileSystemDeleteCf)(1,path);
  }
  SVar4.carry = true;
  SVar4.valueOrError = (dword)handle;
  return SVar4;
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
  Win32FileWriteEaxCf5 WVar1;
  Win32FileWriteEaxCf5 WVar2;
  Win32FileWriteEaxCf5 WVar3;
  
  g_Win32FileBytesTransferred = 0;
  if (byteCount == 0) {
    setEndOfFileSucceeded = SetEndOfFile(handle);
    WVar2.carry = false;
    WVar2.eax = setEndOfFileSucceeded;
    return WVar2;
  }
  operationSucceeded =
       WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  writeCompletionStatusCode = 8;
  if ((operationSucceeded != 0) &&
     (writeCompletionStatusCode = 7, byteCount == g_Win32FileBytesTransferred)) {
    WVar1.eax = 7;
    WVar1.carry = false;
    return WVar1;
  }
  WVar3.carry = true;
  WVar3.eax = writeCompletionStatusCode;
  return WVar3;
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
  Win32FileSeekEaxCf5 WVar1;
  Win32FileSeekEaxCf5 WVar2;
  
  newFilePosition = SetFilePointer(handle,distance,(PLONG)0x0,moveMethod);
  if (newFilePosition != 0xffffffff) {
    WVar1.carry = false;
    WVar1.eax = newFilePosition;
    return WVar1;
  }
  WVar2.carry = true;
  WVar2.eax = 9;
  return WVar2;
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
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = MoveFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB);
  if (operationSucceeded != 0) {
    SVar1.carry = false;
    SVar1.valueOrError = operationSucceeded;
    return SVar1;
  }
  SVar2.carry = true;
  SVar2.valueOrError = 1;
  return SVar2;
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
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = CopyFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB,1);
  if (operationSucceeded != 0) {
    SVar1.carry = false;
    SVar1.valueOrError = operationSucceeded;
    return SVar1;
  }
  SVar2.carry = true;
  SVar2.valueOrError = 1;
  return SVar2;
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
  uint uVar1;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  word awStack_418 [256];
  word local_218 [248];
  undefined4 uStack_28;
  
  Package_SetLastErrorPath(path);
  uStack_28 = 0x576502;
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  uVar1 = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
  if (uVar1 == 0) {
    if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
      WidePath_SplitParentAndLeaf(local_218,awStack_418,path);
      SVar2 = Win32File_CreateDirectoryRecursiveCf(flags,awStack_418);
      if (!SVar2.carry) {
        RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
        uVar1 = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
        if (uVar1 != 0)
        goto 
        Win32File_CreateDirectoryRecursiveCf_ReturnSuccessWithCarryClearAfterDirectOrRecursiveCreate
        ;
      }
    }
    Package_SetLastErrorPath(path);
    SVar3.carry = true;
    SVar3.valueOrError = 8;
    return SVar3;
  }
Win32File_CreateDirectoryRecursiveCf_ReturnSuccessWithCarryClearAfterDirectOrRecursiveCreate:
  SVar2.carry = false;
  SVar2.valueOrError = uVar1;
  return SVar2;
}


/* Address: 0x005765A0.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls RemoveDirectoryA. Error 11 is returned with CF set.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_RemoveDirectoryCf(word *path)

{
  BOOL operationSucceeded;
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = RemoveDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    SVar1.carry = false;
    SVar1.valueOrError = operationSucceeded;
    return SVar1;
  }
  SVar2.carry = true;
  SVar2.valueOrError = 0xb;
  return SVar2;
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
  DriveLetterEnumerationEaxEcx8 DVar1;
  
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
  DVar1.driveCountMirror = enumeratedDriveCount;
  DVar1.driveCount = enumeratedDriveCount;
  return DVar1;
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
  byte bVar1;
  int iVar2;
  byte *pbVar3;
  bool bVar4;
  
  if ((flags & FILESYSTEM_DOS83_COMPONENT_ONLY) == 0) {
    if (pathAnsi[1] == 0x3a) {
      bVar1 = *pathAnsi;
      if (bVar1 < 0x41) {
        return true;
      }
      if (0x7a < bVar1) {
        return true;
      }
      if ((bVar1 < 0x61) && (0x5a < bVar1)) {
        return true;
      }
      pathAnsi = pathAnsi + 2;
    }
    if (*pathAnsi == 0x5c) {
      pathAnsi = pathAnsi + 1;
    }
    while (bVar4 = Win32Path_ValidateDos83Cf
                             (flags | (FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION|
                                      FILESYSTEM_DOS83_COMPONENT_ONLY),pathAnsi), !bVar4) {
      while( true ) {
        bVar1 = *pathAnsi;
        pathAnsi = pathAnsi + 1;
        if (bVar1 == 0x5c) break;
        if (bVar1 == 0) {
          return false;
        }
      }
    }
  }
  else {
    iVar2 = 8;
    do {
      bVar1 = *pathAnsi;
      if (bVar1 == 0) break;
      if (bVar1 == 0x2a) {
        pathAnsi = pathAnsi + 1;
        if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
          return true;
        }
        break;
      }
      if (0x2c < bVar1) {
        if (bVar1 < 0x2f) break;
        if (bVar1 == 0x2f) {
          return true;
        }
        if (0x39 < bVar1) {
          if (bVar1 == 0x3f) {
            if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
              return true;
            }
          }
          else {
            if (bVar1 < 0x41) {
              return true;
            }
            if (0x5a < bVar1) {
              if (bVar1 == 0x5c) break;
              if (bVar1 < 0x61) {
                return true;
              }
              if (0x7a < bVar1) {
                return true;
              }
            }
          }
        }
      }
      pathAnsi = pathAnsi + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (iVar2 != 8) {
      bVar1 = *pathAnsi;
      iVar2 = 3;
      if (bVar1 != 0) {
        if (bVar1 == 0x2e) {
          do {
            pbVar3 = pathAnsi;
            pathAnsi = pbVar3 + 1;
            bVar1 = *pathAnsi;
            if (bVar1 == 0) {
              return false;
            }
            if (bVar1 == 0x2a) {
              if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                return true;
              }
              break;
            }
            if (0x2c < bVar1) {
              if (bVar1 < 0x30) {
                return true;
              }
              if (0x39 < bVar1) {
                if (bVar1 == 0x3f) {
                  if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                    return true;
                  }
                }
                else {
                  if (bVar1 < 0x41) {
                    return true;
                  }
                  if (0x5a < bVar1) {
                    if (bVar1 == 0x5c) {
                      return false;
                    }
                    if (bVar1 < 0x61) {
                      return true;
                    }
                    if (0x7a < bVar1) {
                      return true;
                    }
                  }
                }
              }
            }
            iVar2 = iVar2 + -1;
          } while (iVar2 != 0);
          bVar1 = pbVar3[2];
          if (bVar1 == 0) {
            return false;
          }
        }
        if ((flags & FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION) == 0) {
          return true;
        }
        if (bVar1 != 0x5c) {
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
  BOOL BVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  dword *rightRecordDwords;
  dword *pdVar5;
  word *destination;
  dword *leftRecordDwords;
  dword *pdVar6;
  FileSystemEnumerationEaxEcxCf9 FVar7;
  FileSystemEnumerationEaxEcxCf9 FVar8;
  CompareFlagsCfZf2 CVar9;
  int iVar10;
  
  if (mode == FILESYSTEM_ENUMERATE_VOLUME_LABEL) {
    g_Win32DriveRootPathScratchA = *pathOrVolumeText;
    BVar1 = GetVolumeInformationA
                      ((LPCSTR)&g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratchA,0x80,
                       (LPDWORD)0x0,(LPDWORD)0x0,(LPDWORD)0x0,(LPSTR)0x0,0);
    if (BVar1 == 0) goto LAB_00576af2;
    uVar2 = 0;
    if (0xff < outputCapacityBytes) {
      Text_CopyNarrowToUtf16Cf(0x200,(word *)outputRecords,g_Win32PathScratchA);
      FVar8.entryCount = 1;
      FVar8.recordSizeBytes = 0x200;
      FVar8.carry = false;
      return FVar8;
    }
  }
  else {
    Package_SetLastErrorPath((word *)pathOrVolumeText);
    RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,(word *)pathOrVolumeText);
    hFindFile = FindFirstFileA((LPCSTR)g_Win32PathScratchA,
                               (LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    if (hFindFile == (HANDLE)0xffffffff) {
LAB_00576af2:
      return (FileSystemEnumerationEaxEcxCf9)(unkuint9)0x200;
    }
    uVar2 = 0;
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
          uVar2 = uVar2 + 1;
          outputCapacityBytes = outputCapacityBytes - 0x200;
        }
      }
      BVar1 = FindNextFileA(hFindFile,(LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    } while (BVar1 != 0);
    FindClose(hFindFile);
    if (1 < uVar2) {
      iVar3 = uVar2 - 1;
      rightRecordDwords = (dword *)(outputRecords + 0x200);
      leftRecordDwords = (dword *)outputRecords;
      iVar10 = iVar3;
      do {
        do {
          CVar9 = Utf16String_CompareAsciiCaseInsensitiveFlags
                            ((word *)rightRecordDwords,(word *)leftRecordDwords);
          if (!CVar9.carry && !CVar9.zero) {
            pdVar5 = rightRecordDwords;
            pdVar6 = (dword *)g_Win32PathScratchA;
            for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
              *pdVar6 = *pdVar5;
              pdVar5 = pdVar5 + 1;
              pdVar6 = pdVar6 + 1;
            }
            pdVar5 = leftRecordDwords;
            pdVar6 = rightRecordDwords;
            for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
              *pdVar6 = *pdVar5;
              pdVar5 = pdVar5 + 1;
              pdVar6 = pdVar6 + 1;
            }
            pdVar5 = (dword *)g_Win32PathScratchA;
            pdVar6 = leftRecordDwords;
            for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
              *pdVar6 = *pdVar5;
              pdVar5 = pdVar5 + 1;
              pdVar6 = pdVar6 + 1;
            }
          }
          rightRecordDwords = rightRecordDwords + 0x80;
          leftRecordDwords = leftRecordDwords + 0x80;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        iVar3 = iVar10 + -1;
        rightRecordDwords = (dword *)(outputRecords + 0x200);
        leftRecordDwords = (dword *)outputRecords;
        iVar10 = iVar3;
      } while (iVar3 != 0);
    }
  }
  FVar7.entryCount = uVar2;
  FVar7.recordSizeBytes = 0x200;
  FVar7.carry = false;
  return FVar7;
}


/* Address: 0x00576020.
   Ownership: platform/filesystem/win32.
   Purpose: Reads exactly byteCount bytes. CF clear means the requested count was transferred; CF set returns
   engine error 6.
*/
Win32FileReadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_ReadExactCf(FileIoByteCount byteCount,void *destination,void *handle)

{
  Win32FileReadEaxCf5 WVar1;
  Win32FileReadEaxCf5 WVar2;
  
  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  if (g_Win32FileBytesTransferred == byteCount) {
    WVar1.carry = false;
    WVar1.eax = g_Win32FileBytesTransferred;
    return WVar1;
  }
  WVar2.carry = true;
  WVar2.eax = 6;
  return WVar2;
}


/* Address: 0x00576100.
   Ownership: platform/filesystem/win32.
   Purpose: Returns the low 32-bit file size with CF clear. GetFileSize failure returns zero with CF set.
*/
Win32FileSizeEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetSizeCf(void *handle)

{
  DWORD fileSize;
  Win32FileSizeEaxCf5 WVar1;
  Win32FileSizeEaxCf5 WVar2;
  
  fileSize = GetFileSize(handle,(LPDWORD)0x0);
  if (fileSize != 0xffffffff) {
    WVar1.carry = false;
    WVar1.eax = fileSize;
    return WVar1;
  }
  WVar2.eax = 0;
  WVar2.carry = true;
  return WVar2;
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
  StatusValueEaxCf5 SVar1;
  
  narrowPathLength = GetCurrentDirectoryA(0xff,(LPSTR)g_Win32PathScratchA);
  if (narrowPathLength != 0) {
    SVar1 = Text_CopyNarrowToUtf16Cf(0x200,destination,g_Win32PathScratchA);
    return (StatusValueEaxCf5)((uint5)SVar1 & 0xffffffff);
  }
  destination[0] = 0;
  destination[1] = 0;
  SVar1.valueOrError = 0;
  SVar1.carry = true;
  return SVar1;
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
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = SetCurrentDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    SVar1.carry = false;
    SVar1.valueOrError = operationSucceeded;
    return SVar1;
  }
  SVar2.carry = true;
  SVar2.valueOrError = 10;
  return SVar2;
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
  Win32FileOpenEaxCf5 WVar1;
  Win32FileOpenEaxCf5 WVar2;
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
    WVar1.carry = false;
    WVar1.eax = (dword)fileHandle;
    return WVar1;
  }
  WVar2.carry = true;
  WVar2.eax = 1;
  return WVar2;
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

