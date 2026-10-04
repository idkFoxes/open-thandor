/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/palette_optimizer.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Palette optimiser and combiner (converter/editor code, no caller in the game): folds and packs the used
   colours of the palette banks, merges and removes banks and combines assets, remapping the colour indices of
   the subresources. */

#include <thandor/graphics/resources/palette_optimizer.h>
#include <thandor/thandor.h>

/* Module data. */

static uint32_t g_GraphicsPaletteBankSlots[512] = {0};

static uint8_t g_GraphicsPaletteRemapBytes[256] = {0};

/* Returns the first entry (colour dword, second dword) of palette bank bankIndex. */
static uint32_t *GraphicsPaletteTextureSource_GetBankEntries(GraphicsTextureSourceHeaderView *textureSource,
                                                             GraphicsPaletteIndex bankIndex)
{
  return (uint32_t *)((uint8_t *)textureSource + GRAPHICS_PALETTE_BANKS_OFFSET +
                      bankIndex * GRAPHICS_PALETTE_BANK_BYTES);
}

/* Marks the first entryCount palette entries unused; fully transparent colours become plain black. */
static void GraphicsPaletteTextureSource_MarkAllEntriesUnused(GraphicsTextureSourceHeaderView *textureSource,
                                                              uint32_t entryCount)
{
  uint32_t *entry;

  entry = GraphicsPaletteTextureSource_GetBankEntries(textureSource,0);
  for (; entryCount != 0; entryCount--) {
    if ((*entry & ARGB8888_ALPHA_MASK) == 0) {
      *entry = GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
    }
    else {
      *entry = *entry | GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
    }
    entry = entry + 2;
  }
}

/* Clears the unused mark on every entry a subresource pixel references (paletteIndex -1: no bank). The caller
   makes sure there is at least one subresource. */
static void GraphicsPaletteTextureSource_ClearMarkOnReferencedEntries(GraphicsTextureSourceHeaderView *textureSource)
{
  uint32_t remainingSubresources;
  GraphicsTextureSourceEntry *subresourceEntry;
  GraphicsPaletteIndex subresourceBank;
  int remainingPixels;
  uint32_t *bankEntries;
  uint8_t *pixelCursor;

  subresourceEntry = (GraphicsTextureSourceEntry *)
                     ((uint8_t *)textureSource + textureSource->tableDescriptor.subresourceTableOffset);
  for (remainingSubresources = textureSource->tableDescriptor.subresourceCount; remainingSubresources != 0;
       remainingSubresources--) {
    subresourceBank = subresourceEntry->paletteIndex;
    remainingPixels = subresourceEntry->pixelWidth * subresourceEntry->pixelHeight;
    if (subresourceBank != -1) {
      bankEntries = GraphicsPaletteTextureSource_GetBankEntries(textureSource,subresourceBank);
      pixelCursor = (uint8_t *)textureSource + subresourceEntry->dataOffset;
      for (; remainingPixels != 0; remainingPixels--) {
        bankEntries[(uint32_t)*pixelCursor * 2] = bankEntries[(uint32_t)*pixelCursor * 2] &
                                                  ~GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
        pixelCursor++;
      }
    }
    subresourceEntry++;
  }
}

/* Stores the number of used colours of each bank in g_GraphicsPaletteBankSlots (at most
   GRAPHICS_PALETTE_BANK_SLOT_CAPACITY banks). The caller makes sure there is at least one bank. */
