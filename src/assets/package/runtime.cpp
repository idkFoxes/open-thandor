/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static PckMountSlot g_PackageMountSlots[1024] = {};

uint8_t *g_PackageScratchBuffer = nullptr;

THANDOR_ALIGN(4) uint16_t g_PackageLastErrorPath[256] = {};

/* Package_UpsertEntry: fills the PCK_ENTRY_PATH_UNITS code-unit path field (PckEntryHeader.path) at
   nameDestination with path: its code units up to and including the terminator (at most the whole field),
   then zeros.
   The original copied the full field (0x7B whole dwords = 492 bytes) whatever the path's length, so for the
   short save-game entry names (g_ArmyHexPathUtf16 ... g_OldunitHexPathUtf16, 0x12-0x1A bytes each) it read
   up to 0x1EC bytes past the string, into whatever variables follow it. Bounded here because that read runs
   past the caller's object; only the bytes behind the terminator in the path field of the entry header
   written to the save file change (zeros instead of memory contents). Every reader stops at the terminator
   (Package_FindEntryInMount / Package_FindEntryAcrossMounts compare up to it, Package_FindEntry's copy is
   then used as a string), so the save format and every result are unchanged. */
void Package_CopyEntryPathDwords(uint8_t *nameDestination,uint16_t *path)

{
  uint16_t *nameUnits;
  int unitIndex;

  nameUnits = (uint16_t *)nameDestination;
  unitIndex = 0;
  while (unitIndex < PCK_ENTRY_PATH_UNITS) {
    nameUnits[unitIndex] = path[unitIndex];
    unitIndex++;
    if (path[unitIndex - 1] == 0) break;
  }
  for (; unitIndex < PCK_ENTRY_PATH_UNITS; unitIndex++) {
    nameUnits[unitIndex] = 0;
  }
}

/* Loads path into a caller buffer of the given capacity: from the first mounted package that has it, otherwise
   from the loose file (PACKAGE_LOAD_* flags in the top two bits of the capacity select loose-only loading and a
   first try next to the executable). Returns true and stores the byte count in *outByteCountOrError; returns
   false with an error code there instead, FATAL_ERROR_OUT_OF_MEMORY when the entry does not fit (or is to be
   decoded into g_PackageScratchBuffer, which holds the packed data). outByteCountOrError may be NULL.
*/
Bool8 Package_LoadEntryIntoBuffer
          (PckLoadCapacityFlags bufferCapacityAndLoadFlags,uint8_t *destination,uint16_t *path,
           uint32_t *outByteCountOrError)

{
  uint32_t bufferCapacity;
  PckEntryHeader *entry;
  EngineFileHandle entryFileHandle;
  void *handle;
  uint32_t byteCount;
  uint32_t decodedByteCount;
  uint32_t decodeErrorCode;
  Bool8 decoded;
  uint32_t statusCode;
  uint32_t errorCode;

  bufferCapacity = bufferCapacityAndLoadFlags & PACKAGE_LOAD_CAPACITY_MASK;
  entry = nullptr;
  if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_SKIP_PACKAGES) == 0) {
    entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  }
  if (entry != nullptr) {
    if (entry->unpackedSize <= bufferCapacity && entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1 &&
        destination != g_PackageScratchBuffer) {
      decoded = Package_DecodeEntryInto(destination,entry,entryFileHandle,&decodedByteCount,&decodeErrorCode);
      if (outByteCountOrError != nullptr) {
        *outByteCountOrError = decoded ? decodedByteCount : decodeErrorCode;
      }
      return decoded;
    }
    Package_SetLastErrorPath(path);
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
  }
  else {
    /* not in a mounted package (or packages skipped): load the loose file */
    if ((bufferCapacityAndLoadFlags & PACKAGE_LOAD_EXECUTABLE_DIRECTORY_FIRST) == 0) {
      statusCode = g_FileSystemOpen(0,path,&handle);
    }
    else {
      WidePath_CombineDirectoryAndLeaf
                (g_FileSystemCombinedPathScratchUtf16,path,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      statusCode = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle);
      if (statusCode != 0) {
        statusCode = g_FileSystemOpen(0,path,&handle);
      }
    }
    if (statusCode != 0) {
      errorCode = statusCode; /* the open error code */
    }
    else {
      if (!g_FileSystemGetSize(handle,&byteCount)) {
        errorCode = byteCount; /* a failed size query leaves 0 in byteCount, which becomes the error code */
      }
      /* a file larger than the buffer is truncated to the capacity, unless that exceeds 8 MiB */
      else if (bufferCapacity < byteCount && PACKAGE_SCRATCH_BUFFER_BYTES - 1 < bufferCapacity) {
        errorCode = FATAL_ERROR_OUT_OF_MEMORY;
      }
      else {
        if (bufferCapacity < byteCount) {
          byteCount = bufferCapacity;
        }
        statusCode = g_FileSystemReadExact((FileIoByteCount)byteCount,destination,handle);
        if (statusCode == 0) {
          g_FileSystemClose(handle);
          if (outByteCountOrError != nullptr) {
            *outByteCountOrError = byteCount;
          }
          return true;
        }
        errorCode = statusCode;
      }
      g_FileSystemClose(handle);
    }
  }
  if (outByteCountOrError != nullptr) {
    *outByteCountOrError = errorCode;
  }
  return false;
}

