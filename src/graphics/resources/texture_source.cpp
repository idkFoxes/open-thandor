/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture_source.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/texture.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(16) GraphicsTextureSourceLifecycleCallbackTable g_GraphicsTextureSourceLifecycleCallbacks3 = {
    .releasePackage = THANDOR_SLOT(GraphicsTextureSource_ReleasePackageAsset),
    .clone = THANDOR_SLOT(GraphicsTextureSource_CloneAsset),
    .releaseClone = THANDOR_SLOT(GraphicsTextureSource_ReleaseClonedAsset)};

THANDOR_ALIGN(4) GraphicsTextureSourceLoadPackageAssetProc *g_GraphicsTextureSourceLoadPackageAsset = &GraphicsTextureSource_LoadPackageAsset;

GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitHalfSourceRgb = nullptr;

GraphicsTextureSourceStretchDirectColorBilinearProc *g_GraphicsTextureSourceStretchDirectColorBilinear = nullptr;

GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries = &GraphicsTextureSource_ConvertPaletteEntries;

GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase = &GraphicsTextureSource_ResolveAllocationBase;

GraphicsTextureSourceGetLogicalSizeProc *g_GraphicsTextureSourceGetLogicalSize = &GraphicsTextureSource_GetLogicalSize;

GraphicsTextureSourceTestOpaquePixelProc *g_GraphicsTextureSourceTestOpaquePixel = &GraphicsTextureSource_TestOpaquePixel;

GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitSourceAlpha = nullptr;

GraphicsTextureSourceBlitModulatedSourceAlphaProc *g_GraphicsTextureSourceBlitModulatedSourceAlpha = nullptr;






/* Returns the logical width and height of one subresource of a 'gfx' texture source, i.e. the extent the
   tiled blits repeat (installed as g_GraphicsTextureSourceGetLogicalSize), or 0 x 0 when the asset is not a
   'gfx' asset or the index is out of range (the original only signalled failure and returned no size; the
   only callers that test it, the glyph size queries, use width 0 then).
*/
GraphicsTextureLogicalSize GraphicsTextureSource_GetLogicalSize
          (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureLogicalSize size;
  GraphicsTextureSourceEntry *entry;

  size.logicalWidthPixels = 0;
  size.logicalHeightPixels = 0;
  if (((sourceAsset->common).magic == ASSET_MAGIC_GFX) &&
     (subresourceIndex < (sourceAsset->tableDescriptor).subresourceCount)) {
    entry = (GraphicsTextureSourceEntry *)((uint8_t *)sourceAsset + subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE +
                                           (sourceAsset->tableDescriptor).subresourceTableOffset);
    size.logicalWidthPixels = entry->logicalWidth;
    size.logicalHeightPixels = entry->logicalHeight;
  }
  return size;
}


/* Hit test of a sprite drawn at (drawX, drawY) (installed as g_GraphicsTextureSourceTestOpaquePixel): maps the
   query point into the stored pixels of the subresource and returns true when that pixel has a non-zero
   alpha, for direct ARGB and paletted subresources alike. Returns false for transparent pixels, points
   outside the stored pixels and invalid input.
*/
Bool8 GraphicsTextureSource_TestOpaquePixel(GraphicsScreenCoordinate queryY,GraphicsScreenCoordinate queryX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset)

{
  const GraphicsTextureSourceEntry *entry;
  const uint8_t *pixels;
  int recordOffset;
  int paletteIndex;
  int localX;
  int localY;
  int pixelIndex;

  if ((queryX < drawX) || (queryY < drawY)) {
    return false;
  }
  if (((sourceAsset->common).magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= (sourceAsset->tableDescriptor).subresourceCount)) {
    return false;
  }
  recordOffset = subresourceIndex * GFX_SUBRESOURCE_RECORD_SIZE;
  entry = (const GraphicsTextureSourceEntry *)
          ((uint8_t *)sourceAsset + recordOffset + (sourceAsset->tableDescriptor).subresourceTableOffset);
  /* query point relative to the stored pixels, which start at (originX, originY) of the sprite */
  localX = (queryX - drawX) - entry->originX;
  if (entry->originX > queryX - drawX) {
    return false;
  }
  localY = (queryY - drawY) - entry->originY;
  if (entry->originY > queryY - drawY) {
    return false;
  }
  if ((localX >= (int)entry->pixelWidth) || (localY >= (int)entry->pixelHeight)) {
    return false;
  }
  paletteIndex = entry->paletteIndex;
  pixelIndex = localY * entry->pixelWidth + localX;
  pixels = (const uint8_t *)sourceAsset + entry->dataOffset;
  /* opaque = any alpha bit set in the ARGB8888 pixel (direct) or palette entry (paletted, 8 bytes each in
     the 256-entry bank at asset + 0x200 + paletteIndex * 0x800) */
  if (paletteIndex == -1) {
    return ARGB8888_RGB_MASK < ((const uint32_t *)pixels)[pixelIndex];
  }
  return ARGB8888_RGB_MASK <
         ((GraphicsPaletteTextureSourceAsset *)sourceAsset)->paletteEntries
                    [(int32_t)(paletteIndex * GRAPHICS_PALETTE_BANK_ENTRIES + (uint32_t)pixels[pixelIndex])].argb8888;
}






