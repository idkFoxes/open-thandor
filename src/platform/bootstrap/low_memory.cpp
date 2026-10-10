/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/low_memory.cpp
 * Project code (not in the original game)
 */

/* Memory below 2 GB for the large-address-aware build, see <thandor/platform/bootstrap/low_memory.h>. Compiled
   only with THANDOR_LARGE_ADDRESS_AWARE. */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <thandor/platform/bootstrap/low_memory.h>
#include <thandor/platform/bootstrap/image.h>

#include <SDL3/SDL_stdinc.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <new>

namespace {

constexpr uintptr_t k2G = 0x80000000u;
constexpr size_t kMiB = size_t{1} << 20;
constexpr size_t kGranule = 64 * 1024;          /* the large pool's unit (the allocation granularity) */
constexpr size_t kSmallLimit = 256 * 1024;      /* larger blocks come from the large pool (a fixed heap's limit) */
constexpr size_t kMaxLargePoolBytes = 1024 * kMiB;
constexpr size_t kMaxGranules = kMaxLargePoolBytes / kGranule;
/* sizes tried for the pools, largest first, until one fits below 2 GB */
constexpr size_t kLargePoolSizes[] = {1024 * kMiB, 768 * kMiB, 512 * kMiB, 384 * kMiB, 256 * kMiB, 128 * kMiB};
constexpr size_t kSmallHeapSizes[] = {384 * kMiB, 256 * kMiB, 192 * kMiB, 128 * kMiB, 64 * kMiB};

struct LowPools {
  INIT_ONCE once = INIT_ONCE_STATIC_INIT;
  SRWLOCK lock = SRWLOCK_INIT; /* the large pool's bookkeeping */
  HANDLE smallHeap = nullptr;
  uintptr_t smallBase = 0;
  uintptr_t smallEnd = 0;
  uintptr_t largeBase = 0;
  uintptr_t largeEnd = 0;
  uint32_t granules = 0;
  uint32_t searchFrom = 0; /* no free granule below it */
  uint64_t used[kMaxGranules / 64] = {};
  uint32_t runLength[kMaxGranules] = {}; /* granules of the block that starts there */
  size_t largeInUse = 0;
  size_t largePeak = 0;
  volatile LONG64 smallInUse = 0;
  volatile LONG64 smallPeak = 0;
  volatile LONG64 fallbackCount = 0;
  volatile LONG64 fallbackHighCount = 0;
  volatile LONG fallbackLogs = 0;
};
constinit LowPools s_pools; /* constant-initialized: operator new may run before any dynamic initializer */

bool Fits(uintptr_t base, size_t bytes)
{
  return (base != 0) && (base + bytes <= k2G);
}

/* The extent of the reservation that contains address (the regions sharing its allocation base). */
uintptr_t ReservationEnd(uintptr_t address)
{
  MEMORY_BASIC_INFORMATION region;
  if (VirtualQuery(reinterpret_cast<void *>(address), &region, sizeof region) == 0) {
    return address;
  }
  void *const allocationBase = region.AllocationBase;
  uintptr_t end = reinterpret_cast<uintptr_t>(region.BaseAddress) + region.RegionSize;
  while ((VirtualQuery(reinterpret_cast<void *>(end), &region, sizeof region) != 0) &&
         (region.AllocationBase == allocationBase) && (region.State != MEM_FREE)) {
    end = reinterpret_cast<uintptr_t>(region.BaseAddress) + region.RegionSize;
  }
  return end;
}

BOOL CALLBACK InitPools(PINIT_ONCE, PVOID, PVOID *)
{
  for (size_t size : kLargePoolSizes) {
    void *base = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
    if ((base != nullptr) && Fits(reinterpret_cast<uintptr_t>(base), size)) {
      s_pools.largeBase = reinterpret_cast<uintptr_t>(base);
      s_pools.largeEnd = s_pools.largeBase + size;
      s_pools.granules = static_cast<uint32_t>(size / kGranule);
      break;
    }
    if (base != nullptr) {
      VirtualFree(base, 0, MEM_RELEASE);
    }
  }
  for (size_t size : kSmallHeapSizes) {
    HANDLE heap = HeapCreate(0, kMiB, size);
    if (heap == nullptr) {
      continue;
    }
    const auto base = reinterpret_cast<uintptr_t>(heap);
    const uintptr_t end = ReservationEnd(base);
    if (Fits(base, end - base)) {
      s_pools.smallHeap = heap;
      s_pools.smallBase = base;
      s_pools.smallEnd = end;
      break;
    }
    HeapDestroy(heap);
  }
  return TRUE;
}

void EnsurePools()
{
  InitOnceExecuteOnce(&s_pools.once, InitPools, nullptr, nullptr);
}

bool InLargePool(uintptr_t address)
{
  return (address >= s_pools.largeBase) && (address < s_pools.largeEnd);
}

bool InSmallHeap(uintptr_t address)
{
  return (address >= s_pools.smallBase) && (address < s_pools.smallEnd);
}

bool GranuleUsed(uint32_t index)
{
  return (s_pools.used[index / 64] >> (index % 64)) & 1u;
}

void MarkGranules(uint32_t first, uint32_t count, bool used)
{
  for (uint32_t index = first; index < first + count; index++) {
    if (used) {
      s_pools.used[index / 64] |= uint64_t{1} << (index % 64);
    }
    else {
      s_pools.used[index / 64] &= ~(uint64_t{1} << (index % 64));
    }
  }
}

/* First fit over the granules; committed (zeroed) pages. nullptr when the pool has no such run or no commit. */
void *AllocLarge(size_t bytes)
{
  if (s_pools.granules == 0) {
    return nullptr;
  }
  const size_t wanted = (bytes + kGranule - 1) / kGranule;
  if (wanted > s_pools.granules) {
    return nullptr;
  }
  const auto count = static_cast<uint32_t>(wanted);
  AcquireSRWLockExclusive(&s_pools.lock);
  uint32_t start = s_pools.searchFrom;
  while (start + count <= s_pools.granules) {
    if (GranuleUsed(start)) {
      start++;
      continue;
    }
    uint32_t length = 1;
    while ((length < count) && !GranuleUsed(start + length)) {
      length++;
    }
    if (length == count) {
      break;
    }
    start += length + 1;
  }
  void *block = nullptr;
  if (start + count <= s_pools.granules) {
    void *address = reinterpret_cast<void *>(s_pools.largeBase + static_cast<uintptr_t>(start) * kGranule);
    block = VirtualAlloc(address, static_cast<size_t>(count) * kGranule, MEM_COMMIT, PAGE_READWRITE);
    if (block != nullptr) {
      MarkGranules(start, count, true);
      s_pools.runLength[start] = count;
      if (start == s_pools.searchFrom) {
        s_pools.searchFrom = start + count;
      }
      s_pools.largeInUse += static_cast<size_t>(count) * kGranule;
      s_pools.largePeak = std::max(s_pools.largePeak, s_pools.largeInUse);
    }
  }
  ReleaseSRWLockExclusive(&s_pools.lock);
  return block;
}

void FreeLarge(uintptr_t address)
{
  const auto start = static_cast<uint32_t>((address - s_pools.largeBase) / kGranule);
  AcquireSRWLockExclusive(&s_pools.lock);
  const uint32_t count = s_pools.runLength[start];
  if (count != 0) {
    VirtualFree(reinterpret_cast<void *>(address), static_cast<size_t>(count) * kGranule, MEM_DECOMMIT);
    MarkGranules(start, count, false);
    s_pools.runLength[start] = 0;
    s_pools.searchFrom = std::min(s_pools.searchFrom, start);
    s_pools.largeInUse -= static_cast<size_t>(count) * kGranule;
  }
  ReleaseSRWLockExclusive(&s_pools.lock);
}

size_t BlockSize(void *block)
{
  const auto address = reinterpret_cast<uintptr_t>(block);
  if (InLargePool(address)) {
    AcquireSRWLockShared(&s_pools.lock);
    const size_t size = static_cast<size_t>(s_pools.runLength[(address - s_pools.largeBase) / kGranule]) * kGranule;
    ReleaseSRWLockShared(&s_pools.lock);
    return size;
  }
  const SIZE_T size = HeapSize(InSmallHeap(address) ? s_pools.smallHeap : GetProcessHeap(), 0, block);
  return (size == static_cast<SIZE_T>(-1)) ? 0 : size;
}

void NoteSmall(LONG64 delta)
{
  const LONG64 now = InterlockedAdd64(&s_pools.smallInUse, delta);
  LONG64 peak = s_pools.smallPeak;
  while ((now > peak) && (InterlockedCompareExchange64(&s_pools.smallPeak, now, peak) != peak)) {
    peak = s_pools.smallPeak;
  }
}

/* aligned operator new above 16 bytes: the raw block's address is kept in the word before the aligned one */
void *AllocAligned(size_t bytes, size_t alignment, bool zero) noexcept
{
  if (alignment <= 16) {
    return LowMemory_Alloc(bytes, zero);
  }
  void *raw = LowMemory_Alloc(bytes + alignment + sizeof(void *), zero);
  if (raw == nullptr) {
    return nullptr;
  }
  const uintptr_t aligned =
      (reinterpret_cast<uintptr_t>(raw) + sizeof(void *) + alignment - 1) & ~static_cast<uintptr_t>(alignment - 1);
  reinterpret_cast<void **>(aligned)[-1] = raw;
  return reinterpret_cast<void *>(aligned);
}

void FreeAligned(void *block, size_t alignment) noexcept
{
  if ((block == nullptr) || (alignment <= 16)) {
    LowMemory_Free(block);
    return;
  }
  LowMemory_Free(static_cast<void **>(block)[-1]);
}

void *ThrowingNew(size_t bytes)
{
  void *block = LowMemory_Alloc((bytes != 0) ? bytes : 1, false);
  if (block == nullptr) {
    throw std::bad_alloc();
  }
  return block;
}

void *ThrowingNewAligned(size_t bytes, std::align_val_t alignment)
{
  void *block = AllocAligned((bytes != 0) ? bytes : 1, static_cast<size_t>(alignment), false);
  if (block == nullptr) {
    throw std::bad_alloc();
  }
  return block;
}

void *SDLCALL SdlMalloc(size_t bytes)
{
  return LowMemory_Alloc((bytes != 0) ? bytes : 1, false);
}

void *SDLCALL SdlCalloc(size_t count, size_t size)
{
  if ((size != 0) && (count > SIZE_MAX / size)) {
    return nullptr;
  }
  const size_t bytes = count * size;
  return LowMemory_Alloc((bytes != 0) ? bytes : 1, true);
}

void *SDLCALL SdlRealloc(void *block, size_t bytes)
{
  if (block == nullptr) {
    return LowMemory_Alloc((bytes != 0) ? bytes : 1, false);
  }
  if (bytes == 0) {
    bytes = 1;
  }
  const auto address = reinterpret_cast<uintptr_t>(block);
  if (InSmallHeap(address) && (bytes <= kSmallLimit)) {
    const size_t before = BlockSize(block);
    void *grown = HeapReAlloc(s_pools.smallHeap, 0, block, bytes);
    if (grown != nullptr) {
      NoteSmall(static_cast<LONG64>(BlockSize(grown)) - static_cast<LONG64>(before));
      return grown;
    }
  }
  const size_t old = BlockSize(block);
  if ((old >= bytes) && InLargePool(address) && (old - bytes < kGranule)) {
    return block;
  }
  void *moved = LowMemory_Alloc(bytes, false);
  if (moved == nullptr) {
    return nullptr;
  }
  std::memcpy(moved, block, std::min(old, bytes));
  LowMemory_Free(block);
  return moved;
}

void SDLCALL SdlFree(void *block)
{
  LowMemory_Free(block);
}

} // namespace

