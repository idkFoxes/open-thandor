#include <thandor/assets/package/codec.h>

/* Implementation ownership: assets/package/codec. */

/* Address: 0x0040A9C0.
   Ownership: assets/package/codec.
   Purpose: PCK method-2 encoder. Copies the 0x200-byte FieldGridAsset header, compacts each 0x80-byte
   FieldGridCell to four persisted dwords, then invokes method 0. Method 2 writer: packs each 0x80 runtime cell
   back to the 0x10-byte on-disk record (+0x54,+0x48,+0x4C,+0x50) then Huffman.
   Local calls: PckCodec_EncodeHuffmanRle.
*/
dword PckCodec_EncodeFieldGrid
                (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
                PckDecodedByteCount sourceImageSizeBytes,FieldGridAsset *sourceGrid)

{
  AssetMagic AVar1;
  AssetMagic *compactFieldImageBase;
  PckHeaderDwordCount headerDwordCount;
  uint uVar2;
  PckCompactFieldImageByteCount compactImageSizeBytes;
  AssetMagic *compactWriteCursor;
  bool bVar3;
  void *pvVar4;
  AssetMagic persistedCellDword;
  
  uVar2 = sourceGrid->gridWidth * sourceGrid->gridHeight * 0x10;
  bVar3 = 0xfffffdff < uVar2;
  _pvVar4 = (*g_MemoryApi.alloc)(uVar2 + 0x200);
  compactImageSizeBytes = (PckCompactFieldImageByteCount)((ulonglong)_pvVar4 >> 0x20);
  compactFieldImageBase = SUB84(_pvVar4,0);
  if (!bVar3) {
    compactWriteCursor = compactFieldImageBase;
    for (headerDwordCount = 0x80; headerDwordCount != 0; headerDwordCount = headerDwordCount - 1) {
      *compactWriteCursor = (sourceGrid->common).magic;
      sourceGrid = (FieldGridAsset *)&(sourceGrid->common).allocationSizeBytes;
      compactWriteCursor = compactWriteCursor + 1;
    }
    uVar2 = compactImageSizeBytes - 0x200 >> 4;
    do {
      persistedCellDword =
           *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0xc);
      *compactWriteCursor =
           *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0x12);
      compactWriteCursor[1] = persistedCellDword;
      AVar1 = *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0x10);
      compactWriteCursor[2] =
           *(AssetMagic *)((sourceGrid->common).buildMetadata.names.producerName + 0xe);
      compactWriteCursor[3] = AVar1;
      sourceGrid = (FieldGridAsset *)((sourceGrid->common).buildMetadata.names.sourceName + 8);
      compactWriteCursor = compactWriteCursor + 4;
      uVar2 = uVar2 - 1;
    } while (uVar2 != 0);
    *(PckCompactFieldImageByteCount *)destination = compactImageSizeBytes;
    bVar3 = (byte *)0xffffffef < destination;
    PckCodec_EncodeHuffmanRle
              (destinationCapacityBytes - 0x10,destination + 0x10,compactImageSizeBytes,
               (byte *)compactFieldImageBase);
    if (!bVar3) {
      (*g_MemoryApi.free)(compactFieldImageBase);
      return (dword)(compactFieldImageBase + 4);
    }
    (*g_MemoryApi.free)(compactFieldImageBase);
  }
  return (dword)compactFieldImageBase;
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
dword PckCodec_DecodeFieldGrid
                (PckOutputCapacityBytes destinationCapacityBytes,FieldGridAsset *destinationGrid,
                PckStoredByteCount sourceSizeBytes,byte *source)

