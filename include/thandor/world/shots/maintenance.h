/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/shots/maintenance.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_SHOTS_MAINTENANCE_H
#define THANDOR_WORLD_SHOTS_MAINTENANCE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/shots/maintenance. */
/* ShotRuntimeSlot.impactEffectEmissionFlags bit: a beam (direct-line shot) has emitted its impact effect. */
#define SHOT_IMPACT_EFFECT_EMITTED 0x1
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

void ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode);

void ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject);

void ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode);

void ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNode *modelNode);

#endif /* THANDOR_WORLD_SHOTS_MAINTENANCE_H */
