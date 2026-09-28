/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/slots.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/slots.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/model/slots.

   Per-class model runtime callbacks from g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes (0x0051FC98), indexed
   by the model definition's class id (+0x4C): modelClassInitialize (run by
   ModelRuntimePool_CreateInstanceByDefinitionId), modelReleaseOrCommit (ModelRuntimePool_DestroyHierarchyAndDetach),
   modelUnrebase (ModelRuntimePool_UnrebaseBeforeSave) and modelRebaseOrLoadRepair
   (ModelRuntimePool_RebaseAfterLoad). The class state at +0x60..+0xEC of a ModelRuntimeSlot means something
   different for every class. */

/* Address: 0x005200C0.
   Class initializer of model class 2 (modelClassInitialize[2]). Turns on texture scrolling for the root node:
   the definition's primary animated subresource (+0x1B8) and, only together with it, the secondary one (+0x1BC),
   both starting at texture offset 0.
*/
void __thandor_preserve_eax_edx
ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices
          (ModelDefinitionRuntimeSemanticView280 *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot
          )

{
  ModelRuntimeNode *rootModelNode;
  ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
  AssetRecordByteCount secondaryAnimatedSubresourceIndex;

  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  primaryAnimatedSubresourceIndex = modelDefinition->primaryAnimatedSubresourceIndex1B8;
  secondaryAnimatedSubresourceIndex = modelDefinition->secondaryAnimatedSubresourceIndex1BC;
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


/* Address: 0x00522A90.
   Class initializer of model class 3 (modelClassInitialize[3]). Marks the class fields +0x68..+0x74 as unset
   (0x80000000) and +0x78 / +0xE8 as 0x7FFFFFFF, records the local Y of the root's first child and grandchild,
   and derives a starting timer value (stored at +0x10 and +0xD0) from the definition values at +0x0C, +0x18,
   +0xC0 and +0xC4.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint32_t definitionValue0C;
  AssetRecordByteCount definitionValueC0;
  int initialTimingValue;
  ModelRuntimeNode *rootChild0Node;
  Q12 grandchildLocalYQ12;

  /* modelDefinition[i] is 12 bytes: [1].byteSize is +0x0C, [2].byteSize +0x18, [0x10] +0xC0 / +0xC4 */
  definitionValue0C = modelDefinition[1].byteSize;
  (modelRuntimeSlot->classLinkState).classState68 = 0x80000000;
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.classState = 0x80000000;
  (modelRuntimeSlot->classLinkState).classState70 = 0x80000000;
  (modelRuntimeSlot->classLinkState).classState74 = 0x80000000;
  (modelRuntimeSlot->classState).classStateA8 = 0;
  (modelRuntimeSlot->classState).classStateAC = 0;
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classLinkState).classState78 = 0x7fffffff;
  rootChild0Node = ((modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode)->childNodes[0];
  grandchildLocalYQ12 = (rootChild0Node->childNodes[0]->modelPayload).localTranslationYQ12;
  (modelRuntimeSlot->classState).classStateB0 = (rootChild0Node->modelPayload).localTranslationYQ12;
  (modelRuntimeSlot->classState).classStateB4 = grandchildLocalYQ12;
  definitionValueC0 = modelDefinition[0x10].byteSize;
  initialTimingValue = (int)(((int64_t)
                 (int)(((int64_t)(int)definitionValue0C << 12) /
                      (int64_t)(int)definitionValueC0) * (int64_t)(int)(modelDefinition[0x10].flags + definitionValueC0))
               / (int64_t)(int)definitionValueC0) -
          (int)(((int64_t)(int)definitionValueC0 * (int64_t)(int)modelDefinition[2].byteSize) /
               (int64_t)(int)(definitionValue0C << 2));
  *(int *)modelRuntimeSlot->reserved10_37 = initialTimingValue;
  (modelRuntimeSlot->classState).classStateD0 = initialTimingValue;
  (modelRuntimeSlot->classState).enabledStateE8 = 0x7fffffff;
  return;
}


