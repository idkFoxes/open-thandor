#include <thandor/gameplay/army/audio.h>

/* Implementation ownership: gameplay/army/audio. */

/* Address: 0x0051D5F0.
   Ownership: gameplay/army/audio.
   Purpose: Finds the matching enabled frontend player runtime block, converts its 256 palette entries to ARGB, and
   copies the associated 0x1000-byte texture payload into the army graphics asset. Typed parameters: p3
   armyGraphicsAsset→ArmyGraphicsAssetAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8
ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
          (FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset)

{
  int iVar1;
  undefined4 in_EAX;
  uint uVar2;
  FrontendPlayerRuntimeBlockCount FVar3;
  undefined4 in_EDX;
  int iVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  byte *pbVar6;
  uint *puVar7;
  
  FVar3 = g_FrontendPlayerRuntimeBlockCount;
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  while ((frontendPlayerRuntimeId != (pFVar5->factionAssignment).factionAssignmentIndex ||
         ((pFVar5->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0))) {
    pFVar5 = pFVar5 + 1;
    FVar3 = FVar3 - 1;
    if (FVar3 == 0)
    goto ArmyGraphics_CopyFrontendPlayerPaletteAndTexture_ReturnAfterCopyOrPlayerLookupMiss;
  }
  pbVar6 = pFVar5->snapshotPayloadB0_13AF;
  iVar1 = *(int *)(*(int *)(armyGraphicsAsset + 0xb8) + 0xe2c + armyGraphicsAsset);
  iVar4 = 0x100;
  puVar7 = (uint *)(*(int *)(*(int *)(armyGraphicsAsset + 0xb8) + 0xe28 + armyGraphicsAsset) * 0x800
                    + 0x200 + armyGraphicsAsset);
  do {
    uVar2 = *(uint *)pbVar6;
    if ((uVar2 & 0xffffff) == 0) {
      uVar2 = uVar2 & 0xffffff;
    }
    else {
      uVar2 = uVar2 | 0xff000000;
    }
    *puVar7 = uVar2;
    pbVar6 = pbVar6 + 3;
    puVar7 = puVar7 + 2;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  puVar7 = (uint *)(iVar1 + armyGraphicsAsset);
  for (iVar4 = 0x400; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar7 = *(uint *)pbVar6;
    pbVar6 = pbVar6 + 4;
    puVar7 = puVar7 + 1;
  }
ArmyGraphics_CopyFrontendPlayerPaletteAndTexture_ReturnAfterCopyOrPlayerLookupMiss:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00520740.
   Ownership: gameplay/army/audio.
   Purpose: Table membership RUNTIME_UPDATE[26]. Updates the two definition-selected projected looping sound
   channels when movement or turn state is active and the projected terrain cell is valid. Class method-D partition
   slots 24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  void *pvVar2;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pvVar2 = armyRuntime->definitionOrAsset;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = *(uint *)((int)pvVar2 + 0xd8);
    if ((((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
        (worldRuntime->dwordArray != (dword *)0x0)) &&
       (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)) {
      bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
      uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,worldRuntime);
      if (!bVar3) {
        SpatialSound_UpdateDesiredPositionedGains
                  (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                   *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                   (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
      }
    }
  }
  uVar1 = *(uint *)((int)pvVar2 + 0xd0);
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)))) {
    bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
  }
  return;
}

/* Address: 0x00520E60.
   Ownership: gameplay/army/audio.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[42]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: ArmyRuntimeClass_UpdatePositionedSoundsVariantB [gameplay/army/runtime],
   ArmyRuntimeClass_UpdatePositionedSoundsVariantA [gameplay/army/runtime].
*/
void ArmyRuntimeAudio_DispatchPositionedSoundVariant
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  if (*(int *)((int)armyRuntime->definitionOrAsset + 0x278) == 1) {
    ArmyRuntimeClass_UpdatePositionedSoundsVariantB(worldRuntime,armyRuntime);
  }
  else {
    ArmyRuntimeClass_UpdatePositionedSoundsVariantA(worldRuntime,armyRuntime);
  }
  return;
}

