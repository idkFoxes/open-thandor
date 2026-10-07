/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/audio.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/audio.h>
#include <thandor/thandor.h>

/* Feeds the world sound slot soundSlotIndex (0 = none; out of range or no slot table = none) with the position of
   modelNode, using the positioned-sound distance and gain of definition, when TerrainGrid_TestProjectedCellMaskBits01
   reports occupancy bit 0 or 1 of the active faction at the model's cell. Shared by the class sound updates below. */
static void ArmyRuntimeAudio_UpdateSoundAtModel(WorldRuntimeContext *worldRuntime,ModelDefinition *definition,
          ModelRuntimeNode *modelNode,uint32_t soundSlotIndex)

{
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;

  if ((soundSlotIndex == 0) || (soundSlotIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  soundSlot = ArmySound_Slot(worldRuntime,soundSlotIndex);
  if (soundSlot == nullptr) {
    return;
  }
  worldPosition = &(modelNode->worldTransform).translation;
  if (!TerrainGrid_TestProjectedCellMaskBits01(worldPosition->y,worldPosition->x,worldRuntime)) {
    SpatialSound_UpdateDesiredPositionedGains
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,worldPosition,
               soundSlot);
  }
}

/* Per-tick sound update of a turning/moving unit: while it turns, the turn sound (sound slot index
   turningLoopSoundSlotIndex of the model's definition) and the movement sound (movingLoopSoundSlotIndex) follow
   the unit's position; while it only moves, just the movement sound does. A sound is fed only when
   TerrainGrid_TestProjectedCellMaskBits01 reports occupancy bit 0 or 1 of the active faction at the unit's cell
   (returns false).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[1], [2], [17], [18] and [19],
   which ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id. The original has five
   copies of this function: the tracked (slot 2), glider (slot 17), ground (slot 1) and water (slot 19) updates,
   and for slot 18 a dispatcher that picks the water copy for placement contact kind 1 (water surface) and the
   ground copy otherwise. The ground copy re-reads the definition between the two sounds (skipped after a masked
   cell); nothing in between writes it.
*/
void ArmyRuntimeAudio_UpdateTurnAndMoveSounds
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
  if (!Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) &&
      (Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) ||
       ((modelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING))) {
    ArmyRuntimeAudio_UpdateSoundAtModel
              (worldRuntime,definition,modelRuntime->rootModelNodeOrSavedOffset.modelNode,
               definition->loopingSoundSlotIndex);
  }
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
  if (!Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) &&
      (Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) ||
       ((modelRuntime->classState).behaviorState == ARMY_FACTORY_STATE_BUILDING))) {
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->loopingSoundSlotIndex);
  }
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if (((modelRuntime->classState).behaviorState != ARMY_FACTORY_STATE_BUILDING) &&
      ((modelRuntime->classState).behaviorState != ARMY_FACTORY_STATE_IDLE)) {
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->positionedSoundSlotIndex);
  }
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
  if (!Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) &&
      (Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) ||
       ((modelRuntime->classState).classStateAC == 1))) {
    definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->loopingSoundSlotIndex);
  }
  if (((modelRuntime->classState).classStateB0 != 6) && ((modelRuntime->classState).classStateB0 != 0)) {
    definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
    ArmyRuntimeAudio_UpdateSoundAtModel(worldRuntime,definition,modelNode,definition->positionedSoundSlotIndex);
  }
}

/* Runs the looping positioned-sound update only while the model researches (state flag 0x40).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[4], which
   ArmyRuntimeHierarchy_DispatchClassMethodDRecursive calls by the model's class id.
*/
void ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  if (Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING)) {
    ArmyRuntime_UpdateLoopingPositionedSound(worldRuntime,modelRuntime);
  }
}

