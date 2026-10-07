/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/damage.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/damage.h>
#include <thandor/thandor.h>

/* Runtime update of the resource storage class (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[15]):
   moves the storage's fill-level child node between the heights definition runtimeValue24 and runtimeValue28 in
   proportion to the owner faction's current Xenite (or Tritium when definition classParameterC0 is 1) over its
   storage limit, then emits the damage-threshold effect.
*/
void ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelDefinition *classDefinition;
  ModelRuntimeNode *rootModelNodeRuntime;
  int factionRecordByteOffset;
  ModelRuntimeNode *childNode;

  classDefinition = modelRuntime->modelDefinition;
  /* byte offset of the faction record's xeniteCurrentQ4 / xeniteStorageLimitQ4 pair, or 16 bytes further that
     of tritiumCurrentQ4 / tritiumStorageLimitQ4 */
  factionRecordByteOffset = modelRuntime->ownerArmyRuntime->factionIndex * GAME_FACTION_RUNTIME_RECORD_BYTES;
  if (classDefinition->classParameterC0 == 1) {
    factionRecordByteOffset = factionRecordByteOffset + 16;
  }
  rootModelNodeRuntime = modelRuntime->rootModelNode;
  childNode = rootModelNodeRuntime->childNodes[0];
  if ((rootModelNodeRuntime->childCount != 0) && (childNode != nullptr)) {
    (childNode->modelPayload).localTranslationZQ12 =
         (int)(((int64_t)(int)(classDefinition->runtimeValue28 - classDefinition->runtimeValue24) *
               (int64_t)
               (int)Thandor_LoadU32(reinterpret_cast<const uint8_t *>(g_GameFactionRuntimeImage.records) +
                                    factionRecordByteOffset)) /
              (int64_t)(int)Thandor_LoadU32(reinterpret_cast<const uint8_t *>(g_GameFactionRuntimeImage.records) +
                                            factionRecordByteOffset + 4)
              ) + classDefinition->runtimeValue24;
    childNode->runtimeFlags = childNode->runtimeFlags | MODEL_NODE_FLAG_TRANSFORM_DIRTY;
    ModelNodeRuntime_RebuildTransformsFromRoot(rootModelNodeRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
}

/* Runtime update of army class 4 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[4]):
   while the army is intact it runs its timed shot/effect emitters and animated sub-nodes (only during research
   when the definition's gate timedEffectsRequireStateBit40 is set), then emits the damage-threshold effect.
*/

void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedEffectsUpdateView *modelRuntime)

{
  if (!Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) &&
     ((modelRuntime->modelDefinition->timedEffectsRequireStateBit40 == 0 ||
      (Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING))))) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
    ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,ModelView_Cast<ModelRuntimeUpdateView>(modelRuntime));
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,ModelView_Cast<ModelRuntimeSlot>(modelRuntime));
}

/* Applies an impact's damage to a living army (health, capped at the definition's maximumHealth). When the
   health reaches zero the army is marked destroyed; a child passes the excess damage on to its parent army; a
   root army of class 0 with a zero placementContactKindIndex only turns to the impact
   angle, any other root army is counted in its owner faction's relation counter C (group-A command classes:
   counter D).
*/
void ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  GameFactionRuntimeRecord *ownerFaction;
  int maxHealth;
  uint32_t classId;
  int previousHealth;
  int remainingHealth;
  int healthToMaximum;
  int ownerFactionIndex;
  Bool8 rotateToImpact;
  ModelRuntimeNode *parentModelNode;

  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if ((int)modelRuntime->health <= 0) {
    return;
  }
  maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
  healthField = reinterpret_cast<Q12 *>(&modelRuntime->health); /* the health dword read signed */
  previousHealth = *healthField;
  *healthField = *healthField - damageAmount;
  /* the new health is still > 0; a negative damage (repair) never raises it above maxHealth */
  if (previousHealth > damageAmount) {
    healthToMaximum = maxHealth - modelRuntime->health;
    if (healthToMaximum == 0 || maxHealth < (int)modelRuntime->health) {
      modelRuntime->health = modelRuntime->health + healthToMaximum;
    }
    return;
  }
  /* the new health is <= 0 */
  remainingHealth = modelRuntime->health; /* <= 0; its negation is the excess damage */
  (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
  parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
  /* a destroyed model links to itself */
  modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
  modelRuntime->health = 0;
  if (parentModelNode != nullptr) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(-remainingHealth,(parentModelNode->runtimePayload).modelRuntime);
    return;
  }
  /* Original quirk: the branch here tests a CPU flag that is formally undefined at that point (it is left
     over from a multiplication); measured on an AMD Zen 3 it still holds the result of the runtimeClassId /
     placementContactKindIndex tests above, so the counters are updated unless the army was turned to the
     impact. This code follows that. */
  rotateToImpact =
       (modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_00) &&
       (modelRuntime->definitionOrSavedId.runtimeDefinition->placementContactKindIndex == 0);
  if (rotateToImpact) {
    (modelRuntime->rootModelNodeOrSavedOffset.modelNode->modelPayload).worldRotationAngle0 = impactAngle;
    return;
  }
  ownerFactionIndex = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
  ownerFaction = &g_GameFactionRuntimeImage.records[ownerFactionIndex];
  classId = modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId;
  ownerFaction->relationCounterC = ownerFaction->relationCounterC + 1;
  if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[classId] ==
      ArmyRuntime_ClassCommandHandlerGroupA) {
    /* armies of the group-A command class move from counter C to counter D */
    ownerFaction->relationCounterD = ownerFaction->relationCounterD + 1;
    ownerFaction->relationCounterC = ownerFaction->relationCounterC - 1;
  }
}