/* Address: 0x00523240.
   Ownership: gameplay/army/audio.
   Purpose: Table membership RUNTIME_UPDATE[41]. Second exact class-table implementation of the dual projected
   looping-sound update used by a different runtime class. Class method-D partition slots 24-47 receive
   (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantB
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  void *pvVar2;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar2 = armyRuntime->definitionOrAsset;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pvVar2 = armyRuntime->definitionOrAsset;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = *(uint *)((int)pvVar2 + 0xd8);
    if ((((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
        (worldRuntime->dwordArray != (dword *)0x0)) &&
       (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)) {
      bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
      uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((modelNode1->worldTransform).translation.y,
                         (modelNode1->worldTransform).translation.x,worldRuntime);
      if (!bVar3) {
        SpatialSound_UpdateDesiredPositionedGains
                  (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                   *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                   (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
      }
    }
  }
  uVar1 = *(uint *)((int)pvVar2 + 0xd0);
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     ((worldRuntime->dwordArray != (dword *)0x0 &&
      (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar1] != 0)))) {
    bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar2 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar2 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
  }
  return;
}

/* Address: 0x00523DD0.
   Ownership: gameplay/army/audio.
   Purpose: Table membership RUNTIME_UPDATE[29],RUNTIME_UPDATE[30],RUNTIME_UPDATE[31],RUNTIME_UPDATE[32]. Updates
   the movement-linked projected looping sound while movement or turn velocity is active and the projected terrain
   cell is valid. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdateMovementProjectedLoopingSound
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  uint uVar2;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar1 = armyRuntime->definitionOrAsset;
  if ((((armyRuntime->movementStateFlags != 0) ||
       ((armyRuntime->movementControl).turnVelocityAngle16 != 0)) &&
      (uVar2 = *(uint *)((int)pvVar1 + 0xd8), uVar2 != 0)) &&
     (((uVar2 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar2] != 0)))) {
    bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar1 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar1 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
  }
  return;
}

/* Address: 0x00524410.
   Ownership: gameplay/army/audio.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[35]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdateConditionalProjectedSound
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  SpatialSoundSlot *slot;
  int extraout_ECX;
  GraphicsFixedVec3 *worldPosition;
  bool bVar2;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x1ac), uVar1 != 0)))) &&
     (((uVar1 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (worldRuntime->dwordArray[uVar1] != 0)))) {
    bVar2 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    worldPosition = &(modelNode1->worldTransform).translation;
    slot = (SpatialSoundSlot *)
           TerrainGrid_TestProjectedCellMaskBits01Cf
                     ((modelNode1->worldTransform).translation.y,worldPosition->x,worldRuntime);
    if (!bVar2) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX + 0x78),worldPosition,slot);
    }
  }
  return;
}

/* Address: 0x00524DA0.
   Ownership: gameplay/army/audio.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[37]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdatePrimaryAndSecondaryProjectedSounds
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  bool bVar2;
  undefined8 uVar3;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x1ac), uVar1 != 0)))) &&
     (((uVar1 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (bVar2 = false, worldRuntime->dwordArray[uVar1] != 0)))) {
    uVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar2) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar3 >> 0x20),(SpatialSoundSlot *)uVar3);
    }
  }
  if ((((armyRuntime->articulatedContact).fallbackPosition0Q12 != 1) &&
      (uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x274),
      (armyRuntime->articulatedContact).fallbackPosition0Q12 != 0)) &&
     ((uVar1 != 0 &&
      (((uVar1 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
       (bVar2 = false, worldRuntime->dwordArray[uVar1] != 0)))))) {
    uVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar2) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX_00 + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX_00 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar3 >> 0x20),(SpatialSoundSlot *)uVar3);
    }
  }
  return;
}

/* Address: 0x00526490.
   Ownership: gameplay/army/audio.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[45]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdateAssetProjectedSound
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  uint uVar2;
  bool bVar3;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  pvVar1 = armyRuntime->definitionOrAsset;
  uVar2 = *(uint *)((int)pvVar1 + 0x1ac);
  if ((((worldRuntime->dwordArray != (dword *)0x0) && (uVar2 != 0)) &&
      (uVar2 < worldRuntime->dwordArrayCount)) &&
     (modelNode1 = armyRuntime->modelNodeRuntime, worldRuntime->dwordArray[uVar2] != 0)) {
    bVar3 = (ModelRuntimeNode *)0xffffff6b < modelNode1;
    uVar4 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)((int)pvVar1 + 0x7c),
                 *(SpatialSoundGainQ15 *)((int)pvVar1 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar4 >> 0x20),(SpatialSoundSlot *)uVar4);
    }
  }
  return;
}

/* Address: 0x00526EB0.
   Ownership: gameplay/army/audio.
   Purpose: Table membership RUNTIME_UPDATE[46]. Updates the terrain-contact and articulated-state projected sound
   channels when their definition-selected sound slots are active. Class method-D partition slots 24-47 receive
   (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void ArmyRuntimeAudio_UpdateTerrainContactAndArticulatedProjectedSounds
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  bool bVar2;
  undefined8 uVar3;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).terrainContactMode ==
         ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE)) &&
       (uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x1ac), uVar1 != 0)))) &&
     (((uVar1 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (bVar2 = false, worldRuntime->dwordArray[uVar1] != 0)))) {
    uVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar2) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar3 >> 0x20),(SpatialSoundSlot *)uVar3);
    }
  }
  if ((((armyRuntime->articulatedContact).lateralOffsetQ12 != 6) &&
      (uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x274),
      (armyRuntime->articulatedContact).lateralOffsetQ12 != 0)) &&
     ((uVar1 != 0 &&
      (((uVar1 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
       (bVar2 = false, worldRuntime->dwordArray[uVar1] != 0)))))) {
    uVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x,worldRuntime);
    if (!bVar2) {
      SpatialSound_UpdateDesiredPositionedGains
                (*(SpatialSoundMaximumDistanceQ12 *)(extraout_ECX_00 + 0x7c),
                 *(SpatialSoundGainQ15 *)(extraout_ECX_00 + 0x78),
                 (GraphicsFixedVec3 *)((ulonglong)uVar3 >> 0x20),(SpatialSoundSlot *)uVar3);
    }
  }
  return;
}

/* Address: 0x00527B20.
   Ownership: gameplay/army/audio.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FC98[28]@0051FC98. Class method-D partition slots
   24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: ArmyRuntime_UpdateLoopingPositionedSound [gameplay/army/runtime].
*/
void ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  if ((armyRuntime->runtimeFlags & 0x40) != 0) {
    ArmyRuntime_UpdateLoopingPositionedSound(worldRuntime,armyRuntime);
  }
  return;
}
