/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/mailbox.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/mailbox.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* mailbox chunk packet (0x10031 request / 0x80030 chunk, 0x100 bytes): header (with packetHeader.sequenceToken), payload = chunk offset (payload byte 0), transfer byte count (payload byte 4), chunk data (from payload byte 8, 58 dwords). */
static UiRuntimeRecord g_UiTransferChunkPacket = {0};

/* ping answer packet 0x10033 (same layout as the 0x10032 ping): header (with header.sequenceToken), echoed tick (backendSessionValue). */
static FrontendPacket10032HostValue g_UiTransferPingEchoPacket = {0};

UiTransferMailboxTickCounter g_UiTransferMailboxTickCounter = 0;

uint32_t g_UiRuntimeRecordWriteIndex = 0;

uint32_t g_UiTransferUnitCursor = 0;

/* uint32_t sequence token stamped into outgoing network packets (initial 0x12340000, low 16 bits XORed with a random value; network/protocol/mailbox.cpp, ui/frontend/network.cpp). */
uint32_t g_UiTransferSequenceToken = 0x12340000;

uint32_t g_UiTransferSenderContext = 0;

UiTransferMailboxState g_UiTransferMailbox = {};

/* Implementation ownership: network/protocol/mailbox. */

/* Header bytes of the mailbox chunk packet g_UiTransferChunkPacket; its payload holds the chunk
   offset (+0), the transfer byte count (+4) and the chunk data (+8). The packed type is written byte by byte:
   0x31,0,1,0 = FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST, 0x30,0,8,0 = FRONTEND_PACKET_80030_MAILBOX_CHUNK.
   Round keys 12..15 of g_UiTransferRoundKeys are not a string, although their bytes spell "mohTG sakere!!!e".
   In the original image the packet directly follows the key table. */
#define UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES ((uint8_t *)&g_UiTransferChunkPacket.packetHeader)
#define UI_TRANSFER_CHUNK_PACKET_OFFSET (*(UiTransferMailboxByteOffset *)(g_UiTransferChunkPacket.payload + 0))
#define UI_TRANSFER_CHUNK_PACKET_BYTE_COUNT (*(UiTransferMailboxByteCount *)(g_UiTransferChunkPacket.payload + 4))
#define UI_TRANSFER_CHUNK_PACKET_DATA ((uint32_t *)(g_UiTransferChunkPacket.payload + 8))

/* Descrambles the received datagram in place and checks its XOR checksum: the XOR of all dwords of the packet
   (unit count * 8 dwords, checksum field zeroed) must equal the transmitted checksum. See the checksum quirk
   at UiTransferMailbox_ServiceAndRetransmitTimer. */
static Bool8 UiTransferMailbox_DecryptAndVerifyRecord(UiRuntimeRecord *ringRecord)
{
  UiTransferXorChecksum *checksumField;
  UiTransferXorChecksum checksum;
  const uint32_t *packetDwordCursor;
  uint32_t unitCount;
  int dwordsRemaining;

  UiTransfer_DecryptPacketBlocks
            (g_UiTransferRoundKeys,ringRecord,256,ringRecord);
  LOCK();
  checksumField = &ringRecord->packetHeader.xorChecksum;
  checksum = *checksumField;
  *checksumField = 0;
  UNLOCK();
  unitCount = ringRecord->packetHeader.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
  /* Not in the original: a unit count of 0 or one that does not fit the 0x100-byte ring slot is rejected like
     a bad checksum (see the quirk at UiTransferMailbox_ServiceAndRetransmitTimer). A valid packet always fits:
     the datagram is received into a 0x100-byte buffer, a longer one fails to arrive. */
  if (unitCount == 0 || unitCount > FRONTEND_PACKET_MAX_UNIT_COUNT) {
    return false;
  }
  dwordsRemaining = (int)(unitCount << 3);
  packetDwordCursor = (const uint32_t *)ringRecord;
  do {
    checksum = checksum ^ *packetDwordCursor;
    packetDwordCursor++;
    dwordsRemaining--;
  } while (dwordsRemaining != 0);
  return checksum == 0;
}

/* Requests the chunk at the first missing offset of the incoming transfer (packet 0x10031) and restarts the
   retry countdown. */
