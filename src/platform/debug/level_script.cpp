/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/level_script.cpp
 * Project code (not in the original game)
 */

/* Level-script log (developer tools): hooks of InGameConditionRuntime_UpdateScheduledRecords
   (gameplay/session/runtime.c). At simulation tick 20 it writes every used condition (16 raw bytes) and trigger
   of the level script to thandor.log, before and after their first evaluation, and it logs every end trigger
   that fires. Always on in a THANDOR_DEV_TOOLS build (the campaign tools read it). */

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* the conditions and triggers are logged at this simulation tick, the first evaluation */
#define LEVEL_SCRIPT_LOG_TICK 20

void DebugHook_LevelScriptBeforeEvaluation(void)
{
  const uint8_t *raw;
  int index;
  if (g_GameFactionRuntimeImage.tail.simulationTick != LEVEL_SCRIPT_LOG_TICK) {
    return;
  }
  /* dump the level script before its first evaluation: every used condition (16 raw bytes) and trigger */
  raw = (const uint8_t *)&(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
  for (index = 0; index < 64; index++) {
    const uint8_t *c = raw + index * 16;
    if (c[0] != 0) {
      Thandor_Log("level script: condition %2d: %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x %02x %02x | "
                  "%02x %02x %02x %02x",index,c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],c[8],c[9],c[10],c[11],
                  c[12],c[13],c[14],c[15]);
    }
  }
  for (index = 0; index < 16; index++) {
    const uint8_t *t = (const uint8_t *)&(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule.triggers[index];
    if (t[0] != 0) {
      Thandor_Log("level script: trigger %2d: %02x %02x %02x %02x %02x %02x %02x %02x",index,t[0],t[1],t[2],t[3],
                  t[4],t[5],t[6],t[7]);
    }
  }
}

void DebugHook_LevelScriptAfterEvaluation(const InGameLevelConditionStorage *storage)
{
  const uint8_t *c;
  if (g_GameFactionRuntimeImage.tail.simulationTick != LEVEL_SCRIPT_LOG_TICK) {
    return;
  }
  c = (const uint8_t *)&(storage->schedule).conditions[10];
  Thandor_Log("level script: after evaluation condition 10: %02x %02x %02x %02x | %02x, storage %p/%p",
              c[0],c[1],c[2],c[3],c[4],(const void *)storage,
              (void *)g_InGameLevelRuntimeGlobalBlock.conditionStorage);
}

void DebugHook_LevelScriptEndTrigger(const InGameLevelConditionStorage *storage, int triggerIndex,
                                     const InGameEndConditionTriggerRecord8 *trigger)
{
  const InGameScheduledConditionRecord10 *condition = &(storage->schedule).conditions[trigger->conditionIndex];
  Thandor_Log("level script: end trigger %u fired at tick %u: condition %u kind %u operands %d %d %d, "
              "faction %u (local %u, lifecycle %u), end selection %u",
              (unsigned)triggerIndex,(unsigned)g_GameFactionRuntimeImage.tail.simulationTick,
              (unsigned)trigger->conditionIndex,(unsigned)(condition->statusAndKind.kind & ~1u),
              ((const int *)condition)[1],((const int *)condition)[2],((const int *)condition)[3],
              (unsigned)trigger->factionRuntimeIndex,
              (unsigned)(g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex,
              (unsigned)g_GameFactionRuntimeImage.tail.factionLifecycleStates[trigger->factionRuntimeIndex],
              (unsigned)trigger->endMovieSelectionIndex);
}
