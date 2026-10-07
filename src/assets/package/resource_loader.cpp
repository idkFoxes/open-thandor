/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/resource_loader.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/resource_loader.h>
#include <thandor/thandor.h>

/* Loads a whole resource into a fresh arena buffer. A mounted package entry is decoded into the buffer;
   otherwise the loose file is read, first from the executable's directory, then from the path as given.
   Returns true with the buffer in *outBuffer and its byte count in *outByteCount. On failure returns false
   with the file-system, decoder or out-of-memory code in *outErrorCode and leaves *outBuffer and
   *outByteCount unchanged. outByteCount and outErrorCode may be NULL. Same load as Package_LoadEntry (without
   its failure log): both use Package_LoadEntryWithSize.
*/
bool Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode)

{
  void *buffer;

  buffer = Package_LoadEntryWithSize(path,outByteCount,outErrorCode);
  if (buffer == nullptr) {
    return false;
  }
  *outBuffer = buffer;
  return true;
}

/* Frees a buffer returned by Resource_Load (or Package_LoadEntry) back to the arena heap.
*/
void Resource_Release(void *resourceBuffer)

{
  g_MemoryApi.free(resourceBuffer);
}
