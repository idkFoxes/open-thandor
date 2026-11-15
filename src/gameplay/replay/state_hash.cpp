/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/replay/state_hash.cpp
 * Project code (not in the original game)
 */

#include <string.h>
#include <thandor/gameplay/replay/state_hash.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/world/pathing/mode.h>

/* Factored out of platform/debug/statehash.cpp (DebugStateHash_AfterStep) unchanged, so the determinism references
   keep their values. */

static int32_t GameStateHash_ArmyIndex(const void *army)
{
  if (army == nullptr) {
    return -1;
  }
  return (int32_t)(Asset_ByteDistance(army, g_ArmyRuntimeSlots) / (int)sizeof(ArmyRuntimeSlot));
}

uint64_t GameStateHash_Compute(GameStateHashCounts *counts, FILE *detail)
{
  uint64_t hash = GAME_STATE_HASH_SEED;
  WorldOwnerListNode *node;
  unsigned armies = 0;
  unsigned nodes = 0;
  unsigned slotIndex;
  int faction;

  hash = GameStateHash_AddWord(hash, g_GameFactionRuntimeImage.tail.simulationTick);
  hash = GameStateHash_AddWord(hash, g_RandomGeneratorState.secondarySeed);
  /* every world object in update order: class and world position (models, shots and effects) */
  for (node = g_InGameRuntimeRoot->worldRuntime.ownerListHead; node != nullptr; node = node->nextNode) {
    hash = GameStateHash_AddWord(hash, (uint32_t)node->ownerClassId);
    hash = GameStateHash_AddWord(hash, (uint32_t)node->worldXQ12);
    hash = GameStateHash_AddWord(hash, (uint32_t)node->worldYQ12);
    hash = GameStateHash_AddWord(hash, (uint32_t)node->worldZQ12);
    if (detail != nullptr) {
      fprintf(detail, "  object %u class %u pos %d %d %d\n", nodes, (unsigned)node->ownerClassId,
              (int)node->worldXQ12, (int)node->worldYQ12, (int)node->worldZQ12);
    }
    nodes++;
  }
  /* armies by slot: identity, orders, targets (as slot indices), health of the root model */
  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *army = &g_ArmyRuntimeSlots[slotIndex];
    ModelRuntimeSlot *model;
    uint32_t health = 0;
    if (army->modelNodeRuntime == nullptr) {
      continue;
    }
    model = army->modelRuntimeOrSavedOffset.modelRuntime;
    if (model != nullptr) {
      health = model->health;
    }
    const int32_t targetIndex = GameStateHash_ArmyIndex(army->commandTargetArmyRuntime);
    const int32_t assignedIndex = GameStateHash_ArmyIndex(
        reinterpret_cast<const void *>(static_cast<uintptr_t>(army->assignedTargetArmyRuntime)));
    hash = GameStateHash_AddWord(hash, slotIndex);
    hash = GameStateHash_AddWord(hash, (uint32_t)army->armyAssetId);
    hash = GameStateHash_AddWord(hash, (uint32_t)army->factionIndex);
    hash = GameStateHash_AddWord(hash, (uint32_t)army->movementStateFlags);
    hash = GameStateHash_AddWord(hash, (uint32_t)army->commandModeFlags);
    hash = GameStateHash_AddWord(hash, (uint32_t)army->commandGeneration);
    hash = GameStateHash_AddWord(hash, army->aiUnitState);
    hash = GameStateHash_AddWord(hash, (uint32_t)targetIndex);
    hash = GameStateHash_AddWord(hash, (uint32_t)assignedIndex);
    hash = GameStateHash_AddWord(hash, health);
    armies++;
    if (detail != nullptr) {
      fprintf(detail, "  army %u asset %u faction %u flags %08x mode %08x gen %u ai %u target %d assigned %d "
              "health %u pos %d %d %d\n", slotIndex, (unsigned)army->armyAssetId, (unsigned)army->factionIndex,
              (unsigned)army->movementStateFlags, (unsigned)army->commandModeFlags,
              (unsigned)army->commandGeneration, (unsigned)army->aiUnitState, targetIndex, assignedIndex,
              (unsigned)health, army->modelNodeRuntime->worldTransform.translation.x,
              army->modelNodeRuntime->worldTransform.translation.y,
              army->modelNodeRuntime->worldTransform.translation.z);
    }
  }
  /* faction resources */
  for (faction = 0; faction < 8; faction++) {
    GameFactionRuntimeRecord *record = &g_GameFactionRuntimeImage.records[faction];
    hash = GameStateHash_AddWord(hash, (uint32_t)record->xeniteCurrentQ4);
    hash = GameStateHash_AddWord(hash, (uint32_t)record->tritiumCurrentQ4);
    hash = GameStateHash_AddWord(hash, (uint32_t)record->suppliedEnergyDemandQ4);
  }
  /* open-thandor: the improved pathing mode and its per-unit side table (every slot as raw dwords; free slots
     are zero). Only with the improved mode, so the hashes of the original mode stay as they were. */
  if (Pathing_Improved()) {
    static uint32_t s_pathingWords[ARMY_RUNTIME_SLOT_COUNT * sizeof(PathingUnitState) / sizeof(uint32_t)];
    static_assert(sizeof(s_pathingWords) == sizeof(g_PathingUnitState));
    hash = GameStateHash_AddWord(hash, (uint32_t)g_PathingMode);
    memcpy(s_pathingWords, g_PathingUnitState, sizeof(s_pathingWords));
    for (uint32_t pathingWord : s_pathingWords) {
      hash = GameStateHash_AddWord(hash, pathingWord);
    }
  }
  if (counts != nullptr) {
    counts->armies = armies;
    counts->objects = nodes;
  }
  return hash;
}
