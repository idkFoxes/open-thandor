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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052C080 */
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode);

/* 0x0052C1A0 */
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject);

/* 0x0052C1B0 */
void __thandor_void_preserve_eax_ecx_edx
ShotRuntimeMaintenance_UpdateHierarchyProjectedSound
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode);

/* 0x0052C230 */
void __thandor_void_preserve_eax_ecx_edx
ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects
          (WorldRuntimeContext *worldRuntime,ShotModelRuntimeNodeClassView100 *modelNode);

#endif /* THANDOR_WORLD_SHOTS_MAINTENANCE_H */