/* Address: 0x00522B90.
   Class initializer of model class 17 (modelClassInitialize[17]). Seeds the class state from the root node:
   +0x60 = 0x4000, +0x64 = the root's rotation angle 2, +0x68 / +0x6C = its world X / Y, +0x70 = 0x18, and
   bits 1 and 2 of +0xB8 when the root has more than two children.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint32_t *classStateFlags;
  ModelRuntimeNode *rootModelNode;
  Q12 rootWorldYQ12;

  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.classState = 0x4000;
  (modelRuntimeSlot->classLinkState).classState64 =
       (rootModelNode->modelPayload).worldRotationAngle2;
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classLinkState).classState70 = 0x18;
  if (2 < rootModelNode->childCount) {
    classStateFlags = &(modelRuntimeSlot->classState).classStateB8;
    *classStateFlags = *classStateFlags | 6;
  }
  rootWorldYQ12 = (rootModelNode->worldTransform).translation.y;
  (modelRuntimeSlot->classLinkState).classState68 = (rootModelNode->worldTransform).translation.x;
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.classState = rootWorldYQ12;
  return;
}


/* Address: 0x00523CA0.
   Class initializer of model class 9 (modelClassInitialize[9]). Clears +0x60..+0x80, then checks the eight packed
   keys 0..7 of key class 2 in the model resource of the root's grandchild: the counter of every missing key
   (+0x60, +0x64, ... +0x7C) becomes -1, the others stay 0.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint32_t *classCounterField;
  ModelRuntimeArmyLinkOrState4 *armyLinkCounterField;
  ModelLookupPayloadResult keyLookupResult;
  uint32_t *matchedClassCounterField;
  ModelRuntimeNode *rootModelNode;
  ModelRuntimeNode *rootGrandchildNode;

  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classLinkState).classState80 = 0;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = NULL;
  rootGrandchildNode = rootModelNode->childNodes[0];
  (modelRuntimeSlot->classLinkState).classState64 = 0;
  (modelRuntimeSlot->classLinkState).classState68 = 0;
  rootGrandchildNode = rootGrandchildNode->childNodes[0];
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime = NULL;
  (modelRuntimeSlot->classLinkState).classState70 = 0;
  (modelRuntimeSlot->classLinkState).classState74 = 0;
  (modelRuntimeSlot->classLinkState).classState78 = 0;
  (modelRuntimeSlot->classLinkState).classState7C = 0;
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(0,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)((modelRuntimeSlot->classLinkState).modelLinkOrState60.classState - 1);
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(1,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    matchedClassCounterField = &(modelRuntimeSlot->classLinkState).classState64;
    *matchedClassCounterField = *matchedClassCounterField - 1;
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(2,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    classCounterField = &(modelRuntimeSlot->classLinkState).classState68;
    *classCounterField = *classCounterField - 1;
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(3,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    armyLinkCounterField = &(modelRuntimeSlot->classLinkState).armyLinkOrState6C;
    armyLinkCounterField->armyRuntime = (ArmyRuntimeSlot *)(armyLinkCounterField->classState - 1);
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(4,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    classCounterField = &(modelRuntimeSlot->classLinkState).classState70;
    *classCounterField = *classCounterField - 1;
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(5,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    classCounterField = &(modelRuntimeSlot->classLinkState).classState74;
    *classCounterField = *classCounterField - 1;
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(6,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    classCounterField = &(modelRuntimeSlot->classLinkState).classState78;
    *classCounterField = *classCounterField - 1;
  }
  keyLookupResult = ModelLookupTable_FindPackedKeyEntryRegs(7,2,(rootGrandchildNode->modelPayload).modelResource);
  if (keyLookupResult.notFound) {
    classCounterField = &(modelRuntimeSlot->classLinkState).classState7C;
    *classCounterField = *classCounterField - 1;
  }
  return;
}


/* Address: 0x005243D0.
   Class initializer of model class 11 (modelClassInitialize[11]): clears the class fields +0x64, +0x68, +0x74
   and +0xB8.
*/
void ModelRuntimeSlotClassInit_ClearFields64_68_74_B8
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classLinkState).classState74 = 0;
  (modelRuntimeSlot->classLinkState).classState64 = 0;
  (modelRuntimeSlot->classLinkState).classState68 = 0;
  return;
}

