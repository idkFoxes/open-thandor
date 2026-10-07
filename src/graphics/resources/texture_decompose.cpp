/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture_decompose.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Subresource decomposition of a texture source: cuts the ARGB or indexed pixels of every subresource into
   rectangular regions (asset conversion). */

#include <thandor/graphics/resources/texture_decompose.h>
#include <thandor/thandor.h>

/* Work state of GraphicsTextureSource_DecomposeSubresourceRegions while it cuts a sprite sheet into regions.
   The work area is the new asset's block: header (and palette bank), then the record table growing upwards,
   free bytes, the packed sprite pixels growing downwards, and the working copy of the sheet at the end. */
struct GraphicsTextureDecomposeState {
  GraphicsTextureSourceAsset *asset;   /* the new asset */
  GraphicsTextureSourceEntry *records; /* its subresource record table */
  int sourceWidth;                     /* pixels per row of the sheet, the row stride of the working copy */
  int rowsRemaining;                   /* rows from the scanned row to the bottom of the sheet, counting it */
  int freeBytes;                       /* bytes of the work area not yet taken by records or packed pixels */
  uint8_t *packedPixels;               /* lowest packed sprite pixel */
  uint32_t packedPixelBytes;
  Bool8 edgeTransparent;                /* the edge colour is transparent, so border rows/columns in it are trimmed */
};

/* Module data. */

/* GraphicsTextureSourceDecomposeSubresourceProc * hook slot, statically GraphicsTextureSource_DecomposeSubresourceRegions */
[[maybe_unused]] static GraphicsTextureSourceDecomposeSubresourceProc *g_GraphicsTextureSourceDecomposeSubresourceRegionsCf = &GraphicsTextureSource_DecomposeSubresourceRegions;

/* Takes `amount` bytes of the work area's free bytes. Returns false (taking nothing) unless at least one byte
   stays free. */
static Bool8 GraphicsTextureDecompose_ReserveBytes(int *freeBytes,int amount)
{
  int bytesLeft;

  bytesLeft = *freeBytes - amount;
  if (bytesLeft == 0 || *freeBytes < amount) {
    return false;
  }
  *freeBytes = bytesLeft;
  return true;
}

/* Copies `count` dwords upwards from the lowest one (the packed pixels move down onto the record table end). Both
   are dword-aligned positions inside asset blocks (header, palette bank or pixels), copied as raw dwords. */
static void GraphicsTextureDecompose_CopyDwords(void *destination,const void *source,uint32_t count)
{
  uint32_t *destinationDword = static_cast<uint32_t *>(destination);
  const uint32_t *sourceDword = static_cast<const uint32_t *>(source);

  for (; count != 0; count--) {
    *destinationDword = *sourceDword;
    destinationDword++;
    sourceDword++;
  }
}

/* The work area behind the new asset's record table, as bytes (state->freeBytes counts from its start). */
static uint8_t *GraphicsTextureDecompose_WorkBytes(const GraphicsTextureDecomposeState *state)
{
  return reinterpret_cast<uint8_t *>(state->records); /* byte view of the block from the record table on */
}

/* Appends a record to the new asset's table: logical height and origin zero, the given palette index; the
   caller fills in the sizes. */
static GraphicsTextureSourceEntry *GraphicsTextureDecompose_AddRecord
          (GraphicsTextureDecomposeState *state,GraphicsPaletteIndex paletteIndex)
{
  GraphicsTextureSourceEntry *record;
  AssetSubresourceCount recordIndex;

  recordIndex = (state->asset->tableDescriptor).subresourceCount;
  (state->asset->tableDescriptor).subresourceCount = recordIndex + 1;
  record = &state->records[recordIndex];
  record->logicalHeight = 0;
  record->originX = 0;
  record->originY = 0;
  record->paletteIndex = paletteIndex;
  return record;
}

/* True when the `count` (at least 1) ARGB pixels from `pixel` on, `step` pixels apart, all equal `color`. */
static Bool8 GraphicsTextureDecompose_ArgbRunIs(const uint32_t *pixel,uint32_t count,int step,uint32_t color)
{
  for (; count != 0; count--) {
    if (*pixel != color) {
      return false;
    }
    pixel = pixel + step;
  }
  return true;
}

