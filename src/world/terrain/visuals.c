/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/visuals.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/visuals.h>

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
  FieldGridAsset *pFVar1;
  AssetDimension AVar2;
  AssetDimension AVar3;
  InGameRuntimeRootImageC3E4 *pIVar4;
  AssetRelativeOffset AVar6;
  uint uVar7;
  int iVar8;
  ArenaAllocEaxCf5 AVar9;
  StatusValueEaxCf5 SVar10;
  TerrainCompositeTextureRuntime *pTVar5;
  
  pIVar4 = g_InGameRuntimeRoot;
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  AVar2 = pFVar1->gridWidth;
  AVar3 = pFVar1->gridHeight;
  AVar9 = (*g_MemoryApi.alloc)(AVar2 * 0xc * AVar3 + 0x260);
  pTVar5 = (TerrainCompositeTextureRuntime *)AVar9.eax;
  if (!AVar9.carry) {
    g_TerrainCompositeTexture = pTVar5;
    *(TerrainCompositeTextureRuntime **)(pIVar4->opaque9A74_9B4B + 8) = pTVar5;
    pTVar5->sourceEntries[0].pixelWidth = AVar2;
    pTVar5->sourceEntries[0].pixelHeight = AVar3;
    pTVar5->sourceEntries[0].originX = 0;
    pTVar5->sourceEntries[0].originY = 0;
    iVar8 = AVar2 * 4 * AVar3;
    pTVar5->sourceEntries[0].paletteIndex = -1;
    pTVar5->sourceEntries[0].logicalWidth = AVar2;
    pTVar5->sourceEntries[0].logicalHeight = AVar3;
    pTVar5->sourceEntries[0].dataOffset = 0x260;
    pTVar5->sourceEntries[1].pixelWidth = AVar2;
    pTVar5->sourceEntries[1].pixelHeight = AVar3;
    pTVar5->sourceEntries[1].originX = 0;
    pTVar5->sourceEntries[1].originY = 0;
    pTVar5->sourceEntries[1].paletteIndex = -1;
    pTVar5->sourceEntries[1].logicalWidth = AVar2;
    pTVar5->sourceEntries[1].logicalHeight = AVar3;
    pTVar5->sourceEntries[1].dataOffset = iVar8 + 0x260U;
    pTVar5->sourceEntries[2].pixelWidth = AVar2;
    pTVar5->sourceEntries[2].pixelHeight = AVar3;
    pTVar5->sourceEntries[2].originX = 0;
    pTVar5->sourceEntries[2].originY = 0;
    AVar6 = iVar8 + 0x260U + iVar8;
    pTVar5->sourceEntries[2].paletteIndex = -1;
    pTVar5->sourceEntries[2].logicalWidth = AVar2;
    pTVar5->sourceEntries[2].logicalHeight = AVar3;
    pTVar5->sourceEntries[2].dataOffset = AVar6;
    (pTVar5->textureSource).common.magic = ASSET_MAGIC_GFX;
    uVar7 = AVar6 + iVar8;
    (pTVar5->textureSource).tableDescriptor.subresourceCount = 3;
    (pTVar5->textureSource).tableDescriptor.paletteBankCount = 0;
    (pTVar5->textureSource).tableDescriptor.subresourceTableOffset = 0x200;
    (pTVar5->textureSource).opaqueTablePayloadBC_1FF[0] = 0;
    (pTVar5->textureSource).opaqueTablePayloadBC_1FF[1] = 0;
    (pTVar5->textureSource).opaqueTablePayloadBC_1FF[2] = 0;
    (pTVar5->textureSource).opaqueTablePayloadBC_1FF[3] = 0;
    (pTVar5->textureSource).common.allocationSizeBytes = uVar7;
    TerrainCompositeTexture_FillPlane1();
    TerrainCompositeTexture_FillPlane2();
    TerrainCompositeTexture_RebuildPlane0();
    AVar9.carry = false;
    AVar9.eax = uVar7;
  }
  SVar10.valueOrError = AVar9.eax;
  SVar10.carry = AVar9.carry;
  return SVar10;
}


/* Address: 0x00503B10.
   Ownership: world/terrain/visuals.
   Purpose: Allocates and 64-KiB-aligns the shared terrain byte lookup and fills its clamp/offset tables using
   delta 0x15 with verified caps 0x87 and 0xFF.
*/
bool __thandor_void_preserve_ecx_edx TerrainByteClampLookup_Initialize(void)

