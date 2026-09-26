/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/texture.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/resources/texture. */

/* Address: 0x0057E970.
   Ownership: graphics/resources/texture.
   Purpose: Allocates texture-set metadata, then creates one runtime texture resource per source entry. DirectDraw
   adapters create staging and device textures. ABI: CF clear means success. CF set means failure.
   Local calls: GraphicsTextureSet_AllocateMetadata, GraphicsTexture_SelectPixelFormat,
   GraphicsTexture_CreateStagingTexture, GraphicsTexture_RegisterSlot, GraphicsTexture_ReleaseObjects,
   GraphicsTexture_CreateDeviceTexture.
   Cross-module calls: Glide3_TextureSet_CreateBackend [graphics/backend/glide].
*/
GraphicsTextureSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *setSourceAsset;
  dword currentDownsampleShift;
  int adapterIndex;
  GraphicsAdapterRecord *adapters;
  DDPIXELFORMAT *selectedPixelFormat;
  GraphicsTextureResource *newTexture;
  bool registerFailed;
  GraphicsTextureSetEaxCf5 allocatedSet;
  ArenaAllocEaxCf5 textureAllocation;
  GraphicsTextureSetEaxCf5 successResult;
  GraphicsTextureSetEaxCf5 failureResult;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount entriesRemaining;
  GraphicsSubresourceIndex subresourceIndex;
  
  adapters = g_GraphicsAdapters;
  adapterIndex = g_ActiveGraphicsAdapterIndex;
  allocatedSet = GraphicsTextureSet_AllocateMetadata(sourceAsset);
  if (allocatedSet.carry) {
    failureResult.carry = true;
    failureResult.textureSet = allocatedSet.textureSet;
    return failureResult;
  }
  if (adapters[adapterIndex].deviceGuid.Data1 == 1) {
    allocatedSet.carry = Glide3_TextureSet_CreateBackend(allocatedSet.textureSet,sourceAsset);
    return allocatedSet;
  }
  setSourceAsset = (allocatedSet.textureSet)->sourceAsset;
  entriesRemaining = (setSourceAsset->tableDescriptor).subresourceCount;
  entryCursor = (allocatedSet.textureSet)->entries;
  subresourceIndex = 0;
  do {
    selectedPixelFormat = GraphicsTexture_SelectPixelFormat(subresourceIndex,setSourceAsset);
    textureAllocation = (*g_MemoryApi.alloc)(0x50);
    newTexture = (GraphicsTextureResource *)textureAllocation.eax;
    if (!textureAllocation.carry) {
      newTexture->stagingTexture2 = (IDirect3DTexture2 *)0x0;
      newTexture->stagingSurface3 = (IDirectDrawSurface3 *)0x0;
      newTexture->stagingSurfaceBase = (IDirectDrawSurface *)0x0;
      entryCursor->texture = newTexture;
      newTexture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
      newTexture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
      newTexture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
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
        (*g_MemoryApi.free)(newTexture);
        entryCursor->texture = (GraphicsTextureResource *)0x0;
      }
      else {
        GraphicsTexture_CreateDeviceTexture(newTexture);
      }
    }
    subresourceIndex = subresourceIndex + 1;
    entryCursor = entryCursor + 1;
    entriesRemaining = entriesRemaining - 1;
  } while (entriesRemaining != 0);
  successResult.carry = false;
  successResult.textureSet = allocatedSet.textureSet;
  return successResult;
}


/* Address: 0x0057AD30.
   Ownership: graphics/resources/texture.
   Purpose: Dispatches all-resource texture rebuilding by selected backend. The Glide marker delegates to
   Glide3_TextureResource_ReinitializeAll. DirectDraw scans all 4096 slots, releases every registered resource's
   six COM objects, and recreates its staging texture. Device textures remain lazy and are recreated on demand.
   Local calls: GraphicsTexture_ReleaseObjects, GraphicsTexture_CreateStagingTexture.
   Cross-module calls: Glide3_TextureResource_ReinitializeAll [graphics/backend/glide].
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsTexture_RebuildAllStagingTextures(void)

{
  GraphicsTextureResource *texture;
  int textureSlotsRemaining;
  GraphicsTextureResource **textureSlotCursor;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureResource_ReinitializeAll();
    return;
  }
  textureSlotsRemaining = 0x1000;
  textureSlotCursor = g_GraphicsTextureSlots;
  do {
    texture = *textureSlotCursor;
    if (texture != (GraphicsTextureResource *)0x0) {
      GraphicsTexture_ReleaseObjects(texture);
      GraphicsTexture_CreateStagingTexture(texture);
    }
    textureSlotCursor = textureSlotCursor + 1;
    textureSlotsRemaining = textureSlotsRemaining + -1;
  } while (textureSlotsRemaining != 0);
  return;
}


/* Address: 0x0057EAF0.
   Ownership: graphics/resources/texture.
   Purpose: Unregisters and releases all runtime texture resources, frees texture-set metadata, and returns the
   owned GraphicsTextureSourceAsset.
   Local calls: GraphicsTexture_ReleaseObjects, GraphicsTextureSet_FreeMetadata.
   Cross-module calls: Glide3_TextureSet_DestroyBackend [graphics/backend/glide].
*/
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
GraphicsTextureSet_Destroy(GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource **slotCursor;
  GraphicsTextureSourceAsset *releasedSourceAsset;
  int slotsRemaining;
  dword entriesRemaining;
  GraphicsTextureSetEntry *entryCursor;
  GraphicsTextureResource **matchedSlot;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    releasedSourceAsset = Glide3_TextureSet_DestroyBackend(set,set);
    return releasedSourceAsset;
  }
  releasedSourceAsset = (GraphicsTextureSourceAsset *)0x0;
  if (set != (GraphicsTextureSet *)0x0) {
    entriesRemaining = set->subresourceCount;
    entryCursor = set->entries;
    do {
      texture = entryCursor->texture;
      if (texture != (GraphicsTextureResource *)0x0) {
        slotsRemaining = 0x1000;
        slotCursor = g_GraphicsTextureSlots;
        do {
          matchedSlot = slotCursor;
          if (texture == *matchedSlot) break;
          slotsRemaining = slotsRemaining + -1;
          slotCursor = matchedSlot + 1;
        } while (slotsRemaining != 0);
        *matchedSlot = (GraphicsTextureResource *)0x0;
        GraphicsTexture_ReleaseObjects(texture);
        (*g_MemoryApi.free)(texture);
      }
      entryCursor = entryCursor + 1;
      entriesRemaining = entriesRemaining - 1;
    } while (entriesRemaining != 0);
    releasedSourceAsset = GraphicsTextureSet_FreeMetadata(set);
  }
  return releasedSourceAsset;
}


/* Address: 0x00485E40.
   Ownership: graphics/resources/texture.
   Purpose: Loads a package-backed gfx source asset, invokes g_GraphicsCreateTextureSet, and returns the created
   set. If texture-set creation fails, the loaded package asset is released. packageContext0 and packageContext1
   are the ECX/EDX Package_LoadEntry context values; their higher-level meaning remains unresolved. ABI: CF clear
   means success. CF set means package loading or texture-set creation failed.
   Cross-module calls: Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
GraphicsTextureSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSet_LoadPackage(word *pathUtf16)

{
  GraphicsTextureSourceAsset *loadedTextureSource;
  GraphicsTextureSet *createdTextureSet;
  PackageLoadEntryEaxCf5 loadResult;
  GraphicsTextureSetEaxCf5 createResult;
  
  loadResult = Package_LoadEntry(pathUtf16);
  loadedTextureSource = loadResult.bufferOrError;
  if (!loadResult.carry) {
    createResult = (*g_GraphicsCreateTextureSet)(loadedTextureSource);
    createdTextureSet = createResult.textureSet;
    if (!createResult.carry) {
      return createResult;
    }
    Resource_Release(loadedTextureSource);
    loadedTextureSource = (GraphicsTextureSourceAsset *)createdTextureSet;
  }
  createResult.carry = true;
  createResult.textureSet = (GraphicsTextureSet *)loadedTextureSource;
  return createResult;
}


/* Address: 0x00485E80.
   Ownership: graphics/resources/texture.
   Purpose: Destroys one texture set through g_GraphicsDestroyTextureSet. That service returns the owned source
   asset, which this wrapper releases through Resource_Release.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *allocation;
  
  allocation = (*g_GraphicsDestroyTextureSet)(set);
  Resource_Release(allocation);
  return;
}


/* Address: 0x00485FC0.
   Ownership: graphics/resources/texture.
   Purpose: Default two-argument refresh implementation used before a hardware backend replaces the service table.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  return;
}


/* Address: 0x00486070.
   Ownership: graphics/resources/texture.
   Purpose: Default no-argument texture-resource rebuild service used before a hardware backend installs the real
   dispatcher.
*/
void __cdecl GraphicsTexture_RebuildNoOp(void)

{
  return;
}

/* Address: 0x004A9270.
   Ownership: graphics/resources/texture.
   Purpose: Validates the source asset and subresource index. EAX returns logicalWidth and EDX returns
   logicalHeight. ABI: CF clear means success. CF set means failure.
*/
GraphicsTextureSizeEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
GraphicsTextureSource_GetLogicalSizeRegs
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  undefined4 in_EAX = 0; /* failure (CF set): EAX/EDX untouched; callers check CF */
  undefined4 in_EDX = 0; /* see in_EAX */
  GraphicsTextureSizeEaxEdxCf9 successResult;
  GraphicsTextureSizeEaxEdxCf9 failureResult;
  AssetRelativeOffset subresourceTableOffset;
  
  if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
     (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
    subresourceTableOffset = (sourceAsset->tableDescriptor).subresourceTableOffset;
    successResult.logicalHeightPixels =
         *(undefined4 *)
          ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
          subresourceIndex * 0x20 + subresourceTableOffset + -0x24);
    successResult.logicalWidthPixels =
         *(undefined4 *)
          ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
          subresourceIndex * 0x20 + subresourceTableOffset + -0x28);
    successResult.carry = false;
    return successResult;
  }
  failureResult.logicalHeightPixels = in_EDX;
  failureResult.logicalWidthPixels = in_EAX;
  failureResult.carry = true;
  return failureResult;
}


