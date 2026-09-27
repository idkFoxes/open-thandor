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
  AiKnowledgeDataImage *knowledgeData;
  uint32_t placementCount;
  uint32_t normalAnglesOrQuantum;
  bool chainFailed;
  PlacementDispatchResult dispatchResult;
  
  knowledgeData = g_AiKnowledgeData;
  normalAnglesOrQuantum = (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles;
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (7,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,normalAnglesOrQuantum,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  placementCount = dispatchResult.value;
  if ((!dispatchResult.failed) && (placementCount != 0)) {
    dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                      (4,0,normalAnglesOrQuantum,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                       factionIndex,(UiRootNode *)worldRuntime);
    if ((dispatchResult.failed) &&
       (normalAnglesOrQuantum = (knowledgeData->parameters).specialSiteSeparationQuantumQ12,
       ((placementCount - 1) + normalAnglesOrQuantum) / normalAnglesOrQuantum < 5)) {
      chainFailed = AiPlacement_ReserveSeparatedSpecialSiteChain
                        (armyAssetId,workspaceRecord,factionIndex,worldRuntime);
      return chainFailed;
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
  bool hasEntry;
  SiteWeightResult weightResult;
  
  hasEntry = AiSecondaryWorkspace_HasEntryByIdCf(ARM_0050_UNIT_MDL0103);
  if (!hasEntry) {
    hasEntry = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0301_BUILDING_MDL0318);
    if (hasEntry) {
      hasEntry = ArmyAssetRegistry_FindEnabledByIdCf(ARM_0050_UNIT_MDL0103);
      if (!hasEntry) {
        if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[0] & 0x800) == 0
           ) {
          weightResult = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          if (!weightResult.noSite) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0xb,weightResult.score,2);
          }
        }
        else {
          weightResult = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
          if (!weightResult.noSite) {
            AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x32,weightResult.score,0);
          }
        }
      }
    }
    else {
      weightResult = AiCandidatePlanning_ComputeSpecialSiteWeight(factionIndex,worldRuntime);
      if (!weightResult.noSite) {
        AiCandidateWorkspace_AddOrAccumulateWeightedEntry(0x12d,weightResult.score,0);
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
  siteEntry = g_AiWorkspaceBuffer05_Size0200;
  while( true ) {
    if (remainingCount == 0) {
      worldY = currentCell->worldX;
      worldX = currentCell->worldY;
      if (g_AiWorkspace05Count < 0x20) {
        siteEntry->cellWorldXQ12 = worldY;
        siteEntry->cellWorldYQ12 = worldX;
        siteEntry->cell = currentCell;
        knowledgeData = g_AiKnowledgeData;
        primaryCapOrWeight = (g_AiKnowledgeData->parameters).unknownParameterDword33;
        deltaXOrPrimaryTerm = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        deltaXOrPrimaryTerm = primaryCapOrWeight - deltaXOrPrimaryTerm;
        if (deltaXOrPrimaryTerm < 0) {
          deltaXOrPrimaryTerm = 0;
        }
        primaryCapOrWeight = (knowledgeData->parameters).unknownParameterDwords36_37[1];
        workspace02CapOrWeight = (knowledgeData->parameters).unknownParameterDwords36_37[0];
        workspace02Distance = AiWorkspace02_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if ((int)workspace02CapOrWeight < (int)workspace02Distance) {
          workspace02Distance = workspace02CapOrWeight;
        }
        workspace02CapOrWeight = (knowledgeData->parameters).unknownParameterDwords40_47[0];
        secondaryDistanceCap = (knowledgeData->parameters).generalSiteSecondaryDistanceCapQ12;
        secondaryDistance = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if ((int)secondaryDistanceCap < (int)secondaryDistance) {
          secondaryDistance = secondaryDistanceCap;
        }
        g_AiWorkspace05Count = g_AiWorkspace05Count + 1;
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
    siteEntry = siteEntry + 1;
    remainingCount = remainingCount - 1;
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
  int deltaXOrCapTerm;
  int workspace02Distance;
  uint32_t remainingCount;
  int workspace02CapOrScore;
  int deltaYOrDistanceOrWeight;
  uint8_t *siteEntryBytes;
  
  remainingCount = g_AiWorkspace06Count;
  siteEntryBytes = g_AiWorkspaceBuffer06_Size0400;
  while( true ) {
    if (remainingCount == 0) {
      worldY = currentCell->worldX;
      worldX = currentCell->worldY;
      if (g_AiWorkspace06Count < 0x40) {
        *(Q12 *)siteEntryBytes = worldY;
        *(Q12 *)(siteEntryBytes + 4) = worldX;
        *(FieldGridCell **)(siteEntryBytes + 0xc) = currentCell;
        deltaXOrCapTerm = *(int *)(siteEntryBytes + 0xc4);
        deltaYOrDistanceOrWeight = AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        deltaXOrCapTerm = deltaXOrCapTerm - deltaYOrDistanceOrWeight;
        if (deltaXOrCapTerm < 0) {
          deltaXOrCapTerm = 0;
        }
        deltaYOrDistanceOrWeight = *(int *)(siteEntryBytes + 0xd4);
        workspace02CapOrScore = *(int *)(siteEntryBytes + 0xd0);
        workspace02Distance = AiWorkspace02_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if (workspace02CapOrScore < workspace02Distance) {
          workspace02Distance = workspace02CapOrScore;
        }
        workspace02CapOrScore = deltaXOrCapTerm * deltaYOrDistanceOrWeight + workspace02Distance * *(int *)(siteEntryBytes + 0xe0);
        deltaXOrCapTerm = *(int *)(siteEntryBytes + 200);
        deltaYOrDistanceOrWeight = AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(worldX,worldY);
        if (-1 < deltaXOrCapTerm - deltaYOrDistanceOrWeight) {
          workspace02CapOrScore = workspace02CapOrScore + (deltaXOrCapTerm - deltaYOrDistanceOrWeight) * *(int *)(siteEntryBytes + 0xd8);
        }
        g_AiWorkspace06Count = g_AiWorkspace06Count + 1;
        *(int *)(siteEntryBytes + 8) = workspace02CapOrScore;
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
    remainingCount = remainingCount - 1;
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
  AiWorkspace00EntryView8 *workspace00Entry;
  ModelLookupEntryResult markerLookup;
  ModelLocalPointRegs12 markerPoint;
  int nearestDistanceOrPriority;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;
  
  remainingFeatureCount = g_AiWorkspace08Count;
  featureAssetId = ARM_0330_BUILDING_MDL0303;
  duplicateSeparation = (g_AiKnowledgeData->parameters).terrainFeatureMinimumAxisSeparationQ12 * 0x55 >> 8;
  countOrDeltaX = g_AiWorkspace00Count;
  workspace00Entry = g_AiWorkspaceBuffer00_Size0400;
  if ((terrainFeatureCell->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
    featureAssetId = ARM_0332_BUILDING_MDL0302;
  }
  for (; terrainFeatureEntry = g_AiWorkspaceBuffer08_Size0200, countOrDeltaX != 0; countOrDeltaX = countOrDeltaX + -1) {
    runtimeSlot = (int *)workspace00Entry->runtimeSlotAddressOrZero;
    if ((runtimeSlot != (int *)0x0) &&
       (modelNodeRuntime = (ModelRuntimeNode *)runtimeSlot[1], *(int *)(*runtimeSlot + 0x4c) == 0xd)) {
      markerLookup = ModelLookupTable_ContainsPackedKeyCf
                         (1,5,(modelNodeRuntime->modelPayload).modelResource);
      if (!markerLookup.notFound) {
        markerPoint = ModelNodeRuntime_TransformLocalPointRegs(markerLookup.entry,modelNodeRuntime);
        markerDistance = FixedMath_Length2(markerPoint.ecx - terrainFeatureCell->worldY,
                                  markerPoint.eax - terrainFeatureCell->worldX);
        if ((int)markerDistance < 0x2001) {
          return;
        }
      }
    }
    workspace00Entry = workspace00Entry + 1;
  }
  do {
    if (remainingFeatureCount == 0) {
      if (g_AiWorkspace08Count < 0x20) {
        terrainFeatureEntry->cell = terrainFeatureCell;
        terrainFeatureEntry->armyAssetId = featureAssetId;
        nearestDistanceOrPriority = 0x7fffffff;
        workspace00Entry = g_AiWorkspaceBuffer00_Size0400;
        for (countOrDeltaX = g_AiWorkspace00Count; countOrDeltaX != 0; countOrDeltaX = countOrDeltaX + -1) {
          if (workspace00Entry->runtimeSlotAddressOrZero != 0) {
            entityOrDeltaY = *(int *)(workspace00Entry->runtimeSlotAddressOrZero + 4);
            entityDeltaX = *(int *)(entityOrDeltaY + 0x94) - terrainFeatureCell->worldX;
            if (entityDeltaX < 0) {
              entityDeltaX = -entityDeltaX;
            }
            entityOrDeltaY = *(int *)(entityOrDeltaY + 0x98) - terrainFeatureCell->worldY;
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
          workspace00Entry = workspace00Entry + 1;
        }
        nearestDistanceOrPriority = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - nearestDistanceOrPriority;
        if (nearestDistanceOrPriority < 0) {
          nearestDistanceOrPriority = 0;
        }
        g_AiWorkspace08Count = g_AiWorkspace08Count + 1;
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
        workspace00Entry = g_AiWorkspaceBuffer00_Size0400;
        countOrDeltaX = g_AiWorkspace00Count;
        do {
          if (countOrDeltaX == 0) {
            nearestDistanceOrPriority = (g_AiKnowledgeData->parameters).placementClearancePaddingQ12 - nearestDistanceOrPriority;
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
            entityOrDeltaY = *(int *)(workspace00Entry->runtimeSlotAddressOrZero + 4);
            entityDeltaX = *(int *)(entityOrDeltaY + 0x94) - terrainFeatureCell->worldX;
            if (entityDeltaX < 0) {
              entityDeltaX = -entityDeltaX;
            }
            entityOrDeltaY = *(int *)(entityOrDeltaY + 0x98) - terrainFeatureCell->worldY;
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
          workspace00Entry = workspace00Entry + 1;
          countOrDeltaX = countOrDeltaX + -1;
        } while( true );
      }
    }
    remainingFeatureCount = remainingFeatureCount - 1;
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
  PlacementDispatchResult dispatchResult;
  
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (0,0,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,placementContext,
                     inGameRoot);
  return dispatchResult.failed;
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
  PlacementDispatchResult dispatchResult;
  
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (4,0,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  return dispatchResult.failed;
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
StatusResult __thandor_eax_cf_preserve_ecx_edx
AiPlacement_QueryReachableSiteBucketCount
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiKnowledgeDataImage *knowledgeData;
  uint32_t placementCount;
  uint32_t normalAnglesOrBucketCount;
  PlacementDispatchResult dispatchResult;
  StatusResult countResult;
  StatusResult failureResult;
  
  knowledgeData = g_AiKnowledgeData;
  normalAnglesOrBucketCount = (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles;
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,normalAnglesOrBucketCount,
                     workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,factionIndex,
                     (UiRootNode *)worldRuntime);
  placementCount = dispatchResult.value;
  if (dispatchResult.failed) {
    dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                      (0,0,normalAnglesOrBucketCount,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                       factionIndex,(UiRootNode *)worldRuntime);
    if (dispatchResult.failed) {
      failureResult.valueOrError = dispatchResult.value;
      failureResult.failed = dispatchResult.failed;
      return failureResult;
    }
    normalAnglesOrBucketCount = 0;
  }
  else if ((placementCount != 0) &&
          (dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                             (0,0,normalAnglesOrBucketCount,workspaceRecord->worldY,workspaceRecord->worldX,armyAssetId,
                              factionIndex,(UiRootNode *)worldRuntime), dispatchResult.failed)) {
    /* Round the placement count up to whole separation quanta. */
    normalAnglesOrBucketCount = (knowledgeData->parameters).specialSiteSeparationQuantumQ12;
    normalAnglesOrBucketCount = ((placementCount - 1) + normalAnglesOrBucketCount) / normalAnglesOrBucketCount;
  }
  else {
    normalAnglesOrBucketCount = 0;
  }
  countResult.failed = false;
  countResult.valueOrError = normalAnglesOrBucketCount;
  return countResult;
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
  AiKnowledgeDataImage *knowledgeData;
  uint32_t quantumOrBucketCount;
  bool chainFailed;
  PlacementDispatchResult dispatchResult;
  
  knowledgeData = g_AiKnowledgeData;
  dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                    (3,(g_AiKnowledgeData->parameters).placementClearancePaddingQ12,
                     (uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,workspaceRecord->worldY,
                     workspaceRecord->worldX,armyAssetId,factionIndex,(UiRootNode *)worldRuntime);
  if ((dispatchResult.failed) ||
     (quantumOrBucketCount = (knowledgeData->parameters).specialSiteSeparationQuantumQ12,
     quantumOrBucketCount = ((dispatchResult.value - 1) + quantumOrBucketCount) / quantumOrBucketCount, quantumOrBucketCount == 0)) {
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
   Ownership: gameplay/ai/placement.
   Purpose: Scans workspace08 records, reserves eligible mode-three site clusters, enforces minimum distance from
   workspace02 and workspace03, and returns an assigned-count and faction-resource-scaled weight for the first
   acceptable special-site asset.
   Local calls: AiPlacement_ReserveMode3SiteCluster.
   Cross-module calls: AiWorkspace03_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiWorkspace02_GetMinimumManhattanDistanceToPoint [gameplay/ai/workspaces],
   AiPrimaryWorkspace_CountAssignedEntriesById [gameplay/ai/workspaces].
*/
SiteWeightResult __thandor_eax_cf_preserve_ecx_edx
AiCandidatePlanning_ComputeSpecialSiteWeight
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
  featureEntry = g_AiWorkspaceBuffer08_Size0200;
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
         (int)AiWorkspace03_GetMinimumManhattanDistanceToPoint(workspaceRecord->worldY,workspaceRecord->worldX))
        && ((int)(knowledgeData->parameters).specialSiteMinimumWorkspaceDistanceQ12 <=
            (int)AiWorkspace02_GetMinimumManhattanDistanceToPoint
                   (workspaceRecord->worldY,workspaceRecord->worldX)))
    break;
    featureEntry = featureEntry + 1;
    countOrTritium = countOrTritium + -1;
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
   Ownership: gameplay/ai/placement.
   Purpose: It returns the selected anchor coordinates when a valid entry exists. Stock ARM contains 675 records
   and 326 unique ids; placement workspace, producer, tier, class, and faction-role semantics are not inferred from
   numeric adjacency. Typed parameters: p2 referenceWorldXQ12→Q12, p3 referenceWorldYQ12→Q12. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
AiAnchorResult __thandor_preserve_eax
AiPlacement_FindNearestValidWorkspace09Anchor
          (Q12 referenceWorldXQ12,Q12 referenceWorldYQ12,PckArmyAssetIdCatalog armyAssetId,
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
    gridCellCursor = g_AiWorkspaceBuffer09_Size1000;
    do {
      candidateCell = *gridCellCursor;
      deltaX = referenceWorldYQ12 - candidateCell->worldX;
      if (deltaX < 0) {
        deltaX = -deltaX;
      }
      deltaY = referenceWorldXQ12 - candidateCell->worldY;
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
      gridCellCursor = gridCellCursor + 1;
      remainingCount = remainingCount + -1;
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
  int deltaX;
  Q12 worldXQ12;
  int deltaY;
  ArmyRuntimeCreateResult firstInstance;
  ArmyRuntimeCreateResult secondInstance;
  ArmyRuntimeCreateResult thirdInstance;
  ArmyRuntimeCreateResult fourthInstance;
  AiAnchorResult anchor;
  AiKnowledgeDataImage *knowledgeData;
  
  knowledgeData = g_AiKnowledgeData;
  anchor = AiPlacement_FindNearestValidWorkspace09Anchor
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  worldXQ12 = anchor.worldYQ12;
  worldYQ12 = anchor.worldXQ12;
  if (anchor.notFound) {
    return true;
  }
  firstInstance = ArmyRuntime_CreateInstanceFromAssetCf
                    (1,0,worldXQ12,worldYQ12,factionIndex,ARM_0333_BUILDING_MDL0307,worldRuntime);
  if (firstInstance.failed) {
    return true;
  }
  deltaX = worldYQ12 - workspaceRecord->worldX;
  if (deltaX < 0) {
    deltaX = -deltaX;
  }
  deltaY = worldXQ12 - workspaceRecord->worldY;
  if (deltaY < 0) {
    deltaY = -deltaY;
  }
  if ((uint32_t)(deltaX + deltaY) < (knowledgeData->parameters).specialSiteSeparationQuantumQ12)
  goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarryClear;
  anchor = AiPlacement_FindNearestValidWorkspace09Anchor
                    (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                     factionIndex,worldRuntime);
  if (anchor.notFound) goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet;
  secondInstance = ArmyRuntime_CreateInstanceFromAssetCf
                    (1,0,anchor.worldYQ12,anchor.worldXQ12,factionIndex,ARM_0333_BUILDING_MDL0307,
                     worldRuntime);
  if (secondInstance.failed) goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet;
  deltaX = anchor.worldXQ12 - workspaceRecord->worldX;
  if (deltaX < 0) {
    deltaX = -deltaX;
  }
  deltaY = anchor.worldYQ12 - workspaceRecord->worldY;
  if (deltaY < 0) {
    deltaY = -deltaY;
  }
  if ((knowledgeData->parameters).specialSiteSeparationQuantumQ12 <= (uint32_t)(deltaX + deltaY)) {
    anchor = AiPlacement_FindNearestValidWorkspace09Anchor
                      (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307,
                       factionIndex,worldRuntime);
    if (!anchor.notFound) {
      thirdInstance = ArmyRuntime_CreateInstanceFromAssetCf
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
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyThirdSecondFirstAndReturnCarryClear:
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)thirdInstance.armyRuntimeOrError);
          goto AiPlacement_ReserveSeparatedSpecialSiteChain_DestroySecondFirstAndReturnCarryClear;
        }
        anchor = AiPlacement_FindNearestValidWorkspace09Anchor
                          (workspaceRecord->worldY,workspaceRecord->worldX,ARM_0333_BUILDING_MDL0307
                           ,factionIndex,worldRuntime);
        if (!anchor.notFound) {
          fourthInstance = ArmyRuntime_CreateInstanceFromAssetCf
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
              ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)fourthInstance.armyRuntimeOrError);
              goto 
              AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyThirdSecondFirstAndReturnCarryClear
              ;
            }
            ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)fourthInstance.armyRuntimeOrError);
          }
        }
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)thirdInstance.armyRuntimeOrError);
      }
    }
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)secondInstance.armyRuntimeOrError);
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarrySet:
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)firstInstance.armyRuntimeOrError);
    return true;
  }
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroySecondFirstAndReturnCarryClear:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)secondInstance.armyRuntimeOrError);
AiPlacement_ReserveSeparatedSpecialSiteChain_DestroyFirstAndReturnCarryClear:
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,(GameEntityRuntime *)firstInstance.armyRuntimeOrError);
  return false;
}

