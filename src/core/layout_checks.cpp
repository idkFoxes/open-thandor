/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/layout_checks.cpp
 */

/* The structs of the type headers (<area>/<module>/types.h) are the original's 32-bit layouts on x86 and x64
   (core/ptr32.h): their sizes and the offsets of their pointer fields, checked at compile time. Generated from the
   x86 layouts (MSVC /d1reportAllClassLayout); runtime-only structs with native pointers (the arena, timer, PCK
   mount and Huffman workspace, spatial sound slots) and the copies of Windows/DirectX structs are not listed. */

#include <stddef.h>
#include <type_traits>
#include <thandor/thandor.h>
#include <thandor/core/flags.h>
#include <thandor/graphics/render/model.h> /* ModelMeshGroupHeader, ModelMeshHeader (MDL records) */

static_assert(sizeof(GraphicsTextureLogicalSize) == 0x8,
              "GraphicsTextureLogicalSize keeps its 32-bit layout");
static_assert(sizeof(FixedVectorAngles) == 0x8,
              "FixedVectorAngles keeps its 32-bit layout");
static_assert(sizeof(FixedElevationAzimuth) == 0x8,
              "FixedElevationAzimuth keeps its 32-bit layout");
static_assert(sizeof(ArmySegmentMeter) == 0x8,
              "ArmySegmentMeter keeps its 32-bit layout");
static_assert(sizeof(ModelWorldPoint) == 0xC,
              "ModelWorldPoint keeps its 32-bit layout");
static_assert(sizeof(FixedDirection) == 0xC,
              "FixedDirection keeps its 32-bit layout");
static_assert(sizeof(HINSTANCE__) == 0x4,
              "HINSTANCE__ keeps its 32-bit layout");
static_assert(sizeof(ModelResource) == 0x210,
              "ModelResource keeps its 32-bit layout");
static_assert(sizeof(GameEntityDamageCounterOrTerminalReference4) == 0x4 &&
              offsetof(GameEntityDamageCounterOrTerminalReference4, terminalEntity) == 0x0,
              "GameEntityDamageCounterOrTerminalReference4 keeps its 32-bit layout");
static_assert(sizeof(GraphicsFixedVec3) == 0xC,
              "GraphicsFixedVec3 keeps its 32-bit layout");
static_assert(sizeof(GraphicsFixedMatrix3x4) == 0x30,
              "GraphicsFixedMatrix3x4 keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionRecordPrefix) == 0xC,
              "ModelDefinitionRecordPrefix keeps its 32-bit layout");
static_assert(sizeof(GameEntityTechnologyPayload) == 0xFC,
              "GameEntityTechnologyPayload keeps its 32-bit layout");
static_assert(sizeof(EffectRuntimeOwnerReference) == 0x4 &&
              offsetof(EffectRuntimeOwnerReference, modelNode) == 0x0 &&
              offsetof(EffectRuntimeOwnerReference, modelRuntime) == 0x0 &&
              offsetof(EffectRuntimeOwnerReference, armyRuntime) == 0x0 &&
              offsetof(EffectRuntimeOwnerReference, terrainImpactColumns) == 0x0 &&
              offsetof(EffectRuntimeOwnerReference, entityRuntime) == 0x0,
              "EffectRuntimeOwnerReference keeps its 32-bit layout");
static_assert(sizeof(EffectRuntimeOwnerAndDefinitionState) == 0x8,
              "EffectRuntimeOwnerAndDefinitionState keeps its 32-bit layout");
static_assert(sizeof(EffectRuntimeLifecycleState) == 0x10,
              "EffectRuntimeLifecycleState keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeLinkedChildSpawnParameters) == 0xC,
              "ArmyRuntimeLinkedChildSpawnParameters keeps its 32-bit layout");
static_assert(sizeof(GameEntityOwnershipState10) == 0x10 &&
              offsetof(GameEntityOwnershipState10, definitionOrClassRecord) == 0x0 &&
              offsetof(GameEntityOwnershipState10, modelNode) == 0x4 &&
              offsetof(GameEntityOwnershipState10, runtimeLink) == 0x8,
              "GameEntityOwnershipState10 keeps its 32-bit layout");
/* Step 13 X3: the typed accessors (member functions) leave the record a trivially copyable standard-layout
   aggregate: savegame pools and the GameEntityRuntime views copy and overlay it as raw bytes. */
static_assert(std::is_trivially_copyable_v<GameEntityOwnershipState10> &&
              std::is_standard_layout_v<GameEntityOwnershipState10> &&
              std::is_aggregate_v<GameEntityOwnershipState10> &&
              offsetof(GameEntityRuntimeCommon, ownership) == 0x0 &&
              offsetof(GameEntityRuntime, common) == 0x0,
              "GameEntityOwnershipState10 stays a plain 32-bit record at GameEntityRuntime +0x0");
static_assert(sizeof(ShotDefinition) == 0x2E0 &&
              offsetof(ShotDefinition, primaryEffectDefinition) == 0x10 &&
              offsetof(ShotDefinition, terrainImpactEffectDefinitions31) == 0x14 &&
              offsetof(ShotDefinition, targetClassImpactEffectDefinitions8) == 0x90 &&
              offsetof(ShotDefinition, launchEffectDefinition) == 0xD4 &&
              offsetof(ShotDefinition, secondaryEffectDefinition) == 0x268 &&
              offsetof(ShotDefinition, ownedNestedResource) == 0x294,
              "ShotDefinition keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeSelectionState) == 0x44 &&
              offsetof(WorldRuntimeSelectionState, selectedEntity) == 0x20 &&
              offsetof(WorldRuntimeSelectionState, dispatchCommandCallback) == 0x24 &&
              offsetof(WorldRuntimeSelectionState, resolveContextActionPrimaryCallback) == 0x28 &&
              offsetof(WorldRuntimeSelectionState, resolveContextActionSecondaryCallback) == 0x2C &&
              offsetof(WorldRuntimeSelectionState, beginPointerCaptureCallback) == 0x30 &&
              offsetof(WorldRuntimeSelectionState, updateDragSelectionCallback) == 0x34 &&
              offsetof(WorldRuntimeSelectionState, commitPointerActionCallback) == 0x38 &&
              offsetof(WorldRuntimeSelectionState, dispatchWorldContextActionCallback) == 0x3C,
              "WorldRuntimeSelectionState keeps its 32-bit layout");
static_assert(sizeof(EffectModelNodeReferenceOrSavedOffset) == 0x4 &&
              offsetof(EffectModelNodeReferenceOrSavedOffset, modelNode) == 0x0,
              "EffectModelNodeReferenceOrSavedOffset keeps its 32-bit layout");
static_assert(sizeof(WorldMotionState) == 0x38,
              "WorldMotionState keeps its 32-bit layout");
static_assert(sizeof(WorldMotionSnapshot) == 0x1C,
              "WorldMotionSnapshot keeps its 32-bit layout");
static_assert(sizeof(WorldLightingState) == 0x20,
              "WorldLightingState keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeInteractionState) == 0x4C,
              "WorldRuntimeInteractionState keeps its 32-bit layout");
static_assert(sizeof(WorldFieldRegionState) == 0x10 &&
              offsetof(WorldFieldRegionState, clearTransientStateCallback) == 0x0,
              "WorldFieldRegionState keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeArmyLinkOrState) == 0x4 &&
              offsetof(ModelRuntimeArmyLinkOrState, armyRuntime) == 0x0,
              "ModelRuntimeArmyLinkOrState keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeCoordinateCommandOrHistoryValue) == 0x4,
              "ArmyRuntimeCoordinateCommandOrHistoryValue keeps its 32-bit layout");
static_assert(sizeof(GraphicsAdapterRecord) == 0x80,
              "GraphicsAdapterRecord keeps its 32-bit layout");
static_assert(sizeof(ResourceRegistrationRuntimePayloadReference4) == 0x4 &&
              offsetof(ResourceRegistrationRuntimePayloadReference4, armyRuntime) == 0x0 &&
              offsetof(ResourceRegistrationRuntimePayloadReference4, effectRuntime) == 0x0 &&
              offsetof(ResourceRegistrationRuntimePayloadReference4, shotRuntime) == 0x0,
              "ResourceRegistrationRuntimePayloadReference4 keeps its 32-bit layout");
static_assert(sizeof(ModelAttachmentTransformRecord) == 0x10,
              "ModelAttachmentTransformRecord keeps its 32-bit layout");
static_assert(sizeof(WorldObjectRecordCommon) == 0x50 &&
              offsetof(WorldObjectRecordCommon, ownerWorld) == 0x8,
              "WorldObjectRecordCommon keeps its 32-bit layout");
static_assert(sizeof(WorldObjectRecord) == 0x100,
              "WorldObjectRecord keeps its 32-bit layout");
static_assert(sizeof(GameEntityImpactOwnerLinksPayloadFC) == 0xFC &&
              offsetof(GameEntityImpactOwnerLinksPayloadFC, attachment0ChildModelRuntime) == 0x3C &&
              offsetof(GameEntityImpactOwnerLinksPayloadFC, attachment1ChildModelRuntime) == 0x5C,
              "GameEntityImpactOwnerLinksPayloadFC keeps its 32-bit layout");
static_assert(sizeof(GameEntityRuntimeClassPayload) == 0xFC,
              "GameEntityRuntimeClassPayload keeps its 32-bit layout");
static_assert(sizeof(GameEntityImpactReactionBytes8) == 0x8,
              "GameEntityImpactReactionBytes8 keeps its 32-bit layout");
static_assert(sizeof(GameEntityPathingReferenceState8) == 0x8 &&
              offsetof(GameEntityPathingReferenceState8, overlappingEntity) == 0x0 &&
              offsetof(GameEntityPathingReferenceState8, secondaryPathingReference) == 0x4,
              "GameEntityPathingReferenceState8 keeps its 32-bit layout");
static_assert(sizeof(GameEntityPathingAndImpactState10) == 0x10,
              "GameEntityPathingAndImpactState10 keeps its 32-bit layout");
static_assert(sizeof(GameEntityDamageState2C) == 0x2C,
              "GameEntityDamageState2C keeps its 32-bit layout");
static_assert(sizeof(GameEntityCommandTargetState) == 0x18 &&
              offsetof(GameEntityCommandTargetState, targetEntity) == 0x0,
              "GameEntityCommandTargetState keeps its 32-bit layout");
static_assert(sizeof(GameEntityRuntimeCommon) == 0x104,
              "GameEntityRuntimeCommon keeps its 32-bit layout");
static_assert(sizeof(GameEntityRuntime) == 0x200,
              "GameEntityRuntime keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeReferenceOrSavedOffset) == 0x4 &&
              offsetof(ArmyRuntimeReferenceOrSavedOffset, armyRuntime) == 0x0 &&
              offsetof(ArmyRuntimeReferenceOrSavedOffset, entityRuntime) == 0x0 &&
              offsetof(ArmyRuntimeReferenceOrSavedOffset, modelRuntime) == 0x0,
              "ArmyRuntimeReferenceOrSavedOffset keeps its 32-bit layout");
static_assert(sizeof(AssetProducerSourceNames) == 0x80,
              "AssetProducerSourceNames keeps its 32-bit layout");
static_assert(sizeof(AssetBuildTimestampSet) == 0x18,
              "AssetBuildTimestampSet keeps its 32-bit layout");
static_assert(sizeof(GeneratedAssetBuildMetadata) == 0xA0,
              "GeneratedAssetBuildMetadata keeps its 32-bit layout");
static_assert(sizeof(GeneratedAssetCommonPrefix) == 0xB0,
              "GeneratedAssetCommonPrefix keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeMovementControlState) == 0x8,
              "ArmyRuntimeMovementControlState keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeContactRadiusOrLinkedSlotMask) == 0x4,
              "ArmyRuntimeContactRadiusOrLinkedSlotMask keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeArticulatedContactState) == 0x14,
              "ArmyRuntimeArticulatedContactState keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeLinkedChildPendingCounts) == 0x4,
              "ArmyRuntimeLinkedChildPendingCounts keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeLinkedChildOverloadedState) == 0x10,
              "ArmyRuntimeLinkedChildOverloadedState keeps its 32-bit layout");
static_assert(sizeof(GraphicsPaletteAssetEntry) == 0x8,
              "GraphicsPaletteAssetEntry keeps its 32-bit layout");
static_assert(sizeof(GraphicsPaletteAsset) == 0x208,
              "GraphicsPaletteAsset keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureResource) == 0x50,
              "GraphicsTextureResource keeps its 32-bit layout");
static_assert(sizeof(ShotDefinitionReferenceOrSavedId) == 0x4 &&
              offsetof(ShotDefinitionReferenceOrSavedId, definition) == 0x0,
              "ShotDefinitionReferenceOrSavedId keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeSlotLinkOrState) == 0x4 &&
              offsetof(ModelRuntimeSlotLinkOrState, modelRuntime) == 0x0,
              "ModelRuntimeSlotLinkOrState keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSetEntry) == 0x20 &&
              offsetof(GraphicsTextureSetEntry, texture) == 0x0 &&
              offsetof(GraphicsTextureSetEntry, sourceAsset) == 0xC &&
              offsetof(GraphicsTextureSetEntry, sourceEntry) == 0x10,
              "GraphicsTextureSetEntry keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeSlotClassState) == 0x7C,
              "ModelRuntimeSlotClassState keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeClassLinkState) == 0x24,
              "ModelRuntimeClassLinkState keeps its 32-bit layout");
static_assert(sizeof(ShotRuntimeOwnerAndTrajectoryState) == 0x18 &&
              offsetof(ShotRuntimeOwnerAndTrajectoryState, ownerArmyRuntime) == 0x0,
              "ShotRuntimeOwnerAndTrajectoryState keeps its 32-bit layout");
static_assert(sizeof(ShotModelNodeReferenceOrSavedOffset) == 0x4 &&
              offsetof(ShotModelNodeReferenceOrSavedOffset, modelNode) == 0x0,
              "ShotModelNodeReferenceOrSavedOffset keeps its 32-bit layout");
static_assert(sizeof(ShotModelRuntimeStateOrSavedOffset) == 0x4 &&
              offsetof(ShotModelRuntimeStateOrSavedOffset, runtimeStatePointer) == 0x0 &&
              offsetof(ShotModelRuntimeStateOrSavedOffset, targetModelRuntime) == 0x0,
              "ShotModelRuntimeStateOrSavedOffset keeps its 32-bit layout");
static_assert(sizeof(ShotRuntimeSlot) == 0x40,
              "ShotRuntimeSlot keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeNodeCommon) == 0xC &&
              offsetof(WorldRuntimeNodeCommon, previousNode) == 0x0 &&
              offsetof(WorldRuntimeNodeCommon, nextNode) == 0x4 &&
              offsetof(WorldRuntimeNodeCommon, ownerWorld) == 0x8,
              "WorldRuntimeNodeCommon keeps its 32-bit layout");
static_assert(sizeof(EffectDefinitionTransitionPrefix) == 0x8,
              "EffectDefinitionTransitionPrefix keeps its 32-bit layout");
static_assert(sizeof(EffectDefinition) == 0xC0 &&
              offsetof(EffectDefinition, linkedEffectDefinition) == 0x14 &&
              offsetof(EffectDefinition, linkedShotDefinition) == 0x1C &&
              offsetof(EffectDefinition, periodicEffectDefinition) == 0x48 &&
              offsetof(EffectDefinition, ownedNestedResource) == 0x74,
              "EffectDefinition keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimePayloadReference4) == 0x4 &&
              offsetof(ModelRuntimePayloadReference4, modelRuntime) == 0x0 &&
              offsetof(ModelRuntimePayloadReference4, armyRuntime) == 0x0 &&
              offsetof(ModelRuntimePayloadReference4, effectRuntime) == 0x0 &&
              offsetof(ModelRuntimePayloadReference4, shotRuntime) == 0x0 &&
              offsetof(ModelRuntimePayloadReference4, opaqueRuntime) == 0x0,
              "ModelRuntimePayloadReference4 keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeNodeModelPayload) == 0x3C &&
              offsetof(WorldRuntimeNodeModelPayload, paletteAsset) == 0x24 &&
              offsetof(WorldRuntimeNodeModelPayload, textureSet) == 0x28 &&
              offsetof(WorldRuntimeNodeModelPayload, modelResource) == 0x34,
              "WorldRuntimeNodeModelPayload keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeNode) == 0x100 &&
              offsetof(ModelRuntimeNode, shadingRecord) == 0x5C &&
              offsetof(ModelRuntimeNode, modelRuntimeLinkOrSavedOffset) == 0x60 &&
              offsetof(ModelRuntimeNode, parentNode) == 0xC4 &&
              offsetof(ModelRuntimeNode, childNodes) == 0xCC,
              "ModelRuntimeNode keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeNodeReferenceOrSavedOffset4) == 0x4 &&
              offsetof(ModelRuntimeNodeReferenceOrSavedOffset4, modelNode) == 0x0,
              "ModelRuntimeNodeReferenceOrSavedOffset4 keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeAttachmentDescriptor) == 0x20 &&
              offsetof(ModelRuntimeAttachmentDescriptor, childModelRuntimeOrSavedOffset) == 0x0 &&
              offsetof(ModelRuntimeAttachmentDescriptor, sourceTransform) == 0x4 &&
              offsetof(ModelRuntimeAttachmentDescriptor, parentModelNodeOrSavedOffset) == 0x8,
              "ModelRuntimeAttachmentDescriptor keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionReferenceOrSavedId) == 0x4 &&
              offsetof(ModelDefinitionReferenceOrSavedId, definition) == 0x0 &&
              offsetof(ModelDefinitionReferenceOrSavedId, runtimeDefinition) == 0x0,
              "ModelDefinitionReferenceOrSavedId keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeSlotReferenceOrSavedOffset) == 0x4 &&
              offsetof(ModelRuntimeSlotReferenceOrSavedOffset, modelRuntime) == 0x0,
              "ModelRuntimeSlotReferenceOrSavedOffset keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeSlot) == 0x120 &&
              offsetof(ArmyRuntimeSlot, modelNodeRuntime) == 0x4 &&
              offsetof(ArmyRuntimeSlot, linkedEntityRuntime) == 0x8 &&
              offsetof(ArmyRuntimeSlot, commandTargetArmyRuntime) == 0x1C &&
              offsetof(ArmyRuntimeSlot, linkedArmyRuntimeOrSavedOffset) == 0x6C &&
              offsetof(ArmyRuntimeSlot, linkedArmyRuntime) == 0xF0,
              "ArmyRuntimeSlot keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeSlot) == 0x200,
              "ModelRuntimeSlot keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeNodePayload) == 0x3C,
              "WorldRuntimeNodePayload keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeNode) == 0x50 &&
              offsetof(WorldRuntimeNode, runtimePayload) == 0x48,
              "WorldRuntimeNode keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSourceEntry) == 0x20,
              "GraphicsTextureSourceEntry keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSourceTableDescriptor) == 0xC,
              "GraphicsTextureSourceTableDescriptor keeps its 32-bit layout");
static_assert(sizeof(EffectDefinitionReferenceOrSavedId) == 0x4 &&
              offsetof(EffectDefinitionReferenceOrSavedId, definition) == 0x0,
              "EffectDefinitionReferenceOrSavedId keeps its 32-bit layout");
static_assert(sizeof(GraphicsShadingRuntimeRecord) == 0x40,
              "GraphicsShadingRuntimeRecord keeps its 32-bit layout");
static_assert(sizeof(EffectRuntimeSlot) == 0x40,
              "EffectRuntimeSlot keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSet) == 0x28 &&
              offsetof(GraphicsTextureSet, sourceAsset) == 0x0,
              "GraphicsTextureSet keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSourceAsset) == 0x200,
              "GraphicsTextureSourceAsset keeps its 32-bit layout");
static_assert(sizeof(UiCommandPayloadTriple) == 0xC,
              "UiCommandPayloadTriple keeps its 32-bit layout");
static_assert(sizeof(UiCommandPayloadTextBatch48) == 0x30,
              "UiCommandPayloadTextBatch48 keeps its 32-bit layout");
static_assert(sizeof(NetworkEndpointFamilyPortFields4) == 0x4,
              "NetworkEndpointFamilyPortFields4 keeps its 32-bit layout");
static_assert(sizeof(NetworkEndpointAddressHeader4) == 0x4,
              "NetworkEndpointAddressHeader4 keeps its 32-bit layout");
static_assert(sizeof(PckHuffmanSymbolState) == 0x4,
              "PckHuffmanSymbolState keeps its 32-bit layout");
static_assert(sizeof(SpriteAssetReferenceOrSavedId) == 0x4 &&
              offsetof(SpriteAssetReferenceOrSavedId, spriteAsset) == 0x0 &&
              offsetof(SpriteAssetReferenceOrSavedId, modelResource) == 0x0,
              "SpriteAssetReferenceOrSavedId keeps its 32-bit layout");
static_assert(sizeof(GeneratedAssetRegistryHeader) == 0xBC &&
              offsetof(GeneratedAssetRegistryHeader, previousRegistryAsset) == 0xB4,
              "GeneratedAssetRegistryHeader keeps its 32-bit layout");
static_assert(sizeof(SpriteAssetHeader) == 0x200,
              "SpriteAssetHeader keeps its 32-bit layout");
static_assert(sizeof(Utf16DecimalDigitPair4) == 0x4,
              "Utf16DecimalDigitPair4 keeps its 32-bit layout");
static_assert(sizeof(WidePathBuffer256) == 0x200,
              "WidePathBuffer256 keeps its 32-bit layout");
static_assert(sizeof(FrontendTaskAssignmentFactionTextRow) == 0x50,
              "FrontendTaskAssignmentFactionTextRow keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayModeSelection) == 0x10,
              "FrontendDisplayModeSelection keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayModeCandidates) == 0x28,
              "FrontendDisplayModeCandidates keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayModeEnumerationState) == 0x280,
              "FrontendDisplayModeEnumerationState keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayModeCandidateValues) == 0x28,
              "FrontendDisplayModeCandidateValues keeps its 32-bit layout");
static_assert(sizeof(FrontendTaskAssignmentFactionTexts) == 0x280,
              "FrontendTaskAssignmentFactionTexts keeps its 32-bit layout");
static_assert(sizeof(FrontendUiScratch) == 0x280,
              "FrontendUiScratch keeps its 32-bit layout");
static_assert(sizeof(UiGridDimensions) == 0x8,
              "UiGridDimensions keeps its 32-bit layout");
static_assert(sizeof(FieldGridCoordinates) == 0x8,
              "FieldGridCoordinates keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeContext) == 0x15C &&
              offsetof(WorldRuntimeContext, fieldGrid) == 0x54 &&
              offsetof(WorldRuntimeContext, objectArray) == 0x58 &&
              offsetof(WorldRuntimeContext, dwordArray) == 0xC0 &&
              offsetof(WorldRuntimeContext, tickSpinLock) == 0xD0 &&
              offsetof(WorldRuntimeContext, simulationAndNetworkTickCallback) == 0xD4 &&
              offsetof(WorldRuntimeContext, ownerListHead) == 0xD8,
              "WorldRuntimeContext keeps its 32-bit layout");
static_assert(sizeof(UiNodeBase) == 0x4C &&
              offsetof(UiNodeBase, nextSibling) == 0x0 &&
              offsetof(UiNodeBase, firstChild) == 0x4 &&
              offsetof(UiNodeBase, parent) == 0x8 &&
              offsetof(UiNodeBase, vtable) == 0xC,
              "UiNodeBase keeps its 32-bit layout");
static_assert(sizeof(UiNumericTextControl) == 0x94 &&
              offsetof(UiNumericTextControl, activationSound) == 0x68,
              "UiNumericTextControl keeps its 32-bit layout");
static_assert(sizeof(UiSelectableControl) == 0x54,
              "UiSelectableControl keeps its 32-bit layout");
static_assert(sizeof(UiSoundSelectableControl) == 0x60 &&
              offsetof(UiSoundSelectableControl, activationSound) == 0x5C,
              "UiSoundSelectableControl keeps its 32-bit layout");
static_assert(sizeof(UiNodeVtable) == 0x48 &&
              offsetof(UiNodeVtable, relocate) == 0x0 &&
              offsetof(UiNodeVtable, method04) == 0x4 &&
              offsetof(UiNodeVtable, drawClipped) == 0x8 &&
              offsetof(UiNodeVtable, layout) == 0xC &&
              offsetof(UiNodeVtable, nonRightPress) == 0x10 &&
              offsetof(UiNodeVtable, nonRightRelease) == 0x14 &&
              offsetof(UiNodeVtable, rightPress) == 0x18 &&
              offsetof(UiNodeVtable, rightRelease) == 0x1C &&
              offsetof(UiNodeVtable, nonRightDrag) == 0x20 &&
              offsetof(UiNodeVtable, rightDrag) == 0x24 &&
              offsetof(UiNodeVtable, pointerMove) == 0x28 &&
              offsetof(UiNodeVtable, hitTest) == 0x2C &&
              offsetof(UiNodeVtable, keyboardEvent) == 0x30 &&
              offsetof(UiNodeVtable, applyFlags) == 0x34 &&
              offsetof(UiNodeVtable, suppressActionId) == 0x38 &&
              offsetof(UiNodeVtable, unsuppressActionId) == 0x3C &&
              offsetof(UiNodeVtable, tick) == 0x40 &&
              offsetof(UiNodeVtable, pointerWheel) == 0x44,
              "UiNodeVtable keeps its 32-bit layout");
static_assert(sizeof(UiPageStackControl) == 0x54 &&
              offsetof(UiPageStackControl, pages) == 0x50,
              "UiPageStackControl keeps its 32-bit layout");
