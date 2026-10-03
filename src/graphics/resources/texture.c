/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/texture.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/resources/texture. */

/* Box filter of the 2x/4x downsampling uploads, written out per channel (the original unpacks the ARGB8888 texels
   to word lanes, adds them with PADDW, shifts right and repacks with PACKUSWB). */
#define TEXTURE_TEXEL_BLUE(texel) ((uint16_t)(uint8_t)(texel))
#define TEXTURE_TEXEL_GREEN(texel) ((uint16_t)(uint8_t)((uint32_t)(texel) >> 8))
#define TEXTURE_TEXEL_RED(texel) ((uint16_t)(uint8_t)((uint32_t)(texel) >> 16))
#define TEXTURE_TEXEL_ALPHA(texel) ((uint16_t)(uint8_t)((uint32_t)(texel) >> 24))
/* PACKUSWB of one averaged word lane: values above 0xff saturate to 0xff. */
#define TEXTURE_SATURATE_TO_BYTE(lane) ((uint32_t)((ARGB8888_CHANNEL_MAX < (lane)) ? ARGB8888_CHANNEL_MAX : (uint8_t)(lane)))

/* Address: 0x0057E970.
   Creates the renderer textures of a texture asset: allocates the set metadata and hands it to the Glide
   backend, or (DirectDraw) creates one texture resource per subresource with its staging and device
   texture and registers it in g_GraphicsTextureSlots. A subresource whose allocation or registration
   fails is left NULL; only a failed metadata allocation fails the call: then NULL is returned with its error in
   *outErrorCode (outErrorCode may be NULL). Never NULL on success.
*/
GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode)

{
  GraphicsTextureSourceAsset *setSourceAsset;
  uint32_t currentDownsampleShift;
  int adapterIndex;
  GraphicsAdapterRecord *adapters;
  DDPIXELFORMAT *selectedPixelFormat;
  GraphicsTextureResource *newTexture;
  bool registerFailed;
  GraphicsTextureSet *allocatedSet;
  uint32_t textureAllocationError;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount entriesRemaining;
  GraphicsSubresourceIndex subresourceIndex;

  adapters = g_GraphicsAdapters;
  adapterIndex = g_ActiveGraphicsAdapterIndex;
  allocatedSet = GraphicsTextureSet_AllocateMetadata(sourceAsset,outErrorCode);
  if (allocatedSet == NULL) {
    return NULL;
  }
  if (adapters[adapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    /* the Glide backend always succeeds (returns false) */
    Glide3_TextureSet_CreateBackend(allocatedSet,sourceAsset);
    return allocatedSet;
  }
  setSourceAsset = allocatedSet->sourceAsset;
  entriesRemaining = (setSourceAsset->tableDescriptor).subresourceCount;
  entryCursor = allocatedSet->entries;
  subresourceIndex = 0;
  do {
    selectedPixelFormat = GraphicsTexture_SelectPixelFormat(subresourceIndex,setSourceAsset);
    textureAllocationError = g_MemoryApi.alloc(sizeof(GraphicsTextureResource),(void **)&newTexture);
    if (textureAllocationError != 0) {
      newTexture = (GraphicsTextureResource *)textureAllocationError;
    }
    else {
      newTexture->stagingTexture2 = NULL;
      newTexture->stagingSurface3 = NULL;
      newTexture->stagingSurfaceBase = NULL;
      entryCursor->texture = newTexture;
      newTexture->deviceTexture2 = NULL;
      newTexture->deviceSurface3 = NULL;
      newTexture->deviceSurfaceBase = NULL;
      newTexture->sourceAsset = setSourceAsset;
      newTexture->subresourceIndex = subresourceIndex;
      newTexture->pixelFormat = selectedPixelFormat;
      currentDownsampleShift = g_TextureDownsampleShift;
      newTexture->lastUsedCounter = 0;
      newTexture->textureHandle = 0;
      newTexture->downsampleShift = currentDownsampleShift;
      newTexture = GraphicsTexture_CreateStagingTexture(newTexture);
      registerFailed = GraphicsTexture_RegisterSlot(newTexture);
      if (registerFailed) {
        GraphicsTexture_ReleaseObjects(newTexture);
        g_MemoryApi.free(newTexture);
        entryCursor->texture = NULL;
      }
      else {
        GraphicsTexture_CreateDeviceTexture(newTexture);
      }
    }
    subresourceIndex++;
    entryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return allocatedSet;
}


/* Address: 0x0057AD30.
   Recreates the texture objects of every registered texture, e.g. after the display mode or device
   changed: Glide reinitialises its own resources; for DirectDraw each resource's surfaces are released and
   its staging texture rebuilt, while the device texture is recreated lazily when next used.
*/
void GraphicsTexture_RebuildAllStagingTextures(void)

{
  GraphicsTextureResource *texture;
  int textureSlotsRemaining;
  GraphicsTextureResource **textureSlotCursor;

  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    Glide3_TextureResource_ReinitializeAll();
    return;
  }
  textureSlotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
  textureSlotCursor = g_GraphicsTextureSlots;
  do {
    texture = *textureSlotCursor;
    if (texture != NULL) {
      GraphicsTexture_ReleaseObjects(texture);
      GraphicsTexture_CreateStagingTexture(texture);
    }
    textureSlotCursor++;
    textureSlotsRemaining--;
  } while (textureSlotsRemaining != 0);
}


/* Address: 0x0057EAF0.
   Destroys a texture set made by GraphicsTextureSet_Create: every texture resource is removed from
   g_GraphicsTextureSlots, released and freed, then the set metadata is freed. Returns the source asset
   the set was built from, so the caller can release it too.
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

  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    releasedSourceAsset = Glide3_TextureSet_DestroyBackend(set,set);
    return releasedSourceAsset;
  }
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
        GraphicsTexture_ReleaseObjects(texture);
        g_MemoryApi.free(texture);
      }
      entryCursor++;
      entriesRemaining--;
    } while (entriesRemaining != 0);
    releasedSourceAsset = GraphicsTextureSet_FreeMetadata(set);
  }
  return releasedSourceAsset;
}


/* Address: 0x00485E40.
   Loads a 'gfx' texture source from the package and builds a renderer texture set from it through
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


/* Address: 0x00485E80.
   Counterpart of GraphicsTextureSet_LoadPackage (installed as g_GraphicsTextureSetReleasePackage): destroys the
   texture set through g_GraphicsDestroyTextureSet and releases the 'gfx' source asset that call hands back.
*/
void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *sourceAsset;

  sourceAsset = g_GraphicsDestroyTextureSet(set);
  Resource_Release(sourceAsset);
  return;
}


/* Address: 0x00485FC0.
   Default of g_GraphicsRefreshTextureColor and g_GraphicsRefreshTextureAlpha: the default texture sets
   (GraphicsTextureSet_AllocateMetadata) own no renderer textures, so there is nothing to re-upload until
   Graphics_Init installs GraphicsTextureSet_RefreshColor/RefreshAlpha.
*/
void GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  return;
}