/* Package_Mount and Package_MountLowPriority, once a free slot is chosen: opens path for writing (next to the
   executable first, then as given), allocates the entry-header array and reads the directory into
   mountSlot. Stores the file handle or the open/allocation/directory error code in *outFileHandleOrError (may
   be NULL); returns true on success. On a failed directory read the file is closed and the slot left free. */
static Bool8 Package_MountIntoSlot(PckMountSlot *mountSlot,uint16_t *path,uintptr_t *outFileHandleOrError)

{
  void *handle;
  PckEntryHeader *allocatedEntryHeaders;
  uint32_t openError;
  uint32_t allocError;
  uint32_t errorCode;

  WidePath_CombineDirectoryAndLeaf
            (g_FileSystemCombinedPathScratchUtf16,path,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  openError = g_FileSystemOpen
                    (FILESYSTEM_OPEN_WRITE_ACCESS,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&handle);
  if (openError != 0) {
    openError = g_FileSystemOpen(FILESYSTEM_OPEN_WRITE_ACCESS,path,&handle);
  }
  if (openError != 0) {
    errorCode = openError;
  }
  else {
    allocError = g_MemoryApi.alloc(PACKAGE_DIRECTORY_BYTES,(void **)&allocatedEntryHeaders);
    if (allocError == 0) {
      mountSlot->fileHandle = (EngineFileHandle)handle;
      mountSlot->entryHeaders = allocatedEntryHeaders;
      mountSlot->entryCount = 0;
      /* The original ignored the result and kept the package mounted with whatever the directory read
         left. Rejected here because an unread directory is not usable: the mount fails as for a missing
         file. Every valid archive (the game's packages, saves, a freshly created save package) reads. */
      if (Package_ReadDirectory((EngineFileHandle)handle,&errorCode)) {
        if (outFileHandleOrError != nullptr) {
          *outFileHandleOrError = (uintptr_t)handle;
        }
        return true;
      }
      Thandor_Log("Package_Mount: \"%ls\" has no readable directory (error 0x%08X)",(wchar_t *)path,errorCode);
      g_MemoryApi.free(allocatedEntryHeaders);
      mountSlot->fileHandle = 0;
      mountSlot->entryHeaders = nullptr;
      mountSlot->entryCount = 0;
    }
    else {
      errorCode = allocError;
    }
    g_FileSystemClose(handle);
  }
  if (outFileHandleOrError != nullptr) {
    *outFileHandleOrError = errorCode;
  }
  return false;
}

/* Like Package_Mount, but takes the last free mount slot: lookups scan the table from the front, so this
   archive loses against every other one. FileSystem_Init mounts engine.pck this way. Same result as
   Package_Mount.
*/
Bool8 Package_MountLowPriority(uint16_t *path,uintptr_t *outFileHandleOrError)

{
  int slotsRemaining;
  PckMountSlot *mountSlot;

  mountSlot = g_PackageMountSlots + (PACKAGE_MOUNT_SLOT_COUNT - 1);
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && mountSlot->fileHandle != 0) {
    mountSlot--;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    if (outFileHandleOrError != nullptr) {
      *outFileHandleOrError = FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
    }
    return false;
  }
  return Package_MountIntoSlot(mountSlot,path,outFileHandleOrError);
}

