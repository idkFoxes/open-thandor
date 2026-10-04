/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/damage.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_DAMAGE_H
#define THANDOR_GAMEPLAY_ARMY_DAMAGE_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/damage. */

/* healthRegenerationDelayTicks after every hit: health regenerates again this many ticks later */
#define ARMY_DAMAGE_REGENERATION_DELAY_TICKS 0x200

/* packed point key class of a model's damage-effect emitter points (ArmyRuntime_EmitDamageThresholdEffect) */
#define ARMY_MODEL_POINT_CLASS_DAMAGE_EMITTER 3

/* Functions are grouped by semantic ownership. */

void ArmyRuntimeClass_UpdateTransformAndDamageEffect
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

void ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedEffectsUpdateView *modelRuntime);

void ArmyRuntime_ApplyImpactDamageAndFinalizeState
          (AngleTurn32 impactAngle,DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_ApplyImpactDamageToRuntimeAndParent(AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,ModelRuntimeSlot *targetModelRuntime);

void ArmyRuntime_ApplyDamageAndPropagateToParent(DamageAmount32 damageAmount,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_EmitDamageThresholdEffect(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void GameEntityRuntime_ApplyImpactDamageAndFactionRelationState
          (AngleTurn32 impactAngle,FactionRuntimeIndex sourceFactionIndex,
          ImpactDamageValue32 impactValue,GameEntityRuntime *targetEntityRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_DAMAGE_H */
