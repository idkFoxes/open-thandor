# Call graph: `graphics/render/model`

16 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004BDC90` **ModelRender_DrawMeshGroupsWithTemporaryTransform** — local: `ModelRender_SubmitMeshTriangles`; cross: `ModelNodeRuntime_BuildViewFacingRotation` → `world/model/hierarchy`, `ModelNodeRuntime_BuildBillboardRotation` → `world/model/hierarchy`
- `0x004BE1F0` **ModelRender_DrawMeshGroupsAlternatePath** — local: `ModelRender_SubmitMeshTrianglesAlternatePath`; cross: `ModelNodeRuntime_BuildViewFacingRotation` → `world/model/hierarchy`, `ModelNodeRuntime_BuildBillboardRotation` → `world/model/hierarchy`
- `0x0050A5C0` **ModelProjectedBounds_AccumulateHierarchyRecursive** — local: `ModelProjectedBounds_AccumulateNode`
- `0x004BD4B0` **ModelRender_PrepareProjectedVertex** — local: `ModelRender_ComputeVertexIntensityDefaultPath`, `ModelRender_ComputeVertexIntensityScaledPath`; cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x004BD9B0` **ModelRender_SubmitTriangle** — local: `ModelRender_ComputeFacingDotQ12`, `ModelRender_PrepareProjectedVertex`; cross: `GraphicsPrimitiveQueue_AppendTriangle` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_SetVertexColors` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_SetMaterial` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_OffsetTextureCoordinates` → `graphics/render/primitives`
- `0x004BDC20` **ModelRender_SubmitMeshTriangles** — local: `ModelRender_PrepareViewDirections`, `ModelRender_SubmitTriangle`
- `0x004BE180` **ModelRender_SubmitMeshTrianglesAlternatePath** — local: `ModelRender_PrepareViewDirections`, `ModelRender_SubmitTriangleAlternatePath`
- `0x004BD6B0` **ModelRender_PrepareProjectedVertexAlternatePath** — local: `ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath`; cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x004BDFB0` **ModelRender_SubmitTriangleAlternatePath** — local: `ModelRender_PrepareProjectedVertexAlternatePath`; cross: `GraphicsPrimitiveQueue_AppendTriangle` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_SetVertexColors` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_SetMaterial` → `graphics/render/primitives`
- `0x0050A4A0` **ModelProjectedBounds_AccumulateNode** — local: `ModelProjectedBounds_ExpandWithCurrentScratchPoint`; cross: `FixedTransform_Compose` → `core/math/fixed`
- `0x004BD7E0` **ModelRender_ComputeFacingDotQ12** — cross: `FixedVec3_DotQ12` → `core/math/fixed`
- `0x004CC710` **ModelRender_ComputeVertexIntensityDefaultPath** — cross: `FixedVec3_DotQ12` → `core/math/fixed`
- `0x004CC820` **ModelRender_ComputeVertexIntensityScaledPath** — cross: `FixedVec3_DotQ12` → `core/math/fixed`
- `0x004CC940` **ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath** — cross: `FixedTransform_ApplyDirection` → `core/math/fixed`, `FixedVec3_NormalizeQ28` → `core/math/fixed`, `FixedVec3_DotQ12` → `core/math/fixed`
- `0x0050A430` **ModelProjectedBounds_ExpandWithCurrentScratchPoint** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x004BD800` **ModelRender_PrepareViewDirections** — cross: `FixedTransform_Compose` → `core/math/fixed`, `FixedMath_VectorToAngles3Regs` → `core/math/fixed`, `FixedMath_WriteDirectionQ28` → `core/math/fixed`, `FixedTransform_ApplyTransposeDirection` → `core/math/fixed`