/* Address: 0x004A92C0.
   Ownership: graphics/resources/texture.
   Purpose: Converts a screen-space point into source-pixel coordinates using originX/originY, validates it against
   pixelWidth/pixelHeight, then checks the ARGB alpha byte. Direct-color and paletted entries are both supported.
   ABI: CF set means the tested source pixel is opaque/hit. CF clear means transparent, outside the image, or
   invalid input.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsTextureSource_TestOpaquePixel
          (GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  byte *entryField;
  AssetRelativeOffset tableOffset;
  int paletteIndex;
  int localXOrPixelIndex;
  int entryOffset;
  int localY;
  
  if ((drawX <= queryX) && (drawY <= queryY)) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      tableOffset = (sourceAsset->tableDescriptor).subresourceTableOffset;
      entryOffset = subresourceIndex * 0x20;
      entryField = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
               entryOffset + tableOffset + -0x18;
      localXOrPixelIndex = (queryX - drawX) - *(int *)entryField;
      if ((((*(int *)entryField <= queryX - drawX) &&
           (entryField = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     entryOffset + tableOffset + -0x14, localY = (queryY - drawY) - *(int *)entryField,
           *(int *)entryField <= queryY - drawY)) &&
          (localXOrPixelIndex < *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                           entryOffset + tableOffset + -0x10))) &&
         (localY < *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          entryOffset + tableOffset + -0xc))) {
        paletteIndex = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        entryOffset + tableOffset + -0x20);
        localXOrPixelIndex = localY * *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                entryOffset + tableOffset + -0x10) + localXOrPixelIndex;
        if (paletteIndex == -1) {
          if (0xffffff <
              *(uint *)(localXOrPixelIndex * 4 +
                        *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                entryOffset + tableOffset + -0x1c) + (int)sourceAsset)) {
            return true;
          }
        }
        else if (0xffffff <
                 *(uint *)(sourceAsset[paletteIndex * 4 + 1].common.buildMetadata.
                           assetRelativeAddressAnchor28 +
                          (uint)*(byte *)(localXOrPixelIndex + *(int *)((sourceAsset->common).buildMetadata.
                                                           assetRelativeAddressAnchor28 +
                                                          entryOffset + tableOffset + -0x1c) + (int)sourceAsset)
                          * 8 + -0x28)) {
          return true;
        }
      }
    }
  }
  return false;
}


/* Address: 0x004A9A50.
   Ownership: graphics/resources/texture.
   Purpose: Repeats one logical source tile across the requested destination rectangle and calls
   g_GraphicsTextureSourceBlitSourceAlpha for each tile. repeatEndX or repeatEndY equal to INT32_MIN means one
   logical tile extent from the corresponding origin. The first tile origin is moved backward by whole logical tile
   dimensions until it covers the clipping minimum. CF is cleared before return. It selects an existing resource
   facet and does not imply sprite, model, or effect identity.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  dword tileWidth;
  int tileX;
  dword tileHeight;
  int tileY;
  bool overflowed;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  tileHeight = logicalSize.logicalHeightPixels;
  tileWidth = logicalSize.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + tileWidth;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + tileHeight;
  }
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
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}


/* Address: 0x004AA0A0.
   Ownership: graphics/resources/texture.
   Purpose: Repeats one logical source tile and calls g_GraphicsTextureSourceBlitHalfSourceRgb for each tile.
   repeatEndX or repeatEndY equal to INT32_MIN means one logical tile extent from the corresponding origin. The
   first tile origin is moved backward by whole logical tile dimensions until it covers the clipping minimum. CF is
   cleared before return. It selects an existing resource facet and does not imply sprite, model, or effect
   identity.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledHalfSourceRgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  dword tileWidth;
  int tileX;
  dword tileHeight;
  int tileY;
  bool overflowed;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  tileHeight = logicalSize.logicalHeightPixels;
  tileWidth = logicalSize.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + tileWidth;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + tileHeight;
  }
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
        (*g_GraphicsTextureSourceBlitHalfSourceRgb)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}


/* Address: 0x004AB9A0.
   Ownership: graphics/resources/texture.
   Purpose: Repeats one logical source tile and calls g_GraphicsTextureSourceBlitSaturatedAddRgb for each tile.
   repeatEndX or repeatEndY equal to INT32_MIN means one logical tile extent from the corresponding origin. The
   wrapper aligns the first tile backward to cover the clipping minimum. CF is cleared before return. It selects an
   existing resource facet and does not imply sprite, model, or effect identity.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledSaturatedAddRgb
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  dword tileWidth;
  int tileX;
  dword tileHeight;
  int tileY;
  bool overflowed;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  tileHeight = logicalSize.logicalHeightPixels;
  tileWidth = logicalSize.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + tileWidth;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + tileHeight;
  }
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
        (*g_GraphicsTextureSourceBlitSaturatedAddRgb)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}


/* Address: 0x004ABF70.
   Ownership: graphics/resources/texture.
   Purpose: Repeats one logical source tile and calls g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd for each tile.
   repeatEndX or repeatEndY equal to INT32_MIN means one logical tile extent from the corresponding origin. The
   wrapper aligns the first tile backward to cover the clipping minimum. CF is cleared before return. It selects an
   existing resource facet and does not imply sprite, model, or effect identity.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
          GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  dword tileWidth;
  int tileX;
  dword tileHeight;
  int tileY;
  bool overflowed;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  tileHeight = logicalSize.logicalHeightPixels;
  tileWidth = logicalSize.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + tileWidth;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + tileHeight;
  }
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
        (*g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,tileY,tileX,subresourceIndex,sourceAsset,
                   framebuffer);
        tileX = tileX + tileWidth;
      } while (tileX < repeatEndX);
    }
  }
  return;
}


/* Address: 0x004AC8E0.
   Ownership: graphics/resources/texture.
   Purpose: Decomposes texture-source subresource regions and returns the recovered EAX/CF asset result.
*/
GraphicsTextureSourceAssetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSource_DecomposeSubresourceRegionsCf
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  word *entryCounterField;
  byte backgroundIndex;
  byte edgeIndex;
  int regionCopyBytes;
  uint regionWidth;
  uint regionCopyWidth;
  GraphicsTextureSourceAsset *copySourceOrError;
  dword packedDateTime;
  dword largestBlockSize;
  int remainingBytesOrCount;
  int paletteIndexOrCount;
  int offsetOrColumnCount;
  uint backgroundColorOrCount;
  AssetSubresourceCount entryCount;
  int copyBytesRemaining;
  int fillBytesRemaining;
  uint pixelCountOrCounter;
  uint scanCountOrEdgeColor;
  uint copyRemaining;
  uint fillRemaining;
  int sourceWidthOrTableBytes;
  int entryOffsetOrRows;
  uint fillRows;
  byte *entryOrByteCursor;
  PckConverterVersion pixelDataOffset;
  uint *scanCursor;
  uint *rowCursor;
  GraphicsTextureSourceAsset *copyDestination;
  byte *scanByteCursor;
  byte *probeByteCursor;
  byte *trimByteCursor;
  byte *trimByteProbe;
  uint *probeCursor;
  uint *trimProbe;
  uint *fillCursor;
  bool matchedOrEdgeTransparent;
  bool bytesMatched;
  ArenaShrinkEaxCf5 shrinkResult;
  GraphicsTextureSourceAssetEaxCf5 decomposedAsset;
  GraphicsTextureSourceAssetEaxCf5 successResult;
  GraphicsTextureSourceAssetEaxCf5 failureResult;
  ArenaLargestAllocationEaxEcxCf9 largestBlock;
  uint *regionStart;
  uint packedPixelBytes;
  uint *packedPixelCursor;
  int freeBytesAfterPacked;
  int rowsRemaining;
  uint *fillRowCursor;
  
  decomposedAsset.assetOrError = (GraphicsTextureSourceAsset *)0x2c;
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
     ((sourceAsset->tableDescriptor).subresourceCount <= entryIndex)) goto LAB_004ad0f7;
  offsetOrColumnCount = entryIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
  sourceWidthOrTableBytes = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                   offsetOrColumnCount + -0x10);
  rowsRemaining = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                      offsetOrColumnCount + -0xc);
  largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
  largestBlockSize = largestBlock.blockSizeOrSentinel;
  decomposedAsset.assetOrError = (GraphicsTextureSourceAsset *)largestBlock.allocationOrError;
  if (largestBlock.carry) goto LAB_004ad0f7;
  copySourceOrError = sourceAsset;
  copyDestination = decomposedAsset.assetOrError;
  for (remainingBytesOrCount = 0x80; remainingBytesOrCount != 0; remainingBytesOrCount = remainingBytesOrCount + -1) {
    (copyDestination->common).magic = (copySourceOrError->common).magic;
    copySourceOrError = (GraphicsTextureSourceAsset *)&(copySourceOrError->common).allocationSizeBytes;
    copyDestination = (GraphicsTextureSourceAsset *)&(copyDestination->common).allocationSizeBytes;
  }
  paletteIndexOrCount = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                   offsetOrColumnCount + -0x20);
  copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
  remainingBytesOrCount = largestBlockSize - 0x200;
  if (remainingBytesOrCount != 0 && 0x1ff < (int)largestBlockSize) {
    offsetOrColumnCount = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     offsetOrColumnCount + -0x1c);
    packedDateTime = (*g_LocaleGetPackedCurrentTime)();
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.dateValue1 = packedDateTime;
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.dateValue2 = packedDateTime;
    packedDateTime = (*g_LocaleGetPackedCurrentDate)();
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.timeValue1 = packedDateTime;
    ((decomposedAsset.assetOrError)->common).buildMetadata.timestamps.timeValue2 = packedDateTime;
    (*g_LocaleCopyDefaultComputerLabelUtf16)
              (((decomposedAsset.assetOrError)->common).buildMetadata.names.sourceName);
    entryOrByteCursor = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + offsetOrColumnCount + -0x28;
    if (paletteIndexOrCount == -1) {
      ((decomposedAsset.assetOrError)->tableDescriptor).paletteBankCount = 0;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount = 0;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceTableOffset = 0x200;
      backgroundColorOrCount = *(uint *)entryOrByteCursor;
      pixelCountOrCounter = sourceWidthOrTableBytes * rowsRemaining;
      matchedOrEdgeTransparent = true;
      scanCountOrEdgeColor = pixelCountOrCounter;
      scanByteCursor = entryOrByteCursor;
      do {
        probeByteCursor = scanByteCursor;
        if (scanCountOrEdgeColor == 0) break;
        scanCountOrEdgeColor = scanCountOrEdgeColor - 1;
        probeByteCursor = scanByteCursor + 4;
        matchedOrEdgeTransparent = backgroundColorOrCount == *(uint *)scanByteCursor;
        scanByteCursor = probeByteCursor;
      } while (matchedOrEdgeTransparent);
      scanCountOrEdgeColor = *(uint *)(probeByteCursor + -4);
      copySourceOrError = (GraphicsTextureSourceAsset *)0x2d;
      if (!matchedOrEdgeTransparent) {
        copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
        freeBytesAfterPacked = remainingBytesOrCount + pixelCountOrCounter * -4;
        if (freeBytesAfterPacked != 0 && (int)(pixelCountOrCounter * 4) <= remainingBytesOrCount) {
          scanCursor = (uint *)((int)decomposedAsset.assetOrError + freeBytesAfterPacked + 0x200);
          matchedOrEdgeTransparent = scanCursor == (uint *)0x0;
          rowCursor = scanCursor;
          for (pixelCountOrCounter = pixelCountOrCounter & 0x3fffffff; pixelCountOrCounter != 0; pixelCountOrCounter = pixelCountOrCounter - 1) {
            *rowCursor = *(uint *)entryOrByteCursor;
            entryOrByteCursor = entryOrByteCursor + 4;
            rowCursor = rowCursor + 1;
          }
          packedPixelBytes = 0;
          offsetOrColumnCount = sourceWidthOrTableBytes;
          packedPixelCursor = scanCursor;
code_r0x004ace04:
          do {
            if (offsetOrColumnCount != 0) {
              offsetOrColumnCount = offsetOrColumnCount + -1;
              rowCursor = scanCursor + 1;
              matchedOrEdgeTransparent = backgroundColorOrCount == *scanCursor;
              scanCursor = rowCursor;
              if (matchedOrEdgeTransparent) goto code_r0x004ace04;
            }
            if (!matchedOrEdgeTransparent) {
              copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
              remainingBytesOrCount = freeBytesAfterPacked + -0x20;
              if (remainingBytesOrCount == 0 || freeBytesAfterPacked < 0x20) goto LAB_004ad0ec;
              entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
              scanCursor = scanCursor + -1;
              offsetOrColumnCount = offsetOrColumnCount + 1;
              ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount =
                   ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount + 1;
              entryOffsetOrRows = entryCount * 0x20;
              matchedOrEdgeTransparent = entryOffsetOrRows == 0;
              paletteIndexOrCount = offsetOrColumnCount;
              rowCursor = scanCursor;
              do {
                probeCursor = rowCursor;
                if (paletteIndexOrCount == 0) break;
                paletteIndexOrCount = paletteIndexOrCount + -1;
                probeCursor = rowCursor + 1;
                matchedOrEdgeTransparent = backgroundColorOrCount == *rowCursor;
                rowCursor = probeCursor;
              } while (!matchedOrEdgeTransparent);
              if (matchedOrEdgeTransparent) {
                probeCursor = probeCursor + -1;
              }
              entryOrByteCursor = decomposedAsset.assetOrError[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                        entryOffsetOrRows + -0x28;
              *(uint *)entryOrByteCursor = (uint)((int)probeCursor - (int)scanCursor) >> 2;
              entryOrByteCursor[4] = 0;
              entryOrByteCursor[5] = 0;
              entryOrByteCursor[6] = 0;
              entryOrByteCursor[7] = 0;
              *(word *)((int)(entryOrByteCursor + 0x10) + 0) = 0;
              *(word *)((int)(entryOrByteCursor + 0x10) + 2) = 0;
              *(word *)((int)(entryOrByteCursor + 0x14) + 0) = 0;
              *(word *)((int)(entryOrByteCursor + 0x14) + 2) = 0;
              ((AssetProducerSourceNames *)(entryOrByteCursor + 8))->producerName[0] = 0xffff;
              ((AssetProducerSourceNames *)(entryOrByteCursor + 8))->producerName[1] = 0xffff;
              paletteIndexOrCount = rowsRemaining;
              rowCursor = scanCursor;
              do {
                *(uint *)(entryOrByteCursor + 4) = *(uint *)(entryOrByteCursor + 4) + 1;
                rowCursor = rowCursor + sourceWidthOrTableBytes;
                paletteIndexOrCount = paletteIndexOrCount + -1;
                matchedOrEdgeTransparent = true;
                if (paletteIndexOrCount == 0) break;
                matchedOrEdgeTransparent = backgroundColorOrCount == *rowCursor;
              } while (!matchedOrEdgeTransparent);
              *(uint *)(entryOrByteCursor + 0x18) = *(uint *)entryOrByteCursor;
              *(uint *)(entryOrByteCursor + 0x1c) = *(uint *)(entryOrByteCursor + 4);
              rowCursor = scanCursor;
              do {
                pixelCountOrCounter = *(uint *)entryOrByteCursor;
                probeCursor = rowCursor;
                do {
                  if (pixelCountOrCounter == 0) break;
                  pixelCountOrCounter = pixelCountOrCounter - 1;
                  matchedOrEdgeTransparent = scanCountOrEdgeColor == *probeCursor;
                  probeCursor = probeCursor + 1;
                } while (matchedOrEdgeTransparent);
                if ((!matchedOrEdgeTransparent) || (0xffffff < scanCountOrEdgeColor)) goto LAB_004acf82;
                rowCursor = rowCursor + sourceWidthOrTableBytes;
                *(uint *)(entryOrByteCursor + 0x14) = *(uint *)(entryOrByteCursor + 0x14) + 1;
                entryCounterField = (word *)(entryOrByteCursor + 0x1c);
                *(uint *)entryCounterField = *(uint *)entryCounterField - 1;
                matchedOrEdgeTransparent = *(uint *)entryCounterField == 0;
              } while (!matchedOrEdgeTransparent);
              rowCursor = rowCursor + -sourceWidthOrTableBytes;
              *(uint *)(entryOrByteCursor + 0x14) = *(uint *)(entryOrByteCursor + 0x14) - 1;
              *(uint *)(entryOrByteCursor + 0x1c) = *(uint *)(entryOrByteCursor + 0x1c) + 1;
LAB_004acf82:
              probeCursor = (uint *)((int)rowCursor + sourceWidthOrTableBytes * 4 * *(uint *)(entryOrByteCursor + 0x1c));
              do {
                probeCursor = probeCursor + -sourceWidthOrTableBytes;
                matchedOrEdgeTransparent = probeCursor == (uint *)0x0;
                pixelCountOrCounter = *(uint *)entryOrByteCursor;
                trimProbe = probeCursor;
                do {
                  if (pixelCountOrCounter == 0) break;
                  pixelCountOrCounter = pixelCountOrCounter - 1;
                  matchedOrEdgeTransparent = scanCountOrEdgeColor == *trimProbe;
                  trimProbe = trimProbe + 1;
                } while (matchedOrEdgeTransparent);
                if ((!matchedOrEdgeTransparent) || (0xffffff < scanCountOrEdgeColor)) goto LAB_004acfc4;
                entryCounterField = (word *)(entryOrByteCursor + 0x1c);
                *(uint *)entryCounterField = *(uint *)entryCounterField - 1;
              } while (*(uint *)entryCounterField != 0);
              *(uint *)(entryOrByteCursor + 0x1c) = *(uint *)(entryOrByteCursor + 0x1c) + 1;
LAB_004acfc4:
              pixelCountOrCounter = *(uint *)(entryOrByteCursor + 0x1c);
              regionStart = rowCursor;
              probeCursor = rowCursor;
              if ((scanCountOrEdgeColor & 0xff000000) == 0) {
                do {
                  do {
                    regionStart = probeCursor;
                    if (scanCountOrEdgeColor != *rowCursor) goto LAB_004ad012;
                    rowCursor = rowCursor + sourceWidthOrTableBytes;
                    pixelCountOrCounter = pixelCountOrCounter - 1;
                    probeCursor = regionStart;
                  } while (pixelCountOrCounter != 0);
                  *(uint *)(entryOrByteCursor + 0x10) = *(uint *)(entryOrByteCursor + 0x10) + 1;
                  rowCursor = regionStart + 1;
                  pixelCountOrCounter = *(uint *)(entryOrByteCursor + 0x1c);
                  entryCounterField = (word *)(entryOrByteCursor + 0x18);
                  *(uint *)entryCounterField = *(uint *)entryCounterField - 1;
                  probeCursor = rowCursor;
                } while (*(uint *)entryCounterField != 0);
                *(uint *)(entryOrByteCursor + 0x10) = *(uint *)(entryOrByteCursor + 0x10) - 1;
                *(uint *)(entryOrByteCursor + 0x18) = *(uint *)(entryOrByteCursor + 0x18) + 1;
              }
LAB_004ad012:
              rowCursor = regionStart;
              pixelCountOrCounter = *(uint *)(entryOrByteCursor + 0x1c);
              probeCursor = (uint *)((int)regionStart +
                                *(uint *)(entryOrByteCursor + 0x18) +
                                *(uint *)(entryOrByteCursor + 0x18) +
                                *(uint *)(entryOrByteCursor + 0x18) + *(uint *)(entryOrByteCursor + 0x18) + -4);
              regionStart = probeCursor;
              if ((scanCountOrEdgeColor & 0xff000000) == 0) {
                do {
                  do {
                    if (scanCountOrEdgeColor != *probeCursor) goto LAB_004ad066;
                    probeCursor = probeCursor + sourceWidthOrTableBytes;
                    pixelCountOrCounter = pixelCountOrCounter - 1;
                  } while (pixelCountOrCounter != 0);
                  pixelCountOrCounter = *(uint *)(entryOrByteCursor + 0x1c);
                  probeCursor = regionStart + -1;
                  entryCounterField = (word *)(entryOrByteCursor + 0x18);
                  *(uint *)entryCounterField = *(uint *)entryCounterField - 1;
                  regionStart = probeCursor;
                } while (*(uint *)entryCounterField != 0);
                *(uint *)(entryOrByteCursor + 0x18) = *(uint *)(entryOrByteCursor + 0x18) + 1;
              }
LAB_004ad066:
              pixelCountOrCounter = *(uint *)(entryOrByteCursor + 0x1c);
              paletteIndexOrCount = *(uint *)(entryOrByteCursor + 0x18) * pixelCountOrCounter;
              freeBytesAfterPacked = remainingBytesOrCount + paletteIndexOrCount * -4;
              if (freeBytesAfterPacked == 0 || remainingBytesOrCount < paletteIndexOrCount * 4) goto LAB_004ad0e9;
              packedPixelCursor = packedPixelCursor + -paletteIndexOrCount;
              packedPixelBytes = packedPixelBytes + paletteIndexOrCount * 4;
              regionWidth = *(uint *)entryOrByteCursor;
              fillRows = *(uint *)(entryOrByteCursor + 4);
              regionCopyWidth = *(uint *)(entryOrByteCursor + 0x18);
              copyRemaining = regionCopyWidth;
              probeCursor = packedPixelCursor;
              trimProbe = rowCursor;
              do {
                for (; copyRemaining != 0; copyRemaining = copyRemaining - 1) {
                  *probeCursor = *rowCursor;
                  rowCursor = rowCursor + 1;
                  probeCursor = probeCursor + 1;
                }
                rowCursor = trimProbe + sourceWidthOrTableBytes;
                pixelCountOrCounter = pixelCountOrCounter - 1;
                fillRemaining = regionWidth;
                copyRemaining = regionCopyWidth;
                fillCursor = scanCursor;
                trimProbe = rowCursor;
                fillRowCursor = scanCursor;
              } while (pixelCountOrCounter != 0);
              do {
                for (; fillRemaining != 0; fillRemaining = fillRemaining - 1) {
                  *fillCursor = backgroundColorOrCount;
                  fillCursor = fillCursor + 1;
                }
                fillRows = fillRows - 1;
                fillRemaining = regionWidth;
                fillCursor = fillRowCursor + sourceWidthOrTableBytes;
                fillRowCursor = fillRowCursor + sourceWidthOrTableBytes;
              } while (fillRows != 0);
              matchedOrEdgeTransparent = 0; /* Ghidra: ZF after an ESP adjustment (&stack0x00000000 == 0x40), never set on a real stack */
              goto code_r0x004ace04;
            }
            rowsRemaining = rowsRemaining + -1;
            matchedOrEdgeTransparent = rowsRemaining == 0;
            offsetOrColumnCount = sourceWidthOrTableBytes;
          } while (!matchedOrEdgeTransparent);
          sourceWidthOrTableBytes = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount * 0x20;
          backgroundColorOrCount = packedPixelBytes >> 2;
          ((decomposedAsset.assetOrError)->common).allocationSizeBytes = packedPixelBytes + sourceWidthOrTableBytes + 0x200;
          entryOrByteCursor = decomposedAsset.assetOrError[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                    sourceWidthOrTableBytes + -0x28;
          for (; backgroundColorOrCount != 0; backgroundColorOrCount = backgroundColorOrCount - 1) {
            *(uint *)entryOrByteCursor = *packedPixelCursor;
            packedPixelCursor = packedPixelCursor + 1;
            entryOrByteCursor = entryOrByteCursor + 4;
          }
          shrinkResult = (*g_MemoryApi.shrinkInPlace)
                             (((decomposedAsset.assetOrError)->common).allocationSizeBytes,
                              decomposedAsset.assetOrError);
          copySourceOrError = (GraphicsTextureSourceAsset *)shrinkResult.scratchOrError;
          if (!shrinkResult.carry) {
            pixelDataOffset = ((decomposedAsset.assetOrError)->common).allocationSizeBytes;
            copySourceOrError = decomposedAsset.assetOrError + 1;
            entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
            do {
              pixelDataOffset = pixelDataOffset + (copySourceOrError->common).buildMetadata.timestamps.dateValue1 *
                                (copySourceOrError->common).buildMetadata.timestamps.timeValue1 * -4;
              (copySourceOrError->common).converterVersion = pixelDataOffset;
              copySourceOrError = (GraphicsTextureSourceAsset *)
                       &(copySourceOrError->common).buildMetadata.timestamps.dateValue2;
              entryCount = entryCount - 1;
            } while (entryCount != 0);
            successResult.carry = false;
            successResult.assetOrError = decomposedAsset.assetOrError;
            return successResult;
          }
        }
      }
    }
    else {
      ((decomposedAsset.assetOrError)->tableDescriptor).paletteBankCount = 1;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount = 0;
      ((decomposedAsset.assetOrError)->tableDescriptor).subresourceTableOffset = 0xa00;
      copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
      offsetOrColumnCount = largestBlockSize - 0xa00;
      if (offsetOrColumnCount != 0 && 0x7ff < remainingBytesOrCount) {
        copySourceOrError = sourceAsset + paletteIndexOrCount * 4 + 1;
        copyDestination = decomposedAsset.assetOrError + 1;
        for (remainingBytesOrCount = 0x200; remainingBytesOrCount != 0; remainingBytesOrCount = remainingBytesOrCount + -1) {
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
          remainingBytesOrCount = remainingBytesOrCount + -1;
          probeByteCursor = scanByteCursor + 1;
          matchedOrEdgeTransparent = backgroundIndex == *scanByteCursor;
          scanByteCursor = probeByteCursor;
        } while (matchedOrEdgeTransparent);
        edgeIndex = probeByteCursor[-1];
        copySourceOrError = (GraphicsTextureSourceAsset *)0x2d;
        if (!matchedOrEdgeTransparent) {
          copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
          remainingBytesOrCount = offsetOrColumnCount - paletteIndexOrCount;
          if (remainingBytesOrCount != 0 && paletteIndexOrCount <= offsetOrColumnCount) {
            scanByteCursor = (byte *)((int)copyDestination + remainingBytesOrCount);
            probeByteCursor = scanByteCursor;
            for (; paletteIndexOrCount != 0; paletteIndexOrCount = paletteIndexOrCount + -1) {
              *probeByteCursor = *entryOrByteCursor;
              entryOrByteCursor = entryOrByteCursor + 1;
              probeByteCursor = probeByteCursor + 1;
            }
            matchedOrEdgeTransparent = (*(uint *)(decomposedAsset.assetOrError[1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)edgeIndex * 8 + -0x28) & 0xff000000
                     ) == 0;
            entryOrByteCursor = decomposedAsset.assetOrError[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                      (uint)edgeIndex * 8 + -0x28;
            *(uint *)entryOrByteCursor = *(uint *)entryOrByteCursor & 0xffffff;
            packedPixelCursor = (uint *)((uint)scanByteCursor & 0xfffffffc);
            packedPixelBytes = 0;
            copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
            freeBytesAfterPacked = remainingBytesOrCount + -3;
            bytesMatched = freeBytesAfterPacked == 0;
            offsetOrColumnCount = sourceWidthOrTableBytes;
            if (!bytesMatched && 2 < remainingBytesOrCount) {
code_r0x004acaa4:
              do {
                if (offsetOrColumnCount != 0) {
                  offsetOrColumnCount = offsetOrColumnCount + -1;
                  entryOrByteCursor = scanByteCursor + 1;
                  bytesMatched = backgroundIndex == *scanByteCursor;
                  scanByteCursor = entryOrByteCursor;
                  if (bytesMatched) goto code_r0x004acaa4;
                }
                if (!bytesMatched) {
                  copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
                  remainingBytesOrCount = freeBytesAfterPacked + -0x20;
                  if (remainingBytesOrCount == 0 || freeBytesAfterPacked < 0x20) goto LAB_004ad0ec;
                  entryCount = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount;
                  scanByteCursor = scanByteCursor + -1;
                  ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount =
                       ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount + 1;
                  entryOffsetOrRows = entryCount * 0x20;
                  bytesMatched = entryOffsetOrRows == 0;
                  paletteIndexOrCount = offsetOrColumnCount + 1;
                  entryOrByteCursor = scanByteCursor;
                  do {
                    probeByteCursor = entryOrByteCursor;
                    if (paletteIndexOrCount == 0) break;
                    paletteIndexOrCount = paletteIndexOrCount + -1;
                    probeByteCursor = entryOrByteCursor + 1;
                    bytesMatched = backgroundIndex == *entryOrByteCursor;
                    entryOrByteCursor = probeByteCursor;
                  } while (!bytesMatched);
                  if (bytesMatched) {
                    probeByteCursor = probeByteCursor + -1;
                  }
                  entryOrByteCursor = decomposedAsset.assetOrError[5].common.buildMetadata.assetRelativeAddressAnchor28
                            + entryOffsetOrRows + -0x28;
                  *(int *)entryOrByteCursor = (int)probeByteCursor - (int)scanByteCursor;
                  entryOrByteCursor[4] = 0;
                  entryOrByteCursor[5] = 0;
                  entryOrByteCursor[6] = 0;
                  entryOrByteCursor[7] = 0;
                  *(word *)((int)(entryOrByteCursor + 0x10) + 0) = 0;
                  *(word *)((int)(entryOrByteCursor + 0x10) + 2) = 0;
                  *(word *)((int)(entryOrByteCursor + 0x14) + 0) = 0;
                  *(word *)((int)(entryOrByteCursor + 0x14) + 2) = 0;
                  ((AssetProducerSourceNames *)(entryOrByteCursor + 8))->producerName[0] = 0;
                  ((AssetProducerSourceNames *)(entryOrByteCursor + 8))->producerName[1] = 0;
                  paletteIndexOrCount = rowsRemaining;
                  probeByteCursor = scanByteCursor;
                  do {
                    *(int *)(entryOrByteCursor + 4) = *(int *)(entryOrByteCursor + 4) + 1;
                    probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                    paletteIndexOrCount = paletteIndexOrCount + -1;
                    bytesMatched = true;
                    if (paletteIndexOrCount == 0) break;
                    bytesMatched = backgroundIndex == *probeByteCursor;
                  } while (!bytesMatched);
                  *(int *)(entryOrByteCursor + 0x18) = *(int *)entryOrByteCursor;
                  *(int *)(entryOrByteCursor + 0x1c) = *(int *)(entryOrByteCursor + 4);
                  probeByteCursor = scanByteCursor;
                  do {
                    paletteIndexOrCount = *(int *)entryOrByteCursor;
                    trimByteCursor = probeByteCursor;
                    do {
                      if (paletteIndexOrCount == 0) break;
                      paletteIndexOrCount = paletteIndexOrCount + -1;
                      bytesMatched = edgeIndex == *trimByteCursor;
                      trimByteCursor = trimByteCursor + 1;
                    } while (bytesMatched);
                    if ((!bytesMatched) || (!matchedOrEdgeTransparent)) goto LAB_004acc23;
                    probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                    *(int *)(entryOrByteCursor + 0x14) = *(int *)(entryOrByteCursor + 0x14) + 1;
                    entryCounterField = (word *)(entryOrByteCursor + 0x1c);
                    *(int *)entryCounterField = *(int *)entryCounterField + -1;
                    bytesMatched = *(int *)entryCounterField == 0;
                  } while (!bytesMatched);
                  probeByteCursor = probeByteCursor + -sourceWidthOrTableBytes;
                  *(int *)(entryOrByteCursor + 0x14) = *(int *)(entryOrByteCursor + 0x14) + -1;
                  *(int *)(entryOrByteCursor + 0x1c) = *(int *)(entryOrByteCursor + 0x1c) + 1;
LAB_004acc23:
                  trimByteCursor = probeByteCursor + sourceWidthOrTableBytes * *(int *)(entryOrByteCursor + 0x1c);
                  do {
                    trimByteCursor = trimByteCursor + -sourceWidthOrTableBytes;
                    bytesMatched = trimByteCursor == (byte *)0x0;
                    paletteIndexOrCount = *(int *)entryOrByteCursor;
                    trimByteProbe = trimByteCursor;
                    do {
                      if (paletteIndexOrCount == 0) break;
                      paletteIndexOrCount = paletteIndexOrCount + -1;
                      bytesMatched = edgeIndex == *trimByteProbe;
                      trimByteProbe = trimByteProbe + 1;
                    } while (bytesMatched);
                    if ((!bytesMatched) || (!matchedOrEdgeTransparent)) goto LAB_004acc64;
                    entryCounterField = (word *)(entryOrByteCursor + 0x1c);
                    *(int *)entryCounterField = *(int *)entryCounterField + -1;
                  } while (*(int *)entryCounterField != 0);
                  *(int *)(entryOrByteCursor + 0x1c) = *(int *)(entryOrByteCursor + 0x1c) + 1;
LAB_004acc64:
                  paletteIndexOrCount = *(int *)(entryOrByteCursor + 0x1c);
                  regionStart = (uint *)probeByteCursor;
                  scanCursor = (uint *)probeByteCursor;
                  if (matchedOrEdgeTransparent) {
                    do {
                      do {
                        regionStart = scanCursor;
                        if (edgeIndex != *probeByteCursor) goto LAB_004accb0;
                        probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                        paletteIndexOrCount = paletteIndexOrCount + -1;
                        scanCursor = regionStart;
                      } while (paletteIndexOrCount != 0);
                      *(int *)(entryOrByteCursor + 0x10) = *(int *)(entryOrByteCursor + 0x10) + 1;
                      probeByteCursor = (byte *)((int)regionStart + 1);
                      paletteIndexOrCount = *(int *)(entryOrByteCursor + 0x1c);
                      entryCounterField = (word *)(entryOrByteCursor + 0x18);
                      *(int *)entryCounterField = *(int *)entryCounterField + -1;
                      scanCursor = (uint *)probeByteCursor;
                    } while (*(int *)entryCounterField != 0);
                    *(int *)(entryOrByteCursor + 0x10) = *(int *)(entryOrByteCursor + 0x10) + -1;
                    *(int *)(entryOrByteCursor + 0x18) = *(int *)(entryOrByteCursor + 0x18) + 1;
                  }
LAB_004accb0:
                  scanCursor = regionStart;
                  paletteIndexOrCount = *(int *)(entryOrByteCursor + 0x1c);
                  probeByteCursor = (byte *)((int)regionStart + *(int *)(entryOrByteCursor + 0x18) + -1);
                  regionStart = (uint *)probeByteCursor;
                  if (matchedOrEdgeTransparent) {
                    do {
                      do {
                        if (edgeIndex != *probeByteCursor) goto LAB_004accf1;
                        probeByteCursor = probeByteCursor + sourceWidthOrTableBytes;
                        paletteIndexOrCount = paletteIndexOrCount + -1;
                      } while (paletteIndexOrCount != 0);
                      paletteIndexOrCount = *(int *)(entryOrByteCursor + 0x1c);
                      probeByteCursor = (byte *)((int)regionStart + -1);
                      entryCounterField = (word *)(entryOrByteCursor + 0x18);
                      *(int *)entryCounterField = *(int *)entryCounterField + -1;
                      regionStart = (uint *)probeByteCursor;
                    } while (*(int *)entryCounterField != 0);
                    *(int *)(entryOrByteCursor + 0x18) = *(int *)(entryOrByteCursor + 0x18) + 1;
                  }
LAB_004accf1:
                  paletteIndexOrCount = *(int *)(entryOrByteCursor + 0x1c);
                  backgroundColorOrCount = *(int *)(entryOrByteCursor + 0x18) * paletteIndexOrCount + 3U & 0xfffffffc;
                  freeBytesAfterPacked = remainingBytesOrCount - backgroundColorOrCount;
                  if (freeBytesAfterPacked == 0 || remainingBytesOrCount < (int)backgroundColorOrCount) goto LAB_004ad0e9;
                  packedPixelCursor = (uint *)((int)packedPixelCursor + -backgroundColorOrCount);
                  packedPixelBytes = packedPixelBytes + backgroundColorOrCount;
                  remainingBytesOrCount = *(int *)entryOrByteCursor;
                  entryOffsetOrRows = *(int *)(entryOrByteCursor + 4);
                  regionCopyBytes = *(int *)(entryOrByteCursor + 0x18);
                  copyBytesRemaining = regionCopyBytes;
                  entryOrByteCursor = (byte *)packedPixelCursor;
                  rowCursor = scanCursor;
                  do {
                    for (; copyBytesRemaining != 0; copyBytesRemaining = copyBytesRemaining + -1) {
                      *entryOrByteCursor = *(byte *)scanCursor;
                      scanCursor = (uint *)((int)scanCursor + 1);
                      entryOrByteCursor = entryOrByteCursor + 1;
                    }
                    scanCursor = (uint *)((int)rowCursor + sourceWidthOrTableBytes);
                    paletteIndexOrCount = paletteIndexOrCount + -1;
                    fillBytesRemaining = remainingBytesOrCount;
                    copyBytesRemaining = regionCopyBytes;
                    probeByteCursor = scanByteCursor;
                    rowCursor = scanCursor;
                    trimByteCursor = scanByteCursor;
                  } while (paletteIndexOrCount != 0);
                  do {
                    for (; fillBytesRemaining != 0; fillBytesRemaining = fillBytesRemaining + -1) {
                      *probeByteCursor = backgroundIndex;
                      probeByteCursor = probeByteCursor + 1;
                    }
                    entryOffsetOrRows = entryOffsetOrRows + -1;
                    fillBytesRemaining = remainingBytesOrCount;
                    probeByteCursor = trimByteCursor + sourceWidthOrTableBytes;
                    trimByteCursor = trimByteCursor + sourceWidthOrTableBytes;
                  } while (entryOffsetOrRows != 0);
                  bytesMatched = 0; /* Ghidra: ZF after an ESP adjustment (&stack0x00000000 == 0x40), never set on a real stack */
                  offsetOrColumnCount = offsetOrColumnCount + 1;
                  goto code_r0x004acaa4;
                }
                rowsRemaining = rowsRemaining + -1;
                bytesMatched = rowsRemaining == 0;
                offsetOrColumnCount = sourceWidthOrTableBytes;
              } while (!bytesMatched);
              sourceWidthOrTableBytes = ((decomposedAsset.assetOrError)->tableDescriptor).subresourceCount * 0x20;
              backgroundColorOrCount = packedPixelBytes >> 2;
              ((decomposedAsset.assetOrError)->common).allocationSizeBytes = packedPixelBytes + sourceWidthOrTableBytes + 0xa00;
              entryOrByteCursor = decomposedAsset.assetOrError[5].common.buildMetadata.assetRelativeAddressAnchor28 +
                        sourceWidthOrTableBytes + -0x28;
              for (; backgroundColorOrCount != 0; backgroundColorOrCount = backgroundColorOrCount - 1) {
                *(uint *)entryOrByteCursor = *packedPixelCursor;
                packedPixelCursor = (uint *)((int)packedPixelCursor + 4);
                entryOrByteCursor = entryOrByteCursor + 4;
              }
              shrinkResult = (*g_MemoryApi.shrinkInPlace)
                                 (((decomposedAsset.assetOrError)->common).allocationSizeBytes,
                                  decomposedAsset.assetOrError);
              copySourceOrError = (GraphicsTextureSourceAsset *)shrinkResult.scratchOrError;
              if (!shrinkResult.carry) {
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
                  entryCount = entryCount - 1;
                } while (entryCount != 0);
                decomposedAsset.carry = false;
                return decomposedAsset;
              }
            }
          }
        }
      }
    }
  }
LAB_004ad0ec:
  (*g_MemoryApi.free)(decomposedAsset.assetOrError);
  decomposedAsset.assetOrError = copySourceOrError;
LAB_004ad0f7:
  failureResult.carry = true;
  failureResult.assetOrError = decomposedAsset.assetOrError;
  return failureResult;
LAB_004ad0e9:
  copySourceOrError = (GraphicsTextureSourceAsset *)0x14;
  goto LAB_004ad0ec;
}