/* Address: 0x00486070.
   Default of g_GraphicsRebuildAllStagingTextures: without renderer textures there is nothing to rebuild after
   a display mode change until Graphics_Init installs GraphicsTexture_RebuildAllStagingTextures.
*/
void __cdecl GraphicsTexture_RebuildNoOp(void)

{
  return;
}

/* Address: 0x004A9270.
   Returns the logical width and height of one subresource of a 'gfx' texture source, i.e. the extent the
   tiled blits repeat (installed as g_GraphicsTextureSourceGetLogicalSize), or 0 x 0 when the asset is not a
   'gfx' asset or the index is out of range (the original signalled that with CF and left the size registers
   untouched; the only callers that test it, the glyph size queries, use width 0 then).
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


/* Address: 0x004A92C0.
   Hit test of a sprite drawn at (drawX, drawY) (installed as g_GraphicsTextureSourceTestOpaquePixel): maps the
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


/* Address: 0x004A9A50.
   Fills a rectangle with copies of one subresource laid out on its logical-size grid anchored at the tile
   origin, drawing each copy with g_GraphicsTextureSourceBlitSourceAlpha (installed as
   g_GraphicsTextureSourceBlitTiledSourceAlpha; also called directly by the text controls). The area ends at
   repeatEnd (GRAPHICS_TILED_BLIT_ONE_TILE: one tile past the origin), clipped to clipMax and the framebuffer;
   CF is always clear on return.
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
  /* ADD / JLE: step the origin by whole tiles until it is positive and past the clip minimum; the tile
     before it is the first one drawn */
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


/* Address: 0x004AA0A0.
   GraphicsTextureSource_BlitTiledSourceAlpha with g_GraphicsTextureSourceBlitHalfSourceRgb as the per-tile
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
  /* ADD / JLE: step the origin by whole tiles until it is positive and past the clip minimum; the tile
     before it is the first one drawn */
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


/* Address: 0x004AB9A0.
   GraphicsTextureSource_BlitTiledSourceAlpha with g_GraphicsTextureSourceBlitSaturatedAddRgb as the per-tile
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
  /* ADD / JLE: step the origin by whole tiles until it is positive and past the clip minimum; the tile
     before it is the first one drawn */
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


/* Address: 0x004ABF70.
   GraphicsTextureSource_BlitTiledSourceAlpha with g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd as the
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
  /* ADD / JLE: step the origin by whole tiles until it is positive and past the clip minimum; the tile
     before it is the first one drawn */
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

/* Address: 0x004AC8E0.
   Cuts one subresource of a 'gfx' texture source (a sheet of sprites) into its separate sprites and returns
   them as a new 'gfx' asset, one subresource per sprite (installed as
   g_GraphicsTextureSourceDecomposeSubresourceRegionsCf). The first pixel is the background; each block of
   other pixels (as wide as the run in its first row, as high as the run in its first column) becomes one
   subresource, trimmed of border rows/columns in the colour of the first non-background pixel when that colour
   is transparent, and is then cleared from the work copy. The work area is the largest free arena block,
   shrunk to the result at the end. Returns true with the new asset in *outAsset; returns false with
   FATAL_ERROR_GFX_ASSET_INVALID, 0x2D (nothing but background), FATAL_ERROR_GENERAL_FAILURE (work area too
   small) or the allocator's error in *outError. No caller in the recovered code (only the hook slot).
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

/* Address: 0x004AD630.
   Loads a 'gfx' texture source for the software renderer (installed as g_GraphicsTextureSourceLoadPackageAsset;
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


/* Address: 0x004AD670.
   Makes a private heap copy of a 'gfx' texture source with its palettes converted to the current framebuffer
   format, so it can be modified independently (g_GraphicsTextureSourceLifecycleCallbacks3.clone). Returns the
   copy; the original sets CF on failure and returns the allocation error, or after a failed conversion the
   result of freeing the copy again (this C signature has no CF).
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


/* Address: 0x004AD6C0.
   Fills the framebuffer-pixel half of every palette entry of a 'gfx' texture source from its ARGB8888 half,
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


/* Address: 0x004AD770.
   Releases a texture source loaded by GraphicsTextureSource_LoadPackageAsset back to the resource cache
   (g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage).
*/
void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;

  allocation = g_GraphicsTextureSourceResolveAllocationBase(sourceAsset);
  Resource_Release(allocation);
  return;
}


/* Address: 0x004AD790.
   Frees a copy made by GraphicsTextureSource_CloneAsset (g_GraphicsTextureSourceLifecycleCallbacks3.releaseClone).
*/
void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;

  allocation = g_GraphicsTextureSourceResolveAllocationBase(sourceAsset);
  g_MemoryApi.free(allocation);
  return;
}


/* Address: 0x004AD7B0.
   Returns the allocation that holds a texture source (installed as g_GraphicsTextureSourceResolveAllocationBase);
   the asset is its own allocation, but both release callbacks ask this slot first.
*/
GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset)

{
  return sourceAsset;
}


/* Shared upload body, defined below with its helpers (also used by GraphicsTexture_UploadColor_2x/_4x). */
static void GraphicsTextureUploadScaled_Upload(GraphicsTextureResource *texture,uint32_t blockSize);

/* Address: 0x0057ADA0.
   Copies the pixels of a texture's subresource into its staging surface at full size, converted to the
   surface's pixel format (g_GraphicsDispatchTable.colorUpload[0], used for downsample shift 0 by
   GraphicsTexture_CreateStagingTexture and GraphicsTextureSet_RefreshColor). Direct ARGB8888 and paletted
   sources are handled; for RGB surfaces each channel is shifted into the position of its mask (16-bit or
   32-bit stores); an 8-bit surface receives the palette indices, or the alpha byte of direct pixels, and a
   matching palette (a grey ramp for direct pixels) is built. A lost surface is restored first;
   g_ActiveTextureUploads is raised for the duration. With a block size of 1 this is exactly the shared
   scaled-upload path (one source pixel per block, the "average" of one texel is the texel itself).
*/
void GraphicsTexture_UploadColor_1x(GraphicsTextureResource *texture)

{
  GraphicsTextureUploadScaled_Upload(texture,1);
}


/* Index of the lowest set bit of a DirectDraw channel mask (the mask must not be zero). */
static int GraphicsTextureUploadScaled_LowestMaskBit(uint32_t mask)
{
  int bit = 0;

  while (((mask >> bit) & 1) == 0) {
    bit++;
  }
  return bit;
}

/* Index of the highest set bit of a DirectDraw channel mask (the mask must not be zero). */
static int GraphicsTextureUploadScaled_HighestMaskBit(uint32_t mask)
{
  int bit = 31;

  while ((mask >> bit) == 0) {
    bit--;
  }
  return bit;
}

/* Placement of the ARGB8888 bytes in a destination pixel: each byte is shifted right to its mask's width,
   then left to the mask's lowest bit. */
