#ifndef THANDOR_CORE_TEXT_STRING_H
#define THANDOR_CORE_TEXT_STRING_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/text/string. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00402D50 */
dword WideNumber_FormatUtf16 (WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits, WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator, WideNumberSignedValue32 value,word *destination);

/* 0x00403010 */
undefined8 Utf16String_CompareAsciiCaseInsensitiveFlags(word *rightText,word *leftText);

/* 0x0041BAA0 */
int Text_CopyNarrowToUtf16Cf(TextOutputCapacityBytes capacityBytes,word *destination,byte *source);

/* 0x00586DA0 */
dword Utf16_CopyAndReturnByteLength(word *destination,word *source);

/* 0x00402D30 */
void WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,word *source,word *destination);

#endif /* THANDOR_CORE_TEXT_STRING_H */
