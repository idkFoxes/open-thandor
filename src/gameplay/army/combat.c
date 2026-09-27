/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/combat.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/combat.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/combat. */

/* Address: 0x00523980.
   Ownership: gameplay/army/combat.
   Purpose: Table membership RUNTIME_UPDATE[9]. Resolves target aim state, smooths pitch and yaw, follows the
   linked target, resolves shot launch transforms from model attachments, and fires the active attachment slots.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Role: Tracks the target, smooths
   yaw/pitch, solves launch angles and fires from matching attachments. Inputs: Army weapon state, model hierarchy,
   ShotDefinition, target and world context.
   Local calls: ArmyRuntime_EmitDamageThresholdEffect.
   Cross-module calls: ArmyRuntime_ResolveShotAimPoint [gameplay/army/runtime],
   ArmyRuntime_UpdateMovementAndWaypoints [gameplay/army/movement], ModelNodeRuntime_SmoothYawTowardTarget
   [world/model/hierarchy], ModelNodeRuntime_SmoothPitchTowardTarget [world/model/hierarchy],
   ShotDefinition_ComputeLaunchAnglesRegs [assets/shot/catalog], ModelNodeRuntime_ComputeRelativeDirectionAngle
   [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *nodeRuntimeFlags;
  uint32_t *remainingTicks;
  ModelMeshGroupMask *nodeMeshMask;
  ArmyWeaponDefinitionView68 *weaponDefinitionView;
  ModelRuntimeNode *pitchNode;
  MdlSerializedNodeHeader38 *attachmentNodeHeader;
  ArmyRuntimeSlot *commandTargetArmy;
  InGameSimulationStepBatchTicks stepTicks;
  Q12 point0Z;
  ShotRuntimeState14 targetRuntimeReference;
  Q12 point0Y;
  Q12 point0X;
  AngleTurn32 targetPitchAngle16;
  SprAttachmentSelectorOrdinal attachmentSelectorOrdinal;
  bool callCarry;
  ShotLaunchAnglesEaxEdx8 launchAngles;
  ModelRelativeDirectionAnglesEaxEdx8 relativeAngles;
  AimSmoothResult smoothResult;
  MovementStepResult movementResult;
  WorldPositionResult aimPoint;
  GameEntityRuntime *ownerEntity;
  ModelRuntimeNode *currentNode;
  
  stepTicks = g_InGameSimulationStepTicks;
  currentNode = modelRuntime->rootModelNode->childNodes[0]->childNodes[0];
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    remainingTicks = modelRuntime->attachmentReloadTicks;
    *remainingTicks = *remainingTicks - g_InGameSimulationStepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[0] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 1;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 1;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[1] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 2;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 2;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[2] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 4;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 3;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[3] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 8;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 4;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[4] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x10;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 5;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[5] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x20;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 6;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[6] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x40;
    }
    remainingTicks = modelRuntime->attachmentReloadTicks + 7;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->attachmentReloadTicks[7] = 0;
      nodeMeshMask = &(currentNode->modelPayload).meshGroupMask;
      *nodeMeshMask = *nodeMeshMask | 0x80;
    }
    remainingTicks = &modelRuntime->sharedInterShotTicks;
    *remainingTicks = *remainingTicks - stepTicks;
    if ((int)*remainingTicks < 0) {
      modelRuntime->sharedInterShotTicks = 0;
    }
    weaponDefinitionView = modelRuntime->modelDefinition;
    ownerEntity = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    currentNode = modelRuntime->rootModelNode;
    aimPoint = ArmyRuntime_ResolveShotAimPoint
                       ((currentNode->worldTransform).translation.z,
                        (currentNode->worldTransform).translation.y,
                        (currentNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition,
                        ownerEntity);
    point0X = aimPoint.worldZQ12;
    point0Y = aimPoint.worldYQ12;
    point0Z = aimPoint.worldXQ12;
    if (aimPoint.unresolved) {
      movementResult = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)ownerEntity);
      if (((!movementResult.arrived) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        currentNode = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(currentNode,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(currentNode->childNodes[0],modelRuntime,0);
      }
    }
    else {
      currentNode = modelRuntime->rootModelNode;
      pitchNode = currentNode->childNodes[0];
      launchAngles = ShotDefinition_ComputeLaunchAnglesRegs
                        (point0X,point0Y,point0Z,(pitchNode->worldTransform).translation.z,
                         (pitchNode->worldTransform).translation.y,
                         (pitchNode->worldTransform).translation.x,weaponDefinitionView->shotDefinition);
      relativeAngles = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (currentNode,launchAngles.elevationAngle,launchAngles.headingAngle);
      targetPitchAngle16 = relativeAngles.relativePitchAngle;
      smoothResult = ModelNodeRuntime_SmoothYawTowardTarget
                         (currentNode,modelRuntime,relativeAngles.relativeYawAngle);
      if (smoothResult.outsideTolerance) {
        ModelNodeRuntime_SmoothPitchTowardTarget(pitchNode,modelRuntime,targetPitchAngle16);
      }
      else {
        smoothResult = ModelNodeRuntime_SmoothPitchTowardTarget
                           (pitchNode,modelRuntime,targetPitchAngle16);
        if (((smoothResult.value == targetPitchAngle16) &&
            (weaponDefinitionView = modelRuntime->modelDefinition, modelRuntime->sharedInterShotTicks == 0)) &&
           (callCarry = ArmyRuntimeCommand_UpdateTargetFollowingState
                              (point0X,point0Y,point0Z,worldRuntime,(ArmyRuntimeSlot *)modelRuntime)
           , !callCarry)) {
          currentNode = pitchNode->childNodes[0];
          attachmentNodeHeader = weaponDefinitionView->modelPointSource64;
          nodeRuntimeFlags = &currentNode->runtimeFlags;
          *nodeRuntimeFlags = *nodeRuntimeFlags | 1;
          attachmentNodeHeader = *(MdlSerializedNodeHeader38 **)(attachmentNodeHeader->childSerializedOffsets[0] + 0x18);
          attachmentSelectorOrdinal = 0;
          do {
            if (modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] == 0) {
              modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] =
                   weaponDefinitionView->attachmentReloadTicks;
              commandTargetArmy = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
              targetRuntimeReference = 0;
              if (commandTargetArmy != (ArmyRuntimeSlot *)0x0) {
                targetRuntimeReference = (commandTargetArmy->modelRuntimeOrSavedOffset).savedIdOrOffset;
              }
              callCarry = ArmyRuntime_ResolveShotLaunchFromModelAttachment
                                (targetRuntimeReference,point0X,point0Y,point0Z,
                                 attachmentSelectorOrdinal,weaponDefinitionView->shotDefinition,currentNode,attachmentNodeHeader,
                                 worldRuntime);
              if (!callCarry) {
                modelRuntime->sharedInterShotTicks = weaponDefinitionView->sharedInterShotTicks;
                ArmyRuntime_SetNonzeroActionVector
                          (launchAngles.headingAngle,weaponDefinitionView->postLaunchVector1Q12,
                           weaponDefinitionView->postLaunchVector0Q12,modelRuntime->ownerArmyRuntime);
                nodeMeshMask = &(modelRuntime->rootModelNode->childNodes[0]->childNodes[0]->modelPayload).
                          meshGroupMask;
                *nodeMeshMask = *nodeMeshMask & -2 << ((uint8_t)attachmentSelectorOrdinal & 0x1f);
                break;
              }
              modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] = 0xffffffff;
            }
            attachmentSelectorOrdinal = attachmentSelectorOrdinal + 1;
          } while (attachmentSelectorOrdinal < 8);
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00525130.
   Ownership: gameplay/army/combat.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[15]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_EmitDamageThresholdEffect.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  ModelDefinitionRuntimeSemanticView280 *classDefinition;
  ModelRuntimeNode *rootModelNodeRuntime;
  int factionRecordByteOffset;
  ModelRuntimeNode *childNode;
  
  classDefinition = modelRuntime->modelDefinition;
  factionRecordByteOffset = modelRuntime->ownerArmyRuntime->factionIndex * 0x740;
  if (*(int *)classDefinition->reserved0C0_0DB == 1) {
    factionRecordByteOffset = factionRecordByteOffset + 0x10;
  }
  rootModelNodeRuntime = modelRuntime->rootModelNode;
  childNode = rootModelNodeRuntime->childNodes[0];
  if ((rootModelNodeRuntime->childCount != 0) && (childNode != (ModelRuntimeNode *)0x0)) {
    (childNode->modelPayload).localTranslationZQ12 =
         (int)(((int64_t)(int)(classDefinition->runtimeValue28 - classDefinition->runtimeValue24) *
               (int64_t)
               *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + factionRecordByteOffset + -0x78)) /
              (int64_t)*(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + factionRecordByteOffset + -0x74)
              ) + classDefinition->runtimeValue24;
    childNode->runtimeFlags = childNode->runtimeFlags | 1;
    ModelNodeRuntime_RebuildTransformsFromRoot(rootModelNodeRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00527AC0.
   Ownership: gameplay/army/combat.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[4]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Runs the combined timed-emitter, animated-subnode and damage-
   effect update for its runtime class. Inputs: WorldRuntimeContext and ArmyRuntimeSlot. Outputs: Updated timers,
   model pose and damage-threshold effect state.
   Local calls: ArmyRuntime_EmitDamageThresholdEffect.
   Cross-module calls: ArmyRuntime_UpdateTimedShotAndEffectEmitters [gameplay/army/runtime],
   ArmyRuntime_UpdateAnimatedModelSubnodes [gameplay/army/runtime].
*/

