#include <thandor/gameplay/army/combat.h>

/* Implementation ownership: gameplay/army/combat. */

/* Address: 0x00523980.
   Ownership: gameplay/army/combat.
   Purpose: Table membership RUNTIME_UPDATE[9]. Resolves target aim state, smooths pitch and yaw, follows the
   linked target, resolves shot launch transforms from model attachments, and fires the active attachment slots.
   Runtime-update partition slots 0-23 receive (worldRuntime, armyRuntime). Role: Tracks the target, smooths
   yaw/pitch, solves launch angles and fires from matching attachments. Inputs: Army weapon state, model hierarchy,
   ShotDefinition, target and world context.
   Local calls: ArmyRuntime_EmitDamageThresholdEffect.
   Cross-module calls: ArmyRuntime_ResolveShotAimPointCf [gameplay/army/runtime],
   ArmyRuntime_UpdateMovementAndWaypoints [gameplay/army/movement], ModelNodeRuntime_SmoothYawTowardTarget
   [world/model/hierarchy], ModelNodeRuntime_SmoothPitchTowardTarget [world/model/hierarchy],
   ShotDefinition_ComputeLaunchAnglesRegs [assets/shot/catalog], ModelNodeRuntime_ComputeRelativeDirectionAngle
   [world/model/hierarchy].
*/
void ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  dword *pdVar1;
  ModelMeshGroupMask *pMVar2;
  ArmyRuntimeSlot **ppAVar3;
  Q12 *pQVar4;
  byte *pbVar5;
  void *pvVar6;
  int iVar7;
  MdlSerializedNodeHeader38 *definitionNode;
  InGameSimulationStepBatchTicks IVar8;
  Q12 point0Z;
  uint targetYawAngle16;
  void *shotRuntimeState14;
  ModelRuntimeNode *in_ECX;
  Q12 point0Y;
  ModelRuntimeNode *extraout_ECX;
  ModelRuntimeNode *extraout_ECX_00;
  ModelRuntimeNode *modelNodeRuntime;
  ModelRuntimeNode *extraout_ECX_01;
  ModelRuntimeNode *extraout_ECX_02;
  ModelRuntimeNode *extraout_ECX_03;
  ModelRuntimeNode *extraout_ECX_04;
  ModelRuntimeNode *in_EDX;
  Q12 point0X;
  ModelRuntimeNode *extraout_EDX;
  AngleTurn32 targetPitchAngle16;
  ModelRuntimeNode *extraout_EDX_00;
  ModelRuntimeNode *extraout_EDX_01;
  ModelRuntimeNode *extraout_EDX_02;
  ModelRuntimeNode *extraout_EDX_03;
  undefined1 uVar9;
  bool bVar10;
  ShotAimXZRegisterPairQ12 SVar11;
  WorldPositionXYRegisterPairQ12 WVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  IVar8 = g_InGameSimulationStepTicks;
  modelNode2 = armyRuntime->modelNodeRuntime->childNodes[0]->childNodes[0];
  if ((armyRuntime->runtimeFlags & 9) == 0) {
    pdVar1 = &armyRuntime->classState60;
    *pdVar1 = *pdVar1 - g_InGameSimulationStepTicks;
    if ((int)*pdVar1 < 0) {
      armyRuntime->classState60 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 1;
    }
    pdVar1 = &armyRuntime->ownerValue64;
    *pdVar1 = *pdVar1 - IVar8;
    if ((int)*pdVar1 < 0) {
      armyRuntime->ownerValue64 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 2;
    }
    pdVar1 = &armyRuntime->ownerValue68;
    *pdVar1 = *pdVar1 - IVar8;
    if ((int)*pdVar1 < 0) {
      armyRuntime->ownerValue68 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 4;
    }
    ppAVar3 = &armyRuntime->linkedArmyRuntimeOrSavedOffset;
    *ppAVar3 = (ArmyRuntimeSlot *)((int)*ppAVar3 - IVar8);
    if ((int)*ppAVar3 < 0) {
      armyRuntime->linkedArmyRuntimeOrSavedOffset = (ArmyRuntimeSlot *)0x0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 8;
    }
    pQVar4 = &armyRuntime->fallbackWorldYQ12;
    *pQVar4 = *pQVar4 - IVar8;
    if (*pQVar4 < 0) {
      armyRuntime->fallbackWorldYQ12 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 0x10;
    }
    pQVar4 = &armyRuntime->fallbackWorldXQ12;
    *pQVar4 = *pQVar4 - IVar8;
    if (*pQVar4 < 0) {
      armyRuntime->fallbackWorldXQ12 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 0x20;
    }
    pQVar4 = &armyRuntime->movementTarget0Q12;
    *pQVar4 = *pQVar4 - IVar8;
    if (*pQVar4 < 0) {
      armyRuntime->movementTarget0Q12 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 0x40;
    }
    pQVar4 = &armyRuntime->movementTarget1Q12;
    *pQVar4 = *pQVar4 - IVar8;
    if (*pQVar4 < 0) {
      armyRuntime->movementTarget1Q12 = 0;
      pMVar2 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar2 = *pMVar2 | 0x80;
    }
    pdVar1 = &armyRuntime->definitionClassValue80;
    uVar9 = *pdVar1 < IVar8;
    *pdVar1 = *pdVar1 - IVar8;
    if ((int)*pdVar1 < 0) {
      armyRuntime->definitionClassValue80 = 0;
    }
    pvVar6 = armyRuntime->definitionOrAsset;
    entityRuntime1 = armyRuntime->linkedEntityRuntime;
    modelNode2 = armyRuntime->modelNodeRuntime;
    SVar11 = ArmyRuntime_ResolveShotAimPointCf
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,
                        *(ShotDefinition **)((int)pvVar6 + 0x2c),entityRuntime1);
    point0X = (Q12)(SVar11 >> 0x20);
    point0Z = (Q12)SVar11;
    if ((bool)uVar9) {
      WVar12 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      in_EDX = (ModelRuntimeNode *)(WVar12 >> 0x20);
      if (((!(bool)uVar9) || (armyRuntime->movementStateFlags != 0)) ||
         (in_ECX = extraout_ECX, (armyRuntime->movementControl).turnVelocityAngle16 != 0)) {
        modelNode2 = armyRuntime->modelNodeRuntime;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,armyRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],armyRuntime,0);
        in_ECX = extraout_ECX_00;
        in_EDX = extraout_EDX;
      }
    }
    else {
      modelNode2 = armyRuntime->modelNodeRuntime;
      modelNode1 = modelNode2->childNodes[0];
      uVar13 = ShotDefinition_ComputeLaunchAnglesRegs
                         (point0X,point0Y,point0Z,(modelNode1->worldTransform).translation.z,
                          (modelNode1->worldTransform).translation.y,
                          (modelNode1->worldTransform).translation.x,
                          *(ShotDefinition **)((int)pvVar6 + 0x2c));
      targetYawAngle16 =
           ModelNodeRuntime_ComputeRelativeDirectionAngle
                     (modelNode2,(AngleTurn32)((ulonglong)uVar13 >> 0x20),(AngleTurn32)uVar13);
      ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,armyRuntime,targetYawAngle16);
      if ((bool)uVar9) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,armyRuntime,targetPitchAngle16);
        in_ECX = extraout_ECX_01;
        in_EDX = extraout_EDX_00;
      }
      else {
        uVar14 = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,armyRuntime,targetPitchAngle16);
        in_EDX = (ModelRuntimeNode *)((ulonglong)uVar14 >> 0x20);
        in_ECX = extraout_ECX_02;
        if ((ModelRuntimeNode *)uVar14 == in_EDX) {
          pvVar6 = armyRuntime->definitionOrAsset;
          bVar10 = false;
          if ((armyRuntime->definitionClassValue80 == 0) &&
             (ArmyRuntimeCommand_UpdateTargetFollowingState
                        (point0X,point0Y,point0Z,worldRuntime,armyRuntime), in_ECX = extraout_ECX_03
             , in_EDX = extraout_EDX_01, !bVar10)) {
            in_ECX = extraout_ECX_03->childNodes[0];
            iVar7 = *(int *)((int)pvVar6 + 100);
            in_ECX->runtimeFlags = in_ECX->runtimeFlags | 1;
            definitionNode = *(MdlSerializedNodeHeader38 **)(*(int *)(iVar7 + 0x18) + 0x18);
            in_EDX = (ModelRuntimeNode *)0x0;
            do {
              if (*(int *)(armyRuntime->reservedE0_EB + (int)in_EDX * 4 + -0x80) == 0) {
                *(undefined4 *)(armyRuntime->reservedE0_EB + (int)in_EDX * 4 + -0x80) =
                     *(undefined4 *)((int)pvVar6 + 0x30);
                entityRuntime1 =
                     (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity;
                bVar10 = false;
                shotRuntimeState14 = (void *)0x0;
                if (entityRuntime1 != (GameEntityRuntime *)0x0) {
                  shotRuntimeState14 = (entityRuntime1->common).ownership.definitionOrClassRecord;
                }
                ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf
                          ((ShotRuntimeState14)shotRuntimeState14,point0X,point0Y,point0Z,
                           (SprAttachmentSelectorOrdinal)in_EDX,
                           *(ShotDefinition **)((int)pvVar6 + 0x2c),in_ECX,definitionNode,
                           worldRuntime);
                if (!bVar10) {
                  armyRuntime->definitionClassValue80 = *(dword *)((int)pvVar6 + 0x38);
                  ArmyRuntime_SetNonzeroActionVector
                            ((AngleTurn32)uVar13,*(Q12 *)((int)pvVar6 + 0x44),
                             *(Q12 *)((int)pvVar6 + 0x40),
                             (ArmyRuntimeSlot *)armyRuntime->linkedEntityRuntime);
                  pMVar2 = &(armyRuntime->modelNodeRuntime->childNodes[0]->childNodes[0]->
                            modelPayload).meshGroupMask;
                  *pMVar2 = *pMVar2 & -2 << ((byte)extraout_EDX_03 & 0x1f);
                  in_ECX = extraout_EDX_03;
                  in_EDX = extraout_EDX_03;
                  break;
                }
                pbVar5 = armyRuntime->reservedE0_EB + (int)extraout_EDX_02 * 4 + -0x80;
                pbVar5[0] = 0xff;
                pbVar5[1] = 0xff;
                pbVar5[2] = 0xff;
                pbVar5[3] = 0xff;
                in_ECX = extraout_ECX_04;
                in_EDX = extraout_EDX_02;
              }
              in_EDX = (ModelRuntimeNode *)((int)&(in_EDX->common).previousNode + 1);
            } while (in_EDX < (ModelRuntimeNode *)0x8);
          }
        }
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,in_EDX,armyRuntime->modelNodeRuntime);
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
  return;
}

