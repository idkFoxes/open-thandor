/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/codec/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/audio/codec/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) uint64_t g_SoundDecodeMmxWordLaneMask0 = 0xFFFFull;

__declspec(align(8)) uint64_t g_SoundDecodeMmxWordLaneMask1 = 0xFFFF0000ull;

__declspec(align(16)) uint64_t g_SoundDecodeMmxWordLaneMask2 = 0xFFFF00000000ull;

__declspec(align(8)) uint64_t g_SoundDecodeMmxWordLaneMask3 = 0xFFFF000000000000ull;

__declspec(align(16)) short *g_CosineDerivedLookupAllocation = 0;

__declspec(align(4)) short *g_CosineDerivedLookupSecondTable = 0;
