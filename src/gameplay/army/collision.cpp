/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/collision.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/collision.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/collision. */

/* Tests whether a model of this definition, with its placement radius (footprintRadius), would overlap any
   army in the world's owner list: a cheap depth-bin mask overlap first, then the exact circle test
   ArmyCollision_TestPointWithinExpandedRuntimeRadius. Returns true when an army is in the way (a
   definition without radius never collides).
   Called directly by ArmyPlacement_CanPlaceMobileUnit.
*/

Bool8 ArmyCollision_TestPointAgainstRuntimeList
          (Q12 worldXQ12,Q12 worldYQ12,uint8_t *modelDefinition,WorldRuntimeContext *worldRuntime)

{
  int placementRadiusQ12;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  WorldOwnerListNode *ownerNode;
  Bool8 hit;

  placementRadiusQ12 = ((ModelDefinition *)modelDefinition)->footprintRadius;
  ownerNode = worldRuntime->ownerListHead;
  if ((placementRadiusQ12 != 0) && (ownerNode != NULL)) {
    firstMaskHigh = DepthInterval_BuildBinMask(placementRadiusQ12,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(placementRadiusQ12,worldXQ12);
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        hit = DepthBinMasks_Overlap
                          (firstMaskLow,firstMaskHigh,ownerNode->modelDepthBinMaskFar,
                           ownerNode->modelDepthBinMaskNear);
        if (hit) {
          hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                            (placementRadiusQ12,worldXQ12,worldYQ12,(ModelRuntimeSlot *)ownerNode->runtimePayload);
          if (hit) {
            return true;
          }
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != NULL);
  }
  return false;
}

/* Finds the first model that a moving unit would run into at the given point: every model in the world's
   owner list whose depth bins overlap the unit's own, except the unit itself, the model it is linked to
   (classState.linkedArmyRuntimeOrSavedOffset) and models linked to it, and whose collision circle reaches the
   point within the unit's placement radius (its definition's footprintRadius). Returns that model runtime
   (never NULL), or NULL when nothing is in the way.
   Called directly by the ground-movement code (gameplay/army/movement.c) and by
   ArmyPlacement_TestGridRuntimeAndFieldBlocking.
*/
ModelRuntimeSlot *ArmyCollision_FindBlockingRuntimeForCurrentUnit
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *currentModelNode;
  uint32_t clearanceRadiusQ12;
  ModelRuntimeSlot *candidateModelRuntime;
  Bool8 hit;
  ModelRuntimeNode *candidateModelNode;
  
  currentModelNode = currentRuntime->modelNodeRuntime;
  clearanceRadiusQ12 = currentRuntime->modelDefinition->footprintRadius;
  if (clearanceRadiusQ12 == 0) {
    return NULL;
  }
  for (candidateModelNode = (ModelRuntimeNode *)worldRuntime->ownerListHead; candidateModelNode != NULL;
      candidateModelNode = (ModelRuntimeNode *)(candidateModelNode->common).nextNode) {
    if (candidateModelNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    hit = DepthBinMasks_Overlap
                      (currentRuntime->modelNodeRuntime->depthBinMaskFar,
                       currentRuntime->modelNodeRuntime->depthBinMaskNear,
                       candidateModelNode->depthBinMaskFar,candidateModelNode->depthBinMaskNear);
    if (!hit) {
      continue;
    }
    candidateModelRuntime = (candidateModelNode->runtimePayload).modelRuntime;
    if (currentModelNode == candidateModelNode) {
      continue;
    }
    /* the unit's linked model and models linked to the unit never block it (the original also tests
       currentRuntime for NULL here, after it was already dereferenced, so that test never fired) */
    if ((candidateModelRuntime == currentRuntime->linkedRuntime) ||
        ((ModelRuntimeSlot *)currentRuntime ==
         (candidateModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime)) {
      continue;
    }
    hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                      (clearanceRadiusQ12,worldXQ12,worldYQ12,candidateModelRuntime);
    if (hit) {
      /* never NULL here: the radius test and the link test above dereference it */
      return candidateModelRuntime;
    }
  }
  return NULL;
}

/* Tests whether a circle of queryRadiusQ12 at a candidate point hits any army in the world's owner list:
   depth-bin overlap first, then the exact circle test; armies of runtime class 0 and 12 never block, class-13
   armies also block when the point comes near their (1,5) anchor point. With
   ARMY_PLACEMENT_MODE_STRUCTURES_ONLY in the mode only armies of depth-bin class 0x90 count. Returns true on
   a hit.
   Called directly by ArmyPlacement_CanPlaceBuilding and
   ArmyPlacement_CanPlaceAnchoredModel.
*/

Bool8 ArmyPlacementCollision_TestPointAgainstRuntimeList
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime)

{
  int modelClassId;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  WorldOwnerListNode *ownerNode;
  Bool8 hit;

  ownerNode = worldRuntime->ownerListHead;
  if ((queryRadiusQ12 == 0) || (ownerNode == NULL)) {
    return false;
  }
  firstMaskHigh = DepthInterval_BuildBinMask(queryRadiusQ12,worldYQ12);
  firstMaskLow = DepthInterval_BuildBinMask(queryRadiusQ12,worldXQ12);
  for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelClassId = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId;
    hit = DepthBinMasks_Overlap
                      (firstMaskLow,firstMaskHigh,ownerNode->modelDepthBinMaskFar,
                       ownerNode->modelDepthBinMaskNear);
    if (!hit) {
      continue;
    }
    if (((placementFilterFlags & ARMY_PLACEMENT_MODE_STRUCTURES_ONLY) != 0) &&
        (g_ArmyRuntimeDepthBinClassByModelClass[modelClassId] != ARMY_DEPTH_BIN_CLASS_STRUCTURE)) {
      continue;
    }
    if ((modelClassId == MODEL_RUNTIME_CLASS_00) || (modelClassId == MODEL_RUNTIME_CLASS_12)) {
      continue;
    }
    hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                      (queryRadiusQ12,worldXQ12,worldYQ12,(ModelRuntimeSlot *)ownerNode->runtimePayload);
    if (hit) {
      return true;
    }
    if (modelClassId == MODEL_RUNTIME_CLASS_13) {
      hit = ArmyPlacementCandidate_TestModelAnchorDistance
                        (queryRadiusQ12,worldXQ12,worldYQ12,(ModelRuntimeSlot *)ownerNode->runtimePayload);
      if (hit) {
        return true;
      }
    }
  }
  return false;
}

