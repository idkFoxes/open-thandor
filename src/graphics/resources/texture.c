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
   query point into the stored pixels of the subresource and sets CF when that pixel has a non-zero alpha,
   for direct ARGB and paletted subresources alike. CF is clear for transparent pixels, points outside the
   stored pixels and invalid input.
*/
bool GraphicsTextureSource_TestOpaquePixel(GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  uint8_t *recordField;
  AssetRelativeOffset tableOffset;
  int paletteIndex;
  int localXOrPixelIndex;
  int recordOffset;
  int localY;

  if ((drawX <= queryX) && (drawY <= queryY)) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      tableOffset = (sourceAsset->tableDescriptor).subresourceTableOffset;
      recordOffset = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE;
      recordField = (uint8_t *)&((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->originX;
      localXOrPixelIndex = (queryX - drawX) - *(int *)recordField;
      if ((((*(int *)recordField <= queryX - drawX) &&
           (recordField = (uint8_t *)&((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->originY,
           localY = (queryY - drawY) - *(int *)recordField,
           *(int *)recordField <= queryY - drawY)) &&
          (localXOrPixelIndex < (int)((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->pixelWidth)) &&
         (localY < (int)((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->pixelHeight)) {
        paletteIndex = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->paletteIndex;
        localXOrPixelIndex = localY * ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->pixelWidth +
                             localXOrPixelIndex;
        /* opaque = any alpha bit set in the ARGB8888 pixel (direct) or palette entry (paletted, 8 bytes each in
           the 256-entry bank at asset + 0x200 + paletteIndex * 0x800) */
        if (paletteIndex == -1) {
          if (ARGB8888_RGB_MASK <
              *(uint32_t *)(localXOrPixelIndex * 4 +
                        (int)((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->dataOffset +
                        (int)sourceAsset)) {
            return true;
          }
        }
        else if (ARGB8888_RGB_MASK <
                 ((GraphicsPaletteTextureSourceAsset *)sourceAsset)->paletteEntries[paletteIndex * GRAPHICS_PALETTE_BANK_ENTRIES + (uint32_t)*(uint8_t *)(localXOrPixelIndex + (int)((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->dataOffset + (int)sourceAsset)].argb8888) {
          return true;
        }
      }
    }
  }
  return false;
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
  uint32_t allocationSizeOrCount;
  GraphicsPaletteTextureSourceAsset *cloneCursor;
  uint32_t cloneAllocationError;

  allocationSizeOrCount = (sourceAsset->common).allocationSizeBytes;
  cloneAllocationError = g_MemoryApi.alloc(allocationSizeOrCount,(void **)&clonedAsset);
  if (cloneAllocationError != 0) {
    clonedAsset = (GraphicsPaletteTextureSourceAsset *)cloneAllocationError;
  }
  else {
    /* REP MOVSD of the whole allocation */
    cloneCursor = clonedAsset;
    for (allocationSizeOrCount = allocationSizeOrCount >> 2; allocationSizeOrCount != 0; allocationSizeOrCount--) {
      cloneCursor->magic = (sourceAsset->common).magic;
      sourceAsset = (GraphicsTextureSourceAsset *)&(sourceAsset->common).allocationSizeBytes;
      cloneCursor = (GraphicsPaletteTextureSourceAsset *)&cloneCursor->allocationSizeBytes;
    }
    if (g_GraphicsTextureSourceConvertPaletteEntries(clonedAsset) == 0) {
      return (GraphicsTextureSourceAsset *)clonedAsset;
    }
    /* Original quirk: the clone's result after a failed conversion is the free's status (0 = NULL) */
    clonedAsset = (GraphicsPaletteTextureSourceAsset *)g_MemoryApi.free(clonedAsset);
  }
  return (GraphicsTextureSourceAsset *)clonedAsset;
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


/* Address: 0x004AD7C0.
   Stores the logical width and height of the first subresource of a 'gfx' texture source whose subresource
   count is 1..4095 and returns true; returns false (outputs untouched) otherwise. No caller or table slot
   referencing it is known.
*/
bool GraphicsTextureSource_GetFirstLogicalSize
          (GraphicsTextureSourceAsset *sourceAsset,uint32_t *outWidthPixels,uint32_t *outHeightPixels)

{
  uint32_t entryCount;
  uint8_t *firstSubresourceRecord;

  if ((sourceAsset->common).magic == ASSET_MAGIC_GFX) {
    entryCount = (sourceAsset->tableDescriptor).subresourceCount;
    if ((entryCount != 0) && (entryCount <= 4095)) {
      firstSubresourceRecord =
           (uint8_t *)sourceAsset + (sourceAsset->tableDescriptor).subresourceTableOffset;
      *outWidthPixels = ((GraphicsTextureSourceEntry *)firstSubresourceRecord)->logicalWidth;
      *outHeightPixels = ((GraphicsTextureSourceEntry *)firstSubresourceRecord)->logicalHeight;
      return true;
    }
  }
  return false;
}


/* Address: 0x0057ADA0.
   Copies the pixels of a texture's subresource into its staging surface at full size, converted to the
   surface's pixel format (g_GraphicsDispatchTable.colorUpload[0], used for downsample shift 0 by
   GraphicsTexture_CreateStagingTexture and GraphicsTextureSet_RefreshColor). Direct ARGB8888 and paletted
   sources are handled; for RGB surfaces each channel is shifted into the position of its mask (16-bit or
   32-bit stores); an 8-bit surface receives the palette indices, or the alpha byte of direct pixels, and a
   matching palette (a grey ramp for direct pixels) is built. A lost surface is restored first;
   g_ActiveTextureUploads is raised for the duration.
*/
void GraphicsTexture_UploadColor_1x(GraphicsTextureResource *texture)

{
  int offsetOrGreenTopBit;
  DDPIXELFORMAT *destinationFormat;
  uint32_t maskOrArgb;
  uint32_t greenMask;
  uint32_t blueMask;
  int blueTopBit;
  uint8_t paletteGreen;
  uint8_t paletteBlue;
  uint8_t paletteFlags;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int paletteIndexOrCounter;
  DirectDrawPaletteEntry grayPaletteEntry;
  uint8_t *sourcePixel;
  GraphicsTexturePaletteEntry *paletteSourceCursor;
  uint16_t *destinationWord;
  uint8_t *byteCursorOrSourceRow;
  DirectDrawPaletteEntry *paletteEntryCursor;
  uint32_t *destinationDword;
  uint8_t *sourceByteRow;
  uint16_t *destinationWordRow;
  uint8_t *destinationByteRow;
  uint32_t *destinationDwordRow;
  IDirectDrawPalette *createdPalette;
  GraphicsTextureSourceAsset *paletteBank;
  int rowsRemaining;
  int sourceWidth;
  int alphaShiftRight;
  int blueShiftRight;
  int greenShiftRight;
  int redShiftRight;
  int alphaShiftLeft;
  int blueShiftLeft;
  int greenShiftLeft;
  int redShiftLeft;
  IDirectDrawSurface3 *stagingSurface3;
  uint32_t subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    hresult = stagingSurface3->lpVtbl->IsLost(stagingSurface3);
    paletteIndexOrCounter = 0;
    if (hresult != 0) {
      paletteIndexOrCounter = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (paletteIndexOrCounter == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      hresult = stagingSurface3->lpVtbl->Lock
                         (stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,
                          NULL);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrGreenTopBit = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                              (sourceAsset->tableDescriptor).subresourceTableOffset;
        paletteIndexOrCounter = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrGreenTopBit))->paletteIndex;
        sourceWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrGreenTopBit))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrGreenTopBit))->pixelHeight;
        offsetOrGreenTopBit = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrGreenTopBit))->dataOffset;
        if (paletteIndexOrCounter < 0) {
          sourcePixel = (uint8_t *)sourceAsset + offsetOrGreenTopBit
          ;
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            paletteIndexOrCounter = sourceWidth;
            byteCursorOrSourceRow = g_SurfaceDesc.lpSurface;
            sourceByteRow = sourcePixel;
            destinationByteRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursorOrSourceRow = sourcePixel[3];
                  sourcePixel = sourcePixel + 4;
                  paletteIndexOrCounter--;
                  byteCursorOrSourceRow++;
                } while (paletteIndexOrCounter != 0);
                sourcePixel = sourceByteRow + sourceWidth * 4;
                rowsRemaining--;
                paletteIndexOrCounter = sourceWidth;
                byteCursorOrSourceRow = destinationByteRow + destinationPitch;
                sourceByteRow = sourcePixel;
                destinationByteRow = destinationByteRow + destinationPitch;
              } while (rowsRemaining != 0);
              stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 256;
              grayPaletteEntry.red = 0;
              grayPaletteEntry.green = 0;
              grayPaletteEntry.blue = 0;
              grayPaletteEntry.flags = 0;
              /* grey ramp: all four bytes step by one per entry */
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                *paletteEntryCursor = grayPaletteEntry;
                paletteEntryCursor++;
                /* bytes never exceed 255 before the last (unused) step, so no carry crosses bytes */
                grayPaletteEntry.red++;
                grayPaletteEntry.green++;
                grayPaletteEntry.blue++;
                grayPaletteEntry.flags++;
                paletteIndexOrCounter--;
              } while (paletteIndexOrCounter != 0);
              /* as in the original, the palette is released at once and never attached (no SetPalette) */
              hresult = g_DirectDraw2->lpVtbl->CreatePalette
                                 (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,
                                  NULL);
              if (hresult == 0) {
                createdPalette->lpVtbl->Release(createdPalette);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
            maskOrArgb = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((maskOrArgb != 0) && (greenMask != 0)) && (blueMask != 0)) {
              /* per channel: shift the ARGB byte right to the mask's width, then left to its lowest bit */
              redShiftLeft = 0;
              if (maskOrArgb != 0) {
                for (; (maskOrArgb >> redShiftLeft & 1) == 0; redShiftLeft++) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft++) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft++) {
                }
              }
              paletteIndexOrCounter = 31;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                }
              }
              offsetOrGreenTopBit = 31;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit--) {
                }
              }
              blueTopBit = 31;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit--) {
                }
              }
              redShiftRight = 24 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 16 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              maskOrArgb = destinationFormat->dwRGBAlphaBitMask;
              if (maskOrArgb == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 16;
              }
              else {
                alphaShiftLeft = 0;
                if (maskOrArgb != 0) {
                  for (; (maskOrArgb >> alphaShiftLeft & 1) == 0; alphaShiftLeft++) {
                  }
                }
                paletteIndexOrCounter = 31;
                if (maskOrArgb != 0) {
                  for (; maskOrArgb >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                  }
                }
                alphaShiftRight = 32 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrSourceRow = sourcePixel;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 17) {
                do {
                  do {
                    maskOrArgb = *(uint32_t *)sourcePixel;
                    *destinationWord = (uint16_t)(((maskOrArgb & ARGB8888_ALPHA_MASK) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((maskOrArgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((maskOrArgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((maskOrArgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK));
                    sourcePixel = sourcePixel + 4;
                    paletteIndexOrCounter--;
                    destinationWord++;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth * 4;
                  rowsRemaining--;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationWordRow = (uint16_t *)((int)destinationWordRow + destinationPitch);
                } while (rowsRemaining != 0);
                rowsRemaining = 0;
              }
              else {
                do {
                  do {
                    maskOrArgb = *(uint32_t *)sourcePixel;
                    *destinationDword = ((maskOrArgb & ARGB8888_ALPHA_MASK) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK) |
                               ((maskOrArgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK) |
                               ((maskOrArgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK) |
                               ((maskOrArgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK);
                    sourcePixel = sourcePixel + 4;
                    paletteIndexOrCounter--;
                    destinationDword++;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth * 4;
                  rowsRemaining--;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationDwordRow = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                } while (rowsRemaining != 0);
                rowsRemaining = 0;
              }
            }
          }
        }
        else {
          sourcePixel = (uint8_t *)sourceAsset + offsetOrGreenTopBit
          ;
          paletteBank = sourceAsset + paletteIndexOrCounter * 4 + 1;
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            paletteIndexOrCounter = sourceWidth;
            byteCursorOrSourceRow = g_SurfaceDesc.lpSurface;
            sourceByteRow = sourcePixel;
            destinationByteRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursorOrSourceRow = *sourcePixel;
                  sourcePixel++;
                  paletteIndexOrCounter--;
                  byteCursorOrSourceRow++;
                } while (paletteIndexOrCounter != 0);
                sourcePixel = sourceByteRow + sourceWidth;
                rowsRemaining--;
                paletteIndexOrCounter = sourceWidth;
                byteCursorOrSourceRow = destinationByteRow + destinationPitch;
                sourceByteRow = sourcePixel;
                destinationByteRow = destinationByteRow + destinationPitch;
              } while (rowsRemaining != 0);
              stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 256;
              /* the four bytes of each entry's argb8888 word, copied in memory order */
              paletteSourceCursor = (GraphicsTexturePaletteEntry *)paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = ((uint8_t *)&paletteSourceCursor->argb8888)[1];
                paletteBlue = ((uint8_t *)&paletteSourceCursor->argb8888)[2];
                paletteFlags = ((uint8_t *)&paletteSourceCursor->argb8888)[3];
                paletteEntryCursor->red = ((uint8_t *)&paletteSourceCursor->argb8888)[0];
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor++;
                paletteEntryCursor++;
                paletteIndexOrCounter--;
              } while (paletteIndexOrCounter != 0);
              /* as in the original, the palette is released at once and never attached (no SetPalette) */
              hresult = g_DirectDraw2->lpVtbl->CreatePalette
                                 (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,
                                  NULL);
              if (hresult == 0) {
                createdPalette->lpVtbl->Release(createdPalette);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
            maskOrArgb = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((maskOrArgb != 0) && (greenMask != 0)) && (blueMask != 0)) {
              /* per channel: shift the ARGB byte right to the mask's width, then left to its lowest bit */
              redShiftLeft = 0;
              if (maskOrArgb != 0) {
                for (; (maskOrArgb >> redShiftLeft & 1) == 0; redShiftLeft++) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft++) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft++) {
                }
              }
              paletteIndexOrCounter = 31;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                }
              }
              offsetOrGreenTopBit = 31;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit--) {
                }
              }
              blueTopBit = 31;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit--) {
                }
              }
              redShiftRight = 24 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 16 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              maskOrArgb = destinationFormat->dwRGBAlphaBitMask;
              if (maskOrArgb == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 16;
              }
              else {
                alphaShiftLeft = 0;
                if (maskOrArgb != 0) {
                  for (; (maskOrArgb >> alphaShiftLeft & 1) == 0; alphaShiftLeft++) {
                  }
                }
                paletteIndexOrCounter = 31;
                if (maskOrArgb != 0) {
                  for (; maskOrArgb >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                  }
                }
                alphaShiftRight = 32 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrSourceRow = sourcePixel;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 17) {
                do {
                  do {
                    maskOrArgb = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)*sourcePixel].argb8888;
                    *destinationWord = (uint16_t)(((maskOrArgb & ARGB8888_ALPHA_MASK) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((maskOrArgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((maskOrArgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((maskOrArgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK));
                    sourcePixel++;
                    paletteIndexOrCounter--;
                    destinationWord++;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth;
                  rowsRemaining--;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationWordRow = (uint16_t *)((int)destinationWordRow + destinationPitch);
                } while (rowsRemaining != 0);
                stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              }
              else {
                do {
                  do {
                    maskOrArgb = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)*sourcePixel].argb8888;
                    *destinationDword = ((maskOrArgb & ARGB8888_ALPHA_MASK) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK) |
                               ((maskOrArgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK) |
                               ((maskOrArgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK) |
                               ((maskOrArgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK);
                    sourcePixel++;
                    paletteIndexOrCounter--;
                    destinationDword++;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth;
                  rowsRemaining--;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationDwordRow = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                } while (rowsRemaining != 0);
                stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
          }
        }
        stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      }
    }
  }
GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads--;
  return;
}


/* Address: 0x0057B410.
   GraphicsTexture_UploadColor_1x at half size (g_GraphicsDispatchTable.colorUpload[1]): every destination
   pixel is the average of a 2x2 source block (8-bit surfaces take the top-left source pixel).
*/
void GraphicsTexture_UploadColor_2x(GraphicsTextureResource *texture)

{
  DDPIXELFORMAT *destinationFormat;
  uint32_t redOrAlphaMask;
  uint32_t greenMask;
  uint32_t blueMask;
  uint32_t topLeftTexel;
  uint32_t topRightTexel;
  uint32_t bottomLeftTexel;
  uint32_t bottomRightTexel;
  bool hasMore;
  int blueTopBit;
  uint8_t paletteGreen;
  uint8_t paletteBlue;
  uint8_t paletteFlags;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int paletteIndexOrCounter;
  DirectDrawPaletteEntry grayPaletteEntry;
  int offsetOrRemaining;
  uint8_t *byteCursor;
  GraphicsTexturePaletteEntry *paletteSourceCursor;
  AssetProducerSourceNames *sourceTexel;
  uint8_t *byteCursorOrRow;
  uint16_t *destinationWord;
  DirectDrawPaletteEntry *paletteEntryCursor;
  uint32_t *destinationDword;
  uint16_t averageBlue;
  uint32_t averageRgb;
  uint16_t averageGreen;
  uint16_t averageRed;
  uint16_t averageAlpha;
  uint8_t *sourceByteRow;
  AssetProducerSourceNames *sourceTexelRow;
  uint8_t *destinationByteRow;
  uint16_t *destinationWordRow;
  uint32_t *destinationDwordRow;
  IDirectDrawPalette *createdPalette;
  GraphicsTextureSourceAsset *paletteBank;
  int rowsRemaining;
  int sourceWidth;
  int alphaShiftRight;
  int blueShiftRight;
  int greenShiftRight;
  int redShiftRight;
  int alphaShiftLeft;
  int blueShiftLeft;
  int greenShiftLeft;
  int redShiftLeft;
  TH_LEGACY_LONG savedPitch; /* dead stores, but they steer MSVC's register allocation (removing them changes the code) */
  TH_LEGACY_LPVOID savedSurfaceBits;
  IDirectDrawSurface3 *stagingSurface3;
  uint32_t subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;

  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    hresult = stagingSurface3->lpVtbl->IsLost(stagingSurface3);
    paletteIndexOrCounter = 0;
    if (hresult != 0) {
      paletteIndexOrCounter = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (paletteIndexOrCounter == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      hresult = stagingSurface3->lpVtbl->Lock
                         (stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,
                          NULL);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                            (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->paletteIndex;
        sourceWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->pixelHeight;
        offsetOrRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->dataOffset;
        if (paletteIndexOrCounter < 0) {
          sourceTexel = (AssetProducerSourceNames *)
                    GFX_ANCHORED_ASSET_BYTES(sourceAsset,offsetOrRemaining);
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            paletteIndexOrCounter = sourceWidth;
            byteCursor = g_SurfaceDesc.lpSurface;
            sourceTexelRow = sourceTexel;
            byteCursorOrRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursor = *(uint8_t *)((int)sourceTexel->producerName + 3);
                  sourceTexel = (AssetProducerSourceNames *)((int)sourceTexel->producerName + 8);
                  offsetOrRemaining = paletteIndexOrCounter - 2;
                  hasMore = 1 < paletteIndexOrCounter;
                  paletteIndexOrCounter = offsetOrRemaining;
                  byteCursor++;
                } while (offsetOrRemaining != 0 && hasMore);
                sourceTexel = (AssetProducerSourceNames *)((int)sourceTexelRow->producerName + sourceWidth * 8);
                offsetOrRemaining = rowsRemaining - 2;
                hasMore = 1 < rowsRemaining;
                paletteIndexOrCounter = sourceWidth;
                byteCursor = byteCursorOrRow + destinationPitch;
                sourceTexelRow = sourceTexel;
                byteCursorOrRow = byteCursorOrRow + destinationPitch;
                rowsRemaining = offsetOrRemaining;
              } while (offsetOrRemaining != 0 && hasMore);
              stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 256;
              grayPaletteEntry.red = 0;
              grayPaletteEntry.green = 0;
              grayPaletteEntry.blue = 0;
              grayPaletteEntry.flags = 0;
              /* grey ramp: all four bytes step by one per entry */
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                *paletteEntryCursor = grayPaletteEntry;
                paletteEntryCursor++;
                /* bytes never exceed 255 before the last (unused) step, so no carry crosses bytes */
                grayPaletteEntry.red++;
                grayPaletteEntry.green++;
                grayPaletteEntry.blue++;
                grayPaletteEntry.flags++;
                paletteIndexOrCounter--;
              } while (paletteIndexOrCounter != 0);
              /* as in the original, the palette is released at once and never attached (no SetPalette) */
              hresult = g_DirectDraw2->lpVtbl->CreatePalette
                                 (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,
                                  NULL);
              if (hresult == 0) {
                createdPalette->lpVtbl->Release(createdPalette);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
            redOrAlphaMask = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((redOrAlphaMask != 0) && (greenMask != 0)) && (blueMask != 0)) {
              /* per channel: shift the ARGB byte right to the mask's width, then left to its lowest bit */
              redShiftLeft = 0;
              if (redOrAlphaMask != 0) {
                for (; (redOrAlphaMask >> redShiftLeft & 1) == 0; redShiftLeft++) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft++) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft++) {
                }
              }
              paletteIndexOrCounter = 31;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                }
              }
              offsetOrRemaining = 31;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrRemaining == 0; offsetOrRemaining--) {
                }
              }
              blueTopBit = 31;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit--) {
                }
              }
              redShiftRight = 24 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 16 - ((offsetOrRemaining + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              redOrAlphaMask = destinationFormat->dwRGBAlphaBitMask;
              if (redOrAlphaMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 16;
              }
              else {
                alphaShiftLeft = 0;
                if (redOrAlphaMask != 0) {
                  for (; (redOrAlphaMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft++) {
                  }
                }
                paletteIndexOrCounter = 31;
                if (redOrAlphaMask != 0) {
                  for (; redOrAlphaMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                  }
                }
                alphaShiftRight = 32 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 17) {
                do {
                  do {
                    topLeftTexel = *(uint32_t *)sourceTexel->producerName;
                    topRightTexel = *(uint32_t *)(sourceTexel->producerName + 2);
                    bottomLeftTexel = *(uint32_t *)(sourceTexel->producerName + sourceWidth * 2);
                    bottomRightTexel = *(uint32_t *)(sourceTexel->producerName + sourceWidth * 2 + 2);
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(topLeftTexel) + TEXTURE_TEXEL_BLUE(topRightTexel) +
                                          TEXTURE_TEXEL_BLUE(bottomLeftTexel) + TEXTURE_TEXEL_BLUE(bottomRightTexel)) >> 2;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(topLeftTexel) + TEXTURE_TEXEL_GREEN(topRightTexel) +
                                           TEXTURE_TEXEL_GREEN(bottomLeftTexel) + TEXTURE_TEXEL_GREEN(bottomRightTexel)) >> 2;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(topLeftTexel) + TEXTURE_TEXEL_RED(topRightTexel) +
                                         TEXTURE_TEXEL_RED(bottomLeftTexel) + TEXTURE_TEXEL_RED(bottomRightTexel)) >> 2;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(topLeftTexel) + TEXTURE_TEXEL_ALPHA(topRightTexel) +
                                           TEXTURE_TEXEL_ALPHA(bottomLeftTexel) + TEXTURE_TEXEL_ALPHA(bottomRightTexel)) >> 2;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK));
                    sourceTexel = (AssetProducerSourceNames *)(sourceTexel->producerName + 4);
                    offsetOrRemaining = paletteIndexOrCounter - 2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationWord++;
                  } while (offsetOrRemaining != 0 && hasMore);
                  sourceTexel = (AssetProducerSourceNames *)(sourceTexelRow->producerName + sourceWidth * 4);
                  offsetOrRemaining = rowsRemaining - 2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationWordRow = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
              }
              else {
                do {
                  do {
                    topLeftTexel = *(uint32_t *)sourceTexel->producerName;
                    topRightTexel = *(uint32_t *)(sourceTexel->producerName + 2);
                    bottomLeftTexel = *(uint32_t *)(sourceTexel->producerName + sourceWidth * 2);
                    bottomRightTexel = *(uint32_t *)(sourceTexel->producerName + sourceWidth * 2 + 2);
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(topLeftTexel) + TEXTURE_TEXEL_BLUE(topRightTexel) +
                                          TEXTURE_TEXEL_BLUE(bottomLeftTexel) + TEXTURE_TEXEL_BLUE(bottomRightTexel)) >> 2;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(topLeftTexel) + TEXTURE_TEXEL_GREEN(topRightTexel) +
                                           TEXTURE_TEXEL_GREEN(bottomLeftTexel) + TEXTURE_TEXEL_GREEN(bottomRightTexel)) >> 2;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(topLeftTexel) + TEXTURE_TEXEL_RED(topRightTexel) +
                                         TEXTURE_TEXEL_RED(bottomLeftTexel) + TEXTURE_TEXEL_RED(bottomRightTexel)) >> 2;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(topLeftTexel) + TEXTURE_TEXEL_ALPHA(topRightTexel) +
                                           TEXTURE_TEXEL_ALPHA(bottomLeftTexel) + TEXTURE_TEXEL_ALPHA(bottomRightTexel)) >> 2;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK))
                               << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK);
                    sourceTexel = (AssetProducerSourceNames *)(sourceTexel->producerName + 4);
                    offsetOrRemaining = paletteIndexOrCounter - 2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationDword++;
                  } while (offsetOrRemaining != 0 && hasMore);
                  sourceTexel = (AssetProducerSourceNames *)(sourceTexelRow->producerName + sourceWidth * 4);
                  offsetOrRemaining = rowsRemaining - 2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationDwordRow = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
              }
            }
          }
        }
        else {
          byteCursor = (uint8_t *)sourceAsset + offsetOrRemaining;
          paletteBank = sourceAsset + paletteIndexOrCounter * 4 + 1;
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            paletteIndexOrCounter = sourceWidth;
            byteCursorOrRow = g_SurfaceDesc.lpSurface;
            sourceByteRow = byteCursor;
            destinationByteRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursorOrRow = *byteCursor;
                  byteCursor = byteCursor + 2;
                  offsetOrRemaining = paletteIndexOrCounter - 2;
                  hasMore = 1 < paletteIndexOrCounter;
                  paletteIndexOrCounter = offsetOrRemaining;
                  byteCursorOrRow++;
                } while (offsetOrRemaining != 0 && hasMore);
                byteCursor = sourceByteRow + sourceWidth * 2;
                offsetOrRemaining = rowsRemaining - 2;
                hasMore = 1 < rowsRemaining;
                paletteIndexOrCounter = sourceWidth;
                byteCursorOrRow = destinationByteRow + destinationPitch;
                sourceByteRow = byteCursor;
                destinationByteRow = destinationByteRow + destinationPitch;
                rowsRemaining = offsetOrRemaining;
              } while (offsetOrRemaining != 0 && hasMore);
              stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 256;
              /* the four bytes of each entry's argb8888 word, copied in memory order */
              paletteSourceCursor = (GraphicsTexturePaletteEntry *)paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = ((uint8_t *)&paletteSourceCursor->argb8888)[1];
                paletteBlue = ((uint8_t *)&paletteSourceCursor->argb8888)[2];
                paletteFlags = ((uint8_t *)&paletteSourceCursor->argb8888)[3];
                paletteEntryCursor->red = ((uint8_t *)&paletteSourceCursor->argb8888)[0];
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor++;
                paletteEntryCursor++;
                paletteIndexOrCounter--;
              } while (paletteIndexOrCounter != 0);
              /* as in the original, the palette is released at once and never attached (no SetPalette) */
              hresult = g_DirectDraw2->lpVtbl->CreatePalette
                                 (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,
                                  NULL);
              if (hresult == 0) {
                createdPalette->lpVtbl->Release(createdPalette);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
            redOrAlphaMask = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((redOrAlphaMask != 0) && (greenMask != 0)) && (blueMask != 0)) {
              /* per channel: shift the ARGB byte right to the mask's width, then left to its lowest bit */
              redShiftLeft = 0;
              if (redOrAlphaMask != 0) {
                for (; (redOrAlphaMask >> redShiftLeft & 1) == 0; redShiftLeft++) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft++) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft++) {
                }
              }
              paletteIndexOrCounter = 31;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                }
              }
              offsetOrRemaining = 31;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrRemaining == 0; offsetOrRemaining--) {
                }
              }
              blueTopBit = 31;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit--) {
                }
              }
              redShiftRight = 24 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 16 - ((offsetOrRemaining + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              redOrAlphaMask = destinationFormat->dwRGBAlphaBitMask;
              if (redOrAlphaMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 16;
              }
              else {
                alphaShiftLeft = 0;
                if (redOrAlphaMask != 0) {
                  for (; (redOrAlphaMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft++) {
                  }
                }
                paletteIndexOrCounter = 31;
                if (redOrAlphaMask != 0) {
                  for (; redOrAlphaMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                  }
                }
                alphaShiftRight = 32 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrRow = byteCursor;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 17) {
                do {
                  do {
                    topLeftTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)*byteCursor].argb8888;
                    topRightTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[1]].argb8888;
                    bottomLeftTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth]].argb8888;
                    bottomRightTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth + 1]].argb8888;
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(topLeftTexel) + TEXTURE_TEXEL_BLUE(topRightTexel) +
                                          TEXTURE_TEXEL_BLUE(bottomLeftTexel) + TEXTURE_TEXEL_BLUE(bottomRightTexel)) >> 2;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(topLeftTexel) + TEXTURE_TEXEL_GREEN(topRightTexel) +
                                           TEXTURE_TEXEL_GREEN(bottomLeftTexel) + TEXTURE_TEXEL_GREEN(bottomRightTexel)) >> 2;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(topLeftTexel) + TEXTURE_TEXEL_RED(topRightTexel) +
                                         TEXTURE_TEXEL_RED(bottomLeftTexel) + TEXTURE_TEXEL_RED(bottomRightTexel)) >> 2;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(topLeftTexel) + TEXTURE_TEXEL_ALPHA(topRightTexel) +
                                           TEXTURE_TEXEL_ALPHA(bottomLeftTexel) + TEXTURE_TEXEL_ALPHA(bottomRightTexel)) >> 2;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK));
                    byteCursor = byteCursor + 2;
                    offsetOrRemaining = paletteIndexOrCounter - 2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationWord++;
                  } while (offsetOrRemaining != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 2;
                  offsetOrRemaining = rowsRemaining - 2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationWordRow = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
                stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              }
              else {
                do {
                  do {
                    topLeftTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)*byteCursor].argb8888;
                    topRightTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[1]].argb8888;
                    bottomLeftTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth]].argb8888;
                    bottomRightTexel = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth + 1]].argb8888;
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(topLeftTexel) + TEXTURE_TEXEL_BLUE(topRightTexel) +
                                          TEXTURE_TEXEL_BLUE(bottomLeftTexel) + TEXTURE_TEXEL_BLUE(bottomRightTexel)) >> 2;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(topLeftTexel) + TEXTURE_TEXEL_GREEN(topRightTexel) +
                                           TEXTURE_TEXEL_GREEN(bottomLeftTexel) + TEXTURE_TEXEL_GREEN(bottomRightTexel)) >> 2;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(topLeftTexel) + TEXTURE_TEXEL_RED(topRightTexel) +
                                         TEXTURE_TEXEL_RED(bottomLeftTexel) + TEXTURE_TEXEL_RED(bottomRightTexel)) >> 2;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(topLeftTexel) + TEXTURE_TEXEL_ALPHA(topRightTexel) +
                                           TEXTURE_TEXEL_ALPHA(bottomLeftTexel) + TEXTURE_TEXEL_ALPHA(bottomRightTexel)) >> 2;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK))
                               << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK);
                    byteCursor = byteCursor + 2;
                    offsetOrRemaining = paletteIndexOrCounter - 2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationDword++;
                  } while (offsetOrRemaining != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 2;
                  offsetOrRemaining = rowsRemaining - 2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationDwordRow = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
                stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
          }
        }
        stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      }
    }
  }
GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads--;
  return;
}


/* Address: 0x0057BBE0.
   GraphicsTexture_UploadColor_1x at quarter size (g_GraphicsDispatchTable.colorUpload[2]): every destination
   pixel is the average of a 4x4 source block (8-bit surfaces take the top-left source pixel).
*/
void GraphicsTexture_UploadColor_4x(GraphicsTextureResource *texture)

{
  int offsetOrGreenTopBit;
  DDPIXELFORMAT *destinationFormat;
  uint32_t blueMask;
  uint32_t texel00;
  uint32_t texel01;
  uint32_t texel02;
  uint32_t texel03;
  uint32_t texel04;
  uint32_t texel05;
  uint32_t texel06;
  uint32_t texel07;
  uint32_t texel08;
  uint32_t texel09;
  uint32_t texel10;
  uint32_t texel11;
  uint32_t texel12;
  uint32_t texel13;
  uint32_t texel14;
  uint32_t texel15;
  bool hasMore;
  int blueTopBit;
  uint8_t paletteGreen;
  uint8_t paletteBlue;
  uint8_t paletteFlags;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int paletteIndexOrCounter;
  DirectDrawPaletteEntry grayPaletteEntry;
  uint32_t nextColumnsOrMask;
  uint32_t columnsOrMask;
  uint8_t *byteCursor;
  GraphicsTexturePaletteEntry *paletteSourceCursor;
  uint16_t *sourceTexel;
  uint8_t *byteCursorOrRow;
  uint16_t *destinationWord;
  DirectDrawPaletteEntry *paletteEntryCursor;
  uint32_t *destinationDword;
  uint16_t averageBlue;
  uint32_t averageRgb;
  uint16_t averageGreen;
  uint16_t averageRed;
  uint16_t averageAlpha;
  uint8_t *sourceByteRow;
  uint16_t *sourceTexelRow;
  uint8_t *destinationByteRow;
  uint16_t *destinationWordRow;
  uint32_t *destinationDwordRow;
  IDirectDrawPalette *createdPalette;
  GraphicsTextureSourceAsset *paletteBank;
  int rowsRemaining;
  uint32_t sourceWidth;
  int alphaShiftRight;
  int blueShiftRight;
  int greenShiftRight;
  int redShiftRight;
  int alphaShiftLeft;
  int blueShiftLeft;
  int greenShiftLeft;
  int redShiftLeft;
  TH_LEGACY_LONG savedPitch; /* dead stores, but they steer MSVC's register allocation (removing them changes the code) */
  TH_LEGACY_LPVOID savedSurfaceBits;
  IDirectDrawSurface3 *stagingSurface3;
  uint32_t subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    hresult = stagingSurface3->lpVtbl->IsLost(stagingSurface3);
    paletteIndexOrCounter = 0;
    if (hresult != 0) {
      paletteIndexOrCounter = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (paletteIndexOrCounter == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      hresult = stagingSurface3->lpVtbl->Lock
                         (stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,
                          NULL);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrGreenTopBit = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                              (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = *(int *)(GFX_ANCHORED_ASSET_BYTES(sourceAsset,offsetOrGreenTopBit) +
                                         GFX_SUBRESOURCE_PALETTE_INDEX);
        sourceWidth = *(uint32_t *)(GFX_ANCHORED_ASSET_BYTES(sourceAsset,offsetOrGreenTopBit) +
                                    GFX_SUBRESOURCE_PIXEL_WIDTH);
        rowsRemaining = *(int *)(GFX_ANCHORED_ASSET_BYTES(sourceAsset,offsetOrGreenTopBit) +
                                 GFX_SUBRESOURCE_PIXEL_HEIGHT);
        offsetOrGreenTopBit = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrGreenTopBit))->dataOffset;
        if (paletteIndexOrCounter < 0) {
          sourceTexel = (uint16_t *)GFX_ANCHORED_ASSET_BYTES(sourceAsset,offsetOrGreenTopBit);
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            columnsOrMask = sourceWidth;
            byteCursor = g_SurfaceDesc.lpSurface;
            sourceTexelRow = sourceTexel;
            byteCursorOrRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursor = *(uint8_t *)((int)sourceTexel + 3);
                  sourceTexel = sourceTexel + 8;
                  nextColumnsOrMask = columnsOrMask - 4;
                  hasMore = 3 < (int)columnsOrMask;
                  columnsOrMask = nextColumnsOrMask;
                  byteCursor++;
                } while (nextColumnsOrMask != 0 && hasMore);
                sourceTexel = sourceTexelRow + sourceWidth * 8;
                paletteIndexOrCounter = rowsRemaining - 4;
                hasMore = 3 < rowsRemaining;
                columnsOrMask = sourceWidth & 0xfffffff;
                byteCursor = byteCursorOrRow + destinationPitch;
                sourceTexelRow = sourceTexel;
                byteCursorOrRow = byteCursorOrRow + destinationPitch;
                rowsRemaining = paletteIndexOrCounter;
              } while (paletteIndexOrCounter != 0 && hasMore);
              stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 256;
              grayPaletteEntry.red = 0;
              grayPaletteEntry.green = 0;
              grayPaletteEntry.blue = 0;
              grayPaletteEntry.flags = 0;
              /* grey ramp: all four bytes step by one per entry */
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                *paletteEntryCursor = grayPaletteEntry;
                paletteEntryCursor++;
                /* bytes never exceed 255 before the last (unused) step, so no carry crosses bytes */
                grayPaletteEntry.red++;
                grayPaletteEntry.green++;
                grayPaletteEntry.blue++;
                grayPaletteEntry.flags++;
                paletteIndexOrCounter--;
              } while (paletteIndexOrCounter != 0);
              /* as in the original, the palette is released at once and never attached (no SetPalette) */
              hresult = g_DirectDraw2->lpVtbl->CreatePalette
                                 (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,
                                  NULL);
              if (hresult == 0) {
                createdPalette->lpVtbl->Release(createdPalette);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
            columnsOrMask = destinationFormat->dwRBitMask;
            nextColumnsOrMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((columnsOrMask != 0) && (nextColumnsOrMask != 0)) && (blueMask != 0)) {
              /* per channel: shift the ARGB byte right to the mask's width, then left to its lowest bit */
              redShiftLeft = 0;
              if (columnsOrMask != 0) {
                for (; (columnsOrMask >> redShiftLeft & 1) == 0; redShiftLeft++) {
                }
              }
              greenShiftLeft = 0;
              if (nextColumnsOrMask != 0) {
                for (; (nextColumnsOrMask >> greenShiftLeft & 1) == 0; greenShiftLeft++) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft++) {
                }
              }
              paletteIndexOrCounter = 31;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                }
              }
              offsetOrGreenTopBit = 31;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit--) {
                }
              }
              blueTopBit = 31;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit--) {
                }
              }
              redShiftRight = 24 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 16 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              columnsOrMask = destinationFormat->dwRGBAlphaBitMask;
              if (columnsOrMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 16;
              }
              else {
                alphaShiftLeft = 0;
                if (columnsOrMask != 0) {
                  for (; (columnsOrMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft++) {
                  }
                }
                paletteIndexOrCounter = 31;
                if (columnsOrMask != 0) {
                  for (; columnsOrMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                  }
                }
                alphaShiftRight = 32 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              columnsOrMask = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 17) {
                do {
                  do {
                    texel00 = *(uint32_t *)sourceTexel;
                    texel01 = *(uint32_t *)(sourceTexel + 2);
                    texel02 = *(uint32_t *)(sourceTexel + sourceWidth * 2);
                    texel03 = *(uint32_t *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel04 = *(uint32_t *)(sourceTexel + 4);
                    texel05 = *(uint32_t *)(sourceTexel + 6);
                    texel06 = *(uint32_t *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel07 = *(uint32_t *)(sourceTexel + (sourceWidth + 3) * 2);
                    sourceTexel = sourceTexel + sourceWidth * 4;
                    texel08 = *(uint32_t *)sourceTexel;
                    texel09 = *(uint32_t *)(sourceTexel + 2);
                    texel10 = *(uint32_t *)(sourceTexel + sourceWidth * 2);
                    texel11 = *(uint32_t *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel12 = *(uint32_t *)(sourceTexel + 4);
                    texel13 = *(uint32_t *)(sourceTexel + 6);
                    texel14 = *(uint32_t *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel15 = *(uint32_t *)(sourceTexel + (sourceWidth + 3) * 2);
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(texel00) + TEXTURE_TEXEL_BLUE(texel01) +
                                          TEXTURE_TEXEL_BLUE(texel02) + TEXTURE_TEXEL_BLUE(texel03) +
                                          TEXTURE_TEXEL_BLUE(texel04) + TEXTURE_TEXEL_BLUE(texel05) +
                                          TEXTURE_TEXEL_BLUE(texel06) + TEXTURE_TEXEL_BLUE(texel07) +
                                          TEXTURE_TEXEL_BLUE(texel08) + TEXTURE_TEXEL_BLUE(texel09) +
                                          TEXTURE_TEXEL_BLUE(texel10) + TEXTURE_TEXEL_BLUE(texel11) +
                                          TEXTURE_TEXEL_BLUE(texel12) + TEXTURE_TEXEL_BLUE(texel13) +
                                          TEXTURE_TEXEL_BLUE(texel14) + TEXTURE_TEXEL_BLUE(texel15)) >> 4;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(texel00) + TEXTURE_TEXEL_GREEN(texel01) +
                                           TEXTURE_TEXEL_GREEN(texel02) + TEXTURE_TEXEL_GREEN(texel03) +
                                           TEXTURE_TEXEL_GREEN(texel04) + TEXTURE_TEXEL_GREEN(texel05) +
                                           TEXTURE_TEXEL_GREEN(texel06) + TEXTURE_TEXEL_GREEN(texel07) +
                                           TEXTURE_TEXEL_GREEN(texel08) + TEXTURE_TEXEL_GREEN(texel09) +
                                           TEXTURE_TEXEL_GREEN(texel10) + TEXTURE_TEXEL_GREEN(texel11) +
                                           TEXTURE_TEXEL_GREEN(texel12) + TEXTURE_TEXEL_GREEN(texel13) +
                                           TEXTURE_TEXEL_GREEN(texel14) + TEXTURE_TEXEL_GREEN(texel15)) >> 4;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(texel00) + TEXTURE_TEXEL_RED(texel01) +
                                         TEXTURE_TEXEL_RED(texel02) + TEXTURE_TEXEL_RED(texel03) +
                                         TEXTURE_TEXEL_RED(texel04) + TEXTURE_TEXEL_RED(texel05) +
                                         TEXTURE_TEXEL_RED(texel06) + TEXTURE_TEXEL_RED(texel07) +
                                         TEXTURE_TEXEL_RED(texel08) + TEXTURE_TEXEL_RED(texel09) +
                                         TEXTURE_TEXEL_RED(texel10) + TEXTURE_TEXEL_RED(texel11) +
                                         TEXTURE_TEXEL_RED(texel12) + TEXTURE_TEXEL_RED(texel13) +
                                         TEXTURE_TEXEL_RED(texel14) + TEXTURE_TEXEL_RED(texel15)) >> 4;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(texel00) + TEXTURE_TEXEL_ALPHA(texel01) +
                                           TEXTURE_TEXEL_ALPHA(texel02) + TEXTURE_TEXEL_ALPHA(texel03) +
                                           TEXTURE_TEXEL_ALPHA(texel04) + TEXTURE_TEXEL_ALPHA(texel05) +
                                           TEXTURE_TEXEL_ALPHA(texel06) + TEXTURE_TEXEL_ALPHA(texel07) +
                                           TEXTURE_TEXEL_ALPHA(texel08) + TEXTURE_TEXEL_ALPHA(texel09) +
                                           TEXTURE_TEXEL_ALPHA(texel10) + TEXTURE_TEXEL_ALPHA(texel11) +
                                           TEXTURE_TEXEL_ALPHA(texel12) + TEXTURE_TEXEL_ALPHA(texel13) +
                                           TEXTURE_TEXEL_ALPHA(texel14) + TEXTURE_TEXEL_ALPHA(texel15)) >> 4;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK));
                    sourceTexel = sourceTexel + (sourceWidth * -2 + 4) * 2;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationWord++;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  sourceTexel = sourceTexelRow + sourceWidth * 8;
                  paletteIndexOrCounter = rowsRemaining - 4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth & 0xfffffff;
                  destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationWordRow = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
              }
              else {
                do {
                  do {
                    texel00 = *(uint32_t *)sourceTexel;
                    texel01 = *(uint32_t *)(sourceTexel + 2);
                    texel02 = *(uint32_t *)(sourceTexel + sourceWidth * 2);
                    texel03 = *(uint32_t *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel04 = *(uint32_t *)(sourceTexel + 4);
                    texel05 = *(uint32_t *)(sourceTexel + 6);
                    texel06 = *(uint32_t *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel07 = *(uint32_t *)(sourceTexel + (sourceWidth + 3) * 2);
                    sourceTexel = sourceTexel + sourceWidth * 4;
                    texel08 = *(uint32_t *)sourceTexel;
                    texel09 = *(uint32_t *)(sourceTexel + 2);
                    texel10 = *(uint32_t *)(sourceTexel + sourceWidth * 2);
                    texel11 = *(uint32_t *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel12 = *(uint32_t *)(sourceTexel + 4);
                    texel13 = *(uint32_t *)(sourceTexel + 6);
                    texel14 = *(uint32_t *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel15 = *(uint32_t *)(sourceTexel + (sourceWidth + 3) * 2);
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(texel00) + TEXTURE_TEXEL_BLUE(texel01) +
                                          TEXTURE_TEXEL_BLUE(texel02) + TEXTURE_TEXEL_BLUE(texel03) +
                                          TEXTURE_TEXEL_BLUE(texel04) + TEXTURE_TEXEL_BLUE(texel05) +
                                          TEXTURE_TEXEL_BLUE(texel06) + TEXTURE_TEXEL_BLUE(texel07) +
                                          TEXTURE_TEXEL_BLUE(texel08) + TEXTURE_TEXEL_BLUE(texel09) +
                                          TEXTURE_TEXEL_BLUE(texel10) + TEXTURE_TEXEL_BLUE(texel11) +
                                          TEXTURE_TEXEL_BLUE(texel12) + TEXTURE_TEXEL_BLUE(texel13) +
                                          TEXTURE_TEXEL_BLUE(texel14) + TEXTURE_TEXEL_BLUE(texel15)) >> 4;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(texel00) + TEXTURE_TEXEL_GREEN(texel01) +
                                           TEXTURE_TEXEL_GREEN(texel02) + TEXTURE_TEXEL_GREEN(texel03) +
                                           TEXTURE_TEXEL_GREEN(texel04) + TEXTURE_TEXEL_GREEN(texel05) +
                                           TEXTURE_TEXEL_GREEN(texel06) + TEXTURE_TEXEL_GREEN(texel07) +
                                           TEXTURE_TEXEL_GREEN(texel08) + TEXTURE_TEXEL_GREEN(texel09) +
                                           TEXTURE_TEXEL_GREEN(texel10) + TEXTURE_TEXEL_GREEN(texel11) +
                                           TEXTURE_TEXEL_GREEN(texel12) + TEXTURE_TEXEL_GREEN(texel13) +
                                           TEXTURE_TEXEL_GREEN(texel14) + TEXTURE_TEXEL_GREEN(texel15)) >> 4;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(texel00) + TEXTURE_TEXEL_RED(texel01) +
                                         TEXTURE_TEXEL_RED(texel02) + TEXTURE_TEXEL_RED(texel03) +
                                         TEXTURE_TEXEL_RED(texel04) + TEXTURE_TEXEL_RED(texel05) +
                                         TEXTURE_TEXEL_RED(texel06) + TEXTURE_TEXEL_RED(texel07) +
                                         TEXTURE_TEXEL_RED(texel08) + TEXTURE_TEXEL_RED(texel09) +
                                         TEXTURE_TEXEL_RED(texel10) + TEXTURE_TEXEL_RED(texel11) +
                                         TEXTURE_TEXEL_RED(texel12) + TEXTURE_TEXEL_RED(texel13) +
                                         TEXTURE_TEXEL_RED(texel14) + TEXTURE_TEXEL_RED(texel15)) >> 4;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(texel00) + TEXTURE_TEXEL_ALPHA(texel01) +
                                           TEXTURE_TEXEL_ALPHA(texel02) + TEXTURE_TEXEL_ALPHA(texel03) +
                                           TEXTURE_TEXEL_ALPHA(texel04) + TEXTURE_TEXEL_ALPHA(texel05) +
                                           TEXTURE_TEXEL_ALPHA(texel06) + TEXTURE_TEXEL_ALPHA(texel07) +
                                           TEXTURE_TEXEL_ALPHA(texel08) + TEXTURE_TEXEL_ALPHA(texel09) +
                                           TEXTURE_TEXEL_ALPHA(texel10) + TEXTURE_TEXEL_ALPHA(texel11) +
                                           TEXTURE_TEXEL_ALPHA(texel12) + TEXTURE_TEXEL_ALPHA(texel13) +
                                           TEXTURE_TEXEL_ALPHA(texel14) + TEXTURE_TEXEL_ALPHA(texel15)) >> 4;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK))
                               << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK);
                    sourceTexel = sourceTexel + (sourceWidth * -2 + 4) * 2;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationDword++;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  sourceTexel = sourceTexelRow + sourceWidth * 8;
                  paletteIndexOrCounter = rowsRemaining - 4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth & 0xfffffff;
                  destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationDwordRow = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
              }
            }
          }
        }
        else {
          byteCursor = (uint8_t *)sourceAsset + offsetOrGreenTopBit;
          paletteBank = sourceAsset + paletteIndexOrCounter * 4 + 1;
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            columnsOrMask = sourceWidth;
            byteCursorOrRow = g_SurfaceDesc.lpSurface;
            sourceByteRow = byteCursor;
            destinationByteRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursorOrRow = *byteCursor;
                  byteCursor = byteCursor + 4;
                  nextColumnsOrMask = columnsOrMask - 4;
                  hasMore = 3 < (int)columnsOrMask;
                  columnsOrMask = nextColumnsOrMask;
                  byteCursorOrRow++;
                } while (nextColumnsOrMask != 0 && hasMore);
                byteCursor = sourceByteRow + sourceWidth * 4;
                paletteIndexOrCounter = rowsRemaining - 4;
                hasMore = 3 < rowsRemaining;
                columnsOrMask = sourceWidth;
                byteCursorOrRow = destinationByteRow + destinationPitch;
                sourceByteRow = byteCursor;
                destinationByteRow = destinationByteRow + destinationPitch;
                rowsRemaining = paletteIndexOrCounter;
              } while (paletteIndexOrCounter != 0 && hasMore);
              stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 256;
              /* the four bytes of each entry's argb8888 word, copied in memory order */
              paletteSourceCursor = (GraphicsTexturePaletteEntry *)paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = ((uint8_t *)&paletteSourceCursor->argb8888)[1];
                paletteBlue = ((uint8_t *)&paletteSourceCursor->argb8888)[2];
                paletteFlags = ((uint8_t *)&paletteSourceCursor->argb8888)[3];
                paletteEntryCursor->red = ((uint8_t *)&paletteSourceCursor->argb8888)[0];
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor++;
                paletteEntryCursor++;
                paletteIndexOrCounter--;
              } while (paletteIndexOrCounter != 0);
              /* as in the original, the palette is released at once and never attached (no SetPalette) */
              hresult = g_DirectDraw2->lpVtbl->CreatePalette
                                 (g_DirectDraw2,DDPCAPS_8BIT | DDPCAPS_ALLOW256,g_TexturePaletteEntries,&createdPalette,
                                  NULL);
              if (hresult == 0) {
                createdPalette->lpVtbl->Release(createdPalette);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
            columnsOrMask = destinationFormat->dwRBitMask;
            nextColumnsOrMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((columnsOrMask != 0) && (nextColumnsOrMask != 0)) && (blueMask != 0)) {
              /* per channel: shift the ARGB byte right to the mask's width, then left to its lowest bit */
              redShiftLeft = 0;
              if (columnsOrMask != 0) {
                for (; (columnsOrMask >> redShiftLeft & 1) == 0; redShiftLeft++) {
                }
              }
              greenShiftLeft = 0;
              if (nextColumnsOrMask != 0) {
                for (; (nextColumnsOrMask >> greenShiftLeft & 1) == 0; greenShiftLeft++) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft++) {
                }
              }
              paletteIndexOrCounter = 31;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                }
              }
              offsetOrGreenTopBit = 31;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit--) {
                }
              }
              blueTopBit = 31;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit--) {
                }
              }
              redShiftRight = 24 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 16 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              columnsOrMask = destinationFormat->dwRGBAlphaBitMask;
              if (columnsOrMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 16;
              }
              else {
                alphaShiftLeft = 0;
                if (columnsOrMask != 0) {
                  for (; (columnsOrMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft++) {
                  }
                }
                paletteIndexOrCounter = 31;
                if (columnsOrMask != 0) {
                  for (; columnsOrMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter--) {
                  }
                }
                alphaShiftRight = 32 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              columnsOrMask = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrRow = byteCursor;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 17) {
                do {
                  do {
                    texel00 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,*byteCursor);
                    texel01 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[1]);
                    texel02 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth]);
                    texel03 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 1]);
                    texel04 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[2]);
                    texel05 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[3]);
                    texel06 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 2]);
                    texel07 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 3]);
                    byteCursor = byteCursor + sourceWidth * 2;
                    texel08 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,*byteCursor);
                    texel09 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[1]);
                    texel10 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth]);
                    texel11 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 1]);
                    texel12 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[2]);
                    texel13 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[3]);
                    texel14 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 2]);
                    texel15 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 3]);
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(texel00) + TEXTURE_TEXEL_BLUE(texel01) +
                                          TEXTURE_TEXEL_BLUE(texel02) + TEXTURE_TEXEL_BLUE(texel03) +
                                          TEXTURE_TEXEL_BLUE(texel04) + TEXTURE_TEXEL_BLUE(texel05) +
                                          TEXTURE_TEXEL_BLUE(texel06) + TEXTURE_TEXEL_BLUE(texel07) +
                                          TEXTURE_TEXEL_BLUE(texel08) + TEXTURE_TEXEL_BLUE(texel09) +
                                          TEXTURE_TEXEL_BLUE(texel10) + TEXTURE_TEXEL_BLUE(texel11) +
                                          TEXTURE_TEXEL_BLUE(texel12) + TEXTURE_TEXEL_BLUE(texel13) +
                                          TEXTURE_TEXEL_BLUE(texel14) + TEXTURE_TEXEL_BLUE(texel15)) >> 4;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(texel00) + TEXTURE_TEXEL_GREEN(texel01) +
                                           TEXTURE_TEXEL_GREEN(texel02) + TEXTURE_TEXEL_GREEN(texel03) +
                                           TEXTURE_TEXEL_GREEN(texel04) + TEXTURE_TEXEL_GREEN(texel05) +
                                           TEXTURE_TEXEL_GREEN(texel06) + TEXTURE_TEXEL_GREEN(texel07) +
                                           TEXTURE_TEXEL_GREEN(texel08) + TEXTURE_TEXEL_GREEN(texel09) +
                                           TEXTURE_TEXEL_GREEN(texel10) + TEXTURE_TEXEL_GREEN(texel11) +
                                           TEXTURE_TEXEL_GREEN(texel12) + TEXTURE_TEXEL_GREEN(texel13) +
                                           TEXTURE_TEXEL_GREEN(texel14) + TEXTURE_TEXEL_GREEN(texel15)) >> 4;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(texel00) + TEXTURE_TEXEL_RED(texel01) +
                                         TEXTURE_TEXEL_RED(texel02) + TEXTURE_TEXEL_RED(texel03) +
                                         TEXTURE_TEXEL_RED(texel04) + TEXTURE_TEXEL_RED(texel05) +
                                         TEXTURE_TEXEL_RED(texel06) + TEXTURE_TEXEL_RED(texel07) +
                                         TEXTURE_TEXEL_RED(texel08) + TEXTURE_TEXEL_RED(texel09) +
                                         TEXTURE_TEXEL_RED(texel10) + TEXTURE_TEXEL_RED(texel11) +
                                         TEXTURE_TEXEL_RED(texel12) + TEXTURE_TEXEL_RED(texel13) +
                                         TEXTURE_TEXEL_RED(texel14) + TEXTURE_TEXEL_RED(texel15)) >> 4;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(texel00) + TEXTURE_TEXEL_ALPHA(texel01) +
                                           TEXTURE_TEXEL_ALPHA(texel02) + TEXTURE_TEXEL_ALPHA(texel03) +
                                           TEXTURE_TEXEL_ALPHA(texel04) + TEXTURE_TEXEL_ALPHA(texel05) +
                                           TEXTURE_TEXEL_ALPHA(texel06) + TEXTURE_TEXEL_ALPHA(texel07) +
                                           TEXTURE_TEXEL_ALPHA(texel08) + TEXTURE_TEXEL_ALPHA(texel09) +
                                           TEXTURE_TEXEL_ALPHA(texel10) + TEXTURE_TEXEL_ALPHA(texel11) +
                                           TEXTURE_TEXEL_ALPHA(texel12) + TEXTURE_TEXEL_ALPHA(texel13) +
                                           TEXTURE_TEXEL_ALPHA(texel14) + TEXTURE_TEXEL_ALPHA(texel15)) >> 4;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK)) << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK)) |
                               (uint16_t)(((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                                       ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK));
                    byteCursor = byteCursor + sourceWidth * -2 + 4;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationWord++;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 4;
                  paletteIndexOrCounter = rowsRemaining - 4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth;
                  destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationWordRow = (uint16_t *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
                stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              }
              else {
                do {
                  do {
                    texel00 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,*byteCursor);
                    texel01 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[1]);
                    texel02 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth]);
                    texel03 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 1]);
                    texel04 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[2]);
                    texel05 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[3]);
                    texel06 = GFX_ANCHORED_PALETTE_ARGB(paletteBank,byteCursor[sourceWidth + 2]);
                    texel07 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth + 3]].argb8888;
                    byteCursor = byteCursor + sourceWidth * 2;
                    texel08 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)*byteCursor].argb8888;
                    texel09 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[1]].argb8888;
                    texel10 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth]].argb8888;
                    texel11 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth + 1]].argb8888;
                    texel12 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[2]].argb8888;
                    texel13 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[3]].argb8888;
                    texel14 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth + 2]].argb8888;
                    texel15 = ((GraphicsTexturePaletteEntry *)paletteBank)[(uint32_t)byteCursor[sourceWidth + 3]].argb8888;
                    averageBlue = (uint16_t)(TEXTURE_TEXEL_BLUE(texel00) + TEXTURE_TEXEL_BLUE(texel01) +
                                          TEXTURE_TEXEL_BLUE(texel02) + TEXTURE_TEXEL_BLUE(texel03) +
                                          TEXTURE_TEXEL_BLUE(texel04) + TEXTURE_TEXEL_BLUE(texel05) +
                                          TEXTURE_TEXEL_BLUE(texel06) + TEXTURE_TEXEL_BLUE(texel07) +
                                          TEXTURE_TEXEL_BLUE(texel08) + TEXTURE_TEXEL_BLUE(texel09) +
                                          TEXTURE_TEXEL_BLUE(texel10) + TEXTURE_TEXEL_BLUE(texel11) +
                                          TEXTURE_TEXEL_BLUE(texel12) + TEXTURE_TEXEL_BLUE(texel13) +
                                          TEXTURE_TEXEL_BLUE(texel14) + TEXTURE_TEXEL_BLUE(texel15)) >> 4;
                    averageGreen = (uint16_t)(TEXTURE_TEXEL_GREEN(texel00) + TEXTURE_TEXEL_GREEN(texel01) +
                                           TEXTURE_TEXEL_GREEN(texel02) + TEXTURE_TEXEL_GREEN(texel03) +
                                           TEXTURE_TEXEL_GREEN(texel04) + TEXTURE_TEXEL_GREEN(texel05) +
                                           TEXTURE_TEXEL_GREEN(texel06) + TEXTURE_TEXEL_GREEN(texel07) +
                                           TEXTURE_TEXEL_GREEN(texel08) + TEXTURE_TEXEL_GREEN(texel09) +
                                           TEXTURE_TEXEL_GREEN(texel10) + TEXTURE_TEXEL_GREEN(texel11) +
                                           TEXTURE_TEXEL_GREEN(texel12) + TEXTURE_TEXEL_GREEN(texel13) +
                                           TEXTURE_TEXEL_GREEN(texel14) + TEXTURE_TEXEL_GREEN(texel15)) >> 4;
                    averageRed = (uint16_t)(TEXTURE_TEXEL_RED(texel00) + TEXTURE_TEXEL_RED(texel01) +
                                         TEXTURE_TEXEL_RED(texel02) + TEXTURE_TEXEL_RED(texel03) +
                                         TEXTURE_TEXEL_RED(texel04) + TEXTURE_TEXEL_RED(texel05) +
                                         TEXTURE_TEXEL_RED(texel06) + TEXTURE_TEXEL_RED(texel07) +
                                         TEXTURE_TEXEL_RED(texel08) + TEXTURE_TEXEL_RED(texel09) +
                                         TEXTURE_TEXEL_RED(texel10) + TEXTURE_TEXEL_RED(texel11) +
                                         TEXTURE_TEXEL_RED(texel12) + TEXTURE_TEXEL_RED(texel13) +
                                         TEXTURE_TEXEL_RED(texel14) + TEXTURE_TEXEL_RED(texel15)) >> 4;
                    averageAlpha = (uint16_t)(TEXTURE_TEXEL_ALPHA(texel00) + TEXTURE_TEXEL_ALPHA(texel01) +
                                           TEXTURE_TEXEL_ALPHA(texel02) + TEXTURE_TEXEL_ALPHA(texel03) +
                                           TEXTURE_TEXEL_ALPHA(texel04) + TEXTURE_TEXEL_ALPHA(texel05) +
                                           TEXTURE_TEXEL_ALPHA(texel06) + TEXTURE_TEXEL_ALPHA(texel07) +
                                           TEXTURE_TEXEL_ALPHA(texel08) + TEXTURE_TEXEL_ALPHA(texel09) +
                                           TEXTURE_TEXEL_ALPHA(texel10) + TEXTURE_TEXEL_ALPHA(texel11) +
                                           TEXTURE_TEXEL_ALPHA(texel12) + TEXTURE_TEXEL_ALPHA(texel13) +
                                           TEXTURE_TEXEL_ALPHA(texel14) + TEXTURE_TEXEL_ALPHA(texel15)) >> 4;
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 |
                                 TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & SHIFT_COUNT_MASK))
                               << ((uint8_t)alphaShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_BLUE_MASK) >> ((uint8_t)blueShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)blueShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_GREEN_MASK) >> ((uint8_t)greenShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)greenShiftLeft & SHIFT_COUNT_MASK) |
                               ((averageRgb & ARGB8888_RED_MASK) >> ((uint8_t)redShiftRight & SHIFT_COUNT_MASK)) <<
                               ((uint8_t)redShiftLeft & SHIFT_COUNT_MASK);
                    byteCursor = byteCursor + sourceWidth * -2 + 4;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationDword++;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 4;
                  paletteIndexOrCounter = rowsRemaining - 4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth;
                  destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationDwordRow = (uint32_t *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
                stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
          }
        }
        stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      }
    }
  }
GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads--;
  return;
}


/* Address: 0x0057C6C0.
   Alpha counterpart of GraphicsTexture_UploadColor_1x (g_GraphicsDispatchTable.alphaUpload[0], called by
   GraphicsTextureSet_RefreshAlpha): the subresource's pixels are read as one alpha byte each and written, at
   full size, into the alpha bits of the staging surface with all RGB bits set (white). 8-bit surfaces are
   skipped. A lost surface is restored first; g_ActiveTextureUploads is raised for the duration.
*/
void GraphicsTexture_UploadAlpha_1x(GraphicsTextureResource *texture)

{
  IDirectDrawSurface3 *stagingSurface3;
  GraphicsTextureSourceAsset *sourceAsset;
  GraphicsSubresourceIndex subresourceIndex;
  DDPIXELFORMAT *destinationFormat;
  uint8_t rotateShift;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int restoreResultOrMaskWidth;
  uint8_t alphaShift;
  uint32_t rgbMaskBits;
  uint8_t *maskCursor;
  uint16_t *destinationWord;
  uint32_t *destinationDword;
  int offsetOrColumnsRemaining;
  uint16_t *destinationWordRow;
  uint32_t *destinationDwordRow;
  int rowsRemaining;
  
  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    hresult = stagingSurface3->lpVtbl->IsLost(stagingSurface3);
    restoreResultOrMaskWidth = 0;
    if (hresult != 0) {
      restoreResultOrMaskWidth = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (restoreResultOrMaskWidth == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      hresult = stagingSurface3->lpVtbl->Lock
                        (stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,NULL);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrColumnsRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                   (sourceAsset->tableDescriptor).subresourceTableOffset;
        restoreResultOrMaskWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelHeight;
        maskCursor = (uint8_t *)sourceAsset +
                     ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->dataOffset;
        if (((restoreResultOrMaskWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) &&
           (destinationFormat->dwRGBBitCount != 8)) {
          offsetOrColumnsRemaining = 31;
          if (destinationFormat->dwRGBAlphaBitMask != 0) {
            for (; destinationFormat->dwRGBAlphaBitMask >> offsetOrColumnsRemaining == 0; offsetOrColumnsRemaining--) {
            }
          }
          /* ROL by (top alpha bit - 7) puts the byte's top bit on the mask's top bit; bits that spill into the RGB
             fields are hidden by OR-ing all RGB mask bits */
          alphaShift = (char)offsetOrColumnsRemaining - 7;
          rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
          destinationDword = g_SurfaceDesc.lpSurface;
          destinationWord = g_SurfaceDesc.lpSurface;
          offsetOrColumnsRemaining = restoreResultOrMaskWidth;
          destinationDwordRow = g_SurfaceDesc.lpSurface;
          destinationWordRow = g_SurfaceDesc.lpSurface;
          if (destinationFormat->dwRGBBitCount < 17) {
            do {
              do {
                rotateShift = alphaShift & SHIFT_COUNT_MASK;
                *destinationWord = (uint16_t)*maskCursor << rotateShift | (uint16_t)(*maskCursor >> (32 - rotateShift)) |
                           (uint16_t)rgbMaskBits;
                maskCursor++;
                offsetOrColumnsRemaining--;
                destinationWord++;
              } while (offsetOrColumnsRemaining != 0);
              destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
              rowsRemaining--;
              offsetOrColumnsRemaining = restoreResultOrMaskWidth;
              destinationWordRow = destinationWord;
            } while (rowsRemaining != 0);
          }
          else {
            do {
              do {
                rotateShift = alphaShift & SHIFT_COUNT_MASK;
                *destinationDword = (uint32_t)*maskCursor << rotateShift | (uint32_t)(*maskCursor >> (32 - rotateShift)) | rgbMaskBits;
                maskCursor++;
                offsetOrColumnsRemaining--;
                /* The original advances only 2 bytes per 32-bit store (ADD EDI,0x2). */
                destinationDword = (uint32_t *)((int)destinationDword + 2);
              } while (offsetOrColumnsRemaining != 0);
              destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
              rowsRemaining--;
              offsetOrColumnsRemaining = restoreResultOrMaskWidth;
              destinationDwordRow = destinationDword;
            } while (rowsRemaining != 0);
          }
        }
        stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      }
    }
  }
  g_ActiveTextureUploads--;
  return;
}