{
  AssetMagic AVar1;
  dword dVar2;
  AssetMagic *compactFieldImageBase;
  PckDecodedByteCount outputSizeBytes;
  int iVar3;
  FieldGridDimension columnsRemaining;
  int iVar4;
  FieldGridDimension rowsRemaining;
  Q12 currentWorldYQ12;
  AssetMagic *compactReadCursor;
  FieldGridCell *currentWorldCoordinateCell;
  FieldGridAsset *expandedZeroCursor;
  FieldGridAsset *expandedWriteCursor;
  bool bVar5;
  void *pvVar6;
  FieldGridDimension gridWidth;
  
  bVar5 = sourceSizeBytes < 0x10;
  _pvVar6 = (*g_MemoryApi.alloc)(*(dword *)source);
  compactFieldImageBase = SUB84(_pvVar6,0);
  if (!bVar5) {
    bVar5 = (byte *)0xffffffef < source;
    PckCodec_DecodeHuffmanRle
              (outputSizeBytes,(byte *)compactFieldImageBase,
               (PckStoredByteCount)((ulonglong)_pvVar6 >> 0x20),source + 0x10);
    if (!bVar5) {
      iVar4 = compactFieldImageBase[0x2e] * compactFieldImageBase[0x2f];
      compactReadCursor = compactFieldImageBase;
      expandedWriteCursor = destinationGrid;
      for (iVar3 = 0x80; iVar3 != 0; iVar3 = iVar3 + -1) {
        (expandedWriteCursor->common).magic = *compactReadCursor;
        compactReadCursor = compactReadCursor + 1;
        expandedWriteCursor = (FieldGridAsset *)&(expandedWriteCursor->common).allocationSizeBytes;
      }
      expandedZeroCursor = expandedWriteCursor;
      for (iVar3 = iVar4 * 0x20; iVar3 != 0; iVar3 = iVar3 + -1) {
        (expandedZeroCursor->common).magic = 0;
        expandedZeroCursor = (FieldGridAsset *)&(expandedZeroCursor->common).allocationSizeBytes;
      }
      do {
        AVar1 = compactReadCursor[1];
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0x12) =
             *compactReadCursor;
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0xc) =
             AVar1;
        AVar1 = compactReadCursor[3];
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0xe) =
             compactReadCursor[2];
        *(AssetMagic *)((expandedWriteCursor->common).buildMetadata.names.producerName + 0x10) =
             AVar1;
        compactReadCursor = compactReadCursor + 4;
        expandedWriteCursor =
             (FieldGridAsset *)((expandedWriteCursor->common).buildMetadata.names.sourceName + 8);
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      iVar4 = 0;
      currentWorldYQ12 = 0;
      gridWidth = destinationGrid->gridWidth;
      rowsRemaining = destinationGrid->gridHeight;
      currentWorldCoordinateCell = destinationGrid->cells;
      columnsRemaining = gridWidth;
      iVar3 = 0;
      do {
        do {
          currentWorldCoordinateCell->worldX = iVar4;
          currentWorldCoordinateCell->worldY = currentWorldYQ12;
          iVar4 = iVar4 + 0x901;
          currentWorldCoordinateCell = currentWorldCoordinateCell + 1;
          columnsRemaining = columnsRemaining - 1;
        } while (columnsRemaining != 0);
        iVar4 = iVar3 + 0x480;
        currentWorldYQ12 = currentWorldYQ12 + -1999;
        rowsRemaining = rowsRemaining - 1;
        columnsRemaining = gridWidth;
        iVar3 = iVar4;
      } while (rowsRemaining != 0);
      dVar2 = (*g_MemoryApi.free)(compactFieldImageBase);
      return dVar2;
    }
    compactFieldImageBase = (AssetMagic *)(*g_MemoryApi.free)(compactFieldImageBase);
  }
  return (dword)compactFieldImageBase;
}

/* Address: 0x0040A960.
   Ownership: assets/package/codec.
   Purpose: Copies sourceSize bytes when destinationCapacity is large enough and returns the four-byte-aligned
   size. This is PCK compression method 1. Method 1 writer: plain dword-tail-safe copy.
*/
dword PckCodec_EncodeStored
                (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
                PckDecodedByteCount sourceSizeBytes,byte *source)

