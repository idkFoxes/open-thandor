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
#include <thandor/gameplay/session/runtime.h>
#include <thandor/ui/ingame/editor_tools.h>
#include <thandor/platform/bootstrap/image.h>

/* In-game command codes are handler addresses relative to InGameCommandQueue_AppendLocalPlayerCommand
   (whose original address is INGAME_COMMAND_CODE_BASE); a command is executed by calling
   INGAME_COMMAND_CODE_BASE + code. Single player calls the named handler directly instead of queueing the code. */
inline constexpr auto INGAME_COMMAND_CODE_BASE = 0x0055F130;
inline constexpr auto INGAME_COMMAND_SELECT_SINGLE_ARMY = 0x9A0; /* FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection */
inline constexpr auto INGAME_COMMAND_REPLACE_SELECTION = 0xA00; /* InGamePlayerSelection_ReplaceWithArmyRuntimeIndex */
inline constexpr auto INGAME_COMMAND_SELECTION_INSERT = 0xA60; /* FrontendPlayerSelection_InsertThreeEntriesAndRefresh */
inline constexpr auto INGAME_COMMAND_SELECTION_REMOVE = 0xB00; /* FrontendPlayerSelection_RemoveThreeEntriesAndRefresh */
inline constexpr auto INGAME_COMMAND_SELECTION_CLEAR = 0xBA0; /* FrontendPlayerSelection_ClearAndRefreshLocalPanels */
inline constexpr auto INGAME_COMMAND_MOVE = 0xD40; /* InGamePlayerSelection_ApplyMoveCommand */
inline constexpr auto INGAME_COMMAND_POSITION = 0xD70; /* InGamePlayerSelection_ApplyPositionCommand */
inline constexpr auto INGAME_COMMAND_SELECT_ARMY = 0xDA0; /* InGamePlayerSelection_SelectArmyRuntimeIndex */
inline constexpr auto INGAME_COMMAND_TARGET_POSITION = 0xDE0; /* InGamePlayerSelection_ApplyTargetPositionCommand */
inline constexpr auto INGAME_COMMAND_PLACE_ARMY = 0x13A0; /* InGameCommand_ExecuteLocalPlacementFromSelection */
inline constexpr auto INGAME_COMMAND_SELECT_MODEL_AND_ARMY = 0x1620; /* FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel */
inline constexpr auto INGAME_COMMAND_SET_SLOW_RENDERING = 0x340; /* FrontendPlayerRuntime_SetSlowRenderingFlagById */
inline constexpr auto INGAME_COMMAND_ASSIGN_ARMY_TOKEN = 0x16B0; /* FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch */
inline constexpr auto INGAME_COMMAND_APPLY_UI_FLAG_MASKS = 0x310; /* UiCommandRuntimeFlags_ApplyClearSetToggleMasks */
inline constexpr auto INGAME_COMMAND_RESULTS_READY = 0x470; /* FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton */
/* Commands issued by the in-game command buttons (ui/ingame/commands) */
inline constexpr auto INGAME_COMMAND_PLAYER_DEPARTURE = 0x150; /* InGameCommand_HandlePlayerDeparture */
inline constexpr auto INGAME_COMMAND_SELECTION_GROUP = 0xBE0; /* FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh */
inline constexpr auto INGAME_COMMAND_QUEUE_ARMY = 0xFE0; /* GameFactionRuntime_RegisterArmyAssetPointers */
inline constexpr auto INGAME_COMMAND_CONSUME_PENDING_ARMY = 0x14F0; /* GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid */
inline constexpr auto INGAME_COMMAND_TERRAIN_RELAXATION = 0x3200; /* TerrainGrid_RunDirectionalRelaxationPasses */
/* Army lists and army placement (gameplay/faction/runtime) */
inline constexpr auto INGAME_COMMAND_CANCEL_QUEUED_ARMY = 0x1030; /* GameFactionRuntime_CancelQueuedArmyAssetsAndRefund */
inline constexpr auto INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT = 0x12D0; /* GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer */
inline constexpr auto INGAME_COMMAND_SELL_ARMY = 0x1570; /* GameFactionRuntime_SellArmyAssetAndRefundSevenEighths */
inline constexpr auto INGAME_COMMAND_PLACEMENT_CREATE_ARMY = 0x2E50; /* PlayerRuntime_CreatePlacementArmy */
inline constexpr auto INGAME_COMMAND_PLACEMENT_SET_FACTION = 0x2EC0; /* PlayerRuntime_SetPlacementFaction */
inline constexpr auto INGAME_COMMAND_PLACEMENT_SET_ARMY = 0x2EF0; /* PlayerRuntime_SetPlacementArmy */
inline constexpr auto INGAME_COMMAND_PLACEMENT_CLEAR_ARMY = 0x3190; /* PlayerRuntime_ClearPlacementArmy */
/* Technology page, chat and army removal (ui/frontend/player) */
inline constexpr auto INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE = 0x1700; /* FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology */
inline constexpr auto INGAME_COMMAND_CHAT_SET_RECIPIENTS = 0x1770; /* FrontendPlayerTextCommand_SetPackedState */
inline constexpr auto INGAME_COMMAND_CHAT_APPEND = 0x17A0; /* FrontendPlayerTextCommand_AppendTripleClamped */
inline constexpr auto INGAME_COMMAND_CHAT_PUBLISH = 0x1810; /* FrontendPlayerTextCommand_PublishConditionalRichText */
inline constexpr auto INGAME_COMMAND_DESTROY_ARMIES = 0x2DE0; /* FrontendPlayerSelection_ApplyEntryOrAll */
/* Relations, map editor and editor hotkeys (ui/ingame/runtime) */
inline constexpr auto INGAME_COMMAND_ADVANCE_RELATION = 0x660; /* GameFactionRuntime_AdvancePairwiseRelationState */
inline constexpr auto INGAME_COMMAND_RESET_RELATION = 0x7E0; /* GameFactionRuntime_ResetPairwiseRelationState */
inline constexpr auto INGAME_COMMAND_EDITOR_ACTIVE_STATE = 0x18C0; /* InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState */
inline constexpr auto INGAME_COMMAND_EDITOR_SELECT_RANGE = 0x1D00; /* PlayerPairList_InsertRange */
inline constexpr auto INGAME_COMMAND_EDITOR_DESELECT_RANGE = 0x1D40; /* PlayerPairList_RemoveRange */
inline constexpr auto INGAME_COMMAND_EDITOR_CLEAR_SELECTION = 0x1ED0; /* SelectionPlayerRuntime_ClearTerrainEditSelectionState */
inline constexpr auto INGAME_COMMAND_EDITOR_CLEAR_SCRATCH = 0x1F20; /* FieldGrid_ClearPlayerScratchPlane */
inline constexpr auto INGAME_COMMAND_EDITOR_RAISE_HEIGHTS = 0x1F70; /* FieldGrid_ApplyPositiveCellDeltas */
inline constexpr auto INGAME_COMMAND_EDITOR_LOWER_HEIGHTS = 0x2290; /* FieldGrid_ApplyNegativeCellDeltas */
inline constexpr auto INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS = 0x25A0; /* TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting */
inline constexpr auto INGAME_COMMAND_EDITOR_COPY_MATERIALS = 0x2700; /* TerrainEditBuffer_CopyCellMaterialBytes */
inline constexpr auto INGAME_COMMAND_EDITOR_PAINT_MATERIAL = 0x2770; /* FieldGrid_ApplyLocalCellUpdate */
inline constexpr auto INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS = 0x2800; /* TerrainEditBuffer_SubtractCurrentCellMaterialBytes */
inline constexpr auto INGAME_COMMAND_EDITOR_COMMIT_MATERIALS = 0x2870; /* TerrainEditBuffer_CommitFlagsAndMaterialDeltas */
inline constexpr auto INGAME_COMMAND_EDITOR_REPLACE_MATCHING = 0x28E0; /* TerrainMaterialEdit_SeedMatchingRegionReplacement */
inline constexpr auto INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET = 0x29B0; /* TerrainMaterialEdit_SeedNonTargetRegionReplacement */
inline constexpr auto INGAME_COMMAND_EDITOR_RESET_INFLUENCE = 0x2A80; /* FieldGrid_ResetLocalInfluenceState */
inline constexpr auto INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE = 0x2AE0; /* FieldGrid_RebuildLocalInfluenceState */
inline constexpr auto INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS = 0x2C90; /* TerrainEditBuffer_ConvertHeightsToDeltas */
inline constexpr auto INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES = 0x2D00; /* WorldRuntime_TurnAuxiliaryAnglesClamped */
inline constexpr auto INGAME_COMMAND_EDITOR_TURN_LIGHT = 0x2D70; /* TerrainLighting_AdjustDirectionAndRecomputeField */
inline constexpr auto INGAME_COMMAND_PLACEMENT_MOVE = 0x2F20; /* SelectionPlayerRuntime_MovePrimarySelectionBy */
inline constexpr auto INGAME_COMMAND_PLACEMENT_ROTATE = 0x30F0; /* SelectionPlayerRuntime_RotatePrimarySelectionBy */
inline constexpr auto INGAME_COMMAND_EDITOR_SAVE_MAP = 0x31C0; /* InGameUiCommand_SaveFieldAndLevelAssetImages */
inline constexpr auto INGAME_COMMAND_EDITOR_SMOOTH = 0x3260; /* FieldGrid_ApplyEncodedCellUpdate */
inline constexpr auto INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED = 0x32A0; /* FieldGrid_SetCellFluidReceiverExcluded */
inline constexpr auto INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED = 0x32E0; /* FieldGrid_SetCellFluidSourceExcluded */
inline constexpr auto INGAME_COMMAND_EDITOR_APPLY_REGION_MASK = 0x3320; /* FieldGrid_SetCellResourceSupportFlag */
/* Frontend command codes work the same way, relative to FrontendCommandQueue_EnqueueLocalPlayerCommand
   (whose original address is FRONTEND_COMMAND_CODE_BASE). */
