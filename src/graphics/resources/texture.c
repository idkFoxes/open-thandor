/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/texture.h>
#include <thandor/thandor.h>

/* Module data. */

GraphicsTextureSourceGetLogicalSizeProc *g_GraphicsTextureSourceGetLogicalSize = (void *)GraphicsTextureSource_GetLogicalSize;

GraphicsTextureSourceTestOpaquePixelProc *g_GraphicsTextureSourceTestOpaquePixel = (void *)GraphicsTextureSource_TestOpaquePixel;

GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitSourceAlpha = 0;

GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledSourceAlpha = (void *)GraphicsTextureSource_BlitTiledSourceAlpha;

GraphicsTextureSourceBlitModulatedSourceAlphaProc *g_GraphicsTextureSourceBlitModulatedSourceAlpha = 0;

/* Implementation ownership: graphics/resources/texture. */

/* Creates the renderer textures of a texture asset: allocates the set metadata, then one texture resource per
   subresource, registered in g_GraphicsTextureSlots. Nothing reads the resources any more (they held the
   surfaces of the original's hardware renderers); they are still allocated and registered so the arena layout,
   and with it the texture-set addresses GraphicsPrimitiveQueue_RadixSortForRendering sorts opaque packets by,
   stays as before.
   A subresource whose allocation or registration fails is left NULL; only a failed metadata allocation fails the
   call: then NULL is returned with its error in *outErrorCode (outErrorCode may be NULL). Never NULL on success.
*/
GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode)

{
  GraphicsTextureResource *newTexture;
  bool registerFailed;
  GraphicsTextureSet *allocatedSet;
  uint32_t textureAllocationError;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount entriesRemaining;

  allocatedSet = GraphicsTextureSet_AllocateMetadata(sourceAsset,outErrorCode);
  if (allocatedSet == NULL) {
    return NULL;
  }
  entriesRemaining = (allocatedSet->sourceAsset->tableDescriptor).subresourceCount;
  entryCursor = allocatedSet->entries;
  do {
    textureAllocationError = g_MemoryApi.alloc(sizeof(GraphicsTextureResource),(void **)&newTexture);
    if (textureAllocationError == 0) {
      entryCursor->texture = newTexture;
      registerFailed = GraphicsTexture_RegisterSlot(newTexture);
      if (registerFailed) {
        g_MemoryApi.free(newTexture);
        entryCursor->texture = NULL;
      }
    }
    entryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return allocatedSet;
}


/* Destroys a texture set made by GraphicsTextureSet_Create: every texture resource is removed from
   g_GraphicsTextureSlots and freed, then the set metadata is freed. Returns the source asset the set was
   built from, so the caller can release it too.
*/
GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource **slotCursor;
  GraphicsTextureSourceAsset *releasedSourceAsset;
  int slotsRemaining;
  uint32_t entriesRemaining;
  GraphicsTextureSetEntry *entryCursor;
  GraphicsTextureResource **matchedSlot;

  releasedSourceAsset = NULL;
  if (set != NULL) {
    entriesRemaining = set->subresourceCount;
    entryCursor = set->entries;
    do {
      texture = entryCursor->texture;
      if (texture != NULL) {
        /* find the texture's slot; if it is not registered the scan ends on (and clears) the last slot */
        slotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
        slotCursor = g_GraphicsTextureSlots;
        do {
          matchedSlot = slotCursor;
          if (texture == *matchedSlot) break;
          slotsRemaining--;
          slotCursor = matchedSlot + 1;
        } while (slotsRemaining != 0);
        *matchedSlot = NULL;
        g_MemoryApi.free(texture);
      }
      entryCursor++;
      entriesRemaining--;
    } while (entriesRemaining != 0);
    releasedSourceAsset = GraphicsTextureSet_FreeMetadata(set);
  }
  return releasedSourceAsset;
}


/* Loads a 'gfx' texture source from the package and builds a renderer texture set from it through
   g_GraphicsCreateTextureSet (installed as g_GraphicsTextureSetLoadPackage). When the set cannot be created the
   loaded asset is released again. Returns the set (never NULL), or NULL with the load or creation error in
   *outErrorCode (outErrorCode may be NULL).
*/
GraphicsTextureSet * GraphicsTextureSet_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode)

{
  GraphicsTextureSourceAsset *loadedSource;
  GraphicsTextureSet *createdSet;
  uint32_t errorCode;

  loadedSource = Package_LoadEntry(pathUtf16,&errorCode);
  if (loadedSource != NULL) {
    createdSet = g_GraphicsCreateTextureSet(loadedSource,&errorCode);
    if (createdSet != NULL) {
      return createdSet;
    }
    Resource_Release(loadedSource);
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return NULL;
}


/* Counterpart of GraphicsTextureSet_LoadPackage (installed as g_GraphicsTextureSetReleasePackage): destroys the
   texture set through g_GraphicsDestroyTextureSet and releases the 'gfx' source asset that call hands back.
*/
void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *sourceAsset;

  sourceAsset = g_GraphicsDestroyTextureSet(set);
  Resource_Release(sourceAsset);
  return;
}


/* g_GraphicsRefreshTextureColor and g_GraphicsRefreshTextureAlpha: the software renderer reads the source pixels
   directly, so there is nothing to re-upload after they changed (the original's hardware renderers installed
   their own re-uploads here).
*/
void GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  return;
}


