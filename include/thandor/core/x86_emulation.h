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

/* The structure that contains the member p points at. */
#define THANDOR_CONTAINER_OF(p, Outer, member) ((Outer *)((unsigned char *)(p) - offsetof(Outer, member)))

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
        return (uintptr_t)_InterlockedExchangePointer((void *volatile *)(ptr), (void *)(uintptr_t)(value));
    }
    else
    {
        static_assert(sizeof(T) == 4, "THANDOR_ATOMIC_EXCHANGE needs a 4-byte or pointer-sized location");
        return (uint32_t)_InterlockedExchange((volatile long *)(ptr), (long)(uintptr_t)(value));
    }
}
#define THANDOR_ATOMIC_EXCHANGE(ptr, value) thandor_atomic_exchange((ptr), (value))

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

static __inline unsigned long long thandor_mmx_pmulhw(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i;
    x.q = a; y.q = b;
    for (i = 0; i < 4; i++) r.sw[i] = (short)(((int)x.sw[i] * (int)y.sw[i]) >> 16);
    return r.q;
}

static __inline unsigned long long thandor_mmx_pmaddwd(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    x.q = a; y.q = b;
    r.sd[0] = (int)x.sw[0] * y.sw[0] + (int)x.sw[1] * y.sw[1];
    r.sd[1] = (int)x.sw[2] * y.sw[2] + (int)x.sw[3] * y.sw[3];
    return r.q;
}

static __inline unsigned long long thandor_mmx_paddusb(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i, s;
    x.q = a; y.q = b;
    for (i = 0; i < 8; i++) { s = x.ub[i] + y.ub[i]; r.ub[i] = (unsigned char)(s > 0xff ? 0xff : s); }
    return r.q;
}

static __inline unsigned long long thandor_mmx_paddusw(unsigned long long a, unsigned long long b)
{
    ThandorMmx x, y, r;
    int i;
    unsigned s;
    x.q = a; y.q = b;
    for (i = 0; i < 4; i++) { s = (unsigned)x.uw[i] + y.uw[i]; r.uw[i] = (unsigned short)(s > 0xffff ? 0xffff : s); }
    return r.q;
}

static __inline unsigned long long thandor_mmx_paddsw(unsigned long long a, unsigned long long b)
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

static __inline unsigned long long thandor_mmx_psraw(unsigned long long a, unsigned long long count)
{
    ThandorMmx x, r;
    int i;
    unsigned c = count > 15 ? 15 : (unsigned)count;
    x.q = a;
    for (i = 0; i < 4; i++) r.sw[i] = (short)(x.sw[i] >> c);
    return r.q;
}

/* Operands are integers or the 8-byte lane structs some MMX values are typed as. */
static __inline unsigned long long thandor_mmx_rgb(SoftwareRgbWordLanes v) { unsigned long long q; memcpy(&q, &v, 8); return q; }
static __inline unsigned long long thandor_mmx_bgra(SoftwareBgraWordLanes v) { unsigned long long q; memcpy(&q, &v, 8); return q; }
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
