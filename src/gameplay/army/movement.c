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
void ArmyRuntimeClass_UpdateArticulatedMovement
               (WorldRuntimeContext *worldRuntime,ArmyArticulatedRuntimeSlotView *armyRuntime)

{
  GameEntityCommandFlags *pGVar1;
  ArmyMovementStateFlags *pAVar2;
  Q12 *pQVar3;
  ArmyRuntimeSlot *armyRuntime_00;
  int iVar4;
  void *pvVar5;
  ArmyRuntimeCoordinateCommandOrHistoryValue4 AVar6;
  sdword sVar7;
  int iVar8;
  int extraout_EAX;
  dword dVar9;
  uint uVar10;
  undefined4 extraout_EAX_00;
  undefined4 extraout_EAX_01;
  int extraout_EAX_02;
  int extraout_EAX_03;
  int extraout_EAX_04;
  uint uVar11;
  int extraout_ECX;
  int extraout_ECX_00;
  AngleTurn32 extraout_ECX_01;
  AngleTurn32 AVar12;
  undefined4 extraout_ECX_02;
  int iVar13;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  int iVar14;
  bool bVar15;
  WorldPositionXYRegisterPairQ12 WVar16;
  ulonglong uVar17;
  undefined8 uVar18;
  undefined8 uStack_24;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  armyRuntime_00 = armyRuntime->linkedArmyRuntime;
  modelNode1 = armyRuntime->modelNodeRuntime;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 4;
  if ((armyRuntime_00 != (ArmyRuntimeSlot *)0x0) &&
     (((*(int *)((int)armyRuntime_00->definitionOrAsset + 0xdc) == 0 ||
       (bVar15 = false, *(int *)((int)armyRuntime->definitionOrAsset + 0xdc) == 0)) ||
      (ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                 (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0xdc),
                  (modelNode1->worldTransform).translation.y,
                  (modelNode1->worldTransform).translation.x,armyRuntime_00), !bVar15)))) {
    armyRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  AVar12 = (modelNode1->modelPayload).worldRotationAngle2;
  iVar14 = (modelNode1->worldTransform).translation.x;
  iVar4 = (modelNode1->worldTransform).translation.y;
  pvVar5 = armyRuntime->definitionOrAsset;
  sVar7 = FieldGrid_InterpolateWaterDelta
                    ((modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((*(int *)((int)pvVar5 + 0x198) < sVar7) &&
     (iVar8 = sVar7 * *(int *)((int)pvVar5 + 0x194) >> 7, -1 < iVar8)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(iVar8,(ArmyRuntimeSlot *)armyRuntime);
  }
  iVar8 = (armyRuntime->movementControl).movementAdvancePerTickQ12;
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 3U) != 0) {
    if (((int)armyRuntime->runtimeStateA8 < 0x801) &&
       ((int)(armyRuntime->articulatedContact).terrainContactMode < 0x801)) {
      iVar13 = *(int *)((int)pvVar5 + 0x18) + iVar8;
    }
    else {
      iVar13 = -(*(int *)((int)pvVar5 + 0x18) - iVar8);
    }
    (armyRuntime->movementControl).movementAdvancePerTickQ12 = iVar13;
    iVar8 = (int)(iVar8 * (armyRuntime->articulatedContact).fallbackPosition1Q12 *
                 g_InGameSimulationStepTicks) >> 0xc;
    if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 2U) == 0) {
      armyRuntime->runtimeStateA8 = armyRuntime->runtimeStateA8 + iVar8;
      if (0xfff < armyRuntime->runtimeStateA8) {
        pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar3 = *pQVar3 & 0xfffffffe;
        armyRuntime->runtimeStateA8 = 0;
        armyRuntime->movementTarget0Q12 = armyRuntime->runtimeState90;
        armyRuntime->definitionClassValue80 = armyRuntime->runtimeState98;
        armyRuntime->definitionClassValue88 = armyRuntime->articulatedHeightOrStateA0;
        AVar6 = (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue;
        dVar9 = (armyRuntime->linkedChildSpawnParameters).parameter0;
        armyRuntime->classState60 = armyRuntime->ownerValue64;
        armyRuntime->ownerValue68 = armyRuntime->fallbackWorldYQ12;
        (armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory = AVar6;
        (armyRuntime->movementControl).movementAdvancePerTickQ12 = dVar9;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (armyRuntime->modelNodeRuntime->childNodes[0],worldRuntime,
                   (ArmyRuntimeSlot *)armyRuntime);
        iVar8 = extraout_EAX_02;
        iVar13 = extraout_EDX_01;
      }
    }
    else {
      (armyRuntime->articulatedContact).terrainContactMode =
           (armyRuntime->articulatedContact).terrainContactMode + iVar8;
      if (0xfff < (armyRuntime->articulatedContact).terrainContactMode) {
        pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar3 = *pQVar3 & 0xfffffffd;
        (armyRuntime->articulatedContact).terrainContactMode =
             ARMY_TERRAIN_CONTACT_ACQUIRE_OR_INITIALIZE_CONTACT_SLOT;
        armyRuntime->movementTarget1Q12 = armyRuntime->runtimeState94;
        armyRuntime->definitionClassValue84 = armyRuntime->articulatedCoordinateOrState9C;
        armyRuntime->runtimeState8C = armyRuntime->runtimeStateA4;
        AVar6 = (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue;
        dVar9 = (armyRuntime->linkedChildSpawnParameters).parameter0;
        armyRuntime->classState60 = armyRuntime->ownerValue64;
        armyRuntime->linkedArmyRuntimeOrSavedOffset =
             (ArmyRuntimeSlot *)armyRuntime->fallbackWorldXQ12;
        (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory = AVar6;
        (armyRuntime->movementControl).movementAdvancePerTickQ12 = dVar9;
        ArmyArticulatedRuntime_UpdateContactChildAndEffects
                  (armyRuntime->modelNodeRuntime->childNodes[1],worldRuntime,
                   (ArmyRuntimeSlot *)armyRuntime);
        iVar8 = extraout_EAX_03;
        iVar13 = extraout_EDX_02;
      }
    }
    uVar17 = CONCAT44(iVar13,iVar8);
    bVar15 = false;
    if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 3U) != 0)
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    modelNode1 = armyRuntime->modelNodeRuntime;
    WVar16 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    if (bVar15) {
      GameEntityRuntime_ResolveCommandTargetPositionCf(armyRuntime->linkedEntityRuntime);
      if (!bVar15) {
        dVar9 = FixedMath_Atan2Angle16
                          (extraout_ECX_00 - (modelNode1->worldTransform).translation.y,
                           extraout_EAX_04 - (modelNode1->worldTransform).translation.x);
        uVar10 = dVar9 - (modelNode1->modelPayload).worldRotationAngle2;
        uVar11 = uVar10 & 0xffff;
        uVar17 = CONCAT44(extraout_EDX_04,uVar10) & 0xffffffff0000ffff;
        if ((0x800 < uVar11) && (uVar11 < 0xf800))
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
    else {
      dVar9 = FixedMath_Length2((int)(WVar16 >> 0x20) - (modelNode1->worldTransform).translation.y,
                                (int)WVar16 - (modelNode1->worldTransform).translation.x);
      uVar17 = CONCAT44(extraout_EDX_03,dVar9);
      if (0x40 < dVar9) goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
    }
ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint:
    entityRuntime1 = armyRuntime->linkedEntityRuntime;
    pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    bVar15 = false;
    *pQVar3 = *pQVar3 & 0xffffffcf;
    uVar17 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
    if (!bVar15) {
      pAVar2 = &(entityRuntime1->common).commandFlags;
      *pAVar2 = *pAVar2 | 0x10;
    }
    goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
  }
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x10U) == 0) {
    bVar15 = false;
    if ((armyRuntime->runtimeFlags & 8) == 0) {
      WVar16 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
      if (bVar15) {
        GameEntityRuntime_ResolveCommandTargetPositionCf(armyRuntime->linkedEntityRuntime);
        if (bVar15) goto ArmyArticulatedMovement_SharedContinuation;
        dVar9 = FixedMath_Atan2Angle16
                          (extraout_ECX - (modelNode1->worldTransform).translation.y,
                           extraout_EAX - (modelNode1->worldTransform).translation.x);
        uStack_24 = CONCAT44(dVar9,0xffffffff);
      }
      else {
        iVar8 = (int)WVar16 - (modelNode1->worldTransform).translation.x;
        iVar13 = (int)(WVar16 >> 0x20) - (modelNode1->worldTransform).translation.y;
        if ((iVar8 == 0) && (iVar13 == 0)) {
          uStack_24 = (ulonglong)(modelNode1->modelPayload).worldRotationAngle2 << 0x20;
        }
        else {
          uStack_24 = FixedMath_Vector2AngleAndLengthRegs(iVar13,iVar8);
        }
      }
      uVar10 = uStack_24._4_4_ - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
      if (0 < (int)(uint)uStack_24) {
        if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x20U) == 0) {
          if ((uVar10 < 0x2001) || (0xdfff < uVar10)) {
ArmyArticulatedMovement_UpdateSelectedTerrainContact:
            if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 4U) == 0) {
              pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar3 = *pQVar3 & 0xffa0;
              pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar3 = *pQVar3 | 0x25;
              uVar17 = ArmyArticulatedRuntime_UpdateLeftTerrainContact
                                 (uStack_24._4_4_,(uint)uStack_24,armyRuntime,worldRuntime);
            }
            else {
              pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar3 = *pQVar3 & 0xffa0;
              pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
              *pQVar3 = *pQVar3 | 0x2a;
              uVar17 = ArmyArticulatedRuntime_UpdateRightTerrainContact
                                 (uStack_24._4_4_,(uint)uStack_24,armyRuntime,worldRuntime);
            }
            goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
          }
        }
        else if ((uVar10 < 0x201) || (0xfdff < uVar10))
        goto ArmyArticulatedMovement_UpdateSelectedTerrainContact;
      }
      if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x20U) != 0)
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
      if ((uint)uStack_24 < 0x41)
      goto ArmyArticulatedMovement_ClearContactTransitionAndAdvanceWaypoint;
      if ((-1 < (int)(uint)uStack_24) || ((0x800 < uVar10 && (uVar10 < 0xf800)))) {
        uVar17 = ArmyArticulatedRuntime_UpdateSelectedTerrainContact
                           (uVar10,armyRuntime,worldRuntime);
        pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar3 = *pQVar3 & 0xffffffa0;
        if ((uint)uVar17 < 0x8000) {
          pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
          *pQVar3 = *pQVar3 | 0x15;
        }
        else {
          pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
          *pQVar3 = *pQVar3 | 0x1a;
        }
        goto ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms;
      }
    }
ArmyArticulatedMovement_SharedContinuation:
    if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x40U) == 0) {
      dVar9 = FixedMath_Atan2Angle16
                        (armyRuntime->definitionClassValue80 - armyRuntime->definitionClassValue84,
                         armyRuntime->movementTarget0Q12 - armyRuntime->movementTarget1Q12);
      uVar10 = (dVar9 - (modelNode1->modelPayload).worldRotationAngle2) - 0x4000 & 0xffff;
      if ((0xfff < uVar10) && ((uVar10 < 0x7000 || ((0x8fff < uVar10 && (uVar10 < 0xf000))))))
      goto ArmyArticulatedMovement_InitializeSelectedTerrainContact;
    }
    uVar17._0_4_ = (armyRuntime->modelNodeRuntime->worldTransform).translation.x;
    uVar17._4_4_ = (armyRuntime->modelNodeRuntime->worldTransform).translation.y;
    if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) == 0) {
      return;
    }
  }
  else {
ArmyArticulatedMovement_InitializeSelectedTerrainContact:
    uVar10 = ((armyRuntime->articulatedContact).fallbackPosition0Q12 >> 0x10) +
             armyRuntime->ownerValue64 & 0xffff;
    if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 4U) == 0) {
      pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar3 = *pQVar3 & 0xffffffc0;
      pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar3 = *pQVar3 | 0x45;
      ArmyArticulatedRuntime_InitializeLeftTerrainContact(uVar10,armyRuntime,worldRuntime);
      uVar17 = CONCAT44(extraout_EDX,extraout_EAX_00);
    }
    else {
      pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar3 = *pQVar3 & 0xffffffc0;
      pQVar3 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar3 = *pQVar3 | 0x4a;
      ArmyArticulatedRuntime_InitializeRightTerrainContact(uVar10,armyRuntime,worldRuntime);
      uVar17 = CONCAT44(extraout_EDX_00,extraout_EAX_01);
    }
  }
ArmyArticulatedMovement_CommitPositionSuspensionAndTransforms:
  modelNode1 = armyRuntime->modelNodeRuntime;
  (modelNode1->worldTransform).translation.x = (int)uVar17;
  (modelNode1->worldTransform).translation.y = (int)(uVar17 >> 0x20);
  modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(modelNode1,worldRuntime);
  if (((iVar14 != (modelNode1->worldTransform).translation.x) ||
      (iVar4 != (modelNode1->worldTransform).translation.y)) ||
     (AVar12 != (modelNode1->modelPayload).worldRotationAngle2)) {
    entityRuntime1 = armyRuntime->linkedEntityRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
    pGVar1 = &(entityRuntime1->common).commandFlags;
    *pGVar1 = *pGVar1 & 0xfffffffb;
    AVar12 = extraout_ECX_01;
  }
  uVar18 = ModelNodeRuntime_RebuildTransformsFromRoot
                     (AVar12,armyRuntime->definitionOrAsset,modelNode1);
  iVar14 = (int)((ulonglong)uVar18 >> 0x20);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_02,iVar14,*(DepthIntervalRadius32 *)(iVar14 + 0xdc),modelNode1);
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
void ArmyRuntime_ResolveCommandTargetAndRoute
               (GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyRuntimeSlot *extraout_EAX;
  ArmyRuntimeSlot *armySlot1;
  ArmyCommandGeneration extraout_ECX;
  ArmyCommandGeneration extraout_ECX_00;
  ArmyCommandGeneration AVar1;
  ModelRuntimeNode *modelNode1;
  
  AVar1 = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & 0x20) != 0) {
    if ((armyRuntime->commandModeFlags & 8) == 0) {
      targetRuntime =
           (GameEntityRuntime *)
           ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
      AVar1 = extraout_ECX_00;
    }
    else {
      targetRuntime =
           (GameEntityRuntime *)
           ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
      AVar1 = extraout_ECX;
    }
  }
  if (targetRuntime == (GameEntityRuntime *)0x0) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
    armySlot1 = (ArmyRuntimeSlot *)0x0;
  }
  else {
    armyRuntime->commandGeneration = AVar1;
    armyRuntime->commandModeFlags = 1;
    modelNode1 = (targetRuntime->common).ownership.modelNode;
    ArmyRuntime_StartMoveCommandWithFallbackWaypoints
              ((modelNode1->worldTransform).translation.y,(modelNode1->worldTransform).translation.x
               ,(ArmyMovementRuntime *)armyRuntime);
    armySlot1 = extraout_EAX;
  }
  armyRuntime->commandTargetArmyRuntime = armySlot1;
  return;
}