inline constexpr auto FRONTEND_COMMAND_CODE_BASE = 0x00543F50;
inline constexpr auto FRONTEND_COMMAND_PLAYER_READY = 0xD0; /* FrontendPlayerRuntime_RecordReadyAndUpdateWaitState */
inline constexpr auto FRONTEND_COMMAND_XOR_PLAYER_STATE = 0x3B0; /* FrontendPlayerRuntime_XorStateMaskByPlayerId */
inline constexpr auto FRONTEND_COMMAND_STOP_ROM_TRANSITION = 0x1340; /* ScenarioCatalog_RequestRomTransitionStopCallback */
inline constexpr auto FRONTEND_COMMAND_EXECUTE_ROM_ACTION = 0x1350; /* FrontendRomActionTable_ExecuteRecord */
inline constexpr auto FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE = 0x16F0; /* FrontendTransfer_MarkUnavailableIfModeBit0Callback */
inline constexpr auto FRONTEND_COMMAND_SNAPSHOTS_RECEIVED = 0x1710; /* FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady */
/* Lobby handshake and chat (ui/frontend/player) */
inline constexpr auto FRONTEND_COMMAND_BRIEFING_READY = 0x1E0; /* FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton */
inline constexpr auto FRONTEND_COMMAND_SET_CONSENSUS_VALUE = 0x820; /* FrontendPlayerRuntime_SetConsensusValueAndRefresh */
inline constexpr auto FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED = 0xE00; /* FrontendPlayerRuntime_MarkScenarioCatalogReceivedById */
inline constexpr auto FRONTEND_COMMAND_CHAT_BEGIN = 0x1540; /* FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById */
inline constexpr auto FRONTEND_COMMAND_CHAT_APPEND = 0x15B0; /* FrontendPlayerMessageBuffer_AppendTripleById */
inline constexpr auto FRONTEND_COMMAND_CHAT_PUBLISH = 0x1640; /* FrontendPlayerMessageBuffer_PublishTextById */
/* Scenario selection ("Choose game" page, assets/scenario/catalog) */
inline constexpr auto FRONTEND_COMMAND_LOAD_FIELD_GRID = 0x460; /* FrontendScenarioSession_LoadOrRequestFieldGrid */
inline constexpr auto FRONTEND_COMMAND_LOAD_LEVEL = 0x920; /* FrontendScenarioSession_LoadOrRequestLevelAsset */
inline constexpr auto FRONTEND_COMMAND_LOAD_CAMPAIGN = 0xB70; /* FrontendScenarioSession_LoadOrRequestCampaignBundle */
inline constexpr auto FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE = 0xDC0; /* FrontendSession_ReturnToMainPage */
inline constexpr auto FRONTEND_COMMAND_SHOW_SAVED_GAMES = 0xE70; /* ScenarioCatalog_RebuildSaveRecordListPage */
inline constexpr auto FRONTEND_COMMAND_SHOW_SINGLE_GAMES = 0xF50; /* ScenarioCatalog_RebuildLevelRecordListPage */
inline constexpr auto FRONTEND_COMMAND_SHOW_CAMPAIGNS = 0x10D0; /* ScenarioCatalog_RebuildCampaignRecordListPage */
inline constexpr auto FRONTEND_COMMAND_SELECT_SAVED_GAME = 0x11F0; /* ScenarioCatalog_SelectSavedGameAndShowDescription */
inline constexpr auto FRONTEND_COMMAND_SELECT_SINGLE_GAME = 0x12A0; /* ScenarioCatalog_SelectLevelAndShowDescription */
inline constexpr auto FRONTEND_COMMAND_SELECT_CAMPAIGN = 0x12F0; /* ScenarioCatalog_SelectCampaignAndShowDescription */
/* Scenario transfer acknowledgements of a network client (FrontendScenarioTransfer_ProcessReceivedAsset) */
inline constexpr auto FRONTEND_COMMAND_LEVEL_RECEIVED = 0x8D0; /* FrontendPlayerRuntime_MarkTaskAssignmentReadyById */
inline constexpr auto FRONTEND_COMMAND_FIELD_GRID_RECEIVED = 0x360; /* FrontendPlayerRuntime_MarkLevelReceivedById */
inline constexpr auto FRONTEND_COMMAND_BUNDLE_RECEIVED = 0x410; /* FrontendPlayerRuntime_MarkLevelLoadedById (campaign or level bundle) */
/* Faction setup page, mission briefing and leaving pages (ui/frontend/runtime, ui/frontend/session) */
inline constexpr auto FRONTEND_COMMAND_CYCLE_FACTION_COLOUR = 0x650; /* FrontendFactionSetup_CycleFactionColour */
inline constexpr auto FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE = 0x6F0; /* FrontendFactionSetup_ToggleFactionActive */
inline constexpr auto FRONTEND_COMMAND_CHOOSE_FACTION = 0x750; /* FrontendFactionSetup_ChooseFaction */
inline constexpr auto FRONTEND_COMMAND_APPLY_GAME_SPEED = 0x2C0; /* FrontendSession_ApplyGameSpeedAndReturnToMainPage */
inline constexpr auto FRONTEND_COMMAND_SET_GAME_SPEED = 0x300; /* FrontendSession_SetGameSpeedPercent */
inline constexpr auto FRONTEND_COMMAND_RELEASE_CAMPAIGN = 0x320; /* FrontendSession_ReleaseSelectedResourceAndReturnToMainPage */
/* Records of the frontend and in-game command queues (g_*CommandQueueRecords[16]); more commands are dropped. */
inline constexpr auto COMMAND_QUEUE_CAPACITY = 16;
/* Received commands are executed only when codeBase + code lies below these original addresses. */
inline constexpr auto FRONTEND_COMMAND_HANDLER_REGION_END = 0x005456F0; /* g_FrontendRootNode in the original image */
inline constexpr auto INGAME_COMMAND_HANDLER_REGION_END = 0x00562499; /* InGameCommandHandlerCodeRegionEnd: the 0x90 filler bytes before g_InGameUiActionHandlersPage10 in the original image */