/* Keeps the model's looping sound (its definition's loopingSoundSlotIndex) at the model's position while
   state flag 1 (switched off) is clear and the active faction's cell bits 0/1 are set there. Reached through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[10], [14] and [16] and through
   ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled (classMethodD[4]).
*/
void ArmyRuntime_UpdateLoopingPositionedSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelDefinition *definition;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  Bool8 cellMasked;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  soundSlotIndex = definition->loopingSoundSlotIndex;
  if (Any((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) || (soundSlotIndex == 0) ||
      (soundSlotIndex >= worldRuntime->dwordArrayCount) || (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  slot = ArmySound_Slot(worldRuntime,soundSlotIndex);
  if (slot == nullptr) {
    return;
  }
  worldPosition = &(modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation;
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y,
                     worldPosition->x,worldRuntime);
  if (!cellMasked) {
    SpatialSound_UpdateDesiredPositionedGains
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,worldPosition,slot);
  }
}

/* Air-raid alert (called directly by ArmyRuntimeClass_UpdateAircraft on every tick of an attack
   run): when the aircraft of factionIndex is hostile to the active faction and flies over a grid cell with bit
   0x10 in the active faction's byte of the cell's occupancyMask, the sound soundAssetIndex is played
   unpositioned at the effects gain, at most once per 16 ticks of the faction's relationTransitionTick.
*/
void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint(FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext)

{
  SoundVoiceSet **voiceSetRef;
  FieldGridDimension gridWidthCells;
  uint32_t activeFactionIndex;
  int cellColumn;
  uint32_t projectedRow;
  int cellRow;
  Bool8 capabilityClear;
  FieldGridAsset *fieldGrid;

  if ((soundAssetIndex == 0) || (worldContext->dwordArray == nullptr) ||
      (soundAssetIndex >= worldContext->dwordArrayCount)) {
    return;
  }
  voiceSetRef = ArmySound_VoiceSetRef(worldContext,soundAssetIndex);
  if (voiceSetRef == nullptr) {
    return;
  }
  fieldGrid = worldContext->fieldGrid;
  /* world point -> grid cell (fixed-point projection, rounded) */
  projectedRow = FIXED_MUL_SHR(worldYQ12,FIELD_GRID_WORLD_Y_TO_ROW_Q20,Q20_SHIFT + 1);
  gridWidthCells = fieldGrid->gridWidth;
  cellColumn = (int)((FIXED_MUL_SHR(worldXQ12,FIELD_GRID_WORLD_X_TO_COLUMN_Q20,Q20_SHIFT) - projectedRow) + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if (cellColumn < 0) {
    return;
  }
  cellRow = (int)(projectedRow * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((cellRow < 0) || (cellColumn >= (int)gridWidthCells) || (cellRow >= (int)fieldGrid->gridHeight)) {
    return;
  }
  activeFactionIndex = worldContext->activeFactionRuntimeIndex;
  if (factionIndex == activeFactionIndex) {
    return;
  }
  capabilityClear = GameFactionRuntime_TestCapabilityBitClear(activeFactionIndex,factionIndex);
  /* the active faction's byte of the cell's occupancy mask */
  if ((capabilityClear) &&
      ((FieldGridCell_OccupancyByte(&fieldGrid->cells[(int32_t)(gridWidthCells * cellRow + cellColumn)],activeFactionIndex) &
        ARMY_DEPTH_BIN_STRUCTURE_BIT) != 0) &&
      (16 < g_GameFactionRuntimeImage.records[activeFactionIndex].relationTransitionTick)) {
    g_GameFactionRuntimeImage.records[activeFactionIndex].relationTransitionTick = 0;
    g_SoundPlayOneShot(g_SoundEffectsGainQ15,g_SoundEffectsGainQ15,*voiceSetRef,nullptr);
  }
}

/* Plays the one-shot sound soundAssetIndex (an index into the world's sound slot array; 0, out of range or an
   empty slot is ignored) at the model's position, with the range and gain of the model's definition
   (positionedSoundMaximumDistanceQ12/positionedSoundGainQ15), where the active faction's cell bits 0/1 are set.
   Called directly by ArmyRuntimeClass_UpdateAircraft for the home pad: with the definition's secondarySoundIndex
   when an aircraft lands or takes off (the pad's platform sound), with its primarySoundIndex when a returning
   aircraft opens the pad (the hatch sound class 22 plays itself in
   ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode). The original has one copy per
   definition field (ModelRuntime_PlayDefinitionSecondaryOneShotSound,
   ModelRuntime_PlayDefinitionPrimaryOneShotSound).
*/
void ModelRuntime_PlayDefinitionOneShotSound(ModelRuntimeSlot *modelRuntime,uint32_t soundAssetIndex,
          WorldRuntimeContext *worldRuntime)

{
  ModelDefinition *definition;
  ModelRuntimeNode *rootNode;
  SoundVoiceSet **voiceSetRef;
  Bool8 cellMasked;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  rootNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if ((soundAssetIndex == 0) || (soundAssetIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == nullptr)) {
    return;
  }
  voiceSetRef = ArmySound_VoiceSetRef(worldRuntime,soundAssetIndex);
  if (voiceSetRef == nullptr) {
    return;
  }
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,worldRuntime);
  if (!cellMasked) {
    SpatialSound_PlayPositionedOneShot
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,
               &(rootNode->worldTransform).translation,voiceSetRef);
  }
}
