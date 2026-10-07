/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level_script.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level_script.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* L"flm\\ende0000.flm" */
uint16_t g_SessionEndMoviePathUtf16[17] = {'f', 'l', 'm', '\\', 'e', 'n', 'd', 'e', '0', '0', '0', '0', '.', 'f', 'l', 'm', 0};

/* L"flm\\ende0001.flm" */
uint16_t g_FlmEnde0001FlmPathUtf16[17] = {'f', 'l', 'm', '\\', 'e', 'n', 'd', 'e', '0', '0', '0', '1', '.', 'f', 'l', 'm', 0};

/* Evaluates one scheduled condition of the level script (its satisfied bit is already cleared in the record).
   COUNTDOWN_ELAPSED also counts its operand 1 down by the step ticks and clamps it at 0 once elapsed.
   Unknown kinds (and unused records) never hold. */
static Bool8 InGameScheduledCondition_Holds(InGameLevelConditionStorage *levelConditionStorage,
                                           InGameScheduledConditionRecord10 *condition,
                                           InGameScheduledConditionKind kind)
{
  uint32_t *operands;
  WorldOwnerListNode *worldNode;
  ArmyRuntimeSlot *army;
  uint32_t firstFactionIndex;
  uint32_t secondFactionIndex;
  uint32_t armiesStillNeeded;
  uint32_t remainingTicks;
  uint32_t runtimeClassId;
  FieldGridAsset *fieldGrid;
  uint32_t cellCount;
  uint32_t cellsLeft;
  uint32_t occupiedCellCount;
  uint8_t *cellBytes;
  ResourceExtractionDescriptor32 *cellOccupancyMask;

  operands = condition->payload.operands;
  switch(kind & INGAME_SCHEDULED_CONDITION_KIND_MASK) {
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != nullptr; worldNode = worldNode->nextNode) {
      if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (operands[0] ==
          WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex)) {
        return false;
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != nullptr; worldNode = worldNode->nextNode) {
      if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
           [WorldOwnerNode_ModelRuntime(worldNode)->definitionOrSavedId.runtimeDefinition->runtimeClassId] ==
           ArmyRuntime_ClassCommandHandlerGroupA)) &&
         (WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
          operands[0])) {
        return false;
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != nullptr; worldNode = worldNode->nextNode) {
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        army = WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        if ((operands[0] == army->factionIndex) && (army->armyAssetId == operands[2])) {
          return false;
        }
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
    firstFactionIndex = operands[1];
    secondFactionIndex = operands[0];
    return (g_GameFactionRuntimeImage.tail.factionLifecycleStates[firstFactionIndex] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
           (g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
           (FACTION_RELATION_STATE_ALLIED - 1 < (g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates >>
                 ((char)secondFactionIndex * 4 & 31U) & 0xf));
  case INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST:
    return (int)operands[1] <= (int)g_GameFactionRuntimeImage.records[operands[0]].xeniteCurrentQ4;
  case INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST:
    return (int)operands[1] <= (int)g_GameFactionRuntimeImage.records[operands[0]].tritiumCurrentQ4;
  case INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST:
    return (int)operands[1] <= (int)g_GameFactionRuntimeImage.records[operands[0]].tritiumExtractionRateQ4PerTick;
  case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
    armiesStillNeeded = operands[1];
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != nullptr; worldNode = worldNode->nextNode) {
      if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
          operands[0]) &&
         (WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime->armyAssetId ==
          operands[2])) {
        armiesStillNeeded--;
        if (armiesStillNeeded == 0) {
          return true;
        }
      }
    }
    return false;
  case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
    /* operand 0 is a byte offset into the cell records; operand 1 the percentage */
    fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
    cellCount = fieldGrid->gridWidth * fieldGrid->gridHeight;
    cellBytes = reinterpret_cast<uint8_t *>(fieldGrid->cells) + operands[0];
    occupiedCellCount = 0;
    cellsLeft = cellCount;
    /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
    do {
      /* the low dword of the 64-bit occupancy mask, operand 0 bytes further */
      cellOccupancyMask =
           reinterpret_cast<ResourceExtractionDescriptor32 *>(cellBytes + offsetof(FieldGridCell, occupancyMask));
      cellBytes = cellBytes + sizeof(FieldGridCell);
      occupiedCellCount = occupiedCellCount + ((*cellOccupancyMask & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0);
      cellsLeft--;
    } while (cellsLeft != 0);
    return (int)operands[1] <= (int)(((uint64_t)occupiedCellCount * 100) / (uint64_t)cellCount);
  case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
    remainingTicks = operands[1] - g_InGameSimulationStepTicks;
    operands[1] = remainingTicks;
    if ((int)remainingTicks < 1) {
      operands[1] = 0;
      return true;
    }
    return false;
  case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
    return (int)g_GameFactionRuntimeImage.records[operands[0]].xeniteStorageLimitQ4 < (250 << Q4_SHIFT) + 1;
  case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
    for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        worldNode != nullptr; worldNode = worldNode->nextNode) {
      if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        runtimeClassId =
             WorldOwnerNode_ModelRuntime(worldNode)->definitionOrSavedId.runtimeDefinition->runtimeClassId;
        if ((g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[runtimeClassId] !=
             ArmyRuntime_ClassCommandHandlerGroupA) && (runtimeClassId == operands[0])) {
          return false;
        }
      }
    }
    return true;
  case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
    return (InGameScheduledCondition_EvaluatePostfixExpression<uint32_t>
              (levelConditionStorage,&condition->statusAndKind.kindAndExpression[1]) & 1) != 0;
  default:
    return false;
  }
}

