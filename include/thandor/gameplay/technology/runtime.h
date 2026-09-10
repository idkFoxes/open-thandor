#ifndef THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H
#define THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/technology/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005139C0 */
void Technology_UnlockForFaction (GraphicsWorldCoordinateQ12 notificationXQ12, GraphicsWorldCoordinateQ12 notificationYQ12,TechnologyId technologyIndex, FactionRuntimeIndex factionIndex);

/* 0x00513AE0 */
undefined4 Technology_IsUnlockedForFactionCf (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x00513B20 */
void Technology_IsAvailableForFactionCf (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex);

/* 0x0052AE10 */
void Technology_ApplyRecordToEntity (PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity);

/* 0x00539BB0 */
undefined8 __fastcall TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks (dword registerContext,FactionRuntimeIndex factionIndex);

#endif /* THANDOR_GAMEPLAY_TECHNOLOGY_RUNTIME_H */