static_assert(sizeof(InGamePersistentSettingsPage3508) == 0x3508,
              "InGamePersistentSettingsPage3508 keeps its 32-bit layout");
static_assert(sizeof(UiPointerListControl) == 0x64 &&
              offsetof(UiPointerListControl, rowSlots) == 0x50 &&
              offsetof(UiPointerListControl, selectedRowSlot) == 0x60,
              "UiPointerListControl keeps its 32-bit layout");
static_assert(sizeof(FixedSinCos) == 0x8,
              "FixedSinCos keeps its 32-bit layout");
static_assert(sizeof(FrontendPersistentSettingsPage) == 0x417C,
              "FrontendPersistentSettingsPage keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSetupPageState) == 0x4BCC,
              "FrontendNetworkSetupPageState keeps its 32-bit layout");
static_assert(sizeof(MovieFrameDimensions) == 0x8,
              "MovieFrameDimensions keeps its 32-bit layout");
static_assert(sizeof(GraphicsProjectedPointPair) == 0x8,
              "GraphicsProjectedPointPair keeps its 32-bit layout");
static_assert(sizeof(FieldGridCell) == 0x80,
              "FieldGridCell keeps its 32-bit layout");
static_assert(sizeof(FieldGridAsset) == 0x280,
              "FieldGridAsset keeps its 32-bit layout");
static_assert(sizeof(UiScrollableViewportSize) == 0x8,
              "UiScrollableViewportSize keeps its 32-bit layout");
static_assert(sizeof(InGameCommandTextEditControlCC) == 0xCC &&
              offsetof(InGameCommandTextEditControlCC, activationSound) == 0x68,
              "InGameCommandTextEditControlCC keeps its 32-bit layout");
static_assert(sizeof(InGameCommandTextEntryPage2320) == 0x2320,
              "InGameCommandTextEntryPage2320 keeps its 32-bit layout");
static_assert(sizeof(SelectionInfoEntitySlots) == 0x80 &&
              offsetof(SelectionInfoEntitySlots, entries) == 0x0,
              "SelectionInfoEntitySlots keeps its 32-bit layout");
static_assert(sizeof(GraphicsCursorInputEvent18) == 0x18,
              "GraphicsCursorInputEvent18 keeps its 32-bit layout");
static_assert(sizeof(UiTextEditControl) == 0x80 &&
              offsetof(UiTextEditControl, activationSound) == 0x68,
              "UiTextEditControl keeps its 32-bit layout");
static_assert(sizeof(KeyboardInputEvent) == 0x8,
              "KeyboardInputEvent keeps its 32-bit layout");
static_assert(sizeof(LevelAssetResourceTables) == 0x24,
              "LevelAssetResourceTables keeps its 32-bit layout");
static_assert(sizeof(LevelAssetPathOffsets) == 0x28,
              "LevelAssetPathOffsets keeps its 32-bit layout");
static_assert(sizeof(LevelAssetHeader) == 0x200,
              "LevelAssetHeader keeps its 32-bit layout");
static_assert(sizeof(LevelPlayerSlotRecord) == 0x20,
              "LevelPlayerSlotRecord keeps its 32-bit layout");
static_assert(sizeof(LevelWorldSettings) == 0x90,
              "LevelWorldSettings keeps its 32-bit layout");
static_assert(sizeof(LevelAssetRuntimePrefix) == 0x370,
              "LevelAssetRuntimePrefix keeps its 32-bit layout");
static_assert(sizeof(FrontendRootPageState) == 0x26C4,
              "FrontendRootPageState keeps its 32-bit layout");
static_assert(sizeof(TerrainDirectionRecord) == 0x20,
              "TerrainDirectionRecord keeps its 32-bit layout");
static_assert(sizeof(UiTransferPacketHeader) == 0x10,
              "UiTransferPacketHeader keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket50001SessionAdvertisement) == 0xA0,
              "FrontendPacket50001SessionAdvertisement keeps its 32-bit layout");
static_assert(sizeof(UiTransferEndpointDescriptor) == 0x10,
              "UiTransferEndpointDescriptor keeps its 32-bit layout");
static_assert(sizeof(FrontendSessionDiscoveryRecord) == 0xB0,
              "FrontendSessionDiscoveryRecord keeps its 32-bit layout");
static_assert(sizeof(AiCandidateWorkspaceEntry) == 0x8,
              "AiCandidateWorkspaceEntry keeps its 32-bit layout");
static_assert(sizeof(UiCommandQueueRecord) == 0x10,
              "UiCommandQueueRecord keeps its 32-bit layout");
static_assert(sizeof(LevelArchivePathTemplate18) == 0x18,
              "LevelArchivePathTemplate18 keeps its 32-bit layout");
static_assert(sizeof(RuntimeModelClassPriorityTable24) == 0x60,
              "RuntimeModelClassPriorityTable24 keeps its 32-bit layout");
static_assert(sizeof(UiTextListControl) == 0x68 &&
              offsetof(UiTextListControl, rowTextSlots) == 0x50 &&
              offsetof(UiTextListControl, selectedRowSlot) == 0x60 &&
              offsetof(UiTextListControl, activationSound) == 0x64,
              "UiTextListControl keeps its 32-bit layout");
static_assert(sizeof(SprRelocationBlockHeader) == 0x20,
              "SprRelocationBlockHeader keeps its 32-bit layout");
static_assert(sizeof(InGameNotificationPayload) == 0x18,
              "InGameNotificationPayload keeps its 32-bit layout");
static_assert(sizeof(UiTransferSenderEndpointSlot) == 0x80,
              "UiTransferSenderEndpointSlot keeps its 32-bit layout");
static_assert(sizeof(FrontendModelPointerHitContext) == 0x118 &&
              offsetof(FrontendModelPointerHitContext, candidateModelListHead) == 0xD8 &&
              offsetof(FrontendModelPointerHitContext, selectedModelNode) == 0xE0 &&
              offsetof(FrontendModelPointerHitContext, keyboardFallback) == 0x100 &&
              offsetof(FrontendModelPointerHitContext, hoverCursorCallback) == 0x104 &&
              offsetof(FrontendModelPointerHitContext, heldButtonCursorCallback) == 0x108 &&
              offsetof(FrontendModelPointerHitContext, buttonPressCallback) == 0x10C &&
              offsetof(FrontendModelPointerHitContext, buttonDragCallback) == 0x110 &&
              offsetof(FrontendModelPointerHitContext, buttonReleaseCallback) == 0x114,
              "FrontendModelPointerHitContext keeps its 32-bit layout");
static_assert(sizeof(RecentTextHistorySlot) == 0x100,
              "RecentTextHistorySlot keeps its 32-bit layout");
static_assert(sizeof(RecentTextHistoryPointerList) == 0x24 &&
              offsetof(RecentTextHistoryPointerList, entries) == 0x4,
              "RecentTextHistoryPointerList keeps its 32-bit layout");
static_assert(sizeof(RecentTextHistoryView) == 0x100,
              "RecentTextHistoryView keeps its 32-bit layout");
static_assert(sizeof(InGameNotificationQueueRecord) == 0x20,
              "InGameNotificationQueueRecord keeps its 32-bit layout");
static_assert(sizeof(MdlSerializedNodeHeader) == 0x38,
              "MdlSerializedNodeHeader keeps its 32-bit layout");
static_assert(sizeof(WorldMotionSplineKeyframe) == 0x20,
              "WorldMotionSplineKeyframe keeps its 32-bit layout");
static_assert(sizeof(PatchArchivePathTemplate18) == 0x18,
              "PatchArchivePathTemplate18 keeps its 32-bit layout");
static_assert(sizeof(TerrainMaterialSuffixEntry) == 0x4,
              "TerrainMaterialSuffixEntry keeps its 32-bit layout");
static_assert(sizeof(UiListColumn) == 0x8,
              "UiListColumn keeps its 32-bit layout");
static_assert(sizeof(UiListControl) == 0x74 &&
              offsetof(UiListControl, rowSlots) == 0x50 &&
              offsetof(UiListControl, selectedRowSlot) == 0x60 &&
              offsetof(UiListControl, activationSound) == 0x68,
              "UiListControl keeps its 32-bit layout");
static_assert(sizeof(ScenarioCampaignDataPathTemplate2A) == 0x2A,
              "ScenarioCampaignDataPathTemplate2A keeps its 32-bit layout");
static_assert(sizeof(SprPointerRelocationRecord) == 0x40,
              "SprPointerRelocationRecord keeps its 32-bit layout");
static_assert(sizeof(GraphicsProjectedPoint2i) == 0x8,
              "GraphicsProjectedPoint2i keeps its 32-bit layout");
static_assert(sizeof(EntityPathingPriorityPair) == 0x8 &&
              offsetof(EntityPathingPriorityPair, entity) == 0x0,
              "EntityPathingPriorityPair keeps its 32-bit layout");
static_assert(sizeof(UiTransferPacket) == 0x40,
              "UiTransferPacket keeps its 32-bit layout");
static_assert(sizeof(UiScrollableControl) == 0x90,
              "UiScrollableControl keeps its 32-bit layout");
static_assert(sizeof(ModelRaycastTriangleDescriptor) == 0x40 &&
              offsetof(ModelRaycastTriangleDescriptor, vertex0) == 0x0 &&
              offsetof(ModelRaycastTriangleDescriptor, vertex1) == 0xC &&
              offsetof(ModelRaycastTriangleDescriptor, vertex2) == 0x18,
              "ModelRaycastTriangleDescriptor keeps its 32-bit layout");
static_assert(sizeof(GraphicsOffscreenSceneExtents) == 0x8,
              "GraphicsOffscreenSceneExtents keeps its 32-bit layout");
static_assert(sizeof(UiSettingsValueControl) == 0x68,
              "UiSettingsValueControl keeps its 32-bit layout");
static_assert(sizeof(SprGroupRelocationHeader) == 0x20,
              "SprGroupRelocationHeader keeps its 32-bit layout");
static_assert(sizeof(GraphicsOffscreenViewParameters) == 0x1C,
              "GraphicsOffscreenViewParameters keeps its 32-bit layout");
static_assert(sizeof(NetworkSessionContext) == 0x100,
              "NetworkSessionContext keeps its 32-bit layout");
static_assert(sizeof(InGameCameraCommandDispatchRecord) == 0xC,
              "InGameCameraCommandDispatchRecord keeps its 32-bit layout");
static_assert(offsetof(InGameCameraCommandDispatchRecord, action) == 0x8 && sizeof(InGameCameraKeyAction) == 4,
              "the camera key action keeps the 32-bit continuation slot");
static_assert(sizeof(InGameCameraCommandDispatchTable) == 0xD0,
              "InGameCameraCommandDispatchTable keeps its 32-bit layout");
static_assert(sizeof(SelectionPlayerPairRecord) == 0x8,
              "SelectionPlayerPairRecord keeps its 32-bit layout");
static_assert(sizeof(MovieFileHeader) == 0x200,
              "MovieFileHeader keeps its 32-bit layout");
static_assert(sizeof(UiRootNode) == 0x58 &&
              offsetof(UiRootNode, rootFlags) == 0x4C &&
              sizeof(UiRootNode::rootFlags) == 4 && alignof(decltype(UiRootNode::rootFlags)) == 4 &&
              offsetof(UiRootNode, callbacks) == 0x50 &&
              offsetof(UiRootNode, previousRoot) == 0x54,
              "UiRootNode keeps its 32-bit layout");
static_assert(sizeof(UiPanelControl) == 0x58,
              "UiPanelControl keeps its 32-bit layout");
static_assert(sizeof(UiResizableWindowControl) == 0x78,
              "UiResizableWindowControl keeps its 32-bit layout");
static_assert(sizeof(UiTitledWindowControl) == 0x54,
              "UiTitledWindowControl keeps its 32-bit layout");
static_assert(sizeof(InGameRuntimeRoot) == 0xC3E4 &&
              offsetof(InGameRuntimeRoot, activeEndMovieRuntime) == 0x22C &&
              offsetof(InGameRuntimeRoot, levelMovieRuntime) == 0x8D4 &&
              offsetof(InGameRuntimeRoot, worldOverlayCallback) == 0xB8C &&
              offsetof(InGameRuntimeRoot, localPlayerMarkedCells) == 0xBA0 &&
              offsetof(InGameRuntimeRoot, minimapTextureSource) == 0x9A7C &&
              offsetof(InGameRuntimeRoot, notificationButtonTextureSource) == 0x9B50 &&
              offsetof(InGameRuntimeRoot, selectionDetailEntity) == 0xA068,
              "InGameRuntimeRoot keeps its 32-bit layout");
static_assert(sizeof(MovieRuntime) == 0x224 &&
              offsetof(MovieRuntime, fileHeader) == 0xC0 &&
              offsetof(MovieRuntime, audioVoiceSet) == 0xCC &&
              offsetof(MovieRuntime, activeAudioBuffer) == 0xD0 &&
              offsetof(MovieRuntime, streamHandle) == 0xD4 &&
              offsetof(MovieRuntime, loadedVideoEnd) == 0xDC &&
              offsetof(MovieRuntime, refillSemaphore) == 0xF8,
              "MovieRuntime keeps its 32-bit layout");
static_assert(sizeof(UiRootCallbacks) == 0x14 &&
              offsetof(UiRootCallbacks, vetoClose) == 0x0 &&
              offsetof(UiRootCallbacks, frameUpdate) == 0x4 &&
              offsetof(UiRootCallbacks, method08) == 0x8 &&
              offsetof(UiRootCallbacks, keyboardFallback) == 0xC &&
              offsetof(UiRootCallbacks, pointerMissPolicy) == 0x10,
              "UiRootCallbacks keeps its 32-bit layout");
static_assert(sizeof(ScenarioLevelDataPathTemplate24) == 0x24,
              "ScenarioLevelDataPathTemplate24 keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeAttachmentSavedDescriptor) == 0x20 &&
              offsetof(ModelRuntimeAttachmentSavedDescriptor, sourceTransform) == 0x4,
              "ModelRuntimeAttachmentSavedDescriptor keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeSlotClassStateSerializedScalar) == 0x7C,
              "ModelRuntimeSlotClassStateSerializedScalar keeps its 32-bit layout");
static_assert(sizeof(ResourceRegistrationRecordSavedView) == 0x100,
              "ResourceRegistrationRecordSavedView keeps its 32-bit layout");
static_assert(sizeof(ResourceRegistrationPointerOrSavedOffset4) == 0x4 &&
              offsetof(ResourceRegistrationPointerOrSavedOffset4, runtimePointer) == 0x0,
              "ResourceRegistrationPointerOrSavedOffset4 keeps its 32-bit layout");
static_assert(sizeof(ResourceRegistrationRuntimeImageSavedView) == 0xDC &&
              offsetof(ResourceRegistrationRuntimeImageSavedView, records) == 0x58 &&
              offsetof(ResourceRegistrationRuntimeImageSavedView, tailRecord) == 0xD8,
              "ResourceRegistrationRuntimeImageSavedView keeps its 32-bit layout");
static_assert(sizeof(ResourceRegistrationRecord) == 0x100 &&
              offsetof(ResourceRegistrationRecord, paletteAsset) == 0x30 &&
              offsetof(ResourceRegistrationRecord, textureSet) == 0x34 &&
              offsetof(ResourceRegistrationRecord, spriteAsset) == 0x40,
              "ResourceRegistrationRecord keeps its 32-bit layout");
static_assert(sizeof(ArmyArticulatedRuntimeSlotView) == 0x120 &&
              offsetof(ArmyArticulatedRuntimeSlotView, definitionOrAsset) == 0x0 &&
              offsetof(ArmyArticulatedRuntimeSlotView, modelNodeRuntime) == 0x4 &&
              offsetof(ArmyArticulatedRuntimeSlotView, linkedEntityRuntime) == 0x8 &&
              offsetof(ArmyArticulatedRuntimeSlotView, commandTargetArmyRuntime) == 0x1C &&
              offsetof(ArmyArticulatedRuntimeSlotView, linkedArmyRuntimeOrSavedOffset) == 0x6C &&
              offsetof(ArmyArticulatedRuntimeSlotView, linkedArmyRuntime) == 0xF0,
              "ArmyArticulatedRuntimeSlotView keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeLinkedChildSlotMaskState) == 0x4,
              "ArmyRuntimeLinkedChildSlotMaskState keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeLinkedChildMaskArticulatedContactState) == 0x14,
              "ArmyRuntimeLinkedChildMaskArticulatedContactState keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeLinkedChildMaskSlotView) == 0x120 &&
              offsetof(ArmyRuntimeLinkedChildMaskSlotView, definitionOrAsset) == 0x0 &&
              offsetof(ArmyRuntimeLinkedChildMaskSlotView, modelNodeRuntime) == 0x4 &&
              offsetof(ArmyRuntimeLinkedChildMaskSlotView, linkedEntityRuntime) == 0x8 &&
              offsetof(ArmyRuntimeLinkedChildMaskSlotView, commandTargetArmyRuntime) == 0x1C &&
              offsetof(ArmyRuntimeLinkedChildMaskSlotView, linkedArmyRuntimeOrSavedOffset) == 0x6C &&
              offsetof(ArmyRuntimeLinkedChildMaskSlotView, linkedArmyRuntime) == 0xF0,
              "ArmyRuntimeLinkedChildMaskSlotView keeps its 32-bit layout");
static_assert(sizeof(ArmyGraphicsBinding) == 0x8 &&
              offsetof(ArmyGraphicsBinding, textureSet) == 0x0 &&
              offsetof(ArmyGraphicsBinding, paletteAsset) == 0x4,
              "ArmyGraphicsBinding keeps its 32-bit layout");
static_assert(sizeof(WorldPointXYQ12) == 0x8,
              "WorldPointXYQ12 keeps its 32-bit layout");
static_assert(sizeof(ArmyMovementRuntime) == 0x120 &&
              offsetof(ArmyMovementRuntime, entityRuntime) == 0x0 &&
              offsetof(ArmyMovementRuntime, modelNodeRuntime) == 0x4 &&
              offsetof(ArmyMovementRuntime, linkedEntityRuntime) == 0x8 &&
              offsetof(ArmyMovementRuntime, commandTargetArmyRuntime) == 0x1C &&
              offsetof(ArmyMovementRuntime, linkedArmyRuntimeOrSavedOffset) == 0x6C,
              "ArmyMovementRuntime keeps its 32-bit layout");
static_assert(sizeof(InGameFieldImageSaveContext58) == 0x58 &&
              offsetof(InGameFieldImageSaveContext58, fieldGridAsset) == 0x54,
              "InGameFieldImageSaveContext58 keeps its 32-bit layout");
static_assert(sizeof(RuntimeMaintenanceAudioRefreshCallbacks) == 0xC &&
              offsetof(RuntimeMaintenanceAudioRefreshCallbacks, army) == 0x0 &&
              offsetof(RuntimeMaintenanceAudioRefreshCallbacks, shot) == 0x4 &&
              offsetof(RuntimeMaintenanceAudioRefreshCallbacks, effect) == 0x8,
              "RuntimeMaintenanceAudioRefreshCallbacks keeps its 32-bit layout");
static_assert(sizeof(RuntimeMaintenancePrimaryUpdateCallbacks) == 0xC &&
              offsetof(RuntimeMaintenancePrimaryUpdateCallbacks, army) == 0x0 &&
              offsetof(RuntimeMaintenancePrimaryUpdateCallbacks, shot) == 0x4 &&
              offsetof(RuntimeMaintenancePrimaryUpdateCallbacks, effect) == 0x8,
              "RuntimeMaintenancePrimaryUpdateCallbacks keeps its 32-bit layout");
static_assert(sizeof(RuntimeMaintenanceTerrainStateRefreshCallbacks) == 0xC &&
              offsetof(RuntimeMaintenanceTerrainStateRefreshCallbacks, army) == 0x0 &&
              offsetof(RuntimeMaintenanceTerrainStateRefreshCallbacks, shot) == 0x4 &&
              offsetof(RuntimeMaintenanceTerrainStateRefreshCallbacks, effect) == 0x8,
              "RuntimeMaintenanceTerrainStateRefreshCallbacks keeps its 32-bit layout");
static_assert(sizeof(RuntimeMaintenanceOccupancyRebuildCallbacks) == 0xC &&
              offsetof(RuntimeMaintenanceOccupancyRebuildCallbacks, army) == 0x0 &&
              offsetof(RuntimeMaintenanceOccupancyRebuildCallbacks, shot) == 0x4 &&
              offsetof(RuntimeMaintenanceOccupancyRebuildCallbacks, effect) == 0x8,
              "RuntimeMaintenanceOccupancyRebuildCallbacks keeps its 32-bit layout");
static_assert(sizeof(RuntimeMaintenanceCallbackPhasesTyped) == 0x30,
              "RuntimeMaintenanceCallbackPhasesTyped keeps its 32-bit layout");
static_assert(sizeof(NetworkBackendInstanceDescriptorPrefix) == 0x38,
              "NetworkBackendInstanceDescriptorPrefix keeps its 32-bit layout");
static_assert(sizeof(PersistentSettingsRuntime) == 0x20C &&
              offsetof(PersistentSettingsRuntime, image) == 0x0,
              "PersistentSettingsRuntime keeps its 32-bit layout");
static_assert(sizeof(PersistentSettingsImage) == 0xC8,
              "PersistentSettingsImage keeps its 32-bit layout");
static_assert(sizeof(AiKnowledgeParameters) == 0x200,
              "AiKnowledgeParameters keeps its 32-bit layout");
static_assert(sizeof(AiKnowledgeDataImage) == 0x200,
              "AiKnowledgeDataImage keeps its 32-bit layout");
static_assert(sizeof(AiScoredSiteWorkspaceEntry) == 0x10 &&
              offsetof(AiScoredSiteWorkspaceEntry, cell) == 0xC,
              "AiScoredSiteWorkspaceEntry keeps its 32-bit layout");
static_assert(sizeof(AiTerrainFeatureWorkspaceEntry) == 0x10 &&
              offsetof(AiTerrainFeatureWorkspaceEntry, cell) == 0x0,
              "AiTerrainFeatureWorkspaceEntry keeps its 32-bit layout");
static_assert(sizeof(AiRuntimeWorkspaceEntry) == 0x8 &&
              offsetof(AiRuntimeWorkspaceEntry, modelRuntime) == 0x0,
              "AiRuntimeWorkspaceEntry keeps its 32-bit layout");
static_assert(sizeof(AiTargetWorkspaceEntry) == 0x10 &&
              offsetof(AiTargetWorkspaceEntry, modelRuntime) == 0x8 &&
              offsetof(AiTargetWorkspaceEntry, modelNode) == 0xC,
              "AiTargetWorkspaceEntry keeps its 32-bit layout");
static_assert(sizeof(AiLinkedDefinitionListView) == 0x40,
              "AiLinkedDefinitionListView keeps its 32-bit layout");
static_assert(sizeof(AiArmyScoreWeights) == 0x3C,
              "AiArmyScoreWeights keeps its 32-bit layout");
static_assert(sizeof(AiTechnologyPlanningCandidate) == 0x10 &&
              offsetof(AiTechnologyPlanningCandidate, sourceModelRuntime04) == 0x4,
              "AiTechnologyPlanningCandidate keeps its 32-bit layout");
static_assert(sizeof(SoundSampleAsset) == 0x200,
              "SoundSampleAsset keeps its 32-bit layout");
static_assert(sizeof(SelectionPointerArray32) == 0x80 &&
              offsetof(SelectionPointerArray32, entries) == 0x0,
              "SelectionPointerArray32 keeps its 32-bit layout");
static_assert(sizeof(SelectionPlayerRuntimeBlock) == 0x8118 &&
              offsetof(SelectionPlayerRuntimeBlock, terrainHeightScratchPlane) == 0x8088 &&
              offsetof(SelectionPlayerRuntimeBlock, terrainMaterialEditPlane) == 0x808C &&
              offsetof(SelectionPlayerRuntimeBlock, pendingPlacementArmyAsset) == 0x8098 &&
              offsetof(SelectionPlayerRuntimeBlock, technologyPageBuilding) == 0x80A0,
              "SelectionPlayerRuntimeBlock keeps its 32-bit layout");
static_assert(sizeof(PckArchiveHeader) == 0x200,
              "PckArchiveHeader keeps its 32-bit layout");
static_assert(sizeof(PckEntryHeader) == 0x200,
              "PckEntryHeader keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeSlotUnrebaseView) == 0x200,
              "ModelRuntimeSlotUnrebaseView keeps its 32-bit layout");
static_assert(sizeof(ArmyAssetRecordPrefix) == 0x10,
              "ArmyAssetRecordPrefix keeps its 32-bit layout");
static_assert(sizeof(ArmyAssetRecord) == 0x80 &&
              offsetof(ArmyAssetRecord, linkedRuntimeOrRecord10) == 0x10,
              "ArmyAssetRecord keeps its 32-bit layout");
static_assert(sizeof(ModelDefinition) == 0x280,
              "ModelDefinition keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionResolveView) == 0x280 &&
              offsetof(ModelDefinitionResolveView, shotDefinitionReference) == 0x2C &&
              offsetof(ModelDefinitionResolveView, waterEmitterEffectDefinitionReference) == 0x58 &&
              offsetof(ModelDefinitionResolveView, destructionEffect0) == 0x80 &&
              offsetof(ModelDefinitionResolveView, destructionEffect1) == 0x88 &&
              offsetof(ModelDefinitionResolveView, destructionEffect2) == 0x90 &&
              offsetof(ModelDefinitionResolveView, destructionEffect3) == 0x98 &&
              offsetof(ModelDefinitionResolveView, destructionEffect4) == 0xA0 &&
              offsetof(ModelDefinitionResolveView, destructionEffect5) == 0xA8 &&
              offsetof(ModelDefinitionResolveView, destructionEffect6) == 0xB0 &&
              offsetof(ModelDefinitionResolveView, destructionEffect7) == 0xB8 &&
              offsetof(ModelDefinitionResolveView, emitterShotDefinitionReference) == 0x168 &&
              offsetof(ModelDefinitionResolveView, emitterEffectDefinitionReference) == 0x174 &&
              offsetof(ModelDefinitionResolveView, removalEffectDefinitionReference) == 0x190 &&
              offsetof(ModelDefinitionResolveView, damageEffectDefinitionReference) == 0x254,
              "ModelDefinitionResolveView keeps its 32-bit layout");
