/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/palette.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/palette.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(4) GraphicsPaletteAssetLoadPackageProc *g_GraphicsPaletteAssetLoadPackage = &GraphicsPaletteAsset_LoadPackage;

static GraphicsPaletteAssetValidateProc *g_GraphicsPaletteAssetValidate = &GraphicsPaletteAsset_Validate;

static GraphicsPaletteAssetResolveAllocationBaseProc *g_GraphicsPaletteAssetResolveAllocationBase = &GraphicsPaletteAsset_ResolveAllocationBase;

GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3 = {
    .releasePackage = THANDOR_SLOT(GraphicsPaletteAsset_ReleasePackage),
    .clone = THANDOR_SLOT(GraphicsPaletteAsset_Clone),
    .releaseClone = THANDOR_SLOT(GraphicsPaletteAsset_ReleaseClone)};


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

  loadedPaletteAsset = static_cast<GraphicsPaletteAsset *>(Package_LoadEntry(pathUtf16,&errorCode));
  if (loadedPaletteAsset != nullptr) {
    validatedPaletteAsset = g_GraphicsPaletteAssetValidate(loadedPaletteAsset,&errorCode);
    if (validatedPaletteAsset != nullptr) {
      return validatedPaletteAsset;
    }
    Resource_Release(loadedPaletteAsset);
  }
  if (outErrorCode != nullptr) {
    *outErrorCode = errorCode;
  }
  return nullptr;
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
}


/* Makes an independently owned heap copy of a palette asset (allocationSizeBytes, copied dword by dword) and
   validates it. Returns NULL when the allocation fails (one log line) or the copy does not validate (it is
   freed again). Installed as g_GraphicsPaletteAssetLifecycleCallbacks3.clone.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset)

{
  GraphicsPaletteAsset *clonedAsset;
  void *cloneBlock;
  uint32_t allocationSize;
  uint32_t remainingDwords;
  const uint32_t *sourceDword;
  uint32_t *destinationDword;
  uint32_t allocError;
  GraphicsPaletteAsset *validatedAsset;

  allocationSize = paletteAsset->allocationSizeBytes;
  allocError = g_MemoryApi.alloc(allocationSize,&cloneBlock);
  if (allocError != 0) {
    /* The original returned the allocation error code as the asset pointer; bounded here because a caller
       would use that code as an address. */
    Thandor_Log("GraphicsPaletteAsset_Clone: allocation of %u bytes failed (error %u)",allocationSize,allocError);
    return nullptr;
  }
  clonedAsset = static_cast<GraphicsPaletteAsset *>(cloneBlock);
  /* both asset blocks as dwords: the copy is dword by dword */
  sourceDword = reinterpret_cast<const uint32_t *>(paletteAsset);
  destinationDword = static_cast<uint32_t *>(cloneBlock);
  for (remainingDwords = allocationSize >> 2; remainingDwords != 0; remainingDwords--) {
    *destinationDword = *sourceDword;
    sourceDword++;
    destinationDword++;
  }
  validatedAsset = g_GraphicsPaletteAssetValidate(clonedAsset,nullptr);
  if (validatedAsset != nullptr) {
    return validatedAsset;
  }
  /* The original returned the free's status here (0 = NULL, otherwise an error code used as a pointer);
     bounded here because a failed free would hand out that code as an address. */
  g_MemoryApi.free(clonedAsset);
  return nullptr;
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
}


/* Returns paletteAsset when it starts with the 'pal' signature and its paletteBankCount entries (8 bytes each,
   from GRAPHICS_PALETTE_BANKS_OFFSET) lie inside allocationSizeBytes, otherwise NULL with
   FATAL_ERROR_PALETTE_ASSET_INVALID in *outErrorCode (outErrorCode may be NULL). Installed as
   g_GraphicsPaletteAssetValidate.
   The original only checked the signature; the entry range is checked here because the model and terrain
   packets index the entries by paletteBankCount and a malformed asset made them read past it.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset,uint32_t *outErrorCode)

{
  if (paletteAsset->magic == ASSET_MAGIC_PAL) {
    if (paletteAsset->allocationSizeBytes >= GRAPHICS_PALETTE_BANKS_OFFSET &&
        (uint64_t)paletteAsset->paletteBankCount * sizeof(GraphicsPaletteAssetEntry) <=
            paletteAsset->allocationSizeBytes - GRAPHICS_PALETTE_BANKS_OFFSET) {
      return paletteAsset;
    }
    Thandor_Log("GraphicsPaletteAsset_Validate: rejected pal asset (%u bytes, %u entries)",
                paletteAsset->allocationSizeBytes,paletteAsset->paletteBankCount);
  }
  if (outErrorCode != nullptr) {
    *outErrorCode = FATAL_ERROR_PALETTE_ASSET_INVALID;
  }
  return nullptr;
}


/* Returns the allocation that owns a palette asset, which is the asset itself; both release callbacks go
   through this slot. Installed as g_GraphicsPaletteAssetResolveAllocationBase.
*/
GraphicsPaletteAsset * GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset)

{
  return paletteAsset;
}






