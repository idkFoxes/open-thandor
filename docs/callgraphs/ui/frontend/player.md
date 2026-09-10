# Call graph: `ui/frontend/player`

37 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00548CD0` **FrontendPlayerMessage_SubmitSevenSlotText** — local: `FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById`, `FrontendPlayerMessageBuffer_AppendTripleById`, `FrontendPlayerMessageBuffer_PublishTextById`; cross: `UiTextControl_UpdateNonEmptyValidity` → `ui/controls/text`, `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`, `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x00560750` **FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel** — local: `FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection`, `FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80`; cross: `UiPageStack_SetActiveIndex` → `ui/controls/layout`, `InGameTechnologyPanel_ResetAndSelectCurrentArea` → `ui/ingame/technology`
- `0x00549AF0` **FrontendPlayerConsensus_SubmitSelectedValue** — local: `FrontendPlayerRuntime_SetConsensusValueAndRefresh`; cross: `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x0054D3A0` **FrontendPlayerSetup_ExpireSelectedRuntimeBlock** — local: `FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks`
- `0x0054F540` **FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers** — local: `FrontendPlayerRuntime_RecordReadyAndUpdateWaitState`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `FrontendRecentTextHistory_InsertAndRebuild5` → `ui/frontend/runtime`, `UiTransfer_StagePacketAndSendCf` → `network/protocol/transfer`
- `0x00514EF0` **FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf**
- `0x00514F60` **FrontendPlayerRuntime_ClearAssignmentTokenFromAll**
- `0x00544130` **FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08**
- `0x005442B0` **FrontendPlayerRuntime_MarkFlag08ById**
- `0x00544300` **FrontendPlayerRuntime_XorStateMaskByPlayerId**
- `0x00544360` **FrontendPlayerRuntime_MarkFlag04ById**
- `0x00544820` **FrontendPlayerRuntime_MarkFlag02ById**
- `0x00544D50` **FrontendPlayerRuntime_MarkFlag01AndStoreValuesById**
- `0x00549190` **FrontendPlayerRuntime_InitializeFactionAssignments**
- `0x0054D000` **FrontendPlayerSetup_OpenLocalPageAndResetRoster** — cross: `UiPageStack_SetActiveIndex` → `ui/controls/layout`
- `0x0054D1B0` **FrontendPlayerSetup_SelectCountAndBuildLabel** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_CopyExpandedCf` → `assets/text/richtext`
- `0x0054D720` **FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction** — cross: `UiNodeList_SuppressActionId` → `ui/controls/lists`, `UiNodeList_UnsuppressActionId` → `ui/controls/lists`
- `0x0055F470` **FrontendPlayerRuntime_SetReadyFlagById**
- `0x0055F5A0` **FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B** — cross: `UiNodeList_UnsuppressActionId` → `ui/controls/lists`, `UiNodeList_SuppressActionId` → `ui/controls/lists`
- `0x0055F680` **FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus** — cross: `InGameCommandQueue_AppendLocalPlayerCommand` → `network/protocol/commands`
- `0x0055FB90` **FrontendPlayerSelection_InsertThreeEntriesAndRefresh** — cross: `SelectionPointerArray_InsertUniqueAndRecenter` → `gameplay/selection/runtime`, `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`
- `0x0055FC30` **FrontendPlayerSelection_RemoveThreeEntriesAndRefresh** — cross: `SelectionPointerArray_RemoveFirstMatch` → `gameplay/selection/runtime`, `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`
- `0x0055FCD0` **FrontendPlayerSelection_ClearAndRefreshLocalPanels** — cross: `SelectionPointerArray_Clear32` → `gameplay/selection/runtime`, `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`
- `0x0055FD10` **FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh** — cross: `SelectionPointerArray_RecenterOffsetsAroundAveragePosition` → `gameplay/selection/runtime`, `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`, `SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf` → `gameplay/selection/runtime`, `WorldRuntime_SetPosition80AndRebuildPosition60FromAngles` → `world/runtime/core`
- `0x00560830` **FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology** — cross: `Technology_ApplyRecordToEntity` → `gameplay/technology/runtime`
- `0x005608A0` **FrontendPlayerTextCommand_SetPackedState**
- `0x005608D0` **FrontendPlayerTextCommand_AppendTripleClamped**
- `0x00560940` **FrontendPlayerTextCommand_PublishConditionalRichText** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`, `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `InGameRecentTextHistory_InsertAndRebuild8` → `ui/ingame/runtime`
- `0x00561F10` **FrontendPlayerSelection_ApplyEntryOrAll** — cross: `SelectionPointerArray_ContainsCf` → `gameplay/selection/runtime`, `ArmyRuntime_DestroyInstanceAndRefreshUi` → `gameplay/army/runtime`
- `0x00544020` **FrontendPlayerRuntime_RecordReadyAndUpdateWaitState** — cross: `FrontendCommandQueue_EnqueueLocalPlayerCommand` → `network/protocol/commands`
- `0x00544770` **FrontendPlayerRuntime_SetConsensusValueAndRefresh** — cross: `FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls` → `ui/frontend/settings`
- `0x00545490` **FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById**
- `0x00545500` **FrontendPlayerMessageBuffer_AppendTripleById**
- `0x00545590` **FrontendPlayerMessageBuffer_PublishTextById** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`, `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `FrontendRecentTextHistory_InsertAndRebuild5` → `ui/frontend/runtime`
- `0x0054EBD0` **FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks** — cross: `UiPointerList_RefreshSelectionAndQueueAction` → `ui/controls/lists`
- `0x0055FAD0` **FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection** — cross: `SelectionPointerArray_Clear32` → `gameplay/selection/runtime`, `SelectionPointerArray_InsertUniqueAndRecenter` → `gameplay/selection/runtime`, `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`
- `0x005607E0` **FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80**
