/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/commands.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/commands.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static UiCommandQueueRecord g_FrontendCommandQueueRecords[16] = {0};

/* followed by 12 bytes 0x90 fill (dropped) */
static UiCommandQueueRecord *g_FrontendCommandQueueEnd = g_FrontendCommandQueueRecords;

static UiCommandQueueRecord g_InGameCommandQueueRecords[16] = {0};

static UiCommandQueueRecord *g_InGameCommandQueueEnd = THANDOR_PTR(&g_InGameCommandQueueRecords);

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
  /* dword-wise copies: the first record, then the rest of the queue onto the start */
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
  /* dword-wise copies: the first record (4 dwords), then the rest of the queue onto the start */
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
Bool8 InGameCommandQueue_ContainsTripletValue(InGameCommandPayloadTripletValue32 payloadValue,
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
   call them for such a code, too. */
typedef struct CommandTableEntry {
  uint32_t code;
  void *handler;
} CommandTableEntry;

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
    {0x370, THANDOR_PTR(&InGameCommand_TogglePauseRequest)},
    {0x3F0, THANDOR_PTR(&InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks)},
    {INGAME_COMMAND_RESULTS_READY, THANDOR_PTR(&FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton)},
    {0x550, THANDOR_PTR(&FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus)},
    {INGAME_COMMAND_ADVANCE_RELATION, THANDOR_PTR(&GameFactionRuntime_AdvancePairwiseRelationState)},
    {INGAME_COMMAND_RESET_RELATION, THANDOR_PTR(&GameFactionRuntime_ResetPairwiseRelationState)},
    {0x8F0, THANDOR_PTR(&InGameSelection_SelectAllOwnAircraftPads)},
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
    {0xE10, THANDOR_PTR(&PlayerSelection_ResetMovementPruneAndRecenterEntries)},
    {0xE30, THANDOR_PTR(&PlayerSelection_StopMovement)},
    {0xE50, THANDOR_PTR(&PlayerSelection_CancelTargets)},
    {0xE70, THANDOR_PTR(&PlayerSelection_SelfDestruct)},
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

/* False for the table entries that are no four-argument command handlers: the queue functions and the queue
   lookup helper, listed only because they start in the handler regions. */
static Bool8 CommandDispatch_IsCommandHandler(const void *handler)

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
