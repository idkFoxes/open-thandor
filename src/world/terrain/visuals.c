/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/visuals.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/visuals.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: world/terrain/visuals. */

/* Address: 0x0053D370.
   Ownership: world/terrain/visuals.
   Purpose: Allocates a gfx-compatible runtime image sized as 0x260 plus width*height*12, publishes it globally and
   in the active game state, defines three direct-color source entries with consecutive ARGB planes, then fills
   planes one and two and rebuilds plane zero. CF set propagates allocation failure.
   Local calls: TerrainCompositeTexture_FillPlane1, TerrainCompositeTexture_FillPlane2,
   TerrainCompositeTexture_RebuildPlane0.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx TerrainCompositeTexture_Create(void)

{
  FieldGridAsset *terrainFieldGrid;
  AssetDimension fieldWidth;
  AssetDimension fieldHeight;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  AssetRelativeOffset plane2DataOffset;
  uint totalImageBytes;
  int planeSizeBytes;
  ArenaAllocEaxCf5 allocResult;
  StatusValueEaxCf5 resultStatus;
  TerrainCompositeTextureRuntime *compositeTexture;
  
  inGameRoot = g_InGameRuntimeRoot;
  terrainFieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  fieldWidth = terrainFieldGrid->gridWidth;
  fieldHeight = terrainFieldGrid->gridHeight;
  allocResult = (*g_MemoryApi.alloc)(fieldWidth * 0xc * fieldHeight + 0x260);
  compositeTexture = (TerrainCompositeTextureRuntime *)allocResult.eax;
  if (!allocResult.carry) {
    g_TerrainCompositeTexture = compositeTexture;
    *(TerrainCompositeTextureRuntime **)(inGameRoot->opaque9A74_9B4B + 8) = compositeTexture;
    compositeTexture->sourceEntries[0].pixelWidth = fieldWidth;
    compositeTexture->sourceEntries[0].pixelHeight = fieldHeight;
    compositeTexture->sourceEntries[0].originX = 0;
    compositeTexture->sourceEntries[0].originY = 0;
    planeSizeBytes = fieldWidth * 4 * fieldHeight;
    compositeTexture->sourceEntries[0].paletteIndex = -1;
    compositeTexture->sourceEntries[0].logicalWidth = fieldWidth;
    compositeTexture->sourceEntries[0].logicalHeight = fieldHeight;
    compositeTexture->sourceEntries[0].dataOffset = 0x260;
    compositeTexture->sourceEntries[1].pixelWidth = fieldWidth;
    compositeTexture->sourceEntries[1].pixelHeight = fieldHeight;
    compositeTexture->sourceEntries[1].originX = 0;
    compositeTexture->sourceEntries[1].originY = 0;
    compositeTexture->sourceEntries[1].paletteIndex = -1;
    compositeTexture->sourceEntries[1].logicalWidth = fieldWidth;
    compositeTexture->sourceEntries[1].logicalHeight = fieldHeight;
    compositeTexture->sourceEntries[1].dataOffset = planeSizeBytes + 0x260U;
    compositeTexture->sourceEntries[2].pixelWidth = fieldWidth;
    compositeTexture->sourceEntries[2].pixelHeight = fieldHeight;
    compositeTexture->sourceEntries[2].originX = 0;
    compositeTexture->sourceEntries[2].originY = 0;
    plane2DataOffset = planeSizeBytes + 0x260U + planeSizeBytes;
    compositeTexture->sourceEntries[2].paletteIndex = -1;
    compositeTexture->sourceEntries[2].logicalWidth = fieldWidth;
    compositeTexture->sourceEntries[2].logicalHeight = fieldHeight;
    compositeTexture->sourceEntries[2].dataOffset = plane2DataOffset;
    (compositeTexture->textureSource).common.magic = ASSET_MAGIC_GFX;
    totalImageBytes = plane2DataOffset + planeSizeBytes;
    (compositeTexture->textureSource).tableDescriptor.subresourceCount = 3;
    (compositeTexture->textureSource).tableDescriptor.paletteBankCount = 0;
    (compositeTexture->textureSource).tableDescriptor.subresourceTableOffset = 0x200;
    (compositeTexture->textureSource).opaqueTablePayloadBC_1FF[0] = 0;
    (compositeTexture->textureSource).opaqueTablePayloadBC_1FF[1] = 0;
    (compositeTexture->textureSource).opaqueTablePayloadBC_1FF[2] = 0;
    (compositeTexture->textureSource).opaqueTablePayloadBC_1FF[3] = 0;
    (compositeTexture->textureSource).common.allocationSizeBytes = totalImageBytes;
    TerrainCompositeTexture_FillPlane1();
    TerrainCompositeTexture_FillPlane2();
    TerrainCompositeTexture_RebuildPlane0();
    allocResult.carry = false;
    allocResult.eax = totalImageBytes;
  }
  resultStatus.valueOrError = allocResult.eax;
  resultStatus.carry = allocResult.carry;
  return resultStatus;
}


/* Address: 0x00503B10.
   Ownership: world/terrain/visuals.
   Purpose: Allocates and 64-KiB-aligns the shared terrain byte lookup and fills its clamp/offset tables using
   delta 0x15 with verified caps 0x87 and 0xFF.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx TerrainByteClampLookup_Initialize(void)

{
  void *lookupAllocationBase;
  int lookupRowsRemaining;
  int finalRowsRemaining;
  byte nextInputByte;
  uint clampInputValue;
  uint lookupInputValue;
  byte *lookupWriteCursor;
  ArenaAllocEaxCf5 allocResult;
  
  allocResult = (*g_MemoryApi.alloc)(0x20000);
  lookupAllocationBase = (void *)allocResult.eax;
  if (allocResult.carry) {
    return StatusValue_Fail(allocResult.eax);
  }
  clampInputValue = 0;
  lookupWriteCursor = (byte *)((int)lookupAllocationBase + 0xffffU & 0xffff0000);
  lookupRowsRemaining = 0x40;
  g_TerrainByteClampLookup = lookupWriteCursor;
  do {
    do {
      if (clampInputValue == 0) {
        *lookupWriteCursor = 0;
      }
      else if ((int)(clampInputValue - 0x15) < 1) {
        *lookupWriteCursor = 0;
      }
      else {
        *lookupWriteCursor = (byte)(clampInputValue - 0x15);
      }
      lookupWriteCursor = lookupWriteCursor + 1;
      nextInputByte = (char)clampInputValue + 1;
      clampInputValue = (uint)nextInputByte;
    } while (nextInputByte != 0);
    lookupInputValue = 0;
    do {
      if (lookupInputValue < 0x100) {
        if (lookupInputValue + 0x15 < 0xff) {
          *lookupWriteCursor = (byte)(lookupInputValue + 0x15);
        }
        else {
          *lookupWriteCursor = 0xff;
        }
      }
      else if ((int)(lookupInputValue - 0x15) < 0x100) {
        *lookupWriteCursor = 0xff;
      }
      else {
        *lookupWriteCursor = (byte)(lookupInputValue - 0x15);
      }
      lookupWriteCursor = lookupWriteCursor + 1;
      nextInputByte = (char)lookupInputValue + 1;
      lookupInputValue = (uint)nextInputByte;
    } while (nextInputByte != 0);
    lookupRowsRemaining = lookupRowsRemaining + -1;
    clampInputValue = 0;
  } while (lookupRowsRemaining != 0);
  lookupInputValue = 0;
  do {
    if (lookupInputValue < 0x88) {
      if (lookupInputValue + 0x15 < 0x87) {
        *lookupWriteCursor = (byte)(lookupInputValue + 0x15);
      }
      else {
        *lookupWriteCursor = 0x87;
      }
    }
    else if ((int)(lookupInputValue - 0x15) < 0x88) {
      *lookupWriteCursor = 0x87;
    }
    else {
      *lookupWriteCursor = (byte)(lookupInputValue - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint)nextInputByte;
  } while (nextInputByte != 0);
  lookupInputValue = 0;
  do {
    if (lookupInputValue < 0x100) {
      if (lookupInputValue + 0x15 < 0xff) {
        *lookupWriteCursor = (byte)(lookupInputValue + 0x15);
      }
      else {
        *lookupWriteCursor = 0xff;
      }
    }
    else if ((int)(lookupInputValue - 0x15) < 0x100) {
      *lookupWriteCursor = 0xff;
    }
    else {
      *lookupWriteCursor = (byte)(lookupInputValue - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint)nextInputByte;
  } while (nextInputByte != 0);
  lookupInputValue = 0;
  do {
    if (lookupInputValue < 0x88) {
      if (lookupInputValue + 0x15 < 0x87) {
        *lookupWriteCursor = (byte)(lookupInputValue + 0x15);
      }
      else {
        *lookupWriteCursor = 0x87;
      }
    }
    else if ((int)(lookupInputValue - 0x15) < 0x88) {
      *lookupWriteCursor = 0x87;
    }
    else {
      *lookupWriteCursor = (byte)(lookupInputValue - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint)nextInputByte;
  } while (nextInputByte != 0);
  finalRowsRemaining = 0x7d;
  lookupInputValue = 0;
  do {
    if (lookupInputValue < 0x100) {
      if (lookupInputValue + 0x15 < 0xff) {
        *lookupWriteCursor = (byte)(lookupInputValue + 0x15);
      }
      else {
        *lookupWriteCursor = 0xff;
      }
    }
    else if ((int)(lookupInputValue - 0x15) < 0x100) {
      *lookupWriteCursor = 0xff;
    }
    else {
      *lookupWriteCursor = (byte)(lookupInputValue - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint)nextInputByte;
  } while ((nextInputByte != 0) || (finalRowsRemaining = finalRowsRemaining + -1, finalRowsRemaining != 0));
  return StatusValue_Ok(0);
}


/* Address: 0x00503F30.
   Ownership: world/terrain/visuals.
   Purpose: CF set propagates any resource failure. Ends in cell/boundary init + first direction-table pass (see
   W10 bl...
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], MoviePlayback_AdvanceScheduledFrameAndTick
   [movie/runtime/playback], Package_LoadEntry [assets/package/runtime],
   FieldGrid_InitializeRuntimeCellsAndBoundaryFlags [world/terrain/grid], Random_NextPrimary [core/math/random],
   TerrainDirectionTable_AdvanceAndRebuildVectors [world/terrain/grid].
*/
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx
TerrainVisualResources_LoadPrimary
          (word *primaryResourcePath,word *secondaryResourcePath,FieldGridAsset *field)