void FrontendCommandQueue_EnqueueLocalPlayerCommand(UiActionId commandCode,CommandPayload payload1,
          CommandPayload payload2,CommandPayload payload3);

void FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

void InGameCommandQueue_AppendLocalPlayerCommand(UiActionId commandCode,CommandPayload payload1,
          CommandPayload payload2,CommandPayload payload3);

void InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

bool InGameCommandQueue_ContainsTripletValue(InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress);

/* Rebuild helper: the handler for a received command code from the explicit command table of codeBase, or
   NULL when the original skips it (at or past originalRegionEnd) or the code is no handler. */
CommandQueueHandlerProc *
CommandDispatch_ResolveHandler(uint32_t codeBase,uint32_t originalRegionEnd,uint32_t code);

/* Rebuild helper: executes one received command record of codeBase (FRONTEND_COMMAND_CODE_BASE or
   INGAME_COMMAND_CODE_BASE): an empty code (0) and a code CommandDispatch_ResolveHandler skips do nothing, a
   record with an unknown player (in-game) or a payload out of range for its handler is dropped (logged once). */
void CommandDispatch_ExecuteRecord(uint32_t codeBase,uint32_t originalRegionEnd,const UiCommandQueueRecord *record);

/* A command handler's address as the const void * key of the command tables (a function pointer to an object
   pointer, hence reinterpret_cast). */
