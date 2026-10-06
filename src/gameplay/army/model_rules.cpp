/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/model_rules.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Gameplay rules evaluated on a model hierarchy: armour sums and destroyed marking, condition, energy and
   selection metrics, faction technology variants and the turret yaw/pitch aim. */

#include <thandor/gameplay/army/model_rules.h>
#include <thandor/thandor.h>

static void ModelRuntimeHierarchy_MarkDestroyedFrom(ModelRuntimeSlot *node);

/* Switches the models of an army to the variants its faction's technology selects: runs
   ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive on the army's model runtime hierarchy.
*/
void ModelRuntimeHierarchy_ApplyFactionTechnologyVariants(FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
            (factionIndex,(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
}

/* Marks every not yet destroyed node of the army's model hierarchy (root modelRuntimeOrSavedOffset.modelRuntime) as destroyed, dismantling
   and non-regenerating: sets runtime flags 0x418 (0x400 | 0x10 | 0x08) on each node that does not have flag
   0x08 yet. The world context is not used.
*/
void ModelRuntimeHierarchy_MarkDestroyedRecursive(WorldRuntimeContext *contextArg,ArmyRuntimeSlot *armyRuntime)

{
  (void)contextArg;
  ModelRuntimeHierarchy_MarkDestroyedFrom(armyRuntime->modelRuntimeOrSavedOffset.modelRuntime);
}

/* Model runtime nodes keep their child count in attachmentCount and child pointers in
   attachments[i].childModelRuntimeOrSavedOffset (null slots are skipped); the original walks this tree depth-first with frames on the machine stack. */
static int ModelRuntimeHierarchy_SumArmourFrom(ModelRuntimeSlot *node)
{
  int sum = node->health;
  int childCount = node->attachmentCount;
  int i;
  for (i = 0; i < childCount; i++) {
    ModelRuntimeSlot *child = node->attachments[i].childModelRuntimeOrSavedOffset;
    if (child != nullptr) {
      sum = sum + ModelRuntimeHierarchy_SumArmourFrom(child);
    }
  }
  return sum;
}

/* Body of ModelRuntimeHierarchy_MarkDestroyedRecursive: ORs 0x418 into the runtime flags (classState.stateFlags) of
   node unless flag 0x08 is already set, then recurses into the non-NULL children (same layout as above). */
static void ModelRuntimeHierarchy_MarkDestroyedFrom(ModelRuntimeSlot *node)
{
  int childCount;
  int childIndex;
  if ((node->classState.stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) {
    node->classState.stateFlags = node->classState.stateFlags | (ARMY_MODEL_STATE_NO_REGENERATION | ARMY_MODEL_STATE_DISMANTLING | ARMY_RUNTIME_FLAG_DESTROYED);
  }
  childCount = node->attachmentCount;
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    ModelRuntimeSlot *child = node->attachments[childIndex].childModelRuntimeOrSavedOffset;
    if (child != nullptr) {
      ModelRuntimeHierarchy_MarkDestroyedFrom(child);
    }
  }
}

/* Returns the armour of a model hierarchy (shown in the in-game selection detail): the sum of the current
   armour points (ModelRuntimeSlot.health) of every node, walked depth-first.
*/
int ModelRuntimeHierarchy_SumArmour(int *modelRuntimeRoot)

{
  return ModelRuntimeHierarchy_SumArmourFrom(Thandor_U32ToPointer<ModelRuntimeSlot>(*modelRuntimeRoot));
}

/* Folds one model runtime and its attached children into the owning army's selection figures (cleared by
   ArmyRuntime_RebuildDerivedSelectionMetrics): maxima in the army's occupancyMarkRadius, visibilityRadius and
   visibilityHeightOffset, the largest shot selection range in weaponRangeQ12 and, for armed models, the shot's
   impact damage per target class summed into targetClassShotDamage[8].
*/
void ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *modelRuntimeSlot;
  ModelDefinition *modelDefinition;
  ArmyRuntimeSlot *army;
  ModelRuntimeNode *rootModelNode;
  ShotDefinition *shotDefinition;
  ModelRuntimeAttachmentDescriptor *attachment;
  uint32_t visibilityRadius;
  uint32_t visibilityHeightOffset;
  uint32_t selectionRange;
  int targetClassIndex;
  int childrenRemaining;

  modelRuntimeSlot = modelRuntime;
  modelDefinition = modelRuntimeSlot->definitionOrSavedId.runtimeDefinition;
  army = modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  rootModelNode = modelRuntimeSlot->rootModelNodeOrSavedOffset.modelNode;
  /* a switched-off model counts with the definition's alternative value switchedOffVisibilityRadius */
  if ((modelRuntimeSlot->classState.stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
    visibilityRadius = modelDefinition->visibilityRadius;
  }
  else {
    visibilityRadius = modelDefinition->switchedOffVisibilityRadius;
  }
  if (army->occupancyMarkRadius < modelDefinition->occupancyMarkRadius) {
    army->occupancyMarkRadius = modelDefinition->occupancyMarkRadius;
  }
  if (army->visibilityRadius < visibilityRadius) {
    army->visibilityRadius = visibilityRadius;
  }
  visibilityHeightOffset = (rootModelNode->worldTransform.translation.z -
                            army->modelNodeRuntime->worldTransform.translation.z) +
                           modelDefinition->visibilityHeightOffset;
  if (army->visibilityHeightOffset < visibilityHeightOffset) {
    army->visibilityHeightOffset = visibilityHeightOffset;
  }
  /* the model's shot definition, used when reloadTicks is non-zero */
  shotDefinition = modelDefinition->shotDefinitionReference.definition;
  if (modelDefinition->reloadTicks != 0) {
    selectionRange = ShotDefinition_ComputeSelectionRange(shotDefinition);
    army = modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if ((int)army->weaponRangeQ12 < (int)selectionRange) {
      army->weaponRangeQ12 = selectionRange;
    }
    /* all 8 target classes, last one first */
    for (targetClassIndex = 7; targetClassIndex >= 0; targetClassIndex--) {
      army->targetClassShotDamage[targetClassIndex] += shotDefinition->targetClassImpactDamageQ12[targetClassIndex];
    }
  }
  attachment = modelRuntimeSlot->attachments;
  for (childrenRemaining = modelRuntimeSlot->attachmentCount; childrenRemaining != 0; childrenRemaining--) {
    if (attachment->childModelRuntimeOrSavedOffset != nullptr) {
      ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(attachment->childModelRuntimeOrSavedOffset);
    }
    attachment++;
  }
}

/* Condition of a model hierarchy as a Q12 ratio: the node's armour points (health) relative to its
   definition's maximumHealth, multiplied by the average of 1.0 and the ratios of all attached child
   hierarchies; Q12_ONE is full condition. Unrelated to the draw scale ModelRuntimeNode.modelScaleQ12.
*/
Q12 ModelRuntimeHierarchy_ComputeConditionRatioQ12(ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *childModelRuntime;
  uint32_t attachmentsRemaining;
  int ratioSumQ12;
  ModelRuntimeAttachmentDescriptor *attachment;
  int ratioSampleCount;
  Q12 childConditionRatioQ12;

  /* the sum starts with 1.0 for the node itself */
  ratioSumQ12 = Q12_ONE;
  ratioSampleCount = 1;
  attachment = modelRuntime->attachments;
  for (attachmentsRemaining = modelRuntime->attachmentCount; attachmentsRemaining != 0;
      attachmentsRemaining--) {
    childModelRuntime = attachment->childModelRuntimeOrSavedOffset;
    if (childModelRuntime != nullptr) {
      childConditionRatioQ12 = ModelRuntimeHierarchy_ComputeConditionRatioQ12(childModelRuntime);
      ratioSumQ12 = ratioSumQ12 + childConditionRatioQ12;
      ratioSampleCount++;
    }
    attachment++;
  }
  /* health * average ratio / maximum health */
  return (Q12)(((int64_t)(int)modelRuntime->health * (int64_t)ratioSumQ12) /
               (int64_t)(ratioSampleCount *
                         (int)modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth));
}

/* Energy demand of a model and its directly attached models (classState.energyLoadQ4): totalQ4 is the whole demand,
   activeQ4 only the part of models not switched off (stateFlags bit 0). Attached models count only when the
   definition's modelFlags has bit 0x80; the walk is one level deep, not recursive.
*/
ModelHierarchyEnergyDemand
ModelRuntimeHierarchy_ComputeEnergyDemand(ModelRuntimeSlot *modelRuntime)

{
  uint32_t activeMetricTotal;
  uint32_t attachmentsRemaining;
  uint32_t totalMetric;
  ModelRuntimeSlot *currentChildModelRuntime;
  uint32_t childMetric;
  ModelHierarchyEnergyDemand energyDemand;

  totalMetric = modelRuntime->classState.energyLoadQ4;
  attachmentsRemaining = modelRuntime->attachmentCount;
  activeMetricTotal = 0;
  if ((modelRuntime->classState.stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
    activeMetricTotal = totalMetric;
  }
  if ((modelRuntime->definitionOrSavedId.runtimeDefinition->modelFlags &
       MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY) != 0) {
    for (; attachmentsRemaining != 0; attachmentsRemaining--) {
      currentChildModelRuntime = modelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
      if (currentChildModelRuntime != nullptr) {
        childMetric = currentChildModelRuntime->classState.energyLoadQ4;
        if ((currentChildModelRuntime->classState.stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
          activeMetricTotal = activeMetricTotal + childMetric;
        }
        totalMetric = totalMetric + childMetric;
      }
      /* steps the cursor by one 0x20-byte attachments[] entry */
      /* the slot pointer moves by one descriptor's bytes (so attachments[0] is the next descriptor) */
      modelRuntime = reinterpret_cast<ModelRuntimeSlot *>(reinterpret_cast<uint8_t *>(modelRuntime) +
                                                          sizeof(ModelRuntimeAttachmentDescriptor));
    }
  }
  energyDemand.activeQ4 = activeMetricTotal;
  energyDemand.totalQ4 = totalMetric;
  return energyDemand;
}

/* Turns a weapon or turret node's yaw (localRotationAngle2) toward targetYawAngle16 over the shorter way, for
   the army aim updates (ArmyRuntimeClass_UpdateSingleBarrelTurret/TwinBarrelTurret,
   ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments): the turn velocity grows by the weapon definition's
   acceleration up to its rate limit and is reset when it points away; the target is taken exactly once it is
   within one step. Returns true while the remaining difference exceeds +-MODEL_AIM_TOLERANCE_ANGLE16 (still
   outside the aim tolerance), false once the yaw is within it or on the target.
*/

Bool8 ModelNodeRuntime_SmoothYawTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState,
          AngleTurn32 targetYawAngle16)

{
  ArmyWeaponDefinitionView *aimDefinition;
  int turnRateLimit;
  AngleTurn32 currentYawAngle;
  uint32_t yawAngle;
  uint32_t yawStep;
  int acceleratedVelocity;
  uint32_t yawDelta;
  Bool8 snapToTarget;
  uint32_t remainingYawDelta;

  yawAngle = modelNodeRuntime->modelPayload.localRotationAngle2;
  aimDefinition = smoothingState->modelDefinition;
  yawDelta = targetYawAngle16 - yawAngle & FIXED_ANGLE16_MASK;
  yawStep = smoothingState->yawTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  snapToTarget = false;
  if (yawDelta < FIXED_ANGLE16_HALF_TURN + 1) {
    /* target ahead in the positive direction */
    if ((int)yawStep < 0) {
      smoothingState->yawTurnVelocityAngle16 = 0; /* turning away: stop */
    }
    else if (yawDelta <= yawStep) {
      snapToTarget = true;
    }
    else {
      yawAngle = yawAngle + yawStep;
      turnRateLimit = aimDefinition->yawTurnRateLimitAnglePerTick;
      acceleratedVelocity = smoothingState->yawTurnVelocityAngle16 +
              g_InGameSimulationStepTicks * aimDefinition->yawTurnRateAccelerationAnglePerTick;
      smoothingState->yawTurnVelocityAngle16 = turnRateLimit;
      if (acceleratedVelocity < turnRateLimit) {
        smoothingState->yawTurnVelocityAngle16 = acceleratedVelocity;
      }
    }
  }
  else if (0 < (int)yawStep) {
    smoothingState->yawTurnVelocityAngle16 = 0; /* turning away: stop */
  }
  else if (yawStep + FIXED_ANGLE16_FULL_TURN <= yawDelta) {
    snapToTarget = true;
  }
  else {
    yawAngle = yawAngle + yawStep;
    turnRateLimit = aimDefinition->yawTurnRateLimitAnglePerTick;
    acceleratedVelocity = smoothingState->yawTurnVelocityAngle16 -
            g_InGameSimulationStepTicks * aimDefinition->yawTurnRateAccelerationAnglePerTick;
    smoothingState->yawTurnVelocityAngle16 = -turnRateLimit;
    if (-turnRateLimit < acceleratedVelocity) {
      smoothingState->yawTurnVelocityAngle16 = acceleratedVelocity;
    }
  }
  if (snapToTarget) {
    /* the target is reached within this step */
    currentYawAngle = modelNodeRuntime->modelPayload.localRotationAngle2;
    smoothingState->yawTurnVelocityAngle16 = 0;
    if (targetYawAngle16 != currentYawAngle) {
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      modelNodeRuntime->modelPayload.localRotationAngle2 = targetYawAngle16;
    }
  }
  else {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    modelNodeRuntime->modelPayload.localRotationAngle2 = yawAngle & FIXED_ANGLE16_MASK;
    remainingYawDelta = (yawAngle & 0xffff) - targetYawAngle16 & FIXED_ANGLE16_MASK;
    if (MODEL_AIM_TOLERANCE_ANGLE16 < remainingYawDelta &&
        remainingYawDelta < FIXED_ANGLE16_FULL_TURN - MODEL_AIM_TOLERANCE_ANGLE16) {
      return true; /* still outside the aim tolerance */
    }
  }
  return false;
}

/* Pitch counterpart of ModelNodeRuntime_SmoothYawTowardTarget (same callers): clamps the target to the weapon
   definition's pitch range, then moves localRotationAngle1 toward it with the same accelerate/limit/stop rules,
   without wrap-around. Returns the clamped target pitch when the node is on it (reached within this step or
   already there), otherwise the remaining difference (pitch - clamped target) & FIXED_ANGLE16_MASK.
   Original quirk: the callers compare this value with the unclamped target pitch to decide "aimed", so a
   remaining difference that happens to equal the target also counts (the original also returned the in/out of
   aim tolerance flag, which nobody reads).
*/

uint32_t ModelNodeRuntime_SmoothPitchTowardTarget
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState,
          AngleTurn32 targetPitchAngle16)

{
  ArmyWeaponDefinitionView *aimDefinition;
  uint32_t pitchAngle;
  int pitchStep;
  int rateLimit;
  int acceleratedVelocity;
  Bool8 snapToTarget;
  uint32_t clampedTarget;

  pitchAngle = modelNodeRuntime->modelPayload.localRotationAngle1;
  aimDefinition = smoothingState->modelDefinition;
  clampedTarget = targetPitchAngle16;
  if ((int)aimDefinition->maximumPitchAngle < (int)targetPitchAngle16) {
    clampedTarget = aimDefinition->maximumPitchAngle;
  }
  if ((int)clampedTarget < (int)aimDefinition->minimumPitchAngle) {
    clampedTarget = aimDefinition->minimumPitchAngle;
  }
  pitchStep = smoothingState->pitchTurnVelocityAngle16 * g_InGameSimulationStepTicks;
  snapToTarget = true; /* already there, or reached within this step */
  if (clampedTarget != pitchAngle) {
    if ((int)pitchAngle <= (int)clampedTarget) {
      /* target above */
      if (pitchStep < 0) {
        smoothingState->pitchTurnVelocityAngle16 = 0; /* moving away: stop */
        snapToTarget = false;
      }
      else if (pitchStep < (int)(clampedTarget - pitchAngle)) {
        pitchAngle = pitchAngle + pitchStep;
        rateLimit = aimDefinition->pitchTurnRateLimitAnglePerTick;
        acceleratedVelocity = smoothingState->pitchTurnVelocityAngle16 +
                g_InGameSimulationStepTicks * aimDefinition->pitchTurnRateAccelerationAnglePerTick;
        smoothingState->pitchTurnVelocityAngle16 = rateLimit;
        if (acceleratedVelocity < rateLimit) {
          smoothingState->pitchTurnVelocityAngle16 = acceleratedVelocity;
        }
        snapToTarget = false;
      }
    }
    else if (0 < pitchStep) {
      smoothingState->pitchTurnVelocityAngle16 = 0; /* moving away: stop */
      snapToTarget = false;
    }
    else if ((int)(clampedTarget - pitchAngle) < pitchStep) {
      pitchAngle = pitchAngle + pitchStep;
      rateLimit = aimDefinition->pitchTurnRateLimitAnglePerTick;
      acceleratedVelocity = smoothingState->pitchTurnVelocityAngle16 -
              g_InGameSimulationStepTicks * aimDefinition->pitchTurnRateAccelerationAnglePerTick;
      smoothingState->pitchTurnVelocityAngle16 = -rateLimit;
      if (-rateLimit < acceleratedVelocity) {
        smoothingState->pitchTurnVelocityAngle16 = acceleratedVelocity;
      }
      snapToTarget = false;
    }
  }
  if (!snapToTarget) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    modelNodeRuntime->modelPayload.localRotationAngle1 = pitchAngle;
    return (pitchAngle - clampedTarget) & FIXED_ANGLE16_MASK; /* remaining difference */
  }
  pitchAngle = modelNodeRuntime->modelPayload.localRotationAngle1;
  smoothingState->pitchTurnVelocityAngle16 = 0;
  if (clampedTarget != pitchAngle) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
    modelNodeRuntime->modelPayload.localRotationAngle1 = clampedTarget;
  }
  return clampedTarget;
}

