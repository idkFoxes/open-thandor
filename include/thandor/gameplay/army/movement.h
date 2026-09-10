#ifndef THANDOR_GAMEPLAY_ARMY_MOVEMENT_H
#define THANDOR_GAMEPLAY_ARMY_MOVEMENT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/movement. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00520F60 */
void ArmyRuntimeClass_UpdateArticulatedMovement (WorldRuntimeContext *worldRuntime,ArmyArticulatedRuntimeSlotView *armyRuntime);

/* 0x0051C5A0 */
void ArmyRuntime_ResolveCommandTargetAndRoute (GameEntityRuntime *targetRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00520DF0 */
void ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523410 */
void ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523690 */
void ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00520140 */
void ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime);

/* 0x00522C00 */
void ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime);

/* 0x0051C3E0 */
void ArmyRuntime_ResetMovementStateFromModel(ArmyRuntimeSlot *armyRuntime);

/* 0x0051C500 */
void ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(ArmyRuntimeSlot *armyRuntime);

/* 0x0051CAF0 */
undefined8 __fastcall ArmyRuntime_StartMoveCommandWithAuxiliaryValues (undefined4 incomingEcxValue,undefined4 preservedEdxValue, ArmyMoveAuxiliaryValue1 auxiliaryValue1,ArmyMoveAuxiliaryValue0 auxiliaryValue0, Q12 targetWorldYQ12,Q12 targetWorldXQ12,ArmyMovementRuntime *movementRuntime);

/* 0x0051CDB0 */
void ArmyRuntime_SetPendingMoveTarget (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00521680 */
void ArmyArticulatedRuntime_InitializeTerrainContactGeometry (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00527BC0 */
void ArmyRuntimeClassCommand_NoOp(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C8E0 */
void ArmyRuntime_QueueOrStartMoveCommandVariantA (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051C9A0 */
void ArmyRuntime_QueueOrStartMoveCommandVariantB (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00520840 */
void ArmyRuntimeClass_UpdateGroundMovementVariantA (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime);

/* 0x00522090 */
WorldPositionXYRegisterPairQ12 ArmyArticulatedRuntime_UpdateLeftTerrainContact (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12, ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005222F0 */
WorldPositionXYRegisterPairQ12 ArmyArticulatedRuntime_UpdateRightTerrainContact (AngleTurn32 headingAngle16,Q12 contactDistanceLimitQ12, ArmyArticulatedRuntimeSlotView *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005254F0 */
void ArmyRuntimeClass_UpdateGroundMovementVariantB (WorldRuntimeContext *worldRuntime,ArmyRuntimeMovementCountdownSlotView *armyRuntime);

/* 0x0051CC60 */
void ArmyRuntime_StartClampedMoveCommand (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CD30 */
void ArmyRuntime_StartDirectMoveCommand (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x00521580 */
void ArmyArticulatedRuntime_UpdateContactChildAndEffects (ModelRuntimeNode *contactChildModel,WorldRuntimeContext *worldRuntime, ArmyRuntimeSlot *armyRuntime);

/* 0x005217A0 */
void ArmyArticulatedRuntime_UpdateSuspensionHierarchy (ModelRuntimeNode *modelNodeRuntime,WorldRuntimeContext *worldRuntime);

/* 0x00522550 */
void ArmyArticulatedRuntime_InitializeLeftTerrainContact (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime, WorldRuntimeContext *worldRuntime);

/* 0x00522660 */
void ArmyArticulatedRuntime_InitializeRightTerrainContact (AngleTurn16Stored32 headingAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime, WorldRuntimeContext *worldRuntime);

/* 0x00522770 */
WorldPositionXYRegisterPairQ12 ArmyArticulatedRuntime_UpdateSelectedTerrainContact (AngleTurn32 steeringAngle16,ArmyArticulatedRuntimeSlotView *armyRuntime, WorldRuntimeContext *worldRuntime);

/* 0x00523340 */
void ArmyRuntimeCommand_UpdateTargetFollowingState (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12, WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051CA60 */
void ArmyRuntime_QueueWaypointOrStartMoveVariantA (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CB90 */
void ArmyRuntime_StartMoveCommandWithFallbackWaypoints (Q12 targetWorldY,Q12 targetWorldX,ArmyMovementRuntime *movementRuntime);

/* 0x0051CE30 */
undefined4 ArmyRuntime_ResetMovementStatePreserveQueuedTarget(ArmyMovementRuntime *movementRuntime);

/* 0x0051CE90 */
undefined4 ArmyRuntime_ResetMovementStateFromCurrentPosition(ArmyMovementRuntime *movementRuntime);

/* 0x0051CEE0 */
WorldPositionXYRegisterPairQ12 ArmyRuntime_UpdateMovementAndWaypoints (WorldRuntimeContext *worldRuntime,ArmyMovementRuntime *movementRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_MOVEMENT_H */