void __thandor_preserve_eax
ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedEffectsUpdateView200 *modelRuntime)

{
  if ((((modelRuntime->classState).classStateEC & 9) == 0) &&
     ((modelRuntime->modelDefinition->timedEffectsRequireRuntimeState40Gate1C8 == 0 ||
      (((modelRuntime->classState).classStateEC & 0x40) != 0)))) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x0052A2E0.
   Ownership: gameplay/army/combat.
   Purpose: Typed parameters: p2 impactAngle→AngleTurn32. Nearby but non-identical semantic domains were explicitly
   deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data
   remain unchanged. Typed parameters: p3 damageAmount→DamageAmount32_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: ArmyRuntime_ApplyDamageAndPropagateToParent.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *healthField;
  FactionRelationCounter *relationCounter;
  int maxHealth;
  uint32_t classId;
  int healthOrOwnerIndex;
  bool rotateToImpact;
  ModelRuntimeNode *parentModelNode;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    maxHealth = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    healthField = &armyRuntime->actionVector2Q12;
    healthOrOwnerIndex = *healthField;
    *healthField = *healthField - damageAmount;
    if (*healthField == 0 || SBORROW4(healthOrOwnerIndex,damageAmount) != *healthField < 0) {
      healthOrOwnerIndex = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      parentModelNode = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      armyRuntime->actionVector2Q12 = 0;
      if (parentModelNode == (ModelRuntimeNode *)0x0) {
        rotateToImpact = false;
        if ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0) &&
           (rotateToImpact = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime[1].classLinkState.
                    classState78 == 0, rotateToImpact)) {
          (armyRuntime->modelNodeRuntime->modelPayload).worldRotationAngle0 = impactAngle;
        }
        healthOrOwnerIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
        if ((!rotateToImpact) &&
           (classId = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C,
           relationCounter = &g_GameFactionRuntimeImage.records[healthOrOwnerIndex].relationCounterC,
           *relationCounter = *relationCounter + 1,
           g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[classId] ==
           ArmyRuntime_ClassCommandHandlerGroupA)) {
          relationCounter = &g_GameFactionRuntimeImage.records[healthOrOwnerIndex].relationCounterD;
          *relationCounter = *relationCounter + 1;
          relationCounter = &g_GameFactionRuntimeImage.records[healthOrOwnerIndex].relationCounterC;
          *relationCounter = *relationCounter + -1;
        }
      }
      else {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthOrOwnerIndex,(parentModelNode->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      healthOrOwnerIndex = maxHealth - armyRuntime->actionVector2Q12;
      if (healthOrOwnerIndex == 0 || maxHealth < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + healthOrOwnerIndex;
      }
    }
  }
  return;
}


