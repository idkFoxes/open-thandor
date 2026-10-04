/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/pool.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

ModelRuntimeSlot *g_ModelRuntimeSlots = nullptr;

intptr_t g_ModelRuntimeRebaseDelta = 0;

/* Implementation ownership: world/model/pool. */

/* Attaches a new model to one of the attachment points of modelRuntime (recorded by
   ModelNodeRuntime_CreateHierarchyRecursive): creates the model childDefinitionId for the same army, stores it in
   the attachment entry, hangs its root node into the parent node's child slot and gives it the saved local
   rotation and the attachment translation. Returns true and stores the child's model runtime in
   *outChildModelRuntime, or false (leaving it unchanged) when the model could not be created.
*/
Bool8 ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot **outChildModelRuntime)

{
  ModelAttachmentTransformRecord *sourceTransform;
  AngleTurn32 rotationAngle0;
  AngleTurn32 rotationAngle1;
  AngleTurn32 rotationAngle2;
  Q12 translationX;
  Q12 translationY;
  ModelRuntimeSlot *childModelRuntime;
  ModelRuntimeNode *parentModelNode;
  ModelRuntimeNode *childRootNode;

  if (attachmentIndex < modelRuntime->attachmentCount) {
    if (ModelRuntimePool_CreateInstanceByDefinitionId
                      (paletteAsset,textureSet,
                       modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime,childDefinitionId,
                       worldRuntime,&childModelRuntime) != 0) {
      return false;
    }
    modelRuntime->attachments[attachmentIndex].childModelRuntimeOrSavedOffset = childModelRuntime;
    parentModelNode = modelRuntime->attachments[attachmentIndex].parentModelNodeOrSavedOffset;
    sourceTransform = modelRuntime->attachments[attachmentIndex].sourceTransform;
    childRootNode = (childModelRuntime->rootModelNodeOrSavedOffset).modelNode;
    rotationAngle0 = modelRuntime->attachments[attachmentIndex].childLocalRotationAngle0;
    rotationAngle1 = modelRuntime->attachments[attachmentIndex].childLocalRotationAngle1;
    rotationAngle2 = modelRuntime->attachments[attachmentIndex].childLocalRotationAngle2;
    parentModelNode->childNodes[modelRuntime->attachments[attachmentIndex].childNodeIndex] =
         childRootNode;
    childRootNode->modelPayload.localRotationAngle2 = rotationAngle2;
    childRootNode->modelPayload.localRotationAngle1 = rotationAngle1;
    childRootNode->modelPayload.localRotationAngle0 = rotationAngle0;
    childRootNode->parentNode = parentModelNode;
    translationX = sourceTransform->localTranslationXQ12;
    translationY = sourceTransform->localTranslationYQ12;
    childRootNode->modelPayload.localTranslationZQ12 =
         sourceTransform->localTranslationZQ12;
    childRootNode->modelPayload.localTranslationYQ12 = translationY;
    childRootNode->modelPayload.localTranslationXQ12 = translationX;
    *outChildModelRuntime = childModelRuntime;
    return true;
  }
  /* Original quirk: index past the attachment count still succeeds and leaves a stale value as the child model
     runtime; in its only caller (ModelNodeRuntime_InstantiateLinkedChildrenRecursive) that value is
     childDefinitionId. */
  *outChildModelRuntime = (ModelRuntimeSlot *)(uintptr_t)childDefinitionId;
  return true;
}

/* Allocates and zeroes the model runtime pool (MODEL_RUNTIME_SLOT_COUNT 0x200-byte slots, 4 MiB) and records
   its rebase delta (pool base - 1) for savegames. Returns 0, or the allocation error
   (FATAL_ERROR_ARENA_EXHAUSTED / ARENA_HEAP_CORRUPT, never 0 from the arena).
*/
uint32_t __cdecl ModelRuntimePool_Init()