/* g_GraphicsRebuildAllStagingTextures: the software renderer keeps no device textures, so there is nothing to
   rebuild after a display mode or texture detail change (the original's hardware renderers installed their own
   rebuild here).
*/
void __cdecl GraphicsTexture_RebuildNoOp(void)

{
  return;
}

/* Returns the logical width and height of one subresource of a 'gfx' texture source, i.e. the extent the
   tiled blits repeat (installed as g_GraphicsTextureSourceGetLogicalSize), or 0 x 0 when the asset is not a
   'gfx' asset or the index is out of range (the original only signalled failure and returned no size; the
   only callers that test it, the glyph size queries, use width 0 then).
*/
GraphicsTextureLogicalSize GraphicsTextureSource_GetLogicalSize
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureLogicalSize size;
  GraphicsTextureSourceEntry *entry;

  size.logicalWidthPixels = 0;
  size.logicalHeightPixels = 0;
  if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
     (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
    entry = (GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                           (sourceAsset->tableDescriptor).subresourceTableOffset);
    size.logicalWidthPixels = entry->logicalWidth;
    size.logicalHeightPixels = entry->logicalHeight;
  }
  return size;
}


/* Hit test of a sprite drawn at (drawX, drawY) (installed as g_GraphicsTextureSourceTestOpaquePixel): maps the
   query point into the stored pixels of the subresource and returns true when that pixel has a non-zero
   alpha, for direct ARGB and paletted subresources alike. Returns false for transparent pixels, points
   outside the stored pixels and invalid input.
*/
bool GraphicsTextureSource_TestOpaquePixel(GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  const GraphicsTextureSourceEntry *entry;
  const uint8_t *pixels;
  int recordOffset;
  int paletteIndex;
  int localX;
  int localY;
  int pixelIndex;

  if ((queryX < drawX) || (queryY < drawY)) {
    return false;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE;
  entry = (const GraphicsTextureSourceEntry *)
          ((uint8_t *)sourceAsset + recordOffset + (sourceAsset->tableDescriptor).subresourceTableOffset);
  /* query point relative to the stored pixels, which start at (originX, originY) of the sprite */
  localX = (queryX - drawX) - entry->originX;
  if (entry->originX > queryX - drawX) {
    return false;
  }
  localY = (queryY - drawY) - entry->originY;
  if (entry->originY > queryY - drawY) {
    return false;
  }
  if ((localX >= (int)entry->pixelWidth) || (localY >= (int)entry->pixelHeight)) {
    return false;
  }
  paletteIndex = entry->paletteIndex;
  pixelIndex = localY * entry->pixelWidth + localX;
  pixels = (const uint8_t *)sourceAsset + entry->dataOffset;
  /* opaque = any alpha bit set in the ARGB8888 pixel (direct) or palette entry (paletted, 8 bytes each in
     the 256-entry bank at asset + 0x200 + paletteIndex * 0x800) */
  if (paletteIndex == -1) {
    return ARGB8888_RGB_MASK < ((const uint32_t *)pixels)[pixelIndex];
  }
  return ARGB8888_RGB_MASK <
         ((GraphicsPaletteTextureSourceAsset *)sourceAsset)->paletteEntries
                    [paletteIndex * GRAPHICS_PALETTE_BANK_ENTRIES + (uint32_t)pixels[pixelIndex]].argb8888;
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


/* GraphicsTextureSource_BlitTiledSourceAlpha with g_GraphicsTextureSourceBlitSaturatedAddRgb as the per-tile
   blit (installed as g_GraphicsTextureSourceBlitTiledSaturatedAddRgb).
*/
void GraphicsTextureSource_BlitTiledSaturatedAddRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
        g_GraphicsTextureSourceBlitSaturatedAddRgb
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}


/* GraphicsTextureSource_BlitTiledSourceAlpha with g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd as the
   per-tile blit (installed as g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd).
*/
void GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
        g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}


/* Work state of GraphicsTextureSource_DecomposeSubresourceRegions while it cuts a sprite sheet into regions.
   The work area is the new asset's block: header (and palette bank), then the record table growing upwards,
   free bytes, the packed sprite pixels growing downwards, and the working copy of the sheet at the end. */
typedef struct GraphicsTextureDecomposeState {
  GraphicsTextureSourceAsset *asset;   /* the new asset */
  GraphicsTextureSourceEntry *records; /* its subresource record table */
  int sourceWidth;                     /* pixels per row of the sheet, the row stride of the working copy */
  int rowsRemaining;                   /* rows from the scanned row to the bottom of the sheet, counting it */
  int freeBytes;                       /* bytes of the work area not yet taken by records or packed pixels */
  uint8_t *packedPixels;               /* lowest packed sprite pixel */
  uint32_t packedPixelBytes;
  bool edgeTransparent;                /* the edge colour is transparent, so border rows/columns in it are trimmed */
} GraphicsTextureDecomposeState;

/* Takes `amount` bytes of the work area's free bytes. Returns false (taking nothing) unless at least one byte
   stays free. */
static bool GraphicsTextureDecompose_ReserveBytes(int *freeBytes,int amount)
{
  int bytesLeft;

  bytesLeft = *freeBytes - amount;
  if (bytesLeft == 0 || *freeBytes < amount) {
    return false;
  }
  *freeBytes = bytesLeft;
  return true;
}

