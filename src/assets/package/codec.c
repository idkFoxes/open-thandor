/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/codec.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/codec.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/package/codec. */

/* Success exit of a codec (PckCodecProc): stores byteCount in *outByteCount when it is not NULL. */
static bool PckCodec_Succeed(uint32_t *outByteCount,uint32_t byteCount)
{
  if (outByteCount != NULL) {
    *outByteCount = byteCount;
  }
  return true;
}

/* Failure exit of a codec (PckCodecProc): stores errorCode in *outErrorCode when it is not NULL. */
static bool PckCodec_Fail(uint32_t *outErrorCode,uint32_t errorCode)
{
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return false;
}

/* Address: 0x0040A9C0.
   PCK compression method 2 writer for field grids: keeps only the header and the four persisted dwords of each
   0x80-byte cell (the rest is runtime state that the decoder regenerates), then packs that compact image with
   method 0 behind a PCK_FIELD_GRID_PREFIX_BYTES prefix holding its size. Returns the packed size including the
   prefix in *outByteCount (true), or false with the error code of the allocation or the method-0 encoder in
   *outErrorCode.
*/
bool PckCodec_EncodeFieldGrid(PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  AssetMagic pendingCellDword;
  AssetMagic *compactFieldImageBase;
  PckHeaderDwordCount headerDwordCount;
  uint32_t cellCount;
  uint32_t bytes;
  PckCompactFieldImageByteCount compactImageSizeBytes;
  AssetMagic *compactWriteCursor;
  AssetMagic *headerReadCursor;
  FieldGridCell *sourceCell;
  uint32_t allocError;
  uint32_t encodedByteCount;
  uint32_t encodeErrorCode;
  AssetMagic persistedCellDword;
  
  cellCount = sourceGrid->gridWidth * sourceGrid->gridHeight;
  bytes = cellCount * FIELD_GRID_COMPACT_CELL_BYTES + FIELD_GRID_HEADER_BYTES;
  allocError = g_MemoryApi.alloc(bytes,(void **)&compactFieldImageBase);
  if (allocError != 0) {
    compactFieldImageBase = (AssetMagic *)allocError;
  }
  else {
    compactWriteCursor = compactFieldImageBase;
    /* the header dword by dword; the read cursor then points at cells[0] */
    headerReadCursor = (AssetMagic *)sourceGrid;
    for (headerDwordCount = FIELD_GRID_HEADER_DWORDS; headerDwordCount != 0; headerDwordCount--) {
      *compactWriteCursor = *headerReadCursor;
      headerReadCursor++;
      compactWriteCursor++;
    }
    sourceCell = (FieldGridCell *)headerReadCursor;
    cellCount = cellCount & 0xfffffff; /* (count * 0x10) >> 4 in the original */
    /* per cell: persistedAux54, terrainHeight, waterSurfaceDelta and flagsAndMaterial (cell offsets 0x54, 0x48,
       0x4C and 0x50) */
    do {
      persistedCellDword = sourceCell->terrainHeight;
      *compactWriteCursor = sourceCell->persistedAux54;
      compactWriteCursor[1] = persistedCellDword;
      pendingCellDword = sourceCell->flagsAndMaterial;
      compactWriteCursor[2] = sourceCell->waterSurfaceDelta;
      compactWriteCursor[3] = pendingCellDword;
      sourceCell++;
      compactWriteCursor = compactWriteCursor + 4;
      cellCount--;
    } while (cellCount != 0);
    *(uint32_t *)destination = bytes;
    if (PckCodec_EncodeHuffmanRle
            (destinationCapacityBytes - PCK_FIELD_GRID_PREFIX_BYTES,destination + PCK_FIELD_GRID_PREFIX_BYTES,bytes,
             (uint8_t *)compactFieldImageBase,&encodedByteCount,&encodeErrorCode)) {
      g_MemoryApi.free(compactFieldImageBase);
      return PckCodec_Succeed(outByteCount,encodedByteCount + PCK_FIELD_GRID_PREFIX_BYTES);
    }
    g_MemoryApi.free(compactFieldImageBase);
    return PckCodec_Fail(outErrorCode,encodeErrorCode);
  }
  return PckCodec_Fail(outErrorCode,(uint32_t)compactFieldImageBase);
}


