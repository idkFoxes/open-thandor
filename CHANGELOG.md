# Developer changelog: public V523 → V537

This is the compact developer-facing view of the V523 → V537 recovery. It keeps only changes with practical code/game-system value; generic ABI cleanup and decompiler-noise cleanup are not repeated per function. The current source checkpoint is the V537 Ghidra project, with the matching raw V537 decompiler export. See the [full recovery changelog](CHANGELOG_FULL.md) for every submodule, validation detail, and the complete decompiler-recovery record.

## At a glance

- **126 actionable developer notes across 42 submodules.**
- Generic semantic signature/type refinements folded out of per-function notes: **473 owners**.
- Tracked decompiler artifacts across all split modules: **33,291 → 20,290 (60.9% remaining)**. Module entries below use this same aggregate measure and do not split it by artifact class.
- The most consequential recovery is listed first: gameplay/simulation, then level/world state, rendering/media, UI/network behavior, and finally infrastructure/naming work.

## Navigation

[README](README.md) · V537 GZF · V537 raw C · [Generated types](include/thandor/generated/types.h) · [Full recovery changelog](CHANGELOG_FULL.md) · [Module tree](docs/MODULE_TREE.md) · [Source file guide](docs/SOURCE_FILE_GUIDE.md)

## Gameplay mechanics & simulation

<a id="module-gameplay-army-runtime"></a>
### `gameplay/army/runtime`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-gameplay-army-runtime)

**Decompiler artifacts:** **501 → 99 (19.8% remaining)**.

