/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/effects/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_EFFECTS_RUNTIME_H
#define THANDOR_WORLD_EFFECTS_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/effects/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E120 */
EffectDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectRuntime_FindDefinitionByIdCf(PckEffectDefinitionIdCatalog definitionId);

/* 0x0051E190 */
StatusValueEaxCf5 EffectRuntime_InitGraphicsResources(word *mutableBasePath);

/* 0x0051E210 */
void __thandor_void_preserve_eax_ecx EffectRuntime_ShutdownGraphicsResources(void);

/* 0x0051E340 */
void __thandor_void_preserve_eax_ecx_edx EffectRuntime_RebaseSlotsAfterLoad(void);

/* 0x0051E4A0 */
EffectRuntimeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectRuntimePool_CreateInstanceFromDefinitionCf
          (EffectRuntimeCompletionAction completionAction,EffectRuntimeOwnerReference4 ownerRuntime,
          AngleTurn32 orientationAngle0,AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,
          Q12 worldZQ12,Q12 worldXQ12,Q12 worldYQ12,EffectDefinition *effectDefinition,
          WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_EFFECTS_RUNTIME_H */
