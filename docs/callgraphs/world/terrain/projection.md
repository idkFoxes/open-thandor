# Call graph: `world/terrain/projection`

45 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00506CD0` **TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint** — local: `TerrainProjectedOcclusion_TraceWedge0`, `TerrainProjectedOcclusion_TraceWedge1`, `TerrainProjectedOcclusion_TraceWedge2`, `TerrainProjectedOcclusion_TraceWedge3`, `TerrainProjectedOcclusion_TraceWedge4`, `TerrainProjectedOcclusion_TraceWedge5`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x005099D0` **FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf** — local: `FieldGridTerrainOverlayVariantA_ApplyWedge0`, `FieldGridTerrainOverlayVariantA_ApplyWedge1`, `FieldGridTerrainOverlayVariantA_ApplyWedge2`, `FieldGridTerrainOverlayVariantA_ApplyWedge3`, `FieldGridTerrainOverlayVariantA_ApplyWedge4`, `FieldGridTerrainOverlayVariantA_ApplyWedge5`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x0050A190` **FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf** — local: `FieldGridTerrainOverlayVariantB_ApplyWedge0`, `FieldGridTerrainOverlayVariantB_ApplyWedge1`, `FieldGridTerrainOverlayVariantB_ApplyWedge2`, `FieldGridTerrainOverlayVariantB_ApplyWedge3`, `FieldGridTerrainOverlayVariantB_ApplyWedge4`, `FieldGridTerrainOverlayVariantB_ApplyWedge5`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x00500F50` **TerrainProjectedGrid_TransformShadeAndQueue** — local: `TerrainProjectedGrid_ClipRowSpansAgainstPlane`, `TerrainProjectedVertex_TransformProjectAndShadeVariantB`, `TerrainProjectedVertex_TransformProjectAndShadeVariantA`, `TerrainProjectedQuad_QueueAsTwoTrianglesRegs`
- `0x005066D0` **TerrainProjectedOcclusion_TraceWedge0** — local: `TerrainProjectedOcclusion_ScanDirection0`, `TerrainProjectedOcclusion_ScanDirection1`
- `0x005067D0` **TerrainProjectedOcclusion_TraceWedge1** — local: `TerrainProjectedOcclusion_ScanDirection1`, `TerrainProjectedOcclusion_ScanDirection2`
- `0x005068D0` **TerrainProjectedOcclusion_TraceWedge2** — local: `TerrainProjectedOcclusion_ScanDirection2`, `TerrainProjectedOcclusion_ScanDirection3`
- `0x005069D0` **TerrainProjectedOcclusion_TraceWedge3** — local: `TerrainProjectedOcclusion_ScanDirection3`, `TerrainProjectedOcclusion_ScanDirection4`
- `0x00506AD0` **TerrainProjectedOcclusion_TraceWedge4** — local: `TerrainProjectedOcclusion_ScanDirection4`, `TerrainProjectedOcclusion_ScanDirection5`
- `0x00506BD0` **TerrainProjectedOcclusion_TraceWedge5** — local: `TerrainProjectedOcclusion_ScanDirection5`, `TerrainProjectedOcclusion_ScanDirection0`
- `0x00509580` **FieldGridTerrainOverlayVariantA_ApplyWedge0** — local: `FieldGridTerrainOverlayVariantA_ApplyDirection0`, `FieldGridTerrainOverlayVariantA_ApplyDirection1`
- `0x00509640` **FieldGridTerrainOverlayVariantA_ApplyWedge1** — local: `FieldGridTerrainOverlayVariantA_ApplyDirection1`, `FieldGridTerrainOverlayVariantA_ApplyDirection2`
- `0x005096F0` **FieldGridTerrainOverlayVariantA_ApplyWedge2** — local: `FieldGridTerrainOverlayVariantA_ApplyDirection2`, `FieldGridTerrainOverlayVariantA_ApplyDirection3`
- `0x005097A0` **FieldGridTerrainOverlayVariantA_ApplyWedge3** — local: `FieldGridTerrainOverlayVariantA_ApplyDirection3`, `FieldGridTerrainOverlayVariantA_ApplyDirection4`
- `0x00509860` **FieldGridTerrainOverlayVariantA_ApplyWedge4** — local: `FieldGridTerrainOverlayVariantA_ApplyDirection4`, `FieldGridTerrainOverlayVariantA_ApplyDirection5`
- `0x00509910` **FieldGridTerrainOverlayVariantA_ApplyWedge5** — local: `FieldGridTerrainOverlayVariantA_ApplyDirection5`, `FieldGridTerrainOverlayVariantA_ApplyDirection0`
- `0x00509DD0` **FieldGridTerrainOverlayVariantB_ApplyWedge0** — local: `FieldGridTerrainOverlayVariantB_ApplyDirection0`, `FieldGridTerrainOverlayVariantB_ApplyDirection1`
- `0x00509E70` **FieldGridTerrainOverlayVariantB_ApplyWedge1** — local: `FieldGridTerrainOverlayVariantB_ApplyDirection1`, `FieldGridTerrainOverlayVariantB_ApplyDirection2`
- `0x00509F10` **FieldGridTerrainOverlayVariantB_ApplyWedge2** — local: `FieldGridTerrainOverlayVariantB_ApplyDirection2`, `FieldGridTerrainOverlayVariantB_ApplyDirection3`
- `0x00509FB0` **FieldGridTerrainOverlayVariantB_ApplyWedge3** — local: `FieldGridTerrainOverlayVariantB_ApplyDirection3`, `FieldGridTerrainOverlayVariantB_ApplyDirection4`
- `0x0050A050` **FieldGridTerrainOverlayVariantB_ApplyWedge4** — local: `FieldGridTerrainOverlayVariantB_ApplyDirection4`, `FieldGridTerrainOverlayVariantB_ApplyDirection5`
- `0x0050A0F0` **FieldGridTerrainOverlayVariantB_ApplyWedge5** — local: `FieldGridTerrainOverlayVariantB_ApplyDirection5`, `FieldGridTerrainOverlayVariantB_ApplyDirection0`
- `0x00500CE0` **TerrainProjectedQuad_QueueAsTwoTrianglesRegs** — local: `TerrainProjectedTriangle_ClipInterpolateAndQueueTextured`
- `0x005004A0` **TerrainProjectedVertex_TransformProjectAndShadeVariantA** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`, `GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs` → `graphics/render/shading`
- `0x005006A0` **TerrainProjectedVertex_TransformProjectAndShadeVariantB** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`, `GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs` → `graphics/render/shading`
- `0x00500820` **TerrainProjectedTriangle_ClipInterpolateAndQueueTextured** — cross: `Triangle2D_ComputeBarycentricWeightsQ12Packed` → `core/math/geometry`, `GraphicsPrimitiveQueue_AppendTexturedTriangleRegs` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle` → `graphics/render/primitives`
- `0x00500D30` **TerrainProjectedGrid_ClipRowSpansAgainstPlane**
- `0x005063B0` **TerrainProjectedOcclusion_ScanDirection0**
- `0x00506430` **TerrainProjectedOcclusion_ScanDirection1**
- `0x005064C0` **TerrainProjectedOcclusion_ScanDirection2**
- `0x00506540` **TerrainProjectedOcclusion_ScanDirection3**
- `0x005065C0` **TerrainProjectedOcclusion_ScanDirection4**
- `0x00506650` **TerrainProjectedOcclusion_ScanDirection5**
- `0x00509320` **FieldGridTerrainOverlayVariantA_ApplyDirection0**
- `0x00509380` **FieldGridTerrainOverlayVariantA_ApplyDirection1**
- `0x005093F0` **FieldGridTerrainOverlayVariantA_ApplyDirection2**
- `0x00509450` **FieldGridTerrainOverlayVariantA_ApplyDirection3**
- `0x005094B0` **FieldGridTerrainOverlayVariantA_ApplyDirection4**
- `0x00509520` **FieldGridTerrainOverlayVariantA_ApplyDirection5**
- `0x00509B90` **FieldGridTerrainOverlayVariantB_ApplyDirection0**
- `0x00509BF0` **FieldGridTerrainOverlayVariantB_ApplyDirection1**
- `0x00509C50` **FieldGridTerrainOverlayVariantB_ApplyDirection2**
- `0x00509CB0` **FieldGridTerrainOverlayVariantB_ApplyDirection3**
- `0x00509D10` **FieldGridTerrainOverlayVariantB_ApplyDirection4**
- `0x00509D70` **FieldGridTerrainOverlayVariantB_ApplyDirection5**
