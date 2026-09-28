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
void AiUnitBehavior_UpdateWorkspace01Entities(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

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
   Decides what an idle military unit (ground, tracked, walker, glider or water class) does next. Three scorers run in turn, each
   given the best score so far and returning it unchanged unless it found better: a general site (workspace 05)
   -> move there (kind 1), the faction anchor -> move there (kind 2), a secondary-workspace target (kind 3).
   With kind 3 or no winner the unit is collected for the group assignment of
   AiUnitGroup_AssignCollectedEntitiesToBestTarget.
*/
void AiUnitBehavior_SelectBestAnchorAction
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int workspaceScore;
  AiCandidateScore32 factionAnchorScore;
  uint32_t selectedAnchorActionKind;
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
   Scores the general sites (workspace 05) for a unit and returns the best one if it beats currentBestScore:
   score = ((max(0, bias - Manhattan distance) * scale + site score) >> 12) * the unit's definitionClassValue80,
   so nearer and richer sites score higher. Returns currentBestScore and a NULL entry when none beats it.
   modelDefinition is not used.
   Original register convention: result in EAX and EBX, CF set on failure; ECX and EDX preserved.
*/
AiWorkspace05DistanceSelectionRegs8 AiUnitBehavior_ComputeWorkspace05DistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int negDeltaXOrScore;
  int recordsRemaining;
  Q12 negAbsDeltaYQ12;
  AiScoredSiteWorkspaceEntry *bestEntry;
  AiScoredSiteWorkspaceEntry *workspaceRecordCursor;
  AiWorkspace05DistanceSelectionRegs8 selection;
  
  bestEntry = NULL;
  workspaceRecordCursor = g_AiWorkspaceBuffer05_Size0200;
  for (recordsRemaining = g_AiWorkspace05Count; recordsRemaining != 0; recordsRemaining--) {
    negDeltaXOrScore = (armyRuntimeSlot->articulatedContact).fallbackPosition0Q12 -
            workspaceRecordCursor->cellWorldXQ12;
    if (-1 < negDeltaXOrScore) {
      negDeltaXOrScore = -negDeltaXOrScore;
    }
    negAbsDeltaYQ12 = (armyRuntimeSlot->articulatedContact).fallbackPosition1Q12 -
                   workspaceRecordCursor->cellWorldYQ12;
    if (-1 < negAbsDeltaYQ12) {
      negAbsDeltaYQ12 = -negAbsDeltaYQ12;
    }
    negDeltaXOrScore = negDeltaXOrScore + negAbsDeltaYQ12 + (g_AiKnowledgeData->parameters).workspace05DistanceBiasQ12;
    if (negDeltaXOrScore < 0) {
      negDeltaXOrScore = 0;
    }
    negDeltaXOrScore = (negDeltaXOrScore * (g_AiKnowledgeData->parameters).workspace05DistanceScaleQ12 +
             workspaceRecordCursor->score >> 0xc) * armyRuntimeSlot->definitionClassValue80;
    if (negDeltaXOrScore - currentBestScore != 0 && currentBestScore <= negDeltaXOrScore) {
      bestEntry = workspaceRecordCursor;
      currentBestScore = negDeltaXOrScore;
    }
    workspaceRecordCursor++;
  }
  selection.selectedEntry = bestEntry;
  selection.score = currentBestScore;
  return selection;
}


/* Address: 0x0053B260.
   Scores the faction's primary and secondary anchor points (each only while its cooldown runs) for a unit like
   the general sites: ((max(0, bias - distance) * scale) >> 12) * definitionClassValue84, and returns the
   highest of these and currentBestScore. modelDefinition is not used.
*/
AiCandidateScore32 AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot)

