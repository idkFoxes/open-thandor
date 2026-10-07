/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/tick.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_TICK_H
#define THANDOR_GAMEPLAY_SESSION_TICK_H

#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Reduced update (paused world): the grid refresh runs on even ticks with these bits clear, alternating the
   influence bands and the classification masks by tick bit 1 */
inline constexpr int INGAME_REDUCED_GRID_REFRESH_TICK_MASK = 0xC;

void InGameRuntime_PeriodicCountdownAndClockTick();

void InGameRuntime_UpdateSimulationAndNetworkTick();

#endif /* THANDOR_GAMEPLAY_SESSION_TICK_H */
