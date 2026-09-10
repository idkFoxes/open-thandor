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
void AiPlacement_ReserveAdditionalSpecialSite
               (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
               FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  uint uVar2;
  AiKnowledgeDataImage *pAVar3;
  undefined4 extraout_ECX;
  undefined1 in_CF;
  bool bVar4;
  undefined8 uVar5;
  int iVar6;
  
  pAVar3 = g_AiKnowledgeData;
  uVar5 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (7,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                     (uint)(ushort)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                     workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  iVar6 = (int)uVar5;
  if ((!(bool)in_CF) && (bVar4 = false, iVar6 != 0)) {
    ArmyPlacement_DispatchAssetAtFieldPoint
              (4,0,(FactionRuntimeIndex)((ulonglong)uVar5 >> 0x20),workspaceRecord->worldY,
               workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
    if (bVar4) {
      uVar2 = (pAVar3->parameters).specialSiteSeparationQuantumQ12;
      uVar1 = iVar6 + -1 + uVar2;
      if (uVar1 / uVar2 < 5) {
        AiPlacement_ReserveSeparatedSpecialSiteChain
                  (extraout_ECX,uVar1 % uVar2,armyAssetId,workspaceRecord,factionIndex,worldRuntime)
        ;
        return;
      }
    }
  }
  return;
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
AiPreservedFactionIndexEdxResult
AiCandidatePlanning_AddSpecialSiteCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  FactionRuntimeIndex extraout_EDX;
  FactionRuntimeIndex extraout_EDX_00;
  AiPreservedFactionIndexEdxResult AVar1;
  FactionRuntimeIndex extraout_EDX_01;
  FactionRuntimeIndex extraout_EDX_02;
  FactionRuntimeIndex extraout_EDX_03;
  int iVar2;
  bool bVar3;
  AiCandidateScoreCfEaxPreservedEdxCarrier64 AVar4;
  
  iVar2 = (int)((longlong)factionIndex * 0x740);
  bVar3 = (longlong)iVar2 != (longlong)factionIndex * 0x740;
  AiSecondaryWorkspace_HasEntryByIdCf(ARM_0050_UNIT_MDL0103);
  AVar1.preservedEdxFactionIndex = extraout_EDX;
  if (!bVar3) {
    AiPrimaryWorkspace_HasEntryByIdCf(ARM_0301_BUILDING_MDL0318);
    if (bVar3) {
      ArmyAssetRegistry_FindEnabledByIdCf(ARM_0050_UNIT_MDL0103);
      AVar1.preservedEdxFactionIndex = extraout_EDX_00;
      if (!bVar3) {
        bVar3 = false;
        if ((*(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits + iVar2) &
            0x800) == 0) {
          AVar4 = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          AVar1.preservedEdxFactionIndex = (FactionRuntimeIndex)(AVar4 >> 0x20);
          if (!bVar3) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0xb,(dword)AVar4,2);
            AVar1.preservedEdxFactionIndex = extraout_EDX_03;
          }
        }
        else {
          AVar4 = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          AVar1.preservedEdxFactionIndex = (FactionRuntimeIndex)(AVar4 >> 0x20);
          if (!bVar3) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x32,(dword)AVar4,0);
            AVar1.preservedEdxFactionIndex = extraout_EDX_01;
          }
        }
      }
    }
    else {
      AVar4 = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
      AVar1.preservedEdxFactionIndex = (FactionRuntimeIndex)(AVar4 >> 0x20);
      if (!bVar3) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x12d,(dword)AVar4,0);
        AVar1.preservedEdxFactionIndex = extraout_EDX_02;
      }
    }
  }
  return (AiPreservedFactionIndexEdxResult)AVar1.preservedEdxFactionIndex;
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
AiPlanningGridScratchEdxContinuityResult AiSiteCandidate_AddGeneralCellIfSeparated(void)

