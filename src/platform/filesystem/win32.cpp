/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/filesystem/win32.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/filesystem/win32.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(16) uint16_t g_ExecutableDirectoryUtf16[256] = {0};

THANDOR_ALIGN(16) uint16_t g_FileSystemCombinedPathScratchUtf16[THANDOR_PATH_CAPACITY] = {0};

THANDOR_ALIGN(16) FileSystemOpenProc *g_FileSystemOpen = nullptr;

THANDOR_ALIGN(4) FileSystemCloseProc *g_FileSystemClose = nullptr;

THANDOR_ALIGN(8) FileSystemReadExactProc *g_FileSystemReadExact = nullptr;

THANDOR_ALIGN(4) FileSystemWriteExactOrFlushProc *g_FileSystemWriteExactOrFlush = nullptr;

THANDOR_ALIGN(16) FileSystemGetSizeProc *g_FileSystemGetSize = nullptr;

THANDOR_ALIGN(8) FileSystemSeekProc *g_FileSystemSeek = nullptr;

THANDOR_ALIGN(4) FileSystemGetPositionProc *g_FileSystemGetPosition = nullptr;

/* uint8_t[256] byte map applied to every non-separator character of the file-system config text in FileSystem_Init: identity except a-z -> A-Z and the CP437 lowercase accented letters -> their uppercase forms (case folding). */
static const uint8_t g_FileSystemConfigCharacterNormalizationMap[256] = {
    /*   0 */ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    /*  16 */ 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    /*  32 */ 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    /*  48 */ 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    /*  64 */ 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    /*  80 */ 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    /*  96 */ 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    /* 112 */ 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127,
    /* 128 */ 128, 154, 144, 131, 142, 133, 143, 128, 136, 137, 138, 139, 140, 141, 142, 143,
    /* 144 */ 144, 146, 146, 147, 153, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    /* 160 */ 160, 161, 162, 163, 165, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    /* 176 */ 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    /* 192 */ 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    /* 208 */ 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    /* 224 */ 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    /* 240 */ 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

static WidePathBuffer256 g_InitialWorkingDirectory = {0};

static FileSystemGetCurrentDirectoryProc *g_FileSystemGetCurrentDirectory = nullptr;

static FileSystemSetCurrentDirectoryProc *g_FileSystemSetCurrentDirectory = nullptr;

static FileSystemRemoveDirectoryProc *g_FileSystemRemoveDirectory = nullptr;

static FileSystemGetFreeAndTotalBytesRegsProc *g_FileSystemGetFreeAndTotalBytes = nullptr;

static FileSystemGetLastWriteDosDateProc *g_FileSystemGetLastWriteDosDate = nullptr;

static FileSystemGetLastWriteTimeHighProc *g_FileSystemGetLastWriteTimeHigh = nullptr;

static FileSystemGetVolumeSerialNumberProc *g_FileSystemGetVolumeSerialNumber = nullptr;

static FileSystemMoveProc *g_FileSystemMove = nullptr;

static FileSystemCopyProc *g_FileSystemCopy = nullptr;

static uintptr_t g_EnginePackageLowPriorityMountHandle = 0;

static uint16_t g_ThandorCfgPathUtf16[12] = {'T', 'H', 'A', 'N', 'D', 'O', 'R', '.', 'c', 'f', 'g', 0}; /* L"THANDOR.cfg" */

static uint16_t g_EnginePckPathUtf16[11] = {'e', 'n', 'g', 'i', 'n', 'e', '.', 'p', 'c', 'k', 0}; /* L"engine.pck" */

static uint32_t g_Win32FileBytesTransferred = 0;

/* WIN32_FIND_DATAA of the directory enumeration, reused as file-time,
   DOS-date and disk-space scratch */
static _WIN32_FIND_DATAA g_Win32FindDataScratch = {0};

static void *g_FileSystemInitComputerNameCapacityOrConfigCursor = nullptr;

static uint32_t g_FileSystemConfigRemainingBytes = 0;

/* two narrow path buffers ([1] is the second path of move/copy); the directory sort swaps 0x200-byte records
   through the start of the block. open-thandor: THANDOR_PATH_CAPACITY bytes each instead of the original's 0x100,
   and a path that does not fit fails the operation (Win32Path_ToNarrow) instead of going to Windows cut off:
   with a long game directory the cut-off path named a different file or directory. */
static uint8_t g_Win32PathScratch[2][THANDOR_PATH_CAPACITY] = {0};

/* char[4]: "x:\" root path, drive letter patched at [0] before GetDiskFreeSpaceA/GetVolumeInformationA/GetDriveTypeA */
static char g_Win32DriveRootPathScratchA[4] = "x:\\";

FileSystemDeleteProc *g_FileSystemDelete = nullptr;

FileSystemCreateDirectoryRecursiveProc *g_FileSystemCreateDirectoryRecursive = nullptr;

FileSystemEnumerateDriveLettersProc *g_FileSystemEnumerateDriveLetters = nullptr;

FileSystemGetDriveTypeCodeProc *g_FileSystemGetDriveTypeCode = nullptr;

FileSystemDriveReadyProc *g_FileSystemCheckDriveMediaReady = nullptr;

uint16_t g_DefaultComputerLabelUtf16[32] = {'C', 'o', 'm', 'p', 'u', 't', 'e', 'r', 0}; /* L"Computer" */

FileSystemEnumerateDirectoryOrVolumeEntriesProc *g_FileSystemEnumerateDirectoryOrVolumeEntries = nullptr;

FileSystemValidateDos83Proc *g_FileSystemValidateDos83Path = nullptr;

/* Implementation ownership: platform/filesystem/win32. */

/* open-thandor: converts a UTF-16 path into one of the g_Win32PathScratch buffers for the ANSI file APIs.
   False when it does not fit; the original cut the path off at 0xFF bytes and used it anyway. */
static Bool8 Win32Path_ToNarrow(uint8_t *destination,uint16_t *path)

{
  return RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],destination,path);
}

