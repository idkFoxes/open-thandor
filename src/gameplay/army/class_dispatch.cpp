/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/class_dispatch.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/class_dispatch.h>
#include <thandor/gameplay/army/model_views.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Army entry of the terrainStateRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that
   table): re-registers the owning army's terrain occupancy flags and refreshes the state tint of the model.
*/
void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime)

{
  ArmyRuntime_InitializeTerrainOccupancyFlags
            (worldRuntime,
             (ArmyRuntimeSlot *)
             ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime);
  ModelNodeRuntime_UpdateStateTintRecursive(modelNodeRuntime);
  return;
}

/* Army entry of the audioRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table):
   runs the class sound callbacks (classMethodD) over the model hierarchy of the owner-list node, starting at its
   model runtime (runtimePayload).
*/
void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode)

{
  ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(worldRuntime,(ModelRuntimeSlot *)ownerNode->runtimePayload);
  return;
}

/* Army entry of the primaryUpdate phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table,
   once per simulation step and owner-list node): updates the army's model hierarchy, lets the AI pick targets
   for non-neutral factions, drops a timed-out target command, clears the LOCKED movement flag once nothing
   links to the model any more and counts down movementRetryCountdown.
*/
void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode)

{
  ArmyCommandGeneration *commandGenerationField;
  ModelRuntimeSlot *modelRuntime;
  ArmyRuntimeSlot *armyRuntime;
  int ownerFactionIndex;
  
  modelRuntime = (ModelRuntimeSlot *)ownerNode->runtimePayload;
  armyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  ownerFactionIndex = armyRuntime->factionIndex;
  ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,modelRuntime);
  if (ownerFactionIndex != 0) {
    AiCombatDecision_UpdateTargetAssignment(worldRuntime,armyRuntime);
  }
  commandGenerationField = &armyRuntime->commandGeneration;
  *commandGenerationField = *commandGenerationField - g_InGameSimulationStepTicks;
  if ((int)*commandGenerationField < 0) {
    armyRuntime->commandModeFlags =
         armyRuntime->commandModeFlags & ~(ARMY_COMMAND_MODE_INTERRUPTED | ARMY_COMMAND_MODE_AI_COMBAT_TARGET);
  }
  if (((armyRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) != 0) &&
     ((modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime == nullptr)) {
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & ~ARMY_MOVEMENT_LOCKED;
  }
  if (armyRuntime->movementRetryCountdown != 0) {
    armyRuntime->movementRetryCountdown = armyRuntime->movementRetryCountdown - 1;
  }
  return;
}

/* World owner-list callback: for a model node, clears its runtime flags 0x4 and 0x8, re-registers the owning
   army's terrain occupancy and refreshes the node's state tint. Second pass after
   ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback when a level's armies are set up.
*/
void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    node->runtimeFlags = node->runtimeFlags & ~(TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE | TERRAIN_OCCUPANCY_FLAG_PRESENT);
    /* the owning army runtime (+8) of the node's model runtime */
    ArmyRuntime_InitializeTerrainOccupancyFlags
              (armyContext,((ModelRuntimeSlot *)node->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    ModelNodeRuntime_RefreshStateTint((ModelRuntimeNode *)node);
  }
}

/* World owner-list callback: for a model node of an owned army, adds the army's projected terrain occlusion
   (its depthBinClass mask in the byte of every faction whose nibble in the owner's packed relation states has bit 3
   set) around the node, and marks occupancy bit 2 around it when the active faction's nibble has bit 3 set. First pass of the occupancy rebuild; see
   ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback.
*/
void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *node)

