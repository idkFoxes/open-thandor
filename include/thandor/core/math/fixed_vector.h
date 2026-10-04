/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/fixed_vector.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_FIXED_VECTOR_H
#define THANDOR_CORE_MATH_FIXED_VECTOR_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/fixed_vector. */

/* FixedMath_SqrtQ12Approx: the normalizing shift is even (a mask over bits 1..4), and the cubic in the
   normalized input is ((C3 * x - C2) * x + C1) * x + C0, each product a FIXED_MUL_HIGH */
#define FIXED_SQRT_EVEN_SHIFT_MASK 0x1e
#define FIXED_SQRT_POLY_C3 0x25ed098
#define FIXED_SQRT_POLY_C2 0x1c71c71
#define FIXED_SQRT_POLY_C1 0xb1c71c
#define FIXED_SQRT_POLY_C0 0x66b75U

/* Functions are grouped by semantic ownership. */

uint32_t FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z);

void FixedVec3_NormalizeQ28(GraphicsFixedVec3 *output,GraphicsFixedVec3 *input);

int32_t FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right);

int32_t FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right);

void FixedVec3_CrossQ12(GraphicsFixedVec3 *output,GraphicsFixedVec3 *rightOperand,
                  GraphicsFixedVec3 *leftOperand);

uint32_t FixedMath_LengthVec3(GraphicsFixedVec3 *vector);

uint32_t FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y);

uint32_t FixedMath_UInt64Sqrt(UInt64Half32 high,UInt64Half32 low);

uint32_t FixedMath_SqrtQ12Approx(uint32_t inputValue);

#endif /* THANDOR_CORE_MATH_FIXED_VECTOR_H */
