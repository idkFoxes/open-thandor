/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/filesystem/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_FILESYSTEM_TYPES_H
#define THANDOR_PLATFORM_FILESYSTEM_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/platform/system/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA, *P_WIN32_FIND_DATAA;
typedef struct _FILETIME _FILETIME, *P_FILETIME;

struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

typedef struct _FILETIME FILETIME;

struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[260];
    CHAR cAlternateFileName[14];
};

enum {
    FILESYSTEM_OPEN_CREATE_OR_TRUNCATE=1,
    FILESYSTEM_OPEN_EXCLUSIVE_SHARE=2,
    FILESYSTEM_OPEN_EXISTING_OR_CREATE=4,
    FILESYSTEM_OPEN_WRITE_ACCESS=8
};
using FileSystemOpenFlags = int;

enum {
    FILESYSTEM_CREATE_DIRECTORY_RECURSIVE=1
};
using FileSystemCreateDirectoryFlags = int;

using FileSystemOutputCapacityBytes = uint32_t;

using FileSystemFilePosition = uint32_t;

using DosDriveLetterCode32 = uint32_t;

typedef struct Win32DriveCapacity {
    uint32_t freeBytes; /* free clusters * bytes per sector * sectors per cluster, 32-bit product */
    uint32_t totalBytes; /* total clusters * bytes per sector * sectors per cluster, 32-bit product */
} Win32DriveCapacity;

using FileIoByteCount = uint32_t;
using FileSystemCloseProc = void (void * handle);
using FileSystemCopyProc = uint32_t (uint16_t * destinationPath, uint16_t * sourcePath);
using FileSystemCreateDirectoryRecursiveProc = uint32_t (FileSystemCreateDirectoryFlags flags, uint16_t * path);
using FileSystemDeleteProc = uint32_t (uint32_t unusedFlags, uint16_t * path);
using FileSystemEnumerateDirectoryOrVolumeEntriesProc = uint32_t (FileSystemEnumerationMode mode, uint32_t reserved, FileSystemOutputCapacityBytes outputCapacityBytes, uint8_t * outputRecords, uint8_t * pathOrVolumeText);
using FileSystemGetCurrentDirectoryProc = Bool8 (uint16_t * destination);
using FileSystemGetFreeAndTotalBytesRegsProc = Win32DriveCapacity (DosDriveLetterCode32 driveLetter);
using FileSystemGetLastWriteDosDateProc = uint32_t (uint16_t * path, uint32_t * outDosDateTime);
using FileSystemGetLastWriteTimeHighProc = uint32_t (uint16_t * path, uint32_t * outLastWriteTimeHigh);
using FileSystemGetPositionProc = Bool8 (void * handle, uint32_t * outPosition);
using FileSystemGetSizeProc = Bool8 (void * handle, uint32_t * outSize);
using FileSystemGetVolumeSerialNumberProc = uint32_t (uint8_t * outputLabel, char * path);
using FileSystemMoveProc = uint32_t (uint16_t * destinationPath, uint16_t * sourcePath);
using FileSystemOpenProc = uint32_t (FileSystemOpenFlags openFlags, uint16_t * path, void * * outHandle);
using FileSystemReadExactProc = uint32_t (FileIoByteCount byteCount, void * destination, void * handle);
using FileSystemRemoveDirectoryProc = uint32_t (uint16_t * path);
using FileSystemSeekProc = uint32_t (FileSystemSeekOrigin moveMethod, FileSystemFilePosition distance, void * handle);
using FileSystemSetCurrentDirectoryProc = uint32_t (uint16_t * path);
using FileSystemWriteExactOrFlushProc = uint32_t (FileIoByteCount byteCount, void * source, void * handle);

#endif /* THANDOR_PLATFORM_FILESYSTEM_TYPES_H */
