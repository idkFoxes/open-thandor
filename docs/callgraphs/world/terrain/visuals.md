# Call graph: `world/terrain/visuals`

11 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053D370` **TerrainCompositeTexture_Create** — local: `TerrainCompositeTexture_FillPlane1`, `TerrainCompositeTexture_FillPlane2`, `TerrainCompositeTexture_RebuildPlane0`
- `0x00503B10` **TerrainByteClampLookup_Initialize**
- `0x00503F30` **TerrainVisualResources_LoadPrimary** — cross: `WidePath_SetExtensionCode` → `core/text/path`, `MoviePlayback_AdvanceScheduledFrameAndTick` → `movie/runtime/playback`, `Package_LoadEntry` → `assets/package/runtime`, `FieldGrid_InitializeRuntimeCellsAndBoundaryFlags` → `world/terrain/grid`, `Random_NextPrimary` → `core/math/random`, `TerrainDirectionTable_AdvanceAndRebuildVectors` → `world/terrain/grid`
- `0x005041C0` **TerrainVisualResources_LoadAndClearCellOverlayFlags** — cross: `WidePath_SetExtensionCode` → `core/text/path`, `MoviePlayback_AdvanceScheduledFrameAndTick` → `movie/runtime/playback`, `Package_LoadEntry` → `assets/package/runtime`, `FieldGrid_RebuildCellLookupPointers` → `world/terrain/grid`, `Random_NextPrimary` → `core/math/random`, `TerrainDirectionTable_AdvanceAndRebuildVectors` → `world/terrain/grid`
- `0x00504470` **TerrainVisualResources_Shutdown** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x00505780` **TerrainLighting_BuildColorRampAndSetBaseColor**
- `0x0053D4D0` **TerrainCompositeTexture_Destroy**
- `0x00561EA0` **TerrainLighting_AdjustDirectionAndRecomputeField** — cross: `WorldRuntime_RecomputeFieldRegionNormalsAndLighting` → `world/runtime/core`
- `0x0053D560` **TerrainCompositeTexture_FillPlane1**
- `0x0053D680` **TerrainCompositeTexture_FillPlane2**
- `0x0053D840` **TerrainCompositeTexture_RebuildPlane0** — cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`, `SelectionInfo_FindEntryCf` → `gameplay/selection/runtime`
