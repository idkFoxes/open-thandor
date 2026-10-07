/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_TYPES_H
#define THANDOR_NETWORK_PROTOCOL_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/network/backend/types.h>

struct FrontendPacket50001SessionAdvertisement;
struct UiTransferEndpointDescriptor;
struct UiTransferPacketHeader;
struct UiCommandQueueRecord;
struct UiTransferSenderEndpointSlot;
struct UiTransferPacket;
struct UiTransferMailboxState;
struct FrontendCommandPacketRecord;
struct FrontendPlayerRemovalPacket10007;
union FrontendTransferPacketUnion;
struct FrontendPacket10000Handshake;
struct FrontendPacket20002PlayerDescriptor;
struct FrontendPacket10003JoinAck;
struct FrontendPacket10004PlayerSnapshotRequest;
struct FrontendPacket30005PlayerSnapshot;
struct FrontendPacket10006CapabilityHeartbeat;
struct FrontendPacket40008LobbyRosterSnapshot;
struct FrontendPacket10009SnapshotChunkRequest;
struct FrontendPacket8000ASnapshotChunk;
struct FrontendPacket10012SyncPending;
struct FrontendPacket10013HeartbeatAck;
struct FrontendPacket10032HostValue;
struct FrontendPacket10022StatePending;
struct FrontendPacket10023StateAck;

enum {
    FRONTEND_SNAPSHOT_SOURCE_AVAILABLE=1,
    FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE=2,
    FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY=4
};
using FrontendSnapshotTransferFlags = int;

enum {
    UI_TRANSFER_JOIN_UNAVAILABLE=0,
    UI_TRANSFER_JOIN_AVAILABLE=0xFFFFFFFFu /* stored as -1 in the int joinAvailableFlag */
};
using UiTransferJoinAvailability = int;

enum {
    FRONTEND_PACKET_10000_HANDSHAKE=65536,
    FRONTEND_PACKET_10003_JOIN_ACK=65539,
    FRONTEND_PACKET_10004_SNAPSHOT_REQUEST=65540,
    FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT=65542,
    FRONTEND_PACKET_10007_PLAYER_REMOVAL=65543,
    FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST=65545,
    FRONTEND_PACKET_10011_LOBBY_COMMAND=65553,
    FRONTEND_PACKET_10012_WAIT=65554,
    FRONTEND_PACKET_10013_WAIT_ACK=65555,
    FRONTEND_PACKET_10021_COMMAND_SUBMIT=65569,
    FRONTEND_PACKET_10022_COMMAND_WAIT=65570,
    FRONTEND_PACKET_10023_COMMAND_WAIT_ACK=65571,
    FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST=65585,
    FRONTEND_PACKET_10032_PING=65586,
    FRONTEND_PACKET_10033_PING_ECHO=65587,
    FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR=131074,
    FRONTEND_PACKET_30005_PLAYER_SNAPSHOT=196613,
    FRONTEND_PACKET_40008_SESSION_PLAYER_ROW=262152,
    FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT=327681,
    FRONTEND_PACKET_8000A_SNAPSHOT_CHUNK=524298,
    FRONTEND_PACKET_80030_MAILBOX_CHUNK=524336
};
using UiTransferPacketPackedType = int;

using FrontendRandomSeed = uint32_t;

using FrontendFactionAssignmentIndex = int;

using UiTransferXorChecksum = uint32_t;

using FrontendReadyOrWaitState = uint32_t;

using FrontendConsensusValue = uint32_t;

using UiTransferRemainingByteCount = uint32_t;

using UiTransferMailboxTickCounter = uint32_t;

using UiTransferSequenceToken = uint32_t;

using FrontendPlayerRuntimeId = int;

using PackedUiCommandAndPlayerId = uint32_t;

using UiTransferSenderContext = uint32_t;

using FrontendBackendSessionValue = uint32_t;

using FrontendNetworkTickInterval = uint32_t;

using SessionTransferTimeoutTicks = uint32_t;

using FrontendSnapshotChunkByteOffset = uint32_t;

using UiTransferMailboxByteOffset = uint32_t;

using FrontendPlayerIndex = uint32_t;

/* One payload dword of a queued player command (UiCommandQueueRecord). */
using CommandPayload = uint32_t;

using InGameCommandHandlerAddress32 = int;

