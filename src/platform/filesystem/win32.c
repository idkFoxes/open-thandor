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

/* Copies the zero-terminated name at the start of each of the entryCount enumeration records into the
   string area behind the table's pointer array (stringBytesLeft bytes) and points table[i] at it.
   Returns false as soon as a copied code unit leaves 2 bytes or less of the area (so a full fit fails
   too; the unit is still written); otherwise true with the end of the strings in *outStringEnd. */
static bool FileSystem_CopyRecordNamesIntoTable
          (uint16_t **table,uint32_t entryCount,const uint8_t *records,uint32_t stringBytesLeft,
          uint8_t **outStringEnd)

{
  uint16_t *stringCursor;
  const uint16_t *sourceCodeUnit;
  uint16_t codeUnit;
  uint32_t entryIndex;

  stringCursor = (uint16_t *)(table + entryCount);
  for (entryIndex = 0; entryIndex < entryCount; entryIndex++) {
    table[entryIndex] = stringCursor;
    sourceCodeUnit = (const uint16_t *)(records + entryIndex * FILESYSTEM_ENUMERATION_RECORD_BYTES);
    do {
      codeUnit = *sourceCodeUnit;
      *stringCursor = codeUnit;
      sourceCodeUnit++;
      stringCursor++;
      if (stringBytesLeft <= 2) {
        return false;
      }
      stringBytesLeft -= 2;
    } while (codeUnit != 0);
  }
  *outStringEnd = (uint8_t *)stringCursor;
  return true;
}

/* Address: 0x0040F430.
   Lists a directory (or a drive's volume label) as a compact string table: the fixed-size name records of
   g_FileSystemEnumerateDirectoryOrVolumeEntries are collected in the largest free arena block, then copied
   into a second largest block as an array of entryCount UTF-16 string pointers followed by the strings, which
   is shrunk to its used size. Returns true with the table in *outTable and the entry count in *outEntryCount;
   an empty listing stores NULL and 0. Returns false (outputs untouched) when an arena block cannot be had or
   the strings do not fit; the original left only scratch values in EAX/ECX then. No caller
   in the recovered code (reached only through the function map).
*/
bool FileSystem_BuildEnumerationStringTable
          (FileSystemEnumerationMode enumerationMode,uint32_t reserved,uint8_t *pathOrVolumeText,
          uint16_t ***outTable,uint32_t *outEntryCount)