static_assert(sizeof(ShotTerrainImpactDeformationColumns) == 0x104,
              "ShotTerrainImpactDeformationColumns keeps its 32-bit layout");
static_assert(sizeof(EffectModelRuntimeNode) == 0x100 &&
              offsetof(EffectModelRuntimeNode, effectRuntime) == 0x48 &&
              offsetof(EffectModelRuntimeNode, shadingRecord) == 0x5C &&
              offsetof(EffectModelRuntimeNode, modelRuntimeLinkOrSavedOffset) == 0x60 &&
              offsetof(EffectModelRuntimeNode, parentNode) == 0xC4 &&
              offsetof(EffectModelRuntimeNode, childNodes) == 0xCC,
              "EffectModelRuntimeNode keeps its 32-bit layout");
static_assert(sizeof(ResourceRegistrationRuntimeImage) == 0xDC &&
              offsetof(ResourceRegistrationRuntimeImage, records) == 0x58 &&
              offsetof(ResourceRegistrationRuntimeImage, tailRecord) == 0xD8,
              "ResourceRegistrationRuntimeImage keeps its 32-bit layout");
static_assert(sizeof(RuntimeHexSegmentImage) == 0x8 &&
              offsetof(RuntimeHexSegmentImage, image) == 0x0,
              "RuntimeHexSegmentImage keeps its 32-bit layout");
static_assert(sizeof(InGameEndConditionTriggerRecord8) == 0x8,
              "InGameEndConditionTriggerRecord8 keeps its 32-bit layout");
static_assert(sizeof(InGameScheduledConditionStatusAndKind4) == 0x4,
              "InGameScheduledConditionStatusAndKind4 keeps its 32-bit layout");
static_assert(sizeof(InGameScheduledConditionPayload0C) == 0xC,
              "InGameScheduledConditionPayload0C keeps its 32-bit layout");
static_assert(sizeof(InGameScheduledConditionRecord10) == 0x10,
              "InGameScheduledConditionRecord10 keeps its 32-bit layout");
static_assert(sizeof(InGameEndConditionTriggerRecord8ReferenceView) == 0x8,
              "InGameEndConditionTriggerRecord8ReferenceView keeps its 32-bit layout");
static_assert(sizeof(InGameConditionSchedule) == 0x480,
              "InGameConditionSchedule keeps its 32-bit layout");
static_assert(sizeof(MdlDefinitionSemanticPrefix) == 0x80,
              "MdlDefinitionSemanticPrefix keeps its 32-bit layout");
static_assert(sizeof(TextResourceLocaleCountHeader) == 0xB4,
              "TextResourceLocaleCountHeader keeps its 32-bit layout");
static_assert(sizeof(TextResourceAssetHeader) == 0x200,
              "TextResourceAssetHeader keeps its 32-bit layout");
static_assert(sizeof(TextResourceLocaleBlockPrefix) == 0x10,
              "TextResourceLocaleBlockPrefix keeps its 32-bit layout");
static_assert(sizeof(TextResourcePageBinding) == 0x8 &&
              offsetof(TextResourcePageBinding, selectedLocaleBlock) == 0x0 &&
              offsetof(TextResourcePageBinding, asset) == 0x4,
              "TextResourcePageBinding keeps its 32-bit layout");
static_assert(sizeof(WideNumberFormatState) == 0x158,
              "WideNumberFormatState keeps its 32-bit layout");
static_assert(sizeof(TextResourceOverrideTable) == 0x8000 &&
              offsetof(TextResourceOverrideTable, textPointers) == 0x4000,
              "TextResourceOverrideTable keeps its 32-bit layout");
static_assert(sizeof(RichTextExtent) == 0x8,
              "RichTextExtent keeps its 32-bit layout");
static_assert(sizeof(GraphicsDisplayMode) == 0x10,
              "GraphicsDisplayMode keeps its 32-bit layout");
static_assert(sizeof(GraphicsTexturePaletteEntry) == 0x8,
              "GraphicsTexturePaletteEntry keeps its 32-bit layout");
static_assert(sizeof(GraphicsProjectedVertexSource) == 0x38,
              "GraphicsProjectedVertexSource keeps its 32-bit layout");
static_assert(sizeof(GraphicsPaletteTextureSourceAsset) == 0x208,
              "GraphicsPaletteTextureSourceAsset keeps its 32-bit layout");
/* GraphicsTextureSet_AllocateMetadata reads the record table offset through either view of the same block */
static_assert(offsetof(GraphicsPaletteTextureSourceAsset, subresourceTableOffset) == 0xB8 &&
                  offsetof(GraphicsTextureSourceAsset, tableDescriptor) +
                          offsetof(GraphicsTextureSourceTableDescriptor, subresourceTableOffset) == 0xB8,
              "both texture source views hold subresourceTableOffset at +0xB8");
static_assert(sizeof(SoftwareBgraWordLanes) == 0x8,
              "SoftwareBgraWordLanes keeps its 32-bit layout");
static_assert(sizeof(SoftwarePixelFormatConfig) == 0x24,
              "SoftwarePixelFormatConfig keeps its 32-bit layout");
static_assert(sizeof(GraphicsCapturedTextureSourceAsset) == 0x224,
              "GraphicsCapturedTextureSourceAsset keeps its 32-bit layout");
static_assert(sizeof(SoftwareRasterScalarMmxLane) == 0x8,
              "SoftwareRasterScalarMmxLane keeps its 32-bit layout");
static_assert(sizeof(GraphicsPrimitiveVertexRaw) == 0x20,
              "GraphicsPrimitiveVertexRaw keeps its 32-bit layout");
static_assert(sizeof(GraphicsPrimitivePacket) == 0x80 &&
              offsetof(GraphicsPrimitivePacket, textureEntry) == 0x64,
              "GraphicsPrimitivePacket keeps its 32-bit layout");
static_assert(offsetof(GraphicsPrimitivePacket, renderFlags) == 0x68 && sizeof(GraphicsPrimitivePacket::renderFlags) == 4 &&
                  alignof(GraphicsPrimitivePacket) == 4,
              "GraphicsPrimitivePacket.renderFlags stays a 4-byte field at +0x68 (raster handler, GPU packet)");
static_assert(sizeof(GraphicsPrimitiveRadixBucket) == 0x4 &&
              offsetof(GraphicsPrimitiveRadixBucket, writeCursor) == 0x0,
              "GraphicsPrimitiveRadixBucket keeps its 32-bit layout");
static_assert(sizeof(GraphicsPrimitiveQueueNode) == 0x10 &&
              offsetof(GraphicsPrimitiveQueueNode, packet) == 0x4 &&
              offsetof(GraphicsPrimitiveQueueNode, next) == 0x8 &&
              offsetof(GraphicsPrimitiveQueueNode, previous) == 0xC,
              "GraphicsPrimitiveQueueNode keeps its 32-bit layout");
static_assert(sizeof(GraphicsTriangleInput) == 0x38 &&
              offsetof(GraphicsTriangleInput, vertex0) == 0x0 &&
              offsetof(GraphicsTriangleInput, vertex1) == 0xC &&
              offsetof(GraphicsTriangleInput, vertex2) == 0x18,
              "GraphicsTriangleInput keeps its 32-bit layout");
static_assert(offsetof(GraphicsTriangleInput, renderFlags) == 0x34 && sizeof(GraphicsTriangleInput::renderFlags) == 4 &&
                  alignof(GraphicsTriangleInput) == 4,
              "GraphicsTriangleInput.renderFlags stays the 4-byte MDL triangle flag word at +0x34");
static_assert(sizeof(ModelMeshGroupHeader) == 0x20 && alignof(ModelMeshGroupHeader) == 4 &&
                  offsetof(ModelMeshGroupHeader, meshCount) == 0x4 && offsetof(ModelMeshGroupHeader, groupFlags) == 0xC &&
                  sizeof(ModelMeshGroupHeader::groupFlags) == 4,
              "ModelMeshGroupHeader keeps the MDL mesh group header layout");
static_assert(sizeof(ModelMeshHeader) == 0x20 && alignof(ModelMeshHeader) == 4 && offsetof(ModelMeshHeader, groupMask) == 0x4 &&
                  offsetof(ModelMeshHeader, vertexCount) == 0x8 && offsetof(ModelMeshHeader, triangleCount) == 0xC &&
                  offsetof(ModelMeshHeader, flags) == 0x10 && sizeof(ModelMeshHeader::flags) == 4,
              "ModelMeshHeader keeps the MDL mesh header layout");
static_assert(sizeof(SoftwareRgbWordLanes) == 0x8,
              "SoftwareRgbWordLanes keeps its 32-bit layout");
static_assert(sizeof(SoftwareFramebufferAccess) == 0x10 &&
              offsetof(SoftwareFramebufferAccess, pixels) == 0xC,
              "SoftwareFramebufferAccess keeps its 32-bit layout");
static_assert(sizeof(SoftwareMaskRuntimeView) == 0x6C &&
              offsetof(SoftwareMaskRuntimeView, textureSource) == 0x50 &&
              offsetof(SoftwareMaskRuntimeView, maskPixels) == 0x60 &&
              offsetof(SoftwareMaskRuntimeView, blendedSourcePixels) == 0x64,
              "SoftwareMaskRuntimeView keeps its 32-bit layout");
static_assert(sizeof(SoftwarePixelPackTables) == 0xC00,
              "SoftwarePixelPackTables keeps its 32-bit layout");
static_assert(sizeof(SoftwareRasterTextureAddressState) == 0x18,
              "SoftwareRasterTextureAddressState keeps its 32-bit layout");
static_assert(sizeof(GraphicsCursorFrameRecord) == 0x20,
              "GraphicsCursorFrameRecord keeps its 32-bit layout");
static_assert(sizeof(GraphicsWideFixed) == 0x8,
              "GraphicsWideFixed keeps its 32-bit layout");
static_assert(sizeof(GraphicsFixedRect) == 0x10,
              "GraphicsFixedRect keeps its 32-bit layout");
static_assert(sizeof(GraphicsPrimitiveQueue) == 0x30 &&
              offsetof(GraphicsPrimitiveQueue, packetPool) == 0x8 &&
              offsetof(GraphicsPrimitiveQueue, radixScratchPool) == 0xC &&
              offsetof(GraphicsPrimitiveQueue, traversalCursor) == 0x10,
              "GraphicsPrimitiveQueue keeps its 32-bit layout");
static_assert(sizeof(GraphicsFixedVec2) == 0x8,
              "GraphicsFixedVec2 keeps its 32-bit layout");
static_assert(sizeof(SoftwareRasterTexCoordFixed2) == 0x8,
              "SoftwareRasterTexCoordFixed2 keeps its 32-bit layout");
static_assert(sizeof(GraphicsSceneBounds8) == 0x20,
              "GraphicsSceneBounds8 keeps its 32-bit layout");
static_assert(sizeof(SoftwareRasterColorFixed4) == 0x8,
              "SoftwareRasterColorFixed4 keeps its 32-bit layout");
static_assert(sizeof(SoftwareRasterScanState) == 0x74,
              "SoftwareRasterScanState keeps its 32-bit layout");
static_assert(sizeof(TerrainCompositeTextureRuntime) == 0x264,
              "TerrainCompositeTextureRuntime keeps its 32-bit layout");
static_assert(sizeof(GraphicsPaletteEntry) == 0x4,
              "GraphicsPaletteEntry keeps its 32-bit layout");
static_assert(sizeof(GridScratchCell) == 0x8,
              "GridScratchCell keeps its 32-bit layout");
static_assert(sizeof(TerrainScanSelectorUnion) == 0x4,
              "TerrainScanSelectorUnion keeps its 32-bit layout");
static_assert(sizeof(SoundCoefficientBlock) == 0x200,
              "SoundCoefficientBlock keeps its 32-bit layout");
static_assert(sizeof(AiFactionCandidateCacheState) == 0x20,
              "AiFactionCandidateCacheState keeps its 32-bit layout");
static_assert(sizeof(GameFactionRuntimeRecord) == 0x740 &&
              offsetof(GameFactionRuntimeRecord, runtimeGroupMembers8x32) == 0x2E0,
              "GameFactionRuntimeRecord keeps its 32-bit layout");
static_assert(sizeof(GameFactionRuntimeImageTail) == 0x20,
              "GameFactionRuntimeImageTail keeps its 32-bit layout");
static_assert(sizeof(GameFactionRuntimeImage) == 0x3A20,
              "GameFactionRuntimeImage keeps its 32-bit layout");
static_assert(sizeof(GameDataAuxState) == 0x100,
              "GameDataAuxState keeps its 32-bit layout");
static_assert(sizeof(KeyboardAsciiCaseTransformCallbackTable3) == 0xC &&
              offsetof(KeyboardAsciiCaseTransformCallbackTable3, compareCaseInsensitiveFlags) == 0x0 &&
              offsetof(KeyboardAsciiCaseTransformCallbackTable3, toUpper) == 0x4 &&
              offsetof(KeyboardAsciiCaseTransformCallbackTable3, toLower) == 0x8,
              "KeyboardAsciiCaseTransformCallbackTable3 keeps its 32-bit layout");
static_assert(sizeof(RandomGeneratorState) == 0xC &&
              offsetof(RandomGeneratorState, next) == 0x0,
              "RandomGeneratorState keeps its 32-bit layout");
static_assert(sizeof(UiTextButtonControl) == 0x60 &&
              offsetof(UiTextButtonControl, activationSound) == 0x5C,
              "UiTextButtonControl keeps its 32-bit layout");
static_assert(sizeof(UiNumericPairTextButton) == 0x68,
              "UiNumericPairTextButton keeps its 32-bit layout");
static_assert(sizeof(UiPayloadPairTextButton) == 0x68 &&
              offsetof(UiPayloadPairTextButton, firstPayload) == 0x60 &&
              offsetof(UiPayloadPairTextButton, secondPayload) == 0x64,
              "UiPayloadPairTextButton keeps its 32-bit layout");
static_assert(sizeof(UiCommandRuntimeRecordPrefix) == 0x2C &&
              offsetof(UiCommandRuntimeRecordPrefix, linkedRuntimeOrRecord10) == 0x10 &&
              offsetof(UiCommandRuntimeRecordPrefix, textureSource) == 0x1C,
              "UiCommandRuntimeRecordPrefix keeps its 32-bit layout");
static_assert(sizeof(UiSpriteButtonDrawOffsets) == 0x4,
              "UiSpriteButtonDrawOffsets keeps its 32-bit layout");
static_assert(sizeof(UiSpriteButtonControl) == 0x78 &&
              offsetof(UiSpriteButtonControl, primaryTextureSource) == 0x54 &&
              offsetof(UiSpriteButtonControl, activationSound) == 0x70 &&
              offsetof(UiSpriteButtonControl, alternateTextureSource) == 0x74,
              "UiSpriteButtonControl keeps its 32-bit layout");
static_assert(sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "UiCommandSpriteButtonControl keeps its 32-bit layout");
static_assert(sizeof(UiCatalogEntryControl) == 0x80,
              "UiCatalogEntryControl keeps its 32-bit layout");
static_assert(sizeof(UiDirtyRectEntry) == 0x18 &&
              offsetof(UiDirtyRectEntry, rootNode) == 0x0 &&
              offsetof(UiDirtyRectEntry, rootNodeCopy) == 0x4,
              "UiDirtyRectEntry keeps its 32-bit layout");
static_assert(sizeof(UiImageControl) == 0x6C &&
              offsetof(UiImageControl, textureSource) == 0x54 &&
              offsetof(UiImageControl, keyboardActivationSound) == 0x5C &&
              offsetof(UiImageControl, activeChild) == 0x64 &&
              offsetof(UiImageControl, pointerActivationSound) == 0x68,
              "UiImageControl keeps its 32-bit layout");
static_assert(sizeof(UiImageActionControl) == 0x68 &&
              offsetof(UiImageActionControl, textureSource) == 0x54,
              "UiImageActionControl keeps its 32-bit layout");
static_assert(sizeof(UiConditionalActionControl) == 0x60 &&
              offsetof(UiConditionalActionControl, textLines) == 0x5C,
              "UiConditionalActionControl keeps its 32-bit layout");
static_assert(sizeof(UiFramedTextButtonControl) == 0x60 &&
              offsetof(UiFramedTextButtonControl, activationSound) == 0x5C,
              "UiFramedTextButtonControl keeps its 32-bit layout");
static_assert(sizeof(UiWindowControl) == 0x68 &&
              offsetof(UiWindowControl, iconTextureSource) == 0x54,
              "UiWindowControl keeps its 32-bit layout");
static_assert(sizeof(UiActionHandlerPage) == 0x400 &&
              offsetof(UiActionHandlerPage, handlers) == 0x0,
              "UiActionHandlerPage keeps its 32-bit layout");
static_assert(sizeof(UiActionQueueEntry) == 0x8 &&
              offsetof(UiActionQueueEntry, source) == 0x4,
              "UiActionQueueEntry keeps its 32-bit layout");
static_assert(sizeof(UiTooltipState) == 0x10 &&
              offsetof(UiTooltipState, targetNode) == 0x4,
              "UiTooltipState keeps its 32-bit layout");
static_assert(sizeof(UiRuntimeRecord) == 0x100,
              "UiRuntimeRecord keeps its 32-bit layout");
static_assert(sizeof(UiTransferMailboxState) == 0x18 &&
              offsetof(UiTransferMailboxState, outgoingAllocation) == 0x0 &&
              offsetof(UiTransferMailboxState, receivedAllocation) == 0x8,
              "UiTransferMailboxState keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayResolutionOptionRow) == 0x68,
              "FrontendDisplayResolutionOptionRow keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayResolutionRows) == 0x410,
              "FrontendDisplayResolutionRows keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayAdapterOptionRow) == 0x68 &&
              offsetof(FrontendDisplayAdapterOptionRow, adapterDescriptionUtf16) == 0x0 &&
              offsetof(FrontendDisplayAdapterOptionRow, deviceNameUtf16) == 0x4,
              "FrontendDisplayAdapterOptionRow keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayAdapterRows) == 0x208,
              "FrontendDisplayAdapterRows keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayColorDepthOptionRow) == 0x68,
              "FrontendDisplayColorDepthOptionRow keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplayColorDepthRows) == 0x1A0,
              "FrontendDisplayColorDepthRows keeps its 32-bit layout");
static_assert(sizeof(FrontendDisplaySettingsPageOptionState) == 0x1010,
              "FrontendDisplaySettingsPageOptionState keeps its 32-bit layout");
static_assert(sizeof(UiSelectableOptionRow60) == 0x60,
              "UiSelectableOptionRow60 keeps its 32-bit layout");
static_assert(sizeof(FrontendTextureResolutionRows) == 0x120,
              "FrontendTextureResolutionRows keeps its 32-bit layout");
static_assert(sizeof(UiSelectableOptionRow68) == 0x68,
              "UiSelectableOptionRow68 keeps its 32-bit layout");
static_assert(sizeof(FrontendShadingResolutionRows) == 0x270,
              "FrontendShadingResolutionRows keeps its 32-bit layout");
static_assert(sizeof(FrontendGraphicsRuntimeSettingsPageState) == 0x167C,
              "FrontendGraphicsRuntimeSettingsPageState keeps its 32-bit layout");
static_assert(sizeof(InGamePlayerStatusTextSlot) == 0x80,
              "InGamePlayerStatusTextSlot keeps its 32-bit layout");
static_assert(sizeof(UiRequiredTextEditControl) == 0x80 &&
              offsetof(UiRequiredTextEditControl, activationSound) == 0x68,
              "UiRequiredTextEditControl keeps its 32-bit layout");
static_assert(sizeof(UiDisplayModeSelectionActionHandlerTable) == 0x50 &&
              offsetof(UiDisplayModeSelectionActionHandlerTable, handlers) == 0x0,
              "UiDisplayModeSelectionActionHandlerTable keeps its 32-bit layout");
static_assert(sizeof(FrontendUiActionHandlerPage20Prefix) == 0x180 && /* 0x160 + the 8 handlers of open-thandor */
              offsetof(FrontendUiActionHandlerPage20Prefix, handlers00_54) == 0x0 &&
              offsetof(FrontendUiActionHandlerPage20Prefix, scenarioCatalogRebuildCallbacks) == 0x154 &&
              offsetof(FrontendUiActionHandlerPage20Prefix, handlers5B_5F) == 0x16C,
              "FrontendUiActionHandlerPage20Prefix keeps its 32-bit layout");
static_assert(sizeof(InGameUiActionHandlerPage12Prefix28) == 0x70 &&
              offsetof(InGameUiActionHandlerPage12Prefix28, handlers) == 0x0,
              "InGameUiActionHandlerPage12Prefix28 keeps its 32-bit layout");
static_assert(sizeof(InGameUiActionHandlerPage10Prefix40) == 0xA0 &&
              offsetof(InGameUiActionHandlerPage10Prefix40, handlers) == 0x0,
              "InGameUiActionHandlerPage10Prefix40 keeps its 32-bit layout");
static_assert(sizeof(InGameUiCommandModeActionHandlerPage11) == 0x78 &&
              offsetof(InGameUiCommandModeActionHandlerPage11, handlers) == 0x0,
              "InGameUiCommandModeActionHandlerPage11 keeps its 32-bit layout");
static_assert(sizeof(FrontendTaskAssignmentControlOffsetRow) == 0x1C,
              "FrontendTaskAssignmentControlOffsetRow keeps its 32-bit layout");
static_assert(sizeof(FrontendTaskAssignmentControlOffsetTables) == 0x8C,
              "FrontendTaskAssignmentControlOffsetTables keeps its 32-bit layout");
static_assert(sizeof(ArmyPlacementContactCallbackTable5) == 0x14 &&
              offsetof(ArmyPlacementContactCallbackTable5, callbacks) == 0x0,
              "ArmyPlacementContactCallbackTable5 keeps its 32-bit layout");
static_assert(sizeof(ModelHierarchyEnergyDemand) == 0x8,
              "ModelHierarchyEnergyDemand keeps its 32-bit layout");
static_assert(sizeof(LocaleSystemState) == 0x100,
              "LocaleSystemState keeps its 32-bit layout");
static_assert(sizeof(PcxRgb24) == 0x3,
              "PcxRgb24 keeps its 32-bit layout");
static_assert(sizeof(TechnologyCategoryMasks) == 0x40,
              "TechnologyCategoryMasks keeps its 32-bit layout");
static_assert(sizeof(TechnologyRecord) == 0x40,
              "TechnologyRecord keeps its 32-bit layout");
static_assert(sizeof(TechnologyAssetHeader) == 0x200,
              "TechnologyAssetHeader keeps its 32-bit layout");
static_assert(sizeof(TechnologyAsset) == 0x240,
              "TechnologyAsset keeps its 32-bit layout");
static_assert(sizeof(GeneratedAssetRecordCountHeader) == 0xB4,
              "GeneratedAssetRecordCountHeader keeps its 32-bit layout");
static_assert(sizeof(ArmyAssetHeader) == 0x200,
              "ArmyAssetHeader keeps its 32-bit layout");
static_assert(sizeof(GeneratedAssetEntryCountHeader) == 0xB4,
              "GeneratedAssetEntryCountHeader keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSourceLifecycleCallbackTable) == 0xC &&
              offsetof(GraphicsTextureSourceLifecycleCallbackTable, releasePackage) == 0x0 &&
              offsetof(GraphicsTextureSourceLifecycleCallbackTable, clone) == 0x4 &&
              offsetof(GraphicsTextureSourceLifecycleCallbackTable, releaseClone) == 0x8,
              "GraphicsTextureSourceLifecycleCallbackTable keeps its 32-bit layout");
static_assert(sizeof(GraphicsPaletteAssetLifecycleCallbackTable) == 0xC &&
              offsetof(GraphicsPaletteAssetLifecycleCallbackTable, releasePackage) == 0x0 &&
              offsetof(GraphicsPaletteAssetLifecycleCallbackTable, clone) == 0x4 &&
              offsetof(GraphicsPaletteAssetLifecycleCallbackTable, releaseClone) == 0x8,
              "GraphicsPaletteAssetLifecycleCallbackTable keeps its 32-bit layout");
static_assert(sizeof(RomAssetHeader) == 0x200,
              "RomAssetHeader keeps its 32-bit layout");
static_assert(sizeof(RomRegistrySlot) == 0x8 &&
              offsetof(RomRegistrySlot, record) == 0x0 &&
              offsetof(RomRegistrySlot, runtimeRootNode) == 0x4,
              "RomRegistrySlot keeps its 32-bit layout");
static_assert(sizeof(RomAssetRecordPrefix) == 0xC,
              "RomAssetRecordPrefix keeps its 32-bit layout");
static_assert(sizeof(ModelPackedPointRecord) == 0x10,
              "ModelPackedPointRecord keeps its 32-bit layout");
static_assert(sizeof(ModelAssetHeader) == 0x200,
              "ModelAssetHeader keeps its 32-bit layout");
static_assert(sizeof(EffectAssetHeader) == 0x200,
              "EffectAssetHeader keeps its 32-bit layout");
static_assert(sizeof(ShotAssetHeader) == 0x200,
              "ShotAssetHeader keeps its 32-bit layout");
