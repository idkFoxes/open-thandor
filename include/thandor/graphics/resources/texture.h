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

/* Number of entries in g_GraphicsTextureSlots, the registry of live texture resources
   (GraphicsTexture_RegisterSlot, GraphicsTextureSet_Destroy). */
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

/* Functions are grouped by semantic ownership. */

GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode);

GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set);

GraphicsTextureSet * GraphicsTextureSet_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode);

void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set);

void GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

void __cdecl GraphicsTexture_RebuildNoOp(void);

GraphicsTextureLogicalSize GraphicsTextureSource_GetLogicalSize
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

bool GraphicsTextureSource_TestOpaquePixel(GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

void GraphicsTextureSource_BlitTiledSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

void GraphicsTextureSource_BlitTiledHalfSourceRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

void GraphicsTextureSource_BlitTiledSaturatedAddRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

void GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

GraphicsTextureSourceAsset *GraphicsTextureSource_LoadPackageAsset(uint16_t *pathUtf16,uint32_t *outError);

GraphicsTextureSourceAsset * GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset);

uint32_t GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset);

void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset);

void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset);

GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset);

GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode);

GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set);

bool GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture);

bool GraphicsTextureSource_DecomposeSubresourceRegions
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset,
          GraphicsTextureSourceAsset **outAsset,uint32_t *outError);

extern GraphicsTextureSourceGetLogicalSizeProc *g_GraphicsTextureSourceGetLogicalSize;
extern GraphicsTextureSourceTestOpaquePixelProc *g_GraphicsTextureSourceTestOpaquePixel;
extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitSourceAlpha;
extern GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledSourceAlpha;
extern GraphicsTextureSourceBlitModulatedSourceAlphaProc *g_GraphicsTextureSourceBlitModulatedSourceAlpha;

extern GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures;

extern GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet;
extern GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet;
extern GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledHalfSourceRgb;
extern GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries;
extern GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase;

extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitHalfSourceRgb;
extern GraphicsTextureSourceStretchDirectColorBilinearProc *g_GraphicsTextureSourceStretchDirectColorBilinear;
extern GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitSaturatedAddRgb;
extern GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd;
extern GraphicsTextureResource **g_GraphicsTextureSlots;

extern GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureAlpha;

extern GraphicsTextureSetLoadPackageProc *g_GraphicsTextureSetLoadPackage;
extern GraphicsTextureSetReleasePackageProc *g_GraphicsTextureSetReleasePackage;

extern GraphicsTextureSourceLoadPackageAssetProc *g_GraphicsTextureSourceLoadPackageAsset;

extern GraphicsTextureSourceLifecycleCallbackTable g_GraphicsTextureSourceLifecycleCallbacks3;

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_H */
