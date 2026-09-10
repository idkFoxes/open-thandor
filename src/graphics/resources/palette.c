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
void GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(int param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint extraout_ECX;
  uint uVar6;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  int extraout_EDX;
  uint *puVar7;
  int iVar8;
  uint *puVar9;
  byte *pbVar10;
  int *piVar11;
  uint *puVar12;
  undefined8 uVar13;
  int iStack_1c;
  
  iVar4 = *(int *)(param_1 + 0xb4) << 8;
  if (iVar4 != 0) {
    puVar7 = (uint *)(param_1 + 0x200);
    do {
      if ((*puVar7 & 0xff000000) == 0) {
        uVar3 = 0x70707;
      }
      else {
        uVar3 = *puVar7 | 0x70707;
      }
      *puVar7 = uVar3;
      puVar7 = puVar7 + 2;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    iVar4 = *(int *)(param_1 + 0xb0);
    iVar8 = param_1 + *(int *)(param_1 + 0xb8);
    if (iVar4 != 0) {
      do {
        iVar1 = *(int *)(iVar8 + 8);
        iVar5 = *(int *)(iVar8 + 0x18) * *(int *)(iVar8 + 0x1c);
        if (iVar1 != -1) {
          pbVar10 = (byte *)(*(int *)(iVar8 + 0xc) + param_1);
          for (; iVar5 != 0; iVar5 = iVar5 + -1) {
            puVar7 = (uint *)(param_1 + 0x200 + iVar1 * 0x800 + (uint)*pbVar10 * 8);
            *puVar7 = *puVar7 & 0xfff8f8f8;
            pbVar10 = pbVar10 + 1;
          }
        }
        iVar8 = iVar8 + 0x20;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      piVar11 = (int *)0x4ad930;
      uVar3 = *(uint *)(param_1 + 0xb4);
      puVar7 = (uint *)(param_1 + 0x200);
      if (0x200 < uVar3) {
        uVar3 = 0x200;
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
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
LAB_004ae640:
      piVar11 = (int *)0x4ad930;
      iVar4 = *(int *)(param_1 + 0xb4);
      iVar8 = 0;
      do {
        if (*piVar11 == 0) goto code_r0x004ae655;
        piVar11 = piVar11 + 1;
        iVar8 = iVar8 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      iVar8 = 0;
      iVar4 = *(int *)(param_1 + 0xb4);
      puVar7 = (uint *)(param_1 + 0x200);
      do {
        uVar3 = 0;
        puVar12 = puVar7;
        do {
          uVar2 = *puVar7;
          uVar13 = CONCAT44(iVar4,uVar2);
          uVar6 = uVar3 + 1;
          puVar7 = puVar7 + 2;
          puVar9 = puVar7;
          if ((uVar2 & 0x70707) == 0) {
            do {
              if ((uint)uVar13 == *puVar9) {
                uVar13 = GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                                   (uVar6,uVar3,iVar8,param_1);
                *puVar9 = *puVar9 | 0x70707;
                uVar6 = extraout_ECX;
              }
              iVar4 = (int)((ulonglong)uVar13 >> 0x20);
              uVar6 = uVar6 + 1;
              puVar9 = puVar9 + 2;
            } while (uVar6 < 0x100);
          }
          uVar3 = uVar3 + 1;
        } while (uVar3 < 0xff);
        iVar8 = iVar8 + 1;
        puVar7 = puVar12 + 0x200;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      uVar3 = 0;
LAB_004ae6f0:
      do {
        uVar6 = uVar3 + 1;
        if (*(uint *)(param_1 + 0xb4) <= uVar6) break;
        do {
          uVar13 = GraphicsPaletteTextureSource_CountCombinedUsedColors(uVar6,uVar3,param_1);
          iVar4 = (int)((ulonglong)uVar13 >> 0x20);
          if ((uint)uVar13 < 0x101) {
            GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
                      (extraout_ECX_00,iVar4,iVar4,extraout_ECX_00,param_1);
            uVar3 = extraout_ECX_01;
            goto LAB_004ae6f0;
          }
          uVar6 = iVar4 + 1;
          uVar3 = extraout_ECX_00;
        } while (uVar6 < *(uint *)(param_1 + 0xb4));
        uVar3 = extraout_ECX_00 + 1;
      } while (uVar3 < *(uint *)(param_1 + 0xb4));
      iStack_1c = 0;
      iVar4 = *(int *)(param_1 + 0xb4);
      puVar7 = (uint *)(param_1 + 0x200);
      do {
        iVar8 = 0;
        uVar3 = 0;
        puVar12 = puVar7;
        puVar9 = puVar7;
        do {
          puVar12[1] = puVar7[1];
          uVar6 = *puVar7;
          *puVar12 = uVar6;
          puVar7 = puVar7 + 2;
          if ((uVar6 & 0x70707) == 0) {
            GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                      (uVar3,iVar8,iStack_1c,param_1);
            puVar12 = puVar12 + 2;
            iVar8 = iVar8 + 1;
            uVar3 = extraout_ECX_02;
            iVar4 = extraout_EDX;
          }
          uVar3 = uVar3 + 1;
        } while (uVar3 < 0x100);
        for (iVar8 = uVar3 - iVar8; iVar8 != 0; iVar8 = iVar8 + -1) {
          *puVar12 = 0;
          puVar12 = puVar12 + 2;
        }
        iStack_1c = iStack_1c + 1;
        puVar7 = puVar9 + 0x200;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      return;
    }
  }
  return;
code_r0x004ae655:
  GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(iVar8,param_1);
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
GraphicsPaletteAsset * __fastcall
GraphicsPaletteAsset_LoadPackage(dword packageContext0,dword packageContext1,word *pathUtf16)

{
  GraphicsPaletteAsset *loadedPaletteAsset;
  GraphicsPaletteAsset *validatedPaletteAsset;
  undefined1 in_CF;
  
  loadedPaletteAsset = Package_LoadEntry(pathUtf16);
  if (!(bool)in_CF) {
    validatedPaletteAsset = (*g_GraphicsPaletteAssetValidate)(loadedPaletteAsset);
    if (!(bool)in_CF) {
      return validatedPaletteAsset;
    }
    loadedPaletteAsset = (GraphicsPaletteAsset *)Resource_Release(loadedPaletteAsset);
  }
  return loadedPaletteAsset;
}

/* Address: 0x004AD860.
   Ownership: graphics/resources/palette.
   Purpose: Resolves the owning allocation through g_GraphicsPaletteAssetResolveAllocationBase, then releases it
   through Resource_Release. Use for assets returned by GraphicsPaletteAsset_LoadPackage. Graphics palette
   lifecycle callback.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset)

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
  GraphicsPaletteAsset *pGVar1;
  uint extraout_ECX;
  uint uVar2;
  undefined1 in_CF;
  bool bVar3;
  
  arg0 = (*g_MemoryApi.alloc)(paletteAsset->allocationSizeBytes);
  if (!(bool)in_CF) {
    bVar3 = (extraout_ECX >> 1 & 1) != 0;
    pGVar1 = arg0;
    for (uVar2 = extraout_ECX >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      pGVar1->magic = paletteAsset->magic;
      paletteAsset = (GraphicsPaletteAsset *)&paletteAsset->allocationSizeBytes;
      pGVar1 = (GraphicsPaletteAsset *)&pGVar1->allocationSizeBytes;
    }
    pGVar1 = (*g_GraphicsPaletteAssetValidate)(arg0);
    if (!bVar3) {
      return pGVar1;
    }
    arg0 = (GraphicsPaletteAsset *)(*g_MemoryApi.free)(arg0);
  }
  return arg0;
}

/* Address: 0x004AD8D0.
   Ownership: graphics/resources/palette.
   Purpose: Resolves the owning allocation through g_GraphicsPaletteAssetResolveAllocationBase, then frees it
   through g_MemoryApi.free. Use for assets returned by GraphicsPaletteAsset_Clone. Graphics palette lifecycle
   callback.
*/
void GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset)

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
GraphicsPaletteAsset * GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset)

{
  if (paletteAsset->magic == ASSET_MAGIC_PAL) {
    return paletteAsset;
  }
  return (GraphicsPaletteAsset *)0x35;
}

/* Address: 0x004AD920.
   Ownership: graphics/resources/palette.
   Purpose: Returns the allocation pointer that owns a pal asset. The current implementation is an identity
   function, but both release services route through this slot.
*/
GraphicsPaletteAsset *
GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset)

{
  return paletteAsset;
}

/* Address: 0x004AE7E0.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source combine assets and rebase offsets.
*/
undefined8
GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets(int param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  undefined4 extraout_ECX;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 in_EDX;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  bool bVar12;
  void *pvVar13;
  
  uVar4 = param_2[1] + *(int *)(param_1 + 4);
  bVar12 = uVar4 < 0x200;
  _pvVar13 = (*g_MemoryApi.alloc)(uVar4 - 0x200);
  iVar9 = (int)((ulonglong)_pvVar13 >> 0x20);
  puVar3 = SUB84(_pvVar13,0);
  if (bVar12) {
    return CONCAT44(in_EDX,puVar3);
  }
  puVar10 = param_2;
  puVar11 = puVar3;
  for (iVar5 = 0x80; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar11 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar11 = puVar11 + 1;
  }
  iVar5 = *(int *)(iVar9 + 0xb4);
  iVar1 = *(int *)(iVar9 + 0xb0);
  puVar11[-0x7f] = extraout_ECX;
  puVar11[-0x53] = puVar11[-0x53] + iVar5;
  puVar11[-0x54] = puVar11[-0x54] + iVar1;
  iVar5 = iVar5 * 0x800;
  puVar11[-0x52] = puVar11[-0x52] + iVar5;
  iVar2 = puVar10[-0x53];
  iVar6 = iVar2 << 9;
  if (iVar6 != 0) {
    for (; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar11 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar11 = puVar11 + 1;
    }
  }
  puVar10 = (undefined4 *)(iVar9 + 0x200);
  iVar6 = *(int *)(iVar9 + 0xb4) << 9;
  if (iVar6 != 0) {
    for (; iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar11 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar11 = puVar11 + 1;
    }
  }
  iVar6 = param_2[1];
  iVar8 = param_2[0x2c];
  puVar10 = (undefined4 *)((int)param_2 + param_2[0x2e]);
  do {
    for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar11 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar11 = puVar11 + 1;
    }
    puVar11[-5] = puVar11[-5] + iVar5 + iVar1 * 0x20;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  puVar10 = (undefined4 *)(*(int *)(iVar9 + 0xb8) + iVar9);
  iVar9 = *(int *)(iVar9 + 0xb0);
  do {
    for (iVar5 = 8; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar11 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar11 = puVar11 + 1;
    }
    puVar11[-5] = puVar11[-5] + iVar6 + -0x200;
    if (-1 < (int)puVar11[-6]) {
      puVar11[-6] = puVar11[-6] + iVar2;
    }
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  puVar10 = (undefined4 *)((int)param_2 + param_2[0x2c] * 0x20 + param_2[0x2e]);
  for (uVar4 = (uint)((param_2[1] - param_2[0x2e]) + param_2[0x2c] * -0x20) >> 2; uVar4 != 0;
      uVar4 = uVar4 - 1) {
    *puVar11 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar11 = puVar11 + 1;
  }
  puVar10 = (undefined4 *)(param_1 + *(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xb0) * 0x20);
  for (uVar4 = (uint)((*(int *)(param_1 + 4) - *(int *)(param_1 + 0xb8)) +
                     *(int *)(param_1 + 0xb0) * -0x20) >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar11 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar11 = puVar11 + 1;
  }
  return CONCAT44(in_EDX,puVar3);
}

/* Address: 0x004AE3F0.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source merge palette bank and remap subresources.
   Local calls: GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources.
*/
void __fastcall
GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
          (undefined4 param_1,undefined4 param_2,int param_3,int param_4,int param_5)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint *puVar7;
  int iVar8;
  uint *puVar9;
  byte *pbVar10;
  
  puVar7 = (uint *)(param_5 + 0x200 + param_3 * 0x800);
  puVar1 = (uint *)(param_5 + 0x200 + param_4 * 0x800);
  uVar5 = 0;
  while (uVar2 = *puVar7, (uVar2 & 0x70707) != 0) {
LAB_004ae480:
    uVar5 = uVar5 + 1;
    puVar7 = puVar7 + 2;
    if (0xff < uVar5) {
      iVar6 = *(int *)(param_5 + 0xb0);
      iVar8 = param_5 + *(int *)(param_5 + 0xb8);
      do {
        if (param_3 == *(int *)(iVar8 + 8)) {
          *(int *)(iVar8 + 8) = param_4;
          pbVar10 = (byte *)(param_5 + *(int *)(iVar8 + 0xc));
          iVar4 = *(int *)(iVar8 + 0x18) * *(int *)(iVar8 + 0x1c);
          do {
            *pbVar10 = *(byte *)(*pbVar10 + 0x4ae130);
            pbVar10 = pbVar10 + 1;
            iVar4 = iVar4 + -1;
          } while (iVar4 != 0);
        }
        iVar8 = iVar8 + 0x20;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
      GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(param_3,param_5);
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
undefined8
GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
          (int param_1,int param_2,int param_3,int param_4)

{
  undefined4 in_EAX;
  int iVar1;
  undefined4 in_EDX;
  int iVar2;
  int iVar3;
  char *pcVar4;
  
  iVar2 = *(int *)(param_4 + 0xb0);
  iVar3 = param_4 + *(int *)(param_4 + 0xb8);
  if (param_2 != param_1) {
    do {
      if (param_3 == *(int *)(iVar3 + 8)) {
        pcVar4 = (char *)(param_4 + *(int *)(iVar3 + 0xc));
        iVar1 = *(int *)(iVar3 + 0x18) * *(int *)(iVar3 + 0x1c);
        do {
          if ((char)param_1 == *pcVar4) {
            *pcVar4 = (char)param_2;
          }
          pcVar4 = pcVar4 + 1;
          iVar1 = iVar1 + -1;
        } while (iVar1 != 0);
      }
      iVar3 = iVar3 + 0x20;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004AE370.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source count combined used colors.
*/
undefined8 GraphicsPaletteTextureSource_CountCombinedUsedColors(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 in_EDX;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  
  puVar4 = (uint *)(param_3 + 0x200 + param_2 * 0x800);
  puVar6 = (uint *)(param_3 + 0x200 + param_1 * 0x800);
  iVar2 = 0x100;
  iVar3 = 0;
  do {
    if ((*puVar4 & 0x70707) == 0) {
      iVar1 = 0x100;
      puVar5 = puVar6;
      do {
        if (*puVar4 == *puVar5) goto LAB_004ae3c0;
        puVar5 = puVar5 + 2;
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
      iVar3 = iVar3 + 1;
    }
LAB_004ae3c0:
    puVar4 = puVar4 + 2;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      iVar2 = 0x100;
      do {
        if ((*puVar6 & 0x70707) == 0) {
          iVar3 = iVar3 + 1;
        }
        puVar6 = puVar6 + 2;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      return CONCAT44(in_EDX,iVar3);
    }
  } while( true );
}

/* Address: 0x004AE230.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source remove palette bank and rebase subresources.
*/
undefined4
GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(int param_1,int param_2)

{
  undefined4 in_EAX;
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  iVar2 = param_1 * 0x800 + 0x200;
  puVar5 = (undefined4 *)(param_2 + iVar2);
  puVar4 = (undefined4 *)(param_1 * 0x800 + 0xa00 + param_2);
  uVar1 = (uint)((*(int *)(param_2 + 4) + -0x800) - iVar2) >> 2;
  if (uVar1 != 0) {
    for (; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
  }
  *(int *)(param_2 + 0xb4) = *(int *)(param_2 + 0xb4) + -1;
  *(int *)(param_2 + 0xb8) = *(int *)(param_2 + 0xb8) + -0x800;
  *(int *)(param_2 + 4) = *(int *)(param_2 + 4) + -0x800;
  iVar3 = param_2 + *(int *)(param_2 + 0xb8);
  for (iVar2 = *(int *)(param_2 + 0xb0); iVar2 != 0; iVar2 = iVar2 + -1) {
    *(int *)(iVar3 + 0xc) = *(int *)(iVar3 + 0xc) + -0x800;
    if (param_1 < *(int *)(iVar3 + 8)) {
      *(int *)(iVar3 + 8) = *(int *)(iVar3 + 8) + -1;
    }
    iVar3 = iVar3 + 0x20;
  }
  puVar5 = (undefined4 *)(param_1 * 4 + 0x4ad930);
  puVar4 = (undefined4 *)(param_1 * 4 + 0x4ad934);
  iVar2 = 0x1ff - param_1;
  if (iVar2 != 0) {
    for (; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
  }
  return in_EAX;
}
