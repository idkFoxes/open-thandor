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
  AssetMagic pendingCellDword;
  AssetMagic *compactFieldImageBase;
  int countOrRowStartX;
  FieldGridDimension columnsRemaining;
  int cellCountOrWorldX;
  FieldGridDimension rowsRemaining;
  Q12 currentWorldYQ12;
  AssetMagic *compactReadCursor;
  FieldGridCell *currentWorldCoordinateCell;
  uint32_t *expandedZeroCursor;
  AssetMagic *expandedHeaderCursor;
  FieldGridCell *expandedCell;
  uint32_t allocError;
  uint32_t freeStatus;
  FieldGridDimension gridWidth;
  
  bytes = *(uint32_t *)source;
  allocError = g_MemoryApi.alloc(bytes,(void **)&compactFieldImageBase);
  if (allocError != 0) {
    compactFieldImageBase = (AssetMagic *)allocError;
  }
  else {
    if (PckCodec_DecodeHuffmanRle
            (bytes,(uint8_t *)compactFieldImageBase,sourceSizeBytes - PCK_FIELD_GRID_PREFIX_BYTES,
             source + PCK_FIELD_GRID_PREFIX_BYTES,NULL,NULL)) {
      /* header dwords 0x2E/0x2F are gridWidth/gridHeight */
      cellCountOrWorldX = compactFieldImageBase[46] * compactFieldImageBase[47];
      compactReadCursor = compactFieldImageBase;
      expandedHeaderCursor = (AssetMagic *)destinationGrid;
      for (countOrRowStartX = FIELD_GRID_HEADER_DWORDS; countOrRowStartX != 0; countOrRowStartX--) {
        *expandedHeaderCursor = *compactReadCursor;
        compactReadCursor++;
        expandedHeaderCursor++;
      }
      /* the header cursor now points at cells[0]; clear all cells dword by dword */
      expandedCell = (FieldGridCell *)expandedHeaderCursor;
      expandedZeroCursor = (uint32_t *)expandedHeaderCursor;
      for (countOrRowStartX = cellCountOrWorldX * FIELD_GRID_CELL_DWORDS; countOrRowStartX != 0;
          countOrRowStartX--) {
        *expandedZeroCursor = 0;
        expandedZeroCursor++;
      }
      /* per cell: record dwords 0..3 to persistedAux54, terrainHeight, waterSurfaceDelta, flagsAndMaterial */
      do {
        pendingCellDword = compactReadCursor[1];
        expandedCell->persistedAux54 = *compactReadCursor;
        expandedCell->terrainHeight = pendingCellDword;
        pendingCellDword = compactReadCursor[3];
        expandedCell->waterSurfaceDelta = compactReadCursor[2];
        expandedCell->flagsAndMaterial = pendingCellDword;
        compactReadCursor = compactReadCursor + 4;
        expandedCell++;
        cellCountOrWorldX--;
      } while (cellCountOrWorldX != 0);
      cellCountOrWorldX = 0;
      currentWorldYQ12 = 0;
      gridWidth = destinationGrid->gridWidth;
      rowsRemaining = destinationGrid->gridHeight;
      currentWorldCoordinateCell = destinationGrid->cells;
      columnsRemaining = gridWidth;
      countOrRowStartX = 0;
      do {
        do {
          currentWorldCoordinateCell->worldX = cellCountOrWorldX;
          currentWorldCoordinateCell->worldY = currentWorldYQ12;
          cellCountOrWorldX = cellCountOrWorldX + FIELD_GRID_WORLD_COLUMN_STEP_X;
          currentWorldCoordinateCell++;
          columnsRemaining = columnsRemaining - 1;
        } while (columnsRemaining != 0);
        cellCountOrWorldX = countOrRowStartX + FIELD_GRID_WORLD_ROW_STEP_X;
        currentWorldYQ12 = currentWorldYQ12 + FIELD_GRID_WORLD_ROW_STEP_Y;
        rowsRemaining = rowsRemaining - 1;
        columnsRemaining = gridWidth;
        countOrRowStartX = cellCountOrWorldX;
      } while (rowsRemaining != 0);
      freeStatus = g_MemoryApi.free(compactFieldImageBase);
      return PckCodec_Succeed(outByteCount,freeStatus);
    }
    freeStatus = g_MemoryApi.free(compactFieldImageBase);
    compactFieldImageBase = (AssetMagic *)freeStatus;
  }
  return PckCodec_Fail(outErrorCode,(uint32_t)compactFieldImageBase);
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
  PckHuffmanNodePtr ancestorNode;
  /* maximum frequency, then lowest weight while building the tree, then symbol index while building the
     code table, then the packed output size */
  uint32_t maxCountOrWeightOrSize;
  PckHuffmanBitOffset outputBitOffset;
  /* loop count, frequency scale shift, then code length */
  int countOrShiftOrCodeLength;
  PckDecodedByteCount bytesRemaining;
  /* second-lowest weight, then code bits, dword count, run length and the code table entry */
  uint32_t secondWeightOrCode;
  PckHuffmanNode *lowestWeightNode;
  uint8_t *sourceByteCursor;
  PckHuffmanSymbolState *symbolState;
  PckHuffmanNode *scanNode;
  uint8_t *frequencyByteCursor;
  uint32_t *workspaceClearCursor;
  PckHuffmanNode *leafOrSecondLowestNode;
  PckHuffmanNodePtr currentLeafNode;
  uint32_t *outputClearCursor;
  uint32_t *outputWriteCursor;
  PckHuffmanNodePtr nextInternalNode;
  uint8_t currentSymbolByte;

  workspaceClearCursor = (uint32_t *)g_PckHuffmanSymbolWorkspace256;
  for (countOrShiftOrCodeLength = PCK_HUFFMAN_WORKSPACE_DWORDS;
       bytesRemaining = sourceSizeBytes, sourceByteCursor = source, countOrShiftOrCodeLength != 0;
       countOrShiftOrCodeLength--) {
    *workspaceClearCursor = 0;
    workspaceClearCursor++;
  }
  do {
    g_PckHuffmanSymbolWorkspace256[*sourceByteCursor].frequencyCount =
         g_PckHuffmanSymbolWorkspace256[*sourceByteCursor].frequencyCount + 1;
    bytesRemaining--;
    sourceByteCursor++;
  } while (bytesRemaining != 0);
  /* The symbol table ends where the leaf node workspace begins. */
  maxCountOrWeightOrSize = 0;
  symbolState = g_PckHuffmanSymbolWorkspace256;
  do {
    if (maxCountOrWeightOrSize < symbolState->frequencyCount) {
      maxCountOrWeightOrSize = symbolState->frequencyCount;
    }
    symbolState++;
  } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  /* Scale the counts down until the largest fits the 8-bit table; rounding up keeps rare symbols nonzero. */
  countOrShiftOrCodeLength = 0;
  for (; 255 < maxCountOrWeightOrSize; maxCountOrWeightOrSize = (maxCountOrWeightOrSize + 1) >> 1) {
    countOrShiftOrCodeLength++;
  }
  if (countOrShiftOrCodeLength != 0) {
    symbolState = g_PckHuffmanSymbolWorkspace256;
    do {
      symbolState->frequencyCount = symbolState->frequencyCount + (1 << ((uint8_t)countOrShiftOrCodeLength & 31)) - 1;
      symbolState->frequencyCount = symbolState->frequencyCount >> ((uint8_t)countOrShiftOrCodeLength & 31);
      symbolState++;
    } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  }
  symbolState = g_PckHuffmanSymbolWorkspace256;
  leafOrSecondLowestNode = g_PckHuffmanLeafNodeWorkspace256;
  do {
    leafOrSecondLowestNode->weight = symbolState->frequencyCount;
    symbolState++;
    leafOrSecondLowestNode++;
  } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  /* Join the two lightest live nodes under a new internal node until only the root still has a weight;
     a joined node's weight is cleared, so the root is the only node left with nonzero weight. */
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  for (;;) {
    scanNode = g_PckHuffmanLeafNodeWorkspace256;
    maxCountOrWeightOrSize = UINT32_MAX;
    countOrShiftOrCodeLength = PCK_HUFFMAN_NODE_COUNT;
    secondWeightOrCode = UINT32_MAX;
    do {
      if (scanNode->weight != 0) {
        if (scanNode->weight < maxCountOrWeightOrSize) {
          if (maxCountOrWeightOrSize < secondWeightOrCode) {
            secondWeightOrCode = maxCountOrWeightOrSize;
            leafOrSecondLowestNode = lowestWeightNode;
          }
          maxCountOrWeightOrSize = scanNode->weight;
          lowestWeightNode = scanNode;
        }
        else if (scanNode->weight < secondWeightOrCode) {
          secondWeightOrCode = scanNode->weight;
          leafOrSecondLowestNode = scanNode;
        }
      }
      scanNode++;
      countOrShiftOrCodeLength--;
    } while (countOrShiftOrCodeLength != 0);
    /* Fewer than two weighted nodes left: the tree is complete. */
    if ((int)secondWeightOrCode < 0) break;
    nextInternalNode->weight = maxCountOrWeightOrSize + secondWeightOrCode;
    nextInternalNode->zeroChild = lowestWeightNode;
    nextInternalNode->oneChild = leafOrSecondLowestNode;
    lowestWeightNode->parent = nextInternalNode;
    leafOrSecondLowestNode->parent = nextInternalNode;
    lowestWeightNode->weight = 0;
    leafOrSecondLowestNode->weight = 0;
    nextInternalNode++;
    /* Workspace exhausted: all 256 internal nodes used (original: CMP next,end; JC continue). */
    if (g_PckHuffmanInternalNodeWorkspace256 + PCK_HUFFMAN_SYMBOL_COUNT <= nextInternalNode) goto fail;
  }
  if (destinationCapacityBytes < PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) goto fail;
  /* Frequency table: the low byte of each scaled count. */
  countOrShiftOrCodeLength = PCK_HUFFMAN_FREQUENCY_TABLE_BYTES;
  outputWriteCursor = (uint32_t *)(destination + PCK_HUFFMAN_FREQUENCY_TABLE_BYTES);
  frequencyByteCursor = (uint8_t *)g_PckHuffmanSymbolWorkspace256;
  do {
    *destination = *frequencyByteCursor;
    frequencyByteCursor = frequencyByteCursor + 4;
    destination++;
    countOrShiftOrCodeLength--;
  } while (countOrShiftOrCodeLength != 0);
  /* Code table: walking from each used leaf up to the root collects the code with the root's bit lowest
     (the order the decoder reads it); each entry becomes code bits 0..23 | code length << 24. */
  symbolState = g_PckHuffmanSymbolWorkspace256;
  maxCountOrWeightOrSize = 0;
  do {
    if (symbolState->frequencyCount != 0) {
      countOrShiftOrCodeLength = 0;
      secondWeightOrCode = 0;
      currentLeafNode = g_PckHuffmanLeafNodeWorkspace256 + maxCountOrWeightOrSize;
      do {
        ancestorNode = currentLeafNode->parent;
        secondWeightOrCode = secondWeightOrCode * 2;
        countOrShiftOrCodeLength++;
        if (currentLeafNode == ancestorNode->oneChild) {
          secondWeightOrCode++;
        }
        currentLeafNode = ancestorNode;
      } while (ancestorNode->weight == 0);
      symbolState->frequencyCount = secondWeightOrCode | countOrShiftOrCodeLength * (1 << PCK_HUFFMAN_CODE_LENGTH_SHIFT);
    }
    maxCountOrWeightOrSize++;
    symbolState++;
  } while (maxCountOrWeightOrSize < PCK_HUFFMAN_SYMBOL_COUNT);
  /* Zero the dword-aligned output area, since the tokens are ORed into it. */
  maxCountOrWeightOrSize = (destinationCapacityBytes - PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) & PACKAGE_DWORD_ALIGN_MASK;
  if (maxCountOrWeightOrSize != 0) {
    outputClearCursor = outputWriteCursor;
    for (secondWeightOrCode = (destinationCapacityBytes - PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) >> 2; secondWeightOrCode != 0; secondWeightOrCode--) {
      *outputClearCursor = 0;
      outputClearCursor++;
    }
    /* From here destinationCapacityBytes counts the output bytes still free. */
    destinationCapacityBytes = maxCountOrWeightOrSize - 4;
    /* SUB [capacity],4 / JBE fail (0x0040A69F): exactly 4 free bytes also fail */
    if (4 < maxCountOrWeightOrSize) {
      outputBitOffset = 0;
      /* table + 0x1F, so the final AND with ~0xF rounds up and adds at least 16 bytes of slack */
      maxCountOrWeightOrSize = PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 31;
      do {
        for (;;) {
          currentSymbolByte = *source;
          if (sourceSizeBytes < PCK_HUFFMAN_MIN_RUN_LENGTH || currentSymbolByte != source[1] ||
              currentSymbolByte != source[2]) break;
          /* Run token: count up to 18 equal bytes, emit flag 1 + (count - 3) in 4 bits = count*2 - 5,
             then the byte's code. */
          secondWeightOrCode = 0;
          do {
            if (currentSymbolByte != *source || 17 < secondWeightOrCode) break;
            secondWeightOrCode++;
            source++;
            sourceSizeBytes--;
          } while (sourceSizeBytes != 0);
          *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode * 2 - 5) << (outputBitOffset & 31);
          secondWeightOrCode = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
          *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode & 0xffffff) << (outputBitOffset + 5 & 31);
          for (outputBitOffset = outputBitOffset + 5 + (char)(secondWeightOrCode >> PCK_HUFFMAN_CODE_LENGTH_SHIFT); 7 < outputBitOffset;
              outputBitOffset = outputBitOffset - 8) {
            /* advance the dword write window by one byte */
            outputWriteCursor = (uint32_t *)((uint8_t *)outputWriteCursor + 1);
            maxCountOrWeightOrSize++;
            destinationCapacityBytes--;
            if (destinationCapacityBytes == 0) goto fail;
          }
          if (sourceSizeBytes == 0) goto finish;
        }
        /* Literal token: flag bit 0, then the byte's code. */
        secondWeightOrCode = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
        *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode & 0xffffff) << (outputBitOffset + 1 & 31);
        for (outputBitOffset = outputBitOffset + 1 + (char)(secondWeightOrCode >> PCK_HUFFMAN_CODE_LENGTH_SHIFT); 7 < outputBitOffset;
            outputBitOffset = outputBitOffset - 8) {
          outputWriteCursor = (uint32_t *)((uint8_t *)outputWriteCursor + 1);
          maxCountOrWeightOrSize++;
          destinationCapacityBytes--;
          if (destinationCapacityBytes == 0) goto fail;
        }
        source++;
        sourceSizeBytes--;
      } while (sourceSizeBytes != 0);
