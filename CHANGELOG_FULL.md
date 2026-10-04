# Full recovery changelog: public V523 → V537

This changelog is compared directly against the still-public V523 `open-thandor-main` baseline. V524–V536 are validation history only; unpublished intermediate working trees are not used as the comparison baseline.

[README](README.md) · [Short developer changelog](CHANGELOG.md) · [Common types](include/thandor/core/types.h) · [Module tree](docs/MODULE_TREE.md) · [Source file guide](docs/SOURCE_FILE_GUIDE.md)

Decompiler-noise changes and generic semantic signature/type refinements are summarized once per submodule. Function-level entries are developer notes only: concrete gameplay/state semantics, body/field recovery, meaningful structure/typedef propagation, owner corrections, or specific old interpretation → corrected interpretation. Generic register ABI and generic semantic-signature cleanup do not get individual function bullets.

## Overall decompiler cleanup

- Raw decompiler function inventory: **2,071 → 2,070**.
- Public split-tree inventory: **2,033 → 2,068** functions across **104** leaf submodules.
- Existing public owners changed: **1,881**; unchanged: **151**; newly admitted: **36**; retired standalone owners: **1**.
- V537 final decompiler audit: **2,070 functions, 0 failures, 0 `RecoveredCallable_*` owners**.
- Generic semantic signature/type refinements summarized instead of listed individually: **473 function owners**.
- Function-level developer notes after compaction: **126 entries across 42 submodules**; generic-only entries are omitted.

The token census below is taken from the raw V523 and V537 C exports beginning at the first decompiled function, so datatype-preamble growth does not distort the counts.

| Decompiler artifact | V523 | V537 | Change |
| --- | ---: | ---: | ---: |
| `param_N` | 2,680 | 8 | -2,672 |
| `in_EAX` | 788 | 70 | -718 |
| `in_ECX` | 146 | 17 | -129 |
| `in_EDX` | 640 | 18 | -622 |
| `in_EBX` | 0 | 8 | +8 |
| `in_CF` | 378 | 3 | -375 |
| all `in_*` pseudo-inputs | 2,214 | 245 | -1,969 |
| `extraout_*` | 6,080 | 82 | -5,998 |
| `unaff_*` | 408 | 109 | -299 |
| `CONCAT*` | 13,452 | 12,892 | -560 |
| `undefined*` | 9,331 | 6,740 | -2,591 |
| `RecoveredCallable_*` | 48 | 0 | -48 |

`CONCAT*` is not treated as automatically wrong: many remaining uses are real packed/MMX lane construction or explicit multi-register value assembly. V537 adds one such explicit `CONCAT44` when preserving the recovered two-register relative-direction return. Likewise, the small surviving register pseudo-input set is retained where the binary contract is genuinely register-sensitive rather than forced into false stack parameters.

## V537 final semantic closures

- **Sparse view compaction:** 138 certified spans compacted, removing **105,148 bytes** of misleading sparse auto-fields while preserving unresolved regions as explicit opaque gaps rather than inventing semantics.
- **Ground movement:** dedicated steering/track runtime and definition overlays recover turn velocity, movement advance, heading-error interpolation, water-damage parameters, placement-contact kind, and movement/track constants across the four audited movement functions.
- **Weapon aim / timed targets:** dedicated weapon-aim and timed-target views recover yaw/pitch state, attachment reload state, selected target/matching shot links, reload timing and firing definition fields.
- **Linked-child runtime:** class-local build/spawn state now exposes completed slots/IDs, three inherited spawn-state triplets, three pending spawn counters, linked-child flags, translation limits/step and transition sounds.
- **Resource classes 14/15:** formerly generic faction-capacity/grid behavior is identified as resource extraction and resource-storage capacity logic; affected owner names and fields are corrected accordingly.
- **Placement/contact semantics:** the 32-bit placement-contact dispatch kind is propagated across 13 structures and the five audited placement/collision users; it is no longer described as a generic callback selector.
- **Level runtime:** the old condition-runtime blob interpretation is split into a typed level-runtime global block, mutable level image, 64 scheduled-condition records, 16 trigger records, seven player-slot offsets, and named runtime-tail fields; the misclassified rebase owner is moved to resource registration.
- **Class 21 / vertical deployment / destroy-effects:** class-specific overlays recover trajectory/phase state, vertical deployment travel/step/collision retry state, and destroy/effects definition fields.
- **World-owner fields:** model depth-bin masks, tint/color fields and related model-owner data are propagated into collision, pathing and terrain rendering users.
- **Relative-direction return:** the old scalar return lost one physical result component; V537 restores the EAX:EDX composite return contract.

For each submodule, the recovery line reports owner coverage, the number of generic semantic signature/type refinements folded into the summary, and the generic-artifact reduction percentage. The compact **Decompiler cleanup** line counts removed artifact occurrences once; artifact classes that reached zero are not repeated. Only classes still present in V537 are shown as `old → new (remaining%)`. `CONCAT*` is still treated cautiously because remaining uses can be legitimate packed/MMX or multi-register construction.
## Validation

- V537 aggregate apply: **25/25 stages PASS**.
- V537 aggregate audit: **25/25 audits PASS**.
- V537 memory digest audit: `53d097963e0b99c92b3b635255373d83f8b2ae5f65451ec76a11ff41588bd969` over **1,642,496 bytes**.
- V537 decompiler audit: **2,070 functions, 0 failures**; final raw-export oracle SHA-256 `6f397c5a78f17c46c94dd6d46935b678f58faf59989a0de550dfdf8ea2cfc06a`.
- The accepted V537 state contains **0 `RecoveredCallable_*` names**. Exploratory failures from earlier intermediate versions are not presented as release-state regressions.

## Changes by submodule

<a id="module-assets-army-catalog"></a>
### `assets/army/catalog`

