/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/placement.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/placement.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/placement. */

/* Address: 0x0053A110.
   Tests whether one more special site of this asset fits at a workspace-08 cell: the mode-7 placement query must
   report a nonzero count, the mode-4 query must fail, and the count rounded up to whole separation quanta must be
   at most 4; the result is then that of AiPlacement_ReserveSeparatedSpecialSiteChain. CF (true) means rejected.
*/
bool AiPlacement_ReserveAdditionalSpecialSite(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t placementCount;
  uint32_t headingOrQuantum;
  bool chainFailed;
  PlacementDispatchResult dispatchResult;

  knowledgeData = g_AiKnowledgeData;
  headingOrQuantum = (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles;
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (7,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,headingOrQuantum,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  placementCount = dispatchResult.value;
  if ((!dispatchResult.failed) && (placementCount != 0)) {
    dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                      (4,0,headingOrQuantum,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                       factionIndex,(UiRootNode *)worldRuntime);
    /* ceil(placementCount / quantum) < 5 */
    if ((dispatchResult.failed) &&
       (headingOrQuantum = (knowledgeData->parameters).specialSiteSeparationQuantumQ12,
       ((placementCount - 1) + headingOrQuantum) / headingOrQuantum < 5)) {
      chainFailed = AiPlacement_ReserveSeparatedSpecialSiteChain
                        (armyAssetId,workspaceRecord,factionIndex,worldRuntime);
      return chainFailed;
    }
  }
  return true;
}


/* Address: 0x0053AE60.
   Plans the special-site chain while the secondary workspace has no ARM_0050 entry: without ARM_0301 in the
   primary workspace it proposes that building; with it (and when the ARM_0050 registry lookup reports false)
   it proposes technology 11 (entry kind 2) until the faction has researched it, then ARM_0050. Each candidate
   gets the special-site weight; nothing is added when no site is available.
*/
void AiCandidatePlanning_AddSpecialSiteCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  bool hasEntry;
  SiteWeightResult weightResult;
  
  hasEntry = AiSecondaryWorkspace_HasEntryById(ARM_0050_UNIT_MDL0103);
  if (!hasEntry) {
    hasEntry = AiPrimaryWorkspace_HasEntryById(ARM_0301_BUILDING_MDL0318);
    if (hasEntry) {
      hasEntry = ArmyAssetRegistry_FindEnabledById(ARM_0050_UNIT_MDL0103);
      if (!hasEntry) {
        /* 0x800 is bit 11 of the technology mask, i.e. technology 11 proposed below */
        if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[0] & 0x800) == 0
           ) {
          weightResult = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          if (!weightResult.noSite) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(11,weightResult.score,2);
          }
        }
        else {
          weightResult = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          if (!weightResult.noSite) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(ARM_0050_UNIT_MDL0103,weightResult.score,0);
          }
        }
      }
    }
    else {
      weightResult = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
      if (!weightResult.noSite) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(ARM_0301_BUILDING_MDL0318,weightResult.score,0);
      }
    }
  }
  return;
}


/* Address: 0x00537B20.
   Adds a field cell to the general site list (workspace 05, at most 32 entries) unless an entry already lies
   closer than generalSiteMinimumAxisSeparationQ12 on both axes. The entry's score rewards closeness to the
   primary workspace and distance (capped) from workspaces 02 and 01, weighted by the ki.dat parameters at
   +0x84..+0xA0; unlike the flagged-site list below, these parameters are read from the right base.
*/
void AiSiteCandidate_AddGeneralCellIfSeparated(FieldGridCell *currentCell)