/* Copies the zero-terminated name at the start of each of the entryCount enumeration records into the
   string area behind the table's pointer array (stringBytesLeft bytes) and points table[i] at it.
   Returns false as soon as a copied code unit leaves 2 bytes or less of the area (so a full fit fails
   too; the unit is still written); otherwise true with the end of the strings in *outStringEnd. */
static Bool8 FileSystem_CopyRecordNamesIntoTable
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

/* Lists a directory (or a drive's volume label) as a compact string table: the fixed-size name records of
   g_FileSystemEnumerateDirectoryOrVolumeEntries are collected in the largest free arena block, then copied
   into a second largest block as an array of entryCount UTF-16 string pointers followed by the strings, which
   is shrunk to its used size. Returns true with the table in *outTable and the entry count in *outEntryCount;
   an empty listing stores NULL and 0. Returns false (outputs untouched) when an arena block cannot be had or
   the strings do not fit; the original returned no meaningful results then. Nothing in the game
   calls it.
*/
Bool8 FileSystem_BuildEnumerationStringTable
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
    *outTable = nullptr;
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

/* Starts the file layer: records the executable directory, installs the Win32 implementations of the
   g_FileSystem* function table, replaces the default L"Computer" label with the machine name, allocates the
   8 MiB package scratch buffer, loads THANDOR.cfg (current directory first, then the executable
   directory) and normalizes it in place, remembers the working directory and mounts engine.pck.
   The original never reports failure to its caller; the return value is the result of the engine.pck mount.
*/
uintptr_t __cdecl FileSystem_Init(void)