/* Address: 0x00520DF0.
   Ownership: gameplay/army/movement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[18]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntimeClass_UpdateGroundMovementVariantB, ArmyRuntimeClass_UpdateGroundMovementVariantA.
   Cross-module calls: AiUnitBehavior_UpdateSpecialClass12Entity [gameplay/ai/units].
*/
void ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GameEntityRuntime *armyRuntime_00;
  int extraout_EAX;
  int iVar1;
  
  armyRuntime_00 = armyRuntime->linkedEntityRuntime;
  iVar1 = *(int *)((int)armyRuntime->definitionOrAsset + 0x278);
  if (((armyRuntime_00->common).commandFlags & 0x100) != 0) {
    AiUnitBehavior_UpdateSpecialClass12Entity
              (armyRuntime->definitionOrAsset,(ArmyRuntimeSlot *)armyRuntime_00,
               (armyRuntime_00->common).ownership.ownerIndex,worldRuntime);
    iVar1 = extraout_EAX;
  }
  if (iVar1 == 1) {
    ArmyRuntimeClass_UpdateGroundMovementVariantB
              (worldRuntime,(ArmyRuntimeMovementCountdownSlotView *)armyRuntime);
  }
  else {
    ArmyRuntimeClass_UpdateGroundMovementVariantA
              (worldRuntime,(ArmyRuntimeMovementCountdownSlotView *)armyRuntime);
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
void ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  AngleTurn32 *pAVar2;
  void *pvVar3;
  int iVar4;
  undefined4 directionScale;
  FixedMathScale32 directionScale_00;
  InGameSimulationStepBatchTicks IVar5;
  Q12 point0Z;
  uint targetYawAngle16;
  int in_ECX;
  int extraout_ECX;
  Q12 point0Y;
  int extraout_ECX_00;
  int extraout_ECX_01;
  ModelRuntimeNode *modelNodeRuntime;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  ModelRuntimeNode *modelNodeRuntime_00;
  int extraout_ECX_06;
  int in_EDX;
  Q12 point0X;
  int extraout_EDX;
  AngleTurn32 targetPitchAngle16;
  int extraout_EDX_00;
  int extraout_EDX_01;
  void *shotRuntimeState14;
  int extraout_EDX_02;
  undefined1 uVar6;
  bool bVar7;
  ShotAimXZRegisterPairQ12 SVar8;
  WorldPositionXYRegisterPairQ12 WVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  IVar5 = g_InGameSimulationStepTicks;
  if ((armyRuntime->runtimeFlags & 9) == 0) {
    pvVar3 = armyRuntime->definitionOrAsset;
    entityRuntime1 = armyRuntime->linkedEntityRuntime;
    iVar4 = armyRuntime->commandCoordinate2Q12;
    if (armyRuntime->commandCoordinate1Q12 != 0) {
      modelNode2 = armyRuntime->modelNodeRuntime->childNodes[0];
      pQVar1 = &armyRuntime->commandCoordinate1Q12;
      *pQVar1 = *pQVar1 - g_InGameSimulationStepTicks;
      if (*pQVar1 < 0) {
        IVar5 = IVar5 + armyRuntime->commandCoordinate1Q12;
        armyRuntime->commandCoordinate1Q12 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      pAVar2 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar2 = *pAVar2 + IVar5 * *(int *)((int)pvVar3 + 0x34);
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
      pAVar2 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar2 = *pAVar2 & 0xffff;
    }
    IVar5 = g_InGameSimulationStepTicks;
    if (iVar4 != 0) {
      directionScale = *(undefined4 *)((int)pvVar3 + 0x3c);
      modelNode2 = armyRuntime->modelNodeRuntime->childNodes[0];
      pQVar1 = &armyRuntime->commandCoordinate2Q12;
      *pQVar1 = *pQVar1 - g_InGameSimulationStepTicks;
      if (*pQVar1 < 0) {
        IVar5 = IVar5 + armyRuntime->commandCoordinate2Q12;
        armyRuntime->commandCoordinate2Q12 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection
                (modelNode2,directionScale,-IVar5,directionScale,
                 (FixedVectorStateAddress32)modelNode2);
      *(uint *)(extraout_ECX + 0x4c) = *(uint *)(extraout_ECX + 0x4c) | 1;
    }
    uVar6 = false;
    modelNode2 = armyRuntime->modelNodeRuntime;
    SVar8 = ArmyRuntime_ResolveShotAimPointCf
                      ((modelNode2->worldTransform).translation.z,
                       (modelNode2->worldTransform).translation.y,
                       (modelNode2->worldTransform).translation.x,
                       *(ShotDefinition **)((int)pvVar3 + 0x2c),entityRuntime1);
    point0X = (Q12)(SVar8 >> 0x20);
    point0Z = (Q12)SVar8;
    if ((bool)uVar6) {
      WVar9 = ArmyRuntime_UpdateMovementAndWaypoints
                        (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      in_EDX = (int)(WVar9 >> 0x20);
      if (((!(bool)uVar6) || (armyRuntime->movementStateFlags != 0)) ||
         (in_ECX = extraout_ECX_00, (armyRuntime->movementControl).turnVelocityAngle16 != 0)) {
        modelNode2 = armyRuntime->modelNodeRuntime;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,armyRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],armyRuntime,0);
        in_ECX = extraout_ECX_01;
        in_EDX = extraout_EDX;
      }
    }
    else {
      modelNode2 = armyRuntime->modelNodeRuntime;
      modelNode1 = modelNode2->childNodes[0];
      uVar10 = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNode1->worldTransform).translation.z,
                          (modelNode1->worldTransform).translation.y,
                          (modelNode1->worldTransform).translation.x,
                          *(ShotDefinition **)((int)pvVar3 + 0x2c));
      targetYawAngle16 =
           ModelNodeRuntime_ComputeRelativeDirectionAngle
                     (modelNode2,(AngleTurn32)((ulonglong)uVar10 >> 0x20),(AngleTurn32)uVar10);
      ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,armyRuntime,targetYawAngle16);
      if ((bool)uVar6) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,armyRuntime,targetPitchAngle16);
        in_ECX = extraout_ECX_02;
        in_EDX = extraout_EDX_00;
      }
      else {
        uVar11 = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,armyRuntime,targetPitchAngle16);
        in_EDX = (int)((ulonglong)uVar11 >> 0x20);
        in_ECX = extraout_ECX_03;
        if ((int)uVar11 == in_EDX) {
          pvVar3 = armyRuntime->definitionOrAsset;
          bVar7 = false;
          if (armyRuntime->commandCoordinate1Q12 == 0) {
            ArmyRuntimeCommand_UpdateTargetFollowingState
                      (point0X,point0Y,point0Z,worldRuntime,armyRuntime);
            in_ECX = extraout_ECX_04;
            in_EDX = extraout_EDX_01;
            if (!bVar7) {
              iVar4 = *(int *)((int)pvVar3 + 0x38);
              directionScale_00 = *(FixedMathScale32 *)((int)pvVar3 + 0x3c);
              armyRuntime->commandCoordinate1Q12 =
                   armyRuntime->commandCoordinate1Q12 + *(int *)((int)pvVar3 + 0x30);
              armyRuntime->commandCoordinate2Q12 = armyRuntime->commandCoordinate2Q12 + iVar4;
              FixedVector_StepBackwardAlongOwnDirection
                        (*(undefined4 *)(extraout_ECX_04 + 0xcc),iVar4,iVar4,directionScale_00,
                         *(undefined4 *)(extraout_ECX_04 + 0xcc));
              *(uint *)(extraout_ECX_05 + 0x4c) = *(uint *)(extraout_ECX_05 + 0x4c) | 1;
              ArmyRuntime_SetNonzeroActionVector
                        ((AngleTurn32)uVar10,*(Q12 *)((int)pvVar3 + 0x44),
                         *(Q12 *)((int)pvVar3 + 0x40),
                         (ArmyRuntimeSlot *)armyRuntime->linkedEntityRuntime);
              entityRuntime1 = (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity
              ;
              shotRuntimeState14 = (void *)0x0;
              if (entityRuntime1 != (GameEntityRuntime *)0x0) {
                shotRuntimeState14 = (entityRuntime1->common).ownership.definitionOrClassRecord;
              }
              ModelRuntime_EmitProjectilesFromAttachmentPoints
                        ((ShotRuntimeState14)shotRuntimeState14,point0X,point0Y,point0Z,
                         *(ShotDefinition **)((int)pvVar3 + 0x2c),modelNodeRuntime_00,
                         *(MdlSerializedNodeHeader38 **)
                          (*(int *)(*(int *)((int)pvVar3 + 100) + 0x18) + 0x18),worldRuntime);
              in_ECX = extraout_ECX_06;
              in_EDX = extraout_EDX_02;
            }
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,in_EDX,armyRuntime->modelNodeRuntime);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
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
void ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  AngleTurn32 *pAVar2;
  ArmyCommandModeFlags *pAVar3;
  void *pvVar4;
  int iVar5;
  FixedMathScale32 directionScale;
  InGameSimulationStepBatchTicks IVar6;
  Q12 point0Z;
  uint targetYawAngle16;
  int extraout_EAX;
  int in_ECX;
  int extraout_ECX;
  int extraout_ECX_00;
  Q12 point0Y;
  int extraout_ECX_01;
  int extraout_ECX_02;
  ModelRuntimeNode *modelNodeRuntime;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  undefined4 uVar7;
  int extraout_ECX_06;
  ModelRuntimeNode *modelNodeRuntime_00;
  int extraout_ECX_07;
  int in_EDX;
  Q12 point0X;
  int extraout_EDX;
  AngleTurn32 targetPitchAngle16;
  int extraout_EDX_00;
  int extraout_EDX_01;
  void *shotRuntimeState14;
  int extraout_EDX_02;
  undefined1 uVar8;
  bool bVar9;
  ShotAimXZRegisterPairQ12 SVar10;
  WorldPositionXYRegisterPairQ12 WVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  IVar6 = g_InGameSimulationStepTicks;
  if ((armyRuntime->runtimeFlags & 9) == 0) {
    pvVar4 = armyRuntime->definitionOrAsset;
    entityRuntime1 = armyRuntime->linkedEntityRuntime;
    iVar5 = armyRuntime->commandCoordinate2Q12;
    if (armyRuntime->commandCoordinate1Q12 != 0) {
      modelNode2 = armyRuntime->modelNodeRuntime->childNodes[0];
      pQVar1 = &armyRuntime->commandCoordinate1Q12;
      *pQVar1 = *pQVar1 - g_InGameSimulationStepTicks;
      if (*pQVar1 < 0) {
        IVar6 = IVar6 + armyRuntime->commandCoordinate1Q12;
        armyRuntime->commandCoordinate1Q12 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      pAVar2 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar2 = *pAVar2 + IVar6 * *(int *)((int)pvVar4 + 0x34);
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
      pAVar2 = &(modelNode2->modelPayload).localRotationAngle2;
      *pAVar2 = *pAVar2 & 0xffff;
    }
    IVar6 = g_InGameSimulationStepTicks;
    if (iVar5 != 0) {
      uVar7 = *(undefined4 *)((int)pvVar4 + 0x3c);
      modelNode2 = armyRuntime->modelNodeRuntime->childNodes[0];
      pQVar1 = &armyRuntime->commandCoordinate2Q12;
      *pQVar1 = *pQVar1 - g_InGameSimulationStepTicks;
      if (*pQVar1 < 0) {
        IVar6 = IVar6 + armyRuntime->commandCoordinate2Q12;
        armyRuntime->commandCoordinate2Q12 = 0;
      }
      modelNode2 = modelNode2->childNodes[0];
      FixedVector_StepBackwardAlongOwnDirection
                (modelNode2,uVar7,-IVar6,uVar7,(FixedVectorStateAddress32)modelNode2);
      *(uint *)(extraout_ECX + 0x4c) = *(uint *)(extraout_ECX + 0x4c) | 1;
    }
    IVar6 = g_InGameSimulationStepTicks;
    if (armyRuntime->commandModeFlags != 0) {
      uVar7 = *(undefined4 *)((int)pvVar4 + 0x3c);
      modelNode2 = armyRuntime->modelNodeRuntime->childNodes[0];
      pAVar3 = &armyRuntime->commandModeFlags;
      *pAVar3 = *pAVar3 - g_InGameSimulationStepTicks;
      if ((int)*pAVar3 < 0) {
        IVar6 = IVar6 + armyRuntime->commandModeFlags;
        armyRuntime->commandModeFlags = 0;
      }
      modelNode2 = modelNode2->childNodes[1];
      FixedVector_StepBackwardAlongOwnDirection
                (modelNode2,uVar7,-IVar6,uVar7,(FixedVectorStateAddress32)modelNode2);
      *(uint *)(extraout_ECX_00 + 0x4c) = *(uint *)(extraout_ECX_00 + 0x4c) | 1;
    }
    uVar8 = false;
    modelNode2 = armyRuntime->modelNodeRuntime;
    SVar10 = ArmyRuntime_ResolveShotAimPointCf
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,
                        *(ShotDefinition **)((int)pvVar4 + 0x2c),entityRuntime1);
    point0X = (Q12)(SVar10 >> 0x20);
    point0Z = (Q12)SVar10;
    if ((bool)uVar8) {
      WVar11 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      in_EDX = (int)(WVar11 >> 0x20);
      if (((!(bool)uVar8) || (armyRuntime->movementStateFlags != 0)) ||
         (in_ECX = extraout_ECX_01, (armyRuntime->movementControl).turnVelocityAngle16 != 0)) {
        modelNode2 = armyRuntime->modelNodeRuntime;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,armyRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],armyRuntime,0);
        in_ECX = extraout_ECX_02;
        in_EDX = extraout_EDX;
      }
    }
    else {
      modelNode2 = armyRuntime->modelNodeRuntime;
      modelNode1 = modelNode2->childNodes[0];
      uVar12 = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNode1->worldTransform).translation.z,
                          (modelNode1->worldTransform).translation.y,
                          (modelNode1->worldTransform).translation.x,
                          *(ShotDefinition **)((int)pvVar4 + 0x2c));
      targetYawAngle16 =
           ModelNodeRuntime_ComputeRelativeDirectionAngle
                     (modelNode2,(AngleTurn32)((ulonglong)uVar12 >> 0x20),(AngleTurn32)uVar12);
      ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,armyRuntime,targetYawAngle16);
      if ((bool)uVar8) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,armyRuntime,targetPitchAngle16);
        in_ECX = extraout_ECX_03;
        in_EDX = extraout_EDX_00;
      }
      else {
        uVar13 = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,armyRuntime,targetPitchAngle16);
        in_EDX = (int)((ulonglong)uVar13 >> 0x20);
        in_ECX = extraout_ECX_04;
        if ((int)uVar13 == in_EDX) {
          pvVar4 = armyRuntime->definitionOrAsset;
          bVar9 = false;
          if (armyRuntime->commandCoordinate1Q12 == 0) {
            ArmyRuntimeCommand_UpdateTargetFollowingState
                      (point0X,point0Y,point0Z,worldRuntime,armyRuntime);
            in_ECX = extraout_ECX_05;
            in_EDX = extraout_EDX_01;
            if (!bVar9) {
              iVar5 = *(int *)((int)pvVar4 + 0x38);
              directionScale = *(FixedMathScale32 *)((int)pvVar4 + 0x3c);
              armyRuntime->commandCoordinate1Q12 =
                   armyRuntime->commandCoordinate1Q12 + *(int *)((int)pvVar4 + 0x30);
              if ((armyRuntime->commandCoordinate0Q12 & 1U) == 0) {
                armyRuntime->commandModeFlags = armyRuntime->commandModeFlags + iVar5;
                uVar7 = *(undefined4 *)(extraout_ECX_05 + 0xd0);
              }
              else {
                armyRuntime->commandCoordinate2Q12 = armyRuntime->commandCoordinate2Q12 + iVar5;
                uVar7 = *(undefined4 *)(extraout_ECX_05 + 0xcc);
              }
              armyRuntime->commandCoordinate0Q12 = armyRuntime->commandCoordinate0Q12 + 1;
              FixedVector_StepBackwardAlongOwnDirection(uVar7,iVar5,iVar5,directionScale,uVar7);
              *(uint *)(extraout_ECX_06 + 0x4c) = *(uint *)(extraout_ECX_06 + 0x4c) | 1;
              ArmyRuntime_SetNonzeroActionVector
                        ((AngleTurn32)uVar12,*(Q12 *)((int)pvVar4 + 0x44),
                         *(Q12 *)((int)pvVar4 + 0x40),
                         (ArmyRuntimeSlot *)armyRuntime->linkedEntityRuntime);
              entityRuntime1 = (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity
              ;
              shotRuntimeState14 = (void *)0x0;
              if (entityRuntime1 != (GameEntityRuntime *)0x0) {
                shotRuntimeState14 = (entityRuntime1->common).ownership.definitionOrClassRecord;
              }
              ModelRuntime_EmitProjectilesFromAttachmentPoints
                        ((ShotRuntimeState14)shotRuntimeState14,point0X,point0Y,point0Z,
                         *(ShotDefinition **)((int)pvVar4 + 0x2c),modelNodeRuntime_00,
                         *(MdlSerializedNodeHeader38 **)
                          (extraout_EAX + *(int *)(*(int *)((int)pvVar4 + 100) + 0x18) + 0x18),
                         worldRuntime);
              in_ECX = extraout_ECX_07;
              in_EDX = extraout_EDX_02;
            }
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,in_EDX,armyRuntime->modelNodeRuntime);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
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
void ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime)

{
  GameEntityCommandFlags *pGVar1;
  ArmyMovementStateFlags *pAVar2;
  int *piVar3;
  AngleTurn32 AVar4;
  int worldY;
  void *pvVar5;
  ModelResourceHitTestAndRenderView210 *pMVar6;
  GameEntityMovementCountdownRuntime *pGVar7;
  sdword sVar8;
  int iVar9;
  uint uVar10;
  ArmyRuntimeSlot *armySlot1;
  uint uVar11;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  int extraout_EDX;
  int iVar12;
  uint uVar13;
  uint uVar14;
  undefined4 extraout_EDX_00;
  int iVar15;
  ModelRuntimeNode *modelNode1;
  bool bVar16;
  undefined1 uVar17;
  WorldPositionXYRegisterPairQ12 WVar18;
  FixedLengthAngleEdxEax8 FVar19;
  FixedPlanarPointEdxEax8 FVar20;
  FixedSinCosEdxEax8 FVar21;
  undefined8 uVar22;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  int iStack_24;
  ModelRuntimeNode *modelNode2;
  
  armySlot1 = armyRuntime->linkedArmyRuntime;
  modelNode2 = armyRuntime->modelNodeRuntime;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 4;
  if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
    if ((*(int *)((int)armySlot1->definitionOrAsset + 0xdc) != 0) &&
       (bVar16 = false, *(int *)((int)armyRuntime->definitionOrAsset + 0xdc) != 0)) {
      ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0xdc),
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,armySlot1);
      if (bVar16) goto ArmyGroundMovementCollision_ContinueAfterLinkedRuntimeValidation;
    }
    armyRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
