/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/data.h
 */

#ifndef THANDOR_ASSETS_PACKAGE_DATA_H
#define THANDOR_ASSETS_PACKAGE_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern PckHuffmanSymbolState g_PckHuffmanSymbolWorkspace256[256];

/* Huffman node workspace (original layout): [0..255] leaf nodes (index = byte symbol, 004084C0),
   [256..511] internal nodes (004094C0); the tree-building scans run over both halves as one array. */
extern PckHuffmanNode g_PckHuffmanNodeWorkspace[512];

extern uint16_t g_FileSystemCombinedPathScratchUtf16[256];

extern FileSystemOpenProc *g_FileSystemOpen;

extern FileSystemCloseProc *g_FileSystemClose;

extern FileSystemReadExactProc *g_FileSystemReadExact;

extern FileSystemWriteExactOrFlushProc *g_FileSystemWriteExactOrFlush;

extern FileSystemGetSizeProc *g_FileSystemGetSize;

extern FileSystemSeekProc *g_FileSystemSeek;

extern uint8_t *g_PackageScratchBuffer;

extern PckMountSlot g_PackageMountSlots[1024];

extern PckCodecProc *g_PckEncoderTable[3];

extern PckCodecProc *g_PckDecoderTable[3];

extern uint16_t u_army_hex_0050dfb4[9];

extern PckEntryHeader g_LevelPackageFoundEntry; /* one-entry output buffer (PCK_ENTRY_HEADER_BYTES) of Package_FindEntry for LevelPackage_ValidateAndMount (original: s_NAME__CLIENT__KARTE___00545e91 + 0x15) */

extern uint16_t u_level___lev_005460a6[12];

extern uint16_t u_level___str_005460be[12];

#endif