{
  uint8_t *recordBuffer;
  uint32_t recordBufferBytes;
  uint16_t **table;
  uint32_t tableBlockBytes;
  uint32_t pointerArrayBytes;
  uint32_t entryCount;
  uint8_t *stringEnd;

  if (g_MemoryApi.allocLargestFreeBlock((void **)&recordBuffer,&recordBufferBytes) != 0) {
    return false;
  }
  entryCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
                (enumerationMode,reserved,recordBufferBytes,recordBuffer,pathOrVolumeText);
  if (entryCount == 0) {
    g_MemoryApi.free(recordBuffer);
    *outTable = NULL;
    *outEntryCount = 0;
    return true;
  }
  /* give the unused tail of the record buffer back before taking the next largest block */
  if ((g_MemoryApi.shrinkInPlace(entryCount * FILESYSTEM_ENUMERATION_RECORD_BYTES,recordBuffer) == 0) &&
      (g_MemoryApi.allocLargestFreeBlock((void **)&table,&tableBlockBytes) == 0)) {
    /* the pointer array must fit with room to spare for the strings behind it */
    pointerArrayBytes = entryCount * 4;
    if ((pointerArrayBytes <= tableBlockBytes) && (tableBlockBytes - pointerArrayBytes != 0) &&
        FileSystem_CopyRecordNamesIntoTable
                  (table,entryCount,recordBuffer,tableBlockBytes - pointerArrayBytes,&stringEnd)) {
      g_MemoryApi.shrinkInPlace((uint32_t)(stringEnd - (uint8_t *)table),table);
      g_MemoryApi.free(recordBuffer);
      *outEntryCount = entryCount;
      *outTable = table;
      return true;
    }
    g_MemoryApi.free(table);
  }
  g_MemoryApi.free(recordBuffer);
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
  uint32_t openError;
  uint32_t engineMountResult; /* the mount stores engine.pck's handle, or its error code on failure */

  /* open-thandor: the original took the executable path from the first command-line token, which
     is only a bare "thandor.exe" when started from a shell or batch file; the executable
     directory then came out empty. Use the module path instead. */
  Thandor_GetExecutablePathA((char *)g_Win32PathScratch[0],sizeof g_Win32PathScratch[0]);
  Text_CopyNarrowToUtf16(sizeof g_PackageLastErrorPath,g_PackageLastErrorPath,g_Win32PathScratch[0]);
  /* the leaf (the executable name) lands in the path scratch buffer, which is reused as UTF-16 */
  WidePath_SplitParentAndLeaf
            ((uint16_t *)g_Win32PathScratch[0],g_ExecutableDirectoryUtf16,g_PackageLastErrorPath);
  g_FileSystemOpen = Win32File_Open;
  g_FileSystemClose = Win32File_Close;
  g_FileSystemReadExact = Win32File_ReadExact;
  g_FileSystemWriteExactOrFlush = Win32File_WriteExactOrFlush;
  g_FileSystemGetSize = Win32File_GetSize;
  /* the generated slot type of Delete returns only EAX; callers cast it back to read CF */
  g_FileSystemGetPosition = Win32File_GetPosition;
  g_FileSystemSeek = Win32File_Seek;
  g_FileSystemDelete = Win32File_Delete;
  g_FileSystemGetCurrentDirectory = Win32File_GetCurrentDirectory;
  g_FileSystemSetCurrentDirectory = Win32File_SetCurrentDirectory;
  g_FileSystemRemoveDirectory = Win32File_RemoveDirectory;
  g_FileSystemCreateDirectoryRecursive = Win32File_CreateDirectoryRecursive;
  g_FileSystemEnumerateDriveLetters = Win32Drive_EnumerateLetters;
  g_FileSystemGetDriveTypeCode = Win32Drive_GetEngineTypeCode;
  g_FileSystemCheckDriveMediaReady = Win32Drive_CheckMediaReady;
  g_FileSystemGetFreeAndTotalBytes = Win32Drive_GetFreeAndTotalBytes;
  g_FileSystemGetLastWriteDosDate = Win32File_GetLastWriteDosDate;
  g_FileSystemGetLastWriteTimeHigh = Win32File_GetLastWriteTimeHigh;
  g_FileSystemGetVolumeSerialNumber = Win32Drive_GetVolumeSerialNumber;
  g_FileSystemMove = Win32File_Move;
  g_FileSystemCopy = Win32File_Copy;
  g_FileSystemEnumerateDirectoryOrVolumeEntries =
       Win32FileSystem_EnumerateDirectoryOrVolumeEntries;
  g_FileSystemValidateDos83Path = Win32Path_ValidateDos83;
  /* GetComputerNameA size in/out; the same global later holds the THANDOR.cfg text */
  g_FileSystemInitComputerNameCapacityOrConfigCursor = (pointer)sizeof g_Win32PathScratch[0];
  gotComputerName = GetComputerNameA((LPSTR)g_Win32PathScratch[0],
                                     (LPDWORD)&g_FileSystemInitComputerNameCapacityOrConfigCursor);
  if (gotComputerName != 0) {
    labelCursor = g_DefaultComputerLabelUtf16;
    for (clearCount = sizeof g_DefaultComputerLabelUtf16 / 4; clearCount != 0; clearCount--) {
      labelCursor[0] = 0;
      labelCursor[1] = 0;
      labelCursor += 2;
    }
    Text_CopyNarrowToUtf16(sizeof g_DefaultComputerLabelUtf16,g_DefaultComputerLabelUtf16,g_Win32PathScratch[0]);
  }
  if (ArenaHeap_Alloc(PACKAGE_SCRATCH_BUFFER_BYTES,(void **)&g_PackageScratchBuffer) != 0) {
    FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),true);
  }
  openError = Win32File_Open(0,u_THANDOR_cfg_0040e23d,&configFile);
  if (openError != 0) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,u_THANDOR_cfg_0040e23d,
               g_ExecutableDirectoryUtf16);
    openError = Win32File_Open(0,g_FileSystemCombinedPathScratchUtf16,&configFile);
  }
  if (openError == 0) {
    if (Win32File_GetSize(configFile,&configBytesLeft) && (configBytesLeft != 0)) {
      if (ArenaHeap_Alloc(configBytesLeft,(void **)&configCursor) == 0) {
        if (Win32File_ReadExact(configBytesLeft,configCursor,configFile) != 0) {
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
  if (Package_MountLowPriority(u_engine_pck_0040e255,&engineMountResult)) {
    g_EnginePackageLowPriorityMountHandle = engineMountResult;
  }
  return engineMountResult;
}


/* Address: 0x005762F0.
   Stores the last-write time of a file as a packed DOS date and time (date in the high word, time in
   the low word) in *outDosDateTime and returns 0. Returns FATAL_ERROR_FILE_ACCESS_FAILED when the file
   cannot be opened or its time cannot be read (*outDosDateTime is then left untouched).
*/
uint32_t Win32File_GetLastWriteDosDate(uint16_t *path,uint32_t *outDosDateTime)

{
  BOOL gotFileTime;
  HANDLE fileHandle;
  uint32_t statusCode;

  statusCode = Win32File_Open(0,path,&fileHandle);
  if ((statusCode == 0) && (fileHandle != INVALID_HANDLE_VALUE)) {
    gotFileTime =
         GetFileTime(fileHandle,NULL,NULL,(LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(fileHandle);
    statusCode = FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTime != 0) {
      /* FAT date into the high word, FAT time into the low word of the scratch dword */
      FileTimeToDosDateTime
                ((FILETIME *)&g_Win32FileLastWriteTimeScratch,
                 (LPWORD)&g_Win32FileCreationTimeOrDosDateScratch + 1,
                 (LPWORD)&g_Win32FileCreationTimeOrDosDateScratch);
      *outDosDateTime = g_Win32FileCreationTimeOrDosDateScratch;
      return 0;
    }
  }
  return statusCode;
}


/* Address: 0x00576360.
   Stores the high dword of a file's last-write FILETIME (a coarse modification stamp, about 7 minutes
   per step) in *outLastWriteTimeHigh and returns 0. Returns FATAL_ERROR_FILE_ACCESS_FAILED when the file
   cannot be opened or its time cannot be read (*outLastWriteTimeHigh is then left untouched).
*/
uint32_t Win32File_GetLastWriteTimeHigh(uint16_t *path,uint32_t *outLastWriteTimeHigh)

{
  BOOL gotFileTime;
  HANDLE fileHandle;
  uint32_t statusCode;

  statusCode = Win32File_Open(0,path,&fileHandle);
  if (statusCode == 0) {
    gotFileTime =
         GetFileTime(fileHandle,NULL,NULL,(LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(fileHandle);
    statusCode = FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTime != 0) {
      *outLastWriteTimeHigh = g_Win32FileLastWriteTimeHighScratch;
      return 0;
    }
  }
  return statusCode;
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
  HANDLE fileHandle;
  uint32_t lastWriteTimeHigh;
  uint32_t statusCode;

  statusCode = Win32File_Open(0,(uint16_t *)path,&fileHandle);
  if (statusCode == 0) {
    gotFileTimes =
         GetFileTime(fileHandle,(LPFILETIME)&g_Win32FileCreationTimeOrDosDateScratch,
                     (LPFILETIME)&g_Win32FileLastAccessTimeScratch,
                     (LPFILETIME)&g_Win32FileLastWriteTimeScratch);
    Win32File_Close(fileHandle);
    statusCode = FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTimes != 0) {
      lastWriteTimeHigh = g_Win32FileLastWriteTimeHighScratch;
      *outputLabel = 0;
      return lastWriteTimeHigh;
    }
  }
  return statusCode;
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


/* The whole-file load shared by FileSystem_LoadWholeFile and FileSystem_LoadWholeFileAlternatePath
   (see there). */
static bool FileSystem_LoadWholeFileNearExecutable(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError)

{
  void *handle;
  uint32_t fileSize;
  uint32_t loadError;
  uint32_t openError;
  void *fileBuffer;

  /* first try the path relative to the executable directory */
  WidePath_CombineDirectoryAndLeaf
            (g_FileSystemCombinedPathScratchUtf16,pathUtf16,
             g_ExecutableDirectoryUtf16);
  openError = g_FileSystemOpen(0,g_FileSystemCombinedPathScratchUtf16,&handle);
  if (openError != 0) {
    openError = g_FileSystemOpen(0,pathUtf16,&handle);
    if (openError != 0) {
      *outError = openError;
      return false;
    }
  }
  if (!g_FileSystemGetSize(handle,&fileSize)) {
    loadError = fileSize; /* a failed size query stores 0 there, which becomes the error code */
  }
  else if (g_MemoryApi.alloc(fileSize,&fileBuffer) != 0) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)fileSize,g_FatalErrorDetail1Utf16);
    loadError = FATAL_ERROR_OUT_OF_MEMORY;
  }
  else {
    loadError = g_FileSystemReadExact((FileIoByteCount)fileSize,fileBuffer,handle);
    if (loadError == 0) {
      g_FileSystemClose(handle);
      *outBuffer = fileBuffer;
      return true;
    }
    g_MemoryApi.free(fileBuffer);
  }
  g_FileSystemClose(handle);
  *outError = loadError;
  return false;
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
  return FileSystem_LoadWholeFileNearExecutable(pathUtf16,outBuffer,outError);
}

/* Address: 0x0040F120.
   Same whole-file load as FileSystem_LoadWholeFile (same search order and errors); the only difference in
   the original is that ECX is not preserved: it returns the file size there. Same C interface too: true
   with the buffer in *outBuffer, or false with the error in *outError. No caller in the recovered code
   (reached only through the function map).
*/
bool FileSystem_LoadWholeFileAlternatePath(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError)

{
  return FileSystem_LoadWholeFileNearExecutable(pathUtf16,outBuffer,outError);
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
  uint32_t openError;
  openError = g_FileSystemOpen
                    (FILESYSTEM_OPEN_EXCLUSIVE_SHARE|FILESYSTEM_OPEN_CREATE_OR_TRUNCATE,path,&handle);
  if (openError != 0) {
    return openError;
  }
  writeError = g_FileSystemWriteExactOrFlush(byteCount,source,handle);
  g_FileSystemClose(handle);
  if (writeError == 0) {
    return 0;
  }
  g_FileSystemDelete(1,path); /* the first argument is unused by Win32File_Delete */
  return writeError;
}


/* Address: 0x00576070.
   Writes exactly byteCount bytes to a file; byteCount 0 instead truncates the file at the current
   position (SetEndOfFile). Returns 0 on success, FATAL_ERROR_FILE_WRITE_FAILED when WriteFile fails, or
   FATAL_ERROR_FILE_WRITE_INCOMPLETE when it wrote fewer bytes (disk full). The truncation always
   succeeds: the original passes SetEndOfFile's result on as a success value, which no caller reads.
*/
uint32_t Win32File_WriteExactOrFlush(FileIoByteCount byteCount,void *source,void *handle)

{
  BOOL wroteFile;

  g_Win32FileBytesTransferred = 0;
  if (byteCount == 0) {
    SetEndOfFile(handle);
    return 0;
  }
  wroteFile = WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,NULL);
  if (wroteFile == 0) {
    return FATAL_ERROR_FILE_WRITE_FAILED;
  }
  if (byteCount != g_Win32FileBytesTransferred) {
    return FATAL_ERROR_FILE_WRITE_INCOMPLETE;
  }
  return 0;
}


/* Address: 0x00576140.
   Stores the current position of a file in *outPosition and returns true; returns false with
   *outPosition 0 when SetFilePointer fails (0x0057616D).
*/
bool Win32File_GetPosition(void *handle,uint32_t *outPosition)

{
  DWORD filePosition;

  filePosition = SetFilePointer(handle,0,NULL,FILE_CURRENT);
  if (filePosition != INVALID_SET_FILE_POINTER) {
    *outPosition = filePosition;
    return true;
  }
  *outPosition = 0;
  return false;
}

/* Address: 0x00576180.
   Moves the file pointer (moveMethod is FILESYSTEM_SEEK_BEGIN/CURRENT/END, the Win32 FILE_* values).
   Returns 0 on success, FATAL_ERROR_FILE_SEEK_FAILED on failure. (The original also returned the new
   position on success; no caller uses it.)
*/
uint32_t Win32File_Seek(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle)

{
  DWORD newFilePosition;

  newFilePosition = SetFilePointer(handle,distance,NULL,moveMethod);
  if (newFilePosition != INVALID_SET_FILE_POINTER) {
    return 0;
  }
  return FATAL_ERROR_FILE_SEEK_FAILED;
}


/* Address: 0x005761C0.
   Deletes a file; the first argument is an unused slot of the g_FileSystemDelete interface. Returns 0, or
   FATAL_ERROR_FILE_ACCESS_FAILED when DeleteFileA fails (0x00576202).
*/
uint32_t Win32File_Delete(uint32_t unusedFlags,uint16_t *path)

{
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],path);
  if (DeleteFileA((LPCSTR)g_Win32PathScratch[0]) != 0) {
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}

