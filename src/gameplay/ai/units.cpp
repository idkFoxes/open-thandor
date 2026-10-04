/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/units.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/units.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_AiCollectedEntityCount = 0;

/* Implementation ownership: gameplay/ai/units. */

/* Per AI tick for the faction's own units (workspace 01): clears the collected-army list, lets busy units
   (ARMY_MOVEMENT_ACTIVE / _ROUTE_POINT_REACHED in the movement state, the entity's common.commandFlags) wait
   for their behaviour cooldown (common.aiCommandCooldownTicks), skips units with ARMY_MOVEMENT_LOCKED, and
   dispatches the rest by model class: class 18 to UpdateSpecialClass12Entity, the ground, tracked, walker, water
   and glider classes (1/2/3/19/17) to SelectBestAnchorAction. The class is the runtimeClassId of the model's
   definition.
*/
void AiUnitBehavior_UpdateOwnUnits(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int workspaceEntriesRemaining;
  AiRuntimeWorkspaceEntry *workspaceEntryCursor;
  MdlDefinitionSemanticPrefix *modelDefinition;
  int *behaviorCooldownTicks;
  ModelRuntimeSlot *unitModelRuntime;
  GameEntityRuntime *slotEntityRuntime;

  g_AiCollectedEntityCount = 0;
  workspaceEntriesRemaining = g_AiWorkspace01Count;
  workspaceEntryCursor = g_AiWorkspace01Units;
  for (; workspaceEntriesRemaining != 0; workspaceEntriesRemaining--, workspaceEntryCursor++) {
    unitModelRuntime = workspaceEntryCursor->modelRuntime;
    if (unitModelRuntime == nullptr) continue;
    slotEntityRuntime = unitModelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime;
    if ((slotEntityRuntime->common.commandFlags &
         (ARMY_MOVEMENT_ACTIVE | ARMY_MOVEMENT_LOCKED | ARMY_MOVEMENT_ROUTE_POINT_REACHED)) != 0) {
      if ((slotEntityRuntime->common.commandFlags & ARMY_MOVEMENT_LOCKED) != 0) continue;
      /* Busy entities only get a behavior update when their cooldown runs out (or was already negative,
         which the increment below undoes). */
      behaviorCooldownTicks = &slotEntityRuntime->common.aiCommandCooldownTicks;
      (*behaviorCooldownTicks)--;
      if (*behaviorCooldownTicks != 0) {
        if (*behaviorCooldownTicks > 0) continue;
        (*behaviorCooldownTicks)++;
      }
    }
    modelDefinition =
         (MdlDefinitionSemanticPrefix *)unitModelRuntime->definitionOrSavedId.runtimeDefinition;
    if (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_18) {
      AiUnitBehavior_UpdatePioneerVehicle
                (modelDefinition,unitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime,factionIndex,
                 worldRuntime);
    }
    else if (modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_01_GROUND ||
             modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_02_TRACKED ||
             modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_03_ARTICULATED_WALKER ||
             modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_19_WATER_SURFACE ||
             modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER) {
      AiUnitBehavior_SelectBestAnchorAction
                (modelDefinition,unitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime,factionIndex,
                 worldRuntime);
    }
  }
}


/* Decides what an idle military unit (ground, tracked, walker,
                                       glider or water class) does next. Three scorers run in turn, each
   given the best score so far and returning it unchanged unless it found better: a general site (workspace 05)
   -> move there (kind 1), the faction anchor -> move there (kind 2), a secondary-workspace target (kind 3).
   With kind 3 or no winner the unit is collected for the group assignment of
   AiUnitGroup_AssignCollectedEntitiesToBestTarget.
*/
void AiUnitBehavior_SelectBestAnchorAction
          (MdlDefinitionSemanticPrefix *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int workspaceScore;
  AiCandidateScore32 factionAnchorScore;
  uint32_t selectedAnchorActionKind;
  int currentBestScore;
  AiScoredSiteWorkspaceEntry *selectedWorkspaceEntry;
  AiGeneralSiteDistanceSelection workspaceSelection;
  AiSecondaryWorkspaceDistanceSelection secondarySelection;
  
  workspaceSelection = AiUnitBehavior_ComputeGeneralSiteDistanceScore(0,modelDefinition,armyRuntimeSlot);
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
}