{
  ModelRuntimeSlot *modelRuntimePool;
  uint32_t *poolDword;
  int allocationDwordsRemaining;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(MODEL_RUNTIME_POOL_BYTES,(void **)&modelRuntimePool);
  if (allocError != 0) {
    return allocError;
  }
  /* pool base - 1 */
  g_ModelRuntimeRebaseDelta = (intptr_t)modelRuntimePool - 1;
  g_ModelRuntimeSlots = modelRuntimePool;
  /* zero the pool dword by dword */
  poolDword = (uint32_t *)modelRuntimePool;
  for (allocationDwordsRemaining = MODEL_RUNTIME_POOL_BYTES / 4; allocationDwordsRemaining != 0;
       allocationDwordsRemaining--) {
    *poolDword = 0;
    poolDword++;
  }
  return 0;
}

/* Part of ModelRuntimePool_ShutdownAndReleaseDefinitions: releases the resource of one serialized
   MDL definition node and then, depth first in index order, of all its children (childCount,
   childSerializedOffsets). Only plain nodes (nodeFlags low nibble 0) whose ownedNestedResourcePresent is set own a
   resource (spriteAssetReference). The original walks the tree with an explicit {count, index, node}
   frame stack on the machine stack. */
static void ModelRuntimePool_ReleaseDefinitionNodeResources(MdlSerializedNodeHeader *node)

{
  uint32_t childrenRemaining;
  int childIndex;

  childrenRemaining = node->childCount;
  if ((node->nodeFlags & 0xf) == 0 && node->ownedNestedResourcePresent != 0) {
    Resource_Release(node->spriteAssetReference.spriteAsset);
  }
  for (childIndex = 0; childrenRemaining != 0; childIndex++) {
    /* the child offsets were relocated into pointers when the definition was registered */
    ModelRuntimePool_ReleaseDefinitionNodeResources
              ((MdlSerializedNodeHeader *)(uintptr_t)node->childSerializedOffsets[childIndex]);
    childrenRemaining--;
  }
  return;
}

/* Counterpart of ModelRuntimePool_Init: frees the model runtime pool, releases the resources of every
   registered model definition's node tree (see ModelRuntimePool_ReleaseDefinitionNodeResources) and clears
   the definition registry.
*/
void ModelRuntimePool_ShutdownAndReleaseDefinitions()

{
  int registryRemaining;
  ModelDefinitionRecordPrefix **registryEntry;
  MdlSerializedNodeHeader *rootNode;

  g_MemoryApi.free(g_ModelRuntimeSlots);
  g_ModelRuntimeSlots = nullptr;
  registryEntry = g_ModelDefinitionRegistry;
  for (registryRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT; registryRemaining != 0; registryRemaining--) {
    if (*registryEntry != nullptr) {
      /* the root of the definition's node tree */
      /* 5f-format: ModelDefinition.rootNodeOffsetOrPointer */
      rootNode = Thandor_U32ToPointer<MdlSerializedNodeHeader>(((ModelDefinition *)*registryEntry)->rootNodeOffsetOrPointer);
      if (rootNode != nullptr) {
        ModelRuntimePool_ReleaseDefinitionNodeResources(rootNode);
      }
    }
    *registryEntry = nullptr;
    registryEntry++;
  }
}

/* Part of ModelRuntimePool_UnrebaseBeforeSave: zeroes an unused slot's 0x80 dwords, one dword at a time. */
static void ModelRuntimePool_ZeroUnusedSlotBeforeSave(ModelRuntimeSlotUnrebaseView *modelRuntime)

{
  uint32_t *slotDword;
  int dwordsRemaining;

  slotDword = (uint32_t *)modelRuntime;
  for (dwordsRemaining = sizeof(ModelRuntimeSlot) / 4; dwordsRemaining != 0; dwordsRemaining--) {
    *slotDword = 0;
    slotDword++;
  }
}

/* True when the definition's runtimeClassId selects a column of the 24-class handler matrix
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes). */
static Bool8 ModelDefinition_HasHandledRuntimeClass(const ModelDefinition *definition)
{
  return (uint32_t)definition->runtimeClassId <
         sizeof(g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize) /
         sizeof(g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize[0]);
}

/* Part of ModelRuntimePool_UnrebaseBeforeSave: turns the pointers of one used slot into saved offsets, replaces
   the definition by its id, runs the class's modelUnrebase handler and then unrebases the used attachment
   descriptors (NULL stays 0). */
static void ModelRuntimePool_UnrebaseUsedSlotBeforeSave(ModelRuntimeSlotUnrebaseView *modelRuntime)