{
  GraphicsPaletteAsset *loadedResourceOrError;
  GraphicsTextureSet *materialTextureSet;
  dword randomValue;
  int loopCounter;
  int materialSlotsRemaining;
  word pathCharOrRotationRate;
  uint materialFlagBits;
  TerrainMaterialSuffixEntry *suffixLetterCursor;
  word *pathScanCursor;
  word *pathScanNext;
  GraphicsTextureSet **materialTextureSetSlot;
  TerrainDirectionRecord *directionRecord;
  GraphicsTextureSetEaxCf5 textureSetLoad;
  PackageLoadEntryEaxCf5 packageLoad;
  GraphicsPaletteAssetEaxCf5 paletteLoad;
  StatusValueEaxCf5 randomOrSuccessStatus;
  StatusValueEaxCf5 failureStatus;
  TerrainMaterialSuffixEntry *pathSuffixEntry;
  
  loopCounter = 0x100;
  pathScanCursor = secondaryResourcePath;
  do {
    pathScanNext = pathScanCursor;
    if (loopCounter == 0) break;
    loopCounter = loopCounter + -1;
    pathScanNext = pathScanCursor + 1;
    pathCharOrRotationRate = *pathScanCursor;
    pathScanCursor = pathScanNext;
  } while (pathCharOrRotationRate != 0);
  pathSuffixEntry = (TerrainMaterialSuffixEntry *)(pathScanNext + -1);
  loadedResourceOrError = (GraphicsPaletteAsset *)0x38;
  if (((field->common).magic == ASSET_MAGIC_FLD) &&
     ((field->common).converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    materialFlagBits = field->fieldFlags;
    loopCounter = 0;
    do {
      if ((materialFlagBits & 1) != 0) {
        loopCounter = loopCounter + 1;
      }
      materialFlagBits = materialFlagBits >> 1;
    } while (materialFlagBits != 0);
    materialSlotsRemaining = 0x1a;
    materialFlagBits = field->fieldFlags;
    g_MoviePlaybackScheduleSpan = loopCounter * 2 + 10;
    suffixLetterCursor = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ;
    materialTextureSetSlot = g_TerrainMaterialTextureSets;
    do {
      if ((materialFlagBits & 1) == 0) {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        textureSetLoad = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        materialTextureSet = textureSetLoad.textureSet;
        if (textureSetLoad.carry) {
          materialTextureSet = (GraphicsTextureSet *)0x0;
        }
        *materialTextureSetSlot = materialTextureSet;
      }
      else {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        textureSetLoad = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
        if (textureSetLoad.carry)
        goto TerrainVisualResources_LoadPrimary_ReturnFieldOrResourceLoadFailureStatus;
        MoviePlayback_AdvanceScheduledFrameAndTick();
        *materialTextureSetSlot = (GraphicsTextureSet *)loadedResourceOrError;
      }
      suffixLetterCursor = suffixLetterCursor + 1;
      materialTextureSetSlot = materialTextureSetSlot + 1;
      materialFlagBits = materialFlagBits >> 1;
      materialSlotsRemaining = materialSlotsRemaining + -1;
    } while (materialSlotsRemaining != 0);
    WidePath_SetExtensionCode(0x746164,primaryResourcePath);
    packageLoad = Package_LoadEntry(primaryResourcePath);
    loadedResourceOrError = packageLoad.bufferOrError;
    if (!packageLoad.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainSurfacePacketTablePayload =
           &((GraphicsTextureSetEntry *)loadedResourceOrError->reserved08_AF)->reserved18;
      WidePath_SetExtensionCode(0x786667,primaryResourcePath);
      textureSetLoad = (*g_GraphicsTextureSetLoadPackageCf)(primaryResourcePath);
      loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
      if (!textureSetLoad.carry) {
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)loadedResourceOrError;
        WidePath_SetExtensionCode(0x6c6170,primaryResourcePath);
        paletteLoad = (*g_GraphicsPaletteAssetLoadPackage)(primaryResourcePath);
        loadedResourceOrError = paletteLoad.paletteAsset;
        if (!paletteLoad.carry) {
          MoviePlayback_AdvanceScheduledFrameAndTick();
          g_TerrainPrimaryPalette = loadedResourceOrError;
          pathSuffixEntry->lowercaseLetterUtf16 = 0;
          pathSuffixEntry->terminator = 0;
          WidePath_SetExtensionCode(0x6c6170,secondaryResourcePath);
          paletteLoad = (*g_GraphicsPaletteAssetLoadPackage)(secondaryResourcePath);
          loadedResourceOrError = paletteLoad.paletteAsset;
          if (!paletteLoad.carry) {
            MoviePlayback_AdvanceScheduledFrameAndTick();
            g_TerrainSecondaryPalette = loadedResourceOrError;
            pathSuffixEntry->lowercaseLetterUtf16 = 0;
            pathSuffixEntry->terminator = 0;
            WidePath_SetExtensionCode(0x746164,secondaryResourcePath);
            packageLoad = Package_LoadEntry(secondaryResourcePath);
            loadedResourceOrError = packageLoad.bufferOrError;
            if (!packageLoad.carry) {
              MoviePlayback_AdvanceScheduledFrameAndTick();
              g_TerrainSoilPacketTablePayload =
                   &((GraphicsTextureSetEntry *)loadedResourceOrError->reserved08_AF)->reserved18;
              FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(field);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              loopCounter = 0x100;
              directionRecord = g_TerrainDirectionRecordTable256;
              do {
                randomValue = Random_NextPrimary();
                directionRecord->scaleA = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((ushort)(randomValue >> 0x10) & 0x7f) + 0x200;
                randomValue = Random_NextPrimary();
                if ((int)randomValue < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateA = pathCharOrRotationRate;
                *(short *)&directionRecord->packedAngleA_low16_AngleB_high16 = (short)randomValue;
                randomValue = Random_NextPrimary();
                directionRecord->scaleB = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((ushort)(randomValue >> 0x10) & 0x7f) + 0x200;
                randomOrSuccessStatus.valueOrError = Random_NextPrimary();
                if ((int)randomOrSuccessStatus.valueOrError < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateB = pathCharOrRotationRate;
                *(short *)((int)&directionRecord->packedAngleA_low16_AngleB_high16 + 2) =
                     (short)randomOrSuccessStatus.valueOrError;
                directionRecord->angleAComponent0ScaledQ28 = 0;
                directionRecord->angleAComponent1ScaledQ28 = 0;
                directionRecord->angleBComponent0ScaledQ28 = 0;
                directionRecord = directionRecord + 1;
                loopCounter = loopCounter + -1;
              } while (loopCounter != 0);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              TerrainDirectionTable_AdvanceAndRebuildVectors();
              randomOrSuccessStatus.carry = false;
              return randomOrSuccessStatus;
            }
          }
        }
      }
    }
  }
TerrainVisualResources_LoadPrimary_ReturnFieldOrResourceLoadFailureStatus:
  failureStatus.carry = true;
  failureStatus.valueOrError = (dword)loadedResourceOrError;
  return failureStatus;
}


/* Address: 0x005041C0.
   Ownership: world/terrain/visuals.
   Purpose: Performs the same verified terrain texture, palette, data, lookup, and animation setup as
   TerrainVisualResources_LoadPrimary, then clears bit 0x10000000 in flagsAndMaterial for every 0x80-byte field
   cell. CF set propagates any resource failure. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Clears FLD +0x50 bit
   0x10000000 over all cells. The clear operation is exact; the higher-level semantic meaning remains unresolved
   and must stay conservatively named.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], MoviePlayback_AdvanceScheduledFrameAndTick
   [movie/runtime/playback], Package_LoadEntry [assets/package/runtime], FieldGrid_RebuildCellLookupPointers
   [world/terrain/grid], Random_NextPrimary [core/math/random], TerrainDirectionTable_AdvanceAndRebuildVectors
   [world/terrain/grid].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TerrainVisualResources_LoadAndClearCellOverlayFlags
          (word *primaryResourcePath,word *secondaryResourcePath,FieldGridAsset *field)

{
  GraphicsPaletteAsset *loadedResourceOrError;
  GraphicsTextureSet *materialTextureSet;
  dword randomValue;
  int loopCounter;
  int materialSlotsRemaining;
  word pathCharOrRotationRate;
  uint materialFlagBits;
  TerrainMaterialSuffixEntry *suffixLetterCursor;
  FieldGridCell *fieldCell;
  word *pathScanCursor;
  word *pathScanNext;
  GraphicsTextureSet **materialTextureSetSlot;
  TerrainDirectionRecord *directionRecord;
  GraphicsTextureSetEaxCf5 textureSetLoad;
  PackageLoadEntryEaxCf5 packageLoad;
  GraphicsPaletteAssetEaxCf5 paletteLoad;
  StatusValueEaxCf5 randomOrSuccessStatus;
  StatusValueEaxCf5 failureStatus;
  TerrainMaterialSuffixEntry *pathSuffixEntry;
  
  loopCounter = 0x100;
  pathScanCursor = secondaryResourcePath;
  do {
    pathScanNext = pathScanCursor;
    if (loopCounter == 0) break;
    loopCounter = loopCounter + -1;
    pathScanNext = pathScanCursor + 1;
    pathCharOrRotationRate = *pathScanCursor;
    pathScanCursor = pathScanNext;
  } while (pathCharOrRotationRate != 0);
  pathSuffixEntry = (TerrainMaterialSuffixEntry *)(pathScanNext + -1);
  loadedResourceOrError = (GraphicsPaletteAsset *)0x38;
  if (((field->common).magic == ASSET_MAGIC_FLD) &&
     ((field->common).converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    materialFlagBits = field->fieldFlags;
    loopCounter = 0;
    do {
      if ((materialFlagBits & 1) != 0) {
        loopCounter = loopCounter + 1;
      }
      materialFlagBits = materialFlagBits >> 1;
    } while (materialFlagBits != 0);
    materialSlotsRemaining = 0x1a;
    materialFlagBits = field->fieldFlags;
    g_MoviePlaybackScheduleSpan = loopCounter * 2 + 10;
    suffixLetterCursor = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ;
    materialTextureSetSlot = g_TerrainMaterialTextureSets;
    do {
      if ((materialFlagBits & 1) == 0) {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        textureSetLoad = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        materialTextureSet = textureSetLoad.textureSet;
        if (textureSetLoad.carry) {
          materialTextureSet = (GraphicsTextureSet *)0x0;
        }
        *materialTextureSetSlot = materialTextureSet;
      }
      else {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        textureSetLoad = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
        if (textureSetLoad.carry)
        goto 
        TerrainVisualResources_LoadAndClearCellOverlayFlags_ReturnFieldOrResourceLoadFailureStatus;
        MoviePlayback_AdvanceScheduledFrameAndTick();
        *materialTextureSetSlot = (GraphicsTextureSet *)loadedResourceOrError;
      }
      suffixLetterCursor = suffixLetterCursor + 1;
      materialTextureSetSlot = materialTextureSetSlot + 1;
      materialFlagBits = materialFlagBits >> 1;
      materialSlotsRemaining = materialSlotsRemaining + -1;
    } while (materialSlotsRemaining != 0);
    WidePath_SetExtensionCode(0x746164,primaryResourcePath);
    packageLoad = Package_LoadEntry(primaryResourcePath);
    loadedResourceOrError = packageLoad.bufferOrError;
    if (!packageLoad.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainSurfacePacketTablePayload =
           &((GraphicsTextureSetEntry *)loadedResourceOrError->reserved08_AF)->reserved18;
      WidePath_SetExtensionCode(0x786667,primaryResourcePath);
      textureSetLoad = (*g_GraphicsTextureSetLoadPackageCf)(primaryResourcePath);
      loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
      if (!textureSetLoad.carry) {
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)loadedResourceOrError;
        WidePath_SetExtensionCode(0x6c6170,primaryResourcePath);
        paletteLoad = (*g_GraphicsPaletteAssetLoadPackage)(primaryResourcePath);
        loadedResourceOrError = paletteLoad.paletteAsset;
        if (!paletteLoad.carry) {
          MoviePlayback_AdvanceScheduledFrameAndTick();
          g_TerrainPrimaryPalette = loadedResourceOrError;
          pathSuffixEntry->lowercaseLetterUtf16 = 0;
          pathSuffixEntry->terminator = 0;
          WidePath_SetExtensionCode(0x6c6170,secondaryResourcePath);
          paletteLoad = (*g_GraphicsPaletteAssetLoadPackage)(secondaryResourcePath);
          loadedResourceOrError = paletteLoad.paletteAsset;
          if (!paletteLoad.carry) {
            MoviePlayback_AdvanceScheduledFrameAndTick();
            g_TerrainSecondaryPalette = loadedResourceOrError;
            pathSuffixEntry->lowercaseLetterUtf16 = 0;
            pathSuffixEntry->terminator = 0;
            WidePath_SetExtensionCode(0x746164,secondaryResourcePath);
            packageLoad = Package_LoadEntry(secondaryResourcePath);
            loadedResourceOrError = packageLoad.bufferOrError;
            if (!packageLoad.carry) {
              MoviePlayback_AdvanceScheduledFrameAndTick();
              g_TerrainSoilPacketTablePayload =
                   &((GraphicsTextureSetEntry *)loadedResourceOrError->reserved08_AF)->reserved18;
              FieldGrid_RebuildCellLookupPointers(field);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              loopCounter = 0x100;
              directionRecord = g_TerrainDirectionRecordTable256;
              do {
                randomValue = Random_NextPrimary();
                directionRecord->scaleA = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((ushort)(randomValue >> 0x10) & 0x7f) + 0x200;
                randomValue = Random_NextPrimary();
                if ((int)randomValue < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateA = pathCharOrRotationRate;
                *(short *)&directionRecord->packedAngleA_low16_AngleB_high16 = (short)randomValue;
                randomValue = Random_NextPrimary();
                directionRecord->scaleB = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((ushort)(randomValue >> 0x10) & 0x7f) + 0x200;
                randomOrSuccessStatus.valueOrError = Random_NextPrimary();
                if ((int)randomOrSuccessStatus.valueOrError < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateB = pathCharOrRotationRate;
                *(short *)((int)&directionRecord->packedAngleA_low16_AngleB_high16 + 2) =
                     (short)randomOrSuccessStatus.valueOrError;
                directionRecord->angleAComponent0ScaledQ28 = 0;
                directionRecord->angleAComponent1ScaledQ28 = 0;
                directionRecord->angleBComponent0ScaledQ28 = 0;
                directionRecord = directionRecord + 1;
                loopCounter = loopCounter + -1;
              } while (loopCounter != 0);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              loopCounter = field->gridWidth * field->gridHeight;
              fieldCell = field->cells;
              do {
                fieldCell->flagsAndMaterial =
                     fieldCell->flagsAndMaterial &
                     ~FIELD_CELL_TERRAIN_VISUAL_CLEARABLE_UNRESOLVED_BIT28;
                fieldCell = fieldCell + 1;
                loopCounter = loopCounter + -1;
              } while (loopCounter != 0);
              TerrainDirectionTable_AdvanceAndRebuildVectors();
              randomOrSuccessStatus.carry = false;
              return randomOrSuccessStatus;
            }
          }
        }
      }
    }
  }
TerrainVisualResources_LoadAndClearCellOverlayFlags_ReturnFieldOrResourceLoadFailureStatus:
  failureStatus.carry = true;
  failureStatus.valueOrError = (dword)loadedResourceOrError;
  return failureStatus;
}


/* Address: 0x00504470.
   Ownership: world/terrain/visuals.
   Purpose: Releases and clears all 26 material texture sets, the shared primary texture set, both palettes, and
   both package-backed data images. The data pointers are converted back from their verified +0x20 payload
   addresses before Resource_Release.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx TerrainVisualResources_Shutdown(void)

{
  int materialTextureSetsRemaining;
  GraphicsTextureSet **materialTextureSetCursor;
  void *surfacePacketTablePayload;
  GraphicsTextureSet *releasedPrimaryTextureSet;
  
  materialTextureSetCursor = g_TerrainMaterialTextureSets;
  materialTextureSetsRemaining = 0x1a;
  do {
    if (*materialTextureSetCursor != (GraphicsTextureSet *)0x0) {
      (*g_GraphicsTextureSetReleasePackageCf)(*materialTextureSetCursor);
      *materialTextureSetCursor = (GraphicsTextureSet *)0x0;
    }
    materialTextureSetCursor = materialTextureSetCursor + 1;
    materialTextureSetsRemaining = materialTextureSetsRemaining + -1;
  } while (materialTextureSetsRemaining != 0);
  (*g_GraphicsTextureSetReleasePackageCf)(g_TerrainPrimaryTextureSet);
  (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)(g_TerrainSecondaryPalette);
  (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)(g_TerrainPrimaryPalette);
  surfacePacketTablePayload = g_TerrainSurfacePacketTablePayload;
  if (g_TerrainSoilPacketTablePayload != (void *)0x0) {
    Resource_Release((void *)((int)g_TerrainSoilPacketTablePayload + -0x20));
  }
  if (surfacePacketTablePayload != (void *)0x0) {
    Resource_Release((void *)((int)surfacePacketTablePayload + -0x20));
  }
  g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)0x0;
  g_TerrainSecondaryPalette = (GraphicsPaletteAsset *)0x0;
  g_TerrainPrimaryPalette = (GraphicsPaletteAsset *)0x0;
  g_TerrainSoilPacketTablePayload = (void *)0x0;
  g_TerrainSurfacePacketTablePayload = (void *)0x0;
  return;
}


/* Address: 0x00505780.
   Ownership: world/terrain/visuals.
   Purpose: Typed parameters: p2 secondaryColorArgb→PackedArgb32, p3 baseColorArgb→PackedArgb32, p4
   rampStepColorArgb→PackedArgb32. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          )

{
  uint channelValue;
  int rampStepsRemaining;
  uint *rampEntryCursor;
  PackedArgb32 *lightLutCursor;
  
  rampEntryCursor = &g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = 0x100;
  do {
    channelValue = ((rampStepColorArgb & 0xff) * rampStepsRemaining >> 8) + (baseColorArgb & 0xff);
    if (0xff < channelValue) {
      channelValue = 0xff;
    }
    *rampEntryCursor = channelValue;
    rampEntryCursor = rampEntryCursor + 1;
    rampStepsRemaining = rampStepsRemaining + -1;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = &g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = 0x100;
  do {
    channelValue = ((rampStepColorArgb & 0xff00) * rampStepsRemaining >> 8) + (baseColorArgb & 0xff00);
    if (0xffff < channelValue) {
      channelValue = 0xff00;
    }
    *rampEntryCursor = *rampEntryCursor | channelValue & 0xff00;
    rampEntryCursor = rampEntryCursor + 1;
    rampStepsRemaining = rampStepsRemaining + -1;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = &g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = 0x100;
  do {
    channelValue = ((rampStepColorArgb & 0xff0000) * rampStepsRemaining >> 8) + (baseColorArgb & 0xff0000);
    if (0xffffff < channelValue) {
      channelValue = 0xff0000;
    }
    *rampEntryCursor = *rampEntryCursor | channelValue & 0xff0000;
    rampEntryCursor = rampEntryCursor + 1;
    rampStepsRemaining = rampStepsRemaining + -1;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = &g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = 0x100;
  do {
    *rampEntryCursor = *rampEntryCursor | baseColorArgb & 0xff000000;
    rampEntryCursor = rampEntryCursor + 1;
    rampStepsRemaining = rampStepsRemaining + -1;
  } while (rampStepsRemaining != 0);
  g_TerrainDirectionalLightSecondaryColor = secondaryColorArgb;
  lightLutCursor = &g_TerrainDirectionalLightColorLut;
  for (rampStepsRemaining = 0x101; rampStepsRemaining != 0; rampStepsRemaining = rampStepsRemaining + -1) {
    *lightLutCursor = baseColorArgb;
    lightLutCursor = lightLutCursor + 1;
  }
  return;
}


/* Address: 0x0053D4D0.
   Ownership: world/terrain/visuals.
   Purpose: Resolves the allocation base for the global terrain composite texture and frees it. This helper does
   not clear the published pointer.
*/
void __thandor_preserve_eax TerrainCompositeTexture_Destroy(void)

{
  GraphicsTextureSourceAsset *memory;
  
  memory = (*g_GraphicsTextureSourceResolveAllocationBase)
                     (&g_TerrainCompositeTexture->textureSource);
  (*g_MemoryApi.free)(memory);
  return;
}


/* Address: 0x00561EA0.
   Ownership: world/terrain/visuals.
   Purpose: Handles terrain lighting adjust direction and recompute field.
   Cross-module calls: WorldRuntime_RecomputeFieldRegionNormalsAndLighting [world/runtime/core].
*/
void __thandor_preserve_eax_edx
TerrainLighting_AdjustDirectionAndRecomputeField
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  Q12 originWorldYQ12;
  
  originWorldYQ12 = commandArg2 + g_InGameRuntimeRoot->fieldRegionOriginWorldYQ12_0BAC;
  if (-0x1000 < originWorldYQ12) {
    originWorldYQ12 = -0x1000;
  }
  if (originWorldYQ12 < -0x4000) {
    originWorldYQ12 = -0x4000;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            ((g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionHeight,
             (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionWidth,originWorldYQ12,
             commandArg3 + g_InGameRuntimeRoot->fieldRegionOriginWorldXQ12_0BA8 & 0xffff,
             &g_InGameRuntimeRoot->worldRuntime0A30);
  return;
}


/* Address: 0x0053D560.
   Ownership: world/terrain/visuals.
   Purpose: Rebuilds sourceEntries[1] using field-grid terrain height, material selection, active palette data, and
   the verified height-dependent shading table.
*/
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane1(void)

{
  AssetDimension textureWidth;
  int panelSubresourceIndex;
  undefined4 materialColorArgb;
  PackedArgb32 waterColorArgb;
  short shadedLane0;
  short shadedLane1;
  short shadedLane2;
  short shadedLane3;
  GraphicsTextureSourceAsset *panelTextureSource;
  byte mm0PackedValue1ByteLane2;
  byte mm0PackedValue0ByteLane2;
  byte mm0PackedValue0ByteLane3;
  byte mm0PackedValue0ByteLane1;
  int lightingLevelIndex;
  AssetDimension columnsRemaining;
  byte *planePixelCursor;
  FieldGridCell *fieldCell;
  undefined8 mm0PackedValue0;
  byte mm0PackedValue1ByteLane1;
  byte mm0PackedValue1ByteLane3;
  undefined8 mm0PackedValue1;
  AssetDimension rowsRemaining;
  
  panelTextureSource = g_InGamePanelTextureSource;
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  rowsRemaining = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  planePixelCursor = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 +
            (g_TerrainCompositeTexture->sourceEntries[1].dataOffset - 0x28);
  panelSubresourceIndex = *(int *)((int)g_InGamePanelTextureSource[2].common.buildMetadata.names.sourceName +
                  (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset + 0x18);
  fieldCell = ((g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid)->cells;
  columnsRemaining = textureWidth;
  do {
    do {
      if (fieldCell->waterSurfaceDelta < 1) {
        lightingLevelIndex = fieldCell->terrainHeight >> 7;
        materialColorArgb = *(undefined4 *)
                 (panelTextureSource[panelSubresourceIndex * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 (fieldCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK) * 8 + -0x28);
        if (lightingLevelIndex < 0) {
          lightingLevelIndex = 0x70;
        }
        else if (lightingLevelIndex < 0x60) {
          lightingLevelIndex = lightingLevelIndex + 0x70;
        }
        else {
          lightingLevelIndex = 0xcf;
        }
        mm0PackedValue0ByteLane3 = (byte)((uint)materialColorArgb >> 0x18);
        mm0PackedValue0ByteLane2 = (byte)((uint)materialColorArgb >> 0x10);
        mm0PackedValue0ByteLane1 = (byte)((uint)materialColorArgb >> 8);
        mm0PackedValue0 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                          mm0PackedValue0ByteLane3),
                                                                 mm0PackedValue0ByteLane2),
                                                        CONCAT14(mm0PackedValue0ByteLane2,materialColorArgb)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                        mm0PackedValue0ByteLane1) >> 3,
                                               CONCAT11((char)materialColorArgb,(char)materialColorArgb) >> 3))),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        shadedLane0 = (short)mm0PackedValue0;
        shadedLane1 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
        shadedLane2 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
        shadedLane3 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
        *(uint *)planePixelCursor =
             CONCAT13((0 < shadedLane3) * (shadedLane3 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                      (0xff < shadedLane3),
                      CONCAT12((0 < shadedLane2) * (shadedLane2 < 0x100) *
                               (char)((ulonglong)mm0PackedValue0 >> 0x20) - (0xff < shadedLane2),
                               CONCAT11((0 < shadedLane1) * (shadedLane1 < 0x100) *
                                        (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < shadedLane1),
                                        (0 < shadedLane0) * (shadedLane0 < 0x100) * (char)mm0PackedValue0 -
                                        (0xff < shadedLane0))));
      }
      else {
        lightingLevelIndex = -fieldCell->waterSurfaceDelta >> 5;
        waterColorArgb = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (lightingLevelIndex < 0) {
          if (lightingLevelIndex < -0x3f) {
            lightingLevelIndex = 0x80;
          }
          else {
            lightingLevelIndex = lightingLevelIndex + 0xc0;
          }
        }
        else {
          lightingLevelIndex = 0xbf;
        }
        mm0PackedValue1ByteLane3 = (byte)(waterColorArgb >> 0x18);
        mm0PackedValue1ByteLane2 = (byte)(waterColorArgb >> 0x10);
        mm0PackedValue1ByteLane1 = (byte)(waterColorArgb >> 8);
        mm0PackedValue1 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue1ByteLane3,mm0PackedValue1ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue1ByteLane3,
                                                                          mm0PackedValue1ByteLane3),
                                                                 mm0PackedValue1ByteLane2),
                                                        CONCAT14(mm0PackedValue1ByteLane2,waterColorArgb)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue1ByteLane1,
                                                        mm0PackedValue1ByteLane1) >> 3,
                                               CONCAT11((char)waterColorArgb,(char)waterColorArgb) >> 3))),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        shadedLane0 = (short)mm0PackedValue1;
        shadedLane1 = (short)((ulonglong)mm0PackedValue1 >> 0x10);
        shadedLane2 = (short)((ulonglong)mm0PackedValue1 >> 0x20);
        shadedLane3 = (short)((ulonglong)mm0PackedValue1 >> 0x30);
        *(uint *)planePixelCursor =
             CONCAT13((0 < shadedLane3) * (shadedLane3 < 0x100) * (char)((ulonglong)mm0PackedValue1 >> 0x30) -
                      (0xff < shadedLane3),
                      CONCAT12((0 < shadedLane2) * (shadedLane2 < 0x100) *
                               (char)((ulonglong)mm0PackedValue1 >> 0x20) - (0xff < shadedLane2),
                               CONCAT11((0 < shadedLane1) * (shadedLane1 < 0x100) *
                                        (char)((ulonglong)mm0PackedValue1 >> 0x10) - (0xff < shadedLane1),
                                        (0 < shadedLane0) * (shadedLane0 < 0x100) * (char)mm0PackedValue1 -
                                        (0xff < shadedLane0))));
      }
      fieldCell = fieldCell + 1;
      planePixelCursor = planePixelCursor + 4;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = textureWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x0053D680.
   Ownership: world/terrain/visuals.
   Purpose: Rebuilds sourceEntries[2] using secondary field flags, palette data, terrain height, and the verified
   shading paths.
