/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/projection.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/projection.h>

/* Implementation ownership: world/terrain/projection. */

/* Address: 0x00506CD0.
   Ownership: world/terrain/projection.
   Purpose: Converts one world point to the hexagonal field grid, derives the bounded scan radius and row stride,
   seeds the center occupancy mask, and dispatches all six projected-occlusion wedge traces. Storage remains one
   signed 32-bit word. Typed parameters: p3 referenceHeightQ12→Q12. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged. Typed parameters: p4 worldXQ12→Q12, p5 worldYQ12→Q12.
   Local calls: TerrainProjectedOcclusion_TraceWedge0, TerrainProjectedOcclusion_TraceWedge1,
   TerrainProjectedOcclusion_TraceWedge2, TerrainProjectedOcclusion_TraceWedge3,
   TerrainProjectedOcclusion_TraceWedge4, TerrainProjectedOcclusion_TraceWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (ulonglong occupancyMaskBits,FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,
          Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  FieldGridCell *pFVar8;
  FieldGridCell *pFVar9;
  FieldGridCoordinatesEaxEdx8 FVar10;
  uint uVar11;
  uint uVar12;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    FVar10 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    iVar3 = g_TerrainScanReferenceHeight;
    uVar1 = FVar10.columnQ12 >> 0xc;
    uVar11 = FVar10.rowQ12 >> 0xc;
    uVar4 = (uint)((ulonglong)FVar10 & 0xfff00000fff);
    uVar6 = (uint)(((ulonglong)FVar10 & 0xfff00000fff) >> 0x20);
    uVar5 = uVar6 + uVar4 * 2;
    uVar12 = uVar1;
    if (uVar5 < 0x1000) {
      if (0xfff < uVar4 + uVar6 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    else if (uVar5 < 0x2001) {
      uVar12 = uVar1 + 1;
      if (uVar4 < uVar6) {
        uVar11 = uVar11 + 1;
        uVar12 = uVar1;
      }
    }
    else {
      uVar12 = uVar1 + 1;
      if (0x1fff < uVar4 + uVar6 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)uVar12) && (uVar5 = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)uVar11)) &&
        (uVar11 < fieldGrid->gridHeight)) && (uVar12 < uVar5)) {
      iVar7 = uVar11 * uVar5 + uVar12;
      if ((fieldGrid->cells[iVar7].flagsAndMaterial & 0x88006000) == 0) {
        fieldGrid->cells[iVar7].occupancyMask =
             fieldGrid->cells[iVar7].occupancyMask | occupancyMaskBits;
        iVar2 = g_TerrainScanRowStrideBytes;
        pFVar8 = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 iVar7 * 0x80 + -0x28);
        pFVar9 = (FieldGridCell *)((int)pFVar8 - g_TerrainScanRowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge0
                  (occupancyMaskBits,pFVar9->terrainHeight - iVar3,0,pFVar8);
        TerrainProjectedOcclusion_TraceWedge1
                  (occupancyMaskBits,pFVar9[-1].terrainHeight - iVar3,0,pFVar9);
        pFVar8 = (FieldGridCell *)((pFVar9 + -1)[-1].runtime0C_3F + iVar2 + -0xc);
        TerrainProjectedOcclusion_TraceWedge2
                  (occupancyMaskBits,pFVar8->terrainHeight - iVar3,0,pFVar9 + -1);
        pFVar9 = (FieldGridCell *)(pFVar8->runtime0C_3F + iVar2 + -0xc);
        TerrainProjectedOcclusion_TraceWedge3
                  (occupancyMaskBits,pFVar9->terrainHeight - iVar3,0,pFVar8);
        TerrainProjectedOcclusion_TraceWedge4
                  (occupancyMaskBits,pFVar9[1].terrainHeight - iVar3,0,pFVar9);
        TerrainProjectedOcclusion_TraceWedge5
                  (occupancyMaskBits,*(int *)((int)(pFVar9 + 1) + (200 - iVar2)) - iVar3,0,
                   pFVar9 + 1);
      }
    }
  }
  return;
}


/* Address: 0x005099D0.
   Ownership: world/terrain/projection.
   Purpose: Terrain-class runtime overlay callback; exact six-stack-argument contract and CF result. No unproved
   bit labels are introduced. Typed parameters: p0 cellFlagMask→FieldCellFlagMask_V338. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   radiusWorldUnits→FieldGridRadiusUnits.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyWedge0, FieldGridTerrainOverlayVariantA_ApplyWedge1,
   FieldGridTerrainOverlayVariantA_ApplyWedge2, FieldGridTerrainOverlayVariantA_ApplyWedge3,
   FieldGridTerrainOverlayVariantA_ApplyWedge4, FieldGridTerrainOverlayVariantA_ApplyWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  FieldGridCell *pFVar7;
  FieldGridCell *pFVar8;
  FieldGridCell *fieldCell;
  FieldGridCoordinatesEaxEdx8 FVar9;
  uint uVar10;
  uint uVar11;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    FVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    uVar1 = FVar9.columnQ12 >> 0xc;
    uVar10 = FVar9.rowQ12 >> 0xc;
    uVar3 = (uint)((ulonglong)FVar9 & 0xfff00000fff);
    uVar5 = (uint)(((ulonglong)FVar9 & 0xfff00000fff) >> 0x20);
    uVar4 = uVar5 + uVar3 * 2;
    uVar11 = uVar1;
    if (uVar4 < 0x1000) {
      if (0xfff < uVar3 + uVar5 * 2) {
        uVar10 = uVar10 + 1;
      }
    }
    else if (uVar4 < 0x2001) {
      uVar11 = uVar1 + 1;
      if (uVar3 < uVar5) {
        uVar10 = uVar10 + 1;
        uVar11 = uVar1;
      }
    }
    else {
      uVar11 = uVar1 + 1;
      if (0x1fff < uVar3 + uVar5 * 2) {
        uVar10 = uVar10 + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)uVar11) && (uVar4 = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)uVar10)) &&
        (uVar10 < fieldGrid->gridHeight)) &&
       ((uVar11 < uVar4 &&
        (iVar6 = uVar10 * uVar4 + uVar11,
        (fieldGrid->cells[iVar6].flagsAndMaterial & 0x88006000) == 0)))) {
      if (((fieldGrid->cells[iVar6].flagsAndMaterial & cellFlagMask) != 0) &&
         (fieldGrid->cells[iVar6].waterSurfaceDelta < 0)) {
        fieldGrid->cells[iVar6].runtimeOverlayOrHeightValue04 = cellValue;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      pFVar7 = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               iVar6 * 0x80 + -0x28);
      pFVar8 = (FieldGridCell *)((int)pFVar7 - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyWedge0(0,pFVar7);
      fieldCell = pFVar8 + -1;
      FieldGridTerrainOverlayVariantA_ApplyWedge1(0,pFVar8);
      pFVar7 = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + iVar2 + -0xc);
      FieldGridTerrainOverlayVariantA_ApplyWedge2(0,fieldCell);
      pFVar8 = (FieldGridCell *)(pFVar7->runtime0C_3F + iVar2 + -0xc);
      FieldGridTerrainOverlayVariantA_ApplyWedge3(0,pFVar7);
      FieldGridTerrainOverlayVariantA_ApplyWedge4(0,pFVar8);
      FieldGridTerrainOverlayVariantA_ApplyWedge5(0,pFVar8 + 1);
      return false;
    }
  }
  return true;
}


/* Address: 0x0050A190.
   Ownership: world/terrain/projection.
   Purpose: Terrain-class runtime overlay callback; exact six-stack-argument contract and CF result. No unproved
   bit labels are introduced. Typed parameters: p0 cellFlagMask→FieldCellFlagMask_V338. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   radiusWorldUnits→FieldGridRadiusUnits.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyWedge0, FieldGridTerrainOverlayVariantB_ApplyWedge1,
   FieldGridTerrainOverlayVariantB_ApplyWedge2, FieldGridTerrainOverlayVariantB_ApplyWedge3,
   FieldGridTerrainOverlayVariantB_ApplyWedge4, FieldGridTerrainOverlayVariantB_ApplyWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  FieldGridCell *pFVar7;
  FieldGridCell *pFVar8;
  FieldGridCell *fieldCell;
  FieldGridCoordinatesEaxEdx8 FVar9;
  uint uVar10;
  uint uVar11;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = cellValue;
    g_TerrainScanSharedSelectorValue.fieldCellFlagMask = cellFlagMask;
    FVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    uVar1 = FVar9.columnQ12 >> 0xc;
    uVar10 = FVar9.rowQ12 >> 0xc;
    uVar3 = (uint)((ulonglong)FVar9 & 0xfff00000fff);
    uVar5 = (uint)(((ulonglong)FVar9 & 0xfff00000fff) >> 0x20);
    uVar4 = uVar5 + uVar3 * 2;
    uVar11 = uVar1;
    if (uVar4 < 0x1000) {
      if (0xfff < uVar3 + uVar5 * 2) {
        uVar10 = uVar10 + 1;
      }
    }
    else if (uVar4 < 0x2001) {
      uVar11 = uVar1 + 1;
      if (uVar3 < uVar5) {
        uVar10 = uVar10 + 1;
        uVar11 = uVar1;
      }
    }
    else {
      uVar11 = uVar1 + 1;
      if (0x1fff < uVar3 + uVar5 * 2) {
        uVar10 = uVar10 + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)uVar11) && (uVar4 = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)uVar10)) &&
        (uVar10 < fieldGrid->gridHeight)) &&
       ((uVar11 < uVar4 &&
        (iVar6 = uVar10 * uVar4 + uVar11,
        (fieldGrid->cells[iVar6].flagsAndMaterial & 0x88006000) == 0)))) {
      if (((fieldGrid->cells[iVar6].flagsAndMaterial & cellFlagMask) != 0) &&
         (0 < fieldGrid->cells[iVar6].waterSurfaceDelta)) {
        fieldGrid->cells[iVar6].runtimeOverlayOrHeightValue04 = cellValue;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      pFVar7 = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               iVar6 * 0x80 + -0x28);
      pFVar8 = (FieldGridCell *)((int)pFVar7 - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyWedge0(0,pFVar7);
      fieldCell = pFVar8 + -1;
      FieldGridTerrainOverlayVariantB_ApplyWedge1(0,pFVar8);
      pFVar7 = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + iVar2 + -0xc);
      FieldGridTerrainOverlayVariantB_ApplyWedge2(0,fieldCell);
      pFVar8 = (FieldGridCell *)(pFVar7->runtime0C_3F + iVar2 + -0xc);
      FieldGridTerrainOverlayVariantB_ApplyWedge3(0,pFVar7);
      FieldGridTerrainOverlayVariantB_ApplyWedge4(0,pFVar8);
      FieldGridTerrainOverlayVariantB_ApplyWedge5(0,pFVar8 + 1);
      return false;
    }
  }
  return true;
}


/* Address: 0x00500F50.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected grid transform shade and queue.
   Local calls: TerrainProjectedGrid_ClipRowSpansAgainstPlane,
   TerrainProjectedVertex_TransformProjectAndShadeVariantB,
   TerrainProjectedVertex_TransformProjectAndShadeVariantA, TerrainProjectedQuad_QueueAsTwoTrianglesRegs.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedGrid_TransformShadeAndQueue
          (FieldGridAsset *fieldGrid,FrontendModelPointerContextRuntimeState17C *renderContext)

{
  FieldGridDimension FVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  FieldGridDimension FVar5;
  FieldGridDimension FVar6;
  int iVar7;
  FieldGridDimension FVar8;
  FieldGridCell *pFVar9;
  TerrainProjectedVertexWorkRecord *pTVar10;
  TerrainProjectedRowSpan *pTVar11;
  int iVar12;
  
  if ((renderContext->contextFlags & 0x800) == 0) {
    pTVar11 = g_TerrainProjectedRowSpans;
    FVar1 = fieldGrid->gridWidth;
    FVar5 = fieldGrid->gridHeight;
    do {
      pTVar11->firstColumn = 0;
      pTVar11->endColumnExclusive = FVar1;
      pTVar11 = pTVar11 + 1;
      FVar5 = FVar5 - 1;
    } while (FVar5 != 0);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0 + 1);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0 + 2);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(fieldGrid,g_FrustumPlaneNormalFixed_0 + 3);
    FVar1 = fieldGrid->gridWidth;
    FVar5 = fieldGrid->gridHeight;
    pFVar9 = fieldGrid->cells;
    FVar6 = FVar5;
    FVar8 = FVar1;
    do {
      do {
        pFVar9->flagsAndMaterial = pFVar9->flagsAndMaterial | 0x4200000;
        pFVar9 = pFVar9 + 1;
        FVar8 = FVar8 - 1;
      } while (FVar8 != 0);
      FVar6 = FVar6 - 1;
      FVar8 = FVar1;
    } while (FVar6 != 0);
    pTVar11 = g_TerrainProjectedRowSpans;
    iVar12 = FVar5 - 1;
    iVar7 = g_TerrainProjectedRowSpans[0].firstColumn;
    iVar4 = g_TerrainProjectedRowSpans[0].endColumnExclusive;
    do {
      pTVar11 = pTVar11 + 1;
      iVar2 = pTVar11->firstColumn;
      iVar3 = pTVar11->endColumnExclusive;
      if (iVar4 == 0) {
        pTVar11[-1].firstColumn = iVar2;
        pTVar11[-1].endColumnExclusive = iVar3;
      }
      else if (iVar3 == 0) {
        pTVar11->firstColumn = iVar7;
        pTVar11->endColumnExclusive = iVar4;
      }
      else {
        if (iVar7 < iVar2) {
          pTVar11->firstColumn = iVar7;
        }
        else if (iVar2 < pTVar11[-1].firstColumn) {
          pTVar11[-1].firstColumn = iVar2;
        }
        if (iVar3 < iVar4) {
          pTVar11->endColumnExclusive = iVar4;
        }
        else if (pTVar11[-1].endColumnExclusive < iVar3) {
          pTVar11[-1].endColumnExclusive = iVar3;
        }
      }
      iVar12 = iVar12 + -1;
      iVar7 = iVar2;
      iVar4 = iVar3;
    } while (iVar12 != 0);
  }
  pTVar11 = g_TerrainProjectedRowSpans;
  FVar1 = fieldGrid->gridWidth;
  FVar5 = fieldGrid->gridHeight;
  if (((fieldGrid->runtimeStateFlags & 1) == 0) && ((renderContext->contextFlags & 0x800) != 0)) {
    pFVar9 = fieldGrid->cells;
    do {
      iVar7 = pTVar11->firstColumn;
      iVar4 = pTVar11->endColumnExclusive - iVar7;
      if (iVar4 != 0 && iVar7 <= pTVar11->endColumnExclusive) {
        pTVar10 = (TerrainProjectedVertexWorkRecord *)(pFVar9 + iVar7);
        do {
          TerrainProjectedVertex_TransformProjectAndShadeVariantB(pTVar10);
          pTVar10 = pTVar10 + 1;
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
      }
      pTVar11 = pTVar11 + 1;
      pFVar9 = pFVar9 + FVar1;
      FVar5 = FVar5 - 1;
    } while (FVar5 != 0);
  }
  else {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags & 0xfffffffe;
    renderContext->contextFlags = renderContext->contextFlags & 0xfffff7ff;
    pFVar9 = fieldGrid->cells;
    do {
      iVar7 = pTVar11->firstColumn;
      iVar4 = pTVar11->endColumnExclusive - iVar7;
      if (iVar4 != 0 && iVar7 <= pTVar11->endColumnExclusive) {
        pTVar10 = (TerrainProjectedVertexWorkRecord *)(pFVar9 + iVar7);
        do {
          TerrainProjectedVertex_TransformProjectAndShadeVariantA(pTVar10);
          pTVar10 = pTVar10 + 1;
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
      }
      pTVar11 = pTVar11 + 1;
      pFVar9 = pFVar9 + FVar1;
      FVar5 = FVar5 - 1;
    } while (FVar5 != 0);
  }
  FVar1 = fieldGrid->gridWidth;
  pFVar9 = fieldGrid->cells;
  pTVar11 = g_TerrainProjectedRowSpans;
  iVar7 = fieldGrid->gridHeight - 1;
  do {
    iVar4 = pTVar11->firstColumn;
    iVar12 = pTVar11->endColumnExclusive - iVar4;
    if (iVar12 != 0 && iVar4 <= pTVar11->endColumnExclusive) {
      iVar12 = iVar12 + -1;
      if (iVar12 != 0) {
        pTVar10 = (TerrainProjectedVertexWorkRecord *)(pFVar9 + iVar4);
        do {
          TerrainProjectedQuad_QueueAsTwoTrianglesRegs(FVar1 * 0x80,pTVar10,renderContext);
          pTVar10 = pTVar10 + 1;
          iVar12 = iVar12 + -1;
        } while (iVar12 != 0);
      }
    }
    pTVar11 = pTVar11 + 1;
    pFVar9 = pFVar9 + FVar1;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  return;
}


/* Address: 0x005066D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 0, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection0, TerrainProjectedOcclusion_ScanDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge0
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  TerrainProjectedHeightThresholdQ20 TVar5;
  FieldGridCell *cell_00;
  
  TVar5 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + TVar5) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)TVar5 <= (int)uVar4) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar4;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + 1;
      uVar4 = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,uVar4,cell_00);
      if (g_TerrainScanStepLimit <= uVar4) {
        return;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - iVar2)) & 0x88006000) != 0) {
        return;
      }
      iVar3 = *(int *)((int)cell_00 + (0x48 - iVar2));
      if (0 < *(int *)((int)cell_00 + (0x4c - iVar2))) {
        iVar3 = iVar3 + *(int *)((int)cell_00 + (0x4c - iVar2));
      }
      lVar1 = (longlong)(iVar3 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + uVar4 * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar4) {
        *(ulonglong *)((int)cell_00 + (0x70 - iVar2)) =
             *(ulonglong *)((int)cell_00 + (0x70 - iVar2)) | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar4;
      }
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - iVar2));
      scanStep = scanStep + 7;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      TVar5 = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005067D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 1, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection1, TerrainProjectedOcclusion_ScanDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge1
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  TerrainProjectedHeightThresholdQ20 TVar5;
  FieldGridCell *cell_00;
  
  TVar5 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + TVar5) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)TVar5 <= (int)uVar4) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar4;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      uVar4 = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection1
                (occupancyMaskBits,projectedHeightThresholdQ20,uVar4,
                 (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= uVar4) {
        return;
      }
      if ((*(uint *)((int)cell + (0x50 - iVar2)) & 0x88006000) != 0) {
        return;
      }
      iVar3 = *(int *)((int)cell + (0x48 - iVar2));
      if (0 < *(int *)((int)cell + (0x4c - iVar2))) {
        iVar3 = iVar3 + *(int *)((int)cell + (0x4c - iVar2));
      }
      lVar1 = (longlong)(iVar3 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + uVar4 * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar4) {
        *(ulonglong *)((int)cell + (0x70 - iVar2)) =
             *(ulonglong *)((int)cell + (0x70 - iVar2)) | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar4;
      }
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar2));
      scanStep = scanStep + 7;
      cell = cell_00 + 1;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,cell_00);
      TVar5 = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005068D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 2, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection2, TerrainProjectedOcclusion_ScanDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge2
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  longlong lVar1;
  int iVar2;
  uint uVar3;
  TerrainProjectedHeightThresholdQ20 TVar4;
  
  TVar4 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + TVar4) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)TVar4 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      uVar3 = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection2
                (occupancyMaskBits,projectedHeightThresholdQ20,uVar3,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= uVar3) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell[-1].terrainHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        iVar2 = iVar2 + cell[-1].waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + uVar3 * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell[-1].occupancyMask = cell[-1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      cell_00 = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,cell_00);
      TVar4 = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005069D0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 3, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection3, TerrainProjectedOcclusion_ScanDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge3
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  FieldCellPersistedAux FVar3;
  int iVar4;
  uint uVar5;
  TerrainProjectedHeightThresholdQ20 TVar6;
  FieldGridCell *cell_00;
  
  TVar6 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + TVar6) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      FVar3 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        FVar3 = FVar3 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(int)(FVar3 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)TVar6 <= (int)uVar5) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar5;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + -1;
      uVar5 = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection3
                (occupancyMaskBits,projectedHeightThresholdQ20,uVar5,cell_00);
      if (g_TerrainScanStepLimit <= uVar5) {
        return;
      }
      if ((*(uint *)(cell_00->runtime60_6B + iVar2 + -0x10) & 0x88006000) != 0) {
        return;
      }
      iVar4 = *(int *)(cell_00->runtime60_6B + iVar2 + -0x18);
      if (0 < *(int *)(cell_00->runtime60_6B + iVar2 + -0x14)) {
        iVar4 = iVar4 + *(int *)(cell_00->runtime60_6B + iVar2 + -0x14);
      }
      lVar1 = (longlong)(iVar4 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + uVar5 * 4);
      uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar5) {
        *(ulonglong *)(cell_00->runtime60_6B + iVar2 + 0x10) =
             *(ulonglong *)(cell_00->runtime60_6B + iVar2 + 0x10) | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar5;
      }
      cell = (FieldGridCell *)(cell_00[-1].runtime0C_3F + iVar2 + -0xc);
      scanStep = scanStep + 7;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      TVar6 = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506AD0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 4, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection4, TerrainProjectedOcclusion_ScanDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge4
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  TerrainProjectedHeightThresholdQ20 TVar6;
  
  TVar6 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + TVar6) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar3 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar3 = iVar3 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar3 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)TVar6 <= (int)uVar5) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar5;
      }
      iVar3 = g_TerrainScanRowStrideBytes;
      uVar5 = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection4
                (occupancyMaskBits,projectedHeightThresholdQ20,uVar5,
                 (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= uVar5) {
        return;
      }
      if ((*(uint *)(cell->runtime60_6B + iVar3 + -0x10) & 0x88006000) != 0) {
        return;
      }
      iVar4 = *(int *)(cell->runtime60_6B + iVar3 + -0x18);
      if (0 < *(int *)(cell->runtime60_6B + iVar3 + -0x14)) {
        iVar4 = iVar4 + *(int *)(cell->runtime60_6B + iVar3 + -0x14);
      }
      lVar1 = (longlong)(iVar4 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + uVar5 * 4);
      uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar5) {
        *(ulonglong *)(cell->runtime60_6B + iVar3 + 0x10) =
             *(ulonglong *)(cell->runtime60_6B + iVar3 + 0x10) | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar5;
      }
      pbVar2 = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(pbVar2 + g_TerrainScanRowStrideBytes + iVar3 + -0xc) + -1;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,
                 (FieldGridCell *)(pbVar2 + g_TerrainScanRowStrideBytes + iVar3 + -0xc));
      TVar6 = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506BD0.
   Ownership: world/terrain/projection.
   Purpose: Traces two adjacent directional terrain runs for wedge 5, carries the maximum projected height
   threshold across both legs, stops at excluded cells, and propagates the incoming 64-bit mask. Typed parameters:
   p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainProjectedOcclusion_ScanDirection5, TerrainProjectedOcclusion_ScanDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_TraceWedge5
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  longlong lVar1;
  int iVar2;
  uint uVar3;
  TerrainProjectedHeightThresholdQ20 TVar4;
  
  TVar4 = cell->terrainHeight - g_TerrainScanReferenceHeight;
  if (scanStep < g_TerrainScanStepLimit) {
    projectedHeightThresholdQ20 = (int)(projectedHeightThresholdQ20 + TVar4) >> 1;
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)TVar4 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      uVar3 = scanStep + 4;
      TerrainProjectedOcclusion_ScanDirection5
                (occupancyMaskBits,projectedHeightThresholdQ20,uVar3,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= uVar3) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell[1].terrainHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        iVar2 = iVar2 + cell[1].waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + uVar3 * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell[1].occupancyMask = cell[1].occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      cell_00 = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      TerrainProjectedOcclusion_ScanDirection0
                (occupancyMaskBits,projectedHeightThresholdQ20,scanStep,cell_00);
      TVar4 = projectedHeightThresholdQ20;
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509580.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection0, FieldGridTerrainOverlayVariantA_ApplyDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)fieldCell_00 + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)((int)fieldCell_00 + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell_00 + (0x4c - rowStrideBytes)) < 0)) {
        *(dword *)((int)fieldCell_00 + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)fieldCell_00 + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509640.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection1, FieldGridTerrainOverlayVariantA_ApplyDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep + 4,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell + (0x4c - rowStrideBytes)) < 0)) {
        *(dword *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
      fieldCell = fieldCell_00 + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection2(scanStep,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005096F0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection2, FieldGridTerrainOverlayVariantA_ApplyDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell[-1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) !=
           0) && (fieldCell[-1].waterSurfaceDelta < 0)) {
        fieldCell[-1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + -2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)((int)fieldCell + (-0x80 - g_TerrainScanRowStrideBytes));
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005097A0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection3, FieldGridTerrainOverlayVariantA_ApplyDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  int iVar1;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      iVar1 = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + -1;
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(fieldCell_00->runtime60_6B + iVar1 + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)(fieldCell_00->runtime60_6B + iVar1 + -0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell_00->runtime60_6B + iVar1 + -0x14) < 0)) {
        *(dword *)(fieldCell_00->runtime0C_3F + iVar1 + -8) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(fieldCell_00[-1].runtime0C_3F + iVar1 + -0xc);
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep,(FieldGridCell *)
                          (fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509860.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection4, FieldGridTerrainOverlayVariantA_ApplyDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  byte *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell->runtime60_6B + rowStrideBytes + -0x14) < 0)) {
        *(dword *)(fieldCell->runtime0C_3F + rowStrideBytes + -8) = g_TerrainScanReferenceHeight;
      }
      currentCellRuntimeBase = fieldCell->runtime0C_3F;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) +
                  -1;
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase +
                          g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509910.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantA_ApplyDirection5, FieldGridTerrainOverlayVariantA_ApplyDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyWedge5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell[1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0
          ) && (fieldCell[1].waterSurfaceDelta < 0)) {
        fieldCell[1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + 2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509DD0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection0, FieldGridTerrainOverlayVariantB_ApplyDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)fieldCell_00 + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell_00 + (0x4c - rowStrideBytes))) {
        *(dword *)((int)fieldCell_00 + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)fieldCell_00 + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509E70.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection1, FieldGridTerrainOverlayVariantB_ApplyDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep + 4,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell + (0x4c - rowStrideBytes))) {
        *(dword *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
      fieldCell = fieldCell_00 + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection2(scanStep,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509F10.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection2, FieldGridTerrainOverlayVariantB_ApplyDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell[-1].waterSurfaceDelta) {
        fieldCell[-1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + -2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)((int)fieldCell + (-0x80 - g_TerrainScanRowStrideBytes));
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509FB0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection3, FieldGridTerrainOverlayVariantB_ApplyDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  int iVar1;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      iVar1 = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + -1;
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(fieldCell_00->runtime60_6B + iVar1 + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell_00->runtime60_6B + iVar1 + -0x14)) {
        *(dword *)(fieldCell_00->runtime0C_3F + iVar1 + -8) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(fieldCell_00[-1].runtime0C_3F + iVar1 + -0xc);
      scanStep = scanStep + 7;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep,(FieldGridCell *)
                          (fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x0050A050.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection4, FieldGridTerrainOverlayVariantB_ApplyDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  byte *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(fieldCell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell->runtime60_6B + rowStrideBytes + -0x14)) {
        *(dword *)(fieldCell->runtime0C_3F + rowStrideBytes + -8) = g_TerrainScanReferenceHeight;
      }
      currentCellRuntimeBase = fieldCell->runtime0C_3F;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) +
                  -1;
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase +
                          g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x0050A0F0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGridTerrainOverlayVariantB_ApplyDirection5, FieldGridTerrainOverlayVariantB_ApplyDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyWedge5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell[1].waterSurfaceDelta) {
        fieldCell[1].runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + 2;
      scanStep = scanStep + 7;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep,fieldCell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00500CE0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected quad queue as two triangles register result.
   Local calls: TerrainProjectedTriangle_ClipInterpolateAndQueueTextured.
*/
void __thandor_void_preserve_ecx_edx
TerrainProjectedQuad_QueueAsTwoTrianglesRegs
          (dword rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             (TerrainProjectedVertexWorkRecord *)
             (topLeftVertex->reserved08_0B + (rowStrideBytes - 8)),topLeftVertex,renderContext);
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (topLeftVertex->surfacePacketIndex,topLeftVertex + 1,
             (TerrainProjectedVertexWorkRecord *)
             (topLeftVertex[1].reserved08_0B + (rowStrideBytes - 8)),
             (TerrainProjectedVertexWorkRecord *)
             (topLeftVertex->reserved08_0B + (rowStrideBytes - 8)),renderContext);
  return;
}


