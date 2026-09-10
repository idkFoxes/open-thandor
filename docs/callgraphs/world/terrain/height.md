# Call graph: `world/terrain/height`

40 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00508000` **TerrainHeightBand_TestAroundWorldPoint** — local: `TerrainHeightBand_TestWedge0`, `TerrainHeightBand_TestWedge1`, `TerrainHeightBand_TestWedge2`, `TerrainHeightBand_TestWedge3`, `TerrainHeightBand_TestWedge4`, `TerrainHeightBand_TestWedge5`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x00508920` **TerrainAuxHeightThreshold_TestAroundWorldPoint** — local: `TerrainAuxHeightThreshold_TestWedge0`, `TerrainAuxHeightThreshold_TestWedge1`, `TerrainAuxHeightThreshold_TestWedge2`, `TerrainAuxHeightThreshold_TestWedge3`, `TerrainAuxHeightThreshold_TestWedge4`, `TerrainAuxHeightThreshold_TestWedge5`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x00508D20` **TerrainHeightDelta_ApplyWedge0** — local: `TerrainHeightDelta_ApplyDirection0`, `TerrainHeightDelta_ApplyDirection1`
- `0x00508DC0` **TerrainHeightDelta_ApplyWedge1** — local: `TerrainHeightDelta_ApplyDirection1`, `TerrainHeightDelta_ApplyDirection2`
- `0x00508E60` **TerrainHeightDelta_ApplyWedge2** — local: `TerrainHeightDelta_ApplyDirection2`, `TerrainHeightDelta_ApplyDirection3`
- `0x00508F00` **TerrainHeightDelta_ApplyWedge3** — local: `TerrainHeightDelta_ApplyDirection3`, `TerrainHeightDelta_ApplyDirection4`
- `0x00508FA0` **TerrainHeightDelta_ApplyWedge4** — local: `TerrainHeightDelta_ApplyDirection4`, `TerrainHeightDelta_ApplyDirection5`
- `0x00509040` **TerrainHeightDelta_ApplyWedge5** — local: `TerrainHeightDelta_ApplyDirection5`, `TerrainHeightDelta_ApplyDirection0`
- `0x00504520` **TerrainTriangle_IntersectRayDistanceCf** — cross: `FixedMath_Length3` → `core/math/fixed`
- `0x005049E0` **TerrainRay_AdvanceGridTraversalCf**
- `0x00507AB0` **TerrainHeightBand_TestWedge0** — local: `TerrainHeightBand_TestDirection0`, `TerrainHeightBand_TestDirection1`
- `0x00507BA0` **TerrainHeightBand_TestWedge1** — local: `TerrainHeightBand_TestDirection1`, `TerrainHeightBand_TestDirection2`
- `0x00507C80` **TerrainHeightBand_TestWedge2** — local: `TerrainHeightBand_TestDirection2`, `TerrainHeightBand_TestDirection3`
- `0x00507D60` **TerrainHeightBand_TestWedge3** — local: `TerrainHeightBand_TestDirection3`, `TerrainHeightBand_TestDirection4`
- `0x00507E40` **TerrainHeightBand_TestWedge4** — local: `TerrainHeightBand_TestDirection4`, `TerrainHeightBand_TestDirection5`
- `0x00507F20` **TerrainHeightBand_TestWedge5** — local: `TerrainHeightBand_TestDirection5`, `TerrainHeightBand_TestDirection0`
- `0x00508470` **TerrainAuxHeightThreshold_TestWedge0** — local: `TerrainAuxHeightThreshold_TestDirection0`, `TerrainAuxHeightThreshold_TestDirection1`
- `0x00508540` **TerrainAuxHeightThreshold_TestWedge1** — local: `TerrainAuxHeightThreshold_TestDirection1`, `TerrainAuxHeightThreshold_TestDirection2`
- `0x00508600` **TerrainAuxHeightThreshold_TestWedge2** — local: `TerrainAuxHeightThreshold_TestDirection2`, `TerrainAuxHeightThreshold_TestDirection3`
- `0x005086C0` **TerrainAuxHeightThreshold_TestWedge3** — local: `TerrainAuxHeightThreshold_TestDirection3`, `TerrainAuxHeightThreshold_TestDirection4`
- `0x00508790` **TerrainAuxHeightThreshold_TestWedge4** — local: `TerrainAuxHeightThreshold_TestDirection4`, `TerrainAuxHeightThreshold_TestDirection5`
- `0x00508850` **TerrainAuxHeightThreshold_TestWedge5** — local: `TerrainAuxHeightThreshold_TestDirection5`, `TerrainAuxHeightThreshold_TestDirection0`
- `0x005077F0` **TerrainHeightBand_TestDirection0**
- `0x00507860` **TerrainHeightBand_TestDirection1**
- `0x005078E0` **TerrainHeightBand_TestDirection2**
- `0x00507950` **TerrainHeightBand_TestDirection3**
- `0x005079C0` **TerrainHeightBand_TestDirection4**
- `0x00507A40` **TerrainHeightBand_TestDirection5**
- `0x005081D0` **TerrainAuxHeightThreshold_TestDirection0**
- `0x00508240` **TerrainAuxHeightThreshold_TestDirection1**
- `0x005082B0` **TerrainAuxHeightThreshold_TestDirection2**
- `0x00508320` **TerrainAuxHeightThreshold_TestDirection3**
- `0x00508390` **TerrainAuxHeightThreshold_TestDirection4**
- `0x00508400` **TerrainAuxHeightThreshold_TestDirection5**
- `0x00508AE0` **TerrainHeightDelta_ApplyDirection0**
- `0x00508B40` **TerrainHeightDelta_ApplyDirection1**
- `0x00508BA0` **TerrainHeightDelta_ApplyDirection2**
- `0x00508C00` **TerrainHeightDelta_ApplyDirection3**
- `0x00508C60` **TerrainHeightDelta_ApplyDirection4**
- `0x00508CC0` **TerrainHeightDelta_ApplyDirection5**
