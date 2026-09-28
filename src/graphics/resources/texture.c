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
#define TEXTURE_SATURATE_TO_BYTE(lane) ((uint32_t)((0xff < (lane)) ? 0xff : (uint8_t)(lane)))

/* Address: 0x0057E970.
   Creates the renderer textures of a texture asset: allocates the set metadata and hands it to the Glide
   backend, or (DirectDraw) creates one texture resource per subresource with its staging and device
   texture and registers it in g_GraphicsTextureSlots. A subresource whose allocation or registration
   fails is left NULL; only a failed metadata allocation fails the call (CF set).
*/
TextureSetResult GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *setSourceAsset;
  uint32_t currentDownsampleShift;
  int adapterIndex;
  GraphicsAdapterRecord *adapters;
  DDPIXELFORMAT *selectedPixelFormat;
  GraphicsTextureResource *newTexture;
  bool registerFailed;
  TextureSetResult allocatedSet;
  ArenaAllocResult textureAllocation;
  TextureSetResult successResult;
  TextureSetResult failureResult;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount entriesRemaining;
  GraphicsSubresourceIndex subresourceIndex;
  
  adapters = g_GraphicsAdapters;
  adapterIndex = g_ActiveGraphicsAdapterIndex;
  allocatedSet = GraphicsTextureSet_AllocateMetadata(sourceAsset);
  if (allocatedSet.failed) {
    failureResult.failed = true;
    failureResult.textureSet = allocatedSet.textureSet;
    return failureResult;
  }
  if (adapters[adapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    allocatedSet.failed = Glide3_TextureSet_CreateBackend(allocatedSet.textureSet,sourceAsset);
    return allocatedSet;
  }
  setSourceAsset = (allocatedSet.textureSet)->sourceAsset;
  entriesRemaining = (setSourceAsset->tableDescriptor).subresourceCount;
  entryCursor = (allocatedSet.textureSet)->entries;
  subresourceIndex = 0;
  do {
    selectedPixelFormat = GraphicsTexture_SelectPixelFormat(subresourceIndex,setSourceAsset);
    textureAllocation = g_MemoryApi.alloc(sizeof(GraphicsTextureResource));
    newTexture = (GraphicsTextureResource *)textureAllocation.payloadOrError;
    if (!textureAllocation.failed) {
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
  successResult.failed = false;
  successResult.textureSet = allocatedSet.textureSet;
  return successResult;
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
   loaded asset is released again and CF is set with the creation error; a failed load returns its own error.
*/
TextureSetResult GraphicsTextureSet_LoadPackage(uint16_t *pathUtf16)

{
  GraphicsTextureSourceAsset *loadedSourceOrError;
  GraphicsTextureSet *createdSetOrError;
  PackageLoadResult loadResult;
  TextureSetResult createResult;

  loadResult = Package_LoadEntry(pathUtf16);
  loadedSourceOrError = loadResult.bufferOrError;
  if (!loadResult.failed) {
    createResult = g_GraphicsCreateTextureSet(loadedSourceOrError);
    createdSetOrError = createResult.textureSet;
    if (!createResult.failed) {
      return createResult;
    }
    Resource_Release(loadedSourceOrError);
    loadedSourceOrError = (GraphicsTextureSourceAsset *)createdSetOrError;
  }
  createResult.failed = true;
  createResult.textureSet = (GraphicsTextureSet *)loadedSourceOrError;
  return createResult;
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
   Returns the logical width (EAX) and height (EDX) of one subresource of a 'gfx' texture source, i.e. the
   extent the tiled blits repeat (installed as g_GraphicsTextureSourceGetLogicalSize). CF is set when the asset
   is not a 'gfx' asset or the index is out of range.
*/
TextureSizeResult GraphicsTextureSource_GetLogicalSizeRegs
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  TextureSizeResult successResult;
  TextureSizeResult failureResult;
  AssetRelativeOffset subresourceTableOffset;

  if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
     (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
    subresourceTableOffset = (sourceAsset->tableDescriptor).subresourceTableOffset;
    successResult.logicalHeightPixels =
         ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + subresourceTableOffset))->logicalHeight;
    successResult.logicalWidthPixels =
         ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + subresourceTableOffset))->logicalWidth;
    successResult.failed = false;
    return successResult;
  }
  /* Failure (CF set): the original leaves EAX/EDX untouched; callers check CF before using the width. */
  failureResult.logicalHeightPixels = 0;
  failureResult.logicalWidthPixels = 0;
  failureResult.failed = true;
  return failureResult;
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
          if (0xffffff <
              *(uint32_t *)(localXOrPixelIndex * 4 +
                        (int)((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->dataOffset +
                        (int)sourceAsset)) {
            return true;
          }
        }
        else if (0xffffff <
                 ((GraphicsPaletteTextureSourceAsset *)sourceAsset)->paletteEntries[paletteIndex * 0x100 + (uint32_t)*(uint8_t *)(localXOrPixelIndex + (int)((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + recordOffset + tableOffset))->dataOffset + (int)sourceAsset)].argb8888) {
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
  bool overflowed;
  TextureSizeResult logicalSize;
  
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
      overflowed = SCARRY4(tileOriginX,tileWidth);
      tileOriginX = tileOriginX + tileWidth;
    } while (tileOriginX == 0 || overflowed != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      overflowed = SCARRY4(tileOriginY,tileHeight);
      tileOriginY = tileOriginY + tileHeight;
    } while (tileOriginY == 0 || overflowed != tileOriginY < 0);
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
    for (; tileX = tileOriginX - tileWidth, tileY < repeatEndY; tileY = tileY + tileHeight) {
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
  bool overflowed;
  TextureSizeResult logicalSize;
  
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
      overflowed = SCARRY4(tileOriginX,tileWidth);
      tileOriginX = tileOriginX + tileWidth;
    } while (tileOriginX == 0 || overflowed != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      overflowed = SCARRY4(tileOriginY,tileHeight);
      tileOriginY = tileOriginY + tileHeight;
    } while (tileOriginY == 0 || overflowed != tileOriginY < 0);
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
    for (; tileX = tileOriginX - tileWidth, tileY < repeatEndY; tileY = tileY + tileHeight) {
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
  bool overflowed;
  TextureSizeResult logicalSize;
  
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
      overflowed = SCARRY4(tileOriginX,tileWidth);
      tileOriginX = tileOriginX + tileWidth;
    } while (tileOriginX == 0 || overflowed != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      overflowed = SCARRY4(tileOriginY,tileHeight);
      tileOriginY = tileOriginY + tileHeight;
    } while (tileOriginY == 0 || overflowed != tileOriginY < 0);
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
    for (; tileX = tileOriginX - tileWidth, tileY < repeatEndY; tileY = tileY + tileHeight) {
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
  bool overflowed;
  TextureSizeResult logicalSize;
  
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
      overflowed = SCARRY4(tileOriginX,tileWidth);
      tileOriginX = tileOriginX + tileWidth;
    } while (tileOriginX == 0 || overflowed != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      overflowed = SCARRY4(tileOriginY,tileHeight);
      tileOriginY = tileOriginY + tileHeight;
    } while (tileOriginY == 0 || overflowed != tileOriginY < 0);
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
    for (; tileX = tileOriginX - tileWidth, tileY < repeatEndY; tileY = tileY + tileHeight) {
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


/* Address: 0x004AC8E0.
   Cuts one subresource of a 'gfx' texture source (a sheet of sprites) into its separate sprites and returns
   them as a new 'gfx' asset, one subresource per sprite (installed as
   g_GraphicsTextureSourceDecomposeSubresourceRegionsCf). The first pixel is the background; each block of
   other pixels (as wide as the run in its first row, as high as the run in its first column) becomes one
   subresource, trimmed of border rows/columns in the colour of the first non-background pixel when that colour
   is transparent, and is then cleared from the work copy. The work area is the largest free arena block,
   shrunk to the result at the end. CF is set with FATAL_ERROR_GFX_ASSET_INVALID, 0x2D (nothing but
   background), FATAL_ERROR_GENERAL_FAILURE (work area too small) or the allocator's error.
*/
TextureSourceDecomposeResult GraphicsTextureSource_DecomposeSubresourceRegions
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  uint16_t *entryCounterField;
  uint8_t backgroundIndex;
  uint8_t edgeIndex;
  int regionCopyBytes;
  uint32_t regionWidth;
  uint32_t regionCopyWidth;
  GraphicsTextureSourceAsset *copySourceOrError;
  uint32_t packedDateTime;
  uint32_t largestBlockSize;
  int remainingBytesOrCount;
  int paletteIndexOrCount;
  int offsetOrColumnCount;
  uint32_t backgroundColorOrCount;
  AssetSubresourceCount entryCount;
  int copyBytesRemaining;
  int fillBytesRemaining;
  uint32_t pixelCountOrCounter;
  uint32_t scanCountOrEdgeColor;
  uint32_t copyRemaining;
  uint32_t fillRemaining;
  int sourceWidthOrTableBytes;
  int entryOffsetOrRows;
  uint32_t fillRows;
  uint8_t *entryOrByteCursor;
  PckConverterVersion pixelDataOffset;
  uint32_t *scanCursor;
  uint32_t *rowCursor;
  GraphicsTextureSourceAsset *copyDestination;
  uint8_t *scanByteCursor;
  uint8_t *probeByteCursor;
  uint8_t *trimByteCursor;
  uint8_t *trimByteProbe;
  uint32_t *probeCursor;
  uint32_t *trimProbe;
  uint32_t *fillCursor;
  bool matchedOrEdgeTransparent;
  bool bytesMatched;
  ArenaShrinkResult shrinkResult;
  TextureSourceDecomposeResult decomposedAsset;
  TextureSourceDecomposeResult successResult;
  TextureSourceDecomposeResult failureResult;
  ArenaLargestAllocResult largestBlock;
  uint32_t *regionStart;
  uint32_t packedPixelBytes;
  uint32_t *packedPixelCursor;
  int freeBytesAfterPacked;
  int rowsRemaining;
  uint32_t *fillRowCursor;
  
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
     ((sourceAsset->tableDescriptor).subresourceCount <= entryIndex)) {
    failureResult.failed = true;
    failureResult.assetOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GFX_ASSET_INVALID;
    return failureResult;
  }
  offsetOrColumnCount = entryIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
  sourceWidthOrTableBytes = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnCount))->pixelWidth;
  rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnCount))->pixelHeight;
  largestBlock = g_MemoryApi.allocLargestFreeBlock();
  largestBlockSize = largestBlock.blockSizeOrSentinel;
  decomposedAsset.assetOrError = (GraphicsTextureSourceAsset *)largestBlock.allocationOrError;
  if (largestBlock.failed) {
    failureResult.failed = true;
    failureResult.assetOrError = decomposedAsset.assetOrError;
    return failureResult;
  }
  /* the new asset starts as a copy of the source header (REP MOVSD) */
  copySourceOrError = sourceAsset;
  copyDestination = decomposedAsset.assetOrError;
  for (remainingBytesOrCount = GFX_ASSET_HEADER_SIZE / 4; remainingBytesOrCount != 0; remainingBytesOrCount--) {
    (copyDestination->common).magic = (copySourceOrError->common).magic;
    copySourceOrError = (GraphicsTextureSourceAsset *)&(copySourceOrError->common).allocationSizeBytes;
    copyDestination = (GraphicsTextureSourceAsset *)&(copyDestination->common).allocationSizeBytes;
  }
  paletteIndexOrCount = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnCount))->paletteIndex;
  copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
  remainingBytesOrCount = largestBlockSize - GFX_ASSET_HEADER_SIZE;
  if (remainingBytesOrCount != 0 && GFX_ASSET_HEADER_SIZE - 1 < (int)largestBlockSize) {
    offsetOrColumnCount = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnCount))->dataOffset;
    /* the new asset is stamped with the current time and date and this computer's name */
    packedDateTime = g_LocaleGetPackedCurrentTime();
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.dateValue1 = packedDateTime;
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.dateValue2 = packedDateTime;
    packedDateTime = g_LocaleGetPackedCurrentDate();
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.timeValue1 = packedDateTime;
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.timeValue2 = packedDateTime;
    g_LocaleCopyDefaultComputerLabelUtf16
              (((decomposedAsset.assetOrError)->common).buildMetadata.names.sourceName);
    entryOrByteCursor = (uint8_t *)sourceAsset + offsetOrColumnCount;
    if (paletteIndexOrCount == -1) {
      /* direct ARGB8888 pixels: no palette, the new subresource table follows the header */
      ((decomposedAsset.assetOrError)->tableDescriptor).paletteBankCount = 0;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount = 0;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceTableOffset = GFX_ASSET_HEADER_SIZE;
      /* the first pixel is the background; the first other pixel gives the edge colour trimmed below */
      backgroundColorOrCount = *(uint32_t *)entryOrByteCursor;
      pixelCountOrCounter = sourceWidthOrTableBytes * rowsRemaining;
      matchedOrEdgeTransparent = true;
      scanCountOrEdgeColor = pixelCountOrCounter;
      scanByteCursor = entryOrByteCursor;
      do {
        probeByteCursor = scanByteCursor;
        if (scanCountOrEdgeColor == 0) break;
        scanCountOrEdgeColor--;
        probeByteCursor = scanByteCursor + 4;
        matchedOrEdgeTransparent = backgroundColorOrCount == *(uint32_t *)scanByteCursor;
        scanByteCursor = probeByteCursor;
      } while (matchedOrEdgeTransparent);
      scanCountOrEdgeColor = *(uint32_t *)(probeByteCursor + -4);
      copySourceOrError = (GraphicsTextureSourceAsset *)0x2d; /* nothing but background */
      if (!matchedOrEdgeTransparent) {
        copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
        /* the working copy of the pixels goes to the end of the work area; the packed sprites grow
           downwards in front of it and the new records upwards behind the header */
        freeBytesAfterPacked = remainingBytesOrCount + pixelCountOrCounter * -4;
        if (freeBytesAfterPacked != 0 && (int)(pixelCountOrCounter * 4) <= remainingBytesOrCount) {
          scanCursor = (uint32_t *)((int)decomposedAsset.assetOrError + freeBytesAfterPacked + GFX_ASSET_HEADER_SIZE);
          matchedOrEdgeTransparent = scanCursor == NULL;
          rowCursor = scanCursor;
          for (pixelCountOrCounter = pixelCountOrCounter & 0x3fffffff; pixelCountOrCounter != 0; pixelCountOrCounter--) {
            *rowCursor = *(uint32_t *)entryOrByteCursor;
            entryOrByteCursor = entryOrByteCursor + 4;
            rowCursor++;
          }
          packedPixelBytes = 0;
          offsetOrColumnCount = sourceWidthOrTableBytes;
          packedPixelCursor = scanCursor;
          /* Scan the working copy row by row for the next non-background pixel; each hit starts a region that is
             trimmed, packed below the work area and cleared to the background color. */
          for (;;) {
            if (offsetOrColumnCount != 0) {
              offsetOrColumnCount--;
              rowCursor = scanCursor + 1;
              matchedOrEdgeTransparent = backgroundColorOrCount == *scanCursor;
              scanCursor = rowCursor;
              if (matchedOrEdgeTransparent) continue;
            }
            if (!matchedOrEdgeTransparent) {
              copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
              remainingBytesOrCount = freeBytesAfterPacked - GFX_SUBRESOURCE_RECORD_SIZE;
              if (remainingBytesOrCount == 0 || freeBytesAfterPacked < GFX_SUBRESOURCE_RECORD_SIZE) {
                goto DecomposeFreeWorkBufferAndFail;
              }
              entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
              scanCursor--;
              offsetOrColumnCount++;
              ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount =
                   ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount + 1;
              entryOffsetOrRows = entryCount * GFX_SUBRESOURCE_RECORD_SIZE;
              matchedOrEdgeTransparent = entryOffsetOrRows == 0;
              paletteIndexOrCount = offsetOrColumnCount;
              rowCursor = scanCursor;
              do {
                probeCursor = rowCursor;
                if (paletteIndexOrCount == 0) break;
                paletteIndexOrCount--;
                probeCursor = rowCursor + 1;
                matchedOrEdgeTransparent = backgroundColorOrCount == *rowCursor;
                rowCursor = probeCursor;
              } while (!matchedOrEdgeTransparent);
              if (matchedOrEdgeTransparent) {
                probeCursor--;
              }
              /* new record: logical width = the run of non-background pixels in this row, logical height = the
                 run in this column, palette index -1; the stored pixels start as the whole block */
              entryOrByteCursor = (uint8_t *)decomposedAsset.assetOrError + GFX_ASSET_HEADER_SIZE + entryOffsetOrRows;
              *(uint32_t *)entryOrByteCursor = (uint32_t)((int)probeCursor - (int)scanCursor) >> 2;
              entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT] = 0;
              entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT + 1] = 0;
              entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT + 2] = 0;
              entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT + 3] = 0;
              *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_X) + 0) = 0;
              *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_X) + 2) = 0;
              *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_Y) + 0) = 0;
              *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_Y) + 2) = 0;
              ((AssetProducerSourceNames *)(entryOrByteCursor + GFX_SUBRESOURCE_PALETTE_INDEX))->producerName[0] = 0xffff;
              ((AssetProducerSourceNames *)(entryOrByteCursor + GFX_SUBRESOURCE_PALETTE_INDEX))->producerName[1] = 0xffff;
              paletteIndexOrCount = rowsRemaining;
              rowCursor = scanCursor;
              do {
                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight + 1;
                rowCursor = rowCursor + sourceWidthOrTableBytes;
                paletteIndexOrCount--;
                matchedOrEdgeTransparent = true;
                if (paletteIndexOrCount == 0) break;
                matchedOrEdgeTransparent = backgroundColorOrCount == *rowCursor;
              } while (!matchedOrEdgeTransparent);
              ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth = *(uint32_t *)entryOrByteCursor;
              ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight;
              /* trim top rows, bottom rows, left and right columns that are all edge colour, but only when the
                 edge colour is transparent (alpha 0); the origin records how much was cut at the top/left */
              rowCursor = scanCursor;
              do {
                pixelCountOrCounter = *(uint32_t *)entryOrByteCursor;
                probeCursor = rowCursor;
                do {
                  if (pixelCountOrCounter == 0) break;
                  pixelCountOrCounter--;
                  matchedOrEdgeTransparent = scanCountOrEdgeColor == *probeCursor;
                  probeCursor++;
                } while (matchedOrEdgeTransparent);
                if ((!matchedOrEdgeTransparent) || (0xffffff < scanCountOrEdgeColor)) goto TrueColorTrimBottomRows;
                rowCursor = rowCursor + sourceWidthOrTableBytes;
                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY + 1;
                entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                *(uint32_t *)entryCounterField = *(uint32_t *)entryCounterField - 1;
                matchedOrEdgeTransparent = *(uint32_t *)entryCounterField == 0;
              } while (!matchedOrEdgeTransparent);
              rowCursor = rowCursor + -sourceWidthOrTableBytes;
              (uint32_t)((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY - 1;
              ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight + 1;
TrueColorTrimBottomRows:
              probeCursor = (uint32_t *)((int)rowCursor + sourceWidthOrTableBytes * 4 * ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight);
              do {
                probeCursor = probeCursor + -sourceWidthOrTableBytes;
                matchedOrEdgeTransparent = probeCursor == NULL;
                pixelCountOrCounter = *(uint32_t *)entryOrByteCursor;
                trimProbe = probeCursor;
                do {
                  if (pixelCountOrCounter == 0) break;
                  pixelCountOrCounter--;
                  matchedOrEdgeTransparent = scanCountOrEdgeColor == *trimProbe;
                  trimProbe++;
                } while (matchedOrEdgeTransparent);
                if ((!matchedOrEdgeTransparent) || (0xffffff < scanCountOrEdgeColor)) goto TrueColorTrimLeftColumns;
                entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                *(uint32_t *)entryCounterField = *(uint32_t *)entryCounterField - 1;
              } while (*(uint32_t *)entryCounterField != 0);
              ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight + 1;
TrueColorTrimLeftColumns:
              pixelCountOrCounter = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
              regionStart = rowCursor;
              probeCursor = rowCursor;
              if ((scanCountOrEdgeColor & 0xff000000) == 0) {
                do {
                  do {
                    regionStart = probeCursor;
                    if (scanCountOrEdgeColor != *rowCursor) goto TrueColorTrimRightColumns;
                    rowCursor = rowCursor + sourceWidthOrTableBytes;
                    pixelCountOrCounter--;
                    probeCursor = regionStart;
                  } while (pixelCountOrCounter != 0);
                  ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX + 1;
                  rowCursor = regionStart + 1;
                  pixelCountOrCounter = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                  entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth;
                  *(uint32_t *)entryCounterField = *(uint32_t *)entryCounterField - 1;
                  probeCursor = rowCursor;
                } while (*(uint32_t *)entryCounterField != 0);
                (uint32_t)((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX - 1;
                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + 1;
              }
TrueColorTrimRightColumns:
              rowCursor = regionStart;
              pixelCountOrCounter = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
              probeCursor = (uint32_t *)((int)regionStart +
                                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth +
                                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth +
                                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + -4);
              regionStart = probeCursor;
              if ((scanCountOrEdgeColor & 0xff000000) == 0) {
                do {
                  do {
                    if (scanCountOrEdgeColor != *probeCursor) goto TrueColorPackRegion;
                    probeCursor = probeCursor + sourceWidthOrTableBytes;
                    pixelCountOrCounter--;
                  } while (pixelCountOrCounter != 0);
                  pixelCountOrCounter = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                  probeCursor = regionStart - 1;
                  entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth;
                  *(uint32_t *)entryCounterField = *(uint32_t *)entryCounterField - 1;
                  regionStart = probeCursor;
                } while (*(uint32_t *)entryCounterField != 0);
                ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + 1;
              }
TrueColorPackRegion:
              /* copy the trimmed pixels in front of the packed ones, then clear the whole block to background */
              pixelCountOrCounter = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
              paletteIndexOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth * pixelCountOrCounter;
              freeBytesAfterPacked = remainingBytesOrCount + paletteIndexOrCount * -4;
              if (freeBytesAfterPacked == 0 || remainingBytesOrCount < paletteIndexOrCount * 4) {
                copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
                goto DecomposeFreeWorkBufferAndFail;
              }
              packedPixelCursor = packedPixelCursor + -paletteIndexOrCount;
              packedPixelBytes = packedPixelBytes + paletteIndexOrCount * 4;
              regionWidth = *(uint32_t *)entryOrByteCursor;
              fillRows = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight;
              regionCopyWidth = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth;
              copyRemaining = regionCopyWidth;
              probeCursor = packedPixelCursor;
              trimProbe = rowCursor;
              do {
                for (; copyRemaining != 0; copyRemaining--) {
                  *probeCursor = *rowCursor;
                  rowCursor++;
                  probeCursor++;
                }
                rowCursor = trimProbe + sourceWidthOrTableBytes;
                pixelCountOrCounter--;
                fillRemaining = regionWidth;
                copyRemaining = regionCopyWidth;
                fillCursor = scanCursor;
                trimProbe = rowCursor;
                fillRowCursor = scanCursor;
              } while (pixelCountOrCounter != 0);
              do {
                for (; fillRemaining != 0; fillRemaining--) {
                  *fillCursor = backgroundColorOrCount;
                  fillCursor++;
                }
                fillRows--;
                fillRemaining = regionWidth;
                fillCursor = fillRowCursor + sourceWidthOrTableBytes;
                fillRowCursor = fillRowCursor + sourceWidthOrTableBytes;
              } while (fillRows != 0);
              matchedOrEdgeTransparent = 0; /* Ghidra: ZF after an ESP adjustment (&stack0x00000000 == 0x40), never set on a real stack */
              continue;
            }
            rowsRemaining--;
            matchedOrEdgeTransparent = rowsRemaining == 0;
            offsetOrColumnCount = sourceWidthOrTableBytes;
            if (matchedOrEdgeTransparent) break;
          }
          /* move the packed pixels down behind the record table, shrink the block to header + table + pixels
             and give every record its pixel offset, counting back from the end */
          sourceWidthOrTableBytes = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
          backgroundColorOrCount = packedPixelBytes >> 2;
          ((decomposedAsset.assetOrError)->common).allocationSizeBytes = packedPixelBytes + sourceWidthOrTableBytes + GFX_ASSET_HEADER_SIZE;
          entryOrByteCursor = (uint8_t *)decomposedAsset.assetOrError + GFX_ASSET_HEADER_SIZE + sourceWidthOrTableBytes;
          for (; backgroundColorOrCount != 0; backgroundColorOrCount--) {
            *(uint32_t *)entryOrByteCursor = *packedPixelCursor;
            packedPixelCursor++;
            entryOrByteCursor = entryOrByteCursor + 4;
          }
          shrinkResult = g_MemoryApi.shrinkInPlace
                             (((decomposedAsset.assetOrError)->common).allocationSizeBytes,
                              decomposedAsset.assetOrError);
          copySourceOrError = (GraphicsTextureSourceAsset *)shrinkResult.scratchOrError;
          if (!shrinkResult.failed) {
            pixelDataOffset = ((decomposedAsset.assetOrError)->common).allocationSizeBytes;
            copySourceOrError = decomposedAsset.assetOrError + 1;
            entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
            /* each record seen through the asset header layout: converterVersion = pixel offset,
               dateValue1/timeValue1 = pixel width/height, dateValue2 = the next record */
            do {
              pixelDataOffset = pixelDataOffset + (copySourceOrError->common).buildMetadata.timestamps.dateValue1 *
                                (copySourceOrError->common).buildMetadata.timestamps.timeValue1 * -4;
              (copySourceOrError->common).converterVersion = pixelDataOffset;
              copySourceOrError = (GraphicsTextureSourceAsset *)
                       &(copySourceOrError->common).buildMetadata.timestamps.dateValue2;
              entryCount--;
            } while (entryCount != 0);
            successResult.failed = false;
            successResult.assetOrError = decomposedAsset.assetOrError;
            return successResult;
          }
        }
      }
    }
    else {
      /* 8-bit palette indices: the new asset carries the subresource's palette bank as its only bank, the
         record table follows it; the scan below is the same as above, on bytes */
      ((decomposedAsset.assetOrError)->tableDescriptor).paletteBankCount = 1;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount = 0;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceTableOffset =
           GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE;
      copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
      offsetOrColumnCount = largestBlockSize - (GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE);
      if (offsetOrColumnCount != 0 && GFX_PALETTE_BANK_SIZE - 1 < remainingBytesOrCount) {
        copySourceOrError = sourceAsset + paletteIndexOrCount * 4 + 1;
        copyDestination = decomposedAsset.assetOrError + 1;
        for (remainingBytesOrCount = GFX_PALETTE_BANK_SIZE / 4; remainingBytesOrCount != 0; remainingBytesOrCount--) {
          (copyDestination->common).magic = (copySourceOrError->common).magic;
          copySourceOrError = (GraphicsTextureSourceAsset *)&(copySourceOrError->common).allocationSizeBytes;
          copyDestination = (GraphicsTextureSourceAsset *)&(copyDestination->common).allocationSizeBytes;
        }
        backgroundIndex = *entryOrByteCursor;
        paletteIndexOrCount = sourceWidthOrTableBytes * rowsRemaining;
        matchedOrEdgeTransparent = true;
        remainingBytesOrCount = paletteIndexOrCount;
        scanByteCursor = entryOrByteCursor;
        do {
          probeByteCursor = scanByteCursor;
          if (remainingBytesOrCount == 0) break;
          remainingBytesOrCount--;
          probeByteCursor = scanByteCursor + 1;
          matchedOrEdgeTransparent = backgroundIndex == *scanByteCursor;
          scanByteCursor = probeByteCursor;
        } while (matchedOrEdgeTransparent);
        edgeIndex = probeByteCursor[-1];
        copySourceOrError = (GraphicsTextureSourceAsset *)0x2d; /* nothing but background */
        if (!matchedOrEdgeTransparent) {
          copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
          remainingBytesOrCount = offsetOrColumnCount - paletteIndexOrCount;
          if (remainingBytesOrCount != 0 && paletteIndexOrCount <= offsetOrColumnCount) {
            scanByteCursor = (uint8_t *)((int)copyDestination + remainingBytesOrCount);
            probeByteCursor = scanByteCursor;
            for (; paletteIndexOrCount != 0; paletteIndexOrCount--) {
              *probeByteCursor = *entryOrByteCursor;
              entryOrByteCursor++;
              probeByteCursor++;
            }
            matchedOrEdgeTransparent = (((GraphicsPaletteTextureSourceAsset *)decomposedAsset.assetOrError)->paletteEntries[(uint32_t)edgeIndex].argb8888 & 0xff000000
                     ) == 0;
            /* the edge colour's entry in the new palette always loses its alpha, i.e. becomes transparent */
            entryOrByteCursor = (uint8_t *)&((GraphicsPaletteTextureSourceAsset *)decomposedAsset.assetOrError)->paletteEntries[(uint32_t)edgeIndex];
            *(uint32_t *)entryOrByteCursor = *(uint32_t *)entryOrByteCursor & 0xffffff;
            /* packed sprites are padded to whole dwords */
            packedPixelCursor = (uint32_t *)((uint32_t)scanByteCursor & 0xfffffffc);
            packedPixelBytes = 0;
            copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
            freeBytesAfterPacked = remainingBytesOrCount - 3;
            bytesMatched = freeBytesAfterPacked == 0;
            offsetOrColumnCount = sourceWidthOrTableBytes;
            if (!bytesMatched && 2 < remainingBytesOrCount) {
              /* Same region scan as the direct-color path, on 8-bit palette indices. */
              for (;;) {
                if (offsetOrColumnCount != 0) {
                  offsetOrColumnCount--;
                  entryOrByteCursor = scanByteCursor + 1;
                  bytesMatched = backgroundIndex == *scanByteCursor;
                  scanByteCursor = entryOrByteCursor;
                  if (bytesMatched) continue;
                }
                if (!bytesMatched) {
                  copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
                  remainingBytesOrCount = freeBytesAfterPacked - GFX_SUBRESOURCE_RECORD_SIZE;
                  if (remainingBytesOrCount == 0 || freeBytesAfterPacked < GFX_SUBRESOURCE_RECORD_SIZE) {
                    goto DecomposeFreeWorkBufferAndFail;
                  }
                  entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
                  scanByteCursor--;
                  ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount =
                       ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount + 1;
                  entryOffsetOrRows = entryCount * GFX_SUBRESOURCE_RECORD_SIZE;
                  bytesMatched = entryOffsetOrRows == 0;
                  paletteIndexOrCount = offsetOrColumnCount + 1;
                  entryOrByteCursor = scanByteCursor;
                  do {
                    probeByteCursor = entryOrByteCursor;
                    if (paletteIndexOrCount == 0) break;
                    paletteIndexOrCount--;
                    probeByteCursor = entryOrByteCursor + 1;
                    bytesMatched = backgroundIndex == *entryOrByteCursor;
                    entryOrByteCursor = probeByteCursor;
                  } while (!bytesMatched);
                  if (bytesMatched) {
                    probeByteCursor--;
                  }
                  /* [5]: GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE bytes in (the table follows the one bank) */
                  entryOrByteCursor = (uint8_t *)&decomposedAsset.assetOrError[5] + entryOffsetOrRows;
                  *(int *)entryOrByteCursor = (int)probeByteCursor - (int)scanByteCursor;
                  entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT] = 0;
                  entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT + 1] = 0;
                  entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT + 2] = 0;
                  entryOrByteCursor[GFX_SUBRESOURCE_LOGICAL_HEIGHT + 3] = 0;
                  *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_X) + 0) = 0;
                  *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_X) + 2) = 0;
                  *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_Y) + 0) = 0;
                  *(uint16_t *)((int)(entryOrByteCursor + GFX_SUBRESOURCE_ORIGIN_Y) + 2) = 0;
                  ((AssetProducerSourceNames *)(entryOrByteCursor + GFX_SUBRESOURCE_PALETTE_INDEX))->producerName[0] = 0;
                  ((AssetProducerSourceNames *)(entryOrByteCursor + GFX_SUBRESOURCE_PALETTE_INDEX))->producerName[1] = 0;
                  paletteIndexOrCount = rowsRemaining;
                  probeByteCursor = scanByteCursor;
                  do {
                    ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight + 1;
                    probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                    paletteIndexOrCount--;
                    bytesMatched = true;
                    if (paletteIndexOrCount == 0) break;
                    bytesMatched = backgroundIndex == *probeByteCursor;
                  } while (!bytesMatched);
                  (int)((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth = *(int *)entryOrByteCursor;
                  ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight;
                  probeByteCursor = scanByteCursor;
                  do {
                    paletteIndexOrCount = *(int *)entryOrByteCursor;
                    trimByteCursor = probeByteCursor;
                    do {
                      if (paletteIndexOrCount == 0) break;
                      paletteIndexOrCount--;
                      bytesMatched = edgeIndex == *trimByteCursor;
                      trimByteCursor++;
                    } while (bytesMatched);
                    if ((!bytesMatched) || (!matchedOrEdgeTransparent)) goto PalettedTrimBottomRows;
                    probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                    ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY + 1;
                    entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                    *(int *)entryCounterField = *(int *)entryCounterField - 1;
                    bytesMatched = *(int *)entryCounterField == 0;
                  } while (!bytesMatched);
                  probeByteCursor = probeByteCursor + -sourceWidthOrTableBytes;
                  ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originY - 1;
                  (int)((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight + 1;
PalettedTrimBottomRows:
                  trimByteCursor = probeByteCursor + sourceWidthOrTableBytes * ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                  do {
                    trimByteCursor = trimByteCursor + -sourceWidthOrTableBytes;
                    bytesMatched = trimByteCursor == NULL;
                    paletteIndexOrCount = *(int *)entryOrByteCursor;
                    trimByteProbe = trimByteCursor;
                    do {
                      if (paletteIndexOrCount == 0) break;
                      paletteIndexOrCount--;
                      bytesMatched = edgeIndex == *trimByteProbe;
                      trimByteProbe++;
                    } while (bytesMatched);
                    if ((!bytesMatched) || (!matchedOrEdgeTransparent)) goto PalettedTrimLeftColumns;
                    entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                    *(int *)entryCounterField = *(int *)entryCounterField - 1;
                  } while (*(int *)entryCounterField != 0);
                  ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight + 1;
PalettedTrimLeftColumns:
                  paletteIndexOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                  regionStart = (uint32_t *)probeByteCursor;
                  scanCursor = (uint32_t *)probeByteCursor;
                  if (matchedOrEdgeTransparent) {
                    do {
                      do {
                        regionStart = scanCursor;
                        if (edgeIndex != *probeByteCursor) goto PalettedTrimRightColumns;
                        probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                        paletteIndexOrCount--;
                        scanCursor = regionStart;
                      } while (paletteIndexOrCount != 0);
                      ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX + 1;
                      probeByteCursor = (uint8_t *)((int)regionStart + 1);
                      paletteIndexOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                      entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth;
                      *(int *)entryCounterField = *(int *)entryCounterField - 1;
                      scanCursor = (uint32_t *)probeByteCursor;
                    } while (*(int *)entryCounterField != 0);
                    ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->originX - 1;
                    (int)((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + 1;
                  }
PalettedTrimRightColumns:
                  scanCursor = regionStart;
                  paletteIndexOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                  probeByteCursor = (uint8_t *)((int)regionStart + ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + -1);
                  regionStart = (uint32_t *)probeByteCursor;
                  if (matchedOrEdgeTransparent) {
                    do {
                      do {
                        if (edgeIndex != *probeByteCursor) goto PalettedPackRegion;
                        probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                        paletteIndexOrCount--;
                      } while (paletteIndexOrCount != 0);
                      paletteIndexOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                      probeByteCursor = (uint8_t *)((int)regionStart + -1);
                      entryCounterField = (uint16_t *)&((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth;
                      *(int *)entryCounterField = *(int *)entryCounterField - 1;
                      regionStart = (uint32_t *)probeByteCursor;
                    } while (*(int *)entryCounterField != 0);
                    ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth + 1;
                  }
PalettedPackRegion:
                  paletteIndexOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelHeight;
                  backgroundColorOrCount = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth * paletteIndexOrCount + 3U & 0xfffffffc;
                  freeBytesAfterPacked = remainingBytesOrCount - backgroundColorOrCount;
                  if (freeBytesAfterPacked == 0 || remainingBytesOrCount < (int)backgroundColorOrCount) {
                    copySourceOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_GENERAL_FAILURE;
                    goto DecomposeFreeWorkBufferAndFail;
                  }
                  packedPixelCursor = (uint32_t *)((int)packedPixelCursor + -backgroundColorOrCount);
                  packedPixelBytes = packedPixelBytes + backgroundColorOrCount;
                  remainingBytesOrCount = *(int *)entryOrByteCursor;
                  entryOffsetOrRows = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->logicalHeight;
                  regionCopyBytes = ((GraphicsTextureSourceEntry *)entryOrByteCursor)->pixelWidth;
                  copyBytesRemaining = regionCopyBytes;
                  entryOrByteCursor = (uint8_t *)packedPixelCursor;
                  rowCursor = scanCursor;
                  do {
                    for (; copyBytesRemaining != 0; copyBytesRemaining--) {
                      *entryOrByteCursor = *(uint8_t *)scanCursor;
                      scanCursor = (uint32_t *)((int)scanCursor + 1);
                      entryOrByteCursor++;
                    }
                    scanCursor = (uint32_t *)((int)rowCursor + sourceWidthOrTableBytes);
                    paletteIndexOrCount--;
                    fillBytesRemaining = remainingBytesOrCount;
                    copyBytesRemaining = regionCopyBytes;
                    probeByteCursor = scanByteCursor;
                    rowCursor = scanCursor;
                    trimByteCursor = scanByteCursor;
                  } while (paletteIndexOrCount != 0);
                  do {
                    for (; fillBytesRemaining != 0; fillBytesRemaining--) {
                      *probeByteCursor = backgroundIndex;
                      probeByteCursor++;
                    }
                    entryOffsetOrRows--;
                    fillBytesRemaining = remainingBytesOrCount;
                    probeByteCursor = trimByteCursor + sourceWidthOrTableBytes;
                    trimByteCursor = trimByteCursor + sourceWidthOrTableBytes;
                  } while (entryOffsetOrRows != 0);
                  bytesMatched = 0; /* Ghidra: ZF after an ESP adjustment (&stack0x00000000 == 0x40), never set on a real stack */
                  offsetOrColumnCount++;
                  continue;
                }
                rowsRemaining--;
                bytesMatched = rowsRemaining == 0;
                offsetOrColumnCount = sourceWidthOrTableBytes;
                if (bytesMatched) break;
              }
              sourceWidthOrTableBytes = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
              backgroundColorOrCount = packedPixelBytes >> 2;
              ((decomposedAsset.assetOrError)->common).allocationSizeBytes =
                   packedPixelBytes + sourceWidthOrTableBytes + (GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE);
              entryOrByteCursor = (uint8_t *)decomposedAsset.assetOrError + (GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE) + sourceWidthOrTableBytes;
              for (; backgroundColorOrCount != 0; backgroundColorOrCount--) {
                *(uint32_t *)entryOrByteCursor = *packedPixelCursor;
                packedPixelCursor = (uint32_t *)((int)packedPixelCursor + 4);
                entryOrByteCursor = entryOrByteCursor + 4;
              }
              shrinkResult = g_MemoryApi.shrinkInPlace
                                 (((decomposedAsset.assetOrError)->common).allocationSizeBytes,
                                  decomposedAsset.assetOrError);
              copySourceOrError = (GraphicsTextureSourceAsset *)shrinkResult.scratchOrError;
              if (!shrinkResult.failed) {
                pixelDataOffset = ((decomposedAsset.assetOrError)->common).allocationSizeBytes;
                copySourceOrError = decomposedAsset.assetOrError + 5;
                entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
                do {
                  pixelDataOffset = pixelDataOffset - ((copySourceOrError->common).buildMetadata.timestamps.dateValue1 *
                                     (copySourceOrError->common).buildMetadata.timestamps.timeValue1 + 3 &
                                    0xfffffffc);
                  (copySourceOrError->common).converterVersion = pixelDataOffset;
                  copySourceOrError = (GraphicsTextureSourceAsset *)
                           &(copySourceOrError->common).buildMetadata.timestamps.dateValue2;
                  entryCount--;
                } while (entryCount != 0);
                decomposedAsset.failed = false;
                return decomposedAsset;
              }
            }
          }
        }
      }
    }
  }
