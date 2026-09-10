#include <thandor/world/shots/maintenance.h>

/* Implementation ownership: world/shots/maintenance. */

/* Address: 0x0052C080.
   Ownership: world/shots/maintenance.
   Purpose: Table membership MAINTENANCE[4]. Classifies the shot model node against nearby terrain occupancy,
   refreshes runtime class flags and state tint, then composes the packed tint with the model-definition tint.
   Maintenance table phase terrainStateRefresh, object kind shot. The 4x3 table bytes, target body, calling
   convention, and RET 0x08 contract remain unchanged.
   Cross-module calls: TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy],
   FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], TerrainOccupancyMask_ResolveRuntimeClassFlags
   [world/terrain/occupancy], UiModelControl_RefreshStateTint [ui/controls/misc].
*/
void ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
               (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNode)

{
  PackedArgb32 PVar1;
  undefined4 uVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  uint uVar9;
  int extraout_ECX;
  int extraout_ECX_00;
  GameEntityRuntime *extraout_ECX_01;
  FieldGridRegionMask primaryOccupancyMask;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  undefined8 mm0PackedValue0;
  undefined1 uVar10;
  undefined1 uVar11;
  ulonglong uVar12;
  FixedDirectionXZEdxEax8 FVar13;
  ArmyRuntimeSlot *armySlot1;
  
  armySlot1 = (modelNode->runtimePayload).armyRuntime;
  uVar12 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (0x1000,(modelNode->worldTransform).translation.y,
                      (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  primaryOccupancyMask = (FieldGridRegionMask)(uVar12 >> 0x20);
  if ((*(int *)armySlot1->definitionOrAsset == 0) && (modelNode->renderDepthBiasOrState != 0)) {
    FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState >> 1);
    uVar12 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                       (0x1000,extraout_ECX + (modelNode->worldTransform).translation.y,
                        (int)FVar13 + (modelNode->worldTransform).translation.x,
                        worldRuntime->fieldGrid);
    uVar9 = primaryOccupancyMask | (uint)(uVar12 >> 0x20);
    FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState);
    uVar12 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                       (0x1000,extraout_ECX_00 + (modelNode->worldTransform).translation.y,
                        (int)FVar13 + (modelNode->worldTransform).translation.x,
                        worldRuntime->fieldGrid);
    primaryOccupancyMask = (uint)(uVar12 >> 0x20) | uVar9;
  }
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffff3;
  uVar9 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNode->runtimeFlags,0,primaryOccupancyMask,
                     (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | uVar9;
  armySlot1->linkedEntityRuntime = extraout_ECX_01;
  UiModelControl_RefreshStateTint(modelNode);
  PVar1 = modelNode->tintArgb;
  uVar2 = *(undefined4 *)((int)armySlot1->definitionOrAsset + 0x284);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
  uVar7 = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
  uVar10 = (undefined1)((uint)uVar2 >> 0x18);
  uVar8 = CONCAT11(uVar10,uVar10);
  uVar11 = (undefined1)((uint)uVar2 >> 0x10);
  uVar10 = (undefined1)((uint)uVar2 >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(uVar7 >> 4,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,PVar1)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar1,(char)PVar1) >> 4))),
              CONCAT26(uVar8 >> 4,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar11),CONCAT14(uVar11,uVar2)) >>
                                        0x20) >> 4,
                                CONCAT22(CONCAT11(uVar10,uVar10) >> 4,
                                         CONCAT11((char)uVar2,(char)uVar2) >> 4))));
  sVar3 = (short)mm0PackedValue0;
  sVar4 = (short)((ulonglong)mm0PackedValue0 >> 0x10);
  sVar5 = (short)((ulonglong)mm0PackedValue0 >> 0x20);
  sVar6 = (short)((ulonglong)mm0PackedValue0 >> 0x30);
  modelNode->tintArgb =
       CONCAT13((0 < sVar6) * (sVar6 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x30) -
                (0xff < sVar6),
                CONCAT12((0 < sVar5) * (sVar5 < 0x100) * (char)((ulonglong)mm0PackedValue0 >> 0x20)
                         - (0xff < sVar5),
                         CONCAT11((0 < sVar4) * (sVar4 < 0x100) *
                                  (char)((ulonglong)mm0PackedValue0 >> 0x10) - (0xff < sVar4),
                                  (0 < sVar3) * (sVar3 < 0x100) * (char)mm0PackedValue0 -
                                  (0xff < sVar3))));
  return;
}

/* Address: 0x0052C1A0.
   Ownership: world/shots/maintenance.
   Purpose: Two-argument no-op callback selected from the mixed army, shot, and effect runtime dispatch table for
   the shot-runtime class. Maintenance slot 7 passes two stack arguments and the exact body returns with RET 0x08;
   the callback intentionally ignores WorldRuntimeContext and the runtime object. Maintenance table phase
   occupancyRebuild, object kind shot. The 4x3 table bytes, target body, calling convention, and RET 0x08 contract
   remain unchanged.
*/
void ShotRuntimeMaintenance_OccupancyRebuildNoOp
               (WorldRuntimeContext *worldRuntime,void *runtimeObject)