/* Scores the general sites (workspace 05) for a unit and returns the best one if it beats currentBestScore:
   score = ((max(0, bias - Manhattan distance) * scale + site score) >> 12) * the unit's aiSiteScoreWeight,
   so nearer and richer sites score higher. Returns currentBestScore and a NULL entry when none beats it.
   modelDefinition is not used.
*/
AiGeneralSiteDistanceSelection AiUnitBehavior_ComputeGeneralSiteDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int recordsRemaining;
  Q12 negAbsDeltaXQ12;
  Q12 negAbsDeltaYQ12;
  int distanceTerm;
  int siteScore;
  AiScoredSiteWorkspaceEntry *bestEntry;
  AiScoredSiteWorkspaceEntry *workspaceRecordCursor;
  AiGeneralSiteDistanceSelection selection;

  bestEntry = nullptr;
  workspaceRecordCursor = g_AiWorkspace05GeneralSites;
  for (recordsRemaining = g_AiWorkspace05Count; recordsRemaining != 0; recordsRemaining--) {
    negAbsDeltaXQ12 = (armyRuntimeSlot->articulatedContact).fallbackPosition0Q12 -
            workspaceRecordCursor->cellWorldXQ12;
    if (negAbsDeltaXQ12 >= 0) {
      negAbsDeltaXQ12 = -negAbsDeltaXQ12;
    }
    negAbsDeltaYQ12 = (armyRuntimeSlot->articulatedContact).fallbackPosition1Q12 -
                   workspaceRecordCursor->cellWorldYQ12;
    if (negAbsDeltaYQ12 >= 0) {
      negAbsDeltaYQ12 = -negAbsDeltaYQ12;
    }
    /* bias - Manhattan distance, clamped at 0 */
    distanceTerm = negAbsDeltaXQ12 + negAbsDeltaYQ12 + (g_AiKnowledgeData->parameters).workspace05DistanceBiasQ12;
    if (distanceTerm < 0) {
      distanceTerm = 0;
    }
    siteScore = ((distanceTerm * (g_AiKnowledgeData->parameters).workspace05DistanceScaleQ12 +
             workspaceRecordCursor->score) >> Q12_SHIFT) * armyRuntimeSlot->aiSiteScoreWeight;
    if (currentBestScore < siteScore) {
      bestEntry = workspaceRecordCursor;
      currentBestScore = siteScore;
    }
    workspaceRecordCursor++;
  }
  selection.selectedEntry = bestEntry;
  selection.score = currentBestScore;
  return selection;
}


/* Scores one faction anchor point (anchorYQ12/anchorXQ12 as named in the faction record) for a unit and returns
   the higher of that score and currentBestScore. */
static AiCandidateScore32 AiUnitBehavior_ScoreFactionAnchorPoint
          (const ModelRuntimeNode *modelNode,GraphicsWorldCoordinateQ12 anchorYQ12,
          GraphicsWorldCoordinateQ12 anchorXQ12,const ArmyRuntimeSlot *armyRuntimeSlot,
          AiCandidateScore32 currentBestScore)

{
  int negAbsDeltaFirstQ12;
  Q12 negAbsDeltaSecondQ12;
  int distanceTerm;
  int anchorScore;

  /* Both anchor coordinates are compared with translation.x: the original reads that field twice, so this
     "distance" ignores the unit's Y. */
  negAbsDeltaFirstQ12 = (modelNode->worldTransform).translation.x - anchorYQ12;
  if (negAbsDeltaFirstQ12 >= 0) {
    negAbsDeltaFirstQ12 = -negAbsDeltaFirstQ12;
  }
  negAbsDeltaSecondQ12 = (modelNode->worldTransform).translation.x - anchorXQ12;
  if (negAbsDeltaSecondQ12 >= 0) {
    negAbsDeltaSecondQ12 = -negAbsDeltaSecondQ12;
  }
  /* bias - distance, clamped at 0 */
  distanceTerm = negAbsDeltaFirstQ12 + negAbsDeltaSecondQ12 +
          (g_AiKnowledgeData->parameters).factionAnchorDistanceBiasQ12;
  if (distanceTerm < 0) {
    distanceTerm = 0;
  }
  anchorScore = (distanceTerm * (g_AiKnowledgeData->parameters).factionAnchorDistanceScaleQ12 >> Q12_SHIFT) *
          armyRuntimeSlot->aiFactionAnchorScoreWeight;
  if (currentBestScore < anchorScore) {
    currentBestScore = anchorScore;
  }
  return currentBestScore;
}