{
  ArmyRuntimeSlot *ownerArmy;
  uint32_t relationStates;
  int activeFactionIndex;
  int factionIndex;
  uint64_t visibilityMask;
  uint64_t factionMaskByte;
  Q12 worldXQ12;
  Q12 worldYQ12;
  int ownerFactionIndex;
  FieldGridAsset *fieldGrid;

  if (node->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
    return;
  }
  /* node->worldXQ12 goes to the callees' worldYQ12 and node->worldYQ12 to their worldXQ12,
     as in the original; one of the two namings is swapped. */
  worldYQ12 = node->worldXQ12;
  ownerArmy = ((ModelRuntimeSlot *)node->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  worldXQ12 = node->worldYQ12;
  /* faction 0 = none */
  if (ownerArmy->factionIndex == 0) {
    return;
  }
  ownerFactionIndex = ownerArmy->factionIndex;
  fieldGrid = worldRuntime->fieldGrid;
  relationStates = g_GameFactionRuntimeImage.records[ownerFactionIndex].packedRelationStates;
  factionMaskByte = (uint64_t)(uint32_t)ownerArmy->depthBinClass;
  /* one nibble per faction in relationStates, one byte per faction in the 64-bit mask (faction 7 in the top
     byte); faction 0 (the lowest byte) is not tested and stays zero */
  visibilityMask = 0;
  for (factionIndex = 7; factionIndex >= 1; factionIndex--) {
    visibilityMask = visibilityMask << 8;
    if ((relationStates & FACTION_RELATION_PACKED(FACTION_RELATION_STATE_ALLIED,factionIndex)) != 0) {
      visibilityMask = visibilityMask | factionMaskByte;
    }
  }
  activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
  TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
            (visibilityMask << 8,ownerArmy->visibilityRadius,node->worldZQ12 + ownerArmy->visibilityHeightOffset,
             worldXQ12,worldYQ12,worldRuntime->fieldGrid);
  if ((relationStates >> ((uint8_t)(activeFactionIndex << 2) & 31) & FACTION_RELATION_STATE_ALLIED) != 0) {
    TerrainOccupancyBit2_MarkAroundWorldPoint
              (ownerArmy->occupancyMarkRadius,worldXQ12,worldYQ12,ownerFactionIndex,fieldGrid);
  }
}

/* Runs the placement-validation handler of the army's runtime class
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation, indexed by the definition's
   runtimeClassId) for the army in *armyRuntimeHolder and returns its acceptance.
*/
Bool8 ArmyRuntimeNode_DispatchTypedCallback(Ptr32<ArmyRuntimeSlot> *armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  Bool8 accepted;

  /* the view's first field (modelDefinition) is the army's model runtime pointer */
  accepted = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation
            [((ModelRuntimePlacementValidationView *)*armyRuntimeHolder)->modelDefinition->
             runtimeClassId])
                    (worldRuntime,(ModelRuntimePlacementValidationView *)*armyRuntimeHolder);
  return accepted;
}

/* Runs the class-command handler of the runtime class of the army's model runtime (the definition's
   runtimeClassId) with that model runtime; the AI planners call it to start the class-specific behaviour of the armies
   they create or re-task.
*/
void ArmyRuntime_DispatchClassCommand(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
    [armyRuntime->modelRuntimeOrSavedOffset.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId])
            (worldRuntime,armyRuntime->modelRuntimeOrSavedOffset.modelRuntime);
  return;
}

