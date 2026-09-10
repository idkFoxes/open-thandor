# Call graph: `graphics/resources/texture`

36 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0057E970` **GraphicsTextureSet_Create** — local: `GraphicsTextureSet_AllocateMetadata`, `GraphicsTexture_SelectPixelFormat`, `GraphicsTexture_CreateStagingTexture`, `GraphicsTexture_RegisterSlot`, `GraphicsTexture_ReleaseObjects`, `GraphicsTexture_CreateDeviceTexture`; cross: `Glide3_TextureSet_CreateBackend` → `graphics/backend/glide`
- `0x0057AD30` **GraphicsTexture_RebuildAllStagingTextures** — local: `GraphicsTexture_ReleaseObjects`, `GraphicsTexture_CreateStagingTexture`; cross: `Glide3_TextureResource_ReinitializeAll` → `graphics/backend/glide`
- `0x0057EAF0` **GraphicsTextureSet_Destroy** — local: `GraphicsTexture_ReleaseObjects`, `GraphicsTextureSet_FreeMetadata`; cross: `Glide3_TextureSet_DestroyBackend` → `graphics/backend/glide`
- `0x00485E40` **GraphicsTextureSet_LoadPackage** — cross: `Package_LoadEntry` → `assets/package/runtime`, `Resource_Release` → `assets/resource/runtime`
- `0x00485E80` **GraphicsTextureSet_ReleasePackage** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x00485FC0` **GraphicsTextureSet_RefreshNoOp**
- `0x00486070` **GraphicsTexture_RebuildNoOp**
- `0x004A9270` **GraphicsTextureSource_GetLogicalSizeRegs**
- `0x004A92C0` **GraphicsTextureSource_TestOpaquePixel**
- `0x004A9A50` **GraphicsTextureSource_BlitTiledSourceAlpha**
- `0x004AA0A0` **GraphicsTextureSource_BlitTiledHalfSourceRgb**
- `0x004AB9A0` **GraphicsTextureSource_BlitTiledSaturatedAddRgb**
- `0x004ABF70` **GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd**
- `0x004AD630` **GraphicsTextureSource_LoadPackageAsset** — cross: `Package_LoadEntry` → `assets/package/runtime`, `Resource_Release` → `assets/resource/runtime`
- `0x004AD670` **GraphicsTextureSource_CloneAsset**
- `0x004AD6C0` **GraphicsTextureSource_ConvertPaletteEntries**
- `0x004AD770` **GraphicsTextureSource_ReleasePackageAsset** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x004AD790` **GraphicsTextureSource_ReleaseClonedAsset**
- `0x004AD7B0` **GraphicsTextureSource_ResolveAllocationBase**
- `0x004AD7C0` **GraphicsTextureSource_GetFirstLogicalSizeRegs**
- `0x0057ADA0` **GraphicsTexture_UploadColor_1x** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057B410` **GraphicsTexture_UploadColor_2x** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057BBE0` **GraphicsTexture_UploadColor_4x** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057C6C0` **GraphicsTexture_UploadAlpha_1x** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057C890` **GraphicsTexture_UploadAlpha_2x** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057CAA0` **GraphicsTexture_UploadAlpha_4x** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057EBB0` **GraphicsTextureSet_RefreshColor** — cross: `Glide3_TextureSet_RefreshColor` → `graphics/backend/glide`
- `0x0057EC40` **GraphicsTextureSet_RefreshAlpha** — cross: `Glide3_TextureSet_RefreshAlpha` → `graphics/backend/glide`
- `0x0057AAD0` **GraphicsTexture_CreateDeviceTexture** — local: `GraphicsTexture_EvictOldestDeviceTexture`; cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x00485EA0` **GraphicsTextureSet_AllocateMetadata**
- `0x00485F90` **GraphicsTextureSet_FreeMetadata**
- `0x0057A9D0` **GraphicsTexture_EvictOldestDeviceTexture**
- `0x0057E870` **GraphicsTexture_RegisterSlot**
- `0x0057E8C0` **GraphicsTexture_SelectPixelFormat**
- `0x0057A740` **GraphicsTexture_CreateStagingTexture** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057A900` **GraphicsTexture_ReleaseObjects**