{
  uint32_t ownerArmyOffset;
  uint32_t linkedModelOffset;
  uint32_t linkedArmyOffset;
  uint32_t runtimeClassId;
  uint32_t attachmentsRemaining;
  ModelRuntimeAttachmentSavedDescriptor *attachment;
  ModelRuntimePoolRelativeOffset childRuntimeOffset;
  ModelNodePoolRelativeOffset parentNodeOffset;

  /* 5f-format: ModelRuntimeSlot.ownerArmyRuntimeOrSavedOffset/rootModelNodeOrSavedOffset/linkedModelRuntimeOrSavedOffset/classState.linkedArmyRuntimeOrSavedOffset/attachments (unrebase before save) */
  ownerArmyOffset = modelRuntime->ownerArmyRuntimeSavedOffset - Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne);
  modelRuntime->rootModelNodeSavedOffset =
       modelRuntime->rootModelNodeSavedOffset - Thandor_PointerToI32(g_RuntimeObjectRebaseBaseMinusOne);
  modelRuntime->ownerArmyRuntimeSavedOffset = ownerArmyOffset;
  linkedModelOffset = modelRuntime->linkedModelRuntimeSavedOffset;
  linkedArmyOffset = modelRuntime->classState.linkedArmyRuntimeSavedOffset;
  if (linkedModelOffset != 0) {
    linkedModelOffset = (uint32_t)(linkedModelOffset - g_ModelRuntimeRebaseDelta);
  }
  if (linkedArmyOffset != 0) {
    /* 5f-format: ModelRuntimeSlot.classState.linkedArmyRuntimeOrSavedOffset */
    linkedArmyOffset = linkedArmyOffset - Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne);
  }
  modelRuntime->linkedModelRuntimeSavedOffset = linkedModelOffset;
  modelRuntime->classState.linkedArmyRuntimeSavedOffset = linkedArmyOffset;
  runtimeClassId = modelRuntime->definitionReferenceOrSavedId.runtimeDefinition->runtimeClassId;
  modelRuntime->definitionReferenceOrSavedId.savedIdOrOffset =
       (uint32_t)modelRuntime->definitionReferenceOrSavedId.definition->definitionId;
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase[runtimeClassId]
            ((ModelRuntimeSlot *)modelRuntime);
  attachment = modelRuntime->attachments;
  for (attachmentsRemaining = modelRuntime->attachmentCount; attachmentsRemaining != 0; attachmentsRemaining--) {
    childRuntimeOffset = attachment->childModelRuntimeSavedOffset;
    parentNodeOffset = attachment->parentModelNodeSavedOffset;
    if (childRuntimeOffset != 0) {
      childRuntimeOffset = (ModelRuntimePoolRelativeOffset)(childRuntimeOffset - g_ModelRuntimeRebaseDelta);
    }
    if (parentNodeOffset != 0) {
      /* 5f-format: ModelRuntimeSlot.attachments[].parentModelNodeOrSavedOffset */
      parentNodeOffset = parentNodeOffset - Thandor_PointerToI32(g_RuntimeObjectRebaseBaseMinusOne);
    }
    attachment->childModelRuntimeSavedOffset = childRuntimeOffset;
    attachment->parentModelNodeSavedOffset = parentNodeOffset;
    attachment++;
  }
}

/* Before the model runtime pool is written to a savegame (in-game save, src/gameplay/session/savegame.cpp): turns the
   pointers of every used slot into offsets (owner and linked army against g_ArmyRuntimeRebaseBaseMinusOne, root
   and attachment parent nodes against g_RuntimeObjectRebaseBaseMinusOne, linked model runtime and attachment
   children against g_ModelRuntimeRebaseDelta; NULL stays 0), replaces the definition pointer by its id and runs
   the class's modelUnrebase handler. Unused slots are zeroed. The original also returns the pool and its size
   0x400000 for the save. Counterpart of ModelRuntimePool_RebaseAfterLoad.
*/
void __cdecl ModelRuntimePool_UnrebaseBeforeSave()

