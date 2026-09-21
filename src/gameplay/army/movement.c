/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/movement.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/movement.h>

/* Implementation ownership: gameplay/army/movement. */

/* Address: 0x00520F60.
   Ownership: gameplay/army/movement.
   Purpose: Runtime-update slot 3 prefix. Owns exactly 00520F60-005210DD and transfers into the independently
   preserved shared-tail owner at 005210DE.
   Local calls: ArmyArticulatedRuntime_UpdateContactChildAndEffects, ArmyRuntime_UpdateMovementAndWaypoints,
   ArmyArticulatedRuntime_UpdateLeftTerrainContact, ArmyArticulatedRuntime_UpdateRightTerrainContact,
   ArmyArticulatedRuntime_UpdateSelectedTerrainContact, ArmyArticulatedRuntime_InitializeLeftTerrainContact,
   ArmyArticulatedRuntime_InitializeRightTerrainContact, ArmyArticulatedRuntime_UpdateSuspensionHierarchy.
   Cross-module calls: ArmyRuntime_EmitDamageThresholdEffect [gameplay/army/combat],
   ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement], FieldGrid_InterpolateWaterDelta
   [world/terrain/grid], ArmyRuntime_ApplyDamageAndPropagateToParent [gameplay/army/combat],
   GameEntityRuntime_ResolveCommandTargetPositionCf [gameplay/faction/runtime], FixedMath_Atan2Angle16
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateArticulatedMovement
          (WorldRuntimeContext *worldRuntime,
          ModelRuntimeArticulatedMovementDefinitionView200 *modelRuntime)

{
  ArmyMovementStateFlags *pAVar1;
  Q12 *pQVar2;
  AngleTurn32 AVar3;
  int iVar4;
  int iVar5;
  ModelDefinitionArticulatedMovementView280 *pMVar6;
  sdword sVar7;
  int iVar8;
  dword dVar9;
  uint uVar10;
  ModelRuntimeNode *steeringAngle16;
  int component0;
  ArmyRuntimeCoordinateCommandOrHistoryValue4 x;
  bool bVar11;
  WorldPositionXYEaxEdxCf9 WVar12;
  WorldPositionEaxEcxEdxCf13 WVar13;
  FixedLengthAngleEaxEdx8 FStack_24;
  ArmyRuntimeSlot *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  entityRuntime1 = modelRuntime->linkedArmyRuntime;
  modelNode1 = modelRuntime->rootModelNode;
  pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *pAVar1 = *pAVar1 | 4;
  if ((entityRuntime1 != (ArmyRuntimeSlot *)0x0) &&
     ((((((entityRuntime1->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC == 0
       || (modelRuntime->modelDefinition->placementRadiusOrClearanceDC == 0)) ||
      (bVar11 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                          (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                           (modelNode1->worldTransform).translation.y,
                           (modelNode1->worldTransform).translation.x,entityRuntime1), !bVar11)))) {
    modelRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  AVar3 = (modelNode1->modelPayload).worldRotationAngle2;
  iVar4 = (modelNode1->worldTransform).translation.x;
  iVar5 = (modelNode1->worldTransform).translation.y;
  pMVar6 = modelRuntime->modelDefinition;
  sVar7 = FieldGrid_InterpolateWaterDelta
                    ((modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((pMVar6->waterDamageThreshold198 < sVar7) &&
     (iVar8 = sVar7 * pMVar6->waterDamageMultiplier194 >> 7, -1 < iVar8)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(iVar8,(ArmyRuntimeSlot *)modelRuntime);
  }
  iVar8 = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  x = (ArmyRuntimeCoordinateCommandOrHistoryValue4)pMVar6->movementAdvanceDeltaQ12PerTick18;
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 3U) != 0) {
    if (((int)modelRuntime->runtimeStateA8 < 0x801) &&
       ((int)(modelRuntime->articulatedContact).terrainContactMode < 0x801)) {
      x.signedValue = x.signedValue + iVar8;
    }
    else {
      x.signedValue = -(x.signedValue - iVar8);
    }
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = (Q12)x;
    steeringAngle16 =
         (ModelRuntimeNode *)
         ((int)(iVar8 * (modelRuntime->articulatedContact).fallbackPosition1Q12 *
               g_InGameSimulationStepTicks) >> 0xc);
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 2U) == 0) {
      modelRuntime->runtimeStateA8 =
           (dword)((steeringAngle16->modelPayload).reserved2C_33 +
                  (modelRuntime->runtimeStateA8 - 0x38));
      if (0xfff < modelRuntime->runtimeStateA8) {
        pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar2 = *pQVar2 & 0xfffffffe;
        modelRuntime->runtimeStateA8 = 0;
        modelRuntime->movementTarget0Q12 = modelRuntime->runtimeState90;
        modelRuntime->definitionClassValue80 = modelRuntime->runtimeState98;
        modelRuntime->definitionClassValue88 = modelRuntime->articulatedHeightOrStateA0;
        x = (modelRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue;
        dVar9 = (modelRuntime->linkedChildSpawnParameters).parameter0;
        modelRuntime->classState60 = modelRuntime->ownerValue64;
        modelRuntime->ownerValue68 = modelRuntime->fallbackWorldYQ12;
        (modelRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = x;
        (modelRuntime->movementControl).movementAdvancePerTickQ12 = dVar9;
        steeringAngle16 = modelRuntime->rootModelNode;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (steeringAngle16->childNodes[0],worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
      }
    }
    else {
      (modelRuntime->articulatedContact).terrainContactMode =
           (ArmyTerrainContactDispatchMode)
           ((steeringAngle16->modelPayload).reserved2C_33 +
           ((modelRuntime->articulatedContact).terrainContactMode - 0x38));
      if (0xfff < (modelRuntime->articulatedContact).terrainContactMode) {
        pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar2 = *pQVar2 & 0xfffffffd;
        (modelRuntime->articulatedContact).terrainContactMode =
             ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT;
        modelRuntime->movementTarget1Q12 = modelRuntime->runtimeState94;
        modelRuntime->definitionClassValue84 = modelRuntime->articulatedCoordinateOrState9C;
        modelRuntime->runtimeState8C = modelRuntime->runtimeStateA4;
        x = (modelRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue;
        dVar9 = (modelRuntime->linkedChildSpawnParameters).parameter0;
        modelRuntime->classState60 = modelRuntime->ownerValue64;
        modelRuntime->linkedArmyRuntimeOrSavedOffset =
             (ArmyRuntimeSlot *)modelRuntime->fallbackWorldXQ12;
        (modelRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = x;
        (modelRuntime->movementControl).movementAdvancePerTickQ12 = dVar9;
        steeringAngle16 = modelRuntime->rootModelNode;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (steeringAngle16->childNodes[1],worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
      }
    }
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 3U) != 0)
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    modelNode1 = modelRuntime->rootModelNode;
    WVar12 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (WVar12.carry) {
      WVar13 = GameEntityRuntime_ResolveCommandTargetPositionCf
                         ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime);
      x.signedValue = WVar13.worldZQ12;
      if (!WVar13.carry) {
        dVar9 = FixedMath_Atan2Angle16
                          (WVar13.worldYQ12 - (modelNode1->worldTransform).translation.y,
                           WVar13.worldXQ12 - (modelNode1->worldTransform).translation.x);
        steeringAngle16 =
             (ModelRuntimeNode *)(dVar9 - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff);
        if (((ModelRuntimeNode *)0x800 < steeringAngle16) &&
           (steeringAngle16 < (ModelRuntimeNode *)0xf800))
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
    else {
      x.signedValue = WVar12.worldYQ12 - (modelNode1->worldTransform).translation.y;
      steeringAngle16 =
           (ModelRuntimeNode *)
           FixedMath_Length2(x.signedValue,
                             WVar12.worldXQ12 - (modelNode1->worldTransform).translation.x);
      if ((ModelRuntimeNode *)0x40 < steeringAngle16)
      goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    }
ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint:
    entityRuntime1 = modelRuntime->ownerArmyRuntime;
    pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar2 = *pQVar2 & 0xffffffcf;
    WVar12 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
    x.signedValue = WVar12.worldYQ12;
    steeringAngle16 = (ModelRuntimeNode *)WVar12.worldXQ12;
    if (!WVar12.carry) {
      pAVar1 = &entityRuntime1->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
  }
  if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x10U) == 0) {
    if ((modelRuntime->runtimeFlags & 8) == 0) {
      WVar12 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
      if (WVar12.carry) {
        WVar13 = GameEntityRuntime_ResolveCommandTargetPositionCf
                           ((GameEntityRuntime *)modelRuntime->ownerArmyRuntime);
        x.signedValue = WVar13.worldZQ12;
        if (WVar13.carry) goto ArmyArticulatedMovement_SharedContinuation;
        FStack_24.angle =
             FixedMath_Atan2Angle16
                       (WVar13.worldYQ12 - (modelNode1->worldTransform).translation.y,
                        WVar13.worldXQ12 - (modelNode1->worldTransform).translation.x);
        FStack_24.length = 0xffffffff;
      }
      else {
        iVar8 = WVar12.worldXQ12 - (modelNode1->worldTransform).translation.x;
        component0 = WVar12.worldYQ12 - (modelNode1->worldTransform).translation.y;
        if ((iVar8 == 0) && (component0 == 0)) {
          FStack_24 = (FixedLengthAngleEaxEdx8)
                      ((ulonglong)(modelNode1->modelPayload).worldRotationAngle2 << 0x20);
        }
        else {
          FStack_24 = FixedMath_Vector2AngleAndLengthRegs(component0,iVar8);
        }
        x.signedValue = FStack_24.angle;
      }
      steeringAngle16 =
           (ModelRuntimeNode *)
           (FStack_24.angle - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff);
      if (0 < (int)FStack_24.length) {
        if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x20U) == 0) {
          if ((steeringAngle16 < (ModelRuntimeNode *)0x2001) ||
             ((ModelRuntimeNode *)0xdfff < steeringAngle16)) {
ArmyArticulatedMovement_UpdateSelectedTerrainContact:
            if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 4U) == 0) {
              pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar2 = *pQVar2 & 0xffa0;
              pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar2 = *pQVar2 | 0x25;
              ArmyArticulatedRuntime_UpdateLeftTerrainContact
                        (FStack_24.angle,FStack_24.length,
                         (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
            }
            else {
              pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar2 = *pQVar2 & 0xffa0;
              pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar2 = *pQVar2 | 0x2a;
              ArmyArticulatedRuntime_UpdateRightTerrainContact
                        (FStack_24.angle,FStack_24.length,
                         (ArmyArticulatedRuntimeSlotView *)modelRuntime,worldRuntime);
            }
            goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
          }
        }
        else if ((steeringAngle16 < (ModelRuntimeNode *)0x201) ||
                ((ModelRuntimeNode *)0xfdff < steeringAngle16))
        goto ArmyArticulatedMovement_UpdateSelectedTerrainContact;
      }
      if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x20U) != 0)
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
      if (FStack_24.length < 0x41)
      goto ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint;
      if ((-1 < (int)FStack_24.length) ||
         (((ModelRuntimeNode *)0x800 < steeringAngle16 &&
          (steeringAngle16 < (ModelRuntimeNode *)0xf800)))) {
        ArmyArticulatedRuntime_UpdateSelectedTerrainContact
                  ((AngleTurn32)steeringAngle16,(ArmyArticulatedRuntimeSlotView *)modelRuntime,
                   worldRuntime);
        pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar2 = *pQVar2 & 0xffffffa0;
        if (steeringAngle16 < (ModelRuntimeNode *)0x8000) {
          pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
          *pQVar2 = *pQVar2 | 0x15;
        }
        else {
          pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
          *pQVar2 = *pQVar2 | 0x1a;
        }
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
ArmyArticulatedMovement_SharedContinuation:
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 0x40U) == 0) {
      x.signedValue = modelRuntime->definitionClassValue80 - modelRuntime->definitionClassValue84;
      dVar9 = FixedMath_Atan2Angle16
                        (x.signedValue,
                         modelRuntime->movementTarget0Q12 - modelRuntime->movementTarget1Q12);
      uVar10 = (dVar9 - (modelNode1->modelPayload).worldRotationAngle2) - 0x4000 & 0xffff;
      if ((0xfff < uVar10) && ((uVar10 < 0x7000 || ((0x8fff < uVar10 && (uVar10 < 0xf000))))))
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
    }
    steeringAngle16 =
         (ModelRuntimeNode *)(modelRuntime->rootModelNode->worldTransform).translation.x;
    x = (ArmyRuntimeCoordinateCommandOrHistoryValue4)
        (modelRuntime->rootModelNode->worldTransform).translation.y;
    if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) == 0) {
      return;
    }
  }
  else {
ArmyArticulatedMovement_InitializeSelectedTerrainContact:
    steeringAngle16 =
         (ModelRuntimeNode *)
         (((modelRuntime->articulatedContact).fallbackPosition0Q12 >> 0x10) +
          modelRuntime->ownerValue64 & 0xffff);
    if (((modelRuntime->articulatedContact).fallbackPosition0Q12 & 4U) == 0) {
      pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 & 0xffffffc0;
      pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 | 0x45;
      ArmyArticulatedRuntime_InitializeLeftTerrainContact
                ((AngleTurn16Stored32)steeringAngle16,(ArmyArticulatedRuntimeSlotView *)modelRuntime
                 ,worldRuntime);
    }
    else {
      pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 & 0xffffffc0;
      pQVar2 = &(modelRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 | 0x4a;
      ArmyArticulatedRuntime_InitializeRightTerrainContact
                ((AngleTurn16Stored32)steeringAngle16,(ArmyArticulatedRuntimeSlotView *)modelRuntime
                 ,worldRuntime);
    }
  }
ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms:
  modelNode1 = modelRuntime->rootModelNode;
  (modelNode1->worldTransform).translation.x = (GraphicsWorldCoordinateQ12)steeringAngle16;
  (modelNode1->worldTransform).translation.y = (GraphicsWorldCoordinateQ12)x;
  modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(modelNode1,worldRuntime);
  if (((iVar4 != (modelNode1->worldTransform).translation.x) ||
      (iVar5 != (modelNode1->worldTransform).translation.y)) ||
     (AVar3 != (modelNode1->modelPayload).worldRotationAngle2)) {
    entityRuntime1 = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    pAVar1 = &entityRuntime1->movementStateFlags;
    *pAVar1 = *pAVar1 & 0xfffffffb;
  }
  pMVar6 = modelRuntime->modelDefinition;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  ModelNodeRuntime_UpdateDepthBinMasks(pMVar6->placementRadiusOrClearanceDC,modelNode1);
  return;
}


/* Address: 0x0051C5A0.
   Ownership: gameplay/army/movement.
   Purpose: Resolves the current command target, stores the target state, and starts a route toward the resolved
   target model position when a valid target exists. Two stack arguments are authoritative from RET 0x08; prior
   synthetic fastcall parameters are removed.
   Local calls: ArmyRuntime_ResetMovementStatePreserveQueuedTarget,
   ArmyRuntime_ResetMovementStateFromCurrentPosition, ArmyRuntime_StartMoveCommandWithFallbackWaypoints.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResolveCommandTargetAndRoute
          (GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration AVar1;
  ModelRuntimeNode *modelNode1;
  
  AVar1 = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetRuntime == (GameEntityRuntime *)0x0) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandGeneration = AVar1;
    armyRuntime->commandModeFlags = 1;
    modelNode1 = (targetRuntime->common).ownership.modelNode;
    ArmyRuntime_StartMoveCommandWithFallbackWaypoints
              ((modelNode1->worldTransform).translation.y,(modelNode1->worldTransform).translation.x
               ,(ArmyMovementRuntime *)armyRuntime);
  }
  armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)targetRuntime;
  return;
}


/* Address: 0x00520DF0.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[18]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntimeClass_UpdateGroundMovementVariantB, ArmyRuntimeClass_UpdateGroundMovementVariantA.
   Cross-module calls: AiUnitBehavior_UpdateSpecialClass12Entity [gameplay/ai/units].
*/

void __thandor_preserve_eax_edx
ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyPlacementContactKindIndex32 AVar1;
  
  armyRuntime = modelRuntime->ownerArmyRuntime;
  AVar1 = *(ArmyPlacementContactKindIndex32 *)
           (modelRuntime->modelDefinition->reserved1C0_253 + 0xb8);
  if ((armyRuntime->movementStateFlags & 0x100) != 0) {
    AiUnitBehavior_UpdateSpecialClass12Entity
              ((MdlDefinitionSemanticPrefix80 *)modelRuntime->modelDefinition,armyRuntime,
               armyRuntime->factionIndex,worldRuntime);
  }
  if (AVar1 == 1) {
    ArmyRuntimeClass_UpdateGroundMovementVariantB(worldRuntime,modelRuntime);
  }
  else {
    ArmyRuntimeClass_UpdateGroundMovementVariantA(worldRuntime,modelRuntime);
  }
  return;
}