finish:
      if (outputBitOffset != 0) {
        maxCountOrWeightOrSize++;
      }
      return PckCodec_Succeed(outByteCount,maxCountOrWeightOrSize & 0xfffffff0);
    }
  }
fail:
  return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
}


/* Address: 0x0040A790.
   PCK compression method 0 reader. Rebuilds the encoder's Huffman tree from the 256-byte frequency table at
   the start of source, then decodes literal and run tokens (format in codec.h) until outputSizeBytes bytes are
   written. Returns true on success; false with FATAL_ERROR_GENERAL_FAILURE in *outErrorCode when the tree
   overflows the workspace. sourceSizeBytes is not checked: the bitstream is trusted.
   Original quirk: the byte count reported on success is the leftover run counter (EAX), not a size.
*/
bool PckCodec_DecodeHuffmanRle
          (PckDecodedByteCount outputSizeBytes,uint8_t *destination,PckStoredByteCount sourceSizeBytes,
          uint8_t *source,
          uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckHuffmanSymbolState symbolState;
  /* lowest weight while building the tree, then the input bits at the current offset */
  uint32_t lowWeightOrBitWindow;
  /* literal code bits, then the run length; also the byte count reported on success */
  PckHuffmanRunLength codeBitsOrRunLength;
  PckHuffmanBitOffset nextBitOffset;
  PckHuffmanBitOffset inputBitOffset;
  int remainingCount;
  /* second-lowest weight while building the tree, then the code bits of a run's byte */
  uint32_t secondWeightOrCodeBits;
  PckHuffmanNode *lowestWeightNode;
  PckHuffmanNodePtr literalNode;
  PckHuffmanNodePtr runSymbolNode;
  uint8_t *frequencyByteCursor;
  PckHuffmanNode *scanNode;
  uint32_t *inputCursor;
  PckHuffmanSymbolState *symbolStateCursor;
  PckHuffmanNode *leafOrSecondLowestNode;
  PckHuffmanNodePtr nextInternalNode;

  /* The symbol table ends where the leaf node workspace begins. */
  symbolStateCursor = g_PckHuffmanSymbolWorkspace256;
  frequencyByteCursor = source;
  do {
    symbolState.frequencyCount = *frequencyByteCursor; /* MOVZX of the stored 8-bit frequency */
    *symbolStateCursor = symbolState;
    frequencyByteCursor++;
    symbolStateCursor++;
  } while (symbolStateCursor < g_PckHuffmanLeafNodeWorkspace256);
  /* 0x800 dwords: the leaf and internal node workspaces behind the symbol table */
  for (remainingCount = 2048; remainingCount != 0; remainingCount--) {
    symbolStateCursor->frequencyCount = 0;
    symbolStateCursor++;
  }
  symbolStateCursor = g_PckHuffmanSymbolWorkspace256;
  leafOrSecondLowestNode = g_PckHuffmanLeafNodeWorkspace256;
  do {
    leafOrSecondLowestNode->weight = symbolStateCursor->frequencyCount;
    symbolStateCursor++;
    leafOrSecondLowestNode++;
  } while (symbolStateCursor < g_PckHuffmanLeafNodeWorkspace256);
  /* Same tree construction as PckCodec_EncodeHuffmanRle, so both sides get identical codes. */
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  for (;;) {
    scanNode = g_PckHuffmanLeafNodeWorkspace256;
    lowWeightOrBitWindow = UINT32_MAX;
    remainingCount = PCK_HUFFMAN_NODE_COUNT;
    secondWeightOrCodeBits = UINT32_MAX;
    do {
      if (scanNode->weight != 0) {
        if (scanNode->weight < lowWeightOrBitWindow) {
          if (lowWeightOrBitWindow < secondWeightOrCodeBits) {
            secondWeightOrCodeBits = lowWeightOrBitWindow;
            leafOrSecondLowestNode = lowestWeightNode;
          }
          lowWeightOrBitWindow = scanNode->weight;
          lowestWeightNode = scanNode;
        }
        else if (scanNode->weight < secondWeightOrCodeBits) {
          secondWeightOrCodeBits = scanNode->weight;
          leafOrSecondLowestNode = scanNode;
        }
      }
      scanNode++;
      remainingCount--;
    } while (remainingCount != 0);
    /* Fewer than two weighted nodes left: the tree is complete. */
    if ((int)secondWeightOrCodeBits < 0) break;
    nextInternalNode->weight = lowWeightOrBitWindow + secondWeightOrCodeBits;
    nextInternalNode->zeroChild = lowestWeightNode;
    nextInternalNode->oneChild = leafOrSecondLowestNode;
    lowestWeightNode->parent = nextInternalNode;
    leafOrSecondLowestNode->parent = nextInternalNode;
    lowestWeightNode->weight = 0;
    leafOrSecondLowestNode->weight = 0;
    nextInternalNode++;
    /* The original compares with the next function (PckCodec_EncodeHuffmanRle), whose code starts
       where the internal node workspace ends. */
    if (g_PckHuffmanInternalNodeWorkspace256 + PCK_HUFFMAN_SYMBOL_COUNT <= nextInternalNode) {
      return PckCodec_Fail(outErrorCode,FATAL_ERROR_GENERAL_FAILURE);
    }
  }
  /* The root is the last internal node created (nextInternalNode - 1); leaves have no zeroChild. The input
     is read as a dword window that advances byte by byte. */
  inputBitOffset = 0;
  inputCursor = (uint32_t *)(source + PCK_HUFFMAN_FREQUENCY_TABLE_BYTES);
  do {
    lowWeightOrBitWindow = *inputCursor >> (inputBitOffset & 31);
    nextBitOffset = inputBitOffset + 1;
    if ((lowWeightOrBitWindow & 1) == 0) {
      /* literal token */
      codeBitsOrRunLength = lowWeightOrBitWindow >> 1;
      literalNode = nextInternalNode - 1;
      do {
        if ((codeBitsOrRunLength & 1) == 0) {
          literalNode = literalNode->zeroChild;
        }
        else {
          literalNode = literalNode->oneChild;
        }
        codeBitsOrRunLength = codeBitsOrRunLength >> 1;
        nextBitOffset++;
      } while (literalNode->zeroChild != NULL);
      *destination = (uint8_t)(literalNode - g_PckHuffmanLeafNodeWorkspace256) /* symbol = leaf index */;
      destination++;
      for (inputBitOffset = nextBitOffset; 7 < inputBitOffset; inputBitOffset = inputBitOffset - 8)
      {
        inputCursor = (uint32_t *)((uint8_t *)inputCursor + 1);
      }
      outputSizeBytes--;
      continue;
    }
    /* run token: flag bit, 4-bit (length - 3), then the code of the repeated byte */
    secondWeightOrCodeBits = lowWeightOrBitWindow >> 5;
    inputBitOffset = inputBitOffset + 5;
    runSymbolNode = nextInternalNode - 1;
    do {
      if ((secondWeightOrCodeBits & 1) == 0) {
        runSymbolNode = runSymbolNode->zeroChild;
      }
      else {
        runSymbolNode = runSymbolNode->oneChild;
      }
      secondWeightOrCodeBits = secondWeightOrCodeBits >> 1;
      inputBitOffset++;
    } while (runSymbolNode->zeroChild != NULL);
    for (; 7 < inputBitOffset; inputBitOffset = inputBitOffset - 8) {
      inputCursor = (uint32_t *)((uint8_t *)inputCursor + 1);
    }
    codeBitsOrRunLength = (lowWeightOrBitWindow >> 1 & 0xf) + PCK_HUFFMAN_MIN_RUN_LENGTH;
    do {
      *destination = (uint8_t)(runSymbolNode - g_PckHuffmanLeafNodeWorkspace256) /* symbol = leaf index */;
      destination++;
      outputSizeBytes--;
      if (outputSizeBytes == 0) break;
      codeBitsOrRunLength--;
    } while (codeBitsOrRunLength != 0);
  } while (outputSizeBytes != 0);
  return PckCodec_Succeed(outByteCount,codeBitsOrRunLength);
}

