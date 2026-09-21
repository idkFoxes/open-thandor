/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/units.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/units.h>

/* Implementation ownership: gameplay/ai/units. */

/* Address: 0x0053B0E0.
   Ownership: gameplay/ai/units.
   Purpose: Scans active workspace01 entity records, updates their countdown state, and dispatches supported
   behavior classes either to special class-0x12 planning or to the shared anchor-action selector. MILITARY
   DISPATCH: reads class from instance+0x4C with a SINGLE deref (definitionValue9C_4C, ~0 on disk); class 0x12 ->
   UpdateSpecialClass12Entity, 1/2/3/0x11/0x13 -> SelectBestAnchorAction.
   Local calls: AiUnitBehavior_UpdateSpecialClass12Entity, AiUnitBehavior_SelectBestAnchorAction.
*/
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_UpdateWorkspace01Entities
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  byte *pbVar1;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  MdlDefinitionSemanticPrefix80 *modelDefinition;
  int *behaviorCooldownCounter;
  int *entityRuntime;
  ArmyRuntimeSlot *armySlot1;
  GameEntityRuntime *entityRuntime1;
  
  g_AiCollectedEntityCount = 0;
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  do {
    if (workspaceEntriesRemaining == 0) {
      return;
    }
    armySlot1 = workspaceEntryCursor->armyRuntime;
    if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
      entityRuntime1 = armySlot1->linkedEntityRuntime;
      if (((entityRuntime1->common).commandFlags & 0x13) != 0) {
        if (((entityRuntime1->common).commandFlags & 2) != 0)
        goto AiUnitBehavior_UpdateWorkspace01Entities_AdvanceAfterBehaviorDispatchOrSkip;
        behaviorCooldownCounter = (int *)((entityRuntime1->common).reserved80_9F + 0xc);
        *behaviorCooldownCounter = *behaviorCooldownCounter + -1;
        if (*behaviorCooldownCounter != 0) {
          if (-1 < *behaviorCooldownCounter)
          goto AiUnitBehavior_UpdateWorkspace01Entities_AdvanceAfterBehaviorDispatchOrSkip;
          pbVar1 = (entityRuntime1->common).reserved80_9F + 0xc;
          *(int *)pbVar1 = *(int *)pbVar1 + 1;
        }
      }
      modelDefinition =
           (MdlDefinitionSemanticPrefix80 *)(armySlot1->modelRuntimeOrSavedOffset).modelRuntime;
      if (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_18) {
        AiUnitBehavior_UpdateSpecialClass12Entity
                  (modelDefinition,(ArmyRuntimeSlot *)armySlot1->linkedEntityRuntime,factionIndex,
                   worldRuntime);
      }
      else if ((((modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_01_GROUND) ||
                (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_02_TRACKED)) ||
               (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_03_ARTICULATED_WALKER)) ||
              ((modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_19_WATER_SURFACE ||
               (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER)))) {
        AiUnitBehavior_SelectBestAnchorAction
                  (modelDefinition,(ArmyRuntimeSlot *)armySlot1->linkedEntityRuntime,factionIndex,
                   worldRuntime);
      }
    }
AiUnitBehavior_UpdateWorkspace01Entities_AdvanceAfterBehaviorDispatchOrSkip:
    workspaceEntryCursor = workspaceEntryCursor + 1;
    workspaceEntriesRemaining = workspaceEntriesRemaining + -1;
  } while( true );
}


