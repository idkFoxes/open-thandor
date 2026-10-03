/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/x86_emulation.h
 */

#ifndef THANDOR_CORE_X86_EMULATION_H
#define THANDOR_CORE_X86_EMULATION_H

/*
Helpers that reproduce what the original's x86 code does, expressed in portable C: container-of, atomic exchange,
x87 rounding, CPUID and the MMX lane operations (with the original's wrap-around and saturation).
*/

#include <stddef.h>
#include <intrin.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

/* The structure that contains the member p points at. */
#define THANDOR_CONTAINER_OF(p, Outer, member) ((Outer *)((unsigned char *)(p) - offsetof(Outer, member)))

/* LOCK()/UNLOCK(): no-op markers where the original swaps memory atomically (XCHG); the swap itself is
   spelled out. */
#define LOCK() ((void)0)
#define UNLOCK() ((void)0)

/*
THANDOR_ATOMIC_EXCHANGE(ptr, value): the original's XCHG with memory (implicitly locked) on a 32-bit
location shared with the WinMM timer thread (TimerSystem_RegisterPeriodic callbacks): stores value and
returns the previous contents as uint32_t, in one atomic step. Compiles to XCHG. Sites whose memory only
one thread touches keep LOCK()/UNLOCK() plus a plain load and store.
*/
#define THANDOR_ATOMIC_EXCHANGE(ptr, value) \
    ((uint32_t)_InterlockedExchange((volatile long *)(ptr), (long)(uintptr_t)(value)))

/* ROUND(x): x87 FRNDINT in the default round-to-nearest-even mode. */
#define ROUND(x) rint(x)

/*
cpuid_Version_info(leaf): runs CPUID for leaf. Returns the address of a static array holding the result
as {EAX, EBX, EDX, ECX} (the CPUID output order); callers read feature bits from offset 8 (EDX).
*/
static __inline int cpuid_Version_info(int leaf)
{
    static unsigned int regs[4];
    int r[4];
    __cpuid(r, leaf);
    regs[0] = (unsigned int)r[0];
    regs[1] = (unsigned int)r[1];
    regs[2] = (unsigned int)r[3];
    regs[3] = (unsigned int)r[2];
    return (int)(uintptr_t)regs;
}

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

static __inline unsigned long long thandor_mmx_psllw(unsigned long long a, unsigned long long count)
{
    ThandorMmx x, r;
    int i;
    x.q = a;
    for (i = 0; i < 4; i++) r.uw[i] = count > 15 ? 0 : (unsigned short)(x.uw[i] << count);
    return r.q;
}

static __inline unsigned long long thandor_mmx_identity(unsigned long long v) { return v; }
/* Operands are integers or the 8-byte lane structs some MMX values are typed as. */
static __inline unsigned long long thandor_mmx_rgb(SoftwareRgbWordLanes v) { unsigned long long q; memcpy(&q, &v, 8); return q; }
static __inline unsigned long long thandor_mmx_bgra(SoftwareBgraWordLanes v) { unsigned long long q; memcpy(&q, &v, 8); return q; }
#ifdef __cplusplus
static inline unsigned long long thandor_mmx_q(SoftwareRgbWordLanes v) { return thandor_mmx_rgb(v); }
static inline unsigned long long thandor_mmx_q(SoftwareBgraWordLanes v) { return thandor_mmx_bgra(v); }
static inline unsigned long long thandor_mmx_q(unsigned long long v) { return v; }
#define THANDOR_MMX_Q(v) thandor_mmx_q(v)
#else
#define THANDOR_MMX_Q(v) _Generic((v),     SoftwareRgbWordLanes: thandor_mmx_rgb,     SoftwareBgraWordLanes: thandor_mmx_bgra,     default: thandor_mmx_identity)(v)
#endif

#define pmulhw(a, b) thandor_mmx_pmulhw(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define pmaddwd(a, b) thandor_mmx_pmaddwd(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define paddusb(a, b) thandor_mmx_paddusb(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define paddusw(a, b) thandor_mmx_paddusw(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define paddsw(a, b) thandor_mmx_paddsw(THANDOR_MMX_Q(a), THANDOR_MMX_Q(b))
#define psraw(a, n) thandor_mmx_psraw(THANDOR_MMX_Q(a), (unsigned long long)(n))
#define psllw(a, n) thandor_mmx_psllw(THANDOR_MMX_Q(a), (unsigned long long)(n))


/* The original scales floats by powers of two with integer adds on their bit pattern (e.g. adding
   -0x6000000 to the exponent bits divides by 2^12); this adds delta to the bit pattern of lvalue. */
#define THANDOR_FLOAT_ADD_EXPONENT_BITS(lvalue, delta) (*(int32_t *)&(lvalue) += (int32_t)(delta))

#endif /* THANDOR_CORE_X86_EMULATION_H */
