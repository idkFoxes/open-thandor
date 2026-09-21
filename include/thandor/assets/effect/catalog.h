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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E0B0 */
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx
EffectAsset_PrepareEntries(EffectAssetHeader *asset);

/* 0x0051E3E0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx EffectDefinitions_ResolveCrossReferences(void);

/* 0x0051DFD0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectDefinition_RegisterAndLoadSpriteCf(EffectDefinition *definition);

/* 0x0051E440 */
EffectDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectDefinitionRegistry_FindByIdWithErrorCf(PckEffectDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_EFFECT_CATALOG_H */
