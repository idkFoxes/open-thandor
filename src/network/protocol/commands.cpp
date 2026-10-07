/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/network/protocol/packet_bytes.h>

/* Module data. */

static UiCommandQueueRecord g_FrontendCommandQueueRecords[16] = {};

/* followed by 12 bytes 0x90 fill (dropped) */
static UiCommandQueueRecord *g_FrontendCommandQueueEnd = g_FrontendCommandQueueRecords;

static UiCommandQueueRecord g_InGameCommandQueueRecords[16] = {};

static UiCommandQueueRecord *g_InGameCommandQueueEnd = THANDOR_PTR(&g_InGameCommandQueueRecords);

/* Queues a lobby (frontend) command of the local player for the next network command batch: one 16-byte
   record of (commandCode << 8 | local player id) and three payload dwords. The queue holds 16 records;
   further commands are dropped. The lobby chat command (0x1540) is sent this way.
*/
void FrontendCommandQueue_EnqueueLocalPlayerCommand(UiActionId commandCode,CommandPayload payload1,
          CommandPayload payload2,CommandPayload payload3)

{
  uint32_t packedCommandAndPlayerId;
  UiCommandQueueRecord *writeRecord;
  
  writeRecord = g_FrontendCommandQueueEnd;
  packedCommandAndPlayerId = commandCode << 8 | g_LocalPlayerRuntimeId;
  if (g_FrontendCommandQueueEnd < g_FrontendCommandQueueRecords + COMMAND_QUEUE_CAPACITY) { /* the end of the record buffer */
    g_FrontendCommandQueueEnd->payload3 = payload3;
    writeRecord->payload2 = payload2;
    writeRecord->payload1 = payload1;
    writeRecord->packedCommandAndPlayerId = packedCommandAndPlayerId;
    g_FrontendCommandQueueEnd++;
  }
}


/* Takes the oldest queued lobby command for the outgoing network batch: copies the first 16-byte queue
   record into outputRecord->command and shifts the remaining records down by one. An empty queue only
   writes a zero packed command dword.
*/
void FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord)

{
  int firstRecordDwordsRemaining;
  uint32_t trailingDwordCount;
  uint32_t *copySourceCursor;
  uint32_t *outputRecordWriteCursor;
  uint32_t *copyDestinationCursor;
  UiCommandQueueRecord *queueEndSnapshot;
  
  queueEndSnapshot = g_FrontendCommandQueueEnd;
  if (g_FrontendCommandQueueEnd == g_FrontendCommandQueueRecords) {
    outputRecord->command.packedCommandAndPlayerId = 0;
    return;
  }
  copySourceCursor = Packet_Dwords(g_FrontendCommandQueueRecords);
  outputRecordWriteCursor = Packet_Dwords(&outputRecord->command);
  /* dword-wise copies: the first record, then the rest of the queue onto the start */
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0; firstRecordDwordsRemaining--) {
    *outputRecordWriteCursor = *copySourceCursor;
    copySourceCursor++;
    outputRecordWriteCursor++;
  }
  copyDestinationCursor = Packet_Dwords(g_FrontendCommandQueueRecords);
  trailingDwordCount = (uint32_t)Packet_ByteDistance(queueEndSnapshot,&g_FrontendCommandQueueRecords[1]) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount--) {
      *copyDestinationCursor = *copySourceCursor;
      copySourceCursor++;
      copyDestinationCursor++;
    }
  }
  g_FrontendCommandQueueEnd--;
}


/* Queues an in-game command of the local player for the next lockstep command batch: one 16-byte record
   of (commandCode << 8 | local player id) and three payload dwords. commandCode is one of the
   INGAME_COMMAND_* handler offsets (relative to this function's address). The queue holds 16 records;
   further commands are dropped.
*/
void InGameCommandQueue_AppendLocalPlayerCommand(UiActionId commandCode,CommandPayload payload1,
          CommandPayload payload2,CommandPayload payload3)

{
  uint32_t packedCommandAndPlayerId;
  UiCommandQueueRecord *writeRecord;
  
  writeRecord = g_InGameCommandQueueEnd;
  packedCommandAndPlayerId = commandCode << 8 | g_LocalPlayerRuntimeId;
  if (g_InGameCommandQueueEnd < g_InGameCommandQueueRecords + COMMAND_QUEUE_CAPACITY) { /* the end of the record buffer */
    g_InGameCommandQueueEnd->payload3 = payload3;
    writeRecord->payload2 = payload2;
    writeRecord->payload1 = payload1;
    writeRecord->packedCommandAndPlayerId = packedCommandAndPlayerId;
    g_InGameCommandQueueEnd++;
  }
}


/* Takes the oldest queued in-game command into outputRecord->command (offset 0x10 of the packet record)
   and moves the remaining records one slot down. With an empty queue only the packed command dword is
   cleared, which marks "no command" in the batch.
*/
void InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord)

{
  int firstRecordDwordsRemaining;
  uint32_t trailingDwordCount;
  uint32_t *copySourceCursor;
  uint32_t *outputRecordWriteCursor;
  uint32_t *copyDestinationCursor;
  UiCommandQueueRecord *queueEndSnapshot;
  
  queueEndSnapshot = g_InGameCommandQueueEnd;
  if (g_InGameCommandQueueEnd == g_InGameCommandQueueRecords) {
    outputRecord->command.packedCommandAndPlayerId = 0;
    return;
  }
  copySourceCursor = Packet_Dwords(g_InGameCommandQueueRecords);
  outputRecordWriteCursor = Packet_Dwords(&outputRecord->command);
  /* dword-wise copies: the first record (4 dwords), then the rest of the queue onto the start */
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0; firstRecordDwordsRemaining--) {
    *outputRecordWriteCursor = *copySourceCursor;
    copySourceCursor++;
    outputRecordWriteCursor++;
  }
  copyDestinationCursor = Packet_Dwords(g_InGameCommandQueueRecords);
  trailingDwordCount = (uint32_t)Packet_ByteDistance(queueEndSnapshot,&g_InGameCommandQueueRecords[1]) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount--) {
      *copyDestinationCursor = *copySourceCursor;
      copySourceCursor++;
      copyDestinationCursor++;
    }
  }
  g_InGameCommandQueueEnd--;
}


