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

/* 0x005762F0 */
StatusResult Win32File_GetLastWriteDosDate(uint16_t *path);

/* 0x00576360 */
StatusResult Win32File_GetLastWriteTimeHigh(uint16_t *path);

/* 0x005763C0 */
uint32_t Win32Drive_GetVolumeSerialNumber(uint8_t *outputLabel,char *path);

/* 0x00575F40 */
void __cdecl Win32FileSystem_RestoreInitialDirectory(void);

/* 0x005766F0 */
bool Win32Drive_CheckMediaReady(DosDriveLetterCode32 driveLetter);

/* 0x0040F1F0 */
uint32_t FileSystem_WriteBufferToPath(FileIoByteCount byteCount,void *source,uint16_t *path);

/* 0x00576070 */
Win32FileWriteResult Win32File_WriteExactOrFlush(FileIoByteCount byteCount,void *source,void *handle);

/* 0x00576140 */
Win32FileSeekResult Win32File_GetPosition(void *handle);

/* 0x00576180 */
Win32FileSeekResult Win32File_Seek(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle);

/* 0x005761C0 */
StatusResult Win32File_Delete(uint32_t unusedFlags,uint16_t *path);

/* 0x00576210 */
StatusResult Win32File_Move(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0x00576280 */
StatusResult Win32File_Copy(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0x005764E0 */
StatusResult Win32File_CreateDirectoryRecursive(FileSystemCreateDirectoryFlags flags,uint16_t *path);

/* 0x005765A0 */
StatusResult Win32File_RemoveDirectory(uint16_t *path);

/* 0x005765F0 */
Win32DriveCapacityEdxEax8 Win32Drive_GetFreeAndTotalBytesRegs(DosDriveLetterCode32 driveLetter);

/* 0x005766B0 */
DriveLetterEnumeration Win32Drive_EnumerateLetters(uint8_t *lettersOut);

/* 0x00576790 */
bool Win32Path_ValidateDos83(FileSystemDos83ValidationFlags flags,uint8_t *pathAnsi);

/* 0x00576910 */
DirectoryEnumerationResult Win32FileSystem_EnumerateDirectoryOrVolumeEntries
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText);

/* 0x00576020 */
Win32FileReadResult Win32File_ReadExact(FileIoByteCount byteCount,void *destination,void *handle);

/* 0x00576100 */
Win32FileSizeResult Win32File_GetSize(void *handle);

/* 0x00576430 */
StatusResult Win32File_GetCurrentDirectory(uint16_t *destination);

/* 0x00576490 */
StatusResult Win32File_SetCurrentDirectory(uint16_t *path);

/* 0x00576650 */
EngineDriveTypeCode Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter);

/* 0x00575F60 */
Win32FileOpenResult Win32File_Open(FileSystemOpenFlags openFlags,uint16_t *path);

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

#endif /* THANDOR_PLATFORM_FILESYSTEM_WIN32_H */
