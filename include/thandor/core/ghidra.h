/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/ghidra.h
 */

#ifndef THANDOR_CORE_GHIDRA_H
#define THANDOR_CORE_GHIDRA_H

/*
Ghidra decompiler pseudo-operations, expressed in C.

Values are little-endian x86 register images. Every CONCATxy / SUBxy / ZEXTxy is computed
in 64 bits; results wider than 8 bytes (CONCAT55, CONCAT62, ZEXT513, ...) are truncated to the
low 8 bytes, which matches every use where the result lands in a register or qword.
*/

#include <stddef.h>
#include <string.h>

#define THANDOR_MASK_BYTES(n) ((n) >= 8 ? ~0ull : ((1ull << ((n) * 8)) - 1ull))

/* CONCATxy(hi, lo): hi is x bytes, lo is y bytes; result = hi:lo. */
#define THANDOR_CONCAT(x, y, hi, lo) \
    ((((unsigned long long)(hi) & THANDOR_MASK_BYTES(x)) << ((y) * 8 % 64)) | \
     ((unsigned long long)(lo) & THANDOR_MASK_BYTES(y)))
#define CONCAT11(h, l) THANDOR_CONCAT(1, 1, h, l)
#define CONCAT12(h, l) THANDOR_CONCAT(1, 2, h, l)
#define CONCAT13(h, l) THANDOR_CONCAT(1, 3, h, l)
#define CONCAT14(h, l) THANDOR_CONCAT(1, 4, h, l)
#define CONCAT15(h, l) THANDOR_CONCAT(1, 5, h, l)
#define CONCAT16(h, l) THANDOR_CONCAT(1, 6, h, l)
#define CONCAT17(h, l) THANDOR_CONCAT(1, 7, h, l)
#define CONCAT21(h, l) THANDOR_CONCAT(2, 1, h, l)
#define CONCAT22(h, l) THANDOR_CONCAT(2, 2, h, l)
#define CONCAT24(h, l) THANDOR_CONCAT(2, 4, h, l)
#define CONCAT26(h, l) THANDOR_CONCAT(2, 6, h, l)
#define CONCAT31(h, l) THANDOR_CONCAT(3, 1, h, l)
#define CONCAT35(h, l) THANDOR_CONCAT(3, 5, h, l)
#define CONCAT41(h, l) THANDOR_CONCAT(4, 1, h, l)
#define CONCAT44(h, l) THANDOR_CONCAT(4, 4, h, l)
#define CONCAT51(h, l) THANDOR_CONCAT(5, 1, h, l)
#define CONCAT55(h, l) THANDOR_CONCAT(5, 5, h, l) /* TODO: 10-byte result truncated */
#define CONCAT62(h, l) THANDOR_CONCAT(6, 2, h, l)

/* SUBxy(v, off): y bytes of the x-byte value v starting at byte off. */
#define THANDOR_SUB(y, v, off) (((unsigned long long)(v) >> ((off) * 8)) & THANDOR_MASK_BYTES(y))
#define SUB41(v, off) ((byte)THANDOR_SUB(1, v, off))
#define SUB42(v, off) ((word)THANDOR_SUB(2, v, off))
#define SUB81(v, off) ((byte)THANDOR_SUB(1, v, off))
#define SUB82(v, off) ((word)THANDOR_SUB(2, v, off))
#define SUB84(v, off) ((dword)THANDOR_SUB(4, v, off))

/* ZEXTxy / SEXTxy: zero/sign extension from x bytes. */
#define ZEXT48(v) ((unsigned long long)(dword)(v))
#define ZEXT513(v) ((unsigned long long)(v) & THANDOR_MASK_BYTES(5)) /* TODO: 13-byte result truncated */
#define SEXT48(v) ((long long)(int)(v))

/* Flag helpers: carry / signed overflow of x-byte addition, signed overflow of subtraction. */
#define CARRY1(a, b) ((unsigned char)((unsigned char)(a) + (unsigned char)(b)) < (unsigned char)(a))
#define CARRY4(a, b) ((unsigned int)((unsigned int)(a) + (unsigned int)(b)) < (unsigned int)(a))
#define SCARRY4(a, b) \
    ((((int)(a) ^ (int)((unsigned int)(a) + (unsigned int)(b))) & ((int)(b) ^ (int)((unsigned int)(a) + (unsigned int)(b)))) < 0)
#define SBORROW4(a, b) \
    ((((int)(a) ^ (int)(b)) & ((int)(a) ^ (int)((unsigned int)(a) - (unsigned int)(b)))) < 0)

/*
Partial access "base._off_size_" (Ghidra field-piece syntax).
Power-of-two sizes stay lvalues; odd sizes go through exact-width byte copies.
*/
#define THANDOR_PART(T, base, off) (*(T *)((unsigned char *)&(base) + (off)))

static __inline unsigned long long thandor_read_part(const void *base, unsigned off, unsigned size)
{
    unsigned long long v = 0;
    memcpy(&v, (const unsigned char *)base + off, size > 8 ? 8 : size);
    return v;
}

static __inline void thandor_write_part(void *base, unsigned off, unsigned size, unsigned long long v)
{
    memcpy((unsigned char *)base + off, &v, size > 8 ? 8 : size);
}

#define THANDOR_READ_PART(base, off, size) thandor_read_part(&(base), (off), (size))
#define THANDOR_WRITE_PART(base, off, size, v) thandor_write_part(&(base), (off), (size), (unsigned long long)(v))

/* ADJ(p) on a Ghidra shifted pointer: the structure that contains the member p points at. */
#define THANDOR_CONTAINER_OF(p, Outer, member) ((Outer *)((unsigned char *)(p) - offsetof(Outer, member)))

#endif /* THANDOR_CORE_GHIDRA_H */
