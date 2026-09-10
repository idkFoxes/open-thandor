#include <thandor/world/model/slots.h>

/* Implementation ownership: world/model/slots. */

/* Address: 0x005200C0.
   Ownership: world/model/slots.
   Purpose: Applies definition animation indices to the root model node. Class-initialization callback table slot
   selected by model runtime class id. Role: Applies primary and optional secondary animated texture subresource
   indices from the MDL definition. Inputs: Class-selected model definition fields and root ModelRuntimeNode.
   Outputs: Root animation flags, subresource indices and zeroed UV offsets.
*/
void ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices
               (ModelDefinitionRuntimeSemanticView280 *modelDefinition,
               ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootModelNode;
  ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
  AssetRecordByteCount secondaryAnimatedSubresourceIndex;
  
  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  primaryAnimatedSubresourceIndex = modelDefinition->primaryAnimatedSubresourceIndex1B8;
  secondaryAnimatedSubresourceIndex = modelDefinition->secondaryAnimatedSubresourceIndex1BC;
  if (primaryAnimatedSubresourceIndex != 0) {
    rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | 0x80;
    rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
    rootModelNode->primaryTextureOffsetU = 0;
    rootModelNode->primaryTextureOffsetV = 0;
    if (secondaryAnimatedSubresourceIndex != 0) {
      rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | 0x400;
      rootModelNode->secondaryAnimatedSubresourceIndex = secondaryAnimatedSubresourceIndex;
      rootModelNode->secondaryTextureOffsetU = 0;
      rootModelNode->secondaryTextureOffsetV = 0;
    }
  }
  return;
}

/* Address: 0x00522A90.
   Ownership: world/model/slots.
   Purpose: Initializes class-specific sentinels, bounds, and timing fields. Class-initialization callback table
   slot selected by model runtime class id.
*/
void ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  uint uVar1;
  AssetRecordByteCount AVar2;
  int iVar3;
  ModelRuntimeNode *rootChild0Node;
  Q12 grandchildLocalYQ12;
  
  uVar1 = modelDefinition[1].byteSize;
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
  AVar2 = modelDefinition[0x10].byteSize;
  iVar3 = (int)(((longlong)
                 (int)(CONCAT44(((int)uVar1 >> 0x1f) << 0xc | uVar1 >> 0x14,uVar1 << 0xc) /
                      (longlong)(int)AVar2) * (longlong)(int)(modelDefinition[0x10].flags + AVar2))
               / (longlong)(int)AVar2) -
          (int)(((longlong)(int)AVar2 * (longlong)(int)modelDefinition[2].byteSize) /
               (longlong)(int)(uVar1 << 2));
  *(int *)modelRuntimeSlot->reserved10_37 = iVar3;
  (modelRuntimeSlot->classState).classStateD0 = iVar3;
  (modelRuntimeSlot->classState).enabledStateE8 = 0x7fffffff;
  return;
}

/* Address: 0x00522B90.
   Ownership: world/model/slots.
   Purpose: Seeds class fields from the root model transform. Class-initialization callback table slot selected by
   model runtime class id.
*/
void ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  dword *classStateFlags;
  ModelRuntimeNode *rootModelNode;
  ModelRuntimeArmyLinkOrState4 rootWorldYQ12;
  
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
  rootWorldYQ12 = (ModelRuntimeArmyLinkOrState4)(rootModelNode->worldTransform).translation.y;
  (modelRuntimeSlot->classLinkState).classState68 = (rootModelNode->worldTransform).translation.x;
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C = rootWorldYQ12;
  return;
}