typedef struct GraphicsTextureUploadScaledShifts {
  int alphaShiftRight;
  int redShiftRight;
  int greenShiftRight;
  int blueShiftRight;
  int alphaShiftLeft;
  int redShiftLeft;
  int greenShiftLeft;
  int blueShiftLeft;
} GraphicsTextureUploadScaledShifts;

/* Shifts for an RGB destination format whose red, green and blue masks are all non-zero. Without an alpha
   mask the alpha byte is pushed out of the pixel (right 0, left 16). */
static void GraphicsTextureUploadScaled_ComputeShifts
          (const DDPIXELFORMAT *destinationFormat,GraphicsTextureUploadScaledShifts *shifts)
{
  uint32_t alphaMask;

  shifts->redShiftLeft = GraphicsTextureUploadScaled_LowestMaskBit(destinationFormat->dwRBitMask);
  shifts->greenShiftLeft = GraphicsTextureUploadScaled_LowestMaskBit(destinationFormat->dwGBitMask);
  shifts->blueShiftLeft = GraphicsTextureUploadScaled_LowestMaskBit(destinationFormat->dwBBitMask);
  shifts->redShiftRight =
       24 - ((GraphicsTextureUploadScaled_HighestMaskBit(destinationFormat->dwRBitMask) + 1) - shifts->redShiftLeft);
  shifts->greenShiftRight =
       16 - ((GraphicsTextureUploadScaled_HighestMaskBit(destinationFormat->dwGBitMask) + 1) - shifts->greenShiftLeft);
  shifts->blueShiftRight =
       8 - ((GraphicsTextureUploadScaled_HighestMaskBit(destinationFormat->dwBBitMask) + 1) - shifts->blueShiftLeft);
  alphaMask = destinationFormat->dwRGBAlphaBitMask;
  if (alphaMask == 0) {
    shifts->alphaShiftRight = 0;
    shifts->alphaShiftLeft = 16;
  }
  else {
    shifts->alphaShiftLeft = GraphicsTextureUploadScaled_LowestMaskBit(alphaMask);
    shifts->alphaShiftRight =
         32 - ((GraphicsTextureUploadScaled_HighestMaskBit(alphaMask) + 1) - shifts->alphaShiftLeft);
  }
}

/* An ARGB8888 colour converted to the destination format (16-bit surfaces keep the low word). */
static uint32_t GraphicsTextureUploadScaled_PlaceChannels
          (uint32_t argb,const GraphicsTextureUploadScaledShifts *shifts)
{
  return ((argb & ARGB8888_ALPHA_MASK) >> ((uint8_t)shifts->alphaShiftRight & SHIFT_COUNT_MASK)) <<
         ((uint8_t)shifts->alphaShiftLeft & SHIFT_COUNT_MASK) |
         ((argb & ARGB8888_BLUE_MASK) >> ((uint8_t)shifts->blueShiftRight & SHIFT_COUNT_MASK)) <<
         ((uint8_t)shifts->blueShiftLeft & SHIFT_COUNT_MASK) |
         ((argb & ARGB8888_GREEN_MASK) >> ((uint8_t)shifts->greenShiftRight & SHIFT_COUNT_MASK)) <<
         ((uint8_t)shifts->greenShiftLeft & SHIFT_COUNT_MASK) |
         ((argb & ARGB8888_RED_MASK) >> ((uint8_t)shifts->redShiftRight & SHIFT_COUNT_MASK)) <<
         ((uint8_t)shifts->redShiftLeft & SHIFT_COUNT_MASK);
}

/* Average of the blockSize x blockSize source block whose top-left pixel is at `block` (rows sourceWidth
   pixels apart), as ARGB8888. Direct sources (palette == NULL) hold ARGB8888 pixels, paletted ones one palette
   index per byte. Each channel is its texel sum (as a 16-bit word) divided by the texel count (4 or 16, a
   right shift by 2 or 4 in the original), saturated to a byte like PACKUSWB. For blockSize 1 (the 1x upload,
   which converts each pixel directly) this returns the texel unchanged. */
static uint32_t GraphicsTextureUploadScaled_AverageBlock
          (const uint8_t *block,uint32_t sourceWidth,const GraphicsTexturePaletteEntry *palette,uint32_t blockSize)
{
  uint32_t texelCount = blockSize * blockSize;
  uint32_t alphaSum = 0;
  uint32_t redSum = 0;
  uint32_t greenSum = 0;
  uint32_t blueSum = 0;
  uint32_t row;
  uint32_t column;
  uint32_t texel;
  uint16_t averageAlpha;
  uint16_t averageRed;
  uint16_t averageGreen;
  uint16_t averageBlue;

  for (row = 0; row < blockSize; row++) {
    for (column = 0; column < blockSize; column++) {
      if (palette == NULL) {
        texel = ((const uint32_t *)block)[row * sourceWidth + column];
      }
      else {
        texel = palette[block[row * sourceWidth + column]].argb8888;
      }
      alphaSum += TEXTURE_TEXEL_ALPHA(texel);
      redSum += TEXTURE_TEXEL_RED(texel);
      greenSum += TEXTURE_TEXEL_GREEN(texel);
      blueSum += TEXTURE_TEXEL_BLUE(texel);
    }
  }
  averageAlpha = (uint16_t)((uint16_t)alphaSum / texelCount);
  averageRed = (uint16_t)((uint16_t)redSum / texelCount);
  averageGreen = (uint16_t)((uint16_t)greenSum / texelCount);
  averageBlue = (uint16_t)((uint16_t)blueSum / texelCount);
  return TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24 |
         TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
         TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
         TEXTURE_SATURATE_TO_BYTE(averageBlue);
}

/* RGB surfaces: one averaged pixel per blockSize x blockSize source block (16-bit pixels when wordPixels,
   else 32-bit). Widths and heights are rounded up to whole blocks. */
static void GraphicsTextureUploadScaled_DownsampleRgb
          (const uint8_t *sourcePixels,uint32_t sourceWidth,int sourceHeight,
           const GraphicsTexturePaletteEntry *palette,uint8_t *destinationRow,TH_LEGACY_LONG destinationPitch,
           bool wordPixels,const GraphicsTextureUploadScaledShifts *shifts,uint32_t blockSize)
{
  uint32_t bytesPerPixel = (palette == NULL) ? 4 : 1;
  const uint8_t *sourceRow = sourcePixels;
  int rowsRemaining = sourceHeight;
  const uint8_t *block;
  uint16_t *destinationWord;
  uint32_t *destinationDword;
  int columnsRemaining;
  uint32_t pixel;

  do {
    block = sourceRow;
    destinationWord = (uint16_t *)destinationRow;
    destinationDword = (uint32_t *)destinationRow;
    columnsRemaining = (int)sourceWidth;
    do {
      pixel = GraphicsTextureUploadScaled_PlaceChannels
                   (GraphicsTextureUploadScaled_AverageBlock(block,sourceWidth,palette,blockSize),shifts);
      if (wordPixels) {
        *destinationWord = (uint16_t)pixel;
        destinationWord++;
      }
      else {
        *destinationDword = pixel;
        destinationDword++;
      }
      block += blockSize * bytesPerPixel;
      columnsRemaining -= (int)blockSize;
    } while (columnsRemaining > 0);
    sourceRow += sourceWidth * blockSize * bytesPerPixel;
    destinationRow += destinationPitch;
    rowsRemaining -= (int)blockSize;
  } while (rowsRemaining > 0);
}