/* Address: 0x00524CB0.
   Unrebase handler of model class 13 (modelUnrebase[13], run by ModelRuntimePool_UnrebaseBeforeSave): before a
   save, turns the army pointer at +0x6C into a saved offset relative to g_ArmyRuntimeRebaseBaseMinusOne.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntime)

{
  ArmyRuntimeSlot *linkedArmyRuntime;

  linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
  if (linkedArmyRuntime != NULL) {
    (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime =
         (ArmyRuntimeSlot *)((int)linkedArmyRuntime - (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  return;
}


/* Address: 0x00524CE0.
   Rebase handler of model class 13 (modelRebaseOrLoadRepair[13], run by ModelRuntimePool_RebaseAfterLoad): after
   a load, turns the saved army offset at +0x6C back into a pointer (offset + g_ArmyRuntimeRebaseBaseMinusOne).
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlot_RebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntimeSlot)

{
  ArmyRuntimeSlot *linkedArmyRuntime;

  linkedArmyRuntime = (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime;
  if (linkedArmyRuntime != NULL) {
    /* the address of modelRuntimeOrSavedOffset (+0) is the pointer value itself, i.e. the saved offset */
    (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime =
         (ArmyRuntimeSlot *)
         ((int)&linkedArmyRuntime->modelRuntimeOrSavedOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne)
    ;
  }
  return;
}


/* Address: 0x00524D10.
   Class initializer of model class 13 (modelClassInitialize[13]). Clears the class state (no linked army at
   +0x6C), sets byte +0xBC to 1 and turns on texture scrolling of the root node for the subresource named at
   definition +0xC0.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootModelNode;
  AssetRecordByteCount primaryAnimatedSubresourceIndex;

  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classState).reservedBC_BF[0] = 1;
  (modelRuntimeSlot->classState).reservedBC_BF[1] = 0;
  (modelRuntimeSlot->classState).reservedBC_BF[2] = 0;
  (modelRuntimeSlot->classState).reservedBC_BF[3] = 0;
  (modelRuntimeSlot->classLinkState).classState74 = 0;
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime = NULL;
  (modelRuntimeSlot->classLinkState).classState64 = 0;
  (modelRuntimeSlot->classLinkState).classState68 = 0;
  primaryAnimatedSubresourceIndex = modelDefinition[0x10].byteSize;
  rootModelNode->primaryTextureOffsetU = 0;
  rootModelNode->primaryTextureOffsetV = 0;
  rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
  rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL;
  return;
}


/* Address: 0x005251C0.
   Class initializer of model class 14, the resource extractor (modelClassInitialize[14]). Adds the model's
   storage (definition +0xC4) to its faction's Xenite or Tritium storage limit (selector +0xC0); undone by
   ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation. Unless the owning army's +0xBC is 0x6000000, the
   root's fourth child node is unlinked and dropped.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint8_t *storageLimit;
  int factionRecordOffset;
  int storageLimitOffset;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootModelNode;

  ownerArmy = (modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = NULL;
  factionRecordOffset = ownerArmy->factionIndex * GAME_FACTION_RUNTIME_RECORD_BYTES;
  storageLimitOffset = factionRecordOffset + 4; /* xeniteStorageLimitQ4 */
  if (modelDefinition[0x10].byteSize != 0) {
    storageLimitOffset = factionRecordOffset + 0x14; /* tritiumStorageLimitQ4 */
  }
  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  storageLimit = g_GameFactionRuntimeImage.records[0].reserved78_87 + storageLimitOffset - 0x78;
  *(uint32_t *)storageLimit = *(int *)storageLimit + modelDefinition[0x10].flags;
  if ((((ownerArmy->articulatedContact).fallbackPosition1Q12 != 0x6000000) &&
      (3 < rootModelNode->childCount)) && (rootModelNode->childNodes[3] != NULL)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)rootModelNode->childNodes[3]);
    rootModelNode->childNodes[3] = NULL;
  }
  return;
}