/* Tests whether a placed model collides with another army at a point. The candidate is either a model runtime
   (candidateRuntime != NULL: its placement radius footprintRadius is used, the depth bins of its node must
   overlap, and it, its linked model (classState.linkedArmyRuntimeOrSavedOffset) and models linked to it are
   skipped) or a bare radius (candidateRuntime == NULL: radiusQ12 + 1 is used, no depth-bin pre-test; radiusQ12
   is ignored for a runtime). excludedWorldObject is skipped as well; armies of class 0 and 12 never block,
   class-13 armies also block near their (1,5) anchor. Returns true on a collision.
   Called directly by ArmyPlacementCollision_TestCurrentRuntime (runtime) and
   ArmyPlacement_TestModelTerrainAndRuntimeClearance (radius ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12).
   The original (0x00529F30) passed both kinds in one dword and told them apart with `cmp esi,0x400000; jae`
   (the image base: a radius below it, a model runtime at or above it). Its only two callers pass a model runtime
   and the constant 0xC00, so the kind is known at the call site; it is passed explicitly here because an x64
   heap address is not guaranteed to lie at or above 0x400000. The decisions are the original's.
*/

Bool8 ArmyPlacementCollision_TestCandidateAgainstRuntimeList
          (WorldOwnerListNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
          ModelRuntimeSlot *candidateRuntime,Q12 radiusQ12,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeSlot *ownerModelRuntime;
  uint32_t modelClassId;
  WorldOwnerListNode *candidateNode;
  intptr_t queryRadiusQ12;
  Bool8 candidateIsRuntime;
  Bool8 hit;
  WorldOwnerListNode *ownerNode;

  candidateIsRuntime = candidateRuntime != NULL;
  if (!candidateIsRuntime) {
    queryRadiusQ12 = (intptr_t)(uint32_t)radiusQ12 + 1; /* the radius value plus one */
    candidateNode = NULL;
  }
  else {
    /* the runtime's root node is also its world owner-list node */
    candidateNode = (WorldOwnerListNode *)candidateRuntime->rootModelNodeOrSavedOffset.modelNode;
    queryRadiusQ12 = (intptr_t)candidateRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius;
  }
  if (queryRadiusQ12 == 0) {
    return false;
  }
  for (ownerNode = worldRuntime->ownerListHead; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    if (candidateIsRuntime) {
      hit = DepthBinMasks_Overlap
                      (candidateRuntime->rootModelNodeOrSavedOffset.modelNode->depthBinMaskFar,
                       candidateRuntime->rootModelNodeOrSavedOffset.modelNode->depthBinMaskNear,
                       ownerNode->modelDepthBinMaskFar,ownerNode->modelDepthBinMaskNear);
      if (!hit) {
        continue;
      }
    }
    ownerModelRuntime = (ModelRuntimeSlot *)ownerNode->runtimePayload;
    if ((candidateNode == ownerNode) || (ownerNode == excludedWorldObject)) {
      continue;
    }
    /* the candidate's linked model and models linked to the candidate never block it */
    if (candidateIsRuntime &&
        ((ownerModelRuntime == candidateRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime) ||
         (candidateRuntime == ownerModelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime))) {
      continue;
    }
    modelClassId = ownerModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId;
    if ((modelClassId == MODEL_RUNTIME_CLASS_00) || (modelClassId == MODEL_RUNTIME_CLASS_12)) {
      continue;
    }
    hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                    ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,ownerModelRuntime);
    if (hit) {
      return true;
    }
    if (modelClassId == MODEL_RUNTIME_CLASS_13) {
      hit = ArmyPlacementCandidate_TestModelAnchorDistance
                      ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,ownerModelRuntime);
      if (hit) {
        return true;
      }
    }
  }
  return false;
}

