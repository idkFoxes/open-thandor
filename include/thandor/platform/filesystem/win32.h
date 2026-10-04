/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/filesystem/win32.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_FILESYSTEM_WIN32_H
#define THANDOR_PLATFORM_FILESYSTEM_WIN32_H

#include <thandor/core/types.h>
#include <thandor/platform/filesystem/types.h>
#include <thandor/core/contracts.h>
#include <thandor/core/text/path.h>

/* Submodule: platform/filesystem/win32. */

/* Size of one UTF-16 name record written by Win32FileSystem_EnumerateDirectoryOrVolumeEntries (0x100 code
   units, NUL-padded); returned to the caller as the record stride */
#define FILESYSTEM_ENUMERATION_RECORD_BYTES 0x200
/* The DOS volume-label file attribute (_A_VOLID); file enumeration skips such entries like directories */
#define FILESYSTEM_ATTRIBUTE_VOLUME_LABEL 0x08
/* Functions are grouped by semantic ownership. */

uintptr_t __cdecl FileSystem_Init(void);

/* 0 with the packed DOS date/time, or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_GetLastWriteDosDate(uint16_t *path,uint32_t *outDosDateTime);

/* 0 with the last-write FILETIME's high dword, or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_GetLastWriteTimeHigh(uint16_t *path,uint32_t *outLastWriteTimeHigh);

uint32_t Win32Drive_GetVolumeSerialNumber(uint8_t *outputLabel,char *path);

void __cdecl Win32FileSystem_RestoreInitialDirectory(void);

uint32_t FileSystem_WriteBufferToPath(FileIoByteCount byteCount,void *source,uint16_t *path);

/* 0, FATAL_ERROR_FILE_WRITE_FAILED or FATAL_ERROR_FILE_WRITE_INCOMPLETE */
uint32_t Win32File_WriteExactOrFlush(FileIoByteCount byteCount,void *source,void *handle);

/* false (*outPosition 0) when the position cannot be read */
Bool8 Win32File_GetPosition(void *handle,uint32_t *outPosition);

/* 0 or FATAL_ERROR_FILE_SEEK_FAILED */
uint32_t Win32File_Seek(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle);

/* 0 or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Delete(uint32_t unusedFlags,uint16_t *path);

/* 0 or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Move(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0 or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Copy(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0 or FATAL_ERROR_FILE_WRITE_FAILED */
uint32_t Win32File_CreateDirectoryRecursive(FileSystemCreateDirectoryFlags flags,uint16_t *path);

/* 0 or FATAL_ERROR_REMOVE_DIRECTORY_FAILED */
uint32_t Win32File_RemoveDirectory(uint16_t *path);

Win32DriveCapacity Win32Drive_GetFreeAndTotalBytes(DosDriveLetterCode32 driveLetter);

/* the number of FILESYSTEM_ENUMERATION_RECORD_BYTES records written */
uint32_t Win32FileSystem_EnumerateDirectoryOrVolumeEntries
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText);

/* 0 or FATAL_ERROR_FILE_READ_FAILED */
uint32_t Win32File_ReadExact(FileIoByteCount byteCount,void *destination,void *handle);

/* false (*outSize 0) when the size cannot be read */
Bool8 Win32File_GetSize(void *handle,uint32_t *outSize);

/* false (destination emptied) when the directory cannot be read */
Bool8 Win32File_GetCurrentDirectory(uint16_t *destination);

/* 0 or FATAL_ERROR_SET_DIRECTORY_FAILED */
uint32_t Win32File_SetCurrentDirectory(uint16_t *path);

/* 0 (handle in *outHandle) or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Open(FileSystemOpenFlags openFlags,uint16_t *path,void **outHandle);

void Win32File_Close(void *handle);


Bool8 FileSystem_LoadWholeFileNearExecutable(uint16_t *pathUtf16,void **outBuffer,uint32_t *outByteCount,
          uint32_t *outError);

Bool8 FileSystem_LoadWholeFile(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError);

Bool8 FileSystem_LoadWholeFileAlternatePath(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError);

/* true with the string table (NULL when empty) and its entry count */
Bool8 FileSystem_BuildEnumerationStringTable
          (FileSystemEnumerationMode enumerationMode,uint32_t reserved,uint8_t *pathOrVolumeText,
          uint16_t ***outTable,uint32_t *outEntryCount);

extern FileSystemEnumerateDirectoryOrVolumeEntriesProc *g_FileSystemEnumerateDirectoryOrVolumeEntries;

extern FileSystemDeleteProc *g_FileSystemDelete;
extern FileSystemCreateDirectoryRecursiveProc *g_FileSystemCreateDirectoryRecursive;
extern uint16_t g_DefaultComputerLabelUtf16[32];

extern FileSystemGetPositionProc *g_FileSystemGetPosition;

extern uint16_t g_FileSystemCombinedPathScratchUtf16[THANDOR_PATH_CAPACITY];
extern FileSystemOpenProc *g_FileSystemOpen;
extern FileSystemCloseProc *g_FileSystemClose;
extern FileSystemReadExactProc *g_FileSystemReadExact;
extern FileSystemWriteExactOrFlushProc *g_FileSystemWriteExactOrFlush;
extern FileSystemGetSizeProc *g_FileSystemGetSize;
extern FileSystemSeekProc *g_FileSystemSeek;

extern uint16_t g_ExecutableDirectoryUtf16[256];

#endif /* THANDOR_PLATFORM_FILESYSTEM_WIN32_H */