/* Copies the compact image's header into destinationGrid, clears all cells and expands every 0x10-byte record
   into its cell: record dwords 0..3 go to persistedAux54, terrainHeight, waterSurfaceDelta and
   flagsAndMaterial. The cell count comes from header dwords 0x2E/0x2F (gridWidth/gridHeight); a count of 0 is not
   guarded (the expand loop would run 2^32 times). */
static void PckCodec_ExpandFieldGridImage(FieldGridAsset *destinationGrid,AssetMagic *compactImage)
{
  uint32_t cellCount;
  uint32_t dwordsLeft;
  AssetMagic *compactReadCursor;
  AssetMagic *expandedHeaderCursor;
  uint32_t *expandedZeroCursor;
  FieldGridCell *expandedCell;

  cellCount = compactImage[46] * compactImage[47];
  compactReadCursor = compactImage;
  expandedHeaderCursor = (AssetMagic *)destinationGrid;
  for (dwordsLeft = FIELD_GRID_HEADER_DWORDS; dwordsLeft != 0; dwordsLeft--) {
    *expandedHeaderCursor = *compactReadCursor;
    compactReadCursor++;
    expandedHeaderCursor++;
  }
  /* the header cursor now points at cells[0]; clear all cells dword by dword */
  expandedCell = (FieldGridCell *)expandedHeaderCursor;
  expandedZeroCursor = (uint32_t *)expandedHeaderCursor;
  for (dwordsLeft = cellCount * FIELD_GRID_CELL_DWORDS; dwordsLeft != 0; dwordsLeft--) {
    *expandedZeroCursor = 0;
    expandedZeroCursor++;
  }
  do {
    expandedCell->persistedAux54 = compactReadCursor[0];
    expandedCell->terrainHeight = compactReadCursor[1];
    expandedCell->waterSurfaceDelta = compactReadCursor[2];
    expandedCell->flagsAndMaterial = (FieldCellPackedFlagsAndMaterial)compactReadCursor[3];
    compactReadCursor = compactReadCursor + 4;
    expandedCell++;
    cellCount--;
  } while (cellCount != 0);
}

/* Regenerates the world coordinates of every cell row by row: worldX = column * 0x901 + row * 0x480,
   worldY = row * -1999 (Q12, 32-bit wrap). */
