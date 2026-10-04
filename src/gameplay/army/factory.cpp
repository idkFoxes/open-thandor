/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/factory.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/factory.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/factory. */

/* Takes the first queued secondary asset whose flags match the factory definition's buildable mask
   (classParameterC4) and which the faction can pay for: the Xenite is paid, the build interval and the asset's
   Energy load (held while building) are stored in the factory, and the factory starts building. */
static void ArmyUnitFactory_StartBuildingFirstAffordableAsset(ModelRuntimeUpdateView *modelRuntime,
          FactionRuntimeIndex factionIndex)
{
  FactionArmyAssetCount remainingAssetCount;
  uint32_t *queueEntry;
  ArmyAssetRecord *candidateAsset;
  uint32_t xeniteCostQ4;
  uint32_t buildTicks;
  uint32_t energyLoadQ4;
  PckArmyAssetIdCatalog selectedAssetId;

  remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  queueEntry = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  for (; remainingAssetCount != 0; remainingAssetCount = remainingAssetCount - 1, queueEntry = queueEntry + 1) {
    candidateAsset = Thandor_U32ToPointer<ArmyAssetRecord>(*queueEntry); /* 32-bit format field: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
    if ((candidateAsset->flags & modelRuntime->modelDefinition->classParameterC4) == 0) {
      continue;
    }
    xeniteCostQ4 = candidateAsset->xeniteCostQ4;
    if (g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 < xeniteCostQ4) {
      continue;
    }
    g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - xeniteCostQ4;
    buildTicks = candidateAsset->buildTicks;
    energyLoadQ4 = candidateAsset->energyLoadQ4;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
      buildTicks = (buildTicks >> 4) + 1;
    }
    selectedAssetId = candidateAsset->registryId;
    (modelRuntime->classLinkState).classState68 = buildTicks;
    (modelRuntime->classLinkState).classState74 = energyLoadQ4;
    (modelRuntime->classLinkState).modelLinkOrState.classState = (uint32_t)selectedAssetId;
    (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 + energyLoadQ4;
    (modelRuntime->classLinkState).classState64 = 0;
    g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount =
         g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount - 1;
    /* Remove the entry: shift the rest of the queue down by one. Original quirk: it shifts remainingAssetCount
       entries, i.e. it also copies the slot just behind the last queued entry. */
    do {
      *queueEntry = queueEntry[1];
      queueEntry = queueEntry + 1;
      remainingAssetCount = remainingAssetCount - 1;
    } while (remainingAssetCount != 0);
    (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_BUILDING;
    (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags | ARMY_MODEL_STATE_PRODUCING;
    return;
  }
}

/* Plays the factory definition's one-shot sound at the factory, unless the factory's terrain cell has mask bits
   0/1 set. */
static void ArmyUnitFactory_PlayPrimarySound(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime,
          ModelDefinition *factoryDefinition)
{
  uint32_t soundIndex;
  ModelRuntimeNode *rootNode;
  DirectSoundVoiceSet **soundVoiceSet;
  DirectSoundVoiceSet *quirkVoiceSet;
  GraphicsFixedVec3 *translationVec;

  soundIndex = factoryDefinition->primarySoundIndex;
  if ((soundIndex == 0) || (worldRuntime->dwordArrayCount <= soundIndex) || (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  rootNode = modelRuntime->rootModelNode;
  /* Original quirk: the voice set is read from rootNode + index * 4, not from
     worldRuntime->dwordArray, which is only tested for NULL. */
  soundVoiceSet = THANDOR_PTR32_AT(DirectSoundVoiceSet *, (uint8_t *)rootNode + soundIndex * 4);
  translationVec = &(rootNode->worldTransform).translation;
  if (soundVoiceSet == nullptr) {
    return;
  }
  if (!TerrainGrid_TestProjectedCellMaskBits01((rootNode->worldTransform).translation.y,translationVec->x,
                                               worldRuntime)) {
    /* the dword it points at is taken as the voice set (a 32-bit slot of the node's memory, as in the original) */
    quirkVoiceSet = THANDOR_PTR32_AT(DirectSoundVoiceSet, soundVoiceSet);
    SpatialSound_PlayPositionedOneShot
              (factoryDefinition->positionedSoundMaximumDistanceQ12,factoryDefinition->positionedSoundGainQ15,
               translationVec,&quirkVoiceSet);
  }
}

/* The build is done: creates the army at the spawn point (lookup key 0/5), heading towards the exit point (lookup
   key 1/5), links it to the factory, releases the held Energy load, starts opening the door and, for the active
   faction, queues the "army created" notification. */
static void ArmyUnitFactory_CreateBuiltArmy(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime,
          ModelRuntimeNode *rootNode)
{
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint exitPoint;
  ModelWorldPoint spawnPoint;
  uint32_t exitXQ12;
  uint32_t exitYQ12;
  uint32_t spawnHeading;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ArmyRuntimeSlot *createdArmyRuntime;
  ModelRuntimeSlot *createdModelRuntime;
  ModelRuntimeNode *createdNode;
  ModelDefinition *createdDefinition;
  ModelDefinition *factoryDefinition;
  FactionRuntimeIndex createdFactionIndex;
  uint32_t heldEnergyLoadQ4;
  uint32_t viewPitchAngle;
  int nodeHeading;
  InGameNotificationMovieId notificationMovieId;
  ArmyAssetRecordPrefix *unusedArmyAsset;

  if (!ModelLookupTable_FindPackedPoint(1,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
    return;
  }
  exitPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
  exitYQ12 = exitPoint.yQ12;
  exitXQ12 = exitPoint.xQ12;
  if (!ModelLookupTable_FindPackedPoint(0,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
    return;
  }
  spawnPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
  spawnHeading = FixedMath_Atan2Angle16(exitYQ12 - spawnPoint.yQ12,exitXQ12 - spawnPoint.xQ12);
  ownerArmyRuntime = modelRuntime->ownerArmyRuntime;
  createdArmyRuntime = ArmyRuntime_CreateInstanceFromAsset
                     (4,spawnHeading,spawnPoint.yQ12,spawnPoint.xQ12,ownerArmyRuntime->factionIndex,
                      (modelRuntime->classLinkState).modelLinkOrState.classState,worldRuntime,nullptr);
  if (createdArmyRuntime == nullptr) {
    return;
  }
  g_GameFactionRuntimeImage.records[ownerArmyRuntime->factionIndex].relationCounterA =
       g_GameFactionRuntimeImage.records[ownerArmyRuntime->factionIndex].relationCounterA + 1;
  factoryDefinition = modelRuntime->modelDefinition;
  createdArmyRuntime->movementStateFlags = createdArmyRuntime->movementStateFlags |
                                           (ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_LOCKED);
  ArmyUnitFactory_PlayPrimarySound(worldRuntime,modelRuntime,factoryDefinition);
  heldEnergyLoadQ4 = (modelRuntime->classLinkState).classState74;
  (modelRuntime->classLinkState).armyLinkOrState.armyRuntime = createdArmyRuntime;
  createdFactionIndex = createdArmyRuntime->factionIndex;
  (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_OPENING;
  (modelRuntime->classLinkState).classState74 = 0;
  (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 - heldEnergyLoadQ4;
  createdModelRuntime = createdArmyRuntime->modelRuntimeOrSavedOffset.modelRuntime;
  createdArmyRuntime->movementStateFlags = createdArmyRuntime->movementStateFlags | ARMY_MOVEMENT_LOCKED;
  /* the new army links back to this factory until it has left (ARMY_FACTORY_STATE_WAITING_EXIT) */
  createdModelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime = (ModelRuntimeSlot *)modelRuntime;
  if (createdFactionIndex != worldRuntime->activeFactionRuntimeIndex) {
    return;
  }
  /* result unused (the lookup only records an unknown id in g_PackageLastErrorPath) */
  ArmyAssetRegistry_FindById((modelRuntime->classLinkState).modelLinkOrState.classState,&unusedArmyAsset);
  viewPitchAngle = (worldRuntime->motion).pitchAngle;
  createdNode = createdModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  createdDefinition = createdModelRuntime->definitionOrSavedId.runtimeDefinition;
  nodeHeading = createdNode->modelPayload.worldRotationAngle2;
  createdDefinition->builtCount = createdDefinition->builtCount + 1;
  notificationMovieId = createdDefinition->firstBuiltNotificationMovieId;
  if (createdDefinition->builtCount != 1) {
    notificationMovieId = createdDefinition->nextBuiltNotificationMovieId;
  }
  InGameNotificationQueue_InsertPriorityRecord
            (ARMY_CREATED,0,viewPitchAngle,
             nodeHeading + ARMY_FACTORY_NOTIFICATION_HEADING_OFFSET_ANGLE16 & FIXED_ANGLE16_MASK,
             createdNode->worldTransform.translation.y,createdNode->worldTransform.translation.x,2,
             notificationMovieId);
}

/* Runtime update of the unit factory class (13), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[13]. On the first update it stores the
   factory's exit point (model lookup key 1/5). It builds one queued secondary army asset whose flags match the
   definition's mask (Xenite paid up front, Energy load held while building), creates the army at the spawn
   point, opens the door, sends the army out to the exit point, waits until it has left and closes the door.
*/
void ArmyRuntimeClass_UpdateUnitFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelRuntimeNode *rootNode;
  ModelDefinition *factoryDefinition;
  ModelRuntimeSlot *linkedModelRuntime;
  uint32_t behaviorState;
  uint32_t elapsedTicks;
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint localPoint;
  ArmyRuntimeSlot *linkedArmyRuntime;

  rootNode = modelRuntime->rootModelNode;
  if (((modelRuntime->classState).classStateBC & 1) != 0) {
    if (ModelLookupTable_FindPackedPoint(1,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
      (modelRuntime->classLinkState).classState78 = localPoint.xQ12;
      (modelRuntime->classLinkState).classState7C = localPoint.yQ12;
      (modelRuntime->classState).classStateBC = (modelRuntime->classState).classStateBC & ~1u;
    }
  }
  behaviorState = (modelRuntime->classState).behaviorState;
  factoryDefinition = modelRuntime->modelDefinition;
  if ((3 < rootNode->childCount) && (rootNode->childNodes[3] != nullptr)) {
    WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)rootNode->childNodes[3]);
    rootNode->childNodes[3] = nullptr;
  }
  switch(behaviorState) {
  case ARMY_FACTORY_STATE_IDLE: /* start the first affordable queued asset this factory can build */
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) {
        ArmyUnitFactory_StartBuildingFirstAffordableAsset(modelRuntime,modelRuntime->ownerArmyRuntime->factionIndex);
      }
    }
    else if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case ARMY_FACTORY_STATE_BUILDING: /* when done create the army at the spawn point (lookup keys 1/5 and 0/5 give
                                       its heading) */
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      (modelRuntime->classLinkState).classState64 =
           (modelRuntime->classLinkState).classState64 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      elapsedTicks = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= elapsedTicks) {
        ArmyUnitFactory_CreateBuiltArmy(worldRuntime,modelRuntime,rootNode);
      }
    }
    break;
  case ARMY_FACTORY_STATE_OPENING: /* then send the new army out to the point in classState78/7C */
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV +
         factoryDefinition->movementSpeed * g_InGameSimulationStepTicks;
    if (ARMY_DOOR_TEXTURE_OPEN_V - 1 < rootNode->primaryTextureOffsetV) {
      rootNode->primaryTextureOffsetV = ARMY_DOOR_TEXTURE_OPEN_V;
      (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_WAITING_EXIT;
      if (ModelLookupTable_FindPackedPoint(1,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
        linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState.armyRuntime;
        localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
        linkedModelRuntime = (linkedArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
        ArmyRuntime_StartMoveCommandWithAuxiliaryValues
                  ((modelRuntime->classLinkState).classState7C,
                   (modelRuntime->classLinkState).classState78,localPoint.yQ12,localPoint.xQ12,
                   (ArmyMovementRuntime *)linkedArmyRuntime);
        (linkedModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime =
             (ModelRuntimeSlot *)modelRuntime;
      }
    }
    break;
  case ARMY_FACTORY_STATE_WAITING_EXIT:
    linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState.armyRuntime;
    if ((linkedArmyRuntime == nullptr) ||
       ((ModelRuntimeUpdateView *)
        (((linkedArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).
        linkedArmyRuntimeOrSavedOffset.modelRuntime != modelRuntime)) {
      factoryDefinition = modelRuntime->modelDefinition;
      (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_CLOSING;
      (modelRuntime->classLinkState).armyLinkOrState.armyRuntime = nullptr;
      ArmyUnitFactory_PlayPrimarySound(worldRuntime,modelRuntime,factoryDefinition);
    }
    break;
  case ARMY_FACTORY_STATE_CLOSING:
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV -
         factoryDefinition->movementSpeed * g_InGameSimulationStepTicks;
    if (rootNode->primaryTextureOffsetV < 1) {
      rootNode->primaryTextureOffsetV = 0;
      (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_IDLE;
      (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags & ~ARMY_MODEL_STATE_PRODUCING;
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}

/* Runtime update of production class 11, reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[11]. Builds one queued secondary army asset
   with flag 0x10 at a time: its Xenite cost (xeniteCostQ4) is paid once up front, its Energy load
   (energyLoadQ4) is held on the building while it is built. The finished asset is appended to the faction's
   primary asset list (at most 64 entries, from where it is placed) and, for the active faction, the command grid
   is rebuilt and a notification is queued.
*/
void ArmyRuntimeClass_UpdateStructureFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  uint32_t assetEnergyValue;
  ModelRuntimeSlotLinkOrState selectedAssetLink;
  int factionIndex;
  ArmyAssetRecord *candidateAsset;
  int activeFactionIndex;
  AngleTurn32 headingAngle;
  ModelDefinition *linkedModelDefinition;
  FactionArmyAssetCount remainingAssetCount;
  uint32_t buildTicks;
  uint32_t elapsedTicks;
  uint32_t heldEnergyLoad;
  uint32_t primaryAssetCount;
  uint32_t linkedRootNodeOffset;
  uint32_t pitchAngle;
  uint32_t *queueSlot;
  uint32_t lookupError;
  ArmyAssetRecordPrefix *assetRecord;
  InGameNotificationMovieId notificationMovieId;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ModelRuntimeNode *rootNode;

  switch((modelRuntime->classState).behaviorState) {
  case ARMY_FACTORY_STATE_IDLE: /* start the first affordable queued asset with flag 0x10 */
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) {
        factionIndex = modelRuntime->ownerArmyRuntime->factionIndex;
        queueSlot = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
        for (remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; remainingAssetCount != 0;
            remainingAssetCount = remainingAssetCount - 1) {
          candidateAsset = Thandor_U32ToPointer<ArmyAssetRecord>(*queueSlot); /* 32-bit format field: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
          if (((candidateAsset->flags & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) != 0) &&
             (candidateAsset->xeniteCostQ4 <= g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4))
          {
            g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - candidateAsset->xeniteCostQ4;
            buildTicks = candidateAsset->buildTicks;
            assetEnergyValue = candidateAsset->energyLoadQ4;
            if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
              buildTicks = (buildTicks >> 4) + 1;
            }
            selectedAssetLink = *(ModelRuntimeSlotLinkOrState *)&candidateAsset->registryId;
            (modelRuntime->classLinkState).classState68 = buildTicks;
            (modelRuntime->classLinkState).classState74 = assetEnergyValue;
            (modelRuntime->classLinkState).modelLinkOrState = selectedAssetLink;
            (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 + assetEnergyValue;
            (modelRuntime->classLinkState).classState64 = 0;
            g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount =
                 g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount - 1;
            /* Remove the entry from the queue. Original quirk: it shifts remainingAssetCount entries, i.e. it
               also copies the slot just behind the last queued entry. */
            do {
              *queueSlot = queueSlot[1];
              queueSlot = queueSlot + 1;
              remainingAssetCount = remainingAssetCount - 1;
            } while (remainingAssetCount != 0);
            (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_BUILDING;
            (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags | ARMY_MODEL_STATE_PRODUCING;
            break;
          }
          queueSlot = queueSlot + 1;
        }
      }
    }
    else if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case ARMY_FACTORY_STATE_BUILDING:
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ownerArmyRuntime = modelRuntime->ownerArmyRuntime;
      (modelRuntime->classLinkState).classState64 =
           (modelRuntime->classLinkState).classState64 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      elapsedTicks = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= elapsedTicks) {
        factionIndex = ownerArmyRuntime->factionIndex;
        heldEnergyLoad = (modelRuntime->classLinkState).classState74;
        (modelRuntime->classLinkState).classState74 = 0;
        (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_IDLE;
        (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags & ~ARMY_MODEL_STATE_PRODUCING;
        (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 - heldEnergyLoad;
        lookupError = ArmyAssetRegistry_FindById
                           ((modelRuntime->classLinkState).modelLinkOrState.classState,&assetRecord);
        (modelRuntime->classLinkState).modelLinkOrState.modelRuntime = nullptr;
        if (lookupError == 0) {
          primaryAssetCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
          if (primaryAssetCount < 64) {
            activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
            /* appended to the faction's primary asset list */
            g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[primaryAssetCount] =
                 Thandor_PointerToU32(assetRecord); /* 32-bit format field: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
            g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount =
                 g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount + 1;
            if (activeFactionIndex == ownerArmyRuntime->factionIndex) {
              linkedRootNodeOffset = assetRecord->rootNodeOffsetOrPointer; /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
              InGameArmyStock_RebuildGrid((UiNodeBase *)worldRuntime);
              linkedModelDefinition = (ModelDefinition *)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                                 (ownerArmyRuntime->factionIndex,linkedRootNodeOffset);
              rootNode = modelRuntime->rootModelNode;
              pitchAngle = (worldRuntime->motion).pitchAngle;
              headingAngle = (rootNode->modelPayload).worldRotationAngle2;
              linkedModelDefinition->builtCount = linkedModelDefinition->builtCount + 1;
              notificationMovieId = linkedModelDefinition->firstBuiltNotificationMovieId;
              if (linkedModelDefinition->builtCount != 1) {
                notificationMovieId = linkedModelDefinition->nextBuiltNotificationMovieId;
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,pitchAngle,headingAngle + ARMY_PRODUCTION_NOTIFICATION_HEADING_OFFSET_ANGLE16 & FIXED_ANGLE16_MASK,
                         (rootNode->worldTransform).translation.y,
                         (rootNode->worldTransform).translation.x,3,notificationMovieId);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}

/* Launches one linked asset of a class-22 pad (called directly by
   ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode): finds a not yet launched slot of
   completedSecondaryArmyAssetIds holding linkedArmyAssetId, creates that army on the pad, marks the slot as used
   and links the new aircraft to the pad (classLinkState.modelLinkOrState, state 1 = parked) with the attack
   point and heading (classState70..78) and the pad's platform height; its health is scaled by the pad's health.
   Returns true when no slot matches or the creation fails.
*/
Bool8 ArmyRuntimeSpawner_CreateLinkedChildInstance
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  ArmyRuntimeLinkedChildSlotMaskState *slotMaskState;
  ModelRuntimeSlot *childModelRuntime;
  ModelRuntimeNode *parentRootNode;
  int remainingSlots;
  uint32_t slotBit;
  const Q12 *slotAssetId;
  ArmyRuntimeSlot *createdArmy;
  ModelRuntimeNode *modelNode;

  /* the pad's slots hold one 32-bit asset id each, from completedSecondaryArmyAssetIds on (this view's
     movementTarget0Q12; classParameterC4 slots). Original quirk: slot 0
     is tested even with 0 slots, and the count then runs below 0 instead of stopping. */
  slotBit = 1;
  remainingSlots = ((ModelDefinition *)armyRuntime->definitionOrAsset)->classParameterC4;
  slotAssetId = &armyRuntime->movementTarget0Q12;
  while ((linkedArmyAssetId != *slotAssetId ||
         (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit)
          != 0))) {
    slotAssetId = slotAssetId + 1;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots - 1;
    if (remainingSlots == 0) {
      return true;
    }
  }
  modelNode = armyRuntime->modelNodeRuntime;
  createdArmy = ArmyRuntime_CreateInstanceFromAsset
                    (0,(modelNode->modelPayload).worldRotationAngle2,
                     (modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                     linkedArmyAssetId,worldRuntime,nullptr);
  if (createdArmy == nullptr) {
    return true;
  }
  childModelRuntime = createdArmy->modelRuntimeOrSavedOffset.modelRuntime;
  slotMaskState = &(armyRuntime->articulatedContact).linkedChildSlotMaskState;
  slotMaskState->linkedChildSlotMask = slotMaskState->linkedChildSlotMask | slotBit;
  armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 - 1;
  /* the aircraft's home pad, state 1 = parked (behaviorState), attack point and heading (classState70..78) */
  childModelRuntime->classLinkState.modelLinkOrState.modelRuntime = (ModelRuntimeSlot *)armyRuntime;
  childModelRuntime->classState.behaviorState = ARMY_AIRCRAFT_STATE_PARKED;
  modelNode = childModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  childModelRuntime->classLinkState.classState70 = inheritedValue70;
  childModelRuntime->classLinkState.classState74 = inheritedValue74;
  parentRootNode = armyRuntime->modelNodeRuntime;
  childModelRuntime->classLinkState.classState78 = inheritedValue78;
  (modelNode->childNodes[0]->modelPayload).localTranslationZQ12 =
       (parentRootNode->childNodes[0]->modelPayload).localTranslationZQ12;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  childModelRuntime->health =
       (int)(((int64_t)armyRuntime->actionVector2Q12 *
             (int64_t)(int)childModelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth)
            / (int64_t)(int)((ModelDefinition *)armyRuntime->definitionOrAsset)->maximumHealth);
  return false;
}

/* Group-A command check: returns true when the source model is of class 13 and the candidate model is
   within its radius + 0xC00 (0.75 in Q12) of the source model's anchor point (model lookup entry (1,5),
   transformed to world space), measured in x/y.
*/
Bool8 ArmyRuntime_TestArmyNearFactoryExit(ModelRuntimeSlot *candidateModelRuntime,ModelRuntimeSlot *sourceModelRuntime)

{
  uint32_t candidateRadius;
  ModelRuntimeNode *sourceNode;
  uint32_t anchorDistance;
  ModelPackedPointRecord *anchorRecord;
  ModelWorldPoint anchorPoint;
  ModelRuntimeNode *candidateNode;

  if (sourceModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_13) {
    return false;
  }
  /* the candidate definition's radius (footprintRadiusCopy) */
  candidateRadius = candidateModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadiusCopy;
  sourceNode = sourceModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  candidateNode = candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if (!ModelLookupTable_FindPackedPoint(1,5,(sourceNode->modelPayload).modelResource,&anchorRecord)) {
    return false;
  }
  anchorPoint = ModelNodeRuntime_TransformLocalPoint(anchorRecord,sourceNode);
  anchorDistance = FixedMath_Length2(anchorPoint.yQ12 - (candidateNode->worldTransform).translation.y,
                                     anchorPoint.xQ12 - (candidateNode->worldTransform).translation.x);
  return (int)anchorDistance <= (int)(candidateRadius + 3 * Q12_ONE / 4);
}

/* Xenite refund for the not yet launched linked assets of a class-22 pad that is being dismantled (called
   directly by ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive): sums xeniteValueQ4 of the
   faction's model definition of every linked asset whose slot bit is still clear, divided by 32 (the same
   rate as the pad's own refund).
*/
uint32_t ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric(ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  FactionRuntimeIndex factionIndex;
  int remainingSlots;
  uint32_t metricSum;
  uint32_t slotBit;
  int slotIndex;
  Q12 *linkedAssetIds;
  ArmyAssetRecordPrefix *assetRecord;
  ModelDefinitionRecordPrefix *selectedDefinition;

  metricSum = 0;
  factionIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  slotBit = 1;
  slotIndex = 0;
  remainingSlots = ((ModelDefinition *)armyRuntime->definitionOrAsset)->classParameterC4;
  /* the linked asset ids are consecutive dwords starting at movementTarget0Q12 */
  linkedAssetIds = &armyRuntime->movementTarget0Q12;
  /* Original quirk: a do/while, so slot 0 is always visited; a classParameterC4 of 0 would run on until the
     counter wraps (the pad definitions all have linked slots). */
  do {
    if (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit) == 0) {
      if (ArmyAssetRegistry_FindById(linkedAssetIds[slotIndex],&assetRecord) == 0) {
        selectedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,assetRecord->rootNodeOffsetOrPointer); /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
        metricSum = metricSum + ((ModelDefinition *)selectedDefinition)->xeniteValueQ4;
      }
    }
    slotIndex++;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots - 1;
  } while (remainingSlots != 0);
  return metricSum >> 5;
}

/* EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL: only an owner whose model definition has class 18 turns into the
   army asset named by classParameterC0. The new model keeps the owner's armour points (ModelRuntimeSlot.health) in proportion,
   rescaled by the two definitions' maximumHealth (the full armour), and the owner is destroyed. */
void EffectLifecycle_SpawnArmyFromOwner(WorldRuntimeContext *worldRuntime,GameEntityRuntime *ownerEntity)

{
  ModelRuntimeSlot *ownerModelSlot;
  ModelRuntimeNode *ownerModelNode;
  ModelDefinition *ownerDefinition;
  ArmyRuntimeSlot *createdArmy;
  ModelRuntimeSlot *createdModelSlot;

  if (ownerEntity == nullptr) {
    return;
  }
  ownerModelSlot = (ModelRuntimeSlot *)ownerEntity->common.ownership.definitionOrClassRecord;
  ownerModelNode = ownerEntity->common.ownership.modelNode;
  ownerDefinition = ownerModelSlot->definitionOrSavedId.runtimeDefinition;
  if (ownerDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_18) {
    return;
  }
  createdArmy = ArmyRuntime_CreateInstanceFromAsset
                     (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,
                      ownerModelNode->modelPayload.worldRotationAngle2,
                      ownerModelNode->worldTransform.translation.y,
                      ownerModelNode->worldTransform.translation.x,
                      ownerEntity->common.ownership.ownerIndex,
                      (PckArmyAssetIdCatalog)ownerDefinition->classParameterC0,
                      worldRuntime,nullptr);
  if (createdArmy != nullptr) {
    createdModelSlot = createdArmy->modelRuntimeOrSavedOffset.modelRuntime;
    createdModelSlot->health =
         (int)(((int64_t)(int)ownerModelSlot->health *
                (int64_t)(int)createdModelSlot->definitionOrSavedId.runtimeDefinition->maximumHealth) /
               (int64_t)(int)ownerDefinition->maximumHealth);
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,ownerEntity);
  }
}