*/
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane2(void)

{
  AssetDimension textureWidth;
  int panelSubresourceIndex;
  AssetFormatVersion xeniteColorArgb;
  AssetPackedDate tritiumColorArgb;
  AssetMagic soilColorArgb;
  PackedArgb32 waterColorArgb;
  undefined4 existingPixelArgb;
  short shadedLane0;
  short shadedLane1;
  short shadedLane2;
  short shadedLane3;
  GraphicsTextureSourceAsset *panelTextureSource;
  byte mm0PackedValue2ByteLane2;
  byte mm0PackedValue3ByteLane2;
  byte mm0PackedValue1ByteLane2;
  byte mm0PackedValue1ByteLane1;
  ushort blendedLane0;
  byte mm0PackedValue0ByteLane2;
  byte mm0PackedValue0ByteLane3;
  byte mm0PackedValue1ByteLane3;
  byte mm0PackedValue0ByteLane1;
  int lightingLevelIndex;
  AssetDimension columnsRemaining;
  byte *planePixelCursor;
  FieldGridCell *fieldCell;
  ushort blendedLane1;
  ushort blendedLane2;
  undefined8 mm0PackedValue0;
  byte mm0PackedValue2ByteLane1;
  byte mm0PackedValue2ByteLane3;
  byte mm0PackedValue3ByteLane1;
  byte mm0PackedValue3ByteLane3;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  undefined8 mm0PackedValue1;
  ushort alphaOrBlendedLane3;
  AssetDimension rowsRemaining;
  
  panelTextureSource = g_InGamePanelTextureSource;
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  rowsRemaining = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  planePixelCursor = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 +
            (g_TerrainCompositeTexture->sourceEntries[2].dataOffset - 0x28);
  panelSubresourceIndex = *(int *)((int)g_InGamePanelTextureSource[2].common.buildMetadata.names.sourceName +
                  (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset + 0x18);
  fieldCell = ((g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid)->cells;
  columnsRemaining = textureWidth;
  do {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
        if ((fieldCell->flagsAndMaterial & FIELD_CELL_TRITIUM_SUPPORT) == 0) {
          lightingLevelIndex = fieldCell->terrainHeight >> 7;
          soilColorArgb = panelTextureSource[panelSubresourceIndex * 4 + 2].common.magic;
          if (lightingLevelIndex < 0) {
            lightingLevelIndex = 0x70;
          }
          else if (lightingLevelIndex < 0x60) {
            lightingLevelIndex = lightingLevelIndex + 0x70;
          }
          else {
            lightingLevelIndex = 0xcf;
          }
          mm0PackedValue3ByteLane3 = (byte)(soilColorArgb >> 0x18);
          mm0PackedValue3ByteLane2 = (byte)(soilColorArgb >> 0x10);
          mm0PackedValue3ByteLane1 = (byte)(soilColorArgb >> 8);
          mm0PackedValue3 =
               pmulhw(CONCAT26(CONCAT11(mm0PackedValue3ByteLane3,mm0PackedValue3ByteLane3) >> 3,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue3ByteLane3
                                                                            ,
                                                  mm0PackedValue3ByteLane3),mm0PackedValue3ByteLane2
                                                  ),CONCAT14(mm0PackedValue3ByteLane2,soilColorArgb)) >>
                                                0x20) >> 3,
                                        CONCAT22(CONCAT11(mm0PackedValue3ByteLane1,
                                                          mm0PackedValue3ByteLane1) >> 3,
                                                 CONCAT11((char)soilColorArgb,(char)soilColorArgb) >> 3))),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          shadedLane0 = (short)mm0PackedValue3;
          shadedLane1 = (short)((ulonglong)mm0PackedValue3 >> 0x10);
          shadedLane2 = (short)((ulonglong)mm0PackedValue3 >> 0x20);
          shadedLane3 = (short)((ulonglong)mm0PackedValue3 >> 0x30);
          *(uint *)planePixelCursor =
               CONCAT13((0 < shadedLane3) * (shadedLane3 < 0x100) * (char)((ulonglong)mm0PackedValue3 >> 0x30)
                        - (0xff < shadedLane3),
                        CONCAT12((0 < shadedLane2) * (shadedLane2 < 0x100) *
                                 (char)((ulonglong)mm0PackedValue3 >> 0x20) - (0xff < shadedLane2),
                                 CONCAT11((0 < shadedLane1) * (shadedLane1 < 0x100) *
                                          (char)((ulonglong)mm0PackedValue3 >> 0x10) -
                                          (0xff < shadedLane1),
                                          (0 < shadedLane0) * (shadedLane0 < 0x100) * (char)mm0PackedValue3 -
                                          (0xff < shadedLane0))));
        }
        else {
          lightingLevelIndex = fieldCell->terrainHeight >> 7;
          tritiumColorArgb = panelTextureSource[panelSubresourceIndex * 4 + 2].common.buildMetadata.timestamps.dateValue0;
          if (lightingLevelIndex < 0) {
            lightingLevelIndex = 0x70;
          }
          else if (lightingLevelIndex < 0x60) {
            lightingLevelIndex = lightingLevelIndex + 0x70;
          }
          else {
            lightingLevelIndex = 0xcf;
          }
          mm0PackedValue2ByteLane3 = (byte)(tritiumColorArgb >> 0x18);
          mm0PackedValue2ByteLane2 = (byte)(tritiumColorArgb >> 0x10);
          mm0PackedValue2ByteLane1 = (byte)(tritiumColorArgb >> 8);
          mm0PackedValue2 =
               pmulhw(CONCAT26(CONCAT11(mm0PackedValue2ByteLane3,mm0PackedValue2ByteLane3) >> 3,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue2ByteLane3
                                                                            ,
                                                  mm0PackedValue2ByteLane3),mm0PackedValue2ByteLane2
                                                  ),CONCAT14(mm0PackedValue2ByteLane2,tritiumColorArgb)) >>
                                                0x20) >> 3,
                                        CONCAT22(CONCAT11(mm0PackedValue2ByteLane1,
                                                          mm0PackedValue2ByteLane1) >> 3,
                                                 CONCAT11((char)tritiumColorArgb,(char)tritiumColorArgb) >> 3))),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          shadedLane0 = (short)mm0PackedValue2;
          shadedLane1 = (short)((ulonglong)mm0PackedValue2 >> 0x10);
          shadedLane2 = (short)((ulonglong)mm0PackedValue2 >> 0x20);
          shadedLane3 = (short)((ulonglong)mm0PackedValue2 >> 0x30);
          *(uint *)planePixelCursor =
               CONCAT13((0 < shadedLane3) * (shadedLane3 < 0x100) * (char)((ulonglong)mm0PackedValue2 >> 0x30)
                        - (0xff < shadedLane3),
                        CONCAT12((0 < shadedLane2) * (shadedLane2 < 0x100) *
                                 (char)((ulonglong)mm0PackedValue2 >> 0x20) - (0xff < shadedLane2),
                                 CONCAT11((0 < shadedLane1) * (shadedLane1 < 0x100) *
                                          (char)((ulonglong)mm0PackedValue2 >> 0x10) -
                                          (0xff < shadedLane1),
                                          (0 < shadedLane0) * (shadedLane0 < 0x100) * (char)mm0PackedValue2 -
                                          (0xff < shadedLane0))));
        }
      }
      else {
        lightingLevelIndex = fieldCell->terrainHeight >> 7;
        xeniteColorArgb = panelTextureSource[panelSubresourceIndex * 4 + 2].common.formatVersion;
        if (lightingLevelIndex < 0) {
          lightingLevelIndex = 0x70;
        }
        else if (lightingLevelIndex < 0x60) {
          lightingLevelIndex = lightingLevelIndex + 0x70;
        }
        else {
          lightingLevelIndex = 0xcf;
        }
        mm0PackedValue0ByteLane3 = (byte)(xeniteColorArgb >> 0x18);
        mm0PackedValue0ByteLane2 = (byte)(xeniteColorArgb >> 0x10);
        mm0PackedValue0ByteLane1 = (byte)(xeniteColorArgb >> 8);
        mm0PackedValue0 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                          mm0PackedValue0ByteLane3),
                                                                 mm0PackedValue0ByteLane2),
                                                        CONCAT14(mm0PackedValue0ByteLane2,xeniteColorArgb)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                        mm0PackedValue0ByteLane1) >> 3,
                                               CONCAT11((char)xeniteColorArgb,(char)xeniteColorArgb) >> 3))),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        shadedLane0 = (short)mm0PackedValue0;
        shadedLane1 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
        shadedLane2 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
        shadedLane3 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
        *(uint *)planePixelCursor =
             CONCAT13((0 < shadedLane3) * (shadedLane3 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                      (0xff < shadedLane3),
                      CONCAT12((0 < shadedLane2) * (shadedLane2 < 0x100) *
                               (char)((ulonglong)mm0PackedValue0 >> 0x20) - (0xff < shadedLane2),
                               CONCAT11((0 < shadedLane1) * (shadedLane1 < 0x100) *
                                        (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < shadedLane1),
                                        (0 < shadedLane0) * (shadedLane0 < 0x100) * (char)mm0PackedValue0 -
                                        (0xff < shadedLane0))));
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        lightingLevelIndex = -fieldCell->waterSurfaceDelta >> 5;
        waterColorArgb = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (lightingLevelIndex < 0) {
          if (lightingLevelIndex < -0x3f) {
            lightingLevelIndex = 0x80;
          }
          else {
            lightingLevelIndex = lightingLevelIndex + 0xc0;
          }
        }
        else {
          lightingLevelIndex = 0xbf;
        }
        existingPixelArgb = *(undefined4 *)planePixelCursor;
        mm0PackedValue1ByteLane3 = (byte)(waterColorArgb >> 0x18);
        mm0PackedValue1ByteLane2 = (byte)(waterColorArgb >> 0x10);
        mm0PackedValue1ByteLane1 = (byte)(waterColorArgb >> 8);
        alphaOrBlendedLane3 = (ushort)(((ulonglong)(byte)((uint)existingPixelArgb >> 0x18) << 0x38) >> 0x30);
        mm0PackedValue1 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue1ByteLane3,mm0PackedValue1ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue1ByteLane3,
                                                                          mm0PackedValue1ByteLane3),
                                                                 mm0PackedValue1ByteLane2),
                                                        CONCAT14(mm0PackedValue1ByteLane2,waterColorArgb)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue1ByteLane1,
                                                        mm0PackedValue1ByteLane1) >> 3,
                                               CONCAT11((char)waterColorArgb,(char)waterColorArgb) >> 3))),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        blendedLane0 = (ushort)((short)mm0PackedValue1 + (ushort)(byte)existingPixelArgb) >> 1;
        blendedLane1 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x10) +
                         ((ushort)(((ulonglong)(byte)((uint)existingPixelArgb >> 8) << 0x18) >> 0x10) >> 8)) >>
                 1;
        blendedLane2 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x20) +
                         ((ushort)(((ulonglong)CONCAT21(alphaOrBlendedLane3,(char)((uint)existingPixelArgb >> 0x10)) << 0x28)
                                  >> 0x20) >> 8)) >> 1;
        alphaOrBlendedLane3 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x30) + (alphaOrBlendedLane3 >> 8)) >> 1;
        *(uint *)planePixelCursor =
             CONCAT13((alphaOrBlendedLane3 != 0) * (alphaOrBlendedLane3 < 0x100) * (char)alphaOrBlendedLane3 - (0xff < alphaOrBlendedLane3),
                      CONCAT12((blendedLane2 != 0) * (blendedLane2 < 0x100) * (char)blendedLane2 - (0xff < blendedLane2),
                               CONCAT11((blendedLane1 != 0) * (blendedLane1 < 0x100) * (char)blendedLane1 -
                                        (0xff < blendedLane1),
                                        (blendedLane0 != 0) * (blendedLane0 < 0x100) * (char)blendedLane0 -
                                        (0xff < blendedLane0))));
      }
      fieldCell = fieldCell + 1;
      planePixelCursor = planePixelCursor + 4;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = textureWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x0053D840.
   Ownership: world/terrain/visuals.
   Purpose: Copies either plane one or plane two into sourceEntries[0] according to active mode bit 1, applies per-
   cell attenuation masks, and overlays eligible active entities at their field-grid positions.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid], SelectionInfo_FindEntryCf
   [gameplay/selection/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_RebuildPlane0(void)

{
  byte visibilityFlags;
  AssetDimension textureWidth;
  AssetDimension textureHeight;
  WorldOwnerListNode100 *ownerNode;
  GameEntityRuntime *ownerEntity;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  GraphicsTextureSourceAsset *panelTextureSource;
  uint pixelArgb;
  int cellsRemainingOrRowQ12;
  int counterOrGridColumn;
  int gridRow;
  dword colorVariant;
  AssetRelativeOffset assetOffset;
  byte *pixelCursor;
  FieldGridCell *fieldCell;
  byte *plane0Pixels;
  byte *plane0WriteCursor;
  bool isSelected;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  
  inGameRoot = g_InGameRuntimeRoot;
  if ((g_InGameRuntimeRoot->observedTerrainCompositeFlags4938 & 2) == 0) {
    assetOffset = g_TerrainCompositeTexture->sourceEntries[1].dataOffset;
  }
  else {
    assetOffset = g_TerrainCompositeTexture->sourceEntries[2].dataOffset;
  }
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  textureHeight = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  plane0Pixels = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 +
            (g_TerrainCompositeTexture->sourceEntries[0].dataOffset - 0x28);
  cellsRemainingOrRowQ12 = textureWidth * textureHeight;
  pixelCursor = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 + (assetOffset - 0x28);
  plane0WriteCursor = plane0Pixels;
  for (counterOrGridColumn = cellsRemainingOrRowQ12; counterOrGridColumn != 0; counterOrGridColumn = counterOrGridColumn + -1) {
    *(uint *)plane0WriteCursor = *(uint *)pixelCursor;
    pixelCursor = pixelCursor + 4;
    plane0WriteCursor = plane0WriteCursor + 4;
  }
  counterOrGridColumn = (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  fieldCell = ((inGameRoot->worldRuntime0A30).fieldGrid)->cells;
  pixelCursor = plane0Pixels;
  do {
    visibilityFlags = fieldCell->runtime60_6B[counterOrGridColumn + 0x10];
    pixelArgb = (uint)visibilityFlags;
    if ((visibilityFlags & 0x79) == 0) {
      if ((visibilityFlags & 0xf9) != 0) {
        pixelArgb = (*(uint *)pixelCursor & 0xfefefefe) >> 1;
      }
      *(uint *)pixelCursor = pixelArgb;
    }
    fieldCell = fieldCell + 1;
    pixelCursor = pixelCursor + 4;
    cellsRemainingOrRowQ12 = cellsRemainingOrRowQ12 + -1;
  } while (cellsRemainingOrRowQ12 != 0);
  for (ownerNode = (inGameRoot->worldRuntime0A30).ownerListHead; ownerNode != (WorldOwnerListNode100 *)0x0;
      ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (0xffffff < ownerNode->modelTintArgb)) {
      gridCoordinates = FieldGrid_WorldToGridQ12(ownerNode->worldYQ12,ownerNode->worldXQ12);
      panelTextureSource = g_InGamePanelTextureSource;
      cellsRemainingOrRowQ12 = gridCoordinates.rowQ12;
      counterOrGridColumn = gridCoordinates.columnQ12 + 0x800 >> 0xc;
      if ((SCARRY4(cellsRemainingOrRowQ12,0x800) == counterOrGridColumn < 0) &&
         (((gridRow = cellsRemainingOrRowQ12 + 0x800 >> 0xc, SCARRY4(cellsRemainingOrRowQ12,0x800) == gridRow < 0 &&
           (counterOrGridColumn < (int)textureWidth)) && (gridRow < (int)textureHeight)))) {
        ownerEntity = *(GameEntityRuntime **)((int)ownerNode->runtimePayload + 8);
        colorVariant = g_GameFactionRuntimeImage.records[(ownerEntity->common).ownership.ownerIndex].
                 factionClassOrMode;
        assetOffset = (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset;
        if (colorVariant != 0) {
          isSelected = SelectionInfo_FindEntryCf(ownerEntity);
          if (!isSelected) {
            colorVariant = 0;
          }
          pixelArgb = *(uint *)(g_InGamePanelTextureSource[1].opaqueTablePayloadBC_1FF +
                           *(int *)((int)panelTextureSource[2].common.buildMetadata.names.sourceName +
                                   assetOffset + 0x18) * 0x20 + colorVariant * 8 + 0x44);
          if (ownerNode->modelTintArgb < 0xff000000) {
            pixelArgb = (pixelArgb & 0xfefefefe) +
                    (*(uint *)(plane0Pixels + (gridRow * textureWidth + counterOrGridColumn) * 4) & 0xfefefefe) >> 1;
          }
          *(uint *)(plane0Pixels + (gridRow * textureWidth + counterOrGridColumn) * 4) = pixelArgb;
        }
      }
    }
  }
  return;
}