/* True when the `count` (at least 1) palette indices from `pixel` on, `step` bytes apart, all equal `index`. */
static Bool8 GraphicsTextureDecompose_IndexRunIs(const uint8_t *pixel,uint32_t count,int step,uint8_t index)
{
  for (; count != 0; count--) {
    if (*pixel != index) {
      return false;
    }
    pixel = pixel + step;
  }
  return true;
}

/* Cuts the region whose top-left pixel (not background) is `blockStart` out of the ARGB working copy;
   `columnsLeft` counts the pixels from blockStart to the end of its row. Adds its record (logical width = the
   run of non-background pixels in this row, logical height = the run in this column, palette index -1), trims
   border rows and columns in the edge colour when that is transparent (the origin records how much was cut at
   the top/left; at least one row and column stay), copies the remaining pixels in front of the packed ones and
   clears the whole logical block to background. Returns false when the work area is full. */
static Bool8 GraphicsTextureDecompose_CutArgbRegion
          (GraphicsTextureDecomposeState *state,uint32_t *blockStart,int columnsLeft,uint32_t background,
          uint32_t edgeColor)
{
  GraphicsTextureSourceEntry *record;
  int sourceWidth;
  int count;
  int pixelCount;
  uint32_t rowCount;
  uint32_t column;
  uint32_t *pixel;
  uint32_t *topLeft;
  uint32_t *bottomRow;
  uint32_t *rightColumn;
  uint32_t *packed;

  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,GFX_SUBRESOURCE_RECORD_SIZE)) {
    return false;
  }
  sourceWidth = state->sourceWidth;
  record = GraphicsTextureDecompose_AddRecord(state,-1);
  pixel = blockStart;
  for (count = columnsLeft; count != 0 && *pixel != background; count--) {
    pixel++;
  }
  record->logicalWidth = (uint32_t)(pixel - blockStart);
  pixel = blockStart;
  count = state->rowsRemaining;
  do {
    record->logicalHeight = record->logicalHeight + 1;
    pixel = pixel + sourceWidth;
    count--;
  } while (count != 0 && *pixel != background);
  record->pixelWidth = record->logicalWidth;
  record->pixelHeight = record->logicalHeight;
  topLeft = blockStart;
  if (state->edgeTransparent) {
    while (GraphicsTextureDecompose_ArgbRunIs(topLeft,record->logicalWidth,1,edgeColor)) {
      record->originY = record->originY + 1;
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->originY = record->originY - 1;
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      topLeft = topLeft + sourceWidth;
    }
    bottomRow = topLeft + (int32_t)(sourceWidth * (record->pixelHeight - 1));
    while (GraphicsTextureDecompose_ArgbRunIs(bottomRow,record->logicalWidth,1,edgeColor)) {
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      bottomRow = bottomRow - sourceWidth;
    }
    while (GraphicsTextureDecompose_ArgbRunIs(topLeft,record->pixelHeight,sourceWidth,edgeColor)) {
      record->originX = record->originX + 1;
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->originX = record->originX - 1;
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      topLeft = topLeft + 1;
    }
    rightColumn = topLeft + (record->pixelWidth - 1);
    while (GraphicsTextureDecompose_ArgbRunIs(rightColumn,record->pixelHeight,sourceWidth,edgeColor)) {
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      rightColumn = rightColumn - 1;
    }
  }
  pixelCount = record->pixelWidth * record->pixelHeight;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,pixelCount * 4)) {
    return false;
  }
  state->packedPixels = state->packedPixels - pixelCount * 4;
  state->packedPixelBytes = state->packedPixelBytes + pixelCount * 4;
  packed = reinterpret_cast<uint32_t *>(state->packedPixels); /* ARGB8888 pixels, dword-aligned */
  pixel = topLeft;
  for (rowCount = record->pixelHeight; rowCount != 0; rowCount--) {
    GraphicsTextureDecompose_CopyDwords(packed,pixel,record->pixelWidth);
    packed = packed + record->pixelWidth;
    pixel = pixel + sourceWidth;
  }
  pixel = blockStart;
  for (rowCount = record->logicalHeight; rowCount != 0; rowCount--) {
    for (column = 0; column < record->logicalWidth; column++) {
      pixel[column] = background;
    }
    pixel = pixel + sourceWidth;
  }
  return true;
}