/* Tells whether the local player already queued the command whose handler lives at commandHandlerAddress
   with payloadValue in any of its three payload dwords, so input handlers do not queue a selection change
   twice. Single player has no queue and always answers no.
*/
bool InGameCommandQueue_ContainsTripletValue(InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress)

{
  UiCommandQueueRecord *record;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    for (record = g_InGameCommandQueueRecords; record < g_InGameCommandQueueEnd; record++) {
      if (((commandHandlerAddress - INGAME_COMMAND_CODE_BASE) * 256 | g_LocalPlayerRuntimeId) ==
            record->packedCommandAndPlayerId &&
          (payloadValue == record->payload3 || payloadValue == record->payload2 ||
           payloadValue == record->payload1)) {
        return true;
      }
    }
  }
  return false;
}


/* Explicit command tables (no original counterpart as tables). The original executes a received command by
   calling the address codeBase + code, so any function start in the handler region below the region end is
   reachable; these tables list exactly those functions with their codes, so the codes keep their protocol
   values without the original image layout. Not every entry is a four-argument command handler: the queue
   functions themselves and a few helpers start in the same regions and are listed because the original would
   call them for such a code, too.
   The handler stays an untyped void * on purpose (the one function-address table that is not typed by
   THANDOR_SLOT): the entries have different signatures (the four-argument handlers, the queue functions, the
   lookup helper), and the table identifies a command by its handler's address (CommandDispatch_CodeOfHandler,
   CommandDispatch_IsCommandHandler); only CommandDispatch_ResolveHandler casts a checked entry to
   CommandQueueHandlerProc. */
struct CommandTableEntry {
  uint32_t code;
  void *handler;
};

static const CommandTableEntry g_FrontendCommandTable[] = {
    {0x0, THANDOR_PTR(&FrontendCommandQueue_EnqueueLocalPlayerCommand)},
    {0x60, THANDOR_PTR(&FrontendCommandQueue_DequeueFirstIntoRecord)},
    {FRONTEND_COMMAND_PLAYER_READY, THANDOR_PTR(&FrontendPlayerRuntime_RecordReadyAndUpdateWaitState)},
    {FRONTEND_COMMAND_BRIEFING_READY, THANDOR_PTR(&FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton)},
    {FRONTEND_COMMAND_APPLY_GAME_SPEED, THANDOR_PTR(&FrontendSession_ApplyGameSpeedAndReturnToMainPage)},
    {FRONTEND_COMMAND_SET_GAME_SPEED, THANDOR_PTR(&FrontendSession_SetGameSpeedPercent)},
    {FRONTEND_COMMAND_RELEASE_CAMPAIGN, THANDOR_PTR(&FrontendSession_ReleaseSelectedResourceAndReturnToMainPage)},
    {FRONTEND_COMMAND_FIELD_GRID_RECEIVED, THANDOR_PTR(&FrontendPlayerRuntime_MarkLevelReceivedById)},
    {FRONTEND_COMMAND_XOR_PLAYER_STATE, THANDOR_PTR(&FrontendPlayerRuntime_XorStateMaskByPlayerId)},
    {FRONTEND_COMMAND_BUNDLE_RECEIVED, THANDOR_PTR(&FrontendPlayerRuntime_MarkLevelLoadedById)},
    {FRONTEND_COMMAND_LOAD_FIELD_GRID, THANDOR_PTR(&FrontendScenarioSession_LoadOrRequestFieldGrid)},
    {FRONTEND_COMMAND_CYCLE_FACTION_COLOUR, THANDOR_PTR(&FrontendFactionSetup_CycleFactionColour)},
    {FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE, THANDOR_PTR(&FrontendFactionSetup_ToggleFactionActive)},
    {FRONTEND_COMMAND_CHOOSE_FACTION, THANDOR_PTR(&FrontendFactionSetup_ChooseFaction)},
    {FRONTEND_COMMAND_SET_CONSENSUS_VALUE, THANDOR_PTR(&FrontendPlayerRuntime_SetConsensusValueAndRefresh)},
    {FRONTEND_COMMAND_LEVEL_RECEIVED, THANDOR_PTR(&FrontendPlayerRuntime_MarkTaskAssignmentReadyById)},
    {FRONTEND_COMMAND_LOAD_LEVEL, THANDOR_PTR(&FrontendScenarioSession_LoadOrRequestLevelAsset)},
    {FRONTEND_COMMAND_LOAD_CAMPAIGN, THANDOR_PTR(&FrontendScenarioSession_LoadOrRequestCampaignBundle)},
    {FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE, THANDOR_PTR(&FrontendSession_ReturnToMainPage)},
    {FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED, THANDOR_PTR(&FrontendPlayerRuntime_MarkScenarioCatalogReceivedById)},
    {FRONTEND_COMMAND_SHOW_SAVED_GAMES, THANDOR_PTR(&ScenarioCatalog_RebuildSaveRecordListPage)},
    {FRONTEND_COMMAND_SHOW_SINGLE_GAMES, THANDOR_PTR(&ScenarioCatalog_RebuildLevelRecordListPage)},
    {FRONTEND_COMMAND_SHOW_CAMPAIGNS, THANDOR_PTR(&ScenarioCatalog_RebuildCampaignRecordListPage)},
    {FRONTEND_COMMAND_SELECT_SAVED_GAME, THANDOR_PTR(&ScenarioCatalog_SelectSavedGameAndShowDescription)},
    {FRONTEND_COMMAND_SELECT_SINGLE_GAME, THANDOR_PTR(&ScenarioCatalog_SelectLevelAndShowDescription)},
    {FRONTEND_COMMAND_SELECT_CAMPAIGN, THANDOR_PTR(&ScenarioCatalog_SelectCampaignAndShowDescription)},
    {FRONTEND_COMMAND_STOP_ROM_TRANSITION, THANDOR_PTR(&ScenarioCatalog_RequestRomTransitionStopCallback)},
    {FRONTEND_COMMAND_EXECUTE_ROM_ACTION, THANDOR_PTR(&FrontendRomActionTable_ExecuteRecord)},
    {FRONTEND_COMMAND_CHAT_BEGIN, THANDOR_PTR(&FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById)},
    {FRONTEND_COMMAND_CHAT_APPEND, THANDOR_PTR(&FrontendPlayerMessageBuffer_AppendTripleById)},
    {FRONTEND_COMMAND_CHAT_PUBLISH, THANDOR_PTR(&FrontendPlayerMessageBuffer_PublishTextById)},
    {FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE, THANDOR_PTR(&FrontendTransfer_MarkUnavailableIfModeBit0Callback)},
    {FRONTEND_COMMAND_SNAPSHOTS_RECEIVED, THANDOR_PTR(&FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady)},
};

