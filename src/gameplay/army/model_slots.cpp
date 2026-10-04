/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/model_slots.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/model_slots.h>
#include <thandor/thandor.h>

/* Per-class model runtime callbacks from g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, indexed
   by the model definition's class id (runtimeClassId): modelClassInitialize (run by
   ModelRuntimePool_CreateInstanceByDefinitionId), modelReleaseOrCommit (ModelRuntimePool_DestroyHierarchyAndDetach),
   modelUnrebase (ModelRuntimePool_UnrebaseBeforeSave) and modelRebaseOrLoadRepair
   (ModelRuntimePool_RebaseAfterLoad). The class state of a ModelRuntimeSlot (classLinkState and classState) means
   something different for every class. */

/* Class initializer of model class 2 (modelClassInitialize[2]). Turns on texture scrolling for the root node:
   the definition's primary animated subresource (primaryAnimatedSubresourceIndex) and, only together with it, the
   secondary one (secondaryAnimatedSubresourceIndex),
   both starting at texture offset 0.
*/
void ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices
          (ModelDefinition *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot
          )

{
  ModelRuntimeNode *rootModelNode;
  ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
  AssetRecordByteCount secondaryAnimatedSubresourceIndex;

  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  primaryAnimatedSubresourceIndex = modelDefinition->primaryAnimatedSubresourceIndex;
  secondaryAnimatedSubresourceIndex = modelDefinition->secondaryAnimatedSubresourceIndex;
  if (primaryAnimatedSubresourceIndex != 0) {
    rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL;
    rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
    rootModelNode->primaryTextureOffsetU = 0;
    rootModelNode->primaryTextureOffsetV = 0;
    if (secondaryAnimatedSubresourceIndex != 0) {
      rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL;
      rootModelNode->secondaryAnimatedSubresourceIndex = secondaryAnimatedSubresourceIndex;
      rootModelNode->secondaryTextureOffsetU = 0;
      rootModelNode->secondaryTextureOffsetV = 0;
    }
  }
  return;
}


/* Class initializer of model class 3 (modelClassInitialize[3]). Marks the class fields classState68..classState74 as
   unset (0x80000000) and classState78 / effectEmitterTimerTicks as 0x7FFFFFFF, records the local Y of the root's
   first child and grandchild, and derives a starting timer value (stored in the first dword of classPrefixState
   and in classStateD0) from the definition's movementSpeed, accelerationPerTick, classParameterC0 and
   classParameterC4.
*/
void ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint32_t definitionValue0C;
  AssetRecordByteCount definitionValueC0;
  int initialTimingValue;
  ModelRuntimeNode *rootChild0Node;
  Q12 grandchildLocalYQ12;

  definitionValue0C = ((ModelDefinition *)modelDefinition)->movementSpeed;
  modelRuntimeSlot->classLinkState.classState68 = MODEL_CLASS_STATE_UNSET_COORDINATE;
  modelRuntimeSlot->classLinkState.armyLinkOrState.classState = MODEL_CLASS_STATE_UNSET_COORDINATE;
  modelRuntimeSlot->classLinkState.classState70 = MODEL_CLASS_STATE_UNSET_COORDINATE;
  modelRuntimeSlot->classLinkState.classState74 = MODEL_CLASS_STATE_UNSET_COORDINATE;
  modelRuntimeSlot->classState.classStateA8 = 0;
  modelRuntimeSlot->classState.classStateAC = 0;
  modelRuntimeSlot->classState.behaviorState = 0;
  modelRuntimeSlot->classLinkState.classState78 = INT32_MAX;
  rootChild0Node = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode->childNodes[0];
  grandchildLocalYQ12 = rootChild0Node->childNodes[0]->modelPayload.localTranslationYQ12;
  modelRuntimeSlot->classState.classStateB0 = rootChild0Node->modelPayload.localTranslationYQ12;
  modelRuntimeSlot->classState.classStateB4 = grandchildLocalYQ12;
  definitionValueC0 = ((ModelDefinition *)modelDefinition)->classParameterC0;
  initialTimingValue =
       (int)(((int64_t)(int)(((int64_t)(int)definitionValue0C << 12) / (int64_t)(int)definitionValueC0) *
              (int64_t)(int)(((ModelDefinition *)modelDefinition)->classParameterC4 + definitionValueC0)) /
             (int64_t)(int)definitionValueC0) -
       (int)(((int64_t)(int)definitionValueC0 *
              (int64_t)((ModelDefinition *)modelDefinition)->accelerationPerTick) /
             (int64_t)(int)(definitionValue0C << 2));
  *(int *)modelRuntimeSlot->classPrefixState = initialTimingValue;
  modelRuntimeSlot->classState.classStateD0 = initialTimingValue;
  modelRuntimeSlot->classState.effectEmitterTimerTicks = INT32_MAX;
  return;
}