{
  int negDeltaOrScore;
  Q12 primaryAnchorNegAbsDeltaQ12;
  int secondaryAnchorNegAbsDeltaQ12;
  ModelRuntimeNode *modelNode;
  
  modelNode = armyRuntimeSlot->modelNodeRuntime;
  /* Both anchor coordinates are compared with translation.x: the original reads [modelNode + 0x94] twice
     (0x0053B281/0x0053B287 and 0x0053B2D5/0x0053B2DB), so this "distance" ignores the unit's Y. */
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
    negDeltaOrScore = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12;
    if (-1 < negDeltaOrScore) {
      negDeltaOrScore = -negDeltaOrScore;
    }
    primaryAnchorNegAbsDeltaQ12 =
         (modelNode->worldTransform).translation.x -
         g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12;
    if (-1 < primaryAnchorNegAbsDeltaQ12) {
      primaryAnchorNegAbsDeltaQ12 = -primaryAnchorNegAbsDeltaQ12;
    }
    negDeltaOrScore = negDeltaOrScore + primaryAnchorNegAbsDeltaQ12 +
            (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
    if (negDeltaOrScore < 0) {
      negDeltaOrScore = 0;
    }
    negDeltaOrScore = (negDeltaOrScore * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue84;
    if (negDeltaOrScore - currentBestScore != 0 && currentBestScore <= negDeltaOrScore) {
      currentBestScore = negDeltaOrScore;
    }
  }
  if (g_GameFactionRuntimeImage.records[factionIndex].anchorCooldown0 != 0) {
    negDeltaOrScore = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12;
    if (-1 < negDeltaOrScore) {
      negDeltaOrScore = -negDeltaOrScore;
    }
    secondaryAnchorNegAbsDeltaQ12 = (modelNode->worldTransform).translation.x -
            g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12;
    if (-1 < secondaryAnchorNegAbsDeltaQ12) {
      secondaryAnchorNegAbsDeltaQ12 = -secondaryAnchorNegAbsDeltaQ12;
    }
    negDeltaOrScore = negDeltaOrScore + secondaryAnchorNegAbsDeltaQ12 + (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
    if (negDeltaOrScore < 0) {
      negDeltaOrScore = 0;
    }
    negDeltaOrScore = (negDeltaOrScore * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue84;
    if (negDeltaOrScore - currentBestScore != 0 && currentBestScore <= negDeltaOrScore) {
      currentBestScore = negDeltaOrScore;
    }
  }
  return currentBestScore;
}


/* Address: 0x0053B330.
   Scores the target entries of workspace 07 for a unit, or those of workspace 03 at three quarters of the score
   when workspace 07 is empty: ((max(0, bias - Manhattan distance) * scale) >> 12) * definitionClassValue88.
   Returns the best entry if it beats currentBestScore, else currentBestScore and NULL. modelDefinition is not
   used.
   Original register convention: result in EAX and EBX, CF set on failure; ECX and EDX preserved.
*/
AiSecondaryWorkspaceDistanceSelectionRegs8 AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int negDeltaXOrScore;
  int recordsRemaining;
  Q12 negAbsDeltaYQ12;
  AiTargetWorkspaceEntry *bestEntry;
  AiTargetWorkspaceEntry *workspaceRecordCursor;
  AiSecondaryWorkspaceDistanceSelectionRegs8 selection;
  
  bestEntry = NULL;
  recordsRemaining = g_AiWorkspace07Count;
  workspaceRecordCursor = g_AiWorkspaceBuffer07_Size0400;
  if (g_AiWorkspace07Count == 0) {
    recordsRemaining = g_AiWorkspace03Count;
    workspaceRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspaceBuffer03_Size1000;
  }
  for (; recordsRemaining != 0; recordsRemaining--) {
    negDeltaXOrScore = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.x -
            (int)workspaceRecordCursor->worldXQ12;
    if (-1 < negDeltaXOrScore) {
      negDeltaXOrScore = -negDeltaXOrScore;
    }
    negAbsDeltaYQ12 = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.y -
                   workspaceRecordCursor->worldYQ12;
    if (-1 < negAbsDeltaYQ12) {
      negAbsDeltaYQ12 = -negAbsDeltaYQ12;
    }
    negDeltaXOrScore = negDeltaXOrScore + negAbsDeltaYQ12 + (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceBiasQ12
    ;
    if (negDeltaXOrScore < 0) {
      negDeltaXOrScore = 0;
    }
    negDeltaXOrScore = (negDeltaXOrScore * (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceScaleQ12 >> 0xc) *
            armyRuntimeSlot->definitionClassValue88;
    if (g_AiWorkspace07Count == 0) {
      negDeltaXOrScore = negDeltaXOrScore * 3 >> 2;
    }
    if (currentBestScore < negDeltaXOrScore) {
      bestEntry = workspaceRecordCursor;
      currentBestScore = negDeltaXOrScore;
    }
    workspaceRecordCursor++;
  }
  selection.selectedEntry = bestEntry;
  selection.score = currentBestScore;
  return selection;
}


/* Address: 0x0053B3E0.
   Sends the unit to a general site (a workspace-05 AiScoredSiteWorkspaceEntry: X, Y, score): marks it as
   AI-commanded and no longer group-assigned, zeroes the site's score (the bonus
   AiUnitBehavior_ComputeWorkspace05DistanceScore adds for it, so the next unit is less drawn there) and queues
   the move. worldRuntimeContext is not used.
*/
void AiUnitCommand_AssignWorkspacePoint(uint32_t *workspacePoint,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext)

{
  armyRuntime->runtimeState8C = AI_UNIT_COMMANDED_STATE;
  armyRuntime->runtimeState94 = armyRuntime->runtimeState94 & ~AI_UNIT_STATE94_GROUP_ASSIGNED;
  workspacePoint[2] = 0; /* AiScoredSiteWorkspaceEntry.score */
  ArmyRuntime_QueueOrStartMoveCommandVariantA
            (workspacePoint[1],*workspacePoint,(ArmyMovementRuntime *)armyRuntime);
  return;
}


/* Address: 0x0053B420.
   Sends the unit to the faction's anchor point: the primary anchor while its cooldown runs, otherwise the
   secondary one. Marks the unit as AI-commanded and no longer group-assigned, then queues the move.
   worldRuntimeContext is not used.
*/
void AiUnitCommand_AssignFactionAnchorPoint(FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext)

{
  GraphicsWorldCoordinateQ12 targetWorldX;
  GraphicsWorldCoordinateQ12 targetWorldY;
  
  /* The field at +0x390 (named ...AnchorYQ12) goes to the targetWorldX parameter and +0x394 to targetWorldY,
     so the faction record's anchor X/Y field names are probably swapped. */
  targetWorldX = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12;
  targetWorldY = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
    targetWorldX = g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12;
    targetWorldY = g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12;
  }
  armyRuntime->runtimeState8C = AI_UNIT_COMMANDED_STATE;
  armyRuntime->runtimeState94 = armyRuntime->runtimeState94 & ~AI_UNIT_STATE94_GROUP_ASSIGNED;
  ArmyRuntime_QueueOrStartMoveCommandVariantA
            (targetWorldY,targetWorldX,(ArmyMovementRuntime *)armyRuntime);
  return;
}


/* Address: 0x0053B480.
   Collects an idle unit that is not yet group-assigned into workspace 14 (at most 64 units), from which
   AiUnitGroup_AssignCollectedEntitiesToBestTarget later sends them together to one target.
   worldRuntimeContext is not used.
*/
void AiUnitBehavior_CollectUnassignedEntity(ArmyRuntimeSlot *armyRuntimeSlot,WorldRuntimeContext *worldRuntimeContext)

{
  if ((g_AiCollectedEntityCount < AI_WORKSPACE14_CAPACITY) &&
      ((armyRuntimeSlot->runtimeState94 & AI_UNIT_STATE94_GROUP_ASSIGNED) == 0)) {
    g_AiWorkspaceBuffer14_Size0100[g_AiCollectedEntityCount] = armyRuntimeSlot;
    g_AiCollectedEntityCount++;
  }
  return;
}


/* Address: 0x0053B620.
   AI behaviour of a runtime-class-18 unit, called from AiUnitBehavior_UpdateWorkspace01Entities and, while
   movement flag 0x100 is set, from its movement update. Once it has arrived (flag clear): with as many
   workspace-00 as workspace-04 entries it moves on 0x2D05 along its heading; otherwise it drives to the best
   resource site of workspace 08 that is far enough from workspaces 03/02, scored by priority, distances to
   workspaces 02/01 and to the unit, x2 for ARM_0330, then x3 / (assigned structures of that asset + 3). A site
   whose bucket query (AiPlacement_QueryReachableSiteBucketCount) fails is skipped; one with 0 buckets, or 1..4
   buckets and a CF-clear AiPlacement_ReserveSeparatedSpecialSiteChain, ends the update without a move.
   Not arrived or flag set: sets the flag and resets the movement once within 0x1B03 of its fallback position
   on both axes.
*/
void AiUnitBehavior_UpdateSpecialClass12Entity
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
  bool chainFailed;
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
               (chainFailed = AiPlacement_ReserveSeparatedSpecialSiteChain
                                  (terrainFeatureEntry->armyAssetId,workspaceRecord,factionIndex,
                                   worldRuntime), !chainFailed)) {
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
              /* bias - Manhattan distance to the unit: only sites closer than the bias add to the score */
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
        terrainFeatureEntry++;
        sitesRemainingOrX--;
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

