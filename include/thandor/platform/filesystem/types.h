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

using FileIoByteCount = uint32_t;
using FileSystemCloseProc = void (void * handle);
using FileSystemCreateDirectoryRecursiveProc = uint32_t (FileSystemCreateDirectoryFlags flags, uint16_t * path);
using FileSystemDeleteProc = uint32_t (uint32_t unusedFlags, uint16_t * path);
using FileSystemEnumerateDirectoryOrVolumeEntriesProc = uint32_t (FileSystemEnumerationMode mode, uint32_t reserved, FileSystemOutputCapacityBytes outputCapacityBytes, uint8_t * outputRecords, uint8_t * pathOrVolumeText);
using FileSystemGetPositionProc = Bool8 (void * handle, uint32_t * outPosition);
using FileSystemGetSizeProc = Bool8 (void * handle, uint32_t * outSize);
using FileSystemOpenProc = uint32_t (FileSystemOpenFlags openFlags, uint16_t * path, void * * outHandle);
using FileSystemReadExactProc = uint32_t (FileIoByteCount byteCount, void * destination, void * handle);
using FileSystemSeekProc = uint32_t (FileSystemSeekOrigin moveMethod, FileSystemFilePosition distance, void * handle);
using FileSystemWriteExactOrFlushProc = uint32_t (FileIoByteCount byteCount, void * source, void * handle);

#endif /* THANDOR_PLATFORM_FILESYSTEM_TYPES_H */
