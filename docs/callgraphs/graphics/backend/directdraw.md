# Call graph: `graphics/backend/directdraw`

[Source](../../../../src/graphics/backend/directdraw.c) · [Header](../../../../include/thandor/graphics/backend/directdraw.h) · [Graphviz](directdraw.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-graphics-backend-directdraw)

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00423CF0` **[`GraphicsDisplayMode_IsEnumeratedCf`](../../../../src/graphics/backend/directdraw.c#L5)**
- `0x0054B0E0` **[`DisplayModeTable_ContainsExactModeCf`](../../../../src/graphics/backend/directdraw.c#L34)**
- `0x00578080` **[`DirectDraw_EnumAdapterCallback`](../../../../src/graphics/backend/directdraw.c#L64)** — cross: [`Text_CopyNarrowToUtf16Cf`](../../../../src/core/text/string.c#L185) → [`core/text/string`](../../core/text/string.md)
- `0x005780F0` **[`DirectDraw_EnumDisplayModeCallback`](../../../../src/graphics/backend/directdraw.c#L101)**
- `0x00578920` **[`GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf`](../../../../src/graphics/backend/directdraw.c#L180)** — cross: [`GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf`](../../../../src/graphics/backend/glide.c#L115) → [`graphics/backend/glide`](glide.md), [`Glide3_Shutdown`](../../../../src/graphics/backend/glide.c#L4199) → [`graphics/backend/glide`](glide.md), [`GraphicsTexture_ReleaseObjects`](../../../../src/graphics/resources/texture.c#L3792) → [`graphics/resources/texture`](../resources/texture.md), [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md), [`GraphicsTexture_CreateStagingTexture`](../../../../src/graphics/resources/texture.c#L3700) → [`graphics/resources/texture`](../resources/texture.md)

## Called by

- [`ui/controls/misc`](../../ui/controls/misc.md): [`UiDisplayModeSelection_RefreshEnumeratedOptions`](../../../../src/ui/controls/misc.c#L1114) → [`GraphicsDisplayMode_IsEnumeratedCf`](../../../../src/graphics/backend/directdraw.c#L5)
- [`ui/frontend/settings`](../../ui/frontend/settings.md): [`FrontendDisplaySettingsPage_UpdateModeActionAvailability`](../../../../src/ui/frontend/settings.c#L1669) → [`DisplayModeTable_ContainsExactModeCf`](../../../../src/graphics/backend/directdraw.c#L34)
