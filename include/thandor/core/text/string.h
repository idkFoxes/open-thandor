/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/text/string.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_TEXT_STRING_H
#define THANDOR_CORE_TEXT_STRING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/text/string. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00402D50 */
dword __thandor_eax_preserve_ecx_edx
WideNumber_FormatUtf16
          (WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
          WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator,
          WideNumberSignedValue32 value,word *destination);

/* 0x00403010 */
CompareFlagsCfZf2 __thandor_void_preserve_eax_ecx_edx
Utf16String_CompareAsciiCaseInsensitiveFlags(word *rightText,word *leftText);

/* 0x0041BAA0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Text_CopyNarrowToUtf16Cf(TextOutputCapacityBytes capacityBytes,word *destination,byte *source);

/* 0x00586DA0 */
dword Utf16_CopyAndReturnByteLength(word *destination,word *source);

/* 0x00402D30 */
void __thandor_void_preserve_eax_ecx_edx
WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,word *source,word *destination);

#endif /* THANDOR_CORE_TEXT_STRING_H */