DecomposeFreeWorkBufferAndFail:
  g_MemoryApi.free(decomposedAsset.assetOrError);
  decomposedAsset.assetOrError = copySourceOrError;
  failureResult.failed = true;
  failureResult.assetOrError = decomposedAsset.assetOrError;
  return failureResult;
}

/* Address: 0x004AD630.
   Loads a 'gfx' texture source for the software renderer (installed as g_GraphicsTextureSourceLoadPackageAsset;
   used for the UI, text and selection-panel graphics): the package entry is loaded and its palettes are converted
   to the current framebuffer format. If the conversion fails the entry is released again; CF is set with the
   load or conversion error.
*/
TextureSourceLoadResult GraphicsTextureSource_LoadPackageAsset(uint16_t *pathUtf16)

{
  GraphicsPaletteTextureSourceAsset *loadedSourceOrError;
  GraphicsPaletteTextureSourceAsset *convertedSourceOrError;
  PackageLoadResult loadResult;
  TextureSourceLoadResult convertResult;

  loadResult = Package_LoadEntry(pathUtf16);
  loadedSourceOrError = loadResult.bufferOrError;
  if (!loadResult.failed) {
    convertResult = THANDOR_BITCAST(PaletteTextureSourceResult, TextureSourceLoadResult, g_GraphicsTextureSourceConvertPaletteEntries(loadedSourceOrError));
    convertedSourceOrError = (GraphicsPaletteTextureSourceAsset *)convertResult.textureSource;
    if (!convertResult.failed) {
      return convertResult;
    }
    Resource_Release(loadedSourceOrError);
    loadedSourceOrError = convertedSourceOrError;
  }
  convertResult.failed = true;
  convertResult.textureSource = (GraphicsTextureSourceAsset *)loadedSourceOrError;
  return convertResult;
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
  ArenaAllocResult cloneAllocation;
  PaletteTextureSourceResult convertResult;
  ArenaFreeResult freeResult;

  allocationSizeOrCount = (sourceAsset->common).allocationSizeBytes;
  cloneAllocation = g_MemoryApi.alloc(allocationSizeOrCount);
  clonedAsset = (GraphicsPaletteTextureSourceAsset *)cloneAllocation.payloadOrError;
  if (!cloneAllocation.failed) {
    /* REP MOVSD of the whole allocation */
    cloneCursor = clonedAsset;
    for (allocationSizeOrCount = allocationSizeOrCount >> 2; allocationSizeOrCount != 0; allocationSizeOrCount--) {
      cloneCursor->magic = (sourceAsset->common).magic;
      sourceAsset = (GraphicsTextureSourceAsset *)&(sourceAsset->common).allocationSizeBytes;
      cloneCursor = (GraphicsPaletteTextureSourceAsset *)&cloneCursor->allocationSizeBytes;
    }
    convertResult = g_GraphicsTextureSourceConvertPaletteEntries(clonedAsset);
    if (!convertResult.failed) {
      return (GraphicsTextureSourceAsset *)convertResult.paletteSource;
    }
    freeResult = g_MemoryApi.free(clonedAsset);
    clonedAsset = (GraphicsPaletteTextureSourceAsset *)freeResult.valueOrError;
  }
  return (GraphicsTextureSourceAsset *)clonedAsset;
}


