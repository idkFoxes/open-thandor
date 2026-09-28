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
   Replaces one image of a freshly loaded faction graphics ('gfx') asset with the image a frontend player sent in
   his snapshot payload, so the player's own picture shows in the game: the payload's 256 RGB palette entries
   become opaque ARGB entries (pure black stays transparent), followed by 0x1000 bytes of pixel data. Nothing
   changes when no player with a complete snapshot has frontendPlayerRuntimeId as faction assignment.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
          (FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset)

{
  int pixelDataOffset;
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
    playerRecord++;
    remainingBlocks = remainingBlocks - 1;
    if (remainingBlocks == 0) {
      return;
    }
  }
  payloadCursor = playerRecord->snapshotPayloadB0_13AF;
  /* The asset dword at +0xB8 is an offset; the record behind it holds at +0xE28 the index of the image's
     palette (0x800-byte palettes of 256 8-byte entries from asset +0x200) and at +0xE2C the offset of its
     pixel data. */
  pixelDataOffset = *(int *)(*(int *)(armyGraphicsAsset + 0xb8) + 0xe2c + armyGraphicsAsset);
  remainingCount = 256;
  destinationCursor = (uint32_t *)(*(int *)(*(int *)(armyGraphicsAsset + 0xb8) + 0xe28 + armyGraphicsAsset) * 0x800
                    + 0x200 + armyGraphicsAsset);
  do {
    /* reads four bytes of a three-byte entry; the fourth is replaced by the alpha */
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
    remainingCount--;
  } while (remainingCount != 0);
  destinationCursor = (uint32_t *)(pixelDataOffset + armyGraphicsAsset);
  /* 0x400 dwords = 0x1000 bytes of pixel data */
  for (remainingCount = 0x400; remainingCount != 0; remainingCount--) {
    *destinationCursor = *(uint32_t *)payloadCursor;
    payloadCursor = payloadCursor + 4;
    destinationCursor++;
  }
  return;
}


