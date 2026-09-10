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
void AiUnitBehavior_UpdateWorkspace01Entities
               (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  byte *pbVar1;
  void *unusedDefinitionOrAssetArgument;
  int extraout_ECX;
  int extraout_ECX_00;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
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
        behaviorCooldownCounter = (int *)((entityRuntime1->common).reserved68_9F + 0x24);
        *behaviorCooldownCounter = *behaviorCooldownCounter + -1;
        if (*behaviorCooldownCounter != 0) {
          if (-1 < *behaviorCooldownCounter)
          goto AiUnitBehavior_UpdateWorkspace01Entities_AdvanceAfterBehaviorDispatchOrSkip;
          pbVar1 = (entityRuntime1->common).reserved68_9F + 0x24;
          *(int *)pbVar1 = *(int *)pbVar1 + 1;
        }
      }
      unusedDefinitionOrAssetArgument = armySlot1->definitionOrAsset;
      if (*(int *)((int)unusedDefinitionOrAssetArgument + 0x4c) == 0x12) {
        AiUnitBehavior_UpdateSpecialClass12Entity
                  (unusedDefinitionOrAssetArgument,(ArmyRuntimeSlot *)armySlot1->linkedEntityRuntime
                   ,factionIndex,worldRuntime);
        workspaceEntriesRemaining = extraout_ECX;
      }
      else if ((((*(int *)((int)unusedDefinitionOrAssetArgument + 0x4c) == 1) ||
                (*(int *)((int)unusedDefinitionOrAssetArgument + 0x4c) == 2)) ||
               (*(int *)((int)unusedDefinitionOrAssetArgument + 0x4c) == 3)) ||
              ((*(int *)((int)unusedDefinitionOrAssetArgument + 0x4c) == 0x13 ||
               (*(int *)((int)unusedDefinitionOrAssetArgument + 0x4c) == 0x11)))) {
        AiUnitBehavior_SelectBestAnchorAction
                  (unusedDefinitionOrAssetArgument,(ArmyRuntimeSlot *)armySlot1->linkedEntityRuntime
                   ,factionIndex,worldRuntime);
        workspaceEntriesRemaining = extraout_ECX_00;
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
void AiUnitBehavior_SelectBestAnchorAction
               (undefined4 definitionOrAsset,ArmyRuntimeSlot *armyRuntimeSlot,
               FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint extraout_ECX;
  uint selectedAnchorActionKind;
  int bestAnchorActionScore;
  int currentBestScore;
  dword *unaff_EBX;
  dword *unaff_EDI;
  AiCandidateScoreEaxPreservedEdxCarrier64 AVar1;
  
  AVar1 = AiUnitBehavior_ComputeWorkspace05DistanceScore(0,definitionOrAsset,armyRuntimeSlot);
  bestAnchorActionScore = (int)(AVar1 >> 0x20);
  if ((int)AVar1 != bestAnchorActionScore) {
    bestAnchorActionScore = (int)AVar1;
    unaff_EDI = unaff_EBX;
  }
  AVar1 = AiUnitBehavior_ComputeFactionAnchorDistanceScore
                    (factionIndex,bestAnchorActionScore,definitionOrAsset,armyRuntimeSlot);
  currentBestScore = (int)(AVar1 >> 0x20);
  if ((int)AVar1 != currentBestScore) {
    currentBestScore = (int)AVar1;
  }
  AVar1 = AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
                    (currentBestScore,definitionOrAsset,armyRuntimeSlot);
  selectedAnchorActionKind = extraout_ECX;
  if ((int)AVar1 != (int)(AVar1 >> 0x20)) {
    selectedAnchorActionKind = 3;
    unaff_EDI = unaff_EBX;
  }
  if (selectedAnchorActionKind != 0) {
    if (selectedAnchorActionKind == 1) {
      AiUnitCommand_AssignWorkspacePoint(unaff_EDI,armyRuntimeSlot,worldRuntime);
      return;
    }
    if (selectedAnchorActionKind < 3) {
      AiUnitCommand_AssignFactionAnchorPoint(factionIndex,armyRuntimeSlot,worldRuntime);
      return;
    }
  }
  AiUnitBehavior_CollectUnassignedEntity
            (unaff_EDI,(GameEntityRuntime *)armyRuntimeSlot,worldRuntime);
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
AiCandidateScoreEaxPreservedEdxCarrier64
AiUnitBehavior_ComputeWorkspace05DistanceScore
          (AiCandidateScore32 currentBestScore,undefined4 definitionOrAsset,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int iVar1;
  int recordsRemaining;
  undefined4 in_EDX;
  Q12 deltaYAbsQ12;
  AiScoredSiteWorkspaceEntry *workspaceRecordCursor;
  
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
      currentBestScore = iVar1;
    }
    workspaceRecordCursor = workspaceRecordCursor + 1;
  }
  return CONCAT44(in_EDX,currentBestScore);
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
AiCandidateScoreEaxPreservedEdxCarrier64
AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          undefined4 definitionOrAsset,ArmyRuntimeSlot *armyRuntimeSlot)