/* Address: 0x00576210.
   Moves (renames) a file from sourcePath to destinationPath. Returns 0, or FATAL_ERROR_FILE_ACCESS_FAILED
   when MoveFileA fails.
*/
uint32_t Win32File_Move(uint16_t *destinationPath,uint16_t *sourcePath)

{
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[1],g_Win32PathScratch[1],destinationPath);
  if (MoveFileA((LPCSTR)g_Win32PathScratch[0],(LPCSTR)g_Win32PathScratch[1]) != 0) {
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}


/* Address: 0x00576280.
   Copies a file from sourcePath to destinationPath without overwriting an existing destination. Returns
   0, or FATAL_ERROR_FILE_ACCESS_FAILED when CopyFileA fails.
*/
uint32_t Win32File_Copy(uint16_t *destinationPath,uint16_t *sourcePath)

{
  Package_SetLastErrorPath(sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],sourcePath);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[1],g_Win32PathScratch[1],destinationPath);
  if (CopyFileA((LPCSTR)g_Win32PathScratch[0],(LPCSTR)g_Win32PathScratch[1],TRUE /* fail if exists */) != 0) {
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}


/* Address: 0x005764E0.
   Creates a directory. With FILESYSTEM_CREATE_DIRECTORY_RECURSIVE a failed attempt first creates the
   parent directories (recursively) and then retries. Returns 0, or FATAL_ERROR_FILE_WRITE_FAILED when the
   directory cannot be created.
*/
uint32_t Win32File_CreateDirectoryRecursive(FileSystemCreateDirectoryFlags flags,uint16_t *path)

