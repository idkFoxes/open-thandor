/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/pathtrace.cpp
 * Project code (not in the original game)
 */

/* Movement trace for the pathing measurements (OPEN_THANDOR_PATHTRACE, see pathtrace.h and
   tools/test/run_pathing.py). Only reads the simulation state. */

#include <stdio.h>
#include <stdlib.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/debug/pathtrace.h>
#include <thandor/world/pathing/mode.h>

static FILE *s_traceFile;
static bool s_traceOpened;
static unsigned s_traceFaction = 1;
static bool s_infoWritten[ARMY_RUNTIME_SLOT_COUNT];
static int s_tracedPathingMode = -1;

static FILE *DebugPathTrace_File()
{
  if (!s_traceOpened) {
    const char *path = getenv("OPEN_THANDOR_PATHTRACE");
    const char *faction = getenv("OPEN_THANDOR_PATHTRACE_FACTION");
    s_traceOpened = true;
    if (path != nullptr && path[0] != '\0') {
      s_traceFile = fopen(path, "w");
      if (faction != nullptr) {
        s_traceFaction = (unsigned)atoi(faction);
      }
      Thandor_Log("test aid: movement trace of faction %u -> %s", s_traceFaction, path);
    }
  }
  return s_traceFile;
}

static int DebugPathTrace_SlotIndex(const void *army)
{
  if (army == nullptr) {
    return -1;
  }
  return (int)(Asset_ByteDistance(army, g_ArmyRuntimeSlots) / (int)sizeof(ArmyRuntimeSlot));
}

void DebugPathTrace_Event(int kind, const void *army, const void *other)
{
  FILE *file = DebugPathTrace_File();
  if (file == nullptr || army == nullptr) {
    return;
  }
  if (static_cast<const ArmyRuntimeSlot *>(army)->factionIndex != s_traceFaction) {
    return;
  }
  fprintf(file, "E %u %c %d %d\n", g_GameFactionRuntimeImage.tail.simulationTick, kind,
          DebugPathTrace_SlotIndex(army), DebugPathTrace_SlotIndex(other));
}

void DebugPathTrace_AfterStep()
{
  FILE *file = DebugPathTrace_File();
  unsigned tick = g_GameFactionRuntimeImage.tail.simulationTick;
  unsigned slotIndex;
  if (file == nullptr || g_InGameRuntimeRoot == nullptr) {
    return;
  }
  if ((int)g_PathingMode != s_tracedPathingMode) {
    /* the session's pathing mode (run_pathing.py checks that the requested mode ran) */
    s_tracedPathingMode = (int)g_PathingMode;
    fprintf(file, "P %u %d\n", tick, s_tracedPathingMode);
  }
  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    ArmyRuntimeSlot *army = &g_ArmyRuntimeSlots[slotIndex];
    ModelRuntimeSlot *model;
    ModelDefinition *definition;
    ModelRuntimeNode *node;
    ArmyMovementRuntime *movement;
    if (army->modelNodeRuntime == nullptr) {
      continue;
    }
    model = army->modelRuntimeOrSavedOffset.modelRuntime;
    if (model == nullptr) {
      continue;
    }
    definition = model->definitionOrSavedId.runtimeDefinition;
    if (!s_infoWritten[slotIndex]) {
      s_infoWritten[slotIndex] = true;
      fprintf(file, "I %u %u %u %u %u %d %d %u", slotIndex, (unsigned)army->armyAssetId,
              (unsigned)definition->runtimeClassId, (unsigned)definition->footprintRadius,
              (unsigned)definition->footprintRadiusClass, definition->movementSpeed, definition->accelerationPerTick,
              (unsigned)army->factionIndex);
      if (definition->runtimeClassId == MODEL_RUNTIME_CLASS_01_GROUND ||
          definition->runtimeClassId == MODEL_RUNTIME_CLASS_02_TRACKED ||
          definition->runtimeClassId == MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER ||
          definition->runtimeClassId == MODEL_RUNTIME_CLASS_18 ||
          definition->runtimeClassId == MODEL_RUNTIME_CLASS_19_WATER_SURFACE) {
        /* steering values of the ground movement (drive_ground.cpp): turn rate limit and its acceleration per tick,
           heading error limit far and near, and the distance over which it blends from near to far */
        const ModelDefinitionGroundMovementSteeringView *steering =
            reinterpret_cast<const ModelDefinitionGroundMovementSteeringView *>(definition);
        fprintf(file, " %d %d %u %u %d", (int)steering->turnRateLimitAnglePerTick,
                (int)steering->turnRateAccelerationAnglePerTick, (unsigned)steering->farHeadingErrorLimitAngle,
                (unsigned)steering->nearHeadingErrorLimitAngle, (int)steering->headingErrorInterpolationDistanceQ12);
      }
      fprintf(file, "\n");
    }
    if (army->factionIndex != s_traceFaction || definition->accelerationPerTick == 0) {
      continue;
    }
    node = army->modelNodeRuntime;
    movement = reinterpret_cast<ArmyMovementRuntime *>(army); /* movement view of the slot */
    fprintf(file, "U %u %u %d %d %u %d %d %08x %d %d %d %d %d %d %u\n", tick, slotIndex,
            node->worldTransform.translation.x, node->worldTransform.translation.y,
            (unsigned)node->modelPayload.worldRotationAngle2, model->movementControl.movementAdvancePerTickQ12,
            (int)model->movementControl.turnVelocityAngle16, (unsigned)army->movementStateFlags,
            movement->movementWorldXQ12, movement->movementWorldYQ12, movement->fallbackPosition.worldXQ12,
            movement->fallbackPosition.worldYQ12, movement->movementTargetWorldXQ12,
            movement->movementTargetWorldYQ12, (unsigned)movement->retryCountdown);
  }
  fflush(file);
}
