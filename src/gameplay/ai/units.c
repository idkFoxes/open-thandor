/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/units.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/units.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/units. */

/* Address: 0x0053B0E0.
   Per AI tick for the faction's own units (workspace 01): clears the collected-army list, lets busy units
   (command flags 0x01/0x10) wait for their behaviour cooldown (common +0x8C), skips units with command
   flag 0x02, and dispatches the rest by model class: class 18 to UpdateSpecialClass12Entity, the ground,
   tracked, walker, water and glider classes (1/2/3/19/17) to SelectBestAnchorAction. The class is read with a
   single dereference from the model at +0x4C.
*/
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_UpdateWorkspace01Entities
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  uint8_t *cooldownCounterBytes;
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  MdlDefinitionSemanticPrefix80 *modelDefinition;
  int *behaviorCooldownCounter;
  ArmyRuntimeSlot *armySlot;
  GameEntityRuntime *slotEntityRuntime;

  g_AiCollectedEntityCount = 0;
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspaceBuffer01_Size0200;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--, workspaceEntryCursor++) {
    armySlot = workspaceEntryCursor->armyRuntime;
    if (armySlot == NULL) continue;
    slotEntityRuntime = armySlot->linkedEntityRuntime;
    if ((slotEntityRuntime->common.commandFlags & 0x13) != 0) {
      if ((slotEntityRuntime->common.commandFlags & 2) != 0) continue;
      /* Busy entities only get a behavior update when their cooldown runs out (or was already negative,
         which the increment below undoes). */
      behaviorCooldownCounter = (int *)(slotEntityRuntime->common.reserved80_9F + 0xc);
      *behaviorCooldownCounter = *behaviorCooldownCounter + -1;
      if (*behaviorCooldownCounter != 0) {
        if (-1 < *behaviorCooldownCounter) continue;
        cooldownCounterBytes = slotEntityRuntime->common.reserved80_9F + 0xc;
        *(int *)cooldownCounterBytes = *(int *)cooldownCounterBytes + 1;
      }
    }
    modelDefinition =
         (MdlDefinitionSemanticPrefix80 *)armySlot->modelRuntimeOrSavedOffset.modelRuntime;
    if (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_18) {
      AiUnitBehavior_UpdateSpecialClass12Entity
                (modelDefinition,(ArmyRuntimeSlot *)armySlot->linkedEntityRuntime,factionIndex,
                 worldRuntime);
    }
    else if ((((modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_01_GROUND) ||
              (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_02_TRACKED)) ||
             (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_03_ARTICULATED_WALKER)) ||
            ((modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_19_WATER_SURFACE ||
             (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER)))) {
      AiUnitBehavior_SelectBestAnchorAction
                (modelDefinition,(ArmyRuntimeSlot *)armySlot->linkedEntityRuntime,factionIndex,
                 worldRuntime);
    }
  }
  return;
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
  int workspaceScore;
  AiCandidateScore32 factionAnchorScore;
  uint32_t selectedAnchorActionKind;
  int bestAnchorActionScore;
  int currentBestScore;
  AiScoredSiteWorkspaceEntry *selectedWorkspaceEntry;
  AiWorkspace05DistanceSelectionRegs8 workspaceSelection;
  AiSecondaryWorkspaceDistanceSelectionRegs8 secondarySelection;
  
  workspaceSelection = AiUnitBehavior_ComputeWorkspace05DistanceScore(0,modelDefinition,armyRuntimeSlot);
  workspaceScore = workspaceSelection.score;
  currentBestScore = 0;
  if (workspaceScore != 0) {
    currentBestScore = workspaceScore;
    selectedWorkspaceEntry = workspaceSelection.selectedEntry;
  }
  selectedAnchorActionKind = (uint32_t)(workspaceScore != 0);
  factionAnchorScore = AiUnitBehavior_ComputeFactionAnchorDistanceScore
                    (factionIndex,currentBestScore,modelDefinition,armyRuntimeSlot);
  if (factionAnchorScore != currentBestScore) {
    selectedAnchorActionKind = 2;
    currentBestScore = factionAnchorScore;
  }
  secondarySelection = AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
                    (currentBestScore,modelDefinition,armyRuntimeSlot);
  if (secondarySelection.score != currentBestScore) {
    selectedAnchorActionKind = 3;
    selectedWorkspaceEntry = (AiScoredSiteWorkspaceEntry *)secondarySelection.selectedEntry;
  }
  if (selectedAnchorActionKind != 0) {
    if (selectedAnchorActionKind == 1) {
      AiUnitCommand_AssignWorkspacePoint
                ((uint32_t *)selectedWorkspaceEntry,armyRuntimeSlot,worldRuntime);
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
  int deltaXOrScore;
  int recordsRemaining;
  Q12 deltaYAbsQ12;
  AiScoredSiteWorkspaceEntry *bestEntry;
  AiScoredSiteWorkspaceEntry *workspaceRecordCursor;
  AiWorkspace05DistanceSelectionRegs8 selection;
  
  bestEntry = (AiScoredSiteWorkspaceEntry *)0x0;
  workspaceRecordCursor = g_AiWorkspaceBuffer05_Size0200;
  for (recordsRemaining = g_AiWorkspace05Count; recordsRemaining != 0;
      recordsRemaining = recordsRemaining + -1) {
    deltaXOrScore = (armyRuntimeSlot->articulatedContact).fallbackPosition0Q12 -
            workspaceRecordCursor->cellWorldXQ12;
    if (-1 < deltaXOrScore) {
      deltaXOrScore = -deltaXOrScore;
    }
    deltaYAbsQ12 = (armyRuntimeSlot->articulatedContact).fallbackPosition1Q12 -
                   workspaceRecordCursor->cellWorldYQ12;
    if (-1 < deltaYAbsQ12) {
      deltaYAbsQ12 = -deltaYAbsQ12;
    }
    deltaXOrScore = deltaXOrScore + deltaYAbsQ12 + (g_AiKnowledgeData->parameters).workspace05DistanceBiasQ12;
    if (deltaXOrScore < 0) {
      deltaXOrScore = 0;
    }
    deltaXOrScore = (deltaXOrScore * (g_AiKnowledgeData->parameters).workspace05DistanceScaleQ12 +
             workspaceRecordCursor->score >> 0xc) * armyRuntimeSlot->definitionClassValue80;
    if (deltaXOrScore - currentBestScore != 0 && currentBestScore <= deltaXOrScore) {
      bestEntry = workspaceRecordCursor;
      currentBestScore = deltaXOrScore;
    }
    workspaceRecordCursor = workspaceRecordCursor + 1;
  }
  selection.selectedEntry = bestEntry;
  selection.score = currentBestScore;
  return selection;
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
  int deltaOrScore;
  Q12 primaryAnchorDeltaXAbsQ12;
  int secondaryAnchorDeltaXAbsQ12;
  ModelRuntimeNode *modelNode;
  
  modelNode = armyRuntimeSlot->modelNodeRuntime;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
    deltaOrScore = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12;
    if (-1 < deltaOrScore) {
      deltaOrScore = -deltaOrScore;
    }
    primaryAnchorDeltaXAbsQ12 =
         (modelNode->worldTransform).translation.x -
         g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12;
    if (-1 < primaryAnchorDeltaXAbsQ12) {
      primaryAnchorDeltaXAbsQ12 = -primaryAnchorDeltaXAbsQ12;
    }
    deltaOrScore = deltaOrScore + primaryAnchorDeltaXAbsQ12 +
            (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
    if (deltaOrScore < 0) {
      deltaOrScore = 0;
    }
    deltaOrScore = (deltaOrScore * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue84;
    if (deltaOrScore - currentBestScore != 0 && currentBestScore <= deltaOrScore) {
      currentBestScore = deltaOrScore;
    }
  }
  if (g_GameFactionRuntimeImage.records[factionIndex].anchorCooldown0 != 0) {
    deltaOrScore = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12;
    if (-1 < deltaOrScore) {
      deltaOrScore = -deltaOrScore;
    }
    secondaryAnchorDeltaXAbsQ12 = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12;
    if (-1 < secondaryAnchorDeltaXAbsQ12) {
      secondaryAnchorDeltaXAbsQ12 = -secondaryAnchorDeltaXAbsQ12;
    }
    deltaOrScore = deltaOrScore + secondaryAnchorDeltaXAbsQ12 + (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
    if (deltaOrScore < 0) {
      deltaOrScore = 0;
    }
    deltaOrScore = (deltaOrScore * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue84;
    if (deltaOrScore - currentBestScore != 0 && currentBestScore <= deltaOrScore) {
      currentBestScore = deltaOrScore;
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
  int deltaXOrScore;
  int recordsRemaining;
  Q12 deltaYAbsQ12;
  AiTargetWorkspaceEntry *bestEntry;
  AiTargetWorkspaceEntry *workspaceRecordCursor;
  AiSecondaryWorkspaceDistanceSelectionRegs8 selection;
  
  bestEntry = (AiTargetWorkspaceEntry *)0x0;
  recordsRemaining = g_AiWorkspace07Count;
  workspaceRecordCursor = g_AiWorkspaceBuffer07_Size0400;
  if (g_AiWorkspace07Count == 0) {
    recordsRemaining = g_AiWorkspace03Count;
    workspaceRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspaceBuffer03_Size1000;
  }
  for (; recordsRemaining != 0; recordsRemaining = recordsRemaining + -1) {
    deltaXOrScore = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.x -
            (int)workspaceRecordCursor->worldXQ12;
    if (-1 < deltaXOrScore) {
      deltaXOrScore = -deltaXOrScore;
    }
    deltaYAbsQ12 = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.y -
                   workspaceRecordCursor->worldYQ12;
    if (-1 < deltaYAbsQ12) {
      deltaYAbsQ12 = -deltaYAbsQ12;
    }
    deltaXOrScore = deltaXOrScore + deltaYAbsQ12 + (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceBiasQ12
    ;
    if (deltaXOrScore < 0) {
      deltaXOrScore = 0;
    }
    deltaXOrScore = (deltaXOrScore * (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue88;
    if (g_AiWorkspace07Count == 0) {
      deltaXOrScore = deltaXOrScore * 3 >> 2;
    }
    if (currentBestScore < deltaXOrScore) {
      bestEntry = workspaceRecordCursor;
      currentBestScore = deltaXOrScore;
    }
    workspaceRecordCursor = workspaceRecordCursor + 1;
  }
  selection.selectedEntry = bestEntry;
  selection.score = currentBestScore;
  return selection;
}


/* Address: 0x0053B3E0.
   Ownership: gameplay/ai/units.
   Purpose: Sets behavior mode eight, clears the direct-state bit, resets the source record state, and submits the
   workspace record coordinates to the shared movement assignment helper.
   Cross-module calls: ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement].
*/
void __thandor_preserve_eax
AiUnitCommand_AssignWorkspacePoint
          (uint32_t *workspacePoint,ArmyRuntimeSlot *armyRuntime,
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
  AiKnowledgeDataImage *knowledgeData;
  int siteScoreOrY;
  int distanceTerm;
  uint32_t weightedScore;
  int sitesRemainingOrX;
  uint32_t bestScore;
  int deltaY;
  FieldGridCell *workspaceRecord;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  bool chainReserved;
  FixedSinCosEdxEax8 headingOffset;
  StatusResult bucketCount;
  MovementStepResult movementUpdate;
  FieldGridCell *bestCell;
  ModelRuntimeNode *modelNode;
  
  siteScoreOrY = g_AiWorkspace04Count;
  sitesRemainingOrX = g_AiWorkspace00Count;
  if (((armyRuntime->movementStateFlags & 0x100) == 0) &&
     (movementUpdate = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)armyRuntime), movementUpdate.arrived)) {
    if (sitesRemainingOrX == siteScoreOrY) {
      modelNode = armyRuntime->modelNodeRuntime;
      headingOffset = FixedMath_SinCosScaled((modelNode->modelPayload).worldRotationAngle2,0x2d05);
      sitesRemainingOrX = (modelNode->worldTransform).translation.x;
      siteScoreOrY = (modelNode->worldTransform).translation.y;
      armyRuntime->runtimeState8C = 8;
      ArmyRuntime_QueueOrStartMoveCommandVariantA
                ((int)(headingOffset >> 0x20) + siteScoreOrY,(int)headingOffset + sitesRemainingOrX,(ArmyMovementRuntime *)armyRuntime)
      ;
    }
    else if (((armyRuntime->movementStateFlags & 0x100) == 0) && (g_AiWorkspace08Count != 0)) {
      bestScore = 0;
      sitesRemainingOrX = g_AiWorkspace08Count;
      terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200;
      do {
        knowledgeData = g_AiKnowledgeData;
        workspaceRecord = terrainFeatureEntry->cell;
        siteScoreOrY = AiWorkspace03_GetMinimumManhattanDistanceToPoint
                          (workspaceRecord->worldY,workspaceRecord->worldX);
        if (((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= siteScoreOrY) &&
           (siteScoreOrY = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                              (workspaceRecord->worldY,workspaceRecord->worldX),
           (int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <= siteScoreOrY)) {
          bucketCount = AiPlacement_QueryReachableSiteBucketCount
                             (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,
                              worldRuntime);
          if (!bucketCount.failed) {
            if (bucketCount.valueOrError == 0) {
              return;
            }
            if ((bucketCount.valueOrError < 5) &&
               (chainReserved = AiPlacement_ReserveSeparatedSpecialSiteChain
                                  (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,
                                   worldRuntime), !chainReserved)) {
              return;
            }
            siteScoreOrY = terrainFeatureEntry->priority *
                    (knowledgeData->parameters).specialClass12Workspace08Field0cCoefficient;
            if (((knowledgeData->parameters).specialClass12Workspace02NearDistanceCoefficient != 0) &&
               (distanceTerm = AiWorkspace02_GetMinimumManhattanDistanceToPoint
                                  (workspaceRecord->worldY,workspaceRecord->worldX),
               (int)(distanceTerm - (knowledgeData->parameters).specialClass12Workspace02NearDistanceThresholdQ12)
               < 0)) {
              siteScoreOrY = siteScoreOrY + distanceTerm * (knowledgeData->parameters).
                                      specialClass12Workspace02NearDistanceCoefficient;
            }
            if ((knowledgeData->parameters).specialClass12SecondaryWorkspaceShortfallCoefficient != 0) {
              distanceTerm = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint
                                (workspaceRecord->worldY,workspaceRecord->worldX);
              distanceTerm = distanceTerm - (knowledgeData->parameters).
                              specialClass12SecondaryWorkspaceDistanceThresholdQ12;
              if (distanceTerm < 0) {
                siteScoreOrY = siteScoreOrY - distanceTerm * (knowledgeData->parameters).
                                        specialClass12SecondaryWorkspaceShortfallCoefficient;
              }
            }
            if ((knowledgeData->parameters).specialClass12EntityDistanceCoefficient != 0) {
              workspaceRecord = terrainFeatureEntry->cell;
              distanceTerm = (armyRuntime->modelNodeRuntime->worldTransform).translation.x -
                      workspaceRecord->worldX;
              if (-1 < distanceTerm) {
                distanceTerm = -distanceTerm;
              }
              deltaY = (armyRuntime->modelNodeRuntime->worldTransform).translation.y -
                      workspaceRecord->worldY;
              if (-1 < deltaY) {
                deltaY = -deltaY;
              }
              distanceTerm = distanceTerm + deltaY + (knowledgeData->parameters).specialClass12EntityDistanceBiasQ12;
              if (-1 < distanceTerm) {
                siteScoreOrY = siteScoreOrY + distanceTerm * (knowledgeData->parameters).specialClass12EntityDistanceCoefficient
                ;
              }
            }
            if (terrainFeatureEntry->armyAssetId == ARM_0330_BUILDING_MDL0303) {
              siteScoreOrY = siteScoreOrY * 2;
            }
            distanceTerm = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
            weightedScore = (uint32_t)(siteScoreOrY * 3) / (distanceTerm + 3U);
            if ((int)bestScore < (int)weightedScore) {
              bestScore = weightedScore;
              bestCell = workspaceRecord;
            }
          }
        }
        terrainFeatureEntry = terrainFeatureEntry + 1;
        sitesRemainingOrX = sitesRemainingOrX + -1;
      } while (sitesRemainingOrX != 0);
      if (bestScore != 0) {
        armyRuntime->runtimeState8C = 8;
        ArmyRuntime_QueueOrStartMoveCommandVariantA
                  (bestCell->worldY,bestCell->worldX,(ArmyMovementRuntime *)armyRuntime);
      }
    }
  }
  else {
    sitesRemainingOrX = (armyRuntime->articulatedContact).fallbackPosition0Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.x;
    if (sitesRemainingOrX < 0) {
      sitesRemainingOrX = -sitesRemainingOrX;
    }
    siteScoreOrY = (armyRuntime->articulatedContact).fallbackPosition1Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.y;
    if (siteScoreOrY < 0) {
      siteScoreOrY = -siteScoreOrY;
    }
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags | 0x100;
    if ((sitesRemainingOrX < 0x1b03) && (siteScoreOrY < 0x1b03)) {
      ArmyRuntime_ResetMovementStateFromModel(armyRuntime);
    }
  }
  return;
}

