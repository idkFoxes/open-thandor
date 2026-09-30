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
   Lists a directory (or a drive's volume label) as a compact string table: the fixed-size name records of
   g_FileSystemEnumerateDirectoryOrVolumeEntries are collected in the largest free arena block, then copied
   into a second largest block as an array of entryCount UTF-16 string pointers followed by the strings, which
   is shrunk to its used size. Returns true with the table in *outTable and the entry count in *outEntryCount;
   an empty listing stores NULL and 0. Returns false (outputs untouched) when an arena block cannot be had, the
   enumeration fails or the strings do not fit; the original left only scratch values in EAX/ECX then. No caller
   in the recovered code (reached only through the function map).
*/
bool FileSystem_BuildEnumerationStringTable
          (FileSystemEnumerationMode enumerationMode,uint32_t reserved,uint8_t *pathOrVolumeText,
          uint16_t ***outTable,uint32_t *outEntryCount)

{
  short codeUnit;
  uint8_t *recordBuffer;
  uint32_t recordSizeBytes;
  uint8_t *tableOrError;
  FileSystemOutputCapacityBytes foundEntryCount;
  FileSystemOutputCapacityBytes capacityBytesOrError;
  FileSystemOutputCapacityBytes remainingEntries;
  uint8_t *tablePointerSlot;
  uint8_t *sourceCodeUnit;
  uint8_t *sourceRecord;
  uint8_t *stringCursor;
  bool capacityFlag;
  ArenaShrinkResult shrinkResult;
  ArenaLargestAllocResult largestBlock;
  DirectoryEnumerationResult enumerationResult;

  largestBlock = g_MemoryApi.allocLargestFreeBlock();
  capacityBytesOrError = largestBlock.blockSizeOrSentinel;
  recordBuffer = (uint8_t *)largestBlock.allocationOrError;
  if (!largestBlock.failed) {
    enumerationResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                       (enumerationMode,reserved,capacityBytesOrError,recordBuffer,pathOrVolumeText);
    foundEntryCount = enumerationResult.entryCount;
    recordSizeBytes = enumerationResult.recordSizeBytes;
    tableOrError = (uint8_t *)recordSizeBytes;
    capacityBytesOrError = foundEntryCount;
    if (!enumerationResult.failed) {
      if (foundEntryCount == 0) {
        g_MemoryApi.free(recordBuffer);
        *outTable = NULL;
        *outEntryCount = 0;
        return true;
      }
      /* give the unused tail of the record buffer back before taking the next largest block */
      capacityBytesOrError = foundEntryCount * recordSizeBytes;
      shrinkResult = g_MemoryApi.shrinkInPlace(capacityBytesOrError,recordBuffer);
      tableOrError = (uint8_t *)shrinkResult.scratchOrError;
      if (!shrinkResult.failed) {
        largestBlock = g_MemoryApi.allocLargestFreeBlock();
        capacityBytesOrError = largestBlock.blockSizeOrSentinel;
        tableOrError = (uint8_t *)largestBlock.allocationOrError;
        if (!largestBlock.failed) {
          /* the strings follow the array of foundEntryCount pointers */
          stringCursor = tableOrError + foundEntryCount * 4;
          capacityFlag = foundEntryCount * 4 <= capacityBytesOrError; /* the pointer array fits */
          capacityBytesOrError = capacityBytesOrError + foundEntryCount * -4;
          remainingEntries = foundEntryCount;
          tablePointerSlot = tableOrError;
          sourceRecord = recordBuffer;
          if (capacityFlag && capacityBytesOrError != 0) {
            do {
              *(uint8_t **)tablePointerSlot = stringCursor;
              sourceCodeUnit = sourceRecord;
              do {
                codeUnit = *(short *)sourceCodeUnit;
                *(short *)stringCursor = codeUnit;
                sourceCodeUnit = sourceCodeUnit + 2;
                stringCursor = stringCursor + 2;
                capacityFlag = capacityBytesOrError < 2; /* the block is exhausted */
                capacityBytesOrError = capacityBytesOrError - 2;
                if (capacityFlag || capacityBytesOrError == 0) goto freeTable;
              } while (codeUnit != 0);
              tablePointerSlot = tablePointerSlot + 4;
              sourceRecord = sourceRecord + recordSizeBytes;
              remainingEntries--;
            } while (remainingEntries != 0);
            g_MemoryApi.shrinkInPlace(stringCursor - tableOrError,tableOrError);
            g_MemoryApi.free(recordBuffer);
            *outEntryCount = foundEntryCount;
            *outTable = (uint16_t **)tableOrError;
            return true;
          }
freeTable:
          g_MemoryApi.free(tableOrError);
        }
      }
    }
    g_MemoryApi.free(recordBuffer);
  }
  return false;
}

/* Address: 0x00575CB0.
   Starts the file layer: records the executable directory, installs the Win32 implementations of the
   g_FileSystem* function table, replaces the default L"Computer" label with the machine name, allocates the
   8 MiB package scratch buffer, loads THANDOR.cfg (current directory first, then the executable
   directory) and normalizes it in place, remembers the working directory and mounts engine.pck.
   The original always returns with CF clear; EAX is the result of the engine.pck mount.
*/
uint32_t __cdecl FileSystem_Init(void)

{
  uint8_t configByte;
  uint8_t *configCursor;
  BOOL gotComputerName;
  void *configFile;
  int clearCount;
  ArenaPayloadByteCount configBytesLeft;
  uint16_t *labelCursor;
  ArenaAllocResult allocResult;
  Win32FileOpenResult openResult;
  Win32FileSizeResult sizeResult;
  Win32FileReadResult readResult;
  uint32_t engineHandleOrError; /* engine.pck's handle, or the mount error code */

  /* open-thandor: the original took the executable path from the first command-line token, which
     is only a bare "thandor.exe" when started from a shell or batch file; the executable
     directory then came out empty. Use the module path instead. */
  Thandor_GetExecutablePathA((char *)g_Win32PathScratchA,sizeof g_Win32PathScratchA);
  Text_CopyNarrowToUtf16(sizeof g_PackageLastErrorPath,g_PackageLastErrorPath,g_Win32PathScratchA);
  /* the leaf (the executable name) lands in the path scratch buffer, which is reused as UTF-16 */
  WidePath_SplitParentAndLeaf
            ((uint16_t *)g_Win32PathScratchA,g_ExecutableDirectoryUtf16,g_PackageLastErrorPath);
  g_FileSystemOpen = Win32File_Open;
  g_FileSystemClose = Win32File_Close;
  g_FileSystemReadExact = Win32File_ReadExact;
  g_FileSystemWriteExactOrFlush = Win32File_WriteExactOrFlush;
  g_FileSystemGetSize = Win32File_GetSize;
  /* the generated slot types of GetPosition and Delete return only EAX; callers cast them back to read CF */
  g_FileSystemGetPosition = (FileSystemGetPositionProc *)Win32File_GetPosition;
  g_FileSystemSeek = Win32File_Seek;
  g_FileSystemDelete = (FileSystemDeleteProc *)Win32File_Delete;
  g_FileSystemGetCurrentDirectory = Win32File_GetCurrentDirectory;
  g_FileSystemSetCurrentDirectory = Win32File_SetCurrentDirectory;
  g_FileSystemRemoveDirectory = Win32File_RemoveDirectory;
  g_FileSystemCreateDirectoryRecursive = Win32File_CreateDirectoryRecursive;
  g_FileSystemEnumerateDriveLetters = Win32Drive_EnumerateLetters;
  g_FileSystemGetDriveTypeCode = Win32Drive_GetEngineTypeCode;
  g_FileSystemCheckDriveMediaReady = Win32Drive_CheckMediaReady;
  g_FileSystemGetFreeAndTotalBytesRegs = Win32Drive_GetFreeAndTotalBytesRegs;
  g_FileSystemGetLastWriteDosDate = Win32File_GetLastWriteDosDate;
  g_FileSystemGetLastWriteTimeHigh = Win32File_GetLastWriteTimeHigh;
  g_FileSystemGetVolumeSerialNumber = Win32Drive_GetVolumeSerialNumber;
  g_FileSystemMove = Win32File_Move;
  g_FileSystemCopy = Win32File_Copy;
  g_FileSystemEnumerateDirectoryOrVolumeEntries =
       Win32FileSystem_EnumerateDirectoryOrVolumeEntries;
  g_FileSystemValidateDos83Path = Win32Path_ValidateDos83;
  /* GetComputerNameA size in/out; the same global later holds the THANDOR.cfg text */
  g_FileSystemInitComputerNameCapacityOrConfigCursor = (pointer)sizeof g_Win32PathScratchA;
  gotComputerName = GetComputerNameA((LPSTR)g_Win32PathScratchA,
                                     (LPDWORD)&g_FileSystemInitComputerNameCapacityOrConfigCursor);
  if (gotComputerName != 0) {
    labelCursor = g_DefaultComputerLabelUtf16;
    for (clearCount = sizeof g_DefaultComputerLabelUtf16 / 4; clearCount != 0; clearCount--) {
      labelCursor[0] = 0;
      labelCursor[1] = 0;
      labelCursor += 2;
    }
    Text_CopyNarrowToUtf16(sizeof g_DefaultComputerLabelUtf16,g_DefaultComputerLabelUtf16,g_Win32PathScratchA);
  }
  allocResult = ArenaHeap_Alloc(PACKAGE_SCRATCH_BUFFER_BYTES);
  if (allocResult.failed) {
    FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),true);
  }
  g_PackageScratchBuffer = (uint8_t *)allocResult.payloadOrError;
  openResult = Win32File_Open(0,u_THANDOR_cfg_0040e23d);
  configFile = (void *)openResult.handleOrError;
  if (openResult.failed) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,u_THANDOR_cfg_0040e23d,
               g_ExecutableDirectoryUtf16);
    openResult = Win32File_Open(0,g_FileSystemCombinedPathScratchUtf16);
    configFile = (void *)openResult.handleOrError;
  }
  if (!openResult.failed) {
    sizeResult = Win32File_GetSize(configFile);
    configBytesLeft = sizeResult.sizeOrError;
    if ((!sizeResult.failed) && (configBytesLeft != 0)) {
      allocResult = ArenaHeap_Alloc(configBytesLeft);
      configCursor = (uint8_t *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        readResult = Win32File_ReadExact(configBytesLeft,configCursor,configFile);
        if (readResult.failed) {
          ArenaHeap_Free(configCursor);
        }
        else {
          g_FileSystemInitComputerNameCapacityOrConfigCursor = configCursor;
          g_FileSystemConfigRemainingBytes = configBytesLeft;
          /* Normalize the text in place: separators (<= ' ') and [comments] become NUL, other
             characters go through the normalization map. */
          do {
            configByte = *configCursor;
            if (configByte == '[') {
              /* blank the comment up to its closing ']', which is then blanked as a separator */
              do {
                *configCursor = 0;
                configCursor++;
                configBytesLeft--;
              } while ((configBytesLeft != 0) && (*configCursor != ']'));
              if (configBytesLeft == 0) break;
              configByte = 0;
            }
            if (configByte <= ' ') {
              *configCursor = 0;
            }
            else {
              *configCursor = g_FileSystemConfigCharacterNormalizationMap[configByte];
            }
            configCursor++;
            configBytesLeft--;
          } while (configBytesLeft != 0);
        }
      }
    }
    Win32File_Close(configFile);
  }
  Win32File_GetCurrentDirectory(g_InitialWorkingDirectory.codeUnits);
  if (Package_MountLowPriority(u_engine_pck_0040e255,&engineHandleOrError)) {
    g_EnginePackageLowPriorityMountHandle = engineHandleOrError;
  }
  return engineHandleOrError;
}


