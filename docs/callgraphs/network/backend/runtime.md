# Call graph: `network/backend/runtime`

8 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0054EF60` **FrontendNetwork_HandleHandshakeAndPlayerStatePackets** — cross: `Random_GetSecondarySeed` → `core/math/random`, `UiTransfer_StagePacketAndSendCf` → `network/protocol/transfer`
- `0x0054F240` **FrontendNetwork_HostTickCommandAndSnapshotTransfer** — cross: `UiTransfer_StagePacketAndSendCf` → `network/protocol/transfer`, `FrontendCommandQueue_DequeueFirstIntoRecord` → `network/protocol/commands`, `PckCodec_EncodeHuffmanRle` → `assets/package/codec`, `UiTransferMailbox_SetOutgoingBuffer` → `network/protocol/transfer`, `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x0054FA10` **FrontendNetwork_TickDisconnectTimeoutAndResetSession** — cross: `UiTransferMailbox_ClearReceivedState` → `network/protocol/transfer`, `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `FrontendRomTransition_ActivateRecordByIdCf` → `assets/rom/runtime`, `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `FrontendRecentTextHistory_InsertAndRebuild5` → `ui/frontend/runtime`, `FrontendPlayerRuntime_RecordReadyAndUpdateWaitState` → `ui/frontend/player`, `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x00572710` **FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf** — cross: `FrontendTransfer_SendCommandBatchRequest10021` → `network/protocol/transfer`, `UiTransfer_StagePacketAndSendCf` → `network/protocol/transfer`, `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `InGameRecentTextHistory_InsertAndRebuild8` → `ui/ingame/runtime`
- `0x00584080` **Network_Init** — cross: `DynDLL_Load` → `platform/bootstrap/runtime`, `DynAPI_Resolve` → `platform/bootstrap/runtime`
- `0x00584DF0` **Network_Shutdown**
- `0x00584E50` **NetworkBackend_SetSessionContextCf**
- `0x00585210` **NetworkBackend_SelectInstanceByIndex**
