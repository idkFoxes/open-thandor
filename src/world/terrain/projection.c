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
undefined8 __fastcall
TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 in_EAX;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int extraout_EDX_03;
  FieldGridCell *pFVar8;
  FieldGridCell *pFVar9;
  ulonglong extraout_MM0;
  qword qVar10;
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
    qVar10 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    iVar3 = g_TerrainScanReferenceHeight;
    uVar1 = (int)qVar10 >> 0xc;
    uVar11 = (uint)((longlong)qVar10 >> 0x2c);
    uVar4 = (uint)(qVar10 & 0xfff00000fff);
    uVar6 = (uint)((qVar10 & 0xfff00000fff) >> 0x20);
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
        fieldGrid->cells[iVar7].occupancyMask = fieldGrid->cells[iVar7].occupancyMask | extraout_MM0
        ;
        iVar2 = g_TerrainScanRowStrideBytes;
        pFVar8 = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 iVar7 * 0x80 + -0x28);
        pFVar9 = (FieldGridCell *)((int)pFVar8 - g_TerrainScanRowStrideBytes);
        TerrainProjectedOcclusion_TraceWedge0(pFVar9->terrainHeight - iVar3,0,pFVar8);
        TerrainProjectedOcclusion_TraceWedge1(pFVar9[-1].terrainHeight - extraout_EDX,0,pFVar9);
        pFVar8 = (FieldGridCell *)((pFVar9 + -1)[-1].runtime00_07 + iVar2);
        TerrainProjectedOcclusion_TraceWedge2(pFVar8->terrainHeight - extraout_EDX_00,0,pFVar9 + -1)
        ;
        pFVar9 = (FieldGridCell *)(pFVar8->runtime00_07 + iVar2);
        TerrainProjectedOcclusion_TraceWedge3(pFVar9->terrainHeight - extraout_EDX_01,0,pFVar8);
        TerrainProjectedOcclusion_TraceWedge4(pFVar9[1].terrainHeight - extraout_EDX_02,0,pFVar9);
        TerrainProjectedOcclusion_TraceWedge5
                  (*(int *)((int)(pFVar9 + 1) + (200 - iVar2)) - extraout_EDX_03,0,pFVar9 + 1);
      }
    }
  }
  return CONCAT44(preservedEdxValue,in_EAX);
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
void FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf
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
  qword qVar9;
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
    qVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    uVar1 = (int)qVar9 >> 0xc;
    uVar10 = (uint)((longlong)qVar9 >> 0x2c);
    uVar3 = (uint)(qVar9 & 0xfff00000fff);
    uVar5 = (uint)((qVar9 & 0xfff00000fff) >> 0x20);
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
        *(TerrainOverlayCellRuntimeValue *)(fieldGrid->cells[iVar6].runtime00_07 + 4) = cellValue;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      pFVar7 = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               iVar6 * 0x80 + -0x28);
      pFVar8 = (FieldGridCell *)((int)pFVar7 - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyWedge0(0,pFVar7);
      fieldCell = pFVar8 + -1;
      FieldGridTerrainOverlayVariantA_ApplyWedge1(0,pFVar8);
      pFVar7 = (FieldGridCell *)(fieldCell[-1].runtime00_07 + iVar2);
      FieldGridTerrainOverlayVariantA_ApplyWedge2(0,fieldCell);
      pFVar8 = (FieldGridCell *)(pFVar7->runtime00_07 + iVar2);
      FieldGridTerrainOverlayVariantA_ApplyWedge3(0,pFVar7);
      FieldGridTerrainOverlayVariantA_ApplyWedge4(0,pFVar8);
      FieldGridTerrainOverlayVariantA_ApplyWedge5(0,pFVar8 + 1);
      return;
    }
  }
  return;
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
void FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf
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
  qword qVar9;
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
    qVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    uVar1 = (int)qVar9 >> 0xc;
    uVar10 = (uint)((longlong)qVar9 >> 0x2c);
    uVar3 = (uint)(qVar9 & 0xfff00000fff);
    uVar5 = (uint)((qVar9 & 0xfff00000fff) >> 0x20);
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
        *(TerrainOverlayCellRuntimeValue *)(fieldGrid->cells[iVar6].runtime00_07 + 4) = cellValue;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      pFVar7 = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               iVar6 * 0x80 + -0x28);
      pFVar8 = (FieldGridCell *)((int)pFVar7 - g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyWedge0(0,pFVar7);
      fieldCell = pFVar8 + -1;
      FieldGridTerrainOverlayVariantB_ApplyWedge1(0,pFVar8);
      pFVar7 = (FieldGridCell *)(fieldCell[-1].runtime00_07 + iVar2);
      FieldGridTerrainOverlayVariantB_ApplyWedge2(0,fieldCell);
      pFVar8 = (FieldGridCell *)(pFVar7->runtime00_07 + iVar2);
      FieldGridTerrainOverlayVariantB_ApplyWedge3(0,pFVar7);
      FieldGridTerrainOverlayVariantB_ApplyWedge4(0,pFVar8);
      FieldGridTerrainOverlayVariantB_ApplyWedge5(0,pFVar8 + 1);
      return;
    }
  }
  return;
}

/* Address: 0x00500F50.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected grid transform shade and queue.
   Local calls: TerrainProjectedGrid_ClipRowSpansAgainstPlane,
   TerrainProjectedVertex_TransformProjectAndShadeVariantB,
   TerrainProjectedVertex_TransformProjectAndShadeVariantA, TerrainProjectedQuad_QueueAsTwoTrianglesRegs.
*/
undefined8 TerrainProjectedGrid_TransformShadeAndQueue(int param_1,GraphicsPrimitiveQueue *param_2)