/* Address: 0x00520740.
   Per-tick sound update of a turning/moving unit: while it turns, the turn sound (sound slot index at +0xD8
   of the army's model record) and the movement sound (+0xD0) follow the unit's position; while it only
   moves, just the movement sound does. A sound is fed only when TerrainGrid_TestProjectedCellMaskBits01
   reports occupancy bit 0 or 1 of the active faction at the unit's cell (CF clear).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[2] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *modelSlot;
  bool cellBitsClear;
  
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
       (worldRuntime->dwordArray != NULL)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != NULL) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                           worldRuntime);
        if (!cellBitsClear) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                     worldPosition,soundSlot);
        }
      }
    }
  }
  soundSlotIndex = (modelSlot->classState).classStateD0;
  if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != NULL)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != NULL) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                         worldRuntime);
      if (!cellBitsClear) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                   worldPosition,soundSlot);
      }
    }
  }
  return;
}


/* Address: 0x00520E60.
   Picks the positioned-sound update by the placement contact kind at +0x278 of the army's model record:
   kind 1 (water surface, see g_ArmyPlacementContactKindDispatchTable) uses variant B, every other kind
   variant A.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[18] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/

void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_DispatchPositionedSoundVariant
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  if (*(int *)((armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset + 0x278) ==
      ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
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
   Byte-for-byte duplicate of ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA for another class:
   turn sound (+0xD8) and movement sound (+0xD0) of the model record follow a turning or moving unit.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[17] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantB
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *modelSlot;
  bool cellBitsClear;
  
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
       (worldRuntime->dwordArray != NULL)) {
      soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
      if (soundSlot != NULL) {
        worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
        cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                          ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                           worldRuntime);
        if (!cellBitsClear) {
          SpatialSound_UpdateDesiredPositionedGains
                    ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                     worldPosition,soundSlot);
        }
      }
    }
  }
  soundSlotIndex = (modelSlot->classState).classStateD0;
  if (((soundSlotIndex != 0) && (soundSlotIndex < worldRuntime->dwordArrayCount)) &&
     (worldRuntime->dwordArray != NULL)) {
    soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (soundSlot != NULL) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,worldPosition->x,
                         worldRuntime);
      if (!cellBitsClear) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                   worldPosition,soundSlot);
      }
    }
  }
  return;
}


/* Address: 0x00523DD0.
   Moves the unit's looping sound (sound slot index at +0xD8 of the model record) with the unit while any
   movement state flag is set or it is turning, if its cell passes TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[5..8] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateMovementProjectedLoopingSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool cellBitsClear;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->movementStateFlags != 0) ||
       ((armyRuntime->movementControl).turnVelocityAngle16 != 0)) &&
      (soundSlotIndex = *(uint32_t *)((modelSlot->classState).reservedD4_DB + 4), soundSlotIndex != 0)) &&
     ((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)))) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (slot != NULL) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                         worldPosition->x,worldRuntime);
      if (!cellBitsClear) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                   worldPosition,slot);
      }
    }
  }
  return;
}


/* Address: 0x00524410.
   Moves the sound whose slot index is at +0x1AC of the model record with the unit, unless runtime flag
   0x1 is set, and only while runtime flag 0x40 is set or the army dword at +0xB8 equals 1; the unit's cell
   must pass TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[11] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateConditionalProjectedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool cellBitsClear;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (soundSlotIndex = modelSlot->attachments140[3].childNodeIndex0C, soundSlotIndex != 0)))) &&
     (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)) &&
      (slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex], slot != NULL))
     )) {
    worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
    cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                      ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                       worldPosition->x,worldRuntime);
    if (!cellBitsClear) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                 worldPosition,slot);
    }
  }
  return;
}


/* Address: 0x00524DA0.
   Two sounds that follow the unit: the primary one (slot index at +0x1AC of the model record) under the
   same condition as ArmyRuntimeAudio_UpdateConditionalProjectedSound (flag 0x1 clear, flag 0x40 set or
   army +0xB8 == 1), the secondary one (+0x274) while army +0xB8 is neither 0 nor 1. Each is fed only when
   the unit's cell passes TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[13] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdatePrimaryAndSecondaryProjectedSounds
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  bool cellBitsClear;
  ModelRuntimeNode *modelNode;
  
  modelNode = armyRuntime->modelNodeRuntime;
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1)) &&
       (soundSlotIndex = modelSlot->attachments140[3].childNodeIndex0C, soundSlotIndex != 0)))) &&
     (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)) &&
      (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
      soundSlot != NULL)))) {
    worldPosition = &(modelNode->worldTransform).translation;
    cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                      ((modelNode->worldTransform).translation.y,worldPosition->x,worldRuntime);
    if (!cellBitsClear) {
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
      (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)) &&
       (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
       soundSlot != NULL)))))) {
    worldPosition = &(modelNode->worldTransform).translation;
    cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                      ((modelNode->worldTransform).translation.y,worldPosition->x,worldRuntime);
    if (!cellBitsClear) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,worldPosition,
                 soundSlot);
    }
  }
  return;
}


/* Address: 0x00526490.
   Unconditionally moves the sound whose slot index is at +0x1AC of the model record with the unit, when
   the unit's cell passes TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[21] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateAssetProjectedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeSlot *modelSlot;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  GraphicsFixedVec3 *worldPosition;
  bool cellBitsClear;
  
  modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  soundSlotIndex = modelSlot->attachments140[3].childNodeIndex0C;
  if (((worldRuntime->dwordArray != NULL) && (soundSlotIndex != 0)) &&
     (soundSlotIndex < worldRuntime->dwordArrayCount)) {
    slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
    if (slot != NULL) {
      worldPosition = &(armyRuntime->modelNodeRuntime->worldTransform).translation;
      cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                        ((armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                         worldPosition->x,worldRuntime);
      if (!cellBitsClear) {
        SpatialSound_UpdateDesiredPositionedGains
                  ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                   worldPosition,slot);
      }
    }
  }
  return;
}


/* Address: 0x00526EB0.
   Two sounds that follow the unit: the primary one (slot index at +0x1AC of the model record) while flag
   0x1 is clear and flag 0x40 is set or the contact mode (army +0xAC) is 1, the secondary one (+0x274) while
   the army dword at +0xB0 is neither 0 nor 6. Each is fed only when the unit's cell passes
   TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[22] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeAudio_UpdateTerrainContactAndArticulatedProjectedSounds
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNode;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *soundSlot;
  ModelRuntimeSlot *modelSlot;
  bool cellBitsClear;
  
  modelNode = armyRuntime->modelNodeRuntime;
  if ((((armyRuntime->runtimeFlags & 1) == 0) &&
      ((((armyRuntime->runtimeFlags & 0x40) != 0 ||
        ((armyRuntime->articulatedContact).terrainContactMode ==
         ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE)) &&
       (soundSlotIndex = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->attachments140[3].
                childNodeIndex0C, soundSlotIndex != 0)))) &&
     (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)) &&
      (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
      soundSlot != NULL)))) {
    modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                      ((modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x
                       ,worldRuntime);
    if (!cellBitsClear) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                 &(modelNode->worldTransform).translation,soundSlot);
    }
  }
  if ((((armyRuntime->articulatedContact).lateralOffsetQ12 != 6) &&
      (soundSlotIndex = *(uint32_t *)((armyRuntime->modelRuntimeOrSavedOffset).savedIdOrOffset + 0x274),
      (armyRuntime->articulatedContact).lateralOffsetQ12 != 0)) &&
     ((soundSlotIndex != 0 &&
      (((soundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)) &&
       (soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex],
       soundSlot != NULL)))))) {
    modelSlot = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    cellBitsClear = TerrainGrid_TestProjectedCellMaskBits01
                      ((modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x
                       ,worldRuntime);
    if (!cellBitsClear) {
      SpatialSound_UpdateDesiredPositionedGains
                ((modelSlot->classLinkState).classState7C,(modelSlot->classLinkState).classState78,
                 &(modelNode->worldTransform).translation,soundSlot);
    }
  }
  return;
}


/* Address: 0x00527B20.
   Runs the looping positioned-sound update only while army runtime flag 0x40 is set.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[4] (0x0051FCF8), which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
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

