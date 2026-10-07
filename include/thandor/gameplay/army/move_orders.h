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

/* The movementStateFlags bits ARMY_MOVEMENT_* are ArmyMovementStateFlags (gameplay/army/types.h). */
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

bool ArmyRuntimeCommand_UpdateTargetFollowingState(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_AppendWaypointOrStartMove
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime);

void ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime);

bool ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime,Q12 *outWorldXQ12,
          Q12 *outWorldYQ12);

extern const ArmyCommandGeneration g_ArmyCommandGenerationStandard;

/* The commandModeFlags bits ARMY_COMMAND_MODE_* are ArmyCommandModeFlags (gameplay/army/types.h). */

void ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime);

void ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinate2Q12,Q12 coordinate1Q12,Q12 coordinate0Q12,ArmyRuntimeSlot *armyRuntime);

void GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(GameEntityRuntime *entityRuntime);

bool GameEntityRuntime_ResolveCommandTargetPosition(GameEntityRuntime *targetState,FixedVectorQ12 *outPosition);

#endif /* THANDOR_GAMEPLAY_ARMY_MOVE_ORDERS_H */