static_assert(sizeof(CommandLineArgumentMirrorState500) == 0x500,
              "CommandLineArgumentMirrorState500 keeps its 32-bit layout");
static_assert(sizeof(CommandLineWideArguments) == 0x600,
              "CommandLineWideArguments keeps its 32-bit layout");
static_assert(sizeof(FrontendPlayerFactionAssignmentState) == 0x10,
              "FrontendPlayerFactionAssignmentState keeps its 32-bit layout");
static_assert(sizeof(FrontendPlayerNameUtf16) == 0x28,
              "FrontendPlayerNameUtf16 keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsPageCommonPrefix) == 0x44 &&
              offsetof(FrontendNetworkSettingsPageCommonPrefix, nextSibling) == 0x0 &&
              offsetof(FrontendNetworkSettingsPageCommonPrefix, firstChild) == 0x4 &&
              offsetof(FrontendNetworkSettingsPageCommonPrefix, parent) == 0x8 &&
              offsetof(FrontendNetworkSettingsPageCommonPrefix, vtable) == 0xC,
              "FrontendNetworkSettingsPageCommonPrefix keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkBackendCommonPrefix) == 0x44,
              "FrontendNetworkBackendCommonPrefix keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsPrimaryPageStackView) == 0x250,
              "FrontendNetworkSettingsPrimaryPageStackView keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsGeneratedNameBufferView) == 0x250,
              "FrontendNetworkSettingsGeneratedNameBufferView keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsUiNodeView) == 0x250,
              "FrontendNetworkSettingsUiNodeView keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkGeneratedNamePrefix) == 0x2E,
              "FrontendNetworkGeneratedNamePrefix keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkBackendModeOverlap) == 0x44,
              "FrontendNetworkBackendModeOverlap keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkBackendModePageState) == 0x250,
              "FrontendNetworkBackendModePageState keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsTextEditView) == 0x250,
              "FrontendNetworkSettingsTextEditView keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsPointerListView) == 0x250,
              "FrontendNetworkSettingsPointerListView keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsPageCommonState) == 0x250,
              "FrontendNetworkSettingsPageCommonState keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsSecondaryPageStackView) == 0x250,
              "FrontendNetworkSettingsSecondaryPageStackView keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkSettingsControlView) == 0x250,
              "FrontendNetworkSettingsControlView keeps its 32-bit layout");
static_assert(sizeof(FrontendPlayerRuntimeRecord) == 0x13B0,
              "FrontendPlayerRuntimeRecord keeps its 32-bit layout");
static_assert(sizeof(FrontendCommandPacketRecord) == 0x20,
              "FrontendCommandPacketRecord keeps its 32-bit layout");
static_assert(sizeof(FrontendPlayerRemovalPacket10007) == 0x20,
              "FrontendPlayerRemovalPacket10007 keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket20002PlayerDescriptor) == 0x40,
              "FrontendPacket20002PlayerDescriptor keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket8000ASnapshotChunk) == 0x100,
              "FrontendPacket8000ASnapshotChunk keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10003JoinAck) == 0x20,
              "FrontendPacket10003JoinAck keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket40008LobbyRosterSnapshot) == 0x80,
              "FrontendPacket40008LobbyRosterSnapshot keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10032HostValue) == 0x20,
              "FrontendPacket10032HostValue keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10000Handshake) == 0x20,
              "FrontendPacket10000Handshake keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket30005PlayerSnapshot) == 0x60,
              "FrontendPacket30005PlayerSnapshot keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10012SyncPending) == 0x20,
              "FrontendPacket10012SyncPending keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10013HeartbeatAck) == 0x20,
              "FrontendPacket10013HeartbeatAck keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10004PlayerSnapshotRequest) == 0x20,
              "FrontendPacket10004PlayerSnapshotRequest keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10023StateAck) == 0x20,
              "FrontendPacket10023StateAck keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10022StatePending) == 0x20,
              "FrontendPacket10022StatePending keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10009SnapshotChunkRequest) == 0x20,
              "FrontendPacket10009SnapshotChunkRequest keeps its 32-bit layout");
static_assert(sizeof(FrontendPacket10006CapabilityHeartbeat) == 0x20,
              "FrontendPacket10006CapabilityHeartbeat keeps its 32-bit layout");
static_assert(sizeof(FrontendTransferPacketUnion) == 0x100,
              "FrontendTransferPacketUnion keeps its 32-bit layout");
static_assert(sizeof(ScenarioCatalogHeader) == 0x18,
              "ScenarioCatalogHeader keeps its 32-bit layout");
static_assert(sizeof(ScenarioCatalogRecord) == 0x100,
              "ScenarioCatalogRecord keeps its 32-bit layout");
static_assert(sizeof(FieldGridInterpolationCallbackTable5) == 0x14 &&
              offsetof(FieldGridInterpolationCallbackTable5, callbacks) == 0x0,
              "FieldGridInterpolationCallbackTable5 keeps its 32-bit layout");
static_assert(sizeof(FixedVectorQ12) == 0xC,
              "FixedVectorQ12 keeps its 32-bit layout");
static_assert(sizeof(FixedPlanarPointQ12) == 0x8,
              "FixedPlanarPointQ12 keeps its 32-bit layout");
static_assert(sizeof(PathingDestination) == 0x10,
              "PathingDestination keeps its 32-bit layout");
static_assert(sizeof(WorldCameraOrientation) == 0xC,
              "WorldCameraOrientation keeps its 32-bit layout");
static_assert(sizeof(ShotLaunchAngles) == 0x8,
              "ShotLaunchAngles keeps its 32-bit layout");
static_assert(sizeof(FixedRollAzimuthElevation) == 0xC,
              "FixedRollAzimuthElevation keeps its 32-bit layout");
static_assert(sizeof(WorldCameraPosition) == 0xC,
              "WorldCameraPosition keeps its 32-bit layout");
static_assert(sizeof(FixedLengthAzimuthElevation) == 0xC,
              "FixedLengthAzimuthElevation keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeOrderHandlerMatrix11x24) == 0x420 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, runtimeUpdate) == 0x0 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, classMethodD) == 0x60 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, modelUnrebase) == 0xC0 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, modelRebaseOrLoadRepair) == 0x120 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, modelClassInitialize) == 0x180 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, modelReleaseOrCommit) == 0x1E0 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, placementValidation) == 0x240 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, placementAssetClassDispatch) == 0x2A0 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, classCommand) == 0x300 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, gridInfluenceAdd) == 0x360 &&
              offsetof(ArmyRuntimeOrderHandlerMatrix11x24, gridInfluenceRemove) == 0x3C0,
              "ArmyRuntimeOrderHandlerMatrix11x24 keeps its 32-bit layout");
static_assert(sizeof(FixedAzimuthElevationRoll) == 0xC,
              "FixedAzimuthElevationRoll keeps its 32-bit layout");
static_assert(sizeof(AiSecondaryWorkspaceDistanceSelection) == 0x8 &&
              offsetof(AiSecondaryWorkspaceDistanceSelection, selectedEntry) == 0x4,
              "AiSecondaryWorkspaceDistanceSelection keeps its 32-bit layout");
static_assert(sizeof(TerrainOccupancyResolvedMasks) == 0xC,
              "TerrainOccupancyResolvedMasks keeps its 32-bit layout");
static_assert(sizeof(AiGeneralSiteDistanceSelection) == 0x8 &&
              offsetof(AiGeneralSiteDistanceSelection, selectedEntry) == 0x4,
              "AiGeneralSiteDistanceSelection keeps its 32-bit layout");
static_assert(sizeof(GridPathBestUnreachableCell) == 0x8,
              "GridPathBestUnreachableCell keeps its 32-bit layout");
static_assert(sizeof(AiStrategicClassSelection) == 0x8,
              "AiStrategicClassSelection keeps its 32-bit layout");
static_assert(sizeof(FrontendCreditsUiStateView) == 0x240,
              "FrontendCreditsUiStateView keeps its 32-bit layout");
static_assert(sizeof(CursorPointerEvent) == 0x14,
              "CursorPointerEvent keeps its 32-bit layout");
static_assert(sizeof(GraphicsTextureSourceHeaderView) == 0xBC,
              "GraphicsTextureSourceHeaderView keeps its 32-bit layout");
static_assert(sizeof(InGameLevelSaveWorldView) == 0x180,
              "InGameLevelSaveWorldView keeps its 32-bit layout");
static_assert(sizeof(ModelProjectedBoundsPixels) == 0x10,
              "ModelProjectedBoundsPixels keeps its 32-bit layout");
static_assert(sizeof(FrameProviderResult) == 0x8 &&
              offsetof(FrameProviderResult, frameOrError) == 0x0,
              "FrameProviderResult keeps its 32-bit layout");
static_assert(sizeof(RuntimeModelFactionPrefix) == 0x10 &&
              offsetof(RuntimeModelFactionPrefix, modelRuntime) == 0x0 &&
              offsetof(RuntimeModelFactionPrefix, modelNode) == 0x4,
              "RuntimeModelFactionPrefix keeps its 32-bit layout");
static_assert(sizeof(TerrainProjectedVertexWorkRecord) == 0x80 &&
              offsetof(TerrainProjectedVertexWorkRecord, secondaryOffset) == 0x54,
              "TerrainProjectedVertexWorkRecord keeps its 32-bit layout");
static_assert(sizeof(TriangleBarycentricWeightsQ12) == 0x8,
              "TriangleBarycentricWeightsQ12 keeps its 32-bit layout");
static_assert(sizeof(UiSelectionGeometryControl) == 0x70 &&
              offsetof(UiSelectionGeometryControl, textureSource) == 0x60,
              "UiSelectionGeometryControl keeps its 32-bit layout");
static_assert(sizeof(RomSerializedNodeReferenceOrSavedOffset4) == 0x4 &&
              offsetof(RomSerializedNodeReferenceOrSavedOffset4, node) == 0x0,
              "RomSerializedNodeReferenceOrSavedOffset4 keeps its 32-bit layout");
static_assert(sizeof(RomSerializedNodeHeader) == 0x34,
              "RomSerializedNodeHeader keeps its 32-bit layout");
static_assert(sizeof(InGameRuntimeRootUiGridView) == 0xC3E4 &&
              offsetof(InGameRuntimeRootUiGridView, activeEndMovieRuntime) == 0x22C &&
              offsetof(InGameRuntimeRootUiGridView, levelMovieRuntime) == 0x8D4 &&
              offsetof(InGameRuntimeRootUiGridView, worldOverlayCallback) == 0xB8C &&
              offsetof(InGameRuntimeRootUiGridView, localPlayerMarkedCells) == 0xBA0 &&
              offsetof(InGameRuntimeRootUiGridView, diplomacyPanelSoundVoiceSet) == 0x4D74 &&
              offsetof(InGameRuntimeRootUiGridView, buildCatalogSoundVoiceSet) == 0x5E1C &&
              offsetof(InGameRuntimeRootUiGridView, specialBuildCatalogSoundVoiceSet) == 0x76E8 &&
              offsetof(InGameRuntimeRootUiGridView, armyStockSoundVoiceSet) == 0x8CB4 &&
              offsetof(InGameRuntimeRootUiGridView, selectionDetailEntity) == 0xA068,
              "InGameRuntimeRootUiGridView keeps its 32-bit layout");
static_assert(sizeof(InGameRuntimeRootFrameView) == 0x44C4,
              "InGameRuntimeRootFrameView keeps its 32-bit layout");
static_assert(sizeof(FrontendModelPointerContext) == 0x17C &&
              offsetof(FrontendModelPointerContext, fieldGrid) == 0x54 &&
              offsetof(FrontendModelPointerContext, activePrimitiveQueue) == 0xC8 &&
              offsetof(FrontendModelPointerContext, renderSpinLock) == 0xD0 &&
              offsetof(FrontendModelPointerContext, renderSpinLockReleaseCallback) == 0xD4 &&
              offsetof(FrontendModelPointerContext, candidateModelListHead) == 0xD8 &&
              offsetof(FrontendModelPointerContext, selectedModelNode) == 0xE0 &&
              offsetof(FrontendModelPointerContext, selectedOverlayEntity) == 0xFC &&
              offsetof(FrontendModelPointerContext, keyboardFallback) == 0x100 &&
              offsetof(FrontendModelPointerContext, hoverCursorCallback) == 0x104 &&
              offsetof(FrontendModelPointerContext, heldButtonCursorCallback) == 0x108 &&
              offsetof(FrontendModelPointerContext, buttonPressCallback) == 0x10C &&
              offsetof(FrontendModelPointerContext, buttonDragCallback) == 0x110 &&
              offsetof(FrontendModelPointerContext, buttonReleaseCallback) == 0x114 &&
              offsetof(FrontendModelPointerContext, rightClickCallback) == 0x118 &&
              offsetof(FrontendModelPointerContext, renderPhaseCallback) == 0x15C &&
              offsetof(FrontendModelPointerContext, terrainMarkerCoordinatePairs) == 0x170,
              "FrontendModelPointerContext keeps its 32-bit layout");
static_assert(sizeof(GeneratedTextureRenderContextView) == 0xCC &&
              offsetof(GeneratedTextureRenderContextView, fieldGrid) == 0x54 &&
              offsetof(GeneratedTextureRenderContextView, projectedPointBlockPool) == 0xC8,
              "GeneratedTextureRenderContextView keeps its 32-bit layout");
static_assert(sizeof(ArmyModelTreeNodeAddressView) == 0x18,
              "ArmyModelTreeNodeAddressView keeps its 32-bit layout");
static_assert(sizeof(WorldRuntimeExtendedMapControlView) == 0x170 &&
              offsetof(WorldRuntimeExtendedMapControlView, fieldGrid) == 0x54 &&
              offsetof(WorldRuntimeExtendedMapControlView, objectArray) == 0x58 &&
              offsetof(WorldRuntimeExtendedMapControlView, dwordArray) == 0xC0 &&
              offsetof(WorldRuntimeExtendedMapControlView, tickSpinLock) == 0xD0 &&
              offsetof(WorldRuntimeExtendedMapControlView, simulationAndNetworkTickCallback) == 0xD4 &&
              offsetof(WorldRuntimeExtendedMapControlView, ownerListHead) == 0xD8,
              "WorldRuntimeExtendedMapControlView keeps its 32-bit layout");
static_assert(sizeof(ArmyRuntimeClassUpdate21DefinitionView) == 0x27C &&
              offsetof(ArmyRuntimeClassUpdate21DefinitionView, rootNode) == 0x64 &&
              offsetof(ArmyRuntimeClassUpdate21DefinitionView, removalEffect) == 0x190,
              "ArmyRuntimeClassUpdate21DefinitionView keeps its 32-bit layout");
static_assert(sizeof(FixedTriangleJointAngles) == 0x8,
              "FixedTriangleJointAngles keeps its 32-bit layout");
static_assert(sizeof(SelectionPanelCellAdvance) == 0x8,
              "SelectionPanelCellAdvance keeps its 32-bit layout");
static_assert(sizeof(GeneratedTextureSampleWorkRecord) == 0x18,
              "GeneratedTextureSampleWorkRecord keeps its 32-bit layout");
static_assert(sizeof(GeneratedTextureScratchRuntime) == 0x1A8,
              "GeneratedTextureScratchRuntime keeps its 32-bit layout");
static_assert(sizeof(InGameMissionHelpTextPanel) == 0xE8,
              "InGameMissionHelpTextPanel keeps its 32-bit layout");
static_assert(sizeof(InGameMissionHelpRootView) == 0x43DC,
              "InGameMissionHelpRootView keeps its 32-bit layout");
static_assert(sizeof(FrontendPointerHintControl) == 0x58 &&
              offsetof(FrontendPointerHintControl, commandStream) == 0x54,
              "FrontendPointerHintControl keeps its 32-bit layout");
static_assert(sizeof(FrontendPointerSceneRuntimeView) == 0x43E8 &&
              offsetof(FrontendPointerSceneRuntimeView, fieldGrid) == 0x54 &&
              offsetof(FrontendPointerSceneRuntimeView, activePrimitiveQueue) == 0xC8 &&
              offsetof(FrontendPointerSceneRuntimeView, renderSpinLock) == 0xD0 &&
              offsetof(FrontendPointerSceneRuntimeView, renderSpinLockReleaseCallback) == 0xD4 &&
              offsetof(FrontendPointerSceneRuntimeView, candidateModelListHead) == 0xD8 &&
              offsetof(FrontendPointerSceneRuntimeView, selectedModelNode) == 0xE0 &&
              offsetof(FrontendPointerSceneRuntimeView, selectedOverlayEntity) == 0xFC &&
              offsetof(FrontendPointerSceneRuntimeView, keyboardFallback) == 0x100 &&
              offsetof(FrontendPointerSceneRuntimeView, hoverCursorCallback) == 0x104 &&
              offsetof(FrontendPointerSceneRuntimeView, heldButtonCursorCallback) == 0x108 &&
              offsetof(FrontendPointerSceneRuntimeView, buttonPressCallback) == 0x10C &&
              offsetof(FrontendPointerSceneRuntimeView, buttonDragCallback) == 0x110 &&
              offsetof(FrontendPointerSceneRuntimeView, buttonReleaseCallback) == 0x114 &&
              offsetof(FrontendPointerSceneRuntimeView, terrainMarkerCoordinatePairs) == 0x170,
              "FrontendPointerSceneRuntimeView keeps its 32-bit layout");
static_assert(sizeof(FrontendScenarioSelectionPageView) == 0x26C4,
              "FrontendScenarioSelectionPageView keeps its 32-bit layout");
static_assert(sizeof(FrontendRootResourceSlots) == 0x5954 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_05E0) == 0x5E0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_0644) == 0x644 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_06A4) == 0x6A4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_0704) == 0x704 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_0764) == 0x764 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_0A8C) == 0xA8C &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_0AE4) == 0xAE4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_0B48) == 0xB48 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_0BA8) == 0xBA8 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_0C68) == 0xC68 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_1C8C) == 0x1C8C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_1CF0) == 0x1CF0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_1D50) == 0x1D50 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_1DB0) == 0x1DB0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_1E10) == 0x1E10 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_1E70) == 0x1E70 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_2024) == 0x2024 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_21EC) == 0x21EC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_23CC) == 0x23CC &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_24F8) == 0x24F8 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_255C) == 0x255C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_25BC) == 0x25BC &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_2670) == 0x2670 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_26D4) == 0x26D4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_2790) == 0x2790 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_27F0) == 0x27F0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_2850) == 0x2850 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_28B0) == 0x28B0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2AE0) == 0x2AE0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2B40) == 0x2B40 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2BF4) == 0x2BF4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2C54) == 0x2C54 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2CB4) == 0x2CB4 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_2D0C) == 0x2D0C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_2D70) == 0x2D70 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_2DD0) == 0x2DD0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2EE0) == 0x2EE0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2F48) == 0x2F48 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_2FB0) == 0x2FB0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3018) == 0x3018 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3080) == 0x3080 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_313C) == 0x313C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_31A4) == 0x31A4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_320C) == 0x320C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3274) == 0x3274 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_32DC) == 0x32DC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3344) == 0x3344 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_33AC) == 0x33AC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3414) == 0x3414 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_347C) == 0x347C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_34E4) == 0x34E4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_35A0) == 0x35A0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3608) == 0x3608 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3670) == 0x3670 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_36D8) == 0x36D8 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_3738) == 0x3738 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_379C) == 0x379C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3858) == 0x3858 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_390C) == 0x390C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3974) == 0x3974 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_39DC) == 0x39DC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3A44) == 0x3A44 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3AAC) == 0x3AAC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3B14) == 0x3B14 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_3C98) == 0x3C98 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3D4C) == 0x3D4C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3DAC) == 0x3DAC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3E0C) == 0x3E0C &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_3E64) == 0x3E64 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_3EC8) == 0x3EC8 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3F84) == 0x3F84 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_3FE4) == 0x3FE4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet4_4044) == 0x4044 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_41C0) == 0x41C0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_433C) == 0x433C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_44B8) == 0x44B8 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_4634) == 0x4634 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_485C) == 0x485C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_491C) == 0x491C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_497C) == 0x497C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_49DC) == 0x49DC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_4AD4) == 0x4AD4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_4BD0) == 0x4BD0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_4DC4) == 0x4DC4 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_4EB0) == 0x4EB0 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_4F30) == 0x4F30 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_4FF0) == 0x4FF0 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_5050) == 0x5050 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_50BC) == 0x50BC &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_514C) == 0x514C &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet5_5210) == 0x5210 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_53D8) == 0x53D8 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_5498) == 0x5498 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_54F8) == 0x54F8 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_5558) == 0x5558 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet6_5654) == 0x5654 &&
              offsetof(FrontendRootResourceSlots, menuTextureSource_5720) == 0x5720 &&
              offsetof(FrontendRootResourceSlots, buttonVoiceSet3_57E0) == 0x57E0,
              "FrontendRootResourceSlots keeps its 32-bit layout");
static_assert(sizeof(FrontendTaskAssignmentPageInitView) == 0x26C4,
              "FrontendTaskAssignmentPageInitView keeps its 32-bit layout");
static_assert(sizeof(FieldGridCellSaveImageView) == 0x80,
              "FieldGridCellSaveImageView keeps its 32-bit layout");
static_assert(sizeof(AiStructureWorkspaceEntry) == 0x8 &&
              offsetof(AiStructureWorkspaceEntry, runtimeSlotAddressOrZero) == 0x0,
              "AiStructureWorkspaceEntry keeps its 32-bit layout");
static_assert(sizeof(FrontendLoadedLevelPathOffsets) == 0x28,
              "FrontendLoadedLevelPathOffsets keeps its 32-bit layout");
static_assert(sizeof(FrontendLoadedLevelHeader) == 0x200,
              "FrontendLoadedLevelHeader keeps its 32-bit layout");
static_assert(sizeof(FrontendLoadedLevelAsset) == 0x370,
              "FrontendLoadedLevelAsset keeps its 32-bit layout");
static_assert(sizeof(FrontendNetworkListsRuntimeView) == 0x5650,
              "FrontendNetworkListsRuntimeView keeps its 32-bit layout");
static_assert(sizeof(FrontendResultsFactionWeightPair) == 0x8,
              "FrontendResultsFactionWeightPair keeps its 32-bit layout");
static_assert(sizeof(FrontendResultsColumnSequenceControl) == 0x68 &&
              offsetof(FrontendResultsColumnSequenceControl, factionWeightRaster) == 0x50,
              "FrontendResultsColumnSequenceControl keeps its 32-bit layout");
static_assert(sizeof(FrontendResultsTable<8>) == 0x84 &&
              offsetof(FrontendResultsTable<8>, factionWeightRaster) == 0x50,
              "FrontendResultsTable<8> keeps its 32-bit layout");
static_assert(sizeof(FrontendResultsTable<6>) == 0x7C && offsetof(FrontendResultsTable<6>, modeFlags) == 0x4C &&
              offsetof(FrontendResultsTable<6>, factionWeightRaster) == 0x50 &&
              offsetof(FrontendResultsTable<6>, columnTypeCount) == 0x54 &&
              offsetof(FrontendResultsTable<6>, rowCount) == 0x58 &&
              offsetof(FrontendResultsTable<6>, headerBaselineOffsetPixels) == 0x5C &&
              offsetof(FrontendResultsTable<6>, rowAdvancePixels) == 0x60 &&
              offsetof(FrontendResultsTable<6>, columnTypes) == 0x64,
              "FrontendResultsTable<6> keeps its 32-bit layout");
static_assert(offsetof(FrontendResultsColumnSequenceControl, columnTypeCount) == 0x54 &&
              offsetof(FrontendResultsColumnSequenceControl, rowCount) == 0x58 &&
              offsetof(FrontendResultsColumnSequenceControl, headerBaselineOffsetPixels) == 0x5C &&
              offsetof(FrontendResultsColumnSequenceControl, rowAdvancePixels) == 0x60 &&
              offsetof(FrontendResultsColumnSequenceControl, columnTypes0) == 0x64,
              "FrontendResultsColumnSequenceControl is the one-slot view of FrontendResultsTable<N>");
static_assert(sizeof(ScenarioCatalogDisplayRecord) == 0x100 &&
              offsetof(ScenarioCatalogDisplayRecord, titleResolvedText) == 0x56 &&
              offsetof(ScenarioCatalogDisplayRecord, subtitleResolvedText) == 0x66 &&
              offsetof(ScenarioCatalogDisplayRecord, scenarioResolvedText) == 0x76 &&
              offsetof(ScenarioCatalogDisplayRecord, modeResolvedText) == 0x86,
              "ScenarioCatalogDisplayRecord keeps its 32-bit layout");
static_assert(sizeof(ScenarioCatalogSaveRecord) == 0x100,
              "ScenarioCatalogSaveRecord keeps its 32-bit layout");
static_assert(sizeof(WorldOwnerListNode) == 0x100 &&
              offsetof(WorldOwnerListNode, previousNode) == 0x0 &&
              offsetof(WorldOwnerListNode, nextNode) == 0x4 &&
              offsetof(WorldOwnerListNode, ownerWorld) == 0x8 &&
              offsetof(WorldOwnerListNode, runtimePayload) == 0x48,
              "WorldOwnerListNode keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimePlacementValidationView) == 0x200 &&
              offsetof(ModelRuntimePlacementValidationView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimePlacementValidationView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimePlacementValidationView, ownerArmyRuntime) == 0x8,
              "ModelRuntimePlacementValidationView keeps its 32-bit layout");
static_assert(sizeof(ArmyWeaponDefinitionView) == 0x68 &&
              offsetof(ArmyWeaponDefinitionView, shotDefinition) == 0x2C &&
              offsetof(ArmyWeaponDefinitionView, rootNode) == 0x64,
              "ArmyWeaponDefinitionView keeps its 32-bit layout");
