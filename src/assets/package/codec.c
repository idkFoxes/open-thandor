/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/codec.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/package/codec.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/package/codec. */

/* Address: 0x0040A9C0.
   Ownership: assets/package/codec.
   Purpose: PCK method-2 encoder. Copies the 0x200-byte FieldGridAsset header, compacts each 0x80-byte
   FieldGridCell to four persisted dwords, then invokes method 0. Method 2 writer: packs each 0x80 runtime cell
   back to the 0x10-byte on-disk record (+0x54,+0x48,+0x4C,+0x50) then Huffman.
   Local calls: PckCodec_EncodeHuffmanRle.
*/
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid)

{
  AssetMagic pendingCellDword;
  AssetMagic *encodedSizeOrError;
  AssetMagic *compactFieldImageBase;
  PckHeaderDwordCount headerDwordCount;
  uint32_t cellCount;
  uint32_t bytes;
  PckCompactFieldImageByteCount compactImageSizeBytes;
  AssetMagic *compactWriteCursor;
  ArenaAllocResult allocResult;
  PckCodecResult encodeResult;
  PckCodecResult successResult;
  AssetMagic persistedCellDword;
  
  cellCount = sourceGrid->gridWidth * sourceGrid->gridHeight;
  bytes = cellCount * 0x10 + 0x200;
  allocResult = g_MemoryApi.alloc(bytes);
  compactFieldImageBase = (AssetMagic *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    compactWriteCursor = compactFieldImageBase;
    for (headerDwordCount = 0x80; headerDwordCount != 0; headerDwordCount = headerDwordCount - 1) {
      *compactWriteCursor = (sourceGrid->common).magic;
      sourceGrid = (FieldGridAsset *)&(sourceGrid->common).allocationSizeBytes;
      compactWriteCursor = compactWriteCursor + 1;
    }
    cellCount = cellCount & 0xfffffff;
    do {
      persistedCellDword =
           *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0xc);
      *compactWriteCursor =
           *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0x12);
      compactWriteCursor[1] = persistedCellDword;
      pendingCellDword = *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0x10);
      compactWriteCursor[2] =
           *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0xe);
      compactWriteCursor[3] = pendingCellDword;
      sourceGrid = (FieldGridAsset *)((sourceGrid->common).buildMetadata.names.sourceName + 8);
      compactWriteCursor = compactWriteCursor + 4;
      cellCount = cellCount - 1;
    } while (cellCount != 0);
    *(uint32_t *)destination = bytes;
    encodeResult = PckCodec_EncodeHuffmanRle
                      (destinationCapacityBytes - 0x10,destination + 0x10,bytes,
                       (uint8_t *)compactFieldImageBase);
    encodedSizeOrError = (AssetMagic *)encodeResult.byteCountOrError;
    if (!encodeResult.failed) {
      g_MemoryApi.free(compactFieldImageBase);
      successResult.byteCountOrError = (uint32_t)encodedSizeOrError + 0x10; /* packed size plus the 0x10-byte prefix */
      successResult.failed = false;
      return successResult;
    }
    g_MemoryApi.free(compactFieldImageBase);
    compactFieldImageBase = encodedSizeOrError;
  }
  encodeResult.failed = true;
  encodeResult.byteCountOrError = (uint32_t)compactFieldImageBase;
  return encodeResult;
}


