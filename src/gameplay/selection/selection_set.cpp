/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/selection_set.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/selection_set.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/selection/selection_set. */

/* In-game command handler 0x2F20: moves the player's primary selected model (block placedArmyToken, rebased
   offset, 0 = none) by a pointer-drag delta, writes the new point into its path and tracked coordinates and the
   model transform, and lets the definition's placement contact kind (placementContactKindIndex) set its height
   before the transforms and depth bins are rebuilt. Sent by InGameUiCommand_UpdateInteractionByMode
   (ui/ingame/runtime.c) while the pointer drags with bit 0x4 of g_CursorButtonState set.
*/
void SelectionPlayerRuntime_MovePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved,Q12 deltaYQ12,Q12 deltaXQ12)

{
  uint32_t primaryEntityOffset;
  ModelRuntimeNode *modelNode;
  ModelDefinition *definition;
  int placementContactKind;
  int newWorldXQ12;
  GameEntityRuntime *target;
  int newWorldYQ12;
  WorldRuntimeContext *worldRuntime;

  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  primaryEntityOffset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken;
  if (primaryEntityOffset != 0) {
    target = (GameEntityRuntime *)((uintptr_t)g_ArmyRuntimeRebaseBaseMinusOne + primaryEntityOffset);
    /* the result is ignored: the primary entity is moved whether or not it is still selected */
    SelectionPointerArray_Contains
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNode = (target->common).ownership.modelNode;
    newWorldXQ12 = deltaXQ12 + (modelNode->worldTransform).translation.x;
    newWorldYQ12 = deltaYQ12 + (modelNode->worldTransform).translation.y;
    definition = THANDOR_PTR32_AT(ModelDefinition, (target->common).ownership.definitionOrClassRecord);
    (target->common).pathCoordinate0Q12 = newWorldXQ12;
    (target->common).pathCoordinate1Q12 = newWorldYQ12;
    (target->common).trackedCoordinate0Q12 = newWorldXQ12;
    (target->common).trackedCoordinate1Q12 = newWorldYQ12;
    (target->common).damageState.trackedCoordinate0Q12 = newWorldXQ12;
    (target->common).damageState.trackedCoordinate1Q12 = newWorldYQ12;
    (modelNode->worldTransform).translation.x = newWorldXQ12;
    placementContactKind = definition->placementContactKindIndex;
    (modelNode->worldTransform).translation.y = newWorldYQ12;
    if (placementContactKind == ARMY_PLACEMENT_CONTACT_KIND_ARTICULATED_SUSPENSION) {
      ((modelNode->runtimePayload).armyRuntime)->movementTarget0Q12 = INT32_MAX;
    }
    g_ArmyPlacementContactKindDispatchTable.callbacks[placementContactKind]
              (definition->placementHeightOffsetQ12,newWorldYQ12,newWorldXQ12,modelNode,worldRuntime);
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
    ModelNodeRuntime_UpdateDepthBinMasks(definition->footprintRadius,modelNode);
  }
  return;
}

/* In-game command handler 0x30F0: turns the player's primary selected model (block placedArmyToken) by angleDelta
   (16-bit angle, wraps) and rebuilds its transforms. Sent by InGameUiCommand_UpdateInteractionByMode
   (ui/ingame/runtime.c) with the horizontal pointer drag * 64.
*/
void SelectionPlayerRuntime_RotatePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved0,uint32_t reserved1,AngleTurn32 angleDelta)

