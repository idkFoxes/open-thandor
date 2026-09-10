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
GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *sourceAsset_00;
  uint uVar1;
  dword dVar2;
  GraphicsTextureSet *pGVar3;
  DDPIXELFORMAT *pDVar4;
  GraphicsTextureResource *pGVar5;
  GraphicsTextureResource *texture;
  int extraout_ECX;
  int iVar6;
  word *pwVar7;
  bool bVar8;
  undefined1 uVar9;
  GraphicsTextureSetEntry *pGStack_24;
  AssetSubresourceCount AStack_20;
  GraphicsSubresourceIndex GStack_1c;
  
  iVar6 = g_ActiveGraphicsAdapterIndex * 0x80;
  bVar8 = g_ActiveGraphicsAdapterIndex << 6 < 0;
  pGVar3 = GraphicsTextureSet_AllocateMetadata(sourceAsset);
  if (bVar8) {
    return pGVar3;
  }
  if (*(int *)(extraout_ECX + 0x10 + iVar6) == 1) {
    pGVar3 = Glide3_TextureSet_CreateBackend(sourceAsset);
    return pGVar3;
  }
  sourceAsset_00 = pGVar3->sourceAsset;
  uVar1 = (sourceAsset_00->tableDescriptor).subresourceTableOffset;
  AStack_20 = (sourceAsset_00->tableDescriptor).subresourceCount;
  uVar9 = CARRY4(uVar1,(uint)sourceAsset_00);
  pwVar7 = (word *)((sourceAsset_00->common).buildMetadata.assetRelativeAddressAnchor28 +
                   (uVar1 - 0x28));
  pGStack_24 = pGVar3->entries;
  GStack_1c = 0;
  do {
    pDVar4 = GraphicsTexture_SelectPixelFormat(GStack_1c,sourceAsset_00);
    pGVar5 = (*g_MemoryApi.alloc)(0x50);
    if (!(bool)uVar9) {
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
      dVar2 = g_TextureDownsampleShift;
      pGVar5->lastUsedCounter = 0;
      pGVar5->textureHandle = 0;
      pGVar5->downsampleShift = dVar2;
      pGVar5 = GraphicsTexture_CreateStagingTexture(pGVar5);
      GraphicsTexture_RegisterSlot(pGVar5);
      if ((bool)uVar9) {
        pGVar5 = GraphicsTexture_ReleaseObjects(texture);
        (*g_MemoryApi.free)(pGVar5);
        pGStack_24->texture = (GraphicsTextureResource *)0x0;
      }
      else {
        GraphicsTexture_CreateDeviceTexture(texture);
      }
    }
    GStack_1c = GStack_1c + 1;
    pGStack_24 = pGStack_24 + 1;
    uVar9 = (word *)0xffffffdf < pwVar7;
    pwVar7 = pwVar7 + 0x10;
    AStack_20 = AStack_20 - 1;
  } while (AStack_20 != 0);
  return pGVar3;
}

/* Address: 0x0057AD30.
   Ownership: graphics/resources/texture.
   Purpose: Dispatches all-resource texture rebuilding by selected backend. The Glide marker delegates to
   Glide3_TextureResource_ReinitializeAll. DirectDraw scans all 4096 slots, releases every registered resource's
   six COM objects, and recreates its staging texture. Device textures remain lazy and are recreated on demand.
   Local calls: GraphicsTexture_ReleaseObjects, GraphicsTexture_CreateStagingTexture.
   Cross-module calls: Glide3_TextureResource_ReinitializeAll [graphics/backend/glide].
*/
void __cdecl GraphicsTexture_RebuildAllStagingTextures(void)