{
  return;
}

/* Address: 0x0052C1B0.
   Ownership: world/shots/maintenance.
   Purpose: Binary entry is anchored by g_ArmyRuntimeCallbackTable12[10]@00562DEC. Maintenance table phase
   audioRefresh, object kind shot. Maintenance audioRefresh.shot callback. The second argument is the
   ModelRuntimeNode whose runtimePayload and world transform drive projected sound gain updates. The 4x3 table
   bytes, target body, calling convention, and RET 0x08 contract remain unchanged.
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
               (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime)

{
  uint uVar1;
  void *pvVar2;
  bool bVar3;
  undefined8 uVar4;
  ArmyRuntimeSlot *armySlot1;
  
  armySlot1 = (modelNodeRuntime->runtimePayload).armyRuntime;
  if ((worldRuntime->dwordArray != (dword *)0x0) &&
     (uVar1 = *(uint *)((int)armySlot1->definitionOrAsset + 0x280),
     uVar1 < worldRuntime->dwordArrayCount)) {
    bVar3 = false;
    if (worldRuntime->dwordArray[uVar1] != 0) {
      uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((modelNodeRuntime->worldTransform).translation.y,
                         (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
      if (!bVar3) {
        pvVar2 = armySlot1->definitionOrAsset;
        SpatialSound_UpdateDesiredPositionedGains
                  (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x28c),
                   *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x288),
                   (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
      }
    }
  }
  return;
}

/* Address: 0x0052C230.
   Ownership: world/shots/maintenance.
   Purpose: Maintenance slot 1 receives WorldRuntimeContext and ModelRuntimeNode and advances the linked shot
   runtime payload through timers, animation/model state, collision and effect transitions. The exact fifteen-range
   body and five RET 0x08 exits are sealed against the immutable original binary. The slot-based name remains
   conservative. Advances projectile animation and motion, resolves terrain, secondary-surface and runtime
   collisions, emits impact effects, applies army-hit relation logic and damage, and unlinks completed projectile
   nodes. Maintenance table phase primaryUpdate, object kind shot.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime],
   ModelRuntime_RaycastCandidateListNearestCf [world/model/runtime], FieldGrid_RaycastTerrainSurfaceDistanceCf
   [world/terrain/grid], FieldGrid_RaycastSecondarySurfaceDistanceCf [world/terrain/grid].
*/
void ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
               (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNode)