/* Loads an asset into a newly allocated buffer: from the first mounted package that has the path, otherwise as
   a loose file (first relative to the executable directory, then as given; FileSystem_LoadWholeFileNearExecutable).
   Returns the buffer (never NULL on success) and stores its byte count in *outByteCount; on failure returns
   NULL and stores the error code in *outErrorCode and leaves *outByteCount unchanged. Both pointers may be
   NULL. The core of Package_LoadEntry and Resource_Load (the original has the code twice).
*/
void *Package_LoadEntryWithSize(uint16_t *path,uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckEntryHeader *entry;
  EngineFileHandle entryFileHandle;
  void *buffer;
  uint32_t allocError;
  uint32_t decodeErrorCode;
  uint32_t errorCode;

  entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  if (entry == nullptr) {
    if (FileSystem_LoadWholeFileNearExecutable(path,&buffer,outByteCount,&errorCode)) {
      return buffer;
    }
  }
  else {
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
    /* the packed data is staged in the package scratch buffer, so it must fit there */
    if (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1) {
      allocError = g_MemoryApi.alloc(entry->unpackedSize,&buffer);
      if (allocError != 0) {
        errorCode = allocError;
      }
      else {
        if (Package_DecodeEntryInto((uint8_t *)buffer,entry,entryFileHandle,nullptr,&decodeErrorCode)) {
          if (outByteCount != nullptr) {
            *outByteCount = entry->unpackedSize;
          }
          return buffer;
        }
        errorCode = decodeErrorCode;
        g_MemoryApi.free(buffer);
      }
    }
  }
  if (outErrorCode != nullptr) {
    *outErrorCode = errorCode;
  }
  return nullptr;
}

/* Loads an asset into a newly allocated buffer (Package_LoadEntryWithSize without the byte count). The buffer
   is untyped here; callers cast it to their gfx, fld, lev, mdl, sound, text, ... layout. Returns the buffer
   (never NULL on success); on failure returns NULL and stores the error code in *outErrorCode (outErrorCode
   may be NULL).
*/
void *Package_LoadEntry(uint16_t *path,uint32_t *outErrorCode)

{
  static int loggedFailures; /* open-thandor diagnostics: first failed loads with their caller stack */
  void *buffer;
  uint32_t errorCode;

  buffer = Package_LoadEntryWithSize(path,nullptr,&errorCode);
  if (buffer != nullptr) {
    return buffer;
  }
  if (loggedFailures++ < 8) {
    Thandor_Log("Package_LoadEntry failed: \"%ls\" (error 0x%08X)", (wchar_t *)path, errorCode);
    Thandor_LogStack("  load failure stack", errorCode);
  }
  if (outErrorCode != nullptr) {
    *outErrorCode = errorCode;
  }
  return nullptr;
}

/* Mounts the package archive path (next to the executable first, then as given) in the first free mount slot
   and reads its directory into a fresh PACKAGE_DIRECTORY_BYTES entry-header array. Lookups scan the slots in
   the same order, so earlier mounts win. Returns true and stores the file handle in *outFileHandleOrError;
   returns false with an error code there instead when no slot is free, the file cannot be opened, the
   allocation fails or the directory cannot be read (see Package_MountIntoSlot). outFileHandleOrError may be
   NULL.
*/
Bool8 Package_Mount(uint16_t *path,uintptr_t *outFileHandleOrError)