/* Copies `count` dwords upwards from the lowest one (the packed pixels move down onto the record table end). */
static void GraphicsTextureDecompose_CopyDwords(uint32_t *destination,const uint32_t *source,uint32_t count)
{
  for (; count != 0; count--) {
    *destination = *source;
    destination++;
    source++;
  }
}

/* Appends a record to the new asset's table: logical height and origin zero, the given palette index; the
   caller fills in the sizes. */
static GraphicsTextureSourceEntry *GraphicsTextureDecompose_AddRecord
          (GraphicsTextureDecomposeState *state,GraphicsPaletteIndex paletteIndex)
{
  GraphicsTextureSourceEntry *record;
  AssetSubresourceCount recordIndex;

  recordIndex = (state->asset->tableDescriptor).subresourceCount;
  (state->asset->tableDescriptor).subresourceCount = recordIndex + 1;
  record = &state->records[recordIndex];
  record->logicalHeight = 0;
  record->originX = 0;
  record->originY = 0;
  record->paletteIndex = paletteIndex;
  return record;
}

/* True when the `count` (at least 1) ARGB pixels from `pixel` on, `step` pixels apart, all equal `color`. */
static bool GraphicsTextureDecompose_ArgbRunIs(const uint32_t *pixel,uint32_t count,int step,uint32_t color)
{
  for (; count != 0; count--) {
    if (*pixel != color) {
      return false;
    }
    pixel = pixel + step;
  }
  return true;
}

/* True when the `count` (at least 1) palette indices from `pixel` on, `step` bytes apart, all equal `index`. */
static bool GraphicsTextureDecompose_IndexRunIs(const uint8_t *pixel,uint32_t count,int step,uint8_t index)
{
  for (; count != 0; count--) {
    if (*pixel != index) {
      return false;
    }
    pixel = pixel + step;
  }
  return true;
}

/* Cuts the region whose top-left pixel (not background) is `blockStart` out of the ARGB working copy;
   `columnsLeft` counts the pixels from blockStart to the end of its row. Adds its record (logical width = the
   run of non-background pixels in this row, logical height = the run in this column, palette index -1), trims
   border rows and columns in the edge colour when that is transparent (the origin records how much was cut at
   the top/left; at least one row and column stay), copies the remaining pixels in front of the packed ones and
   clears the whole logical block to background. Returns false when the work area is full. */
static bool GraphicsTextureDecompose_CutArgbRegion
          (GraphicsTextureDecomposeState *state,uint32_t *blockStart,int columnsLeft,uint32_t background,
          uint32_t edgeColor)
{
  GraphicsTextureSourceEntry *record;
  int sourceWidth;
  int count;
  int pixelCount;
  uint32_t rowCount;
  uint32_t column;
  uint32_t *pixel;
  uint32_t *topLeft;
  uint32_t *bottomRow;
  uint32_t *rightColumn;
  uint32_t *packed;

  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,GFX_SUBRESOURCE_RECORD_SIZE)) {
    return false;
  }
  sourceWidth = state->sourceWidth;
  record = GraphicsTextureDecompose_AddRecord(state,-1);
  pixel = blockStart;
  for (count = columnsLeft; count != 0 && *pixel != background; count--) {
    pixel++;
  }
  record->logicalWidth = (uint32_t)(pixel - blockStart);
  pixel = blockStart;
  count = state->rowsRemaining;
  do {
    record->logicalHeight = record->logicalHeight + 1;
    pixel = pixel + sourceWidth;
    count--;
  } while (count != 0 && *pixel != background);
  record->pixelWidth = record->logicalWidth;
  record->pixelHeight = record->logicalHeight;
  topLeft = blockStart;
  if (state->edgeTransparent) {
    while (GraphicsTextureDecompose_ArgbRunIs(topLeft,record->logicalWidth,1,edgeColor)) {
      record->originY = record->originY + 1;
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->originY = record->originY - 1;
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      topLeft = topLeft + sourceWidth;
    }
    bottomRow = topLeft + sourceWidth * (record->pixelHeight - 1);
    while (GraphicsTextureDecompose_ArgbRunIs(bottomRow,record->logicalWidth,1,edgeColor)) {
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      bottomRow = bottomRow - sourceWidth;
    }
    while (GraphicsTextureDecompose_ArgbRunIs(topLeft,record->pixelHeight,sourceWidth,edgeColor)) {
      record->originX = record->originX + 1;
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->originX = record->originX - 1;
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      topLeft = topLeft + 1;
    }
    rightColumn = topLeft + (record->pixelWidth - 1);
    while (GraphicsTextureDecompose_ArgbRunIs(rightColumn,record->pixelHeight,sourceWidth,edgeColor)) {
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      rightColumn = rightColumn - 1;
    }
  }
  pixelCount = record->pixelWidth * record->pixelHeight;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,pixelCount * 4)) {
    return false;
  }
  state->packedPixels = state->packedPixels - pixelCount * 4;
  state->packedPixelBytes = state->packedPixelBytes + pixelCount * 4;
  packed = (uint32_t *)state->packedPixels;
  pixel = topLeft;
  for (rowCount = record->pixelHeight; rowCount != 0; rowCount--) {
    GraphicsTextureDecompose_CopyDwords(packed,pixel,record->pixelWidth);
    packed = packed + record->pixelWidth;
    pixel = pixel + sourceWidth;
  }
  pixel = blockStart;
  for (rowCount = record->logicalHeight; rowCount != 0; rowCount--) {
    for (column = 0; column < record->logicalWidth; column++) {
      pixel[column] = background;
    }
    pixel = pixel + sourceWidth;
  }
  return true;
}

