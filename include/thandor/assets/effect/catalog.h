#ifndef THANDOR_ASSETS_EFFECT_CATALOG_H
#define THANDOR_ASSETS_EFFECT_CATALOG_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/effect/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0051E0B0 */
dword EffectAsset_PrepareEntries(EffectAssetHeader *asset);

/* 0x0051E3E0 */
void __cdecl EffectDefinitions_ResolveCrossReferences(void);

/* 0x0051DFD0 */
undefined8 EffectDefinition_RegisterAndLoadSpriteCf(EffectDefinition *definition);

/* 0x0051E440 */
EffectDefinition * EffectDefinitionRegistry_FindByIdWithErrorCf(PckEffectDefinitionIdCatalog definitionId);

#endif /* THANDOR_ASSETS_EFFECT_CATALOG_H */
