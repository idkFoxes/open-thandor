/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_COMMANDS_H
#define THANDOR_NETWORK_PROTOCOL_COMMANDS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/commands. */

/* In-game command codes are handler addresses relative to InGameCommandQueue_AppendLocalPlayerCommand
   (0x0055F130); a command is executed by calling INGAME_COMMAND_CODE_BASE + code. Single player calls the
   named handler directly instead of queueing the code. */
#define INGAME_COMMAND_CODE_BASE 0x0055F130
#define INGAME_COMMAND_SELECT_SINGLE_ARMY 0x9A0 /* FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection */
#define INGAME_COMMAND_REPLACE_SELECTION 0xA00 /* InGamePlayerSelection_ReplaceWithArmyRuntimeIndex */
#define INGAME_COMMAND_SELECTION_INSERT 0xA60 /* FrontendPlayerSelection_InsertThreeEntriesAndRefresh */
#define INGAME_COMMAND_SELECTION_REMOVE 0xB00 /* FrontendPlayerSelection_RemoveThreeEntriesAndRefresh */
#define INGAME_COMMAND_SELECTION_CLEAR 0xBA0 /* FrontendPlayerSelection_ClearAndRefreshLocalPanels */
#define INGAME_COMMAND_POSITION_VARIANT_B 0xD40 /* InGamePlayerSelection_ApplyPositionCommandVariantB */
#define INGAME_COMMAND_POSITION 0xD70 /* InGamePlayerSelection_ApplyPositionCommand */
#define INGAME_COMMAND_SELECT_ARMY 0xDA0 /* InGamePlayerSelection_SelectArmyRuntimeIndex */
#define INGAME_COMMAND_TARGET_POSITION 0xDE0 /* InGamePlayerSelection_ApplyTargetPositionCommand */
#define INGAME_COMMAND_PLACE_ARMY 0x13A0 /* InGameCommand_ExecuteLocalPlacementFromSelection */
#define INGAME_COMMAND_SELECT_MODEL_AND_ARMY 0x1620 /* FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel */
#define INGAME_COMMAND_SET_SESSION_FLAGS 0x340 /* FrontendPlayerRuntime_SetReadyFlagById */
#define INGAME_COMMAND_ASSIGN_ARMY_TOKEN 0x16B0 /* FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80 */
#define INGAME_COMMAND_APPLY_UI_FLAG_MASKS 0x310 /* UiCommandRuntimeFlags_ApplyClearSetToggleMasks */
#define INGAME_COMMAND_MARK_PLAYER_READY_101B 0x470 /* FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B */
/* Army lists and army placement (gameplay/faction/runtime) */
#define INGAME_COMMAND_CANCEL_QUEUED_ARMY 0x1030 /* GameFactionRuntime_CancelQueuedArmyAssetsAndRefund */
#define INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT 0x12D0 /* GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer */
#define INGAME_COMMAND_SELL_ARMY 0x1570 /* GameFactionRuntime_SellArmyAssetAndRefundSevenEighths */
#define INGAME_COMMAND_PLACEMENT_CREATE_ARMY 0x2E50 /* PlayerRuntime_ResolveAndStoreState8094 */
#define INGAME_COMMAND_PLACEMENT_SET_FACTION 0x2EC0 /* PlayerRuntime_SetState8090 */
#define INGAME_COMMAND_PLACEMENT_SET_ARMY 0x2EF0 /* PlayerRuntime_SetState8094 */
#define INGAME_COMMAND_PLACEMENT_CLEAR_ARMY 0x3190 /* PlayerRuntime_ClearState8094 */
/* Technology page, chat and army removal (ui/frontend/player) */
#define INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE 0x1700 /* FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology */
#define INGAME_COMMAND_CHAT_SET_RECIPIENTS 0x1770 /* FrontendPlayerTextCommand_SetPackedState */
#define INGAME_COMMAND_CHAT_APPEND 0x17A0 /* FrontendPlayerTextCommand_AppendTripleClamped */
#define INGAME_COMMAND_CHAT_PUBLISH 0x1810 /* FrontendPlayerTextCommand_PublishConditionalRichText */
#define INGAME_COMMAND_DESTROY_ARMIES 0x2DE0 /* FrontendPlayerSelection_ApplyEntryOrAll */
/* Frontend command codes work the same way, relative to FrontendCommandQueue_EnqueueLocalPlayerCommand
   (0x00543F50). */