{
  ShotAnimationFrameIndex *pSVar1;
  ShotLifetimeRemainingTicks *pSVar2;
  ShotSecondaryEffectCountdownTicks *pSVar3;
  Q12 *pQVar4;
  ModelTextureSubresourceIndex *pMVar5;
  GraphicsFixedVec3 *pGVar6;
  GraphicsWorldCoordinateQ12 *pGVar7;
  AngleTurn32 *pAVar8;
  ShotFrameAdvanceThresholdQ4 SVar9;
  int *piVar10;
  longlong lVar11;
  ModelRaycastNearestHitCfRegisterResult MVar12;
  uint uVar13;
  ModelPackedPointRecord *localPointRecord;
  Q12 QVar14;
  dword mixedScalarOrPointerCarrier;
  ShotDefinition *shotDefinition2;
  uint uVar15;
  int iVar16;
  int extraout_EAX;
  FixedMathScale32 scale;
  int extraout_EAX_00;
  ModelRuntimeNode *modelNode1;
  ShotDefinition *shotDefinition1;
  int extraout_EAX_01;
  undefined4 extraout_ECX;
  Q12 worldXQ12;
  dword extraout_ECX_00;
  int extraout_ECX_17;
  ShotDefinition *shotDefinition4;
  dword extraout_ECX_02;
  undefined4 extraout_ECX_18;
  int extraout_ECX_19;
  int extraout_ECX_20;
  int extraout_ECX_21;
  int extraout_ECX_22;
  int extraout_ECX_23;
  int extraout_ECX_24;
  int extraout_ECX_25;
  int extraout_ECX_26;
  dword extraout_ECX_12;
  int extraout_ECX_27;
  dword extraout_ECX_14;
  int extraout_ECX_28;
  dword extraout_ECX_16;
  undefined4 worldZQ12;
  int extraout_EDX;
  ShotDefinition *shotDefinition3;
  FixedMathScale32 scale_00;
  int iVar17;
  int extraout_EDX_00;
  FixedMathScale32 scale_01;
  int iVar18;
  Q12 extraout_EDX_01;
  FactionRuntimeIndex FVar19;
  ShotRuntimeSlot *shotRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  bool bVar20;
  undefined8 uVar21;
  ModelRaycastNearestHitCfRegisterResult MVar22;
  ulonglong uVar23;
  FixedLengthElevationEdxEax8 FVar24;
  FixedDirectionXZEdxEax8 FVar25;
  ModelRaycastNearestHitCfRegisterResult MVar26;
  AngleTurn32 AVar27;
  ImpactDamageValue32 impactValue;
  InGameSimulationStepBatchTicks IStack_38;
  int iStack_24;
  ArmyRuntimeSlot *armySlot1;
  ArmyRuntimeSlot *armySlot2;
  EffectDefinition *effectDefinition1;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  WorldRuntimeContext *worldContext1;
  
  IStack_38 = g_InGameSimulationStepTicks;
  do {
    shotRuntime = (modelNode->runtimePayload).shotRuntime;
    shotDefinition3 = (shotRuntime->definitionOrSavedId).definition;
    shotRuntime->projectileAgeTicks = shotRuntime->projectileAgeTicks + 1;
    uVar13 = shotRuntime->animationFrameAccumulatorQ4 + 0x10;
    uVar15 = shotDefinition3->animationFrameCount;
    shotRuntime->animationFrameAccumulatorQ4 = uVar13;
    bVar20 = uVar13 < shotDefinition3->animationFrameAdvanceThresholdQ4;
    SVar9 = shotDefinition3->animationFrameAdvanceThresholdQ4;
    if (!bVar20) {
      pSVar1 = &(shotRuntime->ownerAndTrajectory).animationFrameIndex;
      *pSVar1 = *pSVar1 + 1;
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      shotRuntime->animationFrameAccumulatorQ4 = uVar13 - SVar9;
      uVar13 = (shotRuntime->ownerAndTrajectory).animationFrameIndex;
      bVar20 = uVar15 < uVar13;
      if (uVar15 <= uVar13) {
        pSVar1 = &(shotRuntime->ownerAndTrajectory).animationFrameIndex;
        *pSVar1 = *pSVar1 - uVar15;
        pMVar5 = &modelNode->textureSubresourceBaseIndex;
        bVar20 = (uint)*pMVar5 < uVar15;
        *pMVar5 = *pMVar5 - uVar15;
      }
    }
    pSVar2 = &shotRuntime->lifetimeTicksRemaining;
    *pSVar2 = *pSVar2 - 1;
    modelNodeRuntime = modelNode;
    if (*pSVar2 == 0)
    goto 
    ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_UnlinkExpiredOrOrphanedProjectileAndReturn
    ;
    pSVar3 = &(shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks;
    *pSVar3 = *pSVar3 - 1;
    if (*pSVar3 == 0) {
      (shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks =
           shotDefinition3->secondaryEffectIntervalTicks;
      localPointRecord =
           (ModelPackedPointRecord *)
           ModelLookupTable_ContainsPackedKeyCf(1,3,shotDefinition3->ownedNestedResource);
      if (!bVar20) {
        effectDefinition1 = shotDefinition3->secondaryEffectDefinition;
        worldContext1 = worldRuntime;
        uVar21 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX,localPointRecord,modelNode);
        worldZQ12 = (undefined4)((ulonglong)uVar21 >> 0x20);
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (worldXQ12,worldZQ12,EFFECT_RUNTIME_COMPLETION_NONE,0,0,0x4000,0,worldZQ12,
                   worldXQ12,(Q12)uVar21,effectDefinition1,worldContext1);
      }
    }
    if (shotDefinition3->trajectoryMode == SHOT_TRAJECTORY_DIRECT_LINE) {
      armySlot2 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      shotDefinition3 =
           (ShotDefinition *)
           (shotDefinition3->projectileLifetimeTicks * shotDefinition3->launchSpeedQ12);
      uVar15 = (modelNode->modelPayload).worldRotationAngle1;
      modelNode->renderDepthBiasOrState = (int)shotDefinition3;
      modelNode1 = (ModelRuntimeNode *)0x0;
      if (armySlot2 != (ArmyRuntimeSlot *)0x0) {
        modelNode1 = armySlot2->modelNodeRuntime;
      }
      bVar20 = uVar15 < (uint)shotRuntime->elevationOffsetAngle16;
      MVar26 = ModelRuntime_RaycastCandidateListNearestCf
                         (uVar15 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,(Q12)shotDefinition3,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,MODEL_RUNTIME_CLASS_00,
                          modelNode1,worldRuntime);
      QVar14 = MVar26.nearestDistanceQ12;
      if (bVar20) {
        iStack_24 = *(int *)((int)(((MVar26.nearestModelNode)->runtimePayload).armyRuntime)->
                                  definitionOrAsset + 0x5c);
        if (((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
            [iStack_24] == (EffectDefinition *)0x0) {
          QVar14 = 0x7fffffff;
        }
        MVar26.nearestModelNode = MVar26.nearestModelNode;
        MVar26.nearestDistanceQ12 = QVar14;
      }
      modelNode1 = MVar26.nearestModelNode;
      shotDefinition1 = (ShotDefinition *)MVar26.nearestDistanceQ12;
      uVar15 = (modelNode->modelPayload).worldRotationAngle1;
      bVar20 = uVar15 < (uint)shotRuntime->elevationOffsetAngle16;
      uVar23 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                         (uVar15 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,(Q12)shotDefinition3,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      iVar16 = (int)(uVar23 >> 0x20);
      mixedScalarOrPointerCarrier = (dword)uVar23;
      if ((bVar20) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[iVar16]
          == (EffectDefinition *)0x0)) {
        mixedScalarOrPointerCarrier = 0x7fffffff;
      }
      uVar15 = (modelNode->modelPayload).worldRotationAngle1;
      bVar20 = uVar15 < (uint)shotRuntime->elevationOffsetAngle16;
      uVar23 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                         (uVar15 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,(Q12)shotDefinition3,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      shotDefinition2 = (ShotDefinition *)uVar23;
      if ((bVar20) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition ==
          (EffectDefinition *)0x0)) {
        shotDefinition2 = (ShotDefinition *)0x7fffffff;
      }
      if (shotDefinition2 < mixedScalarOrPointerCarrier) {
        if (shotDefinition2 < shotDefinition1) {
          mixedScalarOrPointerCarrier = (dword)(shotRuntime->definitionOrSavedId).definition;
          if ((shotDefinition2 <= shotDefinition3) &&
             (modelNode->renderDepthBiasOrState = (int)shotDefinition2,
             (shotRuntime->impactEffectEmissionFlags & 1) == 0)) {
            shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
            effectDefinition1 =
                 ((ShotDefinition *)mixedScalarOrPointerCarrier)->primaryEffectDefinition;
            worldContext1 = worldRuntime;
            FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,
                                (FixedMathScale32)shotDefinition2);
            iVar16 = extraout_ECX_27 + (modelNode->worldTransform).translation.y;
            iVar17 = (int)(FVar25 >> 0x20) + (modelNode->worldTransform).translation.z;
            uVar21 = EffectRuntimePool_CreateInstanceFromDefinitionCf
                               (iVar16,iVar17,EFFECT_RUNTIME_COMPLETION_NONE,0,0,0x4000,0,iVar17,
                                iVar16,(int)FVar25 + (modelNode->worldTransform).translation.x,
                                effectDefinition1,worldContext1);
            MVar26.nearestModelNode = modelNode1;
            MVar26.nearestDistanceQ12 = (Q12)((ulonglong)uVar21 >> 0x20);
            mixedScalarOrPointerCarrier = extraout_ECX_14;
          }
        }
        else {

          ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndContinueMotion
          :
          mixedScalarOrPointerCarrier = *(dword *)&shotRuntime->definitionOrSavedId;
          if (shotDefinition1 <= shotDefinition3) {
            modelNode->renderDepthBiasOrState = (int)shotDefinition1;
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      ((modelNode1->runtimePayload).armyRuntime,shotRuntime);
            armySlot2 = *(ArmyRuntimeSlot **)(extraout_EAX_01 + 0x48);
            armySlot1 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            FVar19 = 0;
            if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
              FVar19 = armySlot1->factionIndex;
            }
            LOCK();
            UNLOCK();
            AVar27 = (modelNode->modelPayload).worldRotationAngle0;
            iVar16 = *(int *)(extraout_ECX_25 + 0xb0 + iStack_24 * 4) /
                     *(int *)(extraout_ECX_25 + 0xd0);
            LOCK();
            UNLOCK();
            effectDefinition1 = *(EffectDefinition **)(extraout_ECX_25 + 0x90 + iStack_24 * 4);
            if (((shotRuntime->impactEffectEmissionFlags & 1) == 0) &&
               (effectDefinition1 != (EffectDefinition *)0x0)) {
              shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
              worldContext1 = worldRuntime;
              FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,scale_01);
              iVar18 = (int)(FVar25 >> 0x20) + (modelNode->worldTransform).translation.z;
              iVar17 = -(modelNode->modelPayload).worldRotationAngle1;
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (iVar17,iVar18,EFFECT_RUNTIME_COMPLETION_NONE,0,0,iVar17,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,iVar18,
                         extraout_ECX_26 + (modelNode->worldTransform).translation.y,
                         (int)FVar25 + (modelNode->worldTransform).translation.x,effectDefinition1,
                         worldContext1);
            }
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AVar27,FVar19,iVar16,armySlot2);
            MVar26.nearestModelNode = modelNode1;
            MVar26.nearestDistanceQ12 = extraout_EDX_01;
            mixedScalarOrPointerCarrier = extraout_ECX_12;
          }
        }
      }
      else {
        if (shotDefinition1 <= mixedScalarOrPointerCarrier)
        goto 
        ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndContinueMotion
        ;
        shotDefinition1 = (shotRuntime->definitionOrSavedId).definition;
        MVar12.nearestModelNode = modelNode1;
        MVar12.nearestDistanceQ12 = iVar16;
        MVar26.nearestModelNode = modelNode1;
        MVar26.nearestDistanceQ12 = iVar16;
        if ((mixedScalarOrPointerCarrier <= shotDefinition3) &&
           (modelNode->renderDepthBiasOrState = mixedScalarOrPointerCarrier, MVar26 = MVar12,
           (shotRuntime->impactEffectEmissionFlags & 1) == 0)) {
          shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
          effectDefinition1 = shotDefinition1->terrainImpactEffectDefinitions31[iVar16];
          worldContext1 = worldRuntime;
          FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,
                              mixedScalarOrPointerCarrier);
          iVar17 = extraout_ECX_28 + (modelNode->worldTransform).translation.y;
          iVar18 = (int)(FVar25 >> 0x20) + (modelNode->worldTransform).translation.z;
          uVar21 = EffectRuntimePool_CreateInstanceFromDefinitionCf
                             (iVar17,iVar18,EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                              shotDefinition1->terrainImpactEffectOwnerSlots31 + iVar16,0,0x4000,0,
                              iVar18,iVar17,(int)FVar25 + (modelNode->worldTransform).translation.x,
                              effectDefinition1,worldContext1);
          MVar26.nearestDistanceQ12 = (Q12)((ulonglong)uVar21 >> 0x20);
          shotRuntime = (modelNode->runtimePayload).shotRuntime;
          mixedScalarOrPointerCarrier = extraout_ECX_16;
        }
      }
      shotDefinition3 = (ShotDefinition *)MVar26.nearestDistanceQ12;
      iVar16 = ((shotRuntime->definitionOrSavedId).definition)->modelSpinStepTurn16;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
      pAVar8 = &(modelNode->modelPayload).worldRotationAngle2;
      *pAVar8 = *pAVar8 + iVar16;
      pAVar8 = &(modelNode->modelPayload).worldRotationAngle2;
      *pAVar8 = *pAVar8 & 0xffff;
    }
    else {
      armySlot2 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      uVar15 = (modelNode->modelPayload).worldRotationAngle1;
      modelNode1 = (ModelRuntimeNode *)0x0;
      if (armySlot2 != (ArmyRuntimeSlot *)0x0) {
        modelNode1 = armySlot2->modelNodeRuntime;
      }
      bVar20 = uVar15 < (uint)shotRuntime->elevationOffsetAngle16;
      MVar22 = ModelRuntime_RaycastCandidateListNearestCf
                         (uVar15 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,MODEL_RUNTIME_CLASS_00,
                          modelNode1,worldRuntime);
      QVar14 = MVar22.nearestDistanceQ12;
      if (bVar20) {
        iStack_24 = *(int *)((int)(((MVar22.nearestModelNode)->runtimePayload).armyRuntime)->
                                  definitionOrAsset + 0x5c);
        if (((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
            [iStack_24] == (EffectDefinition *)0x0) {
          QVar14 = 0x7fffffff;
        }
        MVar22.nearestModelNode = MVar22.nearestModelNode;
        MVar22.nearestDistanceQ12 = QVar14;
      }
      shotDefinition1 = (ShotDefinition *)MVar22.nearestDistanceQ12;
      uVar15 = (modelNode->modelPayload).worldRotationAngle1;
      bVar20 = uVar15 < (uint)shotRuntime->elevationOffsetAngle16;
      uVar23 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                         (uVar15 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      shotDefinition3 = (ShotDefinition *)(uVar23 >> 0x20);
      mixedScalarOrPointerCarrier = (dword)uVar23;
      if ((bVar20) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31
          [(int)shotDefinition3] == (EffectDefinition *)0x0)) {
        mixedScalarOrPointerCarrier = 0x7fffffff;
      }
      uVar15 = (modelNode->modelPayload).worldRotationAngle1;
      bVar20 = uVar15 < (uint)shotRuntime->elevationOffsetAngle16;
      uVar23 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                         (uVar15 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      shotDefinition2 = (ShotDefinition *)uVar23;
      if ((bVar20) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition ==
          (EffectDefinition *)0x0)) {
        shotDefinition2 = (ShotDefinition *)0x7fffffff;
      }
      if (shotDefinition2 < mixedScalarOrPointerCarrier) {
        if (shotDefinition2 < shotDefinition1) {
          mixedScalarOrPointerCarrier = (dword)(shotRuntime->definitionOrSavedId).definition;
          shotDefinition3 = shotDefinition1;
          if (shotDefinition2 <= (ShotDefinition *)shotRuntime->launchSpeedQ12) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (((ShotDefinition *)mixedScalarOrPointerCarrier)->
                       shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            effectDefinition1 = *(EffectDefinition **)(extraout_ECX_22 + 0x10);
            FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,scale);
            iVar16 = extraout_ECX_23 + (modelNode->worldTransform).translation.y;
            iVar17 = (int)(FVar25 >> 0x20) + (modelNode->worldTransform).translation.z;
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (iVar16,iVar17,EFFECT_RUNTIME_COMPLETION_NONE,0,0,0x4000,0,iVar17,iVar16,
                       (int)FVar25 + (modelNode->worldTransform).translation.x,effectDefinition1,
                       worldRuntime);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            return;
          }
        }
        else {

          ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndTerminateProjectile
          :
          mixedScalarOrPointerCarrier = *(dword *)&shotRuntime->definitionOrSavedId;
          shotDefinition3 = shotDefinition1;
          if (shotDefinition1 <= (ShotDefinition *)shotRuntime->launchSpeedQ12) {
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((MVar22.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (*(GraphicsTransitionTickCount *)(extraout_ECX_19 + 0x264),
                       modelNode->shadingRecord);
            armySlot2 = *(ArmyRuntimeSlot **)(extraout_EAX + 0x48);
            impactValue = *(ImpactDamageValue32 *)(extraout_ECX_20 + 0xb0 + iStack_24 * 4);
            armySlot1 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            FVar19 = 0;
            if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
              FVar19 = armySlot1->factionIndex;
            }
            LOCK();
            UNLOCK();
            effectDefinition1 = *(EffectDefinition **)(extraout_ECX_20 + 0x90 + iStack_24 * 4);
            AVar27 = (modelNode->modelPayload).worldRotationAngle0;
            if (effectDefinition1 != (EffectDefinition *)0x0) {
              FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,scale_00);
              iVar17 = (int)(FVar25 >> 0x20) + (modelNode->worldTransform).translation.z;
              iVar16 = -(modelNode->modelPayload).worldRotationAngle1;
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (iVar16,iVar17,EFFECT_RUNTIME_COMPLETION_NONE,0,0,iVar16,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,iVar17,
                         extraout_ECX_21 + (modelNode->worldTransform).translation.y,
                         (int)FVar25 + (modelNode->worldTransform).translation.x,effectDefinition1,
                         worldRuntime);
            }
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AVar27,FVar19,impactValue,armySlot2);
            return;
          }
        }
      }
      else {
        if (shotDefinition1 <= mixedScalarOrPointerCarrier)
        goto 
        ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndTerminateProjectile
        ;
        if (mixedScalarOrPointerCarrier <= (ShotDefinition *)shotRuntime->launchSpeedQ12) {
          (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
          InterpolationState_SetNegatedTargetAndRescaleProgress
                    (((shotRuntime->definitionOrSavedId).definition)->
                     shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
          effectDefinition1 = *(EffectDefinition **)(extraout_EAX_00 + 0x14 + extraout_EDX_00 * 4);
          FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,
                              mixedScalarOrPointerCarrier);
          iVar16 = extraout_ECX_24 + (modelNode->worldTransform).translation.y;
          iVar17 = (int)(FVar25 >> 0x20) + (modelNode->worldTransform).translation.z;
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (iVar16,iVar17,EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     extraout_EAX_00 + 0xe0 + extraout_EDX_00 * 4,0,0x4000,0,iVar17,iVar16,
                     (int)FVar25 + (modelNode->worldTransform).translation.x,effectDefinition1,
                     worldRuntime);
          WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNode);
          return;
        }
      }
      shotDefinition1 = (shotRuntime->definitionOrSavedId).definition;
      if (shotDefinition1->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        FVar25 = FixedMath_DirectionFromAnglesScaledRegs
                           ((modelNode->modelPayload).worldRotationAngle1 -
                            shotRuntime->elevationOffsetAngle16,
                            (modelNode->modelPayload).worldRotationAngle0,
                            shotRuntime->launchSpeedQ12);
        shotDefinition3 = (ShotDefinition *)(FVar25 >> 0x20);
        shadingRecord1 = modelNode->shadingRecord;
        pGVar6 = &(modelNode->worldTransform).translation;
        pGVar6->x = pGVar6->x + (int)FVar25;
        if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
          shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + (int)FVar25;
          shadingRecord1->worldYQ12 = shadingRecord1->worldYQ12 + extraout_ECX_00;
          shadingRecord1->worldZQ12 =
               (int)shotDefinition3->terrainImpactEffectDefinitions31 +
               shadingRecord1->worldZQ12 + -0x14;
        }
        pGVar7 = &(modelNode->worldTransform).translation.y;
        *pGVar7 = *pGVar7 + extraout_ECX_00;
        shotDefinition1 = (shotRuntime->definitionOrSavedId).definition;
        pGVar7 = &(modelNode->worldTransform).translation.z;
        *pGVar7 = (int)shotDefinition3->terrainImpactEffectDefinitions31 + *pGVar7 + -0x14;
        piVar10 = (shotRuntime->runtimeStateOrSavedOffset).runtimeStatePointer;
        mixedScalarOrPointerCarrier = extraout_ECX_00;
        if ((shotDefinition1->mode3LeadDisabled != 0) && (piVar10 != (int *)0x0)) {
          iVar16 = piVar10[1];
          if (*(int *)(*piVar10 + 0x4c) == 0x15) {
            iVar16 = *(int *)(iVar16 + 0xcc);
          }
          modelNodeRuntime = (shotRuntime->modelNodeOrSavedOffset).modelNode;
          FixedMath_VectorToAngles3Regs
                    ((*(int *)(*piVar10 + 0x50) + *(int *)(iVar16 + 0x9c)) -
                     (modelNodeRuntime->worldTransform).translation.z,
                     *(int *)(iVar16 + 0x98) - (modelNodeRuntime->worldTransform).translation.y,
                     *(int *)(iVar16 + 0x94) - (modelNodeRuntime->worldTransform).translation.x);
          shotDefinition3 =
               (ShotDefinition *)
               (extraout_EDX - (modelNodeRuntime->modelPayload).worldRotationAngle1);
          shotDefinition2 = (ShotDefinition *)shotDefinition1->mode3LeadDisabled;
          shotDefinition4 =
               (ShotDefinition *)
               ((int)((extraout_ECX_17 - (modelNodeRuntime->modelPayload).worldRotationAngle0) *
                     0x10000) >> 0x10);
          if ((int)shotDefinition2 < (int)shotDefinition3) {
            shotDefinition3 = shotDefinition2;
          }
          if ((int)shotDefinition2 < (int)shotDefinition4) {
            shotDefinition4 = shotDefinition2;
          }
          shotDefinition2 = (ShotDefinition *)-(int)shotDefinition2;
          if ((int)shotDefinition4 < (int)shotDefinition2) {
            shotDefinition4 = shotDefinition2;
          }
          if ((int)shotDefinition3 < (int)shotDefinition2) {
            shotDefinition3 = shotDefinition2;
          }
          pAVar8 = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
          *pAVar8 = (int)shotDefinition3->terrainImpactEffectDefinitions31 + (*pAVar8 - 0x14);
          mixedScalarOrPointerCarrier =
               (int)shotDefinition4->terrainImpactEffectDefinitions31 +
               ((modelNodeRuntime->modelPayload).worldRotationAngle0 - 0x14) & 0xffff;
          (modelNodeRuntime->modelPayload).worldRotationAngle0 = mixedScalarOrPointerCarrier;
        }
      }
      iVar16 = shotDefinition1->modelSpinStepTurn16;
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      pAVar8 = &(modelNodeRuntime->modelPayload).worldRotationAngle2;
      *pAVar8 = *pAVar8 + iVar16;
      pAVar8 = &(modelNodeRuntime->modelPayload).worldRotationAngle2;
      *pAVar8 = *pAVar8 & 0xffff;
      if (shotDefinition1->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        if (shotDefinition1->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) {
          if (shotDefinition1->trajectoryRampDurationTicks != 0) {
            uVar15 = shotRuntime->projectileAgeTicks;
            if (shotDefinition1->trajectoryRampDurationTicks < uVar15) {
              uVar15 = shotDefinition1->trajectoryRampDurationTicks;
            }
            lVar11 = (longlong)
                     (int)(((longlong)(int)uVar15 * (longlong)(int)uVar15) /
                          (longlong)(int)shotDefinition1->trajectoryRampDurationTicks) *
                     (longlong)shotDefinition1->launchSpeedQ12;
            shotDefinition3 =
                 (ShotDefinition *)
                 (lVar11 % (longlong)(int)shotDefinition1->trajectoryRampDurationTicks);
            shotRuntime->launchSpeedQ12 =
                 (Q12)(lVar11 / (longlong)(int)shotDefinition1->trajectoryRampDurationTicks);
            if (shotDefinition1->elevationOffsetAngle16 != 0) {
              uVar15 = shotDefinition1->trajectoryRampDurationTicks * 2;
              iVar16 = uVar15 - shotRuntime->projectileAgeTicks;
              if (uVar15 < shotRuntime->projectileAgeTicks) {
                iVar16 = 0;
              }
              lVar11 = (longlong)iVar16 * (longlong)shotDefinition1->elevationOffsetAngle16;
              shotDefinition3 =
                   (ShotDefinition *)
                   (lVar11 % (longlong)(int)shotDefinition1->trajectoryRampDurationTicks);
              iVar16 = ((int)(lVar11 / (longlong)(int)shotDefinition1->trajectoryRampDurationTicks)
                       >> 1) - shotRuntime->elevationOffsetAngle16;
              shotRuntime->elevationOffsetAngle16 = shotRuntime->elevationOffsetAngle16 + iVar16;
              pAVar8 = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
              *pAVar8 = *pAVar8 + iVar16;
            }
          }
        }
        else if (shotDefinition1->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
          pQVar4 = &(shotRuntime->ownerAndTrajectory).directionComponent2Q12;
          *pQVar4 = *pQVar4 - ((shotRuntime->definitionOrSavedId).definition)->ballisticDivisorQ12;
          FVar24 = FixedMath_VectorToAnglesAndLengthVec3Regs
                             ((GraphicsFixedVec3 *)
                              &(shotRuntime->ownerAndTrajectory).directionComponent0Q12);
          shotDefinition3 = (ShotDefinition *)(FVar24 >> 0x20);
          shotRuntime->launchSpeedQ12 = (Q12)FVar24;
          (modelNodeRuntime->modelPayload).worldRotationAngle0 = extraout_ECX_02;
          (modelNodeRuntime->modelPayload).worldRotationAngle1 = (AngleTurn32)shotDefinition3;
          mixedScalarOrPointerCarrier = extraout_ECX_02;
        }
        else if (shotDefinition1->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
          if ((int)(modelNodeRuntime->modelPayload).worldRotationAngle1 < 0) {
            mixedScalarOrPointerCarrier = (shotRuntime->runtimeStateOrSavedOffset).runtimeState;
            shadingRecord1 = modelNodeRuntime->shadingRecord;
            if ((ShotDefinition *)mixedScalarOrPointerCarrier != (ShotDefinition *)0x0) {
              iVar16 = *(int *)(((ShotDefinition *)mixedScalarOrPointerCarrier)->reservedDword04 +
                               0x94) - (modelNodeRuntime->worldTransform).translation.x;
              shotDefinition3 =
                   (ShotDefinition *)
                   (*(int *)(((ShotDefinition *)mixedScalarOrPointerCarrier)->reservedDword04 + 0x98
                            ) - (modelNodeRuntime->worldTransform).translation.y);
              pGVar6 = &(modelNodeRuntime->worldTransform).translation;
              pGVar6->x = pGVar6->x + iVar16;
              pGVar7 = &(modelNodeRuntime->worldTransform).translation.y;
              *pGVar7 = (int)shotDefinition3->terrainImpactEffectDefinitions31 + *pGVar7 + -0x14;
              if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
                shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + iVar16;
                shadingRecord1->worldYQ12 =
                     (int)shotDefinition3->terrainImpactEffectDefinitions31 +
                     shadingRecord1->worldYQ12 + -0x14;
              }
            }
          }
          else {
            if (shotDefinition1->trajectoryRampDurationTicks != 0) {
              uVar15 = shotRuntime->projectileAgeTicks;
              if (shotDefinition1->trajectoryRampDurationTicks < uVar15) {
                uVar15 = shotDefinition1->trajectoryRampDurationTicks;
              }
              lVar11 = (longlong)
                       (int)(((longlong)
                              (int)(((longlong)(int)uVar15 * (longlong)(int)uVar15) /
                                   (longlong)(int)shotDefinition1->trajectoryRampDurationTicks) *
                             (longlong)(int)uVar15) /
                            (longlong)(int)shotDefinition1->trajectoryRampDurationTicks) *
                       (longlong)shotDefinition1->launchSpeedQ12;
              shotDefinition3 =
                   (ShotDefinition *)
                   (lVar11 % (longlong)(int)shotDefinition1->trajectoryRampDurationTicks);
              shotRuntime->launchSpeedQ12 =
                   (Q12)(lVar11 / (longlong)(int)shotDefinition1->trajectoryRampDurationTicks);
            }
            mixedScalarOrPointerCarrier = (shotRuntime->runtimeStateOrSavedOffset).runtimeState;
            if (shotDefinition1->fixedRangeTransitionAgeThresholdTicks <=
                shotRuntime->projectileAgeTicks) {
              shotDefinition3 = (ShotDefinition *)(shotRuntime->definitionOrSavedId).savedId;
              if ((ShotDefinition *)mixedScalarOrPointerCarrier == (ShotDefinition *)0x0) {

                ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_UnlinkExpiredOrOrphanedProjectileAndReturn
                :
                InterpolationState_SetNegatedTargetAndRescaleProgress
                          (shotDefinition3->shadingReleaseTransitionDurationTicks,
                           modelNodeRuntime->shadingRecord);
                WorldRuntime_UnlinkNodeFromOwnerListD8((WorldRuntimeNode *)modelNodeRuntime);
                (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
                return;
              }
              shadingRecord1 = modelNodeRuntime->shadingRecord;
              iVar16 = *(int *)(((ShotDefinition *)mixedScalarOrPointerCarrier)->reservedDword04 +
                               0x94) - (modelNodeRuntime->worldTransform).translation.x;
              shotDefinition3 =
                   (ShotDefinition *)
                   (*(int *)(((ShotDefinition *)mixedScalarOrPointerCarrier)->reservedDword04 + 0x98
                            ) - (modelNodeRuntime->worldTransform).translation.y);
              pGVar6 = &(modelNodeRuntime->worldTransform).translation;
              pGVar6->x = pGVar6->x + iVar16;
              pGVar7 = &(modelNodeRuntime->worldTransform).translation.y;
              *pGVar7 = (int)shotDefinition3->terrainImpactEffectDefinitions31 + *pGVar7 + -0x14;
              pAVar8 = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
              *pAVar8 = -*pAVar8;
              if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
                shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + iVar16;
                shadingRecord1->worldYQ12 =
                     (int)shotDefinition3->terrainImpactEffectDefinitions31 +
                     shadingRecord1->worldYQ12 + -0x14;
              }
            }
          }
        }
      }
    }
    uVar21 = ModelNodeRuntime_RebuildTransformsFromRoot
                       (mixedScalarOrPointerCarrier,shotDefinition3,modelNodeRuntime);
    ModelNodeRuntime_UpdateDepthBinMasks
              (extraout_ECX_18,(int)((ulonglong)uVar21 >> 0x20),0,modelNodeRuntime);
    IStack_38 = IStack_38 - 1;
    if (IStack_38 == 0) {
      return;
    }
  } while( true );
}
