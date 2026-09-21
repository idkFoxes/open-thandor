/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/placement.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/placement.h>

/* Implementation ownership: gameplay/ai/placement. */

/* Address: 0x0053A110.
   Ownership: gameplay/ai/placement.
   Purpose: Queries the mode-seven placement count for one workspace record and, when the count falls within the
   supported range, requests an additional separated special-site record using the remainder position. Stock ARM
   contains 675 records and 326 unique ids; placement workspace, producer, tier, class, and faction-role semantics
   are not inferred from numeric adjacency. Typed parameters: p3
   workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: AiPlacement_ReserveSeparatedSpecialSiteChain.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_ReserveAdditionalSpecialSite
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *pAVar1;
  dword dVar2;
  uint uVar3;
  bool bVar4;
  ArmyPlacementDispatchEaxCf5 AVar5;
  
  pAVar1 = g_AiKnowledgeData;
  uVar3 = (uint)(ushort)workspaceRecord->triangle0NormalAngles;
  AVar5 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (7,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,uVar3,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  dVar2 = AVar5.eax;
  if ((!AVar5.carry) && (dVar2 != 0)) {
    AVar5 = ArmyPlacement_DispatchAssetAtFieldPoint
                      (4,0,uVar3,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                       factionIndex,(UiRootNode *)worldRuntime);
    if ((AVar5.carry) &&
       (uVar3 = (pAVar1->parameters).specialSiteSeparationQuantumQ12,
       ((dVar2 - 1) + uVar3) / uVar3 < 5)) {
      bVar4 = AiPlacement_ReserveSeparatedSpecialSiteChain
                        (armyAssetId,workspaceRecord,factionIndex,worldRuntime);
      return bVar4;
    }
  }
  return true;
}


/* Address: 0x0053AE60.
   Ownership: gameplay/ai/placement.
   Purpose: Chooses among special candidate IDs 0x12D, 0x32, and 0x0B according to current primary and secondary
   workspace state plus faction flags, then adds the selected candidate with the computed special-site weight. It
   is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Local calls: AiCandidatePlanning_ComputeSpecialSiteWeight.
   Cross-module calls: AiSecondaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces],
   AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces], ArmyAssetRegistry_FindEnabledByIdCf
   [assets/army/catalog], AiCandidateWorkspace_AddOrAccumulateWeightedEntry [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_eax_ecx_edx
AiCandidatePlanning_AddSpecialSiteCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  bool bVar1;
  AiCandidateScoreEaxCf5 AVar2;
  
  bVar1 = AiSecondaryWorkspace_HasEntryByIdCf(ARM_0050_UNIT_MDL0103);
  if (!bVar1) {
    bVar1 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0301_BUILDING_MDL0318);
    if (bVar1) {
      bVar1 = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0050_UNIT_MDL0103);
      if (!bVar1) {
        if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[0] & 0x800) == 0
           ) {
          AVar2 = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          if (!AVar2.carry) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0xb,AVar2.score,2);
          }
        }
        else {
          AVar2 = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          if (!AVar2.carry) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x32,AVar2.score,0);
          }
        }
      }
    }
    else {
      AVar2 = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
      if (!AVar2.carry) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x12d,AVar2.score,0);
      }
    }
  }
  return;
}


/* Address: 0x00537B20.
   Ownership: gameplay/ai/placement.
   Purpose: Adds the current field cell to the 32-entry general site workspace when it is sufficiently separated
   from existing entries. The stored score combines minimum Manhattan distances to three AI workspaces with
   configured knowledge weights. Score-A site filler (buffer 05, 32 entries): reads the KI Score-A block
   (+0x80..+0xA0) CORRECTLY — the contrast case for the Score-B wrong-base bug below.
   Cross-module calls: AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiWorkspace02_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_ecx_edx
AiSiteCandidate_AddGeneralCellIfSeparated(FieldGridCell *currentCell)

{
  Q12 worldY;
  Q12 worldX;
  dword dVar1;
  dword dVar2;
  dword dVar3;
  AiKnowledgeDataImage *pAVar4;
  int iVar5;
  dword dVar6;
  dword dVar7;
  uint uVar8;
  int iVar9;
  AiScoredSiteWorkspaceEntry *pAVar10;
  
  uVar8 = g_AiWorkspace05Count;
  pAVar10 = g_AiWorkspaceBuffer05_Size0200;
  while( true ) {
    if (uVar8 == 0) {
      worldY = currentCell->worldX;
      worldX = currentCell->worldY;
      if (g_AiWorkspace05Count < 0x20) {
        pAVar10->cellWorldXQ12 = worldY;
        pAVar10->cellWorldYQ12 = worldX;
        pAVar10->cell = currentCell;
        pAVar4 = g_AiKnowledgeData;
        dVar1 = (g_AiKnowledgeData->parameters).unknownParameterDword33;
        iVar5 = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        iVar5 = dVar1 - iVar5;
        if (iVar5 < 0) {
          iVar5 = 0;
        }
        dVar1 = (pAVar4->parameters).unknownParameterDwords36_37[1];
        dVar2 = (pAVar4->parameters).unknownParameterDwords36_37[0];
        dVar6 = AiWorkspace02_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if ((int)dVar2 < (int)dVar6) {
          dVar6 = dVar2;
        }
        dVar2 = (pAVar4->parameters).unknownParameterDwords40_47[0];
        dVar3 = (pAVar4->parameters).generalSiteSecondaryDistanceCapQ12;
        dVar7 = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if ((int)dVar3 < (int)dVar7) {
          dVar7 = dVar3;
        }
        g_AiWorkspace05Count = g_AiWorkspace05Count + 1;
        pAVar10->score =
             iVar5 * dVar1 + dVar6 * dVar2 +
             dVar7 * (pAVar4->parameters).generalSiteSecondaryDistanceCoefficient;
      }
      return;
    }
    iVar5 = pAVar10->cellWorldXQ12 - currentCell->worldX;
    if (iVar5 < 0) {
      iVar5 = -iVar5;
    }
    iVar9 = pAVar10->cellWorldYQ12 - currentCell->worldY;
    if (iVar9 < 0) {
      iVar9 = -iVar9;
    }
    if ((iVar5 < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12) &&
       (iVar9 < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12)) break;
    pAVar10 = pAVar10 + 1;
    uVar8 = uVar8 - 1;
  }
  return;
}


/* Address: 0x00537C10.
   Ownership: gameplay/ai/placement.
   Purpose: Adds the current field cell to the 64-entry flagged-site workspace when it is sufficiently separated
   from existing entries. Its score begins with the three-workspace distance result and applies the verified
   flagged-cell adjustment. THE SCORE-B WRONG-BASE BUG: score cap/weight are read from the WORKSPACE BUFFER base
   +0xC8/+0xD8 instead of the KI image (register- clobber), and buffer 06 (64 entries) has ZERO readers — the list
   is filled but never consumed.
   Cross-module calls: AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiWorkspace02_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces].
*/
void __thandor_void_preserve_ecx_edx
AiSiteCandidate_AddFlaggedCellIfSeparated(FieldGridCell *currentCell)