/* Switches every node of a model hierarchy to the first of the (up to six) variant definitions listed in its
   definition (variantModelDefinitionIds) that the faction's technology unlocks. The armour points (health) are
   rescaled to the new definition's maximumHealth so the condition stays the same, and the army's derived metrics are rebuilt.
*/
void ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive(FactionRuntimeIndex factionIndex,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *modelRuntimeSlot;
  ModelDefinition *currentDefinition;
  ModelDefinition *previousDefinition;
  ModelDefinitionRecordPrefix *variantDefinition;
  ModelRuntimeAttachmentDescriptor *attachment;
  PckModelDefinitionIdCatalog modelDefinitionId;
  int variantIndex;
  int childrenRemaining;

  modelRuntimeSlot = modelRuntime;
  currentDefinition = modelRuntimeSlot->definitionOrSavedId.runtimeDefinition;
  for (variantIndex = 0; variantIndex < MODEL_TECHNOLOGY_VARIANT_COUNT; variantIndex++) {
    modelDefinitionId = currentDefinition->variantModelDefinitionIds[variantIndex];
    if (modelDefinitionId == 0) {
      continue;
    }
    /* ModelDefinition_IsFactionTechnologyLocked returns true when the variant is NOT unlocked */
    if (ModelDefinition_IsFactionTechnologyLocked
          (g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits,modelDefinitionId)) {
      continue;
    }
    /* swap to this variant; armour points scale with the new maximum */
    /* always found: an unknown id counts as locked */
    variantDefinition = ModelDefinitionRegistry_FindById(modelDefinitionId);
    previousDefinition = modelRuntimeSlot->definitionOrSavedId.runtimeDefinition;
    modelRuntimeSlot->definitionOrSavedId.definition = variantDefinition;
    modelRuntimeSlot->health =
         (int)(((int64_t)(int)modelRuntimeSlot->health *
               (int64_t)(int)ModelView_Cast<ModelDefinition>(variantDefinition)->maximumHealth) /
              (int64_t)(int)previousDefinition->maximumHealth);
    ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntimeSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    break;
  }
  attachment = modelRuntimeSlot->attachments;
  for (childrenRemaining = modelRuntimeSlot->attachmentCount; childrenRemaining != 0; childrenRemaining--) {
    if (attachment->childModelRuntimeOrSavedOffset != nullptr) {
      ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
                (factionIndex,attachment->childModelRuntimeOrSavedOffset);
    }
    attachment++;
  }
}

