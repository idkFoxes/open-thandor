/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/audio.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/audio.h>

/* Implementation ownership: gameplay/army/audio. */

/* Address: 0x0051D5F0.
   Ownership: gameplay/army/audio.
   Purpose: Finds the matching enabled frontend player runtime block, converts its 256 palette entries to ARGB, and
   copies the associated 0x1000-byte texture payload into the army graphics asset. Typed parameters: p3
   armyGraphicsAsset→ArmyGraphicsAssetAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
          (FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset)

{
  int iVar1;
  uint uVar2;
  FrontendPlayerRuntimeBlockCount FVar3;
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
    if (FVar3 == 0) {
      return;
    }
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
  return;
}


/* Address: 0x00520740.
   Ownership: gameplay/army/audio.
   Purpose: Table membership RUNTIME_UPDATE[26]. Updates the two definition-selected projected looping sound
   channels when movement or turn state is active and the projected terrain cell is valid. Class method-D partition
   slots 24-47 receive (worldRuntime, armyRuntime).
   Cross-module calls: TerrainGrid_TestProjectedCellMaskBits01Cf [world/terrain/grid],
   SpatialSound_UpdateDesiredPositionedGains [audio/spatial/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  SpatialSoundSlot *pSVar2;
  GraphicsFixedVec3 *pGVar3;
  ModelRuntimeSlot *pMVar4;
  bool bVar5;
  
  pMVar4 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pMVar4 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = *(uint *)((pMVar4->classState).reservedD4_DB + 4);
    if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != (dword *)0x0)) {
      pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
      if (pSVar2 != (SpatialSoundSlot *)0x0) {
        pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                           worldRuntime);
        if (!bVar5) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((pMVar4->classLinkState).classState7C,(pMVar4->classLinkState).classState78,
                     pGVar3,pSVar2);
        }
      }
    }
  }
  uVar1 = (pMVar4->classState).classStateD0;
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (dword *)0x0)) {
    pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
    if (pSVar2 != (SpatialSoundSlot *)0x0) {
      pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                         worldRuntime);
      if (!bVar5) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar4->classLinkState).classState7C,(pMVar4->classLinkState).classState78,
                   pGVar3,pSVar2);
      }
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

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_DispatchPositionedSoundVariant
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  if (*(int *)((armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset + 0x278) == 1) {
    ArmyRuntimeClass_UpdatePositionedSoundsVariantB
              (worldRuntime,(ArmyRuntimeGroundMovementPositionedSoundView120 *)armyRuntime);
  }
  else {
    ArmyRuntimeClass_UpdatePositionedSoundsVariantA
              (worldRuntime,(ArmyRuntimeGroundMovementPositionedSoundView120 *)armyRuntime);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantB
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint uVar1;
  SpatialSoundSlot *pSVar2;
  GraphicsFixedVec3 *pGVar3;
  ModelRuntimeSlot *pMVar4;
  bool bVar5;
  
  pMVar4 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    pMVar4 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    uVar1 = *(uint *)((pMVar4->classState).reservedD4_DB + 4);
    if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != (dword *)0x0)) {
      pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
      if (pSVar2 != (SpatialSoundSlot *)0x0) {
        pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                           worldRuntime);
        if (!bVar5) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((pMVar4->classLinkState).classState7C,(pMVar4->classLinkState).classState78,
                     pGVar3,pSVar2);
        }
      }
    }
  }
  uVar1 = (pMVar4->classState).classStateD0;
  if (((uVar1 != 0) && (uVar1 < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (dword *)0x0)) {
    pSVar2 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar1];
    if (pSVar2 != (SpatialSoundSlot *)0x0) {
      pGVar3 = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,pGVar3->x,
                         worldRuntime);
      if (!bVar5) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar4->classLinkState).classState7C,(pMVar4->classLinkState).classState78,
                   pGVar3,pSVar2);
      }
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateMovementProjectedLoopingSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *pMVar1;
  uint uVar2;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool bVar3;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->movementStateFlags != 0) ||
       ((armyRuntime->movementControl).turnVelocityAngle16 != 0)) &&
      (uVar2 = *(uint *)((pMVar1->classState).reservedD4_DB + 4), uVar2 != 0)) &&
     ((uVar2 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)))) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar2];
    if (slot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      bVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                         worldPosition->x,worldRuntime);
      if (!bVar3) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                   worldPosition,slot);
      }
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateConditionalProjectedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *pMVar1;
  uint uVar2;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool bVar3;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (uVar2 = pMVar1->attachments140[3].childNodeIndex0C, uVar2 != 0)))) &&
     (((uVar2 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (slot = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar2], slot != (SpatialSoundSlot *)0x0))
     )) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    bVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!bVar3) {
      SpatialSound_UpdateDesiredPositionedGains
                ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                 worldPosition,slot);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdatePrimaryAndSecondaryProjectedSounds
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *pGVar1;
  ModelRuntimeSlot *pMVar2;
  uint uVar3;
  SpatialSoundSlot *pSVar4;
  bool bVar5;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  pMVar2 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (uVar3 = pMVar2->attachments140[3].childNodeIndex0C, uVar3 != 0)))) &&
     (((uVar3 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (pSVar4 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar3],
      pSVar4 != (SpatialSoundSlot *)0x0)))) {
    pGVar1 = &(modelNode1->worldTransform).translation;
    bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,pGVar1->x,worldRuntime);
    if (!bVar5) {
      SpatialSound_UpdateDesiredPositionedGains
                ((pMVar2->classLinkState).classState7C,(pMVar2->classLinkState).classState78,pGVar1,
                 pSVar4);
    }
  }
  pMVar2 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->articulatedContact).fallbackPosition0Q12 != 1) &&
      (uVar3 = pMVar2[1].classLinkState.classState74,
      (armyRuntime->articulatedContact).fallbackPosition0Q12 != 0)) &&
     ((uVar3 != 0 &&
      (((uVar3 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
       (pSVar4 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar3],
       pSVar4 != (SpatialSoundSlot *)0x0)))))) {
    pGVar1 = &(modelNode1->worldTransform).translation;
    bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode1->worldTransform).translation.y,pGVar1->x,worldRuntime);
    if (!bVar5) {
      SpatialSound_UpdateDesiredPositionedGains
                ((pMVar2->classLinkState).classState7C,(pMVar2->classLinkState).classState78,pGVar1,
                 pSVar4);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateAssetProjectedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *pMVar1;
  uint uVar2;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool bVar3;
  
  pMVar1 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  uVar2 = pMVar1->attachments140[3].childNodeIndex0C;
  if (((worldRuntime->dwordArray != (dword *)0x0) && (uVar2 != 0)) &&
     (uVar2 < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar2];
    if (slot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      bVar3 = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                         worldPosition->x,worldRuntime);
      if (!bVar3) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((pMVar1->classLinkState).classState7C,(pMVar1->classLinkState).classState78,
                   worldPosition,slot);
      }
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateTerrainContactAndArticulatedProjectedSounds
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *pMVar1;
  uint uVar2;
  SpatialSoundSlot *pSVar3;
  ModelRuntimeSlot *pMVar4;
  bool bVar5;
  
  pMVar1 = armyRuntime->modelNodeRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).terrainContactMode ==
         ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE)) &&
       (uVar2 = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
                childNodeIndex0C, uVar2 != 0)))) &&
     (((uVar2 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
      (pSVar3 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar2],
      pSVar3 != (SpatialSoundSlot *)0x0)))) {
    pMVar4 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((pMVar1->worldTransform).translation.y,(pMVar1->worldTransform).translation.x
                       ,worldRuntime);
    if (!bVar5) {
      SpatialSound_UpdateDesiredPositionedGains
                ((pMVar4->classLinkState).classState7C,(pMVar4->classLinkState).classState78,
                 &(pMVar1->worldTransform).translation,pSVar3);
    }
  }
  if ((((armyRuntime->articulatedContact).lateralOffsetQ12 != 6) &&
      (uVar2 = *(uint *)((armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset + 0x274),
      (armyRuntime->articulatedContact).lateralOffsetQ12 != 0)) &&
     ((uVar2 != 0 &&
      (((uVar2 < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (dword *)0x0)) &&
       (pSVar3 = (SpatialSoundSlot *)worldRuntime->dwordArray[uVar2],
       pSVar3 != (SpatialSoundSlot *)0x0)))))) {
    pMVar4 = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    bVar5 = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((pMVar1->worldTransform).translation.y,(pMVar1->worldTransform).translation.x
                       ,worldRuntime);
    if (!bVar5) {
      SpatialSound_UpdateDesiredPositionedGains
                ((pMVar4->classLinkState).classState7C,(pMVar4->classLinkState).classState78,
                 &(pMVar1->worldTransform).translation,pSVar3);
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
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  if ((armyRuntime->runtimeFlags & 0x40) != 0) {
    ArmyRuntime_UpdateLoopingPositionedSound(worldRuntime,armyRuntime);
  }
  return;
}

