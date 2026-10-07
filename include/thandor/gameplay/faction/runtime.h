/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_RUNTIME_H
#define THANDOR_GAMEPLAY_FACTION_RUNTIME_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/gameplay/faction/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

Bool8 GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex);

FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ModelRuntimeSlot *hitModelRuntime,WorldRuntimeContext *worldRuntime);

void GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

uint32_t GameFactionRuntime_FindRuntimeGroupNumber(RuntimeModelFactionPrefix *runtimeEntry);

/* Byte size of one GameFactionRuntimeRecord (8 of them in g_GameFactionRuntimeImage). */
inline constexpr int GAME_FACTION_RUNTIME_RECORD_BYTES = 0x740;

#endif /* THANDOR_GAMEPLAY_FACTION_RUNTIME_H */