/* Address: 0x005762F0.
   Returns the last-write time of a file as a packed DOS date and time (date in the high word, time in
   the low word), CF clear. CF set with FATAL_ERROR_FILE_ACCESS_FAILED when the file cannot be opened or
   its time cannot be read.
*/
StatusResult Win32File_GetLastWriteDosDate(uint16_t *path)

{
  BOOL gotFileTime;
  HANDLE fileHandleOrError;
  Win32FileOpenResult openResult;
  StatusResult successResult;
  StatusResult failureResult;

  openResult = Win32File_Open(0,path);
  fileHandleOrError = (HANDLE)openResult.handleOrError;
  if ((!openResult.failed) && (fileHandleOrError != INVALID_HANDLE_VALUE)) {
    gotFileTime =
         GetFileTime(fileHandleOrError,NULL,NULL,(LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(fileHandleOrError);
    fileHandleOrError = (HANDLE)FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTime != 0) {
      /* FAT date into the high word, FAT time into the low word of the scratch dword */
      FileTimeToDosDateTime
                ((FILETIME *)&g_Win32FileLastWriteTimeScratch,
                 (LPWORD)&g_Win32FileCreationTimeOrDosDateScratch + 1,
                 (LPWORD)&g_Win32FileCreationTimeOrDosDateScratch);
      successResult.failed = false;
      successResult.valueOrError = g_Win32FileCreationTimeOrDosDateScratch;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)fileHandleOrError;
  return failureResult;
}


/* Address: 0x00576360.
   Returns the high dword of a file's last-write FILETIME (a coarse modification stamp, about 7 minutes
   per step), CF clear. CF set with FATAL_ERROR_FILE_ACCESS_FAILED when the file cannot be opened or its
   time cannot be read.
*/
StatusResult Win32File_GetLastWriteTimeHigh(uint16_t *path)

{
  BOOL gotFileTime;
  HANDLE fileHandleOrError;
  Win32FileOpenResult openResult;
  StatusResult successResult;
  StatusResult failureResult;

  openResult = Win32File_Open(0,path);
  fileHandleOrError = (HANDLE)openResult.handleOrError;
  if (!openResult.failed) {
    gotFileTime =
         GetFileTime(fileHandleOrError,NULL,NULL,(LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(fileHandleOrError);
    fileHandleOrError = (HANDLE)FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTime != 0) {
      successResult.failed = false;
      successResult.valueOrError = g_Win32FileLastWriteTimeHighScratch;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)fileHandleOrError;
  return failureResult;
}


/* Address: 0x005763C0.
   Despite its slot name (g_FileSystemGetVolumeSerialNumber) this queries no volume: it reads all three
   FILETIMEs of the file at path, clears the first byte of outputLabel and returns the high dword of the
   last-write time, like Win32File_GetLastWriteTimeHigh. The original reports failure in CF (with
   FATAL_ERROR_FILE_ACCESS_FAILED in EAX, STC at 0x00576416); this C version returns only EAX. Nothing
   calls through g_FileSystemGetVolumeSerialNumber (only FileSystem_Init stores it), so CF is unobservable.
*/
uint32_t Win32Drive_GetVolumeSerialNumber(uint8_t *outputLabel,char *path)

{
  BOOL gotFileTimes;
  HANDLE fileHandleOrError;
  uint32_t lastWriteTimeHigh;
  Win32FileOpenResult openResult;

  openResult = Win32File_Open(0,(uint16_t *)path);
  fileHandleOrError = (HANDLE)openResult.handleOrError;
  if (!openResult.failed) {
    gotFileTimes =
         GetFileTime(fileHandleOrError,(LPFILETIME)&g_Win32FileCreationTimeOrDosDateScratch,
                     (LPFILETIME)&g_Win32FileLastAccessTimeScratch,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(fileHandleOrError);
    fileHandleOrError = (HANDLE)FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTimes != 0) {
      lastWriteTimeHigh = g_Win32FileLastWriteTimeHighScratch;
      *outputLabel = 0;
      return lastWriteTimeHigh;
    }
  }
  return (uint32_t)fileHandleOrError;
}


/* Address: 0x00575F40.
   Changes back to the working directory FileSystem_Init found at startup, if one was captured.
*/
void __cdecl Win32FileSystem_RestoreInitialDirectory(void)

{
  if (g_InitialWorkingDirectory.firstTwoCodeUnits != 0) {
    Win32File_SetCurrentDirectory(g_InitialWorkingDirectory.codeUnits);
  }
  return;
}

/* Address: 0x005766F0.
   Reports whether a drive has usable media: CF clear (false) for fixed, network and other drives,
   CF set (true) for removable and CD-ROM drives. The original contains an unreachable
   \\.\X: + IOCTL_STORAGE_CHECK_VERIFY probe after the type check, so removable and CD-ROM drives are
   always reported as not ready.
*/
bool Win32Drive_CheckMediaReady(DosDriveLetterCode32 driveLetter)

{
  uint32_t engineDriveType;

  engineDriveType = Win32Drive_GetEngineTypeCode(driveLetter);
  if ((engineDriveType != ENGINE_DRIVE_REMOVABLE) && (engineDriveType != ENGINE_DRIVE_CDROM)) {
    return false;
  }
  return true;
}


/* Address: 0x0040EF50.
   Reads a whole file into a new arena buffer: the path is tried next to the executable first, then as
   given. Returns true with the buffer in *outBuffer, or false with the open/size/read error in *outError
   (0 when the size query failed), or FATAL_ERROR_OUT_OF_MEMORY with the file size left in
   g_FatalErrorDetail1Utf16. *outBuffer is only written on success, *outError only on failure. No caller in
   the recovered code (reached only through the function map).
*/
bool FileSystem_LoadWholeFile(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError)

{
  void *handle;
  uint32_t byteCountOrError;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  ArenaAllocResult allocResult;
  FileSystemReadResult readResult;

  /* first try the path relative to the executable directory */
  WidePath_CombineDirectoryAndLeaf
            (g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             g_ExecutableDirectoryUtf16);
  openResult = g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16);
  if (openResult.failed) {
    openResult = g_FileSystemOpen(0,pathUtf16);
    if (openResult.failed) {
      *outError = openResult.handleOrError; /* the open error code */
      return false;
    }
  }
  handle = (void *)openResult.handleOrError;
  sizeResult = g_FileSystemGetSize(handle);
  byteCountOrError = sizeResult.sizeOrError;
  if (!sizeResult.failed) {
    allocResult = g_MemoryApi.alloc(byteCountOrError);
    if (allocResult.failed) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)byteCountOrError,g_FatalErrorDetail1Utf16);
      byteCountOrError = FATAL_ERROR_OUT_OF_MEMORY;
    }
    else {
      readResult = g_FileSystemReadExact((FileIoByteCount)byteCountOrError,(void *)allocResult.payloadOrError,
                                         handle);
      byteCountOrError = readResult.valueOrError;
      if (!readResult.failed) {
        g_FileSystemClose(handle);
        *outBuffer = (void *)allocResult.payloadOrError;
        return true;
      }
      g_MemoryApi.free((void *)allocResult.payloadOrError);
    }
  }
  g_FileSystemClose(handle);
  *outError = byteCountOrError;
  return false;
}