{
  GraphicsTextureResource *texture;
  int textureSlotsRemaining;
  int extraout_ECX;
  GraphicsTextureResource **textureSlotCursor;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureResource_ReinitializeAll();
    return;
  }
  textureSlotsRemaining = 0x1000;
  textureSlotCursor = g_GraphicsTextureSlots;
  do {
    if (*textureSlotCursor != (GraphicsTextureResource *)0x0) {
      texture = GraphicsTexture_ReleaseObjects(*textureSlotCursor);
      GraphicsTexture_CreateStagingTexture(texture);
      textureSlotsRemaining = extraout_ECX;
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
GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set)

{
  GraphicsTextureResource **ppGVar1;
  GraphicsTextureSourceAsset *pGVar2;
  GraphicsTextureResource *pGVar3;
  int iVar4;
  dword extraout_EDX;
  dword dVar5;
  GraphicsTextureSetEntry *pGVar6;
  GraphicsTextureResource **ppGVar7;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    pGVar2 = Glide3_TextureSet_DestroyBackend(set);
    return pGVar2;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (set != (GraphicsTextureSet *)0x0) {
    dVar5 = set->subresourceCount;
    pGVar6 = set->entries;
    do {
      pGVar3 = pGVar6->texture;
      if (pGVar3 != (GraphicsTextureResource *)0x0) {
        iVar4 = 0x1000;
        ppGVar1 = g_GraphicsTextureSlots;
        do {
          ppGVar7 = ppGVar1;
          if (pGVar3 == *ppGVar7) break;
          iVar4 = iVar4 + -1;
          ppGVar1 = ppGVar7 + 1;
        } while (iVar4 != 0);
        *ppGVar7 = (GraphicsTextureResource *)0x0;
        pGVar3 = GraphicsTexture_ReleaseObjects(pGVar3);
        (*g_MemoryApi.free)(pGVar3);
        dVar5 = extraout_EDX;
      }
      pGVar6 = pGVar6 + 1;
      dVar5 = dVar5 - 1;
    } while (dVar5 != 0);
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
GraphicsTextureSet * __fastcall
GraphicsTextureSet_LoadPackage(dword packageContext0,dword packageContext1,word *pathUtf16)

{
  GraphicsTextureSourceAsset *loadedTextureSource;
  GraphicsTextureSet *createdTextureSet;
  undefined1 in_CF;
  
  loadedTextureSource = Package_LoadEntry(pathUtf16);
  if (!(bool)in_CF) {
    createdTextureSet = (*g_GraphicsCreateTextureSet)(loadedTextureSource);
    if (!(bool)in_CF) {
      return createdTextureSet;
    }
    loadedTextureSource = (GraphicsTextureSourceAsset *)Resource_Release(loadedTextureSource);
  }
  return (GraphicsTextureSet *)loadedTextureSource;
}

/* Address: 0x00485E80.
   Ownership: graphics/resources/texture.
   Purpose: Destroys one texture set through g_GraphicsDestroyTextureSet. That service returns the owned source
   asset, which this wrapper releases through Resource_Release.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set)

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
void GraphicsTextureSet_RefreshNoOp
               (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

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
qword GraphicsTextureSource_GetLogicalSizeRegs
                (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  AssetRelativeOffset subresourceTableOffset;
  
  if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
     (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
    subresourceTableOffset = (sourceAsset->tableDescriptor).subresourceTableOffset;
    return CONCAT44(*(undefined4 *)
                     ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     subresourceIndex * 0x20 + subresourceTableOffset + -0x24),
                    *(undefined4 *)
                     ((sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
                     subresourceIndex * 0x20 + subresourceTableOffset + -0x28));
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004A92C0.
   Ownership: graphics/resources/texture.
   Purpose: Converts a screen-space point into source-pixel coordinates using originX/originY, validates it against
   pixelWidth/pixelHeight, then checks the ARGB alpha byte. Direct-color and paletted entries are both supported.
   ABI: CF set means the tested source pixel is opaque/hit. CF clear means transparent, outside the image, or
   invalid input.
*/
void GraphicsTextureSource_TestOpaquePixel
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
            return;
          }
        }
        else if (0xffffff <
                 *(uint *)(sourceAsset[iVar3 * 4 + 1].common.buildMetadata.
                           assetRelativeAddressAnchor28 +
                          (uint)*(byte *)(iVar4 + *(int *)((sourceAsset->common).buildMetadata.
                                                           assetRelativeAddressAnchor28 +
                                                          iVar5 + AVar2 + -0x1c) + (int)sourceAsset)
                          * 8 + -0x28)) {
          return;
        }
      }
    }
  }
  return;
}

