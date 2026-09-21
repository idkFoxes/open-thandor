/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/texture.h>

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
  GraphicsTextureSourceAsset *sourceAsset_00;
  dword dVar1;
  int iVar2;
  GraphicsAdapterRecord *pGVar3;
  DDPIXELFORMAT *pDVar4;
  GraphicsTextureResource *pGVar5;
  bool bVar6;
  GraphicsTextureSetEaxCf5 GVar7;
  ArenaAllocEaxCf5 AVar8;
  GraphicsTextureSetEaxCf5 GVar9;
  GraphicsTextureSetEaxCf5 GVar10;
  GraphicsTextureSetEntry *pGStack_24;
  AssetSubresourceCount AStack_20;
  GraphicsSubresourceIndex GStack_1c;
  
  pGVar3 = g_GraphicsAdapters;
  iVar2 = g_ActiveGraphicsAdapterIndex;
  GVar7 = GraphicsTextureSet_AllocateMetadata(sourceAsset);
  if (GVar7.carry) {
    GVar10.carry = true;
    GVar10.textureSet = GVar7.textureSet;
    return GVar10;
  }
  if (pGVar3[iVar2].deviceGuid.Data1 == 1) {
    GVar7.carry = Glide3_TextureSet_CreateBackend(GVar7.textureSet,sourceAsset);
    return GVar7;
  }
  sourceAsset_00 = (GVar7.textureSet)->sourceAsset;
  AStack_20 = (sourceAsset_00->tableDescriptor).subresourceCount;
  pGStack_24 = (GVar7.textureSet)->entries;
  GStack_1c = 0;
  do {
    pDVar4 = GraphicsTexture_SelectPixelFormat(GStack_1c,sourceAsset_00);
    AVar8 = (*g_MemoryApi.alloc)(0x50);
    pGVar5 = (GraphicsTextureResource *)AVar8.eax;
    if (!AVar8.carry) {
      pGVar5->stagingTexture2 = (IDirect3DTexture2 *)0x0;
      pGVar5->stagingSurface3 = (IDirectDrawSurface3 *)0x0;
      pGVar5->stagingSurfaceBase = (IDirectDrawSurface *)0x0;
      pGStack_24->texture = pGVar5;
      pGVar5->deviceTexture2 = (IDirect3DTexture2 *)0x0;
      pGVar5->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
      pGVar5->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
      pGVar5->sourceAsset = sourceAsset_00;
      pGVar5->subresourceIndex = GStack_1c;
      pGVar5->pixelFormat = pDVar4;
      dVar1 = g_TextureDownsampleShift;
      pGVar5->lastUsedCounter = 0;
      pGVar5->textureHandle = 0;
      pGVar5->downsampleShift = dVar1;
      pGVar5 = GraphicsTexture_CreateStagingTexture(pGVar5);
      bVar6 = GraphicsTexture_RegisterSlot(pGVar5);
      if (bVar6) {
        GraphicsTexture_ReleaseObjects(pGVar5);
        (*g_MemoryApi.free)(pGVar5);
        pGStack_24->texture = (GraphicsTextureResource *)0x0;
      }
      else {
        GraphicsTexture_CreateDeviceTexture(pGVar5);
      }
    }
    GStack_1c = GStack_1c + 1;
    pGStack_24 = pGStack_24 + 1;
    AStack_20 = AStack_20 - 1;
  } while (AStack_20 != 0);
  GVar9.carry = false;
  GVar9.textureSet = GVar7.textureSet;
  return GVar9;
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
  GraphicsTextureResource **ppGVar1;
  GraphicsTextureSourceAsset *pGVar2;
  int iVar3;
  dword dVar4;
  GraphicsTextureSetEntry *pGVar5;
  GraphicsTextureResource **ppGVar6;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    pGVar2 = Glide3_TextureSet_DestroyBackend(set,set);
    return pGVar2;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (set != (GraphicsTextureSet *)0x0) {
    dVar4 = set->subresourceCount;
    pGVar5 = set->entries;
    do {
      texture = pGVar5->texture;
      if (texture != (GraphicsTextureResource *)0x0) {
        iVar3 = 0x1000;
        ppGVar1 = g_GraphicsTextureSlots;
        do {
          ppGVar6 = ppGVar1;
          if (texture == *ppGVar6) break;
          iVar3 = iVar3 + -1;
          ppGVar1 = ppGVar6 + 1;
        } while (iVar3 != 0);
        *ppGVar6 = (GraphicsTextureResource *)0x0;
        GraphicsTexture_ReleaseObjects(texture);
        (*g_MemoryApi.free)(texture);
      }
      pGVar5 = pGVar5 + 1;
      dVar4 = dVar4 - 1;
    } while (dVar4 != 0);
    pGVar2 = GraphicsTextureSet_FreeMetadata(set);
  }
  return pGVar2;
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
  PackageLoadEntryEaxCf5 PVar1;
  GraphicsTextureSetEaxCf5 GVar2;
  
  PVar1 = Package_LoadEntry(pathUtf16);
  loadedTextureSource = PVar1.bufferOrError;
  if (!PVar1.carry) {
    GVar2 = (*g_GraphicsCreateTextureSet)(loadedTextureSource);
    createdTextureSet = GVar2.textureSet;
    if (!GVar2.carry) {
      return GVar2;
    }
    Resource_Release(loadedTextureSource);
    loadedTextureSource = (GraphicsTextureSourceAsset *)createdTextureSet;
  }
  GVar2.carry = true;
  GVar2.textureSet = (GraphicsTextureSet *)loadedTextureSource;
  return GVar2;
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
  undefined4 in_EAX;
  undefined4 in_EDX;
  GraphicsTextureSizeEaxEdxCf9 GVar1;
  GraphicsTextureSizeEaxEdxCf9 GVar2;
  AssetRelativeOffset subresourceTableOffset;
  
  if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
     (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
    subresourceTableOffset = (sourceAsset->tableDescriptor).subresourceTableOffset;
    GVar1.logicalHeightPixels =
         *(undefined4 *)
          ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
          subresourceIndex * 0x20 + subresourceTableOffset + -0x24);
    GVar1.logicalWidthPixels =
         *(undefined4 *)
          ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
          subresourceIndex * 0x20 + subresourceTableOffset + -0x28);
    GVar1.carry = false;
    return GVar1;
  }
  GVar2.logicalHeightPixels = in_EDX;
  GVar2.logicalWidthPixels = in_EAX;
  GVar2.carry = true;
  return GVar2;
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
  byte *pbVar1;
  AssetRelativeOffset AVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if ((drawX <= queryX) && (drawY <= queryY)) {
    if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
       (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
      AVar2 = (sourceAsset->tableDescriptor).subresourceTableOffset;
      iVar5 = subresourceIndex * 0x20;
      pbVar1 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
               iVar5 + AVar2 + -0x18;
      iVar4 = (queryX - drawX) - *(int *)pbVar1;
      if ((((*(int *)pbVar1 <= queryX - drawX) &&
           (pbVar1 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     iVar5 + AVar2 + -0x14, iVar6 = (queryY - drawY) - *(int *)pbVar1,
           *(int *)pbVar1 <= queryY - drawY)) &&
          (iVar4 < *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                           iVar5 + AVar2 + -0x10))) &&
         (iVar6 < *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar5 + AVar2 + -0xc))) {
        iVar3 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar5 + AVar2 + -0x20);
        iVar4 = iVar6 * *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar5 + AVar2 + -0x10) + iVar4;
        if (iVar3 == -1) {
          if (0xffffff <
              *(uint *)(iVar4 * 4 +
                        *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                iVar5 + AVar2 + -0x1c) + (int)sourceAsset)) {
            return true;
          }
        }
        else if (0xffffff <
                 *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                           assetRelativeAddressAnchor28 +
                          (uint)*(byte *)(iVar4 + *(int *)((sourceAsset->common).buildMetadata.
                                                           assetRelativeAddressAnchor28 +
                                                          iVar5 + AVar2 + -0x1c) + (int)sourceAsset)
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
  dword dVar1;
  int arg5;
  dword dVar2;
  int arg4;
  bool bVar3;
  GraphicsTextureSizeEaxEdxCf9 GVar4;
  
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  dVar2 = GVar4.logicalHeightPixels;
  dVar1 = GVar4.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + dVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + dVar2;
  }
  do {
    do {
      bVar3 = SCARRY4(tileOriginX,dVar1);
      tileOriginX = tileOriginX + dVar1;
    } while (tileOriginX == 0 || bVar3 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      bVar3 = SCARRY4(tileOriginY,dVar2);
      tileOriginY = tileOriginY + dVar2;
    } while (tileOriginY == 0 || bVar3 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - dVar2;
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
  if ((int)(tileOriginX - dVar1) < repeatEndX) {
    for (; arg5 = tileOriginX - dVar1, arg4 < repeatEndY; arg4 = arg4 + dVar2) {
      do {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,sourceAsset,
                   framebuffer);
        arg5 = arg5 + dVar1;
      } while (arg5 < repeatEndX);
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
  dword dVar1;
  int arg5;
  dword dVar2;
  int arg4;
  bool bVar3;
  GraphicsTextureSizeEaxEdxCf9 GVar4;
  
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  dVar2 = GVar4.logicalHeightPixels;
  dVar1 = GVar4.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + dVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + dVar2;
  }
  do {
    do {
      bVar3 = SCARRY4(tileOriginX,dVar1);
      tileOriginX = tileOriginX + dVar1;
    } while (tileOriginX == 0 || bVar3 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      bVar3 = SCARRY4(tileOriginY,dVar2);
      tileOriginY = tileOriginY + dVar2;
    } while (tileOriginY == 0 || bVar3 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - dVar2;
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
  if ((int)(tileOriginX - dVar1) < repeatEndX) {
    for (; arg5 = tileOriginX - dVar1, arg4 < repeatEndY; arg4 = arg4 + dVar2) {
      do {
        (*g_GraphicsTextureSourceBlitHalfSourceRgb)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,sourceAsset,
                   framebuffer);
        arg5 = arg5 + dVar1;
      } while (arg5 < repeatEndX);
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
  dword dVar1;
  int arg5;
  dword dVar2;
  int arg4;
  bool bVar3;
  GraphicsTextureSizeEaxEdxCf9 GVar4;
  
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  dVar2 = GVar4.logicalHeightPixels;
  dVar1 = GVar4.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + dVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + dVar2;
  }
  do {
    do {
      bVar3 = SCARRY4(tileOriginX,dVar1);
      tileOriginX = tileOriginX + dVar1;
    } while (tileOriginX == 0 || bVar3 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      bVar3 = SCARRY4(tileOriginY,dVar2);
      tileOriginY = tileOriginY + dVar2;
    } while (tileOriginY == 0 || bVar3 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - dVar2;
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
  if ((int)(tileOriginX - dVar1) < repeatEndX) {
    for (; arg5 = tileOriginX - dVar1, arg4 < repeatEndY; arg4 = arg4 + dVar2) {
      do {
        (*g_GraphicsTextureSourceBlitSaturatedAddRgb)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,sourceAsset,
                   framebuffer);
        arg5 = arg5 + dVar1;
      } while (arg5 < repeatEndX);
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
  dword dVar1;
  int arg5;
  dword dVar2;
  int arg4;
  bool bVar3;
  GraphicsTextureSizeEaxEdxCf9 GVar4;
  
  GVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  dVar2 = GVar4.logicalHeightPixels;
  dVar1 = GVar4.logicalWidthPixels;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + dVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + dVar2;
  }
  do {
    do {
      bVar3 = SCARRY4(tileOriginX,dVar1);
      tileOriginX = tileOriginX + dVar1;
    } while (tileOriginX == 0 || bVar3 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  do {
    do {
      bVar3 = SCARRY4(tileOriginY,dVar2);
      tileOriginY = tileOriginY + dVar2;
    } while (tileOriginY == 0 || bVar3 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - dVar2;
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
  if ((int)(tileOriginX - dVar1) < repeatEndX) {
    for (; arg5 = tileOriginX - dVar1, arg4 < repeatEndY; arg4 = arg4 + dVar2) {
      do {
        (*g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd)
                  (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,sourceAsset,
                   framebuffer);
        arg5 = arg5 + dVar1;
      } while (arg5 < repeatEndX);
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
  word *pwVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  GraphicsTextureSourceAsset *pGVar7;
  dword dVar8;
  dword dVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  AssetSubresourceCount AVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  uint uVar19;
  uint uVar20;
  int iVar21;
  int iVar22;
  uint uVar23;
  byte *pbVar24;
  PckConverterVersion PVar25;
  uint *puVar26;
  uint *puVar27;
  GraphicsTextureSourceAsset *pGVar28;
  byte *pbVar29;
  byte *pbVar30;
  byte *pbVar31;
  byte *pbVar32;
  uint *puVar33;
  uint *puVar34;
  uint *puVar35;
  bool bVar37;
  bool bVar38;
  ArenaShrinkEaxCf5 AVar39;
  GraphicsTextureSourceAssetEaxCf5 GVar40;
  GraphicsTextureSourceAssetEaxCf5 GVar41;
  GraphicsTextureSourceAssetEaxCf5 GVar42;
  ArenaLargestAllocationEaxEcxCf9 AVar43;
  uint *puStackY_4c;
  uint uStack_38;
  uint *puStack_34;
  int iStack_2c;
  int iStack_24;
  uint *puVar36;
  
  GVar40.assetOrError = (GraphicsTextureSourceAsset *)0x2c;
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
     ((sourceAsset->tableDescriptor).subresourceCount <= entryIndex)) goto LAB_004ad0f7;
  iVar12 = entryIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
  iVar21 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                   iVar12 + -0x10);
  iStack_24 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                      iVar12 + -0xc);
  AVar43 = (*g_MemoryApi.allocLargestFreeBlock)();
  dVar9 = AVar43.blockSizeOrSentinel;
  GVar40.assetOrError = (GraphicsTextureSourceAsset *)AVar43.allocationOrError;
  if (AVar43.carry) goto LAB_004ad0f7;
  pGVar7 = sourceAsset;
  pGVar28 = GVar40.assetOrError;
  for (iVar10 = 0x80; iVar10 != 0; iVar10 = iVar10 + -1) {
    (pGVar28->common).magic = (pGVar7->common).magic;
    pGVar7 = (GraphicsTextureSourceAsset *)&(pGVar7->common).allocationSizeBytes;
    pGVar28 = (GraphicsTextureSourceAsset *)&(pGVar28->common).allocationSizeBytes;
  }
  iVar11 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                   iVar12 + -0x20);
  pGVar7 = (GraphicsTextureSourceAsset *)0x14;
  iVar10 = dVar9 - 0x200;
  if (iVar10 != 0 && 0x1ff < (int)dVar9) {
    iVar12 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     iVar12 + -0x1c);
    dVar8 = (*g_LocaleGetPackedCurrentTime)();
    ((GVar40.assetOrError)->common).buildMetadata.timestamps.dateValue1 = dVar8;
    ((GVar40.assetOrError)->common).buildMetadata.timestamps.dateValue2 = dVar8;
    dVar8 = (*g_LocaleGetPackedCurrentDate)();
    ((GVar40.assetOrError)->common).buildMetadata.timestamps.timeValue1 = dVar8;
    ((GVar40.assetOrError)->common).buildMetadata.timestamps.timeValue2 = dVar8;
    (*g_LocaleCopyDefaultComputerLabelUtf16)
              (((GVar40.assetOrError)->common).buildMetadata.names.sourceName);
    pbVar24 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar12 + -0x28;
    if (iVar11 == -1) {
      ((GVar40.assetOrError)->tableDescriptor).paletteBankCount = 0;
      ((GVar40.assetOrError)->tableDescriptor).subresourceCount = 0;
      ((GVar40.assetOrError)->tableDescriptor).subresourceTableOffset = 0x200;
      uVar13 = *(uint *)pbVar24;
      uVar17 = iVar21 * iStack_24;
      bVar37 = true;
      uVar18 = uVar17;
      pbVar29 = pbVar24;
      do {
        pbVar30 = pbVar29;
        if (uVar18 == 0) break;
        uVar18 = uVar18 - 1;
        pbVar30 = pbVar29 + 4;
        bVar37 = uVar13 == *(uint *)pbVar29;
        pbVar29 = pbVar30;
      } while (bVar37);
      uVar18 = *(uint *)(pbVar30 + -4);
      pGVar7 = (GraphicsTextureSourceAsset *)0x2d;
      if (!bVar37) {
        pGVar7 = (GraphicsTextureSourceAsset *)0x14;
        iStack_2c = iVar10 + uVar17 * -4;
        if (iStack_2c != 0 && (int)(uVar17 * 4) <= iVar10) {
          puVar26 = (uint *)((int)GVar40.assetOrError + iStack_2c + 0x200);
          bVar37 = puVar26 == (uint *)0x0;
          puVar27 = puVar26;
          for (uVar17 = uVar17 & 0x3fffffff; uVar17 != 0; uVar17 = uVar17 - 1) {
            *puVar27 = *(uint *)pbVar24;
            pbVar24 = pbVar24 + 4;
            puVar27 = puVar27 + 1;
          }
          uStack_38 = 0;
          iVar12 = iVar21;
          puStack_34 = puVar26;
code_r0x004ace04:
          do {
            if (iVar12 != 0) {
              iVar12 = iVar12 + -1;
              puVar27 = puVar26 + 1;
              bVar37 = uVar13 == *puVar26;
              puVar26 = puVar27;
              if (bVar37) goto code_r0x004ace04;
            }
            if (!bVar37) {
              pGVar7 = (GraphicsTextureSourceAsset *)0x14;
              iVar10 = iStack_2c + -0x20;
              if (iVar10 == 0 || iStack_2c < 0x20) goto LAB_004ad0ec;
              AVar14 = ((GVar40.assetOrError)->tableDescriptor).subresourceCount;
              puVar26 = puVar26 + -1;
              iVar12 = iVar12 + 1;
              ((GVar40.assetOrError)->tableDescriptor).subresourceCount =
                   ((GVar40.assetOrError)->tableDescriptor).subresourceCount + 1;
              iVar22 = AVar14 * 0x20;
              bVar37 = iVar22 == 0;
              iVar11 = iVar12;
              puVar27 = puVar26;
              do {
                puVar33 = puVar27;
                if (iVar11 == 0) break;
                iVar11 = iVar11 + -1;
                puVar33 = puVar27 + 1;
                bVar37 = uVar13 == *puVar27;
                puVar27 = puVar33;
              } while (!bVar37);
              if (bVar37) {
                puVar33 = puVar33 + -1;
              }
              pbVar24 = GVar40.assetOrError[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                        iVar22 + -0x28;
              *(uint *)pbVar24 = (uint)((int)puVar33 - (int)puVar26) >> 2;
              pbVar24[4] = 0;
              pbVar24[5] = 0;
              pbVar24[6] = 0;
              pbVar24[7] = 0;
              *(word *)((int)(pbVar24 + 0x10) + 0) = 0;
              *(word *)((int)(pbVar24 + 0x10) + 2) = 0;
              *(word *)((int)(pbVar24 + 0x14) + 0) = 0;
              *(word *)((int)(pbVar24 + 0x14) + 2) = 0;
              ((AssetProducerSourceNames *)(pbVar24 + 8))->producerName[0] = 0xffff;
              ((AssetProducerSourceNames *)(pbVar24 + 8))->producerName[1] = 0xffff;
              iVar11 = iStack_24;
              puVar27 = puVar26;
              do {
                *(uint *)(pbVar24 + 4) = *(uint *)(pbVar24 + 4) + 1;
                puVar27 = puVar27 + iVar21;
                iVar11 = iVar11 + -1;
                bVar37 = true;
                if (iVar11 == 0) break;
                bVar37 = uVar13 == *puVar27;
              } while (!bVar37);
              *(uint *)(pbVar24 + 0x18) = *(uint *)pbVar24;
              *(uint *)(pbVar24 + 0x1c) = *(uint *)(pbVar24 + 4);
              puVar27 = puVar26;
              do {
                uVar17 = *(uint *)pbVar24;
                puVar33 = puVar27;
                do {
                  if (uVar17 == 0) break;
                  uVar17 = uVar17 - 1;
                  bVar37 = uVar18 == *puVar33;
                  puVar33 = puVar33 + 1;
                } while (bVar37);
                if ((!bVar37) || (0xffffff < uVar18)) goto LAB_004acf82;
                puVar27 = puVar27 + iVar21;
                *(uint *)(pbVar24 + 0x14) = *(uint *)(pbVar24 + 0x14) + 1;
                pwVar1 = (word *)(pbVar24 + 0x1c);
                *(uint *)pwVar1 = *(uint *)pwVar1 - 1;
                bVar37 = *(uint *)pwVar1 == 0;
              } while (!bVar37);
              puVar27 = puVar27 + -iVar21;
              *(uint *)(pbVar24 + 0x14) = *(uint *)(pbVar24 + 0x14) - 1;
              *(uint *)(pbVar24 + 0x1c) = *(uint *)(pbVar24 + 0x1c) + 1;
LAB_004acf82:
              puVar33 = (uint *)((int)puVar27 + iVar21 * 4 * *(uint *)(pbVar24 + 0x1c));
              do {
                puVar33 = puVar33 + -iVar21;
                bVar37 = puVar33 == (uint *)0x0;
                uVar17 = *(uint *)pbVar24;
                puVar34 = puVar33;
                do {
                  if (uVar17 == 0) break;
                  uVar17 = uVar17 - 1;
                  bVar37 = uVar18 == *puVar34;
                  puVar34 = puVar34 + 1;
                } while (bVar37);
                if ((!bVar37) || (0xffffff < uVar18)) goto LAB_004acfc4;
                pwVar1 = (word *)(pbVar24 + 0x1c);
                *(uint *)pwVar1 = *(uint *)pwVar1 - 1;
              } while (*(uint *)pwVar1 != 0);
              *(uint *)(pbVar24 + 0x1c) = *(uint *)(pbVar24 + 0x1c) + 1;
LAB_004acfc4:
              uVar17 = *(uint *)(pbVar24 + 0x1c);
              puStackY_4c = puVar27;
              puVar33 = puVar27;
              if ((uVar18 & 0xff000000) == 0) {
                do {
                  do {
                    puStackY_4c = puVar33;
                    if (uVar18 != *puVar27) goto LAB_004ad012;
                    puVar27 = puVar27 + iVar21;
                    uVar17 = uVar17 - 1;
                    puVar33 = puStackY_4c;
                  } while (uVar17 != 0);
                  *(uint *)(pbVar24 + 0x10) = *(uint *)(pbVar24 + 0x10) + 1;
                  puVar27 = puStackY_4c + 1;
                  uVar17 = *(uint *)(pbVar24 + 0x1c);
                  pwVar1 = (word *)(pbVar24 + 0x18);
                  *(uint *)pwVar1 = *(uint *)pwVar1 - 1;
                  puVar33 = puVar27;
                } while (*(uint *)pwVar1 != 0);
                *(uint *)(pbVar24 + 0x10) = *(uint *)(pbVar24 + 0x10) - 1;
                *(uint *)(pbVar24 + 0x18) = *(uint *)(pbVar24 + 0x18) + 1;
              }
LAB_004ad012:
              puVar27 = puStackY_4c;
              uVar17 = *(uint *)(pbVar24 + 0x1c);
              puVar33 = (uint *)((int)puStackY_4c +
                                *(uint *)(pbVar24 + 0x18) +
                                *(uint *)(pbVar24 + 0x18) +
                                *(uint *)(pbVar24 + 0x18) + *(uint *)(pbVar24 + 0x18) + -4);
              puStackY_4c = puVar33;
              if ((uVar18 & 0xff000000) == 0) {
                do {
                  do {
                    if (uVar18 != *puVar33) goto LAB_004ad066;
                    puVar33 = puVar33 + iVar21;
                    uVar17 = uVar17 - 1;
                  } while (uVar17 != 0);
                  uVar17 = *(uint *)(pbVar24 + 0x1c);
                  puVar33 = puStackY_4c + -1;
                  pwVar1 = (word *)(pbVar24 + 0x18);
                  *(uint *)pwVar1 = *(uint *)pwVar1 - 1;
                  puStackY_4c = puVar33;
                } while (*(uint *)pwVar1 != 0);
                *(uint *)(pbVar24 + 0x18) = *(uint *)(pbVar24 + 0x18) + 1;
              }
LAB_004ad066:
              uVar17 = *(uint *)(pbVar24 + 0x1c);
              iVar11 = *(uint *)(pbVar24 + 0x18) * uVar17;
              iStack_2c = iVar10 + iVar11 * -4;
              if (iStack_2c == 0 || iVar10 < iVar11 * 4) goto LAB_004ad0e9;
              puStack_34 = puStack_34 + -iVar11;
              uStack_38 = uStack_38 + iVar11 * 4;
              uVar5 = *(uint *)pbVar24;
              uVar23 = *(uint *)(pbVar24 + 4);
              uVar6 = *(uint *)(pbVar24 + 0x18);
              uVar19 = uVar6;
              puVar33 = puStack_34;
              puVar34 = puVar27;
              do {
                for (; uVar19 != 0; uVar19 = uVar19 - 1) {
                  *puVar33 = *puVar27;
                  puVar27 = puVar27 + 1;
                  puVar33 = puVar33 + 1;
                }
                puVar27 = puVar34 + iVar21;
                uVar17 = uVar17 - 1;
                uVar20 = uVar5;
                uVar19 = uVar6;
                puVar35 = puVar26;
                puVar34 = puVar27;
                puVar36 = puVar26;
              } while (uVar17 != 0);
              do {
                for (; uVar20 != 0; uVar20 = uVar20 - 1) {
                  *puVar35 = uVar13;
                  puVar35 = puVar35 + 1;
                }
                uVar23 = uVar23 - 1;
                uVar20 = uVar5;
                puVar35 = puVar36 + iVar21;
                puVar36 = puVar36 + iVar21;
              } while (uVar23 != 0);
              bVar37 = &stack0x00000000 == (undefined1 *)0x40;
              goto code_r0x004ace04;
            }
            iStack_24 = iStack_24 + -1;
            bVar37 = iStack_24 == 0;
            iVar12 = iVar21;
          } while (!bVar37);
          iVar21 = ((GVar40.assetOrError)->tableDescriptor).subresourceCount * 0x20;
          uVar13 = uStack_38 >> 2;
          ((GVar40.assetOrError)->common).allocationSizeBytes = uStack_38 + iVar21 + 0x200;
          pbVar24 = GVar40.assetOrError[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                    iVar21 + -0x28;
          for (; uVar13 != 0; uVar13 = uVar13 - 1) {
            *(uint *)pbVar24 = *puStack_34;
            puStack_34 = puStack_34 + 1;
            pbVar24 = pbVar24 + 4;
          }
          AVar39 = (*g_MemoryApi.shrinkInPlace)
                             (((GVar40.assetOrError)->common).allocationSizeBytes,
                              GVar40.assetOrError);
          pGVar7 = (GraphicsTextureSourceAsset *)AVar39.scratchOrError;
          if (!AVar39.carry) {
            PVar25 = ((GVar40.assetOrError)->common).allocationSizeBytes;
            pGVar7 = GVar40.assetOrError + 1;
            AVar14 = ((GVar40.assetOrError)->tableDescriptor).subresourceCount;
            do {
              PVar25 = PVar25 + (pGVar7->common).buildMetadata.timestamps.dateValue1 *
                                (pGVar7->common).buildMetadata.timestamps.timeValue1 * -4;
              (pGVar7->common).converterVersion = PVar25;
              pGVar7 = (GraphicsTextureSourceAsset *)
                       &(pGVar7->common).buildMetadata.timestamps.dateValue2;
              AVar14 = AVar14 - 1;
            } while (AVar14 != 0);
            GVar41.carry = false;
            GVar41.assetOrError = GVar40.assetOrError;
            return GVar41;
          }
        }
      }
    }
    else {
      ((GVar40.assetOrError)->tableDescriptor).paletteBankCount = 1;
      ((GVar40.assetOrError)->tableDescriptor).subresourceCount = 0;
      ((GVar40.assetOrError)->tableDescriptor).subresourceTableOffset = 0xa00;
      pGVar7 = (GraphicsTextureSourceAsset *)0x14;
      iVar12 = dVar9 - 0xa00;
      if (iVar12 != 0 && 0x7ff < iVar10) {
        pGVar7 = sourceAsset + iVar11 * 4 + 1;
        pGVar28 = GVar40.assetOrError + 1;
        for (iVar10 = 0x200; iVar10 != 0; iVar10 = iVar10 + -1) {
          (pGVar28->common).magic = (pGVar7->common).magic;
          pGVar7 = (GraphicsTextureSourceAsset *)&(pGVar7->common).allocationSizeBytes;
          pGVar28 = (GraphicsTextureSourceAsset *)&(pGVar28->common).allocationSizeBytes;
        }
        bVar2 = *pbVar24;
        iVar11 = iVar21 * iStack_24;
        bVar37 = true;
        iVar10 = iVar11;
        pbVar29 = pbVar24;
        do {
          pbVar30 = pbVar29;
          if (iVar10 == 0) break;
          iVar10 = iVar10 + -1;
          pbVar30 = pbVar29 + 1;
          bVar37 = bVar2 == *pbVar29;
          pbVar29 = pbVar30;
        } while (bVar37);
        bVar3 = pbVar30[-1];
        pGVar7 = (GraphicsTextureSourceAsset *)0x2d;
        if (!bVar37) {
          pGVar7 = (GraphicsTextureSourceAsset *)0x14;
          iVar10 = iVar12 - iVar11;
          if (iVar10 != 0 && iVar11 <= iVar12) {
            pbVar29 = (byte *)((int)pGVar28 + iVar10);
            pbVar30 = pbVar29;
            for (; iVar11 != 0; iVar11 = iVar11 + -1) {
              *pbVar30 = *pbVar24;
              pbVar24 = pbVar24 + 1;
              pbVar30 = pbVar30 + 1;
            }
            bVar37 = (*(uint *)(GVar40.assetOrError[1].common.buildMetadata.
                                assetRelativeAddressAnchor28 + (uint)bVar3 * 8 + -0x28) & 0xff000000
                     ) == 0;
            pbVar24 = GVar40.assetOrError[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                      (uint)bVar3 * 8 + -0x28;
            *(uint *)pbVar24 = *(uint *)pbVar24 & 0xffffff;
            puStack_34 = (uint *)((uint)pbVar29 & 0xfffffffc);
            uStack_38 = 0;
            pGVar7 = (GraphicsTextureSourceAsset *)0x14;
            iStack_2c = iVar10 + -3;
            bVar38 = iStack_2c == 0;
            iVar12 = iVar21;
            if (!bVar38 && 2 < iVar10) {
code_r0x004acaa4:
              do {
                if (iVar12 != 0) {
                  iVar12 = iVar12 + -1;
                  pbVar24 = pbVar29 + 1;
                  bVar38 = bVar2 == *pbVar29;
                  pbVar29 = pbVar24;
                  if (bVar38) goto code_r0x004acaa4;
                }
                if (!bVar38) {
                  pGVar7 = (GraphicsTextureSourceAsset *)0x14;
                  iVar10 = iStack_2c + -0x20;
                  if (iVar10 == 0 || iStack_2c < 0x20) goto LAB_004ad0ec;
                  AVar14 = ((GVar40.assetOrError)->tableDescriptor).subresourceCount;
                  pbVar29 = pbVar29 + -1;
                  ((GVar40.assetOrError)->tableDescriptor).subresourceCount =
                       ((GVar40.assetOrError)->tableDescriptor).subresourceCount + 1;
                  iVar22 = AVar14 * 0x20;
                  bVar38 = iVar22 == 0;
                  iVar11 = iVar12 + 1;
                  pbVar24 = pbVar29;
                  do {
                    pbVar30 = pbVar24;
                    if (iVar11 == 0) break;
                    iVar11 = iVar11 + -1;
                    pbVar30 = pbVar24 + 1;
                    bVar38 = bVar2 == *pbVar24;
                    pbVar24 = pbVar30;
                  } while (!bVar38);
                  if (bVar38) {
                    pbVar30 = pbVar30 + -1;
                  }
                  pbVar24 = GVar40.assetOrError[5].common.buildMetadata.assetRelativeAddressAnchor28
                            + iVar22 + -0x28;
                  *(int *)pbVar24 = (int)pbVar30 - (int)pbVar29;
                  pbVar24[4] = 0;
                  pbVar24[5] = 0;
                  pbVar24[6] = 0;
                  pbVar24[7] = 0;
                  *(word *)((int)(pbVar24 + 0x10) + 0) = 0;
                  *(word *)((int)(pbVar24 + 0x10) + 2) = 0;
                  *(word *)((int)(pbVar24 + 0x14) + 0) = 0;
                  *(word *)((int)(pbVar24 + 0x14) + 2) = 0;
                  ((AssetProducerSourceNames *)(pbVar24 + 8))->producerName[0] = 0;
                  ((AssetProducerSourceNames *)(pbVar24 + 8))->producerName[1] = 0;
                  iVar11 = iStack_24;
                  pbVar30 = pbVar29;
                  do {
                    *(int *)(pbVar24 + 4) = *(int *)(pbVar24 + 4) + 1;
                    pbVar30 = pbVar30 + iVar21;
                    iVar11 = iVar11 + -1;
                    bVar38 = true;
                    if (iVar11 == 0) break;
                    bVar38 = bVar2 == *pbVar30;
                  } while (!bVar38);
                  *(int *)(pbVar24 + 0x18) = *(int *)pbVar24;
                  *(int *)(pbVar24 + 0x1c) = *(int *)(pbVar24 + 4);
                  pbVar30 = pbVar29;
                  do {
                    iVar11 = *(int *)pbVar24;
                    pbVar31 = pbVar30;
                    do {
                      if (iVar11 == 0) break;
                      iVar11 = iVar11 + -1;
                      bVar38 = bVar3 == *pbVar31;
                      pbVar31 = pbVar31 + 1;
                    } while (bVar38);
                    if ((!bVar38) || (!bVar37)) goto LAB_004acc23;
                    pbVar30 = pbVar30 + iVar21;
                    *(int *)(pbVar24 + 0x14) = *(int *)(pbVar24 + 0x14) + 1;
                    pwVar1 = (word *)(pbVar24 + 0x1c);
                    *(int *)pwVar1 = *(int *)pwVar1 + -1;
                    bVar38 = *(int *)pwVar1 == 0;
                  } while (!bVar38);
                  pbVar30 = pbVar30 + -iVar21;
                  *(int *)(pbVar24 + 0x14) = *(int *)(pbVar24 + 0x14) + -1;
                  *(int *)(pbVar24 + 0x1c) = *(int *)(pbVar24 + 0x1c) + 1;
LAB_004acc23:
                  pbVar31 = pbVar30 + iVar21 * *(int *)(pbVar24 + 0x1c);
                  do {
                    pbVar31 = pbVar31 + -iVar21;
                    bVar38 = pbVar31 == (byte *)0x0;
                    iVar11 = *(int *)pbVar24;
                    pbVar32 = pbVar31;
                    do {
                      if (iVar11 == 0) break;
                      iVar11 = iVar11 + -1;
                      bVar38 = bVar3 == *pbVar32;
                      pbVar32 = pbVar32 + 1;
                    } while (bVar38);
                    if ((!bVar38) || (!bVar37)) goto LAB_004acc64;
                    pwVar1 = (word *)(pbVar24 + 0x1c);
                    *(int *)pwVar1 = *(int *)pwVar1 + -1;
                  } while (*(int *)pwVar1 != 0);
                  *(int *)(pbVar24 + 0x1c) = *(int *)(pbVar24 + 0x1c) + 1;
LAB_004acc64:
                  iVar11 = *(int *)(pbVar24 + 0x1c);
                  puStackY_4c = (uint *)pbVar30;
                  puVar26 = (uint *)pbVar30;
                  if (bVar37) {
                    do {
                      do {
                        puStackY_4c = puVar26;
                        if (bVar3 != *pbVar30) goto LAB_004accb0;
                        pbVar30 = pbVar30 + iVar21;
                        iVar11 = iVar11 + -1;
                        puVar26 = puStackY_4c;
                      } while (iVar11 != 0);
                      *(int *)(pbVar24 + 0x10) = *(int *)(pbVar24 + 0x10) + 1;
                      pbVar30 = (byte *)((int)puStackY_4c + 1);
                      iVar11 = *(int *)(pbVar24 + 0x1c);
                      pwVar1 = (word *)(pbVar24 + 0x18);
                      *(int *)pwVar1 = *(int *)pwVar1 + -1;
                      puVar26 = (uint *)pbVar30;
                    } while (*(int *)pwVar1 != 0);
                    *(int *)(pbVar24 + 0x10) = *(int *)(pbVar24 + 0x10) + -1;
                    *(int *)(pbVar24 + 0x18) = *(int *)(pbVar24 + 0x18) + 1;
                  }
LAB_004accb0:
                  puVar26 = puStackY_4c;
                  iVar11 = *(int *)(pbVar24 + 0x1c);
                  pbVar30 = (byte *)((int)puStackY_4c + *(int *)(pbVar24 + 0x18) + -1);
                  puStackY_4c = (uint *)pbVar30;
                  if (bVar37) {
                    do {
                      do {
                        if (bVar3 != *pbVar30) goto LAB_004accf1;
                        pbVar30 = pbVar30 + iVar21;
                        iVar11 = iVar11 + -1;
                      } while (iVar11 != 0);
                      iVar11 = *(int *)(pbVar24 + 0x1c);
                      pbVar30 = (byte *)((int)puStackY_4c + -1);
                      pwVar1 = (word *)(pbVar24 + 0x18);
                      *(int *)pwVar1 = *(int *)pwVar1 + -1;
                      puStackY_4c = (uint *)pbVar30;
                    } while (*(int *)pwVar1 != 0);
                    *(int *)(pbVar24 + 0x18) = *(int *)(pbVar24 + 0x18) + 1;
                  }
LAB_004accf1:
                  iVar11 = *(int *)(pbVar24 + 0x1c);
                  uVar13 = *(int *)(pbVar24 + 0x18) * iVar11 + 3U & 0xfffffffc;
                  iStack_2c = iVar10 - uVar13;
                  if (iStack_2c == 0 || iVar10 < (int)uVar13) goto LAB_004ad0e9;
                  puStack_34 = (uint *)((int)puStack_34 + -uVar13);
                  uStack_38 = uStack_38 + uVar13;
                  iVar10 = *(int *)pbVar24;
                  iVar22 = *(int *)(pbVar24 + 4);
                  iVar4 = *(int *)(pbVar24 + 0x18);
                  iVar15 = iVar4;
                  pbVar24 = (byte *)puStack_34;
                  puVar27 = puVar26;
                  do {
                    for (; iVar15 != 0; iVar15 = iVar15 + -1) {
                      *pbVar24 = *(byte *)puVar26;
                      puVar26 = (uint *)((int)puVar26 + 1);
                      pbVar24 = pbVar24 + 1;
                    }
                    puVar26 = (uint *)((int)puVar27 + iVar21);
                    iVar11 = iVar11 + -1;
                    iVar16 = iVar10;
                    iVar15 = iVar4;
                    pbVar30 = pbVar29;
                    puVar27 = puVar26;
                    pbVar31 = pbVar29;
                  } while (iVar11 != 0);
                  do {
                    for (; iVar16 != 0; iVar16 = iVar16 + -1) {
                      *pbVar30 = bVar2;
                      pbVar30 = pbVar30 + 1;
                    }
                    iVar22 = iVar22 + -1;
                    iVar16 = iVar10;
                    pbVar30 = pbVar31 + iVar21;
                    pbVar31 = pbVar31 + iVar21;
                  } while (iVar22 != 0);
                  bVar38 = &stack0x00000000 == (undefined1 *)0x40;
                  iVar12 = iVar12 + 1;
                  goto code_r0x004acaa4;
                }
                iStack_24 = iStack_24 + -1;
                bVar38 = iStack_24 == 0;
                iVar12 = iVar21;
              } while (!bVar38);
              iVar21 = ((GVar40.assetOrError)->tableDescriptor).subresourceCount * 0x20;
              uVar13 = uStack_38 >> 2;
              ((GVar40.assetOrError)->common).allocationSizeBytes = uStack_38 + iVar21 + 0xa00;
              pbVar24 = GVar40.assetOrError[5].common.buildMetadata.assetRelativeAddressAnchor28 +
                        iVar21 + -0x28;
              for (; uVar13 != 0; uVar13 = uVar13 - 1) {
                *(uint *)pbVar24 = *puStack_34;
                puStack_34 = (uint *)((int)puStack_34 + 4);
                pbVar24 = pbVar24 + 4;
              }
              AVar39 = (*g_MemoryApi.shrinkInPlace)
                                 (((GVar40.assetOrError)->common).allocationSizeBytes,
                                  GVar40.assetOrError);
              pGVar7 = (GraphicsTextureSourceAsset *)AVar39.scratchOrError;
              if (!AVar39.carry) {
                PVar25 = ((GVar40.assetOrError)->common).allocationSizeBytes;
                pGVar7 = GVar40.assetOrError + 5;
                AVar14 = ((GVar40.assetOrError)->tableDescriptor).subresourceCount;
                do {
                  PVar25 = PVar25 - ((pGVar7->common).buildMetadata.timestamps.dateValue1 *
                                     (pGVar7->common).buildMetadata.timestamps.timeValue1 + 3 &
                                    0xfffffffc);
                  (pGVar7->common).converterVersion = PVar25;
                  pGVar7 = (GraphicsTextureSourceAsset *)
                           &(pGVar7->common).buildMetadata.timestamps.dateValue2;
                  AVar14 = AVar14 - 1;
                } while (AVar14 != 0);
                GVar40.carry = false;
                return GVar40;
              }
            }
          }
        }
      }
    }
  }
LAB_004ad0ec:
  (*g_MemoryApi.free)(GVar40.assetOrError);
  GVar40.assetOrError = pGVar7;
LAB_004ad0f7:
  GVar42.carry = true;
  GVar42.assetOrError = GVar40.assetOrError;
  return GVar42;
LAB_004ad0e9:
  pGVar7 = (GraphicsTextureSourceAsset *)0x14;
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
  PackageLoadEntryEaxCf5 PVar1;
  GraphicsTextureSourceLoadEaxCf5 GVar2;
  
  PVar1 = Package_LoadEntry(pathUtf16);
  loadedPaletteTextureSource = PVar1.bufferOrError;
  if (!PVar1.carry) {
    GVar2 = (GraphicsTextureSourceLoadEaxCf5)
            (*g_GraphicsTextureSourceConvertPaletteEntries)(loadedPaletteTextureSource);
    convertedTextureSource = (GraphicsPaletteTextureSourceAsset *)GVar2.eax;
    if (!GVar2.carry) {
      return GVar2;
    }
    Resource_Release(loadedPaletteTextureSource);
    loadedPaletteTextureSource = convertedTextureSource;
  }
  GVar2.carry = true;
  GVar2.eax = (GraphicsTextureSourceAsset *)loadedPaletteTextureSource;
  return GVar2;
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
  GraphicsPaletteTextureSourceAsset *arg0;
  uint uVar1;
  GraphicsPaletteTextureSourceAsset *pGVar2;
  ArenaAllocEaxCf5 AVar3;
  GraphicsPaletteTextureSourceEaxCf5 GVar4;
  ArenaFreeEaxCf5 AVar5;
  
  uVar1 = (sourceAsset->common).allocationSizeBytes;
  AVar3 = (*g_MemoryApi.alloc)(uVar1);
  arg0 = (GraphicsPaletteTextureSourceAsset *)AVar3.eax;
  if (!AVar3.carry) {
    pGVar2 = arg0;
    for (uVar1 = uVar1 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
      pGVar2->magic = (sourceAsset->common).magic;
      sourceAsset = (GraphicsTextureSourceAsset *)&(sourceAsset->common).allocationSizeBytes;
      pGVar2 = (GraphicsPaletteTextureSourceAsset *)&pGVar2->allocationSizeBytes;
    }
    GVar4 = (*g_GraphicsTextureSourceConvertPaletteEntries)(arg0);
    if (!GVar4.carry) {
      return (GraphicsTextureSourceAsset *)GVar4.paletteSource;
    }
    AVar5 = (*g_MemoryApi.free)(arg0);
    arg0 = (GraphicsPaletteTextureSourceAsset *)AVar5.eax;
  }
  return (GraphicsTextureSourceAsset *)arg0;
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
  GraphicsPaletteTextureSourceEaxCf5 GVar1;
  GraphicsPaletteTextureSourceEaxCf5 GVar2;
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
    GVar1.carry = false;
    GVar1.paletteSource = sourceAsset;
    return GVar1;
  }
  GVar2.carry = true;
  GVar2.paletteSource = (GraphicsPaletteTextureSourceAsset *)0x2c;
  return GVar2;
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
  uint uVar1;
  undefined4 in_EAX;
  byte *firstSubresourceRecord;
  bool bVar2;
  GraphicsTextureSizeEaxEdxCf9 GVar3;
  uint subresourceCount;
  
  bVar2 = true;
  if ((sourceAsset->common).magic == ASSET_MAGIC_GFX) {
    uVar1 = (sourceAsset->tableDescriptor).subresourceCount;
    bVar2 = true;
    if ((uVar1 != 0) && (bVar2 = 0xfff < uVar1, !bVar2)) {
      firstSubresourceRecord =
           (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
           ((sourceAsset->tableDescriptor).subresourceTableOffset - 0x28);
      in_EAX = *(undefined4 *)firstSubresourceRecord;
      sourceAsset = *(GraphicsTextureSourceAsset **)(firstSubresourceRecord + 4);
      bVar2 = false;
    }
  }
  GVar3.logicalHeightPixels = (dword)sourceAsset;
  GVar3.logicalWidthPixels = in_EAX;
  GVar3.carry = bVar2;
  return GVar3;
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
  int iVar1;
  DDPIXELFORMAT *pDVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined1 uVar7;
  undefined1 uVar8;
  undefined1 uVar9;
  TH_LEGACY_LONG TVar10;
  TH_LEGACY_LPVOID arg1;
  TH_LEGACY_HRESULT TVar11;
  int iVar12;
  DirectDrawPaletteEntry DVar13;
  byte *pbVar14;
  GraphicsTextureSourceAsset *pGVar15;
  ushort *puVar16;
  byte *pbVar17;
  DirectDrawPaletteEntry *pDVar18;
  uint *puVar19;
  byte *pbVar20;
  ushort *puVar21;
  byte *pbVar22;
  uint *puVar23;
  int *piStack_60;
  GraphicsTextureSourceAsset *pGStack_5c;
  int iStack_58;
  int iStack_54;
  int iStack_50;
  int iStack_4c;
  int iStack_48;
  int iStack_44;
  int iStack_40;
  int iStack_3c;
  int iStack_38;
  int iStack_34;
  TH_LEGACY_LONG TStack_30;
  TH_LEGACY_LPVOID pvStack_2c;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    TVar11 = (*stagingSurface3->lpVtbl->IsLost)(stagingSurface3);
    iVar12 = 0;
    if (TVar11 != 0) {
      iVar12 = (*stagingSurface3->lpVtbl->Restore)(stagingSurface3);
    }
    if (iVar12 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar11 = (*stagingSurface3->lpVtbl->Lock)
                         (stagingSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,
                          (TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      TVar10 = g_SurfaceDesc.lPitch;
      if (TVar11 == 0) {
        iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
        TStack_30 = g_SurfaceDesc.lPitch;
        pvStack_2c = g_SurfaceDesc.lpSurface;
        iVar12 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         iVar1 + -0x20);
        iStack_54 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar1 + -0x10);
        iStack_58 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar1 + -0xc);
        iVar1 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x1c);
        if (iVar12 < 0) {
          pbVar14 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x28
          ;
          if ((iStack_54 != 0) && (pDVar2 = texture->pixelFormat, iStack_58 != 0)) {
            iVar12 = iStack_54;
            pbVar17 = g_SurfaceDesc.lpSurface;
            pbVar20 = pbVar14;
            pbVar22 = g_SurfaceDesc.lpSurface;
            if (pDVar2->dwRGBBitCount == 8) {
              do {
                do {
                  *pbVar17 = pbVar14[3];
                  pbVar14 = pbVar14 + 4;
                  iVar12 = iVar12 + -1;
                  pbVar17 = pbVar17 + 1;
                } while (iVar12 != 0);
                pbVar14 = pbVar20 + iStack_54 * 4;
                iStack_58 = iStack_58 + -1;
                iVar12 = iStack_54;
                pbVar17 = pbVar22 + TVar10;
                pbVar20 = pbVar14;
                pbVar22 = pbVar22 + TVar10;
              } while (iStack_58 != 0);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              iVar12 = 0x100;
              DVar13.red = 0;
              DVar13.green = 0;
              DVar13.blue = 0;
              DVar13.flags = 0;
              pDVar18 = g_TexturePaletteEntries;
              do {
                *pDVar18 = DVar13;
                pDVar18 = pDVar18 + 1;
                DVar13 = (DirectDrawPaletteEntry)((int)DVar13 + 0x1010101);
                iVar12 = iVar12 + -1;
              } while (iVar12 != 0);
              TVar11 = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&piStack_60,
                                  (TH_LEGACY_LPVOID)0x0);
              if (TVar11 == 0) {
                (**(code **)(*piStack_60 + 8))(piStack_60);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
            uVar3 = pDVar2->dwRBitMask;
            uVar4 = pDVar2->dwGBitMask;
            uVar5 = pDVar2->dwBBitMask;
            if (((uVar3 != 0) && (uVar4 != 0)) && (uVar5 != 0)) {
              iStack_34 = 0;
              if (uVar3 != 0) {
                for (; (uVar3 >> iStack_34 & 1) == 0; iStack_34 = iStack_34 + 1) {
                }
              }
              iStack_38 = 0;
              if (uVar4 != 0) {
                for (; (uVar4 >> iStack_38 & 1) == 0; iStack_38 = iStack_38 + 1) {
                }
              }
              iStack_3c = 0;
              if (uVar5 != 0) {
                for (; (uVar5 >> iStack_3c & 1) == 0; iStack_3c = iStack_3c + 1) {
                }
              }
              iVar12 = 0x1f;
              if (pDVar2->dwRBitMask != 0) {
                for (; pDVar2->dwRBitMask >> iVar12 == 0; iVar12 = iVar12 + -1) {
                }
              }
              iVar1 = 0x1f;
              if (pDVar2->dwGBitMask != 0) {
                for (; pDVar2->dwGBitMask >> iVar1 == 0; iVar1 = iVar1 + -1) {
                }
              }
              iVar6 = 0x1f;
              if (pDVar2->dwBBitMask != 0) {
                for (; pDVar2->dwBBitMask >> iVar6 == 0; iVar6 = iVar6 + -1) {
                }
              }
              iStack_44 = 0x18 - ((iVar12 + 1) - iStack_34);
              iStack_48 = 0x10 - ((iVar1 + 1) - iStack_38);
              iStack_4c = 8 - ((iVar6 + 1) - iStack_3c);
              uVar3 = pDVar2->dwRGBAlphaBitMask;
              if (uVar3 == 0) {
                iStack_50 = 0;
                iStack_40 = 0x10;
              }
              else {
                iStack_40 = 0;
                if (uVar3 != 0) {
                  for (; (uVar3 >> iStack_40 & 1) == 0; iStack_40 = iStack_40 + 1) {
                  }
                }
                iVar12 = 0x1f;
                if (uVar3 != 0) {
                  for (; uVar3 >> iVar12 == 0; iVar12 = iVar12 + -1) {
                  }
                }
                iStack_50 = 0x20 - ((iVar12 + 1) - iStack_40);
              }
              iVar12 = iStack_54;
              puVar19 = g_SurfaceDesc.lpSurface;
              puVar16 = g_SurfaceDesc.lpSurface;
              pbVar17 = pbVar14;
              puVar23 = g_SurfaceDesc.lpSurface;
              puVar21 = g_SurfaceDesc.lpSurface;
              if (pDVar2->dwRGBBitCount < 0x11) {
                do {
                  do {
                    uVar3 = *(uint *)pbVar14;
                    *puVar16 = (ushort)(((uVar3 & 0xff000000) >> ((byte)iStack_50 & 0x1f)) <<
                                       ((byte)iStack_40 & 0x1f)) |
                               (ushort)(((uVar3 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                                       ((byte)iStack_3c & 0x1f)) |
                               (ushort)(((uVar3 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                                       ((byte)iStack_38 & 0x1f)) |
                               (ushort)(((uVar3 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                                       ((byte)iStack_34 & 0x1f));
                    pbVar14 = pbVar14 + 4;
                    iVar12 = iVar12 + -1;
                    puVar16 = puVar16 + 1;
                  } while (iVar12 != 0);
                  pbVar14 = pbVar17 + iStack_54 * 4;
                  iStack_58 = iStack_58 + -1;
                  iVar12 = iStack_54;
                  puVar16 = (ushort *)((int)puVar21 + TVar10);
                  pbVar17 = pbVar14;
                  puVar21 = (ushort *)((int)puVar21 + TVar10);
                } while (iStack_58 != 0);
                iStack_58 = 0;
              }
              else {
                do {
                  do {
                    uVar3 = *(uint *)pbVar14;
                    *puVar19 = ((uVar3 & 0xff000000) >> ((byte)iStack_50 & 0x1f)) <<
                               ((byte)iStack_40 & 0x1f) |
                               ((uVar3 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                               ((byte)iStack_3c & 0x1f) |
                               ((uVar3 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                               ((byte)iStack_38 & 0x1f) |
                               ((uVar3 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                               ((byte)iStack_34 & 0x1f);
                    pbVar14 = pbVar14 + 4;
                    iVar12 = iVar12 + -1;
                    puVar19 = puVar19 + 1;
                  } while (iVar12 != 0);
                  pbVar14 = pbVar17 + iStack_54 * 4;
                  iStack_58 = iStack_58 + -1;
                  iVar12 = iStack_54;
                  puVar19 = (uint *)((int)puVar23 + TVar10);
                  pbVar17 = pbVar14;
                  puVar23 = (uint *)((int)puVar23 + TVar10);
                } while (iStack_58 != 0);
                iStack_58 = 0;
              }
            }
          }
        }
        else {
          pbVar14 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x28
          ;
          pGStack_5c = sourceAsset + iVar12 * 4 + 1;
          if ((iStack_54 != 0) && (pDVar2 = texture->pixelFormat, iStack_58 != 0)) {
            iVar12 = iStack_54;
            pbVar17 = g_SurfaceDesc.lpSurface;
            pbVar20 = pbVar14;
            pbVar22 = g_SurfaceDesc.lpSurface;
            if (pDVar2->dwRGBBitCount == 8) {
              do {
                do {
                  *pbVar17 = *pbVar14;
                  pbVar14 = pbVar14 + 1;
                  iVar12 = iVar12 + -1;
                  pbVar17 = pbVar17 + 1;
                } while (iVar12 != 0);
                pbVar14 = pbVar20 + iStack_54;
                iStack_58 = iStack_58 + -1;
                iVar12 = iStack_54;
                pbVar17 = pbVar22 + TVar10;
                pbVar20 = pbVar14;
                pbVar22 = pbVar22 + TVar10;
              } while (iStack_58 != 0);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              iVar12 = 0x100;
              pGVar15 = pGStack_5c;
              pDVar18 = g_TexturePaletteEntries;
              do {
                uVar7 = *(undefined1 *)((int)&(pGVar15->common).magic + 1);
                uVar8 = *(undefined1 *)((int)&(pGVar15->common).magic + 2);
                uVar9 = *(undefined1 *)((int)&(pGVar15->common).magic + 3);
                pDVar18->red = *(undefined1 *)&(pGVar15->common).magic;
                pDVar18->green = uVar7;
                pDVar18->blue = uVar8;
                pDVar18->flags = uVar9;
                pGVar15 = (GraphicsTextureSourceAsset *)&(pGVar15->common).formatVersion;
                pDVar18 = pDVar18 + 1;
                iVar12 = iVar12 + -1;
              } while (iVar12 != 0);
              TVar11 = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&piStack_60,
                                  (TH_LEGACY_LPVOID)0x0);
              if (TVar11 == 0) {
                (**(code **)(*piStack_60 + 8))(piStack_60);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
            uVar3 = pDVar2->dwRBitMask;
            uVar4 = pDVar2->dwGBitMask;
            uVar5 = pDVar2->dwBBitMask;
            if (((uVar3 != 0) && (uVar4 != 0)) && (uVar5 != 0)) {
              iStack_34 = 0;
              if (uVar3 != 0) {
                for (; (uVar3 >> iStack_34 & 1) == 0; iStack_34 = iStack_34 + 1) {
                }
              }
              iStack_38 = 0;
              if (uVar4 != 0) {
                for (; (uVar4 >> iStack_38 & 1) == 0; iStack_38 = iStack_38 + 1) {
                }
              }
              iStack_3c = 0;
              if (uVar5 != 0) {
                for (; (uVar5 >> iStack_3c & 1) == 0; iStack_3c = iStack_3c + 1) {
                }
              }
              iVar12 = 0x1f;
              if (pDVar2->dwRBitMask != 0) {
                for (; pDVar2->dwRBitMask >> iVar12 == 0; iVar12 = iVar12 + -1) {
                }
              }
              iVar1 = 0x1f;
              if (pDVar2->dwGBitMask != 0) {
                for (; pDVar2->dwGBitMask >> iVar1 == 0; iVar1 = iVar1 + -1) {
                }
              }
              iVar6 = 0x1f;
              if (pDVar2->dwBBitMask != 0) {
                for (; pDVar2->dwBBitMask >> iVar6 == 0; iVar6 = iVar6 + -1) {
                }
              }
              iStack_44 = 0x18 - ((iVar12 + 1) - iStack_34);
              iStack_48 = 0x10 - ((iVar1 + 1) - iStack_38);
              iStack_4c = 8 - ((iVar6 + 1) - iStack_3c);
              uVar3 = pDVar2->dwRGBAlphaBitMask;
              if (uVar3 == 0) {
                iStack_50 = 0;
                iStack_40 = 0x10;
              }
              else {
                iStack_40 = 0;
                if (uVar3 != 0) {
                  for (; (uVar3 >> iStack_40 & 1) == 0; iStack_40 = iStack_40 + 1) {
                  }
                }
                iVar12 = 0x1f;
                if (uVar3 != 0) {
                  for (; uVar3 >> iVar12 == 0; iVar12 = iVar12 + -1) {
                  }
                }
                iStack_50 = 0x20 - ((iVar12 + 1) - iStack_40);
              }
              iVar12 = iStack_54;
              puVar19 = g_SurfaceDesc.lpSurface;
              puVar16 = g_SurfaceDesc.lpSurface;
              pbVar17 = pbVar14;
              puVar23 = g_SurfaceDesc.lpSurface;
              puVar21 = g_SurfaceDesc.lpSurface;
              if (pDVar2->dwRGBBitCount < 0x11) {
                do {
                  do {
                    uVar3 = *(uint *)((pGStack_5c->common).buildMetadata.
                                      assetRelativeAddressAnchor28 + (uint)*pbVar14 * 8 + -0x28);
                    *puVar16 = (ushort)(((uVar3 & 0xff000000) >> ((byte)iStack_50 & 0x1f)) <<
                                       ((byte)iStack_40 & 0x1f)) |
                               (ushort)(((uVar3 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                                       ((byte)iStack_3c & 0x1f)) |
                               (ushort)(((uVar3 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                                       ((byte)iStack_38 & 0x1f)) |
                               (ushort)(((uVar3 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                                       ((byte)iStack_34 & 0x1f));
                    pbVar14 = pbVar14 + 1;
                    iVar12 = iVar12 + -1;
                    puVar16 = puVar16 + 1;
                  } while (iVar12 != 0);
                  pbVar14 = pbVar17 + iStack_54;
                  iStack_58 = iStack_58 + -1;
                  iVar12 = iStack_54;
                  puVar16 = (ushort *)((int)puVar21 + TVar10);
                  pbVar17 = pbVar14;
                  puVar21 = (ushort *)((int)puVar21 + TVar10);
                } while (iStack_58 != 0);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              }
              else {
                do {
                  do {
                    uVar3 = *(uint *)((pGStack_5c->common).buildMetadata.
                                      assetRelativeAddressAnchor28 + (uint)*pbVar14 * 8 + -0x28);
                    *puVar19 = ((uVar3 & 0xff000000) >> ((byte)iStack_50 & 0x1f)) <<
                               ((byte)iStack_40 & 0x1f) |
                               ((uVar3 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                               ((byte)iStack_3c & 0x1f) |
                               ((uVar3 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                               ((byte)iStack_38 & 0x1f) |
                               ((uVar3 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                               ((byte)iStack_34 & 0x1f);
                    pbVar14 = pbVar14 + 1;
                    iVar12 = iVar12 + -1;
                    puVar19 = puVar19 + 1;
                  } while (iVar12 != 0);
                  pbVar14 = pbVar17 + iStack_54;
                  iStack_58 = iStack_58 + -1;
                  iVar12 = iStack_54;
                  puVar19 = (uint *)((int)puVar23 + TVar10);
                  pbVar17 = pbVar14;
                  puVar23 = (uint *)((int)puVar23 + TVar10);
                } while (iStack_58 != 0);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              }
              goto GraphicsTextureUploadColor1x_DecrementActiveCountAndReturn;
            }
          }
        }
        (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
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
  DDPIXELFORMAT *pDVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  bool bVar9;
  int iVar10;
  undefined1 uVar11;
  undefined1 uVar12;
  undefined1 uVar13;
  TH_LEGACY_LONG TVar14;
  TH_LEGACY_LPVOID arg1;
  TH_LEGACY_HRESULT TVar15;
  int iVar16;
  DirectDrawPaletteEntry DVar17;
  int iVar18;
  byte *pbVar19;
  GraphicsTextureSourceAsset *pGVar20;
  AssetProducerSourceNames *pAVar21;
  byte *pbVar22;
  ushort *puVar23;
  DirectDrawPaletteEntry *pDVar24;
  uint *puVar25;
  ushort uVar26;
  uint3 uVar27;
  ushort uVar28;
  ushort uVar29;
  ushort uVar30;
  ushort uVar31;
  ushort uVar32;
  ushort uVar33;
  byte *pbVar34;
  AssetProducerSourceNames *pAVar35;
  byte *pbVar36;
  ushort *puVar37;
  uint *puVar38;
  int *piStack_60;
  GraphicsTextureSourceAsset *pGStack_5c;
  int iStack_58;
  int iStack_54;
  int iStack_50;
  int iStack_4c;
  int iStack_48;
  int iStack_44;
  int iStack_40;
  int iStack_3c;
  int iStack_38;
  int iStack_34;
  TH_LEGACY_LONG TStack_30;
  TH_LEGACY_LPVOID pvStack_2c;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    TVar15 = (*stagingSurface3->lpVtbl->IsLost)(stagingSurface3);
    iVar16 = 0;
    if (TVar15 != 0) {
      iVar16 = (*stagingSurface3->lpVtbl->Restore)(stagingSurface3);
    }
    if (iVar16 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar15 = (*stagingSurface3->lpVtbl->Lock)
                         (stagingSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,
                          (TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      TVar14 = g_SurfaceDesc.lPitch;
      if (TVar15 == 0) {
        iVar18 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
        TStack_30 = g_SurfaceDesc.lPitch;
        pvStack_2c = g_SurfaceDesc.lpSurface;
        iVar16 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         iVar18 + -0x20);
        iStack_54 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar18 + -0x10);
        iStack_58 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar18 + -0xc);
        iVar18 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         iVar18 + -0x1c);
        if (iVar16 < 0) {
          pAVar21 = (AssetProducerSourceNames *)
                    ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    iVar18 + -0x28);
          if ((iStack_54 != 0) && (pDVar1 = texture->pixelFormat, iStack_58 != 0)) {
            iVar16 = iStack_54;
            pbVar19 = g_SurfaceDesc.lpSurface;
            pAVar35 = pAVar21;
            pbVar22 = g_SurfaceDesc.lpSurface;
            if (pDVar1->dwRGBBitCount == 8) {
              do {
                do {
                  *pbVar19 = *(byte *)((int)pAVar21->producerName + 3);
                  pAVar21 = (AssetProducerSourceNames *)((int)pAVar21->producerName + 8);
                  iVar18 = iVar16 + -2;
                  bVar9 = 1 < iVar16;
                  iVar16 = iVar18;
                  pbVar19 = pbVar19 + 1;
                } while (iVar18 != 0 && bVar9);
                pAVar21 = (AssetProducerSourceNames *)((int)pAVar35->producerName + iStack_54 * 8);
                iVar18 = iStack_58 + -2;
                bVar9 = 1 < iStack_58;
                iVar16 = iStack_54;
                pbVar19 = pbVar22 + TVar14;
                pAVar35 = pAVar21;
                pbVar22 = pbVar22 + TVar14;
                iStack_58 = iVar18;
              } while (iVar18 != 0 && bVar9);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              iVar16 = 0x100;
              DVar17.red = 0;
              DVar17.green = 0;
              DVar17.blue = 0;
              DVar17.flags = 0;
              pDVar24 = g_TexturePaletteEntries;
              do {
                *pDVar24 = DVar17;
                pDVar24 = pDVar24 + 1;
                DVar17 = (DirectDrawPaletteEntry)((int)DVar17 + 0x1010101);
                iVar16 = iVar16 + -1;
              } while (iVar16 != 0);
              TVar15 = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&piStack_60,
                                  (TH_LEGACY_LPVOID)0x0);
              if (TVar15 == 0) {
                (**(code **)(*piStack_60 + 8))(piStack_60);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
            uVar2 = pDVar1->dwRBitMask;
            uVar3 = pDVar1->dwGBitMask;
            uVar4 = pDVar1->dwBBitMask;
            if (((uVar2 != 0) && (uVar3 != 0)) && (uVar4 != 0)) {
              iStack_34 = 0;
              if (uVar2 != 0) {
                for (; (uVar2 >> iStack_34 & 1) == 0; iStack_34 = iStack_34 + 1) {
                }
              }
              iStack_38 = 0;
              if (uVar3 != 0) {
                for (; (uVar3 >> iStack_38 & 1) == 0; iStack_38 = iStack_38 + 1) {
                }
              }
              iStack_3c = 0;
              if (uVar4 != 0) {
                for (; (uVar4 >> iStack_3c & 1) == 0; iStack_3c = iStack_3c + 1) {
                }
              }
              iVar16 = 0x1f;
              if (pDVar1->dwRBitMask != 0) {
                for (; pDVar1->dwRBitMask >> iVar16 == 0; iVar16 = iVar16 + -1) {
                }
              }
              iVar18 = 0x1f;
              if (pDVar1->dwGBitMask != 0) {
                for (; pDVar1->dwGBitMask >> iVar18 == 0; iVar18 = iVar18 + -1) {
                }
              }
              iVar10 = 0x1f;
              if (pDVar1->dwBBitMask != 0) {
                for (; pDVar1->dwBBitMask >> iVar10 == 0; iVar10 = iVar10 + -1) {
                }
              }
              iStack_44 = 0x18 - ((iVar16 + 1) - iStack_34);
              iStack_48 = 0x10 - ((iVar18 + 1) - iStack_38);
              iStack_4c = 8 - ((iVar10 + 1) - iStack_3c);
              uVar2 = pDVar1->dwRGBAlphaBitMask;
              if (uVar2 == 0) {
                iStack_50 = 0;
                iStack_40 = 0x10;
              }
              else {
                iStack_40 = 0;
                if (uVar2 != 0) {
                  for (; (uVar2 >> iStack_40 & 1) == 0; iStack_40 = iStack_40 + 1) {
                  }
                }
                iVar16 = 0x1f;
                if (uVar2 != 0) {
                  for (; uVar2 >> iVar16 == 0; iVar16 = iVar16 + -1) {
                  }
                }
                iStack_50 = 0x20 - ((iVar16 + 1) - iStack_40);
              }
              iVar16 = iStack_54;
              puVar25 = g_SurfaceDesc.lpSurface;
              puVar23 = g_SurfaceDesc.lpSurface;
              puVar38 = g_SurfaceDesc.lpSurface;
              puVar37 = g_SurfaceDesc.lpSurface;
              if (pDVar1->dwRGBBitCount < 0x11) {
                do {
                  do {
                    uVar5 = *(undefined4 *)pAVar21->producerName;
                    uVar6 = *(undefined4 *)(pAVar21->producerName + 2);
                    uVar7 = *(undefined4 *)(pAVar21->producerName + iStack_54 * 2);
                    uVar8 = *(undefined4 *)(pAVar21->producerName + iStack_54 * 2 + 2);
                    uVar30 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar31 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar32 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar33 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar26 = (ushort)((ushort)(byte)uVar5 + (ushort)(byte)uVar6 +
                                     (ushort)(byte)uVar7 + (ushort)(byte)uVar8) >> 2;
                    uVar28 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    uVar29 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar30,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar31,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar32,(char)((uint)uVar7 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar33,(char)((uint)uVar8 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    uVar30 = (ushort)((uVar30 >> 8) + (uVar31 >> 8) + (uVar32 >> 8) + (uVar33 >> 8))
                             >> 2;
                    uVar27 = CONCAT12((uVar29 != 0) * (uVar29 < 0x100) * (char)uVar29 -
                                      (0xff < uVar29),
                                      CONCAT11((uVar28 != 0) * (uVar28 < 0x100) * (char)uVar28 -
                                               (0xff < uVar28),
                                               (uVar26 != 0) * (uVar26 < 0x100) * (char)uVar26 -
                                               (0xff < uVar26)));
                    *puVar23 = (ushort)((((uint)(byte)((uVar30 != 0) * (uVar30 < 0x100) *
                                                       (char)uVar30 - (0xff < uVar30)) << 0x18) >>
                                        ((byte)iStack_50 & 0x1f)) << ((byte)iStack_40 & 0x1f)) |
                               (ushort)(((uVar27 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                                       ((byte)iStack_3c & 0x1f)) |
                               (ushort)(((uVar27 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                                       ((byte)iStack_38 & 0x1f)) |
                               (ushort)(((uVar27 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                                       ((byte)iStack_34 & 0x1f));
                    pAVar21 = (AssetProducerSourceNames *)(pAVar21->producerName + 4);
                    iVar18 = iVar16 + -2;
                    bVar9 = 1 < iVar16;
                    iVar16 = iVar18;
                    puVar23 = puVar23 + 1;
                  } while (iVar18 != 0 && bVar9);
                  pAVar21 = (AssetProducerSourceNames *)(pAVar35->producerName + iStack_54 * 4);
                  iVar18 = iStack_58 + -2;
                  bVar9 = 1 < iStack_58;
                  iVar16 = iStack_54;
                  puVar23 = (ushort *)((int)puVar37 + TVar14);
                  pAVar35 = pAVar21;
                  puVar37 = (ushort *)((int)puVar37 + TVar14);
                  iStack_58 = iVar18;
                } while (iVar18 != 0 && bVar9);
              }
              else {
                do {
                  do {
                    uVar5 = *(undefined4 *)pAVar21->producerName;
                    uVar6 = *(undefined4 *)(pAVar21->producerName + 2);
                    uVar7 = *(undefined4 *)(pAVar21->producerName + iStack_54 * 2);
                    uVar8 = *(undefined4 *)(pAVar21->producerName + iStack_54 * 2 + 2);
                    uVar30 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar31 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar32 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar33 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar26 = (ushort)((ushort)(byte)uVar5 + (ushort)(byte)uVar6 +
                                     (ushort)(byte)uVar7 + (ushort)(byte)uVar8) >> 2;
                    uVar28 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    uVar29 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar30,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar31,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar32,(char)((uint)uVar7 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar33,(char)((uint)uVar8 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    uVar30 = (ushort)((uVar30 >> 8) + (uVar31 >> 8) + (uVar32 >> 8) + (uVar33 >> 8))
                             >> 2;
                    uVar27 = CONCAT12((uVar29 != 0) * (uVar29 < 0x100) * (char)uVar29 -
                                      (0xff < uVar29),
                                      CONCAT11((uVar28 != 0) * (uVar28 < 0x100) * (char)uVar28 -
                                               (0xff < uVar28),
                                               (uVar26 != 0) * (uVar26 < 0x100) * (char)uVar26 -
                                               (0xff < uVar26)));
                    *puVar25 = (((uint)(byte)((uVar30 != 0) * (uVar30 < 0x100) * (char)uVar30 -
                                             (0xff < uVar30)) << 0x18) >> ((byte)iStack_50 & 0x1f))
                               << ((byte)iStack_40 & 0x1f) |
                               ((uVar27 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                               ((byte)iStack_3c & 0x1f) |
                               ((uVar27 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                               ((byte)iStack_38 & 0x1f) |
                               ((uVar27 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                               ((byte)iStack_34 & 0x1f);
                    pAVar21 = (AssetProducerSourceNames *)(pAVar21->producerName + 4);
                    iVar18 = iVar16 + -2;
                    bVar9 = 1 < iVar16;
                    iVar16 = iVar18;
                    puVar25 = puVar25 + 1;
                  } while (iVar18 != 0 && bVar9);
                  pAVar21 = (AssetProducerSourceNames *)(pAVar35->producerName + iStack_54 * 4);
                  iVar18 = iStack_58 + -2;
                  bVar9 = 1 < iStack_58;
                  iVar16 = iStack_54;
                  puVar25 = (uint *)((int)puVar38 + TVar14);
                  pAVar35 = pAVar21;
                  puVar38 = (uint *)((int)puVar38 + TVar14);
                  iStack_58 = iVar18;
                } while (iVar18 != 0 && bVar9);
              }
            }
          }
        }
        else {
          pbVar19 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    iVar18 + -0x28;
          pGStack_5c = sourceAsset + iVar16 * 4 + 1;
          if ((iStack_54 != 0) && (pDVar1 = texture->pixelFormat, iStack_58 != 0)) {
            iVar16 = iStack_54;
            pbVar22 = g_SurfaceDesc.lpSurface;
            pbVar34 = pbVar19;
            pbVar36 = g_SurfaceDesc.lpSurface;
            if (pDVar1->dwRGBBitCount == 8) {
              do {
                do {
                  *pbVar22 = *pbVar19;
                  pbVar19 = pbVar19 + 2;
                  iVar18 = iVar16 + -2;
                  bVar9 = 1 < iVar16;
                  iVar16 = iVar18;
                  pbVar22 = pbVar22 + 1;
                } while (iVar18 != 0 && bVar9);
                pbVar19 = pbVar34 + iStack_54 * 2;
                iVar18 = iStack_58 + -2;
                bVar9 = 1 < iStack_58;
                iVar16 = iStack_54;
                pbVar22 = pbVar36 + TVar14;
                pbVar34 = pbVar19;
                pbVar36 = pbVar36 + TVar14;
                iStack_58 = iVar18;
              } while (iVar18 != 0 && bVar9);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              iVar16 = 0x100;
              pGVar20 = pGStack_5c;
              pDVar24 = g_TexturePaletteEntries;
              do {
                uVar11 = *(undefined1 *)((int)&(pGVar20->common).magic + 1);
                uVar12 = *(undefined1 *)((int)&(pGVar20->common).magic + 2);
                uVar13 = *(undefined1 *)((int)&(pGVar20->common).magic + 3);
                pDVar24->red = *(undefined1 *)&(pGVar20->common).magic;
                pDVar24->green = uVar11;
                pDVar24->blue = uVar12;
                pDVar24->flags = uVar13;
                pGVar20 = (GraphicsTextureSourceAsset *)&(pGVar20->common).formatVersion;
                pDVar24 = pDVar24 + 1;
                iVar16 = iVar16 + -1;
              } while (iVar16 != 0);
              TVar15 = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&piStack_60,
                                  (TH_LEGACY_LPVOID)0x0);
              if (TVar15 == 0) {
                (**(code **)(*piStack_60 + 8))(piStack_60);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
            uVar2 = pDVar1->dwRBitMask;
            uVar3 = pDVar1->dwGBitMask;
            uVar4 = pDVar1->dwBBitMask;
            if (((uVar2 != 0) && (uVar3 != 0)) && (uVar4 != 0)) {
              iStack_34 = 0;
              if (uVar2 != 0) {
                for (; (uVar2 >> iStack_34 & 1) == 0; iStack_34 = iStack_34 + 1) {
                }
              }
              iStack_38 = 0;
              if (uVar3 != 0) {
                for (; (uVar3 >> iStack_38 & 1) == 0; iStack_38 = iStack_38 + 1) {
                }
              }
              iStack_3c = 0;
              if (uVar4 != 0) {
                for (; (uVar4 >> iStack_3c & 1) == 0; iStack_3c = iStack_3c + 1) {
                }
              }
              iVar16 = 0x1f;
              if (pDVar1->dwRBitMask != 0) {
                for (; pDVar1->dwRBitMask >> iVar16 == 0; iVar16 = iVar16 + -1) {
                }
              }
              iVar18 = 0x1f;
              if (pDVar1->dwGBitMask != 0) {
                for (; pDVar1->dwGBitMask >> iVar18 == 0; iVar18 = iVar18 + -1) {
                }
              }
              iVar10 = 0x1f;
              if (pDVar1->dwBBitMask != 0) {
                for (; pDVar1->dwBBitMask >> iVar10 == 0; iVar10 = iVar10 + -1) {
                }
              }
              iStack_44 = 0x18 - ((iVar16 + 1) - iStack_34);
              iStack_48 = 0x10 - ((iVar18 + 1) - iStack_38);
              iStack_4c = 8 - ((iVar10 + 1) - iStack_3c);
              uVar2 = pDVar1->dwRGBAlphaBitMask;
              if (uVar2 == 0) {
                iStack_50 = 0;
                iStack_40 = 0x10;
              }
              else {
                iStack_40 = 0;
                if (uVar2 != 0) {
                  for (; (uVar2 >> iStack_40 & 1) == 0; iStack_40 = iStack_40 + 1) {
                  }
                }
                iVar16 = 0x1f;
                if (uVar2 != 0) {
                  for (; uVar2 >> iVar16 == 0; iVar16 = iVar16 + -1) {
                  }
                }
                iStack_50 = 0x20 - ((iVar16 + 1) - iStack_40);
              }
              iVar16 = iStack_54;
              puVar25 = g_SurfaceDesc.lpSurface;
              puVar23 = g_SurfaceDesc.lpSurface;
              pbVar22 = pbVar19;
              puVar38 = g_SurfaceDesc.lpSurface;
              puVar37 = g_SurfaceDesc.lpSurface;
              if (pDVar1->dwRGBBitCount < 0x11) {
                do {
                  do {
                    uVar5 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*pbVar19 * 8 + -0x28);
                    uVar6 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar19[1] * 8 + -0x28);
                    uVar7 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar19[iStack_54] * 8 + -0x28);
                    uVar8 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar19[iStack_54 + 1] * 8 + -0x28);
                    uVar30 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar31 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar32 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar33 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar26 = (ushort)((ushort)(byte)uVar5 + (ushort)(byte)uVar6 +
                                     (ushort)(byte)uVar7 + (ushort)(byte)uVar8) >> 2;
                    uVar28 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    uVar29 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar30,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar31,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar32,(char)((uint)uVar7 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar33,(char)((uint)uVar8 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    uVar30 = (ushort)((uVar30 >> 8) + (uVar31 >> 8) + (uVar32 >> 8) + (uVar33 >> 8))
                             >> 2;
                    uVar27 = CONCAT12((uVar29 != 0) * (uVar29 < 0x100) * (char)uVar29 -
                                      (0xff < uVar29),
                                      CONCAT11((uVar28 != 0) * (uVar28 < 0x100) * (char)uVar28 -
                                               (0xff < uVar28),
                                               (uVar26 != 0) * (uVar26 < 0x100) * (char)uVar26 -
                                               (0xff < uVar26)));
                    *puVar23 = (ushort)((((uint)(byte)((uVar30 != 0) * (uVar30 < 0x100) *
                                                       (char)uVar30 - (0xff < uVar30)) << 0x18) >>
                                        ((byte)iStack_50 & 0x1f)) << ((byte)iStack_40 & 0x1f)) |
                               (ushort)(((uVar27 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                                       ((byte)iStack_3c & 0x1f)) |
                               (ushort)(((uVar27 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                                       ((byte)iStack_38 & 0x1f)) |
                               (ushort)(((uVar27 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                                       ((byte)iStack_34 & 0x1f));
                    pbVar19 = pbVar19 + 2;
                    iVar18 = iVar16 + -2;
                    bVar9 = 1 < iVar16;
                    iVar16 = iVar18;
                    puVar23 = puVar23 + 1;
                  } while (iVar18 != 0 && bVar9);
                  pbVar19 = pbVar22 + iStack_54 * 2;
                  iVar18 = iStack_58 + -2;
                  bVar9 = 1 < iStack_58;
                  iVar16 = iStack_54;
                  puVar23 = (ushort *)((int)puVar37 + TVar14);
                  pbVar22 = pbVar19;
                  puVar37 = (ushort *)((int)puVar37 + TVar14);
                  iStack_58 = iVar18;
                } while (iVar18 != 0 && bVar9);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              }
              else {
                do {
                  do {
                    uVar5 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*pbVar19 * 8 + -0x28);
                    uVar6 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar19[1] * 8 + -0x28);
                    uVar7 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar19[iStack_54] * 8 + -0x28);
                    uVar8 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar19[iStack_54 + 1] * 8 + -0x28);
                    uVar30 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar31 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar32 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar33 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar26 = (ushort)((ushort)(byte)uVar5 + (ushort)(byte)uVar6 +
                                     (ushort)(byte)uVar7 + (ushort)(byte)uVar8) >> 2;
                    uVar28 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >> 0x10
                                              ) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >> 0x10
                                              ) >> 8)) >> 2;
                    uVar29 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar30,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar31,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar32,(char)((uint)uVar7 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar33,(char)((uint)uVar8 >> 0x10)) << 0x28
                                               ) >> 0x20) >> 8)) >> 2;
                    uVar30 = (ushort)((uVar30 >> 8) + (uVar31 >> 8) + (uVar32 >> 8) + (uVar33 >> 8))
                             >> 2;
                    uVar27 = CONCAT12((uVar29 != 0) * (uVar29 < 0x100) * (char)uVar29 -
                                      (0xff < uVar29),
                                      CONCAT11((uVar28 != 0) * (uVar28 < 0x100) * (char)uVar28 -
                                               (0xff < uVar28),
                                               (uVar26 != 0) * (uVar26 < 0x100) * (char)uVar26 -
                                               (0xff < uVar26)));
                    *puVar25 = (((uint)(byte)((uVar30 != 0) * (uVar30 < 0x100) * (char)uVar30 -
                                             (0xff < uVar30)) << 0x18) >> ((byte)iStack_50 & 0x1f))
                               << ((byte)iStack_40 & 0x1f) |
                               ((uVar27 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                               ((byte)iStack_3c & 0x1f) |
                               ((uVar27 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                               ((byte)iStack_38 & 0x1f) |
                               ((uVar27 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                               ((byte)iStack_34 & 0x1f);
                    pbVar19 = pbVar19 + 2;
                    iVar18 = iVar16 + -2;
                    bVar9 = 1 < iVar16;
                    iVar16 = iVar18;
                    puVar25 = puVar25 + 1;
                  } while (iVar18 != 0 && bVar9);
                  pbVar19 = pbVar22 + iStack_54 * 2;
                  iVar18 = iStack_58 + -2;
                  bVar9 = 1 < iStack_58;
                  iVar16 = iStack_54;
                  puVar25 = (uint *)((int)puVar38 + TVar14);
                  pbVar22 = pbVar19;
                  puVar38 = (uint *)((int)puVar38 + TVar14);
                  iStack_58 = iVar18;
                } while (iVar18 != 0 && bVar9);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              }
              goto GraphicsTextureUploadColor2x_DecrementActiveCountAndReturn;
            }
          }
        }
        (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
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
  int iVar1;
  DDPIXELFORMAT *pDVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  bool bVar20;
  int iVar21;
  undefined1 uVar22;
  undefined1 uVar23;
  undefined1 uVar24;
  TH_LEGACY_LONG TVar25;
  TH_LEGACY_LPVOID arg1;
  TH_LEGACY_HRESULT TVar26;
  int iVar27;
  DirectDrawPaletteEntry DVar28;
  uint uVar29;
  uint uVar30;
  byte *pbVar31;
  GraphicsTextureSourceAsset *pGVar32;
  word *pwVar33;
  byte *pbVar34;
  ushort *puVar35;
  DirectDrawPaletteEntry *pDVar36;
  uint *puVar37;
  ushort uVar38;
  uint3 uVar39;
  ushort uVar40;
  ushort uVar41;
  ushort uVar42;
  ushort uVar43;
  ushort uVar44;
  ushort uVar45;
  ushort uVar46;
  ushort uVar47;
  ushort uVar48;
  ushort uVar49;
  ushort uVar50;
  ushort uVar51;
  ushort uVar52;
  ushort uVar53;
  ushort uVar54;
  ushort uVar55;
  ushort uVar56;
  ushort uVar57;
  byte *pbVar58;
  word *pwVar59;
  byte *pbVar60;
  ushort *puVar61;
  uint *puVar62;
  int *piStack_60;
  GraphicsTextureSourceAsset *pGStack_5c;
  int iStack_58;
  uint uStack_54;
  int iStack_50;
  int iStack_4c;
  int iStack_48;
  int iStack_44;
  int iStack_40;
  int iStack_3c;
  int iStack_38;
  int iStack_34;
  TH_LEGACY_LONG TStack_30;
  TH_LEGACY_LPVOID pvStack_2c;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  stagingSurface3 = texture->stagingSurface3;
  sourceAsset = texture->sourceAsset;
  subresourceIndex = texture->subresourceIndex;
  if (stagingSurface3 != (IDirectDrawSurface3 *)0x0) {
    TVar26 = (*stagingSurface3->lpVtbl->IsLost)(stagingSurface3);
    iVar27 = 0;
    if (TVar26 != 0) {
      iVar27 = (*stagingSurface3->lpVtbl->Restore)(stagingSurface3);
    }
    if (iVar27 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar26 = (*stagingSurface3->lpVtbl->Lock)
                         (stagingSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,
                          (TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      TVar25 = g_SurfaceDesc.lPitch;
      if (TVar26 == 0) {
        iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
        TStack_30 = g_SurfaceDesc.lPitch;
        pvStack_2c = g_SurfaceDesc.lpSurface;
        iVar27 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                         iVar1 + -0x20);
        uStack_54 = *(uint *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                             iVar1 + -0x10);
        iStack_58 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar1 + -0xc);
        iVar1 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar1 + -0x1c);
        if (iVar27 < 0) {
          pwVar33 = (word *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar1 + -0x28);
          if ((uStack_54 != 0) && (pDVar2 = texture->pixelFormat, iStack_58 != 0)) {
            uVar30 = uStack_54;
            pbVar31 = g_SurfaceDesc.lpSurface;
            pwVar59 = pwVar33;
            pbVar34 = g_SurfaceDesc.lpSurface;
            if (pDVar2->dwRGBBitCount == 8) {
              do {
                do {
                  *pbVar31 = *(byte *)((int)pwVar33 + 3);
                  pwVar33 = pwVar33 + 8;
                  uVar29 = uVar30 - 4;
                  bVar20 = 3 < (int)uVar30;
                  uVar30 = uVar29;
                  pbVar31 = pbVar31 + 1;
                } while (uVar29 != 0 && bVar20);
                pwVar33 = pwVar59 + uStack_54 * 8;
                iVar27 = iStack_58 + -4;
                bVar20 = 3 < iStack_58;
                uVar30 = uStack_54 & 0xfffffff;
                pbVar31 = pbVar34 + TVar25;
                pwVar59 = pwVar33;
                pbVar34 = pbVar34 + TVar25;
                iStack_58 = iVar27;
              } while (iVar27 != 0 && bVar20);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              iVar27 = 0x100;
              DVar28.red = 0;
              DVar28.green = 0;
              DVar28.blue = 0;
              DVar28.flags = 0;
              pDVar36 = g_TexturePaletteEntries;
              do {
                *pDVar36 = DVar28;
                pDVar36 = pDVar36 + 1;
                DVar28 = (DirectDrawPaletteEntry)((int)DVar28 + 0x1010101);
                iVar27 = iVar27 + -1;
              } while (iVar27 != 0);
              TVar26 = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&piStack_60,
                                  (TH_LEGACY_LPVOID)0x0);
              if (TVar26 == 0) {
                (**(code **)(*piStack_60 + 8))(piStack_60);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
            uVar30 = pDVar2->dwRBitMask;
            uVar29 = pDVar2->dwGBitMask;
            uVar3 = pDVar2->dwBBitMask;
            if (((uVar30 != 0) && (uVar29 != 0)) && (uVar3 != 0)) {
              iStack_34 = 0;
              if (uVar30 != 0) {
                for (; (uVar30 >> iStack_34 & 1) == 0; iStack_34 = iStack_34 + 1) {
                }
              }
              iStack_38 = 0;
              if (uVar29 != 0) {
                for (; (uVar29 >> iStack_38 & 1) == 0; iStack_38 = iStack_38 + 1) {
                }
              }
              iStack_3c = 0;
              if (uVar3 != 0) {
                for (; (uVar3 >> iStack_3c & 1) == 0; iStack_3c = iStack_3c + 1) {
                }
              }
              iVar27 = 0x1f;
              if (pDVar2->dwRBitMask != 0) {
                for (; pDVar2->dwRBitMask >> iVar27 == 0; iVar27 = iVar27 + -1) {
                }
              }
              iVar1 = 0x1f;
              if (pDVar2->dwGBitMask != 0) {
                for (; pDVar2->dwGBitMask >> iVar1 == 0; iVar1 = iVar1 + -1) {
                }
              }
              iVar21 = 0x1f;
              if (pDVar2->dwBBitMask != 0) {
                for (; pDVar2->dwBBitMask >> iVar21 == 0; iVar21 = iVar21 + -1) {
                }
              }
              iStack_44 = 0x18 - ((iVar27 + 1) - iStack_34);
              iStack_48 = 0x10 - ((iVar1 + 1) - iStack_38);
              iStack_4c = 8 - ((iVar21 + 1) - iStack_3c);
              uVar30 = pDVar2->dwRGBAlphaBitMask;
              if (uVar30 == 0) {
                iStack_50 = 0;
                iStack_40 = 0x10;
              }
              else {
                iStack_40 = 0;
                if (uVar30 != 0) {
                  for (; (uVar30 >> iStack_40 & 1) == 0; iStack_40 = iStack_40 + 1) {
                  }
                }
                iVar27 = 0x1f;
                if (uVar30 != 0) {
                  for (; uVar30 >> iVar27 == 0; iVar27 = iVar27 + -1) {
                  }
                }
                iStack_50 = 0x20 - ((iVar27 + 1) - iStack_40);
              }
              uVar30 = uStack_54;
              puVar37 = g_SurfaceDesc.lpSurface;
              puVar35 = g_SurfaceDesc.lpSurface;
              puVar62 = g_SurfaceDesc.lpSurface;
              puVar61 = g_SurfaceDesc.lpSurface;
              if (pDVar2->dwRGBBitCount < 0x11) {
                do {
                  do {
                    uVar4 = *(undefined4 *)pwVar33;
                    uVar5 = *(undefined4 *)(pwVar33 + 2);
                    uVar6 = *(undefined4 *)(pwVar33 + uStack_54 * 2);
                    uVar7 = *(undefined4 *)(pwVar33 + (uStack_54 + 1) * 2);
                    uVar8 = *(undefined4 *)(pwVar33 + 4);
                    uVar9 = *(undefined4 *)(pwVar33 + 6);
                    uVar10 = *(undefined4 *)(pwVar33 + (uStack_54 + 2) * 2);
                    uVar11 = *(undefined4 *)(pwVar33 + (uStack_54 + 3) * 2);
                    uVar42 = (ushort)(((ulonglong)(byte)((uint)uVar4 >> 0x18) << 0x38) >> 0x30);
                    uVar43 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar44 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar45 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar46 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar49 = (ushort)(((ulonglong)(byte)((uint)uVar9 >> 0x18) << 0x38) >> 0x30);
                    uVar52 = (ushort)(((ulonglong)(byte)((uint)uVar10 >> 0x18) << 0x38) >> 0x30);
                    uVar55 = (ushort)(((ulonglong)(byte)((uint)uVar11 >> 0x18) << 0x38) >> 0x30);
                    pwVar33 = pwVar33 + uStack_54 * 4;
                    uVar12 = *(undefined4 *)pwVar33;
                    uVar13 = *(undefined4 *)(pwVar33 + 2);
                    uVar14 = *(undefined4 *)(pwVar33 + uStack_54 * 2);
                    uVar15 = *(undefined4 *)(pwVar33 + (uStack_54 + 1) * 2);
                    uVar47 = (ushort)(((ulonglong)(byte)((uint)uVar12 >> 0x18) << 0x38) >> 0x30);
                    uVar50 = (ushort)(((ulonglong)(byte)((uint)uVar13 >> 0x18) << 0x38) >> 0x30);
                    uVar53 = (ushort)(((ulonglong)(byte)((uint)uVar14 >> 0x18) << 0x38) >> 0x30);
                    uVar56 = (ushort)(((ulonglong)(byte)((uint)uVar15 >> 0x18) << 0x38) >> 0x30);
                    uVar16 = *(undefined4 *)(pwVar33 + 4);
                    uVar17 = *(undefined4 *)(pwVar33 + 6);
                    uVar18 = *(undefined4 *)(pwVar33 + (uStack_54 + 2) * 2);
                    uVar19 = *(undefined4 *)(pwVar33 + (uStack_54 + 3) * 2);
                    uVar48 = (ushort)(((ulonglong)(byte)((uint)uVar16 >> 0x18) << 0x38) >> 0x30);
                    uVar51 = (ushort)(((ulonglong)(byte)((uint)uVar17 >> 0x18) << 0x38) >> 0x30);
                    uVar54 = (ushort)(((ulonglong)(byte)((uint)uVar18 >> 0x18) << 0x38) >> 0x30);
                    uVar57 = (ushort)(((ulonglong)(byte)((uint)uVar19 >> 0x18) << 0x38) >> 0x30);
                    uVar38 = (ushort)((ushort)(byte)uVar4 + (ushort)(byte)uVar5 +
                                      (ushort)(byte)uVar6 + (ushort)(byte)uVar7 +
                                      (ushort)(byte)uVar8 + (ushort)(byte)uVar9 +
                                      (ushort)(byte)uVar10 + (ushort)(byte)uVar11 +
                                      (ushort)(byte)uVar12 + (ushort)(byte)uVar13 +
                                      (ushort)(byte)uVar14 + (ushort)(byte)uVar15 +
                                     (ushort)(byte)uVar16 + (ushort)(byte)uVar17 +
                                     (ushort)(byte)uVar18 + (ushort)(byte)uVar19) >> 4;
                    uVar40 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar4 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar9 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar12 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar13 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar14 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar15 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar16 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar17 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar18 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar19 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    uVar41 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar42,(char)((uint)uVar4 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar43,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar44,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar45,(char)((uint)uVar7 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar46,(char)((uint)uVar8 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar49,(char)((uint)uVar9 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar52,(char)((uint)uVar10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar55,(char)((uint)uVar11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar47,(char)((uint)uVar12 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar50,(char)((uint)uVar13 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar53,(char)((uint)uVar14 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar56,(char)((uint)uVar15 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar48,(char)((uint)uVar16 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar51,(char)((uint)uVar17 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar54,(char)((uint)uVar18 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar57,(char)((uint)uVar19 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    uVar42 = (ushort)((uVar42 >> 8) + (uVar43 >> 8) + (uVar44 >> 8) + (uVar45 >> 8)
                                      + (uVar46 >> 8) + (uVar49 >> 8) +
                                        (uVar52 >> 8) + (uVar55 >> 8) +
                                      (uVar47 >> 8) + (uVar50 >> 8) + (uVar53 >> 8) + (uVar56 >> 8)
                                     + (uVar48 >> 8) + (uVar51 >> 8) + (uVar54 >> 8) + (uVar57 >> 8)
                                     ) >> 4;
                    uVar39 = CONCAT12((uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 -
                                      (0xff < uVar41),
                                      CONCAT11((uVar40 != 0) * (uVar40 < 0x100) * (char)uVar40 -
                                               (0xff < uVar40),
                                               (uVar38 != 0) * (uVar38 < 0x100) * (char)uVar38 -
                                               (0xff < uVar38)));
                    *puVar35 = (ushort)((((uint)(byte)((uVar42 != 0) * (uVar42 < 0x100) *
                                                       (char)uVar42 - (0xff < uVar42)) << 0x18) >>
                                        ((byte)iStack_50 & 0x1f)) << ((byte)iStack_40 & 0x1f)) |
                               (ushort)(((uVar39 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                                       ((byte)iStack_3c & 0x1f)) |
                               (ushort)(((uVar39 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                                       ((byte)iStack_38 & 0x1f)) |
                               (ushort)(((uVar39 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                                       ((byte)iStack_34 & 0x1f));
                    pwVar33 = pwVar33 + (uStack_54 * -2 + 4) * 2;
                    uVar29 = uVar30 - 4;
                    bVar20 = 3 < (int)uVar30;
                    uVar30 = uVar29;
                    puVar35 = puVar35 + 1;
                  } while (uVar29 != 0 && bVar20);
                  pwVar33 = pwVar59 + uStack_54 * 8;
                  iVar27 = iStack_58 + -4;
                  bVar20 = 3 < iStack_58;
                  uVar30 = uStack_54 & 0xfffffff;
                  puVar35 = (ushort *)((int)puVar61 + TVar25);
                  pwVar59 = pwVar33;
                  puVar61 = (ushort *)((int)puVar61 + TVar25);
                  iStack_58 = iVar27;
                } while (iVar27 != 0 && bVar20);
              }
              else {
                do {
                  do {
                    uVar4 = *(undefined4 *)pwVar33;
                    uVar5 = *(undefined4 *)(pwVar33 + 2);
                    uVar6 = *(undefined4 *)(pwVar33 + uStack_54 * 2);
                    uVar7 = *(undefined4 *)(pwVar33 + (uStack_54 + 1) * 2);
                    uVar8 = *(undefined4 *)(pwVar33 + 4);
                    uVar9 = *(undefined4 *)(pwVar33 + 6);
                    uVar10 = *(undefined4 *)(pwVar33 + (uStack_54 + 2) * 2);
                    uVar11 = *(undefined4 *)(pwVar33 + (uStack_54 + 3) * 2);
                    uVar42 = (ushort)(((ulonglong)(byte)((uint)uVar4 >> 0x18) << 0x38) >> 0x30);
                    uVar43 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar44 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar45 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar46 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar49 = (ushort)(((ulonglong)(byte)((uint)uVar9 >> 0x18) << 0x38) >> 0x30);
                    uVar52 = (ushort)(((ulonglong)(byte)((uint)uVar10 >> 0x18) << 0x38) >> 0x30);
                    uVar55 = (ushort)(((ulonglong)(byte)((uint)uVar11 >> 0x18) << 0x38) >> 0x30);
                    pwVar33 = pwVar33 + uStack_54 * 4;
                    uVar12 = *(undefined4 *)pwVar33;
                    uVar13 = *(undefined4 *)(pwVar33 + 2);
                    uVar14 = *(undefined4 *)(pwVar33 + uStack_54 * 2);
                    uVar15 = *(undefined4 *)(pwVar33 + (uStack_54 + 1) * 2);
                    uVar47 = (ushort)(((ulonglong)(byte)((uint)uVar12 >> 0x18) << 0x38) >> 0x30);
                    uVar50 = (ushort)(((ulonglong)(byte)((uint)uVar13 >> 0x18) << 0x38) >> 0x30);
                    uVar53 = (ushort)(((ulonglong)(byte)((uint)uVar14 >> 0x18) << 0x38) >> 0x30);
                    uVar56 = (ushort)(((ulonglong)(byte)((uint)uVar15 >> 0x18) << 0x38) >> 0x30);
                    uVar16 = *(undefined4 *)(pwVar33 + 4);
                    uVar17 = *(undefined4 *)(pwVar33 + 6);
                    uVar18 = *(undefined4 *)(pwVar33 + (uStack_54 + 2) * 2);
                    uVar19 = *(undefined4 *)(pwVar33 + (uStack_54 + 3) * 2);
                    uVar48 = (ushort)(((ulonglong)(byte)((uint)uVar16 >> 0x18) << 0x38) >> 0x30);
                    uVar51 = (ushort)(((ulonglong)(byte)((uint)uVar17 >> 0x18) << 0x38) >> 0x30);
                    uVar54 = (ushort)(((ulonglong)(byte)((uint)uVar18 >> 0x18) << 0x38) >> 0x30);
                    uVar57 = (ushort)(((ulonglong)(byte)((uint)uVar19 >> 0x18) << 0x38) >> 0x30);
                    uVar38 = (ushort)((ushort)(byte)uVar4 + (ushort)(byte)uVar5 +
                                      (ushort)(byte)uVar6 + (ushort)(byte)uVar7 +
                                      (ushort)(byte)uVar8 + (ushort)(byte)uVar9 +
                                      (ushort)(byte)uVar10 + (ushort)(byte)uVar11 +
                                      (ushort)(byte)uVar12 + (ushort)(byte)uVar13 +
                                      (ushort)(byte)uVar14 + (ushort)(byte)uVar15 +
                                     (ushort)(byte)uVar16 + (ushort)(byte)uVar17 +
                                     (ushort)(byte)uVar18 + (ushort)(byte)uVar19) >> 4;
                    uVar40 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar4 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar9 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar12 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar13 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar14 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar15 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar16 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar17 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar18 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar19 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    uVar41 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar42,(char)((uint)uVar4 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar43,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar44,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar45,(char)((uint)uVar7 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar46,(char)((uint)uVar8 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar49,(char)((uint)uVar9 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar52,(char)((uint)uVar10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar55,(char)((uint)uVar11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar47,(char)((uint)uVar12 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar50,(char)((uint)uVar13 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar53,(char)((uint)uVar14 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar56,(char)((uint)uVar15 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar48,(char)((uint)uVar16 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar51,(char)((uint)uVar17 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar54,(char)((uint)uVar18 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar57,(char)((uint)uVar19 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    uVar42 = (ushort)((uVar42 >> 8) + (uVar43 >> 8) + (uVar44 >> 8) + (uVar45 >> 8)
                                      + (uVar46 >> 8) + (uVar49 >> 8) +
                                        (uVar52 >> 8) + (uVar55 >> 8) +
                                      (uVar47 >> 8) + (uVar50 >> 8) + (uVar53 >> 8) + (uVar56 >> 8)
                                     + (uVar48 >> 8) + (uVar51 >> 8) + (uVar54 >> 8) + (uVar57 >> 8)
                                     ) >> 4;
                    uVar39 = CONCAT12((uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 -
                                      (0xff < uVar41),
                                      CONCAT11((uVar40 != 0) * (uVar40 < 0x100) * (char)uVar40 -
                                               (0xff < uVar40),
                                               (uVar38 != 0) * (uVar38 < 0x100) * (char)uVar38 -
                                               (0xff < uVar38)));
                    *puVar37 = (((uint)(byte)((uVar42 != 0) * (uVar42 < 0x100) * (char)uVar42 -
                                             (0xff < uVar42)) << 0x18) >> ((byte)iStack_50 & 0x1f))
                               << ((byte)iStack_40 & 0x1f) |
                               ((uVar39 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                               ((byte)iStack_3c & 0x1f) |
                               ((uVar39 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                               ((byte)iStack_38 & 0x1f) |
                               ((uVar39 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                               ((byte)iStack_34 & 0x1f);
                    pwVar33 = pwVar33 + (uStack_54 * -2 + 4) * 2;
                    uVar29 = uVar30 - 4;
                    bVar20 = 3 < (int)uVar30;
                    uVar30 = uVar29;
                    puVar37 = puVar37 + 1;
                  } while (uVar29 != 0 && bVar20);
                  pwVar33 = pwVar59 + uStack_54 * 8;
                  iVar27 = iStack_58 + -4;
                  bVar20 = 3 < iStack_58;
                  uVar30 = uStack_54 & 0xfffffff;
                  puVar37 = (uint *)((int)puVar62 + TVar25);
                  pwVar59 = pwVar33;
                  puVar62 = (uint *)((int)puVar62 + TVar25);
                  iStack_58 = iVar27;
                } while (iVar27 != 0 && bVar20);
              }
            }
          }
        }
        else {
          pbVar31 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x28
          ;
          pGStack_5c = sourceAsset + iVar27 * 4 + 1;
          if ((uStack_54 != 0) && (pDVar2 = texture->pixelFormat, iStack_58 != 0)) {
            uVar30 = uStack_54;
            pbVar34 = g_SurfaceDesc.lpSurface;
            pbVar58 = pbVar31;
            pbVar60 = g_SurfaceDesc.lpSurface;
            if (pDVar2->dwRGBBitCount == 8) {
              do {
                do {
                  *pbVar34 = *pbVar31;
                  pbVar31 = pbVar31 + 4;
                  uVar29 = uVar30 - 4;
                  bVar20 = 3 < (int)uVar30;
                  uVar30 = uVar29;
                  pbVar34 = pbVar34 + 1;
                } while (uVar29 != 0 && bVar20);
                pbVar31 = pbVar58 + uStack_54 * 4;
                iVar27 = iStack_58 + -4;
                bVar20 = 3 < iStack_58;
                uVar30 = uStack_54;
                pbVar34 = pbVar60 + TVar25;
                pbVar58 = pbVar31;
                pbVar60 = pbVar60 + TVar25;
                iStack_58 = iVar27;
              } while (iVar27 != 0 && bVar20);
              (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              iVar27 = 0x100;
              pGVar32 = pGStack_5c;
              pDVar36 = g_TexturePaletteEntries;
              do {
                uVar22 = *(undefined1 *)((int)&(pGVar32->common).magic + 1);
                uVar23 = *(undefined1 *)((int)&(pGVar32->common).magic + 2);
                uVar24 = *(undefined1 *)((int)&(pGVar32->common).magic + 3);
                pDVar36->red = *(undefined1 *)&(pGVar32->common).magic;
                pDVar36->green = uVar22;
                pDVar36->blue = uVar23;
                pDVar36->flags = uVar24;
                pGVar32 = (GraphicsTextureSourceAsset *)&(pGVar32->common).formatVersion;
                pDVar36 = pDVar36 + 1;
                iVar27 = iVar27 + -1;
              } while (iVar27 != 0);
              TVar26 = (*g_DirectDraw2->lpVtbl->CreatePalette)
                                 (g_DirectDraw2,0x44,g_TexturePaletteEntries,&piStack_60,
                                  (TH_LEGACY_LPVOID)0x0);
              if (TVar26 == 0) {
                (**(code **)(*piStack_60 + 8))(piStack_60);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
            uVar30 = pDVar2->dwRBitMask;
            uVar29 = pDVar2->dwGBitMask;
            uVar3 = pDVar2->dwBBitMask;
            if (((uVar30 != 0) && (uVar29 != 0)) && (uVar3 != 0)) {
              iStack_34 = 0;
              if (uVar30 != 0) {
                for (; (uVar30 >> iStack_34 & 1) == 0; iStack_34 = iStack_34 + 1) {
                }
              }
              iStack_38 = 0;
              if (uVar29 != 0) {
                for (; (uVar29 >> iStack_38 & 1) == 0; iStack_38 = iStack_38 + 1) {
                }
              }
              iStack_3c = 0;
              if (uVar3 != 0) {
                for (; (uVar3 >> iStack_3c & 1) == 0; iStack_3c = iStack_3c + 1) {
                }
              }
              iVar27 = 0x1f;
              if (pDVar2->dwRBitMask != 0) {
                for (; pDVar2->dwRBitMask >> iVar27 == 0; iVar27 = iVar27 + -1) {
                }
              }
              iVar1 = 0x1f;
              if (pDVar2->dwGBitMask != 0) {
                for (; pDVar2->dwGBitMask >> iVar1 == 0; iVar1 = iVar1 + -1) {
                }
              }
              iVar21 = 0x1f;
              if (pDVar2->dwBBitMask != 0) {
                for (; pDVar2->dwBBitMask >> iVar21 == 0; iVar21 = iVar21 + -1) {
                }
              }
              iStack_44 = 0x18 - ((iVar27 + 1) - iStack_34);
              iStack_48 = 0x10 - ((iVar1 + 1) - iStack_38);
              iStack_4c = 8 - ((iVar21 + 1) - iStack_3c);
              uVar30 = pDVar2->dwRGBAlphaBitMask;
              if (uVar30 == 0) {
                iStack_50 = 0;
                iStack_40 = 0x10;
              }
              else {
                iStack_40 = 0;
                if (uVar30 != 0) {
                  for (; (uVar30 >> iStack_40 & 1) == 0; iStack_40 = iStack_40 + 1) {
                  }
                }
                iVar27 = 0x1f;
                if (uVar30 != 0) {
                  for (; uVar30 >> iVar27 == 0; iVar27 = iVar27 + -1) {
                  }
                }
                iStack_50 = 0x20 - ((iVar27 + 1) - iStack_40);
              }
              uVar30 = uStack_54;
              puVar37 = g_SurfaceDesc.lpSurface;
              puVar35 = g_SurfaceDesc.lpSurface;
              pbVar34 = pbVar31;
              puVar62 = g_SurfaceDesc.lpSurface;
              puVar61 = g_SurfaceDesc.lpSurface;
              if (pDVar2->dwRGBBitCount < 0x11) {
                do {
                  do {
                    uVar4 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*pbVar31 * 8 + -0x28);
                    uVar5 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[1] * 8 + -0x28);
                    uVar6 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[uStack_54] * 8 + -0x28);
                    uVar7 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[uStack_54 + 1] * 8 + -0x28);
                    uVar8 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[2] * 8 + -0x28);
                    uVar9 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[3] * 8 + -0x28);
                    uVar10 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 2] * 8 + -0x28);
                    uVar11 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 3] * 8 + -0x28);
                    uVar42 = (ushort)(((ulonglong)(byte)((uint)uVar4 >> 0x18) << 0x38) >> 0x30);
                    uVar43 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar44 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar45 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar46 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar49 = (ushort)(((ulonglong)(byte)((uint)uVar9 >> 0x18) << 0x38) >> 0x30);
                    uVar52 = (ushort)(((ulonglong)(byte)((uint)uVar10 >> 0x18) << 0x38) >> 0x30);
                    uVar55 = (ushort)(((ulonglong)(byte)((uint)uVar11 >> 0x18) << 0x38) >> 0x30);
                    pbVar31 = pbVar31 + uStack_54 * 2;
                    uVar12 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)*pbVar31 * 8 + -0x28);
                    uVar13 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[1] * 8 + -0x28);
                    uVar14 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54] * 8 + -0x28);
                    uVar15 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 1] * 8 + -0x28);
                    uVar47 = (ushort)(((ulonglong)(byte)((uint)uVar12 >> 0x18) << 0x38) >> 0x30);
                    uVar50 = (ushort)(((ulonglong)(byte)((uint)uVar13 >> 0x18) << 0x38) >> 0x30);
                    uVar53 = (ushort)(((ulonglong)(byte)((uint)uVar14 >> 0x18) << 0x38) >> 0x30);
                    uVar56 = (ushort)(((ulonglong)(byte)((uint)uVar15 >> 0x18) << 0x38) >> 0x30);
                    uVar16 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[2] * 8 + -0x28);
                    uVar17 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[3] * 8 + -0x28);
                    uVar18 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 2] * 8 + -0x28);
                    uVar19 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 3] * 8 + -0x28);
                    uVar48 = (ushort)(((ulonglong)(byte)((uint)uVar16 >> 0x18) << 0x38) >> 0x30);
                    uVar51 = (ushort)(((ulonglong)(byte)((uint)uVar17 >> 0x18) << 0x38) >> 0x30);
                    uVar54 = (ushort)(((ulonglong)(byte)((uint)uVar18 >> 0x18) << 0x38) >> 0x30);
                    uVar57 = (ushort)(((ulonglong)(byte)((uint)uVar19 >> 0x18) << 0x38) >> 0x30);
                    uVar38 = (ushort)((ushort)(byte)uVar4 + (ushort)(byte)uVar5 +
                                      (ushort)(byte)uVar6 + (ushort)(byte)uVar7 +
                                      (ushort)(byte)uVar8 + (ushort)(byte)uVar9 +
                                      (ushort)(byte)uVar10 + (ushort)(byte)uVar11 +
                                      (ushort)(byte)uVar12 + (ushort)(byte)uVar13 +
                                      (ushort)(byte)uVar14 + (ushort)(byte)uVar15 +
                                     (ushort)(byte)uVar16 + (ushort)(byte)uVar17 +
                                     (ushort)(byte)uVar18 + (ushort)(byte)uVar19) >> 4;
                    uVar40 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar4 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar9 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar12 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar13 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar14 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar15 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar16 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar17 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar18 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar19 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    uVar41 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar42,(char)((uint)uVar4 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar43,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar44,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar45,(char)((uint)uVar7 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar46,(char)((uint)uVar8 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar49,(char)((uint)uVar9 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar52,(char)((uint)uVar10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar55,(char)((uint)uVar11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar47,(char)((uint)uVar12 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar50,(char)((uint)uVar13 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar53,(char)((uint)uVar14 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar56,(char)((uint)uVar15 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar48,(char)((uint)uVar16 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar51,(char)((uint)uVar17 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar54,(char)((uint)uVar18 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar57,(char)((uint)uVar19 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    uVar42 = (ushort)((uVar42 >> 8) + (uVar43 >> 8) + (uVar44 >> 8) + (uVar45 >> 8)
                                      + (uVar46 >> 8) + (uVar49 >> 8) +
                                        (uVar52 >> 8) + (uVar55 >> 8) +
                                      (uVar47 >> 8) + (uVar50 >> 8) + (uVar53 >> 8) + (uVar56 >> 8)
                                     + (uVar48 >> 8) + (uVar51 >> 8) + (uVar54 >> 8) + (uVar57 >> 8)
                                     ) >> 4;
                    uVar39 = CONCAT12((uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 -
                                      (0xff < uVar41),
                                      CONCAT11((uVar40 != 0) * (uVar40 < 0x100) * (char)uVar40 -
                                               (0xff < uVar40),
                                               (uVar38 != 0) * (uVar38 < 0x100) * (char)uVar38 -
                                               (0xff < uVar38)));
                    *puVar35 = (ushort)((((uint)(byte)((uVar42 != 0) * (uVar42 < 0x100) *
                                                       (char)uVar42 - (0xff < uVar42)) << 0x18) >>
                                        ((byte)iStack_50 & 0x1f)) << ((byte)iStack_40 & 0x1f)) |
                               (ushort)(((uVar39 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                                       ((byte)iStack_3c & 0x1f)) |
                               (ushort)(((uVar39 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                                       ((byte)iStack_38 & 0x1f)) |
                               (ushort)(((uVar39 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                                       ((byte)iStack_34 & 0x1f));
                    pbVar31 = pbVar31 + uStack_54 * -2 + 4;
                    uVar29 = uVar30 - 4;
                    bVar20 = 3 < (int)uVar30;
                    uVar30 = uVar29;
                    puVar35 = puVar35 + 1;
                  } while (uVar29 != 0 && bVar20);
                  pbVar31 = pbVar34 + uStack_54 * 4;
                  iVar27 = iStack_58 + -4;
                  bVar20 = 3 < iStack_58;
                  uVar30 = uStack_54;
                  puVar35 = (ushort *)((int)puVar61 + TVar25);
                  pbVar34 = pbVar31;
                  puVar61 = (ushort *)((int)puVar61 + TVar25);
                  iStack_58 = iVar27;
                } while (iVar27 != 0 && bVar20);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              }
              else {
                do {
                  do {
                    uVar4 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)*pbVar31 * 8 + -0x28);
                    uVar5 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[1] * 8 + -0x28);
                    uVar6 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[uStack_54] * 8 + -0x28);
                    uVar7 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[uStack_54 + 1] * 8 + -0x28);
                    uVar8 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[2] * 8 + -0x28);
                    uVar9 = *(undefined4 *)
                             ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                             (uint)pbVar31[3] * 8 + -0x28);
                    uVar10 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 2] * 8 + -0x28);
                    uVar11 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 3] * 8 + -0x28);
                    uVar42 = (ushort)(((ulonglong)(byte)((uint)uVar4 >> 0x18) << 0x38) >> 0x30);
                    uVar43 = (ushort)(((ulonglong)(byte)((uint)uVar5 >> 0x18) << 0x38) >> 0x30);
                    uVar44 = (ushort)(((ulonglong)(byte)((uint)uVar6 >> 0x18) << 0x38) >> 0x30);
                    uVar45 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
                    uVar46 = (ushort)(((ulonglong)(byte)((uint)uVar8 >> 0x18) << 0x38) >> 0x30);
                    uVar49 = (ushort)(((ulonglong)(byte)((uint)uVar9 >> 0x18) << 0x38) >> 0x30);
                    uVar52 = (ushort)(((ulonglong)(byte)((uint)uVar10 >> 0x18) << 0x38) >> 0x30);
                    uVar55 = (ushort)(((ulonglong)(byte)((uint)uVar11 >> 0x18) << 0x38) >> 0x30);
                    pbVar31 = pbVar31 + uStack_54 * 2;
                    uVar12 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)*pbVar31 * 8 + -0x28);
                    uVar13 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[1] * 8 + -0x28);
                    uVar14 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54] * 8 + -0x28);
                    uVar15 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 1] * 8 + -0x28);
                    uVar47 = (ushort)(((ulonglong)(byte)((uint)uVar12 >> 0x18) << 0x38) >> 0x30);
                    uVar50 = (ushort)(((ulonglong)(byte)((uint)uVar13 >> 0x18) << 0x38) >> 0x30);
                    uVar53 = (ushort)(((ulonglong)(byte)((uint)uVar14 >> 0x18) << 0x38) >> 0x30);
                    uVar56 = (ushort)(((ulonglong)(byte)((uint)uVar15 >> 0x18) << 0x38) >> 0x30);
                    uVar16 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[2] * 8 + -0x28);
                    uVar17 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[3] * 8 + -0x28);
                    uVar18 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 2] * 8 + -0x28);
                    uVar19 = *(undefined4 *)
                              ((pGStack_5c->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (uint)pbVar31[uStack_54 + 3] * 8 + -0x28);
                    uVar48 = (ushort)(((ulonglong)(byte)((uint)uVar16 >> 0x18) << 0x38) >> 0x30);
                    uVar51 = (ushort)(((ulonglong)(byte)((uint)uVar17 >> 0x18) << 0x38) >> 0x30);
                    uVar54 = (ushort)(((ulonglong)(byte)((uint)uVar18 >> 0x18) << 0x38) >> 0x30);
                    uVar57 = (ushort)(((ulonglong)(byte)((uint)uVar19 >> 0x18) << 0x38) >> 0x30);
                    uVar38 = (ushort)((ushort)(byte)uVar4 + (ushort)(byte)uVar5 +
                                      (ushort)(byte)uVar6 + (ushort)(byte)uVar7 +
                                      (ushort)(byte)uVar8 + (ushort)(byte)uVar9 +
                                      (ushort)(byte)uVar10 + (ushort)(byte)uVar11 +
                                      (ushort)(byte)uVar12 + (ushort)(byte)uVar13 +
                                      (ushort)(byte)uVar14 + (ushort)(byte)uVar15 +
                                     (ushort)(byte)uVar16 + (ushort)(byte)uVar17 +
                                     (ushort)(byte)uVar18 + (ushort)(byte)uVar19) >> 4;
                    uVar40 = (ushort)(((ushort)(((ulonglong)(byte)((uint)uVar4 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar5 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar6 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar8 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar9 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar10 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar11 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar12 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar13 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar14 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                      ((ushort)(((ulonglong)(byte)((uint)uVar15 >> 8) << 0x18) >>
                                               0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar16 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar17 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar18 >> 8) << 0x18) >>
                                              0x10) >> 8) +
                                     ((ushort)(((ulonglong)(byte)((uint)uVar19 >> 8) << 0x18) >>
                                              0x10) >> 8)) >> 4;
                    uVar41 = (ushort)(((ushort)(((ulonglong)
                                                 CONCAT21(uVar42,(char)((uint)uVar4 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar43,(char)((uint)uVar5 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar44,(char)((uint)uVar6 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar45,(char)((uint)uVar7 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar46,(char)((uint)uVar8 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar49,(char)((uint)uVar9 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar52,(char)((uint)uVar10 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar55,(char)((uint)uVar11 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar47,(char)((uint)uVar12 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar50,(char)((uint)uVar13 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar53,(char)((uint)uVar14 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                      ((ushort)(((ulonglong)
                                                 CONCAT21(uVar56,(char)((uint)uVar15 >> 0x10)) <<
                                                0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar48,(char)((uint)uVar16 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar51,(char)((uint)uVar17 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar54,(char)((uint)uVar18 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8) +
                                     ((ushort)(((ulonglong)
                                                CONCAT21(uVar57,(char)((uint)uVar19 >> 0x10)) <<
                                               0x28) >> 0x20) >> 8)) >> 4;
                    uVar42 = (ushort)((uVar42 >> 8) + (uVar43 >> 8) + (uVar44 >> 8) + (uVar45 >> 8)
                                      + (uVar46 >> 8) + (uVar49 >> 8) +
                                        (uVar52 >> 8) + (uVar55 >> 8) +
                                      (uVar47 >> 8) + (uVar50 >> 8) + (uVar53 >> 8) + (uVar56 >> 8)
                                     + (uVar48 >> 8) + (uVar51 >> 8) + (uVar54 >> 8) + (uVar57 >> 8)
                                     ) >> 4;
                    uVar39 = CONCAT12((uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 -
                                      (0xff < uVar41),
                                      CONCAT11((uVar40 != 0) * (uVar40 < 0x100) * (char)uVar40 -
                                               (0xff < uVar40),
                                               (uVar38 != 0) * (uVar38 < 0x100) * (char)uVar38 -
                                               (0xff < uVar38)));
                    *puVar37 = (((uint)(byte)((uVar42 != 0) * (uVar42 < 0x100) * (char)uVar42 -
                                             (0xff < uVar42)) << 0x18) >> ((byte)iStack_50 & 0x1f))
                               << ((byte)iStack_40 & 0x1f) |
                               ((uVar39 & 0xff) >> ((byte)iStack_4c & 0x1f)) <<
                               ((byte)iStack_3c & 0x1f) |
                               ((uVar39 & 0xff00) >> ((byte)iStack_48 & 0x1f)) <<
                               ((byte)iStack_38 & 0x1f) |
                               ((uVar39 & 0xff0000) >> ((byte)iStack_44 & 0x1f)) <<
                               ((byte)iStack_34 & 0x1f);
                    pbVar31 = pbVar31 + uStack_54 * -2 + 4;
                    uVar29 = uVar30 - 4;
                    bVar20 = 3 < (int)uVar30;
                    uVar30 = uVar29;
                    puVar37 = puVar37 + 1;
                  } while (uVar29 != 0 && bVar20);
                  pbVar31 = pbVar34 + uStack_54 * 4;
                  iVar27 = iStack_58 + -4;
                  bVar20 = 3 < iStack_58;
                  uVar30 = uStack_54;
                  puVar37 = (uint *)((int)puVar62 + TVar25);
                  pbVar34 = pbVar31;
                  puVar62 = (uint *)((int)puVar62 + TVar25);
                  iStack_58 = iVar27;
                } while (iVar27 != 0 && bVar20);
                (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
              }
              goto GraphicsTextureUploadColor4x_DecrementActiveCountAndReturn;
            }
          }
        }
        (*stagingSurface3->lpVtbl->Unlock)(stagingSurface3,arg1);
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
  GraphicsTextureSourceAsset *pGVar1;
  GraphicsSubresourceIndex GVar2;
  DDPIXELFORMAT *pDVar3;
  byte bVar4;
  TH_LEGACY_LONG TVar5;
  TH_LEGACY_LPVOID arg1;
  TH_LEGACY_HRESULT TVar6;
  int iVar7;
  byte bVar8;
  uint uVar9;
  byte *pbVar10;
  ushort *puVar11;
  uint *puVar12;
  int iVar13;
  ushort *puVar14;
  uint *puVar15;
  int iStack_38;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  This = texture->stagingSurface3;
  pGVar1 = texture->sourceAsset;
  GVar2 = texture->subresourceIndex;
  if (This != (IDirectDrawSurface3 *)0x0) {
    TVar6 = (*This->lpVtbl->IsLost)(This);
    iVar7 = 0;
    if (TVar6 != 0) {
      iVar7 = (*This->lpVtbl->Restore)(This);
    }
    if (iVar7 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar6 = (*This->lpVtbl->Lock)
                        (This,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      TVar5 = g_SurfaceDesc.lPitch;
      if (TVar6 == 0) {
        iVar13 = GVar2 * 0x20 + (pGVar1->tableDescriptor).subresourceTableOffset;
        iVar7 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                        iVar13 + -0x10);
        iStack_38 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar13 + -0xc);
        pbVar10 = (pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar13 + -0x1c) + -0x28;
        if (((iVar7 != 0) && (pDVar3 = texture->pixelFormat, iStack_38 != 0)) &&
           (pDVar3->dwRGBBitCount != 8)) {
          iVar13 = 0x1f;
          if (pDVar3->dwRGBAlphaBitMask != 0) {
            for (; pDVar3->dwRGBAlphaBitMask >> iVar13 == 0; iVar13 = iVar13 + -1) {
            }
          }
          bVar8 = (char)iVar13 - 7;
          uVar9 = pDVar3->dwRBitMask | pDVar3->dwGBitMask | pDVar3->dwBBitMask;
          puVar12 = g_SurfaceDesc.lpSurface;
          puVar11 = g_SurfaceDesc.lpSurface;
          iVar13 = iVar7;
          puVar15 = g_SurfaceDesc.lpSurface;
          puVar14 = g_SurfaceDesc.lpSurface;
          if (pDVar3->dwRGBBitCount < 0x11) {
            do {
              do {
                bVar4 = bVar8 & 0x1f;
                *puVar11 = (ushort)*pbVar10 << bVar4 | (ushort)(*pbVar10 >> 0x20 - bVar4) |
                           (ushort)uVar9;
                pbVar10 = pbVar10 + 1;
                iVar13 = iVar13 + -1;
                puVar11 = puVar11 + 1;
              } while (iVar13 != 0);
              puVar11 = (ushort *)((int)puVar14 + TVar5);
              iStack_38 = iStack_38 + -1;
              iVar13 = iVar7;
              puVar14 = puVar11;
            } while (iStack_38 != 0);
            (*This->lpVtbl->Unlock)(This,arg1);
            goto GraphicsTextureUploadAlpha1x_DecrementActiveCountAndReturn;
          }
          do {
            do {
              bVar4 = bVar8 & 0x1f;
              *puVar12 = (uint)*pbVar10 << bVar4 | (uint)(*pbVar10 >> 0x20 - bVar4) | uVar9;
              pbVar10 = pbVar10 + 1;
              iVar13 = iVar13 + -1;
              puVar12 = (uint *)((int)puVar12 + 2);
            } while (iVar13 != 0);
            puVar12 = (uint *)((int)puVar15 + TVar5);
            iStack_38 = iStack_38 + -1;
            iVar13 = iVar7;
            puVar15 = puVar12;
          } while (iStack_38 != 0);
        }
        (*This->lpVtbl->Unlock)(This,arg1);
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
  int iVar1;
  IDirectDrawSurface3 *This;
  GraphicsTextureSourceAsset *pGVar2;
  GraphicsSubresourceIndex GVar3;
  DDPIXELFORMAT *pDVar4;
  bool bVar5;
  TH_LEGACY_LONG TVar6;
  TH_LEGACY_LPVOID arg1;
  byte bVar7;
  byte bVar8;
  ushort uVar9;
  TH_LEGACY_HRESULT TVar10;
  int iVar11;
  byte bVar12;
  uint uVar13;
  byte *pbVar14;
  ushort *puVar15;
  uint *puVar16;
  int iVar17;
  ushort *puVar18;
  uint *puVar19;
  int iStack_38;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  This = texture->stagingSurface3;
  pGVar2 = texture->sourceAsset;
  GVar3 = texture->subresourceIndex;
  if (This != (IDirectDrawSurface3 *)0x0) {
    TVar10 = (*This->lpVtbl->IsLost)(This);
    iVar11 = 0;
    if (TVar10 != 0) {
      iVar11 = (*This->lpVtbl->Restore)(This);
    }
    if (iVar11 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar10 = (*This->lpVtbl->Lock)
                         (This,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      TVar6 = g_SurfaceDesc.lPitch;
      if (TVar10 == 0) {
        iVar17 = GVar3 * 0x20 + (pGVar2->tableDescriptor).subresourceTableOffset;
        iVar11 = *(int *)((pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                         iVar17 + -0x10);
        iStack_38 = *(int *)((pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar17 + -0xc);
        pbVar14 = (pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar17 + -0x1c) + -0x28;
        if (((iVar11 != 0) && (pDVar4 = texture->pixelFormat, iStack_38 != 0)) &&
           (pDVar4->dwRGBBitCount != 8)) {
          iVar17 = 0x1f;
          if (pDVar4->dwRGBAlphaBitMask != 0) {
            for (; pDVar4->dwRGBAlphaBitMask >> iVar17 == 0; iVar17 = iVar17 + -1) {
            }
          }
          bVar12 = (char)iVar17 - 9;
          uVar13 = pDVar4->dwRBitMask | pDVar4->dwGBitMask | pDVar4->dwBBitMask;
          puVar16 = g_SurfaceDesc.lpSurface;
          puVar15 = g_SurfaceDesc.lpSurface;
          iVar17 = iVar11;
          puVar19 = g_SurfaceDesc.lpSurface;
          puVar18 = g_SurfaceDesc.lpSurface;
          if (pDVar4->dwRGBBitCount < 0x11) {
            do {
              do {
                bVar7 = *pbVar14 + pbVar14[1];
                bVar8 = bVar7 + pbVar14[iVar11];
                uVar9 = CONCAT11(CARRY1(*pbVar14,pbVar14[1]) + CARRY1(bVar7,pbVar14[iVar11]) +
                                 CARRY1(bVar8,pbVar14[iVar11 + 1]),bVar8 + pbVar14[iVar11 + 1]);
                bVar7 = bVar12 & 0x1f;
                *puVar15 = uVar9 << bVar7 | uVar9 >> 0x20 - bVar7 | (ushort)uVar13;
                pbVar14 = pbVar14 + 2;
                iVar1 = iVar17 + -2;
                bVar5 = 1 < iVar17;
                puVar15 = puVar15 + 1;
                iVar17 = iVar1;
              } while (iVar1 != 0 && bVar5);
              pbVar14 = pbVar14 + iVar11;
              puVar15 = (ushort *)((int)puVar18 + TVar6);
              iVar1 = iStack_38 + -2;
              bVar5 = 1 < iStack_38;
              iVar17 = iVar11;
              puVar18 = puVar15;
              iStack_38 = iVar1;
            } while (iVar1 != 0 && bVar5);
            (*This->lpVtbl->Unlock)(This,arg1);
            goto GraphicsTextureUploadAlpha2x_DecrementActiveCountAndReturn;
          }
          do {
            do {
              bVar7 = *pbVar14 + pbVar14[1];
              bVar8 = bVar7 + pbVar14[iVar11];
              uVar9 = CONCAT11(CARRY1(*pbVar14,pbVar14[1]) + CARRY1(bVar7,pbVar14[iVar11]) +
                               CARRY1(bVar8,pbVar14[iVar11 + 1]),bVar8 + pbVar14[iVar11 + 1]);
              bVar7 = bVar12 & 0x1f;
              *puVar16 = (uint)uVar9 << bVar7 | (uint)(uVar9 >> 0x20 - bVar7) | uVar13;
              pbVar14 = pbVar14 + 2;
              iVar1 = iVar17 + -2;
              bVar5 = 1 < iVar17;
              puVar16 = (uint *)((int)puVar16 + 2);
              iVar17 = iVar1;
            } while (iVar1 != 0 && bVar5);
            pbVar14 = pbVar14 + iVar11;
            puVar16 = (uint *)((int)puVar19 + TVar6);
            iVar1 = iStack_38 + -2;
            bVar5 = 1 < iStack_38;
            iVar17 = iVar11;
            puVar19 = puVar16;
            iStack_38 = iVar1;
          } while (iVar1 != 0 && bVar5);
        }
        (*This->lpVtbl->Unlock)(This,arg1);
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
  int iVar1;
  IDirectDrawSurface3 *This;
  GraphicsTextureSourceAsset *pGVar2;
  GraphicsSubresourceIndex GVar3;
  DDPIXELFORMAT *pDVar4;
  bool bVar5;
  TH_LEGACY_LONG TVar6;
  TH_LEGACY_LPVOID arg1;
  byte bVar7;
  byte bVar8;
  ushort uVar9;
  TH_LEGACY_HRESULT TVar10;
  int iVar11;
  byte bVar12;
  uint uVar13;
  byte *pbVar14;
  ushort *puVar15;
  uint *puVar16;
  int iVar17;
  ushort *puVar18;
  uint *puVar19;
  int iStack_38;
  IDirectDrawSurface3 *stagingSurface3;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *sourceAsset;
  
  g_ActiveTextureUploads = g_ActiveTextureUploads + 1;
  This = texture->stagingSurface3;
  pGVar2 = texture->sourceAsset;
  GVar3 = texture->subresourceIndex;
  if (This != (IDirectDrawSurface3 *)0x0) {
    TVar10 = (*This->lpVtbl->IsLost)(This);
    iVar11 = 0;
    if (TVar10 != 0) {
      iVar11 = (*This->lpVtbl->Restore)(This);
    }
    if (iVar11 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar10 = (*This->lpVtbl->Lock)
                         (This,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      TVar6 = g_SurfaceDesc.lPitch;
      if (TVar10 == 0) {
        iVar17 = GVar3 * 0x20 + (pGVar2->tableDescriptor).subresourceTableOffset;
        iVar11 = *(int *)((pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                         iVar17 + -0x10);
        iStack_38 = *(int *)((pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                            iVar17 + -0xc);
        pbVar14 = (pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                  *(int *)((pGVar2->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar17 + -0x1c) + -0x28;
        if (((iVar11 != 0) && (pDVar4 = texture->pixelFormat, iStack_38 != 0)) &&
           (pDVar4->dwRGBBitCount != 8)) {
          iVar17 = 0x1f;
          if (pDVar4->dwRGBAlphaBitMask != 0) {
            for (; pDVar4->dwRGBAlphaBitMask >> iVar17 == 0; iVar17 = iVar17 + -1) {
            }
          }
          bVar12 = (char)iVar17 - 9;
          uVar13 = pDVar4->dwRBitMask | pDVar4->dwGBitMask | pDVar4->dwBBitMask;
          puVar16 = g_SurfaceDesc.lpSurface;
          puVar15 = g_SurfaceDesc.lpSurface;
          iVar17 = iVar11;
          puVar19 = g_SurfaceDesc.lpSurface;
          puVar18 = g_SurfaceDesc.lpSurface;
          if (pDVar4->dwRGBBitCount < 0x11) {
            do {
              do {
                bVar7 = *pbVar14 + pbVar14[2];
                bVar8 = bVar7 + pbVar14[iVar11 * 2];
                uVar9 = CONCAT11(CARRY1(*pbVar14,pbVar14[2]) + CARRY1(bVar7,pbVar14[iVar11 * 2]) +
                                 CARRY1(bVar8,pbVar14[iVar11 * 2 + 2]),
                                 bVar8 + pbVar14[iVar11 * 2 + 2]);
                bVar7 = bVar12 & 0x1f;
                *puVar15 = uVar9 << bVar7 | uVar9 >> 0x20 - bVar7 | (ushort)uVar13;
                pbVar14 = pbVar14 + 4;
                iVar1 = iVar17 + -4;
                bVar5 = 3 < iVar17;
                puVar15 = puVar15 + 1;
                iVar17 = iVar1;
              } while (iVar1 != 0 && bVar5);
              pbVar14 = pbVar14 + iVar11 * 3;
              puVar15 = (ushort *)((int)puVar18 + TVar6);
              iVar1 = iStack_38 + -4;
              bVar5 = 3 < iStack_38;
              iVar17 = iVar11;
              puVar18 = puVar15;
              iStack_38 = iVar1;
            } while (iVar1 != 0 && bVar5);
            (*This->lpVtbl->Unlock)(This,arg1);
            goto GraphicsTextureUploadAlpha4x_DecrementActiveCountAndReturn;
          }
          do {
            do {
              bVar7 = *pbVar14 + pbVar14[2];
              bVar8 = bVar7 + pbVar14[iVar11 * 2];
              uVar9 = CONCAT11(CARRY1(*pbVar14,pbVar14[2]) + CARRY1(bVar7,pbVar14[iVar11 * 2]) +
                               CARRY1(bVar8,pbVar14[iVar11 * 2 + 2]),bVar8 + pbVar14[iVar11 * 2 + 2]
                              );
              bVar7 = bVar12 & 0x1f;
              *puVar16 = (uint)uVar9 << bVar7 | (uint)(uVar9 >> 0x20 - bVar7) | uVar13;
              pbVar14 = pbVar14 + 4;
              iVar1 = iVar17 + -4;
              bVar5 = 3 < iVar17;
              puVar16 = (uint *)((int)puVar16 + 2);
              iVar17 = iVar1;
            } while (iVar1 != 0 && bVar5);
            pbVar14 = pbVar14 + iVar11 * 3;
            puVar16 = (uint *)((int)puVar19 + TVar6);
            iVar1 = iStack_38 + -4;
            bVar5 = 3 < iStack_38;
            iVar17 = iVar11;
            puVar19 = puVar16;
            iStack_38 = iVar1;
          } while (iVar1 != 0 && bVar5);
        }
        (*This->lpVtbl->Unlock)(This,arg1);
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
  GraphicsTextureResource *arg0;
  GraphicsTextureResource *textureResource;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureSet_RefreshColor(subresourceIndex,set);
    return;
  }
  arg0 = set->entries[subresourceIndex].texture;
  (*g_GraphicsDispatchTable.colorUpload[arg0->downsampleShift])(arg0);
  if (arg0->deviceTexture2 != (IDirect3DTexture2 *)0x0) {
    (*arg0->deviceTexture2->lpVtbl->Load)(arg0->deviceTexture2,arg0->stagingTexture2);
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
  GraphicsTextureResource *arg0;
  GraphicsTextureResource *textureResource;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureSet_RefreshAlpha(subresourceIndex,set);
    return;
  }
  arg0 = set->entries[subresourceIndex].texture;
  (*g_GraphicsDispatchTable.alphaUpload[arg0->downsampleShift])(arg0);
  if (arg0->deviceTexture2 != (IDirect3DTexture2 *)0x0) {
    (*arg0->deviceTexture2->lpVtbl->Load)(arg0->deviceTexture2,arg0->stagingTexture2);
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
  D3DDEVICEDESC_DX6 *pDVar1;
  uint uVar2;
  TH_LEGACY_HRESULT TVar3;
  sdword sVar4;
  GraphicsTextureDownsampleShift GVar5;
  int iVar6;
  DDPIXELFORMAT *pDVar7;
  DDPIXELFORMAT *pDVar8;
  bool bVar9;
  GraphicsTextureSizeEaxEdxCf9 GVar10;
  dword textureHandle;
  IDirect3DTexture2 *deviceTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  
  deviceSurfaceBase = (IDirectDrawSurface *)0x0;
  deviceSurface3 = (IDirectDrawSurface3 *)0x0;
  deviceTexture2 = (IDirect3DTexture2 *)0x0;
  Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = 0x6c;
  pDVar1 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].hardwareDesc;
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 0) {
    g_SurfaceDesc.dwSize = 0x6c;
    return;
  }
  g_SurfaceDesc.dwFlags = 0x1007;
  g_SurfaceDesc.ddsCaps.dwCaps = 0x4001000;
  GVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  if (pDVar1->dcmColorModel == 0) {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | 0x800;
  }
  else {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | 0x4000;
  }
  uVar2 = GVar10.logicalWidthPixels;
  if (GVar10.logicalWidthPixels < GVar10.logicalHeightPixels) {
    uVar2 = GVar10.logicalHeightPixels;
  }
  g_SurfaceDesc.dwHeight = uVar2 >> ((byte)g_TextureDownsampleShift & 0x1f);
  GVar5 = g_TextureDownsampleShift;
  do {
    if (0xf < (int)g_SurfaceDesc.dwHeight) break;
    g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
    GVar5 = GVar5 - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
  } while (GVar5 != GRAPHICS_TEXTURE_DOWNSAMPLE_1X);
  texture->downsampleShift = GVar5;
  pDVar7 = texture->pixelFormat;
  pDVar8 = &g_SurfaceDesc.ddpfPixelFormat;
  g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
  for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
    pDVar8->dwSize = pDVar7->dwSize;
    pDVar7 = (DDPIXELFORMAT *)&pDVar7->dwFlags;
    pDVar8 = (DDPIXELFORMAT *)&pDVar8->dwFlags;
  }
  do {
    TVar3 = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&deviceSurfaceBase,(TH_LEGACY_LPVOID)0x0);
    if (TVar3 == 0) {
      TVar3 = (*deviceSurfaceBase->lpVtbl->QueryInterface)
                        (deviceSurfaceBase,&IID_IDirectDrawSurface3_Local,&deviceSurface3);
      if ((TVar3 == 0) &&
         (TVar3 = (*deviceSurface3->lpVtbl->QueryInterface)
                            (deviceSurface3,&IID_IDirect3DTexture2_Local,&deviceTexture2),
         TVar3 == 0)) {
        TVar3 = (*deviceTexture2->lpVtbl->Load)(deviceTexture2,texture->stagingTexture2);
        if (TVar3 == 0) {
          sVar4 = (*deviceTexture2->lpVtbl->GetHandle)
                            (deviceTexture2,g_Direct3DDevice2,&textureHandle);
          if (sVar4 == 0) {
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
    if ((TVar3 != -0x7789fe84) || (bVar9 = GraphicsTexture_EvictOldestDeviceTexture(texture), bVar9)
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
  dword dVar1;
  int iVar2;
  GraphicsPaletteTextureSourceAsset *pGVar3;
  GraphicsPaletteTextureSourceAsset *pGVar4;
  int iVar5;
  GraphicsPaletteTextureFormatVersion *pGVar6;
  byte *pbVar7;
  GraphicsPaletteTextureSourceEaxCf5 GVar8;
  ArenaAllocEaxCf5 AVar9;
  GraphicsTextureSetEaxCf5 GVar10;
  GraphicsAssetAllocationByteSize GStackY_20;
  
  GVar8 = (*g_GraphicsTextureSourceConvertPaletteEntries)
                    ((GraphicsPaletteTextureSourceAsset *)sourceAsset);
  pGVar3 = GVar8.paletteSource;
  pGVar4 = pGVar3;
  if (!GVar8.carry) {
    GStackY_20 = pGVar3->subresourceCount;
    AVar9 = (*g_MemoryApi.alloc)(GStackY_20 * 0x20 + 8);
    pGVar4 = (GraphicsPaletteTextureSourceAsset *)AVar9.eax;
    if (!AVar9.carry) {
      pGVar6 = &pGVar4->formatVersion;
      pGVar4->magic = (GraphicsPaletteTextureAssetMagic)pGVar3;
      pGVar4->allocationSizeBytes = GStackY_20;
      pbVar7 = pGVar3->reserved10_AF + (pGVar3->subresourceTableOffset - 0x10);
      iVar5 = 0;
      while( true ) {
        dVar1 = 0x1f;
        if (*(uint *)(pbVar7 + 0x18) != 0) {
          for (; *(uint *)(pbVar7 + 0x18) >> dVar1 == 0; dVar1 = dVar1 - 1) {
          }
        }
        *pGVar6 = 0;
        pGVar6[5] = iVar5;
        pGVar6[1] = dVar1;
        if (1 << ((byte)dVar1 & 0x1f) != *(int *)(pbVar7 + 0x18)) break;
        pGVar6[3] = (GraphicsPaletteTextureFormatVersion)pGVar3;
        iVar2 = 0x1f;
        if (*(uint *)(pbVar7 + 0x1c) != 0) {
          for (; *(uint *)(pbVar7 + 0x1c) >> iVar2 == 0; iVar2 = iVar2 + -1) {
          }
        }
        pGVar6[4] = (GraphicsPaletteTextureFormatVersion)pbVar7;
        pGVar6[2] = iVar2;
        if (1 << ((byte)iVar2 & 0x1f) != *(int *)(pbVar7 + 0x1c)) break;
        pGVar6 = pGVar6 + 8;
        pbVar7 = pbVar7 + 0x20;
        iVar5 = iVar5 + 1;
        GStackY_20 = GStackY_20 - 1;
        if (GStackY_20 == 0) {
          return (GraphicsTextureSetEaxCf5)((uint5)AVar9 & 0xffffffff);
        }
      }
      pGVar4 = (GraphicsPaletteTextureSourceAsset *)&k_LowAddressLiteral0000002F;
    }
  }
  GVar10.carry = true;
  GVar10.textureSet = (GraphicsTextureSet *)pGVar4;
  return GVar10;
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
  GraphicsTextureResource *pGVar1;
  dword dVar2;
  int iVar3;
  GraphicsResourceUsageSerial GVar4;
  GraphicsTextureResource *pGVar5;
  GraphicsTextureResource **ppGVar6;
  IDirect3DTexture2 *deviceTexture2;
  IDirectDrawSurface3 *deviceSurface3;
  IDirectDrawSurface *deviceSurfaceBase;
  
  iVar3 = 0x1000;
  GVar4 = 0xffffffff;
  pGVar5 = (GraphicsTextureResource *)0x0;
  ppGVar6 = g_GraphicsTextureSlots;
  do {
    pGVar1 = *ppGVar6;
    if ((((pGVar1 != (GraphicsTextureResource *)0x0) && (pGVar1 != exclude)) &&
        (pGVar1->textureHandle != 0)) && (pGVar1->lastUsedCounter <= GVar4)) {
      GVar4 = pGVar1->lastUsedCounter;
      pGVar5 = pGVar1;
    }
    ppGVar6 = ppGVar6 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  if (pGVar5 == (GraphicsTextureResource *)0x0) {
    return true;
  }
  deviceTexture2 = pGVar5->deviceTexture2;
  if (deviceTexture2 != (IDirect3DTexture2 *)0x0) {
    (*deviceTexture2->lpVtbl->Release)(deviceTexture2);
  }
  deviceSurface3 = pGVar5->deviceSurface3;
  if (deviceSurface3 != (IDirectDrawSurface3 *)0x0) {
    (*deviceSurface3->lpVtbl->Release)(deviceSurface3);
  }
  deviceSurfaceBase = pGVar5->deviceSurfaceBase;
  if (deviceSurfaceBase != (IDirectDrawSurface *)0x0) {
    (*deviceSurfaceBase->lpVtbl->Release)(deviceSurfaceBase);
  }
  dVar2 = pGVar5->textureHandle;
  pGVar5->deviceSurfaceBase = (IDirectDrawSurface *)0x0;
  pGVar5->deviceSurface3 = (IDirectDrawSurface3 *)0x0;
  pGVar5->deviceTexture2 = (IDirect3DTexture2 *)0x0;
  pGVar5->textureHandle = 0;
  if (dVar2 == g_BoundTextureHandle) {
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
  int iVar1;
  DDPIXELFORMAT *pDVar2;
  GraphicsTextureSourceAsset *pGVar3;
  byte *pbVar4;
  int iVar5;
  
  iVar1 = subresourceIndex * 0x20 + (sourceAsset->tableDescriptor).subresourceTableOffset;
  iVar5 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0x20)
  ;
  if (iVar5 < 0) {
    pbVar4 = (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
             *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     iVar1 + -0x1c) + -0x28;
    iVar5 = *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                    iVar1 + -0x10) *
            *(int *)((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 + iVar1 + -0xc
                    );
    do {
      if (*(uint *)pbVar4 < 0xff000000) {
        return (DDPIXELFORMAT *)0x577db0;
      }
      pbVar4 = pbVar4 + 4;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    pDVar2 = (DDPIXELFORMAT *)0x577d90;
  }
  else {
    pGVar3 = sourceAsset + iVar5 * 4 + 1;
    iVar5 = 0x100;
    do {
      if ((pGVar3->common).magic < 0xff000000) {
        return (DDPIXELFORMAT *)0x577df0;
      }
      pGVar3 = (GraphicsTextureSourceAsset *)&(pGVar3->common).formatVersion;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    pDVar2 = (DDPIXELFORMAT *)0x577dd0;
  }
  return pDVar2;
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
  GraphicsTextureResource *in_EAX;
  uint uVar1;
  TH_LEGACY_HRESULT TVar2;
  GraphicsTextureDownsampleShift GVar3;
  int iVar4;
  DDPIXELFORMAT *pDVar5;
  DDPIXELFORMAT *pDVar6;
  GraphicsTextureSizeEaxEdxCf9 GVar7;
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
    GVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset)
    ;
    uVar1 = GVar7.logicalWidthPixels;
    if (GVar7.logicalWidthPixels < GVar7.logicalHeightPixels) {
      uVar1 = GVar7.logicalHeightPixels;
    }
    g_SurfaceDesc.dwHeight = uVar1 >> ((byte)g_TextureDownsampleShift & 0x1f);
    GVar3 = g_TextureDownsampleShift;
    do {
      if (0xf < (int)g_SurfaceDesc.dwHeight) break;
      g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
      GVar3 = GVar3 - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
    } while (GVar3 != GRAPHICS_TEXTURE_DOWNSAMPLE_1X);
    texture->downsampleShift = GVar3;
    pDVar5 = texture->pixelFormat;
    pDVar6 = &g_SurfaceDesc.ddpfPixelFormat;
    g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
    for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
      pDVar6->dwSize = pDVar5->dwSize;
      pDVar5 = (DDPIXELFORMAT *)&pDVar5->dwFlags;
      pDVar6 = (DDPIXELFORMAT *)&pDVar6->dwFlags;
    }
    TVar2 = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&surfaceBase,(TH_LEGACY_LPVOID)0x0);
    if (TVar2 == 0) {
      TVar2 = (*surfaceBase->lpVtbl->QueryInterface)
                        (surfaceBase,&IID_IDirectDrawSurface3_Local,&surface3);
      if (TVar2 == 0) {
        TVar2 = (*surface3->lpVtbl->QueryInterface)(surface3,&IID_IDirect3DTexture2_Local,&texture2)
        ;
        if (TVar2 == 0) goto GraphicsTexture_CommitStagingResourcesAndUpload;
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
    return in_EAX;
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
  return in_EAX;
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

