/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/turrets.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_TURRETS_H
#define THANDOR_GAMEPLAY_ARMY_TURRETS_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

void ArmyRuntimeClass_UpdateSingleBarrelTurret (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime);

void ArmyRuntimeClass_UpdateTwinBarrelTurret (WorldRuntimeContext *worldRuntime,ModelRuntimeWeaponAimStateView *modelRuntime);

void FixedVector_StepBackwardAlongOwnDirection
          (FixedVectorStepMultiplier32 stepMultiplier,FixedMathScale32 directionScale,
          FixedVectorStateAddress32 vectorState);

#endif /* THANDOR_GAMEPLAY_ARMY_TURRETS_H */
