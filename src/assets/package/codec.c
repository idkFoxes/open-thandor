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
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid)

{
  AssetMagic pendingCellDword;
  AssetMagic *encodedSizeOrError;
  AssetMagic *compactFieldImageBase;
  PckHeaderDwordCount headerDwordCount;
  uint cellCount;
  dword bytes;
  PckCompactFieldImageByteCount compactImageSizeBytes;
  AssetMagic *compactWriteCursor;
  ArenaAllocEaxCf5 allocResult;
  PckCodecEaxCf5 encodeResult;
  PckCodecEaxCf5 successResult;
  AssetMagic persistedCellDword;
  
  cellCount = sourceGrid->gridWidth * sourceGrid->gridHeight;
  bytes = cellCount * 0x10 + 0x200;
  allocResult = (*g_MemoryApi.alloc)(bytes);
  compactFieldImageBase = (AssetMagic *)allocResult.eax;
  if (!allocResult.carry) {
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
    *(dword *)destination = bytes;
    encodeResult = PckCodec_EncodeHuffmanRle
                      (destinationCapacityBytes - 0x10,destination + 0x10,bytes,
                       (byte *)compactFieldImageBase);
    encodedSizeOrError = (AssetMagic *)encodeResult.eax;
    if (!encodeResult.carry) {
      (*g_MemoryApi.free)(compactFieldImageBase);
      successResult.eax = (dword)encodedSizeOrError + 0x10; /* packed size plus the 0x10-byte prefix */
      successResult.carry = false;
      return successResult;
    }
    (*g_MemoryApi.free)(compactFieldImageBase);
    compactFieldImageBase = encodedSizeOrError;
  }
  encodeResult.carry = true;
  encodeResult.eax = (dword)compactFieldImageBase;
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
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeFieldGrid
          (PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid,
          PckStoredByteCount sourceSizeBytes,byte *source)

{
  dword bytes;
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
  ArenaAllocEaxCf5 allocResult;
  PckCodecEaxCf5 decodeResult;
  ArenaFreeEaxCf5 freeResult;
  FieldGridDimension gridWidth;
  
  bytes = *(dword *)source;
  allocResult = (*g_MemoryApi.alloc)(bytes);
  compactFieldImageBase = (AssetMagic *)allocResult.eax;
  if (!allocResult.carry) {
    decodeResult = PckCodec_DecodeHuffmanRle
                      (bytes,(byte *)compactFieldImageBase,sourceSizeBytes - 0x10,source + 0x10);
    if (!decodeResult.carry) {
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
      freeResult = (*g_MemoryApi.free)(compactFieldImageBase);
      /* EAX is whatever the free left in it (callers only test CF), CF clear. */
      decodeResult.carry = false;
      decodeResult.eax = (dword)freeResult.eax;
      return decodeResult;
    }
    freeResult = (*g_MemoryApi.free)(compactFieldImageBase);
    compactFieldImageBase = (AssetMagic *)freeResult.eax;
  }
  decodeResult.carry = true;
  decodeResult.eax = (dword)compactFieldImageBase;
  return decodeResult;
}


/* Address: 0x0040A960.
   Ownership: assets/package/codec.
   Purpose: Copies sourceSize bytes when destinationCapacity is large enough and returns the four-byte-aligned
   size. This is PCK compression method 1. Method 1 writer: plain dword-tail-safe copy.
*/
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckDecodedByteCount sourceSizeBytes,byte *source)

{
  PckDwordCopyCount dwordCopyCount;
  PckCodecEaxCf5 successResult;
  PckCodecEaxCf5 errorResult;
  
  if (sourceSizeBytes <= destinationCapacityBytes) {
    for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0;
        dwordCopyCount = dwordCopyCount - 1) {
      *(dword *)destination = *(dword *)source;
      source = source + 4;
      destination = destination + 4;
    }
    successResult.eax = sourceSizeBytes + 3 & 0xfffffffc;
    successResult.carry = false;
    return successResult;
  }
  errorResult.carry = true;
  errorResult.eax = 0x14;
  return errorResult;
}


/* Address: 0x0040A9A0.
   Ownership: assets/package/codec.
   Purpose: Copies sourceSize bytes directly to the destination. This is PCK compression method 1. Method 1 reader:
   stored copy.
*/
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeStored
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckStoredByteCount sourceSizeBytes,byte *source)

