/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/x86_emulation.h
 */

#ifndef THANDOR_CORE_X86_EMULATION_H
#define THANDOR_CORE_X86_EMULATION_H

/*
Helpers that reproduce what the original's x86 code does, expressed in portable C++: container-of, atomic exchange,
x87 rounding and the MMX lane operations (with the original's wrap-around and saturation).
*/

#include <thandor/core/types.h>
#include <stddef.h>
#include <intrin.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <type_traits>

/* The structure that contains the member p points at (memberOffset bytes into it). Like the former C-style cast
   it also takes a pointer to const and yields a mutable Outer *. */
template <class Outer, class P> static __forceinline Outer *thandor_container_of(P *p, size_t memberOffset)
{
    return reinterpret_cast<Outer *>(const_cast<unsigned char *>(reinterpret_cast<const volatile unsigned char *>(p)) -
                                     memberOffset);
}
#define THANDOR_CONTAINER_OF(p, Outer, member) (thandor_container_of<Outer>((p), offsetof(Outer, member)))

/*
THANDOR_ATOMIC_EXCHANGE(ptr, value): the original's XCHG with memory (implicitly locked) on a 32-bit
location shared with the timer thread (g_TimerRegisterPeriodic callbacks): stores value and
returns the previous contents as uint32_t, in one atomic step. Compiles to XCHG. Sites whose memory only
one thread touches use a plain load and store.
A pointer-sized location (a pointer or uintptr_t/handle slot, 8 bytes) is swapped as a whole and the
previous contents come back as uintptr_t.
*/
template <typename T, typename V>
static __forceinline auto thandor_atomic_exchange(T *ptr, V value)
{
    if constexpr (sizeof(T) == 8) {
        return reinterpret_cast<uintptr_t>(_InterlockedExchangePointer(reinterpret_cast<void *volatile *>(ptr),
                                                                       reinterpret_cast<void *>(static_cast<uintptr_t>(value))));
    }
    else
    {
        static_assert(sizeof(T) == 4, "THANDOR_ATOMIC_EXCHANGE needs a 4-byte or pointer-sized location");
        return static_cast<uint32_t>(_InterlockedExchange(reinterpret_cast<volatile long *>(ptr),
                                                          static_cast<long>(static_cast<uintptr_t>(value))));
    }
}
#define THANDOR_ATOMIC_EXCHANGE(ptr, value) thandor_atomic_exchange((ptr), (value))

/*
Unaligned little-endian loads and stores of 16/32/64-bit values at any address, the portable form of the original's
MOV/MOVQ through a cast pointer (`*(uint64_t *)(p + off)`). memcpy makes them alias-safe and alignment-safe; both
compilers turn the fixed-size memcpy into a single load or store, so the code is the same as the cast.
The const void * forms take any pointer. The unsigned char * forms are also constexpr (they assemble the bytes
little-endian during constant evaluation; a void * cannot be read there).
*/
template <class U> static __forceinline U thandor_load_le(const void *p)
{
    U v;
    memcpy(&v, p, sizeof v);
    return v;
}
template <class U> static __forceinline void thandor_store_le(void *p, U v) { memcpy(p, &v, sizeof v); }
template <class U> static __forceinline constexpr U thandor_load_le(const unsigned char *p)
{
    if (std::is_constant_evaluated()) {
        U v = 0;
        for (size_t i = 0; i < sizeof v; i++) v = (U)(v | ((U)p[i] << (8 * i)));
        return v;
    }
    return thandor_load_le<U>(static_cast<const void *>(p));
}
template <class U> static __forceinline constexpr void thandor_store_le(unsigned char *p, U v)
{
    if (std::is_constant_evaluated()) {
        for (size_t i = 0; i < sizeof v; i++) p[i] = (unsigned char)(v >> (8 * i));
        return;
    }
    thandor_store_le<U>(static_cast<void *>(p), v);
}

static __forceinline uint16_t Thandor_LoadU16(const void *p) { return thandor_load_le<uint16_t>(p); }
static __forceinline uint32_t Thandor_LoadU32(const void *p) { return thandor_load_le<uint32_t>(p); }
static __forceinline uint64_t Thandor_LoadU64(const void *p) { return thandor_load_le<uint64_t>(p); }
static __forceinline void Thandor_StoreU16(void *p, uint16_t v) { thandor_store_le<uint16_t>(p, v); }
static __forceinline void Thandor_StoreU32(void *p, uint32_t v) { thandor_store_le<uint32_t>(p, v); }
static __forceinline void Thandor_StoreU64(void *p, uint64_t v) { thandor_store_le<uint64_t>(p, v); }
static __forceinline constexpr uint16_t Thandor_LoadU16(const unsigned char *p) { return thandor_load_le<uint16_t>(p); }
static __forceinline constexpr uint32_t Thandor_LoadU32(const unsigned char *p) { return thandor_load_le<uint32_t>(p); }
static __forceinline constexpr uint64_t Thandor_LoadU64(const unsigned char *p) { return thandor_load_le<uint64_t>(p); }
static __forceinline constexpr void Thandor_StoreU16(unsigned char *p, uint16_t v) { thandor_store_le<uint16_t>(p, v); }
static __forceinline constexpr void Thandor_StoreU32(unsigned char *p, uint32_t v) { thandor_store_le<uint32_t>(p, v); }
static __forceinline constexpr void Thandor_StoreU64(unsigned char *p, uint64_t v) { thandor_store_le<uint64_t>(p, v); }

