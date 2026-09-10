#ifndef THANDOR_WORLD_EFFECTS_RUNTIME_H
#define THANDOR_WORLD_EFFECTS_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/effects/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E120 */
EffectDefinition * EffectRuntime_FindDefinitionByIdCf(PckEffectDefinitionIdCatalog definitionId);

/* 0x0051E190 */
void EffectRuntime_InitGraphicsResources(word *mutableBasePath);

/* 0x0051E210 */
void EffectRuntime_ShutdownGraphicsResources(void);

/* 0x0051E340 */
void __cdecl EffectRuntime_RebaseSlotsAfterLoad(void);

/* 0x0051E4A0 */
undefined8 __fastcall EffectRuntimePool_CreateInstanceFromDefinitionCf (undefined4 param_1,undefined4 param_2,EffectRuntimeCompletionAction completionAction, EffectRuntimeOwnerReference4 ownerRuntime,AngleTurn32 orientationAngle0, AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,Q12 worldZQ12,Q12 worldXQ12, Q12 worldYQ12,EffectDefinition *effectDefinition,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_EFFECTS_RUNTIME_H */