static void PckCodec_GenerateFieldGridWorldCoordinates(FieldGridAsset *grid)
{
  FieldGridDimension gridWidth;
  FieldGridDimension rowsRemaining;
  FieldGridDimension columnsRemaining;
  FieldGridCell *cell;
  int rowStartX;
  int worldX;
  Q12 worldYQ12;

  gridWidth = grid->gridWidth;
  rowsRemaining = grid->gridHeight;
  cell = grid->cells;
  rowStartX = 0;
  worldYQ12 = 0;
  do {
    worldX = rowStartX;
    columnsRemaining = gridWidth;
    do {
      cell->worldX = worldX;
      cell->worldY = worldYQ12;
      worldX = worldX + FIELD_GRID_WORLD_COLUMN_STEP_X;
      cell++;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowStartX = rowStartX + FIELD_GRID_WORLD_ROW_STEP_X;
    worldYQ12 = worldYQ12 + FIELD_GRID_WORLD_ROW_STEP_Y;
    rowsRemaining = rowsRemaining - 1;
  } while (rowsRemaining != 0);
}

/* Address: 0x0040AAA0.
   PCK compression method 2 reader for field grids (see PckCodec_EncodeFieldGrid): unpacks the compact image,
   restores the header, expands every 0x10-byte record into a zeroed FieldGridCell and regenerates the cell world
   coordinates: worldX = column * 0x901 + row * 0x480, worldY = row * -1999 (Q12, 32-bit wrap). Returns true,
   or false with the error code of the allocation in *outErrorCode.
   Original quirk: when the method-0 decoder fails, the error code is not its code but what the following free
   returned (0 unless the heap is corrupt); on success the byte count is likewise the free's return value.
*/
bool PckCodec_DecodeFieldGrid(PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid,
          PckStoredByteCount sourceSizeBytes,uint8_t *source,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  uint32_t bytes;
  AssetMagic *compactFieldImageBase;
  uint32_t allocError;
  uint32_t freeStatus;

  bytes = *(uint32_t *)source;
  allocError = g_MemoryApi.alloc(bytes,(void **)&compactFieldImageBase);
  if (allocError != 0) {
    return PckCodec_Fail(outErrorCode,allocError);
  }
  if (!PckCodec_DecodeHuffmanRle
          (bytes,(uint8_t *)compactFieldImageBase,sourceSizeBytes - PCK_FIELD_GRID_PREFIX_BYTES,
           source + PCK_FIELD_GRID_PREFIX_BYTES,NULL,NULL)) {
    /* Original quirk: reports the free's return value, not the decoder's error code */
    freeStatus = g_MemoryApi.free(compactFieldImageBase);
    return PckCodec_Fail(outErrorCode,freeStatus);
  }
  PckCodec_ExpandFieldGridImage(destinationGrid,compactFieldImageBase);
  PckCodec_GenerateFieldGridWorldCoordinates(destinationGrid);
  freeStatus = g_MemoryApi.free(compactFieldImageBase);
  return PckCodec_Succeed(outByteCount,freeStatus);
}


/* Address: 0x0040A960.
   PCK compression method 1 writer ("stored"), called through slot 1 of g_PckEncoderTable (0x0040E224).
   Copies the source dword by dword when it fits into the destination and returns true with its size rounded up
   to four bytes in *outByteCount; false with FATAL_ERROR_GENERAL_FAILURE in *outErrorCode when it does not fit.
*/
bool PckCodec_EncodeStored(PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceSizeBytes,uint8_t *source,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckDwordCopyCount dwordCopyCount;

  if (sourceSizeBytes <= destinationCapacityBytes) {
    /* only whole dwords are copied (REP MOVSD); a 1..3-byte tail is left out */
    for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0; dwordCopyCount--) {
      *(uint32_t *)destination = *(uint32_t *)source;
      source = source + 4;
      destination = destination + 4;
    }
    return PckCodec_Succeed(outByteCount,sourceSizeBytes + 3 & PACKAGE_DWORD_ALIGN_MASK);
  }
  return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
}


/* Address: 0x0040A9A0.
   PCK compression method 1 reader ("stored"), called through slot 1 of g_PckDecoderTable (0x0040E230).
   Copies the stored bytes dword by dword to the destination; the capacity is not checked.
   Original quirk: the outcome is what SHR ECX,2 shifted out last, so it fails exactly when bit 1 of
   sourceSizeBytes is set (stored sizes written by the encoder are multiples of four, so it succeeds for them).
   Either way the reported value (byte count or error code) is sourceSizeBytes: EAX is untouched and in
   Package_DecodeEntryInto still holds the read size (packedSize).
*/
bool PckCodec_DecodeStored(PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckStoredByteCount sourceSizeBytes,uint8_t *source,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckDwordCopyCount dwordCopyCount;

  for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0; dwordCopyCount--) {
    *(uint32_t *)destination = *(uint32_t *)source;
    source = source + 4;
    destination = destination + 4;
  }
  if ((sourceSizeBytes >> 1 & 1) != 0) {
    return PckCodec_Fail(outErrorCode,sourceSizeBytes);
  }
  return PckCodec_Succeed(outByteCount,sourceSizeBytes);
}


/* Bitstream writer state of PckCodec_EncodeHuffmanRle. */
typedef struct PckHuffmanBitWriter {
  uint32_t *window;              /* dword the tokens are ORed into; advanced byte by byte */
  PckHuffmanBitOffset bitOffset; /* next free bit in the window's first byte (0..7) */
  uint32_t packedSizeBytes;      /* table + bytes passed + 0x1F (see PckCodec_EncodeHuffmanRle) */
  uint32_t freeBytes;            /* output bytes still free; reaching 0 fails */
} PckHuffmanBitWriter;

