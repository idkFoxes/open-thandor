/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/transfer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/transfer.h>
#include <thandor/thandor.h>

/* Module data. */

__declspec(align(16)) int32_t g_FrontendPlayerRuntimeCount = 0;

__declspec(align(16)) FrontendPacket10022StatePending g_FrontendPacket10022Buffer = {0};

/* Encryption S-boxes of the UI transfer 64-bit block cipher (UiTransfer_EncryptPacketBlocks): uint32_t[8][16][16], table n (0x400 bytes each) indexed [round-key nibble n][data nibble n], each entry a 4-bit output (each row a permutation of 0..15). */
static const uint32_t g_UiTransferEncryptSboxes[8][16][16] = {
    /* table 0 */
    {
        {13, 10, 0, 1, 15, 11, 5, 12, 4, 6, 14, 2, 8, 3, 9, 7},
        {0, 1, 7, 15, 3, 10, 2, 8, 9, 6, 14, 11, 12, 13, 4, 5},
        {12, 0, 6, 4, 14, 9, 3, 15, 2, 13, 5, 7, 11, 1, 10, 8},
        {12, 15, 4, 1, 13, 0, 3, 5, 10, 7, 6, 2, 14, 8, 11, 9},
        {4, 5, 14, 0, 6, 3, 10, 15, 13, 2, 9, 8, 7, 11, 1, 12},
        {15, 4, 2, 3, 12, 8, 13, 6, 7, 10, 0, 11, 9, 1, 14, 5},
        {13, 3, 14, 5, 10, 11, 4, 9, 1, 0, 8, 2, 12, 6, 7, 15},
        {13, 0, 8, 3, 11, 7, 9, 14, 15, 10, 6, 1, 5, 4, 12, 2},
        {0, 8, 4, 6, 9, 5, 11, 13, 14, 15, 1, 10, 2, 3, 7, 12},
        {13, 1, 0, 6, 14, 15, 4, 3, 2, 9, 10, 11, 8, 5, 12, 7},
        {14, 8, 0, 7, 10, 6, 3, 9, 11, 15, 13, 1, 12, 5, 2, 4},
        {5, 10, 13, 0, 1, 6, 15, 7, 4, 8, 3, 14, 11, 2, 9, 12},
        {0, 15, 5, 2, 8, 3, 13, 1, 6, 4, 9, 7, 10, 11, 14, 12},
        {9, 6, 7, 8, 15, 0, 2, 11, 1, 3, 4, 14, 12, 10, 5, 13},
        {0, 15, 2, 6, 12, 10, 5, 1, 14, 8, 3, 7, 11, 13, 4, 9},
        {15, 1, 7, 3, 0, 6, 11, 8, 5, 12, 14, 13, 4, 9, 2, 10}},
    /* table 1 */
    {
        {0, 4, 1, 15, 6, 11, 14, 2, 9, 7, 3, 5, 8, 10, 12, 13},
        {0, 15, 2, 1, 6, 9, 4, 3, 14, 11, 7, 8, 10, 5, 12, 13},
        {9, 14, 0, 15, 8, 11, 13, 5, 6, 7, 1, 4, 12, 3, 2, 10},
        {0, 6, 9, 2, 11, 14, 8, 15, 7, 13, 12, 1, 3, 4, 10, 5},
        {6, 3, 8, 15, 1, 11, 14, 12, 0, 7, 4, 9, 2, 10, 5, 13},
        {15, 3, 0, 6, 7, 14, 10, 4, 2, 8, 1, 9, 13, 11, 5, 12},
        {14, 4, 12, 15, 1, 8, 13, 5, 0, 11, 3, 6, 2, 9, 10, 7},
        {0, 6, 11, 2, 3, 7, 4, 9, 5, 12, 1, 14, 8, 15, 10, 13},
        {0, 15, 2, 6, 10, 5, 1, 14, 8, 3, 4, 9, 12, 11, 13, 7},
        {6, 0, 14, 1, 12, 11, 9, 15, 7, 8, 4, 13, 10, 3, 5, 2},
        {9, 7, 3, 6, 2, 1, 14, 0, 4, 8, 15, 13, 11, 12, 10, 5},
        {13, 15, 6, 12, 14, 8, 3, 4, 1, 0, 2, 9, 10, 5, 11, 7},
        {6, 13, 4, 8, 15, 0, 5, 3, 12, 11, 1, 14, 7, 2, 9, 10},
        {4, 6, 9, 14, 0, 11, 8, 7, 3, 1, 2, 15, 10, 12, 5, 13},
        {7, 6, 9, 14, 0, 15, 1, 13, 4, 5, 3, 11, 12, 8, 2, 10},
        {14, 13, 7, 12, 4, 15, 5, 10, 9, 1, 11, 8, 0, 6, 2, 3}},
    /* table 2 */
    {
        {0, 14, 8, 7, 15, 9, 12, 3, 5, 1, 6, 10, 4, 11, 13, 2},
        {0, 1, 14, 8, 5, 12, 6, 4, 2, 10, 3, 15, 7, 13, 11, 9},
        {0, 12, 3, 5, 15, 1, 9, 11, 8, 14, 13, 7, 4, 2, 6, 10},
        {4, 5, 11, 6, 12, 14, 13, 7, 9, 10, 0, 1, 8, 3, 15, 2},
        {0, 6, 1, 12, 8, 4, 5, 14, 3, 11, 7, 2, 13, 15, 9, 10},
        {0, 4, 1, 13, 8, 12, 6, 2, 5, 14, 3, 11, 7, 15, 9, 10},
        {6, 0, 8, 2, 15, 3, 9, 4, 14, 7, 12, 10, 13, 5, 11, 1},
        {5, 0, 9, 11, 14, 13, 12, 15, 7, 6, 10, 3, 8, 1, 4, 2},
        {15, 0, 5, 8, 6, 2, 4, 12, 3, 14, 13, 11, 1, 9, 7, 10},
        {3, 13, 8, 14, 0, 4, 6, 15, 1, 5, 12, 11, 2, 9, 7, 10},
        {3, 13, 8, 14, 0, 2, 6, 15, 1, 5, 12, 11, 10, 9, 7, 4},
        {0, 5, 7, 6, 15, 8, 14, 13, 3, 4, 9, 1, 12, 10, 2, 11},
        {0, 8, 15, 13, 4, 14, 9, 12, 5, 1, 2, 6, 3, 10, 7, 11},
        {8, 0, 3, 15, 6, 11, 4, 5, 1, 9, 10, 14, 13, 12, 7, 2},
        {1, 11, 3, 9, 8, 0, 10, 14, 7, 15, 12, 13, 5, 6, 2, 4},
        {1, 0, 4, 3, 10, 8, 15, 7, 2, 6, 12, 13, 9, 14, 5, 11}},
    /* table 3 */
    {
        {8, 10, 14, 13, 0, 11, 15, 3, 1, 4, 12, 9, 5, 6, 7, 2},
        {12, 0, 9, 5, 6, 7, 4, 8, 10, 2, 1, 14, 15, 11, 3, 13},
        {0, 1, 2, 10, 14, 15, 8, 6, 13, 7, 3, 9, 4, 5, 11, 12},
        {9, 0, 6, 10, 3, 8, 2, 1, 11, 13, 12, 5, 15, 7, 4, 14},
        {11, 10, 1, 0, 15, 3, 13, 8, 6, 2, 5, 4, 12, 14, 9, 7},
        {3, 10, 8, 9, 12, 13, 14, 15, 11, 2, 4, 0, 1, 7, 6, 5},
        {9, 5, 11, 8, 15, 0, 1, 13, 12, 3, 4, 7, 10, 2, 6, 14},
        {11, 10, 1, 0, 15, 3, 13, 8, 6, 2, 5, 4, 12, 14, 9, 7},
        {3, 10, 8, 9, 12, 13, 14, 15, 11, 2, 4, 0, 6, 1, 5, 7},
        {0, 8, 1, 7, 12, 10, 13, 11, 5, 3, 2, 15, 4, 9, 6, 14},
        {8, 14, 0, 3, 6, 10, 7, 15, 1, 2, 4, 11, 13, 12, 5, 9},
        {0, 3, 2, 13, 8, 12, 5, 11, 4, 9, 6, 7, 10, 15, 14, 1},
        {0, 3, 8, 6, 15, 2, 5, 4, 1, 10, 13, 9, 14, 12, 7, 11},
        {1, 0, 5, 2, 15, 10, 8, 13, 3, 6, 9, 12, 4, 11, 14, 7},
        {8, 0, 4, 15, 3, 13, 5, 2, 9, 1, 10, 6, 12, 7, 14, 11},
        {0, 5, 2, 15, 10, 9, 14, 4, 1, 12, 13, 6, 7, 3, 8, 11}},
    /* table 4 */
    {
        {0, 5, 2, 15, 10, 9, 14, 4, 1, 12, 13, 3, 11, 8, 7, 6},
        {0, 5, 2, 6, 12, 15, 7, 8, 9, 14, 4, 1, 11, 10, 3, 13},
        {0, 5, 14, 4, 15, 6, 2, 12, 7, 9, 3, 1, 8, 11, 10, 13},
        {0, 3, 4, 1, 5, 13, 7, 14, 8, 9, 11, 15, 2, 10, 12, 6},
        {5, 3, 1, 0, 2, 8, 9, 14, 7, 6, 12, 4, 15, 10, 13, 11},
        {15, 0, 4, 9, 14, 3, 13, 10, 8, 5, 12, 7, 2, 1, 6, 11},
        {5, 9, 2, 15, 1, 4, 13, 12, 0, 6, 8, 10, 14, 7, 3, 11},
        {9, 0, 4, 5, 3, 15, 14, 6, 1, 10, 11, 13, 2, 12, 8, 7},
        {9, 4, 1, 12, 10, 15, 14, 0, 6, 3, 11, 13, 8, 5, 2, 7},
        {9, 4, 8, 0, 7, 6, 5, 14, 1, 12, 3, 13, 10, 2, 15, 11},
        {5, 9, 0, 4, 15, 3, 14, 13, 1, 12, 8, 2, 11, 10, 7, 6},
        {5, 3, 1, 0, 13, 15, 14, 12, 9, 6, 2, 4, 10, 8, 7, 11},
        {1, 0, 15, 5, 8, 12, 2, 3, 7, 4, 6, 13, 10, 11, 9, 14},
        {0, 2, 9, 12, 3, 5, 1, 10, 7, 13, 15, 6, 14, 8, 11, 4},
        {0, 2, 9, 13, 6, 12, 14, 7, 15, 10, 5, 4, 3, 11, 8, 1},
        {3, 12, 0, 15, 1, 9, 13, 11, 2, 4, 5, 8, 6, 7, 14, 10}},
    /* table 5 */
    {
        {8, 2, 0, 5, 9, 11, 3, 6, 14, 4, 12, 1, 13, 10, 15, 7},
        {0, 4, 6, 3, 2, 8, 15, 5, 7, 13, 14, 1, 10, 12, 11, 9},
        {3, 2, 5, 12, 10, 11, 7, 4, 15, 8, 6, 0, 13, 1, 9, 14},
        {7, 6, 4, 5, 11, 3, 8, 15, 0, 12, 9, 14, 1, 10, 2, 13},
        {14, 15, 11, 2, 4, 0, 8, 3, 9, 12, 13, 7, 6, 10, 5, 1},
        {15, 4, 10, 11, 1, 5, 13, 3, 7, 8, 0, 9, 6, 12, 14, 2},
        {4, 0, 8, 3, 9, 12, 13, 14, 2, 7, 6, 11, 5, 15, 1, 10},
        {10, 4, 1, 14, 5, 2, 11, 3, 6, 8, 15, 9, 12, 0, 7, 13},
        {7, 0, 8, 14, 15, 3, 1, 6, 5, 9, 11, 4, 10, 2, 12, 13},
        {14, 15, 9, 2, 8, 0, 3, 12, 13, 1, 7, 6, 5, 4, 11, 10},
        {6, 1, 0, 8, 12, 2, 14, 15, 9, 3, 7, 5, 13, 4, 10, 11},
        {1, 0, 15, 5, 8, 4, 2, 12, 3, 13, 10, 11, 9, 6, 7, 14},
        {1, 0, 7, 5, 10, 2, 14, 8, 12, 4, 6, 3, 9, 13, 15, 11},
        {1, 0, 15, 5, 8, 6, 2, 12, 4, 10, 7, 3, 13, 11, 9, 14},
        {6, 15, 2, 9, 1, 0, 11, 8, 3, 12, 10, 7, 13, 5, 4, 14},
        {12, 11, 4, 14, 10, 0, 5, 1, 8, 7, 9, 3, 6, 15, 13, 2}},
    /* table 6 */
    {
        {0, 1, 8, 15, 5, 7, 12, 3, 4, 11, 9, 2, 6, 14, 10, 13},
        {15, 11, 12, 6, 1, 0, 8, 14, 2, 5, 9, 3, 4, 7, 13, 10},
        {1, 0, 9, 8, 10, 7, 4, 2, 12, 15, 13, 6, 5, 3, 14, 11},
        {9, 13, 0, 7, 5, 4, 1, 15, 11, 14, 12, 10, 2, 6, 8, 3},
        {12, 10, 7, 15, 9, 5, 14, 4, 0, 8, 11, 13, 2, 3, 6, 1},
        {10, 5, 7, 12, 1, 2, 6, 3, 4, 0, 15, 8, 14, 9, 13, 11},
        {6, 7, 5, 0, 15, 11, 9, 8, 4, 1, 10, 2, 12, 14, 3, 13},
        {14, 15, 9, 11, 2, 8, 0, 12, 4, 3, 5, 7, 1, 13, 6, 10},
        {0, 8, 11, 5, 13, 4, 6, 7, 15, 3, 1, 14, 10, 2, 9, 12},
        {13, 14, 15, 11, 2, 8, 0, 5, 10, 6, 12, 4, 1, 3, 9, 7},
        {2, 0, 9, 7, 15, 5, 1, 8, 4, 3, 6, 11, 10, 13, 14, 12},
        {13, 14, 15, 11, 2, 8, 0, 7, 5, 4, 10, 6, 3, 9, 12, 1},
        {0, 11, 12, 5, 7, 8, 2, 9, 13, 14, 15, 3, 1, 10, 4, 6},
        {14, 3, 15, 5, 0, 1, 8, 7, 2, 4, 12, 9, 11, 10, 13, 6},
        {3, 8, 6, 13, 15, 2, 0, 9, 12, 10, 11, 5, 4, 7, 1, 14},
        {15, 5, 12, 0, 4, 6, 7, 2, 8, 1, 9, 13, 11, 3, 10, 14}},
    /* table 7 */
    {
        {4, 9, 1, 6, 12, 3, 14, 11, 13, 8, 15, 0, 5, 2, 10, 7},
        {6, 5, 9, 15, 0, 1, 8, 11, 12, 3, 13, 4, 14, 2, 7, 10},
        {0, 12, 3, 7, 6, 8, 14, 10, 1, 11, 13, 15, 5, 4, 9, 2},
        {4, 0, 8, 9, 12, 13, 15, 11, 2, 3, 1, 7, 6, 5, 14, 10},
        {5, 1, 10, 6, 12, 2, 4, 11, 15, 0, 13, 14, 8, 3, 9, 7},
        {6, 5, 9, 15, 12, 8, 1, 0, 14, 3, 11, 4, 13, 2, 10, 7},
        {6, 5, 9, 15, 8, 1, 12, 0, 14, 3, 11, 4, 13, 10, 2, 7},
        {8, 10, 0, 11, 6, 3, 13, 12, 15, 5, 7, 9, 14, 2, 4, 1},
        {7, 6, 5, 9, 15, 12, 8, 1, 11, 0, 14, 2, 3, 4, 10, 13},
        {15, 0, 1, 5, 8, 2, 10, 9, 12, 13, 14, 11, 6, 3, 4, 7},
        {8, 1, 0, 11, 5, 9, 12, 13, 14, 15, 2, 3, 6, 4, 7, 10},
        {0, 7, 5, 4, 15, 11, 12, 8, 3, 9, 2, 1, 13, 10, 6, 14},
        {7, 5, 12, 15, 0, 14, 4, 8, 2, 13, 10, 1, 3, 6, 11, 9},
        {12, 13, 10, 11, 2, 4, 0, 3, 5, 9, 15, 8, 7, 1, 14, 6},
        {3, 4, 15, 0, 6, 7, 5, 8, 13, 14, 2, 1, 9, 12, 11, 10},
        {1, 0, 11, 13, 4, 8, 15, 5, 3, 10, 6, 2, 14, 9, 12, 7}},
};