/* Address: 0x0053B4C0.
   Ownership: gameplay/ai/units.
   Purpose: Compares three anchor-distance score families, selects the strongest action source, and either assigns
   a workspace point, assigns a faction anchor, or queues the entity for later group assignment. Best-anchor action
   select for military classes 1/2/3/0x11/0x13 (three distance scores above compete). It is distinct from
   FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId,
   ModelDefinitionId, and TechnologyId domains. Typed parameters: p2 currentBestScore→AiCandidateScore32_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: AiUnitBehavior_ComputeWorkspace05DistanceScore, AiUnitBehavior_ComputeFactionAnchorDistanceScore,
   AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore, AiUnitCommand_AssignWorkspacePoint,
   AiUnitCommand_AssignFactionAnchorPoint, AiUnitBehavior_CollectUnassignedEntity.
*/
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_SelectBestAnchorAction
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  AiCandidateScore32 AVar2;
  uint selectedAnchorActionKind;
  int bestAnchorActionScore;
  int currentBestScore;
  AiScoredSiteWorkspaceEntry *selectedWorkspaceEntry;
  AiWorkspace05DistanceSelectionRegs8 AVar3;
  AiSecondaryWorkspaceDistanceSelectionRegs8 AVar4;
  
  AVar3 = AiUnitBehavior_ComputeWorkspace05DistanceScore(0,modelDefinition,armyRuntimeSlot);
  iVar1 = AVar3.score;
  currentBestScore = 0;
  if (iVar1 != 0) {
    currentBestScore = iVar1;
    selectedWorkspaceEntry = AVar3.selectedEntry;
  }
  selectedAnchorActionKind = (uint)(iVar1 != 0);
  AVar2 = AiUnitBehavior_ComputeFactionAnchorDistanceScore
                    (factionIndex,currentBestScore,modelDefinition,armyRuntimeSlot);
  if (AVar2 != currentBestScore) {
    selectedAnchorActionKind = 2;
    currentBestScore = AVar2;
  }
  AVar4 = AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
                    (currentBestScore,modelDefinition,armyRuntimeSlot);
  if (AVar4.score != currentBestScore) {
    selectedAnchorActionKind = 3;
    selectedWorkspaceEntry = (AiScoredSiteWorkspaceEntry *)AVar4.selectedEntry;
  }
  if (selectedAnchorActionKind != 0) {
    if (selectedAnchorActionKind == 1) {
      AiUnitCommand_AssignWorkspacePoint
                ((dword *)selectedWorkspaceEntry,armyRuntimeSlot,worldRuntime);
      return;
    }
    if (selectedAnchorActionKind < 3) {
      AiUnitCommand_AssignFactionAnchorPoint(factionIndex,armyRuntimeSlot,worldRuntime);
      return;
    }
  }
  AiUnitBehavior_CollectUnassignedEntity(armyRuntimeSlot,worldRuntime);
  return;
}


