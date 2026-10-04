/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/construction.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/construction.h>
#include <thandor/thandor.h>

/* Module data. */

ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime = 0;

static uint32_t g_AiConstructionPendingAssetConsumedCount = 0;

/* Implementation ownership: gameplay/ai/construction. */

/* Builds a pending resource structure (ARM_0330/ARM_0332) of the AI faction: at the first workspace-08 site of
   this asset where the mode-0 placement test passes it creates the structure with the site's heading, rebuilds
   its model transforms, dispatches its class command, starts the effect referenced by its model runtime and
   removes the asset from the faction's pending list. Nothing happens when no site passes.
*/
void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *workspaceRecord;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *primarySlot;
  ModelRuntimeSlot *createdModelRuntime;
  Ptr32<ArmyRuntimeSlot> *createdSlotPair;
  int recordsRemaining;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;

  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  for (recordsRemaining = g_AiWorkspace08Count; recordsRemaining != 0; recordsRemaining--, terrainFeatureEntry++) {
    if (armyAssetId != terrainFeatureEntry->armyAssetId) {
      continue;
    }
    workspaceRecord = terrainFeatureEntry->cell;
    if (AiPlacement_TestWorkspaceRecordAtPoint(armyAssetId,workspaceRecord,factionIndex,(UiRootNode *)worldRuntime)) {
      continue; /* placement rejected */
    }
    createdSlotPair = (Ptr32<ArmyRuntimeSlot> *)ArmyRuntime_CreateInstanceFromAsset
                      (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                       workspaceRecord->worldY,workspaceRecord->worldX,factionIndex,armyAssetId,
                       worldRuntime,NULL);
    if (createdSlotPair == NULL) {
      return;
    }
    /* the create result points at the pair {army slot, model node}; the node is typed as a slot here, so
       the effect arguments below are its fields under ArmyRuntimeSlot names */
    modelNodeRuntime = createdSlotPair[1];
    primarySlot = *createdSlotPair;
    modelNodeRuntime->movementPosition0Q12 = 0;
    createdModelRuntime = (primarySlot->modelRuntimeOrSavedOffset).modelRuntime;
    ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
    ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdSlotPair,worldRuntime); /* the created army */
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){NULL},
               ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle2,
               ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle1,
               ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle0,
               ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.z,
               ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.y,
               ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.x,
               Thandor_U32ToPointer<EffectDefinition>(createdModelRuntime->attachments[2].childLocalRotationAngle0), /* 5f-format: ModelRuntimeSlot.attachments[2].childLocalRotationAngle0 */
               worldRuntime);
    AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
    return;
  }
}

/* Works through the faction's pending asset requests (workspace 04) in order and hands each to its placement
   handler by ARM id: 300 only while no unassigned 330 exists, 330/332 at a workspace site, 333 derived from a
   330/332 site, other ids below 340 at a reachable candidate, ids from 340 on near the faction anchor.
   Returns true as soon as a handler has placed an asset (g_AiConstructionPendingAssetConsumedCount).
*/
Bool8 AiConstructionPlanner_ProcessPendingAssetRequests
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  PckArmyAssetIdCatalog armyAssetId;
  int remainingRequests;
  AiRuntimeWorkspaceEntry *requestEntry;
  ArmyAssetRecordPrefix *armyAsset;
  
  g_AiConstructionPendingAssetConsumedCount = 0;
  requestEntry = g_AiWorkspace04RequestedAssets;
  for (remainingRequests = g_AiWorkspace04Count; remainingRequests != 0; remainingRequests--, requestEntry++) {
    armyAssetId = requestEntry->armyAssetId;
    if (armyAssetId == ARM_0300_BUILDING_MDL0301) {
      if (!AiPrimaryWorkspace_HasUnassignedEntryById(ARM_0330_BUILDING_MDL0303)) {
        AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                  (armyAssetId,factionIndex,worldRuntime);
      }
    }
    else if ((armyAssetId == ARM_0330_BUILDING_MDL0303) ||
            (armyAssetId == ARM_0332_BUILDING_MDL0302)) {
      AiConstructionPlanner_PlaceSpecialAssetFromWorkspace(armyAssetId,factionIndex,worldRuntime);
    }
    else if (armyAssetId == ARM_0331_BUILDING_MDL0308) {
      AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                (armyAssetId,factionIndex,worldRuntime);
    }
    else if (armyAssetId == ARM_0333_BUILDING_MDL0307) {
      AiConstructionPlanner_PlaceTritiumStorageNearResourceSite
                (ARM_0333_BUILDING_MDL0307,factionIndex,worldRuntime);
    }
    else if (armyAssetId < ARM_0340_BUILDING_MDL0314) {
      if (ArmyAssetRegistry_FindById(armyAssetId,&armyAsset) == 0) {
        /* The original then compares the selected definition's placementContactKindIndex with 1 (ignoring the
           selector's status), but both outcomes call the same placement handler. */
        (void)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,armyAsset->rootNodeOffsetOrPointer); /* 5f-format: ArmyAssetRecord.rootNodeOffsetOrPointer */
        AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
                  (armyAssetId,factionIndex,worldRuntime);
      }
    }
    else {
      AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
                (armyAssetId,factionIndex,worldRuntime);
    }
    if (g_AiConstructionPendingAssetConsumedCount != 0) {
      return true;
    }
  }
  return false;
}