/* Address: 0x00525250.
   Class initializer of model class 15, the resource storage (modelClassInitialize[15]). Adds the model's storage
   (definition +0xC4) to its faction's Xenite or Tritium storage limit (selector +0xC0); undone by
   ArmyPlacement_ReleaseFactionCapacity. Unless the owning army's +0xBC is 0x6000000, the root's second child
   node is unlinked and dropped.
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint8_t *storageLimit;
  int factionRecordOffset;
  int storageLimitOffset;
  ArmyRuntimeSlot *ownerArmy;
  ModelRuntimeNode *rootModelNode;

  ownerArmy = (modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  factionRecordOffset = ownerArmy->factionIndex * GAME_FACTION_RUNTIME_RECORD_BYTES;
  storageLimitOffset = factionRecordOffset + 4; /* xeniteStorageLimitQ4 */
  if (modelDefinition[0x10].byteSize != 0) {
    storageLimitOffset = factionRecordOffset + 0x14; /* tritiumStorageLimitQ4 */
  }
  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  storageLimit = g_GameFactionRuntimeImage.records[0].reserved78_87 + storageLimitOffset - 0x78;
  *(uint32_t *)storageLimit = *(int *)storageLimit + modelDefinition[0x10].flags;
  if ((((ownerArmy->articulatedContact).fallbackPosition1Q12 != 0x6000000) &&
      (1 < rootModelNode->childCount)) && (rootModelNode->childNodes[1] != NULL)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)rootModelNode->childNodes[1]);
    rootModelNode->childNodes[1] = NULL;
  }
  return;
}


/* Address: 0x005252E0.
   Class initializer of model class 16, the energy generator (modelClassInitialize[16]): adds the definition's
   generation capacity (+0xC0, Q4) to the owning faction's energyGenerationCapacityQ4. The stock power plant
   (ARM 310) links four such generator models with 200 each.
*/
void __thandor_preserve_eax
ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  FactionProgressAmountQ4 *factionProgressLimitQ4;

  factionProgressLimitQ4 =
       &g_GameFactionRuntimeImage.records
        [((modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex].
        energyGenerationCapacityQ4;
  *factionProgressLimitQ4 = *factionProgressLimitQ4 + modelDefinition[0x10].byteSize;
  return;
}


/* Address: 0x005254B0.
   Release handler of model class 16, the energy generator (modelReleaseOrCommit[16], run by
   ModelRuntimePool_DestroyHierarchyAndDetach): takes the definition's generation capacity (+0xC0) off the
   owning faction's energyGenerationCapacityQ4 again.
*/
void __thandor_preserve_eax
ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  FactionProgressAmountQ4 *factionProgressLimitQ4;

  factionProgressLimitQ4 =
       &g_GameFactionRuntimeImage.records
        [((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex].
        energyGenerationCapacityQ4;
  *factionProgressLimitQ4 = *factionProgressLimitQ4 - modelDefinition[0x10].byteSize;
  return;
}


/* Address: 0x00526340.
   Unrebase handler of model class 21, the aircraft (modelUnrebase[21], run by ModelRuntimePool_UnrebaseBeforeSave):
   before a save, turns the linked model runtime at +0x60 (presumably its base) into a saved offset (pointer -
   g_ModelRuntimeRebaseDelta).
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlot_UnrebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *linkedModelRuntime;

  linkedModelRuntime = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  if (linkedModelRuntime != NULL) {
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)((int)linkedModelRuntime - g_ModelRuntimeRebaseDelta);
  }
  return;
}


/* Address: 0x00526370.
   Rebase handler of model class 21, the aircraft (modelRebaseOrLoadRepair[21], run by
   ModelRuntimePool_RebaseAfterLoad): after a load, turns the saved offset at +0x60 back into a pointer
   (offset + g_ModelRuntimeRebaseDelta).
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlot_RebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeSlot *linkedModelRuntime;

  linkedModelRuntime = (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime;
  if (linkedModelRuntime != NULL) {
    /* reserved10_37 - 0x10 is the saved offset itself (byte pointer arithmetic) */
    (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)(linkedModelRuntime->reserved10_37 + g_ModelRuntimeRebaseDelta - 0x10)
    ;
  }
  return;
}