{
  uint8_t configByte;
  uint8_t *configCursor;
  BOOL gotComputerName;
  void *configFile;
  int clearCount;
  ArenaPayloadByteCount configBytesLeft;
  uint16_t *labelCursor;
  uint32_t openError;
  uintptr_t engineMountResult; /* the mount stores engine.pck's handle, or its error code on failure */

  /* open-thandor: the original took the executable path from the first command-line token, which
     is only a bare "thandor.exe" when started from a shell or batch file; the executable
     directory then came out empty. Use the module path instead. */
  /* an executable path of WIDE_PATH_MAX_CODE_UNITS characters or more leaves the executable directory empty
     (it would not fit g_ExecutableDirectoryUtf16) */
  Thandor_GetExecutablePathA((char *)g_Win32PathScratch[0],WIDE_PATH_MAX_CODE_UNITS);
  Text_CopyNarrowToUtf16(sizeof g_PackageLastErrorPath,g_PackageLastErrorPath,g_Win32PathScratch[0]);
  /* the leaf (the executable name) lands in the path scratch buffer, which is reused as UTF-16 */
  WidePath_SplitParentAndLeaf
            ((uint16_t *)g_Win32PathScratch[0],g_ExecutableDirectoryUtf16,g_PackageLastErrorPath);
  g_FileSystemOpen = Win32File_Open;
  g_FileSystemClose = Win32File_Close;
  g_FileSystemReadExact = Win32File_ReadExact;
  g_FileSystemWriteExactOrFlush = Win32File_WriteExactOrFlush;
  g_FileSystemGetSize = Win32File_GetSize;
  /* the Delete slot returns an error code (0 on success) */
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
  g_FileSystemInitComputerNameCapacityOrConfigCursor = (void *)sizeof g_Win32PathScratch[0];
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
  openError = Win32File_Open(0,g_ThandorCfgPathUtf16,&configFile);
  if (openError != 0) {
    WidePath_CombineDirectoryAndLeaf
              (g_FileSystemCombinedPathScratchUtf16,g_ThandorCfgPathUtf16,
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
  if (Package_MountLowPriority(g_EnginePckPathUtf16,&engineMountResult)) {
    g_EnginePackageLowPriorityMountHandle = engineMountResult;
  }
  return engineMountResult;
}


/* Stores the last-write time of a file as a packed DOS date and time (date in the high word, time in
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
    /* last-write scratch FILETIME at g_Win32FindDataScratch+0x10 (see Win32Drive_GetVolumeSerialNumber) */
    gotFileTime =
         GetFileTime(fileHandle,nullptr,nullptr,(LPFILETIME)&g_Win32FindDataScratch.ftLastAccessTime.dwHighDateTime);
    Win32File_Close(fileHandle);
    statusCode = FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTime != 0) {
      /* FAT date into the high word, FAT time into the low word of the scratch dword (the find data's
         dwFileAttributes slot) */
      FileTimeToDosDateTime
                ((LPFILETIME)&g_Win32FindDataScratch.ftLastAccessTime.dwHighDateTime,
                 (LPWORD)&g_Win32FindDataScratch.dwFileAttributes + 1,
                 (LPWORD)&g_Win32FindDataScratch.dwFileAttributes);
      *outDosDateTime = g_Win32FindDataScratch.dwFileAttributes;
      return 0;
    }
  }
  return statusCode;
}


/* Stores the high dword of a file's last-write FILETIME (a coarse modification stamp, about 7 minutes
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
    /* last-write scratch FILETIME at g_Win32FindDataScratch+0x10 (see Win32Drive_GetVolumeSerialNumber) */
    gotFileTime =
         GetFileTime(fileHandle,nullptr,nullptr,(LPFILETIME)&g_Win32FindDataScratch.ftLastAccessTime.dwHighDateTime);
    Win32File_Close(fileHandle);
    statusCode = FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTime != 0) {
      *outLastWriteTimeHigh = g_Win32FindDataScratch.ftLastWriteTime.dwLowDateTime; /* its dwHighDateTime */
      return 0;
    }
  }
  return statusCode;
}


/* Despite its slot name (g_FileSystemGetVolumeSerialNumber) this queries no volume: it reads all three
   FILETIMEs of the file at path, clears the first byte of outputLabel and returns the high dword of the
   last-write time, like Win32File_GetLastWriteTimeHigh. The original also returns a separate failure flag
   (next to FATAL_ERROR_FILE_ACCESS_FAILED); this C version returns only the value. Nothing calls
   through g_FileSystemGetVolumeSerialNumber (only FileSystem_Init stores it), so the flag is unobservable.
*/
uint32_t Win32Drive_GetVolumeSerialNumber(uint8_t *outputLabel,char *path)

