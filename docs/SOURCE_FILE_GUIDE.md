# Source file guide

This index explains each leaf source/header pair and shows which other curated modules call into it. All links are repository-relative and work on GitHub and in a local clone.

[Module tree](MODULE_TREE.md) · [Umbrella header](../include/thandor/thandor.h) · [Function index](../FUNCTION_INDEX.csv) · [V279 → V523 changelog](../CHANGELOG_V279_TO_V523_FULL.md)

## Assets

<a id="module-assets-army-catalog"></a>
### `assets/army/catalog`

[Source](../src/assets/army/catalog.c) · [Header](../include/thandor/assets/army/catalog.h) · [Call graph](callgraphs/assets/army/catalog.md) · [Graphviz](callgraphs/assets/army/catalog.dot)

This file owns catalog lookup, preparation, and registry operations within asset loading, catalogs, and resource formats. Dominant function families: `ArmyAssetRegistry_*` (18), `ArmyAssetHierarchy_*` (2), `ArmyAssetRecord_*` (2). Representative entry points: `ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf`, `ArmyAssetRegistry_StepForwardFlag0100Without0200Cf`, `ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf`.

**Direct callers:** 10 module(s), 44 cross-module call edge(s).

- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 1 caller function(s): [`AiCandidatePlanning_AddSpecialSiteCandidate`](../src/gameplay/ai/placement.c#L54)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 7 caller function(s): [`AiConstructionPlanner_ConsumeFactionPendingArmyAsset`](../src/gameplay/ai/planning.c#L1722), [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628), [`AiConstructionPlanner_ProcessPendingAssetRequests`](../src/gameplay/ai/planning.c#L431); +4 more
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiCandidateWorkspace_GetEntryEntityValue`](../src/gameplay/ai/workspaces.c#L715)
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 1 caller function(s): [`ArmyPlacement_DispatchAssetAtFieldPoint`](../src/gameplay/army/placement.c#L754)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 4 caller function(s): [`ArmyRuntimeClassUpdateSlot11_DispatchByClassId`](../src/gameplay/army/runtime.c#L1050), [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`](../src/gameplay/army/runtime.c#L406); +1 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 6 caller function(s): [`GameFactionRuntime_CancelQueuedArmyAssetsAndRefund`](../src/gameplay/faction/runtime.c#L930), [`GameFactionRuntime_RebaseLoadedArmyReferences`](../src/gameplay/faction/runtime.c#L220), [`GameFactionRuntime_RecomputeProgressAndScoreMetrics`](../src/gameplay/faction/runtime.c#L539); +3 more
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommandModeG_Select3`](../src/ui/ingame/commands.c#L102)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 4 caller function(s): [`InGameMusic_ComputeTrackSuitabilityScore`](../src/ui/ingame/runtime.c#L3346), [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357), [`InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState`](../src/ui/ingame/runtime.c#L4927); +1 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 3 caller function(s): [`InGameTechnologyPanel_Rebuild`](../src/ui/ingame/technology.c#L457), [`UiCatalogGroup42_RebuildGrid`](../src/ui/ingame/technology.c#L259), [`UiCatalogGroup48_RebuildGrid`](../src/ui/ingame/technology.c#L101)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`assets/package/runtime`](#module-assets-package-runtime), [`gameplay/army/runtime`](#module-gameplay-army-runtime).

<a id="module-assets-effect-catalog"></a>
### `assets/effect/catalog`

[Source](../src/assets/effect/catalog.c) · [Header](../include/thandor/assets/effect/catalog.h) · [Call graph](callgraphs/assets/effect/catalog.md) · [Graphviz](callgraphs/assets/effect/catalog.dot)

This file owns catalog lookup, preparation, and registry operations within asset loading, catalogs, and resource formats. Dominant function families: `EffectAsset_*` (1), `EffectDefinition_*` (1), `EffectDefinitionRegistry_*` (1). Representative entry points: `EffectAsset_PrepareEntries`, `EffectDefinitions_ResolveCrossReferences`, `EffectDefinition_RegisterAndLoadSpriteCf`.

**Direct callers:** 5 module(s), 8 cross-module call edge(s).

- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_RegisterAndResolveReferencesCf`](../src/assets/model/definitions.c#L485)
- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 1 caller function(s): [`ShotDefinition_RegisterAndResolveReferencesCf`](../src/assets/shot/catalog.c#L235)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_ClassCommandHandlerGroupACf`](../src/gameplay/army/runtime.c#L1231)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 1 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/shot/catalog`](#module-assets-shot-catalog), [`assets/sprite/catalog`](#module-assets-sprite-catalog), [`core/text/path`](#module-core-text-path), [`world/effects/runtime`](#module-world-effects-runtime).

<a id="module-assets-fnc-runtime"></a>
### `assets/fnc/runtime`

[Source](../src/assets/fnc/runtime.c) · [Header](../include/thandor/assets/fnc/runtime.h) · [Call graph](callgraphs/assets/fnc/runtime.md) · [Graphviz](callgraphs/assets/fnc/runtime.dot)

This file owns runtime state and lifecycle operations within asset loading, catalogs, and resource formats. Dominant function families: `FncModule_*` (3). Representative entry points: `FncModule_GetBindingModeCf`, `FncModule_LoadAndRelocateCf`, `FncModule_GetExportByIndexCf`.

**Direct callers:** 1 module(s), 2 cross-module call edge(s).

- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)

<a id="module-assets-model-definitions"></a>
### `assets/model/definitions`

[Source](../src/assets/model/definitions.c) · [Header](../include/thandor/assets/model/definitions.h) · [Call graph](callgraphs/assets/model/definitions.md) · [Graphviz](callgraphs/assets/model/definitions.dot)

This file owns definitions within asset loading, catalogs, and resource formats. Dominant function families: `ModelDefinition_*` (5), `ModelDefinitionRegistry_*` (3), `ModelDefinitionHierarchy_*` (2). Representative entry points: `ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf`, `ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology`, `ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf`.

**Direct callers:** 20 module(s), 45 cross-module call edge(s).

- [`assets/army/catalog` source](../src/assets/army/catalog.c) · [graph](callgraphs/assets/army/catalog.md): 4 caller function(s): [`ArmyAssetHierarchy_SumFactionUnlockedArmour`](../src/assets/army/catalog.c#L428), [`ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4`](../src/assets/army/catalog.c#L474), [`ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf`](../src/assets/army/catalog.c#L346); +1 more
- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 1 caller function(s): [`AiSiteCandidate_AddTerrainFeatureCellIfSeparated`](../src/gameplay/ai/placement.c#L256)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 4 caller function(s): [`AiArmyCandidate_ComputeAverageCompatibleAssetScore`](../src/gameplay/ai/planning.c#L129), [`AiArmyCandidate_ComputeFactionWeightedScore`](../src/gameplay/ai/planning.c#L1776), [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628); +1 more
- [`gameplay/ai/technology` source](../src/gameplay/ai/technology.c) · [graph](callgraphs/gameplay/ai/technology.md): 1 caller function(s): [`AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue`](../src/gameplay/ai/technology.c#L5)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiPlanning_RebuildFactionWorkspaces`](../src/gameplay/ai/workspaces.c#L70)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyRuntime_EmitDamageThresholdEffect`](../src/gameplay/army/combat.c#L645)
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 4 caller function(s): [`ArmyPlacementCandidate_TestModelAnchorDistanceCf`](../src/gameplay/army/placement.c#L1146), [`ArmyPlacementCandidate_TestOffsetClearanceCf`](../src/gameplay/army/placement.c#L5), [`ArmyPlacement_DispatchAssetAtFieldPoint`](../src/gameplay/army/placement.c#L754); +1 more
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 7 caller function(s): [`ArmyRuntimeClassUpdateSlot11_DispatchByClassId`](../src/gameplay/army/runtime.c#L1050), [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`](../src/gameplay/army/runtime.c#L406); +4 more
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 1 caller function(s): [`SelectionRuntime_ResetMovementPruneAndRecenterEntries`](../src/gameplay/selection/runtime.c#L1724)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`gameplay/technology/runtime` source](../src/gameplay/technology/runtime.c) · [graph](callgraphs/gameplay/technology/runtime.md): 1 caller function(s): [`TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks`](../src/gameplay/technology/runtime.c#L202)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357)
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 2 caller function(s): [`UiCatalogGroup42_RebuildGrid`](../src/ui/ingame/technology.c#L259), [`UiCatalogGroup48_RebuildGrid`](../src/ui/ingame/technology.c#L101)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 3 caller function(s): [`ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf`](../src/world/model/hierarchy.c#L745), [`ModelNodeRuntime_RaycastHierarchyNearestCf`](../src/world/model/hierarchy.c#L607), [`ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive`](../src/world/model/hierarchy.c#L1572)
- [`world/model/slots` source](../src/world/model/slots.c) · [graph](callgraphs/world/model/slots.md): 1 caller function(s): [`ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters`](../src/world/model/slots.c#L109)
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 1 caller function(s): [`WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries`](../src/world/runtime/core.c#L1212)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)

**Calls into:** [`assets/effect/catalog`](#module-assets-effect-catalog), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/shot/catalog`](#module-assets-shot-catalog), [`assets/sprite/catalog`](#module-assets-sprite-catalog), [`core/text/path`](#module-core-text-path), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime).

<a id="module-assets-package-codec"></a>
### `assets/package/codec`

[Source](../src/assets/package/codec.c) · [Header](../include/thandor/assets/package/codec.h) · [Call graph](callgraphs/assets/package/codec.md) · [Graphviz](callgraphs/assets/package/codec.dot)

This file owns encoding and decoding operations within asset loading, catalogs, and resource formats. Dominant function families: `PckCodec_*` (6). Representative entry points: `PckCodec_EncodeFieldGrid`, `PckCodec_DecodeFieldGrid`, `PckCodec_EncodeStored`.

**Direct callers:** 3 module(s), 10 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 4 caller function(s): [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995), [`FrontendScenarioSession_LoadOrRequestFieldGrid`](../src/assets/scenario/catalog.c#L871), [`FrontendScenarioSession_LoadOrRequestLevelAsset`](../src/assets/scenario/catalog.c#L1381); +1 more
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_HostTickCommandAndSnapshotTransfer`](../src/network/backend/runtime.c#L142)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)

<a id="module-assets-package-runtime"></a>
### `assets/package/runtime`

[Source](../src/assets/package/runtime.c) · [Header](../include/thandor/assets/package/runtime.h) · [Call graph](callgraphs/assets/package/runtime.md) · [Graphviz](callgraphs/assets/package/runtime.dot)

This file owns runtime state and lifecycle operations within asset loading, catalogs, and resource formats. Dominant function families: `Package_*` (14), `LevelPackage_*` (1). Representative entry points: `LevelPackage_ValidateAndMount`, `Package_UpsertEntry`, `Package_LoadEntryIntoBuffer`.

**Direct callers:** 22 module(s), 56 cross-module call edge(s).

- [`assets/army/catalog` source](../src/assets/army/catalog.c) · [graph](callgraphs/assets/army/catalog.md): 1 caller function(s): [`ArmyAsset_PrepareRecords`](../src/assets/army/catalog.c#L289)
- [`assets/effect/catalog` source](../src/assets/effect/catalog.c) · [graph](callgraphs/assets/effect/catalog.md): 2 caller function(s): [`EffectAsset_PrepareEntries`](../src/assets/effect/catalog.c#L5), [`EffectDefinition_RegisterAndLoadSpriteCf`](../src/assets/effect/catalog.c#L98)
- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_RegisterAndResolveReferencesCf`](../src/assets/model/definitions.c#L485)
- [`assets/resource/runtime` source](../src/assets/resource/runtime.c) · [graph](callgraphs/assets/resource/runtime.md): 2 caller function(s): [`ResourceRegistration_OpenSourceCf`](../src/assets/resource/runtime.c#L5), [`Resource_Load`](../src/assets/resource/runtime.c#L66)
- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 2 caller function(s): [`RomAssetRecord_RegisterAndRelocate`](../src/assets/rom/runtime.c#L555), [`RomAsset_PrepareRecords`](../src/assets/rom/runtime.c#L112)
- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 3 caller function(s): [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995), [`FrontendScenarioSession_LoadOrRequestFieldGrid`](../src/assets/scenario/catalog.c#L871), [`FrontendScenarioSession_LoadOrRequestLevelAsset`](../src/assets/scenario/catalog.c#L1381)
- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 2 caller function(s): [`ShotAsset_PrepareEntries`](../src/assets/shot/catalog.c#L5), [`ShotDefinition_RegisterAndResolveReferencesCf`](../src/assets/shot/catalog.c#L235)
- [`assets/text/resources` source](../src/assets/text/resources.c) · [graph](callgraphs/assets/text/resources.md): 1 caller function(s): [`TextResourcePage_Load`](../src/assets/text/resources.c#L255)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiRuntime_InitWorkspace`](../src/gameplay/ai/workspaces.c#L1107)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_InitializePoolAndGraphicsCf`](../src/gameplay/army/runtime.c#L1453)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 1 caller function(s): [`SelectionInfoPanel_InitResources`](../src/gameplay/selection/runtime.c#L827)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 4 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf`](../src/gameplay/session/level.c#L1596); +1 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085)
- [`graphics/resources/palette` source](../src/graphics/resources/palette.c) · [graph](callgraphs/graphics/resources/palette.md): 1 caller function(s): [`GraphicsPaletteAsset_LoadPackage`](../src/graphics/resources/palette.c#L198)
- [`graphics/resources/texture` source](../src/graphics/resources/texture.c) · [graph](callgraphs/graphics/resources/texture.md): 2 caller function(s): [`GraphicsTextureSet_LoadPackage`](../src/graphics/resources/texture.c#L174), [`GraphicsTextureSource_LoadPackageAsset`](../src/graphics/resources/texture.c#L638)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`Movie_Open`](../src/movie/runtime/playback.c#L129)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 2 caller function(s): [`GameData_LoadExternalTables`](../src/platform/bootstrap/runtime.c#L182), [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)
- [`platform/filesystem/win32` source](../src/platform/filesystem/win32.c) · [graph](callgraphs/platform/filesystem/win32.md): 8 caller function(s): [`FileSystem_Init`](../src/platform/filesystem/win32.c#L5), [`Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf`](../src/platform/filesystem/win32.c#L724), [`Win32File_CopyCf`](../src/platform/filesystem/win32.c#L420); +5 more
- [`platform/input/devices` source](../src/platform/input/devices.c) · [graph](callgraphs/platform/input/devices.md): 1 caller function(s): [`DirectInputMouse_Init`](../src/platform/input/devices.c#L81)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 2 caller function(s): [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 2 caller function(s): [`TerrainVisualResources_LoadAndClearCellOverlayFlags`](../src/world/terrain/visuals.c#L400), [`TerrainVisualResources_LoadPrimary`](../src/world/terrain/visuals.c#L228)

**Calls into:** [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/resources`](#module-assets-text-resources), [`core/text/path`](#module-core-text-path).

<a id="module-assets-resource-runtime"></a>
### `assets/resource/runtime`

[Source](../src/assets/resource/runtime.c) · [Header](../include/thandor/assets/resource/runtime.h) · [Call graph](callgraphs/assets/resource/runtime.md) · [Graphviz](callgraphs/assets/resource/runtime.dot)

This file owns runtime state and lifecycle operations within asset loading, catalogs, and resource formats. Dominant function families: `ResourceRegistration_*` (6), `Resource_*` (2). Representative entry points: `ResourceRegistration_OpenSourceCf`, `Resource_Load`, `Resource_Release`.

**Direct callers:** 26 module(s), 54 cross-module call edge(s).

- [`assets/effect/catalog` source](../src/assets/effect/catalog.c) · [graph](callgraphs/assets/effect/catalog.md): 1 caller function(s): [`EffectDefinition_RegisterAndLoadSpriteCf`](../src/assets/effect/catalog.c#L98)
- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_RegisterAndResolveReferencesCf`](../src/assets/model/definitions.c#L485)
- [`assets/package/runtime` source](../src/assets/package/runtime.c) · [graph](callgraphs/assets/package/runtime.md): 1 caller function(s): [`LevelPackage_ValidateAndMount`](../src/assets/package/runtime.c#L5)
- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 2 caller function(s): [`FrontendRomRegistry_ClearAndReleaseNestedResources`](../src/assets/rom/runtime.c#L233), [`RomAssetRecord_RegisterAndRelocate`](../src/assets/rom/runtime.c#L555)
- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 6 caller function(s): [`FrontendScenarioSelectionPage_InitializeAndApplyMapOption`](../src/assets/scenario/catalog.c#L103), [`FrontendScenarioSelection_ActivateSelectedRecord`](../src/assets/scenario/catalog.c#L1578), [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995); +3 more
- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 1 caller function(s): [`ShotDefinition_RegisterAndResolveReferencesCf`](../src/assets/shot/catalog.c#L235)
- [`assets/text/resources` source](../src/assets/text/resources.c) · [graph](callgraphs/assets/text/resources.md): 2 caller function(s): [`TextResourcePage_Load`](../src/assets/text/resources.c#L255), [`TextResourcePage_Unload`](../src/assets/text/resources.c#L100)
- [`core/settings/persistent` source](../src/core/settings/persistent.c) · [graph](callgraphs/core/settings/persistent.md): 1 caller function(s): [`PersistentSettings_Load`](../src/core/settings/persistent.c#L26)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_InitializePoolAndGraphicsCf`](../src/gameplay/army/runtime.c#L1453)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 1 caller function(s): [`SelectionInfoPanel_ShutdownResources`](../src/gameplay/selection/runtime.c#L1093)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085)
- [`graphics/resources/palette` source](../src/graphics/resources/palette.c) · [graph](callgraphs/graphics/resources/palette.md): 2 caller function(s): [`GraphicsPaletteAsset_LoadPackage`](../src/graphics/resources/palette.c#L198), [`GraphicsPaletteAsset_ReleasePackage`](../src/graphics/resources/palette.c#L224)
- [`graphics/resources/texture` source](../src/graphics/resources/texture.c) · [graph](callgraphs/graphics/resources/texture.md): 4 caller function(s): [`GraphicsTextureSet_LoadPackage`](../src/graphics/resources/texture.c#L174), [`GraphicsTextureSet_ReleasePackage`](../src/graphics/resources/texture.c#L201), [`GraphicsTextureSource_LoadPackageAsset`](../src/graphics/resources/texture.c#L638); +1 more
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 2 caller function(s): [`GameData_LoadExternalTables`](../src/platform/bootstrap/runtime.c#L182), [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)
- [`platform/input/devices` source](../src/platform/input/devices.c) · [graph](callgraphs/platform/input/devices.md): 1 caller function(s): [`DirectInputMouse_Init`](../src/platform/input/devices.c#L81)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 3 caller function(s): [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../src/ui/frontend/runtime.c#L3522), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 1 caller function(s): [`FrontendSession_ReleaseSelectedResourceAndReturnToMainPage`](../src/ui/frontend/session.c#L5)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendAudioSettings_SetMusicEnabled`](../src/ui/frontend/settings.c#L1061)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand150_HandlePlayerDepartureAndOwnership`](../src/ui/ingame/commands.c#L1467)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)
- [`ui/support/runtime` source](../src/ui/support/runtime.c) · [graph](callgraphs/ui/support/runtime.md): 1 caller function(s): [`PcxPreview_Load64x64PaletteAndPixelsCf`](../src/ui/support/runtime.c#L193)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntime_ShutdownGraphicsResources`](../src/world/effects/runtime.c#L79)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 1 caller function(s): [`ModelRuntimePool_ShutdownAndReleaseDefinitions`](../src/world/model/runtime.c#L405)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntime_ShutdownGraphicsResources`](../src/world/shots/runtime.c#L141)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 1 caller function(s): [`TerrainVisualResources_Shutdown`](../src/world/terrain/visuals.c#L584)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`core/text/path`](#module-core-text-path), [`platform/filesystem/win32`](#module-platform-filesystem-win32), [`world/runtime/core`](#module-world-runtime-core).

<a id="module-assets-rom-runtime"></a>
### `assets/rom/runtime`

[Source](../src/assets/rom/runtime.c) · [Header](../include/thandor/assets/rom/runtime.h) · [Call graph](callgraphs/assets/rom/runtime.md) · [Graphviz](callgraphs/assets/rom/runtime.dot)

This file owns runtime state and lifecycle operations within asset loading, catalogs, and resource formats. Dominant function families: `FrontendRomTransition_*` (4), `RomRegistry_*` (4), `RomRuntime_*` (4). Representative entry points: `FrontendRomActionTable_ExecuteRecord`, `RomAsset_PrepareRecords`, `RomRuntime_BuildAllRegistryNodeTrees`.

**Direct callers:** 3 module(s), 15 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 1 caller function(s): [`ScenarioCatalog_RequestRomTransitionStopCallback`](../src/assets/scenario/catalog.c#L613)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 6 caller function(s): [`FrontendRuntimeCallback64_DispatchRecord1350`](../src/ui/frontend/runtime.c#L1746), [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../src/ui/frontend/runtime.c#L3522), [`FrontendRuntime_UpdatePointerContextAndSceneViewCf`](../src/ui/frontend/runtime.c#L1587); +3 more

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/sprite/catalog`](#module-assets-sprite-catalog), [`core/math/interpolation`](#module-core-math-interpolation), [`core/text/path`](#module-core-text-path), [`graphics/render/shading`](#module-graphics-render-shading), [`ui/core/runtime`](#module-ui-core-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/runtime/core`](#module-world-runtime-core).

<a id="module-assets-scenario-catalog"></a>
### `assets/scenario/catalog`

[Source](../src/assets/scenario/catalog.c) · [Header](../include/thandor/assets/scenario/catalog.h) · [Call graph](callgraphs/assets/scenario/catalog.md) · [Graphviz](callgraphs/assets/scenario/catalog.dot)

This file owns catalog lookup, preparation, and registry operations within asset loading, catalogs, and resource formats. Dominant function families: `ScenarioCatalog_*` (9), `FrontendScenarioSelection_*` (4), `FrontendScenarioPage_*` (3). Representative entry points: `FrontendScenarioSelection_ApplyLocalizedTextSelection`, `FrontendScenarioSelection_ApplyField70Selection`, `FrontendScenarioSelection_ApplyField50Selection`.

**Direct callers:** 1 module(s), 4 cross-module call edge(s).

- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 2 caller function(s): [`FrontendRuntimeCallback68_DispatchRefresh1340`](../src/ui/frontend/runtime.c#L1788), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)

**Calls into:** [`assets/package/codec`](#module-assets-package-codec), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/rom/runtime`](#module-assets-rom-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/memory/allocator`](#module-core-memory-allocator), [`core/text/path`](#module-core-text-path), [`core/text/string`](#module-core-text-string), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`ui/frontend/session`](#module-ui-frontend-session).

<a id="module-assets-shot-catalog"></a>
### `assets/shot/catalog`

[Source](../src/assets/shot/catalog.c) · [Header](../include/thandor/assets/shot/catalog.h) · [Call graph](callgraphs/assets/shot/catalog.md) · [Graphviz](callgraphs/assets/shot/catalog.dot)

This file owns catalog lookup, preparation, and registry operations within asset loading, catalogs, and resource formats. Dominant function families: `ShotDefinition_*` (5), `ShotAsset_*` (1), `ShotDefinitionRegistry_*` (1). Representative entry points: `ShotAsset_PrepareEntries`, `ShotDefinitions_ValidateTerrainMaterialReferences`, `ShotDefinitionRegistry_FindByIdWithErrorCf`.

**Direct callers:** 8 module(s), 13 cross-module call edge(s).

- [`assets/effect/catalog` source](../src/assets/effect/catalog.c) · [graph](callgraphs/assets/effect/catalog.md): 1 caller function(s): [`EffectDefinitions_ResolveCrossReferences`](../src/assets/effect/catalog.c#L46)
- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_RegisterAndResolveReferencesCf`](../src/assets/model/definitions.c#L485)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`](../src/gameplay/army/combat.c#L5)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 2 caller function(s): [`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA`](../src/gameplay/army/movement.c#L400), [`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB`](../src/gameplay/army/movement.c#L582)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_ResolveShotAimPointCf`](../src/gameplay/army/runtime.c#L1770)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 1 caller function(s): [`ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics`](../src/world/model/hierarchy.c#L1187)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)

**Calls into:** [`assets/effect/catalog`](#module-assets-effect-catalog), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/sprite/catalog`](#module-assets-sprite-catalog), [`core/math/fixed`](#module-core-math-fixed), [`core/text/path`](#module-core-text-path), [`world/shots/runtime`](#module-world-shots-runtime).

<a id="module-assets-sprite-catalog"></a>
### `assets/sprite/catalog`

[Source](../src/assets/sprite/catalog.c) · [Header](../include/thandor/assets/sprite/catalog.h) · [Call graph](callgraphs/assets/sprite/catalog.md) · [Graphviz](callgraphs/assets/sprite/catalog.dot)

This file owns catalog lookup, preparation, and registry operations within asset loading, catalogs, and resource formats. Dominant function families: `SpriteAsset_*` (2), `SpriteAssetRegistry_*` (2). Representative entry points: `SpriteAsset_ValidateGroupCountCf`, `SpriteAssetRegistry_Reset`, `SpriteAssetRegistry_FindById`.

**Direct callers:** 6 module(s), 11 cross-module call edge(s).

- [`assets/effect/catalog` source](../src/assets/effect/catalog.c) · [graph](callgraphs/assets/effect/catalog.md): 1 caller function(s): [`EffectDefinition_RegisterAndLoadSpriteCf`](../src/assets/effect/catalog.c#L98)
- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_RegisterAndResolveReferencesCf`](../src/assets/model/definitions.c#L485)
- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 1 caller function(s): [`RomAssetRecord_RegisterAndRelocate`](../src/assets/rom/runtime.c#L555)
- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 1 caller function(s): [`ShotDefinition_RegisterAndResolveReferencesCf`](../src/assets/shot/catalog.c#L235)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameConditionRuntime_RebaseLoadedRecords`](../src/gameplay/session/runtime.c#L450), [`InGameRuntime_ShutdownAndReleaseResources`](../src/gameplay/session/runtime.c#L1444)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../src/ui/frontend/runtime.c#L3522)

<a id="module-assets-text-resources"></a>
### `assets/text/resources`

[Source](../src/assets/text/resources.c) · [Header](../include/thandor/assets/text/resources.h) · [Call graph](callgraphs/assets/text/resources.md) · [Graphviz](callgraphs/assets/text/resources.dot)

This file owns resource lookup and management within asset loading, catalogs, and resource formats. Dominant function families: `FontGlyph_*` (4), `TextResourcePage_*` (3), `FontRuntime_*` (1). Representative entry points: `TextResourcePage_LoadCompatibilityAliases`, `FontRuntime_Init`, `TextResourcePage_Unload`.

**Direct callers:** 23 module(s), 75 cross-module call edge(s).

- [`assets/package/runtime` source](../src/assets/package/runtime.c) · [graph](callgraphs/assets/package/runtime.md): 1 caller function(s): [`LevelPackage_ValidateAndMount`](../src/assets/package/runtime.c#L5)
- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 3 caller function(s): [`ScenarioCatalog_RebuildCampaignRecordListPage`](../src/assets/scenario/catalog.c#L1251), [`ScenarioCatalog_RebuildLevelRecordListPage`](../src/assets/scenario/catalog.c#L1177), [`ScenarioCatalog_RefreshSelectedRecordLocalizedText`](../src/assets/scenario/catalog.c#L1496)
- [`assets/text/richtext` source](../src/assets/text/richtext.c) · [graph](callgraphs/assets/text/richtext.md): 4 caller function(s): [`RichTextCommandStream_DrawNextWrappedLineCf`](../src/assets/text/richtext.c#L1375), [`RichTextCommandStream_DrawSingleLine`](../src/assets/text/richtext.c#L81), [`RichTextCommandStream_MeasureNextWrappedLineCf`](../src/assets/text/richtext.c#L1266); +1 more
- [`core/error/runtime` source](../src/core/error/runtime.c) · [graph](callgraphs/core/error/runtime.md): 3 caller function(s): [`ErrorSystem_Init`](../src/core/error/runtime.c#L5), [`FatalErrorRuntime_DispatchPendingErrorCf`](../src/core/error/runtime.c#L57), [`FatalError_Exit`](../src/core/error/runtime.c#L168)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 2 caller function(s): [`InGameSaveGameList_SelectAndRefreshDetail`](../src/gameplay/session/savegame.c#L5), [`InGameSaveGamePage_RebuildCatalog`](../src/gameplay/session/savegame.c#L130)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 2 caller function(s): [`FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf`](../src/network/backend/runtime.c#L411), [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 2 caller function(s): [`FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`](../src/network/protocol/transfer.c#L340), [`FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets`](../src/network/protocol/transfer.c#L617)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)
- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 4 caller function(s): [`UiResizableWindowControl_DrawFrameTitleAndChildren`](../src/ui/controls/layout.c#L100), [`UiTitledWindowControl_DrawFrameTitleAndChildren`](../src/ui/controls/layout.c#L223), [`UiTitledWindowControl_LayoutFrameTitleAndChildren`](../src/ui/controls/layout.c#L620); +1 more
- [`ui/controls/lists` source](../src/ui/controls/lists.c) · [graph](callgraphs/ui/controls/lists.md): 1 caller function(s): [`UiPointerList_InitializeColumnLayout`](../src/ui/controls/lists.c#L2314)
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 16 caller function(s): [`UiFormattedContainer_RelocateWithPatchedTextPayloads`](../src/ui/controls/text.c#L3440), [`UiFramedTextButtonControl_DrawClipped`](../src/ui/controls/text.c#L1776), [`UiGraphicsAdapterTextButton_DrawFormattedAdapterText`](../src/ui/controls/text.c#L1025); +13 more
- [`ui/core/runtime` source](../src/ui/core/runtime.c) · [graph](callgraphs/ui/core/runtime.md): 2 caller function(s): [`UiRuntime_Initialize`](../src/ui/core/runtime.c#L194), [`UiRuntime_OpenFourValueDialogCf`](../src/ui/core/runtime.c#L47)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeFromCommandLine`](../src/ui/frontend/network.c#L342)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 4 caller function(s): [`FrontendPlayerMessageBuffer_PublishTextById`](../src/ui/frontend/player.c#L1473), [`FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers`](../src/ui/frontend/player.c#L192), [`FrontendPlayerSetup_SelectCountAndBuildLabel`](../src/ui/frontend/player.c#L645); +1 more
- [`ui/frontend/results` source](../src/ui/frontend/results.c) · [graph](callgraphs/ui/frontend/results.md): 8 caller function(s): [`FrontendResultsTable_DrawColourColumn`](../src/ui/frontend/results.c#L266), [`FrontendResultsTable_DrawColumnSequenceByType`](../src/ui/frontend/results.c#L5), [`FrontendResultsTable_DrawEconomyColumn`](../src/ui/frontend/results.c#L475); +5 more
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 4 caller function(s): [`FrontendRuntime_UpdatePointerContextAndSceneViewCf`](../src/ui/frontend/runtime.c#L1587), [`FrontendUiAction2011_Handler`](../src/ui/frontend/runtime.c#L2023), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749); +1 more
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 2 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129), [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 2 caller function(s): [`FrontendClientSession_DecrementTimeoutsAndCompactPlayers`](../src/ui/frontend/session.c#L398), [`FrontendHostSession_TickShutdownOrReadyConsensus`](../src/ui/frontend/session.c#L510)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendTaskAssignmentPage_Initialize`](../src/ui/frontend/settings.c#L5)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand150_HandlePlayerDepartureAndOwnership`](../src/ui/ingame/commands.c#L1467)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 7 caller function(s): [`InGameHud_UpdateStatusCountersAndSessionPrompts`](../src/ui/ingame/runtime.c#L2674), [`InGamePanel_RebuildPlayerStatusRows`](../src/ui/ingame/runtime.c#L2906), [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357); +4 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 1 caller function(s): [`InGameTechnologyPanel_Rebuild`](../src/ui/ingame/technology.c#L457)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime).

<a id="module-assets-text-richtext"></a>
### `assets/text/richtext`

[Source](../src/assets/text/richtext.c) · [Header](../include/thandor/assets/text/richtext.h) · [Call graph](callgraphs/assets/text/richtext.md) · [Graphviz](callgraphs/assets/text/richtext.dot)

This file owns rich-text parsing, layout, and rendering within asset loading, catalogs, and resource formats. Dominant function families: `RichTextCommandStream_*` (17), `RichTextMarkup_*` (1). Representative entry points: `RichTextCommandStream_MeasureWrappedBlockRegs`, `RichTextCommandStream_DrawWrappedBlockCf`, `RichTextCommandStream_DrawSingleLine`.

**Direct callers:** 26 module(s), 106 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 1 caller function(s): [`ScenarioCatalog_RefreshSelectedRecordLocalizedText`](../src/assets/scenario/catalog.c#L1496)
- [`core/error/runtime` source](../src/core/error/runtime.c) · [graph](callgraphs/core/error/runtime.md): 2 caller function(s): [`FatalErrorRuntime_DispatchPendingErrorCf`](../src/core/error/runtime.c#L57), [`FatalError_Exit`](../src/core/error/runtime.c#L168)
- [`core/settings/persistent` source](../src/core/settings/persistent.c) · [graph](callgraphs/core/settings/persistent.md): 1 caller function(s): [`PersistentSettings_Load`](../src/core/settings/persistent.c#L26)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 1 caller function(s): [`SelectionPanel_DrawNumberCellAndAdvanceRegs`](../src/gameplay/selection/runtime.c#L1888)
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 2 caller function(s): [`InGameSaveGameList_SelectAndRefreshDetail`](../src/gameplay/session/savegame.c#L5), [`InGameSaveGamePage_RebuildCatalog`](../src/gameplay/session/savegame.c#L130)
- [`network/backend/fallback_udp` source](../src/network/backend/fallback_udp.c) · [graph](callgraphs/network/backend/fallback_udp.md): 1 caller function(s): [`NetworkFallback_ParsePeerEndpointCf`](../src/network/backend/fallback_udp.c#L280)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 2 caller function(s): [`FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf`](../src/network/backend/runtime.c#L411), [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 2 caller function(s): [`FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`](../src/network/protocol/transfer.c#L340), [`FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets`](../src/network/protocol/transfer.c#L617)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)
- [`platform/filesystem/win32` source](../src/platform/filesystem/win32.c) · [graph](callgraphs/platform/filesystem/win32.md): 8 caller function(s): [`Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf`](../src/platform/filesystem/win32.c#L724), [`Win32File_CopyCf`](../src/platform/filesystem/win32.c#L420), [`Win32File_CreateDirectoryRecursiveCf`](../src/platform/filesystem/win32.c#L443); +5 more
- [`ui/controls/buttons` source](../src/ui/controls/buttons.c) · [graph](callgraphs/ui/controls/buttons.md): 1 caller function(s): [`UiConditionalActionControl_DrawClipped`](../src/ui/controls/buttons.c#L532)
- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 4 caller function(s): [`UiHorizontalGaugeControl_DrawFrameFillAndLabel`](../src/ui/controls/layout.c#L687), [`UiResizableWindowControl_DrawFrameTitleAndChildren`](../src/ui/controls/layout.c#L100), [`UiTitledWindowControl_DrawFrameTitleAndChildren`](../src/ui/controls/layout.c#L223); +1 more
- [`ui/controls/lists` source](../src/ui/controls/lists.c) · [graph](callgraphs/ui/controls/lists.md): 4 caller function(s): [`UiCatalogEntryControl_DrawClipped`](../src/ui/controls/lists.c#L2570), [`UiListControl_DrawRowsAndSelection`](../src/ui/controls/lists.c#L2150), [`UiTimedListControl_DrawRowsAndSelection`](../src/ui/controls/lists.c#L2380); +1 more
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 16 caller function(s): [`UiFormattedContainer_RelocateWithPatchedTextPayloads`](../src/ui/controls/text.c#L3440), [`UiFramedTextButtonControl_DrawClipped`](../src/ui/controls/text.c#L1776), [`UiGraphicsAdapterTextButton_DrawFormattedAdapterText`](../src/ui/controls/text.c#L1025); +13 more
- [`ui/core/runtime` source](../src/ui/core/runtime.c) · [graph](callgraphs/ui/core/runtime.md): 1 caller function(s): [`UiRuntime_OpenFourValueDialogCf`](../src/ui/core/runtime.c#L47)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 2 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5), [`FrontendNetworkSetupPage_InitializeFromCommandLine`](../src/ui/frontend/network.c#L342)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 5 caller function(s): [`FrontendPlayerMessageBuffer_PublishTextById`](../src/ui/frontend/player.c#L1473), [`FrontendPlayerMessage_SubmitSevenSlotText`](../src/ui/frontend/player.c#L5), [`FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers`](../src/ui/frontend/player.c#L192); +2 more
- [`ui/frontend/results` source](../src/ui/frontend/results.c) · [graph](callgraphs/ui/frontend/results.md): 7 caller function(s): [`FrontendResultsTable_DrawColourColumn`](../src/ui/frontend/results.c#L266), [`FrontendResultsTable_DrawEconomyColumn`](../src/ui/frontend/results.c#L475), [`FrontendResultsTable_DrawFactionColumn`](../src/ui/frontend/results.c#L316); +4 more
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 3 caller function(s): [`FrontendRuntime_UpdatePointerContextAndSceneViewCf`](../src/ui/frontend/runtime.c#L1587), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749), [`Frontend_PlaySelectedEndMovie`](../src/ui/frontend/runtime.c#L2552)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 2 caller function(s): [`FrontendClientSession_DecrementTimeoutsAndCompactPlayers`](../src/ui/frontend/session.c#L398), [`FrontendHostSession_TickShutdownOrReadyConsensus`](../src/ui/frontend/session.c#L510)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendTaskAssignmentPage_Initialize`](../src/ui/frontend/settings.c#L5)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand150_HandlePlayerDepartureAndOwnership`](../src/ui/ingame/commands.c#L1467)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 9 caller function(s): [`InGameHud_UpdateStatusCountersAndSessionPrompts`](../src/ui/ingame/runtime.c#L2674), [`InGamePanel_RebuildPlayerStatusRows`](../src/ui/ingame/runtime.c#L2906), [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357); +6 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 1 caller function(s): [`InGameTechnologyPanel_Rebuild`](../src/ui/ingame/technology.c#L457)
- [`ui/support/runtime` source](../src/ui/support/runtime.c) · [graph](callgraphs/ui/support/runtime.md): 1 caller function(s): [`RecentTextHistory_Insert`](../src/ui/support/runtime.c#L61)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources).

## Audio

<a id="module-audio-backend-runtime"></a>
### `audio/backend/runtime`

[Source](../src/audio/backend/runtime.c) · [Header](../include/thandor/audio/backend/runtime.h) · [Call graph](callgraphs/audio/backend/runtime.md) · [Graphviz](callgraphs/audio/backend/runtime.dot)

This file owns runtime state and lifecycle operations within audio decoding, playback, and spatial sound. Dominant function families: `DirectSound_*` (13), `SoundBackendDisabled_*` (11). Representative entry points: `DirectSound_Shutdown`, `SoundBackendDisabled_CreateSampleVoiceSet`, `SoundBackendDisabled_ReleaseSampleVoiceSet`.

**Direct callers:** 2 module(s), 2 cross-module call edge(s).

- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)

**Calls into:** [`audio/codec/sam`](#module-audio-codec-sam), [`core/math/fixed`](#module-core-math-fixed), [`core/memory/allocator`](#module-core-memory-allocator), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime).

<a id="module-audio-codec-sam"></a>
### `audio/codec/sam`

[Source](../src/audio/codec/sam.c) · [Header](../include/thandor/audio/codec/sam.h) · [Call graph](callgraphs/audio/codec/sam.md) · [Graphviz](callgraphs/audio/codec/sam.dot)

This file owns SAM audio decoding and synthesis within audio decoding, playback, and spatial sound. Dominant function families: `SoundSample_*` (3), `SoundCoefficientTransform_*` (1). Representative entry points: `SoundCoefficientTransform_ApplyCosineBanksMmx`, `SoundSample_DecodeCoefficientBlockToPcmMmx`, `SoundSample_DecodeCoefficientBlockToMonoPcmMmx`.

**Direct callers:** 1 module(s), 2 cross-module call edge(s).

- [`audio/backend/runtime` source](../src/audio/backend/runtime.c) · [graph](callgraphs/audio/backend/runtime.md): 1 caller function(s): [`DirectSound_CreateSampleVoiceSet`](../src/audio/backend/runtime.c#L251)

<a id="module-audio-spatial-runtime"></a>
### `audio/spatial/runtime`

[Source](../src/audio/spatial/runtime.c) · [Header](../include/thandor/audio/spatial/runtime.h) · [Call graph](callgraphs/audio/spatial/runtime.md) · [Graphviz](callgraphs/audio/spatial/runtime.dot)

This file owns runtime state and lifecycle operations within audio decoding, playback, and spatial sound. Dominant function families: `SpatialSoundSlot_*` (4), `SpatialSound_*` (3), `SpatialSoundPool_*` (3). Representative entry points: `SpatialSoundPool_Init`, `SpatialSound_RebuildListenerTransformFromPose`, `SpatialSound_PlayPositionedOneShot`.

**Direct callers:** 9 module(s), 27 cross-module call edge(s).

- [`gameplay/army/audio` source](../src/gameplay/army/audio.c) · [graph](callgraphs/gameplay/army/audio.md): 7 caller function(s): [`ArmyRuntimeAudio_UpdateAssetProjectedSound`](../src/gameplay/army/audio.c#L336), [`ArmyRuntimeAudio_UpdateConditionalProjectedSound`](../src/gameplay/army/audio.c#L243), [`ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA`](../src/gameplay/army/audio.c#L65); +4 more
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 2 caller function(s): [`ArmyArticulatedRuntime_UpdateContactChildAndEffects`](../src/gameplay/army/movement.c#L2542), [`ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation`](../src/gameplay/army/movement.c#L1079)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 9 caller function(s): [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`](../src/gameplay/army/runtime.c#L406), [`ArmyRuntimeClass_UpdatePositionedSoundsVariantA`](../src/gameplay/army/runtime.c#L2120); +6 more
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`GameRuntime_InitializeSpatialAudioAndRenderingCf`](../src/platform/bootstrap/runtime.c#L561)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotRuntimeMaintenance_UpdateHierarchyProjectedSound`](../src/world/shots/maintenance.c#L126)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed).

## Core

<a id="module-core-error-runtime"></a>
### `core/error/runtime`

[Source](../src/core/error/runtime.c) · [Header](../include/thandor/core/error/runtime.h) · [Call graph](callgraphs/core/error/runtime.md) · [Graphviz](callgraphs/core/error/runtime.dot)

This file owns runtime state and lifecycle operations within shared low-level runtime services. Dominant function families: `ErrorRuntime_*` (3), `FatalError_*` (3), `ErrorSystem_*` (1). Representative entry points: `ErrorSystem_Init`, `ErrorRuntime_CallbackAlwaysFailCf`, `ErrorRuntime_CallbackReturnCode8`.

**Direct callers:** 4 module(s), 4 cross-module call edge(s).

- [`core/memory/allocator` source](../src/core/memory/allocator.c) · [graph](callgraphs/core/memory/allocator.md): 1 caller function(s): [`ArenaHeap_Init`](../src/core/memory/allocator.c#L134)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`platform/filesystem/win32` source](../src/platform/filesystem/win32.c) · [graph](callgraphs/platform/filesystem/win32.md): 1 caller function(s): [`FileSystem_Init`](../src/platform/filesystem/win32.c#L5)
- [`ui/core/runtime` source](../src/ui/core/runtime.c) · [graph](callgraphs/ui/core/runtime.md): 1 caller function(s): [`UiRuntime_Initialize`](../src/ui/core/runtime.c#L194)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/memory/synchronization`](#module-core-memory-synchronization), [`ui/controls/layout`](#module-ui-controls-layout).

<a id="module-core-math-fixed"></a>
### `core/math/fixed`

[Source](../src/core/math/fixed.c) · [Header](../include/thandor/core/math/fixed.h) · [Call graph](callgraphs/core/math/fixed.md) · [Graphviz](callgraphs/core/math/fixed.dot)

This file owns fixed-point math and transform helpers within shared low-level runtime services. Dominant function families: `FixedMath_*` (16), `FixedTransform_*` (12), `FixedVec3_*` (4). Representative entry points: `FixedTransform_ComposeEulerAnglesRegs`, `FixedMath_VectorToAnglesAndLength3Regs`, `FixedMath_VectorToAnglesAndLengthVec3Regs`.

**Direct callers:** 27 module(s), 183 cross-module call edge(s).

- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 1 caller function(s): [`ShotDefinition_ComputeLaunchAnglesRegs`](../src/assets/shot/catalog.c#L117)
- [`audio/backend/runtime` source](../src/audio/backend/runtime.c) · [graph](callgraphs/audio/backend/runtime.md): 1 caller function(s): [`DirectSound_Init`](../src/audio/backend/runtime.c#L147)
- [`audio/spatial/runtime` source](../src/audio/spatial/runtime.c) · [graph](callgraphs/audio/spatial/runtime.md): 3 caller function(s): [`SpatialSound_PlayPositionedOneShot`](../src/audio/spatial/runtime.c#L65), [`SpatialSound_RebuildListenerTransformFromPose`](../src/audio/spatial/runtime.c#L28), [`SpatialSound_UpdateDesiredPositionedGains`](../src/audio/spatial/runtime.c#L131)
- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 1 caller function(s): [`AiCombatTarget_EvaluateCandidateScore`](../src/gameplay/ai/combat.c#L218)
- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 1 caller function(s): [`AiSiteCandidate_AddTerrainFeatureCellIfSeparated`](../src/gameplay/ai/placement.c#L256)
- [`gameplay/ai/units` source](../src/gameplay/ai/units.c) · [graph](callgraphs/gameplay/ai/units.md): 1 caller function(s): [`AiUnitBehavior_UpdateSpecialClass12Entity`](../src/gameplay/ai/units.c#L378)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyWeaponRuntime_TestTargetLineOfFireCf`](../src/gameplay/army/combat.c#L443)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 15 caller function(s): [`ArmyArticulatedRuntime_InitializeLeftTerrainContact`](../src/gameplay/army/movement.c#L2960), [`ArmyArticulatedRuntime_InitializeRightTerrainContact`](../src/gameplay/army/movement.c#L3023), [`ArmyArticulatedRuntime_InitializeTerrainContactGeometry`](../src/gameplay/army/movement.c#L1550); +12 more
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 3 caller function(s): [`ArmyPlacementCandidate_TestModelAnchorDistanceCf`](../src/gameplay/army/placement.c#L1146), [`ArmyPlacementCandidate_TestOffsetClearanceCf`](../src/gameplay/army/placement.c#L5), [`ArmyPlacementCollision_TestCandidateAndClearanceCf`](../src/gameplay/army/placement.c#L1029)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 8 caller function(s): [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`](../src/gameplay/army/runtime.c#L5), [`ArmyRuntime_ClassCommandHandlerGroupACf`](../src/gameplay/army/runtime.c#L1231); +5 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`OldUnitRuntime_RebuildScenarioReplayTables`](../src/gameplay/faction/runtime.c#L107)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 2 caller function(s): [`SelectionOverlay_DrawMarkerACForWorldSurfacePoint`](../src/gameplay/selection/overlay.c#L522), [`SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints`](../src/gameplay/selection/overlay.c#L454)
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 6 caller function(s): [`GraphicsObject_ConvertWorldDirectionAnglesToLocalAnglesRegs`](../src/graphics/core/runtime.c#L412), [`GraphicsObject_ExtractTransformEulerAnglesRegs`](../src/graphics/core/runtime.c#L393), [`GraphicsObject_RebuildTransformHierarchyRecursive`](../src/graphics/core/runtime.c#L469); +3 more
- [`graphics/render/model` source](../src/graphics/render/model.c) · [graph](callgraphs/graphics/render/model.md): 9 caller function(s): [`ModelProjectedBounds_AccumulateNode`](../src/graphics/render/model.c#L621), [`ModelProjectedBounds_ExpandWithCurrentScratchPoint`](../src/graphics/render/model.c#L1109), [`ModelRender_ComputeFacingDotQ12`](../src/graphics/render/model.c#L667); +6 more
- [`graphics/render/shading` source](../src/graphics/render/shading.c) · [graph](callgraphs/graphics/render/shading.md): 3 caller function(s): [`GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy`](../src/graphics/render/shading.c#L5), [`GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds`](../src/graphics/render/shading.c#L2883), [`GraphicsShadingRuntime_RebuildCompactLightingRecords`](../src/graphics/render/shading.c#L2372)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`MovieColor_ComputeChromaCodeFromRgb888`](../src/movie/runtime/playback.c#L1593)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_Tick`](../src/ui/frontend/runtime.c#L1382)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 9 caller function(s): [`ModelNodeRuntime_AccumulateTransformedBoundsRecursive`](../src/world/model/hierarchy.c#L106), [`ModelNodeRuntime_BuildBillboardRotation`](../src/world/model/hierarchy.c#L200), [`ModelNodeRuntime_BuildViewFacingRotation`](../src/world/model/hierarchy.c#L170); +6 more
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 3 caller function(s): [`ModelRuntime_CullAndRenderHierarchyRecursive`](../src/world/model/runtime.c#L66), [`ModelRuntime_RaycastCandidateListNearestCf`](../src/world/model/runtime.c#L254), [`ModelRuntime_RenderHierarchyRecursiveAlternatePath`](../src/world/model/runtime.c#L204)
- [`world/motion/runtime` source](../src/world/motion/runtime.c) · [graph](callgraphs/world/motion/runtime.md): 7 caller function(s): [`WorldMotion_AdjustDistanceClampAndRecomputePosition`](../src/world/motion/runtime.c#L231), [`WorldMotion_AdjustHeadingAndRecomputePosition`](../src/world/motion/runtime.c#L189), [`WorldMotion_AdjustPitchClampAndRecomputePosition`](../src/world/motion/runtime.c#L309); +4 more
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 4 caller function(s): [`WorldRuntimeNode_IsPositionInsideBoundsCf`](../src/world/runtime/core.c#L736), [`WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface`](../src/world/runtime/core.c#L1401), [`WorldRuntime_SetPosition60AndDistanceFromPosition80`](../src/world/runtime/core.c#L503); +1 more
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 2 caller function(s): [`ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint`](../src/world/shots/maintenance.c#L5), [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)
- [`world/terrain/grid` source](../src/world/terrain/grid.c) · [graph](callgraphs/world/terrain/grid.md): 13 caller function(s): [`FieldGridCell_ComputeDirectionalLightColor`](../src/world/terrain/grid.c#L3632), [`FieldGridCell_RecomputeTriangleNormalAngles`](../src/world/terrain/grid.c#L3557), [`FieldGrid_InterpolateTerrainHeightAndNormal`](../src/world/terrain/grid.c#L1389); +10 more
- [`world/terrain/height` source](../src/world/terrain/height.c) · [graph](callgraphs/world/terrain/height.md): 1 caller function(s): [`TerrainTriangle_IntersectRayDistanceCf`](../src/world/terrain/height.c#L536)
- [`world/terrain/projection` source](../src/world/terrain/projection.c) · [graph](callgraphs/world/terrain/projection.md): 2 caller function(s): [`TerrainProjectedVertex_TransformProjectAndShadeVariantA`](../src/world/terrain/projection.c#L1498), [`TerrainProjectedVertex_TransformProjectAndShadeVariantB`](../src/world/terrain/projection.c#L1679)

<a id="module-core-math-geometry"></a>
### `core/math/geometry`

[Source](../src/core/math/geometry.c) · [Header](../include/thandor/core/math/geometry.h) · [Call graph](callgraphs/core/math/geometry.md) · [Graphviz](callgraphs/core/math/geometry.dot)

This file owns geometry helpers within shared low-level runtime services. Dominant function families: `Triangle2D_*` (1). Representative entry points: `Triangle2D_ComputeBarycentricWeightsQ12Packed`.

**Direct callers:** 1 module(s), 1 cross-module call edge(s).

- [`world/terrain/projection` source](../src/world/terrain/projection.c) · [graph](callgraphs/world/terrain/projection.md): 1 caller function(s): [`TerrainProjectedTriangle_ClipInterpolateAndQueueTextured`](../src/world/terrain/projection.c#L1838)

<a id="module-core-math-interpolation"></a>
### `core/math/interpolation`

[Source](../src/core/math/interpolation.c) · [Header](../include/thandor/core/math/interpolation.h) · [Call graph](callgraphs/core/math/interpolation.md) · [Graphviz](callgraphs/core/math/interpolation.dot)

This file owns interpolation and numeric transition helpers within shared low-level runtime services. Dominant function families: `CubicSpline_*` (6), `WorldMotionSpline_*` (4), `InterpolationState_*` (1). Representative entry points: `WorldMotionSpline_EvaluateAndApplyAtTime`, `WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf`, `WorldMotionSpline_BuildSixChannelCurves`.

**Direct callers:** 6 module(s), 8 cross-module call edge(s).

- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 2 caller function(s): [`FrontendRomTransition_InitializeFromRecord`](../src/assets/rom/runtime.c#L743), [`FrontendRomTransition_ProcessPendingRecord`](../src/assets/rom/runtime.c#L203)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 1 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 2 caller function(s): [`FrontendRuntime_UpdatePointerContextAndSceneViewCf`](../src/ui/frontend/runtime.c#L1587), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)

**Calls into:** [`world/runtime/core`](#module-world-runtime-core).

<a id="module-core-math-random"></a>
### `core/math/random`

[Source](../src/core/math/random.c) · [Header](../include/thandor/core/math/random.h) · [Call graph](callgraphs/core/math/random.md) · [Graphviz](callgraphs/core/math/random.dot)

This file owns random-number state and selection helpers within shared low-level runtime services. Dominant function families: `Random_*` (6). Representative entry points: `Random_NextPrimary`, `Random_NextSecondary`, `Random_SetBothSeeds`.

**Direct callers:** 10 module(s), 16 cross-module call edge(s).

- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`Movie_Open`](../src/movie/runtime/playback.c#L129)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_HandleHandshakeAndPlayerStatePackets`](../src/network/backend/runtime.c#L5)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 2 caller function(s): [`FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`](../src/network/protocol/transfer.c#L340), [`UiTransferMailbox_RandomizeSequenceToken`](../src/network/protocol/transfer.c#L1168)
- [`ui/controls/buttons` source](../src/ui/controls/buttons.c) · [graph](callgraphs/ui/controls/buttons.md): 1 caller function(s): [`UiSpriteButtonControl_Relocate`](../src/ui/controls/buttons.c#L37)
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 1 caller function(s): [`UiPointer_DispatchPendingEvents`](../src/ui/controls/input.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendUiAction200F_Handler`](../src/ui/frontend/runtime.c#L2473)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 2 caller function(s): [`FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag`](../src/ui/frontend/session.c#L130), [`FrontendSessionAction_ResetNetworkAndReturnToMainPage`](../src/ui/frontend/session.c#L103)
- [`world/terrain/grid` source](../src/world/terrain/grid.c) · [graph](callgraphs/world/terrain/grid.md): 1 caller function(s): [`FieldGrid_InitializeRuntimeCellsAndBoundaryFlags`](../src/world/terrain/grid.c#L1936)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 2 caller function(s): [`TerrainVisualResources_LoadAndClearCellOverlayFlags`](../src/world/terrain/visuals.c#L400), [`TerrainVisualResources_LoadPrimary`](../src/world/terrain/visuals.c#L228)

<a id="module-core-memory-allocator"></a>
### `core/memory/allocator`

[Source](../src/core/memory/allocator.c) · [Header](../include/thandor/core/memory/allocator.h) · [Call graph](callgraphs/core/memory/allocator.md) · [Graphviz](callgraphs/core/memory/allocator.dot)

This file owns memory allocation and heap management within shared low-level runtime services. Dominant function families: `ArenaHeap_*` (8), `PriorityPairHeap_*` (2), `DwordBlock64Array_*` (1). Representative entry points: `PriorityPairHeap_SiftUp`, `PriorityPairHeap_SiftDown`, `DwordBlock64Array_ContainsExactRecordCf`.

**Direct callers:** 10 module(s), 25 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 1 caller function(s): [`FrontendScenarioTransfer_ProcessReceivedAsset`](../src/assets/scenario/catalog.c#L626)
- [`audio/backend/runtime` source](../src/audio/backend/runtime.c) · [graph](callgraphs/audio/backend/runtime.md): 3 caller function(s): [`DirectSound_CreatePcmVoiceSet`](../src/audio/backend/runtime.c#L393), [`DirectSound_CreateSampleVoiceSet`](../src/audio/backend/runtime.c#L251), [`DirectSound_Init`](../src/audio/backend/runtime.c#L147)
- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`graphics/backend/directdraw` source](../src/graphics/backend/directdraw.c) · [graph](callgraphs/graphics/backend/directdraw.md): 1 caller function(s): [`GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf`](../src/graphics/backend/directdraw.c#L180)
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 3 caller function(s): [`GraphicsCursor_RestoreSurfaceBackground`](../src/graphics/core/runtime.c#L1144), [`GraphicsCursor_SaveSurfaceBackground`](../src/graphics/core/runtime.c#L1000), [`Graphics_SetViewportAndClearDepth`](../src/graphics/core/runtime.c#L751)
- [`graphics/resources/framebuffer` source](../src/graphics/resources/framebuffer.c) · [graph](callgraphs/graphics/resources/framebuffer.md): 3 caller function(s): [`GraphicsFramebuffer_BeginAccess`](../src/graphics/resources/framebuffer.c#L367), [`GraphicsFramebuffer_CaptureRegion16Bit`](../src/graphics/resources/framebuffer.c#L133), [`GraphicsFramebuffer_CaptureRegion32Bit`](../src/graphics/resources/framebuffer.c#L255)
- [`graphics/resources/texture` source](../src/graphics/resources/texture.c) · [graph](callgraphs/graphics/resources/texture.md): 8 caller function(s): [`GraphicsTexture_CreateDeviceTexture`](../src/graphics/resources/texture.c#L3352), [`GraphicsTexture_CreateStagingTexture`](../src/graphics/resources/texture.c#L3700), [`GraphicsTexture_UploadAlpha_1x`](../src/graphics/resources/texture.c#L2910); +5 more
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`platform/filesystem/win32` source](../src/platform/filesystem/win32.c) · [graph](callgraphs/platform/filesystem/win32.md): 1 caller function(s): [`FileSystem_Init`](../src/platform/filesystem/win32.c#L5)
- [`world/pathing/grid` source](../src/world/pathing/grid.c) · [graph](callgraphs/world/pathing/grid.md): 1 caller function(s): [`EntityPathing_RebuildOverlappingGroupRoutes`](../src/world/pathing/grid.c#L844)

**Calls into:** [`core/error/runtime`](#module-core-error-runtime).

<a id="module-core-memory-synchronization"></a>
### `core/memory/synchronization`

[Source](../src/core/memory/synchronization.c) · [Header](../include/thandor/core/memory/synchronization.h) · [Call graph](callgraphs/core/memory/synchronization.md) · [Graphviz](callgraphs/core/memory/synchronization.dot)

This file owns runtime synchronization primitives within shared low-level runtime services. Dominant function families: `SpinLock_*` (4), `Runtime_*` (1). Representative entry points: `SpinLock_Acquire`, `SpinLock_TryAcquireFlags`, `SpinLock_Release`.

**Direct callers:** 3 module(s), 3 cross-module call edge(s).

- [`core/error/runtime` source](../src/core/error/runtime.c) · [graph](callgraphs/core/error/runtime.md): 1 caller function(s): [`FatalError_Exit`](../src/core/error/runtime.c#L168)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`platform/system/win32` source](../src/platform/system/win32.c) · [graph](callgraphs/platform/system/win32.md): 1 caller function(s): [`Win32_PumpMessages`](../src/platform/system/win32.c#L5)

**Calls into:** [`audio/backend/runtime`](#module-audio-backend-runtime), [`core/memory/allocator`](#module-core-memory-allocator), [`core/settings/persistent`](#module-core-settings-persistent), [`graphics/core/runtime`](#module-graphics-core-runtime), [`network/backend/runtime`](#module-network-backend-runtime), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime), [`platform/filesystem/win32`](#module-platform-filesystem-win32), [`platform/input/devices`](#module-platform-input-devices), [`platform/system/time_locale`](#module-platform-system-time-locale), [`ui/core/runtime`](#module-ui-core-runtime).

<a id="module-core-settings-persistent"></a>
### `core/settings/persistent`

[Source](../src/core/settings/persistent.c) · [Header](../include/thandor/core/settings/persistent.h) · [Call graph](callgraphs/core/settings/persistent.md) · [Graphviz](callgraphs/core/settings/persistent.dot)

This file owns persistent settings storage within shared low-level runtime services. Dominant function families: `PersistentSettings_*` (6). Representative entry points: `PersistentSettings_Flush`, `PersistentSettings_Load`, `PersistentSettings_ReadDword`.

**Direct callers:** 8 module(s), 85 cross-module call edge(s).

- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718); +1 more
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 3 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593), [`Game_Run`](../src/platform/bootstrap/runtime.c#L508), [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 3 caller function(s): [`FrontendUiAction2011_Handler`](../src/ui/frontend/runtime.c#L2023), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 27 caller function(s): [`FrontendAudioSettings_OpenAndSynchronize`](../src/ui/frontend/settings.c#L747), [`FrontendAudioSettings_SetEffectsEnabled`](../src/ui/frontend/settings.c#L964), [`FrontendAudioSettings_SetEffectsGain`](../src/ui/frontend/settings.c#L1197); +24 more
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 21 caller function(s): [`InGameAudioSettings_OpenAndSynchronize`](../src/ui/ingame/settings.c#L526), [`InGameAudioSettings_SetEffectsEnabled`](../src/ui/ingame/settings.c#L773), [`InGameAudioSettings_SetEffectsGain`](../src/ui/ingame/settings.c#L976); +18 more
- [`world/motion/runtime` source](../src/world/motion/runtime.c) · [graph](callgraphs/world/motion/runtime.md): 1 caller function(s): [`WorldRuntime_ApplyEdgeScrollAndGetCursorFrame`](../src/world/motion/runtime.c#L5)

**Calls into:** [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/path`](#module-core-text-path), [`platform/filesystem/win32`](#module-platform-filesystem-win32).

<a id="module-core-text-path"></a>
### `core/text/path`

[Source](../src/core/text/path.c) · [Header](../include/thandor/core/text/path.h) · [Call graph](callgraphs/core/text/path.md) · [Graphviz](callgraphs/core/text/path.dot)

This file owns path and filename manipulation within shared low-level runtime services. Dominant function families: `WidePath_*` (5). Representative entry points: `WidePath_GetExtensionCode`, `WidePath_SetExtensionCode`, `WidePath_SplitParentAndLeaf`.

**Direct callers:** 22 module(s), 54 cross-module call edge(s).

- [`assets/effect/catalog` source](../src/assets/effect/catalog.c) · [graph](callgraphs/assets/effect/catalog.md): 1 caller function(s): [`EffectDefinition_RegisterAndLoadSpriteCf`](../src/assets/effect/catalog.c#L98)
- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_RegisterAndResolveReferencesCf`](../src/assets/model/definitions.c#L485)
- [`assets/package/runtime` source](../src/assets/package/runtime.c) · [graph](callgraphs/assets/package/runtime.md): 4 caller function(s): [`Package_LoadEntry`](../src/assets/package/runtime.c#L402), [`Package_LoadEntryIntoBuffer`](../src/assets/package/runtime.c#L192), [`Package_Mount`](../src/assets/package/runtime.c#L468); +1 more
- [`assets/resource/runtime` source](../src/assets/resource/runtime.c) · [graph](callgraphs/assets/resource/runtime.md): 1 caller function(s): [`Resource_Load`](../src/assets/resource/runtime.c#L66)
- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 1 caller function(s): [`RomAssetRecord_RegisterAndRelocate`](../src/assets/rom/runtime.c#L555)
- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 7 caller function(s): [`FrontendScenarioSelectionPage_InitializeAndApplyMapOption`](../src/assets/scenario/catalog.c#L103), [`FrontendScenarioSelection_ActivateSelectedRecord`](../src/assets/scenario/catalog.c#L1578), [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995); +4 more
- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 1 caller function(s): [`ShotDefinition_RegisterAndResolveReferencesCf`](../src/assets/shot/catalog.c#L235)
- [`core/settings/persistent` source](../src/core/settings/persistent.c) · [graph](callgraphs/core/settings/persistent.md): 1 caller function(s): [`PersistentSettings_Load`](../src/core/settings/persistent.c#L26)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_InitializePoolAndGraphicsCf`](../src/gameplay/army/runtime.c#L1453)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`LevelAsset_PrepareEndingMoviePathCf`](../src/gameplay/session/level.c#L5)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085)
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 3 caller function(s): [`InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog`](../src/gameplay/session/savegame.c#L90), [`InGameSaveGamePage_RebuildCatalog`](../src/gameplay/session/savegame.c#L130), [`InGameSaveGame_SaveSelectedOrTypedName`](../src/gameplay/session/savegame.c#L287)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`Movie_Open`](../src/movie/runtime/playback.c#L129)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)
- [`platform/filesystem/win32` source](../src/platform/filesystem/win32.c) · [graph](callgraphs/platform/filesystem/win32.md): 2 caller function(s): [`FileSystem_Init`](../src/platform/filesystem/win32.c#L5), [`Win32File_CreateDirectoryRecursiveCf`](../src/platform/filesystem/win32.c#L443)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)
- [`ui/support/runtime` source](../src/ui/support/runtime.c) · [graph](callgraphs/ui/support/runtime.md): 1 caller function(s): [`PcxPreview_Load64x64PaletteAndPixelsCf`](../src/ui/support/runtime.c#L193)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntime_InitGraphicsResources`](../src/world/effects/runtime.c#L36)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntime_InitGraphicsResources`](../src/world/shots/runtime.c#L97)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 2 caller function(s): [`TerrainVisualResources_LoadAndClearCellOverlayFlags`](../src/world/terrain/visuals.c#L400), [`TerrainVisualResources_LoadPrimary`](../src/world/terrain/visuals.c#L228)

<a id="module-core-text-string"></a>
### `core/text/string`

[Source](../src/core/text/string.c) · [Header](../include/thandor/core/text/string.h) · [Call graph](callgraphs/core/text/string.md) · [Graphviz](callgraphs/core/text/string.dot)

This file owns string manipulation and conversion within shared low-level runtime services. Dominant function families: `Text_*` (1), `Utf16_*` (1), `Utf16String_*` (1). Representative entry points: `WideNumber_FormatUtf16`, `Utf16String_CompareAsciiCaseInsensitiveFlags`, `Text_CopyNarrowToUtf16Cf`.

**Direct callers:** 12 module(s), 28 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 1 caller function(s): [`FrontendScenarioSelectionPage_InitializeAndApplyMapOption`](../src/assets/scenario/catalog.c#L103)
- [`graphics/backend/direct3d` source](../src/graphics/backend/direct3d.c) · [graph](callgraphs/graphics/backend/direct3d.md): 1 caller function(s): [`Direct3D_EnumDeviceCallback`](../src/graphics/backend/direct3d.c#L5)
- [`graphics/backend/directdraw` source](../src/graphics/backend/directdraw.c) · [graph](callgraphs/graphics/backend/directdraw.md): 1 caller function(s): [`DirectDraw_EnumAdapterCallback`](../src/graphics/backend/directdraw.c#L64)
- [`graphics/backend/glide` source](../src/graphics/backend/glide.c) · [graph](callgraphs/graphics/backend/glide.md): 1 caller function(s): [`Glide3_InitAndEnumerate`](../src/graphics/backend/glide.c#L692)
- [`network/backend/fallback_udp` source](../src/network/backend/fallback_udp.c) · [graph](callgraphs/network/backend/fallback_udp.md): 2 caller function(s): [`NetworkFallback_FormatAddressUtf16`](../src/network/backend/fallback_udp.c#L415), [`NetworkFallback_FormatPeerAddress`](../src/network/backend/fallback_udp.c#L325)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 7 caller function(s): [`BootstrapApi_ResolveBindingByDestination`](../src/platform/bootstrap/runtime.c#L352), [`CommandLine_Parse`](../src/platform/bootstrap/runtime.c#L1278), [`DynAPI_Bootstrap`](../src/platform/bootstrap/runtime.c#L1176); +4 more
- [`platform/filesystem/win32` source](../src/platform/filesystem/win32.c) · [graph](callgraphs/platform/filesystem/win32.md): 3 caller function(s): [`FileSystem_Init`](../src/platform/filesystem/win32.c#L5), [`Win32FileSystem_EnumerateDirectoryOrVolumeEntriesCf`](../src/platform/filesystem/win32.c#L724), [`Win32File_GetCurrentDirectoryCf`](../src/platform/filesystem/win32.c#L880)
- [`platform/system/time_locale` source](../src/platform/system/time_locale.c) · [graph](callgraphs/platform/system/time_locale.md): 5 caller function(s): [`Locale_FormatCurrentDateUtf16`](../src/platform/system/time_locale.c#L246), [`Locale_FormatCurrentTimeUtf16`](../src/platform/system/time_locale.c#L402), [`Locale_FormatDateFieldsUtf16`](../src/platform/system/time_locale.c#L190); +2 more
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 2 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5), [`FrontendNetworkSetupPage_InitializeFromCommandLine`](../src/ui/frontend/network.c#L342)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 2 caller function(s): [`FrontendPlayerMessageBuffer_PublishTextById`](../src/ui/frontend/player.c#L1473), [`FrontendPlayerTextCommand_PublishConditionalRichText`](../src/ui/frontend/player.c#L1191)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendDebugOverlay_RefreshCountersAndWorldCoordinates`](../src/ui/frontend/runtime.c#L3440)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameHud_UpdateStatusCountersAndSessionPrompts`](../src/ui/ingame/runtime.c#L2674)

## Gameplay

<a id="module-gameplay-ai-combat"></a>
### `gameplay/ai/combat`

[Source](../src/gameplay/ai/combat.c) · [Header](../include/thandor/gameplay/ai/combat.h) · [Call graph](callgraphs/gameplay/ai/combat.md) · [Graphviz](callgraphs/gameplay/ai/combat.dot)

This file owns combat targeting and resolution within game simulation and player/AI behavior. Dominant function families: `AiCombatTarget_*` (2), `AiCombatDecision_*` (1), `AiUnitGroup_*` (1). Representative entry points: `AiCombatDecision_UpdateTargetAssignment`, `AiUnitGroup_AssignCollectedEntitiesToBestTarget`, `AiCombatTarget_SelectBestCandidate`.

**Direct callers:** 2 module(s), 3 cross-module call edge(s).

- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 1 caller function(s): [`AiRuntime_DispatchFactionPlanningPhase`](../src/gameplay/ai/planning.c#L300)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 2 caller function(s): [`ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers`](../src/gameplay/army/runtime.c#L1410), [`ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate`](../src/gameplay/army/runtime.c#L3591)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/combat`](#module-gameplay-army-combat), [`gameplay/army/movement`](#module-gameplay-army-movement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`world/model/runtime`](#module-world-model-runtime).

<a id="module-gameplay-ai-placement"></a>
### `gameplay/ai/placement`

[Source](../src/gameplay/ai/placement.c) · [Header](../include/thandor/gameplay/ai/placement.h) · [Call graph](callgraphs/gameplay/ai/placement.md) · [Graphviz](callgraphs/gameplay/ai/placement.dot)

This file owns placement, site selection, and spatial validation within game simulation and player/AI behavior. Dominant function families: `AiPlacement_*` (7), `AiSiteCandidate_*` (3), `AiCandidatePlanning_*` (2). Representative entry points: `AiPlacement_ReserveAdditionalSpecialSite`, `AiCandidatePlanning_AddSpecialSiteCandidate`, `AiSiteCandidate_AddGeneralCellIfSeparated`.

**Direct callers:** 3 module(s), 11 cross-module call edge(s).

- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 3 caller function(s): [`AiConstructionPlanner_PlaceDerivedAsset14D`](../src/gameplay/ai/planning.c#L575), [`AiRuntime_DispatchFactionPlanningPhase`](../src/gameplay/ai/planning.c#L300), [`AiStructureCandidate_AddWeightedId14BOr14CCandidate`](../src/gameplay/ai/planning.c#L1323)
- [`gameplay/ai/units` source](../src/gameplay/ai/units.c) · [graph](callgraphs/gameplay/ai/units.md): 1 caller function(s): [`AiUnitBehavior_UpdateSpecialClass12Entity`](../src/gameplay/ai/units.c#L378)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 3 caller function(s): [`AiConstructionPlanner_PlaceSpecialAssetFromWorkspace`](../src/gameplay/ai/workspaces.c#L1030), [`AiPlanning_RebuildFactionWorkspaces`](../src/gameplay/ai/workspaces.c#L70), [`AiWorkspaceAssetCandidate_AddWeightedEntry`](../src/gameplay/ai/workspaces.c#L5)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/ai/workspaces`](#module-gameplay-ai-workspaces), [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy).

<a id="module-gameplay-ai-planning"></a>
### `gameplay/ai/planning`

[Source](../src/gameplay/ai/planning.c) · [Header](../include/thandor/gameplay/ai/planning.h) · [Call graph](callgraphs/gameplay/ai/planning.md) · [Graphviz](callgraphs/gameplay/ai/planning.dot)

This file owns planning and decision dispatch within game simulation and player/AI behavior. Dominant function families: `AiArmyCandidate_*` (5), `AiConstructionPlanner_*` (5), `AiStrategicClass_*` (4). Representative entry points: `AiFactionRuntime_RebuildPlanningCapacityState`, `AiArmyCandidate_ComputeAverageCompatibleAssetScore`, `AiPlanning_CollectActiveGridMaskClasses`.

**Direct callers:** 3 module(s), 4 cross-module call edge(s).

- [`gameplay/ai/technology` source](../src/gameplay/ai/technology.c) · [graph](callgraphs/gameplay/ai/technology.md): 1 caller function(s): [`AiTechnologyScore_ComputeCategoryCompatibleCandidateValue`](../src/gameplay/ai/technology.c#L278)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 2 caller function(s): [`AiConstructionPlanner_PlaceSpecialAssetFromWorkspace`](../src/gameplay/ai/workspaces.c#L1030), [`AiPlanning_RebuildFactionWorkspaces`](../src/gameplay/ai/workspaces.c#L70)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`gameplay/ai/combat`](#module-gameplay-ai-combat), [`gameplay/ai/placement`](#module-gameplay-ai-placement), [`gameplay/ai/units`](#module-gameplay-ai-units), [`gameplay/ai/workspaces`](#module-gameplay-ai-workspaces), [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/relations`](#module-gameplay-faction-relations), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/pathing/grid`](#module-world-pathing-grid).

<a id="module-gameplay-ai-technology"></a>
### `gameplay/ai/technology`

[Source](../src/gameplay/ai/technology.c) · [Header](../include/thandor/gameplay/ai/technology.h) · [Call graph](callgraphs/gameplay/ai/technology.md) · [Graphviz](callgraphs/gameplay/ai/technology.dot)

This file owns technology-state logic within game simulation and player/AI behavior. Dominant function families: `AiTechnologyScore_*` (5), `AiTechnologyCompatibility_*` (2), `AiTechnologyCandidate_*` (1). Representative entry points: `AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue`, `AiTechnologyCandidate_IsCurrentlyAvailableCf`, `AiTechnologyPlanning_AddCandidateRecord`.

**Direct callers:** 1 module(s), 2 cross-module call edge(s).

- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiPlanning_RebuildFactionWorkspaces`](../src/gameplay/ai/workspaces.c#L70)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`gameplay/ai/planning`](#module-gameplay-ai-planning), [`gameplay/ai/workspaces`](#module-gameplay-ai-workspaces).

<a id="module-gameplay-ai-units"></a>
### `gameplay/ai/units`

[Source](../src/gameplay/ai/units.c) · [Header](../include/thandor/gameplay/ai/units.h) · [Call graph](callgraphs/gameplay/ai/units.md) · [Graphviz](callgraphs/gameplay/ai/units.dot)

This file owns unit behavior and command selection within game simulation and player/AI behavior. Dominant function families: `AiUnitBehavior_*` (7), `AiUnitCommand_*` (2). Representative entry points: `AiUnitBehavior_UpdateWorkspace01Entities`, `AiUnitBehavior_SelectBestAnchorAction`, `AiUnitBehavior_ComputeWorkspace05DistanceScore`.

**Direct callers:** 2 module(s), 2 cross-module call edge(s).

- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 1 caller function(s): [`AiRuntime_DispatchFactionPlanningPhase`](../src/gameplay/ai/planning.c#L300)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 1 caller function(s): [`ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement`](../src/gameplay/army/movement.c#L366)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`gameplay/ai/placement`](#module-gameplay-ai-placement), [`gameplay/ai/workspaces`](#module-gameplay-ai-workspaces), [`gameplay/army/movement`](#module-gameplay-army-movement).

<a id="module-gameplay-ai-workspaces"></a>
### `gameplay/ai/workspaces`

[Source](../src/gameplay/ai/workspaces.c) · [Header](../include/thandor/gameplay/ai/workspaces.h) · [Call graph](callgraphs/gameplay/ai/workspaces.md) · [Graphviz](callgraphs/gameplay/ai/workspaces.dot)

This file owns AI workspace storage, scoring, and candidate management within game simulation and player/AI behavior. Dominant function families: `AiPrimaryWorkspace_*` (7), `AiCandidateWorkspace_*` (6), `AiSecondaryWorkspace_*` (3). Representative entry points: `AiWorkspaceAssetCandidate_AddWeightedEntry`, `AiPlanning_RebuildFactionWorkspaces`, `AiStrategicCandidate_AddBestWorkspace12Entry`.

**Direct callers:** 5 module(s), 47 cross-module call edge(s).

- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 4 caller function(s): [`AiCandidatePlanning_AddSpecialSiteCandidate`](../src/gameplay/ai/placement.c#L54), [`AiCandidatePlanning_ComputeSpecialSiteWeight`](../src/gameplay/ai/placement.c#L559), [`AiSiteCandidate_AddFlaggedCellIfSeparated`](../src/gameplay/ai/placement.c#L190); +1 more
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 13 caller function(s): [`AiArmyCandidate_AddBestScoredVariantA`](../src/gameplay/ai/planning.c#L690), [`AiArmyCandidate_AddBestScoredVariantB`](../src/gameplay/ai/planning.c#L978), [`AiArmyCandidate_AddBestScoredVariantC`](../src/gameplay/ai/planning.c#L1037); +10 more
- [`gameplay/ai/technology` source](../src/gameplay/ai/technology.c) · [graph](callgraphs/gameplay/ai/technology.md): 1 caller function(s): [`AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue`](../src/gameplay/ai/technology.c#L5)
- [`gameplay/ai/units` source](../src/gameplay/ai/units.c) · [graph](callgraphs/gameplay/ai/units.md): 1 caller function(s): [`AiUnitBehavior_UpdateSpecialClass12Entity`](../src/gameplay/ai/units.c#L378)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`assets/package/runtime`](#module-assets-package-runtime), [`gameplay/ai/placement`](#module-gameplay-ai-placement), [`gameplay/ai/planning`](#module-gameplay-ai-planning), [`gameplay/ai/technology`](#module-gameplay-ai-technology), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy).

<a id="module-gameplay-army-audio"></a>
### `gameplay/army/audio`

[Source](../src/gameplay/army/audio.c) · [Header](../include/thandor/gameplay/army/audio.h) · [Call graph](callgraphs/gameplay/army/audio.md) · [Graphviz](callgraphs/gameplay/army/audio.dot)

This file owns audio-related gameplay behavior within game simulation and player/AI behavior. Dominant function families: `ArmyRuntimeAudio_*` (9), `ArmyGraphics_*` (1). Representative entry points: `ArmyGraphics_CopyFrontendPlayerPaletteAndTexture`, `ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA`, `ArmyRuntimeAudio_DispatchPositionedSoundVariant`.

**Direct callers:** 1 module(s), 1 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_InitializePoolAndGraphicsCf`](../src/gameplay/army/runtime.c#L1453)

**Calls into:** [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-gameplay-army-combat"></a>
### `gameplay/army/combat`

[Source](../src/gameplay/army/combat.c) · [Header](../include/thandor/gameplay/army/combat.h) · [Call graph](callgraphs/gameplay/army/combat.md) · [Graphviz](callgraphs/gameplay/army/combat.dot)

This file owns combat targeting and resolution within game simulation and player/AI behavior. Dominant function families: `ArmyRuntime_*` (5), `ArmyRuntimeClass_*` (2), `ArmyRuntimeWeapon_*` (1). Representative entry points: `ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`, `ArmyRuntimeClass_UpdateTransformAndDamageEffect`, `ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage`.

**Direct callers:** 5 module(s), 25 cross-module call edge(s).

- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 1 caller function(s): [`AiCombatTarget_EvaluateCandidateScore`](../src/gameplay/ai/combat.c#L218)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 8 caller function(s): [`ArmyRuntimeClass_UpdateArticulatedMovement`](../src/gameplay/army/movement.c#L5), [`ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation`](../src/gameplay/army/movement.c#L788), [`ArmyRuntimeClass_UpdateGroundMovementVariantA`](../src/gameplay/army/movement.c#L1724); +5 more
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 10 caller function(s): [`ArmyRuntimeClassUpdateSlot11_DispatchByClassId`](../src/gameplay/army/runtime.c#L1050), [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`](../src/gameplay/army/runtime.c#L5); +7 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`GameEntityRuntime_ApplyImpactDamageAndFactionRelationState`](../src/gameplay/faction/runtime.c#L787)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`assets/shot/catalog`](#module-assets-shot-catalog), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/movement`](#module-gameplay-army-movement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-gameplay-army-movement"></a>
### `gameplay/army/movement`

[Source](../src/gameplay/army/movement.c) · [Header](../include/thandor/gameplay/army/movement.h) · [Call graph](callgraphs/gameplay/army/movement.md) · [Graphviz](callgraphs/gameplay/army/movement.dot)

This file owns movement, route execution, and waypoint handling within game simulation and player/AI behavior. Dominant function families: `ArmyRuntime_*` (13), `ArmyArticulatedRuntime_*` (8), `ArmyRuntimeClass_*` (8). Representative entry points: `ArmyRuntimeClass_UpdateArticulatedMovement`, `ArmyRuntime_ResolveCommandTargetAndRoute`, `ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement`.

**Direct callers:** 8 module(s), 21 cross-module call edge(s).

- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 1 caller function(s): [`AiUnitGroup_AssignCollectedEntitiesToBestTarget`](../src/gameplay/ai/combat.c#L47)
- [`gameplay/ai/units` source](../src/gameplay/ai/units.c) · [graph](callgraphs/gameplay/ai/units.md): 3 caller function(s): [`AiUnitBehavior_UpdateSpecialClass12Entity`](../src/gameplay/ai/units.c#L378), [`AiUnitCommand_AssignFactionAnchorPoint`](../src/gameplay/ai/units.c#L328), [`AiUnitCommand_AssignWorkspacePoint`](../src/gameplay/ai/units.c#L309)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`](../src/gameplay/army/combat.c#L5)
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 1 caller function(s): [`ArmyPlacementContact_InitializeArticulatedSuspension`](../src/gameplay/army/placement.c#L479)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 3 caller function(s): [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntime_ApplyTargetPositionCommand`](../src/gameplay/army/runtime.c#L1728), [`ArmyRuntime_ResolveCommandTarget`](../src/gameplay/army/runtime.c#L1685)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 4 caller function(s): [`SelectionPointerArray_ApplyPositionCommand`](../src/gameplay/selection/runtime.c#L1844), [`SelectionPointerArray_ApplyPositionCommandVariantB`](../src/gameplay/selection/runtime.c#L1644), [`SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries`](../src/gameplay/selection/runtime.c#L2478); +1 more
- [`world/pathing/grid` source](../src/world/pathing/grid.c) · [graph](callgraphs/world/pathing/grid.md): 1 caller function(s): [`EntityPathing_UpdateRouteSegment`](../src/world/pathing/grid.c#L1112)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntime_ApplyArmyHitRelationAndNotifications`](../src/world/shots/runtime.c#L5)

**Calls into:** [`assets/shot/catalog`](#module-assets-shot-catalog), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/ai/units`](#module-gameplay-ai-units), [`gameplay/army/combat`](#module-gameplay-army-combat), [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/pathing/grid`](#module-world-pathing-grid), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-gameplay-army-placement"></a>
### `gameplay/army/placement`

[Source](../src/gameplay/army/placement.c) · [Header](../include/thandor/gameplay/army/placement.h) · [Call graph](callgraphs/gameplay/army/placement.md) · [Graphviz](callgraphs/gameplay/army/placement.dot)

This file owns placement, site selection, and spatial validation within game simulation and player/AI behavior. Dominant function families: `ArmyPlacement_*` (8), `ArmyPlacementContact_*` (5), `ArmyPlacementCollision_*` (4). Representative entry points: `ArmyPlacementCandidate_TestOffsetClearanceCf`, `ArmyPlacement_TestModelTerrainAndRuntimeClearance`, `ArmyPlacement_ValidateAssetAtPointAndCellCornersCf`.

**Direct callers:** 7 module(s), 24 cross-module call edge(s).

- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 6 caller function(s): [`AiPlacement_FindNearestValidWorkspace09Anchor`](../src/gameplay/ai/placement.c#L636), [`AiPlacement_QueryReachableSiteBucketCount`](../src/gameplay/ai/placement.c#L459), [`AiPlacement_ReserveAdditionalSpecialSite`](../src/gameplay/ai/placement.c#L5); +3 more
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 2 caller function(s): [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628), [`AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor`](../src/gameplay/ai/planning.c#L863)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 7 caller function(s): [`ArmyArticulatedRuntime_UpdateLeftTerrainContact`](../src/gameplay/army/movement.c#L1962), [`ArmyArticulatedRuntime_UpdateRightTerrainContact`](../src/gameplay/army/movement.c#L2101), [`ArmyRuntimeClass_UpdateArticulatedMovement`](../src/gameplay/army/movement.c#L5); +4 more
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState`](../src/gameplay/army/runtime.c#L2457)
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 1 caller function(s): [`InGameWorldInput_ResolveContextActionAndCursorCf`](../src/gameplay/input/world.c#L101)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 1 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand_ExecuteLocalPlacementFromSelection`](../src/ui/ingame/commands.c#L527)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/movement`](#module-gameplay-army-movement), [`graphics/render/primitives`](#module-graphics-render-primitives), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/pathing/grid`](#module-world-pathing-grid), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/height`](#module-world-terrain-height).

<a id="module-gameplay-army-runtime"></a>
### `gameplay/army/runtime`

[Source](../src/gameplay/army/runtime.c) · [Header](../include/thandor/gameplay/army/runtime.h) · [Call graph](callgraphs/gameplay/army/runtime.md) · [Graphviz](callgraphs/gameplay/army/runtime.dot)

This file owns runtime state and lifecycle operations within game simulation and player/AI behavior. Dominant function families: `ArmyRuntime_*` (31), `ArmyRuntimeClass_*` (9), `ArmyRuntimeMaintenance_*` (3). Representative entry points: `ArmyRuntimeClassUpdateSlot21_DispatchByClassId`, `ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`, `ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`.

**Direct callers:** 19 module(s), 74 cross-module call edge(s).

- [`assets/army/catalog` source](../src/assets/army/catalog.c) · [graph](callgraphs/assets/army/catalog.md): 1 caller function(s): [`ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf`](../src/assets/army/catalog.c#L694)
- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 1 caller function(s): [`AiCombatDecision_UpdateTargetAssignment`](../src/gameplay/ai/combat.c#L5)
- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 1 caller function(s): [`AiPlacement_ReserveSeparatedSpecialSiteChain`](../src/gameplay/ai/placement.c#L703)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 3 caller function(s): [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628), [`AiConstructionPlanner_PlaceDerivedAsset14D`](../src/gameplay/ai/planning.c#L575), [`AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor`](../src/gameplay/ai/planning.c#L863)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiConstructionPlanner_PlaceSpecialAssetFromWorkspace`](../src/gameplay/ai/workspaces.c#L1030)
- [`gameplay/army/audio` source](../src/gameplay/army/audio.c) · [graph](callgraphs/gameplay/army/audio.md): 2 caller function(s): [`ArmyRuntimeAudio_DispatchPositionedSoundVariant`](../src/gameplay/army/audio.c#L125), [`ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled`](../src/gameplay/army/audio.c#L428)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 2 caller function(s): [`ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage`](../src/gameplay/army/combat.c#L271), [`ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`](../src/gameplay/army/combat.c#L5)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 10 caller function(s): [`ArmyArticulatedRuntime_UpdateLeftTerrainContact`](../src/gameplay/army/movement.c#L1962), [`ArmyArticulatedRuntime_UpdateRightTerrainContact`](../src/gameplay/army/movement.c#L2101), [`ArmyRuntimeClass_UpdateArticulatedMovement`](../src/gameplay/army/movement.c#L5); +7 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 2 caller function(s): [`OldUnitRuntime_MergeMasksAndReplayRecords`](../src/gameplay/faction/runtime.c#L1293), [`PlayerRuntime_ResolveAndStoreState8094`](../src/gameplay/faction/runtime.c#L1213)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 1 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 7 caller function(s): [`SelectionInfo_CollectAttachmentEffectVariantMask`](../src/gameplay/selection/runtime.c#L1531), [`SelectionInfo_TestAllStateField100NonpositiveCf`](../src/gameplay/selection/runtime.c#L1407), [`SelectionInfo_TestAnyStateField100NonnegativeCf`](../src/gameplay/selection/runtime.c#L1443); +4 more
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 1 caller function(s): [`FrontendPlayerSelection_ApplyEntryOrAll`](../src/ui/frontend/player.c#L1228)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 2 caller function(s): [`InGameCommand150_HandlePlayerDepartureAndOwnership`](../src/ui/ingame/commands.c#L1467), [`InGameCommand_ExecuteLocalPlacementFromSelection`](../src/ui/ingame/commands.c#L527)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 2 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682), [`InGameUiCommand_ResolveCursorCodeByMode`](../src/ui/ingame/runtime.c#L3824)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 1 caller function(s): [`ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive`](../src/world/model/hierarchy.c#L1572)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 1 caller function(s): [`ModelRuntimePool_DestroyHierarchyAndDetach`](../src/world/model/runtime.c#L644)
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 1 caller function(s): [`WorldRuntimeNode_ReleaseShutdownBindingsCallback`](../src/world/runtime/core.c#L1376)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/effect/catalog`](#module-assets-effect-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/shot/catalog`](#module-assets-shot-catalog), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/math/fixed`](#module-core-math-fixed), [`core/text/path`](#module-core-text-path), [`gameplay/ai/combat`](#module-gameplay-ai-combat), [`gameplay/army/audio`](#module-gameplay-army-audio), [`gameplay/army/combat`](#module-gameplay-army-combat), [`gameplay/army/movement`](#module-gameplay-army-movement), [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`movie/runtime/playback`](#module-movie-runtime-playback), [`ui/controls/misc`](#module-ui-controls-misc), [`ui/ingame/commands`](#module-ui-ingame-commands), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/technology`](#module-ui-ingame-technology), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/shots/runtime`](#module-world-shots-runtime), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/occupancy`](#module-world-terrain-occupancy), [`world/terrain/projection`](#module-world-terrain-projection).

<a id="module-gameplay-faction-relations"></a>
### `gameplay/faction/relations`

[Source](../src/gameplay/faction/relations.c) · [Header](../include/thandor/gameplay/faction/relations.h) · [Call graph](callgraphs/gameplay/faction/relations.md) · [Graphviz](callgraphs/gameplay/faction/relations.dot)

This file owns faction relationship state within game simulation and player/AI behavior. Dominant function families: `GameFactionRelations_*` (8), `PlayerPairList_*` (4). Representative entry points: `GameFactionRelations_UpdateAllPairsForFaction`, `PlayerPairList_InsertRange`, `PlayerPairList_RemoveRange`.

**Direct callers:** 2 module(s), 3 cross-module call edge(s).

- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 1 caller function(s): [`AiRuntime_DispatchFactionPlanningPhase`](../src/gameplay/ai/planning.c#L300)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiCommand_UpdateInteractionByMode`](../src/ui/ingame/runtime.c#L4348)

**Calls into:** [`gameplay/faction/runtime`](#module-gameplay-faction-runtime).

<a id="module-gameplay-faction-runtime"></a>
### `gameplay/faction/runtime`

[Source](../src/gameplay/faction/runtime.c) · [Header](../include/thandor/gameplay/faction/runtime.h) · [Call graph](callgraphs/gameplay/faction/runtime.md) · [Graphviz](callgraphs/gameplay/faction/runtime.dot)

This file owns runtime state and lifecycle operations within game simulation and player/AI behavior. Dominant function families: `GameFactionRuntime_*` (17), `PlayerRuntime_*` (4), `GameEntityRuntime_*` (3). Representative entry points: `GameFactionRuntime_AdvancePairwiseRelationState`, `GameFactionRuntime_ResetPairwiseRelationState`, `OldUnitRuntime_RebuildScenarioReplayTables`.

**Direct callers:** 17 module(s), 49 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 1 caller function(s): [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995)
- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 1 caller function(s): [`AiCombatTarget_EvaluateCandidateScore`](../src/gameplay/ai/combat.c#L218)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 1 caller function(s): [`AiPurchaseCandidate_ApplyToFaction`](../src/gameplay/ai/planning.c#L1187)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiPlanning_RebuildFactionWorkspaces`](../src/gameplay/ai/workspaces.c#L70)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyRuntime_ApplyImpactDamageToRuntimeAndParent`](../src/gameplay/army/combat.c#L412)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 1 caller function(s): [`ArmyRuntimeClass_UpdateArticulatedMovement`](../src/gameplay/army/movement.c#L5)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 2 caller function(s): [`ArmyRuntime_DestroyInstanceAndRefreshUi`](../src/gameplay/army/runtime.c#L3293), [`ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint`](../src/gameplay/army/runtime.c#L3387)
- [`gameplay/faction/relations` source](../src/gameplay/faction/relations.c) · [graph](callgraphs/gameplay/faction/relations.md): 6 caller function(s): [`GameFactionRelations_BuildEligibleFactionMask`](../src/gameplay/faction/relations.c#L167), [`GameFactionRelations_IsResetEligibleStateCf`](../src/gameplay/faction/relations.c#L337), [`GameFactionRelations_MaybeAdvancePairStateCommon`](../src/gameplay/faction/relations.c#L398); +3 more
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 3 caller function(s): [`InGameWorldInput_BeginPointerCaptureCf`](../src/gameplay/input/world.c#L320), [`InGameWorldInput_CommitPointerActionCf`](../src/gameplay/input/world.c#L611), [`InGameWorldInput_ResolveContextActionAndCursorCf`](../src/gameplay/input/world.c#L101)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 2 caller function(s): [`SelectionPanel_RenderArmyRuntimeMetrics`](../src/gameplay/selection/runtime.c#L5), [`SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries`](../src/gameplay/selection/runtime.c#L2450)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_RunSessionUntilExit`](../src/gameplay/session/runtime.c#L5), [`InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState`](../src/gameplay/session/runtime.c#L1826); +1 more
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_PlaySelectedEndMovie`](../src/ui/frontend/runtime.c#L2552)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 3 caller function(s): [`InGameCommandCatalog_SubmitGroup42Entry`](../src/ui/ingame/commands.c#L861), [`InGameCommandCatalog_SubmitGroup48Entry`](../src/ui/ingame/commands.c#L799), [`InGameCommandSprite_DispatchVariantAControl24`](../src/ui/ingame/commands.c#L923)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 5 caller function(s): [`InGameOtherPlayerCommand_DispatchSelectedTarget`](../src/ui/ingame/runtime.c#L3530), [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682), [`InGameUiCommand_BeginInteractionByMode`](../src/ui/ingame/runtime.c#L3942); +2 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 2 caller function(s): [`UiCatalogGroup42_RebuildGrid`](../src/ui/ingame/technology.c#L259), [`UiCatalogGroup48_RebuildGrid`](../src/ui/ingame/technology.c#L101)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntime_ApplyArmyHitRelationAndNotifications`](../src/world/shots/runtime.c#L5)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/combat`](#module-gameplay-army-combat), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`ui/ingame/commands`](#module-ui-ingame-commands), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/technology`](#module-ui-ingame-technology), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-gameplay-input-world"></a>
### `gameplay/input/world`

[Source](../src/gameplay/input/world.c) · [Header](../include/thandor/gameplay/input/world.h) · [Call graph](callgraphs/gameplay/input/world.md) · [Graphviz](callgraphs/gameplay/input/world.dot)

This file owns world-facing input handling within game simulation and player/AI behavior. Dominant function families: `InGameWorldInput_*` (4), `InGameCameraCommand_*` (2), `InGameTargetingContext_*` (2). Representative entry points: `InGameTargetingContext_AdvanceOrResolveTarget`, `InGameWorldInput_ResolveContextActionAndCursorCf`, `InGameWorldInput_BeginPointerCaptureCf`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/ingame/commands`](#module-ui-ingame-commands), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-gameplay-selection-overlay"></a>
### `gameplay/selection/overlay`

[Source](../src/gameplay/selection/overlay.c) · [Header](../include/thandor/gameplay/selection/overlay.h) · [Call graph](callgraphs/gameplay/selection/overlay.md) · [Graphviz](callgraphs/gameplay/selection/overlay.dot)

This file owns selection overlays and visual selection state within game simulation and player/AI behavior. Dominant function families: `SelectionOverlay_*` (9), `SelectionMarkerCoordinates_*` (5), `InGameWorldOverlay_*` (2). Representative entry points: `InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`, `SelectionOverlay_RenderSelectedArmyMetrics`, `SelectionOverlay_RenderArmyMetricsForEntity`.

**Direct callers:** 1 module(s), 9 cross-module call edge(s).

- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806)

**Calls into:** [`assets/effect/catalog`](#module-assets-effect-catalog), [`core/math/fixed`](#module-core-math-fixed), [`core/math/interpolation`](#module-core-math-interpolation), [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/model`](#module-graphics-render-model), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-gameplay-selection-runtime"></a>
### `gameplay/selection/runtime`

[Source](../src/gameplay/selection/runtime.c) · [Header](../include/thandor/gameplay/selection/runtime.h) · [Call graph](callgraphs/gameplay/selection/runtime.md) · [Graphviz](callgraphs/gameplay/selection/runtime.dot)

This file owns runtime state and lifecycle operations within game simulation and player/AI behavior. Dominant function families: `SelectionPointerArray_*` (12), `SelectionInfo_*` (11), `SelectionPanel_*` (8). Representative entry points: `SelectionPanel_RenderArmyRuntimeMetrics`, `InGameSelection_RebuildOwnedClass16Selection`, `InGamePlayerSelection_ReplaceWithArmyRuntimeIndex`.

**Direct callers:** 12 module(s), 64 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_DestroyInstanceAndRefreshUi`](../src/gameplay/army/runtime.c#L3293)
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 4 caller function(s): [`InGameWorldInput_BeginPointerCaptureCf`](../src/gameplay/input/world.c#L320), [`InGameWorldInput_CommitPointerActionCf`](../src/gameplay/input/world.c#L611), [`InGameWorldInput_ResolveContextActionAndCursorCf`](../src/gameplay/input/world.c#L101); +1 more
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 8 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5), [`SelectionMarkerCoordinates_ApplyType3`](../src/gameplay/selection/overlay.c#L871), [`SelectionMarkerCoordinates_ApplyType4`](../src/gameplay/selection/overlay.c#L894); +5 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_ShutdownAndReleaseResources`](../src/gameplay/session/runtime.c#L1444)
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 1 caller function(s): [`UiArmyMetricsPanel_DrawTextureMetricsAndChildren`](../src/ui/controls/text.c#L3616)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 6 caller function(s): [`FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection`](../src/ui/frontend/player.c#L1588), [`FrontendPlayerSelection_ApplyEntryOrAll`](../src/ui/frontend/player.c#L1228), [`FrontendPlayerSelection_ClearAndRefreshLocalPanels`](../src/ui/frontend/player.c#L961); +3 more
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 2 caller function(s): [`InGameCommandAction_ClearSelectedArmyTokenAndClosePage`](../src/ui/ingame/commands.c#L720), [`InGameCommandSprite_DispatchFixedControl8`](../src/ui/ingame/commands.c#L1013)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 5 caller function(s): [`InGameSelectionPage_ToggleAndRefreshPage2`](../src/ui/ingame/runtime.c#L3592), [`InGameUiCommand_BeginInteractionByMode`](../src/ui/ingame/runtime.c#L3942), [`InGameUiCommand_ResetInteractionByMode`](../src/ui/ingame/runtime.c#L4883); +2 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 4 caller function(s): [`InGameTechnologyPanel_Rebuild`](../src/ui/ingame/technology.c#L457), [`InGameTechnologyPanel_ResetAndSelectCurrentArea`](../src/ui/ingame/technology.c#L36), [`InGameTechnologyResearch_StartSelected`](../src/ui/ingame/technology.c#L410); +1 more
- [`world/terrain/grid` source](../src/world/terrain/grid.c) · [graph](callgraphs/world/terrain/grid.md): 4 caller function(s): [`FieldGrid_ApplyLocalCellUpdate`](../src/world/terrain/grid.c#L849), [`FieldGrid_ApplyNegativeCellDeltas`](../src/world/terrain/grid.c#L352), [`FieldGrid_ApplyPositiveCellDeltas`](../src/world/terrain/grid.c#L137); +1 more
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 1 caller function(s): [`TerrainCompositeTexture_RebuildPlane0`](../src/world/terrain/visuals.c#L1112)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/richtext`](#module-assets-text-richtext), [`gameplay/army/movement`](#module-gameplay-army-movement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/technology`](#module-ui-ingame-technology), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/pathing/grid`](#module-world-pathing-grid).

<a id="module-gameplay-session-level"></a>
### `gameplay/session/level`

[Source](../src/gameplay/session/level.c) · [Header](../include/thandor/gameplay/session/level.h) · [Call graph](callgraphs/gameplay/session/level.md) · [Graphviz](callgraphs/gameplay/session/level.dot)

This file owns level loading and level-owned state within game simulation and player/AI behavior. Dominant function families: `InGameLevelRuntime_*` (4), `LevelAsset_*` (1). Representative entry points: `LevelAsset_PrepareEndingMoviePathCf`, `InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`, `InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`.

**Direct callers:** 2 module(s), 6 cross-module call edge(s).

- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_ShutdownAndReleaseResources`](../src/gameplay/session/runtime.c#L1444)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiCommand_SaveFieldAndLevelAssetImages`](../src/ui/ingame/runtime.c#L5138)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/effect/catalog`](#module-assets-effect-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/shot/catalog`](#module-assets-shot-catalog), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/text/path`](#module-core-text-path), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/session/runtime`](#module-gameplay-session-runtime), [`movie/runtime/playback`](#module-movie-runtime-playback), [`platform/filesystem/win32`](#module-platform-filesystem-win32), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/shots/runtime`](#module-world-shots-runtime), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/visuals`](#module-world-terrain-visuals).

<a id="module-gameplay-session-runtime"></a>
### `gameplay/session/runtime`

[Source](../src/gameplay/session/runtime.c) · [Header](../include/thandor/gameplay/session/runtime.h) · [Call graph](callgraphs/gameplay/session/runtime.md) · [Graphviz](callgraphs/gameplay/session/runtime.dot)

This file owns runtime state and lifecycle operations within game simulation and player/AI behavior. Dominant function families: `InGameRuntime_*` (12), `EndGameResultsUiRuntime_*` (2), `InGameConditionRuntime_*` (2). Representative entry points: `InGameRuntime_RunSessionUntilExit`, `EndGameResultsUiRuntime_UpdateAndHandleInputCf`, `InGameConditionRuntime_RebaseLoadedRecords`.

**Direct callers:** 4 module(s), 4 cross-module call edge(s).

- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 1 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`MoviePlayback_AdvanceScheduledFrameAndTick`](../src/movie/runtime/playback.c#L92)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/sprite/catalog`](#module-assets-sprite-catalog), [`assets/text/resources`](#module-assets-text-resources), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/math/interpolation`](#module-core-math-interpolation), [`core/math/random`](#module-core-math-random), [`core/settings/persistent`](#module-core-settings-persistent), [`core/text/path`](#module-core-text-path), [`gameplay/ai/planning`](#module-gameplay-ai-planning), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`gameplay/session/level`](#module-gameplay-session-level), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`graphics/render/shading`](#module-graphics-render-shading), [`movie/runtime/playback`](#module-movie-runtime-playback), [`network/backend/runtime`](#module-network-backend-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`ui/frontend/session`](#module-ui-frontend-session), [`ui/ingame/commands`](#module-ui-ingame-commands), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/technology`](#module-ui-ingame-technology), [`ui/support/runtime`](#module-ui-support-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/motion/runtime`](#module-world-motion-runtime), [`world/pathing/grid`](#module-world-pathing-grid), [`world/pathing/influence`](#module-world-pathing-influence), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/editing`](#module-world-terrain-editing), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/visuals`](#module-world-terrain-visuals).

<a id="module-gameplay-session-savegame"></a>
### `gameplay/session/savegame`

[Source](../src/gameplay/session/savegame.c) · [Header](../include/thandor/gameplay/session/savegame.h) · [Call graph](callgraphs/gameplay/session/savegame.md) · [Graphviz](callgraphs/gameplay/session/savegame.dot)

This file owns save/load state serialization within game simulation and player/AI behavior. Dominant function families: `InGameSaveGame_*` (1), `InGameSaveGameAction_*` (1), `InGameSaveGameList_*` (1). Representative entry points: `InGameSaveGameList_SelectAndRefreshDetail`, `InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog`, `InGameSaveGamePage_RebuildCatalog`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/path`](#module-core-text-path), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/settings`](#module-ui-ingame-settings).

<a id="module-gameplay-technology-runtime"></a>
### `gameplay/technology/runtime`

[Source](../src/gameplay/technology/runtime.c) · [Header](../include/thandor/gameplay/technology/runtime.h) · [Call graph](callgraphs/gameplay/technology/runtime.md) · [Graphviz](callgraphs/gameplay/technology/runtime.dot)

This file owns runtime state and lifecycle operations within game simulation and player/AI behavior. Dominant function families: `Technology_*` (4), `TechnologyRuntime_*` (1). Representative entry points: `Technology_UnlockForFaction`, `Technology_IsUnlockedForFactionCf`, `Technology_IsAvailableForFactionCf`.

**Direct callers:** 8 module(s), 12 cross-module call edge(s).

- [`assets/model/definitions` source](../src/assets/model/definitions.c) · [graph](callgraphs/assets/model/definitions.md): 1 caller function(s): [`ModelDefinition_UnlockLinkedTechnologyForFactionCf`](../src/assets/model/definitions.c#L757)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 4 caller function(s): [`AiArmyCandidate_AddBestScoredVariantA`](../src/gameplay/ai/planning.c#L690), [`AiPurchaseCandidate_ApplyToFaction`](../src/gameplay/ai/planning.c#L1187), [`AiPurchaseCandidate_HasEligibleProducerCf`](../src/gameplay/ai/planning.c#L1091); +1 more
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive`](../src/gameplay/army/runtime.c#L2995)
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10`](../src/gameplay/faction/runtime.c#L367)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 1 caller function(s): [`FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology`](../src/ui/frontend/player.c#L1108)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357)
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 1 caller function(s): [`InGameTechnologyPanel_Rebuild`](../src/ui/ingame/technology.c#L457)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/technology`](#module-ui-ingame-technology), [`world/model/hierarchy`](#module-world-model-hierarchy).

## Graphics

<a id="module-graphics-backend-direct3d"></a>
### `graphics/backend/direct3d`

[Source](../src/graphics/backend/direct3d.c) · [Header](../include/thandor/graphics/backend/direct3d.h) · [Call graph](callgraphs/graphics/backend/direct3d.md) · [Graphviz](callgraphs/graphics/backend/direct3d.dot)

This file owns Direct3D backend integration within rendering, graphics backends, and GPU/software resources. Dominant function families: `Direct3D_*` (10), `Direct3DRenderer_*` (3), `GraphicsDirect3D_*` (1). Representative entry points: `Direct3D_EnumDeviceCallback`, `GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback`, `Direct3DRenderer_SetAntialiasMode`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`core/text/string`](#module-core-text-string), [`graphics/resources/texture`](#module-graphics-resources-texture).

<a id="module-graphics-backend-directdraw"></a>
### `graphics/backend/directdraw`

[Source](../src/graphics/backend/directdraw.c) · [Header](../include/thandor/graphics/backend/directdraw.h) · [Call graph](callgraphs/graphics/backend/directdraw.md) · [Graphviz](callgraphs/graphics/backend/directdraw.dot)

This file owns DirectDraw display and surface integration within rendering, graphics backends, and GPU/software resources. Dominant function families: `DirectDraw_*` (2), `DisplayModeTable_*` (1), `GraphicsDirectDraw_*` (1). Representative entry points: `GraphicsDisplayMode_IsEnumeratedCf`, `DisplayModeTable_ContainsExactModeCf`, `DirectDraw_EnumAdapterCallback`.

**Direct callers:** 2 module(s), 2 cross-module call edge(s).

- [`ui/controls/misc` source](../src/ui/controls/misc.c) · [graph](callgraphs/ui/controls/misc.md): 1 caller function(s): [`UiDisplayModeSelection_RefreshEnumeratedOptions`](../src/ui/controls/misc.c#L1114)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendDisplaySettingsPage_UpdateModeActionAvailability`](../src/ui/frontend/settings.c#L1669)

**Calls into:** [`core/memory/allocator`](#module-core-memory-allocator), [`core/text/string`](#module-core-text-string), [`graphics/backend/glide`](#module-graphics-backend-glide), [`graphics/resources/texture`](#module-graphics-resources-texture).

<a id="module-graphics-backend-glide"></a>
### `graphics/backend/glide`

[Source](../src/graphics/backend/glide.c) · [Header](../include/thandor/graphics/backend/glide.h) · [Call graph](callgraphs/graphics/backend/glide.md) · [Graphviz](callgraphs/graphics/backend/glide.dot)

This file owns Glide backend integration within rendering, graphics backends, and GPU/software resources. Dominant function families: `Glide3_*` (30), `GlideBackend_*` (3), `GraphicsGlide3_*` (1). Representative entry points: `Glide3_TextureSet_CreateBackend`, `Glide3_TextureResource_ReinitializeAll`, `GlideBackend_ShutdownWrapper`.

**Direct callers:** 4 module(s), 20 cross-module call edge(s).

- [`graphics/backend/directdraw` source](../src/graphics/backend/directdraw.c) · [graph](callgraphs/graphics/backend/directdraw.md): 1 caller function(s): [`GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf`](../src/graphics/backend/directdraw.c#L180)
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 9 caller function(s): [`GraphicsBackend_RefreshActiveAdapterIfReady`](../src/graphics/core/runtime.c#L658), [`GraphicsCursor_ComposeBeforePresent`](../src/graphics/core/runtime.c#L930), [`GraphicsCursor_RestoreAfterPresent`](../src/graphics/core/runtime.c#L978); +6 more
- [`graphics/resources/framebuffer` source](../src/graphics/resources/framebuffer.c) · [graph](callgraphs/graphics/resources/framebuffer.md): 4 caller function(s): [`GraphicsFramebuffer_BeginAccess`](../src/graphics/resources/framebuffer.c#L367), [`GraphicsFramebuffer_CaptureRegion16Bit`](../src/graphics/resources/framebuffer.c#L133), [`GraphicsFramebuffer_EndAccess`](../src/graphics/resources/framebuffer.c#L414); +1 more
- [`graphics/resources/texture` source](../src/graphics/resources/texture.c) · [graph](callgraphs/graphics/resources/texture.md): 5 caller function(s): [`GraphicsTextureSet_Create`](../src/graphics/resources/texture.c#L5), [`GraphicsTextureSet_Destroy`](../src/graphics/resources/texture.c#L123), [`GraphicsTextureSet_RefreshAlpha`](../src/graphics/resources/texture.c#L3325); +2 more

**Calls into:** [`core/text/string`](#module-core-text-string), [`graphics/backend/software`](#module-graphics-backend-software), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/resources/texture`](#module-graphics-resources-texture), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime).

<a id="module-graphics-backend-software"></a>
### `graphics/backend/software`

[Source](../src/graphics/backend/software.c) · [Header](../include/thandor/graphics/backend/software.h) · [Call graph](callgraphs/graphics/backend/software.md) · [Graphviz](callgraphs/graphics/backend/software.dot)

This file owns software rasterization within rendering, graphics backends, and GPU/software resources. Dominant function families: `SoftwareRaster16_*` (20), `SoftwareRasterAux_*` (20), `SoftwareRasterNon16_*` (20). Representative entry points: `SoftwareMaskBuffer_AdvancePatternByPercentTick`, `SoftwareRenderer_ClearViewport`, `SoftwareRenderer_DrawQueue16Bit`.

**Direct callers:** 7 module(s), 16 cross-module call edge(s).

- [`graphics/backend/glide` source](../src/graphics/backend/glide.c) · [graph](callgraphs/graphics/backend/glide.md): 9 caller function(s): [`Glide3_Framebuffer_FillRectArgb`](../src/graphics/backend/glide.c#L4020), [`Glide3_TextureSource_BlitHalfRgbSaturatedAdd`](../src/graphics/backend/glide.c#L3426), [`Glide3_TextureSource_BlitHalfSourceRgb`](../src/graphics/backend/glide.c#L1952); +6 more
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 2 caller function(s): [`Graphics_DrawPrimitiveQueue`](../src/graphics/core/runtime.c#L885), [`Graphics_SetViewportAndClearDepth`](../src/graphics/core/runtime.c#L751)
- [`graphics/render/projection` source](../src/graphics/render/projection.c) · [graph](callgraphs/graphics/render/projection.md): 1 caller function(s): [`GraphicsOffscreen_RenderModelListToTextureSourceCf`](../src/graphics/render/projection.c#L5)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`GameRuntime_InitializeSpatialAudioAndRenderingCf`](../src/platform/bootstrap/runtime.c#L561)
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 1 caller function(s): [`UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren`](../src/ui/controls/text.c#L3697)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/support/runtime` source](../src/ui/support/runtime.c) · [graph](callgraphs/ui/support/runtime.md): 1 caller function(s): [`CreditsScreen_Open`](../src/ui/support/runtime.c#L131)

**Calls into:** [`graphics/render/primitives`](#module-graphics-render-primitives).

<a id="module-graphics-core-runtime"></a>
### `graphics/core/runtime`

[Source](../src/graphics/core/runtime.c) · [Header](../include/thandor/graphics/core/runtime.h) · [Call graph](callgraphs/graphics/core/runtime.md) · [Graphviz](callgraphs/graphics/core/runtime.dot)

This file owns runtime state and lifecycle operations within rendering, graphics backends, and GPU/software resources. Dominant function families: `Graphics_*` (14), `GraphicsCursor_*` (7), `GraphicsObject_*` (5). Representative entry points: `GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer`, `GraphicsCursor_SetFrameIndex`, `GraphicsCursor_ConsumeNextInputEvent`.

**Direct callers:** 11 module(s), 28 cross-module call edge(s).

- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 2 caller function(s): [`SelectionOverlay_DrawMarkerACForWorldSurfacePoint`](../src/gameplay/selection/overlay.c#L522), [`SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints`](../src/gameplay/selection/overlay.c#L454)
- [`graphics/render/model` source](../src/graphics/render/model.c) · [graph](callgraphs/graphics/render/model.md): 3 caller function(s): [`ModelProjectedBounds_ExpandWithCurrentScratchPoint`](../src/graphics/render/model.c#L1109), [`ModelRender_PrepareProjectedVertex`](../src/graphics/render/model.c#L191), [`ModelRender_PrepareProjectedVertexAlternatePath`](../src/graphics/render/model.c#L476)
- [`graphics/render/projection` source](../src/graphics/render/projection.c) · [graph](callgraphs/graphics/render/projection.md): 1 caller function(s): [`GraphicsOffscreen_RenderModelListToTextureSourceCf`](../src/graphics/render/projection.c#L5)
- [`graphics/render/shading` source](../src/graphics/render/shading.c) · [graph](callgraphs/graphics/render/shading.md): 1 caller function(s): [`GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy`](../src/graphics/render/shading.c#L5)
- [`graphics/resources/framebuffer` source](../src/graphics/resources/framebuffer.c) · [graph](callgraphs/graphics/resources/framebuffer.md): 1 caller function(s): [`GraphicsFramebuffer_Present`](../src/graphics/resources/framebuffer.c#L26)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 1 caller function(s): [`ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf`](../src/world/model/hierarchy.c#L344)
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 1 caller function(s): [`WorldRuntimeNode_IsPositionInsideBoundsCf`](../src/world/runtime/core.c#L736)
- [`world/terrain/projection` source](../src/world/terrain/projection.c) · [graph](callgraphs/world/terrain/projection.md): 2 caller function(s): [`TerrainProjectedVertex_TransformProjectAndShadeVariantA`](../src/world/terrain/projection.c#L1498), [`TerrainProjectedVertex_TransformProjectAndShadeVariantB`](../src/world/terrain/projection.c#L1679)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`core/memory/allocator`](#module-core-memory-allocator), [`graphics/backend/glide`](#module-graphics-backend-glide), [`graphics/backend/software`](#module-graphics-backend-software), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/resources/texture`](#module-graphics-resources-texture), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime).

<a id="module-graphics-render-model"></a>
### `graphics/render/model`

[Source](../src/graphics/render/model.c) · [Header](../include/thandor/graphics/render/model.h) · [Call graph](callgraphs/graphics/render/model.md) · [Graphviz](callgraphs/graphics/render/model.dot)

This file owns model rendering within rendering, graphics backends, and GPU/software resources. Dominant function families: `ModelRender_*` (13), `ModelProjectedBounds_*` (3). Representative entry points: `ModelRender_DrawMeshGroupsWithTemporaryTransform`, `ModelRender_DrawMeshGroupsAlternatePath`, `ModelProjectedBounds_AccumulateHierarchyRecursive`.

**Direct callers:** 2 module(s), 4 cross-module call edge(s).

- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 2 caller function(s): [`SelectionOverlay_RenderArmyMetricsForEntity`](../src/gameplay/selection/overlay.c#L345), [`SelectionOverlay_RenderSelectedArmyMetrics`](../src/gameplay/selection/overlay.c#L297)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 2 caller function(s): [`ModelRuntime_CullAndRenderHierarchyRecursive`](../src/world/model/runtime.c#L66), [`ModelRuntime_RenderHierarchyRecursiveAlternatePath`](../src/world/model/runtime.c#L204)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`world/model/hierarchy`](#module-world-model-hierarchy).

<a id="module-graphics-render-primitives"></a>
### `graphics/render/primitives`

[Source](../src/graphics/render/primitives.c) · [Header](../include/thandor/graphics/render/primitives.h) · [Call graph](callgraphs/graphics/render/primitives.md) · [Graphviz](callgraphs/graphics/render/primitives.dot)

This file owns primitive rasterization and draw dispatch within rendering, graphics backends, and GPU/software resources. Dominant function families: `GraphicsPrimitiveQueue_*` (13), `DepthBinMasks_*` (1), `DepthInterval_*` (1). Representative entry points: `GraphicsPrimitiveQueue_RadixSortForRendering`, `GraphicsPrimitiveQueue_AllocateGlobalPool`, `GraphicsPrimitiveQueue_ResetGlobal`.

**Direct callers:** 13 module(s), 37 cross-module call edge(s).

- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 2 caller function(s): [`AiCombatTarget_EvaluateCandidateScore`](../src/gameplay/ai/combat.c#L218), [`AiCombatTarget_SelectBestCandidate`](../src/gameplay/ai/combat.c#L133)
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 4 caller function(s): [`ArmyCollision_FindBlockingRuntimeForCurrentUnitCf`](../src/gameplay/army/placement.c#L705), [`ArmyCollision_TestPointAgainstRuntimeListCf`](../src/gameplay/army/placement.c#L653), [`ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf`](../src/gameplay/army/placement.c#L868); +1 more
- [`graphics/backend/glide` source](../src/graphics/backend/glide.c) · [graph](callgraphs/graphics/backend/glide.md): 1 caller function(s): [`Glide3_DrawPrimitiveQueue`](../src/graphics/backend/glide.c#L328)
- [`graphics/backend/software` source](../src/graphics/backend/software.c) · [graph](callgraphs/graphics/backend/software.md): 3 caller function(s): [`SoftwareRenderer_DrawQueue16Bit`](../src/graphics/backend/software.c#L126), [`SoftwareRenderer_DrawQueueAuxiliary`](../src/graphics/backend/software.c#L192), [`SoftwareRenderer_DrawQueueNon16Bit`](../src/graphics/backend/software.c#L159)
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 1 caller function(s): [`Graphics_DrawPrimitiveQueue`](../src/graphics/core/runtime.c#L885)
- [`graphics/render/model` source](../src/graphics/render/model.c) · [graph](callgraphs/graphics/render/model.md): 2 caller function(s): [`ModelRender_SubmitTriangle`](../src/graphics/render/model.c#L285), [`ModelRender_SubmitTriangleAlternatePath`](../src/graphics/render/model.c#L535)
- [`graphics/render/projection` source](../src/graphics/render/projection.c) · [graph](callgraphs/graphics/render/projection.md): 1 caller function(s): [`GraphicsOffscreen_RenderModelListToTextureSourceCf`](../src/graphics/render/projection.c#L5)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`GameRuntime_InitializeSpatialAudioAndRenderingCf`](../src/platform/bootstrap/runtime.c#L561)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 1 caller function(s): [`ModelNodeRuntime_UpdateDepthBinMasks`](../src/world/model/hierarchy.c#L260)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 1 caller function(s): [`ModelRuntime_RaycastCandidateListNearestCf`](../src/world/model/runtime.c#L254)
- [`world/pathing/grid` source](../src/world/pathing/grid.c) · [graph](callgraphs/world/pathing/grid.md): 1 caller function(s): [`EntityPathing_RebuildOverlappingGroupRoutes`](../src/world/pathing/grid.c#L844)
- [`world/terrain/projection` source](../src/world/terrain/projection.c) · [graph](callgraphs/world/terrain/projection.md): 1 caller function(s): [`TerrainProjectedTriangle_ClipInterpolateAndQueueTextured`](../src/world/terrain/projection.c#L1838)

<a id="module-graphics-render-projection"></a>
### `graphics/render/projection`

[Source](../src/graphics/render/projection.c) · [Header](../include/thandor/graphics/render/projection.h) · [Call graph](callgraphs/graphics/render/projection.md) · [Graphviz](callgraphs/graphics/render/projection.dot)

This file owns projection and projected geometry within rendering, graphics backends, and GPU/software resources. Dominant function families: `GraphicsOffscreen_*` (1), `GraphicsProjectedPoint_*` (1). Representative entry points: `GraphicsOffscreen_RenderModelListToTextureSourceCf`, `GraphicsProjectedPoint_IsInsideTriangleCf`.

**Direct callers:** 1 module(s), 1 cross-module call edge(s).

- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 1 caller function(s): [`ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf`](../src/world/model/hierarchy.c#L344)

**Calls into:** [`graphics/backend/software`](#module-graphics-backend-software), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`world/model/runtime`](#module-world-model-runtime).

<a id="module-graphics-render-shading"></a>
### `graphics/render/shading`

[Source](../src/graphics/render/shading.c) · [Header](../include/thandor/graphics/render/shading.h) · [Call graph](callgraphs/graphics/render/shading.md) · [Graphviz](callgraphs/graphics/render/shading.dot)

This file owns lighting, shading, and generated-texture work within rendering, graphics backends, and GPU/software resources. Dominant function families: `GraphicsShadingGeneratedTexture_*` (19), `GraphicsShadingRuntime_*` (7), `GraphicsIntensityClampTable_*` (1). Representative entry points: `GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy`, `GraphicsIntensityClampTable_InitializeCf`, `GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs`.

**Direct callers:** 10 module(s), 22 cross-module call edge(s).

- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 3 caller function(s): [`FrontendRomTransition_ActivateRecordByIdCf`](../src/assets/rom/runtime.c#L388), [`RomRuntime_ApplyIndexedDescriptor`](../src/assets/rom/runtime.c#L792), [`RomRuntime_UpdateRecordVisibilityAndDescriptorsCf`](../src/assets/rom/runtime.c#L476)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_ShutdownAndReleaseResources`](../src/gameplay/session/runtime.c#L1444)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`GameRuntime_InitializeSpatialAudioAndRenderingCf`](../src/platform/bootstrap/runtime.c#L561)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 2 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806), [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../src/ui/frontend/runtime.c#L3522)
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 1 caller function(s): [`InGameShadingSettings_ApplyLevel`](../src/ui/ingame/settings.c#L648)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 2 caller function(s): [`ModelRuntime_CullAndRenderHierarchyRecursive`](../src/world/model/runtime.c#L66), [`ModelRuntime_RenderHierarchyRecursiveAlternatePath`](../src/world/model/runtime.c#L204)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)
- [`world/terrain/projection` source](../src/world/terrain/projection.c) · [graph](callgraphs/world/terrain/projection.md): 2 caller function(s): [`TerrainProjectedVertex_TransformProjectAndShadeVariantA`](../src/world/terrain/projection.c#L1498), [`TerrainProjectedVertex_TransformProjectAndShadeVariantB`](../src/world/terrain/projection.c#L1679)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`graphics/core/runtime`](#module-graphics-core-runtime), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-graphics-resources-framebuffer"></a>
### `graphics/resources/framebuffer`

[Source](../src/graphics/resources/framebuffer.c) · [Header](../include/thandor/graphics/resources/framebuffer.h) · [Call graph](callgraphs/graphics/resources/framebuffer.md) · [Graphviz](callgraphs/graphics/resources/framebuffer.dot)

This file owns framebuffer and render-target resources within rendering, graphics backends, and GPU/software resources. Dominant function families: `GraphicsFramebuffer_*` (7). Representative entry points: `GraphicsFramebuffer_BeginAccessStub`, `GraphicsFramebuffer_EndAccessStub`, `GraphicsFramebuffer_Present`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`core/memory/allocator`](#module-core-memory-allocator), [`graphics/backend/glide`](#module-graphics-backend-glide), [`graphics/core/runtime`](#module-graphics-core-runtime).

<a id="module-graphics-resources-palette"></a>
### `graphics/resources/palette`

[Source](../src/graphics/resources/palette.c) · [Header](../include/thandor/graphics/resources/palette.h) · [Call graph](callgraphs/graphics/resources/palette.md) · [Graphviz](callgraphs/graphics/resources/palette.dot)

This file owns palette resources and conversion within rendering, graphics backends, and GPU/software resources. Dominant function families: `GraphicsPaletteAsset_*` (7), `GraphicsPaletteTextureSource_*` (6). Representative entry points: `GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices`, `GraphicsPaletteAsset_GetBankCountRegs`, `GraphicsPaletteAsset_LoadPackage`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime).

<a id="module-graphics-resources-texture"></a>
### `graphics/resources/texture`

[Source](../src/graphics/resources/texture.c) · [Header](../include/thandor/graphics/resources/texture.h) · [Call graph](callgraphs/graphics/resources/texture.md) · [Graphviz](callgraphs/graphics/resources/texture.dot)

This file owns texture resources and upload/lookup within rendering, graphics backends, and GPU/software resources. Dominant function families: `GraphicsTexture_*` (14), `GraphicsTextureSource_*` (13), `GraphicsTextureSet_*` (9). Representative entry points: `GraphicsTextureSet_Create`, `GraphicsTexture_RebuildAllStagingTextures`, `GraphicsTextureSet_Destroy`.

**Direct callers:** 5 module(s), 13 cross-module call edge(s).

- [`graphics/backend/direct3d` source](../src/graphics/backend/direct3d.c) · [graph](callgraphs/graphics/backend/direct3d.md): 5 caller function(s): [`Direct3D_PrimitiveHandler_TexturedPreset0`](../src/graphics/backend/direct3d.c#L1327), [`Direct3D_PrimitiveHandler_TexturedPreset1`](../src/graphics/backend/direct3d.c#L1530), [`Direct3D_PrimitiveHandler_TexturedPreset2`](../src/graphics/backend/direct3d.c#L1733); +2 more
- [`graphics/backend/directdraw` source](../src/graphics/backend/directdraw.c) · [graph](callgraphs/graphics/backend/directdraw.md): 1 caller function(s): [`GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf`](../src/graphics/backend/directdraw.c#L180)
- [`graphics/backend/glide` source](../src/graphics/backend/glide.c) · [graph](callgraphs/graphics/backend/glide.md): 3 caller function(s): [`Glide3_Shutdown`](../src/graphics/backend/glide.c#L4199), [`Glide3_TextureSet_CreateBackend`](../src/graphics/backend/glide.c#L5), [`Glide3_TextureSet_DestroyBackend`](../src/graphics/backend/glide.c#L577)
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 1 caller function(s): [`Graphics_Shutdown`](../src/graphics/core/runtime.c#L674)
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 1 caller function(s): [`UiFormattedContainer_DrawClipped`](../src/ui/controls/text.c#L3471)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`core/memory/allocator`](#module-core-memory-allocator), [`graphics/backend/glide`](#module-graphics-backend-glide).

## Movie

<a id="module-movie-runtime-playback"></a>
### `movie/runtime/playback`

[Source](../src/movie/runtime/playback.c) · [Header](../include/thandor/movie/runtime/playback.h) · [Call graph](callgraphs/movie/runtime/playback.md) · [Graphviz](callgraphs/movie/runtime/playback.dot)

This file owns movie playback and scheduling within movie playback and frame scheduling. Dominant function families: `Movie_*` (11), `EndMovieUiRuntime_*` (2), `MovieColor_*` (2). Representative entry points: `Movie_EncodeFlmBufferFromFrameProviderCf`, `MoviePlayback_AdvanceScheduledFrameAndTick`, `Movie_Open`.

**Direct callers:** 11 module(s), 34 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_InitializePoolAndGraphicsCf`](../src/gameplay/army/runtime.c#L1453)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_ProcessQueuedSessionNotificationTimer`](../src/gameplay/session/runtime.c#L628); +1 more
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_PlayIntroMovies`](../src/platform/bootstrap/runtime.c#L1078)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_PlaySelectedEndMovie`](../src/ui/frontend/runtime.c#L2552)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 2 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129), [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 2 caller function(s): [`FrontendSessionAction_CloseMovieAndReturnToMainPage`](../src/ui/frontend/session.c#L29), [`FrontendSession_ApplyGameSpeedAndReturnToMainPage`](../src/ui/frontend/session.c#L585)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState`](../src/ui/ingame/runtime.c#L4927)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntime_InitGraphicsResources`](../src/world/effects/runtime.c#L36)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntime_InitGraphicsResources`](../src/world/shots/runtime.c#L97)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 2 caller function(s): [`TerrainVisualResources_LoadAndClearCellOverlayFlags`](../src/world/terrain/visuals.c#L400), [`TerrainVisualResources_LoadPrimary`](../src/world/terrain/visuals.c#L228)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`core/math/fixed`](#module-core-math-fixed), [`core/math/random`](#module-core-math-random), [`core/text/path`](#module-core-text-path), [`gameplay/session/runtime`](#module-gameplay-session-runtime), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/frontend/session`](#module-ui-frontend-session).

## Network

<a id="module-network-backend-fallback-udp"></a>
### `network/backend/fallback_udp`

[Source](../src/network/backend/fallback_udp.c) · [Header](../include/thandor/network/backend/fallback_udp.h) · [Call graph](callgraphs/network/backend/fallback_udp.md) · [Graphviz](callgraphs/network/backend/fallback_udp.dot)

This file owns fallback UDP transport within network transports, packet transfer, and synchronized commands. Dominant function families: `NetworkBackendFallback_*` (8), `NetworkFallback_*` (8), `NetworkFallbackUdp_*` (3). Representative entry points: `NetworkBackendFallback_Slot0_ReturnError43Cf`, `NetworkBackendFallback_Slot1_NoOp`, `NetworkBackendFallback_Slot2_ReturnError43Cf`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/string`](#module-core-text-string).

<a id="module-network-backend-runtime"></a>
### `network/backend/runtime`

[Source](../src/network/backend/runtime.c) · [Header](../include/thandor/network/backend/runtime.h) · [Call graph](callgraphs/network/backend/runtime.md) · [Graphviz](callgraphs/network/backend/runtime.dot)

This file owns runtime state and lifecycle operations within network transports, packet transfer, and synchronized commands. Dominant function families: `FrontendNetwork_*` (4), `Network_*` (2), `NetworkBackend_*` (2). Representative entry points: `FrontendNetwork_HandleHandshakeAndPlayerStatePackets`, `FrontendNetwork_HostTickCommandAndSnapshotTransfer`, `FrontendNetwork_TickDisconnectTimeoutAndResetSession`.

**Direct callers:** 6 module(s), 7 cross-module call edge(s).

- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_StateTick`](../src/ui/frontend/runtime.c#L3017)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 1 caller function(s): [`FrontendSession_PeriodicTick`](../src/ui/frontend/session.c#L314)

**Calls into:** [`assets/package/codec`](#module-assets-package-codec), [`assets/rom/runtime`](#module-assets-rom-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/math/random`](#module-core-math-random), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`ui/ingame/runtime`](#module-ui-ingame-runtime).

<a id="module-network-protocol-commands"></a>
### `network/protocol/commands`

[Source](../src/network/protocol/commands.c) · [Header](../include/thandor/network/protocol/commands.h) · [Call graph](callgraphs/network/protocol/commands.md) · [Graphviz](callgraphs/network/protocol/commands.dot)

This file owns synchronized command queues within network transports, packet transfer, and synchronized commands. Dominant function families: `InGameCommandQueue_*` (3), `FrontendCommandQueue_*` (2). Representative entry points: `FrontendCommandQueue_EnqueueLocalPlayerCommand`, `FrontendCommandQueue_DequeueFirstIntoRecord`, `InGameCommandQueue_AppendLocalPlayerCommand`.

**Direct callers:** 14 module(s), 76 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 10 caller function(s): [`FrontendScenarioAction_StartFieldGridLoad`](../src/assets/scenario/catalog.c#L385), [`FrontendScenarioPage_OpenCampaignRecordsAndRefresh`](../src/assets/scenario/catalog.c#L346), [`FrontendScenarioPage_OpenLevelRecordsAndRefresh`](../src/assets/scenario/catalog.c#L308); +7 more
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 4 caller function(s): [`InGameTargetingContext_AdvanceOrResolveTarget`](../src/gameplay/input/world.c#L5), [`InGameWorldInput_BeginPointerCaptureCf`](../src/gameplay/input/world.c#L320), [`InGameWorldInput_CommitPointerActionCf`](../src/gameplay/input/world.c#L611); +1 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 2 caller function(s): [`FrontendNetwork_HostTickCommandAndSnapshotTransfer`](../src/network/backend/runtime.c#L142), [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 6 caller function(s): [`FrontendTransfer_BroadcastPendingCommandBatchAndSyncState`](../src/network/protocol/transfer.c#L1008), [`FrontendTransfer_HandleHostSessionAndCommandBatchPackets`](../src/network/protocol/transfer.c#L225), [`FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets`](../src/network/protocol/transfer.c#L617); +3 more
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 4 caller function(s): [`FrontendPlayerConsensus_SubmitSelectedValue`](../src/ui/frontend/player.c#L140), [`FrontendPlayerMessage_SubmitSevenSlotText`](../src/ui/frontend/player.c#L5), [`FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus`](../src/ui/frontend/player.c#L778); +1 more
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 14 caller function(s): [`FrontendCallback_ApplyGameSpeedOrDispatch02C0`](../src/ui/frontend/runtime.c#L1830), [`FrontendCallback_ReleaseSelectedResourceOrDispatch0320`](../src/ui/frontend/runtime.c#L1857), [`FrontendCallback_ReturnToMainPageOrDispatch0DC0`](../src/ui/frontend/runtime.c#L1897); +11 more
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 5 caller function(s): [`FrontendHostSession_TickShutdownOrReadyConsensus`](../src/ui/frontend/session.c#L510), [`FrontendSessionAction_ApplySpeedOrToggleReady`](../src/ui/frontend/session.c#L71), [`FrontendSessionAction_CloseMovieAndReturnToMainPage`](../src/ui/frontend/session.c#L29); +2 more
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendGameplaySettings_SetGameSpeedPercent`](../src/ui/frontend/settings.c#L412)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 10 caller function(s): [`InGameCommandAction_ClearSelectedArmyTokenAndClosePage`](../src/ui/ingame/commands.c#L720), [`InGameCommandAction_SetFlag1000OrMarkReady`](../src/ui/ingame/commands.c#L202), [`InGameCommandAction_ToggleRuntimeFlag0800`](../src/ui/ingame/commands.c#L236); +7 more
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 11 caller function(s): [`InGameHud_UpdateStatusCountersAndSessionPrompts`](../src/ui/ingame/runtime.c#L2674), [`InGameOtherPlayerCommand_DispatchSelectedTarget`](../src/ui/ingame/runtime.c#L3530), [`InGameSelectionPage_ToggleAndRefreshPage2`](../src/ui/ingame/runtime.c#L3592); +8 more
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 2 caller function(s): [`InGameSettingsAction_CloseAndDepartPlayerMode0`](../src/ui/ingame/settings.c#L30), [`InGameSettingsAction_CloseAndDepartPlayerMode1`](../src/ui/ingame/settings.c#L64)
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 1 caller function(s): [`InGameTechnologyResearch_StartSelected`](../src/ui/ingame/technology.c#L410)

<a id="module-network-protocol-transfer"></a>
### `network/protocol/transfer`

[Source](../src/network/protocol/transfer.c) · [Header](../include/thandor/network/protocol/transfer.h) · [Call graph](callgraphs/network/protocol/transfer.md) · [Graphviz](callgraphs/network/protocol/transfer.dot)

This file owns packet staging, transfer, and protocol handlers within network transports, packet transfer, and synchronized commands. Dominant function families: `FrontendTransfer_*` (15), `UiTransferMailbox_*` (6), `UiTransfer_*` (4). Representative entry points: `UiTransferMailbox_ServiceAndRetransmitTimer`, `FrontendTransfer_HandleHostSessionAndCommandBatchPackets`, `FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`.

**Direct callers:** 9 module(s), 46 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 4 caller function(s): [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995), [`FrontendScenarioSession_LoadOrRequestFieldGrid`](../src/assets/scenario/catalog.c#L871), [`FrontendScenarioSession_LoadOrRequestLevelAsset`](../src/assets/scenario/catalog.c#L1381); +1 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 4 caller function(s): [`FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf`](../src/network/backend/runtime.c#L411), [`FrontendNetwork_HandleHandshakeAndPlayerStatePackets`](../src/network/backend/runtime.c#L5), [`FrontendNetwork_HostTickCommandAndSnapshotTransfer`](../src/network/backend/runtime.c#L142); +1 more
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 3 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5), [`FrontendTransferPage_OpenAndRequestMailbox`](../src/ui/frontend/network.c#L315), [`FrontendTransferPage_ValidateInputAndRequestMailbox`](../src/ui/frontend/network.c#L283)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 1 caller function(s): [`FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers`](../src/ui/frontend/player.c#L192)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 3 caller function(s): [`FrontendUiAction200F_Handler`](../src/ui/frontend/runtime.c#L2473), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5), [`Frontend_StateTick`](../src/ui/frontend/runtime.c#L3017)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 3 caller function(s): [`FrontendClientSession_DecrementTimeoutsAndCompactPlayers`](../src/ui/frontend/session.c#L398), [`FrontendSession_PeriodicTick`](../src/ui/frontend/session.c#L314), [`FrontendTransferPage_ResetSessionOpenAndRequestMailbox`](../src/ui/frontend/session.c#L210)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendNetworkSettings_PublishSelectedPlayerDescriptorCf`](../src/ui/frontend/settings.c#L1638)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/math/random`](#module-core-math-random), [`network/protocol/commands`](#module-network-protocol-commands), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`ui/frontend/session`](#module-ui-frontend-session), [`ui/support/runtime`](#module-ui-support-runtime).

## Platform

<a id="module-platform-bootstrap-runtime"></a>
### `platform/bootstrap/runtime`

[Source](../src/platform/bootstrap/runtime.c) · [Header](../include/thandor/platform/bootstrap/runtime.h) · [Call graph](callgraphs/platform/bootstrap/runtime.md) · [Graphviz](callgraphs/platform/bootstrap/runtime.dot)

This file owns runtime state and lifecycle operations within platform bootstrap, Win32 services, filesystem, and input devices. Dominant function families: `DynDLL_*` (3), `Game_*` (3), `CommandLine_*` (2). Representative entry points: `ProcessEntry`, `GameData_ResetDefaults`, `GameData_LoadExternalTables`.

**Direct callers:** 8 module(s), 20 cross-module call edge(s).

- [`audio/backend/runtime` source](../src/audio/backend/runtime.c) · [graph](callgraphs/audio/backend/runtime.md): 1 caller function(s): [`DirectSound_Init`](../src/audio/backend/runtime.c#L147)
- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`graphics/backend/glide` source](../src/graphics/backend/glide.c) · [graph](callgraphs/graphics/backend/glide.md): 3 caller function(s): [`Glide3_InitAndEnumerate`](../src/graphics/backend/glide.c#L692), [`Glide3_Shutdown`](../src/graphics/backend/glide.c#L4199), [`GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf`](../src/graphics/backend/glide.c#L115)
- [`graphics/core/runtime` source](../src/graphics/core/runtime.c) · [graph](callgraphs/graphics/core/runtime.md): 1 caller function(s): [`Graphics_Init`](../src/graphics/core/runtime.c#L518)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`Network_Init`](../src/network/backend/runtime.c#L516)
- [`platform/input/devices` source](../src/platform/input/devices.c) · [graph](callgraphs/platform/input/devices.md): 1 caller function(s): [`DirectInputMouse_Init`](../src/platform/input/devices.c#L81)
- [`platform/system/time_locale` source](../src/platform/system/time_locale.c) · [graph](callgraphs/platform/system/time_locale.md): 1 caller function(s): [`Locale_Init`](../src/platform/system/time_locale.c#L28)

**Calls into:** [`assets/fnc/runtime`](#module-assets-fnc-runtime), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`audio/backend/runtime`](#module-audio-backend-runtime), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/error/runtime`](#module-core-error-runtime), [`core/memory/allocator`](#module-core-memory-allocator), [`core/memory/synchronization`](#module-core-memory-synchronization), [`core/settings/persistent`](#module-core-settings-persistent), [`core/text/path`](#module-core-text-path), [`core/text/string`](#module-core-text-string), [`gameplay/ai/workspaces`](#module-gameplay-ai-workspaces), [`graphics/backend/software`](#module-graphics-backend-software), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/render/shading`](#module-graphics-render-shading), [`movie/runtime/playback`](#module-movie-runtime-playback), [`network/backend/runtime`](#module-network-backend-runtime), [`platform/filesystem/win32`](#module-platform-filesystem-win32), [`platform/input/devices`](#module-platform-input-devices), [`platform/system/time_locale`](#module-platform-system-time-locale), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`world/terrain/visuals`](#module-world-terrain-visuals).

<a id="module-platform-filesystem-win32"></a>
### `platform/filesystem/win32`

[Source](../src/platform/filesystem/win32.c) · [Header](../include/thandor/platform/filesystem/win32.h) · [Call graph](callgraphs/platform/filesystem/win32.md) · [Graphviz](callgraphs/platform/filesystem/win32.dot)

This file owns Win32 platform integration within platform bootstrap, Win32 services, filesystem, and input devices. Dominant function families: `Win32File_*` (16), `Win32Drive_*` (5), `FileSystem_*` (2). Representative entry points: `FileSystem_Init`, `Win32File_GetLastWriteDosDateCf`, `Win32File_GetLastWriteTimeHighCf`.

**Direct callers:** 7 module(s), 7 cross-module call edge(s).

- [`assets/resource/runtime` source](../src/assets/resource/runtime.c) · [graph](callgraphs/assets/resource/runtime.md): 1 caller function(s): [`ResourceRegistration_OpenSourceCf`](../src/assets/resource/runtime.c#L5)
- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`core/settings/persistent` source](../src/core/settings/persistent.c) · [graph](callgraphs/core/settings/persistent.md): 1 caller function(s): [`PersistentSettings_Flush`](../src/core/settings/persistent.c#L5)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 1 caller function(s): [`InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf`](../src/gameplay/session/level.c#L1596)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`](../src/ui/ingame/runtime.c#L5)
- [`world/terrain/grid` source](../src/world/terrain/grid.c) · [graph](callgraphs/world/terrain/grid.md): 1 caller function(s): [`FieldGrid_SaveAssetImageFromRuntimeStateCf`](../src/world/terrain/grid.c#L2689)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/text/richtext`](#module-assets-text-richtext), [`core/error/runtime`](#module-core-error-runtime), [`core/memory/allocator`](#module-core-memory-allocator), [`core/text/path`](#module-core-text-path), [`core/text/string`](#module-core-text-string).

<a id="module-platform-input-devices"></a>
### `platform/input/devices`

[Source](../src/platform/input/devices.c) · [Header](../include/thandor/platform/input/devices.h) · [Call graph](callgraphs/platform/input/devices.md) · [Graphviz](callgraphs/platform/input/devices.dot)

This file owns input-device acquisition and polling within platform bootstrap, Win32 services, filesystem, and input devices. Dominant function families: `Keyboard_*` (8), `DirectInputMouse_*` (7). Representative entry points: `Keyboard_CompareAsciiCaseInsensitiveFlags`, `Keyboard_FlushEvents`, `Keyboard_ReadNextEventRegs`.

**Direct callers:** 4 module(s), 7 cross-module call edge(s).

- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 2 caller function(s): [`MainWindowProc`](../src/platform/bootstrap/runtime.c#L410), [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 1 caller function(s): [`UiPointer_DispatchPendingEvents`](../src/ui/controls/input.c#L5)
- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 1 caller function(s): [`UiFrame_Update`](../src/ui/controls/layout.c#L1592)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime), [`platform/system/time_locale`](#module-platform-system-time-locale).

<a id="module-platform-system-time-locale"></a>
### `platform/system/time_locale`

[Source](../src/platform/system/time_locale.c) · [Header](../include/thandor/platform/system/time_locale.h) · [Call graph](callgraphs/platform/system/time_locale.md) · [Graphviz](callgraphs/platform/system/time_locale.dot)

This file owns time, locale, and system-format helpers within platform bootstrap, Win32 services, filesystem, and input devices. Dominant function families: `Locale_*` (11), `TimerSystem_*` (4), `WinMM_*` (1). Representative entry points: `TimerSystem_Shutdown`, `Locale_Init`, `Locale_MapTelephoneCountryCodeToRegionTagPacked`.

**Direct callers:** 3 module(s), 5 cross-module call edge(s).

- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`platform/input/devices` source](../src/platform/input/devices.c) · [graph](callgraphs/platform/input/devices.md): 2 caller function(s): [`DirectInputMouse_Init`](../src/platform/input/devices.c#L81), [`DirectInputMouse_Shutdown`](../src/platform/input/devices.c#L246)

**Calls into:** [`core/text/string`](#module-core-text-string), [`platform/bootstrap/runtime`](#module-platform-bootstrap-runtime).

<a id="module-platform-system-win32"></a>
### `platform/system/win32`

[Source](../src/platform/system/win32.c) · [Header](../include/thandor/platform/system/win32.h) · [Call graph](callgraphs/platform/system/win32.md) · [Graphviz](callgraphs/platform/system/win32.dot)

This file owns Win32 platform integration within platform bootstrap, Win32 services, filesystem, and input devices. Dominant function families: `Win32_*` (2). Representative entry points: `Win32_PumpMessages`, `Win32_ShouldTranslateMessageFlags`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`core/memory/synchronization`](#module-core-memory-synchronization).

## Ui

<a id="module-ui-controls-buttons"></a>
### `ui/controls/buttons`

[Source](../src/ui/controls/buttons.c) · [Header](../include/thandor/ui/controls/buttons.h) · [Call graph](callgraphs/ui/controls/buttons.md) · [Graphviz](callgraphs/ui/controls/buttons.dot)

This file owns button widgets and actions within frontend and in-game user interface. Dominant function families: `UiSpriteButtonControl_*` (7), `UiImageActionControl_*` (5), `UiConditionalActionControl_*` (4). Representative entry points: `UiTree_AdvanceSpriteButtonAnimations`, `UiSpriteButtonControl_Relocate`, `UiSpriteButtonControl_DrawClipped`.

**Direct callers:** 1 module(s), 1 cross-module call edge(s).

- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 1 caller function(s): [`UiFrame_Update`](../src/ui/controls/layout.c#L1592)

**Calls into:** [`assets/text/richtext`](#module-assets-text-richtext), [`core/math/random`](#module-core-math-random), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/core/runtime`](#module-ui-core-runtime).

<a id="module-ui-controls-input"></a>
### `ui/controls/input`

[Source](../src/ui/controls/input.c) · [Header](../include/thandor/ui/controls/input.h) · [Call graph](callgraphs/ui/controls/input.md) · [Graphviz](callgraphs/ui/controls/input.dot)

This file owns UI input routing within frontend and in-game user interface. Dominant function families: `UiFocusProxyControl_*` (12), `UiKeyboardFocus_*` (5), `UiPointer_*` (5). Representative entry points: `UiPointer_DispatchPendingEvents`, `UiKeyboardFocus_ReleaseNode`, `UiKeyboard_DispatchPendingEvents`.

**Direct callers:** 9 module(s), 30 cross-module call edge(s).

- [`ui/controls/buttons` source](../src/ui/controls/buttons.c) · [graph](callgraphs/ui/controls/buttons.md): 1 caller function(s): [`UiImageActionControl_HandleKeyboardActivationCf`](../src/ui/controls/buttons.c#L511)
- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 8 caller function(s): [`UiFrame_ProcessAndPresent`](../src/ui/controls/layout.c#L346), [`UiFrame_ProcessAndPresentWithLockTransition`](../src/ui/controls/layout.c#L311), [`UiNodeSubtree_AcquireKeyboardFocusDefaults`](../src/ui/controls/layout.c#L1519); +5 more
- [`ui/controls/lists` source](../src/ui/controls/lists.c) · [graph](callgraphs/ui/controls/lists.md): 5 caller function(s): [`UiListControl_HandleKeyboardNavigationCf`](../src/ui/controls/lists.c#L178), [`UiSelectableControl_KeyboardEventCf`](../src/ui/controls/lists.c#L1127), [`UiSelectableControl_SuppressIfActionId`](../src/ui/controls/lists.c#L1194); +2 more
- [`ui/controls/misc` source](../src/ui/controls/misc.c) · [graph](callgraphs/ui/controls/misc.md): 2 caller function(s): [`UiRangeSliderControl_SuppressIfActionId`](../src/ui/controls/misc.c#L532), [`UiRangeSliderControl_UnsuppressIfActionId`](../src/ui/controls/misc.c#L548)
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 8 caller function(s): [`UiNumericTextEditControl_HandleKeyboardAndCommitCf`](../src/ui/controls/text.c#L28), [`UiPathTextEditControl_HandleKeyboardAndValidateCf`](../src/ui/controls/text.c#L289), [`UiRequiredTextEditControl_HandleKeyboardAndValidateCf`](../src/ui/controls/text.c#L633); +5 more
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_KeyboardEventCf`](../src/ui/frontend/runtime.c#L1357)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction101F_Handler`](../src/ui/ingame/runtime.c#L3417)
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 1 caller function(s): [`InGameSettingsPage_ToggleAndSynchronizeControls`](../src/ui/ingame/settings.c#L1045)
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 1 caller function(s): [`InGameTechnologyPanel_ResetAndSelectCurrentArea`](../src/ui/ingame/technology.c#L36)

**Calls into:** [`core/math/random`](#module-core-math-random), [`platform/input/devices`](#module-platform-input-devices), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/misc`](#module-ui-controls-misc), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/ingame/runtime`](#module-ui-ingame-runtime).

<a id="module-ui-controls-layout"></a>
### `ui/controls/layout`

[Source](../src/ui/controls/layout.c) · [Header](../include/thandor/ui/controls/layout.h) · [Call graph](callgraphs/ui/controls/layout.md) · [Graphviz](callgraphs/ui/controls/layout.dot)

This file owns UI layout and geometry within frontend and in-game user interface. Dominant function families: `UiContainer_*` (10), `UiResizableWindowControl_*` (7), `UiFrame_*` (5). Representative entry points: `UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren`, `UiResizableWindowControl_DrawFrameTitleAndChildren`, `UiTitledWindowControl_DrawFrameTitleAndChildren`.

**Direct callers:** 25 module(s), 169 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 6 caller function(s): [`FrontendScenarioSelectionPage_InitializeAndApplyMapOption`](../src/assets/scenario/catalog.c#L103), [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995), [`FrontendScenarioSession_LoadOrRequestLevelAsset`](../src/assets/scenario/catalog.c#L1381); +3 more
- [`core/error/runtime` source](../src/core/error/runtime.c) · [graph](callgraphs/core/error/runtime.md): 2 caller function(s): [`FatalErrorDialog_DismissAndPopRoot`](../src/core/error/runtime.c#L42), [`FatalErrorRuntime_DispatchPendingErrorCf`](../src/core/error/runtime.c#L57)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_RunSessionUntilExit`](../src/gameplay/session/runtime.c#L5); +1 more
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 2 caller function(s): [`InGameSaveGameList_SelectAndRefreshDetail`](../src/gameplay/session/savegame.c#L5), [`InGameSaveGamePage_RebuildCatalog`](../src/gameplay/session/savegame.c#L130)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`MoviePlayback_AdvanceToFrameAndPresent`](../src/movie/runtime/playback.c#L1420)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 2 caller function(s): [`FrontendTransfer_HandleHostSessionAndCommandBatchPackets`](../src/network/protocol/transfer.c#L225), [`FrontendTransfer_HandleSessionListAndJoinAckPackets`](../src/network/protocol/transfer.c#L1184)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_PlayIntroMovies`](../src/platform/bootstrap/runtime.c#L1078)
- [`ui/controls/buttons` source](../src/ui/controls/buttons.c) · [graph](callgraphs/ui/controls/buttons.md): 4 caller function(s): [`UiConditionalActionControl_DrawClipped`](../src/ui/controls/buttons.c#L532), [`UiConditionalActionControl_HitTestWhenEnabled`](../src/ui/controls/buttons.c#L684), [`UiImageActionControl_DrawImageAndChildren`](../src/ui/controls/buttons.c#L394); +1 more
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 7 caller function(s): [`UiFocusProxyControl_HitTestChildProxy`](../src/ui/controls/input.c#L772), [`UiFocusProxyControl_RelocateChild`](../src/ui/controls/input.c#L530), [`UiImageControl_PointerMove`](../src/ui/controls/input.c#L829); +4 more
- [`ui/controls/lists` source](../src/ui/controls/lists.c) · [graph](callgraphs/ui/controls/lists.md): 9 caller function(s): [`UiListControl_DrawRowsAndSelection`](../src/ui/controls/lists.c#L2150), [`UiListControl_SuppressIfActionId`](../src/ui/controls/lists.c#L2296), [`UiListControl_UnsuppressIfActionId`](../src/ui/controls/lists.c#L2278); +6 more
- [`ui/controls/misc` source](../src/ui/controls/misc.c) · [graph](callgraphs/ui/controls/misc.md): 9 caller function(s): [`UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings`](../src/ui/controls/misc.c#L101), [`UiDisplayModeAction_ApplyPendingMode`](../src/ui/controls/misc.c#L257), [`UiDisplayModeAction_CancelAndRebuildPixelPacking`](../src/ui/controls/misc.c#L312); +6 more
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 22 caller function(s): [`UiArmyMetricsPanel_DrawTextureMetricsAndChildren`](../src/ui/controls/text.c#L3616), [`UiFillPanelControl_DrawColorOrTiledTextureAndChildren`](../src/ui/controls/text.c#L2605), [`UiFormattedContainer_RelocateWithPatchedTextPayloads`](../src/ui/controls/text.c#L3440); +19 more
- [`ui/core/runtime` source](../src/ui/core/runtime.c) · [graph](callgraphs/ui/core/runtime.md): 2 caller function(s): [`UiRuntime_Initialize`](../src/ui/core/runtime.c#L194), [`UiRuntime_OpenFourValueDialogCf`](../src/ui/core/runtime.c#L47)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 4 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5), [`FrontendNetworkSetupPage_InitializeFromCommandLine`](../src/ui/frontend/network.c#L342), [`FrontendNetworkSetupPage_InitializeSingleLocalPlayer`](../src/ui/frontend/network.c#L445); +1 more
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 2 caller function(s): [`FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel`](../src/ui/frontend/player.c#L106), [`FrontendPlayerSetup_OpenLocalPageAndResetRoster`](../src/ui/frontend/player.c#L608)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 9 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806), [`FrontendModelPointerContext_Layout`](../src/ui/frontend/runtime.c#L791), [`FrontendModelPointerContext_Relocate`](../src/ui/frontend/runtime.c#L759); +6 more
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 2 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129), [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 4 caller function(s): [`FrontendSessionAction_CloseMovieAndReturnToMainPage`](../src/ui/frontend/session.c#L29), [`FrontendSession_ReturnToMainPage`](../src/ui/frontend/session.c#L614), [`FrontendSession_ShowPage9WithCompactLayout`](../src/ui/frontend/session.c#L194); +1 more
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 5 caller function(s): [`FrontendAudioSettings_OpenAndSynchronize`](../src/ui/frontend/settings.c#L747), [`FrontendDisplaySettings_ApplyMode`](../src/ui/frontend/settings.c#L272), [`FrontendGameplaySettingsPage_InitializeFromPersistentSettings`](../src/ui/frontend/settings.c#L614); +2 more
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 4 caller function(s): [`InGameCommandAction_ClearSelectedArmyTokenAndClosePage`](../src/ui/ingame/commands.c#L720), [`InGameCommandPanel_OpenPage4AndRefreshAvailability`](../src/ui/ingame/commands.c#L762), [`UiCommandModeG_SelectAndSyncPages`](../src/ui/ingame/commands.c#L1847); +1 more
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 13 caller function(s): [`InGameOtherPlayerCommand_RebuildTargetEntries`](../src/ui/ingame/runtime.c#L3207), [`InGamePanel_RebuildPlayerStatusRows`](../src/ui/ingame/runtime.c#L2906), [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357); +10 more
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 7 caller function(s): [`InGameAudioSettings_OpenAndSynchronize`](../src/ui/ingame/settings.c#L526), [`InGameGameplaySettings_SetRightButtonDoesNotScroll`](../src/ui/ingame/settings.c#L234), [`InGameGraphicsSettings_OpenAndSynchronize`](../src/ui/ingame/settings.c#L444); +4 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 3 caller function(s): [`InGameTechnologyResearch_StartSelected`](../src/ui/ingame/technology.c#L410), [`UiCatalogGroup42_RebuildGrid`](../src/ui/ingame/technology.c#L259), [`UiCatalogGroup48_RebuildGrid`](../src/ui/ingame/technology.c#L101)
- [`ui/support/runtime` source](../src/ui/support/runtime.c) · [graph](callgraphs/ui/support/runtime.md): 1 caller function(s): [`CreditsScreen_Open`](../src/ui/support/runtime.c#L131)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`platform/input/devices`](#module-platform-input-devices), [`ui/controls/buttons`](#module-ui-controls-buttons), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime).

<a id="module-ui-controls-lists"></a>
### `ui/controls/lists`

[Source](../src/ui/controls/lists.c) · [Header](../include/thandor/ui/controls/lists.h) · [Call graph](callgraphs/ui/controls/lists.md) · [Graphviz](callgraphs/ui/controls/lists.dot)

This file owns list widgets, selection, and scrolling within frontend and in-game user interface. Dominant function families: `UiScrollableControl_*` (16), `UiPointerList_*` (10), `UiListControl_*` (6). Representative entry points: `UiTimedListControl_HandleKeyboardNavigationCf`, `UiListControl_HandleKeyboardNavigationCf`, `UiPointerList_RefreshSelectionAndQueueAction`.

**Direct callers:** 16 module(s), 221 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 11 caller function(s): [`FrontendScenarioSelectionPage_InitializeAndApplyMapOption`](../src/assets/scenario/catalog.c#L103), [`FrontendScenarioSelection_ActivateSelectedRecord`](../src/assets/scenario/catalog.c#L1578), [`FrontendScenarioSelection_ApplyField50Selection`](../src/assets/scenario/catalog.c#L70); +8 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87)
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 5 caller function(s): [`InGameSaveGameAction_DeleteSelectedSaveAndRefreshCatalog`](../src/gameplay/session/savegame.c#L90), [`InGameSaveGameList_SelectAndRefreshDetail`](../src/gameplay/session/savegame.c#L5), [`InGameSaveGamePage_RebuildCatalog`](../src/gameplay/session/savegame.c#L130); +2 more
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 2 caller function(s): [`FrontendTransfer_HandleHostSessionAndCommandBatchPackets`](../src/network/protocol/transfer.c#L225), [`FrontendTransfer_HandleSessionListAndJoinAckPackets`](../src/network/protocol/transfer.c#L1184)
- [`ui/controls/misc` source](../src/ui/controls/misc.c) · [graph](callgraphs/ui/controls/misc.md): 1 caller function(s): [`UiDisplayModeSelection_RefreshEnumeratedOptions`](../src/ui/controls/misc.c#L1114)
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 4 caller function(s): [`UiPointerList_SortByExpandedTextFieldAscending`](../src/ui/controls/text.c#L1427), [`UiPointerList_SortByExpandedTextFieldDescending`](../src/ui/controls/text.c#L1491), [`UiTextListControl_HandleKeyboardNavigationAndSearchCf`](../src/ui/controls/text.c#L3072); +1 more
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 3 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5), [`FrontendNetworkSetupPage_InitializeSingleLocalPlayer`](../src/ui/frontend/network.c#L445), [`FrontendTransferPage_OpenAndRequestMailbox`](../src/ui/frontend/network.c#L315)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 3 caller function(s): [`FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks`](../src/ui/frontend/player.c#L1518), [`FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B`](../src/ui/frontend/player.c#L725), [`FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction`](../src/ui/frontend/player.c#L669)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 5 caller function(s): [`FrontendRuntime_UpdatePointerContextAndSceneViewCf`](../src/ui/frontend/runtime.c#L1587), [`FrontendUiAction200C_Handler`](../src/ui/frontend/runtime.c#L2421), [`FrontendUiAction200F_Handler`](../src/ui/frontend/runtime.c#L2473); +2 more
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 2 caller function(s): [`FrontendMissionBriefingPage_Initialize`](../src/ui/frontend/scenario.c#L129), [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 2 caller function(s): [`FrontendSessionList_DecrementExpiryAndCompactRows`](../src/ui/frontend/session.c#L252), [`FrontendTransferPage_ResetSessionOpenAndRequestMailbox`](../src/ui/frontend/session.c#L210)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 19 caller function(s): [`FrontendAudioSettings_OpenAndSynchronize`](../src/ui/frontend/settings.c#L747), [`FrontendAudioSettings_SetEffectsEnabled`](../src/ui/frontend/settings.c#L964), [`FrontendAudioSettings_SetMusicEnabled`](../src/ui/frontend/settings.c#L1061); +16 more
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 24 caller function(s): [`InGameCommandModeA_Select0`](../src/ui/ingame/commands.c#L1250), [`InGameCommandModeA_Select1`](../src/ui/ingame/commands.c#L1264), [`InGameCommandModeA_Select2`](../src/ui/ingame/commands.c#L1278); +21 more
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 11 caller function(s): [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357), [`InGameSelectionPage_RebuildActivePlayerEntries`](../src/ui/ingame/runtime.c#L3651), [`InGameSelectionPage_RebuildRuntimeRecordEntries`](../src/ui/ingame/runtime.c#L3717); +8 more
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 23 caller function(s): [`InGameAudioSettings_OpenAndSynchronize`](../src/ui/ingame/settings.c#L526), [`InGameAudioSettings_SetEffectsEnabled`](../src/ui/ingame/settings.c#L773), [`InGameAudioSettings_SetMusicEnabled`](../src/ui/ingame/settings.c#L877); +20 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 3 caller function(s): [`InGameTechnologyAreaTab_SelectAndRebuild`](../src/ui/ingame/technology.c#L5), [`InGameTechnologyPanel_Rebuild`](../src/ui/ingame/technology.c#L457), [`InGameTechnologyResearch_StartSelected`](../src/ui/ingame/technology.c#L410)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/ingame/runtime`](#module-ui-ingame-runtime).

<a id="module-ui-controls-misc"></a>
### `ui/controls/misc`

[Source](../src/ui/controls/misc.c) · [Header](../include/thandor/ui/controls/misc.h) · [Call graph](callgraphs/ui/controls/misc.md) · [Graphviz](callgraphs/ui/controls/misc.dot)

This file owns miscellaneous UI controls within frontend and in-game user interface. Dominant function families: `UiDisplayModeAction_*` (6), `UiImageControl_*` (6), `UiRangeSliderControl_*` (5). Representative entry points: `UiDisplaySettingsRoot_RefreshModeSelection`, `UiDisplayModeAction_UpdateAdapterSelection`, `UiDisplayModeAction_UpdateResolutionSelection`.

**Direct callers:** 4 module(s), 6 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 2 caller function(s): [`ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback`](../src/gameplay/army/runtime.c#L1647), [`ArmyRuntime_CreateInstanceFromAssetCf`](../src/gameplay/army/runtime.c#L3859)
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 2 caller function(s): [`UiPointer_DispatchLeftPress`](../src/ui/controls/input.c#L1324), [`UiPointer_DispatchMiddlePress`](../src/ui/controls/input.c#L1417)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint`](../src/world/effects/maintenance.c#L5)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint`](../src/world/shots/maintenance.c#L5)

**Calls into:** [`graphics/backend/directdraw`](#module-graphics-backend-directdraw), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/core/runtime`](#module-ui-core-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy).

<a id="module-ui-controls-text"></a>
### `ui/controls/text`

[Source](../src/ui/controls/text.c) · [Header](../include/thandor/ui/controls/text.h) · [Call graph](callgraphs/ui/controls/text.md) · [Graphviz](callgraphs/ui/controls/text.dot)

This file owns UI text, labels, and text widgets within frontend and in-game user interface. Dominant function families: `UiTextEditControl_*` (10), `UiFramedTextButtonControl_*` (6), `UiTextListControl_*` (6). Representative entry points: `UiTooltip_TickCountdown`, `UiNumericTextEditControl_HandleKeyboardAndCommitCf`, `UiPathTextEditControl_HandleKeyboardAndValidateCf`.

**Direct callers:** 11 module(s), 13 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 1 caller function(s): [`ScenarioCatalog_RebuildLevelRecordListPage`](../src/assets/scenario/catalog.c#L1177)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_RunSessionUntilExit`](../src/gameplay/session/runtime.c#L5)
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 1 caller function(s): [`InGameSaveGamePage_RebuildCatalog`](../src/gameplay/session/savegame.c#L130)
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 1 caller function(s): [`UiPointer_DispatchMotionAndWheel`](../src/ui/controls/input.c#L1639)
- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 2 caller function(s): [`UiFrame_Draw`](../src/ui/controls/layout.c#L1647), [`UiFrame_Update`](../src/ui/controls/layout.c#L1592)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeFromCommandLine`](../src/ui/frontend/network.c#L342)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 1 caller function(s): [`FrontendPlayerMessage_SubmitSevenSlotText`](../src/ui/frontend/player.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendNetworkSettings_SetPlayerName`](../src/ui/frontend/settings.c#L364)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 2 caller function(s): [`UiCommandVisibilitySingleLineText_DrawWhenAllowed`](../src/ui/ingame/commands.c#L470), [`UiCommandVisibilityWrappedText_DrawWhenAllowed`](../src/ui/ingame/commands.c#L452)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1024_Handler`](../src/ui/ingame/runtime.c#L516)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`graphics/backend/software`](#module-graphics-backend-software), [`graphics/resources/texture`](#module-graphics-resources-texture), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/core/runtime`](#module-ui-core-runtime).

<a id="module-ui-core-runtime"></a>
### `ui/core/runtime`

[Source](../src/ui/core/runtime.c) · [Header](../include/thandor/ui/core/runtime.h) · [Call graph](callgraphs/ui/core/runtime.md) · [Graphviz](callgraphs/ui/core/runtime.dot)

This file owns runtime state and lifecycle operations within frontend and in-game user interface. Dominant function families: `UiNode_*` (12), `UiRuntime_*` (6), `UiRuntimeRecordRing_*` (4). Representative entry points: `UiRootCallbacks_FreeCf`, `UiRootCallbacks_NoOpMethod08`, `UiRuntime_FormatSignedValues140And144`.

**Direct callers:** 17 module(s), 133 cross-module call edge(s).

- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 2 caller function(s): [`FrontendRomActionTable_ExecuteRecord`](../src/assets/rom/runtime.c#L5), [`FrontendRomTransition_ActivateRecordByIdCf`](../src/assets/rom/runtime.c#L388)
- [`core/memory/synchronization` source](../src/core/memory/synchronization.c) · [graph](callgraphs/core/memory/synchronization.md): 1 caller function(s): [`Runtime_Shutdown`](../src/core/memory/synchronization.c#L79)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_RunSessionUntilExit`](../src/gameplay/session/runtime.c#L5); +1 more
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 1 caller function(s): [`InGameSaveGameList_SelectAndRefreshDetail`](../src/gameplay/session/savegame.c#L5)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 2 caller function(s): [`Game_LoadCoreAssets`](../src/platform/bootstrap/runtime.c#L593), [`ProcessEntry`](../src/platform/bootstrap/runtime.c#L5)
- [`ui/controls/buttons` source](../src/ui/controls/buttons.c) · [graph](callgraphs/ui/controls/buttons.md): 8 caller function(s): [`UiConditionalActionControl_EnqueuePrimaryActionIfEnabled`](../src/ui/controls/buttons.c#L703), [`UiImageActionControl_EnqueuePrimaryAction`](../src/ui/controls/buttons.c#L483), [`UiImageActionControl_EnqueueSecondaryAction`](../src/ui/controls/buttons.c#L497); +5 more
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 14 caller function(s): [`UiFocusProxyControl_ForwardKeyboardEventToChildCf`](../src/ui/controls/input.c#L345), [`UiFocusProxyControl_ForwardNonRightDragToChild`](../src/ui/controls/input.c#L681), [`UiFocusProxyControl_ForwardNonRightPressToChild`](../src/ui/controls/input.c#L562); +11 more
- [`ui/controls/layout` source](../src/ui/controls/layout.c) · [graph](callgraphs/ui/controls/layout.md): 11 caller function(s): [`UiFrame_ProcessAndPresent`](../src/ui/controls/layout.c#L346), [`UiFrame_ProcessAndPresentWithLockTransition`](../src/ui/controls/layout.c#L311), [`UiLayoutContainerControl_ApplyFlagsRecursive`](../src/ui/controls/layout.c#L1252); +8 more
- [`ui/controls/lists` source](../src/ui/controls/lists.c) · [graph](callgraphs/ui/controls/lists.md): 17 caller function(s): [`UiCatalogEntryControl_NonRightRelease`](../src/ui/controls/lists.c#L2831), [`UiListControl_HandleKeyboardNavigationCf`](../src/ui/controls/lists.c#L178), [`UiListControl_SelectRowFromPointer`](../src/ui/controls/lists.c#L783); +14 more
- [`ui/controls/misc` source](../src/ui/controls/misc.c) · [graph](callgraphs/ui/controls/misc.md): 14 caller function(s): [`UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings`](../src/ui/controls/misc.c#L101), [`UiDisplayModeAction_ApplyPendingMode`](../src/ui/controls/misc.c#L257), [`UiDisplayModeAction_CancelAndRebuildPixelPacking`](../src/ui/controls/misc.c#L312); +11 more
- [`ui/controls/text` source](../src/ui/controls/text.c) · [graph](callgraphs/ui/controls/text.md): 20 caller function(s): [`UiFramedTextButtonControl_NonRightDrag`](../src/ui/controls/text.c#L2044), [`UiFramedTextButtonControl_NonRightPress`](../src/ui/controls/text.c#L1966), [`UiFramedTextButtonControl_NonRightRelease`](../src/ui/controls/text.c#L2015); +17 more
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 4 caller function(s): [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../src/ui/frontend/runtime.c#L3522), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749), [`Frontend_PlaySelectedEndMovie`](../src/ui/frontend/runtime.c#L2552); +1 more
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 1 caller function(s): [`FrontendSession_PeriodicTick`](../src/ui/frontend/session.c#L314)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 3 caller function(s): [`UiCommandSpriteButtonControl_BeginPress`](../src/ui/ingame/commands.c#L326), [`UiCommandSpriteButtonControl_NonRightRelease`](../src/ui/ingame/commands.c#L352), [`UiCommandSpriteButtonControl_RightRelease`](../src/ui/ingame/commands.c#L385)
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 2 caller function(s): [`UiCatalogGroup42_RebuildGrid`](../src/ui/ingame/technology.c#L259), [`UiCatalogGroup48_RebuildGrid`](../src/ui/ingame/technology.c#L101)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/error/runtime`](#module-core-error-runtime), [`ui/controls/layout`](#module-ui-controls-layout).

<a id="module-ui-frontend-network"></a>
### `ui/frontend/network`

[Source](../src/ui/frontend/network.c) · [Header](../include/thandor/ui/frontend/network.h) · [Call graph](callgraphs/ui/frontend/network.md) · [Graphviz](callgraphs/ui/frontend/network.dot)

This file owns frontend network UI within frontend and in-game user interface. Dominant function families: `FrontendNetworkSetupPage_*` (3), `FrontendTransferPage_*` (2), `FrontendTeardown_*` (1). Representative entry points: `FrontendNetworkSetupPage_InitializeBackendMode`, `FrontendTeardown_SaveRootStateSnapshot80`, `FrontendTransferPage_ValidateInputAndRequestMailbox`.

**Direct callers:** 1 module(s), 2 cross-module call edge(s).

- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 2 caller function(s): [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../src/ui/frontend/runtime.c#L3522), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/string`](#module-core-text-string), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/session`](#module-ui-frontend-session), [`ui/frontend/settings`](#module-ui-frontend-settings), [`ui/support/runtime`](#module-ui-support-runtime).

<a id="module-ui-frontend-player"></a>
### `ui/frontend/player`

[Source](../src/ui/frontend/player.c) · [Header](../include/thandor/ui/frontend/player.h) · [Call graph](callgraphs/ui/frontend/player.md) · [Graphviz](callgraphs/ui/frontend/player.dot)

This file owns frontend player/roster UI within frontend and in-game user interface. Dominant function families: `FrontendPlayerRuntime_*` (21), `FrontendPlayerSelection_*` (5), `FrontendPlayerMessageBuffer_*` (3). Representative entry points: `FrontendPlayerMessage_SubmitSevenSlotText`, `FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel`, `FrontendPlayerConsensus_SubmitSelectedValue`.

**Direct callers:** 13 module(s), 47 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 2 caller function(s): [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995), [`FrontendScenarioTransfer_ProcessReceivedAsset`](../src/assets/scenario/catalog.c#L626)
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 4 caller function(s): [`InGameTargetingContext_AdvanceOrResolveTarget`](../src/gameplay/input/world.c#L5), [`InGameWorldInput_BeginPointerCaptureCf`](../src/gameplay/input/world.c#L320), [`InGameWorldInput_CommitPointerActionCf`](../src/gameplay/input/world.c#L611); +1 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 1 caller function(s): [`FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets`](../src/network/protocol/transfer.c#L617)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeSingleLocalPlayer`](../src/ui/frontend/network.c#L445)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 3 caller function(s): [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749), [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5), [`Frontend_PlaySelectedEndMovie`](../src/ui/frontend/runtime.c#L2552)
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 3 caller function(s): [`FrontendClientSession_DecrementTimeoutsAndCompactPlayers`](../src/ui/frontend/session.c#L398), [`FrontendHostSession_TickShutdownOrReadyConsensus`](../src/ui/frontend/session.c#L510), [`FrontendSessionAction_ApplySpeedOrToggleReady`](../src/ui/frontend/session.c#L71)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 3 caller function(s): [`InGameCommandAction_ClearSelectedArmyTokenAndClosePage`](../src/ui/ingame/commands.c#L720), [`InGameCommandAction_SetFlag1000OrMarkReady`](../src/ui/ingame/commands.c#L202), [`InGameCommandSprite_DispatchFixedControl8`](../src/ui/ingame/commands.c#L1013)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 9 caller function(s): [`InGameHud_UpdateStatusCountersAndSessionPrompts`](../src/ui/ingame/runtime.c#L2674), [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357), [`InGameSelectionPage_ToggleAndRefreshPage2`](../src/ui/ingame/runtime.c#L3592); +6 more
- [`ui/ingame/technology` source](../src/ui/ingame/technology.c) · [graph](callgraphs/ui/ingame/technology.md): 1 caller function(s): [`InGameTechnologyResearch_StartSelected`](../src/ui/ingame/technology.c#L410)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 1 caller function(s): [`ModelRuntimePool_DestroyHierarchyAndDetach`](../src/world/model/runtime.c#L644)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/string`](#module-core-text-string), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`ui/frontend/settings`](#module-ui-frontend-settings), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/technology`](#module-ui-ingame-technology), [`world/runtime/core`](#module-world-runtime-core).

<a id="module-ui-frontend-results"></a>
### `ui/frontend/results`

[Source](../src/ui/frontend/results.c) · [Header](../include/thandor/ui/frontend/results.h) · [Call graph](callgraphs/ui/frontend/results.md) · [Graphviz](callgraphs/ui/frontend/results.dot)

This file owns frontend result screens within frontend and in-game user interface. Dominant function families: `FrontendResultsTable_*` (9). Representative entry points: `FrontendResultsTable_DrawColumnSequenceByType`, `FrontendResultsTable_HitTestAlwaysNone`, `FrontendResultsTable_DrawColourColumn`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext).

<a id="module-ui-frontend-runtime"></a>
### `ui/frontend/runtime`

[Source](../src/ui/frontend/runtime.c) · [Header](../include/thandor/ui/frontend/runtime.h) · [Call graph](callgraphs/ui/frontend/runtime.md) · [Graphviz](callgraphs/ui/frontend/runtime.dot)

This file owns runtime state and lifecycle operations within frontend and in-game user interface. Dominant function families: `FrontendModelPointerContext_*` (14), `FrontendCallback_*` (6), `FrontendRuntime_*` (6). Representative entry points: `Frontend_MainLoop`, `FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction`, `FrontendModelPointerContext_NonRightPress`.

**Direct callers:** 7 module(s), 11 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 2 caller function(s): [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../src/assets/scenario/catalog.c#L995), [`FrontendScenarioSession_LoadOrRequestLevelAsset`](../src/assets/scenario/catalog.c#L1381)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameRuntime_RunSessionUntilExit`](../src/gameplay/session/runtime.c#L5), [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_TickDisconnectTimeoutAndResetSession`](../src/network/backend/runtime.c#L335)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 2 caller function(s): [`FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`](../src/network/protocol/transfer.c#L340), [`FrontendTransfer_HandleHostSessionAndCommandBatchPackets`](../src/network/protocol/transfer.c#L225)
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`Game_Run`](../src/platform/bootstrap/runtime.c#L508)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 2 caller function(s): [`FrontendPlayerMessageBuffer_PublishTextById`](../src/ui/frontend/player.c#L1473), [`FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers`](../src/ui/frontend/player.c#L192)
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 1 caller function(s): [`FrontendSession_ReturnToMainPage`](../src/ui/frontend/session.c#L614)

**Calls into:** [`assets/package/codec`](#module-assets-package-codec), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/rom/runtime`](#module-assets-rom-runtime), [`assets/scenario/catalog`](#module-assets-scenario-catalog), [`assets/sprite/catalog`](#module-assets-sprite-catalog), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/math/fixed`](#module-core-math-fixed), [`core/math/interpolation`](#module-core-math-interpolation), [`core/math/random`](#module-core-math-random), [`core/settings/persistent`](#module-core-settings-persistent), [`core/text/path`](#module-core-text-path), [`core/text/string`](#module-core-text-string), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/overlay`](#module-gameplay-selection-overlay), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`gameplay/session/runtime`](#module-gameplay-session-runtime), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/render/shading`](#module-graphics-render-shading), [`movie/runtime/playback`](#module-movie-runtime-playback), [`network/backend/runtime`](#module-network-backend-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/network`](#module-ui-frontend-network), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/scenario`](#module-ui-frontend-scenario), [`ui/frontend/session`](#module-ui-frontend-session), [`ui/frontend/settings`](#module-ui-frontend-settings), [`ui/support/runtime`](#module-ui-support-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/motion/runtime`](#module-world-motion-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/projection`](#module-world-terrain-projection).

<a id="module-ui-frontend-scenario"></a>
### `ui/frontend/scenario`

[Source](../src/ui/frontend/scenario.c) · [Header](../include/thandor/ui/frontend/scenario.h) · [Call graph](callgraphs/ui/frontend/scenario.md) · [Graphviz](callgraphs/ui/frontend/scenario.dot)

This file owns frontend scenario selection within frontend and in-game user interface. Dominant function families: `FrontendMissionBriefingPage_*` (1), `FrontendRoot_*` (1). Representative entry points: `FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`, `FrontendMissionBriefingPage_Initialize`.

**Direct callers:** 1 module(s), 1 cross-module call edge(s).

- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`Frontend_MainLoop`](../src/ui/frontend/runtime.c#L5)

**Calls into:** [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/settings/persistent`](#module-core-settings-persistent), [`core/text/path`](#module-core-text-path), [`graphics/backend/software`](#module-graphics-backend-software), [`movie/runtime/playback`](#module-movie-runtime-playback), [`network/backend/runtime`](#module-network-backend-runtime), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/session`](#module-ui-frontend-session), [`ui/support/runtime`](#module-ui-support-runtime).

<a id="module-ui-frontend-session"></a>
### `ui/frontend/session`

[Source](../src/ui/frontend/session.c) · [Header](../include/thandor/ui/frontend/session.h) · [Call graph](callgraphs/ui/frontend/session.md) · [Graphviz](callgraphs/ui/frontend/session.dot)

This file owns frontend session/lobby UI within frontend and in-game user interface. Dominant function families: `FrontendSession_*` (6), `FrontendSessionAction_*` (4), `FrontendClientSession_*` (1). Representative entry points: `FrontendSession_ReleaseSelectedResourceAndReturnToMainPage`, `FrontendSessionAction_CloseMovieAndReturnToMainPage`, `FrontendSessionAction_ApplySpeedOrToggleReady`.

**Direct callers:** 8 module(s), 18 cross-module call edge(s).

- [`assets/scenario/catalog` source](../src/assets/scenario/catalog.c) · [graph](callgraphs/assets/scenario/catalog.md): 2 caller function(s): [`FrontendScenarioSelection_ActivateSelectedRecord`](../src/assets/scenario/catalog.c#L1578), [`FrontendScenarioSession_LoadOrRequestFieldGrid`](../src/assets/scenario/catalog.c#L871)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87)
- [`movie/runtime/playback` source](../src/movie/runtime/playback.c) · [graph](callgraphs/movie/runtime/playback.md): 1 caller function(s): [`EndMovieUiRuntime_HandleModeTransitionCf`](../src/movie/runtime/playback.c#L523)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 1 caller function(s): [`FrontendTransfer_TickRequestTimeoutAndResetPage`](../src/network/protocol/transfer.c#L1271)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeBackendMode`](../src/ui/frontend/network.c#L5)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 8 caller function(s): [`FrontendCallback_ApplyGameSpeedOrDispatch02C0`](../src/ui/frontend/runtime.c#L1830), [`FrontendCallback_ReleaseSelectedResourceOrDispatch0320`](../src/ui/frontend/runtime.c#L1857), [`FrontendCallback_ReturnToMainPageOrDispatch0DC0`](../src/ui/frontend/runtime.c#L1897); +5 more
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/frontend/settings` source](../src/ui/frontend/settings.c) · [graph](callgraphs/ui/frontend/settings.md): 1 caller function(s): [`FrontendGameplaySettings_SetGameSpeedPercent`](../src/ui/frontend/settings.c#L412)

**Calls into:** [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/math/random`](#module-core-math-random), [`movie/runtime/playback`](#module-movie-runtime-playback), [`network/backend/runtime`](#module-network-backend-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/frontend/runtime`](#module-ui-frontend-runtime), [`ui/ingame/runtime`](#module-ui-ingame-runtime).

<a id="module-ui-frontend-settings"></a>
### `ui/frontend/settings`

[Source](../src/ui/frontend/settings.c) · [Header](../include/thandor/ui/frontend/settings.h) · [Call graph](callgraphs/ui/frontend/settings.md) · [Graphviz](callgraphs/ui/frontend/settings.dot)

This file owns settings UI and option synchronization within frontend and in-game user interface. Dominant function families: `FrontendAudioSettings_*` (8), `FrontendGameplaySettings_*` (8), `FrontendNetworkSettings_*` (5). Representative entry points: `FrontendTaskAssignmentPage_Initialize`, `FrontendDisplaySettingsAction_ApplyPendingResolution`, `FrontendDisplaySettingsAction_ApplyPendingColorDepth`.

**Direct callers:** 3 module(s), 8 cross-module call edge(s).

- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeFromCommandLine`](../src/ui/frontend/network.c#L342)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 1 caller function(s): [`FrontendPlayerRuntime_SetConsensusValueAndRefresh`](../src/ui/frontend/player.c#L1346)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 5 caller function(s): [`FrontendUiAction2011_Handler`](../src/ui/frontend/runtime.c#L2023), [`FrontendUiAction202CTo2030_SharedHandler`](../src/ui/frontend/runtime.c#L2392), [`FrontendUiAction2045_IndexedSelectionHelper`](../src/ui/frontend/runtime.c#L3351); +2 more

**Calls into:** [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/settings/persistent`](#module-core-settings-persistent), [`graphics/backend/directdraw`](#module-graphics-backend-directdraw), [`network/protocol/commands`](#module-network-protocol-commands), [`network/protocol/transfer`](#module-network-protocol-transfer), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/frontend/session`](#module-ui-frontend-session).

<a id="module-ui-ingame-commands"></a>
### `ui/ingame/commands`

[Source](../src/ui/ingame/commands.c) · [Header](../include/thandor/ui/ingame/commands.h) · [Call graph](callgraphs/ui/ingame/commands.md) · [Graphviz](callgraphs/ui/ingame/commands.dot)

This file owns synchronized command queues within frontend and in-game user interface. Dominant function families: `UiCommandModeG_*` (15), `InGameCommandModeG_*` (6), `InGameCommandModeC_*` (4). Representative entry points: `InGameCommandModeG_Select0`, `InGameCommandModeG_Select1`, `InGameCommandModeG_Select2`.

**Direct callers:** 6 module(s), 45 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntimeClassUpdateSlot11_DispatchByClassId`](../src/gameplay/army/runtime.c#L1050)
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 4 caller function(s): [`GameFactionRuntime_ApplyPairwiseRelationTransition`](../src/gameplay/faction/runtime.c#L1398), [`GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid`](../src/gameplay/faction/runtime.c#L1112), [`GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer`](../src/gameplay/faction/runtime.c#L1045); +1 more
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 1 caller function(s): [`InGameWorldInput_CommitPointerActionCf`](../src/gameplay/input/world.c#L611)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 2 caller function(s): [`InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState`](../src/ui/ingame/runtime.c#L4927), [`InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`](../src/ui/ingame/runtime.c#L5)
- [`ui/ingame/settings` source](../src/ui/ingame/settings.c) · [graph](callgraphs/ui/ingame/settings.md): 2 caller function(s): [`InGameSettingsAction_CloseAndDepartPlayerMode0`](../src/ui/ingame/settings.c#L30), [`InGameSettingsAction_CloseAndDepartPlayerMode1`](../src/ui/ingame/settings.c#L64)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`gameplay/army/placement`](#module-gameplay-army-placement), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/ingame/runtime`](#module-ui-ingame-runtime), [`ui/ingame/settings`](#module-ui-ingame-settings), [`ui/ingame/technology`](#module-ui-ingame-technology), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/visuals`](#module-world-terrain-visuals).

<a id="module-ui-ingame-runtime"></a>
### `ui/ingame/runtime`

[Source](../src/ui/ingame/runtime.c) · [Header](../include/thandor/ui/ingame/runtime.h) · [Call graph](callgraphs/ui/ingame/runtime.md) · [Graphviz](callgraphs/ui/ingame/runtime.dot)

This file owns runtime state and lifecycle operations within frontend and in-game user interface. Dominant function families: `InGameUiCommand_*` (6), `InGameSelectionPage_*` (4), `InGameUiRuntime_*` (4). Representative entry points: `InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`, `InGameSevenSlotCommand_SubmitAndClosePage`, `InGameUiAction1024_Handler`.

**Direct callers:** 13 module(s), 38 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 4 caller function(s): [`ArmyRuntimeClassUpdateSlot11_DispatchByClassId`](../src/gameplay/army/runtime.c#L1050), [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`](../src/gameplay/army/runtime.c#L406); +1 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 3 caller function(s): [`GameFactionRuntime_ApplyPairwiseRelationTransition`](../src/gameplay/faction/runtime.c#L1398), [`GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10`](../src/gameplay/faction/runtime.c#L367), [`GameFactionRuntime_UpdateImpactAlertAnchorAndNotify`](../src/gameplay/faction/runtime.c#L452)
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 1 caller function(s): [`InGameWorldInput_ResolveContextActionAndCursorCf`](../src/gameplay/input/world.c#L101)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 2 caller function(s): [`InGamePlayerSelection_ReplaceWithArmyRuntimeIndex`](../src/gameplay/selection/runtime.c#L520), [`InGameSelection_RebuildOwnedClass16Selection`](../src/gameplay/selection/runtime.c#L482)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718); +1 more
- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 1 caller function(s): [`InGameSaveGame_SaveSelectedOrTypedName`](../src/gameplay/session/savegame.c#L287)
- [`gameplay/technology/runtime` source](../src/gameplay/technology/runtime.c) · [graph](callgraphs/gameplay/technology/runtime.md): 1 caller function(s): [`Technology_UnlockForFaction`](../src/gameplay/technology/runtime.c#L5)
- [`network/backend/runtime` source](../src/network/backend/runtime.c) · [graph](callgraphs/network/backend/runtime.md): 1 caller function(s): [`FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf`](../src/network/backend/runtime.c#L411)
- [`ui/controls/input` source](../src/ui/controls/input.c) · [graph](callgraphs/ui/controls/input.md): 1 caller function(s): [`UiPointer_DispatchMotionAndWheel`](../src/ui/controls/input.c#L1639)
- [`ui/controls/lists` source](../src/ui/controls/lists.c) · [graph](callgraphs/ui/controls/lists.md): 1 caller function(s): [`UiCatalogEntryControl_PointerMove`](../src/ui/controls/lists.c#L2786)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 6 caller function(s): [`FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection`](../src/ui/frontend/player.c#L1588), [`FrontendPlayerSelection_ClearAndRefreshLocalPanels`](../src/ui/frontend/player.c#L961), [`FrontendPlayerSelection_InsertThreeEntriesAndRefresh`](../src/ui/frontend/player.c#L851); +3 more
- [`ui/frontend/session` source](../src/ui/frontend/session.c) · [graph](callgraphs/ui/frontend/session.md): 2 caller function(s): [`FrontendClientSession_DecrementTimeoutsAndCompactPlayers`](../src/ui/frontend/session.c#L398), [`FrontendHostSession_TickShutdownOrReadyConsensus`](../src/ui/frontend/session.c#L510)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 3 caller function(s): [`InGameCommand150_HandlePlayerDepartureAndOwnership`](../src/ui/ingame/commands.c#L1467), [`InGameCommandModeG_Select3`](../src/ui/ingame/commands.c#L102), [`UiCommandSpriteVariantA_PointerMove`](../src/ui/ingame/commands.c#L417)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/path`](#module-core-text-path), [`core/text/string`](#module-core-text-string), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`gameplay/faction/relations`](#module-gameplay-faction-relations), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`gameplay/session/level`](#module-gameplay-session-level), [`gameplay/session/runtime`](#module-gameplay-session-runtime), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`movie/runtime/playback`](#module-movie-runtime-playback), [`network/protocol/commands`](#module-network-protocol-commands), [`platform/filesystem/win32`](#module-platform-filesystem-win32), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/controls/text`](#module-ui-controls-text), [`ui/frontend/player`](#module-ui-frontend-player), [`ui/ingame/commands`](#module-ui-ingame-commands), [`ui/ingame/technology`](#module-ui-ingame-technology), [`ui/support/runtime`](#module-ui-support-runtime), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/shots/runtime`](#module-world-shots-runtime), [`world/terrain/editing`](#module-world-terrain-editing), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/visuals`](#module-world-terrain-visuals).

<a id="module-ui-ingame-settings"></a>
### `ui/ingame/settings`

[Source](../src/ui/ingame/settings.c) · [Header](../include/thandor/ui/ingame/settings.h) · [Call graph](callgraphs/ui/ingame/settings.md) · [Graphviz](callgraphs/ui/ingame/settings.dot)

This file owns settings UI and option synchronization within frontend and in-game user interface. Dominant function families: `InGameAudioSettings_*` (8), `InGameGameplaySettings_*` (7), `InGameSettingsPage_*` (6). Representative entry points: `InGameSettingsAction_CloseAlternatePanel`, `InGameSettingsAction_CloseAndDepartPlayerMode0`, `InGameSettingsAction_CloseAndDepartPlayerMode1`.

**Direct callers:** 2 module(s), 2 cross-module call edge(s).

- [`gameplay/session/savegame` source](../src/gameplay/session/savegame.c) · [graph](callgraphs/gameplay/session/savegame.md): 1 caller function(s): [`InGameSaveGame_SaveSelectedOrTypedName`](../src/gameplay/session/savegame.c#L287)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommandState_CloseSettingsAndDispatchOperation150`](../src/ui/ingame/commands.c#L258)

**Calls into:** [`core/settings/persistent`](#module-core-settings-persistent), [`graphics/render/shading`](#module-graphics-render-shading), [`network/protocol/commands`](#module-network-protocol-commands), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/ingame/commands`](#module-ui-ingame-commands).

<a id="module-ui-ingame-technology"></a>
### `ui/ingame/technology`

[Source](../src/ui/ingame/technology.c) · [Header](../include/thandor/ui/ingame/technology.h) · [Call graph](callgraphs/ui/ingame/technology.md) · [Graphviz](callgraphs/ui/ingame/technology.dot)

This file owns technology-state logic within frontend and in-game user interface. Dominant function families: `InGameTechnologyPanel_*` (2), `InGameTechnologyAreaTab_*` (1), `InGameTechnologyResearch_*` (1). Representative entry points: `InGameTechnologyAreaTab_SelectAndRebuild`, `InGameTechnologyPanel_ResetAndSelectCurrentArea`, `UiCatalogGroup48_RebuildGrid`.

**Direct callers:** 8 module(s), 22 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntime_DestroyInstanceAndRefreshUi`](../src/gameplay/army/runtime.c#L3293)
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`GameFactionRuntime_ApplyPairwiseRelationTransition`](../src/gameplay/faction/runtime.c#L1398)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 2 caller function(s): [`InGamePlayerSelection_ReplaceWithArmyRuntimeIndex`](../src/gameplay/selection/runtime.c#L520), [`InGameSelection_RebuildOwnedClass16Selection`](../src/gameplay/selection/runtime.c#L482)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`gameplay/technology/runtime` source](../src/gameplay/technology/runtime.c) · [graph](callgraphs/gameplay/technology/runtime.md): 1 caller function(s): [`Technology_UnlockForFaction`](../src/gameplay/technology/runtime.c#L5)
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 6 caller function(s): [`FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel`](../src/ui/frontend/player.c#L106), [`FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection`](../src/ui/frontend/player.c#L1588), [`FrontendPlayerSelection_ClearAndRefreshLocalPanels`](../src/ui/frontend/player.c#L961); +3 more
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand_ExecuteLocalPlacementFromSelection`](../src/ui/ingame/commands.c#L527)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameSelectionPage_ToggleAndRefreshPage2`](../src/ui/ingame/runtime.c#L3592)

**Calls into:** [`assets/army/catalog`](#module-assets-army-catalog), [`assets/model/definitions`](#module-assets-model-definitions), [`assets/text/resources`](#module-assets-text-resources), [`assets/text/richtext`](#module-assets-text-richtext), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`gameplay/technology/runtime`](#module-gameplay-technology-runtime), [`network/protocol/commands`](#module-network-protocol-commands), [`ui/controls/input`](#module-ui-controls-input), [`ui/controls/layout`](#module-ui-controls-layout), [`ui/controls/lists`](#module-ui-controls-lists), [`ui/core/runtime`](#module-ui-core-runtime), [`ui/frontend/player`](#module-ui-frontend-player).

<a id="module-ui-support-runtime"></a>
### `ui/support/runtime`

[Source](../src/ui/support/runtime.c) · [Header](../include/thandor/ui/support/runtime.h) · [Call graph](callgraphs/ui/support/runtime.md) · [Graphviz](callgraphs/ui/support/runtime.dot)

This file owns runtime state and lifecycle operations within frontend and in-game user interface. Dominant function families: `RecentTextHistory_*` (4), `CreditsScreen_*` (1), `PcxPreview_*` (1). Representative entry points: `RecentTextHistory_SortAndBuildPointerList`, `RecentTextHistory_Insert`, `RecentTextHistory_RemoveOldest`.

**Direct callers:** 6 module(s), 16 cross-module call edge(s).

- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718)
- [`network/protocol/transfer` source](../src/network/protocol/transfer.c) · [graph](callgraphs/network/protocol/transfer.md): 1 caller function(s): [`UiTransfer_SendPlayerDescriptorPacket20002Cf`](../src/network/protocol/transfer.c#L579)
- [`ui/frontend/network` source](../src/ui/frontend/network.c) · [graph](callgraphs/ui/frontend/network.md): 1 caller function(s): [`FrontendNetworkSetupPage_InitializeSingleLocalPlayer`](../src/ui/frontend/network.c#L445)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 4 caller function(s): [`FrontendRecentTextHistory_InsertAndRebuild5`](../src/ui/frontend/runtime.c#L1811), [`FrontendRecentText_TrimAndSortTopFive`](../src/ui/frontend/runtime.c#L2447), [`Frontend_Init`](../src/ui/frontend/runtime.c#L2749); +1 more
- [`ui/frontend/scenario` source](../src/ui/frontend/scenario.c) · [graph](callgraphs/ui/frontend/scenario.md): 1 caller function(s): [`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](../src/ui/frontend/scenario.c#L5)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 2 caller function(s): [`InGameRecentTextHistory_InsertAndRebuild8`](../src/ui/ingame/runtime.c#L5167), [`InGameRecentText_TrimHistoryToThree`](../src/ui/ingame/runtime.c#L3800)

**Calls into:** [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/text/richtext`](#module-assets-text-richtext), [`core/text/path`](#module-core-text-path), [`graphics/backend/software`](#module-graphics-backend-software), [`ui/controls/layout`](#module-ui-controls-layout).

## World

<a id="module-world-effects-maintenance"></a>
### `world/effects/maintenance`

[Source](../src/world/effects/maintenance.c) · [Header](../include/thandor/world/effects/maintenance.h) · [Call graph](callgraphs/world/effects/maintenance.md) · [Graphviz](callgraphs/world/effects/maintenance.dot)

This file owns periodic update and cleanup operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `EffectRuntimeMaintenance_*` (3), `EffectModelRuntimeMaintenance_*` (1). Representative entry points: `EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint`, `EffectRuntimeMaintenance_OccupancyRebuildNoOp`, `EffectRuntimeMaintenance_AudioRefreshNoOp`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`core/math/fixed`](#module-core-math-fixed), [`core/math/interpolation`](#module-core-math-interpolation), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`graphics/render/shading`](#module-graphics-render-shading), [`ui/controls/misc`](#module-ui-controls-misc), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/shots/runtime`](#module-world-shots-runtime), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/occupancy`](#module-world-terrain-occupancy).

<a id="module-world-effects-runtime"></a>
### `world/effects/runtime`

[Source](../src/world/effects/runtime.c) · [Header](../include/thandor/world/effects/runtime.h) · [Call graph](callgraphs/world/effects/runtime.md) · [Graphviz](callgraphs/world/effects/runtime.dot)

This file owns runtime state and lifecycle operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `EffectRuntime_*` (4), `EffectRuntimePool_*` (1). Representative entry points: `EffectRuntime_FindDefinitionByIdCf`, `EffectRuntime_InitGraphicsResources`, `EffectRuntime_ShutdownGraphicsResources`.

**Direct callers:** 13 module(s), 23 cross-module call edge(s).

- [`assets/effect/catalog` source](../src/assets/effect/catalog.c) · [graph](callgraphs/assets/effect/catalog.md): 1 caller function(s): [`EffectDefinition_RegisterAndLoadSpriteCf`](../src/assets/effect/catalog.c#L98)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 3 caller function(s): [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628), [`AiConstructionPlanner_PlaceDerivedAsset14D`](../src/gameplay/ai/planning.c#L575), [`AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor`](../src/gameplay/ai/planning.c#L863)
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiConstructionPlanner_PlaceSpecialAssetFromWorkspace`](../src/gameplay/ai/workspaces.c#L1030)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyRuntime_EmitDamageThresholdEffect`](../src/gameplay/army/combat.c#L645)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 1 caller function(s): [`ArmyArticulatedRuntime_UpdateContactChildAndEffects`](../src/gameplay/army/movement.c#L2542)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 5 caller function(s): [`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`](../src/gameplay/army/runtime.c#L5), [`ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive`](../src/gameplay/army/runtime.c#L2995), [`ArmyRuntime_ClassCommandHandlerGroupACf`](../src/gameplay/army/runtime.c#L1231); +2 more
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 2 caller function(s): [`InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint`](../src/gameplay/selection/overlay.c#L986), [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand_ExecuteLocalPlacementFromSelection`](../src/ui/ingame/commands.c#L527)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`assets/resource/runtime`](#module-assets-resource-runtime), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/text/path`](#module-core-text-path), [`graphics/render/shading`](#module-graphics-render-shading), [`movie/runtime/playback`](#module-movie-runtime-playback), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/occupancy`](#module-world-terrain-occupancy).

<a id="module-world-model-hierarchy"></a>
### `world/model/hierarchy`

[Source](../src/world/model/hierarchy.c) · [Header](../include/thandor/world/model/hierarchy.h) · [Call graph](callgraphs/world/model/hierarchy.md) · [Graphviz](callgraphs/world/model/hierarchy.dot)

This file owns runtime model hierarchy and transforms within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `ModelNodeRuntime_*` (16), `ModelRuntimeHierarchy_*` (10), `ModelRuntimeNode_*` (2). Representative entry points: `ModelNodeRuntime_UpdateStateTintRecursive`, `ModelNodeRuntime_RebuildTransformsFromRoot`, `ModelRuntimeHierarchy_ApplyFactionTechnologyVariants`.

**Direct callers:** 24 module(s), 103 cross-module call edge(s).

- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 1 caller function(s): [`RomRuntime_BuildAllRegistryNodeTrees`](../src/assets/rom/runtime.c#L154)
- [`gameplay/ai/placement` source](../src/gameplay/ai/placement.c) · [graph](callgraphs/gameplay/ai/placement.md): 1 caller function(s): [`AiSiteCandidate_AddTerrainFeatureCellIfSeparated`](../src/gameplay/ai/placement.c#L256)
- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 4 caller function(s): [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628), [`AiConstructionPlanner_PlaceDerivedAsset14D`](../src/gameplay/ai/planning.c#L575), [`AiConstructionPlanner_PlaceExtendedAssetNearFactionAnchor`](../src/gameplay/ai/planning.c#L863); +1 more
- [`gameplay/ai/workspaces` source](../src/gameplay/ai/workspaces.c) · [graph](callgraphs/gameplay/ai/workspaces.md): 1 caller function(s): [`AiConstructionPlanner_PlaceSpecialAssetFromWorkspace`](../src/gameplay/ai/workspaces.c#L1030)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 3 caller function(s): [`ArmyRuntimeClass_UpdateTransformAndDamageEffect`](../src/gameplay/army/combat.c#L233), [`ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`](../src/gameplay/army/combat.c#L5), [`ArmyRuntime_EmitDamageThresholdEffect`](../src/gameplay/army/combat.c#L645)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 9 caller function(s): [`ArmyArticulatedRuntime_InitializeTerrainContactGeometry`](../src/gameplay/army/movement.c#L1550), [`ArmyArticulatedRuntime_UpdateSuspensionHierarchy`](../src/gameplay/army/movement.c#L2600), [`ArmyRuntimeClass_UpdateArticulatedMovement`](../src/gameplay/army/movement.c#L5); +6 more
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 2 caller function(s): [`ArmyPlacementCandidate_TestModelAnchorDistanceCf`](../src/gameplay/army/placement.c#L1146), [`ArmyPlacement_TestModelTerrainAndRuntimeClearance`](../src/gameplay/army/placement.c#L69)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 17 caller function(s): [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`](../src/gameplay/army/runtime.c#L5), [`ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy`](../src/gameplay/army/runtime.c#L1606); +14 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`GameFactionRuntime_ApplyPairwiseRelationTransition`](../src/gameplay/faction/runtime.c#L1398)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 1 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 4 caller function(s): [`SelectionPlayerRuntime_AdvancePrimarySelectionCycle`](../src/gameplay/selection/runtime.c#L795), [`SelectionPlayerRuntime_ReissuePrimarySelectionPosition`](../src/gameplay/selection/runtime.c#L733), [`SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries`](../src/gameplay/selection/runtime.c#L2505); +1 more
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 2 caller function(s): [`InGameConditionRuntime_UpdateScheduledRecords`](../src/gameplay/session/runtime.c#L1532), [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)
- [`gameplay/technology/runtime` source](../src/gameplay/technology/runtime.c) · [graph](callgraphs/gameplay/technology/runtime.md): 1 caller function(s): [`Technology_UnlockForFaction`](../src/gameplay/technology/runtime.c#L5)
- [`graphics/render/model` source](../src/graphics/render/model.c) · [graph](callgraphs/graphics/render/model.md): 2 caller function(s): [`ModelRender_DrawMeshGroupsAlternatePath`](../src/graphics/render/model.c#L108), [`ModelRender_DrawMeshGroupsWithTemporaryTransform`](../src/graphics/render/model.c#L5)
- [`ui/controls/misc` source](../src/ui/controls/misc.c) · [graph](callgraphs/ui/controls/misc.md): 1 caller function(s): [`UiModelControl_RefreshStateTint`](../src/ui/controls/misc.c#L708)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_FindBestEligibleModelHitTarget`](../src/ui/frontend/runtime.c#L3584)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 1 caller function(s): [`InGameCommand_ExecuteLocalPlacementFromSelection`](../src/ui/ingame/commands.c#L527)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 9 caller function(s): [`ModelRuntimePool_CreateInstanceByDefinitionIdCf`](../src/world/model/runtime.c#L782), [`ModelRuntimePool_DestroyHierarchyAndDetach`](../src/world/model/runtime.c#L644), [`ModelRuntimePool_RebaseAfterLoad`](../src/world/model/runtime.c#L534); +6 more
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 1 caller function(s): [`WorldRuntimeNode_ClearOwnedModelReferencesCallback`](../src/world/runtime/core.c#L1177)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`assets/shot/catalog`](#module-assets-shot-catalog), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/render/projection`](#module-graphics-render-projection), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core).

<a id="module-world-model-runtime"></a>
### `world/model/runtime`

[Source](../src/world/model/runtime.c) · [Header](../include/thandor/world/model/runtime.h) · [Call graph](callgraphs/world/model/runtime.md) · [Graphviz](callgraphs/world/model/runtime.dot)

This file owns runtime state and lifecycle operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `ModelRuntime_*` (8), `ModelRuntimePool_*` (7). Representative entry points: `ModelRuntimePool_RepairDeferredChild`, `ModelRuntime_CullAndRenderHierarchyRecursive`, `ModelRuntime_RenderHierarchyRecursiveAlternatePath`.

**Direct callers:** 13 module(s), 25 cross-module call edge(s).

- [`gameplay/ai/combat` source](../src/gameplay/ai/combat.c) · [graph](callgraphs/gameplay/ai/combat.md): 2 caller function(s): [`AiCombatTarget_EvaluateCandidateScore`](../src/gameplay/ai/combat.c#L218), [`AiUnitGroup_AssignCollectedEntitiesToBestTarget`](../src/gameplay/ai/combat.c#L47)
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyWeaponRuntime_TestTargetLineOfFireCf`](../src/gameplay/army/combat.c#L443)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 2 caller function(s): [`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA`](../src/gameplay/army/movement.c#L400), [`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB`](../src/gameplay/army/movement.c#L582)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 4 caller function(s): [`ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy`](../src/gameplay/army/runtime.c#L1606), [`ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects`](../src/gameplay/army/runtime.c#L2266), [`ArmyRuntime_CreateInstanceFromAssetCf`](../src/gameplay/army/runtime.c#L3859); +1 more
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 2 caller function(s): [`InGameWorldInput_CommitPointerActionCf`](../src/gameplay/input/world.c#L611), [`InGameWorldInput_ResolveContextActionAndCursorCf`](../src/gameplay/input/world.c#L101)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 1 caller function(s): [`SelectionPanel_RenderArmyRuntimeMetrics`](../src/gameplay/selection/runtime.c#L5)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`graphics/render/projection` source](../src/graphics/render/projection.c) · [graph](callgraphs/graphics/render/projection.md): 1 caller function(s): [`GraphicsOffscreen_RenderModelListToTextureSourceCf`](../src/graphics/render/projection.c#L5)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 2 caller function(s): [`InGameSelectionDetailPanel_Rebuild`](../src/ui/ingame/runtime.c#L5357), [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 1 caller function(s): [`ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf`](../src/world/model/hierarchy.c#L745)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntime_ApplyArmyHitRelationAndNotifications`](../src/world/shots/runtime.c#L5)

**Calls into:** [`assets/resource/runtime`](#module-assets-resource-runtime), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`graphics/render/model`](#module-graphics-render-model), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/render/shading`](#module-graphics-render-shading), [`ui/frontend/player`](#module-ui-frontend-player), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/runtime/core`](#module-world-runtime-core), [`world/shots/runtime`](#module-world-shots-runtime).

<a id="module-world-model-slots"></a>
### `world/model/slots`

[Source](../src/world/model/slots.c) · [Header](../include/thandor/world/model/slots.h) · [Call graph](callgraphs/world/model/slots.md) · [Graphviz](callgraphs/world/model/slots.dot)

This file owns model slot/attachment management within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `ModelRuntimeSlotClassInit_*` (14), `ModelRuntimeSlot_*` (4), `ModelRuntimeSlotClassRelease_*` (1). Representative entry points: `ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices`, `ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming`, `ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`world/runtime/core`](#module-world-runtime-core).

<a id="module-world-motion-runtime"></a>
### `world/motion/runtime`

[Source](../src/world/motion/runtime.c) · [Header](../include/thandor/world/motion/runtime.h) · [Call graph](callgraphs/world/motion/runtime.md) · [Graphviz](callgraphs/world/motion/runtime.dot)

This file owns runtime state and lifecycle operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `WorldMotion_*` (9), `WorldRuntime_*` (2). Representative entry points: `WorldRuntime_ApplyEdgeScrollAndGetCursorFrame`, `WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn`, `WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn`.

**Direct callers:** 2 module(s), 14 cross-module call edge(s).

- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 2 caller function(s): [`FrontendModelPointerContext_PointerWheel`](../src/ui/frontend/runtime.c#L1322), [`FrontendModelPointerContext_RightDrag`](../src/ui/frontend/runtime.c#L1179)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`core/settings/persistent`](#module-core-settings-persistent), [`world/runtime/core`](#module-world-runtime-core).

<a id="module-world-pathing-grid"></a>
### `world/pathing/grid`

[Source](../src/world/pathing/grid.c) · [Header](../include/thandor/world/pathing/grid.h) · [Call graph](callgraphs/world/pathing/grid.md) · [Graphviz](callgraphs/world/pathing/grid.dot)

This file owns grid traversal, routing, and cell-state operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `GridScratch_*` (12), `EntityPathing_*` (3), `GridFootprint_*` (3). Representative entry points: `EntityPathing_ResolveDestinationAndRebuildRoutes`, `GridReachability_RebuildConnectedRegionAroundWorldPoint`, `GridScratch_RebuildTerrainAndRuntimeClassificationMasks`.

**Direct callers:** 5 module(s), 18 cross-module call edge(s).

- [`gameplay/ai/planning` source](../src/gameplay/ai/planning.c) · [graph](callgraphs/gameplay/ai/planning.md): 1 caller function(s): [`AiConstructionPlanner_PlaceArmyAssetAtReachableCandidate`](../src/gameplay/ai/planning.c#L1628)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 7 caller function(s): [`ArmyRuntime_QueueOrStartMoveCommandVariantA`](../src/gameplay/army/movement.c#L1636), [`ArmyRuntime_QueueOrStartMoveCommandVariantB`](../src/gameplay/army/movement.c#L1680), [`ArmyRuntime_StartClampedMoveCommand`](../src/gameplay/army/movement.c#L2463); +4 more
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 2 caller function(s): [`ArmyPlacement_TestGridRuntimeAndFieldBlocking`](../src/gameplay/army/placement.c#L288), [`ArmyRuntimeCollision_TestShotSpawnPointCf`](../src/gameplay/army/placement.c#L325)
- [`gameplay/selection/runtime` source](../src/gameplay/selection/runtime.c) · [graph](callgraphs/gameplay/selection/runtime.md): 1 caller function(s): [`SelectionInfo_TestPositionCommandAtWorldPointCf`](../src/gameplay/selection/runtime.c#L1346)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_RunSessionUntilExit`](../src/gameplay/session/runtime.c#L5); +1 more

**Calls into:** [`core/memory/allocator`](#module-core-memory-allocator), [`gameplay/army/movement`](#module-gameplay-army-movement), [`graphics/render/primitives`](#module-graphics-render-primitives), [`world/pathing/influence`](#module-world-pathing-influence).

<a id="module-world-pathing-influence"></a>
### `world/pathing/influence`

[Source](../src/world/pathing/influence.c) · [Header](../include/thandor/world/pathing/influence.h) · [Call graph](callgraphs/world/pathing/influence.md) · [Graphviz](callgraphs/world/pathing/influence.dot)

This file owns pathing influence fields and distance bands within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `GridInfluence_*` (19). Representative entry points: `GridInfluence_AddLowDistanceBands`, `GridInfluence_RemoveLowDistanceBands`, `GridInfluence_AddHighDistanceBands`.

**Direct callers:** 2 module(s), 5 cross-module call edge(s).

- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718), [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)
- [`world/pathing/grid` source](../src/world/pathing/grid.c) · [graph](callgraphs/world/pathing/grid.md): 2 caller function(s): [`EntityPathing_RebuildOverlappingGroupRoutes`](../src/world/pathing/grid.c#L844), [`EntityPathing_UpdateRouteSegment`](../src/world/pathing/grid.c#L1112)

<a id="module-world-runtime-core"></a>
### `world/runtime/core`

[Source](../src/world/runtime/core.c) · [Header](../include/thandor/world/runtime/core.h) · [Call graph](callgraphs/world/runtime/core.md) · [Graphviz](callgraphs/world/runtime/core.dot)

This file owns core within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `WorldRuntime_*` (33), `UnifiedRuntimeDefault_*` (5), `RuntimeHexSegment_*` (4). Representative entry points: `WorldLightingRuntime_UpdateInterpolatedTerrainLighting`, `WorldRuntime_SetPosition60AndDistanceFromPosition80`, `WorldRuntime_SetMotionParameters6CThrough78Clamped`.

**Direct callers:** 21 module(s), 98 cross-module call edge(s).

- [`assets/resource/runtime` source](../src/assets/resource/runtime.c) · [graph](callgraphs/assets/resource/runtime.md): 1 caller function(s): [`ResourceRegistration_ResolveRuntimeRecord`](../src/assets/resource/runtime.c#L442)
- [`assets/rom/runtime` source](../src/assets/rom/runtime.c) · [graph](callgraphs/assets/rom/runtime.md): 3 caller function(s): [`FrontendRomTransition_ActivateRecordByIdCf`](../src/assets/rom/runtime.c#L388), [`RomRuntime_BuildAllRegistryNodeTrees`](../src/assets/rom/runtime.c#L154), [`RomRuntime_BuildNodeTreeRecursive`](../src/assets/rom/runtime.c#L638)
- [`core/math/interpolation` source](../src/core/math/interpolation.c) · [graph](callgraphs/core/math/interpolation.md): 2 caller function(s): [`WorldMotionSpline_EvaluateAndApplyAtTime`](../src/core/math/interpolation.c#L5), [`WorldMotionSpline_EvaluateAndApplyOriginDistanceAtTimeCf`](../src/core/math/interpolation.c#L104)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 5 caller function(s): [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects`](../src/gameplay/army/runtime.c#L2266), [`ArmyRuntime_CreateInstanceFromAssetCf`](../src/gameplay/army/runtime.c#L3859); +2 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`OldUnitRuntime_MergeMasksAndReplayRecords`](../src/gameplay/faction/runtime.c#L1293)
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 6 caller function(s): [`InGameCameraCommand_DispatchByCodeAndModifierFlagsCf`](../src/gameplay/input/world.c#L873), [`InGameTargetingContext_AdvanceOrResolveTarget`](../src/gameplay/input/world.c#L5), [`InGameTargetingContext_CancelAndRestoreState`](../src/gameplay/input/world.c#L1078); +3 more
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 1 caller function(s): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 2 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 5 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718); +2 more
- [`ui/frontend/player` source](../src/ui/frontend/player.c) · [graph](callgraphs/ui/frontend/player.md): 1 caller function(s): [`FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh`](../src/ui/frontend/player.c#L982)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 6 caller function(s): [`FrontendDebugOverlay_RefreshCountersAndWorldCoordinates`](../src/ui/frontend/runtime.c#L3440), [`FrontendModelPointerContext_Layout`](../src/ui/frontend/runtime.c#L791), [`FrontendModelPointerContext_PointerWheel`](../src/ui/frontend/runtime.c#L1322); +3 more
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 7 caller function(s): [`InGameHud_UpdateStatusCountersAndSessionPrompts`](../src/ui/ingame/runtime.c#L2674), [`InGameMapAction_RecenterViewFromGridCoordinates`](../src/ui/ingame/runtime.c#L888), [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682); +4 more
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/model/hierarchy` source](../src/world/model/hierarchy.c) · [graph](callgraphs/world/model/hierarchy.md): 2 caller function(s): [`ModelNodeRuntime_CreateHierarchyRecursiveCf`](../src/world/model/hierarchy.c#L1004), [`ModelRuntimeNode_ReleaseRecursiveAndDetachParent`](../src/world/model/hierarchy.c#L1148)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 1 caller function(s): [`ModelRuntimePool_DestroyHierarchyAndDetach`](../src/world/model/runtime.c#L644)
- [`world/model/slots` source](../src/world/model/slots.c) · [graph](callgraphs/world/model/slots.md): 2 caller function(s): [`ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1`](../src/world/model/slots.c#L301), [`ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3`](../src/world/model/slots.c#L267)
- [`world/motion/runtime` source](../src/world/motion/runtime.c) · [graph](callgraphs/world/motion/runtime.md): 11 caller function(s): [`WorldMotion_AdjustDistanceClampAndRecomputePosition`](../src/world/motion/runtime.c#L231), [`WorldMotion_AdjustHeadingAndClearFieldGridDirty`](../src/world/motion/runtime.c#L216), [`WorldMotion_AdjustHeadingAndRecomputePosition`](../src/world/motion/runtime.c#L189); +8 more
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 1 caller function(s): [`TerrainLighting_AdjustDirectionAndRecomputeField`](../src/world/terrain/visuals.c#L711)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`core/math/fixed`](#module-core-math-fixed), [`gameplay/army/runtime`](#module-gameplay-army-runtime), [`graphics/core/runtime`](#module-graphics-core-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/visuals`](#module-world-terrain-visuals).

<a id="module-world-shots-maintenance"></a>
### `world/shots/maintenance`

[Source](../src/world/shots/maintenance.c) · [Header](../include/thandor/world/shots/maintenance.h) · [Call graph](callgraphs/world/shots/maintenance.md) · [Graphviz](callgraphs/world/shots/maintenance.dot)

This file owns periodic update and cleanup operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `ShotModelRuntimeMaintenance_*` (2), `ShotRuntimeMaintenance_*` (2). Representative entry points: `ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint`, `ShotRuntimeMaintenance_OccupancyRebuildNoOp`, `ShotRuntimeMaintenance_UpdateHierarchyProjectedSound`.

**Direct callers:** none from another curated leaf module.

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`audio/spatial/runtime`](#module-audio-spatial-runtime), [`core/math/fixed`](#module-core-math-fixed), [`core/math/interpolation`](#module-core-math-interpolation), [`gameplay/army/combat`](#module-gameplay-army-combat), [`ui/controls/misc`](#module-ui-controls-misc), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/shots/runtime`](#module-world-shots-runtime), [`world/terrain/grid`](#module-world-terrain-grid), [`world/terrain/occupancy`](#module-world-terrain-occupancy).

<a id="module-world-shots-runtime"></a>
### `world/shots/runtime`

[Source](../src/world/shots/runtime.c) · [Header](../include/thandor/world/shots/runtime.h) · [Call graph](callgraphs/world/shots/runtime.md) · [Graphviz](callgraphs/world/shots/runtime.dot)

This file owns runtime state and lifecycle operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `ShotRuntime_*` (6), `ShotRuntimePool_*` (1). Representative entry points: `ShotRuntime_ApplyArmyHitRelationAndNotifications`, `ShotRuntime_InitGraphicsResources`, `ShotRuntime_ShutdownGraphicsResources`.

**Direct callers:** 7 module(s), 12 cross-module call edge(s).

- [`assets/shot/catalog` source](../src/assets/shot/catalog.c) · [graph](callgraphs/assets/shot/catalog.md): 1 caller function(s): [`ShotDefinition_RegisterAndResolveReferencesCf`](../src/assets/shot/catalog.c#L235)
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 3 caller function(s): [`ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf`](../src/gameplay/army/runtime.c#L2600), [`ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate`](../src/gameplay/army/runtime.c#L3591), [`ArmyRuntime_UpdateTimedShotAndEffectEmitters`](../src/gameplay/army/runtime.c#L4169)
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiAction1210_ResourceRegistrationHelper`](../src/ui/ingame/runtime.c#L682)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/model/runtime` source](../src/world/model/runtime.c) · [graph](callgraphs/world/model/runtime.md): 1 caller function(s): [`ModelRuntime_EmitProjectilesFromAttachmentPoints`](../src/world/model/runtime.c#L725)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166)

**Calls into:** [`assets/model/definitions`](#module-assets-model-definitions), [`assets/resource/runtime`](#module-assets-resource-runtime), [`assets/shot/catalog`](#module-assets-shot-catalog), [`core/math/fixed`](#module-core-math-fixed), [`core/text/path`](#module-core-text-path), [`gameplay/army/movement`](#module-gameplay-army-movement), [`gameplay/faction/runtime`](#module-gameplay-faction-runtime), [`graphics/render/shading`](#module-graphics-render-shading), [`movie/runtime/playback`](#module-movie-runtime-playback), [`ui/core/runtime`](#module-ui-core-runtime), [`world/effects/runtime`](#module-world-effects-runtime), [`world/model/hierarchy`](#module-world-model-hierarchy), [`world/model/runtime`](#module-world-model-runtime), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/occupancy`](#module-world-terrain-occupancy).

<a id="module-world-terrain-editing"></a>
### `world/terrain/editing`

[Source](../src/world/terrain/editing.c) · [Header](../include/thandor/world/terrain/editing.h) · [Call graph](callgraphs/world/terrain/editing.md) · [Graphviz](callgraphs/world/terrain/editing.dot)

This file owns terrain editing operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `TerrainEditBuffer_*` (5), `TerrainMaterialEdit_*` (4), `TerrainRegionCollection_*` (2). Representative entry points: `TerrainRegionCollection_CollectConnectedCellsRecursive`, `TerrainMaterialEdit_SeedMatchingRegionReplacement`, `TerrainMaterialEdit_SeedNonTargetRegionReplacement`.

**Direct callers:** 2 module(s), 8 cross-module call edge(s).

- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 1 caller function(s): [`InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState`](../src/gameplay/session/runtime.c#L1826)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 3 caller function(s): [`InGameUiCommand_BeginInteractionByMode`](../src/ui/ingame/runtime.c#L3942), [`InGameUiCommand_EndInteractionByMode`](../src/ui/ingame/runtime.c#L4815), [`InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`](../src/ui/ingame/runtime.c#L5)

**Calls into:** [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-world-terrain-grid"></a>
### `world/terrain/grid`

[Source](../src/world/terrain/grid.c) · [Header](../include/thandor/world/terrain/grid.h) · [Call graph](callgraphs/world/terrain/grid.md) · [Graphviz](callgraphs/world/terrain/grid.dot)

This file owns grid traversal, routing, and cell-state operations within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `FieldGrid_*` (46), `TerrainGrid_*` (6), `FieldGridCell_*` (3). Representative entry points: `FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf`, `TerrainGrid_RunDirectionalRelaxationPasses`, `FieldGrid_ApplyPositiveCellDeltas`.

**Direct callers:** 22 module(s), 112 cross-module call edge(s).

- [`gameplay/army/audio` source](../src/gameplay/army/audio.c) · [graph](callgraphs/gameplay/army/audio.md): 7 caller function(s): [`ArmyRuntimeAudio_UpdateAssetProjectedSound`](../src/gameplay/army/audio.c#L336), [`ArmyRuntimeAudio_UpdateConditionalProjectedSound`](../src/gameplay/army/audio.c#L243), [`ArmyRuntimeAudio_UpdateDualProjectedLoopingSoundsVariantA`](../src/gameplay/army/audio.c#L65); +4 more
- [`gameplay/army/combat` source](../src/gameplay/army/combat.c) · [graph](callgraphs/gameplay/army/combat.md): 1 caller function(s): [`ArmyWeaponRuntime_TestTargetLineOfFireCf`](../src/gameplay/army/combat.c#L443)
- [`gameplay/army/movement` source](../src/gameplay/army/movement.c) · [graph](callgraphs/gameplay/army/movement.md): 11 caller function(s): [`ArmyArticulatedRuntime_InitializeLeftTerrainContact`](../src/gameplay/army/movement.c#L2960), [`ArmyArticulatedRuntime_InitializeRightTerrainContact`](../src/gameplay/army/movement.c#L3023), [`ArmyArticulatedRuntime_InitializeTerrainContactGeometry`](../src/gameplay/army/movement.c#L1550); +8 more
- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 10 caller function(s): [`ArmyPlacementCollision_TestCandidateAndClearanceCf`](../src/gameplay/army/placement.c#L1029), [`ArmyPlacementCollision_TestCurrentRuntimeCf`](../src/gameplay/army/placement.c#L942), [`ArmyPlacementContact_ApplyTerrainHeight`](../src/gameplay/army/placement.c#L360); +7 more
- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 13 caller function(s): [`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`](../src/gameplay/army/runtime.c#L789), [`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`](../src/gameplay/army/runtime.c#L5), [`ArmyRuntimeClass_UpdateGridBoundEffectsAndModels`](../src/gameplay/army/runtime.c#L1184); +10 more
- [`gameplay/faction/runtime` source](../src/gameplay/faction/runtime.c) · [graph](callgraphs/gameplay/faction/runtime.md): 1 caller function(s): [`OldUnitRuntime_MergeMasksAndReplayRecords`](../src/gameplay/faction/runtime.c#L1293)
- [`gameplay/input/world` source](../src/gameplay/input/world.c) · [graph](callgraphs/gameplay/input/world.md): 1 caller function(s): [`InGameTargetingContext_AdvanceOrResolveTarget`](../src/gameplay/input/world.c#L5)
- [`gameplay/selection/overlay` source](../src/gameplay/selection/overlay.c) · [graph](callgraphs/gameplay/selection/overlay.md): 4 caller function(s): [`InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint`](../src/gameplay/selection/overlay.c#L986), [`InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf`](../src/gameplay/selection/overlay.c#L5), [`SelectionOverlay_DrawMarkerACForWorldSurfacePoint`](../src/gameplay/selection/overlay.c#L522); +1 more
- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 1 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 3 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_UpdateCursorGridAndViewScaleCache`](../src/gameplay/session/runtime.c#L2189), [`InGameRuntime_UpdateSimulationAndNetworkTick`](../src/gameplay/session/runtime.c#L2235)
- [`graphics/render/shading` source](../src/graphics/render/shading.c) · [graph](callgraphs/graphics/render/shading.md): 1 caller function(s): [`GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy`](../src/graphics/render/shading.c#L5)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 4 caller function(s): [`InGameCommandRange_DispatchState0`](../src/ui/ingame/commands.c#L1376), [`InGameCommandRange_DispatchState1`](../src/ui/ingame/commands.c#L1401), [`UiCommandModeG_ApplyMaskedColorVariant`](../src/ui/ingame/commands.c#L1560); +1 more
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 4 caller function(s): [`InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState`](../src/ui/ingame/runtime.c#L4927), [`InGameUiCommand_BeginInteractionByMode`](../src/ui/ingame/runtime.c#L3942), [`InGameUiCommand_SaveFieldAndLevelAssetImages`](../src/ui/ingame/runtime.c#L5138); +1 more
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions`](../src/world/effects/maintenance.c#L114)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 6 caller function(s): [`WorldRuntime_AttachFieldGridAsset`](../src/world/runtime/core.c#L631), [`WorldRuntime_InterpolateTerrainHeightOrSentinel`](../src/world/runtime/core.c#L680), [`WorldRuntime_InterpolateTopSurfaceHeightOrSentinel`](../src/world/runtime/core.c#L716); +3 more
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 2 caller function(s): [`ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects`](../src/world/shots/maintenance.c#L166), [`ShotRuntimeMaintenance_UpdateHierarchyProjectedSound`](../src/world/shots/maintenance.c#L126)
- [`world/terrain/editing` source](../src/world/terrain/editing.c) · [graph](callgraphs/world/terrain/editing.md): 1 caller function(s): [`TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting`](../src/world/terrain/editing.c#L170)
- [`world/terrain/height` source](../src/world/terrain/height.c) · [graph](callgraphs/world/terrain/height.md): 2 caller function(s): [`TerrainAuxHeightThreshold_TestAroundWorldPoint`](../src/world/terrain/height.c#L127), [`TerrainHeightBand_TestAroundWorldPoint`](../src/world/terrain/height.c#L5)
- [`world/terrain/occupancy` source](../src/world/terrain/occupancy.c) · [graph](callgraphs/world/terrain/occupancy.md): 2 caller function(s): [`TerrainOccupancyBit2_MarkAroundWorldPoint`](../src/world/terrain/occupancy.c#L5), [`TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint`](../src/world/terrain/occupancy.c#L72)
- [`world/terrain/projection` source](../src/world/terrain/projection.c) · [graph](callgraphs/world/terrain/projection.md): 3 caller function(s): [`FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf`](../src/world/terrain/projection.c#L106), [`FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf`](../src/world/terrain/projection.c#L202), [`TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint`](../src/world/terrain/projection.c#L5)
- [`world/terrain/visuals` source](../src/world/terrain/visuals.c) · [graph](callgraphs/world/terrain/visuals.md): 3 caller function(s): [`TerrainCompositeTexture_RebuildPlane0`](../src/world/terrain/visuals.c#L1112), [`TerrainVisualResources_LoadAndClearCellOverlayFlags`](../src/world/terrain/visuals.c#L400), [`TerrainVisualResources_LoadPrimary`](../src/world/terrain/visuals.c#L228)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`core/math/random`](#module-core-math-random), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`platform/filesystem/win32`](#module-platform-filesystem-win32), [`world/terrain/height`](#module-world-terrain-height).

<a id="module-world-terrain-height"></a>
### `world/terrain/height`

[Source](../src/world/terrain/height.c) · [Header](../include/thandor/world/terrain/height.h) · [Call graph](callgraphs/world/terrain/height.md) · [Graphviz](callgraphs/world/terrain/height.dot)

This file owns terrain-height sampling and interpolation within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `TerrainAuxHeightThreshold_*` (13), `TerrainHeightBand_*` (13), `TerrainHeightDelta_*` (12). Representative entry points: `TerrainHeightBand_TestAroundWorldPoint`, `TerrainAuxHeightThreshold_TestAroundWorldPoint`, `TerrainHeightDelta_ApplyWedge0`.

**Direct callers:** 2 module(s), 14 cross-module call edge(s).

- [`gameplay/army/placement` source](../src/gameplay/army/placement.c) · [graph](callgraphs/gameplay/army/placement.md): 1 caller function(s): [`ArmyPlacement_TestModelTerrainAndRuntimeClearance`](../src/gameplay/army/placement.c#L69)
- [`world/terrain/grid` source](../src/world/terrain/grid.c) · [graph](callgraphs/world/terrain/grid.md): 4 caller function(s): [`FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf`](../src/world/terrain/grid.c#L750), [`FieldGrid_RaycastSecondarySurfaceDistanceCf`](../src/world/terrain/grid.c#L2260), [`FieldGrid_RaycastTerrainSurfaceDistanceCf`](../src/world/terrain/grid.c#L2171); +1 more

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-world-terrain-occupancy"></a>
### `world/terrain/occupancy`

[Source](../src/world/terrain/occupancy.c) · [Header](../include/thandor/world/terrain/occupancy.h) · [Call graph](callgraphs/world/terrain/occupancy.md) · [Graphviz](callgraphs/world/terrain/occupancy.dot)

This file owns terrain/world occupancy masks within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `TerrainOccupancyBit2_*` (13), `TerrainOccupancyMask_*` (2). Representative entry points: `TerrainOccupancyBit2_MarkAroundWorldPoint`, `TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint`, `TerrainOccupancyMask_ResolveRuntimeClassFlags`.

**Direct callers:** 5 module(s), 11 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 2 caller function(s): [`ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback`](../src/gameplay/army/runtime.c#L1875), [`ArmyRuntime_InitializeTerrainOccupancyFlags`](../src/gameplay/army/runtime.c#L4044)
- [`world/effects/maintenance` source](../src/world/effects/maintenance.c) · [graph](callgraphs/world/effects/maintenance.md): 1 caller function(s): [`EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint`](../src/world/effects/maintenance.c#L5)
- [`world/effects/runtime` source](../src/world/effects/runtime.c) · [graph](callgraphs/world/effects/runtime.md): 1 caller function(s): [`EffectRuntimePool_CreateInstanceFromDefinitionCf`](../src/world/effects/runtime.c#L184)
- [`world/shots/maintenance` source](../src/world/shots/maintenance.c) · [graph](callgraphs/world/shots/maintenance.md): 1 caller function(s): [`ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint`](../src/world/shots/maintenance.c#L5)
- [`world/shots/runtime` source](../src/world/shots/runtime.c) · [graph](callgraphs/world/shots/runtime.md): 1 caller function(s): [`ShotRuntimePool_CreateProjectileFromDefinition`](../src/world/shots/runtime.c#L276)

**Calls into:** [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-world-terrain-projection"></a>
### `world/terrain/projection`

[Source](../src/world/terrain/projection.c) · [Header](../include/thandor/world/terrain/projection.h) · [Call graph](callgraphs/world/terrain/projection.md) · [Graphviz](callgraphs/world/terrain/projection.dot)

This file owns projection and projected geometry within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `FieldGridTerrainOverlayVariantA_*` (13), `FieldGridTerrainOverlayVariantB_*` (13), `TerrainProjectedOcclusion_*` (13). Representative entry points: `TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint`, `FieldGridTerrainOverlayVariantA_ApplyAroundWorldPointCf`, `FieldGridTerrainOverlayVariantB_ApplyAroundWorldPointCf`.

**Direct callers:** 2 module(s), 2 cross-module call edge(s).

- [`gameplay/army/runtime` source](../src/gameplay/army/runtime.c) · [graph](callgraphs/gameplay/army/runtime.md): 1 caller function(s): [`ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback`](../src/gameplay/army/runtime.c#L1875)
- [`ui/frontend/runtime` source](../src/ui/frontend/runtime.c) · [graph](callgraphs/ui/frontend/runtime.md): 1 caller function(s): [`FrontendModelPointerContext_DrawClipped`](../src/ui/frontend/runtime.c#L806)

**Calls into:** [`core/math/fixed`](#module-core-math-fixed), [`core/math/geometry`](#module-core-math-geometry), [`graphics/core/runtime`](#module-graphics-core-runtime), [`graphics/render/primitives`](#module-graphics-render-primitives), [`graphics/render/shading`](#module-graphics-render-shading), [`world/terrain/grid`](#module-world-terrain-grid).

<a id="module-world-terrain-visuals"></a>
### `world/terrain/visuals`

[Source](../src/world/terrain/visuals.c) · [Header](../include/thandor/world/terrain/visuals.h) · [Call graph](callgraphs/world/terrain/visuals.md) · [Graphviz](callgraphs/world/terrain/visuals.dot)

This file owns terrain visual resources and animated visual state within world simulation, terrain, pathing, models, effects, and projectiles. Dominant function families: `TerrainCompositeTexture_*` (5), `TerrainVisualResources_*` (3), `TerrainLighting_*` (2). Representative entry points: `TerrainCompositeTexture_Create`, `TerrainByteClampLookup_Initialize`, `TerrainVisualResources_LoadPrimary`.

**Direct callers:** 6 module(s), 13 cross-module call edge(s).

- [`gameplay/session/level` source](../src/gameplay/session/level.c) · [graph](callgraphs/gameplay/session/level.md): 3 caller function(s): [`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`](../src/gameplay/session/level.c#L42), [`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`](../src/gameplay/session/level.c#L917), [`InGameLevelRuntime_ShutdownLoadedAssetResources`](../src/gameplay/session/level.c#L1539)
- [`gameplay/session/runtime` source](../src/gameplay/session/runtime.c) · [graph](callgraphs/gameplay/session/runtime.md): 4 caller function(s): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../src/gameplay/session/runtime.c#L87), [`InGameRuntime_InitializeLoadedSession`](../src/gameplay/session/runtime.c#L1085), [`InGameRuntime_InitializeNewSession`](../src/gameplay/session/runtime.c#L718); +1 more
- [`platform/bootstrap/runtime` source](../src/platform/bootstrap/runtime.c) · [graph](callgraphs/platform/bootstrap/runtime.md): 1 caller function(s): [`GameRuntime_InitializeSpatialAudioAndRenderingCf`](../src/platform/bootstrap/runtime.c#L561)
- [`ui/ingame/commands` source](../src/ui/ingame/commands.c) · [graph](callgraphs/ui/ingame/commands.md): 2 caller function(s): [`UiCommandModeG_ApplyMaskedColorVariant`](../src/ui/ingame/commands.c#L1560), [`UiCommandModeG_ApplyRawColorVariant`](../src/ui/ingame/commands.c#L1804)
- [`ui/ingame/runtime` source](../src/ui/ingame/runtime.c) · [graph](callgraphs/ui/ingame/runtime.md): 1 caller function(s): [`InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf`](../src/ui/ingame/runtime.c#L5)
- [`world/runtime/core` source](../src/world/runtime/core.c) · [graph](callgraphs/world/runtime/core.md): 1 caller function(s): [`WorldRuntime_SetTerrainLightingConfiguration`](../src/world/runtime/core.c#L1479)

**Calls into:** [`assets/package/runtime`](#module-assets-package-runtime), [`assets/resource/runtime`](#module-assets-resource-runtime), [`core/math/random`](#module-core-math-random), [`core/text/path`](#module-core-text-path), [`gameplay/selection/runtime`](#module-gameplay-selection-runtime), [`movie/runtime/playback`](#module-movie-runtime-playback), [`world/runtime/core`](#module-world-runtime-core), [`world/terrain/grid`](#module-world-terrain-grid).