{
  Q12 cellWorldX;
  Q12 cellWorldY;
  uint32_t primaryCapOrWeight;
  uint32_t workspace02CapOrWeight;
  uint32_t secondaryDistanceCap;
  AiKnowledgeDataImage *knowledgeData;
  int deltaXOrPrimaryTerm;
  uint32_t workspace02Distance;
  uint32_t secondaryDistance;
  uint32_t remainingCount;
  int deltaY;
  AiScoredSiteWorkspaceEntry *siteEntry;
  
  remainingCount = g_AiWorkspace05Count;
  siteEntry = g_AiWorkspace05GeneralSites;
  while( true ) {
    if (remainingCount == 0) {
      cellWorldX = currentCell->worldX;
      cellWorldY = currentCell->worldY;
      if (g_AiWorkspace05Count < AI_WORKSPACE05_CAPACITY) {
        siteEntry->cellWorldXQ12 = cellWorldX;
        siteEntry->cellWorldYQ12 = cellWorldY;
        siteEntry->cell = currentCell;
        knowledgeData = g_AiKnowledgeData;
        primaryCapOrWeight = (g_AiKnowledgeData->parameters).generalSitePrimaryDistanceCapQ12;
        deltaXOrPrimaryTerm = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
        deltaXOrPrimaryTerm = primaryCapOrWeight - deltaXOrPrimaryTerm;
        if (deltaXOrPrimaryTerm < 0) {
          deltaXOrPrimaryTerm = 0;
        }
        primaryCapOrWeight = (knowledgeData->parameters).generalSitePrimaryDistanceCoefficient;
        workspace02CapOrWeight = (knowledgeData->parameters).generalSiteVisibleHostileDistanceCapQ12;
        workspace02Distance = AiHostileWorkspace_GetNearestVisibleHostileDistance(cellWorldY,cellWorldX);
        if ((int)workspace02CapOrWeight < (int)workspace02Distance) {
          workspace02Distance = workspace02CapOrWeight;
        }
        workspace02CapOrWeight = (knowledgeData->parameters).generalSiteVisibleHostileDistanceCoefficient;
        secondaryDistanceCap = (knowledgeData->parameters).generalSiteSecondaryDistanceCapQ12;
        secondaryDistance = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
        if ((int)secondaryDistanceCap < (int)secondaryDistance) {
          secondaryDistance = secondaryDistanceCap;
        }
        g_AiWorkspace05Count++;
        siteEntry->score =
             deltaXOrPrimaryTerm * primaryCapOrWeight + workspace02Distance * workspace02CapOrWeight +
             secondaryDistance * (knowledgeData->parameters).generalSiteSecondaryDistanceCoefficient;
      }
      return;
    }
    deltaXOrPrimaryTerm = siteEntry->cellWorldXQ12 - currentCell->worldX;
    if (deltaXOrPrimaryTerm < 0) {
      deltaXOrPrimaryTerm = -deltaXOrPrimaryTerm;
    }
    deltaY = siteEntry->cellWorldYQ12 - currentCell->worldY;
    if (deltaY < 0) {
      deltaY = -deltaY;
    }
    if ((deltaXOrPrimaryTerm < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12) &&
       (deltaY < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12)) break;
    siteEntry++;
    remainingCount--;
  }
  return;
}


/* Address: 0x00537C10.
   Adds a field cell to the flagged site list (workspace 06, at most 64 entries) unless an entry already lies
   closer than flaggedSiteMinimumAxisSeparationQ12 on both axes; the score is built like the general site score.
   Original bug: the caps and weights are read at +0xC4..+0xE0 from the new entry's address in the workspace
   buffer (EDI still holds it) instead of from g_AiKnowledgeData, so they are whatever lies further on in the
   buffer. Nothing reads workspace 06, so the list is filled but never used.
*/
void AiSiteCandidate_AddFlaggedCellIfSeparated(FieldGridCell *currentCell)

