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
void __thandor_void_preserve_eax_ecx_edx
FrontendCommandQueue_EnqueueLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04)

{
  uint32_t packedCommandAndPlayerId;
  UiCommandQueueRecord *writeRecord;
  
  writeRecord = g_FrontendCommandQueueEnd;
  packedCommandAndPlayerId = commandCode << 8 | g_LocalPlayerRuntimeId;
  if (g_FrontendCommandQueueEnd < g_FrontendCommandQueueRecords + 16) { /* the end of the record buffer */
    g_FrontendCommandQueueEnd->payloadDword04 = payloadDword04;
    writeRecord->payloadDword08 = payloadDword08;
    writeRecord->payloadDword0C = payloadDword0C;
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
void __thandor_void_preserve_ecx_edx
FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord)

{
  int firstRecordDwordsRemaining;
  uint32_t trailingDwordCount;
  UiCommandQueueRecord *copySourceCursor;
  UiCommandQueueRecord *outputRecordWriteCursor;
  UiCommandQueueRecord *copyDestinationCursor;
  UiCommandQueueRecord *queueEndSnapshot;
  
  queueEndSnapshot = g_FrontendCommandQueueEnd;
  if (g_FrontendCommandQueueEnd == g_FrontendCommandQueueRecords) {
    outputRecord->command.packedCommandAndPlayerId = 0;
    return;
  }
  copySourceCursor = g_FrontendCommandQueueRecords;
  outputRecordWriteCursor = &outputRecord->command;
  /* dword-wise copies (REP MOVSD in the original) */
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0; firstRecordDwordsRemaining--) {
    outputRecordWriteCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
    copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
    outputRecordWriteCursor = (UiCommandQueueRecord *)&outputRecordWriteCursor->payloadDword04;
  }
  copyDestinationCursor = g_FrontendCommandQueueRecords;
  trailingDwordCount = (uint32_t)((uint8_t *)queueEndSnapshot - (uint8_t *)&g_FrontendCommandQueueRecords[1]) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount--) {
      copyDestinationCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
      copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
      copyDestinationCursor = (UiCommandQueueRecord *)&copyDestinationCursor->payloadDword04;
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandQueue_AppendLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04)

{
  uint32_t packedCommandAndPlayerId;
  UiCommandQueueRecord *writeRecord;
  
  writeRecord = g_InGameCommandQueueEnd;
  packedCommandAndPlayerId = commandCode << 8 | g_LocalPlayerRuntimeId;
  if (g_InGameCommandQueueEnd < g_InGameCommandQueueRecords + 16) { /* the end of the record buffer */
    g_InGameCommandQueueEnd->payloadDword04 = payloadDword04;
    writeRecord->payloadDword08 = payloadDword08;
    writeRecord->payloadDword0C = payloadDword0C;
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
void __thandor_void_preserve_ecx_edx
InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord)

{
  int firstRecordDwordsRemaining;
  uint32_t trailingDwordCount;
  UiCommandQueueRecord *copySourceCursor;
  UiCommandQueueRecord *outputRecordWriteCursor;
  UiCommandQueueRecord *copyDestinationCursor;
  UiCommandQueueRecord *queueEndSnapshot;
  
  queueEndSnapshot = g_InGameCommandQueueEnd;
  if (g_InGameCommandQueueEnd == g_InGameCommandQueueRecords) {
    (outputRecord->command).packedCommandAndPlayerId = 0;
    return;
  }
  copySourceCursor = g_InGameCommandQueueRecords;
  outputRecordWriteCursor = &outputRecord->command;
  /* REP MOVSD of the first record (4 dwords), then of the rest of the queue onto the start */
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0; firstRecordDwordsRemaining--) {
    outputRecordWriteCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
    copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
    outputRecordWriteCursor = (UiCommandQueueRecord *)&outputRecordWriteCursor->payloadDword04;
  }
  copyDestinationCursor = g_InGameCommandQueueRecords;
  trailingDwordCount = (uint32_t)((uint8_t *)queueEndSnapshot - (uint8_t *)&g_InGameCommandQueueRecords[1]) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount--) {
      copyDestinationCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
      copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
      copyDestinationCursor = (UiCommandQueueRecord *)&copyDestinationCursor->payloadDword04;
    }
  }
  g_InGameCommandQueueEnd--;
  return;
}


/* Address: 0x0055F200.
   Tells whether the local player already queued the command whose handler lives at commandHandlerAddress
   with payloadValue in any of its three payload dwords (returned in CF), so input handlers do not queue a
   selection change twice. Single player has no queue and always answers no.
*/
bool __thandor_cf_preserve_eax_ecx_edx
InGameCommandQueue_ContainsTripletValue
          (InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress)

{
  UiCommandQueueRecord *nextRecord;
  UiCommandQueueRecord *record;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    nextRecord = g_InGameCommandQueueRecords;
    while (record = nextRecord, record < g_InGameCommandQueueEnd) {
      nextRecord = record + 1;
      if ((((commandHandlerAddress - INGAME_COMMAND_CODE_BASE) * 0x100 | g_LocalPlayerRuntimeId) ==
           record->packedCommandAndPlayerId) &&
         (((payloadValue == record->payloadDword04 || (payloadValue == record->payloadDword08)) ||
          (payloadValue == record->payloadDword0C)))) {
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