/* Address: 0x0040F120.
   Same whole-file load as FileSystem_LoadWholeFile (same search order and errors); the only difference in
   the original is that ECX is not preserved: it returns the file size there. Same C interface too: true
   with the buffer in *outBuffer, or false with the error in *outError. No caller in the recovered code
   (reached only through the function map).
*/
bool FileSystem_LoadWholeFileAlternatePath(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError)

{
  void *handle;
  uint32_t byteCountOrError;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  ArenaAllocResult allocResult;
  FileSystemReadResult readResult;

  /* first try the path relative to the executable directory */
  WidePath_CombineDirectoryAndLeaf
            (g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             g_ExecutableDirectoryUtf16);
  openResult = g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16);
  if (openResult.failed) {
    openResult = g_FileSystemOpen(0,pathUtf16);
    if (openResult.failed) {
      *outError = openResult.handleOrError; /* the open error code */
      return false;
    }
  }
  handle = (void *)openResult.handleOrError;
  sizeResult = g_FileSystemGetSize(handle);
  byteCountOrError = sizeResult.sizeOrError;
  if (!sizeResult.failed) {
    allocResult = g_MemoryApi.alloc(byteCountOrError);
    if (allocResult.failed) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)byteCountOrError,g_FatalErrorDetail1Utf16);
      byteCountOrError = FATAL_ERROR_OUT_OF_MEMORY;
    }
    else {
      readResult = g_FileSystemReadExact((FileIoByteCount)byteCountOrError,(void *)allocResult.payloadOrError,
                                         handle);
      byteCountOrError = readResult.valueOrError;
      if (!readResult.failed) {
        g_FileSystemClose(handle);
        *outBuffer = (void *)allocResult.payloadOrError;
        return true;
      }
      g_MemoryApi.free((void *)allocResult.payloadOrError);
    }
  }
  g_FileSystemClose(handle);
  *outError = byteCountOrError;
  return false;
}