{
  undefined4 uVar1;
  undefined4 in_EAX;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 in_EDX;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  undefined4 *puVar7;
  int iVar8;
  undefined8 uVar9;
  
  if ((param_2[1].reserved1C & 0x800) == 0) {
    puVar7 = &DAT_004ffc80;
    uVar1 = *(undefined4 *)(param_1 + 0xb8);
    iVar2 = *(int *)(param_1 + 0xbc);
    do {
      *puVar7 = 0;
      puVar7[1] = uVar1;
      puVar7 = puVar7 + 2;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(param_1,&g_FrustumPlaneNormalFixed_0[0].x);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(param_1,&g_FrustumPlaneNormalFixed_0[1].x);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(param_1,&g_FrustumPlaneNormalFixed_0[2].x);
    TerrainProjectedGrid_ClipRowSpansAgainstPlane(param_1,&g_FrustumPlaneNormalFixed_0[3].x);
    iVar2 = *(int *)(param_1 + 0xb8);
    iVar8 = *(int *)(param_1 + 0xbc);
    iVar5 = param_1 + 0x200;
    iVar3 = iVar8;
    iVar4 = iVar2;
    do {
      do {
        *(uint *)(iVar5 + 0x50) = *(uint *)(iVar5 + 0x50) | 0x4200000;
        iVar5 = iVar5 + 0x80;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      iVar3 = iVar3 + -1;
      iVar4 = iVar2;
    } while (iVar3 != 0);
    piVar6 = (int *)0x4ffc88;
    iVar8 = iVar8 + -1;
    iVar2 = DAT_004ffc80;
    iVar3 = DAT_004ffc84;
    do {
      iVar4 = *piVar6;
      iVar5 = piVar6[1];
      if (iVar3 == 0) {
        piVar6[-2] = iVar4;
        piVar6[-1] = iVar5;
      }
      else if (iVar5 == 0) {
        *piVar6 = iVar2;
        piVar6[1] = iVar3;
      }
      else {
        if (iVar2 < iVar4) {
          *piVar6 = iVar2;
        }
        else if (iVar4 < piVar6[-2]) {
          piVar6[-2] = iVar4;
        }
        if (iVar5 < iVar3) {
          piVar6[1] = iVar3;
        }
        else if (piVar6[-1] < iVar5) {
          piVar6[-1] = iVar5;
        }
      }
      piVar6 = piVar6 + 2;
      iVar8 = iVar8 + -1;
      iVar2 = iVar4;
      iVar3 = iVar5;
    } while (iVar8 != 0);
  }
  piVar6 = &DAT_004ffc80;
  iVar2 = *(int *)(param_1 + 0xbc);
  iVar8 = *(int *)(param_1 + 0xb8) * 0x80;
  if (((*(uint *)(param_1 + 0xb4) & 1) == 0) && ((param_2[1].reserved1C & 0x800) != 0)) {
    iVar3 = param_1 + 0x200;
    do {
      if (*piVar6 < piVar6[1]) {
        iVar4 = iVar3 + *piVar6 * 0x80;
        do {
          uVar9 = TerrainProjectedVertex_TransformProjectAndShadeVariantB(iVar4);
          iVar2 = (int)((ulonglong)uVar9 >> 0x20);
          iVar4 = iVar4 + 0x80;
        } while (extraout_ECX != 1);
      }
      piVar6 = piVar6 + 2;
      iVar3 = iVar3 + iVar8;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  else {
    *(uint *)(param_1 + 0xb4) = *(uint *)(param_1 + 0xb4) & 0xfffffffe;
    param_2[1].reserved1C = param_2[1].reserved1C & 0xfffff7ff;
    iVar3 = param_1 + 0x200;
    do {
      if (*piVar6 < piVar6[1]) {
        iVar4 = iVar3 + *piVar6 * 0x80;
        do {
          uVar9 = TerrainProjectedVertex_TransformProjectAndShadeVariantA(iVar4);
          iVar2 = (int)((ulonglong)uVar9 >> 0x20);
          iVar4 = iVar4 + 0x80;
        } while (extraout_ECX_00 != 1);
      }
      piVar6 = piVar6 + 2;
      iVar3 = iVar3 + iVar8;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  piVar6 = &DAT_004ffc80;
  iVar2 = *(int *)(param_1 + 0xbc) + -1;
  do {
    iVar8 = piVar6[1] - *piVar6;
    if (*piVar6 < piVar6[1]) {
      while (iVar8 != 1) {
        uVar9 = TerrainProjectedQuad_QueueAsTwoTrianglesRegs(param_2);
        iVar2 = (int)((ulonglong)uVar9 >> 0x20);
        iVar8 = extraout_ECX_01;
      }
    }
    piVar6 = piVar6 + 2;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return CONCAT44(in_EDX,in_EAX);
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
void TerrainProjectedOcclusion_TraceWedge0
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar4;
  TerrainProjectedHeightThresholdQ20 TVar5;
  FieldGridCell *cell_00;
  ulonglong in_MM0;
  ulonglong extraout_MM0;
  ulonglong extraout_MM0_00;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar4;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + 1;
      TerrainProjectedOcclusion_ScanDirection0(projectedHeightThresholdQ20,scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
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
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + extraout_ECX * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar4) {
        *(ulonglong *)((int)cell_00 + (0x70 - iVar2)) =
             *(ulonglong *)((int)cell_00 + (0x70 - iVar2)) | extraout_MM0;
        projectedHeightThresholdQ20 = uVar4;
      }
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - iVar2));
      TerrainProjectedOcclusion_ScanDirection1
                (projectedHeightThresholdQ20,extraout_ECX + 3,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      TVar5 = projectedHeightThresholdQ20;
      in_MM0 = extraout_MM0_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainProjectedOcclusion_TraceWedge1
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar4;
  TerrainProjectedHeightThresholdQ20 TVar5;
  FieldGridCell *cell_00;
  ulonglong in_MM0;
  ulonglong extraout_MM0;
  ulonglong extraout_MM0_00;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar4;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      TerrainProjectedOcclusion_ScanDirection1
                (projectedHeightThresholdQ20,scanStep + 4,
                 (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
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
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + extraout_ECX * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar4) {
        *(ulonglong *)((int)cell + (0x70 - iVar2)) =
             *(ulonglong *)((int)cell + (0x70 - iVar2)) | extraout_MM0;
        projectedHeightThresholdQ20 = uVar4;
      }
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar2));
      cell = cell_00 + 1;
      TerrainProjectedOcclusion_ScanDirection2(projectedHeightThresholdQ20,extraout_ECX + 3,cell_00)
      ;
      scanStep = extraout_ECX_00;
      TVar5 = projectedHeightThresholdQ20;
      in_MM0 = extraout_MM0_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainProjectedOcclusion_TraceWedge2
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  longlong lVar1;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar3;
  TerrainProjectedHeightThresholdQ20 TVar4;
  ulonglong in_MM0;
  ulonglong extraout_MM0;
  ulonglong extraout_MM0_00;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar3;
      }
      TerrainProjectedOcclusion_ScanDirection2
                (projectedHeightThresholdQ20,scanStep + 4,
                 (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
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
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + extraout_ECX * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell[-1].occupancyMask = cell[-1].occupancyMask | extraout_MM0;
        projectedHeightThresholdQ20 = uVar3;
      }
      cell_00 = cell + -2;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      TerrainProjectedOcclusion_ScanDirection3(projectedHeightThresholdQ20,extraout_ECX + 3,cell_00)
      ;
      scanStep = extraout_ECX_00;
      TVar4 = projectedHeightThresholdQ20;
      in_MM0 = extraout_MM0_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainProjectedOcclusion_TraceWedge3
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar4;
  TerrainProjectedHeightThresholdQ20 TVar5;
  FieldGridCell *cell_00;
  ulonglong in_MM0;
  ulonglong extraout_MM0;
  ulonglong extraout_MM0_00;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar4;
      }
      iVar2 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + -1;
      TerrainProjectedOcclusion_ScanDirection3(projectedHeightThresholdQ20,scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell_00->runtime58_6F + iVar2 + -8) & 0x88006000) != 0) {
        return;
      }
      iVar3 = *(int *)(cell_00->runtime58_6F + iVar2 + -0x10);
      if (0 < *(int *)(cell_00->runtime58_6F + iVar2 + -0xc)) {
        iVar3 = iVar3 + *(int *)(cell_00->runtime58_6F + iVar2 + -0xc);
      }
      lVar1 = (longlong)(iVar3 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + extraout_ECX * 4);
      uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar4) {
        *(ulonglong *)(cell_00->runtime58_6F + iVar2 + 0x18) =
             *(ulonglong *)(cell_00->runtime58_6F + iVar2 + 0x18) | extraout_MM0;
        projectedHeightThresholdQ20 = uVar4;
      }
      cell = (FieldGridCell *)(cell_00[-1].runtime00_07 + iVar2);
      TerrainProjectedOcclusion_ScanDirection4
                (projectedHeightThresholdQ20,extraout_ECX + 3,
                 (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      TVar5 = projectedHeightThresholdQ20;
      in_MM0 = extraout_MM0_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainProjectedOcclusion_TraceWedge4
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar5;
  TerrainProjectedHeightThresholdQ20 TVar6;
  ulonglong in_MM0;
  ulonglong extraout_MM0;
  ulonglong extraout_MM0_00;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar5;
      }
      iVar3 = g_TerrainScanRowStrideBytes;
      TerrainProjectedOcclusion_ScanDirection4
                (projectedHeightThresholdQ20,scanStep + 4,
                 (FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell->runtime58_6F + iVar3 + -8) & 0x88006000) != 0) {
        return;
      }
      iVar4 = *(int *)(cell->runtime58_6F + iVar3 + -0x10);
      if (0 < *(int *)(cell->runtime58_6F + iVar3 + -0xc)) {
        iVar4 = iVar4 + *(int *)(cell->runtime58_6F + iVar3 + -0xc);
      }
      lVar1 = (longlong)(iVar4 - g_TerrainScanReferenceHeight) *
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + extraout_ECX * 4);
      uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar5) {
        *(ulonglong *)(cell->runtime58_6F + iVar3 + 0x18) =
             *(ulonglong *)(cell->runtime58_6F + iVar3 + 0x18) | extraout_MM0;
        projectedHeightThresholdQ20 = uVar5;
      }
      pbVar2 = cell->runtime00_07;
      cell = (FieldGridCell *)(pbVar2 + g_TerrainScanRowStrideBytes + iVar3) + -1;
      TerrainProjectedOcclusion_ScanDirection5
                (projectedHeightThresholdQ20,extraout_ECX + 3,
                 (FieldGridCell *)(pbVar2 + g_TerrainScanRowStrideBytes + iVar3));
      scanStep = extraout_ECX_00;
      TVar6 = projectedHeightThresholdQ20;
      in_MM0 = extraout_MM0_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainProjectedOcclusion_TraceWedge5
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  longlong lVar1;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar3;
  TerrainProjectedHeightThresholdQ20 TVar4;
  ulonglong in_MM0;
  ulonglong extraout_MM0;
  ulonglong extraout_MM0_00;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar3;
      }
      TerrainProjectedOcclusion_ScanDirection5
                (projectedHeightThresholdQ20,scanStep + 4,
                 (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
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
              (longlong)*(int *)(&g_TerrainHeightDeltaScaleByStepQ12 + extraout_ECX * 4);
      uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)projectedHeightThresholdQ20 <= (int)uVar3) {
        cell[1].occupancyMask = cell[1].occupancyMask | extraout_MM0;
        projectedHeightThresholdQ20 = uVar3;
      }
      cell_00 = cell + 2;
      cell = (FieldGridCell *)(cell[1].runtime00_07 + g_TerrainScanRowStrideBytes);
      TerrainProjectedOcclusion_ScanDirection0(projectedHeightThresholdQ20,extraout_ECX + 3,cell_00)
      ;
      scanStep = extraout_ECX_00;
      TVar4 = projectedHeightThresholdQ20;
      in_MM0 = extraout_MM0_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantA_ApplyWedge0
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection0(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)fieldCell_00 + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)((int)fieldCell_00 + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell_00 + (0x4c - rowStrideBytes)) < 0)) {
        *(undefined4 *)((int)fieldCell_00 + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)fieldCell_00 + (0x80 - rowStrideBytes));
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (extraout_ECX + 3,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantA_ApplyWedge1
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection1
                (scanStep + 4,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)((int)fieldCell + (0x4c - rowStrideBytes)) < 0)) {
        *(undefined4 *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      fieldCell = fieldCell_00 + 1;
      FieldGridTerrainOverlayVariantA_ApplyDirection2(extraout_ECX + 3,fieldCell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantA_ApplyWedge2
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  uint extraout_ECX;
  uint extraout_ECX_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell[-1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) !=
           0) && (fieldCell[-1].waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell[-1].runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + -2;
      fieldCell = (FieldGridCell *)((int)fieldCell + (-0x80 - g_TerrainScanRowStrideBytes));
      FieldGridTerrainOverlayVariantA_ApplyDirection3(extraout_ECX + 3,fieldCell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantA_ApplyWedge3
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + -1;
      FieldGridTerrainOverlayVariantA_ApplyDirection3(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(fieldCell_00->runtime58_6F + rowStrideBytes + -8) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)(fieldCell_00->runtime58_6F + rowStrideBytes + -8) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell_00->runtime58_6F + rowStrideBytes + -0xc) < 0)) {
        *(undefined4 *)(fieldCell_00->runtime00_07 + rowStrideBytes + 4) =
             g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(fieldCell_00[-1].runtime00_07 + rowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (extraout_ECX + 3,
                 (FieldGridCell *)(fieldCell->runtime00_07 + g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantA_ApplyWedge4
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  byte *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantA_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell[-1].runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(fieldCell->runtime58_6F + rowStrideBytes + -8) & 0x88006000) != 0) {
        return;
      }
      if (((*(uint *)(fieldCell->runtime58_6F + rowStrideBytes + -8) &
           g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0) &&
         (*(int *)(fieldCell->runtime58_6F + rowStrideBytes + -0xc) < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + rowStrideBytes + 4) = g_TerrainScanReferenceHeight
        ;
      }
      currentCellRuntimeBase = fieldCell->runtime00_07;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes) + -1;
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (extraout_ECX + 3,
                 (FieldGridCell *)
                 (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantA_ApplyWedge5
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  uint extraout_ECX;
  uint extraout_ECX_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantA_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell[1].flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0
          ) && (fieldCell[1].waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell[1].runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + 2;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime00_07 + g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantA_ApplyDirection0(extraout_ECX + 3,fieldCell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantB_ApplyWedge0
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection0(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)fieldCell_00 + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell_00 + (0x4c - rowStrideBytes))) {
        *(undefined4 *)((int)fieldCell_00 + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)((int)fieldCell_00 + (0x80 - rowStrideBytes));
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (extraout_ECX + 3,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantB_ApplyWedge1
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection1
                (scanStep + 4,
                 (FieldGridCell *)((int)fieldCell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)fieldCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)((int)fieldCell + (0x4c - rowStrideBytes))) {
        *(undefined4 *)((int)fieldCell + (4 - rowStrideBytes)) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = (FieldGridCell *)
                     ((int)fieldCell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      fieldCell = fieldCell_00 + 1;
      FieldGridTerrainOverlayVariantB_ApplyDirection2(extraout_ECX + 3,fieldCell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantB_ApplyWedge2
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  uint extraout_ECX;
  uint extraout_ECX_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)fieldCell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((fieldCell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell[-1].waterSurfaceDelta) {
        *(undefined4 *)(fieldCell[-1].runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + -2;
      fieldCell = (FieldGridCell *)((int)fieldCell + (-0x80 - g_TerrainScanRowStrideBytes));
      FieldGridTerrainOverlayVariantB_ApplyDirection3(extraout_ECX + 3,fieldCell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantB_ApplyWedge3
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *fieldCell_00;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      fieldCell_00 = fieldCell + -1;
      FieldGridTerrainOverlayVariantB_ApplyDirection3(scanStep + 4,fieldCell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(fieldCell_00->runtime58_6F + rowStrideBytes + -8) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell_00->runtime58_6F + rowStrideBytes + -0xc)) {
        *(undefined4 *)(fieldCell_00->runtime00_07 + rowStrideBytes + 4) =
             g_TerrainScanReferenceHeight;
      }
      fieldCell = (FieldGridCell *)(fieldCell_00[-1].runtime00_07 + rowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (extraout_ECX + 3,
                 (FieldGridCell *)(fieldCell->runtime00_07 + g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantB_ApplyWedge4
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  byte *currentCellRuntimeBase;
  int rowStrideBytes;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      FieldGridTerrainOverlayVariantB_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell[-1].runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(fieldCell->runtime58_6F + rowStrideBytes + -8) & 0x88006000) != 0) {
        return;
      }
      if (0 < *(int *)(fieldCell->runtime58_6F + rowStrideBytes + -0xc)) {
        *(undefined4 *)(fieldCell->runtime00_07 + rowStrideBytes + 4) = g_TerrainScanReferenceHeight
        ;
      }
      currentCellRuntimeBase = fieldCell->runtime00_07;
      fieldCell = (FieldGridCell *)
                  (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes) + -1;
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (extraout_ECX + 3,
                 (FieldGridCell *)
                 (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + rowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void FieldGridTerrainOverlayVariantB_ApplyWedge5
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  FieldGridCell *fieldCell_00;
  uint extraout_ECX;
  uint extraout_ECX_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((fieldCell->flagsAndMaterial & 0x88006000) == 0) {
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      FieldGridTerrainOverlayVariantB_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(fieldCell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((fieldCell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell[1].waterSurfaceDelta) {
        *(undefined4 *)(fieldCell[1].runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      fieldCell_00 = fieldCell + 2;
      fieldCell = (FieldGridCell *)(fieldCell[1].runtime00_07 + g_TerrainScanRowStrideBytes);
      FieldGridTerrainOverlayVariantB_ApplyDirection0(extraout_ECX + 3,fieldCell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
undefined8 TerrainProjectedQuad_QueueAsTwoTrianglesRegs(GraphicsPrimitiveQueue *param_1)

{
  undefined4 extraout_EAX;
  undefined4 in_EDX;
  int unaff_EBX;
  GraphicsProjectedVertexSource *unaff_ESI;
  
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (*(int *)unaff_ESI->reserved00_0B,
             (GraphicsProjectedVertexSource *)(unaff_ESI[2].texturedPacketAttributes + 1),
             (GraphicsProjectedVertexSource *)(unaff_ESI->reserved00_0B + unaff_EBX),unaff_ESI,
             param_1);
  TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
            (*(int *)unaff_ESI->reserved00_0B,
             (GraphicsProjectedVertexSource *)(unaff_ESI[2].texturedPacketAttributes + 1),
             (GraphicsProjectedVertexSource *)
             ((int)unaff_ESI[2].texturedPacketAttributes + unaff_EBX + 4),
             (GraphicsProjectedVertexSource *)(unaff_ESI->reserved00_0B + unaff_EBX),param_1);
  return CONCAT44(in_EDX,extraout_EAX);
}

/* Address: 0x005004A0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected vertex transform project and shade variant a.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime], GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
   [graphics/render/shading].
*/
undefined8 TerrainProjectedVertex_TransformProjectAndShadeVariantA(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  ushort uVar9;
  ushort uVar10;
  undefined4 in_EAX;
  int extraout_EAX;
  GraphicsFixedVec3 *extraout_ECX;
  int extraout_ECX_00;
  GraphicsFixedVec3 *extraout_ECX_01;
  GraphicsFixedVec3 *pGVar11;
  undefined4 in_EDX;
  int iVar12;
  int extraout_EDX;
  uint uVar13;
  uint uVar14;
  undefined1 uVar17;
  undefined1 uVar18;
  undefined8 uVar15;
  undefined8 uVar16;
  undefined1 uVar19;
  undefined1 uVar20;
  undefined8 extraout_MM1;
  undefined8 extraout_MM1_00;
  GraphicsProjectedPointEdxEax8 GVar21;
  
  uVar13 = *(uint *)(param_1 + 0x50) & 0xe801ffff;
  if ((*(uint *)(param_1 + 0x50) & 0xff) != 0xff) {
    uVar14 = uVar13 | 0x200000;
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)(param_1 + 0x14),(GraphicsFixedVec3 *)(param_1 + 0x40),
               &g_ViewProjectionMatrixFixed);
    pGVar11 = (GraphicsFixedVec3 *)(param_1 + 0x14);
    if ((int)g_ProjectionScaleFixed < *(int *)(param_1 + 0x1c)) {
      GVar21 = Graphics_ProjectViewPoint(pGVar11);
      iVar12 = (int)(GVar21 >> 0x20);
      *(GraphicsProjectedPointEdxEax8 *)(param_1 + 0xc) = GVar21;
      uVar14 = uVar13;
      if (g_ProjectionClipRect.minX <= (int)GVar21) {
        uVar14 = uVar13 | 0x20000;
      }
      if ((int)GVar21 < g_ProjectionClipRect.maxX) {
        uVar14 = uVar14 | 0x80000;
      }
      if (g_ProjectionClipRect.minY <= iVar12) {
        uVar14 = uVar14 | 0x40000;
      }
      pGVar11 = extraout_ECX;
      if (iVar12 < g_ProjectionClipRect.maxY) {
        uVar14 = uVar14 | 0x100000;
      }
    }
    uVar1 = *(undefined4 *)(param_1 + 0x58);
    uVar2 = *(undefined4 *)(param_1 + 4);
    uVar17 = (undefined1)((uint)uVar1 >> 0x18);
    uVar9 = CONCAT11(uVar17,uVar17);
    uVar18 = (undefined1)((uint)uVar1 >> 0x10);
    uVar17 = (undefined1)((uint)uVar1 >> 8);
    uVar19 = (undefined1)((uint)uVar2 >> 0x18);
    uVar10 = CONCAT11(uVar19,uVar19);
    uVar20 = (undefined1)((uint)uVar2 >> 0x10);
    uVar19 = (undefined1)((uint)uVar2 >> 8);
    uVar15 = CONCAT26(uVar9 >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar9,uVar18),CONCAT14(uVar18,uVar1)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(uVar17,uVar17) >> 6,
                                        CONCAT11((char)uVar1,(char)uVar1) >> 6)));
    uVar16 = CONCAT26(uVar10 >> 2,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar10,uVar20),CONCAT14(uVar20,uVar2)) >>
                                       0x20) >> 2,
                               CONCAT22(CONCAT11(uVar19,uVar19) >> 2,
                                        CONCAT11((char)uVar2,(char)uVar2) >> 2)));
    if (*(int *)(param_1 + 0x68) == 0xff) {
      uVar15 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (pGVar11,(int *)(param_1 + 0x14));
      uVar16 = extraout_MM1;
    }
    uVar16 = pmulhw(uVar15,uVar16);
    sVar5 = (short)uVar16;
    sVar6 = (short)((ulonglong)uVar16 >> 0x10);
    sVar7 = (short)((ulonglong)uVar16 >> 0x20);
    sVar8 = (short)((ulonglong)uVar16 >> 0x30);
    piVar3 = *(int **)(param_1 + 0x54);
    *(uint *)(param_1 + 0x60) =
         CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar16 >> 0x30) - (0xff < sVar8)
                  ,CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)uVar16 >> 0x20) -
                            (0xff < sVar7),
                            CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                     (char)((ulonglong)uVar16 >> 0x10) - (0xff < sVar6),
                                     (0 < sVar5) * (sVar5 < 0x100) * (char)uVar16 - (0xff < sVar5)))
                 );
    uVar13 = uVar14 | 0x4000000;
    iVar12 = piVar3[1];
    iVar4 = piVar3[2];
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + *piVar3;
    *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) + iVar12;
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + iVar4 + *(int *)(param_1 + 0x4c);
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)(param_1 + 0x34),(GraphicsFixedVec3 *)(param_1 + 0x40),
               &g_ViewProjectionMatrixFixed);
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) - extraout_EAX;
    *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) - extraout_ECX_00;
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) - extraout_EDX;
    pGVar11 = (GraphicsFixedVec3 *)(param_1 + 0x34);
    if ((int)g_ProjectionScaleFixed < *(int *)(param_1 + 0x3c)) {
      GVar21 = Graphics_ProjectViewPoint(pGVar11);
      iVar12 = (int)(GVar21 >> 0x20);
      *(GraphicsProjectedPointEdxEax8 *)(param_1 + 0x2c) = GVar21;
      uVar13 = uVar14;
      if (g_ProjectionClipRect.minX <= (int)GVar21) {
        uVar13 = uVar14 | 0x400000;
      }
      if ((int)GVar21 < g_ProjectionClipRect.maxX) {
        uVar13 = uVar13 | 0x1000000;
      }
      if (g_ProjectionClipRect.minY <= iVar12) {
        uVar13 = uVar13 | 0x800000;
      }
      pGVar11 = extraout_ECX_01;
      if (iVar12 < g_ProjectionClipRect.maxY) {
        uVar13 = uVar13 | 0x2000000;
      }
    }
    uVar1 = *(undefined4 *)(param_1 + 0x5c);
    uVar2 = *(undefined4 *)(param_1 + 4);
    uVar17 = (undefined1)((uint)uVar1 >> 0x18);
    uVar9 = CONCAT11(uVar17,uVar17);
    uVar18 = (undefined1)((uint)uVar1 >> 0x10);
    uVar17 = (undefined1)((uint)uVar1 >> 8);
    uVar19 = (undefined1)((uint)uVar2 >> 0x18);
    uVar10 = CONCAT11(uVar19,uVar19);
    uVar20 = (undefined1)((uint)uVar2 >> 0x10);
    uVar19 = (undefined1)((uint)uVar2 >> 8);
    uVar15 = CONCAT26(uVar9 >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar9,uVar18),CONCAT14(uVar18,uVar1)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(uVar17,uVar17) >> 6,
                                        CONCAT11((char)uVar1,(char)uVar1) >> 6)));
    uVar16 = CONCAT26(uVar10 >> 2,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar10,uVar20),CONCAT14(uVar20,uVar2)) >>
                                       0x20) >> 2,
                               CONCAT22(CONCAT11(uVar19,uVar19) >> 2,
                                        CONCAT11((char)uVar2,(char)uVar2) >> 2)));
    if (*(int *)(param_1 + 0x68) == 0xff) {
      uVar15 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (pGVar11,(int *)(param_1 + 0x34));
      uVar16 = extraout_MM1_00;
    }
    uVar16 = pmulhw(uVar15,uVar16);
    sVar5 = (short)uVar16;
    sVar6 = (short)((ulonglong)uVar16 >> 0x10);
    sVar7 = (short)((ulonglong)uVar16 >> 0x20);
    sVar8 = (short)((ulonglong)uVar16 >> 0x30);
    *(uint *)(param_1 + 0x50) = uVar13;
    *(uint *)(param_1 + 100) =
         CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar16 >> 0x30) - (0xff < sVar8)
                  ,CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)uVar16 >> 0x20) -
                            (0xff < sVar7),
                            CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                     (char)((ulonglong)uVar16 >> 0x10) - (0xff < sVar6),
                                     (0 < sVar5) * (sVar5 < 0x100) * (char)uVar16 - (0xff < sVar5)))
                 );
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005006A0.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected vertex transform project and shade variant b.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime], GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
   [graphics/render/shading].