/* Decryption S-boxes of the UI transfer block cipher: uint32_t[8][16][16], table n indexed [round-key nibble n][data nibble], 4-bit outputs (a separate table set from g_UiTransferEncryptSboxes); used by UiTransfer_DecryptPacketBlocks. */
static const uint32_t g_UiTransferDecryptSboxes[8][16][16] = {
    {
        {2, 3, 11, 13, 8, 6, 9, 15, 12, 14, 1, 5, 7, 0, 10, 4},
        {0, 1, 6, 4, 14, 15, 9, 2, 7, 8, 5, 11, 12, 13, 10, 3},
        {1, 13, 8, 6, 3, 10, 2, 11, 15, 5, 14, 12, 0, 9, 4, 7},
        {5, 3, 11, 6, 2, 7, 10, 9, 13, 15, 8, 14, 0, 4, 12, 1},
        {3, 14, 9, 5, 0, 1, 4, 12, 11, 10, 6, 13, 15, 8, 2, 7},
        {10, 13, 2, 3, 1, 15, 7, 8, 5, 12, 9, 11, 4, 6, 14, 0},
        {9, 8, 11, 1, 6, 3, 13, 14, 10, 7, 4, 5, 12, 0, 2, 15},
        {1, 11, 15, 3, 13, 12, 10, 5, 2, 6, 9, 4, 14, 0, 7, 8},
        {0, 10, 12, 13, 2, 5, 3, 14, 1, 4, 11, 6, 15, 7, 8, 9},
        {2, 1, 8, 7, 6, 13, 3, 15, 12, 9, 10, 11, 14, 0, 4, 5},
        {2, 11, 14, 6, 15, 13, 5, 3, 1, 7, 4, 8, 12, 10, 0, 9},
        {3, 4, 13, 10, 8, 0, 5, 7, 9, 14, 1, 12, 15, 2, 11, 6},
        {0, 7, 3, 5, 9, 2, 8, 11, 4, 10, 12, 13, 15, 6, 14, 1},
        {5, 8, 6, 9, 10, 14, 1, 2, 3, 0, 13, 7, 12, 15, 11, 4},
        {0, 7, 2, 10, 14, 6, 3, 11, 9, 15, 5, 12, 4, 13, 8, 1},
        {4, 1, 14, 3, 12, 8, 5, 2, 7, 13, 15, 6, 9, 11, 10, 0}},
    {
        {0, 2, 7, 10, 1, 11, 4, 9, 12, 8, 13, 5, 14, 15, 6, 3},
        {0, 3, 2, 7, 6, 13, 4, 10, 11, 5, 12, 9, 14, 15, 8, 1},
        {2, 10, 14, 13, 11, 7, 8, 9, 4, 0, 15, 5, 12, 6, 1, 3},
        {0, 11, 3, 12, 13, 15, 1, 8, 6, 2, 14, 4, 10, 9, 5, 7},
        {8, 4, 12, 1, 10, 14, 0, 9, 2, 11, 13, 5, 7, 15, 6, 3},
        {2, 10, 8, 1, 7, 14, 3, 4, 9, 11, 6, 13, 15, 12, 5, 0},
        {8, 4, 12, 10, 1, 7, 11, 15, 5, 13, 14, 9, 2, 6, 0, 3},
        {0, 10, 3, 4, 6, 8, 1, 5, 12, 7, 14, 2, 9, 15, 11, 13},
        {0, 6, 2, 9, 10, 5, 3, 15, 8, 11, 4, 13, 12, 14, 7, 1},
        {1, 3, 15, 13, 10, 14, 0, 8, 9, 6, 12, 5, 4, 11, 2, 7},
        {7, 5, 4, 2, 8, 15, 3, 1, 9, 0, 14, 12, 13, 11, 6, 10},
        {9, 8, 10, 6, 7, 13, 2, 15, 5, 11, 12, 14, 3, 0, 4, 1},
        {5, 10, 13, 7, 2, 6, 0, 12, 3, 14, 15, 9, 8, 1, 11, 4},
        {4, 9, 10, 8, 0, 14, 1, 7, 6, 2, 12, 5, 13, 15, 3, 11},
        {4, 6, 14, 10, 8, 9, 1, 0, 13, 2, 15, 11, 12, 7, 3, 5},
        {12, 9, 14, 15, 4, 6, 13, 2, 11, 8, 7, 10, 3, 1, 0, 5}},
    {
        {0, 9, 15, 7, 12, 8, 10, 3, 2, 5, 11, 13, 6, 14, 1, 4},
        {0, 1, 8, 10, 7, 4, 6, 12, 3, 15, 9, 14, 5, 13, 2, 11},
        {0, 5, 13, 2, 12, 3, 14, 11, 8, 6, 15, 7, 1, 10, 9, 4},
        {10, 11, 15, 13, 0, 1, 3, 7, 12, 8, 9, 2, 4, 6, 5, 14},
        {0, 2, 11, 8, 5, 6, 1, 10, 4, 14, 15, 9, 3, 12, 7, 13},
        {0, 2, 7, 10, 1, 8, 6, 12, 4, 14, 15, 11, 5, 3, 9, 13},
        {1, 15, 3, 5, 7, 13, 0, 9, 2, 6, 11, 14, 10, 12, 8, 4},
        {1, 13, 15, 11, 14, 0, 9, 8, 12, 2, 10, 3, 6, 5, 4, 7},
        {1, 12, 5, 8, 6, 2, 4, 14, 3, 13, 15, 11, 7, 10, 9, 0},
        {4, 8, 12, 0, 5, 9, 6, 14, 2, 13, 15, 11, 10, 1, 3, 7},
        {4, 8, 5, 0, 15, 9, 6, 14, 2, 13, 12, 11, 10, 1, 3, 7},
        {0, 11, 14, 8, 9, 1, 3, 2, 5, 10, 13, 15, 12, 7, 6, 4},
        {0, 9, 10, 12, 4, 8, 11, 14, 1, 6, 13, 15, 7, 3, 5, 2},
        {1, 8, 15, 2, 6, 7, 4, 14, 0, 9, 10, 5, 13, 12, 11, 3},
        {5, 0, 14, 2, 15, 12, 13, 8, 4, 3, 6, 1, 10, 11, 7, 9},
        {1, 0, 8, 3, 2, 14, 9, 7, 5, 12, 4, 15, 10, 11, 13, 6}},
    {
        {4, 8, 15, 7, 9, 12, 13, 14, 0, 11, 1, 5, 10, 3, 2, 6},
        {1, 10, 9, 14, 6, 3, 4, 5, 7, 2, 8, 13, 0, 15, 11, 12},
        {0, 1, 2, 10, 12, 13, 7, 9, 6, 11, 3, 14, 15, 8, 4, 5},
        {1, 7, 6, 4, 14, 11, 2, 13, 5, 0, 3, 8, 10, 9, 15, 12},
        {3, 2, 9, 5, 11, 10, 8, 15, 7, 14, 1, 0, 12, 6, 13, 4},
        {11, 12, 9, 0, 10, 15, 14, 13, 2, 3, 1, 8, 4, 5, 6, 7},
        {5, 6, 13, 9, 10, 1, 14, 11, 3, 0, 12, 2, 8, 7, 15, 4},
        {3, 2, 9, 5, 11, 10, 8, 15, 7, 14, 1, 0, 12, 6, 13, 4},
        {11, 13, 9, 0, 10, 14, 12, 15, 2, 3, 1, 8, 4, 5, 6, 7},
        {0, 2, 10, 9, 12, 8, 14, 3, 1, 13, 5, 7, 4, 6, 15, 11},
        {2, 8, 9, 3, 10, 14, 4, 6, 0, 15, 5, 11, 13, 12, 1, 7},
        {0, 15, 2, 1, 8, 6, 10, 11, 4, 9, 12, 7, 5, 3, 14, 13},
        {0, 8, 5, 1, 7, 6, 3, 14, 2, 11, 9, 15, 13, 10, 12, 4},
        {1, 0, 3, 8, 12, 2, 9, 15, 6, 10, 5, 13, 11, 7, 14, 4},
        {1, 9, 7, 4, 2, 6, 11, 13, 0, 8, 10, 15, 12, 5, 14, 3},
        {0, 8, 2, 13, 7, 1, 11, 12, 14, 5, 4, 15, 9, 10, 6, 3}},
    {
        {0, 8, 2, 11, 7, 1, 15, 14, 13, 5, 4, 12, 9, 10, 6, 3},
        {0, 11, 2, 14, 10, 1, 3, 6, 7, 8, 13, 12, 4, 15, 9, 5},
        {0, 11, 6, 10, 3, 1, 5, 8, 12, 9, 14, 13, 7, 15, 2, 4},
        {0, 3, 12, 1, 2, 4, 15, 6, 8, 9, 13, 10, 14, 5, 7, 11},
        {3, 2, 4, 1, 11, 0, 9, 8, 5, 6, 13, 15, 10, 14, 7, 12},
        {1, 13, 12, 5, 2, 9, 14, 11, 8, 3, 7, 15, 10, 6, 4, 0},
        {8, 4, 2, 14, 5, 0, 9, 13, 10, 1, 11, 15, 7, 6, 12, 3},
        {1, 8, 12, 4, 2, 3, 7, 15, 14, 0, 9, 10, 13, 11, 6, 5},
        {7, 2, 14, 9, 1, 13, 8, 15, 12, 0, 4, 10, 3, 11, 6, 5},
        {3, 8, 13, 10, 1, 6, 5, 4, 2, 0, 12, 15, 9, 11, 7, 14},
        {2, 8, 11, 5, 3, 0, 15, 14, 10, 1, 13, 12, 9, 7, 6, 4},
        {3, 2, 10, 1, 11, 0, 9, 14, 13, 8, 12, 15, 7, 4, 6, 5},
        {1, 0, 6, 7, 9, 3, 10, 8, 4, 14, 12, 13, 5, 11, 15, 2},
        {0, 6, 1, 4, 15, 5, 11, 8, 13, 2, 7, 14, 3, 9, 12, 10},
        {0, 15, 1, 12, 11, 10, 4, 7, 14, 2, 9, 13, 5, 3, 6, 8},
        {2, 4, 8, 0, 9, 10, 12, 13, 11, 5, 15, 7, 1, 6, 14, 3}},
    {
        {2, 11, 1, 6, 9, 3, 7, 15, 0, 4, 13, 5, 10, 12, 8, 14},
        {0, 11, 4, 3, 1, 7, 2, 8, 5, 15, 12, 14, 13, 9, 10, 6},
        {11, 13, 1, 0, 7, 2, 10, 6, 9, 14, 4, 5, 3, 12, 15, 8},
        {8, 12, 14, 5, 2, 3, 1, 0, 6, 10, 13, 4, 9, 15, 11, 7},
        {5, 15, 3, 7, 4, 14, 12, 11, 6, 8, 13, 2, 9, 10, 0, 1},
        {10, 4, 15, 7, 1, 5, 12, 8, 9, 11, 2, 3, 13, 6, 14, 0},
        {1, 14, 8, 3, 0, 12, 10, 9, 2, 4, 15, 11, 5, 6, 7, 13},
        {13, 2, 5, 7, 1, 4, 8, 14, 9, 11, 0, 6, 12, 15, 3, 10},
        {1, 6, 13, 5, 11, 8, 7, 0, 2, 9, 12, 10, 14, 15, 3, 4},
        {5, 9, 3, 6, 13, 12, 11, 10, 4, 2, 15, 14, 7, 8, 0, 1},
        {2, 1, 5, 9, 13, 11, 0, 10, 3, 8, 14, 15, 4, 12, 6, 7},
        {1, 0, 6, 8, 5, 3, 13, 14, 4, 12, 10, 11, 7, 9, 15, 2},
        {1, 0, 5, 11, 9, 3, 10, 2, 7, 12, 4, 15, 8, 13, 6, 14},
        {1, 0, 6, 11, 8, 3, 5, 10, 4, 14, 9, 13, 7, 12, 15, 2},
        {5, 4, 2, 8, 14, 13, 0, 11, 7, 3, 10, 6, 9, 12, 15, 1},
        {5, 7, 15, 11, 2, 6, 12, 9, 8, 10, 4, 1, 0, 14, 3, 13}},
    {
        {0, 1, 11, 7, 8, 4, 12, 5, 2, 10, 14, 9, 6, 15, 13, 3},
        {5, 4, 8, 11, 12, 9, 3, 13, 6, 10, 15, 1, 2, 14, 7, 0},
        {1, 0, 7, 13, 6, 12, 11, 5, 3, 2, 4, 15, 8, 10, 14, 9},
        {2, 6, 12, 15, 5, 4, 13, 3, 14, 0, 11, 8, 10, 1, 9, 7},
        {8, 15, 12, 13, 7, 5, 14, 2, 9, 4, 1, 10, 0, 11, 6, 3},
        {9, 4, 5, 7, 8, 1, 6, 2, 11, 13, 0, 15, 3, 14, 12, 10},
        {3, 9, 11, 14, 8, 2, 0, 1, 7, 6, 10, 5, 12, 15, 13, 4},
        {6, 12, 4, 9, 8, 10, 14, 11, 5, 2, 15, 3, 7, 13, 0, 1},
        {0, 10, 13, 9, 5, 3, 6, 7, 1, 14, 12, 2, 15, 4, 11, 8},
        {6, 12, 4, 13, 11, 7, 9, 15, 5, 14, 8, 3, 10, 0, 1, 2},
        {1, 6, 0, 9, 8, 5, 10, 3, 7, 2, 12, 11, 15, 13, 14, 4},
        {6, 15, 4, 12, 9, 8, 11, 7, 5, 13, 10, 3, 14, 0, 1, 2},
        {0, 12, 6, 11, 14, 3, 15, 4, 5, 7, 13, 1, 2, 8, 9, 10},
        {4, 5, 8, 1, 9, 3, 15, 7, 6, 11, 13, 12, 10, 14, 0, 2},
        {6, 14, 5, 0, 12, 11, 2, 13, 1, 7, 9, 10, 8, 3, 15, 4},
        {3, 9, 7, 13, 4, 1, 5, 6, 8, 10, 14, 12, 2, 11, 15, 0}},
    {
        {11, 2, 13, 5, 0, 12, 3, 15, 9, 1, 14, 7, 4, 8, 6, 10},
        {4, 5, 13, 9, 11, 1, 0, 14, 6, 2, 15, 7, 8, 10, 12, 3},
        {0, 8, 15, 2, 13, 12, 4, 3, 5, 14, 7, 9, 1, 10, 6, 11},
        {1, 10, 8, 9, 0, 13, 12, 11, 2, 3, 15, 7, 4, 5, 14, 6},
        {9, 1, 5, 13, 6, 0, 3, 15, 12, 14, 2, 7, 4, 10, 11, 8},
        {7, 6, 13, 9, 11, 1, 0, 15, 5, 2, 14, 10, 4, 12, 8, 3},
        {7, 5, 14, 9, 11, 1, 0, 15, 4, 2, 13, 10, 6, 12, 8, 3},
        {2, 15, 13, 5, 14, 9, 4, 10, 0, 11, 1, 3, 7, 6, 12, 8},
        {9, 7, 11, 12, 13, 2, 1, 0, 6, 3, 14, 8, 5, 15, 10, 4},
        {1, 2, 5, 13, 14, 3, 12, 15, 4, 7, 6, 11, 8, 9, 10, 0},
        {2, 1, 10, 11, 13, 4, 12, 14, 0, 5, 15, 3, 6, 7, 8, 9},
        {0, 11, 10, 8, 3, 2, 14, 1, 7, 9, 13, 5, 6, 12, 15, 4},
        {4, 11, 8, 12, 6, 1, 13, 0, 7, 15, 10, 14, 2, 9, 5, 3},
        {6, 13, 4, 7, 5, 8, 15, 12, 11, 9, 2, 3, 0, 1, 14, 10},
        {3, 11, 10, 0, 1, 6, 4, 5, 7, 12, 15, 14, 13, 8, 9, 2},
        {1, 0, 11, 8, 4, 7, 10, 15, 5, 13, 9, 2, 14, 3, 12, 6}}};