/* Tries to build armyAssetId next to the workspace-08 resource site siteEntry: when
   AiPlacement_ReserveAdditionalSpecialSite accepts the site and a placeable base cell is found near it, the asset
   is created there (technology unlocked), its model node's movement position cleared, its transforms rebuilt, its
   class command dispatched, the effect named by the created model's attachment 2 started at the node and the asset
   removed from the faction's pending list. Returns true when the attempt is over (asset created, or its creation
   failed), false when the site is not usable. */
static Bool8 AiConstructionPlanner_TryPlaceStorageAtResourceSite
          (AiTerrainFeatureWorkspaceEntry *siteEntry,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)
{
  FieldGridCell *sourceCell;
  ArmyRuntimeSlot *createdModelNode;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;
  Ptr32<ArmyRuntimeSlot> *armyRuntime;
  Q12 anchorXQ12;
  Q12 anchorYQ12;

  sourceCell = siteEntry->cell;
  if (AiPlacement_ReserveAdditionalSpecialSite(siteEntry->armyAssetId,sourceCell,factionIndex,worldRuntime)) {
    return false;
  }
  if (!AiPlacement_FindNearestPlaceableBaseSite
         (sourceCell->worldY,sourceCell->worldX,armyAssetId,factionIndex,worldRuntime,&anchorYQ12,&anchorXQ12)) {
    return false;
  }
  armyRuntime = (Ptr32<ArmyRuntimeSlot> *)ArmyRuntime_CreateInstanceFromAsset
                    (ARMY_CREATE_UNLOCK_TECHNOLOGY,0,anchorYQ12,anchorXQ12,factionIndex,armyAssetId,worldRuntime,
                     NULL);
  if (armyRuntime == NULL) {
    return true;
  }
  createdModelNode = armyRuntime[1];
  createdArmySlot = *armyRuntime;
  createdModelNode->movementPosition0Q12 = 0;
  createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)createdModelNode);
  ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)armyRuntime,worldRuntime); /* the created army */
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){NULL},
             ((ModelRuntimeNode *)createdModelNode)->modelPayload.worldRotationAngle2,
             ((ModelRuntimeNode *)createdModelNode)->modelPayload.worldRotationAngle1,
             ((ModelRuntimeNode *)createdModelNode)->modelPayload.worldRotationAngle0,
             ((ModelRuntimeNode *)createdModelNode)->worldTransform.translation.z,
             ((ModelRuntimeNode *)createdModelNode)->worldTransform.translation.y,
             ((ModelRuntimeNode *)createdModelNode)->worldTransform.translation.x,
             Thandor_U32ToPointer<EffectDefinition>(createdModelRuntime->attachments[2].childLocalRotationAngle0), /* 5f-format: ModelRuntimeSlot.attachments[2].childLocalRotationAngle0 */
             worldRuntime);
  AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
  return true;
}