/* Address: 0x0052A3E0.
   Ownership: gameplay/army/combat.
   Purpose: Handles army runtime apply damage and faction relation state.
   Local calls: ArmyRuntime_ApplyDamageAndPropagateToParent.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyDamageAndFactionRelationState
          (FactionRuntimeIndex sourceFactionIndex,DamageAmount32 damageAmount,
          ArmyRuntimeSlot *armyRuntime)

{
  Q12 *healthField;
  int maxHealth;
  int healthOrDelta;
  ModelRuntimeNode *parentModelNode;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    maxHealth = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    healthField = &armyRuntime->actionVector2Q12;
    healthOrDelta = *healthField;
    *healthField = *healthField - damageAmount;
    if (*healthField == 0 || SBORROW4(healthOrDelta,damageAmount) != *healthField < 0) {
      healthOrDelta = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      parentModelNode = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector2Q12 = 0;
      if (parentModelNode != (ModelRuntimeNode *)0x0) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthOrDelta,(parentModelNode->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      healthOrDelta = maxHealth - armyRuntime->actionVector2Q12;
      if (healthOrDelta == 0 || maxHealth < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + healthOrDelta;
      }
    }
  }
  return;
}


/* Address: 0x0052A640.
   Ownership: gameplay/army/combat.
   Purpose: Splits the impact value between the target ArmyRuntimeSlot-linked entity and its parent model runtime
   payload when present, using the exact single-entity impact helper. The normal return is void and the function
   returns with RET 0x10. Typed parameters: p2 impactValue→ImpactDamageValue32_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: GameEntityRuntime_ApplyImpactDamageAndFactionRelationState [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyImpactDamageToRuntimeAndParent
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,ArmyRuntimeSlot *targetArmyRuntime)

{
  ModelRuntimeNode *targetModelNodeRuntime;
  
  targetModelNodeRuntime = targetArmyRuntime->modelNodeRuntime;
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue >> 1,(GameEntityRuntime *)targetArmyRuntime)
  ;
  if (targetModelNodeRuntime->parentNode != (ModelRuntimeNode *)0x0) {
    targetArmyRuntime =
         (ArmyRuntimeSlot *)(targetModelNodeRuntime->parentNode->runtimePayload).modelRuntime;
  }
  GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
            (impactAngle,sourceFactionIndex,impactValue - (impactValue >> 1),
             (GameEntityRuntime *)targetArmyRuntime);
  return;
}


/* Address: 0x0052B9D0.
   Ownership: gameplay/army/combat.
   Purpose: Tests the target against the shot trajectory mode, angular bounds, terrain and model raycasts, faction
   and owner filters, and minimum-clearance constraints. The carry flag preserves the acceptance result. Role:
   Tests terrain, secondary surfaces and runtime models for an unobstructed shot path. Inputs: Weapon/target
   geometry, ShotDefinition trajectory mode and world collision structures. Outputs: Boolean/carry line-of-fire
   result.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_UInt64Sqrt
   [core/math/fixed], FixedMath_Atan2Angle16 [core/math/fixed], ModelRuntime_RaycastCandidateListNearest
   [world/model/runtime], FixedMath_VectorToAnglesAndLength3Regs [core/math/fixed],
   FieldGrid_RaycastTerrainSurfaceDistance [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyWeaponRuntime_TestTargetLineOfFire
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *ownModelRuntime;
  int *shotDefinitionWords;
  int64_t discriminant;
  int deltaXOrScaledLength;
  int minAngleOwnerOrDistance;
  int deltaYOrSpeedSquared;
  AngleTurn32 azimuthAngle;
  int deltaZ;
  AngleTurn32 elevationAngle;
  uint32_t angleOrDistance;
  int maxAngleOrRange;
  uint32_t distanceDifference;
  FixedLengthAngleEaxEdx8 horizontalVector;
  TerrainRaycastResult terrainHit;
  ModelRaycastResult modelHit;
  FixedLengthAnglesEaxEcxEdx12 targetVector;
  GraphicsWorldCoordinateQ12 originZQ12;
  GraphicsWorldCoordinateQ12 originYQ12;
  GraphicsWorldCoordinateQ12 originXQ12;
  WorldOwnerRuntimeClassId requiredOwnerId;
  ModelRuntimeNode *excludedNode;
  GameEntityRuntime *hitEntity;
  GameEntityRuntime *ownOrTargetEntity;
  ModelRuntimeNode *originNode;
  
  originNode = armyRuntime->modelNodeRuntime;
  ownOrTargetEntity = armyRuntime->linkedEntityRuntime;
  ownModelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  deltaXOrScaledLength = targetWorldXQ12 - (originNode->worldTransform).translation.x;
  minAngleOwnerOrDistance = *(int *)(ownModelRuntime->reserved10_37 + 0x14);
  maxAngleOrRange = *(int *)(ownModelRuntime->reserved10_37 + 0x18);
  deltaYOrSpeedSquared = targetWorldYQ12 - (originNode->worldTransform).translation.y;
  shotDefinitionWords = *(int **)(ownModelRuntime->reserved10_37 + 0x1c);
  deltaZ = targetWorldZQ12 - (originNode->worldTransform).translation.z;
  if (*shotDefinitionWords == 1) {
    horizontalVector = FixedMath_Vector2AngleAndLengthRegs(deltaYOrSpeedSquared,deltaXOrScaledLength);
    deltaYOrSpeedSquared = shotDefinitionWords[3] * shotDefinitionWords[3];
    deltaXOrScaledLength = horizontalVector.length * shotDefinitionWords[0x37];
    discriminant = (int64_t)(deltaYOrSpeedSquared + shotDefinitionWords[0x37] * deltaZ * -2) * (int64_t)deltaYOrSpeedSquared -
            (int64_t)deltaXOrScaledLength * (int64_t)deltaXOrScaledLength;
    if (discriminant < 0) {
      return true;
    }
    angleOrDistance = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)discriminant >> 0x20),(UInt64Half32)discriminant);
    if ((-0x1000 < deltaZ) && (deltaZ < 0x1000)) {
      angleOrDistance = -angleOrDistance;
    }
    angleOrDistance = FixedMath_Atan2Angle16(deltaYOrSpeedSquared + angleOrDistance,deltaXOrScaledLength);
    if ((int)angleOrDistance < minAngleOwnerOrDistance) {
      return true;
    }
    if (maxAngleOrRange < (int)angleOrDistance) {
      return true;
    }
    originNode = armyRuntime->modelNodeRuntime;
    modelHit = ModelRuntime_RaycastCandidateListNearest
                       (angleOrDistance,horizontalVector.angle & 0xffff,horizontalVector.length,
                        (originNode->worldTransform).translation.z,
                        (originNode->worldTransform).translation.y,
                        (originNode->worldTransform).translation.x,WORLD_OWNER_RUNTIME_MODEL,
                        (armyRuntime->linkedEntityRuntime->common).ownership.modelNode,worldRuntime)
    ;
    if (!modelHit.hit) {
      return false;
    }
    ownOrTargetEntity = armyRuntime->linkedEntityRuntime;
    hitEntity = ((originNode->runtimePayload).armyRuntime)->linkedEntityRuntime;
    minAngleOwnerOrDistance = (ownOrTargetEntity->common).ownership.ownerIndex;
    if ((ownOrTargetEntity->common).commandState < 1) {
      if (minAngleOwnerOrDistance == (hitEntity->common).ownership.ownerIndex) {
        return false;
      }
    }
    else if (minAngleOwnerOrDistance != (hitEntity->common).ownership.ownerIndex) {
      return false;
    }
    if (hitEntity == (ownOrTargetEntity->common).commandTarget.targetEntity) {
      return false;
    }
    return true;
  }
  if (*shotDefinitionWords == 2) {
    return false;
  }
  targetVector = FixedMath_VectorToAnglesAndLength3Regs(deltaZ,deltaYOrSpeedSquared,deltaXOrScaledLength);
  elevationAngle = targetVector.elevationAngle;
  azimuthAngle = targetVector.azimuthAngle;
  angleOrDistance = targetVector.lengthQ12;
  if (shotDefinitionWords[0xa4] == 0) {
    if ((int)elevationAngle < minAngleOwnerOrDistance) {
      return true;
    }
    if (maxAngleOrRange < (int)elevationAngle) {
      return true;
    }
  }
  excludedNode = (ownOrTargetEntity->common).ownership.modelNode;
  requiredOwnerId = WORLD_OWNER_RUNTIME_MODEL;
  originXQ12 = (originNode->worldTransform).translation.x;
  maxAngleOrRange = shotDefinitionWords[3] * shotDefinitionWords[0x34];
  originYQ12 = (originNode->worldTransform).translation.y;
  originZQ12 = (originNode->worldTransform).translation.z;
  terrainHit = FieldGrid_RaycastTerrainSurfaceDistance
                     (elevationAngle,azimuthAngle,maxAngleOrRange,(originNode->worldTransform).translation.z,
                      (originNode->worldTransform).translation.y,
                      (originNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  minAngleOwnerOrDistance = terrainHit.distanceQ12;
  modelHit = ModelRuntime_RaycastCandidateListNearest
                     (elevationAngle,azimuthAngle,maxAngleOrRange,originZQ12,originYQ12,originXQ12,
                      requiredOwnerId,excludedNode,worldRuntime);
  if ((!modelHit.hit) ? (minAngleOwnerOrDistance <= 0x7ffffffe) :
      (minAngleOwnerOrDistance < modelHit.nearestDistanceQ12)) {
    /* The terrain is hit first: only a ground shot without an entity target landing within 0x400 of the
       aim distance is clear. */
    distanceDifference = minAngleOwnerOrDistance - angleOrDistance;
    if ((int)distanceDifference < 0) {
      distanceDifference = -distanceDifference;
    }
    if (((armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity ==
         (GameEntityRuntime *)0x0) && (distanceDifference < 0x401)) {
      return false;
    }
    return true;
  }
  if (modelHit.hit) {
    /* A model is hit first: blocked (CF set) when its owner fails the commandState owner test and it is not
       the command target; otherwise fall through to the range check. */
    ownOrTargetEntity = armyRuntime->linkedEntityRuntime;
    hitEntity =
         (((modelHit.nearestNodeOrScratch.nearestModelNode)->runtimePayload).armyRuntime)->linkedEntityRuntime;
    minAngleOwnerOrDistance = (ownOrTargetEntity->common).ownership.ownerIndex;
    if (((ownOrTargetEntity->common).commandState < 1) ?
        (minAngleOwnerOrDistance != (hitEntity->common).ownership.ownerIndex) :
        (minAngleOwnerOrDistance == (hitEntity->common).ownership.ownerIndex)) {
      if (hitEntity != (ownOrTargetEntity->common).commandTarget.targetEntity) {
        return true;
      }
    }
  }
  ownOrTargetEntity = (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity;
  if (ownOrTargetEntity != (GameEntityRuntime *)0x0) {
    angleOrDistance = (int)(angleOrDistance * 2 -
                 *(int *)(*(int *)(ownOrTargetEntity->common).ownership.definitionOrClassRecord + 0xdc)
                 ) >> 1;
  }
  if (shotDefinitionWords[3] * ((shotDefinitionWords[0x9c] * -0xaaa >> 0xc) + shotDefinitionWords[0x34] + -1) < (int)angleOrDistance) {
    return true;
  }
  return false;
}