/* Address: 0x004A9A50.
   Ownership: graphics/resources/texture.
   Purpose: Repeats one logical source tile across the requested destination rectangle and calls
   g_GraphicsTextureSourceBlitSourceAlpha for each tile. repeatEndX or repeatEndY equal to INT32_MIN means one
   logical tile extent from the corresponding origin. The first tile origin is moved backward by whole logical tile
   dimensions until it covers the clipping minimum. CF is cleared before return. It selects an existing resource
   facet and does not imply sprite, model, or effect identity.
*/
void GraphicsTextureSource_BlitTiledSourceAlpha
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
               GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
               GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
               SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  int iVar2;
  int arg5;
  GraphicsPixelDimension extraout_ECX;
  int iVar3;
  sdword arg4;
  bool bVar4;
  qword qVar5;
  
  qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  iVar3 = (int)(qVar5 >> 0x20);
  iVar1 = (int)qVar5;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + iVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + iVar3;
  }
  do {
    do {
      bVar4 = SCARRY4(tileOriginX,iVar1);
      tileOriginX = tileOriginX + iVar1;
    } while (tileOriginX == 0 || bVar4 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  iVar2 = tileOriginX - iVar1;
  do {
    do {
      bVar4 = SCARRY4(tileOriginY,iVar3);
      tileOriginY = tileOriginY + iVar3;
    } while (tileOriginY == 0 || bVar4 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - iVar3;
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
  if ((iVar2 < repeatEndX) && (arg5 = iVar2, arg4 < repeatEndY)) {
    do {
      do {
        qVar5 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                          (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,
                           sourceAsset,framebuffer);
        arg4 = (sdword)(qVar5 >> 0x20);
        arg5 = (int)qVar5 + iVar1;
        repeatEndY = extraout_ECX;
      } while (arg5 < repeatEndX);
      arg4 = arg4 + iVar3;
      arg5 = iVar2;
    } while (arg4 < (int)extraout_ECX);
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
void GraphicsTextureSource_BlitTiledHalfSourceRgb
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
               GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
               GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
               SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  int iVar2;
  int arg5;
  GraphicsPixelDimension extraout_ECX;
  int iVar3;
  sdword arg4;
  bool bVar4;
  qword qVar5;
  
  qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  iVar3 = (int)(qVar5 >> 0x20);
  iVar1 = (int)qVar5;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + iVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + iVar3;
  }
  do {
    do {
      bVar4 = SCARRY4(tileOriginX,iVar1);
      tileOriginX = tileOriginX + iVar1;
    } while (tileOriginX == 0 || bVar4 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  iVar2 = tileOriginX - iVar1;
  do {
    do {
      bVar4 = SCARRY4(tileOriginY,iVar3);
      tileOriginY = tileOriginY + iVar3;
    } while (tileOriginY == 0 || bVar4 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - iVar3;
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
  if ((iVar2 < repeatEndX) && (arg5 = iVar2, arg4 < repeatEndY)) {
    do {
      do {
        qVar5 = (*g_GraphicsTextureSourceBlitHalfSourceRgb)
                          (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,
                           sourceAsset,framebuffer);
        arg4 = (sdword)(qVar5 >> 0x20);
        arg5 = (int)qVar5 + iVar1;
        repeatEndY = extraout_ECX;
      } while (arg5 < repeatEndX);
      arg4 = arg4 + iVar3;
      arg5 = iVar2;
    } while (arg4 < (int)extraout_ECX);
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
void GraphicsTextureSource_BlitTiledSaturatedAddRgb
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
               GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
               GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
               SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  int iVar2;
  int arg5;
  GraphicsPixelDimension extraout_ECX;
  int iVar3;
  sdword arg4;
  bool bVar4;
  qword qVar5;
  
  qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  iVar3 = (int)(qVar5 >> 0x20);
  iVar1 = (int)qVar5;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + iVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + iVar3;
  }
  do {
    do {
      bVar4 = SCARRY4(tileOriginX,iVar1);
      tileOriginX = tileOriginX + iVar1;
    } while (tileOriginX == 0 || bVar4 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  iVar2 = tileOriginX - iVar1;
  do {
    do {
      bVar4 = SCARRY4(tileOriginY,iVar3);
      tileOriginY = tileOriginY + iVar3;
    } while (tileOriginY == 0 || bVar4 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - iVar3;
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
  if ((iVar2 < repeatEndX) && (arg5 = iVar2, arg4 < repeatEndY)) {
    do {
      do {
        qVar5 = (*g_GraphicsTextureSourceBlitSaturatedAddRgb)
                          (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,
                           sourceAsset,framebuffer);
        arg4 = (sdword)(qVar5 >> 0x20);
        arg5 = (int)qVar5 + iVar1;
        repeatEndY = extraout_ECX;
      } while (arg5 < repeatEndX);
      arg4 = arg4 + iVar3;
      arg5 = iVar2;
    } while (arg4 < (int)extraout_ECX);
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
void GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsScreenCoordinate repeatEndY,GraphicsScreenCoordinate repeatEndX,
               GraphicsScreenCoordinate tileOriginY,GraphicsScreenCoordinate tileOriginX,
               GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
               SoftwareFramebufferAccess *framebuffer)

{
  int iVar1;
  int iVar2;
  int arg5;
  GraphicsPixelDimension extraout_ECX;
  int iVar3;
  sdword arg4;
  bool bVar4;
  qword qVar5;
  
  qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,sourceAsset);
  iVar3 = (int)(qVar5 >> 0x20);
  iVar1 = (int)qVar5;
  if (repeatEndX == -0x80000000) {
    repeatEndX = tileOriginX + iVar1;
  }
  if (repeatEndY == -0x80000000) {
    repeatEndY = tileOriginY + iVar3;
  }
  do {
    do {
      bVar4 = SCARRY4(tileOriginX,iVar1);
      tileOriginX = tileOriginX + iVar1;
    } while (tileOriginX == 0 || bVar4 != tileOriginX < 0);
  } while (tileOriginX <= clipMinX);
  iVar2 = tileOriginX - iVar1;
  do {
    do {
      bVar4 = SCARRY4(tileOriginY,iVar3);
      tileOriginY = tileOriginY + iVar3;
    } while (tileOriginY == 0 || bVar4 != tileOriginY < 0);
  } while (tileOriginY <= clipMinY);
  arg4 = tileOriginY - iVar3;
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
  if ((iVar2 < repeatEndX) && (arg5 = iVar2, arg4 < repeatEndY)) {
    do {
      do {
        qVar5 = (*g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd)
                          (repeatEndY,repeatEndX,clipMinY,clipMinX,arg4,arg5,subresourceIndex,
                           sourceAsset,framebuffer);
        arg4 = (sdword)(qVar5 >> 0x20);
        arg5 = (int)qVar5 + iVar1;
        repeatEndY = extraout_ECX;
      } while (arg5 < repeatEndX);
      arg4 = arg4 + iVar3;
      arg5 = iVar2;
    } while (arg4 < (int)extraout_ECX);
  }
  return;
}

/* Address: 0x004AD630.
   Ownership: graphics/resources/texture.
   Purpose: Loads one gfx asset through Package_LoadEntry, converts every palette entry to the active framebuffer
   format, and returns the loaded asset. The first two parameters are the ECX/EDX package-loader context preserved
   from Package_LoadEntry; their higher-level semantics remain unresolved. ABI: CF clear means success. CF set
   means load or palette conversion failure.
   Cross-module calls: Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
GraphicsTextureSourceAsset * __fastcall
GraphicsTextureSource_LoadPackageAsset(dword packageContext0,dword packageContext1,word *pathUtf16)

{
  GraphicsPaletteTextureSourceAsset *loadedPaletteTextureSource;
  GraphicsPaletteTextureSourceAsset *convertedTextureSource;
  undefined1 in_CF;
  
  loadedPaletteTextureSource = Package_LoadEntry(pathUtf16);
  if (!(bool)in_CF) {
    convertedTextureSource =
         (*g_GraphicsTextureSourceConvertPaletteEntries)(loadedPaletteTextureSource);
    if (!(bool)in_CF) {
      return (GraphicsTextureSourceAsset *)convertedTextureSource;
    }
    loadedPaletteTextureSource =
         (GraphicsPaletteTextureSourceAsset *)Resource_Release(loadedPaletteTextureSource);
  }
  return (GraphicsTextureSourceAsset *)loadedPaletteTextureSource;
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
  GraphicsPaletteTextureSourceAsset *pGVar1;
  uint extraout_ECX;
  uint uVar2;
  undefined1 in_CF;
  bool bVar3;
  
  arg0 = (*g_MemoryApi.alloc)((sourceAsset->common).allocationSizeBytes);
  if (!(bool)in_CF) {
    bVar3 = (extraout_ECX >> 1 & 1) != 0;
    pGVar1 = arg0;
    for (uVar2 = extraout_ECX >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      pGVar1->magic = (sourceAsset->common).magic;
      sourceAsset = (GraphicsTextureSourceAsset *)&(sourceAsset->common).allocationSizeBytes;
      pGVar1 = (GraphicsPaletteTextureSourceAsset *)&pGVar1->allocationSizeBytes;
    }
    pGVar1 = (*g_GraphicsTextureSourceConvertPaletteEntries)(arg0);
    if (!bVar3) {
      return (GraphicsTextureSourceAsset *)pGVar1;
    }
    arg0 = (GraphicsPaletteTextureSourceAsset *)(*g_MemoryApi.free)(arg0);
  }
  return (GraphicsTextureSourceAsset *)arg0;
}

/* Address: 0x004AD6C0.
   Ownership: graphics/resources/texture.
   Purpose: Validates the gfx signature, then converts paletteBankCount * 256 entries beginning at offset 0x200.
   Each 8-byte entry retains argb8888 and receives framebufferPixel through g_SoftwarePixelPackTables. ABI: CF
   clear means success. CF set means invalid input.
*/
GraphicsPaletteTextureSourceAsset *
GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset)

{
  int paletteEntriesRemaining;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
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
    return sourceAsset;
  }
  return (GraphicsPaletteTextureSourceAsset *)0x2c;
}

/* Address: 0x004AD770.
   Ownership: graphics/resources/texture.
   Purpose: Resolves the underlying allocation base through g_GraphicsTextureSourceResolveAllocationBase, then
   releases it through Resource_Release. Use this path for assets returned by
   GraphicsTextureSource_LoadPackageAsset. Graphics texture-source lifecycle callback.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset)

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
void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset)

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
GraphicsTextureSourceAsset *
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
GraphicsLogicalDimensionsEdxEax8
GraphicsTextureSource_GetFirstLogicalSizeRegs(GraphicsTextureSourceAsset *sourceAsset)

{
  undefined4 in_EAX;
  byte *firstSubresourceRecord;
  uint subresourceCount;
  
  if ((((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
      (subresourceCount = (sourceAsset->tableDescriptor).subresourceCount, subresourceCount != 0))
     && (subresourceCount < 0x1000)) {
    firstSubresourceRecord =
         (sourceAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
         ((sourceAsset->tableDescriptor).subresourceTableOffset - 0x28);
    in_EAX = *(undefined4 *)firstSubresourceRecord;
    sourceAsset = *(GraphicsTextureSourceAsset **)(firstSubresourceRecord + 4);
  }
  return CONCAT44(sourceAsset,in_EAX);
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
void GraphicsTexture_UploadColor_1x(GraphicsTextureResource *texture)

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
void GraphicsTexture_UploadColor_2x(GraphicsTextureResource *texture)

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
void GraphicsTexture_UploadColor_4x(GraphicsTextureResource *texture)

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
void GraphicsTexture_UploadAlpha_1x(GraphicsTextureResource *texture)

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
void GraphicsTexture_UploadAlpha_2x(GraphicsTextureResource *texture)

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
void GraphicsTexture_UploadAlpha_4x(GraphicsTextureResource *texture)

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
void GraphicsTextureSet_RefreshColor
               (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  int extraout_EDX;
  GraphicsTextureResource *textureResource;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureSet_RefreshColor(subresourceIndex,set);
    return;
  }
  textureResource = set->entries[subresourceIndex].texture;
  (*g_GraphicsDispatchTable.colorUpload[textureResource->downsampleShift])(textureResource);
  if (*(int *)(extraout_EDX + 8) != 0) {
    (**(code **)(**(int **)(extraout_EDX + 8) + 0x14))
              (*(int **)(extraout_EDX + 8),*(undefined4 *)(extraout_EDX + 0x14));
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
void GraphicsTextureSet_RefreshAlpha
               (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  int extraout_EDX;
  GraphicsTextureResource *textureResource;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    Glide3_TextureSet_RefreshAlpha(subresourceIndex,set);
    return;
  }
  textureResource = set->entries[subresourceIndex].texture;
  (*g_GraphicsDispatchTable.alphaUpload[textureResource->downsampleShift])(textureResource);
  if (*(int *)(extraout_EDX + 8) != 0) {
    (**(code **)(**(int **)(extraout_EDX + 8) + 0x14))
              (*(int **)(extraout_EDX + 8),*(undefined4 *)(extraout_EDX + 0x14));
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
GraphicsTextureResource * GraphicsTexture_CreateDeviceTexture(GraphicsTextureResource *texture)

{
  D3DDEVICEDESC_DX6 *pDVar1;
  GraphicsTextureResource *in_EAX;
  uint uVar2;
  uint uVar3;
  sdword sVar4;
  GraphicsTextureDownsampleShift GVar5;
  int iVar6;
  DDPIXELFORMAT *pDVar7;
  DDPIXELFORMAT *pDVar8;
  bool bVar9;
  qword qVar10;
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
    return in_EAX;
  }
  g_SurfaceDesc.dwFlags = 0x1007;
  g_SurfaceDesc.ddsCaps.dwCaps = 0x4001000;
  qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset);
  uVar3 = (uint)(qVar10 >> 0x20);
  if (pDVar1->dcmColorModel == 0) {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | 0x800;
  }
  else {
    g_SurfaceDesc.ddsCaps.dwCaps = g_SurfaceDesc.ddsCaps.dwCaps | 0x4000;
  }
  uVar2 = (uint)qVar10;
  if ((uint)qVar10 < uVar3) {
    uVar2 = uVar3;
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
    uVar3 = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&deviceSurfaceBase,(TH_LEGACY_LPVOID)0x0);
    if (uVar3 == 0) {
      uVar3 = (*deviceSurfaceBase->lpVtbl->QueryInterface)
                        (deviceSurfaceBase,&IID_IDirectDrawSurface3_Local,&deviceSurface3);
      if ((uVar3 == 0) &&
         (uVar3 = (*deviceSurface3->lpVtbl->QueryInterface)
                            (deviceSurface3,&IID_IDirect3DTexture2_Local,&deviceTexture2),
         uVar3 == 0)) {
        uVar3 = (*deviceTexture2->lpVtbl->Load)(deviceTexture2,texture->stagingTexture2);
        if (uVar3 == 0) {
          sVar4 = (*deviceTexture2->lpVtbl->GetHandle)
                            (deviceTexture2,g_Direct3DDevice2,&textureHandle);
          if (sVar4 == 0) {
            texture->deviceSurfaceBase = deviceSurfaceBase;
            texture->deviceSurface3 = deviceSurface3;
            texture->deviceTexture2 = deviceTexture2;
            texture->textureHandle = textureHandle;
            return in_EAX;
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
          return in_EAX;
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
    bVar9 = uVar3 < 0x8876017c;
    if ((uVar3 != 0x8876017c) || (GraphicsTexture_EvictOldestDeviceTexture(texture), bVar9))
    goto GraphicsTexture_ReleasePartialDeviceResourcesAfterFailure;
  } while( true );
}

/* Address: 0x00485EA0.
   Ownership: graphics/resources/texture.
   Purpose: Converts the source asset's palette entries for the active framebuffer, allocates 8 +
   subresourceCount*0x20 bytes, and fills one GraphicsTextureSetEntry per source entry. Each pixelWidth and
   pixelHeight must be an exact power of two. widthLog2 and heightLog2 are generated with BSR. ABI: CF clear means
   success. CF set means palette conversion, allocation, or power-of-two validation failed.
*/
GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset)

{
  dword dVar1;
  int iVar2;
  GraphicsPaletteTextureSourceAsset *pGVar3;
  GraphicsPaletteTextureSourceAsset *pGVar4;
  uint uVar5;
  int iVar6;
  GraphicsPaletteTextureFormatVersion *pGVar7;
  int iVar8;
  undefined1 in_CF;
  bool bVar9;
  void *pvVar10;
  GraphicsAssetAllocationByteSize GStackY_28;
  
  pGVar3 = (*g_GraphicsTextureSourceConvertPaletteEntries)
                     ((GraphicsPaletteTextureSourceAsset *)sourceAsset);
  pGVar4 = pGVar3;
  if (!(bool)in_CF) {
    uVar5 = pGVar3->subresourceCount * 0x20;
    bVar9 = 0xfffffff7 < uVar5;
    _pvVar10 = (*g_MemoryApi.alloc)(uVar5 + 8);
    GStackY_28 = (GraphicsAssetAllocationByteSize)((ulonglong)_pvVar10 >> 0x20);
    pGVar4 = SUB84(_pvVar10,0);
    if (!bVar9) {
      pGVar7 = &pGVar4->formatVersion;
      pGVar4->magic = (GraphicsPaletteTextureAssetMagic)pGVar3;
      pGVar4->allocationSizeBytes = GStackY_28;
      iVar8 = (int)&pGVar3->magic + pGVar3->subresourceTableOffset;
      iVar6 = 0;
      while( true ) {
        dVar1 = 0x1f;
        if (*(int *)(iVar8 + 0x18) != 0) {
          for (; *(uint *)(iVar8 + 0x18) >> dVar1 == 0; dVar1 = dVar1 - 1) {
          }
        }
        *pGVar7 = 0;
        pGVar7[5] = iVar6;
        pGVar7[1] = dVar1;
        if (1 << ((byte)dVar1 & 0x1f) != *(int *)(iVar8 + 0x18)) break;
        pGVar7[3] = (GraphicsPaletteTextureFormatVersion)pGVar3;
        iVar2 = 0x1f;
        if (*(int *)(iVar8 + 0x1c) != 0) {
          for (; *(uint *)(iVar8 + 0x1c) >> iVar2 == 0; iVar2 = iVar2 + -1) {
          }
        }
        pGVar7[4] = iVar8;
        pGVar7[2] = iVar2;
        if (1 << ((byte)iVar2 & 0x1f) != *(int *)(iVar8 + 0x1c)) break;
        pGVar7 = pGVar7 + 8;
        iVar8 = iVar8 + 0x20;
        iVar6 = iVar6 + 1;
        GStackY_28 = GStackY_28 - 1;
        if (GStackY_28 == 0) {
          return (GraphicsTextureSet *)pGVar4;
        }
      }
      pGVar4 = (GraphicsPaletteTextureSourceAsset *)&k_LowAddressLiteral0000002F;
    }
  }
  return (GraphicsTextureSet *)pGVar4;
}

/* Address: 0x00485F90.
   Ownership: graphics/resources/texture.
   Purpose: Frees the texture-set metadata allocation and returns set->sourceAsset. Null input returns null.
*/
GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *releasedTextureSet;
  
  releasedTextureSet = (GraphicsTextureSourceAsset *)0x0;
  if (set != (GraphicsTextureSet *)0x0) {
    releasedTextureSet = (GraphicsTextureSourceAsset *)set;
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
void GraphicsTexture_EvictOldestDeviceTexture(GraphicsTextureResource *exclude)

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
    return;
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
  return;
}

/* Address: 0x0057E870.
   Ownership: graphics/resources/texture.
   Purpose: Stores the texture pointer in the first free entry of the 4096-entry texture-slot array. ABI: CF clear
   means success. CF set means failure; EAX may contain an engine error code.
*/
void GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture)

{
  int slotsRemaining;
  GraphicsTextureResource **slotCursor;
  
  slotsRemaining = 0x1000;
  slotCursor = g_GraphicsTextureSlots;
  do {
    if (*slotCursor == (GraphicsTextureResource *)0x0) {
      *slotCursor = texture;
      return;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return;
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
GraphicsTextureResource * GraphicsTexture_CreateStagingTexture(GraphicsTextureResource *texture)

{
  GraphicsTextureResource *in_EAX;
  uint uVar1;
  TH_LEGACY_HRESULT TVar2;
  GraphicsTextureDownsampleShift GVar3;
  int iVar4;
  uint uVar5;
  DDPIXELFORMAT *pDVar6;
  DDPIXELFORMAT *pDVar7;
  qword qVar8;
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
    qVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(texture->subresourceIndex,texture->sourceAsset)
    ;
    uVar5 = (uint)(qVar8 >> 0x20);
    uVar1 = (uint)qVar8;
    if ((uint)qVar8 < uVar5) {
      uVar1 = uVar5;
    }
    g_SurfaceDesc.dwHeight = uVar1 >> ((byte)g_TextureDownsampleShift & 0x1f);
    GVar3 = g_TextureDownsampleShift;
    do {
      if (0xf < (int)g_SurfaceDesc.dwHeight) break;
      g_SurfaceDesc.dwHeight = g_SurfaceDesc.dwHeight * 2;
      GVar3 = GVar3 - GRAPHICS_TEXTURE_DOWNSAMPLE_2X;
    } while (GVar3 != GRAPHICS_TEXTURE_DOWNSAMPLE_1X);
    texture->downsampleShift = GVar3;
    pDVar6 = texture->pixelFormat;
    pDVar7 = &g_SurfaceDesc.ddpfPixelFormat;
    g_SurfaceDesc.dwWidth = g_SurfaceDesc.dwHeight;
    for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
      pDVar7->dwSize = pDVar6->dwSize;
      pDVar6 = (DDPIXELFORMAT *)&pDVar6->dwFlags;
      pDVar7 = (DDPIXELFORMAT *)&pDVar7->dwFlags;
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
GraphicsTextureResource * GraphicsTexture_ReleaseObjects(GraphicsTextureResource *texture)

{
  GraphicsTextureResource *in_EAX;
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
  return in_EAX;
}