/* Class initializer of model class 17 (modelClassInitialize[17]). Seeds the class state from the root node:
   modelLinkOrState = 0x4000, classState64 = the root's rotation angle 2, classState68 / armyLinkOrState = its
   world X / Y, classState70 = 0x18, and bits 1 and 2 of behaviorState when the root has more than two children.
*/
void ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootModelNode;
  Q12 rootWorldYQ12;

  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  modelRuntimeSlot->classLinkState.modelLinkOrState.classState = FIXED_ANGLE16_QUARTER_TURN;
  modelRuntimeSlot->classLinkState.classState64 =
       rootModelNode->modelPayload.worldRotationAngle2;
  modelRuntimeSlot->classState.behaviorState = 0;
  modelRuntimeSlot->classLinkState.classState70 = 24;
  if (2 < rootModelNode->childCount) {
    modelRuntimeSlot->classState.behaviorState |= 6;
  }
  rootWorldYQ12 = rootModelNode->worldTransform.translation.y;
  modelRuntimeSlot->classLinkState.classState68 = rootModelNode->worldTransform.translation.x;
  modelRuntimeSlot->classLinkState.armyLinkOrState.classState = rootWorldYQ12;
  return;
}


/* Class initializer of model class 9 (modelClassInitialize[9]). Clears classLinkState, then checks the eight packed
   keys 0..7 of key class 2 in the model resource of the root's grandchild: the counter of every missing key
   (modelLinkOrState, classState64, ... classState7C) becomes -1, the others stay 0.
*/
void ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootModelNode;
  ModelRuntimeNode *rootChild0Node;
  ModelRuntimeNode *rootGrandchildNode;

  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  modelRuntimeSlot->classLinkState.classState80 = 0;
  modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime = nullptr;
  rootChild0Node = rootModelNode->childNodes[0];
  modelRuntimeSlot->classLinkState.classState64 = 0;
  modelRuntimeSlot->classLinkState.classState68 = 0;
  rootGrandchildNode = rootChild0Node->childNodes[0];
  modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime = nullptr;
  modelRuntimeSlot->classLinkState.classState70 = 0;
  modelRuntimeSlot->classLinkState.classState74 = 0;
  modelRuntimeSlot->classLinkState.classState78 = 0;
  modelRuntimeSlot->classLinkState.classState7C = 0;
  if (!ModelLookupTable_GetPackedPointPosition
         (0,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.modelLinkOrState.classState -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (1,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.classState64 -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (2,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.classState68 -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (3,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.armyLinkOrState.classState -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (4,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.classState70 -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (5,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.classState74 -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (6,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.classState78 -= 1;
  }
  if (!ModelLookupTable_GetPackedPointPosition
         (7,MODEL_POINT_CLASS_SHOT,rootGrandchildNode->modelPayload.modelResource,nullptr)) {
    modelRuntimeSlot->classLinkState.classState7C -= 1;
  }
  return;
}


/* Class initializer of model class 11 (modelClassInitialize[11]): clears the class fields classState64,
   classState68, classState74 and behaviorState.
*/
void ModelRuntimeSlotClassInit_ResetStructureFactoryBuild
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  modelRuntimeSlot->classState.behaviorState = 0;
  modelRuntimeSlot->classLinkState.classState74 = 0;
  modelRuntimeSlot->classLinkState.classState64 = 0;
  modelRuntimeSlot->classLinkState.classState68 = 0;
  return;
}

/* Unrebase handler of model class 13 (modelUnrebase[13], run by ModelRuntimePool_UnrebaseBeforeSave): before a
   save, turns the army pointer classLinkState.armyLinkOrState into a saved offset relative to g_ArmyRuntimeRebaseBaseMinusOne.
*/
void ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntime)

{
  ArmyRuntimeSlot *linkedArmyRuntime;

  linkedArmyRuntime = modelRuntime->classLinkState.armyLinkOrState.armyRuntime;
  if (linkedArmyRuntime != nullptr) {
    /* 32-bit format field: ModelRuntimeSlot.classLinkState.armyLinkOrState (saved offset) */
    modelRuntime->classLinkState.armyLinkOrState.armyRuntime =
         Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(linkedArmyRuntime) - Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
  }
  return;
}


/* Rebase handler of model class 13 (modelRebaseOrLoadRepair[13], run by ModelRuntimePool_RebaseAfterLoad): after
   a load, turns the saved army offset in classLinkState.armyLinkOrState back into a pointer (offset + g_ArmyRuntimeRebaseBaseMinusOne).
*/
void ModelRuntimeSlot_RebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntimeSlot)

{
  ArmyRuntimeSlot *linkedArmyRuntime;

  linkedArmyRuntime = modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime;
  if (linkedArmyRuntime != nullptr) {
    /* 32-bit format field: ModelRuntimeSlot.classLinkState.armyLinkOrState (saved offset) */
    modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime =
         Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(linkedArmyRuntime) + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
  }
  return;
}


/* Class initializer of model class 13 (modelClassInitialize[13]). Clears the class state (no linked army in
   armyLinkOrState), sets classStateBC to 1 and turns on texture scrolling of the root node for the subresource
   named in the definition's classParameterC0.
*/
void ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootModelNode;
  AssetRecordByteCount primaryAnimatedSubresourceIndex;

  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  modelRuntimeSlot->classState.behaviorState = 0;
  modelRuntimeSlot->classState.classStateBC = 1;
  modelRuntimeSlot->classLinkState.classState74 = 0;
  modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime = nullptr;
  modelRuntimeSlot->classLinkState.classState64 = 0;
  modelRuntimeSlot->classLinkState.classState68 = 0;
  primaryAnimatedSubresourceIndex = ((ModelDefinition *)modelDefinition)->classParameterC0;
  rootModelNode->primaryTextureOffsetU = 0;
  rootModelNode->primaryTextureOffsetV = 0;
  rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
  rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL;
  return;
}


/* Class initializer of model class 14, the resource extractor (modelClassInitialize[14]). Adds the model's
   storage (definition classParameterC4) to its faction's Xenite or Tritium storage limit (selector
   classParameterC0); undone by ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation. Unless the owning
   army's articulatedContact.fallbackPosition1Q12 is 0x6000000 (ARMY_PREVIEW_WORLD_POSITION_Q12), the
   root's fourth child node is unlinked and dropped.
*/
void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *storageLimitQ4;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootModelNode;

  ownerArmy = modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime = nullptr;
  factionRecord = &g_GameFactionRuntimeImage.records[ownerArmy->factionIndex];
  if (((ModelDefinition *)modelDefinition)->classParameterC0 != 0) {
    storageLimitQ4 = &factionRecord->tritiumStorageLimitQ4;
  }
  else {
    storageLimitQ4 = &factionRecord->xeniteStorageLimitQ4;
  }
  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  *storageLimitQ4 += ((ModelDefinition *)modelDefinition)->classParameterC4;
  if ((ownerArmy->articulatedContact.fallbackPosition1Q12 != ARMY_PREVIEW_WORLD_POSITION_Q12) &&
      (3 < rootModelNode->childCount) && (rootModelNode->childNodes[3] != nullptr)) {
    WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)rootModelNode->childNodes[3]);
    rootModelNode->childNodes[3] = nullptr;
  }
  return;
}