/* GraphicsTextureDecompose_CutArgbRegion on 8-bit palette indices: the record gets palette index 0 and the
   packed pixels are padded to whole dwords. */
static bool GraphicsTextureDecompose_CutIndexedRegion
          (GraphicsTextureDecomposeState *state,uint8_t *blockStart,int columnsLeft,uint8_t backgroundIndex,
          uint8_t edgeIndex)
{
  GraphicsTextureSourceEntry *record;
  int sourceWidth;
  int count;
  uint32_t packedBytes;
  uint32_t rowCount;
  uint32_t column;
  uint8_t *pixel;
  uint8_t *topLeft;
  uint8_t *bottomRow;
  uint8_t *rightColumn;
  uint8_t *packed;

  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,GFX_SUBRESOURCE_RECORD_SIZE)) {
    return false;
  }
  sourceWidth = state->sourceWidth;
  record = GraphicsTextureDecompose_AddRecord(state,0);
  pixel = blockStart;
  for (count = columnsLeft; count != 0 && *pixel != backgroundIndex; count--) {
    pixel++;
  }
  record->logicalWidth = (uint32_t)(pixel - blockStart);
  pixel = blockStart;
  count = state->rowsRemaining;
  do {
    record->logicalHeight = record->logicalHeight + 1;
    pixel = pixel + sourceWidth;
    count--;
  } while (count != 0 && *pixel != backgroundIndex);
  record->pixelWidth = record->logicalWidth;
  record->pixelHeight = record->logicalHeight;
  topLeft = blockStart;
  if (state->edgeTransparent) {
    while (GraphicsTextureDecompose_IndexRunIs(topLeft,record->logicalWidth,1,edgeIndex)) {
      record->originY = record->originY + 1;
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->originY = record->originY - 1;
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      topLeft = topLeft + sourceWidth;
    }
    bottomRow = topLeft + sourceWidth * (record->pixelHeight - 1);
    while (GraphicsTextureDecompose_IndexRunIs(bottomRow,record->logicalWidth,1,edgeIndex)) {
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      bottomRow = bottomRow - sourceWidth;
    }
    while (GraphicsTextureDecompose_IndexRunIs(topLeft,record->pixelHeight,sourceWidth,edgeIndex)) {
      record->originX = record->originX + 1;
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->originX = record->originX - 1;
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      topLeft = topLeft + 1;
    }
    rightColumn = topLeft + (record->pixelWidth - 1);
    while (GraphicsTextureDecompose_IndexRunIs(rightColumn,record->pixelHeight,sourceWidth,edgeIndex)) {
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      rightColumn = rightColumn - 1;
    }
  }
  packedBytes = record->pixelWidth * record->pixelHeight + 3U & ~3u;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,(int)packedBytes)) {
    return false;
  }
  state->packedPixels = state->packedPixels - packedBytes;
  state->packedPixelBytes = state->packedPixelBytes + packedBytes;
  packed = state->packedPixels;
  pixel = topLeft;
  for (rowCount = record->pixelHeight; rowCount != 0; rowCount--) {
    for (column = 0; column < record->pixelWidth; column++) {
      packed[column] = pixel[column];
    }
    packed = packed + record->pixelWidth;
    pixel = pixel + sourceWidth;
  }
  pixel = blockStart;
  for (rowCount = record->logicalHeight; rowCount != 0; rowCount--) {
    for (column = 0; column < record->logicalWidth; column++) {
      pixel[column] = backgroundIndex;
    }
    pixel = pixel + sourceWidth;
  }
  return true;
}

/* Decomposition of a subresource with direct ARGB8888 pixels: no palette, the new record table follows the
   header. Returns 0 (asset complete and shrunk), 0x2D (nothing but background), FATAL_ERROR_GENERAL_FAILURE
   (work area too small) or the allocator's error. */