*/
undefined8 TerrainProjectedVertex_TransformProjectAndShadeVariantB(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  ushort uVar9;
  ushort uVar10;
  undefined4 in_EAX;
  int extraout_EAX;
  int extraout_ECX;
  GraphicsFixedVec3 *extraout_ECX_00;
  GraphicsFixedVec3 *extraout_ECX_01;
  GraphicsFixedVec3 *viewPoint;
  undefined4 in_EDX;
  int extraout_EDX;
  int iVar11;
  uint uVar12;
  uint uVar13;
  undefined1 uVar16;
  undefined1 uVar17;
  undefined8 uVar14;
  undefined8 uVar15;
  undefined1 uVar18;
  undefined1 uVar19;
  undefined8 extraout_MM1;
  undefined8 extraout_MM1_00;
  GraphicsProjectedPointEdxEax8 GVar20;
  
  viewPoint = (GraphicsFixedVec3 *)(param_1 + 0x14);
  uVar13 = *(uint *)(param_1 + 0x50);
  piVar1 = *(int **)(param_1 + 0x54);
  if ((uVar13 & 0x10000000) != 0) {
    uVar12 = uVar13 & 0xf83fffff;
    uVar13 = uVar12 | 0x4000000;
    iVar11 = piVar1[1];
    iVar2 = piVar1[2];
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + *piVar1;
    *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) + iVar11;
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + iVar2 + *(int *)(param_1 + 0x4c);
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)(param_1 + 0x34),(GraphicsFixedVec3 *)(param_1 + 0x40),
               &g_ViewProjectionMatrixFixed);
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) - extraout_EAX;
    *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) - extraout_ECX;
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) - extraout_EDX;
    viewPoint = (GraphicsFixedVec3 *)(param_1 + 0x34);
    if ((int)g_ProjectionScaleFixed < *(int *)(param_1 + 0x3c)) {
      GVar20 = Graphics_ProjectViewPoint(viewPoint);
      iVar11 = (int)(GVar20 >> 0x20);
      *(GraphicsProjectedPointEdxEax8 *)(param_1 + 0x2c) = GVar20;
      uVar13 = uVar12;
      if (g_ProjectionClipRect.minX <= (int)GVar20) {
        uVar13 = uVar12 | 0x400000;
      }
      if ((int)GVar20 < g_ProjectionClipRect.maxX) {
        uVar13 = uVar13 | 0x1000000;
      }
      if (g_ProjectionClipRect.minY <= iVar11) {
        uVar13 = uVar13 | 0x800000;
      }
      viewPoint = extraout_ECX_00;
      if (iVar11 < g_ProjectionClipRect.maxY) {
        uVar13 = uVar13 | 0x2000000;
      }
    }
    uVar3 = *(undefined4 *)(param_1 + 0x5c);
    uVar4 = *(undefined4 *)(param_1 + 4);
    uVar16 = (undefined1)((uint)uVar3 >> 0x18);
    uVar9 = CONCAT11(uVar16,uVar16);
    uVar17 = (undefined1)((uint)uVar3 >> 0x10);
    uVar16 = (undefined1)((uint)uVar3 >> 8);
    uVar18 = (undefined1)((uint)uVar4 >> 0x18);
    uVar10 = CONCAT11(uVar18,uVar18);
    uVar19 = (undefined1)((uint)uVar4 >> 0x10);
    uVar18 = (undefined1)((uint)uVar4 >> 8);
    uVar14 = CONCAT26(uVar9 >> 6,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar9,uVar17),CONCAT14(uVar17,uVar3)) >>
                                       0x20) >> 6,
                               CONCAT22(CONCAT11(uVar16,uVar16) >> 6,
                                        CONCAT11((char)uVar3,(char)uVar3) >> 6)));
    uVar15 = CONCAT26(uVar10 >> 2,
                      CONCAT24((ushort)(CONCAT35(CONCAT21(uVar10,uVar19),CONCAT14(uVar19,uVar4)) >>
                                       0x20) >> 2,
                               CONCAT22(CONCAT11(uVar18,uVar18) >> 2,
                                        CONCAT11((char)uVar4,(char)uVar4) >> 2)));
    if (*(int *)(param_1 + 0x68) == 0xff) {
      uVar14 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                         (viewPoint,(int *)(param_1 + 0x34));
      viewPoint = extraout_ECX_01;
      uVar15 = extraout_MM1;
    }
    uVar15 = pmulhw(uVar14,uVar15);
    sVar5 = (short)uVar15;
    sVar6 = (short)((ulonglong)uVar15 >> 0x10);
    sVar7 = (short)((ulonglong)uVar15 >> 0x20);
    sVar8 = (short)((ulonglong)uVar15 >> 0x30);
    *(uint *)(param_1 + 100) =
         CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar15 >> 0x30) - (0xff < sVar8)
                  ,CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)uVar15 >> 0x20) -
                            (0xff < sVar7),
                            CONCAT11((0 < sVar6) * (sVar6 < 0x100) *
                                     (char)((ulonglong)uVar15 >> 0x10) - (0xff < sVar6),
                                     (0 < sVar5) * (sVar5 < 0x100) * (char)uVar15 - (0xff < sVar5)))
                 );
  }
  uVar3 = *(undefined4 *)(param_1 + 0x58);
  uVar4 = *(undefined4 *)(param_1 + 4);
  uVar16 = (undefined1)((uint)uVar3 >> 0x18);
  uVar9 = CONCAT11(uVar16,uVar16);
  uVar17 = (undefined1)((uint)uVar3 >> 0x10);
  uVar16 = (undefined1)((uint)uVar3 >> 8);
  uVar18 = (undefined1)((uint)uVar4 >> 0x18);
  uVar10 = CONCAT11(uVar18,uVar18);
  uVar19 = (undefined1)((uint)uVar4 >> 0x10);
  uVar18 = (undefined1)((uint)uVar4 >> 8);
  uVar14 = CONCAT26(uVar9 >> 6,
                    CONCAT24((ushort)(CONCAT35(CONCAT21(uVar9,uVar17),CONCAT14(uVar17,uVar3)) >>
                                     0x20) >> 6,
                             CONCAT22(CONCAT11(uVar16,uVar16) >> 6,
                                      CONCAT11((char)uVar3,(char)uVar3) >> 6)));
  uVar15 = CONCAT26(uVar10 >> 2,
                    CONCAT24((ushort)(CONCAT35(CONCAT21(uVar10,uVar19),CONCAT14(uVar19,uVar4)) >>
                                     0x20) >> 2,
                             CONCAT22(CONCAT11(uVar18,uVar18) >> 2,
                                      CONCAT11((char)uVar4,(char)uVar4) >> 2)));
  if (*(int *)(param_1 + 0x68) == 0xff) {
    uVar14 = GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
                       (viewPoint,(int *)(param_1 + 0x14));
    uVar15 = extraout_MM1_00;
  }
  uVar15 = pmulhw(uVar14,uVar15);
  sVar5 = (short)uVar15;
  sVar6 = (short)((ulonglong)uVar15 >> 0x10);
  sVar7 = (short)((ulonglong)uVar15 >> 0x20);
  sVar8 = (short)((ulonglong)uVar15 >> 0x30);
  *(uint *)(param_1 + 0x60) =
       CONCAT13((0 < sVar8) * (sVar8 < 0x100) * (char)((ulonglong)uVar15 >> 0x30) - (0xff < sVar8),
                CONCAT12((0 < sVar7) * (sVar7 < 0x100) * (char)((ulonglong)uVar15 >> 0x20) -
                         (0xff < sVar7),
                         CONCAT11((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)uVar15 >> 0x10)
                                  - (0xff < sVar6),
                                  (0 < sVar5) * (sVar5 < 0x100) * (char)uVar15 - (0xff < sVar5))));
  *(uint *)(param_1 + 0x50) = uVar13;
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00500820.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected triangle clip interpolate and queue textured.
   Cross-module calls: Triangle2D_ComputeBarycentricWeightsQ12Packed [core/math/geometry],
   GraphicsPrimitiveQueue_AppendTexturedTriangleRegs [graphics/render/primitives],
   GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle [graphics/render/primitives].
