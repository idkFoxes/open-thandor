/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/audio.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/audio.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/army/audio. */

/* Replaces one image of a freshly loaded faction graphics ('gfx') asset with the image a frontend player sent in
   his snapshot payload, so the player's own picture shows in the game: the payload's 256 RGB palette entries
   become opaque ARGB entries (pure black stays transparent), followed by 0x1000 bytes of pixel data. Nothing
   changes when no player with a complete snapshot has frontendPlayerRuntimeId as faction assignment.
*/
void ArmyGraphics_CopyFrontendPlayerPaletteAndTexture(FrontendPlayerRuntimeId frontendPlayerRuntimeId,
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
  payloadCursor = playerRecord->snapshotPayload;
  /* The asset is a texture source asset; the replaced image is subresource 0x71 of its table (entry at table
     offset + 0xE20). Its paletteIndex selects one of the 0x800-byte palettes (256 8-byte entries) from asset
     +0x200, its dataOffset locates the pixel data. */
  pixelDataOffset =
       ((GraphicsTextureSourceEntry *)
        (((GraphicsTextureSourceAsset *)armyGraphicsAsset)->tableDescriptor.subresourceTableOffset +
        armyGraphicsAsset))[ARMY_GRAPHICS_PLAYER_IMAGE_SUBRESOURCE].dataOffset;
  destinationCursor = (uint32_t *)(((GraphicsTextureSourceEntry *)
                                    (((GraphicsTextureSourceAsset *)armyGraphicsAsset)->tableDescriptor.
                                     subresourceTableOffset + armyGraphicsAsset))
                                   [ARMY_GRAPHICS_PLAYER_IMAGE_SUBRESOURCE].paletteIndex * ARMY_GRAPHICS_PALETTE_BYTES
                    + ARMY_GRAPHICS_PALETTE_TABLE_OFFSET + armyGraphicsAsset);
  for (remainingCount = 256; remainingCount != 0; remainingCount--) {
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
  }
  destinationCursor = (uint32_t *)(pixelDataOffset + armyGraphicsAsset);
  for (remainingCount = ARMY_GRAPHICS_PLAYER_IMAGE_DWORDS; remainingCount != 0; remainingCount--) {
    *destinationCursor = *(uint32_t *)payloadCursor;
    payloadCursor = payloadCursor + 4;
    destinationCursor++;
  }
  return;
}


/* Feeds the world sound slot soundSlotIndex (0 = none; out of range or no slot table = none) with the position of
   modelNode, using the positioned-sound distance and gain of definition, when TerrainGrid_TestProjectedCellMaskBits01
   reports occupancy bit 0 or 1 of the active faction at the model's cell. Shared by the class sound updates below. */
static void ArmyRuntimeAudio_UpdateSoundAtModel(WorldRuntimeContext *worldRuntime,ModelDefinition *definition,
          ModelRuntimeNode *modelNode,uint32_t soundSlotIndex)

{
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;

  if ((soundSlotIndex == 0) || (soundSlotIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == NULL)) {
    return;
  }
  soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
  if (soundSlot == NULL) {
    return;
  }
  worldPosition = &(modelNode->worldTransform).translation;
  if (!TerrainGrid_TestProjectedCellMaskBits01(worldPosition->y,worldPosition->x,worldRuntime)) {
    SpatialSound_UpdateDesiredPositionedGains
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,worldPosition,
               soundSlot);
  }
  return;
}


