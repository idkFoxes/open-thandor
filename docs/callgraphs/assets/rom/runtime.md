# Call graph: `assets/rom/runtime`

18 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005452A0` **FrontendRomActionTable_ExecuteRecord** — local: `RomRegistry_FindRecordByIdCf`, `FrontendRomTransition_InitializeFromRecord`, `RomRuntime_UpdateRecordVisibilityAndDescriptorsCf`; cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x00546450` **RomAsset_PrepareRecords** — local: `RomAssetRecord_RegisterAndRelocate`; cross: `Package_SetLastErrorPath` → `assets/package/runtime`
- `0x005466A0` **RomRuntime_BuildAllRegistryNodeTrees** — local: `RomRuntime_BuildNodeTreeRecursive`; cross: `WorldRuntime_LinkNodeIntoOwnerListD8` → `world/runtime/core`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`
- `0x00547FC0` **FrontendRomTransition_ProcessPendingRecord** — local: `FrontendRomTransition_ActivateRecordByIdCf`; cross: `WorldMotionSpline_EvaluateAndApplyAtTime` → `core/math/interpolation`
- `0x00547400` **FrontendRomRegistry_ClearAndReleaseNestedResources** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x00548720` **FrontendRomTransition_RequestStop**
- `0x005487F0` **RomRegistry_FindRecordBySlotValue**
- `0x00548840` **RomRegistry_FindSlotValueByRecord**
- `0x00548890` **RomRecordTable_FindRecordById**
- `0x005488D0` **RomRecordTable_FindIndexById**
- `0x005484D0` **FrontendRomTransition_ActivateRecordByIdCf** — local: `RomRegistry_FindRecordByIdCf`, `RomRegistry_FindSlotValueByRecordIdCf`, `RomRuntime_ApplyIndexedDescriptor`; cross: `GraphicsShadingRuntime_ClearRecordTable` → `graphics/render/shading`, `WorldRuntime_SetPosition60AndDistanceFromPosition80` → `world/runtime/core`, `WorldRuntime_SetMotionParameters6CThrough78Clamped` → `world/runtime/core`, `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x00548600` **RomRuntime_UpdateRecordVisibilityAndDescriptorsCf** — local: `RomRegistry_FindRecordByIdCf`, `RomRuntime_ApplyIndexedDescriptor`; cross: `GraphicsShadingRuntime_ClearRecordTable` → `graphics/render/shading`
- `0x00546330` **RomAssetRecord_RegisterAndRelocate** — cross: `Package_SetLastErrorPath` → `assets/package/runtime`, `WidePath_SetExtensionCode` → `core/text/path`, `Package_LoadEntry` → `assets/package/runtime`, `SpriteAssetRegistry_FindById` → `assets/sprite/catalog`, `SpriteAsset_RegisterAndRelocatePointers` → `assets/sprite/catalog`, `Resource_Release` → `assets/resource/runtime`
- `0x005464C0` **RomRuntime_BuildNodeTreeRecursive** — cross: `WorldObjectArray_AllocateFreeRecordCf` → `world/runtime/core`
- `0x005483C0` **FrontendRomTransition_InitializeFromRecord** — cross: `WorldMotionSpline_BuildSixChannelCurves` → `core/math/interpolation`
- `0x005487A0` **RomRegistry_FindSlotValueByRecordIdCf**
- `0x00548410` **RomRuntime_ApplyIndexedDescriptor** — cross: `GraphicsShadingRuntime_AllocateRecordRegs` → `graphics/render/shading`
- `0x00548740` **RomRegistry_FindRecordByIdCf**