static_assert(sizeof(RuntimeCollisionQueryView) == 0xF4 &&
              offsetof(RuntimeCollisionQueryView, modelDefinition) == 0x0 &&
              offsetof(RuntimeCollisionQueryView, modelNodeRuntime) == 0x4 &&
              offsetof(RuntimeCollisionQueryView, linkedRuntime) == 0xF0,
              "RuntimeCollisionQueryView keeps its 32-bit layout");
static_assert(sizeof(EntityPathingRouteEntityRuntimeView) == 0x10 &&
              offsetof(EntityPathingRouteEntityRuntimeView, modelDefinition) == 0x0 &&
              offsetof(EntityPathingRouteEntityRuntimeView, modelNode) == 0x4 &&
              offsetof(EntityPathingRouteEntityRuntimeView, movementRuntime) == 0x8,
              "EntityPathingRouteEntityRuntimeView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeUpdateView) == 0x200 &&
              offsetof(ModelRuntimeUpdateView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeUpdateView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeUpdateView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeUpdateView keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionClass14PlacementView) == 0x280,
              "ModelDefinitionClass14PlacementView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimePlacementClass14View) == 0x200 &&
              offsetof(ModelRuntimePlacementClass14View, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimePlacementClass14View, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimePlacementClass14View, ownerArmyRuntime) == 0x8,
              "ModelRuntimePlacementClass14View keeps its 32-bit layout");
static_assert(sizeof(InGameTargetingRootTraversalView) == 0x9E60,
              "InGameTargetingRootTraversalView keeps its 32-bit layout");
static_assert(sizeof(ShotModelRuntimeNode) == 0x100 &&
              offsetof(ShotModelRuntimeNode, shotRuntime) == 0x48 &&
              offsetof(ShotModelRuntimeNode, shadingRecord) == 0x5C &&
              offsetof(ShotModelRuntimeNode, modelRuntimeLinkOrSavedOffset) == 0x60 &&
              offsetof(ShotModelRuntimeNode, parentNode) == 0xC4 &&
              offsetof(ShotModelRuntimeNode, childNodes) == 0xCC,
              "ShotModelRuntimeNode keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeClass14UpdateView) == 0x200 &&
              offsetof(ModelRuntimeClass14UpdateView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeClass14UpdateView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeClass14UpdateView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeClass14UpdateView keeps its 32-bit layout");
static_assert(sizeof(ModelRaycastNearestNodeOrScratch4) == 0x4 &&
              offsetof(ModelRaycastNearestNodeOrScratch4, nearestModelNode) == 0x0,
              "ModelRaycastNearestNodeOrScratch4 keeps its 32-bit layout");
static_assert(sizeof(WorldPositionXY) == 0x8,
              "WorldPositionXY keeps its 32-bit layout");
static_assert(sizeof(FixedLengthAngle) == 0x8,
              "FixedLengthAngle keeps its 32-bit layout");
static_assert(sizeof(TerrainPlacementResult) == 0x8,
              "TerrainPlacementResult keeps its 32-bit layout");
static_assert(sizeof(TerrainClassPlacementAndOverlayCallbackTable10) == 0x28 &&
              offsetof(TerrainClassPlacementAndOverlayCallbackTable10, placementTests) == 0x0 &&
              offsetof(TerrainClassPlacementAndOverlayCallbackTable10, overlayCallbacks) == 0x14,
              "TerrainClassPlacementAndOverlayCallbackTable10 keeps its 32-bit layout");
static_assert(sizeof(TerrainProjectedRowSpan) == 0x8,
              "TerrainProjectedRowSpan keeps its 32-bit layout");
static_assert(sizeof(ModelRelativeDirectionAngles) == 0x8,
              "ModelRelativeDirectionAngles keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeLinkedChildSpawnInheritedState) == 0xC,
              "ModelRuntimeLinkedChildSpawnInheritedState keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionVerticalDeploymentView) == 0x280,
              "ModelDefinitionVerticalDeploymentView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeTimedEffectsUpdateView) == 0x200 &&
              offsetof(ModelRuntimeTimedEffectsUpdateView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeTimedEffectsUpdateView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeTimedEffectsUpdateView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeTimedEffectsUpdateView keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionTimedEffectsUpdateView) == 0x280,
              "ModelDefinitionTimedEffectsUpdateView keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionLinkedChildStateView) == 0x280,
              "ModelDefinitionLinkedChildStateView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeTimedTargetState) == 0x28,
              "ModelRuntimeTimedTargetState keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionDestroyEffectsView) == 0x280,
              "ModelDefinitionDestroyEffectsView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeDestroyEffectsView) == 0x200 &&
              offsetof(ModelRuntimeDestroyEffectsView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeDestroyEffectsView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeDestroyEffectsView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeDestroyEffectsView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeGroundMovementTrackView) == 0x200 &&
              offsetof(ModelRuntimeGroundMovementTrackView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeGroundMovementTrackView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeGroundMovementTrackView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeGroundMovementTrackView keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionGroundMovementTrackView) == 0x280,
              "ModelDefinitionGroundMovementTrackView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeLinkedChildPendingSpawnCounts) == 0x4,
              "ModelRuntimeLinkedChildPendingSpawnCounts keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeLinkedChildBuildState) == 0x18,
              "ModelRuntimeLinkedChildBuildState keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeLinkedChildSpawnAndBuildView) == 0x200 &&
              offsetof(ModelRuntimeLinkedChildSpawnAndBuildView, modelDefinition) == 0x0,
              "ModelRuntimeLinkedChildSpawnAndBuildView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeGroundMovementSteeringView) == 0x200 &&
              offsetof(ModelRuntimeGroundMovementSteeringView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeGroundMovementSteeringView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeGroundMovementSteeringView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeGroundMovementSteeringView keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionGroundMovementSteeringView) == 0x280,
              "ModelDefinitionGroundMovementSteeringView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeTimedTargetLinkState) == 0x24 &&
              offsetof(ModelRuntimeTimedTargetLinkState, selectedTargetModelRuntime) == 0x0 &&
              offsetof(ModelRuntimeTimedTargetLinkState, matchingActiveShotRuntime) == 0x4,
              "ModelRuntimeTimedTargetLinkState keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionTimedTargetParameters) == 0x18,
              "ModelDefinitionTimedTargetParameters keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionTimedTargetProjectileView) == 0x280,
              "ModelDefinitionTimedTargetProjectileView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeTimedTargetProjectileView) == 0x200 &&
              offsetof(ModelRuntimeTimedTargetProjectileView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeTimedTargetProjectileView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeTimedTargetProjectileView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeTimedTargetProjectileView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeWeaponAimStateView) == 0x200 &&
              offsetof(ModelRuntimeWeaponAimStateView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeWeaponAimStateView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeWeaponAimStateView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeWeaponAimStateView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeVerticalDeploymentLinkState) == 0x24,
              "ModelRuntimeVerticalDeploymentLinkState keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeVerticalDeploymentView) == 0x200 &&
              offsetof(ModelRuntimeVerticalDeploymentView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeVerticalDeploymentView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeVerticalDeploymentView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeVerticalDeploymentView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeClass21State) == 0x7C,
              "ModelRuntimeClass21State keeps its 32-bit layout");
static_assert(sizeof(ModelDefinitionArticulatedMovementView) == 0x280,
              "ModelDefinitionArticulatedMovementView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeClass21UpdateView) == 0x200 &&
              offsetof(ModelRuntimeClass21UpdateView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeClass21UpdateView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeClass21UpdateView, ownerArmyRuntime) == 0x8,
              "ModelRuntimeClass21UpdateView keeps its 32-bit layout");
static_assert(sizeof(ModelRuntimeArticulatedMovementDefinitionView) == 0x200 &&
              offsetof(ModelRuntimeArticulatedMovementDefinitionView, modelDefinition) == 0x0 &&
              offsetof(ModelRuntimeArticulatedMovementDefinitionView, rootModelNode) == 0x4 &&
              offsetof(ModelRuntimeArticulatedMovementDefinitionView, ownerArmyRuntime) == 0x8 &&
              offsetof(ModelRuntimeArticulatedMovementDefinitionView, commandTargetArmyRuntime) == 0x1C &&
              offsetof(ModelRuntimeArticulatedMovementDefinitionView, linkedArmyRuntimeOrSavedOffset) == 0x6C &&
              offsetof(ModelRuntimeArticulatedMovementDefinitionView, linkedModelRuntime) == 0xF0,
              "ModelRuntimeArticulatedMovementDefinitionView keeps its 32-bit layout");
static_assert(sizeof(InGameLevelRuntimeGlobalBlock20) == 0x20 &&
              offsetof(InGameLevelRuntimeGlobalBlock20, conditionStorage) == 0x0,
              "InGameLevelRuntimeGlobalBlock20 keeps its 32-bit layout");
static_assert(sizeof(InGameLevelConditionStorage) == 0x800,
              "InGameLevelConditionStorage keeps its 32-bit layout");
static_assert(sizeof(LevelInitialArmyPlacementRecord20) == 0x20,
              "LevelInitialArmyPlacementRecord20 keeps its 32-bit layout");
static_assert(sizeof(UiSingleLineTextControl) == 0x5C &&
              offsetof(UiSingleLineTextControl, focusChild) == 0x50 &&
              offsetof(UiSingleLineTextControl, text) == 0x54,
              "UiSingleLineTextControl keeps its 32-bit layout");
static_assert(sizeof(UiWrappedTextControl) == 0x5C &&
              offsetof(UiWrappedTextControl, text) == 0x54,
              "UiWrappedTextControl keeps its 32-bit layout");

/* Step 13 U1: the control types of the six template vtables that had none (UiNodeBase + N dwords each). */
static_assert(sizeof(UiFocusProxyControl) == 0x4C + 4 * 4 && offsetof(UiFocusProxyControl, labelFlags) == 0x4C &&
                  offsetof(UiFocusProxyControl, focusChild) == 0x50 && offsetof(UiFocusProxyControl, text) == 0x54 &&
                  offsetof(UiFocusProxyControl, styleOverride) == 0x58,
              "UiFocusProxyControl is UiNodeBase + 4 dwords (g_UiFocusProxyControlVtable)");
static_assert(sizeof(UiListOffsetControl) == 0x4C + 4 * 4 && offsetof(UiListOffsetControl, labelFlags) == 0x4C &&
                  offsetof(UiListOffsetControl, wrapWidth) == 0x50 && offsetof(UiListOffsetControl, text) == 0x54 &&
                  offsetof(UiListOffsetControl, styleOverride) == 0x58,
              "UiListOffsetControl is UiNodeBase + 4 dwords (g_UiListOffsetControlVtable)");
static_assert(sizeof(UiLayoutContainerControl<1>) == 0x54 && sizeof(UiLayoutContainerControl<2>) == 0x58 &&
                  sizeof(UiLayoutContainerControl<3>) == 0x5C && sizeof(UiLayoutContainerControl<4>) == 0x60 &&
                  sizeof(UiLayoutContainerControl<8>) == 0x70 && sizeof(UiLayoutContainerControl<9>) == 0x74 &&
                  sizeof(UiLayoutContainerControl<13>) == 0x84,
              "UiLayoutContainerControl<N> is UiNodeBase + pageCount + N page slots");
static_assert(offsetof(UiLayoutContainerControl<13>, pageCount) == offsetof(UiPageStackControl, pageCount) &&
                  offsetof(UiLayoutContainerControl<13>, pages) == offsetof(UiPageStackControl, pages) &&
                  sizeof(UiLayoutContainerControl<1>) == sizeof(UiPageStackControl),
              "UiLayoutContainerControl<N> keeps the UiPageStackControl prefix");
static_assert(sizeof(UiCommandSpriteButtonWithDetails) == 0x4C + 12 * 4 &&
                  offsetof(UiCommandSpriteButtonWithDetails, activationInputState) == 0x78,
              "UiCommandSpriteButtonWithDetails is UiNodeBase + 12 dwords (g_UiCommandSpriteButtonWithDetailsVtable)");
static_assert(sizeof(UiCommandVisibilitySingleLineText) == 0x4C + 4 * 4 &&
                  offsetof(UiCommandVisibilitySingleLineText, labelFlags) == 0x4C &&
                  sizeof(UiCommandVisibilityWrappedText) == 0x4C + 4 * 4 &&
                  offsetof(UiCommandVisibilityWrappedText, labelFlags) == 0x4C,
              "the command-visibility texts are UiNodeBase + 4 dwords");
static_assert(sizeof(UiRangeSliderControl) == 0x68 &&
              offsetof(UiRangeSliderControl, clickSound) == 0x64,
              "UiRangeSliderControl keeps its 32-bit layout");
static_assert(sizeof(UiHorizontalGaugeControl) == 0x5C,
              "UiHorizontalGaugeControl keeps its 32-bit layout");
static_assert(sizeof(UiImagePanelControl) == 0x5C &&
              offsetof(UiImagePanelControl, textureSource) == 0x54,
              "UiImagePanelControl keeps its 32-bit layout");
static_assert(sizeof(UiArmyMetricsPanel) == 0x60 &&
              offsetof(UiArmyMetricsPanel, entity) == 0x5C,
              "UiArmyMetricsPanel keeps its 32-bit layout");
static_assert(sizeof(UiFillPanelControl) == 0x5C &&
              offsetof(UiFillPanelControl, textureSource) == 0x54,
              "UiFillPanelControl keeps its 32-bit layout");
static_assert(sizeof(UiNineSlicePanelControl) == 0x5C &&
              offsetof(UiNineSlicePanelControl, textureSource) == 0x50,
              "UiNineSlicePanelControl keeps its 32-bit layout");
static_assert(sizeof(UiSoftwareTexturePreviewControl) == 0x6C &&
              offsetof(UiSoftwareTexturePreviewControl, textureSource) == 0x50 &&
              offsetof(UiSoftwareTexturePreviewControl, blendFactorPixels) == 0x60 &&
              offsetof(UiSoftwareTexturePreviewControl, blendedSourcePixels) == 0x64,
              "UiSoftwareTexturePreviewControl keeps its 32-bit layout");
static_assert(sizeof(UiFormattedContainer) == 0x94 &&
              offsetof(UiFormattedContainer, textureSource) == 0x58,
              "UiFormattedContainer keeps its 32-bit layout");
static_assert(sizeof(UiFormattedContainerWithMarker) == 0xB0,
              "UiFormattedContainerWithMarker keeps its 32-bit layout");
static_assert(sizeof(UiRootStackActionHandlerPage2) == 0x8 &&
              offsetof(UiRootStackActionHandlerPage2, handlers) == 0x0,
              "UiRootStackActionHandlerPage2 keeps its 32-bit layout");
static_assert(sizeof(UiTextButtonTemplateFields) == 0x10,
              "UiTextButtonTemplateFields keeps its 32-bit layout");
static_assert(sizeof(UiLabelTemplateFields) == 0x10 &&
              offsetof(UiLabelTemplateFields, focusChild) == 0x4,
              "UiLabelTemplateFields keeps its 32-bit layout");
static_assert(sizeof(UiRangeSliderTemplateFields) == 0x18,
              "UiRangeSliderTemplateFields keeps its 32-bit layout");
static_assert(sizeof(UiResizableWindowTemplateFields) == 0x2C &&
              offsetof(UiResizableWindowTemplateFields, rootFlags) == 0x0 &&
              sizeof(UiResizableWindowTemplateFields::rootFlags) == 4 &&
              offsetof(UiResizableWindowTemplateFields, callbacks) == 0x4 &&
              offsetof(UiResizableWindowTemplateFields, previousRoot) == 0x8,
              "UiResizableWindowTemplateFields keeps its 32-bit layout");
static_assert(sizeof(UiDisplaySettingsApplyButtonTemplateFields) == 0x40,
              "UiDisplaySettingsApplyButtonTemplateFields keeps its 32-bit layout");
static_assert(sizeof(UiDisplaySettingsReadoutTemplateFields) == 0x50,
              "UiDisplaySettingsReadoutTemplateFields keeps its 32-bit layout");
static_assert(sizeof(UiDisplayModeOptionPrefix) == 0xC,
              "UiDisplayModeOptionPrefix keeps its 32-bit layout");
static_assert(sizeof(UiTechnologyAreaTabPrefix) == 0x8 &&
              offsetof(UiTechnologyAreaTabPrefix, tooltipText) == 0x4,
              "UiTechnologyAreaTabPrefix keeps its 32-bit layout");
static_assert(sizeof(FatalErrorUiImage) == 0x110,
              "FatalErrorUiImage keeps its 32-bit layout");
static_assert(sizeof(DisplaySettingsUiImage) == 0xBD4,
              "DisplaySettingsUiImage keeps its 32-bit layout");
static_assert(sizeof(FourValueDialogUiImage) == 0x1A4,
              "FourValueDialogUiImage keeps its 32-bit layout");
/* Step 13 U2: the typed nodes of the dialog images (tools/dev/ui_image_retype.py --asserts). */
static_assert(offsetof(FatalErrorUiImage, fatalErrorPanel) == 0x0 && sizeof(UiPanelControl) == 0x58,
              "FatalErrorUiImage.fatalErrorPanel is a UiPanelControl");
static_assert(offsetof(FatalErrorUiImage, errorMessageText) == 0x58 && sizeof(UiListOffsetControl) == 0x5C,
              "FatalErrorUiImage.errorMessageText is a UiListOffsetControl");
static_assert(offsetof(FatalErrorUiImage, okButton_fields) == 0x100 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "FatalErrorUiImage.okButton_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(FourValueDialogUiImage, confirmModeDialogPanel) == 0x0 && sizeof(UiPanelControl) == 0x58,
              "FourValueDialogUiImage.confirmModeDialogPanel is a UiPanelControl");
static_assert(offsetof(FourValueDialogUiImage, revertButton_fields) == 0xA4 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "FourValueDialogUiImage.revertButton_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(FourValueDialogUiImage, keepModeButton_fields) == 0x100 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "FourValueDialogUiImage.keepModeButton_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(FourValueDialogUiImage, countdownMessageText) == 0x110 && sizeof(UiListOffsetControl) == 0x5C,
              "FourValueDialogUiImage.countdownMessageText is a UiListOffsetControl");
static_assert(offsetof(FourValueDialogUiImage, countdownMessageText_trailing) == 0x16C,
              "FourValueDialogUiImage.countdownMessageText_trailing follows the control");
static_assert(offsetof(DisplaySettingsUiImage, displaySettingsWindow) == 0x0 && sizeof(UiResizableWindowControl) == 0x78,
              "DisplaySettingsUiImage.displaySettingsWindow is a UiResizableWindowControl");
static_assert(offsetof(DisplaySettingsUiImage, applyButton) == 0xD4 && sizeof(UiDisplaySettingsApplyButton) == 0x8C,
              "DisplaySettingsUiImage.applyButton is a UiDisplaySettingsApplyButton");
static_assert(offsetof(DisplaySettingsUiImage, resolutionHeading) == 0x160 && sizeof(UiFocusProxyControl) == 0x5C,
              "DisplaySettingsUiImage.resolutionHeading is a UiFocusProxyControl");
static_assert(offsetof(DisplaySettingsUiImage, colorDepthHeading) == 0x1BC && sizeof(UiFocusProxyControl) == 0x5C,
              "DisplaySettingsUiImage.colorDepthHeading is a UiFocusProxyControl");
static_assert(offsetof(DisplaySettingsUiImage, adapterHeading) == 0x218 && sizeof(UiFocusProxyControl) == 0x5C,
              "DisplaySettingsUiImage.adapterHeading is a UiFocusProxyControl");
static_assert(offsetof(DisplaySettingsUiImage, colorScaleSliderFrame) == 0x95C && sizeof(UiFocusProxyControl) == 0x5C,
              "DisplaySettingsUiImage.colorScaleSliderFrame is a UiFocusProxyControl");
static_assert(offsetof(DisplaySettingsUiImage, colorBiasSliderFrame) == 0xA1C && sizeof(UiFocusProxyControl) == 0x5C,
              "DisplaySettingsUiImage.colorBiasSliderFrame is a UiFocusProxyControl");
static_assert(offsetof(DisplaySettingsUiImage, colorScaleValueText) == 0xADC && sizeof(UiFocusProxyControl) == 0x5C,
              "DisplaySettingsUiImage.colorScaleValueText is a UiFocusProxyControl");
static_assert(offsetof(DisplaySettingsUiImage, colorBiasValueText) == 0xB38 && sizeof(UiDisplaySettingsValueReadout) == 0x9C,
              "DisplaySettingsUiImage.colorBiasValueText is a UiDisplaySettingsValueReadout");
/* Step 13 U4: the typed nodes of the frontend image (tools/dev/ui_image_retype.py --asserts). */
static_assert(offsetof(FrontendUiImage, frontendRoot) == 0x0 && sizeof(UiPanelControl) == 0x58,
              "FrontendUiImage.frontendRoot is a UiPanelControl");
static_assert(offsetof(FrontendUiImage, frontendViewModeStack) == 0x58 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "FrontendUiImage.frontendViewModeStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(FrontendUiImage, chatInputSlot) == 0xB0 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "FrontendUiImage.chatInputSlot is a UiLayoutContainerControl<2>");
static_assert(offsetof(FrontendUiImage, chatInputEdit) == 0x108 && sizeof(UiRequiredTextEditControl) == 0x80,
              "FrontendUiImage.chatInputEdit is a UiRequiredTextEditControl");
static_assert(offsetof(FrontendUiImage, moviePlaybackView) == 0x1D4 && sizeof(UiSoftwareTexturePreviewControl) == 0x6C,
              "FrontendUiImage.moviePlaybackView is a UiSoftwareTexturePreviewControl");
static_assert(offsetof(FrontendUiImage, movieLetterboxTopBar) == 0x240 && sizeof(UiFillPanelControl) == 0x5C,
              "FrontendUiImage.movieLetterboxTopBar is a UiFillPanelControl");
static_assert(offsetof(FrontendUiImage, movieLetterboxBottomBar) == 0x29C && sizeof(UiFillPanelControl) == 0x5C,
              "FrontendUiImage.movieLetterboxBottomBar is a UiFillPanelControl");
static_assert(offsetof(FrontendUiImage, chatMessageHistory) == 0x2F8 && sizeof(UiConditionalActionControl) == 0x60,
              "FrontendUiImage.chatMessageHistory is a UiConditionalActionControl");
static_assert(offsetof(FrontendUiImage, menuRoomModelView) == 0x368 && sizeof(FrontendModelPointerContext) == 0x17C,
              "FrontendUiImage.menuRoomModelView is a FrontendModelPointerContext");
static_assert(offsetof(FrontendUiImage, frontendPageStack) == 0x508 && sizeof(UiLayoutContainerControl<13>) == 0x84,
              "FrontendUiImage.frontendPageStack is a UiLayoutContainerControl<13>");
static_assert(offsetof(FrontendUiImage, missionBriefingPage) == 0x58C && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.missionBriefingPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, briefingBackButton) == 0x5E8 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.briefingBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, briefingExitButton) == 0x648 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.briefingExitButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, briefingSaveButton) == 0x6A8 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.briefingSaveButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, briefingBeginButton) == 0x708 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.briefingBeginButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, briefingTitleLabel) == 0x768 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.briefingTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, briefingTextScroller) == 0x7C4 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.briefingTextScroller is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, briefingText) == 0x854 && sizeof(UiListOffsetControl) == 0x5C,
              "FrontendUiImage.briefingText is a UiListOffsetControl");
static_assert(offsetof(FrontendUiImage, briefingImage) == 0x8B0 &&
              offsetof(FrontendUiImage, opponentSettingsGroup) - offsetof(FrontendUiImage, briefingImage) == 0x64,
              "FrontendUiImage.briefingImage is a 0x64-byte image action node");
static_assert(sizeof(UiImageActionTemplateNode) == 0x64 &&
              offsetof(UiImageActionTemplateNode, displayFlags) == offsetof(UiImageActionControl, displayFlags) &&
              offsetof(UiImageActionTemplateNode, textureSource) == offsetof(UiImageActionControl, textureSource) &&
              offsetof(UiImageActionTemplateNode, subresource) == offsetof(UiImageActionControl, subresource) &&
              offsetof(UiImageActionTemplateNode, secondaryActionId) == offsetof(UiImageActionControl, secondaryActionId),
              "UiImageActionTemplateNode is UiImageActionControl without letterboxWidth");
static_assert(offsetof(FrontendUiImage, opponentSettingsGroup) == 0x914 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.opponentSettingsGroup is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, opponentWeakLabel) == 0x970 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.opponentWeakLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, opponentStrongLabel) == 0x9CC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.opponentStrongLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, gameSpeedSlider) == 0xA28 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.gameSpeedSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, factionSetupPage) == 0xA90 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.factionSetupPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, factionSetupBackButton) == 0xAEC && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionSetupBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionSetupNextButton) == 0xB4C && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionSetupNextButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionSetupTitleLabel) == 0xBAC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionSetupTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionSetupFinishButton) == 0xC0C && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionSetupFinishButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRosterTable) == 0xC6C && sizeof(UiLayoutContainerControl<1>) == 0x54,
              "FrontendUiImage.factionRosterTable is a UiLayoutContainerControl<1>");