/* Address: 0x0040F1F0.
   Writes a whole buffer to a file, creating or truncating it with exclusive access. A failed write
   leaves no partial file behind: it is closed and deleted. Returns 0 on success, otherwise the open or
   write error (FATAL_ERROR_FILE_ACCESS_FAILED, FATAL_ERROR_FILE_WRITE_FAILED or
   FATAL_ERROR_FILE_WRITE_INCOMPLETE, never 0).
*/
uint32_t FileSystem_WriteBufferToPath(FileIoByteCount byteCount,void *source,uint16_t *path)

{
  void *handle;
  uint32_t writeError;
  FileSystemOpenResult openResult;
  FileSystemWriteResult writeResult;

  openResult = g_FileSystemOpen
                    (FILESYSTEM_OPEN_EXCLUSIVE_SHARE|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE,path);
  if (openResult.failed) {
    return openResult.handleOrError; /* the open error code */
  }
  handle = (void *)openResult.handleOrError;
  writeResult = g_FileSystemWriteExactOrFlush(byteCount,source,handle);
  writeError = writeResult.valueOrError;
  g_FileSystemClose(handle);
  if (!writeResult.failed) {
    return 0;
  }
  g_FileSystemDelete(1,path); /* the first argument is unused by Win32File_Delete */
  return writeError;
}


/* Address: 0x00576070.
   Writes exactly byteCount bytes to a file; byteCount 0 instead truncates the file at the current
   position (SetEndOfFile). CF set with FATAL_ERROR_FILE_WRITE_FAILED when WriteFile fails, or
   FATAL_ERROR_FILE_WRITE_INCOMPLETE when it wrote fewer bytes (disk full).
*/
Win32FileWriteResult Win32File_WriteExactOrFlush(FileIoByteCount byteCount,void *source,void *handle)

{
  BOOL wroteFile;
  uint32_t writeError;
  BOOL truncatedFile;
  Win32FileWriteResult successResult;
  Win32FileWriteResult flushResult;
  Win32FileWriteResult failureResult;

  g_Win32FileBytesTransferred = 0;
  if (byteCount == 0) {
    truncatedFile = SetEndOfFile(handle);
    flushResult.failed = false;
    flushResult.valueOrError = truncatedFile;
    return flushResult;
  }
  wroteFile = WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,NULL);
  writeError = FATAL_ERROR_FILE_WRITE_FAILED;
  if ((wroteFile != 0) &&
     (writeError = FATAL_ERROR_FILE_WRITE_INCOMPLETE, byteCount == g_Win32FileBytesTransferred)) {
    successResult.valueOrError = FATAL_ERROR_FILE_WRITE_INCOMPLETE; /* EAX still holds the code; CF is clear */
    successResult.failed = false;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = writeError;
  return failureResult;
}


/* Address: 0x00576140.
   Returns the current position of a file; 0 with CF set when SetFilePointer fails (0x0057616D).
*/
Win32FileSeekResult Win32File_GetPosition(void *handle)

