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
#include <thandor/gameplay/session/runtime.h>
#include <thandor/gameplay/session/savegame.h>
#include <thandor/gameplay/faction/army_stock.h>

#include <atomic>

/* Reduced update (paused world): the grid refresh runs on even ticks with these bits clear, alternating the
   influence bands and the classification masks by tick bit 1 */
inline constexpr int INGAME_REDUCED_GRID_REFRESH_TICK_MASK = 0xC;

/* g_InGameNetworkTickCountdown is shared between the periodic timer thread (InGameRuntime_PeriodicCountdownAndClockTick
   counts it down) and the main loop (the lockstep reloads it). It stays a plain uint32_t (same layout) and is
   accessed through std::atomic_ref with relaxed ordering: it only paces the steps and publishes no other data
   (the simulation state is ordered by g_InGameStateTickSpinLock), and a relaxed load/store of an aligned 32-bit
   value is the same single mov the plain access was, so the main thread reads and writes the same values. */
static_assert(std::atomic_ref<uint32_t>::is_always_lock_free);
static_assert(std::atomic_ref<uint32_t>::required_alignment == alignof(uint32_t));

inline std::atomic_ref<uint32_t> InGameTick_NetworkTickCountdown()
{
  return std::atomic_ref<uint32_t>(g_InGameNetworkTickCountdown);
}

/* g_InGameResourceRegistrationBusyCount (raised by the main thread while a savegame is written) and
   g_GameFactionRuntimeImage.tail.periodicClockTick (advanced by the timer thread while the busy count is 0, reset
   and read by the main thread) are shared with InGameRuntime_PeriodicCountdownAndClockTick the same way: plain
   variables (same layout), every access through these relaxed std::atomic_ref helpers. Relaxed is enough - the
   busy count only pauses the clock and the clock publishes no other data; the values are the original's. The
   whole-image byte copies of the faction image (daten.hex load / save) are not covered. */
static_assert(std::atomic_ref<uint8_t>::is_always_lock_free);
static_assert(std::atomic_ref<InGamePeriodicClockTick>::is_always_lock_free);
static_assert(std::atomic_ref<InGamePeriodicClockTick>::required_alignment == alignof(InGamePeriodicClockTick));

inline std::atomic_ref<uint8_t> InGameTick_ResourceRegistrationBusyCount()
{
  return std::atomic_ref<uint8_t>(g_InGameResourceRegistrationBusyCount);
}

inline std::atomic_ref<InGamePeriodicClockTick> InGameTick_PeriodicClockTick()
{
  return std::atomic_ref<InGamePeriodicClockTick>(g_GameFactionRuntimeImage.tail.periodicClockTick);
}

void InGameRuntime_PeriodicCountdownAndClockTick();

void InGameRuntime_UpdateSimulationAndNetworkTick();

#endif /* THANDOR_GAMEPLAY_SESSION_TICK_H */