static_assert(offsetof(FrontendUiImage, factionRow1NumberLabel) == 0xCC0 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow1NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow2NumberLabel) == 0xD1C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow2NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow3NumberLabel) == 0xD78 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow3NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow4NumberLabel) == 0xDD4 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow4NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow5NumberLabel) == 0xE30 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow5NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow6NumberLabel) == 0xE8C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow6NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow7NumberLabel) == 0xEE8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow7NumberLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow1ColourButton) == 0xF44 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow1ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow2ColourButton) == 0xFA4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow2ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow3ColourButton) == 0x1004 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow3ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow4ColourButton) == 0x1064 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow4ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow5ColourButton) == 0x10C4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow5ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow6ColourButton) == 0x1124 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow6ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow7ColourButton) == 0x1184 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow7ColourButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow1ModeButton) == 0x11E4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow1ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow2ModeButton) == 0x1244 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow2ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow3ModeButton) == 0x12A4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow3ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow4ModeButton) == 0x1304 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow4ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow5ModeButton) == 0x1364 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow5ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow6ModeButton) == 0x13C4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow6ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow7ModeButton) == 0x1424 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow7ModeButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow1PlayCheckbox) == 0x1484 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow1PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow2PlayCheckbox) == 0x14E4 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow2PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow3PlayCheckbox) == 0x1544 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow3PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow4PlayCheckbox) == 0x15A4 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow4PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow5PlayCheckbox) == 0x1604 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow5PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow6PlayCheckbox) == 0x1664 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow6PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow7PlayCheckbox) == 0x16C4 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.factionRow7PlayCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, factionRow1ParticipantsLabel) == 0x1724 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow1ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow2ParticipantsLabel) == 0x1780 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow2ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow3ParticipantsLabel) == 0x17DC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow3ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow4ParticipantsLabel) == 0x1838 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow4ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow5ParticipantsLabel) == 0x1894 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow5ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow6ParticipantsLabel) == 0x18F0 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow6ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, factionRow7ParticipantsLabel) == 0x194C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.factionRow7ParticipantsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, rosterFactionHeader) == 0x19A8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.rosterFactionHeader is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, rosterModeHeader) == 0x1A08 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.rosterModeHeader is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, rosterColourHeader) == 0x1A68 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.rosterColourHeader is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, rosterAcceptHeader) == 0x1AC8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.rosterAcceptHeader is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, rosterParticipantHeader) == 0x1B24 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.rosterParticipantHeader is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, taskDescriptionLabel) == 0x1B80 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.taskDescriptionLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, taskDescriptionText) == 0x1BDC && sizeof(UiListOffsetControl) == 0x5C,
              "FrontendUiImage.taskDescriptionText is a UiListOffsetControl");
static_assert(offsetof(FrontendUiImage, gameSelectPage) == 0x1C38 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.gameSelectPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, gameSelectCancelButton) == 0x1C94 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.gameSelectCancelButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, gameSelectStartButton) == 0x1CF4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.gameSelectStartButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, loadGameTabButton) == 0x1D54 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.loadGameTabButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, singleGameTabButton) == 0x1DB4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.singleGameTabButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, campaignsTabButton) == 0x1E14 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.campaignsTabButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, gameSelectTitleLabel) == 0x1E74 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.gameSelectTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, gameSelectTabStack) == 0x1ED0 && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "FrontendUiImage.gameSelectTabStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(FrontendUiImage, savedGamesScroller) == 0x1F2C && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.savedGamesScroller is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, savedGamesList) == 0x1FBC && sizeof(UiListControl) == 0x74,
              "FrontendUiImage.savedGamesList is a UiListControl");
static_assert(offsetof(FrontendUiImage, savedGamesLabel) == 0x203C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.savedGamesLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, savedGameDescriptionText) == 0x2098 && sizeof(UiListOffsetControl) == 0x5C,
              "FrontendUiImage.savedGameDescriptionText is a UiListOffsetControl");
static_assert(offsetof(FrontendUiImage, missionsScroller) == 0x20F4 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.missionsScroller is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, missionsList) == 0x2184 && sizeof(UiListControl) == 0x74,
              "FrontendUiImage.missionsList is a UiListControl");
static_assert(offsetof(FrontendUiImage, missionsLabel) == 0x221C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.missionsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, missionDescriptionText) == 0x2278 && sizeof(UiListOffsetControl) == 0x5C,
              "FrontendUiImage.missionDescriptionText is a UiListOffsetControl");
static_assert(offsetof(FrontendUiImage, campaignsScroller) == 0x22D4 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.campaignsScroller is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, campaignsList) == 0x2364 && sizeof(UiListControl) == 0x74,
              "FrontendUiImage.campaignsList is a UiListControl");
static_assert(offsetof(FrontendUiImage, campaignsLabel) == 0x23EC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.campaignsLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, campaignDescriptionText) == 0x2448 && sizeof(UiListOffsetControl) == 0x5C,
              "FrontendUiImage.campaignDescriptionText is a UiListOffsetControl");
static_assert(offsetof(FrontendUiImage, quitConfirmPage) == 0x24A4 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.quitConfirmPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, quitNoButton) == 0x2500 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.quitNoButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, quitYesButton) == 0x2560 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.quitYesButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, quitTitleLabel) == 0x25C0 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.quitTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, optionsPage) == 0x261C && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.optionsPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, optionsOkButton) == 0x2678 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.optionsOkButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, optionsTitleLabel) == 0x26D8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.optionsTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, graphicsSettingsButton) == 0x2734 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.graphicsSettingsButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, settings3DButton) == 0x2794 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.settings3DButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, soundSettingsButton) == 0x27F4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.soundSettingsButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, hidePanelCheckbox) == 0x2854 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.hidePanelCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, scrollSpeedGroup) == 0x28B4 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.scrollSpeedGroup is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, scrollSpeedSlowLabel) == 0x2910 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.scrollSpeedSlowLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, scrollSpeedFastLabel) == 0x296C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.scrollSpeedFastLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, scrollSpeedSlider) == 0x29C8 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.scrollSpeedSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, generalMapGroup) == 0x2A30 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.generalMapGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, autoZoomOffCheckbox) == 0x2A84 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.autoZoomOffCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, autoRotationOffCheckbox) == 0x2AE4 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.autoRotationOffCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, mouseCommandsGroup) == 0x2B44 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.mouseCommandsGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, linkRotationZoomCheckbox) == 0x2B98 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.linkRotationZoomCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, linkRotationTiltCheckbox) == 0x2BF8 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.linkRotationTiltCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, rightButtonNoScrollCheckbox) == 0x2C58 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.rightButtonNoScrollCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, displaySettingsPage) == 0x2CB8 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.displaySettingsPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, displaySettingsBackButton) == 0x2D14 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.displaySettingsBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, displaySettingsApplyButton) == 0x2D74 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.displaySettingsApplyButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, displaySettingsTitle) == 0x2DD4 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.displaySettingsTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, displayAdapterGroup) == 0x2E30 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.displayAdapterGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, displayAdapterOption1) == 0x2E84 && sizeof(UiPayloadPairTextButton) == 0x68,
              "FrontendUiImage.displayAdapterOption1 is a UiPayloadPairTextButton");
static_assert(offsetof(FrontendUiImage, displayAdapterOption2) == 0x2EEC && sizeof(UiPayloadPairTextButton) == 0x68,
              "FrontendUiImage.displayAdapterOption2 is a UiPayloadPairTextButton");
static_assert(offsetof(FrontendUiImage, displayAdapterOption3) == 0x2F54 && sizeof(UiPayloadPairTextButton) == 0x68,
              "FrontendUiImage.displayAdapterOption3 is a UiPayloadPairTextButton");
static_assert(offsetof(FrontendUiImage, displayAdapterOption4) == 0x2FBC && sizeof(UiPayloadPairTextButton) == 0x68,
              "FrontendUiImage.displayAdapterOption4 is a UiPayloadPairTextButton");
static_assert(offsetof(FrontendUiImage, displayAdapterOption5) == 0x3024 && sizeof(UiPayloadPairTextButton) == 0x68,
              "FrontendUiImage.displayAdapterOption5 is a UiPayloadPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionGroup) == 0x308C && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.displayResolutionGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, displayResolutionOption1) == 0x30E0 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption1 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption2) == 0x3148 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption2 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption3) == 0x31B0 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption3 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption4) == 0x3218 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption4 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption5) == 0x3280 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption5 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption6) == 0x32E8 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption6 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption7) == 0x3350 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption7 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption8) == 0x33B8 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption8 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption9) == 0x3420 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption9 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayResolutionOption10) == 0x3488 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayResolutionOption10 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayColorDepthGroup) == 0x34F0 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.displayColorDepthGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, displayColorDepthOption1) == 0x3544 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayColorDepthOption1 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayColorDepthOption2) == 0x35AC && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayColorDepthOption2 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayColorDepthOption3) == 0x3614 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayColorDepthOption3 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, displayColorDepthOption4) == 0x367C && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.displayColorDepthOption4 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, graphicsSettingsPage) == 0x36E4 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.graphicsSettingsPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, graphicsSettingsBackButton) == 0x3740 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.graphicsSettingsBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, graphicsSettingsTitle) == 0x37A0 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.graphicsSettingsTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, shadingEnabledCheckbox) == 0x37FC && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.shadingEnabledCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, shadingLevelGroup) == 0x385C && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.shadingLevelGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, shadingLevelGrid32Depth32) == 0x38B0 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.shadingLevelGrid32Depth32 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, shadingLevelGrid32Depth64) == 0x3918 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.shadingLevelGrid32Depth64 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, shadingLevelGrid32Depth128) == 0x3980 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.shadingLevelGrid32Depth128 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, shadingLevelGrid64Depth64) == 0x39E8 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.shadingLevelGrid64Depth64 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, shadingLevelGrid64Depth128) == 0x3A50 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.shadingLevelGrid64Depth128 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, shadingLevelGrid128Depth128) == 0x3AB8 && sizeof(UiNumericPairTextButton) == 0x68,
              "FrontendUiImage.shadingLevelGrid128Depth128 is a UiNumericPairTextButton");
static_assert(offsetof(FrontendUiImage, polygonDetailLabel) == 0x3B20 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.polygonDetailLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, polygonDetailMinCaption) == 0x3B7C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.polygonDetailMinCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, polygonDetailMaxCaption) == 0x3BD8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.polygonDetailMaxCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, polygonDetailSlider) == 0x3C34 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.polygonDetailSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, textureQualityGroup) == 0x3C9C && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.textureQualityGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, textureQualityLow) == 0x3CF0 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.textureQualityLow is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, textureQualityMedium) == 0x3D50 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.textureQualityMedium is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, textureQualityHigh) == 0x3DB0 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.textureQualityHigh is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, audioSettingsPage) == 0x3E10 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.audioSettingsPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, audioSettingsBackButton) == 0x3E6C && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.audioSettingsBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, audioSettingsTitle) == 0x3ECC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.audioSettingsTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, musicEnabledCheckbox) == 0x3F28 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.musicEnabledCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, soundEffectsEnabledCheckbox) == 0x3F88 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.soundEffectsEnabledCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, reverseStereoCheckbox) == 0x3FE8 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.reverseStereoCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, effectsVolumeLabel) == 0x4048 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.effectsVolumeLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, effectsVolumeMinCaption) == 0x40A4 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.effectsVolumeMinCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, effectsVolumeMaxCaption) == 0x4100 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.effectsVolumeMaxCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, effectsVolumeSlider) == 0x415C && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.effectsVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, movieVolumeLabel) == 0x41C4 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.movieVolumeLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, movieVolumeMinCaption) == 0x4220 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.movieVolumeMinCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, movieVolumeMaxCaption) == 0x427C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.movieVolumeMaxCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, movieVolumeSlider) == 0x42D8 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.movieVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, musicVolumeLabel) == 0x4340 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.musicVolumeLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, musicVolumeMinCaption) == 0x439C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.musicVolumeMinCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, musicVolumeMaxCaption) == 0x43F8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.musicVolumeMaxCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, musicVolumeSlider) == 0x4454 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.musicVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, movieEventVolumeLabel) == 0x44BC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.movieEventVolumeLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, movieEventVolumeMinCaption) == 0x4518 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.movieEventVolumeMinCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, movieEventVolumeMaxCaption) == 0x4574 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.movieEventVolumeMaxCaption is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, movieEventVolumeSlider) == 0x45D0 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.movieEventVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, topBlackBar) == 0x4638 && sizeof(UiFillPanelControl) == 0x5C,
              "FrontendUiImage.topBlackBar is a UiFillPanelControl");
static_assert(offsetof(FrontendUiImage, bottomBar) == 0x4694 && sizeof(UiFillPanelControl) == 0x5C,
              "FrontendUiImage.bottomBar is a UiFillPanelControl");
static_assert(offsetof(FrontendUiImage, bottomBarConditionalAction) == 0x46F0 && sizeof(UiConditionalActionControl) == 0x60,
              "FrontendUiImage.bottomBarConditionalAction is a UiConditionalActionControl");
static_assert(offsetof(FrontendUiImage, bottomBarStatusText) == 0x4750 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.bottomBarStatusText is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, transferProgressGauge) == 0x47AC && sizeof(UiHorizontalGaugeControl) == 0x5C,
              "FrontendUiImage.transferProgressGauge is a UiHorizontalGaugeControl");
static_assert(offsetof(FrontendUiImage, networkGamePage) == 0x4808 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.networkGamePage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, networkGameTitle) == 0x4864 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.networkGameTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, networkGameBackButton) == 0x48C0 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.networkGameBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, networkGameHostButton) == 0x4920 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.networkGameHostButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, networkGameJoinButton) == 0x4980 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.networkGameJoinButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, networkProtocolScrollBox) == 0x49E0 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.networkProtocolScrollBox is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, networkProtocolList) == 0x4A70 && sizeof(UiTextListControl) == 0x68,
              "FrontendUiImage.networkProtocolList is a UiTextListControl");
static_assert(offsetof(FrontendUiImage, sessionListScrollBox) == 0x4AD8 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.sessionListScrollBox is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, sessionList) == 0x4B68 && sizeof(UiListControl) == 0x74,
              "FrontendUiImage.sessionList is a UiListControl");
static_assert(offsetof(FrontendUiImage, hostAddressLabel) == 0x4BEC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.hostAddressLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, playerNameLabel) == 0x4C48 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.playerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, networkProtocolLabel) == 0x4CA4 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.networkProtocolLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, sessionListLabel) == 0x4D00 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.sessionListLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, hostAddressEdit) == 0x4D5C && sizeof(UiRequiredTextEditControl) == 0x80,
              "FrontendUiImage.hostAddressEdit is a UiRequiredTextEditControl");
static_assert(offsetof(FrontendUiImage, playerNameEdit) == 0x4E48 && sizeof(UiRequiredTextEditControl) == 0x80,
              "FrontendUiImage.playerNameEdit is a UiRequiredTextEditControl");
static_assert(offsetof(FrontendUiImage, hostGameSetupPage) == 0x4EDC && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.hostGameSetupPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, hostGameSetupTitle) == 0x4F38 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.hostGameSetupTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, hostGameSetupBackButton) == 0x4F94 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.hostGameSetupBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, hostGameCreateButton) == 0x4FF4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.hostGameCreateButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, gameNameEdit) == 0x5054 && sizeof(UiRequiredTextEditControl) == 0x80,
              "FrontendUiImage.gameNameEdit is a UiRequiredTextEditControl");
static_assert(offsetof(FrontendUiImage, maxPlayersSlider) == 0x50E8 && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.maxPlayersSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, maxPlayersValueText) == 0x5150 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.maxPlayersValueText is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, networkSpeedSlider) == 0x51AC && sizeof(UiRangeSliderControl) == 0x68,
              "FrontendUiImage.networkSpeedSlider is a UiRangeSliderControl");
static_assert(offsetof(FrontendUiImage, networkSpeedValueText) == 0x5214 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.networkSpeedValueText is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, maxPlayersLabel) == 0x5270 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.maxPlayersLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, networkSpeedLabel) == 0x52CC && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.networkSpeedLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, gameNameLabel) == 0x5328 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.gameNameLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, hostLobbyPage) == 0x5384 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.hostLobbyPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, hostLobbyTitle) == 0x53E0 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.hostLobbyTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, hostLobbyBackButton) == 0x543C && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.hostLobbyBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, hostLobbyKickPlayerButton) == 0x549C && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.hostLobbyKickPlayerButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, hostLobbyStartButton) == 0x54FC && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.hostLobbyStartButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, hostLobbyPlayerScrollBox) == 0x555C && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.hostLobbyPlayerScrollBox is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, hostLobbyPlayerList) == 0x55EC && sizeof(UiListControl) == 0x74,
              "FrontendUiImage.hostLobbyPlayerList is a UiListControl");
static_assert(offsetof(FrontendUiImage, hostLobbyPlayerListLabel) == 0x5670 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.hostLobbyPlayerListLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, clientLobbyPage) == 0x56CC && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.clientLobbyPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, clientLobbyTitle) == 0x5728 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.clientLobbyTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, clientLobbyLeaveButton) == 0x5784 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.clientLobbyLeaveButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, clientLobbyPlayerScrollBox) == 0x57E4 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.clientLobbyPlayerScrollBox is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, clientLobbyPlayerList) == 0x5874 && sizeof(UiListControl) == 0x74,
              "FrontendUiImage.clientLobbyPlayerList is a UiListControl");
static_assert(offsetof(FrontendUiImage, clientLobbyPlayerListLabel) == 0x58F8 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.clientLobbyPlayerListLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, displayModeKindGroup) == 0x5954 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.displayModeKindGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, displayModeKindWindow) == 0x59A8 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.displayModeKindWindow is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, displayModeKindBorderless) == 0x5A08 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.displayModeKindBorderless is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, displayModeKindFullscreen) == 0x5A68 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.displayModeKindFullscreen is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, displayResolutionScrollBox) == 0x5AC8 && sizeof(UiScrollableControl) == 0x90,
              "FrontendUiImage.displayResolutionScrollBox is a UiScrollableControl");
static_assert(offsetof(FrontendUiImage, displayResolutionRowPanel) == 0x5B58 && sizeof(UiPanelControl) == 0x58,
              "FrontendUiImage.displayResolutionRowPanel is a UiPanelControl");
static_assert(offsetof(FrontendUiImage, displayPageStack) == 0x71A0 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "FrontendUiImage.displayPageStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(FrontendUiImage, advancedSettingsButton) == 0x71F8 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.advancedSettingsButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedSettingsPage) == 0x7258 && sizeof(UiImagePanelControl) == 0x5C,
              "FrontendUiImage.advancedSettingsPage is a UiImagePanelControl");
static_assert(offsetof(FrontendUiImage, advancedSettingsBackButton) == 0x72B4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "FrontendUiImage.advancedSettingsBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedSettingsTitle) == 0x7314 && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.advancedSettingsTitle is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, advancedEdgesGroup) == 0x7370 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.advancedEdgesGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, advancedEdgesSmooth) == 0x73C4 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedEdgesSmooth is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedEdgesExact) == 0x7424 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedEdgesExact is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedUiScaleGroup) == 0x7484 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.advancedUiScaleGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, advancedUiScaleAuto) == 0x74D8 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedUiScaleAuto is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedUiScale1) == 0x7538 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedUiScale1 is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedUiScale2) == 0x7598 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedUiScale2 is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedUiScale3) == 0x75F8 && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedUiScale3 is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedFrameLimitGroup) == 0x7658 && sizeof(UiTitledWindowControl) == 0x54,
              "FrontendUiImage.advancedFrameLimitGroup is a UiTitledWindowControl");
static_assert(offsetof(FrontendUiImage, advancedFrameLimitOff) == 0x76AC && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedFrameLimitOff is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedFrameLimit60) == 0x770C && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedFrameLimit60 is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedFrameLimit120) == 0x776C && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedFrameLimit120 is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedFrameLimit144) == 0x77CC && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedFrameLimit144 is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedVsyncCheckbox) == 0x782C && sizeof(UiTextButtonControl) == 0x60,
              "FrontendUiImage.advancedVsyncCheckbox is a UiTextButtonControl");
static_assert(offsetof(FrontendUiImage, advancedNoteLabel) == 0x788C && sizeof(UiFocusProxyControl) == 0x5C,
              "FrontendUiImage.advancedNoteLabel is a UiFocusProxyControl");
static_assert(offsetof(FrontendUiImage, chatInputEdit_trailing) == 0x188,
              "FrontendUiImage.chatInputEdit_trailing follows the control");
static_assert(offsetof(FrontendUiImage, chatMessageHistory_trailing) == 0x358,
              "FrontendUiImage.chatMessageHistory_trailing follows the control");
static_assert(offsetof(FrontendUiImage, menuRoomModelView_trailing) == 0x4E4,
              "FrontendUiImage.menuRoomModelView_trailing follows the control");
static_assert(offsetof(FrontendUiImage, factionSetupTitleLabel_trailing) == 0xC08,
              "FrontendUiImage.factionSetupTitleLabel_trailing follows the control");
static_assert(offsetof(FrontendUiImage, rosterFactionHeader_trailing) == 0x1A04,
              "FrontendUiImage.rosterFactionHeader_trailing follows the control");
static_assert(offsetof(FrontendUiImage, rosterModeHeader_trailing) == 0x1A64,
              "FrontendUiImage.rosterModeHeader_trailing follows the control");
static_assert(offsetof(FrontendUiImage, rosterColourHeader_trailing) == 0x1AC4,
              "FrontendUiImage.rosterColourHeader_trailing follows the control");
static_assert(offsetof(FrontendUiImage, savedGamesList_trailing) == 0x2030,
              "FrontendUiImage.savedGamesList_trailing follows the control");
static_assert(offsetof(FrontendUiImage, missionsList_trailing) == 0x21F8,
              "FrontendUiImage.missionsList_trailing follows the control");
static_assert(offsetof(FrontendUiImage, campaignsList_trailing) == 0x23D8,
              "FrontendUiImage.campaignsList_trailing follows the control");
static_assert(offsetof(FrontendUiImage, sessionList_trailing) == 0x4BDC,
              "FrontendUiImage.sessionList_trailing follows the control");
static_assert(offsetof(FrontendUiImage, hostAddressEdit_trailing) == 0x4DDC,
              "FrontendUiImage.hostAddressEdit_trailing follows the control");
static_assert(offsetof(FrontendUiImage, playerNameEdit_trailing) == 0x4EC8,
              "FrontendUiImage.playerNameEdit_trailing follows the control");
static_assert(offsetof(FrontendUiImage, gameNameEdit_trailing) == 0x50D4,
              "FrontendUiImage.gameNameEdit_trailing follows the control");
static_assert(offsetof(FrontendUiImage, hostLobbyPlayerList_trailing) == 0x5660,
              "FrontendUiImage.hostLobbyPlayerList_trailing follows the control");
static_assert(offsetof(FrontendUiImage, clientLobbyPlayerList_trailing) == 0x58E8,
              "FrontendUiImage.clientLobbyPlayerList_trailing follows the control");
static_assert(sizeof(FrontendUiImage) == 0x78E8 && /* 0x5954 + the display mode kind, resolution list and advanced settings nodes of open-thandor */
              offsetof(FrontendUiImage, displayPageStack) == 0x71A0 &&
              offsetof(FrontendUiImage, advancedSettingsButton) == 0x71F8 &&
              offsetof(FrontendUiImage, advancedSettingsPage) == 0x7258 &&
              offsetof(FrontendUiImage, advancedEdgesGroup) == 0x7370 &&
              offsetof(FrontendUiImage, advancedUiScaleGroup) == 0x7484 &&
              offsetof(FrontendUiImage, advancedFrameLimitGroup) == 0x7658 &&
              offsetof(FrontendUiImage, advancedVsyncCheckbox) == 0x782C &&
              offsetof(FrontendUiImage, advancedNoteLabel) == 0x788C &&
              offsetof(FrontendUiImage, displayModeKindGroup) == 0x5954 &&
              offsetof(FrontendUiImage, displayModeKindFullscreen) == 0x5A68 &&
              offsetof(FrontendUiImage, displayResolutionScrollBox) == 0x5AC8 &&
              offsetof(FrontendUiImage, displayResolutionRowPanel) == 0x5B58 &&
              offsetof(FrontendUiImage, displayResolutionExtraOptions) == 0x5BB0,
              "FrontendUiImage keeps its 32-bit layout");
static_assert(sizeof(InGameUiImage) == 0xC3E4,
              "InGameUiImage keeps its 32-bit layout");
/* Step 13 U3: the typed nodes of the in-game image (tools/dev/ui_image_retype.py --asserts). */
static_assert(offsetof(InGameUiImage, inGameRootPanel) == 0x0 && sizeof(UiPanelControl) == 0x58,
              "InGameUiImage.inGameRootPanel is a UiPanelControl");
static_assert(offsetof(InGameUiImage, chatInputPageStack) == 0x58 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.chatInputPageStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, chatInputTextEdit) == 0xB0 && sizeof(UiRequiredTextEditControl) == 0x80,
              "InGameUiImage.chatInputTextEdit is a UiRequiredTextEditControl");
static_assert(offsetof(InGameUiImage, chatInputTextEdit_trailing) == 0x130,
              "InGameUiImage.chatInputTextEdit_trailing follows the control");
static_assert(offsetof(InGameUiImage, primaryPageStack) == 0x17C && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "InGameUiImage.primaryPageStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(InGameUiImage, endMovieView) == 0x1D8 && sizeof(UiImageActionControl) == 0x68,
              "InGameUiImage.endMovieView is a UiImageActionControl");
static_assert(offsetof(InGameUiImage, endMovieLetterboxTop) == 0x240 && sizeof(UiFillPanelControl) == 0x5C,
              "InGameUiImage.endMovieLetterboxTop is a UiFillPanelControl");
static_assert(offsetof(InGameUiImage, endMovieLetterboxBottom) == 0x29C && sizeof(UiFillPanelControl) == 0x5C,
              "InGameUiImage.endMovieLetterboxBottom is a UiFillPanelControl");