{
  DWORD filePosition;
  Win32FileSeekResult successResult;
  Win32FileSeekResult failureResult;

  filePosition = SetFilePointer(handle,0,NULL,FILE_CURRENT);
  if (filePosition != INVALID_SET_FILE_POINTER) {
    successResult.failed = false;
    successResult.positionOrError = filePosition;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.positionOrError = 0;
  return failureResult;
}

/* Address: 0x00576180.
   Moves the file pointer (moveMethod is FILESYSTEM_SEEK_BEGIN/CURRENT/END, the Win32 FILE_* values) and
   returns the new position; CF set with FATAL_ERROR_FILE_SEEK_FAILED on failure.
*/
Win32FileSeekResult Win32File_Seek(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle)

{
  DWORD newFilePosition;
  Win32FileSeekResult successResult;
  Win32FileSeekResult failureResult;

  newFilePosition = SetFilePointer(handle,distance,NULL,moveMethod);
  if (newFilePosition != INVALID_SET_FILE_POINTER) {
    successResult.failed = false;
    successResult.positionOrError = newFilePosition;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.positionOrError = FATAL_ERROR_FILE_SEEK_FAILED;
  return failureResult;
}


/* Address: 0x005761C0.
   Deletes a file; the first argument is an unused slot of the g_FileSystemDelete interface. CF set with
   FATAL_ERROR_FILE_ACCESS_FAILED on failure (0x00576202); on success EAX is DeleteFileA's result.
*/
StatusResult Win32File_Delete(uint32_t unusedFlags,uint16_t *path)

{
  BOOL deletedFile;
  StatusResult successResult;
  StatusResult failureResult;

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,path);
  deletedFile = DeleteFileA((LPCSTR)g_Win32PathScratchA);
  if (deletedFile != 0) {
    successResult.failed = false;
    successResult.valueOrError = deletedFile;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_FILE_ACCESS_FAILED;
  return failureResult;
}

/* Address: 0x00576210.
   Moves (renames) a file from sourcePath to destinationPath; CF set with FATAL_ERROR_FILE_ACCESS_FAILED on
   failure.
*/
StatusResult Win32File_Move(uint16_t *destinationPath,uint16_t *sourcePath)

{
  BOOL movedFile;
  StatusResult successResult;
  StatusResult failureResult;

  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchB,g_Win32PathScratchB,destinationPath);
  movedFile = MoveFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB);
  if (movedFile != 0) {
    successResult.failed = false;
    successResult.valueOrError = movedFile;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_FILE_ACCESS_FAILED;
  return failureResult;
}


/* Address: 0x00576280.
   Copies a file from sourcePath to destinationPath without overwriting an existing destination; CF set
   with FATAL_ERROR_FILE_ACCESS_FAILED on failure.
*/
StatusResult Win32File_Copy(uint16_t *destinationPath,uint16_t *sourcePath)

{
  BOOL copiedFile;
  StatusResult successResult;
  StatusResult failureResult;

  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchB,g_Win32PathScratchB,destinationPath);
  copiedFile = CopyFileA((LPCSTR)g_Win32PathScratchA,(LPCSTR)g_Win32PathScratchB,TRUE /* fail if exists */);
  if (copiedFile != 0) {
    successResult.failed = false;
    successResult.valueOrError = copiedFile;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_FILE_ACCESS_FAILED;
  return failureResult;
}


/* Address: 0x005764E0.
   Creates a directory. With FILESYSTEM_CREATE_DIRECTORY_RECURSIVE a failed attempt first creates the
   parent directories (recursively) and then retries. CF set with FATAL_ERROR_FILE_WRITE_FAILED when the
   directory cannot be created.
*/
StatusResult Win32File_CreateDirectoryRecursive(FileSystemCreateDirectoryFlags flags,uint16_t *path)

{
  uint32_t createdDirectory;
  StatusResult parentOrSuccessResult;
  StatusResult failureResult;
  uint16_t parentPath [256];
  uint16_t leafName [248];

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,path);
  createdDirectory = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,NULL);
  if (createdDirectory == 0) {
    if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
      WidePath_SplitParentAndLeaf(leafName,parentPath,path);
      parentOrSuccessResult = Win32File_CreateDirectoryRecursive(flags,parentPath);
      if (!parentOrSuccessResult.failed) {
        RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,path);
        createdDirectory = CreateDirectoryA((LPCSTR)g_Win32PathScratchA,NULL);
        if (createdDirectory != 0) {
          /* created after its parents */
          parentOrSuccessResult.failed = false;
          parentOrSuccessResult.valueOrError = createdDirectory;
          return parentOrSuccessResult;
        }
      }
    }
    Package_SetLastErrorPath(path);
    failureResult.failed = true;
    failureResult.valueOrError = FATAL_ERROR_FILE_WRITE_FAILED;
    return failureResult;
  }
  parentOrSuccessResult.failed = false;
  parentOrSuccessResult.valueOrError = createdDirectory;
  return parentOrSuccessResult;
}


/* Address: 0x005765A0.
   Removes an (empty) directory; CF set with FATAL_ERROR_REMOVE_DIRECTORY_FAILED on failure. Unlike the
   other path operations it does not record the path in g_PackageLastErrorPath.
*/
StatusResult Win32File_RemoveDirectory(uint16_t *path)

{
  BOOL removedDirectory;
  StatusResult successResult;
  StatusResult failureResult;

  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,path);
  removedDirectory = RemoveDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (removedDirectory != 0) {
    successResult.failed = false;
    successResult.valueOrError = removedDirectory;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_REMOVE_DIRECTORY_FAILED;
  return failureResult;
}


/* Address: 0x005765F0.
   Returns the free (EAX) and total (EDX) bytes of a drive, both 0 when the query fails. The products
   are 32-bit, so drives above 4 GiB wrap.
*/
Win32DriveCapacityEdxEax8 Win32Drive_GetFreeAndTotalBytesRegs(DosDriveLetterCode32 driveLetter)

{
  BOOL gotDiskSpace;
  int freeBytes;
  int totalBytes;

  g_Win32DriveRootPathScratchA[0] = (char)driveLetter; /* "X:\" root path scratch */
  gotDiskSpace =
       GetDiskFreeSpaceA(g_Win32DriveRootPathScratchA,(LPDWORD)&g_Win32DiskSectorsPerClusterScratch
                         ,(LPDWORD)&g_Win32DiskBytesPerSectorScratch,
                         (LPDWORD)&g_Win32DiskFreeClustersScratch,
                         (LPDWORD)&g_Win32DiskTotalClustersScratch);
  totalBytes = 0;
  freeBytes = 0;
  if (gotDiskSpace != 0) {
    freeBytes = g_Win32DiskFreeClustersScratch *
                g_Win32DiskBytesPerSectorScratch * g_Win32DiskSectorsPerClusterScratch;
    totalBytes = g_Win32DiskTotalClustersScratch *
                 g_Win32DiskBytesPerSectorScratch * g_Win32DiskSectorsPerClusterScratch;
  }
  return (uint64_t)(uint32_t)totalBytes << 32 | (uint64_t)(uint32_t)freeBytes; /* EDX = total, EAX = free */
}

/* Address: 0x005766B0.
   Lists the existing drives: writes one letter 'A'..'Z' per set bit of GetLogicalDrives to lettersOut
   (no terminator) and returns the number of letters in EAX and ECX.
*/
DriveLetterEnumeration Win32Drive_EnumerateLetters(uint8_t *lettersOut)

