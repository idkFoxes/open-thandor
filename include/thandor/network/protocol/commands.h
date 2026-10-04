/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_COMMANDS_H
#define THANDOR_NETWORK_PROTOCOL_COMMANDS_H

#include <thandor/core/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/commands. */

/* In-game command codes are handler addresses relative to InGameCommandQueue_AppendLocalPlayerCommand
   (whose original address is INGAME_COMMAND_CODE_BASE); a command is executed by calling
   INGAME_COMMAND_CODE_BASE + code. Single player calls the named handler directly instead of queueing the code. */
#define INGAME_COMMAND_CODE_BASE 0x0055F130
#define INGAME_COMMAND_SELECT_SINGLE_ARMY 0x9A0 /* FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection */
#define INGAME_COMMAND_REPLACE_SELECTION 0xA00 /* InGamePlayerSelection_ReplaceWithArmyRuntimeIndex */
#define INGAME_COMMAND_SELECTION_INSERT 0xA60 /* FrontendPlayerSelection_InsertThreeEntriesAndRefresh */
#define INGAME_COMMAND_SELECTION_REMOVE 0xB00 /* FrontendPlayerSelection_RemoveThreeEntriesAndRefresh */
#define INGAME_COMMAND_SELECTION_CLEAR 0xBA0 /* FrontendPlayerSelection_ClearAndRefreshLocalPanels */
#define INGAME_COMMAND_MOVE 0xD40 /* InGamePlayerSelection_ApplyMoveCommand */
#define INGAME_COMMAND_POSITION 0xD70 /* InGamePlayerSelection_ApplyPositionCommand */
#define INGAME_COMMAND_SELECT_ARMY 0xDA0 /* InGamePlayerSelection_SelectArmyRuntimeIndex */
#define INGAME_COMMAND_TARGET_POSITION 0xDE0 /* InGamePlayerSelection_ApplyTargetPositionCommand */
#define INGAME_COMMAND_PLACE_ARMY 0x13A0 /* InGameCommand_ExecuteLocalPlacementFromSelection */
#define INGAME_COMMAND_SELECT_MODEL_AND_ARMY 0x1620 /* FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel */
#define INGAME_COMMAND_SET_SLOW_RENDERING 0x340 /* FrontendPlayerRuntime_SetSlowRenderingFlagById */
#define INGAME_COMMAND_ASSIGN_ARMY_TOKEN 0x16B0 /* FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch */
#define INGAME_COMMAND_APPLY_UI_FLAG_MASKS 0x310 /* UiCommandRuntimeFlags_ApplyClearSetToggleMasks */
#define INGAME_COMMAND_RESULTS_READY 0x470 /* FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton */
/* Commands issued by the in-game command buttons (ui/ingame/commands) */
#define INGAME_COMMAND_PLAYER_DEPARTURE 0x150 /* InGameCommand_HandlePlayerDeparture */
#define INGAME_COMMAND_SELECTION_GROUP 0xBE0 /* FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh */
#define INGAME_COMMAND_QUEUE_ARMY 0xFE0 /* GameFactionRuntime_RegisterArmyAssetPointers */
#define INGAME_COMMAND_CONSUME_PENDING_ARMY 0x14F0 /* GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid */
#define INGAME_COMMAND_TERRAIN_RELAXATION 0x3200 /* TerrainGrid_RunDirectionalRelaxationPasses */
/* Army lists and army placement (gameplay/faction/runtime) */
#define INGAME_COMMAND_CANCEL_QUEUED_ARMY 0x1030 /* GameFactionRuntime_CancelQueuedArmyAssetsAndRefund */
#define INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT 0x12D0 /* GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer */
#define INGAME_COMMAND_SELL_ARMY 0x1570 /* GameFactionRuntime_SellArmyAssetAndRefundSevenEighths */
#define INGAME_COMMAND_PLACEMENT_CREATE_ARMY 0x2E50 /* PlayerRuntime_CreatePlacementArmy */
#define INGAME_COMMAND_PLACEMENT_SET_FACTION 0x2EC0 /* PlayerRuntime_SetPlacementFaction */
#define INGAME_COMMAND_PLACEMENT_SET_ARMY 0x2EF0 /* PlayerRuntime_SetPlacementArmy */
#define INGAME_COMMAND_PLACEMENT_CLEAR_ARMY 0x3190 /* PlayerRuntime_ClearPlacementArmy */
/* Technology page, chat and army removal (ui/frontend/player) */
#define INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE 0x1700 /* FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology */
#define INGAME_COMMAND_CHAT_SET_RECIPIENTS 0x1770 /* FrontendPlayerTextCommand_SetPackedState */
#define INGAME_COMMAND_CHAT_APPEND 0x17A0 /* FrontendPlayerTextCommand_AppendTripleClamped */
#define INGAME_COMMAND_CHAT_PUBLISH 0x1810 /* FrontendPlayerTextCommand_PublishConditionalRichText */
#define INGAME_COMMAND_DESTROY_ARMIES 0x2DE0 /* FrontendPlayerSelection_ApplyEntryOrAll */
/* Relations, map editor and editor hotkeys (ui/ingame/runtime) */
#define INGAME_COMMAND_ADVANCE_RELATION 0x660 /* GameFactionRuntime_AdvancePairwiseRelationState */
#define INGAME_COMMAND_RESET_RELATION 0x7E0 /* GameFactionRuntime_ResetPairwiseRelationState */
#define INGAME_COMMAND_EDITOR_ACTIVE_STATE 0x18C0 /* InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState */
#define INGAME_COMMAND_EDITOR_SELECT_RANGE 0x1D00 /* PlayerPairList_InsertRange */
#define INGAME_COMMAND_EDITOR_DESELECT_RANGE 0x1D40 /* PlayerPairList_RemoveRange */
#define INGAME_COMMAND_EDITOR_CLEAR_SELECTION 0x1ED0 /* SelectionPlayerRuntime_ClearTerrainEditSelectionState */
#define INGAME_COMMAND_EDITOR_CLEAR_SCRATCH 0x1F20 /* FieldGrid_ClearPlayerScratchPlane */
#define INGAME_COMMAND_EDITOR_RAISE_HEIGHTS 0x1F70 /* FieldGrid_ApplyPositiveCellDeltas */
#define INGAME_COMMAND_EDITOR_LOWER_HEIGHTS 0x2290 /* FieldGrid_ApplyNegativeCellDeltas */
#define INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS 0x25A0 /* TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting */
#define INGAME_COMMAND_EDITOR_COPY_MATERIALS 0x2700 /* TerrainEditBuffer_CopyCellMaterialBytes */
#define INGAME_COMMAND_EDITOR_PAINT_MATERIAL 0x2770 /* FieldGrid_ApplyLocalCellUpdate */
#define INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS 0x2800 /* TerrainEditBuffer_SubtractCurrentCellMaterialBytes */
#define INGAME_COMMAND_EDITOR_COMMIT_MATERIALS 0x2870 /* TerrainEditBuffer_CommitFlagsAndMaterialDeltas */
#define INGAME_COMMAND_EDITOR_REPLACE_MATCHING 0x28E0 /* TerrainMaterialEdit_SeedMatchingRegionReplacement */
#define INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET 0x29B0 /* TerrainMaterialEdit_SeedNonTargetRegionReplacement */
#define INGAME_COMMAND_EDITOR_RESET_INFLUENCE 0x2A80 /* FieldGrid_ResetLocalInfluenceState */
#define INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE 0x2AE0 /* FieldGrid_RebuildLocalInfluenceState */
#define INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS 0x2C90 /* TerrainEditBuffer_ConvertHeightsToDeltas */
#define INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES 0x2D00 /* WorldRuntime_TurnAuxiliaryAnglesClamped */
#define INGAME_COMMAND_EDITOR_TURN_LIGHT 0x2D70 /* TerrainLighting_AdjustDirectionAndRecomputeField */
#define INGAME_COMMAND_PLACEMENT_MOVE 0x2F20 /* SelectionPlayerRuntime_MovePrimarySelectionBy */
#define INGAME_COMMAND_PLACEMENT_ROTATE 0x30F0 /* SelectionPlayerRuntime_RotatePrimarySelectionBy */
#define INGAME_COMMAND_EDITOR_SAVE_MAP 0x31C0 /* InGameUiCommand_SaveFieldAndLevelAssetImages */
#define INGAME_COMMAND_EDITOR_SMOOTH 0x3260 /* FieldGrid_ApplyEncodedCellUpdate */
#define INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED 0x32A0 /* FieldGrid_SetCellFluidReceiverExcluded */
#define INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED 0x32E0 /* FieldGrid_SetCellFluidSourceExcluded */
#define INGAME_COMMAND_EDITOR_APPLY_REGION_MASK 0x3320 /* FieldGrid_SetCellResourceSupportFlag */
/* Frontend command codes work the same way, relative to FrontendCommandQueue_EnqueueLocalPlayerCommand
   (whose original address is FRONTEND_COMMAND_CODE_BASE). */