*/
void TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
               (int param_1,GraphicsProjectedVertexSource *param_2,
               GraphicsProjectedVertexSource *param_3,GraphicsProjectedVertexSource *param_4,
               GraphicsPrimitiveQueue *param_5)

{
  uint *puVar1;
  GraphicsPrimitiveQueueNode **ppGVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  GraphicsPrimitiveDepthFixed GVar6;
  GraphicsPrimitiveDepthFixed GVar7;
  GraphicsPrimitiveDepthFixed GVar8;
  short sVar9;
  short sVar10;
  short sVar11;
  short sVar12;
  short sVar13;
  short sVar14;
  short sVar15;
  short sVar16;
  short sVar17;
  short sVar18;
  short sVar19;
  short sVar20;
  ushort uVar21;
  ushort uVar22;
  ushort uVar23;
  uint uVar24;
  int iVar25;
  int iVar26;
  uint incomingEcxValue;
  undefined4 textureAndMaterialIndices;
  uint uVar27;
  int iVar28;
  uint uVar29;
  bool bVar30;
  PackedArgb32 vertex0DiffuseColor;
  undefined1 uVar32;
  undefined1 uVar33;
  undefined8 uVar31;
  PackedArgb32 vertex1DiffuseColor;
  undefined1 uVar35;
  undefined1 uVar36;
  undefined8 uVar34;
  PackedArgb32 vertex2DiffuseColor;
  undefined1 uVar38;
  undefined1 uVar39;
  undefined8 uVar37;
  ulonglong uVar40;
  GraphicsProjectedVertexSource *vertex2Projected;
  GraphicsProjectedVertexSource *vertex1Projected;
  GraphicsProjectedVertexSource *vertex0Projected;
  GraphicsPrimitiveQueue *primitiveQueue;
  
  uVar24 = param_4[1].texturedPacketAttributes[3] | param_3[1].texturedPacketAttributes[3] |
           param_2[1].texturedPacketAttributes[3];
  if ((uVar24 & 0xff) != 0xff) {
    if ((uVar24 & 0x3e0000) == 0x1e0000) {
      bVar30 = false;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) == 0) {
        uVar40 = Triangle2D_ComputeBarycentricWeightsQ12Packed
                           (param_2->texturedPacketAttributes[1],
                            param_2->texturedPacketAttributes[0],
                            param_3->texturedPacketAttributes[1],
                            param_3->texturedPacketAttributes[0],
                            param_4->texturedPacketAttributes[1],
                            param_4->texturedPacketAttributes[0],(int)param_5[5].packetPool,
                            param_5[5].count);
        iVar26 = (int)(uVar40 >> 0x20);
        if ((!bVar30) && ((int)param_4->texturedPacketAttributes[4] < (int)param_5[5].capacity)) {
          param_5[5].capacity = param_4->texturedPacketAttributes[4];
          dVar3 = param_3[1].texturedPacketAttributes[0];
          dVar4 = param_4[1].texturedPacketAttributes[0];
          dVar5 = param_4[1].texturedPacketAttributes[0];
          param_5[4].primaryNodes[0].next =
               (GraphicsPrimitiveQueueNode *)
               (((*(int *)(param_3[1].reserved00_0B + 8) - *(int *)(param_4[1].reserved00_0B + 8)) *
                 (int)uVar40 >> 0xc) + *(int *)(param_4[1].reserved00_0B + 8));
          param_5[4].primaryNodes[0].previous =
               (GraphicsPrimitiveQueueNode *)(((int)((dVar3 - dVar4) * (int)uVar40) >> 0xc) + dVar5)
          ;
          dVar3 = param_2[1].texturedPacketAttributes[0];
          dVar4 = param_4[1].texturedPacketAttributes[0];
          ppGVar2 = &param_5[4].primaryNodes[0].next;
          *ppGVar2 = (GraphicsPrimitiveQueueNode *)
                     ((int)&(*ppGVar2)->sortKey +
                     ((*(int *)(param_2[1].reserved00_0B + 8) -
                      *(int *)(param_4[1].reserved00_0B + 8)) * iVar26 >> 0xc));
          ppGVar2 = &param_5[4].primaryNodes[0].previous;
          *ppGVar2 = (GraphicsPrimitiveQueueNode *)
                     ((int)&(*ppGVar2)->sortKey + ((int)((dVar3 - dVar4) * iVar26) >> 0xc));
        }
      }
      GVar6 = param_4[1].depth;
      GVar7 = param_3[1].depth;
      GVar8 = param_2[1].depth;
      uVar32 = (undefined1)((uint)GVar6 >> 0x18);
      uVar21 = CONCAT11(uVar32,uVar32);
      uVar33 = (undefined1)((uint)GVar6 >> 0x10);
      uVar32 = (undefined1)((uint)GVar6 >> 8);
      uVar35 = (undefined1)((uint)GVar7 >> 0x18);
      uVar22 = CONCAT11(uVar35,uVar35);
      uVar36 = (undefined1)((uint)GVar7 >> 0x10);
      uVar35 = (undefined1)((uint)GVar7 >> 8);
      uVar38 = (undefined1)((uint)GVar8 >> 0x18);
      uVar23 = CONCAT11(uVar38,uVar38);
      uVar39 = (undefined1)((uint)GVar8 >> 0x10);
      uVar38 = (undefined1)((uint)GVar8 >> 8);
      uVar24 = param_4[1].texturedPacketAttributes[2];
      uVar29 = param_3[1].texturedPacketAttributes[2];
      uVar27 = param_2[1].texturedPacketAttributes[2];
      if ((int)uVar24 < 0) {
        uVar24 = 0;
      }
      if ((int)uVar29 < 0) {
        uVar29 = 0;
      }
      if ((int)uVar27 < 0) {
        uVar27 = 0;
      }
      iVar26 = param_4[1].screenX - (uVar24 >> 1);
      if (iVar26 < 0) {
        iVar26 = 0;
      }
      iVar28 = param_3[1].screenX - (uVar29 >> 1);
      if (iVar28 < 0) {
        iVar28 = 0;
      }
      iVar25 = param_2[1].screenX - (uVar27 >> 1);
      if (iVar25 < 0) {
        iVar25 = 0;
      }
      uVar31 = pmulhw(CONCAT26(uVar21 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar21,uVar33),
                                                          CONCAT14(uVar33,GVar6)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar32,uVar32) >> 4,
                                                 CONCAT11((char)GVar6,(char)GVar6) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar26 * 8));
      uVar34 = pmulhw(CONCAT26(uVar22 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar22,uVar36),
                                                          CONCAT14(uVar36,GVar7)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar35,uVar35) >> 4,
                                                 CONCAT11((char)GVar7,(char)GVar7) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar28 * 8));
      uVar37 = pmulhw(CONCAT26(uVar23 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar23,uVar39),
                                                          CONCAT14(uVar39,GVar8)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar38,uVar38) >> 4,
                                                 CONCAT11((char)GVar8,(char)GVar8) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + iVar25 * 8));
      sVar9 = (short)uVar31;
      sVar10 = (short)((ulonglong)uVar31 >> 0x10);
      sVar11 = (short)((ulonglong)uVar31 >> 0x20);
      sVar12 = (short)((ulonglong)uVar31 >> 0x30);
      vertex0DiffuseColor =
           CONCAT13((0 < sVar12) * (sVar12 < 0x100) * (char)((ulonglong)uVar31 >> 0x30) -
                    (0xff < sVar12),
                    CONCAT12((0 < sVar11) * (sVar11 < 0x100) * (char)((ulonglong)uVar31 >> 0x20) -
                             (0xff < sVar11),
                             CONCAT11((0 < sVar10) * (sVar10 < 0x100) *
                                      (char)((ulonglong)uVar31 >> 0x10) - (0xff < sVar10),
                                      (0 < sVar9) * (sVar9 < 0x100) * (char)uVar31 - (0xff < sVar9))
                            ));
      sVar9 = (short)uVar34;
      sVar10 = (short)((ulonglong)uVar34 >> 0x10);
      sVar11 = (short)((ulonglong)uVar34 >> 0x20);
      sVar12 = (short)((ulonglong)uVar34 >> 0x30);
      vertex1DiffuseColor =
           CONCAT13((0 < sVar12) * (sVar12 < 0x100) * (char)((ulonglong)uVar34 >> 0x30) -
                    (0xff < sVar12),
                    CONCAT12((0 < sVar11) * (sVar11 < 0x100) * (char)((ulonglong)uVar34 >> 0x20) -
                             (0xff < sVar11),
                             CONCAT11((0 < sVar10) * (sVar10 < 0x100) *
                                      (char)((ulonglong)uVar34 >> 0x10) - (0xff < sVar10),
                                      (0 < sVar9) * (sVar9 < 0x100) * (char)uVar34 - (0xff < sVar9))
                            ));
      sVar9 = (short)uVar37;
      sVar10 = (short)((ulonglong)uVar37 >> 0x10);
      sVar11 = (short)((ulonglong)uVar37 >> 0x20);
      sVar12 = (short)((ulonglong)uVar37 >> 0x30);
      vertex2DiffuseColor =
           CONCAT13((0 < sVar12) * (sVar12 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                    (0xff < sVar12),
                    CONCAT12((0 < sVar11) * (sVar11 < 0x100) * (char)((ulonglong)uVar37 >> 0x20) -
                             (0xff < sVar11),
                             CONCAT11((0 < sVar10) * (sVar10 < 0x100) *
                                      (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar10),
                                      (0 < sVar9) * (sVar9 < 0x100) * (char)uVar37 - (0xff < sVar9))
                            ));
      uVar24 = (param_2[1].texturedPacketAttributes[3] & 0x700) +
               (int)g_TerrainSoilPacketTablePayload;
      iVar25 = (param_4[1].texturedPacketAttributes[3] & 0xff) * 0x800;
      iVar28 = (param_3[1].texturedPacketAttributes[3] & 0xff) * 0x800;
      uVar29 = (param_2[1].texturedPacketAttributes[3] & 0xff) * 0x800;
      uVar27 = (int)g_TerrainSoilPacketTablePayload +
               iVar28 + (param_3[1].texturedPacketAttributes[3] & 0x700);
      bVar30 = CARRY4(uVar24,uVar29);
      vertex2Projected = param_2;
      vertex1Projected = param_3;
      vertex0Projected = param_4;
      primitiveQueue = param_5;
      uVar31 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                         (uVar24 + uVar29,uVar29,
                          (dword *)((int)g_TerrainSoilPacketTablePayload +
                                   iVar25 + (param_4[1].texturedPacketAttributes[3] & 0x700)),
                          vertex2DiffuseColor,vertex1DiffuseColor,vertex0DiffuseColor,param_2,
                          param_3,param_4,param_5);
      iVar26 = (int)((ulonglong)uVar31 >> 0x20);
      if (!bVar30) {
        if (iVar25 == iVar28) {
          if (iVar25 != iVar26) {
            bVar30 = 0xffffffdf < incomingEcxValue;
            uVar31 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               (incomingEcxValue + 0x20,iVar26,(dword *)(incomingEcxValue + 0x20),
                                vertex2DiffuseColor,vertex1DiffuseColor,vertex0DiffuseColor,
                                vertex2Projected,vertex1Projected,vertex0Projected,primitiveQueue);
            if (!bVar30) {
              puVar1 = (uint *)((int)uVar31 + 0x68);
              *puVar1 = *puVar1 | 0x10020000;
            }
          }
        }
        else if (iVar25 == iVar26) {
          bVar30 = 0xffffffbf < uVar27;
          uVar31 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             (incomingEcxValue,iVar26,(dword *)(uVar27 + 0x40),vertex2DiffuseColor,
                              vertex1DiffuseColor,vertex0DiffuseColor,vertex2Projected,
                              vertex1Projected,vertex0Projected,primitiveQueue);
          if (!bVar30) {
            puVar1 = (uint *)((int)uVar31 + 0x68);
            *puVar1 = *puVar1 | 0x10020000;
          }
        }
        else if (iVar28 == iVar26) {
          bVar30 = 0xffffff9f < uVar27;
          uVar31 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             (incomingEcxValue,iVar26,(dword *)(uVar27 + 0x60),vertex2DiffuseColor,
                              vertex1DiffuseColor,vertex0DiffuseColor,vertex2Projected,
                              vertex1Projected,vertex0Projected,primitiveQueue);
          if (!bVar30) {
            puVar1 = (uint *)((int)uVar31 + 0x68);
            *puVar1 = *puVar1 | 0x10020000;
          }
        }
        else {
          bVar30 = 0xffffff5f < incomingEcxValue;
          uVar31 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                             (incomingEcxValue + 0xa0,iVar26,(dword *)(uVar27 + 0x80),
                              vertex2DiffuseColor,vertex1DiffuseColor,vertex0DiffuseColor,
                              vertex2Projected,vertex1Projected,vertex0Projected,primitiveQueue);
          if (!bVar30) {
            puVar1 = (uint *)((int)uVar31 + 0x68);
            bVar30 = false;
            *puVar1 = *puVar1 | 0x10020000;
            uVar31 = GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
                               (textureAndMaterialIndices,(int)((ulonglong)uVar31 >> 0x20),
                                (dword *)textureAndMaterialIndices,vertex2DiffuseColor,
                                vertex1DiffuseColor,vertex0DiffuseColor,vertex2Projected,
                                vertex1Projected,vertex0Projected,primitiveQueue);
            if (!bVar30) {
              puVar1 = (uint *)((int)uVar31 + 0x68);
              *puVar1 = *puVar1 | 0x20020000;
            }
          }
        }
      }
    }
    if ((((((param_5[1].reserved1C & 0x1000000) != 0) ||
          (0 < (int)param_4[1].texturedPacketAttributes[2])) ||
         (0 < (int)param_3[1].texturedPacketAttributes[2])) ||
        (0 < (int)param_2[1].texturedPacketAttributes[2])) &&
       (((param_4[1].texturedPacketAttributes[3] | param_3[1].texturedPacketAttributes[3] |
         param_2[1].texturedPacketAttributes[3]) & 0x7c00000) == 0x3c00000)) {
      param_4[1].texturedPacketAttributes[3] = param_4[1].texturedPacketAttributes[3] | 0x10000000;
      param_3[1].texturedPacketAttributes[3] = param_3[1].texturedPacketAttributes[3] | 0x10000000;
      param_2[1].texturedPacketAttributes[3] = param_2[1].texturedPacketAttributes[3] | 0x10000000;
      bVar30 = false;
      if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
        uVar40 = Triangle2D_ComputeBarycentricWeightsQ12Packed
                           (param_2->screenX,param_2->reserved2C,param_3->screenX,
                            param_3->reserved2C,param_4->screenX,param_4->reserved2C,
                            (int)param_5[5].packetPool,param_5[5].count);
        iVar26 = (int)(uVar40 >> 0x20);
        if ((!bVar30) && ((int)*(dword *)(param_4[1].reserved00_0B + 4) < (int)param_5[5].capacity))
        {
          param_5[5].capacity = *(dword *)(param_4[1].reserved00_0B + 4);
          dVar3 = param_3[1].texturedPacketAttributes[0];
          dVar4 = param_4[1].texturedPacketAttributes[0];
          dVar5 = param_4[1].texturedPacketAttributes[0];
          param_5[4].primaryNodes[0].next =
               (GraphicsPrimitiveQueueNode *)
               (((*(int *)(param_3[1].reserved00_0B + 8) - *(int *)(param_4[1].reserved00_0B + 8)) *
                 (int)uVar40 >> 0xc) + *(int *)(param_4[1].reserved00_0B + 8));
          param_5[4].primaryNodes[0].previous =
               (GraphicsPrimitiveQueueNode *)(((int)((dVar3 - dVar4) * (int)uVar40) >> 0xc) + dVar5)
          ;
          dVar3 = param_2[1].texturedPacketAttributes[0];
          dVar4 = param_4[1].texturedPacketAttributes[0];
          ppGVar2 = &param_5[4].primaryNodes[0].next;
          *ppGVar2 = (GraphicsPrimitiveQueueNode *)
                     ((int)&(*ppGVar2)->sortKey +
                     ((*(int *)(param_2[1].reserved00_0B + 8) -
                      *(int *)(param_4[1].reserved00_0B + 8)) * iVar26 >> 0xc));
          ppGVar2 = &param_5[4].primaryNodes[0].previous;
          *ppGVar2 = (GraphicsPrimitiveQueueNode *)
                     ((int)&(*ppGVar2)->sortKey + ((int)((dVar3 - dVar4) * iVar26) >> 0xc));
        }
      }
      dVar3 = param_4[1].reserved2C;
      dVar4 = param_3[1].reserved2C;
      dVar5 = param_2[1].reserved2C;
      uVar32 = (undefined1)(dVar3 >> 0x18);
      uVar21 = CONCAT11(uVar32,uVar32);
      uVar33 = (undefined1)(dVar3 >> 0x10);
      uVar32 = (undefined1)(dVar3 >> 8);
      uVar35 = (undefined1)(dVar4 >> 0x18);
      uVar22 = CONCAT11(uVar35,uVar35);
      uVar36 = (undefined1)(dVar4 >> 0x10);
      uVar35 = (undefined1)(dVar4 >> 8);
      uVar38 = (undefined1)(dVar5 >> 0x18);
      uVar23 = CONCAT11(uVar38,uVar38);
      uVar39 = (undefined1)(dVar5 >> 0x10);
      uVar38 = (undefined1)(dVar5 >> 8);
      uVar31 = pmulhw(CONCAT26(uVar21 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar21,uVar33),
                                                          CONCAT14(uVar33,dVar3)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar32,uVar32) >> 4,
                                                 CONCAT11((char)dVar3,(char)dVar3) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + param_4[1].screenX * 8));
      uVar34 = pmulhw(CONCAT26(uVar22 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar22,uVar36),
                                                          CONCAT14(uVar36,dVar4)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar35,uVar35) >> 4,
                                                 CONCAT11((char)dVar4,(char)dVar4) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + param_3[1].screenX * 8));
      uVar37 = pmulhw(CONCAT26(uVar23 >> 4,
                               CONCAT24((ushort)(CONCAT35(CONCAT21(uVar23,uVar39),
                                                          CONCAT14(uVar39,dVar5)) >> 0x20) >> 4,
                                        CONCAT22(CONCAT11(uVar38,uVar38) >> 4,
                                                 CONCAT11((char)dVar5,(char)dVar5) >> 4))),
                      *(undefined8 *)(&g_PackedLightingLookupTable + param_2[1].screenX * 8));
      sVar9 = (short)uVar31;
      sVar12 = (short)((ulonglong)uVar31 >> 0x10);
      sVar15 = (short)((ulonglong)uVar31 >> 0x20);
      sVar18 = (short)((ulonglong)uVar31 >> 0x30);
      sVar10 = (short)uVar34;
      sVar13 = (short)((ulonglong)uVar34 >> 0x10);
      sVar16 = (short)((ulonglong)uVar34 >> 0x20);
      sVar19 = (short)((ulonglong)uVar34 >> 0x30);
      sVar11 = (short)uVar37;
      sVar14 = (short)((ulonglong)uVar37 >> 0x10);
      sVar17 = (short)((ulonglong)uVar37 >> 0x20);
      sVar20 = (short)((ulonglong)uVar37 >> 0x30);
      GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle
                ((undefined4 *)(param_1 * 0x20 + (int)g_TerrainSurfacePacketTablePayload),
                 CONCAT13((0 < sVar20) * (sVar20 < 0x100) * (char)((ulonglong)uVar37 >> 0x30) -
                          (0xff < sVar20),
                          CONCAT12((0 < sVar17) * (sVar17 < 0x100) *
                                   (char)((ulonglong)uVar37 >> 0x20) - (0xff < sVar17),
                                   CONCAT11((0 < sVar14) * (sVar14 < 0x100) *
                                            (char)((ulonglong)uVar37 >> 0x10) - (0xff < sVar14),
                                            (0 < sVar11) * (sVar11 < 0x100) * (char)uVar37 -
                                            (0xff < sVar11)))),
                 CONCAT13((0 < sVar19) * (sVar19 < 0x100) * (char)((ulonglong)uVar34 >> 0x30) -
                          (0xff < sVar19),
                          CONCAT12((0 < sVar16) * (sVar16 < 0x100) *
                                   (char)((ulonglong)uVar34 >> 0x20) - (0xff < sVar16),
                                   CONCAT11((0 < sVar13) * (sVar13 < 0x100) *
                                            (char)((ulonglong)uVar34 >> 0x10) - (0xff < sVar13),
                                            (0 < sVar10) * (sVar10 < 0x100) * (char)uVar34 -
                                            (0xff < sVar10)))),
                 CONCAT13((0 < sVar18) * (sVar18 < 0x100) * (char)((ulonglong)uVar31 >> 0x30) -
                          (0xff < sVar18),
                          CONCAT12((0 < sVar15) * (sVar15 < 0x100) *
                                   (char)((ulonglong)uVar31 >> 0x20) - (0xff < sVar15),
                                   CONCAT11((0 < sVar12) * (sVar12 < 0x100) *
                                            (char)((ulonglong)uVar31 >> 0x10) - (0xff < sVar12),
                                            (0 < sVar9) * (sVar9 < 0x100) * (char)uVar31 -
                                            (0xff < sVar9)))),(int)param_2,(int)param_3,(int)param_4
                 ,(int)param_5);
    }
  }
  return;
}