{
  uint16_t parentPath [256];
  uint16_t leafName [248];

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],path);
  if (CreateDirectoryA((LPCSTR)g_Win32PathScratch[0],NULL) != 0) {
    return 0;
  }
  if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
    WidePath_SplitParentAndLeaf(leafName,parentPath,path);
    if (Win32File_CreateDirectoryRecursive(flags,parentPath) == 0) {
      RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],path);
      if (CreateDirectoryA((LPCSTR)g_Win32PathScratch[0],NULL) != 0) {
        return 0; /* created after its parents */
      }
    }
  }
  Package_SetLastErrorPath(path);
  return FATAL_ERROR_FILE_WRITE_FAILED;
}


/* Address: 0x005765A0.
   Removes an (empty) directory. Returns 0, or FATAL_ERROR_REMOVE_DIRECTORY_FAILED when RemoveDirectoryA
   fails. Unlike the other path operations it does not record the path in g_PackageLastErrorPath.
*/
uint32_t Win32File_RemoveDirectory(uint16_t *path)

{
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],path);
  if (RemoveDirectoryA((LPCSTR)g_Win32PathScratch[0]) != 0) {
    return 0;
  }
  return FATAL_ERROR_REMOVE_DIRECTORY_FAILED;
}


/* Address: 0x005765F0.
   Returns the free and total bytes of a drive, both 0 when the query fails. The products are 32-bit, so
   drives above 4 GiB wrap.
*/
Win32DriveCapacity Win32Drive_GetFreeAndTotalBytes(DosDriveLetterCode32 driveLetter)

