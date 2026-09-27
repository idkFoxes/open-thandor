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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00575CB0 */
uint32_t __cdecl FileSystem_Init(void);

/* 0x005762F0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteDosDateCf(uint16_t *path);

/* 0x00576360 */
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteTimeHighCf(uint16_t *path);

/* 0x005763C0 */
uint32_t Win32Drive_GetVolumeSerialNumberCf(uint8_t *outputLabel,char *path);

/* 0x00575F40 */
void __cdecl Win32FileSystem_RestoreInitialDirectory(void);

/* 0x005766F0 */
bool __thandor_cf_preserve_eax_ecx_edx
Win32Drive_CheckMediaReadyCf(DosDriveLetterCode32 driveLetter);

/* 0x0040F1F0 */
StatusResult FileSystem_WriteBufferToPathCf(FileIoByteCount byteCount,void *source,uint16_t *path);

/* 0x00576070 */
Win32FileWriteResult __thandor_eax_cf_preserve_ecx_edx
Win32File_WriteExactOrFlushCf(FileIoByteCount byteCount,void *source,void *handle);

/* 0x00576140 */
uint32_t Win32File_GetPositionCf(void *handle);

/* 0x00576180 */
Win32FileSeekResult __thandor_eax_cf_preserve_ecx_edx
Win32File_SeekCf(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle);

/* 0x005761C0 */
uint32_t Win32File_DeleteCf(uint32_t unusedFlags,uint16_t *path);

/* 0x00576210 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_MoveCf(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0x00576280 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_CopyCf(uint16_t *destinationPath,uint16_t *sourcePath);

/* 0x005764E0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_CreateDirectoryRecursiveCf(FileSystemCreateDirectoryFlags flags,uint16_t *path);

/* 0x005765A0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_RemoveDirectoryCf(uint16_t *path);

/* 0x005765F0 */
Win32DriveCapacityEdxEax8 Win32Drive_GetFreeAndTotalBytesRegs(DosDriveLetterCode32 driveLetter);

/* 0x005766B0 */
DriveLetterEnumerationEaxEcx8 __thandor_eax_ecx_preserve_edx
Win32Drive_EnumerateLetters(uint8_t *lettersOut);

/* 0x00576790 */
bool __thandor_cf_preserve_eax_ecx_edx
Win32Path_ValidateDos83Cf(FileSystemDos83ValidationFlags flags,uint8_t *pathAnsi);

/* 0x00576910 */
DirectoryEnumerationResult __thandor_eax_ecx_cf_preserve_edx
Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf
          (FileSystemEnumerationMode mode,uint32_t reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,uint8_t *outputRecords,
          uint8_t *pathOrVolumeText);

/* 0x00576020 */
Win32FileReadResult __thandor_eax_cf_preserve_ecx_edx
Win32File_ReadExactCf(FileIoByteCount byteCount,void *destination,void *handle);

/* 0x00576100 */
Win32FileSizeResult __thandor_eax_cf_preserve_ecx_edx Win32File_GetSizeCf(void *handle);

/* 0x00576430 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
Win32File_GetCurrentDirectoryCf(uint16_t *destination);

/* 0x00576490 */
StatusResult __thandor_eax_cf_preserve_ecx_edx Win32File_SetCurrentDirectoryCf(uint16_t *path);

/* 0x00576650 */
EngineDriveTypeCode __thandor_eax_preserve_ecx_edx
Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter);

/* 0x00575F60 */
Win32FileOpenResult __thandor_eax_cf_preserve_ecx_edx
Win32File_OpenCf(FileSystemOpenFlags openFlags,uint16_t *path);

/* 0x00576000 */
void __thandor_void_preserve_eax_ecx_edx Win32File_Close(void *handle);


/* 0x0040EF50 */
FileLoadResult __thandor_eax_cf_preserve_ecx_edx FileSystem_LoadWholeFileCf(uint16_t *pathUtf16);

/* 0x0040F120 */
FileLoadResult __thandor_eax_cf_preserve_edx
FileSystem_LoadWholeFileAlternatePathCf(uint16_t *pathUtf16);

/* 0x0040F430 */
EnumerationStringTableResult __thandor_eax_ecx_cf_preserve_edx
FileSystem_BuildEnumerationStringTableCf
          (FileSystemEnumerationMode enumerationMode,uint32_t reserved,uint8_t *pathOrVolumeText);

#endif /* THANDOR_PLATFORM_FILESYSTEM_WIN32_H */