/* Per-step update of one model runtime and, recursively, its attached children (called by
   ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers, the primaryUpdate entry of g_RuntimeMaintenanceCallbackPhases).
   Runs the class callback runtimeUpdate[class] of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, then the
   shared bookkeeping: health regeneration, dismantling (Xenite refund while health drains, destruction effect
   at zero), the attachment channel ticks of a model without health, and technology research (Xenite paid once
   when affordable, the Energy load held until the technology is unlocked).
*/
void ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;
  uint32_t previousHealth;
  uint32_t healthLimit;
  uint32_t clampedHealth;
  uint32_t healthDrain;
  uint32_t linkedAssetRefund;
  ModelRuntimeNode *rootNode;
  int factionIndex;
  int researchProgress;
  int researchEnergyLoad;
  int energyRequirement;
  uint32_t ticksRemaining;
  uint32_t attachmentCount;
  uint32_t attachmentIndex;
  uint32_t parentStateFlags;
  ModelRuntimeSlot *childModelRuntime;

  definition = (modelRuntime->definitionOrSavedId).runtimeDefinition;
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[definition->runtimeClassId]
            (worldRuntime,modelRuntime);
  /* every 4 ticks health regenerates by healthRegenerationPerStep up to 3/4 of the definition's
     health (maximumHealth), or decays down to it while bit 0 is set */
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DISMANTLING) == 0) {
    (modelRuntime->classState).healthRegenerationDelayTicks -= g_InGameSimulationStepTicks;
    if ((int)(modelRuntime->classState).healthRegenerationDelayTicks < 0) {
      (modelRuntime->classState).healthRegenerationDelayTicks = 4;
      previousHealth = modelRuntime->health;
      healthLimit = (definition->maximumHealth * 3) >> 2;
      if (previousHealth != 0) {
        if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
          if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_NO_REGENERATION) == 0) {
            clampedHealth = previousHealth + definition->healthRegenerationPerStep;
            if ((int)healthLimit < (int)clampedHealth) {
              clampedHealth = healthLimit;
            }
            if ((int)modelRuntime->health < (int)clampedHealth) {
              modelRuntime->health = clampedHealth;
            }
          }
        }
        else {
          clampedHealth = previousHealth - definition->healthRegenerationPerStep;
          if ((int)clampedHealth < (int)healthLimit) {
            clampedHealth = healthLimit;
          }
          if ((int)clampedHealth < (int)modelRuntime->health) {
            modelRuntime->health = clampedHealth;
          }
        }
      }
    }
  }
  (modelRuntime->classState).dismantleTickCountdown -= g_InGameSimulationStepTicks;
  if ((int)(modelRuntime->classState).dismantleTickCountdown < 0) {
    (modelRuntime->classState).dismantleTickCountdown += 12;
    if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DISMANTLING) != 0) &&
        (definition->xeniteValueQ4 != 0) && (definition->maximumHealth != 0) &&
        (0 < (int)modelRuntime->health)) {
      /* dismantling, every 12 ticks: refund 1/32 of the Xenite value and drain 1/16 of the health */
      factionIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
      g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + (definition->xeniteValueQ4 >> 5);
      healthDrain = definition->maximumHealth >> 4;
      if (definition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        /* a pad also refunds its unlaunched linked assets */
        linkedAssetRefund = ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
                              ((ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
             g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + linkedAssetRefund;
      }
      previousHealth = modelRuntime->health;
      modelRuntime->health -= healthDrain;
      if ((int)previousHealth <= (int)healthDrain) { /* signed compare of the old health */
        modelRuntime->health = 0;
        (modelRuntime->classState).stateFlags ^= (ARMY_MODEL_STATE_DISMANTLING | ARMY_MODEL_STATE_DISMANTLED);
        rootNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                   THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelRuntime = modelRuntime },
                   (rootNode->modelPayload).worldRotationAngle2,
                   (rootNode->modelPayload).worldRotationAngle1,
                   (rootNode->modelPayload).worldRotationAngle0,(rootNode->worldTransform).translation.z,
                   (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,
                   definition->removalEffectDefinitionReference.definition,worldRuntime);
        /* Setting it also skips the attachment tick loop below (the original jumps past it). */
        (modelRuntime->classState).stateFlags |= ARMY_MODEL_STATE_DESTRUCTION_STARTED;
      }
    }
  }
  /* health gone (and not already exploding): tick the attachment channels once per simulation tick
     (only when a linked model runtime exists) */
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0) &&
      ((int)modelRuntime->health < 1) &&
      ((modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime != nullptr)) {
    /* Original quirk: the body runs once before the counter is tested, so a step of 0 ticks wraps around. */
    ticksRemaining = g_InGameSimulationStepTicks;
    do {
      ArmyRuntime_ProcessReadyAttachmentChannels(worldRuntime,modelRuntime);
      modelRuntime->destructionEffectTimers[0] = modelRuntime->destructionEffectTimers[0] - 1;
      modelRuntime->destructionEffectTimers[1] = modelRuntime->destructionEffectTimers[1] - 1;
      modelRuntime->destructionEffectTimers[2] = modelRuntime->destructionEffectTimers[2] - 1;
      modelRuntime->destructionEffectTimers[3] = modelRuntime->destructionEffectTimers[3] - 1;
      modelRuntime->destructionEffectTimers[4] = modelRuntime->destructionEffectTimers[4] - 1;
      modelRuntime->destructionEffectTimers[5] = modelRuntime->destructionEffectTimers[5] - 1;
      modelRuntime->destructionEffectTimers[6] = modelRuntime->destructionEffectTimers[6] - 1;
      modelRuntime->destructionEffectTimers[7] = modelRuntime->destructionEffectTimers[7] - 1;
      ticksRemaining--;
    } while (ticksRemaining != 0);
  }
  /* research progress */
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) &&
      (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0)) {
    researchProgress = modelRuntime->researchElapsedTicks + g_InGameSimulationStepTicks;
    modelRuntime->researchElapsedTicks = researchProgress;
    if (modelRuntime->researchDurationTicks <= researchProgress) {
      Technology_UnlockForFaction
                ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.y,
                 (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.x,modelRuntime->researchTechnologyId,
                 ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex);
      researchEnergyLoad = modelRuntime->researchEnergyLoadQ4;
      (modelRuntime->classState).stateFlags &= ~ARMY_MODEL_STATE_RESEARCHING;
      (modelRuntime->classState).energyLoadQ4 -= researchEnergyLoad;
    }
  }
  /* queued research starts once its Xenite cost can be paid */
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCH_UNPAID) != 0) &&
     (((modelRuntime->classState).stateFlags & (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_PRODUCING)) == 0)) {
    factionIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    energyRequirement = modelRuntime->researchEnergyLoadQ4;
    if (modelRuntime->researchXeniteCostQ4 <=
        (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4) {
      g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 -
           modelRuntime->researchXeniteCostQ4;
      modelRuntime->researchXeniteCostQ4 = 0;
      (modelRuntime->classState).stateFlags ^= (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID);
      (modelRuntime->classState).energyLoadQ4 += energyRequirement;
    }
  }
  /* recurse into the attached child models, passing bit 8 on to them */
  attachmentCount = modelRuntime->attachmentCount;
  parentStateFlags = (modelRuntime->classState).stateFlags;
  for (attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++) {
    childModelRuntime = modelRuntime->attachments[attachmentIndex].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != nullptr) {
      (childModelRuntime->classState).stateFlags |= parentStateFlags & 8;
      ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,childModelRuntime);
    }
  }
}

