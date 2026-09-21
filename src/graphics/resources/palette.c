/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/palette.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/palette.h>

/* Implementation ownership: graphics/resources/palette. */

/* Address: 0x004AE520.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source optimize palette banks and remap indices.
   Local calls: GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank,
   GraphicsPaletteTextureSource_CountCombinedUsedColors,
   GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources,
   GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources.
*/
bool __thandor_cf_preserve_ecx_edx
GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(int textureSourceBase)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint *puVar7;
  int iVar8;
  uint *puVar9;
  byte *pbVar10;
  int *piVar11;
  uint *puVar12;
  int iStack_1c;
  
  iVar4 = *(int *)(textureSourceBase + 0xb4) << 8;
  if (iVar4 != 0) {
    puVar7 = (uint *)(textureSourceBase + 0x200);
    do {
      if ((*puVar7 & 0xff000000) == 0) {
        uVar2 = 0x70707;
      }
      else {
        uVar2 = *puVar7 | 0x70707;
      }
      *puVar7 = uVar2;
      puVar7 = puVar7 + 2;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    iVar4 = *(int *)(textureSourceBase + 0xb0);
    iVar8 = textureSourceBase + *(int *)(textureSourceBase + 0xb8);
    if (iVar4 != 0) {
      do {
        iVar1 = *(int *)(iVar8 + 8);
        iVar5 = *(int *)(iVar8 + 0x18) * *(int *)(iVar8 + 0x1c);
        if (iVar1 != -1) {
          pbVar10 = (byte *)(*(int *)(iVar8 + 0xc) + textureSourceBase);
          for (; iVar5 != 0; iVar5 = iVar5 + -1) {
            puVar7 = (uint *)(textureSourceBase + 0x200 + iVar1 * 0x800 + (uint)*pbVar10 * 8);
            *puVar7 = *puVar7 & 0xfff8f8f8;
            pbVar10 = pbVar10 + 1;
          }
        }
        iVar8 = iVar8 + 0x20;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      piVar11 = (int *)0x4ad930;
      uVar2 = *(uint *)(textureSourceBase + 0xb4);
      puVar7 = (uint *)(textureSourceBase + 0x200);
      if (0x200 < uVar2) {
        uVar2 = 0x200;
      }
      do {
        iVar8 = 0x100;
        iVar4 = 0;
        do {
          if ((*puVar7 & 0x70707) == 0) {
            iVar4 = iVar4 + 1;
          }
          puVar7 = puVar7 + 2;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
        *piVar11 = iVar4;
        piVar11 = piVar11 + 1;
        uVar2 = uVar2 - 1;
      } while (uVar2 != 0);
LAB_004ae640:
      piVar11 = (int *)0x4ad930;
      iVar4 = *(int *)(textureSourceBase + 0xb4);
      iVar8 = 0;
      do {
        if (*piVar11 == 0) goto code_r0x004ae655;
        piVar11 = piVar11 + 1;
        iVar8 = iVar8 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      iVar8 = 0;
      iVar4 = *(int *)(textureSourceBase + 0xb4);
      puVar7 = (uint *)(textureSourceBase + 0x200);
      do {
        uVar2 = 0;
        puVar12 = puVar7;
        do {
          uVar3 = *puVar7;
          uVar6 = uVar2 + 1;
          puVar7 = puVar7 + 2;
          puVar9 = puVar7;
          if ((uVar3 & 0x70707) == 0) {
            do {
              if (uVar3 == *puVar9) {
                GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                          (uVar6,uVar2,iVar8,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase)
                ;
                *puVar9 = *puVar9 | 0x70707;
              }
              uVar6 = uVar6 + 1;
              puVar9 = puVar9 + 2;
            } while (uVar6 < 0x100);
          }
          uVar2 = uVar2 + 1;
        } while (uVar2 < 0xff);
        iVar8 = iVar8 + 1;
        puVar7 = puVar12 + 0x200;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      uVar2 = 0;
LAB_004ae6f0:
      do {
        uVar6 = uVar2 + 1;
        if (*(uint *)(textureSourceBase + 0xb4) <= uVar6) break;
        do {
          uVar3 = GraphicsPaletteTextureSource_CountCombinedUsedColors
                            (uVar6,uVar2,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase);
          if (uVar3 < 0x101) {
            GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
                      (uVar6,uVar2,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase);
            goto LAB_004ae6f0;
          }
          uVar6 = uVar6 + 1;
        } while (uVar6 < *(uint *)(textureSourceBase + 0xb4));
        uVar2 = uVar2 + 1;
      } while (uVar2 < *(uint *)(textureSourceBase + 0xb4));
      iStack_1c = 0;
      iVar4 = *(int *)(textureSourceBase + 0xb4);
      puVar7 = (uint *)(textureSourceBase + 0x200);
      do {
        uVar6 = 0;
        uVar2 = 0;
        puVar12 = puVar7;
        puVar9 = puVar7;
        do {
          puVar12[1] = puVar7[1];
          uVar3 = *puVar7;
          *puVar12 = uVar3;
          puVar7 = puVar7 + 2;
          if ((uVar3 & 0x70707) == 0) {
            GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                      (uVar2,uVar6,iStack_1c,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase)
            ;
            puVar12 = puVar12 + 2;
            uVar6 = uVar6 + 1;
          }
          uVar2 = uVar2 + 1;
        } while (uVar2 < 0x100);
        for (iVar8 = uVar2 - uVar6; iVar8 != 0; iVar8 = iVar8 + -1) {
          *puVar12 = 0;
          puVar12 = puVar12 + 2;
        }
        iStack_1c = iStack_1c + 1;
        puVar7 = puVar9 + 0x200;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      return false;
    }
  }
  return true;
code_r0x004ae655:
  GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
            (iVar8,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase);
  goto LAB_004ae640;
}


/* Address: 0x004AD800.
   Ownership: graphics/resources/palette.
   Purpose: Validates the 'pal' signature, then returns paletteBankCount in ECX. ABI: CF clear means success; CF
   set means invalid input.
*/
void GraphicsPaletteAsset_GetBankCountRegs(GraphicsPaletteAsset *paletteAsset)

{
  return;
}

/* Address: 0x004AD820.
   Ownership: graphics/resources/palette.
   Purpose: Loads one pal asset through Package_LoadEntry, validates it through g_GraphicsPaletteAssetValidate, and
   returns the package-backed allocation. The first two parameters are the ECX/EDX package-loader context; their
   higher-level meanings remain unresolved. ABI: CF clear means success. CF set means loading or validation failed.
   Cross-module calls: Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
GraphicsPaletteAssetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPaletteAsset_LoadPackage(word *pathUtf16)

{
  GraphicsPaletteAsset *loadedPaletteAsset;
  GraphicsPaletteAsset *validatedPaletteAsset;
  PackageLoadEntryEaxCf5 PVar1;
  GraphicsPaletteAssetEaxCf5 GVar2;
  
  PVar1 = Package_LoadEntry(pathUtf16);
  loadedPaletteAsset = PVar1.bufferOrError;
  if (!PVar1.carry) {
    GVar2 = (*g_GraphicsPaletteAssetValidate)(loadedPaletteAsset);
    validatedPaletteAsset = GVar2.paletteAsset;
    if (!GVar2.carry) {
      return GVar2;
    }
    Resource_Release(loadedPaletteAsset);
    loadedPaletteAsset = validatedPaletteAsset;
  }
  GVar2.carry = true;
  GVar2.paletteAsset = loadedPaletteAsset;
  return GVar2;
}


/* Address: 0x004AD860.
   Ownership: graphics/resources/palette.
   Purpose: Resolves the owning allocation through g_GraphicsPaletteAssetResolveAllocationBase, then releases it
   through Resource_Release. Use for assets returned by GraphicsPaletteAsset_LoadPackage. Graphics palette
   lifecycle callback.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *allocation;
  
  allocation = (*g_GraphicsPaletteAssetResolveAllocationBase)(paletteAsset);
  Resource_Release(allocation);
  return;
}


/* Address: 0x004AD880.
   Ownership: graphics/resources/palette.
   Purpose: Allocates paletteAsset->allocationSizeBytes, copies the complete allocation with rep movsd, validates
   the clone, and returns an independently owned heap allocation. ABI: CF clear means success. CF set means
   allocation or validation failed. Graphics palette lifecycle callback.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *arg0;
  uint uVar1;
  GraphicsPaletteAsset *pGVar2;
  ArenaAllocEaxCf5 AVar3;
  GraphicsPaletteAssetEaxCf5 GVar4;
  ArenaFreeEaxCf5 AVar5;
  
  uVar1 = paletteAsset->allocationSizeBytes;
  AVar3 = (*g_MemoryApi.alloc)(uVar1);
  arg0 = (GraphicsPaletteAsset *)AVar3.eax;
  if (!AVar3.carry) {
    pGVar2 = arg0;
    for (uVar1 = uVar1 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
      pGVar2->magic = paletteAsset->magic;
      paletteAsset = (GraphicsPaletteAsset *)&paletteAsset->allocationSizeBytes;
      pGVar2 = (GraphicsPaletteAsset *)&pGVar2->allocationSizeBytes;
    }
    GVar4 = (*g_GraphicsPaletteAssetValidate)(arg0);
    if (!GVar4.carry) {
      return GVar4.paletteAsset;
    }
    AVar5 = (*g_MemoryApi.free)(arg0);
    arg0 = (GraphicsPaletteAsset *)AVar5.eax;
  }
  return arg0;
}


/* Address: 0x004AD8D0.
   Ownership: graphics/resources/palette.
   Purpose: Resolves the owning allocation through g_GraphicsPaletteAssetResolveAllocationBase, then frees it
   through g_MemoryApi.free. Use for assets returned by GraphicsPaletteAsset_Clone. Graphics palette lifecycle
   callback.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *memory;
  
  memory = (*g_GraphicsPaletteAssetResolveAllocationBase)(paletteAsset);
  (*g_MemoryApi.free)(memory);
  return;
}


/* Address: 0x004AD8F0.
   Ownership: graphics/resources/palette.
   Purpose: Validates magic == 0x006C6170. On success returns the input pointer with CF clear. On failure returns
   engine error code 0x35 in EAX with CF set.
*/
GraphicsPaletteAssetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAssetEaxCf5 GVar1;
  GraphicsPaletteAssetEaxCf5 GVar2;
  
  if (paletteAsset->magic == ASSET_MAGIC_PAL) {
    GVar1.carry = false;
    GVar1.paletteAsset = paletteAsset;
    return GVar1;
  }
  GVar2.carry = true;
  GVar2.paletteAsset = (GraphicsPaletteAsset *)0x35;
  return GVar2;
}


/* Address: 0x004AD920.
   Ownership: graphics/resources/palette.
   Purpose: Returns the allocation pointer that owns a pal asset. The current implementation is an identity
   function, but both release services route through this slot.
*/
GraphicsPaletteAsset * __thandor_eax_preserve_ecx_edx
GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset)

{
  return paletteAsset;
}


/* Address: 0x004AE7E0.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source combine assets and rebase offsets.
*/
GraphicsPaletteTextureSourceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets
          (GraphicsPaletteTextureSourceAsset *appendedAsset,
          GraphicsPaletteTextureSourceAsset *baseAsset)

{
  GraphicsPaletteBankCount GVar1;
  GraphicsAssetAllocationByteSize GVar2;
  dword bytes;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  GraphicsAssetSubresourceCount GVar7;
  GraphicsAssetSubresourceCount GVar8;
  GraphicsPaletteTextureSourceAsset *pGVar9;
  GraphicsTexturePaletteEntry *pGVar10;
  byte *pbVar11;
  GraphicsPaletteTextureSourceAsset *pGVar12;
  ArenaAllocEaxCf5 AVar13;
  GraphicsPaletteTextureSourceEaxCf5 GVar14;
  
  bytes = (baseAsset->allocationSizeBytes + appendedAsset->allocationSizeBytes) - 0x200;
  AVar13 = (*g_MemoryApi.alloc)(bytes);
  GVar14.paletteSource = (GraphicsPaletteTextureSourceAsset *)AVar13.eax;
  if (AVar13.carry) {
    GVar14.carry = true;
    return GVar14;
  }
  pGVar9 = baseAsset;
  pGVar12 = GVar14.paletteSource;
  for (iVar3 = 0x80; iVar3 != 0; iVar3 = iVar3 + -1) {
    pGVar12->magic = pGVar9->magic;
    pGVar9 = (GraphicsPaletteTextureSourceAsset *)&pGVar9->allocationSizeBytes;
    pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
  }
  GVar1 = appendedAsset->paletteBankCount;
  GVar8 = appendedAsset->subresourceCount;
  pGVar12[-1].reserved0C = bytes;
  *(GraphicsPaletteBankCount *)pGVar12[-1].reservedBC_1FF =
       *(int *)pGVar12[-1].reservedBC_1FF + GVar1;
  pGVar12[-1].subresourceTableOffset = pGVar12[-1].subresourceTableOffset + GVar8;
  iVar4 = GVar1 * 0x800;
  *(int *)(pGVar12[-1].reservedBC_1FF + 4) = *(int *)(pGVar12[-1].reservedBC_1FF + 4) + iVar4;
  iVar3 = *(int *)pGVar9[-1].reservedBC_1FF;
  iVar5 = iVar3 << 9;
  if (iVar5 != 0) {
    for (; iVar5 != 0; iVar5 = iVar5 + -1) {
      pGVar12->magic = pGVar9->magic;
      pGVar9 = (GraphicsPaletteTextureSourceAsset *)&pGVar9->allocationSizeBytes;
      pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
    }
  }
  pGVar10 = appendedAsset->paletteEntries;
  iVar5 = appendedAsset->paletteBankCount << 9;
  if (iVar5 != 0) {
    for (; iVar5 != 0; iVar5 = iVar5 + -1) {
      pGVar12->magic = pGVar10->argb8888;
      pGVar10 = (GraphicsTexturePaletteEntry *)&pGVar10->framebufferPixel;
      pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
    }
  }
  GVar2 = baseAsset->allocationSizeBytes;
  GVar7 = baseAsset->subresourceCount;
  pbVar11 = baseAsset->reserved10_AF + (baseAsset->subresourceTableOffset - 0x10);
  do {
    for (iVar5 = 8; iVar5 != 0; iVar5 = iVar5 + -1) {
      pGVar12->magic = *(GraphicsPaletteTextureAssetMagic *)pbVar11;
      pbVar11 = pbVar11 + 4;
      pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
    }
    *(GraphicsAssetSubresourceCount *)(pGVar12[-1].reservedBC_1FF + 0x138) =
         *(int *)(pGVar12[-1].reservedBC_1FF + 0x138) + iVar4 + GVar8 * 0x20;
    GVar7 = GVar7 - 1;
  } while (GVar7 != 0);
  pbVar11 = appendedAsset->reserved10_AF + (appendedAsset->subresourceTableOffset - 0x10);
  GVar8 = appendedAsset->subresourceCount;
  do {
    for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
      pGVar12->magic = *(GraphicsPaletteTextureAssetMagic *)pbVar11;
      pbVar11 = pbVar11 + 4;
      pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
    }
    *(GraphicsAssetAllocationByteSize *)(pGVar12[-1].reservedBC_1FF + 0x138) =
         *(int *)(pGVar12[-1].reservedBC_1FF + 0x138) + (GVar2 - 0x200);
    if (-1 < *(int *)(pGVar12[-1].reservedBC_1FF + 0x134)) {
      *(int *)(pGVar12[-1].reservedBC_1FF + 0x134) =
           *(int *)(pGVar12[-1].reservedBC_1FF + 0x134) + iVar3;
    }
    GVar8 = GVar8 - 1;
  } while (GVar8 != 0);
  pbVar11 = baseAsset->reserved10_AF +
            baseAsset->subresourceCount * 0x20 + baseAsset->subresourceTableOffset + -0x10;
  for (uVar6 = (baseAsset->allocationSizeBytes - baseAsset->subresourceTableOffset) +
               baseAsset->subresourceCount * -0x20 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    pGVar12->magic = *(GraphicsPaletteTextureAssetMagic *)pbVar11;
    pbVar11 = pbVar11 + 4;
    pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
  }
  pbVar11 = appendedAsset->reserved10_AF +
            appendedAsset->subresourceCount * 0x20 + appendedAsset->subresourceTableOffset + -0x10;
  for (uVar6 = (appendedAsset->allocationSizeBytes - appendedAsset->subresourceTableOffset) +
               appendedAsset->subresourceCount * -0x20 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    pGVar12->magic = *(GraphicsPaletteTextureAssetMagic *)pbVar11;
    pbVar11 = pbVar11 + 4;
    pGVar12 = (GraphicsPaletteTextureSourceAsset *)&pGVar12->allocationSizeBytes;
  }
  return (GraphicsPaletteTextureSourceEaxCf5)((uint5)AVar13 & 0xffffffff);
}


/* Address: 0x004AE3F0.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source merge palette bank and remap subresources.
   Local calls: GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
          (GraphicsPaletteIndex sourcePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderViewBC *textureSource)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  AssetSubresourceCount AVar6;
  uint *puVar7;
  word *pwVar8;
  uint *puVar9;
  byte *pbVar10;
  
  puVar7 = (uint *)((int)textureSource + sourcePaletteBank * 0x800 + 0x200);
  puVar1 = (uint *)((int)textureSource + destinationPaletteBank * 0x800 + 0x200);
  uVar5 = 0;
  while (uVar2 = *puVar7, (uVar2 & 0x70707) != 0) {
LAB_004ae480:
    uVar5 = uVar5 + 1;
    puVar7 = puVar7 + 2;
    if (0xff < uVar5) {
      AVar6 = (textureSource->tableDescriptor).subresourceCount;
      pwVar8 = (word *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                       ((textureSource->tableDescriptor).subresourceTableOffset - 0x28));
      do {
        if (sourcePaletteBank == *(int *)((AssetProducerSourceNames *)(pwVar8 + 4))->producerName) {
          *(GraphicsPaletteIndex *)((AssetProducerSourceNames *)(pwVar8 + 4))->producerName =
               destinationPaletteBank;
          pbVar10 = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                    *(int *)(pwVar8 + 6) + -0x28;
          iVar4 = *(int *)(pwVar8 + 0xc) * *(int *)(pwVar8 + 0xe);
          do {
            *pbVar10 = *(byte *)(*pbVar10 + 0x4ae130);
            pbVar10 = pbVar10 + 1;
            iVar4 = iVar4 + -1;
          } while (iVar4 != 0);
        }
        pwVar8 = pwVar8 + 0x10;
        AVar6 = AVar6 - 1;
      } while (AVar6 != 0);
      GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
                (sourcePaletteBank,textureSource);
      return;
    }
  }
  uVar3 = 0;
  puVar9 = puVar1;
  do {
    if (uVar2 == *puVar9) goto LAB_004ae478;
    uVar3 = uVar3 + 1;
    puVar9 = puVar9 + 2;
  } while (uVar3 < 0x100);
  uVar3 = 0;
  puVar9 = puVar1;
LAB_004ae450:
  if ((*puVar9 & 0x70707) == 0) goto code_r0x004ae458;
  *puVar9 = uVar2;
  puVar9[1] = puVar7[1];
LAB_004ae478:
  *(char *)(uVar5 + 0x4ae130) = (char)uVar3;
  goto LAB_004ae480;
code_r0x004ae458:
  uVar3 = uVar3 + 1;
  puVar9 = puVar9 + 2;
  if (0xff < uVar3) goto LAB_004ae480;
  goto LAB_004ae450;
}


/* Address: 0x004AE2E0.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source remap color index for palette bank.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
          (uint oldColorIndex,uint newColorIndex,GraphicsPaletteIndex paletteBank,
          GraphicsTextureSourceHeaderViewBC *textureSource)

{
  int iVar1;
  AssetSubresourceCount AVar2;
  word *pwVar3;
  byte *pbVar4;
  
  AVar2 = (textureSource->tableDescriptor).subresourceCount;
  pwVar3 = (word *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   ((textureSource->tableDescriptor).subresourceTableOffset - 0x28));
  if (newColorIndex != oldColorIndex) {
    do {
      if (paletteBank == *(int *)((AssetProducerSourceNames *)(pwVar3 + 4))->producerName) {
        pbVar4 = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                 *(int *)(pwVar3 + 6) + -0x28;
        iVar1 = *(int *)(pwVar3 + 0xc) * *(int *)(pwVar3 + 0xe);
        do {
          if ((byte)oldColorIndex == *pbVar4) {
            *pbVar4 = (byte)newColorIndex;
          }
          pbVar4 = pbVar4 + 1;
          iVar1 = iVar1 + -1;
        } while (iVar1 != 0);
      }
      pwVar3 = pwVar3 + 0x10;
      AVar2 = AVar2 - 1;
    } while (AVar2 != 0);
  }
  return;
}


/* Address: 0x004AE370.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source count combined used colors.
*/
uint __thandor_eax_preserve_ecx_edx
GraphicsPaletteTextureSource_CountCombinedUsedColors
          (GraphicsPaletteIndex candidatePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderViewBC *textureSource)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  
  puVar4 = (uint *)((int)textureSource + destinationPaletteBank * 0x800 + 0x200);
  puVar6 = (uint *)((int)textureSource + candidatePaletteBank * 0x800 + 0x200);
  iVar2 = 0x100;
  uVar3 = 0;
  do {
    if ((*puVar4 & 0x70707) == 0) {
      iVar1 = 0x100;
      puVar5 = puVar6;
      do {
        if (*puVar4 == *puVar5) goto LAB_004ae3c0;
        puVar5 = puVar5 + 2;
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
      uVar3 = uVar3 + 1;
    }
LAB_004ae3c0:
    puVar4 = puVar4 + 2;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      iVar2 = 0x100;
      do {
        if ((*puVar6 & 0x70707) == 0) {
          uVar3 = uVar3 + 1;
        }
        puVar6 = puVar6 + 2;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      return uVar3;
    }
  } while( true );
}


/* Address: 0x004AE230.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source remove palette bank and rebase subresources.
*/
void __thandor_void_preserve_eax_ecx
GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
          (GraphicsPaletteIndex paletteIndex,GraphicsTextureSourceHeaderViewBC *textureSource)

{
  AssetPaletteBankCount *pAVar1;
  AssetRelativeOffset *pAVar2;
  AssetAllocationSizeBytes *pAVar3;
  uint uVar4;
  int iVar5;
  dword *pdVar6;
  word *pwVar7;
  dword *sourceDwordCursor;
  dword *destinationDwordCursor;
  AssetSubresourceCount AVar8;
  
  iVar5 = paletteIndex * 0x800 + 0x200;
  destinationDwordCursor = (dword *)((int)textureSource + iVar5);
  pdVar6 = (dword *)(paletteIndex * 0x800 + 0xa00 + (int)textureSource);
  uVar4 = ((textureSource->common).allocationSizeBytes - 0x800) - iVar5 >> 2;
  if (uVar4 != 0) {
    for (; uVar4 != 0; uVar4 = uVar4 - 1) {
      *destinationDwordCursor = *pdVar6;
      pdVar6 = pdVar6 + 1;
      destinationDwordCursor = destinationDwordCursor + 1;
    }
  }
  pAVar1 = &(textureSource->tableDescriptor).paletteBankCount;
  *pAVar1 = *pAVar1 - 1;
  pAVar2 = &(textureSource->tableDescriptor).subresourceTableOffset;
  *pAVar2 = *pAVar2 - 0x800;
  pAVar3 = &(textureSource->common).allocationSizeBytes;
  *pAVar3 = *pAVar3 - 0x800;
  pwVar7 = (word *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   ((textureSource->tableDescriptor).subresourceTableOffset - 0x28));
  for (AVar8 = (textureSource->tableDescriptor).subresourceCount; AVar8 != 0; AVar8 = AVar8 - 1) {
    *(int *)(pwVar7 + 6) = *(int *)(pwVar7 + 6) + -0x800;
    if (paletteIndex < *(int *)((AssetProducerSourceNames *)(pwVar7 + 4))->producerName) {
      *(int *)((AssetProducerSourceNames *)(pwVar7 + 4))->producerName =
           *(int *)((AssetProducerSourceNames *)(pwVar7 + 4))->producerName + -1;
    }
    pwVar7 = pwVar7 + 0x10;
  }
  pdVar6 = (dword *)(paletteIndex * 4 + 0x4ad930);
  sourceDwordCursor = (dword *)(paletteIndex * 4 + 0x4ad934);
  iVar5 = 0x1ff - paletteIndex;
  if (iVar5 != 0) {
    for (; iVar5 != 0; iVar5 = iVar5 + -1) {
      *pdVar6 = *sourceDwordCursor;
      sourceDwordCursor = sourceDwordCursor + 1;
      pdVar6 = pdVar6 + 1;
    }
  }
  return;
}