{
  ModelRuntimeSlotUnrebaseView *modelRuntime;
  int slotsRemaining;

  modelRuntime = (ModelRuntimeSlotUnrebaseView *)g_ModelRuntimeSlots;
  for (slotsRemaining = MODEL_RUNTIME_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (modelRuntime->rootModelNodeSavedOffset == 0) {
      ModelRuntimePool_ZeroUnusedSlotBeforeSave(modelRuntime);
    }
    else {
      ModelRuntimePool_UnrebaseUsedSlotBeforeSave(modelRuntime);
    }
    modelRuntime++;
  }
}

/* Turns the saved offsets of the used attachment descriptors back into pointers: children get
   g_ModelRuntimeRebaseDelta, parent nodes g_RuntimeObjectRebaseBaseMinusOne; zero offsets stay NULL. */
static void ModelRuntime_RebaseAttachmentsAfterLoad(ModelRuntimeSlot *modelRuntime)
{
  ModelRuntimeAttachmentDescriptor *attachment;
  uint32_t attachmentsRemaining;
  ModelRuntimeSlot *savedChildRuntime;
  ModelRuntimeNode *savedParentNode;
  ModelRuntimeSlot *rebasedChildRuntime;
  ModelRuntimeNode *rebasedParentNode;

  attachment = modelRuntime->attachments;
  for (attachmentsRemaining = modelRuntime->attachmentCount; attachmentsRemaining != 0; attachmentsRemaining--) {
    savedChildRuntime = attachment->childModelRuntimeOrSavedOffset;
    savedParentNode = attachment->parentModelNodeOrSavedOffset;
    rebasedChildRuntime = nullptr;
    if (savedChildRuntime != nullptr) {
      rebasedChildRuntime = (ModelRuntimeSlot *)((uint8_t *)savedChildRuntime + g_ModelRuntimeRebaseDelta);
    }
    rebasedParentNode = nullptr;
    if (savedParentNode != nullptr) {
      /* 5f-format: ModelRuntimeSlot.attachments[].parentModelNodeOrSavedOffset */
      rebasedParentNode = (ModelRuntimeNode *)(g_RuntimeObjectRebaseBaseMinusOne + Thandor_PointerToI32(savedParentNode));
    }
    attachment->childModelRuntimeOrSavedOffset = rebasedChildRuntime;
    attachment->parentModelNodeOrSavedOffset = rebasedParentNode;
    attachment++;
  }
}

/* After a savegame load, counterpart of ModelRuntimePool_UnrebaseBeforeSave: turns the saved offsets of every
   used model runtime slot back into pointers, replaces the saved definition id by the registered definition,
   runs the class's load-repair callback and rebuilds the attachment descriptors from the definition. A slot
   whose definition is no longer registered is dropped.
*/
void ModelRuntimePool_RebaseAfterLoad()

