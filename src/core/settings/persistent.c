/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/persistent.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/settings/persistent.h>

/* Implementation ownership: core/settings/persistent. */

/* Address: 0x00402C00.
   Ownership: core/settings/persistent.
   Purpose: When an image exists, first mirrors g_LocaleCountryCodeOverride into image offset 0x38 through
   PersistentSettings_WriteDword. If dirtyWriteCount is nonzero, writes exactly 200 bytes to runtime.path and then
   resets dirtyWriteCount to zero without checking the save result.
   Local calls: PersistentSettings_WriteDword.
   Cross-module calls: FileSystem_WriteBufferToPathCf [platform/filesystem/win32].
*/
void __thandor_preserve_eax PersistentSettings_Flush(void)

{
  if (g_PersistentSettings.image != (PersistentSettingsImage *)0x0) {
    PersistentSettings_WriteDword(g_LocaleCountryCodeOverride,0x38);
    if (g_PersistentSettings.dirtyWriteCount != 0) {
      FileSystem_WriteBufferToPathCf(200,g_PersistentSettings.image,g_PersistentSettings.path);
      g_PersistentSettings.dirtyWriteCount = 0;
    }
  }
  return;
}


/* Address: 0x00402B00.
   Ownership: core/settings/persistent.
   Purpose: Releases the previous image, allocates and zeroes a fixed 200-byte replacement, and tries to open the
   current UTF-16 path in mode zero. If the direct open fails, an existing path resolver builds an alternate path,
   that path is retried, and the resolved path is copied back into the 0x200-byte runtime path buffer. The function
   reads min(fileSize,200), closes the file, publishes image/loadedByteCount/dirtyWriteCount, and adopts image
   offset 0x38 as g_LocaleCountryCodeOverride only when at least 0x3C bytes were loaded. Read, size, open, or
   allocation failure frees the new allocation and leaves image null. No stable scalar return is defined.
   Cross-module calls: Resource_Release [assets/resource/runtime], WidePath_CombineDirectoryAndLeaf
   [core/text/path], RichTextCommandStream_CopyExpandedCf [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx PersistentSettings_Load(void)

{
  dword *settingsClearCursor;
  void *handle;
  int iVar1;
  uint byteCount;
  PersistentSettingsImage *destination;
  ArenaAllocEaxCf5 AVar2;
  FileSystemOpenEaxCf5 FVar3;
  RichTextCopyExpandedEaxCf5 RVar4;
  FileSystemSizeEaxCf5 FVar5;
  FileSystemReadEaxCf5 FVar6;
  
  Resource_Release(g_PersistentSettings.image);
  g_PersistentSettings.image = (PersistentSettingsImage *)0x0;
  AVar2 = (*g_MemoryApi.alloc)(200);
  settingsClearCursor = (dword *)AVar2.eax;
  if (AVar2.carry) {
    return;
  }
  for (iVar1 = 0x32; iVar1 != 0; iVar1 = iVar1 + -1) {
    *settingsClearCursor = 0;
    settingsClearCursor = settingsClearCursor + 1;
  }
  destination = (PersistentSettingsImage *)(settingsClearCursor + -0x32);
  FVar3 = (*g_FileSystemOpenCf)(0,g_PersistentSettings.path);
  handle = (void *)FVar3.eax;
  if (FVar3.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,g_PersistentSettings.path,
               (word *)&g_ExecutableDirectoryUtf16);
    FVar3 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    if (FVar3.carry) goto PersistentSettings_Load_FreeTemporaryImageAfterOpenOrReadFailure;
    RVar4 = RichTextCommandStream_CopyExpandedCf
                      (0x200,g_PersistentSettings.path,(word *)&g_FileSystemCombinedPathScratchUtf16
                      );
    handle = (void *)RVar4.eax;
  }
  FVar5 = (*g_FileSystemGetSizeCf)(handle);
  if (!FVar5.carry) {
    byteCount = 200;
    if (FVar5.eax < 200) {
      byteCount = FVar5.eax;
    }
    FVar6 = (*g_FileSystemReadExactCf)(byteCount,destination,handle);
    if (!FVar6.carry) {
      (*g_FileSystemClose)(handle);
      if (byteCount < 0x3c) {
        g_PersistentSettings.image = destination;
        g_PersistentSettings.loadedByteCount = byteCount;
        g_PersistentSettings.dirtyWriteCount = 0;
        return;
      }
      g_LocaleCountryCodeOverride = settingsClearCursor[-0x24];
      g_PersistentSettings.image = destination;
      g_PersistentSettings.loadedByteCount = byteCount;
      g_PersistentSettings.dirtyWriteCount = 0;
      return;
    }
  }
  (*g_FileSystemClose)(handle);
PersistentSettings_Load_FreeTemporaryImageAfterOpenOrReadFailure:
  (*g_MemoryApi.free)(destination);
  return;
}


/* Address: 0x00402C50.
   Ownership: core/settings/persistent.
   Purpose: Returns the dword at image+offset only when image is non-null and unsigned offset+4 is no greater than
   loadedByteCount. Otherwise returns defaultValue. No alignment check is performed. Typed parameters: p0
   defaultValue→PersistentSettingsDwordValue_V342. Calling convention, exact VariableStorage serialization,
   function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
dword __thandor_eax_preserve_ecx_edx
PersistentSettings_ReadDword
          (PersistentSettingsDwordValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != (PersistentSettingsImage *)0x0) &&
     (settingsOffsetBytes + 4 <= g_PersistentSettings.loadedByteCount)) {
    defaultValue = *(PersistentSettingsDwordValue *)
                    ((g_PersistentSettings.image)->reserved50_5B + (settingsOffsetBytes - 0x50));
  }
  return defaultValue;
}


/* Address: 0x00402CC0.
   Ownership: core/settings/persistent.
   Purpose: Returns image+offset only when image is non-null and unsigned offset+byteCount is no greater than
   loadedByteCount. Otherwise returns fallback. The returned region is not copied and may be unaligned.
*/
void * __thandor_eax_preserve_ecx_edx
PersistentSettings_GetRegionOrFallback
          (PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  if ((g_PersistentSettings.image != (PersistentSettingsImage *)0x0) &&
     (settingsOffsetBytes + regionByteCount <= g_PersistentSettings.loadedByteCount)) {
    fallback = (g_PersistentSettings.image)->reserved50_5B + (settingsOffsetBytes - 0x50);
  }
  return fallback;
}