/* A queued player command handler: the command code is the handler's code offset from the queue
   function (see CommandDispatch_ResolveHandler); called with the player runtime id and the three payload dwords
   (UiCommandQueueRecord.payload1..payload3). */
using CommandQueueHandlerProc = void (uint32_t playerRuntimeId,uint32_t payload1,uint32_t payload2,
                                     uint32_t payload3);

using UiTransferPayloadByteCount = uint32_t;

using UiTransferMailboxByteCount = uint32_t;

using FrontendHeartbeatTickCount = uint32_t;

using DwordBlockRecordCount = int;

using FrontendPlayerCount = uint32_t;

using FrontendStatusCode = uint32_t;

using FrontendCapabilityFlags = uint32_t;

using FrontendProtocolMagic = uint32_t;


using UiTransferRetryTickCount = uint32_t;

struct UiTransferPacketHeader {
    UiTransferPacketPackedType packedTypeAndUnitCount; 
    UiTransferSequenceToken sequenceToken; 
    UiTransferSenderContext senderContext; 
    UiTransferXorChecksum xorChecksum; 
};

struct FrontendPacket50001SessionAdvertisement {
    struct UiTransferPacketHeader header; 
    UiTransferPayloadByteCount payloadByteCount; 
    UiTransferJoinAvailability joinAvailableFlag; 
    uint16_t sessionTitleUtf16[20]; 
    uint16_t hostDescriptionUtf16[44]; 
    uint16_t playerCountTextUtf16[4]; 
};

struct UiTransferEndpointDescriptor {
    union NetworkEndpointAddressHeader4 addressHeader; 
    NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder; 
    uint8_t zeroPadding[8]; 
};

/* A queued player command. The payload dwords are stored in reverse order: the handler is called as
   handler(playerId, payload1, payload2, payload3) (CommandQueueHandlerProc), payload1 is the last dword. */
struct UiCommandQueueRecord {
    PackedUiCommandAndPlayerId packedCommandAndPlayerId;
    CommandPayload payload3;
    CommandPayload payload2;
    CommandPayload payload1;
};

struct UiTransferSenderEndpointSlot {
    struct UiTransferEndpointDescriptor endpoint; 
    uint32_t chunkTimeoutScratchNeverRead; /* original quirk: chunk requests extend this instead of the player's
                                              heartbeat (UiTransferMailbox_ServiceAndRetransmitTimer) */
    uint8_t reserved0014_007F[108]; 
};

struct UiTransferPacket {
    struct UiTransferPacketHeader header; 
    struct UiCommandQueueRecord commands[3]; 
};

struct UiTransferMailboxState {
    Ptr32<void> outgoingAllocation; 
    UiTransferPayloadByteCount outgoingByteCount; 
    Ptr32<void> receivedAllocation; 
    UiTransferPayloadByteCount receivedByteCount; 
    UiTransferRemainingByteCount receivedRemainingBytes; 
    UiTransferRetryTickCount receiveRetryTicks; 
};

struct FrontendCommandPacketRecord {
    struct UiTransferPacketHeader header; 
    struct UiCommandQueueRecord command; 
};

struct FrontendPlayerRemovalPacket10007 {
    struct UiTransferPacketHeader header; 
    FrontendPlayerRuntimeId removedPlayerToken; 
    uint8_t reservedPayload14_1F[12]; 
};

struct FrontendPacket20002PlayerDescriptor {
    struct UiTransferPacketHeader header; 
    UiTransferPayloadByteCount payloadByteCount; 
    uint32_t reserved14; 
    uint32_t playerDescriptorPayload[10]; 
};

struct FrontendPacket8000ASnapshotChunk {
    struct UiTransferPacketHeader header; 
    uint32_t reserved10; 
    FrontendSnapshotChunkByteOffset snapshotChunkOffset; 
    uint8_t packet10009Buffer[232]; 
};

struct FrontendPacket10003JoinAck {
    struct UiTransferPacketHeader header; 
    FrontendPlayerRuntimeId assignedPlayerRuntimeId; 
    FrontendNetworkTickInterval networkTickInterval; 
    uint8_t reserved18_1F[8]; 
};