/* Per-tick sound update of a turning/moving unit: while it turns, the turn sound (sound slot index
   turningLoopSoundSlotIndex of the model's definition) and the movement sound (movingLoopSoundSlotIndex) follow
   the unit's position; while it only moves, just the movement sound does. A sound is fed only when
   TerrainGrid_TestProjectedCellMaskBits01 reports occupancy bit 0 or 1 of the active faction at the unit's cell
   (returns false).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[2], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateTrackedTurnAndMoveSounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((modelRuntime->movementControl).turnVelocityAngle16 != 0) {
    ArmyRuntimeAudio_UpdateSoundAtModel
              (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
               definition->turningLoopSoundSlotIndex);
  }
  else if ((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
    return;
  }
  ArmyRuntimeAudio_UpdateSoundAtModel
            (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
             definition->movingLoopSoundSlotIndex);
  return;
}


/* Picks the positioned-sound update by the placement contact kind (placementContactKindIndex) of the model's
   definition: kind 1 (water surface, see g_ArmyPlacementContactKindDispatchTable) uses
   ArmyRuntimeClass_UpdateWaterPositionedSounds, every other kind ArmyRuntimeClass_UpdateGroundPositionedSounds.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[18], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/

void ArmyRuntimeAudio_DispatchPositionedSoundVariant(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  if (modelRuntime->definitionOrSavedId.runtimeDefinition->placementContactKindIndex ==
      ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
    ArmyRuntimeClass_UpdateWaterPositionedSounds(worldRuntime,modelRuntime);
  }
  else {
    ArmyRuntimeClass_UpdateGroundPositionedSounds(worldRuntime,modelRuntime);
  }
  return;
}


/* Byte-for-byte duplicate of ArmyRuntimeAudio_UpdateTrackedTurnAndMoveSounds for another class:
   turn sound (turningLoopSoundSlotIndex) and movement sound (movingLoopSoundSlotIndex) of the definition follow
   a turning or moving unit.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[17], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateGliderTurnAndMoveSounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((modelRuntime->movementControl).turnVelocityAngle16 != 0) {
    ArmyRuntimeAudio_UpdateSoundAtModel
              (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
               definition->turningLoopSoundSlotIndex);
  }
  else if ((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
    return;
  }
  ArmyRuntimeAudio_UpdateSoundAtModel
            (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
             definition->movingLoopSoundSlotIndex);
  return;
}


/* Turret sound: moves the turning sound (definition turningLoopSoundSlotIndex) with the turret while it
   turns in pitch (pitchTurnVelocityAngle16) or yaw (yawTurnVelocityAngle16), if its cell passes
   TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[5..8], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateTurretTurnSound
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((modelRuntime->pitchTurnVelocityAngle16 != 0) || (modelRuntime->yawTurnVelocityAngle16 != 0)) {
    ArmyRuntimeAudio_UpdateSoundAtModel
              (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
               definition->turningLoopSoundSlotIndex);
  }
  return;
}


/* Structure factory sound (class 11): moves the looping sound (definition loopingSoundSlotIndex) with the model
   unless it is switched off (state flag 0x1), and only while it researches (flag 0x40) or builds (behaviorState
   ARMY_FACTORY_STATE_BUILDING); the model's cell must pass TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[11], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateStructureFactorySound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) &&
      ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) ||
       ((modelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING))) {
    ArmyRuntimeAudio_UpdateSoundAtModel
              (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
               definition->loopingSoundSlotIndex);
  }
  return;
}


/* Unit factory sounds (class 13), two sounds that follow the model: the looping one (definition
   loopingSoundSlotIndex) under the same condition as ArmyRuntimeAudio_UpdateStructureFactorySound (not switched
   off, researching or building), the positioned one (positionedSoundSlotIndex) while the factory is neither idle
   nor building (door opening, waiting for the exit, closing). Each is fed only when the model's cell passes
   TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[13], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateUnitFactorySounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;
  ModelRuntimeNode *modelNode;

  modelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) &&
      ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) ||
       ((modelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING))) {
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->loopingSoundSlotIndex);
  }
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if (((modelRuntime->classState).behaviorState != ARMY_FACTORY_STATE_BUILDING) &&
      ((modelRuntime->classState).behaviorState != ARMY_FACTORY_STATE_IDLE)) {
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->positionedSoundSlotIndex);
  }
  return;
}


/* Unconditionally moves the sound whose slot index is the definition's loopingSoundSlotIndex with the unit, when
   the unit's cell passes TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[21], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateAssetProjectedSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  ArmyRuntimeAudio_UpdateSoundAtModel
            (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
             definition->loopingSoundSlotIndex);
  return;
}


/* Sounds of the class-22 pad (ModelRuntimeLinkedChildSpawnAndBuildView), two sounds that follow the model:
   the looping one (definition loopingSoundSlotIndex) while it is not switched off and researches (flag 0x40)
   or builds (classState.classStateAC, the view's secondaryArmyAssetBuildState, == 1), the positioned one
   (positionedSoundSlotIndex) while its linked-child transition state (classState.classStateB0) is neither 0
   nor 6. Each is fed only when the model's cell passes
   TerrainGrid_TestProjectedCellMaskBits01.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[22], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateLinkedChildPadSounds
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *modelNode;
  ModelDefinition *definition;

  modelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) &&
      ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) ||
       ((modelRuntime->classState).classStateAC == 1))) {
    definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->loopingSoundSlotIndex);
  }
  if (((modelRuntime->classState).classStateB0 != 6) && ((modelRuntime->classState).classStateB0 != 0)) {
    definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->positionedSoundSlotIndex);
  }
  return;
}


/* Runs the looping positioned-sound update only while the model researches (state flag 0x40).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[4], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) {
    ArmyRuntime_UpdateLoopingPositionedSound(worldRuntime,modelRuntime);
  }
  return;
}

