/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/string.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/text/string.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/text/string. */

/* Address: 0x00402D50.
   Ownership: core/text/string.
   Purpose: Formats one 32-bit value into UTF-16. Decimal mode divides the unsigned magnitude by denominator,
   limits the integer part to integerDigitLimit most-significant digits, supports sign prefixes, left space/zero
   padding, one separator before the final three integer digits, and optional fixed-width fractional digits. Hex
   mode emits the 0x prefix, two/four/six/eight uppercase digits after leading zero-byte suppression, and the
   configured suffix. WIDE_FORMAT_WRITE_TERMINATOR writes a UTF-16 NUL but the returned byte count excludes it.
   denominator must be nonzero.
   Local calls: WideText_CopyCodeUnits.
*/
uint32_t __thandor_eax_preserve_ecx_edx
WideNumber_FormatUtf16
          (WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
          WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator,
          WideNumberSignedValue32 value,uint16_t *destination)

{
  uint64_t previousValue;
  uint32_t integerPartOrHexDigit;
  uint16_t *source;
  WideNumberFormatCodeUnitCount segmentLength;
  uint32_t digitCount;
  UiTextCodeUnitCount codeUnitCount;
  uint32_t fractionRemainder;
  uint16_t *signText;
  uint16_t *paddingText;
  uint16_t *destinationCursor;
  
  segmentLength = g_WideNumberFormatState.hexPrefixLength;
  if ((flags & WIDE_FORMAT_HEXADECIMAL) == 0) {
    segmentLength = 0;
    if (((flags & WIDE_FORMAT_SIGNED_VALUE) != 0) &&
       (((flags & WIDE_FORMAT_SHOW_PLUS_SIGN) != 0 || (value < 0)))) {
      if (value < 0) {
        signText = g_WideNumberFormatState.negativeSign;
        value = -value;
        segmentLength = g_WideNumberFormatState.negativeSignLength;
      }
      else {
        signText = g_WideNumberFormatState.positiveSign;
        segmentLength = g_WideNumberFormatState.positiveSignLength;
      }
    }
    WideText_CopyCodeUnits(segmentLength,signText,destination);
    destinationCursor = destination + segmentLength;
    integerPartOrHexDigit = (uint32_t)value / denominator;
    fractionRemainder = (uint32_t)value % denominator;
    source = g_WideNumberFormatState.digitAlphabet;
    digitCount = 0;
    do {
      previousValue = (uint64_t)integerPartOrHexDigit;
      integerPartOrHexDigit = integerPartOrHexDigit / 10;
      source = source + -1;
      digitCount = digitCount + 1;
      *source = (short)(previousValue % 10) + 0x30;
    } while (integerPartOrHexDigit != 0);
    paddingText = g_WideNumberFormatState.spacePadding;
    if (integerDigitLimit < digitCount) {
      digitCount = integerDigitLimit;
    }
    if ((flags & (WIDE_FORMAT_PAD_WITH_SPACE|WIDE_FORMAT_PAD_WITH_ZERO)) != 0) {
      if ((flags & WIDE_FORMAT_PAD_WITH_ZERO) != 0) {
        paddingText = g_WideNumberFormatState.zeroPadding;
      }
      WideText_CopyCodeUnits(integerDigitLimit - digitCount,paddingText,destinationCursor);
      destinationCursor = destinationCursor + (integerDigitLimit - digitCount);
    }
    if (((flags & WIDE_FORMAT_GROUP_THOUSANDS) != 0) && (3 < digitCount)) {
      codeUnitCount = digitCount - 3;
      WideText_CopyCodeUnits(codeUnitCount,source,destinationCursor);
      segmentLength = g_WideNumberFormatState.groupSeparatorLength;
      source = source + codeUnitCount;
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.groupSeparatorLength,g_WideNumberFormatState.groupSeparator
                 ,destinationCursor + codeUnitCount);
      destinationCursor = destinationCursor + codeUnitCount + segmentLength;
      digitCount = 3;
    }
    WideText_CopyCodeUnits(digitCount,source,destinationCursor);
    segmentLength = g_WideNumberFormatState.decimalSeparatorLength;
    destinationCursor = destinationCursor + digitCount;
    if (((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0) || (denominator != 1)) {
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.decimalSeparatorLength,
                 g_WideNumberFormatState.decimalSeparator,destinationCursor);
      destinationCursor = destinationCursor + segmentLength;
      if (fractionalDigits != 0) {
        do {
          previousValue = (uint64_t)fractionRemainder;
          fractionRemainder = (uint32_t)((previousValue * 10) % (uint64_t)denominator);
          *destinationCursor = (short)((previousValue * 10) / (uint64_t)denominator) + 0x30;
          destinationCursor = destinationCursor + 1;
          fractionalDigits = fractionalDigits - 1;
        } while ((fractionalDigits != 0) && (fractionRemainder != 0));
        /* exact fraction before the digit limit: zero-pad to the fixed width */
        if ((fractionalDigits != 0) && ((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0)) {
          WideText_CopyCodeUnits(fractionalDigits,g_WideNumberFormatState.zeroPadding,destinationCursor);
          destinationCursor = destinationCursor + fractionalDigits;
        }
      }
    }
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *destinationCursor = 0;
    }
  }
  else {
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexPrefixLength,g_WideNumberFormatState.hexPrefix,destination
              );
    destinationCursor = destination + segmentLength;
    for (digitCount = 8; ((value & 0xff000000U) == 0 && (2 < digitCount)); digitCount = digitCount - 2) {
      value = value << 8;
    }
    do {
      integerPartOrHexDigit = (uint32_t)value >> 0x1c;
      value = value << 4;
      *destinationCursor = g_WideNumberFormatState.digitAlphabet[integerPartOrHexDigit];
      segmentLength = g_WideNumberFormatState.hexSuffixLength;
      destinationCursor = destinationCursor + 1;
      digitCount = digitCount - 1;
    } while (digitCount != 0);
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexSuffixLength,g_WideNumberFormatState.hexSuffix,destinationCursor);
    destinationCursor = destinationCursor + segmentLength;
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *destinationCursor = 0;
    }
  }
  return (int)destinationCursor - (int)destination;
}