static const CommandTableEntry g_InGameCommandTable[] = {
    {0x0, THANDOR_PTR(&InGameCommandQueue_AppendLocalPlayerCommand)},
    {0x60, THANDOR_PTR(&InGameCommandQueue_DequeueFirstIntoRecord)},
    {0xD0, THANDOR_PTR(&InGameCommandQueue_ContainsTripletValue)},
    {INGAME_COMMAND_PLAYER_DEPARTURE, THANDOR_PTR(&InGameCommand_HandlePlayerDeparture)},
    {INGAME_COMMAND_APPLY_UI_FLAG_MASKS, THANDOR_PTR(&UiCommandRuntimeFlags_ApplyClearSetToggleMasks)},
    {INGAME_COMMAND_SET_SLOW_RENDERING, THANDOR_PTR(&FrontendPlayerRuntime_SetSlowRenderingFlagById)},
    {0x370, THANDOR_PTR(&InGameCommand_TogglePauseRequest)}, /* toggle pause (hotkey) */
    {0x3F0, THANDOR_PTR(&InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks)}, /* adjust game speed (hotkey) */
    {INGAME_COMMAND_RESULTS_READY, THANDOR_PTR(&FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton)},
    {0x550, THANDOR_PTR(&FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus)}, /* player ready: level loaded */
    {INGAME_COMMAND_ADVANCE_RELATION, THANDOR_PTR(&GameFactionRuntime_AdvancePairwiseRelationState)},
    {INGAME_COMMAND_RESET_RELATION, THANDOR_PTR(&GameFactionRuntime_ResetPairwiseRelationState)},
    {0x8F0, THANDOR_PTR(&InGameSelection_SelectAllOwnAircraftPads)}, /* select own aircraft pads (hotkey) */
    {INGAME_COMMAND_SELECT_SINGLE_ARMY, THANDOR_PTR(&FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection)},
    {INGAME_COMMAND_REPLACE_SELECTION, THANDOR_PTR(&InGamePlayerSelection_ReplaceWithArmyRuntimeIndex)},
    {INGAME_COMMAND_SELECTION_INSERT, THANDOR_PTR(&FrontendPlayerSelection_InsertThreeEntriesAndRefresh)},
    {INGAME_COMMAND_SELECTION_REMOVE, THANDOR_PTR(&FrontendPlayerSelection_RemoveThreeEntriesAndRefresh)},
    {INGAME_COMMAND_SELECTION_CLEAR, THANDOR_PTR(&FrontendPlayerSelection_ClearAndRefreshLocalPanels)},
    {INGAME_COMMAND_SELECTION_GROUP, THANDOR_PTR(&FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh)},
    {INGAME_COMMAND_MOVE, THANDOR_PTR(&InGamePlayerSelection_ApplyMoveCommand)},
    {INGAME_COMMAND_POSITION, THANDOR_PTR(&InGamePlayerSelection_ApplyPositionCommand)},
    {INGAME_COMMAND_SELECT_ARMY, THANDOR_PTR(&InGamePlayerSelection_SelectArmyRuntimeIndex)},
    {INGAME_COMMAND_TARGET_POSITION, THANDOR_PTR(&InGamePlayerSelection_ApplyTargetPositionCommand)},
    {0xE10, THANDOR_PTR(&PlayerSelection_ResetMovementPruneAndRecenterEntries)}, /* S: reset movement */
    {0xE30, THANDOR_PTR(&PlayerSelection_StopMovement)}, /* Shift+S: stop movement (stay where they are) */
    {0xE50, THANDOR_PTR(&PlayerSelection_CancelTargets)}, /* Alt+S: cancel targets */
    {0xE70, THANDOR_PTR(&PlayerSelection_SelfDestruct)}, /* Alt+D: self destruct */
    {0xE90, THANDOR_PTR(&InGameSelection_SetAircraftPadTargetLane1)},
    {0xEC0, THANDOR_PTR(&InGameSelection_SetAircraftPadTargetLane2)},
    {0xEF0, THANDOR_PTR(&SelectionMarkerCoordinates_ApplyType3)},
    {0xF20, THANDOR_PTR(&SelectionMarkerCoordinates_ApplyType4)},
    {0xF50, THANDOR_PTR(&SelectionMarkerCoordinates_ApplyType5)},
    {0xF80, THANDOR_PTR(&SelectionMarkerCoordinates_ApplyType6)},
    {0xFB0, THANDOR_PTR(&SelectionMarkerCoordinates_ApplyType7)},
    {INGAME_COMMAND_QUEUE_ARMY, THANDOR_PTR(&GameFactionRuntime_RegisterArmyAssetPointers)},
    {INGAME_COMMAND_CANCEL_QUEUED_ARMY, THANDOR_PTR(&GameFactionRuntime_CancelQueuedArmyAssetsAndRefund)},
    {INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT, THANDOR_PTR(&GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer)},
    {INGAME_COMMAND_PLACE_ARMY, THANDOR_PTR(&InGameCommand_ExecuteLocalPlacementFromSelection)},
    {INGAME_COMMAND_CONSUME_PENDING_ARMY, THANDOR_PTR(&GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid)},
    {INGAME_COMMAND_SELL_ARMY, THANDOR_PTR(&GameFactionRuntime_SellArmyAssetAndRefundSevenEighths)},
    {INGAME_COMMAND_SELECT_MODEL_AND_ARMY, THANDOR_PTR(&FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel)},
    {INGAME_COMMAND_ASSIGN_ARMY_TOKEN, THANDOR_PTR(&FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch)},
    {INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE, THANDOR_PTR(&FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology)},
    {INGAME_COMMAND_CHAT_SET_RECIPIENTS, THANDOR_PTR(&FrontendPlayerTextCommand_SetPackedState)},
    {INGAME_COMMAND_CHAT_APPEND, THANDOR_PTR(&FrontendPlayerTextCommand_AppendTripleClamped)},
    {INGAME_COMMAND_CHAT_PUBLISH, THANDOR_PTR(&FrontendPlayerTextCommand_PublishConditionalRichText)},
    {INGAME_COMMAND_EDITOR_ACTIVE_STATE, THANDOR_PTR(&InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState)},
    {INGAME_COMMAND_EDITOR_SELECT_RANGE, THANDOR_PTR(&PlayerPairList_InsertRange)},
    {INGAME_COMMAND_EDITOR_DESELECT_RANGE, THANDOR_PTR(&PlayerPairList_RemoveRange)},
    {0x1D80, THANDOR_PTR(&PlayerPairList_InsertUnique)},
    {0x1E20, THANDOR_PTR(&PlayerPairList_RemoveFirstMatch)},
    {INGAME_COMMAND_EDITOR_CLEAR_SELECTION, THANDOR_PTR(&SelectionPlayerRuntime_ClearTerrainEditSelectionState)},
    {INGAME_COMMAND_EDITOR_CLEAR_SCRATCH, THANDOR_PTR(&FieldGrid_ClearPlayerScratchPlane)},
    {INGAME_COMMAND_EDITOR_RAISE_HEIGHTS, THANDOR_PTR(&FieldGrid_ApplyPositiveCellDeltas)},
    {INGAME_COMMAND_EDITOR_LOWER_HEIGHTS, THANDOR_PTR(&FieldGrid_ApplyNegativeCellDeltas)},
    {INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS, THANDOR_PTR(&TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting)},
    {INGAME_COMMAND_EDITOR_COPY_MATERIALS, THANDOR_PTR(&TerrainEditBuffer_CopyCellMaterialBytes)},
    {INGAME_COMMAND_EDITOR_PAINT_MATERIAL, THANDOR_PTR(&FieldGrid_ApplyLocalCellUpdate)},
    {INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS, THANDOR_PTR(&TerrainEditBuffer_SubtractCurrentCellMaterialBytes)},
    {INGAME_COMMAND_EDITOR_COMMIT_MATERIALS, THANDOR_PTR(&TerrainEditBuffer_CommitFlagsAndMaterialDeltas)},
    {INGAME_COMMAND_EDITOR_REPLACE_MATCHING, THANDOR_PTR(&TerrainMaterialEdit_SeedMatchingRegionReplacement)},
    {INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET, THANDOR_PTR(&TerrainMaterialEdit_SeedNonTargetRegionReplacement)},
    {INGAME_COMMAND_EDITOR_RESET_INFLUENCE, THANDOR_PTR(&FieldGrid_ResetLocalInfluenceState)},
    {INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE, THANDOR_PTR(&FieldGrid_RebuildLocalInfluenceState)},
    {INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS, THANDOR_PTR(&TerrainEditBuffer_ConvertHeightsToDeltas)},
    {INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES, THANDOR_PTR(&WorldRuntime_TurnAuxiliaryAnglesClamped)},
    {INGAME_COMMAND_EDITOR_TURN_LIGHT, THANDOR_PTR(&TerrainLighting_AdjustDirectionAndRecomputeField)},
    {INGAME_COMMAND_DESTROY_ARMIES, THANDOR_PTR(&FrontendPlayerSelection_ApplyEntryOrAll)},
    {INGAME_COMMAND_PLACEMENT_CREATE_ARMY, THANDOR_PTR(&PlayerRuntime_CreatePlacementArmy)},
    {INGAME_COMMAND_PLACEMENT_SET_FACTION, THANDOR_PTR(&PlayerRuntime_SetPlacementFaction)},
    {INGAME_COMMAND_PLACEMENT_SET_ARMY, THANDOR_PTR(&PlayerRuntime_SetPlacementArmy)},
    {INGAME_COMMAND_PLACEMENT_MOVE, THANDOR_PTR(&SelectionPlayerRuntime_MovePrimarySelectionBy)},
    {INGAME_COMMAND_PLACEMENT_ROTATE, THANDOR_PTR(&SelectionPlayerRuntime_RotatePrimarySelectionBy)},
    {INGAME_COMMAND_PLACEMENT_CLEAR_ARMY, THANDOR_PTR(&PlayerRuntime_ClearPlacementArmy)},
    {INGAME_COMMAND_EDITOR_SAVE_MAP, THANDOR_PTR(&InGameUiCommand_SaveFieldAndLevelAssetImages)},
    {INGAME_COMMAND_TERRAIN_RELAXATION, THANDOR_PTR(&TerrainGrid_RunDirectionalRelaxationPasses)},
    {INGAME_COMMAND_EDITOR_SMOOTH, THANDOR_PTR(&FieldGrid_ApplyEncodedCellUpdate)},
    {INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED, THANDOR_PTR(&FieldGrid_SetCellFluidReceiverExcluded)},
    {INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED, THANDOR_PTR(&FieldGrid_SetCellFluidSourceExcluded)},
    {INGAME_COMMAND_EDITOR_APPLY_REGION_MASK, THANDOR_PTR(&FieldGrid_SetCellResourceSupportFlag)},
};