struct FrontendPacket40008LobbyRosterSnapshot {
    struct UiTransferPacketHeader header; 
    FrontendPlayerCount pendingSessionPlayerCount; 
    FrontendStatusCode selectedStatusCode0; 
    FrontendStatusCode selectedStatusCode1; 
    uint32_t reserved1C; 
    FrontendPlayerIndex selectedPlayerIndex; 
    FrontendPlayerCount playerCount; 
    FrontendPlayerRuntimeId selectedPlayerRuntimeId; 
    uint8_t reserved2C_37[12]; 
    uint32_t playerDescriptorPayload[10]; 
    uint16_t selectedPlayerStatusTextUtf16[16]; 
};

struct FrontendPacket10032HostValue {
    struct UiTransferPacketHeader header; 
    FrontendBackendSessionValue backendSessionValue; 
    uint8_t reserved14_1F[12]; 
};

struct FrontendPacket10000Handshake {
    struct UiTransferPacketHeader header; 
    FrontendProtocolMagic protocolMagic; 
    uint8_t reserved14_1F[12]; 
};

struct FrontendPacket30005PlayerSnapshot {
    struct UiTransferPacketHeader header; 
    FrontendPlayerIndex playerIndex; 
    FrontendPlayerRuntimeId playerRuntimeId; 
    uint32_t playerDescriptorPayload[10]; 
    struct UiTransferEndpointDescriptor endpoint; 
    FrontendRandomSeed secondaryRandomSeed; 
    FrontendReadyOrWaitState readyOrWaitState; 
    FrontendFactionAssignmentIndex factionAssignmentIndex; 
    FrontendConsensusValue consensusValue; 
};

struct FrontendPacket10012SyncPending {
    struct UiTransferPacketHeader header; 
    uint8_t reserved10_1F[16]; 
};

struct FrontendPacket10013HeartbeatAck {
    struct UiTransferPacketHeader header; 
    uint8_t reserved10_1F[16]; 
};

struct FrontendPacket10004PlayerSnapshotRequest {
    struct UiTransferPacketHeader header; 
    FrontendPlayerIndex requestedPlayerIndex; 
    uint8_t reserved14_1F[12]; 
};

struct FrontendPacket10023StateAck {
    struct UiTransferPacketHeader header; 
    uint8_t reserved10_1F[16]; 
};

struct FrontendPacket10022StatePending {
    struct UiTransferPacketHeader header; 
    uint8_t reserved10_1F[16]; 
};

struct FrontendPacket10009SnapshotChunkRequest {
    struct UiTransferPacketHeader header; 
    uint32_t reserved10; 
    FrontendSnapshotChunkByteOffset snapshotChunkOffset; 
    uint8_t reserved18_1F[8]; 
};

struct FrontendPacket10006CapabilityHeartbeat {
    struct UiTransferPacketHeader header; 
    FrontendCapabilityFlags capabilityFlags; 
    FrontendHeartbeatTickCount heartbeatExpiryTicks; 
    uint8_t reserved18_1F[8]; 
};

union FrontendTransferPacketUnion {
    struct FrontendPacket10000Handshake packet10000Handshake; 
    struct FrontendPacket50001SessionAdvertisement packet50001SessionAdvertisement; 
    struct FrontendPacket20002PlayerDescriptor packet20002PlayerDescriptor; 
    struct FrontendPacket10003JoinAck packet10003JoinAck; 
    struct FrontendPacket10004PlayerSnapshotRequest packet10004PlayerSnapshotRequest; 
    struct FrontendPacket30005PlayerSnapshot packet30005PlayerSnapshot; 
    struct FrontendPacket10006CapabilityHeartbeat packet10006CapabilityHeartbeat; 
    struct FrontendPacket40008LobbyRosterSnapshot packet40008LobbyRosterSnapshot; 
    struct FrontendPacket10009SnapshotChunkRequest packet10009SnapshotChunkRequest; 
    struct FrontendPacket8000ASnapshotChunk packet8000ASnapshotChunk; 
    struct FrontendPacket10012SyncPending packet10012SyncPending; 
    struct FrontendPacket10013HeartbeatAck packet10013HeartbeatAck; 
    struct FrontendPacket10032HostValue packet10032HostValue; 
    struct FrontendPacket10022StatePending packet10022StatePending; 
    struct FrontendPacket10023StateAck packet10023StateAck; 
    struct FrontendPlayerRemovalPacket10007 playerRemoval10007; 
    struct FrontendCommandPacketRecord command10011Or10021; 
    struct UiTransferPacket genericTransferPacket; 
};

#endif /* THANDOR_NETWORK_PROTOCOL_TYPES_H */