/* Runs the sound callback classMethodD[class] of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes for a model
   runtime and recursively for its attached child models (called by
   ArmyRuntimeMaintenance_DispatchClassMethodDRecursive, the audioRefresh entry of
   g_RuntimeMaintenanceCallbackPhases).
*/
void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  uint32_t childCount;
  uint32_t childIndex;
  ModelRuntimeSlot *childModelRuntime;

  /* the attachment count is read before the class callback runs */
  childCount = modelRuntime->attachmentCount;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD
    [modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId])
            (worldRuntime,modelRuntime);
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    childModelRuntime = modelRuntime->attachments[childIndex].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != nullptr) {
      ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(worldRuntime,childModelRuntime);
    }
  }
}

/* Adapters of placementAssetClassDispatch: the slot passes the clearance padding as uint32_t and the definition
   as its ModelDefinitionRecordPrefix; these placement tests take the padding as a signed
   ArmyPlacementClearancePaddingQ12 (same bits) and the full ModelDefinition (the record behind the prefix). */
static Bool8 ArmyPlacementSlot_CanPlaceBuilding
          (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
          uint32_t terrainHeightQ12,Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
          uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outPlacementValue)
{
  return ArmyPlacement_CanPlaceBuilding
           (placementMode,(ArmyPlacementClearancePaddingQ12)placementClearancePaddingQ12,placementHeading,
            terrainHeightQ12,worldYQ12,worldXQ12,(ModelDefinition *)modelDefinition,ownerFactionIndex,worldRuntime,
            outPlacementValue);
}

static Bool8 ArmyPlacementSlot_CanPlaceAnchoredModel
          (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
          uint32_t terrainHeightQ12,Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
          uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outPlacementValue)
{
  return ArmyPlacement_CanPlaceAnchoredModel
           (placementMode,(ArmyPlacementClearancePaddingQ12)placementClearancePaddingQ12,placementHeading,
            terrainHeightQ12,worldYQ12,worldXQ12,(ModelDefinition *)modelDefinition,ownerFactionIndex,worldRuntime,
            outPlacementValue);
}

