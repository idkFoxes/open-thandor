# Call graph: `graphics/backend/glide`

34 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005801B0` **Glide3_TextureSet_CreateBackend** — local: `Glide3_TextureResource_Initialize`, `Glide3_TextureResource_Release`; cross: `GraphicsTexture_SelectPixelFormat` → `graphics/resources/texture`, `GraphicsTexture_RegisterSlot` → `graphics/resources/texture`
- `0x00580430` **Glide3_TextureResource_ReinitializeAll** — local: `Glide3_TextureResource_Release`, `Glide3_TextureResource_Initialize`
- `0x0057F0C0` **GlideBackend_ShutdownWrapper** — local: `Glide3_Shutdown`
- `0x0057F0F0` **GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf** — local: `Glide3_TextureResource_Initialize`; cross: `DynDLL_Load` → `platform/bootstrap/runtime`, `DynAPI_Resolve` → `platform/bootstrap/runtime`, `DynDLL_Unload` → `platform/bootstrap/runtime`
- `0x0057F7B0` **Glide3_DrawPrimitiveQueue** — local: `Glide3_TextureResource_EnsureResident`; cross: `GraphicsPrimitiveQueue_Begin` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_Next` → `graphics/render/primitives`
- `0x005802F0` **Glide3_TextureSet_DestroyBackend** — local: `Glide3_TextureResource_Release`; cross: `GraphicsTextureSet_FreeMetadata` → `graphics/resources/texture`
- `0x00580470` **Glide3_Framebuffer_Present** — local: `Glide3_Cursor_ComposeBeforePresent`
- `0x0057EE90` **Glide3_InitAndEnumerate** — cross: `DynDLL_Load` → `platform/bootstrap/runtime`, `DynAPI_Resolve` → `platform/bootstrap/runtime`, `DynDLL_Unload` → `platform/bootstrap/runtime`, `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x0057F0D0` **GlideBackend_BeginSceneNoOp**
- `0x0057F0E0` **GlideBackend_EndSceneNoOp**
- `0x0057F740` **Glide3_ClearViewport**
- `0x00580370` **Glide3_TextureSet_RefreshColor**
- `0x005803D0` **Glide3_TextureSet_RefreshAlpha**
- `0x00580540` **Glide3_Framebuffer_CaptureRegion**
- `0x005806E0` **Glide3_Cursor_RestoreAfterPresentNoOp**
- `0x005807B0` **Glide3_TextureUpload_1x**
- `0x00580960` **Glide3_TextureUpload_2x**
- `0x00580C60` **Glide3_TextureUpload_4x**
- `0x00581150` **Glide3_TextureSource_BlitSourceAlpha** — cross: `SoftwareTextureSource_BlitSourceAlpha16` → `graphics/backend/software`
- `0x005814D0` **Glide3_TextureSource_BlitHalfSourceRgb** — cross: `SoftwareTextureSource_BlitHalfSourceRgb16` → `graphics/backend/software`
- `0x005817F0` **Glide3_TextureSource_StretchDirectColorBilinear** — cross: `SoftwareTextureSource_StretchDirectColorBilinear16` → `graphics/backend/software`
- `0x00581A90` **Glide3_TextureSource_BlitIntegerScaledSourceAlpha** — cross: `SoftwareTextureSource_BlitHalfSourceRgb16` → `graphics/backend/software`
- `0x00581EF0` **Glide3_TextureSource_BlitSourceAlphaPaletteBank** — cross: `SoftwareTextureSource_BlitSourceAlphaPaletteBank16` → `graphics/backend/software`
- `0x00582290` **Glide3_TextureSource_BlitSaturatedAddRgb** — cross: `SoftwareTextureSource_BlitSaturatedAddRgb16` → `graphics/backend/software`
- `0x00582580` **Glide3_TextureSource_BlitHalfRgbSaturatedAdd** — cross: `SoftwareTextureSource_BlitHalfRgbSaturatedAdd16` → `graphics/backend/software`
- `0x00582870` **Glide3_TextureSource_BlitModulatedSourceAlpha** — cross: `SoftwareTextureSource_BlitHalfRgbSaturatedAdd16` → `graphics/backend/software`
- `0x00582D30` **Glide3_Framebuffer_FillRectArgb** — cross: `SoftwareFramebuffer_FillRectArgb16` → `graphics/backend/software`
- `0x005806F0` **Glide3_Cursor_ComposeBeforePresent** — local: `Glide3_Framebuffer_BeginAccess`, `Glide3_Framebuffer_EndAccess`
- `0x0057F5C0` **Glide3_Shutdown** — cross: `GraphicsTexture_ReleaseObjects` → `graphics/resources/texture`, `DynDLL_Unload` → `platform/bootstrap/runtime`
- `0x0057F630` **Glide3_Framebuffer_BeginAccess**
- `0x0057F6E0` **Glide3_Framebuffer_EndAccess**
- `0x0057FF50` **Glide3_TextureResource_EnsureResident**
- `0x0057FD40` **Glide3_TextureResource_Initialize**
- `0x0057FE80` **Glide3_TextureResource_Release**
