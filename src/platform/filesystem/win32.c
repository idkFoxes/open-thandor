#include <thandor/platform/filesystem/win32.h>

/* Implementation ownership: platform/filesystem/win32. */

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
  dword dVar5;
  int iVar6;
  FileIoByteCount byteCount;
  dword extraout_ECX;
  byte *pbVar7;
  word *pwVar8;
  undefined1 uVar9;
  
  pbVar3 = (byte *)GetCommandLineA();
  pbVar7 = g_Win32PathScratchA;
  if (*pbVar3 == 0x22) {
    do {
      pbVar3 = pbVar3 + 1;
      bVar1 = *pbVar3;
      *pbVar7 = bVar1;
      pbVar7 = pbVar7 + 1;
    } while (bVar1 != 0x22);
  }
  else {
    do {
      bVar1 = *pbVar3;
      *pbVar7 = bVar1;
      pbVar3 = pbVar3 + 1;
      pbVar7 = pbVar7 + 1;
      if (bVar1 == 0) break;
    } while (bVar1 != 0x20);
  }
  pbVar7[-1] = 0;
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
  uVar9 = 0;
  if (BVar4 != 0) {
    uVar9 = 0;
    pwVar8 = g_DefaultComputerLabelUtf16;
    for (iVar6 = 0x10; iVar6 != 0; iVar6 = iVar6 + -1) {
      pwVar8[0] = 0;
      pwVar8[1] = 0;
      pwVar8 = pwVar8 + 2;
    }
    Text_CopyNarrowToUtf16Cf(0x40,g_DefaultComputerLabelUtf16,g_Win32PathScratchA);
  }
  pbVar3 = ArenaHeap_Alloc(0x800000);
  if ((bool)uVar9) {
                    
    FatalError_Exit();
  }
  g_PackageScratchBuffer = pbVar3;
  handle = (void *)Win32File_OpenCf(0,(word *)u_THANDOR_cfg_0040e23d);
  if ((bool)uVar9) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,(word *)u_THANDOR_cfg_0040e23d,
               (word *)&g_ExecutableDirectoryUtf16);
    handle = (void *)Win32File_OpenCf(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    if ((bool)uVar9) goto FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage;
  }
  dVar5 = Win32File_GetSizeCf(handle);
  if (((!(bool)uVar9) && (uVar9 = false, dVar5 != 0)) &&
     (pbVar3 = ArenaHeap_Alloc(dVar5), !(bool)uVar9)) {
    Win32File_ReadExactCf(byteCount,pbVar3,handle);
    dVar5 = extraout_ECX;
    pbVar7 = pbVar3;
    dVar2 = extraout_ECX;
    if ((bool)uVar9) {
      ArenaHeap_Free(pbVar3);
    }
    else {
      do {
        while( true ) {
          g_FileSystemConfigRemainingBytes = dVar2;
          g_FileSystemInitComputerNameCapacityOrConfigCursor = pbVar7;
          bVar1 = *pbVar3;
          uVar9 = bVar1 < 0x20;
          if (0x20 < bVar1) break;
FileSystemConfig_TerminateSeparatorOrComment:
          *pbVar3 = 0;
          pbVar3 = pbVar3 + 1;
          dVar5 = dVar5 - 1;
          pbVar7 = g_FileSystemInitComputerNameCapacityOrConfigCursor;
          dVar2 = g_FileSystemConfigRemainingBytes;
          if (dVar5 == 0) goto FileSystemConfig_CloseInput;
        }
        uVar9 = bVar1 < 0x5b;
        if (bVar1 == 0x5b) {
          do {
            *pbVar3 = 0;
            pbVar3 = pbVar3 + 1;
            dVar5 = dVar5 - 1;
            if (dVar5 == 0) goto FileSystemConfig_CloseInput;
            uVar9 = *pbVar3 < 0x5d;
          } while (*pbVar3 != 0x5d);
          goto FileSystemConfig_TerminateSeparatorOrComment;
        }
        *pbVar3 = (&g_FileSystemConfigCharacterNormalizationMap)[bVar1];
        pbVar3 = pbVar3 + 1;
        dVar5 = dVar5 - 1;
        pbVar7 = g_FileSystemInitComputerNameCapacityOrConfigCursor;
        dVar2 = g_FileSystemConfigRemainingBytes;
      } while (dVar5 != 0);
    }
  }
