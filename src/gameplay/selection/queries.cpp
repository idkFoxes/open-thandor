/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/queries.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/queries.h>
#include <thandor/thandor.h>

/* Module data. */

SelectionInfoEntitySlots *g_SelectionInfoEntitySlots = nullptr;

/* Writes the average world position (model node translation) of the local selection's entities to
   *outPosition and returns true; returns false when the selection is empty (*outPosition is then all 0).
*/
Bool8 SelectionInfoEntitySlots_ComputeAverageWorldPosition(FixedVectorQ12 *outPosition)

{
  ModelRuntimeNode *slotModelNode;
  int worldXAggregateQ12;
  int worldYAggregateQ12;
  int worldZAggregateQ12;
  Ptr32<GameEntityRuntime> *selectionEntitySlotCursor;
  int selectedEntityCount;
  int selectionSlotsRemaining;

  worldXAggregateQ12 = 0;
  worldYAggregateQ12 = 0;
  worldZAggregateQ12 = 0;
  selectionSlotsRemaining = SELECTION_ENTRY_CAPACITY;
  selectedEntityCount = 0;
  selectionEntitySlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntitySlotCursor != nullptr) {
      slotModelNode = ((*selectionEntitySlotCursor)->common).ownership.modelNode;
      worldXAggregateQ12 = worldXAggregateQ12 + (slotModelNode->worldTransform).translation.x;
      worldYAggregateQ12 = worldYAggregateQ12 + (slotModelNode->worldTransform).translation.y;
      worldZAggregateQ12 = worldZAggregateQ12 + (slotModelNode->worldTransform).translation.z;
      selectedEntityCount++;
    }
    selectionEntitySlotCursor++;
    selectionSlotsRemaining--;
  } while (selectionSlotsRemaining != 0);
  if (selectedEntityCount != 0) {
    worldXAggregateQ12 = worldXAggregateQ12 / selectedEntityCount;
    worldYAggregateQ12 = worldYAggregateQ12 / selectedEntityCount;
    worldZAggregateQ12 = worldZAggregateQ12 / selectedEntityCount;
  }
  outPosition->xQ12 = worldXAggregateQ12;
  outPosition->yQ12 = worldYAggregateQ12;
  outPosition->zQ12 = worldZAggregateQ12;
  return selectedEntityCount != 0;
}

/* Returns true when the local selection holds at least one entity, false when it is empty.
*/
Bool8 SelectionInfo_HasAnyEntry()

{
  int entriesRemaining;
  Ptr32<GameEntityRuntime> *selectionEntryCursor;
  Bool8 entryIsEmpty;
  GameEntityRuntime *currentEntry;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  entryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  /* skip the empty entries */
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining--;
    currentEntry = *selectionEntryCursor;
    entryIsEmpty = currentEntry == nullptr;
    selectionEntryCursor++;
  } while (entryIsEmpty);
  return !entryIsEmpty;
}

/* Returns false when every entity of the local selection belongs to the faction ownerIndex (an empty
   selection passes), true as soon as one belongs to another faction.
*/
Bool8 SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex)

{
  int entriesRemaining;
  Ptr32<GameEntityRuntime> *selectionEntryCursor;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  while ((*selectionEntryCursor == nullptr ||
         (ownerIndex == ((*selectionEntryCursor)->common).ownership.ownerIndex))) {
    selectionEntryCursor++;
    entriesRemaining--;
    if (entriesRemaining == 0) {
      return false;
    }
  }
  return true;
}

/* Returns false when the local selection consists only of class-0x16 entities of faction ownerIndex
   and at least one of them has a non-zero classLinkState.classState70 in its model runtime; true otherwise
   (also for an empty selection).
*/
Bool8 SelectionInfo_TestNotOwnAircraftPadsWithAircraft(FactionRuntimeIndex ownerIndex)

{
  ModelRuntimeSlot *classRecord;
  int entryIndex;
  int activeEntryCount;
  GameEntityRuntime *currentEntry;

  activeEntryCount = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    currentEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (currentEntry == nullptr) {
      continue;
    }
    classRecord = (ModelRuntimeSlot *)(currentEntry->common).ownership.definitionOrClassRecord; /* the model runtime */
    if (ownerIndex != (currentEntry->common).ownership.ownerIndex) {
      return true;
    }
    if (classRecord->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_22) {
      return true;
    }
    if (classRecord->classLinkState.classState70 != 0) {
      activeEntryCount++;
    }
  }
  return activeEntryCount == 0;
}