static void UiTransferMailbox_RequestNextChunk(UiTransferEndpointDescriptor *hostEndpoint)
{
  g_UiTransferMailbox.receiveRetryTicks = UI_TRANSFER_CHUNK_RETRY_TICKS;
  UI_TRANSFER_CHUNK_PACKET_OFFSET =
       g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes;
  /* chunk packet header = FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST: type 0x31, 1 unit */
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[0] = UI_TRANSFER_CHUNK_REQUEST_TYPE_BYTE;
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[1] = 0;
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[2] = 1;
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[3] = 0;
  g_UiTransferChunkPacket.packetHeader.sequenceToken = g_UiTransferSequenceToken;
  UiTransfer_StagePacketAndSend
            (hostEndpoint,(UiTransferPacketHeader *)UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES);
}

/* Client: a chunk (0x80030) of the transfer from the host of this session; payload = offset, total size, data. */
static void UiTransferMailbox_ReceiveChunk
          (UiRuntimeRecord *ringRecord,UiTransferSenderEndpointSlot *senderEndpointSlot)
{
  int chunkOffset;
  uint32_t totalByteCount;
  uint32_t chunkEndOffset;
  uint32_t bytesRemaining;
  uint32_t chunkSize;
  uint32_t dwordsRemaining;
  const uint32_t *receivedChunkSourceDwords;
  uint32_t *receivedChunkDestinationDwords;
  void *receivedAllocation;

  if ((g_FrontendSessionToken != ringRecord->packetHeader.sequenceToken) ||
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder !=
       senderEndpointSlot->endpoint.ipv4AddressNetworkOrder)) {
    return;
  }
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks + UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS;
  chunkOffset = *(int *)ringRecord->payload;
  totalByteCount = *(uint32_t *)(ringRecord->payload + 4);
  /* receivedAllocation: NULL = no transfer requested, UI_TRANSFER_MAILBOX_UNAVAILABLE = requested,
     first chunk still missing (allocated here), otherwise the buffer being filled */
  if (g_UiTransferMailbox.receivedAllocation == nullptr) {
    return;
  }
  if (g_UiTransferMailbox.receivedAllocation == UI_TRANSFER_MAILBOX_UNAVAILABLE) {
    /* The original allocated the peer's total size unchecked; bounded here because every sender builds its
       transfer in the package scratch buffer (the scenario catalog is smaller), so a larger total is malformed.
       Logged once: it repeats for every such chunk while the transfer is still requested. */
    if (totalByteCount > PACKAGE_SCRATCH_BUFFER_BYTES) {
      static Bool8 s_loggedOversizedTransfer = false;
      if (!s_loggedOversizedTransfer) {
        s_loggedOversizedTransfer = true;
        Thandor_Log("UiTransferMailbox_ReceiveChunk: rejected transfer of %u bytes (maximum %u)",totalByteCount,
                    (uint32_t)PACKAGE_SCRATCH_BUFFER_BYTES);
      }
      return;
    }
    if (g_MemoryApi.alloc(totalByteCount,&receivedAllocation) != 0) {
      return;
    }
    g_UiTransferMailbox.receivedAllocation = receivedAllocation; /* alloc writes it only on success */
    chunkOffset = 0;
    g_UiTransferMailbox.receivedByteCount = totalByteCount;
    g_UiTransferMailbox.receivedRemainingBytes = totalByteCount;
  }
  /* only the chunk at the expected offset of a transfer of the expected size is taken */
  chunkEndOffset = chunkOffset + g_UiTransferMailbox.receivedRemainingBytes;
  if ((chunkEndOffset != g_UiTransferMailbox.receivedByteCount) || (chunkEndOffset != totalByteCount)) {
    return;
  }
  bytesRemaining = totalByteCount - chunkOffset;
  chunkSize = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
  if (bytesRemaining < UI_TRANSFER_CHUNK_PAYLOAD_BYTES) {
    chunkSize = bytesRemaining;
  }
  g_UiTransferMailbox.receivedRemainingBytes = g_UiTransferMailbox.receivedRemainingBytes - chunkSize;
  receivedChunkSourceDwords = (const uint32_t *)(ringRecord->payload + 8);
  receivedChunkDestinationDwords = (uint32_t *)((uint8_t *)g_UiTransferMailbox.receivedAllocation + chunkOffset);
  for (dwordsRemaining = chunkSize >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *receivedChunkDestinationDwords = *receivedChunkSourceDwords;
    receivedChunkSourceDwords++;
    receivedChunkDestinationDwords++;
  }
  if (g_UiTransferMailbox.receivedRemainingBytes != 0) {
    UiTransferMailbox_RequestNextChunk((UiTransferEndpointDescriptor *)senderEndpointSlot);
  }
}

