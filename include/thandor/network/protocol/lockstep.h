/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/lockstep.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_LOCKSTEP_H
#define THANDOR_NETWORK_PROTOCOL_LOCKSTEP_H

#include <thandor/core/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/lockstep.

   Host side of the lockstep command exchange, shared by the in-game exchange (command_exchange) and the
   frontend/lobby exchange (frontend_session, lobby). Player record 0 is the host, records 1..n-1 are the
   clients; each player has one 0x20-byte command slot parallel to its record. Every tick the host waits until
   every client has submitted (commandSyncPending), clears the flags, takes its own next command into slot 0,
   packs the non-empty slots into the batch buffer and sends that batch to every client in record order.
   The sender context increment and the dequeue of the host's command stay at the call sites. */

/* One exchange: its command slots, batch buffer, packet types and command handler table. */
struct LockstepHostChannel {
    FrontendCommandPacketRecord *commandSlots;  /* one slot per player record, slot 0 = host */
    FrontendCommandPacketRecord *batch;         /* the first record's header doubles as the batch header */
    uint32_t batchType;                         /* packet type of the batch, OR'ed with the unit count */
    UiTransferPacketHeader *waitPacket;         /* sent to clients that already submitted while others are missing */
    uint32_t waitType;                          /* written into waitPacket before every send */
    uint32_t commandCodeBase;                   /* CommandDispatch_ExecuteRecord code base */
    uint32_t handlerRegionEnd;                  /* CommandDispatch_ExecuteRecord original handler region end */
};

/* What Lockstep_PackBatch does when no slot holds a command. */
enum class LockstepEmpty : uint8_t {
    SendHostRecord, /* the (empty) host slot 0 is sent as a batch of one */
    SendNothing     /* nothing is packed, the batch header is not written, 0 is returned */
};

/* True when every client (records 1..n-1) has submitted its command. */
Bool8 Lockstep_AllClientsSubmitted();

/* Clears the submission flag of every client (records 1..n-1). */
void Lockstep_ClearClientSubmissions();

/* While clients are missing: resends the previous batch to every client that has not submitted yet and the
   wait packet to those that have, in record order. */
void Lockstep_ResendBatchOrWait(const LockstepHostChannel &channel);

/* Packs every non-empty command slot (handler offset in bits 8..31 != 0) of the first slotCount slots into the
   batch buffer and writes the batch header; returns the number of packed records (0 only with
   LockstepEmpty::SendNothing). clearPackedSlots keeps only the player id of every packed slot. */
uint32_t Lockstep_PackBatch
          (const LockstepHostChannel &channel,uint32_t slotCount,LockstepEmpty empty,
          Bool8 clearPackedSlots = false);

/* Sends the batch to the clients, records 1..playerCount-1 in ascending order. */
void Lockstep_SendBatchToClients(const LockstepHostChannel &channel,uint32_t playerCount);

/* Executes the first recordCount records of the batch locally. */
void Lockstep_ExecuteRecords(const LockstepHostChannel &channel,uint32_t recordCount);

#endif /* THANDOR_NETWORK_PROTOCOL_LOCKSTEP_H */
