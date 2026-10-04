/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/text/path.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_TEXT_PATH_H
#define THANDOR_CORE_TEXT_PATH_H

#include <thandor/core/text/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/text/path. */
/* Engine path buffers hold at most 256 UTF-16 code units (0x200 bytes) including the terminator, e.g.
   g_FileSystemCombinedPathScratchUtf16 and g_ExecutableDirectoryUtf16. */
#define WIDE_PATH_MAX_CODE_UNITS 0x100
/* open-thandor: capacity in UTF-16 code units of the runtime-only path buffers that receive a
   WidePath_CombineDirectoryAndLeaf result (g_FileSystemCombinedPathScratchUtf16, g_ScenarioCatalogPathScratchUtf16,
   g_LevelResourcePathScratchUtf16, ...). The combine scans directory and leaf for up to WIDE_PATH_MAX_CODE_UNITS units
   each, so it writes up to 2 * WIDE_PATH_MAX_CODE_UNITS units; the loaders also combine the executable directory with
   paths that already start with it (an absolute save or level path). In the original these buffers had
   WIDE_PATH_MAX_CODE_UNITS units, and a game directory of about 120 characters or more made such a combined
   path run past them into the file-system function table. Also the size of the narrow (8-bit) path scratch of
   platform/filesystem. Not part of any file, savegame or packet layout. */
#define THANDOR_PATH_CAPACITY 0x400
/* WidePath_ParseTrailingNumberBeforeExtension: code units scanned for the terminator and the digits */
#define WIDE_PATH_NUMBER_SCAN_MAX_UNITS 32
/* Extension codes for WidePath_SetExtensionCode: the three lower-case letters packed little-endian (the same
   packing as the ASSET_MAGIC_* values). */
#ifndef WIDE_PATH_EXTENSION_FLD
#define WIDE_PATH_EXTENSION_FLD 0x646C66 /* ".fld" field grid */
#define WIDE_PATH_EXTENSION_LEV 0x76656C /* ".lev" level */
#define WIDE_PATH_EXTENSION_CGN 0x6E6763 /* ".cgn" campaign */
#define WIDE_PATH_EXTENSION_SVE 0x657673 /* ".sve" saved game */
#endif
/* Utf16DecimalDigitPair4.packedDigits of the numbered path templates (patchNN.pck, levelNN.pck, ...):
   codeUnits[0] (low half) is the tens digit, codeUnits[1] the ones digit, e.g. UTF16_DIGIT_PAIR('0','0') is
   "00" = 0x300030. Adding UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP (0x9FFFF) takes one from the tens digit and adds
   ten to the ones digit (0xFFFF is -1 on the low half plus a carry, 9 + 1 on the high half), so a ones digit
   of '0' - 1 becomes '9'. */
#define UTF16_DIGIT_PAIR(tens,ones) ((ones) << 16 | (tens))
#define UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP ((10 << 16) - 1)
/* Two UTF-16 code units read as one little-endian dword (first in the low half), e.g. for comparing a name */
#define UTF16_CHAR_PAIR(first,second) ((second) << 16 | (first))

/* Functions are grouped by semantic ownership. */

/* pathCapacity: code units of the buffer holding path */
Bool8 WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,uint16_t *path,
                                size_t pathCapacity = WIDE_PATH_MAX_CODE_UNITS);

Bool8 WidePath_SplitParentAndLeaf(uint16_t *leafOut,uint16_t *parentOut,uint16_t *path);

/* destinationCapacity: code units of the destination buffer */
void WidePath_CombineDirectoryAndLeafBounded
          (uint16_t *destination,size_t destinationCapacity,uint16_t *leaf,uint16_t *directory);

/* Combines into a destination array; its size bounds the result (see WidePath_CombineDirectoryAndLeafBounded). */
template <size_t DestinationCapacity>
inline void WidePath_CombineDirectoryAndLeaf
          (uint16_t (&destination)[DestinationCapacity],uint16_t *leaf,uint16_t *directory)
{
  WidePath_CombineDirectoryAndLeafBounded(destination,DestinationCapacity,leaf,directory);
}

uint32_t WidePath_ParseTrailingNumberBeforeExtension(uint16_t *path);

#endif /* THANDOR_CORE_TEXT_PATH_H */