#define COMMAND_TABLE_COUNT(table) (sizeof(table) / sizeof((table)[0]))

/* Validation of received command records (no original counterpart). The original calls the handler with
   whatever a peer sent; the handlers index fixed arrays with payload values and turn tokens into pointers.
   Each payload dword of a code below gets one check; every other payload dword is passed on unchecked. The
   checks read only the record and state every peer shares (player blocks, field grid, the scenario lists of
   the lobby), so every peer drops the same records. Valid peers (the original game included) never send a
   value that fails a check, so valid sessions take the identical path. */
typedef enum CommandPayloadCheck {
  COMMAND_CHECK_NONE = 0,
  COMMAND_CHECK_FACTION,          /* faction index: GameFactionRuntimeImage.records[8] */
  COMMAND_CHECK_FACTION_ROW,      /* faction setup row 0..6 (the seven rows of the faction setup page) */
  COMMAND_CHECK_SELECTION_GROUP,  /* selection group 0..SELECTION_GROUP_COUNT - 1 */
  COMMAND_CHECK_ARMY_TOKEN,       /* 0 (none) or an army slot as offset from g_ArmyRuntimeRebaseBaseMinusOne */
  COMMAND_CHECK_ARMY_TOKEN_SET,   /* as COMMAND_CHECK_ARMY_TOKEN, but not 0 (the handler rebases it unconditionally) */
  COMMAND_CHECK_MODEL_TOKEN,      /* 0 (none) or a model slot as offset from g_ModelRuntimeRebaseDelta */
  COMMAND_CHECK_TECHNOLOGY,       /* 0 (none), negative (cancel) or a technology record index */
  COMMAND_CHECK_GRID_ROW,         /* Q12 field grid row, one cell of margin around the grid */
  COMMAND_CHECK_GRID_COLUMN,      /* Q12 field grid column, one cell of margin around the grid */
  COMMAND_CHECK_RELAXATION_PASSES,/* 1..TERRAIN_RELAXATION_BUTTON_PASSES (0 wraps the do/while) */
  COMMAND_CHECK_MISSION_ROW,      /* row of the missions list (frontend) */
  COMMAND_CHECK_CAMPAIGN_ROW,     /* row of the campaigns list (frontend) */
  COMMAND_CHECK_SAVED_GAME_ROW    /* row of the saved games list (frontend) */
} CommandPayloadCheck;