{
  uint32_t logicalDriveMask;
  uint32_t enumeratedDriveCount;
  uint8_t currentDriveLetter;
  int driveLettersRemaining;
  DriveLetterEnumeration enumerationResult;

  logicalDriveMask = GetLogicalDrives();
  enumeratedDriveCount = 0;
  driveLettersRemaining = 'Z' - 'A' + 1;
  currentDriveLetter = 'A';
  do {
    if ((logicalDriveMask & 1) != 0) {
      *lettersOut = currentDriveLetter;
      enumeratedDriveCount++;
      lettersOut++;
    }
    logicalDriveMask = logicalDriveMask >> 1;
    currentDriveLetter++;
    driveLettersRemaining--;
  } while (driveLettersRemaining != 0);
  enumerationResult.driveCountMirror = enumeratedDriveCount;
  enumerationResult.driveCount = enumeratedDriveCount;
  return enumerationResult;
}


/* Address: 0x00576790.
   Checks that an ANSI path is made of DOS 8.3 names (letters, digits and characters below ','), with an
   optional "X:" drive and leading '\'. FILESYSTEM_DOS83_ALLOW_WILDCARDS permits '*' and '?';
   FILESYSTEM_DOS83_COMPONENT_ONLY checks one name only, and FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION lets
   that name end at a '\'. CF clear (false) means valid, CF set (true) rejected.
*/
bool Win32Path_ValidateDos83(FileSystemDos83ValidationFlags flags,uint8_t *pathAnsi)

{
  uint8_t pathChar;
  int charsRemaining;
  uint8_t *previousCursor;
  bool componentRejected;

  if ((flags & FILESYSTEM_DOS83_COMPONENT_ONLY) == 0) {
    if (pathAnsi[1] == ':') {
      pathChar = *pathAnsi;
      if (pathChar < 'A') {
        return true;
      }
      if ('z' < pathChar) {
        return true;
      }
      if ((pathChar < 'a') && ('Z' < pathChar)) {
        return true;
      }
      pathAnsi = pathAnsi + 2;
    }
    if (*pathAnsi == '\\') {
      pathAnsi++;
    }
    /* check each '\'-separated name until the terminator */
    while (componentRejected = Win32Path_ValidateDos83
                             (flags | (FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION|
                                      FILESYSTEM_DOS83_COMPONENT_ONLY),pathAnsi), !componentRejected) {
      /* skip past the next '\'; the terminator ends a valid path */
      while (pathChar = *pathAnsi, pathAnsi++, pathChar != '\\') {
        if (pathChar == 0) {
          return false;
        }
      }
    }
  }
  else {
    /* base name: up to 8 characters, ended by '-', '.', '\' or the terminator */
    charsRemaining = DOS83_BASE_NAME_MAX_CHARS;
    do {
      pathChar = *pathAnsi;
      if (pathChar == 0) break;
      if (pathChar == '*') {
        pathAnsi++;
        if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
          return true;
        }
        break;
      }
      if (',' < pathChar) {
        if (pathChar < '/') break;
        if (pathChar == '/') {
          return true;
        }
        if ('9' < pathChar) {
          if (pathChar == '?') {
            if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
              return true;
            }
          }
          else {
            if (pathChar < 'A') {
              return true;
            }
            if ('Z' < pathChar) {
              if (pathChar == '\\') break;
              if (pathChar < 'a') {
                return true;
              }
              if ('z' < pathChar) {
                return true;
              }
            }
          }
        }
      }
      pathAnsi++;
      charsRemaining--;
    } while (charsRemaining != 0);
    if (charsRemaining != DOS83_BASE_NAME_MAX_CHARS) {
      /* optional extension: '.' and up to 3 characters */
      pathChar = *pathAnsi;
      charsRemaining = DOS83_EXTENSION_MAX_CHARS;
      if (pathChar != 0) {
        if (pathChar == '.') {
          do {
            previousCursor = pathAnsi;
            pathAnsi = previousCursor + 1;
            pathChar = *pathAnsi;
            if (pathChar == 0) {
              return false;
            }
            if (pathChar == '*') {
              if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                return true;
              }
              break;
            }
            if (',' < pathChar) {
              if (pathChar < '0') {
                return true;
              }
              if ('9' < pathChar) {
                if (pathChar == '?') {
                  if ((flags & FILESYSTEM_DOS83_ALLOW_WILDCARDS) == 0) {
                    return true;
                  }
                }
                else {
                  if (pathChar < 'A') {
                    return true;
                  }
                  if ('Z' < pathChar) {
                    if (pathChar == '\\') {
                      return false;
                    }
                    if (pathChar < 'a') {
                      return true;
                    }
                    if ('z' < pathChar) {
                      return true;
                    }
                  }
                }
              }
            }
            charsRemaining--;
          } while (charsRemaining != 0);
          pathChar = previousCursor[2];
          if (pathChar == 0) {
            return false;
          }
        }
        if ((flags & FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION) == 0) {
          return true;
        }
        if (pathChar != '\\') {
          return true;
        }
      }
      return false;
    }
  }
  return true;
}


/* Address: 0x00576910.
   Fills outputRecords with 0x200-byte UTF-16 name records: the files (FILESYSTEM_ENUMERATE_FILES) or
   subdirectories (FILESYSTEM_ENUMERATE_DIRECTORIES) matching the wildcard path, sorted by a bubble sort,
   or the single volume label of a drive (FILESYSTEM_ENUMERATE_VOLUME_LABEL, pathOrVolumeText is then the
   ANSI "X:\"). Entries that no longer fit are skipped. Returns the record size in EAX and the record
   count in ECX, always with CF clear (an unreadable path or drive yields 0 records).
*/
DirectoryEnumerationResult Win32FileSystem_EnumerateDirectoryOrVolumeEntries
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText)

