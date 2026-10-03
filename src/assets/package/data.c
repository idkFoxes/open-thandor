/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/package/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/package/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) PckHuffmanSymbolState g_PckHuffmanSymbolWorkspace256[256] = {0};

/* [0..255] leaf nodes (004084C0), [256..511] internal nodes (004094C0) */
__declspec(align(16)) PckHuffmanNode g_PckHuffmanNodeWorkspace[512] = {0};

__declspec(align(16)) uint16_t g_FileSystemCombinedPathScratchUtf16[256] = {0};

__declspec(align(16)) FileSystemOpenProc *g_FileSystemOpen = 0;

__declspec(align(4)) FileSystemCloseProc *g_FileSystemClose = 0;

__declspec(align(8)) FileSystemReadExactProc *g_FileSystemReadExact = 0;

__declspec(align(4)) FileSystemWriteExactOrFlushProc *g_FileSystemWriteExactOrFlush = 0;

__declspec(align(16)) FileSystemGetSizeProc *g_FileSystemGetSize = 0;

__declspec(align(8)) FileSystemSeekProc *g_FileSystemSeek = 0;

__declspec(align(4)) uint8_t *g_PackageScratchBuffer = 0;

__declspec(align(4)) PckMountSlot g_PackageMountSlots[1024] = {0};

__declspec(align(4)) PckCodecProc *g_PckEncoderTable[3] = {(void *)PckCodec_EncodeHuffmanRle, (void *)PckCodec_EncodeStored, (void *)PckCodec_EncodeFieldGrid};

__declspec(align(16)) PckCodecProc *g_PckDecoderTable[3] = {(void *)PckCodec_DecodeHuffmanRle, (void *)PckCodec_DecodeStored, (void *)PckCodec_DecodeFieldGrid};

__declspec(align(4)) uint16_t u_army_hex_0050dfb4[9] = L"army.hex";

/* LevelPackage_ValidateAndMount's one-entry Package_FindEntry output buffer */
__declspec(align(4)) PckEntryHeader g_LevelPackageFoundEntry = {0};

__declspec(align(4)) uint16_t u_level___lev_005460a6[12] = L"level\\*.lev";

__declspec(align(4)) uint16_t u_level___str_005460be[12] = L"level\\*.str";