/* Checks a 'gfx' texture source header against its allocation size (common.allocationSizeBytes): the palette
   banks, the subresource table and every record's pixels (pixelWidth * pixelHeight bytes, 4 per pixel for
   direct colour) lie inside it, and every paletted record names an existing bank. Returns false (and logs one
   line) for a NULL, non-'gfx' or malformed asset. Every stock asset and every asset the game builds in memory
   passes.
   The original used the header unchecked; checked here because a malformed package entry made the palette
   conversion, the blits, the rasterizer and the hit test read or write outside the asset. */
Bool8 GraphicsTextureSource_ValidateAsset(const GraphicsTextureSourceAsset *sourceAsset)

{
  const GraphicsTextureSourceEntry *entry;
  uint64_t byteSize;
  uint64_t pixelBytes;
  uint32_t paletteBankCount;
  uint32_t subresourceCount;
  uint32_t entryIndex;

  if (sourceAsset == nullptr || (sourceAsset->common).magic != ASSET_MAGIC_GFX) {
    return false;
  }
  byteSize = (sourceAsset->common).allocationSizeBytes;
  paletteBankCount = (sourceAsset->tableDescriptor).paletteBankCount;
  subresourceCount = (sourceAsset->tableDescriptor).subresourceCount;
  if (byteSize < GFX_ASSET_HEADER_SIZE ||
      GFX_ASSET_HEADER_SIZE + (uint64_t)paletteBankCount * GFX_PALETTE_BANK_SIZE > byteSize ||
      (uint64_t)(sourceAsset->tableDescriptor).subresourceTableOffset +
              (uint64_t)subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE > byteSize) {
    Thandor_Log("GraphicsTextureSource_ValidateAsset: rejected gfx header (%u bytes, %u banks, %u images at 0x%X)",
                (uint32_t)byteSize,paletteBankCount,subresourceCount,
                (sourceAsset->tableDescriptor).subresourceTableOffset);
    return false;
  }
  entry = (const GraphicsTextureSourceEntry *)
          ((const uint8_t *)sourceAsset + (sourceAsset->tableDescriptor).subresourceTableOffset);
  for (entryIndex = 0; entryIndex < subresourceCount; entryIndex++, entry++) {
    pixelBytes = (uint64_t)entry->pixelWidth * entry->pixelHeight;
    if (entry->paletteIndex == -1) {
      pixelBytes = pixelBytes * 4;
    }
    else if ((uint32_t)entry->paletteIndex >= paletteBankCount) {
      pixelBytes = UINT64_MAX;
    }
    if (pixelBytes > byteSize || (uint64_t)entry->dataOffset + pixelBytes > byteSize) {
      Thandor_Log("GraphicsTextureSource_ValidateAsset: rejected image %u (palette %d of %u, %ux%u at 0x%X, %u bytes)",
                  entryIndex,entry->paletteIndex,paletteBankCount,entry->pixelWidth,entry->pixelHeight,
                  entry->dataOffset,(uint32_t)byteSize);
      return false;
    }
  }
  return true;
}


/* Loads a 'gfx' texture source for the software renderer (installed as g_GraphicsTextureSourceLoadPackageAsset;
   used for the UI, text and selection-panel graphics): the package entry is loaded, its header is checked
   (GraphicsTextureSource_ValidateAsset) and its palettes are converted to the current framebuffer format.
   Returns the texture source (never NULL: the conversion rejects NULL).
   If the check or the conversion fails the entry is released again; on failure returns NULL and stores the
   load or conversion error (FATAL_ERROR_GFX_ASSET_INVALID for a malformed header) in *outError (when outError
   is not NULL).
*/
GraphicsTextureSourceAsset *GraphicsTextureSource_LoadPackageAsset(uint16_t *pathUtf16,uint32_t *outError)

{
  GraphicsPaletteTextureSourceAsset *loadedSource;
  uint32_t loadError;

  loadedSource = (GraphicsPaletteTextureSourceAsset *)Package_LoadEntry(pathUtf16,&loadError);
  if (loadedSource != nullptr) {
    loadError = FATAL_ERROR_GFX_ASSET_INVALID;
    if (GraphicsTextureSource_ValidateAsset((GraphicsTextureSourceAsset *)loadedSource)) {
      loadError = g_GraphicsTextureSourceConvertPaletteEntries(loadedSource);
    }
    if (loadError == 0) {
      return (GraphicsTextureSourceAsset *)loadedSource;
    }
    Resource_Release(loadedSource);
  }
  if (outError != nullptr) {
    *outError = loadError;
  }
  return nullptr;
}