/* Address: 0x00500D30.
   Ownership: world/terrain/projection.
   Purpose: Handles terrain projected grid clip row spans against plane.
*/
undefined8 TerrainProjectedGrid_ClipRowSpansAgainstPlane(int param_1,int *param_2)

{
  longlong lVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_EDX;
  uint uVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  
  if (*param_2 == 0) {
    if (param_2[1] != 0) {
      if (param_2[1] < 0) {
        iVar4 = 0;
        if (-1 < param_2[2]) {
          iVar4 = (int)(((longlong)g_ViewOriginFixed.z * (longlong)param_2[2]) /
                       (longlong)param_2[1]);
        }
        lVar1 = (longlong)(iVar4 + g_ViewOriginFixed.y) * -0x20c8cc;
        iVar2 = (int)((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) >> 0xc;
        iVar4 = *(int *)(param_1 + 0xbc) - iVar2;
        if ((iVar4 != 0 && iVar2 <= *(int *)(param_1 + 0xbc)) && (iVar4 = iVar4 + -1, iVar4 != 0)) {
          puVar6 = (undefined4 *)(iVar2 * 8 + 0x4ffc98);
          for (iVar4 = iVar4 * 2; iVar4 != 0; iVar4 = iVar4 + -1) {
            *puVar6 = 0;
            puVar6 = puVar6 + 1;
          }
        }
      }
      else {
        iVar4 = 0;
        if (-1 < param_2[2]) {
          iVar4 = (int)(((longlong)g_ViewOriginFixed.z * (longlong)param_2[2]) /
                       (longlong)param_2[1]);
        }
        lVar1 = (longlong)(iVar4 + g_ViewOriginFixed.y) * -0x20c8cc;
        iVar4 = (int)((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) >> 0xc;
        if ((-1 < iVar4) && (iVar4 != 0)) {
          puVar6 = &DAT_004ffc80;
          for (iVar4 = iVar4 * 2; iVar4 != 0; iVar4 = iVar4 + -1) {
            *puVar6 = 0;
            puVar6 = puVar6 + 1;
          }
        }
      }
    }
  }
  else if (*param_2 < 0) {
    lVar1 = (longlong)param_2[1] * (longlong)g_ViewOriginFixed.y +
            (longlong)*param_2 * (longlong)g_ViewOriginFixed.x;
    if (-1 < param_2[2]) {
      lVar1 = lVar1 + (longlong)param_2[2] * (longlong)g_ViewOriginFixed.z;
    }
    lVar1 = (longlong)(int)(lVar1 / (longlong)*param_2) * 0x1c6e9c;
    uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14;
    lVar1 = (longlong)(int)(((longlong)param_2[1] * 1999) / (longlong)*param_2) * 0x1c6e9c;
    piVar5 = (int *)0x4ffc78;
    iVar4 = *(int *)(param_1 + 0xbc);
    do {
      piVar5 = piVar5 + 2;
      iVar2 = (int)(uVar3 - 0x1000) >> 0xc;
      uVar3 = uVar3 + (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - 0x800);
      if (*piVar5 < iVar2) {
        *piVar5 = iVar2;
      }
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  else {
    lVar1 = (longlong)param_2[1] * (longlong)g_ViewOriginFixed.y +
            (longlong)*param_2 * (longlong)g_ViewOriginFixed.x;
    if (-1 < param_2[2]) {
      lVar1 = lVar1 + (longlong)param_2[2] * (longlong)g_ViewOriginFixed.z;
    }
    lVar1 = (longlong)(int)(lVar1 / (longlong)*param_2) * 0x1c6e9c;
    uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14;
    lVar1 = (longlong)(int)(((longlong)param_2[1] * 1999) / (longlong)*param_2) * 0x1c6e9c;
    piVar5 = (int *)0x4ffc7c;
    iVar4 = *(int *)(param_1 + 0xbc);
    do {
      piVar5 = piVar5 + 2;
      iVar2 = (int)(uVar3 + 0x1fff) >> 0xc;
      uVar3 = uVar3 + (((int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14) - 0x800);
      if (iVar2 < *piVar5) {
        *piVar5 = iVar2;
      }
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005063B0.
   Ownership: world/terrain/projection.
   Purpose: Scans directional terrain run 0, stops at excluded cells, converts the current cell height relative to
   the shared origin through the distance scale table, and ORs the incoming 64-bit mask when the projected
   threshold is met. Typed parameters: p2 projectedHeightThresholdQ20→TerrainProjectedHeightThresholdQ20_V342, p3
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainProjectedOcclusion_ScanDirection0
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  ulonglong in_MM0;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
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
void TerrainProjectedOcclusion_ScanDirection1
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  ulonglong in_MM0;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
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
void TerrainProjectedOcclusion_ScanDirection2
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  ulonglong in_MM0;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
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
void TerrainProjectedOcclusion_ScanDirection3
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  ulonglong in_MM0;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
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
void TerrainProjectedOcclusion_ScanDirection4
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  ulonglong in_MM0;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes);
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
void TerrainProjectedOcclusion_ScanDirection5
               (TerrainProjectedHeightThresholdQ20 projectedHeightThresholdQ20,
               TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  ulonglong in_MM0;
  
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
        cell->occupancyMask = cell->occupancyMask | in_MM0;
        projectedHeightThresholdQ20 = uVar3;
      }
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes);
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
void FieldGridTerrainOverlayVariantA_ApplyDirection0
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantA_ApplyDirection1
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantA_ApplyDirection2
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantA_ApplyDirection3
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantA_ApplyDirection4
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime00_07 + g_TerrainScanRowStrideBytes);
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
void FieldGridTerrainOverlayVariantA_ApplyDirection5
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (((fieldCell->flagsAndMaterial & g_TerrainScanSharedSelectorValue.fieldCellFlagMask) != 0)
         && (fieldCell->waterSurfaceDelta < 0)) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell->runtime00_07 + g_TerrainScanRowStrideBytes);
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
void FieldGridTerrainOverlayVariantB_ApplyDirection0
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantB_ApplyDirection1
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantB_ApplyDirection2
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantB_ApplyDirection3
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
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
void FieldGridTerrainOverlayVariantB_ApplyDirection4
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell[-1].runtime00_07 + g_TerrainScanRowStrideBytes);
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
void FieldGridTerrainOverlayVariantB_ApplyDirection5
               (TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell)

{
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((fieldCell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        *(undefined4 *)(fieldCell->runtime00_07 + 4) = g_TerrainScanReferenceHeight;
      }
      scanStep = scanStep + 4;
      fieldCell = (FieldGridCell *)(fieldCell->runtime00_07 + g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}
