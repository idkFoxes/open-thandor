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

/* GameEntityRuntime.runtimeFlags (+0xEC) research bits. Technology_IsAvailableForFaction treats an army with
   RESEARCH_RUNNING whose +0x100 holds the technology as already researching it; Technology_ApplyRecordToEntity
   stores the technology in +0x100 and sets RESEARCH_ASSIGNED, and does nothing while either bit is set. */
#define ENTITY_RUNTIME_FLAG_RESEARCH_RUNNING 0x40
#define ENTITY_RUNTIME_FLAG_RESEARCH_ASSIGNED 0x80
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005139C0 */
void __thandor_void_preserve_eax_ecx_edx
Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x00513AE0 */
bool __thandor_cf_preserve_eax_ecx_edx
Technology_IsUnlockedForFaction
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x00513B20 */
bool __thandor_cf_preserve_eax_ecx_edx
Technology_IsAvailableForFaction
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x0052AE10 */
void __thandor_void_preserve_eax_ecx_edx
Technology_ApplyRecordToEntity(PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity);

/* 0x00539BB0 */
void __thandor_void_preserve_eax_ecx_edx
TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void);

#endif /* THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H */