/* Splits a shot impact between the hit army and the army it is mounted on: half (rounded down) goes to the
   hit army, the rest to the parent model node's army, or to the hit army again when it has no parent. Called
   directly by the shot impact handling in world/shots/flight.cpp.
*/
void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,ModelRuntimeSlot *targetModelRuntime)

{
  ModelRuntimeNode *targetModelNodeRuntime;
  ModelRuntimeSlot *secondHalfRecipient;

  /* GameEntityRuntime_ApplyImpactDamageAndFactionRelationState gets the model runtime under its
     GameEntityRuntime parameter type */
  targetModelNodeRuntime = targetModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue >> 1,ModelView_Cast<GameEntityRuntime>(targetModelRuntime));
  secondHalfRecipient = targetModelRuntime;
  if (targetModelNodeRuntime->parentNode != nullptr) {
    secondHalfRecipient = (targetModelNodeRuntime->parentNode->runtimePayload).modelRuntime;
  }
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue - (impactValue >> 1),
             ModelView_Cast<GameEntityRuntime>(secondHalfRecipient));
}

/* Subtracts damageAmount from the health of a living army. When the health drops to zero or below the army is
   flagged destroyed and the excess damage is passed on to the army it is attached to (the parent model node),
   so destroying a mounted part also damages its carrier; a negative damage (repair) is capped at the maximum
   health.
*/
void ArmyRuntime_ApplyDamageAndPropagateToParent(DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime)

{
  Q12 *healthField;
  int previousHealth;
  int remainingHealth;
  int maxHealth;
  ModelRuntimeNode *parentModelNode;

  (modelRuntime->classState).healthRegenerationDelayTicks = ARMY_DAMAGE_REGENERATION_DELAY_TICKS;
  if (0 < (int)modelRuntime->health) {
    maxHealth = modelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth;
    healthField = reinterpret_cast<Q12 *>(&modelRuntime->health); /* the health dword read signed */
    previousHealth = *healthField;
    *healthField = *healthField - damageAmount;
    /* the new health is <= 0 */
    if (previousHealth <= damageAmount) {
      remainingHealth = modelRuntime->health; /* <= 0; its negation is the excess damage */
      (modelRuntime->classState).stateFlags =
           (modelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
      /* a destroyed model links to itself */
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = modelRuntime;
      parentModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode->parentNode;
      modelRuntime->health = 0;
      if (parentModelNode != nullptr) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-remainingHealth,(parentModelNode->runtimePayload).modelRuntime);
      }
      /* Original quirk: without a parent the original goes on to the owner faction's relationCounterC/D update
         of ArmyRuntime_ApplyImpactDamageAndFinalizeState, but guarded by a branch on a CPU flag that is left over
         from a multiplication (measured on an AMD Zen 3: unchanged) and still reflects the parent == NULL test,
         so the branch is always taken and the update never runs: this code omits it. */
    }
    else if (maxHealth < (int)modelRuntime->health) {
      /* a negative damage (repair) never raises the health above maxHealth */
      modelRuntime->health =
           modelRuntime->health + (maxHealth - modelRuntime->health);
    }
  }
}