/* Class initializer of model class 15, the resource storage (modelClassInitialize[15]). Adds the model's storage
   (definition classParameterC4) to its faction's Xenite or Tritium storage limit (selector classParameterC0);
   undone by ArmyPlacement_ReleaseFactionCapacity. Unless the owning army's articulatedContact.fallbackPosition1Q12
   is 0x6000000 (ARMY_PREVIEW_WORLD_POSITION_Q12), the root's second child node is unlinked and dropped.
*/
void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *storageLimitQ4;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootModelNode;

  ownerArmy = modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  factionRecord = &g_GameFactionRuntimeImage.records[ownerArmy->factionIndex];
  if (((ModelDefinition *)modelDefinition)->classParameterC0 != 0) {
    storageLimitQ4 = &factionRecord->tritiumStorageLimitQ4;
  }
  else {
    storageLimitQ4 = &factionRecord->xeniteStorageLimitQ4;
  }
  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  *storageLimitQ4 += ((ModelDefinition *)modelDefinition)->classParameterC4;
  if ((ownerArmy->articulatedContact.fallbackPosition1Q12 != ARMY_PREVIEW_WORLD_POSITION_Q12) &&
      (1 < rootModelNode->childCount) && (rootModelNode->childNodes[1] != nullptr)) {
    WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)rootModelNode->childNodes[1]);
    rootModelNode->childNodes[1] = nullptr;
  }
  return;
}