/* 8-bit surfaces: one byte per blockSize x blockSize source block, byte `byteOffset` of the block's top-left
   source pixel (the alpha byte of a direct pixel, the index of a paletted one). */
static void GraphicsTextureUploadScaled_CopyTopLeftBytes
          (const uint8_t *sourcePixels,uint32_t sourceWidth,int sourceHeight,uint32_t bytesPerPixel,
           uint32_t byteOffset,uint8_t *destinationRow,TH_LEGACY_LONG destinationPitch,uint32_t blockSize)
{
  const uint8_t *sourceRow = sourcePixels;
  int rowsRemaining = sourceHeight;
  const uint8_t *block;
  uint8_t *destination;
  int columnsRemaining;

  do {
    block = sourceRow;
    destination = destinationRow;
    columnsRemaining = (int)sourceWidth;
    do {
      *destination = block[byteOffset];
      destination++;
      block += blockSize * bytesPerPixel;
      columnsRemaining -= (int)blockSize;
    } while (columnsRemaining > 0);
    sourceRow += sourceWidth * blockSize * bytesPerPixel;
    destinationRow += destinationPitch;
    rowsRemaining -= (int)blockSize;
  } while (rowsRemaining > 0);
}

/* 8-bit surfaces: fills g_TexturePaletteEntries (a grey ramp for direct sources, the source's palette bank
   otherwise) and creates a DirectDraw palette from it. As in the original, the palette is released at once
   and never attached (no SetPalette). */
static void GraphicsTextureUploadScaled_CreateUnusedPalette(const GraphicsTexturePaletteEntry *palette)
{
  DirectDrawPaletteEntry *paletteEntry = g_TexturePaletteEntries;
  const uint8_t *argbBytes;
  IDirectDrawPalette *createdPalette;
  int entryIndex;

  for (entryIndex = 0; entryIndex < 256; entryIndex++) {
    if (palette == NULL) {
      /* grey ramp: all four bytes equal the entry index */
      paletteEntry->red = (uint8_t)entryIndex;
      paletteEntry->green = (uint8_t)entryIndex;
      paletteEntry->blue = (uint8_t)entryIndex;
      paletteEntry->flags = (uint8_t)entryIndex;
    }
    else {
      /* the four bytes of the entry's argb8888 word, copied in memory order */
      argbBytes = (const uint8_t *)&palette[entryIndex].argb8888;
      paletteEntry->red = argbBytes[0];
      paletteEntry->green = argbBytes[1];
      paletteEntry->blue = argbBytes[2];
      paletteEntry->flags = argbBytes[3];
    }
    paletteEntry++;
  }
  if (g_DirectDraw2->lpVtbl->CreatePalette
           (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,NULL) == 0) {
    createdPalette->lpVtbl->Release(createdPalette);
  }
}

/* GraphicsTextureUploadScaled_Upload once g_SurfaceDesc holds the locked staging surface: fills it with one
   pixel per blockSize x blockSize source block and unlocks it (8-bit surfaces get their palette built after
   the unlock). */
static void GraphicsTextureUploadScaled_FillLockedSurface
          (GraphicsTextureResource *texture,IDirectDrawSurface3 *stagingSurface3,
           GraphicsTextureSourceAsset *sourceAsset,uint32_t subresourceIndex,uint32_t blockSize)
{
  TH_LEGACY_LPVOID surfaceBits = g_SurfaceDesc.lpSurface;
  TH_LEGACY_LONG destinationPitch = g_SurfaceDesc.lPitch;
  const GraphicsTextureSourceEntry *entry;
  const GraphicsTexturePaletteEntry *palette;
  const uint8_t *sourcePixels;
  uint32_t sourceWidth;
  int sourceHeight;
  DDPIXELFORMAT *destinationFormat;
  GraphicsTextureUploadScaledShifts shifts;

  entry = (const GraphicsTextureSourceEntry *)
          ((uint8_t *)sourceAsset + (int)(subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                          (sourceAsset->tableDescriptor).subresourceTableOffset));
  sourceWidth = entry->pixelWidth;
  sourceHeight = (int)entry->pixelHeight;
  sourcePixels = (const uint8_t *)sourceAsset + entry->dataOffset;
  /* a negative palette index marks direct ARGB8888 pixels; bank n follows the asset header at n * 0x800 */
  palette = NULL;
  if (entry->paletteIndex >= 0) {
    palette = (const GraphicsTexturePaletteEntry *)(sourceAsset + entry->paletteIndex * 4 + 1);
  }
  if ((sourceWidth != 0) && (sourceHeight != 0)) {
    destinationFormat = texture->pixelFormat;
    if (destinationFormat->dwRGBBitCount == 8) {
      if (palette == NULL) {
        GraphicsTextureUploadScaled_CopyTopLeftBytes
                  (sourcePixels,sourceWidth,sourceHeight,4,3,surfaceBits,destinationPitch,blockSize);
      }
      else {
        GraphicsTextureUploadScaled_CopyTopLeftBytes
                  (sourcePixels,sourceWidth,sourceHeight,1,0,surfaceBits,destinationPitch,blockSize);
      }
      stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      GraphicsTextureUploadScaled_CreateUnusedPalette(palette);
      return;
    }
    if ((destinationFormat->dwRBitMask != 0) && (destinationFormat->dwGBitMask != 0) &&
        (destinationFormat->dwBBitMask != 0)) {
      GraphicsTextureUploadScaled_ComputeShifts(destinationFormat,&shifts);
      GraphicsTextureUploadScaled_DownsampleRgb
                (sourcePixels,sourceWidth,sourceHeight,palette,surfaceBits,destinationPitch,
                 destinationFormat->dwRGBBitCount < 17,&shifts,blockSize);
    }
  }
  stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
}

/* Shared body of GraphicsTexture_UploadColor_1x (blockSize 1), GraphicsTexture_UploadColor_2x (blockSize 2)
   and GraphicsTexture_UploadColor_4x (blockSize 4): restores a lost staging surface, locks it and fills it. */
static void GraphicsTextureUploadScaled_Upload(GraphicsTextureResource *texture,uint32_t blockSize)
{
  IDirectDrawSurface3 *stagingSurface3;
  GraphicsTextureSourceAsset *sourceAsset;
  uint32_t subresourceIndex;
  TH_LEGACY_HRESULT restoreResult;

  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    restoreResult = 0;
    if (stagingSurface3->lpVtbl->IsLost(stagingSurface3) != 0) {
      restoreResult = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (restoreResult == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      if (stagingSurface3->lpVtbl->Lock(stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,NULL) == 0) {
        GraphicsTextureUploadScaled_FillLockedSurface
                  (texture,stagingSurface3,sourceAsset,subresourceIndex,blockSize);
      }
    }
  }
  g_ActiveTextureUploads--;
}