struct CommandValidationEntry {
  uint32_t code;
  uint8_t payloadChecks[3]; /* CommandPayloadCheck of payload1, payload2, payload3 (handler argument order) */
};

static const CommandValidationEntry g_FrontendCommandValidation[] = {
    {FRONTEND_COMMAND_CYCLE_FACTION_COLOUR, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION_ROW}},
    {FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION_ROW}},
    {FRONTEND_COMMAND_CHOOSE_FACTION, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION_ROW}},
    {FRONTEND_COMMAND_LOAD_LEVEL, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_MISSION_ROW}},
    {FRONTEND_COMMAND_LOAD_CAMPAIGN, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_CAMPAIGN_ROW}},
    {FRONTEND_COMMAND_SELECT_SAVED_GAME, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_SAVED_GAME_ROW}},
    {FRONTEND_COMMAND_SELECT_SINGLE_GAME, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_MISSION_ROW}},
    {FRONTEND_COMMAND_SELECT_CAMPAIGN, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_CAMPAIGN_ROW}},
};

static const CommandValidationEntry g_InGameCommandValidation[] = {
    {INGAME_COMMAND_ADVANCE_RELATION, {COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION,COMMAND_CHECK_FACTION}},
    {INGAME_COMMAND_RESET_RELATION, {COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION,COMMAND_CHECK_FACTION}},
    {INGAME_COMMAND_SELECT_SINGLE_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_REPLACE_SELECTION, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_SELECTION_INSERT, {COMMAND_CHECK_ARMY_TOKEN,COMMAND_CHECK_ARMY_TOKEN,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_SELECTION_REMOVE, {COMMAND_CHECK_ARMY_TOKEN,COMMAND_CHECK_ARMY_TOKEN,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_SELECTION_GROUP, {COMMAND_CHECK_FACTION,COMMAND_CHECK_NONE,COMMAND_CHECK_SELECTION_GROUP}},
    {INGAME_COMMAND_SELECT_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_QUEUE_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION}},
    {INGAME_COMMAND_CANCEL_QUEUED_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION}},
    {INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION}},
    {INGAME_COMMAND_CONSUME_PENDING_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION}},
    {INGAME_COMMAND_SELL_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION}},
    /* crossed names in the handler: payload2 is the model token, payload3 the army token */
    {INGAME_COMMAND_SELECT_MODEL_AND_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_MODEL_TOKEN,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_ASSIGN_ARMY_TOKEN, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_MODEL_TOKEN}},
    {INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE, {COMMAND_CHECK_NONE,COMMAND_CHECK_TECHNOLOGY,COMMAND_CHECK_MODEL_TOKEN}},
    /* marked editor cells: (last column, row, first column); the range loop wraps for a column near INT32_MAX */
    {INGAME_COMMAND_EDITOR_SELECT_RANGE, {COMMAND_CHECK_GRID_COLUMN,COMMAND_CHECK_GRID_ROW,COMMAND_CHECK_GRID_COLUMN}},
    {INGAME_COMMAND_EDITOR_DESELECT_RANGE, {COMMAND_CHECK_GRID_COLUMN,COMMAND_CHECK_GRID_ROW,COMMAND_CHECK_GRID_COLUMN}},
    {0x1D80, {COMMAND_CHECK_NONE,COMMAND_CHECK_GRID_ROW,COMMAND_CHECK_GRID_COLUMN}}, /* PlayerPairList_InsertUnique */
    /* the anchor cell (row, column) indexes the grid cells unclipped (FieldGrid_ProcessHorizontalSpan) */
    {INGAME_COMMAND_EDITOR_RAISE_HEIGHTS, {COMMAND_CHECK_GRID_ROW,COMMAND_CHECK_GRID_COLUMN,COMMAND_CHECK_NONE}},
    {INGAME_COMMAND_EDITOR_LOWER_HEIGHTS, {COMMAND_CHECK_GRID_ROW,COMMAND_CHECK_GRID_COLUMN,COMMAND_CHECK_NONE}},
    {INGAME_COMMAND_DESTROY_ARMIES, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_ARMY_TOKEN_SET}},
    {INGAME_COMMAND_PLACEMENT_SET_FACTION, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_FACTION}},
    /* stored as placedArmyToken and rebased by the placement move/rotate commands */
    {INGAME_COMMAND_PLACEMENT_SET_ARMY, {COMMAND_CHECK_NONE,COMMAND_CHECK_NONE,COMMAND_CHECK_ARMY_TOKEN}},
    {INGAME_COMMAND_TERRAIN_RELAXATION, {COMMAND_CHECK_NONE,COMMAND_CHECK_RELAXATION_PASSES,COMMAND_CHECK_NONE}},
};

