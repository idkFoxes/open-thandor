# Call graph: `world/terrain/editing`

[Source](../../../../src/world/terrain/editing.c) · [Header](../../../../include/thandor/world/terrain/editing.h) · [Graphviz](editing.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-world-terrain-editing)

11 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005137F0` **[`TerrainRegionCollection_CollectConnectedCellsRecursive`](../../../../src/world/terrain/editing.c#L5)** — local: [`TerrainRegionCollection_RecordConnectedCell`](../../../../src/world/terrain/editing.c#L385)
- `0x00561A10` **[`TerrainMaterialEdit_SeedMatchingRegionReplacement`](../../../../src/world/terrain/editing.c#L79)** — local: [`TerrainMaterialEdit_PropagateMatchingRegionReplacement`](../../../../src/world/terrain/editing.c#L427)
- `0x00561AE0` **[`TerrainMaterialEdit_SeedNonTargetRegionReplacement`](../../../../src/world/terrain/editing.c#L126)** — local: [`TerrainMaterialEdit_PropagateNonTargetRegionReplacement`](../../../../src/world/terrain/editing.c#L508)
- `0x005616D0` **[`TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting`](../../../../src/world/terrain/editing.c#L170)** — cross: [`FieldGridCell_RecomputeTriangleNormalAngles`](../../../../src/world/terrain/grid.c#L3557) → [`world/terrain/grid`](grid.md), [`FieldGridCell_ComputeDirectionalLightColor`](../../../../src/world/terrain/grid.c#L3632) → [`world/terrain/grid`](grid.md)
- `0x00561830` **[`TerrainEditBuffer_CopyCellMaterialBytes`](../../../../src/world/terrain/editing.c#L269)**
- `0x00561930` **[`TerrainEditBuffer_SubtractCurrentCellMaterialBytes`](../../../../src/world/terrain/editing.c#L299)**
- `0x005619A0` **[`TerrainEditBuffer_CommitFlagsAndMaterialDeltas`](../../../../src/world/terrain/editing.c#L327)**
- `0x00561DC0` **[`TerrainEditBuffer_ConvertHeightsToDeltas`](../../../../src/world/terrain/editing.c#L356)**
- `0x00513790` **[`TerrainRegionCollection_RecordConnectedCell`](../../../../src/world/terrain/editing.c#L385)**
- `0x00571600` **[`TerrainMaterialEdit_PropagateMatchingRegionReplacement`](../../../../src/world/terrain/editing.c#L427)**
- `0x00571730` **[`TerrainMaterialEdit_PropagateNonTargetRegionReplacement`](../../../../src/world/terrain/editing.c#L508)**

## Called by

- [`gameplay/session/runtime`](../../gameplay/session/runtime.md): [`InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState`](../../../../src/gameplay/session/runtime.c#L1826) → [`TerrainRegionCollection_CollectConnectedCellsRecursive`](../../../../src/world/terrain/editing.c#L5)
- [`ui/ingame/runtime`](../../ui/ingame/runtime.md): [`InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`](../../../../src/ui/ingame/runtime.c#L5) → [`TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting`](../../../../src/world/terrain/editing.c#L170); [`InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`](../../../../src/ui/ingame/runtime.c#L5) → [`TerrainEditBuffer_CommitFlagsAndMaterialDeltas`](../../../../src/world/terrain/editing.c#L327); [`InGameUiCommand_BeginInteractionByMode`](../../../../src/ui/ingame/runtime.c#L3942) → [`TerrainMaterialEdit_SeedMatchingRegionReplacement`](../../../../src/world/terrain/editing.c#L79); [`InGameUiCommand_BeginInteractionByMode`](../../../../src/ui/ingame/runtime.c#L3942) → [`TerrainMaterialEdit_SeedNonTargetRegionReplacement`](../../../../src/world/terrain/editing.c#L126); [`InGameUiCommand_BeginInteractionByMode`](../../../../src/ui/ingame/runtime.c#L3942) → [`TerrainEditBuffer_CopyCellMaterialBytes`](../../../../src/world/terrain/editing.c#L269); [`InGameUiCommand_EndInteractionByMode`](../../../../src/ui/ingame/runtime.c#L4815) → [`TerrainEditBuffer_ConvertHeightsToDeltas`](../../../../src/world/terrain/editing.c#L356); [`InGameUiCommand_EndInteractionByMode`](../../../../src/ui/ingame/runtime.c#L4815) → [`TerrainEditBuffer_SubtractCurrentCellMaterialBytes`](../../../../src/world/terrain/editing.c#L299)