{
  Q12 cellWorldX;
  Q12 cellWorldY;
  int deltaXOrCapTerm;
  int workspace02Distance;
  uint32_t remainingCount;
  int workspace02CapOrScore;
  int deltaYOrDistanceOrWeight;
  uint8_t *siteEntryBytes;

  remainingCount = g_AiWorkspace06Count;
  siteEntryBytes = g_AiWorkspace06FlaggedSites;
  while( true ) {
    if (remainingCount == 0) {
      cellWorldX = currentCell->worldX;
      cellWorldY = currentCell->worldY;
      if (g_AiWorkspace06Count < AI_WORKSPACE06_CAPACITY) {
        ((AiScoredSiteWorkspaceEntry *)siteEntryBytes)->cellWorldXQ12 = cellWorldX;
        ((AiScoredSiteWorkspaceEntry *)siteEntryBytes)->cellWorldYQ12 = cellWorldY;
        ((AiScoredSiteWorkspaceEntry *)siteEntryBytes)->cell = currentCell;
        /* the wrong-base reads (see above): the ki.dat parameter layout applied to the new entry's address */
        deltaXOrCapTerm = ((AiKnowledgeParameters *)siteEntryBytes)->flaggedSitePrimaryDistanceCapQ12;
        deltaYOrDistanceOrWeight = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
        deltaXOrCapTerm = deltaXOrCapTerm - deltaYOrDistanceOrWeight;
        if (deltaXOrCapTerm < 0) {
          deltaXOrCapTerm = 0;
        }
        deltaYOrDistanceOrWeight = ((AiKnowledgeParameters *)siteEntryBytes)->flaggedSitePrimaryDistanceCoefficient;
        workspace02CapOrScore = ((AiKnowledgeParameters *)siteEntryBytes)->flaggedSiteVisibleHostileDistanceCapQ12;
        workspace02Distance = AiHostileWorkspace_GetNearestVisibleHostileDistance(cellWorldY,cellWorldX);
        if (workspace02CapOrScore < workspace02Distance) {
          workspace02Distance = workspace02CapOrScore;
        }
        workspace02CapOrScore =
             deltaXOrCapTerm * deltaYOrDistanceOrWeight + workspace02Distance *
                  (int)((AiKnowledgeParameters *)siteEntryBytes)->flaggedSiteVisibleHostileDistanceCoefficient;
        deltaXOrCapTerm = ((AiKnowledgeParameters *)siteEntryBytes)->flaggedSiteSecondaryDistanceCapQ12;
        deltaYOrDistanceOrWeight = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
        if (-1 < deltaXOrCapTerm - deltaYOrDistanceOrWeight) {
          workspace02CapOrScore =
               workspace02CapOrScore + (deltaXOrCapTerm - deltaYOrDistanceOrWeight) *
                    (int)((AiKnowledgeParameters *)siteEntryBytes)->flaggedSiteSecondaryDistanceCoefficient;
        }
        g_AiWorkspace06Count++;
        ((AiScoredSiteWorkspaceEntry *)siteEntryBytes)->score = workspace02CapOrScore;
      }
      return;
    }
    deltaXOrCapTerm = *(int *)siteEntryBytes - currentCell->worldX;
    if (deltaXOrCapTerm < 0) {
      deltaXOrCapTerm = -deltaXOrCapTerm;
    }
    deltaYOrDistanceOrWeight = *(int *)(siteEntryBytes + 4) - currentCell->worldY;
    if (deltaYOrDistanceOrWeight < 0) {
      deltaYOrDistanceOrWeight = -deltaYOrDistanceOrWeight;
    }
    if ((deltaXOrCapTerm < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12) &&
       (deltaYOrDistanceOrWeight < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12)) break;
    siteEntryBytes = siteEntryBytes + 0x10;
    remainingCount--;
  }
  return;
}


/* Address: 0x00537CF0.
   Adds a resource site to the terrain-feature list (workspace 08, at most 32 entries): ARM_0330 on a cell with
   xenite support, ARM_0332 otherwise. The cell is dropped when it lies within 2.0 of the 1:5 marker of any class-13
   structure in workspace 00, or when a structure of the same asset stands closer than
   terrainFeatureMinimumAxisSeparationQ12 on both axes. The priority is placementClearancePaddingQ12 minus the
   Manhattan distance to the nearest workspace-00 structure (at least 0). A same-asset entry within a third of the
   separation is replaced instead when the new cell has the higher priority. gridScratchRowStrideBytes is unused.
*/
void AiSiteCandidate_AddTerrainFeatureCellIfSeparated
          (FieldGridCell *terrainFeatureCell,uint32_t gridScratchRowStrideBytes)