/* Address: 0x004AD630.
   Ownership: graphics/resources/texture.
   Purpose: Loads one gfx asset through Package_LoadEntry, converts every palette entry to the active framebuffer
   format, and returns the loaded asset. The first two parameters are the ECX/EDX package-loader context preserved
   from Package_LoadEntry; their higher-level semantics remain unresolved. ABI: CF clear means success. CF set
   means load or palette conversion failure.
   Cross-module calls: Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
GraphicsTextureSourceLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSource_LoadPackageAsset(word *pathUtf16)

{
  GraphicsPaletteTextureSourceAsset *loadedPaletteTextureSource;
  GraphicsPaletteTextureSourceAsset *convertedTextureSource;
  PackageLoadEntryEaxCf5 loadResult;
  GraphicsTextureSourceLoadEaxCf5 convertResult;
  
  loadResult = Package_LoadEntry(pathUtf16);
  loadedPaletteTextureSource = loadResult.bufferOrError;
  if (!loadResult.carry) {
    convertResult = THANDOR_BITCAST(GraphicsPaletteTextureSourceEaxCf5, GraphicsTextureSourceLoadEaxCf5, (*g_GraphicsTextureSourceConvertPaletteEntries)(loadedPaletteTextureSource));
    convertedTextureSource = (GraphicsPaletteTextureSourceAsset *)convertResult.eax;
    if (!convertResult.carry) {
      return convertResult;
    }
    Resource_Release(loadedPaletteTextureSource);
    loadedPaletteTextureSource = convertedTextureSource;
  }
  convertResult.carry = true;
  convertResult.eax = (GraphicsTextureSourceAsset *)loadedPaletteTextureSource;
  return convertResult;
}


/* Address: 0x004AD670.
   Ownership: graphics/resources/texture.
   Purpose: Allocates sourceAsset->allocationSizeBytes, copies the complete allocation in dwords, converts the
   clone's palette entries for the active framebuffer, and returns the independent heap clone. ABI: CF clear means
   success. CF set means allocation or palette conversion failure. Graphics texture-source lifecycle callback.
*/
GraphicsTextureSourceAsset *
GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsPaletteTextureSourceAsset *clonedAsset;
  uint allocationSizeOrCount;
  GraphicsPaletteTextureSourceAsset *cloneCursor;
  ArenaAllocEaxCf5 cloneAllocation;
  GraphicsPaletteTextureSourceEaxCf5 convertResult;
  ArenaFreeEaxCf5 freeResult;
  
  allocationSizeOrCount = (sourceAsset->common).allocationSizeBytes;
  cloneAllocation = (*g_MemoryApi.alloc)(allocationSizeOrCount);
  clonedAsset = (GraphicsPaletteTextureSourceAsset *)cloneAllocation.eax;
  if (!cloneAllocation.carry) {
    cloneCursor = clonedAsset;
    for (allocationSizeOrCount = allocationSizeOrCount >> 2; allocationSizeOrCount != 0; allocationSizeOrCount = allocationSizeOrCount - 1) {
      cloneCursor->magic = (sourceAsset->common).magic;
      sourceAsset = (GraphicsTextureSourceAsset *)&(sourceAsset->common).allocationSizeBytes;
      cloneCursor = (GraphicsPaletteTextureSourceAsset *)&cloneCursor->allocationSizeBytes;
    }
    convertResult = (*g_GraphicsTextureSourceConvertPaletteEntries)(clonedAsset);
    if (!convertResult.carry) {
      return (GraphicsTextureSourceAsset *)convertResult.paletteSource;
    }
    freeResult = (*g_MemoryApi.free)(clonedAsset);
    clonedAsset = (GraphicsPaletteTextureSourceAsset *)freeResult.eax;
  }
  return (GraphicsTextureSourceAsset *)clonedAsset;
}


/* Address: 0x004AD6C0.
   Ownership: graphics/resources/texture.
   Purpose: Validates the gfx signature, then converts paletteBankCount * 256 entries beginning at offset 0x200.
   Each 8-byte entry retains argb8888 and receives framebufferPixel through g_SoftwarePixelPackTables. ABI: CF
   clear means success. CF set means invalid input.
*/
GraphicsPaletteTextureSourceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset)

{
  int paletteEntriesRemaining;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
  GraphicsPaletteTextureSourceEaxCf5 successResult;
  GraphicsPaletteTextureSourceEaxCf5 failureResult;
  uint argb8888;
  
  if ((sourceAsset != (GraphicsPaletteTextureSourceAsset *)0x0) &&
     (sourceAsset->magic == GRAPHICS_PALETTE_TEXTURE_MAGIC_GFX)) {
    paletteEntryCursor = sourceAsset->paletteEntries;
    for (paletteEntriesRemaining = sourceAsset->paletteBankCount << 8; paletteEntriesRemaining != 0;
        paletteEntriesRemaining = paletteEntriesRemaining + -1) {
      argb8888 = paletteEntryCursor->argb8888;
      paletteEntryCursor->framebufferPixel =
           (argb8888 & 0xff000000) +
           *(int *)((int)g_SoftwarePixelPackTables->red + ((argb8888 & 0xff0000) >> 0xe)) +
           *(int *)((int)g_SoftwarePixelPackTables->green + ((argb8888 & 0xff00) >> 6)) +
           g_SoftwarePixelPackTables->blue[argb8888 & 0xff];
      paletteEntryCursor = paletteEntryCursor + 1;
    }
    successResult.carry = false;
    successResult.paletteSource = sourceAsset;
    return successResult;
  }
  failureResult.carry = true;
  failureResult.paletteSource = (GraphicsPaletteTextureSourceAsset *)0x2c;
  return failureResult;
}


/* Address: 0x004AD770.
   Ownership: graphics/resources/texture.
   Purpose: Resolves the underlying allocation base through g_GraphicsTextureSourceResolveAllocationBase, then
   releases it through Resource_Release. Use this path for assets returned by
   GraphicsTextureSource_LoadPackageAsset. Graphics texture-source lifecycle callback.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;
  
  allocation = (*g_GraphicsTextureSourceResolveAllocationBase)(sourceAsset);
  Resource_Release(allocation);
  return;
}


/* Address: 0x004AD790.
   Ownership: graphics/resources/texture.
   Purpose: Resolves the underlying allocation base through g_GraphicsTextureSourceResolveAllocationBase, then
   frees it through g_MemoryApi.free. Use this path for assets returned by GraphicsTextureSource_CloneAsset.
   Graphics texture-source lifecycle callback.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *memory;
  
  memory = (*g_GraphicsTextureSourceResolveAllocationBase)(sourceAsset);
  (*g_MemoryApi.free)(memory);
  return;
}


/* Address: 0x004AD7B0.
   Ownership: graphics/resources/texture.
   Purpose: Returns the allocation pointer that owns a gfx asset. The current implementation is an identity
   function, but all release services route through this slot.
*/
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset)

{
  return sourceAsset;
}


/* Address: 0x004AD7C0.
   Ownership: graphics/resources/texture.
   Purpose: Validates the gfx signature and a subresource count in the range 1..4095, then reads logicalWidth into
   EAX and logicalHeight into EDX from the first source entry at subresourceTableOffset. The qword return models
   EDX:EAX. ABI: CF clear means success; CF set means invalid input.
*/
GraphicsTextureSizeEaxEdxCf9
GraphicsTextureSource_GetFirstLogicalSizeRegs(GraphicsTextureSourceAsset *sourceAsset)

