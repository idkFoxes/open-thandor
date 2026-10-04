/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/commands.h>
#include <thandor/thandor.h>

/* Module data. */

/* indexed by player runtime id (0..254) */
SelectionPlayerRuntimeBlock *g_SelectionPlayerRuntimeBlockPointers[256] = {0};

/* Implementation ownership: gameplay/selection/commands. */

/* In-game command handler (code 0x8F0, key A): replaces the player's selection with every world model of
   definition class 0x16 that the player owns, then rebuilds the selection panels when the player is the local
   one.
*/
void InGameSelection_SelectAllOwnAircraftPads
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

{
  WorldOwnerListNode *ownerNode;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRoot *inGameRoot;

  inGameRoot = g_InGameRuntimeRoot;
  SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  for (ownerNode = (inGameRoot->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      /* model payload: the ModelRuntimeSlot; its owner army is the entity */
      entityRuntime =
           (GameEntityRuntime *)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if ((((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
           MODEL_RUNTIME_CLASS_22) &&
         ((entityRuntime->common).ownership.ownerIndex ==
          g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->factionIndex))
      {
        SelectionPointerArray_InsertUniqueAndRecenter
                  (entityRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection)
        ;
      }
    }
  }
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}

/* In-game command handler INGAME_COMMAND_REPLACE_SELECTION: replaces the player's selection with all world
   entries matching the army at the rebased index (byte offset from g_ArmyRuntimeRebaseBaseMinusOne, 0 = none)
   and refreshes the local selection panels. Nothing is added when the army has no model node.
*/
void InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,uint32_t unusedPayload2,
          RuntimeToken armyRuntimeIndex)

{
  ArmyRuntimeSlot *sourceArmyRuntime;

  if (armyRuntimeIndex != 0) {
    sourceArmyRuntime = (ArmyRuntimeSlot *)((uintptr_t)g_ArmyRuntimeRebaseBaseMinusOne + armyRuntimeIndex);
    SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
    if (sourceArmyRuntime->modelNodeRuntime != NULL) {
      SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
                (sourceArmyRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
      if (playerId == g_LocalPlayerRuntimeId) {
        InGameSelectionDetailPanel_Rebuild();
        InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      }
    }
  }
  return;
}

/* In-game command handler INGAME_COMMAND_MOVE (plain click on the ground): sends the player's
   selection to the world point, each entry keeping its formation offset unless the selection is spread too wide.
   A lone class-0x0D entry takes the point into its definition record instead and the selection is cleared.
*/
void InGamePlayerSelection_ApplyMoveCommand
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,CommandPayload worldXQ12,
          CommandPayload worldYQ12)

{
  SelectionPointerArray_ApplyMoveCommand
            (worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* In-game command handler INGAME_COMMAND_POSITION (Shift/Alt-click on the ground): queues the world point as a
   waypoint for every entry of the player's selection (formation offsets as in the plain move).
*/
void InGamePlayerSelection_ApplyPositionCommand
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,CommandPayload worldXQ12,
          CommandPayload worldYQ12)

{
  SelectionPointerArray_ApplyPositionCommand
            (worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* In-game command handler INGAME_COMMAND_SELECT_ARMY (click on an army as an order target): makes the army at
   the rebased index the command target of every eligible entry of the player's selection. Ignored for index 0
   and for armies without a model node.
*/
void InGamePlayerSelection_SelectArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,uint32_t unusedPayload2,
          RuntimeToken armyRuntimeIndex)

{
  if ((armyRuntimeIndex != 0) &&
     (((ArmyRuntimeSlot *)((uintptr_t)g_ArmyRuntimeRebaseBaseMinusOne + armyRuntimeIndex))->
      modelNodeRuntime != NULL)) {
    SelectionPointerArray_ApplyArmyRuntimeTarget
              ((ArmyRuntimeSlot *)((uintptr_t)g_ArmyRuntimeRebaseBaseMinusOne + armyRuntimeIndex),
               &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  }
  return;
}

/* In-game command handler INGAME_COMMAND_TARGET_POSITION (Ctrl-click on the ground): gives every eligible
   entry of the player's selection the terrain point (surface height, x, y) as its target position.
*/
void InGamePlayerSelection_ApplyTargetPositionCommand(PlayerRuntimeId playerId,CommandPayload surfaceHeightQ12,
          CommandPayload worldXQ12,CommandPayload worldYQ12)

{
  SelectionPointerArray_ApplyTargetPositionCommand
            (surfaceHeightQ12,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* In-game command handler 0xE10 (key S, stop): resets the movement of the player's selection, drops its
   class-0x16 entries and recenters the formation offsets (SelectionRuntime_ResetMovementPruneAndRecenterEntries).
*/
void PlayerSelection_ResetMovementPruneAndRecenterEntries(PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_ResetMovementPruneAndRecenterEntries
            ((Ptr32<GameEntityRuntime> *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* In-game command handler 0xE30 (Shift+S): resets the movement anchors of the eligible entries of the player's
   selection and clears their command flag 0x200. The block pointer doubles as its selection array (first
   member).
*/
void PlayerSelection_StopMovement
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_StopMovement
            ((Ptr32<GameEntityRuntime> *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* In-game command handler 0xE50 (Alt+S): interrupts the active targets of the eligible entries of the player's
   selection and clears flag 0x10 of their commandModeFlags.
*/
void PlayerSelection_CancelTargets
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_CancelTargets
            ((Ptr32<GameEntityRuntime> *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* In-game command handler 0xE70 (Alt+D): applies the model hierarchy flags 0x418 to the eligible entries of the
   player's selection (SelectionRuntime_SelfDestruct).
*/
void PlayerSelection_SelfDestruct
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3)

{
  SelectionRuntime_SelfDestruct
            ((Ptr32<GameEntityRuntime> *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* Pointer-mode handler for lane 1 (g_InGamePointerModeHandlers[1], chosen in gameplay/input/world.c when the
   modifier mask (no modifier = 7, Shift = 1) and the attachment variant mask leave 1; networked as command code
   0xE90): stores the pointed world point and preview heading as marker lane 1 of every class-0x16 entity in
   the player's selection (SelectionPointerArray_SetAircraftPadTargets).
*/
void InGameSelection_SetAircraftPadTargetLane1
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (1,heading16,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  return;
}

/* Pointer-mode handler for lane 2 (g_InGamePointerModeHandlers[2]: modifier mask & attachment variant mask
   == 2, Alt = 2; networked as command code 0xEC0): like InGameSelection_SetAircraftPadTargetLane1, for marker lane 2.
*/
void InGameSelection_SetAircraftPadTargetLane2
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (2,heading16,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  return;
}

/* Move command for a selection (ArmyRuntime_StartRoutedMoveCommand per entity): each entity is sent to
   the target shifted by its offset from the selection's centre, so the group keeps its formation, unless the
   selection is spread too widely, then all go to the target itself. If the selection is exactly one class-0xD
   entity (a production structure, cf. gameplay/faction/runtime.c), the target becomes its point in the model
   runtime's classLinkState.classState78/7C (flag 0x800 in classState.stateFlags) and the selection is cleared.
*/
void SelectionPointerArray_ApplyMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  ModelRuntimeSlot *class13Record;
  int entryIndex;
  Q12 entryTargetY;
  int selectedEntryCount;
  Q12 entryTargetX;
  GameEntityRuntime *selectedEntry;
  GameEntityRuntime *singleClass13Entry;
  Bool8 spreadTooLarge;
  ModelDefinition *entityDefinition;

  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLarge(selection);
  entryTargetY = targetWorldY;
  entryTargetX = targetWorldX;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    movementRuntime = (ArmyMovementRuntime *)selection->entries[entryIndex];
    if (movementRuntime == NULL) {
      continue;
    }
    /* classState60/ownerValue64 are the entity's selection offsets (common.selectionOffsetXQ12/YQ12,
       centre - position)
       written by SelectionPointerArray_RecenterOffsetsAroundAveragePosition */
    if (!spreadTooLarge) {
      entryTargetX = entryTargetX - movementRuntime->classState60;
      entryTargetY = entryTargetY - movementRuntime->ownerValue64;
    }
    ArmyRuntime_StartRoutedMoveCommand(entryTargetY,entryTargetX,movementRuntime);
    if (!spreadTooLarge) {
      entryTargetX = entryTargetX + movementRuntime->classState60;
      entryTargetY = entryTargetY + movementRuntime->ownerValue64;
    }
  }
  selectedEntryCount = 0;
  singleClass13Entry = NULL;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = selection->entries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    selectedEntryCount++;
    entityDefinition = THANDOR_PTR32_AT(ModelDefinition, (selectedEntry->common).ownership.definitionOrClassRecord);
    if (entityDefinition->accelerationPerTick != 0) {
      return;
    }
    if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
      singleClass13Entry = selectedEntry;
    }
  }
  if ((selectedEntryCount == 1) && (singleClass13Entry != NULL)) {
    class13Record = (ModelRuntimeSlot *)(singleClass13Entry->common).ownership.definitionOrClassRecord;
    class13Record->classLinkState.classState78 = targetWorldX;
    class13Record->classLinkState.classState7C = targetWorldY;
    class13Record->classState.stateFlags = class13Record->classState.stateFlags | ARMY_MODEL_STATE_RALLY_POINT_SET;
    SelectionPointerArray_Clear32(selection);
  }
}

/* Stops the selected entities: every entity without command flag 0x2 has its movement reset to its current
   model position and command-mode bit 0x10 and movement bit 0x200 cleared; class-0x16 entities are dropped
   from the selection. The formation offsets are then recomputed, and a selection of exactly one class-0xD
   entity gets its point in the model runtime's classLinkState.classState78/7C reset to its model's lookup point
   (1,5) (flag 0x800 cleared) and the selection cleared.
*/
void SelectionRuntime_ResetMovementPruneAndRecenterEntries(Ptr32<GameEntityRuntime> *selectionEntries)

{
  ModelRuntimeSlot *entryModelRuntime;
  ModelDefinition *entityDefinition;
  ModelRuntimeSlot *class13Record;
  ModelRuntimeNode *modelNodeRuntime;
  int entryIndex;
  int selectedEntryCount;
  ArmyRuntimeSlot *entryArmy;
  GameEntityRuntime *selectedEntry;
  GameEntityRuntime *singleClass13Entry;
  ModelPackedPointRecord *anchorRecord;
  ModelWorldPoint localPoint;

  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entryArmy = (ArmyRuntimeSlot *)selectionEntries[entryIndex];
    if ((entryArmy == NULL) || ((entryArmy->movementStateFlags & ARMY_MOVEMENT_LOCKED) != 0)) {
      continue;
    }
    ArmyRuntime_ResetMovementStateFromModel(entryArmy);
    entryArmy->commandModeFlags = entryArmy->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
    entryArmy->movementStateFlags = entryArmy->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
    entryModelRuntime = entryArmy->modelRuntimeOrSavedOffset.modelRuntime;
    if ((entryModelRuntime->definitionOrSavedId).runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      selectionEntries[entryIndex] = NULL;
      (entryModelRuntime->classState).classStateDC = 0;
    }
  }
  selectedEntryCount = 0;
  singleClass13Entry = NULL;
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            ((SelectionPointerArray32 *)selectionEntries);
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = selectionEntries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    selectedEntryCount++;
    entityDefinition = THANDOR_PTR32_AT(ModelDefinition, (selectedEntry->common).ownership.definitionOrClassRecord);
    if (entityDefinition->accelerationPerTick != 0) {
      return;
    }
    if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
      singleClass13Entry = selectedEntry;
    }
  }
  if ((selectedEntryCount == 1) && (singleClass13Entry != NULL)) {
    class13Record = (ModelRuntimeSlot *)(singleClass13Entry->common).ownership.definitionOrClassRecord;
    modelNodeRuntime = (singleClass13Entry->common).ownership.modelNode;
    class13Record->classState.stateFlags = class13Record->classState.stateFlags & ~ARMY_MODEL_STATE_RALLY_POINT_SET;
    if (ModelLookupTable_FindPackedPoint(1,5,(modelNodeRuntime->modelPayload).modelResource,&anchorRecord)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(anchorRecord,modelNodeRuntime);
      class13Record->classLinkState.classState78 = localPoint.xQ12;
      class13Record->classLinkState.classState7C = localPoint.yQ12;
      SelectionPointerArray_Clear32((SelectionPointerArray32 *)selectionEntries);
    }
  }
}

/* Waypoint move for a selection (ArmyRuntime_AppendWaypointOrStartMove per entity): like
   SelectionPointerArray_ApplyMoveCommand each entity gets the target shifted by its formation
   offset, unless the selection is spread too widely, but without the class-0xD special case.
*/
void SelectionPointerArray_ApplyPositionCommand(Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  int entriesRemaining;
  Bool8 spreadTooLarge;

  /* selection is advanced as a cursor over its entries; the targets are shifted per entry and restored */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  spreadTooLarge = SelectionPointerArray_IsSpatialSpreadTooLarge(selection);
  do {
    movementRuntime = THANDOR_PTR32_AT(ArmyMovementRuntime, selection);
    if (movementRuntime != NULL) {
      if (!spreadTooLarge) {
        targetWorldX = targetWorldX - movementRuntime->classState60;
        targetWorldY = targetWorldY - movementRuntime->ownerValue64;
      }
      ArmyRuntime_AppendWaypointOrStartMove(targetWorldY,targetWorldX,movementRuntime);
      if (!spreadTooLarge) {
        targetWorldX = targetWorldX + movementRuntime->classState60;
        targetWorldY = targetWorldY + movementRuntime->ownerValue64;
      }
    }
    selection = (SelectionPointerArray32 *)&selection->entries[1]; /* next entry */
    entriesRemaining--;
  } while (entriesRemaining != 0);
}

/* Orders a selection onto a target entity: every entity with a non-zero state (stateOrTechnologyId) gets
   targetArmyRuntime as its command target (ArmyRuntime_ResolveCommandTarget), stored again in
   assignedTargetArmyRuntime, command-mode bits 0x14 set and
   movement bit 0x200 cleared.
*/
void SelectionPointerArray_ApplyArmyRuntimeTarget(ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int entriesRemaining;
  Bool8 stateIsZero;

  /* selection is advanced as a cursor over its entries */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    runtimeState = THANDOR_PTR32_AT(ArmyRuntimeSlot, selection);
    if (runtimeState != NULL) {
      stateIsZero = ArmyRuntime_TestHasNoWeaponDamage(runtimeState);
      if (!stateIsZero) {
        ArmyRuntime_ResolveCommandTarget(targetArmyRuntime,runtimeState);
        runtimeState->assignedTargetArmyRuntime = Thandor_PointerToU32(targetArmyRuntime); /* 5f-format: ArmyRuntimeSlot.assignedTargetArmyRuntime */
        runtimeState->commandModeFlags = runtimeState->commandModeFlags |
                                       (ARMY_COMMAND_MODE_SELECTION_ORDER | ARMY_COMMAND_MODE_INTERRUPTED);
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
      }
    }
    selection = (SelectionPointerArray32 *)&selection->entries[1]; /* next entry */
    entriesRemaining--;
  } while (entriesRemaining != 0);
}

/* Orders a selection onto a target position: every entity with a non-zero state (stateOrTechnologyId) gets the
   three
   command coordinates (ArmyRuntime_ApplyTargetPositionCommand), command-mode bits 0x14 set, movement bit 0x200
   cleared and its command generation shifted left by 2.
*/
void SelectionPointerArray_ApplyTargetPositionCommand
          (Q12 coordinateA,uint32_t coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *runtimeState;
  int entriesRemaining;
  Bool8 stateIsZero;

  /* selection is advanced as a cursor over its entries */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    runtimeState = THANDOR_PTR32_AT(ArmyRuntimeSlot, selection);
    if (runtimeState != NULL) {
      stateIsZero = ArmyRuntime_TestHasNoWeaponDamage(runtimeState);
      if (!stateIsZero) {
        ArmyRuntime_ApplyTargetPositionCommand(coordinateA,coordinateB,coordinateC,runtimeState);
        runtimeState->commandModeFlags = runtimeState->commandModeFlags |
                                       (ARMY_COMMAND_MODE_SELECTION_ORDER | ARMY_COMMAND_MODE_INTERRUPTED);
        runtimeState->movementStateFlags = runtimeState->movementStateFlags & ~ARMY_MOVEMENT_ROUTED;
        runtimeState->commandGeneration = runtimeState->commandGeneration << 2;
      }
    }
    selection = (SelectionPointerArray32 *)&selection->entries[1]; /* next entry */
    entriesRemaining--;
  } while (entriesRemaining != 0);
}

/* For every selected entity without command flag 0x2: resets its movement flags and anchor coordinates to the
   current model position (GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel) and clears command
   flag 0x200.
*/
void SelectionRuntime_StopMovement(Ptr32<GameEntityRuntime> *selectionEntries)

{
  GameEntityCommandFlags *commandFlagsPtr;
  GameEntityRuntime *entityRuntime;
  int entriesRemaining;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    entityRuntime = *selectionEntries;
    if ((entityRuntime != NULL) &&
       (((entityRuntime->common).commandFlags & ARMY_MOVEMENT_LOCKED) == 0)) {
      GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(entityRuntime);
      commandFlagsPtr = &(entityRuntime->common).commandFlags;
      *commandFlagsPtr = *commandFlagsPtr & ~(uint32_t)ARMY_MOVEMENT_ROUTED;
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
}

/* For every selected entity without command flag 0x2: drops an active attack/follow target
   (ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration) and clears command-mode bit 0x10.
*/
void SelectionRuntime_CancelTargets(Ptr32<GameEntityRuntime> *selectionEntries)

{
  GameEntityRuntime *armyRuntime;
  int entriesRemaining;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  do {
    armyRuntime = *selectionEntries;
    if ((armyRuntime != NULL) &&
       ((((ArmyRuntimeSlot *)armyRuntime)->movementStateFlags & ARMY_MOVEMENT_LOCKED) == 0)) {
      ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration((ArmyRuntimeSlot *)armyRuntime);
      ((ArmyRuntimeSlot *)armyRuntime)->commandModeFlags =
           ((ArmyRuntimeSlot *)armyRuntime)->commandModeFlags & ~(uint32_t)ARMY_COMMAND_MODE_SELECTION_ORDER;
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
}

/* For every selected entity without command flag 0x2: sets runtime flags 0x418 on all nodes of its model
   hierarchy that do not have flag 0x08 yet (ModelRuntimeHierarchy_MarkDestroyedRecursive).
*/
void SelectionRuntime_SelfDestruct(Ptr32<GameEntityRuntime> *selectionEntries)

{
  GameEntityRuntime *modelRuntime;
  int entriesRemaining;
  WorldRuntimeContext *contextArg;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  contextArg = &g_InGameRuntimeRoot->worldRuntime;
  do {
    modelRuntime = *selectionEntries;
    if ((modelRuntime != NULL) &&
       (((modelRuntime->common).commandFlags & 2) == 0)) {
      ModelRuntimeHierarchy_MarkDestroyedRecursive(contextArg,(ArmyRuntimeSlot *)modelRuntime);
    }
    selectionEntries = selectionEntries + 1;
    entriesRemaining--;
  } while (entriesRemaining != 0);
}

/* For every selected entity whose definition class is 0x16, counts how often each of the three lane asset ids
   (g_InGamePointerModePreviewArmyIds[1], [2] and [4]) occurs among the
   13 child asset ids completedSecondaryArmyAssetIds of the model runtime. For every lane bit set in laneMask
   (1, 2, 4) it stores that lane's count byte (linkedChildPendingSpawnCounts.slot0..2) and the point
   (worldYQ12, worldXQ12, heading16) in linkedChildSpawnInheritedState[lane].
   Called by the pointer-mode handlers InGameSelection_SetAircraftPadTargetLane1/2 and
   SelectionMarkerCoordinates_ApplyType3..7.
*/
void SelectionPointerArray_SetAircraftPadTargets
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12,
          SelectionPointerArray32 *selection)

{
  ModelRuntimeLinkedChildSpawnAndBuildView *padRuntime;
  GameEntityRuntime *selectedEntry;
  int markerSourceId;
  int entryIndex;
  int packedMarkerMatches;
  int markerSlotIndex;

  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = selection->entries[entryIndex];
    if (selectedEntry == NULL) {
      continue;
    }
    /* entry -> model runtime (dword 0) -> definition */
    padRuntime = THANDOR_PTR32_AT(ModelRuntimeLinkedChildSpawnAndBuildView, selectedEntry);
    if (padRuntime->modelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      /* one match counter per byte: lane 1 in bits 0-7, lane 2 in bits 8-15, lane 4 in bits 16-23 */
      packedMarkerMatches = 0;
      for (markerSlotIndex = 12; markerSlotIndex >= 0; markerSlotIndex--) {
        markerSourceId = padRuntime->completedSecondaryArmyAssetIds[markerSlotIndex];
        if (markerSourceId == g_InGamePointerModePreviewArmyIds[1]) {
          packedMarkerMatches = packedMarkerMatches + SELECTION_PACKED_LANE_ONE(0);
        }
        if (markerSourceId == g_InGamePointerModePreviewArmyIds[2]) {
          packedMarkerMatches = packedMarkerMatches + SELECTION_PACKED_LANE_ONE(1);
        }
        if (markerSourceId == g_InGamePointerModePreviewArmyIds[4]) {
          packedMarkerMatches = packedMarkerMatches + SELECTION_PACKED_LANE_ONE(2);
        }
      }
      if ((laneMask & 1) != 0) {
        /* the lane's match count becomes its pending launch count; the point its launch target */
        padRuntime->linkedChildPendingSpawnCounts.slot0 = (char)packedMarkerMatches;
        padRuntime->linkedChildSpawnInheritedState[0].inheritedValue70 = worldYQ12;
        padRuntime->linkedChildSpawnInheritedState[0].inheritedValue74 = worldXQ12;
        padRuntime->linkedChildSpawnInheritedState[0].inheritedValue78 = heading16;
      }
      if ((laneMask & 2) != 0) {
        padRuntime->linkedChildPendingSpawnCounts.slot1 = (char)((uint32_t)packedMarkerMatches >> 8);
        padRuntime->linkedChildSpawnInheritedState[1].inheritedValue70 = worldYQ12;
        padRuntime->linkedChildSpawnInheritedState[1].inheritedValue74 = worldXQ12;
        padRuntime->linkedChildSpawnInheritedState[1].inheritedValue78 = heading16;
      }
      if ((laneMask & 4) != 0) {
        padRuntime->linkedChildPendingSpawnCounts.slot2 = (char)((uint32_t)packedMarkerMatches >> 16);
        padRuntime->linkedChildSpawnInheritedState[2].inheritedValue70 = worldYQ12;
        padRuntime->linkedChildSpawnInheritedState[2].inheritedValue74 = worldXQ12;
        padRuntime->linkedChildSpawnInheritedState[2].inheritedValue78 = heading16;
      }
    }
  }
  return;
}

/* Pointer-mode handler 3 (g_InGamePointerModeHandlers[3]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 3 (lanes 1 and 2) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType3(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (3,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Pointer-mode handler 4 (g_InGamePointerModeHandlers[4]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 4 (lane 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType4(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (4,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Pointer-mode handler 5 (g_InGamePointerModeHandlers[5]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 5 (lanes 1 and 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType5(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (5,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Pointer-mode handler 6 (g_InGamePointerModeHandlers[6]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 6 (lanes 2 and 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType6(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (6,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Pointer-mode handler 7 (g_InGamePointerModeHandlers[7]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 7 (lanes 1, 2 and 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType7(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (7,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

InGamePointerModeHandler *g_InGamePointerModeHandlers[8] = {
    /* 0 */ 0,
    /* 1 */ THANDOR_FN(InGameSelection_SetAircraftPadTargetLane1),
    /* 2 */ THANDOR_FN(InGameSelection_SetAircraftPadTargetLane2),
    /* 3 */ THANDOR_FN(SelectionMarkerCoordinates_ApplyType3),
    /* 4 */ THANDOR_FN(SelectionMarkerCoordinates_ApplyType4),
    /* 5 */ THANDOR_FN(SelectionMarkerCoordinates_ApplyType5),
    /* 6 */ THANDOR_FN(SelectionMarkerCoordinates_ApplyType6),
    /* 7 */ THANDOR_FN(SelectionMarkerCoordinates_ApplyType7)};