/* GraphicsTextureDecompose_CutArgbRegion on 8-bit palette indices: the record gets palette index 0 and the
   packed pixels are padded to whole dwords. */
static Bool8 GraphicsTextureDecompose_CutIndexedRegion
          (GraphicsTextureDecomposeState *state,uint8_t *blockStart,int columnsLeft,uint8_t backgroundIndex,
          uint8_t edgeIndex)
{
  GraphicsTextureSourceEntry *record;
  int sourceWidth;
  int count;
  uint32_t packedBytes;
  uint32_t rowCount;
  uint32_t column;
  uint8_t *pixel;
  uint8_t *topLeft;
  uint8_t *bottomRow;
  uint8_t *rightColumn;
  uint8_t *packed;

  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,GFX_SUBRESOURCE_RECORD_SIZE)) {
    return false;
  }
  sourceWidth = state->sourceWidth;
  record = GraphicsTextureDecompose_AddRecord(state,0);
  pixel = blockStart;
  for (count = columnsLeft; count != 0 && *pixel != backgroundIndex; count--) {
    pixel++;
  }
  record->logicalWidth = (uint32_t)(pixel - blockStart);
  pixel = blockStart;
  count = state->rowsRemaining;
  do {
    record->logicalHeight = record->logicalHeight + 1;
    pixel = pixel + sourceWidth;
    count--;
  } while (count != 0 && *pixel != backgroundIndex);
  record->pixelWidth = record->logicalWidth;
  record->pixelHeight = record->logicalHeight;
  topLeft = blockStart;
  if (state->edgeTransparent) {
    while (GraphicsTextureDecompose_IndexRunIs(topLeft,record->logicalWidth,1,edgeIndex)) {
      record->originY = record->originY + 1;
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->originY = record->originY - 1;
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      topLeft = topLeft + sourceWidth;
    }
    bottomRow = topLeft + (int32_t)(sourceWidth * (record->pixelHeight - 1));
    while (GraphicsTextureDecompose_IndexRunIs(bottomRow,record->logicalWidth,1,edgeIndex)) {
      record->pixelHeight = record->pixelHeight - 1;
      if (record->pixelHeight == 0) {
        record->pixelHeight = record->pixelHeight + 1;
        break;
      }
      bottomRow = bottomRow - sourceWidth;
    }
    while (GraphicsTextureDecompose_IndexRunIs(topLeft,record->pixelHeight,sourceWidth,edgeIndex)) {
      record->originX = record->originX + 1;
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->originX = record->originX - 1;
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      topLeft = topLeft + 1;
    }
    rightColumn = topLeft + (record->pixelWidth - 1);
    while (GraphicsTextureDecompose_IndexRunIs(rightColumn,record->pixelHeight,sourceWidth,edgeIndex)) {
      record->pixelWidth = record->pixelWidth - 1;
      if (record->pixelWidth == 0) {
        record->pixelWidth = record->pixelWidth + 1;
        break;
      }
      rightColumn = rightColumn - 1;
    }
  }
  packedBytes = record->pixelWidth * record->pixelHeight + 3U & ~3u;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,(int)packedBytes)) {
    return false;
  }
  state->packedPixels = state->packedPixels - packedBytes;
  state->packedPixelBytes = state->packedPixelBytes + packedBytes;
  packed = state->packedPixels;
  pixel = topLeft;
  for (rowCount = record->pixelHeight; rowCount != 0; rowCount--) {
    for (column = 0; column < record->pixelWidth; column++) {
      packed[column] = pixel[column];
    }
    packed = packed + record->pixelWidth;
    pixel = pixel + sourceWidth;
  }
  pixel = blockStart;
  for (rowCount = record->logicalHeight; rowCount != 0; rowCount--) {
    for (column = 0; column < record->logicalWidth; column++) {
      pixel[column] = backgroundIndex;
    }
    pixel = pixel + sourceWidth;
  }
  return true;
}

/* Decomposition of a subresource with direct ARGB8888 pixels: no palette, the new record table follows the
   header. Returns 0 (asset complete and shrunk), 0x2D (nothing but background), FATAL_ERROR_GENERAL_FAILURE
   (work area too small) or the allocator's error. */
