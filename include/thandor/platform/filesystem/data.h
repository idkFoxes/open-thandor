/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/filesystem/data.h
 */

#ifndef THANDOR_PLATFORM_FILESYSTEM_DATA_H
#define THANDOR_PLATFORM_FILESYSTEM_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint8_t g_FileSystemConfigCharacterNormalizationMap[256]; /* 004027D0 g_FileSystemConfigCharacterNormalizationMap: uint8_t[256] byte map applied to every non-separator character of the file-system config text in FileSystem_Init: identity except a-z -> A-Z and the CP437 lowercase accented letters -> their uppercase forms (case folding). */

extern WidePathBuffer256 g_InitialWorkingDirectory; /* 0040ABC0 g_InitialWorkingDirectory */

extern FileSystemDeleteProc *g_FileSystemDelete; /* 0040B1DC g_FileSystemDelete */

extern FileSystemGetCurrentDirectoryProc *g_FileSystemGetCurrentDirectory; /* 0040B1E0 g_FileSystemGetCurrentDirectory */

extern FileSystemSetCurrentDirectoryProc *g_FileSystemSetCurrentDirectory; /* 0040B1E4 g_FileSystemSetCurrentDirectory */

extern FileSystemCreateDirectoryRecursiveProc *g_FileSystemCreateDirectoryRecursive; /* 0040B1E8 g_FileSystemCreateDirectoryRecursive */

extern FileSystemRemoveDirectoryProc *g_FileSystemRemoveDirectory; /* 0040B1EC g_FileSystemRemoveDirectory */

extern FileSystemEnumerateDriveLettersProc *g_FileSystemEnumerateDriveLetters; /* 0040B1F0 g_FileSystemEnumerateDriveLetters */

extern FileSystemGetDriveTypeCodeProc *g_FileSystemGetDriveTypeCode; /* 0040B1F4 g_FileSystemGetDriveTypeCode */

extern FileSystemDriveReadyProc *g_FileSystemCheckDriveMediaReady; /* 0040B1F8 g_FileSystemCheckDriveMediaReady */

extern FileSystemGetFreeAndTotalBytesRegsProc *g_FileSystemGetFreeAndTotalBytes; /* 0040B1FC g_FileSystemGetFreeAndTotalBytes */

extern FileSystemGetLastWriteDosDateProc *g_FileSystemGetLastWriteDosDate; /* 0040B200 g_FileSystemGetLastWriteDosDate */

extern FileSystemGetLastWriteTimeHighProc *g_FileSystemGetLastWriteTimeHigh; /* 0040B204 g_FileSystemGetLastWriteTimeHigh */

extern FileSystemGetVolumeSerialNumberProc *g_FileSystemGetVolumeSerialNumber; /* 0040B208 g_FileSystemGetVolumeSerialNumber */

extern FileSystemMoveProc *g_FileSystemMove; /* 0040B20C g_FileSystemMove */

extern FileSystemCopyProc *g_FileSystemCopy; /* 0040B210 g_FileSystemCopy */

extern uint32_t g_EnginePackageLowPriorityMountHandle; /* 0040B220 g_EnginePackageLowPriorityMountHandle */

extern uint16_t u_THANDOR_cfg_0040e23d[12]; /* 0040E23D u_THANDOR_cfg_0040e23d */

extern uint16_t u_engine_pck_0040e255[11]; /* 0040E255 u_engine_pck_0040e255 */

extern uint16_t g_DefaultComputerLabelUtf16[32]; /* 0040E270 g_DefaultComputerLabelUtf16 */

extern uint32_t g_Win32FileBytesTransferred; /* 00575950 g_Win32FileBytesTransferred */

extern _WIN32_FIND_DATAA g_Win32FindDataScratch; /* 00575954 g_Win32FindDataScratch: the WIN32_FIND_DATAA of FindFirstFileA/FindNextFileA (cFileName at 00575980), reused as scratch by platform/filesystem/win32.c: GetFileTime FILETIMEs at +0x00/+0x08/+0x10 (4 bytes off the find-data FILETIME fields), the FileTimeToDosDateTime dword at +0x00, the GetDiskFreeSpaceA dwords at +0x18..+0x24 */

extern pointer g_FileSystemInitComputerNameCapacityOrConfigCursor; /* 00575A94 g_FileSystemInitComputerNameCapacityOrConfigCursor */

extern uint32_t g_FileSystemConfigRemainingBytes; /* 00575A98 g_FileSystemConfigRemainingBytes */

extern uint8_t g_Win32PathScratch[2][256]; /* 00575A9C g_Win32PathScratch: two 0x100-byte narrow path buffers ([1] at 00575B9C, second path of move/copy); the directory sort swaps 0x200-byte records through the whole block */

extern char g_Win32DriveRootPathScratchA[4]; /* 00575C9C g_Win32DriveRootPathScratchA: char[4]: "x:\" root path, drive letter patched at [0] before GetDiskFreeSpaceA/GetVolumeInformationA/GetDriveTypeA; platform/filesystem/win32.c */

#endif