{
  BOOL gotFileTimes;
  HANDLE fileHandle;
  uint32_t lastWriteTimeHigh;
  uint32_t statusCode;

  statusCode = Win32File_Open(0,(uint16_t *)path,&fileHandle);
  if (statusCode == 0) {
    /* the three scratch FILETIMEs are packed from the start of g_Win32FindDataScratch (+0x00, +0x08,
       +0x10), 4 bytes before the find data's own ftCreationTime/ftLastAccessTime/ftLastWriteTime
       (original layout); the last-write high dword thus lands in ftLastWriteTime.dwLowDateTime */
    gotFileTimes =
         GetFileTime(fileHandle,(LPFILETIME)&g_Win32FindDataScratch.dwFileAttributes,
                     (LPFILETIME)&g_Win32FindDataScratch.ftCreationTime.dwHighDateTime,
                     (LPFILETIME)&g_Win32FindDataScratch.ftLastAccessTime.dwHighDateTime);
    Win32File_Close(fileHandle);
    statusCode = FATAL_ERROR_FILE_ACCESS_FAILED;
    if (gotFileTimes != 0) {
      lastWriteTimeHigh = g_Win32FindDataScratch.ftLastWriteTime.dwLowDateTime;
      *outputLabel = 0;
      return lastWriteTimeHigh;
    }
  }
  return statusCode;
}


/* Changes back to the working directory FileSystem_Init found at startup, if one was captured.
*/
void __cdecl Win32FileSystem_RestoreInitialDirectory(void)

{
  if (g_InitialWorkingDirectory.firstTwoCodeUnits != 0) {
    Win32File_SetCurrentDirectory(g_InitialWorkingDirectory.codeUnits);
  }
  return;
}

/* Reports whether a drive has usable media: returns false (ready) for fixed, network and other drives,
   true (not ready) for removable and CD-ROM drives. The original contains an unreachable
   \\.\X: + IOCTL_STORAGE_CHECK_VERIFY probe after the type check, so removable and CD-ROM drives are
   always reported as not ready.
*/
Bool8 Win32Drive_CheckMediaReady(DosDriveLetterCode32 driveLetter)

{
  uint32_t engineDriveType;

  engineDriveType = Win32Drive_GetEngineTypeCode(driveLetter);
  if ((engineDriveType != ENGINE_DRIVE_REMOVABLE) && (engineDriveType != ENGINE_DRIVE_CDROM)) {
    return false;
  }
  return true;
}


/* The whole-file load of FileSystem_LoadWholeFile and FileSystem_LoadWholeFileAlternatePath (see there),
   also used for loose files by Package_LoadEntry and Resource_Load (assets/package). On success also stores
   the file size in *outByteCount (outByteCount may be NULL). */
Bool8 FileSystem_LoadWholeFileNearExecutable(uint16_t *pathUtf16,void **outBuffer,uint32_t *outByteCount,
          uint32_t *outError)

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
      if (outByteCount != nullptr) {
        *outByteCount = fileSize;
      }
      return true;
    }
    g_MemoryApi.free(fileBuffer);
  }
  g_FileSystemClose(handle);
  *outError = loadError;
  return false;
}

/* Reads a whole file into a new arena buffer: the path is tried next to the executable first, then as
   given. Returns true with the buffer in *outBuffer, or false with the open/size/read error in *outError
   (0 when the size query failed), or FATAL_ERROR_OUT_OF_MEMORY with the file size left in
   g_FatalErrorDetail1Utf16. *outBuffer is only written on success, *outError only on failure. Nothing in
   the game calls it.
*/
Bool8 FileSystem_LoadWholeFile(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError)

{
  return FileSystem_LoadWholeFileNearExecutable(pathUtf16,outBuffer,nullptr,outError);
}

