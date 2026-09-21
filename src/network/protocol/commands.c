/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/commands.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/commands.h>

/* Implementation ownership: network/protocol/commands. */

/* Address: 0x00543F50.
   Ownership: network/protocol/commands.
   Purpose: Appends one 0x10-byte command record to the bounded sixteen-record frontend queue. RET 0x10 proves four
   stack arguments. LOBBY command stack enqueue: dispatch base + (cmd>>8), bounded by the lobby handler-region end.
   The 0x1540 chat RPC enqueues here (L3 closure — not an entity base). Typed parameters: p0
   commandCode→UiActionId_V338.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendCommandQueue_EnqueueLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04)

{
  uint packedCommandAndPlayerId;
  UiCommandQueueRecord *writeRecord;
  
  writeRecord = g_FrontendCommandQueueEnd;
  packedCommandAndPlayerId = commandCode << 8 | g_LocalPlayerRuntimeId;
  if (g_FrontendCommandQueueEnd < &g_FrontendCommandQueueEnd) {
    g_FrontendCommandQueueEnd->payloadDword04 = payloadDword04;
    writeRecord->payloadDword08 = payloadDword08;
    writeRecord->payloadDword0C = payloadDword0C;
    writeRecord->packedCommandAndPlayerId = packedCommandAndPlayerId;
    g_FrontendCommandQueueEnd = g_FrontendCommandQueueEnd + 1;
  }
  return;
}


/* Address: 0x00543FB0.
   Ownership: network/protocol/commands.
   Purpose: Writes one UiCommandQueueRecord into FrontendCommandPacketRecord.command at +0x10, or clears the packed
   command dword when the queue is empty. Lobby stack dequeue into a FrontendCommandPacketRecord (0x20 batch unit).
*/
void __thandor_void_preserve_ecx_edx
FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord)

{
  int firstRecordDwordsRemaining;
  uint trailingDwordCount;
  UiCommandQueueRecord *copySourceCursor;
  UiCommandQueueRecord *outputRecordWriteCursor;
  UiCommandQueueRecord *copyDestinationCursor;
  UiCommandQueueRecord *queueEndSnapshot;
  
  queueEndSnapshot = g_FrontendCommandQueueEnd;
  if (g_FrontendCommandQueueEnd == g_FrontendCommandQueueRecords) {
    (outputRecord->command).packedCommandAndPlayerId = 0;
    return;
  }
  copySourceCursor = g_FrontendCommandQueueRecords;
  outputRecordWriteCursor = &outputRecord->command;
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0;
      firstRecordDwordsRemaining = firstRecordDwordsRemaining + -1) {
    outputRecordWriteCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
    copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
    outputRecordWriteCursor = (UiCommandQueueRecord *)&outputRecordWriteCursor->payloadDword04;
  }
  copyDestinationCursor = g_FrontendCommandQueueRecords;
  trailingDwordCount = (uint)(queueEndSnapshot + -0x543e5) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount = trailingDwordCount - 1) {
      copyDestinationCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
      copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
      copyDestinationCursor = (UiCommandQueueRecord *)&copyDestinationCursor->payloadDword04;
    }
  }
  g_FrontendCommandQueueEnd = g_FrontendCommandQueueEnd + -1;
  return;
}


/* Address: 0x0055F130.
   Ownership: network/protocol/commands.
   Purpose: Appends one 0x10-byte local-player command record to the bounded sixteen-record in-game queue. RET 0x10
   proves four stack arguments. IN-GAME lockstep stack append — separate dispatch base from the lobby stack (N4
   phase split). Typed parameters: p0 commandCode→UiActionId_V338. Nearby but non-identical semantic domains were
   explicitly deferred.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandQueue_AppendLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04)

{
  uint packedCommandAndPlayerId;
  UiCommandQueueRecord *writeRecord;
  
  writeRecord = g_InGameCommandQueueEnd;
  packedCommandAndPlayerId = commandCode << 8 | g_LocalPlayerRuntimeId;
  if (g_InGameCommandQueueEnd < &g_InGameCommandQueueEnd) {
    g_InGameCommandQueueEnd->payloadDword04 = payloadDword04;
    writeRecord->payloadDword08 = payloadDword08;
    writeRecord->payloadDword0C = payloadDword0C;
    writeRecord->packedCommandAndPlayerId = packedCommandAndPlayerId;
    g_InGameCommandQueueEnd = g_InGameCommandQueueEnd + 1;
  }
  return;
}


/* Address: 0x0055F190.
   Ownership: network/protocol/commands.
   Purpose: Writes one UiCommandQueueRecord into FrontendCommandPacketRecord.command at +0x10, or clears the packed
   command dword when the queue is empty. In-game stack dequeue.
*/
void __thandor_void_preserve_ecx_edx
InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord)

{
  int firstRecordDwordsRemaining;
  uint trailingDwordCount;
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
  for (firstRecordDwordsRemaining = 4; firstRecordDwordsRemaining != 0;
      firstRecordDwordsRemaining = firstRecordDwordsRemaining + -1) {
    outputRecordWriteCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
    copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
    outputRecordWriteCursor = (UiCommandQueueRecord *)&outputRecordWriteCursor->payloadDword04;
  }
  copyDestinationCursor = g_InGameCommandQueueRecords;
  trailingDwordCount = (uint)(queueEndSnapshot + -0x55efd) >> 2;
  if (trailingDwordCount != 0) {
    for (; trailingDwordCount != 0; trailingDwordCount = trailingDwordCount - 1) {
      copyDestinationCursor->packedCommandAndPlayerId = copySourceCursor->packedCommandAndPlayerId;
      copySourceCursor = (UiCommandQueueRecord *)&copySourceCursor->payloadDword04;
      copyDestinationCursor = (UiCommandQueueRecord *)&copyDestinationCursor->payloadDword04;
    }
  }
  g_InGameCommandQueueEnd = g_InGameCommandQueueEnd + -1;
  return;
}


/* Address: 0x0055F200.
   Ownership: network/protocol/commands.
   Purpose: Scans the fixed 0x10-byte queued-command records for one packed player/key value and any of three
   payload values; returns carry set on a match. Typed parameters: p2
   payloadValue→InGameCommandPayloadTripletValue32_V345, p3
   commandHandlerAddress→InGameCommandHandlerAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
InGameCommandQueue_ContainsTripletValueCf
          (InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress)

{
  UiCommandQueueRecord *pUVar1;
  UiCommandQueueRecord *pUVar2;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    pUVar1 = g_InGameCommandQueueRecords;
    while (pUVar2 = pUVar1, pUVar2 < g_InGameCommandQueueEnd) {
      pUVar1 = pUVar2 + 1;
      if ((((commandHandlerAddress + -0x55f130) * 0x100 | g_LocalPlayerRuntimeId) ==
           pUVar2->packedCommandAndPlayerId) &&
         (((payloadValue == pUVar2->payloadDword04 || (payloadValue == pUVar2->payloadDword08)) ||
          (payloadValue == pUVar2->payloadDword0C)))) {
        return true;
      }
    }
  }
  return false;
}