/* Scores the faction's primary and secondary anchor points (each only while its cooldown runs) for a unit like
   the general sites: ((max(0, bias - distance) * scale) >> 12) * aiFactionAnchorScoreWeight, and returns the
   highest of these and currentBestScore. modelDefinition is not used.
*/
AiCandidateScore32 AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          MdlDefinitionSemanticPrefix *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot)

{
  ModelRuntimeNode *modelNode;

  modelNode = armyRuntimeSlot->modelNodeRuntime;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
    currentBestScore = AiUnitBehavior_ScoreFactionAnchorPoint
              (modelNode,g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12,
               g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12,armyRuntimeSlot,
               currentBestScore);
  }
  if (g_GameFactionRuntimeImage.records[factionIndex].anchorCooldown0 != 0) {
    currentBestScore = AiUnitBehavior_ScoreFactionAnchorPoint
              (modelNode,g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12,
               g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12,armyRuntimeSlot,
               currentBestScore);
  }
  return currentBestScore;
}


/* Scores the target entries of workspace 07 for a unit, or those of workspace 03 at three quarters of the score
   when workspace 07 is empty: ((max(0, bias - Manhattan distance) * scale) >> 12) * aiSecondaryWorkspaceScoreWeight.
   Returns the best entry if it beats currentBestScore, else currentBestScore and NULL. modelDefinition is not
   used.
*/
AiSecondaryWorkspaceDistanceSelection AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot)