{
  int slotsRemaining;
  PckMountSlot *mountSlot;

  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && mountSlot->fileHandle != 0) {
    mountSlot++;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    if (outFileHandleOrError != nullptr) {
      *outFileHandleOrError = FATAL_ERROR_GENERAL_FAILURE; /* no free slot */
    }
    return false;
  }
  return Package_MountIntoSlot(mountSlot,path,outFileHandleOrError);
}

/* The mount slot holding fileHandle, or NULL when it is not mounted. A zero handle is never found (it would
   otherwise match a free slot). Used by Package_FindEntry and Package_Unmount. */
static PckMountSlot *Package_FindMountSlot(EngineFileHandle fileHandle)

{
  PckMountSlot *mountSlot;
  int slotsRemaining;

  if (fileHandle == 0) {
    return nullptr;
  }
  mountSlot = g_PackageMountSlots;
  for (slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (fileHandle == mountSlot->fileHandle) {
      return mountSlot;
    }
    mountSlot++;
  }
  return nullptr;
}

/* Package_FindEntry sort order: true when record later is smaller than record front, comparing the whole
   PCK_ENTRY_HEADER_BYTES record as unsigned UTF-16 code units (the path first, then whatever the output
   record held behind it). Equal records are not smaller. */
static Bool8 Package_FoundEntryIsSmaller(const PckEntryHeader *later,const PckEntryHeader *front)

{
  const uint16_t *laterUnits;
  const uint16_t *frontUnits;
  int unitIndex;

  laterUnits = (const uint16_t *)later;
  frontUnits = (const uint16_t *)front;
  for (unitIndex = 0; unitIndex < PCK_ENTRY_HEADER_BYTES / 2; unitIndex++) {
    if (laterUnits[unitIndex] != frontUnits[unitIndex]) {
      return laterUnits[unitIndex] < frontUnits[unitIndex];
    }
  }
  return false;
}

/* Exchanges two whole PCK_ENTRY_HEADER_BYTES records, dword by dword. */
static void Package_SwapFoundEntries(PckEntryHeader *first,PckEntryHeader *second)

{
  uint32_t *firstDwords;
  uint32_t *secondDwords;
  uint32_t swappedDword;
  int dwordIndex;

  firstDwords = (uint32_t *)first;
  secondDwords = (uint32_t *)second;
  for (dwordIndex = 0; dwordIndex < PCK_ENTRY_HEADER_BYTES / 4; dwordIndex++) {
    swappedDword = firstDwords[dwordIndex];
    firstDwords[dwordIndex] = secondDwords[dwordIndex];
    secondDwords[dwordIndex] = swappedDword;
  }
}

/* Package_FindEntry, final step: exchange sort of the entryCount found records. Every record is compared
   with each later one and the two are swapped when the later one is smaller. */
static void Package_SortFoundEntries(PckEntryHeader *entries,int entryCount)

{
  int frontIndex;
  int laterIndex;

  for (frontIndex = 0; frontIndex + 1 < entryCount; frontIndex++) {
    for (laterIndex = frontIndex + 1; laterIndex < entryCount; laterIndex++) {
      if (Package_FoundEntryIsSmaller(&entries[laterIndex],&entries[frontIndex])) {
        Package_SwapFoundEntries(&entries[frontIndex],&entries[laterIndex]);
      }
    }
  }
}

/* Lists the entries of the mounted package fileHandle whose path matches pattern (Package_WildcardPathMatches):
   copies each path into a PCK_ENTRY_HEADER_BYTES output record while the capacity lasts and sorts the records
   by path (UTF-16 code-unit order). Returns true with the match count in *outMatchCount (each record is
   PCK_ENTRY_HEADER_BYTES); returns false when the handle is not mounted, leaving *outMatchCount unchanged
   (the original returned FATAL_ERROR_GENERAL_FAILURE, which no caller reads).
*/
Bool8 Package_FindEntry(PckOutputCapacityBytes outputCapacityBytes,PckEntryHeader *outputEntries,
                       uint16_t *pattern,EngineFileHandle fileHandle,uint32_t *outMatchCount)