{
  PckDwordCopyCount dwordCopyCount;
  PckCodecEaxCf5 copyResult;
  
  for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0;
      dwordCopyCount = dwordCopyCount - 1) {
    *(dword *)destination = *(dword *)source;
    source = source + 4;
    destination = destination + 4;
  }
  copyResult.carry = (sourceSizeBytes >> 1 & 1) != 0;
  /* EAX is untouched; in Package_DecodeEntryInto it still holds the read size (packedSize). */
  copyResult.eax = sourceSizeBytes;
  return copyResult;
}


/* Address: 0x0040A4C0.
   Ownership: assets/package/codec.
   Purpose: Builds the package Huffman tree, emits the 256-byte frequency table, and encodes literals plus
   3-to-18-byte repeated runs. Returns the aligned packed size with CF clear; returns error 0x14 with CF set on
   failure. PCK compressionMethod 0 writer: order-0 Huffman over literal/RLE symbols.
*/
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_EncodeHuffmanRle
          (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
          PckDecodedByteCount sourceSizeBytes,byte *source)

{
  PckHuffmanNodePtr ancestorNode;
  uint weightIndexOrSize;
  PckHuffmanBitOffset outputBitOffset;
  int countOrCodeLength;
  PckDecodedByteCount bytesRemaining;
  uint secondWeightOrCode;
  PckHuffmanNode *lowestWeightNode;
  byte *sourceByteCursor;
  PckHuffmanSymbolState *symbolState;
  PckHuffmanNode *scanNode;
  byte *frequencyByteCursor;
  dword *workspaceClearCursor;
  PckHuffmanNode *leafOrSecondLowestNode;
  PckHuffmanNodePtr currentLeafNode;
  uint *outputClearCursor;
  uint *outputWriteCursor;
  PckCodecEaxCf5 successResult;
  PckCodecEaxCf5 errorResult;
  PckHuffmanNodePtr nextInternalNode;
  byte currentSymbolByte;
  PckHuffmanNodePtr parentNode;
  
  workspaceClearCursor = (dword *)g_PckHuffmanSymbolWorkspace256;
  for (countOrCodeLength = 0x900; bytesRemaining = sourceSizeBytes, sourceByteCursor = source, countOrCodeLength != 0; countOrCodeLength = countOrCodeLength + -1) {
    *workspaceClearCursor = 0;
    workspaceClearCursor = workspaceClearCursor + 1;
  }
  do {
    g_PckHuffmanSymbolWorkspace256[*sourceByteCursor].frequencyCount =
         g_PckHuffmanSymbolWorkspace256[*sourceByteCursor].frequencyCount + 1;
    bytesRemaining = bytesRemaining - 1;
    sourceByteCursor = sourceByteCursor + 1;
  } while (bytesRemaining != 0);
  weightIndexOrSize = 0;
  symbolState = g_PckHuffmanSymbolWorkspace256;
  do {
    if (weightIndexOrSize < symbolState->frequencyCount) {
      weightIndexOrSize = symbolState->frequencyCount;
    }
    symbolState = symbolState + 1;
  } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  countOrCodeLength = 0;
  for (; 0xff < weightIndexOrSize; weightIndexOrSize = weightIndexOrSize + 1 >> 1) {
    countOrCodeLength = countOrCodeLength + 1;
  }
  if (countOrCodeLength != 0) {
    symbolState = g_PckHuffmanSymbolWorkspace256;
    do {
      symbolState->frequencyCount = symbolState->frequencyCount + (1 << ((byte)countOrCodeLength & 0x1f)) + -1;
      symbolState->frequencyCount = symbolState->frequencyCount >> ((byte)countOrCodeLength & 0x1f);
      symbolState = symbolState + 1;
    } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  }
  symbolState = g_PckHuffmanSymbolWorkspace256;
  leafOrSecondLowestNode = g_PckHuffmanLeafNodeWorkspace256;
  do {
    leafOrSecondLowestNode->weight = THANDOR_BITCAST(PckHuffmanSymbolState, PckHuffmanWeight, *symbolState);
    symbolState = symbolState + 1;
    leafOrSecondLowestNode = leafOrSecondLowestNode + 1;
  } while (symbolState < g_PckHuffmanLeafNodeWorkspace256);
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  while( true ) {
    scanNode = g_PckHuffmanLeafNodeWorkspace256;
    weightIndexOrSize = 0xffffffff;
    countOrCodeLength = 0x200;
    secondWeightOrCode = 0xffffffff;
    do {
      if (scanNode->weight != 0) {
        if (scanNode->weight < weightIndexOrSize) {
          if (weightIndexOrSize < secondWeightOrCode) {
            secondWeightOrCode = weightIndexOrSize;
            leafOrSecondLowestNode = lowestWeightNode;
          }
          weightIndexOrSize = scanNode->weight;
          lowestWeightNode = scanNode;
        }
        else if (scanNode->weight < secondWeightOrCode) {
          secondWeightOrCode = scanNode->weight;
          leafOrSecondLowestNode = scanNode;
        }
      }
      scanNode = scanNode + 1;
      countOrCodeLength = countOrCodeLength + -1;
    } while (countOrCodeLength != 0);
    if ((int)secondWeightOrCode < 0) break;
    nextInternalNode->weight = weightIndexOrSize + secondWeightOrCode;
    nextInternalNode->zeroChild = lowestWeightNode;
    nextInternalNode->oneChild = leafOrSecondLowestNode;
    lowestWeightNode->parent = nextInternalNode;
    leafOrSecondLowestNode->parent = nextInternalNode;
    lowestWeightNode->weight = 0;
    leafOrSecondLowestNode->weight = 0;
    nextInternalNode = nextInternalNode + 1;
    /* Workspace exhausted: all 256 internal nodes used (original: CMP next,end; JC continue). */
    if (g_PckHuffmanInternalNodeWorkspace256 + 0x100 <= nextInternalNode) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
  }
  if (destinationCapacityBytes < 0x100) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
  countOrCodeLength = 0x100;
  outputWriteCursor = (uint *)(destination + 0x100);
  frequencyByteCursor = (byte *)g_PckHuffmanSymbolWorkspace256;
  do {
    *destination = *frequencyByteCursor;
    frequencyByteCursor = frequencyByteCursor + 4;
    destination = destination + 1;
    countOrCodeLength = countOrCodeLength + -1;
  } while (countOrCodeLength != 0);
  symbolState = g_PckHuffmanSymbolWorkspace256;
  weightIndexOrSize = 0;
  do {
    if (symbolState->frequencyCount != 0) {
      countOrCodeLength = 0;
      secondWeightOrCode = 0;
      currentLeafNode = g_PckHuffmanLeafNodeWorkspace256 + weightIndexOrSize;
      do {
        ancestorNode = currentLeafNode->parent;
        secondWeightOrCode = secondWeightOrCode * 2;
        countOrCodeLength = countOrCodeLength + 1;
        if (currentLeafNode == ancestorNode->oneChild) {
          secondWeightOrCode = secondWeightOrCode + 1;
        }
        currentLeafNode = ancestorNode;
      } while (ancestorNode->weight == 0);
      symbolState->frequencyCount = secondWeightOrCode | countOrCodeLength * 0x1000000;
    }
    weightIndexOrSize = weightIndexOrSize + 1;
    symbolState = symbolState + 1;
  } while (weightIndexOrSize < 0x100);
  weightIndexOrSize = destinationCapacityBytes - 0x100 & 0xfffffffc;
  if (weightIndexOrSize != 0) {
    outputClearCursor = outputWriteCursor;
    for (secondWeightOrCode = destinationCapacityBytes - 0x100 >> 2; secondWeightOrCode != 0; secondWeightOrCode = secondWeightOrCode - 1) {
      *outputClearCursor = 0;
      outputClearCursor = outputClearCursor + 1;
    }
    destinationCapacityBytes = weightIndexOrSize - 4;
    if (3 < weightIndexOrSize) {
      outputBitOffset = 0;
      weightIndexOrSize = 0x11f;
      do {
        while( true ) {
          currentSymbolByte = *source;
          if (((sourceSizeBytes < 3) || (currentSymbolByte != source[1])) ||
             (currentSymbolByte != source[2])) break;
          secondWeightOrCode = 0;
          do {
            if ((currentSymbolByte != *source) || (0x11 < secondWeightOrCode)) break;
            secondWeightOrCode = secondWeightOrCode + 1;
            source = source + 1;
            sourceSizeBytes = sourceSizeBytes - 1;
          } while (sourceSizeBytes != 0);
          *outputWriteCursor = *outputWriteCursor | secondWeightOrCode * 2 + -5 << (outputBitOffset & 0x1f);
          secondWeightOrCode = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
          *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode & 0xffffff) << (outputBitOffset + 5 & 0x1f);
          for (outputBitOffset = outputBitOffset + 5 + (char)(secondWeightOrCode >> 0x18); 7 < outputBitOffset;
              outputBitOffset = outputBitOffset - 8) {
            outputWriteCursor = (uint *)((int)outputWriteCursor + 1);
            weightIndexOrSize = weightIndexOrSize + 1;
            destinationCapacityBytes = destinationCapacityBytes - 1;
            if (destinationCapacityBytes == 0) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
          }
          if (sourceSizeBytes == 0)
          goto PckCodec_EncodeHuffmanRle_FinalizeBitstreamAndReturnAlignedSizeWithCarryClear;
        }
        secondWeightOrCode = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
        *outputWriteCursor = *outputWriteCursor | (secondWeightOrCode & 0xffffff) << (outputBitOffset + 1 & 0x1f);
        for (outputBitOffset = outputBitOffset + 1 + (char)(secondWeightOrCode >> 0x18); 7 < outputBitOffset;
            outputBitOffset = outputBitOffset - 8) {
          outputWriteCursor = (uint *)((int)outputWriteCursor + 1);
          weightIndexOrSize = weightIndexOrSize + 1;
          destinationCapacityBytes = destinationCapacityBytes - 1;
          if (destinationCapacityBytes == 0) goto PckCodec_EncodeHuffmanRle_ReturnCapacityError;
        }
        source = source + 1;
        sourceSizeBytes = sourceSizeBytes - 1;
      } while (sourceSizeBytes != 0);
PckCodec_EncodeHuffmanRle_FinalizeBitstreamAndReturnAlignedSizeWithCarryClear:
      if (outputBitOffset != 0) {
        weightIndexOrSize = weightIndexOrSize + 1;
      }
      successResult.eax = weightIndexOrSize & 0xfffffff0;
      successResult.carry = false;
      return successResult;
    }
  }