void *LowMemory_Alloc(size_t bytes, bool zero) noexcept
{
  EnsurePools();
  void *block = nullptr;
  if ((bytes <= kSmallLimit) && (s_pools.smallHeap != nullptr)) {
    block = HeapAlloc(s_pools.smallHeap, zero ? HEAP_ZERO_MEMORY : 0, bytes);
    if (block != nullptr) {
      NoteSmall(static_cast<LONG64>(BlockSize(block)));
      return block;
    }
  }
  block = AllocLarge(bytes); /* committed pages are zero */
  if (block != nullptr) {
    return block;
  }
  block = HeapAlloc(GetProcessHeap(), zero ? HEAP_ZERO_MEMORY : 0, bytes);
  if (block != nullptr) {
    InterlockedIncrement64(&s_pools.fallbackCount);
    const bool high = reinterpret_cast<uintptr_t>(block) + bytes > k2G;
    if (high) {
      InterlockedIncrement64(&s_pools.fallbackHighCount);
    }
    if (InterlockedIncrement(&s_pools.fallbackLogs) <= 10) {
      Thandor_Log("low memory: pools full, %llu bytes from the process heap at 0x%llX%s",
                  static_cast<unsigned long long>(bytes),
                  static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(block)), high ? " (above 2 GB)" : "");
    }
  }
  return block;
}