{
  int recordsRemaining;
  int negAbsDeltaXQ12;
  Q12 negAbsDeltaYQ12;
  int distanceTerm;
  int targetScore;
  AiTargetWorkspaceEntry *bestEntry;
  AiTargetWorkspaceEntry *workspaceRecordCursor;
  AiSecondaryWorkspaceDistanceSelection selection;

  bestEntry = nullptr;
  recordsRemaining = g_AiWorkspace07Count;
  workspaceRecordCursor = g_AiWorkspace07Targets;
  if (g_AiWorkspace07Count == 0) {
    /* Original quirk: workspace 03 holds 8-byte AiRuntimeWorkspaceEntry records but the original walks it with
       the 16-byte workspace-07 stride (0x0053B3C0 `add edi,0x10`) and reads its (+0, +4) as world X/Y, i.e. the
       (modelRuntime, armyAssetId) of ws03[2i]: the second half of the walk reads stale entries of an earlier
       rebuild, for count > 256 past the 0x1000-byte buffer. Kept: it feeds the AI decisions and the AI hash. */
    recordsRemaining = g_AiWorkspace03Count;
    workspaceRecordCursor = (AiTargetWorkspaceEntry *)g_AiWorkspace03UnseenHostiles;
  }
  for (; recordsRemaining != 0; recordsRemaining--) {
    negAbsDeltaXQ12 = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.x -
            (int)workspaceRecordCursor->worldXQ12;
    if (negAbsDeltaXQ12 >= 0) {
      negAbsDeltaXQ12 = -negAbsDeltaXQ12;
    }
    negAbsDeltaYQ12 = (armyRuntimeSlot->modelNodeRuntime->worldTransform).translation.y -
                   workspaceRecordCursor->worldYQ12;
    if (negAbsDeltaYQ12 >= 0) {
      negAbsDeltaYQ12 = -negAbsDeltaYQ12;
    }
    /* bias - Manhattan distance, clamped at 0 */
    distanceTerm = negAbsDeltaXQ12 + negAbsDeltaYQ12 +
         (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceBiasQ12;
    if (distanceTerm < 0) {
      distanceTerm = 0;
    }
    targetScore = (distanceTerm * (g_AiKnowledgeData->parameters).secondaryWorkspaceDistanceScaleQ12 >> Q12_SHIFT) *
            armyRuntimeSlot->aiSecondaryWorkspaceScoreWeight;
    if (g_AiWorkspace07Count == 0) {
      targetScore = targetScore * 3 >> 2;
    }
    if (currentBestScore < targetScore) {
      bestEntry = workspaceRecordCursor;
      currentBestScore = targetScore;
    }
    workspaceRecordCursor++;
  }
  selection.selectedEntry = bestEntry;
  selection.score = currentBestScore;
  return selection;
}


/* Sends the unit to a general site (a workspace-05 AiScoredSiteWorkspaceEntry: X, Y, score): marks it as
   AI-commanded and no longer group-assigned, zeroes the site's score (the bonus
   AiUnitBehavior_ComputeGeneralSiteDistanceScore adds for it, so the next unit is less drawn there) and queues
   the move. worldRuntimeContext is not used.
*/
void AiUnitCommand_AssignWorkspacePoint(uint32_t *workspacePoint,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext)

{
  armyRuntime->aiUnitState = AI_UNIT_COMMANDED_STATE;
  armyRuntime->aiUnitFlags = armyRuntime->aiUnitFlags & ~AI_UNIT_STATE94_GROUP_ASSIGNED;
  workspacePoint[2] = 0; /* AiScoredSiteWorkspaceEntry.score */
  ArmyRuntime_StartRoutedMoveCommand
            (workspacePoint[1],*workspacePoint,(ArmyMovementRuntime *)armyRuntime);
}


/* Sends the unit to the faction's anchor point: the primary anchor while its cooldown runs, otherwise the
   secondary one. Marks the unit as AI-commanded and no longer group-assigned, then queues the move.
   worldRuntimeContext is not used.
*/
void AiUnitCommand_AssignFactionAnchorPoint(FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext)

{
  GraphicsWorldCoordinateQ12 targetWorldX;
  GraphicsWorldCoordinateQ12 targetWorldY;
  
  /* The field ...AnchorYQ12 goes to the targetWorldX parameter and ...AnchorXQ12 to targetWorldY,
     so the faction record's anchor X/Y field names are probably swapped. */
  targetWorldX = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12;
  targetWorldY = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12;
  if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) {
    targetWorldX = g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorYQ12;
    targetWorldY = g_GameFactionRuntimeImage.records[factionIndex].secondaryAnchorXQ12;
  }
  armyRuntime->aiUnitState = AI_UNIT_COMMANDED_STATE;
  armyRuntime->aiUnitFlags = armyRuntime->aiUnitFlags & ~AI_UNIT_STATE94_GROUP_ASSIGNED;
  ArmyRuntime_StartRoutedMoveCommand
            (targetWorldY,targetWorldX,(ArmyMovementRuntime *)armyRuntime);
}


/* Collects an idle unit that is not yet group-assigned into workspace 14 (at most 64 units), from which
   AiUnitGroup_AssignCollectedEntitiesToBestTarget later sends them together to one target.
   worldRuntimeContext is not used.
*/
void AiUnitBehavior_CollectUnassignedEntity(ArmyRuntimeSlot *armyRuntimeSlot,WorldRuntimeContext *worldRuntimeContext)

{
  if ((g_AiCollectedEntityCount < AI_WORKSPACE14_CAPACITY) &&
      ((armyRuntimeSlot->aiUnitFlags & AI_UNIT_STATE94_GROUP_ASSIGNED) == 0)) {
    g_AiWorkspace14CollectedArmies[g_AiCollectedEntityCount] = armyRuntimeSlot;
    g_AiCollectedEntityCount++;
  }
}


/* Raw score of a workspace-08 resource site for the pioneer vehicle, before the division by the asset's assigned
   structures: priority, nearness to visible hostiles (workspace 02), shortfall against the secondary workspace,
   nearness to the unit, x2 for ARM_0330. */
static int AiUnitBehavior_ScorePioneerSite
          (const AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry,const ArmyRuntimeSlot *armyRuntime,
          const AiKnowledgeDataImage *knowledgeData)