template <class Proc> static inline const void *CommandDispatch_HandlerKey(Proc *handler)
{
  return reinterpret_cast<const void *>(handler);
}

/* Rebuild helper: the command code of handler in the table of codeBase, or 0xFFFFFFFF. */
uint32_t CommandDispatch_CodeOfHandler(uint32_t codeBase,const void *handler);

/* Rebuild helper: the command code of Handler in the table of CodeBase, looked up once and cached (one cache
   per handler). A handler missing from the table yields 0xFFFFFFFF and is logged once. */
template<uint32_t CodeBase,auto Handler>
inline uint32_t CommandDispatch_CachedCodeOf()

{
  static const uint32_t s_code = CommandDispatch_CodeOfHandler(CodeBase,CommandDispatch_HandlerKey(Handler));
  static bool s_loggedMissing;

  if (s_code == 0xFFFFFFFFu && s_loggedMissing == 0) {
    s_loggedMissing = 1;
    Thandor_Log("network: handler %p is not in the command table of base 0x%08X, command dropped",
                CommandDispatch_HandlerKey(Handler),CodeBase);
  }
  return s_code;
}

/* Rebuild helper ("call locally or queue", the pattern of every command site in the original): a local
   session calls Handler(g_LocalPlayerRuntimeId,payload1,payload2,payload3) directly; a networked session
   queues Handler's in-game command code (its entry in the in-game command table) with the three payloads as
   CommandPayload dwords for the next command batch. A handler without a table entry is not queued (logged). */