static uint32_t GraphicsTextureDecompose_ArgbRegions
          (GraphicsTextureDecomposeState *state,const uint32_t *sourcePixels)
{
  GraphicsTextureSourceAsset *asset;
  uint32_t background;
  uint32_t edgeColor;
  uint32_t pixelCount;
  uint32_t scanLeft;
  const uint32_t *sourcePixel;
  uint32_t *workPixels;
  uint32_t *scanCursor;
  int columnsLeft;
  uint32_t tableBytes;
  uint32_t arenaError;
  uint32_t pixelDataOffset;
  GraphicsTextureSourceEntry *record;
  AssetSubresourceCount recordsLeft;

  asset = state->asset;
  (asset->tableDescriptor).paletteBankCount = 0;
  (asset->tableDescriptor).subresourceCount = 0;
  (asset->tableDescriptor).subresourceTableOffset = GFX_ASSET_HEADER_SIZE;
  state->records = (GraphicsTextureSourceEntry *)((uint8_t *)asset + GFX_ASSET_HEADER_SIZE);
  /* the first pixel is the background; the first other pixel gives the edge colour */
  background = sourcePixels[0];
  pixelCount = state->sourceWidth * state->rowsRemaining;
  sourcePixel = sourcePixels;
  for (scanLeft = pixelCount; scanLeft != 0 && *sourcePixel == background; scanLeft--) {
    sourcePixel++;
  }
  if (scanLeft == 0) {
    /* nothing but background (error 0x2D, shared with the cursor frame check) */
    return FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE;
  }
  edgeColor = *sourcePixel;
  /* the working copy of the pixels goes to the end of the work area */
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,(int)(pixelCount * 4))) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  workPixels = (uint32_t *)((uint8_t *)state->records + state->freeBytes);
  GraphicsTextureDecompose_CopyDwords(workPixels,sourcePixels,pixelCount & DWORD_COUNT_MASK);
  state->edgeTransparent = (edgeColor & ARGB8888_ALPHA_MASK) == 0;
  state->packedPixels = (uint8_t *)workPixels;
  state->packedPixelBytes = 0;
  /* Scan the working copy row by row for the next non-background pixel; each hit starts a region that is cut
     out and cleared to background, then the scan goes on at the same pixel. */
  scanCursor = workPixels;
  do {
    columnsLeft = state->sourceWidth;
    while (columnsLeft != 0) {
      if (*scanCursor != background) {
        if (!GraphicsTextureDecompose_CutArgbRegion(state,scanCursor,columnsLeft,background,edgeColor)) {
          return FATAL_ERROR_GENERAL_FAILURE;
        }
      }
      else {
        scanCursor++;
        columnsLeft--;
      }
    }
    state->rowsRemaining--;
  } while (state->rowsRemaining != 0);
  /* move the packed pixels down behind the record table, shrink the block to header + table + pixels
     and give every record its pixel offset, counting back from the end */
  tableBytes = (asset->tableDescriptor).subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
  (asset->common).allocationSizeBytes = state->packedPixelBytes + tableBytes + GFX_ASSET_HEADER_SIZE;
  GraphicsTextureDecompose_CopyDwords
            ((uint32_t *)((uint8_t *)state->records + tableBytes),(uint32_t *)state->packedPixels,
             state->packedPixelBytes >> 2);
  arenaError = g_MemoryApi.shrinkInPlace((asset->common).allocationSizeBytes,asset);
  if (arenaError != 0) {
    return arenaError;
  }
  pixelDataOffset = (asset->common).allocationSizeBytes;
  record = state->records;
  recordsLeft = (asset->tableDescriptor).subresourceCount;
  do {
    pixelDataOffset = pixelDataOffset - record->pixelWidth * record->pixelHeight * 4;
    record->dataOffset = pixelDataOffset;
    record++;
    recordsLeft--;
  } while (recordsLeft != 0);
  return 0;
}

/* Decomposition of a subresource with 8-bit palette indices: the new asset carries the subresource's palette
   bank (`sourcePalette`) as its only bank and the record table follows it; otherwise as
   GraphicsTextureDecompose_ArgbRegions, on bytes. */
static uint32_t GraphicsTextureDecompose_IndexedRegions
          (GraphicsTextureDecomposeState *state,const uint8_t *sourcePalette,const uint8_t *sourcePixels)
{
  GraphicsTextureSourceAsset *asset;
  uint8_t backgroundIndex;
  uint8_t edgeIndex;
  int pixelCount;
  int scanLeft;
  const uint8_t *sourcePixel;
  uint8_t *workPixels;
  uint8_t *workPixel;
  uint8_t *scanCursor;
  GraphicsTexturePaletteEntry *edgeEntry;
  int columnsLeft;
  uint32_t tableBytes;
  uint32_t arenaError;
  uint32_t pixelDataOffset;
  GraphicsTextureSourceEntry *record;
  AssetSubresourceCount recordsLeft;

  asset = state->asset;
  (asset->tableDescriptor).paletteBankCount = 1;
  (asset->tableDescriptor).subresourceCount = 0;
  (asset->tableDescriptor).subresourceTableOffset = GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE;
  state->records = (GraphicsTextureSourceEntry *)((uint8_t *)asset + (GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE));
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,GFX_PALETTE_BANK_SIZE)) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  GraphicsTextureDecompose_CopyDwords
            ((uint32_t *)((uint8_t *)asset + GFX_ASSET_HEADER_SIZE),(const uint32_t *)sourcePalette,
             GFX_PALETTE_BANK_SIZE / 4);
  backgroundIndex = sourcePixels[0];
  pixelCount = state->sourceWidth * state->rowsRemaining;
  sourcePixel = sourcePixels;
  for (scanLeft = pixelCount; scanLeft != 0 && *sourcePixel == backgroundIndex; scanLeft--) {
    sourcePixel++;
  }
  if (scanLeft == 0) {
    /* nothing but background (error 0x2D, shared with the cursor frame check) */
    return FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE;
  }
  edgeIndex = *sourcePixel;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,pixelCount)) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  workPixels = (uint8_t *)state->records + state->freeBytes;
  sourcePixel = sourcePixels;
  workPixel = workPixels;
  for (scanLeft = pixelCount; scanLeft != 0; scanLeft--) {
    *workPixel = *sourcePixel;
    sourcePixel++;
    workPixel++;
  }
  /* the edge colour's entry in the new palette always loses its alpha, i.e. becomes transparent */
  edgeEntry = &((GraphicsPaletteTextureSourceAsset *)asset)->paletteEntries[(uint32_t)edgeIndex];
  state->edgeTransparent = (edgeEntry->argb8888 & ARGB8888_ALPHA_MASK) == 0;
  edgeEntry->argb8888 = edgeEntry->argb8888 & ARGB8888_RGB_MASK;
  /* packed sprites are padded to whole dwords */
  state->packedPixels = (uint8_t *)((uint32_t)workPixels & ~3u);
  state->packedPixelBytes = 0;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,3)) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  scanCursor = workPixels;
  do {
    columnsLeft = state->sourceWidth;
    while (columnsLeft != 0) {
      if (*scanCursor != backgroundIndex) {
        if (!GraphicsTextureDecompose_CutIndexedRegion(state,scanCursor,columnsLeft,backgroundIndex,edgeIndex)) {
          return FATAL_ERROR_GENERAL_FAILURE;
        }
      }
      else {
        scanCursor++;
        columnsLeft--;
      }
    }
    state->rowsRemaining--;
  } while (state->rowsRemaining != 0);
  tableBytes = (asset->tableDescriptor).subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
  (asset->common).allocationSizeBytes =
       state->packedPixelBytes + tableBytes + (GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE);
  GraphicsTextureDecompose_CopyDwords
            ((uint32_t *)((uint8_t *)state->records + tableBytes),(uint32_t *)state->packedPixels,
             state->packedPixelBytes >> 2);
  arenaError = g_MemoryApi.shrinkInPlace((asset->common).allocationSizeBytes,asset);
  if (arenaError != 0) {
    return arenaError;
  }
  pixelDataOffset = (asset->common).allocationSizeBytes;
  record = state->records;
  recordsLeft = (asset->tableDescriptor).subresourceCount;
  do {
    pixelDataOffset = pixelDataOffset - (record->pixelWidth * record->pixelHeight + 3 & ~3u);
    record->dataOffset = pixelDataOffset;
    record++;
    recordsLeft--;
  } while (recordsLeft != 0);
  return 0;
}

