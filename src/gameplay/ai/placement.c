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
  uint32_t heading;
  uint32_t quantum;

  knowledgeData = g_AiKnowledgeData;
  heading = (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles;
  if (!ArmyPlacement_CanPlaceAssetAtFieldPoint
                (7,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,heading,
                 workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                 (UiRootNode *)worldRuntime,&placementCount)) {
    return true;
  }
  if (placementCount == 0) {
    return true;
  }
  if (ArmyPlacement_CanPlaceAssetAtFieldPoint
                (4,0,heading,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                 factionIndex,(UiRootNode *)worldRuntime,NULL)) {
    return true;
  }
  /* ceil(placementCount / quantum) must stay below 5 */
  quantum = (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
  if (((placementCount - 1) + quantum) / quantum >= 5) {
    return true;
  }
  return AiPlacement_ReserveSeparatedSpecialSiteChain(armyAssetId,workspaceRecord,factionIndex,worldRuntime);
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
  uint32_t siteWeight;

  hasEntry = AiSecondaryWorkspace_HasEntryById(ARM_0050_UNIT_MDL0103);
  if (!hasEntry) {
    hasEntry = AiPrimaryWorkspace_HasEntryById(ARM_0301_BUILDING_MDL0318);
    if (hasEntry) {
      hasEntry = ArmyAssetRegistry_FindEnabledById(ARM_0050_UNIT_MDL0103);
      if (!hasEntry) {
        /* bit 11 of the technology mask: technology 11, proposed below */
        if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[0] & 1 << 11) == 0
           ) {
          if (AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime,&siteWeight)) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(11,siteWeight,2);
          }
        }
        else {
          if (AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime,&siteWeight)) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(ARM_0050_UNIT_MDL0103,siteWeight,0);
          }
        }
      }
    }
    else {
      if (AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime,&siteWeight)) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(ARM_0301_BUILDING_MDL0318,siteWeight,0);
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
  uint32_t primaryDistanceCap;
  uint32_t primaryWeight;
  uint32_t workspace02DistanceCap;
  uint32_t workspace02Weight;
  uint32_t secondaryDistanceCap;
  AiKnowledgeDataImage *knowledgeData;
  int primaryDistance;
  int primaryTerm;
  uint32_t workspace02Distance;
  uint32_t secondaryDistance;
  uint32_t remainingCount;
  int deltaX;
  int deltaY;
  AiScoredSiteWorkspaceEntry *siteEntry;
  
  remainingCount = g_AiWorkspace05Count;
  siteEntry = g_AiWorkspace05GeneralSites;
  for (; remainingCount != 0; siteEntry++, remainingCount--) {
    deltaX = siteEntry->cellWorldXQ12 - currentCell->worldX;
    if (deltaX < 0) {
      deltaX = -deltaX;
    }
    deltaY = siteEntry->cellWorldYQ12 - currentCell->worldY;
    if (deltaY < 0) {
      deltaY = -deltaY;
    }
    if ((deltaX < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12) &&
       (deltaY < (int)(g_AiKnowledgeData->parameters).generalSiteMinimumAxisSeparationQ12)) {
      return;
    }
  }
  cellWorldX = currentCell->worldX;
  cellWorldY = currentCell->worldY;
  if (g_AiWorkspace05Count < AI_WORKSPACE05_CAPACITY) {
    siteEntry->cellWorldXQ12 = cellWorldX;
    siteEntry->cellWorldYQ12 = cellWorldY;
    siteEntry->cell = currentCell;
    knowledgeData = g_AiKnowledgeData;
    primaryDistanceCap = (g_AiKnowledgeData->parameters).generalSitePrimaryDistanceCapQ12;
    primaryDistance = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
    primaryTerm = primaryDistanceCap - primaryDistance;
    if (primaryTerm < 0) {
      primaryTerm = 0;
    }
    primaryWeight = (knowledgeData->parameters).generalSitePrimaryDistanceCoefficient;
    workspace02DistanceCap = (knowledgeData->parameters).generalSiteVisibleHostileDistanceCapQ12;
    workspace02Distance = AiHostileWorkspace_GetNearestVisibleHostileDistance(cellWorldY,cellWorldX);
    if ((int)workspace02DistanceCap < (int)workspace02Distance) {
      workspace02Distance = workspace02DistanceCap;
    }
    workspace02Weight = (knowledgeData->parameters).generalSiteVisibleHostileDistanceCoefficient;
    secondaryDistanceCap = (knowledgeData->parameters).generalSiteSecondaryDistanceCapQ12;
    secondaryDistance = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
    if ((int)secondaryDistanceCap < (int)secondaryDistance) {
      secondaryDistance = secondaryDistanceCap;
    }
    g_AiWorkspace05Count++;
    siteEntry->score =
         primaryTerm * primaryWeight + workspace02Distance * workspace02Weight +
         secondaryDistance * (knowledgeData->parameters).generalSiteSecondaryDistanceCoefficient;
  }
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
  int deltaX;
  int deltaY;
  uint32_t remainingCount;
  AiScoredSiteWorkspaceEntry *siteEntry;
  AiKnowledgeParameters *wrongBaseParameters;
  int primaryDistanceCap;
  int primaryDistance;
  int primaryTerm;
  int primaryWeight;
  int workspace02DistanceCap;
  int workspace02Distance;
  int secondaryDistanceCap;
  int secondaryDistance;
  int score;

  remainingCount = g_AiWorkspace06Count;
  siteEntry = (AiScoredSiteWorkspaceEntry *)g_AiWorkspace06FlaggedSites;
  for (; remainingCount != 0; siteEntry++, remainingCount--) {
    deltaX = siteEntry->cellWorldXQ12 - currentCell->worldX;
    if (deltaX < 0) {
      deltaX = -deltaX;
    }
    deltaY = siteEntry->cellWorldYQ12 - currentCell->worldY;
    if (deltaY < 0) {
      deltaY = -deltaY;
    }
    if ((deltaX < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12) &&
       (deltaY < (int)(g_AiKnowledgeData->parameters).flaggedSiteMinimumAxisSeparationQ12)) {
      return;
    }
  }
  cellWorldX = currentCell->worldX;
  cellWorldY = currentCell->worldY;
  if (g_AiWorkspace06Count < AI_WORKSPACE06_CAPACITY) {
    siteEntry->cellWorldXQ12 = cellWorldX;
    siteEntry->cellWorldYQ12 = cellWorldY;
    siteEntry->cell = currentCell;
    /* the wrong-base reads (see above): the ki.dat parameter layout applied to the new entry's address */
    wrongBaseParameters = (AiKnowledgeParameters *)siteEntry;
    primaryDistanceCap = wrongBaseParameters->flaggedSitePrimaryDistanceCapQ12;
    primaryDistance = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
    primaryTerm = primaryDistanceCap - primaryDistance;
    if (primaryTerm < 0) {
      primaryTerm = 0;
    }
    primaryWeight = wrongBaseParameters->flaggedSitePrimaryDistanceCoefficient;
    workspace02DistanceCap = wrongBaseParameters->flaggedSiteVisibleHostileDistanceCapQ12;
    workspace02Distance = AiHostileWorkspace_GetNearestVisibleHostileDistance(cellWorldY,cellWorldX);
    if (workspace02DistanceCap < workspace02Distance) {
      workspace02Distance = workspace02DistanceCap;
    }
    score = primaryTerm * primaryWeight +
            workspace02Distance * (int)wrongBaseParameters->flaggedSiteVisibleHostileDistanceCoefficient;
    secondaryDistanceCap = wrongBaseParameters->flaggedSiteSecondaryDistanceCapQ12;
    secondaryDistance = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(cellWorldY,cellWorldX);
    /* unlike the general site score: (cap - distance) * weight when not negative, not min(distance, cap) */
    if (-1 < secondaryDistanceCap - secondaryDistance) {
      score = score + (secondaryDistanceCap - secondaryDistance) *
                      (int)wrongBaseParameters->flaggedSiteSecondaryDistanceCoefficient;
    }
    g_AiWorkspace06Count++;
    siteEntry->score = score;
  }
}


