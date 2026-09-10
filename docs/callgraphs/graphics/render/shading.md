# Call graph: `graphics/render/shading`

27 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004CDD40` **GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy** — local: `GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry`, `GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds`, `GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks`, `GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks`, `GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set`, `GraphicsShadingGeneratedTexture_FilterGridScratchMmx`, `GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear`, `GraphicsShadingGeneratedTexture_AdvanceTileCursor`; cross: `FixedVec3_DotQ28` → `core/math/fixed`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `FieldGrid_InterpolateTopSurfaceHeight` → `world/terrain/grid`, `FieldGrid_RaycastTerrainTrianglesAlongDirection` → `world/terrain/grid`, `FieldGrid_RaycastTerrainSurfaceDistanceCf` → `world/terrain/grid`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x004BCF70` **GraphicsIntensityClampTable_InitializeCf**
- `0x004CCA90` **GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs**
- `0x004CCB40` **GraphicsShadingRuntime_AllocateRecordRegs**
- `0x004CCC60` **GraphicsShadingRuntime_ClearRecordTable**
- `0x004CCD00` **GraphicsShadingRuntime_RebuildCompactLightingRecords** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`
- `0x004CCD70` **GraphicsShadingRuntime_CollectNearbyRecords**
- `0x004CCFF0` **GraphicsShadingRuntime_InitializeGeneratedTextureCf**
- `0x004CD1B0` **GraphicsShadingRuntime_Shutdown**
- `0x004CD200` **GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes**
- `0x004CD360` **GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources**
- `0x004D1170` **GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlockCf**
- `0x004CD880` **GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear** — local: `GraphicsShadingGeneratedTexture_TransformPointXYQuantized`, `GraphicsShadingGeneratedTexture_RasterizeTriangleMask`
- `0x004CD930` **GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear** — local: `GraphicsShadingGeneratedTexture_ComposeTransform`, `GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear`
- `0x004CD9F0` **GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set** — local: `GraphicsShadingGeneratedTexture_TransformPointXYQuantized`, `GraphicsShadingGeneratedTexture_RasterizeTriangleMask`
- `0x004CDAB0` **GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set** — local: `GraphicsShadingGeneratedTexture_ComposeTransform`, `GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set`
- `0x004CDB80` **GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords** — local: `GraphicsShadingGeneratedTexture_TransformPointXY`
- `0x004CDC20` **GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds** — local: `GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords`; cross: `FixedTransform_Compose` → `core/math/fixed`
- `0x00485020` **GraphicsShadingGeneratedTexture_TransformPointXY**
- `0x004CD2B0` **GraphicsShadingGeneratedTexture_AdvanceTileCursor**
- `0x004CD3D0` **GraphicsShadingGeneratedTexture_FilterGridScratchMmx**
- `0x004CDCE0` **GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry**
- `0x004D1060` **GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks**
- `0x004D1150` **GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks**
- `0x00484FA0` **GraphicsShadingGeneratedTexture_TransformPointXYQuantized**
- `0x00485320` **GraphicsShadingGeneratedTexture_ComposeTransform**
- `0x004CD690` **GraphicsShadingGeneratedTexture_RasterizeTriangleMask**