static_assert(offsetof(InGameUiImage, endMoviePageStack) == 0x2F8 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.endMoviePageStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, resultsScreenPanel) == 0x350 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.resultsScreenPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, resultsChartPageStack) == 0x3AC && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "InGameUiImage.resultsChartPageStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(InGameUiImage, resultsChart1) == 0x408 && offsetof(InGameUiImage, resultsChart2) == 0x484 &&
              offsetof(InGameUiImage, resultsChart3) == 0x500,
              "InGameUiImage.resultsChart1..3 keep their offsets");
static_assert(offsetof(InGameUiImage, resultsChart2) - offsetof(InGameUiImage, resultsChart1) == sizeof(FrontendResultsTable<6>) &&
              offsetof(InGameUiImage, resultsChart3) - offsetof(InGameUiImage, resultsChart2) == sizeof(FrontendResultsTable<6>) &&
              offsetof(InGameUiImage, resultsTabMilitary) - offsetof(InGameUiImage, resultsChart3) == sizeof(FrontendResultsTable<8>),
              "InGameUiImage.resultsChart1..3 are FrontendResultsTable<6>, <6> and <8>");
static_assert(offsetof(InGameUiImage, resultsTabMilitary) == 0x584 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsTabMilitary is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsTabEconomy) == 0x5E4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsTabEconomy is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsTabThird) == 0x644 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsTabThird is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsContinueButton) == 0x6A4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsContinueButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsSecondaryExitButton) == 0x704 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsSecondaryExitButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsChartModeButtonA) == 0x764 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsChartModeButtonA is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsChartModeButtonB) == 0x7C4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.resultsChartModeButtonB is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, resultsSummaryText) == 0x824 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.resultsSummaryText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, playerStatusBox) == 0x8E4 && sizeof(UiConditionalActionTextBox<8>) == 0x7C,
              "InGameUiImage.playerStatusBox is a UiConditionalActionTextBox<8>");
static_assert(offsetof(InGameUiImage, messageHistoryPanel) == 0x960 && sizeof(UiConditionalActionTextBox<8>) == 0x7C,
              "InGameUiImage.messageHistoryPanel is a UiConditionalActionTextBox<8>");
static_assert(offsetof(InGameUiImage, worldViewArea) == 0x9DC && sizeof(UiLayoutContainerControl<1>) == 0x54,
              "InGameUiImage.worldViewArea is a UiLayoutContainerControl<1>");
static_assert(offsetof(InGameUiImage, worldView) == 0xA30 && sizeof(FrontendModelPointerContext) == 0x17C,
              "InGameUiImage.worldView is a FrontendModelPointerContext");
static_assert(offsetof(InGameUiImage, worldView_trailing) == 0xBAC,
              "InGameUiImage.worldView_trailing follows the control");
static_assert(offsetof(InGameUiImage, gameWindowPageStack) == 0xBD0 && sizeof(UiLayoutContainerControl<9>) == 0x74,
              "InGameUiImage.gameWindowPageStack is a UiLayoutContainerControl<9>");
static_assert(offsetof(InGameUiImage, gameMenuWindow) == 0xC44 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.gameMenuWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, quitGameWindow) == 0xCA0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.quitGameWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, saveGameWindow) == 0xCFC && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.saveGameWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, graphicsSettingsWindow) == 0xD58 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.graphicsSettingsWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, audioSettingsWindow) == 0xDB4 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.audioSettingsWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, missionHelpWindow) == 0xE10 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.missionHelpWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyWindow) == 0xE6C && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, missionHelpTitle) == 0xEC8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.missionHelpTitle is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, missionHelpCloseButton) == 0xF24 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.missionHelpCloseButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, missionHelpBriefingTab) == 0xF84 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.missionHelpBriefingTab is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, missionHelpKeyboardTab) == 0xFE4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.missionHelpKeyboardTab is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, missionHelpMouseTab) == 0x1044 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.missionHelpMouseTab is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, missionHelpTabPageStack) == 0x10A4 && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "InGameUiImage.missionHelpTabPageStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(InGameUiImage, missionBriefingScroll) == 0x1100 && sizeof(UiScrollableControl) == 0x90,
              "InGameUiImage.missionBriefingScroll is a UiScrollableControl");
static_assert(offsetof(InGameUiImage, missionBriefingText) == 0x1190 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.missionBriefingText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, keyboardHelpScroll) == 0x11EC && sizeof(UiScrollableControl) == 0x90,
              "InGameUiImage.keyboardHelpScroll is a UiScrollableControl");
static_assert(offsetof(InGameUiImage, keyboardHelpKeyColumn) == 0x127C && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.keyboardHelpKeyColumn is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, keyboardHelpDescriptionColumn) == 0x12D8 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.keyboardHelpDescriptionColumn is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, mouseHelpScroll) == 0x1334 && sizeof(UiScrollableControl) == 0x90,
              "InGameUiImage.mouseHelpScroll is a UiScrollableControl");
static_assert(offsetof(InGameUiImage, mouseHelpText) == 0x13C4 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.mouseHelpText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, technologyTitle) == 0x1420 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.technologyTitle is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, technologyCloseButton) == 0x147C && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyCloseButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyResearchButton) == 0x14DC && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyResearchButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab1) == 0x1544 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab1 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab2) == 0x15AC && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab2 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab3) == 0x1614 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab3 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab4) == 0x167C && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab4 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab5) == 0x16E4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab5 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab6) == 0x174C && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab6 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab7) == 0x17B4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.technologyAreaTab7 is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab1Icon) == 0x1814 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab1Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab2Icon) == 0x1870 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab2Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab3Icon) == 0x18CC && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab3Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab4Icon) == 0x1928 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab4Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab5Icon) == 0x1984 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab5Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab6Icon) == 0x19E0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab6Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyAreaTab7Icon) == 0x1A3C && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyAreaTab7Icon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyDescriptionFrame) == 0x1A98 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.technologyDescriptionFrame is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, technologyDescriptionScroll) == 0x1AF4 && sizeof(UiScrollableControl) == 0x90,
              "InGameUiImage.technologyDescriptionScroll is a UiScrollableControl");
static_assert(offsetof(InGameUiImage, technologyDescriptionText) == 0x1B84 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.technologyDescriptionText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, messageWindow) == 0x1BE0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.messageWindow is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, messageWindowTitle) == 0x1C3C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.messageWindowTitle is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, messageTextEdit) == 0x1C98 && sizeof(UiRequiredTextEditControl) == 0x80,
              "InGameUiImage.messageTextEdit is a UiRequiredTextEditControl");
static_assert(offsetof(InGameUiImage, messageTextEdit_trailing) == 0x1D18,
              "InGameUiImage.messageTextEdit_trailing follows the control");
static_assert(offsetof(InGameUiImage, messageCancelButton) == 0x1D64 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.messageCancelButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, messageSendAndCloseButton) == 0x1DC4 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.messageSendAndCloseButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, messageSendButton) == 0x1E24 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.messageSendButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientPlayersTab) == 0x1E84 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientPlayersTab is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientGroupsTab) == 0x1EE4 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientGroupsTab is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientAllTab) == 0x1F44 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientAllTab is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientPageStack) == 0x1FA4 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.messageRecipientPageStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, messageRecipientScroll) == 0x1FFC && sizeof(UiScrollableControl) == 0x90,
              "InGameUiImage.messageRecipientScroll is a UiScrollableControl");
static_assert(offsetof(InGameUiImage, messageRecipientList) == 0x208C && sizeof(UiPanelControl) == 0x58,
              "InGameUiImage.messageRecipientList is a UiPanelControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox1) == 0x20E4 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox1 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox2) == 0x2144 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox2 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox3) == 0x21A4 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox3 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox4) == 0x2204 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox4 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox5) == 0x2264 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox5 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox6) == 0x22C4 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox6 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, messageRecipientCheckbox7) == 0x2324 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.messageRecipientCheckbox7 is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, worldViewCyclingInfoText) == 0x2384 && sizeof(UiCommandVisibilitySingleLineText) == 0x5C,
              "InGameUiImage.worldViewCyclingInfoText is a UiCommandVisibilitySingleLineText");
static_assert(offsetof(InGameUiImage, worldViewStatusTextA) == 0x23E0 && sizeof(UiCommandVisibilitySingleLineText) == 0x5C,
              "InGameUiImage.worldViewStatusTextA is a UiCommandVisibilitySingleLineText");
static_assert(offsetof(InGameUiImage, worldViewStatusTextB) == 0x243C && sizeof(UiCommandVisibilitySingleLineText) == 0x5C,
              "InGameUiImage.worldViewStatusTextB is a UiCommandVisibilitySingleLineText");
static_assert(offsetof(InGameUiImage, worldViewWrappedStatusText) == 0x2498 && sizeof(UiCommandVisibilityWrappedText) == 0x5C,
              "InGameUiImage.worldViewWrappedStatusText is a UiCommandVisibilityWrappedText");
static_assert(offsetof(InGameUiImage, gameMenuTitle) == 0x24F4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.gameMenuTitle is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, gameMenuSaveButton) == 0x2550 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.gameMenuSaveButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, gameMenuQuitButton) == 0x25B0 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.gameMenuQuitButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, gameMenuGraphicsButton) == 0x2610 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.gameMenuGraphicsButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, gameMenuAudioButton) == 0x2670 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.gameMenuAudioButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, rightButtonNoScrollCheckbox) == 0x26D0 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.rightButtonNoScrollCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, scrollSpeedGroup) == 0x2730 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.scrollSpeedGroup is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, scrollSpeedMinLabel) == 0x278C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.scrollSpeedMinLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, scrollSpeedMaxLabel) == 0x27E8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.scrollSpeedMaxLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, scrollSpeedSlider) == 0x2844 && sizeof(UiRangeSliderControl) == 0x68,
              "InGameUiImage.scrollSpeedSlider is a UiRangeSliderControl");
static_assert(offsetof(InGameUiImage, autoCameraGroup) == 0x28AC && sizeof(UiTitledWindowControl) == 0x54,
              "InGameUiImage.autoCameraGroup is a UiTitledWindowControl");
static_assert(offsetof(InGameUiImage, autoZoomOffCheckbox) == 0x2900 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.autoZoomOffCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, autoRotationOffCheckbox) == 0x2960 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.autoRotationOffCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, cameraLinkGroup) == 0x29C0 && sizeof(UiTitledWindowControl) == 0x54,
              "InGameUiImage.cameraLinkGroup is a UiTitledWindowControl");
static_assert(offsetof(InGameUiImage, linkRotationZoomCheckbox) == 0x2A14 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.linkRotationZoomCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, linkRotationTiltCheckbox) == 0x2A74 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.linkRotationTiltCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, hidePanelCheckbox) == 0x2AD4 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.hidePanelCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, gameMenuCloseButton) == 0x2B34 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.gameMenuCloseButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, saveGameBackButton) == 0x2B94 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.saveGameBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, saveGameTitle) == 0x2BF4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.saveGameTitle is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, saveGameSaveButton) == 0x2C50 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.saveGameSaveButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, saveGameDeleteButton) == 0x2CB0 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.saveGameDeleteButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, saveGameListScroll) == 0x2D10 && sizeof(UiScrollableControl) == 0x90,
              "InGameUiImage.saveGameListScroll is a UiScrollableControl");
static_assert(offsetof(InGameUiImage, saveGameList) == 0x2DA0 && sizeof(UiListControl) == 0x74,
              "InGameUiImage.saveGameList is a UiListControl");
static_assert(offsetof(InGameUiImage, saveGameList_trailing) == 0x2E14,
              "InGameUiImage.saveGameList_trailing follows the control");
static_assert(offsetof(InGameUiImage, saveGameListHeaderLabel) == 0x2E1C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.saveGameListHeaderLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, saveGameDescriptionText) == 0x2E78 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.saveGameDescriptionText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, saveNameEntryStack) == 0x2ED4 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.saveNameEntryStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, saveNameEdit) == 0x2F2C && sizeof(UiRequiredTextEditControl) == 0x80,
              "InGameUiImage.saveNameEdit is a UiRequiredTextEditControl");
static_assert(offsetof(InGameUiImage, saveNameEdit_trailing) == 0x2FAC,
              "InGameUiImage.saveNameEdit_trailing follows the control");
static_assert(offsetof(InGameUiImage, saveNameLabel) == 0x2FD8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.saveNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, quitMenuBackButton) == 0x3034 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.quitMenuBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, quitMenuTitleLabel) == 0x3094 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.quitMenuTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, quitMenuAbortMissionButton) == 0x30F0 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.quitMenuAbortMissionButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, quitMenuSurrenderButton) == 0x3150 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.quitMenuSurrenderButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, quitMenuRestartMissionButton) == 0x31B0 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.quitMenuRestartMissionButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, graphicsOptionsBackButton) == 0x3210 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.graphicsOptionsBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, graphicsOptionsTitleLabel) == 0x3270 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.graphicsOptionsTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, shadingEnabledCheckbox) == 0x32CC && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.shadingEnabledCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, shadingLevelGroup) == 0x332C && sizeof(UiTitledWindowControl) == 0x54,
              "InGameUiImage.shadingLevelGroup is a UiTitledWindowControl");
static_assert(offsetof(InGameUiImage, shadingLevel32x32Button) == 0x3380 && sizeof(UiNumericPairTextButton) == 0x68,
              "InGameUiImage.shadingLevel32x32Button is a UiNumericPairTextButton");
static_assert(offsetof(InGameUiImage, shadingLevel32x64Button) == 0x33E8 && sizeof(UiNumericPairTextButton) == 0x68,
              "InGameUiImage.shadingLevel32x64Button is a UiNumericPairTextButton");
static_assert(offsetof(InGameUiImage, shadingLevel32x128Button) == 0x3450 && sizeof(UiNumericPairTextButton) == 0x68,
              "InGameUiImage.shadingLevel32x128Button is a UiNumericPairTextButton");
static_assert(offsetof(InGameUiImage, shadingLevel64x64Button) == 0x34B8 && sizeof(UiNumericPairTextButton) == 0x68,
              "InGameUiImage.shadingLevel64x64Button is a UiNumericPairTextButton");
static_assert(offsetof(InGameUiImage, shadingLevel64x128Button) == 0x3520 && sizeof(UiNumericPairTextButton) == 0x68,
              "InGameUiImage.shadingLevel64x128Button is a UiNumericPairTextButton");
static_assert(offsetof(InGameUiImage, shadingLevel128x128Button) == 0x3588 && sizeof(UiNumericPairTextButton) == 0x68,
              "InGameUiImage.shadingLevel128x128Button is a UiNumericPairTextButton");
static_assert(offsetof(InGameUiImage, modelDetailGroup) == 0x35F0 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.modelDetailGroup is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, modelDetailMinLabel) == 0x364C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.modelDetailMinLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, modelDetailMaxLabel) == 0x36A8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.modelDetailMaxLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, modelDetailSlider) == 0x3704 && sizeof(UiRangeSliderControl) == 0x68,
              "InGameUiImage.modelDetailSlider is a UiRangeSliderControl");
static_assert(offsetof(InGameUiImage, textureQualityGroup) == 0x376C && sizeof(UiTitledWindowControl) == 0x54,
              "InGameUiImage.textureQualityGroup is a UiTitledWindowControl");
static_assert(offsetof(InGameUiImage, textureQualityLowButton) == 0x37C0 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.textureQualityLowButton is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, textureQualityMediumButton) == 0x3820 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.textureQualityMediumButton is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, textureQualityHighButton) == 0x3880 && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.textureQualityHighButton is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, soundOptionsBackButton) == 0x38E0 && sizeof(UiFramedTextButtonControl) == 0x60,
              "InGameUiImage.soundOptionsBackButton is a UiFramedTextButtonControl");
static_assert(offsetof(InGameUiImage, soundOptionsTitleLabel) == 0x3940 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.soundOptionsTitleLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, musicEnabledCheckbox) == 0x399C && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.musicEnabledCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, effectsEnabledCheckbox) == 0x39FC && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.effectsEnabledCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, reverseStereoCheckbox) == 0x3A5C && sizeof(UiTextButtonControl) == 0x60,
              "InGameUiImage.reverseStereoCheckbox is a UiTextButtonControl");
static_assert(offsetof(InGameUiImage, effectsVolumeGroup) == 0x3ABC && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.effectsVolumeGroup is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, effectsVolumeMinLabel) == 0x3B18 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.effectsVolumeMinLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, effectsVolumeMaxLabel) == 0x3B74 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.effectsVolumeMaxLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, effectsVolumeSlider) == 0x3BD0 && sizeof(UiRangeSliderControl) == 0x68,
              "InGameUiImage.effectsVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(InGameUiImage, movieVolumeGroup) == 0x3C38 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.movieVolumeGroup is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, movieVolumeMinLabel) == 0x3C94 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.movieVolumeMinLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, movieVolumeMaxLabel) == 0x3CF0 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.movieVolumeMaxLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, movieVolumeSlider) == 0x3D4C && sizeof(UiRangeSliderControl) == 0x68,
              "InGameUiImage.movieVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(InGameUiImage, musicVolumeGroup) == 0x3DB4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.musicVolumeGroup is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, musicVolumeMinLabel) == 0x3E10 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.musicVolumeMinLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, musicVolumeMaxLabel) == 0x3E6C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.musicVolumeMaxLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, musicVolumeSlider) == 0x3EC8 && sizeof(UiRangeSliderControl) == 0x68,
              "InGameUiImage.musicVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(InGameUiImage, messageMovieVolumeGroup) == 0x3F30 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.messageMovieVolumeGroup is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, messageMovieVolumeMinLabel) == 0x3F8C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.messageMovieVolumeMinLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, messageMovieVolumeMaxLabel) == 0x3FE8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.messageMovieVolumeMaxLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, messageMovieVolumeSlider) == 0x4044 && sizeof(UiRangeSliderControl) == 0x68,
              "InGameUiImage.messageMovieVolumeSlider is a UiRangeSliderControl");
static_assert(offsetof(InGameUiImage, sidePanelStack) == 0x40AC && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.sidePanelStack is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, sidePanelFrameLeftEdge) == 0x4104 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.sidePanelFrameLeftEdge is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, sidePanelFrameRightEdge) == 0x4160 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.sidePanelFrameRightEdge is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, sidePanelFrameTopCap) == 0x41BC && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.sidePanelFrameTopCap is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, sidePanelFrameMenuBar) == 0x4218 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.sidePanelFrameMenuBar is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, sidePanelFrameInfoSection) == 0x4274 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.sidePanelFrameInfoSection is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, sidePanelFrameBottomCap) == 0x42D0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.sidePanelFrameBottomCap is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, sidePanelMenuButtonStack) == 0x432C && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "InGameUiImage.sidePanelMenuButtonStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(InGameUiImage, inGameMenuButton) == 0x4388 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.inGameMenuButton is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, missionObjectivesButton) == 0x4400 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.missionObjectivesButton is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, countdownDisplayPanel) == 0x4478 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.countdownDisplayPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, countdownText) == 0x44D4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.countdownText is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, resourceBarModeStack) == 0x4530 && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "InGameUiImage.resourceBarModeStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(InGameUiImage, resourcePanel) == 0x458C && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.resourcePanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, editorTabStripA) == 0x45E8 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.editorTabStripA is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, gamePanelsModeStack) == 0x4644 && sizeof(UiLayoutContainerControl<3>) == 0x5C,
              "InGameUiImage.gamePanelsModeStack is a UiLayoutContainerControl<3>");
static_assert(offsetof(InGameUiImage, gamePanelsArea) == 0x46A0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.gamePanelsArea is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, editorTabStripB) == 0x46FC && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.editorTabStripB is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, resourcePanelImageToggle8) == 0x4758 && sizeof(UiImageControl) == 0x6C,
              "InGameUiImage.resourcePanelImageToggle8 is a UiImageControl");
static_assert(offsetof(InGameUiImage, resourcePanelImageToggle8Popup) == 0x47C4 && sizeof(UiNineSlicePanelControl) == 0x5C,
              "InGameUiImage.resourcePanelImageToggle8Popup is a UiNineSlicePanelControl");
static_assert(offsetof(InGameUiImage, resourcePanelImageToggle9) == 0x4820 && sizeof(UiImageControl) == 0x6C,
              "InGameUiImage.resourcePanelImageToggle9 is a UiImageControl");
static_assert(offsetof(InGameUiImage, resourcePanelImageToggle9Popup) == 0x488C && sizeof(UiNineSlicePanelControl) == 0x5C,
              "InGameUiImage.resourcePanelImageToggle9Popup is a UiNineSlicePanelControl");
static_assert(offsetof(InGameUiImage, resourcePanelImageToggle9Popup_trailing) == 0x48E8,
              "InGameUiImage.resourcePanelImageToggle9Popup_trailing follows the control");
static_assert(offsetof(InGameUiImage, resourcePanelIconButton) == 0x48EC && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.resourcePanelIconButton is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, xeniteGauge) == 0x4964 && sizeof(UiFormattedContainer) == 0x94,
              "InGameUiImage.xeniteGauge is a UiFormattedContainer");
static_assert(offsetof(InGameUiImage, xeniteGauge_trailing) == 0x49F8,
              "InGameUiImage.xeniteGauge_trailing follows the control");
static_assert(offsetof(InGameUiImage, tritiumGauge) == 0x49FC && sizeof(UiFormattedContainer) == 0x94,
              "InGameUiImage.tritiumGauge is a UiFormattedContainer");
static_assert(offsetof(InGameUiImage, tritiumGauge_trailing) == 0x4A90,
              "InGameUiImage.tritiumGauge_trailing follows the control");
static_assert(offsetof(InGameUiImage, energyGauge) == 0x4A94 && sizeof(UiFormattedContainer) == 0x94,
              "InGameUiImage.energyGauge is a UiFormattedContainer");
static_assert(offsetof(InGameUiImage, energyGauge_trailing) == 0x4B28,
              "InGameUiImage.energyGauge_trailing follows the control");
static_assert(offsetof(InGameUiImage, xeniteAmountText) == 0x4B44 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.xeniteAmountText is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, xeniteAmountText_trailing) == 0x4BA0,
              "InGameUiImage.xeniteAmountText_trailing follows the control");
static_assert(offsetof(InGameUiImage, editorModeTabTerrainHeight) == 0x4BA4 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.editorModeTabTerrainHeight is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, editorModeTabTerrainMaterial) == 0x4C1C && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.editorModeTabTerrainMaterial is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, editorModeTabTerrainSmoothing) == 0x4C94 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.editorModeTabTerrainSmoothing is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, diplomacyPanel) == 0x4D0C && sizeof(UiImageControl) == 0x6C,
              "InGameUiImage.diplomacyPanel is a UiImageControl");
static_assert(offsetof(InGameUiImage, diplomacyFrame) == 0x4D78 && sizeof(UiNineSlicePanelControl) == 0x5C,
              "InGameUiImage.diplomacyFrame is a UiNineSlicePanelControl");
static_assert(offsetof(InGameUiImage, diplomacyRow1) == 0x4DD4 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow1 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow2) == 0x4E2C && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow2 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow3) == 0x4E84 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow3 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow4) == 0x4EDC && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow4 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow5) == 0x4F34 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow5 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow6) == 0x4F8C && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow6 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow7) == 0x4FE4 && sizeof(UiLayoutContainerControl<2>) == 0x58,
              "InGameUiImage.diplomacyRow7 is a UiLayoutContainerControl<2>");
static_assert(offsetof(InGameUiImage, diplomacyRow1PlayerNumberLabel) == 0x503C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow1PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow2PlayerNumberLabel) == 0x5098 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow2PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow3PlayerNumberLabel) == 0x50F4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow3PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow4PlayerNumberLabel) == 0x5150 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow4PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow5PlayerNumberLabel) == 0x51AC && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow5PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow6PlayerNumberLabel) == 0x5208 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow6PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow7PlayerNumberLabel) == 0x5264 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow7PlayerNumberLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow1FactionLabel) == 0x52C0 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow1FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow2FactionLabel) == 0x531C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow2FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow3FactionLabel) == 0x5378 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow3FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow4FactionLabel) == 0x53D4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow4FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow5FactionLabel) == 0x5430 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow5FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow6FactionLabel) == 0x548C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow6FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow7FactionLabel) == 0x54E8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow7FactionLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow1RelationLabel) == 0x5544 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow1RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow2RelationLabel) == 0x55A0 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow2RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow3RelationLabel) == 0x55FC && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow3RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow4RelationLabel) == 0x5658 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow4RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow5RelationLabel) == 0x56B4 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow5RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow6RelationLabel) == 0x5710 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow6RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow7RelationLabel) == 0x576C && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow7RelationLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow1PlayerNameLabel) == 0x57C8 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow1PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow2PlayerNameLabel) == 0x5824 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow2PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow3PlayerNameLabel) == 0x5880 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow3PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow4PlayerNameLabel) == 0x58DC && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow4PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow5PlayerNameLabel) == 0x5938 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow5PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow6PlayerNameLabel) == 0x5994 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow6PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow7PlayerNameLabel) == 0x59F0 && sizeof(UiFocusProxyControl) == 0x5C,
              "InGameUiImage.diplomacyRow7PlayerNameLabel is a UiFocusProxyControl");