/* Address: 0x0057C890.
   GraphicsTexture_UploadAlpha_1x at half size (g_GraphicsDispatchTable.alphaUpload[1]): each destination alpha
   is the 10-bit sum of a 2x2 block of source bytes, scaled to the alpha mask.
*/
void GraphicsTexture_UploadAlpha_2x(GraphicsTextureResource *texture)

{
  int nextRemaining;
  IDirectDrawSurface3 *stagingSurface3;
  GraphicsTextureSourceAsset *sourceAsset;
  GraphicsSubresourceIndex subresourceIndex;
  DDPIXELFORMAT *destinationFormat;
  bool hasMore;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  uint8_t rotateShift;
  uint16_t maskSum;
  TH_LEGACY_HRESULT hresult;
  int restoreResultOrMaskWidth;
  uint8_t alphaShift;
  uint32_t rgbMaskBits;
  uint8_t *maskCursor;
  uint16_t *destinationWord;
  uint32_t *destinationDword;
  int offsetOrColumnsRemaining;
  uint16_t *destinationWordRow;
  uint32_t *destinationDwordRow;
  int rowsRemaining;
  
  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    hresult = stagingSurface3->lpVtbl->IsLost(stagingSurface3);
    restoreResultOrMaskWidth = 0;
    if (hresult != 0) {
      restoreResultOrMaskWidth = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (restoreResultOrMaskWidth == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      hresult = stagingSurface3->lpVtbl->Lock
                         (stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,NULL);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrColumnsRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                   (sourceAsset->tableDescriptor).subresourceTableOffset;
        restoreResultOrMaskWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelHeight;
        maskCursor = (uint8_t *)sourceAsset +
                     ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->dataOffset;
        if (((restoreResultOrMaskWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) &&
           (destinationFormat->dwRGBBitCount != 8)) {
          offsetOrColumnsRemaining = 31;
          if (destinationFormat->dwRGBAlphaBitMask != 0) {
            for (; destinationFormat->dwRGBAlphaBitMask >> offsetOrColumnsRemaining == 0; offsetOrColumnsRemaining--) {
            }
          }
          /* as in the 1x version, for the 10-bit sum of four bytes */
          alphaShift = (char)offsetOrColumnsRemaining - 9;
          rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
          destinationDword = g_SurfaceDesc.lpSurface;
          destinationWord = g_SurfaceDesc.lpSurface;
          offsetOrColumnsRemaining = restoreResultOrMaskWidth;
          destinationDwordRow = g_SurfaceDesc.lpSurface;
          destinationWordRow = g_SurfaceDesc.lpSurface;
          if (destinationFormat->dwRGBBitCount < 17) {
            do {
              do {
                /* ADD AL / ADC AH: 16-bit sum of the 2x2 mask bytes. */
                maskSum = (uint16_t)((uint16_t)*maskCursor + (uint16_t)maskCursor[1] + (uint16_t)maskCursor[restoreResultOrMaskWidth] +
                                   (uint16_t)maskCursor[restoreResultOrMaskWidth + 1]);
                rotateShift = alphaShift & SHIFT_COUNT_MASK;
                *destinationWord = maskSum << rotateShift | maskSum >> (32 - rotateShift) | (uint16_t)rgbMaskBits;
                maskCursor = maskCursor + 2;
                nextRemaining = offsetOrColumnsRemaining - 2;
                hasMore = 1 < offsetOrColumnsRemaining;
                destinationWord++;
                offsetOrColumnsRemaining = nextRemaining;
              } while (nextRemaining != 0 && hasMore);
              maskCursor = maskCursor + restoreResultOrMaskWidth;
              destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
              nextRemaining = rowsRemaining - 2;
              hasMore = 1 < rowsRemaining;
              offsetOrColumnsRemaining = restoreResultOrMaskWidth;
              destinationWordRow = destinationWord;
              rowsRemaining = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
          }
          else {
            do {
              do {
                /* ADD AL / ADC AH: 16-bit sum of the 2x2 mask bytes. */
                maskSum = (uint16_t)((uint16_t)*maskCursor + (uint16_t)maskCursor[1] + (uint16_t)maskCursor[restoreResultOrMaskWidth] +
                                   (uint16_t)maskCursor[restoreResultOrMaskWidth + 1]);
                rotateShift = alphaShift & SHIFT_COUNT_MASK;
                *destinationDword = (uint32_t)maskSum << rotateShift | (uint32_t)(maskSum >> (32 - rotateShift)) | rgbMaskBits;
                maskCursor = maskCursor + 2;
                nextRemaining = offsetOrColumnsRemaining - 2;
                hasMore = 1 < offsetOrColumnsRemaining;
                /* The original advances only 2 bytes per 32-bit store (ADD EDI,0x2). */
                destinationDword = (uint32_t *)((int)destinationDword + 2);
                offsetOrColumnsRemaining = nextRemaining;
              } while (nextRemaining != 0 && hasMore);
              maskCursor = maskCursor + restoreResultOrMaskWidth;
              destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
              nextRemaining = rowsRemaining - 2;
              hasMore = 1 < rowsRemaining;
              offsetOrColumnsRemaining = restoreResultOrMaskWidth;
              destinationDwordRow = destinationDword;
              rowsRemaining = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
          }
        }
        stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      }
    }
  }
  g_ActiveTextureUploads--;
  return;
}


/* Address: 0x0057CAA0.
   GraphicsTexture_UploadAlpha_1x at quarter size (g_GraphicsDispatchTable.alphaUpload[2]): unlike the colour
   upload this is no full 4x4 average, each destination alpha is the sum of the four source bytes at (0,0),
   (2,0), (0,2) and (2,2) of its block, scaled like the 2x version.
*/
void GraphicsTexture_UploadAlpha_4x(GraphicsTextureResource *texture)

{
  int nextRemaining;
  IDirectDrawSurface3 *stagingSurface3;
  GraphicsTextureSourceAsset *sourceAsset;
  GraphicsSubresourceIndex subresourceIndex;
  DDPIXELFORMAT *destinationFormat;
  bool hasMore;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  uint8_t rotateShift;
  uint16_t maskSum;
  TH_LEGACY_HRESULT hresult;
  int restoreResultOrMaskWidth;
  uint8_t alphaShift;
  uint32_t rgbMaskBits;
  uint8_t *maskCursor;
  uint16_t *destinationWord;
  uint32_t *destinationDword;
  int offsetOrColumnsRemaining;
  uint16_t *destinationWordRow;
  uint32_t *destinationDwordRow;
  int rowsRemaining;
  
  g_ActiveTextureUploads++;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != NULL) {
    hresult = stagingSurface3->lpVtbl->IsLost(stagingSurface3);
    restoreResultOrMaskWidth = 0;
    if (hresult != 0) {
      restoreResultOrMaskWidth = stagingSurface3->lpVtbl->Restore(stagingSurface3);
    }
    if (restoreResultOrMaskWidth == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      hresult = stagingSurface3->lpVtbl->Lock
                         (stagingSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,NULL);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrColumnsRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                   (sourceAsset->tableDescriptor).subresourceTableOffset;
        restoreResultOrMaskWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelHeight;
        maskCursor = (uint8_t *)sourceAsset +
                     ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->dataOffset;
        if (((restoreResultOrMaskWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) &&
           (destinationFormat->dwRGBBitCount != 8)) {
          offsetOrColumnsRemaining = 31;
          if (destinationFormat->dwRGBAlphaBitMask != 0) {
            for (; destinationFormat->dwRGBAlphaBitMask >> offsetOrColumnsRemaining == 0; offsetOrColumnsRemaining--) {
            }
          }
          /* as in the 1x version, for the 10-bit sum of four bytes */
          alphaShift = (char)offsetOrColumnsRemaining - 9;
          rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
          destinationDword = g_SurfaceDesc.lpSurface;
          destinationWord = g_SurfaceDesc.lpSurface;
          offsetOrColumnsRemaining = restoreResultOrMaskWidth;
          destinationDwordRow = g_SurfaceDesc.lpSurface;
          destinationWordRow = g_SurfaceDesc.lpSurface;
          if (destinationFormat->dwRGBBitCount < 17) {
            do {
              do {
                /* ADD AL / ADC AH: 16-bit sum of the four mask taps. */
                maskSum = (uint16_t)((uint16_t)*maskCursor + (uint16_t)maskCursor[2] +
                                   (uint16_t)maskCursor[restoreResultOrMaskWidth * 2] +
                                   (uint16_t)maskCursor[restoreResultOrMaskWidth * 2 + 2]);
                rotateShift = alphaShift & SHIFT_COUNT_MASK;
                *destinationWord = maskSum << rotateShift | maskSum >> (32 - rotateShift) | (uint16_t)rgbMaskBits;
                maskCursor = maskCursor + 4;
                nextRemaining = offsetOrColumnsRemaining - 4;
                hasMore = 3 < offsetOrColumnsRemaining;
                destinationWord++;
                offsetOrColumnsRemaining = nextRemaining;
              } while (nextRemaining != 0 && hasMore);
              maskCursor = maskCursor + restoreResultOrMaskWidth * 3;
              destinationWord = (uint16_t *)((int)destinationWordRow + destinationPitch);
              nextRemaining = rowsRemaining - 4;
              hasMore = 3 < rowsRemaining;
              offsetOrColumnsRemaining = restoreResultOrMaskWidth;
              destinationWordRow = destinationWord;
              rowsRemaining = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
          }
          else {
            do {
              do {
                /* ADD AL / ADC AH: 16-bit sum of the four mask taps. */
                maskSum = (uint16_t)((uint16_t)*maskCursor + (uint16_t)maskCursor[2] +
                                   (uint16_t)maskCursor[restoreResultOrMaskWidth * 2] +
                                   (uint16_t)maskCursor[restoreResultOrMaskWidth * 2 + 2]);
                rotateShift = alphaShift & SHIFT_COUNT_MASK;
                *destinationDword = (uint32_t)maskSum << rotateShift | (uint32_t)(maskSum >> (32 - rotateShift)) | rgbMaskBits;
                maskCursor = maskCursor + 4;
                nextRemaining = offsetOrColumnsRemaining - 4;
                hasMore = 3 < offsetOrColumnsRemaining;
                /* The original advances only 2 bytes per 32-bit store (ADD EDI,0x2). */
                destinationDword = (uint32_t *)((int)destinationDword + 2);
                offsetOrColumnsRemaining = nextRemaining;
              } while (nextRemaining != 0 && hasMore);
              maskCursor = maskCursor + restoreResultOrMaskWidth * 3;
              destinationDword = (uint32_t *)((int)destinationDwordRow + destinationPitch);
              nextRemaining = rowsRemaining - 4;
              hasMore = 3 < rowsRemaining;
              offsetOrColumnsRemaining = restoreResultOrMaskWidth;
              destinationDwordRow = destinationDword;
              rowsRemaining = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
          }
        }
        stagingSurface3->lpVtbl->Unlock(stagingSurface3,surfaceBits);
      }
    }
  }
  g_ActiveTextureUploads--;
  return;
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
  int32_t handleResult;
  GraphicsTextureDownsampleShift effectiveShift;
  int dwordsRemaining;
  DDPIXELFORMAT *sourceFormatCursor;
  DDPIXELFORMAT *destinationFormatCursor;
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
  do {
    hresult = g_DirectDraw2->lpVtbl->CreateSurface
                      (g_DirectDraw2,&g_SurfaceDesc,&deviceSurfaceBase,NULL);
    if (hresult == 0) {
      hresult = deviceSurfaceBase->lpVtbl->QueryInterface
                        (deviceSurfaceBase,&IID_IDirectDrawSurface3_Local,&deviceSurface3);
      if ((hresult == 0) &&
         (hresult = deviceSurface3->lpVtbl->QueryInterface
                            (deviceSurface3,&IID_IDirect3DTexture2_Local,&deviceTexture2),
         hresult == 0)) {
        hresult = deviceTexture2->lpVtbl->Load(deviceTexture2,texture->stagingTexture2);
        if (hresult == 0) {
          handleResult = deviceTexture2->lpVtbl->GetHandle
                            (deviceTexture2,g_Direct3DDevice2,&textureHandle);
          if (handleResult == 0) {
            texture->deviceSurfaceBase = deviceSurfaceBase;
            texture->deviceSurface3 = deviceSurface3;
            texture->deviceTexture2 = deviceTexture2;
            texture->textureHandle = textureHandle;
            return;
          }
          /* GetHandle failed: give up without retrying. */
          break;
        }
      }
    }
    /* A creation step or Load failed: release this attempt; on DDERR_OUTOFVIDEOMEMORY evict the oldest device
       texture and retry while eviction succeeds. */
    if (deviceTexture2 != NULL) {
      deviceTexture2->lpVtbl->Release(deviceTexture2);
      deviceTexture2 = NULL;
    }
    if (deviceSurface3 != NULL) {
      deviceSurface3->lpVtbl->Release(deviceSurface3);
      deviceSurface3 = NULL;
    }
    if (deviceSurfaceBase != NULL) {
      deviceSurfaceBase->lpVtbl->Release(deviceSurfaceBase);
      deviceSurfaceBase = NULL;
    }
    /* stop unless out of video memory and an eviction succeeded; the De Morgan form
       (hresult == ... && !Evict(...)) changes the register load order after the loop */
  } while (!((hresult != DDERR_OUTOFVIDEOMEMORY) || GraphicsTexture_EvictOldestDeviceTexture(texture)));
  if (deviceTexture2 != NULL) {
    deviceTexture2->lpVtbl->Release(deviceTexture2);
  }
  if (deviceSurface3 != NULL) {
    deviceSurface3->lpVtbl->Release(deviceSurface3);
  }
  if (deviceSurfaceBase != NULL) {
    deviceSurfaceBase->lpVtbl->Release(deviceSurfaceBase);
  }
  texture->deviceSurfaceBase = NULL;
  texture->deviceSurface3 = NULL;
  texture->deviceTexture2 = NULL;
  texture->textureHandle = 0;
  return;
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
  uint32_t widthLog2;
  int heightLog2;
  GraphicsPaletteTextureSourceAsset *convertedSource;
  GraphicsPaletteTextureSourceAsset *metadataOrError;
  int entryIndex;
  GraphicsPaletteTextureFormatVersion *entryFieldCursor;
  uint8_t *sourceEntry;
  uint32_t convertError;
  uint32_t metadataAllocationError;
  GraphicsAssetAllocationByteSize entriesRemaining;

  convertedSource = (GraphicsPaletteTextureSourceAsset *)sourceAsset;
  convertError = g_GraphicsTextureSourceConvertPaletteEntries(convertedSource);
  metadataOrError = (GraphicsPaletteTextureSourceAsset *)convertError;
  if (convertError == 0) {
    entriesRemaining = convertedSource->subresourceCount;
    /* the set is typed as a palette asset here: magic = sourceAsset, allocationSizeBytes = image count, and
       from formatVersion on eight dwords per entry */
    metadataAllocationError = g_MemoryApi.alloc(entriesRemaining * GRAPHICS_TEXTURE_SET_ENTRY_BYTES + 8,
                                                (void **)&metadataOrError);
    if (metadataAllocationError != 0) {
      metadataOrError = (GraphicsPaletteTextureSourceAsset *)metadataAllocationError;
    }
    else {
      entryFieldCursor = &metadataOrError->formatVersion;
      metadataOrError->magic = (GraphicsPaletteTextureAssetMagic)convertedSource;
      metadataOrError->allocationSizeBytes = entriesRemaining;
      sourceEntry = (uint8_t *)convertedSource + convertedSource->subresourceTableOffset;
      entryIndex = 0;
      while( true ) {
        /* BSR of pixelWidth (source entry +0x18); the original leaves the register undefined for 0 */
        widthLog2 = 31;
        if (((GraphicsTextureSourceEntry *)sourceEntry)->pixelWidth != 0) {
          for (; ((GraphicsTextureSourceEntry *)sourceEntry)->pixelWidth >> widthLog2 == 0; widthLog2--) {
          }
        }
        *entryFieldCursor = 0;
        entryFieldCursor[5] = entryIndex;
        entryFieldCursor[1] = widthLog2;
        if (1 << ((uint8_t)widthLog2 & SHIFT_COUNT_MASK) != ((GraphicsTextureSourceEntry *)sourceEntry)->pixelWidth) break;
        entryFieldCursor[3] = (GraphicsPaletteTextureFormatVersion)convertedSource;
        /* BSR of pixelHeight (source entry +0x1C) */
        heightLog2 = 31;
        if (((GraphicsTextureSourceEntry *)sourceEntry)->pixelHeight != 0) {
          for (; ((GraphicsTextureSourceEntry *)sourceEntry)->pixelHeight >> heightLog2 == 0; heightLog2--) {
          }
        }
        entryFieldCursor[4] = (GraphicsPaletteTextureFormatVersion)sourceEntry;
        entryFieldCursor[2] = heightLog2;
        if (1 << ((uint8_t)heightLog2 & SHIFT_COUNT_MASK) != ((GraphicsTextureSourceEntry *)sourceEntry)->pixelHeight) break;
        entryFieldCursor = entryFieldCursor + 8;
        sourceEntry = sourceEntry + GFX_SUBRESOURCE_RECORD_SIZE;
        entryIndex++;
        entriesRemaining--;
        if (entriesRemaining == 0) {
          return (GraphicsTextureSet *)metadataOrError;
        }
      }
      metadataOrError = (GraphicsPaletteTextureSourceAsset *)FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO;
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = (uint32_t)metadataOrError;
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
  int entryOffset;
  DDPIXELFORMAT *selectedFormat;
  GraphicsTextureSourceAsset *paletteEntryCursor;
  uint8_t *pixelCursor;
  int paletteIndexOrCount;
  
  /* byte offset of the image's GraphicsTextureSourceEntry */
  entryOffset = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
  paletteIndexOrCount =((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->paletteIndex
  ;
  if (paletteIndexOrCount < 0) {
    /* no palette: scan the ARGB pixels */
    pixelCursor= (uint8_t *)sourceAsset + ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->dataOffset;
    paletteIndexOrCount = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->pixelWidth *
            ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->pixelHeight;
    do {
      if (*(uint32_t *)pixelCursor < ARGB8888_ALPHA_MASK) { /* alpha below 0xFF */
        return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
      }
      pixelCursor = pixelCursor + 4;
      paletteIndexOrCount--;
    } while (paletteIndexOrCount != 0);
    selectedFormat = (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0);
  }
  else {
    /* palette bank paletteIndex: 0x400 bytes (256 ARGB colours) each, starting right after the 0x100-byte
       asset header (sourceAsset + 1) */
    paletteEntryCursor = sourceAsset + paletteIndexOrCount * 4 + 1;
    paletteIndexOrCount = 256;
    do {
      if ((paletteEntryCursor->common).magic < ARGB8888_ALPHA_MASK) { /* alpha below 0xFF */
        return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedAlphaTextureFormat,0);
      }
      paletteEntryCursor = (GraphicsTextureSourceAsset *)&(paletteEntryCursor->common).formatVersion;
      paletteIndexOrCount--;
    } while (paletteIndexOrCount != 0);
    selectedFormat = (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedOpaqueTextureFormat,0);
  }
  return selectedFormat;
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

