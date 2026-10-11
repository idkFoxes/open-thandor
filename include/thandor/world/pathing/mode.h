/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/mode.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_MODE_H
#define THANDOR_WORLD_PATHING_MODE_H

#include <cstdint>
#include <thandor/gameplay/army/pool.h>

/* open-thandor (docs/plans/pathfinding.md 4, 5): the pathing mode of the running session. Fixed before the session
   is built and never changed during it. With PATHING_MODE_ORIGINAL every pathing and movement path is bit-exact with
   the original game (determinism references, AI hash, multiplayer with 1.05); the improved code sits behind
   Pathing_Improved() branches and draws no extra random numbers. */
enum PathingMode : uint8_t {
  PATHING_MODE_ORIGINAL = 0,
  PATHING_MODE_IMPROVED = 1,
};

/* The mode of the running session (simulation state: the same on every peer, stored in savegames). */
extern PathingMode g_PathingMode;

inline bool Pathing_Improved()
{
  return g_PathingMode == PATHING_MODE_IMPROVED;
}

/* The single-player setting (thandor.ini, "Erweitert" page; owner decision D1: default improved). */
extern PathingMode g_PathingModeSetting;

/* Set by the lobby negotiation (D2) for a network session: true only when every peer is Open Thandor with a
   matching pathing protocol and the improved mode is agreed; false forces the original mode. */
extern bool g_PathingNetworkImprovedAgreed;

/* Chooses g_PathingMode for a new session (local: the setting; networked: the lobby agreement) and clears the
   per-unit side table. Called once before the session's world is built. A loaded savegame sets the mode from its
   extension entry afterwards instead (PathingMode_SetLoaded). */
void PathingMode_ApplyForNewSession(bool networked);

/* Developer tools only (THANDOR_DEV_TOOLS): OPEN_THANDOR_PATHING=original|improved replaces the chosen mode of a
   new session (the measurement and determinism scripts set it explicitly); otherwise, and in a build without the
   developer tools, returns mode unchanged. Applied by PathingMode_ApplyForNewSession and, for test runs, by the
   session-initialising debug hook (src/platform/debug/hooks.cpp) until the session build calls
   PathingMode_ApplyForNewSession.
   Merge note (pathing-mode/pm-harness): the settings package adds the same override; keep this one function
   (name, variable and values as here) and call it once at the end of the new-session mode choice; the call in
   DebugHook_SessionInitializing can then go. */
PathingMode PathingMode_DevOverride(PathingMode mode);

/* A loaded savegame's mode (absent extension entry: PATHING_MODE_ORIGINAL). */
void PathingMode_SetLoaded(PathingMode mode);

/* Per-unit state of the improved mode, indexed like g_ArmyRuntimeSlots (ArmyRuntimeSlot layout stays unchanged).
   Simulation state: saved in the savegame extension and part of the state hash while the improved mode is on;
   all zero and unused with the original mode. Field owners: waitSteps / blockerSlot / flags (P2 yield),
   sideStep* (P2 side step), failedPlans (P3 arrival), reserved for later packages (P4 look-ahead). */
struct PathingUnitState {
  uint16_t waitSteps;      /* P2: steps waited for a friendly blocker without re-planning */
  uint16_t blockerSlot;    /* P2: army slot index + 1 of the blocker waited for, 0 = none */
  int32_t sideStepX;       /* P2: side-step target (world X, Q12), valid with PATHING_UNIT_SIDE_STEP */
  int32_t sideStepY;       /* P2: side-step target (world Y, Q12) */
  uint16_t failedPlans;    /* P3: consecutive plans that did not reach an occupied group target */
  uint8_t flags;           /* PATHING_UNIT_* */
  uint8_t reserved0;
  uint32_t reserved[4];    /* later packages (P4); saved and hashed as zero until used */
};
static_assert(sizeof(PathingUnitState) == 32);

inline constexpr uint8_t PATHING_UNIT_SIDE_STEP = 0x01;  /* P2: moving to sideStepX/Y, then back to the route */
inline constexpr uint8_t PATHING_UNIT_WAITING = 0x02;    /* P2: waiting for blockerSlot */

extern PathingUnitState g_PathingUnitState[ARMY_RUNTIME_SLOT_COUNT];

/* Clears the state of one slot (call when an army slot is created or released). */
void PathingUnitState_Clear(int slotIndex);

#endif /* THANDOR_WORLD_PATHING_MODE_H */
