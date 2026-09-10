# Call graph: `network/protocol/transfer`

27 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004AEB10` **UiTransferMailbox_ServiceAndRetransmitTimer** — local: `UiTransferBlock_Transform64BitBlocksWithRoundKeys16`, `UiTransfer_StagePacketAndSendCf`
- `0x0054ECB0` **FrontendTransfer_HandleHostSessionAndCommandBatchPackets** — local: `FrontendTransfer_SendQueued10011AndOptional10004`, `UiTransfer_StagePacketAndSendCf`; cross: `UiPointerList_InitializeColumnLayout` → `ui/controls/lists`, `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `FrontendState_DispatchCode` → `ui/frontend/runtime`, `FrontendCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`
- `0x0054F680` **FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf** — local: `UiTransfer_StagePacketAndSendCf`, `FrontendTransfer_SendQueued10011AndOptional10004`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `FrontendRecentTextHistory_InsertAndRebuild5` → `ui/frontend/runtime`, `Random_SetBothSeeds` → `core/math/random`, `Random_SelectSecondaryStream` → `core/math/random`
- `0x00545640` **FrontendTransfer_MarkUnavailableIfModeBit0Callback** — local: `UiTransferMailbox_MarkUnavailable`
- `0x00545660` **FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady** — local: `UiTransferMailbox_SetOutgoingBuffer`
- `0x0054E230` **UiTransfer_SendPacketType10000Value2931Cf** — local: `UiTransfer_StagePacketAndSendCf`
- `0x0054E470` **UiTransfer_SendPlayerDescriptorPacket20002Cf** — local: `UiTransfer_StagePacketAndSendCf`; cross: `PcxPreview_Load64x64PaletteAndPixelsCf` → `ui/support/runtime`
- `0x0054E4E0` **FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets** — local: `UiTransfer_StagePacketAndSendCf`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `RichTextCommandStream_CopyExpandedCf` → `assets/text/richtext`, `FrontendCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`, `FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction` → `ui/frontend/player`
- `0x0054E9B0` **FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands** — local: `UiTransfer_StagePacketAndSendCf`; cross: `FrontendCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`
- `0x0054EEF0` **FrontendTransfer_SendPacket10006** — local: `UiTransfer_StagePacketAndSendCf`
- `0x005723F0` **FrontendTransfer_BroadcastPendingCommandBatchAndSyncState** — local: `UiTransfer_StagePacketAndSendCf`; cross: `InGameCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`
- `0x00572920` **FrontendTransfer_SendCommandBatchRequest10021** — local: `UiTransfer_StagePacketAndSendCf`; cross: `InGameCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`
- `0x004AF110` **UiTransferMailbox_ClearReceivedState**
- `0x004AF170` **UiTransferMailbox_GetReceivedBufferCf**
- `0x004AF1C0` **UiTransferMailbox_RandomizeSequenceToken** — cross: `Random_NextPrimary` → `core/math/random`
- `0x0054E260` **FrontendTransfer_HandleSessionListAndJoinAckPackets** — cross: `UiPointerList_RefreshSelectionAndQueueAction` → `ui/controls/lists`, `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `UiPointerList_InitializeColumnLayout` → `ui/controls/lists`
- `0x0054EF30` **FrontendTransfer_TickRequestTimeoutAndResetPage** — cross: `FrontendTransferPage_ResetSessionOpenAndRequestMailbox` → `ui/frontend/session`
- `0x0054FBA0` **FrontendTransfer_ConsumeProcessedFlagFrontendCf**
- `0x005722C0` **FrontendTransfer_HandleSyncRequest10021AndReply10023**
- `0x00572560` **FrontendTransfer_DispatchStagedCommandRecords**
- `0x00572AA0` **FrontendTransfer_ConsumeProcessedFlagCf**
- `0x00407160` **UiTransfer_TransformPacketBlocks**
- `0x004072F0` **UiTransferBlock_Transform64BitBlocksWithRoundKeys16**
- `0x004AF140` **UiTransferMailbox_MarkUnavailable**
- `0x004AF1A0` **UiTransferMailbox_SetOutgoingBuffer**
- `0x0054F9A0` **FrontendTransfer_SendQueued10011AndOptional10004** — local: `UiTransfer_StagePacketAndSendCf`; cross: `FrontendCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`
- `0x004AEF70` **UiTransfer_StagePacketAndSendCf** — local: `UiTransfer_TransformPacketBlocks`