{
  int iVar1;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,currentBestScore);
}

/* Address: 0x0053B330.
   Ownership: gameplay/ai/units.
   Purpose: Scores distance from the entity to workspace07, or workspace03 when workspace07 is empty, with the
   alternate workspace path receiving the verified three-quarter scaling. Distance score vs the secondary workspace
   list. Typed parameters: p2 currentBestScore→AiCandidateScore32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged. Typed
   parameters: p4 armyRuntimeSlot→ArmyRuntimeSlot *.
*/
AiCandidateScoreEaxPreservedEdxCarrier64
AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,undefined4 definitionOrAsset,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int iVar1;
  int recordsRemaining;
  undefined4 in_EDX;
  Q12 deltaYAbsQ12;
  AiTargetWorkspaceEntry *workspaceRecordCursor;
  
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
      currentBestScore = iVar1;
    }
    workspaceRecordCursor = workspaceRecordCursor + 1;
    recordsRemaining = recordsRemaining + -1;
  } while (recordsRemaining != 0);
AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore_ReturnBestDistanceWeightedScore:
  return CONCAT44(in_EDX,currentBestScore);
}

/* Address: 0x0053B3E0.
   Ownership: gameplay/ai/units.
   Purpose: Sets behavior mode eight, clears the direct-state bit, resets the source record state, and submits the
   workspace record coordinates to the shared movement assignment helper.
   Cross-module calls: ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement].
*/
void AiUnitCommand_AssignWorkspacePoint
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
void AiUnitCommand_AssignFactionAnchorPoint
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
void AiUnitBehavior_CollectUnassignedEntity
               (undefined4 selectionSource,GameEntityRuntime *entityRuntime,
               WorldRuntimeContext *worldRuntimeContext)

