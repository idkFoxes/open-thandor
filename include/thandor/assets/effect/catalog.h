/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/effect/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_EFFECT_CATALOG_H
#define THANDOR_ASSETS_EFFECT_CATALOG_H

#include <thandor/assets/effect/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/effect/catalog. */

/* Slots of g_EffectDefinitionRegistry (256 pointers; a null slot is free). */
#define EFFECT_DEFINITION_REGISTRY_SLOT_COUNT 256
/* Functions are grouped by semantic ownership. */

Bool8 EffectAsset_PrepareEntries(EffectAssetHeader *asset,uint32_t *outError);

uint32_t EffectDefinitions_ResolveCrossReferences(void);

Bool8 EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition,uint32_t *outError);

uint32_t EffectDefinitionRegistry_FindById(PckEffectDefinitionIdCatalog definitionId,EffectDefinition **outDefinition);

extern EffectDefinition *g_EffectDefinitionRegistry[256];

#endif /* THANDOR_ASSETS_EFFECT_CATALOG_H */
