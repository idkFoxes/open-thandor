/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/effect/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_EFFECT_CATALOG_H
#define THANDOR_ASSETS_EFFECT_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/effect/catalog. */

/* Slots of g_EffectDefinitionRegistry (0x0051DBD0, 256 pointers; a null slot is free). */
#define EFFECT_DEFINITION_REGISTRY_SLOT_COUNT 256
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E0B0 */
bool EffectAsset_PrepareEntries(EffectAssetHeader *asset,uint32_t *outError);

/* 0x0051E3E0 */
uint32_t EffectDefinitions_ResolveCrossReferences(void);

/* 0x0051DFD0 */
bool EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition,uint32_t *outError);

/* 0x0051E440 */
uint32_t EffectDefinitionRegistry_FindById(PckEffectDefinitionIdCatalog definitionId,EffectDefinition **outDefinition);

#endif /* THANDOR_ASSETS_EFFECT_CATALOG_H */
