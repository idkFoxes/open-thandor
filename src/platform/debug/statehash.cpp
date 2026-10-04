/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/statehash.cpp
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/statehash.h>

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
static unsigned s_pauseTick;    /* OPEN_THANDOR_STATEHASH_PAUSE_AT: pause the game after this tick instead of exiting
                                   (a frame-rate independent moment for screenshot comparisons) */
static FILE *s_output;
static int s_speedStepTicks;    /* OPEN_THANDOR_STATEHASH_SPEED, 0 = keep the game's speed (1); applied once */

/* The game speed is applied at the end of this simulation tick, not at a wall-clock moment such as
   DebugStateHash_SessionStart: the steps run during the initialisation (the "waiting for players" loop of
   InGameRuntime_InitializeNewSession, which sets the speed back to 1 before them) are 1, 2 or 3 depending on the
   load, and every step scales movement, timers and production by g_InGameSimulationStepTicks, so applying the
   speed at the session start made the runs diverge (seen as a statehash.txt starting at tick 3 or 4). Tick 2 is
   where the speed took effect in the usual unloaded run, so those runs keep their hashes. */
#define STATEHASH_SPEED_AFTER_TICK 2u

static int32_t DebugStateHash_ArmyIndex(const void *army)
{
  if (army == nullptr) {
    return -1;
  }
  return (int32_t)(((const uint8_t *)army - (const uint8_t *)g_ArmyRuntimeSlots) / (int)sizeof(ArmyRuntimeSlot));
}

void DebugStateHash_SessionInitializing(void)
{
  const char *steps = getenv("OPEN_THANDOR_STATEHASH");
  const char *seed = getenv("OPEN_THANDOR_STATEHASH_SEED");
  if (steps == nullptr || atoi(steps) <= 0) {
    return;
  }
  /* The simulation draws from g_RandomGeneratorState.next; in a local game that is the primary stream, which the
     frontend and the in-game UI also advance per drawn frame (ambient sound and music in InGameUiRoot_UpdateFrame,
     button and input code), so the run would depend on the frame rate. A network game seeds both streams and moves
     the simulation to the secondary one before the session is built; do the same with a fixed seed. It has to
     happen here, before the level is instantiated: the first simulation steps (ticks 1-2, under load 1-3) already
     run inside the initialisation, in the "waiting for players" loop of InGameRuntime_InitializeNewSession, before
     DebugStateHash_SessionStart. They draw the first random part of every effect emitter timer
     (ArmyRuntime_UpdateTimedShotAndEffectEmitters); seeded only at SessionStart, those timers came from a primary
     stream that the frames drawn so far had advanced, and one emitter effect (with its random draw) came a tick
     early or late around tick 68 depending on the load. */
  Random_SetBothSeeds((RandomSeed)(seed != nullptr ? strtoul(seed, nullptr, 0) : 12345u));
  Random_SelectSecondaryStream();
  /* OPEN_THANDOR_STATEHASH_SPEED=1..5: game speed (what key G sets), applied after simulation tick
     STATEHASH_SPEED_AFTER_TICK by DebugStateHash_ApplySpeedAtFixedTick */
  {
    const char *speed = getenv("OPEN_THANDOR_STATEHASH_SPEED");
    int stepTicks = speed != nullptr ? atoi(speed) : 0;
    s_speedStepTicks = (stepTicks >= 1 && stepTicks <= INGAME_SIMULATION_STEP_TICKS_MAX) ? stepTicks : 0;
  }
}

void DebugStateHash_SessionStart(void)
{
  const char *steps = getenv("OPEN_THANDOR_STATEHASH");
  const char *seed = getenv("OPEN_THANDOR_STATEHASH_SEED");
  const char *detail = getenv("OPEN_THANDOR_STATEHASH_DETAIL");
  if (steps == nullptr || atoi(steps) <= 0) {
    return;
  }
  s_stepsWanted = atoi(steps);
  s_stepsDone = 0;
  s_detailTick = detail != nullptr ? (unsigned)atoi(detail) : 0;
  s_pauseTick = getenv("OPEN_THANDOR_STATEHASH_PAUSE_AT") != nullptr ?
                (unsigned)atoi(getenv("OPEN_THANDOR_STATEHASH_PAUSE_AT")) : 0;
  /* the random streams were seeded in DebugStateHash_SessionInitializing */
  s_output = fopen("statehash.txt", "w");
  Thandor_Log("test aid: state hash for %d simulation steps, seed %s -> statehash.txt", s_stepsWanted,
              seed != nullptr ? seed : "12345");
}

/* Scenario orders (OPEN_THANDOR_ARENA_ORDERS), issued from the step hook after fixed simulation ticks (not after a
   number of recorded steps: under load the first recorded tick can differ by one); they run in the next step, as
   the game's own commands do. The ticks equal the step numbers 100, 500 and 20 of the first version plus the two
   ticks before the first recorded one, so the stored references stay valid. */
#define ARENA_MOVE_OUT_TICK 102
#define ARENA_MOVE_BACK_TICK 502
#define ARENA_MOVE_ROWS 20
#define ARENA_PRODUCTION_TICK 22
#define ARENA_ROW_STEP_X 0x480   /* world X per grid row (isometric lattice, see tools/data/fld.py) */
#define ARENA_ROW_STEP_Y (-1999) /* world Y per grid row */

