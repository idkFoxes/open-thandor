/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/commands.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: network/protocol/commands. */

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
  return;
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
  copySourceCursor = (uint32_t *)g_FrontendCommandQueueRecords;
  outputRecordWriteCursor = (uint32_t *)&outputRecord->command;
  /* dword-wise copies (REP MOVSD in the original) */
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0; firstRecordDwordsRemaining--) {
    *outputRecordWriteCursor = *copySourceCursor;
    copySourceCursor++;
    outputRecordWriteCursor++;
  }
  copyDestinationCursor = (uint32_t *)g_FrontendCommandQueueRecords;
  trailingDwordCount = (uint32_t)((uint8_t *)queueEndSnapshot - (uint8_t *)&g_FrontendCommandQueueRecords[1]) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount--) {
      *copyDestinationCursor = *copySourceCursor;
      copySourceCursor++;
      copyDestinationCursor++;
    }
  }
  g_FrontendCommandQueueEnd--;
  return;
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
  return;
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
  copySourceCursor = (uint32_t *)g_InGameCommandQueueRecords;
  outputRecordWriteCursor = (uint32_t *)&outputRecord->command;
  /* REP MOVSD of the first record (4 dwords), then of the rest of the queue onto the start */
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0; firstRecordDwordsRemaining--) {
    *outputRecordWriteCursor = *copySourceCursor;
    copySourceCursor++;
    outputRecordWriteCursor++;
  }
  copyDestinationCursor = (uint32_t *)g_InGameCommandQueueRecords;
  trailingDwordCount = (uint32_t)((uint8_t *)queueEndSnapshot - (uint8_t *)&g_InGameCommandQueueRecords[1]) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount--) {
      *copyDestinationCursor = *copySourceCursor;
      copySourceCursor++;
      copyDestinationCursor++;
    }
  }
  g_InGameCommandQueueEnd--;
  return;
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
   calling codeBase + code (CALL EAX), so any function start in the handler region below the region end is
   reachable; these tables list exactly those functions with their codes, so the codes keep their protocol
   values without the original image layout. Not every entry is a four-argument command handler: the queue
   functions themselves and a few helpers start in the same regions and are listed because the original would
   call them for such a code, too. */
typedef struct CommandTableEntry {
  uint32_t code;
  void *handler;
} CommandTableEntry;

