/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/runtime/entity_registry.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* World object registry: the object array and its free-record allocation, the owner list of model, shot and
   effect nodes and the owner-list callbacks that clear references when an object goes away. */

#include <thandor/world/runtime/entity_registry.h>
#include <thandor/thandor.h>

/* Attaches the pool of 0x100-byte object records that WorldObjectArray_AllocateFreeRecord hands out (callers
   attach 0x100 or 0x4000 records).
*/
void WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world)

{
  world->objectArray = objectArray;
  world->objectCount = count;
  return;
}

/* Takes the first free record of the world's object pool (WorldRuntime_AttachObjectArray): marks it allocated
   (which also resets its other flag bits) and stores the owning world. Returns the record, or NULL when the
   pool is exhausted (the original returned FATAL_ERROR_GENERAL_FAILURE as its failure result; callers that pass an error
   code on use that constant).
*/
WorldObjectRecord *WorldObjectArray_AllocateFreeRecord(WorldRuntimeContext *worldRuntime)

{
  WorldObjectRecordCount recordsRemaining;
  WorldObjectRecord *recordCursor;

  recordsRemaining = worldRuntime->objectCount;
  recordCursor = worldRuntime->objectArray;
  while (recordsRemaining != 0 &&
         (recordCursor->common.allocationFlags & WORLD_OBJECT_RECORD_ALLOCATED) != 0) {
    recordCursor = recordCursor + 1;
    recordsRemaining--;
  }
  if (recordsRemaining == 0) {
    return NULL;
  }
  recordCursor->common.allocationFlags = WORLD_OBJECT_RECORD_ALLOCATED;
  recordCursor->common.ownerWorld = worldRuntime;
  return recordCursor;
}

/* Marks node as linked and puts it at the head of its world's owner list (ownerListHead; the head is
   swapped atomically, the neighbour links are then set without a lock).
*/
void WorldRuntime_LinkOwnerListNode(WorldOwnerListNode *node)

{
  Ptr32<WorldOwnerListNode> *ownerListHeadLink;
  WorldOwnerListNode *previousHeadNode;
  WorldRuntimeContext *ownerWorld;

  ownerWorld = node->ownerWorld;
  node->runtimeFlags = node->runtimeFlags | WORLD_OWNER_NODE_LINKED;
  LOCK();
  ownerListHeadLink = &ownerWorld->ownerListHead;
  previousHeadNode = *ownerListHeadLink;
  *ownerListHeadLink = node;
  UNLOCK();
  node->previousNode = NULL;
  node->nextNode = previousHeadNode;
  if (previousHeadNode != NULL) {
    previousHeadNode->previousNode = node;
  }
  return;
}

/* Takes a linked node out of its world's owner list (fixing the neighbours or the list head) and clears all
   of its runtime flags, the linked mark included.
*/
void WorldRuntime_UnlinkOwnerListNode(WorldOwnerListNode *node)

{
  WorldOwnerListNode *previousNode;
  WorldOwnerListNode *nextNode;

  if ((node->runtimeFlags & WORLD_OWNER_NODE_LINKED) != 0) {
    previousNode = node->previousNode;
    nextNode = node->nextNode;
    if (previousNode == NULL) {
      node->ownerWorld->ownerListHead = nextNode;
    }
    else {
      previousNode->nextNode = nextNode;
    }
    if (nextNode != NULL) {
      nextNode->previousNode = previousNode;
    }
  }
  node->runtimeFlags = 0;
  return;
}

/* Calls callback(callbackContext, node) for every node of the world's owner list (ownerListHead), from the most
   recently linked one on.
*/
void WorldRuntime_ForEachOwnerListNode(void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world)

{
  WorldOwnerListNode *node;

  for (node = world->ownerListHead; node != NULL; node = node->nextNode) {
    callback(callbackContext,node);
  }
  return;
}

/* Callback of WorldRuntime_ForEachOwnerListNode from ArmyRuntime_DestroyInstanceAndRefreshUi: removes
   every reference to the destroyed object from one world node, so nothing keeps targeting it. For a model
   node: its hierarchy's targets and two fields of the owning army (ownerArmyRuntimeOrSavedOffset); for an
   effect node: its owner model node (lifecycleOwnerAndDefinition).
*/
void WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode *node)