/* Address: 0x00523410.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[7]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Class-specific update combining movement, aim and
   projectile/effect processing variant A. Inputs: World context and army runtime state. Outputs: Updated
   movement/aim/model state and possible emitted projectiles/effects.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints, ArmyRuntimeCommand_UpdateTargetFollowingState.
   Cross-module calls: FixedVector_StepBackwardAlongOwnDirection [core/math/fixed],
   ArmyRuntime_ResolveShotAimPointCf [gameplay/army/runtime], ModelNodeRuntime_SmoothYawTowardTarget
   [world/model/hierarchy], ModelNodeRuntime_SmoothPitchTowardTarget [world/model/hierarchy],
   ShotDefinition_ComputeLaunchAnglesRegs [assets/shot/catalog], ModelNodeRuntime_ComputeRelativeDirectionAngle
   [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *pMVar1;
  WeaponAimCountdownTicks *pWVar2;
  AngleTurn32 *pAVar3;
  ArmyWeaponDefinitionView68 *pAVar4;
  int iVar5;
  FixedMathScale32 FVar6;
  ModelRuntimeNode *modelNodeRuntime;
  dword stepMultiplier;
  ArmyRuntimeSlot *pAVar7;
  InGameSimulationStepBatchTicks IVar8;
  Q12 point0Z;
  Q12 point0Y;
  Q12 point0X;
  AngleTurn32 targetPitchAngle16;
  ShotRuntimeState14 shotRuntimeState14;
  bool bVar9;
  ShotLaunchAnglesEaxEdx8 SVar10;
  ModelRelativeDirectionAnglesEaxEdx8 MVar11;
  ModelSmoothEaxCf5 MVar12;
  WorldPositionXYEaxEdxCf9 WVar13;
  WorldPositionEaxEcxEdxCf13 WVar14;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode2;
  
  IVar8 = g_InGameSimulationStepTicks;
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    pAVar4 = modelRuntime->modelDefinition;
    entityRuntime1 = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    iVar5 = modelRuntime->attachment0BackwardStepCountdownTicks28;
    if (modelRuntime->attachmentReloadCountdownTicks24 != 0) {
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      pWVar2 = &modelRuntime->attachmentReloadCountdownTicks24;
      *pWVar2 = *pWVar2 - g_InGameSimulationStepTicks;
      if (*pWVar2 < 0) {
        IVar8 = IVar8 + modelRuntime->attachmentReloadCountdownTicks24;
        modelRuntime->attachmentReloadCountdownTicks24 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      pAVar3 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar3 = *pAVar3 + IVar8 * pAVar4->localRotationAngle2StepPerTick34;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
      pAVar3 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar3 = *pAVar3 & 0xffff;
    }
    IVar8 = g_InGameSimulationStepTicks;
    if (iVar5 != 0) {
      FVar6 = pAVar4->backwardStepScale3C;
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      pWVar2 = &modelRuntime->attachment0BackwardStepCountdownTicks28;
      *pWVar2 = *pWVar2 - g_InGameSimulationStepTicks;
      if (*pWVar2 < 0) {
        IVar8 = IVar8 + modelRuntime->attachment0BackwardStepCountdownTicks28;
        modelRuntime->attachment0BackwardStepCountdownTicks28 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-IVar8,FVar6,(FixedVectorStateAddress32)modelNode2);
      pMVar1 = &modelNode2->runtimeFlags;
      *pMVar1 = *pMVar1 | 1;
    }
    modelNode2 = modelRuntime->rootModelNode;
    WVar14 = ArmyRuntime_ResolveShotAimPointCf
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,pAVar4->shotDefinition,
                        entityRuntime1);
    point0X = WVar14.worldZQ12;
    point0Y = WVar14.worldYQ12;
    point0Z = WVar14.worldXQ12;
    if (WVar14.carry) {
      WVar13 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      if (((!WVar13.carry) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        modelNode2 = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],modelRuntime,0);
      }
    }
    else {
      modelNode2 = modelRuntime->rootModelNode;
      modelNodeRuntime = modelNode2->childNodes[0];
      SVar10 = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNodeRuntime->worldTransform).translation.z,
                          (modelNodeRuntime->worldTransform).translation.y,
                          (modelNodeRuntime->worldTransform).translation.x,pAVar4->shotDefinition);
      MVar11 = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (modelNode2,SVar10.elevationAngle,SVar10.headingAngle);
      targetPitchAngle16 = MVar11.relativePitchAngle;
      MVar12 = ModelNodeRuntime_SmoothYawTowardTarget
                         (modelNode2,modelRuntime,MVar11.relativeYawAngle);
      if (MVar12.carry) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,modelRuntime,targetPitchAngle16);
      }
      else {
        MVar12 = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,modelRuntime,targetPitchAngle16);
        if ((MVar12.eax == targetPitchAngle16) &&
           (pAVar4 = modelRuntime->modelDefinition,
           modelRuntime->attachmentReloadCountdownTicks24 == 0)) {
          bVar9 = ArmyRuntimeCommand_UpdateTargetFollowingState
                            (point0X,point0Y,point0Z,worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
          if (!bVar9) {
            stepMultiplier = pAVar4->sharedInterShotTicks;
            FVar6 = pAVar4->backwardStepScale3C;
            modelRuntime->attachmentReloadCountdownTicks24 =
                 modelRuntime->attachmentReloadCountdownTicks24 + pAVar4->attachmentReloadTicks;
            modelRuntime->attachment0BackwardStepCountdownTicks28 =
                 modelRuntime->attachment0BackwardStepCountdownTicks28 + stepMultiplier;
            modelNode2 = modelNodeRuntime->childNodes[0];
            FixedVector_StepBackwardAlongOwnDirection
                      (stepMultiplier,FVar6,(FixedVectorStateAddress32)modelNode2);
            modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
            ArmyRuntime_SetNonzeroActionVector
                      (SVar10.headingAngle,pAVar4->postLaunchVector1Q12,pAVar4->postLaunchVector0Q12
                       ,modelRuntime->ownerArmyRuntime);
            pAVar7 = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
            shotRuntimeState14 = 0;
            if (pAVar7 != (ArmyRuntimeSlot *)0x0) {
              shotRuntimeState14 = (pAVar7->modelRuntimeOrSavedOffset).savedIdOrOffset;
            }
            ModelRuntime_EmitProjectilesFromAttachmentPoints
                      (shotRuntimeState14,point0X,point0Y,point0Z,pAVar4->shotDefinition,modelNode2,
                       *(MdlSerializedNodeHeader38 **)
                        (pAVar4->modelPointSource64->childSerializedOffsets[0] + 0x18),worldRuntime)
            ;
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00523690.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[8]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime). Role: Class-specific update combining movement, aim and
   projectile/effect processing variant B. Inputs: World context and army runtime state. Outputs: Updated
   movement/aim/model state and possible emitted projectiles/effects.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints, ArmyRuntimeCommand_UpdateTargetFollowingState.
   Cross-module calls: FixedVector_StepBackwardAlongOwnDirection [core/math/fixed],
   ArmyRuntime_ResolveShotAimPointCf [gameplay/army/runtime], ModelNodeRuntime_SmoothYawTowardTarget
   [world/model/hierarchy], ModelNodeRuntime_SmoothPitchTowardTarget [world/model/hierarchy],
   ShotDefinition_ComputeLaunchAnglesRegs [assets/shot/catalog], ModelNodeRuntime_ComputeRelativeDirectionAngle
   [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *pMVar1;
  WeaponAimCountdownTicks *pWVar2;
  AngleTurn32 *pAVar3;
  ArmyWeaponDefinitionView68 *pAVar4;
  FixedMathScale32 FVar5;
  ModelRuntimeNode *modelNodeRuntime;
  dword stepMultiplier;
  ArmyRuntimeSlot *pAVar6;
  InGameSimulationStepBatchTicks IVar7;
  Q12 point0Z;
  int iVar8;
  Q12 point0Y;
  Q12 point0X;
  AngleTurn32 targetPitchAngle16;
  ShotRuntimeState14 shotRuntimeState14;
  bool bVar9;
  ShotLaunchAnglesEaxEdx8 SVar10;
  ModelRelativeDirectionAnglesEaxEdx8 MVar11;
  ModelSmoothEaxCf5 MVar12;
  WorldPositionXYEaxEdxCf9 WVar13;
  WorldPositionEaxEcxEdxCf13 WVar14;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode2;
  
  IVar7 = g_InGameSimulationStepTicks;
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    pAVar4 = modelRuntime->modelDefinition;
    entityRuntime1 = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    iVar8 = modelRuntime->attachment0BackwardStepCountdownTicks28;
    if (modelRuntime->attachmentReloadCountdownTicks24 != 0) {
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      pWVar2 = &modelRuntime->attachmentReloadCountdownTicks24;
      *pWVar2 = *pWVar2 - g_InGameSimulationStepTicks;
      if (*pWVar2 < 0) {
        IVar7 = IVar7 + modelRuntime->attachmentReloadCountdownTicks24;
        modelRuntime->attachmentReloadCountdownTicks24 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      pAVar3 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar3 = *pAVar3 + IVar7 * pAVar4->localRotationAngle2StepPerTick34;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
      pAVar3 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar3 = *pAVar3 & 0xffff;
    }
    IVar7 = g_InGameSimulationStepTicks;
    if (iVar8 != 0) {
      FVar5 = pAVar4->backwardStepScale3C;
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      pWVar2 = &modelRuntime->attachment0BackwardStepCountdownTicks28;
      *pWVar2 = *pWVar2 - g_InGameSimulationStepTicks;
      if (*pWVar2 < 0) {
        IVar7 = IVar7 + modelRuntime->attachment0BackwardStepCountdownTicks28;
        modelRuntime->attachment0BackwardStepCountdownTicks28 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection(-IVar7,FVar5,(FixedVectorStateAddress32)modelNode2);
      pMVar1 = &modelNode2->runtimeFlags;
      *pMVar1 = *pMVar1 | 1;
    }
    IVar7 = g_InGameSimulationStepTicks;
    if (modelRuntime->attachment1BackwardStepCountdownTicks2C != 0) {
      FVar5 = pAVar4->backwardStepScale3C;
      modelNode2 = modelRuntime->rootModelNode->childNodes[0];
      pWVar2 = &modelRuntime->attachment1BackwardStepCountdownTicks2C;
      *pWVar2 = *pWVar2 - g_InGameSimulationStepTicks;
      if (*pWVar2 < 0) {
        IVar7 = IVar7 + modelRuntime->attachment1BackwardStepCountdownTicks2C;
        modelRuntime->attachment1BackwardStepCountdownTicks2C = 0;
      }
      modelNode2 = modelNode2->childNodes[1];
      FixedVector_StepBackwardAlongOwnDirection(-IVar7,FVar5,(FixedVectorStateAddress32)modelNode2);
      pMVar1 = &modelNode2->runtimeFlags;
      *pMVar1 = *pMVar1 | 1;
    }
    modelNode2 = modelRuntime->rootModelNode;
    WVar14 = ArmyRuntime_ResolveShotAimPointCf
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,pAVar4->shotDefinition,
                        entityRuntime1);
    point0X = WVar14.worldZQ12;
    point0Y = WVar14.worldYQ12;
    point0Z = WVar14.worldXQ12;
    if (WVar14.carry) {
      WVar13 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      if (((!WVar13.carry) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        modelNode2 = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],modelRuntime,0);
      }
    }
    else {
      modelNode2 = modelRuntime->rootModelNode;
      modelNodeRuntime = modelNode2->childNodes[0];
      SVar10 = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNodeRuntime->worldTransform).translation.z,
                          (modelNodeRuntime->worldTransform).translation.y,
                          (modelNodeRuntime->worldTransform).translation.x,pAVar4->shotDefinition);
      MVar11 = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (modelNode2,SVar10.elevationAngle,SVar10.headingAngle);
      targetPitchAngle16 = MVar11.relativePitchAngle;
      MVar12 = ModelNodeRuntime_SmoothYawTowardTarget
                         (modelNode2,modelRuntime,MVar11.relativeYawAngle);
      if (MVar12.carry) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,modelRuntime,targetPitchAngle16);
      }
      else {
        MVar12 = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,modelRuntime,targetPitchAngle16);
        if ((MVar12.eax == targetPitchAngle16) &&
           (pAVar4 = modelRuntime->modelDefinition,
           modelRuntime->attachmentReloadCountdownTicks24 == 0)) {
          bVar9 = ArmyRuntimeCommand_UpdateTargetFollowingState
                            (point0X,point0Y,point0Z,worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
          if (!bVar9) {
            stepMultiplier = pAVar4->sharedInterShotTicks;
            FVar5 = pAVar4->backwardStepScale3C;
            modelRuntime->attachmentReloadCountdownTicks24 =
                 modelRuntime->attachmentReloadCountdownTicks24 + pAVar4->attachmentReloadTicks;
            if ((modelRuntime->alternatingAttachmentSequence20 & 1) == 0) {
              modelRuntime->attachment1BackwardStepCountdownTicks2C =
                   modelRuntime->attachment1BackwardStepCountdownTicks2C + stepMultiplier;
              modelNode2 = modelNodeRuntime->childNodes[1];
              iVar8 = 4;
            }
            else {
              modelRuntime->attachment0BackwardStepCountdownTicks28 =
                   modelRuntime->attachment0BackwardStepCountdownTicks28 + stepMultiplier;
              modelNode2 = modelNodeRuntime->childNodes[0];
              iVar8 = 0;
            }
            modelRuntime->alternatingAttachmentSequence20 =
                 modelRuntime->alternatingAttachmentSequence20 + 1;
            FixedVector_StepBackwardAlongOwnDirection
                      (stepMultiplier,FVar5,(FixedVectorStateAddress32)modelNode2);
            modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
            ArmyRuntime_SetNonzeroActionVector
                      (SVar10.headingAngle,pAVar4->postLaunchVector1Q12,pAVar4->postLaunchVector0Q12
                       ,modelRuntime->ownerArmyRuntime);
            pAVar6 = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
            shotRuntimeState14 = 0;
            if (pAVar6 != (ArmyRuntimeSlot *)0x0) {
              shotRuntimeState14 = (pAVar6->modelRuntimeOrSavedOffset).savedIdOrOffset;
            }
            ModelRuntime_EmitProjectilesFromAttachmentPoints
                      (shotRuntimeState14,point0X,point0Y,point0Z,pAVar4->shotDefinition,modelNode2,
                       *(MdlSerializedNodeHeader38 **)
                        (iVar8 + pAVar4->modelPointSource64->childSerializedOffsets[0] + 0x18),
                       worldRuntime);
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  return;
}


/* Address: 0x00520140.
   Ownership: gameplay/army/movement.
   Purpose: Table membership RUNTIME_UPDATE[2]. Updates waypoint-driven ground movement, turn acceleration, terrain
   and runtime collision response, model orientation, track-texture animation, and timed shot/effect emitters.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement],
   FieldGrid_InterpolateWaterDelta [world/terrain/grid], ArmyRuntime_ApplyDamageAndPropagateToParent
   [gameplay/army/combat], FixedMath_Vector2AngleAndLengthRegs [core/math/fixed],
   ArmyRuntime_UpdateActivationMetricAndPlayStartSound [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView200 *modelRuntime)

{
  ArmyMovementStateFlags *pAVar1;
  ArmyRuntimeSlot *pAVar2;
  AngleTurn32 AVar3;
  int worldX;
  int worldY;
  ModelDefinitionGroundMovementTrackView280 *pMVar4;
  ArmyPlacementContactKindIndex32 AVar5;
  ModelResourceHitTestAndRenderView210 *pMVar6;
  sdword sVar7;
  int iVar8;
  uint uVar9;
  dword dVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  ModelRuntimeNode *modelNode1;
  bool bVar15;
  FixedLengthAngleEaxEdx8 FVar16;
  FixedPlanarPointEdxEax8 FVar17;
  FixedSinCosEdxEax8 FVar18;
  ArmyCollisionFindEaxCf5 AVar19;
  WorldPositionXYEaxEdxCf9 WVar20;
  FixedEulerAnglesEaxEbxEdx12 FVar21;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  dword dStack_24;
  ModelRuntimeNode *modelNode2;
  
  pAVar2 = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *pAVar1 = *pAVar1 | 4;
  if (pAVar2 != (ArmyRuntimeSlot *)0x0) {
    if (((((pAVar2->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC != 0) &&
       (modelRuntime->modelDefinition->placementRadiusOrClearanceDC != 0)) {
      bVar15 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                         (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x,pAVar2);
      if (bVar15) goto ArmyGroundMovementCollision_ContinueAfterLinkedRuntimeValidation;
    }
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
  }
ArmyGroundMovementCollision_ContinueAfterLinkedRuntimeValidation:
  AVar3 = (modelNode2->modelPayload).worldRotationAngle2;
  worldX = (modelNode2->worldTransform).translation.x;
  worldY = (modelNode2->worldTransform).translation.y;
  pMVar4 = modelRuntime->modelDefinition;
  sVar7 = FieldGrid_InterpolateWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
  if ((pMVar4->waterDamageThreshold198 < sVar7) &&
     (iVar8 = sVar7 * pMVar4->waterDamageMultiplier194 >> 7, -1 < iVar8)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(iVar8,(ArmyRuntimeSlot *)modelRuntime);
  }
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    WVar20 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (WVar20.carry) goto ArmyGroundMovementCollision_StopMovementAndTurn;
    iVar12 = WVar20.worldYQ12 - (modelNode2->worldTransform).translation.y;
    iVar8 = WVar20.worldXQ12 - (modelNode2->worldTransform).translation.x;
    if ((iVar8 == 0) && (iVar12 == 0)) {
      FVar16 = (FixedLengthAngleEaxEdx8)
               ((ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20);
    }
    else {
      FVar16 = FixedMath_Vector2AngleAndLengthRegs(iVar12,iVar8);
    }
    uVar14 = FVar16.angle;
    pMVar4 = modelRuntime->modelDefinition;
    uVar9 = (modelNode2->modelPayload).worldRotationAngle2;
    uVar11 = (modelRuntime->movementControl).turnVelocityAngle16;
    uVar13 = uVar14 - uVar9 & 0xffff;
    if (uVar13 < 0x8000) {
      if ((int)uVar11 < 0) {
ArmyGroundMovementCollision_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (uVar11 < uVar13) {
        uVar9 = uVar9 + uVar11;
        iVar12 = pMVar4->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        iVar8 = uVar11 + pMVar4->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar12;
        if (iVar8 < iVar12) {
          (modelRuntime->movementControl).turnVelocityAngle16 = iVar8;
        }
      }
      else {
ArmyGroundMovementCollision_SnapFacingToDesiredHeading:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
        uVar9 = uVar14;
      }
    }
    else {
      if (0 < (int)uVar11) goto ArmyGroundMovementCollision_ResetTurnVelocityForDirectionReversal;
      if (uVar11 + 0x10000 <= uVar13) goto ArmyGroundMovementCollision_SnapFacingToDesiredHeading;
      uVar9 = uVar9 + uVar11;
      iVar12 = -pMVar4->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      iVar8 = uVar11 - pMVar4->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = iVar12;
      if (iVar12 < iVar8) {
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar8;
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    uVar9 = uVar9 & 0xffff;
    if (uVar9 != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar9;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    dStack_24 = FVar16.length;
    uVar11 = pMVar4->farHeadingErrorLimitAngleC4;
    uVar9 = uVar9 - uVar14 & 0xffff;
    if ((int)dStack_24 < pMVar4->headingErrorInterpolationDistanceQ12C0) {
      uVar11 = pMVar4->nearHeadingErrorLimitAngleC8 +
               (int)(((longlong)(int)(uVar11 - pMVar4->nearHeadingErrorLimitAngleC8) *
                     (longlong)(int)dStack_24) /
                    (longlong)pMVar4->headingErrorInterpolationDistanceQ12C0);
    }
    if ((uVar11 < uVar9) && (uVar9 < 0x10000 - uVar11)) {
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    iVar8 = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (iVar8 < (int)dStack_24 >> 1) {
      FVar17 = FixedTrig_ProjectPlanarPointRegs
                         (iVar8,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pAVar2 = modelRuntime->ownerArmyRuntime;
      WVar20 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pAVar2);
      FVar17 = WVar20._0_8_;
      pAVar1 = &pAVar2->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    AVar5 = pMVar4->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = pMVar4->placementHeightOffsetQ12;
    worldRuntime_00 = worldRuntime;
    AVar19 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                       ((Q12)(FVar17 >> 0x20),(Q12)FVar17,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (AVar19.carry) {
      modelNode1 = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,(ArmyRuntimeSlot *)AVar19.eax,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar17._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar17._4_4_ = (modelNode1->worldTransform).translation.y;
      pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    pAVar2 = modelRuntime->ownerArmyRuntime;
    modelNode1 = modelRuntime->rootModelNode;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[AVar5])
              (heightOffsetQ12,(Q12)(FVar17 >> 0x20),(Q12)FVar17,modelNode2,worldRuntime_00);
    iVar8 = pAVar2->actionVector1Q12 + -1;
    if (iVar8 < 0) goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    iVar8 = iVar8 * pAVar2->actionVector2Q12;
    pAVar2->actionVector1Q12 = pAVar2->actionVector1Q12 + -1;
  }
  else {
ArmyGroundMovementCollision_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState:
    pAVar2 = modelRuntime->ownerArmyRuntime;
    modelNode1 = modelRuntime->rootModelNode;
    iVar8 = pAVar2->actionVector1Q12 + -1;
    if (iVar8 < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x,modelNode1,worldRuntime);
      }
      goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    }
    iVar8 = iVar8 * pAVar2->actionVector2Q12;
    pAVar2->actionVector1Q12 = pAVar2->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode1->worldTransform).translation.y,(modelNode1->worldTransform).translation.x
               ,modelNode1,worldRuntime);
  }
  FVar21 = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - iVar8,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode1->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode1->modelPayload).worldRotationAngle2,
                      (modelNode1->modelPayload).worldRotationAngle1,
                      (modelNode1->modelPayload).worldRotationAngle0);
  (modelNode1->modelPayload).worldRotationAngle0 = FVar21.angle0;
  (modelNode1->modelPayload).worldRotationAngle1 = FVar21.angle1;
  (modelNode1->modelPayload).worldRotationAngle2 = FVar21.angle2;
ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms:
  pMVar6 = (modelNode1->modelPayload).modelResource;
  uVar14 = AVar3 - 0x4000 & 0xffff;
  uVar9 = (modelNode1->modelPayload).worldRotationAngle2 - 0x4000 & 0xffff;
  FVar18 = FixedMath_SinCosScaled(uVar9,pMVar6->localBoundsY0Q12);
  iVar8 = (int)FVar18 + (modelNode1->worldTransform).translation.x;
  iVar12 = (int)(FVar18 >> 0x20) + (modelNode1->worldTransform).translation.y;
  FVar18 = FixedMath_SinCosScaled(uVar14,pMVar6->localBoundsY0Q12);
  FVar16 = FixedMath_Vector2AngleAndLengthRegs
                     (iVar12 - ((int)(FVar18 >> 0x20) + worldY),iVar8 - ((int)FVar18 + worldX));
  dVar10 = FVar16.length;
  uVar11 = FVar16.angle - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < uVar11) && (uVar11 < 0xc000)) {
    dVar10 = -dVar10;
  }
  modelNode1->primaryTextureOffsetU =
       modelNode1->primaryTextureOffsetU +
       dVar10 * modelRuntime->modelDefinition->trackTextureUScalePerDistance14;
  FVar18 = FixedMath_SinCosScaled(uVar9 ^ 0x8000,pMVar6->localBoundsY0Q12);
  iVar8 = (int)FVar18 + (modelNode1->worldTransform).translation.x;
  iVar12 = (int)(FVar18 >> 0x20) + (modelNode1->worldTransform).translation.y;
  FVar18 = FixedMath_SinCosScaled(uVar14 ^ 0x8000,pMVar6->localBoundsY0Q12);
  FVar16 = FixedMath_Vector2AngleAndLengthRegs
                     (iVar12 - ((int)(FVar18 >> 0x20) + worldY),iVar8 - ((int)FVar18 + worldX));
  dVar10 = FVar16.length;
  uVar9 = FVar16.angle - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < uVar9) && (uVar9 < 0xc000)) {
    dVar10 = -dVar10;
  }
  iVar8 = modelNode1->primaryTextureOffsetU;
  iVar12 = dVar10 * modelRuntime->modelDefinition->trackTextureUScalePerDistance14 +
           modelNode1->secondaryTextureOffsetU;
  if (iVar12 < 0x100001) {
    if (iVar12 < -0x100000) {
      iVar12 = iVar12 + 0x100000;
    }
  }
  else {
    iVar12 = iVar12 + -0x100000;
  }
  if (iVar8 < 0x100001) {
    if (iVar8 < -0x100000) {
      iVar8 = iVar8 + 0x100000;
    }
  }
  else {
    iVar8 = iVar8 + -0x100000;
  }
  modelNode1->secondaryTextureOffsetU = iVar12;
  modelNode1->primaryTextureOffsetU = iVar8;
  if (((worldX != (modelNode1->worldTransform).translation.x) ||
      (worldY != (modelNode1->worldTransform).translation.y)) ||
     (AVar3 != (modelNode1->modelPayload).worldRotationAngle2)) {
    pAVar2 = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    pAVar1 = &pAVar2->movementStateFlags;
    *pAVar1 = *pAVar1 & 0xfffffffb;
  }
  pMVar4 = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  ModelNodeRuntime_UpdateDepthBinMasks(pMVar4->placementRadiusOrClearanceDC,modelNode1);
  return;
}


/* Address: 0x00522C00.
   Ownership: gameplay/army/movement.
   Purpose: Table membership RUNTIME_UPDATE[17]. Updates route movement, yaw and banking state, child-model
   animation, terrain contact, collision response, and timed effects for the class-table implementation. Runtime-
   update partition slots 0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement],
   TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid], SpatialSound_PlayPositionedOneShot
   [audio/spatial/runtime], FixedMath_Vector2AngleAndLengthRegs [core/math/fixed],
   ArmyRuntime_UpdateActivationMetricAndPlayStartSound [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyMovementStateFlags *pAVar1;
  AngleTurn32 *pAVar2;
  dword *pdVar3;
  ModelDefinitionGroundMovementSteeringView280 *pMVar4;
  DirectSoundVoiceSet **voiceSetRef;
  AngleTurn32 AVar5;
  int iVar6;
  int iVar7;
  ModelRuntimeNode *pMVar8;
  AngleTurn32 AVar9;
  ArmyPlacementContactKindIndex32 AVar10;
  dword dVar11;
  int iVar12;
  GraphicsFixedVec3 *worldPosition;
  ArmyRuntimeSlot *pAVar13;
  int iVar14;
  AngleTurn32 angle16;
  uint uVar15;
  uint uVar16;
  ModelRuntimeNode *modelNode2;
  bool bVar17;
  FixedPlanarPointEdxEax8 FVar18;
  ArmyCollisionFindEaxCf5 AVar19;
  WorldPositionXYEaxEdxCf9 WVar20;
  FixedEulerAnglesEaxEbxEdx12 FVar21;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  FixedLengthAngleEaxEdx8 FStack_24;
  ModelRuntimeNode *modelNode1;
  
  pAVar13 = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *pAVar1 = *pAVar1 | 4;
  if (pAVar13 != (ArmyRuntimeSlot *)0x0) {
    pMVar4 = modelRuntime->modelDefinition;
    if (((((pAVar13->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC != 0) &&
       (pMVar4->placementRadiusOrClearanceDC != 0)) {
      bVar17 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                         (pMVar4->placementRadiusOrClearanceDC,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x,pAVar13);
      if (bVar17) goto ArmyMovementBanking_ContinueAfterLinkedRuntimeValidation;
    }
    uVar16 = modelNode2->childCount;
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
    if ((2 < uVar16) && (((modelRuntime->classState).classStateB8 & 4) != 0)) {
      pdVar3 = &(modelRuntime->classState).classStateB8;
      *pdVar3 = *pdVar3 | 1;
      uVar16 = *(uint *)(pMVar4->reserved26C_277 + 8);
      if ((uVar16 != 0) &&
         ((uVar16 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
        voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar16];
        if (voiceSetRef != (DirectSoundVoiceSet **)0x0) {
          worldPosition = &(modelRuntime->rootModelNode->worldTransform).translation;
          bVar17 = TerrainGrid_TestProjectedCellMaskBits01Cf
                             ((modelRuntime->rootModelNode->worldTransform).translation.y,
                              worldPosition->x,worldRuntime);
          if (!bVar17) {
            SpatialSound_PlayPositionedOneShot
                      (pMVar4->positionedSoundMaximumDistanceQ12,pMVar4->positionedSoundGainQ15,
                       worldPosition,voiceSetRef);
          }
        }
      }
    }
  }
ArmyMovementBanking_ContinueAfterLinkedRuntimeValidation:
  modelNode2 = modelRuntime->rootModelNode;
  AVar5 = (modelNode2->modelPayload).worldRotationAngle2;
  iVar6 = (modelNode2->worldTransform).translation.x;
  iVar7 = (modelNode2->worldTransform).translation.y;
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    if (((modelRuntime->classState).classStateB8 & 1) != 0) {
      pdVar3 = &(modelRuntime->classLinkState).classState70;
      *pdVar3 = *pdVar3 - 1;
      if (*pdVar3 == 0) {
        pdVar3 = &(modelRuntime->classState).classStateB8;
        *pdVar3 = *pdVar3 & 0xfffffff8;
      }
      modelNode1 = modelNode2->childNodes[1];
      pMVar8 = modelNode2->childNodes[2];
      pAVar2 = &(modelNode2->childNodes[0]->modelPayload).localRotationAngle1;
      *pAVar2 = *pAVar2 + 0x2aa;
      pAVar2 = &(modelNode1->modelPayload).localRotationAngle1;
      *pAVar2 = *pAVar2 + 0x2aa;
      pAVar2 = &(pMVar8->modelPayload).localRotationAngle1;
      *pAVar2 = *pAVar2 + 0x2aa;
    }
    WVar20 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    pAVar13 = (ArmyRuntimeSlot *)WVar20.worldYQ12;
    dVar11 = WVar20.worldXQ12;
    if (WVar20.carry) goto ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState;
    if ((dVar11 != (modelRuntime->classLinkState).classState68) &&
       (pAVar13 != (modelRuntime->classLinkState).armyLinkOrState6C.armyRuntime)) {
      (modelRuntime->classLinkState).classState68 = dVar11;
      (modelRuntime->classLinkState).armyLinkOrState6C.classState = (dword)pAVar13;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    }
    iVar12 = dVar11 - (modelNode2->worldTransform).translation.x;
    iVar14 = (int)pAVar13 - (modelNode2->worldTransform).translation.y;
    if ((iVar12 == 0) && (iVar14 == 0)) {
      FStack_24 = (FixedLengthAngleEaxEdx8)
                  ((ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20);
    }
    else {
      FStack_24 = FixedMath_Vector2AngleAndLengthRegs(iVar14,iVar12);
    }
    angle16 = FStack_24.angle;
    pMVar4 = modelRuntime->modelDefinition;
    uVar16 = (modelRuntime->movementControl).turnVelocityAngle16;
    if ((uVar16 == 0) && (FStack_24.length < (uint)pMVar4->headingErrorInterpolationDistanceQ12C0))
    {
ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity:
      (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      uVar15 = (modelRuntime->rootModelNode->modelPayload).worldRotationAngle2;
    }
    else {
      AVar9 = (modelNode2->modelPayload).worldRotationAngle2;
      uVar15 = angle16 - AVar9 & 0xffff;
      if (uVar15 < 0x8000) {
        if ((int)uVar16 < 0) goto ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity;
        if (uVar16 < uVar15) {
          iVar14 = pMVar4->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
          iVar12 = uVar16 + pMVar4->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks
          ;
          (modelRuntime->movementControl).turnVelocityAngle16 = iVar14;
          uVar15 = AVar9 + uVar16;
          if (iVar12 < iVar14) {
            (modelRuntime->movementControl).turnVelocityAngle16 = iVar12;
          }
        }
        else {
ArmyMovementBanking_SnapFacingToDesiredHeading:
          (modelRuntime->movementControl).turnVelocityAngle16 = 0;
          uVar15 = angle16;
        }
      }
      else {
        if (0 < (int)uVar16) goto ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity;
        if (uVar16 + 0x10000 <= uVar15) goto ArmyMovementBanking_SnapFacingToDesiredHeading;
        uVar15 = AVar9 + uVar16;
        iVar14 = -pMVar4->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        iVar12 = uVar16 - pMVar4->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar14;
        if (iVar14 < iVar12) {
          (modelRuntime->movementControl).turnVelocityAngle16 = iVar12;
        }
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    if ((uVar15 & 0xffff) != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar15 & 0xffff;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    iVar12 = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks
    ;
    if (iVar12 < (int)FStack_24.length >> 1) {
      FVar18 = FixedTrig_ProjectPlanarPointRegs
                         (iVar12,angle16,(modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pAVar13 = modelRuntime->ownerArmyRuntime;
      WVar20 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pAVar13);
      FVar18 = WVar20._0_8_;
      pAVar1 = &pAVar13->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    AVar10 = pMVar4->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = pMVar4->placementHeightOffsetQ12;
    worldRuntime_00 = worldRuntime;
    AVar19 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                       ((Q12)(FVar18 >> 0x20),(Q12)FVar18,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (AVar19.carry) {
      modelNode1 = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,(ArmyRuntimeSlot *)AVar19.eax,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar18._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar18._4_4_ = (modelNode1->worldTransform).translation.y;
      pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    pAVar13 = modelRuntime->ownerArmyRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[AVar10])
              (heightOffsetQ12,(Q12)(FVar18 >> 0x20),(Q12)FVar18,modelNode2,worldRuntime_00);
    modelNode2 = modelRuntime->rootModelNode;
    iVar12 = pAVar13->actionVector1Q12 + -1;
    if (iVar12 < 0) goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    iVar12 = iVar12 * pAVar13->actionVector2Q12;
    pAVar13->actionVector1Q12 = pAVar13->actionVector1Q12 + -1;
  }
  else {
ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
    pAVar13 = modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    iVar12 = pAVar13->actionVector1Q12 + -1;
    if (iVar12 < 0) {
      (*g_ArmyPlacementContactKindDispatchTable.callbacks
        [modelRuntime->modelDefinition->placementContactKindIndex278])
                (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    }
    iVar12 = iVar12 * pAVar13->actionVector2Q12;
    pAVar13->actionVector1Q12 = pAVar13->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,modelNode2,worldRuntime);
  }
  FVar21 = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - iVar12,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode2->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = FVar21.angle0;
  (modelNode2->modelPayload).worldRotationAngle1 = FVar21.angle1;
  (modelNode2->modelPayload).worldRotationAngle2 = FVar21.angle2;
ArmyMovementBanking_FinalizeBankingEffectsAndTransforms:
  pMVar4 = modelRuntime->modelDefinition;
  if ((modelRuntime->classLinkState).modelLinkOrState60.modelRuntime != (ModelRuntimeSlot *)0x4000)
  {
    FVar21 = FixedTransform_ComposeEulerAnglesRegs
                       ((modelNode2->modelPayload).worldRotationAngle2,
                        (modelNode2->modelPayload).worldRotationAngle1,
                        (modelNode2->modelPayload).worldRotationAngle0,0,
                        (modelRuntime->classLinkState).modelLinkOrState60.classState,
                        (modelRuntime->classLinkState).classState64);
    (modelNode2->modelPayload).worldRotationAngle0 = FVar21.angle0;
    (modelNode2->modelPayload).worldRotationAngle1 = FVar21.angle1;
    (modelNode2->modelPayload).worldRotationAngle2 = FVar21.angle2;
  }
  uVar16 = FStack_24.angle - (modelRuntime->classLinkState).classState64 & 0xffff;
  if (((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) ||
     ((int)FStack_24.length <= pMVar4->movementStepQ12PerTick0C * 0x20)) {
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)
         &((modelRuntime->classLinkState).modelLinkOrState60.modelRuntime)->definitionValue84_40;
    if (0x4000 < (modelRuntime->classLinkState).modelLinkOrState60.signedScalarState) {
      (modelRuntime->classLinkState).modelLinkOrState60.classState = 0x4000;
    }
  }
  else {
    if (uVar16 < 0x8001) {
      if (0x400 < uVar16) {
        uVar16 = 0x400;
      }
    }
    else if (uVar16 < 0xfc00) {
      uVar16 = 0xfffffc00;
    }
    (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime =
         (ModelRuntimeSlot *)((modelRuntime->classLinkState).modelLinkOrState60.classState - 0x100);
    (modelRuntime->classLinkState).classState64 =
         uVar16 + (modelRuntime->classLinkState).classState64 & 0xffff;
    if ((modelRuntime->classLinkState).modelLinkOrState60.signedScalarState < 0x3800) {
      (modelRuntime->classLinkState).modelLinkOrState60.classState = 0x3800;
    }
  }
  if (((modelRuntime->classState).classStateB8 & 2) == 0) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
  }
  if (((iVar6 != (modelNode2->worldTransform).translation.x) ||
      (iVar7 != (modelNode2->worldTransform).translation.y)) ||
     (AVar5 != (modelNode2->modelPayload).worldRotationAngle2)) {
    pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
    *pAVar1 = *pAVar1 & 0xfffffffb;
  }
  pMVar4 = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_UpdateDepthBinMasks(pMVar4->placementRadiusOrClearanceDC,modelNode2);
  return;
}


/* Address: 0x0051C3E0.
   Ownership: gameplay/army/movement.
   Purpose: Resets movement flags and target state from the current model position, updates the verified command-
   state word, and clears inherited movement flags through the attached model hierarchy.
   Cross-module calls: ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime)

{
  dword *pdVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  GraphicsWorldCoordinateQ12 GVar3;
  ArmyCommandGeneration AVar4;
  int iVar5;
  int unaff_EBP;
  int unaff_EDI;
  ModelRuntimeSlot *pMVar6;
  bool bVar7;
  int iVar8;
  ModelRuntimeNode *modelNode1;
  
  AVar4 = g_ArmyCommandGenerationStandard;
  modelNode1 = armyRuntime->modelNodeRuntime;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & 0xffffffc6;
  bVar7 = ArmyRuntime_TestStateField100ZeroCf(armyRuntime);
  if ((!bVar7) && ((armyRuntime->commandModeFlags & 3) != 0)) {
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags & 0xfffffffc;
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 4;
    armyRuntime->commandGeneration = AVar4;
    armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  GVar2 = (modelNode1->worldTransform).translation.x;
  GVar3 = (modelNode1->worldTransform).translation.y;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = GVar2;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = GVar3;
  armyRuntime->movementTarget0Q12 = GVar2;
  armyRuntime->movementTarget1Q12 = GVar3;
  armyRuntime->movementPosition0Q12 = GVar2;
  armyRuntime->movementPosition1Q12 = GVar3;
  pMVar6 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  iVar5 = 0;
  if ((((pMVar6->classState).classStateEC & 0x10) != 0) && (pMVar6->definitionValue60_3C != 0)) {
    do {
      pdVar1 = &(pMVar6->classState).classStateEC;
      *pdVar1 = *pdVar1 & 0xfffffde7;
      iVar5 = iVar5 + 1;
      iVar8 = unaff_EDI;
      do {
        while (unaff_EBP == 0) {
          iVar5 = iVar5 + -1;
          if (iVar5 == 0) {
            return;
          }
        }
        unaff_EBP = unaff_EBP + -1;
        unaff_EDI = iVar8 + 0x20;
        pMVar6 = *(ModelRuntimeSlot **)(iVar8 + 0x140);
        iVar8 = unaff_EDI;
      } while (pMVar6 == (ModelRuntimeSlot *)0x0);
    } while( true );
  }
  return;
}


/* Address: 0x0051C500.
   Ownership: gameplay/army/movement.
   Purpose: If target-command mode bits 0 or 1 are active, clears them, sets transition bit 2, stamps the standard
   command generation, and clears the current command target. The helper is also used by the 32-slot command reset
   traversal.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;
  
  commandGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->commandModeFlags & 3) != 0) {
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags & 0xfffffffc;
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 4;
    armyRuntime->commandGeneration = commandGeneration;
    armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  return;
}


/* Address: 0x0051CAF0.
   Ownership: gameplay/army/movement.
   Purpose: Typed parameters: p2 auxiliaryValue1→ArmyMoveAuxiliaryValue1_V344, p3
   auxiliaryValue0→ArmyMoveAuxiliaryValue0_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar3;
  
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x24b;
  movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff4f;
  movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xffffffef;
  movementRuntime->queuedWaypointCount = 1;
  EVar3 = EntityPathing_ResolveDestinationAndRebuildRoutes
                    (targetWorldYQ12,targetWorldXQ12,movementRuntime->entityRuntime,worldRuntime);
  (movementRuntime->fallbackPosition).worldXQ12 = EVar3.fallbackWorldXQ12;
  (movementRuntime->fallbackPosition).worldYQ12 = EVar3.fallbackWorldYQ12;
  movementRuntime->movementTargetWorldXQ12 = EVar3.fallbackWorldXQ12;
  movementRuntime->movementTargetWorldYQ12 = EVar3.fallbackWorldYQ12;
  movementRuntime->movementWorldXQ12 = EVar3.primaryWorldXQ12;
  movementRuntime->movementWorldYQ12 = EVar3.primaryWorldYQ12;
  GVar1 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
  GVar2 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
  movementRuntime->retryCountdown = 0x40;
  movementRuntime->lastCheckedWorldXQ12 = GVar1;
  movementRuntime->lastCheckedWorldYQ12 = GVar2;
  movementRuntime->queuedWaypoints[0].worldXQ12 = auxiliaryValue0;
  movementRuntime->queuedWaypoints[0].worldYQ12 = auxiliaryValue1;
  return;
}


/* Address: 0x0051CDB0.
   Ownership: gameplay/army/movement.
   Purpose: Stores a pending movement target, mirrors it into the verified queue and fallback fields according to
   current flags, and records the current model position.
*/
void __thandor_preserve_eax_edx
ArmyRuntime_SetPendingMoveTarget
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  ModelRuntimeNode *modelNode1;
  
  if ((movementRuntime->movementStateFlags & 2) == 0) {
    modelNode1 = movementRuntime->modelNodeRuntime;
    movementRuntime->movementWorldXQ12 = targetWorldX;
    movementRuntime->movementWorldYQ12 = targetWorldY;
    if ((movementRuntime->movementStateFlags & 1) == 0) {
      (movementRuntime->fallbackPosition).worldXQ12 = targetWorldX;
      (movementRuntime->fallbackPosition).worldYQ12 = targetWorldY;
    }
    if ((movementRuntime->movementStateFlags & 0x400) != 0) {
      movementRuntime->movementTargetWorldXQ12 = targetWorldX;
      movementRuntime->movementTargetWorldYQ12 = targetWorldY;
    }
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 1;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffffcf;
    GVar1 = (modelNode1->worldTransform).translation.x;
    GVar2 = (modelNode1->worldTransform).translation.y;
    movementRuntime->retryCountdown = 0x40;
    movementRuntime->lastCheckedWorldXQ12 = GVar1;
    movementRuntime->lastCheckedWorldYQ12 = GVar2;
  }
  return;
}


