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
StatusResult __thandor_void_preserve_ecx_edx
EffectAsset_PrepareEntries(EffectAssetHeader *asset);

/* 0x0051E3E0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx EffectDefinitions_ResolveCrossReferences(void);

/* 0x0051DFD0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition);

/* 0x0051E440 */
EffectDefinitionResult __thandor_eax_cf_preserve_ecx_edx
EffectDefinitionRegistry_FindByIdWithError(PckEffectDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_EFFECT_CATALOG_H */