FileSystemConfig_CloseInput:
  Win32File_Close(handle);
FileSystemConfig_CaptureWorkingDirectoryAndMountEnginePackage:
  Win32File_GetCurrentDirectoryCf(g_InitialWorkingDirectory.codeUnits);
  dVar5 = Package_MountLowPriority((word *)u_engine_pck_0040e255);
  if (!(bool)uVar9) {
    g_EnginePackageLowPriorityMountHandle = dVar5;
  }
  return dVar5;
}

/* Address: 0x005762F0.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a path, reads its last-write FILETIME, converts it to DOS date/time, and returns the packed DOS
   date with CF clear.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
undefined8 Win32File_GetLastWriteDosDateCf(word *path)

{
  HANDLE hFile;
  BOOL fileTimeQuerySucceeded;
  undefined4 in_EDX;
  undefined1 in_CF;
  
  hFile = (HANDLE)Win32File_OpenCf(0,path);
  if ((!(bool)in_CF) && (hFile != (HANDLE)0xffffffff)) {
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
      return CONCAT44(in_EDX,g_Win32FileCreationTimeOrDosDateScratch);
    }
  }
  return CONCAT44(in_EDX,hFile);
}

/* Address: 0x00576360.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a path and returns the high dword of its last-write FILETIME. CF reports open or GetFileTime
   failure.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
undefined8 Win32File_GetLastWriteTimeHighCf(word *path)

{
  HANDLE hFile;
  BOOL fileTimeQuerySucceeded;
  undefined4 in_EDX;
  undefined1 in_CF;
  
  hFile = (HANDLE)Win32File_OpenCf(0,path);
  if (!(bool)in_CF) {
    fileTimeQuerySucceeded =
         GetFileTime(hFile,(LPFILETIME)0x0,(LPFILETIME)0x0,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(hFile);
    hFile = (HANDLE)0x1;
    if (fileTimeQuerySucceeded != 0) {
      return CONCAT44(in_EDX,g_Win32FileLastWriteTimeHighScratch);
    }
  }
  return CONCAT44(in_EDX,hFile);
}

/* Address: 0x005763C0.
   Ownership: platform/filesystem/win32.
   Purpose: Filesystem service-table callback that normalizes a path, queries Win32 volume information, returns the
   captured volume serial number in EAX, and reports failure through CF.
   Local calls: Win32File_OpenCf, Win32File_Close.
*/
dword Win32Drive_GetVolumeSerialNumberCf(byte *outputLabel,char *path)

