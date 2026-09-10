#ifndef THANDOR_GRAPHICS_RESOURCES_TEXTURE_H
#define THANDOR_GRAPHICS_RESOURCES_TEXTURE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/texture. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0057E970 */
GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057AD30 */
void __cdecl GraphicsTexture_RebuildAllStagingTextures(void);

/* 0x0057EAF0 */
GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set);

/* 0x00485E40 */
GraphicsTextureSet * __fastcall GraphicsTextureSet_LoadPackage(dword packageContext0,dword packageContext1,word *pathUtf16);

/* 0x00485E80 */
void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set);

/* 0x00485FC0 */
void GraphicsTextureSet_RefreshNoOp (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x00486070 */
void __cdecl GraphicsTexture_RebuildNoOp(void);

/* 0x004A9270 */
qword GraphicsTextureSource_GetLogicalSizeRegs (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x004A92C0 */
void GraphicsTextureSource_TestOpaquePixel (GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x004A9A50 */
void GraphicsTextureSource_BlitTiledSourceAlpha (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX, GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AA0A0 */
void GraphicsTextureSource_BlitTiledHalfSourceRgb (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX, GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AB9A0 */
void GraphicsTextureSource_BlitTiledSaturatedAddRgb (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX, GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004ABF70 */
void GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX, GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AD630 */
GraphicsTextureSourceAsset * __fastcall GraphicsTextureSource_LoadPackageAsset(dword packageContext0,dword packageContext1,word *pathUtf16);

/* 0x004AD670 */
GraphicsTextureSourceAsset * GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD6C0 */
GraphicsPaletteTextureSourceAsset * GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset);

/* 0x004AD770 */
void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD790 */
void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD7B0 */
GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD7C0 */
GraphicsLogicalDimensionsEdxEax8 GraphicsTextureSource_GetFirstLogicalSizeRegs(GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057ADA0 */
void GraphicsTexture_UploadColor_1x(GraphicsTextureResource *texture);

/* 0x0057B410 */
void GraphicsTexture_UploadColor_2x(GraphicsTextureResource *texture);

/* 0x0057BBE0 */
void GraphicsTexture_UploadColor_4x(GraphicsTextureResource *texture);

/* 0x0057C6C0 */
void GraphicsTexture_UploadAlpha_1x(GraphicsTextureResource *texture);

/* 0x0057C890 */
void GraphicsTexture_UploadAlpha_2x(GraphicsTextureResource *texture);

/* 0x0057CAA0 */
void GraphicsTexture_UploadAlpha_4x(GraphicsTextureResource *texture);

/* 0x0057EBB0 */
void GraphicsTextureSet_RefreshColor (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x0057EC40 */
void GraphicsTextureSet_RefreshAlpha (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x0057AAD0 */
GraphicsTextureResource * GraphicsTexture_CreateDeviceTexture(GraphicsTextureResource *texture);

/* 0x00485EA0 */
GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset);

/* 0x00485F90 */
GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set);

/* 0x0057A9D0 */
void GraphicsTexture_EvictOldestDeviceTexture(GraphicsTextureResource *exclude);

/* 0x0057E870 */
void GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture);

/* 0x0057E8C0 */
DDPIXELFORMAT * GraphicsTexture_SelectPixelFormat (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057A740 */
GraphicsTextureResource * GraphicsTexture_CreateStagingTexture(GraphicsTextureResource *texture);

/* 0x0057A900 */
GraphicsTextureResource * GraphicsTexture_ReleaseObjects(GraphicsTextureResource *texture);

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_H */
