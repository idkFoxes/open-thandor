/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/editing.c
 * Reverse engineering by idkFoxes 2026
 */

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
void __thandor_void_preserve_eax_ecx_edx
TerrainRegionCollection_CollectConnectedCellsRecursive
          (FieldGridRegionMask requiredOccupancyMask,FieldGridRowStrideBytes rowStrideBytes,
          FieldGridCell *cell)

{
  FieldGridCell *rowCellCursor;
  FieldGridCell *cell_00;
  FieldGridCell *cell_01;
  FieldGridCell *pFVar1;
  
  pFVar1 = cell;
  do {
    cell_00 = pFVar1;
    TerrainRegionCollection_RecordConnectedCell(requiredOccupancyMask,cell_00);
    if ((cell_00[-1].flagsAndMaterial & requiredOccupancyMask) == 0) break;
    pFVar1 = cell_00 + -1;
  } while ((cell_00[-1].flagsAndMaterial & 0x88016000) == 0);
  while ((pFVar1 = cell + 1, (cell[1].flagsAndMaterial & requiredOccupancyMask) != 0 &&
         ((cell[1].flagsAndMaterial & 0x88016000) == 0))) {
    TerrainRegionCollection_RecordConnectedCell(requiredOccupancyMask,pFVar1);
    cell = pFVar1;
  }
  cell_01 = (FieldGridCell *)((int)cell_00 - rowStrideBytes);
  do {
    if (((cell_01->flagsAndMaterial & 0x88016000) == 0) &&
       ((cell_01->flagsAndMaterial & requiredOccupancyMask) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredOccupancyMask,rowStrideBytes,cell_01);
    }
    cell_01 = cell_01 + 1;
  } while (cell_01 <= (FieldGridCell *)((int)pFVar1 - rowStrideBytes));
  rowCellCursor = (FieldGridCell *)(cell_00[-1].runtime0C_3F + rowStrideBytes + -0xc);
  do {
    if (((rowCellCursor->flagsAndMaterial & 0x88016000) == 0) &&
       ((rowCellCursor->flagsAndMaterial & requiredOccupancyMask) != 0)) {
      TerrainRegionCollection_CollectConnectedCellsRecursive
                (requiredOccupancyMask,rowStrideBytes,rowCellCursor);
    }
    rowCellCursor = rowCellCursor + 1;
  } while (rowCellCursor < (FieldGridCell *)(pFVar1->runtime0C_3F + rowStrideBytes + -0xc));
  return;
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
void __thandor_void_preserve_eax_ecx_edx
TerrainMaterialEdit_SeedMatchingRegionReplacement
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
void __thandor_void_preserve_eax_ecx_edx
TerrainMaterialEdit_SeedNonTargetRegionReplacement
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
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3)

{
  FieldGridAsset *pFVar1;
  FieldGridDimension FVar2;
  int iVar3;
  int iVar4;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *pFVar5;
  int *piVar6;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar6 = g_SelectionPlayerRuntimeBlockPointers[commandArg0]->terrainHeightScratchPlane8088;
  FVar2 = pFVar1->gridWidth;
  iVar4 = FVar2 * pFVar1->gridHeight;
  pFVar1->runtimeStateFlags = pFVar1->runtimeStateFlags | 1;
  rowStrideBytes = FVar2 * 0x80;
  pFVar5 = pFVar1->cells;
  do {
    iVar3 = *piVar6;
    if (iVar3 != 0) {
      pFVar5->terrainHeight = pFVar5->terrainHeight - iVar3;
      pFVar5->waterSurfaceDelta = pFVar5->waterSurfaceDelta + iVar3;
      *piVar6 = -*piVar6;
      if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5);
        if (((pFVar5[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar6[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + -1);
        }
        if (((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) && (piVar6[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
        }
        pFVar5 = pFVar5 + -FVar2;
        if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5);
        }
        if ((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
        }
        pFVar5 = pFVar5 + FVar2 * 2 + -1;
        if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5);
        }
        cell = pFVar5 + 1;
        if ((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        pFVar5 = cell + -FVar2;
      }
    }
    pFVar5 = pFVar5 + 1;
    piVar6 = piVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}


/* Address: 0x00561830.
   Ownership: world/terrain/editing.
   Purpose: Copies the low material byte from every 0x80-byte field cell into the selected player terrain-edit
   buffer at runtime offset +0x808C. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CopyCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  TerrainMaterialIndex *pFVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pFVar4 = (TerrainMaterialIndex *)
           g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane808C;
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
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_SubtractCurrentCellMaterialBytes
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  dword *pdVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  pdVar4 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainMaterialEditPlane808C;
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
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_CommitFlagsAndMaterialDeltas
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
void __thandor_void_preserve_eax_ecx_edx
TerrainEditBuffer_ConvertHeightsToDeltas
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  FieldGridCell *pFVar3;
  int *piVar4;
  
  pFVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar4 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
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

void __thandor_void_preserve_eax_ecx_edx
TerrainRegionCollection_RecordConnectedCell
          (FieldGridRegionMask requiredOccupancyMask,FieldGridCell *cell)

{
  ArmyRuntimeSavedOffset AVar1;
  uint uVar2;
  TerrainRegionCollectionCount TVar3;
  int iVar4;
  
  TVar3 = g_TerrainRegionCollectionStoredCount;
  g_TerrainRegionCollectionVisitedCount = g_TerrainRegionCollectionVisitedCount + 1;
  uVar2 = cell->resourceExtractionDescriptor7C;
  cell->flagsAndMaterial = cell->flagsAndMaterial | FIELD_CELL_CONNECTED_REGION_VISITED;
  if ((requiredOccupancyMask & uVar2) != 0) {
    cell->resourceExtractionDescriptor7C = 0;
    LOCK();
    AVar1 = cell->armyRuntimeSavedOffset6C;
    cell->armyRuntimeSavedOffset6C = 0;
    iVar4 = g_TerrainRegionCollectionEntries;
    UNLOCK();
    if (TVar3 < 0x800) {
      g_TerrainRegionCollectionStoredCount = g_TerrainRegionCollectionStoredCount + 1;
      *(uint *)(g_TerrainRegionCollectionEntries + TVar3 * 8) = uVar2;
      *(ArmyRuntimeSavedOffset *)(iVar4 + 4 + TVar3 * 8) = AVar1;
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
void __thandor_eax_preserve_ecx_edx
TerrainMaterialEdit_PropagateMatchingRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
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
      iVar5 = iVar9;
      do {
        iVar2 = iVar5 + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY + -1,iVar5);
        iVar5 = iVar2;
      } while (iVar2 <= gridX);
      iVar5 = iVar9 + -1;
      do {
        iVar2 = iVar5 + 1;
        TerrainMaterialEdit_PropagateMatchingRegionReplacement(gridY + 1,iVar5);
        iVar5 = iVar2;
      } while (iVar2 < gridX);
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
void __thandor_eax_preserve_ecx_edx
TerrainMaterialEdit_PropagateNonTargetRegionReplacement
          (FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
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
      iVar5 = iVar9;
      do {
        iVar2 = iVar5 + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY + -1,iVar5);
        iVar5 = iVar2;
      } while (iVar2 <= gridX);
      iVar5 = iVar9 + -1;
      do {
        iVar2 = iVar5 + 1;
        TerrainMaterialEdit_PropagateNonTargetRegionReplacement(gridY + 1,iVar5);
        iVar5 = iVar2;
      } while (iVar2 < gridX);
    }
  }
  return;
}