/* Address: 0x0052A200.
   Ownership: gameplay/army/combat.
   Purpose: Typed parameters: p2 damageAmount→DamageAmount32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyDamageAndPropagateToParent
          (DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *healthField;
  int healthValue;
  int maxHealth;
  ModelRuntimeNode *parentModelNode;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    maxHealth = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    healthField = &armyRuntime->actionVector2Q12;
    healthValue = *healthField;
    *healthField = *healthField - damageAmount;
    if (*healthField == 0 || SBORROW4(healthValue,damageAmount) != *healthField < 0) {
      healthValue = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      parentModelNode = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector2Q12 = 0;
      if (parentModelNode != (ModelRuntimeNode *)0x0) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-healthValue,(parentModelNode->runtimePayload).armyRuntime)
        ;
      }
    }
    else if (maxHealth < armyRuntime->actionVector2Q12) {
      armyRuntime->actionVector2Q12 =
           armyRuntime->actionVector2Q12 + (maxHealth - armyRuntime->actionVector2Q12);
    }
  }
  return;
}


/* Address: 0x00528200.
   Ownership: gameplay/army/combat.
   Purpose: Emits the configured randomized effect from successive model attachment points when the runtime falls
   below its verified damage threshold and cooldown expires. Role: Emits an effect when an army/placeable crosses a
   configured damage threshold. Inputs: Army health/damage state, definition threshold/effect and model attachment
   lookup. Outputs: EffectRuntime at the transformed damage attachment point. Edges: Transforms the matching
   attachment and calls EffectRuntimePool_CreateInstanceFromDefinition.
   Cross-module calls: ModelLookupTable_ContainsPackedKey [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   EffectRuntimePool_CreateInstanceFromDefinition [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_EmitDamageThresholdEffect
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t randomOrPointX;
  ModelPackedPointRecord *localPointRecord;
  uint32_t definitionOrRandom;
  uint32_t randomValue;
  uint32_t worldXQ12;
  uint32_t randomOffset;
  uint32_t worldZQ12;
  AngleTurn32 orientationAngle0;
  ModelLookupEntryResult lookupResult;
  ModelWorldPoint transformedPoint;
  EffectDefinition *effectDefinition;
  
  if ((armyRuntime->runtimeFlags & 0x210) != 0) {
    return;
  }
  definitionOrRandom = (armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset;
  if (*(int *)(definitionOrRandom + 0x60) < 1) {
    return;
  }
  if (*(int *)(definitionOrRandom + 0x250) <= (armyRuntime->actionVector2Q12 * 100) / *(int *)(definitionOrRandom + 0x60)) {
    return;
  }
  if (0 < armyRuntime->selectionMetric3) {
    armyRuntime->selectionMetric3 = armyRuntime->selectionMetric3 - g_InGameSimulationStepTicks;
    return;
  }
  randomOffset = 0;
  if (*(int *)(definitionOrRandom + 0x25c) != 0) {
    randomOrPointX = g_RandomGeneratorState.next();
    randomOffset = randomOrPointX % *(uint32_t *)(definitionOrRandom + 0x25c);
  }
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  armyRuntime->selectionMetric3 = randomOffset + *(int *)(definitionOrRandom + 600);
  lookupResult = ModelLookupTable_ContainsPackedKey
                    (armyRuntime->selectionMetric4,3,(modelNodeRuntime->modelPayload).modelResource)
  ;
  localPointRecord = lookupResult.entry;
  if (lookupResult.notFound) {
    /* Wrap around to the first emitter point. */
    armyRuntime->selectionMetric4 = -1;
    lookupResult = ModelLookupTable_ContainsPackedKey(0,3,(modelNodeRuntime->modelPayload).modelResource)
    ;
    localPointRecord = lookupResult.entry;
  }
  if (lookupResult.notFound) {
    /* No emitter point at all: use the model origin. */
    randomOrPointX = (modelNodeRuntime->worldTransform).translation.x;
    worldXQ12 = (modelNodeRuntime->worldTransform).translation.y;
    worldZQ12 = (modelNodeRuntime->worldTransform).translation.z;
  }
  else {
    transformedPoint = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNodeRuntime);
    worldZQ12 = transformedPoint.zQ12;
    worldXQ12 = transformedPoint.yQ12;
    randomOrPointX = transformedPoint.xQ12;
    armyRuntime->selectionMetric4 = armyRuntime->selectionMetric4 + 1;
  }
  effectDefinition = *(EffectDefinition **)(definitionOrRandom + 0x254);
  definitionOrRandom = g_RandomGeneratorState.next();
  randomOffset = definitionOrRandom & 0xffff;
  randomValue = g_RandomGeneratorState.next();
  EffectRuntimePool_CreateInstanceFromDefinition
            (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),definitionOrRandom >> 0x10,
             (randomValue & 0x1fff) + 0x1fff,randomOffset,worldZQ12,worldXQ12,randomOrPointX,effectDefinition,worldRuntime
            );
  return;
}