{
  int slotIndex;
  ModelRuntimeSlot *modelRuntime;
  ArmyRuntimeSlot *rebasedOwnerArmy;
  ArmyRuntimeSlot *savedLinkedArmy;
  ArmyRuntimeSlot *rebasedLinkedArmy;
  ModelRuntimeSlot *rebasedLinkedRuntime;
  ModelDefinitionRecordPrefix *registeredDefinition;

  for (slotIndex = 0; slotIndex < MODEL_RUNTIME_SLOT_COUNT; slotIndex++) {
    modelRuntime = &g_ModelRuntimeSlots[slotIndex];
    if (modelRuntime->rootModelNodeOrSavedOffset.modelNode == nullptr) {
      continue;
    }
    /* saved offsets + pool deltas: the owner army (always rebased) and the linked army
       (classState.linkedArmyRuntimeOrSavedOffset) get g_ArmyRuntimeRebaseBaseMinusOne, the root node and
       attachment parent nodes g_RuntimeObjectRebaseBaseMinusOne, the linked model runtime and attachment children
       g_ModelRuntimeRebaseDelta; zero offsets other than the owner stay NULL. */
    /* 5f-format: ModelRuntimeSlot.ownerArmyRuntimeOrSavedOffset/rootModelNodeOrSavedOffset (rebase after load) */
    rebasedOwnerArmy = Thandor_U32ToPointer<ArmyRuntimeSlot>((int)modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime +
                                           Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
    modelRuntime->rootModelNodeOrSavedOffset.modelNode =
         (ModelRuntimeNode *)
         (g_RuntimeObjectRebaseBaseMinusOne + (int)modelRuntime->rootModelNodeOrSavedOffset.modelNode);
    modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime = rebasedOwnerArmy;
    savedLinkedArmy = modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.armyRuntime;
    rebasedLinkedRuntime = nullptr;
    if (modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime != nullptr) {
      rebasedLinkedRuntime = (ModelRuntimeSlot *)
                   ((uint8_t *)modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime + g_ModelRuntimeRebaseDelta);
    }
    rebasedLinkedArmy = nullptr;
    if (savedLinkedArmy != nullptr) {
      /* 5f-format: ModelRuntimeSlot.classState.linkedArmyRuntimeOrSavedOffset */
      rebasedLinkedArmy = Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(savedLinkedArmy) + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
    }
    modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = rebasedLinkedRuntime;
    modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.armyRuntime = rebasedLinkedArmy;

    registeredDefinition = ModelDefinitionRegistry_FindById
                             ((PckModelDefinitionIdCatalog)modelRuntime->definitionOrSavedId.savedIdOrOffset);
    if (registeredDefinition == nullptr) {
      /* definition no longer registered: drop the instance */
      modelRuntime->rootModelNodeOrSavedOffset.modelNode = nullptr;
      continue;
    }
    if (!ModelDefinition_HasHandledRuntimeClass((ModelDefinition *)registeredDefinition)) {
      /* The original indexes the handler matrix unchecked; bounded here because no instance of such a
         definition can have been created (ModelRuntimePool_CreateInstanceByDefinitionId): dropped like an
         unregistered one. */
      Thandor_Log("model: saved instance of a definition with runtime class %d outside 0..23, dropped",
                  ((ModelDefinition *)registeredDefinition)->runtimeClassId);
      modelRuntime->rootModelNodeOrSavedOffset.modelNode = nullptr;
      continue;
    }
    modelRuntime->definitionOrSavedId.definition = registeredDefinition;
    g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelRebaseOrLoadRepair
      [((ModelDefinition *)registeredDefinition)->runtimeClassId](modelRuntime);
    if (modelRuntime->attachmentCount != 0) {
      ModelRuntime_RebaseAttachmentsAfterLoad(modelRuntime);
      modelRuntime->attachmentCount = 0;
      /* 5f-format: ModelDefinition.rootNodeOffsetOrPointer */
      ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                (modelRuntime,
                 Thandor_U32ToPointer<MdlSerializedNodeHeader>(
                 modelRuntime->definitionOrSavedId.runtimeDefinition->rootNodeOffsetOrPointer));
    }
  }
}

/* Destroys a model runtime: drops player references to it, runs its class release handler, destroys the
   attached model runtimes, clears world nodes that still point to it and releases its node tree.
   An attached part is then removed from its parent's attachment list and the army's derived metrics are
   rebuilt; a root model destroys its army instead, first spawning the army asset its definition names in
   destroyedReplacementArmyAssetId at the same place, unless that id is -1 or the owner record's
   classState.stateFlags has ARMY_MODEL_STATE_DESTRUCTION_STARTED (0x20).
*/
void ModelRuntimePool_DestroyHierarchyAndDetach(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  int *ownerRecord;
  ModelDefinitionRecordPrefix *modelDefinition;
  ModelRuntimeSlot *childRuntime;
  ModelRuntimeNode *rootModelNode;
  GameEntityRuntime *entityRuntime;
  Q12 translationX;
  Q12 translationY;
  AngleTurn32 orientationAngle;
  ModelDefinition *ownerDefinition;
  uint32_t runtimeClassId;
  uint32_t attachmentsRemaining;
  ModelRuntimeAttachmentDescriptor *attachment;
  ModelRuntimeSlot *parentRuntime;
  ModelRuntimeNode *parentModelNode;

  modelDefinition = modelRuntime->definitionOrSavedId.definition;
  runtimeClassId = ((ModelDefinition *)modelDefinition)->runtimeClassId;
  FrontendPlayerRuntime_ClearAssignmentTokenFromAll((uintptr_t)modelRuntime);
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[runtimeClassId]
            (modelDefinition,modelRuntime);
  attachment = modelRuntime->attachments;
  for (attachmentsRemaining = modelRuntime->attachmentCount; attachmentsRemaining != 0; attachmentsRemaining--) {
    childRuntime = attachment->childModelRuntimeOrSavedOffset;
    if (childRuntime != nullptr) {
      ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,childRuntime);
    }
    attachment++;
  }
  rootModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  entityRuntime = (GameEntityRuntime *)modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  /* position and heading for the replacement army, read before the node tree is released */
  translationX = rootModelNode->worldTransform.translation.x;
  translationY = rootModelNode->worldTransform.translation.y;
  orientationAngle = rootModelNode->modelPayload.worldRotationAngle2;
  parentModelNode = rootModelNode->parentNode;
  WorldRuntime_ForEachOwnerListNode
            (modelRuntime,WorldRuntimeNode_ClearDetachedEntityReferencesCallback,worldRuntime);
  ModelRuntimeNode_ReleaseRecursiveAndDetachParent(rootModelNode);
  modelRuntime->rootModelNodeOrSavedOffset.modelNode = nullptr;
  if (parentModelNode == nullptr) {
    if (entityRuntime->common.ownership.definitionOrClassRecord != nullptr) {
      ownerRecord = (int *)entityRuntime->common.ownership.definitionOrClassRecord;
      entityRuntime->common.ownership.definitionOrClassRecord = nullptr;
      ownerDefinition = ((ModelRuntimeSlot *)ownerRecord)->definitionOrSavedId.runtimeDefinition;
      /* ownerRecord is the owner's root ModelRuntimeSlot */
      if ((((ModelRuntimeSlot *)ownerRecord)->classState.stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0 &&
         (ownerDefinition->destroyedReplacementArmyAssetId != -1)) {
        /* the third parameter of ArmyRuntime_CreateInstanceFromAsset takes y, as at its other callers */
        ArmyRuntime_CreateInstanceFromAsset
                  (0,orientationAngle,translationY,translationX,0,
                   ownerDefinition->destroyedReplacementArmyAssetId,
                   worldRuntime,nullptr);
      }
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
    }
  }
  else {
    parentRuntime = parentModelNode->runtimePayload.modelRuntime;
    attachment = parentRuntime->attachments;
    for (attachmentsRemaining = parentRuntime->attachmentCount; attachmentsRemaining != 0; attachmentsRemaining--) {
      if (attachment->childModelRuntimeOrSavedOffset == modelRuntime) {
        attachment->childModelRuntimeOrSavedOffset = nullptr;
      }
      attachment++;
    }
    ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)entityRuntime);
  }
}