/* Clears the symbol table and both node workspaces, then counts how often each byte value occurs in source.
   An empty source is not guarded: the count loop would run 2^32 times. */
static void PckCodec_EncoderCountFrequencies(uint8_t *source,PckDecodedByteCount sourceSizeBytes)
{
  uint32_t *workspaceClearCursor;
  int clearDwordCount;

  workspaceClearCursor = (uint32_t *)g_PckHuffmanSymbolWorkspace256;
  for (clearDwordCount = PCK_HUFFMAN_WORKSPACE_DWORDS; clearDwordCount != 0; clearDwordCount--) {
    *workspaceClearCursor = 0;
    workspaceClearCursor++;
  }
  do {
    g_PckHuffmanSymbolWorkspace256[*source].frequencyCount =
         g_PckHuffmanSymbolWorkspace256[*source].frequencyCount + 1;
    sourceSizeBytes--;
    source++;
  } while (sourceSizeBytes != 0);
}

/* Scales the counts down until the largest fits the 8-bit table; rounding up keeps rare symbols nonzero. */
static void PckCodec_EncoderScaleFrequencies(void)
{
  uint32_t maxCount;
  int scaleShift;
  int symbolIndex;
  PckHuffmanSymbolState *symbolState;

  maxCount = 0;
  for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_SYMBOL_COUNT; symbolIndex++) {
    if (maxCount < g_PckHuffmanSymbolWorkspace256[symbolIndex].frequencyCount) {
      maxCount = g_PckHuffmanSymbolWorkspace256[symbolIndex].frequencyCount;
    }
  }
  scaleShift = 0;
  for (; 255 < maxCount; maxCount = (maxCount + 1) >> 1) {
    scaleShift++;
  }
  if (scaleShift != 0) {
    for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_SYMBOL_COUNT; symbolIndex++) {
      symbolState = &g_PckHuffmanSymbolWorkspace256[symbolIndex];
      symbolState->frequencyCount = symbolState->frequencyCount + (1 << ((uint8_t)scaleShift & 31)) - 1;
      symbolState->frequencyCount = symbolState->frequencyCount >> ((uint8_t)scaleShift & 31);
    }
  }
}

/* Scans all leaf and internal nodes (the two workspaces are contiguous) for the two lightest nodes with nonzero
   weight. Returns false when fewer than two are left (the second-lowest weight is still UINT32_MAX, tested as
   negative like the original). */
static bool PckCodec_EncoderFindTwoLightestNodes(PckHuffmanNode **outLowestNode,uint32_t *outLowestWeight,
          PckHuffmanNode **outSecondLowestNode,uint32_t *outSecondLowestWeight)
{
  PckHuffmanNode *scanNode;
  PckHuffmanNode *lowestNode;
  PckHuffmanNode *secondLowestNode;
  uint32_t lowestWeight;
  uint32_t secondLowestWeight;
  int nodesLeft;

  lowestNode = NULL;
  secondLowestNode = NULL;
  lowestWeight = UINT32_MAX;
  secondLowestWeight = UINT32_MAX;
  scanNode = g_PckHuffmanLeafNodeWorkspace256;
  for (nodesLeft = PCK_HUFFMAN_NODE_COUNT; nodesLeft != 0; nodesLeft--) {
    if (scanNode->weight != 0) {
      if (scanNode->weight < lowestWeight) {
        /* the previous lowest becomes the second-lowest */
        if (lowestWeight < secondLowestWeight) {
          secondLowestWeight = lowestWeight;
          secondLowestNode = lowestNode;
        }
        lowestWeight = scanNode->weight;
        lowestNode = scanNode;
      }
      else if (scanNode->weight < secondLowestWeight) {
        secondLowestWeight = scanNode->weight;
        secondLowestNode = scanNode;
      }
    }
    scanNode++;
  }
  *outLowestNode = lowestNode;
  *outLowestWeight = lowestWeight;
  *outSecondLowestNode = secondLowestNode;
  *outSecondLowestWeight = secondLowestWeight;
  return (int)secondLowestWeight >= 0;
}