/* uint32_t[16] packet cipher round keys (UiTransfer_EncryptPacketBlocks/DecryptPacketBlocks take this as the 16-key table). Keys 12..15 are plain key values, although their bytes spell the text "mohTG sakere!!!e". */
static const uint32_t g_UiTransferRoundKeys[16] = {
    /*  0 */ 0x1234567, 0x13579BDF, 0x76543210, 0xFDB97531, 0x2468ACE, 0x2357BD23, 0xECA86420, 0x32DB7532,
    /*  8 */ 0xF1E2D3C, 0x4B5A6978, 0xC3D2E1F0, 0x8796A5B4, 0x54686F6D, 0x61732047, 0x6572656B, 0x65212121,
};

/* mailbox chunk packet (0x10031 request / 0x80030 chunk, 0x100 bytes): header (with packetHeader.sequenceToken), payload = chunk offset (payload byte 0), transfer byte count (payload byte 4), chunk data (from payload byte 8, 58 dwords). */
static UiRuntimeRecord g_UiTransferChunkPacket = {0};

/* ping answer packet 0x10033 (same layout as the 0x10032 ping): header (with header.sequenceToken), echoed tick (backendSessionValue). */
static FrontendPacket10032HostValue g_UiTransferPingEchoPacket = {0};

static UiTransferMailboxTickCounter g_UiTransferMailboxTickCounter = 0;

/* version string shown to joining players ("1.5.45") */
static uint16_t g_GameVersionUtf16[7] = {'1', '.', '5', '.', '4', '5', 0}; /* L"1.5.45" */

static FrontendPacket10000Handshake g_FrontendPacket10000Buffer = {0};

static FrontendPacket50001SessionAdvertisement g_FrontendPacket50001Buffer = {0};

static FrontendPacket20002PlayerDescriptor g_FrontendPacket20002Buffer = {0};

static FrontendPacket10003JoinAck g_FrontendPacket10003Buffer = {0};

static FrontendPacket10004PlayerSnapshotRequest g_FrontendPacket10004Buffer = {0};

static FrontendPacket10006CapabilityHeartbeat g_FrontendPacket10006Buffer = {0};

static FrontendPacket40008LobbyRosterSnapshot g_FrontendPacket40008Buffer = {0};

