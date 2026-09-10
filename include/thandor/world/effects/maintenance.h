#ifndef THANDOR_WORLD_EFFECTS_MAINTENANCE_H
#define THANDOR_WORLD_EFFECTS_MAINTENANCE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/effects/maintenance. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E790 */
void EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime);

/* 0x0051E830 */
void EffectRuntimeMaintenance_OccupancyRebuildNoOp (WorldRuntimeContext *worldRuntime,void *runtimeObject);

/* 0x0051E840 */
void EffectRuntimeMaintenance_AudioRefreshNoOp (WorldRuntimeContext *worldRuntime,void *runtimeObject);

/* 0x0051E850 */
void EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode);

#endif /* THANDOR_WORLD_EFFECTS_MAINTENANCE_H */
