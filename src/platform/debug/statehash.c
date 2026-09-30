/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/statehash.c
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* Scenario orders (OPEN_THANDOR_ARENA_ORDERS), issued from the step hook at fixed step numbers so they happen at
   the same simulation tick in every run; they run in the next step, as the game's own commands do. */
#define ARENA_MOVE_OUT_STEP 100
#define ARENA_MOVE_BACK_STEP 500
#define ARENA_MOVE_ROWS 20
#define ARENA_PRODUCTION_STEP 20
#define ARENA_ROW_STEP_X 0x480   /* world X per grid row (isometric lattice, see tools/data/fld.py) */
#define ARENA_ROW_STEP_Y (-1999) /* world Y per grid row */

static Q12 s_armyHomeX[ARMY_RUNTIME_SLOT_COUNT];
static Q12 s_armyHomeY[ARMY_RUNTIME_SLOT_COUNT];

/* "move": every army of faction 1 drives ARENA_MOVE_ROWS rows towards the other side (down on the screen), every
   army of faction 2 the same distance the other way (up), then all drive back to where they stood. Immobile
   armies ignore the order (ArmyRuntime_StartRoutedMoveCommand). */
static void DebugArena_MoveOrders(int step)
{
  unsigned slotIndex;
  if (step != ARENA_MOVE_OUT_STEP && step != ARENA_MOVE_BACK_STEP) {
    return;
  }
  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *army = &g_ArmyRuntimeSlots[slotIndex];
    int direction;
    if (army->modelNodeRuntime == NULL || (army->factionIndex != 1 && army->factionIndex != 2)) {
      continue;
    }
    direction = army->factionIndex == 1 ? 1 : -1;
    if (step == ARENA_MOVE_OUT_STEP) {
      Q12 x = army->modelNodeRuntime->worldTransform.translation.x;
      Q12 y = army->modelNodeRuntime->worldTransform.translation.y;
      s_armyHomeX[slotIndex] = x;
      s_armyHomeY[slotIndex] = y;
      /* the engine takes Y first: translation.y, then translation.x */
      ArmyRuntime_StartRoutedMoveCommand(y + direction * ARENA_MOVE_ROWS * ARENA_ROW_STEP_Y,
                                         x + direction * ARENA_MOVE_ROWS * ARENA_ROW_STEP_X,
                                         (ArmyMovementRuntime *)army);
    }
    else if (s_armyHomeX[slotIndex] != 0 || s_armyHomeY[slotIndex] != 0) {
      ArmyRuntime_StartRoutedMoveCommand(s_armyHomeY[slotIndex], s_armyHomeX[slotIndex], (ArmyMovementRuntime *)army);
    }
  }
}

/* "production": both factions queue units for every factory (the build menu's call, which checks nothing: the
   factories take what they can afford) and every lab starts the first technology of its range that is available. */
static void DebugArena_ProductionOrders(int step)
{
  static const uint32_t queued[] = {1, 10, 30, 60, 80, 100, 110, 130, 150, 170, 180, 200, 250, 260, 280};
  static const struct { uint32_t assetId; int firstTech; int lastTech; } labs[] = {
    {320, 180, 194}, {321, 91, 110}, {322, 120, 140}, {323, 150, 170}};
  unsigned slotIndex;
  unsigned index;
  int faction;
  if (step != ARENA_PRODUCTION_STEP) {
    return;
  }
  for (faction = 1; faction <= 2; faction++) {
    for (index = 0; index < sizeof queued / sizeof queued[0]; index++) {
      GameFactionRuntime_RegisterArmyAssetPointers(0, 2, (PckArmyAssetIdCatalog)queued[index],
                                                   (FactionRuntimeIndex)faction);
    }
  }
  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *army = &g_ArmyRuntimeSlots[slotIndex];
    if (army->modelNodeRuntime == NULL || army->modelRuntimeOrSavedOffset.modelRuntime == NULL) {
      continue;
    }
    for (index = 0; index < sizeof labs / sizeof labs[0]; index++) {
      int tech;
      if ((uint32_t)army->armyAssetId != labs[index].assetId) {
        continue;
      }
      for (tech = labs[index].firstTech; tech <= labs[index].lastTech; tech++) {
        /* true = available (locked, prerequisites met, not researched elsewhere) */
        if (Technology_IsAvailableForFaction((PckTechnologyIdCatalog)tech, army->factionIndex)) {
          Technology_ApplyRecordToEntity((PckTechnologyIdCatalog)tech,
                                         (GameEntityRuntime *)army->modelRuntimeOrSavedOffset.modelRuntime);
          Thandor_Log("test aid: arena: faction %u lab %u researches technology %d", (unsigned)army->factionIndex,
                      labs[index].assetId, tech);
          break;
        }
      }
    }
  }
}

/* "production" summary at the last recorded step: units built (relationCounterA), Xenite, Tritium, queue left and
   the technologies unlocked since the level start, per faction. */
static void DebugArena_ProductionSummary(int step)
{
  static uint32_t s_startMasks[3][8];
  int faction;
  int word;
  if (step != 1 && step != s_stepsWanted) {
    return;
  }
  for (faction = 1; faction <= 2; faction++) {
    GameFactionRuntimeRecord *record = &g_GameFactionRuntimeImage.records[faction];
    char unlocked[128];
    int length = 0;
    unlocked[0] = '\0';
    for (word = 0; word < 8; word++) {
      uint32_t added;
      int bit;
      if (step == 1) {
        s_startMasks[faction][word] = record->technologyMasks256Bits[word];
        continue;
      }
      added = record->technologyMasks256Bits[word] & ~s_startMasks[faction][word];
      for (bit = 0; bit < 32 && length < (int)sizeof unlocked - 8; bit++) {
        if ((added >> bit) & 1u) {
          length += sprintf(unlocked + length, " %d", word * 32 + bit);
        }
      }
    }
    if (step != 1) {
      Thandor_Log("test aid: arena: faction %d built %d units, Xenite %d, Tritium %d, queue %d, researched:%s",
                  faction, (int)record->relationCounterA, (int)(record->xeniteCurrentQ4 >> 4),
                  (int)(record->tritiumCurrentQ4 >> 4), (int)record->secondaryArmyAssetCount, unlocked);
    }
  }
}

static void DebugArena_Orders(int step)
{
  const char *orders = getenv("OPEN_THANDOR_ARENA_ORDERS");
  if (orders == NULL) {
    return;
  }
  if (strcmp(orders, "move") == 0) {
    DebugArena_MoveOrders(step);
  }
  else if (strcmp(orders, "production") == 0) {
    DebugArena_ProductionSummary(step);
    DebugArena_ProductionOrders(step);
  }
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
  DebugArena_Orders(s_stepsDone);
  if (s_stepsDone >= s_stepsWanted) {
    fclose(s_output);
    s_output = NULL;
    Thandor_Log("test aid: state hash done after %d steps (tick %u)", s_stepsDone, tick);
    ExitProcess(0);
  }
}

#endif
