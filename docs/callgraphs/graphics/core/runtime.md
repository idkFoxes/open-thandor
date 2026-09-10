# Call graph: `graphics/core/runtime`

28 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00576C30` **GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer** — local: `GraphicsCursor_RestoreAfterPresent`, `GraphicsCursor_ComposeBeforePresent`
- `0x004168B0` **GraphicsCursor_SetFrameIndex**
- `0x004168E0` **GraphicsCursor_ConsumeNextInputEvent**
- `0x00486430` **Graphics_ProjectViewPoint**
- `0x00486490` **Graphics_SetProjectionClipRect**
- `0x004864D0` **Graphics_SetViewProjectionParameters** — cross: `FixedTransform_BuildRotationBasis` → `core/math/fixed`, `FixedTransform_Compose` → `core/math/fixed`, `FixedMath_Atan2Angle16` → `core/math/fixed`, `FixedMath_SinCosQ28` → `core/math/fixed`
- `0x00486640` **Graphics_SetProjectionViewport**
- `0x004866C0` **Graphics_SetAuxiliaryOrientation** — cross: `FixedMath_WriteDirectionQ28` → `core/math/fixed`, `FixedTransform_BuildRotationBasis` → `core/math/fixed`
- `0x00486730` **Graphics_SetSceneBounds**
- `0x00486790` **Graphics_SetActivePrimitiveQueue**
- `0x004867B0` **Graphics_RebuildFrustumPlanes** — cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `FixedMath_WriteDirectionScaled` → `core/math/fixed`, `FixedVec3_CrossQ12` → `core/math/fixed`, `FixedVec3_NormalizeQ28` → `core/math/fixed`
- `0x004A9100` **GraphicsBackend_RefreshActiveAdapterNoOp**
- `0x004BCFE0` **GraphicsObject_ExtractTransformEulerAnglesRegs** — cross: `FixedTransform_ExtractEulerAnglesRegs` → `core/math/fixed`
- `0x004BD000` **GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs** — cross: `FixedTransform_InvertRigidQ28` → `core/math/fixed`, `FixedMath_WriteDirectionQ28` → `core/math/fixed`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `FixedMath_VectorToAnglesVec3Regs` → `core/math/fixed`
- `0x004BD050` **GraphicsObject_SetTranslationDirectionPackedAnglesAndScale**
- `0x004BD080` **GraphicsObject_SetRotationEulerAnglesPacked**
- `0x004BD0B0` **GraphicsObject_RebuildTransformHierarchyRecursive** — cross: `FixedTransform_BuildRotationBasis` → `core/math/fixed`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `FixedTransform_Compose` → `core/math/fixed`
- `0x00578560` **Graphics_Init** — cross: `CommandLine_FindOption` → `platform/bootstrap/runtime`, `Glide3_InitAndEnumerate` → `graphics/backend/glide`, `DynDLL_Load` → `platform/bootstrap/runtime`, `DynAPI_Resolve` → `platform/bootstrap/runtime`
- `0x005794E0` **GraphicsBackend_RefreshActiveAdapterIfReady** — cross: `Glide3_Shutdown` → `graphics/backend/glide`
- `0x00579520` **Graphics_Shutdown** — cross: `GlideBackend_ShutdownWrapper` → `graphics/backend/glide`, `GraphicsTexture_ReleaseObjects` → `graphics/resources/texture`
- `0x0057A5C0` **Graphics_SetViewportAndClearDepth** — cross: `SoftwareRenderer_ClearViewport` → `graphics/backend/software`, `Memory_ZeroDwords` → `core/memory/allocator`, `Glide3_ClearViewport` → `graphics/backend/glide`
- `0x0057E6D0` **Graphics_BeginScene** — cross: `GlideBackend_BeginSceneNoOp` → `graphics/backend/glide`
- `0x0057E750` **Graphics_EndScene** — cross: `GlideBackend_EndSceneNoOp` → `graphics/backend/glide`
- `0x0057E7A0` **Graphics_DrawPrimitiveQueue** — cross: `SoftwareRenderer_DrawPrimitiveQueueBridge` → `graphics/backend/software`, `GraphicsPrimitiveQueue_Begin` → `graphics/render/primitives`, `GraphicsPrimitiveQueue_Next` → `graphics/render/primitives`, `Glide3_DrawPrimitiveQueue` → `graphics/backend/glide`
- `0x0057A330` **GraphicsCursor_ComposeBeforePresent** — local: `GraphicsCursor_SaveSurfaceBackground`, `GraphicsCursor_RestoreSurfaceBackground`; cross: `Glide3_Cursor_ComposeBeforePresent` → `graphics/backend/glide`
- `0x0057A2C0` **GraphicsCursor_RestoreAfterPresent** — local: `GraphicsCursor_RestoreSurfaceBackground`; cross: `Glide3_Cursor_RestoreAfterPresentNoOp` → `graphics/backend/glide`
- `0x00579EC0` **GraphicsCursor_SaveSurfaceBackground** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x0057A0C0` **GraphicsCursor_RestoreSurfaceBackground** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
