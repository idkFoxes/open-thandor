/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/lobby.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_LOBBY_H
#define THANDOR_NETWORK_PROTOCOL_LOBBY_H

#include <thandor/core/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

/* Frontend (lobby / session start) command batch from the host: unit count = command records, handled by
   FrontendTransfer_HandleHostSessionAndCommandBatchPackets and FrontendTransfer_HandleGameplayCommandAndRosterPackets. */
inline constexpr auto FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE = 0x10;
/* High word of every Thandor sequence token (the low word is random, see
   UiTransferMailbox_RandomizeSequenceToken); a host answers only discovery probes that carry it. */
inline constexpr auto FRONTEND_SEQUENCE_TOKEN_HIGH_MASK = 0xffff0000;
inline constexpr auto FRONTEND_SEQUENCE_TOKEN_HIGH_WORD = 0x12340000;
/* FrontendCapabilityFlags::FRONTEND_CAPABILITY_CD (network/protocol/types.h) is the player capability bit of the
   0x10006 heartbeat and the 0x20002 descriptor (FrontendTransfer_SendCapabilityHeartbeat always sets it); the host
   then shows L"CD" in that player's list row. Bit 0 of the same word: a 64x64 picture <player name>.pcx was
   found (UiTransfer_SendPlayerDescriptor). */
inline constexpr auto FRONTEND_DESCRIPTOR_HAS_PICTURE = 0x1;

/* Lobby timeout: reload value of g_SessionTransferTimeoutTicks on a client (join ack, host session packet) and
   the heartbeat value of the 0x10006 packet. */
inline constexpr auto FRONTEND_LOBBY_TIMEOUT_TICKS = 0x40;
/* Rows of the network game page's session list (sessionList); further advertised sessions are ignored. */
inline constexpr auto FRONTEND_SESSION_LIST_CAPACITY = 0x20;
/* Step from one player record's endpoint to the next one's in UiTransferEndpointDescriptor units (0x13B). */
#define FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE (sizeof(FrontendPlayerRuntimeRecord) / sizeof(UiTransferEndpointDescriptor))
/* Position of a (16-byte aligned) FrontendPlayerRuntimeRecord field relative to the record's endpoint, in
   UiTransferEndpointDescriptor units: the original reads record fields through an endpoint cursor. */
#define FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(field) \
  (((int)offsetof(FrontendPlayerRuntimeRecord,field) - (int)offsetof(FrontendPlayerRuntimeRecord,endpoint)) / \
   (int)sizeof(UiTransferEndpointDescriptor))

/* Text resources of the 0x50001 session advertisement (FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets) and
   of the chat-history notice for an arriving player snapshot (0x30005). */
inline constexpr auto TEXT_ID_SESSION_TITLE_TEMPLATE = 0x211A; /* selector 0 = game version */
inline constexpr auto TEXT_ID_SESSION_HOST_TEMPLATE = 0x211B; /* selector 0 = game name, selector 1 = host player name */
inline constexpr auto TEXT_ID_SESSION_PLAYER_COUNT_TEMPLATE = 0x211C; /* selector 0 = players, selector 1 = player limit */
inline constexpr auto TEXT_ID_NETWORK_PLAYER_ARRIVED = 0xFF03; /* selector 0 = player name */

void FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          struct FrontendUiImage *frontendRuntime);

void FrontendTransfer_MarkUnavailableIfModeBit0Callback(uint32_t senderPlayerId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3);

void FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady
          (int playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3);

bool UiTransfer_SendDiscoveryProbe();

bool UiTransfer_SendPlayerDescriptor();

void FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          struct FrontendUiImage *frontendRuntime);

void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(struct FrontendUiImage *frontendRuntime);

void FrontendTransfer_SendCapabilityHeartbeat();

void FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          struct FrontendUiImage *frontendRuntime);

void FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRoot);

bool FrontendTransfer_ConsumeProcessedFlagForMenuTick();

void FrontendTransfer_SendLobbyCommandAndSnapshotRequest();

void FrontendTransfer_ExecuteLobbyCommandRecords
          (const FrontendCommandPacketRecord *commandRecord,uint32_t commandCount);

/* Not in the original: makes a plain UTF-16 text received from a peer (a typed name, a ping text) safe to draw;
   every rich-text command code becomes '?' and a missing terminator is added in the last unit. Returns whether
   anything was changed. */
bool FrontendTransfer_SanitizePeerTextUtf16(uint16_t *text,int unitCount);

extern UiTransferEndpointDescriptor g_FrontendSelectedNetworkEndpoint;
extern uint32_t g_FrontendSessionToken;
extern SessionTransferTimeoutTicks g_SessionTransferTimeoutTicks;

extern uint32_t g_FrontendPendingSessionPlayerCount; /* followed by an all-zero dword no code reaches (dropped) */
extern uint32_t g_FrontendExpectedPlayerRuntimeBlockCount;

extern int32_t g_FrontendPlayerRuntimeCount;
extern FrontendCommandPacketRecord g_FrontendPacket10011Buffer;

#endif /* THANDOR_NETWORK_PROTOCOL_LOBBY_H */