/* True while two active factions (1..7) are still not allied (relation state below 8): the game goes on. */
static Bool8 InGameConditionRuntime_HasUnalliedActiveFactionPair()
{
  uint32_t factionIndex;
  uint32_t otherFactionIndex;

  for (factionIndex = 1; factionIndex < 7; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      continue;
    }
    for (otherFactionIndex = factionIndex + 1; otherFactionIndex < 8; otherFactionIndex++) {
      if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[otherFactionIndex] ==
           FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
         ((g_GameFactionRuntimeImage.records[otherFactionIndex].packedRelationStates >> (factionIndex * 4 & 31) &
           0xf) < FACTION_RELATION_STATE_ALLIED)) {
        return true;
      }
    }
  }
  return false;
}

/* Chooses the end movie from the local faction's view and requests it: the trigger's variant when the ended
   faction is the local one or one it rates above 3, the other variant when the local faction has not ended and
   rates it 3 or below, variant 0 when the local faction has ended too (or is unused). */
static void InGameConditionRuntime_RequestEndMovie(const InGameEndConditionTriggerRecord8 *endTrigger,
                                                   const WorldRuntimeContext *worldRuntime,
                                                   uint32_t endedFactionIndex)
{
  uint32_t localFactionIndex;

  localFactionIndex = worldRuntime->activeFactionRuntimeIndex;
  g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
  if (localFactionIndex != endedFactionIndex) {
    g_EndMovieVariantIndex = 0;
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[localFactionIndex] <
         FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED) &&
       (g_GameFactionRuntimeImage.tail.factionLifecycleStates[localFactionIndex] !=
        FACTION_RUNTIME_LIFECYCLE_INACTIVE)) {
      g_EndMovieVariantIndex = endTrigger->movieVariantSelector ^ 1;
      if (FACTION_RELATION_STATE_FRIENDLY - 1 <
          (g_GameFactionRuntimeImage.records[localFactionIndex].packedRelationStates >>
               ((char)endedFactionIndex * 4 & 31U) & 0xf)) {
        g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
      }
    }
  }
  g_EndMovieSelectionIndex = (uint32_t)endTrigger->endMovieSelectionIndex;
  g_EndMoviePath = g_SessionEndMoviePathUtf16;
  if (g_EndMovieVariantIndex == 0) {
    g_EndMoviePath = g_FlmEnde0001FlmPathUtf16;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
}

/* Ends the (active) faction of a fired end trigger: marks it ENDING_PENDING and, unless skipArmyDisableWhenOne
   == 1, destroys its units, takes map input from the local player when it is theirs and stops while two active
   factions are still not allied (then the local player's build/stock/diplomacy panels are closed when the ended
   faction is theirs). Otherwise the end movie is requested. */
