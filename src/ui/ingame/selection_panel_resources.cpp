/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/selection_panel_resources.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/selection_panel_resources.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* L"gfx\\panel\\select.gfx" */
static uint16_t g_GfxPanelSelectGfxPathUtf16[21] =
    {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 's', 'e', 'l', 'e', 'c', 't', '.', 'g', 'f', 'x', 0};

/* L"gfx\\panel\\info.gfx" */
static uint16_t g_GfxPanelInfoGfxPathUtf16[19] =
    {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'i', 'n', 'f', 'o', '.', 'g', 'f', 'x', 0};

/* L"gfx\\panel\\select.dat" */
static uint16_t g_GfxPanelSelectDatPathUtf16[21] =
    {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 's', 'e', 'l', 'e', 'c', 't', '.', 'd', 'a', 't', 0};

/* L"gfx\\panel\\info.dat" */
static uint16_t g_GfxPanelInfoDatPathUtf16[19] =
    {'g', 'f', 'x', '\\', 'p', 'a', 'n', 'e', 'l', '\\', 'i', 'n', 'f', 'o', '.', 'd', 'a', 't', 0};

GraphicsTextureSourceAsset *g_SelectionPanelTextureSource = nullptr;

GraphicsTextureSourceAsset *g_InfoPanelTextureSource = nullptr;

void *g_SelectionPanelData = nullptr;

void *g_InfoPanelData = nullptr;

/* Empties the 32 selection entries of all eight player blocks. */
static void SelectionInfoPanel_ClearAllPlayerSelections()

{
  int blockIndex;
  int entryIndex;

  for (blockIndex = 0; blockIndex < 8; blockIndex++) {
    for (entryIndex = 0; entryIndex < SELECTION_ENTRY_CAPACITY; entryIndex++) {
      g_SelectionPlayerBlocks[blockIndex].selection.entries[entryIndex] = nullptr;
    }
  }
}

/* The subresource entry table of a loaded texture source. */
static GraphicsTextureSourceEntry *SelectionInfoPanel_TextureEntries(GraphicsTextureSourceAsset *textureSource)

{
  return GraphicsTextureSource_Entries(textureSource);
}

/* Stores a dword at byteOffset inside a texture-source record (records are not aligned). */
static void SelectionInfoPanel_SetRecordDword(uint8_t *record,int byteOffset,uint32_t value)

{
  *Thandor_At<uint32_t>(record,byteOffset) = value;
}

/* Rewrites a two-dword info.gfx record to { 0, referencePayloadValue }. */
static void SelectionInfoPanel_PatchShortRecord(uint8_t *record,uint32_t referencePayloadValue)

{
  SelectionInfoPanel_SetRecordDword(record,0,0);
  SelectionInfoPanel_SetRecordDword(record,4,referencePayloadValue);
}

/* Rewrites an eight-dword info.gfx record to { first, second, 0xFFFF0000 x4, first, second }. */
static void SelectionInfoPanel_PatchLongRecord(uint8_t *record,uint32_t first,uint32_t second)

{
  int byteOffset;

  SelectionInfoPanel_SetRecordDword(record,0,first);
  SelectionInfoPanel_SetRecordDword(record,4,second);
  for (byteOffset = 8; byteOffset <= 20; byteOffset += 4) {
    SelectionInfoPanel_SetRecordDword(record,byteOffset,0xFFFF0000u);
  }
  SelectionInfoPanel_SetRecordDword(record,24,first);
  SelectionInfoPanel_SetRecordDword(record,28,second);
}

/* Both patches touch subresources up to 51 (0x33). */
inline constexpr uint32_t SELECTION_INFO_PANEL_PATCHED_SUBRESOURCE_COUNT = 52;
/* Sizes of the rewritten info.gfx records and of the dword read from the record of subresource 44. */
inline constexpr uint64_t SELECTION_INFO_PANEL_SHORT_RECORD_BYTES = 8;
inline constexpr uint64_t SELECTION_INFO_PANEL_LONG_RECORD_BYTES = 32;
inline constexpr uint64_t SELECTION_INFO_PANEL_REFERENCE_DWORD_BYTES = 4;
/* pixelHeight / logicalHeight the info.gfx patch sets on subresources 45, 46, 50 and 51. */
inline constexpr uint32_t SELECTION_INFO_PANEL_PATCHED_HEIGHT = 4;

/* True when byteCount bytes at byteOffset lie inside the allocation of the texture source. */
static bool SelectionInfoPanel_RangeInside(const GraphicsTextureSourceAsset *textureSource,uint64_t byteOffset,
                                           uint64_t byteCount)

{
  return byteOffset + byteCount <= (textureSource->common).allocationSizeBytes;
}

/* True when the pixels of entry, read with pixelHeight and dataOffset as given, lie inside the allocation (the
   same rule as GraphicsTextureSource_ValidateAsset: 4 bytes per pixel for direct colour, else 1). */
