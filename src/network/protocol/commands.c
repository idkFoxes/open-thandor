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

/* Address: 0x00543F50.
   Queues a lobby (frontend) command of the local player for the next network command batch: one 16-byte
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


/* Address: 0x00543FB0.
   Takes the oldest queued lobby command for the outgoing network batch: copies the first 16-byte queue
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


/* Address: 0x0055F130.
   Queues an in-game command of the local player for the next lockstep command batch: one 16-byte record
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


/* Address: 0x0055F190.
   Takes the oldest queued in-game command into outputRecord->command (offset 0x10 of the packet record)
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


/* Address: 0x0055F200.
   Tells whether the local player already queued the command whose handler lives at commandHandlerAddress
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


/* Rebuild helper (no original counterpart).
   The original executes a received command with `ADD EAX,codeBase; CMP EAX,originalRegionEnd; JNC skip;
   CALL EAX`: the code is the handler's distance from the queue function in the original image. Here the
   original handler address is resolved to its recovered C function. Codes at or past the region end are
   skipped like in the original (NULL). A code that does not hit an original function start would make the
   original jump into the middle of code; valid codes never do, so it is logged once and skipped.
*/
CommandQueueHandlerProc *
CommandDispatch_ResolveHandler(uint32_t codeBase,uint32_t originalRegionEnd,uint32_t code)

{
  static int s_loggedInvalidCode;
  uint32_t originalAddress;
  CommandQueueHandlerProc *handler;

  originalAddress = codeBase + code;
  if (originalAddress >= originalRegionEnd) {
    return NULL;
  }
  handler = (CommandQueueHandlerProc *)Thandor_FunctionAtOriginalAddress(originalAddress);
  if ((handler == NULL) && (s_loggedInvalidCode == 0)) {
    s_loggedInvalidCode = 1;
    Thandor_Log("network: command code 0x%X (base 0x%08X) is no original function start 0x%08X, ignored",
                code, codeBase, originalAddress);
  }
  return handler;
}