{
  uint entryCount;
  undefined4 in_EAX;
  byte *firstSubresourceRecord;
  bool invalid;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  uint subresourceCount;
  
  invalid = true;
  if ((sourceAsset->common).magic == ASSET_MAGIC_GFX) {
    entryCount = (sourceAsset->tableDescriptor).subresourceCount;
    invalid = true;
    if ((entryCount != 0) && (invalid = 0xfff < entryCount, !invalid)) {
      firstSubresourceRecord =
           (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
           ((sourceAsset->tableDescriptor).subresourceTableOffset - 0x28);
      in_EAX = *(undefined4 *)firstSubresourceRecord;
      sourceAsset = *(GraphicsTextureSourceAsset **)(firstSubresourceRecord + 4);
      invalid = false;
    }
  }
  logicalSize.logicalHeightPixels = (dword)sourceAsset;
  logicalSize.logicalWidthPixels = in_EAX;
  logicalSize.carry = invalid;
  return logicalSize;
}


/* Address: 0x0057ADA0.
   Ownership: graphics/resources/texture.
   Purpose: Restores and locks texture->stagingSurface3, converts the selected gfx source entry into the
   destination DDPIXELFORMAT, unlocks the surface, and brackets the operation with g_ActiveTextureUploads.
   paletteIndex < 0 selects direct 32-bit source pixels. Nonnegative paletteIndex selects one 256-entry palette
   bank at asset+0x200+paletteIndex*0x800. For non-8-bit surfaces, channel positions are derived from the
   destination RGB/alpha masks. Destinations up to 16 bits use word stores; wider destinations use dword packing.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadColor_1x(GraphicsTextureResource *texture)

{
  int offsetOrGreenTopBit;
  DDPIXELFORMAT *destinationFormat;
  uint maskOrArgb;
  uint greenMask;
  uint blueMask;
  int blueTopBit;
  undefined1 paletteGreen;
  undefined1 paletteBlue;
  undefined1 paletteFlags;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int paletteIndexOrCounter;
  DirectDrawPaletteEntry grayPaletteEntry;
  byte *sourcePixel;
  GraphicsTextureSourceAsset *paletteSourceCursor;
  ushort *destinationWord;
  byte *byteCursorOrSourceRow;
  DirectDrawPaletteEntry *paletteEntryCursor;
  uint *destinationDword;
  byte *sourceByteRow;
  ushort *destinationWordRow;
  byte *destinationByteRow;
  uint *destinationDwordRow;
  int *createdPalette;
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
  TH_LEGACY_LONG savedPitch;
  TH_LEGACY_LPVOID savedSurfaceBits;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    hresult = (*stagingSurface3->lpVtbl->IsLost)(stagingSurface3);
    paletteIndexOrCounter = 0;
    if (hresult != 0) {
      paletteIndexOrCounter = (*stagingSurface3->lpVtbl->Restore)(stagingSurface3);
    }
    if (paletteIndexOrCounter == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      hresult = (*stagingSurface3->lpVtbl->Lock)
                         (stagingSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,
                          (TH_LEGACY_HANDLE)0x0);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrGreenTopBit = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrGreenTopBit + -0x20);
        sourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrGreenTopBit + -0x10);
        rowsRemaining = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrGreenTopBit + -0xc);
        offsetOrGreenTopBit = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        offsetOrGreenTopBit + -0x1c);
        if (paletteIndexOrCounter < 0) {
          sourcePixel = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + offsetOrGreenTopBit + -0x28
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
                  paletteIndexOrCounter = paletteIndexOrCounter + -1;
                  byteCursorOrSourceRow = byteCursorOrSourceRow + 1;
                } while (paletteIndexOrCounter != 0);
                sourcePixel = sourceByteRow + sourceWidth * 4;
                rowsRemaining = rowsRemaining + -1;
                paletteIndexOrCounter = sourceWidth;
                byteCursorOrSourceRow = destinationByteRow + destinationPitch;
                sourceByteRow = sourcePixel;
                destinationByteRow = destinationByteRow + destinationPitch;
              } while (rowsRemaining != 0);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 0x100;
              grayPaletteEntry.red = 0;
              grayPaletteEntry.green = 0;
              grayPaletteEntry.blue = 0;
              grayPaletteEntry.flags = 0;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                *paletteEntryCursor = grayPaletteEntry;
                paletteEntryCursor = paletteEntryCursor + 1;
                grayPaletteEntry = THANDOR_BITCAST(int, DirectDrawPaletteEntry, (THANDOR_BITCAST(DirectDrawPaletteEntry, int, grayPaletteEntry) + 0x1010101));
                paletteIndexOrCounter = paletteIndexOrCounter + -1;
              } while (paletteIndexOrCounter != 0);
              hresult = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&createdPalette,
                                  (TH_LEGACY_LPVOID)0x0);
              if (hresult == 0) {
                (**(code **)(*createdPalette + 8))(createdPalette);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
            maskOrArgb = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((maskOrArgb != 0) && (greenMask != 0)) && (blueMask != 0)) {
              redShiftLeft = 0;
              if (maskOrArgb != 0) {
                for (; (maskOrArgb >> redShiftLeft & 1) == 0; redShiftLeft = redShiftLeft + 1) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft = greenShiftLeft + 1) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft = blueShiftLeft + 1) {
                }
              }
              paletteIndexOrCounter = 0x1f;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                }
              }
              offsetOrGreenTopBit = 0x1f;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit = offsetOrGreenTopBit + -1) {
                }
              }
              blueTopBit = 0x1f;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit = blueTopBit + -1) {
                }
              }
              redShiftRight = 0x18 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 0x10 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              maskOrArgb = destinationFormat->dwRGBAlphaBitMask;
              if (maskOrArgb == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 0x10;
              }
              else {
                alphaShiftLeft = 0;
                if (maskOrArgb != 0) {
                  for (; (maskOrArgb >> alphaShiftLeft & 1) == 0; alphaShiftLeft = alphaShiftLeft + 1) {
                  }
                }
                paletteIndexOrCounter = 0x1f;
                if (maskOrArgb != 0) {
                  for (; maskOrArgb >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                  }
                }
                alphaShiftRight = 0x20 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrSourceRow = sourcePixel;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 0x11) {
                do {
                  do {
                    maskOrArgb = *(uint *)sourcePixel;
                    *destinationWord = (ushort)(((maskOrArgb & 0xff000000) >> ((byte)alphaShiftRight & 0x1f)) <<
                                       ((byte)alphaShiftLeft & 0x1f)) |
                               (ushort)(((maskOrArgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                                       ((byte)blueShiftLeft & 0x1f)) |
                               (ushort)(((maskOrArgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                                       ((byte)greenShiftLeft & 0x1f)) |
                               (ushort)(((maskOrArgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                                       ((byte)redShiftLeft & 0x1f));
                    sourcePixel = sourcePixel + 4;
                    paletteIndexOrCounter = paletteIndexOrCounter + -1;
                    destinationWord = destinationWord + 1;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth * 4;
                  rowsRemaining = rowsRemaining + -1;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationWordRow = (ushort *)((int)destinationWordRow + destinationPitch);
                } while (rowsRemaining != 0);
                rowsRemaining = 0;
              }
              else {
                do {
                  do {
                    maskOrArgb = *(uint *)sourcePixel;
                    *destinationDword = ((maskOrArgb & 0xff000000) >> ((byte)alphaShiftRight & 0x1f)) <<
                               ((byte)alphaShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                               ((byte)blueShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                               ((byte)greenShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                               ((byte)redShiftLeft & 0x1f);
                    sourcePixel = sourcePixel + 4;
                    paletteIndexOrCounter = paletteIndexOrCounter + -1;
                    destinationDword = destinationDword + 1;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth * 4;
                  rowsRemaining = rowsRemaining + -1;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationDwordRow = (uint *)((int)destinationDwordRow + destinationPitch);
                } while (rowsRemaining != 0);
                rowsRemaining = 0;
              }
            }
          }
        }
        else {
          sourcePixel = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + offsetOrGreenTopBit + -0x28
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
                  sourcePixel = sourcePixel + 1;
                  paletteIndexOrCounter = paletteIndexOrCounter + -1;
                  byteCursorOrSourceRow = byteCursorOrSourceRow + 1;
                } while (paletteIndexOrCounter != 0);
                sourcePixel = sourceByteRow + sourceWidth;
                rowsRemaining = rowsRemaining + -1;
                paletteIndexOrCounter = sourceWidth;
                byteCursorOrSourceRow = destinationByteRow + destinationPitch;
                sourceByteRow = sourcePixel;
                destinationByteRow = destinationByteRow + destinationPitch;
              } while (rowsRemaining != 0);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 0x100;
              paletteSourceCursor = paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 1);
                paletteBlue = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 2);
                paletteFlags = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 3);
                paletteEntryCursor->red = *(undefined1 *)&(paletteSourceCursor->common).magic;
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor = (GraphicsTextureSourceAsset *)&(paletteSourceCursor->common).formatVersion;
                paletteEntryCursor = paletteEntryCursor + 1;
                paletteIndexOrCounter = paletteIndexOrCounter + -1;
              } while (paletteIndexOrCounter != 0);
              hresult = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&createdPalette,
                                  (TH_LEGACY_LPVOID)0x0);
              if (hresult == 0) {
                (**(code **)(*createdPalette + 8))(createdPalette);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
            maskOrArgb = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((maskOrArgb != 0) && (greenMask != 0)) && (blueMask != 0)) {
              redShiftLeft = 0;
              if (maskOrArgb != 0) {
                for (; (maskOrArgb >> redShiftLeft & 1) == 0; redShiftLeft = redShiftLeft + 1) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft = greenShiftLeft + 1) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft = blueShiftLeft + 1) {
                }
              }
              paletteIndexOrCounter = 0x1f;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                }
              }
              offsetOrGreenTopBit = 0x1f;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit = offsetOrGreenTopBit + -1) {
                }
              }
              blueTopBit = 0x1f;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit = blueTopBit + -1) {
                }
              }
              redShiftRight = 0x18 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 0x10 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              maskOrArgb = destinationFormat->dwRGBAlphaBitMask;
              if (maskOrArgb == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 0x10;
              }
              else {
                alphaShiftLeft = 0;
                if (maskOrArgb != 0) {
                  for (; (maskOrArgb >> alphaShiftLeft & 1) == 0; alphaShiftLeft = alphaShiftLeft + 1) {
                  }
                }
                paletteIndexOrCounter = 0x1f;
                if (maskOrArgb != 0) {
                  for (; maskOrArgb >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                  }
                }
                alphaShiftRight = 0x20 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrSourceRow = sourcePixel;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 0x11) {
                do {
                  do {
                    maskOrArgb = *(uint *)((paletteBank->common).buildMetadata.
                                      assetRelativeAddressAnchor28 + (uint)*sourcePixel * 8 + -0x28);
                    *destinationWord = (ushort)(((maskOrArgb & 0xff000000) >> ((byte)alphaShiftRight & 0x1f)) <<
                                       ((byte)alphaShiftLeft & 0x1f)) |
                               (ushort)(((maskOrArgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                                       ((byte)blueShiftLeft & 0x1f)) |
                               (ushort)(((maskOrArgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                                       ((byte)greenShiftLeft & 0x1f)) |
                               (ushort)(((maskOrArgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                                       ((byte)redShiftLeft & 0x1f));
                    sourcePixel = sourcePixel + 1;
                    paletteIndexOrCounter = paletteIndexOrCounter + -1;
                    destinationWord = destinationWord + 1;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth;
                  rowsRemaining = rowsRemaining + -1;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationWordRow = (ushort *)((int)destinationWordRow + destinationPitch);
                } while (rowsRemaining != 0);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              }
              else {
                do {
                  do {
                    maskOrArgb = *(uint *)((paletteBank->common).buildMetadata.
                                      assetRelativeAddressAnchor28 + (uint)*sourcePixel * 8 + -0x28);
                    *destinationDword = ((maskOrArgb & 0xff000000) >> ((byte)alphaShiftRight & 0x1f)) <<
                               ((byte)alphaShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                               ((byte)blueShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                               ((byte)greenShiftLeft & 0x1f) |
                               ((maskOrArgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                               ((byte)redShiftLeft & 0x1f);
                    sourcePixel = sourcePixel + 1;
                    paletteIndexOrCounter = paletteIndexOrCounter + -1;
                    destinationDword = destinationDword + 1;
                  } while (paletteIndexOrCounter != 0);
                  sourcePixel = byteCursorOrSourceRow + sourceWidth;
                  rowsRemaining = rowsRemaining + -1;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrSourceRow = sourcePixel;
                  destinationDwordRow = (uint *)((int)destinationDwordRow + destinationPitch);
                } while (rowsRemaining != 0);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
          }
        }
        (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
      }
    }
  }
GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads = g_ActiveTextureUploads - 1;
  return;
}


/* Address: 0x0057B410.
   Ownership: graphics/resources/texture.
   Purpose: Restores and locks texture->stagingSurface3, converts the selected gfx source entry into the
   destination DDPIXELFORMAT, unlocks the surface, and brackets the operation with g_ActiveTextureUploads.
   paletteIndex < 0 selects direct 32-bit source pixels. Nonnegative paletteIndex selects one 256-entry palette
   bank at asset+0x200+paletteIndex*0x800. For non-8-bit surfaces, channel positions are derived from the
   destination RGB/alpha masks. Destinations up to 16 bits use word stores; wider destinations use dword packing.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadColor_2x(GraphicsTextureResource *texture)

{
  DDPIXELFORMAT *destinationFormat;
  uint redOrAlphaMask;
  uint greenMask;
  uint blueMask;
  undefined4 topLeftTexel;
  undefined4 topRightTexel;
  undefined4 bottomLeftTexel;
  undefined4 bottomRightTexel;
  bool hasMore;
  int blueTopBit;
  undefined1 paletteGreen;
  undefined1 paletteBlue;
  undefined1 paletteFlags;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int paletteIndexOrCounter;
  DirectDrawPaletteEntry grayPaletteEntry;
  int offsetOrRemaining;
  byte *byteCursor;
  GraphicsTextureSourceAsset *paletteSourceCursor;
  AssetProducerSourceNames *sourceTexel;
  byte *byteCursorOrRow;
  ushort *destinationWord;
  DirectDrawPaletteEntry *paletteEntryCursor;
  uint *destinationDword;
  ushort averageBlue;
  uint3 averageRgb;
  ushort averageGreen;
  ushort averageRed;
  ushort topLeftAlphaOrAverage;
  ushort topRightAlpha;
  ushort bottomLeftAlpha;
  ushort bottomRightAlpha;
  byte *sourceByteRow;
  AssetProducerSourceNames *sourceTexelRow;
  byte *destinationByteRow;
  ushort *destinationWordRow;
  uint *destinationDwordRow;
  int *createdPalette;
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
  TH_LEGACY_LONG savedPitch;
  TH_LEGACY_LPVOID savedSurfaceBits;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    hresult = (*stagingSurface3->lpVtbl->IsLost)(stagingSurface3);
    paletteIndexOrCounter = 0;
    if (hresult != 0) {
      paletteIndexOrCounter = (*stagingSurface3->lpVtbl->Restore)(stagingSurface3);
    }
    if (paletteIndexOrCounter == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      hresult = (*stagingSurface3->lpVtbl->Lock)
                         (stagingSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,
                          (TH_LEGACY_HANDLE)0x0);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrRemaining = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrRemaining + -0x20);
        sourceWidth = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrRemaining + -0x10);
        rowsRemaining = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrRemaining + -0xc);
        offsetOrRemaining = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrRemaining + -0x1c);
        if (paletteIndexOrCounter < 0) {
          sourceTexel = (AssetProducerSourceNames *)
                    ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    offsetOrRemaining + -0x28);
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            paletteIndexOrCounter = sourceWidth;
            byteCursor = g_SurfaceDesc.lpSurface;
            sourceTexelRow = sourceTexel;
            byteCursorOrRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursor = *(byte *)((int)sourceTexel->producerName + 3);
                  sourceTexel = (AssetProducerSourceNames *)((int)sourceTexel->producerName + 8);
                  offsetOrRemaining = paletteIndexOrCounter + -2;
                  hasMore = 1 < paletteIndexOrCounter;
                  paletteIndexOrCounter = offsetOrRemaining;
                  byteCursor = byteCursor + 1;
                } while (offsetOrRemaining != 0 && hasMore);
                sourceTexel = (AssetProducerSourceNames *)((int)sourceTexelRow->producerName + sourceWidth * 8);
                offsetOrRemaining = rowsRemaining + -2;
                hasMore = 1 < rowsRemaining;
                paletteIndexOrCounter = sourceWidth;
                byteCursor = byteCursorOrRow + destinationPitch;
                sourceTexelRow = sourceTexel;
                byteCursorOrRow = byteCursorOrRow + destinationPitch;
                rowsRemaining = offsetOrRemaining;
              } while (offsetOrRemaining != 0 && hasMore);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 0x100;
              grayPaletteEntry.red = 0;
              grayPaletteEntry.green = 0;
              grayPaletteEntry.blue = 0;
              grayPaletteEntry.flags = 0;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                *paletteEntryCursor = grayPaletteEntry;
                paletteEntryCursor = paletteEntryCursor + 1;
                grayPaletteEntry = THANDOR_BITCAST(int, DirectDrawPaletteEntry, (THANDOR_BITCAST(DirectDrawPaletteEntry, int, grayPaletteEntry) + 0x1010101));
                paletteIndexOrCounter = paletteIndexOrCounter + -1;
              } while (paletteIndexOrCounter != 0);
              hresult = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&createdPalette,
                                  (TH_LEGACY_LPVOID)0x0);
              if (hresult == 0) {
                (**(code **)(*createdPalette + 8))(createdPalette);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
            redOrAlphaMask = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((redOrAlphaMask != 0) && (greenMask != 0)) && (blueMask != 0)) {
              redShiftLeft = 0;
              if (redOrAlphaMask != 0) {
                for (; (redOrAlphaMask >> redShiftLeft & 1) == 0; redShiftLeft = redShiftLeft + 1) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft = greenShiftLeft + 1) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft = blueShiftLeft + 1) {
                }
              }
              paletteIndexOrCounter = 0x1f;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                }
              }
              offsetOrRemaining = 0x1f;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrRemaining == 0; offsetOrRemaining = offsetOrRemaining + -1) {
                }
              }
              blueTopBit = 0x1f;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit = blueTopBit + -1) {
                }
              }
              redShiftRight = 0x18 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 0x10 - ((offsetOrRemaining + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              redOrAlphaMask = destinationFormat->dwRGBAlphaBitMask;
              if (redOrAlphaMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 0x10;
              }
              else {
                alphaShiftLeft = 0;
                if (redOrAlphaMask != 0) {
                  for (; (redOrAlphaMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft = alphaShiftLeft + 1) {
                  }
                }
                paletteIndexOrCounter = 0x1f;
                if (redOrAlphaMask != 0) {
                  for (; redOrAlphaMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                  }
                }
                alphaShiftRight = 0x20 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 0x11) {
                do {
                  do {
                    topLeftTexel = *(undefined4 *)sourceTexel->producerName;
                    topRightTexel = *(undefined4 *)(sourceTexel->producerName + 2);
                    bottomLeftTexel = *(undefined4 *)(sourceTexel->producerName + sourceWidth * 2);
                    bottomRightTexel = *(undefined4 *)(sourceTexel->producerName + sourceWidth * 2 + 2);
                    topLeftAlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 0x18) << 0x38) >> 0x30);
                    topRightAlpha = (ushort)(((ulonglong)(byte)((uint)topRightTexel >> 0x18) << 0x38) >> 0x30);
                    bottomLeftAlpha = (ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 0x18) << 0x38) >> 0x30);
                    bottomRightAlpha = (ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)topLeftTexel + (ushort)(byte)topRightTexel +
                                     (ushort)(byte)bottomLeftTexel + (ushort)(byte)bottomRightTexel) >> 2;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)topRightTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(topLeftAlphaOrAverage,(char)((uint)topLeftTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(topRightAlpha,(char)((uint)topRightTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomLeftAlpha,(char)((uint)bottomLeftTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomRightAlpha,(char)((uint)bottomRightTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    topLeftAlphaOrAverage = (ushort)((topLeftAlphaOrAverage >> 8) + (topRightAlpha >> 8) + (bottomLeftAlpha >> 8) + (bottomRightAlpha >> 8))
                             >> 2;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationWord = (ushort)((((uint)(byte)((topLeftAlphaOrAverage != 0) * (topLeftAlphaOrAverage < 0x100) *
                                                       (char)topLeftAlphaOrAverage - (0xff < topLeftAlphaOrAverage)) << 0x18) >>
                                        ((byte)alphaShiftRight & 0x1f)) << ((byte)alphaShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                                       ((byte)blueShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                                       ((byte)greenShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                                       ((byte)redShiftLeft & 0x1f));
                    sourceTexel = (AssetProducerSourceNames *)(sourceTexel->producerName + 4);
                    offsetOrRemaining = paletteIndexOrCounter + -2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationWord = destinationWord + 1;
                  } while (offsetOrRemaining != 0 && hasMore);
                  sourceTexel = (AssetProducerSourceNames *)(sourceTexelRow->producerName + sourceWidth * 4);
                  offsetOrRemaining = rowsRemaining + -2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationWordRow = (ushort *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
              }
              else {
                do {
                  do {
                    topLeftTexel = *(undefined4 *)sourceTexel->producerName;
                    topRightTexel = *(undefined4 *)(sourceTexel->producerName + 2);
                    bottomLeftTexel = *(undefined4 *)(sourceTexel->producerName + sourceWidth * 2);
                    bottomRightTexel = *(undefined4 *)(sourceTexel->producerName + sourceWidth * 2 + 2);
                    topLeftAlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 0x18) << 0x38) >> 0x30);
                    topRightAlpha = (ushort)(((ulonglong)(byte)((uint)topRightTexel >> 0x18) << 0x38) >> 0x30);
                    bottomLeftAlpha = (ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 0x18) << 0x38) >> 0x30);
                    bottomRightAlpha = (ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)topLeftTexel + (ushort)(byte)topRightTexel +
                                     (ushort)(byte)bottomLeftTexel + (ushort)(byte)bottomRightTexel) >> 2;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)topRightTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(topLeftAlphaOrAverage,(char)((uint)topLeftTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(topRightAlpha,(char)((uint)topRightTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomLeftAlpha,(char)((uint)bottomLeftTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomRightAlpha,(char)((uint)bottomRightTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    topLeftAlphaOrAverage = (ushort)((topLeftAlphaOrAverage >> 8) + (topRightAlpha >> 8) + (bottomLeftAlpha >> 8) + (bottomRightAlpha >> 8))
                             >> 2;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationDword = (((uint)(byte)((topLeftAlphaOrAverage != 0) * (topLeftAlphaOrAverage < 0x100) * (char)topLeftAlphaOrAverage -
                                             (0xff < topLeftAlphaOrAverage)) << 0x18) >> ((byte)alphaShiftRight & 0x1f))
                               << ((byte)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                               ((byte)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                               ((byte)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                               ((byte)redShiftLeft & 0x1f);
                    sourceTexel = (AssetProducerSourceNames *)(sourceTexel->producerName + 4);
                    offsetOrRemaining = paletteIndexOrCounter + -2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationDword = destinationDword + 1;
                  } while (offsetOrRemaining != 0 && hasMore);
                  sourceTexel = (AssetProducerSourceNames *)(sourceTexelRow->producerName + sourceWidth * 4);
                  offsetOrRemaining = rowsRemaining + -2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationDwordRow = (uint *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
              }
            }
          }
        }
        else {
          byteCursor = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    offsetOrRemaining + -0x28;
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
                  offsetOrRemaining = paletteIndexOrCounter + -2;
                  hasMore = 1 < paletteIndexOrCounter;
                  paletteIndexOrCounter = offsetOrRemaining;
                  byteCursorOrRow = byteCursorOrRow + 1;
                } while (offsetOrRemaining != 0 && hasMore);
                byteCursor = sourceByteRow + sourceWidth * 2;
                offsetOrRemaining = rowsRemaining + -2;
                hasMore = 1 < rowsRemaining;
                paletteIndexOrCounter = sourceWidth;
                byteCursorOrRow = destinationByteRow + destinationPitch;
                sourceByteRow = byteCursor;
                destinationByteRow = destinationByteRow + destinationPitch;
                rowsRemaining = offsetOrRemaining;
              } while (offsetOrRemaining != 0 && hasMore);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 0x100;
              paletteSourceCursor = paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 1);
                paletteBlue = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 2);
                paletteFlags = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 3);
                paletteEntryCursor->red = *(undefined1 *)&(paletteSourceCursor->common).magic;
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor = (GraphicsTextureSourceAsset *)&(paletteSourceCursor->common).formatVersion;
                paletteEntryCursor = paletteEntryCursor + 1;
                paletteIndexOrCounter = paletteIndexOrCounter + -1;
              } while (paletteIndexOrCounter != 0);
              hresult = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&createdPalette,
                                  (TH_LEGACY_LPVOID)0x0);
              if (hresult == 0) {
                (**(code **)(*createdPalette + 8))(createdPalette);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
            redOrAlphaMask = destinationFormat->dwRBitMask;
            greenMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((redOrAlphaMask != 0) && (greenMask != 0)) && (blueMask != 0)) {
              redShiftLeft = 0;
              if (redOrAlphaMask != 0) {
                for (; (redOrAlphaMask >> redShiftLeft & 1) == 0; redShiftLeft = redShiftLeft + 1) {
                }
              }
              greenShiftLeft = 0;
              if (greenMask != 0) {
                for (; (greenMask >> greenShiftLeft & 1) == 0; greenShiftLeft = greenShiftLeft + 1) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft = blueShiftLeft + 1) {
                }
              }
              paletteIndexOrCounter = 0x1f;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                }
              }
              offsetOrRemaining = 0x1f;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrRemaining == 0; offsetOrRemaining = offsetOrRemaining + -1) {
                }
              }
              blueTopBit = 0x1f;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit = blueTopBit + -1) {
                }
              }
              redShiftRight = 0x18 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 0x10 - ((offsetOrRemaining + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              redOrAlphaMask = destinationFormat->dwRGBAlphaBitMask;
              if (redOrAlphaMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 0x10;
              }
              else {
                alphaShiftLeft = 0;
                if (redOrAlphaMask != 0) {
                  for (; (redOrAlphaMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft = alphaShiftLeft + 1) {
                  }
                }
                paletteIndexOrCounter = 0x1f;
                if (redOrAlphaMask != 0) {
                  for (; redOrAlphaMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                  }
                }
                alphaShiftRight = 0x20 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              paletteIndexOrCounter = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrRow = byteCursor;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 0x11) {
                do {
                  do {
                    topLeftTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*byteCursor * 8 + -0x28);
                    topRightTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[1] * 8 + -0x28);
                    bottomLeftTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth] * 8 + -0x28);
                    bottomRightTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth + 1] * 8 + -0x28);
                    topLeftAlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 0x18) << 0x38) >> 0x30);
                    topRightAlpha = (ushort)(((ulonglong)(byte)((uint)topRightTexel >> 0x18) << 0x38) >> 0x30);
                    bottomLeftAlpha = (ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 0x18) << 0x38) >> 0x30);
                    bottomRightAlpha = (ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)topLeftTexel + (ushort)(byte)topRightTexel +
                                     (ushort)(byte)bottomLeftTexel + (ushort)(byte)bottomRightTexel) >> 2;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)topRightTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(topLeftAlphaOrAverage,(char)((uint)topLeftTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(topRightAlpha,(char)((uint)topRightTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomLeftAlpha,(char)((uint)bottomLeftTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomRightAlpha,(char)((uint)bottomRightTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    topLeftAlphaOrAverage = (ushort)((topLeftAlphaOrAverage >> 8) + (topRightAlpha >> 8) + (bottomLeftAlpha >> 8) + (bottomRightAlpha >> 8))
                             >> 2;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationWord = (ushort)((((uint)(byte)((topLeftAlphaOrAverage != 0) * (topLeftAlphaOrAverage < 0x100) *
                                                       (char)topLeftAlphaOrAverage - (0xff < topLeftAlphaOrAverage)) << 0x18) >>
                                        ((byte)alphaShiftRight & 0x1f)) << ((byte)alphaShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                                       ((byte)blueShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                                       ((byte)greenShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                                       ((byte)redShiftLeft & 0x1f));
                    byteCursor = byteCursor + 2;
                    offsetOrRemaining = paletteIndexOrCounter + -2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationWord = destinationWord + 1;
                  } while (offsetOrRemaining != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 2;
                  offsetOrRemaining = rowsRemaining + -2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationWordRow = (ushort *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              }
              else {
                do {
                  do {
                    topLeftTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*byteCursor * 8 + -0x28);
                    topRightTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[1] * 8 + -0x28);
                    bottomLeftTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth] * 8 + -0x28);
                    bottomRightTexel = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth + 1] * 8 + -0x28);
                    topLeftAlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 0x18) << 0x38) >> 0x30);
                    topRightAlpha = (ushort)(((ulonglong)(byte)((uint)topRightTexel >> 0x18) << 0x38) >> 0x30);
                    bottomLeftAlpha = (ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 0x18) << 0x38) >> 0x30);
                    bottomRightAlpha = (ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)topLeftTexel + (ushort)(byte)topRightTexel +
                                     (ushort)(byte)bottomLeftTexel + (ushort)(byte)bottomRightTexel) >> 2;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)topLeftTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)topRightTexel >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomLeftTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)bottomRightTexel >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(topLeftAlphaOrAverage,(char)((uint)topLeftTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(topRightAlpha,(char)((uint)topRightTexel >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomLeftAlpha,(char)((uint)bottomLeftTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(bottomRightAlpha,(char)((uint)bottomRightTexel >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    topLeftAlphaOrAverage = (ushort)((topLeftAlphaOrAverage >> 8) + (topRightAlpha >> 8) + (bottomLeftAlpha >> 8) + (bottomRightAlpha >> 8))
                             >> 2;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationDword = (((uint)(byte)((topLeftAlphaOrAverage != 0) * (topLeftAlphaOrAverage < 0x100) * (char)topLeftAlphaOrAverage -
                                             (0xff < topLeftAlphaOrAverage)) << 0x18) >> ((byte)alphaShiftRight & 0x1f))
                               << ((byte)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                               ((byte)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                               ((byte)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                               ((byte)redShiftLeft & 0x1f);
                    byteCursor = byteCursor + 2;
                    offsetOrRemaining = paletteIndexOrCounter + -2;
                    hasMore = 1 < paletteIndexOrCounter;
                    paletteIndexOrCounter = offsetOrRemaining;
                    destinationDword = destinationDword + 1;
                  } while (offsetOrRemaining != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 2;
                  offsetOrRemaining = rowsRemaining + -2;
                  hasMore = 1 < rowsRemaining;
                  paletteIndexOrCounter = sourceWidth;
                  destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationDwordRow = (uint *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = offsetOrRemaining;
                } while (offsetOrRemaining != 0 && hasMore);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
          }
        }
        (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
      }
    }
  }
GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads = g_ActiveTextureUploads - 1;
  return;
}


/* Address: 0x0057BBE0.
   Ownership: graphics/resources/texture.
   Purpose: Restores and locks texture->stagingSurface3, converts the selected gfx source entry into the
   destination DDPIXELFORMAT, unlocks the surface, and brackets the operation with g_ActiveTextureUploads.
   paletteIndex < 0 selects direct 32-bit source pixels. Nonnegative paletteIndex selects one 256-entry palette
   bank at asset+0x200+paletteIndex*0x800. For non-8-bit surfaces, channel positions are derived from the
   destination RGB/alpha masks. Destinations up to 16 bits use word stores; wider destinations use dword packing.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadColor_4x(GraphicsTextureResource *texture)

{
  int offsetOrGreenTopBit;
  DDPIXELFORMAT *destinationFormat;
  uint blueMask;
  undefined4 texel00;
  undefined4 texel01;
  undefined4 texel02;
  undefined4 texel03;
  undefined4 texel04;
  undefined4 texel05;
  undefined4 texel06;
  undefined4 texel07;
  undefined4 texel08;
  undefined4 texel09;
  undefined4 texel10;
  undefined4 texel11;
  undefined4 texel12;
  undefined4 texel13;
  undefined4 texel14;
  undefined4 texel15;
  bool hasMore;
  int blueTopBit;
  undefined1 paletteGreen;
  undefined1 paletteBlue;
  undefined1 paletteFlags;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int paletteIndexOrCounter;
  DirectDrawPaletteEntry grayPaletteEntry;
  uint nextColumnsOrMask;
  uint columnsOrMask;
  byte *byteCursor;
  GraphicsTextureSourceAsset *paletteSourceCursor;
  word *sourceTexel;
  byte *byteCursorOrRow;
  ushort *destinationWord;
  DirectDrawPaletteEntry *paletteEntryCursor;
  uint *destinationDword;
  ushort averageBlue;
  uint3 averageRgb;
  ushort averageGreen;
  ushort averageRed;
  ushort texel00AlphaOrAverage;
  ushort texel01Alpha;
  ushort texel02Alpha;
  ushort texel03Alpha;
  ushort texel04Alpha;
  ushort texel08Alpha;
  ushort texel12Alpha;
  ushort texel05Alpha;
  ushort texel09Alpha;
  ushort texel13Alpha;
  ushort texel06Alpha;
  ushort texel10Alpha;
  ushort texel14Alpha;
  ushort texel07Alpha;
  ushort texel11Alpha;
  ushort texel15Alpha;
  byte *sourceByteRow;
  word *sourceTexelRow;
  byte *destinationByteRow;
  ushort *destinationWordRow;
  uint *destinationDwordRow;
  int *createdPalette;
  GraphicsTextureSourceAsset *paletteBank;
  int rowsRemaining;
  uint sourceWidth;
  int alphaShiftRight;
  int blueShiftRight;
  int greenShiftRight;
  int redShiftRight;
  int alphaShiftLeft;
  int blueShiftLeft;
  int greenShiftLeft;
  int redShiftLeft;
  TH_LEGACY_LONG savedPitch;
  TH_LEGACY_LPVOID savedSurfaceBits;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    hresult = (*stagingSurface3->lpVtbl->IsLost)(stagingSurface3);
    paletteIndexOrCounter = 0;
    if (hresult != 0) {
      paletteIndexOrCounter = (*stagingSurface3->lpVtbl->Restore)(stagingSurface3);
    }
    if (paletteIndexOrCounter == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      hresult = (*stagingSurface3->lpVtbl->Lock)
                         (stagingSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,
                          (TH_LEGACY_HANDLE)0x0);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrGreenTopBit = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
        savedPitch = g_SurfaceDesc.lPitch;
        savedSurfaceBits = g_SurfaceDesc.lpSurface;
        paletteIndexOrCounter = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrGreenTopBit + -0x20);
        sourceWidth = *(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                             offsetOrGreenTopBit + -0x10);
        rowsRemaining = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrGreenTopBit + -0xc);
        offsetOrGreenTopBit = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        offsetOrGreenTopBit + -0x1c);
        if (paletteIndexOrCounter < 0) {
          sourceTexel = (word *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrGreenTopBit + -0x28);
          if ((sourceWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) {
            columnsOrMask = sourceWidth;
            byteCursor = g_SurfaceDesc.lpSurface;
            sourceTexelRow = sourceTexel;
            byteCursorOrRow = g_SurfaceDesc.lpSurface;
            if (destinationFormat->dwRGBBitCount == 8) {
              do {
                do {
                  *byteCursor = *(byte *)((int)sourceTexel + 3);
                  sourceTexel = sourceTexel + 8;
                  nextColumnsOrMask = columnsOrMask - 4;
                  hasMore = 3 < (int)columnsOrMask;
                  columnsOrMask = nextColumnsOrMask;
                  byteCursor = byteCursor + 1;
                } while (nextColumnsOrMask != 0 && hasMore);
                sourceTexel = sourceTexelRow + sourceWidth * 8;
                paletteIndexOrCounter = rowsRemaining + -4;
                hasMore = 3 < rowsRemaining;
                columnsOrMask = sourceWidth & 0xfffffff;
                byteCursor = byteCursorOrRow + destinationPitch;
                sourceTexelRow = sourceTexel;
                byteCursorOrRow = byteCursorOrRow + destinationPitch;
                rowsRemaining = paletteIndexOrCounter;
              } while (paletteIndexOrCounter != 0 && hasMore);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 0x100;
              grayPaletteEntry.red = 0;
              grayPaletteEntry.green = 0;
              grayPaletteEntry.blue = 0;
              grayPaletteEntry.flags = 0;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                *paletteEntryCursor = grayPaletteEntry;
                paletteEntryCursor = paletteEntryCursor + 1;
                grayPaletteEntry = THANDOR_BITCAST(int, DirectDrawPaletteEntry, (THANDOR_BITCAST(DirectDrawPaletteEntry, int, grayPaletteEntry) + 0x1010101));
                paletteIndexOrCounter = paletteIndexOrCounter + -1;
              } while (paletteIndexOrCounter != 0);
              hresult = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&createdPalette,
                                  (TH_LEGACY_LPVOID)0x0);
              if (hresult == 0) {
                (**(code **)(*createdPalette + 8))(createdPalette);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
            columnsOrMask = destinationFormat->dwRBitMask;
            nextColumnsOrMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((columnsOrMask != 0) && (nextColumnsOrMask != 0)) && (blueMask != 0)) {
              redShiftLeft = 0;
              if (columnsOrMask != 0) {
                for (; (columnsOrMask >> redShiftLeft & 1) == 0; redShiftLeft = redShiftLeft + 1) {
                }
              }
              greenShiftLeft = 0;
              if (nextColumnsOrMask != 0) {
                for (; (nextColumnsOrMask >> greenShiftLeft & 1) == 0; greenShiftLeft = greenShiftLeft + 1) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft = blueShiftLeft + 1) {
                }
              }
              paletteIndexOrCounter = 0x1f;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                }
              }
              offsetOrGreenTopBit = 0x1f;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit = offsetOrGreenTopBit + -1) {
                }
              }
              blueTopBit = 0x1f;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit = blueTopBit + -1) {
                }
              }
              redShiftRight = 0x18 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 0x10 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              columnsOrMask = destinationFormat->dwRGBAlphaBitMask;
              if (columnsOrMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 0x10;
              }
              else {
                alphaShiftLeft = 0;
                if (columnsOrMask != 0) {
                  for (; (columnsOrMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft = alphaShiftLeft + 1) {
                  }
                }
                paletteIndexOrCounter = 0x1f;
                if (columnsOrMask != 0) {
                  for (; columnsOrMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                  }
                }
                alphaShiftRight = 0x20 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              columnsOrMask = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 0x11) {
                do {
                  do {
                    texel00 = *(undefined4 *)sourceTexel;
                    texel01 = *(undefined4 *)(sourceTexel + 2);
                    texel02 = *(undefined4 *)(sourceTexel + sourceWidth * 2);
                    texel03 = *(undefined4 *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel04 = *(undefined4 *)(sourceTexel + 4);
                    texel05 = *(undefined4 *)(sourceTexel + 6);
                    texel06 = *(undefined4 *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel07 = *(undefined4 *)(sourceTexel + (sourceWidth + 3) * 2);
                    texel00AlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)texel00 >> 0x18) << 0x38) >> 0x30);
                    texel01Alpha = (ushort)(((ulonglong)(byte)((uint)texel01 >> 0x18) << 0x38) >> 0x30);
                    texel02Alpha = (ushort)(((ulonglong)(byte)((uint)texel02 >> 0x18) << 0x38) >> 0x30);
                    texel03Alpha = (ushort)(((ulonglong)(byte)((uint)texel03 >> 0x18) << 0x38) >> 0x30);
                    texel04Alpha = (ushort)(((ulonglong)(byte)((uint)texel04 >> 0x18) << 0x38) >> 0x30);
                    texel05Alpha = (ushort)(((ulonglong)(byte)((uint)texel05 >> 0x18) << 0x38) >> 0x30);
                    texel06Alpha = (ushort)(((ulonglong)(byte)((uint)texel06 >> 0x18) << 0x38) >> 0x30);
                    texel07Alpha = (ushort)(((ulonglong)(byte)((uint)texel07 >> 0x18) << 0x38) >> 0x30);
                    sourceTexel = sourceTexel + sourceWidth * 4;
                    texel08 = *(undefined4 *)sourceTexel;
                    texel09 = *(undefined4 *)(sourceTexel + 2);
                    texel10 = *(undefined4 *)(sourceTexel + sourceWidth * 2);
                    texel11 = *(undefined4 *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel08Alpha = (ushort)(((ulonglong)(byte)((uint)texel08 >> 0x18) << 0x38) >> 0x30);
                    texel09Alpha = (ushort)(((ulonglong)(byte)((uint)texel09 >> 0x18) << 0x38) >> 0x30);
                    texel10Alpha = (ushort)(((ulonglong)(byte)((uint)texel10 >> 0x18) << 0x38) >> 0x30);
                    texel11Alpha = (ushort)(((ulonglong)(byte)((uint)texel11 >> 0x18) << 0x38) >> 0x30);
                    texel12 = *(undefined4 *)(sourceTexel + 4);
                    texel13 = *(undefined4 *)(sourceTexel + 6);
                    texel14 = *(undefined4 *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel15 = *(undefined4 *)(sourceTexel + (sourceWidth + 3) * 2);
                    texel12Alpha = (ushort)(((ulonglong)(byte)((uint)texel12 >> 0x18) << 0x38) >> 0x30);
                    texel13Alpha = (ushort)(((ulonglong)(byte)((uint)texel13 >> 0x18) << 0x38) >> 0x30);
                    texel14Alpha = (ushort)(((ulonglong)(byte)((uint)texel14 >> 0x18) << 0x38) >> 0x30);
                    texel15Alpha = (ushort)(((ulonglong)(byte)((uint)texel15 >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)texel00 + (ushort)(byte)texel01 +
                                      (ushort)(byte)texel02 + (ushort)(byte)texel03 +
                                      (ushort)(byte)texel04 + (ushort)(byte)texel05 +
                                      (ushort)(byte)texel06 + (ushort)(byte)texel07 +
                                      (ushort)(byte)texel08 + (ushort)(byte)texel09 +
                                      (ushort)(byte)texel10 + (ushort)(byte)texel11 +
                                     (ushort)(byte)texel12 + (ushort)(byte)texel13 +
                                     (ushort)(byte)texel14 + (ushort)(byte)texel15) >> 4;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)texel00 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel01 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel02 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel03 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel04 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel05 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel06 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel07 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel08 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel09 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel12 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel13 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel14 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel15 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(texel00AlphaOrAverage,(char)((uint)texel00 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel01Alpha,(char)((uint)texel01 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel02Alpha,(char)((uint)texel02 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel03Alpha,(char)((uint)texel03 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel04Alpha,(char)((uint)texel04 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel05Alpha,(char)((uint)texel05 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel06Alpha,(char)((uint)texel06 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel07Alpha,(char)((uint)texel07 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel08Alpha,(char)((uint)texel08 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel09Alpha,(char)((uint)texel09 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel10Alpha,(char)((uint)texel10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel11Alpha,(char)((uint)texel11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel12Alpha,(char)((uint)texel12 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel13Alpha,(char)((uint)texel13 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel14Alpha,(char)((uint)texel14 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel15Alpha,(char)((uint)texel15 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    texel00AlphaOrAverage = (ushort)((texel00AlphaOrAverage >> 8) + (texel01Alpha >> 8) + (texel02Alpha >> 8) + (texel03Alpha >> 8)
                                      + (texel04Alpha >> 8) + (texel05Alpha >> 8) +
                                        (texel06Alpha >> 8) + (texel07Alpha >> 8) +
                                      (texel08Alpha >> 8) + (texel09Alpha >> 8) + (texel10Alpha >> 8) + (texel11Alpha >> 8)
                                     + (texel12Alpha >> 8) + (texel13Alpha >> 8) + (texel14Alpha >> 8) + (texel15Alpha >> 8)
                                     ) >> 4;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationWord = (ushort)((((uint)(byte)((texel00AlphaOrAverage != 0) * (texel00AlphaOrAverage < 0x100) *
                                                       (char)texel00AlphaOrAverage - (0xff < texel00AlphaOrAverage)) << 0x18) >>
                                        ((byte)alphaShiftRight & 0x1f)) << ((byte)alphaShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                                       ((byte)blueShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                                       ((byte)greenShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                                       ((byte)redShiftLeft & 0x1f));
                    sourceTexel = sourceTexel + (sourceWidth * -2 + 4) * 2;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationWord = destinationWord + 1;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  sourceTexel = sourceTexelRow + sourceWidth * 8;
                  paletteIndexOrCounter = rowsRemaining + -4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth & 0xfffffff;
                  destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationWordRow = (ushort *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
              }
              else {
                do {
                  do {
                    texel00 = *(undefined4 *)sourceTexel;
                    texel01 = *(undefined4 *)(sourceTexel + 2);
                    texel02 = *(undefined4 *)(sourceTexel + sourceWidth * 2);
                    texel03 = *(undefined4 *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel04 = *(undefined4 *)(sourceTexel + 4);
                    texel05 = *(undefined4 *)(sourceTexel + 6);
                    texel06 = *(undefined4 *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel07 = *(undefined4 *)(sourceTexel + (sourceWidth + 3) * 2);
                    texel00AlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)texel00 >> 0x18) << 0x38) >> 0x30);
                    texel01Alpha = (ushort)(((ulonglong)(byte)((uint)texel01 >> 0x18) << 0x38) >> 0x30);
                    texel02Alpha = (ushort)(((ulonglong)(byte)((uint)texel02 >> 0x18) << 0x38) >> 0x30);
                    texel03Alpha = (ushort)(((ulonglong)(byte)((uint)texel03 >> 0x18) << 0x38) >> 0x30);
                    texel04Alpha = (ushort)(((ulonglong)(byte)((uint)texel04 >> 0x18) << 0x38) >> 0x30);
                    texel05Alpha = (ushort)(((ulonglong)(byte)((uint)texel05 >> 0x18) << 0x38) >> 0x30);
                    texel06Alpha = (ushort)(((ulonglong)(byte)((uint)texel06 >> 0x18) << 0x38) >> 0x30);
                    texel07Alpha = (ushort)(((ulonglong)(byte)((uint)texel07 >> 0x18) << 0x38) >> 0x30);
                    sourceTexel = sourceTexel + sourceWidth * 4;
                    texel08 = *(undefined4 *)sourceTexel;
                    texel09 = *(undefined4 *)(sourceTexel + 2);
                    texel10 = *(undefined4 *)(sourceTexel + sourceWidth * 2);
                    texel11 = *(undefined4 *)(sourceTexel + (sourceWidth + 1) * 2);
                    texel08Alpha = (ushort)(((ulonglong)(byte)((uint)texel08 >> 0x18) << 0x38) >> 0x30);
                    texel09Alpha = (ushort)(((ulonglong)(byte)((uint)texel09 >> 0x18) << 0x38) >> 0x30);
                    texel10Alpha = (ushort)(((ulonglong)(byte)((uint)texel10 >> 0x18) << 0x38) >> 0x30);
                    texel11Alpha = (ushort)(((ulonglong)(byte)((uint)texel11 >> 0x18) << 0x38) >> 0x30);
                    texel12 = *(undefined4 *)(sourceTexel + 4);
                    texel13 = *(undefined4 *)(sourceTexel + 6);
                    texel14 = *(undefined4 *)(sourceTexel + (sourceWidth + 2) * 2);
                    texel15 = *(undefined4 *)(sourceTexel + (sourceWidth + 3) * 2);
                    texel12Alpha = (ushort)(((ulonglong)(byte)((uint)texel12 >> 0x18) << 0x38) >> 0x30);
                    texel13Alpha = (ushort)(((ulonglong)(byte)((uint)texel13 >> 0x18) << 0x38) >> 0x30);
                    texel14Alpha = (ushort)(((ulonglong)(byte)((uint)texel14 >> 0x18) << 0x38) >> 0x30);
                    texel15Alpha = (ushort)(((ulonglong)(byte)((uint)texel15 >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)texel00 + (ushort)(byte)texel01 +
                                      (ushort)(byte)texel02 + (ushort)(byte)texel03 +
                                      (ushort)(byte)texel04 + (ushort)(byte)texel05 +
                                      (ushort)(byte)texel06 + (ushort)(byte)texel07 +
                                      (ushort)(byte)texel08 + (ushort)(byte)texel09 +
                                      (ushort)(byte)texel10 + (ushort)(byte)texel11 +
                                     (ushort)(byte)texel12 + (ushort)(byte)texel13 +
                                     (ushort)(byte)texel14 + (ushort)(byte)texel15) >> 4;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)texel00 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel01 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel02 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel03 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel04 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel05 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel06 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel07 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel08 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel09 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel12 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel13 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel14 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel15 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(texel00AlphaOrAverage,(char)((uint)texel00 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel01Alpha,(char)((uint)texel01 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel02Alpha,(char)((uint)texel02 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel03Alpha,(char)((uint)texel03 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel04Alpha,(char)((uint)texel04 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel05Alpha,(char)((uint)texel05 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel06Alpha,(char)((uint)texel06 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel07Alpha,(char)((uint)texel07 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel08Alpha,(char)((uint)texel08 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel09Alpha,(char)((uint)texel09 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel10Alpha,(char)((uint)texel10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel11Alpha,(char)((uint)texel11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel12Alpha,(char)((uint)texel12 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel13Alpha,(char)((uint)texel13 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel14Alpha,(char)((uint)texel14 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel15Alpha,(char)((uint)texel15 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    texel00AlphaOrAverage = (ushort)((texel00AlphaOrAverage >> 8) + (texel01Alpha >> 8) + (texel02Alpha >> 8) + (texel03Alpha >> 8)
                                      + (texel04Alpha >> 8) + (texel05Alpha >> 8) +
                                        (texel06Alpha >> 8) + (texel07Alpha >> 8) +
                                      (texel08Alpha >> 8) + (texel09Alpha >> 8) + (texel10Alpha >> 8) + (texel11Alpha >> 8)
                                     + (texel12Alpha >> 8) + (texel13Alpha >> 8) + (texel14Alpha >> 8) + (texel15Alpha >> 8)
                                     ) >> 4;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationDword = (((uint)(byte)((texel00AlphaOrAverage != 0) * (texel00AlphaOrAverage < 0x100) * (char)texel00AlphaOrAverage -
                                             (0xff < texel00AlphaOrAverage)) << 0x18) >> ((byte)alphaShiftRight & 0x1f))
                               << ((byte)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                               ((byte)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                               ((byte)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                               ((byte)redShiftLeft & 0x1f);
                    sourceTexel = sourceTexel + (sourceWidth * -2 + 4) * 2;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationDword = destinationDword + 1;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  sourceTexel = sourceTexelRow + sourceWidth * 8;
                  paletteIndexOrCounter = rowsRemaining + -4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth & 0xfffffff;
                  destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
                  sourceTexelRow = sourceTexel;
                  destinationDwordRow = (uint *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
              }
            }
          }
        }
        else {
          byteCursor = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + offsetOrGreenTopBit + -0x28
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
                  byteCursorOrRow = byteCursorOrRow + 1;
                } while (nextColumnsOrMask != 0 && hasMore);
                byteCursor = sourceByteRow + sourceWidth * 4;
                paletteIndexOrCounter = rowsRemaining + -4;
                hasMore = 3 < rowsRemaining;
                columnsOrMask = sourceWidth;
                byteCursorOrRow = destinationByteRow + destinationPitch;
                sourceByteRow = byteCursor;
                destinationByteRow = destinationByteRow + destinationPitch;
                rowsRemaining = paletteIndexOrCounter;
              } while (paletteIndexOrCounter != 0 && hasMore);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              paletteIndexOrCounter = 0x100;
              paletteSourceCursor = paletteBank;
              paletteEntryCursor = g_TexturePaletteEntries;
              do {
                paletteGreen = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 1);
                paletteBlue = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 2);
                paletteFlags = *(undefined1 *)((int)&(paletteSourceCursor->common).magic + 3);
                paletteEntryCursor->red = *(undefined1 *)&(paletteSourceCursor->common).magic;
                paletteEntryCursor->green = paletteGreen;
                paletteEntryCursor->blue = paletteBlue;
                paletteEntryCursor->flags = paletteFlags;
                paletteSourceCursor = (GraphicsTextureSourceAsset *)&(paletteSourceCursor->common).formatVersion;
                paletteEntryCursor = paletteEntryCursor + 1;
                paletteIndexOrCounter = paletteIndexOrCounter + -1;
              } while (paletteIndexOrCounter != 0);
              hresult = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&createdPalette,
                                  (TH_LEGACY_LPVOID)0x0);
              if (hresult == 0) {
                (**(code **)(*createdPalette + 8))(createdPalette);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
            columnsOrMask = destinationFormat->dwRBitMask;
            nextColumnsOrMask = destinationFormat->dwGBitMask;
            blueMask = destinationFormat->dwBBitMask;
            if (((columnsOrMask != 0) && (nextColumnsOrMask != 0)) && (blueMask != 0)) {
              redShiftLeft = 0;
              if (columnsOrMask != 0) {
                for (; (columnsOrMask >> redShiftLeft & 1) == 0; redShiftLeft = redShiftLeft + 1) {
                }
              }
              greenShiftLeft = 0;
              if (nextColumnsOrMask != 0) {
                for (; (nextColumnsOrMask >> greenShiftLeft & 1) == 0; greenShiftLeft = greenShiftLeft + 1) {
                }
              }
              blueShiftLeft = 0;
              if (blueMask != 0) {
                for (; (blueMask >> blueShiftLeft & 1) == 0; blueShiftLeft = blueShiftLeft + 1) {
                }
              }
              paletteIndexOrCounter = 0x1f;
              if (destinationFormat->dwRBitMask != 0) {
                for (; destinationFormat->dwRBitMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                }
              }
              offsetOrGreenTopBit = 0x1f;
              if (destinationFormat->dwGBitMask != 0) {
                for (; destinationFormat->dwGBitMask >> offsetOrGreenTopBit == 0; offsetOrGreenTopBit = offsetOrGreenTopBit + -1) {
                }
              }
              blueTopBit = 0x1f;
              if (destinationFormat->dwBBitMask != 0) {
                for (; destinationFormat->dwBBitMask >> blueTopBit == 0; blueTopBit = blueTopBit + -1) {
                }
              }
              redShiftRight = 0x18 - ((paletteIndexOrCounter + 1) - redShiftLeft);
              greenShiftRight = 0x10 - ((offsetOrGreenTopBit + 1) - greenShiftLeft);
              blueShiftRight = 8 - ((blueTopBit + 1) - blueShiftLeft);
              columnsOrMask = destinationFormat->dwRGBAlphaBitMask;
              if (columnsOrMask == 0) {
                alphaShiftRight = 0;
                alphaShiftLeft = 0x10;
              }
              else {
                alphaShiftLeft = 0;
                if (columnsOrMask != 0) {
                  for (; (columnsOrMask >> alphaShiftLeft & 1) == 0; alphaShiftLeft = alphaShiftLeft + 1) {
                  }
                }
                paletteIndexOrCounter = 0x1f;
                if (columnsOrMask != 0) {
                  for (; columnsOrMask >> paletteIndexOrCounter == 0; paletteIndexOrCounter = paletteIndexOrCounter + -1) {
                  }
                }
                alphaShiftRight = 0x20 - ((paletteIndexOrCounter + 1) - alphaShiftLeft);
              }
              columnsOrMask = sourceWidth;
              destinationDword = g_SurfaceDesc.lpSurface;
              destinationWord = g_SurfaceDesc.lpSurface;
              byteCursorOrRow = byteCursor;
              destinationDwordRow = g_SurfaceDesc.lpSurface;
              destinationWordRow = g_SurfaceDesc.lpSurface;
              if (destinationFormat->dwRGBBitCount < 0x11) {
                do {
                  do {
                    texel00 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*byteCursor * 8 + -0x28);
                    texel01 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[1] * 8 + -0x28);
                    texel02 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth] * 8 + -0x28);
                    texel03 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth + 1] * 8 + -0x28);
                    texel04 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[2] * 8 + -0x28);
                    texel05 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[3] * 8 + -0x28);
                    texel06 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 2] * 8 + -0x28);
                    texel07 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 3] * 8 + -0x28);
                    texel00AlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)texel00 >> 0x18) << 0x38) >> 0x30);
                    texel01Alpha = (ushort)(((ulonglong)(byte)((uint)texel01 >> 0x18) << 0x38) >> 0x30);
                    texel02Alpha = (ushort)(((ulonglong)(byte)((uint)texel02 >> 0x18) << 0x38) >> 0x30);
                    texel03Alpha = (ushort)(((ulonglong)(byte)((uint)texel03 >> 0x18) << 0x38) >> 0x30);
                    texel04Alpha = (ushort)(((ulonglong)(byte)((uint)texel04 >> 0x18) << 0x38) >> 0x30);
                    texel05Alpha = (ushort)(((ulonglong)(byte)((uint)texel05 >> 0x18) << 0x38) >> 0x30);
                    texel06Alpha = (ushort)(((ulonglong)(byte)((uint)texel06 >> 0x18) << 0x38) >> 0x30);
                    texel07Alpha = (ushort)(((ulonglong)(byte)((uint)texel07 >> 0x18) << 0x38) >> 0x30);
                    byteCursor = byteCursor + sourceWidth * 2;
                    texel08 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)*byteCursor * 8 + -0x28);
                    texel09 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[1] * 8 + -0x28);
                    texel10 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth] * 8 + -0x28);
                    texel11 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 1] * 8 + -0x28);
                    texel08Alpha = (ushort)(((ulonglong)(byte)((uint)texel08 >> 0x18) << 0x38) >> 0x30);
                    texel09Alpha = (ushort)(((ulonglong)(byte)((uint)texel09 >> 0x18) << 0x38) >> 0x30);
                    texel10Alpha = (ushort)(((ulonglong)(byte)((uint)texel10 >> 0x18) << 0x38) >> 0x30);
                    texel11Alpha = (ushort)(((ulonglong)(byte)((uint)texel11 >> 0x18) << 0x38) >> 0x30);
                    texel12 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[2] * 8 + -0x28);
                    texel13 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[3] * 8 + -0x28);
                    texel14 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 2] * 8 + -0x28);
                    texel15 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 3] * 8 + -0x28);
                    texel12Alpha = (ushort)(((ulonglong)(byte)((uint)texel12 >> 0x18) << 0x38) >> 0x30);
                    texel13Alpha = (ushort)(((ulonglong)(byte)((uint)texel13 >> 0x18) << 0x38) >> 0x30);
                    texel14Alpha = (ushort)(((ulonglong)(byte)((uint)texel14 >> 0x18) << 0x38) >> 0x30);
                    texel15Alpha = (ushort)(((ulonglong)(byte)((uint)texel15 >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)texel00 + (ushort)(byte)texel01 +
                                      (ushort)(byte)texel02 + (ushort)(byte)texel03 +
                                      (ushort)(byte)texel04 + (ushort)(byte)texel05 +
                                      (ushort)(byte)texel06 + (ushort)(byte)texel07 +
                                      (ushort)(byte)texel08 + (ushort)(byte)texel09 +
                                      (ushort)(byte)texel10 + (ushort)(byte)texel11 +
                                     (ushort)(byte)texel12 + (ushort)(byte)texel13 +
                                     (ushort)(byte)texel14 + (ushort)(byte)texel15) >> 4;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)texel00 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel01 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel02 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel03 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel04 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel05 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel06 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel07 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel08 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel09 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel12 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel13 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel14 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel15 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(texel00AlphaOrAverage,(char)((uint)texel00 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel01Alpha,(char)((uint)texel01 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel02Alpha,(char)((uint)texel02 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel03Alpha,(char)((uint)texel03 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel04Alpha,(char)((uint)texel04 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel05Alpha,(char)((uint)texel05 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel06Alpha,(char)((uint)texel06 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel07Alpha,(char)((uint)texel07 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel08Alpha,(char)((uint)texel08 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel09Alpha,(char)((uint)texel09 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel10Alpha,(char)((uint)texel10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel11Alpha,(char)((uint)texel11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel12Alpha,(char)((uint)texel12 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel13Alpha,(char)((uint)texel13 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel14Alpha,(char)((uint)texel14 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel15Alpha,(char)((uint)texel15 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    texel00AlphaOrAverage = (ushort)((texel00AlphaOrAverage >> 8) + (texel01Alpha >> 8) + (texel02Alpha >> 8) + (texel03Alpha >> 8)
                                      + (texel04Alpha >> 8) + (texel05Alpha >> 8) +
                                        (texel06Alpha >> 8) + (texel07Alpha >> 8) +
                                      (texel08Alpha >> 8) + (texel09Alpha >> 8) + (texel10Alpha >> 8) + (texel11Alpha >> 8)
                                     + (texel12Alpha >> 8) + (texel13Alpha >> 8) + (texel14Alpha >> 8) + (texel15Alpha >> 8)
                                     ) >> 4;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationWord = (ushort)((((uint)(byte)((texel00AlphaOrAverage != 0) * (texel00AlphaOrAverage < 0x100) *
                                                       (char)texel00AlphaOrAverage - (0xff < texel00AlphaOrAverage)) << 0x18) >>
                                        ((byte)alphaShiftRight & 0x1f)) << ((byte)alphaShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                                       ((byte)blueShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                                       ((byte)greenShiftLeft & 0x1f)) |
                               (ushort)(((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                                       ((byte)redShiftLeft & 0x1f));
                    byteCursor = byteCursor + sourceWidth * -2 + 4;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationWord = destinationWord + 1;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 4;
                  paletteIndexOrCounter = rowsRemaining + -4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth;
                  destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationWordRow = (ushort *)((int)destinationWordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              }
              else {
                do {
                  do {
                    texel00 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*byteCursor * 8 + -0x28);
                    texel01 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[1] * 8 + -0x28);
                    texel02 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth] * 8 + -0x28);
                    texel03 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[sourceWidth + 1] * 8 + -0x28);
                    texel04 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[2] * 8 + -0x28);
                    texel05 = *(undefined4 *)
                             ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)byteCursor[3] * 8 + -0x28);
                    texel06 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 2] * 8 + -0x28);
                    texel07 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 3] * 8 + -0x28);
                    texel00AlphaOrAverage = (ushort)(((ulonglong)(byte)((uint)texel00 >> 0x18) << 0x38) >> 0x30);
                    texel01Alpha = (ushort)(((ulonglong)(byte)((uint)texel01 >> 0x18) << 0x38) >> 0x30);
                    texel02Alpha = (ushort)(((ulonglong)(byte)((uint)texel02 >> 0x18) << 0x38) >> 0x30);
                    texel03Alpha = (ushort)(((ulonglong)(byte)((uint)texel03 >> 0x18) << 0x38) >> 0x30);
                    texel04Alpha = (ushort)(((ulonglong)(byte)((uint)texel04 >> 0x18) << 0x38) >> 0x30);
                    texel05Alpha = (ushort)(((ulonglong)(byte)((uint)texel05 >> 0x18) << 0x38) >> 0x30);
                    texel06Alpha = (ushort)(((ulonglong)(byte)((uint)texel06 >> 0x18) << 0x38) >> 0x30);
                    texel07Alpha = (ushort)(((ulonglong)(byte)((uint)texel07 >> 0x18) << 0x38) >> 0x30);
                    byteCursor = byteCursor + sourceWidth * 2;
                    texel08 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)*byteCursor * 8 + -0x28);
                    texel09 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[1] * 8 + -0x28);
                    texel10 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth] * 8 + -0x28);
                    texel11 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 1] * 8 + -0x28);
                    texel08Alpha = (ushort)(((ulonglong)(byte)((uint)texel08 >> 0x18) << 0x38) >> 0x30);
                    texel09Alpha = (ushort)(((ulonglong)(byte)((uint)texel09 >> 0x18) << 0x38) >> 0x30);
                    texel10Alpha = (ushort)(((ulonglong)(byte)((uint)texel10 >> 0x18) << 0x38) >> 0x30);
                    texel11Alpha = (ushort)(((ulonglong)(byte)((uint)texel11 >> 0x18) << 0x38) >> 0x30);
                    texel12 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[2] * 8 + -0x28);
                    texel13 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[3] * 8 + -0x28);
                    texel14 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 2] * 8 + -0x28);
                    texel15 = *(undefined4 *)
                              ((paletteBank->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)byteCursor[sourceWidth + 3] * 8 + -0x28);
                    texel12Alpha = (ushort)(((ulonglong)(byte)((uint)texel12 >> 0x18) << 0x38) >> 0x30);
                    texel13Alpha = (ushort)(((ulonglong)(byte)((uint)texel13 >> 0x18) << 0x38) >> 0x30);
                    texel14Alpha = (ushort)(((ulonglong)(byte)((uint)texel14 >> 0x18) << 0x38) >> 0x30);
                    texel15Alpha = (ushort)(((ulonglong)(byte)((uint)texel15 >> 0x18) << 0x38) >> 0x30);
                    averageBlue = (ushort)((ushort)(byte)texel00 + (ushort)(byte)texel01 +
                                      (ushort)(byte)texel02 + (ushort)(byte)texel03 +
                                      (ushort)(byte)texel04 + (ushort)(byte)texel05 +
                                      (ushort)(byte)texel06 + (ushort)(byte)texel07 +
                                      (ushort)(byte)texel08 + (ushort)(byte)texel09 +
                                      (ushort)(byte)texel10 + (ushort)(byte)texel11 +
                                     (ushort)(byte)texel12 + (ushort)(byte)texel13 +
                                     (ushort)(byte)texel14 + (ushort)(byte)texel15) >> 4;
                    averageGreen = (ushort)(((ushort)(((ulonglong)(byte)((uint)texel00 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel01 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel02 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel03 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel04 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel05 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel06 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel07 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel08 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel09 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)texel11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel12 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel13 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel14 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)texel15 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    averageRed = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(texel00AlphaOrAverage,(char)((uint)texel00 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel01Alpha,(char)((uint)texel01 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel02Alpha,(char)((uint)texel02 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel03Alpha,(char)((uint)texel03 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel04Alpha,(char)((uint)texel04 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel05Alpha,(char)((uint)texel05 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel06Alpha,(char)((uint)texel06 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel07Alpha,(char)((uint)texel07 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel08Alpha,(char)((uint)texel08 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel09Alpha,(char)((uint)texel09 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel10Alpha,(char)((uint)texel10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(texel11Alpha,(char)((uint)texel11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel12Alpha,(char)((uint)texel12 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel13Alpha,(char)((uint)texel13 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel14Alpha,(char)((uint)texel14 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(texel15Alpha,(char)((uint)texel15 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    texel00AlphaOrAverage = (ushort)((texel00AlphaOrAverage >> 8) + (texel01Alpha >> 8) + (texel02Alpha >> 8) + (texel03Alpha >> 8)
                                      + (texel04Alpha >> 8) + (texel05Alpha >> 8) +
                                        (texel06Alpha >> 8) + (texel07Alpha >> 8) +
                                      (texel08Alpha >> 8) + (texel09Alpha >> 8) + (texel10Alpha >> 8) + (texel11Alpha >> 8)
                                     + (texel12Alpha >> 8) + (texel13Alpha >> 8) + (texel14Alpha >> 8) + (texel15Alpha >> 8)
                                     ) >> 4;
                    averageRgb = CONCAT12((averageRed != 0) * (averageRed < 0x100) * (char)averageRed -
                                      (0xff < averageRed),
                                      CONCAT11((averageGreen != 0) * (averageGreen < 0x100) * (char)averageGreen -
                                               (0xff < averageGreen),
                                               (averageBlue != 0) * (averageBlue < 0x100) * (char)averageBlue -
                                               (0xff < averageBlue)));
                    *destinationDword = (((uint)(byte)((texel00AlphaOrAverage != 0) * (texel00AlphaOrAverage < 0x100) * (char)texel00AlphaOrAverage -
                                             (0xff < texel00AlphaOrAverage)) << 0x18) >> ((byte)alphaShiftRight & 0x1f))
                               << ((byte)alphaShiftLeft & 0x1f) |
                               ((averageRgb & 0xff) >> ((byte)blueShiftRight & 0x1f)) <<
                               ((byte)blueShiftLeft & 0x1f) |
                               ((averageRgb & 0xff00) >> ((byte)greenShiftRight & 0x1f)) <<
                               ((byte)greenShiftLeft & 0x1f) |
                               ((averageRgb & 0xff0000) >> ((byte)redShiftRight & 0x1f)) <<
                               ((byte)redShiftLeft & 0x1f);
                    byteCursor = byteCursor + sourceWidth * -2 + 4;
                    nextColumnsOrMask = columnsOrMask - 4;
                    hasMore = 3 < (int)columnsOrMask;
                    columnsOrMask = nextColumnsOrMask;
                    destinationDword = destinationDword + 1;
                  } while (nextColumnsOrMask != 0 && hasMore);
                  byteCursor = byteCursorOrRow + sourceWidth * 4;
                  paletteIndexOrCounter = rowsRemaining + -4;
                  hasMore = 3 < rowsRemaining;
                  columnsOrMask = sourceWidth;
                  destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
                  byteCursorOrRow = byteCursor;
                  destinationDwordRow = (uint *)((int)destinationDwordRow + destinationPitch);
                  rowsRemaining = paletteIndexOrCounter;
                } while (paletteIndexOrCounter != 0 && hasMore);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
          }
        }
        (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,surfaceBits);
      }
    }
  }
GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads = g_ActiveTextureUploads - 1;
  return;
}


/* Address: 0x0057C6C0.
   Ownership: graphics/resources/texture.
   Purpose: Restores and locks texture->stagingSurface3, treats sourceEntry.dataOffset as a one-byte-per-pixel mask
   plane, packs the mask into the destination alpha field, forces every destination RGB mask bit to one, unlocks
   the surface, and brackets the operation with g_ActiveTextureUploads. The function intentionally skips 8-bit
   destination surfaces. The 1x path packs each source mask byte directly.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadAlpha_1x(GraphicsTextureResource *texture)

{
  IDirectDrawSurface3 *This;
  GraphicsTextureSourceAsset *textureSource;
  GraphicsSubresourceIndex textureSubresource;
  DDPIXELFORMAT *destinationFormat;
  byte rotateShift;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  TH_LEGACY_HRESULT hresult;
  int restoreResultOrWidth;
  byte alphaShift;
  uint rgbMaskBits;
  byte *maskCursor;
  ushort *destinationWord;
  uint *destinationDword;
  int offsetOrCounter;
  ushort *destinationWordRow;
  uint *destinationDwordRow;
  int rowsRemaining;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  This = texture->stagingSurface3;
  textureSource = texture->sourceAsset;
  textureSubresource = texture->subresourceIndex;
  if (This != (IDirectDrawSurface3 *)0x0) {
    hresult = (*This->lpVtbl->IsLost)(This);
    restoreResultOrWidth = 0;
    if (hresult != 0) {
      restoreResultOrWidth = (*This->lpVtbl->Restore)(This);
    }
    if (restoreResultOrWidth == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      hresult = (*This->lpVtbl->Lock)
                        (This,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrCounter = textureSubresource * 0x20 + (textureSource->tableDescriptor).subresourceTableOffset;
        restoreResultOrWidth = *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                        offsetOrCounter + -0x10);
        rowsRemaining = *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrCounter + -0xc);
        maskCursor = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                          offsetOrCounter + -0x1c) + -0x28;
        if (((restoreResultOrWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) &&
           (destinationFormat->dwRGBBitCount != 8)) {
          offsetOrCounter = 0x1f;
          if (destinationFormat->dwRGBAlphaBitMask != 0) {
            for (; destinationFormat->dwRGBAlphaBitMask >> offsetOrCounter == 0; offsetOrCounter = offsetOrCounter + -1) {
            }
          }
          alphaShift = (char)offsetOrCounter - 7;
          rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
          destinationDword = g_SurfaceDesc.lpSurface;
          destinationWord = g_SurfaceDesc.lpSurface;
          offsetOrCounter = restoreResultOrWidth;
          destinationDwordRow = g_SurfaceDesc.lpSurface;
          destinationWordRow = g_SurfaceDesc.lpSurface;
          if (destinationFormat->dwRGBBitCount < 0x11) {
            do {
              do {
                rotateShift = alphaShift & 0x1f;
                *destinationWord = (ushort)*maskCursor << rotateShift | (ushort)(*maskCursor >> 0x20 - rotateShift) |
                           (ushort)rgbMaskBits;
                maskCursor = maskCursor + 1;
                offsetOrCounter = offsetOrCounter + -1;
                destinationWord = destinationWord + 1;
              } while (offsetOrCounter != 0);
              destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
              rowsRemaining = rowsRemaining + -1;
              offsetOrCounter = restoreResultOrWidth;
              destinationWordRow = destinationWord;
            } while (rowsRemaining != 0);
            (*This->lpVtbl->Unlock)(This,surfaceBits);
            goto GraphicsTextureUploadAlpha1x_DecrementActiveCountAndReturn;
          }
          do {
            do {
              rotateShift = alphaShift & 0x1f;
              *destinationDword = (uint)*maskCursor << rotateShift | (uint)(*maskCursor >> 0x20 - rotateShift) | rgbMaskBits;
              maskCursor = maskCursor + 1;
              offsetOrCounter = offsetOrCounter + -1;
              destinationDword = (uint *)((int)destinationDword + 2);
            } while (offsetOrCounter != 0);
            destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
            rowsRemaining = rowsRemaining + -1;
            offsetOrCounter = restoreResultOrWidth;
            destinationDwordRow = destinationDword;
          } while (rowsRemaining != 0);
        }
        (*This->lpVtbl->Unlock)(This,surfaceBits);
      }
    }
  }
GraphicsTextureUploadAlpha1x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads = g_ActiveTextureUploads - 1;
  return;
}


/* Address: 0x0057C890.
   Ownership: graphics/resources/texture.
   Purpose: Restores and locks texture->stagingSurface3, treats sourceEntry.dataOffset as a one-byte-per-pixel mask
   plane, packs the mask into the destination alpha field, forces every destination RGB mask bit to one, unlocks
   the surface, and brackets the operation with g_ActiveTextureUploads. The function intentionally skips 8-bit
   destination surfaces. The 2x path sums the four bytes at (0,0), (1,0), (0,1), and (1,1), then scales the 10-bit
   sum into the alpha mask while advancing two source pixels and rows.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadAlpha_2x(GraphicsTextureResource *texture)

{
  int nextRemaining;
  IDirectDrawSurface3 *This;
  GraphicsTextureSourceAsset *textureSource;
  GraphicsSubresourceIndex textureSubresource;
  DDPIXELFORMAT *destinationFormat;
  bool hasMore;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  byte partialSumOrShift;
  byte partialSum;
  ushort maskSum;
  TH_LEGACY_HRESULT hresult;
  int restoreResultOrWidth;
  byte alphaShift;
  uint rgbMaskBits;
  byte *maskCursor;
  ushort *destinationWord;
  uint *destinationDword;
  int offsetOrCounter;
  ushort *destinationWordRow;
  uint *destinationDwordRow;
  int rowsRemaining;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  This = texture->stagingSurface3;
  textureSource = texture->sourceAsset;
  textureSubresource = texture->subresourceIndex;
  if (This != (IDirectDrawSurface3 *)0x0) {
    hresult = (*This->lpVtbl->IsLost)(This);
    restoreResultOrWidth = 0;
    if (hresult != 0) {
      restoreResultOrWidth = (*This->lpVtbl->Restore)(This);
    }
    if (restoreResultOrWidth == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      hresult = (*This->lpVtbl->Lock)
                         (This,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrCounter = textureSubresource * 0x20 + (textureSource->tableDescriptor).subresourceTableOffset;
        restoreResultOrWidth = *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrCounter + -0x10);
        rowsRemaining = *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrCounter + -0xc);
        maskCursor = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                          offsetOrCounter + -0x1c) + -0x28;
        if (((restoreResultOrWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) &&
           (destinationFormat->dwRGBBitCount != 8)) {
          offsetOrCounter = 0x1f;
          if (destinationFormat->dwRGBAlphaBitMask != 0) {
            for (; destinationFormat->dwRGBAlphaBitMask >> offsetOrCounter == 0; offsetOrCounter = offsetOrCounter + -1) {
            }
          }
          alphaShift = (char)offsetOrCounter - 9;
          rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
          destinationDword = g_SurfaceDesc.lpSurface;
          destinationWord = g_SurfaceDesc.lpSurface;
          offsetOrCounter = restoreResultOrWidth;
          destinationDwordRow = g_SurfaceDesc.lpSurface;
          destinationWordRow = g_SurfaceDesc.lpSurface;
          if (destinationFormat->dwRGBBitCount < 0x11) {
            do {
              do {
                partialSumOrShift = *maskCursor + maskCursor[1];
                partialSum = partialSumOrShift + maskCursor[restoreResultOrWidth];
                maskSum = CONCAT11(CARRY1(*maskCursor,maskCursor[1]) + CARRY1(partialSumOrShift,maskCursor[restoreResultOrWidth]) +
                                 CARRY1(partialSum,maskCursor[restoreResultOrWidth + 1]),partialSum + maskCursor[restoreResultOrWidth + 1]);
                partialSumOrShift = alphaShift & 0x1f;
                *destinationWord = maskSum << partialSumOrShift | maskSum >> 0x20 - partialSumOrShift | (ushort)rgbMaskBits;
                maskCursor = maskCursor + 2;
                nextRemaining = offsetOrCounter + -2;
                hasMore = 1 < offsetOrCounter;
                destinationWord = destinationWord + 1;
                offsetOrCounter = nextRemaining;
              } while (nextRemaining != 0 && hasMore);
              maskCursor = maskCursor + restoreResultOrWidth;
              destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
              nextRemaining = rowsRemaining + -2;
              hasMore = 1 < rowsRemaining;
              offsetOrCounter = restoreResultOrWidth;
              destinationWordRow = destinationWord;
              rowsRemaining = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
            (*This->lpVtbl->Unlock)(This,surfaceBits);
            goto GraphicsTextureUploadAlpha2x_DecrementActiveCountAndReturn;
          }
          do {
            do {
              partialSumOrShift = *maskCursor + maskCursor[1];
              partialSum = partialSumOrShift + maskCursor[restoreResultOrWidth];
              maskSum = CONCAT11(CARRY1(*maskCursor,maskCursor[1]) + CARRY1(partialSumOrShift,maskCursor[restoreResultOrWidth]) +
                               CARRY1(partialSum,maskCursor[restoreResultOrWidth + 1]),partialSum + maskCursor[restoreResultOrWidth + 1]);
              partialSumOrShift = alphaShift & 0x1f;
              *destinationDword = (uint)maskSum << partialSumOrShift | (uint)(maskSum >> 0x20 - partialSumOrShift) | rgbMaskBits;
              maskCursor = maskCursor + 2;
              nextRemaining = offsetOrCounter + -2;
              hasMore = 1 < offsetOrCounter;
              destinationDword = (uint *)((int)destinationDword + 2);
              offsetOrCounter = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
            maskCursor = maskCursor + restoreResultOrWidth;
            destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
            nextRemaining = rowsRemaining + -2;
            hasMore = 1 < rowsRemaining;
            offsetOrCounter = restoreResultOrWidth;
            destinationDwordRow = destinationDword;
            rowsRemaining = nextRemaining;
          } while (nextRemaining != 0 && hasMore);
        }
        (*This->lpVtbl->Unlock)(This,surfaceBits);
      }
    }
  }
GraphicsTextureUploadAlpha2x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads = g_ActiveTextureUploads - 1;
  return;
}


/* Address: 0x0057CAA0.
   Ownership: graphics/resources/texture.
   Purpose: Restores and locks texture->stagingSurface3, treats sourceEntry.dataOffset as a one-byte-per-pixel mask
   plane, packs the mask into the destination alpha field, forces every destination RGB mask bit to one, unlocks
   the surface, and brackets the operation with g_ActiveTextureUploads. The function intentionally skips 8-bit
   destination surfaces. The 4x path is not a full 16-sample box average. It uses four taps at (0,0), (2,0), (0,2),
   and (2,2), scales their sum into the alpha mask, then advances four source pixels and rows.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_UploadAlpha_4x(GraphicsTextureResource *texture)

{
  int nextRemaining;
  IDirectDrawSurface3 *This;
  GraphicsTextureSourceAsset *textureSource;
  GraphicsSubresourceIndex textureSubresource;
  DDPIXELFORMAT *destinationFormat;
  bool hasMore;
  TH_LEGACY_LONG destinationPitch;
  TH_LEGACY_LPVOID surfaceBits;
  byte partialSumOrShift;
  byte partialSum;
  ushort maskSum;
  TH_LEGACY_HRESULT hresult;
  int restoreResultOrWidth;
  byte alphaShift;
  uint rgbMaskBits;
  byte *maskCursor;
  ushort *destinationWord;
  uint *destinationDword;
  int offsetOrCounter;
  ushort *destinationWordRow;
  uint *destinationDwordRow;
  int rowsRemaining;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  This = texture->stagingSurface3;
  textureSource = texture->sourceAsset;
  textureSubresource = texture->subresourceIndex;
  if (This != (IDirectDrawSurface3 *)0x0) {
    hresult = (*This->lpVtbl->IsLost)(This);
    restoreResultOrWidth = 0;
    if (hresult != 0) {
      restoreResultOrWidth = (*This->lpVtbl->Restore)(This);
    }
    if (restoreResultOrWidth == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      hresult = (*This->lpVtbl->Lock)
                         (This,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0);
      surfaceBits = g_SurfaceDesc.lpSurface;
      destinationPitch = g_SurfaceDesc.lPitch;
      if (hresult == 0) {
        offsetOrCounter = textureSubresource * 0x20 + (textureSource->tableDescriptor).subresourceTableOffset;
        restoreResultOrWidth = *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                         offsetOrCounter + -0x10);
        rowsRemaining = *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                            offsetOrCounter + -0xc);
        maskCursor = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                          offsetOrCounter + -0x1c) + -0x28;
        if (((restoreResultOrWidth != 0) && (destinationFormat = texture->pixelFormat, rowsRemaining != 0)) &&
           (destinationFormat->dwRGBBitCount != 8)) {
          offsetOrCounter = 0x1f;
          if (destinationFormat->dwRGBAlphaBitMask != 0) {
            for (; destinationFormat->dwRGBAlphaBitMask >> offsetOrCounter == 0; offsetOrCounter = offsetOrCounter + -1) {
            }
          }
          alphaShift = (char)offsetOrCounter - 9;
          rgbMaskBits = destinationFormat->dwRBitMask | destinationFormat->dwGBitMask | destinationFormat->dwBBitMask;
          destinationDword = g_SurfaceDesc.lpSurface;
          destinationWord = g_SurfaceDesc.lpSurface;
          offsetOrCounter = restoreResultOrWidth;
          destinationDwordRow = g_SurfaceDesc.lpSurface;
          destinationWordRow = g_SurfaceDesc.lpSurface;
          if (destinationFormat->dwRGBBitCount < 0x11) {
            do {
              do {
                partialSumOrShift = *maskCursor + maskCursor[2];
                partialSum = partialSumOrShift + maskCursor[restoreResultOrWidth * 2];
                maskSum = CONCAT11(CARRY1(*maskCursor,maskCursor[2]) + CARRY1(partialSumOrShift,maskCursor[restoreResultOrWidth * 2]) +
                                 CARRY1(partialSum,maskCursor[restoreResultOrWidth * 2 + 2]),
                                 partialSum + maskCursor[restoreResultOrWidth * 2 + 2]);
                partialSumOrShift = alphaShift & 0x1f;
                *destinationWord = maskSum << partialSumOrShift | maskSum >> 0x20 - partialSumOrShift | (ushort)rgbMaskBits;
                maskCursor = maskCursor + 4;
                nextRemaining = offsetOrCounter + -4;
                hasMore = 3 < offsetOrCounter;
                destinationWord = destinationWord + 1;
                offsetOrCounter = nextRemaining;
              } while (nextRemaining != 0 && hasMore);
              maskCursor = maskCursor + restoreResultOrWidth * 3;
              destinationWord = (ushort *)((int)destinationWordRow + destinationPitch);
              nextRemaining = rowsRemaining + -4;
              hasMore = 3 < rowsRemaining;
              offsetOrCounter = restoreResultOrWidth;
              destinationWordRow = destinationWord;
              rowsRemaining = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
            (*This->lpVtbl->Unlock)(This,surfaceBits);
            goto GraphicsTextureUploadAlpha4x_DecrementActiveCountAndReturn;
          }
          do {
            do {
              partialSumOrShift = *maskCursor + maskCursor[2];
              partialSum = partialSumOrShift + maskCursor[restoreResultOrWidth * 2];
              maskSum = CONCAT11(CARRY1(*maskCursor,maskCursor[2]) + CARRY1(partialSumOrShift,maskCursor[restoreResultOrWidth * 2]) +
                               CARRY1(partialSum,maskCursor[restoreResultOrWidth * 2 + 2]),partialSum + maskCursor[restoreResultOrWidth * 2 + 2]
                              );
              partialSumOrShift = alphaShift & 0x1f;
              *destinationDword = (uint)maskSum << partialSumOrShift | (uint)(maskSum >> 0x20 - partialSumOrShift) | rgbMaskBits;
              maskCursor = maskCursor + 4;
              nextRemaining = offsetOrCounter + -4;
              hasMore = 3 < offsetOrCounter;
              destinationDword = (uint *)((int)destinationDword + 2);
              offsetOrCounter = nextRemaining;
            } while (nextRemaining != 0 && hasMore);
            maskCursor = maskCursor + restoreResultOrWidth * 3;
            destinationDword = (uint *)((int)destinationDwordRow + destinationPitch);
            nextRemaining = rowsRemaining + -4;
            hasMore = 3 < rowsRemaining;
            offsetOrCounter = restoreResultOrWidth;
            destinationDwordRow = destinationDword;
            rowsRemaining = nextRemaining;
          } while (nextRemaining != 0 && hasMore);
        }
        (*This->lpVtbl->Unlock)(This,surfaceBits);
      }
    }
  }
GraphicsTextureUploadAlpha4x_DecrementActiveCountAndReturn:
  g_ActiveTextureUploads = g_ActiveTextureUploads - 1;
  return;
}


/* Address: 0x0057EBB0.
   Ownership: graphics/resources/texture.
   Purpose: Runs the color-upload handler selected by texture->downsampleShift and reloads the device texture from
   staging.
   Cross-module calls: Glide3_TextureSet_RefreshColor [graphics/backend/glide].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource *textureResource;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureSet_RefreshColor(subresourceIndex,set);
    return;
  }
  texture = set->entries[subresourceIndex].texture;
  (*g_GraphicsDispatchTable.colorUpload[texture->downsampleShift])(texture);
  if (texture->deviceTexture2 != (IDirect3DTexture2 *)0x0) {
    (*texture->deviceTexture2->lpVtbl->Load)(texture->deviceTexture2,texture->stagingTexture2);
    g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
  }
  return;
}


/* Address: 0x0057EC40.
   Ownership: graphics/resources/texture.
   Purpose: Runs the alpha-upload handler selected by texture->downsampleShift and reloads the device texture from
   staging.
   Cross-module calls: Glide3_TextureSet_RefreshAlpha [graphics/backend/glide].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource *textureResource;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureSet_RefreshAlpha(subresourceIndex,set);
    return;
  }
  texture = set->entries[subresourceIndex].texture;
  (*g_GraphicsDispatchTable.alphaUpload[texture->downsampleShift])(texture);
  if (texture->deviceTexture2 != (IDirect3DTexture2 *)0x0) {
    (*texture->deviceTexture2->lpVtbl->Load)(texture->deviceTexture2,texture->stagingTexture2);
    g_TextureDeviceReloadCount = g_TextureDeviceReloadCount + 1;
  }
  return;
}


/* Address: 0x0057AAD0.
   Ownership: graphics/resources/texture.
   Purpose: Creates the device-memory surface and IDirect3DTexture2 view, loads stagingTexture2, obtains a
   D3DTEXTUREHANDLE, and stores the device-side triplet. When CreateSurface reports 0x8876017C, the function evicts
   the least-recently-used device texture and retries while eviction succeeds. The routine preserves incoming EAX.
   Existing call sites mirror texture in EAX and use that preserved pointer.
   Local calls: GraphicsTexture_EvictOldestDeviceTexture.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_CreateDeviceTexture(GraphicsTextureResource *texture)

{
  D3DDEVICEDESC_DX6 *deviceDesc;
  uint largerExtent;
  TH_LEGACY_HRESULT hresult;
  sdword handleResult;
  GraphicsTextureDownsampleShift effectiveShift;
  int dwordsRemaining;
  DDPIXELFORMAT *sourceFormatCursor;
  DDPIXELFORMAT *destinationFormatCursor;
  bool evictFailed;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  dword textureHandle;
  IDirect3DTexture2 *deviceTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  
  deviceSurfaceBase = (IDirectDrawSurface *)0x0;
  deviceSurface3 = (IDirectDrawSurface3 *)0x0;
  deviceTexture2 = (IDirect3DTexture2 *)0x0;
  Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = 0x6c;
  deviceDesc = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 0) {
    g_SurfaceDesc.dwSize = 0x6c;
    return;
  }
  g_SurfaceDesc.dwFlags = 0x1007;
  g_SurfaceDesc.ddsCaps.dwCaps = 0x4001000;
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  if (deviceDesc->dcmColorModel == 0) {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | 0x800;
  }
  else {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | 0x4000;
  }
  largerExtent = logicalSize.logicalWidthPixels;
  if (logicalSize.logicalWidthPixels < logicalSize.logicalHeightPixels) {
    largerExtent = logicalSize.logicalHeightPixels;
  }
  g_SurfaceDesc.dwHeight = largerExtent >> ((byte)g_TextureDownsampleShift & 0x1f);
  effectiveShift = g_TextureDownsampleShift;
  do {
    if (0xf < (int)g_SurfaceDesc.dwHeight) break;
    g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
    effectiveShift = effectiveShift - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
  } while (effectiveShift != GRAPHICS_TEXTURE_DOWNSAMPLE_1X);
  texture->downsampleShift = effectiveShift;
  sourceFormatCursor = texture->pixelFormat;
  destinationFormatCursor = &g_SurfaceDesc.ddpfPixelFormat;
  g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
  for (dwordsRemaining = 8; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
    destinationFormatCursor->dwSize = sourceFormatCursor->dwSize;
    sourceFormatCursor = (DDPIXELFORMAT *)&sourceFormatCursor->dwFlags;
    destinationFormatCursor = (DDPIXELFORMAT *)&destinationFormatCursor->dwFlags;
  }
  do {
    hresult = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&deviceSurfaceBase,(TH_LEGACY_LPVOID)0x0);
    if (hresult == 0) {
      hresult = (*deviceSurfaceBase->lpVtbl->QueryInterface)
                        (deviceSurfaceBase,&IID_IDirectDrawSurface3_Local,&deviceSurface3);
      if ((hresult == 0) &&
         (hresult = (*deviceSurface3->lpVtbl->QueryInterface)
                            (deviceSurface3,&IID_IDirect3DTexture2_Local,&deviceTexture2),
         hresult == 0)) {
        hresult = (*deviceTexture2->lpVtbl->Load)(deviceTexture2,texture->stagingTexture2);
        if (hresult == 0) {
          handleResult = (*deviceTexture2->lpVtbl->GetHandle)
                            (deviceTexture2,g_Direct3DDevice2,&textureHandle);
          if (handleResult == 0) {
            texture->deviceSurfaceBase = deviceSurfaceBase;
            texture->deviceSurface3 = deviceSurface3;
            texture->deviceTexture2 = deviceTexture2;
            texture->textureHandle = textureHandle;
            return;
          }
GraphicsTexture_ReleasePartialDeviceResourcesAfterFailure:
          if (deviceTexture2 != (IDirect3DTexture2 *)0x0) {
            (*deviceTexture2->lpVtbl->Release)(deviceTexture2);
          }
          if (deviceSurface3 != (IDirectDrawSurface3 *)0x0) {
            (*deviceSurface3->lpVtbl->Release)(deviceSurface3);
          }
          if (deviceSurfaceBase != (IDirectDrawSurface *)0x0) {
            (*deviceSurfaceBase->lpVtbl->Release)(deviceSurfaceBase);
          }
          texture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
          texture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
          texture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
          texture->textureHandle = 0;
          return;
        }
      }
    }
    if (deviceTexture2 != (IDirect3DTexture2 *)0x0) {
      (*deviceTexture2->lpVtbl->Release)(deviceTexture2);
      deviceTexture2 = (IDirect3DTexture2 *)0x0;
    }
    if (deviceSurface3 != (IDirectDrawSurface3 *)0x0) {
      (*deviceSurface3->lpVtbl->Release)(deviceSurface3);
      deviceSurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (deviceSurfaceBase != (IDirectDrawSurface *)0x0) {
      (*deviceSurfaceBase->lpVtbl->Release)(deviceSurfaceBase);
      deviceSurfaceBase = (IDirectDrawSurface *)0x0;
    }
    if ((hresult != -0x7789fe84) || (evictFailed = GraphicsTexture_EvictOldestDeviceTexture(texture), evictFailed)
       ) goto GraphicsTexture_ReleasePartialDeviceResourcesAfterFailure;
  } while( true );
}


/* Address: 0x00485EA0.
   Ownership: graphics/resources/texture.
   Purpose: Converts the source asset's palette entries for the active framebuffer, allocates 8 +
   subresourceCount*0x20 bytes, and fills one GraphicsTextureSetEntry per source entry. Each pixelWidth and
   pixelHeight must be an exact power of two. widthLog2 and heightLog2 are generated with BSR. ABI: CF clear means
   success. CF set means palette conversion, allocation, or power-of-two validation failed.
*/
GraphicsTextureSetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset)

