/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/move_orders.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_MOVE_ORDERS_H
#define THANDOR_GAMEPLAY_ARMY_MOVE_ORDERS_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* ArmyRuntimeSlot/ArmyMovementRuntime.movementStateFlags bits, as set and tested by the move-command starters
   and ArmyRuntime_UpdateMovementAndWaypoints. */
inline constexpr int ARMY_MOVEMENT_ACTIVE = 0x1; /* a move target is set (movementWorld*Q12 / fallbackPosition) */
inline constexpr int ARMY_MOVEMENT_LOCKED = 0x2; /* new move orders are appended to the waypoint queue, not started */
inline constexpr int ARMY_MOVEMENT_STATIONARY = 0x4; /* set by the ground/articulated class updates at the start of a tick,
                                               cleared again when the model moved or turned */
inline constexpr int ARMY_MOVEMENT_WAYPOINTS_QUEUED = 0x8; /* queuedWaypoints[0..queuedWaypointCount-1] are in use */
inline constexpr int ARMY_MOVEMENT_ROUTE_POINT_REACHED = 0x10; /* set by the class updates on reaching the current route point:
                                                  the next ArmyRuntime_UpdateMovementAndWaypoints advances */
inline constexpr int ARMY_MOVEMENT_TARGET_FOLLOWING = 0x20; /* started by ArmyRuntimeCommand_UpdateTargetFollowingState; a new
                                               command resets the move first */
inline constexpr int ARMY_MOVEMENT_ORDERED = 0x40; /* set by the routed, queued and waypoint move starters, cleared by the
                                               direct/clamped starters and the resets; never tested by the game */
inline constexpr int ARMY_MOVEMENT_DIRECT = 0x80; /* ArmyRuntime_StartDirectMoveCommand: orders are never queued */
inline constexpr int ARMY_MOVEMENT_SPECIAL_BEHAVIOR = 0x100; /* class 18: AiUnitBehavior_UpdatePioneerVehicle runs each tick */
inline constexpr int ARMY_MOVEMENT_ROUTED = 0x200; /* routed move command: kept by the reset/target-following paths */
inline constexpr int ARMY_MOVEMENT_MIRROR_TARGET = 0x400; /* ArmyRuntime_SetPendingMoveTarget also sets movementTargetWorld*Q12 */
/* retryCountdown after every route (re)build; while it is non-zero and the model has not moved,
   ArmyRuntime_UpdateMovementAndWaypoints does not rebuild the route */
inline constexpr int ARMY_MOVEMENT_RETRY_TICKS = 0x40;
/* ArmyMovementRuntime.queuedWaypoints[8]; when full, a new waypoint overwrites the last one */
inline constexpr int ARMY_MOVEMENT_WAYPOINT_CAPACITY = 8;

/* ArmyRuntime_StartClampedMoveCommand: a target-following move is routed to a point at most 2.0 (Q12)
   away from the current final target (movementTargetWorld*Q12) */
inline constexpr int ARMY_MOVEMENT_FOLLOW_MAX_STEP_Q12 = 0x2000;

void ArmyRuntime_ResolveCommandTargetAndRoute(GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime);

void ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime);

void ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime);

void ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_SetPendingMoveTarget(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_StartRoutedMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_StartNextQueuedWaypointMove
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_StartClampedMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_StartDirectMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

Bool8 ArmyRuntimeCommand_UpdateTargetFollowingState(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_AppendWaypointOrStartMove
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime);

Bool8 ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime,Q12 *outWorldXQ12,
          Q12 *outWorldYQ12);

extern const ArmyCommandGeneration g_ArmyCommandGenerationStandard;

/* ArmyRuntimeSlot.commandModeFlags: what the current command targets (ArmyRuntime_ResolveCommandTarget,
   ArmyRuntime_ApplyTargetPositionCommand, ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration) */
inline constexpr int ARMY_COMMAND_MODE_TARGET_ARMY = 0x1; /* commandTargetArmyRuntime */

inline constexpr int ARMY_COMMAND_MODE_TARGET_POSITION = 0x2; /* commandCoordinate0-2Q12 */

inline constexpr int ARMY_COMMAND_MODE_INTERRUPTED = 0x4; /* a target command was cancelled; commandGeneration re-stamped */

inline constexpr int ARMY_COMMAND_MODE_AI_COMBAT_TARGET = 0x8; /* target picked by the AI combat target selection: target-following

                                                  moves are clamped (ArmyRuntime_StartClampedMoveCommand) */
inline constexpr int ARMY_COMMAND_MODE_SELECTION_ORDER = 0x10; /* target/position order given to the selection (set with INTERRUPTED);

                                                  cleared by move commands and SelectionRuntime_CancelTargets */
inline constexpr int ARMY_COMMAND_MODE_UNUSED_400 = 0x400; /* cleared by ArmyRuntime_AppendWaypointOrStartMove; never set or tested */

void ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime);

void ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinate2Q12,Q12 coordinate1Q12,Q12 coordinate0Q12,ArmyRuntimeSlot *armyRuntime);

void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime);

Bool8 GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState,FixedVectorQ12 *outPosition);

#endif /* THANDOR_GAMEPLAY_ARMY_MOVE_ORDERS_H */