/* Returns false when the local selection can take a ground position order: some entity's
   definition has a non-zero accelerationPerTick, or the selection is a single entity of definition class 0x0D (13).
   True otherwise; the world input then ignores the ground click.
*/
Bool8 SelectionInfo_TestAnyActiveOrSingleClass13()

{
  int entryIndex;
  int selectedEntryCount;
  GameEntityRuntime *selectedEntry;
  Bool8 selectedEntryIsClass13;
  ModelDefinition *selectedDefinition;

  selectedEntryCount = 0;
  selectedEntryIsClass13 = false;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntry == nullptr) {
      continue;
    }
    selectedEntryCount++;
    selectedDefinition = THANDOR_PTR32_AT(ModelDefinition, (selectedEntry->common).ownership.definitionOrClassRecord);
    if (selectedDefinition->accelerationPerTick != 0) {
      return false;
    }
    selectedEntryIsClass13 = selectedDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13;
  }
  if ((selectedEntryCount == 1) && (selectedEntryIsClass13)) {
    return false;
  }
  return true;
}

/* Fallback of SelectionInfo_TestPositionCommandAtWorldPoint: the first class-0x0D entity of the local selection
   tests the grid cell mask bands selected by its capability flags (0x80 -> band 3, 4 -> band 1, else 6);
   true when there is no such entity. */
static Bool8 SelectionInfo_TestClass13CellBandsAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12)

{
  GameEntityRuntime *selectedEntity;
  ModelDefinition *class13Definition;
  uint32_t capabilityFlags;
  uint8_t highBandIndex;
  int entryIndex;

  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntity = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntity == nullptr) {
      continue;
    }
    class13Definition = THANDOR_PTR32_AT(ModelDefinition, (selectedEntity->common).ownership.definitionOrClassRecord);
    if (class13Definition->runtimeClassId != MODEL_RUNTIME_CLASS_13) {
      continue;
    }
    capabilityFlags = class13Definition->classParameterC4;
    if ((capabilityFlags & 0x80) != 0) {
      highBandIndex = 3;
    }
    else if ((capabilityFlags & 4) != 0) {
      highBandIndex = 1;
    }
    else {
      highBandIndex = 6;
    }
    return GridScratch_TestProjectedCellMaskBands(worldXQ12,worldYQ12,7,highBandIndex);
  }
  return true;
}

/* Tests whether the local selection could be ordered to a world point (returns the result of the test). The
   first entity whose definition has a non-zero accelerationPerTick is temporarily moved to the point and asked
   through its typed callback; without such an entity the first class-0x0D entity tests the grid cell mask bands
   selected by its capability flags (0x80 -> band 3, 4 -> band 1, else 6). Returns true when neither exists.
*/
Bool8 SelectionInfo_TestPositionCommandAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime)

{
  GraphicsWorldCoordinateQ12 savedTranslationX;
  GraphicsWorldCoordinateQ12 savedTranslationY;
  ModelRuntimeNode *selectedModelNode;
  int entryIndex;
  Bool8 testResult;
  GameEntityRuntime *selectedEntity;

  selectedEntity = nullptr;
  selectedModelNode = nullptr;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntity = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntity == nullptr) {
      continue;
    }
    selectedModelNode = (selectedEntity->common).ownership.modelNode;
    if (((ModelRuntimeSlot *)(selectedEntity->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
        runtimeDefinition->accelerationPerTick != 0) {
      break;
    }
  }
  if (entryIndex == SELECTION_ENTRY_CAPACITY) {
    return SelectionInfo_TestClass13CellBandsAtWorldPoint(worldXQ12,worldYQ12);
  }
  /* swap the point in; note that translation.x receives worldYQ12 and
     translation.y worldXQ12 */
  savedTranslationX = (selectedModelNode->worldTransform).translation.x;
  (selectedModelNode->worldTransform).translation.x = worldYQ12;
  savedTranslationY = (selectedModelNode->worldTransform).translation.y;
  (selectedModelNode->worldTransform).translation.y = worldXQ12;
  testResult = ArmyRuntimeNode_DispatchTypedCallback((Ptr32<ArmyRuntimeSlot> *)selectedEntity,inGameRuntime);
  (selectedModelNode->worldTransform).translation.x = savedTranslationX;
  (selectedModelNode->worldTransform).translation.y = savedTranslationY;
  return testResult;
}

/* Returns false as soon as one entity of the local selection passes
   ArmyRuntime_TestWeaponDamageNonnegative but fails ArmyRuntime_TestHasNoWeaponDamage (its state value
   stateOrTechnologyId is positive); true when none does.
*/
Bool8 SelectionInfo_TestNoEntryHasWeaponDamage()