{
  HANDLE findHandle;
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
    g_Win32DriveRootPathScratchA[0] = *pathOrVolumeText; /* the drive letter of the "X:\" root path scratch */
    apiSucceeded = GetVolumeInformationA
                      (g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratchA,128,
                       NULL,NULL,NULL,NULL,0);
    if (apiSucceeded == 0) {
      enumerationResult.recordSizeBytes = FILESYSTEM_ENUMERATION_RECORD_BYTES;
      enumerationResult.entryCount = 0;
      enumerationResult.failed = false;
      return enumerationResult;
    }
    recordCount = 0;
    /* the check allows 0x100 bytes, but the copy may write a whole 0x200-byte record */
    if (255 < outputCapacityBytes) {
      Text_CopyNarrowToUtf16(FILESYSTEM_ENUMERATION_RECORD_BYTES,(uint16_t *)outputRecords,g_Win32PathScratchA);
      volumeResult.entryCount = 1;
      volumeResult.recordSizeBytes = FILESYSTEM_ENUMERATION_RECORD_BYTES;
      volumeResult.failed = false;
      return volumeResult;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)pathOrVolumeText);
    RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,
                                       (uint16_t *)pathOrVolumeText);
    /* the WIN32_FIND_DATAA lands in the file-time scratch block (dwFileAttributes first) */
    findHandle = FindFirstFileA((LPCSTR)g_Win32PathScratchA,
                                (LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    if (findHandle == INVALID_HANDLE_VALUE) {
      enumerationResult.recordSizeBytes = FILESYSTEM_ENUMERATION_RECORD_BYTES;
      enumerationResult.entryCount = 0;
      enumerationResult.failed = false;
      return enumerationResult;
    }
    recordCount = 0;
    destination = (uint16_t *)outputRecords;
    do {
      /* files: neither directory nor volume label; directories: not "." or ".." */
      if ((mode == FILESYSTEM_ENUMERATE_FILES) ?
          ((g_Win32FileCreationTimeOrDosDateScratch & (FILE_ATTRIBUTE_DIRECTORY | FILESYSTEM_ATTRIBUTE_VOLUME_LABEL)) == 0) :
          ((mode == FILESYSTEM_ENUMERATE_DIRECTORIES) &&
           ((g_Win32FileCreationTimeOrDosDateScratch & FILE_ATTRIBUTE_DIRECTORY) != 0) &&
           ((g_Win32FindDataFileNameA != '.') ||
            ((g_Win32FindDataFileNameSecondCharA != '\0') &&
             ((g_Win32FindDataFileNameSecondCharA != '.') || (g_Win32FindDataFileNameThirdCharA != '\0')))))) {
        if (FILESYSTEM_ENUMERATION_RECORD_BYTES - 1 < outputCapacityBytes) {
          Text_CopyNarrowToUtf16(FILESYSTEM_ENUMERATION_RECORD_BYTES,destination,
                                 (uint8_t *)&g_Win32FindDataFileNameA);
          destination = destination + FILESYSTEM_ENUMERATION_RECORD_BYTES / 2;
          recordCount++;
          outputCapacityBytes = outputCapacityBytes - FILESYSTEM_ENUMERATION_RECORD_BYTES;
        }
      }
      apiSucceeded = FindNextFileA(findHandle,(LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
    } while (apiSucceeded != 0);
    FindClose(findHandle);
    /* bubble sort; each swap goes through g_Win32PathScratchA, which is only 0x100 bytes: the 0x200-byte
       record also fills g_Win32PathScratchB behind it (original behaviour; relies on B directly following A,
       0x00575A9C/0x00575B9C, which the generated image struct g_ImageData_0057594C keeps) */
    if (1 < recordCount) {
      comparisonsRemaining = recordCount - 1;
      rightRecordDwords = (uint32_t *)(outputRecords + FILESYSTEM_ENUMERATION_RECORD_BYTES);
      leftRecordDwords = (uint32_t *)outputRecords;
      passesRemaining = comparisonsRemaining;
      do {
        do {
          compareFlags = Utf16String_CompareAsciiCaseInsensitiveFlags
                            ((uint16_t *)rightRecordDwords,(uint16_t *)leftRecordDwords);
          if (!compareFlags.less && !compareFlags.equal) {
            copySource = rightRecordDwords;
            copyDestination = (uint32_t *)g_Win32PathScratchA;
            for (dwordsRemaining = FILESYSTEM_ENUMERATION_RECORD_BYTES / 4; dwordsRemaining != 0;
                 dwordsRemaining--) {
              *copyDestination = *copySource;
              copySource++;
              copyDestination++;
            }
            copySource = leftRecordDwords;
            copyDestination = rightRecordDwords;
            for (dwordsRemaining = FILESYSTEM_ENUMERATION_RECORD_BYTES / 4; dwordsRemaining != 0;
                 dwordsRemaining--) {
              *copyDestination = *copySource;
              copySource++;
              copyDestination++;
            }
            copySource = (uint32_t *)g_Win32PathScratchA;
            copyDestination = leftRecordDwords;
            for (dwordsRemaining = FILESYSTEM_ENUMERATION_RECORD_BYTES / 4; dwordsRemaining != 0;
                 dwordsRemaining--) {
              *copyDestination = *copySource;
              copySource++;
              copyDestination++;
            }
          }
          rightRecordDwords = rightRecordDwords + FILESYSTEM_ENUMERATION_RECORD_BYTES / 4;
          leftRecordDwords = leftRecordDwords + FILESYSTEM_ENUMERATION_RECORD_BYTES / 4;
          comparisonsRemaining--;
        } while (comparisonsRemaining != 0);
        comparisonsRemaining = passesRemaining - 1;
        rightRecordDwords = (uint32_t *)(outputRecords + FILESYSTEM_ENUMERATION_RECORD_BYTES);
        leftRecordDwords = (uint32_t *)outputRecords;
        passesRemaining = comparisonsRemaining;
      } while (comparisonsRemaining != 0);
    }
  }
  enumerationResult.entryCount = recordCount;
  enumerationResult.recordSizeBytes = FILESYSTEM_ENUMERATION_RECORD_BYTES;
  enumerationResult.failed = false;
  return enumerationResult;
}


/* Address: 0x00576020.
   Reads exactly byteCount bytes from a file and returns the count, CF clear; CF set with
   FATAL_ERROR_FILE_READ_FAILED when fewer bytes arrive (end of file or read error).
*/
Win32FileReadResult Win32File_ReadExact(FileIoByteCount byteCount,void *destination,void *handle)

{
  Win32FileReadResult successResult;
  Win32FileReadResult failureResult;

  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,NULL);
  if (g_Win32FileBytesTransferred == byteCount) {
    successResult.failed = false;
    successResult.valueOrError = g_Win32FileBytesTransferred;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_FILE_READ_FAILED;
  return failureResult;
}


/* Address: 0x00576100.
   Returns the size of a file (low 32 bits), CF clear; CF set with 0 when GetFileSize fails.
*/
Win32FileSizeResult Win32File_GetSize(void *handle)

{
  DWORD fileSize;
  Win32FileSizeResult successResult;
  Win32FileSizeResult failureResult;

  fileSize = GetFileSize(handle,NULL);
  if (fileSize != INVALID_FILE_SIZE) {
    successResult.failed = false;
    successResult.sizeOrError = fileSize;
    return successResult;
  }
  failureResult.sizeOrError = 0;
  failureResult.failed = true;
  return failureResult;
}


/* Address: 0x00576430.
   Stores the current directory as UTF-16 into destination (0x200 bytes), CF clear. When
   GetCurrentDirectoryA fails, destination becomes an empty string and CF is set with 0.
*/
StatusResult Win32File_GetCurrentDirectory(uint16_t *destination)

{
  DWORD narrowPathLength;
  StatusResult copyResult;
  uint32_t bytesWritten;

  narrowPathLength = GetCurrentDirectoryA(sizeof g_Win32PathScratchA - 1,(LPSTR)g_Win32PathScratchA);
  if (narrowPathLength != 0) {
    bytesWritten = Text_CopyNarrowToUtf16(WIDE_PATH_MAX_CODE_UNITS * sizeof(uint16_t),destination,g_Win32PathScratchA);
    /* Original quirk: the copy's EAX is passed on with CF cleared, so a cut-off path "succeeds" with
       FATAL_ERROR_GENERAL_FAILURE as its value. */
    copyResult.valueOrError = bytesWritten != 0 ? bytesWritten : FATAL_ERROR_GENERAL_FAILURE;
    copyResult.failed = false;
    return copyResult;
  }
  destination[0] = 0;
  destination[1] = 0;
  copyResult.valueOrError = 0;
  copyResult.failed = true;
  return copyResult;
}


/* Address: 0x00576490.
   Changes the current directory; CF set with FATAL_ERROR_SET_DIRECTORY_FAILED on failure.
*/
StatusResult Win32File_SetCurrentDirectory(uint16_t *path)

{
  BOOL changedDirectory;
  StatusResult successResult;
  StatusResult failureResult;

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,path);
  changedDirectory = SetCurrentDirectoryA((LPCSTR)g_Win32PathScratchA);
  if (changedDirectory != 0) {
    successResult.failed = false;
    successResult.valueOrError = changedDirectory;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_SET_DIRECTORY_FAILED;
  return failureResult;
}


/* Address: 0x00576650.
   Classifies a drive for the engine: ENGINE_DRIVE_REMOVABLE, ENGINE_DRIVE_REMOTE, ENGINE_DRIVE_CDROM, or
   ENGINE_DRIVE_OTHER for fixed, RAM-disk and unknown drives.
*/
EngineDriveTypeCode Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter)

{
  UINT win32DriveType;

  g_Win32DriveRootPathScratchA[0] = (char)driveLetter; /* "X:\" root path scratch */
  win32DriveType = GetDriveTypeA(g_Win32DriveRootPathScratchA);
  if (win32DriveType == DRIVE_REMOVABLE) {
    return ENGINE_DRIVE_REMOVABLE;
  }
  if (DRIVE_NO_ROOT_DIR < win32DriveType) {
    if (win32DriveType == DRIVE_REMOTE) {
      return ENGINE_DRIVE_REMOTE;
    }
    /* only DRIVE_CDROM (5) is left in this range */
    if ((DRIVE_FIXED < win32DriveType) && (win32DriveType < DRIVE_RAMDISK)) {
      return ENGINE_DRIVE_CDROM;
    }
  }
  return ENGINE_DRIVE_OTHER;
}


/* Address: 0x00575F60.
   Opens a file (path recorded in g_PackageLastErrorPath for error messages). The FileSystemOpenFlags
   select the creation mode (create/truncate, open-or-create, open existing), sharing and access; files
   are always opened write-through. Returns the handle with CF clear, or CF set with
   FATAL_ERROR_FILE_ACCESS_FAILED.
*/
Win32FileOpenResult Win32File_Open(FileSystemOpenFlags openFlags,uint16_t *path)

{
  HANDLE fileHandle;
  Win32FileOpenResult successResult;
  Win32FileOpenResult failureResult;
  DWORD desiredAccess;
  DWORD shareMode;
  DWORD creationDisposition;

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratchA,g_Win32PathScratchA,path);
  if ((openFlags & FILESYSTEM_OPEN_CREATE_OR_TRUNCATE) == 0) {
    if ((openFlags & FILESYSTEM_OPEN_EXISTING_OR_CREATE) == 0) {
      creationDisposition = OPEN_EXISTING;
    }
    else {
      creationDisposition = OPEN_ALWAYS;
    }
  }
  else {
    creationDisposition = CREATE_ALWAYS;
  }
  if ((openFlags & FILESYSTEM_OPEN_EXCLUSIVE_SHARE) == 0) {
    if ((openFlags & FILESYSTEM_OPEN_CREATE_OR_TRUNCATE) == 0) {
      shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    }
    else {
      shareMode = FILE_SHARE_READ;
    }
  }
  else {
    shareMode = 0;
  }
  if ((openFlags & (FILESYSTEM_OPEN_WRITE_ACCESS|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE)) == 0) {
    desiredAccess = GENERIC_READ;
  }
  else {
    desiredAccess = GENERIC_READ | GENERIC_WRITE;
  }
  fileHandle = CreateFileA((LPCSTR)g_Win32PathScratchA,desiredAccess,shareMode,NULL,creationDisposition,
                           FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_NORMAL,NULL);
  if (fileHandle != INVALID_HANDLE_VALUE) {
    successResult.failed = false;
    successResult.handleOrError = (uint32_t)fileHandle;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.handleOrError = FATAL_ERROR_FILE_ACCESS_FAILED;
  return failureResult;
}


/* Address: 0x00576000.
   Closes a file handle.
*/
void Win32File_Close(void *handle)

{
  CloseHandle(handle);
  return;
}

