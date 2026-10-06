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
    return nullptr;
  }
  recordCursor->common.allocationFlags = WORLD_OBJECT_RECORD_ALLOCATED;
  recordCursor->common.ownerWorld = worldRuntime;
  return recordCursor;
}

/* Marks node as linked and puts it at the head of its world's owner list (ownerListHead; the head is
   swapped first, then the neighbour links are set).
*/
void WorldRuntime_LinkOwnerListNode(WorldOwnerListNode *node)

{
  Ptr32<WorldOwnerListNode> *ownerListHeadLink;
  WorldOwnerListNode *previousHeadNode;
  WorldRuntimeContext *ownerWorld;

  ownerWorld = node->ownerWorld;
  node->runtimeFlags = node->runtimeFlags | WORLD_OWNER_NODE_LINKED;
  ownerListHeadLink = &ownerWorld->ownerListHead;
  previousHeadNode = *ownerListHeadLink;
  *ownerListHeadLink = node;
  node->previousNode = nullptr;
  node->nextNode = previousHeadNode;
  if (previousHeadNode != nullptr) {
    previousHeadNode->previousNode = node;
  }
}

/* Takes a linked node out of its world's owner list (fixing the neighbours or the list head) and clears all
   of its runtime flags, the linked mark included. runtimeFlags (+0x4C) is the same field as
   WorldObjectRecordCommon.allocationFlags, so this also frees the node's world object record
   (WORLD_OBJECT_RECORD_ALLOCATED). The node's own previousNode/nextNode stay as they were;
   WorldRuntime_ForEachOwnerListNode relies on that.
*/
void WorldRuntime_UnlinkOwnerListNode(WorldOwnerListNode *node)

{
  WorldOwnerListNode *previousNode;
  WorldOwnerListNode *nextNode;

  if ((node->runtimeFlags & WORLD_OWNER_NODE_LINKED) != 0) {
    previousNode = node->previousNode;
    nextNode = node->nextNode;
    if (previousNode == nullptr) {
      node->ownerWorld->ownerListHead = nextNode;
    }
    else {
      previousNode->nextNode = nextNode;
    }
    if (nextNode != nullptr) {
      nextNode->previousNode = previousNode;
    }
  }
  node->runtimeFlags = 0;
}

/* Calls callback(callbackContext, node) for every node of the world's owner list (ownerListHead), from the most
   recently linked one on.
   Original quirk: callbacks may destroy objects while the walk runs (e.g. the shutdown release callback via
   ArmyRuntime_DestroyInstanceAndRefreshUi), unlinking this node and others and nesting further ForEach calls.
   The walk still reads node->nextNode afterwards; that works only because WorldRuntime_UnlinkOwnerListNode
   leaves the unlinked node's own links intact and the object pools are not freed meanwhile. Keep both if this
   list is ever rewritten (clearing the links on unlink would end or change the walk).
*/
void WorldRuntime_ForEachOwnerListNode(void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world)

{
  WorldOwnerListNode *node;

  for (node = world->ownerListHead; node != nullptr; node = node->nextNode) {
    callback(callbackContext,node);
  }
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
    modelRuntime = WorldOwnerNode_ModelRuntime(node);
    ModelRuntimeHierarchy_ClearMatchingTargetRecursive(releasedObject,modelRuntime);
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
      ownerArmy->commandTargetArmyRuntime = nullptr;
      ownerArmy->commandModeFlags =
           ownerArmy->commandModeFlags &
           ~(ARMY_COMMAND_MODE_TARGET_ARMY | ARMY_COMMAND_MODE_INTERRUPTED | ARMY_COMMAND_MODE_AI_COMBAT_TARGET);
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) &&
          (releasedObject ==
           WorldOwnerNode_EffectRuntime(node)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.
           modelNode)) {
    WorldOwnerNode_EffectRuntime(node)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode =
         nullptr;
  }
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
        WorldOwnerNode_EffectRuntime(node)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode) {
      WorldOwnerNode_EffectRuntime(node)->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode =
           nullptr;
    }
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    /* the linked model runtime and, for an aircraft, the linked base model runtime */
    modelRuntime = WorldOwnerNode_ModelRuntime(node);
    if (detachedObject == modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime) {
      modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
    }
    if (modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT &&
        detachedObject == modelRuntime->classLinkState.modelLinkOrState.modelRuntime) {
      modelRuntime->classLinkState.modelLinkOrState.modelRuntime = nullptr;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          (detachedObject ==
           WorldOwnerNode_ShotRuntime(node)->runtimeStateOrSavedOffset.runtimeStatePointer)) {
    WorldOwnerNode_ShotRuntime(node)->runtimeStateOrSavedOffset.runtimeState = 0;
  }
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
               WorldOwnerNode_ModelRuntime(node)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) {
    node->runtimeFlags = node->runtimeFlags & ~(WORLD_OWNER_NODE_LINKED | WORLD_OBJECT_RECORD_ALLOCATED);
    WorldOwnerNode_ShotRuntime(node)->modelNodeOrSavedOffset.savedIdOrOffset = 0;
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    node->runtimeFlags = node->runtimeFlags & ~(WORLD_OWNER_NODE_LINKED | WORLD_OBJECT_RECORD_ALLOCATED);
    WorldOwnerNode_EffectRuntime(node)->modelNodeOrSavedOffset.savedIdOrOffset = 0;
  }
}