{
  PckMountSlot *mountSlot;
  PckEntryHeader *entry;
  PckEntryHeader *outputEntry;
  PckEntryCount entriesRemaining;
  int matchedCount;
  int unitIndex;

  mountSlot = Package_FindMountSlot(fileHandle);
  if (mountSlot == nullptr) {
    return false;
  }
  matchedCount = 0;
  outputEntry = outputEntries;
  entry = mountSlot->entryHeaders;
  for (entriesRemaining = mountSlot->entryCount; entriesRemaining != 0; entriesRemaining--) {
    if (!Package_WildcardPathMatches(pattern,entry->path)) { /* false means match */
      if (outputCapacityBytes < PCK_ENTRY_HEADER_BYTES) {
        /* output full: stop with the matches so far.
           Original quirk: these matches are returned unsorted */
        *outMatchCount = matchedCount;
        return true;
      }
      outputCapacityBytes = outputCapacityBytes - PCK_ENTRY_HEADER_BYTES;
      /* only the path is copied; the rest of the output record is left as it was */
      for (unitIndex = 0; unitIndex < PCK_ENTRY_PATH_UNITS; unitIndex++) {
        outputEntry->path[unitIndex] = entry->path[unitIndex];
      }
      outputEntry++;
      matchedCount++;
    }
    entry++;
  }
  Package_SortFoundEntries(outputEntries,matchedCount);
  *outMatchCount = matchedCount;
  return true;
}

/* Unmounts the package fileHandle: frees its entry-header array, closes the file and clears the mount slot.
   Does nothing for a zero or unknown handle.
*/
void Package_Unmount(EngineFileHandle fileHandle)

{
  PckMountSlot *mountSlot;

  mountSlot = Package_FindMountSlot(fileHandle);
  if (mountSlot == nullptr) {
    return;
  }
  g_MemoryApi.free(mountSlot->entryHeaders);
  g_FileSystemClose(THANDOR_PTR(fileHandle));
  mountSlot->fileHandle = 0;
  mountSlot->entryHeaders = nullptr;
  mountSlot->entryCount = 0;
}

/* Compares a UTF-16 archive path against a pattern for the package entry search. '?' matches any one code
   unit; '*' only skips the candidate to its next dot or terminator (no full globbing), which is enough for
   patterns like "level\*.lev". The comparison is case-sensitive. Returns false on a match.
*/
Bool8 Package_WildcardPathMatches(uint16_t *pattern,uint16_t *candidate)

{
  uint16_t patternCodeUnit;

  do {
    patternCodeUnit = *pattern++;
    while (patternCodeUnit == '*') {
      while (*candidate != '.' && *candidate != 0) {
        candidate++;
      }
      patternCodeUnit = *pattern++;
    }
    if (patternCodeUnit != '?' && patternCodeUnit != *candidate) {
      return true; /* mismatch */
    }
    candidate++;
  } while (patternCodeUnit != 0);
  return false; /* both ended together */
}

