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

/* Number of entries in g_GraphicsTextureSlots, the registry of live DirectDraw texture resources
   (GraphicsTexture_RegisterSlot, GraphicsTexture_RebuildAllStagingTextures, GraphicsTextureSet_Destroy). */
#define GRAPHICS_TEXTURE_SLOT_CAPACITY 4096

/* repeatEndY/repeatEndX value of the tiled blits (GraphicsTextureSource_BlitTiled*): repeat along that axis
   for exactly one tile extent from the tile origin. */
#define GRAPHICS_TILED_BLIT_ONE_TILE (-0x80000000)

/* Subresource table of a 'gfx' texture source: one 32-byte record per subresource at
   asset + subresourceTableOffset (+ index * GFX_SUBRESOURCE_RECORD_SIZE). The pixel data offset is relative to
   the asset start as well. */
/* Offset of buildMetadata.assetAnchor28 in an asset. Some functions address the asset through that field and
   compile differently from the plain (uint8_t *)asset + offset form, so they keep the anchor. */
#define GFX_ASSET_ANCHOR28_OFFSET 0x28
/* Address of the byte at an asset-relative offset, computed through the anchor field (asset + 0x28). */
#define GFX_ANCHORED_ASSET_BYTES(asset,offset) \
  ((asset)->common.buildMetadata.assetAnchor28 + (offset) - GFX_ASSET_ANCHOR28_OFFSET)
/* argb8888 of entry `index` of the palette bank that starts at `bank` (8-byte GraphicsTexturePaletteEntry),
   computed through the anchor field. */
#define GFX_ANCHORED_PALETTE_ARGB(bank,index) \
  (*(uint32_t *)GFX_ANCHORED_ASSET_BYTES(bank,(uint32_t)(index) * 8))
/* The 0x200-byte asset header (sizeof(GraphicsTextureSourceAsset)) is followed by paletteBankCount palette banks
   of 256 eight-byte entries (argb8888, framebuffer pixel); the subresource table and the pixels lie at the
   offsets the header and the records name. */
#define GFX_ASSET_HEADER_SIZE 0x200
#define GFX_PALETTE_BANK_SIZE 0x800
#define GFX_SUBRESOURCE_RECORD_SIZE 0x20
#define GRAPHICS_TEXTURE_SET_ENTRY_BYTES 0x20 /* GraphicsTextureSetEntry, after the 8-byte texture set header */
#define GFX_SUBRESOURCE_LOGICAL_WIDTH 0x00  /* tile extent used by the tiled blits */
#define GFX_SUBRESOURCE_LOGICAL_HEIGHT 0x04
#define GFX_SUBRESOURCE_PALETTE_INDEX 0x08  /* -1: direct ARGB8888 pixels; else palette bank, 8-bit indices */
#define GFX_SUBRESOURCE_PIXEL_OFFSET 0x0C   /* asset-relative offset of the pixels */
#define GFX_SUBRESOURCE_ORIGIN_X 0x10       /* position of the stored pixels inside the logical extent */
#define GFX_SUBRESOURCE_ORIGIN_Y 0x14
#define GFX_SUBRESOURCE_PIXEL_WIDTH 0x18    /* stored pixels per row */
#define GFX_SUBRESOURCE_PIXEL_HEIGHT 0x1C   /* stored rows */
/* Pixels of an asset with a single subresource record directly after the header (offscreen renders) */
#define GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET (GFX_ASSET_HEADER_SIZE + GFX_SUBRESOURCE_RECORD_SIZE)

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0057E970 */
GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode);

/* 0x0057AD30 */
void GraphicsTexture_RebuildAllStagingTextures(void);

/* 0x0057EAF0 */
GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set);

/* 0x00485E40 */
GraphicsTextureSet * GraphicsTextureSet_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode);

/* 0x00485E80 */
void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set);

/* 0x00485FC0 */
void GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x00486070 */
void __cdecl GraphicsTexture_RebuildNoOp(void);

/* 0x004A9270 */
TextureSizeResult GraphicsTextureSource_GetLogicalSizeRegs
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x004A92C0 */
bool GraphicsTextureSource_TestOpaquePixel(GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x004A9A50 */
void GraphicsTextureSource_BlitTiledSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004AA0A0 */
void GraphicsTextureSource_BlitTiledHalfSourceRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004AB9A0 */
void GraphicsTextureSource_BlitTiledSaturatedAddRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004ABF70 */
void GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x004AD630 */
GraphicsTextureSourceAsset *GraphicsTextureSource_LoadPackageAsset(uint16_t *pathUtf16,uint32_t *outError);

/* 0x004AD670 */
GraphicsTextureSourceAsset * GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD6C0 */
uint32_t GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset);

/* 0x004AD770 */
void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD790 */
void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD7B0 */
GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset);

/* 0x004AD7C0 */
bool GraphicsTextureSource_GetFirstLogicalSize
          (GraphicsTextureSourceAsset *sourceAsset,uint32_t *outWidthPixels,uint32_t *outHeightPixels);

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
void GraphicsTextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x0057EC40 */
void GraphicsTextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x0057AAD0 */
void GraphicsTexture_CreateDeviceTexture(GraphicsTextureResource *texture);

/* 0x00485EA0 */
GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode);

/* 0x00485F90 */
GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set);

/* 0x0057A9D0 */
bool GraphicsTexture_EvictOldestDeviceTexture(GraphicsTextureResource *exclude);

/* 0x0057E870 */
bool GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture);

/* 0x0057E8C0 */
DDPIXELFORMAT * GraphicsTexture_SelectPixelFormat (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

/* 0x0057A740 */
GraphicsTextureResource * GraphicsTexture_CreateStagingTexture(GraphicsTextureResource *texture);

/* 0x0057A900 */
void GraphicsTexture_ReleaseObjects(GraphicsTextureResource *texture);


/* 0x004AC8E0 */
bool GraphicsTextureSource_DecomposeSubresourceRegions
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset,
          GraphicsTextureSourceAsset **outAsset,uint32_t *outError);

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_H */
