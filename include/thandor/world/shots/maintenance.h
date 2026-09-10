#ifndef THANDOR_WORLD_SHOTS_MAINTENANCE_H
#define THANDOR_WORLD_SHOTS_MAINTENANCE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/shots/maintenance. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052C080 */
void ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNode);

/* 0x0052C1A0 */
void ShotRuntimeMaintenance_OccupancyRebuildNoOp (WorldRuntimeContext *worldRuntime,void *runtimeObject);

/* 0x0052C1B0 */
void ShotRuntimeMaintenance_UpdateHierarchyProjectedSound (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime);

/* 0x0052C230 */
void ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNode);

#endif /* THANDOR_WORLD_SHOTS_MAINTENANCE_H */