/* Address: 0x0057B410.
   GraphicsTexture_UploadColor_1x at half size (g_GraphicsDispatchTable.colorUpload[1]): every destination
   pixel is the average of a 2x2 source block (8-bit surfaces take the top-left source pixel).
*/
void GraphicsTexture_UploadColor_2x(GraphicsTextureResource *texture)

{
  GraphicsTextureUploadScaled_Upload(texture,2);
}


/* Address: 0x0057BBE0.
   GraphicsTexture_UploadColor_1x at quarter size (g_GraphicsDispatchTable.colorUpload[2]): every destination
   pixel is the average of a 4x4 source block (8-bit surfaces take the top-left source pixel).
*/
void GraphicsTexture_UploadColor_4x(GraphicsTextureResource *texture)

{
  GraphicsTextureUploadScaled_Upload(texture,4);
}


/* Unscaled alpha of one destination pixel: at full size (blockSize 1) the source byte itself, otherwise the
   16-bit sum of four bytes of the blockSize x blockSize source block: the whole 2x2 block at half size, the
   bytes at (0,0), (2,0), (0,2) and (2,2) at quarter size. */
static uint16_t GraphicsTextureUploadAlpha_SampleBlock(const uint8_t *maskCursor,int maskWidth,int blockSize)
{
  int tap;

  if (blockSize == 1) {
    return *maskCursor;
  }
  tap = blockSize / 2;
  return (uint16_t)((uint16_t)maskCursor[0] + (uint16_t)maskCursor[tap] + (uint16_t)maskCursor[maskWidth * tap] +
                    (uint16_t)maskCursor[maskWidth * tap + tap]);
}

/* GraphicsTextureUploadAlpha_Upload once g_SurfaceDesc holds the locked staging surface: writes one alpha
   value per blockSize x blockSize block of source bytes (8-bit surfaces are left untouched) and unlocks it. */
static void GraphicsTextureUploadAlpha_FillLockedSurface
          (GraphicsTextureResource *texture,IDirectDrawSurface3 *stagingSurface3,
           GraphicsTextureSourceAsset *sourceAsset,GraphicsSubresourceIndex subresourceIndex,int blockSize)
{
  TH_LEGACY_LPVOID surfaceBits = g_SurfaceDesc.lpSurface;
  TH_LEGACY_LONG destinationPitch = g_SurfaceDesc.lPitch;
  const GraphicsTextureSourceEntry *entry;
  const uint8_t *maskCursor;
  int maskWidth;
  int rowsRemaining;
  int columnsRemaining;
  DDPIXELFORMAT *destinationFormat;
  bool sixteenBitPixels;
  int topAlphaBit;
  uint8_t rotateShift;
  uint32_t rgbMaskBits;
  uint16_t maskSum;
  uint8_t *destinationRow;
  uint8_t *destination;

  entry = (const GraphicsTextureSourceEntry *)
          ((uint8_t *)sourceAsset + (int)(subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                          (sourceAsset->tableDescriptor).subresourceTableOffset));
  maskWidth = (int)entry->pixelWidth;
  rowsRemaining = (int)entry->pixelHeight;
  maskCursor = (const uint8_t *)sourceAsset + entry->dataOffset;
  destinationFormat = texture->pixelFormat;
  if ((maskWidth != 0) && (rowsRemaining != 0) && (destinationFormat->dwRGBBitCount != 8)) {
    topAlphaBit = 31;
    if (destinationFormat->dwRGBAlphaBitMask != 0) {
      topAlphaBit = GraphicsTextureUploadScaled_HighestMaskBit(destinationFormat->dwRGBAlphaBitMask);
    }
    /* ROL by (top alpha bit - 7) puts the byte's top bit on the mask's top bit (top alpha bit - 9 for the
       10-bit sum of four bytes); bits that spill into the RGB fields are hidden by OR-ing all RGB mask bits */
    rotateShift = (uint8_t)(topAlphaBit - ((blockSize == 1) ? 7 : 9)) & SHIFT_COUNT_MASK;
    rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
    sixteenBitPixels = destinationFormat->dwRGBBitCount < 17;
    destinationRow = (uint8_t *)surfaceBits;
    do {
      destination = destinationRow;
      columnsRemaining = maskWidth;
      do {
        maskSum = GraphicsTextureUploadAlpha_SampleBlock(maskCursor,maskWidth,blockSize);
        if (sixteenBitPixels) {
          *(uint16_t *)destination =
               (uint16_t)(maskSum << rotateShift | maskSum >> (32 - rotateShift) | (uint16_t)rgbMaskBits);
        }
        else {
          *(uint32_t *)destination =
               (uint32_t)maskSum << rotateShift | (uint32_t)(maskSum >> (32 - rotateShift)) | rgbMaskBits;
        }
        /* Original quirk: 32-bit stores also advance only 2 bytes (ADD EDI,0x2), so each one overlaps the
           previous one. */
        destination += 2;
        maskCursor += blockSize;
        columnsRemaining -= blockSize;
      } while (columnsRemaining > 0);
      /* skip the source rows the block covered below this one */
      maskCursor += maskWidth * (blockSize - 1);
      destinationRow += destinationPitch;
      rowsRemaining -= blockSize;
    } while (rowsRemaining > 0);
  }
  stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
}

/* Shared body of GraphicsTexture_UploadAlpha_1x (blockSize 1), GraphicsTexture_UploadAlpha_2x (blockSize 2)
   and GraphicsTexture_UploadAlpha_4x (blockSize 4): restores a lost staging surface, locks it and fills it. */
static void GraphicsTextureUploadAlpha_Upload(GraphicsTextureResource *texture,int blockSize)
{
  IDirectDrawSurface3 *stagingSurface3;
  GraphicsTextureSourceAsset *sourceAsset;
  GraphicsSubresourceIndex subresourceIndex;
  TH_LEGACY_HRESULT restoreResult;

  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    restoreResult = 0;
    if (stagingSurface3->lpVtbl->IsLost(stagingSurface3) != 0) {
      restoreResult = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (restoreResult == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      if (stagingSurface3->lpVtbl->Lock(stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,NULL) == 0) {
        GraphicsTextureUploadAlpha_FillLockedSurface
                  (texture,stagingSurface3,sourceAsset,subresourceIndex,blockSize);
      }
    }
  }
  g_ActiveTextureUploads--;
}