/* Runs AiConstructionPlanner_TryPlaceStorageAtResourceSite for every workspace-08 site of siteAssetId in order;
   returns true as soon as one attempt is over. */
static Bool8 AiConstructionPlanner_TryPlaceStorageAtResourceSitesOf
          (PckArmyAssetIdCatalog siteAssetId,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)
{
  AiTerrainFeatureWorkspaceEntry *siteEntry;
  int remainingEntries;

  siteEntry = g_AiWorkspace08TerrainFeatureSites;
  for (remainingEntries = g_AiWorkspace08Count; remainingEntries != 0; remainingEntries--, siteEntry++) {
    if ((siteEntry->armyAssetId == siteAssetId) &&
        AiConstructionPlanner_TryPlaceStorageAtResourceSite(siteEntry,armyAssetId,factionIndex,worldRuntime)) {
      return true;
    }
  }
  return false;
}

/* Builds a pending ARM_0333 (the caller's only asset here, 0x14D) next to a resource site: first for each
   ARM_0330 site of workspace 08, then for each ARM_0332 site, where AiPlacement_ReserveAdditionalSpecialSite
   accepts the site it creates the structure at the nearest valid workspace-09 cell, initialises it like the other
   planners, starts its effect and removes it from the pending list (a failed creation ends the attempt). Without
   such a site it falls back to AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate.
*/
void AiConstructionPlanner_PlaceTritiumStorageNearResourceSite(PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  if (AiConstructionPlanner_TryPlaceStorageAtResourceSitesOf
        (ARM_0330_BUILDING_MDL0303,armyAssetId,factionIndex,worldRuntime)) {
    return;
  }
  if (AiConstructionPlanner_TryPlaceStorageAtResourceSitesOf
        (ARM_0332_BUILDING_MDL0302,armyAssetId,factionIndex,worldRuntime)) {
    return;
  }
  AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate(armyAssetId,factionIndex,worldRuntime);
  return;
}

/* Creates armyAssetId for the faction at the chosen field cell (technology unlocked), clears the new model
   node's movement position, rebuilds its transforms, dispatches the army's class command, spawns the effect named
   by the created model's attachment 2 at the node and removes the asset from the faction's pending list. Shared tail of the
   placement planners; nothing happens when the creation fails. */
static void AiConstructionPlanner_CreatePlacedAsset
          (FieldGridCell *cell,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)
{
  Ptr32<ArmyRuntimeSlot> *createdSlots;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *createdArmySlot;
  ModelRuntimeSlot *createdModelRuntime;

  createdSlots = (Ptr32<ArmyRuntimeSlot> *)ArmyRuntime_CreateInstanceFromAsset
                    (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)cell->triangle0NormalAngles,
                     cell->worldY,cell->worldX,factionIndex,armyAssetId,worldRuntime,NULL);
  if (createdSlots == NULL) {
    return;
  }
  modelNodeRuntime = createdSlots[1];
  createdArmySlot = *createdSlots;
  modelNodeRuntime->movementPosition0Q12 = 0;
  createdModelRuntime = (createdArmySlot->modelRuntimeOrSavedOffset).modelRuntime;
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
  ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdSlots,worldRuntime); /* the created army */
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){NULL},
             ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle2,
             ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle1,
             ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle0,
             ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.z,
             ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.y,
             ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.x,
             Thandor_U32ToPointer<EffectDefinition>(createdModelRuntime->attachments[2].childLocalRotationAngle0), /* 5f-format: ModelRuntimeSlot.attachments[2].childLocalRotationAngle0 */
             worldRuntime);
  AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
}