/* Makes a private heap copy of a 'gfx' texture source with its palettes converted to the current framebuffer
   format, so it can be modified independently (g_GraphicsTextureSourceLifecycleCallbacks3.clone). Returns the
   copy; on failure it returns the allocation error cast to a pointer, or after a failed conversion the
   result of freeing the copy again (as the original, which also flagged the failure separately).
*/
GraphicsTextureSourceAsset *
GraphicsTextureSource_CloneAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsPaletteTextureSourceAsset *clonedAsset;
  uint32_t allocationSizeBytes;
  uint32_t dwordsRemaining;
  const uint32_t *sourceDword;
  uint32_t *cloneDword;
  uint32_t cloneAllocationError;

  allocationSizeBytes = (sourceAsset->common).allocationSizeBytes;
  cloneAllocationError = g_MemoryApi.alloc(allocationSizeBytes,(void **)&clonedAsset);
  if (cloneAllocationError != 0) {
    return (GraphicsTextureSourceAsset *)(uintptr_t)cloneAllocationError;
  }
  /* copy the whole allocation dword by dword (a trailing partial dword is not copied) */
  sourceDword = (const uint32_t *)sourceAsset;
  cloneDword = (uint32_t *)clonedAsset;
  for (dwordsRemaining = allocationSizeBytes >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *cloneDword = *sourceDword;
    sourceDword++;
    cloneDword++;
  }
  if (g_GraphicsTextureSourceConvertPaletteEntries(clonedAsset) == 0) {
    return (GraphicsTextureSourceAsset *)clonedAsset;
  }
  /* Original quirk: the clone's result after a failed conversion is the free's status (0 = NULL) */
  return (GraphicsTextureSourceAsset *)(uintptr_t)g_MemoryApi.free(clonedAsset);
}


/* Fills the framebuffer-pixel half of every palette entry of a 'gfx' texture source from its ARGB8888 half,
   packed for the current framebuffer format through g_SoftwarePixelPackTables (alpha is kept in the top byte),
   so the software blits can copy palette colours directly (installed as
   g_GraphicsTextureSourceConvertPaletteEntries). Returns 0 on success, FATAL_ERROR_GFX_ASSET_INVALID for a
   NULL or non-'gfx' asset (the asset itself is the success value of the original).
*/
uint32_t GraphicsTextureSource_ConvertPaletteEntries(GraphicsPaletteTextureSourceAsset *sourceAsset)

{
  int paletteEntriesRemaining;
  GraphicsTexturePaletteEntry *paletteEntryCursor;
  uint32_t argb8888;
  
  if ((sourceAsset != nullptr) &&
     (sourceAsset->magic == GRAPHICS_PALETTE_TEXTURE_MAGIC_GFX)) {
    paletteEntryCursor = sourceAsset->paletteEntries;
    for (paletteEntriesRemaining = sourceAsset->paletteBankCount << 8; paletteEntriesRemaining != 0;
        paletteEntriesRemaining--) {
      argb8888 = paletteEntryCursor->argb8888;
      /* the red and green shifts yield byte offsets into the dword tables (channel value * 4) */
      paletteEntryCursor->framebufferPixel =
           (argb8888 & ARGB8888_ALPHA_MASK) +
           *(int *)((uintptr_t)g_SoftwarePixelPackTables->red + ((argb8888 & ARGB8888_RED_MASK) >> 14)) +
           *(int *)((uintptr_t)g_SoftwarePixelPackTables->green + ((argb8888 & ARGB8888_GREEN_MASK) >> 6)) +
           g_SoftwarePixelPackTables->blue[argb8888 & ARGB8888_BLUE_MASK];
      paletteEntryCursor++;
    }
    return 0;
  }
  return FATAL_ERROR_GFX_ASSET_INVALID;
}


/* Releases a texture source loaded by GraphicsTextureSource_LoadPackageAsset back to the resource cache
   (g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage).
*/
void GraphicsTextureSource_ReleasePackageAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;

  allocation = g_GraphicsTextureSourceResolveAllocationBase(sourceAsset);
  Resource_Release(allocation);
}


/* Frees a copy made by GraphicsTextureSource_CloneAsset (g_GraphicsTextureSourceLifecycleCallbacks3.releaseClone).
*/
void GraphicsTextureSource_ReleaseClonedAsset(GraphicsTextureSourceAsset *sourceAsset)

{
  GraphicsTextureSourceAsset *allocation;

  allocation = g_GraphicsTextureSourceResolveAllocationBase(sourceAsset);
  g_MemoryApi.free(allocation);
}


/* Returns the allocation that holds a texture source (installed as g_GraphicsTextureSourceResolveAllocationBase);
   the asset is its own allocation, but both release callbacks ask this slot first.
*/
GraphicsTextureSourceAsset * GraphicsTextureSource_ResolveAllocationBase(GraphicsTextureSourceAsset *sourceAsset)

{
  return sourceAsset;
}




