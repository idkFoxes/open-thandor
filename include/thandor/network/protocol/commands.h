#ifndef THANDOR_NETWORK_PROTOCOL_COMMANDS_H
#define THANDOR_NETWORK_PROTOCOL_COMMANDS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/commands. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00543F50 */
void FrontendCommandQueue_EnqueueLocalPlayerCommand (UiActionId commandCode,CommandPayloadDword0C payloadDword0C, CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04);

/* 0x00543FB0 */
void FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

/* 0x0055F130 */
void InGameCommandQueue_AppendLocalPlayerCommand (UiActionId commandCode,CommandPayloadDword0C payloadDword0C, CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04);

/* 0x0055F190 */
void InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

/* 0x0055F200 */
undefined8 InGameCommandQueue_ContainsTripletValueCf (InGameCommandPayloadTripletValue32 payloadValue, InGameCommandHandlerAddress32 commandHandlerAddress);

#endif /* THANDOR_NETWORK_PROTOCOL_COMMANDS_H */
