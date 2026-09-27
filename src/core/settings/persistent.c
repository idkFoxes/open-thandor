/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/persistent.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/settings/persistent.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/settings/persistent. */

/* Address: 0x00402C00.
   Saves the settings: mirrors g_LocaleCountryCodeOverride into the image and, when anything changed since the
   last load or save, writes the 200-byte image back to the settings file. The save result is not checked.
*/
void __thandor_preserve_eax PersistentSettings_Flush(void)

{
  if (g_PersistentSettings.image != NULL) {
    PersistentSettings_Write(g_LocaleCountryCodeOverride,PERSISTENT_SETTING_LOCALE_COUNTRY_CODE);
    if (g_PersistentSettings.dirtyWriteCount != 0) {
      FileSystem_WriteBufferToPath(PERSISTENT_SETTINGS_IMAGE_BYTES,g_PersistentSettings.image,
                                   g_PersistentSettings.path);
      g_PersistentSettings.dirtyWriteCount = 0;
    }
  }
}


/* Address: 0x00402B00.
   Loads the settings file into a fresh zeroed 200-byte image; a shorter file leaves the rest zero, so every
   PersistentSettings_Read beyond the loaded bytes falls back to its default. When the file is not found at
   its path it is looked up in the executable directory. Any failure leaves the image null (all defaults).
*/
void __thandor_void_preserve_eax_ecx PersistentSettings_Load(void)

{
  uint32_t *clearCursor;
  void *fileHandle;
  int dwordsRemaining;
  uint32_t byteCount;
  PersistentSettingsImage *image;
  ArenaAllocResult allocResult;
  FileSystemOpenResult openResult;
  RichTextCopyResult pathCopyResult;
  FileSystemSizeResult sizeResult;
  FileSystemReadResult readResult;

  Resource_Release(g_PersistentSettings.image);
  g_PersistentSettings.image = NULL;
  allocResult = g_MemoryApi.alloc(PERSISTENT_SETTINGS_IMAGE_BYTES);
  clearCursor = (uint32_t *)allocResult.payloadOrError;
  if (allocResult.failed) {
    return;
  }
  for (dwordsRemaining = PERSISTENT_SETTINGS_IMAGE_BYTES / 4; dwordsRemaining != 0; dwordsRemaining--) {
    *clearCursor = 0;
    clearCursor++;
  }
  image = (PersistentSettingsImage *)(clearCursor - PERSISTENT_SETTINGS_IMAGE_BYTES / 4);
  openResult = g_FileSystemOpen(0,g_PersistentSettings.path);
  fileHandle = (void *)openResult.handleOrError;
  if (openResult.failed) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,g_PersistentSettings.path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    if (openResult.failed) {
      g_MemoryApi.free(image);
      return;
    }
    /* The original also continues with EAX = the byte count returned by the path copy below as the
       file handle (MOV EBX,EAX at 0x00402B90), not the handle from this open. Kept as is. */
    pathCopyResult = RichTextCommandStream_CopyExpanded
                      (sizeof g_PersistentSettings.path,g_PersistentSettings.path,
                       (uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    fileHandle = (void *)pathCopyResult.bytesWritten;
  }
  sizeResult = g_FileSystemGetSize(fileHandle);
  if (!sizeResult.failed) {
    byteCount = PERSISTENT_SETTINGS_IMAGE_BYTES;
    if (sizeResult.sizeOrError < PERSISTENT_SETTINGS_IMAGE_BYTES) {
      byteCount = sizeResult.sizeOrError;
    }
    readResult = g_FileSystemReadExact(byteCount,image,fileHandle);
    if (!readResult.failed) {
      g_FileSystemClose(fileHandle);
      if (byteCount < PERSISTENT_SETTING_LOCALE_COUNTRY_CODE + 4) {
        g_PersistentSettings.image = image;
        g_PersistentSettings.loadedByteCount = byteCount;
        g_PersistentSettings.dirtyWriteCount = 0;
        return;
      }
      /* the country code override is only taken over when the file contains it */
      g_LocaleCountryCodeOverride = image->localeCountryCodeOverride;
      g_PersistentSettings.image = image;
      g_PersistentSettings.loadedByteCount = byteCount;
      g_PersistentSettings.dirtyWriteCount = 0;
      return;
    }
  }
  g_FileSystemClose(fileHandle);
  g_MemoryApi.free(image);
}


/* Address: 0x00402C50.
   Returns the setting dword at settingsOffsetBytes, or defaultValue when no settings file was loaded or the
   file was too short to contain it.
*/
uint32_t __thandor_eax_preserve_ecx_edx
PersistentSettings_Read
          (PersistentSettingsValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != NULL) &&
     (settingsOffsetBytes + 4 <= g_PersistentSettings.loadedByteCount)) {
    defaultValue = *(PersistentSettingsValue *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes);
  }
  return defaultValue;
}


/* Address: 0x00402CC0.
   Returns a pointer into the settings image at settingsOffsetBytes (not a copy), or fallback when no settings
   file was loaded or the file was too short to contain the whole region. Used for the stored names.
*/
void * __thandor_eax_preserve_ecx_edx
PersistentSettings_GetRegionOrFallback
          (PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != NULL) &&
     (settingsOffsetBytes + regionByteCount <= g_PersistentSettings.loadedByteCount)) {
    fallback = (uint8_t *)g_PersistentSettings.image + settingsOffsetBytes;
  }
  return fallback;
}


/* Address: 0x00402CF0.
   Copies a block (whole dwords only; trailing 1-3 bytes are dropped) into the settings image and marks it
   dirty, even when nothing changed. The bound is the image capacity, not the loaded size, and the loaded
   size is not extended, so a block past the end of a short file is saved but not read back until reload.
*/
void __thandor_void_preserve_eax_ecx_edx
PersistentSettings_WriteBlock
          (PersistentSettingsByteCount regionByteCount,uint32_t *source,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  uint32_t dwordsRemaining;
  uint32_t *destination;

  if ((g_PersistentSettings.image != NULL) &&
     (settingsOffsetBytes + regionByteCount < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) {
    destination = (uint32_t *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes);
    dwordsRemaining = regionByteCount >> 2;
    if (dwordsRemaining != 0) {
      for (; dwordsRemaining != 0; dwordsRemaining--) {
        *destination = *source;
        source++;
        destination++;
      }
      g_PersistentSettings.dirtyWriteCount++;
    }
  }
}


/* Address: 0x00402C80.
   Stores one setting dword in the image and marks it dirty, but only when the value actually changes. Like
   WriteBlock it checks against the image capacity, not the loaded size.
*/
void __thandor_void_preserve_eax_ecx_edx
PersistentSettings_Write
          (PersistentSettingsValue value,PersistentSettingsByteOffset settingsOffsetBytes)

{
  if (((g_PersistentSettings.image != NULL) &&
      (settingsOffsetBytes + 4 < PERSISTENT_SETTINGS_IMAGE_BYTES + 1)) &&
     (*(PersistentSettingsValue *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes) != value)) {
    *(PersistentSettingsValue *)((uint8_t *)g_PersistentSettings.image + settingsOffsetBytes) = value;
    g_PersistentSettings.dirtyWriteCount++;
  }
}