/* Address: 0x0057C6C0.
   Alpha counterpart of GraphicsTexture_UploadColor_1x (g_GraphicsDispatchTable.alphaUpload[0], called by
   GraphicsTextureSet_RefreshAlpha): the subresource's pixels are read as one alpha byte each and written, at
   full size, into the alpha bits of the staging surface with all RGB bits set (white). 8-bit surfaces are
   skipped. A lost surface is restored first; g_ActiveTextureUploads is raised for the duration.
*/
void GraphicsTexture_UploadAlpha_1x(GraphicsTextureResource *texture)

{
  GraphicsTextureUploadAlpha_Upload(texture,1);
}


/* Address: 0x0057C890.
   GraphicsTexture_UploadAlpha_1x at half size (g_GraphicsDispatchTable.alphaUpload[1]): each destination alpha
   is the 10-bit sum of a 2x2 block of source bytes, scaled to the alpha mask.
*/
void GraphicsTexture_UploadAlpha_2x(GraphicsTextureResource *texture)

{
  GraphicsTextureUploadAlpha_Upload(texture,2);
}


/* Address: 0x0057CAA0.
   GraphicsTexture_UploadAlpha_1x at quarter size (g_GraphicsDispatchTable.alphaUpload[2]): unlike the colour
   upload this is no full 4x4 average, each destination alpha is the sum of the four source bytes at (0,0),
   (2,0), (0,2) and (2,2) of its block, scaled like the 2x version.
*/
void GraphicsTexture_UploadAlpha_4x(GraphicsTextureResource *texture)

{
  GraphicsTextureUploadAlpha_Upload(texture,4);
}


/* Address: 0x0057EBB0.
   Re-uploads the colour channels of one subresource after its source pixels changed (installed as
   g_GraphicsRefreshTextureColor): the colour upload for the texture's downsample level refills the staging texture,
   and an existing device texture is reloaded from it.
*/
void GraphicsTextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;

  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    Glide3_TextureSet_RefreshColor(subresourceIndex,set);
    return;
  }
  texture = set->entries[subresourceIndex].texture;
  g_GraphicsDispatchTable.colorUpload[texture->downsampleShift](texture);
  if (texture->deviceTexture2 != NULL) {
    texture->deviceTexture2->lpVtbl->Load(texture->deviceTexture2,texture->stagingTexture2);
    g_TextureDeviceReloadCount++;
  }
}


/* Address: 0x0057EC40.
   Like GraphicsTextureSet_RefreshColor, but re-uploads only the alpha channel of the subresource
   (g_GraphicsRefreshTextureAlpha; used for the shading texture set).
*/
void GraphicsTextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;

  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    Glide3_TextureSet_RefreshAlpha(subresourceIndex,set);
    return;
  }
  texture = set->entries[subresourceIndex].texture;
  g_GraphicsDispatchTable.alphaUpload[texture->downsampleShift](texture);
  if (texture->deviceTexture2 != NULL) {
    texture->deviceTexture2->lpVtbl->Load(texture->deviceTexture2,texture->stagingTexture2);
    g_TextureDeviceReloadCount++;
  }
}


/* One creation attempt of GraphicsTexture_CreateDeviceTexture, with g_SurfaceDesc describing the device
   surface: creates the surface, queries its IDirectDrawSurface3 and IDirect3DTexture2 interfaces and loads the
   staging texture into it. Returns the result of the first failing step (0 when all succeeded); the objects
   obtained so far are left in *deviceSurfaceBase, *deviceSurface3 and *deviceTexture2. */
static TH_LEGACY_HRESULT GraphicsTexture_CreateAndLoadDeviceSurface
          (GraphicsTextureResource *texture,IDirectDrawSurface **deviceSurfaceBase,
           IDirectDrawSurface3 **deviceSurface3,IDirect3DTexture2 **deviceTexture2)
{
  TH_LEGACY_HRESULT hresult;

  hresult = g_DirectDraw2->lpVtbl->CreateSurface(g_DirectDraw2,&g_SurfaceDesc,deviceSurfaceBase,NULL);
  if (hresult != 0) {
    return hresult;
  }
  hresult = (*deviceSurfaceBase)->lpVtbl->QueryInterface
                    (*deviceSurfaceBase,&IID_IDirectDrawSurface3_Local,deviceSurface3);
  if (hresult != 0) {
    return hresult;
  }
  hresult = (*deviceSurface3)->lpVtbl->QueryInterface
                    (*deviceSurface3,&IID_IDirect3DTexture2_Local,deviceTexture2);
  if (hresult != 0) {
    return hresult;
  }
  return (*deviceTexture2)->lpVtbl->Load(*deviceTexture2,texture->stagingTexture2);
}

/* Releases the objects of a failed device texture attempt (texture interface first) and clears the pointers. */
static void GraphicsTexture_ReleaseDeviceAttempt
          (IDirectDrawSurface **deviceSurfaceBase,IDirectDrawSurface3 **deviceSurface3,
           IDirect3DTexture2 **deviceTexture2)
{
  if (*deviceTexture2 != NULL) {
    (*deviceTexture2)->lpVtbl->Release(*deviceTexture2);
    *deviceTexture2 = NULL;
  }
  if (*deviceSurface3 != NULL) {
    (*deviceSurface3)->lpVtbl->Release(*deviceSurface3);
    *deviceSurface3 = NULL;
  }
  if (*deviceSurfaceBase != NULL) {
    (*deviceSurfaceBase)->lpVtbl->Release(*deviceSurfaceBase);
    *deviceSurfaceBase = NULL;
  }
}

/* Address: 0x0057AAD0.
   Makes a texture usable by the Direct3D device: creates the square device surface (video memory for a
   hardware device, system memory for an emulated one), loads the staging texture into it and stores the
   surface, texture interface and D3D texture handle in texture. On DDERR_OUTOFVIDEOMEMORY the least recently
   used device texture is evicted and the creation retried; any other failure leaves the device fields NULL.
   Preserves EAX; callers keep texture in it.
*/
void GraphicsTexture_CreateDeviceTexture(GraphicsTextureResource *texture)