#define FRONTEND_COMMAND_CODE_BASE 0x00543F50
#define FRONTEND_COMMAND_PLAYER_READY 0xD0 /* FrontendPlayerRuntime_RecordReadyAndUpdateWaitState */
#define FRONTEND_COMMAND_XOR_PLAYER_STATE 0x3B0 /* FrontendPlayerRuntime_XorStateMaskByPlayerId */
#define FRONTEND_COMMAND_STOP_ROM_TRANSITION 0x1340 /* ScenarioCatalog_RequestRomTransitionStopCallback */
#define FRONTEND_COMMAND_EXECUTE_ROM_ACTION 0x1350 /* FrontendRomActionTable_ExecuteRecord */
#define FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE 0x16F0 /* FrontendTransfer_MarkUnavailableIfModeBit0Callback */
#define FRONTEND_COMMAND_SNAPSHOTS_RECEIVED 0x1710 /* FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady */
/* Lobby handshake and chat (ui/frontend/player) */
#define FRONTEND_COMMAND_BRIEFING_READY 0x1E0 /* FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton */
#define FRONTEND_COMMAND_SET_CONSENSUS_VALUE 0x820 /* FrontendPlayerRuntime_SetConsensusValueAndRefresh */
#define FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED 0xE00 /* FrontendPlayerRuntime_MarkScenarioCatalogReceivedById */
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
#define FRONTEND_COMMAND_SELECT_SAVED_GAME 0x11F0 /* ScenarioCatalog_SelectSavedGameAndShowDescription */
#define FRONTEND_COMMAND_SELECT_SINGLE_GAME 0x12A0 /* ScenarioCatalog_SelectLevelAndShowDescription */
#define FRONTEND_COMMAND_SELECT_CAMPAIGN 0x12F0 /* ScenarioCatalog_SelectCampaignAndShowDescription */
/* Scenario transfer acknowledgements of a network client (FrontendScenarioTransfer_ProcessReceivedAsset) */
#define FRONTEND_COMMAND_LEVEL_RECEIVED 0x8D0 /* FrontendPlayerRuntime_MarkTaskAssignmentReadyById */
#define FRONTEND_COMMAND_FIELD_GRID_RECEIVED 0x360 /* FrontendPlayerRuntime_MarkLevelReceivedById */
#define FRONTEND_COMMAND_BUNDLE_RECEIVED 0x410 /* FrontendPlayerRuntime_MarkLevelLoadedById (campaign or level bundle) */
/* Faction setup page, mission briefing and leaving pages (ui/frontend/runtime, ui/frontend/session) */
#define FRONTEND_COMMAND_CYCLE_FACTION_COLOUR 0x650 /* FrontendFactionSetup_CycleFactionColour */
#define FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE 0x6F0 /* FrontendFactionSetup_ToggleFactionActive */
#define FRONTEND_COMMAND_CHOOSE_FACTION 0x750 /* FrontendFactionSetup_ChooseFaction */
#define FRONTEND_COMMAND_APPLY_GAME_SPEED 0x2C0 /* FrontendSession_ApplyGameSpeedAndReturnToMainPage */
#define FRONTEND_COMMAND_SET_GAME_SPEED 0x300 /* FrontendSession_SetGameSpeedPercent */
#define FRONTEND_COMMAND_RELEASE_CAMPAIGN 0x320 /* FrontendSession_ReleaseSelectedResourceAndReturnToMainPage */
/* Records of the frontend and in-game command queues (g_*CommandQueueRecords[16]); more commands are dropped. */
#define COMMAND_QUEUE_CAPACITY 16
/* Received commands are executed only when codeBase + code lies below these original addresses. */
#define FRONTEND_COMMAND_HANDLER_REGION_END 0x005456F0 /* g_FrontendRootNode in the original image */
#define INGAME_COMMAND_HANDLER_REGION_END 0x00562499 /* InGameCommandHandlerCodeRegionEnd: the 0x90 filler bytes before g_InGameUiActionHandlersPage10 in the original image */
/* Functions are grouped by semantic ownership. */

