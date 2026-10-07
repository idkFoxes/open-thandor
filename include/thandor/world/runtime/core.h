/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/runtime/core.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_RUNTIME_CORE_H
#define THANDOR_WORLD_RUNTIME_CORE_H

#include <thandor/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

/* WorldRuntimeContext.runtimeFlags: the flag enum WorldRuntimeFlags (world/runtime/flags.h). */
/* Height returned by WorldRuntime_InterpolateTopSurfaceHeightOrSentinel when no field grid is attached. */
inline constexpr int WORLD_HEIGHT_NO_FIELD_GRID = 0x7ffff000;

void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world);

uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

void WorldRuntime_AttachAndClearDwordArray(WorldWorkspaceElementCount count,uintptr_t *array,WorldRuntimeContext *world);

#endif /* THANDOR_WORLD_RUNTIME_CORE_H */