/* Logged-once flags, parallel to the validation tables. */
static uint8_t g_FrontendCommandValidationLogged[COMMAND_TABLE_COUNT(g_FrontendCommandValidation)];
static uint8_t g_InGameCommandValidationLogged[COMMAND_TABLE_COUNT(g_InGameCommandValidation)];

/* False for the table entries that are no four-argument command handlers: the queue functions and the queue
   lookup helper, listed only because they start in the handler regions. */
static bool CommandDispatch_IsCommandHandler(const void *handler)

{
  return handler != CommandDispatch_HandlerKey(&FrontendCommandQueue_EnqueueLocalPlayerCommand) &&
         handler != CommandDispatch_HandlerKey(&FrontendCommandQueue_DequeueFirstIntoRecord) &&
         handler != CommandDispatch_HandlerKey(&InGameCommandQueue_AppendLocalPlayerCommand) &&
         handler != CommandDispatch_HandlerKey(&InGameCommandQueue_DequeueFirstIntoRecord) &&
         handler != CommandDispatch_HandlerKey(&InGameCommandQueue_ContainsTripletValue);
}

/* The handler table of a command code base (FRONTEND_COMMAND_CODE_BASE or INGAME_COMMAND_CODE_BASE). */
static const CommandTableEntry *CommandDispatch_TableForBase(uint32_t codeBase,uint32_t *outCount)

{
  if (codeBase == FRONTEND_COMMAND_CODE_BASE) {
    *outCount = COMMAND_TABLE_COUNT(g_FrontendCommandTable);
    return g_FrontendCommandTable;
  }
  if (codeBase == INGAME_COMMAND_CODE_BASE) {
    *outCount = COMMAND_TABLE_COUNT(g_InGameCommandTable);
    return g_InGameCommandTable;
  }
  *outCount = 0;
  return nullptr;
}


/* Rebuild helper (no original counterpart).
   The original executes a received command by calling codeBase + code when that address is below
   originalRegionEnd: the code is the handler's distance from the queue function in the original image. Here the code is
   looked up in the explicit command table of that base. Codes at or past the region end are skipped like in the
   original (NULL). A code that does not hit a function start would make the original jump into the middle of
   code; valid codes never do, so it is logged once and skipped.
*/
CommandQueueHandlerProc *
CommandDispatch_ResolveHandler(uint32_t codeBase,uint32_t originalRegionEnd,uint32_t code)

{
  static int s_loggedInvalidCode;
  const CommandTableEntry *table;
  uint32_t count;
  uint32_t index;

  if (codeBase + code >= originalRegionEnd) {
    return nullptr;
  }
  table = CommandDispatch_TableForBase(codeBase,&count);
  for (index = 0; index < count; index++) {
    if (table[index].code == code) {
      /* Not in the original: the queue functions (codes 0x0 and 0x60) and the in-game queue lookup helper
         (0xD0) start in the handler region, so the original would call them for such a received code, e.g.
         the dequeue function with the player id as its record pointer. No valid peer sends these codes (0 is
         the empty record and skipped by every caller), so they are ignored like a code that is no handler. */
      if (!CommandDispatch_IsCommandHandler(table[index].handler)) {
        break;
      }
      return reinterpret_cast<CommandQueueHandlerProc *>(table[index].handler); /* the table keys handlers as const void * */
    }
  }
  if (s_loggedInvalidCode == 0) {
    s_loggedInvalidCode = 1;
    Thandor_Log("network: command code 0x%X (base 0x%08X) is no command handler, ignored",code,codeBase);
  }
  return nullptr;
}