{
  PckDwordCopyCount dwordCopyCount;
  
  if (sourceSizeBytes <= destinationCapacityBytes) {
    for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0;
        dwordCopyCount = dwordCopyCount - 1) {
      *(undefined4 *)destination = *(undefined4 *)source;
      source = source + 4;
      destination = destination + 4;
    }
    return sourceSizeBytes + 3 & 0xfffffffc;
  }
  return 0x14;
}

/* Address: 0x0040A9A0.
   Ownership: assets/package/codec.
   Purpose: Copies sourceSize bytes directly to the destination. This is PCK compression method 1. Method 1 reader:
   stored copy.
*/
dword PckCodec_DecodeStored
                (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
                PckStoredByteCount sourceSizeBytes,byte *source)

{
  dword in_EAX;
  PckDwordCopyCount dwordCopyCount;
  
  for (dwordCopyCount = sourceSizeBytes >> 2; dwordCopyCount != 0;
      dwordCopyCount = dwordCopyCount - 1) {
    *(undefined4 *)destination = *(undefined4 *)source;
    source = source + 4;
    destination = destination + 4;
  }
  return in_EAX;
}

/* Address: 0x0040A4C0.
   Ownership: assets/package/codec.
   Purpose: Builds the package Huffman tree, emits the 256-byte frequency table, and encodes literals plus
   3-to-18-byte repeated runs. Returns the aligned packed size with CF clear; returns error 0x14 with CF set on
   failure. PCK compressionMethod 0 writer: order-0 Huffman over literal/RLE symbols.
*/
dword PckCodec_EncodeHuffmanRle
                (PckOutputCapacityBytes destinationCapacityBytes,byte *destination,
                PckDecodedByteCount sourceSizeBytes,byte *source)