static FrontendPacket8000ASnapshotChunk g_FrontendPacket8000ABuffer = {0};

static FrontendCommandPacketRecord g_FrontendPacket10011Buffer = {0};

static FrontendPacket10013HeartbeatAck g_FrontendPacket10013Buffer = {0};

static FrontendPacket10032HostValue g_FrontendPacket10032Buffer = {0};

static uint32_t g_FrontendHostPublishRoundRobinCounter = 0;

uint32_t g_UiRuntimeRecordWriteIndex = 0;

uint32_t g_UiTransferUnitCursor = 0;

/* uint32_t sequence token stamped into outgoing network packets (initial 0x12340000, low 16 bits XORed with a random value in transfer.c; network/protocol/transfer.c, ui/frontend/network.c). */
uint32_t g_UiTransferSequenceToken = 0x12340000;

uint32_t g_UiTransferSenderContext = 0;

UiTransferMailboxState g_UiTransferMailbox = {0};

UiTransferEndpointDescriptor g_FrontendSelectedNetworkEndpoint = {0};

uint32_t g_FrontendSessionToken = 0;

SessionTransferTimeoutTicks g_SessionTransferTimeoutTicks = 0;

uint32_t g_FrontendTransferResponsePending = 0;

uint32_t g_FrontendLocalPlayerPcxPreview = 0;

uint32_t g_FrontendPendingSessionPlayerCount = 0;

uint32_t g_FrontendExpectedPlayerRuntimeBlockCount = 0;

FrontendCommandPacketRecord g_FrontendClientPlayerCommandRecords[8] = {0};

FrontendCommandPacketRecord g_FrontendClientCommandBatchPacketBuffer[8] = {0};

FrontendCommandPacketRecord g_FrontendPacket10021Buffer = {0};

/* Implementation ownership: network/protocol/transfer. */

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
  if (g_UiTransferMailbox.receivedAllocation == NULL) {
    return;
  }
  if (g_UiTransferMailbox.receivedAllocation == UI_TRANSFER_MAILBOX_UNAVAILABLE) {
    if (g_MemoryApi.alloc(totalByteCount,&g_UiTransferMailbox.receivedAllocation) != 0) {
      return;
    }
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

  if (g_UiTransferMailbox.outgoingAllocation == NULL) {
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
  /* original quirk: extends the sender's receive scratch slot instead of the player record, see the timer's
     comment */
  senderEndpointSlot->transferTimeoutTicks =
       senderEndpointSlot->transferTimeoutTicks + UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS;
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
   senderEndpointSlot->transferTimeoutTicks (the sender's slot of the auxiliary endpoint buffer), not to the
   requesting player's heartbeatExpiryTicks (the same offset in the player record), which was probably meant; the
   scratch dword is never read, so the host's heartbeat countdown is not extended by chunk requests. The client
   branch (0x80030) extends g_SessionTransferTimeoutTicks as intended.
*/
void UiTransferMailbox_ServiceAndRetransmitTimer(void)

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
                   (g_UiRuntimeRecordWriteIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeRecordEndpointSlots),256,
                   (uint8_t *)(g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex))) {
    slotIndex = g_UiRuntimeRecordWriteIndex;
    nextSlotIndex = slotIndex + 1;
    ringRecord = g_UiRuntimeRecordRing + slotIndex;
    if (!UiTransferMailbox_DecryptAndVerifyRecord(ringRecord)) {
      continue;
    }
    senderEndpointSlot =
         (UiTransferSenderEndpointSlot *)(slotIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeRecordEndpointSlots);
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


/* Copies one 0x20-byte command packet record dword by dword. */
static void FrontendTransfer_CopyCommandRecord
          (FrontendCommandPacketRecord *destination,const FrontendCommandPacketRecord *source)
{
  uint32_t *destinationDwords;
  const uint32_t *sourceDwords;
  int dwordCount;

  destinationDwords = (uint32_t *)destination;
  sourceDwords = (const uint32_t *)source;
  for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
    *destinationDwords = *sourceDwords;
    sourceDwords++;
    destinationDwords++;
  }
}

/* Executes commandCount consecutive 0x20-byte lobby command records (the original executes at least one: a
   count of 0 wraps). Command dword = handler offset << 8 | player id; offsets past the command handlers are
   ignored. */
static void FrontendTransfer_ExecuteLobbyCommandRecords
          (const FrontendCommandPacketRecord *commandRecord,uint32_t commandCount)
{
  uint32_t packedCommand;
  uint32_t commandHandlerIndex;
  CommandQueueHandlerProc *commandHandler;

  /* Not in the original: a count of 0 (which wraps) or one past the 0x100-byte receive slot executes nothing.
     The receive check (UiTransferMailbox_DecryptAndVerifyRecord) already drops such packets; valid batches
     have 1..8 records. */
  if (commandCount == 0 || commandCount > FRONTEND_PACKET_MAX_UNIT_COUNT) {
    return;
  }
  do {
    packedCommand = commandRecord->command.packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      commandHandler = CommandDispatch_ResolveHandler
                                 (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
      if (commandHandler != NULL) {
        (*commandHandler)
                  (packedCommand & 0xff,commandRecord->command.payload1,commandRecord->command.payload2,
                   commandRecord->command.payload3);
      }
    }
    commandRecord++;
    commandCount--;
  } while (commandCount != 0);
}

/* Client side of the host lobby: accepts the host's 0x40008 session packet (one player-list row and the
   player's name; a non-zero expected block count starts the session and switches to
   FRONTEND_NETWORK_STATE_CLIENT_STARTING) and the host's lobby command batches, whose commands it executes
   before answering with its own next queued command (packet 0x10011). Packets from other hosts or sessions
   are ignored.
*/
void FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int dwordCount;
  int playerIndex;
  uint32_t playerCount;
  uint32_t *packetCursor;
  uint32_t *nameSourceCursor;
  uint32_t *nameDestinationCursor;
  uint32_t *playerRowCursor;
  FrontendPlayerRuntimeRecord *playerRecord;

  if ((packet->packet40008LobbyRosterSnapshot.header.packedTypeAndUnitCount ==
       FRONTEND_PACKET_40008_SESSION_PLAYER_ROW) &&
      (g_FrontendSessionToken == packet->packet40008LobbyRosterSnapshot.header.sequenceToken) &&
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder == senderEndpoint->ipv4AddressNetworkOrder)) {
    playerCount = packet->packet40008LobbyRosterSnapshot.playerCount;
    if ((packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex < 8) && (playerCount < 9)) {
      packetCursor = (uint32_t *)packet;
      playerRowCursor = (uint32_t *)g_FrontendPlayerListRows
                        [packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex];
      g_FrontendPlayerRuntimeCount = playerCount;
      /* the whole 0x80-byte packet becomes the player's list row */
      for (dwordCount = 32; dwordCount != 0; dwordCount--) {
        *playerRowCursor = *packetCursor;
        packetCursor++;
        playerRowCursor++;
      }
      playerRecord = g_FrontendPlayerRuntimeBlocks +
               packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex;
      playerRecord->playerRuntimeId = packet->packet40008LobbyRosterSnapshot.selectedPlayerRuntimeId;
      /* the player's name (playerName.textUtf16) */
      nameSourceCursor = packet->packet40008LobbyRosterSnapshot.playerDescriptorPayload;
      nameDestinationCursor = (uint32_t *)playerRecord->playerName.textUtf16;
      for (dwordCount = 10; dwordCount != 0; dwordCount--) {
        *nameDestinationCursor = *nameSourceCursor;
        nameSourceCursor++;
        nameDestinationCursor++;
      }
      UiPointerList_InitializeColumnLayout
                (playerCount,(void **)g_FrontendPlayerListRows,
                 (UiPointerListControl *)FRONTEND_UI(frontendRuntime,clientLobbyPlayerList));
    }
    g_SessionTransferTimeoutTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
    /* the host starts the session: this many player snapshots follow */
    if (packet->packet40008LobbyRosterSnapshot.pendingSessionPlayerCount != 0) {
      g_FrontendPlayerRuntimeBlockCount = 0;
      g_FrontendExpectedPlayerRuntimeBlockCount =
           packet->packet40008LobbyRosterSnapshot.pendingSessionPlayerCount;
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(frontendRuntime,frontendPageStack));
      FrontendState_DispatchCode(1);
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_CLIENT_STARTING;
      FrontendTransfer_SendLobbyCommandAndSnapshotRequest();
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      for (playerIndex = 0; playerIndex < 8; playerIndex++) {
        playerRecord->factionAssignment.roleStateFlags = 0;
        playerRecord->colourCycleFlags = 0;
        playerRecord->snapshotTransferFlags = 0;
        playerRecord++;
      }
    }
    return;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
       FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken) &&
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder == senderEndpoint->ipv4AddressNetworkOrder)) {
    FrontendTransfer_ExecuteLobbyCommandRecords
              (&packet->command10011Or10021,
               packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT);
    g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011_LOBBY_COMMAND;
    FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
    if (g_FrontendPacket10011Buffer.command.packedCommandAndPlayerId != 0) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
    }
  }
}


/* Client side of the session start: executes a new command batch from the host (a repeated batch only
   re-sends the last 0x10011 answer), answers 0x10012 with 0x10013, removes a player on 0x10007, stores the
   next player snapshot (0x30005, which also seeds the random streams) and serves chunks of the local
   player's PCX preview on 0x10009. Only packets of the selected host and session count; returns true only when
   a new command batch was executed.
*/
Bool8 FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg)

{
  UiTransferSenderContext batchSenderContext;
  uint32_t expectedBlockCount;
  uint32_t playerIndex;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int dwordCount;
  uint32_t *nextPlayerCursor;
  uint32_t *packetCursor;
  uint32_t *previewSourceCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *recordDwordCursor;
  uint32_t *chunkDestinationCursor;
  uint16_t *resolvedText;

  expectedBlockCount = g_FrontendExpectedPlayerRuntimeBlockCount;
  /* only packets of the selected host and session count */
  if ((g_FrontendSessionToken != packet->packet10000Handshake.header.sequenceToken) ||
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder != senderEndpoint->ipv4AddressNetworkOrder)) {
    return false;
  }
  if ((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
      FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) {
    batchSenderContext = packet->packet10000Handshake.header.senderContext;
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* the same batch again: our answer got lost, repeat it */
    if (batchSenderContext == g_FrontendSelectedPlayerToken) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
      return false;
    }
    g_FrontendSelectedPlayerToken = batchSenderContext;
    FrontendTransfer_ExecuteLobbyCommandRecords
              (&packet->command10011Or10021,
               packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT);
    FrontendTransfer_SendLobbyCommandAndSnapshotRequest();
    g_FrontendTransferResponsePending = 1;
    return true;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10012_WAIT) {
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    g_FrontendPacket10013Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10013_WAIT_ACK;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10013Buffer.header);
    return false;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10007_PLAYER_REMOVAL) {
    /* the first record is compared before the count is checked */
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if (packet->playerRemoval10007.removedPlayerToken == playerRecord->playerRuntimeId) {
        /* "player left" message with the name, then close the gap in the record array */
        resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_REMOVED);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText);
        FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
        nextPlayerCursor = (uint32_t *)(playerRecord + 1);
        recordDwordCursor = (uint32_t *)playerRecord;
        for (dwordCount = (playersRemaining - 1) * (sizeof(FrontendPlayerRuntimeRecord) / sizeof(uint32_t));
             dwordCount != 0; dwordCount--) {
          *recordDwordCursor = *nextPlayerCursor;
          nextPlayerCursor++;
          recordDwordCursor++;
        }
        g_FrontendPlayerRuntimeBlockCount--;
        return false;
      }
      playerRecord++;
      playersRemaining--;
    } while (playersRemaining != 0);
    return false;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_30005_PLAYER_SNAPSHOT) {
    /* only the next missing snapshot is taken */
    playerIndex = packet->packet30005PlayerSnapshot.playerIndex;
    if ((playerIndex < g_FrontendExpectedPlayerRuntimeBlockCount) &&
        (playerIndex == g_FrontendPlayerRuntimeBlockCount)) {
      Random_SetBothSeeds(packet->packet30005PlayerSnapshot.secondaryRandomSeed);
      Random_SelectSecondaryStream();
      g_FrontendPlayerRuntimeBlockCount++;
      packetCursor = (uint32_t *)packet;
      recordDwordCursor = (uint32_t *)(g_FrontendPlayerRuntimeBlocks + playerIndex);
      /* the first 0x60 bytes of the packet become the head of the player's record */
      for (dwordCount = 24; dwordCount != 0; dwordCount--) {
        *recordDwordCursor = *packetCursor;
        packetCursor++;
        recordDwordCursor++;
      }
      resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_ARRIVED);
      RichTextCommandStream_PatchPayloadBySelector
                (0,packet->packet30005PlayerSnapshot.playerDescriptorPayload,resolvedText);
      FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
    }
    return false;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST) {
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         packet->packet10009SnapshotChunkRequest.snapshotChunkOffset;
    chunkDestinationCursor = (uint32_t *)g_FrontendPacket8000ABuffer.packet10009Buffer;
    previewSourceCursor = (uint32_t *)
             (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A_SNAPSHOT_CHUNK;
    /* 0xE8-byte chunks; the last one at 0x1220 has 0xE0 bytes (the preview is 0x1300 bytes) */
    dwordCount = 58;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
      dwordCount = 56;
    }
    for (; dwordCount != 0; dwordCount--) {
      *chunkDestinationCursor = *previewSourceCursor;
      previewSourceCursor++;
      chunkDestinationCursor++;
    }
    /* answered only once every expected player snapshot has arrived */
    if (expectedBlockCount == g_FrontendPlayerRuntimeBlockCount) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket8000ABuffer.header);
    }
    return false;
  }
  return false;
}


