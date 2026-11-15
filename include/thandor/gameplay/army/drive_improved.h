/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/drive_improved.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_DRIVE_IMPROVED_H
#define THANDOR_GAMEPLAY_ARMY_DRIVE_IMPROVED_H

#include <thandor/gameplay/army/types.h>
#include <thandor/core/contracts.h>

/* open-thandor, improved pathing mode only (Pathing_Improved(), docs/plans/pathfinding.md 5, packages P1 and P6).
   Not in the original; the original movement code calls these from one branch per entry point and is unchanged
   when the mode is off. Integer and table math only (deterministic on every compiler), no random numbers. */

/* P1: the advance of a ground unit (drive_ground.cpp, ArmyGroundMovement_SteerAndDrive) after it has turned this
   step. facingAngle is the new facing, routeAngleAndLength the direction and distance to the route point.
   The allowed heading error is a quarter turn for an intermediate route point; only the final route point
   (the plan's fallback position, no queued waypoint) keeps the original narrowing towards the near limit inside
   the interpolation distance (owner decision D4), blended from a quarter turn instead of the far limit. Within the
   limit the unit accelerates as in the original but its advance is capped at movementSpeed * cos(heading error),
   approached by deceleration instead of a jump. Beyond the limit it coasts: the advance halves each step and
   *coasting is set; the caller moves the unit along its facing (never onto the route point). Returns false when
   the unit stands (advance 0): the caller places it stationary and it turns on the spot. */
bool ArmyDriveImproved_UpdateGroundAdvance
          (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView *modelRuntime,
           Q12 waypointWorldXQ12,Q12 waypointWorldYQ12,uint32_t facingAngle,FixedLengthAngle routeAngleAndLength,
           bool *coasting);

/* P6: a banking unit (glider, drive_banking.cpp) gets a new route point. The original drops speed and turn velocity
   to 0; here the turn velocity is still cleared but the speed keeps its component along the new direction
   (advance * cos(change of the travel direction)), 0 for a change of a quarter turn or more. The old travel
   direction is the direction to the old route point while the unit is still away from it, otherwise the bank
   heading (classState64), which follows the travel direction while the unit moves. */
void ArmyDriveImproved_KeepBankingSpeedOnNewRoutePoint
          (ModelRuntimeGroundMovementSteeringView *modelRuntime,ModelRuntimeNode *rootNode,
           Q12 oldWaypointWorldXQ12,Q12 oldWaypointWorldYQ12,Q12 waypointWorldXQ12,Q12 waypointWorldYQ12);

#endif /* THANDOR_GAMEPLAY_ARMY_DRIVE_IMPROVED_H */
