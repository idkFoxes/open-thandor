/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/texture.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_TEXTURE_H
#define THANDOR_GRAPHICS_RESOURCES_TEXTURE_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/texture. */

/* Subresource table of a 'gfx' texture source: one 32-byte record per subresource at
   asset + subresourceTableOffset (+ index * GFX_SUBRESOURCE_RECORD_SIZE). The pixel data offset is relative to
   the asset start as well. */
/* The 0x200-byte asset header (sizeof(GraphicsTextureSourceAsset)) is followed by paletteBankCount palette banks
   of 256 eight-byte entries (argb8888, framebuffer pixel); the subresource table and the pixels lie at the
   offsets the header and the records name. */
#define GFX_ASSET_HEADER_SIZE 0x200
#define GFX_PALETTE_BANK_SIZE 0x800
#define GFX_SUBRESOURCE_RECORD_SIZE 0x20
#define GRAPHICS_TEXTURE_SET_ENTRY_BYTES sizeof(GraphicsTextureSetEntry) /* 0x20 on x86, after the texture set header */
#define GRAPHICS_TEXTURE_SET_HEADER_BYTES offsetof(GraphicsTextureSet, entries) /* 8 on x86 */
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

GraphicsTextureLogicalSize GraphicsTextureSource_GetLogicalSize
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

Bool8 GraphicsTextureSource_TestOpaquePixel(GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset);

Bool8 GraphicsTextureSource_ValidateAsset(const GraphicsTextureSourceAsset *sourceAsset);

GraphicsTextureSourceAsset *GraphicsTextureSource_LoadPackageAsset(uint16_t *pathUtf16,uint32_t *outError);

GraphicsTextureSourceAsset * GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset);

uint32_t GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset);

void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset);

void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset);

GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset);

extern GraphicsTextureSourceGetLogicalSizeProc *g_GraphicsTextureSourceGetLogicalSize;
extern GraphicsTextureSourceTestOpaquePixelProc *g_GraphicsTextureSourceTestOpaquePixel;
extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitSourceAlpha;
extern GraphicsTextureSourceBlitModulatedSourceAlphaProc *g_GraphicsTextureSourceBlitModulatedSourceAlpha;

extern GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries;
extern GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase;

extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitHalfSourceRgb;
extern GraphicsTextureSourceStretchDirectColorBilinearProc *g_GraphicsTextureSourceStretchDirectColorBilinear;

extern GraphicsTextureSourceLoadPackageAssetProc *g_GraphicsTextureSourceLoadPackageAsset;

extern GraphicsTextureSourceLifecycleCallbackTable g_GraphicsTextureSourceLifecycleCallbacks3;

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_H */
