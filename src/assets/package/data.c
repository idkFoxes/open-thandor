/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/package/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 004080C0 g_PckHuffmanSymbolWorkspace256 */
__declspec(align(16)) PckHuffmanSymbolState g_PckHuffmanSymbolWorkspace256[256] = {0};

/* 004084C0 g_PckHuffmanNodeWorkspace: [0..255] leaf nodes (004084C0), [256..511] internal nodes (004094C0) */
__declspec(align(16)) PckHuffmanNode g_PckHuffmanNodeWorkspace[512] = {0};

/* 0040ADC0 g_FileSystemCombinedPathScratchUtf16 */
__declspec(align(16)) uint16_t g_FileSystemCombinedPathScratchUtf16[256] = {0};

/* 0040B1C0 g_FileSystemOpen */
__declspec(align(16)) FileSystemOpenProc *g_FileSystemOpen = 0;

/* 0040B1C4 g_FileSystemClose */
__declspec(align(4)) FileSystemCloseProc *g_FileSystemClose = 0;

/* 0040B1C8 g_FileSystemReadExact */
__declspec(align(8)) FileSystemReadExactProc *g_FileSystemReadExact = 0;

/* 0040B1CC g_FileSystemWriteExactOrFlush */
__declspec(align(4)) FileSystemWriteExactOrFlushProc *g_FileSystemWriteExactOrFlush = 0;

/* 0040B1D0 g_FileSystemGetSize */
__declspec(align(16)) FileSystemGetSizeProc *g_FileSystemGetSize = 0;

/* 0040B1D8 g_FileSystemSeek */
__declspec(align(8)) FileSystemSeekProc *g_FileSystemSeek = 0;

/* 0040B21C g_PackageScratchBuffer */
__declspec(align(4)) uint8_t *g_PackageScratchBuffer = 0;

/* 0040B224 g_PackageMountSlots */
__declspec(align(4)) PckMountSlot g_PackageMountSlots[1024] = {0};

/* 0040E224 g_PckEncoderTable */
__declspec(align(4)) PckCodecProc *g_PckEncoderTable[3] = {(void *)PckCodec_EncodeHuffmanRle, (void *)PckCodec_EncodeStored, (void *)PckCodec_EncodeFieldGrid};

/* 0040E230 g_PckDecoderTable */
__declspec(align(16)) PckCodecProc *g_PckDecoderTable[3] = {(void *)PckCodec_DecodeHuffmanRle, (void *)PckCodec_DecodeStored, (void *)PckCodec_DecodeFieldGrid};

/* 0050DFB4 u_army_hex_0050dfb4 */
__declspec(align(4)) uint16_t u_army_hex_0050dfb4[9] = L"army.hex";

/* 00545EA6 g_LevelPackageFoundEntry: LevelPackage_ValidateAndMount's one-entry Package_FindEntry output buffer */
__declspec(align(4)) PckEntryHeader g_LevelPackageFoundEntry = {0};

/* 005460A6 u_level___lev_005460a6 */
__declspec(align(4)) uint16_t u_level___lev_005460a6[12] = L"level\\*.lev";

/* 005460BE u_level___str_005460be */
__declspec(align(4)) uint16_t u_level___str_005460be[12] = L"level\\*.str";