/* Reads the packed data of entry from the package fileHandle into g_PackageScratchBuffer and unpacks it into
   destination with the decoder of its compression method (g_PckDecoderTable). Returns true on success with the
   decoder's byte count in *outByteCount; on failure returns false with the seek, read or decoder error code in
   *outErrorCode and leaves the entry path in g_PackageLastErrorPath. Either out pointer may be NULL.
*/
Bool8 Package_DecodeEntryInto(uint8_t *destination,PckEntryHeader *entry,EngineFileHandle fileHandle,
                             uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckCompressionMethod entryCompression;
  uint32_t decoderStatusCode;

  /* The original indexed g_PckDecoderTable with the file's method unchecked and let the stored decoder copy
     packedSize bytes into the unpackedSize buffer. Bounded here because both come from the file: an unknown
     method fails, and so does a stored entry whose packed size exceeds its unpacked size rounded up to whole
     dwords (Package_UpsertEntry writes stored entries dword-aligned, and the allocations round up further, so
     those few bytes behind the data are written as in the original). */
  entryCompression = entry->compressionMethod;
  if ((uint32_t)entryCompression >= sizeof g_PckDecoderTable / sizeof g_PckDecoderTable[0] ||
      (entryCompression == PCK_COMPRESSION_STORED &&
       entry->packedSize > (entry->unpackedSize + 3 & PACKAGE_DWORD_ALIGN_MASK))) {
    Thandor_Log("Package_DecodeEntryInto: \"%ls\" has method %d, packed %u, unpacked %u; entry rejected",
                (wchar_t *)entry->path,entryCompression,(unsigned)entry->packedSize,
                (unsigned)entry->unpackedSize);
    decoderStatusCode = FATAL_ERROR_GENERAL_FAILURE;
    Package_SetLastErrorPath(entry->path);
    if (outErrorCode != nullptr) {
      *outErrorCode = decoderStatusCode;
    }
    return false;
  }
  decoderStatusCode = g_FileSystemSeek
                    (FILESYSTEM_SEEK_BEGIN,entry->runtimePayloadOffset + PCK_ENTRY_HEADER_BYTES,THANDOR_PTR(fileHandle));
  if (decoderStatusCode == 0) {
    decoderStatusCode = g_FileSystemReadExact(entry->packedSize,g_PackageScratchBuffer,THANDOR_PTR(fileHandle));
    if (decoderStatusCode == 0) {
      /* the decoder stores its byte count straight into *outByteCount (NULL is allowed) */
      if (g_PckDecoderTable[entryCompression]
              (entry->unpackedSize,destination,entry->packedSize,g_PackageScratchBuffer,outByteCount,
               &decoderStatusCode)) {
        return true;
      }
    }
  }
  Package_SetLastErrorPath(entry->path);
  if (outErrorCode != nullptr) {
    *outErrorCode = decoderStatusCode;
  }
  return false;
}

/* Stores path in g_PackageLastErrorPath for the fatal-error message of a failed load: its code units up to
   and including the terminator, at most 0x100 (a longer path is cut and terminated in the last unit), then
   zeros to the end of the buffer.
   The original measured the length in code units (at most 0x100, terminator included) but used it as a byte
   count, copying twice as many code units as the path has: past the caller's string (some callers pass an
   asset buffer, whose magic then becomes the text) and, for paths over 0x80 units, on into
   g_FatalErrorDetail1Utf16 behind the 0x100-unit buffer. Bounded here because both reads and the write run
   past their objects. Only units behind the terminator change, which no reader looks at; the
   g_FatalErrorDetail1Utf16 spill is dropped (it only showed the bytes behind the path as garbage in the
   fatal-error box), and a path of 0x100 units or more now ends in a terminator. */
void Package_SetLastErrorPath(uint16_t *path)

{
  int unitIndex;

  unitIndex = 0;
  while (unitIndex < 256) {
    g_PackageLastErrorPath[unitIndex] = path[unitIndex];
    unitIndex++;
    if (path[unitIndex - 1] == 0) break;
  }
  if (unitIndex == 256) {
    g_PackageLastErrorPath[255] = 0;
  }
  for (; unitIndex < 256; unitIndex++) {
    g_PackageLastErrorPath[unitIndex] = 0;
  }
}

/* Finds the entry whose name equals path exactly (no wildcards, case-sensitive: package paths are stored in
   lower case) in the package mounted as fileHandle and returns its entry header. Returns NULL when no entry
   matches, and also for a path longer than an entry name, an unmounted handle or an empty package. Called
   directly by Package_UpsertEntry and Package_DeleteEntry (no callback table).
*/
PckEntryHeader *Package_FindEntryInMount(uint16_t *path,EngineFileHandle fileHandle)