/* Cuts one subresource of a 'gfx' texture source (a sheet of sprites) into its separate sprites and returns
   them as a new 'gfx' asset, one subresource per sprite (installed as
   g_GraphicsTextureSourceDecomposeSubresourceRegionsCf). The first pixel is the background; each block of
   other pixels (as wide as the run in its first row, as high as the run in its first column) becomes one
   subresource, trimmed of border rows/columns in the colour of the first non-background pixel when that colour
   is transparent, and is then cleared from the work copy. The work area is the largest free arena block,
   shrunk to the result at the end. Returns true with the new asset in *outAsset; returns false with
   FATAL_ERROR_GFX_ASSET_INVALID, 0x2D (nothing but background), FATAL_ERROR_GENERAL_FAILURE (work area too
   small) or the allocator's error in *outError. No caller in the game code (only the hook slot).
*/
bool GraphicsTextureSource_DecomposeSubresourceRegions
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset,
          GraphicsTextureSourceAsset **outAsset,uint32_t *outError)

{
  GraphicsTextureSourceEntry *sourceEntry;
  GraphicsTextureDecomposeState state;
  GraphicsTextureSourceAsset *decomposedAsset;
  GraphicsPaletteIndex paletteIndex;
  uint8_t *sourcePixels;
  uint32_t largestBlockSize;
  uint32_t packedDateTime;
  uint32_t error;

  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
     ((sourceAsset->tableDescriptor).subresourceCount <= entryIndex)) {
    *outError = FATAL_ERROR_GFX_ASSET_INVALID;
    return false;
  }
  sourceEntry = (GraphicsTextureSourceEntry *)
                ((uint8_t *)sourceAsset +
                 (entryIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset));
  state.sourceWidth = sourceEntry->pixelWidth;
  state.rowsRemaining = sourceEntry->pixelHeight;
  error = g_MemoryApi.allocLargestFreeBlock((void **)&decomposedAsset,&largestBlockSize);
  if (error != 0) {
    *outError = error;
    return false;
  }
  /* the new asset starts as a copy of the source header */
  GraphicsTextureDecompose_CopyDwords
            ((uint32_t *)decomposedAsset,(const uint32_t *)sourceAsset,GFX_ASSET_HEADER_SIZE / 4);
  paletteIndex = sourceEntry->paletteIndex;
  state.asset = decomposedAsset;
  state.freeBytes = (int)largestBlockSize;
  if (!GraphicsTextureDecompose_ReserveBytes(&state.freeBytes,GFX_ASSET_HEADER_SIZE)) {
    error = FATAL_ERROR_GENERAL_FAILURE;
  }
  else {
    /* the new asset is stamped with the current time and date and this computer's name */
    packedDateTime = g_LocaleGetPackedCurrentTime();
    ((decomposedAsset)->common).buildMetadata.timestamps.timeValue1 = packedDateTime;
    ((decomposedAsset)->common).buildMetadata.timestamps.timeValue2 = packedDateTime;
    packedDateTime = g_LocaleGetPackedCurrentDate();
    ((decomposedAsset)->common).buildMetadata.timestamps.dateValue1 = packedDateTime;
    ((decomposedAsset)->common).buildMetadata.timestamps.dateValue2 = packedDateTime;
    g_LocaleCopyDefaultComputerLabelUtf16
              (((decomposedAsset)->common).buildMetadata.names.sourceName);
    sourcePixels = (uint8_t *)sourceAsset + sourceEntry->dataOffset;
    if (paletteIndex == -1) {
      error = GraphicsTextureDecompose_ArgbRegions(&state,(const uint32_t *)sourcePixels);
    }
    else {
      error = GraphicsTextureDecompose_IndexedRegions
                        (&state,(uint8_t *)sourceAsset + GFX_ASSET_HEADER_SIZE + paletteIndex * GFX_PALETTE_BANK_SIZE,
                         sourcePixels);
    }
    if (error == 0) {
      *outAsset = decomposedAsset;
      return true;
    }
  }
  g_MemoryApi.free(decomposedAsset);
  *outError = error;
  return false;
}

