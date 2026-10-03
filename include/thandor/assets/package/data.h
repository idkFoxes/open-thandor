/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/data.h
 */

#ifndef THANDOR_ASSETS_PACKAGE_DATA_H
#define THANDOR_ASSETS_PACKAGE_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern PckHuffmanSymbolState g_PckHuffmanSymbolWorkspace256[256]; /* 004080C0 g_PckHuffmanSymbolWorkspace256 */

/* 004084C0 Huffman node workspace (original layout): [0..255] leaf nodes (index = byte symbol, 004084C0),
   [256..511] internal nodes (004094C0); the tree-building scans run over both halves as one array. */
extern PckHuffmanNode g_PckHuffmanNodeWorkspace[512];

extern uint16_t g_FileSystemCombinedPathScratchUtf16[256]; /* 0040ADC0 g_FileSystemCombinedPathScratchUtf16 */

extern FileSystemOpenProc *g_FileSystemOpen; /* 0040B1C0 g_FileSystemOpen */

extern FileSystemCloseProc *g_FileSystemClose; /* 0040B1C4 g_FileSystemClose */

extern FileSystemReadExactProc *g_FileSystemReadExact; /* 0040B1C8 g_FileSystemReadExact */

extern FileSystemWriteExactOrFlushProc *g_FileSystemWriteExactOrFlush; /* 0040B1CC g_FileSystemWriteExactOrFlush */

extern FileSystemGetSizeProc *g_FileSystemGetSize; /* 0040B1D0 g_FileSystemGetSize */

extern FileSystemSeekProc *g_FileSystemSeek; /* 0040B1D8 g_FileSystemSeek */

extern uint8_t *g_PackageScratchBuffer; /* 0040B21C g_PackageScratchBuffer */

extern PckMountSlot g_PackageMountSlots[1024]; /* 0040B224 g_PackageMountSlots */

extern PckCodecProc *g_PckEncoderTable[3]; /* 0040E224 g_PckEncoderTable */

extern PckCodecProc *g_PckDecoderTable[3]; /* 0040E230 g_PckDecoderTable */

extern uint16_t u_army_hex_0050dfb4[9]; /* 0050DFB4 u_army_hex_0050dfb4 */

extern PckEntryHeader g_LevelPackageFoundEntry; /* 00545EA6 g_LevelPackageFoundEntry: one-entry output buffer (PCK_ENTRY_HEADER_BYTES) of Package_FindEntry for LevelPackage_ValidateAndMount (original: s_NAME__CLIENT__KARTE___00545e91 + 0x15) */

extern uint16_t u_level___lev_005460a6[12]; /* 005460A6 u_level___lev_005460a6 */

extern uint16_t u_level___str_005460be[12]; /* 005460BE u_level___str_005460be */

#endif
