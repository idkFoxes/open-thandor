/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/fixed_point.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_FIXED_POINT_H
#define THANDOR_CORE_MATH_FIXED_POINT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Fixed-point formats and helpers shared by the fixed-point math files and their users. */

#ifndef Q12_ONE
#define Q12_ONE 0x1000 /* 1.0 in Q12 fixed point (scales, world coordinates) */
#endif
#ifndef Q28_ONE
#define Q28_ONE 0x10000000 /* 1.0 in Q28 fixed point (g_FixedSineQ28 values) */
#endif
/* Indices into g_FixedSineQ28 (one sine over 1.5 turns from a quarter turn before angle 0):
   sin(a) = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + a], cos(a) = g_FixedSineQ28[FIXED_SINE_TABLE_COS + a]
   (a = -16384..65535 for sin, -32768..65535 for cos). */
#define FIXED_SINE_TABLE_SIN 0x4000
#define FIXED_SINE_TABLE_COS 0x8000
#ifndef Q8_ONE
#define Q8_ONE 0x100 /* 1.0 in Q8 fixed point (game speed, AI scales and ratios) */
#endif
#ifndef Q4_SHIFT
#define Q4_SHIFT 4 /* fraction bits of the Q4 resource and Energy rates */
#endif

/* Engine angles are 16-bit fractions of a full turn (0x10000 = 360 degrees), as used by the
   g_FixedSineQ28 lookups and returned by FixedMath_Atan2Angle16. */
#define FIXED_ANGLE16_EIGHTH_TURN 0x2000
#define FIXED_ANGLE16_QUARTER_TURN 0x4000
#define FIXED_ANGLE16_HALF_TURN 0x8000
#define FIXED_ANGLE16_THREE_QUARTER_TURN 0xC000
#define FIXED_ANGLE16_FULL_TURN 0x10000
/* Wraps an angle to 16 bits (one full turn) before a g_FixedSineQ28 lookup */
#define FIXED_ANGLE16_MASK 0xffff

/* The low 32 bits of a signed 64-bit product shifted right by `shift` (0 < shift < 32): the original's
   double shift of the high:low halves (SHLD by 32-shift / SHRD by shift), which drops the fixed-point factor of a product (28 for a Q28
   factor, 12 for Q12). Written as the two halves so the compiler emits the same SHLD form. */
#define FIXED_PRODUCT_SHR(product, shift) \
  ((int)((uint64_t)(product) >> 32) << (32 - (shift)) | (uint32_t)(product) >> (shift))
/* (a * b) >> shift in 64 bits, low 32 bits: the product and the shift in one step (IMUL + SHRD). Use it
   where the product is not needed otherwise; check the code stays the same (a named temporary for the product
   can change the register allocation). */
#define FIXED_MUL_SHR(a, b, shift) FIXED_PRODUCT_SHR((int64_t)(a) * (int64_t)(b), shift)
/* Fixed-point fraction bits (shift counts) */
#define Q12_SHIFT 12
#define Q20_SHIFT 20
#define Q28_SHIFT 28
#define Q12_FRACTION_MASK 0xfff /* fraction part of a Q12 value */

/* High 32 bits of the signed 64-bit product a * b (the high half of a one-operand IMUL) */
#define FIXED_MUL_HIGH(a, b) ((int)((uint64_t)((int64_t)(a) * (int64_t)(b)) >> 32))
/* 2^32 (a 64-bit constant): 1.0 in Q32, the dividend of FixedVec3_NormalizeQ28's reciprocal length */
#define Q32_ONE 0x100000000

/* FixedMath_UInt64Sqrt of a 64-bit sum of squares, passed as its high and low halves */
#define FIXED_UINT64_SQRT(value) \
  FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)(value) >> 32),(UInt64Half32)(value))

#endif /* THANDOR_CORE_MATH_FIXED_POINT_H */
