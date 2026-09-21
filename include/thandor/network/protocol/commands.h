/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_COMMANDS_H
#define THANDOR_NETWORK_PROTOCOL_COMMANDS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/commands. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00543F50 */
void __thandor_void_preserve_eax_ecx_edx
FrontendCommandQueue_EnqueueLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04);

/* 0x00543FB0 */
void __thandor_void_preserve_ecx_edx
FrontendCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

/* 0x0055F130 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandQueue_AppendLocalPlayerCommand
          (UiActionId commandCode,CommandPayloadDword0C payloadDword0C,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword04 payloadDword04);

/* 0x0055F190 */
void __thandor_void_preserve_ecx_edx
InGameCommandQueue_DequeueFirstIntoRecord(FrontendCommandPacketRecord *outputRecord);

/* 0x0055F200 */
bool __thandor_cf_preserve_eax_ecx_edx
InGameCommandQueue_ContainsTripletValueCf
          (InGameCommandPayloadTripletValue32 payloadValue,
          InGameCommandHandlerAddress32 commandHandlerAddress);

#endif /* THANDOR_NETWORK_PROTOCOL_COMMANDS_H */
