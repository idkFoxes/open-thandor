# Call graph: `core/math/random`

[Source](../../../../src/core/math/random.c) · [Header](../../../../include/thandor/core/math/random.h) · [Graphviz](random.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-core-math-random)

6 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004030B0` **[`Random_NextPrimary`](../../../../src/core/math/random.c#L5)**
- `0x004030E0` **[`Random_NextSecondary`](../../../../src/core/math/random.c#L21)**
- `0x00403110` **[`Random_SetBothSeeds`](../../../../src/core/math/random.c#L36)**
- `0x00403130` **[`Random_GetSecondarySeed`](../../../../src/core/math/random.c#L48)**
- `0x00403140` **[`Random_SelectSecondaryStream`](../../../../src/core/math/random.c#L59)**
- `0x00403150` **[`Random_SelectPrimaryStream`](../../../../src/core/math/random.c#L70)**

## Called by

- [`gameplay/session/runtime`](../../gameplay/session/runtime.md): [`EndGameResultsUiRuntime_UpdateAndHandleInputCf`](../../../../src/gameplay/session/runtime.c#L87) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
- [`movie/runtime/playback`](../../movie/runtime/playback.md): [`Movie_Open`](../../../../src/movie/runtime/playback.c#L129) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
- [`network/backend/runtime`](../../network/backend/runtime.md): [`FrontendNetwork_HandleHandshakeAndPlayerStatePackets`](../../../../src/network/backend/runtime.c#L5) → [`Random_GetSecondarySeed`](../../../../src/core/math/random.c#L48)
- [`network/protocol/transfer`](../../network/protocol/transfer.md): [`FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`](../../../../src/network/protocol/transfer.c#L340) → [`Random_SetBothSeeds`](../../../../src/core/math/random.c#L36); [`FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf`](../../../../src/network/protocol/transfer.c#L340) → [`Random_SelectSecondaryStream`](../../../../src/core/math/random.c#L59); [`UiTransferMailbox_RandomizeSequenceToken`](../../../../src/network/protocol/transfer.c#L1168) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
- [`ui/controls/buttons`](../../ui/controls/buttons.md): [`UiSpriteButtonControl_Relocate`](../../../../src/ui/controls/buttons.c#L37) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
- [`ui/controls/input`](../../ui/controls/input.md): [`UiPointer_DispatchPendingEvents`](../../../../src/ui/controls/input.c#L5) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
- [`ui/frontend/runtime`](../../ui/frontend/runtime.md): [`FrontendUiAction200F_Handler`](../../../../src/ui/frontend/runtime.c#L2473) → [`Random_SelectPrimaryStream`](../../../../src/core/math/random.c#L70)
- [`ui/frontend/session`](../../ui/frontend/session.md): [`FrontendSessionAction_ResetNetworkAndReturnToMainPage`](../../../../src/ui/frontend/session.c#L103) → [`Random_SelectPrimaryStream`](../../../../src/core/math/random.c#L70); [`FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag`](../../../../src/ui/frontend/session.c#L130) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5); [`FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag`](../../../../src/ui/frontend/session.c#L130) → [`Random_SetBothSeeds`](../../../../src/core/math/random.c#L36); [`FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag`](../../../../src/ui/frontend/session.c#L130) → [`Random_SelectSecondaryStream`](../../../../src/core/math/random.c#L59)
- [`world/terrain/grid`](../../world/terrain/grid.md): [`FieldGrid_InitializeRuntimeCellsAndBoundaryFlags`](../../../../src/world/terrain/grid.c#L1936) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
- [`world/terrain/visuals`](../../world/terrain/visuals.md): [`TerrainVisualResources_LoadPrimary`](../../../../src/world/terrain/visuals.c#L228) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5); [`TerrainVisualResources_LoadAndClearCellOverlayFlags`](../../../../src/world/terrain/visuals.c#L400) → [`Random_NextPrimary`](../../../../src/core/math/random.c#L5)