static uint32_t GraphicsTextureDecompose_ArgbRegions
          (GraphicsTextureDecomposeState *state,const uint32_t *sourcePixels)
{
  GraphicsTextureSourceAsset *asset;
  uint32_t background;
  uint32_t edgeColor;
  uint32_t pixelCount;
  uint32_t scanLeft;
  const uint32_t *sourcePixel;
  uint32_t *workPixels;
  uint32_t *scanCursor;
  int columnsLeft;
  uint32_t tableBytes;
  uint32_t arenaError;
  uint32_t pixelDataOffset;
  GraphicsTextureSourceEntry *record;
  AssetSubresourceCount recordsLeft;

  asset = state->asset;
  (asset->tableDescriptor).paletteBankCount = 0;
  (asset->tableDescriptor).subresourceCount = 0;
  (asset->tableDescriptor).subresourceTableOffset = GFX_ASSET_HEADER_SIZE;
  state->records = GraphicsTextureSource_Entries(asset);
  /* the first pixel is the background; the first other pixel gives the edge colour */
  background = sourcePixels[0];
  pixelCount = state->sourceWidth * state->rowsRemaining;
  sourcePixel = sourcePixels;
  for (scanLeft = pixelCount; scanLeft != 0 && *sourcePixel == background; scanLeft--) {
    sourcePixel++;
  }
  if (scanLeft == 0) {
    /* nothing but background (error 0x2D, shared with the cursor frame check) */
    return FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE;
  }
  edgeColor = *sourcePixel;
  /* the working copy of the pixels goes to the end of the work area */
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,(int)(pixelCount * 4))) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  state->packedPixels = GraphicsTextureDecompose_WorkBytes(state) + state->freeBytes;
  workPixels = reinterpret_cast<uint32_t *>(state->packedPixels); /* ARGB8888 working copy, dword-aligned */
  GraphicsTextureDecompose_CopyDwords(workPixels,sourcePixels,pixelCount & DWORD_COUNT_MASK);
  state->edgeTransparent = (edgeColor & ARGB8888_ALPHA_MASK) == 0;
  state->packedPixelBytes = 0;
  /* Scan the working copy row by row for the next non-background pixel; each hit starts a region that is cut
     out and cleared to background, then the scan goes on at the same pixel. */
  scanCursor = workPixels;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    columnsLeft = state->sourceWidth;
    while (columnsLeft != 0) {
      if (*scanCursor != background) {
        if (!GraphicsTextureDecompose_CutArgbRegion(state,scanCursor,columnsLeft,background,edgeColor)) {
          return FATAL_ERROR_GENERAL_FAILURE;
        }
      }
      else {
        scanCursor++;
        columnsLeft--;
      }
    }
    state->rowsRemaining--;
  } while (state->rowsRemaining != 0);
  /* move the packed pixels down behind the record table, shrink the block to header + table + pixels
     and give every record its pixel offset, counting back from the end */
  tableBytes = (asset->tableDescriptor).subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
  (asset->common).allocationSizeBytes = state->packedPixelBytes + tableBytes + GFX_ASSET_HEADER_SIZE;
  GraphicsTextureDecompose_CopyDwords
            (GraphicsTextureDecompose_WorkBytes(state) + tableBytes,state->packedPixels,state->packedPixelBytes >> 2);
  arenaError = g_MemoryApi.shrinkInPlace((asset->common).allocationSizeBytes,asset);
  if (arenaError != 0) {
    return arenaError;
  }
  pixelDataOffset = (asset->common).allocationSizeBytes;
  record = state->records;
  recordsLeft = (asset->tableDescriptor).subresourceCount;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    pixelDataOffset = pixelDataOffset - record->pixelWidth * record->pixelHeight * 4;
    record->dataOffset = pixelDataOffset;
    record++;
    recordsLeft--;
  } while (recordsLeft != 0);
  return 0;
}

/* Decomposition of a subresource with 8-bit palette indices: the new asset carries the subresource's palette
   bank (`sourcePalette`) as its only bank and the record table follows it; otherwise as
   GraphicsTextureDecompose_ArgbRegions, on bytes. */