PckCodec_EncodeHuffmanRle_ReturnCapacityError:
  errorResult.carry = true;
  errorResult.eax = 0x14;
  return errorResult;
}


/* Address: 0x0040A790.
   Ownership: assets/package/codec.
   Purpose: Rebuilds the package Huffman tree from the first 256 source bytes and decodes literal or repeated-run
   tokens until outputSize bytes have been produced. CF reports success or failure. PCK compressionMethod 0 reader.
*/
PckCodecEaxCf5 __thandor_eax_cf_preserve_ecx_edx
PckCodec_DecodeHuffmanRle
          (PckDecodedByteCount outputSizeBytes,byte *destination,PckStoredByteCount sourceSizeBytes,
          byte *source)

{
  PckHuffmanSymbolState symbolState;
  uint lowWeightOrBitWindow;
  PckHuffmanRunLength runLength;
  PckHuffmanBitOffset nextBitOffset;
  PckHuffmanBitOffset inputBitOffset;
  int remainingCount;
  uint secondWeightOrCodeBits;
  PckHuffmanNode *lowestWeightNode;
  PckHuffmanNodePtr literalNode;
  PckHuffmanNodePtr currentHuffmanNode;
  byte *frequencyByteCursor;
  PckHuffmanNode *scanNode;
  uint *inputCursor;
  PckHuffmanSymbolState *symbolStateCursor;
  PckHuffmanNode *leafOrSecondLowestNode;
  PckCodecEaxCf5 huffmanResult;
  PckHuffmanNodePtr nextInternalNode;
  
  symbolStateCursor = g_PckHuffmanSymbolWorkspace256;
  frequencyByteCursor = source;
  do {
    symbolState.frequencyCount = *frequencyByteCursor; /* MOVZX of the stored 8-bit frequency */
    *symbolStateCursor = symbolState;
    frequencyByteCursor = frequencyByteCursor + 1;
    symbolStateCursor = symbolStateCursor + 1;
  } while (symbolStateCursor < g_PckHuffmanLeafNodeWorkspace256);
  for (remainingCount = 0x800; remainingCount != 0; remainingCount = remainingCount + -1) {
    symbolStateCursor->frequencyCount = 0;
    symbolStateCursor = symbolStateCursor + 1;
  }
  symbolStateCursor = g_PckHuffmanSymbolWorkspace256;
  leafOrSecondLowestNode = g_PckHuffmanLeafNodeWorkspace256;
  do {
    leafOrSecondLowestNode->weight = THANDOR_BITCAST(PckHuffmanSymbolState, PckHuffmanWeight, *symbolStateCursor);
    symbolStateCursor = symbolStateCursor + 1;
    leafOrSecondLowestNode = leafOrSecondLowestNode + 1;
  } while (symbolStateCursor < g_PckHuffmanLeafNodeWorkspace256);
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  for (;;) {
    scanNode = g_PckHuffmanLeafNodeWorkspace256;
    lowWeightOrBitWindow = 0xffffffff;
    remainingCount = 0x200;
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
      scanNode = scanNode + 1;
      remainingCount = remainingCount + -1;
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
    nextInternalNode = nextInternalNode + 1;
    /* The original compares with the next function (PckCodec_EncodeHuffmanRle), whose code starts
       where the internal node workspace ends. */
    if (g_PckHuffmanInternalNodeWorkspace256 + 256 <= nextInternalNode) {
      huffmanResult.carry = true;
      huffmanResult.eax = 0x14;
      return huffmanResult;
    }
  }
  inputBitOffset = 0;
  inputCursor = (uint *)(source + 0x100);
  do {
    lowWeightOrBitWindow = *inputCursor >> (inputBitOffset & 0x1f);
    nextBitOffset = inputBitOffset + 1;
    if ((lowWeightOrBitWindow & 1) == 0) {
      runLength = lowWeightOrBitWindow >> 1;
      literalNode = nextInternalNode + -1;
      do {
        if ((runLength & 1) == 0) {
          literalNode = literalNode->zeroChild;
        }
        else {
          literalNode = literalNode->oneChild;
        }
        runLength = runLength >> 1;
        nextBitOffset = nextBitOffset + 1;
      } while (literalNode->zeroChild != (PckHuffmanNodePtr)0x0);
      *destination = (byte)(literalNode - g_PckHuffmanLeafNodeWorkspace256) /* symbol = leaf index */;
      destination = destination + 1;
      for (inputBitOffset = nextBitOffset; 7 < inputBitOffset; inputBitOffset = inputBitOffset - 8)
      {
        inputCursor = (uint *)((int)inputCursor + 1);
      }
      outputSizeBytes = outputSizeBytes - 1;
      continue;
    }
    secondWeightOrCodeBits = lowWeightOrBitWindow >> 5;
    inputBitOffset = inputBitOffset + 5;
    currentHuffmanNode = nextInternalNode + -1;
    do {
      if ((secondWeightOrCodeBits & 1) == 0) {
        currentHuffmanNode = currentHuffmanNode->zeroChild;
      }
      else {
        currentHuffmanNode = currentHuffmanNode->oneChild;
      }
      secondWeightOrCodeBits = secondWeightOrCodeBits >> 1;
      inputBitOffset = inputBitOffset + 1;
    } while (currentHuffmanNode->zeroChild != (PckHuffmanNodePtr)0x0);
    for (; 7 < inputBitOffset; inputBitOffset = inputBitOffset - 8) {
      inputCursor = (uint *)((int)inputCursor + 1);
    }
    runLength = (lowWeightOrBitWindow >> 1 & 0xf) + 3;
    do {
      *destination = (byte)(currentHuffmanNode - g_PckHuffmanLeafNodeWorkspace256) /* symbol = leaf index */;
      destination = destination + 1;
      outputSizeBytes = outputSizeBytes - 1;
      if (outputSizeBytes == 0) break;
      runLength = runLength - 1;
    } while (runLength != 0);
  } while (outputSizeBytes != 0);
  huffmanResult.carry = false;
  huffmanResult.eax = runLength;
  return huffmanResult;
}