/* Damage smoke/fire of a damaged army: while its health is below the definition's threshold percentage
   (damageEffectHealthPercent) of the class maximum (maximumHealth), it emits the definition's effect
   (damageEffectDefinitionReference) every damageEffectIntervalTicks + random(damageEffectRandomTicks) ticks
   from the model's damage points (packed point key class 3), cycling through them, or from the model origin
   when it has none, with random orientation angles. Called directly at the end of nearly every army class
   runtime update (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate slots, e.g. [4], [9], [15] in
   this file).
*/
void ArmyRuntime_EmitDamageThresholdEffect(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t cooldownRandom;
  uint32_t pointXQ12;
  ModelPackedPointRecord *localPointRecord;
  ModelDefinition *definition;
  uint32_t randomBits;
  uint32_t randomValue;
  uint32_t pointYQ12;
  uint32_t cooldownRandomTicks;
  uint32_t angleRandom;
  uint32_t pointZQ12;
  Bool8 emitterPointFound;
  ModelWorldPoint transformedPoint;
  EffectDefinition *effectDefinition;

  if (Any((modelRuntime->classState).stateFlags & (ARMY_MODEL_STATE_DISMANTLING | ARMY_MODEL_STATE_DISMANTLED)))
  {
    return;
  }
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((int)definition->maximumHealth < 1) {
    return;
  }
  if (definition->damageEffectHealthPercent <=
      ((int)modelRuntime->health * 100) / (int)definition->maximumHealth) {
    return;
  }
  if (0 < modelRuntime->damageEffectCooldownTicks) {
    modelRuntime->damageEffectCooldownTicks =
         modelRuntime->damageEffectCooldownTicks - g_InGameSimulationStepTicks;
    return;
  }
  cooldownRandomTicks = 0;
  if (definition->damageEffectRandomTicks != 0) {
    cooldownRandom = g_RandomGeneratorState.next();
    cooldownRandomTicks = cooldownRandom % definition->damageEffectRandomTicks;
  }
  modelNodeRuntime = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  modelRuntime->damageEffectCooldownTicks = cooldownRandomTicks + definition->damageEffectIntervalTicks;
  emitterPointFound = ModelLookupTable_FindPackedPoint
                    (modelRuntime->damageEffectPointIndex,ARMY_MODEL_POINT_CLASS_DAMAGE_EMITTER,
                     (modelNodeRuntime->modelPayload).modelResource,&localPointRecord);
  if (!emitterPointFound) {
    /* Wrap around to the first emitter point. */
    modelRuntime->damageEffectPointIndex = -1;
    emitterPointFound = ModelLookupTable_FindPackedPoint
                      (0,ARMY_MODEL_POINT_CLASS_DAMAGE_EMITTER,(modelNodeRuntime->modelPayload).modelResource,
                       &localPointRecord);
  }
  if (!emitterPointFound) {
    /* No emitter point at all: use the model origin. */
    pointXQ12 = (modelNodeRuntime->worldTransform).translation.x;
    pointYQ12 = (modelNodeRuntime->worldTransform).translation.y;
    pointZQ12 = (modelNodeRuntime->worldTransform).translation.z;
  }
  else {
    transformedPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,modelNodeRuntime);
    pointZQ12 = transformedPoint.zQ12;
    pointYQ12 = transformedPoint.yQ12;
    pointXQ12 = transformedPoint.xQ12;
    modelRuntime->damageEffectPointIndex++;
  }
  effectDefinition = definition->damageEffectDefinitionReference.definition;
  randomBits = g_RandomGeneratorState.next();
  angleRandom = randomBits & FIXED_ANGLE16_MASK;
  randomValue = g_RandomGeneratorState.next();
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },randomBits >> 16,
             (randomValue & (FIXED_ANGLE16_EIGHTH_TURN - 1)) + (FIXED_ANGLE16_EIGHTH_TURN - 1),angleRandom,
             pointZQ12,pointYQ12,pointXQ12,effectDefinition,worldRuntime);
}

/* Applies impactValue to an entity's integrity (called twice per hit by ArmyRuntime_ApplyImpactDamageToRuntimeAndParent;
   a negative value repairs and goes to the entity its runtime link points at). A destroyed entity passes the
   overkill on to its parent model's army, or, without a parent, is turned to the impact angle (definition class 0
   with placementContactKindIndex 0) and counted in the score counters: a loss for its faction, a kill for
   sourceFactionIndex (the heavier counters D/F instead of C/E for classes handled by
   ArmyRuntime_ClassCommandHandlerGroupA). Repair beyond the definition maximum (maximumHealth) is clamped and the
   excess handed to the first linked army that is not at full integrity.
*/
void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime)