void LowMemory_Free(void *block) noexcept
{
  if (block == nullptr) {
    return;
  }
  const auto address = reinterpret_cast<uintptr_t>(block);
  if (InLargePool(address)) {
    FreeLarge(address);
  }
  else if (InSmallHeap(address)) {
    NoteSmall(-static_cast<LONG64>(BlockSize(block)));
    HeapFree(s_pools.smallHeap, 0, block);
  }
  else {
    HeapFree(GetProcessHeap(), 0, block);
  }
}

void LowMemory_InstallSdlAllocator() noexcept
{
  EnsurePools();
  if (!SDL_SetMemoryFunctions(SdlMalloc, SdlCalloc, SdlRealloc, SdlFree)) {
    Thandor_Log("low memory: SDL_SetMemoryFunctions failed");
  }
}

void LowMemory_LogState(const char *when) noexcept
{
  EnsurePools();
  AcquireSRWLockShared(&s_pools.lock);
  const size_t largeInUse = s_pools.largeInUse;
  const size_t largePeak = s_pools.largePeak;
  ReleaseSRWLockShared(&s_pools.lock);
  Thandor_Log("low memory %s: large pool 0x%08llX-0x%08llX (%llu MiB, %llu MiB in use, peak %llu MiB), small heap "
              "0x%08llX-0x%08llX (%llu MiB, %llu KiB in use, peak %llu KiB), %lld process heap fallbacks (%lld above "
              "2 GB)",
              when, static_cast<unsigned long long>(s_pools.largeBase),
              static_cast<unsigned long long>(s_pools.largeEnd),
              static_cast<unsigned long long>((s_pools.largeEnd - s_pools.largeBase) / kMiB),
              static_cast<unsigned long long>(largeInUse / kMiB), static_cast<unsigned long long>(largePeak / kMiB),
              static_cast<unsigned long long>(s_pools.smallBase), static_cast<unsigned long long>(s_pools.smallEnd),
              static_cast<unsigned long long>((s_pools.smallEnd - s_pools.smallBase) / kMiB),
              static_cast<unsigned long long>(s_pools.smallInUse) / 1024,
              static_cast<unsigned long long>(s_pools.smallPeak) / 1024,
              static_cast<long long>(s_pools.fallbackCount), static_cast<long long>(s_pools.fallbackHighCount));
}

