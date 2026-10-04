/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/effects/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_EFFECTS_RUNTIME_H
#define THANDOR_WORLD_EFFECTS_RUNTIME_H

#include <thandor/assets/effect/types.h>
#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/effects/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/effects/runtime. */

/* g_EffectRuntimeSlots: a 0x40000-byte pool of 0x40-byte EffectRuntimeSlot entries (EffectRuntime_InitGraphicsResources) */
#define EFFECT_RUNTIME_SLOT_COUNT 0x1000
#define EFFECT_RUNTIME_POOL_BYTES 0x40000 /* EFFECT_RUNTIME_SLOT_COUNT * sizeof(EffectRuntimeSlot) */

/* Functions are grouped by semantic ownership. */

EffectDefinition *EffectRuntime_FindDefinitionById(PckEffectDefinitionIdCatalog definitionId);

Bool8 EffectRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError);

void EffectRuntime_ShutdownGraphicsResources();

void EffectRuntime_RebaseSlotsAfterLoad();

EffectRuntimeSlot *EffectRuntimePool_CreateInstanceFromDefinition
          (EffectRuntimeCompletionAction completionAction,EffectRuntimeOwnerReference ownerRuntime,
          AngleTurn32 orientationAngle0,AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,
          Q12 worldZQ12,Q12 worldXQ12,Q12 worldYQ12,EffectDefinition *effectDefinition,
          WorldRuntimeContext *worldRuntime);

extern GraphicsTextureSet *g_EffectTextureSet;
extern GraphicsPaletteAsset *g_EffectPalette;
extern EffectRuntimeSlot *g_EffectRuntimeSlots;

#endif /* THANDOR_WORLD_EFFECTS_RUNTIME_H */
