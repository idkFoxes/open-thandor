/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/texture_set.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Texture sets: creation and destruction through the device slots, package load/release, the set metadata
   and the texture slot registry. */

#include <thandor/graphics/resources/texture_set.h>
#include <thandor/thandor.h>

/* Module data. */

__declspec(align(4)) GraphicsTextureSetLoadPackageProc *g_GraphicsTextureSetLoadPackage = THANDOR_FN(GraphicsTextureSet_LoadPackage);

__declspec(align(16)) GraphicsTextureSetReleasePackageProc *g_GraphicsTextureSetReleasePackage = THANDOR_FN(GraphicsTextureSet_ReleasePackage);

GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureAlpha = THANDOR_FN(GraphicsTextureSet_RefreshNoOp);

GraphicsTextureResource **g_GraphicsTextureSlots = 0;

static GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureColor = THANDOR_FN(GraphicsTextureSet_RefreshNoOp);

GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet = THANDOR_FN(GraphicsTextureSet_AllocateMetadata);

GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet = THANDOR_FN(GraphicsTextureSet_FreeMetadata);

GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures = THANDOR_FN(GraphicsTexture_RebuildNoOp);

/* Creates the renderer textures of a texture asset: allocates the set metadata, then one texture resource per
   subresource, registered in g_GraphicsTextureSlots. Nothing reads the resources any more (they held the
   surfaces of the original's hardware renderers); they are still allocated and registered so the arena layout,
   and with it the texture-set addresses GraphicsPrimitiveQueue_RadixSortForRendering sorts opaque packets by,
   stays as before.
   A subresource whose allocation or registration fails is left NULL; only a failed metadata allocation fails the
   call: then NULL is returned with its error in *outErrorCode (outErrorCode may be NULL). Never NULL on success.
*/
GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode)

{
  GraphicsTextureResource *newTexture;
  Bool8 registerFailed;
  GraphicsTextureSet *allocatedSet;
  uint32_t textureAllocationError;
  GraphicsTextureSetEntry *entryCursor;
  AssetSubresourceCount entriesRemaining;

  allocatedSet = GraphicsTextureSet_AllocateMetadata(sourceAsset,outErrorCode);
  if (allocatedSet == NULL) {
    return NULL;
  }
  entriesRemaining = (allocatedSet->sourceAsset->tableDescriptor).subresourceCount;
  entryCursor = allocatedSet->entries;
  do {
    textureAllocationError = g_MemoryApi.alloc(sizeof(GraphicsTextureResource),(void **)&newTexture);
    if (textureAllocationError == 0) {
      entryCursor->texture = newTexture;
      registerFailed = GraphicsTexture_RegisterSlot(newTexture);
      if (registerFailed) {
        g_MemoryApi.free(newTexture);
        entryCursor->texture = NULL;
      }
    }
    entryCursor++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return allocatedSet;
}

/* Destroys a texture set made by GraphicsTextureSet_Create: every texture resource is removed from
   g_GraphicsTextureSlots and freed, then the set metadata is freed. Returns the source asset the set was
   built from, so the caller can release it too.
*/
GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set)

{
  GraphicsTextureResource *texture;
  GraphicsTextureResource **slotCursor;
  GraphicsTextureSourceAsset *releasedSourceAsset;
  int slotsRemaining;
  uint32_t entriesRemaining;
  GraphicsTextureSetEntry *entryCursor;
  GraphicsTextureResource **matchedSlot;

  releasedSourceAsset = NULL;
  if (set != NULL) {
    entriesRemaining = set->subresourceCount;
    entryCursor = set->entries;
    do {
      texture = entryCursor->texture;
      if (texture != NULL) {
        /* find the texture's slot; if it is not registered the scan ends on (and clears) the last slot */
        slotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
        slotCursor = g_GraphicsTextureSlots;
        do {
          matchedSlot = slotCursor;
          if (texture == *matchedSlot) break;
          slotsRemaining--;
          slotCursor = matchedSlot + 1;
        } while (slotsRemaining != 0);
        *matchedSlot = NULL;
        g_MemoryApi.free(texture);
      }
      entryCursor++;
      entriesRemaining--;
    } while (entriesRemaining != 0);
    releasedSourceAsset = GraphicsTextureSet_FreeMetadata(set);
  }
  return releasedSourceAsset;
}

/* Loads a 'gfx' texture source from the package and builds a renderer texture set from it through
   g_GraphicsCreateTextureSet (installed as g_GraphicsTextureSetLoadPackage). When the set cannot be created the
   loaded asset is released again. Returns the set (never NULL), or NULL with the load or creation error in
   *outErrorCode (outErrorCode may be NULL).
*/
GraphicsTextureSet * GraphicsTextureSet_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode)

{
  GraphicsTextureSourceAsset *loadedSource;
  GraphicsTextureSet *createdSet;
  uint32_t errorCode;

  loadedSource = (GraphicsTextureSourceAsset *)Package_LoadEntry(pathUtf16,&errorCode);
  if (loadedSource != NULL) {
    createdSet = g_GraphicsCreateTextureSet(loadedSource,&errorCode);
    if (createdSet != NULL) {
      return createdSet;
    }
    Resource_Release(loadedSource);
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return NULL;
}

/* Counterpart of GraphicsTextureSet_LoadPackage (installed as g_GraphicsTextureSetReleasePackage): destroys the
   texture set through g_GraphicsDestroyTextureSet and releases the 'gfx' source asset that call hands back.
*/
void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *sourceAsset;

  sourceAsset = g_GraphicsDestroyTextureSet(set);
  Resource_Release(sourceAsset);
  return;
}

