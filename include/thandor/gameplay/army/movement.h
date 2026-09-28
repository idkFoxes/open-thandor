/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/movement.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_MOVEMENT_H
#define THANDOR_GAMEPLAY_ARMY_MOVEMENT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/movement. */

/* ArmyRuntimeSlot/ArmyMovementRuntime.movementStateFlags bits, as set and tested by the move-command starters
   and ArmyRuntime_UpdateMovementAndWaypoints. Bit 0x40 is not named yet. */
#define ARMY_MOVEMENT_ACTIVE 0x1            /* a move target is set (movementWorld*Q12 / fallbackPosition) */
#define ARMY_MOVEMENT_LOCKED 0x2            /* new move orders are appended to the waypoint queue, not started */
#define ARMY_MOVEMENT_STATIONARY 0x4        /* set by the ground/articulated class updates at the start of a tick,
                                               cleared again when the model moved or turned */
#define ARMY_MOVEMENT_WAYPOINTS_QUEUED 0x8  /* queuedWaypoints[0..queuedWaypointCount-1] are in use */
#define ARMY_MOVEMENT_ROUTE_POINT_REACHED 0x10 /* set by the class updates on reaching the current route point:
                                                  the next ArmyRuntime_UpdateMovementAndWaypoints advances */
#define ARMY_MOVEMENT_TARGET_FOLLOWING 0x20 /* started by ArmyRuntimeCommand_UpdateTargetFollowingState; a new
                                               command resets the move first */
#define ARMY_MOVEMENT_DIRECT 0x80           /* ArmyRuntime_StartDirectMoveCommand: orders are never queued */
#define ARMY_MOVEMENT_ROUTED 0x200          /* routed move command: kept by the reset/target-following paths */
#define ARMY_MOVEMENT_MIRROR_TARGET 0x400   /* ArmyRuntime_SetPendingMoveTarget also sets movementTargetWorld*Q12 */
/* retryCountdown after every route (re)build; while it is non-zero and the model has not moved,
   ArmyRuntime_UpdateMovementAndWaypoints does not rebuild the route */
#define ARMY_MOVEMENT_RETRY_TICKS 0x40
/* ArmyMovementRuntime.queuedWaypoints[8]; when full, a new waypoint overwrites the last one */
#define ARMY_MOVEMENT_WAYPOINT_CAPACITY 8
/* ArmyRuntime_UpdateMovementAndWaypoints: the route end (fallbackPosition) counts as reached within
   +-0x40 exclusive, the final target (movementTarget*) within +-1.0 (Q12) inclusive on both axes */
#define ARMY_MOVEMENT_ROUTE_END_RADIUS_Q12 0x40
#define ARMY_MOVEMENT_TARGET_RADIUS_Q12 0x1000
/* ArmyRuntime_StartClampedMoveCommand: a target-following move is routed to a point at most 2.0 (Q12)
   away from the current final target (movementTargetWorld*Q12) */
#define ARMY_MOVEMENT_FOLLOW_MAX_STEP_Q12 0x2000
/* Step state of the two-legged articulated walker (runtime-update slot 3), kept in
   ArmyArticulatedRuntimeSlotView.articulatedContact.fallbackPosition0Q12 (+0xB8). The upper 16 bits hold half
   the heading change of a turn step (ArmyArticulatedRuntime_UpdateSelectedTerrainContact). */
#define ARMY_ARTICULATED_STEP_LEFT 0x1          /* the left foot is moving (progress runtimeStateA8) */
#define ARMY_ARTICULATED_STEP_RIGHT 0x2         /* the right foot is moving (progress terrainContactMode) */
#define ARMY_ARTICULATED_STEP_LEFT_LAST 0x4     /* the current/last step is by the left foot */
#define ARMY_ARTICULATED_STEP_RIGHT_LAST 0x8    /* the current/last step is by the right foot */
#define ARMY_ARTICULATED_STEP_TURN 0x10         /* turn on the spot; the other foot follows with the other half */
#define ARMY_ARTICULATED_STEP_WALK 0x20         /* walking step towards the route point */
#define ARMY_ARTICULATED_STEP_CLOSE 0x40        /* closing step: the feet are brought side by side again */
#define ARMY_ARTICULATED_STEP_OBSTRUCTED 0x80   /* the foot target was blocked; a second block cancels the step */
/* step progress runs from 0 to 1.0 (Q12); the step ends once it exceeds 0xFFF */
#define ARMY_ARTICULATED_STEP_PROGRESS_END_Q12 0x1000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00520F60 */
void ArmyRuntimeClass_UpdateArticulatedMovement (WorldRuntimeContext *worldRuntime, ModelRuntimeArticulatedMovementDefinitionView200 *modelRuntime);

/* 0x0051C5A0 */
void ArmyRuntime_ResolveCommandTargetAndRoute(GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00520DF0 */
void ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x00523410 */
void ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime);

/* 0x00523690 */
void ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime);

/* 0x00520140 */
void ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView200 *modelRuntime);

/* 0x00522C00 */
void ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x0051C3E0 */
void ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime);

/* 0x0051C500 */
void ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime);

/* 0x0051CAF0 */
void ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime);

/* 0x0051CDB0 */
void ArmyRuntime_SetPendingMoveTarget(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00521680 */
void ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00527BC0 */
void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C8E0 */
void ArmyRuntime_QueueOrStartMoveCommandVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051C9A0 */
void ArmyRuntime_QueueOrStartMoveCommandVariantB
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00520840 */
void ArmyRuntimeClass_UpdateGroundMovementVariantA (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x00522090 */
void ArmyArticulatedRuntime_UpdateLeftTerrainContact(AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005222F0 */
void ArmyArticulatedRuntime_UpdateRightTerrainContact(AngleTurn32 headingAngle16,Q12 routeDistanceQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005254F0 */
void ArmyRuntimeClass_UpdateGroundMovementVariantB (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x0051CC60 */
void ArmyRuntime_StartClampedMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CD30 */
void ArmyRuntime_StartDirectMoveCommand(Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00521580 */
void ArmyArticulatedRuntime_UpdateContactChildAndEffects(ModelRuntimeNode *legNode,WorldRuntimeContext *worldRuntime,
          ArmyRuntimeSlot *armyRuntime);

/* 0x005217A0 */
void ArmyArticulatedRuntime_UpdateSuspensionHierarchy
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00522550 */
void ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00522660 */
void ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00522770 */
void ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00523340 */
bool ArmyRuntimeCommand_UpdateTargetFollowingState(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051CA60 */
void ArmyRuntime_QueueWaypointOrStartMoveVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CB90 */
void ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CE30 */
void ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime);

/* 0x0051CE90 */
void ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime);

/* 0x0051CEE0 */
MovementStepResult ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_MOVEMENT_H */