static bool SelectionInfoPanel_PixelsInside(const GraphicsTextureSourceAsset *textureSource,
                                            const GraphicsTextureSourceEntry &entry,uint32_t pixelHeight,
                                            AssetRelativeOffset dataOffset)

{
  uint64_t pixelBytes;

  pixelBytes = static_cast<uint64_t>(entry.pixelWidth) * pixelHeight;
  if (entry.paletteIndex == -1) {
    pixelBytes = pixelBytes * 4;
  }
  return SelectionInfoPanel_RangeInside(textureSource,dataOffset,pixelBytes);
}

/* select.gfx patch precondition: subresources 45 and 46 exist and each one's pixels stay inside the allocation
   after the data offsets are swapped. Every stock select.gfx passes (179 subresources, 45 and 46 are both 4x9). */
static bool SelectionInfoPanel_CanPatchSelectionTexture(GraphicsTextureSourceAsset *selectionTextureSource)

{
  const GraphicsTextureSourceEntry *entries;

  if ((selectionTextureSource->tableDescriptor).subresourceCount < SELECTION_INFO_PANEL_PATCHED_SUBRESOURCE_COUNT) {
    return false;
  }
  entries = SelectionInfoPanel_TextureEntries(selectionTextureSource);
  return SelectionInfoPanel_PixelsInside(selectionTextureSource,entries[45],entries[45].pixelHeight,
                                         entries[46].dataOffset) &&
         SelectionInfoPanel_PixelsInside(selectionTextureSource,entries[46],entries[46].pixelHeight,
                                         entries[45].dataOffset);
}

/* info.gfx patch precondition: subresources 0..51 exist, the dword read at subresource 44, the short records of
   47..49 and the long records of 45, 46, 50 and 51 lie inside the allocation, and so do the pixels of 45, 46, 50
   and 51 at their new height of 4. Every stock info.gfx passes (179 subresources, all patched records inside). */
static bool SelectionInfoPanel_CanPatchInfoTexture(GraphicsTextureSourceAsset *infoTextureSource)

{
  static constexpr int shortRecordIndices[] = {47,48,49};
  static constexpr int longRecordIndices[] = {45,46,50,51};
  const GraphicsTextureSourceEntry *entries;

  if ((infoTextureSource->tableDescriptor).subresourceCount < SELECTION_INFO_PANEL_PATCHED_SUBRESOURCE_COUNT) {
    return false;
  }
  entries = SelectionInfoPanel_TextureEntries(infoTextureSource);
  if (!SelectionInfoPanel_RangeInside(infoTextureSource,entries[44].dataOffset,
                                      SELECTION_INFO_PANEL_REFERENCE_DWORD_BYTES)) {
    return false;
  }
  for (const int index : shortRecordIndices) {
    if (!SelectionInfoPanel_RangeInside(infoTextureSource,entries[index].dataOffset,
                                        SELECTION_INFO_PANEL_SHORT_RECORD_BYTES)) {
      return false;
    }
  }
  for (const int index : longRecordIndices) {
    if (!SelectionInfoPanel_RangeInside(infoTextureSource,entries[index].dataOffset,
                                        SELECTION_INFO_PANEL_LONG_RECORD_BYTES) ||
        !SelectionInfoPanel_PixelsInside(infoTextureSource,entries[index],SELECTION_INFO_PANEL_PATCHED_HEIGHT,
                                         entries[index].dataOffset)) {
      return false;
    }
  }
  return true;
}

/* select.gfx: swaps the data offsets of subresources 45 and 46. */
static void SelectionInfoPanel_PatchSelectionTexture(GraphicsTextureSourceAsset *selectionTextureSource)

{
  GraphicsTextureSourceEntry *entries;
  AssetRelativeOffset swappedDataOffset;

  entries = SelectionInfoPanel_TextureEntries(selectionTextureSource);
  swappedDataOffset = entries[46].dataOffset;
  entries[46].dataOffset = entries[45].dataOffset;
  entries[45].dataOffset = swappedDataOffset;
}

/* info.gfx: rewrites the records of several sequences (the records at the dataOffset of subresources
   0x2D..0x33; their layout is not typed) and sets the heights of subresources 45, 46, 50 and 51 to 4.
   referencePayloadValue is the first dword of the record of subresource 0x2C. */
static void SelectionInfoPanel_PatchInfoTexture(GraphicsTextureSourceAsset *infoTextureSource)

