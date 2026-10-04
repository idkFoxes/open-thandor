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

/* Types (split from generated/types.h by tools/dev/split_types.py). */

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
typedef int FileSystemOpenFlags;

enum {
    FILESYSTEM_CREATE_DIRECTORY_RECURSIVE=1
};
typedef int FileSystemCreateDirectoryFlags;

typedef uint32_t FileSystemOutputCapacityBytes;

typedef uint32_t FileSystemFilePosition;

typedef uint32_t DosDriveLetterCode32;

typedef struct Win32DriveCapacity {
    uint32_t freeBytes; /* free clusters * bytes per sector * sectors per cluster, 32-bit product */
    uint32_t totalBytes; /* total clusters * bytes per sector * sectors per cluster, 32-bit product */
} Win32DriveCapacity;

typedef uint32_t FileIoByteCount;
typedef void FileSystemCloseProc(void * handle);
typedef uint32_t FileSystemCopyProc(uint16_t * destinationPath, uint16_t * sourcePath);
typedef uint32_t FileSystemCreateDirectoryRecursiveProc(FileSystemCreateDirectoryFlags flags, uint16_t * path);
typedef uint32_t FileSystemDeleteProc(uint32_t unusedFlags, uint16_t * path);
typedef uint32_t FileSystemEnumerateDirectoryOrVolumeEntriesProc(FileSystemEnumerationMode mode, uint32_t reserved, FileSystemOutputCapacityBytes outputCapacityBytes, uint8_t * outputRecords, uint8_t * pathOrVolumeText);
typedef Bool8 FileSystemGetCurrentDirectoryProc(uint16_t * destination);
typedef Win32DriveCapacity FileSystemGetFreeAndTotalBytesRegsProc(DosDriveLetterCode32 driveLetter);
typedef uint32_t FileSystemGetLastWriteDosDateProc(uint16_t * path, uint32_t * outDosDateTime);
typedef uint32_t FileSystemGetLastWriteTimeHighProc(uint16_t * path, uint32_t * outLastWriteTimeHigh);
typedef Bool8 FileSystemGetPositionProc(void * handle, uint32_t * outPosition);
typedef Bool8 FileSystemGetSizeProc(void * handle, uint32_t * outSize);
typedef uint32_t FileSystemGetVolumeSerialNumberProc(uint8_t * outputLabel, char * path);
typedef uint32_t FileSystemMoveProc(uint16_t * destinationPath, uint16_t * sourcePath);
typedef uint32_t FileSystemOpenProc(FileSystemOpenFlags openFlags, uint16_t * path, void * * outHandle);
typedef uint32_t FileSystemReadExactProc(FileIoByteCount byteCount, void * destination, void * handle);
typedef uint32_t FileSystemRemoveDirectoryProc(uint16_t * path);
typedef uint32_t FileSystemSeekProc(FileSystemSeekOrigin moveMethod, FileSystemFilePosition distance, void * handle);
typedef uint32_t FileSystemSetCurrentDirectoryProc(uint16_t * path);
typedef uint32_t FileSystemWriteExactOrFlushProc(FileIoByteCount byteCount, void * source, void * handle);

#endif /* THANDOR_PLATFORM_FILESYSTEM_TYPES_H */