/* g_GraphicsRefreshTextureColor and g_GraphicsRefreshTextureAlpha: the software renderer reads the source pixels
   directly, so there is nothing to re-upload after they changed (the original's hardware renderers installed
   their own re-uploads here).
*/
void GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set)

{
  return;
}

/* g_GraphicsRebuildAllStagingTextures: the software renderer keeps no device textures, so there is nothing to
   rebuild after a display mode or texture detail change (the original's hardware renderers installed their own
   rebuild here).
*/
void __cdecl GraphicsTexture_RebuildNoOp(void)

{
  return;
}

/* GraphicsTextureSet_AllocateMetadata: fills set->entries from the asset's source entries (rows
   GFX_SUBRESOURCE_RECORD_SIZE bytes apart, starting at firstSourceEntry), entryCount of them; at least one
   entry is always processed, as in the original. Each entry gets no texture yet, its image index, the source
   asset and entry, and log2 of the width and height (31 for a zero size). Returns false at the first image
   whose width or height is not a power of two (that entry is left partly written). */
static Bool8 GraphicsTextureSet_FillEntries
          (GraphicsTextureSet *set,GraphicsTextureSourceAsset *sourceAsset,uint8_t *firstSourceEntry,
           GraphicsAssetAllocationByteSize entryCount)
{
  GraphicsTextureSetEntry *entry = set->entries;
  GraphicsTextureSourceEntry *sourceEntry = (GraphicsTextureSourceEntry *)firstSourceEntry;
  GraphicsAssetAllocationByteSize entriesRemaining = entryCount;
  int entryIndex = 0;
  uint32_t widthLog2;
  int heightLog2;

  do {
    /* index of the highest set bit of pixelWidth; the original leaves the register undefined for 0 */
    widthLog2 = 31;
    if (sourceEntry->pixelWidth != 0) {
      while (sourceEntry->pixelWidth >> widthLog2 == 0) {
        widthLog2--;
      }
    }
    entry->texture = NULL;
    entry->subresourceIndex = entryIndex;
    entry->widthLog2 = widthLog2;
    if (1 << ((uint8_t)widthLog2 & SHIFT_COUNT_MASK) != sourceEntry->pixelWidth) {
      return false;
    }
    entry->sourceAsset = sourceAsset;
    /* index of the highest set bit of pixelHeight */
    heightLog2 = 31;
    if (sourceEntry->pixelHeight != 0) {
      while (sourceEntry->pixelHeight >> heightLog2 == 0) {
        heightLog2--;
      }
    }
    entry->sourceEntry = sourceEntry;
    entry->heightLog2 = heightLog2;
    if (1 << ((uint8_t)heightLog2 & SHIFT_COUNT_MASK) != sourceEntry->pixelHeight) {
      return false;
    }
    entry++;
    sourceEntry = (GraphicsTextureSourceEntry *)((uint8_t *)sourceEntry + GFX_SUBRESOURCE_RECORD_SIZE);
    entryIndex++;
    entriesRemaining--;
  } while (entriesRemaining != 0);
  return true;
}

/* Builds a texture set for a 'gfx' asset: converts its palettes to the display format, then allocates the
   set (an 8-byte header with the source asset and image count, then one 0x20-byte GraphicsTextureSetEntry
   per image) and fills each entry with the image index, source entry and log2 of its width and height.
   Returns the set (never NULL), or NULL with the conversion/arena error or FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO
   in *outErrorCode (outErrorCode may be NULL; on the size error the set is not freed, as in the original).
*/
GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode)

{
  GraphicsPaletteTextureSourceAsset *convertedSource;
  GraphicsTextureSet *set;
  uint32_t errorCode;
  GraphicsAssetAllocationByteSize entryCount;

  convertedSource = (GraphicsPaletteTextureSourceAsset *)sourceAsset;
  errorCode = g_GraphicsTextureSourceConvertPaletteEntries(convertedSource);
  if (errorCode == 0) {
    entryCount = convertedSource->subresourceCount;
    errorCode = g_MemoryApi.alloc(entryCount * GRAPHICS_TEXTURE_SET_ENTRY_BYTES + GRAPHICS_TEXTURE_SET_HEADER_BYTES,(void **)&set);
    if (errorCode == 0) {
      set->sourceAsset = sourceAsset;
      set->subresourceCount = entryCount;
      if (GraphicsTextureSet_FillEntries
               (set,sourceAsset,(uint8_t *)convertedSource + convertedSource->subresourceTableOffset,entryCount)) {
        return set;
      }
      errorCode = FATAL_ERROR_TEXTURE_SIZE_NOT_POWER_OF_TWO;
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return NULL;
}

/* Counterpart of GraphicsTextureSet_AllocateMetadata: frees the set and returns its source asset so the
   caller can release that too. NULL for a NULL set.
*/
GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set)

{
  GraphicsTextureSourceAsset *sourceAsset;

  sourceAsset = NULL;
  if (set != NULL) {
    sourceAsset = set->sourceAsset;
    g_MemoryApi.free(set);
  }
  return sourceAsset;
}

/* Enters a texture into the first free slot of g_GraphicsTextureSlots (the registry the original used to evict
   and rebuild device textures). Returns true when all GRAPHICS_TEXTURE_SLOT_CAPACITY slots are taken.
*/
Bool8 GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture)

{
  int slotsRemaining;
  GraphicsTextureResource **slotCursor;
  
  slotsRemaining = GRAPHICS_TEXTURE_SLOT_CAPACITY;
  slotCursor = g_GraphicsTextureSlots;
  do {
    if (*slotCursor == NULL) {
      *slotCursor = texture;
      return false;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return true;
}