/* Address: 0x00523CA0.
   Ownership: world/model/slots.
   Purpose: Builds eight model-key presence counters in the class-state overlay. Class-initialization callback
   table slot selected by model runtime class id.
   Cross-module calls: ModelLookupTable_FindPackedKeyEntryRegsCf [assets/model/definitions].
*/
void ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  dword *pdVar1;
  ModelRuntimeArmyLinkOrState4 *pMVar2;
  undefined1 in_CF;
  dword *matchedClassCounterField;
  ModelRuntimeArmyLinkOrState4 *class6CCounterField;
  ModelRuntimeNode *modelKeyLookupNode;
  ModelRuntimeNode *modelNode1;
  
  modelKeyLookupNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classLinkState).classState80 = 0;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
  modelNode1 = modelKeyLookupNode->childNodes[0];
  (modelRuntimeSlot->classLinkState).classState64 = 0;
  (modelRuntimeSlot->classLinkState).classState68 = 0;
  modelNode1 = modelNode1->childNodes[0];
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime = (ArmyRuntimeSlot *)0x0;
  (modelRuntimeSlot->classLinkState).classState70 = 0;
  (modelRuntimeSlot->classLinkState).classState74 = 0;
  (modelRuntimeSlot->classLinkState).classState78 = 0;
  (modelRuntimeSlot->classLinkState).classState7C = 0;
  ModelLookupTable_FindPackedKeyEntryRegsCf(0,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)((modelRuntimeSlot->classLinkState).modelLinkOrState60.classState - 1);
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(1,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    matchedClassCounterField = &(modelRuntimeSlot->classLinkState).classState64;
    *matchedClassCounterField = *matchedClassCounterField - 1;
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(2,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    pdVar1 = &(modelRuntimeSlot->classLinkState).classState68;
    *pdVar1 = *pdVar1 - 1;
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(3,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    pMVar2 = &(modelRuntimeSlot->classLinkState).armyLinkOrState6C;
    pMVar2->armyRuntime = (ArmyRuntimeSlot *)(pMVar2->classState - 1);
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(4,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    pdVar1 = &(modelRuntimeSlot->classLinkState).classState70;
    *pdVar1 = *pdVar1 - 1;
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(5,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    pdVar1 = &(modelRuntimeSlot->classLinkState).classState74;
    *pdVar1 = *pdVar1 - 1;
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(6,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    pdVar1 = &(modelRuntimeSlot->classLinkState).classState78;
    *pdVar1 = *pdVar1 - 1;
  }
  ModelLookupTable_FindPackedKeyEntryRegsCf(7,2,(modelNode1->modelPayload).modelResource);
  if ((bool)in_CF) {
    pdVar1 = &(modelRuntimeSlot->classLinkState).classState7C;
    *pdVar1 = *pdVar1 - 1;
  }
  return;
}

/* Address: 0x005243D0.
   Ownership: world/model/slots.
   Purpose: Clears four class-state fields. Class-initialization callback table slot selected by model runtime
   class id.
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
   Ownership: world/model/slots.
   Purpose: Converts the class-specific army link at offset 0x6C to a saved pool offset. Model-unrebase partition
   slots 48-71 receive one ModelRuntimeSlot pointer.
*/
void ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntime)

{
  ArmyRuntimeSlot *linkedArmyRuntime;
  
  linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime;
  if (linkedArmyRuntime != (ArmyRuntimeSlot *)0x0) {
    (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime =
         (ArmyRuntimeSlot *)((int)linkedArmyRuntime - (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  return;
}

/* Address: 0x00524CE0.
   Ownership: world/model/slots.
   Purpose: Rebases the class-specific army link at offset 0x6C. Pointer-rebase callback table slot selected by
   model runtime class id.
*/
void ModelRuntimeSlot_RebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntimeSlot)

{
  ArmyRuntimeSlot *linkedArmyRuntime;
  
  linkedArmyRuntime = (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime;
  if (linkedArmyRuntime != (ArmyRuntimeSlot *)0x0) {
    (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime =
         (ArmyRuntimeSlot *)
         ((int)&linkedArmyRuntime->definitionOrAsset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  return;
}

/* Address: 0x00524D10.
   Ownership: world/model/slots.
   Purpose: Enables root animation and copies the definition C0 value. Class-initialization callback table slot
   selected by model runtime class id. Role: Clears class state, copies MDL +0xC0 into the root primary animation
   selector and enables animation. Inputs: Class-selected MDL definition and new ModelRuntimeSlot. Outputs:
   Initialized class state and root animated subresource.
*/
void ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootModelNode;
  AssetRecordByteCount primaryAnimatedSubresourceIndex;
  
  rootModelNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classState).reservedBC_CF[0] = 1;
  (modelRuntimeSlot->classState).reservedBC_CF[1] = 0;
  (modelRuntimeSlot->classState).reservedBC_CF[2] = 0;
  (modelRuntimeSlot->classState).reservedBC_CF[3] = 0;
  (modelRuntimeSlot->classLinkState).classState74 = 0;
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime = (ArmyRuntimeSlot *)0x0;
  (modelRuntimeSlot->classLinkState).classState64 = 0;
  (modelRuntimeSlot->classLinkState).classState68 = 0;
  primaryAnimatedSubresourceIndex = modelDefinition[0x10].byteSize;
  rootModelNode->primaryTextureOffsetU = 0;
  rootModelNode->primaryTextureOffsetV = 0;
  rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
  rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | 0x80;
  return;
}

/* Address: 0x005251C0.
   Ownership: world/model/slots.
   Purpose: Accumulates a faction metric and detaches root child slot 3. Class-initialization callback table slot
   selected by model runtime class id.
   Cross-module calls: WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core].
*/
void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode1;
  
  armySlot1 = (modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
  iVar2 = armySlot1->factionIndex * 0x740;
  iVar3 = iVar2 + 4;
  if (modelDefinition[0x10].byteSize != 0) {
    iVar3 = iVar2 + 0x14;
  }
  modelNode1 = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  pbVar1 = g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar3 + -0x78;
  *(dword *)pbVar1 = *(int *)pbVar1 + modelDefinition[0x10].flags;
  if ((((armySlot1->articulatedContact).fallbackPosition1Q12 != 0x6000000) &&
      (3 < modelNode1->childCount)) && (modelNode1->childNodes[3] != (ModelRuntimeNode *)0x0)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode1->childNodes[3]);
    modelNode1->childNodes[3] = (ModelRuntimeNode *)0x0;
  }
  return;
}

/* Address: 0x00525250.
   Ownership: world/model/slots.
   Purpose: Class initializer selected by model class 15. Accumulates definition C4 into the owning faction metric
   and detaches root child slot 1. The normal EDX:EAX value is preserved pass-through state and is not consumed by
   the table caller. Class-initialization callback table slot selected by model runtime class id.
   Cross-module calls: WorldRuntime_UnlinkNodeFromOwnerListD8 [world/runtime/core].
*/
void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode1;
  
  armySlot1 = (modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  iVar2 = armySlot1->factionIndex * 0x740;
  iVar3 = iVar2 + 4;
  if (modelDefinition[0x10].byteSize != 0) {
    iVar3 = iVar2 + 0x14;
  }
  modelNode1 = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  pbVar1 = g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar3 + -0x78;
  *(dword *)pbVar1 = *(int *)pbVar1 + modelDefinition[0x10].flags;
  if ((((armySlot1->articulatedContact).fallbackPosition1Q12 != 0x6000000) &&
      (1 < modelNode1->childCount)) && (modelNode1->childNodes[1] != (ModelRuntimeNode *)0x0)) {
    WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode1->childNodes[1]);
    modelNode1->childNodes[1] = (ModelRuntimeNode *)0x0;
  }
  return;
}

/* Address: 0x005252E0.
   Ownership: world/model/slots.
   Purpose: Accumulates the definition value into the owning faction progress limit. Class-initialization callback
   table slot selected by model runtime class id. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Model class 16
   Energy-generator init callback. Adds class-16 MDL +0xC0 to owning faction energyGenerationCapacityQ4. Stock
   Power plant (ARM 310) has four linked class-16 generator MDLs with +0xC0=200 Q4 each.
*/
void ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity
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
   Ownership: world/model/slots.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FE78[16]@0051FE78. Model release partition slots
   0-23 receive (modelDefinition, modelRuntime). [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Model class 16
   release callback. Subtracts class-16 MDL +0xC0 from owning faction energyGenerationCapacityQ4; exact inverse of
   the class-16 initialization callback.
*/
void ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity
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
   Ownership: world/model/slots.
   Purpose: Converts the class-specific model-runtime link at offset 0x60 to a saved pool offset. Model-unrebase
   partition slots 48-71 receive one ModelRuntimeSlot pointer.
*/
void ModelRuntimeSlot_UnrebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *linkedModelRuntime;
  
  linkedModelRuntime = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  if (linkedModelRuntime != (ModelRuntimeSlot *)0x0) {
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)((int)linkedModelRuntime - g_ModelRuntimeRebaseDelta);
  }
  return;
}

/* Address: 0x00526370.
   Ownership: world/model/slots.
   Purpose: Rebases the class-specific model-runtime link at offset 0x60. Pointer-rebase callback table slot
   selected by model runtime class id.
*/
void ModelRuntimeSlot_RebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeSlot *linkedModelRuntime;
  
  linkedModelRuntime = (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime;
  if (linkedModelRuntime != (ModelRuntimeSlot *)0x0) {
    (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)(linkedModelRuntime->reserved10_37 + g_ModelRuntimeRebaseDelta + -0x10)
    ;
  }
  return;
}

/* Address: 0x005263A0.
   Ownership: world/model/slots.
   Purpose: Clears class state and sets root child 0 local Z to 0x1000. Class-initialization callback table slot
   selected by model runtime class id.
*/
void ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  ModelRuntimeNode *rootGrandchildNode;
  ModelRuntimeNode *rootChildNode;
  
  rootChildNode = (modelRuntimeSlot->rootModelNodeOrSavedOffset).modelNode;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
  rootGrandchildNode = rootChildNode->childNodes[0];
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (rootGrandchildNode->modelPayload).localTranslationZQ12 = 0x1000;
  return;
}

/* Address: 0x00526E00.
   Ownership: world/model/slots.
   Purpose: Clears extended class state and enables root animation. Class-initialization callback table slot
   selected by model runtime class id. Role: Clears extended class state and enables the root animation channel.
   Inputs: Class-selected model definition and new ModelRuntimeSlot. Outputs: Reset class state plus root animation
   flags/selector.
*/
void ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  int stateDwordsRemaining;
  dword *stateClearCursor;
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
  (modelRuntimeSlot->classLinkState).armyLinkOrState6C.armyRuntime = (ArmyRuntimeSlot *)0x0;
  (modelRuntimeSlot->classLinkState).classState70 = 0;
  (modelRuntimeSlot->classState).classStateDC = 0;
  rootModelNode->runtimeFlags = rootModelNode->runtimeFlags | 0x80;
  rootModelNode->primaryAnimatedSubresourceIndex = primaryAnimatedSubresourceIndex;
  rootModelNode->primaryTextureOffsetU = 0;
  rootModelNode->primaryTextureOffsetV = 0;
  stateClearCursor = &(modelRuntimeSlot->classLinkState).classState78;
  for (stateDwordsRemaining = 0xd; stateDwordsRemaining != 0;
      stateDwordsRemaining = stateDwordsRemaining + -1) {
    *stateClearCursor = 0;
    stateClearCursor = stateClearCursor + 1;
  }
  return;
}

/* Address: 0x00527B80.
   Ownership: world/model/slots.
   Purpose: Fourth exact one-argument no-op reused across many unified runtime object method-table entries. It
   returns with ret 0x04. Pointer-rebase callback table slot selected by model runtime class id.
*/
void ModelRuntimeSlotPointerRebase_NoOp(ModelRuntimeSlot *modelRuntimeSlot)

{
  return;
}

/* Address: 0x00527B90.
   Ownership: world/model/slots.
   Purpose: Exact two-argument no-op reused across unified runtime object method tables. It returns with ret 0x08.
   Class-initialization callback table slot selected by model runtime class id.
*/
void ModelRuntimeSlotClassInit_NoOp
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  return;
}

/* Address: 0x005283B0.
   Ownership: world/model/slots.
   Purpose: Clears class-state field 0x60. Class-initialization callback table slot selected by model runtime class
   id.
*/
void ModelRuntimeSlotClassInit_ClearField60
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
  return;
}

/* Address: 0x005285D0.
   Ownership: world/model/slots.
   Purpose: Clears class-state fields 0x60 and 0xB8. Class-initialization callback table slot selected by model
   runtime class id.
*/
void ModelRuntimeSlotClassInit_ClearFields60AndB8
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot)

{
  (modelRuntimeSlot->classState).classStateB8 = 0;
  (modelRuntimeSlot->classLinkState).modelLinkOrState60.modelRuntime = (ModelRuntimeSlot *)0x0;
  return;
}
