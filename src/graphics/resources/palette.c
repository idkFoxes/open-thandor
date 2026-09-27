/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/palette.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/palette.h>
#include <thandor/thandor.h>

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
  int subresourceBank;
  uint indexOrColor;
  uint colorOrCombinedCount;
  int remainingCount;
  int remainingPixels;
  uint secondIndex;
  uint *entryCursor;
  int cursorOrBankIndex;
  uint *scanOrBankStart;
  byte *pixelCursor;
  int *bankSlotCursor;
  uint *bankCursor;
  int bankIndex;
  bool emptyBankFound;
  bool banksMerged;

  remainingCount = *(int *)(textureSourceBase + 0xb4) << 8;
  if (remainingCount != 0) {
    entryCursor = (uint *)(textureSourceBase + 0x200);
    do {
      if ((*entryCursor & 0xff000000) == 0) {
        indexOrColor = 0x70707;
      }
      else {
        indexOrColor = *entryCursor | 0x70707;
      }
      *entryCursor = indexOrColor;
      entryCursor = entryCursor + 2;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    remainingCount = *(int *)(textureSourceBase + 0xb0);
    cursorOrBankIndex = textureSourceBase + *(int *)(textureSourceBase + 0xb8);
    if (remainingCount != 0) {
      do {
        subresourceBank = *(int *)(cursorOrBankIndex + 8);
        remainingPixels = *(int *)(cursorOrBankIndex + 0x18) * *(int *)(cursorOrBankIndex + 0x1c);
        if (subresourceBank != -1) {
          pixelCursor = (byte *)(*(int *)(cursorOrBankIndex + 0xc) + textureSourceBase);
          for (; remainingPixels != 0; remainingPixels = remainingPixels + -1) {
            entryCursor = (uint *)(textureSourceBase + 0x200 + subresourceBank * 0x800 + (uint)*pixelCursor * 8);
            *entryCursor = *entryCursor & 0xfff8f8f8;
            pixelCursor = pixelCursor + 1;
          }
        }
        cursorOrBankIndex = cursorOrBankIndex + 0x20;
        remainingCount = remainingCount + -1;
      } while (remainingCount != 0);
      bankSlotCursor = (int *)THANDOR_ADDR(g_GraphicsPaletteBankSlots,0);
      indexOrColor = *(uint *)(textureSourceBase + 0xb4);
      entryCursor = (uint *)(textureSourceBase + 0x200);
      if (0x200 < indexOrColor) {
        indexOrColor = 0x200;
      }
      do {
        cursorOrBankIndex = 0x100;
        remainingCount = 0;
        do {
          if ((*entryCursor & 0x70707) == 0) {
            remainingCount = remainingCount + 1;
          }
          entryCursor = entryCursor + 2;
          cursorOrBankIndex = cursorOrBankIndex + -1;
        } while (cursorOrBankIndex != 0);
        *bankSlotCursor = remainingCount;
        bankSlotCursor = bankSlotCursor + 1;
        indexOrColor = indexOrColor - 1;
      } while (indexOrColor != 0);
      /* Remove every bank without a used color, rescanning from the first bank after each removal. */
      for (;;) {
        bankSlotCursor = (int *)THANDOR_ADDR(g_GraphicsPaletteBankSlots,0);
        remainingCount = *(int *)(textureSourceBase + 0xb4);
        cursorOrBankIndex = 0;
        emptyBankFound = false;
        do {
          if (*bankSlotCursor == 0) {
            emptyBankFound = true;
            break;
          }
          bankSlotCursor = bankSlotCursor + 1;
          cursorOrBankIndex = cursorOrBankIndex + 1;
          remainingCount = remainingCount + -1;
        } while (remainingCount != 0);
        if (!emptyBankFound) break;
        GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
                  (cursorOrBankIndex,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase);
      }
      cursorOrBankIndex = 0;
      remainingCount = *(int *)(textureSourceBase + 0xb4);
      entryCursor = (uint *)(textureSourceBase + 0x200);
      do {
        indexOrColor = 0;
        bankCursor = entryCursor;
        do {
          colorOrCombinedCount = *entryCursor;
          secondIndex = indexOrColor + 1;
          entryCursor = entryCursor + 2;
          scanOrBankStart = entryCursor;
          if ((colorOrCombinedCount & 0x70707) == 0) {
            do {
              if (colorOrCombinedCount == *scanOrBankStart) {
                GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                          (secondIndex,indexOrColor,cursorOrBankIndex,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase)
                ;
                *scanOrBankStart = *scanOrBankStart | 0x70707;
              }
              secondIndex = secondIndex + 1;
              scanOrBankStart = scanOrBankStart + 2;
            } while (secondIndex < 0x100);
          }
          indexOrColor = indexOrColor + 1;
        } while (indexOrColor < 0xff);
        cursorOrBankIndex = cursorOrBankIndex + 1;
        entryCursor = bankCursor + 0x200;
        remainingCount = remainingCount + -1;
      } while (remainingCount != 0);
      /* Merge bank pairs whose combined used colors fit into 256 entries; after a merge, retry the same first
         bank against the remaining ones. */
      indexOrColor = 0;
      for (;;) {
        secondIndex = indexOrColor + 1;
        if (*(uint *)(textureSourceBase + 0xb4) <= secondIndex) break;
        banksMerged = false;
        do {
          colorOrCombinedCount = GraphicsPaletteTextureSource_CountCombinedUsedColors
                            (secondIndex,indexOrColor,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase);
          if (colorOrCombinedCount < 0x101) {
            GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
                      (secondIndex,indexOrColor,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase);
            banksMerged = true;
            break;
          }
          secondIndex = secondIndex + 1;
        } while (secondIndex < *(uint *)(textureSourceBase + 0xb4));
        if (!banksMerged) {
          indexOrColor = indexOrColor + 1;
          if (*(uint *)(textureSourceBase + 0xb4) <= indexOrColor) break;
        }
      }
      bankIndex = 0;
      remainingCount = *(int *)(textureSourceBase + 0xb4);
      entryCursor = (uint *)(textureSourceBase + 0x200);
      do {
        secondIndex = 0;
        indexOrColor = 0;
        bankCursor = entryCursor;
        scanOrBankStart = entryCursor;
        do {
          bankCursor[1] = entryCursor[1];
          colorOrCombinedCount = *entryCursor;
          *bankCursor = colorOrCombinedCount;
          entryCursor = entryCursor + 2;
          if ((colorOrCombinedCount & 0x70707) == 0) {
            GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                      (indexOrColor,secondIndex,bankIndex,(GraphicsTextureSourceHeaderViewBC *)textureSourceBase)
            ;
            bankCursor = bankCursor + 2;
            secondIndex = secondIndex + 1;
          }
          indexOrColor = indexOrColor + 1;
        } while (indexOrColor < 0x100);
        for (cursorOrBankIndex = indexOrColor - secondIndex; cursorOrBankIndex != 0; cursorOrBankIndex = cursorOrBankIndex + -1) {
          *bankCursor = 0;
          bankCursor = bankCursor + 2;
        }
        bankIndex = bankIndex + 1;
        entryCursor = scanOrBankStart + 0x200;
        remainingCount = remainingCount + -1;
      } while (remainingCount != 0);
      return false;
    }
  }
  return true;
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
  PackageLoadEntryEaxCf5 loadResult;
  GraphicsPaletteAssetEaxCf5 validateResult;
  
  loadResult = Package_LoadEntry(pathUtf16);
  loadedPaletteAsset = loadResult.bufferOrError;
  if (!loadResult.carry) {
    validateResult = (*g_GraphicsPaletteAssetValidate)(loadedPaletteAsset);
    validatedPaletteAsset = validateResult.paletteAsset;
    if (!validateResult.carry) {
      return validateResult;
    }
    Resource_Release(loadedPaletteAsset);
    loadedPaletteAsset = validatedPaletteAsset;
  }
  validateResult.carry = true;
  validateResult.paletteAsset = loadedPaletteAsset;
  return validateResult;
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
  GraphicsPaletteAsset *clonedAsset;
  uint sizeOrDwordCount;
  GraphicsPaletteAsset *destinationCursor;
  ArenaAllocEaxCf5 allocResult;
  GraphicsPaletteAssetEaxCf5 validateResult;
  ArenaFreeEaxCf5 freeResult;
  
  sizeOrDwordCount = paletteAsset->allocationSizeBytes;
  allocResult = (*g_MemoryApi.alloc)(sizeOrDwordCount);
  clonedAsset = (GraphicsPaletteAsset *)allocResult.eax;
  if (!allocResult.carry) {
    destinationCursor = clonedAsset;
    for (sizeOrDwordCount = sizeOrDwordCount >> 2; sizeOrDwordCount != 0; sizeOrDwordCount = sizeOrDwordCount - 1) {
      destinationCursor->magic = paletteAsset->magic;
      paletteAsset = (GraphicsPaletteAsset *)&paletteAsset->allocationSizeBytes;
      destinationCursor = (GraphicsPaletteAsset *)&destinationCursor->allocationSizeBytes;
    }
    validateResult = (*g_GraphicsPaletteAssetValidate)(clonedAsset);
    if (!validateResult.carry) {
      return validateResult.paletteAsset;
    }
    freeResult = (*g_MemoryApi.free)(clonedAsset);
    clonedAsset = (GraphicsPaletteAsset *)freeResult.eax;
  }
  return clonedAsset;
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
  GraphicsPaletteAssetEaxCf5 successResult;
  GraphicsPaletteAssetEaxCf5 failureResult;
  
  if (paletteAsset->magic == ASSET_MAGIC_PAL) {
    successResult.carry = false;
    successResult.paletteAsset = paletteAsset;
    return successResult;
  }
  failureResult.carry = true;
  failureResult.paletteAsset = (GraphicsPaletteAsset *)0x35;
  return failureResult;
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
  GraphicsPaletteBankCount appendedBankCount;
  GraphicsAssetAllocationByteSize baseAllocationSize;
  dword bytes;
  int headerCountOrBaseBanks;
  int appendedPaletteBytesOrCount;
  int remainingDwords;
  uint tailDwordCount;
  GraphicsAssetSubresourceCount remainingBaseSubresources;
  GraphicsAssetSubresourceCount appendedSubresourceCount;
  GraphicsPaletteTextureSourceAsset *sourceCursor;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
  byte *byteSourceCursor;
  GraphicsPaletteTextureSourceAsset *destinationCursor;
  ArenaAllocEaxCf5 allocResult;
  GraphicsPaletteTextureSourceEaxCf5 result;
  
  bytes = (baseAsset->allocationSizeBytes + appendedAsset->allocationSizeBytes) - 0x200;
  allocResult = (*g_MemoryApi.alloc)(bytes);
  result.paletteSource = (GraphicsPaletteTextureSourceAsset *)allocResult.eax;
  if (allocResult.carry) {
    result.carry = true;
    return result;
  }
  sourceCursor = baseAsset;
  destinationCursor = result.paletteSource;
  for (headerCountOrBaseBanks = 0x80; headerCountOrBaseBanks != 0; headerCountOrBaseBanks = headerCountOrBaseBanks + -1) {
    destinationCursor->magic = sourceCursor->magic;
    sourceCursor = (GraphicsPaletteTextureSourceAsset *)&sourceCursor->allocationSizeBytes;
    destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
  }
  appendedBankCount = appendedAsset->paletteBankCount;
  appendedSubresourceCount = appendedAsset->subresourceCount;
  destinationCursor[-1].reserved0C = bytes;
  *(GraphicsPaletteBankCount *)destinationCursor[-1].reservedBC_1FF =
       *(int *)destinationCursor[-1].reservedBC_1FF + appendedBankCount;
  destinationCursor[-1].subresourceTableOffset = destinationCursor[-1].subresourceTableOffset + appendedSubresourceCount;
  appendedPaletteBytesOrCount = appendedBankCount * 0x800;
  *(int *)(destinationCursor[-1].reservedBC_1FF + 4) = *(int *)(destinationCursor[-1].reservedBC_1FF + 4) + appendedPaletteBytesOrCount;
  headerCountOrBaseBanks = *(int *)sourceCursor[-1].reservedBC_1FF;
  remainingDwords = headerCountOrBaseBanks << 9;
  if (remainingDwords != 0) {
    for (; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destinationCursor->magic = sourceCursor->magic;
      sourceCursor = (GraphicsPaletteTextureSourceAsset *)&sourceCursor->allocationSizeBytes;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
  }
  paletteEntryCursor = appendedAsset->paletteEntries;
  remainingDwords = appendedAsset->paletteBankCount << 9;
  if (remainingDwords != 0) {
    for (; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destinationCursor->magic = paletteEntryCursor->argb8888;
      paletteEntryCursor = (GraphicsTexturePaletteEntry *)&paletteEntryCursor->framebufferPixel;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
  }
  baseAllocationSize = baseAsset->allocationSizeBytes;
  remainingBaseSubresources = baseAsset->subresourceCount;
  byteSourceCursor = baseAsset->reserved10_AF + (baseAsset->subresourceTableOffset - 0x10);
  do {
    for (remainingDwords = 8; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
      destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
      byteSourceCursor = byteSourceCursor + 4;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
    *(GraphicsAssetSubresourceCount *)(destinationCursor[-1].reservedBC_1FF + 0x138) =
         *(int *)(destinationCursor[-1].reservedBC_1FF + 0x138) + appendedPaletteBytesOrCount + appendedSubresourceCount * 0x20;
    remainingBaseSubresources = remainingBaseSubresources - 1;
  } while (remainingBaseSubresources != 0);
  byteSourceCursor = appendedAsset->reserved10_AF + (appendedAsset->subresourceTableOffset - 0x10);
  appendedSubresourceCount = appendedAsset->subresourceCount;
  do {
    for (appendedPaletteBytesOrCount = 8; appendedPaletteBytesOrCount != 0; appendedPaletteBytesOrCount = appendedPaletteBytesOrCount + -1) {
      destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
      byteSourceCursor = byteSourceCursor + 4;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
    *(GraphicsAssetAllocationByteSize *)(destinationCursor[-1].reservedBC_1FF + 0x138) =
         *(int *)(destinationCursor[-1].reservedBC_1FF + 0x138) + (baseAllocationSize - 0x200);
    if (-1 < *(int *)(destinationCursor[-1].reservedBC_1FF + 0x134)) {
      *(int *)(destinationCursor[-1].reservedBC_1FF + 0x134) =
           *(int *)(destinationCursor[-1].reservedBC_1FF + 0x134) + headerCountOrBaseBanks;
    }
    appendedSubresourceCount = appendedSubresourceCount - 1;
  } while (appendedSubresourceCount != 0);
  byteSourceCursor = baseAsset->reserved10_AF +
            baseAsset->subresourceCount * 0x20 + baseAsset->subresourceTableOffset + -0x10;
  for (tailDwordCount = (baseAsset->allocationSizeBytes - baseAsset->subresourceTableOffset) +
               baseAsset->subresourceCount * -0x20 >> 2; tailDwordCount != 0; tailDwordCount = tailDwordCount - 1) {
    destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
    byteSourceCursor = byteSourceCursor + 4;
    destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
  }
  byteSourceCursor = appendedAsset->reserved10_AF +
            appendedAsset->subresourceCount * 0x20 + appendedAsset->subresourceTableOffset + -0x10;
  for (tailDwordCount = (appendedAsset->allocationSizeBytes - appendedAsset->subresourceTableOffset) +
               appendedAsset->subresourceCount * -0x20 >> 2; tailDwordCount != 0; tailDwordCount = tailDwordCount - 1) {
    destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
    byteSourceCursor = byteSourceCursor + 4;
    destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
  }
  return THANDOR_BITCAST(qword, GraphicsPaletteTextureSourceEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
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
  uint *destinationBankEntries;
  uint packedColor;
  uint destinationColorIndex;
  int remainingPixels;
  uint sourceColorIndex;
  AssetSubresourceCount remainingSubresources;
  uint *sourceEntry;
  word *subresourceEntry;
  uint *destinationEntry;
  byte *pixelCursor;
  
  sourceEntry = (uint *)((int)textureSource + sourcePaletteBank * 0x800 + 0x200);
  destinationBankEntries = (uint *)((int)textureSource + destinationPaletteBank * 0x800 + 0x200);
  /* Build g_GraphicsPaletteRemapBytes: every used source color (marker bits 0x70707 clear) maps to an identical
     destination color, or else is copied into the first free destination entry (marker bits set). When the
     destination bank is full, that remap byte is left unchanged. */
  for (sourceColorIndex = 0; sourceColorIndex < 0x100; sourceColorIndex = sourceColorIndex + 1) {
    packedColor = *sourceEntry;
    if ((packedColor & 0x70707) == 0) {
      destinationEntry = destinationBankEntries;
      for (destinationColorIndex = 0; destinationColorIndex < 0x100;
          destinationColorIndex = destinationColorIndex + 1) {
        if (packedColor == *destinationEntry) break;
        destinationEntry = destinationEntry + 2;
      }
      if (destinationColorIndex < 0x100) {
        *(char *)(sourceColorIndex + THANDOR_ADDR(g_GraphicsPaletteRemapBytes,0)) = (char)destinationColorIndex;
      }
      else {
        destinationEntry = destinationBankEntries;
        for (destinationColorIndex = 0; destinationColorIndex < 0x100;
            destinationColorIndex = destinationColorIndex + 1) {
          if ((*destinationEntry & 0x70707) != 0) break;
          destinationEntry = destinationEntry + 2;
        }
        if (destinationColorIndex < 0x100) {
          *destinationEntry = packedColor;
          destinationEntry[1] = sourceEntry[1];
          *(char *)(sourceColorIndex + THANDOR_ADDR(g_GraphicsPaletteRemapBytes,0)) = (char)destinationColorIndex;
        }
      }
    }
    sourceEntry = sourceEntry + 2;
  }
  remainingSubresources = (textureSource->tableDescriptor).subresourceCount;
  subresourceEntry = (word *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   ((textureSource->tableDescriptor).subresourceTableOffset - 0x28));
  do {
    if (sourcePaletteBank == *(int *)((AssetProducerSourceNames *)(subresourceEntry + 4))->producerName) {
      *(GraphicsPaletteIndex *)((AssetProducerSourceNames *)(subresourceEntry + 4))->producerName =
           destinationPaletteBank;
      pixelCursor = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                *(int *)(subresourceEntry + 6) + -0x28;
      remainingPixels = *(int *)(subresourceEntry + 0xc) * *(int *)(subresourceEntry + 0xe);
      do {
        *pixelCursor = *(byte *)(*pixelCursor + THANDOR_ADDR(g_GraphicsPaletteRemapBytes,0));
        pixelCursor = pixelCursor + 1;
        remainingPixels = remainingPixels + -1;
      } while (remainingPixels != 0);
    }
    subresourceEntry = subresourceEntry + 0x10;
    remainingSubresources = remainingSubresources - 1;
  } while (remainingSubresources != 0);
  GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(sourcePaletteBank,textureSource);
  return;
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
  int remainingPixels;
  AssetSubresourceCount remainingSubresources;
  word *subresourceEntry;
  byte *pixelCursor;
  
  remainingSubresources = (textureSource->tableDescriptor).subresourceCount;
  subresourceEntry = (word *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   ((textureSource->tableDescriptor).subresourceTableOffset - 0x28));
  if (newColorIndex != oldColorIndex) {
    do {
      if (paletteBank == *(int *)((AssetProducerSourceNames *)(subresourceEntry + 4))->producerName) {
        pixelCursor = (textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                 *(int *)(subresourceEntry + 6) + -0x28;
        remainingPixels = *(int *)(subresourceEntry + 0xc) * *(int *)(subresourceEntry + 0xe);
        do {
          if ((byte)oldColorIndex == *pixelCursor) {
            *pixelCursor = (byte)newColorIndex;
          }
          pixelCursor = pixelCursor + 1;
          remainingPixels = remainingPixels + -1;
        } while (remainingPixels != 0);
      }
      subresourceEntry = subresourceEntry + 0x10;
      remainingSubresources = remainingSubresources - 1;
    } while (remainingSubresources != 0);
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
  int remainingCandidateEntries;
  int remainingEntries;
  uint usedColorCount;
  uint *destinationEntry;
  uint *candidateEntry;
  uint *candidateBankCursor;
  
  destinationEntry = (uint *)((int)textureSource + destinationPaletteBank * 0x800 + 0x200);
  candidateBankCursor = (uint *)((int)textureSource + candidatePaletteBank * 0x800 + 0x200);
  usedColorCount = 0;
  /* Used destination colors that the candidate bank does not contain as well... */
  remainingEntries = 0x100;
  do {
    if ((*destinationEntry & 0x70707) == 0) {
      remainingCandidateEntries = 0x100;
      candidateEntry = candidateBankCursor;
      do {
        if (*destinationEntry == *candidateEntry) break;
        candidateEntry = candidateEntry + 2;
        remainingCandidateEntries = remainingCandidateEntries + -1;
      } while (remainingCandidateEntries != 0);
      if (remainingCandidateEntries == 0) {
        usedColorCount = usedColorCount + 1;
      }
    }
    destinationEntry = destinationEntry + 2;
    remainingEntries = remainingEntries + -1;
  } while (remainingEntries != 0);
  /* ...plus every used candidate color. */
  remainingEntries = 0x100;
  do {
    if ((*candidateBankCursor & 0x70707) == 0) {
      usedColorCount = usedColorCount + 1;
    }
    candidateBankCursor = candidateBankCursor + 2;
    remainingEntries = remainingEntries + -1;
  } while (remainingEntries != 0);
  return usedColorCount;
}


/* Address: 0x004AE230.
   Ownership: graphics/resources/palette.
   Purpose: Handles graphics palette texture source remove palette bank and rebase subresources.
*/
void __thandor_void_preserve_eax_ecx
GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
          (GraphicsPaletteIndex paletteIndex,GraphicsTextureSourceHeaderViewBC *textureSource)

{
  AssetPaletteBankCount *paletteBankCountField;
  AssetRelativeOffset *subresourceTableOffsetField;
  AssetAllocationSizeBytes *allocationSizeField;
  uint remainingDwords;
  int bankOffsetOrCount;
  dword *copyCursor;
  word *subresourceEntry;
  dword *sourceDwordCursor;
  dword *destinationDwordCursor;
  AssetSubresourceCount remainingSubresources;
  
  bankOffsetOrCount = paletteIndex * 0x800 + 0x200;
  destinationDwordCursor = (dword *)((int)textureSource + bankOffsetOrCount);
  copyCursor = (dword *)(paletteIndex * 0x800 + 0xa00 + (int)textureSource);
  remainingDwords = ((textureSource->common).allocationSizeBytes - 0x800) - bankOffsetOrCount >> 2;
  if (remainingDwords != 0) {
    for (; remainingDwords != 0; remainingDwords = remainingDwords - 1) {
      *destinationDwordCursor = *copyCursor;
      copyCursor = copyCursor + 1;
      destinationDwordCursor = destinationDwordCursor + 1;
    }
  }
  paletteBankCountField = &(textureSource->tableDescriptor).paletteBankCount;
  *paletteBankCountField = *paletteBankCountField - 1;
  subresourceTableOffsetField = &(textureSource->tableDescriptor).subresourceTableOffset;
  *subresourceTableOffsetField = *subresourceTableOffsetField - 0x800;
  allocationSizeField = &(textureSource->common).allocationSizeBytes;
  *allocationSizeField = *allocationSizeField - 0x800;
  subresourceEntry = (word *)((textureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   ((textureSource->tableDescriptor).subresourceTableOffset - 0x28));
  for (remainingSubresources = (textureSource->tableDescriptor).subresourceCount; remainingSubresources != 0; remainingSubresources = remainingSubresources - 1) {
    *(int *)(subresourceEntry + 6) = *(int *)(subresourceEntry + 6) + -0x800;
    if (paletteIndex < *(int *)((AssetProducerSourceNames *)(subresourceEntry + 4))->producerName) {
      *(int *)((AssetProducerSourceNames *)(subresourceEntry + 4))->producerName =
           *(int *)((AssetProducerSourceNames *)(subresourceEntry + 4))->producerName + -1;
    }
    subresourceEntry = subresourceEntry + 0x10;
  }
  copyCursor = (dword *)(paletteIndex * 4 + THANDOR_ADDR(g_GraphicsPaletteBankSlots,0));
  sourceDwordCursor = (dword *)(paletteIndex * 4 + THANDOR_ADDR(g_GraphicsPaletteBankSlots,0x4));
  bankOffsetOrCount = 0x1ff - paletteIndex;
  if (bankOffsetOrCount != 0) {
    for (; bankOffsetOrCount != 0; bankOffsetOrCount = bankOffsetOrCount + -1) {
      *copyCursor = *sourceDwordCursor;
      sourceDwordCursor = sourceDwordCursor + 1;
      copyCursor = copyCursor + 1;
    }
  }
  return;
}

