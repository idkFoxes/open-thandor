/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/shots/flight.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_SHOTS_FLIGHT_H
#define THANDOR_WORLD_SHOTS_FLIGHT_H

#include <thandor/ui/ingame/types.h>
#include <thandor/world/shots/types.h>
#include <thandor/core/contracts.h>

/* ShotRuntimeSlot.impactEffectEmissionFlags bit: a beam (direct-line shot) has emitted its impact effect. */
inline constexpr int SHOT_IMPACT_EFFECT_EMITTED = 0x1;

void ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode);

void ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject);

void ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode);

void ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode);

#endif /* THANDOR_WORLD_SHOTS_FLIGHT_H */
