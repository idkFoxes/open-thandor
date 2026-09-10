#include <thandor/world/terrain/editing.h>

/* Implementation ownership: world/terrain/editing. */

/* Address: 0x005137F0.
   Ownership: world/terrain/editing.
   Purpose: Recursively expands a connected field-cell region through the verified neighboring rows and columns,
   recording cells whose occupancy mask matches and whose material flags do not reject traversal. Typed parameters:
   p2 requiredOccupancyMask→FieldGridRegionMask, p3 rowStrideBytes→FieldGridRowStrideBytes. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainRegionCollection_RecordConnectedCell.
*/
undefined8 __fastcall
TerrainRegionCollection_CollectConnectedCellsRecursive
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,
          FieldGridRegionMask requiredOccupancyMask,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell)

{
  undefined4 in_EAX;
  FieldCellPackedFlagsAndMaterial extraout_EAX;
  FieldCellPackedFlagsAndMaterial extraout_EAX_00;
  FieldCellPackedFlagsAndMaterial FVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  FieldGridCell *rowCellCursor;
  FieldGridCell *pFVar2;
  FieldGridCell *pFVar3;
  FieldGridCell *pFVar4;
  undefined8 uVar5;
  
  pFVar3 = cell;
  do {
    pFVar2 = pFVar3;
    TerrainRegionCollection_RecordConnectedCell(requiredOccupancyMask,pFVar2);
    FVar1 = extraout_EAX;
    if ((pFVar2[-1].flagsAndMaterial & extraout_EAX) == 0) break;
    requiredOccupancyMask = extraout_EAX;
    pFVar3 = pFVar2 + -1;
  } while ((pFVar2[-1].flagsAndMaterial & 0x88016000) == 0);
  while ((pFVar3 = cell + 1, (cell[1].flagsAndMaterial & FVar1) != 0 &&
         ((cell[1].flagsAndMaterial & 0x88016000) == 0))) {
    TerrainRegionCollection_RecordConnectedCell(FVar1,pFVar3);
    FVar1 = extraout_EAX_00;
    cell = pFVar3;
  }
  uVar5 = CONCAT44(pFVar2,FVar1);
  pFVar2 = (FieldGridCell *)((int)pFVar2 - rowStrideBytes);
  pFVar4 = (FieldGridCell *)((int)pFVar3 - rowStrideBytes);
  do {
    if (((pFVar2->flagsAndMaterial & 0x88016000) == 0) &&
       ((pFVar2->flagsAndMaterial & (FieldCellPackedFlagsAndMaterial)uVar5) != 0)) {
      uVar5 = TerrainRegionCollection_CollectConnectedCellsRecursive
                        (rowStrideBytes,(int)((ulonglong)uVar5 >> 0x20),
                         (FieldCellPackedFlagsAndMaterial)uVar5,rowStrideBytes,pFVar2);
      rowStrideBytes = extraout_ECX;
    }
    FVar1 = (FieldCellPackedFlagsAndMaterial)uVar5;
    pFVar2 = pFVar2 + 1;
  } while (pFVar2 <= pFVar4);
  rowCellCursor = (FieldGridCell *)(rowStrideBytes + -0x80 + (int)((ulonglong)uVar5 >> 0x20));
  pFVar3 = (FieldGridCell *)(pFVar3->runtime00_07 + rowStrideBytes);
  do {
    uVar5 = CONCAT44(rowCellCursor,FVar1);
    if (((rowCellCursor->flagsAndMaterial & 0x88016000) == 0) &&
       (uVar5 = CONCAT44(rowCellCursor,FVar1), (rowCellCursor->flagsAndMaterial & FVar1) != 0)) {
      uVar5 = TerrainRegionCollection_CollectConnectedCellsRecursive
                        (rowStrideBytes,rowCellCursor,FVar1,rowStrideBytes,rowCellCursor);
      rowStrideBytes = extraout_ECX_00;
    }
    rowCellCursor = (FieldGridCell *)((ulonglong)uVar5 >> 0x20);
    FVar1 = (FieldCellPackedFlagsAndMaterial)uVar5;
    rowCellCursor = rowCellCursor + 1;
  } while (rowCellCursor < pFVar3);
  return CONCAT44(preservedEdxValue,in_EAX);
}

