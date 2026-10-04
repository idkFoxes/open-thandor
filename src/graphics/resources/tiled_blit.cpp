/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/tiled_blit.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Tiled texture source blits: a texture source repeated over a rectangle with one of the blit slots
   (source alpha, half source RGB, saturated add, half RGB saturated add). */

#include <thandor/graphics/resources/tiled_blit.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledHalfSourceRgb = &GraphicsTextureSource_BlitTiledHalfSourceRgb;

GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledSourceAlpha = &GraphicsTextureSource_BlitTiledSourceAlpha;

/* The tile stepping below never ends for a zero (or, as int, negative) logical size, which
   g_GraphicsTextureSourceGetLogicalSize returns for an invalid asset or index; the original looped forever there.
   Such a call can draw nothing (every call that ends draws no tile), so the blits return early instead. Logs
   once. */
static Bool8 TiledBlit_TileSizeUsable(uint32_t tileWidth,uint32_t tileHeight)

{
  static Bool8 loggedEmptyTile;

  if ((int)tileWidth > 0 && (int)tileHeight > 0) {
    return true;
  }
  if (!loggedEmptyTile) {
    loggedEmptyTile = true;
    Thandor_Log("GraphicsTextureSource_BlitTiled: skipped a tile of %ux%u (invalid asset or subresource)",
                tileWidth,tileHeight);
  }
  return false;
}

/* Fills a rectangle with copies of one subresource laid out on its logical-size grid anchored at the tile
   origin, drawing each copy with g_GraphicsTextureSourceBlitSourceAlpha (installed as
   g_GraphicsTextureSourceBlitTiledSourceAlpha; also called directly by the text controls). The area ends at
   repeatEnd (GRAPHICS_TILED_BLIT_ONE_TILE: one tile past the origin), clipped to clipMax and the framebuffer.
*/
void GraphicsTextureSource_BlitTiledSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  uint32_t tileWidth;
  int tileX;
  uint32_t tileHeight;
  int tileY;
  int64_t steppedOrigin;
  GraphicsTextureLogicalSize logicalSize;
  
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(subresourceIndex,sourceAsset);
  tileHeight = logicalSize.logicalHeightPixels;
  tileWidth = logicalSize.logicalWidthPixels;
  if (!TiledBlit_TileSizeUsable(tileWidth,tileHeight)) {
    return;
  }
  if (repeatEndX == GRAPHICS_TILED_BLIT_ONE_TILE) {
    repeatEndX = tileOriginX + tileWidth;
  }
  if (repeatEndY == GRAPHICS_TILED_BLIT_ONE_TILE) {
    repeatEndY = tileOriginY + tileHeight;
  }
  /* Step the origin by whole tiles until it is positive (tested on the exact, non-wrapping sum) and past the
     clip minimum; the tile before it is the first one drawn */
  do {
    do {
      steppedOrigin = (int64_t)tileOriginX + (int)tileWidth;
      tileOriginX = tileOriginX + tileWidth;
    } while (steppedOrigin <= 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      steppedOrigin = (int64_t)tileOriginY + (int)tileHeight;
      tileOriginY = tileOriginY + tileHeight;
    } while (steppedOrigin <= 0);
  } while (tileOriginY <= clipMinY);
  tileY = tileOriginY - tileHeight;
  if (clipMaxX < repeatEndX) {
    repeatEndX = clipMaxX;
  }
  if (clipMaxY < repeatEndY) {
    repeatEndY = clipMaxY;
  }
  if ((int)framebuffer->width < repeatEndX) {
    repeatEndX = framebuffer->width;
  }
  if ((int)framebuffer->height < repeatEndY) {
    repeatEndY = framebuffer->height;
  }
  if ((int)(tileOriginX - tileWidth) < repeatEndX) {
    for (; tileY < repeatEndY; tileY = tileY + tileHeight) {
      tileX = tileOriginX - tileWidth;
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}

/* GraphicsTextureSource_BlitTiledSourceAlpha with g_GraphicsTextureSourceBlitHalfSourceRgb as the per-tile
   blit (installed as g_GraphicsTextureSourceBlitTiledHalfSourceRgb).
*/
void GraphicsTextureSource_BlitTiledHalfSourceRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  uint32_t tileWidth;
  int tileX;
  uint32_t tileHeight;
  int tileY;
  int64_t steppedOrigin;
  GraphicsTextureLogicalSize logicalSize;
  
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(subresourceIndex,sourceAsset);
  tileHeight = logicalSize.logicalHeightPixels;
  tileWidth = logicalSize.logicalWidthPixels;
  if (!TiledBlit_TileSizeUsable(tileWidth,tileHeight)) {
    return;
  }
  if (repeatEndX == GRAPHICS_TILED_BLIT_ONE_TILE) {
    repeatEndX = tileOriginX + tileWidth;
  }
  if (repeatEndY == GRAPHICS_TILED_BLIT_ONE_TILE) {
    repeatEndY = tileOriginY + tileHeight;
  }
  /* Step the origin by whole tiles until it is positive (tested on the exact, non-wrapping sum) and past the
     clip minimum; the tile before it is the first one drawn */
  do {
    do {
      steppedOrigin = (int64_t)tileOriginX + (int)tileWidth;
      tileOriginX = tileOriginX + tileWidth;
    } while (steppedOrigin <= 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      steppedOrigin = (int64_t)tileOriginY + (int)tileHeight;
      tileOriginY = tileOriginY + tileHeight;
    } while (steppedOrigin <= 0);
  } while (tileOriginY <= clipMinY);
  tileY = tileOriginY - tileHeight;
  if (clipMaxX < repeatEndX) {
    repeatEndX = clipMaxX;
  }
  if (clipMaxY < repeatEndY) {
    repeatEndY = clipMaxY;
  }
  if ((int)framebuffer->width < repeatEndX) {
    repeatEndX = framebuffer->width;
  }
  if ((int)framebuffer->height < repeatEndY) {
    repeatEndY = framebuffer->height;
  }
  if ((int)(tileOriginX - tileWidth) < repeatEndX) {
    for (; tileY < repeatEndY; tileY = tileY + tileHeight) {
      tileX = tileOriginX - tileWidth;
      do {
        g_GraphicsTextureSourceBlitHalfSourceRgb
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}
