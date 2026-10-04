/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/selection_panel_resources.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/selection_panel_resources.h>
#include <thandor/thandor.h>

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
  return (GraphicsTextureSourceEntry *)
         ((uint8_t *)textureSource + (textureSource->tableDescriptor).subresourceTableOffset);
}

/* Stores a dword at byteOffset inside a texture-source record (records are not aligned). */
static void SelectionInfoPanel_SetRecordDword(uint8_t *record,int byteOffset,uint32_t value)

{
  *(uint32_t *)(record + byteOffset) = value;
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

  base = (uint8_t *)infoTextureSource;
  entries = SelectionInfoPanel_TextureEntries(infoTextureSource);
  referencePayloadValue = *(uint32_t *)(base + entries[44].dataOffset);
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
   info.gfx) - the exact meaning of these patches is not known. Returns true on success; on failure returns false
   with the failing loader's error in *outError.
*/
Bool8 SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots,uint32_t *outError)

{
  GraphicsTextureSourceAsset *selectionTextureSource;
  GraphicsTextureSourceAsset *infoTextureSource;
  GraphicsTextureSourceAsset *selectionPanelData;
  GraphicsTextureSourceAsset *infoPanelData;
  uint32_t loadErrorCode;

  selectionTextureSource =
       g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)g_GfxPanelSelectGfxPathUtf16,&loadErrorCode);
  if (selectionTextureSource == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_SelectionPanelTextureSource = selectionTextureSource;
  infoTextureSource =
       g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)g_GfxPanelInfoGfxPathUtf16,&loadErrorCode);
  if (infoTextureSource == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_InfoPanelTextureSource = infoTextureSource;
  selectionPanelData = (GraphicsTextureSourceAsset *)Package_LoadEntry((uint16_t *)g_GfxPanelSelectDatPathUtf16,&loadErrorCode);
  if (selectionPanelData == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_SelectionPanelData = selectionPanelData;
  infoPanelData = (GraphicsTextureSourceAsset *)Package_LoadEntry((uint16_t *)g_GfxPanelInfoDatPathUtf16,&loadErrorCode);
  if (infoPanelData == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  g_InfoPanelData = infoPanelData;
  SelectionInfoPanel_ClearAllPlayerSelections();
  g_SelectionInfoEntitySlots = entitySlots;
  SelectionInfoPanel_PatchSelectionTexture(g_SelectionPanelTextureSource);
  SelectionInfoPanel_PatchInfoTexture(g_InfoPanelTextureSource);
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
  return;
}