/* Address: 0x0053B1D0.
   Ownership: gameplay/ai/units.
   Purpose: Distance score vs the Score-A site list (workspace 05). Typed parameters: p2
   currentBestScore→AiCandidateScore32_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p4
   armyRuntimeSlot→ArmyRuntimeSlot *. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
AiWorkspace05DistanceSelectionRegs8 __thandor_eax_ebx_cf_preserve_ecx_edx
AiUnitBehavior_ComputeWorkspace05DistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int iVar1;
  int recordsRemaining;
  Q12 deltaYAbsQ12;
  AiScoredSiteWorkspaceEntry *pAVar2;
  AiScoredSiteWorkspaceEntry *workspaceRecordCursor;
  AiWorkspace05DistanceSelectionRegs8 AVar3;
  
  pAVar2 = (AiScoredSiteWorkspaceEntry *)0x0;
  workspaceRecordCursor = g_AiWorkspaceBuffer05_Size0200;
  for (recordsRemaining = g_AiWorkspace05Count; recordsRemaining != 0;
      recordsRemaining = recordsRemaining + -1) {
    iVar1 = (armyRuntimeSlot->articulatedContact).fallbackPosition0Q12 -
            workspaceRecordCursor->cellWorldXQ12;
    if (-1 < iVar1) {
      iVar1 = -iVar1;
    }
    deltaYAbsQ12 = (armyRuntimeSlot->articulatedContact).fallbackPosition1Q12 -
                   workspaceRecordCursor->cellWorldYQ12;
    if (-1 < deltaYAbsQ12) {
      deltaYAbsQ12 = -deltaYAbsQ12;
    }
    iVar1 = iVar1 + deltaYAbsQ12 + (g_AiKnowledgeData->parameters).workspace05DistanceBiasQ12;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    iVar1 = (iVar1 * (g_AiKnowledgeData->parameters).workspace05DistanceScaleQ12 +
             workspaceRecordCursor->score >> 0xc) * armyRuntimeSlot->definitionClassValue80;
    if (iVar1 - currentBestScore != 0 && currentBestScore <= iVar1) {
      pAVar2 = workspaceRecordCursor;
      currentBestScore = iVar1;
    }
    workspaceRecordCursor = workspaceRecordCursor + 1;
  }
  AVar3.selectedEntry = pAVar2;
  AVar3.score = currentBestScore;
  return AVar3;
}


/* Address: 0x0053B260.
   Ownership: gameplay/ai/units.
   Purpose: Computes the strongest knowledge-scaled distance score from the entity runtime anchor to either
   configured faction anchor pair and returns the maximum against the incoming score. Distance score vs the faction
   anchor point. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and
   PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains. Typed parameters: p3
   currentBestScore→AiCandidateScore32_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot)

{
  int iVar1;
  Q12 primaryAnchorDeltaXAbsQ12;
  int iVar2;
  ModelRuntimeNode *modelNode;
  
  modelNode = armyRuntimeSlot->modelNodeRuntime;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
    iVar1 = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12;
    if (-1 < iVar1) {
      iVar1 = -iVar1;
    }
    primaryAnchorDeltaXAbsQ12 =
         (modelNode->worldTransform).translation.x -
         g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12;
    if (-1 < primaryAnchorDeltaXAbsQ12) {
      primaryAnchorDeltaXAbsQ12 = -primaryAnchorDeltaXAbsQ12;
    }
    iVar1 = iVar1 + primaryAnchorDeltaXAbsQ12 +
            (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    iVar1 = (iVar1 * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue84;
    if (iVar1 - currentBestScore != 0 && currentBestScore <= iVar1) {
      currentBestScore = iVar1;
    }
  }
  if (g_GameFactionRuntimeImage.records[factionIndex].anchorCooldown0 != 0) {
    iVar1 = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12;
    if (-1 < iVar1) {
      iVar1 = -iVar1;
    }
    iVar2 = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12;
    if (-1 < iVar2) {
      iVar2 = -iVar2;
    }
    iVar1 = iVar1 + iVar2 + (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    iVar1 = (iVar1 * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue84;
    if (iVar1 - currentBestScore != 0 && currentBestScore <= iVar1) {
      currentBestScore = iVar1;
    }
  }
  return currentBestScore;
}


/* Address: 0x0053B330.
   Ownership: gameplay/ai/units.
   Purpose: Scores distance from the entity to workspace07, or workspace03 when workspace07 is empty, with the
   alternate workspace path receiving the verified three-quarter scaling. Distance score vs the secondary workspace
   list. Typed parameters: p2 currentBestScore→AiCandidateScore32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged. Typed
   parameters: p4 armyRuntimeSlot→ArmyRuntimeSlot *.
*/
AiSecondaryWorkspaceDistanceSelectionRegs8 __thandor_eax_ebx_cf_preserve_ecx_edx
AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int iVar1;
  int recordsRemaining;
  Q12 deltaYAbsQ12;
  AiTargetWorkspaceEntry *pAVar2;
  AiTargetWorkspaceEntry *workspaceRecordCursor;
  AiSecondaryWorkspaceDistanceSelectionRegs8 AVar3;
  
  pAVar2 = (AiTargetWorkspaceEntry *)0x0;
  recordsRemaining = g_AiWorkspace07Count;
  workspaceRecordCursor = g_AiWorkspaceBuffer07_Size0400;
  if (g_AiWorkspace07Count == 0) {
    recordsRemaining = g_AiWorkspace03Count;
    workspaceRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspaceBuffer03_Size1000;
    if (g_AiWorkspace03Count == 0)
    goto AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore_ReturnBestDistanceWeightedScore;
  }
  do {
    iVar1 = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.x -
            (int)workspaceRecordCursor->worldXQ12;
    if (-1 < iVar1) {
      iVar1 = -iVar1;
    }
    deltaYAbsQ12 = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.y -
                   workspaceRecordCursor->worldYQ12;
    if (-1 < deltaYAbsQ12) {
      deltaYAbsQ12 = -deltaYAbsQ12;
    }
    iVar1 = iVar1 + deltaYAbsQ12 + (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceBiasQ12
    ;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    iVar1 = (iVar1 * (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue88;
    if (g_AiWorkspace07Count == 0) {
      iVar1 = iVar1 * 3 >> 2;
    }
    if (currentBestScore < iVar1) {
      pAVar2 = workspaceRecordCursor;
      currentBestScore = iVar1;
    }
    workspaceRecordCursor = workspaceRecordCursor + 1;
    recordsRemaining = recordsRemaining + -1;
  } while (recordsRemaining != 0);
AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore_ReturnBestDistanceWeightedScore:
  AVar3.selectedEntry = pAVar2;
  AVar3.score = currentBestScore;
  return AVar3;
}


/* Address: 0x0053B3E0.
   Ownership: gameplay/ai/units.
   Purpose: Sets behavior mode eight, clears the direct-state bit, resets the source record state, and submits the
   workspace record coordinates to the shared movement assignment helper.
   Cross-module calls: ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement].
*/
void __thandor_preserve_eax
AiUnitCommand_AssignWorkspacePoint
          (dword *workspacePoint,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext)

{
  armyRuntime->runtimeState8C = 8;
  armyRuntime->runtimeState94 = armyRuntime->runtimeState94 & 0xfffffffe;
  workspacePoint[2] = 0;
  ArmyRuntime_QueueOrStartMoveCommandVariantA
            (workspacePoint[1],*workspacePoint,(ArmyMovementRuntime *)armyRuntime);
  return;
}


/* Address: 0x0053B420.
   Ownership: gameplay/ai/units.
   Purpose: Chooses the active faction anchor pair, sets behavior mode eight, clears the direct-state bit, and
   submits that anchor to the shared movement assignment helper. It is distinct from FrontendPlayerIndex_V306,
   PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId
   domains.
   Cross-module calls: ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement].
*/
void __thandor_preserve_eax_edx
AiUnitCommand_AssignFactionAnchorPoint
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext)

{
  GraphicsWorldCoordinateQ12 targetWorldX;
  GraphicsWorldCoordinateQ12 targetWorldY;
  
  targetWorldX = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12;
  targetWorldY = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
    targetWorldX = g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12;
    targetWorldY = g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12;
  }
  armyRuntime->runtimeState8C = 8;
  armyRuntime->runtimeState94 = armyRuntime->runtimeState94 & 0xfffffffe;
  ArmyRuntime_QueueOrStartMoveCommandVariantA
            (targetWorldY,targetWorldX,(ArmyMovementRuntime *)armyRuntime);
  return;
}


