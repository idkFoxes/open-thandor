/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/audio.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/audio.h>
#include <thandor/thandor.h>

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
  int textureOffset;
  uint32_t paletteColor;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  int remainingCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint8_t *payloadCursor;
  uint32_t *destinationCursor;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((frontendPlayerRuntimeId != (playerRecord->factionAssignment).factionAssignmentIndex ||
         ((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0))) {
    playerRecord = playerRecord + 1;
    remainingBlocks = remainingBlocks - 1;
    if (remainingBlocks == 0) {
      return;
    }
  }
  payloadCursor = playerRecord->snapshotPayloadB0_13AF;
  textureOffset = *(int *)(*(int *)(armyGraphicsAsset + 0xb8) + 0xe2c + armyGraphicsAsset);
  remainingCount = 0x100;
  destinationCursor = (uint32_t *)(*(int *)(*(int *)(armyGraphicsAsset + 0xb8) + 0xe28 + armyGraphicsAsset) * 0x800
                    + 0x200 + armyGraphicsAsset);
  do {
    paletteColor = *(uint32_t *)payloadCursor;
    if ((paletteColor & 0xffffff) == 0) {
      paletteColor = paletteColor & 0xffffff;
    }
    else {
      paletteColor = paletteColor | 0xff000000;
    }
    *destinationCursor = paletteColor;
    payloadCursor = payloadCursor + 3;
    destinationCursor = destinationCursor + 2;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  destinationCursor = (uint32_t *)(textureOffset + armyGraphicsAsset);
  for (remainingCount = 0x400; remainingCount != 0; remainingCount = remainingCount + -1) {
    *destinationCursor = *(uint32_t *)payloadCursor;
    payloadCursor = payloadCursor + 4;
    destinationCursor = destinationCursor + 1;
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
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *modelSlot;
  bool cellMasked;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    soundSlotIndex = *(uint32_t *)((modelSlot->classState).reservedD4_DB + 4);
    if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != (uint32_t *)0x0)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != (SpatialSoundSlot *)0x0) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                           worldRuntime);
        if (!cellMasked) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                     worldPosition,soundSlot);
        }
      }
    }
  }
  soundSlotIndex = (modelSlot->classState).classStateD0;
  if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (uint32_t *)0x0)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                         worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                   worldPosition,soundSlot);
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
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *modelSlot;
  bool cellMasked;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((armyRuntime->movementControl).turnVelocityAngle16 == 0) {
    modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    if ((armyRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    soundSlotIndex = *(uint32_t *)((modelSlot->classState).reservedD4_DB + 4);
    if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
       (worldRuntime->dwordArray != (uint32_t *)0x0)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != (SpatialSoundSlot *)0x0) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                           worldRuntime);
        if (!cellMasked) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                     worldPosition,soundSlot);
        }
      }
    }
  }
  soundSlotIndex = (modelSlot->classState).classStateD0;
  if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != (uint32_t *)0x0)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                         worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                   worldPosition,soundSlot);
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
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool cellMasked;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->movementStateFlags != 0) ||
       ((armyRuntime->movementControl).turnVelocityAngle16 != 0)) &&
      (soundSlotIndex = *(uint32_t *)((modelSlot->classState).reservedD4_DB + 4), soundSlotIndex != 0)) &&
     ((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)))) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (slot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                         worldPosition->x,worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
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
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool cellMasked;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (soundSlotIndex = modelSlot->attachments140[3].childNodeIndex0C, soundSlotIndex != 0)))) &&
     (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)) &&
      (slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex], slot != (SpatialSoundSlot *)0x0))
     )) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
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
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  bool cellMasked;
  ModelRuntimeNode *modelNode;
  
  modelNode = armyRuntime->modelNodeRuntime;
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (soundSlotIndex = modelSlot->attachments140[3].childNodeIndex0C, soundSlotIndex != 0)))) &&
     (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)) &&
      (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
      soundSlot != (SpatialSoundSlot *)0x0)))) {
    worldPosition = &(modelNode->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode->worldTransform).translation.y,worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,worldPosition,
                 soundSlot);
    }
  }
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->articulatedContact).fallbackPosition0Q12 != 1) &&
      (soundSlotIndex = modelSlot[1].classLinkState.classState74,
      (armyRuntime->articulatedContact).fallbackPosition0Q12 != 0)) &&
     ((soundSlotIndex != 0 &&
      (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)) &&
       (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
       soundSlot != (SpatialSoundSlot *)0x0)))))) {
    worldPosition = &(modelNode->worldTransform).translation;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode->worldTransform).translation.y,worldPosition->x,worldRuntime);
    if (!cellMasked) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,worldPosition,
                 soundSlot);
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
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool cellMasked;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  soundSlotIndex = modelSlot->attachments140[3].childNodeIndex0C;
  if (((worldRuntime->dwordArray != (uint32_t *)0x0) && (soundSlotIndex != 0)) &&
     (soundSlotIndex < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (slot != (SpatialSoundSlot *)0x0) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                         worldPosition->x,worldRuntime);
      if (!cellMasked) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
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
  ModelRuntimeNode *modelNode;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  ModelRuntimeSlot *modelSlot;
  bool cellMasked;
  
  modelNode = armyRuntime->modelNodeRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).terrainContactMode ==
         ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE)) &&
       (soundSlotIndex = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
                childNodeIndex0C, soundSlotIndex != 0)))) &&
     (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)) &&
      (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
      soundSlot != (SpatialSoundSlot *)0x0)))) {
    modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x
                       ,worldRuntime);
    if (!cellMasked) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                 &(modelNode->worldTransform).translation,soundSlot);
    }
  }
  if ((((armyRuntime->articulatedContact).lateralOffsetQ12 != 6) &&
      (soundSlotIndex = *(uint32_t *)((armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset + 0x274),
      (armyRuntime->articulatedContact).lateralOffsetQ12 != 0)) &&
     ((soundSlotIndex != 0 &&
      (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != (uint32_t *)0x0)) &&
       (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
       soundSlot != (SpatialSoundSlot *)0x0)))))) {
    modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    cellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
                      ((modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x
                       ,worldRuntime);
    if (!cellMasked) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                 &(modelNode->worldTransform).translation,soundSlot);
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

