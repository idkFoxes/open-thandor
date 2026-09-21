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
dword __cdecl FileSystem_Init(void);

/* 0x005762F0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteDosDateCf(word *path);

/* 0x00576360 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetLastWriteTimeHighCf(word *path);

/* 0x005763C0 */
dword Win32Drive_GetVolumeSerialNumberCf(byte *outputLabel,char *path);

/* 0x00575F40 */
void __cdecl Win32FileSystem_RestoreInitialDirectory(void);

/* 0x005766F0 */
bool __thandor_cf_preserve_eax_ecx_edx
Win32Drive_CheckMediaReadyCf(DosDriveLetterCode32 driveLetter);

/* 0x0040F1F0 */
StatusValueEaxCf5 FileSystem_WriteBufferToPathCf(FileIoByteCount byteCount,void *source,word *path);

/* 0x00576070 */
Win32FileWriteEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_WriteExactOrFlushCf(FileIoByteCount byteCount,void *source,void *handle);

/* 0x00576140 */
dword Win32File_GetPositionCf(void *handle);

/* 0x00576180 */
Win32FileSeekEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_SeekCf(FileSystemSeekOrigin moveMethod,FileSystemFilePosition distance,void *handle);

/* 0x005761C0 */
dword Win32File_DeleteCf(dword unusedFlags,word *path);

/* 0x00576210 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_MoveCf(word *destinationPath,word *sourcePath);

/* 0x00576280 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_CopyCf(word *destinationPath,word *sourcePath);

/* 0x005764E0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_CreateDirectoryRecursiveCf(FileSystemCreateDirectoryFlags flags,word *path);

/* 0x005765A0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_RemoveDirectoryCf(word *path);

/* 0x005765F0 */
Win32DriveCapacityEdxEax8 Win32Drive_GetFreeAndTotalBytesRegs(DosDriveLetterCode32 driveLetter);

/* 0x005766B0 */
DriveLetterEnumerationEaxEcx8 __thandor_eax_ecx_preserve_edx
Win32Drive_EnumerateLetters(byte *lettersOut);

/* 0x00576790 */
bool __thandor_cf_preserve_eax_ecx_edx
Win32Path_ValidateDos83Cf(FileSystemDos83ValidationFlags flags,byte *pathAnsi);

/* 0x00576910 */
FileSystemEnumerationEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf
          (FileSystemEnumerationMode mode,dword reserved,
          FileSystemOutputCapacityBytes outputCapacityBytes,byte *outputRecords,
          byte *pathOrVolumeText);

/* 0x00576020 */
Win32FileReadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_ReadExactCf(FileIoByteCount byteCount,void *destination,void *handle);

/* 0x00576100 */
Win32FileSizeEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_GetSizeCf(void *handle);

/* 0x00576430 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_GetCurrentDirectoryCf(word *destination);

/* 0x00576490 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx Win32File_SetCurrentDirectoryCf(word *path);

/* 0x00576650 */
EngineDriveTypeCode __thandor_eax_preserve_ecx_edx
Win32Drive_GetEngineTypeCode(DosDriveLetterCode32 driveLetter);

/* 0x00575F60 */
Win32FileOpenEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Win32File_OpenCf(FileSystemOpenFlags openFlags,word *path);

/* 0x00576000 */
void __thandor_void_preserve_eax_ecx_edx Win32File_Close(void *handle);


/* 0x0040EF50 */
FileBufferEaxCf5 __thandor_eax_cf_preserve_ecx_edx FileSystem_LoadWholeFileCf(word *pathUtf16);

/* 0x0040F120 */
FileBufferEaxCf5 __thandor_eax_cf_preserve_edx
FileSystem_LoadWholeFileAlternatePathCf(word *pathUtf16);

/* 0x0040F430 */
FileSystemStringTableEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
FileSystem_BuildEnumerationStringTableCf
          (FileSystemEnumerationMode enumerationMode,dword reserved,byte *pathOrVolumeText);

#endif /* THANDOR_PLATFORM_FILESYSTEM_WIN32_H */
