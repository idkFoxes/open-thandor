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
   and ArmyRuntime_UpdateMovementAndWaypoints. Bits 0x04, 0x10 and 0x40 are not named yet. */
#define ARMY_MOVEMENT_ACTIVE 0x1            /* a move target is set (movementWorld*Q12 / fallbackPosition) */
#define ARMY_MOVEMENT_LOCKED 0x2            /* new move orders are appended to the waypoint queue, not started */
#define ARMY_MOVEMENT_WAYPOINTS_QUEUED 0x8  /* queuedWaypoints[0..queuedWaypointCount-1] are in use */
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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00520F60 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateArticulatedMovement (WorldRuntimeContext *worldRuntime, ModelRuntimeArticulatedMovementDefinitionView200 *modelRuntime);

/* 0x0051C5A0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResolveCommandTargetAndRoute
          (GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00520DF0 */
void __thandor_preserve_eax_edx ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x00523410 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime);

/* 0x00523690 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView200 *modelRuntime);

/* 0x00520140 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementTrackView200 *modelRuntime);

/* 0x00522C00 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x0051C3E0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime);

/* 0x0051C500 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime);

/* 0x0051CAF0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartMoveCommandWithAuxiliaryValues
          (ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0,
          Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime);

/* 0x0051CDB0 */
void __thandor_preserve_eax_edx
ArmyRuntime_SetPendingMoveTarget
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00521680 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeTerrainContactGeometry
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00527BC0 */
void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C8E0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueOrStartMoveCommandVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051C9A0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueOrStartMoveCommandVariantB
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00520840 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateGroundMovementVariantA (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x00522090 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateLeftTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005222F0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateRightTerrainContact
          (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12,
          ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005254F0 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateGroundMovementVariantB (WorldRuntimeContext *worldRuntime,ModelRuntimeGroundMovementSteeringView200 *modelRuntime );

/* 0x0051CC60 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartClampedMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CD30 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartDirectMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00521580 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateContactChildAndEffects
          (ModelRuntimeNode *contactChildModel,WorldRuntimeContext *worldRuntime,
          ArmyRuntimeSlot *armyRuntime);

/* 0x005217A0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSuspensionHierarchy
          (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00522550 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeLeftTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00522660 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_InitializeRightTerrainContact
          (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00522770 */
void __thandor_void_preserve_eax_ecx_edx
ArmyArticulatedRuntime_UpdateSelectedTerrainContact
          (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00523340 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeCommand_UpdateTargetFollowingState
          (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051CA60 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_QueueWaypointOrStartMoveVariantA
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CB90 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_StartMoveCommandWithFallbackWaypoints
          (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CE30 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime);

/* 0x0051CE90 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime);

/* 0x0051CEE0 */
MovementStepResult __thandor_eax_edx_cf_preserve_ecx
ArmyRuntime_UpdateMovementAndWaypoints
          (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_MOVEMENT_H */
