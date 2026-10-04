/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/resource_loader.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/resource_loader.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/package/resource_loader. */

/* Loads a whole resource into a fresh arena buffer. A mounted package entry is decoded into the buffer;
   otherwise the loose file is read, first from the executable's directory, then from the path as given.
   Returns true with the buffer in *outBuffer and its byte count in *outByteCount. On failure returns false
   with the file-system, decoder or out-of-memory code in *outErrorCode and leaves *outBuffer and
   *outByteCount unchanged. outByteCount and outErrorCode may be NULL.
*/
Bool8 Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckEntryHeader *entry;
  uint32_t fileSize;
  uint32_t errorCode;
  void *fileHandle;
  void *buffer;
  Bool8 gotSize;
  EngineFileHandle entryFileHandle;

  entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  if (entry == NULL) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    errorCode = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&fileHandle);
    if (errorCode != 0) {
      errorCode = g_FileSystemOpen(0,path,&fileHandle);
    }
    if (errorCode == 0) {
      gotSize = g_FileSystemGetSize(fileHandle,&fileSize);
      errorCode = fileSize; /* 0 when the size query failed */
      if (gotSize) {
        if (g_MemoryApi.alloc(fileSize,&buffer) != 0) {
          /* the requested size becomes the detail line of the out-of-memory message */
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)fileSize,g_FatalErrorDetail1Utf16);
          errorCode = FATAL_ERROR_OUT_OF_MEMORY;
        }
        else {
          errorCode = g_FileSystemReadExact((FileIoByteCount)fileSize,buffer,fileHandle);
          if (errorCode == 0) {
            g_FileSystemClose(fileHandle);
            *outBuffer = buffer;
            if (outByteCount != NULL) {
              *outByteCount = fileSize;
            }
            return true;
          }
          g_MemoryApi.free(buffer);
        }
      }
      g_FileSystemClose(fileHandle);
    }
  }
  else {
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
    if (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1) {
      errorCode = g_MemoryApi.alloc(entry->unpackedSize,&buffer);
      if (errorCode == 0) {
        if (Package_DecodeEntryInto((uint8_t *)buffer,entry,entryFileHandle,NULL,&errorCode)) {
          *outBuffer = buffer;
          if (outByteCount != NULL) {
            *outByteCount = entry->unpackedSize;
          }
          return true;
        }
        g_MemoryApi.free(buffer);
      }
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return false;
}

/* Frees a buffer returned by Resource_Load (or Package_LoadEntry) back to the arena heap.
*/
void Resource_Release(void *resourceBuffer)

{
  g_MemoryApi.free(resourceBuffer);
}