/* Address: 0x00561A10.
   Ownership: world/terrain/editing.
   Purpose: Clears the selected player material edit buffer, converts world coordinates to a field cell, captures
   the original and replacement material bytes, marks the field dirty, and starts matching-region propagation. It
   is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset
   identifiers. Typed parameters: p4 worldYQ12→Q12, p5 worldXQ12→Q12. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: TerrainMaterialEdit_PropagateMatchingRegionReplacement.
*/
void TerrainMaterialEdit_SeedMatchingRegionReplacement
               (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue replacementMaterialByte,
               Q12 worldYQ12,Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  FieldGridAsset *pFVar2;
  TerrainMaterialIndex FVar3;
  int iVar3;
  int gridY;
  dword *pdVar4;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  pFVar2 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pdVar4 = pSVar1->terrainMaterialEditPlane808C;
  for (iVar3 = pFVar2->gridWidth * pFVar2->gridHeight; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pdVar4 = 0;
    pdVar4 = pdVar4 + 1;
  }
  iVar3 = worldXQ12 >> 0xc;
  if ((((-1 < iVar3) && (gridY = worldYQ12 >> 0xc, -1 < gridY)) && (iVar3 < (int)pFVar2->gridWidth))
     && (gridY < (int)pFVar2->gridHeight)) {
    FVar3 = pFVar2->cells[gridY * pFVar2->gridWidth + iVar3].flagsAndMaterial &
            FIELD_CELL_MATERIAL_ID_MASK;
    pdVar4 = pSVar1->terrainMaterialEditPlane808C;
    if (FVar3 != replacementMaterialByte) {
      pFVar2->runtimeStateFlags = pFVar2->runtimeStateFlags | 1;
      g_TerrainMaterialEditReplacementMaterialByte = replacementMaterialByte;
      g_TerrainMaterialEditFieldGrid = pFVar2;
      g_TerrainMaterialEditDeltaBuffer = pdVar4;
      g_TerrainMaterialEditReferenceMaterialByte = FVar3;
      TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY,iVar3);
    }
  }
  return;
}

/* Address: 0x00561AE0.
   Ownership: world/terrain/editing.
   Purpose: Clears the selected player material edit buffer, converts world coordinates to a field cell, records
   the requested material byte, marks the field dirty, and starts propagation through connected non-target cells.
   It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset
   identifiers. Typed parameters: p4 worldYQ12→Q12, p5 worldXQ12→Q12. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: TerrainMaterialEdit_PropagateNonTargetRegionReplacement.
*/
void TerrainMaterialEdit_SeedNonTargetRegionReplacement
               (FrontendPlayerIndex playerIndex,TerrainMaterialByteValue referenceMaterialByte,
               Q12 worldYQ12,Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  FieldGridAsset *pFVar2;
  int iVar3;
  int gridY;
  dword *pdVar4;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  pFVar2 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pdVar4 = pSVar1->terrainMaterialEditPlane808C;
  for (iVar3 = pFVar2->gridWidth * pFVar2->gridHeight; iVar3 != 0; iVar3 = iVar3 + -1) {
    *pdVar4 = 0;
    pdVar4 = pdVar4 + 1;
  }
  iVar3 = worldXQ12 >> 0xc;
  if ((((-1 < iVar3) && (gridY = worldYQ12 >> 0xc, -1 < gridY)) && (iVar3 < (int)pFVar2->gridWidth))
     && (gridY < (int)pFVar2->gridHeight)) {
    pdVar4 = pSVar1->terrainMaterialEditPlane808C;
    if ((pFVar2->cells[gridY * pFVar2->gridWidth + iVar3].flagsAndMaterial &
        FIELD_CELL_MATERIAL_ID_MASK) != referenceMaterialByte) {
      pFVar2->runtimeStateFlags = pFVar2->runtimeStateFlags | 1;
      g_TerrainMaterialEditReferenceMaterialByte = referenceMaterialByte;
      g_TerrainMaterialEditFieldGrid = pFVar2;
      g_TerrainMaterialEditDeltaBuffer = pdVar4;
      TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY,iVar3);
    }
  }
  return;
}