/* Address: 0x0040AAA0.
   Ownership: assets/package/codec.
   Purpose: PCK method-2 decoder. Restores the FieldGridAsset header, expands compact 0x10-byte records into zeroed
   0x80-byte FieldGridCell records, and generates worldX/worldY for every cell. 0x10-byte packed cell -> 0x80
   FieldGridCell: dword0 -> +0x54, dword1 -> +0x48 (terrainHeight), dword2 -> +0x4C (waterSurfaceDelta), dword3 ->
   +0x50 (flagsAndMaterial, packs FLD layer bytes 8-11 LE — R1). Generated world coords (32-bit wrap): worldX =
   col*0x901 + row*0x480, worldY = -1999*row (triangle lattice 2305/1152/1999; inverse of the T4 sampler).
   Local calls: PckCodec_DecodeHuffmanRle.
*/
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid,
          PckStoredByteCount sourceSizeBytes,uint8_t *source)

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
  FieldGridAsset *expandedZeroCursor;
  FieldGridAsset *expandedWriteCursor;
  ArenaAllocResult allocResult;
  PckCodecResult decodeResult;
  ArenaFreeResult freeResult;
  FieldGridDimension gridWidth;
  
  bytes = *(uint32_t *)source;
  allocResult = g_MemoryApi.alloc(bytes);
  compactFieldImageBase = (AssetMagic *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    decodeResult = PckCodec_DecodeHuffmanRle
                      (bytes,(uint8_t *)compactFieldImageBase,sourceSizeBytes - 0x10,source + 0x10);
    if (!decodeResult.failed) {
      cellCountOrWorldX = compactFieldImageBase[0x2e] * compactFieldImageBase[0x2f];
      compactReadCursor = compactFieldImageBase;
      expandedWriteCursor = destinationGrid;
      for (countOrRowStartX = 0x80; countOrRowStartX != 0; countOrRowStartX = countOrRowStartX + -1) {
        (expandedWriteCursor->common).magic = *compactReadCursor;
        compactReadCursor = compactReadCursor + 1;
        expandedWriteCursor = (FieldGridAsset *)&(expandedWriteCursor->common).allocationSizeBytes;
      }
      expandedZeroCursor = expandedWriteCursor;
      for (countOrRowStartX = cellCountOrWorldX * 0x20; countOrRowStartX != 0; countOrRowStartX = countOrRowStartX + -1) {
        (expandedZeroCursor->common).magic = 0;
        expandedZeroCursor = (FieldGridAsset *)&(expandedZeroCursor->common).allocationSizeBytes;
      }
      do {
        pendingCellDword = compactReadCursor[1];
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0x12) =
             *compactReadCursor;
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0xc) =
             pendingCellDword;
        pendingCellDword = compactReadCursor[3];
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0xe) =
             compactReadCursor[2];
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0x10) =
             pendingCellDword;
        compactReadCursor = compactReadCursor + 4;
        expandedWriteCursor =
             (FieldGridAsset *)((expandedWriteCursor->common).buildMetadata.names.sourceName + 8);
        cellCountOrWorldX = cellCountOrWorldX + -1;
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
          cellCountOrWorldX = cellCountOrWorldX + 0x901;
          currentWorldCoordinateCell = currentWorldCoordinateCell + 1;
          columnsRemaining = columnsRemaining - 1;
        } while (columnsRemaining != 0);
        cellCountOrWorldX = countOrRowStartX + 0x480;
        currentWorldYQ12 = currentWorldYQ12 + -1999;
        rowsRemaining = rowsRemaining - 1;
        columnsRemaining = gridWidth;
        countOrRowStartX = cellCountOrWorldX;
      } while (rowsRemaining != 0);
      freeResult = g_MemoryApi.free(compactFieldImageBase);
      /* EAX is whatever the free left in it (callers only test CF), CF clear. */
      decodeResult.failed = false;
      decodeResult.byteCountOrError = (uint32_t)freeResult.valueOrError;
      return decodeResult;
    }
    freeResult = g_MemoryApi.free(compactFieldImageBase);
    compactFieldImageBase = (AssetMagic *)freeResult.valueOrError;
  }
  decodeResult.failed = true;
  decodeResult.byteCountOrError = (uint32_t)compactFieldImageBase;
  return decodeResult;
}


/* Address: 0x0040A960.
   Ownership: assets/package/codec.
   Purpose: Copies sourceSize bytes when destinationCapacity is large enough and returns the four-byte-aligned
   size. This is PCK compression method 1. Method 1 writer: plain dword-tail-safe copy.
*/
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceSizeBytes,uint8_t *source)

{
  PckDwordCopyCount dwordCopyCount;
  PckCodecResult successResult;
  PckCodecResult errorResult;
  
  if (sourceSizeBytes <= destinationCapacityBytes) {
    for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0;
        dwordCopyCount = dwordCopyCount - 1) {
      *(uint32_t *)destination = *(uint32_t *)source;
      source = source + 4;
      destination = destination + 4;
    }
    successResult.byteCountOrError = sourceSizeBytes + 3 & 0xfffffffc;
    successResult.failed = false;
    return successResult;
  }
  errorResult.failed = true;
  errorResult.byteCountOrError = 0x14;
  return errorResult;
}


/* Address: 0x0040A9A0.
   Ownership: assets/package/codec.
   Purpose: Copies sourceSize bytes directly to the destination. This is PCK compression method 1. Method 1 reader:
   stored copy.
*/
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckStoredByteCount sourceSizeBytes,uint8_t *source)

{
  PckDwordCopyCount dwordCopyCount;
  PckCodecResult copyResult;
  
  for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0;
      dwordCopyCount = dwordCopyCount - 1) {
    *(uint32_t *)destination = *(uint32_t *)source;
    source = source + 4;
    destination = destination + 4;
  }
  copyResult.failed = (sourceSizeBytes >> 1 & 1) != 0;
  /* EAX is untouched; in Package_DecodeEntryInto it still holds the read size (packedSize). */
  copyResult.byteCountOrError = sourceSizeBytes;
  return copyResult;
}


