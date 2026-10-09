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

static uint32_t g_GraphicsPaletteBankSlots[512] = {};

static uint8_t g_GraphicsPaletteRemapBytes[256] = {};

/* The texture source as bytes: the header, the palette banks, the record table and the pixels are one block and
   every offset in it is relative to its start. */
static uint8_t *GraphicsPaletteTextureSource_Bytes(GraphicsTextureSourceHeaderView *textureSource)
{
  return reinterpret_cast<uint8_t *>(textureSource); /* byte view of the asset block */
}

static const uint8_t *GraphicsPaletteTextureSource_Bytes(const GraphicsPaletteTextureSourceAsset *asset)
{
  return reinterpret_cast<const uint8_t *>(asset); /* byte view of the asset block */
}

/* The subresource record table at textureSource + subresourceTableOffset. */
static GraphicsTextureSourceEntry *GraphicsPaletteTextureSource_Entries(GraphicsTextureSourceHeaderView *textureSource)
{
  /* the records are 32-byte GraphicsTextureSourceEntry structs inside the asset block */
  return reinterpret_cast<GraphicsTextureSourceEntry *>(GraphicsPaletteTextureSource_Bytes(textureSource) +
                                                       textureSource->tableDescriptor.subresourceTableOffset);
}

/* Returns the first entry (colour dword, second dword) of palette bank bankIndex. */
static uint32_t *GraphicsPaletteTextureSource_GetBankEntries(GraphicsTextureSourceHeaderView *textureSource,
                                                             GraphicsPaletteIndex bankIndex)
{
  /* the optimiser walks the 8-byte GraphicsTexturePaletteEntry records of a bank as dword pairs */
  return reinterpret_cast<uint32_t *>(GraphicsPaletteTextureSource_Bytes(textureSource) +
                                      GRAPHICS_PALETTE_BANKS_OFFSET + bankIndex * GRAPHICS_PALETTE_BANK_BYTES);
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

  subresourceEntry = GraphicsPaletteTextureSource_Entries(textureSource);
  for (remainingSubresources = textureSource->tableDescriptor.subresourceCount; remainingSubresources != 0;
       remainingSubresources--) {
    subresourceBank = subresourceEntry->paletteIndex;
    remainingPixels = subresourceEntry->pixelWidth * subresourceEntry->pixelHeight;
    if (subresourceBank != -1) {
      bankEntries = GraphicsPaletteTextureSource_GetBankEntries(textureSource,subresourceBank);
      pixelCursor = GraphicsPaletteTextureSource_Bytes(textureSource) + subresourceEntry->dataOffset;
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
   Original quirk: it reads past the GRAPHICS_PALETTE_BANK_SLOT_CAPACITY counted slots for larger bank counts.
   The original always checked the first slot, also when bankCount was 0 (a do-while that then kept scanning
   until it found a zero slot), so once every bank was removed (all subresources bank-less) the optimiser
   removed a bank from a count of 0 and crashed; bounded here because a bank count of 0 now returns -1. */
static GraphicsPaletteIndex GraphicsPaletteTextureSource_FindEmptyBank(uint32_t bankCount)
{
  const uint32_t *slot;
  GraphicsPaletteIndex bankIndex;
  uint32_t remainingBanks;

  slot = g_GraphicsPaletteBankSlots;
  bankIndex = 0;
  for (remainingBanks = bankCount; remainingBanks != 0; remainingBanks--) {
    if (*slot == 0) {
      return bankIndex;
    }
    slot++;
    bankIndex++;
  }
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
static bool GraphicsPaletteTextureSource_MergeFirstFittingBankInto(GraphicsTextureSourceHeaderView *textureSource,
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
bool GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(intptr_t textureSourceBase)

{
  GraphicsTextureSourceHeaderView *textureSource;
  uint32_t entryCount;
  GraphicsPaletteIndex emptyBank;
  uint32_t bankIndex;
  uint32_t destinationBank;

  /* the banks start at GRAPHICS_PALETTE_BANKS_OFFSET, entries are 8 bytes (colour, second dword) */
  textureSource = reinterpret_cast<GraphicsTextureSourceHeaderView *>(textureSourceBase); /* the asset's address */
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
  /* Remove every bank without a used color, rescanning from the first bank after each removal; this stops when
     no bank is left (see GraphicsPaletteTextureSource_FindEmptyBank). */
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
bool GraphicsPaletteAsset_GetBankCount(GraphicsPaletteAsset *paletteAsset,uint32_t *outBankCount)

{
  if (paletteAsset->magic != ASSET_MAGIC_PAL) {
    return false;
  }
  *outBankCount = paletteAsset->paletteBankCount;
  return true;
}

/* Copies dwordCount dwords from source to destination and returns the destination position behind them. Both
   are dword-aligned positions inside asset blocks (header, banks, records or pixels), copied as raw dwords. */
static uint32_t *GraphicsPaletteTextureSource_CopyDwords(void *destination,const void *source,uint32_t dwordCount)
{
  uint32_t *destinationDword = static_cast<uint32_t *>(destination);
  const uint32_t *sourceDword = static_cast<const uint32_t *>(source);

  for (; dwordCount != 0; dwordCount--) {
    *destinationDword = *sourceDword;
    sourceDword++;
    destinationDword++;
  }
  return destinationDword;
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
  const uint8_t *sourceRecord;
  uint32_t *destinationDword;
  GraphicsTextureSourceEntry *destinationEntry;
  void *block;
  GraphicsPaletteTextureSourceAsset *combinedAsset;

  bytes = (baseAsset->allocationSizeBytes + appendedAsset->allocationSizeBytes) - GRAPHICS_PALETTE_BANKS_OFFSET;
  if (g_MemoryApi.alloc(bytes,&block) != 0) {
    return nullptr;
  }
  combinedAsset = static_cast<GraphicsPaletteTextureSourceAsset *>(block);
  /* the header */
  destinationDword = GraphicsPaletteTextureSource_CopyDwords(combinedAsset,baseAsset,GRAPHICS_PALETTE_BANKS_OFFSET / 4);
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
                               (destinationDword,baseAsset->paletteEntries,
                                baseBankCount * (GRAPHICS_PALETTE_BANK_BYTES / 4));
  destinationDword = GraphicsPaletteTextureSource_CopyDwords
                               (destinationDword,appendedAsset->paletteEntries,
                                appendedAsset->paletteBankCount * (GRAPHICS_PALETTE_BANK_BYTES / 4));
  /* base subresource entries: their pixels move behind the appended banks and entries.
     The original copied both entry tables in do-whiles, so an asset without subresources looped 2^32 times;
     bounded here because the subresource counts of the inputs are not checked (0 copies no entry). */
  baseAllocationSize = baseAsset->allocationSizeBytes;
  remainingSubresources = baseAsset->subresourceCount;
  sourceRecord = GraphicsPaletteTextureSource_Bytes(baseAsset) + baseAsset->subresourceTableOffset;
  for (; remainingSubresources != 0; remainingSubresources--) {
    /* the record about to be copied to destinationDword */
    destinationEntry = reinterpret_cast<GraphicsTextureSourceEntry *>(destinationDword);
    destinationDword = GraphicsPaletteTextureSource_CopyDwords
                                 (destinationDword,sourceRecord,GFX_SUBRESOURCE_RECORD_SIZE / 4);
    sourceRecord = sourceRecord + GFX_SUBRESOURCE_RECORD_SIZE;
    destinationEntry->dataOffset = destinationEntry->dataOffset + appendedPaletteBytes +
                                   appendedSubresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
  }
  /* appended subresource entries: their pixels move behind the base pixels, their banks behind the base banks */
  sourceRecord = GraphicsPaletteTextureSource_Bytes(appendedAsset) + appendedAsset->subresourceTableOffset;
  for (remainingSubresources = appendedAsset->subresourceCount; remainingSubresources != 0;
       remainingSubresources--) {
    /* the record about to be copied to destinationDword */
    destinationEntry = reinterpret_cast<GraphicsTextureSourceEntry *>(destinationDword);
    destinationDword = GraphicsPaletteTextureSource_CopyDwords
                                 (destinationDword,sourceRecord,GFX_SUBRESOURCE_RECORD_SIZE / 4);
    sourceRecord = sourceRecord + GFX_SUBRESOURCE_RECORD_SIZE;
    destinationEntry->dataOffset = destinationEntry->dataOffset + (baseAllocationSize - GRAPHICS_PALETTE_BANKS_OFFSET);
    if (-1 < destinationEntry->paletteIndex) { /* entries without a bank keep -1 */
      destinationEntry->paletteIndex = destinationEntry->paletteIndex + baseBankCount;
    }
  }
  /* the pixel data behind each entry table */
  destinationDword = GraphicsPaletteTextureSource_CopyDwords
                               (destinationDword,
                                GraphicsPaletteTextureSource_Bytes(baseAsset) +
                                baseAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE +
                                baseAsset->subresourceTableOffset,
                                (baseAsset->allocationSizeBytes - baseAsset->subresourceTableOffset -
                                 baseAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE) >> 2);
  GraphicsPaletteTextureSource_CopyDwords
            (destinationDword,
             GraphicsPaletteTextureSource_Bytes(appendedAsset) +
             appendedAsset->subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE +
             appendedAsset->subresourceTableOffset,
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
  
  sourceEntry = GraphicsPaletteTextureSource_GetBankEntries(textureSource,sourcePaletteBank);
  destinationBankEntries = GraphicsPaletteTextureSource_GetBankEntries(textureSource,destinationPaletteBank);
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
  subresourceEntry = GraphicsPaletteTextureSource_Entries(textureSource);
  /* at least one subresource: the optimiser, the only caller, returns early when there is none */
  do {
    if (sourcePaletteBank == subresourceEntry->paletteIndex) {
      subresourceEntry->paletteIndex = destinationPaletteBank;
      pixelCursor = GraphicsPaletteTextureSource_Bytes(textureSource) + subresourceEntry->dataOffset;
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
  subresourceEntry = GraphicsPaletteTextureSource_Entries(textureSource);
  if (newColorIndex != oldColorIndex) {
    /* at least one subresource: the optimiser, the only caller, returns early when there is none */
    do {
      if (paletteBank == subresourceEntry->paletteIndex) {
        pixelCursor = GraphicsPaletteTextureSource_Bytes(textureSource) + subresourceEntry->dataOffset;
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
  
  destinationEntry = GraphicsPaletteTextureSource_GetBankEntries(textureSource,destinationPaletteBank);
  candidateBankCursor = GraphicsPaletteTextureSource_GetBankEntries(textureSource,candidatePaletteBank);
  usedColorCount = 0;
  /* Used destination colors that the candidate bank does not contain as well... */
  remainingEntries = GRAPHICS_PALETTE_BANK_ENTRIES;
  for (; remainingEntries != 0; remainingEntries--) {
    if ((*destinationEntry & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      remainingCandidateEntries = GRAPHICS_PALETTE_BANK_ENTRIES;
      candidateEntry = candidateBankCursor;
      for (; remainingCandidateEntries != 0; remainingCandidateEntries--) {
        if (*destinationEntry == *candidateEntry) break;
        candidateEntry = candidateEntry + 2;
      }
      if (remainingCandidateEntries == 0) {
        usedColorCount++;
      }
    }
    destinationEntry = destinationEntry + 2;
  }
  /* ...plus every used candidate color. */
  remainingEntries = GRAPHICS_PALETTE_BANK_ENTRIES;
  for (; remainingEntries != 0; remainingEntries--) {
    if ((*candidateBankCursor & GRAPHICS_PALETTE_ENTRY_UNUSED_MARK) == 0) {
      usedColorCount++;
    }
    candidateBankCursor = candidateBankCursor + 2;
  }
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
  bankDestinationCursor = GraphicsPaletteTextureSource_GetBankEntries(textureSource,paletteIndex);
  bankSourceCursor = GraphicsPaletteTextureSource_GetBankEntries(textureSource,paletteIndex + 1);
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
  subresourceEntry = GraphicsPaletteTextureSource_Entries(textureSource);
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