/* True when the cell lies within 2.0 of the 1:5 marker of any class-13 structure in workspace 00. */
static bool AiSiteCandidate_IsNearClass13StructureMarker(const FieldGridCell *terrainFeatureCell)

{
  uint32_t remainingCount;
  AiStructureWorkspaceEntry *workspace00Entry;
  ModelRuntimeSlot *runtimeSlot;
  ModelRuntimeNode *modelNodeRuntime;
  ModelPackedPointRecord *markerRecord;
  ModelWorldPoint markerPoint;
  uint32_t markerDistance;

  workspace00Entry = g_AiWorkspace00Structures;
  for (remainingCount = g_AiWorkspace00Count; remainingCount != 0; remainingCount--) {
    runtimeSlot = (ModelRuntimeSlot *)workspace00Entry->runtimeSlotAddressOrZero;
    if (runtimeSlot != NULL) {
      modelNodeRuntime = runtimeSlot->rootModelNodeOrSavedOffset.modelNode;
      if ((runtimeSlot->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
          (ModelLookupTable_FindPackedPoint(1,5,(modelNodeRuntime->modelPayload).modelResource,&markerRecord))) {
        markerPoint = ModelNodeRuntime_TransformLocalPoint(markerRecord,modelNodeRuntime);
        markerDistance = FixedMath_Length2(markerPoint.yQ12 - terrainFeatureCell->worldY,
                                  markerPoint.xQ12 - terrainFeatureCell->worldX);
        if ((int)markerDistance < 2 * Q12_ONE + 1) { /* within 2.0 */
          return true;
        }
      }
    }
    workspace00Entry++;
  }
  return false;
}


/* Scans the workspace-00 structures for the smallest Manhattan distance to the cell (INT32_MAX when none has a
   runtime slot). Returns false, leaving *outNearestDistance unset, as soon as a structure of the same asset lies
   closer than terrainFeatureMinimumAxisSeparationQ12 on both axes. */
static bool AiSiteCandidate_FindNearestStructureDistance(const FieldGridCell *terrainFeatureCell,
          PckArmyAssetIdCatalog featureAssetId,int *outNearestDistance)

{
  uint32_t remainingCount;
  AiStructureWorkspaceEntry *workspace00Entry;
  ModelRuntimeNode *structureNode;
  int deltaX;
  int deltaY;
  int nearestDistance;

  nearestDistance = INT32_MAX;
  workspace00Entry = g_AiWorkspace00Structures;
  for (remainingCount = g_AiWorkspace00Count; remainingCount != 0; remainingCount--) {
    if (workspace00Entry->runtimeSlotAddressOrZero != 0) {
      structureNode =
           ((ModelRuntimeSlot *)workspace00Entry->runtimeSlotAddressOrZero)->rootModelNodeOrSavedOffset.modelNode;
      deltaX = structureNode->worldTransform.translation.x - terrainFeatureCell->worldX;
      if (deltaX < 0) {
        deltaX = -deltaX;
      }
      deltaY = structureNode->worldTransform.translation.y - terrainFeatureCell->worldY;
      if (deltaY < 0) {
        deltaY = -deltaY;
      }
      if ((featureAssetId == workspace00Entry->armyAssetId) &&
          (deltaX < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12) &&
          (deltaY < (int)(g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12)) {
        return false;
      }
      if (deltaX + deltaY < nearestDistance) {
        nearestDistance = deltaX + deltaY;
      }
    }
    workspace00Entry++;
  }
  *outNearestDistance = nearestDistance;
  return true;
}


/* Terrain-feature priority: placementClearancePaddingQ12 minus the nearest structure distance, at least 0. */
static int AiSiteCandidate_TerrainFeaturePriority(int nearestDistance)

{
  int priority;

  priority = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - nearestDistance;
  if (priority < 0) {
    priority = 0;
  }
  return priority;
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
  int deltaX;
  int deltaY;
  uint32_t duplicateSeparation;
  PckArmyAssetIdCatalog featureAssetId;
  int nearestDistance;
  int priority;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;

  featureAssetId = ARM_0330_BUILDING_MDL0303;
  duplicateSeparation = (g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12 * 85 >> 8; /* ~1/3 */
  if ((terrainFeatureCell->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
    featureAssetId = ARM_0332_BUILDING_MDL0302;
  }
  if (AiSiteCandidate_IsNearClass13StructureMarker(terrainFeatureCell)) {
    return;
  }
  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  for (remainingFeatureCount = g_AiWorkspace08Count; remainingFeatureCount != 0;
       remainingFeatureCount--, terrainFeatureEntry++) {
    if (featureAssetId != terrainFeatureEntry->armyAssetId) {
      continue;
    }
    deltaX = terrainFeatureEntry->cell->worldX - terrainFeatureCell->worldX;
    if (deltaX < 0) {
      deltaX = -deltaX;
    }
    deltaY = terrainFeatureEntry->cell->worldY - terrainFeatureCell->worldY;
    if (deltaY < 0) {
      deltaY = -deltaY;
    }
    if ((deltaX < (int)duplicateSeparation) && (deltaY < (int)duplicateSeparation)) {
      /* a near duplicate: replace it when the new cell has the higher priority, never append */
      if (!AiSiteCandidate_FindNearestStructureDistance(terrainFeatureCell,featureAssetId,&nearestDistance)) {
        return;
      }
      priority = AiSiteCandidate_TerrainFeaturePriority(nearestDistance);
      if (priority > terrainFeatureEntry->priority) {
        terrainFeatureEntry->cell = terrainFeatureCell;
        terrainFeatureEntry->priority = priority;
      }
      return;
    }
  }
  if (g_AiWorkspace08Count < AI_WORKSPACE08_CAPACITY) {
    /* Original quirk: cell and asset are written into the free slot before the structure scan, which may still
       reject the cell; the slot then stays beyond the count. */
    terrainFeatureEntry->cell = terrainFeatureCell;
    terrainFeatureEntry->armyAssetId = featureAssetId;
    if (!AiSiteCandidate_FindNearestStructureDistance(terrainFeatureCell,featureAssetId,&nearestDistance)) {
      return;
    }
    priority = AiSiteCandidate_TerrainFeaturePriority(nearestDistance);
    g_AiWorkspace08Count++;
    terrainFeatureEntry->priority = priority;
  }
}


/* Address: 0x00539200.
   Runs the mode-0 placement query for the asset at a workspace cell (its position and heading) and returns the
   query's result inverted: true when the asset cannot be placed there.
*/
bool AiPlacement_TestWorkspaceRecordAtPoint(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,UiRootNode *inGameRoot)

{
  return !ArmyPlacement_CanPlaceAssetAtFieldPoint
                    (0,0,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,placementContext,
                     inGameRoot,NULL);
}


/* Address: 0x0053A1B0.
   Runs the mode-4 placement query for the asset at a workspace cell (its position and heading) and returns
   the query's result inverted: true when the asset cannot be placed there in that mode.
*/
bool AiPlacement_TestMode4AtWorkspaceRecord(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  return !ArmyPlacement_CanPlaceAssetAtFieldPoint
                    (4,0,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime,NULL);
}


/* Address: 0x0053B570.
   Counts how many separation quanta of special sites the asset could use at a workspace cell: when the mode-3
   query reports a nonzero count and the mode-0 query fails, the count is rounded up to whole
   specialSiteSeparationQuantumQ12 units; otherwise the result is 0. Returns false (*outBucketCount untouched)
   only when both the mode-3 and the mode-0 query fail; otherwise stores the count and returns true.
*/
bool AiPlacement_QueryReachableSiteBucketCount(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outBucketCount)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t placementCount;
  uint32_t heading;
  uint32_t quantum;
  uint32_t bucketCount;

  knowledgeData = g_AiKnowledgeData;
  heading = (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles;
  if (!ArmyPlacement_CanPlaceAssetAtFieldPoint
                (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,heading,
                 workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                 (UiRootNode *)worldRuntime,&placementCount)) {
    if (!ArmyPlacement_CanPlaceAssetAtFieldPoint
                  (0,0,heading,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                   factionIndex,(UiRootNode *)worldRuntime,NULL)) {
      return false;
    }
    bucketCount = 0;
  }
  else if ((placementCount != 0) &&
          (!ArmyPlacement_CanPlaceAssetAtFieldPoint
                         (0,0,heading,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                          factionIndex,(UiRootNode *)worldRuntime,NULL))) {
    /* Round the placement count up to whole separation quanta. */
    quantum = (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
    bucketCount = ((placementCount - 1) + quantum) / quantum;
  }
  else {
    bucketCount = 0;
  }
  *outBucketCount = bucketCount;
  return true;
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
  uint32_t quantum;
  uint32_t bucketCount;
  uint32_t placementCount;

  knowledgeData = g_AiKnowledgeData;
  if (!ArmyPlacement_CanPlaceAssetAtFieldPoint
                (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                 (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                 workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime,
                 &placementCount)) {
    return true;
  }
  /* ceil(placementCount / quantum) */
  quantum = (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
  bucketCount = ((placementCount - 1) + quantum) / quantum;
  if (bucketCount == 0) {
    return true;
  }
  if (bucketCount < 5) {
    /* A small cluster is accepted only when the separated chain reports CF set. */
    if (!AiPlacement_ReserveSeparatedSpecialSiteChain(armyAssetId,workspaceRecord,factionIndex,worldRuntime)) {
      return true;
    }
  }
  return false;
}


/* Address: 0x0053AD50.
   Finds the first workspace-08 resource site that passes AiPlacement_ReserveMode3SiteCluster and lies at least
   specialSiteMinimumWorkspaceDistanceQ12 from workspaces 03 and 02, and weighs it: the site's base weight
   (ARM_0330 or other) x3 / (2 * assigned structures of that asset + 6); for a non-ARM_0330 site it is further
   scaled by (2 * unpowered + supplied energy demand) / (tritiumCurrentQ4 << 8) when the faction has tritium.
   Returns true with the weight in *outWeight; false (*outWeight untouched) when no site qualifies.
*/
bool AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outWeight)

{
  FieldGridCell *workspaceRecord;
  uint32_t baseWeight;
  int remainingSites;
  int assignedCount;
  int tritiumScaled;
  uint32_t weight;
  AiTerrainFeatureWorkspaceEntry *featureEntry;
  bool clusterRejected;
  AiKnowledgeDataImage *knowledgeData;

  knowledgeData = g_AiKnowledgeData;
  remainingSites = g_AiWorkspace08Count;
  featureEntry = g_AiWorkspace08TerrainFeatureSites;
  for (; remainingSites != 0; featureEntry++, remainingSites--) {
    workspaceRecord = featureEntry->cell;
    clusterRejected = AiPlacement_ReserveMode3SiteCluster
                      (featureEntry->armyAssetId,workspaceRecord,factionIndex,worldRuntime);
    if ((!clusterRejected) &&
        ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <=
         (int)AiHostileWorkspace_GetNearestUnseenHostileDistance(workspaceRecord->worldY,workspaceRecord->worldX))
        && ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <=
            (int)AiHostileWorkspace_GetNearestVisibleHostileDistance
                   (workspaceRecord->worldY,workspaceRecord->worldX))) {
      assignedCount = AiPrimaryWorkspace_CountAssignedEntriesById(featureEntry->armyAssetId);
      baseWeight = (knowledgeData->parameters).specialSite14aBaseWeight;
      if (featureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
        baseWeight = (knowledgeData->parameters).specialSite14cBaseWeight;
      }
      weight = (baseWeight * 3) / (assignedCount * 2 + 6U);
      if (featureEntry->armyAssetId != ARM_0330_BUILDING_MDL0303) {
        tritiumScaled = g_GameFactionRuntimeImage.records[factionIndex].tritiumCurrentQ4 << 8;
        if (tritiumScaled != 0) {
          weight = (uint32_t)(((int64_t)(int)weight *
                         (int64_t)
                         (int)(g_GameFactionRuntimeImage.records[factionIndex].unpoweredEnergyDemandQ4 *
                               2 + g_GameFactionRuntimeImage.records[factionIndex].
                                   suppliedEnergyDemandQ4)) / (int64_t)tritiumScaled);
        }
      }
      *outWeight = weight;
      return true;
    }
  }
  return false;
}


/* Address: 0x00539330.
   Returns the position of the workspace-09 cell nearest (Manhattan distance) to the reference point at which
   the mode-1 placement query accepts the asset: returns true and stores the cell position in *outWorldYQ12 /
   *outWorldXQ12, or returns false (outputs untouched) when no such cell exists.
   Note the argument order: Y first, then X, like ArmyPlacement_CanPlaceAssetAtFieldPoint.
*/
bool AiPlacement_FindNearestPlaceableBaseSite
          (Q12 referenceWorldYQ12,Q12 referenceWorldXQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime,Q12 *outWorldYQ12,
          Q12 *outWorldXQ12)

{
  int deltaX;
  int remainingCount;
  uint32_t candidateDistance;
  int deltaY;
  uint32_t bestDistance;
  FieldGridCell **gridCellCursor;
  FieldGridCell *bestCell;
  FieldGridCell *candidateCell;

  candidateDistance = 0;
  if (g_AiWorkspace09Count != 0) {
    bestDistance = INT32_MAX;
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
        if (ArmyPlacement_CanPlaceAssetAtFieldPoint
                          (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,candidateCell->worldY,
                           candidateCell->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime,NULL)) {
          bestDistance = candidateDistance;
          bestCell = candidateCell;
        }
      }
      gridCellCursor++;
      remainingCount--;
    } while (remainingCount != 0);
    if ((int)bestDistance < INT32_MAX) {
      *outWorldXQ12 = bestCell->worldX;
      *outWorldYQ12 = bestCell->worldY;
      return true;
    }
  }
  return false;
}


/* Manhattan distance between an anchor point and a workspace cell, as compared unsigned against
   specialSiteSeparationQuantumQ12. */
static uint32_t AiPlacement_AnchorManhattanDistanceToCell(Q12 anchorYQ12,Q12 anchorXQ12,
          const FieldGridCell *workspaceRecord)

{
  int deltaX;
  int deltaY;

  deltaX = anchorXQ12 - workspaceRecord->worldX;
  if (deltaX < 0) {
    deltaX = -deltaX;
  }
  deltaY = anchorYQ12 - workspaceRecord->worldY;
  if (deltaY < 0) {
    deltaY = -deltaY;
  }
  return (uint32_t)(deltaX + deltaY);
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
  ArmyRuntimeSlot *probeInstances[4];
  uint32_t probeCount;
  ArmyRuntimeSlot *probeInstance;
  Q12 anchorXQ12;
  Q12 anchorYQ12;
  AiKnowledgeDataImage *knowledgeData;
  bool rejected;

  knowledgeData = g_AiKnowledgeData;
  rejected = true;
  probeCount = 0;
  while (probeCount < 4) {
    if (!AiPlacement_FindNearestPlaceableBaseSite
           (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,factionIndex,worldRuntime,
            &anchorYQ12,&anchorXQ12)) {
      break;
    }
    /* Y before X, as at every ArmyRuntime_CreateInstanceFromAsset call site */
    probeInstance = ArmyRuntime_CreateInstanceFromAsset
                      (1,0,anchorYQ12,anchorXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,worldRuntime,NULL);
    if (probeInstance == NULL) {
      break;
    }
    probeInstances[probeCount] = probeInstance;
    probeCount++;
    if (AiPlacement_AnchorManhattanDistanceToCell(anchorYQ12,anchorXQ12,workspaceRecord) <
        (knowledgeData->parameters).specialSiteSeparationQuantumQ12) {
      rejected = false;
      break;
    }
  }
  /* destroy the temporary instances, newest first */
  while (probeCount != 0) {
    probeCount--;
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)probeInstances[probeCount]);
  }
  return rejected;
}

