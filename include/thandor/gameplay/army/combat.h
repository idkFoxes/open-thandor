#ifndef THANDOR_GAMEPLAY_ARMY_COMBAT_H
#define THANDOR_GAMEPLAY_ARMY_COMBAT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/combat. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00523980 */
void ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00525130 */
void ArmyRuntimeClass_UpdateTransformAndDamageEffect (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527AC0 */
void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A2E0 */
void ArmyRuntime_ApplyImpactDamageAndFinalizeState (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A3E0 */
undefined8 ArmyRuntime_ApplyDamageAndFactionRelationState (FactionRuntimeIndex sourceFactionIndex,DamageAmount32 damageAmount, ArmyRuntimeSlot *armyRuntime);

/* 0x0052A640 */
void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex, ImpactDamageValue32 impactValue,ArmyRuntimeSlot *targetArmyRuntime);

/* 0x0052B9D0 */
undefined8 ArmyWeaponRuntime_TestTargetLineOfFireCf (Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12, WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A200 */
void ArmyRuntime_ApplyDamageAndPropagateToParent (DamageAmount32 damageAmount,ArmyRuntimeSlot *armyRuntime);

/* 0x00528200 */
void ArmyRuntime_EmitDamageThresholdEffect (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_COMBAT_H */