{
  dword widthLog2;
  int heightLog2;
  GraphicsPaletteTextureSourceAsset *convertedSource;
  GraphicsPaletteTextureSourceAsset *metadataOrError;
  int entryIndex;
  GraphicsPaletteTextureFormatVersion *entryFieldCursor;
  byte *sourceEntry;
  GraphicsPaletteTextureSourceEaxCf5 convertResult;
  ArenaAllocEaxCf5 metadataAllocation;
  GraphicsTextureSetEaxCf5 failureResult;
  GraphicsAssetAllocationByteSize entriesRemaining;
  
  convertResult = (*g_GraphicsTextureSourceConvertPaletteEntries)
                    ((GraphicsPaletteTextureSourceAsset *)sourceAsset);
  convertedSource = convertResult.paletteSource;
  metadataOrError = convertedSource;
  if (!convertResult.carry) {
    entriesRemaining = convertedSource->subresourceCount;
    metadataAllocation = (*g_MemoryApi.alloc)(entriesRemaining * 0x20 + 8);
    metadataOrError = (GraphicsPaletteTextureSourceAsset *)metadataAllocation.eax;
    if (!metadataAllocation.carry) {
      entryFieldCursor = &metadataOrError->formatVersion;
      metadataOrError->magic = (GraphicsPaletteTextureAssetMagic)convertedSource;
      metadataOrError->allocationSizeBytes = entriesRemaining;
      sourceEntry = convertedSource->reserved10_AF + (convertedSource->subresourceTableOffset - 0x10);
      entryIndex = 0;
      while( true ) {
        widthLog2 = 0x1f;
        if (*(uint *)(sourceEntry + 0x18) != 0) {
          for (; *(uint *)(sourceEntry + 0x18) >> widthLog2 == 0; widthLog2 = widthLog2 - 1) {
          }
        }
        *entryFieldCursor = 0;
        entryFieldCursor[5] = entryIndex;
        entryFieldCursor[1] = widthLog2;
        if (1 << ((byte)widthLog2 & 0x1f) != *(int *)(sourceEntry + 0x18)) break;
        entryFieldCursor[3] = (GraphicsPaletteTextureFormatVersion)convertedSource;
        heightLog2 = 0x1f;
        if (*(uint *)(sourceEntry + 0x1c) != 0) {
          for (; *(uint *)(sourceEntry + 0x1c) >> heightLog2 == 0; heightLog2 = heightLog2 + -1) {
          }
        }
        entryFieldCursor[4] = (GraphicsPaletteTextureFormatVersion)sourceEntry;
        entryFieldCursor[2] = heightLog2;
        if (1 << ((byte)heightLog2 & 0x1f) != *(int *)(sourceEntry + 0x1c)) break;
        entryFieldCursor = entryFieldCursor + 8;
        sourceEntry = sourceEntry + 0x20;
        entryIndex = entryIndex + 1;
        entriesRemaining = entriesRemaining - 1;
        if (entriesRemaining == 0) {
          return THANDOR_BITCAST(qword, GraphicsTextureSetEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, metadataAllocation) & 0xFFFFFFFFFFull) & 0xffffffff));
        }
      }
      metadataOrError = (GraphicsPaletteTextureSourceAsset *)&k_LowAddressLiteral0000002F;
    }
  }
  failureResult.carry = true;
  failureResult.textureSet = (GraphicsTextureSet *)metadataOrError;
  return failureResult;
}