/* Frontend command handler FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE, queued by
   FrontendNetwork_HostTickCommandAndSnapshotTransfer once the host has published the packed player
   snapshots and executed on every peer: a client marks its receive mailbox unavailable, so it waits for
   the new transfer instead of reading an old one. The host and a local game do nothing.
*/
void FrontendTransfer_MarkUnavailableIfModeBit0Callback(uint32_t senderPlayerId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_MarkUnavailable();
  }
  return;
}


/* Frontend command handler 0x1710 (relative to FRONTEND_COMMAND_CODE_BASE), queued by a client in
   Frontend_MainLoop once it has unpacked the host's published player snapshots, and executed on every
   peer: marks that player FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY. On the host, once every player is
   marked, the published block is no longer needed: its allocation is freed and the outgoing mailbox
   emptied.
*/
void FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady
          (int playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3)

{
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;

  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerRuntimeId == playerRecord->playerRuntimeId) {
      playerRecord->snapshotTransferFlags =
           playerRecord->snapshotTransferFlags | FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
      playersRemaining = g_FrontendPlayerRuntimeBlockCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
        return;
      }
      do {
        if ((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
          return;
        }
        playersRemaining--;
        playerRecord = playerRecord + 1;
      } while (playersRemaining != 0);
      g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
      UiTransferMailbox_SetOutgoingBuffer(0,NULL);
      return;
    }
    playerRecord = playerRecord + 1;
    playersRemaining--;
  } while (playersRemaining != 0);
  return;
}


/* Sends the session discovery probe (0x10000 handshake with FRONTEND_PROTOCOL_MAGIC) to
   g_FrontendNetworkEndpointScratch, the address from the join dialog or the broadcast address. Hosts answer
   with a 0x50001 session advertisement. Returns the result of UiTransfer_StagePacketAndSend.
*/
Bool8 UiTransfer_SendDiscoveryProbe(void)

{
  Bool8 sendCarry;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic = FRONTEND_PROTOCOL_MAGIC;
  sendCarry = UiTransfer_StagePacketAndSend
                    (&g_FrontendNetworkEndpointScratch,&g_FrontendPacket10000Buffer.header);
  return sendCarry;
}


/* Introduces the local player to the host (0x20002 player descriptor): the player name (20 UTF-16 units)
   whose last unit is replaced by flags: bit 0 = a 64x64 picture <name>.pcx was found (loaded into
   g_FrontendLocalPlayerPcxPreview), bit 8 = shown as "CD" in the lobby list (always set). Returns the
   result of UiTransfer_StagePacketAndSend.
*/
Bool8 UiTransfer_SendPlayerDescriptor(void)

{
  int dwordCount;
  uint32_t *nameSourceCursor;
  uint32_t *payloadCursor;
  Bool8 callCarry;
  
  g_FrontendPacket20002Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR;
  g_FrontendPacket20002Buffer.payloadByteCount = 64;
  nameSourceCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
  payloadCursor = g_FrontendPacket20002Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount--) {
    *payloadCursor = *nameSourceCursor;
    nameSourceCursor++;
    payloadCursor++;
  }
  /* the last name unit becomes the flags word */
  ((uint16_t *)payloadCursor)[-1] = 0;
  callCarry = PcxPreview_Load64x64PaletteAndPixels
                    ((PcxPreview64 *)g_FrontendLocalPlayerPcxPreview,g_FrontendLocalPlayerNameUtf16);
  if (!callCarry) {
    ((uint16_t *)payloadCursor)[-1] |= FRONTEND_DESCRIPTOR_HAS_PICTURE;
  }
  ((uint16_t *)payloadCursor)[-1] |= FRONTEND_CAPABILITY_CD;
  callCarry = UiTransfer_StagePacketAndSend
                    (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket20002Buffer.header);
  return callCarry;
}


/* Host: answers a discovery probe (0x10000) with the session advertisement 0x50001 (title, host description,
   player count); joinable while the lobby is not full. */
static void FrontendTransfer_SendSessionAdvertisement
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *hostLobbyPlayerList,UiRangeSliderControl *maxPlayersSlider)
{
  FrontendRootRuntimeAddress32 frontendRootNode;
  uint16_t *resolvedText;

  /* the game name comes from the frontend root node, not from the handler's frontendRuntime */
  frontendRootNode = g_FrontendRootNode;
  g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
  if ((packet->packet10000Handshake.protocolMagic == FRONTEND_PROTOCOL_MAGIC) &&
      ((packet->packet10000Handshake.header.sequenceToken & FRONTEND_SEQUENCE_TOKEN_HIGH_MASK) ==
       FRONTEND_SEQUENCE_TOKEN_HIGH_WORD) &&
      (hostLobbyPlayerList->rowCount < (uint32_t)maxPlayersSlider->value)) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
  }
  resolvedText = TextResource_Resolve(TEXT_ID_SESSION_TITLE_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,g_GameVersionUtf16,resolvedText);
  RichTextCommandStream_CopyExpanded
            (40,g_FrontendPacket50001Buffer.sessionTitleUtf16,resolvedText,NULL);
  resolvedText = TextResource_Resolve(TEXT_ID_SESSION_HOST_TEMPLATE);
  /* the game name typed into gameNameEdit */
  RichTextCommandStream_PatchPayloadBySelector
            (0,((UiTextEditControl *)FRONTEND_UI(frontendRootNode,gameNameEdit))->textBuffer,resolvedText);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendLocalPlayerNameUtf16,resolvedText);
  RichTextCommandStream_CopyExpanded
            (88,g_FrontendPacket50001Buffer.hostDescriptionUtf16,resolvedText,NULL);
  resolvedText = TextResource_Resolve(TEXT_ID_SESSION_PLAYER_COUNT_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendNetworkRuntimeCountTextUtf16,resolvedText);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendNetworkPlayerCountTextUtf16,resolvedText);
  RichTextCommandStream_CopyExpanded(8,g_FrontendPacket50001Buffer.playerCountTextUtf16,resolvedText,NULL);
  g_FrontendPacket50001Buffer.header.packedTypeAndUnitCount =
       FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT;
  g_FrontendPacket50001Buffer.payloadByteCount = 32;
  UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket50001Buffer.header);
}

/* Smallest player runtime id below 0xFF that no current player uses (0xFF when all are taken). */
static uint32_t FrontendTransfer_FindLowestFreePlayerRuntimeId(void)
{
  uint32_t candidateId;
  int playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;

  candidateId = 0;
  do {
    /* scan every player; restart with the next candidate as soon as one uses it */
    playersRemaining = g_FrontendPlayerRuntimeCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    while (candidateId != (uint32_t)playerRecord->playerRuntimeId) {
      playersRemaining--;
      playerRecord++;
      if (playersRemaining == 0) {
        return candidateId;
      }
    }
    candidateId++;
  } while (candidateId < 255);
  return candidateId;
}

/* Host: admits a joining player (0x20002 player descriptor): a new player-list row, the lowest free player id
   and the join ack 0x10003. */
static void FrontendTransfer_AdmitJoiningPlayer
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *hostLobbyPlayerList)
{
  uint16_t descriptorStatusBits;
  uint32_t assignedPlayerRuntimeId;
  int dwordCount;
  const uint32_t *packetDwords;
  const uint32_t *endpointDwords;
  uint32_t *joiningPlayerRecordDwordCursor;

  joiningPlayerRecordDwordCursor = (uint32_t *)hostLobbyPlayerList->rowSlots[hostLobbyPlayerList->rowCount];
  hostLobbyPlayerList->rowCount = hostLobbyPlayerList->rowCount + 1;
  /* the new row: the 0x40-byte descriptor packet followed by the sender's 0x10-byte endpoint */
  packetDwords = (const uint32_t *)packet;
  for (dwordCount = 16; dwordCount != 0; dwordCount--) {
    *joiningPlayerRecordDwordCursor = *packetDwords;
    packetDwords++;
    joiningPlayerRecordDwordCursor++;
  }
  endpointDwords = (const uint32_t *)senderEndpoint;
  for (dwordCount = 4; dwordCount != 0; dwordCount--) {
    *joiningPlayerRecordDwordCursor = *endpointDwords;
    endpointDwords++;
    joiningPlayerRecordDwordCursor++;
  }
  assignedPlayerRuntimeId = FrontendTransfer_FindLowestFreePlayerRuntimeId();
  /* the cursor now points just past the copied endpoint; the fields below are addressed relative to it */
  descriptorStatusBits = *(uint16_t *)((uint8_t *)joiningPlayerRecordDwordCursor - 18);
  *joiningPlayerRecordDwordCursor = 1;
  joiningPlayerRecordDwordCursor[-15] = assignedPlayerRuntimeId;
  joiningPlayerRecordDwordCursor[6] = descriptorStatusBits & 0xff;
  joiningPlayerRecordDwordCursor[9] = descriptorStatusBits & 0xff00;
  joiningPlayerRecordDwordCursor[16] = 0;
  joiningPlayerRecordDwordCursor[17] = 0;
  joiningPlayerRecordDwordCursor[7] = 0;
  joiningPlayerRecordDwordCursor[10] = 0;
  joiningPlayerRecordDwordCursor[11] = 0;
  joiningPlayerRecordDwordCursor[13] = 0;
  joiningPlayerRecordDwordCursor[14] = 0;
  joiningPlayerRecordDwordCursor[15] = 0;
  if ((descriptorStatusBits & FRONTEND_CAPABILITY_CD) != 0) {
    joiningPlayerRecordDwordCursor[10] = 0x440043; /* L"CD" */
  }
  *(uint16_t *)((uint8_t *)joiningPlayerRecordDwordCursor - 18) = 0;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)hostLobbyPlayerList->rowCount,
             g_FrontendNetworkRuntimeCountTextUtf16);
  g_FrontendPacket10003Buffer.networkTickInterval = g_SessionNetworkTickInterval;
  g_FrontendPacket10003Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10003_JOIN_ACK;
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = assignedPlayerRuntimeId;
  UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket10003Buffer.header);
  g_FrontendPlayerRuntimeCount++;
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
}

