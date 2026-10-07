/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/perception.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_PERCEPTION_H
#define THANDOR_GAMEPLAY_AI_PERCEPTION_H

#include <thandor/core/types.h>
#include <thandor/gameplay/ai/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/pathing/grid.h> /* GRID_SCRATCH_* of the site masks below */
#include <thandor/world/pathing/types.h>
#include <thandor/core/contracts.h>

/* AiPlanning_RebuildFactionWorkspaces site scan: scratch-grid state bits (world/pathing/grid.h) that rule a
   site out - map edge and terrain classes 28..30 - and the low distance bands of grid classes 0..5 */
inline constexpr auto AI_SITE_SCRATCH_OBSTACLE_BITS =
  (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT30 | GRID_SCRATCH_TERRAIN_CLASS_BIT29 |
   GRID_SCRATCH_TERRAIN_CLASS_BIT28); /* 0xf0000000 */
inline constexpr auto AI_SITE_SCRATCH_BANDS_CLASSES_0_TO_5 = 0x3f * GRID_SCRATCH_LOW_BAND0; /* 0x3f00 */

void AiPlanning_RebuildFactionWorkspaces(AiPlanningPhaseIndex planningPhaseDispatchIndex,
          FactionRuntimeIndex unusedFactionIndex,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_PERCEPTION_H */