/* Loads a 'gfx' texture source for the software renderer (installed as g_GraphicsTextureSourceLoadPackageAsset;
   used for the UI, text and selection-panel graphics): the package entry is loaded and its palettes are converted
   to the current framebuffer format. Returns the texture source (never NULL: the conversion rejects NULL).
   If the conversion fails the entry is released again; on failure returns NULL and stores the load or
   conversion error in *outError (when outError is not NULL).
*/
GraphicsTextureSourceAsset *GraphicsTextureSource_LoadPackageAsset(uint16_t *pathUtf16,uint32_t *outError)

{
  GraphicsPaletteTextureSourceAsset *loadedSource;
  uint32_t loadError;

  loadedSource = Package_LoadEntry(pathUtf16,&loadError);
  if (loadedSource != NULL) {
    loadError = g_GraphicsTextureSourceConvertPaletteEntries(loadedSource);
    if (loadError == 0) {
      return (GraphicsTextureSourceAsset *)loadedSource;
    }
    Resource_Release(loadedSource);
  }
  if (outError != NULL) {
    *outError = loadError;
  }
  return NULL;
}


/* Makes a private heap copy of a 'gfx' texture source with its palettes converted to the current framebuffer
   format, so it can be modified independently (g_GraphicsTextureSourceLifecycleCallbacks3.clone). Returns the
   copy; on failure it returns the allocation error cast to a pointer, or after a failed conversion the
   result of freeing the copy again (as the original, which also flagged the failure separately).
*/
GraphicsTextureSourceAsset *
GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsPaletteTextureSourceAsset *clonedAsset;
  uint32_t allocationSizeBytes;
  uint32_t dwordsRemaining;
  const uint32_t *sourceDword;
  uint32_t *cloneDword;
  uint32_t cloneAllocationError;

  allocationSizeBytes = (sourceAsset->common).allocationSizeBytes;
  cloneAllocationError = g_MemoryApi.alloc(allocationSizeBytes,(void **)&clonedAsset);
  if (cloneAllocationError != 0) {
    return (GraphicsTextureSourceAsset *)cloneAllocationError;
  }
  /* copy the whole allocation dword by dword (a trailing partial dword is not copied) */
  sourceDword = (const uint32_t *)sourceAsset;
  cloneDword = (uint32_t *)clonedAsset;
  for (dwordsRemaining = allocationSizeBytes >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *cloneDword = *sourceDword;
    sourceDword++;
    cloneDword++;
  }
  if (g_GraphicsTextureSourceConvertPaletteEntries(clonedAsset) == 0) {
    return (GraphicsTextureSourceAsset *)clonedAsset;
  }
  /* Original quirk: the clone's result after a failed conversion is the free's status (0 = NULL) */
  return (GraphicsTextureSourceAsset *)g_MemoryApi.free(clonedAsset);
}


/* Fills the framebuffer-pixel half of every palette entry of a 'gfx' texture source from its ARGB8888 half,
   packed for the current framebuffer format through g_SoftwarePixelPackTables (alpha is kept in the top byte),
   so the software blits can copy palette colours directly (installed as
   g_GraphicsTextureSourceConvertPaletteEntries). Returns 0 on success, FATAL_ERROR_GFX_ASSET_INVALID for a
   NULL or non-'gfx' asset (the asset itself is the success value of the original).
*/
uint32_t GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset)

{
  int paletteEntriesRemaining;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
  uint32_t argb8888;
  
  if ((sourceAsset != NULL) &&
     (sourceAsset->magic == GRAPHICS_PALETTE_TEXTURE_MAGIC_GFX)) {
    paletteEntryCursor = sourceAsset->paletteEntries;
    for (paletteEntriesRemaining = sourceAsset->paletteBankCount << 8; paletteEntriesRemaining != 0;
        paletteEntriesRemaining--) {
      argb8888 = paletteEntryCursor->argb8888;
      /* the red and green shifts yield byte offsets into the dword tables (channel value * 4) */
      paletteEntryCursor->framebufferPixel =
           (argb8888 & ARGB8888_ALPHA_MASK) +
           *(int *)((int)g_SoftwarePixelPackTables->red + ((argb8888 & ARGB8888_RED_MASK) >> 14)) +
           *(int *)((int)g_SoftwarePixelPackTables->green + ((argb8888 & ARGB8888_GREEN_MASK) >> 6)) +
           g_SoftwarePixelPackTables->blue[argb8888 & ARGB8888_BLUE_MASK];
      paletteEntryCursor++;
    }
    return 0;
  }
  return FATAL_ERROR_GFX_ASSET_INVALID;
}


/* Releases a texture source loaded by GraphicsTextureSource_LoadPackageAsset back to the resource cache
   (g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage).
*/
void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;

  allocation = g_GraphicsTextureSourceResolveAllocationBase(sourceAsset);
  Resource_Release(allocation);
  return;
}


