/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/yield.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_WORLD_PATHING_YIELD_H
#define THANDOR_WORLD_PATHING_YIELD_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/world/pathing/types.h>

/* Improved pathing package P2 (docs/plans/pathfinding.md 3 causes 3 and 4, 5 row P2): yielding to friendly
   blockers. Every function here is called only behind Pathing_Improved(); the original mode never reaches this
   module. Per-unit state lives in g_PathingUnitState (world/pathing/mode.h): waitSteps, blockerSlot,
   sideStepX/Y and the flags PATHING_UNIT_WAITING / PATHING_UNIT_SIDE_STEP.

   - A collision with an own-faction mobile unit (classes 1, 2, 3, 17, 18, 19) that is driving (ARMY_MOVEMENT_ACTIVE
     and an advance above 0) does not set ARMY_MOVEMENT_ROUTE_POINT_REACHED: the mover stands and waits, at most
     PATHING_YIELD_WAIT_STEPS steps in a row (the budget is not reset by a change of blocker), then it re-plans at
     once (retry countdown 0) with the blocker stamped as an obstacle (low distance bands) for that plan.
     A friendly that has a move but stands (turning on the spot or blocked itself) gets the original handling (an
     immediate re-plan when the mover moved since its last plan), except that the retry wait is cut to the same
     budget instead of the original 64 steps. Measured on the pathing arena: waiting for a standing friendly as
     well (the first variant) was slower in the open and point cases than re-planning around it.
   - An idle friendly blocker gets a side step: a short move perpendicular to the mover's direction, away from it,
     after which it is idle again (or drives back into the 0x1000 box of its last target, as after the original
     push). Units standing in another unit's planned corridor get the same side step instead of the original snap
     to the nearest open cell centre (EntityPathing_UpdateRouteSegment); idle units whose stale segment is only
     crossed are no longer pushed at all.
   - Head-on (two moving units each waiting for the other): the lower army slot index side-steps to its right.
   - Two units that already overlap may move apart: a model the mover overlaps does not block a step that
     increases the distance to it.
   Movers: the ground drive (drive_ground.cpp, classes 1, 2, 18, 19). The glider slide (drive_banking.cpp) and the
   walker keep the original collision handling: measured, the glider got slower with it (it rarely collides
   anyway), and the walker sets its foot down beside the other one. Both still count as friendly blockers.
   Integer arithmetic only, no random numbers. */

/* Steps a unit waits for a moving friendly blocker before it re-plans (1.2 s). */
inline constexpr int PATHING_YIELD_WAIT_STEPS = 24;

/* ArmyCollision_FindBlockingRuntimeForCurrentUnit with the move-apart rule: a model the unit already overlaps at
   its current position does not block a next position that lies farther from it. Arguments as there (Y first). */
ModelRuntimeSlot *PathingYield_FindBlockingRuntime
          (Q12 nextWorldYQ12,Q12 nextWorldXQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime);

/* The movement code's collision branch: moverModel ran into blocker. Returns true when the mover waits for a
   friendly (the caller then does not set ARMY_MOVEMENT_ROUTE_POINT_REACHED; a flag the route-point jump set this
   step is cleared), false for the original handling (also when the wait budget is used up: the retry countdown is
   then cleared so the next step re-plans). May give the blocker or the mover a side step. */
bool PathingYield_HandleBlocked
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *moverModel,ModelRuntimeSlot *blocker);

/* The movement code found nothing in the way: the wait (if any) ends and its budget is reset. */
void PathingYield_HandleFree(ArmyRuntimeSlot *moverArmy);

/* EntityPathing_ResolveDestinationAndRebuildRoutes, after the planning masks are set: when the planning army's wait
   budget ran out, its blocker is stamped into the primary scratch grid as an obstacle (the low distance band bits of
   the touched cells are saved first; the start cell keeps its own bits). PathingYield_EndReplan restores the bits
   and clears the army's wait state; it is called for every plan of an army. */
void PathingYield_BeginReplan(ArmyRuntimeSlot *plannerArmy,int startRow,int startColumn);
void PathingYield_EndReplan(ArmyRuntimeSlot *plannerArmy);

/* EntityPathing_RebuildOverlappingGroupRoutes: the planning entity and its target, the direction the side steps
   of idle units in its corridor turn away from. */
void PathingYield_NoteGroupPlan(GameEntityRuntime *routeEntityRuntime,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
                                WorldRuntimeContext *worldRuntime);

/* EntityPathing_UpdateRouteSegment for an idle unit whose start cell lies in a corridor planned earlier: a side
   step point perpendicular to the planner's direction, away from it, that is open in the current (copied) grid
   for the unit's masks (g_GridPathBlockingMask, g_GridPathEntityClassMask as set for it) and free of other models.
   Returns false when there is none (the caller then keeps the original snap). */
bool PathingYield_FindCorridorSideStep
          (EntityPathingRouteEntityRuntimeView *routeEntityRuntime,Q12 *outWorldXQ12,Q12 *outWorldYQ12);

/* Gives the army a side step to the point (ArmyRuntime_SetPendingMoveTarget, then PATHING_UNIT_SIDE_STEP). */
void PathingYield_IssueSideStep(ArmyMovementRuntime *movementRuntime,Q12 worldXQ12,Q12 worldYQ12);

#endif /* THANDOR_WORLD_PATHING_YIELD_H */