/* Class initializer of model class 16, the energy generator (modelClassInitialize[16]): adds the definition's
   generation capacity (classParameterC0, Q4) to the owning faction's energyGenerationCapacityQ4. The stock power plant
   (ARM 310) links four such generator models with 200 each.
*/
void ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  FactionProgressAmountQ4 *factionProgressLimitQ4;

  factionProgressLimitQ4 =
       &g_GameFactionRuntimeImage.records
        [modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex].
        energyGenerationCapacityQ4;
  *factionProgressLimitQ4 = *factionProgressLimitQ4 + ((ModelDefinition *)modelDefinition)->classParameterC0;
  return;
}


/* Release handler of model class 16, the energy generator (modelReleaseOrCommit[16], run by
   ModelRuntimePool_DestroyHierarchyAndDetach): takes the definition's generation capacity (classParameterC0) off the
   owning faction's energyGenerationCapacityQ4 again.
*/
void ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  FactionProgressAmountQ4 *factionProgressLimitQ4;

  factionProgressLimitQ4 =
       &g_GameFactionRuntimeImage.records
        [modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex].
        energyGenerationCapacityQ4;
  *factionProgressLimitQ4 = *factionProgressLimitQ4 - ((ModelDefinition *)modelDefinition)->classParameterC0;
  return;
}


/* Unrebase handler of model class 21, the aircraft (modelUnrebase[21], run by ModelRuntimePool_UnrebaseBeforeSave):
   before a save, turns the linked model runtime classLinkState.modelLinkOrState (its home pad) into a saved
   offset (pointer - g_ModelRuntimeRebaseDelta).
*/
void ModelRuntimeSlot_UnrebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *linkedModelRuntime;

  linkedModelRuntime = modelRuntime->classLinkState.modelLinkOrState.modelRuntime;
  if (linkedModelRuntime != nullptr) {
    /* 32-bit format field: ModelRuntimeSlot.classLinkState.modelLinkOrState (saved offset) */
    modelRuntime->classLinkState.modelLinkOrState.modelRuntime =
         (ModelRuntimeSlot *)(Thandor_PointerToI32(linkedModelRuntime) - g_ModelRuntimeRebaseDelta);
  }
  return;
}


