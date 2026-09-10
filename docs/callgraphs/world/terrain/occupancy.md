# Call graph: `world/terrain/occupancy`

15 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00507460` **TerrainOccupancyBit2_MarkAroundWorldPoint** — local: `TerrainOccupancyBit2_MarkWedge0`, `TerrainOccupancyBit2_MarkWedge1`, `TerrainOccupancyBit2_MarkWedge2`, `TerrainOccupancyBit2_MarkWedge3`, `TerrainOccupancyBit2_MarkWedge4`, `TerrainOccupancyBit2_MarkWedge5`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x00507610` **TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint** — cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x005138F0` **TerrainOccupancyMask_ResolveRuntimeClassFlags**
- `0x005070A0` **TerrainOccupancyBit2_MarkWedge0** — local: `TerrainOccupancyBit2_MarkDirection0`, `TerrainOccupancyBit2_MarkDirection1`
- `0x00507140` **TerrainOccupancyBit2_MarkWedge1** — local: `TerrainOccupancyBit2_MarkDirection1`, `TerrainOccupancyBit2_MarkDirection2`
- `0x005071E0` **TerrainOccupancyBit2_MarkWedge2** — local: `TerrainOccupancyBit2_MarkDirection2`, `TerrainOccupancyBit2_MarkDirection3`
- `0x00507280` **TerrainOccupancyBit2_MarkWedge3** — local: `TerrainOccupancyBit2_MarkDirection3`, `TerrainOccupancyBit2_MarkDirection4`
- `0x00507320` **TerrainOccupancyBit2_MarkWedge4** — local: `TerrainOccupancyBit2_MarkDirection4`, `TerrainOccupancyBit2_MarkDirection5`
- `0x005073C0` **TerrainOccupancyBit2_MarkWedge5** — local: `TerrainOccupancyBit2_MarkDirection5`, `TerrainOccupancyBit2_MarkDirection0`
- `0x00506EA0` **TerrainOccupancyBit2_MarkDirection0**
- `0x00506EF0` **TerrainOccupancyBit2_MarkDirection1**
- `0x00506F50` **TerrainOccupancyBit2_MarkDirection2**
- `0x00506FA0` **TerrainOccupancyBit2_MarkDirection3**
- `0x00506FF0` **TerrainOccupancyBit2_MarkDirection4**
- `0x00507050` **TerrainOccupancyBit2_MarkDirection5**
