/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/shots/ballistics.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_SHOTS_BALLISTICS_H
#define THANDOR_WORLD_SHOTS_BALLISTICS_H

#include <thandor/assets/shot/types.h>
#include <thandor/core/types.h>
#include <thandor/world/shots/types.h>
#include <thandor/core/contracts.h>

ShotLaunchAngles ShotDefinition_ComputeLaunchAngles
          (Q12 targetZ,Q12 targetY,Q12 targetX,Q12 launchZ,Q12 launchY,Q12 launchX,
          ShotDefinition *definition);

uint32_t ShotDefinition_ComputeSelectionRange(ShotDefinition *definition);

Q12 ShotDefinition_GetLeadSpeed(ShotDefinition *definition);

uint32_t ShotDefinition_ComputeRampUpLeadTime(ShotDefinition *definition);

#endif /* THANDOR_WORLD_SHOTS_BALLISTICS_H */