{
  if ((g_AiCollectedEntityCount < 0x40) &&
     ((*(uint *)((entityRuntime->common).reserved68_9F + 0x2c) & 1) == 0)) {
    g_AiWorkspaceBuffer14_Size0100[g_AiCollectedEntityCount] = entityRuntime;
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
void AiUnitBehavior_UpdateSpecialClass12Entity
               (undefined4 unusedDefinitionOrAssetArgument,ArmyRuntimeSlot *armyRuntime,
               FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  uint uVar2;
  dword dVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  AiKnowledgeDataImage *extraout_ECX_04;
  AiKnowledgeDataImage *extraout_ECX_05;
  AiKnowledgeDataImage *knowledgeData8;
  uint extraout_EDX;
  uint extraout_EDX_00;
  uint extraout_EDX_01;
  uint extraout_EDX_02;
  uint extraout_EDX_03;
  uint extraout_EDX_04;
  int iVar7;
  uint uVar8;
  FieldGridCell *workspaceRecord;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool bVar9;
  FixedSinCosEdxEax8 FVar10;
  AiWorkspaceCountEaxPreservedEdxCarrier64 AVar11;
  FieldGridCell *local_20;
  ModelRuntimeNode *modelNode;
  
  iVar6 = g_AiWorkspace00Count;
  bVar9 = false;
  if (((armyRuntime->movementStateFlags & 0x100) == 0) &&
     (ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)armyRuntime), bVar9
     )) {
    if (iVar6 == extraout_ECX) {
      modelNode = armyRuntime->modelNodeRuntime;
      FVar10 = FixedMath_SinCosScaled((modelNode->modelPayload).worldRotationAngle2,0x2d05);
      iVar6 = (modelNode->worldTransform).translation.x;
      iVar1 = (modelNode->worldTransform).translation.y;
      armyRuntime->runtimeState8C = 8;
      ArmyRuntime_QueueOrStartMoveCommandVariantA
                ((int)(FVar10 >> 0x20) + iVar1,(int)FVar10 + iVar6,
                 (ArmyMovementRuntime *)armyRuntime);
    }
    else if (((armyRuntime->movementStateFlags & 0x100) == 0) &&
            (terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200, g_AiWorkspace08Count != 0)) {
      do {
        knowledgeData8 = g_AiKnowledgeData;
        workspaceRecord = terrainFeatureEntry->cell;
        iVar1 = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                          (workspaceRecord->worldY,workspaceRecord->worldX);
        iVar6 = extraout_ECX_00;
        uVar8 = extraout_EDX;
        if ((int)(knowledgeData8->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= iVar1) {
          uVar2 = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                            (workspaceRecord->worldY,workspaceRecord->worldX);
          uVar5 = (knowledgeData8->parameters).specialSiteMinimumWorkspaceDistanceQ12;
          bVar9 = uVar2 < uVar5;
          iVar6 = extraout_ECX_01;
          uVar8 = extraout_EDX_00;
          if (((int)uVar5 <= (int)uVar2) &&
             (dVar3 = AiPlacement_QueryReachableSiteBucketCount
                                (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,
                                 worldRuntime), iVar6 = extraout_ECX_02, uVar8 = extraout_EDX_01,
             !bVar9)) {
            if (dVar3 == 0) {
              return;
            }
            bVar9 = dVar3 < 4;
            uVar8 = extraout_EDX_01;
            if ((dVar3 < 5) &&
               (AiPlacement_ReserveSeparatedSpecialSiteChain
                          (extraout_ECX_02,extraout_EDX_01,terrainFeatureEntry->armyAssetId,
                           workspaceRecord,factionIndex,worldRuntime), iVar6 = extraout_ECX_03,
               uVar8 = extraout_EDX_02, !bVar9)) {
              return;
            }
            iVar1 = terrainFeatureEntry->priority *
                    (knowledgeData8->parameters).specialClass12Workspace08Field0cCoefficient;
            if (((knowledgeData8->parameters).specialClass12Workspace02NearDistanceCoefficient != 0)
               && (iVar4 = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                                     (workspaceRecord->worldY,workspaceRecord->worldX),
                  knowledgeData8 = extraout_ECX_04, uVar8 = extraout_EDX_03,
                  (int)(iVar4 - (extraout_ECX_04->parameters).
                                specialClass12Workspace02NearDistanceThresholdQ12) < 0)) {
              iVar1 = iVar1 + iVar4 * (extraout_ECX_04->parameters).
                                      specialClass12Workspace02NearDistanceCoefficient;
            }
            if ((knowledgeData8->parameters).specialClass12SecondaryWorkspaceShortfallCoefficient !=
                0) {
              iVar4 = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint
                                (workspaceRecord->worldY,workspaceRecord->worldX);
              iVar4 = iVar4 - (extraout_ECX_05->parameters).
                              specialClass12SecondaryWorkspaceDistanceThresholdQ12;
              knowledgeData8 = extraout_ECX_05;
              uVar8 = extraout_EDX_04;
              if (iVar4 < 0) {
                iVar1 = iVar1 - iVar4 * (extraout_ECX_05->parameters).
                                        specialClass12SecondaryWorkspaceShortfallCoefficient;
              }
            }
            if ((knowledgeData8->parameters).specialClass12EntityDistanceCoefficient != 0) {
              workspaceRecord = terrainFeatureEntry->cell;
              iVar4 = (armyRuntime->modelNodeRuntime->worldTransform).translation.x -
                      workspaceRecord->worldX;
              if (-1 < iVar4) {
                iVar4 = -iVar4;
              }
              iVar7 = (armyRuntime->modelNodeRuntime->worldTransform).translation.y -
                      workspaceRecord->worldY;
              if (-1 < iVar7) {
                iVar7 = -iVar7;
              }
              iVar4 = iVar4 + iVar7 +
                      (knowledgeData8->parameters).specialClass12EntityDistanceBiasQ12;
              if (-1 < iVar4) {
                iVar1 = iVar1 + iVar4 * (knowledgeData8->parameters).
                                        specialClass12EntityDistanceCoefficient;
              }
            }
            if (terrainFeatureEntry->armyAssetId == ARM_0330_BUILDING_MDL0303) {
              iVar1 = iVar1 * 2;
            }
            AVar11 = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
            uVar5 = (uint)(iVar1 * 3) / ((int)AVar11 + 3U);
            if ((int)uVar8 < (int)uVar5) {
              uVar8 = uVar5;
              local_20 = workspaceRecord;
            }
          }
        }
        terrainFeatureEntry = terrainFeatureEntry + 1;
      } while (iVar6 != 1);
      if (uVar8 != 0) {
        armyRuntime->runtimeState8C = 8;
        ArmyRuntime_QueueOrStartMoveCommandVariantA
                  (local_20->worldY,local_20->worldX,(ArmyMovementRuntime *)armyRuntime);
      }
    }
  }
  else {
    iVar6 = (armyRuntime->articulatedContact).fallbackPosition0Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.x;
    if (iVar6 < 0) {
      iVar6 = -iVar6;
    }
    iVar1 = (armyRuntime->articulatedContact).fallbackPosition1Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.y;
    if (iVar1 < 0) {
      iVar1 = -iVar1;
    }
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags | 0x100;
    if ((iVar6 < 0x1b03) && (iVar1 < 0x1b03)) {
      ArmyRuntime_ResetMovementStateFromModel(armyRuntime);
    }
  }
  return;
}
