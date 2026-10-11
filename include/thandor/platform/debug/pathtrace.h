/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/pathtrace.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_PATHTRACE_H
#define THANDOR_PLATFORM_DEBUG_PATHTRACE_H

/* Movement trace (developer tools, THANDOR_DEV_TOOLS). OPEN_THANDOR_PATHTRACE=<file> writes, after every simulation
   step, one line per mobile army of faction OPEN_THANDOR_PATHTRACE_FACTION (default 1):
     U <tick> <slot> <x> <y> <heading> <advance> <turnVelocity> <movementFlags> <routeX> <routeY> <fallbackX>
       <fallbackY> <targetX> <targetY> <retryCountdown>
   once per army an info line
     I <slot> <asset> <runtimeClass> <footprintRadius> <radiusClass> <movementSpeed> <acceleration> <faction>
   (for every army, mobile or not, so the layout can be checked), the session's pathing mode (g_PathingMode, 0
   original, 1 improved) before the first step's lines and whenever it changes
     P <tick> <mode>
   and the pathing/movement events as they happen:
     E <tick> R <slot> -1        route (re)planned (EntityPathing_ResolveDestinationAndRebuildRoutes)
     E <tick> T <slot> <planner> another army's planning changed this army's route point
                                 (EntityPathing_UpdateRouteSegment -> ArmyRuntime_SetPendingMoveTarget)
     E <tick> B <slot> <blocker> the next position was blocked by another model (drive / walker collision test)
     E <tick> H <slot> -1        heading error above the limit: advance set to 0, turning on the spot
   Read by tools/test/run_pathing.py. Nothing changes the simulation. */

/* Pathing/movement event (kind 'R', 'T', 'B', 'H'): army and other are ArmyRuntimeSlot pointers (other may be
   NULL). */
void DebugPathTrace_Event(int kind, const void *army, const void *other);
/* After every complete simulation step, before the state hash hook. */
void DebugPathTrace_AfterStep();

#endif /* THANDOR_PLATFORM_DEBUG_PATHTRACE_H */