/* True when token is 0 (allowZero) or a slot start of a pool of slotCount slotBytes-sized slots, given as
   the slot's offset from pool base - 1 (the protocol's token form, see g_ArmyRuntimeRebaseBaseMinusOne and
   g_ModelRuntimeRebaseDelta). */
static bool CommandDispatch_IsPoolToken(uint32_t token,uint32_t slotBytes,uint32_t slotCount,bool allowZero)

{
  if (token == 0) {
    return allowZero;
  }
  return (token - 1) % slotBytes == 0 && (token - 1) / slotBytes < slotCount;
}

/* True when the Q12 grid coordinate names a cell index in -1..cellCount (one cell of margin: the snapped
   editor rectangle can end one cell outside the grid; the original reads such cells, too). */
static bool CommandDispatch_IsGridCoordinate(uint32_t coordinateQ12,FieldGridDimension cellCount)

{
  int cellIndex;

  cellIndex = (int)coordinateQ12 >> Q12_SHIFT;
  return -1 <= cellIndex && cellIndex <= (int)cellCount;
}

/* True when rowIndex names a row of the frontend list control, or the list has no rows at all (rowSlots NULL:
   the select handlers then only reset the description, as in the original). */
static bool CommandDispatch_IsListRow(const UiNodeBase *listNode,uint32_t rowIndex)

{
  const UiListControl *list;

  list = reinterpret_cast<const UiListControl *>(listNode); /* the node is a list control */
  return list->rowSlots == nullptr || rowIndex < list->rowCount;
}

/* Applies one CommandPayloadCheck to a payload dword. */
static bool CommandDispatch_IsPayloadValid(CommandPayloadCheck check,uint32_t value)

{
  const FieldGridAsset *fieldGrid;

  switch (check) {
  case COMMAND_CHECK_NONE:
    return true;
  case COMMAND_CHECK_FACTION:
    return value < COMMAND_TABLE_COUNT(g_GameFactionRuntimeImage.records);
  case COMMAND_CHECK_FACTION_ROW:
    return value < COMMAND_TABLE_COUNT(g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets);
  case COMMAND_CHECK_SELECTION_GROUP:
    return value < SELECTION_GROUP_COUNT;
  case COMMAND_CHECK_ARMY_TOKEN:
  case COMMAND_CHECK_ARMY_TOKEN_SET:
    return CommandDispatch_IsPoolToken(value,sizeof(ArmyRuntimeSlot),ARMY_RUNTIME_SLOT_COUNT,
                                       check == COMMAND_CHECK_ARMY_TOKEN);
  case COMMAND_CHECK_MODEL_TOKEN:
    return CommandDispatch_IsPoolToken(value,sizeof(ModelRuntimeSlot),MODEL_RUNTIME_SLOT_COUNT,true);
  case COMMAND_CHECK_TECHNOLOGY:
    return (int)value < TECHNOLOGY_RECORD_COUNT;
  case COMMAND_CHECK_GRID_ROW:
  case COMMAND_CHECK_GRID_COLUMN:
    fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
    return CommandDispatch_IsGridCoordinate
                     (value,check == COMMAND_CHECK_GRID_ROW ? fieldGrid->gridHeight : fieldGrid->gridWidth);
  case COMMAND_CHECK_RELAXATION_PASSES:
    return value != 0 && value <= TERRAIN_RELAXATION_BUTTON_PASSES;
  case COMMAND_CHECK_MISSION_ROW:
    return CommandDispatch_IsListRow(&g_FrontendRootNode->missionsList.base,value);
  case COMMAND_CHECK_CAMPAIGN_ROW:
    return CommandDispatch_IsListRow(&g_FrontendRootNode->campaignsList.base,value);
  case COMMAND_CHECK_SAVED_GAME_ROW:
    return CommandDispatch_IsListRow(&g_FrontendRootNode->savedGamesList.base,value);
  }
  return true;
}

/* Rebuild helper (no original counterpart): whether a received record of codeBase may reach its handler. An
   in-game record must name a player with a selection block (every in-game handler may index
   g_SelectionPlayerRuntimeBlockPointers with it; the frontend handlers search the player list instead), and
   the payload dwords must pass the checks of the code's validation entry. A rejected record is logged once per
   code (the player id once overall). */
static bool CommandDispatch_ValidateRecord(uint32_t codeBase,const UiCommandQueueRecord *record)

{
  static int s_loggedUnknownPlayer;
  const CommandValidationEntry *table;
  uint8_t *loggedFlags;
  uint32_t count;
  uint32_t index;
  uint32_t code;
  uint32_t playerId;

  code = record->packedCommandAndPlayerId >> 8;
  playerId = record->packedCommandAndPlayerId & 0xff;
  if (codeBase == INGAME_COMMAND_CODE_BASE) {
    if (g_SelectionPlayerRuntimeBlockPointers[playerId] == nullptr) {
      if (s_loggedUnknownPlayer == 0) {
        s_loggedUnknownPlayer = 1;
        Thandor_Log("network: command 0x%X names player %u without a player block, dropped",code,playerId);
      }
      return false;
    }
    table = g_InGameCommandValidation;
    loggedFlags = g_InGameCommandValidationLogged;
    count = COMMAND_TABLE_COUNT(g_InGameCommandValidation);
  }
  else {
    table = g_FrontendCommandValidation;
    loggedFlags = g_FrontendCommandValidationLogged;
    count = COMMAND_TABLE_COUNT(g_FrontendCommandValidation);
  }
  for (index = 0; index < count; index++) {
    if (table[index].code != code) {
      continue;
    }
    if (CommandDispatch_IsPayloadValid((CommandPayloadCheck)table[index].payloadChecks[0],record->payload1) &&
        CommandDispatch_IsPayloadValid((CommandPayloadCheck)table[index].payloadChecks[1],record->payload2) &&
        CommandDispatch_IsPayloadValid((CommandPayloadCheck)table[index].payloadChecks[2],record->payload3)) {
      return true;
    }
    if (loggedFlags[index] == 0) {
      loggedFlags[index] = 1;
      Thandor_Log("network: command 0x%X (base 0x%08X) payload 0x%X 0x%X 0x%X out of range, dropped",code,
                  codeBase,record->payload1,record->payload2,record->payload3);
    }
    return false;
  }
  return true;
}