{
  Q12 worldY;
  Q12 worldX;
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  
  uVar3 = g_AiWorkspace06Count;
  pbVar6 = g_AiWorkspaceBuffer06_Size0400;
  while( true ) {
    if (uVar3 == 0) {
      worldY = currentCell->worldX;
      worldX = currentCell->worldY;
      if (g_AiWorkspace06Count < 0x40) {
        *(Q12 *)pbVar6 = worldY;
        *(Q12 *)(pbVar6 + 4) = worldX;
        *(FieldGridCell **)(pbVar6 + 0xc) = currentCell;
        iVar1 = *(int *)(pbVar6 + 0xc4);
        iVar5 = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        iVar1 = iVar1 - iVar5;
        if (iVar1 < 0) {
          iVar1 = 0;
        }
        iVar5 = *(int *)(pbVar6 + 0xd4);
        iVar4 = *(int *)(pbVar6 + 0xd0);
        iVar2 = AiWorkspace02_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if (iVar4 < iVar2) {
          iVar2 = iVar4;
        }
        iVar4 = iVar1 * iVar5 + iVar2 * *(int *)(pbVar6 + 0xe0);
        iVar1 = *(int *)(pbVar6 + 200);
        iVar5 = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if (-1 < iVar1 - iVar5) {
          iVar4 = iVar4 + (iVar1 - iVar5) * *(int *)(pbVar6 + 0xd8);
        }
        g_AiWorkspace06Count = g_AiWorkspace06Count + 1;
        *(int *)(pbVar6 + 8) = iVar4;
      }
      return;
    }
    iVar1 = *(int *)pbVar6 - currentCell->worldX;
    if (iVar1 < 0) {
      iVar1 = -iVar1;
    }
    iVar5 = *(int *)(pbVar6 + 4) - currentCell->worldY;
    if (iVar5 < 0) {
      iVar5 = -iVar5;
    }
    if ((iVar1 < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12) &&
       (iVar5 < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12)) break;
    pbVar6 = pbVar6 + 0x10;
    uVar3 = uVar3 - 1;
  }
  return;
}