{
  HANDLE hFile;
  BOOL volumeInformationQuerySucceeded;
  dword volumeSerialNumber;
  undefined1 in_CF;
  
  hFile = (HANDLE)Win32File_OpenCf(0,(word *)path);
  if (!(bool)in_CF) {
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
undefined8 Win32Drive_CheckMediaReadyCf(DosDriveLetterCode32 driveLetter)

{
  undefined4 in_EAX;
  dword driveTypeCode;
  undefined4 in_EDX;
  
  driveTypeCode = Win32Drive_GetEngineTypeCode(driveLetter);
  if ((driveTypeCode != 0x28) && (driveTypeCode != 0x2b)) {
    return CONCAT44(in_EDX,in_EAX);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0040F1F0.
   Ownership: platform/filesystem/win32.
   Purpose: Opens a UTF-16 path with engine mode 3, writes exactly byteCount bytes, and closes the handle. On write
   failure it closes and deletes the partial file. CF clear returns EAX zero; CF set preserves the backend error.
*/
undefined4 FileSystem_WriteBufferToPathCf(FileIoByteCount byteCount,void *source,word *path)

{
  void *handle;
  undefined1 in_CF;
  
  handle = (void *)(*g_FileSystemOpenCf)
                             (FILESYSTEM_OPEN_EXCLUSIVE_SHARE|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE,
                              path);
  if (!(bool)in_CF) {
    (*g_FileSystemWriteExactOrFlushCf)(byteCount,source,handle);
    if (!(bool)in_CF) {
      (*g_FileSystemClose)(handle);
      return 0;
    }
    (*g_FileSystemClose)(handle);
    handle = (void *)0x1;
    (*g_FileSystemDeleteCf)(1,path);
  }
  return handle;
}

/* Address: 0x00576070.
   Ownership: platform/filesystem/win32.
   Purpose: Writes exactly byteCount bytes, or flushes the handle when byteCount is zero. CF set returns engine
   error 7 or 8.
*/
dword Win32File_WriteExactOrFlushCf(FileIoByteCount byteCount,void *source,void *handle)

{
  BOOL operationSucceeded;
  dword writeCompletionStatusCode;
  BOOL setEndOfFileSucceeded;
  
  g_Win32FileBytesTransferred = 0;
  if (byteCount == 0) {
    setEndOfFileSucceeded = SetEndOfFile(handle);
    return setEndOfFileSucceeded;
  }
  operationSucceeded =
       WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  writeCompletionStatusCode = 8;
  if ((operationSucceeded != 0) &&
     (writeCompletionStatusCode = 7, byteCount == g_Win32FileBytesTransferred)) {
    return writeCompletionStatusCode;
  }
  return writeCompletionStatusCode;
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
dword Win32File_SeekCf(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle)

{
  DWORD newFilePosition;
  
  newFilePosition = SetFilePointer(handle,distance,(PLONG)0x0,moveMethod);
  if (newFilePosition != 0xffffffff) {
    return newFilePosition;
  }
  return 9;
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
undefined8 Win32File_MoveCf(word *destinationPath,word *sourcePath)

{
  BOOL operationSucceeded;
  undefined4 in_EDX;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = MoveFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB);
  if (operationSucceeded != 0) {
    return CONCAT44(in_EDX,operationSucceeded);
  }
  return CONCAT44(in_EDX,1);
}

/* Address: 0x00576280.
   Ownership: platform/filesystem/win32.
   Purpose: Converts source and destination UTF-16 paths and calls CopyFileA with fail-if-exists enabled. CF set
   returns error 1.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
undefined8 Win32File_CopyCf(word *destinationPath,word *sourcePath)

{
  BOOL operationSucceeded;
  undefined4 in_EDX;
  
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchB,destinationPath);
  operationSucceeded = CopyFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB,1);
  if (operationSucceeded != 0) {
    return CONCAT44(in_EDX,operationSucceeded);
  }
  return CONCAT44(in_EDX,1);
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
undefined8 Win32File_CreateDirectoryRecursiveCf(FileSystemCreateDirectoryFlags flags,word *path)

{
  BOOL BVar1;
  undefined4 in_EDX;
  undefined1 uVar2;
  word awStack_418 [256];
  word local_218 [248];
  undefined4 uStack_28;
  
  Package_SetLastErrorPath(path);
  uStack_28 = 0x576502;
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  BVar1 = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
  if (BVar1 == 0) {
    if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
      uVar2 = &stack0xffffffe8 < (undefined1 *)0x400;
      WidePath_SplitParentAndLeaf(local_218,awStack_418,path);
      Win32File_CreateDirectoryRecursiveCf(flags,awStack_418);
      if (!(bool)uVar2) {
        RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
        BVar1 = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,(LPSECURITY_ATTRIBUTES)0x0);
        if (BVar1 != 0)
        goto 
        Win32File_CreateDirectoryRecursiveCf_ReturnSuccessWithCarryClearAfterDirectOrRecursiveCreate
        ;
      }
    }
    Package_SetLastErrorPath(path);
    return CONCAT44(in_EDX,8);
  }
Win32File_CreateDirectoryRecursiveCf_ReturnSuccessWithCarryClearAfterDirectOrRecursiveCreate:
  return CONCAT44(in_EDX,BVar1);
}

/* Address: 0x005765A0.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls RemoveDirectoryA. Error 11 is returned with CF set.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext].
*/
undefined8 Win32File_RemoveDirectoryCf(word *path)

{
  BOOL operationSucceeded;
  undefined4 in_EDX;
  
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = RemoveDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    return CONCAT44(in_EDX,operationSucceeded);
  }
  return CONCAT44(in_EDX,0xb);
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
dword Win32Drive_EnumerateLetters(byte *lettersOut)

{
  uint logicalDriveMask;
  dword enumeratedDriveCount;
  byte currentDriveLetter;
  int driveLettersRemaining;
  
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
  return enumeratedDriveCount;
}

/* Address: 0x00576790.
   Ownership: platform/filesystem/win32.
   Purpose: Validates an ANSI path as DOS-style 8.3 components. Flag bit 0 permits '*' and '?', bit 1 selects one-
   component recursion, and bit 2 permits directory separators and a trailing path. CF clear means valid; CF set
   means rejected. Typed parameters: p0 flags→FileSystemDos83ValidationFlags_V331. Nearby but non-identical
   semantic domains were explicitly deferred.
*/
undefined4 Win32Path_ValidateDos83Cf(FileSystemDos83ValidationFlags flags,byte *pathAnsi)

{
  byte bVar1;
  undefined4 in_EAX;
  int iVar2;
  byte *pbVar3;
  bool bVar4;
  
  if ((flags & FILESYSTEM_DOS83_COMPONENT_ONLY) == 0) {
    if (pathAnsi[1] == 0x3a) {
      bVar1 = *pathAnsi;
      if (bVar1 < 0x41) {
        return in_EAX;
      }
      if (0x7a < bVar1) {
        return in_EAX;
      }
      if ((bVar1 < 0x61) && (0x5a < bVar1)) {
        return in_EAX;
      }
      pathAnsi = pathAnsi + 2;
    }
    bVar4 = *pathAnsi < 0x5c;
    if (*pathAnsi == 0x5c) {
      pathAnsi = pathAnsi + 1;
    }
    while (Win32Path_ValidateDos83Cf
                     (flags | (FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION|
                              FILESYSTEM_DOS83_COMPONENT_ONLY),pathAnsi), !bVar4) {
      while( true ) {
        bVar1 = *pathAnsi;
        pathAnsi = pathAnsi + 1;
        bVar4 = bVar1 < 0x5c;
        if (bVar1 == 0x5c) break;
        if (bVar1 == 0) {
          return in_EAX;
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
          return in_EAX;
        }
        break;
      }
      if (0x2c < bVar1) {
        if (bVar1 < 0x2f) break;
        if (bVar1 == 0x2f) {
          return in_EAX;
        }
        if (0x39 < bVar1) {
          if (bVar1 == 0x3f) {
            if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
              return in_EAX;
            }
          }
          else {
            if (bVar1 < 0x41) {
              return in_EAX;
            }
            if (0x5a < bVar1) {
              if (bVar1 == 0x5c) break;
              if (bVar1 < 0x61) {
                return in_EAX;
              }
              if (0x7a < bVar1) {
                return in_EAX;
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
              return in_EAX;
            }
            if (bVar1 == 0x2a) {
              if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                return in_EAX;
              }
              break;
            }
            if (0x2c < bVar1) {
              if (bVar1 < 0x30) {
                return in_EAX;
              }
              if (0x39 < bVar1) {
                if (bVar1 == 0x3f) {
                  if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                    return in_EAX;
                  }
                }
                else {
                  if (bVar1 < 0x41) {
                    return in_EAX;
                  }
                  if (0x5a < bVar1) {
                    if (bVar1 == 0x5c) {
                      return in_EAX;
                    }
                    if (bVar1 < 0x61) {
                      return in_EAX;
                    }
                    if (0x7a < bVar1) {
                      return in_EAX;
                    }
                  }
                }
              }
            }
            iVar2 = iVar2 + -1;
          } while (iVar2 != 0);
          bVar1 = pbVar3[2];
          if (bVar1 == 0) {
            return in_EAX;
          }
        }
        if ((flags & FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION) == 0) {
          return in_EAX;
        }
        if (bVar1 != 0x5c) {
          return in_EAX;
        }
      }
      return in_EAX;
    }
  }
  return in_EAX;
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
dword Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf
                (FileSystemEnumerationMode mode,dword reserved,
                FileSystemOutputCapacityBytes outputCapacityBytes,byte *outputRecords,
                byte *pathOrVolumeText)

{
  HANDLE hFindFile;
  BOOL BVar1;
  uint uVar2;
  int extraout_ECX;
  int iVar3;
  int extraout_ECX_00;
  int iVar4;
  word *pwVar5;
  word *pwVar6;
  byte *pbVar7;
  word *leftText;
  word *pwVar8;
  bool bVar9;
  bool bVar10;
  
  if (mode == FILESYSTEM_ENUMERATE_VOLUME_LABEL) {
    g_Win32DriveRootPathScratchA = *pathOrVolumeText;
    BVar1 = GetVolumeInformationA
                      ((LPCSTR)&g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratchA,0x80,
                       (LPDWORD)0x0,(LPDWORD)0x0,(LPDWORD)0x0,(LPSTR)0x0,0);
    if (BVar1 == 0) {
      return 0x200;
    }
    if (0xff < outputCapacityBytes) {
      Text_CopyNarrowToUtf16Cf(0x200,(word *)outputRecords,g_Win32PathScratchA);
      return 0x200;
    }
  }
  else {
    Package_SetLastErrorPath((word *)pathOrVolumeText);
    RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,(word *)pathOrVolumeText);
    hFindFile = FindFirstFileA((LPCSTR)g_Win32PathScratchA,
                               (LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    if (hFindFile == (HANDLE)0xffffffff) {
      return 0x200;
    }
    uVar2 = 0;
    pwVar5 = (word *)outputRecords;
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
          Text_CopyNarrowToUtf16Cf(0x200,pwVar5,(byte *)&g_Win32FindDataFileNameA);
          pwVar5 = pwVar5 + 0x100;
          uVar2 = extraout_ECX + 1;
          outputCapacityBytes = outputCapacityBytes - 0x200;
        }
      }
      BVar1 = FindNextFileA(hFindFile,(LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    } while (BVar1 != 0);
    FindClose(hFindFile);
    bVar9 = uVar2 < 2;
    if (!bVar9) {
      iVar3 = uVar2 - 1;
      bVar10 = iVar3 == 0;
      pwVar5 = (word *)(outputRecords + 0x200);
      leftText = (word *)outputRecords;
      do {
        do {
          Utf16String_CompareAsciiCaseInsensitiveFlags(pwVar5,leftText);
          if (!bVar9 && !bVar10) {
            pwVar6 = pwVar5;
            pbVar7 = g_Win32PathScratchA;
            for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
              *(undefined4 *)pbVar7 = *(undefined4 *)pwVar6;
              pwVar6 = pwVar6 + 2;
              pbVar7 = pbVar7 + 4;
            }
            pwVar6 = leftText;
            pwVar8 = pwVar5;
            for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
              *(undefined4 *)pwVar8 = *(undefined4 *)pwVar6;
              pwVar6 = pwVar6 + 2;
              pwVar8 = pwVar8 + 2;
            }
            pbVar7 = g_Win32PathScratchA;
            pwVar6 = leftText;
            for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
              *(undefined4 *)pwVar6 = *(undefined4 *)pbVar7;
              pbVar7 = pbVar7 + 4;
              pwVar6 = pwVar6 + 2;
            }
          }
          pwVar5 = pwVar5 + 0x100;
          bVar9 = (word *)0xfffffdff < leftText;
          bVar10 = extraout_ECX_00 == 1;
          leftText = leftText + 0x100;
        } while (!bVar10);
        iVar3 = iVar3 + -1;
        pwVar5 = (word *)(outputRecords + 0x200);
        bVar9 = iVar3 == 0;
        bVar10 = iVar3 == 1;
        leftText = (word *)outputRecords;
      } while (!bVar9);
    }
  }
  return 0x200;
}

/* Address: 0x00576020.
   Ownership: platform/filesystem/win32.
   Purpose: Reads exactly byteCount bytes. CF clear means the requested count was transferred; CF set returns
   engine error 6.
*/
dword Win32File_ReadExactCf(FileIoByteCount byteCount,void *destination,void *handle)

{
  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,(LPOVERLAPPED)0x0);
  if (g_Win32FileBytesTransferred == byteCount) {
    return g_Win32FileBytesTransferred;
  }
  return 6;
}