static_assert(offsetof(InGameUiImage, diplomacyRow1RelationButton) == 0x5A4C && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow1RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow2RelationButton) == 0x5AC8 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow2RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow3RelationButton) == 0x5B44 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow3RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow4RelationButton) == 0x5BC0 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow4RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow5RelationButton) == 0x5C3C && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow5RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow6RelationButton) == 0x5CB8 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow6RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow7RelationButton) == 0x5D34 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.diplomacyRow7RelationButton is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, diplomacyRow7RelationButton_trailing) == 0x5DB0,
              "InGameUiImage.diplomacyRow7RelationButton_trailing follows the control");
static_assert(offsetof(InGameUiImage, buildCatalogPanel) == 0x5DB4 && sizeof(UiImageControl) == 0x6C,
              "InGameUiImage.buildCatalogPanel is a UiImageControl");
static_assert(offsetof(InGameUiImage, buildCatalogFrame) == 0x5E20 && sizeof(UiNineSlicePanelControl) == 0x5C,
              "InGameUiImage.buildCatalogFrame is a UiNineSlicePanelControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry00) == 0x5E7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry00 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry01) == 0x5EFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry01 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry02) == 0x5F7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry02 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry03) == 0x5FFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry03 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry04) == 0x607C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry04 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry05) == 0x60FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry05 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry06) == 0x617C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry06 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry07) == 0x61FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry07 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry08) == 0x627C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry08 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry09) == 0x62FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry09 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry10) == 0x637C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry10 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry11) == 0x63FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry11 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry12) == 0x647C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry12 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry13) == 0x64FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry13 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry14) == 0x657C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry14 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry15) == 0x65FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry15 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry16) == 0x667C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry16 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry17) == 0x66FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry17 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry18) == 0x677C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry18 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry19) == 0x67FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry19 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry20) == 0x687C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry20 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry21) == 0x68FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry21 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry22) == 0x697C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry22 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry23) == 0x69FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry23 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry24) == 0x6A7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry24 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry25) == 0x6AFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry25 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry26) == 0x6B7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry26 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry27) == 0x6BFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry27 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry28) == 0x6C7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry28 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry29) == 0x6CFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry29 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry30) == 0x6D7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry30 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry31) == 0x6DFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry31 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry32) == 0x6E7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry32 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry33) == 0x6EFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry33 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry34) == 0x6F7C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry34 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry35) == 0x6FFC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry35 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry36) == 0x707C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry36 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry37) == 0x70FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry37 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry38) == 0x717C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry38 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry39) == 0x71FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry39 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry40) == 0x727C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry40 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry41) == 0x72FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry41 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry42) == 0x737C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry42 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry43) == 0x73FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry43 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry44) == 0x747C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry44 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry45) == 0x74FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry45 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry46) == 0x757C && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry46 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry47) == 0x75FC && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.buildCatalogEntry47 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, buildCatalogEntry47_trailing) == 0x767C,
              "InGameUiImage.buildCatalogEntry47_trailing follows the control");
static_assert(offsetof(InGameUiImage, specialBuildCatalogPanel) == 0x7680 && sizeof(UiImageControl) == 0x6C,
              "InGameUiImage.specialBuildCatalogPanel is a UiImageControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogFrame) == 0x76EC && sizeof(UiNineSlicePanelControl) == 0x5C,
              "InGameUiImage.specialBuildCatalogFrame is a UiNineSlicePanelControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry00) == 0x7748 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry00 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry01) == 0x77C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry01 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry02) == 0x7848 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry02 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry03) == 0x78C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry03 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry04) == 0x7948 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry04 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry05) == 0x79C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry05 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry06) == 0x7A48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry06 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry07) == 0x7AC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry07 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry08) == 0x7B48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry08 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry09) == 0x7BC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry09 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry10) == 0x7C48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry10 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry11) == 0x7CC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry11 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry12) == 0x7D48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry12 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry13) == 0x7DC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry13 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry14) == 0x7E48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry14 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry15) == 0x7EC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry15 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry16) == 0x7F48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry16 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry17) == 0x7FC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry17 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry18) == 0x8048 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry18 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry19) == 0x80C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry19 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry20) == 0x8148 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry20 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry21) == 0x81C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry21 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry22) == 0x8248 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry22 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry23) == 0x82C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry23 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry24) == 0x8348 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry24 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry25) == 0x83C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry25 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry26) == 0x8448 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry26 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry27) == 0x84C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry27 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry28) == 0x8548 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry28 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry29) == 0x85C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry29 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry30) == 0x8648 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry30 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry31) == 0x86C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry31 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry32) == 0x8748 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry32 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry33) == 0x87C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry33 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry34) == 0x8848 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry34 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry35) == 0x88C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry35 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry36) == 0x8948 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry36 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry37) == 0x89C8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry37 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry38) == 0x8A48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry38 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry39) == 0x8AC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry39 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry40) == 0x8B48 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry40 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry41) == 0x8BC8 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.specialBuildCatalogEntry41 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, specialBuildCatalogEntry41_trailing) == 0x8C48,
              "InGameUiImage.specialBuildCatalogEntry41_trailing follows the control");
static_assert(offsetof(InGameUiImage, armyStockPanel) == 0x8C4C && sizeof(UiImageControl) == 0x6C,
              "InGameUiImage.armyStockPanel is a UiImageControl");
static_assert(offsetof(InGameUiImage, armyStockFrame) == 0x8CB8 && sizeof(UiNineSlicePanelControl) == 0x5C,
              "InGameUiImage.armyStockFrame is a UiNineSlicePanelControl");
static_assert(offsetof(InGameUiImage, armyStockSlot00) == 0x8D14 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot00 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot01) == 0x8D90 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot01 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot02) == 0x8E0C && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot02 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot03) == 0x8E88 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot03 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot04) == 0x8F04 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot04 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot05) == 0x8F80 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot05 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot06) == 0x8FFC && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot06 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot07) == 0x9078 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot07 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot08) == 0x90F4 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot08 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot09) == 0x9170 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot09 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot10) == 0x91EC && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot10 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot11) == 0x9268 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot11 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot12) == 0x92E4 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot12 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot13) == 0x9360 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot13 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot14) == 0x93DC && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot14 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot15) == 0x9458 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot15 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot16) == 0x94D4 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot16 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot17) == 0x9550 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot17 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot18) == 0x95CC && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot18 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot19) == 0x9648 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot19 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot20) == 0x96C4 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot20 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot21) == 0x9740 && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot21 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot22) == 0x97BC && sizeof(UiCommandSpriteButtonWithDetails) == 0x7C,
              "InGameUiImage.armyStockSlot22 is a UiCommandSpriteButtonWithDetails");
static_assert(offsetof(InGameUiImage, armyStockSlot23) == 0x9838 && sizeof(UiCatalogEntryControl) == 0x80,
              "InGameUiImage.armyStockSlot23 is a UiCatalogEntryControl");
static_assert(offsetof(InGameUiImage, editorModeTabRegion) == 0x98B8 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.editorModeTabRegion is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, editorModeTabUnitPlacement) == 0x9930 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.editorModeTabUnitPlacement is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, minimapView) == 0x9A1C && sizeof(UiSelectionGeometryControl) == 0x70,
              "InGameUiImage.minimapView is a UiSelectionGeometryControl");
static_assert(offsetof(InGameUiImage, modePreviewPageStack) == 0x9A8C && sizeof(UiLayoutContainerControl<8>) == 0x70,
              "InGameUiImage.modePreviewPageStack is a UiLayoutContainerControl<8>");
static_assert(offsetof(InGameUiImage, heightToolPreview) == 0x9B60 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.heightToolPreview is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialToolSelectedSwatch) == 0x9BBC && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialToolSelectedSwatch is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, smoothingToolPreview) == 0x9C18 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.smoothingToolPreview is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, unitPlacementPreviewFrame) == 0x9C74 && sizeof(UiFillPanelControl) == 0x5C,
              "InGameUiImage.unitPlacementPreviewFrame is a UiFillPanelControl");
static_assert(offsetof(InGameUiImage, unitPlacementPreviewImage) == 0x9CD0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.unitPlacementPreviewImage is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, objectPlacementPreviewFrame) == 0x9D2C && sizeof(UiFillPanelControl) == 0x5C,
              "InGameUiImage.objectPlacementPreviewFrame is a UiFillPanelControl");
static_assert(offsetof(InGameUiImage, objectPlacementPreviewImage) == 0x9D88 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.objectPlacementPreviewImage is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, regionToolPreview) == 0x9DE4 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.regionToolPreview is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, regionToolPreview_trailing) == 0x9E40,
              "InGameUiImage.regionToolPreview_trailing follows the control");
static_assert(offsetof(InGameUiImage, modeDetailPageStack) == 0x9EE0 && sizeof(UiLayoutContainerControl<8>) == 0x70,
              "InGameUiImage.modeDetailPageStack is a UiLayoutContainerControl<8>");
static_assert(offsetof(InGameUiImage, selectionDetailPanel) == 0x9F50 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.selectionDetailPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, selectionDetailPageStack) == 0x9FAC && sizeof(UiLayoutContainerControl<4>) == 0x60,
              "InGameUiImage.selectionDetailPageStack is a UiLayoutContainerControl<4>");
static_assert(offsetof(InGameUiImage, singleSelectionMetrics) == 0xA00C && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.singleSelectionMetrics is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, singleSelectionStatsText) == 0xA06C && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.singleSelectionStatsText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, singleSelectionStatsText_trailing) == 0xA0C8,
              "InGameUiImage.singleSelectionStatsText_trailing follows the control");
static_assert(offsetof(InGameUiImage, hoverItemIcon) == 0xA140 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.hoverItemIcon is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, hoverItemStatsText) == 0xA19C && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.hoverItemStatsText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, multiSelectionCell00) == 0xA1F8 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell00 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell01) == 0xA258 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell01 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell02) == 0xA2B8 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell02 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell03) == 0xA318 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell03 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell04) == 0xA378 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell04 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell05) == 0xA3D8 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell05 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell06) == 0xA438 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell06 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell07) == 0xA498 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell07 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell08) == 0xA4F8 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell08 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell09) == 0xA558 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell09 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell10) == 0xA5B8 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell10 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, multiSelectionCell11) == 0xA618 && sizeof(UiArmyMetricsPanel) == 0x60,
              "InGameUiImage.multiSelectionCell11 is a UiArmyMetricsPanel");
static_assert(offsetof(InGameUiImage, heightToolPanel) == 0xA678 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.heightToolPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialPalettePanel) == 0xA6D4 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialPalettePanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch00) == 0xA730 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch00 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch00Selector_fields) == 0xA7D8 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch00Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch01) == 0xA7E8 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch01 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch01Selector_fields) == 0xA890 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch01Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch02) == 0xA8A0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch02 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch02Selector_fields) == 0xA948 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch02Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch03) == 0xA958 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch03 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch03Selector_fields) == 0xAA00 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch03Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch04) == 0xAA10 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch04 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch04Selector_fields) == 0xAAB8 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch04Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch05) == 0xAAC8 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch05 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch05Selector_fields) == 0xAB70 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch05Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch06) == 0xAB80 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch06 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch06Selector_fields) == 0xAC28 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch06Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch07) == 0xAC38 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch07 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch07Selector_fields) == 0xACE0 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch07Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch08) == 0xACF0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch08 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch08Selector_fields) == 0xAD98 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch08Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch09) == 0xADA8 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch09 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch09Selector_fields) == 0xAE50 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch09Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch10) == 0xAE60 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch10 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch10Selector_fields) == 0xAF08 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch10Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, materialSwatch11) == 0xAF18 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.materialSwatch11 is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, materialSwatch11Selector_fields) == 0xAFC0 && sizeof(UiTextButtonTemplateFields) == 0x10,
              "InGameUiImage.materialSwatch11Selector_fields is a UiTextButtonTemplateFields");
static_assert(offsetof(InGameUiImage, smoothingToolPanel) == 0xAFD0 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.smoothingToolPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, unitPlacementPanel) == 0xB02C && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.unitPlacementPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, unitPlacementStatsText) == 0xB088 && sizeof(UiListOffsetControl) == 0x5C,
              "InGameUiImage.unitPlacementStatsText is a UiListOffsetControl");
static_assert(offsetof(InGameUiImage, objectPlacementPanel) == 0xB0E4 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.objectPlacementPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, regionToolPanel) == 0xB140 && sizeof(UiImagePanelControl) == 0x5C,
              "InGameUiImage.regionToolPanel is a UiImagePanelControl");
static_assert(offsetof(InGameUiImage, modeCommandPageStack) == 0xB19C && sizeof(UiLayoutContainerControl<9>) == 0x74,
              "InGameUiImage.modeCommandPageStack is a UiLayoutContainerControl<9>");
static_assert(offsetof(InGameUiImage, selectionGroupButton0) == 0xB210 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton0 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton0_trailing) == 0xB28C,
              "InGameUiImage.selectionGroupButton0_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton1) == 0xB290 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton1 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton1_trailing) == 0xB30C,
              "InGameUiImage.selectionGroupButton1_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton2) == 0xB310 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton2 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton2_trailing) == 0xB38C,
              "InGameUiImage.selectionGroupButton2_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton3) == 0xB390 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton3 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton3_trailing) == 0xB40C,
              "InGameUiImage.selectionGroupButton3_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton4) == 0xB410 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton4 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton4_trailing) == 0xB48C,
              "InGameUiImage.selectionGroupButton4_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton5) == 0xB490 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton5 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton5_trailing) == 0xB50C,
              "InGameUiImage.selectionGroupButton5_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton6) == 0xB510 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton6 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton6_trailing) == 0xB58C,
              "InGameUiImage.selectionGroupButton6_trailing follows the control");
static_assert(offsetof(InGameUiImage, selectionGroupButton7) == 0xB590 && sizeof(UiCommandSpriteButtonControl) == 0x7C,
              "InGameUiImage.selectionGroupButton7 is a UiCommandSpriteButtonControl");
static_assert(offsetof(InGameUiImage, selectionGroupButton7_trailing) == 0xB60C,
              "InGameUiImage.selectionGroupButton7_trailing follows the control");
static_assert(offsetof(InGameUiImage, heightToolOption0) == 0xB610 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.heightToolOption0 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, heightToolOption1) == 0xB688 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.heightToolOption1 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, heightToolOption2) == 0xB700 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.heightToolOption2 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, heightToolOption3) == 0xB778 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.heightToolOption3 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, materialToolOption0) == 0xB7F0 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.materialToolOption0 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, materialToolOption1) == 0xB868 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.materialToolOption1 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, materialToolOption2) == 0xB8E0 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.materialToolOption2 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, materialToolOption3) == 0xB958 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.materialToolOption3 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, smoothingToolOption0) == 0xB9D0 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.smoothingToolOption0 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, smoothingToolOption1) == 0xBA48 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.smoothingToolOption1 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, smoothingToolOption2) == 0xBAC0 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.smoothingToolOption2 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, smoothingRelaxGatedButton) == 0xBB38 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.smoothingRelaxGatedButton is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, smoothingRelaxLandButton) == 0xBBB0 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.smoothingRelaxLandButton is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, unitPlacementOption0) == 0xBC28 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.unitPlacementOption0 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, unitPlacementOption2) == 0xBCA0 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.unitPlacementOption2 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, unitPlacementOption1) == 0xBD18 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.unitPlacementOption1 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, objectPlacementOption0) == 0xBD90 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.objectPlacementOption0 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, objectPlacementOption2) == 0xBE08 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.objectPlacementOption2 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, objectPlacementOption1) == 0xBE80 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.objectPlacementOption1 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, regionToolOption0) == 0xBEF8 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.regionToolOption0 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, regionToolOption1) == 0xBF70 && sizeof(UiSpriteButtonControl) == 0x78,
              "InGameUiImage.regionToolOption1 is a UiSpriteButtonControl");
static_assert(offsetof(InGameUiImage, regionToolOption1_trailing) == 0xBFE8,
              "InGameUiImage.regionToolOption1_trailing follows the control");

/* Typed table entries (core/slot.h): a function of exactly the slot's signature is the entry itself; one taking a
   registered prefixed type (ui/controls/node_views.h), a view of the prefix chain or void * gets a thunk. Other
   arities, return types and scalar parameter types do not compile. */
static_assert(ThandorSlot<&UiNode_ApplyFlagsRecursive>::pick<void(UiNodeFlagMask, UiNodeFlagMask, UiNodeBase *)>() ==
                  &UiNode_ApplyFlagsRecursive,
              "THANDOR_SLOT of an exact signature is the function itself");
static_assert(ThandorSlot<&UiImageControl_PointerMove>::pick<GraphicsCursorFrameIndex(UiPixelCoordinate, UiPixelCoordinate,
                                                                                      UiNodeBase *)>() != nullptr,
              "THANDOR_SLOT of UiImageControl * (prefix chain over UiSelectableControl) for a UiNodeBase * slot");
static_assert(ThandorSlot<&UiSelectableControl_KeyboardEvent>::pick<Bool8(UiKeyboardStateMask, UiKeyboardEventCode,
                                                                          UiNodeBase *)>() != nullptr,
              "THANDOR_SLOT of UiSoundSelectableControl * for a UiNodeBase * slot");
static_assert(ThandorSlot<&InGameUiRuntime_ResetNotificationButtonCursor>::pick<void(WorldRuntimeContext *)>() != nullptr,
              "THANDOR_SLOT of void * for a typed pointer slot");
static_assert(thandor_slot_is_view_of<UiCatalogEntryControl, UiNodeBase>() &&
                  thandor_slot_is_view_of<InGameMissionHelpRootView, UiNodeBase>() &&
                  thandor_slot_is_view_of<UiSelectableOptionRow68, UiNodeBase>() &&
                  !thandor_slot_is_view_of<UiNodeBase, UiImageControl>(),
              "UI node prefix registrations reach UiNodeBase");

/* core/flags.h: THANDOR_FLAG_ENUM on a 32-bit flag set at global scope and an 8-bit one in a namespace. */
enum class FlagsCheck32 : uint32_t { None = 0, A = 0x1, B = 0x2, High = 0x80000000u };
THANDOR_FLAG_ENUM(FlagsCheck32);
namespace flags_check {
enum class Byte : uint8_t { None = 0, A = 0x1, B = 0x80 };
THANDOR_FLAG_ENUM(Byte);
enum class NotFlags : uint32_t { A = 1 };
} // namespace flags_check

constexpr FlagsCheck32 FlagsCheck_Compound()

{
  FlagsCheck32 flags = FlagsCheck32::A;
  flags |= FlagsCheck32::High;
  flags ^= FlagsCheck32::B;
  flags &= ~FlagsCheck32::A;
  return flags;
}

static_assert(ToBits(FlagsCheck32::A | FlagsCheck32::High) == 0x80000001u && ToBits(~FlagsCheck32::A) == 0xFFFFFFFEu &&
                  (FlagsCheck32::A & FlagsCheck32::B) == FlagsCheck32::None &&
                  ToBits(FlagsCheck32::A ^ (FlagsCheck32::A | FlagsCheck32::B)) == 0x2 &&
                  FlagsCheck_Compound() == (FlagsCheck32::High | FlagsCheck32::B),
              "THANDOR_FLAG_ENUM operators keep the bit values of a 32-bit flag set");
static_assert(!Any(FlagsCheck32::None) && Any(FlagsCheck32::High) && FromBits<FlagsCheck32>(0x3) == (FlagsCheck32::A | FlagsCheck32::B) &&
                  std::is_same_v<decltype(ToBits(FlagsCheck32::A)), uint32_t>,
              "Any, ToBits and FromBits of a 32-bit flag set");
static_assert(ToBits(~flags_check::Byte::A) == 0xFE && ToBits(flags_check::Byte::A | flags_check::Byte::B) == 0x81 &&
                  FromBits<flags_check::Byte>(0xFF) == ~flags_check::Byte::None && Any(flags_check::Byte::B) &&
                  std::is_same_v<decltype(ToBits(flags_check::Byte::A)), uint8_t>,
              "THANDOR_FLAG_ENUM on an 8-bit flag set in a namespace stays 8 bits wide");
static_assert(ThandorFlagEnum<FlagsCheck32> && ThandorFlagEnum<flags_check::Byte> &&
                  !ThandorFlagEnum<flags_check::NotFlags> && !ThandorFlagEnum<uint32_t>,
              "only enums marked with THANDOR_FLAG_ENUM are flag enums");

/* Step 13 X10 (ui/ingame casts): the world input read the owner army of a model runtime as the dword at
   runtimePayload + 8. */
static_assert(offsetof(ModelRuntimeSlot, ownerArmyRuntimeOrSavedOffset) == 0x8,
              "ModelRuntimeSlot.ownerArmyRuntimeOrSavedOffset is the dword at +8 of the owner-list payload");
static_assert(offsetof(ArmyArticulatedRuntimeSlotView, articulatedContact) ==
                  offsetof(ModelRuntimeSlot, classState) + offsetof(ModelRuntimeSlotClassState, classStateAC),
              "the selection panel's terrain contact mode is ModelRuntimeSlot classStateAC");

/* Step 13 X4 (gameplay/ai casts): the field names the AI now reads instead of the decompiled views sit at the
   offsets the original used. */
static_assert(offsetof(ModelRuntimeNode, tintArgb) == 0x58 && offsetof(ArmyRuntimeSlot, movementPosition0Q12) == 0x58,
              "the created model node's tint is the word the AI planners cleared as ArmyRuntimeSlot.movementPosition0Q12");
static_assert(offsetof(ModelDefinition, removalEffectDefinitionReference) == 0x190 &&
                  offsetof(ModelRuntimeSlot, attachments) + 2 * sizeof(ModelRuntimeAttachmentDescriptor) +
                          offsetof(ModelRuntimeAttachmentDescriptor, childLocalRotationAngle0) == 0x190 &&
                  offsetof(ArmyRuntimeSlot, modelRuntimeOrSavedOffset) == 0x0 &&
                  offsetof(ModelRuntimeSlot, definitionOrSavedId) == 0x0,
              "the AI planners' construction effect is ModelDefinition.removalEffectDefinitionReference");
static_assert(offsetof(ModelRuntimeSlot, classLinkState) == 24 * 4 &&
                  offsetof(ModelRuntimeSlot, classState) + offsetof(ModelRuntimeSlotClassState, classStateAC) == 43 * 4 &&
                  offsetof(ModelRuntimeSlot, classState) + offsetof(ModelRuntimeSlotClassState, behaviorState) == 46 * 4 &&
                  offsetof(ModelRuntimeSlot, classState) + offsetof(ModelRuntimeSlotClassState, stateFlags) == 59 * 4,
              "ModelRuntimeSlot words 24, 43, 46 and 59 the AI reads by name");
static_assert(sizeof(AiStructureWorkspaceEntry) == sizeof(AiRuntimeWorkspaceEntry) &&
                  offsetof(AiStructureWorkspaceEntry, runtimeSlotAddressOrZero) == offsetof(AiRuntimeWorkspaceEntry, modelRuntime) &&
                  offsetof(AiStructureWorkspaceEntry, armyAssetId) == offsetof(AiRuntimeWorkspaceEntry, armyAssetId) &&
                  offsetof(AiScoredSiteWorkspaceEntry, cellWorldXQ12) == 0x0 &&
                  offsetof(AiScoredSiteWorkspaceEntry, cellWorldYQ12) == 0x4 &&
                  offsetof(AiScoredSiteWorkspaceEntry, score) == 0x8,
              "AI workspace entries the AI walks as each other");

/* Step 13 E2 (ui/frontend and ui/ingame state families as enum class): the fields keep their offsets, sizes and
   underlying types (player records are copied dword by dword and sent; the cursor frame is a UiImageActionControl
   dword of the in-game template image). */
static_assert(offsetof(FrontendPlayerRuntimeRecord, commandSyncPending) == 0x50 &&
                  offsetof(FrontendPlayerRuntimeRecord, factionAssignment) == 0x54 &&
                  offsetof(FrontendPlayerFactionAssignmentState, roleStateFlags) == 0xC &&
                  sizeof(FrontendPlayerRuntimeRecord::commandSyncPending) == 4 &&
                  sizeof(FrontendPlayerFactionAssignmentState::roleStateFlags) == 4,
              "commandSyncPending and roleStateFlags stay dwords at +0x50 and +0x60 of the player record");
static_assert(offsetof(InGameNotificationPayload, payloadKind) == 0x14 && sizeof(InGameNotificationPayload::payloadKind) == 4,
              "InGameNotificationPayload.payloadKind stays the dword at +0x14");
static_assert(offsetof(InGameRuntimeRoot, notificationButtonCursorFrame) == 0x9B4C &&
                  offsetof(InGameRuntimeRootUiGridView, notificationButtonCursorFrame) == 0x9B4C &&
                  offsetof(InGameTargetingRootTraversalView, notificationButtonCursorFrame) == 0x9B4C &&
                  offsetof(InGameTargetingRootTraversalView, actionState) == 0x50 &&
                  sizeof(InGameRuntimeRoot::notificationButtonCursorFrame) == 4 &&
                  sizeof(InGameTargetingRootTraversalView::actionState) == 4,
              "the notification button cursor frame stays the dword at +0x9B4C of the root (+0x50 of the button)");