/* Address: 0x00485F90.
   Ownership: graphics/resources/texture.
   Purpose: Frees the texture-set metadata allocation and returns set->sourceAsset. Null input returns null.
*/
GraphicsTextureSourceAsset * __thandor_eax_preserve_ecx_edx
GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *releasedTextureSet;
  
  releasedTextureSet = (GraphicsTextureSourceAsset *)0x0;
  if (set != (GraphicsTextureSet *)0x0) {
    releasedTextureSet = set->sourceAsset;
    (*g_MemoryApi.free)(set);
  }
  return releasedTextureSet;
}


/* Address: 0x0057A9D0.
   Ownership: graphics/resources/texture.
   Purpose: Only the selected device-side COM triplet is released; staging objects remain available for reloading.
   If the evicted handle was bound, D3DRENDERSTATE_TEXTUREHANDLE is set to zero. ABI: CF clear means an object was
   evicted. CF set means no eligible object existed.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsTexture_EvictOldestDeviceTexture(GraphicsTextureResource *exclude)

{
  GraphicsTextureResource *candidate;
  dword evictedHandle;
  int slotsRemaining;
  GraphicsResourceUsageSerial oldestUsage;
  GraphicsTextureResource *oldestTexture;
  GraphicsTextureResource **slotCursor;
  IDirect3DTexture2 *deviceTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  
  slotsRemaining = 0x1000;
  oldestUsage = 0xffffffff;
  oldestTexture = (GraphicsTextureResource *)0x0;
  slotCursor = g_GraphicsTextureSlots;
  do {
    candidate = *slotCursor;
    if ((((candidate != (GraphicsTextureResource *)0x0) && (candidate != exclude)) &&
        (candidate->textureHandle != 0)) && (candidate->lastUsedCounter <= oldestUsage)) {
      oldestUsage = candidate->lastUsedCounter;
      oldestTexture = candidate;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  if (oldestTexture == (GraphicsTextureResource *)0x0) {
    return true;
  }
  deviceTexture2 = oldestTexture->deviceTexture2;
  if (deviceTexture2 != (IDirect3DTexture2 *)0x0) {
    (*deviceTexture2->lpVtbl->Release)(deviceTexture2);
  }
  deviceSurface3 = oldestTexture->deviceSurface3;
  if (deviceSurface3 != (IDirectDrawSurface3 *)0x0) {
    (*deviceSurface3->lpVtbl->Release)(deviceSurface3);
  }
  deviceSurfaceBase = oldestTexture->deviceSurfaceBase;
  if (deviceSurfaceBase != (IDirectDrawSurface *)0x0) {
    (*deviceSurfaceBase->lpVtbl->Release)(deviceSurfaceBase);
  }
  evictedHandle = oldestTexture->textureHandle;
  oldestTexture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
  oldestTexture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
  oldestTexture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
  oldestTexture->textureHandle = 0;
  if (evictedHandle == g_BoundTextureHandle) {
    g_BoundTextureHandle = 0;
    (*g_Direct3DDevice2->lpVtbl->SetRenderState)(g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
  }
  return false;
}


/* Address: 0x0057E870.
   Ownership: graphics/resources/texture.
   Purpose: Stores the texture pointer in the first free entry of the 4096-entry texture-slot array. ABI: CF clear
   means success. CF set means failure; EAX may contain an engine error code.
*/
bool __thandor_void_preserve_eax_ecx GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture)