{
  FieldGridCell *cell;
  int siteScore;
  int hostileDistance;
  int secondaryDistanceTerm;
  int unitDistanceTerm;
  int negAbsDeltaY;

  cell = terrainFeatureEntry->cell;
  siteScore = terrainFeatureEntry->priority *
          (knowledgeData->parameters).specialClass12Workspace08Field0cCoefficient;
  if ((knowledgeData->parameters).specialClass12Workspace02NearDistanceCoefficient != 0) {
    hostileDistance = AiHostileWorkspace_GetNearestVisibleHostileDistance(cell->worldY,cell->worldX);
    if ((int)(hostileDistance - (knowledgeData->parameters).specialClass12Workspace02NearDistanceThresholdQ12)
        < 0) {
      siteScore = siteScore + hostileDistance *
              (knowledgeData->parameters).specialClass12Workspace02NearDistanceCoefficient;
    }
  }
  if ((knowledgeData->parameters).specialClass12SecondaryWorkspaceShortfallCoefficient != 0) {
    secondaryDistanceTerm = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(cell->worldY,cell->worldX);
    secondaryDistanceTerm = secondaryDistanceTerm -
            (knowledgeData->parameters).specialClass12SecondaryWorkspaceDistanceThresholdQ12;
    if (secondaryDistanceTerm < 0) {
      siteScore = siteScore - secondaryDistanceTerm *
              (knowledgeData->parameters).specialClass12SecondaryWorkspaceShortfallCoefficient;
    }
  }
  if ((knowledgeData->parameters).specialClass12EntityDistanceCoefficient != 0) {
    /* bias - Manhattan distance to the unit: only sites closer than the bias add to the score */
    unitDistanceTerm = (armyRuntime->modelNodeRuntime->worldTransform).translation.x - cell->worldX;
    if (unitDistanceTerm >= 0) {
      unitDistanceTerm = -unitDistanceTerm;
    }
    negAbsDeltaY = (armyRuntime->modelNodeRuntime->worldTransform).translation.y - cell->worldY;
    if (negAbsDeltaY >= 0) {
      negAbsDeltaY = -negAbsDeltaY;
    }
    unitDistanceTerm = unitDistanceTerm + negAbsDeltaY +
            (knowledgeData->parameters).specialClass12EntityDistanceBiasQ12;
    if (unitDistanceTerm >= 0) {
      siteScore = siteScore + unitDistanceTerm *
              (knowledgeData->parameters).specialClass12EntityDistanceCoefficient;
    }
  }
  if (terrainFeatureEntry->armyAssetId == ARM_0330_BUILDING_MDL0303) {
    siteScore = siteScore * 2;
  }
  return siteScore;
}