{
  PckHuffmanNodePtr pPVar1;
  uint uVar2;
  PckHuffmanBitOffset outputBitOffset;
  int iVar3;
  PckDecodedByteCount PVar4;
  uint uVar5;
  PckHuffmanNode *unaff_EBX;
  byte *pbVar6;
  PckHuffmanSymbolState *pPVar7;
  PckHuffmanNode *pPVar8;
  byte *frequencyByteCursor;
  undefined4 *workspaceClearCursor;
  PckHuffmanNode *pPVar9;
  PckHuffmanNodePtr currentLeafNode;
  uint *puVar10;
  uint *puVar11;
  PckHuffmanNodePtr nextInternalNode;
  byte currentSymbolByte;
  PckHuffmanNodePtr parentNode;
  
  workspaceClearCursor = g_PckHuffmanSymbolWorkspace256;
  for (iVar3 = 0x900; PVar4 = sourceSizeBytes, pbVar6 = source, iVar3 != 0; iVar3 = iVar3 + -1) {
    *workspaceClearCursor = 0;
    workspaceClearCursor = workspaceClearCursor + 1;
  }
  do {
    g_PckHuffmanSymbolWorkspace256[*pbVar6].frequencyCount =
         g_PckHuffmanSymbolWorkspace256[*pbVar6].frequencyCount + 1;
    PVar4 = PVar4 - 1;
    pbVar6 = pbVar6 + 1;
  } while (PVar4 != 0);
  uVar2 = 0;
  pPVar7 = g_PckHuffmanSymbolWorkspace256;
  do {
    if (uVar2 < pPVar7->frequencyCount) {
      uVar2 = pPVar7->frequencyCount;
    }
    pPVar7 = pPVar7 + 1;
  } while (pPVar7 < g_PckHuffmanLeafNodeWorkspace256);
  iVar3 = 0;
  for (; 0xff < uVar2; uVar2 = uVar2 + 1 >> 1) {
    iVar3 = iVar3 + 1;
  }
  if (iVar3 != 0) {
    pPVar7 = g_PckHuffmanSymbolWorkspace256;
    do {
      pPVar7->frequencyCount = pPVar7->frequencyCount + (1 << ((byte)iVar3 & 0x1f)) + -1;
      pPVar7->frequencyCount = pPVar7->frequencyCount >> ((byte)iVar3 & 0x1f);
      pPVar7 = pPVar7 + 1;
    } while (pPVar7 < g_PckHuffmanLeafNodeWorkspace256);
  }
  pPVar7 = g_PckHuffmanSymbolWorkspace256;
  pPVar9 = g_PckHuffmanLeafNodeWorkspace256;
  do {
    pPVar9->weight = (PckHuffmanWeight)*pPVar7;
    pPVar7 = pPVar7 + 1;
    pPVar9 = pPVar9 + 1;
  } while (pPVar7 < g_PckHuffmanLeafNodeWorkspace256);
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  while( true ) {
    pPVar8 = g_PckHuffmanLeafNodeWorkspace256;
    uVar2 = 0xffffffff;
    iVar3 = 0x200;
    uVar5 = 0xffffffff;
    do {
      if (pPVar8->weight != 0) {
        if (pPVar8->weight < uVar2) {
          if (uVar2 < uVar5) {
            uVar5 = uVar2;
            pPVar9 = unaff_EBX;
          }
          uVar2 = pPVar8->weight;
          unaff_EBX = pPVar8;
        }
        else if (pPVar8->weight < uVar5) {
          uVar5 = pPVar8->weight;
          pPVar9 = pPVar8;
        }
      }
      pPVar8 = pPVar8 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    if ((int)uVar5 < 0) break;
    nextInternalNode->weight = uVar2 + uVar5;
    nextInternalNode->zeroChild = unaff_EBX;
    nextInternalNode->oneChild = pPVar9;
    unaff_EBX->parent = nextInternalNode;
    pPVar9->parent = nextInternalNode;
    unaff_EBX->weight = 0;
    pPVar9->weight = 0;
    nextInternalNode = nextInternalNode + 1;
    if ((PckHuffmanNodePtr)((int)&g_PckHuffmanInternalNodeWorkspace256[0xff].parent + 3) <
        nextInternalNode) {
      return 0x14;
    }
  }
  if (destinationCapacityBytes < 0x100) {
    return 0x14;
  }
  iVar3 = 0x100;
  puVar11 = (uint *)(destination + 0x100);
  frequencyByteCursor = (byte *)g_PckHuffmanSymbolWorkspace256;
  do {
    *destination = *frequencyByteCursor;
    frequencyByteCursor = frequencyByteCursor + 4;
    destination = destination + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  pPVar7 = g_PckHuffmanSymbolWorkspace256;
  uVar2 = 0;
  do {
    if (pPVar7->frequencyCount != 0) {
      iVar3 = 0;
      uVar5 = 0;
      currentLeafNode = g_PckHuffmanLeafNodeWorkspace256 + uVar2;
      do {
        pPVar1 = currentLeafNode->parent;
        uVar5 = uVar5 * 2;
        iVar3 = iVar3 + 1;
        if (currentLeafNode == pPVar1->oneChild) {
          uVar5 = uVar5 + 1;
        }
        currentLeafNode = pPVar1;
      } while (pPVar1->weight == 0);
      pPVar7->frequencyCount = uVar5 | iVar3 * 0x1000000;
    }
    uVar2 = uVar2 + 1;
    pPVar7 = pPVar7 + 1;
  } while (uVar2 < 0x100);
  uVar2 = destinationCapacityBytes - 0x100 & 0xfffffffc;
  if (uVar2 != 0) {
    puVar10 = puVar11;
    for (uVar5 = destinationCapacityBytes - 0x100 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    destinationCapacityBytes = uVar2 - 4;
    if (3 < uVar2) {
      outputBitOffset = 0;
      uVar2 = 0x11f;
      do {
        while( true ) {
          currentSymbolByte = *source;
          if (((sourceSizeBytes < 3) || (currentSymbolByte != source[1])) ||
             (currentSymbolByte != source[2])) break;
          uVar5 = 0;
          do {
            if ((currentSymbolByte != *source) || (0x11 < uVar5)) break;
            uVar5 = uVar5 + 1;
            source = source + 1;
            sourceSizeBytes = sourceSizeBytes - 1;
          } while (sourceSizeBytes != 0);
          *puVar11 = *puVar11 | uVar5 * 2 + -5 << (outputBitOffset & 0x1f);
          uVar5 = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
          *puVar11 = *puVar11 | (uVar5 & 0xffffff) << (outputBitOffset + 5 & 0x1f);
          for (outputBitOffset = outputBitOffset + 5 + (char)(uVar5 >> 0x18); 7 < outputBitOffset;
              outputBitOffset = outputBitOffset - 8) {
            puVar11 = (uint *)((int)puVar11 + 1);
            uVar2 = uVar2 + 1;
            destinationCapacityBytes = destinationCapacityBytes - 1;
            if (destinationCapacityBytes == 0) {
              return 0x14;
            }
          }
          if (sourceSizeBytes == 0)
          goto PckCodec_EncodeHuffmanRle_FinalizeBitstreamAndReturnAlignedSizeWithCarryClear;
        }
        uVar5 = g_PckHuffmanSymbolWorkspace256[currentSymbolByte].frequencyCount;
        *puVar11 = *puVar11 | (uVar5 & 0xffffff) << (outputBitOffset + 1 & 0x1f);
        for (outputBitOffset = outputBitOffset + 1 + (char)(uVar5 >> 0x18); 7 < outputBitOffset;
            outputBitOffset = outputBitOffset - 8) {
          puVar11 = (uint *)((int)puVar11 + 1);
          uVar2 = uVar2 + 1;
          destinationCapacityBytes = destinationCapacityBytes - 1;
          if (destinationCapacityBytes == 0) {
            return 0x14;
          }
        }
        source = source + 1;
        sourceSizeBytes = sourceSizeBytes - 1;
      } while (sourceSizeBytes != 0);
PckCodec_EncodeHuffmanRle_FinalizeBitstreamAndReturnAlignedSizeWithCarryClear:
      if (outputBitOffset != 0) {
        uVar2 = uVar2 + 1;
      }
      return uVar2 & 0xfffffff0;
    }
  }
  return 0x14;
}

/* Address: 0x0040A790.
   Ownership: assets/package/codec.
   Purpose: Rebuilds the package Huffman tree from the first 256 source bytes and decodes literal or repeated-run
   tokens until outputSize bytes have been produced. CF reports success or failure. PCK compressionMethod 0 reader.
*/
dword PckCodec_DecodeHuffmanRle
                (PckDecodedByteCount outputSizeBytes,byte *destination,
                PckStoredByteCount sourceSizeBytes,byte *source)

{
  PckHuffmanSymbolState PVar1;
  uint uVar2;
  PckHuffmanRunLength runLength;
  PckHuffmanBitOffset nextBitOffset;
  PckHuffmanBitOffset inputBitOffset;
  int iVar3;
  uint uVar4;
  PckHuffmanNode *unaff_EBX;
  PckHuffmanNodePtr pPVar5;
  PckHuffmanNodePtr currentHuffmanNode;
  byte *frequencyByteCursor;
  PckHuffmanNode *pPVar6;
  uint *puVar7;
  PckHuffmanSymbolState *pPVar8;
  PckHuffmanNode *pPVar9;
  PckHuffmanNodePtr nextInternalNode;
  
  pPVar8 = g_PckHuffmanSymbolWorkspace256;
  frequencyByteCursor = source;
  do {
    PVar1.frequencyCount._1_3_ = 0;
    PVar1.frequencyCount._0_1_ = *frequencyByteCursor;
    *pPVar8 = PVar1;
    frequencyByteCursor = frequencyByteCursor + 1;
    pPVar8 = pPVar8 + 1;
  } while (pPVar8 < g_PckHuffmanLeafNodeWorkspace256);
  for (iVar3 = 0x800; iVar3 != 0; iVar3 = iVar3 + -1) {
    pPVar8->frequencyCount = 0;
    pPVar8 = pPVar8 + 1;
  }
  pPVar8 = g_PckHuffmanSymbolWorkspace256;
  pPVar9 = g_PckHuffmanLeafNodeWorkspace256;
  do {
    pPVar9->weight = (PckHuffmanWeight)*pPVar8;
    pPVar8 = pPVar8 + 1;
    pPVar9 = pPVar9 + 1;
  } while (pPVar8 < g_PckHuffmanLeafNodeWorkspace256);
  nextInternalNode = g_PckHuffmanInternalNodeWorkspace256;
  while( true ) {
    pPVar6 = g_PckHuffmanLeafNodeWorkspace256;
    uVar2 = 0xffffffff;
    iVar3 = 0x200;
    uVar4 = 0xffffffff;
    do {
      if (pPVar6->weight != 0) {
        if (pPVar6->weight < uVar2) {
          if (uVar2 < uVar4) {
            uVar4 = uVar2;
            pPVar9 = unaff_EBX;
          }
          uVar2 = pPVar6->weight;
          unaff_EBX = pPVar6;
        }
        else if (pPVar6->weight < uVar4) {
          uVar4 = pPVar6->weight;
          pPVar9 = pPVar6;
        }
      }
      pPVar6 = pPVar6 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    if ((int)uVar4 < 0) break;
    nextInternalNode->weight = uVar2 + uVar4;
    nextInternalNode->zeroChild = unaff_EBX;
    nextInternalNode->oneChild = pPVar9;
    unaff_EBX->parent = nextInternalNode;
    pPVar9->parent = nextInternalNode;
    unaff_EBX->weight = 0;
    pPVar9->weight = 0;
    nextInternalNode = nextInternalNode + 1;
    if (PckCodec_EncodeHuffmanRle <= nextInternalNode) {
      return 0x14;
    }
  }
  inputBitOffset = 0;
  puVar7 = (uint *)(source + 0x100);
  do {
    while( true ) {
      uVar2 = *puVar7 >> (inputBitOffset & 0x1f);
      nextBitOffset = inputBitOffset + 1;
      if ((uVar2 & 1) != 0) break;
      uVar2 = uVar2 >> 1;
      pPVar5 = nextInternalNode + -1;
      do {
        if ((uVar2 & 1) == 0) {
          pPVar5 = pPVar5->zeroChild;
        }
        else {
          pPVar5 = pPVar5->oneChild;
        }
        uVar2 = uVar2 >> 1;
        nextBitOffset = nextBitOffset + 1;
      } while (pPVar5->zeroChild != (PckHuffmanNodePtr)0x0);
      *destination = (byte)((uint)(pPVar5 + -0x4084c) >> 4);
      destination = destination + 1;
      for (inputBitOffset = nextBitOffset; 7 < inputBitOffset; inputBitOffset = inputBitOffset - 8)
      {
        puVar7 = (uint *)((int)puVar7 + 1);
      }
      outputSizeBytes = outputSizeBytes - 1;
      if (outputSizeBytes == 0) {
        return uVar2;
      }
    }
    uVar4 = uVar2 >> 5;
    inputBitOffset = inputBitOffset + 5;
    currentHuffmanNode = nextInternalNode + -1;
    do {
      if ((uVar4 & 1) == 0) {
        currentHuffmanNode = currentHuffmanNode->zeroChild;
      }
      else {
        currentHuffmanNode = currentHuffmanNode->oneChild;
      }
      uVar4 = uVar4 >> 1;
      inputBitOffset = inputBitOffset + 1;
    } while (currentHuffmanNode->zeroChild != (PckHuffmanNodePtr)0x0);
    for (; 7 < inputBitOffset; inputBitOffset = inputBitOffset - 8) {
      puVar7 = (uint *)((int)puVar7 + 1);
    }
    runLength = (uVar2 >> 1 & 0xf) + 3;
    do {
      *destination = (byte)((uint)(currentHuffmanNode + -0x4084c) >> 4);
      destination = destination + 1;
      outputSizeBytes = outputSizeBytes - 1;
      if (outputSizeBytes == 0) break;
      runLength = runLength - 1;
    } while (runLength != 0);
    if (outputSizeBytes == 0) {
      return runLength;
    }
  } while( true );
}