/* Copies the scaled counts into the leaf weights, then joins the two lightest live nodes under a new internal
   node until only the root still has a weight; a joined node's weight is cleared, so the root is the only node
   left with nonzero weight. Returns false when all 256 internal nodes are used up. */
static bool PckCodec_EncoderBuildTree(void)
{
  int symbolIndex;
  PckHuffmanNodePtr nextInternalNode;
  PckHuffmanNode *lowestNode;
  PckHuffmanNode *secondLowestNode;
  uint32_t lowestWeight;
  uint32_t secondLowestWeight;

  for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_SYMBOL_COUNT; symbolIndex++) {
    g_PckHuffmanLeafNodeWorkspace256[symbolIndex].weight = g_PckHuffmanSymbolWorkspace256[symbolIndex].frequencyCount;
  }
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  while (PckCodec_EncoderFindTwoLightestNodes(&lowestNode,&lowestWeight,&secondLowestNode,&secondLowestWeight)) {
    nextInternalNode->weight = lowestWeight + secondLowestWeight;
    nextInternalNode->zeroChild = lowestNode;
    nextInternalNode->oneChild = secondLowestNode;
    lowestNode->parent = nextInternalNode;
    secondLowestNode->parent = nextInternalNode;
    lowestNode->weight = 0;
    secondLowestNode->weight = 0;
    nextInternalNode++;
    /* Workspace exhausted: all 256 internal nodes used (original: CMP next,end; JC continue). */
    if (g_PckHuffmanInternalNodeWorkspace256 + PCK_HUFFMAN_SYMBOL_COUNT <= nextInternalNode) {
      return false;
    }
  }
  return true;
}

/* Writes the frequency table: the low byte of each scaled count. */
static void PckCodec_EncoderWriteFrequencyTable(uint8_t *destination)
{
  int symbolIndex;

  for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_FREQUENCY_TABLE_BYTES; symbolIndex++) {
    destination[symbolIndex] = *(uint8_t *)&g_PckHuffmanSymbolWorkspace256[symbolIndex].frequencyCount;
  }
}

/* Replaces each used symbol's count with its code: walking from the leaf up to the root collects the code with
   the root's bit lowest (the order the decoder reads it); each entry becomes code bits 0..23 | code length << 24.
   Original quirk: with a single distinct byte value the tree has no internal node, the leaf's parent is NULL
   and the walk dereferences it. */
static void PckCodec_EncoderAssignCodes(void)
{
  uint32_t symbolIndex;
  PckHuffmanSymbolState *symbolState;
  PckHuffmanNodePtr currentNode;
  PckHuffmanNodePtr ancestorNode;
  uint32_t codeBits;
  int codeLength;

  for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_SYMBOL_COUNT; symbolIndex++) {
    symbolState = &g_PckHuffmanSymbolWorkspace256[symbolIndex];
    if (symbolState->frequencyCount != 0) {
      codeLength = 0;
      codeBits = 0;
      currentNode = g_PckHuffmanLeafNodeWorkspace256 + symbolIndex;
      do {
        ancestorNode = currentNode->parent;
        codeBits = codeBits * 2;
        codeLength++;
        if (currentNode == ancestorNode->oneChild) {
          codeBits++;
        }
        currentNode = ancestorNode;
      } while (ancestorNode->weight == 0);
      symbolState->frequencyCount = codeBits | codeLength * (1 << PCK_HUFFMAN_CODE_LENGTH_SHIFT);
    }
  }
}

/* ORs one token into the output: tokenHeader in its headerBitCount flag/count bits, then the code of symbol.
   Then moves the window on by the whole bytes written. Returns false when the output runs full. */