{
  int lengthRemaining; /* PCK_ENTRY_PATH_UNITS minus the path length, terminator included */
  int remainingCount;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  Bool8 matched;
  PckEntryHeader *currentEntry;

  matched = true;
  lengthRemaining = PCK_ENTRY_PATH_UNITS;
  pathCursor = path;
  do {
    if (lengthRemaining == 0) break;
    lengthRemaining--;
    matched = *pathCursor == 0;
    pathCursor++;
  } while (!matched);
  if (!matched) {
    return nullptr; /* path too long */
  }
  mountSlot = g_PackageMountSlots;
  remainingCount = PACKAGE_MOUNT_SLOT_COUNT;
  while (fileHandle != mountSlot->fileHandle) {
    mountSlot++;
    remainingCount--;
    if (remainingCount == 0) {
      return nullptr; /* not mounted */
    }
  }
  currentEntry = mountSlot->entryHeaders;
  entriesRemaining = mountSlot->entryCount;
  if (entriesRemaining == 0) {
    return nullptr; /* empty package */
  }
  do {
    /* compare code units over the path length including its terminator, stopping at the first difference */
    matched = false;
    remainingCount = -(lengthRemaining - PCK_ENTRY_PATH_UNITS);
    pathCursor = path;
    nameCursor = currentEntry->path;
    while (remainingCount != 0) {
      matched = *pathCursor == *nameCursor;
      remainingCount--;
      pathCursor++;
      nameCursor++;
      if (!matched) break;
    }
    if (matched) {
      return currentEntry;
    }
    currentEntry++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return nullptr;
}

/* Finds path in the mounted packages, scanning the mount slots from the front so that the first mounted
   package that has the entry wins. The path is lowercased in place first (package paths are stored in lower
   case). Returns the entry header and stores the package's handle in *outFileHandle; returns NULL (leaving
   *outFileHandle unchanged) when the path is too long for an entry or no package has it. A found entry is
   never NULL.
*/
PckEntryHeader *Package_FindEntryAcrossMounts(uint16_t *path,EngineFileHandle *outFileHandle)

{
  uint32_t codeUnit;
  int pathLength;
  int compareRemaining;
  int slotsRemaining;
  PckEntryCount entriesRemaining;
  PckMountSlot *mountSlot;
  uint16_t *pathCursor;
  uint16_t *nameCursor;
  Bool8 matched;
  PckEntryHeader *currentEntry;

  /* Lowercase the path in place and count its code units, terminator included. A path that reaches
     PCK_ENTRY_PATH_UNITS code units (terminator included) is rejected after its last unit was written. */
  pathLength = 0;
  do {
    codeUnit = path[pathLength];
    if ('A' - 1 < codeUnit && codeUnit < 'Z' + 1) {
      codeUnit = codeUnit + ('a' - 'A');
    }
    path[pathLength] = (uint16_t)codeUnit;
    pathLength++;
    if (pathLength == PCK_ENTRY_PATH_UNITS) return nullptr; /* path too long */
  } while (codeUnit != 0);
  mountSlot = g_PackageMountSlots;
  for (slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    currentEntry = mountSlot->entryHeaders;
    if (currentEntry != nullptr) {
      for (entriesRemaining = mountSlot->entryCount; entriesRemaining != 0;
           entriesRemaining = entriesRemaining - 1) {
        /* compare the lowercased path, terminator included, with the entry's path */
        matched = false;
        compareRemaining = pathLength;
        pathCursor = path;
        nameCursor = currentEntry->path;
        while (compareRemaining != 0) {
          matched = *pathCursor == *nameCursor;
          compareRemaining--;
          pathCursor++;
          nameCursor++;
          if (!matched) break;
        }
        if (matched) {
          *outFileHandle = mountSlot->fileHandle;
          return currentEntry;
        }
        currentEntry++;
      }
    }
    mountSlot++;
  }
  return nullptr;
}

/* Package_ReadDirectory, once the slot is found: reads the archive header and every entry header of
   fileHandle into mountSlot. Returns 0 or the file-system error code of the failed seek/read, or
   FATAL_ERROR_GENERAL_FAILURE when the archive has more entries than the PACKAGE_DIRECTORY_BYTES array holds.
   The original stored the entry count before the reads and wrote any count into the array; on a failure it
   left the count with the headers not read (uninitialised in a fresh array). Bounded here because the count
   comes from the file: a failure leaves the slot with no entries, and the count is only stored once every
   header is read. The game's archives hold at most 423 entries. */
static uint32_t Package_ReadDirectoryIntoSlot(PckMountSlot *mountSlot,EngineFileHandle fileHandle)

{
  uint32_t statusCode;
  PckEntryCount entryCount;
  PckEntryCount entriesRemaining;
  FileSystemFilePosition entryHeaderOffset;
  PckEntryHeader *entryHeader;

  entryHeader = mountSlot->entryHeaders;
  mountSlot->entryCount = 0;
  statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  statusCode = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,g_PackageScratchBuffer,THANDOR_PTR(fileHandle));
  if (statusCode != 0) {
    return statusCode;
  }
  entryCount = ((PckArchiveHeader *)g_PackageScratchBuffer)->entryCount;
  if (entryCount > PACKAGE_DIRECTORY_BYTES / PCK_ENTRY_HEADER_BYTES) {
    Thandor_Log("Package_ReadDirectory: %u entries, at most %u fit; package rejected",
                (unsigned)entryCount,(unsigned)(PACKAGE_DIRECTORY_BYTES / PCK_ENTRY_HEADER_BYTES));
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  /* each entry is its header followed directly by its packed payload */
  entryHeaderOffset = PCK_ENTRY_HEADER_BYTES;
  for (entriesRemaining = entryCount; entriesRemaining != 0; entriesRemaining--) {
    statusCode = g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,entryHeader,THANDOR_PTR(fileHandle));
    if (statusCode != 0) {
      Thandor_Log("Package_ReadDirectory: entry header %u of %u not read (error 0x%08X); package rejected",
                  (unsigned)(entryCount - entriesRemaining),(unsigned)entryCount,statusCode);
      return statusCode;
    }
    entryHeader->runtimePayloadOffset = entryHeaderOffset;
    entryHeaderOffset = entryHeaderOffset + entryHeader->packedSize + PCK_ENTRY_HEADER_BYTES;
    entryHeader++;
    statusCode = g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,entryHeaderOffset,THANDOR_PTR(fileHandle));
    if (statusCode != 0) {
      return statusCode;
    }
  }
  mountSlot->entryCount = entryCount;
  return 0;
}