static Q12 s_armyHomeX[ARMY_RUNTIME_SLOT_COUNT];
static Q12 s_armyHomeY[ARMY_RUNTIME_SLOT_COUNT];

/* "move": every army of faction 1 drives ARENA_MOVE_ROWS rows towards the other side (down on the screen), every
   army of faction 2 the same distance the other way (up), then all drive back to where they stood. Immobile
   armies ignore the order (ArmyRuntime_StartRoutedMoveCommand). */
static void DebugArena_MoveOrders(unsigned tick)
{
  unsigned slotIndex;
  if (tick != ARENA_MOVE_OUT_TICK && tick != ARENA_MOVE_BACK_TICK) {
    return;
  }
  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *army = &g_ArmyRuntimeSlots[slotIndex];
    int direction;
    if (army->modelNodeRuntime == nullptr || (army->factionIndex != 1 && army->factionIndex != 2)) {
      continue;
    }
    direction = army->factionIndex == 1 ? 1 : -1;
    if (tick == ARENA_MOVE_OUT_TICK) {
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
static void DebugArena_ProductionOrders(unsigned tick)
{
  static const uint32_t queued[] = {1, 10, 30, 60, 80, 100, 110, 130, 150, 170, 180, 200, 250, 260, 280};
  static const struct { uint32_t assetId; int firstTech; int lastTech; } labs[] = {
    {320, 180, 194}, {321, 91, 110}, {322, 120, 140}, {323, 150, 170}};
  unsigned slotIndex;
  unsigned index;
  int faction;
  if (tick != ARENA_PRODUCTION_TICK) {
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
    if (army->modelNodeRuntime == nullptr || army->modelRuntimeOrSavedOffset.modelRuntime == nullptr) {
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

static void DebugArena_Orders(int step, unsigned tick)
{
  const char *orders = getenv("OPEN_THANDOR_ARENA_ORDERS");
  if (orders == nullptr) {
    return;
  }
  if (strcmp(orders, "move") == 0) {
    DebugArena_MoveOrders(tick);
  }
  else if (strcmp(orders, "production") == 0) {
    DebugArena_ProductionSummary(step);
    DebugArena_ProductionOrders(tick);
  }
}

/* Sets the game speed of OPEN_THANDOR_STATEHASH_SPEED for every player once tick has reached
   STATEHASH_SPEED_AFTER_TICK (from the step hook, under the step lock: the next step runs at the new speed). */
static void DebugStateHash_ApplySpeedAtFixedTick(unsigned tick)
{
  unsigned playerIndex;
  if (s_speedStepTicks == 0 || tick < STATEHASH_SPEED_AFTER_TICK) {
    return;
  }
  for (playerIndex = 0; playerIndex < g_FrontendPlayerRuntimeBlockCount; playerIndex++) {
    g_SelectionPlayerRuntimeBlockPointers[g_FrontendPlayerRuntimeBlocks[playerIndex].playerRuntimeId]
         ->simulationStepTicks = (InGameSimulationStepBatchTicks)s_speedStepTicks;
  }
  g_InGameSimulationStepTicks = (InGameSimulationStepBatchTicks)s_speedStepTicks;
  Thandor_Log("test aid: simulation step ticks %d from tick %u on", s_speedStepTicks, tick + 1);
  s_speedStepTicks = 0;
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
  DebugStateHash_ApplySpeedAtFixedTick(tick);
  if (s_stepsWanted == 0 || s_output == nullptr || g_InGameRuntimeRoot == nullptr) {
    return;
  }
  detail = (tick == s_detailTick);
  StateHash_Add(&hash, tick);
  StateHash_Add(&hash, g_RandomGeneratorState.secondarySeed);
  /* every world object in update order: class and world position (models, shots and effects) */
  for (node = g_InGameRuntimeRoot->worldRuntime.ownerListHead; node != nullptr; node = node->nextNode) {
    StateHash_Add(&hash, (uint32_t)node->ownerClassId);
    StateHash_Add(&hash, (uint32_t)node->worldXQ12);
    StateHash_Add(&hash, (uint32_t)node->worldYQ12);
    StateHash_Add(&hash, (uint32_t)node->worldZQ12);
    if (detail) {
      fprintf(s_output, "  object %u class %u pos %d %d %d\n", nodes, (unsigned)node->ownerClassId,
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
  DebugArena_Orders(s_stepsDone, tick);
  if (s_pauseTick != 0 && tick >= s_pauseTick) {
    /* the flag the pause command toggles once all players agree (InGameCommand_TogglePauseRequest) */
    g_UiCommandRuntimeFlags |= UI_COMMAND_RUNTIME_FLAG_PAUSED;
    fclose(s_output);
    s_output = nullptr;
    s_stepsWanted = 0;
    Thandor_Log("test aid: state hash paused the game at tick %u", tick);
    return;
  }
  if (s_stepsDone >= s_stepsWanted) {
    fclose(s_output);
    s_output = nullptr;
    Thandor_Log("test aid: state hash done after %d steps (tick %u)", s_stepsDone, tick);
    ExitProcess(0);
  }
}