/* Validates a placed building (the live counterpart of ArmyPlacement_CanPlaceBuilding): no
   other army may overlap it, the terrain around it must suit its contact kind, and - unless
   UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set - its field-grid point must not be blocked for
   its faction and it must stand within the support radius (supportRadius) plus its own margin
   (placementFlags) of another model of the same faction. Returns true when rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[4, 10, 11, 15, 16,
   20, 22, 23], called by ArmyRuntimeNode_DispatchTypedCallback; also called directly by
   ArmyPlacement_TestModelTerrainAndRuntimeClearance and ArmyPlacement_TestGridOccupancyMask.
*/

Bool8 ArmyPlacementCollision_TestCurrentRuntime
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  ModelDefinition *placementDefinition;
  uint32_t ownClearanceQ12;
  ArmyRuntimeSlot *ownerArmy;
  uint32_t neighborSupportRadiusQ12;
  int64_t deltaYSquared;
  int64_t remainingSquared;
  int terrainHeightQ12;
  int reachQ12;
  int deltaXQ12;
  int deltaYQ12;
  ModelRuntimeNode *ownerNode;
  ModelRuntimeSlot *neighbor;
  Bool8 blocked;
  Bool8 (*terrainTest)(FieldGridRadiusUnits,Q12,Q12,Q12,FieldGridAsset *);
  ModelRuntimeNode *rootNode;

  rootNode = modelRuntime->rootModelNode;
  placementDefinition = modelRuntime->modelDefinition;
  ownClearanceQ12 = placementDefinition->placementFlags;
  /* reference height for the terrain test: the node's height without the definition's and the resource's
     height offsets */
  terrainHeightQ12 = ((rootNode->worldTransform).translation.z - placementDefinition->placementHeightOffsetQ12) -
          ((rootNode->modelPayload).modelResource)->placementHeightOffsetQ12;
  terrainTest = TerrainHeightBand_TestAroundWorldPoint;
  if (placementDefinition->placementContactKindIndex == ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
    terrainTest = TerrainAuxHeightThreshold_TestAroundWorldPoint;
  }
  blocked = ArmyPlacementCollision_TestCandidateAgainstRuntimeList
                    ((WorldOwnerListNode *)rootNode,(rootNode->worldTransform).translation.y,
                     (rootNode->worldTransform).translation.x,(ModelRuntimeSlot *)modelRuntime,0,
                     worldRuntime);
  if (blocked) {
    return true;
  }
  /* rejected on the terrain test's own result */
  blocked = terrainTest(placementDefinition->footprintRadius,terrainHeightQ12,
                        (rootNode->worldTransform).translation.y,
                        (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (blocked) {
    return true;
  }
  rootNode = modelRuntime->rootModelNode;
  ownerArmy = modelRuntime->ownerArmyRuntime;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) != 0) {
    return false;
  }
  blocked = FieldGrid_TestWorldPointBlocked
                    (ownerArmy->factionIndex,(rootNode->worldTransform).translation.y,
                     (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (blocked) {
    return true;
  }
  ownerNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  if (ownerNode == NULL) {
    return false;
  }
  /* support check: some same-faction model with a support radius (supportRadius of its definition) must lie
     within that radius plus our own margin */
  for (; ownerNode != NULL; ownerNode = (ModelRuntimeNode *)(ownerNode->common).nextNode) {
    if ((ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) || (ownerNode == rootNode)) {
      continue;
    }
    neighbor = (ownerNode->runtimePayload).modelRuntime;
    neighborSupportRadiusQ12 = neighbor->definitionOrSavedId.runtimeDefinition->supportRadius;
    if ((ownerArmy->factionIndex != neighbor->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) ||
        (neighborSupportRadiusQ12 == 0)) {
      continue;
    }
    reachQ12 = neighborSupportRadiusQ12 + ownClearanceQ12;
    deltaXQ12 = (rootNode->worldTransform).translation.x - (ownerNode->worldTransform).translation.x;
    remainingSquared = (int64_t)reachQ12 * (int64_t)reachQ12 - (int64_t)deltaXQ12 * (int64_t)deltaXQ12;
    if (remainingSquared < 0) {
      continue;
    }
    deltaYQ12 = (rootNode->worldTransform).translation.y - (ownerNode->worldTransform).translation.y;
    deltaYSquared = (int64_t)deltaYQ12 * (int64_t)deltaYQ12;
    if (remainingSquared - deltaYSquared >= 0) {
      return false;
    }
  }
  return true;
}

/* Exact circle test between a point and a model: true when the point lies within the model's
   placement radius (definition footprintRadius) plus queryRadiusQ12 of the model's position, compared
   on the 64-bit squares. Models without radius and a zero query radius never hit.
   Called directly by the runtime-list collision scans in this file and by the movement code
   (gameplay/army/movement.c).
*/
Bool8 ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeSlot *modelRuntime)

{
  int64_t radiusSquared;
  int64_t distanceSquared;
  int deltaXQ12;
  int deltaYQ12;
  int reachQ12;

  if ((modelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius == 0) ||
      (queryRadiusQ12 == 0)) {
    return false;
  }
  deltaXQ12 = (modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.x - worldYQ12;
  deltaYQ12 = (modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y - worldXQ12;
  distanceSquared = (int64_t)deltaYQ12 * (int64_t)deltaYQ12 + (int64_t)deltaXQ12 * (int64_t)deltaXQ12;
  reachQ12 = modelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius + queryRadiusQ12;
  radiusSquared = (int64_t)reachQ12 * (int64_t)reachQ12;
  /* hit when the 64-bit difference radius^2 - distance^2 (wrapping) is not negative */
  return (int64_t)((uint64_t)radiusSquared - (uint64_t)distanceSquared) >= 0;
}