{
  D3DDEVICEDESC_DX6 *deviceDesc;
  uint32_t largerExtent;
  TH_LEGACY_HRESULT hresult;
  GraphicsTextureDownsampleShift effectiveShift;
  GraphicsTextureLogicalSize logicalSize;
  uint32_t textureHandle;
  IDirect3DTexture2 *deviceTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  
  deviceSurfaceBase = NULL;
  deviceSurface3 = NULL;
  deviceTexture2 = NULL;
  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
  deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
    g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
    return;
  }
  g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(texture->subresourceIndex,texture->sourceAsset);
  /* a hardware device description without a colour model means the device is emulated in software */
  if (deviceDesc->dcmColorModel == 0) {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | DDSCAPS_SYSTEMMEMORY;
  }
  else {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | DDSCAPS_VIDEOMEMORY;
  }
  largerExtent = logicalSize.logicalWidthPixels;
  if (logicalSize.logicalWidthPixels < logicalSize.logicalHeightPixels) {
    largerExtent = logicalSize.logicalHeightPixels;
  }
  g_SurfaceDesc.dwHeight = largerExtent >> ((uint8_t)g_TextureDownsampleShift & SHIFT_COUNT_MASK);
  effectiveShift = g_TextureDownsampleShift;
  /* textures are at least 16x16: lower the downsample shift until the edge reaches 16 */
  while ((int)g_SurfaceDesc.dwHeight <= 15) {
    g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
    effectiveShift = effectiveShift - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
    if (effectiveShift == GRAPHICS_TEXTURE_DOWNSAMPLE_1X) {
      break;
    }
  }
  texture->downsampleShift = effectiveShift;
  g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
  g_SurfaceDesc.ddpfPixelFormat = *texture->pixelFormat;
  /* A failed attempt is released; on DDERR_OUTOFVIDEOMEMORY the oldest device texture is evicted and the
     creation retried, as long as there was one to evict (the eviction returns true when there was none). */
  do {
    hresult = GraphicsTexture_CreateAndLoadDeviceSurface(texture,&deviceSurfaceBase,&deviceSurface3,&deviceTexture2);
    if (hresult != 0) {
      GraphicsTexture_ReleaseDeviceAttempt(&deviceSurfaceBase,&deviceSurface3,&deviceTexture2);
    }
  } while ((hresult == DDERR_OUTOFVIDEOMEMORY) && !GraphicsTexture_EvictOldestDeviceTexture(texture));
  if ((hresult == 0) &&
      (deviceTexture2->lpVtbl->GetHandle(deviceTexture2,g_Direct3DDevice2,&textureHandle) == 0)) {
    texture->deviceSurfaceBase = deviceSurfaceBase;
    texture->deviceSurface3 = deviceSurface3;
    texture->deviceTexture2 = deviceTexture2;
    texture->textureHandle = textureHandle;
    return;
  }
  /* creation failed (nothing left to release) or GetHandle failed (no retry): release what is held */
  GraphicsTexture_ReleaseDeviceAttempt(&deviceSurfaceBase,&deviceSurface3,&deviceTexture2);
  texture->deviceSurfaceBase = NULL;
  texture->deviceSurface3 = NULL;
  texture->deviceTexture2 = NULL;
  texture->textureHandle = 0;
  return;
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

/* Address: 0x00485EA0.
   Builds a texture set for a 'gfx' asset: converts its palettes to the display format, then allocates the
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


/* Address: 0x00485F90.
   Counterpart of GraphicsTextureSet_AllocateMetadata: frees the set and returns its source asset so the
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


/* Address: 0x0057A9D0.
   Frees video memory: releases the device-side surfaces and texture of the least recently used registered
   texture that has a device handle (other than exclude); its staging copy stays so it can be reloaded later.
   Unbinds the handle if it was bound. CF set when there was nothing to evict.
*/
bool GraphicsTexture_EvictOldestDeviceTexture(GraphicsTextureResource *exclude)

{
  GraphicsTextureResource *candidate;
  uint32_t evictedHandle;
  int slotsRemaining;
  GraphicsResourceUsageSerial oldestUsage;
  GraphicsTextureResource *oldestTexture;
  GraphicsTextureResource **slotCursor;
  IDirect3DTexture2 *deviceTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  
  slotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
  oldestUsage = UINT32_MAX;
  oldestTexture = NULL;
  slotCursor = g_GraphicsTextureSlots;
  do {
    candidate = *slotCursor;
    if ((((candidate != NULL) && (candidate != exclude)) &&
        (candidate->textureHandle != 0)) && (candidate->lastUsedCounter <= oldestUsage)) {
      oldestUsage = candidate->lastUsedCounter;
      oldestTexture = candidate;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  if (oldestTexture == NULL) {
    return true;
  }
  deviceTexture2 = oldestTexture->deviceTexture2;
  if (deviceTexture2 != NULL) {
    deviceTexture2->lpVtbl->Release(deviceTexture2);
  }
  deviceSurface3 = oldestTexture->deviceSurface3;
  if (deviceSurface3 != NULL) {
    deviceSurface3->lpVtbl->Release(deviceSurface3);
  }
  deviceSurfaceBase = oldestTexture->deviceSurfaceBase;
  if (deviceSurfaceBase != NULL) {
    deviceSurfaceBase->lpVtbl->Release(deviceSurfaceBase);
  }
  evictedHandle = oldestTexture->textureHandle;
  oldestTexture->deviceSurfaceBase = NULL;
  oldestTexture->deviceSurface3 = NULL;
  oldestTexture->deviceTexture2 = NULL;
  oldestTexture->textureHandle = 0;
  if (evictedHandle == g_BoundTextureHandle) {
    g_BoundTextureHandle = 0;
    g_Direct3DDevice2->lpVtbl->SetRenderState(g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
  }
  return false;
}


/* Address: 0x0057E870.
   Enters a texture into the first free slot of g_GraphicsTextureSlots, the registry used to evict device
   textures and to rebuild all staging textures. CF set when all GRAPHICS_TEXTURE_SLOT_CAPACITY slots are
   taken.
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


/* Address: 0x0057E8C0.
   Picks the texture pixel format for one image: the alpha format if any pixel (direct ARGB image) or any of
   the 256 palette colours (palette image) is not fully opaque, else the opaque format. Palette images use
   the "selected" pair of formats.
*/
DDPIXELFORMAT *
GraphicsTexture_SelectPixelFormat
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  const GraphicsTextureSourceEntry *entry;
  int paletteIndex;
  int pixelsRemaining;
  int paletteEntriesRemaining;
  const uint32_t *pixelCursor;
  const GraphicsTexturePaletteEntry *paletteEntryCursor;

  entry = (const GraphicsTextureSourceEntry *)
          ((uint8_t *)sourceAsset +
           (subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset));
  paletteIndex = entry->paletteIndex;
  if (paletteIndex < 0) {
    /* no palette: scan the ARGB pixels */
    pixelCursor = (const uint32_t *)((uint8_t *)sourceAsset + entry->dataOffset);
    /* Original quirk: a 0-pixel image wraps the count and scans on past its data */
    pixelsRemaining = entry->pixelWidth * entry->pixelHeight;
    do {
      if (*pixelCursor < ARGB8888_ALPHA_MASK) { /* alpha below 0xFF */
        return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
      }
      pixelCursor++;
      pixelsRemaining--;
    } while (pixelsRemaining != 0);
    return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0);
  }
  /* palette bank paletteIndex: GFX_PALETTE_BANK_SIZE bytes (256 GraphicsTexturePaletteEntry) each, starting
     right after the GFX_ASSET_HEADER_SIZE-byte asset header */
  paletteEntryCursor = (const GraphicsTexturePaletteEntry *)
                       ((uint8_t *)sourceAsset + GFX_ASSET_HEADER_SIZE + paletteIndex * GFX_PALETTE_BANK_SIZE);
  for (paletteEntriesRemaining = 256; paletteEntriesRemaining != 0; paletteEntriesRemaining--) {
    if (paletteEntryCursor->argb8888 < ARGB8888_ALPHA_MASK) { /* alpha below 0xFF */
      return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedAlphaTextureFormat,0);
    }
    paletteEntryCursor++;
  }
  return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedOpaqueTextureFormat,0);
}