/* Address: 0x00521680.
   Ownership: gameplay/army/movement.
   Purpose: Projects the root model onto the field surface and initializes the paired lateral contact-point
   coordinates, heading values, and fixed Q30 orientation terms used by the articulated runtime. Two stack
   arguments are authoritative from RET 0x08; prior register parameters and return are synthetic.
   Cross-module calls: FieldGrid_InterpolateTerrainHeight [world/terrain/grid],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy], FixedMath_SinCosScaled [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeCoordinateCommandOrHistoryValue4 AVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
  int iVar5;
  FixedSinCosEdxEax8 FVar6;
  FieldGridHeightEaxCf5 FVar7;
  Q12 worldXQ12;
  Q12 worldYQ12;
  ArmyArticulatedRuntimeSlotView *pAVar1;
  
  pAVar1 = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).modelRuntime;
  worldXQ12 = (modelNodeRuntime->worldTransform).translation.x;
  worldYQ12 = (modelNodeRuntime->worldTransform).translation.y;
  dVar2 = 0;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar7 = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    dVar2 = FVar7.heightQ12;
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  FVar6 = FixedMath_SinCosScaled
                    ((modelNodeRuntime->modelPayload).worldRotationAngle2 + 0x4000 & 0xffff,
                     (pAVar1->articulatedContact).lateralOffsetQ12);
  iVar5 = (int)(FVar6 >> 0x20);
  dVar3 = worldXQ12 + (int)FVar6;
  dVar4 = worldYQ12 + iVar5;
  pAVar1->movementTarget0Q12 = dVar3;
  pAVar1->definitionClassValue80 = dVar4;
  pAVar1->definitionClassValue88 = dVar2;
  pAVar1->runtimeState90 = dVar3;
  pAVar1->runtimeState98 = dVar4;
  pAVar1->articulatedHeightOrStateA0 = dVar2;
  dVar3 = worldXQ12 - (int)FVar6;
  dVar4 = worldYQ12 - iVar5;
  pAVar1->movementTarget1Q12 = dVar3;
  pAVar1->definitionClassValue84 = dVar4;
  pAVar1->runtimeState8C = dVar2;
  pAVar1->runtimeState94 = dVar3;
  pAVar1->articulatedCoordinateOrState9C = dVar4;
  pAVar1->runtimeStateA4 = dVar2;
  AVar1 = (ArmyRuntimeCoordinateCommandOrHistoryValue4)
          (modelNodeRuntime->modelPayload).worldRotationAngle2;
  pAVar1->classState60 = (dword)AVar1;
  pAVar1->ownerValue64 = (dword)AVar1;
  (pAVar1->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = AVar1;
  (pAVar1->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue = AVar1;
  (pAVar1->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = AVar1;
  (pAVar1->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue = AVar1;
  pAVar1->ownerValue68 = 0x40000000;
  pAVar1->fallbackWorldYQ12 = 0x40000000;
  pAVar1->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)0x40000000;
  pAVar1->fallbackWorldXQ12 = 0x40000000;
  return;
}


/* Address: 0x00527BC0.
   Ownership: gameplay/army/movement.
   Purpose: Third exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Default class-command no-op used by fourteen class slots.
*/
void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x0051C8E0.
   Ownership: gameplay/army/movement.
   Purpose: Handles army runtime queue or start move command variant a.
   Local calls: ArmyRuntime_QueueWaypointOrStartMoveVariantA.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueOrStartMoveCommandVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar3;
  
  if (*(int *)((int)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
              0x18) != 0) {
    if ((movementRuntime->movementStateFlags & 2) == 0) {
      worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x241;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffb47;
      movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xffffffef;
      EVar3 = EntityPathing_ResolveDestinationAndRebuildRoutes
                        (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
      (movementRuntime->fallbackPosition).worldXQ12 = EVar3.fallbackWorldXQ12;
      (movementRuntime->fallbackPosition).worldYQ12 = EVar3.fallbackWorldYQ12;
      movementRuntime->movementTargetWorldXQ12 = EVar3.fallbackWorldXQ12;
      movementRuntime->movementTargetWorldYQ12 = EVar3.fallbackWorldYQ12;
      movementRuntime->movementWorldXQ12 = EVar3.primaryWorldXQ12;
      movementRuntime->movementWorldYQ12 = EVar3.primaryWorldYQ12;
      GVar1 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
      GVar2 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
      movementRuntime->retryCountdown = 0x40;
      movementRuntime->lastCheckedWorldXQ12 = GVar1;
      movementRuntime->lastCheckedWorldYQ12 = GVar2;
    }
    else {
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffff7;
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(targetWorldY,targetWorldX,movementRuntime);
    }
  }
  return;
}


