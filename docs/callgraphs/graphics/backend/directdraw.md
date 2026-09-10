# Call graph: `graphics/backend/directdraw`

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00423CF0` **GraphicsDisplayMode_IsEnumeratedCf**
- `0x0054B0E0` **DisplayModeTable_ContainsExactModeCf**
- `0x00578080` **DirectDraw_EnumAdapterCallback** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x005780F0` **DirectDraw_EnumDisplayModeCallback**
- `0x00578920` **GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf** — cross: `GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf` → `graphics/backend/glide`, `Glide3_Shutdown` → `graphics/backend/glide`, `GraphicsTexture_ReleaseObjects` → `graphics/resources/texture`, `Memory_ZeroDwords` → `core/memory/allocator`, `GraphicsTexture_CreateStagingTexture` → `graphics/resources/texture`