/* Address: 0x00537CF0.
   Ownership: gameplay/ai/placement.
   Purpose: Adds a terrain-feature site candidate for class 0x14A or 0x14C after rejecting nearby model marker key
   1:5 and duplicate neighborhood entries. The stored priority is derived from the nearest compatible AI runtime
   entity. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Terrain-feature site split reads FLD +0x50 0x0800. This is FLD
   support state, not a render-color classification. [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired detached enum
   dictionary AiKnowledgePackedParameterIndex after transferring its complete value vocabulary to code annotation.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], FixedMath_Length2 [core/math/fixed].
*/
void __thandor_void_preserve_ecx_edx
AiSiteCandidate_AddTerrainFeatureCellIfSeparated
          (FieldGridCell *terrainFeatureCell,dword gridScratchRowStrideBytes)

{
  uint uVar1;
  int *piVar2;
  ModelRuntimeNode *modelNodeRuntime;
  dword dVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  PckArmyAssetIdCatalog PVar8;
  AiWorkspace00EntryView8 *pAVar9;
  ModelLookupEntryEaxCf5 MVar10;
  ModelLocalPointRegs12 MVar11;
  int local_20;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  
  uVar1 = g_AiWorkspace08Count;
  PVar8 = ARM_0330_BUILDING_MDL0303;
  uVar6 = (g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12 * 0x55 >> 8;
  iVar4 = g_AiWorkspace00Count;
  pAVar9 = g_AiWorkspaceBuffer00_Size0400;
  if ((terrainFeatureCell->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
    PVar8 = ARM_0332_BUILDING_MDL0302;
  }
  for (; terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200, iVar4 != 0; iVar4 = iVar4 + -1) {
    piVar2 = (int *)pAVar9->runtimeSlotAddressOrZero;
    if ((piVar2 != (int *)0x0) &&
       (modelNodeRuntime = (ModelRuntimeNode *)piVar2[1], *(int *)(*piVar2 + 0x4c) == 0xd)) {
      MVar10 = ModelLookupTable_ContainsPackedKeyCf
                         (1,5,(modelNodeRuntime->modelPayload).modelResource);
      if (!MVar10.carry) {
        MVar11 = ModelNodeRuntime_TransformLocalPointRegs(MVar10.entry,modelNodeRuntime);
        dVar3 = FixedMath_Length2(MVar11.ecx - terrainFeatureCell->worldY,
                                  MVar11.eax - terrainFeatureCell->worldX);
        if ((int)dVar3 < 0x2001) {
          return;
        }
      }
    }
    pAVar9 = pAVar9 + 1;
  }
  do {
    if (uVar1 == 0) {
      if (g_AiWorkspace08Count < 0x20) {
        terrainFeatureEntry->cell = terrainFeatureCell;
        terrainFeatureEntry->armyAssetId = PVar8;
        local_20 = 0x7fffffff;
        pAVar9 = g_AiWorkspaceBuffer00_Size0400;
        for (iVar4 = g_AiWorkspace00Count; iVar4 != 0; iVar4 = iVar4 + -1) {
          if (pAVar9->runtimeSlotAddressOrZero != 0) {
            iVar7 = *(int *)(pAVar9->runtimeSlotAddressOrZero + 4);
            iVar5 = *(int *)(iVar7 + 0x94) - terrainFeatureCell->worldX;
            if (iVar5 < 0) {
              iVar5 = -iVar5;
            }
            iVar7 = *(int *)(iVar7 + 0x98) - terrainFeatureCell->worldY;
            if (iVar7 < 0) {
              iVar7 = -iVar7;
            }
            if (((PVar8 == pAVar9->armyAssetId) &&
                (iVar5 < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12
                )) && (iVar7 < (int)(g_AiKnowledgeData->parameters).
                                    terrainFeatureMinimumAxisSeparationQ12)) {
              return;
            }
            if (iVar5 + iVar7 < local_20) {
              local_20 = iVar5 + iVar7;
            }
          }
          pAVar9 = pAVar9 + 1;
        }
        local_20 = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - local_20;
        if (local_20 < 0) {
          local_20 = 0;
        }
        g_AiWorkspace08Count = g_AiWorkspace08Count + 1;
        terrainFeatureEntry->priority = local_20;
      }
      return;
    }
    if (PVar8 == terrainFeatureEntry->armyAssetId) {
      iVar4 = terrainFeatureEntry->cell->worldX - terrainFeatureCell->worldX;
      if (iVar4 < 0) {
        iVar4 = -iVar4;
      }
      iVar7 = terrainFeatureEntry->cell->worldY - terrainFeatureCell->worldY;
      if (iVar7 < 0) {
        iVar7 = -iVar7;
      }
      if ((iVar4 < (int)uVar6) && (iVar7 < (int)uVar6)) {
        local_20 = 0x7fffffff;
        pAVar9 = g_AiWorkspaceBuffer00_Size0400;
        iVar4 = g_AiWorkspace00Count;
        do {
          if (iVar4 == 0) {
            local_20 = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - local_20;
            if (local_20 < 0) {
              local_20 = 0;
            }
            if (local_20 <= terrainFeatureEntry->priority) {
              return;
            }
            terrainFeatureEntry->cell = terrainFeatureCell;
            terrainFeatureEntry->priority = local_20;
            return;
          }
          if (pAVar9->runtimeSlotAddressOrZero != 0) {
            iVar7 = *(int *)(pAVar9->runtimeSlotAddressOrZero + 4);
            iVar5 = *(int *)(iVar7 + 0x94) - terrainFeatureCell->worldX;
            if (iVar5 < 0) {
              iVar5 = -iVar5;
            }
            iVar7 = *(int *)(iVar7 + 0x98) - terrainFeatureCell->worldY;
            if (iVar7 < 0) {
              iVar7 = -iVar7;
            }
            if (((PVar8 == pAVar9->armyAssetId) &&
                (iVar5 < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12
                )) && (iVar7 < (int)(g_AiKnowledgeData->parameters).
                                    terrainFeatureMinimumAxisSeparationQ12)) {
              return;
            }
            if (iVar5 + iVar7 < local_20) {
              local_20 = iVar5 + iVar7;
            }
          }
          pAVar9 = pAVar9 + 1;
          iVar4 = iVar4 + -1;
        } while( true );
      }
    }
    uVar1 = uVar1 - 1;
    terrainFeatureEntry = terrainFeatureEntry + 1;
  } while( true );
}


/* Address: 0x00539200.
   Ownership: gameplay/ai/placement.
   Purpose: Invokes the shared AI placement query in mode zero using the workspace record identifier and world
   coordinates together with the current asset, faction, and planning context. Stock ARM contains 675 records and
   326 unique ids; placement workspace, producer, tier, class, and faction-role semantics are not inferred from
   numeric adjacency. Typed parameters: p4 placementContext→ArmyPlacementContext_V344. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p3 workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_TestWorkspaceRecordAtPoint
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,UiRootNode *inGameRoot)

{
  ArmyPlacementDispatchEaxCf5 AVar1;
  
  AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (0,0,(uint)(ushort)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,placementContext,
                     inGameRoot);
  return AVar1.carry;
}


/* Address: 0x0053A1B0.
   Ownership: gameplay/ai/placement.
   Purpose: Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier, class, and
   faction-role semantics are not inferred from numeric adjacency. Typed parameters: p3
   workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_TestMode4AtWorkspaceRecord
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  ArmyPlacementDispatchEaxCf5 AVar1;
  
  AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (4,0,(uint)(ushort)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  return AVar1.carry;
}


/* Address: 0x0053B570.
   Ownership: gameplay/ai/placement.
   Purpose: Runs the shared placement query in the verified primary and fallback modes and converts a successful
   returned count into knowledge-sized placement buckets. Stock ARM contains 675 records and 326 unique ids;
   placement workspace, producer, tier, class, and faction-role semantics are not inferred from numeric adjacency.
   Typed parameters: p3 workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
AiPlacement_QueryReachableSiteBucketCount
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *pAVar1;
  dword dVar2;
  uint uVar3;
  ArmyPlacementDispatchEaxCf5 AVar4;
  StatusValueEaxCf5 SVar5;
  StatusValueEaxCf5 SVar6;
  
  pAVar1 = g_AiKnowledgeData;
  uVar3 = (uint)(ushort)workspaceRecord->triangle0NormalAngles;
  AVar4 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,uVar3,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  dVar2 = AVar4.eax;
  if (AVar4.carry) {
    AVar4 = ArmyPlacement_DispatchAssetAtFieldPoint
                      (0,0,uVar3,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                       factionIndex,(UiRootNode *)worldRuntime);
    if (AVar4.carry) {
      SVar6.valueOrError = AVar4.eax;
      SVar6.carry = AVar4.carry;
      return SVar6;
    }
  }
  else if ((dVar2 != 0) &&
          (AVar4 = ArmyPlacement_DispatchAssetAtFieldPoint
                             (0,0,uVar3,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                              factionIndex,(UiRootNode *)worldRuntime), AVar4.carry)) {
    uVar3 = (pAVar1->parameters).specialSiteSeparationQuantumQ12;
    uVar3 = ((dVar2 - 1) + uVar3) / uVar3;
    goto AiPlacement_QueryReachableSiteBucketCount_ReturnComputedOrZeroCountWithCarryClear;
  }
  uVar3 = 0;
AiPlacement_QueryReachableSiteBucketCount_ReturnComputedOrZeroCountWithCarryClear:
  SVar5.carry = false;
  SVar5.valueOrError = uVar3;
  return SVar5;
}


/* Address: 0x0053ACD0.
   Ownership: gameplay/ai/placement.
   Purpose: Runs the shared mode-three placement query for one workspace record and, for supported nonzero bucket
   counts, expands the request into a separated site chain using the calculated remainder. Stock ARM contains 675
   records and 326 unique ids; placement workspace, producer, tier, class, and faction-role semantics are not
   inferred from numeric adjacency. Typed parameters: p3 workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: AiPlacement_ReserveSeparatedSpecialSiteChain.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_ReserveMode3SiteCluster
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *pAVar1;
  uint uVar2;
  bool bVar3;
  ArmyPlacementDispatchEaxCf5 AVar4;
  
  pAVar1 = g_AiKnowledgeData;
  AVar4 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                     (uint)(ushort)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                     workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  if ((AVar4.carry) ||
     (uVar2 = (pAVar1->parameters).specialSiteSeparationQuantumQ12,
     uVar2 = ((AVar4.eax - 1) + uVar2) / uVar2, uVar2 == 0)) {
    return true;
  }
  if ((uVar2 < 5) &&
     (bVar3 = AiPlacement_ReserveSeparatedSpecialSiteChain
                        (armyAssetId,workspaceRecord,factionIndex,worldRuntime), !bVar3)) {
    return true;
  }
  return false;
}


/* Address: 0x0053AD50.
   Ownership: gameplay/ai/placement.
   Purpose: Scans workspace08 records, reserves eligible mode-three site clusters, enforces minimum distance from
   workspace02 and workspace03, and returns an assigned-count and faction-resource-scaled weight for the first
   acceptable special-site asset.
   Local calls: AiPlacement_ReserveMode3SiteCluster.
   Cross-module calls: AiWorkspace03_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiWorkspace02_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiPrimaryWorkspace_CountAssignedEntriesById [gameplay/ai/workspaces].
*/
AiCandidateScoreEaxCf5 __thandor_eax_cf_preserve_ecx_edx
AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *workspaceRecord;
  dword dVar1;
  dword in_EAX;
  int iVar2;
  uint uVar3;
  AiTerrainFeatureWorkspaceEntry *pAVar4;
  bool bVar5;
  AiCandidateScoreEaxCf5 AVar6;
  AiCandidateScoreEaxCf5 AVar7;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  iVar2 = g_AiWorkspace08Count;
  pAVar4 = g_AiWorkspaceBuffer08_Size0200;
  while( true ) {
    if (iVar2 == 0) {
      AVar7.carry = true;
      AVar7.score = in_EAX;
      return AVar7;
    }
    workspaceRecord = pAVar4->cell;
    bVar5 = AiPlacement_ReserveMode3SiteCluster
                      (pAVar4->armyAssetId,workspaceRecord,factionIndex,worldRuntime);
    if (((!bVar5) &&
        (in_EAX = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                            (workspaceRecord->worldY,workspaceRecord->worldX),
        (int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= (int)in_EAX)) &&
       (in_EAX = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                           (workspaceRecord->worldY,workspaceRecord->worldX),
       (int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= (int)in_EAX))
    break;
    pAVar4 = pAVar4 + 1;
    iVar2 = iVar2 + -1;
  }
  iVar2 = AiPrimaryWorkspace_CountAssignedEntriesById(pAVar4->armyAssetId);
  dVar1 = (knowledgeData->parameters).specialSite14aBaseWeight;
  if (pAVar4->armyAssetId != ARM_0330_BUILDING_MDL0303) {
    dVar1 = (knowledgeData->parameters).specialSite14cBaseWeight;
  }
  uVar3 = (dVar1 * 3) / (iVar2 * 2 + 6U);
  if (pAVar4->armyAssetId != ARM_0330_BUILDING_MDL0303) {
    iVar2 = g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 << 8;
    if (iVar2 != 0) {
      uVar3 = (uint)(((longlong)(int)uVar3 *
                     (longlong)
                     (int)(g_GameFactionRuntimeImage.records[factionIndex].unpoweredEnergyDemandQ4 *
                           2 + g_GameFactionRuntimeImage.records[factionIndex].
                               suppliedEnergyDemandQ4)) / (longlong)iVar2);
    }
  }
  AVar6.carry = false;
  AVar6.score = uVar3;
  return AVar6;
}


/* Address: 0x00539330.
   Ownership: gameplay/ai/placement.
   Purpose: It returns the selected anchor coordinates when a valid entry exists. Stock ARM contains 675 records
   and 326 unique ids; placement workspace, producer, tier, class, and faction-role semantics are not inferred from
   numeric adjacency. Typed parameters: p2 referenceWorldXQ12→Q12, p3 referenceWorldYQ12→Q12. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
AiWorkspace09AnchorEcxEdxCf9 __thandor_preserve_eax
AiPlacement_FindNearestValidWorkspace09Anchor
          (Q12 referenceWorldXQ12,Q12 referenceWorldYQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  uint in_EDX;
  int iVar5;
  uint uVar6;
  FieldGridCell **gridCellCursor;
  ArmyPlacementDispatchEaxCf5 AVar7;
  AiWorkspace09AnchorEcxEdxCf9 AVar8;
  AiWorkspace09AnchorEcxEdxCf9 AVar9;
  FieldGridCell *local_18;
  FieldGridCell *gridCell1;
  
  if (g_AiWorkspace09Count != 0) {
    uVar6 = 0x7fffffff;
    iVar4 = g_AiWorkspace09Count;
    gridCellCursor = g_AiWorkspaceBuffer09_Size1000;
    do {
      gridCell1 = *gridCellCursor;
      iVar3 = referenceWorldYQ12 - gridCell1->worldX;
      if (iVar3 < 0) {
        iVar3 = -iVar3;
      }
      iVar5 = referenceWorldXQ12 - gridCell1->worldY;
      if (iVar5 < 0) {
        iVar5 = -iVar5;
      }
      in_EDX = iVar5 + iVar3;
      if ((int)in_EDX < (int)uVar6) {
        AVar7 = ArmyPlacement_DispatchAssetAtFieldPoint
                          (1,0,(uint)(ushort)gridCell1->triangle0NormalAngles,gridCell1->worldY,
                           gridCell1->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
        if (!AVar7.carry) {
          uVar6 = in_EDX;
          local_18 = gridCell1;
        }
      }
      gridCellCursor = gridCellCursor + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    if ((int)uVar6 < 0x7fffffff) {
      uVar1 = local_18->worldX;
      uVar2 = local_18->worldY;
      AVar8.worldYQ12 = uVar2;
      AVar8.worldXQ12 = uVar1;
      AVar8.carry = false;
      return AVar8;
    }
  }
  AVar9.carry = true;
  AVar9.worldXQ12 = (int)((ulonglong)in_EDX << 0x20);
  AVar9.worldYQ12 = (int)(((ulonglong)in_EDX << 0x20) >> 0x20);
  return AVar9;
}


/* Address: 0x00539EF0.
   Ownership: gameplay/ai/placement.
   Purpose: Builds up to four mode-one placement records for a special workspace entity while enforcing the
   configured Manhattan separation threshold between accepted points. Accepted temporary records are linked into
   the caller workspace. Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier,
   class, and faction-role semantics are not inferred from numeric adjacency. Typed parameters: p3
   workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: AiPlacement_FindNearestValidWorkspace09Anchor.
   Cross-module calls: ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime],
   ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_ReserveSeparatedSpecialSiteChain
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  Q12 worldYQ12;
  int iVar1;
  Q12 worldXQ12;
  int iVar2;
  ArmyRuntimeCreateEaxCf5 AVar3;
  ArmyRuntimeCreateEaxCf5 AVar4;
  ArmyRuntimeCreateEaxCf5 AVar5;
  ArmyRuntimeCreateEaxCf5 AVar6;
  AiWorkspace09AnchorEcxEdxCf9 AVar7;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  AVar7 = AiPlacement_FindNearestValidWorkspace09Anchor
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  worldXQ12 = AVar7.worldYQ12;
  worldYQ12 = AVar7.worldXQ12;
  if (AVar7.carry) {
    return true;
  }
  AVar3 = ArmyRuntime_CreateInstanceFromAssetCf
                    (1,0,worldXQ12,worldYQ12,factionIndex,ARM_0333_BUILDING_MDL0307,worldRuntime);
  if (AVar3.carry) {
    return true;
  }
  iVar1 = worldYQ12 - workspaceRecord->worldX;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  iVar2 = worldXQ12 - workspaceRecord->worldY;
  if (iVar2 < 0) {
    iVar2 = -iVar2;
  }
  if ((uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12)
  goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarryClear;
  AVar7 = AiPlacement_FindNearestValidWorkspace09Anchor
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  if (AVar7.carry) goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet;
  AVar4 = ArmyRuntime_CreateInstanceFromAssetCf
                    (1,0,AVar7.worldYQ12,AVar7.worldXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                     worldRuntime);
  if (AVar4.carry) goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet;
  iVar1 = AVar7.worldXQ12 - workspaceRecord->worldX;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  iVar2 = AVar7.worldYQ12 - workspaceRecord->worldY;
  if (iVar2 < 0) {
    iVar2 = -iVar2;
  }
  if ((knowledgeData->parameters).specialSiteSeparationQuantumQ12 <= (uint)(iVar1 + iVar2)) {
    AVar7 = AiPlacement_FindNearestValidWorkspace09Anchor
                      (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                       factionIndex,worldRuntime);
    if (!AVar7.carry) {
      AVar5 = ArmyRuntime_CreateInstanceFromAssetCf
                        (1,0,AVar7.worldYQ12,AVar7.worldXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                         worldRuntime);
      if (!AVar5.carry) {
        iVar1 = AVar7.worldXQ12 - workspaceRecord->worldX;
        if (iVar1 < 0) {
          iVar1 = -iVar1;
        }
        iVar2 = AVar7.worldYQ12 - workspaceRecord->worldY;
        if (iVar2 < 0) {
          iVar2 = -iVar2;
        }
        if ((uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12) {
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyThirdSecondFirstAndReturnCarryClear:
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar5.eax);
          goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroySecondFirstAndReturnCarryClear;
        }
        AVar7 = AiPlacement_FindNearestValidWorkspace09Anchor
                          (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307
                           ,factionIndex,worldRuntime);
        if (!AVar7.carry) {
          AVar6 = ArmyRuntime_CreateInstanceFromAssetCf
                            (1,0,AVar7.worldYQ12,AVar7.worldXQ12,factionIndex,
                             ARM_0333_BUILDING_MDL0307,worldRuntime);
          if (!AVar6.carry) {
            iVar1 = AVar7.worldXQ12 - workspaceRecord->worldX;
            if (iVar1 < 0) {
              iVar1 = -iVar1;
            }
            iVar2 = AVar7.worldYQ12 - workspaceRecord->worldY;
            if (iVar2 < 0) {
              iVar2 = -iVar2;
            }
            if ((uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12)
            {
              ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar6.eax);
              goto 
              AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyThirdSecondFirstAndReturnCarryClear
              ;
            }
            ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar6.eax);
          }
        }
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar5.eax);
      }
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar4.eax);
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet:
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar3.eax);
    return true;
  }
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroySecondFirstAndReturnCarryClear:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar4.eax);
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarryClear:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)AVar3.eax);
  return false;
}