/* Host: a player requests the chunk (0x80030) at the offset in the payload of the outgoing transfer. */
static void UiTransferMailbox_ServeChunkRequest
          (UiRuntimeRecord *ringRecord,UiTransferSenderEndpointSlot *senderEndpointSlot)
{
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t bytesRemaining;
  uint32_t chunkSize;
  uint32_t dwordsRemaining;
  const uint32_t *mailboxSourceDwords;
  uint32_t *chunkPayloadCursor;

  if (g_UiTransferMailbox.outgoingAllocation == nullptr) {
    return;
  }
  /* the first record is compared before the count is checked */
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((ringRecord->packetHeader.sequenceToken != playerRecord->peerSequenceToken) ||
         (senderEndpointSlot->endpoint.ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)) {
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return; /* not a player: drop the request */
    }
  }
  /* The original served any requested offset, copying up to a chunk from behind the outgoing buffer; bounded
     here because the offset comes from the peer: an offset past the outgoing size is dropped. Valid requesters
     only ask for offsets below the size (the chunk is then clipped to the rest). Logged once: a peer may repeat
     the request every frame. */
  if (*(UiTransferMailboxByteOffset *)ringRecord->payload > g_UiTransferMailbox.outgoingByteCount) {
    static Bool8 s_loggedOutOfRangeRequest = false;
    if (!s_loggedOutOfRangeRequest) {
      s_loggedOutOfRangeRequest = true;
      Thandor_Log("UiTransferMailbox_ServeChunkRequest: rejected chunk request at offset %u (transfer %u bytes)",
                  *(UiTransferMailboxByteOffset *)ringRecord->payload,
                  (uint32_t)g_UiTransferMailbox.outgoingByteCount);
    }
    return;
  }
  /* original quirk: extends the sender's receive scratch slot instead of the player record, see the timer's
     comment */
  senderEndpointSlot->chunkTimeoutScratchNeverRead =
       senderEndpointSlot->chunkTimeoutScratchNeverRead + UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS;
  UI_TRANSFER_CHUNK_PACKET_OFFSET = *(UiTransferMailboxByteOffset *)ringRecord->payload;
  playerRecord->transferProgressBytes = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
  UI_TRANSFER_CHUNK_PACKET_BYTE_COUNT = g_UiTransferMailbox.outgoingByteCount;
  playerRecord->transferProgressBytes = playerRecord->transferProgressBytes + UI_TRANSFER_CHUNK_PACKET_OFFSET;
  /* chunk packet header = FRONTEND_PACKET_80030_MAILBOX_CHUNK: type 0x30, 8 units (0x100 bytes) */
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[0] = UI_TRANSFER_CHUNK_TYPE_BYTE;
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[1] = 0;
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[2] = 8;
  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[3] = 0;
  bytesRemaining = UI_TRANSFER_CHUNK_PACKET_BYTE_COUNT - UI_TRANSFER_CHUNK_PACKET_OFFSET;
  chunkSize = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
  if (bytesRemaining < UI_TRANSFER_CHUNK_PAYLOAD_BYTES) {
    chunkSize = bytesRemaining;
  }
  mailboxSourceDwords =
       (const uint32_t *)((uint8_t *)g_UiTransferMailbox.outgoingAllocation + UI_TRANSFER_CHUNK_PACKET_OFFSET);
  chunkPayloadCursor = UI_TRANSFER_CHUNK_PACKET_DATA;
  for (dwordsRemaining = chunkSize >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *chunkPayloadCursor = *mailboxSourceDwords;
    mailboxSourceDwords++;
    chunkPayloadCursor++;
  }
  g_UiTransferChunkPacket.packetHeader.sequenceToken = g_UiTransferSequenceToken;
  UiTransfer_StagePacketAndSend
            ((UiTransferEndpointDescriptor *)senderEndpointSlot,
             (UiTransferPacketHeader *)UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES);
}

/* Ping answer (0x10033): stores the round trip in ticks and "<ticks * 4>ms" (half the round trip) as text in
   the record of the answering player. */