[source](src/assets/army/catalog.cpp) · [header](include/thandor/assets/army/catalog.h) · [call graph](docs/source_guide/assets.md#file-assets-army-catalog)


**Recovery impact:** **23/23 owners touched (100%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **155 → 23 (85.2% reduced)**.

**Decompiler cleanup:** **135 artifact occurrences reduced**; remaining: `unaff_*` **23 → 23** (100% remaining).

<a id="module-assets-effect-catalog"></a>
### `assets/effect/catalog`

[source](src/assets/effect/catalog.cpp) · [header](include/thandor/assets/effect/catalog.h) · [call graph](docs/source_guide/assets.md#file-assets-effect-catalog)


**Recovery impact:** **4/4 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **20 → 6 (70% reduced)**.

**Decompiler cleanup:** **22 artifact occurrences reduced**; remaining: `in_EAX` **0 → 6** (new in V537).

<a id="module-assets-fnc-runtime"></a>
### `assets/fnc/runtime`

source · header · call graph


**Recovery impact:** **2/3 owners touched (66.7%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **8 → 0 (100% reduced)**.

**Decompiler cleanup:** **10 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-assets-model-definitions"></a>
### `assets/model/definitions`

[source](src/assets/model/definitions.cpp) · [header](include/thandor/assets/model/definitions.h) · [call graph](docs/source_guide/assets.md#file-assets-model-definitions) · [short developer notes](CHANGELOG.md#module-assets-model-definitions)


**Recovery impact:** **13/14 owners touched (92.9%)**; **1 developer-relevant function note (7.1% of owners)**; **5 semantic signature/type refinements summarized here**; generic artifacts **73 → 17 (76.7% reduced)**.

**Decompiler cleanup:** **59 artifact occurrences reduced**; remaining: `unaff_*` **17 → 16** (94.1% remaining) | `undefined*` **13 → 1** (7.7% remaining).

**Developer notes:**

- `0x00528600` **`ModelDefinition_RegisterAndResolveReferencesCf`** — body/types: model-definition registration now stores the recovered 32-bit placement-contact kind field rather than a generic placement callback selector.

<a id="module-assets-package-codec"></a>
### `assets/package/codec`

[source](src/assets/package/codec.cpp) · [header](include/thandor/assets/package/codec.h) · [call graph](docs/source_guide/assets.md#file-assets-package-codec)


**Recovery impact:** **6/6 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **19 → 7 (63.2% reduced)**.

**Decompiler cleanup:** **12 artifact occurrences reduced**; remaining: `in_EAX` **2 → 2** (100% remaining) | `undefined*` **5 → 5** (100% remaining).

<a id="module-assets-package-runtime"></a>
### `assets/package/runtime`

[source](src/assets/package/runtime.cpp) · [header](include/thandor/assets/package/runtime.h) · [call graph](docs/source_guide/assets.md#file-assets-package-runtime)


**Recovery impact:** **15/15 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **85 → 15 (82.4% reduced)**.

**Decompiler cleanup:** **79 artifact occurrences reduced**; remaining: `in_EBX` **0 → 6** (new in V537) | `in_ECX` **0 → 2** (new in V537) | `undefined*` **27 → 7** (25.9% remaining) | `CONCAT*` **2 → 1** (50% remaining).

<a id="module-assets-resource-runtime"></a>
### `assets/resource/runtime`

source · header · call graph · [short developer notes](CHANGELOG.md#module-assets-resource-runtime)


**Recovery impact:** **4/8 owners touched (50%)**; **1 developer-relevant function note (12.5% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **29 → 3 (89.7% reduced)**.

**Decompiler cleanup:** **32 artifact occurrences reduced**; remaining: `in_ECX` **0 → 3** (new in V537) | `CONCAT*` **9 → 6** (66.7% remaining).

**Developer notes:**

- `0x00532B00` **`ResourceRegistration_ResolveRuntimeRecord`** — body/types: resource-registration lookup now uses the recovered level block/player-slot offsets and named start-camera fields rather than condition-runtime aliases.

<a id="module-assets-rom-runtime"></a>
### `assets/rom/runtime`

[source](src/assets/rom/runtime.cpp) · [header](include/thandor/assets/rom/runtime.h) · [call graph](docs/source_guide/assets.md#file-assets-rom-runtime)


**Recovery impact:** **17/18 owners touched (94.4%)**; **0 developer-relevant function notes**; **6 semantic signature/type refinements summarized here**; generic artifacts **121 → 8 (93.4% reduced)**.

**Decompiler cleanup:** **118 artifact occurrences reduced**; remaining: `unaff_*` **8 → 8** (100% remaining).

<a id="module-assets-scenario-catalog"></a>
### `assets/scenario/catalog`

[source](src/assets/scenario/catalog.cpp) · [header](include/thandor/assets/scenario/catalog.h) · [call graph](docs/source_guide/assets.md#file-assets-scenario-catalog)


**Recovery impact:** **22/22 owners touched (100%)**; **0 developer-relevant function notes**; **15 semantic signature/type refinements summarized here**; generic artifacts **296 → 7 (97.6% reduced)**.

**Decompiler cleanup:** **299 artifact occurrences reduced**; remaining: `undefined*` **120 → 7** (5.8% remaining).

<a id="module-assets-shot-catalog"></a>
### `assets/shot/catalog`

[source](src/assets/shot/catalog.cpp) · [header](include/thandor/assets/shot/catalog.h) · [call graph](docs/source_guide/assets.md#file-assets-shot-catalog)


**Recovery impact:** **7/8 owners touched (87.5%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **44 → 6 (86.4% reduced)**.

**Decompiler cleanup:** **42 artifact occurrences reduced**; remaining: `in_EAX` **6 → 6** (100% remaining).

<a id="module-assets-sprite-catalog"></a>
### `assets/sprite/catalog`

[source](src/assets/sprite/catalog.cpp) · [header](include/thandor/assets/sprite/catalog.h) · [call graph](docs/source_guide/assets.md#file-assets-sprite-catalog)


**Recovery impact:** **3/5 owners touched (60%)**; split inventory **4 → 5**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**.

<a id="module-assets-text-resources"></a>
### `assets/text/resources`

[source](src/assets/text/resources.cpp) · [header](include/thandor/assets/text/resources.h) · [call graph](docs/source_guide/assets.md#file-assets-text-resources)


**Recovery impact:** **11/11 owners touched (100%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **32 → 3 (90.6% reduced)**.

**Decompiler cleanup:** **29 artifact occurrences reduced**; remaining: `in_EAX` **2 → 2** (100% remaining) | `undefined*` **9 → 1** (11.1% remaining).

<a id="module-assets-text-richtext"></a>
### `assets/text/richtext`

[source](src/assets/text/richtext.cpp) · [header](include/thandor/assets/text/richtext.h) · [call graph](docs/source_guide/assets.md#file-assets-text-richtext) · [short developer notes](CHANGELOG.md#module-assets-text-richtext)


**Recovery impact:** **18/18 owners touched (100%)**; **3 developer-relevant function notes (16.7% of owners)**; **11 semantic signature/type refinements summarized here**; generic artifacts **212 → 34 (84.0% reduced)**.

**Decompiler cleanup:** **192 artifact occurrences reduced**; remaining: `param_N` **67 → 9** (13.4% remaining) | `unaff_*` **2 → 2** (100% remaining) | `undefined*` **63 → 23** (36.5% remaining).

**Developer notes:**

- `0x0041B420` **`RichTextCommandStream_PatchNestedStreamPointerPayloads`** — identity: renamed from `RichTextCommandStream_FindNestedStreamPointer`
- `0x0041B520` **`RichTextCommandStream_PatchOpcode1APayloadPair`** — identity: renamed from `RichTextCommandStream_FindOpcode1APayloadPair`
- `0x0041B620` **`RichTextCommandStream_PatchInlinePayloads`** — identity: renamed from `RichTextCommandStream_FindInlinePayloadPair`

<a id="module-audio-backend-runtime"></a>
### `audio/backend/runtime`

[source](src/audio/backend/runtime.cpp) · [header](include/thandor/audio/backend/runtime.h) · [call graph](docs/source_guide/audio.md#file-audio-backend-runtime)


**Recovery impact:** **22/24 owners touched (91.7%)**; **0 developer-relevant function notes**; generic artifacts **35 → 0 (100% reduced)**.

**Decompiler cleanup:** **35 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-audio-codec-sam"></a>
### `audio/codec/sam`

[source](src/audio/codec/sam.cpp) · [header](include/thandor/audio/codec/sam.h) · [call graph](docs/source_guide/audio.md#file-audio-codec-sam)


**Recovery impact:** **5/5 owners touched (100%)**; split inventory **4 → 5**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **1,555 → 1,553 (0.1% reduced)**.

**Decompiler cleanup:** **2 artifact occurrences reduced**; remaining: `in_EAX` **2 → 2** (100% remaining) | `undefined*` **1,553 → 1,551** (99.9% remaining) | `CONCAT*` **19 → 19** (100% remaining).

<a id="module-audio-spatial-runtime"></a>
### `audio/spatial/runtime`

[source](src/audio/spatial/runtime.cpp) · [header](include/thandor/audio/spatial/runtime.h) · [call graph](docs/source_guide/audio.md#file-audio-spatial-runtime)


**Recovery impact:** **10/10 owners touched (100%)**; **0 developer-relevant function notes**; **3 semantic signature/type refinements summarized here**; generic artifacts **51 → 0 (100% reduced)**.

**Decompiler cleanup:** **53 artifact occurrences reduced**; remaining: `CONCAT*` **4 → 2** (50% remaining).

<a id="module-core-error-runtime"></a>
### `core/error/runtime`

[source](src/core/error/runtime.cpp) · [header](include/thandor/core/error/runtime.h) · [call graph](docs/source_guide/core.md#file-core-error-runtime) · [short developer notes](CHANGELOG.md#module-core-error-runtime)


**Recovery impact:** **8/9 owners touched (88.9%)**; **1 developer-relevant function note (11.1% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **40 → 1 (97.5% reduced)**.

**Decompiler cleanup:** **39 artifact occurrences reduced**; remaining: `undefined*` **7 → 1** (14.3% remaining).

**Developer notes:**

- `0x00575890` **[`ErrorSystem_Init`](src/core/error/runtime.cpp#L41)** — body/types: error-system initialization now uses recovered named runtime globals instead of opaque scalar globals.

<a id="module-core-math-fixed"></a>
### `core/math/fixed`

source · header · call graph · [short developer notes](CHANGELOG.md#module-core-math-fixed)


**Recovery impact:** **35/37 owners touched (94.6%)**; split inventory **36 → 37**; **2 developer-relevant function notes (5.4% of owners)**; **11 semantic signature/type refinements summarized here**; generic artifacts **44 → 1 (97.7% reduced)**.

**Decompiler cleanup:** **49 artifact occurrences reduced**; remaining: `undefined*` **18 → 1** (5.6% remaining) | `CONCAT*` **18 → 12** (66.7% remaining).

**Developer notes:**

- `0x004BEB20` **`FixedTransform_ApplyEulerRotationToVectorRegs`** — body/types: fixed-transform vector rotation uses recovered composite angle/vector types rather than width-only intermediates.
- `0x004BEC50` **`FixedTransform_RotateDirectionScaledCoreRegs`** — body/types: scaled direction rotation uses recovered composite vector/angle result types.

<a id="module-core-math-geometry"></a>
### `core/math/geometry`

[source](src/core/math/geometry.cpp) · [header](include/thandor/core/math/geometry.h) · [call graph](docs/source_guide/core.md#file-core-math-geometry)


**Recovery impact:** **1/1 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **69 → 0 (100% reduced)**.

**Decompiler cleanup:** **79 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-core-math-interpolation"></a>
### `core/math/interpolation`

[source](src/core/math/interpolation.cpp) · [header](include/thandor/core/math/interpolation.h) · [call graph](docs/source_guide/core.md#file-core-math-interpolation) · [short developer notes](CHANGELOG.md#module-core-math-interpolation)


**Recovery impact:** **11/12 owners touched (91.7%)**; **1 developer-relevant function note (8.3% of owners)**; **1 semantic signature/type refinement summarized here**; generic artifacts **55 → 2 (96.4% reduced)**.

**Decompiler cleanup:** **55 artifact occurrences reduced**; remaining: `undefined*` **5 → 2** (40% remaining).

**Developer notes:**

- `0x0053D320` **[`CubicSpline_EvaluateDerivativeQ12`](src/core/math/spline.cpp#L267)** — body/types: spline derivative evaluation propagates recovered fixed-point channel/value types.

<a id="module-core-math-random"></a>
### `core/math/random`

[source](src/core/math/random.cpp) · [header](include/thandor/core/math/random.h) · [call graph](docs/source_guide/core.md#file-core-math-random)


**Recovery impact:** **4/6 owners touched (66.7%)**; **0 developer-relevant function notes**.

<a id="module-core-memory-allocator"></a>
### `core/memory/allocator`

[source](src/core/memory/allocator.cpp) · [header](include/thandor/core/memory/allocator.h) · [call graph](docs/source_guide/core.md#file-core-memory-allocator) · [short developer notes](CHANGELOG.md#module-core-memory-allocator)


**Recovery impact:** **11/12 owners touched (91.7%)**; **1 developer-relevant function note (8.3% of owners)**; **1 semantic signature/type refinement summarized here**; generic artifacts **24 → 11 (54.2% reduced)**.

**Decompiler cleanup:** **14 artifact occurrences reduced**; remaining: `in_EAX` **2 → 2** (100% remaining) | `in_ZF` **5 → 5** (100% remaining) | `undefined*` **11 → 4** (36.4% remaining).

**Developer notes:**

- `0x005863C0` **[`ArenaHeap_Init`](src/core/memory/allocator.cpp#L115)** — body/types: heap initialization uses recovered arena size/capacity semantics instead of anonymous globals.

<a id="module-core-memory-synchronization"></a>
### `core/memory/synchronization`

[source](src/core/memory/synchronization.cpp) · [header](include/thandor/core/memory/synchronization.h) · [call graph](docs/source_guide/core.md#file-core-memory-synchronization)


**Recovery impact:** **5/5 owners touched (100%)**; **0 developer-relevant function notes**.

<a id="module-core-settings-persistent"></a>
### `core/settings/persistent`

[source](src/core/settings/persistent.cpp) · [header](include/thandor/core/settings/persistent.h) · [call graph](docs/source_guide/core.md#file-core-settings-persistent)


**Recovery impact:** **6/6 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **16 → 0 (100% reduced)**.

**Decompiler cleanup:** **16 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-core-text-path"></a>
### `core/text/path`

[source](src/core/text/path.cpp) · [header](include/thandor/core/text/path.h) · [call graph](docs/source_guide/core.md#file-core-text-path)


**Recovery impact:** **4/5 owners touched (80%)**; **0 developer-relevant function notes**; **4 semantic signature/type refinements summarized here**; generic artifacts **26 → 2 (92.3% reduced)**.

**Decompiler cleanup:** **27 artifact occurrences reduced**; remaining: `undefined*` **12 → 2** (16.7% remaining).

<a id="module-core-text-string"></a>
### `core/text/string`

[source](src/core/text/string.cpp) · [header](include/thandor/core/text/string.h) · [call graph](docs/source_guide/core.md#file-core-text-string)


**Recovery impact:** **4/5 owners touched (80%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **43 → 0 (100% reduced)**.

**Decompiler cleanup:** **45 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-gameplay-ai-combat"></a>
### `gameplay/ai/combat`

[source](src/gameplay/ai/combat.cpp) · [header](include/thandor/gameplay/ai/combat.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-combat)


**Recovery impact:** **4/4 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **13 → 0 (100% reduced)**.

**Decompiler cleanup:** **14 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-gameplay-ai-placement"></a>
### `gameplay/ai/placement`

[source](src/gameplay/ai/placement.cpp) · [header](include/thandor/gameplay/ai/placement.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-placement)


**Recovery impact:** **12/12 owners touched (100%)**; **0 developer-relevant function notes**; **3 semantic signature/type refinements summarized here**; generic artifacts **136 → 14 (89.7% reduced)**.

**Decompiler cleanup:** **124 artifact occurrences reduced**; remaining: `in_EAX` **11 → 6** (54.5% remaining) | `in_EDX` **22 → 6** (27.3% remaining) | `undefined*` **17 → 2** (11.8% remaining).

<a id="module-gameplay-ai-planning"></a>
### `gameplay/ai/planning`

[source](src/gameplay/ai/planning.cpp) · [header](include/thandor/gameplay/ai/planning.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-planning)


**Recovery impact:** **24/24 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **233 → 0 (100% reduced)**.

**Decompiler cleanup:** **233 artifact occurrences reduced**; remaining: `CONCAT*` **16 → 16** (100% remaining).

<a id="module-gameplay-ai-technology"></a>
### `gameplay/ai/technology`

[source](src/gameplay/ai/technology.cpp) · [header](include/thandor/gameplay/ai/technology.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-technology) · [short developer notes](CHANGELOG.md#module-gameplay-ai-technology)


**Recovery impact:** **9/9 owners touched (100%)**; **1 developer-relevant function note (11.1% of owners)**; **1 semantic signature/type refinement summarized here**; generic artifacts **21 → 0 (100% reduced)**.

**Decompiler cleanup:** **21 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x0053BC20` **[`AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8`](src/gameplay/ai/technology.cpp#L316)** — body/types: AI technology relation scaling uses recovered relation/scale types instead of generic scalar locals.

<a id="module-gameplay-ai-units"></a>
### `gameplay/ai/units`

[source](src/gameplay/ai/units.cpp) · [header](include/thandor/gameplay/ai/units.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-units)


**Recovery impact:** **9/9 owners touched (100%)**; **0 developer-relevant function notes**; **6 semantic signature/type refinements summarized here**; generic artifacts **62 → 0 (100% reduced)**.

**Decompiler cleanup:** **65 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-gameplay-ai-workspaces"></a>
### `gameplay/ai/workspaces`

[source](src/gameplay/ai/workspaces.cpp) · [header](include/thandor/gameplay/ai/workspaces.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-ai-workspaces) · [short developer notes](CHANGELOG.md#module-gameplay-ai-workspaces)


**Recovery impact:** **26/26 owners touched (100%)**; **1 developer-relevant function note (3.8% of owners)**; **4 semantic signature/type refinements summarized here**; generic artifacts **117 → 0 (100% reduced)**.

**Decompiler cleanup:** **120 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x00538230` **[`AiPlanning_RebuildFactionWorkspaces`](src/gameplay/ai/perception.cpp#L455)** — body/types: AI workspace rebuilding now treats the field-grid resource cell value as `ResourceExtractionDescriptor32`, fixing the old generic dword interpretation.

<a id="module-gameplay-army-audio"></a>
### `gameplay/army/audio`

[source](src/gameplay/army/audio.cpp) · [header](include/thandor/gameplay/army/audio.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-army-audio) · [short developer notes](CHANGELOG.md#module-gameplay-army-audio)


**Recovery impact:** **10/10 owners touched (100%)**; **1 developer-relevant function note (10% of owners)**; **1 semantic signature/type refinement summarized here**; generic artifacts **28 → 0 (100% reduced)**.

**Decompiler cleanup:** **29 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x00520E60` **[`ArmyRuntimeAudio_DispatchPositionedSoundVariant`](src/gameplay/army/audio.cpp#L76)** — body/types: positioned-sound dispatch now passes the class-specific `ArmyRuntimeGroundMovementPositionedSoundView120` to both variant handlers instead of an untyped `ArmyRuntimeSlot` view.

<a id="module-gameplay-army-combat"></a>
### `gameplay/army/combat`

[source](src/gameplay/army/combat.cpp) · [header](include/thandor/gameplay/army/combat.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-army-combat) · [short developer notes](CHANGELOG.md#module-gameplay-army-combat)


**Recovery impact:** **9/9 owners touched (100%)**; **3 developer-relevant function notes (33.3% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **100 → 0 (100% reduced)**.

**Decompiler cleanup:** **105 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x00523980` **[`ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments`](src/gameplay/army/combat.cpp#L97)** — body/types: weapon targeting now consumes `ModelRuntimeWeaponAimStateView200`; relative yaw/pitch and their turn velocities are explicit rather than packed into generic class state.
- `0x00525130` **`ArmyRuntimeClass_UpdateResourceStorageFillAndDamageEffect`** — identity/fix: the old `UpdateTransformAndDamageEffect` name hid class-15 behavior. It is now `ArmyRuntimeClass_UpdateResourceStorageFillAndDamageEffect`, with storage selector and empty/full child-Z endpoints typed explicitly.
- `0x00527AC0` **[`ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage`](src/gameplay/army/damage.cpp#L55)** — body/types: timed-effects update uses a dedicated runtime/definition view and names the definition gate that requires runtime state before emit/update work proceeds.

<a id="module-gameplay-army-movement"></a>
### `gameplay/army/movement`

source · header · call graph · [short developer notes](CHANGELOG.md#module-gameplay-army-movement)


**Recovery impact:** **31/32 owners touched (96.9%)**; **9 developer-relevant function notes (28.1% of owners)**; **3 semantic signature/type refinements summarized here**; generic artifacts **349 → 12 (96.6% reduced)**.

**Decompiler cleanup:** **355 artifact occurrences reduced**; remaining: `unaff_*` **8 → 8** (100% remaining) | `undefined*` **69 → 4** (5.8% remaining).

**Developer notes:**

- `0x00520140` **[`ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation`](src/gameplay/army/drive_ground.cpp#L352)** — body/types: uses ground-movement track/definition views; opaque offsets are replaced by water-damage threshold/multiplier, turn rate/acceleration, movement advance, heading-error limits, track texture scale, and placement-contact kind.
- `0x00520840` **`ArmyRuntimeClass_UpdateGroundMovementVariantA`** — body/types: uses the ground-movement steering view; water damage, movement/turn control, heading limits, and placement-contact dispatch fields replace reserved byte-offset accesses.
- `0x00520DF0` **[`ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement`](src/gameplay/army/drive_ground.cpp#L20)** — body/types: switched the ground-movement path to `ModelRuntimeGroundMovementSteeringView200`, exposing the steering state used by the callee chain.
- `0x00520F60` **[`ArmyRuntimeClass_UpdateArticulatedMovement`](src/gameplay/army/walker.cpp#L334)** — body/types: articulated movement now uses the recovered movement-definition view and names the per-tick movement delta plus water-damage threshold/multiplier fields.
- `0x005217A0` **[`ArmyArticulatedRuntime_UpdateSuspensionHierarchy`](src/gameplay/army/walker.cpp#L973)** — body/types: vector-angle, vector-length, and Euler composite return objects are kept distinct through suspension calculations, fixing the old temporary-type cross-assignment without changing the math.
- `0x00522C00` **[`ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation`](src/gameplay/army/drive_banking.cpp#L379)** — body/types: uses the ground-movement steering view and names movement advance, turn velocity, turn rate/acceleration, heading interpolation distance, movement step, and placement-contact kind.
- `0x00523410` **`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantA`** — body/types: weapon-aim state is explicit: yaw/pitch turn velocity, attachment reload countdowns, shot definition, launch vectors, and attachment model-point source replace opaque runtime offsets.
- `0x00523690` **`ArmyRuntimeClass_UpdateMovementAimAndProjectilesVariantB`** — body/types: weapon-aim state is explicit for the alternate firing path, including alternating attachment sequence, per-attachment reload state, yaw/pitch velocities, shot definition, and launch vectors.
- `0x005254F0` **`ArmyRuntimeClass_UpdateGroundMovementVariantB`** — body/types: uses the ground-movement steering view; movement/turn control, heading limits, and placement-contact dispatch fields replace reserved byte-offset accesses.

<a id="module-gameplay-army-placement"></a>
### `gameplay/army/placement`

[source](src/gameplay/army/placement.cpp) · [header](include/thandor/gameplay/army/placement.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-army-placement) · [short developer notes](CHANGELOG.md#module-gameplay-army-placement)


**Recovery impact:** **24/25 owners touched (96%)**; **9 developer-relevant function notes (36% of owners)**; **6 semantic signature/type refinements summarized here**; generic artifacts **145 → 4 (97.2% reduced)**.

**Decompiler cleanup:** **147 artifact occurrences reduced**; remaining: `in_EAX` **9 → 4** (44.4% remaining).

**Developer notes:**

- `0x005244B0` **`ArmyPlacementCandidate_TestOffsetClearanceCf`** — body/types: placement dispatch now reads the recovered 32-bit `placementContactKindIndex278` instead of the old generic callback-selector field.
- `0x00524570` **[`ArmyPlacement_TestModelTerrainAndRuntimeClearance`](src/gameplay/army/placement.cpp#L92)** — body/types: terrain/runtime clearance now uses the recovered placement-contact kind field rather than a generic placement callback selector.
- `0x00525320` **`ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacityAndClearExtractionGridCell`** — identity/fix: the old name implied generic faction-capacity/placement ownership. Recovered class-14 semantics show this is `ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacityAndClearExtractionGridCell`; the body now uses the storage-capacity contribution, resource-field support selector, and extraction descriptor.
- `0x00525420` **`ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacity`** — identity/fix: the old generic `ArmyPlacement_ReleaseFactionCapacity` name was wrong. The class-15 contract is resource storage, so the owner is now `ModelRuntimeSlotClassRelease_SubtractFactionResourceStorageCapacity` with explicit storage selector/capacity fields.
- `0x00527740` **`ArmyPlacementCollision_TestCurrentRuntimeCf`** — body/types: current-runtime placement dispatch uses the recovered `placementContactKindIndex278` field.
- `0x005278D0` **`ArmyPlacementCollision_TestCandidateAndClearanceCf`** — body/types: candidate placement selects the terrain/contact callback through typed `ArmyPlacementContactKindIndex32` storage instead of a generic dword selector.
- `0x00529CB0` **`ArmyCollision_TestPointAgainstRuntimeListCf`** — body/types: collision broad-phase accesses now use named near/far model depth-bin masks instead of opaque world-owner fields.
- `0x00529D70` **`ArmyPlacementCollision_TestPointAgainstRuntimeListCf`** — body/types: placement collision broad-phase accesses now use named near/far model depth-bin masks.
- `0x00529F30` **`ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf`** — body/types: candidate collision broad-phase accesses now use named near/far model depth-bin masks.

<a id="module-gameplay-army-runtime"></a>
### `gameplay/army/runtime`

source · header · call graph · [short developer notes](CHANGELOG.md#module-gameplay-army-runtime)


**Recovery impact:** **56/56 owners touched (100%)**; **13 developer-relevant function notes (23.2% of owners)**; **8 semantic signature/type refinements summarized here**; generic artifacts **443 → 58 (86.9% reduced)**.

**Decompiler cleanup:** **402 artifact occurrences reduced**; remaining: `in_EAX` **16 → 5** (31.2% remaining) | `in_ECX` **51 → 6** (11.8% remaining) | `in_EDX` **16 → 5** (31.2% remaining) | `extraout_*` **223 → 1** (0.4% remaining) | `unaff_*` **10 → 8** (80% remaining) | `undefined*` **112 → 33** (29.5% remaining) | `CONCAT*` **58 → 41** (70.7% remaining).

**Developer notes:**

- `0x0051D350` **[`ArmyRuntimeNode_DispatchTypedCallback`](src/gameplay/army/class_dispatch.cpp#L154)** — body/types: typed callback dispatch contract is propagated into the army runtime node dispatcher.
- `0x0051D960` **`ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs`** — body/types: save-time pointer-to-offset conversion uses recovered pool/link field types.
- `0x00520CF0` **`ArmyRuntimeClass_UpdatePositionedSoundsVariantA`** — body/types: positioned-sound variant A now uses its class-specific runtime view; both sound slots, gain, maximum distance, link state and positioned-sound class state are explicit.
- `0x00523E70` **[`ArmyRuntimeClass_SelectProjectileTargetNode`](src/gameplay/army/weapons.cpp#L199)** — identity: renamed from `ArmyRuntimeClass_SelectProjectileTargetNodeCf`; body/types: target selection now uses the timed-target projectile view; selected target model, matching active shot, shot-definition reference, mode-2 range and adjacent link state are explicit.
- `0x00523FC0` **[`ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects`](src/gameplay/army/weapons.cpp#L251)** — body/types: timed-target firing now uses typed target/link state, reload countdown/interval, selected target model, matching shot runtime and shot-definition state instead of generic class-link fields.
- `0x00524740` **`ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId`** — body/types: class-update slot 13 dispatch uses recovered model/class views instead of opaque dispatch state.
- `0x00525020` **`ArmyRuntimeClass_UpdateResourceExtractionGridAndEffects`** — identity/fix: the old `UpdateGridBoundEffectsAndModels` label was too generic. Class-14 behavior is resource extraction, so the owner is now `ArmyRuntimeClass_UpdateResourceExtractionGridAndEffects` with typed extraction descriptor and weight.
- `0x00525960` **`ArmyRuntimeClass_UpdatePositionedSoundsVariantB`** — body/types: positioned-sound variant B now uses its class-specific runtime view; both sound slots, gain, maximum distance, link state and positioned-sound class state are explicit.
- `0x00525A60` **`ArmyRuntimeClassUpdateSlot21_DispatchByClassId`** — body/types: class-update slot 21 dispatch uses recovered model/class views instead of opaque dispatch state; body/types: class-21 dispatch now uses `ModelRuntimeClass21UpdateView200`; trajectory reference height, phase/travel values, vertical arc coefficient, completion effect, movement step and placement-contact kind replace anonymous class-state slots.
- `0x00526620` **[`ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode`](src/gameplay/army/aircraft.cpp#L516)** — body/types: linked-model class dispatch uses recovered class/link state rather than anonymous runtime slots; body/types: linked-child spawn/build state is recovered as a dedicated view: selected asset/build counters, completed asset IDs, inherited spawn triplets, pending spawn counts, runtime flags, slot capacity, translation limits/step and transition sounds are named.
- `0x00527C00` **[`ArmyRuntime_UpdateTimedShotAndEffectEmitters`](src/gameplay/army/weapons.cpp#L614)** — body/types: timed shot/effect emitter state and definition references are propagated through the update loop.
- `0x00528330` **[`ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy`](src/gameplay/army/class_updates.cpp#L193)** — body/types: destroy/effects update now uses a dedicated runtime/definition overlay and names the per-tick vertical translation step.
- `0x005283D0` **[`ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState`](src/gameplay/army/class_updates.cpp#L307)** — body/types: vertical-deployment/collision state is represented by recovered class-state fields instead of anonymous offsets; body/types: vertical deployment now uses dedicated definition/runtime views; travel accumulator/limit, per-tick vertical step, collision retry countdown and deployment sound are named.

<a id="module-gameplay-faction-relations"></a>
### `gameplay/faction/relations`

[source](src/gameplay/faction/relations.cpp) · [header](include/thandor/gameplay/faction/relations.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-faction-relations) · [short developer notes](CHANGELOG.md#module-gameplay-faction-relations)


**Recovery impact:** **12/12 owners touched (100%)**; **1 developer-relevant function note (8.3% of owners)**; **6 semantic signature/type refinements summarized here**; generic artifacts **75 → 0 (100% reduced)**.

**Decompiler cleanup:** **80 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x0053C0F0` **`GameFactionRelations_EvaluateTransitionRulesCf`** — body/types: end-condition rules now read the typed level condition schedule and trigger records through `g_InGameLevelRuntimeGlobalBlock` instead of the misclassified condition-runtime blob.

<a id="module-gameplay-faction-runtime"></a>
### `gameplay/faction/runtime`

[source](src/gameplay/faction/runtime.cpp) · [header](include/thandor/gameplay/faction/runtime.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-faction-runtime) · [short developer notes](CHANGELOG.md#module-gameplay-faction-runtime)


**Recovery impact:** **28/28 owners touched (100%)**; **1 developer-relevant function note (3.6% of owners)**; **16 semantic signature/type refinements summarized here**; generic artifacts **141 → 22 (84.4% reduced)**.

**Decompiler cleanup:** **127 artifact occurrences reduced**; remaining: `in_EAX` **14 → 5** (35.7% remaining) | `in_ECX` **2 → 4** (100% increase) | `in_EDX` **12 → 5** (41.7% remaining) | `unaff_*` **2 → 2** (100% remaining) | `undefined*` **58 → 6** (10.3% remaining).

**Developer notes:**

- `0x00565320` **[`OldUnitRuntime_RebuildScenarioReplayTables`](src/gameplay/faction/carryover.cpp#L27)** — body/types: replay-table rebuild now names the saved model local-rotation-angle field instead of treating it as an anonymous scalar.

<a id="module-gameplay-input-world"></a>
### `gameplay/input/world`

[source](src/gameplay/input/world.cpp) · [header](include/thandor/gameplay/input/world.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-input-world) · [short developer notes](CHANGELOG.md#module-gameplay-input-world)


**Recovery impact:** **8/8 owners touched (100%)**; split inventory **8 → 7**; **1 developer-relevant function note (12.5% of owners)**; **1 semantic signature/type refinement summarized here**; generic artifacts **115 → 7 (93.9% reduced)**.

**Decompiler cleanup:** **109 artifact occurrences reduced**; remaining: `undefined*` **20 → 7** (35% remaining).

**Developer notes:**

- `0x0056F770` **`InGameCameraCommand_CaptureBookmarkSlot7EntryAndSharedSuccessTailCf`** — ownership: retired the detached standalone owner; ownership: the logic is no longer emitted as a separate standalone function.

<a id="module-gameplay-selection-overlay"></a>
### `gameplay/selection/overlay`

[source](src/gameplay/selection/overlay.cpp) · [header](include/thandor/gameplay/selection/overlay.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-selection-overlay)


**Recovery impact:** **16/16 owners touched (100%)**; **0 developer-relevant function notes**; **13 semantic signature/type refinements summarized here**; generic artifacts **356 → 0 (100% reduced)**.

**Decompiler cleanup:** **362 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-gameplay-selection-runtime"></a>
### `gameplay/selection/runtime`

source · header · call graph


**Recovery impact:** **56/58 owners touched (96.6%)**; split inventory **55 → 58**; **0 developer-relevant function notes**; **26 semantic signature/type refinements summarized here**; generic artifacts **352 → 27 (92.3% reduced)**.

**Decompiler cleanup:** **338 artifact occurrences reduced**; remaining: `in_CF` **11 → 3** (27.3% remaining) | `in_ZF` **12 → 16** (33.3% increase) | `undefined*` **87 → 8** (9.2% remaining).

<a id="module-gameplay-session-level"></a>
### `gameplay/session/level`

[source](src/gameplay/session/level.cpp) · [header](include/thandor/gameplay/session/level.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-session-level) · [short developer notes](CHANGELOG.md#module-gameplay-session-level)


**Recovery impact:** **5/5 owners touched (100%)**; **4 developer-relevant function notes (80% of owners)**; **3 semantic signature/type refinements summarized here**; generic artifacts **235 → 4 (98.3% reduced)**.

**Decompiler cleanup:** **239 artifact occurrences reduced**; remaining: `undefined*` **73 → 4** (5.5% remaining).

**Developer notes:**

- `0x005311D0` **`InGameLevelRuntime_LoadResourcesAfterDefaultResetCf`** — body/types: level loading now uses the recovered global level block, condition storage, player-slot offset array, relation masks, sample tables and initial-army placement count instead of indexing an opaque condition-runtime allocation.
- `0x00532020` **`InGameLevelRuntime_LoadResourcesAfterExternalTablesCf`** — body/types: external-table level loading now uses typed level storage/player-slot/sample fields and calls the correctly identified resource-registration rebase owner.
- `0x005329C0` **[`InGameLevelRuntime_ShutdownLoadedAssetResources`](src/gameplay/session/level.cpp#L1216)** — body/types: shutdown now releases the typed level condition-storage allocation through the recovered level-runtime global block.
- `0x00532CA0` **`InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf`** — body/types: level save now uses the recovered model local-rotation-angle field in the serialized world-state path.

<a id="module-gameplay-session-runtime"></a>
### `gameplay/session/runtime`

[source](src/gameplay/session/runtime.cpp) · [header](include/thandor/gameplay/session/runtime.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-session-runtime) · [short developer notes](CHANGELOG.md#module-gameplay-session-runtime)


**Recovery impact:** **15/16 owners touched (93.8%)**; **4 developer-relevant function notes (25% of owners)**; **3 semantic signature/type refinements summarized here**; generic artifacts **140 → 9 (93.6% reduced)**.

**Decompiler cleanup:** **139 artifact occurrences reduced**; remaining: `undefined*` **45 → 9** (20% remaining).

**Developer notes:**

- `0x0050E120` **[`InGameConditionRuntime_UpdateScheduledRecords`](src/gameplay/session/runtime.cpp#L6148)** — body/types: the 64 scheduled conditions and 16 trigger records now use typed kind/status/payload records; raw `+0x10` byte stepping and numeric condition kinds are replaced by the recovered schedule view/enums.
- `0x0050EA90` **[`ResourceRegistrationRuntime_RebaseLoadedRecords`](src/gameplay/session/runtime.cpp#L4416)** — identity/fix: this owner was misclassified as `InGameConditionRuntime_RebaseLoadedRecords`. V537 identifies it as `ResourceRegistrationRuntime_RebaseLoadedRecords` and types the resource-registration pointer/offset domains it rebases.
- `0x00513160` **[`InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState`](src/gameplay/session/runtime.cpp#L6557)** — body/types: resource/energy update now reaches the level header through typed level condition storage instead of indexing `g_InGameConditionRuntime` as an unrelated record array.
- `0x00566290` **`EndGameResultsUiRuntime_UpdateAndHandleInputCf`** — body/types: end-game result handling now reads typed schedule conditions and level runtime-tail/music fields from the recovered level storage rather than anonymous condition-runtime offsets.

<a id="module-gameplay-session-savegame"></a>
### `gameplay/session/savegame`

[source](src/gameplay/session/savegame.cpp) · [header](include/thandor/gameplay/session/savegame.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-session-savegame)


**Recovery impact:** **5/5 owners touched (100%)**; **0 developer-relevant function notes**; **3 semantic signature/type refinements summarized here**; generic artifacts **76 → 2 (97.4% reduced)**.

**Decompiler cleanup:** **77 artifact occurrences reduced**; remaining: `undefined*` **12 → 2** (16.7% remaining).

<a id="module-gameplay-technology-runtime"></a>
### `gameplay/technology/runtime`

[source](src/gameplay/technology/runtime.cpp) · [header](include/thandor/gameplay/technology/runtime.h) · [call graph](docs/source_guide/gameplay.md#file-gameplay-technology-runtime)


**Recovery impact:** **5/5 owners touched (100%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **24 → 1 (95.8% reduced)**.

**Decompiler cleanup:** **24 artifact occurrences reduced**; remaining: `undefined*` **6 → 1** (16.7% remaining).

<a id="module-graphics-backend-direct3d"></a>
### `graphics/backend/direct3d`

source · header · call graph


**Recovery impact:** **14/14 owners touched (100%)**; **0 developer-relevant function notes**; **4 semantic signature/type refinements summarized here**; generic artifacts **291 → 210 (27.8% reduced)**.

**Decompiler cleanup:** **88 artifact occurrences reduced**; remaining: `in_ST1` **30 → 30** (100% remaining) | `in_ST2` **8 → 8** (100% remaining) | `in_ST3` **8 → 8** (100% remaining) | `extraout_*` **63 → 44** (69.8% remaining) | `undefined*` **141 → 120** (85.1% remaining) | `CONCAT*` **203 → 196** (96.6% remaining).

<a id="module-graphics-backend-directdraw"></a>
### `graphics/backend/directdraw`

[source](src/graphics/backend/directdraw.cpp) · [header](include/thandor/graphics/backend/directdraw.h) · [call graph](docs/source_guide/graphics.md#file-graphics-backend-directdraw)


**Recovery impact:** **3/5 owners touched (60%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **32 → 8 (75% reduced)**.

**Decompiler cleanup:** **28 artifact occurrences reduced**; remaining: `in_EAX` **6 → 2** (33.3% remaining) | `undefined*` **12 → 6** (50% remaining).

<a id="module-graphics-backend-glide"></a>
### `graphics/backend/glide`

source · header · call graph · [short developer notes](CHANGELOG.md#module-graphics-backend-glide)


**Recovery impact:** **35/37 owners touched (94.6%)**; split inventory **34 → 37**; **2 developer-relevant function notes (5.4% of owners)**; **3 semantic signature/type refinements summarized here**; generic artifacts **410 → 307 (25.1% reduced)**.

**Decompiler cleanup:** **113 artifact occurrences reduced**; remaining: `extraout_*` **34 → 2** (5.9% remaining) | `undefined*` **327 → 305** (93.3% remaining) | `CONCAT*` **569 → 559** (98.2% remaining).

**Developer notes:**

- `0x00580FF0` **`GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444`** — identity/body: admitted the alpha downsample owner and recovered source/destination image/subresource parameter roles.
- `0x005810A0` **`GraphicsGlide3_DownsampleAlternateAlphaSamplesToWhiteArgb4444`** — identity/body: admitted the alternate-alpha downsample owner and recovered source/destination image/subresource parameter roles.

<a id="module-graphics-backend-software"></a>
### `graphics/backend/software`

[source](src/graphics/backend/software.cpp) · [header](include/thandor/graphics/backend/software.h) · [call graph](docs/source_guide/graphics.md#file-graphics-backend-software)


**Recovery impact:** **42/103 owners touched (40.8%)**; **0 developer-relevant function notes**; **4 semantic signature/type refinements summarized here**; generic artifacts **4,266 → 4,138 (3.0% reduced)**.

**Decompiler cleanup:** **152 artifact occurrences reduced**; remaining: `in_MM2_Da` **60 → 60** (100% remaining) | `undefined*` **4,106 → 4,078** (99.3% remaining) | `CONCAT*` **10,894 → 10,870** (99.8% remaining).

<a id="module-graphics-core-runtime"></a>
### `graphics/core/runtime`

source · [header](include/thandor/graphics/core/runtime.h) · [call graph](docs/source_guide/graphics.md#file-graphics-core-runtime)


**Recovery impact:** **27/28 owners touched (96.4%)**; **0 developer-relevant function notes**; **6 semantic signature/type refinements summarized here**; generic artifacts **143 → 14 (90.2% reduced)**.

**Decompiler cleanup:** **133 artifact occurrences reduced**; remaining: `in_EAX` **8 → 2** (25% remaining) | `undefined*` **45 → 12** (26.7% remaining).

<a id="module-graphics-render-model"></a>
### `graphics/render/model`

source · [header](include/thandor/graphics/render/model.h) · [call graph](docs/source_guide/graphics.md#file-graphics-render-model) · [short developer notes](CHANGELOG.md#module-graphics-render-model)


**Recovery impact:** **14/16 owners touched (87.5%)**; **1 developer-relevant function note (6.2% of owners)**; **13 semantic signature/type refinements summarized here**; generic artifacts **375 → 71 (81.1% reduced)**.

**Decompiler cleanup:** **318 artifact occurrences reduced**; remaining: `extraout_*` **78 → 2** (2.6% remaining) | `undefined*` **118 → 69** (58.5% remaining) | `CONCAT*` **148 → 134** (90.5% remaining).

**Developer notes:**

- `0x004CC940` **[`ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath`](src/graphics/render/model_lighting.cpp#L1397)** — body/types: nearby-light alternate path uses recovered render/light parameter roles instead of generic scalar arguments.

<a id="module-graphics-render-primitives"></a>
### `graphics/render/primitives`

[source](src/graphics/render/primitives.cpp) · [header](include/thandor/graphics/render/primitives.h) · [call graph](docs/source_guide/graphics.md#file-graphics-render-primitives) · [short developer notes](CHANGELOG.md#module-graphics-render-primitives)


**Recovery impact:** **15/15 owners touched (100%)**; **2 developer-relevant function notes (13.3% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **86 → 9 (89.5% reduced)**.

**Decompiler cleanup:** **82 artifact occurrences reduced**; remaining: `in_EAX` **6 → 6** (100% remaining) | `undefined*` **31 → 3** (9.7% remaining).

**Developer notes:**

- `0x004D0A10` **[`GraphicsPrimitiveQueue_AllocateGlobalPool`](src/graphics/render/primitives.cpp#L153)** — body/types: primitive-queue allocation uses the recovered queue capacity/global contract.
- `0x004D0DA0` **`GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangleCf`** — identity: renamed from `GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle`

<a id="module-graphics-render-projection"></a>
### `graphics/render/projection`

[source](src/graphics/render/projection.cpp) · [header](include/thandor/graphics/render/projection.h) · [call graph](docs/source_guide/graphics.md#file-graphics-render-projection)


**Recovery impact:** **3/3 owners touched (100%)**; split inventory **2 → 3**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **21 → 1 (95.2% reduced)**.

**Decompiler cleanup:** **22 artifact occurrences reduced**; remaining: `undefined*` **6 → 1** (16.7% remaining).

<a id="module-graphics-render-shading"></a>
### `graphics/render/shading`

source · header · call graph · [short developer notes](CHANGELOG.md#module-graphics-render-shading)


**Recovery impact:** **27/27 owners touched (100%)**; **6 developer-relevant function notes (22.2% of owners)**; **6 semantic signature/type refinements summarized here**; generic artifacts **957 → 100 (89.6% reduced)**.

**Decompiler cleanup:** **860 artifact occurrences reduced**; remaining: `undefined*` **112 → 100** (89.3% remaining) | `CONCAT*` **59 → 56** (94.9% remaining).

**Developer notes:**

- `0x004CD200` **[`GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes`](src/graphics/render/shadow_texture.cpp#L722)** — body/types: generated-texture scratch/global state names were propagated into reset/clear logic.
- `0x004CD2B0` **[`GraphicsShadingGeneratedTexture_AdvanceTileCursor`](src/graphics/render/shadow_texture.cpp#L1118)** — body/types: generated-texture tile cursor state is represented through named scratch/global state.
- `0x004CD360` **[`GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources`](src/graphics/render/shadow_texture.cpp#L766)** — body/types: touched-alpha subresource state is propagated through the refresh path.
- `0x004CD3D0` **[`GraphicsShadingGeneratedTexture_FilterGridScratchMmx`](src/graphics/render/shadow_texture.cpp#L1198)** — body/types: generated-texture filter scratch state is typed/named while retaining the MMX operation sequence.
- `0x004CD690` **[`GraphicsShadingGeneratedTexture_RasterizeTriangleMask`](src/graphics/render/shadow_texture.cpp#L1493)** — body/types: rasterizer mask inputs and generated-texture scratch state are typed instead of generic parameters.
- `0x004CDD40` **[`GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy`](src/graphics/render/shadow_texture.cpp#L517)** — body/types: hierarchy processing replaces hundreds of generic parameters/temporaries with the recovered generated-texture/runtime views.

<a id="module-graphics-resources-framebuffer"></a>
### `graphics/resources/framebuffer`

[source](src/graphics/resources/framebuffer.cpp) · [header](include/thandor/graphics/resources/framebuffer.h) · [call graph](docs/source_guide/graphics.md#file-graphics-resources-framebuffer)


**Recovery impact:** **7/7 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **11 → 5 (54.5% reduced)**.

**Decompiler cleanup:** **6 artifact occurrences reduced**; remaining: `undefined*` **5 → 5** (100% remaining) | `CONCAT*` **1 → 1** (100% remaining).

<a id="module-graphics-resources-palette"></a>
### `graphics/resources/palette`

[source](src/graphics/resources/palette.cpp) · [header](include/thandor/graphics/resources/palette.h) · [call graph](docs/source_guide/graphics.md#file-graphics-resources-palette)


**Recovery impact:** **12/13 owners touched (92.3%)**; **0 developer-relevant function notes**; **5 semantic signature/type refinements summarized here**; generic artifacts **165 → 0 (100% reduced)**.

**Decompiler cleanup:** **170 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-graphics-resources-texture"></a>
### `graphics/resources/texture`

source · [header](include/thandor/graphics/resources/texture.h) · [call graph](docs/source_guide/graphics.md#file-graphics-resources-texture) · [short developer notes](CHANGELOG.md#module-graphics-resources-texture)


**Recovery impact:** **35/37 owners touched (94.6%)**; split inventory **36 → 37**; **1 developer-relevant function note (2.7% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **191 → 139 (27.2% reduced)**.

**Decompiler cleanup:** **55 artifact occurrences reduced**; remaining: `in_EAX` **14 → 8** (57.1% remaining) | `in_EDX` **2 → 2** (100% remaining) | `undefined*` **134 → 129** (96.3% remaining) | `CONCAT*` **103 → 100** (97.1% remaining).

**Developer notes:**

- `0x004AC8E0` **`GraphicsTextureSource_DecomposeSubresourceRegionsCf`** — identity/body: admitted the texture-subresource decomposition owner and typed its region/image contract.

<a id="module-movie-runtime-playback"></a>
### `movie/runtime/playback`

[source](src/movie/runtime/playback.cpp) · [header](include/thandor/movie/runtime/playback.h) · [call graph](docs/source_guide/movie.md#file-movie-runtime-playback) · [short developer notes](CHANGELOG.md#module-movie-runtime-playback)


**Recovery impact:** **16/18 owners touched (88.9%)**; **1 developer-relevant function note (5.6% of owners)**; **7 semantic signature/type refinements summarized here**; generic artifacts **359 → 137 (61.8% reduced)**.

**Decompiler cleanup:** **234 artifact occurrences reduced**; remaining: `unaff_*` **16 → 2** (12.5% remaining) | `undefined*` **165 → 135** (81.8% remaining) | `CONCAT*` **235 → 223** (94.9% remaining).

**Developer notes:**

- `0x004A7770` **[`Movie_EncodeFrame4x4Delta`](src/movie/runtime/flm_encoder.cpp#L381)** — body/types: frame-delta encoder parameters and packed block/state values were propagated through the encoder body.

<a id="module-network-backend-fallback-udp"></a>
### `network/backend/fallback_udp`

[source](src/network/backend/fallback_udp.cpp) · [header](include/thandor/network/backend/fallback_udp.h) · [call graph](docs/source_guide/network.md#file-network-backend-fallback-udp) · [short developer notes](CHANGELOG.md#module-network-backend-fallback-udp)


**Recovery impact:** **17/22 owners touched (77.3%)**; split inventory **19 → 22**; **9 developer-relevant function notes (40.9% of owners)**; **5 semantic signature/type refinements summarized here**; generic artifacts **55 → 3 (94.5% reduced)**.

**Decompiler cleanup:** **56 artifact occurrences reduced**; remaining: `extraout_*` **2 → 2** (100% remaining) | `undefined*` **14 → 1** (7.1% remaining) | `CONCAT*` **5 → 1** (20% remaining).

**Developer notes:**

- `0x00584E80` **`NetworkFallback_OpenAndBindUdpSocketCf`** — body/types: network endpoint/address-family/socket state is typed instead of raw packed dwords.
- `0x00585030` **[`NetworkFallback_CloseActiveSocket`](src/network/backend/fallback_udp.cpp#L254)** — body/types: active fallback socket state is represented by the recovered socket contract.
- `0x00585060` **`NetworkFallback_ReceiveDatagramCf`** — body/types: receive path uses typed endpoint/socket address storage.
- `0x005850B0` **`NetworkFallback_SendDatagramCf`** — body/types: send path uses typed endpoint/socket address storage.
- `0x005852A0` **`NetworkBackend_OpenAndBindActiveSocketCf`** — body/types: backend socket creation/bind uses recovered endpoint, address-family and packed socket-address types.
- `0x00585450` **`NetworkFallbackUdp_CloseSocket`** — body/types: fallback UDP close uses the recovered socket handle/type contract.
- `0x00585480` **`NetworkFallbackUdp_ReceiveDatagram`** — body/types: fallback UDP receive uses typed endpoint/socket-address storage.
- `0x005854E0` **`NetworkFallbackUdp_SendDatagram`** — body/types: fallback UDP send uses typed endpoint/socket-address storage.
- `0x00585550` **`NetworkBackend_ParseEndpointTextCf`** — identity/body: recovered the endpoint-text parser owner and applied typed packed endpoint/address-family output.

<a id="module-network-backend-runtime"></a>
### `network/backend/runtime`

[source](src/network/backend/runtime.cpp) · [header](include/thandor/network/backend/runtime.h) · [call graph](docs/source_guide/network.md#file-network-backend-runtime)


**Recovery impact:** **11/11 owners touched (100%)**; split inventory **8 → 11**; **0 developer-relevant function notes**; **6 semantic signature/type refinements summarized here**; generic artifacts **64 → 0 (100% reduced)**.

**Decompiler cleanup:** **74 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-network-protocol-commands"></a>
### `network/protocol/commands`

[source](src/network/protocol/commands.cpp) · [header](include/thandor/network/protocol/commands.h) · [call graph](docs/source_guide/network.md#file-network-protocol-commands)


**Recovery impact:** **5/5 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **9 → 0 (100% reduced)**.

**Decompiler cleanup:** **11 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-network-protocol-transfer"></a>
### `network/protocol/transfer`

source · header · call graph


**Recovery impact:** **27/27 owners touched (100%)**; **0 developer-relevant function notes**; **11 semantic signature/type refinements summarized here**; generic artifacts **150 → 16 (89.3% reduced)**.

**Decompiler cleanup:** **149 artifact occurrences reduced**; remaining: `in_EAX` **27 → 2** (7.4% remaining) | `in_ECX` **0 → 2** (new in V537) | `undefined*` **49 → 12** (24.5% remaining).

<a id="module-platform-bootstrap-runtime"></a>
### `platform/bootstrap/runtime`

[source](src/platform/bootstrap/runtime.cpp) · [header](include/thandor/platform/bootstrap/runtime.h) · [call graph](docs/source_guide/platform.md#file-platform-bootstrap-runtime) · [short developer notes](CHANGELOG.md#module-platform-bootstrap-runtime)


**Recovery impact:** **15/17 owners touched (88.2%)**; **1 developer-relevant function note (5.9% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **137 → 14 (89.8% reduced)**.

**Decompiler cleanup:** **137 artifact occurrences reduced**; remaining: `in_EAX` **7 → 5** (71.4% remaining) | `in_EBX` **0 → 2** (new in V537) | `extraout_*` **58 → 2** (3.4% remaining) | `undefined*` **49 → 5** (10.2% remaining) | `CONCAT*` **13 → 1** (7.7% remaining).

**Developer notes:**

- `0x00585F50` **`MainWindowProc`** — body/types: window procedure propagates recovered runtime/global types into message handling.

<a id="module-platform-filesystem-win32"></a>
### `platform/filesystem/win32`

[source](src/platform/filesystem/win32.cpp) · [header](include/thandor/platform/filesystem/win32.h) · [call graph](docs/source_guide/platform.md#file-platform-filesystem-win32)


**Recovery impact:** **25/29 owners touched (86.2%)**; split inventory **26 → 29**; **0 developer-relevant function notes**; **13 semantic signature/type refinements summarized here**; generic artifacts **107 → 4 (96.3% reduced)**.

**Decompiler cleanup:** **120 artifact occurrences reduced**; remaining: `undefined*` **37 → 4** (10.8% remaining) | `CONCAT*` **18 → 1** (5.6% remaining).

<a id="module-platform-input-devices"></a>
### `platform/input/devices`

[source](src/platform/input/devices.cpp) · [header](include/thandor/platform/input/devices.h) · [call graph](docs/source_guide/platform.md#file-platform-input-devices)


**Recovery impact:** **15/15 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **20 → 6 (70% reduced)**.

**Decompiler cleanup:** **16 artifact occurrences reduced**; remaining: `in_EAX` **2 → 2** (100% remaining) | `extraout_*` **8 → 2** (25% remaining) | `undefined*` **5 → 2** (40% remaining).

<a id="module-platform-system-time-locale"></a>
### `platform/system/time_locale`

[source](src/platform/system/time_locale.cpp) · [header](include/thandor/platform/system/time_locale.h) · [call graph](docs/source_guide/platform.md#file-platform-system-time-locale) · [short developer notes](CHANGELOG.md#module-platform-system-time-locale)


**Recovery impact:** **12/16 owners touched (75%)**; **2 developer-relevant function notes (12.5% of owners)**; **1 semantic signature/type refinement summarized here**; generic artifacts **18 → 5 (72.2% reduced)**.

**Decompiler cleanup:** **13 artifact occurrences reduced**; remaining: `undefined*` **18 → 5** (27.8% remaining) | `CONCAT*` **1 → 1** (100% remaining).

**Developer notes:**

- `0x00586F10` **[`Locale_FormatCurrentDateUtf16`](src/platform/system/time_locale.cpp#L148)** — body/types: date formatting uses recovered locale option/state types.
- `0x005871B0` **[`Locale_FormatCurrentTimeUtf16`](src/platform/system/time_locale.cpp#L304)** — body/types: time formatting uses recovered locale option/state types.

<a id="module-platform-system-win32"></a>
### `platform/system/win32`

[source](src/platform/system/win32.cpp) · [header](include/thandor/platform/system/win32.h) · [call graph](docs/source_guide/platform.md#file-platform-system-win32)


**Recovery impact:** **2/2 owners touched (100%)**; **0 developer-relevant function notes**.

<a id="module-ui-controls-buttons"></a>
### `ui/controls/buttons`

[source](src/ui/controls/buttons.cpp) · [header](include/thandor/ui/controls/buttons.h) · [call graph](docs/source_guide/ui.md#file-ui-controls-buttons)


**Recovery impact:** **17/17 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **44 → 3 (93.2% reduced)**.

**Decompiler cleanup:** **41 artifact occurrences reduced**; remaining: `unaff_*` **3 → 3** (100% remaining).

<a id="module-ui-controls-input"></a>
### `ui/controls/input`

[source](src/ui/controls/input.cpp) · [header](include/thandor/ui/controls/input.h) · [call graph](docs/source_guide/ui.md#file-ui-controls-input)


**Recovery impact:** **32/32 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **179 → 70 (60.9% reduced)**.

**Decompiler cleanup:** **111 artifact occurrences reduced**; remaining: `extraout_*` **79 → 7** (8.9% remaining) | `undefined*` **66 → 63** (95.5% remaining) | `CONCAT*` **89 → 87** (97.8% remaining).

<a id="module-ui-controls-layout"></a>
### `ui/controls/layout`

source · header · call graph


**Recovery impact:** **49/49 owners touched (100%)**; split inventory **48 → 49**; **0 developer-relevant function notes**; **4 semantic signature/type refinements summarized here**; generic artifacts **166 → 5 (97.0% reduced)**.

**Decompiler cleanup:** **163 artifact occurrences reduced**; remaining: `extraout_*` **89 → 2** (2.2% remaining) | `unaff_*` **2 → 2** (100% remaining) | `undefined*` **18 → 1** (5.6% remaining) | `CONCAT*` **4 → 2** (50% remaining).

<a id="module-ui-controls-lists"></a>
### `ui/controls/lists`

[source](src/ui/controls/lists.cpp) · [header](include/thandor/ui/controls/lists.h) · [call graph](docs/source_guide/ui.md#file-ui-controls-lists) · [short developer notes](CHANGELOG.md#module-ui-controls-lists)


**Recovery impact:** **62/64 owners touched (96.9%)**; split inventory **54 → 64**; **4 developer-relevant function notes (6.2% of owners)**; **20 semantic signature/type refinements summarized here**; generic artifacts **284 → 17 (94.0% reduced)**.

**Decompiler cleanup:** **278 artifact occurrences reduced**; remaining: `unaff_*` **4 → 9** (125% increase) | `undefined*` **32 → 8** (25% remaining) | `CONCAT*` **7 → 1** (14.3% remaining).

**Developer notes:**

- `0x0040FFE0` **`UiTimedListTree_BuildDirectoryRecordBlockCf`** — body/types: path-building scratch is typed as fixed scratch buffers rather than anonymous local byte storage.
- `0x00410380` **`UiTimedListTree_BuildDirectoryHierarchyCf`** — body/types: directory-hierarchy construction uses the recovered timed-list path scratch contract.
- `0x00410520` **`UiTimedListTree_AttachDirectoryRecordBlockCf`** — body/types: directory record attachment uses typed path scratch/state instead of opaque temporary buffers.
- `0x00410700` **`UiTimedListTree_BuildRecordPathCf`** — body/types: record-path construction uses the recovered fixed path scratch buffers and typed path state.

<a id="module-ui-controls-misc"></a>
### `ui/controls/misc`

source · header · call graph


**Recovery impact:** **23/23 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **240 → 6 (97.5% reduced)**.

**Decompiler cleanup:** **234 artifact occurrences reduced**; remaining: `extraout_*` **222 → 2** (0.9% remaining) | `undefined*` **9 → 4** (44.4% remaining).

<a id="module-ui-controls-text"></a>
### `ui/controls/text`

[source](src/ui/controls/text.cpp) · [header](include/thandor/ui/controls/text.h) · [call graph](docs/source_guide/ui.md#file-ui-controls-text)


**Recovery impact:** **63/63 owners touched (100%)**; **0 developer-relevant function notes**; **8 semantic signature/type refinements summarized here**; generic artifacts **293 → 3 (99.0% reduced)**.

**Decompiler cleanup:** **297 artifact occurrences reduced**; remaining: `in_ZF` **0 → 2** (new in V537) | `undefined*` **40 → 1** (2.5% remaining).

<a id="module-ui-core-runtime"></a>
### `ui/core/runtime`

[source](src/ui/core/runtime.cpp) · [header](include/thandor/ui/core/runtime.h) · [call graph](docs/source_guide/ui.md#file-ui-core-runtime)


**Recovery impact:** **21/28 owners touched (75%)**; split inventory **27 → 28**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **18 → 1 (94.4% reduced)**.

**Decompiler cleanup:** **17 artifact occurrences reduced**; remaining: `undefined*` **5 → 1** (20% remaining).

<a id="module-ui-frontend-network"></a>
### `ui/frontend/network`

[source](src/ui/frontend/network.cpp) · [header](include/thandor/ui/frontend/network.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-network)


**Recovery impact:** **6/6 owners touched (100%)**; **0 developer-relevant function notes**; **4 semantic signature/type refinements summarized here**; generic artifacts **57 → 2 (96.5% reduced)**.

**Decompiler cleanup:** **58 artifact occurrences reduced**; remaining: `undefined*` **17 → 2** (11.8% remaining).

<a id="module-ui-frontend-player"></a>
### `ui/frontend/player`

[source](src/ui/frontend/player.cpp) · [header](include/thandor/ui/frontend/player.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-player) · [short developer notes](CHANGELOG.md#module-ui-frontend-player)


**Recovery impact:** **37/37 owners touched (100%)**; **1 developer-relevant function note (2.7% of owners)**; **18 semantic signature/type refinements summarized here**; generic artifacts **144 → 1 (99.3% reduced)**.

**Decompiler cleanup:** **148 artifact occurrences reduced**; remaining: `undefined*` **57 → 1** (1.8% remaining).

**Developer notes:**

- `0x0054D3A0` **[`FrontendPlayerSetup_ExpireSelectedRuntimeBlock`](src/ui/frontend/player.cpp#L187)** — structure fix: sparse frontend-network storage is compacted into an explicit opaque gap; the function keeps the same +0x8 access without inventing a false field meaning.

<a id="module-ui-frontend-results"></a>
### `ui/frontend/results`

[source](src/ui/frontend/results.cpp) · [header](include/thandor/ui/frontend/results.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-results)


**Recovery impact:** **12/12 owners touched (100%)**; split inventory **9 → 12**; **0 developer-relevant function notes**; **11 semantic signature/type refinements summarized here**; generic artifacts **148 → 0 (100% reduced)**.

**Decompiler cleanup:** **155 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-ui-frontend-runtime"></a>
### `ui/frontend/runtime`

source · header · call graph · [short developer notes](CHANGELOG.md#module-ui-frontend-runtime)


**Recovery impact:** **44/50 owners touched (88%)**; **7 developer-relevant function notes (14% of owners)**; **15 semantic signature/type refinements summarized here**; generic artifacts **486 → 9 (98.1% reduced)**.

**Decompiler cleanup:** **483 artifact occurrences reduced**; remaining: `extraout_*` **225 → 4** (1.8% remaining) | `undefined*` **201 → 5** (2.5% remaining) | `CONCAT*` **7 → 1** (14.3% remaining).

**Developer notes:**

- `0x0050BC60` **[`FrontendModelPointerContext_RenderWorldViewQueuesClipped`](src/ui/frontend/menu_room.cpp#L337)** — identity: renamed from `FrontendModelPointerContext_DrawClipped`.
- `0x0050CC80` **[`FrontendModelPointerContext_DispatchWorldCameraPointerInput`](src/ui/frontend/menu_room.cpp#L596)** — identity: renamed from `FrontendModelPointerContext_RightDrag`.
- `0x0050CED0` **[`FrontendModelPointerContext_PointerWheel`](src/ui/frontend/menu_room.cpp#L720)** — body/types: frontend model-pointer wheel handling uses the compact recovered pointer-context traversal view.
- `0x0050CF90` **[`FrontendModelPointerContext_Tick`](src/ui/frontend/menu_room.cpp#L770)** — body/types: frontend model-pointer tick handling uses the compact recovered pointer-context traversal view.
- `0x00543B70` **[`FrontendMenu_BindSharedResources`](src/ui/frontend/lifecycle.cpp#L363)** — structure fix: shared-resource binding now addresses task-assignment controls through the compact frontend opaque span plus certified offsets, instead of autogenerated `field_0x...` members.
- `0x005445A0` **`FrontendUiAction2044_IndexedSelectionHelper`** — body/types: indexed frontend selection now uses the recovered level player-slot offset array instead of an untyped global block lookup.
- `0x00546700` **[`Frontend_Init`](src/ui/frontend/lifecycle.cpp#L188)** — structure fix: frontend initialization now operates on compact verified opaque spans for unresolved regions while retaining known callback/state offsets; this removes thousands of misleading sparse auto-fields without claiming new semantics.

<a id="module-ui-frontend-scenario"></a>
### `ui/frontend/scenario`

source · header · call graph · [short developer notes](CHANGELOG.md#module-ui-frontend-scenario)


**Recovery impact:** **2/2 owners touched (100%)**; **1 developer-relevant function note (50% of owners)**; generic artifacts **14 → 1 (92.9% reduced)**.

**Decompiler cleanup:** **13 artifact occurrences reduced**; remaining: `undefined*` **3 → 1** (33.3% remaining).

**Developer notes:**

- `0x00547D60` **[`FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState`](src/ui/frontend/state.cpp#L334)** — structure fix: frontend-root sparse fields are represented as verified opaque spans; existing offsets for movie, page stacks, pointer context and masks are preserved without autogenerated pseudo-fields.

<a id="module-ui-frontend-session"></a>
### `ui/frontend/session`

[source](src/ui/frontend/session.cpp) · [header](include/thandor/ui/frontend/session.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-session)


**Recovery impact:** **14/14 owners touched (100%)**; **0 developer-relevant function notes**; **8 semantic signature/type refinements summarized here**; generic artifacts **47 → 0 (100% reduced)**.

**Decompiler cleanup:** **47 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-ui-frontend-settings"></a>
### `ui/frontend/settings`

[source](src/ui/frontend/settings.cpp) · [header](include/thandor/ui/frontend/settings.h) · [call graph](docs/source_guide/ui.md#file-ui-frontend-settings) · [short developer notes](CHANGELOG.md#module-ui-frontend-settings)


**Recovery impact:** **33/33 owners touched (100%)**; **2 developer-relevant function notes (6.1% of owners)**; **4 semantic signature/type refinements summarized here**; generic artifacts **344 → 12 (96.5% reduced)**.

**Decompiler cleanup:** **334 artifact occurrences reduced**; remaining: `undefined*` **36 → 12** (33.3% remaining).

**Developer notes:**

- `0x00549250` **[`FrontendTaskAssignmentPage_Initialize`](src/ui/frontend/task_assignment.cpp#L32)** — body/types: task-assignment initialization now indexes the recovered seven player-slot offsets from the level-runtime global block and uses named task-page control storage.
- `0x00549620` **[`FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls`](src/ui/frontend/task_assignment.cpp#L279)** — body/types: task-assignment refresh propagates recovered control-offset/state types.

<a id="module-ui-ingame-commands"></a>
### `ui/ingame/commands`

[source](src/ui/ingame/commands.cpp) · [header](include/thandor/ui/ingame/commands.h) · [call graph](docs/source_guide/ui.md#file-ui-ingame-commands)


**Recovery impact:** **65/67 owners touched (97%)**; **0 developer-relevant function notes**; **6 semantic signature/type refinements summarized here**; generic artifacts **127 → 0 (100% reduced)**.

**Decompiler cleanup:** **129 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-ui-ingame-runtime"></a>
### `ui/ingame/runtime`

source · header · call graph · [short developer notes](CHANGELOG.md#module-ui-ingame-runtime)


**Recovery impact:** **31/33 owners touched (93.9%)**; **4 developer-relevant function notes (12.1% of owners)**; **4 semantic signature/type refinements summarized here**; generic artifacts **402 → 12 (97.0% reduced)**.

**Decompiler cleanup:** **444 artifact occurrences reduced**; remaining: `unaff_*` **6 → 8** (33.3% increase) | `undefined*` **58 → 4** (6.9% remaining).

**Developer notes:**

- `0x0050ECE0` **`InGameUiAction1210_ResourceRegistrationHelper`** — body/types: the resource-registration helper now uses typed level header/resource-table storage and the correctly named `ResourceRegistrationRuntime_RebaseLoadedRecords` owner.
- `0x005609F0` **[`InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState`](src/ui/ingame/editor_tools.cpp#L1075)** — body/types: interaction-subsystem state now uses the recovered resource-extraction descriptor field instead of a generic runtime cell dword.
- `0x00569780` **[`InGameUiRuntime_DispatchWorldContextActionCallback`](src/ui/ingame/key_commands.cpp#L374)** — body/types: world-context action dispatch uses recovered callback/runtime types instead of opaque callback storage.
- `0x0056A8A0` **`InGameUiAction101F_Handler`** — body/types: UI action 0x101F now reads the typed level header/title text resource through recovered level storage rather than a condition-runtime alias.

<a id="module-ui-ingame-settings"></a>
### `ui/ingame/settings`

[source](src/ui/ingame/settings.cpp) · [header](include/thandor/ui/ingame/settings.h) · [call graph](docs/source_guide/ui.md#file-ui-ingame-settings)


**Recovery impact:** **30/30 owners touched (100%)**; **0 developer-relevant function notes**; **7 semantic signature/type refinements summarized here**; generic artifacts **223 → 0 (100% reduced)**.

**Decompiler cleanup:** **227 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-ui-ingame-technology"></a>
### `ui/ingame/technology`

[source](src/ui/ingame/technology.cpp) · [header](include/thandor/ui/ingame/technology.h) · [call graph](docs/source_guide/ui.md#file-ui-ingame-technology)


**Recovery impact:** **6/6 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **63 → 1 (98.4% reduced)**.

**Decompiler cleanup:** **65 artifact occurrences reduced**; remaining: `undefined*` **12 → 1** (8.3% remaining).

<a id="module-ui-support-runtime"></a>
### `ui/support/runtime`

source · header · call graph


**Recovery impact:** **6/6 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **41 → 3 (92.7% reduced)**.

**Decompiler cleanup:** **38 artifact occurrences reduced**; remaining: `undefined*` **29 → 3** (10.3% remaining).

<a id="module-world-effects-maintenance"></a>
### `world/effects/maintenance`

source · header · call graph


**Recovery impact:** **4/4 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **61 → 28 (54.1% reduced)**.

**Decompiler cleanup:** **33 artifact occurrences reduced**; remaining: `undefined*` **40 → 28** (70% remaining) | `CONCAT*` **63 → 63** (100% remaining).

<a id="module-world-effects-runtime"></a>
### `world/effects/runtime`

source · [header](include/thandor/world/effects/runtime.h) · [call graph](docs/source_guide/world.md#file-world-effects-runtime)


**Recovery impact:** **5/5 owners touched (100%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **30 → 0 (100% reduced)**.

**Decompiler cleanup:** **32 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-world-model-hierarchy"></a>
### `world/model/hierarchy`

[source](src/world/model/hierarchy.cpp) · [header](include/thandor/world/model/hierarchy.h) · [call graph](docs/source_guide/world.md#file-world-model-hierarchy) · [short developer notes](CHANGELOG.md#module-world-model-hierarchy)


**Recovery impact:** **28/29 owners touched (96.6%)**; split inventory **28 → 29**; **4 developer-relevant function notes (13.8% of owners)**; **14 semantic signature/type refinements summarized here**; generic artifacts **199 → 19 (90.5% reduced)**.

**Decompiler cleanup:** **191 artifact occurrences reduced**; remaining: `unaff_*` **18 → 16** (88.9% remaining) | `undefined*` **46 → 3** (6.5% remaining) | `CONCAT*` **21 → 10** (47.6% remaining).

**Developer notes:**

- `0x004BEB80` **`ModelNodeRuntime_TransformLocalPointRegs`** — body/types: local-point transformation uses recovered composite point/result types.
- `0x004BEBC0` **[`ModelNodeRuntime_ComputeRelativeDirectionAngle`](src/world/model/hierarchy.cpp#L314)** — fix: the old `uint` return exposed only one angle; V537 recovers the two-register `ModelRelativeDirectionAnglesEaxEdx8` result so both relative-direction angles survive the call boundary.
- `0x0052AAC0` **[`ModelNodeRuntime_SmoothYawTowardTarget`](src/world/model/gameplay_rules.cpp#L228)** — body/types: yaw smoothing now uses `ModelRuntimeWeaponAimStateView200`; yaw velocity plus rate/acceleration limits are named and typed.
- `0x0052AC00` **[`ModelNodeRuntime_SmoothPitchTowardTarget`](src/world/model/gameplay_rules.cpp#L313)** — body/types: pitch smoothing now uses `ModelRuntimeWeaponAimStateView200`; pitch velocity, min/max pitch and rate/acceleration limits are named and typed.

<a id="module-world-model-runtime"></a>
### `world/model/runtime`

source · [header](include/thandor/world/model/runtime.h) · [call graph](docs/source_guide/world.md#file-world-model-runtime)


**Recovery impact:** **15/15 owners touched (100%)**; **0 developer-relevant function notes**; **7 semantic signature/type refinements summarized here**; generic artifacts **122 → 4 (96.7% reduced)**.

**Decompiler cleanup:** **123 artifact occurrences reduced**; remaining: `in_EAX` **4 → 3** (75% remaining) | `undefined*` **28 → 1** (3.6% remaining) | `CONCAT*` **6 → 1** (16.7% remaining).

<a id="module-world-model-slots"></a>
### `world/model/slots`

[source](src/world/model/slots.cpp) · [header](include/thandor/world/model/slots.h) · [call graph](docs/source_guide/world.md#file-world-model-slots) · [short developer notes](CHANGELOG.md#module-world-model-slots)


**Recovery impact:** **15/20 owners touched (75%)**; **2 developer-relevant function notes (10% of owners)**; generic artifacts **10 → 0 (100% reduced)**.

**Decompiler cleanup:** **10 artifact occurrences reduced**; remaining: `CONCAT*` **1 → 1** (100% remaining).

**Developer notes:**

- `0x005251C0` **`ModelRuntimeSlotClassInit_AddFactionResourceStorageCapacityAndDetachRootChild3`** — identity/fix: the generic faction-metric name obscured class-14 resource semantics. The owner now adds the recovered resource-storage-capacity contribution and handles the resource-field support selector before detaching root child 3.
- `0x00525250` **`ModelRuntimeSlotClassInit_AddFactionResourceStorageCapacityAndDetachRootChild1`** — identity/fix: the generic faction-metric name obscured class-15 resource semantics. The owner now explicitly adds resource-storage capacity using the recovered storage selector before detaching root child 1.

<a id="module-world-motion-runtime"></a>
### `world/motion/runtime`

source · header · call graph · [short developer notes](CHANGELOG.md#module-world-motion-runtime)


**Recovery impact:** **11/11 owners touched (100%)**; **6 developer-relevant function notes (54.5% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **59 → 0 (100% reduced)**.

**Decompiler cleanup:** **59 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x0050C920` **[`WorldMotion_AdjustHeadingAndRecomputePosition`](src/world/camera/camera_input.cpp#L179)** — body/types: recovered world-motion constants/globals are used directly by heading/position adjustment.
- `0x0050C990` **[`WorldMotion_AdjustHeadingAndClearFieldGridDirty`](src/world/camera/camera_input.cpp#L203)** — body/types: recovered world-motion constants/globals and field-grid dirty-state semantics replace opaque scalar accesses.
- `0x0050C9C0` **[`WorldMotion_AdjustDistanceClampAndRecomputePosition`](src/world/camera/camera_input.cpp#L219)** — body/types: distance-clamp motion uses recovered world-motion constants and typed position state.
- `0x0050CA80` **[`WorldMotion_AdjustPositionMagnitudeClamp`](src/world/camera/camera_input.cpp#L264)** — body/types: magnitude-clamp motion uses recovered world-motion constants/state.
- `0x0050CB10` **[`WorldMotion_AdjustPitchClampAndRecomputePosition`](src/world/camera/camera_input.cpp#L303)** — body/types: pitch-clamp/recompute uses recovered world-motion constants/state.
- `0x0050CBE0` **[`WorldMotion_AdjustPitchClampAndClearFieldGridDirty`](src/world/camera/camera_input.cpp#L350)** — body/types: pitch-clamp/dirty-state path uses recovered world-motion constants and field-grid state.

<a id="module-world-pathing-grid"></a>
### `world/pathing/grid`

source · [header](include/thandor/world/pathing/grid.h) · [call graph](docs/source_guide/world.md#file-world-pathing-grid) · [short developer notes](CHANGELOG.md#module-world-pathing-grid)


**Recovery impact:** **25/27 owners touched (92.6%)**; **4 developer-relevant function notes (14.8% of owners)**; **6 semantic signature/type refinements summarized here**; generic artifacts **220 → 6 (97.3% reduced)**.

**Decompiler cleanup:** **228 artifact occurrences reduced**; remaining: `param_N` **3 → 3** (100% remaining) | `unaff_*` **90 → 2** (2.2% remaining) | `undefined*` **22 → 1** (4.5% remaining).

**Developer notes:**

- `0x00534960` **[`GridPathRegion_MarkUnreachableFromCell`](src/world/pathing/reachability.cpp#L142)** — body/types: unreachable-region marking propagates recovered grid/path record types.
- `0x005349D0` **[`EntityPathing_ResolveDestinationAndRebuildRoutes`](src/world/pathing/route.cpp#L53)** — body/types: route rebuilding uses recovered destination/backtrack result contracts rather than scalar register fragments.
- `0x005369A0` **[`EntityPathing_UpdateRouteSegment`](src/world/pathing/route.cpp#L553)** — body/types: route-segment update propagates recovered route/grid state types.
- `0x00536C90` **[`EntityPathing_RebuildOverlappingGroupRoutes`](src/world/pathing/route.cpp#L272)** — body/types: overlapping-group route rebuilding now uses named near/far model depth-bin masks in world-owner filtering.

<a id="module-world-pathing-influence"></a>
### `world/pathing/influence`

[source](src/world/pathing/influence.cpp) · [header](include/thandor/world/pathing/influence.h) · [call graph](docs/source_guide/world.md#file-world-pathing-influence)


**Recovery impact:** **19/19 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **72 → 0 (100% reduced)**.

**Decompiler cleanup:** **72 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

<a id="module-world-runtime-core"></a>
### `world/runtime/core`

[source](src/world/runtime/core.cpp) · [header](include/thandor/world/runtime/core.h) · [call graph](docs/source_guide/world.md#file-world-runtime-core) · [short developer notes](CHANGELOG.md#module-world-runtime-core)


**Recovery impact:** **41/50 owners touched (82%)**; **4 developer-relevant function notes (8% of owners)**; **15 semantic signature/type refinements summarized here**; generic artifacts **105 → 25 (76.2% reduced)**.

**Decompiler cleanup:** **86 artifact occurrences reduced**; remaining: `undefined*` **67 → 25** (37.3% remaining) | `CONCAT*` **187 → 181** (96.8% remaining).

**Developer notes:**

- `0x004BE760` **`WorldRuntime_InterpolateTerrainHeightOrSentinel`** — body/types: terrain-height interpolation uses the recovered world/field-grid semantic types.
- `0x004BE790` **`WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel`** — body/types: water-surface interpolation uses the recovered world/field-grid semantic types.
- `0x004BE7C0` **[`WorldRuntime_InterpolateTopSurfaceHeightOrSentinel`](src/world/runtime/core.cpp#L41)** — body/types: top-surface interpolation uses the recovered world/field-grid semantic types.
- `0x00532FA0` **[`WorldLightingRuntime_UpdateInterpolatedTerrainLighting`](src/world/terrain/field_lighting.cpp#L396)** — body/types: terrain-light interpolation now reads typed level-runtime-tail fields for cycle duration, base/ramp colors, alternate colors and lighting key colors instead of anonymous condition-runtime offsets.

<a id="module-world-shots-maintenance"></a>
### `world/shots/maintenance`

source · header · call graph


**Recovery impact:** **4/4 owners touched (100%)**; **0 developer-relevant function notes**; generic artifacts **81 → 11 (86.4% reduced)**.

**Decompiler cleanup:** **70 artifact occurrences reduced**; remaining: `undefined*` **19 → 11** (57.9% remaining) | `CONCAT*` **21 → 21** (100% remaining).

<a id="module-world-shots-runtime"></a>
### `world/shots/runtime`

source · [header](include/thandor/world/shots/runtime.h) · [call graph](docs/source_guide/world.md#file-world-shots-runtime)


**Recovery impact:** **7/7 owners touched (100%)**; **0 developer-relevant function notes**; **2 semantic signature/type refinements summarized here**; generic artifacts **50 → 11 (78% reduced)**.

**Decompiler cleanup:** **41 artifact occurrences reduced**; remaining: `undefined*` **24 → 11** (45.8% remaining) | `CONCAT*` **23 → 21** (91.3% remaining).

<a id="module-world-terrain-editing"></a>
### `world/terrain/editing`

[source](src/world/terrain/editing.cpp) · [header](include/thandor/world/terrain/editing.h) · [call graph](docs/source_guide/world.md#file-world-terrain-editing) · [short developer notes](CHANGELOG.md#module-world-terrain-editing)


**Recovery impact:** **11/11 owners touched (100%)**; **1 developer-relevant function note (9.1% of owners)**; **4 semantic signature/type refinements summarized here**; generic artifacts **76 → 0 (100% reduced)**.

**Decompiler cleanup:** **80 artifact occurrences reduced**; **0 tracked artifact occurrences remain**.

**Developer notes:**

- `0x00513790` **[`TerrainRegionCollection_RecordConnectedCell`](src/world/terrain/editing.cpp#L363)** — body/types: connected-cell recording now names the field-grid resource-extraction descriptor instead of generic `runtime7C` storage.

<a id="module-world-terrain-grid"></a>
### `world/terrain/grid`

source · [header](include/thandor/world/terrain/grid.h) · [call graph](docs/source_guide/world.md#file-world-terrain-grid)


**Recovery impact:** **55/56 owners touched (98.2%)**; **0 developer-relevant function notes**; **11 semantic signature/type refinements summarized here**; generic artifacts **421 → 17 (96.0% reduced)**.

**Decompiler cleanup:** **418 artifact occurrences reduced**; remaining: `extraout_*` **331 → 12** (3.6% remaining) | `undefined*` **39 → 5** (12.8% remaining) | `CONCAT*` **20 → 6** (30% remaining).

<a id="module-world-terrain-height"></a>
### `world/terrain/height`

source · header · call graph


**Recovery impact:** **40/40 owners touched (100%)**; **0 developer-relevant function notes**; **9 semantic signature/type refinements summarized here**; generic artifacts **217 → 0 (100% reduced)**.

**Decompiler cleanup:** **238 artifact occurrences reduced**; remaining: `CONCAT*` **29 → 8** (27.6% remaining).

<a id="module-world-terrain-occupancy"></a>
### `world/terrain/occupancy`

[source](src/world/terrain/occupancy.cpp) · [header](include/thandor/world/terrain/occupancy.h) · [call graph](docs/source_guide/world.md#file-world-terrain-occupancy)


**Recovery impact:** **15/15 owners touched (100%)**; **0 developer-relevant function notes**; **1 semantic signature/type refinement summarized here**; generic artifacts **44 → 2 (95.5% reduced)**.

**Decompiler cleanup:** **43 artifact occurrences reduced**; remaining: `undefined*` **2 → 2** (100% remaining) | `CONCAT*` **15 → 14** (93.3% remaining).

<a id="module-world-terrain-projection"></a>
### `world/terrain/projection`

source · header · call graph · [short developer notes](CHANGELOG.md#module-world-terrain-projection)


**Recovery impact:** **45/45 owners touched (100%)**; **2 developer-relevant function notes (4.4% of owners)**; **6 semantic signature/type refinements summarized here**; generic artifacts **632 → 67 (89.4% reduced)**.

**Decompiler cleanup:** **571 artifact occurrences reduced**; remaining: `undefined*` **148 → 67** (45.3% remaining) | `CONCAT*` **162 → 156** (96.3% remaining).

**Developer notes:**

- `0x00500D30` **[`TerrainProjectedGrid_ClipRowSpansAgainstPlane`](src/graphics/terrain/terrain_render.cpp#L659)** — body/types: the projected-grid scratch area is recovered as 260 typed row-span records instead of a flat opaque buffer.
- `0x00500F50` **[`TerrainProjectedGrid_TransformShadeAndQueue`](src/graphics/terrain/terrain_render.cpp#L89)** — body/types: transform/shade/queue consumes the same typed 260-row projected span workspace.

<a id="module-world-terrain-visuals"></a>
### `world/terrain/visuals`

source · header · call graph · [short developer notes](CHANGELOG.md#module-world-terrain-visuals)


**Recovery impact:** **11/11 owners touched (100%)**; **1 developer-relevant function note (9.1% of owners)**; **2 semantic signature/type refinements summarized here**; generic artifacts **82 → 16 (80.5% reduced)**.

**Decompiler cleanup:** **70 artifact occurrences reduced**; remaining: `undefined*` **28 → 16** (57.1% remaining) | `CONCAT*` **83 → 79** (95.2% remaining).

**Developer notes:**

- `0x0053D840` **[`TerrainCompositeTexture_RebuildPlane0`](src/graphics/terrain/minimap_composite.cpp#L353)** — body/types: terrain composite rebuilding now reads the named model tint ARGB field instead of an anonymous world-owner scalar.