{
  Q12 worldY;
  Q12 worldX;
  dword dVar1;
  AiKnowledgeDataImage *pAVar2;
  int iVar3;
  dword dVar4;
  uint uVar5;
  int extraout_ECX;
  dword in_EDX;
  int iVar6;
  Q12 worldX_00;
  Q12 worldX_01;
  FieldGridCell *unaff_ESI;
  AiScoredSiteWorkspaceEntry *pAVar7;
  
  uVar5 = g_AiWorkspace05Count;
  pAVar7 = g_AiWorkspaceBuffer05_Size0200;
  while( true ) {
    if (uVar5 == 0) {
      worldY = unaff_ESI->worldX;
      worldX = unaff_ESI->worldY;
      if (g_AiWorkspace05Count < 0x20) {
        pAVar7->cellWorldXQ12 = worldY;
        pAVar7->cellWorldYQ12 = worldX;
        pAVar7->cell = unaff_ESI;
        pAVar2 = g_AiKnowledgeData;
        AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        AiWorkspace02_GetMinimumManhattanDistanceToPoint(worldX_00,worldY);
        dVar1 = (pAVar2->parameters).generalSiteSecondaryDistanceCapQ12;
        dVar4 = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX_01,worldY);
        if ((int)dVar1 < (int)dVar4) {
          dVar4 = dVar1;
        }
        g_AiWorkspace05Count = g_AiWorkspace05Count + 1;
        pAVar7->score =
             extraout_ECX + dVar4 * (pAVar2->parameters).generalSiteSecondaryDistanceCoefficient;
      }
      return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
    }
    iVar3 = pAVar7->cellWorldXQ12 - unaff_ESI->worldX;
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    iVar6 = pAVar7->cellWorldYQ12 - unaff_ESI->worldY;
    if (iVar6 < 0) {
      iVar6 = -iVar6;
    }
    if ((iVar3 < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12) &&
       (iVar6 < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12)) break;
    pAVar7 = pAVar7 + 1;
    uVar5 = uVar5 - 1;
  }
  return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
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
AiPlanningGridScratchEdxContinuityResult AiSiteCandidate_AddFlaggedCellIfSeparated(void)

{
  Q12 worldY;
  Q12 worldX;
  int iVar1;
  int iVar2;
  uint uVar3;
  int extraout_ECX;
  dword in_EDX;
  int iVar4;
  Q12 worldX_00;
  Q12 worldX_01;
  int unaff_ESI;
  byte *pbVar5;
  
  uVar3 = g_AiWorkspace06Count;
  pbVar5 = g_AiWorkspaceBuffer06_Size0400;
  while( true ) {
    if (uVar3 == 0) {
      worldY = *(Q12 *)(unaff_ESI + 0x40);
      worldX = *(Q12 *)(unaff_ESI + 0x44);
      if (g_AiWorkspace06Count < 0x40) {
        *(Q12 *)pbVar5 = worldY;
        *(Q12 *)(pbVar5 + 4) = worldX;
        *(int *)(pbVar5 + 0xc) = unaff_ESI;
        AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        AiWorkspace02_GetMinimumManhattanDistanceToPoint(worldX_00,worldY);
        iVar1 = *(int *)(pbVar5 + 200);
        iVar2 = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX_01,worldY);
        iVar4 = extraout_ECX;
        if (-1 < iVar1 - iVar2) {
          iVar4 = extraout_ECX + (iVar1 - iVar2) * *(int *)(pbVar5 + 0xd8);
        }
        g_AiWorkspace06Count = g_AiWorkspace06Count + 1;
        *(int *)(pbVar5 + 8) = iVar4;
      }
      return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
    }
    iVar1 = *(int *)pbVar5 - *(int *)(unaff_ESI + 0x40);
    if (iVar1 < 0) {
      iVar1 = -iVar1;
    }
    iVar4 = *(int *)(pbVar5 + 4) - *(int *)(unaff_ESI + 0x44);
    if (iVar4 < 0) {
      iVar4 = -iVar4;
    }
    if ((iVar1 < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12) &&
       (iVar4 < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12)) break;
    pbVar5 = pbVar5 + 0x10;
    uVar3 = uVar3 - 1;
  }
  return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
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
AiPlanningGridScratchEdxContinuityResult AiSiteCandidate_AddTerrainFeatureCellIfSeparated(void)

