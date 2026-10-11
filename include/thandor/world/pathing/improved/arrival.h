/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/improved/arrival.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_IMPROVED_ARRIVAL_H
#define THANDOR_WORLD_PATHING_IMPROVED_ARRIVAL_H

#include <cstdint>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/gameplay/selection/types.h>

/* open-thandor (docs/plans/pathfinding.md 5, package P3: arrival for groups). Improved pathing mode only
   (Pathing_Improved()); nothing here is called with the original mode. Integer math, no random numbers. */

/* PathingUnitState.flags bit of P3 (P2 owns 0x01 and 0x02): the unit counts as arrived next to its occupied group
   target and stays so while its movement is inactive (its final target lies outside the original 0x1000 box). */
inline constexpr uint8_t PATHING_UNIT_GROUP_ARRIVED = 0x80;

/* Gap between two neighbouring units counted as "touching" (added to the radius sum) and between formation slots. */
inline constexpr int PATHING_ARRIVAL_MARGIN_Q12 = 0x800;

/* Consecutive re-plans next to an arrived friendly near the target after which the unit counts as arrived. */
inline constexpr uint16_t PATHING_ARRIVAL_FAILED_PLAN_LIMIT = 6;

/* Called by ArmyRuntime_UpdateMovementAndWaypoints (improved mode) where the original would re-plan (route point
   reached away from the route end, or route end reached outside the target box). planning tells whether a plan
   follows (false: the unit only waits for its retry countdown). Returns true when the unit counts as arrived:
   - it already arrived at its group target before and is still inactive;
   - it touches (radius sum + margin) an arrived own-faction mobile unit that stands on its target or has the same
     final target (a point order: the arrived units form a cluster around the point);
   - or after PATHING_ARRIVAL_FAILED_PLAN_LIMIT plans in a row near its target while touching an arrived friendly
     (PathingUnitState.failedPlans).
   The caller then ends the movement with PathingArrival_Finish. */
bool PathingArrival_TestArrived(ArmyMovementRuntime *movementRuntime,bool planning);

/* Ends the movement like the original arrival (move flags cleared, the current position as steering point) and
   marks the unit PATHING_UNIT_GROUP_ARRIVED. Returns true (arrived). */
bool PathingArrival_Finish(ArmyMovementRuntime *movementRuntime,Q12 *outWorldXQ12,Q12 *outWorldYQ12);

/* Formation slots for a point order of a multi-unit selection (the selection is spread too widely to keep its
   offsets): every mobile entry gets its own slot of a hex spiral around the target at unit spacing, only on cells
   passable for the largest unit; the slots are handed out far side first (beyond the target, seen from the group
   centre) to the entries nearest to the target first (ties by spiral and entry order), so units arriving later need
   not pass the ones already standing. Writes the
   slot of entry i to slotX/Y[i] (entries without a slot keep the target). Returns false when there is nothing to
   spread (fewer than two mobile entries or no scratch grid): the caller then keeps the original single point. */
bool PathingFormation_AssignPointSlots(Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection,
                                       Q12 slotWorldX[32],Q12 slotWorldY[32]);

#endif /* THANDOR_WORLD_PATHING_IMPROVED_ARRIVAL_H */