{
  ModelRuntimeSlot *modelRuntime;
  ArmyRuntimeSlot *ownerArmy;

  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    modelRuntime = (ModelRuntimeSlot *)node->runtimePayload;
    ModelRuntimeHierarchy_ClearMatchingTargetRecursive(releasedObject,(int *)modelRuntime);
    /* the army that owns the model */
    ownerArmy = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    /* 5f-format: ArmyRuntimeSlot.assignedTargetArmyRuntime (pool offset in saves) */
    if (releasedObject == Thandor_U32ToPointer<void>(ownerArmy->assignedTargetArmyRuntime)) {
      ownerArmy->assignedTargetArmyRuntime = 0;
    }
    /* the command target is only a live reference while ARMY_COMMAND_MODE_TARGET_ARMY is set; it is cleared
       together with the INTERRUPTED and AI_COMBAT_TARGET bits */
    if ((ownerArmy->commandModeFlags & ARMY_COMMAND_MODE_TARGET_ARMY) != 0 &&
        releasedObject == ownerArmy->commandTargetArmyRuntime) {
      ownerArmy->commandTargetArmyRuntime = NULL;
      ownerArmy->commandModeFlags =
           ownerArmy->commandModeFlags &
           ~(ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_INTERRUPTED | ARMY_COMMAND_MODE_AI_COMBAT_TARGET);
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) &&
          (releasedObject ==
           ((EffectRuntimeSlot *)node->runtimePayload)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.
           modelNode)) {
    ((EffectRuntimeSlot *)node->runtimePayload)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode =
         NULL;
  }
  return;
}

/* WorldRuntime_ForEachOwnerListNode callback run while a model runtime is destroyed
   (detachedObject = that model runtime): every effect (owner model node), shot (runtimeStateOrSavedOffset) or
   entity (classState.linkedArmyRuntimeOrSavedOffset, and classLinkState.modelLinkOrState for definition class
   0x15) that still points at it gets the pointer cleared, so nothing keeps a dangling reference.
*/
void WorldRuntimeNode_ClearDetachedEntityReferencesCallback(void *detachedObject,WorldOwnerListNode *node)

{
  ModelRuntimeSlot *modelRuntime;

  if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    if (detachedObject ==
        ((EffectRuntimeSlot *)node->runtimePayload)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode) {
      ((EffectRuntimeSlot *)node->runtimePayload)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode =
           NULL;
    }
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    /* the linked model runtime and, for an aircraft, the linked base model runtime */
    modelRuntime = (ModelRuntimeSlot *)node->runtimePayload;
    if (detachedObject == modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime) {
      modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
    }
    if (modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT &&
        detachedObject == modelRuntime->classLinkState.modelLinkOrState.modelRuntime) {
      modelRuntime->classLinkState.modelLinkOrState.modelRuntime = NULL;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          (detachedObject ==
           ((ShotRuntimeSlot *)node->runtimePayload)->runtimeStateOrSavedOffset.runtimeStatePointer)) {
    ((ShotRuntimeSlot *)node->runtimePayload)->runtimeStateOrSavedOffset.runtimeState = 0;
  }
  return;
}

/* WorldRuntime_ForEachOwnerListNode callback used when an in-game session shuts down, before the level
   resources are destroyed: destroys the army of every model node; for shot and effect nodes it clears flag bits
   31 (linked into the owner list) and 30 (record allocated) and zeroes the back-reference modelNodeOrSavedOffset of their runtime
   payload.
*/
void WorldRuntimeNode_ReleaseShutdownBindingsCallback(WorldRuntimeContext *shutdownContext,WorldOwnerListNode *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    ArmyRuntime_DestroyInstanceAndRefreshUi
              (shutdownContext,
               (GameEntityRuntime *)
               ((ModelRuntimeSlot *)node->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) {
    node->runtimeFlags = node->runtimeFlags & ~(WORLD_OWNER_NODE_LINKED | WORLD_OBJECT_RECORD_ALLOCATED);
    ((ShotRuntimeSlot *)node->runtimePayload)->modelNodeOrSavedOffset.savedIdOrOffset = 0;
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    node->runtimeFlags = node->runtimeFlags & ~(WORLD_OWNER_NODE_LINKED | WORLD_OBJECT_RECORD_ALLOCATED);
    ((EffectRuntimeSlot *)node->runtimePayload)->modelNodeOrSavedOffset.savedIdOrOffset = 0;
  }
  return;
}