{
  int *integrityField;
  GameEntityRuntimeFlags *runtimeFlagsField;
  FactionRelationCounter *relationCounter;
  ModelRuntimeSlot *attachedModelRuntime;
  GameEntityRuntime *attachmentCursor;
  void *definitionRecord;
  int maximumIntegrity;
  int previousIntegrity;
  int overkillIntegrity;
  int repairExcess;
  int victimFactionIndex;
  int victimClassId;
  int attachmentsRemaining;
  ModelRuntimeNode *parentNode;

  if (impactValue < 0) {
    targetEntityRuntime = THANDOR_PTR32_AT(GameEntityRuntime, (targetEntityRuntime->common).ownership.runtimeLink);
  }
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state08 = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.reactionCode09 = 2;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0A = 0;
  (targetEntityRuntime->common).pathingAndImpactState.impactReaction.state0B = 0;
  if (0 < (targetEntityRuntime->common).damageState.remainingIntegrity) {
    maximumIntegrity =
         (targetEntityRuntime->common).ownership.modelDefinition()->
         maximumHealth;
    integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
    previousIntegrity = *integrityField;
    *integrityField = *integrityField - impactValue;
    /* the impact used up the remaining integrity: the entity is destroyed */
    if (previousIntegrity <= impactValue) {
      overkillIntegrity = (targetEntityRuntime->common).damageState.remainingIntegrity;
      runtimeFlagsField = &(targetEntityRuntime->common).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField | ARMY_RUNTIME_FLAG_DESTROYED;
      parentNode = ((targetEntityRuntime->common).ownership.modelNode)->parentNode;
      (targetEntityRuntime->common).damageState.counterOrTerminalReference.terminalEntity =
           targetEntityRuntime;
      (targetEntityRuntime->common).damageState.remainingIntegrity = 0;
      if (parentNode == nullptr) {
        definitionRecord = (targetEntityRuntime->common).ownership.definitionOrClassRecord;
        if ((static_cast<ModelDefinition *>(definitionRecord)->runtimeClassId == 0) &&
           (static_cast<ModelDefinition *>(definitionRecord)->placementContactKindIndex == 0)) {
          (((targetEntityRuntime->common).ownership.modelNode)->modelPayload).worldRotationAngle0 =
               impactAngle;
        }
        if (impactValue != 0) {
          /* Original quirk: the branch here tests a CPU flag that is left over from a multiplication (measured
             on an AMD Zen 3: unchanged), so it still holds the impactValue == 0 test, whose own branch already left
             for zero; the branch is never taken and the counters are always updated. This code follows that. */
          victimFactionIndex =
               (targetEntityRuntime->common).ownership.linkedArmyRuntime()->factionIndex;
          victimClassId =
               (targetEntityRuntime->common).ownership.modelDefinition()->
               runtimeClassId;
          relationCounter = &g_GameFactionRuntimeImage.records[victimFactionIndex].relationCounterC;
          *relationCounter = *relationCounter + 1;
          relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
          *relationCounter = *relationCounter + 1;
          if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[victimClassId] ==
              ArmyRuntime_ClassCommandHandlerGroupA) {
            relationCounter = &g_GameFactionRuntimeImage.records[victimFactionIndex].relationCounterD;
            *relationCounter = *relationCounter + 1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterF;
            *relationCounter = *relationCounter + 1;
            relationCounter = &g_GameFactionRuntimeImage.records[victimFactionIndex].relationCounterC;
            *relationCounter = *relationCounter - 1;
            relationCounter = &g_GameFactionRuntimeImage.records[sourceFactionIndex].relationCounterE;
            *relationCounter = *relationCounter - 1;
          }
        }
      }
      else {
        /* the overkill goes on to the parent's army */
        ArmyRuntime_ApplyDamageAndPropagateToParent(-overkillIntegrity,
                                                    (parentNode->runtimePayload).modelRuntime);
      }
    }
    else {
      integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
      repairExcess = maximumIntegrity - *integrityField;
      if (repairExcess == 0 || maximumIntegrity < *integrityField) {
        /* repaired to or beyond the maximum: clamp, the (negative) excess repairs a linked army */
        integrityField = &(targetEntityRuntime->common).damageState.remainingIntegrity;
        *integrityField = *integrityField + repairExcess;
        /* common.ownership.ownerIndex is the model runtime's attachmentCount here, the attached child model
           runtimes are its attachments[] with a stride of 0x20 (attachmentCursor advances by those 0x20 bytes) */
        attachedModelRuntime = (targetEntityRuntime->classPayload).impactOwnerLinks.attachment0ChildModelRuntime;
        attachmentCursor = targetEntityRuntime;
        for (attachmentsRemaining = (targetEntityRuntime->common).ownership.ownerIndex; attachmentsRemaining != 0;
             attachmentsRemaining--) {
          /* the child's health against its definition's maximumHealth: not at full health */
          if ((attachedModelRuntime != nullptr) &&
             (attachedModelRuntime->health !=
              attachedModelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth)) {
            ArmyRuntime_ApplyDamageAndPropagateToParent(repairExcess,attachedModelRuntime);
            return;
          }
          attachedModelRuntime = (attachmentCursor->classPayload).impactOwnerLinks.attachment1ChildModelRuntime;
          /* advance by one 0x20-byte attachment descriptor: the entity view starting at commandTarget.targetWorldXQ12 */
          attachmentCursor =
               reinterpret_cast<GameEntityRuntime *>(&(attachmentCursor->common).commandTarget.targetWorldXQ12);
        }
      }
    }
  }
}