/* Same whole-file load as FileSystem_LoadWholeFile (same search order and errors); the only difference in
   the original is that it also returns the file size. Same C interface too: true
   with the buffer in *outBuffer, or false with the error in *outError. Nothing in the game calls it.
*/
Bool8 FileSystem_LoadWholeFileAlternatePath(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError)

{
  return FileSystem_LoadWholeFileNearExecutable(pathUtf16,outBuffer,nullptr,outError);
}

/* Writes a whole buffer to a file, creating or truncating it with exclusive access. A failed write
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


/* Writes exactly byteCount bytes to a file; byteCount 0 instead truncates the file at the current
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
  wroteFile = WriteFile(handle,source,byteCount,&g_Win32FileBytesTransferred,nullptr);
  if (wroteFile == 0) {
    return FATAL_ERROR_FILE_WRITE_FAILED;
  }
  if (byteCount != g_Win32FileBytesTransferred) {
    return FATAL_ERROR_FILE_WRITE_INCOMPLETE;
  }
  return 0;
}


/* Stores the current position of a file in *outPosition and returns true; returns false with
   *outPosition 0 when SetFilePointer fails.
*/
Bool8 Win32File_GetPosition(void *handle,uint32_t *outPosition)

{
  DWORD filePosition;

  filePosition = SetFilePointer(handle,0,nullptr,FILE_CURRENT);
  if (filePosition != INVALID_SET_FILE_POINTER) {
    *outPosition = filePosition;
    return true;
  }
  *outPosition = 0;
  return false;
}

/* Moves the file pointer (moveMethod is FILESYSTEM_SEEK_BEGIN/CURRENT/END, the Win32 FILE_* values).
   Returns 0 on success, FATAL_ERROR_FILE_SEEK_FAILED on failure. (The original also returned the new
   position on success; no caller uses it.)
*/
uint32_t Win32File_Seek(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle)

{
  DWORD newFilePosition;

  newFilePosition = SetFilePointer(handle,distance,nullptr,moveMethod);
  if (newFilePosition != INVALID_SET_FILE_POINTER) {
    return 0;
  }
  return FATAL_ERROR_FILE_SEEK_FAILED;
}


/* Deletes a file; the first argument is an unused slot of the g_FileSystemDelete interface. Returns 0, or
   FATAL_ERROR_FILE_ACCESS_FAILED when DeleteFileA fails.
*/
uint32_t Win32File_Delete(uint32_t unusedFlags,uint16_t *path)