static bool PckCodec_EncoderEmitToken(PckHuffmanBitWriter *output,uint32_t tokenHeader,uint8_t headerBitCount,
          uint8_t symbol)
{
  uint32_t symbolCode;

  *output->window = *output->window | tokenHeader << (output->bitOffset & 31);
  symbolCode = g_PckHuffmanSymbolWorkspace256[symbol].frequencyCount;
  *output->window = *output->window | (symbolCode & 0xffffff) << (output->bitOffset + headerBitCount & 31);
  for (output->bitOffset =
            output->bitOffset + headerBitCount + (char)(symbolCode >> PCK_HUFFMAN_CODE_LENGTH_SHIFT);
       7 < output->bitOffset; output->bitOffset = output->bitOffset - 8) {
    /* advance the dword write window by one byte */
    output->window = (uint32_t *)((uint8_t *)output->window + 1);
    output->packedSizeBytes++;
    output->freeBytes--;
    if (output->freeBytes == 0) {
      return false;
    }
  }
  return true;
}

/* Encodes the source as run tokens (3..18 equal bytes: flag 1 + (count - 3) in 4 bits = count*2 - 5, then the
   byte's code) and literal tokens (flag bit 0, then the byte's code). Returns false when the output runs full. */
static bool PckCodec_EncoderWriteTokens(PckHuffmanBitWriter *output,uint8_t *source,
          PckDecodedByteCount sourceBytesLeft)
{
  uint8_t symbol;
  uint32_t runLength;

  do {
    symbol = *source;
    if (PCK_HUFFMAN_MIN_RUN_LENGTH <= sourceBytesLeft && symbol == source[1] && symbol == source[2]) {
      runLength = 0;
      while (sourceBytesLeft != 0 && *source == symbol && runLength < 18) {
        runLength++;
        source++;
        sourceBytesLeft--;
      }
      if (!PckCodec_EncoderEmitToken(output,runLength * 2 - 5,5,symbol)) {
        return false;
      }
    }
    else {
      /* the literal flag is a 0 bit, so ORing the header changes nothing */
      if (!PckCodec_EncoderEmitToken(output,0,1,symbol)) {
        return false;
      }
      source++;
      sourceBytesLeft--;
    }
  } while (sourceBytesLeft != 0);
  return true;
}

/* Address: 0x0040A4C0.
   PCK compression method 0 writer. Counts the byte frequencies of the source, scales them to 8 bits, builds a
   Huffman tree from them, writes the 256-byte frequency table and then the bitstream of literal and 3..18-byte
   run tokens (format in codec.h). Returns the packed size (rounded up to 16 bytes, with at least 16 bytes of
   slack for the decoder's dword reads) in *outByteCount with true, or false with FATAL_ERROR_GENERAL_FAILURE in
   *outErrorCode when the tree overflows or the output does not fit.
*/
bool PckCodec_EncodeHuffmanRle(PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceSizeBytes,uint8_t *source,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  uint32_t alignedOutputBytes;
  uint32_t *outputClearCursor;
  uint32_t clearDwordCount;
  PckHuffmanBitWriter output;

  PckCodec_EncoderCountFrequencies(source,sourceSizeBytes);
  PckCodec_EncoderScaleFrequencies();
  if (!PckCodec_EncoderBuildTree()) {
    return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
  }
  if (destinationCapacityBytes < PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) {
    return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
  }
  PckCodec_EncoderWriteFrequencyTable(destination);
  PckCodec_EncoderAssignCodes();
  /* Zero the dword-aligned output area, since the tokens are ORed into it. */
  alignedOutputBytes = (destinationCapacityBytes - PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) & PACKAGE_DWORD_ALIGN_MASK;
  if (alignedOutputBytes == 0) {
    return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
  }
  outputClearCursor = (uint32_t *)(destination + PCK_HUFFMAN_FREQUENCY_TABLE_BYTES);
  for (clearDwordCount = (destinationCapacityBytes - PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) >> 2; clearDwordCount != 0;
       clearDwordCount--) {
    *outputClearCursor = 0;
    outputClearCursor++;
  }
  /* SUB [capacity],4 / JBE fail (0x0040A69F): exactly 4 free bytes also fail */
  if (alignedOutputBytes <= 4) {
    return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
  }
  output.window = (uint32_t *)(destination + PCK_HUFFMAN_FREQUENCY_TABLE_BYTES);
  output.bitOffset = 0;
  /* table + 0x1F, so the final AND with ~0xF rounds up and adds at least 16 bytes of slack */
  output.packedSizeBytes = PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 31;
  output.freeBytes = alignedOutputBytes - 4;
  if (!PckCodec_EncoderWriteTokens(&output,source,sourceSizeBytes)) {
    return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
  }
  if (output.bitOffset != 0) {
    output.packedSizeBytes++;
  }
  return PckCodec_Succeed(outByteCount,output.packedSizeBytes & 0xfffffff0);
}