static void UiTransferMailbox_StorePingRoundTrip
          (UiRuntimeRecord *ringRecord,UiTransferSenderEndpointSlot *senderEndpointSlot)
{
  int playersRemaining;
  int roundTripTicks;
  uint32_t numberByteCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint8_t *latencyTextCursor;
  uint8_t *latencySuffixCursor;

  playerRecord = g_FrontendPlayerRuntimeBlocks;
  for (playersRemaining = g_FrontendPlayerRuntimeCount; playersRemaining > 0; playersRemaining--) {
    if ((ringRecord->packetHeader.sequenceToken == playerRecord->peerSequenceToken) &&
        (senderEndpointSlot->endpoint.ipv4AddressNetworkOrder == playerRecord->endpoint.ipv4AddressNetworkOrder)) {
      roundTripTicks = g_UiTransferMailboxTickCounter - *(int *)ringRecord->payload;
      playerRecord->pingRoundTripTicks = roundTripTicks;
      latencyTextCursor = (uint8_t *)playerRecord->pingTextUtf16;
      numberByteCount = g_WideNumberFormatUtf16
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,roundTripTicks * 4,(uint16_t *)latencyTextCursor);
      latencySuffixCursor = latencyTextCursor + numberByteCount;
      latencySuffixCursor[0] = 'm'; /* UTF-16 "ms" and terminator */
      latencySuffixCursor[1] = 0;
      latencySuffixCursor[2] = 's';
      latencySuffixCursor[3] = 0;
      latencySuffixCursor[4] = 0;
      latencySuffixCursor[5] = 0;
      return;
    }
    playerRecord++;
  }
}

/* Network receive timer (125 Hz, so one tick is 8 ms). Drains the UDP socket into the record ring: each
   datagram is descrambled and its XOR checksum verified. The transfer and ping packets are answered right
   here; every other valid packet is kept in the ring for the frontend/in-game handlers. Transfer: the host
   sends a data blob (g_UiTransferMailbox) in chunks of UI_TRANSFER_CHUNK_PAYLOAD_BYTES as 0x80030 packets,
   each requested by the receiver with a 0x10031 packet naming the next offset; a request that stays
   unanswered for UI_TRANSFER_CHUNK_RETRY_TICKS ticks is sent again. Ping: 0x10032 is echoed as 0x10033,
   whose round trip becomes the player's latency text. Skipped while the ring lock is held elsewhere.
   Original quirk: the checksum loop XORs (unit count * 8) dwords, taking the unit count from the (descrambled)
   packet header - neither the received byte count nor the 0x100-byte ring slot limit it,
   so a count above 8 XORs past the slot and a count of 0 wraps the counter and reads on until it faults. Here
   such packets are rejected (UiTransferMailbox_DecryptAndVerifyRecord); valid packets are not affected.
   Original quirk: on a host chunk request (0x10031) the timeout extension goes to
   senderEndpointSlot->chunkTimeoutScratchNeverRead (the sender's slot of the auxiliary endpoint buffer), not to the
   requesting player's heartbeatExpiryTicks (the same offset in the player record), which was probably meant; the
   scratch dword is never read, so the host's heartbeat countdown is not extended by chunk requests. The client
   branch (0x80030) extends g_SessionTransferTimeoutTicks as intended.
*/
void UiTransferMailbox_ServiceAndRetransmitTimer()

