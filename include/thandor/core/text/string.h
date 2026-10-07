/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/text/string.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_TEXT_STRING_H
#define THANDOR_CORE_TEXT_STRING_H

#include <thandor/core/text/types.h>
#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* ASCII/UTF-16 letter case bit: 'A' | this = 'a' (Utf16String_CompareAsciiCaseInsensitiveFlags) */
inline constexpr auto TEXT_ASCII_LOWER_CASE_BIT = 0x20;

uint32_t WideNumber_FormatUtf16(WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
          WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator,
          WideNumberSignedValue32 value,uint16_t *destination);

int Utf16String_CompareAsciiCaseInsensitiveFlags(uint16_t *rightText,uint16_t *leftText);

uint32_t Text_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source);

uint32_t Utf16_CopyAndReturnByteLength(uint16_t *destination,uint16_t *source);

void WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,uint16_t *source,uint16_t *destination);

extern WideNumberFormatUtf16Proc *g_WideNumberFormatUtf16;

#endif /* THANDOR_CORE_TEXT_STRING_H */