/* Loads the 256-byte frequency table into the symbol table, clears the leaf and internal node workspaces (0x800
   dwords behind the symbol table) and copies the frequencies into the leaf weights. */
static void PckCodec_DecoderLoadFrequencies(uint8_t *frequencyTable)
{
  uint32_t *workspaceClearCursor;
  int clearDwordCount;
  int symbolIndex;

  for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_SYMBOL_COUNT; symbolIndex++) {
    g_PckHuffmanSymbolWorkspace256[symbolIndex].frequencyCount = frequencyTable[symbolIndex];
  }
  workspaceClearCursor = (uint32_t *)g_PckHuffmanLeafNodeWorkspace256;
  for (clearDwordCount = 2048; clearDwordCount != 0; clearDwordCount--) {
    *workspaceClearCursor = 0;
    workspaceClearCursor++;
  }
  for (symbolIndex = 0; symbolIndex < PCK_HUFFMAN_SYMBOL_COUNT; symbolIndex++) {
    g_PckHuffmanLeafNodeWorkspace256[symbolIndex].weight = g_PckHuffmanSymbolWorkspace256[symbolIndex].frequencyCount;
  }
}

/* Same tree construction as PckCodec_EncodeHuffmanRle, so both sides get identical codes. Returns the root (the
   last internal node created), or NULL when all 256 internal nodes are used up. Leaves have no zeroChild.
   Original quirk: with fewer than two weighted symbols no internal node is created and the "root" is the node
   just before the internal node workspace (the last leaf). */
static PckHuffmanNode *PckCodec_DecoderBuildTree(void)
{
  PckHuffmanNodePtr nextInternalNode;
  PckHuffmanNode *lowestNode;
  PckHuffmanNode *secondLowestNode;
  uint32_t lowestWeight;
  uint32_t secondLowestWeight;

  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  while (PckCodec_EncoderFindTwoLightestNodes(&lowestNode,&lowestWeight,&secondLowestNode,&secondLowestWeight)) {
    nextInternalNode->weight = lowestWeight + secondLowestWeight;
    nextInternalNode->zeroChild = lowestNode;
    nextInternalNode->oneChild = secondLowestNode;
    lowestNode->parent = nextInternalNode;
    secondLowestNode->parent = nextInternalNode;
    lowestNode->weight = 0;
    secondLowestNode->weight = 0;
    nextInternalNode++;
    /* The original compares with the next function (PckCodec_EncodeHuffmanRle), whose code starts
       where the internal node workspace ends. */
    if (g_PckHuffmanInternalNodeWorkspace256 + PCK_HUFFMAN_SYMBOL_COUNT <= nextInternalNode) {
      return NULL;
    }
  }
  return nextInternalNode - 1;
}

/* Walks from the root to a leaf, consuming one bit of *codeBits (lowest first: 0 = zeroChild, 1 = oneChild)
   and advancing *bitOffset per step. Returns the leaf; its index in the leaf workspace is the symbol. */