/* Address: 0x00576100.
   Ownership: platform/filesystem/win32.
   Purpose: Returns the low 32-bit file size with CF clear. GetFileSize failure returns zero with CF set.
*/
dword Win32File_GetSizeCf(void *handle)

{
  DWORD fileSize;
  
  fileSize = GetFileSize(handle,(LPDWORD)0x0);
  if (fileSize != 0xffffffff) {
    return fileSize;
  }
  return 0;
}

/* Address: 0x00576430.
   Ownership: platform/filesystem/win32.
   Purpose: Gets the current ANSI directory and converts it into a 0x200-byte UTF-16 destination. CF reports Win32
   failure.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
longlong Win32File_GetCurrentDirectoryCf(word *destination)

{
  DWORD narrowPathLength;
  int copiedPathByteLength;
  uint in_EDX;
  
  narrowPathLength = GetCurrentDirectoryA(0xff,(LPSTR)g_Win32PathScratchA);
  if (narrowPathLength != 0) {
    copiedPathByteLength = Text_CopyNarrowToUtf16Cf(0x200,destination,g_Win32PathScratchA);
    return CONCAT44(in_EDX,copiedPathByteLength);
  }
  destination[0] = 0;
  destination[1] = 0;
  return (ulonglong)in_EDX << 0x20;
}

/* Address: 0x00576490.
   Ownership: platform/filesystem/win32.
   Purpose: Converts one UTF-16 path and calls SetCurrentDirectoryA. Error 10 is returned with CF set.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], RichTextCommandStream_CopyToNarrowCf
   [assets/text/richtext].
*/
undefined8 Win32File_SetCurrentDirectoryCf(word *path)

{
  BOOL operationSucceeded;
  undefined4 in_EDX;
  
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrowCf(0x100,g_Win32PathScratchA,path);
  operationSucceeded = SetCurrentDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (operationSucceeded != 0) {
    return CONCAT44(in_EDX,operationSucceeded);
  }
  return CONCAT44(in_EDX,10);
}

/* Address: 0x00576650.
   Ownership: platform/filesystem/win32.
   Purpose: Maps GetDriveTypeA to engine codes 0x28 removable, 0x2A remote, 0x2B CD-ROM, or 0x29 otherwise. Typed
   parameters: p0 driveLetter→DosDriveLetterCode32_V342. Calling convention, exact VariableStorage serialization,
   function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
EngineDriveTypeCode Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter)

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
dword Win32File_OpenCf(FileSystemOpenFlags openFlags,word *path)

{
  HANDLE fileHandle;
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
    return (dword)fileHandle;
  }
  return 1;
}

/* Address: 0x00576000.
   Ownership: platform/filesystem/win32.
   Purpose: Closes one Win32 file handle.
*/
void Win32File_Close(void *handle)

{
  CloseHandle(handle);
  return;
}
