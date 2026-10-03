/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/filesystem/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/platform/filesystem/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* uint8_t[256] byte map applied to every non-separator character of the file-system config text in FileSystem_Init: identity except a-z -> A-Z and the CP437 lowercase accented letters -> their uppercase forms (case folding). */
__declspec(align(16)) uint8_t g_FileSystemConfigCharacterNormalizationMap[256] = {
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

__declspec(align(16)) WidePathBuffer256 g_InitialWorkingDirectory = {0};

__declspec(align(4)) FileSystemDeleteProc *g_FileSystemDelete = 0;

__declspec(align(16)) FileSystemGetCurrentDirectoryProc *g_FileSystemGetCurrentDirectory = 0;

__declspec(align(4)) FileSystemSetCurrentDirectoryProc *g_FileSystemSetCurrentDirectory = 0;

__declspec(align(8)) FileSystemCreateDirectoryRecursiveProc *g_FileSystemCreateDirectoryRecursive = 0;

__declspec(align(4)) FileSystemRemoveDirectoryProc *g_FileSystemRemoveDirectory = 0;

__declspec(align(16)) FileSystemEnumerateDriveLettersProc *g_FileSystemEnumerateDriveLetters = 0;

__declspec(align(4)) FileSystemGetDriveTypeCodeProc *g_FileSystemGetDriveTypeCode = 0;

__declspec(align(8)) FileSystemDriveReadyProc *g_FileSystemCheckDriveMediaReady = 0;

__declspec(align(4)) FileSystemGetFreeAndTotalBytesRegsProc *g_FileSystemGetFreeAndTotalBytes = 0;

__declspec(align(16)) FileSystemGetLastWriteDosDateProc *g_FileSystemGetLastWriteDosDate = 0;

__declspec(align(4)) FileSystemGetLastWriteTimeHighProc *g_FileSystemGetLastWriteTimeHigh = 0;

__declspec(align(8)) FileSystemGetVolumeSerialNumberProc *g_FileSystemGetVolumeSerialNumber = 0;

__declspec(align(4)) FileSystemMoveProc *g_FileSystemMove = 0;

__declspec(align(16)) FileSystemCopyProc *g_FileSystemCopy = 0;

__declspec(align(16)) uint32_t g_EnginePackageLowPriorityMountHandle = 0;

__declspec(align(4)) uint16_t u_THANDOR_cfg_0040e23d[12] = L"THANDOR.cfg";

__declspec(align(4)) uint16_t u_engine_pck_0040e255[11] = L"engine.pck";

__declspec(align(16)) uint16_t g_DefaultComputerLabelUtf16[32] = L"Computer";

__declspec(align(16)) uint32_t g_Win32FileBytesTransferred = 0;

/* WIN32_FIND_DATAA of the directory enumeration, reused as file-time,
   DOS-date and disk-space scratch (see image_data.h) */
__declspec(align(4)) _WIN32_FIND_DATAA g_Win32FindDataScratch = {0};

__declspec(align(4)) pointer g_FileSystemInitComputerNameCapacityOrConfigCursor = 0;

__declspec(align(8)) uint32_t g_FileSystemConfigRemainingBytes = 0;

/* two 0x100-byte narrow path buffers ([1] is the second path of move/copy); the directory sort swaps 0x200-byte records through the whole block */
__declspec(align(4)) uint8_t g_Win32PathScratch[2][256] = {0};

/* char[4]: "x:\" root path, drive letter patched at [0] before GetDiskFreeSpaceA/GetVolumeInformationA/GetDriveTypeA; platform/filesystem/win32.c */
__declspec(align(4)) char g_Win32DriveRootPathScratchA[4] = "x:\\";