template<auto Handler,typename P1,typename P2,typename P3>
inline void InGameCommand_Issue(P1 payload1,P2 payload2,P3 payload3)

{
  uint32_t code;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    Handler(g_LocalPlayerRuntimeId,payload1,payload2,payload3);
  }
  else {
    code = CommandDispatch_CachedCodeOf<INGAME_COMMAND_CODE_BASE,Handler>();
    if (code != 0xFFFFFFFFu) {
      InGameCommandQueue_AppendLocalPlayerCommand
                ((UiActionId)code,(CommandPayload)payload1,(CommandPayload)payload2,(CommandPayload)payload3);
    }
  }
}

/* The same for a lobby (frontend) command: Handler's code in the frontend command table, queued with
   FrontendCommandQueue_EnqueueLocalPlayerCommand. */
template<auto Handler,typename P1,typename P2,typename P3>
inline void FrontendCommand_Issue(P1 payload1,P2 payload2,P3 payload3)

{
  uint32_t code;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    Handler(g_LocalPlayerRuntimeId,payload1,payload2,payload3);
  }
  else {
    code = CommandDispatch_CachedCodeOf<FRONTEND_COMMAND_CODE_BASE,Handler>();
    if (code != 0xFFFFFFFFu) {
      FrontendCommandQueue_EnqueueLocalPlayerCommand
                ((UiActionId)code,(CommandPayload)payload1,(CommandPayload)payload2,(CommandPayload)payload3);
    }
  }
}

/* InGameCommand_Issue for a handler chosen at run time (the pointer-mode handlers of the world input): the
   code is looked up in the in-game command table on every networked call (not cached); a handler without a
   table entry is not queued (logged once). */
void InGameCommand_IssueHandler(CommandQueueHandlerProc *handler,CommandPayload payload1,CommandPayload payload2,
          CommandPayload payload3);

#endif /* THANDOR_NETWORK_PROTOCOL_COMMANDS_H */