{
  uint32_t slotIndex;
  uint32_t nextSlotIndex;
  UiRuntimeRecord *ringRecord;
  UiTransferSenderEndpointSlot *senderEndpointSlot;
  Bool8 lockBusy;

  g_UiTransferMailboxTickCounter++;
  lockBusy = g_SpinLockTryAcquire(&g_UiRuntimeRecordRingLock);
  if (lockBusy) {
    return;
  }
  /* Receive loop: handles (or rejects) one record per pass until the backend has no more data.
     The datagram goes to ring slot g_UiRuntimeRecordWriteIndex (0x100 bytes), the sender's address to
     the matching 0x80-byte slot of the auxiliary buffer; the slot is only kept (write index advanced)
     for packets that are not handled here. */
  while (g_NetworkBackendSlot4
                  ((WinSockAddress *)
                   (g_UiRuntimeRecordWriteIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + (uintptr_t)g_UiRuntimeRecordEndpointSlots),256,
                   (uint8_t *)(g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex))) {
    slotIndex = g_UiRuntimeRecordWriteIndex;
    nextSlotIndex = slotIndex + 1;
    ringRecord = g_UiRuntimeRecordRing + slotIndex;
    if (!UiTransferMailbox_DecryptAndVerifyRecord(ringRecord)) {
      continue;
    }
    senderEndpointSlot =
         (UiTransferSenderEndpointSlot *)(slotIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + (uintptr_t)g_UiRuntimeRecordEndpointSlots);
    if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_80030_MAILBOX_CHUNK) {
      UiTransferMailbox_ReceiveChunk(ringRecord,senderEndpointSlot);
    }
    else if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST) {
      UiTransferMailbox_ServeChunkRequest(ringRecord,senderEndpointSlot);
    }
    else if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_10032_PING) {
      /* ping: echo the sender's tick count back as 0x10033 */
      g_UiTransferPingEchoPacket.backendSessionValue = *(uint32_t *)ringRecord->payload;
      g_UiTransferPingEchoPacket.header.packedTypeAndUnitCount = FRONTEND_PACKET_10033_PING_ECHO;
      g_UiTransferPingEchoPacket.header.sequenceToken = g_UiTransferSequenceToken;
      UiTransfer_StagePacketAndSend
                ((UiTransferEndpointDescriptor *)senderEndpointSlot,&g_UiTransferPingEchoPacket.header);
    }
    else if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_10033_PING_ECHO) {
      UiTransferMailbox_StorePingRoundTrip(ringRecord,senderEndpointSlot);
    }
    else {
      /* keep the packet: advance the ring write index (256 slots) */
      g_UiRuntimeRecordWriteIndex = nextSlotIndex;
      if (UI_RUNTIME_RECORD_RING_LAST_INDEX < nextSlotIndex) {
        g_UiRuntimeRecordWriteIndex = 0;
      }
    }
  }
  /* no more data: re-request the missing chunk when the retry countdown expires */
  if (g_UiTransferMailbox.receiveRetryTicks != 0) {
    g_UiTransferMailbox.receiveRetryTicks = g_UiTransferMailbox.receiveRetryTicks - 1;
    if ((g_UiTransferMailbox.receiveRetryTicks == 0) && (g_UiTransferMailbox.receivedRemainingBytes != 0)) {
      UiTransferMailbox_RequestNextChunk(&g_FrontendSelectedNetworkEndpoint);
    }
  }
  g_SpinLockRelease(&g_UiRuntimeRecordRingLock);
}

/* Empties the receive side of the transfer mailbox (allocation, byte count, remaining bytes, retry ticks) so a
   new transfer can be received; the outgoing buffer is left alone. Consumers call it after taking a buffer.
*/
void UiTransferMailbox_ClearReceivedState()

{
  g_UiTransferMailbox.receivedAllocation = nullptr;
  g_UiTransferMailbox.receivedByteCount = 0;
  g_UiTransferMailbox.receivedRemainingBytes = 0;
  g_UiTransferMailbox.receiveRetryTicks = 0;
}

/* Hands out a completely received transfer: returns its (non-NULL) buffer and stores its byte count in
   *outByteCount once an allocation exists and no bytes are outstanding. An empty, unavailable or still
   incomplete mailbox returns NULL and leaves *outByteCount untouched (the original returned nothing
   meaningful then; every caller reads the results only on success).
*/
void *UiTransferMailbox_GetReceivedBuffer(uint32_t *outByteCount)

{
  if (g_UiTransferMailbox.receivedAllocation != UI_TRANSFER_MAILBOX_UNAVAILABLE &&
      g_UiTransferMailbox.receivedAllocation != nullptr &&
      g_UiTransferMailbox.receivedRemainingBytes == 0) {
    *outByteCount = g_UiTransferMailbox.receivedByteCount;
    return g_UiTransferMailbox.receivedAllocation;
  }
  return nullptr;
}

/* Gives this machine a new random session identity before it opens or looks for a session: XORs a random
   16-bit value into the low word of the transfer sequence token. The high word stays (a host answers the
   discovery probe only for 0x1234).
*/
void UiTransferMailbox_RandomizeSequenceToken()

{
  uint32_t randomValue;
  
  randomValue = Random_NextPrimary();
  g_UiTransferSequenceToken = g_UiTransferSequenceToken ^ randomValue & 0xffff;
}

/* Marks the receive side as unavailable: publishes the UI_TRANSFER_MAILBOX_UNAVAILABLE sentinel and sets the
   byte count, remaining bytes and retry ticks to one, so the mailbox is neither empty nor receivable.
*/
void UiTransferMailbox_MarkUnavailable()