{
  uint32_t primaryEntityOffset;
  ModelRuntimeNode *modelNodeRuntime;
  GameEntityRuntime *target;

  primaryEntityOffset = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken;
  if (primaryEntityOffset != 0) {
    target = (GameEntityRuntime *)((uintptr_t)g_ArmyRuntimeRebaseBaseMinusOne + primaryEntityOffset);
    /* the result is ignored, as in SelectionPlayerRuntime_MovePrimarySelectionBy */
    SelectionPointerArray_Contains
              (target,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNodeRuntime = (target->common).ownership.modelNode;
    (modelNodeRuntime->modelPayload).worldRotationAngle2 =
         angleDelta + (modelNodeRuntime->modelPayload).worldRotationAngle2 & FIXED_ANGLE16_MASK;
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}

/* Removes an entity from the selections of all eight players (every matching entry of each player block's
   32-entry selection becomes NULL), so no selection keeps pointing at an entity that is being destroyed.
*/
void SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target)

{
  int entriesRemainingInBlock;
  int playerBlocksRemaining;
  SelectionPlayerRuntimeBlock *currentSelectionEntry;
  SelectionPlayerRuntimeBlock *selectionEntryCursor;

  /* The cursor is typed as a block but walks the selection entries one pointer at a time: entries[0] of the
     cursor is the current entry. */
  playerBlocksRemaining = 8;
  entriesRemainingInBlock = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionPlayerBlocks;
  do {
    do {
      currentSelectionEntry = selectionEntryCursor;
      if (target == (currentSelectionEntry->selection).entries[0]) {
        (currentSelectionEntry->selection).entries[0] = NULL;
      }
      entriesRemainingInBlock--;
      selectionEntryCursor =
           (SelectionPlayerRuntimeBlock *)((currentSelectionEntry->selection).entries + 1);
    } while (entriesRemainingInBlock != 0);
    entriesRemainingInBlock = SELECTION_ENTRY_CAPACITY;
    playerBlocksRemaining--;
    /* currentSelectionEntry points at the last entry (entries[31]); seen through that pointer,
       chatRecipientMaskAndWriteOffset lies exactly sizeof(SelectionPlayerRuntimeBlock) further on, at the first
       entry of the next block */
    selectionEntryCursor =
         (SelectionPlayerRuntimeBlock *)&currentSelectionEntry->chatRecipientMaskAndWriteOffset;
  } while (playerBlocksRemaining != 0);
}

/* Removes an entity from one 32-entry selection array: only the first matching entry is set to NULL
   (an entity is in a selection at most once).
*/
void SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entryIndex;

  /* search the 32 entries; the first match is cleared. */
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    if (array->entries[entryIndex] == target) {
      array->entries[entryIndex] = NULL;
      return;
    }
  }
}

/* In-game command handler 0x1ED0: empties the player's marked-cell list (the field cells collected by
   PlayerPairList_InsertRange) and, for the local player, the in-game root's copy of its count
   (localPlayerMarkedCellCount). Sent by InGameUiCommand_BeginInteractionByMode and
   InGameUiCommand_ResetInteractionByMode (ui/ingame/runtime.c).
*/
void SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2)

{
  InGameRuntimeRoot *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount = 0;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    inGameRuntimeRoot->localPlayerMarkedCellCount = 0;
  }
  return;
}

/* Tells whether the field cell (worldXQ12, worldYQ12) is in the player's marked-cell list (see
   PlayerPairList_InsertUnique): false when listed, true when not. Used by the FieldGrid cell
   updates in world/terrain/grid.c.
*/
Bool8 SelectionPlayerPairList_ContainsPair(SelectionPlayerPairValue worldYQ12,SelectionPlayerPairKey worldXQ12,
          PlayerRuntimeId playerRuntimeId)

{
  uint32_t pairRecordsRemaining;
  SelectionPlayerPairRecord *pairRecordCursor;

  pairRecordsRemaining = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount;
  pairRecordCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCells;
  for (; pairRecordsRemaining != 0; pairRecordsRemaining--) {
    if ((worldXQ12 == pairRecordCursor->pairKey) && (worldYQ12 == pairRecordCursor->pairValue)) {
      return false;
    }
    pairRecordCursor++;
  }
  return true;
}

/* Adds to a selection every entity in the world of the same army type (army asset id) and faction as
   sourceArmyRuntime, i.e. "select all units of this kind"; each insertion recomputes the formation offsets.
*/
void SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
          (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection)

{
  PckArmyAssetIdCatalog sourceArmyAssetId;
  int sourceFactionIndex;
  GameEntityRuntime *entityRuntime;
  WorldOwnerListNode *ownerNode;

  sourceArmyAssetId = sourceArmyRuntime->armyAssetId;
  sourceFactionIndex = sourceArmyRuntime->factionIndex;
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    /* model payload: the ModelRuntimeSlot; its owner army is the entity */
    entityRuntime =
         (GameEntityRuntime *)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if ((sourceArmyAssetId == (entityRuntime->common).runtimeIdentityOrArmyAssetId) &&
       (sourceFactionIndex == (entityRuntime->common).ownership.ownerIndex)) {
      SelectionPointerArray_InsertUniqueAndRecenter(entityRuntime,selection);
    }
  }
}

/* Adds an entity to a 32-entry selection (into the first free entry, unless it is already in it or the
   selection is full) and recomputes every entry's formation offset from the new centre.
*/
void SelectionPointerArray_InsertUniqueAndRecenter(GameEntityRuntime *entityRuntime,SelectionPointerArray32 *selection)

{
  int entryIndex;

  /* Two search passes: look for entityRuntime, and when absent store it in the first null slot. */
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    if (selection->entries[entryIndex] == entityRuntime) break;
  }
  if (entryIndex == SELECTION_ENTRY_CAPACITY) {
    for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
      if (selection->entries[entryIndex] == NULL) {
        selection->entries[entryIndex] = entityRuntime;
        break;
      }
    }
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition(selection);
}

