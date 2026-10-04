/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/statehash.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_STATEHASH_H
#define THANDOR_PLATFORM_DEBUG_STATEHASH_H

/* Determinism test aid (developer tools, THANDOR_DEV_TOOLS). OPEN_THANDOR_STATEHASH=<steps> writes one line per completed
   simulation step to statehash.txt ("<tick> <hash> armies <n> objects <n> seed <secondary seed>") and exits after
   <steps> steps; the hash covers every world object (class, world position) in update order, every army (asset,
   faction, flags, orders, targets as slot indices, root model health), the faction resources, the simulation tick
   and the simulation random seed - never pointers. OPEN_THANDOR_STATEHASH_SEED=<n> (default 12345) seeds both random
   streams and moves the simulation to the secondary stream, as a network game does, so the per-frame ambient sound
   draws no longer change it. OPEN_THANDOR_STATEHASH_DETAIL=<tick> also writes every army's values at that tick.
   Driven by tools/test/run_determinism.py. */

/* Before the session is built (InGameRuntime_RunSessionUntilExit): seeds both random streams and selects the
   secondary one, before the first simulation steps, which already run during the initialisation; reads the game
   speed of OPEN_THANDOR_STATEHASH_SPEED, which the step hook applies after a fixed simulation tick. */
void DebugStateHash_SessionInitializing();
/* After the session is initialised, before its first frame (InGameRuntime_RunSessionUntilExit): starts recording;
   the steps run during the initialisation are not recorded. */
void DebugStateHash_SessionStart();
/* After every complete simulation step (InGameRuntime_UpdateSimulationAndNetworkTick, step lock held). */
void DebugStateHash_AfterStep();

#endif /* THANDOR_PLATFORM_DEBUG_STATEHASH_H */