{
  Package_SetLastErrorPath(path);
  if (Win32Path_ToNarrow(g_Win32PathScratch[0],path) && DeleteFileA((LPCSTR)g_Win32PathScratch[0]) != 0) {
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}

/* Moves (renames) a file from sourcePath to destinationPath. Returns 0, or FATAL_ERROR_FILE_ACCESS_FAILED
   when MoveFileA fails.
*/
uint32_t Win32File_Move(uint16_t *destinationPath,uint16_t *sourcePath)

{
  Package_SetLastErrorPath(sourcePath);
  if (!Win32Path_ToNarrow(g_Win32PathScratch[0],sourcePath) ||
      !Win32Path_ToNarrow(g_Win32PathScratch[1],destinationPath)) {
    return FATAL_ERROR_FILE_ACCESS_FAILED;
  }
  if (MoveFileA((LPCSTR)g_Win32PathScratch[0],(LPCSTR)g_Win32PathScratch[1]) != 0) {
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}


/* Copies a file from sourcePath to destinationPath without overwriting an existing destination. Returns
   0, or FATAL_ERROR_FILE_ACCESS_FAILED when CopyFileA fails.
*/
uint32_t Win32File_Copy(uint16_t *destinationPath,uint16_t *sourcePath)

{
  Package_SetLastErrorPath(sourcePath);
  if (!Win32Path_ToNarrow(g_Win32PathScratch[0],sourcePath) ||
      !Win32Path_ToNarrow(g_Win32PathScratch[1],destinationPath)) {
    return FATAL_ERROR_FILE_ACCESS_FAILED;
  }
  if (CopyFileA((LPCSTR)g_Win32PathScratch[0],(LPCSTR)g_Win32PathScratch[1],TRUE /* fail if exists */) != 0) {
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}


/* Creates a directory. With FILESYSTEM_CREATE_DIRECTORY_RECURSIVE a failed attempt first creates the
   parent directories (recursively) and then retries. Returns 0, or FATAL_ERROR_FILE_WRITE_FAILED when the
   directory cannot be created.
*/
uint32_t Win32File_CreateDirectoryRecursive(FileSystemCreateDirectoryFlags flags,uint16_t *path)

{
  uint16_t parentPath [WIDE_PATH_MAX_CODE_UNITS];
  /* open-thandor: WIDE_PATH_MAX_CODE_UNITS units like the split's input (the original had 0xF8, too small for
     a path without a backslash) */
  uint16_t leafName [WIDE_PATH_MAX_CODE_UNITS];

  Package_SetLastErrorPath(path);
  if (!Win32Path_ToNarrow(g_Win32PathScratch[0],path)) {
    return FATAL_ERROR_FILE_WRITE_FAILED;
  }
  if (CreateDirectoryA((LPCSTR)g_Win32PathScratch[0],nullptr) != 0) {
    return 0;
  }
  if ((flags & FILESYSTEM_CREATE_DIRECTORY_RECURSIVE) != 0) {
    WidePath_SplitParentAndLeaf(leafName,parentPath,path);
    if (Win32File_CreateDirectoryRecursive(flags,parentPath) == 0) {
      Win32Path_ToNarrow(g_Win32PathScratch[0],path); /* fitted above */
      if (CreateDirectoryA((LPCSTR)g_Win32PathScratch[0],nullptr) != 0) {
        return 0; /* created after its parents */
      }
    }
  }
  Package_SetLastErrorPath(path);
  return FATAL_ERROR_FILE_WRITE_FAILED;
}


/* Removes an (empty) directory. Returns 0, or FATAL_ERROR_REMOVE_DIRECTORY_FAILED when RemoveDirectoryA
   fails. Unlike the other path operations it does not record the path in g_PackageLastErrorPath.
*/
uint32_t Win32File_RemoveDirectory(uint16_t *path)

{
  if (Win32Path_ToNarrow(g_Win32PathScratch[0],path) && RemoveDirectoryA((LPCSTR)g_Win32PathScratch[0]) != 0) {
    return 0;
  }
  return FATAL_ERROR_REMOVE_DIRECTORY_FAILED;
}


/* Returns the free and total bytes of a drive, both 0 when the query fails. The products are 32-bit, so
   drives above 4 GiB wrap.
*/
Win32DriveCapacity Win32Drive_GetFreeAndTotalBytes(DosDriveLetterCode32 driveLetter)

{
  BOOL gotDiskSpace;
  int freeBytes;
  int totalBytes;
  Win32DriveCapacity capacity;

  g_Win32DriveRootPathScratchA[0] = (char)driveLetter; /* "X:\" root path scratch */
  /* the four results land in the find-data scratch (original layout): sectors per cluster in
     ftLastWriteTime.dwHighDateTime, bytes per sector in nFileSizeHigh, free clusters in nFileSizeLow,
     total clusters in dwReserved0 */
  gotDiskSpace =
       GetDiskFreeSpaceA(g_Win32DriveRootPathScratchA,(LPDWORD)&g_Win32FindDataScratch.ftLastWriteTime.dwHighDateTime
                         ,(LPDWORD)&g_Win32FindDataScratch.nFileSizeHigh,
                         (LPDWORD)&g_Win32FindDataScratch.nFileSizeLow,
                         (LPDWORD)&g_Win32FindDataScratch.dwReserved0);
  totalBytes = 0;
  freeBytes = 0;
  if (gotDiskSpace != 0) {
    freeBytes = g_Win32FindDataScratch.nFileSizeLow *
                g_Win32FindDataScratch.nFileSizeHigh * g_Win32FindDataScratch.ftLastWriteTime.dwHighDateTime;
    totalBytes = g_Win32FindDataScratch.dwReserved0 *
                 g_Win32FindDataScratch.nFileSizeHigh * g_Win32FindDataScratch.ftLastWriteTime.dwHighDateTime;
  }
  capacity.freeBytes = (uint32_t)freeBytes;
  capacity.totalBytes = (uint32_t)totalBytes;
  return capacity;
}

/* Lists the existing drives: writes one letter 'A'..'Z' per set bit of GetLogicalDrives to lettersOut
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


/* Checks that an ANSI path is made of DOS 8.3 names (letters, digits and characters below ','), with an
   optional "X:" drive and leading '\'. FILESYSTEM_DOS83_ALLOW_WILDCARDS permits '*' and '?';
   FILESYSTEM_DOS83_COMPONENT_ONLY checks one name only, and FILESYSTEM_DOS83_ALLOW_PATH_CONTINUATION lets
   that name end at a '\'. Returns false when valid, true when rejected.
*/
Bool8 Win32Path_ValidateDos83(FileSystemDos83ValidationFlags flags,uint8_t *pathAnsi)

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
static Bool8 Win32FileSystem_FoundEntryMatchesMode(FileSystemEnumerationMode mode)

{
  if (mode == FILESYSTEM_ENUMERATE_FILES) {
    return (g_Win32FindDataScratch.dwFileAttributes &
            (FILE_ATTRIBUTE_DIRECTORY | FILESYSTEM_ATTRIBUTE_VOLUME_LABEL)) == 0;
  }
  if (mode != FILESYSTEM_ENUMERATE_DIRECTORIES) {
    return false;
  }
  if ((g_Win32FindDataScratch.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    return false;
  }
  return (g_Win32FindDataScratch.cFileName[0] != '.') ||
         ((g_Win32FindDataScratch.cFileName[1] != '\0') &&
          ((g_Win32FindDataScratch.cFileName[1] != '.') || (g_Win32FindDataScratch.cFileName[2] != '\0')));
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

/* Fills outputRecords with 0x200-byte UTF-16 name records: the files (FILESYSTEM_ENUMERATE_FILES) or
   subdirectories (FILESYSTEM_ENUMERATE_DIRECTORIES) matching the wildcard path, sorted by a bubble sort,
   or the single volume label of a drive (FILESYSTEM_ENUMERATE_VOLUME_LABEL, pathOrVolumeText is then the
   ANSI "X:\"). Entries that no longer fit are skipped. Returns the number of records written (an
   unreadable path or drive yields 0 records); the record stride is always
   FILESYSTEM_ENUMERATION_RECORD_BYTES. (The original returned the stride as well as the count and
   never reported failure.)
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
          (g_Win32DriveRootPathScratchA,(LPSTR)g_Win32PathScratch[0],128,nullptr,nullptr,nullptr,nullptr,0) == 0) {
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
  if (!Win32Path_ToNarrow(g_Win32PathScratch[0],(uint16_t *)pathOrVolumeText)) {
    return 0;
  }
  findHandle = FindFirstFileA((LPCSTR)g_Win32PathScratch[0],&g_Win32FindDataScratch);
  if (findHandle == INVALID_HANDLE_VALUE) {
    return 0;
  }
  recordCount = 0;
  destination = (uint16_t *)outputRecords;
  do {
    if (Win32FileSystem_FoundEntryMatchesMode(mode) &&
        (FILESYSTEM_ENUMERATION_RECORD_BYTES - 1 < outputCapacityBytes)) {
      Text_CopyNarrowToUtf16(FILESYSTEM_ENUMERATION_RECORD_BYTES,destination,
                             (uint8_t *)g_Win32FindDataScratch.cFileName);
      destination = destination + FILESYSTEM_ENUMERATION_RECORD_BYTES / 2;
      recordCount++;
      outputCapacityBytes = outputCapacityBytes - FILESYSTEM_ENUMERATION_RECORD_BYTES;
    }
  } while (FindNextFileA(findHandle,&g_Win32FindDataScratch) != 0);
  FindClose(findHandle);
  /* bubble sort; each swap goes through the path scratch block (original quirk: the original's 0x80-dword copy
     starts at the first 0x100-byte path buffer and runs on through the second up to its end; here the buffers
     are THANDOR_PATH_CAPACITY bytes and the FILESYSTEM_ENUMERATION_RECORD_BYTES record stays in the first) */
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


/* Reads exactly byteCount bytes from a file. Returns 0 on success, FATAL_ERROR_FILE_READ_FAILED when
   fewer bytes arrive (end of file or read error). (The original returned the byte count on success.)
*/
uint32_t Win32File_ReadExact(FileIoByteCount byteCount,void *destination,void *handle)

{
  g_Win32FileBytesTransferred = 0;
  ReadFile(handle,destination,byteCount,&g_Win32FileBytesTransferred,nullptr);
  if (g_Win32FileBytesTransferred == byteCount) {
    return 0;
  }
  return FATAL_ERROR_FILE_READ_FAILED;
}


/* Stores the size of a file (low 32 bits) in *outSize and returns true; returns false with *outSize 0
   when GetFileSize fails (some callers pass that 0 on as their error code).
*/
Bool8 Win32File_GetSize(void *handle,uint32_t *outSize)

{
  DWORD fileSize;

  fileSize = GetFileSize(handle,nullptr);
  if (fileSize != INVALID_FILE_SIZE) {
    *outSize = fileSize;
    return true;
  }
  *outSize = 0;
  return false;
}


/* Stores the current directory as UTF-16 into destination (0x200 bytes) and returns true. When
   GetCurrentDirectoryA fails, destination becomes an empty string and the result is false. (The
   original also passed on the copy's byte count, or FATAL_ERROR_GENERAL_FAILURE for a cut-off path, as
   the success value; no caller read it.)
*/
Bool8 Win32File_GetCurrentDirectory(uint16_t *destination)

{
  DWORD narrowPathLength;

  /* open-thandor: a directory that does not fit counts as a failure (GetCurrentDirectoryA then returns the
     size it needs and leaves the buffer as it was; the original copied that stale content) */
  narrowPathLength = GetCurrentDirectoryA(WIDE_PATH_MAX_CODE_UNITS - 1,(LPSTR)g_Win32PathScratch[0]);
  if (narrowPathLength != 0 && narrowPathLength < WIDE_PATH_MAX_CODE_UNITS - 1) {
    Text_CopyNarrowToUtf16(WIDE_PATH_MAX_CODE_UNITS * sizeof(uint16_t),destination,g_Win32PathScratch[0]);
    return true;
  }
  destination[0] = 0;
  destination[1] = 0;
  return false;
}


/* Changes the current directory. Returns 0, or FATAL_ERROR_SET_DIRECTORY_FAILED when SetCurrentDirectoryA
   fails.
*/
uint32_t Win32File_SetCurrentDirectory(uint16_t *path)

{
  Package_SetLastErrorPath(path);
  if (Win32Path_ToNarrow(g_Win32PathScratch[0],path) && SetCurrentDirectoryA((LPCSTR)g_Win32PathScratch[0]) != 0) {
    return 0;
  }
  return FATAL_ERROR_SET_DIRECTORY_FAILED;
}


/* Classifies a drive for the engine: ENGINE_DRIVE_REMOVABLE, ENGINE_DRIVE_REMOTE, ENGINE_DRIVE_CDROM, or
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


/* Opens a file (path recorded in g_PackageLastErrorPath for error messages). The FileSystemOpenFlags
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
  if (!Win32Path_ToNarrow(g_Win32PathScratch[0],path)) {
    return FATAL_ERROR_FILE_ACCESS_FAILED;
  }
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
  fileHandle = CreateFileA((LPCSTR)g_Win32PathScratch[0],desiredAccess,shareMode,nullptr,creationDisposition,
                           FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_NORMAL,nullptr);
  if (fileHandle != INVALID_HANDLE_VALUE) {
    *outHandle = fileHandle;
    return 0;
  }
  return FATAL_ERROR_FILE_ACCESS_FAILED;
}


/* Closes a file handle.
*/
void Win32File_Close(void *handle)

{
  CloseHandle(handle);
  return;
}

