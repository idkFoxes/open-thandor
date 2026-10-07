/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/lockstep.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/lockstep.h>
#include <thandor/thandor.h>

#include <cstddef>

/* The original reads a client's commandSyncPending as the first dword of the endpoint descriptor behind the
   record's endpoint; the named field is used here, which needs it right behind the endpoint. */
static_assert(offsetof(FrontendPlayerRuntimeRecord,commandSyncPending) ==
              offsetof(FrontendPlayerRuntimeRecord,endpoint) + sizeof(UiTransferEndpointDescriptor),
              "commandSyncPending must follow the player endpoint");

bool Lockstep_AllClientsSubmitted()
{
  FrontendPlayerRuntimeRecord *clientRecord;
  int clientsRemaining;

  /* record 0 is the host itself, only the clients (records 1..n-1) are checked */
  if (g_FrontendPlayerRuntimeBlockCount - 1 != 0) {
    clientRecord = g_FrontendPlayerRuntimeBlocks + 1;
    for (clientsRemaining = g_FrontendPlayerRuntimeBlockCount - 1; clientsRemaining != 0; clientsRemaining--) {
      if (clientRecord->commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        return false;
      }
      clientRecord++;
    }
  }
  return true;
}

void Lockstep_ClearClientSubmissions()
{
  FrontendPlayerRuntimeRecord *clientRecord;
  int clientsRemaining;

  if (g_FrontendPlayerRuntimeBlockCount - 1 != 0) {
    clientRecord = g_FrontendPlayerRuntimeBlocks + 1;
    for (clientsRemaining = g_FrontendPlayerRuntimeBlockCount - 1; clientsRemaining != 0; clientsRemaining--) {
      clientRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      clientRecord++;
    }
  }
}

void Lockstep_ResendBatchOrWait(const LockstepHostChannel &channel)
{
  FrontendPlayerRuntimeRecord *clientRecord;
  FrontendPlayerRuntimeBlockCount clientsRemaining;

  clientRecord = g_FrontendPlayerRuntimeBlocks + 1;
  for (clientsRemaining = g_FrontendPlayerRuntimeBlockCount - 1; clientsRemaining != 0; clientsRemaining--) {
    if (clientRecord->commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
      UiTransfer_StagePacketAndSend(&clientRecord->endpoint,&channel.batch[0].header);
    }
    else {
      channel.waitPacket->packedTypeAndUnitCount = channel.waitType;
      UiTransfer_StagePacketAndSend(&clientRecord->endpoint,channel.waitPacket);
    }
    clientRecord++;
  }
}

uint32_t Lockstep_PackBatch
          (const LockstepHostChannel &channel,uint32_t slotCount,LockstepEmpty empty,bool clearPackedSlots)
{
  FrontendCommandPacketRecord *commandRecord;
  FrontendCommandPacketRecord *batchCursor;
  uint32_t batchCount;

  batchCount = 0;
  commandRecord = channel.commandSlots;
  batchCursor = channel.batch;
  /* The original loops do-while (a slot count of 0 would wrap); there is always at least the host's slot. */
  for (; slotCount != 0; slotCount--) {
    if ((commandRecord->command.packedCommandAndPlayerId & 0xffffff00) != 0) {
      FrontendTransfer_CopyCommandRecord(batchCursor,commandRecord);
      batchCursor++;
      batchCount++;
      if (clearPackedSlots) {
        commandRecord->command.packedCommandAndPlayerId = commandRecord->command.packedCommandAndPlayerId & 0xff;
      }
    }
    commandRecord++;
  }
  /* Original quirk: the empty test is done on the shifted count */
  if (batchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    if (empty == LockstepEmpty::SendNothing) {
      return 0;
    }
    /* Nothing pending: send the first record (the host's, empty) as a batch of one. */
    FrontendTransfer_CopyCommandRecord(batchCursor,channel.commandSlots);
    batchCount = 1;
  }
  /* the first packed record's header doubles as the batch header */
  channel.batch[0].header.packedTypeAndUnitCount = batchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | channel.batchType;
  return batchCount;
}

void Lockstep_SendBatchToClients(const LockstepHostChannel &channel,uint32_t playerCount)
{
  FrontendPlayerRuntimeRecord *clientRecord;
  uint32_t clientsRemaining;

  clientRecord = g_FrontendPlayerRuntimeBlocks + 1;
  for (clientsRemaining = playerCount - 1; clientsRemaining != 0; clientsRemaining--) {
    UiTransfer_StagePacketAndSend(&clientRecord->endpoint,&channel.batch[0].header);
    clientRecord++;
  }
}

void Lockstep_ExecuteRecords(const LockstepHostChannel &channel,uint32_t recordCount)
{
  const FrontendCommandPacketRecord *commandRecord;

  commandRecord = channel.batch;
  for (; recordCount != 0; recordCount--) {
    CommandDispatch_ExecuteRecord(channel.commandCodeBase,channel.handlerRegionEnd,&commandRecord->command);
    commandRecord++;
  }
}