{
  uint8_t *base;
  GraphicsTextureSourceEntry *entries;
  uint32_t referencePayloadValue;

  base = GraphicsTextureSource_Bytes(infoTextureSource);
  entries = SelectionInfoPanel_TextureEntries(infoTextureSource);
  referencePayloadValue = *Thandor_At<uint32_t>(base,entries[44].dataOffset);
  SelectionInfoPanel_PatchShortRecord(base + entries[47].dataOffset,referencePayloadValue);
  SelectionInfoPanel_PatchShortRecord(base + entries[48].dataOffset,referencePayloadValue);
  SelectionInfoPanel_PatchShortRecord(base + entries[49].dataOffset,referencePayloadValue);
  entries[50].pixelHeight = 4;
  entries[50].logicalHeight = 4;
  entries[51].pixelHeight = 4;
  entries[51].logicalHeight = 4;
  entries[45].pixelHeight = 4;
  entries[45].logicalHeight = 4;
  entries[46].pixelHeight = 4;
  entries[46].logicalHeight = 4;
  SelectionInfoPanel_PatchLongRecord(base + entries[50].dataOffset,0,referencePayloadValue);
  SelectionInfoPanel_PatchLongRecord(base + entries[51].dataOffset,0,referencePayloadValue);
  SelectionInfoPanel_PatchLongRecord(base + entries[45].dataOffset,referencePayloadValue,0);
  SelectionInfoPanel_PatchLongRecord(base + entries[46].dataOffset,referencePayloadValue,0);
}

/* Loads the selection and information panel graphics (gfx\panel\select.gfx, info.gfx) and their 0x1A4-byte .dat
   tables, empties the selection of all eight player blocks and stores the caller's entity-slot table. Then it
   patches sequence descriptors inside the loaded textures (swaps two select.gfx entries, rewrites frames of
   info.gfx) - the exact meaning of these patches is not known. A texture with fewer than 52 subresources or a
   patched record outside its allocation is left unpatched with one log line (the panel still loads). Returns
   true on success; on failure returns false with the failing loader's error in *outError.
*/
bool SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots,uint32_t *outError)

{
  GraphicsTextureSourceAsset *selectionTextureSource;
  GraphicsTextureSourceAsset *infoTextureSource;
  GraphicsTextureSourceAsset *selectionPanelData;
  GraphicsTextureSourceAsset *infoPanelData;
  uint32_t loadErrorCode;

  selectionTextureSource =
       g_GraphicsTextureSourceLoadPackageAsset(g_GfxPanelSelectGfxPathUtf16,&loadErrorCode);
  if (selectionTextureSource == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_SelectionPanelTextureSource = selectionTextureSource;
  infoTextureSource =
       g_GraphicsTextureSourceLoadPackageAsset(g_GfxPanelInfoGfxPathUtf16,&loadErrorCode);
  if (infoTextureSource == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_InfoPanelTextureSource = infoTextureSource;
  selectionPanelData = static_cast<GraphicsTextureSourceAsset *>(Package_LoadEntry(g_GfxPanelSelectDatPathUtf16,&loadErrorCode));
  if (selectionPanelData == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_SelectionPanelData = selectionPanelData;
  infoPanelData = static_cast<GraphicsTextureSourceAsset *>(Package_LoadEntry(g_GfxPanelInfoDatPathUtf16,&loadErrorCode));
  if (infoPanelData == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_InfoPanelData = infoPanelData;
  SelectionInfoPanel_ClearAllPlayerSelections();
  g_SelectionInfoEntitySlots = entitySlots;
  /* The original patched both textures unchecked; checked here because a select.gfx / info.gfx with fewer than
     52 subresources or with a patched record outside the allocation made the patches read and write outside
     the asset. Such a texture is left unpatched (one log line) and the panel loads and draws it as it is. */
  if (SelectionInfoPanel_CanPatchSelectionTexture(g_SelectionPanelTextureSource)) {
    SelectionInfoPanel_PatchSelectionTexture(g_SelectionPanelTextureSource);
  }
  else {
    Thandor_Log("SelectionInfoPanel_InitResources: select.gfx left unpatched (%u subresources, %u bytes)",
                (g_SelectionPanelTextureSource->tableDescriptor).subresourceCount,
                (g_SelectionPanelTextureSource->common).allocationSizeBytes);
  }
  if (SelectionInfoPanel_CanPatchInfoTexture(g_InfoPanelTextureSource)) {
    SelectionInfoPanel_PatchInfoTexture(g_InfoPanelTextureSource);
  }
  else {
    Thandor_Log("SelectionInfoPanel_InitResources: info.gfx left unpatched (%u subresources, %u bytes)",
                (g_InfoPanelTextureSource->tableDescriptor).subresourceCount,
                (g_InfoPanelTextureSource->common).allocationSizeBytes);
  }
  return true;
}

/* Counterpart of SelectionInfoPanel_InitResources: releases both panel textures and both .dat tables and clears
   the four resource pointers.
*/
void SelectionInfoPanel_ShutdownResources()

{
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_SelectionPanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InfoPanelTextureSource);
  Resource_Release(g_SelectionPanelData);
  Resource_Release(g_InfoPanelData);
  g_SelectionPanelTextureSource = nullptr;
  g_InfoPanelTextureSource = nullptr;
  g_SelectionPanelData = nullptr;
  g_InfoPanelData = nullptr;
}