static const CommandTableEntry g_FrontendCommandTable[] = {
    {0x0, (void *)&FrontendCommandQueue_EnqueueLocalPlayerCommand},
    {0x60, (void *)&FrontendCommandQueue_DequeueFirstIntoRecord},
    {FRONTEND_COMMAND_PLAYER_READY, (void *)&FrontendPlayerRuntime_RecordReadyAndUpdateWaitState},
    {FRONTEND_COMMAND_BRIEFING_READY, (void *)&FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton},
    {FRONTEND_COMMAND_APPLY_GAME_SPEED, (void *)&FrontendSession_ApplyGameSpeedAndReturnToMainPage},
    {FRONTEND_COMMAND_SET_GAME_SPEED, (void *)&FrontendSession_SetGameSpeedPercent},
    {FRONTEND_COMMAND_RELEASE_CAMPAIGN, (void *)&FrontendSession_ReleaseSelectedResourceAndReturnToMainPage},
    {FRONTEND_COMMAND_FIELD_GRID_RECEIVED, (void *)&FrontendPlayerRuntime_MarkLevelReceivedById},
    {FRONTEND_COMMAND_XOR_PLAYER_STATE, (void *)&FrontendPlayerRuntime_XorStateMaskByPlayerId},
    {FRONTEND_COMMAND_BUNDLE_RECEIVED, (void *)&FrontendPlayerRuntime_MarkLevelLoadedById},
    {FRONTEND_COMMAND_LOAD_FIELD_GRID, (void *)&FrontendScenarioSession_LoadOrRequestFieldGrid},
    {FRONTEND_COMMAND_CYCLE_FACTION_COLOUR, (void *)&FrontendFactionSetup_CycleFactionColour},
    {FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE, (void *)&FrontendFactionSetup_ToggleFactionActive},
    {FRONTEND_COMMAND_CHOOSE_FACTION, (void *)&FrontendFactionSetup_ChooseFaction},
    {FRONTEND_COMMAND_SET_CONSENSUS_VALUE, (void *)&FrontendPlayerRuntime_SetConsensusValueAndRefresh},
    {FRONTEND_COMMAND_LEVEL_RECEIVED, (void *)&FrontendPlayerRuntime_MarkTaskAssignmentReadyById},
    {FRONTEND_COMMAND_LOAD_LEVEL, (void *)&FrontendScenarioSession_LoadOrRequestLevelAsset},
    {FRONTEND_COMMAND_LOAD_CAMPAIGN, (void *)&FrontendScenarioSession_LoadOrRequestCampaignBundle},
    {FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE, (void *)&FrontendSession_ReturnToMainPage},
    {FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED, (void *)&FrontendPlayerRuntime_MarkScenarioCatalogReceivedById},
    {FRONTEND_COMMAND_SHOW_SAVED_GAMES, (void *)&ScenarioCatalog_RebuildSaveRecordListPage},
    {FRONTEND_COMMAND_SHOW_SINGLE_GAMES, (void *)&ScenarioCatalog_RebuildLevelRecordListPage},
    {FRONTEND_COMMAND_SHOW_CAMPAIGNS, (void *)&ScenarioCatalog_RebuildCampaignRecordListPage},
    {FRONTEND_COMMAND_SELECT_SAVED_GAME, (void *)&ScenarioCatalog_SelectSavedGameAndShowDescription},
    {FRONTEND_COMMAND_SELECT_SINGLE_GAME, (void *)&ScenarioCatalog_SelectLevelAndShowDescription},
    {FRONTEND_COMMAND_SELECT_CAMPAIGN, (void *)&ScenarioCatalog_SelectCampaignAndShowDescription},
    {FRONTEND_COMMAND_STOP_ROM_TRANSITION, (void *)&ScenarioCatalog_RequestRomTransitionStopCallback},
    {FRONTEND_COMMAND_EXECUTE_ROM_ACTION, (void *)&FrontendRomActionTable_ExecuteRecord},
    {FRONTEND_COMMAND_CHAT_BEGIN, (void *)&FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById},
    {FRONTEND_COMMAND_CHAT_APPEND, (void *)&FrontendPlayerMessageBuffer_AppendTripleById},
    {FRONTEND_COMMAND_CHAT_PUBLISH, (void *)&FrontendPlayerMessageBuffer_PublishTextById},
    {FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE, (void *)&FrontendTransfer_MarkUnavailableIfModeBit0Callback},
    {FRONTEND_COMMAND_SNAPSHOTS_RECEIVED, (void *)&FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady},
};