{
  uint uVar1;
  int *piVar2;
  ModelRuntimeNode *modelNodeRuntime;
  ModelPackedPointRecord *localPointRecord;
  dword dVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  dword in_EDX;
  int iVar7;
  PckArmyAssetIdCatalog PVar8;
  FieldGridCell *unaff_ESI;
  byte *pbVar9;
  bool bVar10;
  undefined8 uVar11;
  int local_20;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  
  uVar1 = g_AiWorkspace08Count;
  PVar8 = ARM_0330_BUILDING_MDL0303;
  uVar6 = (g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12 * 0x55 >> 8;
  iVar4 = g_AiWorkspace00Count;
  pbVar9 = g_AiWorkspaceBuffer00_Size0400;
  if ((unaff_ESI->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
    PVar8 = ARM_0332_BUILDING_MDL0302;
  }
  for (; terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200, iVar4 != 0; iVar4 = iVar4 + -1) {
    piVar2 = *(int **)pbVar9;
    if (piVar2 != (int *)0x0) {
      modelNodeRuntime = (ModelRuntimeNode *)piVar2[1];
      bVar10 = *(uint *)(*piVar2 + 0x4c) < 0xd;
      if ((*(uint *)(*piVar2 + 0x4c) == 0xd) &&
         (localPointRecord =
               (ModelPackedPointRecord *)
               ModelLookupTable_ContainsPackedKeyCf
                         (1,5,(modelNodeRuntime->modelPayload).modelResource), !bVar10)) {
        uVar11 = ModelNodeRuntime_TransformLocalPointRegs
                           (extraout_ECX,localPointRecord,modelNodeRuntime);
        dVar3 = FixedMath_Length2(extraout_ECX_00 - unaff_ESI->worldY,
                                  (int)uVar11 - unaff_ESI->worldX);
        if ((int)dVar3 < 0x2001) {
          return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
        }
      }
    }
    pbVar9 = pbVar9 + 8;
  }
  do {
    if (uVar1 == 0) {
      if (g_AiWorkspace08Count < 0x20) {
        terrainFeatureEntry->cell = unaff_ESI;
        terrainFeatureEntry->armyAssetId = PVar8;
        local_20 = 0x7fffffff;
        pbVar9 = g_AiWorkspaceBuffer00_Size0400;
        for (iVar4 = g_AiWorkspace00Count; iVar4 != 0; iVar4 = iVar4 + -1) {
          if (*(int *)pbVar9 != 0) {
            iVar7 = *(int *)(*(int *)pbVar9 + 4);
            iVar5 = *(int *)(iVar7 + 0x94) - unaff_ESI->worldX;
            if (iVar5 < 0) {
              iVar5 = -iVar5;
            }
            iVar7 = *(int *)(iVar7 + 0x98) - unaff_ESI->worldY;
            if (iVar7 < 0) {
              iVar7 = -iVar7;
            }
            if (((PVar8 == *(PckArmyAssetIdCatalog *)(pbVar9 + 4)) &&
                (iVar5 < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12
                )) && (iVar7 < (int)(g_AiKnowledgeData->parameters).
                                    terrainFeatureMinimumAxisSeparationQ12)) {
              return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
            }
            if (iVar5 + iVar7 < local_20) {
              local_20 = iVar5 + iVar7;
            }
          }
          pbVar9 = pbVar9 + 8;
        }
        local_20 = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - local_20;
        if (local_20 < 0) {
          local_20 = 0;
        }
        g_AiWorkspace08Count = g_AiWorkspace08Count + 1;
        terrainFeatureEntry->priority = local_20;
      }
      return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
    }
    if (PVar8 == terrainFeatureEntry->armyAssetId) {
      iVar4 = terrainFeatureEntry->cell->worldX - unaff_ESI->worldX;
      if (iVar4 < 0) {
        iVar4 = -iVar4;
      }
      iVar7 = terrainFeatureEntry->cell->worldY - unaff_ESI->worldY;
      if (iVar7 < 0) {
        iVar7 = -iVar7;
      }
      if ((iVar4 < (int)uVar6) && (iVar7 < (int)uVar6)) {
        local_20 = 0x7fffffff;
        pbVar9 = g_AiWorkspaceBuffer00_Size0400;
        iVar4 = g_AiWorkspace00Count;
        do {
          if (iVar4 == 0) {
            local_20 = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - local_20;
            if (local_20 < 0) {
              local_20 = 0;
            }
            if (local_20 <= terrainFeatureEntry->priority) {
              return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
            }
            terrainFeatureEntry->cell = unaff_ESI;
            terrainFeatureEntry->priority = local_20;
            return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
          }
          if (*(int *)pbVar9 != 0) {
            iVar7 = *(int *)(*(int *)pbVar9 + 4);
            iVar5 = *(int *)(iVar7 + 0x94) - unaff_ESI->worldX;
            if (iVar5 < 0) {
              iVar5 = -iVar5;
            }
            iVar7 = *(int *)(iVar7 + 0x98) - unaff_ESI->worldY;
            if (iVar7 < 0) {
              iVar7 = -iVar7;
            }
            if (((PVar8 == *(PckArmyAssetIdCatalog *)(pbVar9 + 4)) &&
                (iVar5 < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12
                )) && (iVar7 < (int)(g_AiKnowledgeData->parameters).
                                    terrainFeatureMinimumAxisSeparationQ12)) {
              return (AiPlanningGridScratchEdxContinuityResult)in_EDX;
            }
            if (iVar5 + iVar7 < local_20) {
              local_20 = iVar5 + iVar7;
            }
          }
          pbVar9 = pbVar9 + 8;
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
undefined4
AiPlacement_TestWorkspaceRecordAtPoint
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,UiRootNode *inGameRoot)

{
  undefined4 in_EAX;
  
  ArmyPlacement_DispatchAssetAtFieldPoint
            (0,0,(uint)(ushort)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
             workspaceRecord->worldX,armyAssetId,placementContext,inGameRoot);
  return in_EAX;
}

/* Address: 0x0053A1B0.
   Ownership: gameplay/ai/placement.
   Purpose: Stock ARM contains 675 records and 326 unique ids; placement workspace, producer, tier, class, and
   faction-role semantics are not inferred from numeric adjacency. Typed parameters: p3
   workspaceRecord→AiPlacementWorkspaceRecordAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
undefined4
AiPlacement_TestMode4AtWorkspaceRecord
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  undefined4 in_EAX;
  
  ArmyPlacement_DispatchAssetAtFieldPoint
            (4,0,(uint)(ushort)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
             workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  return in_EAX;
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
dword AiPlacement_QueryReachableSiteBucketCount
                (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
                FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  AiKnowledgeDataImage *pAVar2;
  dword dVar3;
  FactionRuntimeIndex ownerFactionIndex;
  undefined1 in_CF;
  bool bVar4;
  undefined8 uVar5;
  int iVar6;
  
  pAVar2 = g_AiKnowledgeData;
  uVar5 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                     (uint)(ushort)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                     workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  ownerFactionIndex = (FactionRuntimeIndex)((ulonglong)uVar5 >> 0x20);
  iVar6 = (int)uVar5;
  if ((bool)in_CF) {
    dVar3 = ArmyPlacement_DispatchAssetAtFieldPoint
                      (0,0,ownerFactionIndex,workspaceRecord->worldY,workspaceRecord->worldX,
                       armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
    if ((bool)in_CF) {
      return dVar3;
    }
  }
  else {
    bVar4 = false;
    if ((iVar6 != 0) &&
       (ArmyPlacement_DispatchAssetAtFieldPoint
                  (0,0,ownerFactionIndex,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId
                   ,factionIndex,(UiRootNode *)worldRuntime), bVar4)) {
      uVar1 = (pAVar2->parameters).specialSiteSeparationQuantumQ12;
      return (iVar6 + -1 + uVar1) / uVar1;
    }
  }
  return 0;
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
void AiPlacement_ReserveMode3SiteCluster
               (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
               FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint uVar1;
  uint uVar2;
  AiKnowledgeDataImage *pAVar3;
  int iVar4;
  uint uVar5;
  undefined4 extraout_ECX;
  undefined1 in_CF;
  bool bVar6;
  
  pAVar3 = g_AiKnowledgeData;
  iVar4 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                     (uint)(ushort)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                     workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  if (!(bool)in_CF) {
    uVar2 = (pAVar3->parameters).specialSiteSeparationQuantumQ12;
    uVar1 = iVar4 + -1 + uVar2;
    uVar5 = uVar1 / uVar2;
    if (uVar5 != 0) {
      bVar6 = uVar5 < 4;
      if ((uVar5 < 5) &&
         (AiPlacement_ReserveSeparatedSpecialSiteChain
                    (extraout_ECX,uVar1 % uVar2,armyAssetId,workspaceRecord,factionIndex,
                     worldRuntime), !bVar6)) {
        return;
      }
      return;
    }
  }
  return;
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
AiCandidateScoreCfEaxPreservedEdxCarrier64
AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *workspaceRecord;
  dword dVar1;
  int in_EAX;
  int extraout_EAX;
  uint uVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar3;
  undefined4 in_EDX;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar4;
  AiWorkspaceCountEaxPreservedEdxCarrier64 AVar5;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  bVar4 = false;
  terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
  if (g_AiWorkspace08Count != 0) {
    do {
      workspaceRecord = terrainFeatureEntry->cell;
      AiPlacement_ReserveMode3SiteCluster
                (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,worldRuntime);
      in_EAX = extraout_EAX;
      iVar3 = extraout_ECX;
      if (!bVar4) {
        in_EAX = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                           (workspaceRecord->worldY,workspaceRecord->worldX);
        iVar3 = extraout_ECX_00;
        if ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= in_EAX) {
          in_EAX = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                             (workspaceRecord->worldY,workspaceRecord->worldX);
          iVar3 = extraout_ECX_01;
          if ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= in_EAX) {
            AVar5 = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
            dVar1 = (knowledgeData->parameters).specialSite14aBaseWeight;
            if (terrainFeatureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
              dVar1 = (knowledgeData->parameters).specialSite14cBaseWeight;
            }
            uVar2 = (dVar1 * 3) / ((int)AVar5 * 2 + 6U);
            if (terrainFeatureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
              iVar3 = g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 << 8;
              if (iVar3 != 0) {
                uVar2 = (uint)(((longlong)(int)uVar2 *
                               (longlong)
                               (int)(g_GameFactionRuntimeImage.records[factionIndex].
                                     unpoweredEnergyDemandQ4 * 2 +
                                    g_GameFactionRuntimeImage.records[factionIndex].
                                    suppliedEnergyDemandQ4)) / (longlong)iVar3);
              }
            }
            return CONCAT44(in_EDX,uVar2);
          }
        }
      }
      bVar4 = (AiTerrainFeatureWorkspaceEntry *)0xffffffef < terrainFeatureEntry;
      terrainFeatureEntry = terrainFeatureEntry + 1;
    } while (iVar3 != 1);
  }
  return CONCAT44(in_EDX,in_EAX);
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
AiWorkspace09AnchorCfRegisterResult
AiPlacement_FindNearestValidWorkspace09Anchor
          (Q12 referenceWorldXQ12,Q12 referenceWorldYQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  uint in_EDX;
  int iVar3;
  uint extraout_EDX;
  uint uVar4;
  FieldGridCell **gridCellCursor;
  bool bVar5;
  AiWorkspace09AnchorCfRegisterResult AVar6;
  FieldGridCell *local_18;
  FieldGridCell *gridCell1;
  
  if (g_AiWorkspace09Count != 0) {
    uVar4 = 0x7fffffff;
    iVar2 = g_AiWorkspace09Count;
    gridCellCursor = g_AiWorkspaceBuffer09_Size1000;
    do {
      gridCell1 = *gridCellCursor;
      iVar1 = referenceWorldYQ12 - gridCell1->worldX;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      iVar3 = referenceWorldXQ12 - gridCell1->worldY;
      if (iVar3 < 0) {
        iVar3 = -iVar3;
      }
      in_EDX = iVar3 + iVar1;
      bVar5 = in_EDX < uVar4;
      if ((int)in_EDX < (int)uVar4) {
        ArmyPlacement_DispatchAssetAtFieldPoint
                  (1,0,(uint)(ushort)gridCell1->triangle0NormalAngles,gridCell1->worldY,
                   gridCell1->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
        iVar2 = extraout_ECX;
        in_EDX = extraout_EDX;
        if (!bVar5) {
          uVar4 = extraout_EDX;
          local_18 = gridCell1;
        }
      }
      gridCellCursor = gridCellCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if ((int)uVar4 < 0x7fffffff) {
      AVar6.worldYQ12 = local_18->worldX;
      AVar6.worldXQ12 = local_18->worldY;
      return AVar6;
    }
  }
  return (AiWorkspace09AnchorCfRegisterResult)((ulonglong)in_EDX << 0x20);
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
void __fastcall
AiPlacement_ReserveSeparatedSpecialSiteChain
          (undefined4 param_1,uint param_2,undefined4 param_3,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  GameEntityRuntime *entityRuntime;
  GameEntityRuntime *entityRuntime_00;
  GameEntityRuntime *entityRuntime_01;
  GameEntityRuntime *entityRuntime_02;
  Q12 worldYQ12;
  int extraout_ECX;
  int iVar1;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  Q12 worldXQ12;
  int extraout_EDX;
  int iVar2;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  bool bVar3;
  AiWorkspace09AnchorCfRegisterResult AVar4;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  bVar3 = &stack0xffffffe4 < (undefined1 *)0x8;
  AVar4 = AiPlacement_FindNearestValidWorkspace09Anchor
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  worldXQ12 = AVar4.worldXQ12;
  worldYQ12 = AVar4.worldYQ12;
  if (bVar3) {
    return;
  }
  entityRuntime =
       (GameEntityRuntime *)
       ArmyRuntime_CreateInstanceFromAssetCf
                 (1,0,worldXQ12,worldYQ12,factionIndex,ARM_0333_BUILDING_MDL0307,worldRuntime);
  if (bVar3) {
    return;
  }
  iVar1 = extraout_ECX - workspaceRecord->worldX;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  iVar2 = extraout_EDX - workspaceRecord->worldY;
  if (iVar2 < 0) {
    iVar2 = -iVar2;
  }
  bVar3 = (uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
  if (bVar3) goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarryClear;
  AVar4 = AiPlacement_FindNearestValidWorkspace09Anchor
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  if ((bVar3) ||
     (entityRuntime_00 =
           (GameEntityRuntime *)
           ArmyRuntime_CreateInstanceFromAssetCf
                     (1,0,AVar4.worldXQ12,AVar4.worldYQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                      worldRuntime), bVar3))
  goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet;
  iVar1 = extraout_ECX_00 - workspaceRecord->worldX;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  iVar2 = extraout_EDX_00 - workspaceRecord->worldY;
  if (iVar2 < 0) {
    iVar2 = -iVar2;
  }
  bVar3 = (uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
  if (!bVar3) {
    AVar4 = AiPlacement_FindNearestValidWorkspace09Anchor
                      (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                       factionIndex,worldRuntime);
    if ((!bVar3) &&
       (entityRuntime_01 =
             (GameEntityRuntime *)
             ArmyRuntime_CreateInstanceFromAssetCf
                       (1,0,AVar4.worldXQ12,AVar4.worldYQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                        worldRuntime), !bVar3)) {
      iVar1 = extraout_ECX_01 - workspaceRecord->worldX;
      if (iVar1 < 0) {
        iVar1 = -iVar1;
      }
      iVar2 = extraout_EDX_01 - workspaceRecord->worldY;
      if (iVar2 < 0) {
        iVar2 = -iVar2;
      }
      bVar3 = (uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
      if (bVar3) {
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyThirdSecondFirstAndReturnCarryClear:
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime_01);
        goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroySecondFirstAndReturnCarryClear;
      }
      AVar4 = AiPlacement_FindNearestValidWorkspace09Anchor
                        (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                         factionIndex,worldRuntime);
      if ((!bVar3) &&
         (entityRuntime_02 =
               (GameEntityRuntime *)
               ArmyRuntime_CreateInstanceFromAssetCf
                         (1,0,AVar4.worldXQ12,AVar4.worldYQ12,factionIndex,ARM_0333_BUILDING_MDL0307
                          ,worldRuntime), !bVar3)) {
        iVar1 = extraout_ECX_02 - workspaceRecord->worldX;
        if (iVar1 < 0) {
          iVar1 = -iVar1;
        }
        iVar2 = extraout_EDX_02 - workspaceRecord->worldY;
        if (iVar2 < 0) {
          iVar2 = -iVar2;
        }
        if ((uint)(iVar1 + iVar2) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12) {
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime_02);
          goto 
          AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyThirdSecondFirstAndReturnCarryClear;
        }
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime_02);
      }
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime_01);
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime_00);
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet:
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
    return;
  }
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroySecondFirstAndReturnCarryClear:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime_00);
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarryClear:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
  return;
}
