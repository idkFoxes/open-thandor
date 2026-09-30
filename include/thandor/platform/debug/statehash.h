/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/statehash.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_STATEHASH_H
#define THANDOR_PLATFORM_DEBUG_STATEHASH_H

/* Determinism test aid (test build only). OPEN_THANDOR_STATEHASH=<steps> writes one line per completed
   simulation step to statehash.txt ("<tick> <hash> armies <n> objects <n> seed <secondary seed>") and exits after
   <steps> steps; the hash covers every world object (class, world position) in update order, every army (asset,
   faction, flags, orders, targets as slot indices, root model health), the faction resources, the simulation tick
   and the simulation random seed - never pointers. OPEN_THANDOR_STATEHASH_SEED=<n> (default 12345) seeds both random
   streams and moves the simulation to the secondary stream, as a network game does, so the per-frame ambient sound
   draws no longer change it. OPEN_THANDOR_STATEHASH_DETAIL=<tick> also writes every army's values at that tick.
   Driven by tools/test/run_determinism.py. */

#ifdef THANDOR_TEST_AIDS
/* After the session is initialised, before its first simulation step (InGameRuntime_RunSessionUntilExit). */
void DebugStateHash_SessionStart(void);
/* After every complete simulation step (InGameRuntime_UpdateSimulationAndNetworkTick, step lock held). */
void DebugStateHash_AfterStep(void);
#endif

#endif /* THANDOR_PLATFORM_DEBUG_STATEHASH_H */