static const CommandTableEntry g_InGameCommandTable[] = {
    {0x0, (void *)&InGameCommandQueue_AppendLocalPlayerCommand},
    {0x60, (void *)&InGameCommandQueue_DequeueFirstIntoRecord},
    {0xD0, (void *)&InGameCommandQueue_ContainsTripletValue},
    {INGAME_COMMAND_PLAYER_DEPARTURE, (void *)&InGameCommand_HandlePlayerDeparture},
    {INGAME_COMMAND_APPLY_UI_FLAG_MASKS, (void *)&UiCommandRuntimeFlags_ApplyClearSetToggleMasks},
    {INGAME_COMMAND_SET_SLOW_RENDERING, (void *)&FrontendPlayerRuntime_SetSlowRenderingFlagById},
    {0x370, (void *)&InGameCommand_TogglePauseRequest},
    {0x3F0, (void *)&InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks},
    {INGAME_COMMAND_RESULTS_READY, (void *)&FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton},
    {0x550, (void *)&FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus},
    {INGAME_COMMAND_ADVANCE_RELATION, (void *)&GameFactionRuntime_AdvancePairwiseRelationState},
    {INGAME_COMMAND_RESET_RELATION, (void *)&GameFactionRuntime_ResetPairwiseRelationState},
    {0x8F0, (void *)&InGameSelection_SelectAllOwnAircraftPads},
    {INGAME_COMMAND_SELECT_SINGLE_ARMY, (void *)&FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection},
    {INGAME_COMMAND_REPLACE_SELECTION, (void *)&InGamePlayerSelection_ReplaceWithArmyRuntimeIndex},
    {INGAME_COMMAND_SELECTION_INSERT, (void *)&FrontendPlayerSelection_InsertThreeEntriesAndRefresh},
    {INGAME_COMMAND_SELECTION_REMOVE, (void *)&FrontendPlayerSelection_RemoveThreeEntriesAndRefresh},
    {INGAME_COMMAND_SELECTION_CLEAR, (void *)&FrontendPlayerSelection_ClearAndRefreshLocalPanels},
    {INGAME_COMMAND_SELECTION_GROUP, (void *)&FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh},
    {INGAME_COMMAND_MOVE, (void *)&InGamePlayerSelection_ApplyMoveCommand},
    {INGAME_COMMAND_POSITION, (void *)&InGamePlayerSelection_ApplyPositionCommand},
    {INGAME_COMMAND_SELECT_ARMY, (void *)&InGamePlayerSelection_SelectArmyRuntimeIndex},
    {INGAME_COMMAND_TARGET_POSITION, (void *)&InGamePlayerSelection_ApplyTargetPositionCommand},
    {0xE10, (void *)&PlayerSelection_ResetMovementPruneAndRecenterEntries},
    {0xE30, (void *)&PlayerSelection_StopMovement},
    {0xE50, (void *)&PlayerSelection_CancelTargets},
    {0xE70, (void *)&PlayerSelection_SelfDestruct},
    {0xE90, (void *)&InGameSelection_SetAircraftPadTargetLane1},
    {0xEC0, (void *)&InGameSelection_SetAircraftPadTargetLane2},
    {0xEF0, (void *)&SelectionMarkerCoordinates_ApplyType3},
    {0xF20, (void *)&SelectionMarkerCoordinates_ApplyType4},
    {0xF50, (void *)&SelectionMarkerCoordinates_ApplyType5},
    {0xF80, (void *)&SelectionMarkerCoordinates_ApplyType6},
    {0xFB0, (void *)&SelectionMarkerCoordinates_ApplyType7},
    {INGAME_COMMAND_QUEUE_ARMY, (void *)&GameFactionRuntime_RegisterArmyAssetPointers},
    {INGAME_COMMAND_CANCEL_QUEUED_ARMY, (void *)&GameFactionRuntime_CancelQueuedArmyAssetsAndRefund},
    {INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT, (void *)&GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer},
    {INGAME_COMMAND_PLACE_ARMY, (void *)&InGameCommand_ExecuteLocalPlacementFromSelection},
    {INGAME_COMMAND_CONSUME_PENDING_ARMY, (void *)&GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid},
    {INGAME_COMMAND_SELL_ARMY, (void *)&GameFactionRuntime_SellArmyAssetAndRefundSevenEighths},
    {INGAME_COMMAND_SELECT_MODEL_AND_ARMY, (void *)&FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel},
    {INGAME_COMMAND_ASSIGN_ARMY_TOKEN, (void *)&FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch},
    {INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE, (void *)&FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology},
    {INGAME_COMMAND_CHAT_SET_RECIPIENTS, (void *)&FrontendPlayerTextCommand_SetPackedState},
    {INGAME_COMMAND_CHAT_APPEND, (void *)&FrontendPlayerTextCommand_AppendTripleClamped},
    {INGAME_COMMAND_CHAT_PUBLISH, (void *)&FrontendPlayerTextCommand_PublishConditionalRichText},
    {INGAME_COMMAND_EDITOR_ACTIVE_STATE, (void *)&InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState},
    {INGAME_COMMAND_EDITOR_SELECT_RANGE, (void *)&PlayerPairList_InsertRange},
    {INGAME_COMMAND_EDITOR_DESELECT_RANGE, (void *)&PlayerPairList_RemoveRange},
    {0x1D80, (void *)&PlayerPairList_InsertUnique},
    {0x1E20, (void *)&PlayerPairList_RemoveFirstMatch},
    {INGAME_COMMAND_EDITOR_CLEAR_SELECTION, (void *)&SelectionPlayerRuntime_ClearTerrainEditSelectionState},
    {INGAME_COMMAND_EDITOR_CLEAR_SCRATCH, (void *)&FieldGrid_ClearPlayerScratchPlane},
    {INGAME_COMMAND_EDITOR_RAISE_HEIGHTS, (void *)&FieldGrid_ApplyPositiveCellDeltas},
    {INGAME_COMMAND_EDITOR_LOWER_HEIGHTS, (void *)&FieldGrid_ApplyNegativeCellDeltas},
    {INGAME_COMMAND_EDITOR_COMMIT_HEIGHTS, (void *)&TerrainEditBuffer_CommitHeightDeltasAndRefreshLighting},
    {INGAME_COMMAND_EDITOR_COPY_MATERIALS, (void *)&TerrainEditBuffer_CopyCellMaterialBytes},
    {INGAME_COMMAND_EDITOR_PAINT_MATERIAL, (void *)&FieldGrid_ApplyLocalCellUpdate},
    {INGAME_COMMAND_EDITOR_SUBTRACT_MATERIALS, (void *)&TerrainEditBuffer_SubtractCurrentCellMaterialBytes},
    {INGAME_COMMAND_EDITOR_COMMIT_MATERIALS, (void *)&TerrainEditBuffer_CommitFlagsAndMaterialDeltas},
    {INGAME_COMMAND_EDITOR_REPLACE_MATCHING, (void *)&TerrainMaterialEdit_SeedMatchingRegionReplacement},
    {INGAME_COMMAND_EDITOR_REPLACE_NON_TARGET, (void *)&TerrainMaterialEdit_SeedNonTargetRegionReplacement},
    {INGAME_COMMAND_EDITOR_RESET_INFLUENCE, (void *)&FieldGrid_ResetLocalInfluenceState},
    {INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE, (void *)&FieldGrid_RebuildLocalInfluenceState},
    {INGAME_COMMAND_EDITOR_HEIGHTS_TO_DELTAS, (void *)&TerrainEditBuffer_ConvertHeightsToDeltas},
    {INGAME_COMMAND_EDITOR_TURN_AUXILIARY_ANGLES, (void *)&WorldRuntime_TurnAuxiliaryAnglesClamped},
    {INGAME_COMMAND_EDITOR_TURN_LIGHT, (void *)&TerrainLighting_AdjustDirectionAndRecomputeField},
    {INGAME_COMMAND_DESTROY_ARMIES, (void *)&FrontendPlayerSelection_ApplyEntryOrAll},
    {INGAME_COMMAND_PLACEMENT_CREATE_ARMY, (void *)&PlayerRuntime_CreatePlacementArmy},
    {INGAME_COMMAND_PLACEMENT_SET_FACTION, (void *)&PlayerRuntime_SetPlacementFaction},
    {INGAME_COMMAND_PLACEMENT_SET_ARMY, (void *)&PlayerRuntime_SetPlacementArmy},
    {INGAME_COMMAND_PLACEMENT_MOVE, (void *)&SelectionPlayerRuntime_MovePrimarySelectionBy},
    {INGAME_COMMAND_PLACEMENT_ROTATE, (void *)&SelectionPlayerRuntime_RotatePrimarySelectionBy},
    {INGAME_COMMAND_PLACEMENT_CLEAR_ARMY, (void *)&PlayerRuntime_ClearPlacementArmy},
    {INGAME_COMMAND_EDITOR_SAVE_MAP, (void *)&InGameUiCommand_SaveFieldAndLevelAssetImages},
    {INGAME_COMMAND_TERRAIN_RELAXATION, (void *)&TerrainGrid_RunDirectionalRelaxationPasses},
    {INGAME_COMMAND_EDITOR_SMOOTH, (void *)&FieldGrid_ApplyEncodedCellUpdate},
    {INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED, (void *)&FieldGrid_SetCellFluidReceiverExcluded},
    {INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED, (void *)&FieldGrid_SetCellFluidSourceExcluded},
    {INGAME_COMMAND_EDITOR_APPLY_REGION_MASK, (void *)&FieldGrid_SetCellResourceSupportFlag},
};

