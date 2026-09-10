# Call graph: `world/terrain/editing`

11 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005137F0` **TerrainRegionCollection_CollectConnectedCellsRecursive** — local: `TerrainRegionCollection_RecordConnectedCell`
- `0x00561A10` **TerrainMaterialEdit_SeedMatchingRegionReplacement** — local: `TerrainMaterialEdit_PropagateMatchingRegionReplacement`
- `0x00561AE0` **TerrainMaterialEdit_SeedNonTargetRegionReplacement** — local: `TerrainMaterialEdit_PropagateNonTargetRegionReplacement`
- `0x005616D0` **TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting** — cross: `FieldGridCell_RecomputeTriangleNormalAngles` → `world/terrain/grid`, `FieldGridCell_ComputeDirectionalLightColor` → `world/terrain/grid`
- `0x00561830` **TerrainEditBuffer_CopyCellMaterialBytes**
- `0x00561930` **TerrainEditBuffer_SubtractCurrentCellMaterialBytes**
- `0x005619A0` **TerrainEditBuffer_CommitFlagsAndMaterialDeltas**
- `0x00561DC0` **TerrainEditBuffer_ConvertHeightsToDeltas**
- `0x00513790` **TerrainRegionCollection_RecordConnectedCell**
- `0x00571600` **TerrainMaterialEdit_PropagateMatchingRegionReplacement**
- `0x00571730` **TerrainMaterialEdit_PropagateNonTargetRegionReplacement**