/* Address: 0x0053B480.
   Ownership: gameplay/ai/units.
   Purpose: Appends an entity pointer to the bounded 64-entry group-assignment workspace when the entity is not
   already marked assigned. Collects idle military entities for group assignment (feeds
   AiUnitGroup_AssignCollectedEntitiesToBestTarget). Typed parameters: p3 entityRuntime→GameEntityRuntime *.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_CollectUnassignedEntity
          (ArmyRuntimeSlot *armyRuntimeSlot,WorldRuntimeContext *worldRuntimeContext)

{
  if ((g_AiCollectedEntityCount < 0x40) && ((armyRuntimeSlot->runtimeState94 & 1) == 0)) {
    g_AiWorkspaceBuffer14_Size0100[g_AiCollectedEntityCount] = armyRuntimeSlot;
    g_AiCollectedEntityCount = g_AiCollectedEntityCount + 1;
  }
  return;
}


/* Address: 0x0053B620.
   Ownership: gameplay/ai/units.
   Purpose: Updates a class-0x12 entity by reusing its current target when valid, otherwise scoring separated
   workspace08 sites with workspace-distance penalties, assigned-count pressure, and the special 0x14A multiplier
   before assigning movement. Class 0x12 special handler reached from the (single-deref) military dispatch.
   Cross-module calls: ArmyRuntime_UpdateMovementAndWaypoints [gameplay/army/movement], FixedMath_SinCosScaled
   [core/math/fixed], ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement],
   AiWorkspace03_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiWorkspace02_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiPlacement_QueryReachableSiteBucketCount [gameplay/ai/placement].
*/
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_UpdateSpecialClass12Entity
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntime,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *pAVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  FieldGridCell *workspaceRecord;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar8;
  FixedSinCosEdxEax8 FVar9;
  StatusValueEaxCf5 SVar10;
  WorldPositionXYEaxEdxCf9 WVar11;
  FieldGridCell *local_20;
  ModelRuntimeNode *modelNode;
  
  iVar2 = g_AiWorkspace04Count;
  iVar5 = g_AiWorkspace00Count;
  if (((armyRuntime->movementStateFlags & 0x100) == 0) &&
     (WVar11 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)armyRuntime), WVar11.carry)) {
    if (iVar5 == iVar2) {
      modelNode = armyRuntime->modelNodeRuntime;
      FVar9 = FixedMath_SinCosScaled((modelNode->modelPayload).worldRotationAngle2,0x2d05);
      iVar5 = (modelNode->worldTransform).translation.x;
      iVar2 = (modelNode->worldTransform).translation.y;
      armyRuntime->runtimeState8C = 8;
      ArmyRuntime_QueueOrStartMoveCommandVariantA
                ((int)(FVar9 >> 0x20) + iVar2,(int)FVar9 + iVar5,(ArmyMovementRuntime *)armyRuntime)
      ;
    }
    else if (((armyRuntime->movementStateFlags & 0x100) == 0) && (g_AiWorkspace08Count != 0)) {
      uVar6 = 0;
      iVar5 = g_AiWorkspace08Count;
      terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
      do {
        pAVar1 = g_AiKnowledgeData;
        workspaceRecord = terrainFeatureEntry->cell;
        iVar2 = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                          (workspaceRecord->worldY,workspaceRecord->worldX);
        if (((int)(pAVar1->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= iVar2) &&
           (iVar2 = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                              (workspaceRecord->worldY,workspaceRecord->worldX),
           (int)(pAVar1->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= iVar2)) {
          SVar10 = AiPlacement_QueryReachableSiteBucketCount
                             (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,
                              worldRuntime);
          if (!SVar10.carry) {
            if (SVar10.valueOrError == 0) {
              return;
            }
            if ((SVar10.valueOrError < 5) &&
               (bVar8 = AiPlacement_ReserveSeparatedSpecialSiteChain
                                  (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,
                                   worldRuntime), !bVar8)) {
              return;
            }
            iVar2 = terrainFeatureEntry->priority *
                    (pAVar1->parameters).specialClass12Workspace08Field0cCoefficient;
            if (((pAVar1->parameters).specialClass12Workspace02NearDistanceCoefficient != 0) &&
               (iVar3 = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                                  (workspaceRecord->worldY,workspaceRecord->worldX),
               (int)(iVar3 - (pAVar1->parameters).specialClass12Workspace02NearDistanceThresholdQ12)
               < 0)) {
              iVar2 = iVar2 + iVar3 * (pAVar1->parameters).
                                      specialClass12Workspace02NearDistanceCoefficient;
            }
            if ((pAVar1->parameters).specialClass12SecondaryWorkspaceShortfallCoefficient != 0) {
              iVar3 = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint
                                (workspaceRecord->worldY,workspaceRecord->worldX);
              iVar3 = iVar3 - (pAVar1->parameters).
                              specialClass12SecondaryWorkspaceDistanceThresholdQ12;
              if (iVar3 < 0) {
                iVar2 = iVar2 - iVar3 * (pAVar1->parameters).
                                        specialClass12SecondaryWorkspaceShortfallCoefficient;
              }
            }
            if ((pAVar1->parameters).specialClass12EntityDistanceCoefficient != 0) {
              workspaceRecord = terrainFeatureEntry->cell;
              iVar3 = (armyRuntime->modelNodeRuntime->worldTransform).translation.x -
                      workspaceRecord->worldX;
              if (-1 < iVar3) {
                iVar3 = -iVar3;
              }
              iVar7 = (armyRuntime->modelNodeRuntime->worldTransform).translation.y -
                      workspaceRecord->worldY;
              if (-1 < iVar7) {
                iVar7 = -iVar7;
              }
              iVar3 = iVar3 + iVar7 + (pAVar1->parameters).specialClass12EntityDistanceBiasQ12;
              if (-1 < iVar3) {
                iVar2 = iVar2 + iVar3 * (pAVar1->parameters).specialClass12EntityDistanceCoefficient
                ;
              }
            }
            if (terrainFeatureEntry->armyAssetId == ARM_0330_BUILDING_MDL0303) {
              iVar2 = iVar2 * 2;
            }
            iVar3 = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
            uVar4 = (uint)(iVar2 * 3) / (iVar3 + 3U);
            if ((int)uVar6 < (int)uVar4) {
              uVar6 = uVar4;
              local_20 = workspaceRecord;
            }
          }
        }
        terrainFeatureEntry = terrainFeatureEntry + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      if (uVar6 != 0) {
        armyRuntime->runtimeState8C = 8;
        ArmyRuntime_QueueOrStartMoveCommandVariantA
                  (local_20->worldY,local_20->worldX,(ArmyMovementRuntime *)armyRuntime);
      }
    }
  }
  else {
    iVar5 = (armyRuntime->articulatedContact).fallbackPosition0Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.x;
    if (iVar5 < 0) {
      iVar5 = -iVar5;
    }
    iVar2 = (armyRuntime->articulatedContact).fallbackPosition1Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.y;
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags | 0x100;
    if ((iVar5 < 0x1b03) && (iVar2 < 0x1b03)) {
      ArmyRuntime_ResetMovementStateFromModel(armyRuntime);
    }
  }
  return;
}

