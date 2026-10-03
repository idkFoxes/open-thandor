/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/original_code.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_ORIGINAL_CODE_H
#define THANDOR_PLATFORM_DEBUG_ORIGINAL_CODE_H

/* Developer tools (THANDOR_DEV_TOOLS): differential tests run copies of original machine code. */

/* An executable copy of the original code bytes [address, address + size) (original virtual addresses),
   read from thandor_original.exe next to the executable; NULL when the file or the range is missing.
   Position-independent functions only. */
void *Thandor_LoadOriginalCodeCopy(unsigned address, unsigned size);

#endif /* THANDOR_PLATFORM_DEBUG_ORIGINAL_CODE_H */