static void GraphicsPaletteTextureSource_CountUsedColorsPerBank(GraphicsTextureSourceHeaderView *textureSource)
{
  uint32_t countedBanks;
  uint32_t bankIndex;
  uint32_t entryIndex;
  uint32_t usedColorCount;
  uint32_t *entry;

  countedBanks = textureSource->tableDescriptor.paletteBankCount;
  if (GRAPHICS_PALETTE_BANK_SLOT_CAPACITY < countedBanks) {
    countedBanks = GRAPHICS_PALETTE_BANK_SLOT_CAPACITY;
  }
  entry = GraphicsPaletteTextureSource_GetBankEntries(textureSource,0);
  for (bankIndex = 0; bankIndex < countedBanks; bankIndex++) {
    usedColorCount = 0;
    for (entryIndex = 0; entryIndex < GRAPHICS_PALETTE_BANK_ENTRIES; entryIndex++) {
      if ((*entry & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
        usedColorCount++;
      }
      entry = entry + 2;
    }
    g_GraphicsPaletteBankSlots[bankIndex] = usedColorCount;
  }
}

/* Returns the first bank whose g_GraphicsPaletteBankSlots count is 0 among the first bankCount slots, or -1.
   Original quirk: the scan always checks the first slot, also when bankCount is 0 (it then keeps scanning
   until it finds a zero slot), and it reads past the GRAPHICS_PALETTE_BANK_SLOT_CAPACITY counted slots for
   larger bank counts. */
static GraphicsPaletteIndex GraphicsPaletteTextureSource_FindEmptyBank(uint32_t bankCount)
{
  const uint32_t *slot;
  GraphicsPaletteIndex bankIndex;
  uint32_t remainingBanks;

  slot = g_GraphicsPaletteBankSlots;
  bankIndex = 0;
  remainingBanks = bankCount;
  do {
    if (*slot == 0) {
      return bankIndex;
    }
    slot++;
    bankIndex++;
    remainingBanks--;
  } while (remainingBanks != 0);
  return -1;
}

/* Folds duplicate used colours inside bank bankIndex: pixels using a later copy of a used colour move to the
   first one, and the copy is marked unused. */
static void GraphicsPaletteTextureSource_FoldDuplicateColorsInBank(GraphicsTextureSourceHeaderView *textureSource,
                                                                   GraphicsPaletteIndex bankIndex)
{
  uint32_t *bankEntries;
  uint32_t firstIndex;
  uint32_t laterIndex;
  uint32_t color;

  bankEntries = GraphicsPaletteTextureSource_GetBankEntries(textureSource,bankIndex);
  for (firstIndex = 0; firstIndex < 255; firstIndex++) {
    color = bankEntries[firstIndex * 2];
    if ((color & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      for (laterIndex = firstIndex + 1; laterIndex < GRAPHICS_PALETTE_BANK_ENTRIES; laterIndex++) {
        if (color == bankEntries[laterIndex * 2]) {
          GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank(laterIndex,firstIndex,bankIndex,textureSource);
          bankEntries[laterIndex * 2] = bankEntries[laterIndex * 2] | GRAPHICS_PALETTE_ENTRY_UNUSED_MARK;
        }
      }
    }
  }
}

/* Merges the first bank behind destinationBank whose used colours fit together with destinationBank's into one
   bank (at most GRAPHICS_PALETTE_BANK_ENTRIES). Returns true when a bank was merged. */
static Bool8 GraphicsPaletteTextureSource_MergeFirstFittingBankInto(GraphicsTextureSourceHeaderView *textureSource,
                                                                    uint32_t destinationBank)
{
  uint32_t candidateBank;
  uint32_t combinedCount;

  for (candidateBank = destinationBank + 1; candidateBank < textureSource->tableDescriptor.paletteBankCount;
       candidateBank++) {
    combinedCount = GraphicsPaletteTextureSource_CountCombinedUsedColors(candidateBank,destinationBank,textureSource);
    if (combinedCount < GRAPHICS_PALETTE_BANK_ENTRIES + 1) {
      GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources(candidateBank,destinationBank,textureSource);
      return true;
    }
  }
  return false;
}

/* Packs the used entries of bank bankIndex to its front (remapping the pixels) and zeroes the colour dword of
   the remaining entries. Original quirk: every entry is also copied to the write position when it is unused;
   the second dword of the zeroed tail keeps whatever was there. */
static void GraphicsPaletteTextureSource_PackUsedEntriesOfBank(GraphicsTextureSourceHeaderView *textureSource,
                                                               GraphicsPaletteIndex bankIndex)
{
  uint32_t *readEntry;
  uint32_t *writeEntry;
  uint32_t readIndex;
  uint32_t packedCount;
  uint32_t color;
  int remainingEntries;

  readEntry = GraphicsPaletteTextureSource_GetBankEntries(textureSource,bankIndex);
  writeEntry = readEntry;
  packedCount = 0;
  for (readIndex = 0; readIndex < GRAPHICS_PALETTE_BANK_ENTRIES; readIndex++) {
    writeEntry[1] = readEntry[1];
    color = *readEntry;
    *writeEntry = color;
    readEntry = readEntry + 2;
    if ((color & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank(readIndex,packedCount,bankIndex,textureSource);
      writeEntry = writeEntry + 2;
      packedCount++;
    }
  }
  for (remainingEntries = GRAPHICS_PALETTE_BANK_ENTRIES - packedCount; remainingEntries != 0; remainingEntries--) {
    *writeEntry = 0;
    writeEntry = writeEntry + 2;
  }
}

/* Shrinks the palette banks of a palette texture source in place: marks every entry unused, clears the mark
   on each entry a subresource pixel references, removes banks without a used colour, folds duplicate colours
   within a bank, merges bank pairs whose used colours fit into one bank and finally packs the used entries of
   every bank to its front (zeroing the rest), remapping the pixel indices at each step. Returns true when the
   source has no palette bank or no subresource. No caller or table reference is known (converter/editor code left
   in the game).
*/
Bool8 GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(intptr_t textureSourceBase)

{
  GraphicsTextureSourceHeaderView *textureSource;
  uint32_t entryCount;
  GraphicsPaletteIndex emptyBank;
  uint32_t bankIndex;
  uint32_t destinationBank;

  /* the banks start at GRAPHICS_PALETTE_BANKS_OFFSET, entries are 8 bytes (colour, second dword) */
  textureSource = (GraphicsTextureSourceHeaderView *)textureSourceBase;
  entryCount = textureSource->tableDescriptor.paletteBankCount << 8;
  if (entryCount == 0) {
    return true;
  }
  GraphicsPaletteTextureSource_MarkAllEntriesUnused(textureSource,entryCount);
  if (textureSource->tableDescriptor.subresourceCount == 0) {
    return true;
  }
  GraphicsPaletteTextureSource_ClearMarkOnReferencedEntries(textureSource);
  GraphicsPaletteTextureSource_CountUsedColorsPerBank(textureSource);
  /* Remove every bank without a used color, rescanning from the first bank after each removal. */
  emptyBank = GraphicsPaletteTextureSource_FindEmptyBank(textureSource->tableDescriptor.paletteBankCount);
  while (emptyBank != -1) {
    GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(emptyBank,textureSource);
    emptyBank = GraphicsPaletteTextureSource_FindEmptyBank(textureSource->tableDescriptor.paletteBankCount);
  }
  for (bankIndex = 0; bankIndex < textureSource->tableDescriptor.paletteBankCount; bankIndex++) {
    GraphicsPaletteTextureSource_FoldDuplicateColorsInBank(textureSource,bankIndex);
  }
  /* Merge bank pairs whose combined used colors fit into 256 entries; after a merge, retry the same
     destination bank against the remaining ones. */
  destinationBank = 0;
  while (destinationBank + 1 < textureSource->tableDescriptor.paletteBankCount) {
    if (!GraphicsPaletteTextureSource_MergeFirstFittingBankInto(textureSource,destinationBank)) {
      destinationBank++;
    }
  }
  for (bankIndex = 0; bankIndex < textureSource->tableDescriptor.paletteBankCount; bankIndex++) {
    GraphicsPaletteTextureSource_PackUsedEntriesOfBank(textureSource,bankIndex);
  }
  return false;
}

/* Checks the 'pal' signature: returns true and stores paletteBankCount (+0xB0) in *outBankCount, or returns
   false (leaving *outBankCount untouched) for a wrong signature. No caller or table reference is known.
*/
Bool8 GraphicsPaletteAsset_GetBankCount(GraphicsPaletteAsset *paletteAsset,uint32_t *outBankCount)

{
  if (paletteAsset->magic != ASSET_MAGIC_PAL) {
    return false;
  }
  *outBankCount = paletteAsset->paletteBankCount;
  return true;
}

/* Copies dwordCount dwords from source to destination and returns the destination position behind them. */
static uint32_t *GraphicsPaletteTextureSource_CopyDwords(uint32_t *destination,const uint32_t *source,
                                                         uint32_t dwordCount)
{
  for (; dwordCount != 0; dwordCount--) {
    *destination = *source;
    source++;
    destination++;
  }
  return destination;
}

/* Builds a new palette texture source from baseAsset followed by appendedAsset: one header (base's, with the
   size and the bank and subresource counts summed), base banks, appended banks, base subresource entries,
   appended entries, base pixel data, appended pixel data. Pixel offsets of both entry sets and the bank index
   of appended entries are rebased. Returns the new asset, or NULL when the allocation fails.
   No caller or table reference is known (converter/editor code left in the game).
*/
GraphicsPaletteTextureSourceAsset * GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets
          (GraphicsPaletteTextureSourceAsset *appendedAsset,
          GraphicsPaletteTextureSourceAsset *baseAsset)

{
  GraphicsPaletteBankCount appendedBankCount;
  GraphicsAssetAllocationByteSize baseAllocationSize;
  uint32_t bytes;
  int baseBankCount;
  int appendedPaletteBytes;
  GraphicsAssetSubresourceCount remainingSubresources;
  GraphicsAssetSubresourceCount appendedSubresourceCount;
  const uint32_t *sourceDword;
  uint32_t *destinationDword;
  GraphicsTextureSourceEntry *destinationEntry;
  GraphicsPaletteTextureSourceAsset *combinedAsset;

  bytes = (baseAsset->allocationSizeBytes + appendedAsset->allocationSizeBytes) - GRAPHICS_PALETTE_BANKS_OFFSET;
  if (g_MemoryApi.alloc(bytes,(void **)&combinedAsset) != 0) {
    return nullptr;
  }
  /* the header */
  destinationDword = GraphicsPaletteTextureSource_CopyDwords
                               ((uint32_t *)combinedAsset,(const uint32_t *)baseAsset,GRAPHICS_PALETTE_BANKS_OFFSET / 4);
  appendedBankCount = appendedAsset->paletteBankCount;
  appendedSubresourceCount = appendedAsset->subresourceCount;
  combinedAsset->allocationSizeBytes = bytes;
  combinedAsset->paletteBankCount = combinedAsset->paletteBankCount + appendedBankCount;
  combinedAsset->subresourceCount = combinedAsset->subresourceCount + appendedSubresourceCount;
  appendedPaletteBytes = appendedBankCount * GRAPHICS_PALETTE_BANK_BYTES;
  combinedAsset->subresourceTableOffset = combinedAsset->subresourceTableOffset + appendedPaletteBytes;
  /* base banks, then appended banks */
  baseBankCount = baseAsset->paletteBankCount;
  destinationDword = GraphicsPaletteTextureSource_CopyDwords
                               (destinationDword,(const uint32_t *)baseAsset->paletteEntries,
                                baseBankCount * (GRAPHICS_PALETTE_BANK_BYTES / 4));
  destinationDword = GraphicsPaletteTextureSource_CopyDwords
                               (destinationDword,(const uint32_t *)appendedAsset->paletteEntries,
                                appendedAsset->paletteBankCount * (GRAPHICS_PALETTE_BANK_BYTES / 4));
  /* base subresource entries: their pixels move behind the appended banks and entries.
     Original quirk: do-while, so an asset without subresources would loop 2^32 times (same for appended). */
  baseAllocationSize = baseAsset->allocationSizeBytes;
  remainingSubresources = baseAsset->subresourceCount;
  sourceDword = (const uint32_t *)((uint8_t *)baseAsset + baseAsset->subresourceTableOffset);
  do {
    destinationEntry = (GraphicsTextureSourceEntry *)destinationDword;
    destinationDword = GraphicsPaletteTextureSource_CopyDwords
                                 (destinationDword,sourceDword,GFX_SUBRESOURCE_RECORD_SIZE / 4);
    sourceDword = sourceDword + GFX_SUBRESOURCE_RECORD_SIZE / 4;
    destinationEntry->dataOffset = destinationEntry->dataOffset + appendedPaletteBytes +
                                   appendedSubresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
    remainingSubresources--;
  } while (remainingSubresources != 0);
  /* appended subresource entries: their pixels move behind the base pixels, their banks behind the base banks */
  sourceDword = (const uint32_t *)((uint8_t *)appendedAsset + appendedAsset->subresourceTableOffset);
  remainingSubresources = appendedAsset->subresourceCount;
  do {
    destinationEntry = (GraphicsTextureSourceEntry *)destinationDword;
    destinationDword = GraphicsPaletteTextureSource_CopyDwords
                                 (destinationDword,sourceDword,GFX_SUBRESOURCE_RECORD_SIZE / 4);
    sourceDword = sourceDword + GFX_SUBRESOURCE_RECORD_SIZE / 4;
    destinationEntry->dataOffset = destinationEntry->dataOffset + (baseAllocationSize - GRAPHICS_PALETTE_BANKS_OFFSET);
    if (-1 < destinationEntry->paletteIndex) { /* entries without a bank keep -1 */
      destinationEntry->paletteIndex = destinationEntry->paletteIndex + baseBankCount;
    }
    remainingSubresources--;
  } while (remainingSubresources != 0);
  /* the pixel data behind each entry table */
  destinationDword = GraphicsPaletteTextureSource_CopyDwords
                               (destinationDword,
                                (const uint32_t *)((uint8_t *)baseAsset +
                                                   baseAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE +
                                                   baseAsset->subresourceTableOffset),
                                (baseAsset->allocationSizeBytes - baseAsset->subresourceTableOffset -
                                 baseAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE) >> 2);
  GraphicsPaletteTextureSource_CopyDwords
            (destinationDword,
             (const uint32_t *)((uint8_t *)appendedAsset +
                                appendedAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE +
                                appendedAsset->subresourceTableOffset),
             (appendedAsset->allocationSizeBytes - appendedAsset->subresourceTableOffset -
              appendedAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE) >> 2);
  return combinedAsset;
}

/* Moves the used colours of bank sourcePaletteBank into bank destinationPaletteBank (reusing identical colours,
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
  /* the subresource table */
  remainingSubresources = (textureSource->tableDescriptor).subresourceCount;
  subresourceEntry = (GraphicsTextureSourceEntry *)
                     ((uint8_t *)textureSource + (textureSource->tableDescriptor).subresourceTableOffset);
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
}

/* Replaces colour index oldColorIndex by newColorIndex in the pixels of every subresource that uses palette
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
}

/* Returns how many entries one bank would need to hold the used colours of both destinationPaletteBank and
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

/* Deletes palette bank paletteIndex: moves everything behind it 0x800 bytes down, lowers the bank count, the
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
  int removedBankOffset;
  int remainingSlots;
  uint32_t *bankSourceCursor;
  GraphicsTextureSourceEntry *subresourceEntry;
  uint32_t *slotSourceCursor;
  uint32_t *slotDestinationCursor;
  uint32_t *bankDestinationCursor;
  AssetSubresourceCount remainingSubresources;

  removedBankOffset = paletteIndex * GRAPHICS_PALETTE_BANK_BYTES + GRAPHICS_PALETTE_BANKS_OFFSET;
  bankDestinationCursor = (uint32_t *)((uint8_t *)textureSource + removedBankOffset);
  bankSourceCursor = (uint32_t *)((uint8_t *)textureSource + GRAPHICS_PALETTE_BANKS_OFFSET +
                                  (paletteIndex + 1) * GRAPHICS_PALETTE_BANK_BYTES);
  remainingDwords = (((textureSource->common).allocationSizeBytes - GRAPHICS_PALETTE_BANK_BYTES) -
                     removedBankOffset) >> 2;
  for (; remainingDwords != 0; remainingDwords--) {
    *bankDestinationCursor = *bankSourceCursor;
    bankSourceCursor++;
    bankDestinationCursor++;
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
  slotDestinationCursor = &g_GraphicsPaletteBankSlots[paletteIndex];
  slotSourceCursor = &g_GraphicsPaletteBankSlots[paletteIndex + 1];
  for (remainingSlots = GRAPHICS_PALETTE_BANK_SLOT_CAPACITY - 1 - paletteIndex; remainingSlots != 0;
       remainingSlots--) { /* slots behind it */
    *slotDestinationCursor = *slotSourceCursor;
    slotSourceCursor++;
    slotDestinationCursor++;
  }
}
