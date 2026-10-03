/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/combat.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_COMBAT_H
#define THANDOR_GAMEPLAY_ARMY_COMBAT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/combat. */

/* healthRegenerationDelayTicks (+0xF8) after every hit: health regenerates again this many ticks later */
#define ARMY_DAMAGE_REGENERATION_DELAY_TICKS 0x200
/* launch attachments (reload timers, projectile mesh bits) of the turret-weapon class */
#define ARMY_WEAPON_ATTACHMENT_COUNT 8
/* meshGroupMask bit of the barrel node that shows the projectile of attachment slot 0-7 */
#define ARMY_WEAPON_ATTACHMENT_MESH_BIT(slot) (1 << (slot))
/* shot range check: the projectile flies (lifetime - 2/3 of the ramp ticks - 1) ticks at full speed; -2/3 in Q12 */
#define ARMY_SHOT_RAMP_RANGE_FACTOR_Q12 (-(2 * Q12_ONE / 3))
/* a ground shot without an entity target may hit the terrain this close to the aim distance */
#define ARMY_GROUND_SHOT_LANDING_TOLERANCE_Q12 0x400
/* packed point key class of a model's damage-effect emitter points (ArmyRuntime_EmitDamageThresholdEffect) */
#define ARMY_MODEL_POINT_CLASS_DAMAGE_EMITTER 3
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00523980 */
void ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime);

/* 0x00525130 */
void ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

/* 0x00527AC0 */
void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedEffectsUpdateView *modelRuntime);

/* 0x0052A2E0 */
void ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime);

/* 0x0052A640 */
void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,ModelRuntimeSlot *targetModelRuntime);

/* 0x0052B9D0 */
bool ArmyWeaponRuntime_TestTargetLineOfFire(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x0052A200 */
void ArmyRuntime_ApplyDamageAndPropagateToParent(DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime);

/* 0x00528200 */
void ArmyRuntime_EmitDamageThresholdEffect(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_COMBAT_H */
