/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/combat.c
 * Reverse engineering by idkFoxes 2026
 */

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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments
          (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime)

{
  ModelRuntimeFlags *pMVar1;
  dword *pdVar2;
  ModelMeshGroupMask *pMVar3;
  ArmyWeaponDefinitionView68 *pAVar4;
  ModelRuntimeNode *modelNodeRuntime;
  MdlSerializedNodeHeader38 *pMVar5;
  ArmyRuntimeSlot *pAVar6;
  InGameSimulationStepBatchTicks IVar7;
  Q12 point0Z;
  ShotRuntimeState14 shotRuntimeState14;
  Q12 point0Y;
  Q12 point0X;
  AngleTurn32 targetPitchAngle16;
  SprAttachmentSelectorOrdinal attachmentSelectorOrdinal;
  bool bVar8;
  ShotLaunchAnglesEaxEdx8 SVar9;
  ModelRelativeDirectionAnglesEaxEdx8 MVar10;
  ModelSmoothEaxCf5 MVar11;
  WorldPositionXYEaxEdxCf9 WVar12;
  WorldPositionEaxEcxEdxCf13 WVar13;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode2;
  
  IVar7 = g_InGameSimulationStepTicks;
  modelNode2 = modelRuntime->rootModelNode->childNodes[0]->childNodes[0];
  if (((modelRuntime->classState).classStateEC & 9) == 0) {
    pdVar2 = modelRuntime->attachmentReloadTicks;
    *pdVar2 = *pdVar2 - g_InGameSimulationStepTicks;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[0] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 1;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 1;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[1] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 2;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 2;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[2] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 4;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 3;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[3] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 8;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 4;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[4] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 0x10;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 5;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[5] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 0x20;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 6;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[6] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 0x40;
    }
    pdVar2 = modelRuntime->attachmentReloadTicks + 7;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->attachmentReloadTicks[7] = 0;
      pMVar3 = &(modelNode2->modelPayload).meshGroupMask;
      *pMVar3 = *pMVar3 | 0x80;
    }
    pdVar2 = &modelRuntime->sharedInterShotTicks;
    *pdVar2 = *pdVar2 - IVar7;
    if ((int)*pdVar2 < 0) {
      modelRuntime->sharedInterShotTicks = 0;
    }
    pAVar4 = modelRuntime->modelDefinition;
    entityRuntime1 = (GameEntityRuntime *)modelRuntime->ownerArmyRuntime;
    modelNode2 = modelRuntime->rootModelNode;
    WVar13 = ArmyRuntime_ResolveShotAimPointCf
                       ((modelNode2->worldTransform).translation.z,
                        (modelNode2->worldTransform).translation.y,
                        (modelNode2->worldTransform).translation.x,pAVar4->shotDefinition,
                        entityRuntime1);
    point0X = WVar13.worldZQ12;
    point0Y = WVar13.worldYQ12;
    point0Z = WVar13.worldXQ12;
    if (WVar13.carry) {
      WVar12 = ArmyRuntime_UpdateMovementAndWaypoints
                         (worldRuntime,(ArmyMovementRuntime *)entityRuntime1);
      if (((!WVar12.carry) || (modelRuntime->pitchTurnVelocityAngle16 != 0)) ||
         (modelRuntime->yawTurnVelocityAngle16 != 0)) {
        modelNode2 = modelRuntime->rootModelNode;
        ModelNodeRuntime_SmoothYawTowardTarget(modelNode2,modelRuntime,0);
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNode2->childNodes[0],modelRuntime,0);
      }
    }
    else {
      modelNode2 = modelRuntime->rootModelNode;
      modelNodeRuntime = modelNode2->childNodes[0];
      SVar9 = ShotDefinition_ComputeLaunchAnglesRegs
                        (point0X,point0Y,point0Z,(modelNodeRuntime->worldTransform).translation.z,
                         (modelNodeRuntime->worldTransform).translation.y,
                         (modelNodeRuntime->worldTransform).translation.x,pAVar4->shotDefinition);
      MVar10 = ModelNodeRuntime_ComputeRelativeDirectionAngle
                         (modelNode2,SVar9.elevationAngle,SVar9.headingAngle);
      targetPitchAngle16 = MVar10.relativePitchAngle;
      MVar11 = ModelNodeRuntime_SmoothYawTowardTarget
                         (modelNode2,modelRuntime,MVar10.relativeYawAngle);
      if (MVar11.carry) {
        ModelNodeRuntime_SmoothPitchTowardTarget(modelNodeRuntime,modelRuntime,targetPitchAngle16);
      }
      else {
        MVar11 = ModelNodeRuntime_SmoothPitchTowardTarget
                           (modelNodeRuntime,modelRuntime,targetPitchAngle16);
        if (((MVar11.eax == targetPitchAngle16) &&
            (pAVar4 = modelRuntime->modelDefinition, modelRuntime->sharedInterShotTicks == 0)) &&
           (bVar8 = ArmyRuntimeCommand_UpdateTargetFollowingState
                              (point0X,point0Y,point0Z,worldRuntime,(ArmyRuntimeSlot *)modelRuntime)
           , !bVar8)) {
          modelNode2 = modelNodeRuntime->childNodes[0];
          pMVar5 = pAVar4->modelPointSource64;
          pMVar1 = &modelNode2->runtimeFlags;
          *pMVar1 = *pMVar1 | 1;
          pMVar5 = *(MdlSerializedNodeHeader38 **)(pMVar5->childSerializedOffsets[0] + 0x18);
          attachmentSelectorOrdinal = 0;
          do {
            if (modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] == 0) {
              modelRuntime->attachmentReloadTicks[attachmentSelectorOrdinal] =
                   pAVar4->attachmentReloadTicks;
              pAVar6 = modelRuntime->ownerArmyRuntime->commandTargetArmyRuntime;
              shotRuntimeState14 = 0;
              if (pAVar6 != (ArmyRuntimeSlot *)0x0) {
                shotRuntimeState14 = (pAVar6->modelRuntimeOrSavedOffset).savedIdOrOffset;
              }
              bVar8 = ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf
                                (shotRuntimeState14,point0X,point0Y,point0Z,
                                 attachmentSelectorOrdinal,pAVar4->shotDefinition,modelNode2,pMVar5,
                                 worldRuntime);
              if (!bVar8) {
                modelRuntime->sharedInterShotTicks = pAVar4->sharedInterShotTicks;
                ArmyRuntime_SetNonzeroActionVector
                          (SVar9.headingAngle,pAVar4->postLaunchVector1Q12,
                           pAVar4->postLaunchVector0Q12,modelRuntime->ownerArmyRuntime);
                pMVar3 = &(modelRuntime->rootModelNode->childNodes[0]->childNodes[0]->modelPayload).
                          meshGroupMask;
                *pMVar3 = *pMVar3 & -2 << ((byte)attachmentSelectorOrdinal & 0x1f);
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
  ModelDefinitionRuntimeSemanticView280 *pMVar1;
  ModelRuntimeNode *modelNodeRuntime;
  int iVar2;
  ModelRuntimeNode *modelNode1;
  
  pMVar1 = modelRuntime->modelDefinition;
  iVar2 = modelRuntime->ownerArmyRuntime->factionIndex * 0x740;
  if (*(int *)pMVar1->reserved0C0_0DB == 1) {
    iVar2 = iVar2 + 0x10;
  }
  modelNodeRuntime = modelRuntime->rootModelNode;
  modelNode1 = modelNodeRuntime->childNodes[0];
  if ((modelNodeRuntime->childCount != 0) && (modelNode1 != (ModelRuntimeNode *)0x0)) {
    (modelNode1->modelPayload).localTranslationZQ12 =
         (int)(((longlong)(int)(pMVar1->runtimeValue28 - pMVar1->runtimeValue24) *
               (longlong)
               *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar2 + -0x78)) /
              (longlong)*(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar2 + -0x74)
              ) + pMVar1->runtimeValue24;
    modelNode1->runtimeFlags = modelNode1->runtimeFlags | 1;
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
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
  Q12 *pQVar1;
  FactionRelationCounter *pFVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  bool bVar6;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    iVar3 = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
    pQVar1 = &armyRuntime->actionVector2Q12;
    iVar5 = *pQVar1;
    *pQVar1 = *pQVar1 - damageAmount;
    if (*pQVar1 == 0 || SBORROW4(iVar5,damageAmount) != *pQVar1 < 0) {
      iVar5 = armyRuntime->actionVector2Q12;
      armyRuntime->runtimeFlags = armyRuntime->runtimeFlags | 8;
      modelNode1 = armyRuntime->modelNodeRuntime->parentNode;
      armyRuntime->actionVector1Q12 = (Q12)armyRuntime;
      armyRuntime->actionVector2Q12 = 0;
      if (modelNode1 == (ModelRuntimeNode *)0x0) {
        bVar6 = false;
        if ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0) &&
           (bVar6 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime[1].classLinkState.
                    classState78 == 0, bVar6)) {
          (armyRuntime->modelNodeRuntime->modelPayload).worldRotationAngle0 = impactAngle;
        }
        iVar5 = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
        if ((!bVar6) &&
           (dVar4 = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C,
           pFVar2 = &g_GameFactionRuntimeImage.records[iVar5].relationCounterC,
           *pFVar2 = *pFVar2 + 1,
           g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[dVar4] ==
           ArmyRuntime_ClassCommandHandlerGroupACf)) {
          pFVar2 = &g_GameFactionRuntimeImage.records[iVar5].relationCounterD;
          *pFVar2 = *pFVar2 + 1;
          pFVar2 = &g_GameFactionRuntimeImage.records[iVar5].relationCounterC;
          *pFVar2 = *pFVar2 + -1;
        }
      }
      else {
        ArmyRuntime_ApplyDamageAndPropagateToParent(-iVar5,(modelNode1->runtimePayload).armyRuntime)
        ;
      }
    }
    else {
      iVar5 = iVar3 - armyRuntime->actionVector2Q12;
      if (iVar5 == 0 || iVar3 < armyRuntime->actionVector2Q12) {
        armyRuntime->actionVector2Q12 = armyRuntime->actionVector2Q12 + iVar5;
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
  Q12 *pQVar1;
  int iVar2;
  int iVar3;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    iVar2 = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
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
   [core/math/fixed], FixedMath_Atan2Angle16 [core/math/fixed], ModelRuntime_RaycastCandidateListNearestCf
   [world/model/runtime], FixedMath_VectorToAnglesAndLength3Regs [core/math/fixed],
   FieldGrid_RaycastTerrainSurfaceDistanceCf [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyWeaponRuntime_TestTargetLineOfFireCf
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *pMVar1;
  int *piVar2;
  longlong lVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  AngleTurn32 azimuthAngle;
  int x;
  AngleTurn32 elevationAngle;
  dword dVar7;
  int iVar8;
  uint uVar9;
  FixedLengthAngleEaxEdx8 FVar10;
  FieldGridRaycastEaxEdxCf9 FVar11;
  ModelRaycastNearestHitEaxEdxCf9 MVar12;
  FixedLengthAnglesEaxEcxEdx12 FVar13;
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
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  iVar4 = targetWorldXQ12 - (modelNode1->worldTransform).translation.x;
  iVar5 = *(int *)(pMVar1->reserved10_37 + 0x14);
  iVar8 = *(int *)(pMVar1->reserved10_37 + 0x18);
  iVar6 = targetWorldYQ12 - (modelNode1->worldTransform).translation.y;
  piVar2 = *(int **)(pMVar1->reserved10_37 + 0x1c);
  x = targetWorldZQ12 - (modelNode1->worldTransform).translation.z;
  if (*piVar2 == 1) {
    FVar10 = FixedMath_Vector2AngleAndLengthRegs(iVar6,iVar4);
    iVar6 = piVar2[3] * piVar2[3];
    iVar4 = FVar10.length * piVar2[0x37];
    lVar3 = (longlong)(iVar6 + piVar2[0x37] * x * -2) * (longlong)iVar6 -
            (longlong)iVar4 * (longlong)iVar4;
    if (lVar3 < 0) {
      return true;
    }
    dVar7 = FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar3 >> 0x20),(UInt64Half32)lVar3);
    if ((-0x1000 < x) && (x < 0x1000)) {
      dVar7 = -dVar7;
    }
    dVar7 = FixedMath_Atan2Angle16(iVar6 + dVar7,iVar4);
    if ((int)dVar7 < iVar5) {
      return true;
    }
    if (iVar8 < (int)dVar7) {
      return true;
    }
    modelNode1 = armyRuntime->modelNodeRuntime;
    MVar12 = ModelRuntime_RaycastCandidateListNearestCf
                       (dVar7,FVar10.angle & 0xffff,FVar10.length,
                        (modelNode1->worldTransform).translation.z,
                        (modelNode1->worldTransform).translation.y,
                        (modelNode1->worldTransform).translation.x,MODEL_RUNTIME_CLASS_00,
                        (armyRuntime->linkedEntityRuntime->common).ownership.modelNode,worldRuntime)
    ;
    if (!MVar12.carry) {
      return false;
    }
    entityRuntime2 = armyRuntime->linkedEntityRuntime;
    entityRuntime1 = ((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime;
    iVar5 = (entityRuntime2->common).ownership.ownerIndex;
    if ((entityRuntime2->common).commandState < 1) {
      if (iVar5 == (entityRuntime1->common).ownership.ownerIndex) {
        return false;
      }
    }
    else if (iVar5 != (entityRuntime1->common).ownership.ownerIndex) {
      return false;
    }
    if (entityRuntime1 == (entityRuntime2->common).commandTarget.targetEntity) {
      return false;
    }
    return true;
  }
  if (*piVar2 == 2) {
    return false;
  }
  FVar13 = FixedMath_VectorToAnglesAndLength3Regs(x,iVar6,iVar4);
  elevationAngle = FVar13.elevationAngle;
  azimuthAngle = FVar13.azimuthAngle;
  dVar7 = FVar13.lengthQ12;
  if (piVar2[0xa4] == 0) {
    if ((int)elevationAngle < iVar5) {
      return true;
    }
    if (iVar8 < (int)elevationAngle) {
      return true;
    }
  }
  excludedNode = (entityRuntime2->common).ownership.modelNode;
  requiredOwnerId = MODEL_RUNTIME_CLASS_00;
  originXQ12 = (modelNode1->worldTransform).translation.x;
  iVar8 = piVar2[3] * piVar2[0x34];
  originYQ12 = (modelNode1->worldTransform).translation.y;
  originZQ12 = (modelNode1->worldTransform).translation.z;
  FVar11 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                     (elevationAngle,azimuthAngle,iVar8,(modelNode1->worldTransform).translation.z,
                      (modelNode1->worldTransform).translation.y,
                      (modelNode1->worldTransform).translation.x,worldRuntime->fieldGrid);
  iVar5 = FVar11.distanceQ12;
  MVar12 = ModelRuntime_RaycastCandidateListNearestCf
                     (elevationAngle,azimuthAngle,iVar8,originZQ12,originYQ12,originXQ12,
                      requiredOwnerId,excludedNode,worldRuntime);
  if (!MVar12.carry) {
    if (0x7ffffffe < iVar5)
    goto ArmyWeaponRuntime_TestTargetLineOfFire_EvaluateTargetClearanceAgainstWeaponRange;
LAB_0052bca0:
    uVar9 = iVar5 - dVar7;
    if ((int)uVar9 < 0) {
      uVar9 = -uVar9;
    }
    if (((armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity ==
         (GameEntityRuntime *)0x0) && (uVar9 < 0x401)) {
      return false;
    }
    return true;
  }
  if (iVar5 < MVar12.nearestDistanceQ12) goto LAB_0052bca0;
  entityRuntime2 = armyRuntime->linkedEntityRuntime;
  entityRuntime1 =
       (((MVar12.edxCarrier.nearestModelNode)->runtimePayload).armyRuntime)->linkedEntityRuntime;
  iVar5 = (entityRuntime2->common).ownership.ownerIndex;
  if ((entityRuntime2->common).commandState < 1) {
    if (iVar5 == (entityRuntime1->common).ownership.ownerIndex)
    goto ArmyWeaponRuntime_TestTargetLineOfFire_EvaluateTargetClearanceAgainstWeaponRange;
  }
  else if (iVar5 != (entityRuntime1->common).ownership.ownerIndex)
  goto ArmyWeaponRuntime_TestTargetLineOfFire_EvaluateTargetClearanceAgainstWeaponRange;
  if (entityRuntime1 != (entityRuntime2->common).commandTarget.targetEntity) {
    return true;
  }
ArmyWeaponRuntime_TestTargetLineOfFire_EvaluateTargetClearanceAgainstWeaponRange:
  entityRuntime2 = (armyRuntime->linkedEntityRuntime->common).commandTarget.targetEntity;
  if (entityRuntime2 != (GameEntityRuntime *)0x0) {
    dVar7 = (int)(dVar7 * 2 -
                 *(int *)(*(int *)(entityRuntime2->common).ownership.definitionOrClassRecord + 0xdc)
                 ) >> 1;
  }
  if (piVar2[3] * ((piVar2[0x9c] * -0xaaa >> 0xc) + piVar2[0x34] + -1) < (int)dVar7) {
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
  Q12 *pQVar1;
  int iVar2;
  int iVar3;
  ModelRuntimeNode *modelNode1;
  
  armyRuntime->reservedF8_FF[0] = 0;
  armyRuntime->reservedF8_FF[1] = 2;
  armyRuntime->reservedF8_FF[2] = 0;
  armyRuntime->reservedF8_FF[3] = 0;
  if (0 < armyRuntime->actionVector2Q12) {
    iVar3 = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classLinkState).
            modelLinkOrState60.signedScalarState;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_EmitDamageThresholdEffect
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  dword dVar1;
  ModelPackedPointRecord *localPointRecord;
  dword dVar2;
  dword dVar3;
  dword worldXQ12;
  uint uVar4;
  dword worldZQ12;
  AngleTurn32 orientationAngle0;
  ModelLookupEntryEaxCf5 MVar5;
  ModelLocalPointRegs12 MVar6;
  EffectDefinition *effectDefinition;
  
  if ((armyRuntime->runtimeFlags & 0x210) != 0) {
    return;
  }
  dVar2 = (armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset;
  if (*(int *)(dVar2 + 0x60) < 1) {
    return;
  }
  if (*(int *)(dVar2 + 0x250) <= (armyRuntime->actionVector2Q12 * 100) / *(int *)(dVar2 + 0x60)) {
    return;
  }
  if (0 < armyRuntime->selectionMetric3) {
    armyRuntime->selectionMetric3 = armyRuntime->selectionMetric3 - g_InGameSimulationStepTicks;
    return;
  }
  uVar4 = 0;
  if (*(int *)(dVar2 + 0x25c) != 0) {
    dVar1 = (*g_RandomGeneratorState.next)();
    uVar4 = dVar1 % *(uint *)(dVar2 + 0x25c);
  }
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  armyRuntime->selectionMetric3 = uVar4 + *(int *)(dVar2 + 600);
  MVar5 = ModelLookupTable_ContainsPackedKeyCf
                    (armyRuntime->selectionMetric4,3,(modelNodeRuntime->modelPayload).modelResource)
  ;
  localPointRecord = MVar5.entry;
  if (MVar5.carry) {
    armyRuntime->selectionMetric4 = -1;
    MVar5 = ModelLookupTable_ContainsPackedKeyCf(0,3,(modelNodeRuntime->modelPayload).modelResource)
    ;
    localPointRecord = MVar5.entry;
    if (MVar5.carry) {
      dVar1 = (modelNodeRuntime->worldTransform).translation.x;
      worldXQ12 = (modelNodeRuntime->worldTransform).translation.y;
      worldZQ12 = (modelNodeRuntime->worldTransform).translation.z;
      goto LAB_005282dd;
    }
  }
  MVar6 = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNodeRuntime);
  worldZQ12 = MVar6.edx;
  worldXQ12 = MVar6.ecx;
  dVar1 = MVar6.eax;
  armyRuntime->selectionMetric4 = armyRuntime->selectionMetric4 + 1;
LAB_005282dd:
  effectDefinition = *(EffectDefinition **)(dVar2 + 0x254);
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar4 = dVar2 & 0xffff;
  dVar3 = (*g_RandomGeneratorState.next)();
  EffectRuntimePool_CreateInstanceFromDefinitionCf
            (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,dVar2 >> 0x10,
             (dVar3 & 0x1fff) + 0x1fff,uVar4,worldZQ12,worldXQ12,dVar1,effectDefinition,worldRuntime
            );
  return;
}