#define FRONTEND_COMMAND_CODE_BASE 0x00543F50
#define FRONTEND_COMMAND_PLAYER_READY 0xD0 /* FrontendPlayerRuntime_RecordReadyAndUpdateWaitState */
#define FRONTEND_COMMAND_XOR_PLAYER_STATE 0x3B0 /* FrontendPlayerRuntime_XorStateMaskByPlayerId */
#define FRONTEND_COMMAND_STOP_ROM_TRANSITION 0x1340 /* ScenarioCatalog_RequestRomTransitionStopCallback */
#define FRONTEND_COMMAND_EXECUTE_ROM_ACTION 0x1350 /* FrontendRomActionTable_ExecuteRecord */
#define FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE 0x16F0 /* FrontendTransfer_MarkUnavailableIfModeBit0Callback */
/* Lobby handshake and chat (ui/frontend/player) */
#define FRONTEND_COMMAND_BRIEFING_READY 0x1E0 /* FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08 */
#define FRONTEND_COMMAND_SET_CONSENSUS_VALUE 0x820 /* FrontendPlayerRuntime_SetConsensusValueAndRefresh */
#define FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED 0xE00 /* FrontendPlayerRuntime_MarkFlag01AndStoreValuesById */
#define FRONTEND_COMMAND_CHAT_BEGIN 0x1540 /* FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById */
#define FRONTEND_COMMAND_CHAT_APPEND 0x15B0 /* FrontendPlayerMessageBuffer_AppendTripleById */
#define FRONTEND_COMMAND_CHAT_PUBLISH 0x1640 /* FrontendPlayerMessageBuffer_PublishTextById */
/* Scenario selection ("Choose game" page, assets/scenario/catalog) */
#define FRONTEND_COMMAND_LOAD_FIELD_GRID 0x460 /* FrontendScenarioSession_LoadOrRequestFieldGrid */
#define FRONTEND_COMMAND_LOAD_LEVEL 0x920 /* FrontendScenarioSession_LoadOrRequestLevelAsset */
#define FRONTEND_COMMAND_LOAD_CAMPAIGN 0xB70 /* FrontendScenarioSession_LoadOrRequestCampaignBundle */
#define FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE 0xDC0 /* FrontendSession_ReturnToMainPage */
#define FRONTEND_COMMAND_SHOW_SAVED_GAMES 0xE70 /* ScenarioCatalog_RebuildSaveRecordListPage */
#define FRONTEND_COMMAND_SHOW_SINGLE_GAMES 0xF50 /* ScenarioCatalog_RebuildLevelRecordListPage */
#define FRONTEND_COMMAND_SHOW_CAMPAIGNS 0x10D0 /* ScenarioCatalog_RebuildCampaignRecordListPage */
#define FRONTEND_COMMAND_SELECT_SAVED_GAME 0x11F0 /* ScenarioCatalog_RefreshSelectedRecordLocalizedText */
#define FRONTEND_COMMAND_SELECT_SINGLE_GAME 0x12A0 /* ScenarioCatalog_RefreshSelectedRecordField70DisplayId */
#define FRONTEND_COMMAND_SELECT_CAMPAIGN 0x12F0 /* ScenarioCatalog_RefreshSelectedRecordField50DisplayId */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00543F50 */
void __thandor_void_preserve_eax_ecx_edx
FrontendCommandQueue_EnqueueLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04);

/* 0x00543FB0 */
void __thandor_void_preserve_ecx_edx
FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

/* 0x0055F130 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandQueue_AppendLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04);

/* 0x0055F190 */
void __thandor_void_preserve_ecx_edx
InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

/* 0x0055F200 */
bool __thandor_cf_preserve_eax_ecx_edx
InGameCommandQueue_ContainsTripletValue
          (InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress);

#endif /* THANDOR_NETWORK_PROTOCOL_COMMANDS_H */