/* Frees a copy made by GraphicsTextureSource_CloneAsset (g_GraphicsTextureSourceLifecycleCallbacks3.releaseClone).
*/
void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;

  allocation = g_GraphicsTextureSourceResolveAllocationBase(sourceAsset);
  g_MemoryApi.free(allocation);
  return;
}


/* Returns the allocation that holds a texture source (installed as g_GraphicsTextureSourceResolveAllocationBase);
   the asset is its own allocation, but both release callbacks ask this slot first.
*/
GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset)

{
  return sourceAsset;
}


/* GraphicsTextureSet_AllocateMetadata: fills set->entries from the asset's source entries (rows
   GFX_SUBRESOURCE_RECORD_SIZE bytes apart, starting at firstSourceEntry), entryCount of them; at least one
   entry is always processed, as in the original. Each entry gets no texture yet, its image index, the source
   asset and entry, and log2 of the width and height (31 for a zero size). Returns false at the first image
   whose width or height is not a power of two (that entry is left partly written). */
static bool GraphicsTextureSet_FillEntries
          (GraphicsTextureSet *set,GraphicsTextureSourceAsset *sourceAsset,uint8_t *firstSourceEntry,
           GraphicsAssetAllocationByteSize entryCount)
{
  GraphicsTextureSetEntry *entry = set->entries;
  GraphicsTextureSourceEntry *sourceEntry = (GraphicsTextureSourceEntry *)firstSourceEntry;
  GraphicsAssetAllocationByteSize entriesRemaining = entryCount;
  int entryIndex = 0;
  uint32_t widthLog2;
  int heightLog2;

  do {
    /* index of the highest set bit of pixelWidth; the original leaves the register undefined for 0 */
    widthLog2 = 31;
    if (sourceEntry->pixelWidth != 0) {
      while (sourceEntry->pixelWidth >> widthLog2 == 0) {
        widthLog2--;
      }
    }
    entry->texture = NULL;
    entry->subresourceIndex = entryIndex;
    entry->widthLog2 = widthLog2;
    if (1 << ((uint8_t)widthLog2 & SHIFT_COUNT_MASK) != sourceEntry->pixelWidth) {
      return false;
    }
    entry->sourceAsset = sourceAsset;
    /* index of the highest set bit of pixelHeight */
    heightLog2 = 31;
    if (sourceEntry->pixelHeight != 0) {
      while (sourceEntry->pixelHeight >> heightLog2 == 0) {
        heightLog2--;
      }
    }
    entry->sourceEntry = sourceEntry;
    entry->heightLog2 = heightLog2;
    if (1 << ((uint8_t)heightLog2 & SHIFT_COUNT_MASK) != sourceEntry->pixelHeight) {
      return false;
    }
    entry++;
    sourceEntry = (GraphicsTextureSourceEntry *)((uint8_t *)sourceEntry + GFX_SUBRESOURCE_RECORD_SIZE);
    entryIndex++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return true;
}

/* Builds a texture set for a 'gfx' asset: converts its palettes to the display format, then allocates the
   set (an 8-byte header with the source asset and image count, then one 0x20-byte GraphicsTextureSetEntry
   per image) and fills each entry with the image index, source entry and log2 of its width and height.
   Returns the set (never NULL), or NULL with the conversion/arena error or FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO
   in *outErrorCode (outErrorCode may be NULL; on the size error the set is not freed, as in the original).
*/
GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode)

{
  GraphicsPaletteTextureSourceAsset *convertedSource;
  GraphicsTextureSet *set;
  uint32_t errorCode;
  GraphicsAssetAllocationByteSize entryCount;

  convertedSource = (GraphicsPaletteTextureSourceAsset *)sourceAsset;
  errorCode = g_GraphicsTextureSourceConvertPaletteEntries(convertedSource);
  if (errorCode == 0) {
    entryCount = convertedSource->subresourceCount;
    errorCode = g_MemoryApi.alloc(entryCount * GRAPHICS_TEXTURE_SET_ENTRY_BYTES + 8,(void **)&set);
    if (errorCode == 0) {
      set->sourceAsset = sourceAsset;
      set->subresourceCount = entryCount;
      if (GraphicsTextureSet_FillEntries
               (set,sourceAsset,(uint8_t *)convertedSource + convertedSource->subresourceTableOffset,entryCount)) {
        return set;
      }
      errorCode = FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO;
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return NULL;
}


/* Counterpart of GraphicsTextureSet_AllocateMetadata: frees the set and returns its source asset so the
   caller can release that too. NULL for a NULL set.
*/
GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *sourceAsset;

  sourceAsset = NULL;
  if (set != NULL) {
    sourceAsset = set->sourceAsset;
    g_MemoryApi.free(set);
  }
  return sourceAsset;
}


/* Enters a texture into the first free slot of g_GraphicsTextureSlots (the registry the original used to evict
   and rebuild device textures). Returns true when all GRAPHICS_TEXTURE_SLOT_CAPACITY slots are taken.
*/
bool GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture)

{
  int slotsRemaining;
  GraphicsTextureResource **slotCursor;
  
  slotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
  slotCursor = g_GraphicsTextureSlots;
  do {
    if (*slotCursor == NULL) {
      *slotCursor = texture;
      return false;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return true;
}

