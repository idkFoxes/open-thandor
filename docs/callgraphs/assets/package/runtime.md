# Call graph: `assets/package/runtime`

15 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005460E0` **LevelPackage_ValidateAndMount** — local: `Package_Mount`, `Package_FindEntry`, `Package_LoadEntry`, `Package_Unmount`; cross: `Resource_Release` → `assets/resource/runtime`, `TextResourcePage_LoadCompatibilityAliases` → `assets/text/resources`
- `0x0040E840` **Package_UpsertEntry** — local: `Package_FindEntryInMount`, `Package_DeleteEntry`, `Package_ReadDirectory`
- `0x0040ED00` **Package_LoadEntryIntoBuffer** — local: `Package_FindEntryAcrossMounts`, `Package_DecodeEntryInto`, `Package_SetLastErrorPath`; cross: `WidePath_CombineDirectoryAndLeaf` → `core/text/path`
- `0x0040E450` **Package_MountLowPriority** — local: `Package_ReadDirectory`; cross: `WidePath_CombineDirectoryAndLeaf` → `core/text/path`
- `0x0040E6F0` **Package_DeleteEntry** — local: `Package_FindEntryInMount`, `Package_ReadDirectory`
- `0x0040EE30` **Package_LoadEntry** — local: `Package_FindEntryAcrossMounts`, `Package_DecodeEntryInto`; cross: `WidePath_CombineDirectoryAndLeaf` → `core/text/path`
- `0x0040E3A0` **Package_Mount** — local: `Package_ReadDirectory`; cross: `WidePath_CombineDirectoryAndLeaf` → `core/text/path`
- `0x0040EB70` **Package_FindEntry** — local: `Package_WildcardPathMatches`
- `0x0040E500` **Package_Unmount**
- `0x0040ECA0` **Package_WildcardPathMatches**
- `0x0040EAF0` **Package_DecodeEntryInto** — local: `Package_SetLastErrorPath`
- `0x0040E2B0` **Package_SetLastErrorPath**
- `0x0040E640` **Package_FindEntryInMount**
- `0x0040EA20` **Package_FindEntryAcrossMounts**
- `0x0040E570` **Package_ReadDirectory**