/* Address: 0x004AD6C0.
   Fills the framebuffer-pixel half of every palette entry of a 'gfx' texture source from its ARGB8888 half,
   packed for the current framebuffer format through g_SoftwarePixelPackTables (alpha is kept in the top byte),
   so the software blits can copy palette colours directly (installed as
   g_GraphicsTextureSourceConvertPaletteEntries). CF is set with FATAL_ERROR_GFX_ASSET_INVALID for a NULL or
   non-'gfx' asset.
*/
PaletteTextureSourceResult GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset)

{
  int paletteEntriesRemaining;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
  PaletteTextureSourceResult successResult;
  PaletteTextureSourceResult failureResult;
  uint32_t argb8888;
  
  if ((sourceAsset != NULL) &&
     (sourceAsset->magic == GRAPHICS_PALETTE_TEXTURE_MAGIC_GFX)) {
    paletteEntryCursor = sourceAsset->paletteEntries;
    for (paletteEntriesRemaining = sourceAsset->paletteBankCount << 8; paletteEntriesRemaining != 0;
        paletteEntriesRemaining--) {
      argb8888 = paletteEntryCursor->argb8888;
      /* the red and green shifts yield byte offsets into the dword tables (channel value * 4) */
      paletteEntryCursor->framebufferPixel =
           (argb8888 & 0xff000000) +
           *(int *)((int)g_SoftwarePixelPackTables->red + ((argb8888 & 0xff0000) >> 0xe)) +
           *(int *)((int)g_SoftwarePixelPackTables->green + ((argb8888 & 0xff00) >> 6)) +
           g_SoftwarePixelPackTables->blue[argb8888 & 0xff];
      paletteEntryCursor++;
    }
    successResult.failed = false;
    successResult.paletteSource = sourceAsset;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.paletteSource = (GraphicsPaletteTextureSourceAsset *)FATAL_ERROR_GFX_ASSET_INVALID;
  return failureResult;
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
   Returns the logical width (EAX) and height (EDX) of the first subresource of a 'gfx' texture source whose
   subresource count is 1..4095; CF is set otherwise. No caller or table slot referencing it is known.
*/
TextureSizeResult
GraphicsTextureSource_GetFirstLogicalSizeRegs(GraphicsTextureSourceAsset *sourceAsset)

{
  uint32_t entryCount;
  uint8_t *firstSubresourceRecord;
  TextureSizeResult logicalSize;

  /* Failure (CF set): the original leaves EAX untouched and EDX = sourceAsset; no caller reads them then. */
  logicalSize.logicalWidthPixels = 0;
  logicalSize.logicalHeightPixels = (uint32_t)sourceAsset;
  logicalSize.failed = true;
  if ((sourceAsset->common).magic == ASSET_MAGIC_GFX) {
    entryCount = (sourceAsset->tableDescriptor).subresourceCount;
    if ((entryCount != 0) && (entryCount <= 0xfff)) {
      firstSubresourceRecord =
           (uint8_t *)sourceAsset + (sourceAsset->tableDescriptor).subresourceTableOffset;
      logicalSize.logicalWidthPixels = ((GraphicsTextureSourceEntry *)firstSubresourceRecord)->logicalWidth;
      logicalSize.logicalHeightPixels = ((GraphicsTextureSourceEntry *)firstSubresourceRecord)->logicalHeight;
      logicalSize.failed = false;
    }
  }
  return logicalSize;
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
  GraphicsTextureSourceAsset *paletteSourceCursor;
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
        offsetOrGreenTopBit = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
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
                grayPaletteEntry = THANDOR_BITCAST(int, DirectDrawPaletteEntry, (THANDOR_BITCAST(DirectDrawPaletteEntry, int, grayPaletteEntry) + 0x1010101));
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
                    *destinationWord = (uint16_t)(((maskOrArgb & 0xff000000) >> ((uint8_t)alphaShiftRight & 0x1f)) <<
                                       ((uint8_t)alphaShiftLeft & 0x1f)) |
                               (uint16_t)(((maskOrArgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                                       ((uint8_t)blueShiftLeft & 0x1f)) |
                               (uint16_t)(((maskOrArgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                                       ((uint8_t)greenShiftLeft & 0x1f)) |
                               (uint16_t)(((maskOrArgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                                       ((uint8_t)redShiftLeft & 0x1f));
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
                    *destinationDword = ((maskOrArgb & 0xff000000) >> ((uint8_t)alphaShiftRight & 0x1f)) <<
                               ((uint8_t)alphaShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                               ((uint8_t)blueShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                               ((uint8_t)greenShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                               ((uint8_t)redShiftLeft & 0x1f);
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
              paletteSourceCursor = paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 1);
                paletteBlue = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 2);
                paletteFlags = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 3);
                paletteEntryCursor->red = *(uint8_t *)&(paletteSourceCursor->common).magic;
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor = (GraphicsTextureSourceAsset *)&(paletteSourceCursor->common).formatVersion;
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
                    *destinationWord = (uint16_t)(((maskOrArgb & 0xff000000) >> ((uint8_t)alphaShiftRight & 0x1f)) <<
                                       ((uint8_t)alphaShiftLeft & 0x1f)) |
                               (uint16_t)(((maskOrArgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                                       ((uint8_t)blueShiftLeft & 0x1f)) |
                               (uint16_t)(((maskOrArgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                                       ((uint8_t)greenShiftLeft & 0x1f)) |
                               (uint16_t)(((maskOrArgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                                       ((uint8_t)redShiftLeft & 0x1f));
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
                    *destinationDword = ((maskOrArgb & 0xff000000) >> ((uint8_t)alphaShiftRight & 0x1f)) <<
                               ((uint8_t)alphaShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                               ((uint8_t)blueShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                               ((uint8_t)greenShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                               ((uint8_t)redShiftLeft & 0x1f);
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
  GraphicsTextureSourceAsset *paletteSourceCursor;
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
        offsetOrRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->paletteIndex;
        sourceWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->pixelHeight;
        offsetOrRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrRemaining))->dataOffset;
        if (paletteIndexOrCounter < 0) {
          sourceTexel = (AssetProducerSourceNames *)
                    ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    offsetOrRemaining - GFX_ASSET_ANCHOR28_OFFSET);
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
                grayPaletteEntry = THANDOR_BITCAST(int, DirectDrawPaletteEntry, (THANDOR_BITCAST(DirectDrawPaletteEntry, int, grayPaletteEntry) + 0x1010101));
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & 0x1f)) << ((uint8_t)alphaShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                                       ((uint8_t)blueShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                                       ((uint8_t)greenShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                                       ((uint8_t)redShiftLeft & 0x1f));
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & 0x1f))
                               << ((uint8_t)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                               ((uint8_t)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                               ((uint8_t)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                               ((uint8_t)redShiftLeft & 0x1f);
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
              paletteSourceCursor = paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 1);
                paletteBlue = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 2);
                paletteFlags = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 3);
                paletteEntryCursor->red = *(uint8_t *)&(paletteSourceCursor->common).magic;
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor = (GraphicsTextureSourceAsset *)&(paletteSourceCursor->common).formatVersion;
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & 0x1f)) << ((uint8_t)alphaShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                                       ((uint8_t)blueShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                                       ((uint8_t)greenShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                                       ((uint8_t)redShiftLeft & 0x1f));
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & 0x1f))
                               << ((uint8_t)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                               ((uint8_t)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                               ((uint8_t)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                               ((uint8_t)redShiftLeft & 0x1f);
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
  GraphicsTextureSourceAsset *paletteSourceCursor;
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
        offsetOrGreenTopBit = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrGreenTopBit + (GFX_SUBRESOURCE_PALETTE_INDEX - GFX_ASSET_ANCHOR28_OFFSET));
        sourceWidth = *(uint32_t *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                             offsetOrGreenTopBit + (GFX_SUBRESOURCE_PIXEL_WIDTH - GFX_ASSET_ANCHOR28_OFFSET));
        rowsRemaining = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrGreenTopBit + (GFX_SUBRESOURCE_PIXEL_HEIGHT - GFX_ASSET_ANCHOR28_OFFSET));
        offsetOrGreenTopBit = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrGreenTopBit))->dataOffset;
        if (paletteIndexOrCounter < 0) {
          sourceTexel = (uint16_t *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrGreenTopBit - GFX_ASSET_ANCHOR28_OFFSET);
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
                grayPaletteEntry = THANDOR_BITCAST(int, DirectDrawPaletteEntry, (THANDOR_BITCAST(DirectDrawPaletteEntry, int, grayPaletteEntry) + 0x1010101));
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & 0x1f)) << ((uint8_t)alphaShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                                       ((uint8_t)blueShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                                       ((uint8_t)greenShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                                       ((uint8_t)redShiftLeft & 0x1f));
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & 0x1f))
                               << ((uint8_t)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                               ((uint8_t)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                               ((uint8_t)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                               ((uint8_t)redShiftLeft & 0x1f);
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
          byteCursor = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + offsetOrGreenTopBit - GFX_ASSET_ANCHOR28_OFFSET
          ;
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
              paletteSourceCursor = paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 1);
                paletteBlue = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 2);
                paletteFlags = *(uint8_t *)((int)&(paletteSourceCursor->common).magic + 3);
                paletteEntryCursor->red = *(uint8_t *)&(paletteSourceCursor->common).magic;
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor = (GraphicsTextureSourceAsset *)&(paletteSourceCursor->common).formatVersion;
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
                    texel00 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)*byteCursor * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel01 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[1] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel02 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[sourceWidth] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel03 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[sourceWidth + 1] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel04 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[2] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel05 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[3] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel06 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth + 2] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel07 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth + 3] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    byteCursor = byteCursor + sourceWidth * 2;
                    texel08 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)*byteCursor * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel09 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[1] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel10 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel11 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth + 1] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel12 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[2] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel13 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[3] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel14 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth + 2] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel15 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth + 3] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationWord = (uint16_t)(((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >>
                                        ((uint8_t)alphaShiftRight & 0x1f)) << ((uint8_t)alphaShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                                       ((uint8_t)blueShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                                       ((uint8_t)greenShiftLeft & 0x1f)) |
                               (uint16_t)(((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                                       ((uint8_t)redShiftLeft & 0x1f));
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
                    texel00 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)*byteCursor * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel01 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[1] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel02 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[sourceWidth] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel03 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[sourceWidth + 1] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel04 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[2] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel05 = *(uint32_t *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint32_t)byteCursor[3] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
                    texel06 = *(uint32_t *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint32_t)byteCursor[sourceWidth + 2] * 8 - GFX_ASSET_ANCHOR28_OFFSET);
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
                    averageRgb = TEXTURE_SATURATE_TO_BYTE(averageRed) << 16 | TEXTURE_SATURATE_TO_BYTE(averageGreen) << 8 |
                                 TEXTURE_SATURATE_TO_BYTE(averageBlue);
                    *destinationDword = ((TEXTURE_SATURATE_TO_BYTE(averageAlpha) << 24) >> ((uint8_t)alphaShiftRight & 0x1f))
                               << ((uint8_t)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((uint8_t)blueShiftRight & 0x1f)) <<
                               ((uint8_t)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((uint8_t)greenShiftRight & 0x1f)) <<
                               ((uint8_t)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((uint8_t)redShiftRight & 0x1f)) <<
                               ((uint8_t)redShiftLeft & 0x1f);
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
        offsetOrColumnsRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
        restoreResultOrMaskWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelHeight;
        maskCursor = (uint8_t *)sourceAsset + ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->dataOffset;
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
                rotateShift = alphaShift & 0x1f;
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
                rotateShift = alphaShift & 0x1f;
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
        offsetOrColumnsRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
        restoreResultOrMaskWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelHeight;
        maskCursor = (uint8_t *)sourceAsset + ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->dataOffset;
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
                rotateShift = alphaShift & 0x1f;
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
                rotateShift = alphaShift & 0x1f;
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
        offsetOrColumnsRemaining = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE + (sourceAsset->tableDescriptor).subresourceTableOffset;
        restoreResultOrMaskWidth = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelWidth;
        rowsRemaining = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->pixelHeight;
        maskCursor = (uint8_t *)sourceAsset + ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + offsetOrColumnsRemaining))->dataOffset;
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
                rotateShift = alphaShift & 0x1f;
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
                rotateShift = alphaShift & 0x1f;
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
  bool evictFailed;
  TextureSizeResult logicalSize;
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
  g_SurfaceDesc.dwHeight = largerExtent >> ((uint8_t)g_TextureDownsampleShift & 0x1f);
  effectiveShift = g_TextureDownsampleShift;
  /* textures are at least 16x16: lower the downsample shift until the edge reaches 16 */
  do {
    if (0xf < (int)g_SurfaceDesc.dwHeight) break;
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
  for (;;) {
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
    if ((hresult != DDERR_OUTOFVIDEOMEMORY) ||
        (evictFailed = GraphicsTexture_EvictOldestDeviceTexture(texture), evictFailed)) break;
  }
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
   CF set with the conversion/arena error, or FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO (the set is then not
   freed, as in the original).
*/
TextureSetResult GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset)

{
  uint32_t widthLog2;
  int heightLog2;
  GraphicsPaletteTextureSourceAsset *convertedSource;
  GraphicsPaletteTextureSourceAsset *metadataOrError;
  int entryIndex;
  GraphicsPaletteTextureFormatVersion *entryFieldCursor;
  uint8_t *sourceEntry;
  PaletteTextureSourceResult convertResult;
  ArenaAllocResult metadataAllocation;
  TextureSetResult failureResult;
  GraphicsAssetAllocationByteSize entriesRemaining;
  
  convertResult = g_GraphicsTextureSourceConvertPaletteEntries
                    ((GraphicsPaletteTextureSourceAsset *)sourceAsset);
  convertedSource = convertResult.paletteSource;
  metadataOrError = convertedSource;
  if (!convertResult.failed) {
    entriesRemaining = convertedSource->subresourceCount;
    metadataAllocation = g_MemoryApi.alloc(entriesRemaining * 0x20 + 8);
    /* the set is typed as a palette asset here: magic = sourceAsset, allocationSizeBytes = image count, and
       from formatVersion on eight dwords per entry */
    metadataOrError = (GraphicsPaletteTextureSourceAsset *)metadataAllocation.payloadOrError;
    if (!metadataAllocation.failed) {
      entryFieldCursor = &metadataOrError->formatVersion;
      metadataOrError->magic = (GraphicsPaletteTextureAssetMagic)convertedSource;
      metadataOrError->allocationSizeBytes = entriesRemaining;
      sourceEntry = (uint8_t *)convertedSource + convertedSource->subresourceTableOffset;
      entryIndex = 0;
      while( true ) {
        /* BSR of pixelWidth (source entry +0x18); the original leaves the register undefined for 0 */
        widthLog2 = 0x1f;
        if (((GraphicsTextureSourceEntry *)sourceEntry)->pixelWidth != 0) {
          for (; ((GraphicsTextureSourceEntry *)sourceEntry)->pixelWidth >> widthLog2 == 0; widthLog2--) {
          }
        }
        *entryFieldCursor = 0;
        entryFieldCursor[5] = entryIndex;
        entryFieldCursor[1] = widthLog2;
        if (1 << ((uint8_t)widthLog2 & 0x1f) != ((GraphicsTextureSourceEntry *)sourceEntry)->pixelWidth) break;
        entryFieldCursor[3] = (GraphicsPaletteTextureFormatVersion)convertedSource;
        /* BSR of pixelHeight (source entry +0x1C) */
        heightLog2 = 0x1f;
        if (((GraphicsTextureSourceEntry *)sourceEntry)->pixelHeight != 0) {
          for (; ((GraphicsTextureSourceEntry *)sourceEntry)->pixelHeight >> heightLog2 == 0; heightLog2--) {
          }
        }
        entryFieldCursor[4] = (GraphicsPaletteTextureFormatVersion)sourceEntry;
        entryFieldCursor[2] = heightLog2;
        if (1 << ((uint8_t)heightLog2 & 0x1f) != ((GraphicsTextureSourceEntry *)sourceEntry)->pixelHeight) break;
        entryFieldCursor = entryFieldCursor + 8;
        sourceEntry = sourceEntry + 0x20;
        entryIndex++;
        entriesRemaining--;
        if (entriesRemaining == 0) {
          return THANDOR_BITCAST(uint64_t, TextureSetResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, metadataAllocation) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
      }
      metadataOrError = (GraphicsPaletteTextureSourceAsset *)FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO;
    }
  }
  failureResult.failed = true;
  failureResult.textureSet = (GraphicsTextureSet *)metadataOrError;
  return failureResult;
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
  oldestUsage = 0xffffffff;
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
  entryOffset = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
  paletteIndexOrCount =((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->paletteIndex
  ;
  if (paletteIndexOrCount < 0) {
    /* no palette: scan the ARGB pixels */
    pixelCursor= (uint8_t *)sourceAsset + ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->dataOffset;
    paletteIndexOrCount = ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->pixelWidth *
            ((GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + entryOffset))->pixelHeight;
    do {
      if (*(uint32_t *)pixelCursor < 0xff000000) { /* alpha below 0xFF */
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
      if ((paletteEntryCursor->common).magic < 0xff000000) { /* alpha below 0xFF */
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
  TextureSizeResult logicalSize;
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
    g_SurfaceDesc.dwHeight = largerExtent >> ((uint8_t)g_TextureDownsampleShift & 0x1f);
    effectiveShift = g_TextureDownsampleShift;
    /* textures are at least 16x16: lower the downsample shift until the edge reaches 16 */
    do {
      if (0xf < (int)g_SurfaceDesc.dwHeight) break;
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
    g_BoundTextureHandle = 0xffffffff;
  }
  return;
}