{
  BOOL gotDiskSpace;
  int freeBytes;
  int totalBytes;
  Win32DriveCapacity capacity;

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
  capacity.freeBytes = (uint32_t)freeBytes;
  capacity.totalBytes = (uint32_t)totalBytes;
  return capacity;
}

/* Address: 0x005766B0.
   Lists the existing drives: writes one letter 'A'..'Z' per set bit of GetLogicalDrives to lettersOut
   (no terminator) and returns the number of letters.
*/
uint32_t Win32Drive_EnumerateLetters(uint8_t *lettersOut)

{
  uint32_t logicalDriveMask;
  uint32_t enumeratedDriveCount;
  uint8_t currentDriveLetter;
  int driveLettersRemaining;

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
  return enumeratedDriveCount;
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
    while (!Win32Path_ValidateDos83
              (flags | (FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION|FILESYSTEM_DOS83_COMPONENT_ONLY),
               pathAnsi)) {
      /* skip past the next '\'; the terminator ends a valid path */
      do {
        pathChar = *pathAnsi;
        pathAnsi++;
        if (pathChar == 0) {
          return false;
        }
      } while (pathChar != '\\');
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
    if (charsRemaining == DOS83_BASE_NAME_MAX_CHARS) {
      return true; /* empty base name (a leading '*' counts as empty too) */
    }
    /* optional extension: '.' and up to 3 characters */
    pathChar = *pathAnsi;
    if (pathChar == 0) {
      return false;
    }
    if (pathChar == '.') {
      for (charsRemaining = DOS83_EXTENSION_MAX_CHARS; charsRemaining != 0; charsRemaining--) {
        pathAnsi++;
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
      }
      /* the character after the last one checked must end the name */
      pathChar = pathAnsi[1];
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
    return false;
  }
  return true;
}


/* Whether the entry FindFirstFileA/FindNextFileA just stored in the scratch WIN32_FIND_DATAA belongs in
   the listing: files are neither directory nor volume label; directories are directories other than
   "." and "..". Any other mode matches nothing. */
static bool Win32FileSystem_FoundEntryMatchesMode(FileSystemEnumerationMode mode)

{
  if (mode == FILESYSTEM_ENUMERATE_FILES) {
    return (g_Win32FileCreationTimeOrDosDateScratch &
            (FILE_ATTRIBUTE_DIRECTORY | FILESYSTEM_ATTRIBUTE_VOLUME_LABEL)) == 0;
  }
  if (mode != FILESYSTEM_ENUMERATE_DIRECTORIES) {
    return false;
  }
  if ((g_Win32FileCreationTimeOrDosDateScratch & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    return false;
  }
  return (g_Win32FindDataFileNameA != '.') ||
         ((g_Win32FindDataFileNameSecondCharA != '\0') &&
          ((g_Win32FindDataFileNameSecondCharA != '.') || (g_Win32FindDataFileNameThirdCharA != '\0')));
}

/* Copies one FILESYSTEM_ENUMERATION_RECORD_BYTES record dword by dword. */
static void Win32FileSystem_CopyEnumerationRecord(uint32_t *destination,const uint32_t *source)

{
  int dwordsRemaining;

  for (dwordsRemaining = FILESYSTEM_ENUMERATION_RECORD_BYTES / 4; dwordsRemaining != 0; dwordsRemaining--) {
    *destination = *source;
    source++;
    destination++;
  }
}

/* Address: 0x00576910.
   Fills outputRecords with 0x200-byte UTF-16 name records: the files (FILESYSTEM_ENUMERATE_FILES) or
   subdirectories (FILESYSTEM_ENUMERATE_DIRECTORIES) matching the wildcard path, sorted by a bubble sort,
   or the single volume label of a drive (FILESYSTEM_ENUMERATE_VOLUME_LABEL, pathOrVolumeText is then the
   ANSI "X:\"). Entries that no longer fit are skipped. Returns the number of records written (an
   unreadable path or drive yields 0 records); the record stride is always
   FILESYSTEM_ENUMERATION_RECORD_BYTES. (The original returned the stride in EAX, the count in ECX and
   always cleared CF.)
*/
uint32_t Win32FileSystem_EnumerateDirectoryOrVolumeEntries
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText)

{
  HANDLE findHandle;
  uint32_t recordCount;
  uint32_t passesRemaining;
  uint32_t comparisonsRemaining;
  uint16_t *destination;
  uint32_t *leftRecordDwords;
  uint32_t *rightRecordDwords;

  if (mode == FILESYSTEM_ENUMERATE_VOLUME_LABEL) {
    g_Win32DriveRootPathScratchA[0] = *pathOrVolumeText; /* the drive letter of the "X:\" root path scratch */
    if (GetVolumeInformationA
          (g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratch[0],128,NULL,NULL,NULL,NULL,0) == 0) {
      return 0;
    }
    /* the check allows 0x100 bytes, but the copy may write a whole 0x200-byte record */
    if (255 < outputCapacityBytes) {
      Text_CopyNarrowToUtf16(FILESYSTEM_ENUMERATION_RECORD_BYTES,(uint16_t *)outputRecords,g_Win32PathScratch[0]);
      return 1;
    }
    return 0;
  }
  Package_SetLastErrorPath((uint16_t *)pathOrVolumeText);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],
                                     (uint16_t *)pathOrVolumeText);
  /* the WIN32_FIND_DATAA lands in the file-time scratch block (dwFileAttributes first) */
  findHandle = FindFirstFileA((LPCSTR)g_Win32PathScratch[0],
                              (LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch);
  if (findHandle == INVALID_HANDLE_VALUE) {
    return 0;
  }
  recordCount = 0;
  destination = (uint16_t *)outputRecords;
  do {
    if (Win32FileSystem_FoundEntryMatchesMode(mode) &&
        (FILESYSTEM_ENUMERATION_RECORD_BYTES - 1 < outputCapacityBytes)) {
      Text_CopyNarrowToUtf16(FILESYSTEM_ENUMERATION_RECORD_BYTES,destination,
                             (uint8_t *)&g_Win32FindDataFileNameA);
      destination = destination + FILESYSTEM_ENUMERATION_RECORD_BYTES / 2;
      recordCount++;
      outputCapacityBytes = outputCapacityBytes - FILESYSTEM_ENUMERATION_RECORD_BYTES;
    }
  } while (FindNextFileA(findHandle,(LPWIN32_FIND_DATAA)&g_Win32FileCreationTimeOrDosDateScratch) != 0);
  FindClose(findHandle);
  /* bubble sort; each swap goes through the whole path scratch block: the 0x200-byte record fills both
     0x100-byte path buffers (original quirk: the original passes the first buffer, 0x00575A9C, and
     overruns into the second, 0x00575B9C) */
  if (1 < recordCount) {
    for (passesRemaining = recordCount - 1; passesRemaining != 0; passesRemaining--) {
      leftRecordDwords = (uint32_t *)outputRecords;
      rightRecordDwords = (uint32_t *)(outputRecords + FILESYSTEM_ENUMERATION_RECORD_BYTES);
      for (comparisonsRemaining = passesRemaining; comparisonsRemaining != 0; comparisonsRemaining--) {
        if (Utf16String_CompareAsciiCaseInsensitiveFlags
              ((uint16_t *)rightRecordDwords,(uint16_t *)leftRecordDwords) > 0) {
          Win32FileSystem_CopyEnumerationRecord((uint32_t *)g_Win32PathScratch,rightRecordDwords);
          Win32FileSystem_CopyEnumerationRecord(rightRecordDwords,leftRecordDwords);
          Win32FileSystem_CopyEnumerationRecord(leftRecordDwords,(uint32_t *)g_Win32PathScratch);
        }
        leftRecordDwords = leftRecordDwords + FILESYSTEM_ENUMERATION_RECORD_BYTES / 4;
        rightRecordDwords = rightRecordDwords + FILESYSTEM_ENUMERATION_RECORD_BYTES / 4;
      }
    }
  }
  return recordCount;
}