/* Returns the condition ratio (Q12, Q12_ONE = full condition) of the model hierarchy of a runtime entry (an
   army).
*/
Q12 ModelRuntime_QueryHierarchyConditionRatioQ12(RuntimeModelFactionPrefix *runtimeEntry)

{
  return ModelRuntimeHierarchy_ComputeConditionRatioQ12(runtimeEntry->modelRuntime);
}

/* Returns the active energy demand of an army's model hierarchy (ModelRuntimeHierarchy_ComputeEnergyDemand);
   the in-game selection detail shows it divided by 16 as the energy value.
*/
int ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *armyRuntime)

{
  ModelHierarchyEnergyDemand energyDemand;

  energyDemand =
       ModelRuntimeHierarchy_ComputeEnergyDemand(armyRuntime->modelRuntimeOrSavedOffset.modelRuntime);
  return (int)energyDemand.activeQ4;
}

/* Returns the energy demand of an army's model hierarchy (ModelRuntimeHierarchy_ComputeEnergyDemand): the
   active part and the total. The selection panel (src/ui/ingame/selection_panel_metrics.cpp) draws it as a stepped meter.
*/
ModelHierarchyEnergyDemand
ModelRuntime_QueryHierarchyEnergyDemand(RuntimeModelFactionPrefix *runtimeEntry)

{
  return ModelRuntimeHierarchy_ComputeEnergyDemand(runtimeEntry->modelRuntime);
}
