/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/cipher.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_CIPHER_H
#define THANDOR_NETWORK_PROTOCOL_CIPHER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/cipher. */

/* Packet cipher (UiTransfer_EncryptPacketBlocks / UiTransfer_DecryptPacketBlocks): 16 rounds with one 32-bit
   key each (g_UiTransferRoundKeys). Each round looks up every nibble in its own
   16x16 dword table (row = key nibble, column = data nibble); table n handles nibble n. */
#define UI_TRANSFER_CIPHER_ROUND_COUNT 16
#define UI_TRANSFER_CIPHER_ROW_BYTES 0x40    /* 16 dword entries */
#define UI_TRANSFER_CIPHER_TABLE_BYTES 0x400 /* 16 rows */

/* Functions are grouped by semantic ownership. */

void UiTransfer_EncryptPacketBlocks(const uint32_t *roundKeys16,uint32_t *outputBlocks,UiTransferPayloadByteCount byteCount,
          uint32_t *inputBlocks);

void UiTransfer_DecryptPacketBlocks
          (const uint32_t *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,void *source);

extern const uint32_t g_UiTransferRoundKeys[16];

#endif /* THANDOR_NETWORK_PROTOCOL_CIPHER_H */