{
  g_UiTransferMailbox.receivedAllocation = UI_TRANSFER_MAILBOX_UNAVAILABLE;
  g_UiTransferMailbox.receivedByteCount = 1;
  g_UiTransferMailbox.receivedRemainingBytes = 1;
  g_UiTransferMailbox.receiveRetryTicks = 1;
}

/* Publishes the buffer the next outgoing transfer sends (NULL/0 withdraws it). The allocation is later
   released through g_MemoryApi.free by the frontend transfer consumers.
*/
void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation)

{
  g_UiTransferMailbox.outgoingAllocation = allocation;
  g_UiTransferMailbox.outgoingByteCount = byteCount;
}

/* Sends one packet to endpoint; every packet of the game goes through here. Stamps the header with this
   machine's sequence token and sender context and the XOR checksum over all dwords, then writes a scrambled
   copy (UiTransfer_EncryptPacketBlocks) into the next free units of a 256-unit ring (0x20 bytes per unit,
   with a parallel ring of 16-byte endpoint copies) and hands that copy to the backend send slot. The unit
   count is the high word of packedTypeAndUnitCount. Returns true when the backend send failed.
*/
Bool8 UiTransfer_StagePacketAndSend(UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet)

{
  uint32_t nextUnitCursor;
  Bool8 moreBytes;
  uint8_t *endpointBufferBase;
  uint32_t currentSequenceToken;
  uint32_t currentSenderContext;
  UiTransferXorChecksum checksum;
  uint32_t unitCount;
  UiTransferPayloadByteCount byteCount;
  UiTransferPayloadByteCount bytesRemaining;
  int endpointOffset;
  int dataOffset;
  int dwordCount;
  uint32_t *outputBlocks;
  const uint32_t *packetDwordCursor;
  const uint32_t *endpointSourceDwords;
  uint32_t *endpointDestinationDwordCursor;

  currentSenderContext = g_UiTransferSenderContext;
  currentSequenceToken = g_UiTransferSequenceToken;
  unitCount = packet->packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
  dataOffset = g_UiTransferUnitCursor << 5; /* 0x20 bytes per unit */
  nextUnitCursor = unitCount + g_UiTransferUnitCursor;
  endpointOffset = g_UiTransferUnitCursor << 4; /* 16 bytes per endpoint copy */
  g_UiTransferUnitCursor = nextUnitCursor;
  if (255 < nextUnitCursor) {
    /* the packet does not fit before the end of the ring: start over at unit 0 */
    dataOffset = 0;
    endpointOffset = 0;
    g_UiTransferUnitCursor = unitCount;
  }
  outputBlocks = (uint32_t *)(g_UiTransferDataBuffer + dataOffset);
  endpointBufferBase = g_UiTransferEndpointBuffer->zeroPadding;
  byteCount = unitCount << 5;
  packet->xorChecksum = 0;
  packet->sequenceToken = currentSequenceToken;
  packet->senderContext = currentSenderContext;
  /* XOR over all dwords of the packet (checksum field zeroed); stops once the byte count is used up or was
     not above 3 (a byte count of 0 still XORs the first dword) */
  checksum = 0;
  bytesRemaining = byteCount;
  packetDwordCursor = (const uint32_t *)packet;
  do {
    checksum = checksum ^ *packetDwordCursor;
    packetDwordCursor++;
    moreBytes = 3 < (int)bytesRemaining;
    bytesRemaining = bytesRemaining - 4;
  } while (bytesRemaining != 0 && moreBytes);
  packet->xorChecksum = checksum;
  UiTransfer_EncryptPacketBlocks
            (g_UiTransferRoundKeys,outputBlocks,byteCount,(uint32_t *)packet);
  /* endpointBufferBase points 8 bytes into the endpoint ring, so "- 8" is the endpoint slot itself */
  endpointDestinationDwordCursor = (uint32_t *)(endpointBufferBase + endpointOffset + -8);
  endpointSourceDwords = (const uint32_t *)endpoint;
  for (dwordCount = 4; dwordCount != 0; dwordCount--) {
    *endpointDestinationDwordCursor = *endpointSourceDwords;
    endpointSourceDwords++;
    endpointDestinationDwordCursor++;
  }
  return !g_NetworkBackendSlot5
                ((WinSockAddress *)(endpointBufferBase + endpointOffset + -8),byteCount,(uint8_t *)outputBlocks);
}
