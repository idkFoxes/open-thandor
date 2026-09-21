/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/texture.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_TEXTURE_H
#define THANDOR_GRAPHICS_RESOURCES_TEXTURE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/texture. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0057E970 */
GraphicsTextureSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057AD30 */
void __thandor_void_preserve_eax_ecx_edx GraphicsTexture_RebuildAllStagingTextures(void);

/* 0x0057EAF0 */
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
GraphicsTextureSet_Destroy(GraphicsTextureSet *set);

/* 0x00485E40 */
GraphicsTextureSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSet_LoadPackage(word *pathUtf16);

/* 0x00485E80 */
void __thandor_void_preserve_eax_ecx_edx GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set);

/* 0x00485FC0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x00486070 */
void __cdecl GraphicsTexture_RebuildNoOp(void);

/* 0x004A9270 */
GraphicsTextureSizeEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
GraphicsTextureSource_GetLogicalSizeRegs
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x004A92C0 */
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsTextureSource_TestOpaquePixel
          (GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x004A9A50 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004AA0A0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledHalfSourceRgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004AB9A0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledSaturatedAddRgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004ABF70 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004AD630 */
GraphicsTextureSourceLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSource_LoadPackageAsset(word *pathUtf16);

/* 0x004AD670 */
GraphicsTextureSourceAsset * GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD6C0 */
GraphicsPaletteTextureSourceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset);

/* 0x004AD770 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD790 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD7B0 */
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD7C0 */
GraphicsTextureSizeEaxEdxCf9
GraphicsTextureSource_GetFirstLogicalSizeRegs(GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057ADA0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadColor_1x(GraphicsTextureResource *texture);

/* 0x0057B410 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadColor_2x(GraphicsTextureResource *texture);

/* 0x0057BBE0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadColor_4x(GraphicsTextureResource *texture);

/* 0x0057C6C0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadAlpha_1x(GraphicsTextureResource *texture);

/* 0x0057C890 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadAlpha_2x(GraphicsTextureResource *texture);

/* 0x0057CAA0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadAlpha_4x(GraphicsTextureResource *texture);

/* 0x0057EBB0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x0057EC40 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x0057AAD0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_CreateDeviceTexture(GraphicsTextureResource *texture);

/* 0x00485EA0 */
GraphicsTextureSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset);

/* 0x00485F90 */
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set);

/* 0x0057A9D0 */
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsTexture_EvictOldestDeviceTexture(GraphicsTextureResource *exclude);

/* 0x0057E870 */
bool __thandor_void_preserve_eax_ecx GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture);

/* 0x0057E8C0 */
DDPIXELFORMAT * GraphicsTexture_SelectPixelFormat (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057A740 */
GraphicsTextureResource * __thandor_eax_preserve_ecx_edx
GraphicsTexture_CreateStagingTexture(GraphicsTextureResource *texture);

/* 0x0057A900 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_ReleaseObjects(GraphicsTextureResource *texture);


/* 0x004AC8E0 */
GraphicsTextureSourceAssetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSource_DecomposeSubresourceRegionsCf
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset);

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_H */