static PckHuffmanNode *PckCodec_DecoderReadSymbol(PckHuffmanNode *root,uint32_t *codeBits,
          PckHuffmanBitOffset *bitOffset)
{
  PckHuffmanNode *node;

  node = root;
  do {
    if ((*codeBits & 1) == 0) {
      node = node->zeroChild;
    }
    else {
      node = node->oneChild;
    }
    *codeBits = *codeBits >> 1;
    *bitOffset = *bitOffset + 1;
  } while (node->zeroChild != NULL);
  return node;
}

/* Moves the input byte cursor past every whole byte consumed, leaving a bit offset of 0..7. */
static uint8_t *PckCodec_DecoderSkipWholeBytes(uint8_t *inputByte,PckHuffmanBitOffset *bitOffset)
{
  while (7 < *bitOffset) {
    *bitOffset = *bitOffset - 8;
    inputByte++;
  }
  return inputByte;
}

/* Address: 0x0040A790.
   PCK compression method 0 reader. Rebuilds the encoder's Huffman tree from the 256-byte frequency table at
   the start of source, then decodes literal and run tokens (format in codec.h) until outputSizeBytes bytes are
   written. Returns true on success; false with FATAL_ERROR_GENERAL_FAILURE in *outErrorCode when the tree
   overflows the workspace. sourceSizeBytes is not checked: the bitstream is trusted.
   Original quirk: the byte count reported on success is not a size but what is left of the last token: the
   unused code bits of a literal, or the run counter of a run (EAX).
*/
bool PckCodec_DecodeHuffmanRle
          (PckDecodedByteCount outputSizeBytes,uint8_t *destination,PckStoredByteCount sourceSizeBytes,
          uint8_t *source,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckHuffmanNode *root;
  PckHuffmanNode *symbolNode;
  uint8_t *inputByte;
  PckHuffmanBitOffset inputBitOffset;
  uint32_t bitWindow;
  uint32_t codeBits;
  PckHuffmanRunLength runLength;
  /* what is left of the last token; reported as the byte count (see the quirk above) */
  PckHuffmanRunLength lastTokenLeftover;

  PckCodec_DecoderLoadFrequencies(source);
  root = PckCodec_DecoderBuildTree();
  if (root == NULL) {
    return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
  }
  /* The input is read as a dword window that advances byte by byte. */
  inputBitOffset = 0;
  inputByte = source + PCK_HUFFMAN_FREQUENCY_TABLE_BYTES;
  do {
    bitWindow = *(uint32_t *)inputByte >> (inputBitOffset & 31);
    if ((bitWindow & 1) == 0) {
      /* literal token: flag bit 0, then the code of the byte */
      codeBits = bitWindow >> 1;
      inputBitOffset = inputBitOffset + 1;
      symbolNode = PckCodec_DecoderReadSymbol(root,&codeBits,&inputBitOffset);
      *destination = (uint8_t)(symbolNode - g_PckHuffmanLeafNodeWorkspace256) /* symbol = leaf index */;
      destination++;
      inputByte = PckCodec_DecoderSkipWholeBytes(inputByte,&inputBitOffset);
      outputSizeBytes--;
      lastTokenLeftover = codeBits;
    }
    else {
      /* run token: flag bit 1, 4-bit (length - 3), then the code of the repeated byte */
      codeBits = bitWindow >> 5;
      inputBitOffset = inputBitOffset + 5;
      symbolNode = PckCodec_DecoderReadSymbol(root,&codeBits,&inputBitOffset);
      inputByte = PckCodec_DecoderSkipWholeBytes(inputByte,&inputBitOffset);
      runLength = (bitWindow >> 1 & 0xf) + PCK_HUFFMAN_MIN_RUN_LENGTH;
      /* a run is cut short when the output is full; the counter then keeps its value */
      do {
        *destination = (uint8_t)(symbolNode - g_PckHuffmanLeafNodeWorkspace256) /* symbol = leaf index */;
        destination++;
        outputSizeBytes--;
        if (outputSizeBytes == 0) break;
        runLength--;
      } while (runLength != 0);
      lastTokenLeftover = runLength;
    }
  } while (outputSizeBytes != 0);
  return PckCodec_Succeed(outByteCount,lastTokenLeftover);
}