/* Address: 0x00525130.
   Ownership: gameplay/army/combat.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[15]@0051FC98. Runtime-update partition slots
   0-23 receive (worldRuntime, armyRuntime).
   Local calls: ArmyRuntime_EmitDamageThresholdEffect.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void ArmyRuntimeClass_UpdateTransformAndDamageEffect
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  ModelRuntimeNode *modelNodeRuntime;
  longlong lVar2;
  longlong lVar3;
  int iVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar1 = armyRuntime->definitionOrAsset;
  iVar4 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex * 0x740;
  if (*(int *)((int)pvVar1 + 0xc0) == 1) {
    iVar4 = iVar4 + 0x10;
  }
  lVar3 = (longlong)(*(int *)((int)pvVar1 + 0x28) - *(int *)((int)pvVar1 + 0x24)) *
          (longlong)*(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x78);
  lVar2 = (longlong)*(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x74);
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  modelNode1 = modelNodeRuntime->childNodes[0];
  if ((modelNodeRuntime->childCount != 0) && (modelNode1 != (ModelRuntimeNode *)0x0)) {
    (modelNode1->modelPayload).localTranslationZQ12 =
         (int)(lVar3 / lVar2) + *(int *)((int)pvVar1 + 0x24);
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
    ModelNodeRuntime_RebuildTransformsFromRoot(pvVar1,(int)(lVar3 % lVar2),modelNodeRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
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
void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  if (((armyRuntime->runtimeFlags & 9) == 0) &&
     ((*(int *)((int)armyRuntime->definitionOrAsset + 0x1c8) == 0 ||
      ((armyRuntime->runtimeFlags & 0x40) != 0)))) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,armyRuntime);
    ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,armyRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,armyRuntime);
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
void ArmyRuntime_ApplyImpactDamageAndFinalizeState
               (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  FactionRelationCounter *pFVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    iVar3 = *(int *)((int)armyRuntime->definitionOrAsset + 0x60);
    pQVar1 = &armyRuntime->actionVector2Q12;
    iVar4 = *pQVar1;
    *pQVar1 = *pQVar1 - damageAmount;
    if (*pQVar1 == 0 || SBORROW4(iVar4,damageAmount) != *pQVar1 < 0) {
      iVar4 = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      modelNode1 = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      armyRuntime->actionVector2Q12 = 0;
      if (modelNode1 == (ModelRuntimeNode *)0x0) {
        bVar5 = false;
        if ((*(int *)((int)armyRuntime->definitionOrAsset + 0x4c) == 0) &&
           (bVar5 = *(int *)((int)armyRuntime->definitionOrAsset + 0x278) == 0, bVar5)) {
          (armyRuntime->modelNodeRuntime->modelPayload).worldRotationAngle0 = impactAngle;
        }
        iVar4 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
        if ((!bVar5) &&
           (iVar3 = *(int *)((int)armyRuntime->definitionOrAsset + 0x4c),
           pFVar2 = &g_GameFactionRuntimeImage.records[iVar4].relationCounterC,
           *pFVar2 = *pFVar2 + 1,
           g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[iVar3] ==
           ArmyRuntime_ClassCommandHandlerGroupACf)) {
          pFVar2 = &g_GameFactionRuntimeImage.records[iVar4].relationCounterD;
          *pFVar2 = *pFVar2 + 1;
          pFVar2 = &g_GameFactionRuntimeImage.records[iVar4].relationCounterC;
          *pFVar2 = *pFVar2 + -1;
        }
      }
      else {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-iVar4,(modelNode1->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      iVar4 = iVar3 - armyRuntime->actionVector2Q12;
      if (iVar4 == 0 || iVar3 < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + iVar4;
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
undefined8
ArmyRuntime_ApplyDamageAndFactionRelationState
          (FactionRuntimeIndex sourceFactionIndex,DamageAmount32 damageAmount,
          ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  int iVar2;
  undefined4 in_EAX;
  undefined4 in_EDX;
  int iVar3;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    iVar2 = *(int *)((int)armyRuntime->definitionOrAsset + 0x60);
    pQVar1 = &armyRuntime->actionVector2Q12;
    iVar3 = *pQVar1;
    *pQVar1 = *pQVar1 - damageAmount;
    if (*pQVar1 == 0 || SBORROW4(iVar3,damageAmount) != *pQVar1 < 0) {
      iVar3 = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      modelNode1 = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector2Q12 = 0;
      if (modelNode1 != (ModelRuntimeNode *)0x0) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-iVar3,(modelNode1->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      iVar3 = iVar2 - armyRuntime->actionVector2Q12;
      if (iVar3 == 0 || iVar2 < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + iVar3;
      }
    }
  }
  return CONCAT44(in_EDX,in_EAX);
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
void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent
               (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
               ImpactDamageValue32 impactValue,ArmyRuntimeSlot *targetArmyRuntime)

{
  int extraout_EAX;
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
            (impactAngle,sourceFactionIndex,impactValue - extraout_EAX,
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
   [core/math/fixed], FixedMath_Atan2Angle16 [core/math/fixed], ModelRuntime_RaycastCandidateListNearestCf
   [world/model/runtime], FixedMath_VectorToAnglesAndLength3Regs [core/math/fixed],
   FieldGrid_RaycastTerrainSurfaceDistanceCf [world/terrain/grid].
*/
undefined8
ArmyWeaponRuntime_TestTargetLineOfFireCf
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  int *piVar2;
  longlong lVar3;
  undefined4 in_EAX;
  int iVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  AngleTurn32 azimuthAngle;
  int extraout_ECX;
  undefined4 in_EDX;
  int x;
  AngleTurn32 elevationAngle;
  int extraout_EDX;
  uint uVar8;
  undefined1 uVar9;
  bool bVar10;
  FixedLengthElevationEdxEax8 FVar11;
  ulonglong uVar12;
  ModelRaycastNearestHitCfRegisterResult MVar13;
  FixedLengthAngleEdxEax8 FVar14;
  AngleTurn32 azimuthAngle_00;
  GraphicsWorldCoordinateQ12 originZQ12;
  GraphicsWorldCoordinateQ12 originYQ12;
  GraphicsWorldCoordinateQ12 originXQ12;
  ModelRuntimeClassId requiredOwnerId;
  ModelRuntimeNode *excludedNode;
  GameEntityRuntime *entityRuntime1;
  GameEntityRuntime *entityRuntime2;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  entityRuntime2 = armyRuntime->linkedEntityRuntime;
  pvVar1 = armyRuntime->definitionOrAsset;
  iVar4 = targetWorldXQ12 - (modelNode1->worldTransform).translation.x;
  iVar5 = *(int *)((int)pvVar1 + 0x24);
  uVar8 = *(uint *)((int)pvVar1 + 0x28);
  iVar7 = targetWorldYQ12 - (modelNode1->worldTransform).translation.y;
  piVar2 = *(int **)((int)pvVar1 + 0x2c);
  x = targetWorldZQ12 - (modelNode1->worldTransform).translation.z;
  if (*piVar2 == 1) {
    FVar14 = FixedMath_Vector2AngleAndLengthRegs(iVar7,iVar4);
    iVar7 = piVar2[3] * piVar2[3];
    iVar4 = (int)FVar14 * piVar2[0x37];
    lVar3 = (longlong)(iVar7 + piVar2[0x37] * x * -2) * (longlong)iVar7 -
            (longlong)iVar4 * (longlong)iVar4;
    if (-1 < lVar3) {
      dVar6 = FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar3 >> 0x20),(UInt64Half32)lVar3);
      if ((extraout_EDX < x) && (x < 0x1000)) {
        dVar6 = -dVar6;
      }
      dVar6 = FixedMath_Atan2Angle16(extraout_ECX + dVar6,iVar4);
      if (((int)dVar6 < iVar5) || (bVar10 = dVar6 < uVar8, (int)uVar8 < (int)dVar6))
      goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked;
      modelNode1 = armyRuntime->modelNodeRuntime;
      ModelRuntime_RaycastCandidateListNearestCf
                (dVar6,(uint)(FVar14 >> 0x20) & 0xffff,(int)FVar14,
                 (modelNode1->worldTransform).translation.z,
                 (modelNode1->worldTransform).translation.y,
                 (modelNode1->worldTransform).translation.x,MODEL_RUNTIME_CLASS_00,
                 (armyRuntime->linkedEntityRuntime->common).ownership.modelNode,worldRuntime);
      if (bVar10) {
        entityRuntime2 = armyRuntime->linkedEntityRuntime;
        entityRuntime1 = ((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime;
        iVar5 = (entityRuntime2->common).ownership.ownerIndex;
        if ((entityRuntime2->common).commandState < 1) {
          if (iVar5 == (entityRuntime1->common).ownership.ownerIndex)
          goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnClear;
        }
        else if (iVar5 != (entityRuntime1->common).ownership.ownerIndex)
        goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnClear;
        if (entityRuntime1 != (entityRuntime2->common).commandTarget.targetEntity)
        goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked;
      }
      goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnClear;
    }
    goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked;
  }
  if (*piVar2 == 2) goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnClear;
  FVar11 = FixedMath_VectorToAnglesAndLength3Regs(x,iVar7,iVar4);
  elevationAngle = (AngleTurn32)(FVar11 >> 0x20);
  iVar4 = (int)FVar11;
  if ((piVar2[0xa4] == 0) && (((int)elevationAngle < iVar5 || ((int)uVar8 < (int)elevationAngle))))
  goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked;
  excludedNode = (entityRuntime2->common).ownership.modelNode;
  requiredOwnerId = MODEL_RUNTIME_CLASS_00;
  originXQ12 = (modelNode1->worldTransform).translation.x;
  iVar7 = (int)((longlong)piVar2[3] * (longlong)piVar2[0x34]);
  uVar9 = (longlong)iVar7 != (longlong)piVar2[3] * (longlong)piVar2[0x34];
  originYQ12 = (modelNode1->worldTransform).translation.y;
  originZQ12 = (modelNode1->worldTransform).translation.z;
  azimuthAngle_00 = azimuthAngle;
  uVar12 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                     (elevationAngle,azimuthAngle,iVar7,(modelNode1->worldTransform).translation.z,
                      (modelNode1->worldTransform).translation.y,
                      (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  iVar5 = (int)uVar12;
  MVar13 = ModelRuntime_RaycastCandidateListNearestCf
                     (elevationAngle,azimuthAngle_00,iVar7,originZQ12,originYQ12,originXQ12,
                      requiredOwnerId,excludedNode,worldRuntime);
  if ((bool)uVar9) {
    if (MVar13.nearestDistanceQ12 <= iVar5) {
      entityRuntime2 = armyRuntime->linkedEntityRuntime;
      entityRuntime1 =
           (((MVar13.nearestModelNode)->runtimePayload).armyRuntime)->linkedEntityRuntime;
      iVar5 = (entityRuntime2->common).ownership.ownerIndex;
      if ((entityRuntime2->common).commandState < 1) {
        if (iVar5 != (entityRuntime1->common).ownership.ownerIndex)
        goto ArmyWeaponRuntime_TestTargetLineOfFire_ValidateRaycastHitMatchesCommandTarget;
      }
      else if (iVar5 == (entityRuntime1->common).ownership.ownerIndex) {
ArmyWeaponRuntime_TestTargetLineOfFire_ValidateRaycastHitMatchesCommandTarget:
        if (entityRuntime1 != (entityRuntime2->common).commandTarget.targetEntity)
        goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked;
      }
ArmyWeaponRuntime_TestTargetLineOfFire_EvaluateTargetClearanceAgainstWeaponRange:
      entityRuntime2 = (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity;
      if (entityRuntime2 != (GameEntityRuntime *)0x0) {
        iVar4 = iVar4 * 2 -
                *(int *)(*(int *)(entityRuntime2->common).ownership.definitionOrClassRecord + 0xdc)
                >> 1;
      }
      if (iVar4 <= piVar2[3] * ((piVar2[0x9c] * -0xaaa >> 0xc) + piVar2[0x34] + -1))
      goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnClear;
      goto ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked;
    }
  }
  else if (0x7ffffffe < iVar5)
  goto ArmyWeaponRuntime_TestTargetLineOfFire_EvaluateTargetClearanceAgainstWeaponRange;
  uVar8 = iVar5 - iVar4;
  if ((int)uVar8 < 0) {
    uVar8 = -uVar8;
  }
  if (((armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity ==
       (GameEntityRuntime *)0x0) && (uVar8 < 0x401)) {
ArmyWeaponRuntime_TestTargetLineOfFire_ReturnClear:
    return CONCAT44(in_EDX,in_EAX);
  }
ArmyWeaponRuntime_TestTargetLineOfFire_ReturnBlocked:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052A200.
   Ownership: gameplay/army/combat.
   Purpose: Typed parameters: p2 damageAmount→DamageAmount32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void ArmyRuntime_ApplyDamageAndPropagateToParent
               (DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime)

{
  Q12 *pQVar1;
  int iVar2;
  int iVar3;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    iVar3 = *(int *)((int)armyRuntime->definitionOrAsset + 0x60);
    pQVar1 = &armyRuntime->actionVector2Q12;
    iVar2 = *pQVar1;
    *pQVar1 = *pQVar1 - damageAmount;
    if (*pQVar1 == 0 || SBORROW4(iVar2,damageAmount) != *pQVar1 < 0) {
      iVar2 = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      modelNode1 = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector2Q12 = 0;
      if (modelNode1 != (ModelRuntimeNode *)0x0) {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-iVar2,(modelNode1->runtimePayload).armyRuntime)
        ;
      }
    }
    else if (iVar3 < armyRuntime->actionVector2Q12) {
      armyRuntime->actionVector2Q12 =
           armyRuntime->actionVector2Q12 + (iVar3 - armyRuntime->actionVector2Q12);
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
   attachment and calls EffectRuntimePool_CreateInstanceFromDefinitionCf.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void ArmyRuntime_EmitDamageThresholdEffect
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  ModelRuntimeNode *modelNodeRuntime;
  dword dVar2;
  ModelPackedPointRecord *localPointRecord;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar3;
  GraphicsWorldCoordinateQ12 extraout_ECX_01;
  GraphicsWorldCoordinateQ12 worldXQ12;
  undefined4 extraout_ECX_02;
  uint uVar4;
  undefined4 extraout_EDX;
  AngleTurn32 orientationAngle0;
  bool bVar5;
  undefined8 uVar6;
  EffectDefinition *effectDefinition;
  
  if ((armyRuntime->runtimeFlags & 0x210) != 0) {
    return;
  }
  pvVar1 = armyRuntime->definitionOrAsset;
  if (*(int *)((int)pvVar1 + 0x60) < 1) {
    return;
  }
  if (*(int *)((int)pvVar1 + 0x250) <=
      (armyRuntime->actionVector2Q12 * 100) / *(int *)((int)pvVar1 + 0x60)) {
    return;
  }
  if (0 < armyRuntime->selectionMetric3) {
    armyRuntime->selectionMetric3 = armyRuntime->selectionMetric3 - g_InGameSimulationStepTicks;
    return;
  }
  uVar4 = 0;
  if (*(int *)((int)pvVar1 + 0x25c) != 0) {
    dVar2 = (*g_RandomGeneratorState.next)();
    uVar4 = (uint)(CONCAT44(extraout_EDX,dVar2) % (ulonglong)*(uint *)((int)pvVar1 + 0x25c));
  }
  bVar5 = CARRY4(uVar4,*(uint *)((int)pvVar1 + 600));
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  armyRuntime->selectionMetric3 = uVar4 + *(uint *)((int)pvVar1 + 600);
  localPointRecord =
       (ModelPackedPointRecord *)
       ModelLookupTable_ContainsPackedKeyCf
                 (armyRuntime->selectionMetric4,3,(modelNodeRuntime->modelPayload).modelResource);
  uVar3 = extraout_ECX;
  if (bVar5) {
    armyRuntime->selectionMetric4 = -1;
    localPointRecord =
         (ModelPackedPointRecord *)
         ModelLookupTable_ContainsPackedKeyCf(0,3,(modelNodeRuntime->modelPayload).modelResource);
    uVar3 = extraout_ECX_00;
    if (bVar5) {
      worldXQ12 = (modelNodeRuntime->worldTransform).translation.y;
      uVar6 = CONCAT44((modelNodeRuntime->worldTransform).translation.z,
                       (modelNodeRuntime->worldTransform).translation.x);
      goto LAB_005282dd;
    }
  }
  uVar6 = ModelNodeRuntime_TransformLocalPointRegs(uVar3,localPointRecord,modelNodeRuntime);
  armyRuntime->selectionMetric4 = armyRuntime->selectionMetric4 + 1;
  worldXQ12 = extraout_ECX_01;
LAB_005282dd:
  effectDefinition = *(EffectDefinition **)((int)pvVar1 + 0x254);
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar4 = dVar2 & 0xffff;
  dVar2 = (*g_RandomGeneratorState.next)();
  EffectRuntimePool_CreateInstanceFromDefinitionCf
            (extraout_ECX_02,orientationAngle0,EFFECT_RUNTIME_COMPLETION_NONE,0,orientationAngle0,
             (dVar2 & 0x1fff) + 0x1fff,uVar4,(Q12)((ulonglong)uVar6 >> 0x20),worldXQ12,(Q12)uVar6,
             effectDefinition,worldRuntime);
  return;
}