/* Address: 0x00402CF0.
   Ownership: core/settings/persistent.
   Purpose: When image is non-null and unsigned offset+byteCount is no greater than 200, copies floor(byteCount/4)
   dwords forward from source to image+offset with rep movsd. Trailing one to three bytes are ignored. A nonempty
   copy increments dirtyWriteCount once even when bytes are unchanged. loadedByteCount is not extended.
*/
void __thandor_void_preserve_eax_ecx_edx
PersistentSettings_WriteDwords
          (PersistentSettingsByteCount regionByteCount,dword *source,
          PersistentSettingsByteOffset settingsOffsetBytes)

{
  uint dwordsRemaining;
  dword *destinationDwordCursor;
  
  if ((g_PersistentSettings.image != (PersistentSettingsImage *)0x0) &&
     (settingsOffsetBytes + regionByteCount < 0xc9)) {
    destinationDwordCursor =
         (dword *)((g_PersistentSettings.image)->reserved50_5B + (settingsOffsetBytes - 0x50));
    dwordsRemaining = regionByteCount >> 2;
    if (dwordsRemaining != 0) {
      for (; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
        *destinationDwordCursor = *source;
        source = source + 1;
        destinationDwordCursor = destinationDwordCursor + 1;
      }
      g_PersistentSettings.dirtyWriteCount = g_PersistentSettings.dirtyWriteCount + 1;
    }
  }
  return;
}


/* Address: 0x00402C80.
   Ownership: core/settings/persistent.
   Purpose: When image is non-null and unsigned offset+4 is no greater than the fixed 200-byte capacity, writes
   value only when it differs from the stored dword and increments dirtyWriteCount once. The check does not use
   loadedByteCount and no alignment check is performed. Typed parameters: p0
   value→PersistentSettingsDwordValue_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
PersistentSettings_WriteDword
          (PersistentSettingsDwordValue value,PersistentSettingsByteOffset settingsOffsetBytes)

{
  if (((g_PersistentSettings.image != (PersistentSettingsImage *)0x0) &&
      (settingsOffsetBytes + 4 < 0xc9)) &&
     (*(PersistentSettingsDwordValue *)
       ((g_PersistentSettings.image)->reserved50_5B + (settingsOffsetBytes - 0x50)) != value)) {
    *(PersistentSettingsDwordValue *)
     ((g_PersistentSettings.image)->reserved50_5B + (settingsOffsetBytes - 0x50)) = value;
    g_PersistentSettings.dirtyWriteCount = g_PersistentSettings.dirtyWriteCount + 1;
  }
  return;
}