/* Host: stores a player's capability heartbeat (0x10006) and its "CD" label. */
static void FrontendTransfer_StoreCapabilityHeartbeat
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *hostLobbyPlayerList)
{
  FrontendCapabilityFlags capabilityFlags;
  FrontendHeartbeatTickCount heartbeatExpiryTicks;
  int playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;

  /* the first record is compared before the count is checked */
  playersRemaining = (int)hostLobbyPlayerList->rowCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((packet->packet10000Handshake.header.sequenceToken != playerRecord->peerSequenceToken) ||
         (senderEndpoint->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)) {
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  capabilityFlags = packet->packet10006CapabilityHeartbeat.capabilityFlags;
  heartbeatExpiryTicks = packet->packet10006CapabilityHeartbeat.heartbeatExpiryTicks;
  playerRecord->capabilityFlags = capabilityFlags;
  playerRecord->heartbeatExpiryTicks = heartbeatExpiryTicks;
  playerRecord->capabilityLabelUtf16[0] = 0;
  playerRecord->capabilityLabelUtf16[1] = 0;
  playerRecord->capabilityLabelUtf16[2] = 0;
  playerRecord->capabilityLabelUtf16[3] = 0;
  if ((capabilityFlags & FRONTEND_CAPABILITY_CD) != 0) {
    /* L"CD" */
    playerRecord->capabilityLabelUtf16[0] = 'C';
    playerRecord->capabilityLabelUtf16[1] = 0;
    playerRecord->capabilityLabelUtf16[2] = 'D';
    playerRecord->capabilityLabelUtf16[3] = 0;
  }
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
}

/* Host: adds its own next queued command (slot 0) to the commands collected from the players, compacts the
   non-empty slots into one lobby command batch (clearing them; the player id stays), sends it to every player
   but the host (record 0) and executes it. Nothing is sent when no slot holds a command. */
static void FrontendTransfer_BroadcastAndExecuteLobbyCommands(void)
{
  uint32_t commandCount;
  int slotsRemaining;
  int peersRemaining;
  FrontendCommandPacketRecord *commandRecord;
  FrontendCommandPacketRecord *batchCursor;
  UiTransferEndpointDescriptor *peerEndpointCursor;

  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  commandCount = 0;
  commandRecord = g_FrontendPlayerCommandRecords;
  batchCursor = g_FrontendCommandBatchPacketBuffer;
  slotsRemaining = g_FrontendPlayerRuntimeCount;
  do {
    if ((commandRecord->command.packedCommandAndPlayerId & 0xffffff00) != 0) {
      FrontendTransfer_CopyCommandRecord(batchCursor,commandRecord);
      batchCursor++;
      commandCount++;
      commandRecord->command.packedCommandAndPlayerId = commandRecord->command.packedCommandAndPlayerId & 0xff;
    }
    commandRecord++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  if (commandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    return;
  }
  /* the first packed record's header doubles as the batch header */
  g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
       commandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE;
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  for (peersRemaining = g_FrontendPlayerRuntimeCount - 1; peersRemaining != 0; peersRemaining--) {
    UiTransfer_StagePacketAndSend(peerEndpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
    peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
  FrontendTransfer_ExecuteLobbyCommandRecords(g_FrontendCommandBatchPacketBuffer,commandCount & 0xffff);
}

/* Host: keeps a player's next command (0x10011) in its command slot and marks it pending, then broadcasts
   and executes the collected commands. Packets from unknown senders are ignored. */
static void FrontendTransfer_CollectLobbyCommand
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet)
{
  int playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;

  /* the command slots run parallel to the player records; the first record is compared before the count is
     checked */
  commandRecord = g_FrontendPlayerCommandRecords;
  playersRemaining = g_FrontendPlayerRuntimeCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((packet->packet10000Handshake.header.sequenceToken != playerRecord->peerSequenceToken) ||
         (senderEndpoint->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)) {
    playerRecord++;
    commandRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
  /* keep the player's 0x20-byte command packet; the host's own command goes into slot 0 */
  FrontendTransfer_CopyCommandRecord(commandRecord,&packet->command10011Or10021);
  FrontendTransfer_BroadcastAndExecuteLobbyCommands();
}

/* Host side of the lobby. Answers a discovery probe (0x10000) with the session advertisement (0x50001:
   title, host description, player count; joinable while the lobby is not full), admits a joining player
   (0x20002: new player-list row, lowest free player id, join ack 0x10003), stores a player's capability
   heartbeat (0x10006), and collects each player's next command (0x10011), broadcasting the non-empty ones
   as one lobby command batch that the host then executes itself.
   The lobby's player list is the frontend's hostLobbyPlayerList, the player limit the value of
   maxPlayersSlider.
*/
void FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  UiPointerListControl *hostLobbyPlayerList;
  UiRangeSliderControl *maxPlayersSlider;

  hostLobbyPlayerList = (UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList);
  maxPlayersSlider = (UiRangeSliderControl *)FRONTEND_UI(frontendRuntime,maxPlayersSlider);
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10000_HANDSHAKE) {
    FrontendTransfer_SendSessionAdvertisement(senderEndpoint,packet,hostLobbyPlayerList,maxPlayersSlider);
  }
  else if ((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR) &&
           ((uint32_t)maxPlayersSlider->value > hostLobbyPlayerList->rowCount)) {
    FrontendTransfer_AdmitJoiningPlayer(senderEndpoint,packet,hostLobbyPlayerList);
  }
  else if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT) {
    FrontendTransfer_StoreCapabilityHeartbeat(senderEndpoint,packet,hostLobbyPlayerList);
  }
  else if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10011_LOBBY_COMMAND) {
    FrontendTransfer_CollectLobbyCommand(senderEndpoint,packet);
  }
  /* a player descriptor while the lobby is full and every other packet are ignored */
}


/* Host lobby tick, first part: sends every joined player (peerCount = rowCount - 1, all but the host) the
   session packet 0x40008 for the player-list row chosen round robin, plus a 0x10032 tick stamp. */
static void FrontendTransfer_SendSessionPlayerRowToPeers(uint32_t roundRobinCounter,uint32_t rowCount,int peerCount)
{
  uint32_t selectedIndex;
  uint32_t textByteCount;
  int dwordCount;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  UiTransferEndpointDescriptor *endpoint;
  uint8_t *descriptorSourceCursor;
  uint32_t *descriptorDestinationCursor;

  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008_SESSION_PLAYER_ROW;
  g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
  g_FrontendHostPublishRoundRobinCounter++;
  selectedIndex = roundRobinCounter % rowCount;
  /* the selected player's record fields, addressed from the record-1 endpoint in 0x10-byte steps: the unit at
     heartbeatExpiryTicks gives playerRuntimeId (+4) and playerName (+8), the one at transferProgressBytes
     capabilityLabelUtf16 (+8), and pingRoundTripTicks is read directly */
  g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
       peerEndpointCursor[(selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(heartbeatExpiryTicks)].ipv4AddressNetworkOrder;
  g_FrontendPacket40008Buffer.selectedStatusCode0 =
       *(FrontendStatusCode *)peerEndpointCursor[(selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(transferProgressBytes)].zeroPadding;
  g_FrontendPacket40008Buffer.selectedStatusCode1 =
       *(FrontendStatusCode *)(peerEndpointCursor[(selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(transferProgressBytes)].zeroPadding + 4);
  g_FrontendPacket40008Buffer.selectedPlayerIndex = selectedIndex;
  g_FrontendPacket40008Buffer.playerCount = rowCount;
  endpoint = peerEndpointCursor;
  textByteCount = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                     peerEndpointCursor[(selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(pingRoundTripTicks)].addressHeader.packedFamilyAndPort << 2,
                     g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16);
  *(uint32_t *)((uint8_t *)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + textByteCount) =
       ('s' << 16 | 'm'); /* L"ms" */
  *(uint16_t *)((uint8_t *)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + textByteCount + 4) = 0;
  descriptorSourceCursor = peerEndpointCursor[(selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(heartbeatExpiryTicks)].zeroPadding;
  descriptorDestinationCursor = g_FrontendPacket40008Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount--) {
    *descriptorDestinationCursor = *(uint32_t *)descriptorSourceCursor;
    descriptorSourceCursor = descriptorSourceCursor + 4;
    descriptorDestinationCursor++;
  }
  g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032_PING;
  g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
  /* to every player but the host (record 0) */
  for (; peerCount != 0; peerCount--) {
    UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket40008Buffer.header);
    UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket10032Buffer.header);
    endpoint = endpoint + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
}

/* Host lobby tick. Sends every joined player the session packet 0x40008 for one player-list row, chosen
   round robin (with its row, name and ping text "<n>ms"), plus a 0x10032 tick stamp; a pending session start
   switches to FRONTEND_NETWORK_STATE_HOST_STARTING. Then it adds the host's own next queued command to the
   players' collected ones, broadcasts the non-empty ones as one lobby command batch and executes them.
*/
void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint32_t roundRobinCounter;
  uint32_t rowCount;
  int peerCount;

  roundRobinCounter = g_FrontendHostPublishRoundRobinCounter;
  rowCount = ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount;
  peerCount = rowCount - 1;
  if (peerCount != 0 && 0 < (int)rowCount) {
    FrontendTransfer_SendSessionPlayerRowToPeers(roundRobinCounter,rowCount,peerCount);
  }
  if (g_FrontendPendingSessionPlayerCount != 0) {
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOST_STARTING;
    g_FrontendPendingSessionPlayerCount = 0;
  }
  FrontendTransfer_BroadcastAndExecuteLobbyCommands();
}


/* Sends the client's capability heartbeat (0x10006) to the selected host: the CD capability and a heartbeat
   value of 0x40, which the host stores in this player's record.
*/
void FrontendTransfer_SendCapabilityHeartbeat(void)

{
  g_FrontendPacket10006Buffer.header.packedTypeAndUnitCount =
       FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT;
  g_FrontendPacket10006Buffer.capabilityFlags = FRONTEND_CAPABILITY_CD;
  g_FrontendPacket10006Buffer.heartbeatExpiryTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10006Buffer.header);
  return;
}


/* Host, while clients are still missing: resends the previous command batch to every client that has not
   submitted its command yet and COMMAND_WAIT to those that have. */
static void FrontendTransfer_ResendBatchOrWaitToClients(void)
{
  FrontendPlayerRuntimeBlockCount peersRemaining;
  UiTransferEndpointDescriptor *peerEndpointCursor;

  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  for (peersRemaining = g_FrontendPlayerRuntimeBlockCount - 1; peersRemaining != 0; peersRemaining--) {
    /* peerEndpointCursor[1] is the 0x10 bytes after the endpoint, i.e. the same record's
       commandSyncPending */
    if (peerEndpointCursor[1].addressHeader.packedFamilyAndPort == 0) {
      UiTransfer_StagePacketAndSend
                (peerEndpointCursor,&g_FrontendClientCommandBatchPacketBuffer[0].header);
    }
    else {
      g_FrontendPacket10022Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_COMMAND_WAIT;
      UiTransfer_StagePacketAndSend(peerEndpointCursor,&g_FrontendPacket10022Buffer.header);
    }
    /* 0x13B endpoints of 0x10 bytes = one 0x13B0-byte player record */
    peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
}

/* Host side of the in-game command exchange. When every client (player records 1..n-1) has submitted its
   command, clears their ready flags, takes the host's own next command into slot 0, packs all non-empty
   command slots into g_FrontendClientCommandBatchPacketBuffer (at least one record) and sends that
   COMMAND_BATCH to every client; returns false. Otherwise returns true and, with
   notifyWaitingPeers, resends the previous batch to clients that have not submitted yet and COMMAND_WAIT
   to those that have.
*/
Bool8 FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers)

