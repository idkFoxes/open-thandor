/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/palette.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/palette.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(4) GraphicsPaletteAssetLoadPackageProc *g_GraphicsPaletteAssetLoadPackage = THANDOR_FN(GraphicsPaletteAsset_LoadPackage);

static GraphicsPaletteAssetValidateProc *g_GraphicsPaletteAssetValidate = THANDOR_FN(GraphicsPaletteAsset_Validate);

static GraphicsPaletteAssetResolveAllocationBaseProc *g_GraphicsPaletteAssetResolveAllocationBase = THANDOR_FN(GraphicsPaletteAsset_ResolveAllocationBase);

GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3 = {
    .releasePackage = THANDOR_FN(GraphicsPaletteAsset_ReleasePackage),
    .clone = THANDOR_FN(GraphicsPaletteAsset_Clone),
    .releaseClone = THANDOR_FN(GraphicsPaletteAsset_ReleaseClone)};

/* Implementation ownership: graphics/resources/palette. */


/* Loads a 'pal' palette asset from pathUtf16 (Package_LoadEntry) and validates it through
   g_GraphicsPaletteAssetValidate; an invalid asset is released again. Returns the asset (never NULL), or NULL
   with the load or validation error in *outErrorCode (outErrorCode may be NULL).
   Installed as g_GraphicsPaletteAssetLoadPackage (used by the army graphics and frontend palette loaders).
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode)

{
  GraphicsPaletteAsset *loadedPaletteAsset;
  GraphicsPaletteAsset *validatedPaletteAsset;
  uint32_t errorCode;

  loadedPaletteAsset = (GraphicsPaletteAsset *)Package_LoadEntry(pathUtf16,&errorCode);
  if (loadedPaletteAsset != NULL) {
    validatedPaletteAsset = g_GraphicsPaletteAssetValidate(loadedPaletteAsset,&errorCode);
    if (validatedPaletteAsset != NULL) {
      return validatedPaletteAsset;
    }
    Resource_Release(loadedPaletteAsset);
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return NULL;
}


/* Releases a palette asset loaded by GraphicsPaletteAsset_LoadPackage: resolves its allocation through
   g_GraphicsPaletteAssetResolveAllocationBase and hands it to Resource_Release. Installed as
   g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage.
*/
void GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *allocation;
  
  allocation = g_GraphicsPaletteAssetResolveAllocationBase(paletteAsset);
  Resource_Release(allocation);
  return;
}


/* Makes an independently owned heap copy of a palette asset (allocationSizeBytes, copied dword by dword) and
   validates it; an invalid copy is freed again and the free result returned. Installed as
   g_GraphicsPaletteAssetLifecycleCallbacks3.clone.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *clonedAsset;
  uint32_t allocationSize;
  uint32_t remainingDwords;
  const uint32_t *sourceDword;
  uint32_t *destinationDword;
  uint32_t allocError;
  GraphicsPaletteAsset *validatedAsset;

  allocationSize = paletteAsset->allocationSizeBytes;
  allocError = g_MemoryApi.alloc(allocationSize,(void **)&clonedAsset);
  if (allocError != 0) {
    /* Original quirk: a failed allocation returns its error code as the asset pointer */
    return (GraphicsPaletteAsset *)(uintptr_t)allocError;
  }
  sourceDword = (const uint32_t *)paletteAsset;
  destinationDword = (uint32_t *)clonedAsset;
  for (remainingDwords = allocationSize >> 2; remainingDwords != 0; remainingDwords--) {
    *destinationDword = *sourceDword;
    sourceDword++;
    destinationDword++;
  }
  validatedAsset = g_GraphicsPaletteAssetValidate(clonedAsset,NULL);
  if (validatedAsset != NULL) {
    return validatedAsset;
  }
  /* Original quirk: the clone's result after a failed validation is the free's status (0 = NULL) */
  return (GraphicsPaletteAsset *)(uintptr_t)g_MemoryApi.free(clonedAsset);
}


/* Frees a palette asset made by GraphicsPaletteAsset_Clone: resolves its allocation through
   g_GraphicsPaletteAssetResolveAllocationBase and frees it with g_MemoryApi.free. Installed as
   g_GraphicsPaletteAssetLifecycleCallbacks3.releaseClone.
*/
void GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *allocation;
  
  allocation = g_GraphicsPaletteAssetResolveAllocationBase(paletteAsset);
  g_MemoryApi.free(allocation);
  return;
}


/* Returns paletteAsset when it starts with the 'pal' signature, otherwise NULL with
   FATAL_ERROR_PALETTE_ASSET_INVALID in *outErrorCode (outErrorCode may be NULL). Installed as
   g_GraphicsPaletteAssetValidate.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset,uint32_t *outErrorCode)

{
  if (paletteAsset->magic == ASSET_MAGIC_PAL) {
    return paletteAsset;
  }
  if (outErrorCode != NULL) {
    *outErrorCode = FATAL_ERROR_PALETTE_ASSET_INVALID;
  }
  return NULL;
}


/* Returns the allocation that owns a palette asset, which is the asset itself; both release callbacks go
   through this slot. Installed as g_GraphicsPaletteAssetResolveAllocationBase.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset)

{
  return paletteAsset;
}






