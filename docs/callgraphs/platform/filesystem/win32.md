# Call graph: `platform/filesystem/win32`

26 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00575CB0` **FileSystem_Init** — local: `Win32File_OpenCf`, `Win32File_GetSizeCf`, `Win32File_ReadExactCf`, `Win32File_Close`, `Win32File_GetCurrentDirectoryCf`; cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`, `WidePath_SplitParentAndLeaf` → `core/text/path`, `ArenaHeap_Alloc` → `core/memory/allocator`, `FatalError_Exit` → `core/error/runtime`, `WidePath_CombineDirectoryAndLeaf` → `core/text/path`, `ArenaHeap_Free` → `core/memory/allocator`, `Package_MountLowPriority` → `assets/package/runtime`
- `0x005762F0` **Win32File_GetLastWriteDosDateCf** — local: `Win32File_OpenCf`, `Win32File_Close`
- `0x00576360` **Win32File_GetLastWriteTimeHighCf** — local: `Win32File_OpenCf`, `Win32File_Close`
- `0x005763C0` **Win32Drive_GetVolumeSerialNumberCf** — local: `Win32File_OpenCf`, `Win32File_Close`
- `0x00575F40` **Win32FileSystem_RestoreInitialDirectory** — local: `Win32File_SetCurrentDirectoryCf`
- `0x005766F0` **Win32Drive_CheckMediaReadyCf** — local: `Win32Drive_GetEngineTypeCode`
- `0x0040F1F0` **FileSystem_WriteBufferToPathCf**
- `0x00576070` **Win32File_WriteExactOrFlushCf**
- `0x00576140` **Win32File_GetPositionCf**
- `0x00576180` **Win32File_SeekCf**
- `0x005761C0` **Win32File_DeleteCf** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x00576210` **Win32File_MoveCf** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x00576280` **Win32File_CopyCf** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x005764E0` **Win32File_CreateDirectoryRecursiveCf** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`, `WidePath_SplitParentAndLeaf` → `core/text/path`
- `0x005765A0` **Win32File_RemoveDirectoryCf** — cross: `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x005765F0` **Win32Drive_GetFreeAndTotalBytesRegs**
- `0x005766B0` **Win32Drive_EnumerateLetters**
- `0x00576790` **Win32Path_ValidateDos83Cf**
- `0x00576910` **Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`, `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`, `Utf16String_CompareAsciiCaseInsensitiveFlags` → `core/text/string`
- `0x00576020` **Win32File_ReadExactCf**
- `0x00576100` **Win32File_GetSizeCf**
- `0x00576430` **Win32File_GetCurrentDirectoryCf** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00576490` **Win32File_SetCurrentDirectoryCf** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x00576650` **Win32Drive_GetEngineTypeCode**
- `0x00575F60` **Win32File_OpenCf** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x00576000` **Win32File_Close**