/* Computes the centre (average model world X/Y) of a selection and stores for every entity its offset
   centre - position in common.selectionOffsetXQ12/YQ12; move orders subtract that offset from
   the target so the group keeps its formation.
*/
void SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection)

{
  ModelRuntimeNode *modelNode;
  GameEntityRuntime *entry;
  int averageXQ12;
  int averageYQ12;
  int selectedCount;
  int entryIndex;

  averageXQ12 = 0;
  averageYQ12 = 0;
  selectedCount = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entry = selection->entries[entryIndex];
    if (entry != NULL) {
      modelNode = (entry->common).ownership.modelNode;
      selectedCount++;
      averageXQ12 = averageXQ12 + modelNode->worldTransform.translation.x;
      averageYQ12 = averageYQ12 + modelNode->worldTransform.translation.y;
    }
  }
  if (selectedCount == 0) {
    return;
  }
  averageXQ12 = averageXQ12 / selectedCount;
  averageYQ12 = averageYQ12 / selectedCount;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entry = selection->entries[entryIndex];
    if (entry != NULL) {
      modelNode = (entry->common).ownership.modelNode;
      (entry->common).selectionOffsetXQ12 = averageXQ12 - modelNode->worldTransform.translation.x;
      (entry->common).selectionOffsetYQ12 = averageYQ12 - modelNode->worldTransform.translation.y;
    }
  }
}

/* Tells whether target is one of the 32 entries of a selection array: false when found, true when not. Used by
   FrontendPlayerSelection_ApplyEntryOrAll (ui/frontend/player.c); the primary-selection move/rotate handlers call
   it and ignore the result.
*/
Bool8 SelectionPointerArray_Contains(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  int entryIndex;

  /* search the 32 entries: false when target was found. */
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    if (array->entries[entryIndex] == target) {
      return false;
    }
  }
  return true;
}

/* Tells the move commands whether a selection is too scattered to keep its formation: true when the
   bounding box of the entities' selection offsets (common.selectionOffsetXQ12/YQ12) is wider
   than 5.0 (Q12 0x5000) on either axis or the two extents add up to more than 7.0 (0x7000). An empty
   selection returns false.
*/
Bool8 SelectionPointerArray_IsSpatialSpreadTooLarge(SelectionPointerArray32 *selection)

{
  GameEntityRuntime *entry;
  int minOffsetX;
  int maxOffsetX;
  int minOffsetY;
  int maxOffsetY;
  int entryIndex;

  /* the first non-empty entry seeds the bounds */
  entryIndex = 0;
  while (selection->entries[entryIndex] == NULL) {
    entryIndex++;
    if (entryIndex == SELECTION_ENTRY_CAPACITY) {
      return false;
    }
  }
  entry = selection->entries[entryIndex];
  minOffsetX = entry->common.selectionOffsetXQ12;
  maxOffsetY = entry->common.selectionOffsetYQ12;
  maxOffsetX = minOffsetX;
  minOffsetY = maxOffsetY;
  for (; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    entry = selection->entries[entryIndex];
    if (entry == NULL) {
      continue;
    }
    if (entry->common.selectionOffsetXQ12 < minOffsetX) {
      minOffsetX = entry->common.selectionOffsetXQ12;
    }
    if (entry->common.selectionOffsetYQ12 < minOffsetY) {
      minOffsetY = entry->common.selectionOffsetYQ12;
    }
    if (maxOffsetX < entry->common.selectionOffsetXQ12) {
      maxOffsetX = entry->common.selectionOffsetXQ12;
    }
    if (maxOffsetY < entry->common.selectionOffsetYQ12) {
      maxOffsetY = entry->common.selectionOffsetYQ12;
    }
  }
  if (((maxOffsetX - minOffsetX < 5 * Q12_ONE + 1) && (maxOffsetY - minOffsetY < 5 * Q12_ONE + 1)) &&
     ((maxOffsetX - minOffsetX) + (maxOffsetY - minOffsetY) < 7 * Q12_ONE + 1)) {
    return false;
  }
  return true;
}

/* Empties a 32-entry selection array (all entries NULL).
*/
void SelectionPointerArray_Clear32(SelectionPointerArray32 *array)

{
  int entriesRemaining;

  /* array is advanced as a cursor over its entries */
  for (entriesRemaining = SELECTION_ENTRY_CAPACITY; entriesRemaining != 0; entriesRemaining--) {
    array->entries[0] = NULL;
    array = (SelectionPointerArray32 *)&array->entries[1];
  }
}