{
  int slotsRemaining;
  GraphicsTextureResource **slotCursor;
  
  slotsRemaining = 0x1000;
  slotCursor = g_GraphicsTextureSlots;
  do {
    if (*slotCursor == (GraphicsTextureResource *)0x0) {
      *slotCursor = texture;
      return false;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return true;
}


/* Address: 0x0057E8C0.
   Ownership: graphics/resources/texture.
   Purpose: Selects an opaque or alpha-capable DirectDraw pixel format by scanning either direct pixels or a
   256-entry palette bank.
*/
DDPIXELFORMAT *
GraphicsTexture_SelectPixelFormat
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  int entryOffset;
  DDPIXELFORMAT *selectedFormat;
  GraphicsTextureSourceAsset *paletteEntryCursor;
  byte *pixelCursor;
  int paletteIndexOrCount;
  
  entryOffset = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
  paletteIndexOrCount = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + entryOffset + -0x20)
  ;
  if (paletteIndexOrCount < 0) {
    pixelCursor = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
             *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     entryOffset + -0x1c) + -0x28;
    paletteIndexOrCount = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    entryOffset + -0x10) *
            *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + entryOffset + -0xc
                    );
    do {
      if (*(uint *)pixelCursor < 0xff000000) {
        return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
      }
      pixelCursor = pixelCursor + 4;
      paletteIndexOrCount = paletteIndexOrCount + -1;
    } while (paletteIndexOrCount != 0);
    selectedFormat = (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0);
  }
  else {
    paletteEntryCursor = sourceAsset + paletteIndexOrCount * 4 + 1;
    paletteIndexOrCount = 0x100;
    do {
      if ((paletteEntryCursor->common).magic < 0xff000000) {
        return (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedAlphaTextureFormat,0);
      }
      paletteEntryCursor = (GraphicsTextureSourceAsset *)&(paletteEntryCursor->common).formatVersion;
      paletteIndexOrCount = paletteIndexOrCount + -1;
    } while (paletteIndexOrCount != 0);
    selectedFormat = (DDPIXELFORMAT *)THANDOR_ADDR(g_Direct3DSelectedOpaqueTextureFormat,0);
  }
  return selectedFormat;
}