/* Address: 0x00403010.
   Compares two NUL-terminated UTF-16 strings, ignoring the case of ASCII letters only, and returns the result
   in the flags (ZF equal, CF leftText < rightText); a string that ends first compares as equal-or-greater
   (CF clear), with ZF set only when both end together.
*/
TextCompareResult __thandor_void_preserve_eax_ecx_edx
Utf16String_CompareAsciiCaseInsensitiveFlags(uint16_t *rightText,uint16_t *leftText)

{
  uint16_t leftCodeUnit;
  uint16_t otherCodeUnit;
  uint16_t rightCodeUnit;
  TextCompareResult compareFlags;
  
  do {
    do {
      leftCodeUnit = *leftText;
      rightCodeUnit = *rightText;
      leftText++;
      rightText++;
      otherCodeUnit = rightCodeUnit;
      if ((leftCodeUnit == 0) || (otherCodeUnit = leftCodeUnit, rightCodeUnit == 0)) {
        compareFlags.equal = otherCodeUnit == 0;
        compareFlags.less = false;
        return compareFlags;
      }
    } while (leftCodeUnit == rightCodeUnit);
    /* Fold only when one side is an upper-case letter ('A'..'Z', 0x41..0x5A) and the other a lower-case one
       ('a'..'z', 0x61..0x7A); the lower-case test on leftCodeUnit has no upper bound in the original either
       (CMP EAX,0x61 / JC only), which is harmless because rightCodeUnit is then an upper-case letter. */
    if (0x40 < leftCodeUnit) {
      if (leftCodeUnit < 0x5b) {
        if ((0x60 < rightCodeUnit) && (rightCodeUnit < 0x7b)) {
          leftCodeUnit = leftCodeUnit | 0x20;
        }
      }
      else if ((((0x60 < leftCodeUnit) && (rightCodeUnit < 0x7b)) && (0x40 < rightCodeUnit)) &&
              (rightCodeUnit < 0x5b)) {
        rightCodeUnit = rightCodeUnit | 0x20;
      }
    }
  } while (leftCodeUnit == rightCodeUnit);
  return THANDOR_BITCAST(int, TextCompareResult, ((uint16_t)(leftCodeUnit < rightCodeUnit) << 8));
}


/* Address: 0x0041BAA0.
   Widens a NUL-terminated 8-bit string to UTF-16 (each byte zero-extended) into a buffer of capacityBytes
   bytes. Returns the bytes written including the terminator; if the string does not fit it is cut off
   and terminated, and CF is set with FATAL_ERROR_GENERAL_FAILURE.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
Text_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source)

{
  uint32_t remainingCapacityBytes;
  bool capacityExhausted;
  StatusResult successResult;
  StatusResult overflowResult;
  uint8_t sourceByte;

  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    /* SUB ECX,2 / JBE: fails once the capacity would reach zero, so one unit always stays unused */
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      overflowResult.failed = true;
      overflowResult.valueOrError = FATAL_ERROR_GENERAL_FAILURE;
      return overflowResult;
    }
    *destination = (uint16_t)sourceByte;
    source++;
    destination++;
  } while (sourceByte != 0);
  successResult.valueOrError = capacityBytes - remainingCapacityBytes;
  successResult.failed = false;
  return successResult;
}


/* Address: 0x00586DA0.
   Copies a NUL-terminated UTF-16 string including its terminator and returns its length in bytes, without
   the terminator.
*/
uint32_t Utf16_CopyAndReturnByteLength(uint16_t *destination,uint16_t *source)

{
  uint32_t completedByteLength;
  uint16_t copiedCodeUnit;
  uint32_t nextByteLength;
  
  nextByteLength = 0;
  do {
    completedByteLength = nextByteLength;
    copiedCodeUnit = *source;
    *destination = copiedCodeUnit;
    source++;
    destination++;
    nextByteLength = completedByteLength + 2;
  } while (copiedCodeUnit != 0);
  return completedByteLength;
}

/* Address: 0x00402D30.
   Ownership: core/text/string.
   Purpose: Copies exactly codeUnitCount UTF-16 code units from source to destination with rep movsw. Zero count
   performs no copy. The function does not append a terminator and has no scalar return. Typed parameters: p0
   codeUnitCount→UiTextCodeUnitCount_V300. Nearby but non-identical semantic domains were explicitly deferred.
*/
void __thandor_void_preserve_eax_ecx_edx
WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,uint16_t *source,uint16_t *destination)

{
  if (codeUnitCount != 0) {
    for (; codeUnitCount != 0; codeUnitCount = codeUnitCount - 1) {
      *destination = *source;
      source = source + 1;
      destination = destination + 1;
    }
  }
  return;
}

