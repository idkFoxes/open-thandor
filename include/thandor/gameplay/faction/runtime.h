/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_RUNTIME_H
#define THANDOR_GAMEPLAY_FACTION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/runtime. */

/* Functions are grouped by semantic ownership. */

Bool8 GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex);

FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ModelRuntimeSlot *hitModelRuntime,WorldRuntimeContext *worldRuntime);

void GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

uint32_t GameFactionRuntime_FindRuntimeGroupNumber(RuntimeModelFactionPrefix *runtimeEntry);

#endif /* THANDOR_GAMEPLAY_FACTION_RUNTIME_H */