/* Default placement of a pending building (every asset the request dispatcher does not handle specially), only
   while the faction's primary anchor cooldown is nonzero: among the workspace-09 cells at least 0x2000 from every
   active primary-workspace structure it takes the one with the lowest Manhattan distance to the primary anchor
   plus 4x the distance to the nearest workspace-02 site (at most 0x5000) or, without such a site, 2x the
   distance to the nearest workspace-03 site (at most 0x8000), where the mode-1 placement test passes. The
   building is created there, initialised like the other planners and removed from the pending list.
*/
void AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  int anchorDistanceY;
  int structureDistance;
  int siteDistance;
  int candidateScore;
  int remainingCells;
  int anchorDistanceX;
  FieldGridCell **gridCellCursor;
  FieldGridCell *bestCell;
  int bestScore;
  FieldGridCell *candidateCell;
  Bool8 siteDistanceInRange;

  if ((g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown == 0) ||
     (g_AiWorkspace09Count == 0)) {
    return;
  }
  bestScore = INT32_MAX;
  gridCellCursor = g_AiWorkspace09Cells;
  for (remainingCells = g_AiWorkspace09Count; remainingCells != 0; remainingCells--, gridCellCursor++) {
    candidateCell = *gridCellCursor;
    anchorDistanceY = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorYQ12 - candidateCell->worldX;
    if (anchorDistanceY < 0) {
      anchorDistanceY = -anchorDistanceY;
    }
    anchorDistanceX = g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorXQ12 - candidateCell->worldY;
    if (anchorDistanceX < 0) {
      anchorDistanceX = -anchorDistanceX;
    }
    if (anchorDistanceX + anchorDistanceY >= bestScore) continue;
    structureDistance = AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint
                          (candidateCell->worldY,candidateCell->worldX);
    if (structureDistance <= 2 * Q12_ONE - 1) continue;
    /* Distance to the nearest workspace-02 site (x4, at most 5.0), or without any such site to the
       nearest workspace-03 site (x2, at most 8.0). */
    siteDistance = AiHostileWorkspace_GetNearestVisibleHostileDistance(candidateCell->worldY,candidateCell->worldX);
    if (siteDistance < INT32_MAX) {
      siteDistanceInRange = siteDistance < 5 * Q12_ONE + 1;
      if (siteDistanceInRange) {
        siteDistance = siteDistance * 4;
      }
    }
    else {
      siteDistance = AiHostileWorkspace_GetNearestUnseenHostileDistance(candidateCell->worldY,candidateCell->worldX);
      siteDistanceInRange = siteDistance < 8 * Q12_ONE + 1;
      if (siteDistanceInRange) {
        siteDistance = siteDistance * 2;
      }
    }
    if (!siteDistanceInRange) continue;
    candidateScore = anchorDistanceX + anchorDistanceY + siteDistance;
    if ((candidateScore < bestScore) &&
       (ArmyPlacement_CanPlaceAssetAtFieldPoint
                          (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,
                           candidateCell->worldY,candidateCell->worldX,armyAssetId,factionIndex,
                           (UiRootNode *)worldRuntime,NULL))) {
      bestCell = candidateCell;
      bestScore = candidateScore;
    }
  }
  if (bestScore < INT32_MAX) {
    AiConstructionPlanner_CreatePlacedAsset(bestCell,armyAssetId,factionIndex,worldRuntime);
  }
}