{
  FrontendPlayerRuntimeBlockCount peersRemaining;
  FrontendPlayerRuntimeBlockCount slotsRemaining;
  int clientsRemaining;
  int batchCount;
  FrontendCommandPacketRecord *commandRecordCursor;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  FrontendPlayerRuntimeRecord *clientRecord;
  FrontendCommandPacketRecord *batchCursor;

  /* record 0 is the host itself, only the clients (records 1..n-1) are checked */
  if (g_FrontendPlayerRuntimeBlockCount - 1 != 0) {
    clientRecord = g_FrontendPlayerRuntimeBlocks + 1;
    for (clientsRemaining = g_FrontendPlayerRuntimeBlockCount - 1; clientsRemaining != 0; clientsRemaining--) {
      if (clientRecord->commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        if (notifyWaitingPeers != 0) {
          FrontendTransfer_ResendBatchOrWaitToClients();
        }
        return true;
      }
      clientRecord++;
    }
    clientRecord = g_FrontendPlayerRuntimeBlocks + 1;
    for (clientsRemaining = g_FrontendPlayerRuntimeBlockCount - 1; clientsRemaining != 0; clientsRemaining--) {
      clientRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      clientRecord++;
    }
  }
  g_UiTransferSenderContext++;
  InGameCommandQueue_DequeueFirstIntoRecord(g_FrontendClientPlayerCommandRecords);
  batchCount = 0;
  commandRecordCursor = g_FrontendClientPlayerCommandRecords;
  batchCursor = g_FrontendClientCommandBatchPacketBuffer;
  slotsRemaining = g_FrontendPlayerRuntimeBlockCount;
  /* Pack every non-empty 0x20-byte command slot (handler offset != 0) into the batch. */
  do {
    if ((commandRecordCursor->command.packedCommandAndPlayerId & 0xffffff00) != 0) {
      FrontendTransfer_CopyCommandRecord(batchCursor,commandRecordCursor);
      batchCursor++;
      batchCount++;
    }
    commandRecordCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  if (batchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    /* Nothing pending: send the first record (the host's, empty) as a batch of one. */
    FrontendTransfer_CopyCommandRecord(batchCursor,g_FrontendClientPlayerCommandRecords);
    batchCount = 1;
  }
  /* the first packed record's header doubles as the batch header */
  g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
       batchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_COMMAND_BATCH_TYPE;
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  for (peersRemaining = g_FrontendPlayerRuntimeBlockCount - 1; peersRemaining != 0; peersRemaining--) {
    UiTransfer_StagePacketAndSend
              (peerEndpointCursor,&g_FrontendClientCommandBatchPacketBuffer[0].header);
    peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
  return false;
}


/* Client side of the lockstep exchange: sends the host its next in-game command (FRONTEND_PACKET_COMMAND_SUBMIT)
   with the oldest queued command, or an empty record when none is queued. The sender context counts the
   submissions.
*/
void FrontendTransfer_SendCommandSubmit(void)

{
  g_FrontendPacket10021Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_COMMAND_SUBMIT;
  g_UiTransferSenderContext++;
  InGameCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10021Buffer);
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
  return;
}


/* Empties the receive side of the transfer mailbox (allocation, byte count, remaining bytes, retry ticks) so a
   new transfer can be received; the outgoing buffer is left alone. Consumers call it after taking a buffer.
*/
void UiTransferMailbox_ClearReceivedState(void)

{
  g_UiTransferMailbox.receivedAllocation = NULL;
  g_UiTransferMailbox.receivedByteCount = 0;
  g_UiTransferMailbox.receivedRemainingBytes = 0;
  g_UiTransferMailbox.receiveRetryTicks = 0;
  return;
}


/* Hands out a completely received transfer: returns its (non-NULL) buffer and stores its byte count in
   *outByteCount once an allocation exists and no bytes are outstanding. An empty, unavailable or still
   incomplete mailbox returns NULL and leaves *outByteCount untouched (the original returned nothing
   meaningful then; every caller reads the results only on success).
*/
void *UiTransferMailbox_GetReceivedBuffer(uint32_t *outByteCount)

{
  if (g_UiTransferMailbox.receivedAllocation != UI_TRANSFER_MAILBOX_UNAVAILABLE &&
      g_UiTransferMailbox.receivedAllocation != NULL &&
      g_UiTransferMailbox.receivedRemainingBytes == 0) {
    *outByteCount = g_UiTransferMailbox.receivedByteCount;
    return g_UiTransferMailbox.receivedAllocation;
  }
  return NULL;
}


/* Gives this machine a new random session identity before it opens or looks for a session: XORs a random
   16-bit value into the low word of the transfer sequence token. The high word stays (a host answers the
   discovery probe only for 0x1234).
*/
void UiTransferMailbox_RandomizeSequenceToken(void)

{
  uint32_t randomValue;
  
  randomValue = Random_NextPrimary();
  g_UiTransferSequenceToken = g_UiTransferSequenceToken ^ randomValue & 0xffff;
  return;
}


/* Session advertisement (0x50001): updates the known session (same sequence token and IPv4 address) in place,
   or appends it while the list has fewer than FRONTEND_SESSION_LIST_CAPACITY rows. */
static void FrontendTransfer_StoreSessionAdvertisement
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *sessionList)
{
  int sessionsRemaining;
  int dwordCount;
  FrontendSessionDiscoveryRecord *discoveryRecord;
  FrontendSessionDiscoveryRecord **sessionRowCursor;
  uint32_t *recordDwordCursor;
  const uint32_t *sourceDwords;

  discoveryRecord = g_FrontendSessionDiscoveryRecords;
  sessionRowCursor = g_FrontendSessionListRows;
  for (sessionsRemaining = (int)sessionList->rowCount; sessionsRemaining != 0; sessionsRemaining--) {
    if ((packet->packet10000Handshake.header.sequenceToken == discoveryRecord->advertisement.header.sequenceToken) &&
        (senderEndpoint->ipv4AddressNetworkOrder == discoveryRecord->senderEndpoint.ipv4AddressNetworkOrder)) {
      break;
    }
    sessionRowCursor++;
    discoveryRecord++;
  }
  if ((sessionsRemaining == 0) && (sessionList->rowCount >= FRONTEND_SESSION_LIST_CAPACITY)) {
    return; /* unknown session and the list is full */
  }
  if (sessionsRemaining == 0) {
    /* append: the row after the last one */
    *sessionRowCursor = discoveryRecord;
    sessionList->rowCount = sessionList->rowCount + 1;
  }
  /* the payload byte count is overwritten with 0x20 before the packet is stored */
  packet->packet50001SessionAdvertisement.payloadByteCount = 32;
  /* the 0xA0-byte advertisement followed by the sender's 0x10-byte endpoint */
  recordDwordCursor = (uint32_t *)discoveryRecord;
  sourceDwords = (const uint32_t *)packet;
  for (dwordCount = 40; dwordCount != 0; dwordCount--) {
    *recordDwordCursor = *sourceDwords;
    sourceDwords++;
    recordDwordCursor++;
  }
  sourceDwords = (const uint32_t *)senderEndpoint;
  for (dwordCount = 4; dwordCount != 0; dwordCount--) {
    *recordDwordCursor = *sourceDwords;
    sourceDwords++;
    recordDwordCursor++;
  }
  UiPointerList_RefreshSelectionAndQueueAction(sessionList);
}

/* Network game page (browsing): a session advertisement (0x50001) updates its row in the session list or
   appends one (at most 0x20 sessions); the join ack (0x10003) from the selected host takes over the assigned
   player id and network tick interval, switches to the host-lobby page and FRONTEND_NETWORK_STATE_JOINED and
   marks this machine as a network client.
*/
void FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int dwordCount;
  uint32_t *playerRowCursor;

  if (packet->packet10000Handshake.header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT) {
    FrontendTransfer_StoreSessionAdvertisement
              (senderEndpoint,packet,(UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList));
  }
  else if ((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10003_JOIN_ACK) &&
           (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken) &&
           (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder == senderEndpoint->ipv4AddressNetworkOrder)) {
    g_LocalPlayerRuntimeId = packet->packet10003JoinAck.assignedPlayerRuntimeId;
    g_SessionNetworkTickInterval = packet->packet10003JoinAck.networkTickInterval;
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_CLIENT_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendRuntime,frontendPageStack));
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_JOINED;
    g_SessionTransferTimeoutTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_CLIENT;
    playerRowCursor = (uint32_t *)g_FrontendPlayerListRows[0];
    for (dwordCount = 256; dwordCount != 0; dwordCount--) {
      *playerRowCursor = 0;
      playerRowCursor++;
    }
    UiPointerList_InitializeColumnLayout
              (0,(void **)g_FrontendPlayerListRows,
               (UiPointerListControl *)FRONTEND_UI(frontendRuntime,clientLobbyPlayerList));
  }
}


/* Host timeout of a client in the host's lobby, called by FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState
   while g_FrontendNetworkState is FRONTEND_NETWORK_STATE_JOINED: when g_SessionTransferTimeoutTicks runs out
   (nothing heard from the host), the client leaves as if its lobby Leave button had been pressed and goes
   back to the session list.
*/
void FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRoot)

{
  g_SessionTransferTimeoutTicks--;
  if (g_SessionTransferTimeoutTicks == 0) {
    FrontendTransferPage_ResetSessionOpenAndRequestMailbox(FRONTEND_UI(frontendRoot,clientLobbyLeaveButton));
  }
  return;
}


/* Frontend copy of FrontendTransfer_ConsumeProcessedFlag: atomically takes and clears
   g_FrontendTransferResponsePending (set by FrontendTransfer_HandleGameplayCommandAndRosterPackets after a new
   command batch). Returns true when no batch arrived, so Frontend_StateTick ends its tick early.
*/
Bool8 FrontendTransfer_ConsumeProcessedFlagForMenuTick(void)

{
  int previousFlag;

  /* atomic exchange: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}


/* Host side of the in-game command exchange: finds the player the packet came from (sequence token and
   IPv4 address) and refreshes its timeout. A COMMAND_SUBMIT with a new sender context is stored in that
   player's command slot and marks the player ready for the next batch; a repeated one (retransmit) and a
   COMMAND_WAIT_ACK only refresh the timeout.
*/
void FrontendTransfer_HostHandleCommandSubmitOrWaitAck
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSequenceToken senderSequenceToken;
  UiTransferSenderContext packetSenderContext;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int dwordCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;
  uint32_t *copySource;
  uint32_t *copyDestination;

  senderSequenceToken =packet->packet10000Handshake.header.sequenceToken;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_COMMAND_SUBMIT) {
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if (packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_COMMAND_WAIT_ACK) {
      return;
    }
    while ((senderSequenceToken != playerRecord->peerSequenceToken ||
           (sourceContext->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)))
    {
      playersRemaining--;
      playerRecord++;
      if (playersRemaining == 0) {
        return;
      }
    }
    playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    return;
  }
  /* the command slots run parallel to the player records */
  commandRecord = g_FrontendClientPlayerCommandRecords;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((senderSequenceToken != playerRecord->peerSequenceToken ||
         (sourceContext->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder))) {
    commandRecord++;
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  packetSenderContext = packet->packet10000Handshake.header.senderContext;
  playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
  /* the client bumps its sender context per new command; an equal one is a retransmit */
  if (packetSenderContext != commandRecord->header.senderContext) {
    playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    /* copy the whole 0x20-byte packet, header included, into the slot */
    copySource = (uint32_t *)packet;
    copyDestination = (uint32_t *)commandRecord;
    for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
      *copyDestination = *copySource;
      copySource++;
      copyDestination++;
    }
  }
  return;
}