/* Rebase handler of model class 21, the aircraft (modelRebaseOrLoadRepair[21], run by
   ModelRuntimePool_RebaseAfterLoad): after a load, turns the saved offset in classLinkState.modelLinkOrState back into a pointer
   (offset + g_ModelRuntimeRebaseDelta).
*/
void ModelRuntimeSlot_RebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeSlot *linkedModelRuntime;

  linkedModelRuntime = modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime;
  if (linkedModelRuntime != nullptr) {
    modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime =
         (ModelRuntimeSlot *)((uint8_t *)linkedModelRuntime + g_ModelRuntimeRebaseDelta);
  }
  return;
}


/* Class initializer of model class 21, the aircraft (modelClassInitialize[21]): no base linked yet in
   modelLinkOrState, behaviorState cleared, and the local Z of the root's first child set to Q12_ONE.
*/
void ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootChild0Node;
  ModelRuntimeNode *rootModelNode;

  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime = nullptr;
  rootChild0Node = rootModelNode->childNodes[0];
  modelRuntimeSlot->classState.behaviorState = 0;
  rootChild0Node->modelPayload.localTranslationZQ12 = Q12_ONE;
  return;
}

/* Class initializer of model class 22 (modelClassInitialize[22]), the aircraft pad: clears the class
   state including the 13 dwords from classLinkState.classState78 on (the army asset ids
   ArmyPlacement_ReleaseClassStateReservation looks up) and turns on texture scrolling of the root node for the
   subresource named in the definition's classParameterC0.
*/
void ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  int stateDwordsRemaining;
  uint32_t *stateClearCursor;
  ModelRuntimeNode *rootModelNode;
  AssetRecordByteCount primaryAnimatedSubresourceIndex;

  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  primaryAnimatedSubresourceIndex = ((ModelDefinition *)modelDefinition)->classParameterC0;
  modelRuntimeSlot->classState.classStateAC = 0;
  modelRuntimeSlot->classState.classStateB0 = 0;
  modelRuntimeSlot->classState.classStateB4 = 0;
  modelRuntimeSlot->classLinkState.classState74 = 0;
  modelRuntimeSlot->classLinkState.classState64 = 0;
  modelRuntimeSlot->classLinkState.classState68 = 0;
  modelRuntimeSlot->classLinkState.armyLinkOrState.armyRuntime = nullptr;
  modelRuntimeSlot->classLinkState.classState70 = 0;
  modelRuntimeSlot->classState.classStateDC = 0;
  rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL;
  rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
  rootModelNode->primaryTextureOffsetU = 0;
  rootModelNode->primaryTextureOffsetV = 0;
  stateClearCursor = &modelRuntimeSlot->classLinkState.classState78;
  for (stateDwordsRemaining = 13; stateDwordsRemaining != 0; stateDwordsRemaining--) {
    *stateClearCursor++ = 0;
  }
  return;
}


/* Default rebase/load-repair handler (modelRebaseOrLoadRepair, every class except 13 and 21, run by
   ModelRuntimePool_RebaseAfterLoad): those classes keep no pointers in their class state, so this does nothing.
*/
void ModelRuntimeSlotPointerRebase_NoOp(ModelRuntimeSlot *modelRuntimeSlot)

{
  return;
}


/* Default class initializer (modelClassInitialize of classes 0, 1, 4-8, 10 and 18-20): those classes need no
   class state, so this does nothing.
*/
void ModelRuntimeSlotClassInit_NoOp
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  return;
}

/* Class initializer of model class 12 (modelClassInitialize[12]): clears the link classLinkState.modelLinkOrState.
*/
void ModelRuntimeSlotClassInit_ClearField60
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime = nullptr;
  return;
}

/* Class initializer of model class 23 (modelClassInitialize[23]): clears the class fields modelLinkOrState and
   behaviorState.
*/
void ModelRuntimeSlotClassInit_ClearFields60AndB8
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  modelRuntimeSlot->classState.behaviorState = 0;
  modelRuntimeSlot->classLinkState.modelLinkOrState.modelRuntime = nullptr;
  return;
}
