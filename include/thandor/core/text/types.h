/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/text/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_TEXT_TYPES_H
#define THANDOR_CORE_TEXT_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct WideNumberFormatState WideNumberFormatState, *PWideNumberFormatState;

using WideNumberFormatCodeUnitCount = uint32_t;

using TextOutputCapacityBytes = uint32_t;

using PackedFileExtensionCode32 = uint32_t;

using WideNumberIntegerDigitLimit = uint32_t;

using WideNumberSignedValue32 = int;

using WideNumberFractionalDigitCount = uint32_t;

using WideNumberDenominator32 = uint32_t;

struct WideNumberFormatState {
    WideNumberFormatCodeUnitCount decimalSeparatorLength; 
    WideNumberFormatCodeUnitCount groupSeparatorLength; 
    WideNumberFormatCodeUnitCount positiveSignLength; 
    WideNumberFormatCodeUnitCount negativeSignLength; 
    WideNumberFormatCodeUnitCount hexPrefixLength; 
    WideNumberFormatCodeUnitCount hexSuffixLength; 
    uint16_t decimalSeparator[16]; 
    uint16_t groupSeparator[16]; 
    uint16_t positiveSign[16]; 
    uint16_t negativeSign[16]; 
    uint16_t hexPrefix[16]; 
    uint16_t hexSuffix[16]; 
    uint16_t spacePadding[16]; 
    uint16_t zeroPadding[16]; 
    uint16_t reservedZero[16]; 
    uint16_t digitAlphabet[16]; 
};
using WideNumberFormatUtf16Proc = uint32_t (WideNumberFormatFlags flags, uint32_t fractionalDigits, uint32_t integerDigitLimit, uint32_t denominator, int32_t value, uint16_t * destination);

#endif /* THANDOR_CORE_TEXT_TYPES_H */