ArmyGroundMovementCollision_ContinueAfterLinkedRuntimeValidation:
  AVar4 = (modelNode2->modelPayload).worldRotationAngle2;
  iVar15 = (modelNode2->worldTransform).translation.x;
  worldY = (modelNode2->worldTransform).translation.y;
  sVar8 = FieldGrid_InterpolateWaterDelta(worldY,iVar15,worldRuntime->fieldGrid);
  if ((*(int *)(extraout_EDX + 0x198) < sVar8) &&
     (iVar9 = sVar8 * *(int *)(extraout_EDX + 0x194) >> 7, -1 < iVar9)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(iVar9,(ArmyRuntimeSlot *)armyRuntime);
  }
  bVar16 = false;
  if ((armyRuntime->runtimeFlags & 8) == 0) {
    WVar18 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    if (bVar16) goto ArmyGroundMovementCollision_StopMovementAndTurn;
    iVar12 = (int)(WVar18 >> 0x20) - (modelNode2->worldTransform).translation.y;
    iVar9 = (int)WVar18 - (modelNode2->worldTransform).translation.x;
    if ((iVar9 == 0) && (iVar12 == 0)) {
      FVar19 = (ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20;
    }
    else {
      FVar19 = FixedMath_Vector2AngleAndLengthRegs(iVar12,iVar9);
    }
    uVar13 = (uint)(FVar19 >> 0x20);
    pvVar5 = armyRuntime->definitionOrAsset;
    uVar10 = (modelNode2->modelPayload).worldRotationAngle2;
    uVar11 = (armyRuntime->movementControl).turnVelocityAngle16;
    uVar14 = uVar13 - uVar10 & 0xffff;
    if (uVar14 < 0x8000) {
      if ((int)uVar11 < 0) {
ArmyGroundMovementCollision_ResetTurnVelocityForDirectionReversal:
        (armyRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (uVar11 < uVar14) {
        uVar10 = uVar10 + uVar11;
        iVar12 = *(int *)((int)pvVar5 + 0x10) * g_InGameSimulationStepTicks;
        iVar9 = uVar11 + *(int *)((int)pvVar5 + 0x1c) * g_InGameSimulationStepTicks;
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar12;
        if (iVar9 < iVar12) {
          (armyRuntime->movementControl).turnVelocityAngle16 = iVar9;
        }
      }
      else {
ArmyGroundMovementCollision_SnapFacingToDesiredHeading:
        (armyRuntime->movementControl).turnVelocityAngle16 = 0;
        uVar10 = uVar13;
      }
    }
    else {
      if (0 < (int)uVar11) goto ArmyGroundMovementCollision_ResetTurnVelocityForDirectionReversal;
      if (uVar11 + 0x10000 <= uVar14) goto ArmyGroundMovementCollision_SnapFacingToDesiredHeading;
      uVar10 = uVar10 + uVar11;
      iVar12 = -*(int *)((int)pvVar5 + 0x10) * g_InGameSimulationStepTicks;
      iVar9 = uVar11 - *(int *)((int)pvVar5 + 0x1c) * g_InGameSimulationStepTicks;
      (armyRuntime->movementControl).turnVelocityAngle16 = iVar12;
      if (iVar12 < iVar9) {
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar9;
      }
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    uVar10 = uVar10 & 0xffff;
    if (uVar10 != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar10;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    iStack_24 = (int)FVar19;
    uVar11 = *(uint *)((int)pvVar5 + 0xc4);
    uVar10 = uVar10 - uVar13 & 0xffff;
    if (iStack_24 < *(int *)((int)pvVar5 + 0xc0)) {
      uVar11 = *(int *)((int)pvVar5 + 200) +
               (int)(((longlong)(int)(uVar11 - *(int *)((int)pvVar5 + 200)) * (longlong)iStack_24) /
                    (longlong)*(int *)((int)pvVar5 + 0xc0));
    }
    if ((uVar11 < uVar10) && (uVar10 < 0x10000 - uVar11)) {
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound(worldRuntime,(ArmyRuntimeSlot *)armyRuntime)
    ;
    uVar10 = (armyRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    uVar17 = uVar10 < (uint)(iStack_24 >> 1);
    if ((int)uVar10 < iStack_24 >> 1) {
      FVar20 = FixedTrig_ProjectPlanarPointRegs
                         (uVar10,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pGVar7 = armyRuntime->linkedEntityRuntime;
      FVar20 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pGVar7);
      pAVar2 = &(pGVar7->common).commandFlags;
      uVar17 = 0;
      *pAVar2 = *pAVar2 | 0x10;
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    heightOffsetQ12 = *(Q12 *)((int)pvVar5 + 0x54);
    worldRuntime_00 = worldRuntime;
    armySlot1 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                          ((Q12)(FVar20 >> 0x20),(Q12)FVar20,(ArmyRuntimeSlot *)armyRuntime,
                           worldRuntime);
    iVar9 = extraout_ECX;
    if ((bool)uVar17) {
      modelNode1 = armyRuntime->modelNodeRuntime;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)armyRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,armySlot1,worldRuntime);
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar20._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar20._4_4_ = (modelNode1->worldTransform).translation.y;
      pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
      *pGVar1 = *pGVar1 | 0x10;
      iVar9 = extraout_ECX_00;
    }
    pGVar7 = armyRuntime->linkedEntityRuntime;
    modelNode1 = armyRuntime->modelNodeRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[iVar9])
              (heightOffsetQ12,(Q12)(FVar20 >> 0x20),(Q12)FVar20,modelNode2,worldRuntime_00);
    iVar9 = (pGVar7->common).damageState.countdownOrState + -1;
    if (iVar9 < 0) goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    uVar22 = CONCAT44(0x4000,iVar9 * (pGVar7->common).damageState.remainingIntegrity);
    piVar3 = &(pGVar7->common).damageState.countdownOrState;
    *piVar3 = *piVar3 + -1;
  }
  else {
ArmyGroundMovementCollision_StopMovementAndTurn:
    (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (armyRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementCollision_ProcessStationaryPlacementAndDamageState:
    modelNode1 = armyRuntime->modelNodeRuntime;
    if ((armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState + -1 < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                  (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x,modelNode1,worldRuntime);
      }
      goto ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms;
    }
    piVar3 = &(armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState;
    *piVar3 = *piVar3 + -1;
    uVar22 = (*g_ArmyPlacementContactKindDispatchTable.callbacks
               [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                       (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                        (modelNode1->worldTransform).translation.y,
                        (modelNode1->worldTransform).translation.x,modelNode1,worldRuntime);
  }
  uVar10 = ((armyRuntime->linkedEntityRuntime->common).damageState.reserved00 + 0x8000) -
           (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  uVar22 = FixedTransform_ComposeEulerAnglesRegs
                     (0,(int)((ulonglong)uVar22 >> 0x20) - (int)uVar22,uVar10,
                      (modelNode1->modelPayload).worldRotationAngle2,
                      (modelNode1->modelPayload).worldRotationAngle1,
                      (modelNode1->modelPayload).worldRotationAngle0);
  (modelNode1->modelPayload).worldRotationAngle0 = (AngleTurn32)uVar22;
  (modelNode1->modelPayload).worldRotationAngle1 = uVar10;
  (modelNode1->modelPayload).worldRotationAngle2 = (AngleTurn32)((ulonglong)uVar22 >> 0x20);
ArmyGroundMovementCollision_FinalizeEffectsAnimationAndTransforms:
  pMVar6 = (modelNode1->modelPayload).modelResource;
  uVar11 = AVar4 - 0x4000 & 0xffff;
  FVar21 = FixedMath_SinCosScaled
                     ((modelNode1->modelPayload).worldRotationAngle2 - 0x4000 & 0xffff,
                      pMVar6->localBoundsY0Q12);
  iVar9 = (int)FVar21 + (modelNode1->worldTransform).translation.x;
  iVar12 = (int)(FVar21 >> 0x20) + (modelNode1->worldTransform).translation.y;
  FVar21 = FixedMath_SinCosScaled(uVar11,pMVar6->localBoundsY0Q12);
  FVar19 = FixedMath_Vector2AngleAndLengthRegs
                     (iVar12 - ((int)(FVar21 >> 0x20) + worldY),iVar9 - ((int)FVar21 + iVar15));
  iVar9 = (int)FVar19;
  uVar10 = (int)(FVar19 >> 0x20) - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < uVar10) && (uVar10 < 0xc000)) {
    iVar9 = -iVar9;
  }
  modelNode1->primaryTextureOffsetU =
       modelNode1->primaryTextureOffsetU +
       iVar9 * *(int *)((int)armyRuntime->definitionOrAsset + 0x14);
  FVar21 = FixedMath_SinCosScaled(extraout_ECX_01 ^ 0x8000,pMVar6->localBoundsY0Q12);
  iVar9 = (int)FVar21 + (modelNode1->worldTransform).translation.x;
  iVar12 = (int)(FVar21 >> 0x20) + (modelNode1->worldTransform).translation.y;
  FVar21 = FixedMath_SinCosScaled(uVar11 ^ 0x8000,pMVar6->localBoundsY0Q12);
  FVar19 = FixedMath_Vector2AngleAndLengthRegs
                     (iVar12 - ((int)(FVar21 >> 0x20) + worldY),iVar9 - ((int)FVar21 + iVar15));
  iVar9 = (int)FVar19;
  uVar10 = (int)(FVar19 >> 0x20) - (modelNode1->modelPayload).worldRotationAngle2 & 0xffff;
  if ((0x4000 < uVar10) && (uVar10 < 0xc000)) {
    iVar9 = -iVar9;
  }
  iVar12 = modelNode1->primaryTextureOffsetU;
  iVar9 = iVar9 * *(int *)((int)armyRuntime->definitionOrAsset + 0x14) +
          modelNode1->secondaryTextureOffsetU;
  if (iVar9 < 0x100001) {
    if (iVar9 < -0x100000) {
      iVar9 = iVar9 + 0x100000;
    }
  }
  else {
    iVar9 = iVar9 + -0x100000;
  }
  if (iVar12 < 0x100001) {
    if (iVar12 < -0x100000) {
      iVar12 = iVar12 + 0x100000;
    }
  }
  else {
    iVar12 = iVar12 + -0x100000;
  }
  modelNode1->secondaryTextureOffsetU = iVar9;
  modelNode1->primaryTextureOffsetU = iVar12;
  if (((iVar15 != (modelNode1->worldTransform).translation.x) ||
      (worldY != (modelNode1->worldTransform).translation.y)) ||
     (AVar4 != (modelNode1->modelPayload).worldRotationAngle2)) {
    pGVar7 = armyRuntime->linkedEntityRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
    pGVar1 = &(pGVar7->common).commandFlags;
    *pGVar1 = *pGVar1 & 0xfffffffb;
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  uVar22 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_02,extraout_EDX_00,modelNode1);
  iVar15 = (int)((ulonglong)uVar22 >> 0x20);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_03,iVar15,*(DepthIntervalRadius32 *)(iVar15 + 0xdc),modelNode1);
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
void ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime)

{
  GameEntityCommandFlags *pGVar1;
  AngleTurn32 *pAVar2;
  ArmyMovementStateFlags *pAVar3;
  Q12 *pQVar4;
  int *piVar5;
  AngleTurn32 AVar6;
  int iVar7;
  ModelRuntimeNode *pMVar8;
  AngleTurn32 AVar9;
  int iVar10;
  uint uVar11;
  ArmyRuntimeSlot *armySlot1;
  int extraout_ECX;
  int extraout_ECX_00;
  void *extraout_ECX_01;
  void *pvVar12;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  void *extraout_EDX;
  int iVar13;
  AngleTurn32 angle16;
  uint uVar14;
  undefined4 extraout_EDX_00;
  int iVar15;
  GameEntityMovementCountdownRuntime *pGVar16;
  ModelRuntimeNode *modelNode2;
  bool bVar17;
  undefined1 uVar18;
  undefined8 uVar19;
  WorldPositionXYRegisterPairQ12 WVar20;
  FixedPlanarPointEdxEax8 FVar21;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  undefined8 uStack_24;
  ModelRuntimeNode *modelNode1;
  
  armySlot1 = armyRuntime->linkedArmyRuntime;
  modelNode2 = armyRuntime->modelNodeRuntime;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 4;
  if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
    pvVar12 = armyRuntime->definitionOrAsset;
    if ((*(int *)((int)armySlot1->definitionOrAsset + 0xdc) != 0) &&
       (bVar17 = false, *(int *)((int)pvVar12 + 0xdc) != 0)) {
      ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                (*(Q12 *)((int)pvVar12 + 0xdc),(modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,armySlot1);
      pvVar12 = extraout_EDX;
      if (bVar17) goto ArmyMovementBanking_ContinueAfterLinkedRuntimeValidation;
    }
    uVar11 = modelNode2->childCount;
    armyRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
    if ((2 < uVar11) && (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 4U) != 0)) {
      pQVar4 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar4 = *pQVar4 | 1;
      uVar11 = *(uint *)((int)pvVar12 + 0x274);
      if ((uVar11 != 0) &&
         (((uVar11 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
          (modelNode2 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar11] != 0)))) {
        bVar17 = (ModelRuntimeNode *)0xffffff6b < modelNode2;
        uVar19 = TerrainGrid_TestProjectedCellMaskBits01Cf
                           ((modelNode2->worldTransform).translation.y,
                            (modelNode2->worldTransform).translation.x,worldRuntime);
        if (!bVar17) {
          SpatialSound_PlayPositionedOneShot
                    (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar12 + 0x7c),
                     *(SpatialSoundGainQ15 *)((int)pvVar12 + 0x78),
                     (GraphicsFixedVec3 *)((ulonglong)uVar19 >> 0x20),(DirectSoundVoiceSet **)uVar19
                    );
        }
      }
    }
  }
ArmyMovementBanking_ContinueAfterLinkedRuntimeValidation:
  modelNode2 = armyRuntime->modelNodeRuntime;
  AVar6 = (modelNode2->modelPayload).worldRotationAngle2;
  iVar15 = (modelNode2->worldTransform).translation.x;
  iVar7 = (modelNode2->worldTransform).translation.y;
  if ((armyRuntime->runtimeFlags & 8) == 0) {
    bVar17 = false;
    if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 1U) != 0) {
      pQVar4 = &armyRuntime->fallbackWorldYQ12;
      *pQVar4 = *pQVar4 + -1;
      if (*pQVar4 == 0) {
        pQVar4 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
        *pQVar4 = *pQVar4 & 0xfffffff8;
      }
      modelNode1 = modelNode2->childNodes[1];
      pMVar8 = modelNode2->childNodes[2];
      pAVar2 = &(modelNode2->childNodes[0]->modelPayload).localRotationAngle1;
      *pAVar2 = *pAVar2 + 0x2aa;
      pAVar2 = &(modelNode1->modelPayload).localRotationAngle1;
      *pAVar2 = *pAVar2 + 0x2aa;
      pAVar2 = &(pMVar8->modelPayload).localRotationAngle1;
      bVar17 = 0xfffffd55 < *pAVar2;
      *pAVar2 = *pAVar2 + 0x2aa;
    }
    WVar20 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    armySlot1 = (ArmyRuntimeSlot *)(WVar20 >> 0x20);
    if (bVar17) goto ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState;
    if (((dword)WVar20 != armyRuntime->ownerValue68) &&
       (armySlot1 != armyRuntime->linkedArmyRuntimeOrSavedOffset)) {
      armyRuntime->ownerValue68 = (int)WVar20;
      armyRuntime->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)(int)(WVar20 >> 0x20);
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      (armyRuntime->movementControl).turnVelocityAngle16 = 0;
    }
    iVar10 = (dword)WVar20 - (modelNode2->worldTransform).translation.x;
    iVar13 = (int)armySlot1 - (modelNode2->worldTransform).translation.y;
    if ((iVar10 == 0) && (iVar13 == 0)) {
      uStack_24 = (ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20;
    }
    else {
      uStack_24 = FixedMath_Vector2AngleAndLengthRegs(iVar13,iVar10);
    }
    angle16 = (AngleTurn32)(uStack_24 >> 0x20);
    pvVar12 = armyRuntime->definitionOrAsset;
    uVar11 = (armyRuntime->movementControl).turnVelocityAngle16;
    if ((uVar11 == 0) && ((uint)uStack_24 < *(uint *)((int)pvVar12 + 0xc0))) {
ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity:
      (armyRuntime->movementControl).turnVelocityAngle16 = 0;
      uVar14 = (armyRuntime->modelNodeRuntime->modelPayload).worldRotationAngle2;
    }
    else {
      AVar9 = (modelNode2->modelPayload).worldRotationAngle2;
      uVar14 = angle16 - AVar9 & 0xffff;
      if (uVar14 < 0x8000) {
        if ((int)uVar11 < 0) goto ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity;
        if (uVar11 < uVar14) {
          iVar13 = *(int *)((int)pvVar12 + 0x10) * g_InGameSimulationStepTicks;
          iVar10 = uVar11 + *(int *)((int)pvVar12 + 0x1c) * g_InGameSimulationStepTicks;
          (armyRuntime->movementControl).turnVelocityAngle16 = iVar13;
          uVar14 = AVar9 + uVar11;
          if (iVar10 < iVar13) {
            (armyRuntime->movementControl).turnVelocityAngle16 = iVar10;
          }
        }
        else {
ArmyMovementBanking_SnapFacingToDesiredHeading:
          (armyRuntime->movementControl).turnVelocityAngle16 = 0;
          uVar14 = angle16;
        }
      }
      else {
        if (0 < (int)uVar11) goto ArmyMovementBanking_HoldCurrentHeadingAndResetTurnVelocity;
        if (uVar11 + 0x10000 <= uVar14) goto ArmyMovementBanking_SnapFacingToDesiredHeading;
        uVar14 = AVar9 + uVar11;
        iVar13 = -*(int *)((int)pvVar12 + 0x10) * g_InGameSimulationStepTicks;
        iVar10 = uVar11 - *(int *)((int)pvVar12 + 0x1c) * g_InGameSimulationStepTicks;
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar13;
        if (iVar13 < iVar10) {
          (armyRuntime->movementControl).turnVelocityAngle16 = iVar10;
        }
      }
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    if ((uVar14 & 0xffff) != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar14 & 0xffff;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound(worldRuntime,(ArmyRuntimeSlot *)armyRuntime)
    ;
    uVar14 = (int)(uint)uStack_24 >> 1;
    uVar11 = (armyRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    uVar18 = uVar11 < uVar14;
    if ((int)uVar11 < (int)uVar14) {
      FVar21 = FixedTrig_ProjectPlanarPointRegs
                         (uVar11,angle16,(modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pGVar16 = armyRuntime->linkedEntityRuntime;
      FVar21 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pGVar16);
      pAVar3 = &(pGVar16->common).commandFlags;
      uVar18 = 0;
      *pAVar3 = *pAVar3 | 0x10;
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    heightOffsetQ12 = *(Q12 *)((int)pvVar12 + 0x54);
    worldRuntime_00 = worldRuntime;
    armySlot1 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                          ((Q12)(FVar21 >> 0x20),(Q12)FVar21,(ArmyRuntimeSlot *)armyRuntime,
                           worldRuntime);
    iVar10 = extraout_ECX;
    if ((bool)uVar18) {
      modelNode1 = armyRuntime->modelNodeRuntime;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)armyRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,armySlot1,worldRuntime);
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar21._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar21._4_4_ = (modelNode1->worldTransform).translation.y;
      pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
      *pGVar1 = *pGVar1 | 0x10;
      iVar10 = extraout_ECX_00;
    }
    pGVar16 = armyRuntime->linkedEntityRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[iVar10])
              (heightOffsetQ12,(Q12)(FVar21 >> 0x20),(Q12)FVar21,modelNode2,worldRuntime_00);
    modelNode2 = armyRuntime->modelNodeRuntime;
    iVar10 = (pGVar16->common).damageState.countdownOrState + -1;
    if (iVar10 < 0) goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    uVar19 = CONCAT44(0x4000,iVar10 * (pGVar16->common).damageState.remainingIntegrity);
    piVar5 = &(pGVar16->common).damageState.countdownOrState;
    *piVar5 = *piVar5 + -1;
  }
  else {
ArmyMovementBanking_ProcessStoppedMovementPlacementAndDamageState:
    (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (armyRuntime->movementControl).turnVelocityAngle16 = 0;
    modelNode2 = armyRuntime->modelNodeRuntime;
    if ((armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState + -1 < 0) {
      pGVar16 = armyRuntime->definitionOrAsset;
      (*g_ArmyPlacementContactKindDispatchTable.callbacks
        [*(int *)(pGVar16[1].common.reserved68_9F + 0x10)])
                (*(Q12 *)((pGVar16->common).damageState.reserved0C_2B + 0x14),
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      goto ArmyMovementBanking_FinalizeBankingEffectsAndTransforms;
    }
    piVar5 = &(armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState;
    *piVar5 = *piVar5 + -1;
    uVar19 = (*g_ArmyPlacementContactKindDispatchTable.callbacks
               [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                       (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
  }
  pGVar16 = (GameEntityMovementCountdownRuntime *)
            (((armyRuntime->linkedEntityRuntime->common).damageState.reserved00 + 0x8000) -
             (modelNode2->modelPayload).worldRotationAngle2 & 0xffff);
  uVar19 = FixedTransform_ComposeEulerAnglesRegs
                     (0,(int)((ulonglong)uVar19 >> 0x20) - (int)uVar19,(AngleTurn32)pGVar16,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = (AngleTurn32)uVar19;
  (modelNode2->modelPayload).worldRotationAngle1 = (AngleTurn32)pGVar16;
  (modelNode2->modelPayload).worldRotationAngle2 = (AngleTurn32)((ulonglong)uVar19 >> 0x20);
ArmyMovementBanking_FinalizeBankingEffectsAndTransforms:
  pvVar12 = armyRuntime->definitionOrAsset;
  if (armyRuntime->classState60 != 0x4000) {
    uVar19 = FixedTransform_ComposeEulerAnglesRegs
                       ((modelNode2->modelPayload).worldRotationAngle2,
                        (modelNode2->modelPayload).worldRotationAngle1,
                        (modelNode2->modelPayload).worldRotationAngle0,0,armyRuntime->classState60,
                        armyRuntime->ownerValue64);
    (modelNode2->modelPayload).worldRotationAngle0 = (AngleTurn32)uVar19;
    (modelNode2->modelPayload).worldRotationAngle1 = (AngleTurn32)pGVar16;
    (modelNode2->modelPayload).worldRotationAngle2 = (AngleTurn32)((ulonglong)uVar19 >> 0x20);
    pvVar12 = extraout_ECX_01;
  }
  uVar11 = uStack_24._4_4_ - armyRuntime->ownerValue64 & 0xffff;
  if (((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) ||
     ((int)uStack_24 <= *(int *)((int)pvVar12 + 0xc) * 0x20)) {
    armyRuntime->classState60 = armyRuntime->classState60 + 0x40;
    if (0x4000 < (int)armyRuntime->classState60) {
      armyRuntime->classState60 = 0x4000;
    }
  }
  else {
    if (uVar11 < 0x8001) {
      if (0x400 < uVar11) {
        uVar11 = 0x400;
      }
    }
    else if (uVar11 < 0xfc00) {
      uVar11 = 0xfffffc00;
    }
    armyRuntime->classState60 = armyRuntime->classState60 - 0x100;
    armyRuntime->ownerValue64 = uVar11 + armyRuntime->ownerValue64 & 0xffff;
    if ((int)armyRuntime->classState60 < 0x3800) {
      armyRuntime->classState60 = 0x3800;
    }
  }
  if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 2U) == 0) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  }
  if (((iVar15 != (modelNode2->worldTransform).translation.x) ||
      (iVar7 != (modelNode2->worldTransform).translation.y)) ||
     (AVar6 != (modelNode2->modelPayload).worldRotationAngle2)) {
    pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
    *pGVar1 = *pGVar1 & 0xfffffffb;
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  uVar19 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_02,extraout_EDX_00,modelNode2);
  iVar15 = (int)((ulonglong)uVar19 >> 0x20);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_03,iVar15,*(DepthIntervalRadius32 *)(iVar15 + 0xdc),modelNode2);
  return;
}

/* Address: 0x0051C3E0.
   Ownership: gameplay/army/movement.
   Purpose: Resets movement flags and target state from the current model position, updates the verified command-
   state word, and clears inherited movement flags through the attached model hierarchy.
   Cross-module calls: ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime].
*/
void ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  ArmyCommandGeneration AVar3;
  int iVar4;
  int unaff_EBP;
  int unaff_EDI;
  void *pvVar5;
  bool bVar6;
  int iVar7;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  bVar6 = false;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & 0xffffffc6;
  AVar3 = ArmyRuntime_TestStateField100ZeroCf();
  if ((!bVar6) && ((armyRuntime->commandModeFlags & 3) != 0)) {
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags & 0xfffffffc;
    armyRuntime->commandModeFlags = armyRuntime->commandModeFlags | 4;
    armyRuntime->commandGeneration = AVar3;
    armyRuntime->commandTargetArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
  GVar1 = (modelNode1->worldTransform).translation.x;
  GVar2 = (modelNode1->worldTransform).translation.y;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = GVar1;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = GVar2;
  armyRuntime->movementTarget0Q12 = GVar1;
  armyRuntime->movementTarget1Q12 = GVar2;
  armyRuntime->movementPosition0Q12 = GVar1;
  armyRuntime->movementPosition1Q12 = GVar2;
  pvVar5 = armyRuntime->definitionOrAsset;
  iVar4 = 0;
  if (((*(uint *)((int)pvVar5 + 0xec) & 0x10) != 0) && (*(int *)((int)pvVar5 + 0x3c) != 0)) {
    do {
      *(uint *)((int)pvVar5 + 0xec) = *(uint *)((int)pvVar5 + 0xec) & 0xfffffde7;
      iVar4 = iVar4 + 1;
      iVar7 = unaff_EDI;
      do {
        while (unaff_EBP == 0) {
          iVar4 = iVar4 + -1;
          if (iVar4 == 0) {
            return;
          }
        }
        unaff_EBP = unaff_EBP + -1;
        unaff_EDI = iVar7 + 0x20;
        pvVar5 = *(void **)(iVar7 + 0x140);
        iVar7 = unaff_EDI;
      } while (pvVar5 == (void *)0x0);
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
void ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime)

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
undefined8 __fastcall
ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,
          ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  undefined4 in_EAX;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationRegisterResult EVar3;
  
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
  return CONCAT44(preservedEdxValue,in_EAX);
}

/* Address: 0x0051CDB0.
   Ownership: gameplay/army/movement.
   Purpose: Stores a pending movement target, mirrors it into the verified queue and fallback fields according to
   current flags, and records the current model position.
*/
void ArmyRuntime_SetPendingMoveTarget
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
void ArmyArticulatedRuntime_InitializeTerrainContactGeometry
               (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeCoordinateCommandOrHistoryValue4 AVar1;
  dword dVar2;
  Q12 extraout_ECX;
  Q12 worldXQ12;
  int extraout_ECX_00;
  dword dVar3;
  Q12 extraout_EDX;
  Q12 worldYQ12;
  int iVar4;
  undefined8 uVar5;
  FixedSinCosEdxEax8 FVar6;
  int iVar7;
  int iVar8;
  dword dVar9;
  ArmyArticulatedRuntimeSlotView *pAVar1;
  
  pAVar1 = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).modelRuntime;
  worldXQ12 = (modelNodeRuntime->worldTransform).translation.x;
  worldYQ12 = (modelNodeRuntime->worldTransform).translation.y;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    worldXQ12 = extraout_ECX;
    worldYQ12 = extraout_EDX;
  }
  uVar5 = ModelNodeRuntime_RebuildTransformsFromRoot(worldXQ12,worldYQ12,modelNodeRuntime);
  iVar7 = extraout_ECX_00;
  FVar6 = FixedMath_SinCosScaled
                    ((modelNodeRuntime->modelPayload).worldRotationAngle2 + 0x4000 & 0xffff,
                     (pAVar1->articulatedContact).lateralOffsetQ12);
  iVar8 = (int)((ulonglong)uVar5 >> 0x20);
  dVar9 = (dword)uVar5;
  iVar4 = (int)(FVar6 >> 0x20);
  dVar2 = iVar7 + (int)FVar6;
  dVar3 = iVar8 + iVar4;
  pAVar1->movementTarget0Q12 = dVar2;
  pAVar1->definitionClassValue80 = dVar3;
  pAVar1->definitionClassValue88 = dVar9;
  pAVar1->runtimeState90 = dVar2;
  pAVar1->runtimeState98 = dVar3;
  pAVar1->articulatedHeightOrStateA0 = dVar9;
  dVar2 = iVar7 - (int)FVar6;
  dVar3 = iVar8 - iVar4;
  pAVar1->movementTarget1Q12 = dVar2;
  pAVar1->definitionClassValue84 = dVar3;
  pAVar1->runtimeState8C = dVar9;
  pAVar1->runtimeState94 = dVar2;
  pAVar1->articulatedCoordinateOrState9C = dVar3;
  pAVar1->runtimeStateA4 = dVar9;
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
void ArmyRuntime_QueueOrStartMoveCommandVariantA
               (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationRegisterResult EVar3;
  
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
void ArmyRuntime_QueueOrStartMoveCommandVariantB
               (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationRegisterResult EVar3;
  
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
void ArmyRuntimeClass_UpdateGroundMovementVariantA
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime)

{
  GameEntityCommandFlags *pGVar1;
  ArmyMovementStateFlags *pAVar2;
  int *piVar3;
  AngleTurn32 AVar4;
  int worldY;
  void *pvVar5;
  GameEntityMovementCountdownRuntime *pGVar6;
  sdword sVar7;
  int iVar8;
  uint uVar9;
  ArmyRuntimeSlot *armySlot1;
  uint uVar10;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  int extraout_EDX;
  int iVar11;
  uint uVar12;
  uint uVar13;
  undefined4 extraout_EDX_00;
  int iVar14;
  ModelRuntimeNode *modelNode2;
  bool bVar15;
  undefined1 uVar16;
  WorldPositionXYRegisterPairQ12 WVar17;
  FixedLengthAngleEdxEax8 FVar18;
  FixedPlanarPointEdxEax8 FVar19;
  undefined8 uVar20;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  int local_24;
  ModelRuntimeNode *modelNode1;
  
  armySlot1 = armyRuntime->linkedArmyRuntime;
  modelNode2 = armyRuntime->modelNodeRuntime;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 4;
  if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
    if ((*(int *)((int)armySlot1->definitionOrAsset + 0xdc) != 0) &&
       (bVar15 = false, *(int *)((int)armyRuntime->definitionOrAsset + 0xdc) != 0)) {
      ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0xdc),
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,armySlot1);
      if (bVar15) goto ArmyGroundMovementVariantA_ContinueAfterLinkedRuntimeValidation;
    }
    armyRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
ArmyGroundMovementVariantA_ContinueAfterLinkedRuntimeValidation:
  AVar4 = (modelNode2->modelPayload).worldRotationAngle2;
  iVar14 = (modelNode2->worldTransform).translation.x;
  worldY = (modelNode2->worldTransform).translation.y;
  sVar7 = FieldGrid_InterpolateWaterDelta(worldY,iVar14,worldRuntime->fieldGrid);
  if ((*(int *)(extraout_EDX + 0x198) < sVar7) &&
     (iVar8 = sVar7 * *(int *)(extraout_EDX + 0x194) >> 7, -1 < iVar8)) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(iVar8,(ArmyRuntimeSlot *)armyRuntime);
  }
  bVar15 = false;
  if ((armyRuntime->runtimeFlags & 8) == 0) {
    WVar17 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    if (bVar15) goto ArmyGroundMovementVariantA_StopMovementAndTurn;
    iVar11 = (int)(WVar17 >> 0x20) - (modelNode2->worldTransform).translation.y;
    iVar8 = (int)WVar17 - (modelNode2->worldTransform).translation.x;
    if ((iVar8 == 0) && (iVar11 == 0)) {
      FVar18 = (ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20;
    }
    else {
      FVar18 = FixedMath_Vector2AngleAndLengthRegs(iVar11,iVar8);
    }
    uVar12 = (uint)(FVar18 >> 0x20);
    pvVar5 = armyRuntime->definitionOrAsset;
    uVar9 = (modelNode2->modelPayload).worldRotationAngle2;
    uVar10 = (armyRuntime->movementControl).turnVelocityAngle16;
    uVar13 = uVar12 - uVar9 & 0xffff;
    if (uVar13 < 0x8000) {
      if ((int)uVar10 < 0) {
ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal:
        (armyRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (uVar10 < uVar13) {
        uVar9 = uVar9 + uVar10;
        iVar11 = *(int *)((int)pvVar5 + 0x10) * g_InGameSimulationStepTicks;
        iVar8 = uVar10 + *(int *)((int)pvVar5 + 0x1c) * g_InGameSimulationStepTicks;
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar11;
        if (iVar8 < iVar11) {
          (armyRuntime->movementControl).turnVelocityAngle16 = iVar8;
        }
      }
      else {
ArmyGroundMovementVariantA_SnapFacingToDesiredHeading:
        (armyRuntime->movementControl).turnVelocityAngle16 = 0;
        uVar9 = uVar12;
      }
    }
    else {
      if (0 < (int)uVar10) goto ArmyGroundMovementVariantA_ResetTurnVelocityForDirectionReversal;
      if (uVar10 + 0x10000 <= uVar13) goto ArmyGroundMovementVariantA_SnapFacingToDesiredHeading;
      uVar9 = uVar9 + uVar10;
      iVar11 = -*(int *)((int)pvVar5 + 0x10) * g_InGameSimulationStepTicks;
      iVar8 = uVar10 - *(int *)((int)pvVar5 + 0x1c) * g_InGameSimulationStepTicks;
      (armyRuntime->movementControl).turnVelocityAngle16 = iVar11;
      if (iVar11 < iVar8) {
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar8;
      }
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    uVar9 = uVar9 & 0xffff;
    if (uVar9 != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar9;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    local_24 = (int)FVar18;
    uVar10 = *(uint *)((int)pvVar5 + 0xc4);
    uVar9 = uVar9 - uVar12 & 0xffff;
    if (local_24 < *(int *)((int)pvVar5 + 0xc0)) {
      uVar10 = *(int *)((int)pvVar5 + 200) +
               (int)(((longlong)(int)(uVar10 - *(int *)((int)pvVar5 + 200)) * (longlong)local_24) /
                    (longlong)*(int *)((int)pvVar5 + 0xc0));
    }
    if ((uVar10 < uVar9) && (uVar9 < 0x10000 - uVar10)) {
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound(worldRuntime,(ArmyRuntimeSlot *)armyRuntime)
    ;
    uVar9 = (armyRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    uVar16 = uVar9 < (uint)(local_24 >> 1);
    if ((int)uVar9 < local_24 >> 1) {
      FVar19 = FixedTrig_ProjectPlanarPointRegs
                         (uVar9,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pGVar6 = armyRuntime->linkedEntityRuntime;
      FVar19 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pGVar6);
      pAVar2 = &(pGVar6->common).commandFlags;
      uVar16 = 0;
      *pAVar2 = *pAVar2 | 0x10;
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    heightOffsetQ12 = *(Q12 *)((int)pvVar5 + 0x54);
    worldRuntime_00 = worldRuntime;
    armySlot1 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                          ((Q12)(FVar19 >> 0x20),(Q12)FVar19,(ArmyRuntimeSlot *)armyRuntime,
                           worldRuntime);
    iVar8 = extraout_ECX;
    if ((bool)uVar16) {
      modelNode1 = armyRuntime->modelNodeRuntime;
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)armyRuntime,(modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,armySlot1,worldRuntime);
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar19._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar19._4_4_ = (modelNode1->worldTransform).translation.y;
      pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
      *pGVar1 = *pGVar1 | 0x10;
      iVar8 = extraout_ECX_00;
    }
    pGVar6 = armyRuntime->linkedEntityRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[iVar8])
              (heightOffsetQ12,(Q12)(FVar19 >> 0x20),(Q12)FVar19,modelNode2,worldRuntime_00);
    modelNode2 = armyRuntime->modelNodeRuntime;
    iVar8 = (pGVar6->common).damageState.countdownOrState + -1;
    if (iVar8 < 0) goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    uVar20 = CONCAT44(0x4000,iVar8 * (pGVar6->common).damageState.remainingIntegrity);
    piVar3 = &(pGVar6->common).damageState.countdownOrState;
    *piVar3 = *piVar3 + -1;
  }
  else {
ArmyGroundMovementVariantA_StopMovementAndTurn:
    (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (armyRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantA_ProcessStationaryPlacementAndDamageState:
    modelNode2 = armyRuntime->modelNodeRuntime;
    if ((armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState + -1 < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                  (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                   (modelNode2->worldTransform).translation.y,
                   (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      }
      goto ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms;
    }
    piVar3 = &(armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState;
    *piVar3 = *piVar3 + -1;
    uVar20 = (*g_ArmyPlacementContactKindDispatchTable.callbacks
               [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                       (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
  }
  uVar9 = ((armyRuntime->linkedEntityRuntime->common).damageState.reserved00 + 0x8000) -
          (modelNode2->modelPayload).worldRotationAngle2 & 0xffff;
  uVar20 = FixedTransform_ComposeEulerAnglesRegs
                     (0,(int)((ulonglong)uVar20 >> 0x20) - (int)uVar20,uVar9,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = (AngleTurn32)uVar20;
  (modelNode2->modelPayload).worldRotationAngle1 = uVar9;
  (modelNode2->modelPayload).worldRotationAngle2 = (AngleTurn32)((ulonglong)uVar20 >> 0x20);
ArmyGroundMovementVariantA_FinalizeEffectsAnimationAndTransforms:
  if (((iVar14 != (modelNode2->worldTransform).translation.x) ||
      (worldY != (modelNode2->worldTransform).translation.y)) ||
     (AVar4 != (modelNode2->modelPayload).worldRotationAngle2)) {
    pGVar6 = armyRuntime->linkedEntityRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
    pGVar1 = &(pGVar6->common).commandFlags;
    *pGVar1 = *pGVar1 & 0xfffffffb;
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  uVar20 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_01,extraout_EDX_00,modelNode2);
  iVar14 = (int)((ulonglong)uVar20 >> 0x20);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_02,iVar14,*(DepthIntervalRadius32 *)(iVar14 + 0xdc),modelNode2);
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
WorldPositionXYRegisterPairQ12
ArmyArticulatedRuntime_UpdateLeftTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *pGVar1;
  Q12 *pQVar2;
  void *pvVar3;
  int iVar4;
  undefined4 in_EAX;
  dword dVar5;
  uint scale;
  ArmyRuntimeSlot *collisionPartnerArmyRuntime;
  AngleTurn32 extraout_ECX;
  FieldGridAsset *field;
  dword extraout_ECX_00;
  AngleTurn32 angle;
  FieldGridAsset *field_00;
  undefined4 in_EDX;
  dword dVar6;
  int extraout_EDX;
  int iVar7;
  bool bVar8;
  FixedSinCosEdxEax8 FVar9;
  FixedSinCosEdxEax8 FVar10;
  WorldPositionXYRegisterPairQ12 WVar11;
  qword qVar12;
  AngleTurn32 angle_00;
  UQ12 UVar13;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pvVar3 = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  FVar9 = FixedMath_SinCosScaled
                    (headingAngle16 + 0x4000 & 0xffff,
                     (armyRuntime->articulatedContact).lateralOffsetQ12);
  UVar13 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  angle_00 = extraout_ECX;
  dVar5 = FixedMath_Length2(((int)(FVar9 >> 0x20) + armyRuntime->articulatedCoordinateOrState9C) -
                            (modelNode1->worldTransform).translation.y,
                            ((int)FVar9 + armyRuntime->runtimeState94) -
                            (modelNode1->worldTransform).translation.x);
  scale = dVar5 + *(int *)((int)pvVar3 + 0xc0);
  if (scale < (uint)contactDistanceLimitQ12) {
    FVar10 = FixedMath_SinCosScaled(headingAngle16,scale);
    WVar11 = CONCAT44((int)(FVar10 >> 0x20) + (modelNode1->worldTransform).translation.y,
                      (int)FVar10 + (modelNode1->worldTransform).translation.x);
  }
  else {
    WVar11 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
  }
  dVar5 = (int)WVar11 + (int)FVar9;
  dVar6 = (int)(WVar11 >> 0x20) + (int)(FVar9 >> 0x20);
  armyRuntime->runtimeState90 = dVar5;
  armyRuntime->runtimeState98 = dVar6;
  FVar9 = FixedMath_SinCosScaled(angle_00,UVar13);
  bVar8 = false;
  if (field == (FieldGridAsset *)0x0) goto ArmyLeftTerrainContact_ReturnAfterUpdate;
  qVar12 = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(FVar9 >> 0x20) + dVar6,(int)FVar9 + dVar5,field);
  if (bVar8) {
ArmyLeftTerrainContact_RecomputeUsingOppositeContactAnchor:
    FVar9 = FixedMath_SinCosScaled
                      (headingAngle16 + 0x4000 & 0xffff,
                       (armyRuntime->articulatedContact).lateralOffsetQ12);
    UVar13 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
    dVar5 = armyRuntime->movementTarget1Q12 + (int)FVar9 * 2;
    dVar6 = armyRuntime->definitionClassValue84 + (int)(FVar9 >> 0x20) * 2;
    armyRuntime->runtimeState90 = dVar5;
    armyRuntime->runtimeState98 = dVar6;
    FVar9 = FixedMath_SinCosScaled(angle,UVar13);
    bVar8 = (undefined1 *)0xfffffff7 < &stack0xffffffdc;
    qVar12 = FieldGrid_InterpolateTerrainHeightAndNormal
                       ((int)(FVar9 >> 0x20) + dVar6,(int)FVar9 + dVar5,field_00);
    if (bVar8) goto ArmyLeftTerrainContact_ReturnAfterUpdate;
    armyRuntime->articulatedHeightOrStateA0 = (int)qVar12;
    armyRuntime->fallbackWorldYQ12 = (Q12)(qVar12 >> 0x20);
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
  }
  else {
    armyRuntime->fallbackWorldYQ12 = (Q12)(qVar12 >> 0x20);
    armyRuntime->articulatedHeightOrStateA0 = (int)qVar12;
    collisionPartnerArmyRuntime =
         ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                   (armyRuntime->runtimeState98,armyRuntime->runtimeState90,
                    (ArmyRuntimeSlot *)armyRuntime,worldRuntime);
    if (bVar8) {
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)armyRuntime,
                 (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                 (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                 collisionPartnerArmyRuntime,worldRuntime);
      goto ArmyLeftTerrainContact_RecomputeUsingOppositeContactAnchor;
    }
    pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar2 = *pQVar2 & 0xffffff7f;
    dVar5 = extraout_ECX_00;
    iVar7 = extraout_EDX;
  }
  pvVar3 = armyRuntime->definitionOrAsset;
  dVar5 = FixedMath_Length3(iVar7 - armyRuntime->definitionClassValue88,
                            dVar5 - armyRuntime->definitionClassValue80,
                            armyRuntime->runtimeState90 - armyRuntime->movementTarget0Q12);
  iVar7 = dVar5 + *(int *)((int)pvVar3 + 0xc4) * 2;
  iVar4 = *(int *)((int)pvVar3 + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (iVar7 != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((longlong)(ulonglong)(uint)(iVar4 << 0xd) / (longlong)iVar7);
  }
ArmyLeftTerrainContact_ReturnAfterUpdate:
  return CONCAT44(in_EDX,in_EAX);
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
WorldPositionXYRegisterPairQ12
ArmyArticulatedRuntime_UpdateRightTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  GameEntityCommandFlags *pGVar1;
  Q12 *pQVar2;
  void *pvVar3;
  int iVar4;
  undefined4 in_EAX;
  dword dVar5;
  uint scale;
  ArmyRuntimeSlot *collisionPartnerArmyRuntime;
  AngleTurn32 extraout_ECX;
  FieldGridAsset *field;
  int extraout_ECX_00;
  AngleTurn32 angle;
  FieldGridAsset *field_00;
  undefined4 in_EDX;
  int iVar6;
  dword extraout_EDX;
  bool bVar7;
  FixedSinCosEdxEax8 FVar8;
  FixedSinCosEdxEax8 FVar9;
  WorldPositionXYRegisterPairQ12 WVar10;
  qword qVar11;
  AngleTurn32 angle_00;
  UQ12 UVar12;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pvVar3 = armyRuntime->definitionOrAsset;
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
       headingAngle16;
  FVar8 = FixedMath_SinCosScaled
                    (headingAngle16 - 0x4000 & 0xffff,
                     (armyRuntime->articulatedContact).lateralOffsetQ12);
  UVar12 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  angle_00 = extraout_ECX;
  dVar5 = FixedMath_Length2(((int)(FVar8 >> 0x20) + armyRuntime->runtimeState98) -
                            (modelNode1->worldTransform).translation.y,
                            ((int)FVar8 + armyRuntime->runtimeState90) -
                            (modelNode1->worldTransform).translation.x);
  scale = dVar5 + *(int *)((int)pvVar3 + 0xc0);
  if (scale < (uint)contactDistanceLimitQ12) {
    FVar9 = FixedMath_SinCosScaled(headingAngle16,scale);
    WVar10 = CONCAT44((int)(FVar9 >> 0x20) + (modelNode1->worldTransform).translation.y,
                      (int)FVar9 + (modelNode1->worldTransform).translation.x);
  }
  else {
    WVar10 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
  }
  dVar5 = (int)WVar10 + (int)FVar8;
  iVar6 = (int)(WVar10 >> 0x20) + (int)(FVar8 >> 0x20);
  armyRuntime->runtimeState94 = dVar5;
  armyRuntime->articulatedCoordinateOrState9C = iVar6;
  FVar8 = FixedMath_SinCosScaled(angle_00,UVar12);
  bVar7 = false;
  if (field == (FieldGridAsset *)0x0) goto ArmyRightTerrainContact_ReturnAfterUpdate;
  qVar11 = FieldGrid_InterpolateTerrainHeightAndNormal
                     ((int)(FVar8 >> 0x20) + iVar6,(int)FVar8 + dVar5,field);
  if (bVar7) {
ArmyRightTerrainContact_RecomputeUsingOppositeContactAnchor:
    FVar8 = FixedMath_SinCosScaled
                      (headingAngle16 - 0x4000 & 0xffff,
                       (armyRuntime->articulatedContact).lateralOffsetQ12);
    UVar12 = (armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.contactRadiusQ12;
    dVar5 = armyRuntime->movementTarget0Q12 + (int)FVar8 * 2;
    iVar6 = armyRuntime->definitionClassValue80 + (int)(FVar8 >> 0x20) * 2;
    armyRuntime->runtimeState94 = dVar5;
    armyRuntime->articulatedCoordinateOrState9C = iVar6;
    FVar8 = FixedMath_SinCosScaled(angle,UVar12);
    bVar7 = (undefined1 *)0xfffffff7 < &stack0xffffffdc;
    qVar11 = FieldGrid_InterpolateTerrainHeightAndNormal
                       ((int)(FVar8 >> 0x20) + iVar6,(int)FVar8 + dVar5,field_00);
    if (bVar7) goto ArmyRightTerrainContact_ReturnAfterUpdate;
    armyRuntime->runtimeStateA4 = (dword)qVar11;
    armyRuntime->fallbackWorldXQ12 = (Q12)(qVar11 >> 0x20);
    iVar6 = armyRuntime->articulatedCoordinateOrState9C;
    dVar5 = armyRuntime->runtimeStateA4;
    pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
    *pGVar1 = *pGVar1 | 0x10;
    if (((armyRuntime->articulatedContact).fallbackPosition0Q12 & 0x80U) != 0) {
      pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
      *pQVar2 = *pQVar2 & 0xfffffffc;
    }
    pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar2 = *pQVar2 | 0x80;
  }
  else {
    armyRuntime->fallbackWorldXQ12 = (Q12)(qVar11 >> 0x20);
    armyRuntime->runtimeStateA4 = (dword)qVar11;
    collisionPartnerArmyRuntime =
         ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                   (armyRuntime->articulatedCoordinateOrState9C,armyRuntime->runtimeState94,
                    (ArmyRuntimeSlot *)armyRuntime,worldRuntime);
    if (bVar7) {
      ArmyRuntime_HandleCollisionPartner
                ((ArmyRuntimeSlot *)armyRuntime,
                 (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                 (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                 collisionPartnerArmyRuntime,worldRuntime);
      goto ArmyRightTerrainContact_RecomputeUsingOppositeContactAnchor;
    }
    pQVar2 = &(armyRuntime->articulatedContact).fallbackPosition0Q12;
    *pQVar2 = *pQVar2 & 0xffffff7f;
    iVar6 = extraout_ECX_00;
    dVar5 = extraout_EDX;
  }
  pvVar3 = armyRuntime->definitionOrAsset;
  dVar5 = FixedMath_Length3(dVar5 - armyRuntime->runtimeState8C,
                            iVar6 - armyRuntime->definitionClassValue84,
                            armyRuntime->runtimeState94 - armyRuntime->movementTarget1Q12);
  iVar6 = dVar5 + *(int *)((int)pvVar3 + 0xc4) * 2;
  iVar4 = *(int *)((int)pvVar3 + 0xc0);
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = 0x2000;
  if (iVar6 != 0) {
    (armyRuntime->articulatedContact).fallbackPosition1Q12 =
         (Q12)((longlong)(ulonglong)(uint)(iVar4 << 0xd) / (longlong)iVar6);
  }
ArmyRightTerrainContact_ReturnAfterUpdate:
  return CONCAT44(in_EDX,in_EAX);
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
void ArmyRuntimeClass_UpdateGroundMovementVariantB
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime)

{
  GameEntityCommandFlags *pGVar1;
  ArmyMovementStateFlags *pAVar2;
  int *piVar3;
  ArmyRuntimeSlot *armyRuntime_00;
  AngleTurn32 AVar4;
  int iVar5;
  void *pvVar6;
  GameEntityMovementCountdownRuntime *pGVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int iVar11;
  uint uVar12;
  uint uVar13;
  undefined4 extraout_EDX;
  int iVar14;
  ModelRuntimeNode *modelNode2;
  bool bVar15;
  undefined1 uVar16;
  WorldPositionXYRegisterPairQ12 WVar17;
  FixedLengthAngleEdxEax8 FVar18;
  FixedPlanarPointEdxEax8 FVar19;
  undefined8 uVar20;
  Q12 heightOffsetQ12;
  WorldRuntimeContext *worldRuntime_00;
  int local_24;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime_00 = armyRuntime->linkedArmyRuntime;
  modelNode2 = armyRuntime->modelNodeRuntime;
  pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
  *pGVar1 = *pGVar1 | 4;
  if (armyRuntime_00 != (ArmyRuntimeSlot *)0x0) {
    if ((*(int *)((int)armyRuntime_00->definitionOrAsset + 0xdc) != 0) &&
       (bVar15 = false, *(int *)((int)armyRuntime->definitionOrAsset + 0xdc) != 0)) {
      ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0xdc),
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,armyRuntime_00);
      if (bVar15) goto ArmyGroundMovementVariantB_ContinueAfterLinkedRuntimeValidation;
    }
    armyRuntime->linkedArmyRuntime = (ArmyRuntimeSlot *)0x0;
  }
ArmyGroundMovementVariantB_ContinueAfterLinkedRuntimeValidation:
  AVar4 = (modelNode2->modelPayload).worldRotationAngle2;
  iVar14 = (modelNode2->worldTransform).translation.x;
  iVar5 = (modelNode2->worldTransform).translation.y;
  bVar15 = false;
  if ((armyRuntime->runtimeFlags & 8) == 0) {
    WVar17 = ArmyRuntime_UpdateMovementAndWaypoints
                       (worldRuntime,(ArmyMovementRuntime *)armyRuntime->linkedEntityRuntime);
    if (bVar15) goto ArmyGroundMovementVariantB_StopMovementAndTurn;
    iVar11 = (int)(WVar17 >> 0x20) - (modelNode2->worldTransform).translation.y;
    iVar8 = (int)WVar17 - (modelNode2->worldTransform).translation.x;
    if ((iVar8 == 0) && (iVar11 == 0)) {
      FVar18 = (ulonglong)(modelNode2->modelPayload).worldRotationAngle2 << 0x20;
    }
    else {
      FVar18 = FixedMath_Vector2AngleAndLengthRegs(iVar11,iVar8);
    }
    uVar12 = (uint)(FVar18 >> 0x20);
    pvVar6 = armyRuntime->definitionOrAsset;
    uVar9 = (modelNode2->modelPayload).worldRotationAngle2;
    uVar10 = (armyRuntime->movementControl).turnVelocityAngle16;
    uVar13 = uVar12 - uVar9 & 0xffff;
    if (uVar13 < 0x8000) {
      if ((int)uVar10 < 0) {
ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal:
        (armyRuntime->movementControl).turnVelocityAngle16 = 0;
      }
      else if (uVar10 < uVar13) {
        uVar9 = uVar9 + uVar10;
        iVar11 = *(int *)((int)pvVar6 + 0x10) * g_InGameSimulationStepTicks;
        iVar8 = uVar10 + *(int *)((int)pvVar6 + 0x1c) * g_InGameSimulationStepTicks;
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar11;
        if (iVar8 < iVar11) {
          (armyRuntime->movementControl).turnVelocityAngle16 = iVar8;
        }
      }
      else {
ArmyGroundMovementVariantB_SnapFacingToDesiredHeading:
        (armyRuntime->movementControl).turnVelocityAngle16 = 0;
        uVar9 = uVar12;
      }
    }
    else {
      if (0 < (int)uVar10) goto ArmyGroundMovementVariantB_ResetTurnVelocityForDirectionReversal;
      if (uVar10 + 0x10000 <= uVar13) goto ArmyGroundMovementVariantB_SnapFacingToDesiredHeading;
      uVar9 = uVar9 + uVar10;
      iVar11 = -*(int *)((int)pvVar6 + 0x10) * g_InGameSimulationStepTicks;
      iVar8 = uVar10 - *(int *)((int)pvVar6 + 0x1c) * g_InGameSimulationStepTicks;
      (armyRuntime->movementControl).turnVelocityAngle16 = iVar11;
      if (iVar11 < iVar8) {
        (armyRuntime->movementControl).turnVelocityAngle16 = iVar8;
      }
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    uVar9 = uVar9 & 0xffff;
    if (uVar9 != (modelNode2->modelPayload).worldRotationAngle2) {
      (modelNode2->modelPayload).worldRotationAngle2 = uVar9;
      modelNode2->runtimeFlags = modelNode2->runtimeFlags | 1;
    }
    local_24 = (int)FVar18;
    uVar10 = *(uint *)((int)pvVar6 + 0xc4);
    uVar9 = uVar9 - uVar12 & 0xffff;
    if (local_24 < *(int *)((int)pvVar6 + 0xc0)) {
      uVar10 = *(int *)((int)pvVar6 + 200) +
               (int)(((longlong)(int)(uVar10 - *(int *)((int)pvVar6 + 200)) * (longlong)local_24) /
                    (longlong)*(int *)((int)pvVar6 + 0xc0));
    }
    if ((uVar10 < uVar9) && (uVar9 < 0x10000 - uVar10)) {
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      goto ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState;
    }
    ArmyRuntime_UpdateActivationMetricAndPlayStartSound(worldRuntime,(ArmyRuntimeSlot *)armyRuntime)
    ;
    uVar9 = (armyRuntime->movementControl).movementAdvancePerTickQ12 * g_InGameSimulationStepTicks;
    uVar16 = uVar9 < (uint)(local_24 >> 1);
    if ((int)uVar9 < local_24 >> 1) {
      FVar19 = FixedTrig_ProjectPlanarPointRegs
                         (uVar9,(modelNode2->modelPayload).worldRotationAngle2,
                          (modelNode2->worldTransform).translation.y,
                          (modelNode2->worldTransform).translation.x);
    }
    else {
      pGVar7 = armyRuntime->linkedEntityRuntime;
      FVar19 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,(ArmyMovementRuntime *)pGVar7);
      pAVar2 = &(pGVar7->common).commandFlags;
      uVar16 = 0;
      *pAVar2 = *pAVar2 | 0x10;
    }
    modelNode2 = armyRuntime->modelNodeRuntime;
    heightOffsetQ12 = *(Q12 *)((int)pvVar6 + 0x54);
    worldRuntime_00 = worldRuntime;
    ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
              ((Q12)(FVar19 >> 0x20),(Q12)FVar19,(ArmyRuntimeSlot *)armyRuntime,worldRuntime);
    if ((bool)uVar16) {
      modelNode1 = armyRuntime->modelNodeRuntime;
      (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
      FVar19._0_4_ = (modelNode1->worldTransform).translation.x;
      FVar19._4_4_ = (modelNode1->worldTransform).translation.y;
      pGVar1 = &(armyRuntime->linkedEntityRuntime->common).commandFlags;
      *pGVar1 = *pGVar1 | 0x10;
    }
    pGVar7 = armyRuntime->linkedEntityRuntime;
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[extraout_ECX])
              (heightOffsetQ12,(Q12)(FVar19 >> 0x20),(Q12)FVar19,modelNode2,worldRuntime_00);
    modelNode2 = armyRuntime->modelNodeRuntime;
    iVar8 = (pGVar7->common).damageState.countdownOrState + -1;
    if (iVar8 < 0) goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    uVar20 = CONCAT44(0x4000,iVar8 * (pGVar7->common).damageState.remainingIntegrity);
    piVar3 = &(pGVar7->common).damageState.countdownOrState;
    *piVar3 = *piVar3 + -1;
  }
  else {
ArmyGroundMovementVariantB_StopMovementAndTurn:
    (armyRuntime->movementControl).movementAdvancePerTickQ12 = 0;
    (armyRuntime->movementControl).turnVelocityAngle16 = 0;
ArmyGroundMovementVariantB_ProcessStationaryPlacementAndDamageState:
    modelNode2 = armyRuntime->modelNodeRuntime;
    if ((armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState + -1 < 0) {
      if ((worldRuntime->fieldGrid->runtimeStateFlags & 1) != 0) {
        (*g_ArmyPlacementContactKindDispatchTable.callbacks
          [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                  (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                   (modelNode2->worldTransform).translation.y,
                   (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
      }
      goto ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms;
    }
    piVar3 = &(armyRuntime->linkedEntityRuntime->common).damageState.countdownOrState;
    *piVar3 = *piVar3 + -1;
    uVar20 = (*g_ArmyPlacementContactKindDispatchTable.callbacks
               [*(int *)((int)armyRuntime->definitionOrAsset + 0x278)])
                       (*(Q12 *)((int)armyRuntime->definitionOrAsset + 0x54),
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,modelNode2,worldRuntime);
  }
  uVar9 = ((armyRuntime->linkedEntityRuntime->common).damageState.reserved00 + 0x8000) -
          (modelNode2->modelPayload).worldRotationAngle2 & 0xffff;
  uVar20 = FixedTransform_ComposeEulerAnglesRegs
                     (0,(int)((ulonglong)uVar20 >> 0x20) - (int)uVar20,uVar9,
                      (modelNode2->modelPayload).worldRotationAngle2,
                      (modelNode2->modelPayload).worldRotationAngle1,
                      (modelNode2->modelPayload).worldRotationAngle0);
  (modelNode2->modelPayload).worldRotationAngle0 = (AngleTurn32)uVar20;
  (modelNode2->modelPayload).worldRotationAngle1 = uVar9;
  (modelNode2->modelPayload).worldRotationAngle2 = (AngleTurn32)((ulonglong)uVar20 >> 0x20);
ArmyGroundMovementVariantB_FinalizeEffectsAnimationAndTransforms:
  if (((iVar14 != (modelNode2->worldTransform).translation.x) ||
      (iVar5 != (modelNode2->worldTransform).translation.y)) ||
     (AVar4 != (modelNode2->modelPayload).worldRotationAngle2)) {
    pGVar7 = armyRuntime->linkedEntityRuntime;
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
    pGVar1 = &(pGVar7->common).commandFlags;
    *pGVar1 = *pGVar1 & 0xfffffffb;
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ArmyRuntimeSlot *)armyRuntime);
  uVar20 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_00,extraout_EDX,modelNode2);
  iVar14 = (int)((ulonglong)uVar20 >> 0x20);
  ModelNodeRuntime_UpdateDepthBinMasks
            (extraout_ECX_01,iVar14,*(DepthIntervalRadius32 *)(iVar14 + 0xdc),modelNode2);
  return;
}

/* Address: 0x0051CC60.
   Ownership: gameplay/army/movement.
   Purpose: Clamps an oversized requested displacement to the verified 0x2000 distance and starts a movement route
   toward the resulting point.
   Cross-module calls: FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_SinCosScaled
   [core/math/fixed], EntityPathing_ResolveDestinationAndRebuildRoutes [world/pathing/grid].
*/
void ArmyRuntime_StartClampedMoveCommand
               (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  FixedLengthAngleEdxEax8 FVar4;
  FixedSinCosEdxEax8 FVar5;
  EntityPathingDestinationRegisterResult EVar6;
  
  pIVar3 = g_InGameRuntimeRoot;
  if (((movementRuntime->movementStateFlags & 3) == 0) && (movementRuntime->retryCountdown == 0)) {
    FVar4 = FixedMath_Vector2AngleAndLengthRegs
                      (targetWorldY - movementRuntime->movementTargetWorldYQ12,
                       targetWorldX - movementRuntime->movementTargetWorldXQ12);
    if (0x2000 < (int)FVar4) {
      FVar5 = FixedMath_SinCosScaled((AngleTurn32)(FVar4 >> 0x20),0x2000);
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
void ArmyRuntime_StartDirectMoveCommand
               (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  WorldRuntimeContext *worldRuntime;
  EntityPathingDestinationRegisterResult EVar3;
  
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
void ArmyArticulatedRuntime_UpdateContactChildAndEffects
               (ModelRuntimeNode *contactChildModel,WorldRuntimeContext *worldRuntime,
               ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  sdword sVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int effectDefinition;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0xcc);
  if ((((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
      (worldRuntime->dwordArray != (dword *)0x0)) &&
     (bVar3 = false, worldRuntime->dwordArray[uVar1] != 0)) {
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       (armyRuntime->modelNodeRuntime->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_PlayPositionedOneShot
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(DirectSoundVoiceSet **)uVar4);
    }
  }
  modelNode1 = contactChildModel->childNodes[0]->childNodes[0]->childNodes[0];
  sVar2 = FieldGrid_GetNearestWaterDelta
                    ((modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  if (sVar2 < 1) {
    effectDefinition = *(int *)(extraout_ECX_00 + 0x174);
  }
  else {
    effectDefinition = *(int *)(extraout_ECX_00 + 0x58);
  }
  if (effectDefinition != 0) {
    EffectRuntimePool_CreateInstanceFromDefinitionCf
              (extraout_ECX_00,effectDefinition,EFFECT_RUNTIME_COMPLETION_NONE,0,
               (modelNode1->modelPayload).worldRotationAngle2,
               (modelNode1->modelPayload).worldRotationAngle1,
               (modelNode1->modelPayload).worldRotationAngle0,
               (modelNode1->worldTransform).translation.z,(modelNode1->worldTransform).translation.y
               ,(modelNode1->worldTransform).translation.x,(EffectDefinition *)effectDefinition,
               worldRuntime);
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
void ArmyArticulatedRuntime_UpdateSuspensionHierarchy
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
  longlong lVar10;
  uint uVar11;
  dword dVar12;
  dword sideLength0Q12;
  short sVar13;
  int extraout_ECX;
  int iVar14;
  AngleTurn32 angle2;
  int extraout_ECX_00;
  int iVar15;
  AngleTurn32 angle2_00;
  int extraout_ECX_01;
  uint uVar16;
  uint uVar17;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  AngleTurn32 AVar18;
  AngleTurn32 extraout_ECX_06;
  AngleTurn32 extraout_ECX_07;
  short sVar19;
  AngleTurn32 angle1;
  AngleTurn32 angle1_00;
  int iVar20;
  uint uVar21;
  uint uVar22;
  int iVar23;
  FixedLengthAngleEdxEax8 FVar24;
  FixedSinCosEdxEax8 FVar25;
  undefined8 uVar26;
  FixedLengthElevationEdxEax8 FVar27;
  FixedLengthElevationEdxEax8 FVar28;
  FixedEulerPairEdxEax8 FVar29;
  UQ12 scale;
  GraphicsWorldCoordinateQ12 GVar30;
  GraphicsWorldCoordinateQ12 GVar31;
  GraphicsWorldCoordinateQ12 GVar32;
  GraphicsWorldCoordinateQ12 GVar33;
  int iVar34;
  GraphicsWorldCoordinateQ12 GVar35;
  int iVar36;
  GraphicsWorldCoordinateQ12 GVar37;
  ArmyArticulatedRuntimeSlotView *pAVar2;
  ArmyRuntimeSlot *armySlot1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  pAVar2 = (ArmyArticulatedRuntimeSlotView *)(modelNodeRuntime->runtimePayload).armyRuntime;
  modelNode2 = modelNodeRuntime->childNodes[0]->childNodes[0]->childNodes[0]->childNodes[0];
  dVar2 = pAVar2->runtimeStateA8;
  lVar10 = (longlong)(int)(pAVar2->runtimeState90 - pAVar2->movementTarget0Q12) *
           (longlong)(int)dVar2;
  dVar12 = pAVar2->runtimeState98;
  (modelNode2->worldTransform).translation.x =
       ((int)((ulonglong)lVar10 >> 0x20) << 0x14 | (uint)lVar10 >> 0xc) + pAVar2->movementTarget0Q12
  ;
  lVar10 = (longlong)(int)(dVar12 - pAVar2->definitionClassValue80) * (longlong)(int)dVar2;
  (modelNode2->worldTransform).translation.y =
       ((int)((ulonglong)lVar10 >> 0x20) << 0x14 | (uint)lVar10 >> 0xc) +
       pAVar2->definitionClassValue80;
  lVar10 = (longlong)
           (int)((((int)((0x1000 - dVar2) * *(int *)((int)pAVar2->definitionOrAsset + 0xc4)) >> 10)
                 + pAVar2->articulatedHeightOrStateA0) - pAVar2->definitionClassValue88) *
           (longlong)(int)dVar2;
  (modelNode2->worldTransform).translation.z =
       ((int)((ulonglong)lVar10 >> 0x20) << 0x14 | (uint)lVar10 >> 0xc) +
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
  iVar14 = 0x1000 - pAVar2->runtimeStateA8;
  g_ArmySuspensionBlendVectorAXQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)extraout_ECX) >>
             0x20) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)extraout_ECX) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar14) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar14) >> 0xc);
  g_ArmySuspensionBlendVectorAYQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)extraout_ECX) >>
             0x20) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)extraout_ECX) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar14) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar14) >> 0xc);
  g_ArmySuspensionBlendVectorAZQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)extraout_ECX) >>
             0x20) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)extraout_ECX) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar14) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar14) >> 0xc);
  FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12);
  modelNode1 = modelNodeRuntime->childNodes[1]->childNodes[0]->childNodes[0]->childNodes[0];
  AVar6 = (pAVar2->articulatedContact).terrainContactMode;
  lVar10 = (longlong)(int)(pAVar2->runtimeState94 - pAVar2->movementTarget1Q12) *
           (longlong)(int)AVar6;
  iVar14 = pAVar2->articulatedCoordinateOrState9C;
  (modelNode1->worldTransform).translation.x =
       ((int)((ulonglong)lVar10 >> 0x20) << 0x14 | (uint)lVar10 >> 0xc) + pAVar2->movementTarget1Q12
  ;
  lVar10 = (longlong)(int)(iVar14 - pAVar2->definitionClassValue84) * (longlong)(int)AVar6;
  (modelNode1->worldTransform).translation.y =
       ((int)((ulonglong)lVar10 >> 0x20) << 0x14 | (uint)lVar10 >> 0xc) +
       pAVar2->definitionClassValue84;
  lVar10 = (longlong)
           (int)((((int)((0x1000 - AVar6) * *(int *)((int)pAVar2->definitionOrAsset + 0xc4)) >> 10)
                 + pAVar2->runtimeStateA4) - pAVar2->runtimeState8C) * (longlong)(int)AVar6;
  (modelNode1->worldTransform).translation.z =
       ((int)((ulonglong)lVar10 >> 0x20) << 0x14 | (uint)lVar10 >> 0xc) + pAVar2->runtimeState8C;
  iVar14 = (pAVar2->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue;
  iVar7 = (pAVar2->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  iVar8 = (pAVar2->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.signedValue;
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12,
             pAVar2->fallbackWorldXQ12 >> 0x10,pAVar2->fallbackWorldXQ12 & 0xffff);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorBXQ12,
             (int)pAVar2->linkedArmyRuntimeOrSavedOffset >> 0x10,
             (uint)pAVar2->linkedArmyRuntimeOrSavedOffset & 0xffff);
  iVar15 = 0x1000 - (pAVar2->articulatedContact).terrainContactMode;
  g_ArmySuspensionBlendVectorAXQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)extraout_ECX_00) >>
             0x20) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAXQ12 * (longlong)extraout_ECX_00) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar15) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBXQ12 * (longlong)iVar15) >> 0xc);
  g_ArmySuspensionBlendVectorAYQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)extraout_ECX_00) >>
             0x20) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAYQ12 * (longlong)extraout_ECX_00) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar15) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBYQ12 * (longlong)iVar15) >> 0xc);
  g_ArmySuspensionBlendVectorAZQ12 =
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)extraout_ECX_00) >>
             0x20) << 0x14 |
       (uint)((longlong)g_ArmySuspensionBlendVectorAZQ12 * (longlong)extraout_ECX_00) >> 0xc) +
       ((int)((ulonglong)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar15) >> 0x20) <<
        0x14 | (uint)((longlong)g_ArmySuspensionBlendVectorBZQ12 * (longlong)iVar15) >> 0xc);
  FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&g_ArmySuspensionBlendVectorAXQ12);
  pvVar9 = pAVar2->definitionOrAsset;
  LOCK();
  UNLOCK();
  armySlot1 = (modelNodeRuntime->runtimePayload).armyRuntime;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 =
       (((int)((armySlot1->ownerValue64 - armySlot1->classState60) * 0x10000) >> 0x10) *
        (armySlot1->runtimeStateA8 + (armySlot1->articulatedContact).terrainContactMode) >> 0xc) +
       armySlot1->classState60 & 0xffff;
  iVar15 = (modelNode2->worldTransform).translation.x;
  iVar23 = (modelNode2->worldTransform).translation.y;
  iVar34 = ((modelNode1->modelPayload).modelResource)->placementHeightOffsetQ12;
  iVar36 = (modelNode1->worldTransform).translation.x;
  iVar20 = (modelNode1->worldTransform).translation.y;
  pGVar1 = &(modelNode2->worldTransform).translation.z;
  *pGVar1 = *pGVar1 + iVar34;
  pGVar1 = &(modelNode1->worldTransform).translation.z;
  *pGVar1 = *pGVar1 + iVar34;
  iVar34 = (modelNode2->worldTransform).translation.z;
  iVar20 = iVar23 + iVar20 >> 1;
  iVar23 = (modelNode1->worldTransform).translation.z;
  (modelNodeRuntime->worldTransform).translation.x = iVar15 + iVar36 >> 1;
  (modelNodeRuntime->worldTransform).translation.y = iVar20;
  iVar15 = (iVar34 + iVar23 >> 1) + *(int *)((int)pvVar9 + 0x54);
  (modelNodeRuntime->worldTransform).translation.z = iVar15;
  GVar37 = (modelNode1->worldTransform).translation.z;
  GVar35 = (modelNode1->worldTransform).translation.y;
  GVar33 = (modelNode1->worldTransform).translation.x;
  GVar32 = (modelNode2->worldTransform).translation.z;
  GVar31 = (modelNode2->worldTransform).translation.y;
  GVar30 = (modelNode2->worldTransform).translation.x;
  ModelNodeRuntime_RebuildTransformsFromRoot(iVar15,iVar20,modelNodeRuntime);
  modelNode2 = modelNodeRuntime->childNodes[0];
  modelNode1 = modelNodeRuntime->childNodes[1];
  FVar24 = FixedMath_Vector2AngleAndLengthRegs
                     (GVar31 - (modelNode2->worldTransform).translation.y,
                      GVar30 - (modelNode2->worldTransform).translation.x);
  uVar22 = (uint)FVar24;
  FVar24 = FixedMath_Vector2AngleAndLengthRegs
                     (GVar35 - (modelNode1->worldTransform).translation.y,
                      GVar33 - (modelNode1->worldTransform).translation.x);
  uVar11 = (uint)FVar24;
  AVar18 = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  uVar16 = extraout_ECX_01 - AVar18;
  uVar21 = (int)(FVar24 >> 0x20) - AVar18;
  uVar17 = uVar16 & 0xffff;
  sVar13 = (short)uVar16;
  uVar16 = uVar21 & 0xffff;
  sVar19 = (short)uVar21;
  if ((0x3fff < uVar17) && (uVar17 < 0xc001)) {
    sVar13 = sVar13 + -0x8000;
  }
  if ((0x3fff < uVar16) && (uVar16 < 0xc001)) {
    sVar19 = sVar19 + -0x8000;
  }
  uVar16 = (uint)sVar13;
  iVar15 = (int)sVar19;
  if (uVar11 < 0x140) {
    if (uVar11 < 0x40) {
      iVar15 = 0;
    }
    else {
      iVar15 = (int)(iVar15 * (uVar11 - 0x40)) >> 8;
    }
  }
  if (uVar22 < 0x140) {
    if (uVar22 < 0x40) {
      uVar16 = 0;
    }
    else {
      uVar16 = (int)(uVar16 * (uVar22 - 0x40)) >> 8;
    }
  }
  AVar18 = (modelNodeRuntime->modelPayload).worldRotationAngle2;
  uVar22 = iVar15 + 0x8000U & 0xffff;
  (modelNode2->modelPayload).localRotationAngle2 = uVar16 & 0xffff;
  (modelNode1->modelPayload).localRotationAngle2 = uVar22;
  uVar22 = (AVar18 - 0x4000) + uVar22 & 0xffff;
  scale = (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
          contactRadiusOrLinkedSlotMask.contactRadiusQ12;
  FVar25 = FixedMath_SinCosScaled
                     (AVar18 + 0x4000 + (uVar16 & 0xffff) & 0xffff,
                      (((modelNodeRuntime->runtimePayload).armyRuntime)->articulatedContact).
                      contactRadiusOrLinkedSlotMask.contactRadiusQ12);
  iVar15 = GVar30 + (int)FVar25;
  iVar23 = GVar31 + (int)(FVar25 >> 0x20);
  FVar25 = FixedMath_SinCosScaled(uVar22,scale);
  iVar20 = (int)(FVar25 >> 0x20);
  iVar34 = GVar33 - (int)FVar25;
  iVar36 = GVar35 - iVar20;
  uVar26 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_02,iVar20,modelNode2);
  ModelNodeRuntime_RebuildTransformsFromRoot
            (extraout_ECX_03,(int)((ulonglong)uVar26 >> 0x20),modelNode1);
  modelNode2 = modelNode2->childNodes[0];
  modelNode1 = modelNode1->childNodes[0];
  FVar27 = FixedMath_VectorToAnglesAndLength3Regs
                     (GVar32 - (modelNode2->worldTransform).translation.z,
                      iVar23 - (modelNode2->worldTransform).translation.y,
                      iVar15 - (modelNode2->worldTransform).translation.x);
  FVar28 = FixedMath_VectorToAnglesAndLength3Regs
                     (GVar37 - (modelNode1->worldTransform).translation.z,
                      iVar36 - (modelNode1->worldTransform).translation.y,
                      iVar34 - (modelNode1->worldTransform).translation.x);
  modelNode2 = modelNode2->childNodes[0];
  dVar12 = FixedMath_LengthVec3
                     ((GraphicsFixedVec3 *)&(modelNode2->modelPayload).localTranslationXQ12);
  sideLength0Q12 =
       FixedMath_LengthVec3
                 ((GraphicsFixedVec3 *)
                  &(modelNode2->childNodes[0]->modelPayload).localTranslationXQ12);
  modelNode2 = modelNodeRuntime->childNodes[0]->childNodes[0];
  modelNode1 = modelNodeRuntime->childNodes[1]->childNodes[0];
  uVar26 = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,dVar12,(Q12)FVar27);
  iVar15 = (int)uVar26 - extraout_ECX_04;
  if (iVar15 < 0) {
    (modelNode2->modelPayload).localRotationAngle0 = 0x8000;
    (modelNode2->modelPayload).localRotationAngle1 = iVar15 + 0x4000;
  }
  else {
    (modelNode2->modelPayload).localRotationAngle0 = 0;
    (modelNode2->modelPayload).localRotationAngle1 = 0x4000 - iVar15;
  }
  modelNode2 = modelNode2->childNodes[0];
  (modelNode2->modelPayload).localRotationAngle1 = 0x4000 - (int)((ulonglong)uVar26 >> 0x20);
  uVar26 = FixedGeometry_SolveTriangleJointAnglesRegs(sideLength0Q12,dVar12,(Q12)FVar28);
  iVar23 = (int)((ulonglong)uVar26 >> 0x20);
  iVar15 = (int)uVar26 - extraout_ECX_05;
  if (iVar15 < 0) {
    (modelNode1->modelPayload).localRotationAngle0 = 0;
    (modelNode1->modelPayload).localRotationAngle1 = iVar15 + 0x4000;
  }
  else {
    (modelNode1->modelPayload).localRotationAngle0 = 0x8000;
    (modelNode1->modelPayload).localRotationAngle1 = 0x4000 - iVar15;
  }
  modelNode1 = modelNode1->childNodes[0];
  AVar18 = 0x4000 - iVar23;
  (modelNode1->modelPayload).localRotationAngle1 = AVar18;
  modelNode2 = modelNode2->childNodes[0];
  modelNode1 = modelNode1->childNodes[0];
  (modelNode2->modelPayload).localRotationAngle0 = 0;
  (modelNode2->modelPayload).localRotationAngle1 = 0x4000;
  (modelNode2->modelPayload).localRotationAngle2 = 0;
  (modelNode1->modelPayload).localRotationAngle0 = 0;
  (modelNode1->modelPayload).localRotationAngle1 = 0x4000;
  (modelNode1->modelPayload).localRotationAngle2 = 0;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  ModelNodeRuntime_RebuildTransformsFromRoot(AVar18,iVar23,modelNodeRuntime);
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (modelNode2->modelPayload).worldRotationAngle2,
             (modelNode2->modelPayload).worldRotationAngle1,
             (modelNode2->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB);
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (((iVar3 - iVar4) * 0x10000 >> 0x10) * dVar2 >> 0xc) + iVar5 & 0xffff,angle1,angle2);
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA);
  FVar29 = FixedTransform_ExtractEulerAnglesRegs
                     ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch);
  (modelNode2->modelPayload).localRotationAngle0 = extraout_ECX_06;
  (modelNode2->modelPayload).localRotationAngle1 = (AngleTurn32)(FVar29 >> 0x20);
  (modelNode2->modelPayload).localRotationAngle2 = (AngleTurn32)FVar29;
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (modelNode1->modelPayload).worldRotationAngle2,
             (modelNode1->modelPayload).worldRotationAngle1,
             (modelNode1->modelPayload).worldRotationAngle0);
  FixedTransform_InvertRigidQ28
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB);
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (((iVar14 - iVar7) * 0x10000 >> 0x10) * AVar6 >> 0xc) + iVar8 & 0xffff,angle1_00,
             angle2_00);
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchB,
             (GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixScratchA);
  FVar29 = FixedTransform_ExtractEulerAnglesRegs
                     ((GraphicsFixedMatrix3x4 *)&g_ArmySuspensionRotationMatrixComposedScratch);
  (modelNode1->modelPayload).localRotationAngle0 = extraout_ECX_07;
  (modelNode1->modelPayload).localRotationAngle1 = (AngleTurn32)(FVar29 >> 0x20);
  (modelNode1->modelPayload).localRotationAngle2 = (AngleTurn32)FVar29;
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
void ArmyArticulatedRuntime_InitializeLeftTerrainContact
               (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
               WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  int iVar2;
  void *pvVar3;
  int iVar4;
  dword dVar5;
  AngleTurn32 angle;
  bool bVar6;
  FixedSinCosEdxEax8 FVar7;
  qword qVar8;
  
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  FVar7 = FixedMath_SinCosScaled
                    (headingAngle16 + 0x4000U & 0xffff,
                     (armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState90 = (int)FVar7 * 2 + armyRuntime->runtimeState94;
  armyRuntime->runtimeState98 =
       (int)(FVar7 >> 0x20) * 2 + armyRuntime->articulatedCoordinateOrState9C;
  FVar7 = FixedMath_SinCosScaled
                    (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  bVar6 = false;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    qVar8 = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(FVar7 >> 0x20) + armyRuntime->runtimeState98,
                       (int)FVar7 + armyRuntime->runtimeState90,worldRuntime->fieldGrid);
    if (!bVar6) {
      armyRuntime->articulatedHeightOrStateA0 = (int)qVar8;
      armyRuntime->fallbackWorldYQ12 = (Q12)(qVar8 >> 0x20);
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
void ArmyArticulatedRuntime_InitializeRightTerrainContact
               (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
               WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  int iVar2;
  void *pvVar3;
  int iVar4;
  dword dVar5;
  AngleTurn32 angle;
  bool bVar6;
  FixedSinCosEdxEax8 FVar7;
  qword qVar8;
  
  armyRuntime->ownerValue64 = headingAngle16;
  (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.signedValue =
       headingAngle16;
  FVar7 = FixedMath_SinCosScaled
                    (headingAngle16 - 0x4000U & 0xffff,
                     (armyRuntime->articulatedContact).lateralOffsetQ12);
  armyRuntime->runtimeState94 = (int)FVar7 * 2 + armyRuntime->runtimeState90;
  armyRuntime->articulatedCoordinateOrState9C =
       (int)(FVar7 >> 0x20) * 2 + armyRuntime->runtimeState98;
  FVar7 = FixedMath_SinCosScaled
                    (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                           contactRadiusQ12);
  bVar6 = false;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    qVar8 = FieldGrid_InterpolateTerrainHeightAndNormal
                      ((int)(FVar7 >> 0x20) + armyRuntime->articulatedCoordinateOrState9C,
                       (int)FVar7 + armyRuntime->runtimeState94,worldRuntime->fieldGrid);
    if (!bVar6) {
      armyRuntime->runtimeStateA4 = (dword)qVar8;
      armyRuntime->fallbackWorldXQ12 = (Q12)(qVar8 >> 0x20);
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
WorldPositionXYRegisterPairQ12
ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  Q12 *pQVar1;
  void *pvVar2;
  dword dVar3;
  longlong lVar4;
  undefined4 in_EAX;
  int iVar5;
  dword dVar6;
  int iVar7;
  int extraout_ECX;
  uint uVar8;
  int extraout_ECX_00;
  AngleTurn32 angle;
  int extraout_ECX_01;
  int extraout_ECX_02;
  AngleTurn32 angle_00;
  undefined4 in_EDX;
  uint uVar9;
  int iVar10;
  bool bVar11;
  FixedSinCosEdxEax8 FVar12;
  qword qVar13;
  FieldGridAsset *fieldGrid1;
  ModelRuntimeNode *modelNode1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  if (steeringAngle16 < 0x8001) {
    if (*(uint *)((int)pvVar2 + 200) < steeringAngle16) {
      steeringAngle16 = *(AngleTurn32 *)((int)pvVar2 + 200);
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    lVar4 = (longlong)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    uVar9 = (int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc;
    FVar12 = FixedMath_SinCosScaled(armyRuntime->classState60 + 0x8000 & 0xffff,uVar9);
    iVar5 = (int)FVar12 + (modelNode1->worldTransform).translation.x;
    iVar10 = (int)(FVar12 >> 0x20) + (modelNode1->worldTransform).translation.y;
    uVar8 = (extraout_ECX + steeringAngle16) - 0x8000 & 0xffff;
    (armyRuntime->linkedChildOverloadedState).leftHeadingCommandOrSpawnValue.headingOrTurnValue =
         uVar8;
    FVar12 = FixedMath_SinCosScaled(uVar8,uVar9);
    iVar5 = iVar5 + (int)FVar12;
    iVar10 = iVar10 + (int)(FVar12 >> 0x20);
    FVar12 = FixedMath_SinCosScaled
                       (extraout_ECX_00 + 0x4000U & 0xffff,
                        (armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState90 = (int)FVar12 + iVar5;
    armyRuntime->runtimeState98 = (int)(FVar12 >> 0x20) + iVar10;
    FVar12 = FixedMath_SinCosScaled
                       (angle,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                              contactRadiusQ12);
    fieldGrid1 = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = ((int)steeringAngle16 >> 1) + armyRuntime->classState60 & 0xffff;
    bVar11 = false;
    if (fieldGrid1 != (FieldGridAsset *)0x0) {
      qVar13 = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(FVar12 >> 0x20) + armyRuntime->runtimeState98,
                          (int)FVar12 + armyRuntime->runtimeState90,fieldGrid1);
      if (!bVar11) {
        armyRuntime->articulatedHeightOrStateA0 = (int)qVar13;
        armyRuntime->fallbackWorldYQ12 = (Q12)(qVar13 >> 0x20);
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
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  else {
    uVar8 = 0x10000 - *(int *)((int)pvVar2 + 200);
    if (steeringAngle16 < uVar8) {
      steeringAngle16 = uVar8;
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    iVar7 = steeringAngle16 - 0x10000;
    lVar4 = (longlong)(armyRuntime->articulatedContact).lateralOffsetQ12 * 0x1800;
    uVar9 = (int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc;
    FVar12 = FixedMath_SinCosScaled(armyRuntime->classState60 + 0x8000 & 0xffff,uVar9);
    iVar5 = (int)FVar12 + (modelNode1->worldTransform).translation.x;
    iVar10 = (int)(FVar12 >> 0x20) + (modelNode1->worldTransform).translation.y;
    uVar8 = (extraout_ECX_01 + iVar7) - 0x8000U & 0xffff;
    (armyRuntime->linkedChildOverloadedState).rightHeadingCommandOrSpawnValue.headingOrTurnValue =
         uVar8;
    FVar12 = FixedMath_SinCosScaled(uVar8,uVar9);
    iVar5 = iVar5 + (int)FVar12;
    iVar10 = iVar10 + (int)(FVar12 >> 0x20);
    FVar12 = FixedMath_SinCosScaled
                       (extraout_ECX_02 - 0x4000U & 0xffff,
                        (armyRuntime->articulatedContact).lateralOffsetQ12);
    armyRuntime->runtimeState94 = (int)FVar12 + iVar5;
    armyRuntime->articulatedCoordinateOrState9C = (int)(FVar12 >> 0x20) + iVar10;
    FVar12 = FixedMath_SinCosScaled
                       (angle_00,(armyRuntime->articulatedContact).contactRadiusOrLinkedSlotMask.
                                 contactRadiusQ12);
    fieldGrid1 = worldRuntime->fieldGrid;
    armyRuntime->ownerValue64 = (iVar7 >> 1) + armyRuntime->classState60 & 0xffff;
    bVar11 = false;
    if (fieldGrid1 != (FieldGridAsset *)0x0) {
      qVar13 = FieldGrid_InterpolateTerrainHeightAndNormal
                         ((int)(FVar12 >> 0x20) + armyRuntime->articulatedCoordinateOrState9C,
                          (int)FVar12 + armyRuntime->runtimeState94,fieldGrid1);
      if (!bVar11) {
        armyRuntime->runtimeStateA4 = (dword)qVar13;
        armyRuntime->fallbackWorldXQ12 = (Q12)(qVar13 >> 0x20);
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
  return CONCAT44(in_EDX,in_EAX);
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
void ArmyRuntimeCommand_UpdateTargetFollowingState
               (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
               WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyMovementRuntime *movementRuntime;
  undefined1 in_CF;
  
  ArmyWeaponRuntime_TestTargetLineOfFireCf
            (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,worldRuntime,armyRuntime);
  if ((bool)in_CF) {
    if (((armyRuntime ==
          (movementRuntime->entityRuntime->classPayload).impactOwnerLinks.primaryImpactArmyRuntime)
        || ((movementRuntime->entityRuntime->classPayload).impactOwnerLinks.primaryImpactArmyRuntime
            == (ArmyRuntimeSlot *)0x0)) && ((movementRuntime->movementStateFlags & 0x20) == 0)) {
      if ((movementRuntime->commandModeFlags & 8) == 0) {
        ArmyRuntime_StartMoveCommandWithFallbackWaypoints
                  (targetWorldYQ12,targetWorldXQ12,movementRuntime);
      }
      else {
        ArmyRuntime_StartClampedMoveCommand(targetWorldYQ12,targetWorldXQ12,movementRuntime);
      }
    }
    return;
  }
  if ((movementRuntime->movementStateFlags & 0x20) != 0) {
    if ((movementRuntime->commandModeFlags & 8) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget(movementRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition(movementRuntime);
    }
  }
  return;
}

/* Address: 0x0051CA60.
   Ownership: gameplay/army/movement.
   Purpose: Appends a waypoint to the bounded movement queue when the runtime is already moving, otherwise starts
   the variant-A movement command immediately.
   Local calls: ArmyRuntime_QueueOrStartMoveCommandVariantA.
*/
void ArmyRuntime_QueueWaypointOrStartMoveVariantA
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
void ArmyRuntime_StartMoveCommandWithFallbackWaypoints
               (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime)

{
  GraphicsWorldCoordinateQ12 GVar1;
  GraphicsWorldCoordinateQ12 GVar2;
  int iVar3;
  WorldRuntimeContext *worldRuntime;
  Q12 *fallbackCoordinateRead;
  Q12 *waypointCoordinateWrite;
  EntityPathingDestinationRegisterResult EVar4;
  
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
undefined4 ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime)

{
  undefined4 in_EAX;
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
  return in_EAX;
}

/* Address: 0x0051CE90.
   Ownership: gameplay/army/movement.
   Purpose: Resets the movement-state flags and, for the verified queued state, copies the current model position
   into the active and fallback target fields.
*/
undefined4 ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime)

{
  undefined4 in_EAX;
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
  return in_EAX;
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
WorldPositionXYRegisterPairQ12
ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime)

{
  ArmyWaypointCount *pAVar1;
  int iVar2;
  uint uVar3;
  undefined4 extraout_EAX;
  int iVar4;
  uint uVar5;
  undefined4 extraout_EDX;
  Q12 *queuedCoordinateRead;
  Q12 *queuedCoordinateWrite;
  bool bVar6;
  WorldPositionXYRegisterPairQ12 WVar7;
  WorldPositionXYRegisterPairQ12 WVar8;
  EntityPathingDestinationRegisterResult EVar9;
  Q12 queuedWorldYQ12;
  Q12 queuedWorldXQ12;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = movementRuntime->modelNodeRuntime;
  if ((movementRuntime->movementStateFlags & 0x10) == 0) {
    if ((movementRuntime->movementStateFlags & 1) != 0) {
ArmyRuntime_UpdateMovementAndWaypoints_ReturnStoredMovementPositionWithCarryClear:
      WVar7._0_4_ = movementRuntime->movementWorldXQ12;
      WVar7._4_4_ = movementRuntime->movementWorldYQ12;
      return WVar7;
    }
  }
  else {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffffef;
    iVar2 = (movementRuntime->fallbackPosition).worldXQ12 -
            (modelNode1->worldTransform).translation.x;
    iVar4 = (movementRuntime->fallbackPosition).worldYQ12 -
            (modelNode1->worldTransform).translation.y;
    if ((((iVar2 < 0x40) && (iVar4 < 0x40)) && (-0x40 < iVar2)) && (-0x40 < iVar4)) {
      if ((movementRuntime->movementStateFlags & 8) != 0) {
        queuedWorldXQ12 = movementRuntime->queuedWaypoints[0].worldXQ12;
        queuedWorldYQ12 = movementRuntime->queuedWaypoints[0].worldYQ12;
        pAVar1 = &movementRuntime->queuedWaypointCount;
        *pAVar1 = *pAVar1 - 1;
        if (*pAVar1 == 0) {
          movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xfffffff7;
        }
        else {
          iVar2 = 0xe;
          queuedCoordinateRead = &movementRuntime->queuedWaypoints[1].worldXQ12;
          queuedCoordinateWrite = &movementRuntime->queuedWaypoints[0].worldXQ12;
          for (; iVar2 != 0; iVar2 = iVar2 + -1) {
            *queuedCoordinateWrite = *queuedCoordinateRead;
            queuedCoordinateRead = queuedCoordinateRead + 1;
            queuedCoordinateWrite = queuedCoordinateWrite + 1;
          }
        }
        ArmyRuntime_QueueOrStartMoveCommandVariantB(queuedWorldYQ12,queuedWorldXQ12,movementRuntime)
        ;
        WVar7 = ArmyRuntime_UpdateMovementAndWaypoints(worldRuntime,movementRuntime);
        return WVar7;
      }
    }
    else {
      uVar3 = (modelNode1->worldTransform).translation.x;
      uVar5 = (modelNode1->worldTransform).translation.y;
      bVar6 = false;
      if (((movementRuntime->retryCountdown != 0) &&
          (bVar6 = uVar3 < (uint)movementRuntime->lastCheckedWorldXQ12,
          uVar3 == movementRuntime->lastCheckedWorldXQ12)) &&
         (bVar6 = uVar5 < (uint)movementRuntime->lastCheckedWorldYQ12,
         uVar5 == movementRuntime->lastCheckedWorldYQ12))
      goto ArmyRuntime_UpdateMovementAndWaypoints_ReturnStoredMovementPositionWithCarryClear;
      movementRuntime->retryCountdown = 0x40;
      movementRuntime->lastCheckedWorldXQ12 = uVar3;
      movementRuntime->lastCheckedWorldYQ12 = uVar5;
      EVar9 = EntityPathing_ResolveDestinationAndRebuildRoutes
                        ((movementRuntime->fallbackPosition).worldYQ12,
                         (movementRuntime->fallbackPosition).worldXQ12,
                         movementRuntime->entityRuntime,worldRuntime);
      if (!bVar6) {
        movementRuntime->movementWorldXQ12 = EVar9.primaryWorldXQ12;
        movementRuntime->movementWorldYQ12 = EVar9.primaryWorldYQ12;
        (movementRuntime->fallbackPosition).worldXQ12 = EVar9.fallbackWorldXQ12;
        (movementRuntime->fallbackPosition).worldYQ12 = EVar9.fallbackWorldYQ12;
        return EVar9._0_8_;
      }
    }
  }
  uVar3 = (modelNode1->worldTransform).translation.x - movementRuntime->movementTargetWorldXQ12;
  if ((int)uVar3 < 0) {
    uVar3 = -uVar3;
  }
  uVar5 = (modelNode1->worldTransform).translation.y - movementRuntime->movementTargetWorldYQ12;
  if ((int)uVar5 < 0) {
    uVar5 = -uVar5;
  }
  if ((uVar3 < 0x1001) && (uVar5 < 0x1001)) {
    movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & 0xffffff06;
    WVar8._0_4_ = (modelNode1->worldTransform).translation.x;
    WVar8._4_4_ = (modelNode1->worldTransform).translation.y;
    return WVar8;
  }
  ArmyRuntime_StartDirectMoveCommand
            (movementRuntime->movementTargetWorldYQ12,movementRuntime->movementTargetWorldXQ12,
             movementRuntime);
  return CONCAT44(extraout_EDX,extraout_EAX);
}