/* The replaceable global allocation functions: every C++ allocation of the game comes from the pools. */
void *operator new(size_t bytes)
{
  return ThrowingNew(bytes);
}
void *operator new[](size_t bytes)
{
  return ThrowingNew(bytes);
}
void *operator new(size_t bytes, const std::nothrow_t &) noexcept
{
  return LowMemory_Alloc((bytes != 0) ? bytes : 1, false);
}
void *operator new[](size_t bytes, const std::nothrow_t &) noexcept
{
  return LowMemory_Alloc((bytes != 0) ? bytes : 1, false);
}
void *operator new(size_t bytes, std::align_val_t alignment)
{
  return ThrowingNewAligned(bytes, alignment);
}
void *operator new[](size_t bytes, std::align_val_t alignment)
{
  return ThrowingNewAligned(bytes, alignment);
}
void *operator new(size_t bytes, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
  return AllocAligned((bytes != 0) ? bytes : 1, static_cast<size_t>(alignment), false);
}
void *operator new[](size_t bytes, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
  return AllocAligned((bytes != 0) ? bytes : 1, static_cast<size_t>(alignment), false);
}
void operator delete(void *block) noexcept
{
  LowMemory_Free(block);
}
void operator delete[](void *block) noexcept
{
  LowMemory_Free(block);
}
void operator delete(void *block, size_t) noexcept
{
  LowMemory_Free(block);
}
void operator delete[](void *block, size_t) noexcept
{
  LowMemory_Free(block);
}
void operator delete(void *block, const std::nothrow_t &) noexcept
{
  LowMemory_Free(block);
}
void operator delete[](void *block, const std::nothrow_t &) noexcept
{
  LowMemory_Free(block);
}
void operator delete(void *block, std::align_val_t alignment) noexcept
{
  FreeAligned(block, static_cast<size_t>(alignment));
}
void operator delete[](void *block, std::align_val_t alignment) noexcept
{
  FreeAligned(block, static_cast<size_t>(alignment));
}
void operator delete(void *block, size_t, std::align_val_t alignment) noexcept
{
  FreeAligned(block, static_cast<size_t>(alignment));
}
void operator delete[](void *block, size_t, std::align_val_t alignment) noexcept
{
  FreeAligned(block, static_cast<size_t>(alignment));
}
void operator delete(void *block, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
  FreeAligned(block, static_cast<size_t>(alignment));
}
void operator delete[](void *block, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
  FreeAligned(block, static_cast<size_t>(alignment));
}