void FrontendCommandQueue_EnqueueLocalPlayerCommand(UiActionId commandCode,CommandPayload payload1,
          CommandPayload payload2,CommandPayload payload3);

void FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

void InGameCommandQueue_AppendLocalPlayerCommand(UiActionId commandCode,CommandPayload payload1,
          CommandPayload payload2,CommandPayload payload3);

void InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

Bool8 InGameCommandQueue_ContainsTripletValue(InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress);

/* Rebuild helper: the handler for a received command code from the explicit command table of codeBase, or
   NULL when the original skips it (at or past originalRegionEnd) or the code is no handler. */
CommandQueueHandlerProc *
CommandDispatch_ResolveHandler(uint32_t codeBase,uint32_t originalRegionEnd,uint32_t code);

/* Rebuild helper: executes one received command record of codeBase (FRONTEND_COMMAND_CODE_BASE or
   INGAME_COMMAND_CODE_BASE): an empty code (0) and a code CommandDispatch_ResolveHandler skips do nothing, a
   record with an unknown player (in-game) or a payload out of range for its handler is dropped (logged once). */
void CommandDispatch_ExecuteRecord(uint32_t codeBase,uint32_t originalRegionEnd,const UiCommandQueueRecord *record);

/* Rebuild helper: the command code of handler in the table of codeBase, or 0xFFFFFFFF. */
uint32_t CommandDispatch_CodeOfHandler(uint32_t codeBase,const void *handler);

#endif /* THANDOR_NETWORK_PROTOCOL_COMMANDS_H */