/* Address: 0x0057A740.
   Ownership: graphics/resources/texture.
   Purpose: Creates the system-memory staging surface, obtains IDirectDrawSurface3 and IDirect3DTexture2 views,
   clears the device-side triplet and texture handle, and invokes the color-upload converter selected by
   downsampleShift. The routine preserves incoming EAX. Callers that mirror texture in EAX use the preserved
   register as a pointer result; the stack argument remains the actual formal parameter.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
GraphicsTextureResource * __thandor_eax_preserve_ecx_edx
GraphicsTexture_CreateStagingTexture(GraphicsTextureResource *texture)

{
  uint largerExtent;
  TH_LEGACY_HRESULT hresult;
  GraphicsTextureDownsampleShift effectiveShift;
  int dwordsRemaining;
  DDPIXELFORMAT *sourceFormatCursor;
  DDPIXELFORMAT *destinationFormatCursor;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  IDirect3DTexture2 *texture2;
  IDirectDrawSurface3 *surface3;
  IDirectDrawSurface *surfaceBase;
  
  surfaceBase = (IDirectDrawSurface *)0x0;
  surface3 = (IDirectDrawSurface3 *)0x0;
  texture2 = (IDirect3DTexture2 *)0x0;
  Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = 0x6c;
  if (1 < g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1) {
    g_SurfaceDesc.dwFlags = 0x1007;
    g_SurfaceDesc.ddsCaps.dwCaps = 0x1800;
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset)
    ;
    largerExtent = logicalSize.logicalWidthPixels;
    if (logicalSize.logicalWidthPixels < logicalSize.logicalHeightPixels) {
      largerExtent = logicalSize.logicalHeightPixels;
    }
    g_SurfaceDesc.dwHeight = largerExtent >> ((byte)g_TextureDownsampleShift & 0x1f);
    effectiveShift = g_TextureDownsampleShift;
    do {
      if (0xf < (int)g_SurfaceDesc.dwHeight) break;
      g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
      effectiveShift = effectiveShift - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
    } while (effectiveShift != GRAPHICS_TEXTURE_DOWNSAMPLE_1X);
    texture->downsampleShift = effectiveShift;
    sourceFormatCursor = texture->pixelFormat;
    destinationFormatCursor = &g_SurfaceDesc.ddpfPixelFormat;
    g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
    for (dwordsRemaining = 8; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
      destinationFormatCursor->dwSize = sourceFormatCursor->dwSize;
      sourceFormatCursor = (DDPIXELFORMAT *)&sourceFormatCursor->dwFlags;
      destinationFormatCursor = (DDPIXELFORMAT *)&destinationFormatCursor->dwFlags;
    }
    hresult = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&surfaceBase,(TH_LEGACY_LPVOID)0x0);
    if (hresult == 0) {
      hresult = (*surfaceBase->lpVtbl->QueryInterface)
                        (surfaceBase,&IID_IDirectDrawSurface3_Local,&surface3);
      if (hresult == 0) {
        hresult = (*surface3->lpVtbl->QueryInterface)(surface3,&IID_IDirect3DTexture2_Local,&texture2)
        ;
        if (hresult == 0) goto GraphicsTexture_CommitStagingResourcesAndUpload;
      }
    }
    if (texture2 != (IDirect3DTexture2 *)0x0) {
      (*texture2->lpVtbl->Release)(texture2);
    }
    if (surface3 != (IDirectDrawSurface3 *)0x0) {
      (*surface3->lpVtbl->Release)(surface3);
    }
    if (surfaceBase != (IDirectDrawSurface *)0x0) {
      (*surfaceBase->lpVtbl->Release)(surfaceBase);
    }
    texture->stagingSurfaceBase = (IDirectDrawSurface *)0x0;
    texture->stagingSurface3 = (IDirectDrawSurface3 *)0x0;
    texture->stagingTexture2 = (IDirect3DTexture2 *)0x0;
    return texture; /* preserved EAX: callers mirror texture in EAX */
  }
GraphicsTexture_CommitStagingResourcesAndUpload:
  texture->stagingSurfaceBase = surfaceBase;
  texture->stagingSurface3 = surface3;
  texture->stagingTexture2 = texture2;
  texture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
  texture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
  texture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
  texture->textureHandle = 0;
  (*g_GraphicsDispatchTable.colorUpload[texture->downsampleShift])(texture);
  return texture; /* preserved EAX: callers mirror texture in EAX */
}


/* Address: 0x0057A900.
   Ownership: graphics/resources/texture.
   Purpose: Releases the device and staging COM triplets, clears all six pointers and textureHandle, and
   invalidates g_BoundTextureHandle with 0xFFFFFFFF when it referred to the released handle. The routine preserves
   incoming EAX. Several callers mirror texture in EAX and chain the preserved value into another helper.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsTexture_ReleaseObjects(GraphicsTextureResource *texture)

{
  IDirect3DTexture2 *currentTexture2;
  IDirect3DTexture2 *stagingTexture2;
  IDirectDrawSurface3 *currentSurface3;
  IDirectDrawSurface3 *stagingSurface3;
  IDirectDrawSurface *currentBaseSurface;
  IDirectDrawSurface *stagingSurfaceBase;
  dword releasedTextureHandle;
  
  currentTexture2 = texture->deviceTexture2;
  if (currentTexture2 != (IDirect3DTexture2 *)0x0) {
    (*currentTexture2->lpVtbl->Release)(currentTexture2);
  }
  currentSurface3 = texture->deviceSurface3;
  if (currentSurface3 != (IDirectDrawSurface3 *)0x0) {
    (*currentSurface3->lpVtbl->Release)(currentSurface3);
  }
  currentBaseSurface = texture->deviceSurfaceBase;
  if (currentBaseSurface != (IDirectDrawSurface *)0x0) {
    (*currentBaseSurface->lpVtbl->Release)(currentBaseSurface);
  }
  stagingTexture2 = texture->stagingTexture2;
  if (stagingTexture2 != (IDirect3DTexture2 *)0x0) {
    (*stagingTexture2->lpVtbl->Release)(stagingTexture2);
  }
  stagingSurface3 = texture->stagingSurface3;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    (*stagingSurface3->lpVtbl->Release)(stagingSurface3);
  }
  stagingSurfaceBase = texture->stagingSurfaceBase;
  if (stagingSurfaceBase != (IDirectDrawSurface *)0x0) {
    (*stagingSurfaceBase->lpVtbl->Release)(stagingSurfaceBase);
  }
  releasedTextureHandle = texture->textureHandle;
  texture->stagingSurfaceBase = (IDirectDrawSurface *)0x0;
  texture->stagingSurface3 = (IDirectDrawSurface3 *)0x0;
  texture->stagingTexture2 = (IDirect3DTexture2 *)0x0;
  texture->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
  texture->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
  texture->deviceTexture2 = (IDirect3DTexture2 *)0x0;
  texture->textureHandle = 0;
  if (releasedTextureHandle == g_BoundTextureHandle) {
    g_BoundTextureHandle = 0xffffffff;
  }
  return;
}