static Bool8 ArmyPlacementSlot_CanPlaceResourceExtractor
          (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
          uint32_t terrainHeightQ12,Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
          uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outPlacementValue)
{
  return ArmyPlacement_CanPlaceResourceExtractor
           (placementMode,(ArmyPlacementClearancePaddingQ12)placementClearancePaddingQ12,placementHeading,
            terrainHeightQ12,worldYQ12,worldXQ12,modelDefinition,ownerFactionIndex,worldRuntime,
            outPlacementValue);
}

ArmyRuntimeOrderHandlerMatrix11x24 g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes = {
    .runtimeUpdate = {
        /*  0 */ THANDOR_SLOT(ArmyRuntime_UpdateTimedShotAndEffectEmitters),
        /*  1 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateGroundMovement),
        /*  2 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation),
        /*  3 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateArticulatedMovement),
        /*  4 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage),
        /*  5 */ THANDOR_SLOT(ArmyRuntimeClass_NoOpTickUpdate),
        /*  6 */ THANDOR_SLOT(ArmyRuntimeClass_NoOpTickUpdate),
        /*  7 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateSingleBarrelTurret),
        /*  8 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateTwinBarrelTurret),
        /*  9 */ THANDOR_SLOT(ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments),
        /* 10 */ THANDOR_SLOT(ArmyRuntime_UpdateAnimatedModelSubnodes),
        /* 11 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateStructureFactory),
        /* 12 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy),
        /* 13 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateUnitFactory),
        /* 14 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateGridBoundEffectsAndModels),
        /* 15 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateTransformAndDamageEffect),
        /* 16 */ THANDOR_SLOT(ArmyRuntime_UpdateTimedShotAndEffectEmitters),
        /* 17 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation),
        /* 18 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement),
        /* 19 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateWaterSurfaceMovement),
        /* 20 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects),
        /* 21 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateAircraft),
        /* 22 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode),
        /* 23 */ THANDOR_SLOT(ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState)
    },
    .classMethodD = {
        /*  0 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD),
        /*  1 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurnAndMoveSounds),
        /*  2 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurnAndMoveSounds),
        /*  3 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD),
        /*  4 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled),
        /*  5 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  6 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  7 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  8 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  9 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 10 */ THANDOR_SLOT(ArmyRuntime_UpdateLoopingPositionedSound),
        /* 11 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateStructureFactorySound),
        /* 12 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 13 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateUnitFactorySounds),
        /* 14 */ THANDOR_SLOT(ArmyRuntime_UpdateLoopingPositionedSound),
        /* 15 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 16 */ THANDOR_SLOT(ArmyRuntime_UpdateLoopingPositionedSound),
        /* 17 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurnAndMoveSounds),
        /* 18 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurnAndMoveSounds),
        /* 19 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateTurnAndMoveSounds),
        /* 20 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 21 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateAssetProjectedSound),
        /* 22 */ THANDOR_SLOT(ArmyRuntimeAudio_UpdateLinkedChildPadSounds),
        /* 23 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpD)
    },
    .modelUnrebase = {
        /*  0 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  1 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  2 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  3 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  4 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  5 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  6 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  7 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  8 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  9 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 10 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 11 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 12 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 13 */ THANDOR_SLOT(ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C),
        /* 14 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 15 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 16 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 17 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 18 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 19 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 20 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 21 */ THANDOR_SLOT(ModelRuntimeSlot_UnrebaseClassModelLinkOffset60),
        /* 22 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 23 */ THANDOR_SLOT(UnifiedRuntimeDefault_OneArgNoOpC)
    },
    .modelRebaseOrLoadRepair = {
        /*  0 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  1 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  2 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  3 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  4 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  5 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  6 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  7 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  8 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /*  9 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 10 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 11 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 12 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 13 */ THANDOR_SLOT(ModelRuntimeSlot_RebaseClassArmyLinkOffset6C),
        /* 14 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 15 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 16 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 17 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 18 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 19 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 20 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 21 */ THANDOR_SLOT(ModelRuntimeSlot_RebaseClassModelLinkOffset60),
        /* 22 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp),
        /* 23 */ THANDOR_SLOT(ModelRuntimeSlotPointerRebase_NoOp)
    },
    .modelClassInitialize = {
        /*  0 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  1 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  2 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices),
        /*  3 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming),
        /*  4 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  5 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  6 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  7 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  8 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /*  9 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters),
        /* 10 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /* 11 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_ResetStructureFactoryBuild),
        /* 12 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_ClearField60),
        /* 13 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0),
        /* 14 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3),
        /* 15 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1),
        /* 16 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity),
        /* 17 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform),
        /* 18 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /* 19 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /* 20 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_NoOp),
        /* 21 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset),
        /* 22 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation),
        /* 23 */ THANDOR_SLOT(ModelRuntimeSlotClassInit_ClearFields60AndB8)
    },
    .modelReleaseOrCommit = {
        /*  0 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  1 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  2 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  3 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  4 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  5 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  6 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  7 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  8 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  9 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 10 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 11 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 12 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 13 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 14 */ THANDOR_SLOT(ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation),
        /* 15 */ THANDOR_SLOT(ArmyPlacement_ReleaseFactionCapacity),
        /* 16 */ THANDOR_SLOT(ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity),
        /* 17 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 18 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 19 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 20 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 21 */ THANDOR_SLOT(ArmyPlacement_ReleaseClassStateReservation),
        /* 22 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 23 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgNoOpB)
    },
    .placementValidation = {
        /*  0 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  1 */ THANDOR_SLOT(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /*  2 */ THANDOR_SLOT(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /*  3 */ THANDOR_SLOT(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /*  4 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /*  5 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  6 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  7 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  8 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  9 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /* 10 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /* 11 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /* 12 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /* 13 */ THANDOR_SLOT(ArmyPlacement_TestModelTerrainAndRuntimeClearance),
        /* 14 */ THANDOR_SLOT(ArmyPlacement_TestGridOccupancyMask),
        /* 15 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /* 16 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /* 17 */ THANDOR_SLOT(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /* 18 */ THANDOR_SLOT(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /* 19 */ THANDOR_SLOT(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /* 20 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /* 21 */ THANDOR_SLOT(UnifiedRuntimeDefault_TwoArgSuccess),
        /* 22 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime),
        /* 23 */ THANDOR_SLOT(ArmyPlacementCollision_TestCurrentRuntime)
    },
    .placementAssetClassDispatch = {
        /*  0 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /*  1 */ THANDOR_SLOT(ArmyPlacement_CanPlaceMobileUnit),
        /*  2 */ THANDOR_SLOT(ArmyPlacement_CanPlaceMobileUnit),
        /*  3 */ THANDOR_SLOT(ArmyPlacement_CanPlaceMobileUnit),
        /*  4 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /*  5 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /*  6 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /*  7 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /*  8 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /*  9 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /* 10 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /* 11 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /* 12 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /* 13 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceAnchoredModel),
        /* 14 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceResourceExtractor),
        /* 15 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /* 16 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /* 17 */ THANDOR_SLOT(ArmyPlacement_CanPlaceMobileUnit),
        /* 18 */ THANDOR_SLOT(ArmyPlacement_CanPlaceMobileUnit),
        /* 19 */ THANDOR_SLOT(ArmyPlacement_CanPlaceMobileUnit),
        /* 20 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /* 21 */ THANDOR_SLOT(ArmyPlacement_CanPlaceAnywhere),
        /* 22 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding),
        /* 23 */ THANDOR_SLOT(ArmyPlacementSlot_CanPlaceBuilding)
    },
    .classCommand = {
        /*  0 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  1 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  2 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  3 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  4 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /*  5 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  6 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  7 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  8 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /*  9 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /* 10 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 11 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 12 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /* 13 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 14 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 15 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 16 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 17 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /* 18 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /* 19 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /* 20 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 21 */ THANDOR_SLOT(ArmyRuntimeClassCommand_NoOp),
        /* 22 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 23 */ THANDOR_SLOT(ArmyRuntime_ClassCommandHandlerGroupA)
    },
    .gridInfluenceAdd = {
        /*  0 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /*  1 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /*  2 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /*  3 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /*  4 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /*  5 */ THANDOR_SLOT(GridInfluence_AddNoOp),
        /*  6 */ THANDOR_SLOT(GridInfluence_AddNoOp),
        /*  7 */ THANDOR_SLOT(GridInfluence_AddNoOp),
        /*  8 */ THANDOR_SLOT(GridInfluence_AddNoOp),
        /*  9 */ THANDOR_SLOT(GridInfluence_AddNoOp),
        /* 10 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 11 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 12 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 13 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 14 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 15 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 16 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 17 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /* 18 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /* 19 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands),
        /* 20 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 21 */ THANDOR_SLOT(GridInfluence_AddNoOp),
        /* 22 */ THANDOR_SLOT(GridInfluence_AddLowDistanceBands),
        /* 23 */ THANDOR_SLOT(GridInfluence_AddHighDistanceBands)
    },
    .gridInfluenceRemove = {
        /*  0 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /*  1 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /*  2 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /*  3 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /*  4 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /*  5 */ THANDOR_SLOT(GridInfluence_RemoveNoOp),
        /*  6 */ THANDOR_SLOT(GridInfluence_RemoveNoOp),
        /*  7 */ THANDOR_SLOT(GridInfluence_RemoveNoOp),
        /*  8 */ THANDOR_SLOT(GridInfluence_RemoveNoOp),
        /*  9 */ THANDOR_SLOT(GridInfluence_RemoveNoOp),
        /* 10 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 11 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 12 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 13 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 14 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 15 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 16 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 17 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /* 18 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /* 19 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands),
        /* 20 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 21 */ THANDOR_SLOT(GridInfluence_RemoveNoOp),
        /* 22 */ THANDOR_SLOT(GridInfluence_RemoveLowDistanceBands),
        /* 23 */ THANDOR_SLOT(GridInfluence_RemoveHighDistanceBands)
    }};

RuntimeMaintenanceCallbackPhasesTyped g_RuntimeMaintenanceCallbackPhases = {
    .primaryUpdate = {.army = THANDOR_SLOT(ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers), .shot = THANDOR_SLOT(ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects), .effect = THANDOR_SLOT(EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions)},
    .terrainStateRefresh = {.army = THANDOR_SLOT(ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint), .shot = THANDOR_SLOT(ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint), .effect = THANDOR_SLOT(EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint)},
    .occupancyRebuild = {.army = THANDOR_SLOT(ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback), .shot = THANDOR_SLOT(ShotRuntimeMaintenance_OccupancyRebuildNoOp), .effect = THANDOR_SLOT(EffectRuntimeMaintenance_OccupancyRebuildNoOp)},
    .audioRefresh = {.army = THANDOR_SLOT(ArmyRuntimeMaintenance_DispatchClassMethodDRecursive), .shot = THANDOR_SLOT(ShotRuntimeMaintenance_UpdateHierarchyProjectedSound), .effect = THANDOR_SLOT(EffectRuntimeMaintenance_AudioRefreshNoOp)}};

/* Per-tick update of army classes 5 and 6 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[5] and
   [6]): those classes have nothing to update, so this does nothing.
*/
void ArmyRuntimeClass_NoOpTickUpdate(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  return;
}

/* Default model-unrebase handler (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase, every class
   except 13 and 21): those classes keep no pointers that need unrebasing, so this does nothing.
*/
void UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Default model release/commit handler (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit,
   every class except 14-16 and 21): those classes hold no faction capacity or placement reservation to release,
   so this does nothing.
*/
void UnifiedRuntimeDefault_TwoArgNoOpB
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Default placement validation (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation, classes
   0, 5-9, 12 and 21): accepts every placement.
*/
Bool8 UnifiedRuntimeDefault_TwoArgSuccess
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  return false;
}

/* Default class method D (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD, classes 0, 3, 9,
   12, 15, 20 and 23), the slot where the other classes update their looping and positioned sounds: these classes
   have none, so this does nothing.
*/
void UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  return;
}