/* Address: 0x00576020.
   Reads exactly byteCount bytes from a file. Returns 0 on success, FATAL_ERROR_FILE_READ_FAILED when
   fewer bytes arrive (end of file or read error). (The original returned the byte count on success.)
*/
uint32_t Win32File_ReadExact(FileIoByteCount byteCount,void *destination,void *handle)

{
  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,NULL);
  if (g_Win32FileBytesTransferred == byteCount) {
    return 0;
  }
  return FATAL_ERROR_FILE_READ_FAILED;
}


/* Address: 0x00576100.
   Stores the size of a file (low 32 bits) in *outSize and returns true; returns false with *outSize 0
   when GetFileSize fails (some callers pass that 0 on as their error code).
*/
bool Win32File_GetSize(void *handle,uint32_t *outSize)

{
  DWORD fileSize;

  fileSize = GetFileSize(handle,NULL);
  if (fileSize != INVALID_FILE_SIZE) {
    *outSize = fileSize;
    return true;
  }
  *outSize = 0;
  return false;
}


/* Address: 0x00576430.
   Stores the current directory as UTF-16 into destination (0x200 bytes) and returns true. When
   GetCurrentDirectoryA fails, destination becomes an empty string and the result is false. (The
   original also passed on the copy's byte count, or FATAL_ERROR_GENERAL_FAILURE for a cut-off path, as
   the success value; no caller read it.)
*/
bool Win32File_GetCurrentDirectory(uint16_t *destination)

