/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/low_memory.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_LOW_MEMORY_H
#define THANDOR_PLATFORM_BOOTSTRAP_LOW_MEMORY_H

/*
Memory below 2 GB for a large-address-aware build (CMake option THANDOR_LARGE_ADDRESS_AWARE).

The original layouts keep pointers in 32-bit fields (core/ptr32.h), so everything the game points to from such a
field must lie below 2 GB. The default build gets that from /LARGEADDRESSAWARE:NO, which limits the whole process
(also the graphics driver) to 2 GB. The large-address-aware build gives the driver the full 64-bit address space and
keeps the game's own memory low instead: at the first allocation it reserves two pools below 2 GB, a fixed Win32
heap for the small blocks and a page pool for the large ones, and every C++ allocation (the replaced operator
new/delete), every SDL allocation (SDL_SetMemoryFunctions, LowMemory_InstallSdlAllocator) and the arena
(ArenaHeap_Init) come from them. The image is at its fixed base 0x10000000 and the main thread's stack is reserved
at process start, both below 2 GB. When the pools are full an allocation falls back to the process heap (logged);
a pointer of 2 GB or more that reaches a 32-bit field still stops the game (Thandor_Ptr32Overflow).
*/

#include <cstddef>

#ifdef THANDOR_LARGE_ADDRESS_AWARE

/* A block below 2 GB (nullptr when nothing is left at all), 16-byte aligned; zeroed when zero is true. */
void *LowMemory_Alloc(size_t bytes, bool zero) noexcept;
/* Frees a block of LowMemory_Alloc (nullptr does nothing). */
void LowMemory_Free(void *block) noexcept;
/* Routes SDL's allocations to LowMemory (before any other SDL call). */
void LowMemory_InstallSdlAllocator() noexcept;
/* Logs the pools (ranges, use, peak, fallbacks) with "when" naming the moment. */
void LowMemory_LogState(const char *when) noexcept;

#endif

#endif /* THANDOR_PLATFORM_BOOTSTRAP_LOW_MEMORY_H */