/* Address: 0x0040A4C0.
   PCK compression method 0 writer. Counts the byte frequencies of the source, scales them to 8 bits, builds a
   Huffman tree from them, writes the 256-byte frequency table and then the bitstream of literal and 3..18-byte
   run tokens (format in codec.h). Returns the packed size (rounded up to 16 bytes, with at least 16 bytes of
   slack for the decoder's dword reads) with CF clear, or FATAL_ERROR_GENERAL_FAILURE with CF set when the tree
   overflows or the output does not fit.
*/
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeHuffmanRle
          (PckOutputCapacityBytes destinationCapacityBytes,uint8_t *destination,
          PckDecodedByteCount sourceSizeBytes,uint8_t *source)

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
  PckCodecResult successResult;
  PckCodecResult errorResult;
  PckHuffmanNodePtr nextInternalNode;
  uint8_t currentSymbolByte;

  /* 0x900 dwords: symbol table (0x100) + leaf nodes (0x400) + internal nodes (0x400) */
  workspaceClearCursor = (uint32_t *)g_PckHuffmanSymbolWorkspace256;
  for (countOrShiftOrCodeLength = 0x900; bytesRemaining = sourceSizeBytes, sourceByteCursor = source, countOrShiftOrCodeLength != 0; countOrShiftOrCodeLength--) {
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
  for (; 0xff < maxCountOrWeightOrSize; maxCountOrWeightOrSize = (maxCountOrWeightOrSize + 1) >> 1) {
    countOrShiftOrCodeLength++;
  }
  if (countOrShiftOrCodeLength != 0) {
    symbolState = g_PckHuffmanSymbolWorkspace256;
    do {
      symbolState->frequencyCount = symbolState->frequencyCount + (1 << ((uint8_t)countOrShiftOrCodeLength & 0x1f)) - 1;
      symbolState->frequencyCount = symbolState->frequencyCount >> ((uint8_t)countOrShiftOrCodeLength & 0x1f);
      symbolState++;
    } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  }
  symbolState = g_PckHuffmanSymbolWorkspace256;
  leafOrSecondLowestNode = g_PckHuffmanLeafNodeWorkspace256;
  do {
    leafOrSecondLowestNode->weight = THANDOR_BITCAST(PckHuffmanSymbolState, PckHuffmanWeight, *symbolState);
    symbolState++;
    leafOrSecondLowestNode++;
  } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  /* Join the two lightest live nodes under a new internal node until only the root still has a weight;
     a joined node's weight is cleared, so the root is the only node left with nonzero weight. */
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  while( true ) {
    scanNode = g_PckHuffmanLeafNodeWorkspace256;
    maxCountOrWeightOrSize = 0xffffffff;
    countOrShiftOrCodeLength = PCK_HUFFMAN_NODE_COUNT;
    secondWeightOrCode = 0xffffffff;
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
    if (g_PckHuffmanInternalNodeWorkspace256 + PCK_HUFFMAN_SYMBOL_COUNT <= nextInternalNode) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
  }
  if (destinationCapacityBytes < PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
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
      symbolState->frequencyCount = secondWeightOrCode | countOrShiftOrCodeLength * 0x1000000;
    }
    maxCountOrWeightOrSize++;
    symbolState++;
  } while (maxCountOrWeightOrSize < PCK_HUFFMAN_SYMBOL_COUNT);
  /* Zero the dword-aligned output area, since the tokens are ORed into it. */
  maxCountOrWeightOrSize = (destinationCapacityBytes - PCK_HUFFMAN_FREQUENCY_TABLE_BYTES) & 0xfffffffc;
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
      maxCountOrWeightOrSize = PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 0x1f;
      do {
        while( true ) {
          currentSymbolByte = *source;
          if (((sourceSizeBytes < PCK_HUFFMAN_MIN_RUN_LENGTH) || (currentSymbolByte != source[1])) ||
             (currentSymbolByte != source[2])) break;
          /* Run token: count up to 18 equal bytes, emit flag 1 + (count - 3) in 4 bits = count*2 - 5,
             then the byte's code. */
          secondWeightOrCode = 0;
          do {
            if ((currentSymbolByte != *source) || (0x11 < secondWeightOrCode)) break;
            secondWeightOrCode++;
            source++;
            sourceSizeBytes--;
          } while (sourceSizeBytes != 0);
          *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode * 2 - 5) << (outputBitOffset & 0x1f);
          secondWeightOrCode = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
          *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode & 0xffffff) << (outputBitOffset + 5 & 0x1f);
          for (outputBitOffset = outputBitOffset + 5 + (char)(secondWeightOrCode >> 0x18); 7 < outputBitOffset;
              outputBitOffset = outputBitOffset - 8) {
            /* advance the dword write window by one byte */
            outputWriteCursor = (uint32_t *)((int)outputWriteCursor + 1);
            maxCountOrWeightOrSize++;
            destinationCapacityBytes--;
            if (destinationCapacityBytes == 0) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
          }
          if (sourceSizeBytes == 0)
          goto PckCodec_EncodeHuffmanRle_FinalizeBitstreamAndReturnAlignedSizeWithCarryClear;
        }
        /* Literal token: flag bit 0, then the byte's code. */
        secondWeightOrCode = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
        *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode & 0xffffff) << (outputBitOffset + 1 & 0x1f);
        for (outputBitOffset = outputBitOffset + 1 + (char)(secondWeightOrCode >> 0x18); 7 < outputBitOffset;
            outputBitOffset = outputBitOffset - 8) {
          outputWriteCursor = (uint32_t *)((int)outputWriteCursor + 1);
          maxCountOrWeightOrSize++;
          destinationCapacityBytes--;
          if (destinationCapacityBytes == 0) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
        }
        source++;
        sourceSizeBytes--;
      } while (sourceSizeBytes != 0);