{
  DWORD narrowPathLength;

  narrowPathLength = GetCurrentDirectoryA(sizeof g_Win32PathScratch[0] - 1,(LPSTR)g_Win32PathScratch[0]);
  if (narrowPathLength != 0) {
    Text_CopyNarrowToUtf16(WIDE_PATH_MAX_CODE_UNITS * sizeof(uint16_t),destination,g_Win32PathScratch[0]);
    return true;
  }
  destination[0] = 0;
  destination[1] = 0;
  return false;
}


/* Address: 0x00576490.
   Changes the current directory. Returns 0, or FATAL_ERROR_SET_DIRECTORY_FAILED when SetCurrentDirectoryA
   fails.
*/
uint32_t Win32File_SetCurrentDirectory(uint16_t *path)

{
  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],path);
  if (SetCurrentDirectoryA((LPCSTR)g_Win32PathScratch[0]) != 0) {
    return 0;
  }
  return FATAL_ERROR_SET_DIRECTORY_FAILED;
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
   are always opened write-through. Returns 0 and stores the handle in *outHandle, or returns
   FATAL_ERROR_FILE_ACCESS_FAILED and leaves *outHandle unchanged.
*/
uint32_t Win32File_Open(FileSystemOpenFlags openFlags,uint16_t *path,void **outHandle)

{
  HANDLE fileHandle;
  DWORD desiredAccess;
  DWORD shareMode;
  DWORD creationDisposition;

  Package_SetLastErrorPath(path);
  RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],g_Win32PathScratch[0],path);
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
  fileHandle = CreateFileA((LPCSTR)g_Win32PathScratch[0],desiredAccess,shareMode,NULL,creationDisposition,
                           FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_NORMAL,NULL);
  if (fileHandle != INVALID_HANDLE_VALUE) {
    *outHandle = fileHandle;
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}


/* Address: 0x00576000.
   Closes a file handle.
*/
void Win32File_Close(void *handle)

{
  CloseHandle(handle);
  return;
}