/* Loads the directory of the mounted package fileHandle into its mount slot: reads the archive header for the
   entry count, then every entry header, recording the file offset of the entry (its header; the packed
   payload follows it) and seeking past the payload to the next header. Returns true on success; false with
   the file-system error, or FATAL_ERROR_GENERAL_FAILURE when fileHandle is not mounted, in *outErrorCode
   (which may be NULL). The original also returned the last seek position on success; no caller used it.
*/
Bool8 Package_ReadDirectory(EngineFileHandle fileHandle,uint32_t *outErrorCode)

{
  uint32_t statusCode;
  int slotsRemaining;
  PckMountSlot *mountSlot;

  /* no zero-handle check here: a zero handle matches the first free slot */
  mountSlot = g_PackageMountSlots;
  slotsRemaining = PACKAGE_MOUNT_SLOT_COUNT;
  while (slotsRemaining != 0 && fileHandle != mountSlot->fileHandle) {
    mountSlot++;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    statusCode = FATAL_ERROR_GENERAL_FAILURE; /* not mounted */
  }
  else {
    statusCode = Package_ReadDirectoryIntoSlot(mountSlot,fileHandle);
    if (statusCode == 0) {
      return true;
    }
  }
  if (outErrorCode != nullptr) {
    *outErrorCode = statusCode;
  }
  return false;
}