{
  void *lookupAllocationBase;
  int lookupRowsRemaining;
  int iVar1;
  byte bVar2;
  uint clampInputValue;
  uint uVar3;
  byte *lookupWriteCursor;
  ArenaAllocEaxCf5 AVar4;
  
  AVar4 = (*g_MemoryApi.alloc)(0x20000);
  lookupAllocationBase = (void *)AVar4.eax;
  if (AVar4.carry) {
    return true;
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
      bVar2 = (char)clampInputValue + 1;
      clampInputValue = (uint)bVar2;
    } while (bVar2 != 0);
    uVar3 = 0;
    do {
      if (uVar3 < 0x100) {
        if (uVar3 + 0x15 < 0xff) {
          *lookupWriteCursor = (byte)(uVar3 + 0x15);
        }
        else {
          *lookupWriteCursor = 0xff;
        }
      }
      else if ((int)(uVar3 - 0x15) < 0x100) {
        *lookupWriteCursor = 0xff;
      }
      else {
        *lookupWriteCursor = (byte)(uVar3 - 0x15);
      }
      lookupWriteCursor = lookupWriteCursor + 1;
      bVar2 = (char)uVar3 + 1;
      uVar3 = (uint)bVar2;
    } while (bVar2 != 0);
    lookupRowsRemaining = lookupRowsRemaining + -1;
    clampInputValue = 0;
  } while (lookupRowsRemaining != 0);
  uVar3 = 0;
  do {
    if (uVar3 < 0x88) {
      if (uVar3 + 0x15 < 0x87) {
        *lookupWriteCursor = (byte)(uVar3 + 0x15);
      }
      else {
        *lookupWriteCursor = 0x87;
      }
    }
    else if ((int)(uVar3 - 0x15) < 0x88) {
      *lookupWriteCursor = 0x87;
    }
    else {
      *lookupWriteCursor = (byte)(uVar3 - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    bVar2 = (char)uVar3 + 1;
    uVar3 = (uint)bVar2;
  } while (bVar2 != 0);
  uVar3 = 0;
  do {
    if (uVar3 < 0x100) {
      if (uVar3 + 0x15 < 0xff) {
        *lookupWriteCursor = (byte)(uVar3 + 0x15);
      }
      else {
        *lookupWriteCursor = 0xff;
      }
    }
    else if ((int)(uVar3 - 0x15) < 0x100) {
      *lookupWriteCursor = 0xff;
    }
    else {
      *lookupWriteCursor = (byte)(uVar3 - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    bVar2 = (char)uVar3 + 1;
    uVar3 = (uint)bVar2;
  } while (bVar2 != 0);
  uVar3 = 0;
  do {
    if (uVar3 < 0x88) {
      if (uVar3 + 0x15 < 0x87) {
        *lookupWriteCursor = (byte)(uVar3 + 0x15);
      }
      else {
        *lookupWriteCursor = 0x87;
      }
    }
    else if ((int)(uVar3 - 0x15) < 0x88) {
      *lookupWriteCursor = 0x87;
    }
    else {
      *lookupWriteCursor = (byte)(uVar3 - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    bVar2 = (char)uVar3 + 1;
    uVar3 = (uint)bVar2;
  } while (bVar2 != 0);
  iVar1 = 0x7d;
  uVar3 = 0;
  do {
    if (uVar3 < 0x100) {
      if (uVar3 + 0x15 < 0xff) {
        *lookupWriteCursor = (byte)(uVar3 + 0x15);
      }
      else {
        *lookupWriteCursor = 0xff;
      }
    }
    else if ((int)(uVar3 - 0x15) < 0x100) {
      *lookupWriteCursor = 0xff;
    }
    else {
      *lookupWriteCursor = (byte)(uVar3 - 0x15);
    }
    lookupWriteCursor = lookupWriteCursor + 1;
    bVar2 = (char)uVar3 + 1;
    uVar3 = (uint)bVar2;
  } while ((bVar2 != 0) || (iVar1 = iVar1 + -1, iVar1 != 0));
  return false;
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
  GraphicsPaletteAsset *pGVar1;
  GraphicsTextureSet *pGVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  word wVar6;
  uint uVar7;
  TerrainMaterialSuffixEntry *pTVar8;
  word *pwVar9;
  word *pwVar10;
  GraphicsTextureSet **ppGVar12;
  TerrainDirectionRecord *pTVar13;
  GraphicsTextureSetEaxCf5 GVar14;
  PackageLoadEntryEaxCf5 PVar15;
  GraphicsPaletteAssetEaxCf5 GVar16;
  StatusValueEaxCf5 SVar17;
  StatusValueEaxCf5 SVar18;
  TerrainMaterialSuffixEntry *pTVar11;
  
  iVar4 = 0x100;
  pwVar9 = secondaryResourcePath;
  do {
    pwVar10 = pwVar9;
    if (iVar4 == 0) break;
    iVar4 = iVar4 + -1;
    pwVar10 = pwVar9 + 1;
    wVar6 = *pwVar9;
    pwVar9 = pwVar10;
  } while (wVar6 != 0);
  pTVar11 = (TerrainMaterialSuffixEntry *)(pwVar10 + -1);
  pGVar1 = (GraphicsPaletteAsset *)0x38;
  if (((field->common).magic == ASSET_MAGIC_FLD) &&
     ((field->common).converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    uVar7 = field->fieldFlags;
    iVar4 = 0;
    do {
      if ((uVar7 & 1) != 0) {
        iVar4 = iVar4 + 1;
      }
      uVar7 = uVar7 >> 1;
    } while (uVar7 != 0);
    iVar5 = 0x1a;
    uVar7 = field->fieldFlags;
    g_MoviePlaybackScheduleSpan = iVar4 * 2 + 10;
    pTVar8 = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ;
    ppGVar12 = g_TerrainMaterialTextureSets;
    do {
      if ((uVar7 & 1) == 0) {
        *pTVar11 = *pTVar8;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        GVar14 = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        pGVar2 = GVar14.textureSet;
        if (GVar14.carry) {
          pGVar2 = (GraphicsTextureSet *)0x0;
        }
        *ppGVar12 = pGVar2;
      }
      else {
        *pTVar11 = *pTVar8;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        GVar14 = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        pGVar1 = (GraphicsPaletteAsset *)GVar14.textureSet;
        if (GVar14.carry)
        goto TerrainVisualResources_LoadPrimary_ReturnFieldOrResourceLoadFailureStatus;
        MoviePlayback_AdvanceScheduledFrameAndTick();
        *ppGVar12 = (GraphicsTextureSet *)pGVar1;
      }
      pTVar8 = pTVar8 + 1;
      ppGVar12 = ppGVar12 + 1;
      uVar7 = uVar7 >> 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    WidePath_SetExtensionCode(0x746164,primaryResourcePath);
    PVar15 = Package_LoadEntry(primaryResourcePath);
    pGVar1 = PVar15.bufferOrError;
    if (!PVar15.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainSurfacePacketTablePayload =
           &((GraphicsTextureSetEntry *)pGVar1->reserved08_AF)->reserved18;
      WidePath_SetExtensionCode(0x786667,primaryResourcePath);
      GVar14 = (*g_GraphicsTextureSetLoadPackageCf)(primaryResourcePath);
      pGVar1 = (GraphicsPaletteAsset *)GVar14.textureSet;
      if (!GVar14.carry) {
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)pGVar1;
        WidePath_SetExtensionCode(0x6c6170,primaryResourcePath);
        GVar16 = (*g_GraphicsPaletteAssetLoadPackage)(primaryResourcePath);
        pGVar1 = GVar16.paletteAsset;
        if (!GVar16.carry) {
          MoviePlayback_AdvanceScheduledFrameAndTick();
          g_TerrainPrimaryPalette = pGVar1;
          pTVar11->lowercaseLetterUtf16 = 0;
          pTVar11->terminator = 0;
          WidePath_SetExtensionCode(0x6c6170,secondaryResourcePath);
          GVar16 = (*g_GraphicsPaletteAssetLoadPackage)(secondaryResourcePath);
          pGVar1 = GVar16.paletteAsset;
          if (!GVar16.carry) {
            MoviePlayback_AdvanceScheduledFrameAndTick();
            g_TerrainSecondaryPalette = pGVar1;
            pTVar11->lowercaseLetterUtf16 = 0;
            pTVar11->terminator = 0;
            WidePath_SetExtensionCode(0x746164,secondaryResourcePath);
            PVar15 = Package_LoadEntry(secondaryResourcePath);
            pGVar1 = PVar15.bufferOrError;
            if (!PVar15.carry) {
              MoviePlayback_AdvanceScheduledFrameAndTick();
              g_TerrainSoilPacketTablePayload =
                   &((GraphicsTextureSetEntry *)pGVar1->reserved08_AF)->reserved18;
              FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(field);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              iVar4 = 0x100;
              pTVar13 = g_TerrainDirectionRecordTable256;
              do {
                dVar3 = Random_NextPrimary();
                pTVar13->scaleA = (dVar3 & 0x1f) + 0x80;
                wVar6 = ((ushort)(dVar3 >> 0x10) & 0x7f) + 0x200;
                dVar3 = Random_NextPrimary();
                if ((int)dVar3 < 0) {
                  wVar6 = -wVar6;
                }
                pTVar13->rateA = wVar6;
                *(short *)&pTVar13->packedAngleA_low16_AngleB_high16 = (short)dVar3;
                dVar3 = Random_NextPrimary();
                pTVar13->scaleB = (dVar3 & 0x1f) + 0x80;
                wVar6 = ((ushort)(dVar3 >> 0x10) & 0x7f) + 0x200;
                SVar17.valueOrError = Random_NextPrimary();
                if ((int)SVar17.valueOrError < 0) {
                  wVar6 = -wVar6;
                }
                pTVar13->rateB = wVar6;
                *(short *)((int)&pTVar13->packedAngleA_low16_AngleB_high16 + 2) =
                     (short)SVar17.valueOrError;
                pTVar13->angleAComponent0ScaledQ28 = 0;
                pTVar13->angleAComponent1ScaledQ28 = 0;
                pTVar13->angleBComponent0ScaledQ28 = 0;
                pTVar13 = pTVar13 + 1;
                iVar4 = iVar4 + -1;
              } while (iVar4 != 0);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              TerrainDirectionTable_AdvanceAndRebuildVectors();
              SVar17.carry = false;
              return SVar17;
            }
          }
        }
      }
    }
  }
TerrainVisualResources_LoadPrimary_ReturnFieldOrResourceLoadFailureStatus:
  SVar18.carry = true;
  SVar18.valueOrError = (dword)pGVar1;
  return SVar18;
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
  GraphicsPaletteAsset *pGVar1;
  GraphicsTextureSet *pGVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  word wVar6;
  uint uVar7;
  TerrainMaterialSuffixEntry *pTVar8;
  FieldGridCell *pFVar9;
  word *pwVar10;
  word *pwVar11;
  GraphicsTextureSet **ppGVar13;
  TerrainDirectionRecord *pTVar14;
  GraphicsTextureSetEaxCf5 GVar15;
  PackageLoadEntryEaxCf5 PVar16;
  GraphicsPaletteAssetEaxCf5 GVar17;
  StatusValueEaxCf5 SVar18;
  StatusValueEaxCf5 SVar19;
  TerrainMaterialSuffixEntry *pTVar12;
  
  iVar4 = 0x100;
  pwVar10 = secondaryResourcePath;
  do {
    pwVar11 = pwVar10;
    if (iVar4 == 0) break;
    iVar4 = iVar4 + -1;
    pwVar11 = pwVar10 + 1;
    wVar6 = *pwVar10;
    pwVar10 = pwVar11;
  } while (wVar6 != 0);
  pTVar12 = (TerrainMaterialSuffixEntry *)(pwVar11 + -1);
  pGVar1 = (GraphicsPaletteAsset *)0x38;
  if (((field->common).magic == ASSET_MAGIC_FLD) &&
     ((field->common).converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    uVar7 = field->fieldFlags;
    iVar4 = 0;
    do {
      if ((uVar7 & 1) != 0) {
        iVar4 = iVar4 + 1;
      }
      uVar7 = uVar7 >> 1;
    } while (uVar7 != 0);
    iVar5 = 0x1a;
    uVar7 = field->fieldFlags;
    g_MoviePlaybackScheduleSpan = iVar4 * 2 + 10;
    pTVar8 = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ;
    ppGVar13 = g_TerrainMaterialTextureSets;
    do {
      if ((uVar7 & 1) == 0) {
        *pTVar12 = *pTVar8;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        GVar15 = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        pGVar2 = GVar15.textureSet;
        if (GVar15.carry) {
          pGVar2 = (GraphicsTextureSet *)0x0;
        }
        *ppGVar13 = pGVar2;
      }
      else {
        *pTVar12 = *pTVar8;
        WidePath_SetExtensionCode(0x786667,secondaryResourcePath);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        GVar15 = (*g_GraphicsTextureSetLoadPackageCf)(secondaryResourcePath);
        pGVar1 = (GraphicsPaletteAsset *)GVar15.textureSet;
        if (GVar15.carry)
        goto 
        TerrainVisualResources_LoadAndClearCellOverlayFlags_ReturnFieldOrResourceLoadFailureStatus;
        MoviePlayback_AdvanceScheduledFrameAndTick();
        *ppGVar13 = (GraphicsTextureSet *)pGVar1;
      }
      pTVar8 = pTVar8 + 1;
      ppGVar13 = ppGVar13 + 1;
      uVar7 = uVar7 >> 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    WidePath_SetExtensionCode(0x746164,primaryResourcePath);
    PVar16 = Package_LoadEntry(primaryResourcePath);
    pGVar1 = PVar16.bufferOrError;
    if (!PVar16.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainSurfacePacketTablePayload =
           &((GraphicsTextureSetEntry *)pGVar1->reserved08_AF)->reserved18;
      WidePath_SetExtensionCode(0x786667,primaryResourcePath);
      GVar15 = (*g_GraphicsTextureSetLoadPackageCf)(primaryResourcePath);
      pGVar1 = (GraphicsPaletteAsset *)GVar15.textureSet;
      if (!GVar15.carry) {
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)pGVar1;
        WidePath_SetExtensionCode(0x6c6170,primaryResourcePath);
        GVar17 = (*g_GraphicsPaletteAssetLoadPackage)(primaryResourcePath);
        pGVar1 = GVar17.paletteAsset;
        if (!GVar17.carry) {
          MoviePlayback_AdvanceScheduledFrameAndTick();
          g_TerrainPrimaryPalette = pGVar1;
          pTVar12->lowercaseLetterUtf16 = 0;
          pTVar12->terminator = 0;
          WidePath_SetExtensionCode(0x6c6170,secondaryResourcePath);
          GVar17 = (*g_GraphicsPaletteAssetLoadPackage)(secondaryResourcePath);
          pGVar1 = GVar17.paletteAsset;
          if (!GVar17.carry) {
            MoviePlayback_AdvanceScheduledFrameAndTick();
            g_TerrainSecondaryPalette = pGVar1;
            pTVar12->lowercaseLetterUtf16 = 0;
            pTVar12->terminator = 0;
            WidePath_SetExtensionCode(0x746164,secondaryResourcePath);
            PVar16 = Package_LoadEntry(secondaryResourcePath);
            pGVar1 = PVar16.bufferOrError;
            if (!PVar16.carry) {
              MoviePlayback_AdvanceScheduledFrameAndTick();
              g_TerrainSoilPacketTablePayload =
                   &((GraphicsTextureSetEntry *)pGVar1->reserved08_AF)->reserved18;
              FieldGrid_RebuildCellLookupPointers(field);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              iVar4 = 0x100;
              pTVar14 = g_TerrainDirectionRecordTable256;
              do {
                dVar3 = Random_NextPrimary();
                pTVar14->scaleA = (dVar3 & 0x1f) + 0x80;
                wVar6 = ((ushort)(dVar3 >> 0x10) & 0x7f) + 0x200;
                dVar3 = Random_NextPrimary();
                if ((int)dVar3 < 0) {
                  wVar6 = -wVar6;
                }
                pTVar14->rateA = wVar6;
                *(short *)&pTVar14->packedAngleA_low16_AngleB_high16 = (short)dVar3;
                dVar3 = Random_NextPrimary();
                pTVar14->scaleB = (dVar3 & 0x1f) + 0x80;
                wVar6 = ((ushort)(dVar3 >> 0x10) & 0x7f) + 0x200;
                SVar18.valueOrError = Random_NextPrimary();
                if ((int)SVar18.valueOrError < 0) {
                  wVar6 = -wVar6;
                }
                pTVar14->rateB = wVar6;
                *(short *)((int)&pTVar14->packedAngleA_low16_AngleB_high16 + 2) =
                     (short)SVar18.valueOrError;
                pTVar14->angleAComponent0ScaledQ28 = 0;
                pTVar14->angleAComponent1ScaledQ28 = 0;
                pTVar14->angleBComponent0ScaledQ28 = 0;
                pTVar14 = pTVar14 + 1;
                iVar4 = iVar4 + -1;
              } while (iVar4 != 0);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              iVar4 = field->gridWidth * field->gridHeight;
              pFVar9 = field->cells;
              do {
                pFVar9->flagsAndMaterial =
                     pFVar9->flagsAndMaterial &
                     ~FIELD_CELL_TERRAIN_VISUAL_CLEARABLE_UNRESOLVED_BIT28;
                pFVar9 = pFVar9 + 1;
                iVar4 = iVar4 + -1;
              } while (iVar4 != 0);
              TerrainDirectionTable_AdvanceAndRebuildVectors();
              SVar18.carry = false;
              return SVar18;
            }
          }
        }
      }
    }
  }
TerrainVisualResources_LoadAndClearCellOverlayFlags_ReturnFieldOrResourceLoadFailureStatus:
  SVar19.carry = true;
  SVar19.valueOrError = (dword)pGVar1;
  return SVar19;
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
  uint uVar1;
  int iVar2;
  uint *puVar3;
  PackedArgb32 *pPVar4;
  
  puVar3 = &g_TerrainLightingColorRampArgb256;
  iVar2 = 0x100;
  do {
    uVar1 = ((rampStepColorArgb & 0xff) * iVar2 >> 8) + (baseColorArgb & 0xff);
    if (0xff < uVar1) {
      uVar1 = 0xff;
    }
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  puVar3 = &g_TerrainLightingColorRampArgb256;
  iVar2 = 0x100;
  do {
    uVar1 = ((rampStepColorArgb & 0xff00) * iVar2 >> 8) + (baseColorArgb & 0xff00);
    if (0xffff < uVar1) {
      uVar1 = 0xff00;
    }
    *puVar3 = *puVar3 | uVar1 & 0xff00;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  puVar3 = &g_TerrainLightingColorRampArgb256;
  iVar2 = 0x100;
  do {
    uVar1 = ((rampStepColorArgb & 0xff0000) * iVar2 >> 8) + (baseColorArgb & 0xff0000);
    if (0xffffff < uVar1) {
      uVar1 = 0xff0000;
    }
    *puVar3 = *puVar3 | uVar1 & 0xff0000;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  puVar3 = &g_TerrainLightingColorRampArgb256;
  iVar2 = 0x100;
  do {
    *puVar3 = *puVar3 | baseColorArgb & 0xff000000;
    puVar3 = puVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  g_TerrainDirectionalLightSecondaryColor = secondaryColorArgb;
  pPVar4 = &g_TerrainDirectionalLightColorLut;
  for (iVar2 = 0x101; iVar2 != 0; iVar2 = iVar2 + -1) {
    *pPVar4 = baseColorArgb;
    pPVar4 = pPVar4 + 1;
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
  AssetDimension AVar1;
  int iVar2;
  undefined4 uVar3;
  PackedArgb32 PVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  GraphicsTextureSourceAsset *pGVar9;
  byte mm0PackedValue1ByteLane2;
  byte mm0PackedValue0ByteLane2;
  byte mm0PackedValue0ByteLane3;
  byte mm0PackedValue0ByteLane1;
  int iVar10;
  AssetDimension AVar11;
  byte *pbVar12;
  FieldGridCell *pFVar13;
  undefined8 mm0PackedValue0;
  byte mm0PackedValue1ByteLane1;
  byte mm0PackedValue1ByteLane3;
  undefined8 mm0PackedValue1;
  AssetDimension local_1c;
  
  pGVar9 = g_InGamePanelTextureSource;
  AVar1 = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  local_1c = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  pbVar12 = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 +
            (g_TerrainCompositeTexture->sourceEntries[1].dataOffset - 0x28);
  iVar2 = *(int *)((int)g_InGamePanelTextureSource[2].common.buildMetadata.names.sourceName +
                  (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset + 0x18);
  pFVar13 = ((g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid)->cells;
  AVar11 = AVar1;
  do {
    do {
      if (pFVar13->waterSurfaceDelta < 1) {
        iVar10 = pFVar13->terrainHeight >> 7;
        uVar3 = *(undefined4 *)
                 (pGVar9[iVar2 * 4 + 1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 (pFVar13->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK) * 8 + -0x28);
        if (iVar10 < 0) {
          iVar10 = 0x70;
        }
        else if (iVar10 < 0x60) {
          iVar10 = iVar10 + 0x70;
        }
        else {
          iVar10 = 0xcf;
        }
        mm0PackedValue0ByteLane3 = (byte)((uint)uVar3 >> 0x18);
        mm0PackedValue0ByteLane2 = (byte)((uint)uVar3 >> 0x10);
        mm0PackedValue0ByteLane1 = (byte)((uint)uVar3 >> 8);
        mm0PackedValue0 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                          mm0PackedValue0ByteLane3),
                                                                 mm0PackedValue0ByteLane2),
                                                        CONCAT14(mm0PackedValue0ByteLane2,uVar3)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                        mm0PackedValue0ByteLane1) >> 3,
                                               CONCAT11((char)uVar3,(char)uVar3) >> 3))),
                    *(undefined8 *)(&g_PackedLightingLookupTable + iVar10 * 8));
        sVar5 = (short)mm0PackedValue0;
        sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
        sVar7 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
        sVar8 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
        *(uint *)pbVar12 =
             CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                      (0xff < sVar8),
                      CONCAT12((0 < sVar7) * (sVar7 < 0x100) *
                               (char)((ulonglong)mm0PackedValue0 >> 0x20) - (0xff < sVar7),
                               CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                        (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar6),
                                        (0 < sVar5) * (sVar5 < 0x100) * (char)mm0PackedValue0 -
                                        (0xff < sVar5))));
      }
      else {
        iVar10 = -pFVar13->waterSurfaceDelta >> 5;
        PVar4 = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (iVar10 < 0) {
          if (iVar10 < -0x3f) {
            iVar10 = 0x80;
          }
          else {
            iVar10 = iVar10 + 0xc0;
          }
        }
        else {
          iVar10 = 0xbf;
        }
        mm0PackedValue1ByteLane3 = (byte)(PVar4 >> 0x18);
        mm0PackedValue1ByteLane2 = (byte)(PVar4 >> 0x10);
        mm0PackedValue1ByteLane1 = (byte)(PVar4 >> 8);
        mm0PackedValue1 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue1ByteLane3,mm0PackedValue1ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue1ByteLane3,
                                                                          mm0PackedValue1ByteLane3),
                                                                 mm0PackedValue1ByteLane2),
                                                        CONCAT14(mm0PackedValue1ByteLane2,PVar4)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue1ByteLane1,
                                                        mm0PackedValue1ByteLane1) >> 3,
                                               CONCAT11((char)PVar4,(char)PVar4) >> 3))),
                    *(undefined8 *)(&g_PackedLightingLookupTable + iVar10 * 8));
        sVar5 = (short)mm0PackedValue1;
        sVar6 = (short)((ulonglong)mm0PackedValue1 >> 0x10);
        sVar7 = (short)((ulonglong)mm0PackedValue1 >> 0x20);
        sVar8 = (short)((ulonglong)mm0PackedValue1 >> 0x30);
        *(uint *)pbVar12 =
             CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)mm0PackedValue1 >> 0x30) -
                      (0xff < sVar8),
                      CONCAT12((0 < sVar7) * (sVar7 < 0x100) *
                               (char)((ulonglong)mm0PackedValue1 >> 0x20) - (0xff < sVar7),
                               CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                        (char)((ulonglong)mm0PackedValue1 >> 0x10) - (0xff < sVar6),
                                        (0 < sVar5) * (sVar5 < 0x100) * (char)mm0PackedValue1 -
                                        (0xff < sVar5))));
      }
      pFVar13 = pFVar13 + 1;
      pbVar12 = pbVar12 + 4;
      AVar11 = AVar11 - 1;
    } while (AVar11 != 0);
    local_1c = local_1c - 1;
    AVar11 = AVar1;
  } while (local_1c != 0);
  return;
}


