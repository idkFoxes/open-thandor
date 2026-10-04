/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/turrets.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_TURRETS_H
#define THANDOR_GAMEPLAY_ARMY_TURRETS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/turrets. */

/* Functions are grouped by semantic ownership. */

void ArmyRuntimeClass_UpdateSingleBarrelTurret (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime);

void ArmyRuntimeClass_UpdateTwinBarrelTurret (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_TURRETS_H */