{
  GameEntityRuntime *armyRuntime;
  int entriesRemaining;
  Ptr32<GameEntityRuntime> *selectionEntryCursor;
  Bool8 stateTestResult;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    armyRuntime = *selectionEntryCursor;
    if (armyRuntime != nullptr) {
      stateTestResult = ArmyRuntime_TestWeaponDamageNonnegative((ArmyRuntimeSlot *)armyRuntime);
      if (stateTestResult) {
        stateTestResult = ArmyRuntime_TestHasNoWeaponDamage((ArmyRuntimeSlot *)armyRuntime);
        if (!stateTestResult) {
          return false;
        }
      }
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return true;
}

/* Returns true when ArmyRuntime_TestWeaponDamageNonnegative holds for any entity of the local
   selection, false otherwise.
*/
Bool8 SelectionInfo_TestAnyEntryWeaponDamageNonnegative()

{
  int entriesRemaining;
  Ptr32<GameEntityRuntime> *selectionEntryCursor;
  Bool8 stateTestResult;

  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != nullptr) {
      stateTestResult = ArmyRuntime_TestWeaponDamageNonnegative((ArmyRuntimeSlot *)*selectionEntryCursor);
      if (stateTestResult) {
        return true;
      }
    }
    selectionEntryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return false;
}

/* Returns the first entity of the local player's selection (the first non-NULL entry), or NULL when nothing is
   selected; the in-game panels use it as the representative of the selection.
*/
GameEntityRuntime * SelectionInfo_GetFirstEntry()

{
  GameEntityRuntime *firstEntry;
  int entriesRemaining;
  Ptr32<GameEntityRuntime> *selectionEntryCursor;
  Ptr32<GameEntityRuntime> *nextSelectionEntryCursor;
  Bool8 currentEntryIsEmpty;

  /* skip the empty entries */
  entriesRemaining = SELECTION_ENTRY_CAPACITY;
  firstEntry = nullptr;
  currentEntryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    nextSelectionEntryCursor = selectionEntryCursor;
    if (entriesRemaining == 0) break;
    entriesRemaining--;
    nextSelectionEntryCursor = selectionEntryCursor + 1;
    currentEntryIsEmpty = *selectionEntryCursor == nullptr;
    selectionEntryCursor = nextSelectionEntryCursor;
  } while (currentEntryIsEmpty);
  if (!currentEntryIsEmpty) {
    firstEntry = nextSelectionEntryCursor[-1];
  }
  return firstEntry;
}

/* Tests whether entry is missing from the local selection: true when absent, false when it is selected.
*/
Bool8 SelectionInfo_IsEntryAbsent(GameEntityRuntime *entry)

{
  /* search the 32 selection slots */
  int slotIndex;

  for (slotIndex = 0; slotIndex < SELECTION_ENTRY_CAPACITY; slotIndex++) {
    if (g_SelectionInfoEntitySlots->entries[slotIndex] == entry) {
      return false;
    }
  }
  return true;
}

/* Returns the OR of the attachment effect variant masks of all entities in the local selection (per entity from
   ArmyRuntime_GetAttachmentEffectVariantMask).
*/
uint32_t SelectionInfo_CollectAttachmentEffectVariantMask()

{
  uint32_t effectVariantMask;
  int entryIndex;
  GameEntityRuntime *selectedEntry;

  effectVariantMask = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntry != nullptr) {
      effectVariantMask |=
           ArmyRuntime_GetAttachmentEffectVariantMask
              ((ModelRuntimeLinkedChildSpawnAndBuildView *)(selectedEntry->common).ownership.definitionOrClassRecord);
    }
  }
  return effectVariantMask;
}

/* Returns the OR of the capability flags of the local selection: definition class 0x16 contributes 8, class
   0x0D the capability dword classParameterC4 of its definition; other classes contribute nothing.
*/
uint32_t SelectionInfo_CollectCapabilityFlags()

{
  uint32_t capabilityMask;
  int entryIndex;
  GameEntityRuntime *selectedEntry;
  ModelDefinition *entityDefinition;

  capabilityMask = 0;
  for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
    selectedEntry = g_SelectionInfoEntitySlots->entries[entryIndex];
    if (selectedEntry == nullptr) {
      continue;
    }
    entityDefinition = THANDOR_PTR32_AT(ModelDefinition, (selectedEntry->common).ownership.definitionOrClassRecord);
    if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
      capabilityMask = capabilityMask | 8;
    }
    else if (entityDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
      capabilityMask = capabilityMask | entityDefinition->classParameterC4;
    }
  }
  return capabilityMask;
}