/* Address: 0x005616D0.
   Ownership: world/terrain/editing.
   Purpose: Commits per-cell terrain-height edit deltas from a player/runtime edit buffer into the FieldGrid and
   recomputes affected triangle normals and directional lighting.
   Cross-module calls: FieldGridCell_RecomputeTriangleNormalAngles [world/terrain/grid],
   FieldGridCell_ComputeDirectionalLightColor [world/terrain/grid].
*/
void TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
               (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  FieldGridCell *cell;
  FieldGridAsset *pFVar1;
  int iVar2;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int rowStrideBytes;
  FieldGridRowStrideBytes rowStrideBytes_00;
  FieldGridRowStrideBytes extraout_EDX;
  int extraout_EDX_00;
  FieldGridRowStrideBytes extraout_EDX_01;
  int extraout_EDX_02;
  FieldGridRowStrideBytes extraout_EDX_03;
  int extraout_EDX_04;
  FieldGridCell *pFVar4;
  int *piVar5;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar5 = g_SelectionPlayerRuntimeBlockPointers[commandArg0]->terrainHeightScratchPlane8088;
  iVar3 = pFVar1->gridWidth * pFVar1->gridHeight;
  pFVar1->runtimeStateFlags = pFVar1->runtimeStateFlags | 1;
  rowStrideBytes = pFVar1->gridWidth << 7;
  pFVar4 = pFVar1->cells;
  do {
    iVar2 = *piVar5;
    if (iVar2 != 0) {
      pFVar4->terrainHeight = pFVar4->terrainHeight - iVar2;
      pFVar4->waterSurfaceDelta = pFVar4->waterSurfaceDelta + iVar2;
      *piVar5 = -*piVar5;
      if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
        FieldGridCell_ComputeDirectionalLightColor(pFVar4);
        iVar3 = extraout_ECX;
        rowStrideBytes = rowStrideBytes_00;
        if (((pFVar4[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar5[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + -1);
          iVar3 = extraout_ECX_00;
          rowStrideBytes = extraout_EDX;
        }
        if (((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) && (piVar5[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
          iVar3 = extraout_ECX_01;
          rowStrideBytes = extraout_EDX_00;
        }
        pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes);
        if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar3 = extraout_ECX_02;
          rowStrideBytes = extraout_EDX_01;
        }
        if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
          iVar3 = extraout_ECX_03;
          rowStrideBytes = extraout_EDX_02;
        }
        cell = (FieldGridCell *)(pFVar4[-1].runtime00_07 + rowStrideBytes * 2);
        if ((cell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
          iVar3 = extraout_ECX_04;
          rowStrideBytes = extraout_EDX_03;
        }
        pFVar4 = cell + 1;
        if ((cell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar3 = extraout_ECX_05;
          rowStrideBytes = extraout_EDX_04;
        }
        pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes);
      }
    }
    pFVar4 = pFVar4 + 1;
    piVar5 = piVar5 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}

/* Address: 0x00561830.
   Ownership: world/terrain/editing.
   Purpose: Copies the low material byte from every 0x80-byte field cell into the selected player terrain-edit
   buffer at runtime offset +0x808C. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers.
*/
void __fastcall
TerrainEditBuffer_CopyCellMaterialBytes
          (undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  TerrainMaterialIndex *pFVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pFVar4 = (TerrainMaterialIndex *)
           g_SelectionPlayerRuntimeBlockPointers[playerIndex]->terrainMaterialEditPlane808C;
  iVar2 = pFVar1->gridWidth * pFVar1->gridHeight;
  pFVar3 = pFVar1->cells;
  do {
    *pFVar4 = pFVar3->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
    pFVar3 = pFVar3 + 1;
    pFVar4 = pFVar4 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/* Address: 0x00561930.
   Ownership: world/terrain/editing.
   Purpose: It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset
   identifiers.
*/
void __fastcall
TerrainEditBuffer_SubtractCurrentCellMaterialBytes
          (undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  dword *pdVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pdVar4 = g_SelectionPlayerRuntimeBlockPointers[playerIndex]->terrainMaterialEditPlane808C;
  iVar2 = pFVar1->gridWidth * pFVar1->gridHeight;
  pFVar3 = pFVar1->cells;
  do {
    *pdVar4 = *pdVar4 - (pFVar3->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK);
    pFVar3 = pFVar3 + 1;
    pdVar4 = pdVar4 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/* Address: 0x005619A0.
   Ownership: world/terrain/editing.
   Purpose: Commits per-cell flags/material edit deltas from the corresponding player/runtime edit buffer into
   FieldGridCell flags/material state.
*/
void TerrainEditBuffer_CommitFlagsAndMaterialDeltas
               (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  dword *pdVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pdVar4 = g_SelectionPlayerRuntimeBlockPointers[commandArg0]->terrainMaterialEditPlane808C;
  iVar2 = pFVar1->gridWidth * pFVar1->gridHeight;
  *(uint *)(pFVar1[-1].sourcePath + 0x1a) = *(uint *)(pFVar1[-1].sourcePath + 0x1a) | 1;
  pFVar3 = pFVar1->cells;
  do {
    pFVar3->flagsAndMaterial = pFVar3->flagsAndMaterial + *pdVar4;
    *pdVar4 = -*pdVar4;
    pFVar3 = pFVar3 + 1;
    pdVar4 = pdVar4 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/* Address: 0x00561DC0.
   Ownership: world/terrain/editing.
   Purpose: Replaces each selected-player height-buffer value with fieldCell.terrainHeight minus the previous
   buffer value. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed
   asset identifiers.
*/
void __fastcall
TerrainEditBuffer_ConvertHeightsToDeltas
          (undefined4 param_1,undefined4 param_2,FrontendPlayerIndex playerIndex)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  int *piVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar4 = g_SelectionPlayerRuntimeBlockPointers[playerIndex]->terrainHeightScratchPlane8088;
  iVar2 = pFVar1->gridWidth * pFVar1->gridHeight;
  pFVar3 = pFVar1->cells;
  do {
    *piVar4 = pFVar3->terrainHeight - *piVar4;
    pFVar3 = pFVar3 + 1;
    piVar4 = piVar4 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/* Address: 0x00513790.
   Ownership: world/terrain/editing.
   Purpose: Marks one field cell as visited, removes matching occupancy bits, and appends the original occupancy
   mask and optional rebased model-runtime pointer to the bounded connected-region collection. Typed parameters: p2
   requiredOccupancyMask→FieldGridRegionMask. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void TerrainRegionCollection_RecordConnectedCell
               (FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell)

{
  byte *pbVar1;
  undefined4 uVar2;
  uint uVar3;
  TerrainRegionCollectionCount TVar4;
  int iVar5;
  
  TVar4 = g_TerrainRegionCollectionStoredCount;
  g_TerrainRegionCollectionVisitedCount = g_TerrainRegionCollectionVisitedCount + 1;
  uVar3 = cell->runtime7C;
  cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_CONNECTED_REGION_VISITED;
  if ((requiredOccupancyMask & uVar3) != 0) {
    cell->runtime7C = 0;
    LOCK();
    pbVar1 = cell->runtime58_6F + 0x14;
    uVar2 = *(undefined4 *)pbVar1;
    pbVar1[0] = 0;
    iVar5 = g_TerrainRegionCollectionEntries;
    pbVar1[1] = 0;
    pbVar1[2] = 0;
    pbVar1[3] = 0;
    UNLOCK();
    if (TVar4 < 0x800) {
      g_TerrainRegionCollectionStoredCount = g_TerrainRegionCollectionStoredCount + 1;
      *(uint *)(g_TerrainRegionCollectionEntries + TVar4 * 8) = uVar3;
      *(undefined4 *)(iVar5 + 4 + TVar4 * 8) = uVar2;
    }
  }
  return;
}

/* Address: 0x00571600.
   Ownership: world/terrain/editing.
   Purpose: Recursively replaces a connected region whose material equals the captured original byte, updates the
   paired delta buffer, and propagates through neighboring rows and columns. Typed parameters: p0
   gridY→FieldGridCellCoordinate_V331, p1 gridX→FieldGridCellCoordinate_V331. Calling convention, parameter
   storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainMaterialEdit_PropagateMatchingRegionReplacement
               (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int extraout_ECX;
  FieldGridCellCoordinate gridY_00;
  FieldGridCellCoordinate extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar6;
  int *piVar7;
  int *piVar8;
  int iVar9;
  
  uVar1 = g_TerrainMaterialEditReferenceMaterialByte;
  if ((((-1 < gridY) && (-1 < gridX)) &&
      (iVar5 = *(int *)(g_TerrainMaterialEditFieldGrid + 0xb8),
      gridY < *(int *)(g_TerrainMaterialEditFieldGrid + 0xbc))) && (gridX < iVar5)) {
    iVar2 = gridY * iVar5 + gridX;
    piVar8 = (int *)(g_TerrainMaterialEditDeltaBuffer + iVar2 * 4);
    iVar2 = iVar2 * 0x80 + 0x200 + g_TerrainMaterialEditFieldGrid;
    uVar3 = *(uint *)(iVar2 + 0x50) & 0xff;
    iVar4 = gridX;
    iVar6 = iVar2;
    piVar7 = piVar8;
    if (g_TerrainMaterialEditReferenceMaterialByte == uVar3) {
      do {
        iVar9 = iVar4;
        iVar4 = uVar3 - g_TerrainMaterialEditReplacementMaterialByte;
        *(int *)(iVar6 + 0x50) = *(int *)(iVar6 + 0x50) - iVar4;
        *piVar7 = *piVar7 + iVar4;
        if (iVar9 < 1) break;
        uVar3 = *(uint *)(iVar6 + -0x30) & 0xff;
        iVar4 = iVar9 + -1;
        iVar6 = iVar6 + -0x80;
        piVar7 = piVar7 + -1;
      } while (uVar1 == uVar3);
      LOCK();
      UNLOCK();
      while( true ) {
        gridX = gridX + 1;
        piVar8 = piVar8 + 1;
        if ((iVar5 <= gridX) || (uVar3 = *(uint *)(iVar2 + 0xd0) & 0xff, uVar1 != uVar3)) break;
        iVar4 = uVar3 - g_TerrainMaterialEditReplacementMaterialByte;
        *(int *)(iVar2 + 0xd0) = *(int *)(iVar2 + 0xd0) - iVar4;
        *piVar8 = *piVar8 + iVar4;
        iVar2 = iVar2 + 0x80;
      }
      iVar5 = gridY + -1;
      iVar2 = iVar9;
      do {
        iVar4 = iVar2 + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(iVar5,iVar2);
        iVar5 = extraout_ECX;
        iVar2 = iVar4;
      } while (iVar4 <= extraout_EDX);
      gridY_00 = extraout_ECX + 2;
      iVar5 = iVar9 + -1;
      do {
        iVar2 = iVar5 + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY_00,iVar5);
        gridY_00 = extraout_ECX_00;
        iVar5 = iVar2;
      } while (iVar2 < extraout_EDX_00);
    }
  }
  return;
}

/* Address: 0x00571730.
   Ownership: world/terrain/editing.
   Purpose: Recursively replaces connected cells whose material differs from the requested target byte, updates the
   paired delta buffer, and propagates through neighboring rows and columns. Typed parameters: p0
   gridY→FieldGridCellCoordinate_V331, p1 gridX→FieldGridCellCoordinate_V331. Calling convention, parameter
   storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainMaterialEdit_PropagateNonTargetRegionReplacement
               (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int extraout_ECX;
  FieldGridCellCoordinate gridY_00;
  FieldGridCellCoordinate extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar6;
  int *piVar7;
  int *piVar8;
  int iVar9;
  
  uVar1 = g_TerrainMaterialEditReferenceMaterialByte;
  if ((((-1 < gridY) && (-1 < gridX)) &&
      (iVar5 = *(int *)(g_TerrainMaterialEditFieldGrid + 0xb8),
      gridY < *(int *)(g_TerrainMaterialEditFieldGrid + 0xbc))) && (gridX < iVar5)) {
    iVar2 = gridY * iVar5 + gridX;
    piVar8 = (int *)(g_TerrainMaterialEditDeltaBuffer + iVar2 * 4);
    iVar2 = iVar2 * 0x80 + 0x200 + g_TerrainMaterialEditFieldGrid;
    uVar3 = *(uint *)(iVar2 + 0x50) & 0xff;
    iVar4 = gridX;
    iVar6 = iVar2;
    piVar7 = piVar8;
    if (g_TerrainMaterialEditReferenceMaterialByte != uVar3) {
      do {
        iVar9 = iVar4;
        *(int *)(iVar6 + 0x50) = *(int *)(iVar6 + 0x50) - (uVar3 - uVar1);
        *piVar7 = *piVar7 + (uVar3 - uVar1);
        if (iVar9 < 1) break;
        uVar3 = *(uint *)(iVar6 + -0x30) & 0xff;
        iVar4 = iVar9 + -1;
        iVar6 = iVar6 + -0x80;
        piVar7 = piVar7 + -1;
      } while (uVar1 != uVar3);
      LOCK();
      UNLOCK();
      while( true ) {
        gridX = gridX + 1;
        piVar8 = piVar8 + 1;
        if ((iVar5 <= gridX) || (uVar3 = *(uint *)(iVar2 + 0xd0) & 0xff, uVar1 == uVar3)) break;
        iVar4 = uVar3 - uVar1;
        *(int *)(iVar2 + 0xd0) = *(int *)(iVar2 + 0xd0) - iVar4;
        *piVar8 = *piVar8 + iVar4;
        iVar2 = iVar2 + 0x80;
      }
      iVar5 = gridY + -1;
      iVar2 = iVar9;
      do {
        iVar4 = iVar2 + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(iVar5,iVar2);
        iVar5 = extraout_ECX;
        iVar2 = iVar4;
      } while (iVar4 <= extraout_EDX);
      gridY_00 = extraout_ECX + 2;
      iVar5 = iVar9 + -1;
      do {
        iVar2 = iVar5 + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY_00,iVar5);
        gridY_00 = extraout_ECX_00;
        iVar5 = iVar2;
      } while (iVar2 < extraout_EDX_00);
    }
  }
  return;
}