/* Address: 0x0053D680.
   Ownership: world/terrain/visuals.
   Purpose: Rebuilds sourceEntries[2] using secondary field flags, palette data, terrain height, and the verified
   shading paths.
*/
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane2(void)

{
  AssetDimension AVar1;
  int iVar2;
  AssetFormatVersion AVar3;
  AssetPackedDate AVar4;
  AssetMagic AVar5;
  PackedArgb32 PVar6;
  undefined4 uVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  short sVar11;
  GraphicsTextureSourceAsset *pGVar12;
  byte mm0PackedValue2ByteLane2;
  byte mm0PackedValue3ByteLane2;
  byte mm0PackedValue1ByteLane2;
  byte mm0PackedValue1ByteLane1;
  ushort uVar17;
  byte mm0PackedValue0ByteLane2;
  byte mm0PackedValue0ByteLane3;
  byte mm0PackedValue1ByteLane3;
  byte mm0PackedValue0ByteLane1;
  int iVar13;
  AssetDimension AVar14;
  byte *pbVar15;
  FieldGridCell *pFVar16;
  ushort uVar18;
  ushort uVar19;
  undefined8 mm0PackedValue0;
  byte mm0PackedValue2ByteLane1;
  byte mm0PackedValue2ByteLane3;
  byte mm0PackedValue3ByteLane1;
  byte mm0PackedValue3ByteLane3;
  undefined8 mm0PackedValue2;
  undefined8 mm0PackedValue3;
  undefined8 mm0PackedValue1;
  ushort uVar20;
  AssetDimension local_1c;
  
  pGVar12 = g_InGamePanelTextureSource;
  AVar1 = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  local_1c = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  pbVar15 = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 +
            (g_TerrainCompositeTexture->sourceEntries[2].dataOffset - 0x28);
  iVar2 = *(int *)((int)g_InGamePanelTextureSource[2].common.buildMetadata.names.sourceName +
                  (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset + 0x18);
  pFVar16 = ((g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid)->cells;
  AVar14 = AVar1;
  do {
    do {
      if ((pFVar16->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
        if ((pFVar16->flagsAndMaterial & FIELD_CELL_TRITIUM_SUPPORT) == 0) {
          iVar13 = pFVar16->terrainHeight >> 7;
          AVar5 = pGVar12[iVar2 * 4 + 2].common.magic;
          if (iVar13 < 0) {
            iVar13 = 0x70;
          }
          else if (iVar13 < 0x60) {
            iVar13 = iVar13 + 0x70;
          }
          else {
            iVar13 = 0xcf;
          }
          mm0PackedValue3ByteLane3 = (byte)(AVar5 >> 0x18);
          mm0PackedValue3ByteLane2 = (byte)(AVar5 >> 0x10);
          mm0PackedValue3ByteLane1 = (byte)(AVar5 >> 8);
          mm0PackedValue3 =
               pmulhw(CONCAT26(CONCAT11(mm0PackedValue3ByteLane3,mm0PackedValue3ByteLane3) >> 3,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue3ByteLane3
                                                                            ,
                                                  mm0PackedValue3ByteLane3),mm0PackedValue3ByteLane2
                                                  ),CONCAT14(mm0PackedValue3ByteLane2,AVar5)) >>
                                                0x20) >> 3,
                                        CONCAT22(CONCAT11(mm0PackedValue3ByteLane1,
                                                          mm0PackedValue3ByteLane1) >> 3,
                                                 CONCAT11((char)AVar5,(char)AVar5) >> 3))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar13 * 8));
          sVar8 = (short)mm0PackedValue3;
          sVar9 = (short)((ulonglong)mm0PackedValue3 >> 0x10);
          sVar10 = (short)((ulonglong)mm0PackedValue3 >> 0x20);
          sVar11 = (short)((ulonglong)mm0PackedValue3 >> 0x30);
          *(uint *)pbVar15 =
               CONCAT13((0 < sVar11) * (sVar11 < 0x100) * (char)((ulonglong)mm0PackedValue3 >> 0x30)
                        - (0xff < sVar11),
                        CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                 (char)((ulonglong)mm0PackedValue3 >> 0x20) - (0xff < sVar10),
                                 CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                          (char)((ulonglong)mm0PackedValue3 >> 0x10) -
                                          (0xff < sVar9),
                                          (0 < sVar8) * (sVar8 < 0x100) * (char)mm0PackedValue3 -
                                          (0xff < sVar8))));
        }
        else {
          iVar13 = pFVar16->terrainHeight >> 7;
          AVar4 = pGVar12[iVar2 * 4 + 2].common.buildMetadata.timestamps.dateValue0;
          if (iVar13 < 0) {
            iVar13 = 0x70;
          }
          else if (iVar13 < 0x60) {
            iVar13 = iVar13 + 0x70;
          }
          else {
            iVar13 = 0xcf;
          }
          mm0PackedValue2ByteLane3 = (byte)(AVar4 >> 0x18);
          mm0PackedValue2ByteLane2 = (byte)(AVar4 >> 0x10);
          mm0PackedValue2ByteLane1 = (byte)(AVar4 >> 8);
          mm0PackedValue2 =
               pmulhw(CONCAT26(CONCAT11(mm0PackedValue2ByteLane3,mm0PackedValue2ByteLane3) >> 3,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue2ByteLane3
                                                                            ,
                                                  mm0PackedValue2ByteLane3),mm0PackedValue2ByteLane2
                                                  ),CONCAT14(mm0PackedValue2ByteLane2,AVar4)) >>
                                                0x20) >> 3,
                                        CONCAT22(CONCAT11(mm0PackedValue2ByteLane1,
                                                          mm0PackedValue2ByteLane1) >> 3,
                                                 CONCAT11((char)AVar4,(char)AVar4) >> 3))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar13 * 8));
          sVar8 = (short)mm0PackedValue2;
          sVar9 = (short)((ulonglong)mm0PackedValue2 >> 0x10);
          sVar10 = (short)((ulonglong)mm0PackedValue2 >> 0x20);
          sVar11 = (short)((ulonglong)mm0PackedValue2 >> 0x30);
          *(uint *)pbVar15 =
               CONCAT13((0 < sVar11) * (sVar11 < 0x100) * (char)((ulonglong)mm0PackedValue2 >> 0x30)
                        - (0xff < sVar11),
                        CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                 (char)((ulonglong)mm0PackedValue2 >> 0x20) - (0xff < sVar10),
                                 CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                          (char)((ulonglong)mm0PackedValue2 >> 0x10) -
                                          (0xff < sVar9),
                                          (0 < sVar8) * (sVar8 < 0x100) * (char)mm0PackedValue2 -
                                          (0xff < sVar8))));
        }
      }
      else {
        iVar13 = pFVar16->terrainHeight >> 7;
        AVar3 = pGVar12[iVar2 * 4 + 2].common.formatVersion;
        if (iVar13 < 0) {
          iVar13 = 0x70;
        }
        else if (iVar13 < 0x60) {
          iVar13 = iVar13 + 0x70;
        }
        else {
          iVar13 = 0xcf;
        }
        mm0PackedValue0ByteLane3 = (byte)(AVar3 >> 0x18);
        mm0PackedValue0ByteLane2 = (byte)(AVar3 >> 0x10);
        mm0PackedValue0ByteLane1 = (byte)(AVar3 >> 8);
        mm0PackedValue0 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                          mm0PackedValue0ByteLane3),
                                                                 mm0PackedValue0ByteLane2),
                                                        CONCAT14(mm0PackedValue0ByteLane2,AVar3)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                        mm0PackedValue0ByteLane1) >> 3,
                                               CONCAT11((char)AVar3,(char)AVar3) >> 3))),
                    *(undefined8 *)(&g_PackedLightingLookupTable + iVar13 * 8));
        sVar8 = (short)mm0PackedValue0;
        sVar9 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
        sVar10 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
        sVar11 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
        *(uint *)pbVar15 =
             CONCAT13((0 < sVar11) * (sVar11 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                      (0xff < sVar11),
                      CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                               (char)((ulonglong)mm0PackedValue0 >> 0x20) - (0xff < sVar10),
                               CONCAT11((0 < sVar9) * (sVar9 < 0x100) *
                                        (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar9),
                                        (0 < sVar8) * (sVar8 < 0x100) * (char)mm0PackedValue0 -
                                        (0xff < sVar8))));
      }
      if (0 < pFVar16->waterSurfaceDelta) {
        iVar13 = -pFVar16->waterSurfaceDelta >> 5;
        PVar6 = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (iVar13 < 0) {
          if (iVar13 < -0x3f) {
            iVar13 = 0x80;
          }
          else {
            iVar13 = iVar13 + 0xc0;
          }
        }
        else {
          iVar13 = 0xbf;
        }
        uVar7 = *(undefined4 *)pbVar15;
        mm0PackedValue1ByteLane3 = (byte)(PVar6 >> 0x18);
        mm0PackedValue1ByteLane2 = (byte)(PVar6 >> 0x10);
        mm0PackedValue1ByteLane1 = (byte)(PVar6 >> 8);
        uVar20 = (ushort)(((ulonglong)(byte)((uint)uVar7 >> 0x18) << 0x38) >> 0x30);
        mm0PackedValue1 =
             pmulhw(CONCAT26(CONCAT11(mm0PackedValue1ByteLane3,mm0PackedValue1ByteLane3) >> 3,
                             CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue1ByteLane3,
                                                                          mm0PackedValue1ByteLane3),
                                                                 mm0PackedValue1ByteLane2),
                                                        CONCAT14(mm0PackedValue1ByteLane2,PVar6)) >>
                                              0x20) >> 3,
                                      CONCAT22(CONCAT11(mm0PackedValue1ByteLane1,
                                                        mm0PackedValue1ByteLane1) >> 3,
                                               CONCAT11((char)PVar6,(char)PVar6) >> 3))),
                    *(undefined8 *)(&g_PackedLightingLookupTable + iVar13 * 8));
        uVar17 = (ushort)((short)mm0PackedValue1 + (ushort)(byte)uVar7) >> 1;
        uVar18 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x10) +
                         ((ushort)(((ulonglong)(byte)((uint)uVar7 >> 8) << 0x18) >> 0x10) >> 8)) >>
                 1;
        uVar19 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x20) +
                         ((ushort)(((ulonglong)CONCAT21(uVar20,(char)((uint)uVar7 >> 0x10)) << 0x28)
                                  >> 0x20) >> 8)) >> 1;
        uVar20 = (ushort)((short)((ulonglong)mm0PackedValue1 >> 0x30) + (uVar20 >> 8)) >> 1;
        *(uint *)pbVar15 =
             CONCAT13((uVar20 != 0) * (uVar20 < 0x100) * (char)uVar20 - (0xff < uVar20),
                      CONCAT12((uVar19 != 0) * (uVar19 < 0x100) * (char)uVar19 - (0xff < uVar19),
                               CONCAT11((uVar18 != 0) * (uVar18 < 0x100) * (char)uVar18 -
                                        (0xff < uVar18),
                                        (uVar17 != 0) * (uVar17 < 0x100) * (char)uVar17 -
                                        (0xff < uVar17))));
      }
      pFVar16 = pFVar16 + 1;
      pbVar15 = pbVar15 + 4;
      AVar14 = AVar14 - 1;
    } while (AVar14 != 0);
    local_1c = local_1c - 1;
    AVar14 = AVar1;
  } while (local_1c != 0);
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
  byte bVar1;
  AssetDimension AVar2;
  AssetDimension AVar3;
  WorldOwnerListNode100 *pWVar4;
  GameEntityRuntime *entry;
  InGameRuntimeRootImageC3E4 *pIVar5;
  GraphicsTextureSourceAsset *pGVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  dword dVar11;
  AssetRelativeOffset AVar12;
  byte *pbVar13;
  FieldGridCell *pFVar14;
  byte *pbVar15;
  byte *pbVar16;
  bool bVar17;
  FieldGridCoordinatesEaxEdx8 FVar18;
  
  pIVar5 = g_InGameRuntimeRoot;
  if ((g_InGameRuntimeRoot->observedTerrainCompositeFlags4938 & 2) == 0) {
    AVar12 = g_TerrainCompositeTexture->sourceEntries[1].dataOffset;
  }
  else {
    AVar12 = g_TerrainCompositeTexture->sourceEntries[2].dataOffset;
  }
  AVar2 = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  AVar3 = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  pbVar15 = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 +
            (g_TerrainCompositeTexture->sourceEntries[0].dataOffset - 0x28);
  iVar8 = AVar2 * AVar3;
  pbVar13 = (g_TerrainCompositeTexture->textureSource).common.buildMetadata.
            assetRelativeAddressAnchor28 + (AVar12 - 0x28);
  pbVar16 = pbVar15;
  for (iVar9 = iVar8; iVar9 != 0; iVar9 = iVar9 + -1) {
    *(uint *)pbVar16 = *(uint *)pbVar13;
    pbVar13 = pbVar13 + 4;
    pbVar16 = pbVar16 + 4;
  }
  iVar9 = (pIVar5->worldRuntime0A30).activeFactionRuntimeIndex;
  pFVar14 = ((pIVar5->worldRuntime0A30).fieldGrid)->cells;
  pbVar13 = pbVar15;
  do {
    bVar1 = pFVar14->runtime60_6B[iVar9 + 0x10];
    uVar7 = (uint)bVar1;
    if ((bVar1 & 0x79) == 0) {
      if ((bVar1 & 0xf9) != 0) {
        uVar7 = (*(uint *)pbVar13 & 0xfefefefe) >> 1;
      }
      *(uint *)pbVar13 = uVar7;
    }
    pFVar14 = pFVar14 + 1;
    pbVar13 = pbVar13 + 4;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  for (pWVar4 = (pIVar5->worldRuntime0A30).ownerListHead; pWVar4 != (WorldOwnerListNode100 *)0x0;
      pWVar4 = pWVar4->nextNode) {
    if ((pWVar4->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (0xffffff < pWVar4->modelTintArgb)) {
      FVar18 = FieldGrid_WorldToGridQ12(pWVar4->worldYQ12,pWVar4->worldXQ12);
      pGVar6 = g_InGamePanelTextureSource;
      iVar8 = FVar18.rowQ12;
      iVar9 = FVar18.columnQ12 + 0x800 >> 0xc;
      if ((SCARRY4(iVar8,0x800) == iVar9 < 0) &&
         (((iVar10 = iVar8 + 0x800 >> 0xc, SCARRY4(iVar8,0x800) == iVar10 < 0 &&
           (iVar9 < (int)AVar2)) && (iVar10 < (int)AVar3)))) {
        entry = *(GameEntityRuntime **)((int)pWVar4->runtimePayload + 8);
        dVar11 = g_GameFactionRuntimeImage.records[(entry->common).ownership.ownerIndex].
                 factionClassOrMode;
        AVar12 = (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset;
        if (dVar11 != 0) {
          bVar17 = SelectionInfo_FindEntryCf(entry);
          if (!bVar17) {
            dVar11 = 0;
          }
          uVar7 = *(uint *)(g_InGamePanelTextureSource[1].opaqueTablePayloadBC_1FF +
                           *(int *)((int)pGVar6[2].common.buildMetadata.names.sourceName +
                                   AVar12 + 0x18) * 0x20 + dVar11 * 8 + 0x44);
          if (pWVar4->modelTintArgb < 0xff000000) {
            uVar7 = (uVar7 & 0xfefefefe) +
                    (*(uint *)(pbVar15 + (iVar10 * AVar2 + iVar9) * 4) & 0xfefefefe) >> 1;
          }
          *(uint *)(pbVar15 + (iVar10 * AVar2 + iVar9) * 4) = uVar7;
        }
      }
    }
  }
  return;
}