/* Address: 0x0051C9A0.
   Ownership: gameplay/army/movement.
   Purpose: Handles army runtime queue or start move command variant b.
   Local calls: ArmyRuntime_QueueWaypointOrStartMoveVariantA.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueOrStartMoveCommandVariantB
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar3;
  
  if (*(int *)((int)(movementRuntime->entityRuntime->common).ownership.definitionOrClassRecord +
              0x18) != 0) {
    if ((movementRuntime->movementStateFlags & 2) == 0) {
      worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x241;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff4f;
      movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xffffffef;
      EVar3 = EntityPathing_ResolveDestinationAndRebuildRoutes
                        (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
      (movementRuntime->fallbackPosition).worldXQ12 = EVar3.fallbackWorldXQ12;
      (movementRuntime->fallbackPosition).worldYQ12 = EVar3.fallbackWorldYQ12;
      movementRuntime->movementTargetWorldXQ12 = EVar3.fallbackWorldXQ12;
      movementRuntime->movementTargetWorldYQ12 = EVar3.fallbackWorldYQ12;
      movementRuntime->movementWorldXQ12 = EVar3.primaryWorldXQ12;
      movementRuntime->movementWorldYQ12 = EVar3.primaryWorldYQ12;
      GVar1 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
      GVar2 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
      movementRuntime->retryCountdown = 0x40;
      movementRuntime->lastCheckedWorldXQ12 = GVar1;
      movementRuntime->lastCheckedWorldYQ12 = GVar2;
    }
    else {
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffff7;
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(targetWorldY,targetWorldX,movementRuntime);
    }
  }
  return;
}


/* Address: 0x00520840.
   Ownership: gameplay/army/movement.
   Purpose: Updates route-following, turn acceleration, terrain clearance, collision response, model orientation,
   and depth-bin state for the first verified ground-movement runtime class. Runtime-update partition slots 0-23
   receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement],
   FieldGrid_InterpolateWaterDelta [world/terrain/grid], ArmyRuntime_ApplyDamageAndPropagateToParent
   [gameplay/army/combat], FixedMath_Vector2AngleAndLengthRegs [core/math/fixed],
   ArmyRuntime_UpdateActivationMetricAndPlayStartSound [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs
   [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementVariantA
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyMovementStateFlags *pAVar1;
  ArmyRuntimeSlot *pAVar2;
  AngleTurn32 AVar3;
  int worldX;
  int worldY;
  ModelDefinitionGroundMovementSteeringView280 *pMVar4;
  ArmyPlacementContactKindIndex32 AVar5;
  sdword sVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  ModelRuntimeNode *modelNode2;
  bool bVar13;
  FixedLengthAngleEaxEdx8 FVar14;
  FixedPlanarPointEdxEax8 FVar15;
  ArmyCollisionFindEaxCf5 AVar16;
  WorldPositionXYEaxEdxCf9 WVar17;
  FixedEulerAnglesEaxEbxEdx12 FVar18;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  dword local_24;
  ModelRuntimeNode *modelNode1;
  
  pAVar2 = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *pAVar1 = *pAVar1 | 4;
  if (pAVar2 != (ArmyRuntimeSlot *)0x0) {
    if (((((pAVar2->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC != 0) &&
       (modelRuntime->modelDefinition->placementRadiusOrClearanceDC != 0)) {
      bVar13 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                         (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x,pAVar2);
      if (bVar13) goto ArmyGroundMovementVariantA_ContinueAfterLinkedRuntimeValidation;
    }
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
  }
ArmyGroundMovementVariantA_ContinueAfterLinkedRuntimeValidation:
  AVar3 = (modelNode2->modelPayload).worldRotationAngle2;
  worldX = (modelNode2->worldTransform).translation.x;
  worldY = (modelNode2->worldTransform).translation.y;
  pMVar4 = modelRuntime->modelDefinition;
  sVar6 = FieldGrid_InterpolateWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
  if ((pMVar4->waterDamageThreshold198 < sVar6) &&
     (iVar7 = sVar6 * pMVar4->waterDamageMultiplier194 >> 7, -1 < iVar7)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(iVar7,(ArmyRuntimeSlot *)modelRuntime);
  }
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    WVar17 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (WVar17.carry) goto ArmyGroundMovementVariantA_StopMovementAndTurn;
    iVar10 = WVar17.worldYQ12 - (modelNode2->worldTransform).translation.y;
    iVar7 = WVar17.worldXQ12 - (modelNode2->worldTransform).translation.x;
    if ((iVar7 == 0) && (iVar10 == 0)) {
      FVar14 = (FixedLengthAngleEaxEdx8)
               ((ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20);
    }
    else {
      FVar14 = FixedMath_Vector2AngleAndLengthRegs(iVar10,iVar7);
    }
    uVar11 = FVar14.angle;
    pMVar4 = modelRuntime->modelDefinition;
    uVar8 = (modelNode2->modelPayload).worldRotationAngle2;
    uVar9 = (modelRuntime->movementControl).turnVelocityAngle16;
    uVar12 = uVar11 - uVar8 & 0xffff;
    if (uVar12 < 0x8000) {
      if ((int)uVar9 < 0) {
ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (uVar9 < uVar12) {
        uVar8 = uVar8 + uVar9;
        iVar10 = pMVar4->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        iVar7 = uVar9 + pMVar4->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar10;
        if (iVar7 < iVar10) {
          (modelRuntime->movementControl).turnVelocityAngle16 = iVar7;
        }
      }
      else {
ArmyGroundMovementVariantA_SnapFacingToDesiredHeading:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
        uVar8 = uVar11;
      }
    }
    else {
      if (0 < (int)uVar9) goto ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal;
      if (uVar9 + 0x10000 <= uVar12) goto ArmyGroundMovementVariantA_SnapFacingToDesiredHeading;
      uVar8 = uVar8 + uVar9;
      iVar10 = -pMVar4->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      iVar7 = uVar9 - pMVar4->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = iVar10;
      if (iVar10 < iVar7) {
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar7;
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    uVar8 = uVar8 & 0xffff;
    if (uVar8 != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar8;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    local_24 = FVar14.length;
    uVar9 = pMVar4->farHeadingErrorLimitAngleC4;
    uVar8 = uVar8 - uVar11 & 0xffff;
    if ((int)local_24 < pMVar4->headingErrorInterpolationDistanceQ12C0) {
      uVar9 = pMVar4->nearHeadingErrorLimitAngleC8 +
              (int)(((longlong)(int)(uVar9 - pMVar4->nearHeadingErrorLimitAngleC8) *
                    (longlong)(int)local_24) /
                   (longlong)pMVar4->headingErrorInterpolationDistanceQ12C0);
    }
    if ((uVar9 < uVar8) && (uVar8 < 0x10000 - uVar9)) {
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    iVar7 = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (iVar7 < (int)local_24 >> 1) {
      FVar15 = FixedTrig_ProjectPlanarPointRegs
                         (iVar7,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pAVar2 = modelRuntime->ownerArmyRuntime;
      WVar17 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pAVar2);
      FVar15 = WVar17._0_8_;
      pAVar1 = &pAVar2->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    AVar5 = pMVar4->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = pMVar4->placementHeightOffsetQ12;
    worldRuntime_00 = worldRuntime;
    AVar16 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                       ((Q12)(FVar15 >> 0x20),(Q12)FVar15,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (AVar16.carry) {
      modelNode1 = modelRuntime->rootModelNode;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)modelRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,(ArmyRuntimeSlot *)AVar16.eax,
                 worldRuntime);
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar15._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar15._4_4_ = (modelNode1->worldTransform).translation.y;
      pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    pAVar2 = modelRuntime->ownerArmyRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[AVar5])
              (heightOffsetQ12,(Q12)(FVar15 >> 0x20),(Q12)FVar15,modelNode2,worldRuntime_00);
    modelNode2 = modelRuntime->rootModelNode;
    iVar7 = pAVar2->actionVector1Q12 + -1;
    if (iVar7 < 0) goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    iVar7 = iVar7 * pAVar2->actionVector2Q12;
    pAVar2->actionVector1Q12 = pAVar2->actionVector1Q12 + -1;
  }
  else {
ArmyGroundMovementVariantA_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState:
    pAVar2 = modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    iVar7 = pAVar2->actionVector1Q12 + -1;
    if (iVar7 < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (modelNode2->worldTransform).translation.y,
                   (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      }
      goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    }
    iVar7 = iVar7 * pAVar2->actionVector2Q12;
    pAVar2->actionVector1Q12 = pAVar2->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,modelNode2,worldRuntime);
  }
  FVar18 = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - iVar7,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode2->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = FVar18.angle0;
  (modelNode2->modelPayload).worldRotationAngle1 = FVar18.angle1;
  (modelNode2->modelPayload).worldRotationAngle2 = FVar18.angle2;
ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms:
  if (((worldX != (modelNode2->worldTransform).translation.x) ||
      (worldY != (modelNode2->worldTransform).translation.y)) ||
     (AVar3 != (modelNode2->modelPayload).worldRotationAngle2)) {
    pAVar2 = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    pAVar1 = &pAVar2->movementStateFlags;
    *pAVar1 = *pAVar1 & 0xfffffffb;
  }
  pMVar4 = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_UpdateDepthBinMasks(pMVar4->placementRadiusOrClearanceDC,modelNode2);
  return;
}


/* Address: 0x00522090.
   Ownership: gameplay/army/movement.
   Purpose: Updates the left-side articulated terrain contact, retries from the previous contact on obstruction,
   and derives the Q13 interpolation step from the resolved segment length. Four stack arguments are authoritative
   from RET 0x10; EDX:EAX preserves the resolved world X/Y contact pair.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FixedMath_Length2 [core/math/fixed],
   FieldGrid_InterpolateTerrainHeightAndNormal [world/terrain/grid],
   ArmyCollision_FindBlockingRuntimeForCurrentUnitCf [gameplay/army/placement], ArmyRuntime_HandleCollisionPartner
   [gameplay/army/runtime], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateLeftTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *pGVar1;
  Q12 *pQVar2;
  void *pvVar3;
  FieldGridAsset *pFVar4;
  dword dVar5;
  uint uVar6;
  int iVar7;
  uint angle;
  int iVar8;
  dword dVar9;
  FixedSinCosEdxEax8 FVar10;
  FixedSinCosEdxEax8 FVar11;
  ArmyCollisionFindEaxCf5 AVar12;
  WorldPositionXYEaxEdxCf9 WVar13;
  FieldGridHeightNormalEaxEdxCf9 FVar14;
  UQ12 UVar15;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pvVar3 = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  angle = headingAngle16 + 0x4000 & 0xffff;
  FVar10 = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  UVar15 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  dVar5 = FixedMath_Length2(((int)(FVar10 >> 0x20) + armyRuntime->articulatedCoordinateOrState9C) -
                            (modelNode1->worldTransform).translation.y,
                            ((int)FVar10 + armyRuntime->runtimeState94) -
                            (modelNode1->worldTransform).translation.x);
  uVar6 = dVar5 + *(int *)((int)pvVar3 + 0xc0);
  if (uVar6 < (uint)contactDistanceLimitQ12) {
    FVar11 = FixedMath_SinCosScaled(headingAngle16,uVar6);
    iVar7 = (int)FVar11 + (modelNode1->worldTransform).translation.x;
    iVar8 = (int)(FVar11 >> 0x20) + (modelNode1->worldTransform).translation.y;
  }
  else {
    WVar13 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    iVar8 = WVar13.worldYQ12;
    iVar7 = WVar13.worldXQ12;
  }
  dVar5 = iVar7 + (int)FVar10;
  dVar9 = iVar8 + (int)(FVar10 >> 0x20);
  pFVar4 = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = dVar5;
  armyRuntime->runtimeState98 = dVar9;
  FVar10 = FixedMath_SinCosScaled(angle,UVar15);
  if (pFVar4 == (FieldGridAsset *)0x0) {
    return;
  }
  FVar14 = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(FVar10 >> 0x20) + dVar9,(int)FVar10 + dVar5,pFVar4);
  if (!FVar14.carry) {
    armyRuntime->fallbackWorldYQ12 = FVar14.packedNormalAngles;
    armyRuntime->articulatedHeightOrStateA0 = FVar14.heightQ12;
    dVar5 = armyRuntime->runtimeState98;
    iVar7 = armyRuntime->articulatedHeightOrStateA0;
    AVar12 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                       (dVar5,armyRuntime->runtimeState90,(RuntimeCollisionQueryViewF4 *)armyRuntime
                        ,worldRuntime);
    if (!AVar12.carry) {
      pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 & 0xffffff7f;
      goto LAB_00522285;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ArmyRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               (ArmyRuntimeSlot *)AVar12.eax,worldRuntime);
  }
  uVar6 = headingAngle16 + 0x4000 & 0xffff;
  FVar10 = FixedMath_SinCosScaled(uVar6,(armyRuntime->articulatedContact).lateralOffsetQ12);
  UVar15 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  dVar5 = armyRuntime->movementTarget1Q12 + (int)FVar10 * 2;
  dVar9 = armyRuntime->definitionClassValue84 + (int)(FVar10 >> 0x20) * 2;
  pFVar4 = worldRuntime->fieldGrid;
  armyRuntime->runtimeState90 = dVar5;
  armyRuntime->runtimeState98 = dVar9;
  FVar10 = FixedMath_SinCosScaled(uVar6,UVar15);
  FVar14 = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(FVar10 >> 0x20) + dVar9,(int)FVar10 + dVar5,pFVar4);
  if (FVar14.carry) {
    return;
  }
  armyRuntime->articulatedHeightOrStateA0 = FVar14.heightQ12;
  armyRuntime->fallbackWorldYQ12 = FVar14.packedNormalAngles;
  dVar5 = armyRuntime->runtimeState98;
  iVar7 = armyRuntime->articulatedHeightOrStateA0;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 0x10;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x80U) != 0) {
    pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar2 = *pQVar2 & 0xfffffffc;
  }
  pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  *pQVar2 = *pQVar2 | 0x80;
LAB_00522285:
  pvVar3 = armyRuntime->definitionOrAsset;
  dVar5 = FixedMath_Length3(iVar7 - armyRuntime->definitionClassValue88,
                            dVar5 - armyRuntime->definitionClassValue80,
                            armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
  iVar7 = dVar5 + *(int *)((int)pvVar3 + 0xc4) * 2;
  iVar8 = *(int *)((int)pvVar3 + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (iVar7 != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((longlong)(ulonglong)(uint)(iVar8 << 0xd) / (longlong)iVar7);
  }
  return;
}


/* Address: 0x005222F0.
   Ownership: gameplay/army/movement.
   Purpose: Updates the right-side articulated terrain contact, retries from the previous contact on obstruction,
   and derives the Q13 interpolation step from the resolved segment length. Four stack arguments are authoritative
   from RET 0x10; EDX:EAX preserves the resolved world X/Y contact pair.
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FixedMath_Length2 [core/math/fixed],
   FieldGrid_InterpolateTerrainHeightAndNormal [world/terrain/grid],
   ArmyCollision_FindBlockingRuntimeForCurrentUnitCf [gameplay/army/placement], ArmyRuntime_HandleCollisionPartner
   [gameplay/army/runtime], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateRightTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *pGVar1;
  Q12 *pQVar2;
  void *pvVar3;
  FieldGridAsset *pFVar4;
  dword dVar5;
  uint uVar6;
  int iVar7;
  uint angle;
  int iVar8;
  FixedSinCosEdxEax8 FVar9;
  FixedSinCosEdxEax8 FVar10;
  ArmyCollisionFindEaxCf5 AVar11;
  WorldPositionXYEaxEdxCf9 WVar12;
  FieldGridHeightNormalEaxEdxCf9 FVar13;
  UQ12 UVar14;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pvVar3 = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  angle = headingAngle16 - 0x4000 & 0xffff;
  FVar9 = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  UVar14 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  dVar5 = FixedMath_Length2(((int)(FVar9 >> 0x20) + armyRuntime->runtimeState98) -
                            (modelNode1->worldTransform).translation.y,
                            ((int)FVar9 + armyRuntime->runtimeState90) -
                            (modelNode1->worldTransform).translation.x);
  uVar6 = dVar5 + *(int *)((int)pvVar3 + 0xc0);
  if (uVar6 < (uint)contactDistanceLimitQ12) {
    FVar10 = FixedMath_SinCosScaled(headingAngle16,uVar6);
    iVar7 = (int)FVar10 + (modelNode1->worldTransform).translation.x;
    iVar8 = (int)(FVar10 >> 0x20) + (modelNode1->worldTransform).translation.y;
  }
  else {
    WVar12 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    iVar8 = WVar12.worldYQ12;
    iVar7 = WVar12.worldXQ12;
  }
  dVar5 = iVar7 + (int)FVar9;
  iVar8 = iVar8 + (int)(FVar9 >> 0x20);
  pFVar4 = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = dVar5;
  armyRuntime->articulatedCoordinateOrState9C = iVar8;
  FVar9 = FixedMath_SinCosScaled(angle,UVar14);
  if (pFVar4 == (FieldGridAsset *)0x0) {
    return;
  }
  FVar13 = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(FVar9 >> 0x20) + iVar8,(int)FVar9 + dVar5,pFVar4);
  if (!FVar13.carry) {
    armyRuntime->fallbackWorldXQ12 = FVar13.packedNormalAngles;
    armyRuntime->runtimeStateA4 = FVar13.heightQ12;
    iVar7 = armyRuntime->articulatedCoordinateOrState9C;
    dVar5 = armyRuntime->runtimeStateA4;
    AVar11 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                       (iVar7,armyRuntime->runtimeState94,(RuntimeCollisionQueryViewF4 *)armyRuntime
                        ,worldRuntime);
    if (!AVar11.carry) {
      pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 & 0xffffff7f;
      goto LAB_005224e5;
    }
    ArmyRuntime_HandleCollisionPartner
              ((ArmyRuntimeSlot *)armyRuntime,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
               (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
               (ArmyRuntimeSlot *)AVar11.eax,worldRuntime);
  }
  uVar6 = headingAngle16 - 0x4000 & 0xffff;
  FVar9 = FixedMath_SinCosScaled(uVar6,(armyRuntime->articulatedContact).lateralOffsetQ12);
  UVar14 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  dVar5 = armyRuntime->movementTarget0Q12 + (int)FVar9 * 2;
  iVar7 = armyRuntime->definitionClassValue80 + (int)(FVar9 >> 0x20) * 2;
  pFVar4 = worldRuntime->fieldGrid;
  armyRuntime->runtimeState94 = dVar5;
  armyRuntime->articulatedCoordinateOrState9C = iVar7;
  FVar9 = FixedMath_SinCosScaled(uVar6,UVar14);
  FVar13 = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(FVar9 >> 0x20) + iVar7,(int)FVar9 + dVar5,pFVar4);
  if (FVar13.carry) {
    return;
  }
  armyRuntime->runtimeStateA4 = FVar13.heightQ12;
  armyRuntime->fallbackWorldXQ12 = FVar13.packedNormalAngles;
  iVar7 = armyRuntime->articulatedCoordinateOrState9C;
  dVar5 = armyRuntime->runtimeStateA4;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 0x10;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x80U) != 0) {
    pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar2 = *pQVar2 & 0xfffffffc;
  }
  pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
  *pQVar2 = *pQVar2 | 0x80;
LAB_005224e5:
  pvVar3 = armyRuntime->definitionOrAsset;
  dVar5 = FixedMath_Length3(dVar5 - armyRuntime->runtimeState8C,
                            iVar7 - armyRuntime->definitionClassValue84,
                            armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
  iVar7 = dVar5 + *(int *)((int)pvVar3 + 0xc4) * 2;
  iVar8 = *(int *)((int)pvVar3 + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (iVar7 != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((longlong)(ulonglong)(uint)(iVar8 << 0xd) / (longlong)iVar7);
  }
  return;
}


/* Address: 0x005254F0.
   Ownership: gameplay/army/movement.
   Purpose: Updates route-following, turning, collision response, model orientation, and depth-bin state for the
   second verified ground-movement runtime class. Runtime-update partition slots 0-23 receive (worldRuntime,
   armyRuntime).
   Local calls: ArmyRuntime_UpdateMovementAndWaypoints.
   Cross-module calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf [gameplay/army/placement],
   FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], ArmyRuntime_UpdateActivationMetricAndPlayStartSound
   [gameplay/army/runtime], FixedTrig_ProjectPlanarPointRegs [core/math/fixed],
   ArmyCollision_FindBlockingRuntimeForCurrentUnitCf [gameplay/army/placement],
   FixedTransform_ComposeEulerAnglesRegs [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGroundMovementVariantB
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime
          )

{
  ArmyMovementStateFlags *pAVar1;
  ArmyRuntimeSlot *pAVar2;
  AngleTurn32 AVar3;
  int iVar4;
  int iVar5;
  ModelDefinitionGroundMovementSteeringView280 *pMVar6;
  ArmyPlacementContactKindIndex32 AVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  ModelRuntimeNode *modelNode2;
  bool bVar14;
  FixedLengthAngleEaxEdx8 FVar15;
  FixedPlanarPointEdxEax8 FVar16;
  ArmyCollisionFindEaxCf5 AVar17;
  WorldPositionXYEaxEdxCf9 WVar18;
  FixedEulerAnglesEaxEbxEdx12 FVar19;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  dword local_24;
  ModelRuntimeNode *modelNode1;
  
  pAVar2 = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
  modelNode2 = modelRuntime->rootModelNode;
  pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
  *pAVar1 = *pAVar1 | 4;
  if (pAVar2 != (ArmyRuntimeSlot *)0x0) {
    if (((((pAVar2->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC != 0) &&
       (modelRuntime->modelDefinition->placementRadiusOrClearanceDC != 0)) {
      bVar14 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                         (modelRuntime->modelDefinition->placementRadiusOrClearanceDC,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x,pAVar2);
      if (bVar14) goto ArmyGroundMovementVariantB_ContinueAfterLinkedRuntimeValidation;
    }
    (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0;
  }
ArmyGroundMovementVariantB_ContinueAfterLinkedRuntimeValidation:
  AVar3 = (modelNode2->modelPayload).worldRotationAngle2;
  iVar4 = (modelNode2->worldTransform).translation.x;
  iVar5 = (modelNode2->worldTransform).translation.y;
  if (((modelRuntime->classState).classStateEC & 8) == 0) {
    WVar18 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)modelRuntime->ownerArmyRuntime);
    if (WVar18.carry) goto ArmyGroundMovementVariantB_StopMovementAndTurn;
    iVar11 = WVar18.worldYQ12 - (modelNode2->worldTransform).translation.y;
    iVar8 = WVar18.worldXQ12 - (modelNode2->worldTransform).translation.x;
    if ((iVar8 == 0) && (iVar11 == 0)) {
      FVar15 = (FixedLengthAngleEaxEdx8)
               ((ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20);
    }
    else {
      FVar15 = FixedMath_Vector2AngleAndLengthRegs(iVar11,iVar8);
    }
    uVar12 = FVar15.angle;
    pMVar6 = modelRuntime->modelDefinition;
    uVar9 = (modelNode2->modelPayload).worldRotationAngle2;
    uVar10 = (modelRuntime->movementControl).turnVelocityAngle16;
    uVar13 = uVar12 - uVar9 & 0xffff;
    if (uVar13 < 0x8000) {
      if ((int)uVar10 < 0) {
ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (uVar10 < uVar13) {
        uVar9 = uVar9 + uVar10;
        iVar11 = pMVar6->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
        iVar8 = uVar10 + pMVar6->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar11;
        if (iVar8 < iVar11) {
          (modelRuntime->movementControl).turnVelocityAngle16 = iVar8;
        }
      }
      else {
ArmyGroundMovementVariantB_SnapFacingToDesiredHeading:
        (modelRuntime->movementControl).turnVelocityAngle16 = 0;
        uVar9 = uVar12;
      }
    }
    else {
      if (0 < (int)uVar10) goto ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal;
      if (uVar10 + 0x10000 <= uVar13) goto ArmyGroundMovementVariantB_SnapFacingToDesiredHeading;
      uVar9 = uVar9 + uVar10;
      iVar11 = -pMVar6->turnRateLimitAnglePerTick10 * g_InGameSimulationStepTicks;
      iVar8 = uVar10 - pMVar6->turnRateAccelerationAnglePerTick1C * g_InGameSimulationStepTicks;
      (modelRuntime->movementControl).turnVelocityAngle16 = iVar11;
      if (iVar11 < iVar8) {
        (modelRuntime->movementControl).turnVelocityAngle16 = iVar8;
      }
    }
    modelNode2 = modelRuntime->rootModelNode;
    uVar9 = uVar9 & 0xffff;
    if (uVar9 != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar9;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    local_24 = FVar15.length;
    uVar10 = pMVar6->farHeadingErrorLimitAngleC4;
    uVar9 = uVar9 - uVar12 & 0xffff;
    if ((int)local_24 < pMVar6->headingErrorInterpolationDistanceQ12C0) {
      uVar10 = pMVar6->nearHeadingErrorLimitAngleC8 +
               (int)(((longlong)(int)(uVar10 - pMVar6->nearHeadingErrorLimitAngleC8) *
                     (longlong)(int)local_24) /
                    (longlong)pMVar6->headingErrorInterpolationDistanceQ12C0);
    }
    if ((uVar10 < uVar9) && (uVar9 < 0x10000 - uVar10)) {
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound
              (worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
    iVar8 = (modelRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    if (iVar8 < (int)local_24 >> 1) {
      FVar16 = FixedTrig_ProjectPlanarPointRegs
                         (iVar8,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pAVar2 = modelRuntime->ownerArmyRuntime;
      WVar18 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pAVar2);
      FVar16 = WVar18._0_8_;
      pAVar1 = &pAVar2->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    AVar7 = pMVar6->placementContactKindIndex278;
    modelNode2 = modelRuntime->rootModelNode;
    heightOffsetQ12 = pMVar6->placementHeightOffsetQ12;
    worldRuntime_00 = worldRuntime;
    AVar17 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                       ((Q12)(FVar16 >> 0x20),(Q12)FVar16,
                        (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    if (AVar17.carry) {
      modelNode1 = modelRuntime->rootModelNode;
      (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar16._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar16._4_4_ = (modelNode1->worldTransform).translation.y;
      pAVar1 = &modelRuntime->ownerArmyRuntime->movementStateFlags;
      *pAVar1 = *pAVar1 | 0x10;
    }
    pAVar2 = modelRuntime->ownerArmyRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[AVar7])
              (heightOffsetQ12,(Q12)(FVar16 >> 0x20),(Q12)FVar16,modelNode2,worldRuntime_00);
    modelNode2 = modelRuntime->rootModelNode;
    iVar8 = pAVar2->actionVector1Q12 + -1;
    if (iVar8 < 0) goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    iVar8 = iVar8 * pAVar2->actionVector2Q12;
    pAVar2->actionVector1Q12 = pAVar2->actionVector1Q12 + -1;
  }
  else {
ArmyGroundMovementVariantB_StopMovementAndTurn:
    (modelRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (modelRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState:
    pAVar2 = modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    iVar8 = pAVar2->actionVector1Q12 + -1;
    if (iVar8 < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [modelRuntime->modelDefinition->placementContactKindIndex278])
                  (modelRuntime->modelDefinition->placementHeightOffsetQ12,
                   (modelNode2->worldTransform).translation.y,
                   (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      }
      goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    }
    iVar8 = iVar8 * pAVar2->actionVector2Q12;
    pAVar2->actionVector1Q12 = pAVar2->actionVector1Q12 + -1;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks
      [modelRuntime->modelDefinition->placementContactKindIndex278])
              (modelRuntime->modelDefinition->placementHeightOffsetQ12,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,modelNode2,worldRuntime);
  }
  FVar19 = FixedTransform_ComposeEulerAnglesRegs
                     (0,0x4000 - iVar8,
                      (modelRuntime->ownerArmyRuntime->actionVector0Q12 + 0x8000) -
                      (modelNode2->modelPayload).worldRotationAngle2 & 0xffff,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = FVar19.angle0;
  (modelNode2->modelPayload).worldRotationAngle1 = FVar19.angle1;
  (modelNode2->modelPayload).worldRotationAngle2 = FVar19.angle2;
ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms:
  if (((iVar4 != (modelNode2->worldTransform).translation.x) ||
      (iVar5 != (modelNode2->worldTransform).translation.y)) ||
     (AVar3 != (modelNode2->modelPayload).worldRotationAngle2)) {
    pAVar2 = modelRuntime->ownerArmyRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView200 *)modelRuntime);
    pAVar1 = &pAVar2->movementStateFlags;
    *pAVar1 = *pAVar1 & 0xfffffffb;
  }
  pMVar6 = modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_UpdateDepthBinMasks(pMVar6->placementRadiusOrClearanceDC,modelNode2);
  return;
}


/* Address: 0x0051CC60.
   Ownership: gameplay/army/movement.
   Purpose: Clamps an oversized requested displacement to the verified 0x2000 distance and starts a movement route
   toward the resulting point.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_SinCosScaled
   [core/math/fixed], EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartClampedMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  FixedLengthAngleEaxEdx8 FVar4;
  FixedSinCosEdxEax8 FVar5;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar6;
  
  pIVar3 = g_InGameRuntimeRoot;
  if (((movementRuntime->movementStateFlags & 3) == 0) && (movementRuntime->retryCountdown == 0)) {
    FVar4 = FixedMath_Vector2AngleAndLengthRegs
                      (targetWorldY - movementRuntime->movementTargetWorldYQ12,
                       targetWorldX - movementRuntime->movementTargetWorldXQ12);
    if (0x2000 < (int)FVar4.length) {
      FVar5 = FixedMath_SinCosScaled(FVar4.angle,0x2000);
      targetWorldX = (int)FVar5 + movementRuntime->movementTargetWorldXQ12;
      targetWorldY = (int)(FVar5 >> 0x20) + movementRuntime->movementTargetWorldYQ12;
    }
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffba7;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0xa1;
    EVar6 = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,
                       &pIVar3->worldRuntime0A30);
    (movementRuntime->fallbackPosition).worldXQ12 = EVar6.fallbackWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = EVar6.fallbackWorldYQ12;
    movementRuntime->movementWorldXQ12 = EVar6.primaryWorldXQ12;
    movementRuntime->movementWorldYQ12 = EVar6.primaryWorldYQ12;
    GVar1 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    GVar2 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    movementRuntime->retryCountdown = 0x40;
    movementRuntime->lastCheckedWorldXQ12 = GVar1;
    movementRuntime->lastCheckedWorldYQ12 = GVar2;
  }
  return;
}


/* Address: 0x0051CD30.
   Ownership: gameplay/army/movement.
   Purpose: Handles army runtime start direct move command.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartDirectMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar3;
  
  if ((movementRuntime->movementStateFlags & 2) == 0) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x81;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff87;
    EVar3 = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
    (movementRuntime->fallbackPosition).worldXQ12 = EVar3.fallbackWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = EVar3.fallbackWorldYQ12;
    movementRuntime->movementWorldXQ12 = EVar3.primaryWorldXQ12;
    movementRuntime->movementWorldYQ12 = EVar3.primaryWorldYQ12;
    GVar1 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    GVar2 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    movementRuntime->retryCountdown = 0x40;
    movementRuntime->lastCheckedWorldXQ12 = GVar1;
    movementRuntime->lastCheckedWorldYQ12 = GVar2;
  }
  return;
}


/* Address: 0x00521580.
   Ownership: gameplay/army/movement.
   Purpose: Updates one articulated contact child, resolves terrain interaction, and emits the linked effect state.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_PlayPositionedOneShot [audio/spatial/runtime], FieldGrid_GetNearestWaterDelta [world/terrain/grid],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateContactChildAndEffects
          (ModelRuntimeNode *contactChildModel,WorldRuntimeContext *worldRuntime,
          ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *pMVar1;
  uint uVar2;
  DirectSoundVoiceSet **voiceSetRef;
  sdword sVar3;
  EffectDefinition *effectDefinition;
  bool bVar4;
  ModelRuntimeNode *modelNode1;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  uVar2 = *(uint *)(pMVar1->classState).reservedCC_CF;
  if ((((uVar2 != 0) && (uVar2 < worldRuntime->dwordArrayCount)) &&
      (worldRuntime->dwordArray != (dword *)0x0)) &&
     (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar2],
     voiceSetRef != (DirectSoundVoiceSet **)0x0)) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    bVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!bVar4) {
      SpatialSound_PlayPositionedOneShot
                ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                 worldPosition,voiceSetRef);
    }
  }
  modelNode1 = contactChildModel->childNodes[0]->childNodes[0]->childNodes[0];
  sVar3 = FieldGrid_GetNearestWaterDelta
                    ((modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (sVar3 < 1) {
    effectDefinition = (EffectDefinition *)pMVar1->attachments140[1].childLocalRotationAngle1;
  }
  else {
    effectDefinition = (EffectDefinition *)pMVar1->definitionValueB4_58;
  }
  if (effectDefinition != (EffectDefinition *)0x0) {
    EffectRuntimePool_CreateInstanceFromDefinitionCf
              (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,
               (modelNode1->modelPayload).worldRotationAngle2,
               (modelNode1->modelPayload).worldRotationAngle1,
               (modelNode1->modelPayload).worldRotationAngle0,
               (modelNode1->worldTransform).translation.z,(modelNode1->worldTransform).translation.y
               ,(modelNode1->worldTransform).translation.x,effectDefinition,worldRuntime);
  }
  return;
}


/* Address: 0x005217A0.
   Ownership: gameplay/army/movement.
   Purpose: Updates paired articulated subtrees from interpolated contact points, solves their joint angles, and
   rebuilds the root and child transforms for the current suspension state. Two stack arguments are authoritative
   from RET 0x08. The second world-runtime argument remains part of the caller contract even where the
   implementation reuses its stack slot.
   Cross-module calls: FixedMath_WriteDirectionQ28 [core/math/fixed], FixedMath_VectorToAnglesVec3Regs
   [core/math/fixed], ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_SinCosScaled [core/math/fixed],
   FixedMath_VectorToAnglesAndLength3Regs [core/math/fixed].
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSuspensionHierarchy
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  GraphicsWorldCoordinateQ12 *pGVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  ArmyTerrainContactDispatchMode AVar6;
  int iVar7;
  int iVar8;
  void *pvVar9;
  int iVar10;
  int iVar11;
  AngleTurn32 AVar12;
  longlong lVar13;
  dword dVar14;
  dword sideLength0Q12;
  short sVar15;
  int iVar16;
  int iVar17;
  uint uVar18;
  uint uVar19;
  short sVar20;
  uint uVar21;
  uint uVar22;
  int iVar23;
  FixedLengthAngleEaxEdx8 FVar24;
  FixedLengthAngleEaxEdx8 FVar25;
  FixedSinCosEdxEax8 FVar26;
  FixedTriangleJointAnglesEaxEdx8 FVar27;
  FixedLengthAnglesEaxEcxEdx12 FVar28;
  FixedLengthAnglesEaxEcxEdx12 FVar29;
  FixedEulerAnglesEaxEcxEdx12 FVar30;
  FixedMathVectorAnglesRegs8 FVar31;
  FixedMathVectorAnglesRegs8 FVar32;
  UQ12 scale;
  GraphicsWorldCoordinateQ12 GVar33;
  GraphicsWorldCoordinateQ12 GVar34;
  GraphicsWorldCoordinateQ12 GVar35;
  GraphicsWorldCoordinateQ12 GVar36;
  int iVar37;
  GraphicsWorldCoordinateQ12 GVar38;
  int iVar39;
  GraphicsWorldCoordinateQ12 GVar40;
  AngleTurn32 local_50;
  AngleTurn32 local_4c;
  AngleTurn32 local_44;
  AngleTurn32 local_40;
  ArmyArticulatedRuntimeSlotView *pAVar2;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  pAVar2 = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).armyRuntime;
  modelNode2 = modelNodeRuntime->childNodes[0]->childNodes[0]->childNodes[0]->childNodes[0];
  dVar2 = pAVar2->runtimeStateA8;
  lVar13 = (longlong)(int)(pAVar2->runtimeState90 - pAVar2->movementTarget0Q12) *
           (longlong)(int)dVar2;
  dVar14 = pAVar2->runtimeState98;
  (modelNode2->worldTransform).translation.x =
       ((int)((ulonglong)lVar13 >> 0x20) << 0x14 | (uint)lVar13 >> 0xc) + pAVar2->movementTarget0Q12
  ;
  lVar13 = (longlong)(int)(dVar14 - pAVar2->definitionClassValue80) * (longlong)(int)dVar2;
  (modelNode2->worldTransform).translation.y =
       ((int)((ulonglong)lVar13 >> 0x20) << 0x14 | (uint)lVar13 >> 0xc) +
       pAVar2->definitionClassValue80;
  lVar13 = (longlong)
           (int)((((int)((0x1000 - dVar2) * *(int *)((int)pAVar2->definitionOrAsset + 0xc4)) >> 10)
                 + pAVar2->articulatedHeightOrStateA0) - pAVar2->definitionClassValue88) *
           (longlong)(int)dVar2;
  (modelNode2->worldTransform).translation.z =
       ((int)((ulonglong)lVar13 >> 0x20) << 0x14 | (uint)lVar13 >> 0xc) +
       pAVar2->definitionClassValue88;
  iVar3 = (pAVar2->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue;
  iVar4 = (pAVar2->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.signedValue;
  iVar5 = (pAVar2->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.signedValue;
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12,
             pAVar2->fallbackWorldYQ12 >> 0x10,pAVar2->fallbackWorldYQ12 & 0xffff);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorBXQ12,
             (int)pAVar2->ownerValue68 >> 0x10,pAVar2->ownerValue68 & 0xffff);
  iVar16 = 0x1000 - pAVar2->runtimeStateA8;
  g_ArmySuspensionBlendVectorAXQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)(int)dVar2) >> 0x20
             ) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)(int)dVar2) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar16) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar16) >> 0xc);
  g_ArmySuspensionBlendVectorAYQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)(int)dVar2) >> 0x20
             ) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)(int)dVar2) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar16) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar16) >> 0xc);
  g_ArmySuspensionBlendVectorAZQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)(int)dVar2) >> 0x20
             ) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)(int)dVar2) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar16) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar16) >> 0xc);
  FVar31 = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12);
  modelNode1 = modelNodeRuntime->childNodes[1]->childNodes[0]->childNodes[0]->childNodes[0];
  AVar6 = (pAVar2->articulatedContact).terrainContactMode;
  lVar13 = (longlong)(int)(pAVar2->runtimeState94 - pAVar2->movementTarget1Q12) *
           (longlong)(int)AVar6;
  iVar16 = pAVar2->articulatedCoordinateOrState9C;
  (modelNode1->worldTransform).translation.x =
       ((int)((ulonglong)lVar13 >> 0x20) << 0x14 | (uint)lVar13 >> 0xc) + pAVar2->movementTarget1Q12
  ;
  lVar13 = (longlong)(int)(iVar16 - pAVar2->definitionClassValue84) * (longlong)(int)AVar6;
  (modelNode1->worldTransform).translation.y =
       ((int)((ulonglong)lVar13 >> 0x20) << 0x14 | (uint)lVar13 >> 0xc) +
       pAVar2->definitionClassValue84;
  lVar13 = (longlong)
           (int)((((int)((0x1000 - AVar6) * *(int *)((int)pAVar2->definitionOrAsset + 0xc4)) >> 10)
                 + pAVar2->runtimeStateA4) - pAVar2->runtimeState8C) * (longlong)(int)AVar6;
  (modelNode1->worldTransform).translation.z =
       ((int)((ulonglong)lVar13 >> 0x20) << 0x14 | (uint)lVar13 >> 0xc) + pAVar2->runtimeState8C;
  iVar16 = (pAVar2->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue;
  iVar7 = (pAVar2->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  iVar8 = (pAVar2->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12,
             pAVar2->fallbackWorldXQ12 >> 0x10,pAVar2->fallbackWorldXQ12 & 0xffff);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorBXQ12,
             (int)pAVar2->linkedArmyRuntimeOrSavedOffset >> 0x10,
             (uint)pAVar2->linkedArmyRuntimeOrSavedOffset & 0xffff);
  iVar17 = 0x1000 - (pAVar2->articulatedContact).terrainContactMode;
  g_ArmySuspensionBlendVectorAXQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)(int)AVar6) >> 0x20
             ) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)(int)AVar6) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar17) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar17) >> 0xc);
  g_ArmySuspensionBlendVectorAYQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)(int)AVar6) >> 0x20
             ) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)(int)AVar6) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar17) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar17) >> 0xc);
  g_ArmySuspensionBlendVectorAZQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)(int)AVar6) >> 0x20
             ) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)(int)AVar6) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar17) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar17) >> 0xc);
  FVar32 = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12);
  pvVar9 = pAVar2->definitionOrAsset;
  LOCK();
  UNLOCK();
  armySlot1 = (modelNodeRuntime->runtimePayload).armyRuntime;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 =
       (((int)((armySlot1->ownerValue64 - armySlot1->classState60) * 0x10000) >> 0x10) *
        (armySlot1->runtimeStateA8 + (armySlot1->articulatedContact).terrainContactMode) >> 0xc) +
       armySlot1->classState60 & 0xffff;
  iVar17 = (modelNode2->worldTransform).translation.x;
  iVar23 = (modelNode2->worldTransform).translation.y;
  iVar37 = ((modelNode1->modelPayload).modelResource)->placementHeightOffsetQ12;
  iVar39 = (modelNode1->worldTransform).translation.x;
  iVar10 = (modelNode1->worldTransform).translation.y;
  pGVar1 = &(modelNode2->worldTransform).translation.z;
  *pGVar1 = *pGVar1 + iVar37;
  pGVar1 = &(modelNode1->worldTransform).translation.z;
  *pGVar1 = *pGVar1 + iVar37;
  iVar37 = (modelNode2->worldTransform).translation.z;
  iVar11 = (modelNode1->worldTransform).translation.z;
  (modelNodeRuntime->worldTransform).translation.x = iVar17 + iVar39 >> 1;
  (modelNodeRuntime->worldTransform).translation.y = iVar23 + iVar10 >> 1;
  (modelNodeRuntime->worldTransform).translation.z =
       (iVar37 + iVar11 >> 1) + *(int *)((int)pvVar9 + 0x54);
  GVar40 = (modelNode1->worldTransform).translation.z;
  GVar38 = (modelNode1->worldTransform).translation.y;
  GVar36 = (modelNode1->worldTransform).translation.x;
  GVar35 = (modelNode2->worldTransform).translation.z;
  GVar34 = (modelNode2->worldTransform).translation.y;
  GVar33 = (modelNode2->worldTransform).translation.x;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  modelNode2 = modelNodeRuntime->childNodes[0];
  modelNode1 = modelNodeRuntime->childNodes[1];
  FVar24 = FixedMath_Vector2AngleAndLengthRegs
                     (GVar34 - (modelNode2->worldTransform).translation.y,
                      GVar33 - (modelNode2->worldTransform).translation.x);
  dVar14 = FVar24.length;
  FVar25 = FixedMath_Vector2AngleAndLengthRegs
                     (GVar38 - (modelNode1->worldTransform).translation.y,
                      GVar36 - (modelNode1->worldTransform).translation.x);
  uVar22 = FVar25.length;
  AVar12 = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  uVar18 = FVar24.angle - AVar12;
  uVar21 = FVar25.angle - AVar12;
  uVar19 = uVar18 & 0xffff;
  sVar15 = (short)uVar18;
  uVar18 = uVar21 & 0xffff;
  sVar20 = (short)uVar21;
  if ((0x3fff < uVar19) && (uVar19 < 0xc001)) {
    sVar15 = sVar15 + -0x8000;
  }
  if ((0x3fff < uVar18) && (uVar18 < 0xc001)) {
    sVar20 = sVar20 + -0x8000;
  }
  uVar18 = (uint)sVar15;
  iVar17 = (int)sVar20;
  if (uVar22 < 0x140) {
    if (uVar22 < 0x40) {
      iVar17 = 0;
    }
    else {
      iVar17 = (int)(iVar17 * (uVar22 - 0x40)) >> 8;
    }
  }
  if (dVar14 < 0x140) {
    if (dVar14 < 0x40) {
      uVar18 = 0;
    }
    else {
      uVar18 = (int)(uVar18 * (dVar14 - 0x40)) >> 8;
    }
  }
  AVar12 = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  uVar22 = iVar17 + 0x8000U & 0xffff;
  (modelNode2->modelPayload).localRotationAngle2 = uVar18 & 0xffff;
  (modelNode1->modelPayload).localRotationAngle2 = uVar22;
  uVar22 = (AVar12 - 0x4000) + uVar22 & 0xffff;
  scale = (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
          contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  FVar26 = FixedMath_SinCosScaled
                     (AVar12 + 0x4000 + (uVar18 & 0xffff) & 0xffff,
                      (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
                      contactRadiusOrLinkedSlotMask.contactRadiusQ12);
  iVar17 = GVar33 + (int)FVar26;
  iVar23 = GVar34 + (int)(FVar26 >> 0x20);
  FVar26 = FixedMath_SinCosScaled(uVar22,scale);
  iVar37 = GVar36 - (int)FVar26;
  iVar39 = GVar38 - (int)(FVar26 >> 0x20);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode2);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
  modelNode2 = modelNode2->childNodes[0];
  modelNode1 = modelNode1->childNodes[0];
  FVar28 = FixedMath_VectorToAnglesAndLength3Regs
                     (GVar35 - (modelNode2->worldTransform).translation.z,
                      iVar23 - (modelNode2->worldTransform).translation.y,
                      iVar17 - (modelNode2->worldTransform).translation.x);
  uVar22 = FVar28.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
  iVar17 = FVar28.elevationAngle + 0x4000;
  if ((0x3fff < uVar22) && (uVar22 < 0xc001)) {
    iVar17 = -iVar17;
  }
  FVar29 = FixedMath_VectorToAnglesAndLength3Regs
                     (GVar40 - (modelNode1->worldTransform).translation.z,
                      iVar39 - (modelNode1->worldTransform).translation.y,
                      iVar37 - (modelNode1->worldTransform).translation.x);
  uVar22 = FVar29.azimuthAngle - (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
  iVar23 = FVar29.elevationAngle + 0x4000;
  if ((0x3fff < uVar22) && (uVar22 < 0xc001)) {
    iVar23 = -iVar23;
  }
  modelNode2 = modelNode2->childNodes[0];
  dVar14 = FixedMath_LengthVec3
                     ((GraphicsFixedVec3 *)&(modelNode2->modelPayload).localTranslationXQ12);
  sideLength0Q12 =
       FixedMath_LengthVec3
                 ((GraphicsFixedVec3 *)
                  &(modelNode2->childNodes[0]->modelPayload).localTranslationXQ12);
  modelNode2 = modelNodeRuntime->childNodes[0]->childNodes[0];
  modelNode1 = modelNodeRuntime->childNodes[1]->childNodes[0];
  FVar27 = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,dVar14,FVar28.lengthQ12);
  iVar17 = FVar27.jointAngle0 - iVar17;
  if (iVar17 < 0) {
    (modelNode2->modelPayload).localRotationAngle0 = 0x8000;
    (modelNode2->modelPayload).localRotationAngle1 = iVar17 + 0x4000;
  }
  else {
    (modelNode2->modelPayload).localRotationAngle0 = 0;
    (modelNode2->modelPayload).localRotationAngle1 = 0x4000 - iVar17;
  }
  modelNode2 = modelNode2->childNodes[0];
  (modelNode2->modelPayload).localRotationAngle1 = 0x4000 - FVar27.jointAngle1;
  FVar27 = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,dVar14,FVar29.lengthQ12);
  iVar23 = FVar27.jointAngle0 - iVar23;
  if (iVar23 < 0) {
    (modelNode1->modelPayload).localRotationAngle0 = 0;
    (modelNode1->modelPayload).localRotationAngle1 = iVar23 + 0x4000;
  }
  else {
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - iVar23;
  }
  modelNode1 = modelNode1->childNodes[0];
  (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - FVar27.jointAngle1;
  modelNode2 = modelNode2->childNodes[0];
  modelNode1 = modelNode1->childNodes[0];
  (modelNode2->modelPayload).localRotationAngle0 = 0;
  (modelNode2->modelPayload).localRotationAngle1 = 0x4000;
  (modelNode2->modelPayload).localRotationAngle2 = 0;
  (modelNode1->modelPayload).localRotationAngle0 = 0;
  (modelNode1->modelPayload).localRotationAngle1 = 0x4000;
  (modelNode1->modelPayload).localRotationAngle2 = 0;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (modelNode2->modelPayload).worldRotationAngle2,
             (modelNode2->modelPayload).worldRotationAngle1,
             (modelNode2->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB);
  local_40 = FVar31.edx;
  local_44 = FVar31.ecx;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (((iVar3 - iVar4) * 0x10000 >> 0x10) * dVar2 >> 0xc) + iVar5 & 0xffff,local_44,local_40
            );
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA);
  FVar30 = FixedTransform_ExtractEulerAnglesRegs
                     ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch);
  (modelNode2->modelPayload).localRotationAngle0 = FVar30.ecxAngle;
  (modelNode2->modelPayload).localRotationAngle1 = FVar30.edxAngle;
  (modelNode2->modelPayload).localRotationAngle2 = FVar30.eaxAngle;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (modelNode1->modelPayload).worldRotationAngle2,
             (modelNode1->modelPayload).worldRotationAngle1,
             (modelNode1->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB);
  local_4c = FVar32.edx;
  local_50 = FVar32.ecx;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (((iVar16 - iVar7) * 0x10000 >> 0x10) * AVar6 >> 0xc) + iVar8 & 0xffff,local_50,
             local_4c);
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA);
  FVar30 = FixedTransform_ExtractEulerAnglesRegs
                     ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch);
  (modelNode1->modelPayload).localRotationAngle0 = FVar30.ecxAngle;
  (modelNode1->modelPayload).localRotationAngle1 = FVar30.edxAngle;
  (modelNode1->modelPayload).localRotationAngle2 = FVar30.eaxAngle;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  return;
}


/* Address: 0x00522550.
   Ownership: gameplay/army/movement.
   Purpose: Initializes the left terrain-contact sample and its interpolation step from the current articulated
   geometry. Typed parameters: p2 headingAngle16→AngleTurn16Stored32_V304. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FieldGrid_InterpolateTerrainHeightAndNormal
   [world/terrain/grid], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  int iVar2;
  void *pvVar3;
  int iVar4;
  dword dVar5;
  uint angle;
  FixedSinCosEdxEax8 FVar6;
  FieldGridHeightNormalEaxEdxCf9 FVar7;
  
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  angle = headingAngle16 + 0x4000U & 0xffff;
  FVar6 = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState90 = (int)FVar6 * 2 + armyRuntime->runtimeState94;
  armyRuntime->runtimeState98 =
       (int)(FVar6 >> 0x20) * 2 + armyRuntime->articulatedCoordinateOrState9C;
  FVar6 = FixedMath_SinCosScaled
                    (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar7 = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(FVar6 >> 0x20) + armyRuntime->runtimeState98,
                       (int)FVar6 + armyRuntime->runtimeState90,worldRuntime->fieldGrid);
    if (!FVar7.carry) {
      armyRuntime->articulatedHeightOrStateA0 = FVar7.heightQ12;
      armyRuntime->fallbackWorldYQ12 = FVar7.packedNormalAngles;
      pvVar3 = armyRuntime->definitionOrAsset;
      dVar5 = FixedMath_Length3(armyRuntime->articulatedHeightOrStateA0 -
                                armyRuntime->definitionClassValue88,
                                armyRuntime->runtimeState98 - armyRuntime->definitionClassValue80,
                                armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
      iVar2 = dVar5 + *(int *)((int)pvVar3 + 0xc4) * 4;
      iVar4 = *(int *)((int)pvVar3 + 0xc0);
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
      if (iVar2 != 0) {
        (armyRuntime->articulatedContact).fallbackPosition1Q12 =
             (Q12)((longlong)(ulonglong)(uint)(iVar4 << 0xd) / (longlong)iVar2);
        pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar1 = *pQVar1 & 0xffffff7f;
      }
    }
  }
  return;
}


/* Address: 0x00522660.
   Ownership: gameplay/army/movement.
   Purpose: Initializes the right terrain-contact sample and its interpolation step from the current articulated
   geometry. Typed parameters: p2 headingAngle16→AngleTurn16Stored32_V304. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FieldGrid_InterpolateTerrainHeightAndNormal
   [world/terrain/grid], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  int iVar2;
  void *pvVar3;
  int iVar4;
  dword dVar5;
  uint angle;
  FixedSinCosEdxEax8 FVar6;
  FieldGridHeightNormalEaxEdxCf9 FVar7;
  
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  angle = headingAngle16 - 0x4000U & 0xffff;
  FVar6 = FixedMath_SinCosScaled(angle,(armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState94 = (int)FVar6 * 2 + armyRuntime->runtimeState90;
  armyRuntime->articulatedCoordinateOrState9C =
       (int)(FVar6 >> 0x20) * 2 + armyRuntime->runtimeState98;
  FVar6 = FixedMath_SinCosScaled
                    (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar7 = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(FVar6 >> 0x20) + armyRuntime->articulatedCoordinateOrState9C,
                       (int)FVar6 + armyRuntime->runtimeState94,worldRuntime->fieldGrid);
    if (!FVar7.carry) {
      armyRuntime->runtimeStateA4 = FVar7.heightQ12;
      armyRuntime->fallbackWorldXQ12 = FVar7.packedNormalAngles;
      pvVar3 = armyRuntime->definitionOrAsset;
      dVar5 = FixedMath_Length3(armyRuntime->runtimeStateA4 - armyRuntime->runtimeState8C,
                                armyRuntime->articulatedCoordinateOrState9C -
                                armyRuntime->definitionClassValue84,
                                armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
      iVar2 = dVar5 + *(int *)((int)pvVar3 + 0xc4) * 4;
      iVar4 = *(int *)((int)pvVar3 + 0xc0);
      (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
      if (iVar2 != 0) {
        (armyRuntime->articulatedContact).fallbackPosition1Q12 =
             (Q12)((longlong)(ulonglong)(uint)(iVar4 << 0xd) / (longlong)iVar2);
        pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar1 = *pQVar1 & 0xffffff7f;
      }
    }
  }
  return;
}


/* Address: 0x00522770.
   Ownership: gameplay/army/movement.
   Purpose: Selects the left or right articulated contact from the signed steering angle, samples its terrain
   point, and rebuilds the corresponding interpolation state. Three stack arguments are authoritative from RET
   0x0C; EDX:EAX preserves the selected world X/Y contact pair.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed], FieldGrid_InterpolateTerrainHeightAndNormal
   [world/terrain/grid], FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  void *pvVar2;
  dword dVar3;
  longlong lVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  FixedSinCosEdxEax8 FVar11;
  FieldGridHeightNormalEaxEdxCf9 FVar12;
  FieldGridAsset *fieldGrid1;
  ModelRuntimeNode *modelNode1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  if (steeringAngle16 < 0x8001) {
    if (*(uint *)((int)pvVar2 + 200) < steeringAngle16) {
      steeringAngle16 = *(AngleTurn32 *)((int)pvVar2 + 200);
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    uVar8 = armyRuntime->classState60 + 0x8000 & 0xffff;
    lVar4 = (longlong)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    uVar9 = (int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc;
    FVar11 = FixedMath_SinCosScaled(uVar8,uVar9);
    iVar5 = (int)FVar11 + (modelNode1->worldTransform).translation.x;
    iVar10 = (int)(FVar11 >> 0x20) + (modelNode1->worldTransform).translation.y;
    uVar8 = (uVar8 + steeringAngle16) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
         uVar8;
    FVar11 = FixedMath_SinCosScaled(uVar8,uVar9);
    iVar5 = iVar5 + (int)FVar11;
    iVar10 = iVar10 + (int)(FVar11 >> 0x20);
    uVar8 = uVar8 + 0x4000 & 0xffff;
    FVar11 = FixedMath_SinCosScaled(uVar8,(armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState90 = (int)FVar11 + iVar5;
    armyRuntime->runtimeState98 = (int)(FVar11 >> 0x20) + iVar10;
    FVar11 = FixedMath_SinCosScaled
                       (uVar8,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid1 = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = ((int)steeringAngle16 >> 1) + armyRuntime->classState60 & 0xffff;
    if (fieldGrid1 != (FieldGridAsset *)0x0) {
      FVar12 = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(FVar11 >> 0x20) + armyRuntime->runtimeState98,
                          (int)FVar11 + armyRuntime->runtimeState90,fieldGrid1);
      if (!FVar12.carry) {
        armyRuntime->articulatedHeightOrStateA0 = FVar12.heightQ12;
        armyRuntime->fallbackWorldYQ12 = FVar12.packedNormalAngles;
      }
    }
    pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar1 = *pQVar1 & 0xffff;
    dVar6 = armyRuntime->runtimeState90;
    dVar3 = armyRuntime->runtimeState98;
    iVar5 = armyRuntime->articulatedHeightOrStateA0;
    pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar1 = *pQVar1 | steeringAngle16 << 0xf;
    pvVar2 = armyRuntime->definitionOrAsset;
    dVar6 = FixedMath_Length3(iVar5 - armyRuntime->definitionClassValue88,
                              dVar3 - armyRuntime->definitionClassValue80,
                              dVar6 - armyRuntime->movementTarget0Q12);
    iVar5 = dVar6 + *(int *)((int)pvVar2 + 0xc4) * 4;
    iVar10 = *(int *)((int)pvVar2 + 0xc0);
    (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
    if (iVar5 != 0) {
      (armyRuntime->articulatedContact).fallbackPosition1Q12 =
           (Q12)((longlong)(ulonglong)(uint)(iVar10 << 0xd) / (longlong)iVar5);
      return;
    }
  }
  else {
    uVar8 = 0x10000 - *(int *)((int)pvVar2 + 200);
    if (steeringAngle16 < uVar8) {
      steeringAngle16 = uVar8;
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    iVar7 = steeringAngle16 - 0x10000;
    uVar8 = armyRuntime->classState60 + 0x8000 & 0xffff;
    lVar4 = (longlong)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    uVar9 = (int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc;
    FVar11 = FixedMath_SinCosScaled(uVar8,uVar9);
    iVar5 = (int)FVar11 + (modelNode1->worldTransform).translation.x;
    iVar10 = (int)(FVar11 >> 0x20) + (modelNode1->worldTransform).translation.y;
    uVar8 = (uVar8 + iVar7) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
         uVar8;
    FVar11 = FixedMath_SinCosScaled(uVar8,uVar9);
    iVar5 = iVar5 + (int)FVar11;
    iVar10 = iVar10 + (int)(FVar11 >> 0x20);
    uVar8 = uVar8 - 0x4000 & 0xffff;
    FVar11 = FixedMath_SinCosScaled(uVar8,(armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState94 = (int)FVar11 + iVar5;
    armyRuntime->articulatedCoordinateOrState9C = (int)(FVar11 >> 0x20) + iVar10;
    FVar11 = FixedMath_SinCosScaled
                       (uVar8,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid1 = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = (iVar7 >> 1) + armyRuntime->classState60 & 0xffff;
    if (fieldGrid1 != (FieldGridAsset *)0x0) {
      FVar12 = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(FVar11 >> 0x20) + armyRuntime->articulatedCoordinateOrState9C,
                          (int)FVar11 + armyRuntime->runtimeState94,fieldGrid1);
      if (!FVar12.carry) {
        armyRuntime->runtimeStateA4 = FVar12.heightQ12;
        armyRuntime->fallbackWorldXQ12 = FVar12.packedNormalAngles;
      }
    }
    pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar1 = *pQVar1 & 0xffff;
    dVar6 = armyRuntime->runtimeState94;
    iVar5 = armyRuntime->articulatedCoordinateOrState9C;
    dVar3 = armyRuntime->runtimeStateA4;
    pQVar1 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar1 = *pQVar1 | iVar7 * 0x8000;
    pvVar2 = armyRuntime->definitionOrAsset;
    dVar6 = FixedMath_Length3(dVar3 - armyRuntime->runtimeState8C,
                              iVar5 - armyRuntime->definitionClassValue84,
                              dVar6 - armyRuntime->movementTarget1Q12);
    iVar5 = dVar6 + *(int *)((int)pvVar2 + 0xc4) * 4;
    iVar10 = *(int *)((int)pvVar2 + 0xc0);
    (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
    if (iVar5 != 0) {
      (armyRuntime->articulatedContact).fallbackPosition1Q12 =
           (Q12)((longlong)(ulonglong)(uint)(iVar10 << 0xd) / (longlong)iVar5);
    }
  }
  return;
}


/* Address: 0x00523340.
   Ownership: gameplay/army/movement.
   Purpose: Evaluates the current target-following condition and starts, clamps, or resets movement according to
   the selected army runtime movement flags. Typed parameters: p2 targetWorldZQ12→Q12, p3 targetWorldYQ12→Q12, p4
   targetWorldXQ12→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyRuntime_StartMoveCommandWithFallbackWaypoints, ArmyRuntime_StartClampedMoveCommand,
   ArmyRuntime_ResetMovementStatePreserveQueuedTarget, ArmyRuntime_ResetMovementStateFromCurrentPosition.
   Cross-module calls: ArmyWeaponRuntime_TestTargetLineOfFireCf [gameplay/army/combat].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeCommand_UpdateTargetFollowingState
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GameEntityRuntime *movementRuntime;
  GameEntityRuntime *pGVar1;
  bool bVar2;
  
  movementRuntime = armyRuntime->linkedEntityRuntime;
  bVar2 = ArmyWeaponRuntime_TestTargetLineOfFireCf
                    (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,worldRuntime,armyRuntime);
  pGVar1 = (movementRuntime->common).ownership.definitionOrClassRecord;
  if (bVar2) {
    if (((armyRuntime == (pGVar1->classPayload).impactOwnerLinks.primaryImpactArmyRuntime) ||
        ((pGVar1->classPayload).impactOwnerLinks.primaryImpactArmyRuntime == (ArmyRuntimeSlot *)0x0)
        ) && (((movementRuntime->common).commandFlags & 0x20) == 0)) {
      if (((movementRuntime->common).commandTarget.targetFlags & 8) == 0) {
        ArmyRuntime_StartMoveCommandWithFallbackWaypoints
                  (targetWorldYQ12,targetWorldXQ12,(ArmyMovementRuntime *)movementRuntime);
      }
      else {
        ArmyRuntime_StartClampedMoveCommand
                  (targetWorldYQ12,targetWorldXQ12,(ArmyMovementRuntime *)movementRuntime);
      }
    }
    return true;
  }
  if (((movementRuntime->common).commandFlags & 0x20) != 0) {
    if (((movementRuntime->common).commandTarget.targetFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)movementRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)movementRuntime);
    }
  }
  return false;
}


/* Address: 0x0051CA60.
   Ownership: gameplay/army/movement.
   Purpose: Appends a waypoint to the bounded movement queue when the runtime is already moving, otherwise starts
   the variant-A movement command immediately.
   Local calls: ArmyRuntime_QueueOrStartMoveCommandVariantA.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueWaypointOrStartMoveVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  uint queuedWaypointIndex;
  
  if (((movementRuntime->movementStateFlags & 0x80) == 0) &&
     ((movementRuntime->movementStateFlags & 3) != 0)) {
    if ((movementRuntime->movementStateFlags & 8) == 0) {
      movementRuntime->queuedWaypointCount = 0;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 8;
    }
    queuedWaypointIndex = movementRuntime->queuedWaypointCount;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x40;
    if (queuedWaypointIndex < 8) {
      queuedWaypointIndex = queuedWaypointIndex + 1;
    }
    movementRuntime->commandModeFlags = movementRuntime->commandModeFlags & 0xfffffbef;
    movementRuntime->queuedWaypoints[queuedWaypointIndex - 1].worldXQ12 = targetWorldX;
    movementRuntime->queuedWaypoints[queuedWaypointIndex - 1].worldYQ12 = targetWorldY;
    movementRuntime->queuedWaypointCount = queuedWaypointIndex;
  }
  else {
    ArmyRuntime_QueueOrStartMoveCommandVariantA(targetWorldY,targetWorldX,movementRuntime);
  }
  return;
}


/* Address: 0x0051CB90.
   Ownership: gameplay/army/movement.
   Purpose: Starts a movement command while preserving or shifting the verified waypoint history when the runtime
   was already following a queued route.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  int iVar3;
  WorldRuntimeContext *worldRuntime;
  Q12 *fallbackCoordinateRead;
  Q12 *waypointCoordinateWrite;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar4;
  
  if (((movementRuntime->movementStateFlags & 0x202) == 0) && (movementRuntime->retryCountdown == 0)
     ) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffb67;
    if ((movementRuntime->movementStateFlags & 1) != 0) {
      iVar3 = 0x10;
      fallbackCoordinateRead = &(movementRuntime->fallbackPosition).worldXQ12;
      waypointCoordinateWrite = &movementRuntime->queuedWaypoints[0].worldXQ12;
      for (; iVar3 != 0; iVar3 = iVar3 + -1) {
        *waypointCoordinateWrite = *fallbackCoordinateRead;
        fallbackCoordinateRead = fallbackCoordinateRead + 1;
        waypointCoordinateWrite = waypointCoordinateWrite + 1;
      }
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 8;
      if (movementRuntime->queuedWaypointCount < 8) {
        movementRuntime->queuedWaypointCount = movementRuntime->queuedWaypointCount + 1;
      }
    }
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags | 0x61;
    EVar4 = EntityPathing_ResolveDestinationAndRebuildRoutes
                      (targetWorldY,targetWorldX,movementRuntime->entityRuntime,worldRuntime);
    (movementRuntime->fallbackPosition).worldXQ12 = EVar4.fallbackWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = EVar4.fallbackWorldYQ12;
    movementRuntime->movementTargetWorldXQ12 = EVar4.fallbackWorldXQ12;
    movementRuntime->movementTargetWorldYQ12 = EVar4.fallbackWorldYQ12;
    movementRuntime->movementWorldXQ12 = EVar4.primaryWorldXQ12;
    movementRuntime->movementWorldYQ12 = EVar4.primaryWorldYQ12;
    GVar1 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    GVar2 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    movementRuntime->retryCountdown = 0x40;
    movementRuntime->lastCheckedWorldXQ12 = GVar1;
    movementRuntime->lastCheckedWorldYQ12 = GVar2;
  }
  return;
}


/* Address: 0x0051CE30.
   Ownership: gameplay/army/movement.
   Purpose: Resets the movement-state flags and copies the current model position into all verified target fields
   except when the preserved queued-target mode is active.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldXQ12;
  GraphicsWorldCoordinateQ12 currentWorldYQ12;
  
  if ((movementRuntime->movementStateFlags & 8) == 0) {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffffe;
  }
  if ((movementRuntime->movementStateFlags & 0x200) == 0) {
    currentWorldXQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldYQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    (movementRuntime->fallbackPosition).worldXQ12 = currentWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = currentWorldYQ12;
    movementRuntime->movementTargetWorldXQ12 = currentWorldXQ12;
    movementRuntime->movementTargetWorldYQ12 = currentWorldYQ12;
    movementRuntime->movementWorldXQ12 = currentWorldXQ12;
    movementRuntime->movementWorldYQ12 = currentWorldYQ12;
  }
  movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff0f;
  return;
}


/* Address: 0x0051CE90.
   Ownership: gameplay/army/movement.
   Purpose: Resets the movement-state flags and, for the verified queued state, copies the current model position
   into the active and fallback target fields.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 currentWorldXQ12;
  GraphicsWorldCoordinateQ12 currentWorldYQ12;
  
  if ((movementRuntime->movementStateFlags & 8) == 0) {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffffe;
  }
  else {
    currentWorldXQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.x;
    currentWorldYQ12 = (movementRuntime->modelNodeRuntime->worldTransform).translation.y;
    (movementRuntime->fallbackPosition).worldXQ12 = currentWorldXQ12;
    (movementRuntime->fallbackPosition).worldYQ12 = currentWorldYQ12;
    movementRuntime->movementWorldXQ12 = currentWorldXQ12;
    movementRuntime->movementWorldYQ12 = currentWorldYQ12;
  }
  movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff0f;
  return;
}


/* Address: 0x0051CEE0.
   Ownership: gameplay/army/movement.
   Purpose: Advances active movement toward the current target, consumes queued waypoints at arrival, rebuilds
   routes when the model diverges, and clears movement state near the final destination. Two stack arguments are
   authoritative from RET 0x08. EDX:EAX carries the resolved world X/Y pair and CF reports movement resolution
   status.
   Local calls: ArmyRuntime_QueueOrStartMoveCommandVariantB, ArmyRuntime_StartDirectMoveCommand.
   Cross-module calls: EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
WorldPositionXYEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime)

{
  ArmyWaypointCount *pAVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  Q12 *queuedCoordinateRead;
  Q12 *queuedCoordinateWrite;
  bool bVar11;
  WorldPositionXYEaxEdxCf9 WVar12;
  WorldPositionXYEaxEdxCf9 WVar13;
  WorldPositionXYEaxEdxCf9 WVar14;
  WorldPositionXYEaxEdxCf9 WVar15;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar16;
  Q12 queuedWorldYQ12;
  Q12 queuedWorldXQ12;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = movementRuntime->modelNodeRuntime;
  if ((movementRuntime->movementStateFlags & 0x10) == 0) {
    if ((movementRuntime->movementStateFlags & 1) != 0) {
ArmyRuntime_UpdateMovementAndWaypoints_ReturnStoredMovementPositionWithCarryClear:
      uVar3 = movementRuntime->movementWorldXQ12;
      uVar4 = movementRuntime->movementWorldYQ12;
      WVar12.worldYQ12 = uVar4;
      WVar12.worldXQ12 = uVar3;
      WVar12.carry = false;
      return WVar12;
    }
  }
  else {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffffef;
    iVar7 = (movementRuntime->fallbackPosition).worldXQ12 -
            (modelNode1->worldTransform).translation.x;
    iVar9 = (movementRuntime->fallbackPosition).worldYQ12 -
            (modelNode1->worldTransform).translation.y;
    if ((((iVar7 < 0x40) && (iVar9 < 0x40)) && (-0x40 < iVar7)) && (-0x40 < iVar9)) {
      if ((movementRuntime->movementStateFlags & 8) != 0) {
        queuedWorldXQ12 = movementRuntime->queuedWaypoints[0].worldXQ12;
        queuedWorldYQ12 = movementRuntime->queuedWaypoints[0].worldYQ12;
        pAVar1 = &movementRuntime->queuedWaypointCount;
        *pAVar1 = *pAVar1 - 1;
        if (*pAVar1 == 0) {
          movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffff7;
        }
        else {
          iVar7 = 0xe;
          queuedCoordinateRead = &movementRuntime->queuedWaypoints[1].worldXQ12;
          queuedCoordinateWrite = &movementRuntime->queuedWaypoints[0].worldXQ12;
          for (; iVar7 != 0; iVar7 = iVar7 + -1) {
            *queuedCoordinateWrite = *queuedCoordinateRead;
            queuedCoordinateRead = queuedCoordinateRead + 1;
            queuedCoordinateWrite = queuedCoordinateWrite + 1;
          }
        }
        ArmyRuntime_QueueOrStartMoveCommandVariantB(queuedWorldYQ12,queuedWorldXQ12,movementRuntime)
        ;
        WVar13 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,movementRuntime);
        return WVar13;
      }
    }
    else {
      iVar7 = (modelNode1->worldTransform).translation.x;
      iVar9 = (modelNode1->worldTransform).translation.y;
      if (((movementRuntime->retryCountdown != 0) &&
          (iVar7 == movementRuntime->lastCheckedWorldXQ12)) &&
         (iVar9 == movementRuntime->lastCheckedWorldYQ12))
      goto ArmyRuntime_UpdateMovementAndWaypoints_ReturnStoredMovementPositionWithCarryClear;
      movementRuntime->retryCountdown = 0x40;
      movementRuntime->lastCheckedWorldXQ12 = iVar7;
      movementRuntime->lastCheckedWorldYQ12 = iVar9;
      EVar16 = EntityPathing_ResolveDestinationAndRebuildRoutes
                         ((movementRuntime->fallbackPosition).worldYQ12,
                          (movementRuntime->fallbackPosition).worldXQ12,
                          movementRuntime->entityRuntime,worldRuntime);
      if (!EVar16.carry) {
        movementRuntime->movementWorldXQ12 = EVar16.primaryWorldXQ12;
        movementRuntime->movementWorldYQ12 = EVar16.primaryWorldYQ12;
        (movementRuntime->fallbackPosition).worldXQ12 = EVar16.fallbackWorldXQ12;
        (movementRuntime->fallbackPosition).worldYQ12 = EVar16.fallbackWorldYQ12;
        WVar13.carry = false;
        WVar13.worldXQ12 = (int)EVar16._0_8_;
        WVar13.worldYQ12 = (int)(EVar16._0_8_ >> 0x20);
        return WVar13;
      }
    }
  }
  uVar8 = (modelNode1->worldTransform).translation.x - movementRuntime->movementTargetWorldXQ12;
  if ((int)uVar8 < 0) {
    uVar8 = -uVar8;
  }
  uVar10 = (modelNode1->worldTransform).translation.y - movementRuntime->movementTargetWorldYQ12;
  if ((int)uVar10 < 0) {
    uVar10 = -uVar10;
  }
  uVar2 = uVar8;
  if ((uVar8 < 0x1001) && (uVar2 = uVar10, uVar10 < 0x1001)) {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff06;
    uVar5 = (modelNode1->worldTransform).translation.x;
    uVar6 = (modelNode1->worldTransform).translation.y;
    WVar14.worldYQ12 = uVar6;
    WVar14.worldXQ12 = uVar5;
    WVar14.carry = true;
    return WVar14;
  }
  bVar11 = uVar2 < 0x1000;
  ArmyRuntime_StartDirectMoveCommand
            (movementRuntime->movementTargetWorldYQ12,movementRuntime->movementTargetWorldXQ12,
             movementRuntime);
  WVar15.worldYQ12 = uVar10;
  WVar15.worldXQ12 = uVar8;
  WVar15.carry = bVar11;
  return WVar15;
}

