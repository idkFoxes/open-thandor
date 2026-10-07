/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/combat.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_COMBAT_H
#define THANDOR_GAMEPLAY_ARMY_COMBAT_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>
#include <thandor/core/math/fixed_point.h> /* Q12_ONE */

/* launch attachments (reload timers, projectile mesh bits) of the turret-weapon class */
inline constexpr int ARMY_WEAPON_ATTACHMENT_COUNT = 8;
/* meshGroupMask bit of the barrel node that shows the projectile of attachment slot 0-7 */
constexpr int ARMY_WEAPON_ATTACHMENT_MESH_BIT(int slot) { return 1 << slot; }
/* shot range check: the projectile flies (lifetime - 2/3 of the ramp ticks - 1) ticks at full speed; -2/3 in Q12 */
inline constexpr auto ARMY_SHOT_RAMP_RANGE_FACTOR_Q12 = -(2 * Q12_ONE / 3);
/* a ground shot without an entity target may hit the terrain this close to the aim distance */
inline constexpr int ARMY_GROUND_SHOT_LANDING_TOLERANCE_Q12 = 0x400;

void ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime);

bool ArmyWeaponRuntime_TestTargetLineOfFire(Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_COMBAT_H */