static uint32_t GraphicsTextureDecompose_IndexedRegions
          (GraphicsTextureDecomposeState *state,const uint8_t *sourcePalette,const uint8_t *sourcePixels)
{
  GraphicsTextureSourceAsset *asset;
  uint8_t backgroundIndex;
  uint8_t edgeIndex;
  int pixelCount;
  int scanLeft;
  const uint8_t *sourcePixel;
  uint8_t *workPixels;
  uint8_t *workPixel;
  uint8_t *scanCursor;
  GraphicsTexturePaletteEntry *edgeEntry;
  int columnsLeft;
  uint32_t tableBytes;
  uint32_t arenaError;
  uint32_t pixelDataOffset;
  GraphicsTextureSourceEntry *record;
  AssetSubresourceCount recordsLeft;

  asset = state->asset;
  (asset->tableDescriptor).paletteBankCount = 1;
  (asset->tableDescriptor).subresourceCount = 0;
  (asset->tableDescriptor).subresourceTableOffset = GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE;
  state->records = GraphicsTextureSource_Entries(asset);
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,GFX_PALETTE_BANK_SIZE)) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  GraphicsTextureDecompose_CopyDwords
            (GraphicsTextureSource_Bytes(asset) + GFX_ASSET_HEADER_SIZE,sourcePalette,GFX_PALETTE_BANK_SIZE / 4);
  backgroundIndex = sourcePixels[0];
  pixelCount = state->sourceWidth * state->rowsRemaining;
  sourcePixel = sourcePixels;
  for (scanLeft = pixelCount; scanLeft != 0 && *sourcePixel == backgroundIndex; scanLeft--) {
    sourcePixel++;
  }
  if (scanLeft == 0) {
    /* nothing but background (error 0x2D, shared with the cursor frame check) */
    return FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE;
  }
  edgeIndex = *sourcePixel;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,pixelCount)) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  workPixels = GraphicsTextureDecompose_WorkBytes(state) + state->freeBytes;
  sourcePixel = sourcePixels;
  workPixel = workPixels;
  for (scanLeft = pixelCount; scanLeft != 0; scanLeft--) {
    *workPixel = *sourcePixel;
    sourcePixel++;
    workPixel++;
  }
  /* the edge colour's entry in the new palette always loses its alpha, i.e. becomes transparent */
  /* the new asset seen as a palette texture source, whose bank follows the header */
  edgeEntry = &reinterpret_cast<GraphicsPaletteTextureSourceAsset *>(asset)->paletteEntries[(uint32_t)edgeIndex];
  state->edgeTransparent = (edgeEntry->argb8888 & ARGB8888_ALPHA_MASK) == 0;
  edgeEntry->argb8888 = edgeEntry->argb8888 & ARGB8888_RGB_MASK;
  /* packed sprites are padded to whole dwords: round the work copy's start down to a dword address */
  state->packedPixels = workPixels - (reinterpret_cast<uintptr_t>(workPixels) & 3);
  state->packedPixelBytes = 0;
  if (!GraphicsTextureDecompose_ReserveBytes(&state->freeBytes,3)) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  scanCursor = workPixels;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    columnsLeft = state->sourceWidth;
    while (columnsLeft != 0) {
      if (*scanCursor != backgroundIndex) {
        if (!GraphicsTextureDecompose_CutIndexedRegion(state,scanCursor,columnsLeft,backgroundIndex,edgeIndex)) {
          return FATAL_ERROR_GENERAL_FAILURE;
        }
      }
      else {
        scanCursor++;
        columnsLeft--;
      }
    }
    state->rowsRemaining--;
  } while (state->rowsRemaining != 0);
  tableBytes = (asset->tableDescriptor).subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE;
  (asset->common).allocationSizeBytes =
       state->packedPixelBytes + tableBytes + (GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE);
  GraphicsTextureDecompose_CopyDwords
            (GraphicsTextureDecompose_WorkBytes(state) + tableBytes,state->packedPixels,state->packedPixelBytes >> 2);
  arenaError = g_MemoryApi.shrinkInPlace((asset->common).allocationSizeBytes,asset);
  if (arenaError != 0) {
    return arenaError;
  }
  pixelDataOffset = (asset->common).allocationSizeBytes;
  record = state->records;
  recordsLeft = (asset->tableDescriptor).subresourceCount;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    pixelDataOffset = pixelDataOffset - (record->pixelWidth * record->pixelHeight + 3 & ~3u);
    record->dataOffset = pixelDataOffset;
    record++;
    recordsLeft--;
  } while (recordsLeft != 0);
  return 0;
}

