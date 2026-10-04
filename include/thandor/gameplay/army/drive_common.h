/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/drive_common.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_DRIVE_COMMON_H
#define THANDOR_GAMEPLAY_ARMY_DRIVE_COMMON_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

void ArmyRuntime_UpdateActivationMetricAndPlayStartSound
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

void ArmyRuntime_HandleCollisionPartner(ModelRuntimeSlot *currentModelRuntime,Q12 currentWorldYQ12,Q12 currentWorldXQ12,
          ModelRuntimeSlot *collisionPartnerModelRuntime,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_DRIVE_COMMON_H */