/* Compile-time check of the constexpr forms (little-endian byte order, store/load round trip). */
static_assert([] {
    const unsigned char b[4] = {0x78, 0x56, 0x34, 0x12};
    return Thandor_LoadU16(b) == 0x5678u && Thandor_LoadU32(b) == 0x12345678u;
}());
static_assert([] {
    unsigned char b[8] = {};
    Thandor_StoreU64(b, 0x0123456789abcdefull);
    Thandor_StoreU16(b + 2, 0xbeefu);
    return b[0] == 0xef && b[2] == 0xef && b[3] == 0xbe && b[7] == 0x01 && Thandor_LoadU64(b) == 0x01234567beefcdefull;
}());

/* ROUND(x): x87 FRNDINT in the default round-to-nearest-even mode. */
#define ROUND(x) rint(x)

/*
MMX instructions on 64-bit register images (Intel SDM semantics, little-endian lanes).
*/
typedef union ThandorMmx {
    unsigned long long q;
    short sw[4];
    unsigned short uw[4];
    int sd[2];
    unsigned char ub[8];
} ThandorMmx;

static inline unsigned long long thandor_mmx_pmulhw(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i;
    x.q = a; y.q = b;
    for (i = 0; i < 4; i++) r.sw[i] = (short)(((int)x.sw[i] * (int)y.sw[i]) >> 16);
    return r.q;
}

static inline unsigned long long thandor_mmx_pmaddwd(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    x.q = a; y.q = b;
    r.sd[0] = (int)x.sw[0] * y.sw[0] + (int)x.sw[1] * y.sw[1];
    r.sd[1] = (int)x.sw[2] * y.sw[2] + (int)x.sw[3] * y.sw[3];
    return r.q;
}

static inline unsigned long long thandor_mmx_paddusb(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i, s;
    x.q = a; y.q = b;
    for (i = 0; i < 8; i++) { s = x.ub[i] + y.ub[i]; r.ub[i] = (unsigned char)(s > 0xff ? 0xff : s); }
    return r.q;
}

static inline unsigned long long thandor_mmx_paddusw(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i;
    unsigned s;
    x.q = a; y.q = b;
    for (i = 0; i < 4; i++) { s = (unsigned)x.uw[i] + y.uw[i]; r.uw[i] = (unsigned short)(s > 0xffff ? 0xffff : s); }
    return r.q;
}

static inline unsigned long long thandor_mmx_paddsw(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i, s;
    x.q = a; y.q = b;
    for (i = 0; i < 4; i++) {
        s = x.sw[i] + y.sw[i];
        r.sw[i] = (short)(s > 0x7fff ? 0x7fff : s < -0x8000 ? -0x8000 : s);
    }
    return r.q;
}

static inline unsigned long long thandor_mmx_psraw(unsigned long long a, unsigned long long count)
{
    ThandorMmx x, r;
    int i;
    unsigned c = count > 15 ? 15 : (unsigned)count;
    x.q = a;
    for (i = 0; i < 4; i++) r.sw[i] = (short)(x.sw[i] >> c);
    return r.q;
}

/* Operands are integers or the 8-byte lane structs some MMX values are typed as. */
static inline unsigned long long thandor_mmx_rgb(SoftwareRgbWordLanes v) { unsigned long long q; memcpy(&q, &v, 8); return q; }
static inline unsigned long long thandor_mmx_bgra(SoftwareBgraWordLanes v) { unsigned long long q; memcpy(&q, &v, 8); return q; }
static inline unsigned long long thandor_mmx_q(SoftwareRgbWordLanes v) { return thandor_mmx_rgb(v); }
static inline unsigned long long thandor_mmx_q(SoftwareBgraWordLanes v) { return thandor_mmx_bgra(v); }
static inline unsigned long long thandor_mmx_q(unsigned long long v) { return v; }
#define THANDOR_MMX_Q(v) thandor_mmx_q(v)

#define pmulhw(a, b) thandor_mmx_pmulhw(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define pmaddwd(a, b) thandor_mmx_pmaddwd(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define paddusb(a, b) thandor_mmx_paddusb(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define paddusw(a, b) thandor_mmx_paddusw(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define paddsw(a, b) thandor_mmx_paddsw(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define psraw(a, n) thandor_mmx_psraw(THANDOR_MMX_Q(a), (unsigned long long)(n))

#endif /* THANDOR_CORE_X86_EMULATION_H */
