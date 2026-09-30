/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/statehash.c
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/statehash.h>

#ifdef THANDOR_TEST_AIDS

/* FNV-1a over 32-bit words; pointers never go in (addresses differ between builds), only pool indices. */
typedef struct StateHash {
  uint64_t value;
} StateHash;

static void StateHash_Add(StateHash *hash, uint32_t word)
{
  int byteIndex;
  for (byteIndex = 0; byteIndex < 4; byteIndex++) {
    hash->value ^= (word >> (byteIndex * 8)) & 0xffu;
    hash->value *= 0x100000001b3ull;
  }
}

static int s_stepsWanted;       /* OPEN_THANDOR_STATEHASH: steps to record, 0 = off */
static int s_stepsDone;
static unsigned s_detailTick;   /* OPEN_THANDOR_STATEHASH_DETAIL: tick whose per-object values are written too */
static FILE *s_output;

static int32_t DebugStateHash_ArmyIndex(const void *army)
{
  if (army == NULL) {
    return -1;
  }
  return (int32_t)(((const uint8_t *)army - (const uint8_t *)g_ArmyRuntimeSlots) / (int)sizeof(ArmyRuntimeSlot));
}

void DebugStateHash_SessionStart(void)
{
  const char *steps = getenv("OPEN_THANDOR_STATEHASH");
  const char *seed = getenv("OPEN_THANDOR_STATEHASH_SEED");
  const char *detail = getenv("OPEN_THANDOR_STATEHASH_DETAIL");
  if (steps == NULL || atoi(steps) <= 0) {
    return;
  }
  s_stepsWanted = atoi(steps);
  s_stepsDone = 0;
  s_detailTick = detail != NULL ? (unsigned)atoi(detail) : 0;
  /* The simulation draws from g_RandomGeneratorState.next; in a local game that is the primary stream, which the
     ambient sound and music code (InGameUiRoot_UpdateFrame) also advances once per rendered frame, so the run would
     depend on the frame rate. A network game seeds both streams and moves the simulation to the secondary one;
     do the same with a fixed seed. */
  Random_SetBothSeeds((RandomSeed)(seed != NULL ? strtoul(seed, NULL, 0) : 12345u));
  Random_SelectSecondaryStream();
  s_output = fopen("statehash.txt", "w");
  Thandor_Log("test aid: state hash for %d simulation steps, seed %s -> statehash.txt", s_stepsWanted,
              seed != NULL ? seed : "12345");
}

void DebugStateHash_AfterStep(void)
{
  StateHash hash = {0xcbf29ce484222325ull};
  WorldOwnerListNode *node;
  unsigned tick = g_GameFactionRuntimeImage.tail.simulationTick;
  unsigned armies = 0;
  unsigned nodes = 0;
  unsigned slotIndex;
  int faction;
  int detail;
  if (s_stepsWanted == 0 || s_output == NULL || g_InGameRuntimeRoot == NULL) {
    return;
  }
  detail = (tick == s_detailTick);
  StateHash_Add(&hash, tick);
  StateHash_Add(&hash, g_RandomGeneratorState.secondarySeed);
  /* every world object in update order: class and world position (models, shots and effects) */
  for (node = g_InGameRuntimeRoot->worldRuntime.ownerListHead; node != NULL; node = node->nextNode) {
    StateHash_Add(&hash, (uint32_t)node->ownerClassId);
    StateHash_Add(&hash, (uint32_t)node->worldXQ12);
    StateHash_Add(&hash, (uint32_t)node->worldYQ12);
    StateHash_Add(&hash, (uint32_t)node->worldZQ12);
    nodes++;
  }
  /* armies by slot: identity, orders, targets (as slot indices), health of the root model */
  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *army = &g_ArmyRuntimeSlots[slotIndex];
    ModelRuntimeSlot *model;
    uint32_t health = 0;
    if (army->modelNodeRuntime == NULL) {
      continue;
    }
    model = army->modelRuntimeOrSavedOffset.modelRuntime;
    if (model != NULL) {
      health = model->health;
    }
    StateHash_Add(&hash, slotIndex);
    StateHash_Add(&hash, (uint32_t)army->armyAssetId);
    StateHash_Add(&hash, (uint32_t)army->factionIndex);
    StateHash_Add(&hash, (uint32_t)army->movementStateFlags);
    StateHash_Add(&hash, (uint32_t)army->commandModeFlags);
    StateHash_Add(&hash, (uint32_t)army->commandGeneration);
    StateHash_Add(&hash, army->aiUnitState);
    StateHash_Add(&hash, (uint32_t)DebugStateHash_ArmyIndex(army->commandTargetArmyRuntime));
    StateHash_Add(&hash, (uint32_t)DebugStateHash_ArmyIndex((const void *)(uintptr_t)army->assignedTargetArmyRuntime));
    StateHash_Add(&hash, health);
    armies++;
    if (detail) {
      fprintf(s_output, "  army %u asset %u faction %u flags %08x mode %08x gen %u ai %u target %d assigned %d "
              "health %u pos %d %d %d\n", slotIndex, (unsigned)army->armyAssetId, (unsigned)army->factionIndex,
              (unsigned)army->movementStateFlags, (unsigned)army->commandModeFlags,
              (unsigned)army->commandGeneration, (unsigned)army->aiUnitState,
              DebugStateHash_ArmyIndex(army->commandTargetArmyRuntime),
              DebugStateHash_ArmyIndex((const void *)(uintptr_t)army->assignedTargetArmyRuntime), (unsigned)health,
              army->modelNodeRuntime->worldTransform.translation.x,
              army->modelNodeRuntime->worldTransform.translation.y,
              army->modelNodeRuntime->worldTransform.translation.z);
    }
  }
  /* faction resources */
  for (faction = 0; faction < 8; faction++) {
    GameFactionRuntimeRecord *record = &g_GameFactionRuntimeImage.records[faction];
    StateHash_Add(&hash, (uint32_t)record->xeniteCurrentQ4);
    StateHash_Add(&hash, (uint32_t)record->tritiumCurrentQ4);
    StateHash_Add(&hash, (uint32_t)record->suppliedEnergyDemandQ4);
  }
  fprintf(s_output, "%u %08x%08x armies %u objects %u seed %08x\n", tick, (unsigned)(hash.value >> 32),
          (unsigned)hash.value, armies, nodes, (unsigned)g_RandomGeneratorState.secondarySeed);
  s_stepsDone++;
  if (s_stepsDone >= s_stepsWanted) {
    fclose(s_output);
    s_output = NULL;
    Thandor_Log("test aid: state hash done after %d steps (tick %u)", s_stepsDone, tick);
    ExitProcess(0);
  }
}

#endif