#define COMMAND_TABLE_COUNT(table) (sizeof(table) / sizeof((table)[0]))

/* False for the table entries that are no four-argument command handlers: the queue functions and the queue
   lookup helper, listed only because they start in the handler regions. */
static bool CommandDispatch_IsCommandHandler(const void *handler)

{
  return handler != (const void *)&FrontendCommandQueue_EnqueueLocalPlayerCommand &&
         handler != (const void *)&FrontendCommandQueue_DequeueFirstIntoRecord &&
         handler != (const void *)&InGameCommandQueue_AppendLocalPlayerCommand &&
         handler != (const void *)&InGameCommandQueue_DequeueFirstIntoRecord &&
         handler != (const void *)&InGameCommandQueue_ContainsTripletValue;
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
  return NULL;
}


/* Rebuild helper (no original counterpart).
   The original executes a received command with ADD EAX,codeBase; CMP EAX,originalRegionEnd; JNC skip;
   CALL EAX: the code is the handler's distance from the queue function in the original image. Here the code is
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
    return NULL;
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
      return (CommandQueueHandlerProc *)table[index].handler;
    }
  }
  if (s_loggedInvalidCode == 0) {
    s_loggedInvalidCode = 1;
    Thandor_Log("network: command code 0x%X (base 0x%08X) is no command handler, ignored",code,codeBase);
  }
  return NULL;
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