/* Places a pending asset at a workspace-10 cell, only while the faction's primary anchor cooldown is 0. Each cell
   is scored by its Manhattan distance to the faction's ARM_0300 structure (g_AiWorkspaceOwnedAsset300Runtime;
   the raw cell coordinates without one) plus 16 random bits; the lowest-scoring cell where the mode-1
   placement test passes and GridReachability_RebuildConnectedRegionAroundWorldPoint (with the radius of the
   asset's model definition) finds a connected region wins. The asset is created there, initialised like the
   other planners and removed from the pending list.
*/
void AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  uint32_t radiusMetric;
  FieldGridCell *candidateCell;
  int distanceX;
  int candidateScore;
  uint32_t randomBits;
  int remainingCells;
  int distanceY;
  FieldGridCell **gridCellCursor;
  Bool8 regionUnreachable;
  ArmyAssetRecordPrefix *armyAsset;
  ModelDefinitionRecordPrefix *modelDefinition;
  FieldGridCell *bestCell;
  int bestScore;

  if ((ArmyAssetRegistry_FindById(armyAssetId,&armyAsset) != 0) ||
     (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0)) {
    return;
  }
  modelDefinition = ModelDefinitionRegistry_FindById
                     (Thandor_U32ToPointer<AiLinkedDefinitionListView>(armyAsset->rootNodeOffsetOrPointer)->definitionIds[0]); /* 5f-format: ArmyAssetRecord.rootNodeOffsetOrPointer */
  if (modelDefinition == NULL) {
    return;
  }
  radiusMetric = ((ModelDefinition *)modelDefinition)->footprintRadius;
  if (g_AiWorkspace10Count == 0) {
    return;
  }
  bestScore = INT32_MAX;
  gridCellCursor = g_AiWorkspace10Cells;
  for (remainingCells = g_AiWorkspace10Count; remainingCells != 0; remainingCells--, gridCellCursor++) {
    candidateCell = *gridCellCursor;
    distanceX = candidateCell->worldX;
    distanceY = candidateCell->worldY;
    if (g_AiWorkspaceOwnedAsset300Runtime != NULL) {
      distanceX = distanceX -
                  (g_AiWorkspaceOwnedAsset300Runtime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.x;
      if (distanceX < 0) {
        distanceX = -distanceX;
      }
      distanceY = distanceY -
                  (g_AiWorkspaceOwnedAsset300Runtime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y;
      if (distanceY < 0) {
        distanceY = -distanceY;
      }
    }
    randomBits = g_RandomGeneratorState.next();
    candidateScore = distanceY + distanceX + (randomBits & 0xffff);
    if ((candidateScore < bestScore) &&
       (ArmyPlacement_CanPlaceAssetAtFieldPoint
                          (1,0,(uint32_t)(uint16_t)candidateCell->triangle0NormalAngles,candidateCell->worldY,
                           candidateCell->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime,
                           NULL))) {
      regionUnreachable = GridReachability_RebuildConnectedRegionAroundWorldPoint
                            (radiusMetric,candidateCell->worldY,candidateCell->worldX);
      if (!regionUnreachable) {
        bestCell = candidateCell;
        bestScore = candidateScore;
      }
    }
  }
  if (bestScore < INT32_MAX) {
    AiConstructionPlanner_CreatePlacedAsset(bestCell,armyAssetId,factionIndex,worldRuntime);
  }
}

/* Called after the AI has placed an army asset: counts the placement (global counter and the faction's
   relationCounterB) and removes the first entry for that asset's registry record from the faction's pending
   primary army asset list, shifting the rest down. The registry lookup's result is not checked; an unknown id simply
   matches no entry.
*/
void AiConstructionPlanner_ConsumeFactionPendingArmyAsset
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex)

{
  FactionRelationCounter *relationCounter;
  FactionArmyAssetCount *pendingAssetCount;
  FactionArmyAssetCount remainingAssets;
  uint32_t *assetPointerCursor;
  ArmyAssetRecordPrefix *armyAsset;
  
  g_AiConstructionPendingAssetConsumedCount++;
  remainingAssets = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in armyAsset,
     which then matches no list entry) */
  ArmyAssetRegistry_FindById(armyAssetId,&armyAsset);
  assetPointerCursor = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds;
  relationCounter = &g_GameFactionRuntimeImage.records[factionIndex].relationCounterB;
  (*relationCounter)++;
  for (; remainingAssets != 0; remainingAssets--) {
    if (armyAsset == Thandor_U32ToPointer<ArmyAssetRecordPrefix>(*assetPointerCursor)) break; /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
    assetPointerCursor++;
  }
  if (remainingAssets == 0) {
    return;
  }
  pendingAssetCount = &g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
  (*pendingAssetCount)--;
  /* Shift the entries after the match down by one. */
  for (remainingAssets--; remainingAssets != 0; remainingAssets--) {
    *assetPointerCursor = assetPointerCursor[1];
    assetPointerCursor++;
  }
}
