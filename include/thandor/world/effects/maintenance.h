/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/effects/maintenance.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_EFFECTS_MAINTENANCE_H
#define THANDOR_WORLD_EFFECTS_MAINTENANCE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/effects/maintenance. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E790 */
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode);

/* 0x0051E830 */
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_OccupancyRebuildNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject);

/* 0x0051E840 */
void __thandor_void_preserve_eax_ecx_edx
EffectRuntimeMaintenance_AudioRefreshNoOp(WorldRuntimeContext *worldRuntime,void *runtimeObject);

/* 0x0051E850 */
void __thandor_void_preserve_eax_ecx_edx
EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions
          (WorldRuntimeContext *worldRuntime,EffectModelRuntimeNodeClassView100 *modelNode);

#endif /* THANDOR_WORLD_EFFECTS_MAINTENANCE_H */