static void InGameConditionRuntime_EndTriggerFaction(const InGameEndConditionTriggerRecord8 *endTrigger)
{
  InGameRuntimeRoot *triggerRoot;
  InGameRuntimeRoot *relationRoot;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;
  ArmyRuntimeSlot *army;
  uint32_t endedFactionIndex;

  triggerRoot = g_InGameRuntimeRoot;
  endedFactionIndex = (uint32_t)endTrigger->factionRuntimeIndex;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  g_GameFactionRuntimeImage.tail.factionLifecycleStates[endedFactionIndex] = FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING;
  if (endTrigger->skipArmyDisableWhenOne != 1) {
    worldNode = (triggerRoot->worldRuntime).ownerListHead;
    if (worldNode != nullptr) {
      for (; worldNode != nullptr; worldNode = worldNode->nextNode) {
        if (worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
          army = WorldOwnerNode_ModelRuntime(worldNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
          if (endedFactionIndex == army->factionIndex) {
            ModelRuntimeHierarchy_MarkDestroyedRecursive(worldRuntime,army);
          }
        }
      }
      g_GameFactionRuntimeImage.records[endedFactionIndex].secondaryArmyAssetCount = 0;
      g_GameFactionRuntimeImage.records[endedFactionIndex].primaryArmyAssetCount = 0;
    }
    relationRoot = g_InGameRuntimeRoot;
    if (endedFactionIndex == (triggerRoot->worldRuntime).activeFactionRuntimeIndex) {
      g_UiCommandRuntimeFlags =
           g_UiCommandRuntimeFlags |
           (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED);
    }
    if (InGameConditionRuntime_HasUnalliedActiveFactionPair()) {
      if ((uint32_t)endTrigger->factionRuntimeIndex == (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex) {
        g_InGameRuntimeRoot->diplomacyPanelNodeFlags = g_InGameRuntimeRoot->diplomacyPanelNodeFlags | 8;
        InGameUiImage *relationUi = InGameUi_Image(relationRoot);
        relationUi->buildCatalogPanel.selectable.base.nodeFlags =
             relationUi->buildCatalogPanel.selectable.base.nodeFlags | 8;
        relationUi->specialBuildCatalogPanel.selectable.base.nodeFlags =
             relationUi->specialBuildCatalogPanel.selectable.base.nodeFlags | 8;
        relationUi->armyStockPanel.selectable.base.nodeFlags = relationUi->armyStockPanel.selectable.base.nodeFlags | 8;
      }
      return;
    }
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  }
  InGameConditionRuntime_RequestEndMovie(endTrigger,worldRuntime,endedFactionIndex);
}

/* The level script, evaluated every 20 simulation steps: first promotes factions that were marked as ending to
   ended, then re-evaluates the level's 64 scheduled conditions (bit 0 of each record's kind = satisfied: unit
   counts, resource amounts, map share, countdowns, boolean expressions over other conditions), and finally checks
   the 16 end triggers. The first active trigger whose condition holds ends its faction: its units are disabled,
   the local player loses map input when it is theirs, and unless two remaining active factions are still not
   allied (relation state below 8) the end movie is chosen and UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING ends the
   session. A trigger with skipArmyDisableWhenOne == 1 goes to the end movie directly.
   Expression tokens: 0xFC end, 0xFD NOT, 0xFE AND, 0xFF OR, anything else pushes that condition's result bit
   (see InGameScheduledCondition_EvaluatePostfixExpression).
*/
void InGameConditionRuntime_UpdateScheduledRecords()

{
  InGameLevelConditionStorage *levelConditionStorage;
  uint32_t factionIndex;
  int conditionIndex;
  int triggerIndex;
  InGameScheduledConditionRecord10 *condition;
  InGameScheduledConditionKind kind;
  InGameEndConditionTriggerRecord8 *endTrigger;

  DebugHook_LevelScriptBeforeEvaluation();
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING) {
      g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] =
           FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED;
    }
  }
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  condition = (levelConditionStorage->schedule).conditions;
  for (conditionIndex = 0; conditionIndex < INGAME_SCHEDULED_CONDITION_COUNT; conditionIndex++) {
    /* clear the satisfied bit, then set it again when the condition holds */
    kind = condition->statusAndKind.kind;
    condition->statusAndKind.raw = condition->statusAndKind.raw & ~(uint32_t)INGAME_SCHEDULED_CONDITION_SATISFIED;
    if (InGameScheduledCondition_Holds(levelConditionStorage,condition,kind)) {
      condition->statusAndKind.raw = condition->statusAndKind.raw | INGAME_SCHEDULED_CONDITION_SATISFIED;
    }
    condition++;
  }
  DebugHook_LevelScriptAfterEvaluation(levelConditionStorage);
  /* the same 8-byte trigger records, viewed through the type the debug hook takes */
  endTrigger = reinterpret_cast<InGameEndConditionTriggerRecord8 *>((levelConditionStorage->schedule).triggers);
  for (triggerIndex = 0; triggerIndex < INGAME_END_CONDITION_TRIGGER_COUNT; triggerIndex++, endTrigger++) {
    if ((endTrigger->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
       (((levelConditionStorage->schedule).conditions[endTrigger->conditionIndex].statusAndKind.raw &
         INGAME_SCHEDULED_CONDITION_SATISFIED) != 0)) {
      endTrigger->stateFlags = endTrigger->stateFlags | INGAME_END_CONDITION_TRIGGER_PROCESSED;
      DebugHook_LevelScriptEndTrigger(levelConditionStorage,triggerIndex,endTrigger);
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[endTrigger->factionRuntimeIndex] ==
          FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
        InGameConditionRuntime_EndTriggerFaction(endTrigger);
        return;
      }
    }
  }
}
