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

/* Types (split out by tools/dev/split_types.py). */

typedef struct FrontendPacket50001SessionAdvertisement FrontendPacket50001SessionAdvertisement, *PFrontendPacket50001SessionAdvertisement;
typedef struct UiTransferEndpointDescriptor UiTransferEndpointDescriptor, *PUiTransferEndpointDescriptor;
typedef struct UiTransferPacketHeader UiTransferPacketHeader, *PUiTransferPacketHeader;
typedef struct UiCommandQueueRecord UiCommandQueueRecord, *PUiCommandQueueRecord;
typedef struct UiTransferSenderEndpointSlot UiTransferSenderEndpointSlot, *PUiTransferSenderEndpointSlot;
typedef struct UiTransferPacket UiTransferPacket, *PUiTransferPacket;
typedef struct UiTransferMailboxState UiTransferMailboxState, *PUiTransferMailboxState;
typedef struct FrontendCommandPacketRecord FrontendCommandPacketRecord, *PFrontendCommandPacketRecord;
typedef struct FrontendPlayerRemovalPacket10007 FrontendPlayerRemovalPacket10007, *PFrontendPlayerRemovalPacket10007;
typedef union FrontendTransferPacketUnion FrontendTransferPacketUnion, *PFrontendTransferPacketUnion;
typedef struct FrontendPacket10000Handshake FrontendPacket10000Handshake, *PFrontendPacket10000Handshake;
typedef struct FrontendPacket20002PlayerDescriptor FrontendPacket20002PlayerDescriptor, *PFrontendPacket20002PlayerDescriptor;
typedef struct FrontendPacket10003JoinAck FrontendPacket10003JoinAck, *PFrontendPacket10003JoinAck;
typedef struct FrontendPacket10004PlayerSnapshotRequest FrontendPacket10004PlayerSnapshotRequest, *PFrontendPacket10004PlayerSnapshotRequest;
typedef struct FrontendPacket30005PlayerSnapshot FrontendPacket30005PlayerSnapshot, *PFrontendPacket30005PlayerSnapshot;
typedef struct FrontendPacket10006CapabilityHeartbeat FrontendPacket10006CapabilityHeartbeat, *PFrontendPacket10006CapabilityHeartbeat;
typedef struct FrontendPacket40008LobbyRosterSnapshot FrontendPacket40008LobbyRosterSnapshot, *PFrontendPacket40008LobbyRosterSnapshot;
typedef struct FrontendPacket10009SnapshotChunkRequest FrontendPacket10009SnapshotChunkRequest, *PFrontendPacket10009SnapshotChunkRequest;
typedef struct FrontendPacket8000ASnapshotChunk FrontendPacket8000ASnapshotChunk, *PFrontendPacket8000ASnapshotChunk;
typedef struct FrontendPacket10012SyncPending FrontendPacket10012SyncPending, *PFrontendPacket10012SyncPending;
typedef struct FrontendPacket10013HeartbeatAck FrontendPacket10013HeartbeatAck, *PFrontendPacket10013HeartbeatAck;
typedef struct FrontendPacket10032HostValue FrontendPacket10032HostValue, *PFrontendPacket10032HostValue;
typedef struct FrontendPacket10022StatePending FrontendPacket10022StatePending, *PFrontendPacket10022StatePending;
typedef struct FrontendPacket10023StateAck FrontendPacket10023StateAck, *PFrontendPacket10023StateAck;

enum {
    FRONTEND_SNAPSHOT_SOURCE_AVAILABLE=1,
    FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE=2,
    FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY=4
};
typedef int FrontendSnapshotTransferFlags;

enum {
    UI_TRANSFER_JOIN_UNAVAILABLE=0,
    UI_TRANSFER_JOIN_AVAILABLE=4294967295
};
typedef int UiTransferJoinAvailability;

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
typedef int UiTransferPacketPackedType;

typedef uint32_t FrontendRandomSeed;

typedef int FrontendFactionAssignmentIndex;

typedef uint32_t UiTransferXorChecksum;

typedef uint32_t FrontendReadyOrWaitState;

typedef uint32_t FrontendConsensusValue;

typedef uint32_t UiTransferRemainingByteCount;

typedef uint32_t UiTransferMailboxTickCounter;

typedef uint32_t UiTransferSequenceToken;

typedef int FrontendPlayerRuntimeId;

typedef uint32_t PackedUiCommandAndPlayerId;

typedef uint32_t UiTransferSenderContext;

typedef uint32_t FrontendBackendSessionValue;

typedef uint32_t FrontendNetworkTickInterval;

typedef uint32_t SessionTransferTimeoutTicks;

typedef uint32_t FrontendSnapshotChunkByteOffset;

typedef uint32_t UiTransferMailboxByteOffset;

typedef uint32_t FrontendPlayerIndex;

/* One payload dword of a queued player command (UiCommandQueueRecord). */
typedef uint32_t CommandPayload;

typedef int InGameCommandHandlerAddress32;

/* A queued player command handler: the command code is the handler's code offset from the queue
   function (see CommandDispatch_ResolveHandler); called with the player runtime id and the three payload dwords
   (UiCommandQueueRecord.payload1..payload3). */
typedef void CommandQueueHandlerProc(uint32_t playerRuntimeId,uint32_t payload1,uint32_t payload2,
                                     uint32_t payload3);

typedef uint32_t UiTransferPayloadByteCount;

typedef uint32_t UiTransferMailboxByteCount;

typedef uint32_t FrontendHeartbeatTickCount;

typedef int DwordBlockRecordCount;

typedef uint32_t FrontendPlayerCount;

typedef uint32_t FrontendStatusCode;

typedef uint32_t FrontendCapabilityFlags;

typedef uint32_t FrontendProtocolMagic;

typedef int FrontendRootRuntimeAddress32;

typedef uint32_t UiTransferRetryTickCount;

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
    uint32_t transferTimeoutTicks; 
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
