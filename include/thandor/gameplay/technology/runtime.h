/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/technology/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H
#define THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/technology/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005139C0 */
void __thandor_void_preserve_eax_ecx_edx
Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x00513AE0 */
bool __thandor_cf_preserve_eax_ecx_edx
Technology_IsUnlockedForFactionCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x00513B20 */
bool __thandor_cf_preserve_eax_ecx_edx
Technology_IsAvailableForFactionCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x0052AE10 */
void __thandor_void_preserve_eax_ecx_edx
Technology_ApplyRecordToEntity(PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity);

/* 0x00539BB0 */
void __thandor_void_preserve_eax_ecx_edx
TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void);

#endif /* THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H */