PckCodec_EncodeHuffmanRle_FinalizeBitstreamAndReturnAlignedSizeWithCarryClear:
      if (outputBitOffset != 0) {
        maxCountOrWeightOrSize++;
      }
      successResult.byteCountOrError = maxCountOrWeightOrSize & 0xfffffff0;
      successResult.failed = false;
      return successResult;
    }
  }
PckCodec_EncodeHuffmanRle_ReturnCapacityError:
  errorResult.failed = true;
  errorResult.byteCountOrError = FATAL_ERROR_GENERAL_FAILURE;
  return errorResult;
}


/* Address: 0x0040A790.
   PCK compression method 0 reader. Rebuilds the encoder's Huffman tree from the 256-byte frequency table at
   the start of source, then decodes literal and run tokens (format in codec.h) until outputSizeBytes bytes are
   written. CF clear on success; FATAL_ERROR_GENERAL_FAILURE with CF set when the tree overflows the workspace.
   sourceSizeBytes is not checked: the bitstream is trusted.
*/
PckCodecResult __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeHuffmanRle
          (PckDecodedByteCount outputSizeBytes,uint8_t *destination,PckStoredByteCount sourceSizeBytes,
          uint8_t *source)

{
  PckHuffmanSymbolState symbolState;
  /* lowest weight while building the tree, then the input bits at the current offset */
  uint32_t lowWeightOrBitWindow;
  /* literal code bits, then the run length; also the EAX value of the success return */
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
  PckCodecResult huffmanResult;
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
  for (remainingCount = 0x800; remainingCount != 0; remainingCount--) {
    symbolStateCursor->frequencyCount = 0;
    symbolStateCursor++;
  }
  symbolStateCursor = g_PckHuffmanSymbolWorkspace256;
  leafOrSecondLowestNode = g_PckHuffmanLeafNodeWorkspace256;
  do {
    leafOrSecondLowestNode->weight = THANDOR_BITCAST(PckHuffmanSymbolState, PckHuffmanWeight, *symbolStateCursor);
    symbolStateCursor++;
    leafOrSecondLowestNode++;
  } while (symbolStateCursor < g_PckHuffmanLeafNodeWorkspace256);
  /* Same tree construction as PckCodec_EncodeHuffmanRle, so both sides get identical codes. */
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  for (;;) {
    scanNode = g_PckHuffmanLeafNodeWorkspace256;
    lowWeightOrBitWindow = 0xffffffff;
    remainingCount = PCK_HUFFMAN_NODE_COUNT;
    secondWeightOrCodeBits = 0xffffffff;
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
      huffmanResult.failed = true;
      huffmanResult.byteCountOrError = FATAL_ERROR_GENERAL_FAILURE;
      return huffmanResult;
    }
  }
  /* The root is the last internal node created (nextInternalNode - 1); leaves have no zeroChild. The input
     is read as a dword window that advances byte by byte. */
  inputBitOffset = 0;
  inputCursor = (uint32_t *)(source + PCK_HUFFMAN_FREQUENCY_TABLE_BYTES);
  do {
    lowWeightOrBitWindow = *inputCursor >> (inputBitOffset & 0x1f);
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
        inputCursor = (uint32_t *)((int)inputCursor + 1);
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
      inputCursor = (uint32_t *)((int)inputCursor + 1);
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
  huffmanResult.failed = false;
  huffmanResult.byteCountOrError = codeBitsOrRunLength;
  return huffmanResult;
}

