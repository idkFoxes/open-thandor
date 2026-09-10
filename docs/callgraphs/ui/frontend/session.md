# Call graph: `ui/frontend/session`

14 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00544270` **FrontendSession_ReleaseSelectedResourceAndReturnToMainPage** — local: `FrontendSession_ReturnToMainPage`; cross: `Resource_Release` → `assets/resource/runtime`
- `0x00548FE0` **FrontendSessionAction_CloseMovieAndReturnToMainPage** — local: `FrontendSession_ReturnToMainPage`; cross: `Movie_Close` → `movie/runtime/playback`, `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x00549090` **FrontendSessionAction_ApplySpeedOrToggleReady** — local: `FrontendSession_ApplyGameSpeedAndReturnToMainPage`; cross: `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`, `FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08` → `ui/frontend/player`
- `0x0054C770` **FrontendSessionAction_ResetNetworkAndReturnToMainPage** — local: `FrontendSession_ReturnToMainPage`; cross: `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`, `Random_SelectPrimaryStream` → `core/math/random`
- `0x0054D0B0` **FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag** — local: `FrontendSession_ReturnToMainPage`; cross: `Random_NextPrimary` → `core/math/random`, `Random_SetBothSeeds` → `core/math/random`, `Random_SelectSecondaryStream` → `core/math/random`, `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x00544250` **FrontendSession_SetGameSpeedPercent**
- `0x0054A790` **FrontendSession_ShowPage9WithCompactLayout** — cross: `UiPageStack_SetActiveIndex` → `ui/controls/layout`
- `0x0054D2E0` **FrontendTransferPage_ResetSessionOpenAndRequestMailbox** — cross: `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `UiNodeList_SuppressActionId` → `ui/controls/lists`, `UiPointerList_InitializeColumnLayout` → `ui/controls/lists`, `UiTransferMailbox_RandomizeSequenceToken` → `network/protocol/transfer`, `UiTransfer_SendPacketType10000Value2931Cf` → `network/protocol/transfer`
- `0x0054E3A0` **FrontendSessionList_DecrementExpiryAndCompactRows** — cross: `UiPointerList_RefreshSelectionAndQueueAction` → `ui/controls/lists`
- `0x00565670` **FrontendSession_PeriodicTick** — cross: `FrontendTransfer_DispatchStagedCommandRecords` → `network/protocol/transfer`, `UiRuntimeRecordRing_DiscardOldestCf` → `ui/core/runtime`, `FrontendTransfer_HandleSyncRequest10021AndReply10023` → `network/protocol/transfer`, `FrontendTransfer_BroadcastPendingCommandBatchAndSyncState` → `network/protocol/transfer`, `UiRuntimeRecordRing_ContainsIdCf` → `ui/core/runtime`, `FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf` → `network/backend/runtime`, `FrontendTransfer_ConsumeProcessedFlagCf` → `network/protocol/transfer`
- `0x005725D0` **FrontendClientSession_DecrementTimeoutsAndCompactPlayers** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `InGameRecentTextHistory_InsertAndRebuild8` → `ui/ingame/runtime`, `UiTransfer_StagePacketAndSendCf` → `network/protocol/transfer`, `FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus` → `ui/frontend/player`
- `0x00572960` **FrontendHostSession_TickShutdownOrReadyConsensus** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `InGameRecentTextHistory_InsertAndRebuild8` → `ui/ingame/runtime`, `FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus` → `ui/frontend/player`, `InGameCommandQueue_AppendLocalPlayerCommand` → `network/protocol/commands`
- `0x00544210` **FrontendSession_ApplyGameSpeedAndReturnToMainPage** — local: `FrontendSession_ReturnToMainPage`; cross: `Movie_Close` → `movie/runtime/playback`
- `0x00544D10` **FrontendSession_ReturnToMainPage** — cross: `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `FrontendState_DispatchCode` → `ui/frontend/runtime`