/* Rebuild helper (no original counterpart): executes one received command record of codeBase. The original
   calls codeBase + code with (player id, payload1, payload2, payload3) when the code is not 0 and lies below
   the handler region end; here the code is resolved by CommandDispatch_ResolveHandler and the record must
   pass CommandDispatch_ValidateRecord. */
void CommandDispatch_ExecuteRecord(uint32_t codeBase,uint32_t originalRegionEnd,const UiCommandQueueRecord *record)

{
  CommandQueueHandlerProc *commandHandler;
  uint32_t code;

  code = record->packedCommandAndPlayerId >> 8;
  if (code == 0) {
    return;
  }
  commandHandler = CommandDispatch_ResolveHandler(codeBase,originalRegionEnd,code);
  if (commandHandler == nullptr || !CommandDispatch_ValidateRecord(codeBase,record)) {
    return;
  }
  (*commandHandler)(record->packedCommandAndPlayerId & 0xff,record->payload1,record->payload2,record->payload3);
}


/* Rebuild helper (no original counterpart): the command code of handler in the table of codeBase (the
   original computes it as the handler's address minus the base), or 0xFFFFFFFF when the function is no command
   handler. */
uint32_t CommandDispatch_CodeOfHandler(uint32_t codeBase,const void *handler)

{
  const CommandTableEntry *table;
  uint32_t count;
  uint32_t index;

  table = CommandDispatch_TableForBase(codeBase,&count);
  for (index = 0; index < count; index++) {
    if (table[index].handler == handler) {
      return table[index].code;
    }
  }
  return 0xFFFFFFFFu;
}


/* Rebuild helper (no original counterpart), declared in commands.h: InGameCommand_Issue for a handler chosen
   at run time. */
void InGameCommand_IssueHandler(CommandQueueHandlerProc *handler,CommandPayload payload1,CommandPayload payload2,
          CommandPayload payload3)

{
  static bool s_loggedMissing;
  uint32_t code;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    handler(g_LocalPlayerRuntimeId,payload1,payload2,payload3);
    return;
  }
  code = CommandDispatch_CodeOfHandler(INGAME_COMMAND_CODE_BASE,CommandDispatch_HandlerKey(handler));
  if (code == 0xFFFFFFFFu) {
    if (s_loggedMissing == 0) {
      s_loggedMissing = 1;
      Thandor_Log("network: handler %p is not in the in-game command table, command dropped",CommandDispatch_HandlerKey(handler));
    }
    return;
  }
  InGameCommandQueue_AppendLocalPlayerCommand((UiActionId)code,payload1,payload2,payload3);
}

/* Startup check (no original counterpart): the issue helpers queue the code CommandDispatch_CodeOfHandler
   derives from the tables, so every handler must map back to its own entry's code (a handler listed twice
   would queue the first code), and the pilot sites of InGameCommand_Issue must derive the constants they
   queued before. Logs each mismatch; runs once during static initialisation (the tables are in this file). */
static bool CommandDispatch_CheckDerivedCodes(void)

{
  static const struct {
    uint32_t code;
    const void *handler;
  } s_pilotCodes[] = {
      {INGAME_COMMAND_CONSUME_PENDING_ARMY,reinterpret_cast<const void *>(&GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid)},
      {INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT,reinterpret_cast<const void *>(&GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer)},
      {INGAME_COMMAND_SELL_ARMY,reinterpret_cast<const void *>(&GameFactionRuntime_SellArmyAssetAndRefundSevenEighths)}, /* handler keys (function addresses) */
  };
  static const uint32_t s_codeBases[2] = {FRONTEND_COMMAND_CODE_BASE,INGAME_COMMAND_CODE_BASE};
  const CommandTableEntry *table;
  uint32_t count;
  uint32_t index;
  uint32_t baseIndex;
  uint32_t derived;
  bool allMatch;

  allMatch = true;
  for (baseIndex = 0; baseIndex < 2; baseIndex++) {
    table = CommandDispatch_TableForBase(s_codeBases[baseIndex],&count);
    for (index = 0; index < count; index++) {
      derived = CommandDispatch_CodeOfHandler(s_codeBases[baseIndex],table[index].handler);
      if (derived != table[index].code) {
        allMatch = false;
        Thandor_Log("network: command 0x%X (base 0x%08X) derives code 0x%X from its handler",table[index].code,
                    s_codeBases[baseIndex],derived);
      }
    }
  }
  for (index = 0; index < COMMAND_TABLE_COUNT(s_pilotCodes); index++) {
    derived = CommandDispatch_CodeOfHandler(INGAME_COMMAND_CODE_BASE,s_pilotCodes[index].handler);
    if (derived != s_pilotCodes[index].code) {
      allMatch = false;
      Thandor_Log("network: in-game command 0x%X derives code 0x%X from its handler",s_pilotCodes[index].code,
                  derived);
    }
  }
  return allMatch;
}

static const bool g_CommandDerivedCodesMatch = CommandDispatch_CheckDerivedCodes();