/* Creates a model runtime for an army from a model definition id: takes the first free pool slot, copies the
   definition's starting values (health and the destructionEffectTimers), raises two of the
   army's values to the definition's, builds the model node tree, its bounding radius and transforms, and runs
   the definition class's initialize handler. Returns 0 and stores the slot in *outModelRuntime, or returns
   FATAL_ERROR_GENERAL_FAILURE (no pool or no free slot), the node tree's error, or
   FATAL_ERROR_MODEL_DEFINITION_MISSING (never 0) and leaves *outModelRuntime unchanged.
*/
uint32_t ModelRuntimePool_CreateInstanceByDefinitionId
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot **outModelRuntime)

{
  int slotIndex;
  int prefixIndex;
  ModelRuntimeSlot *modelRuntime;
  ModelDefinition *definitionView;
  uint32_t modelFlags;
  ModelRuntimeNode *modelNodeRuntime;

  /* first free slot (no root node) */
  if (g_ModelRuntimeSlots == nullptr) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  slotIndex = 0;
  while (g_ModelRuntimeSlots[slotIndex].rootModelNodeOrSavedOffset.modelNode != nullptr) {
    slotIndex++;
    if (slotIndex == MODEL_RUNTIME_SLOT_COUNT) {
      return FATAL_ERROR_GENERAL_FAILURE;
    }
  }
  modelRuntime = &g_ModelRuntimeSlots[slotIndex];

  definitionView = (ModelDefinition *)ModelDefinitionRegistry_LookupById(modelDefinitionId);
  if (definitionView == nullptr) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,modelDefinitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_MODEL_DEFINITION_MISSING;
  }
  if (!ModelDefinition_HasHandledRuntimeClass(definitionView)) {
    /* The original indexes the 24-class handler matrix with runtimeClassId unchecked; bounded here because
       a definition with another class has no handlers: it is treated like a missing definition. */
    Thandor_Log("model: definition %d has runtime class %d outside 0..23, not created",
                (int)modelDefinitionId,definitionView->runtimeClassId);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,modelDefinitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_MODEL_DEFINITION_MISSING;
  }

  modelRuntime->definitionOrSavedId.definition = (ModelDefinitionRecordPrefix *)definitionView;
  modelRuntime->rootModelNodeOrSavedOffset.modelNode = nullptr;
  modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime = armyRuntime;
  modelRuntime->attachmentCount = 0;
  modelRuntime->health = definitionView->maximumHealth;
  if (armyRuntime->visibilityRadius < definitionView->visibilityRadius) {
    armyRuntime->visibilityRadius = definitionView->visibilityRadius;
  }
  if (armyRuntime->occupancyMarkRadius < definitionView->occupancyMarkRadius) {
    armyRuntime->occupancyMarkRadius = definitionView->occupancyMarkRadius;
  }
  /* Original quirk: only the first 32 bytes of classPrefixState are cleared; effectModelFlags and the 4 bytes after
     it keep the old slot's values. */
  for (prefixIndex = 0; prefixIndex < 32; prefixIndex++) {
    modelRuntime->classPrefixState[prefixIndex] = 0;
  }
  modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = nullptr;
  modelRuntime->destructionEffectTimers[0] = definitionView->destructionEffectDelayTicks0;
  modelRuntime->destructionEffectTimers[1] = definitionView->destructionEffectDelayTicks1;
  modelRuntime->destructionEffectTimers[2] = definitionView->destructionEffectDelayTicks2;
  modelRuntime->destructionEffectTimers[3] = definitionView->destructionEffectDelayTicks3;
  modelRuntime->destructionEffectTimers[4] = definitionView->destructionEffectDelayTicks4;
  modelRuntime->destructionEffectTimers[5] = definitionView->destructionEffectDelayTicks5;
  modelRuntime->destructionEffectTimers[6] = definitionView->destructionEffectDelayTicks6;
  modelRuntime->destructionEffectTimers[7] = definitionView->destructionEffectDelayTicks7;
  modelRuntime->classState.shotEmitterTimerTicks = 1;
  modelRuntime->classState.effectEmitterTimerTicks = 1;
  modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime = nullptr;
  modelRuntime->classState.energyLoadQ4 = definitionView->energyLoadQ4;
  modelRuntime->classState.stateFlags = 0;
  modelRuntime->classState.healthRegenerationDelayTicks = 0;
  modelRuntime->classState.dismantleTickCountdown = 0;
  modelRuntime->damageEffectPointIndex = 0;
  modelFlags = definitionView->modelFlags;
  /* 5f-format: ModelDefinition.rootNodeOffsetOrPointer */
  if (Thandor_U32ToPointer<MdlSerializedNodeHeader>(definitionView->rootNodeOffsetOrPointer) != nullptr) {
    if (!ModelNodeRuntime_CreateHierarchyRecursive
            (paletteAsset,textureSet,modelRuntime,
             Thandor_U32ToPointer<MdlSerializedNodeHeader>(definitionView->rootNodeOffsetOrPointer),worldRuntime,
             &modelNodeRuntime)) {
      /* the hierarchy's error code: no free world object record */
      return FATAL_ERROR_GENERAL_FAILURE;
    }
    modelRuntime->rootModelNodeOrSavedOffset.modelNode = modelNodeRuntime;
    ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    if ((modelFlags & MODEL_DEFINITION_FLAG_RAY_TRANSPARENT) != 0) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | MODEL_NODE_FLAG_RAY_TRANSPARENT;
    }
  }
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize
    [modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId])
            (modelRuntime->definitionOrSavedId.definition,modelRuntime);
  *outModelRuntime = modelRuntime;
  return 0;
}