/* Host side: executes the command batch it has just broadcast (g_FrontendClientCommandBatchPacketBuffer) on
   the local simulation, so host and clients run the same commands in the same tick. The high 24 bits of
   each packed command are the handler's offset from InGameCommandQueue_AppendLocalPlayerCommand, the low
   8 bits the player id; offsets beyond the handler code region are ignored. The original handler address
   is resolved to its C function by CommandDispatch_ResolveHandler.
*/
void FrontendTransfer_DispatchStagedCommandRecords(void)

{
  uint32_t packedCommand;
  uint32_t commandHandlerIndex;
  uint32_t remainingCount;
  FrontendCommandPacketRecord *commandRecord;

  commandRecord = g_FrontendClientCommandBatchPacketBuffer;
  for (remainingCount = g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount >>
                        FRONTEND_PACKET_UNIT_COUNT_SHIFT;
      remainingCount != 0; remainingCount--) {
    packedCommand = commandRecord->command.packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      CommandQueueHandlerProc *commandHandler =
           CommandDispatch_ResolveHandler
                     (INGAME_COMMAND_CODE_BASE,INGAME_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
      if (commandHandler != NULL)
      {
        (*commandHandler)
                  (packedCommand & 0xff,commandRecord->command.payload1,commandRecord->command.payload2,
                   commandRecord->command.payload3);
      }
    }
    commandRecord++;
  }
  return;
}


/* Client side: atomically takes and clears g_FrontendTransferResponsePending, which
   FrontendNetwork_HandleCommandBatchAndPlayerTimeout sets after executing a new command batch. Returns true
   when no batch arrived, so the in-game tick waits for the host instead of advancing the simulation.
*/
Bool8 FrontendTransfer_ConsumeProcessedFlag(void)

{
  int previousFlag;

  /* atomic exchange: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}


/* Encrypts an outgoing packet: byteCount/8 64-bit blocks in CBC mode (each input block is XORed with the
   previous output block, starting from zero), each through 16 rounds keyed by roundKeys16 and the eight
   nibble substitution tables g_UiTransferEncryptSboxes. UiTransfer_DecryptPacketBlocks is the
   matching decryption used on receive.
*/
void UiTransfer_EncryptPacketBlocks(const uint32_t *roundKeys16,uint32_t *outputBlocks,UiTransferPayloadByteCount byteCount,
          uint32_t *inputBlocks)

{
  const uint32_t *roundKeyNibble0;
  const uint32_t *roundKeyNibble1;
  const uint32_t *roundKeyNibble2;
  const uint32_t *roundKeyNibble3;
  const uint32_t *roundKeyNibble4;
  const uint32_t *roundKeyNibble5;
  const uint32_t *roundKeyNibble6;
  const uint32_t *roundKeyNibble7;
  uint32_t roundInputHalf;
  uint32_t roundIndex;
  uint32_t blocksRemaining;
  uint32_t leftState;
  uint32_t rightState;
  
  blocksRemaining = byteCount >> 3;
  if (blocksRemaining != 0) {
    rightState = 0;
    leftState = 0;
    do {
      roundIndex = 0;
      roundInputHalf = *inputBlocks ^ leftState;
      rightState = inputBlocks[1] ^ rightState;
      do {
        leftState = rightState;
        roundKeyNibble0 = roundKeys16 + roundIndex;
        roundKeyNibble1 = roundKeys16 + roundIndex;
        roundKeyNibble2 = roundKeys16 + roundIndex;
        roundKeyNibble3 = roundKeys16 + roundIndex;
        roundKeyNibble4 = roundKeys16 + roundIndex;
        roundKeyNibble5 = roundKeys16 + roundIndex;
        roundKeyNibble6 = roundKeys16 + roundIndex;
        roundKeyNibble7 = roundKeys16 + roundIndex;
        roundIndex++;
        /* one nibble per table: table n, row = key nibble n, column = input nibble n */
        /* byte offsets into the uint32_t[16][16] tables: row = key nibble * 64, column = input nibble * 4 */
        rightState =(((((((*(int *)((uint8_t *)g_UiTransferEncryptSboxes[0] +
                                    (roundInputHalf & 0xf) * 4 + (*roundKeyNibble0 & 0xf) * UI_TRANSFER_CIPHER_ROW_BYTES) << 4 |
                           *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[1] +
                                    ((roundInputHalf & 0xf0) >> 4) * 4 + (*roundKeyNibble1 & 0xf0) * 4)) << 4
                          | *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[2] +
                                     ((roundInputHalf & 0xf00) >> 8) * 4 + ((*roundKeyNibble2 & 0xf00) >> 2))
                          ) << 4 | *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[3] +
                                            ((roundInputHalf & 0xf000) >> 12) * 4 +
                                            ((*roundKeyNibble3 & 0xf000) >> 6))) << 4 |
                        *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[4] +
                                 ((roundInputHalf & 0xf0000) >> 16) * 4 +
                                 ((*roundKeyNibble4 & 0xf0000) >> 10))) << 4 |
                       *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[5] +
                                ((roundInputHalf & 0xf00000) >> 20) * 4 +
                                ((*roundKeyNibble5 & 0xf00000) >> 14))) << 4 |
                      *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[6] +
                               ((roundInputHalf & 0xf000000) >> 24) * 4 +
                               ((*roundKeyNibble6 & 0xf000000) >> 18))) << 4 |
                     *(uint32_t *)((uint8_t *)g_UiTransferEncryptSboxes[7] +
                              (roundInputHalf >> 28) * 4 + ((*roundKeyNibble7 & 0xf0000000) >> 22))) ^
                     leftState;
        roundInputHalf = leftState;
      } while (roundIndex < 16);
      *outputBlocks = leftState;
      outputBlocks[1] = rightState;
      inputBlocks = inputBlocks + 2;
      outputBlocks = outputBlocks + 2;
      blocksRemaining--;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Decrypts a received packet in place or into destination (they may alias): the inverse of
   UiTransfer_EncryptPacketBlocks, running the 16 rounds backwards with the second table set
   (g_UiTransferDecryptSboxes) and XORing each result with the previous ciphertext block (CBC).
*/
void UiTransfer_DecryptPacketBlocks
          (const uint32_t *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,void *source)

{
  uint32_t leftHalf;
  int roundIndex;
  uint32_t rightHalf;
  uint32_t savedHalf;
  uint32_t blocksRemaining;
  uint32_t previousCipherLow;
  uint32_t previousCipherHigh;
  
  blocksRemaining = byteCount >> 3;
  if (blocksRemaining != 0) {
    previousCipherHigh = 0;
    previousCipherLow = 0;
    do {
      roundIndex = 15;
      leftHalf = *(uint32_t *)source;
      rightHalf = ((uint32_t *)source)[1];
      do {
        savedHalf = leftHalf;
        rightHalf = rightHalf ^ savedHalf;
        leftHalf = ((((((*(int *)(((roundKeys16[roundIndex] & 0xf0000000) >> 22) + THANDOR_ADDR(g_UiTransferDecryptSboxes,7 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                              (rightHalf & 0xf) * 4) << 4 |
                     *(uint32_t *)(((roundKeys16[roundIndex] & 0xf000000) >> 18) + THANDOR_ADDR(g_UiTransferDecryptSboxes,6 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                              ((rightHalf & 0xf0) >> 4) * 4)) << 4 |
                    *(uint32_t *)(((roundKeys16[roundIndex] & 0xf00000) >> 14) + THANDOR_ADDR(g_UiTransferDecryptSboxes,5 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                             ((rightHalf & 0xf00) >> 8) * 4)) << 4 |
                   *(uint32_t *)(((roundKeys16[roundIndex] & 0xf0000) >> 10) + THANDOR_ADDR(g_UiTransferDecryptSboxes,4 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                            ((rightHalf & 0xf000) >> 12) * 4)) << 4 |
                  *(uint32_t *)(((roundKeys16[roundIndex] & 0xf000) >> 6) + THANDOR_ADDR(g_UiTransferDecryptSboxes,3 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                           ((rightHalf & 0xf0000) >> 16) * 4)) << 4 |
                 *(uint32_t *)(((roundKeys16[roundIndex] & 0xf00) >> 2) + THANDOR_ADDR(g_UiTransferDecryptSboxes,2 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                          ((rightHalf & 0xf00000) >> 20) * 4)) << 4 |
                *(uint32_t *)((roundKeys16[roundIndex] & 0xf0) * 4 + THANDOR_ADDR(g_UiTransferDecryptSboxes,1 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                         ((rightHalf & 0xf000000) >> 24) * 4)) << 4 |
                *(uint32_t *)((roundKeys16[roundIndex] & 0xf) * UI_TRANSFER_CIPHER_ROW_BYTES + THANDOR_ADDR(g_UiTransferDecryptSboxes,0) + (rightHalf >> 28) * 4);
        roundIndex--;
        rightHalf = savedHalf;
      } while (-1 < roundIndex);
      leftHalf = leftHalf ^ previousCipherLow;
      savedHalf = savedHalf ^ previousCipherHigh;
      previousCipherLow = *(uint32_t *)source;
      previousCipherHigh = ((uint32_t *)source)[1];
      *(uint32_t *)destination = leftHalf;
      ((uint32_t *)destination)[1] = savedHalf;
      source = (uint8_t *)source + 8;
      destination = (uint8_t *)destination + 8;
      blocksRemaining--;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Marks the receive side as unavailable: publishes the UI_TRANSFER_MAILBOX_UNAVAILABLE sentinel and sets the
   byte count, remaining bytes and retry ticks to one, so the mailbox is neither empty nor receivable.
*/
void UiTransferMailbox_MarkUnavailable(void)

{
  g_UiTransferMailbox.receivedAllocation = UI_TRANSFER_MAILBOX_UNAVAILABLE;
  g_UiTransferMailbox.receivedByteCount = 1;
  g_UiTransferMailbox.receivedRemainingBytes = 1;
  g_UiTransferMailbox.receiveRetryTicks = 1;
  return;
}


/* Publishes the buffer the next outgoing transfer sends (NULL/0 withdraws it). The allocation is later
   released through g_MemoryApi.free by the frontend transfer consumers.
*/
void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation)

{
  g_UiTransferMailbox.outgoingAllocation = allocation;
  g_UiTransferMailbox.outgoingByteCount = byteCount;
  return;
}


/* Client answer to the host while the session starts and after each lobby command batch: sends the oldest
   queued lobby command in packet 0x10011 (a new sender sequence number each time) and, while player snapshots
   are still missing, requests the next one with packet 0x10004.
*/
void FrontendTransfer_SendLobbyCommandAndSnapshotRequest(void)

{
  FrontendPlayerRuntimeBlockCount nextPlayerIndex;
  
  g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011_LOBBY_COMMAND;
  g_UiTransferSenderContext++;
  FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
  /* the snapshots received so far, read before sending like the original */
  nextPlayerIndex = g_FrontendPlayerRuntimeBlockCount;
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
  if (nextPlayerIndex < g_FrontendExpectedPlayerRuntimeBlockCount) {
    g_FrontendPacket10004Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_10004_SNAPSHOT_REQUEST;
    g_FrontendPacket10004Buffer.requestedPlayerIndex = nextPlayerIndex;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10004Buffer.header);
  }
  return;
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