/* Address: 0x0057A740.
   Creates the system-memory staging copy of a texture (square, at least 16x16, the downsample shift adjusted
   to match) with its IDirectDrawSurface3 and IDirect3DTexture2 interfaces, clears the device-side fields
   (GraphicsTexture_CreateDeviceTexture fills them on first use) and converts the image into it with the
   colour upload for that shift. Only Direct3D devices get staging surfaces; for the software and Glide
   adapters the fields are just cleared before the upload. When the creation fails all staging fields are
   NULL and nothing is uploaded. Returns texture (EAX preserved).
*/
GraphicsTextureResource * GraphicsTexture_CreateStagingTexture(GraphicsTextureResource *texture)

{
  uint32_t largerExtent;
  TH_LEGACY_HRESULT hresult;
  GraphicsTextureDownsampleShift effectiveShift;
  int dwordsRemaining;
  DDPIXELFORMAT *sourceFormatCursor;
  DDPIXELFORMAT *destinationFormatCursor;
  GraphicsTextureLogicalSize logicalSize;
  IDirect3DTexture2 *texture2;
  IDirectDrawSurface3 *surface3;
  IDirectDrawSurface *surfaceBase;
  
  surfaceBase = NULL;
  surface3 = NULL;
  texture2 = NULL;
  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
  if (1 < g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1) {
    g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
    logicalSize = g_GraphicsTextureSourceGetLogicalSize(texture->subresourceIndex,texture->sourceAsset)
    ;
    largerExtent = logicalSize.logicalWidthPixels;
    if (logicalSize.logicalWidthPixels < logicalSize.logicalHeightPixels) {
      largerExtent = logicalSize.logicalHeightPixels;
    }
    g_SurfaceDesc.dwHeight = largerExtent >> ((uint8_t)g_TextureDownsampleShift & SHIFT_COUNT_MASK);
    effectiveShift = g_TextureDownsampleShift;
    /* textures are at least 16x16: lower the downsample shift until the edge reaches 16 */
    do {
      if (15 < (int)g_SurfaceDesc.dwHeight) break;
      g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
      effectiveShift = effectiveShift - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
    } while (effectiveShift != GRAPHICS_TEXTURE_DOWNSAMPLE_1X);
    texture->downsampleShift = effectiveShift;
    sourceFormatCursor = texture->pixelFormat;
    destinationFormatCursor = &g_SurfaceDesc.ddpfPixelFormat;
    g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
    /* copy the 32-byte DDPIXELFORMAT dword by dword */
    for (dwordsRemaining = 8; dwordsRemaining != 0; dwordsRemaining--) {
      destinationFormatCursor->dwSize = sourceFormatCursor->dwSize;
      sourceFormatCursor = (DDPIXELFORMAT *)&sourceFormatCursor->dwFlags;
      destinationFormatCursor = (DDPIXELFORMAT *)&destinationFormatCursor->dwFlags;
    }
    hresult = g_DirectDraw2->lpVtbl->CreateSurface
                      (g_DirectDraw2,&g_SurfaceDesc,&surfaceBase,NULL);
    if (hresult == 0) {
      hresult = surfaceBase->lpVtbl->QueryInterface
                        (surfaceBase,&IID_IDirectDrawSurface3_Local,&surface3);
      if (hresult == 0) {
        hresult = surface3->lpVtbl->QueryInterface(surface3,&IID_IDirect3DTexture2_Local,&texture2)
        ;
      }
    }
    if (hresult != 0) {
      if (texture2 != NULL) {
        texture2->lpVtbl->Release(texture2);
      }
      if (surface3 != NULL) {
        surface3->lpVtbl->Release(surface3);
      }
      if (surfaceBase != NULL) {
        surfaceBase->lpVtbl->Release(surfaceBase);
      }
      texture->stagingSurfaceBase = NULL;
      texture->stagingSurface3 = NULL;
      texture->stagingTexture2 = NULL;
      return texture; /* preserved EAX: callers mirror texture in EAX */
    }
  }
  texture->stagingSurfaceBase = surfaceBase;
  texture->stagingSurface3 = surface3;
  texture->stagingTexture2 = texture2;
  texture->deviceSurfaceBase = NULL;
  texture->deviceSurface3 = NULL;
  texture->deviceTexture2 = NULL;
  texture->textureHandle = 0;
  g_GraphicsDispatchTable.colorUpload[texture->downsampleShift](texture);
  return texture; /* preserved EAX: callers mirror texture in EAX */
}


/* Address: 0x0057A900.
   Releases every DirectDraw/Direct3D object of a texture (device and staging) and clears the fields. When the
   texture was the one bound to the device, g_BoundTextureHandle becomes 0xFFFFFFFF, which matches no handle,
   so the next bind sets the render state again. Preserves EAX; callers keep texture in it.
*/
void GraphicsTexture_ReleaseObjects(GraphicsTextureResource *texture)

{
  IDirect3DTexture2 *deviceTexture2;
  IDirect3DTexture2 *stagingTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface3 *stagingSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  IDirectDrawSurface *stagingSurfaceBase;
  uint32_t releasedTextureHandle;
  
  deviceTexture2 = texture->deviceTexture2;
  if (deviceTexture2 != NULL) {
    deviceTexture2->lpVtbl->Release(deviceTexture2);
  }
  deviceSurface3 = texture->deviceSurface3;
  if (deviceSurface3 != NULL) {
    deviceSurface3->lpVtbl->Release(deviceSurface3);
  }
  deviceSurfaceBase = texture->deviceSurfaceBase;
  if (deviceSurfaceBase != NULL) {
    deviceSurfaceBase->lpVtbl->Release(deviceSurfaceBase);
  }
  stagingTexture2 = texture->stagingTexture2;
  if (stagingTexture2 != NULL) {
    stagingTexture2->lpVtbl->Release(stagingTexture2);
  }
  stagingSurface3 = texture->stagingSurface3;
  if (stagingSurface3 != NULL) {
    stagingSurface3->lpVtbl->Release(stagingSurface3);
  }
  stagingSurfaceBase = texture->stagingSurfaceBase;
  if (stagingSurfaceBase != NULL) {
    stagingSurfaceBase->lpVtbl->Release(stagingSurfaceBase);
  }
  releasedTextureHandle = texture->textureHandle;
  texture->stagingSurfaceBase = NULL;
  texture->stagingSurface3 = NULL;
  texture->stagingTexture2 = NULL;
  texture->deviceSurfaceBase = NULL;
  texture->deviceSurface3 = NULL;
  texture->deviceTexture2 = NULL;
  texture->textureHandle = 0;
  if (releasedTextureHandle == g_BoundTextureHandle) {
    g_BoundTextureHandle = UINT32_MAX;
  }
  return;
}