/* AI behaviour of a runtime-class-18 unit (the pioneer vehicle, which turns into a building at a resource
   site): drives it to the best free resource site of workspace 08. Called from AiUnitBehavior_UpdateOwnUnits and, while
   movement flag 0x100 is set, from its movement update. Once it has arrived (flag clear): with as many
   workspace-00 as workspace-04 entries it moves on 0x2D05 along its heading; otherwise it drives to the best
   resource site of workspace 08 that is far enough from workspaces 03/02, scored by priority, distances to
   workspaces 02/01 and to the unit, x2 for ARM_0330, then x3 / (assigned structures of that asset + 3). A site
   whose bucket query (AiPlacement_QueryReachableSiteBucketCount) fails is skipped; one with 0 buckets, or 1..4
   buckets and a successful (false) AiPlacement_ReserveSeparatedSpecialSiteChain, ends the update without a move.
   Not arrived or flag set: sets the flag and resets the movement once within 0x1B03 of its fallback position
   on both axes.
*/
void AiUnitBehavior_UpdatePioneerVehicle
          (MdlDefinitionSemanticPrefix *modelDefinition,ArmyRuntimeSlot *armyRuntime,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t workspace00Count;
  uint32_t workspace04Count;
  uint32_t sitesRemaining;
  int unseenHostileDistance;
  int visibleHostileDistance;
  int siteScore;
  int assignedEntryCount;
  uint32_t weightedScore;
  uint32_t bestScore;
  int unitWorldX;
  int unitWorldY;
  int absDeltaX;
  int absDeltaY;
  FieldGridCell *siteCell;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  Bool8 chainFailed;
  FixedSinCos headingOffset;
  uint32_t bucketCount;
  Q12 steerWorldXQ12; /* unused here */
  Q12 steerWorldYQ12; /* unused here */
  FieldGridCell *bestCell;
  ModelRuntimeNode *modelNode;

  workspace04Count = g_AiWorkspace04Count;
  workspace00Count = g_AiWorkspace00Count;
  if (((armyRuntime->movementStateFlags & ARMY_MOVEMENT_SPECIAL_BEHAVIOR) == 0) &&
     (ArmyRuntime_UpdateMovementAndWaypoints
        (worldRuntime,(ArmyMovementRuntime *)armyRuntime,&steerWorldXQ12,&steerWorldYQ12))) {
    if (workspace00Count == workspace04Count) {
      modelNode = armyRuntime->modelNodeRuntime;
      headingOffset = FixedMath_SinCosScaled((modelNode->modelPayload).worldRotationAngle2,5 * FIELD_GRID_WORLD_COLUMN_STEP_X);
      unitWorldX = (modelNode->worldTransform).translation.x;
      unitWorldY = (modelNode->worldTransform).translation.y;
      armyRuntime->aiUnitState = 8;
      ArmyRuntime_StartRoutedMoveCommand
                (headingOffset.sinValue + unitWorldY,headingOffset.cosValue + unitWorldX,
                 (ArmyMovementRuntime *)armyRuntime);
    }
    else if (((armyRuntime->movementStateFlags & ARMY_MOVEMENT_SPECIAL_BEHAVIOR) == 0) && (g_AiWorkspace08Count != 0)) {
      bestScore = 0;
      terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
      for (sitesRemaining = g_AiWorkspace08Count; sitesRemaining != 0; sitesRemaining--, terrainFeatureEntry++) {
        knowledgeData = g_AiKnowledgeData;
        siteCell = terrainFeatureEntry->cell;
        unseenHostileDistance = AiHostileWorkspace_GetNearestUnseenHostileDistance
                          (siteCell->worldY,siteCell->worldX);
        if ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 > unseenHostileDistance) {
          continue;
        }
        visibleHostileDistance = AiHostileWorkspace_GetNearestVisibleHostileDistance
                          (siteCell->worldY,siteCell->worldX);
        if ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 > visibleHostileDistance) {
          continue;
        }
        if (!AiPlacement_QueryReachableSiteBucketCount
              (terrainFeatureEntry->armyAssetId,siteCell,factionIndex,worldRuntime,&bucketCount)) {
          continue;
        }
        if (bucketCount == 0) {
          return;
        }
        if (bucketCount < 5) {
          chainFailed = AiPlacement_ReserveSeparatedSpecialSiteChain
                            (terrainFeatureEntry->armyAssetId,siteCell,factionIndex,worldRuntime);
          if (!chainFailed) {
            return;
          }
        }
        siteScore = AiUnitBehavior_ScorePioneerSite(terrainFeatureEntry,armyRuntime,knowledgeData);
        assignedEntryCount = AiPrimaryWorkspace_CountAssignedEntriesById(terrainFeatureEntry->armyAssetId);
        weightedScore = (uint32_t)(siteScore * 3) / (assignedEntryCount + 3U);
        if ((int)bestScore < (int)weightedScore) {
          bestScore = weightedScore;
          bestCell = siteCell;
        }
      }
      if (bestScore != 0) {
        armyRuntime->aiUnitState = 8;
        ArmyRuntime_StartRoutedMoveCommand
                  (bestCell->worldY,bestCell->worldX,(ArmyMovementRuntime *)armyRuntime);
      }
    }
  }
  else {
    absDeltaX = (armyRuntime->articulatedContact).fallbackPosition0Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.x;
    if (absDeltaX < 0) {
      absDeltaX = -absDeltaX;
    }
    absDeltaY = (armyRuntime->articulatedContact).fallbackPosition1Q12 -
            (armyRuntime->modelNodeRuntime->worldTransform).translation.y;
    if (absDeltaY < 0) {
      absDeltaY = -absDeltaY;
    }
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags | ARMY_MOVEMENT_SPECIAL_BEHAVIOR;
    if ((absDeltaX < 3 * FIELD_GRID_WORLD_COLUMN_STEP_X) && (absDeltaY < 3 * FIELD_GRID_WORLD_COLUMN_STEP_X)) {
      ArmyRuntime_ResetMovementStateFromModel(armyRuntime);
    }
  }
}

