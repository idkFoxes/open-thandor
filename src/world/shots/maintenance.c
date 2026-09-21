/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/maintenance.c
 * Reverse engineering by idkFoxes 2026
 */

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
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  PackedArgb32 PVar1;
  PackedArgb32 PVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  FieldGridRegionMask primaryOccupancyMask;
  dword dVar9;
  undefined1 mm0PackedValue0ByteLane1;
  undefined1 mm0PackedValue0ByteLane2;
  undefined8 mm0PackedValue0;
  undefined1 uVar10;
  undefined1 uVar11;
  FixedDirectionXyzRegs12 FVar12;
  TerrainOccupancyResolvedMasksRegs12 TVar13;
  uint uVar14;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  primaryOccupancyMask =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 (0x1000,(modelNode->worldTransform).translation.y,
                  (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  if ((((shotRuntime->definitionOrSavedId).definition)->trajectoryMode ==
       SHOT_TRAJECTORY_DIRECT_LINE) && (modelNode->renderDepthBiasOrState != 0)) {
    FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState >> 1);
    dVar9 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (0x1000,FVar12.ecx + (modelNode->worldTransform).translation.y,
                       FVar12.eax + (modelNode->worldTransform).translation.x,
                       worldRuntime->fieldGrid);
    uVar14 = primaryOccupancyMask | dVar9;
    FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,
                        modelNode->renderDepthBiasOrState);
    dVar9 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                      (0x1000,FVar12.ecx + (modelNode->worldTransform).translation.y,
                       FVar12.eax + (modelNode->worldTransform).translation.x,
                       worldRuntime->fieldGrid);
    primaryOccupancyMask = dVar9 | uVar14;
  }
  modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffff3;
  TVar13 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (modelNode->runtimeFlags,0,primaryOccupancyMask,
                      (char)worldRuntime->activeFactionRuntimeIndex);
  modelNode->runtimeFlags = modelNode->runtimeFlags | TVar13.runtimeFlags;
  shotRuntime->terrainRuntimeClassState = TVar13.primaryOccupancyMask;
  UiModelControl_RefreshStateTint((ModelRuntimeNode *)modelNode);
  PVar1 = modelNode->tintArgb;
  PVar2 = ((shotRuntime->definitionOrSavedId).definition)->stateTintArgb;
  mm0PackedValue0ByteLane1 = (undefined1)(PVar1 >> 0x18);
  uVar7 = CONCAT11(mm0PackedValue0ByteLane1,mm0PackedValue0ByteLane1);
  mm0PackedValue0ByteLane2 = (undefined1)(PVar1 >> 0x10);
  mm0PackedValue0ByteLane1 = (undefined1)(PVar1 >> 8);
  uVar10 = (undefined1)(PVar2 >> 0x18);
  uVar8 = CONCAT11(uVar10,uVar10);
  uVar11 = (undefined1)(PVar2 >> 0x10);
  uVar10 = (undefined1)(PVar2 >> 8);
  mm0PackedValue0 =
       pmulhw(CONCAT26(uVar7 >> 4,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(uVar7,mm0PackedValue0ByteLane2),
                                                  CONCAT14(mm0PackedValue0ByteLane2,PVar1)) >> 0x20)
                                >> 4,CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                       mm0PackedValue0ByteLane1) >> 4,
                                              CONCAT11((char)PVar1,(char)PVar1) >> 4))),
              CONCAT26(uVar8 >> 4,
                       CONCAT24((ushort)(CONCAT35(CONCAT21(uVar8,uVar11),CONCAT14(uVar11,PVar2)) >>
                                        0x20) >> 4,
                                CONCAT22(CONCAT11(uVar10,uVar10) >> 4,
                                         CONCAT11((char)PVar2,(char)PVar2) >> 4))));
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
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject)

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
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  GraphicsFixedVec3 *worldPosition;
  uint uVar1;
  SpatialSoundSlot *slot;
  bool bVar2;
  ShotRuntimeSlot *shotRuntime;
  
  shotRuntime = modelNode->shotRuntime;
  if ((worldRuntime->dwordArray != (dword *)0x0) &&
     (uVar1 = ((shotRuntime->definitionOrSavedId).definition)->terrainGridMaskIndex,
     uVar1 < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
    worldPosition = &(modelNode->worldTransform).translation;
    if (slot != (SpatialSoundSlot *)0x0) {
      bVar2 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((modelNode->worldTransform).translation.y,worldPosition->x,worldRuntime);
      if (!bVar2) {
        SpatialSound_UpdateDesiredPositionedGains
                  (((shotRuntime->definitionOrSavedId).definition)->
                   positionedSoundMaximumDistanceQ12,
                   ((shotRuntime->definitionOrSavedId).definition)->positionedSoundGainQ15,
                   worldPosition,slot);
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
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode)

{
  ShotAnimationFrameIndex *pSVar1;
  ShotLifetimeRemainingTicks *pSVar2;
  ShotSecondaryEffectCountdownTicks *pSVar3;
  Q12 *pQVar4;
  GraphicsFixedVec3 *pGVar5;
  GraphicsWorldCoordinateQ12 *pGVar6;
  AngleTurn32 *pAVar7;
  ShotFrameAdvanceThresholdQ4 SVar8;
  int *piVar9;
  uint uVar10;
  uint uVar11;
  dword mixedScalarOrPointerCarrier;
  int iVar12;
  int iVar13;
  ModelRuntimeNode *modelNode1;
  uint scale;
  uint scale_00;
  Q12 worldXQ12;
  Q12 trajectoryStepYQ12;
  AngleTurn16Stored32 headingTurnDeltaAngle16;
  AngleTurn32 ballisticAzimuthAngle;
  ModelRaycastNearestNodeOrScratch4 MVar14;
  dword dVar15;
  AngleTurn16Stored32 elevationTurnDeltaAngle16;
  ShotDefinition *pSVar16;
  FactionRuntimeIndex FVar17;
  ShotRuntimeSlot *shotRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  ModelLookupEntryEaxCf5 MVar18;
  ModelRaycastNearestHitEaxEdxCf9 MVar19;
  FieldGridRaycastEaxEdxCf9 FVar20;
  FixedMathVectorAnglesRegs8 FVar21;
  ModelLocalPointRegs12 MVar22;
  FixedLengthAnglesEaxEcxEdx12 FVar23;
  FixedDirectionXyzRegs12 FVar24;
  EffectDefinition *pEVar25;
  WorldRuntimeContext *pWVar26;
  AngleTurn32 AVar27;
  Q12 impactValue;
  InGameSimulationStepBatchTicks IStack_38;
  int iStack_24;
  ArmyRuntimeSlot *armySlot1;
  ArmyRuntimeSlot *armySlot2;
  GraphicsShadingRuntimeRecord *shadingRecord1;
  
  IStack_38 = g_InGameSimulationStepTicks;
  do {
    shotRuntime = modelNode->shotRuntime;
    pSVar16 = (shotRuntime->definitionOrSavedId).definition;
    shotRuntime->projectileAgeTicks = shotRuntime->projectileAgeTicks + 1;
    uVar10 = shotRuntime->animationFrameAccumulatorQ4 + 0x10;
    uVar11 = pSVar16->animationFrameCount;
    shotRuntime->animationFrameAccumulatorQ4 = uVar10;
    SVar8 = pSVar16->animationFrameAdvanceThresholdQ4;
    if (pSVar16->animationFrameAdvanceThresholdQ4 <= uVar10) {
      pSVar1 = &(shotRuntime->ownerAndTrajectory).animationFrameIndex;
      *pSVar1 = *pSVar1 + 1;
      modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex + 1;
      shotRuntime->animationFrameAccumulatorQ4 = uVar10 - SVar8;
      if (uVar11 <= (shotRuntime->ownerAndTrajectory).animationFrameIndex) {
        pSVar1 = &(shotRuntime->ownerAndTrajectory).animationFrameIndex;
        *pSVar1 = *pSVar1 - uVar11;
        modelNode->textureSubresourceBaseIndex = modelNode->textureSubresourceBaseIndex - uVar11;
      }
    }
    pSVar2 = &shotRuntime->lifetimeTicksRemaining;
    *pSVar2 = *pSVar2 - 1;
    modelNodeRuntime = (ModelRuntimeNode *)modelNode;
    if (*pSVar2 == 0)
    goto 
    ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_UnlinkExpiredOrOrphanedProjectileAndReturn
    ;
    pSVar3 = &(shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks;
    *pSVar3 = *pSVar3 - 1;
    if (*pSVar3 == 0) {
      (shotRuntime->ownerAndTrajectory).secondaryEffectCountdownTicks =
           pSVar16->secondaryEffectIntervalTicks;
      MVar18 = ModelLookupTable_ContainsPackedKeyCf(1,3,pSVar16->ownedNestedResource);
      if (!MVar18.carry) {
        pEVar25 = pSVar16->secondaryEffectDefinition;
        pWVar26 = worldRuntime;
        MVar22 = ModelNodeRuntime_TransformLocalPointRegs
                           (MVar18.entry,(ModelRuntimeNode *)modelNode);
        worldXQ12 = MVar22.ecx;
        EffectRuntimePool_CreateInstanceFromDefinitionCf
                  (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,0x4000,0,
                   MVar22.edx,worldXQ12,MVar22.eax,pEVar25,pWVar26);
      }
    }
    if (pSVar16->trajectoryMode == SHOT_TRAJECTORY_DIRECT_LINE) {
      armySlot2 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      uVar11 = pSVar16->projectileLifetimeTicks * pSVar16->launchSpeedQ12;
      AVar27 = (modelNode->modelPayload).worldRotationAngle1;
      modelNode->renderDepthBiasOrState = uVar11;
      modelNode1 = (ModelRuntimeNode *)0x0;
      if (armySlot2 != (ArmyRuntimeSlot *)0x0) {
        modelNode1 = armySlot2->modelNodeRuntime;
      }
      MVar19 = ModelRuntime_RaycastCandidateListNearestCf
                         (AVar27 - shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,uVar11,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,MODEL_RUNTIME_CLASS_00,
                          modelNode1,worldRuntime);
      MVar14 = MVar19.edxCarrier;
      uVar10 = MVar19.nearestDistanceQ12;
      if ((MVar19.carry) &&
         (iStack_24 = *(int *)(((((MVar14.nearestModelNode)->runtimePayload).modelRuntime)->
                               definitionOrSavedId).savedIdOrOffset + 0x5c),
         ((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
         [iStack_24] == (EffectDefinition *)0x0)) {
        uVar10 = 0x7fffffff;
      }
      FVar20 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,uVar11,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      dVar15 = FVar20.materialOrCellIndex;
      scale = FVar20.distanceQ12;
      if ((FVar20.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[dVar15]
          == (EffectDefinition *)0x0)) {
        scale = 0x7fffffff;
      }
      FVar20 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,uVar11,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      scale_00 = FVar20.distanceQ12;
      if ((FVar20.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition ==
          (EffectDefinition *)0x0)) {
        scale_00 = 0x7fffffff;
      }
      if (scale_00 < scale) {
        if (scale_00 < uVar10) {
          pSVar16 = (shotRuntime->definitionOrSavedId).definition;
          if ((scale_00 <= uVar11) &&
             (modelNode->renderDepthBiasOrState = scale_00,
             (shotRuntime->impactEffectEmissionFlags & 1) == 0)) {
            shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
            pEVar25 = pSVar16->primaryEffectDefinition;
            pWVar26 = worldRuntime;
            FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,scale_00);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,0x4000,0,
                       FVar24.edx + (modelNode->worldTransform).translation.z,
                       FVar24.ecx + (modelNode->worldTransform).translation.y,
                       FVar24.eax + (modelNode->worldTransform).translation.x,pEVar25,pWVar26);
          }
        }
        else {

          ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndContinueMotion
          :
          pSVar16 = (shotRuntime->definitionOrSavedId).definition;
          if (uVar10 <= uVar11) {
            modelNode->renderDepthBiasOrState = uVar10;
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((MVar14.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            armySlot2 = ((MVar14.nearestModelNode)->runtimePayload).armyRuntime;
            armySlot1 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            FVar17 = 0;
            if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
              FVar17 = armySlot1->factionIndex;
            }
            LOCK();
            UNLOCK();
            AVar27 = (modelNode->modelPayload).worldRotationAngle0;
            iVar12 = pSVar16->targetClassImpactDamageQ12[iStack_24] /
                     (int)pSVar16->projectileLifetimeTicks;
            LOCK();
            UNLOCK();
            pEVar25 = pSVar16->targetClassImpactEffectDefinitions8[iStack_24];
            if (((shotRuntime->impactEffectEmissionFlags & 1) == 0) &&
               (pEVar25 != (EffectDefinition *)0x0)) {
              shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
              pWVar26 = worldRuntime;
              FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,uVar10);
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,
                         -(modelNode->modelPayload).worldRotationAngle1,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,
                         FVar24.edx + (modelNode->worldTransform).translation.z,
                         FVar24.ecx + (modelNode->worldTransform).translation.y,
                         FVar24.eax + (modelNode->worldTransform).translation.x,pEVar25,pWVar26);
            }
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AVar27,FVar17,iVar12,armySlot2);
          }
        }
      }
      else {
        if (uVar10 <= scale)
        goto 
        ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndContinueMotion
        ;
        pSVar16 = (shotRuntime->definitionOrSavedId).definition;
        if ((scale <= uVar11) &&
           (modelNode->renderDepthBiasOrState = scale,
           (shotRuntime->impactEffectEmissionFlags & 1) == 0)) {
          shotRuntime->impactEffectEmissionFlags = shotRuntime->impactEffectEmissionFlags | 1;
          pEVar25 = pSVar16->terrainImpactEffectDefinitions31[dVar15];
          pWVar26 = worldRuntime;
          FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,scale);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     (ModelRuntimeNode *)(pSVar16->terrainImpactEffectOwnerSlots31 + dVar15),0,
                     0x4000,0,FVar24.edx + (modelNode->worldTransform).translation.z,
                     FVar24.ecx + (modelNode->worldTransform).translation.y,
                     FVar24.eax + (modelNode->worldTransform).translation.x,pEVar25,pWVar26);
          shotRuntime = modelNode->shotRuntime;
        }
      }
      iVar12 = ((shotRuntime->definitionOrSavedId).definition)->modelSpinStepTurn16;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
      pAVar7 = &(modelNode->modelPayload).worldRotationAngle2;
      *pAVar7 = *pAVar7 + iVar12;
      pAVar7 = &(modelNode->modelPayload).worldRotationAngle2;
      *pAVar7 = *pAVar7 & 0xffff;
    }
    else {
      armySlot2 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
      modelNode1 = (ModelRuntimeNode *)0x0;
      if (armySlot2 != (ArmyRuntimeSlot *)0x0) {
        modelNode1 = armySlot2->modelNodeRuntime;
      }
      MVar19 = ModelRuntime_RaycastCandidateListNearestCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,MODEL_RUNTIME_CLASS_00,
                          modelNode1,worldRuntime);
      MVar14 = MVar19.edxCarrier;
      uVar11 = MVar19.nearestDistanceQ12;
      if ((MVar19.carry) &&
         (iStack_24 = *(int *)(((((MVar14.nearestModelNode)->runtimePayload).modelRuntime)->
                               definitionOrSavedId).savedIdOrOffset + 0x5c),
         ((shotRuntime->definitionOrSavedId).definition)->targetClassImpactEffectDefinitions8
         [iStack_24] == (EffectDefinition *)0x0)) {
        uVar11 = 0x7fffffff;
      }
      FVar20 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      dVar15 = FVar20.materialOrCellIndex;
      mixedScalarOrPointerCarrier = FVar20.distanceQ12;
      if ((FVar20.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->terrainImpactEffectDefinitions31[dVar15]
          == (EffectDefinition *)0x0)) {
        mixedScalarOrPointerCarrier = 0x7fffffff;
      }
      FVar20 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                         ((modelNode->modelPayload).worldRotationAngle1 -
                          shotRuntime->elevationOffsetAngle16,
                          (modelNode->modelPayload).worldRotationAngle0,shotRuntime->launchSpeedQ12,
                          (modelNode->worldTransform).translation.z,
                          (modelNode->worldTransform).translation.y,
                          (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
      uVar10 = FVar20.distanceQ12;
      if ((FVar20.carry) &&
         (((shotRuntime->definitionOrSavedId).definition)->primaryEffectDefinition ==
          (EffectDefinition *)0x0)) {
        uVar10 = 0x7fffffff;
      }
      if (uVar10 < mixedScalarOrPointerCarrier) {
        if (uVar10 < uVar11) {
          pSVar16 = (shotRuntime->definitionOrSavedId).definition;
          if (uVar10 <= (uint)shotRuntime->launchSpeedQ12) {
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (pSVar16->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            pEVar25 = pSVar16->primaryEffectDefinition;
            FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                               ((modelNode->modelPayload).worldRotationAngle1,
                                (modelNode->modelPayload).worldRotationAngle0,uVar10);
            EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,0x4000,0,
                       FVar24.edx + (modelNode->worldTransform).translation.z,
                       FVar24.ecx + (modelNode->worldTransform).translation.y,
                       FVar24.eax + (modelNode->worldTransform).translation.x,pEVar25,worldRuntime);
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            return;
          }
        }
        else {

          ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndTerminateProjectile
          :
          pSVar16 = (shotRuntime->definitionOrSavedId).definition;
          if (uVar11 <= (uint)shotRuntime->launchSpeedQ12) {
            ShotRuntime_ApplyArmyHitRelationAndNotifications
                      (((MVar14.nearestModelNode)->runtimePayload).armyRuntime,shotRuntime);
            InterpolationState_SetNegatedTargetAndRescaleProgress
                      (pSVar16->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
            armySlot2 = ((MVar14.nearestModelNode)->runtimePayload).armyRuntime;
            impactValue = pSVar16->targetClassImpactDamageQ12[iStack_24];
            armySlot1 = (shotRuntime->ownerAndTrajectory).ownerArmyRuntime;
            FVar17 = 0;
            if (armySlot1 != (ArmyRuntimeSlot *)0x0) {
              FVar17 = armySlot1->factionIndex;
            }
            LOCK();
            UNLOCK();
            pEVar25 = pSVar16->targetClassImpactEffectDefinitions8[iStack_24];
            AVar27 = (modelNode->modelPayload).worldRotationAngle0;
            if (pEVar25 != (EffectDefinition *)0x0) {
              FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                                 ((modelNode->modelPayload).worldRotationAngle1,
                                  (modelNode->modelPayload).worldRotationAngle0,uVar11);
              EffectRuntimePool_CreateInstanceFromDefinitionCf
                        (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,
                         -(modelNode->modelPayload).worldRotationAngle1,
                         (modelNode->modelPayload).worldRotationAngle0 + 0x8000 & 0xffff,
                         FVar24.edx + (modelNode->worldTransform).translation.z,
                         FVar24.ecx + (modelNode->worldTransform).translation.y,
                         FVar24.eax + (modelNode->worldTransform).translation.x,pEVar25,worldRuntime
                        );
            }
            WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
            (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
            ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AVar27,FVar17,impactValue,armySlot2);
            return;
          }
        }
      }
      else {
        if (uVar11 <= mixedScalarOrPointerCarrier)
        goto 
        ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_HandleNearestArmyHitAndTerminateProjectile
        ;
        pSVar16 = (shotRuntime->definitionOrSavedId).definition;
        if (mixedScalarOrPointerCarrier <= (uint)shotRuntime->launchSpeedQ12) {
          (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
          InterpolationState_SetNegatedTargetAndRescaleProgress
                    (pSVar16->shadingReleaseTransitionDurationTicks,modelNode->shadingRecord);
          pEVar25 = pSVar16->terrainImpactEffectDefinitions31[dVar15];
          FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                             ((modelNode->modelPayload).worldRotationAngle1,
                              (modelNode->modelPayload).worldRotationAngle0,
                              mixedScalarOrPointerCarrier);
          EffectRuntimePool_CreateInstanceFromDefinitionCf
                    (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER,
                     (ModelRuntimeNode *)(pSVar16->terrainImpactEffectOwnerSlots31 + dVar15),0,
                     0x4000,0,FVar24.edx + (modelNode->worldTransform).translation.z,
                     FVar24.ecx + (modelNode->worldTransform).translation.y,
                     FVar24.eax + (modelNode->worldTransform).translation.x,pEVar25,worldRuntime);
          WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNode);
          return;
        }
      }
      pSVar16 = (shotRuntime->definitionOrSavedId).definition;
      if (pSVar16->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        FVar24 = FixedMath_DirectionFromAnglesScaledRegs
                           ((modelNode->modelPayload).worldRotationAngle1 -
                            shotRuntime->elevationOffsetAngle16,
                            (modelNode->modelPayload).worldRotationAngle0,
                            shotRuntime->launchSpeedQ12);
        trajectoryStepYQ12 = FVar24.ecx;
        shadingRecord1 = modelNode->shadingRecord;
        pGVar5 = &(modelNode->worldTransform).translation;
        pGVar5->x = pGVar5->x + FVar24.eax;
        if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
          shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + FVar24.eax;
          shadingRecord1->worldYQ12 = shadingRecord1->worldYQ12 + trajectoryStepYQ12;
          shadingRecord1->worldZQ12 = shadingRecord1->worldZQ12 + FVar24.edx;
        }
        pGVar6 = &(modelNode->worldTransform).translation.y;
        *pGVar6 = *pGVar6 + trajectoryStepYQ12;
        pSVar16 = (shotRuntime->definitionOrSavedId).definition;
        pGVar6 = &(modelNode->worldTransform).translation.z;
        *pGVar6 = *pGVar6 + FVar24.edx;
        piVar9 = (shotRuntime->runtimeStateOrSavedOffset).runtimeStatePointer;
        if ((pSVar16->guidanceTurnLimitAngle16 != 0) && (piVar9 != (int *)0x0)) {
          iVar12 = piVar9[1];
          if (*(int *)(*piVar9 + 0x4c) == 0x15) {
            iVar12 = *(int *)(iVar12 + 0xcc);
          }
          modelNodeRuntime = (shotRuntime->modelNodeOrSavedOffset).modelNode;
          FVar21 = FixedMath_VectorToAngles3Regs
                             ((*(int *)(*piVar9 + 0x50) + *(int *)(iVar12 + 0x9c)) -
                              (modelNodeRuntime->worldTransform).translation.z,
                              *(int *)(iVar12 + 0x98) -
                              (modelNodeRuntime->worldTransform).translation.y,
                              *(int *)(iVar12 + 0x94) -
                              (modelNodeRuntime->worldTransform).translation.x);
          elevationTurnDeltaAngle16 =
               FVar21.edx - (modelNodeRuntime->modelPayload).worldRotationAngle1;
          iVar12 = pSVar16->guidanceTurnLimitAngle16;
          headingTurnDeltaAngle16 =
               (int)((FVar21.ecx - (modelNodeRuntime->modelPayload).worldRotationAngle0) * 0x10000)
               >> 0x10;
          if (iVar12 < elevationTurnDeltaAngle16) {
            elevationTurnDeltaAngle16 = iVar12;
          }
          if (iVar12 < headingTurnDeltaAngle16) {
            headingTurnDeltaAngle16 = iVar12;
          }
          iVar12 = -iVar12;
          if (headingTurnDeltaAngle16 < iVar12) {
            headingTurnDeltaAngle16 = iVar12;
          }
          if (elevationTurnDeltaAngle16 < iVar12) {
            elevationTurnDeltaAngle16 = iVar12;
          }
          pAVar7 = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
          *pAVar7 = *pAVar7 + elevationTurnDeltaAngle16;
          (modelNodeRuntime->modelPayload).worldRotationAngle0 =
               headingTurnDeltaAngle16 + (modelNodeRuntime->modelPayload).worldRotationAngle0 &
               0xffff;
        }
      }
      iVar12 = pSVar16->modelSpinStepTurn16;
      modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
      pAVar7 = &(modelNodeRuntime->modelPayload).worldRotationAngle2;
      *pAVar7 = *pAVar7 + iVar12;
      pAVar7 = &(modelNodeRuntime->modelPayload).worldRotationAngle2;
      *pAVar7 = *pAVar7 & 0xffff;
      if (pSVar16->trajectoryMode != SHOT_TRAJECTORY_DIRECT_LINE) {
        if (pSVar16->trajectoryMode == SHOT_TRAJECTORY_LEAD_ADJUSTED) {
          if (pSVar16->trajectoryRampDurationTicks != 0) {
            uVar11 = shotRuntime->projectileAgeTicks;
            if (pSVar16->trajectoryRampDurationTicks < uVar11) {
              uVar11 = pSVar16->trajectoryRampDurationTicks;
            }
            shotRuntime->launchSpeedQ12 =
                 (Q12)(((longlong)
                        (int)(((longlong)(int)uVar11 * (longlong)(int)uVar11) /
                             (longlong)(int)pSVar16->trajectoryRampDurationTicks) *
                       (longlong)pSVar16->launchSpeedQ12) /
                      (longlong)(int)pSVar16->trajectoryRampDurationTicks);
            if (pSVar16->elevationOffsetAngle16 != 0) {
              uVar11 = pSVar16->trajectoryRampDurationTicks * 2;
              iVar12 = uVar11 - shotRuntime->projectileAgeTicks;
              if (uVar11 < shotRuntime->projectileAgeTicks) {
                iVar12 = 0;
              }
              iVar12 = ((int)(((longlong)iVar12 * (longlong)pSVar16->elevationOffsetAngle16) /
                             (longlong)(int)pSVar16->trajectoryRampDurationTicks) >> 1) -
                       shotRuntime->elevationOffsetAngle16;
              shotRuntime->elevationOffsetAngle16 = shotRuntime->elevationOffsetAngle16 + iVar12;
              pAVar7 = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
              *pAVar7 = *pAVar7 + iVar12;
            }
          }
        }
        else if (pSVar16->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
          pQVar4 = &(shotRuntime->ownerAndTrajectory).directionComponent2Q12;
          *pQVar4 = *pQVar4 - ((shotRuntime->definitionOrSavedId).definition)->ballisticDivisorQ12;
          FVar23 = FixedMath_VectorToAnglesAndLengthVec3Regs
                             ((GraphicsFixedVec3 *)
                              &(shotRuntime->ownerAndTrajectory).directionComponent0Q12);
          ballisticAzimuthAngle = FVar23.azimuthAngle;
          shotRuntime->launchSpeedQ12 = FVar23.lengthQ12;
          (modelNodeRuntime->modelPayload).worldRotationAngle0 = ballisticAzimuthAngle;
          (modelNodeRuntime->modelPayload).worldRotationAngle1 = FVar23.elevationAngle;
        }
        else if (pSVar16->trajectoryMode == SHOT_TRAJECTORY_FIXED_RANGE) {
          if ((int)(modelNodeRuntime->modelPayload).worldRotationAngle1 < 0) {
            shadingRecord1 = modelNodeRuntime->shadingRecord;
            if ((shotRuntime->runtimeStateOrSavedOffset).runtimeState != 0) {
              iVar12 = *(int *)((shotRuntime->runtimeStateOrSavedOffset).runtimeState + 4);
              iVar13 = *(int *)(iVar12 + 0x94) - (modelNodeRuntime->worldTransform).translation.x;
              iVar12 = *(int *)(iVar12 + 0x98) - (modelNodeRuntime->worldTransform).translation.y;
              pGVar5 = &(modelNodeRuntime->worldTransform).translation;
              pGVar5->x = pGVar5->x + iVar13;
              pGVar6 = &(modelNodeRuntime->worldTransform).translation.y;
              *pGVar6 = *pGVar6 + iVar12;
              if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
                shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + iVar13;
                shadingRecord1->worldYQ12 = shadingRecord1->worldYQ12 + iVar12;
              }
            }
          }
          else {
            if (pSVar16->trajectoryRampDurationTicks != 0) {
              uVar11 = shotRuntime->projectileAgeTicks;
              if (pSVar16->trajectoryRampDurationTicks < uVar11) {
                uVar11 = pSVar16->trajectoryRampDurationTicks;
              }
              shotRuntime->launchSpeedQ12 =
                   (Q12)(((longlong)
                          (int)(((longlong)
                                 (int)(((longlong)(int)uVar11 * (longlong)(int)uVar11) /
                                      (longlong)(int)pSVar16->trajectoryRampDurationTicks) *
                                (longlong)(int)uVar11) /
                               (longlong)(int)pSVar16->trajectoryRampDurationTicks) *
                         (longlong)pSVar16->launchSpeedQ12) /
                        (longlong)(int)pSVar16->trajectoryRampDurationTicks);
            }
            if (pSVar16->fixedRangeTransitionAgeThresholdTicks <= shotRuntime->projectileAgeTicks) {
              pSVar16 = (ShotDefinition *)(shotRuntime->definitionOrSavedId).savedId;
              if ((shotRuntime->runtimeStateOrSavedOffset).runtimeState == 0) {

                ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects_UnlinkExpiredOrOrphanedProjectileAndReturn
                :
                InterpolationState_SetNegatedTargetAndRescaleProgress
                          (pSVar16->shadingReleaseTransitionDurationTicks,
                           modelNodeRuntime->shadingRecord);
                WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
                (shotRuntime->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
                return;
              }
              iVar12 = *(int *)((shotRuntime->runtimeStateOrSavedOffset).runtimeState + 4);
              shadingRecord1 = modelNodeRuntime->shadingRecord;
              iVar13 = *(int *)(iVar12 + 0x94) - (modelNodeRuntime->worldTransform).translation.x;
              iVar12 = *(int *)(iVar12 + 0x98) - (modelNodeRuntime->worldTransform).translation.y;
              pGVar5 = &(modelNodeRuntime->worldTransform).translation;
              pGVar5->x = pGVar5->x + iVar13;
              pGVar6 = &(modelNodeRuntime->worldTransform).translation.y;
              *pGVar6 = *pGVar6 + iVar12;
              pAVar7 = &(modelNodeRuntime->modelPayload).worldRotationAngle1;
              *pAVar7 = -*pAVar7;
              if (shadingRecord1 != (GraphicsShadingRuntimeRecord *)0x0) {
                shadingRecord1->worldXQ12 = shadingRecord1->worldXQ12 + iVar13;
                shadingRecord1->worldYQ12 = shadingRecord1->worldYQ12 + iVar12;
              }
            }
          }
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    ModelNodeRuntime_UpdateDepthBinMasks(0,modelNodeRuntime);
    IStack_38 = IStack_38 - 1;
    if (IStack_38 == 0) {
      return;
    }
  } while( true );
}

