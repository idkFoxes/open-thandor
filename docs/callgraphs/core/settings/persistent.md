# Call graph: `core/settings/persistent`

6 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00402C00` **PersistentSettings_Flush** — local: `PersistentSettings_WriteDword`; cross: `FileSystem_WriteBufferToPathCf` → `platform/filesystem/win32`
- `0x00402B00` **PersistentSettings_Load** — cross: `Resource_Release` → `assets/resource/runtime`, `WidePath_CombineDirectoryAndLeaf` → `core/text/path`, `RichTextCommandStream_CopyExpandedCf` → `assets/text/richtext`
- `0x00402C50` **PersistentSettings_ReadDword**
- `0x00402CC0` **PersistentSettings_GetRegionOrFallback**
- `0x00402CF0` **PersistentSettings_WriteDwords**
- `0x00402C80` **PersistentSettings_WriteDword**
