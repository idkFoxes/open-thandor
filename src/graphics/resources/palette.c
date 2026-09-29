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
   Shrinks the palette banks of a palette texture source in place: marks every entry unused, clears the mark
   on each entry a subresource pixel references, removes banks without a used colour, folds duplicate colours
   within a bank, merges bank pairs whose used colours fit into one bank and finally packs the used entries of
   every bank to its front (zeroing the rest), remapping the pixel indices at each step. CF set (true) when the
   source has no palette bank or no subresource. No caller or table reference is known (converter/editor code left
   in the game).
*/
bool GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(int textureSourceBase)

{
  int subresourceBank;
  uint32_t indexOrColor;
  uint32_t colorOrCombinedCount;
  int remainingCount;
  int remainingPixels;
  uint32_t secondIndex;
  uint32_t *entryCursor;
  int cursorOrBankIndex;
  uint32_t *scanOrBankStart;
  uint8_t *pixelCursor;
  int *bankSlotCursor;
  uint32_t *bankCursor;
  int bankIndex;
  bool emptyBankFound;
  bool banksMerged;

  /* textureSourceBase is a GraphicsTextureSourceHeaderView address; the banks start at
     GRAPHICS_PALETTE_BANKS_OFFSET, entries are 8 bytes (colour, second dword) */
  remainingCount = ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount << 8;
  if (remainingCount != 0) {
    /* mark every entry unused; fully transparent colours become plain black */
    entryCursor = (uint32_t *)(textureSourceBase + GRAPHICS_PALETTE_BANKS_OFFSET);
    do {
      if ((*entryCursor & 0xff000000) == 0) {
        indexOrColor = GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
      }
      else {
        indexOrColor = *entryCursor | GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
      }
      *entryCursor = indexOrColor;
      entryCursor = entryCursor + 2;
      remainingCount--;
    } while (remainingCount != 0);
    remainingCount = ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.subresourceCount;
    cursorOrBankIndex = textureSourceBase +
                        ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.subresourceTableOffset;
    if (remainingCount != 0) {
      /* clear the mark on every entry a pixel uses (cursorOrBankIndex walks the GraphicsTextureSourceEntry
         table; paletteIndex -1: no bank) */
      do {
        subresourceBank = ((GraphicsTextureSourceEntry *)cursorOrBankIndex)->paletteIndex;
        remainingPixels = ((GraphicsTextureSourceEntry *)cursorOrBankIndex)->pixelWidth *
                          ((GraphicsTextureSourceEntry *)cursorOrBankIndex)->pixelHeight;
        if (subresourceBank != -1) {
          pixelCursor = (uint8_t *)(((GraphicsTextureSourceEntry *)cursorOrBankIndex)->dataOffset + textureSourceBase);
          for (; remainingPixels != 0; remainingPixels--) {
            entryCursor = (uint32_t *)(textureSourceBase + GRAPHICS_PALETTE_BANKS_OFFSET +
                                       subresourceBank * GRAPHICS_PALETTE_BANK_BYTES + (uint32_t)*pixelCursor * 8);
            *entryCursor = *entryCursor & ~GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
            pixelCursor++;
          }
        }
        cursorOrBankIndex = cursorOrBankIndex + GFX_SUBRESOURCE_RECORD_SIZE;
        remainingCount--;
      } while (remainingCount != 0);
      /* count the used colours of each bank (at most GRAPHICS_PALETTE_BANK_SLOT_CAPACITY banks) */
      bankSlotCursor = (int *)g_GraphicsPaletteBankSlots;
      indexOrColor = ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount;
      entryCursor = (uint32_t *)(textureSourceBase + GRAPHICS_PALETTE_BANKS_OFFSET);
      if (GRAPHICS_PALETTE_BANK_SLOT_CAPACITY < indexOrColor) {
        indexOrColor = GRAPHICS_PALETTE_BANK_SLOT_CAPACITY;
      }
      do {
        cursorOrBankIndex = GRAPHICS_PALETTE_BANK_ENTRIES;
        remainingCount = 0;
        do {
          if ((*entryCursor & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
            remainingCount++;
          }
          entryCursor = entryCursor + 2;
          cursorOrBankIndex--;
        } while (cursorOrBankIndex != 0);
        *bankSlotCursor = remainingCount;
        bankSlotCursor++;
        indexOrColor--;
      } while (indexOrColor != 0);
      /* Remove every bank without a used color, rescanning from the first bank after each removal. */
      for (;;) {
        bankSlotCursor = (int *)g_GraphicsPaletteBankSlots;
        remainingCount = ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount;
        cursorOrBankIndex = 0;
        emptyBankFound = false;
        do {
          if (*bankSlotCursor == 0) {
            emptyBankFound = true;
            break;
          }
          bankSlotCursor++;
          cursorOrBankIndex++;
          remainingCount--;
        } while (remainingCount != 0);
        if (!emptyBankFound) break;
        GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
                  (cursorOrBankIndex,(GraphicsTextureSourceHeaderView *)textureSourceBase);
      }
      /* fold duplicates inside each bank: pixels using a later copy of a used colour move to the first one,
         and the copy is marked unused */
      cursorOrBankIndex = 0;
      remainingCount = ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount;
      entryCursor = (uint32_t *)(textureSourceBase + GRAPHICS_PALETTE_BANKS_OFFSET);
      do {
        indexOrColor = 0;
        bankCursor = entryCursor;
        do {
          colorOrCombinedCount = *entryCursor;
          secondIndex = indexOrColor + 1;
          entryCursor = entryCursor + 2;
          scanOrBankStart = entryCursor;
          if ((colorOrCombinedCount & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
            do {
              if (colorOrCombinedCount == *scanOrBankStart) {
                GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                          (secondIndex,indexOrColor,cursorOrBankIndex,
                           (GraphicsTextureSourceHeaderView *)textureSourceBase);
                *scanOrBankStart = *scanOrBankStart | GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
              }
              secondIndex++;
              scanOrBankStart = scanOrBankStart + 2;
            } while (secondIndex < GRAPHICS_PALETTE_BANK_ENTRIES);
          }
          indexOrColor++;
        } while (indexOrColor < 0xff);
        cursorOrBankIndex++;
        entryCursor = bankCursor + GRAPHICS_PALETTE_BANK_BYTES / 4; /* next bank */
        remainingCount--;
      } while (remainingCount != 0);
      /* Merge bank pairs whose combined used colors fit into 256 entries; after a merge, retry the same first
         bank against the remaining ones. */
      indexOrColor = 0;
      for (;;) {
        secondIndex = indexOrColor + 1;
        if (((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount <= secondIndex) {
          break;
        }
        banksMerged = false;
        do {
          colorOrCombinedCount = GraphicsPaletteTextureSource_CountCombinedUsedColors
                            (secondIndex,indexOrColor,(GraphicsTextureSourceHeaderView *)textureSourceBase);
          if (colorOrCombinedCount < GRAPHICS_PALETTE_BANK_ENTRIES + 1) {
            GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
                      (secondIndex,indexOrColor,(GraphicsTextureSourceHeaderView *)textureSourceBase);
            banksMerged = true;
            break;
          }
          secondIndex++;
        } while (secondIndex < ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount);
        if (!banksMerged) {
          indexOrColor++;
          if (((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount <= indexOrColor) {
            break;
          }
        }
      }
      /* pack the used entries of each bank to its front and zero the colours of the rest */
      bankIndex = 0;
      remainingCount = ((GraphicsTextureSourceHeaderView *)textureSourceBase)->tableDescriptor.paletteBankCount;
      entryCursor = (uint32_t *)(textureSourceBase + GRAPHICS_PALETTE_BANKS_OFFSET);
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
          if ((colorOrCombinedCount & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
            GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
                      (indexOrColor,secondIndex,bankIndex,(GraphicsTextureSourceHeaderView *)textureSourceBase);
            bankCursor = bankCursor + 2;
            secondIndex++;
          }
          indexOrColor++;
        } while (indexOrColor < GRAPHICS_PALETTE_BANK_ENTRIES);
        for (cursorOrBankIndex = indexOrColor - secondIndex; cursorOrBankIndex != 0; cursorOrBankIndex--) {
          *bankCursor = 0;
          bankCursor = bankCursor + 2;
        }
        bankIndex++;
        entryCursor = scanOrBankStart + GRAPHICS_PALETTE_BANK_BYTES / 4;
        remainingCount--;
      } while (remainingCount != 0);
      return false;
    }
  }
  return true;
}


/* Address: 0x004AD800.
   In the original: checks the 'pal' signature and returns paletteBankCount in ECX with CF clear, CF set for a
   wrong signature. The C body is empty (the register result has no C form here); no caller or table reference
   is known.
*/
void GraphicsPaletteAsset_GetBankCountRegs(GraphicsPaletteAsset *paletteAsset)

{
  return;
}

/* Address: 0x004AD820.
   Loads a 'pal' palette asset from pathUtf16 (Package_LoadEntry) and validates it through
   g_GraphicsPaletteAssetValidate; an invalid asset is released again. CF set with the load or validation error.
   Installed as g_GraphicsPaletteAssetLoadPackage (used by the army graphics and frontend palette loaders).
*/
PaletteAssetResult GraphicsPaletteAsset_LoadPackage(uint16_t *pathUtf16)

{
  GraphicsPaletteAsset *loadedPaletteAsset;
  GraphicsPaletteAsset *validateErrorOrAsset;
  PackageLoadResult loadResult;
  PaletteAssetResult validateResult;
  
  loadResult = Package_LoadEntry(pathUtf16);
  loadedPaletteAsset = loadResult.bufferOrError;
  if (!loadResult.failed) {
    validateResult = g_GraphicsPaletteAssetValidate(loadedPaletteAsset);
    validateErrorOrAsset = validateResult.paletteAsset;
    if (!validateResult.failed) {
      return validateResult;
    }
    Resource_Release(loadedPaletteAsset);
    loadedPaletteAsset = validateErrorOrAsset;
  }
  validateResult.failed = true;
  validateResult.paletteAsset = loadedPaletteAsset;
  return validateResult;
}


/* Address: 0x004AD860.
   Releases a palette asset loaded by GraphicsPaletteAsset_LoadPackage: resolves its allocation through
   g_GraphicsPaletteAssetResolveAllocationBase and hands it to Resource_Release. Installed as
   g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage.
*/
void GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *allocation;
  
  allocation = g_GraphicsPaletteAssetResolveAllocationBase(paletteAsset);
  Resource_Release(allocation);
  return;
}


/* Address: 0x004AD880.
   Makes an independently owned heap copy of a palette asset (allocationSizeBytes, copied dword by dword) and
   validates it; an invalid copy is freed again and the free result returned. Installed as
   g_GraphicsPaletteAssetLifecycleCallbacks3.clone.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *clonedAsset;
  uint32_t sizeOrDwordCount;
  GraphicsPaletteAsset *destinationCursor;
  ArenaAllocResult allocResult;
  PaletteAssetResult validateResult;
  ArenaFreeResult freeResult;
  
  sizeOrDwordCount = paletteAsset->allocationSizeBytes;
  allocResult = g_MemoryApi.alloc(sizeOrDwordCount);
  clonedAsset = (GraphicsPaletteAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    destinationCursor = clonedAsset;
    for (sizeOrDwordCount = sizeOrDwordCount >> 2; sizeOrDwordCount != 0; sizeOrDwordCount--) {
      destinationCursor->magic = paletteAsset->magic;
      paletteAsset = (GraphicsPaletteAsset *)&paletteAsset->allocationSizeBytes;
      destinationCursor = (GraphicsPaletteAsset *)&destinationCursor->allocationSizeBytes;
    }
    validateResult = g_GraphicsPaletteAssetValidate(clonedAsset);
    if (!validateResult.failed) {
      return validateResult.paletteAsset;
    }
    freeResult = g_MemoryApi.free(clonedAsset);
    clonedAsset = (GraphicsPaletteAsset *)freeResult.valueOrError;
  }
  return clonedAsset;
}


/* Address: 0x004AD8D0.
   Frees a palette asset made by GraphicsPaletteAsset_Clone: resolves its allocation through
   g_GraphicsPaletteAssetResolveAllocationBase and frees it with g_MemoryApi.free. Installed as
   g_GraphicsPaletteAssetLifecycleCallbacks3.releaseClone.
*/
void GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *allocation;
  
  allocation = g_GraphicsPaletteAssetResolveAllocationBase(paletteAsset);
  g_MemoryApi.free(allocation);
  return;
}


/* Address: 0x004AD8F0.
   Accepts paletteAsset when it starts with the 'pal' signature (CF clear, pointer returned), otherwise CF set
   with FATAL_ERROR_PALETTE_ASSET_INVALID. Installed as g_GraphicsPaletteAssetValidate.
*/
PaletteAssetResult GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset)

{
  PaletteAssetResult successResult;
  PaletteAssetResult failureResult;
  
  if (paletteAsset->magic == ASSET_MAGIC_PAL) {
    successResult.failed = false;
    successResult.paletteAsset = paletteAsset;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.paletteAsset = (GraphicsPaletteAsset *)FATAL_ERROR_PALETTE_ASSET_INVALID;
  return failureResult;
}


/* Address: 0x004AD920.
   Returns the allocation that owns a palette asset, which is the asset itself; both release callbacks go
   through this slot. Installed as g_GraphicsPaletteAssetResolveAllocationBase.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset)

{
  return paletteAsset;
}


/* Address: 0x004AE7E0.
   Builds a new palette texture source from baseAsset followed by appendedAsset: one header (base's, with the
   size and the bank and subresource counts summed), base banks, appended banks, base subresource entries,
   appended entries, base pixel data, appended pixel data. Pixel offsets of both entry sets and the bank index
   of appended entries are rebased. CF set with the arena error when the allocation fails.
   No caller or table reference is known (converter/editor code left in the game).
*/
PaletteTextureSourceResult GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets
          (GraphicsPaletteTextureSourceAsset *appendedAsset,
          GraphicsPaletteTextureSourceAsset *baseAsset)

{
  GraphicsPaletteBankCount appendedBankCount;
  GraphicsAssetAllocationByteSize baseAllocationSize;
  uint32_t bytes;
  int headerCountOrBaseBanks;
  int appendedPaletteBytesOrCount;
  int remainingDwords;
  uint32_t tailDwordCount;
  GraphicsAssetSubresourceCount remainingBaseSubresources;
  GraphicsAssetSubresourceCount appendedSubresourceCount;
  GraphicsPaletteTextureSourceAsset *sourceCursor;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
  uint8_t *byteSourceCursor;
  GraphicsPaletteTextureSourceAsset *destinationCursor;
  ArenaAllocResult allocResult;
  PaletteTextureSourceResult result;
  
  bytes = (baseAsset->allocationSizeBytes + appendedAsset->allocationSizeBytes) - GRAPHICS_PALETTE_BANKS_OFFSET;
  allocResult = g_MemoryApi.alloc(bytes);
  result.paletteSource = (GraphicsPaletteTextureSourceAsset *)allocResult.payloadOrError;
  if (allocResult.failed) {
    result.failed = true;
    return result;
  }
  sourceCursor = baseAsset;
  destinationCursor = result.paletteSource;
  /* the header */
  for (headerCountOrBaseBanks = GRAPHICS_PALETTE_BANKS_OFFSET / 4; headerCountOrBaseBanks != 0;
       headerCountOrBaseBanks--) {
    destinationCursor->magic = sourceCursor->magic;
    sourceCursor = (GraphicsPaletteTextureSourceAsset *)&sourceCursor->allocationSizeBytes;
    destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
  }
  /* After the header copy destinationCursor is result + GRAPHICS_PALETTE_BANKS_OFFSET, so the header is
     addressed back from it. Inside the entry loops ((GraphicsTextureSourceEntry *)destinationCursor)[-1] is
     the entry just copied. */
  appendedBankCount = appendedAsset->paletteBankCount;
  appendedSubresourceCount = appendedAsset->subresourceCount;
  /* combined header = destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET */
  ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->common.allocationSizeBytes = bytes;
  ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->tableDescriptor.paletteBankCount =
       ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->tableDescriptor.paletteBankCount + appendedBankCount;
  ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->tableDescriptor.subresourceCount =
       ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->tableDescriptor.subresourceCount + appendedSubresourceCount;
  appendedPaletteBytesOrCount = appendedBankCount * GRAPHICS_PALETTE_BANK_BYTES;
  ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->tableDescriptor.subresourceTableOffset =
       ((GraphicsTextureSourceHeaderView *)((uint8_t *)destinationCursor - GRAPHICS_PALETTE_BANKS_OFFSET))
       ->tableDescriptor.subresourceTableOffset + appendedPaletteBytesOrCount;
  headerCountOrBaseBanks = ((GraphicsTextureSourceHeaderView *)((uint8_t *)sourceCursor -
                                                                GRAPHICS_PALETTE_BANKS_OFFSET))
                               ->tableDescriptor.paletteBankCount;
  remainingDwords = headerCountOrBaseBanks * (GRAPHICS_PALETTE_BANK_BYTES / 4);
  if (remainingDwords != 0) {
    for (; remainingDwords != 0; remainingDwords--) {
      destinationCursor->magic = sourceCursor->magic;
      sourceCursor = (GraphicsPaletteTextureSourceAsset *)&sourceCursor->allocationSizeBytes;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
  }
  paletteEntryCursor = appendedAsset->paletteEntries;
  remainingDwords = appendedAsset->paletteBankCount * (GRAPHICS_PALETTE_BANK_BYTES / 4);
  if (remainingDwords != 0) {
    for (; remainingDwords != 0; remainingDwords--) {
      destinationCursor->magic = paletteEntryCursor->argb8888;
      paletteEntryCursor = (GraphicsTexturePaletteEntry *)&paletteEntryCursor->framebufferPixel;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
  }
  baseAllocationSize = baseAsset->allocationSizeBytes;
  remainingBaseSubresources = baseAsset->subresourceCount;
  byteSourceCursor = (uint8_t *)baseAsset + baseAsset->subresourceTableOffset;
  do {
    for (remainingDwords = GFX_SUBRESOURCE_RECORD_SIZE / 4; remainingDwords != 0; remainingDwords--) {
      destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
      byteSourceCursor = byteSourceCursor + 4;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
    ((GraphicsTextureSourceEntry *)destinationCursor)[-1].dataOffset =
         ((GraphicsTextureSourceEntry *)destinationCursor)[-1].dataOffset + appendedPaletteBytesOrCount +
         appendedSubresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
    remainingBaseSubresources--;
  } while (remainingBaseSubresources != 0);
  byteSourceCursor = (uint8_t *)appendedAsset + appendedAsset->subresourceTableOffset;
  appendedSubresourceCount = appendedAsset->subresourceCount;
  do {
    for (appendedPaletteBytesOrCount = GFX_SUBRESOURCE_RECORD_SIZE / 4; appendedPaletteBytesOrCount != 0;
         appendedPaletteBytesOrCount--) {
      destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
      byteSourceCursor = byteSourceCursor + 4;
      destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
    }
    ((GraphicsTextureSourceEntry *)destinationCursor)[-1].dataOffset =
         ((GraphicsTextureSourceEntry *)destinationCursor)[-1].dataOffset +
         (baseAllocationSize - GRAPHICS_PALETTE_BANKS_OFFSET);
    if (-1 < ((GraphicsTextureSourceEntry *)destinationCursor)[-1].paletteIndex) { /* entries without a bank keep -1 */
      ((GraphicsTextureSourceEntry *)destinationCursor)[-1].paletteIndex =
           ((GraphicsTextureSourceEntry *)destinationCursor)[-1].paletteIndex + headerCountOrBaseBanks;
    }
    appendedSubresourceCount--;
  } while (appendedSubresourceCount != 0);
  /* the pixel data behind each entry table */
  byteSourceCursor = (uint8_t *)baseAsset +
            baseAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE + baseAsset->subresourceTableOffset;
  for (tailDwordCount = (baseAsset->allocationSizeBytes - baseAsset->subresourceTableOffset) +
               baseAsset->subresourceCount * -GFX_SUBRESOURCE_RECORD_SIZE >> 2; tailDwordCount != 0; tailDwordCount--) {
    destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
    byteSourceCursor = byteSourceCursor + 4;
    destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
  }
  byteSourceCursor = (uint8_t *)appendedAsset +
            appendedAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE + appendedAsset->subresourceTableOffset;
  for (tailDwordCount = (appendedAsset->allocationSizeBytes - appendedAsset->subresourceTableOffset) +
               appendedAsset->subresourceCount * -GFX_SUBRESOURCE_RECORD_SIZE >> 2; tailDwordCount != 0;
       tailDwordCount--) {
    destinationCursor->magic = *(GraphicsPaletteTextureAssetMagic *)byteSourceCursor;
    byteSourceCursor = byteSourceCursor + 4;
    destinationCursor = (GraphicsPaletteTextureSourceAsset *)&destinationCursor->allocationSizeBytes;
  }
  return THANDOR_BITCAST(uint64_t, PaletteTextureSourceResult,
                         THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xffffffff);
}


/* Address: 0x004AE3F0.
   Moves the used colours of bank sourcePaletteBank into bank destinationPaletteBank (reusing identical colours,
   otherwise taking free entries), rewrites the pixels of every subresource that used the source bank to the
   destination bank and its new indices, then removes the source bank. Called by
   GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices once the pair is known to fit.
*/
void GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
          (GraphicsPaletteIndex sourcePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderView *textureSource)

{
  uint32_t *destinationBankEntries;
  uint32_t packedColor;
  uint32_t destinationColorIndex;
  int remainingPixels;
  uint32_t sourceColorIndex;
  AssetSubresourceCount remainingSubresources;
  uint32_t *sourceEntry;
  GraphicsTextureSourceEntry *subresourceEntry;
  uint32_t *destinationEntry;
  uint8_t *pixelCursor;
  
  sourceEntry = (uint32_t *)((uint8_t *)textureSource + sourcePaletteBank * GRAPHICS_PALETTE_BANK_BYTES +
                             GRAPHICS_PALETTE_BANKS_OFFSET);
  destinationBankEntries = (uint32_t *)((uint8_t *)textureSource + destinationPaletteBank * GRAPHICS_PALETTE_BANK_BYTES +
                                        GRAPHICS_PALETTE_BANKS_OFFSET);
  /* Build g_GraphicsPaletteRemapBytes: every used source color (marker bits 0x70707 clear) maps to an identical
     destination color, or else is copied into the first free destination entry (marker bits set). When the
     destination bank is full, that remap byte is left unchanged. */
  for (sourceColorIndex = 0; sourceColorIndex < GRAPHICS_PALETTE_BANK_ENTRIES; sourceColorIndex++) {
    packedColor = *sourceEntry;
    if ((packedColor & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      destinationEntry = destinationBankEntries;
      for (destinationColorIndex = 0; destinationColorIndex < GRAPHICS_PALETTE_BANK_ENTRIES;
          destinationColorIndex++) {
        if (packedColor == *destinationEntry) break;
        destinationEntry = destinationEntry + 2;
      }
      if (destinationColorIndex < GRAPHICS_PALETTE_BANK_ENTRIES) {
        g_GraphicsPaletteRemapBytes[sourceColorIndex] = (uint8_t)destinationColorIndex;
      }
      else {
        destinationEntry = destinationBankEntries;
        for (destinationColorIndex = 0; destinationColorIndex < GRAPHICS_PALETTE_BANK_ENTRIES;
            destinationColorIndex++) {
          if ((*destinationEntry & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) != 0) break;
          destinationEntry = destinationEntry + 2;
        }
        if (destinationColorIndex < GRAPHICS_PALETTE_BANK_ENTRIES) {
          *destinationEntry = packedColor;
          destinationEntry[1] = sourceEntry[1];
          g_GraphicsPaletteRemapBytes[sourceColorIndex] = (uint8_t)destinationColorIndex;
        }
      }
    }
    sourceEntry = sourceEntry + 2;
  }
  /* the subresource table (&buildMetadata.reserved28_2F - 0x28 is the asset base) */
  remainingSubresources = (textureSource->tableDescriptor).subresourceCount;
  subresourceEntry = (GraphicsTextureSourceEntry *)((textureSource->common).buildMetadata.reserved28_2F +
                   ((textureSource->tableDescriptor).subresourceTableOffset - GFX_ASSET_ANCHOR28_OFFSET));
  do {
    if (sourcePaletteBank == subresourceEntry->paletteIndex) {
      subresourceEntry->paletteIndex = destinationPaletteBank;
      pixelCursor = (uint8_t *)textureSource + subresourceEntry->dataOffset;
      remainingPixels = subresourceEntry->pixelWidth * subresourceEntry->pixelHeight;
      do {
        *pixelCursor = g_GraphicsPaletteRemapBytes[*pixelCursor];
        pixelCursor++;
        remainingPixels--;
      } while (remainingPixels != 0);
    }
    subresourceEntry++;
    remainingSubresources--;
  } while (remainingSubresources != 0);
  GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(sourcePaletteBank,textureSource);
  return;
}


/* Address: 0x004AE2E0.
   Replaces colour index oldColorIndex by newColorIndex in the pixels of every subresource that uses palette
   bank paletteBank (nothing to do when both are equal). Called by
   GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices.
*/
void GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
          (uint32_t oldColorIndex,uint32_t newColorIndex,GraphicsPaletteIndex paletteBank,
          GraphicsTextureSourceHeaderView *textureSource)

{
  int remainingPixels;
  AssetSubresourceCount remainingSubresources;
  GraphicsTextureSourceEntry *subresourceEntry;
  uint8_t *pixelCursor;
  
  remainingSubresources = (textureSource->tableDescriptor).subresourceCount;
  subresourceEntry = (GraphicsTextureSourceEntry *)((uint8_t *)textureSource +
                                                    (textureSource->tableDescriptor).subresourceTableOffset);
  if (newColorIndex != oldColorIndex) {
    do {
      if (paletteBank == subresourceEntry->paletteIndex) {
        pixelCursor = (uint8_t *)textureSource + subresourceEntry->dataOffset;
        remainingPixels = subresourceEntry->pixelWidth * subresourceEntry->pixelHeight;
        do {
          if ((uint8_t)oldColorIndex == *pixelCursor) {
            *pixelCursor = (uint8_t)newColorIndex;
          }
          pixelCursor++;
          remainingPixels--;
        } while (remainingPixels != 0);
      }
      subresourceEntry++;
      remainingSubresources--;
    } while (remainingSubresources != 0);
  }
  return;
}


/* Address: 0x004AE370.
   Returns how many entries one bank would need to hold the used colours of both destinationPaletteBank and
   candidatePaletteBank (colours present in both counted once). Called by
   GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices to find bank pairs that can be merged.
*/
uint32_t GraphicsPaletteTextureSource_CountCombinedUsedColors
          (GraphicsPaletteIndex candidatePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderView *textureSource)

{
  int remainingCandidateEntries;
  int remainingEntries;
  uint32_t usedColorCount;
  uint32_t *destinationEntry;
  uint32_t *candidateEntry;
  uint32_t *candidateBankCursor;
  
  destinationEntry = (uint32_t *)((uint8_t *)textureSource + destinationPaletteBank * GRAPHICS_PALETTE_BANK_BYTES +
                                  GRAPHICS_PALETTE_BANKS_OFFSET);
  candidateBankCursor = (uint32_t *)((uint8_t *)textureSource + candidatePaletteBank * GRAPHICS_PALETTE_BANK_BYTES +
                                     GRAPHICS_PALETTE_BANKS_OFFSET);
  usedColorCount = 0;
  /* Used destination colors that the candidate bank does not contain as well... */
  remainingEntries = GRAPHICS_PALETTE_BANK_ENTRIES;
  do {
    if ((*destinationEntry & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      remainingCandidateEntries = GRAPHICS_PALETTE_BANK_ENTRIES;
      candidateEntry = candidateBankCursor;
      do {
        if (*destinationEntry == *candidateEntry) break;
        candidateEntry = candidateEntry + 2;
        remainingCandidateEntries--;
      } while (remainingCandidateEntries != 0);
      if (remainingCandidateEntries == 0) {
        usedColorCount++;
      }
    }
    destinationEntry = destinationEntry + 2;
    remainingEntries--;
  } while (remainingEntries != 0);
  /* ...plus every used candidate color. */
  remainingEntries = GRAPHICS_PALETTE_BANK_ENTRIES;
  do {
    if ((*candidateBankCursor & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      usedColorCount++;
    }
    candidateBankCursor = candidateBankCursor + 2;
    remainingEntries--;
  } while (remainingEntries != 0);
  return usedColorCount;
}


/* Address: 0x004AE230.
   Deletes palette bank paletteIndex: moves everything behind it 0x800 bytes down, lowers the bank count, the
   subresource table offset, the allocation size and every subresource's pixel offset accordingly, renumbers
   the subresources of later banks and drops the bank's slot from g_GraphicsPaletteBankSlots. Called by the
   palette optimiser (GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices/MergePaletteBank...).
*/
void GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
          (GraphicsPaletteIndex paletteIndex,GraphicsTextureSourceHeaderView *textureSource)

{
  AssetPaletteBankCount *paletteBankCountField;
  AssetRelativeOffset *subresourceTableOffsetField;
  AssetAllocationSizeBytes *allocationSizeField;
  uint32_t remainingDwords;
  int bankOffsetOrCount;
  uint32_t *bankSourceOrSlotDestination;
  GraphicsTextureSourceEntry *subresourceEntry;
  uint32_t *sourceDwordCursor;
  uint32_t *bankDestinationCursor;
  AssetSubresourceCount remainingSubresources;
  
  bankOffsetOrCount = paletteIndex * GRAPHICS_PALETTE_BANK_BYTES + GRAPHICS_PALETTE_BANKS_OFFSET;
  bankDestinationCursor = (uint32_t *)((uint8_t *)textureSource + bankOffsetOrCount);
  bankSourceOrSlotDestination = (uint32_t *)((uint8_t *)textureSource + GRAPHICS_PALETTE_BANKS_OFFSET +
                                             (paletteIndex + 1) * GRAPHICS_PALETTE_BANK_BYTES);
  remainingDwords = ((textureSource->common).allocationSizeBytes - GRAPHICS_PALETTE_BANK_BYTES) -
                    bankOffsetOrCount >> 2;
  if (remainingDwords != 0) {
    for (; remainingDwords != 0; remainingDwords--) {
      *bankDestinationCursor = *bankSourceOrSlotDestination;
      bankSourceOrSlotDestination++;
      bankDestinationCursor++;
    }
  }
  paletteBankCountField = &(textureSource->tableDescriptor).paletteBankCount;
  *paletteBankCountField = *paletteBankCountField - 1;
  subresourceTableOffsetField = &(textureSource->tableDescriptor).subresourceTableOffset;
  *subresourceTableOffsetField = *subresourceTableOffsetField - GRAPHICS_PALETTE_BANK_BYTES;
  allocationSizeField = &(textureSource->common).allocationSizeBytes;
  *allocationSizeField = *allocationSizeField - GRAPHICS_PALETTE_BANK_BYTES;
  subresourceEntry = (GraphicsTextureSourceEntry *)((uint8_t *)textureSource +
                                                    (textureSource->tableDescriptor).subresourceTableOffset);
  for (remainingSubresources = (textureSource->tableDescriptor).subresourceCount; remainingSubresources != 0;
       remainingSubresources--) {
    subresourceEntry->dataOffset = subresourceEntry->dataOffset - GRAPHICS_PALETTE_BANK_BYTES;
    if (paletteIndex < subresourceEntry->paletteIndex) {
      subresourceEntry->paletteIndex = subresourceEntry->paletteIndex - 1;
    }
    subresourceEntry++;
  }
  bankSourceOrSlotDestination = &g_GraphicsPaletteBankSlots[paletteIndex];
  sourceDwordCursor = &g_GraphicsPaletteBankSlots[paletteIndex + 1];
  bankOffsetOrCount = GRAPHICS_PALETTE_BANK_SLOT_CAPACITY - 1 - paletteIndex; /* slots behind it */
  if (bankOffsetOrCount != 0) {
    for (; bankOffsetOrCount != 0; bankOffsetOrCount--) {
      *bankSourceOrSlotDestination = *sourceDwordCursor;
      sourceDwordCursor++;
      bankSourceOrSlotDestination++;
    }
  }
  return;
}