/* Cuts one subresource of a 'gfx' texture source (a sheet of sprites) into its separate sprites and returns
   them as a new 'gfx' asset, one subresource per sprite (installed as
   g_GraphicsTextureSourceDecomposeSubresourceRegionsCf). The first pixel is the background; each block of
   other pixels (as wide as the run in its first row, as high as the run in its first column) becomes one
   subresource, trimmed of border rows/columns in the colour of the first non-background pixel when that colour
   is transparent, and is then cleared from the work copy. The work area is the largest free arena block,
   shrunk to the result at the end. Returns true with the new asset in *outAsset; returns false with
   FATAL_ERROR_GFX_ASSET_INVALID, 0x2D (nothing but background), FATAL_ERROR_GENERAL_FAILURE (work area too
   small) or the allocator's error in *outError. No caller in the game code (only the hook slot).
*/
bool GraphicsTextureSource_DecomposeSubresourceRegions
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset,
          GraphicsTextureSourceAsset **outAsset,uint32_t *outError)

{
  GraphicsTextureSourceEntry *sourceEntry;
  GraphicsTextureDecomposeState state;
  void *block;
  GraphicsTextureSourceAsset *decomposedAsset;
  GraphicsPaletteIndex paletteIndex;
  uint8_t *sourcePixels;
  uint32_t largestBlockSize;
  uint32_t packedDateTime;
  uint32_t error;

  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
     ((sourceAsset->tableDescriptor).subresourceCount <= entryIndex)) {
    *outError = FATAL_ERROR_GFX_ASSET_INVALID;
    return false;
  }
  sourceEntry = &GraphicsTextureSource_Entries(sourceAsset)[entryIndex];
  state.sourceWidth = sourceEntry->pixelWidth;
  state.rowsRemaining = sourceEntry->pixelHeight;
  error = g_MemoryApi.allocLargestFreeBlock(&block,&largestBlockSize);
  if (error != 0) {
    *outError = error;
    return false;
  }
  decomposedAsset = static_cast<GraphicsTextureSourceAsset *>(block);
  /* the new asset starts as a copy of the source header */
  GraphicsTextureDecompose_CopyDwords(decomposedAsset,sourceAsset,GFX_ASSET_HEADER_SIZE / 4);
  paletteIndex = sourceEntry->paletteIndex;
  state.asset = decomposedAsset;
  state.freeBytes = (int)largestBlockSize;
  if (!GraphicsTextureDecompose_ReserveBytes(&state.freeBytes,GFX_ASSET_HEADER_SIZE)) {
    error = FATAL_ERROR_GENERAL_FAILURE;
  }
  else {
    /* the new asset is stamped with the current time and date and this computer's name */
    packedDateTime = g_LocaleGetPackedCurrentTime();
    ((decomposedAsset)->common).buildMetadata.timestamps.timeValue1 = packedDateTime;
    ((decomposedAsset)->common).buildMetadata.timestamps.timeValue2 = packedDateTime;
    packedDateTime = g_LocaleGetPackedCurrentDate();
    ((decomposedAsset)->common).buildMetadata.timestamps.dateValue1 = packedDateTime;
    ((decomposedAsset)->common).buildMetadata.timestamps.dateValue2 = packedDateTime;
    g_LocaleCopyDefaultComputerLabelUtf16
              (((decomposedAsset)->common).buildMetadata.names.sourceName);
    sourcePixels = GraphicsTextureSource_Bytes(sourceAsset) + sourceEntry->dataOffset;
    if (paletteIndex == -1) {
      error = GraphicsTextureDecompose_ArgbRegions
                        (&state,reinterpret_cast<const uint32_t *>(sourcePixels)); /* ARGB8888 pixels */
    }
    else {
      error = GraphicsTextureDecompose_IndexedRegions
                        (&state,GraphicsTextureSource_Bytes(sourceAsset) + GFX_ASSET_HEADER_SIZE +
                                paletteIndex * GFX_PALETTE_BANK_SIZE,
                         sourcePixels);
    }
    if (error == 0) {
      *outAsset = decomposedAsset;
      return true;
    }
  }
  g_MemoryApi.free(decomposedAsset);
  *outError = error;
  return false;
}