/* Address: 0x005004A0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected vertex transform project and shade variant a.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime], GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
   [graphics/render/shading].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedVertex_TransformProjectAndShadeVariantA(TerrainProjectedVertexWorkRecord *vertex)

{
  GraphicsWorldCoordinateQ12 *pGVar1;
  PackedArgb32 PVar2;
  PackedArgb32 PVar3;
  GraphicsFixedVec3 *pGVar4;
  int iVar5;
  int iVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  ushort uVar11;
  ushort uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  undefined1 uVar18;
  undefined1 uVar19;
  MmxPackedValue64 MVar16;
  undefined8 uVar17;
  undefined1 uVar20;
  undefined1 uVar21;
  GraphicsProjectedPointPair GVar22;
  
  uVar14 = vertex->projectionFlags & 0xe801ffff;
  if ((vertex->projectionFlags & 0xff) != 0xff) {
    uVar15 = uVar14 | 0x200000;
    FixedTransform_ApplyPoint(&vertex->viewPointA,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed)
    ;
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointA).z) {
      GVar22 = Graphics_ProjectViewPoint(&vertex->viewPointA);
      vertex->projectedPointA = GVar22;
      uVar15 = uVar14;
      if (g_ProjectionClipRect.minX <= GVar22.projectedX) {
        uVar15 = uVar14 | 0x20000;
      }
      if (GVar22.projectedX < g_ProjectionClipRect.maxX) {
        uVar15 = uVar15 | 0x80000;
      }
      if (g_ProjectionClipRect.minY <= GVar22.projectedY) {
        uVar15 = uVar15 | 0x40000;
      }
      if (GVar22.projectedY < g_ProjectionClipRect.maxY) {
        uVar15 = uVar15 | 0x100000;
      }
    }
    PVar2 = vertex->packedColorA;
    PVar3 = vertex->basePackedColor;
    uVar18 = (undefined1)(PVar2 >> 0x18);
    uVar11 = CONCAT11(uVar18,uVar18);
    uVar19 = (undefined1)(PVar2 >> 0x10);
    uVar18 = (undefined1)(PVar2 >> 8);
    uVar20 = (undefined1)(PVar3 >> 0x18);
    uVar12 = CONCAT11(uVar20,uVar20);
    uVar21 = (undefined1)(PVar3 >> 0x10);
    uVar20 = (undefined1)(PVar3 >> 8);
    MVar16 = CONCAT26(uVar11 >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar11,uVar19),CONCAT14(uVar19,PVar2)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(uVar18,uVar18) >> 6,
                                        CONCAT11((char)PVar2,(char)PVar2) >> 6)));
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      MVar16 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointA,MVar16);
    }
    uVar17 = pmulhw(MVar16,CONCAT26(uVar12 >> 2,
                                    CONCAT24((ushort)(CONCAT35(CONCAT21(uVar12,uVar21),
                                                               CONCAT14(uVar21,PVar3)) >> 0x20) >> 2
                                             ,CONCAT22(CONCAT11(uVar20,uVar20) >> 2,
                                                       CONCAT11((char)PVar3,(char)PVar3) >> 2))));
    sVar7 = (short)uVar17;
    sVar8 = (short)((ulonglong)uVar17 >> 0x10);
    sVar9 = (short)((ulonglong)uVar17 >> 0x20);
    sVar10 = (short)((ulonglong)uVar17 >> 0x30);
    pGVar4 = vertex->secondaryOffset;
    vertex->shadedColorA =
         CONCAT13((0 < sVar10) * (sVar10 < 0x100) * (char)((ulonglong)uVar17 >> 0x30) -
                  (0xff < sVar10),
                  CONCAT12((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar17 >> 0x20) -
                           (0xff < sVar9),
                           CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                    (char)((ulonglong)uVar17 >> 0x10) - (0xff < sVar8),
                                    (0 < sVar7) * (sVar7 < 0x100) * (char)uVar17 - (0xff < sVar7))))
    ;
    uVar14 = uVar15 | 0x4000000;
    iVar5 = pGVar4->x;
    iVar6 = pGVar4->y;
    iVar13 = pGVar4->z;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x + iVar5;
    iVar13 = iVar13 + vertex->secondaryProjectionDepthQ12;
    pGVar1 = &(vertex->sourcePoint).y;
    *pGVar1 = *pGVar1 + iVar6;
    pGVar1 = &(vertex->sourcePoint).z;
    *pGVar1 = *pGVar1 + iVar13;
    FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed)
    ;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x - iVar5;
    pGVar1 = &(vertex->sourcePoint).y;
    *pGVar1 = *pGVar1 - iVar6;
    pGVar1 = &(vertex->sourcePoint).z;
    *pGVar1 = *pGVar1 - iVar13;
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointB).z) {
      GVar22 = Graphics_ProjectViewPoint(&vertex->viewPointB);
      vertex->projectedPointB = GVar22;
      uVar14 = uVar15;
      if (g_ProjectionClipRect.minX <= GVar22.projectedX) {
        uVar14 = uVar15 | 0x400000;
      }
      if (GVar22.projectedX < g_ProjectionClipRect.maxX) {
        uVar14 = uVar14 | 0x1000000;
      }
      if (g_ProjectionClipRect.minY <= GVar22.projectedY) {
        uVar14 = uVar14 | 0x800000;
      }
      if (GVar22.projectedY < g_ProjectionClipRect.maxY) {
        uVar14 = uVar14 | 0x2000000;
      }
    }
    PVar2 = vertex->packedColorB;
    PVar3 = vertex->basePackedColor;
    uVar18 = (undefined1)(PVar2 >> 0x18);
    uVar11 = CONCAT11(uVar18,uVar18);
    uVar19 = (undefined1)(PVar2 >> 0x10);
    uVar18 = (undefined1)(PVar2 >> 8);
    uVar20 = (undefined1)(PVar3 >> 0x18);
    uVar12 = CONCAT11(uVar20,uVar20);
    uVar21 = (undefined1)(PVar3 >> 0x10);
    uVar20 = (undefined1)(PVar3 >> 8);
    MVar16 = CONCAT26(uVar11 >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar11,uVar19),CONCAT14(uVar19,PVar2)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(uVar18,uVar18) >> 6,
                                        CONCAT11((char)PVar2,(char)PVar2) >> 6)));
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      MVar16 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointB,MVar16);
    }
    uVar17 = pmulhw(MVar16,CONCAT26(uVar12 >> 2,
                                    CONCAT24((ushort)(CONCAT35(CONCAT21(uVar12,uVar21),
                                                               CONCAT14(uVar21,PVar3)) >> 0x20) >> 2
                                             ,CONCAT22(CONCAT11(uVar20,uVar20) >> 2,
                                                       CONCAT11((char)PVar3,(char)PVar3) >> 2))));
    sVar7 = (short)uVar17;
    sVar8 = (short)((ulonglong)uVar17 >> 0x10);
    sVar9 = (short)((ulonglong)uVar17 >> 0x20);
    sVar10 = (short)((ulonglong)uVar17 >> 0x30);
    vertex->projectionFlags = uVar14;
    vertex->shadedColorB =
         CONCAT13((0 < sVar10) * (sVar10 < 0x100) * (char)((ulonglong)uVar17 >> 0x30) -
                  (0xff < sVar10),
                  CONCAT12((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar17 >> 0x20) -
                           (0xff < sVar9),
                           CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                    (char)((ulonglong)uVar17 >> 0x10) - (0xff < sVar8),
                                    (0 < sVar7) * (sVar7 < 0x100) * (char)uVar17 - (0xff < sVar7))))
    ;
  }
  return;
}


/* Address: 0x005006A0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected vertex transform project and shade variant b.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime], GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
   [graphics/render/shading].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedVertex_TransformProjectAndShadeVariantB(TerrainProjectedVertexWorkRecord *vertex)

{
  GraphicsWorldCoordinateQ12 *pGVar1;
  GraphicsFixedVec3 *pGVar2;
  int iVar3;
  int iVar4;
  PackedArgb32 PVar5;
  PackedArgb32 PVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  ushort uVar11;
  ushort uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  undefined1 uVar18;
  undefined1 uVar19;
  MmxPackedValue64 MVar16;
  undefined8 uVar17;
  undefined1 uVar20;
  undefined1 uVar21;
  GraphicsProjectedPointPair GVar22;
  
  uVar15 = vertex->projectionFlags;
  pGVar2 = vertex->secondaryOffset;
  if ((uVar15 & 0x10000000) != 0) {
    uVar14 = uVar15 & 0xf83fffff;
    uVar15 = uVar14 | 0x4000000;
    iVar3 = pGVar2->x;
    iVar4 = pGVar2->y;
    iVar13 = pGVar2->z;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x + iVar3;
    iVar13 = iVar13 + vertex->secondaryProjectionDepthQ12;
    pGVar1 = &(vertex->sourcePoint).y;
    *pGVar1 = *pGVar1 + iVar4;
    pGVar1 = &(vertex->sourcePoint).z;
    *pGVar1 = *pGVar1 + iVar13;
    FixedTransform_ApplyPoint(&vertex->viewPointB,&vertex->sourcePoint,&g_ViewProjectionMatrixFixed)
    ;
    (vertex->sourcePoint).x = (vertex->sourcePoint).x - iVar3;
    pGVar1 = &(vertex->sourcePoint).y;
    *pGVar1 = *pGVar1 - iVar4;
    pGVar1 = &(vertex->sourcePoint).z;
    *pGVar1 = *pGVar1 - iVar13;
    if ((int)g_ProjectionScaleFixed < (vertex->viewPointB).z) {
      GVar22 = Graphics_ProjectViewPoint(&vertex->viewPointB);
      vertex->projectedPointB = GVar22;
      uVar15 = uVar14;
      if (g_ProjectionClipRect.minX <= GVar22.projectedX) {
        uVar15 = uVar14 | 0x400000;
      }
      if (GVar22.projectedX < g_ProjectionClipRect.maxX) {
        uVar15 = uVar15 | 0x1000000;
      }
      if (g_ProjectionClipRect.minY <= GVar22.projectedY) {
        uVar15 = uVar15 | 0x800000;
      }
      if (GVar22.projectedY < g_ProjectionClipRect.maxY) {
        uVar15 = uVar15 | 0x2000000;
      }
    }
    PVar5 = vertex->packedColorB;
    PVar6 = vertex->basePackedColor;
    uVar18 = (undefined1)(PVar5 >> 0x18);
    uVar11 = CONCAT11(uVar18,uVar18);
    uVar19 = (undefined1)(PVar5 >> 0x10);
    uVar18 = (undefined1)(PVar5 >> 8);
    uVar20 = (undefined1)(PVar6 >> 0x18);
    uVar12 = CONCAT11(uVar20,uVar20);
    uVar21 = (undefined1)(PVar6 >> 0x10);
    uVar20 = (undefined1)(PVar6 >> 8);
    MVar16 = CONCAT26(uVar11 >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar11,uVar19),CONCAT14(uVar19,PVar5)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(uVar18,uVar18) >> 6,
                                        CONCAT11((char)PVar5,(char)PVar5) >> 6)));
    if (vertex->lightingLookupIndexOrSentinel == 0xff) {
      MVar16 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (&vertex->viewPointB,MVar16);
    }
    uVar17 = pmulhw(MVar16,CONCAT26(uVar12 >> 2,
                                    CONCAT24((ushort)(CONCAT35(CONCAT21(uVar12,uVar21),
                                                               CONCAT14(uVar21,PVar6)) >> 0x20) >> 2
                                             ,CONCAT22(CONCAT11(uVar20,uVar20) >> 2,
                                                       CONCAT11((char)PVar6,(char)PVar6) >> 2))));
    sVar7 = (short)uVar17;
    sVar8 = (short)((ulonglong)uVar17 >> 0x10);
    sVar9 = (short)((ulonglong)uVar17 >> 0x20);
    sVar10 = (short)((ulonglong)uVar17 >> 0x30);
    vertex->shadedColorB =
         CONCAT13((0 < sVar10) * (sVar10 < 0x100) * (char)((ulonglong)uVar17 >> 0x30) -
                  (0xff < sVar10),
                  CONCAT12((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar17 >> 0x20) -
                           (0xff < sVar9),
                           CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                    (char)((ulonglong)uVar17 >> 0x10) - (0xff < sVar8),
                                    (0 < sVar7) * (sVar7 < 0x100) * (char)uVar17 - (0xff < sVar7))))
    ;
  }
  PVar5 = vertex->packedColorA;
  PVar6 = vertex->basePackedColor;
  uVar18 = (undefined1)(PVar5 >> 0x18);
  uVar11 = CONCAT11(uVar18,uVar18);
  uVar19 = (undefined1)(PVar5 >> 0x10);
  uVar18 = (undefined1)(PVar5 >> 8);
  uVar20 = (undefined1)(PVar6 >> 0x18);
  uVar12 = CONCAT11(uVar20,uVar20);
  uVar21 = (undefined1)(PVar6 >> 0x10);
  uVar20 = (undefined1)(PVar6 >> 8);
  MVar16 = CONCAT26(uVar11 >> 6,
                    CONCAT24((ushort)(CONCAT35(CONCAT21(uVar11,uVar19),CONCAT14(uVar19,PVar5)) >>
                                     0x20) >> 6,
                             CONCAT22(CONCAT11(uVar18,uVar18) >> 6,
                                      CONCAT11((char)PVar5,(char)PVar5) >> 6)));
  if (vertex->lightingLookupIndexOrSentinel == 0xff) {
    MVar16 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                       (&vertex->viewPointA,MVar16);
  }
  uVar17 = pmulhw(MVar16,CONCAT26(uVar12 >> 2,
                                  CONCAT24((ushort)(CONCAT35(CONCAT21(uVar12,uVar21),
                                                             CONCAT14(uVar21,PVar6)) >> 0x20) >> 2,
                                           CONCAT22(CONCAT11(uVar20,uVar20) >> 2,
                                                    CONCAT11((char)PVar6,(char)PVar6) >> 2))));
  sVar7 = (short)uVar17;
  sVar8 = (short)((ulonglong)uVar17 >> 0x10);
  sVar9 = (short)((ulonglong)uVar17 >> 0x20);
  sVar10 = (short)((ulonglong)uVar17 >> 0x30);
  vertex->shadedColorA =
       CONCAT13((0 < sVar10) * (sVar10 < 0x100) * (char)((ulonglong)uVar17 >> 0x30) -
                (0xff < sVar10),
                CONCAT12((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar17 >> 0x20) -
                         (0xff < sVar9),
                         CONCAT11((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar17 >> 0x10)
                                  - (0xff < sVar8),
                                  (0 < sVar7) * (sVar7 < 0x100) * (char)uVar17 - (0xff < sVar7))));
  vertex->projectionFlags = uVar15;
  return;
}


/* Address: 0x00500820.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected triangle clip interpolate and queue textured.
   Cross-module calls: Triangle2D_ComputeBarycentricWeightsQ12Packed [core/math/geometry],
   GraphicsPrimitiveQueue_AppendTexturedTriangleRegs [graphics/render/primitives],
   GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
          (int surfacePacketIndex,TerrainProjectedVertexWorkRecord *vertex2,
          TerrainProjectedVertexWorkRecord *vertex1,TerrainProjectedVertexWorkRecord *vertex0,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  GraphicsPrimitiveDispatchFlags *pGVar1;
  dword dVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  short sVar11;
  short sVar12;
  short sVar13;
  short sVar14;
  ushort uVar15;
  ushort uVar16;
  ushort uVar17;
  void *pvVar18;
  uint uVar19;
  int iVar20;
  int iVar21;
  uint uVar22;
  int iVar23;
  uint uVar24;
  int iVar25;
  int iVar26;
  bool bVar27;
  PackedArgb32 PVar28;
  undefined1 uVar30;
  undefined1 uVar31;
  undefined8 uVar29;
  PackedArgb32 PVar32;
  undefined1 uVar34;
  undefined1 uVar35;
  undefined8 uVar33;
  PackedArgb32 PVar36;
  undefined1 uVar38;
  undefined1 uVar39;
  undefined8 uVar37;
  TriangleBarycentricWeightsQ12 TVar40;
  GraphicsPrimitivePacketEaxCf5 GVar41;
  TerrainProjectedVertexWorkRecord *vertex2Projected;
  TerrainProjectedVertexWorkRecord *vertex1Projected;
  TerrainProjectedVertexWorkRecord *vertex0Projected;
  FrontendModelPointerContextRuntimeState17C *renderContext_00;
  
  uVar19 = vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags;
  if ((uVar19 & 0xff) != 0xff) {
    if ((uVar19 & 0x3e0000) == 0x1e0000) {
      bVar27 = false;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) == 0) {
        TVar40 = Triangle2D_ComputeBarycentricWeightsQ12Packed
                           ((vertex2->projectedPointA).projectedY,
                            (vertex2->projectedPointA).projectedX,
                            (vertex1->projectedPointA).projectedY,
                            (vertex1->projectedPointA).projectedX,
                            (vertex0->projectedPointA).projectedY,
                            (vertex0->projectedPointA).projectedX,renderContext->cursorWorldYQ12,
                            renderContext->cursorWorldXQ12);
        dVar2 = (vertex0->viewPointA).z;
        if ((!bVar27) && ((int)dVar2 < (int)renderContext->callbackArgumentF0)) {
          renderContext->callbackArgumentF0 = dVar2;
          iVar21 = (vertex1->sourcePoint).y;
          iVar23 = (vertex0->sourcePoint).y;
          iVar20 = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               (((vertex1->sourcePoint).x - (vertex0->sourcePoint).x) * TVar40.weightVertexB_Q12 >>
               0xc) + (vertex0->sourcePoint).x;
          renderContext->callbackArgumentEC =
               ((iVar21 - iVar23) * TVar40.weightVertexB_Q12 >> 0xc) + iVar20;
          iVar21 = (vertex2->sourcePoint).y;
          iVar23 = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               renderContext->callbackArgumentE8 +
               (((vertex2->sourcePoint).x - (vertex0->sourcePoint).x) * TVar40.weightVertexA_Q12 >>
               0xc);
          renderContext->callbackArgumentEC =
               renderContext->callbackArgumentEC +
               ((iVar21 - iVar23) * TVar40.weightVertexA_Q12 >> 0xc);
        }
      }
      pvVar18 = g_TerrainSoilPacketTablePayload;
      PVar28 = vertex0->shadedColorA;
      PVar32 = vertex1->shadedColorA;
      PVar36 = vertex2->shadedColorA;
      uVar30 = (undefined1)(PVar28 >> 0x18);
      uVar15 = CONCAT11(uVar30,uVar30);
      uVar31 = (undefined1)(PVar28 >> 0x10);
      uVar30 = (undefined1)(PVar28 >> 8);
      uVar34 = (undefined1)(PVar32 >> 0x18);
      uVar16 = CONCAT11(uVar34,uVar34);
      uVar35 = (undefined1)(PVar32 >> 0x10);
      uVar34 = (undefined1)(PVar32 >> 8);
      uVar38 = (undefined1)(PVar36 >> 0x18);
      uVar17 = CONCAT11(uVar38,uVar38);
      uVar39 = (undefined1)(PVar36 >> 0x10);
      uVar38 = (undefined1)(PVar36 >> 8);
      uVar19 = vertex0->secondaryProjectionDepthQ12;
      uVar24 = vertex1->secondaryProjectionDepthQ12;
      uVar22 = vertex2->secondaryProjectionDepthQ12;
      if ((int)uVar19 < 0) {
        uVar19 = 0;
      }
      if ((int)uVar24 < 0) {
        uVar24 = 0;
      }
      if ((int)uVar22 < 0) {
        uVar22 = 0;
      }
      iVar21 = vertex0->lightingLookupIndexOrSentinel - (uVar19 >> 1);
      if (iVar21 < 0) {
        iVar21 = 0;
      }
      iVar23 = vertex1->lightingLookupIndexOrSentinel - (uVar24 >> 1);
      if (iVar23 < 0) {
        iVar23 = 0;
      }
      iVar20 = vertex2->lightingLookupIndexOrSentinel - (uVar22 >> 1);
      if (iVar20 < 0) {
        iVar20 = 0;
      }
      uVar29 = pmulhw(CONCAT26(uVar15 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar15,uVar31),
                                                          CONCAT14(uVar31,PVar28)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar30,uVar30) >> 4,
                                                 CONCAT11((char)PVar28,(char)PVar28) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar21 * 8));
      uVar33 = pmulhw(CONCAT26(uVar16 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar16,uVar35),
                                                          CONCAT14(uVar35,PVar32)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar34,uVar34) >> 4,
                                                 CONCAT11((char)PVar32,(char)PVar32) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar23 * 8));
      uVar37 = pmulhw(CONCAT26(uVar17 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar17,uVar39),
                                                          CONCAT14(uVar39,PVar36)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar38,uVar38) >> 4,
                                                 CONCAT11((char)PVar36,(char)PVar36) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar20 * 8));
      sVar3 = (short)uVar29;
      sVar4 = (short)((ulonglong)uVar29 >> 0x10);
      sVar5 = (short)((ulonglong)uVar29 >> 0x20);
      sVar6 = (short)((ulonglong)uVar29 >> 0x30);
      PVar28 = CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar29 >> 0x30) -
                        (0xff < sVar6),
                        CONCAT12((0 < sVar5) * (sVar5 < 0x100) * (char)((ulonglong)uVar29 >> 0x20) -
                                 (0xff < sVar5),
                                 CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                          (char)((ulonglong)uVar29 >> 0x10) - (0xff < sVar4),
                                          (0 < sVar3) * (sVar3 < 0x100) * (char)uVar29 -
                                          (0xff < sVar3))));
      sVar3 = (short)uVar33;
      sVar4 = (short)((ulonglong)uVar33 >> 0x10);
      sVar5 = (short)((ulonglong)uVar33 >> 0x20);
      sVar6 = (short)((ulonglong)uVar33 >> 0x30);
      PVar32 = CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar33 >> 0x30) -
                        (0xff < sVar6),
                        CONCAT12((0 < sVar5) * (sVar5 < 0x100) * (char)((ulonglong)uVar33 >> 0x20) -
                                 (0xff < sVar5),
                                 CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                          (char)((ulonglong)uVar33 >> 0x10) - (0xff < sVar4),
                                          (0 < sVar3) * (sVar3 < 0x100) * (char)uVar33 -
                                          (0xff < sVar3))));
      sVar3 = (short)uVar37;
      sVar4 = (short)((ulonglong)uVar37 >> 0x10);
      sVar5 = (short)((ulonglong)uVar37 >> 0x20);
      sVar6 = (short)((ulonglong)uVar37 >> 0x30);
      PVar36 = CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                        (0xff < sVar6),
                        CONCAT12((0 < sVar5) * (sVar5 < 0x100) * (char)((ulonglong)uVar37 >> 0x20) -
                                 (0xff < sVar5),
                                 CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                          (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar4),
                                          (0 < sVar3) * (sVar3 < 0x100) * (char)uVar37 -
                                          (0xff < sVar3))));
      iVar26 = (vertex0->projectionFlags & 0xff) * 0x800;
      iVar25 = (vertex1->projectionFlags & 0xff) * 0x800;
      iVar20 = (vertex2->projectionFlags & 0xff) * 0x800;
      iVar21 = iVar25 + (vertex1->projectionFlags & 0x700);
      iVar23 = iVar20 + (vertex2->projectionFlags & 0x700);
      vertex2Projected = vertex2;
      vertex1Projected = vertex1;
      vertex0Projected = vertex0;
      renderContext_00 = renderContext;
      GVar41 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                         ((dword *)((int)g_TerrainSoilPacketTablePayload +
                                   iVar26 + (vertex0->projectionFlags & 0x700)),PVar36,PVar32,PVar28
                          ,(GraphicsProjectedVertexSource *)vertex2,
                          (GraphicsProjectedVertexSource *)vertex1,
                          (GraphicsProjectedVertexSource *)vertex0,renderContext);
      if (!GVar41.carry) {
        if (iVar26 == iVar25) {
          if (iVar26 != iVar20) {
            GVar41 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               ((dword *)((int)pvVar18 + iVar23 + 0x20),PVar36,PVar32,PVar28,
                                (GraphicsProjectedVertexSource *)vertex2Projected,
                                (GraphicsProjectedVertexSource *)vertex1Projected,
                                (GraphicsProjectedVertexSource *)vertex0Projected,renderContext_00);
            if (!GVar41.carry) {
              pGVar1 = &(GVar41.packet)->renderFlags;
              *pGVar1 = *pGVar1 | 0x10020000;
            }
          }
        }
        else if (iVar26 == iVar20) {
          GVar41 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((dword *)((int)pvVar18 + iVar21 + 0x40),PVar36,PVar32,PVar28,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,renderContext_00);
          if (!GVar41.carry) {
            pGVar1 = &(GVar41.packet)->renderFlags;
            *pGVar1 = *pGVar1 | 0x10020000;
          }
        }
        else if (iVar25 == iVar20) {
          GVar41 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((dword *)((int)pvVar18 + iVar21 + 0x60),PVar36,PVar32,PVar28,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,renderContext_00);
          if (!GVar41.carry) {
            pGVar1 = &(GVar41.packet)->renderFlags;
            *pGVar1 = *pGVar1 | 0x10020000;
          }
        }
        else {
          GVar41 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             ((dword *)((int)pvVar18 + iVar21 + 0x80),PVar36,PVar32,PVar28,
                              (GraphicsProjectedVertexSource *)vertex2Projected,
                              (GraphicsProjectedVertexSource *)vertex1Projected,
                              (GraphicsProjectedVertexSource *)vertex0Projected,renderContext_00);
          if (!GVar41.carry) {
            pGVar1 = &(GVar41.packet)->renderFlags;
            *pGVar1 = *pGVar1 | 0x10020000;
            GVar41 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               ((dword *)((int)pvVar18 + iVar23 + 0xa0),PVar36,PVar32,PVar28,
                                (GraphicsProjectedVertexSource *)vertex2Projected,
                                (GraphicsProjectedVertexSource *)vertex1Projected,
                                (GraphicsProjectedVertexSource *)vertex0Projected,renderContext_00);
            if (!GVar41.carry) {
              pGVar1 = &(GVar41.packet)->renderFlags;
              *pGVar1 = *pGVar1 | 0x20020000;
            }
          }
        }
      }
    }
    if ((((((renderContext->contextFlags & 0x1000000) != 0) ||
          (0 < vertex0->secondaryProjectionDepthQ12)) || (0 < vertex1->secondaryProjectionDepthQ12))
        || (0 < vertex2->secondaryProjectionDepthQ12)) &&
       (((vertex0->projectionFlags | vertex1->projectionFlags | vertex2->projectionFlags) &
        0x7c00000) == 0x3c00000)) {
      vertex0->projectionFlags = vertex0->projectionFlags | 0x10000000;
      vertex1->projectionFlags = vertex1->projectionFlags | 0x10000000;
      vertex2->projectionFlags = vertex2->projectionFlags | 0x10000000;
      bVar27 = false;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
        TVar40 = Triangle2D_ComputeBarycentricWeightsQ12Packed
                           ((vertex2->projectedPointB).projectedY,
                            (vertex2->projectedPointB).projectedX,
                            (vertex1->projectedPointB).projectedY,
                            (vertex1->projectedPointB).projectedX,
                            (vertex0->projectedPointB).projectedY,
                            (vertex0->projectedPointB).projectedX,renderContext->cursorWorldYQ12,
                            renderContext->cursorWorldXQ12);
        dVar2 = (vertex0->viewPointB).z;
        if ((!bVar27) && ((int)dVar2 < (int)renderContext->callbackArgumentF0)) {
          renderContext->callbackArgumentF0 = dVar2;
          iVar21 = (vertex1->sourcePoint).y;
          iVar23 = (vertex0->sourcePoint).y;
          iVar20 = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               (((vertex1->sourcePoint).x - (vertex0->sourcePoint).x) * TVar40.weightVertexB_Q12 >>
               0xc) + (vertex0->sourcePoint).x;
          renderContext->callbackArgumentEC =
               ((iVar21 - iVar23) * TVar40.weightVertexB_Q12 >> 0xc) + iVar20;
          iVar21 = (vertex2->sourcePoint).y;
          iVar23 = (vertex0->sourcePoint).y;
          renderContext->callbackArgumentE8 =
               renderContext->callbackArgumentE8 +
               (((vertex2->sourcePoint).x - (vertex0->sourcePoint).x) * TVar40.weightVertexA_Q12 >>
               0xc);
          renderContext->callbackArgumentEC =
               renderContext->callbackArgumentEC +
               ((iVar21 - iVar23) * TVar40.weightVertexA_Q12 >> 0xc);
        }
      }
      PVar28 = vertex0->shadedColorB;
      PVar32 = vertex1->shadedColorB;
      PVar36 = vertex2->shadedColorB;
      uVar30 = (undefined1)(PVar28 >> 0x18);
      uVar15 = CONCAT11(uVar30,uVar30);
      uVar31 = (undefined1)(PVar28 >> 0x10);
      uVar30 = (undefined1)(PVar28 >> 8);
      uVar34 = (undefined1)(PVar32 >> 0x18);
      uVar16 = CONCAT11(uVar34,uVar34);
      uVar35 = (undefined1)(PVar32 >> 0x10);
      uVar34 = (undefined1)(PVar32 >> 8);
      uVar38 = (undefined1)(PVar36 >> 0x18);
      uVar17 = CONCAT11(uVar38,uVar38);
      uVar39 = (undefined1)(PVar36 >> 0x10);
      uVar38 = (undefined1)(PVar36 >> 8);
      uVar29 = pmulhw(CONCAT26(uVar15 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar15,uVar31),
                                                          CONCAT14(uVar31,PVar28)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar30,uVar30) >> 4,
                                                 CONCAT11((char)PVar28,(char)PVar28) >> 4))),
                      *(undefined8 *)
                       (&g_PackedLightingLookupTable + vertex0->lightingLookupIndexOrSentinel * 8));
      uVar33 = pmulhw(CONCAT26(uVar16 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar16,uVar35),
                                                          CONCAT14(uVar35,PVar32)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar34,uVar34) >> 4,
                                                 CONCAT11((char)PVar32,(char)PVar32) >> 4))),
                      *(undefined8 *)
                       (&g_PackedLightingLookupTable + vertex1->lightingLookupIndexOrSentinel * 8));
      uVar37 = pmulhw(CONCAT26(uVar17 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar17,uVar39),
                                                          CONCAT14(uVar39,PVar36)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar38,uVar38) >> 4,
                                                 CONCAT11((char)PVar36,(char)PVar36) >> 4))),
                      *(undefined8 *)
                       (&g_PackedLightingLookupTable + vertex2->lightingLookupIndexOrSentinel * 8));
      sVar3 = (short)uVar29;
      sVar6 = (short)((ulonglong)uVar29 >> 0x10);
      sVar9 = (short)((ulonglong)uVar29 >> 0x20);
      sVar12 = (short)((ulonglong)uVar29 >> 0x30);
      sVar4 = (short)uVar33;
      sVar7 = (short)((ulonglong)uVar33 >> 0x10);
      sVar10 = (short)((ulonglong)uVar33 >> 0x20);
      sVar13 = (short)((ulonglong)uVar33 >> 0x30);
      sVar5 = (short)uVar37;
      sVar8 = (short)((ulonglong)uVar37 >> 0x10);
      sVar11 = (short)((ulonglong)uVar37 >> 0x20);
      sVar14 = (short)((ulonglong)uVar37 >> 0x30);
      GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangleCf
                ((dword *)(surfacePacketIndex * 0x20 + (int)g_TerrainSurfacePacketTablePayload),
                 CONCAT13((0 < sVar14) * (sVar14 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                          (0xff < sVar14),
                          CONCAT12((0 < sVar11) * (sVar11 < 0x100) *
                                   (char)((ulonglong)uVar37 >> 0x20) - (0xff < sVar11),
                                   CONCAT11((0 < sVar8) * (sVar8 < 0x100) *
                                            (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar8),
                                            (0 < sVar5) * (sVar5 < 0x100) * (char)uVar37 -
                                            (0xff < sVar5)))),
                 CONCAT13((0 < sVar13) * (sVar13 < 0x100) * (char)((ulonglong)uVar33 >> 0x30) -
                          (0xff < sVar13),
                          CONCAT12((0 < sVar10) * (sVar10 < 0x100) *
                                   (char)((ulonglong)uVar33 >> 0x20) - (0xff < sVar10),
                                   CONCAT11((0 < sVar7) * (sVar7 < 0x100) *
                                            (char)((ulonglong)uVar33 >> 0x10) - (0xff < sVar7),
                                            (0 < sVar4) * (sVar4 < 0x100) * (char)uVar33 -
                                            (0xff < sVar4)))),
                 CONCAT13((0 < sVar12) * (sVar12 < 0x100) * (char)((ulonglong)uVar29 >> 0x30) -
                          (0xff < sVar12),
                          CONCAT12((0 < sVar9) * (sVar9 < 0x100) * (char)((ulonglong)uVar29 >> 0x20)
                                   - (0xff < sVar9),
                                   CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                            (char)((ulonglong)uVar29 >> 0x10) - (0xff < sVar6),
                                            (0 < sVar3) * (sVar3 < 0x100) * (char)uVar29 -
                                            (0xff < sVar3)))),
                 (GraphicsProjectedVertexSource *)vertex2,(GraphicsProjectedVertexSource *)vertex1,
                 (GraphicsProjectedVertexSource *)vertex0,renderContext);
    }
  }
  return;
}


/* Address: 0x00500D30.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected grid clip row spans against plane.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainProjectedGrid_ClipRowSpansAgainstPlane
          (FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  FieldGridDimension FVar5;
  int *piVar6;
  TerrainProjectedRowSpan *pTVar7;
  
  if (planeNormal->x == 0) {
    if (planeNormal->y != 0) {
      if (planeNormal->y < 0) {
        iVar2 = 0;
        if (-1 < planeNormal->z) {
          iVar2 = (int)(((longlong)g_ViewOriginFixed.z * (longlong)planeNormal->z) /
                       (longlong)planeNormal->y);
        }
        lVar1 = (longlong)(iVar2 + g_ViewOriginFixed.y) * -0x20c8cc;
        iVar4 = (int)((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) >> 0xc;
        iVar2 = fieldGrid->gridHeight - iVar4;
        if ((iVar2 != 0 && iVar4 <= (int)fieldGrid->gridHeight) && (iVar2 = iVar2 + -1, iVar2 != 0))
        {
          pTVar7 = g_TerrainProjectedRowSpans + iVar4 + 3;
          for (iVar2 = iVar2 * 2; iVar2 != 0; iVar2 = iVar2 + -1) {
            pTVar7->firstColumn = 0;
            pTVar7 = (TerrainProjectedRowSpan *)&pTVar7->endColumnExclusive;
          }
        }
      }
      else {
        iVar2 = 0;
        if (-1 < planeNormal->z) {
          iVar2 = (int)(((longlong)g_ViewOriginFixed.z * (longlong)planeNormal->z) /
                       (longlong)planeNormal->y);
        }
        lVar1 = (longlong)(iVar2 + g_ViewOriginFixed.y) * -0x20c8cc;
        iVar2 = (int)((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) >> 0xc;
        if ((-1 < iVar2) && (iVar2 != 0)) {
          pTVar7 = g_TerrainProjectedRowSpans;
          for (iVar2 = iVar2 * 2; iVar2 != 0; iVar2 = iVar2 + -1) {
            pTVar7->firstColumn = 0;
            pTVar7 = (TerrainProjectedRowSpan *)&pTVar7->endColumnExclusive;
          }
        }
      }
    }
  }
  else if (planeNormal->x < 0) {
    lVar1 = (longlong)planeNormal->y * (longlong)g_ViewOriginFixed.y +
            (longlong)planeNormal->x * (longlong)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      lVar1 = lVar1 + (longlong)planeNormal->z * (longlong)g_ViewOriginFixed.z;
    }
    lVar1 = (longlong)(int)(lVar1 / (longlong)planeNormal->x) * 0x1c6e9c;
    uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14;
    lVar1 = (longlong)(int)(((longlong)planeNormal->y * 1999) / (longlong)planeNormal->x) * 0x1c6e9c
    ;
    piVar6 = (int *)0x4ffc78;
    FVar5 = fieldGrid->gridHeight;
    do {
      piVar6 = piVar6 + 2;
      iVar2 = (int)(uVar3 - 0x1000) >> 0xc;
      uVar3 = uVar3 + (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - 0x800);
      if (*piVar6 < iVar2) {
        *piVar6 = iVar2;
      }
      FVar5 = FVar5 - 1;
    } while (FVar5 != 0);
  }
  else {
    lVar1 = (longlong)planeNormal->y * (longlong)g_ViewOriginFixed.y +
            (longlong)planeNormal->x * (longlong)g_ViewOriginFixed.x;
    if (-1 < planeNormal->z) {
      lVar1 = lVar1 + (longlong)planeNormal->z * (longlong)g_ViewOriginFixed.z;
    }
    lVar1 = (longlong)(int)(lVar1 / (longlong)planeNormal->x) * 0x1c6e9c;
    uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14;
    lVar1 = (longlong)(int)(((longlong)planeNormal->y * 1999) / (longlong)planeNormal->x) * 0x1c6e9c
    ;
    piVar6 = (int *)0x4ffc7c;
    FVar5 = fieldGrid->gridHeight;
    do {
      piVar6 = piVar6 + 2;
      iVar2 = (int)(uVar3 + 0x1fff) >> 0xc;
      uVar3 = uVar3 + (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - 0x800);
      if (iVar2 < *piVar6) {
        *piVar6 = iVar2;
      }
      FVar5 = FVar5 - 1;
    } while (FVar5 != 0);
  }
  return;
}


/* Address: 0x005063B0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 0, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection0
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = cell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506430.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 1, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection1
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005064C0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 2, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection2
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506540.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 3, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection3
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = cell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005065C0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 4, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection4
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506650.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 5, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx_mm0
TerrainProjectedOcclusion_ScanDirection5
          (ulonglong occupancyMaskBits,
          TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
          TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar2 = cell->terrainHeight;
      if (0 < cell->waterSurfaceDelta) {
        iVar2 = iVar2 + cell->waterSurfaceDelta;
      }
      lVar1 = (longlong)(iVar2 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + scanStep * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell->occupancyMask = cell->occupancyMask | occupancyMaskBits;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509320.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509380.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005093F0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509450.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x005094B0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc)
      ;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509520.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantA_ApplyDirection5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509B90.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection0
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509BF0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection1
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509C50.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection2
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509CB0.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection3
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = fieldCell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509D10.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection4
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc)
      ;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00509D70.
   Ownership: world/terrain/projection.
   Purpose: Transitive terrain-overlay cell helper; exact two-stack-argument contract. Typed parameters: p0
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridTerrainOverlayVariantB_ApplyDirection5
          (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        fieldCell->runtimeOverlayOrHeightValue04 = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

