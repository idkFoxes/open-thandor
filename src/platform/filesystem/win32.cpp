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

static uint16_t g_ThandorCfgPathUtf16[12] = {'T', 'H', 'A', 'N', 'D', 'O', 'R', '.', 'c', 'f', 'g', 0}; /* L"THANDOR.cfg" */

static uint16_t g_EnginePckPathUtf16[11] = {'e', 'n', 'g', 'i', 'n', 'e', '.', 'p', 'c', 'k', 0}; /* L"engine.pck" */

static uint32_t g_Win32FileBytesTransferred = 0;

/* WIN32_FIND_DATAA of the directory enumeration */
static _WIN32_FIND_DATAA g_Win32FindDataScratch = {0};

/* two narrow path buffers ([1] held the second path of the original's move/copy, which had no caller); the
   directory sort swaps 0x200-byte records through the start of the block. open-thandor: THANDOR_PATH_CAPACITY
   bytes each instead of the original's 0x100, and a path that does not fit fails the operation
   (Win32Path_ToNarrow) instead of going to Windows cut off: with a long game directory the cut-off path named
   a different file or directory. */
static uint8_t g_Win32PathScratch[2][THANDOR_PATH_CAPACITY] = {0};

/* char[4]: "x:\" root path, drive letter patched at [0] before GetVolumeInformationA */
static char g_Win32DriveRootPathScratchA[4] = "x:\\";

FileSystemDeleteProc *g_FileSystemDelete = nullptr;

FileSystemCreateDirectoryRecursiveProc *g_FileSystemCreateDirectoryRecursive = nullptr;

uint16_t g_DefaultComputerLabelUtf16[32] = {'C', 'o', 'm', 'p', 'u', 't', 'e', 'r', 0}; /* L"Computer" */

FileSystemEnumerateDirectoryOrVolumeEntriesProc *g_FileSystemEnumerateDirectoryOrVolumeEntries = nullptr;

/* open-thandor: converts a UTF-16 path into one of the g_Win32PathScratch buffers for the ANSI file APIs.
   False when it does not fit; the original cut the path off at 0xFF bytes and used it anyway. */
static Bool8 Win32Path_ToNarrow(uint8_t *destination,uint16_t *path)

{
  return RichTextCommandStream_CopyToNarrow(sizeof g_Win32PathScratch[0],destination,path);
}

/* Starts the file layer: records the executable directory, installs the Win32 implementations of the
   g_FileSystem* function table, replaces the default L"Computer" label with the machine name, allocates the
   8 MiB package scratch buffer, loads THANDOR.cfg (current directory first, then the executable
   directory) and normalizes it in place, remembers the working directory and mounts engine.pck.
   The original never reports failure to its caller; the return value is the result of the engine.pck mount.
*/
uintptr_t FileSystem_Init()

{
  uint8_t configByte;
  uint8_t *configCursor;
  BOOL gotComputerName;
  void *configFile;
  int clearCount;
  ArenaPayloadByteCount configBytesLeft;
  uint16_t *labelCursor;
  uint32_t openError;
  DWORD computerNameCapacity;
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
  g_FileSystemCreateDirectoryRecursive = Win32File_CreateDirectoryRecursive;
  g_FileSystemEnumerateDirectoryOrVolumeEntries =
       Win32FileSystem_EnumerateDirectoryOrVolumeEntries;
  /* the original kept this GetComputerNameA size in/out in the global that later held the THANDOR.cfg cursor */
  computerNameCapacity = sizeof g_Win32PathScratch[0];
  gotComputerName = GetComputerNameA((LPSTR)g_Win32PathScratch[0],&computerNameCapacity);
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
          /* Normalize the text in place: separators (<= ' ') and [comments] become NUL, other
             characters go through the normalization map. The original also stored the text's address and
             size in two globals that nothing read; the block stays allocated (arena layout). */
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
  /* the original also kept the handle of a successful mount in a global that nothing read */
  Package_MountLowPriority(g_EnginePckPathUtf16,&engineMountResult);
  return engineMountResult;
}


/* Changes back to the working directory FileSystem_Init found at startup, if one was captured.
*/
void Win32FileSystem_RestoreInitialDirectory()

{
  if (g_InitialWorkingDirectory.firstTwoCodeUnits != 0) {
    Win32File_SetCurrentDirectory(g_InitialWorkingDirectory.codeUnits);
  }
  return;
}


/* Reads a whole file into a new arena buffer: the path is tried next to the executable first, then as
   given. Returns true with the buffer in *outBuffer and the file size in *outByteCount (outByteCount may be
   NULL), or false with the open/size/read error in *outError (0 when the size query failed), or
   FATAL_ERROR_OUT_OF_MEMORY with the file size left in g_FatalErrorDetail1Utf16. *outBuffer is only written
   on success, *outError only on failure. Used for loose files by Package_LoadEntry and Resource_Load
   (assets/package); the original's two whole-file loader entry points around it had no caller. */
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