/* Address: 0x005263A0.
   Class initializer of model class 21, the aircraft (modelClassInitialize[21]): no base linked yet at +0x60,
   +0xB8 cleared, and the local Z of the root's first child set to Q12_ONE.
*/
void ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootChild0Node;
  ModelRuntimeNode *rootModelNode;

  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = NULL;
  rootChild0Node = rootModelNode->childNodes[0];
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (rootChild0Node->modelPayload).localTranslationZQ12 = Q12_ONE;
  return;
}

/* Address: 0x00526E00.
   Class initializer of model class 22 (modelClassInitialize[22]), presumably the aircraft base: clears the class
   state including the 13 slots at +0x78..+0xAB (the army asset ids ArmyPlacement_ReleaseClassStateReservation
   looks up) and turns on texture scrolling of the root node for the subresource named at definition +0xC0.
*/
void __thandor_void_preserve_eax_ecx
ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  int stateDwordsRemaining;
  uint32_t *stateClearCursor;
  ModelRuntimeNode *rootModelNode;
  AssetRecordByteCount primaryAnimatedSubresourceIndex;

  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  primaryAnimatedSubresourceIndex = modelDefinition[0x10].byteSize;
  (modelRuntimeSlot->classState).classStateAC = 0;
  (modelRuntimeSlot->classState).classStateB0 = 0;
  (modelRuntimeSlot->classState).classStateB4 = 0;
  (modelRuntimeSlot->classLinkState).classState74 = 0;
  (modelRuntimeSlot->classLinkState).classState64 = 0;
  (modelRuntimeSlot->classLinkState).classState68 = 0;
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime = NULL;
  (modelRuntimeSlot->classLinkState).classState70 = 0;
  (modelRuntimeSlot->classState).classStateDC = 0;
  rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL;
  rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
  rootModelNode->primaryTextureOffsetU = 0;
  rootModelNode->primaryTextureOffsetV = 0;
  stateClearCursor = &(modelRuntimeSlot->classLinkState).classState78;
  for (stateDwordsRemaining = 13; stateDwordsRemaining != 0;
      stateDwordsRemaining = stateDwordsRemaining - 1) {
    *stateClearCursor = 0;
    stateClearCursor = stateClearCursor + 1;
  }
  return;
}


/* Address: 0x00527B80.
   Default rebase/load-repair handler (modelRebaseOrLoadRepair, every class except 13 and 21, run by
   ModelRuntimePool_RebaseAfterLoad): those classes keep no pointers in their class state, so this does nothing
   (RET 4).
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeSlotPointerRebase_NoOp(ModelRuntimeSlot *modelRuntimeSlot)

{
  return;
}


/* Address: 0x00527B90.
   Default class initializer (modelClassInitialize of classes 0, 1, 4-8, 10 and 18-20): those classes need no
   class state, so this does nothing (RET 8).
*/
void ModelRuntimeSlotClassInit_NoOp
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  return;
}

/* Address: 0x005283B0.
   Class initializer of model class 12 (modelClassInitialize[12]): clears the link at +0x60.
*/
void ModelRuntimeSlotClassInit_ClearField60
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = NULL;
  return;
}

/* Address: 0x005285D0.
   Class initializer of model class 23 (modelClassInitialize[23]): clears the class fields +0x60 and +0xB8.
*/
void ModelRuntimeSlotClassInit_ClearFields60AndB8
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = NULL;
  return;
}
