/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/filesystem/win32.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_FILESYSTEM_WIN32_H
#define THANDOR_PLATFORM_FILESYSTEM_WIN32_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/filesystem/win32. */

/* Size of one UTF-16 name record written by Win32FileSystem_EnumerateDirectoryOrVolumeEntries (0x100 code
   units, NUL-padded); returned to the caller as the record stride */
#define FILESYSTEM_ENUMERATION_RECORD_BYTES 0x200
/* The DOS volume-label file attribute (_A_VOLID); file enumeration skips such entries like directories */
#define FILESYSTEM_ATTRIBUTE_VOLUME_LABEL 0x08
/* Win32Path_ValidateDos83: characters of a DOS 8.3 base name and extension */
#define DOS83_BASE_NAME_MAX_CHARS 8
#define DOS83_EXTENSION_MAX_CHARS 3
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00575CB0 */
uint32_t __cdecl FileSystem_Init(void);

/* 0x005762F0: 0 with the packed DOS date/time, or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_GetLastWriteDosDate(uint16_t *path,uint32_t *outDosDateTime);

/* 0x00576360: 0 with the last-write FILETIME's high dword, or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_GetLastWriteTimeHigh(uint16_t *path,uint32_t *outLastWriteTimeHigh);

/* 0x005763C0 */
uint32_t Win32Drive_GetVolumeSerialNumber(uint8_t *outputLabel,char *path);

/* 0x00575F40 */
void __cdecl Win32FileSystem_RestoreInitialDirectory(void);

/* 0x005766F0 */
bool Win32Drive_CheckMediaReady(DosDriveLetterCode32 driveLetter);

/* 0x0040F1F0 */
uint32_t FileSystem_WriteBufferToPath(FileIoByteCount byteCount,void *source,uint16_t *path);

/* 0x00576070: 0, FATAL_ERROR_FILE_WRITE_FAILED or FATAL_ERROR_FILE_WRITE_INCOMPLETE */
uint32_t Win32File_WriteExactOrFlush(FileIoByteCount byteCount,void *source,void *handle);

/* 0x00576140: false (*outPosition 0) when the position cannot be read */
bool Win32File_GetPosition(void *handle,uint32_t *outPosition);

/* 0x00576180: 0 or FATAL_ERROR_FILE_SEEK_FAILED */
uint32_t Win32File_Seek(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle);

/* 0x005761C0: 0 or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Delete(uint32_t unusedFlags,uint16_t *path);

/* 0x00576210: 0 or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Move(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0x00576280: 0 or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Copy(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0x005764E0: 0 or FATAL_ERROR_FILE_WRITE_FAILED */
uint32_t Win32File_CreateDirectoryRecursive(FileSystemCreateDirectoryFlags flags,uint16_t *path);

/* 0x005765A0: 0 or FATAL_ERROR_REMOVE_DIRECTORY_FAILED */
uint32_t Win32File_RemoveDirectory(uint16_t *path);

/* 0x005765F0 */
Win32DriveCapacity Win32Drive_GetFreeAndTotalBytes(DosDriveLetterCode32 driveLetter);

/* 0x005766B0 */
uint32_t Win32Drive_EnumerateLetters(uint8_t *lettersOut);

/* 0x00576790 */
bool Win32Path_ValidateDos83(FileSystemDos83ValidationFlags flags,uint8_t *pathAnsi);

/* 0x00576910: the number of FILESYSTEM_ENUMERATION_RECORD_BYTES records written */
uint32_t Win32FileSystem_EnumerateDirectoryOrVolumeEntries
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText);

/* 0x00576020: 0 or FATAL_ERROR_FILE_READ_FAILED */
uint32_t Win32File_ReadExact(FileIoByteCount byteCount,void *destination,void *handle);

/* 0x00576100: false (*outSize 0) when the size cannot be read */
bool Win32File_GetSize(void *handle,uint32_t *outSize);

/* 0x00576430: false (destination emptied) when the directory cannot be read */
bool Win32File_GetCurrentDirectory(uint16_t *destination);

/* 0x00576490: 0 or FATAL_ERROR_SET_DIRECTORY_FAILED */
uint32_t Win32File_SetCurrentDirectory(uint16_t *path);

/* 0x00576650 */
EngineDriveTypeCode Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter);

/* 0x00575F60: 0 (handle in *outHandle) or FATAL_ERROR_FILE_ACCESS_FAILED */
uint32_t Win32File_Open(FileSystemOpenFlags openFlags,uint16_t *path,void **outHandle);

/* 0x00576000 */
void Win32File_Close(void *handle);


/* 0x0040EF50 */
bool FileSystem_LoadWholeFile(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError);

/* 0x0040F120 */
bool FileSystem_LoadWholeFileAlternatePath(uint16_t *pathUtf16,void **outBuffer,uint32_t *outError);

/* 0x0040F430: true with the string table (NULL when empty) and its entry count */
bool FileSystem_BuildEnumerationStringTable
          (FileSystemEnumerationMode enumerationMode,uint32_t reserved,uint8_t *pathOrVolumeText,
          uint16_t ***outTable,uint32_t *outEntryCount);

extern FileSystemEnumerateDirectoryOrVolumeEntriesProc *g_FileSystemEnumerateDirectoryOrVolumeEntries; /* 0040B214 g_FileSystemEnumerateDirectoryOrVolumeEntries */
extern FileSystemValidateDos83Proc *g_FileSystemValidateDos83Path; /* 0040B218 g_FileSystemValidateDos83Path */

#endif /* THANDOR_PLATFORM_FILESYSTEM_WIN32_H */