- `0x0051D350` **[`ArmyRuntimeNode_DispatchTypedCallback`](src/gameplay/army/class_dispatch.cpp#L154)** — typed callback dispatch contract is propagated into the army runtime node dispatcher.
- `0x0051D960` **`ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs`** — save-time pointer-to-offset conversion uses recovered pool/link field types.
- `0x00520CF0` **`ArmyRuntimeClass_UpdatePositionedSoundsVariantA`** — positioned-sound variant A now uses its class-specific runtime view; both sound slots, gain, maximum distance, link state and positioned-sound class state are explicit.
- `0x00523E70` **[`ArmyRuntimeClass_SelectProjectileTargetNode`](src/gameplay/army/weapons.cpp#L199)** — renamed from `ArmyRuntimeClass_SelectProjectileTargetNodeCf`; target selection now uses the timed-target projectile view; selected target model, matching active shot, shot-definition reference, mode-2 range and adjacent link state are explicit.
- `0x00523FC0` **[`ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects`](src/gameplay/army/weapons.cpp#L251)** — timed-target firing now uses typed target/link state, reload countdown/interval, selected target model, matching shot runtime and shot-definition state instead of generic class-link fields.
- `0x00524740` **`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`** — class-update slot 13 dispatch uses recovered model/class views instead of opaque dispatch state.
- `0x00525020` **`ArmyRuntimeClass_UpdateResourceExtractionGridAndEffects`** — the old `UpdateGridBoundEffectsAndModels` label was too generic. Class-14 behavior is resource extraction, so the owner is now `ArmyRuntimeClass_UpdateResourceExtractionGridAndEffects` with typed extraction descriptor and weight.
- `0x00525960` **`ArmyRuntimeClass_UpdatePositionedSoundsVariantB`** — positioned-sound variant B now uses its class-specific runtime view; both sound slots, gain, maximum distance, link state and positioned-sound class state are explicit.
- `0x00525A60` **`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`** — class-update slot 21 dispatch uses recovered model/class views instead of opaque dispatch state; class-21 dispatch now uses `ModelRuntimeClass21UpdateView200`; trajectory reference height, phase/travel values, vertical arc coefficient, completion effect, movement step and placement-contact kind replace anonymous class-state slots.
- `0x00526620` **[`ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`](src/gameplay/army/aircraft.cpp#L516)** — linked-model class dispatch uses recovered class/link state rather than anonymous runtime slots; linked-child spawn/build state is recovered as a dedicated view: selected asset/build counters, completed asset IDs, inherited spawn triplets, pending spawn counts, runtime flags, slot capacity, translation limits/step and transition sounds are named.
- `0x00527C00` **[`ArmyRuntime_UpdateTimedShotAndEffectEmitters`](src/gameplay/army/weapons.cpp#L614)** — timed shot/effect emitter state and definition references are propagated through the update loop.
- `0x00528330` **[`ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy`](src/gameplay/army/class_updates.cpp#L193)** — destroy/effects update now uses a dedicated runtime/definition overlay and names the per-tick vertical translation step.
- `0x005283D0` **[`ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState`](src/gameplay/army/class_updates.cpp#L307)** — vertical-deployment/collision state is represented by recovered class-state fields instead of anonymous offsets; vertical deployment now uses dedicated definition/runtime views; travel accumulator/limit, per-tick vertical step, collision retry countdown and deployment sound are named.

<a id="module-gameplay-army-movement"></a>
### `gameplay/army/movement`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-gameplay-army-movement)

**Decompiler artifacts:** **367 → 12 (3.3% remaining)**.

- `0x00520140` **[`ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation`](src/gameplay/army/drive_ground.cpp#L352)** — uses ground-movement track/definition views; opaque offsets are replaced by water-damage threshold/multiplier, turn rate/acceleration, movement advance, heading-error limits, track texture scale, and placement-contact kind.
- `0x00520840` **`ArmyRuntimeClass_UpdateGroundMovementVariantA`** — uses the ground-movement steering view; water damage, movement/turn control, heading limits, and placement-contact dispatch fields replace reserved byte-offset accesses.
- `0x00520DF0` **[`ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement`](src/gameplay/army/drive_ground.cpp#L20)** — switched the ground-movement path to `ModelRuntimeGroundMovementSteeringView200`, exposing the steering state used by the callee chain.
- `0x00520F60` **[`ArmyRuntimeClass_UpdateArticulatedMovement`](src/gameplay/army/walker.cpp#L334)** — articulated movement now uses the recovered movement-definition view and names the per-tick movement delta plus water-damage threshold/multiplier fields.
- `0x005217A0` **[`ArmyArticulatedRuntime_UpdateSuspensionHierarchy`](src/gameplay/army/walker.cpp#L973)** — vector-angle, vector-length, and Euler composite return objects are kept distinct through suspension calculations, fixing the old temporary-type cross-assignment without changing the math.
- `0x00522C00` **[`ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation`](src/gameplay/army/drive_banking.cpp#L379)** — uses the ground-movement steering view and names movement advance, turn velocity, turn rate/acceleration, heading interpolation distance, movement step, and placement-contact kind.
- `0x00523410` **`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA`** — weapon-aim state is explicit: yaw/pitch turn velocity, attachment reload countdowns, shot definition, launch vectors, and attachment model-point source replace opaque runtime offsets.
- `0x00523690` **`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB`** — weapon-aim state is explicit for the alternate firing path, including alternating attachment sequence, per-attachment reload state, yaw/pitch velocities, shot definition, and launch vectors.
- `0x005254F0` **`ArmyRuntimeClass_UpdateGroundMovementVariantB`** — uses the ground-movement steering view; movement/turn control, heading limits, and placement-contact dispatch fields replace reserved byte-offset accesses.

<a id="module-gameplay-army-placement"></a>
### `gameplay/army/placement`

[source](src/gameplay/army/placement.cpp) · [header](include/thandor/gameplay/army/placement.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-army-placement) · [full changes](CHANGELOG_FULL.md#module-gameplay-army-placement)

**Decompiler artifacts:** **151 → 4 (2.6% remaining)**.

- `0x005244B0` **`ArmyPlacementCandidate_TestOffsetClearanceCf`** — placement dispatch now reads the recovered 32-bit `placementContactKindIndex278` instead of the old generic callback-selector field.
- `0x00524570` **[`ArmyPlacement_TestModelTerrainAndRuntimeClearance`](src/gameplay/army/placement.cpp#L92)** — terrain/runtime clearance now uses the recovered placement-contact kind field rather than a generic placement callback selector.
- `0x00525320` **`ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacityAndClearExtractionGridCell`** — the old name implied generic faction-capacity/placement ownership. Recovered class-14 semantics show this is `ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacityAndClearExtractionGridCell`; the body now uses the storage-capacity contribution, resource-field support selector, and extraction descriptor.
- `0x00525420` **`ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacity`** — the old generic `ArmyPlacement_ReleaseFactionCapacity` name was wrong. The class-15 contract is resource storage, so the owner is now `ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacity` with explicit storage selector/capacity fields.
- `0x00527740` **`ArmyPlacementCollision_TestCurrentRuntimeCf`** — current-runtime placement dispatch uses the recovered `placementContactKindIndex278` field.
- `0x005278D0` **`ArmyPlacementCollision_TestCandidateAndClearanceCf`** — candidate placement selects the terrain/contact callback through typed `ArmyPlacementContactKindIndex32` storage instead of a generic dword selector.
- `0x00529CB0` **`ArmyCollision_TestPointAgainstRuntimeListCf`** — collision broad-phase accesses now use named near/far model depth-bin masks instead of opaque world-owner fields.
- `0x00529D70` **`ArmyPlacementCollision_TestPointAgainstRuntimeListCf`** — placement collision broad-phase accesses now use named near/far model depth-bin masks.
- `0x00529F30` **`ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf`** — candidate collision broad-phase accesses now use named near/far model depth-bin masks.

<a id="module-world-motion-runtime"></a>
### `world/motion/runtime`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-world-motion-runtime)

**Decompiler artifacts:** **59 → 0 (0.0% remaining)**.

- `0x0050C920` **[`WorldMotion_AdjustHeadingAndRecomputePosition`](src/world/camera/camera_input.cpp#L179)** — recovered world-motion constants/globals are used directly by heading/position adjustment.
- `0x0050C990` **[`WorldMotion_AdjustHeadingAndClearFieldGridDirty`](src/world/camera/camera_input.cpp#L203)** — recovered world-motion constants/globals and field-grid dirty-state semantics replace opaque scalar accesses.
- `0x0050C9C0` **[`WorldMotion_AdjustDistanceClampAndRecomputePosition`](src/world/camera/camera_input.cpp#L219)** — distance-clamp motion uses recovered world-motion constants and typed position state.
- `0x0050CA80` **[`WorldMotion_AdjustPositionMagnitudeClamp`](src/world/camera/camera_input.cpp#L264)** — magnitude-clamp motion uses recovered world-motion constants/state.
- `0x0050CB10` **[`WorldMotion_AdjustPitchClampAndRecomputePosition`](src/world/camera/camera_input.cpp#L303)** — pitch-clamp/recompute uses recovered world-motion constants/state.
- `0x0050CBE0` **[`WorldMotion_AdjustPitchClampAndClearFieldGridDirty`](src/world/camera/camera_input.cpp#L350)** — pitch-clamp/dirty-state path uses recovered world-motion constants and field-grid state.

<a id="module-world-model-hierarchy"></a>
### `world/model/hierarchy`

[source](src/world/model/hierarchy.cpp) · [header](include/thandor/world/model/hierarchy.h) · [call graph](docs/source_guide/world.md#file-world-model-hierarchy) · [full changes](CHANGELOG_FULL.md#module-world-model-hierarchy)

**Decompiler artifacts:** **220 → 29 (13.2% remaining)**.

- `0x004BEB80` **`ModelNodeRuntime_TransformLocalPointRegs`** — local-point transformation uses recovered composite point/result types.
- `0x004BEBC0` **[`ModelNodeRuntime_ComputeRelativeDirectionAngle`](src/world/model/hierarchy.cpp#L314)** — the old `uint` return exposed only one angle; V537 recovers the two-register `ModelRelativeDirectionAnglesEaxEdx8` result so both relative-direction angles survive the call boundary.
- `0x0052AAC0` **[`ModelNodeRuntime_SmoothYawTowardTarget`](src/world/model/gameplay_rules.cpp#L228)** — yaw smoothing now uses `ModelRuntimeWeaponAimStateView200`; yaw velocity plus rate/acceleration limits are named and typed.
- `0x0052AC00` **[`ModelNodeRuntime_SmoothPitchTowardTarget`](src/world/model/gameplay_rules.cpp#L313)** — pitch smoothing now uses `ModelRuntimeWeaponAimStateView200`; pitch velocity, min/max pitch and rate/acceleration limits are named and typed.

<a id="module-world-pathing-grid"></a>
### `world/pathing/grid`

source · [header](include/thandor/world/pathing/grid.h) · [call graph](docs/source_guide/world.md#file-world-pathing-grid) · [full changes](CHANGELOG_FULL.md#module-world-pathing-grid)

**Decompiler artifacts:** **234 → 6 (2.6% remaining)**.

- `0x00534960` **[`GridPathRegion_MarkUnreachableFromCell`](src/world/pathing/reachability.cpp#L142)** — unreachable-region marking propagates recovered grid/path record types.
- `0x005349D0` **[`EntityPathing_ResolveDestinationAndRebuildRoutes`](src/world/pathing/route.cpp#L53)** — route rebuilding uses recovered destination/backtrack result contracts rather than scalar register fragments.
- `0x005369A0` **[`EntityPathing_UpdateRouteSegment`](src/world/pathing/route.cpp#L553)** — route-segment update propagates recovered route/grid state types.
- `0x00536C90` **[`EntityPathing_RebuildOverlappingGroupRoutes`](src/world/pathing/route.cpp#L272)** — overlapping-group route rebuilding now uses named near/far model depth-bin masks in world-owner filtering.

<a id="module-gameplay-army-combat"></a>
### `gameplay/army/combat`

[source](src/gameplay/army/combat.cpp) · [header](include/thandor/gameplay/army/combat.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-army-combat) · [full changes](CHANGELOG_FULL.md#module-gameplay-army-combat)

**Decompiler artifacts:** **105 → 0 (0.0% remaining)**.

- `0x00523980` **[`ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`](src/gameplay/army/combat.cpp#L97)** — weapon targeting now consumes `ModelRuntimeWeaponAimStateView200`; relative yaw/pitch and their turn velocities are explicit rather than packed into generic class state.
- `0x00525130` **`ArmyRuntimeClass_UpdateResourceStorageFillAndDamageEffect`** — the old `UpdateTransformAndDamageEffect` name hid class-15 behavior. It is now `ArmyRuntimeClass_UpdateResourceStorageFillAndDamageEffect`, with storage selector and empty/full child-Z endpoints typed explicitly.
- `0x00527AC0` **[`ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage`](src/gameplay/army/damage.cpp#L55)** — timed-effects update uses a dedicated runtime/definition view and names the definition gate that requires runtime state before emit/update work proceeds.

<a id="module-world-model-slots"></a>
### `world/model/slots`

[source](src/world/model/slots.cpp) · [header](include/thandor/world/model/slots.h) · [call graph](docs/source_guide/world.md#file-world-model-slots) · [full changes](CHANGELOG_FULL.md#module-world-model-slots)

**Decompiler artifacts:** **11 → 1 (9.1% remaining)**.

- `0x005251C0` **`ModelRuntimeSlotClassInit_AddFactionResourceStorageCapacityAndDetachRootChild3`** — the generic faction-metric name obscured class-14 resource semantics. The owner now adds the recovered resource-storage-capacity contribution and handles the resource-field support selector before detaching root child 3.
- `0x00525250` **`ModelRuntimeSlotClassInit_AddFactionResourceStorageCapacityAndDetachRootChild1`** — the generic faction-metric name obscured class-15 resource semantics. The owner now explicitly adds resource-storage capacity using the recovered storage selector before detaching root child 1.

<a id="module-gameplay-ai-technology"></a>
### `gameplay/ai/technology`

[source](src/gameplay/ai/technology.cpp) · [header](include/thandor/gameplay/ai/technology.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-technology) · [full changes](CHANGELOG_FULL.md#module-gameplay-ai-technology)

**Decompiler artifacts:** **21 → 0 (0.0% remaining)**.

- `0x0053BC20` **[`AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8`](src/gameplay/ai/technology.cpp#L316)** — AI technology relation scaling uses recovered relation/scale types instead of generic scalar locals.

<a id="module-gameplay-ai-workspaces"></a>
### `gameplay/ai/workspaces`

[source](src/gameplay/ai/workspaces.cpp) · [header](include/thandor/gameplay/ai/workspaces.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-workspaces) · [full changes](CHANGELOG_FULL.md#module-gameplay-ai-workspaces)

**Decompiler artifacts:** **120 → 0 (0.0% remaining)**.

- `0x00538230` **[`AiPlanning_RebuildFactionWorkspaces`](src/gameplay/ai/perception.cpp#L455)** — AI workspace rebuilding now treats the field-grid resource cell value as `ResourceExtractionDescriptor32`, fixing the old generic dword interpretation.

<a id="module-gameplay-faction-relations"></a>
### `gameplay/faction/relations`

[source](src/gameplay/faction/relations.cpp) · [header](include/thandor/gameplay/faction/relations.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-faction-relations) · [full changes](CHANGELOG_FULL.md#module-gameplay-faction-relations)

**Decompiler artifacts:** **80 → 0 (0.0% remaining)**.

- `0x0053C0F0` **`GameFactionRelations_EvaluateTransitionRulesCf`** — end-condition rules now read the typed level condition schedule and trigger records through `g_InGameLevelRuntimeGlobalBlock` instead of the misclassified condition-runtime blob.

<a id="module-gameplay-faction-runtime"></a>
### `gameplay/faction/runtime`

[source](src/gameplay/faction/runtime.cpp) · [header](include/thandor/gameplay/faction/runtime.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-faction-runtime) · [full changes](CHANGELOG_FULL.md#module-gameplay-faction-runtime)

**Decompiler artifacts:** **147 → 22 (15.0% remaining)**.

- `0x00565320` **[`OldUnitRuntime_RebuildScenarioReplayTables`](src/gameplay/faction/carryover.cpp#L27)** — replay-table rebuild now names the saved model local-rotation-angle field instead of treating it as an anonymous scalar.


## Level, world state & resource systems

<a id="module-gameplay-session-level"></a>
### `gameplay/session/level`

[source](src/gameplay/session/level.cpp) · [header](include/thandor/gameplay/session/level.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-session-level) · [full changes](CHANGELOG_FULL.md#module-gameplay-session-level)

**Decompiler artifacts:** **243 → 4 (1.6% remaining)**.

- `0x005311D0` **`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`** — level loading now uses the recovered global level block, condition storage, player-slot offset array, relation masks, sample tables and initial-army placement count instead of indexing an opaque condition-runtime allocation.
- `0x00532020` **`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`** — external-table level loading now uses typed level storage/player-slot/sample fields and calls the correctly identified resource-registration rebase owner.
- `0x005329C0` **[`InGameLevelRuntime_ShutdownLoadedAssetResources`](src/gameplay/session/level.cpp#L1216)** — shutdown now releases the typed level condition-storage allocation through the recovered level-runtime global block.
- `0x00532CA0` **`InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf`** — level save now uses the recovered model local-rotation-angle field in the serialized world-state path.

<a id="module-gameplay-session-runtime"></a>
### `gameplay/session/runtime`

[source](src/gameplay/session/runtime.cpp) · [header](include/thandor/gameplay/session/runtime.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-session-runtime) · [full changes](CHANGELOG_FULL.md#module-gameplay-session-runtime)

**Decompiler artifacts:** **148 → 9 (6.1% remaining)**.

- `0x0050E120` **[`InGameConditionRuntime_UpdateScheduledRecords`](src/gameplay/session/runtime.cpp#L6148)** — the 64 scheduled conditions and 16 trigger records now use typed kind/status/payload records; raw `+0x10` byte stepping and numeric condition kinds are replaced by the recovered schedule view/enums.
- `0x0050EA90` **[`ResourceRegistrationRuntime_RebaseLoadedRecords`](src/gameplay/session/runtime.cpp#L4416)** — this owner was misclassified as `InGameConditionRuntime_RebaseLoadedRecords`. V537 identifies it as `ResourceRegistrationRuntime_RebaseLoadedRecords` and types the resource-registration pointer/offset domains it rebases.
- `0x00513160` **[`InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState`](src/gameplay/session/runtime.cpp#L6557)** — resource/energy update now reaches the level header through typed level condition storage instead of indexing `g_InGameConditionRuntime` as an unrelated record array.
- `0x00566290` **`EndGameResultsUiRuntime_UpdateAndHandleInputCf`** — end-game result handling now reads typed schedule conditions and level runtime-tail/music fields from the recovered level storage rather than anonymous condition-runtime offsets.

<a id="module-world-runtime-core"></a>
### `world/runtime/core`

[source](src/world/runtime/core.cpp) · [header](include/thandor/world/runtime/core.h) · [call graph](docs/source_guide/world.md#file-world-runtime-core) · [full changes](CHANGELOG_FULL.md#module-world-runtime-core)

**Decompiler artifacts:** **292 → 206 (70.5% remaining)**.

- `0x004BE760` **`WorldRuntime_InterpolateTerrainHeightOrSentinel`** — terrain-height interpolation uses the recovered world/field-grid semantic types.
- `0x004BE790` **`WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel`** — water-surface interpolation uses the recovered world/field-grid semantic types.
- `0x004BE7C0` **[`WorldRuntime_InterpolateTopSurfaceHeightOrSentinel`](src/world/runtime/core.cpp#L41)** — top-surface interpolation uses the recovered world/field-grid semantic types.
- `0x00532FA0` **[`WorldLightingRuntime_UpdateInterpolatedTerrainLighting`](src/world/terrain/field_lighting.cpp#L396)** — terrain-light interpolation now reads typed level-runtime-tail fields for cycle duration, base/ramp colors, alternate colors and lighting key colors instead of anonymous condition-runtime offsets.

<a id="module-assets-resource-runtime"></a>
### `assets/resource/runtime`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-assets-resource-runtime)

**Decompiler artifacts:** **38 → 9 (23.7% remaining)**.

- `0x00532B00` **`ResourceRegistration_ResolveRuntimeRecord`** — resource-registration lookup now uses the recovered level block/player-slot offsets and named start-camera fields rather than condition-runtime aliases.


## Rendering, terrain, effects & media

<a id="module-graphics-render-shading"></a>
### `graphics/render/shading`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-graphics-render-shading)

**Decompiler artifacts:** **1,016 → 156 (15.4% remaining)**.

- `0x004CD200` **[`GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes`](src/graphics/render/shadow_texture.cpp#L722)** — generated-texture scratch/global state names were propagated into reset/clear logic.
- `0x004CD2B0` **[`GraphicsShadingGeneratedTexture_AdvanceTileCursor`](src/graphics/render/shadow_texture.cpp#L1118)** — generated-texture tile cursor state is represented through named scratch/global state.
- `0x004CD360` **[`GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources`](src/graphics/render/shadow_texture.cpp#L766)** — touched-alpha subresource state is propagated through the refresh path.
- `0x004CD3D0` **[`GraphicsShadingGeneratedTexture_FilterGridScratchMmx`](src/graphics/render/shadow_texture.cpp#L1198)** — generated-texture filter scratch state is typed/named while retaining the MMX operation sequence.
- `0x004CD690` **[`GraphicsShadingGeneratedTexture_RasterizeTriangleMask`](src/graphics/render/shadow_texture.cpp#L1493)** — rasterizer mask inputs and generated-texture scratch state are typed instead of generic parameters.
- `0x004CDD40` **[`GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy`](src/graphics/render/shadow_texture.cpp#L517)** — hierarchy processing replaces hundreds of generic parameters/temporaries with the recovered generated-texture/runtime views.

<a id="module-graphics-backend-glide"></a>
### `graphics/backend/glide`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-graphics-backend-glide)

**Decompiler artifacts:** **979 → 866 (88.5% remaining)**.

- `0x00580FF0` **`GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444`** — identity/body: admitted the alpha downsample owner and recovered source/destination image/subresource parameter roles.
- `0x005810A0` **`GraphicsGlide3_DownsampleAlternateAlphaSamplesToWhiteArgb4444`** — identity/body: admitted the alternate-alpha downsample owner and recovered source/destination image/subresource parameter roles.

<a id="module-graphics-render-primitives"></a>
### `graphics/render/primitives`

[source](src/graphics/render/primitives.cpp) · [header](include/thandor/graphics/render/primitives.h) · [call graph](docs/source_guide/graphics.md#file-graphics-render-primitives) · [full changes](CHANGELOG_FULL.md#module-graphics-render-primitives)

**Decompiler artifacts:** **91 → 9 (9.9% remaining)**.

- `0x004D0A10` **[`GraphicsPrimitiveQueue_AllocateGlobalPool`](src/graphics/render/primitives.cpp#L153)** — primitive-queue allocation uses the recovered queue capacity/global contract.
- `0x004D0DA0` **`GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangleCf`** — renamed from `GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle`

<a id="module-world-terrain-projection"></a>
### `world/terrain/projection`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-world-terrain-projection)

**Decompiler artifacts:** **794 → 223 (28.1% remaining)**.

- `0x00500D30` **[`TerrainProjectedGrid_ClipRowSpansAgainstPlane`](src/graphics/terrain/terrain_render.cpp#L659)** — the projected-grid scratch area is recovered as 260 typed row-span records instead of a flat opaque buffer.
- `0x00500F50` **[`TerrainProjectedGrid_TransformShadeAndQueue`](src/graphics/terrain/terrain_render.cpp#L89)** — transform/shade/queue consumes the same typed 260-row projected span workspace.

<a id="module-gameplay-army-audio"></a>
### `gameplay/army/audio`

[source](src/gameplay/army/audio.cpp) · [header](include/thandor/gameplay/army/audio.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-army-audio) · [full changes](CHANGELOG_FULL.md#module-gameplay-army-audio)

**Decompiler artifacts:** **29 → 0 (0.0% remaining)**.

- `0x00520E60` **[`ArmyRuntimeAudio_DispatchPositionedSoundVariant`](src/gameplay/army/audio.cpp#L76)** — positioned-sound dispatch now passes the class-specific `ArmyRuntimeGroundMovementPositionedSoundView120` to both variant handlers instead of an untyped `ArmyRuntimeSlot` view.

<a id="module-graphics-render-model"></a>
### `graphics/render/model`

source · [header](include/thandor/graphics/render/model.h) · [call graph](docs/source_guide/graphics.md#file-graphics-render-model) · [full changes](CHANGELOG_FULL.md#module-graphics-render-model)

**Decompiler artifacts:** **523 → 205 (39.2% remaining)**.

- `0x004CC940` **[`ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath`](src/graphics/render/model_lighting.cpp#L1397)** — nearby-light alternate path uses recovered render/light parameter roles instead of generic scalar arguments.

<a id="module-graphics-resources-texture"></a>
### `graphics/resources/texture`

source · [header](include/thandor/graphics/resources/texture.h) · [call graph](docs/source_guide/graphics.md#file-graphics-resources-texture) · [full changes](CHANGELOG_FULL.md#module-graphics-resources-texture)

**Decompiler artifacts:** **294 → 239 (81.3% remaining)**.

- `0x004AC8E0` **`GraphicsTextureSource_DecomposeSubresourceRegionsCf`** — identity/body: admitted the texture-subresource decomposition owner and typed its region/image contract.

<a id="module-movie-runtime-playback"></a>
### `movie/runtime/playback`

[source](src/movie/runtime/playback.cpp) · [header](include/thandor/movie/runtime/playback.h) · [call graph](docs/source_guide/movie.md#file-movie-runtime-playback) · [full changes](CHANGELOG_FULL.md#module-movie-runtime-playback)

**Decompiler artifacts:** **594 → 360 (60.6% remaining)**.

- `0x004A7770` **[`Movie_EncodeFrame4x4Delta`](src/movie/runtime/flm_encoder.cpp#L381)** — frame-delta encoder parameters and packed block/state values were propagated through the encoder body.

<a id="module-world-terrain-editing"></a>
### `world/terrain/editing`

[source](src/world/terrain/editing.cpp) · [header](include/thandor/world/terrain/editing.h) · [call graph](docs/source_guide/world.md#file-world-terrain-editing) · [full changes](CHANGELOG_FULL.md#module-world-terrain-editing)

**Decompiler artifacts:** **80 → 0 (0.0% remaining)**.

- `0x00513790` **[`TerrainRegionCollection_RecordConnectedCell`](src/world/terrain/editing.cpp#L363)** — connected-cell recording now names the field-grid resource-extraction descriptor instead of generic `runtime7C` storage.

<a id="module-world-terrain-visuals"></a>
### `world/terrain/visuals`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-world-terrain-visuals)

**Decompiler artifacts:** **165 → 95 (57.6% remaining)**.

- `0x0053D840` **[`TerrainCompositeTexture_RebuildPlane0`](src/graphics/terrain/minimap_composite.cpp#L353)** — terrain composite rebuilding now reads the named model tint ARGB field instead of an anonymous world-owner scalar.


## Network, frontend & UI behavior

<a id="module-network-backend-fallback-udp"></a>
### `network/backend/fallback_udp`

[source](src/network/backend/fallback_udp.cpp) · [header](include/thandor/network/backend/fallback_udp.h) · [call graph](docs/source_guide/network.md#file-network-backend-fallback-udp) · [full changes](CHANGELOG_FULL.md#module-network-backend-fallback-udp)

**Decompiler artifacts:** **60 → 4 (6.7% remaining)**.

- `0x00584E80` **`NetworkFallback_OpenAndBindUdpSocketCf`** — network endpoint/address-family/socket state is typed instead of raw packed dwords.
- `0x00585030` **[`NetworkFallback_CloseActiveSocket`](src/network/backend/fallback_udp.cpp#L254)** — active fallback socket state is represented by the recovered socket contract.
- `0x00585060` **`NetworkFallback_ReceiveDatagramCf`** — receive path uses typed endpoint/socket address storage.
- `0x005850B0` **`NetworkFallback_SendDatagramCf`** — send path uses typed endpoint/socket address storage.
- `0x005852A0` **`NetworkBackend_OpenAndBindActiveSocketCf`** — backend socket creation/bind uses recovered endpoint, address-family and packed socket-address types.
- `0x00585450` **`NetworkFallbackUdp_CloseSocket`** — fallback UDP close uses the recovered socket handle/type contract.
- `0x00585480` **`NetworkFallbackUdp_ReceiveDatagram`** — fallback UDP receive uses typed endpoint/socket-address storage.
- `0x005854E0` **`NetworkFallbackUdp_SendDatagram`** — fallback UDP send uses typed endpoint/socket-address storage.
- `0x00585550` **`NetworkBackend_ParseEndpointTextCf`** — identity/body: recovered the endpoint-text parser owner and applied typed packed endpoint/address-family output.

<a id="module-ui-frontend-runtime"></a>
### `ui/frontend/runtime`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-ui-frontend-runtime)

**Decompiler artifacts:** **493 → 10 (2.0% remaining)**.

- `0x0050BC60` **[`FrontendModelPointerContext_RenderWorldViewQueuesClipped`](src/ui/frontend/menu_room.cpp#L337)** — renamed from `FrontendModelPointerContext_DrawClipped`.
- `0x0050CC80` **[`FrontendModelPointerContext_DispatchWorldCameraPointerInput`](src/ui/frontend/menu_room.cpp#L596)** — renamed from `FrontendModelPointerContext_RightDrag`.
- `0x0050CED0` **[`FrontendModelPointerContext_PointerWheel`](src/ui/frontend/menu_room.cpp#L720)** — frontend model-pointer wheel handling uses the compact recovered pointer-context traversal view.
- `0x0050CF90` **[`FrontendModelPointerContext_Tick`](src/ui/frontend/menu_room.cpp#L770)** — frontend model-pointer tick handling uses the compact recovered pointer-context traversal view.
- `0x00543B70` **[`FrontendMenu_BindSharedResources`](src/ui/frontend/lifecycle.cpp#L363)** — shared-resource binding now addresses task-assignment controls through the compact frontend opaque span plus certified offsets, instead of autogenerated `field_0x...` members.
- `0x005445A0` **`FrontendUiAction2044_IndexedSelectionHelper`** — indexed frontend selection now uses the recovered level player-slot offset array instead of an untyped global block lookup.
- `0x00546700` **[`Frontend_Init`](src/ui/frontend/lifecycle.cpp#L188)** — frontend initialization now operates on compact verified opaque spans for unresolved regions while retaining known callback/state offsets; this removes thousands of misleading sparse auto-fields without claiming new semantics.

<a id="module-ui-controls-lists"></a>
### `ui/controls/lists`

[source](src/ui/controls/lists.cpp) · [header](include/thandor/ui/controls/lists.h) · [call graph](docs/source_guide/ui.md#file-ui-controls-lists) · [full changes](CHANGELOG_FULL.md#module-ui-controls-lists)

**Decompiler artifacts:** **291 → 18 (6.2% remaining)**.

- `0x0040FFE0` **`UiTimedListTree_BuildDirectoryRecordBlockCf`** — path-building scratch is typed as fixed scratch buffers rather than anonymous local byte storage.
- `0x00410380` **`UiTimedListTree_BuildDirectoryHierarchyCf`** — directory-hierarchy construction uses the recovered timed-list path scratch contract.
- `0x00410520` **`UiTimedListTree_AttachDirectoryRecordBlockCf`** — directory record attachment uses typed path scratch/state instead of opaque temporary buffers.
- `0x00410700` **`UiTimedListTree_BuildRecordPathCf`** — record-path construction uses the recovered fixed path scratch buffers and typed path state.

<a id="module-ui-ingame-runtime"></a>
### `ui/ingame/runtime`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-ui-ingame-runtime)

**Decompiler artifacts:** **454 → 12 (2.6% remaining)**.

- `0x0050ECE0` **`InGameUiAction1210_ResourceRegistrationHelper`** — the resource-registration helper now uses typed level header/resource-table storage and the correctly named `ResourceRegistrationRuntime_RebaseLoadedRecords` owner.
- `0x005609F0` **[`InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState`](src/ui/ingame/editor_tools.cpp#L1075)** — interaction-subsystem state now uses the recovered resource-extraction descriptor field instead of a generic runtime cell dword.
- `0x00569780` **[`InGameUiRuntime_DispatchWorldContextActionCallback`](src/ui/ingame/key_commands.cpp#L374)** — world-context action dispatch uses recovered callback/runtime types instead of opaque callback storage.
- `0x0056A8A0` **`InGameUiAction101F_Handler`** — UI action 0x101F now reads the typed level header/title text resource through recovered level storage rather than a condition-runtime alias.

<a id="module-ui-frontend-settings"></a>
### `ui/frontend/settings`

[source](src/ui/frontend/settings.cpp) · [header](include/thandor/ui/frontend/settings.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-settings) · [full changes](CHANGELOG_FULL.md#module-ui-frontend-settings)

**Decompiler artifacts:** **346 → 12 (3.5% remaining)**.

- `0x00549250` **[`FrontendTaskAssignmentPage_Initialize`](src/ui/frontend/task_assignment.cpp#L32)** — task-assignment initialization now indexes the recovered seven player-slot offsets from the level-runtime global block and uses named task-page control storage.
- `0x00549620` **[`FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls`](src/ui/frontend/task_assignment.cpp#L279)** — task-assignment refresh propagates recovered control-offset/state types.

<a id="module-ui-frontend-player"></a>
### `ui/frontend/player`

[source](src/ui/frontend/player.cpp) · [header](include/thandor/ui/frontend/player.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-player) · [full changes](CHANGELOG_FULL.md#module-ui-frontend-player)

**Decompiler artifacts:** **149 → 1 (0.7% remaining)**.

- `0x0054D3A0` **[`FrontendPlayerSetup_ExpireSelectedRuntimeBlock`](src/ui/frontend/player.cpp#L187)** — sparse frontend-network storage is compacted into an explicit opaque gap; the function keeps the same +0x8 access without inventing a false field meaning.

<a id="module-ui-frontend-scenario"></a>
### `ui/frontend/scenario`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-ui-frontend-scenario)

**Decompiler artifacts:** **14 → 1 (7.1% remaining)**.

- `0x00547D60` **[`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](src/ui/frontend/state.cpp#L334)** — frontend-root sparse fields are represented as verified opaque spans; existing offsets for movie, page stacks, pointer context and masks are preserved without autogenerated pseudo-fields.


## Core, assets & platform infrastructure

<a id="module-core-math-fixed"></a>
### `core/math/fixed`

source · header · call graph · [full changes](CHANGELOG_FULL.md#module-core-math-fixed)

**Decompiler artifacts:** **62 → 13 (21.0% remaining)**.

- `0x004BEB20` **`FixedTransform_ApplyEulerRotationToVectorRegs`** — fixed-transform vector rotation uses recovered composite angle/vector types rather than width-only intermediates.
- `0x004BEC50` **`FixedTransform_RotateDirectionScaledCoreRegs`** — scaled direction rotation uses recovered composite vector/angle result types.

<a id="module-platform-system-time-locale"></a>
### `platform/system/time_locale`

[source](src/platform/system/time_locale.cpp) · [header](include/thandor/platform/system/time_locale.h) · [call graph](docs/source_guide/platform.md#file-platform-system-time-locale) · [full changes](CHANGELOG_FULL.md#module-platform-system-time-locale)

**Decompiler artifacts:** **19 → 6 (31.6% remaining)**.

- `0x00586F10` **[`Locale_FormatCurrentDateUtf16`](src/platform/system/time_locale.cpp#L148)** — date formatting uses recovered locale option/state types.
- `0x005871B0` **[`Locale_FormatCurrentTimeUtf16`](src/platform/system/time_locale.cpp#L304)** — time formatting uses recovered locale option/state types.

<a id="module-assets-model-definitions"></a>
### `assets/model/definitions`

[source](src/assets/model/definitions.cpp) · [header](include/thandor/assets/model/definitions.h) · [call graph](docs/source_guide/assets.md#file-assets-model-definitions) · [full changes](CHANGELOG_FULL.md#module-assets-model-definitions)

**Decompiler artifacts:** **76 → 17 (22.4% remaining)**.

- `0x00528600` **`ModelDefinition_RegisterAndResolveReferencesCf`** — model-definition registration now stores the recovered 32-bit placement-contact kind field rather than a generic placement callback selector.

<a id="module-core-error-runtime"></a>
### `core/error/runtime`

[source](src/core/error/runtime.cpp) · [header](include/thandor/core/error/runtime.h) · [call graph](docs/source_guide/core.md#file-core-error-runtime) · [full changes](CHANGELOG_FULL.md#module-core-error-runtime)

**Decompiler artifacts:** **40 → 1 (2.5% remaining)**.

- `0x00575890` **[`ErrorSystem_Init`](src/core/error/runtime.cpp#L41)** — error-system initialization now uses recovered named runtime globals instead of opaque scalar globals.

<a id="module-core-math-interpolation"></a>
### `core/math/interpolation`

[source](src/core/math/interpolation.cpp) · [header](include/thandor/core/math/interpolation.h) · [call graph](docs/source_guide/core.md#file-core-math-interpolation) · [full changes](CHANGELOG_FULL.md#module-core-math-interpolation)

**Decompiler artifacts:** **57 → 2 (3.5% remaining)**.

- `0x0053D320` **[`CubicSpline_EvaluateDerivativeQ12`](src/core/math/spline.cpp#L267)** — spline derivative evaluation propagates recovered fixed-point channel/value types.

<a id="module-core-memory-allocator"></a>
### `core/memory/allocator`

[source](src/core/memory/allocator.cpp) · [header](include/thandor/core/memory/allocator.h) · [call graph](docs/source_guide/core.md#file-core-memory-allocator) · [full changes](CHANGELOG_FULL.md#module-core-memory-allocator)

**Decompiler artifacts:** **25 → 11 (44.0% remaining)**.

- `0x005863C0` **[`ArenaHeap_Init`](src/core/memory/allocator.cpp#L115)** — heap initialization uses recovered arena size/capacity semantics instead of anonymous globals.

<a id="module-platform-bootstrap-runtime"></a>
### `platform/bootstrap/runtime`

[source](src/platform/bootstrap/runtime.cpp) · [header](include/thandor/platform/bootstrap/runtime.h) · [call graph](docs/source_guide/platform.md#file-platform-bootstrap-runtime) · [full changes](CHANGELOG_FULL.md#module-platform-bootstrap-runtime)

**Decompiler artifacts:** **150 → 15 (10.0% remaining)**.

- `0x00585F50` **`MainWindowProc`** — window procedure propagates recovered runtime/global types into message handling.


## Naming & ownership corrections

<a id="module-assets-text-richtext"></a>
### `assets/text/richtext`

[source](src/assets/text/richtext.cpp) · [header](include/thandor/assets/text/richtext.h) · [call graph](docs/source_guide/assets.md#file-assets-text-richtext) · [full changes](CHANGELOG_FULL.md#module-assets-text-richtext)

**Decompiler artifacts:** **226 → 34 (15.0% remaining)**.

- `0x0041B420` **`RichTextCommandStream_PatchNestedStreamPointerPayloads`** — renamed from `RichTextCommandStream_FindNestedStreamPointer`
- `0x0041B520` **`RichTextCommandStream_PatchOpcode1APayloadPair`** — renamed from `RichTextCommandStream_FindOpcode1APayloadPair`
- `0x0041B620` **`RichTextCommandStream_PatchInlinePayloads`** — renamed from `RichTextCommandStream_FindInlinePayloadPair`

<a id="module-gameplay-input-world"></a>
### `gameplay/input/world`

[source](src/gameplay/input/world.cpp) · [header](include/thandor/gameplay/input/world.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-input-world) · [full changes](CHANGELOG_FULL.md#module-gameplay-input-world)

**Decompiler artifacts:** **116 → 7 (6.0% remaining)**.

- `0x0056F770` **`InGameCameraCommand_CaptureBookmarkSlot7EntryAndSharedSuccessTailCf`** — retired the detached standalone owner; the logic is no longer emitted as a separate standalone function.