{
  uint32_t remainingFeatureCount;
  int *runtimeSlot;
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t markerDistance;
  int countOrDeltaX;
  int entityDeltaX;
  uint32_t duplicateSeparation;
  int entityOrDeltaY;
  PckArmyAssetIdCatalog featureAssetId;
  AiStructureWorkspaceEntry *workspace00Entry;
  ModelLookupEntryResult markerLookup;
  ModelWorldPoint markerPoint;
  int nearestDistanceOrPriority;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  
  remainingFeatureCount = g_AiWorkspace08Count;
  featureAssetId = ARM_0330_BUILDING_MDL0303;
  duplicateSeparation = (g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12 * 0x55 >> 8; /* ~1/3 */
  countOrDeltaX = g_AiWorkspace00Count;
  workspace00Entry = g_AiWorkspace00Structures;
  if ((terrainFeatureCell->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
    featureAssetId = ARM_0332_BUILDING_MDL0302;
  }
  for (; terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites, countOrDeltaX != 0; countOrDeltaX = countOrDeltaX -
       1) {
    runtimeSlot = (int *)workspace00Entry->runtimeSlotAddressOrZero;
    /* runtimeSlot is a ModelRuntimeSlot: [0] the model definition, [1] the model node */
    if ((runtimeSlot != NULL) &&
       (modelNodeRuntime = (ModelRuntimeNode *)runtimeSlot[1],
       ((ModelRuntimeSlot *)runtimeSlot)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
       MODEL_RUNTIME_CLASS_13)) {
      markerLookup = ModelLookupTable_ContainsPackedKey
                         (1,5,(modelNodeRuntime->modelPayload).modelResource);
      if (!markerLookup.notFound) {
        markerPoint = ModelNodeRuntime_TransformLocalPointRegs(markerLookup.entry,modelNodeRuntime);
        markerDistance = FixedMath_Length2(markerPoint.yQ12 - terrainFeatureCell->worldY,
                                  markerPoint.xQ12 - terrainFeatureCell->worldX);
        if ((int)markerDistance < 0x2001) { /* within 2.0 (Q12) */
          return;
        }
      }
    }
    workspace00Entry++;
  }
  do {
    if (remainingFeatureCount == 0) {
      if (g_AiWorkspace08Count < AI_WORKSPACE08_CAPACITY) {
        terrainFeatureEntry->cell = terrainFeatureCell;
        terrainFeatureEntry->armyAssetId = featureAssetId;
        nearestDistanceOrPriority = 0x7fffffff;
        workspace00Entry = g_AiWorkspace00Structures;
        for (countOrDeltaX = g_AiWorkspace00Count; countOrDeltaX != 0; countOrDeltaX = countOrDeltaX - 1) {
          if (workspace00Entry->runtimeSlotAddressOrZero != 0) {
            entityOrDeltaY =
                 (int)((ModelRuntimeSlot *)workspace00Entry->runtimeSlotAddressOrZero)->rootModelNodeOrSavedOffset.modelNode;
            entityDeltaX =
                 ((ModelRuntimeNode *)entityOrDeltaY)->worldTransform.translation.x - terrainFeatureCell->worldX;
            if (entityDeltaX < 0) {
              entityDeltaX = -entityDeltaX;
            }
            entityOrDeltaY =
                 ((ModelRuntimeNode *)entityOrDeltaY)->worldTransform.translation.y - terrainFeatureCell->worldY;
            if (entityOrDeltaY < 0) {
              entityOrDeltaY = -entityOrDeltaY;
            }
            if (((featureAssetId == workspace00Entry->armyAssetId) &&
                (entityDeltaX < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12
                )) && (entityOrDeltaY < (int)(g_AiKnowledgeData->parameters).
                                    terrainFeatureMinimumAxisSeparationQ12)) {
              return;
            }
            if (entityDeltaX + entityOrDeltaY < nearestDistanceOrPriority) {
              nearestDistanceOrPriority = entityDeltaX + entityOrDeltaY;
            }
          }
          workspace00Entry++;
        }
        nearestDistanceOrPriority = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 -
             nearestDistanceOrPriority;
        if (nearestDistanceOrPriority < 0) {
          nearestDistanceOrPriority = 0;
        }
        g_AiWorkspace08Count++;
        terrainFeatureEntry->priority = nearestDistanceOrPriority;
      }
      return;
    }
    if (featureAssetId == terrainFeatureEntry->armyAssetId) {
      countOrDeltaX = terrainFeatureEntry->cell->worldX - terrainFeatureCell->worldX;
      if (countOrDeltaX < 0) {
        countOrDeltaX = -countOrDeltaX;
      }
      entityOrDeltaY = terrainFeatureEntry->cell->worldY - terrainFeatureCell->worldY;
      if (entityOrDeltaY < 0) {
        entityOrDeltaY = -entityOrDeltaY;
      }
      if ((countOrDeltaX < (int)duplicateSeparation) && (entityOrDeltaY < (int)duplicateSeparation)) {
        nearestDistanceOrPriority = 0x7fffffff;
        workspace00Entry = g_AiWorkspace00Structures;
        countOrDeltaX = g_AiWorkspace00Count;
        do {
          if (countOrDeltaX == 0) {
            nearestDistanceOrPriority = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 -
                 nearestDistanceOrPriority;
            if (nearestDistanceOrPriority < 0) {
              nearestDistanceOrPriority = 0;
            }
            if (nearestDistanceOrPriority <= terrainFeatureEntry->priority) {
              return;
            }
            terrainFeatureEntry->cell = terrainFeatureCell;
            terrainFeatureEntry->priority = nearestDistanceOrPriority;
            return;
          }
          if (workspace00Entry->runtimeSlotAddressOrZero != 0) {
            entityOrDeltaY =
                 (int)((ModelRuntimeSlot *)workspace00Entry->runtimeSlotAddressOrZero)->rootModelNodeOrSavedOffset.modelNode;
            entityDeltaX =
                 ((ModelRuntimeNode *)entityOrDeltaY)->worldTransform.translation.x - terrainFeatureCell->worldX;
            if (entityDeltaX < 0) {
              entityDeltaX = -entityDeltaX;
            }
            entityOrDeltaY =
                 ((ModelRuntimeNode *)entityOrDeltaY)->worldTransform.translation.y - terrainFeatureCell->worldY;
            if (entityOrDeltaY < 0) {
              entityOrDeltaY = -entityOrDeltaY;
            }
            if (((featureAssetId == workspace00Entry->armyAssetId) &&
                (entityDeltaX < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12
                )) && (entityOrDeltaY < (int)(g_AiKnowledgeData->parameters).
                                    terrainFeatureMinimumAxisSeparationQ12)) {
              return;
            }
            if (entityDeltaX + entityOrDeltaY < nearestDistanceOrPriority) {
              nearestDistanceOrPriority = entityDeltaX + entityOrDeltaY;
            }
          }
          workspace00Entry++;
          countOrDeltaX--;
        } while( true );
      }
    }
    remainingFeatureCount--;
    terrainFeatureEntry++;
  } while( true );
}


/* Address: 0x00539200.
   Runs the mode-0 placement query for the asset at a workspace cell (its position and heading) and returns the
   query's CF: true when the asset cannot be placed there.
*/
bool AiPlacement_TestWorkspaceRecordAtPoint(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,UiRootNode *inGameRoot)

{
  PlacementDispatchResult dispatchResult;
  
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (0,0,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,placementContext,
                     inGameRoot);
  return dispatchResult.failed;
}


/* Address: 0x0053A1B0.
   Runs the mode-4 placement query for the asset at a workspace cell (its position and heading) and returns
   the query's CF: true when the asset cannot be placed there in that mode.
*/
bool AiPlacement_TestMode4AtWorkspaceRecord(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  PlacementDispatchResult dispatchResult;
  
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (4,0,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  return dispatchResult.failed;
}


/* Address: 0x0053B570.
   Counts how many separation quanta of special sites the asset could use at a workspace cell: when the mode-3
   query reports a nonzero count and the mode-0 query fails, the count is rounded up to whole
   specialSiteSeparationQuantumQ12 units; otherwise the result is 0. CF is set only when both the mode-3 and the
   mode-0 query fail (EAX then holds the mode-0 error).
*/
StatusResult AiPlacement_QueryReachableSiteBucketCount(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t placementCount;
  uint32_t headingOrBucketCount;
  PlacementDispatchResult dispatchResult;
  StatusResult countResult;
  StatusResult failureResult;
  
  knowledgeData = g_AiKnowledgeData;
  headingOrBucketCount = (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles;
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,headingOrBucketCount,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  placementCount = dispatchResult.value;
  if (dispatchResult.failed) {
    dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                      (0,0,headingOrBucketCount,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                       factionIndex,(UiRootNode *)worldRuntime);
    if (dispatchResult.failed) {
      failureResult.valueOrError = dispatchResult.value;
      failureResult.failed = dispatchResult.failed;
      return failureResult;
    }
    headingOrBucketCount = 0;
  }
  else if ((placementCount != 0) &&
          (dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                             (0,0,headingOrBucketCount,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                              factionIndex,(UiRootNode *)worldRuntime), dispatchResult.failed)) {
    /* Round the placement count up to whole separation quanta. */
    headingOrBucketCount = (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
    headingOrBucketCount = ((placementCount - 1) + headingOrBucketCount) / headingOrBucketCount;
  }
  else {
    headingOrBucketCount = 0;
  }
  countResult.failed = false;
  countResult.valueOrError = headingOrBucketCount;
  return countResult;
}


/* Address: 0x0053ACD0.
   Tests a workspace-08 site cell for the asset: the mode-3 placement query must succeed with a count of at least
   one separation quantum (rounded up). More than 4 quanta accept the site outright; 1-4 quanta accept it only
   when AiPlacement_ReserveSeparatedSpecialSiteChain reports CF set. CF (true) means rejected.
*/
bool AiPlacement_ReserveMode3SiteCluster(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t quantumOrBucketCount;
  bool chainFailed;
  PlacementDispatchResult dispatchResult;

  knowledgeData = g_AiKnowledgeData;
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                     (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                     workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  /* ceil(placementCount / quantum) */
  if ((dispatchResult.failed) ||
     (quantumOrBucketCount = (knowledgeData->parameters).specialSiteSeparationQuantumQ12,
     quantumOrBucketCount = ((dispatchResult.value - 1) + quantumOrBucketCount) / quantumOrBucketCount,
          quantumOrBucketCount == 0)) {
    return true;
  }
  if ((quantumOrBucketCount < 5) &&
     (chainFailed = AiPlacement_ReserveSeparatedSpecialSiteChain
                        (armyAssetId,workspaceRecord,factionIndex,worldRuntime), !chainFailed)) {
    return true;
  }
  return false;
}


/* Address: 0x0053AD50.
   Finds the first workspace-08 resource site that passes AiPlacement_ReserveMode3SiteCluster and lies at least
   specialSiteMinimumWorkspaceDistanceQ12 from workspaces 03 and 02, and weighs it: the site's base weight
   (ARM_0330 or other) x3 / (2 * assigned structures of that asset + 6); for a non-ARM_0330 site it is further
   scaled by (2 * unpowered + supplied energy demand) / (tritiumCurrentQ4 << 8) when the faction has tritium.
   CF (noSite) is set when no site qualifies.
*/
SiteWeightResult AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *workspaceRecord;
  uint32_t baseWeight;
  int countOrTritium;
  uint32_t weight;
  AiTerrainFeatureWorkspaceEntry *featureEntry;
  bool clusterRejected;
  SiteWeightResult weightResult;
  AiKnowledgeDataImage *knowledgeData;

  knowledgeData = g_AiKnowledgeData;
  countOrTritium = g_AiWorkspace08Count;
  featureEntry = g_AiWorkspace08TerrainFeatureSites;
  while( true ) {
    if (countOrTritium == 0) {
      /* No site (CF set): EAX holds whatever the last check left there; callers read the score only with CF
         clear. */
      weightResult.noSite = true;
      weightResult.score = 0;
      return weightResult;
    }
    workspaceRecord = featureEntry->cell;
    clusterRejected = AiPlacement_ReserveMode3SiteCluster
                      (featureEntry->armyAssetId,workspaceRecord,factionIndex,worldRuntime);
    if ((!clusterRejected) &&
        ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <=
         (int)AiHostileWorkspace_GetNearestUnseenHostileDistance(workspaceRecord->worldY,workspaceRecord->worldX))
        && ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <=
            (int)AiHostileWorkspace_GetNearestVisibleHostileDistance
                   (workspaceRecord->worldY,workspaceRecord->worldX)))
    break;
    featureEntry++;
    countOrTritium--;
  }
  countOrTritium = AiPrimaryWorkspace_CountAssignedEntriesById(featureEntry->armyAssetId);
  baseWeight = (knowledgeData->parameters).specialSite14aBaseWeight;
  if (featureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
    baseWeight = (knowledgeData->parameters).specialSite14cBaseWeight;
  }
  weight = (baseWeight * 3) / (countOrTritium * 2 + 6U);
  if (featureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
    countOrTritium = g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 << 8;
    if (countOrTritium != 0) {
      weight = (uint32_t)(((int64_t)(int)weight *
                     (int64_t)
                     (int)(g_GameFactionRuntimeImage.records[factionIndex].unpoweredEnergyDemandQ4 *
                           2 + g_GameFactionRuntimeImage.records[factionIndex].
                               suppliedEnergyDemandQ4)) / (int64_t)countOrTritium);
    }
  }
  weightResult.noSite = false;
  weightResult.score = weight;
  return weightResult;
}


/* Address: 0x00539330.
   Returns the position of the workspace-09 cell nearest (Manhattan distance) to the reference point at which
   the mode-1 placement query accepts the asset. CF (notFound) is set when no such cell exists.
   Note the argument order: Y first, then X, like ArmyPlacement_DispatchAssetAtFieldPoint.
*/
AiAnchorResult AiPlacement_FindNearestPlaceableBaseSite
          (Q12 referenceWorldYQ12,Q12 referenceWorldXQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  int deltaX;
  int remainingCount;
  uint32_t candidateDistance;
  int deltaY;
  uint32_t bestDistance;
  FieldGridCell **gridCellCursor;
  PlacementDispatchResult dispatchResult;
  AiAnchorResult anchorResult;
  FieldGridCell *bestCell;
  FieldGridCell *candidateCell;

  /* Not found (CF set): ECX is the exhausted loop counter (0) and EDX the last candidate distance (or the
     caller's EDX for an empty workspace); no caller reads them when CF is set. */
  candidateDistance = 0;
  if (g_AiWorkspace09Count != 0) {
    bestDistance = 0x7fffffff;
    remainingCount = g_AiWorkspace09Count;
    gridCellCursor = g_AiWorkspace09Cells;
    do {
      candidateCell = *gridCellCursor;
      deltaX = referenceWorldXQ12 - candidateCell->worldX;
      if (deltaX < 0) {
        deltaX = -deltaX;
      }
      deltaY = referenceWorldYQ12 - candidateCell->worldY;
      if (deltaY < 0) {
        deltaY = -deltaY;
      }
      candidateDistance = deltaY + deltaX;
      if ((int)candidateDistance < (int)bestDistance) {
        dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                          (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,candidateCell->worldY,
                           candidateCell->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
        if (!dispatchResult.failed) {
          bestDistance = candidateDistance;
          bestCell = candidateCell;
        }
      }
      gridCellCursor++;
      remainingCount--;
    } while (remainingCount != 0);
    if ((int)bestDistance < 0x7fffffff) {
      anchorResult.worldXQ12 = bestCell->worldX;
      anchorResult.worldYQ12 = bestCell->worldY;
      anchorResult.notFound = false;
      return anchorResult;
    }
  }
  anchorResult.notFound = true;
  anchorResult.worldXQ12 = 0;
  anchorResult.worldYQ12 = (Q12)candidateDistance;
  return anchorResult;
}


/* Address: 0x00539EF0.
   Probes the ARM_0333 building positions around a workspace cell: up to four times it takes the nearest free
   workspace-09 anchor (AiPlacement_FindNearestPlaceableBaseSite), creates a temporary ARM_0333 instance
   there (so the next search finds the next anchor) and checks its Manhattan distance to the cell. CF is clear
   as soon as one of them lies closer than specialSiteSeparationQuantumQ12, and set when all are at least that
   far or an anchor/instance is missing. Every temporary instance is destroyed again before returning; the
   armyAssetId argument is not used.
*/
bool AiPlacement_ReserveSeparatedSpecialSiteChain(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  Q12 firstAnchorXQ12;
  int deltaX;
  Q12 firstAnchorYQ12;
  int deltaY;
  ArmyRuntimeCreateResult firstInstance;
  ArmyRuntimeCreateResult secondInstance;
  ArmyRuntimeCreateResult thirdInstance;
  ArmyRuntimeCreateResult fourthInstance;
  AiAnchorResult anchor;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  anchor = AiPlacement_FindNearestPlaceableBaseSite
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  firstAnchorYQ12 = anchor.worldYQ12;
  firstAnchorXQ12 = anchor.worldXQ12;
  if (anchor.notFound) {
    return true;
  }
  /* Y before X, as at every ArmyRuntime_CreateInstanceFromAsset call site */
  firstInstance = ArmyRuntime_CreateInstanceFromAsset
                    (1,0,firstAnchorYQ12,firstAnchorXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,worldRuntime);
  if (firstInstance.failed) {
    return true;
  }
  deltaX = firstAnchorXQ12 - workspaceRecord->worldX;
  if (deltaX < 0) {
    deltaX = -deltaX;
  }
  deltaY = firstAnchorYQ12 - workspaceRecord->worldY;
  if (deltaY < 0) {
    deltaY = -deltaY;
  }
  if ((uint32_t)(deltaX + deltaY) <
      (knowledgeData->parameters).specialSiteSeparationQuantumQ12) goto destroy_first_and_succeed;
  anchor = AiPlacement_FindNearestPlaceableBaseSite
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  if (anchor.notFound) goto destroy_first_and_fail;
  secondInstance = ArmyRuntime_CreateInstanceFromAsset
                    (1,0,anchor.worldYQ12,anchor.worldXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                     worldRuntime);
  if (secondInstance.failed) goto destroy_first_and_fail;
  deltaX = anchor.worldXQ12 - workspaceRecord->worldX;
  if (deltaX < 0) {
    deltaX = -deltaX;
  }
  deltaY = anchor.worldYQ12 - workspaceRecord->worldY;
  if (deltaY < 0) {
    deltaY = -deltaY;
  }
  if ((knowledgeData->parameters).specialSiteSeparationQuantumQ12 <= (uint32_t)(deltaX + deltaY)) {
    anchor = AiPlacement_FindNearestPlaceableBaseSite
                      (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                       factionIndex,worldRuntime);
    if (!anchor.notFound) {
      thirdInstance = ArmyRuntime_CreateInstanceFromAsset
                        (1,0,anchor.worldYQ12,anchor.worldXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                         worldRuntime);
      if (!thirdInstance.failed) {
        deltaX = anchor.worldXQ12 - workspaceRecord->worldX;
        if (deltaX < 0) {
          deltaX = -deltaX;
        }
        deltaY = anchor.worldYQ12 - workspaceRecord->worldY;
        if (deltaY < 0) {
          deltaY = -deltaY;
        }
        if ((uint32_t)(deltaX + deltaY) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12) {
destroy_third_second_first_and_succeed:
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)thirdInstance.armyRuntimeOrError);
          goto destroy_second_first_and_succeed;
        }
        anchor = AiPlacement_FindNearestPlaceableBaseSite
                          (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307
                           ,factionIndex,worldRuntime);
        if (!anchor.notFound) {
          fourthInstance = ArmyRuntime_CreateInstanceFromAsset
                            (1,0,anchor.worldYQ12,anchor.worldXQ12,factionIndex,
                             ARM_0333_BUILDING_MDL0307,worldRuntime);
          if (!fourthInstance.failed) {
            deltaX = anchor.worldXQ12 - workspaceRecord->worldX;
            if (deltaX < 0) {
              deltaX = -deltaX;
            }
            deltaY = anchor.worldYQ12 - workspaceRecord->worldY;
            if (deltaY < 0) {
              deltaY = -deltaY;
            }
            if ((uint32_t)(deltaX + deltaY) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12)
            {
              ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,
                                                      (GameEntityRuntime *)fourthInstance.armyRuntimeOrError);
              goto destroy_third_second_first_and_succeed;
            }
            ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,
                                                    (GameEntityRuntime *)fourthInstance.armyRuntimeOrError);
          }
        }
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)thirdInstance.armyRuntimeOrError);
      }
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)secondInstance.armyRuntimeOrError);
destroy_first_and_fail:
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)firstInstance.armyRuntimeOrError);
    return true;
  }
destroy_second_first_and_succeed:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)secondInstance.armyRuntimeOrError);
destroy_first_and_succeed:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)firstInstance.armyRuntimeOrError);
  return false;
}

