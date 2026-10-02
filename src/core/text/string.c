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
   Formats a 32-bit number as UTF-16 text with the locale strings of g_WideNumberFormatState and returns the
   length written in bytes (without the NUL that WIDE_FORMAT_WRITE_TERMINATOR adds). Decimal mode prints
   value / denominator with optional sign, padding to integerDigitLimit, one group separator before the last
   three digits and up to fractionalDigits fraction digits (denominator must not be 0); hexadecimal mode prints
   prefix, 2/4/6/8 digits without leading zero bytes, and suffix.
*/
uint32_t WideNumber_FormatUtf16(WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
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
    /* without a sign signText stays unset, but then zero code units are copied from it */
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
    /* the integer digits are built backwards in the scratch array just before digitAlphabet */
    source = g_WideNumberFormatState.digitAlphabet;
    digitCount = 0;
    do {
      previousValue = (uint64_t)integerPartOrHexDigit;
      integerPartOrHexDigit = integerPartOrHexDigit / 10;
      source--;
      digitCount++;
      *source = (short)(previousValue % 10) + '0';
    } while (integerPartOrHexDigit != 0);
    /* only the integerDigitLimit most significant digits are printed */
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
          *destinationCursor = (short)((previousValue * 10) / (uint64_t)denominator) + '0';
          destinationCursor++;
          fractionalDigits--;
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
              (g_WideNumberFormatState.hexPrefixLength,g_WideNumberFormatState.hexPrefix,destination);
    destinationCursor = destination + segmentLength;
    /* drop leading zero bytes, keeping at least one byte (two digits) */
    for (digitCount = 8; (value & 0xff000000U) == 0 && digitCount > 2; digitCount -= 2) {
      value = value << 8;
    }
    do {
      integerPartOrHexDigit = (uint32_t)value >> 28;
      value = value << 4;
      *destinationCursor = g_WideNumberFormatState.digitAlphabet[integerPartOrHexDigit];
      segmentLength = g_WideNumberFormatState.hexSuffixLength;
      destinationCursor++;
      digitCount--;
    } while (digitCount != 0);
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexSuffixLength,g_WideNumberFormatState.hexSuffix,destinationCursor);
    destinationCursor = destinationCursor + segmentLength;
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *destinationCursor = 0;
    }
  }
  return (uint8_t *)destinationCursor - (uint8_t *)destination;
}


/* Address: 0x00403010.
   Compares two NUL-terminated UTF-16 strings, ignoring the case of ASCII letters only, and returns the order
   of leftText relative to rightText: -1 when less, 0 when equal, 1 when greater. A string that ends first
   compares as greater (1), equal (0) only when both end together. (The original returned the order in ZF/CF;
   the name keeps "Flags" because the generated image data refers to it.)
*/
int Utf16String_CompareAsciiCaseInsensitiveFlags(uint16_t *rightText,uint16_t *leftText)

{
  uint16_t leftCodeUnit;
  uint16_t otherCodeUnit;
  uint16_t rightCodeUnit;

  do {
    do {
      leftCodeUnit = *leftText;
      rightCodeUnit = *rightText;
      leftText++;
      rightText++;
      otherCodeUnit = rightCodeUnit;
      if ((leftCodeUnit == 0) || (otherCodeUnit = leftCodeUnit, rightCodeUnit == 0)) {
        return otherCodeUnit == 0 ? 0 : 1;
      }
    } while (leftCodeUnit == rightCodeUnit);
    /* Fold only when one side is an upper-case letter ('A'..'Z', 0x41..0x5A) and the other a lower-case one
       ('a'..'z', 0x61..0x7A); the lower-case test on leftCodeUnit has no upper bound in the original either
       (CMP EAX,0x61 / JC only), which is harmless because rightCodeUnit is then an upper-case letter. */
    if ('A' - 1 < leftCodeUnit) {
      if (leftCodeUnit < 'Z' + 1) {
        if ('a' - 1 < rightCodeUnit && rightCodeUnit < 'z' + 1) {
          leftCodeUnit = leftCodeUnit | TEXT_ASCII_LOWER_CASE_BIT;
        }
      }
      else if ('a' - 1 < leftCodeUnit && rightCodeUnit < 'z' + 1 && 'A' - 1 < rightCodeUnit &&
               rightCodeUnit < 'Z' + 1) {
        rightCodeUnit = rightCodeUnit | TEXT_ASCII_LOWER_CASE_BIT;
      }
    }
  } while (leftCodeUnit == rightCodeUnit);
  return leftCodeUnit < rightCodeUnit ? -1 : 1;
}


/* Address: 0x0041BAA0.
   Widens a NUL-terminated 8-bit string to UTF-16 (each byte zero-extended) into a buffer of capacityBytes
   bytes. Returns the bytes written including the terminator (always at least 2); if the string does not
   fit it is cut off and terminated, and 0 is returned (the original reported FATAL_ERROR_GENERAL_FAILURE
   with CF set).
*/
uint32_t Text_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source)

{
  uint32_t remainingCapacityBytes;
  bool capacityExhausted;
  uint8_t sourceByte;

  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    /* SUB ECX,2 / JBE: fails once the capacity would reach zero, so one unit always stays unused */
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      return 0;
    }
    *destination = (uint16_t)sourceByte;
    source++;
    destination++;
  } while (sourceByte != 0);
  return capacityBytes - remainingCapacityBytes;
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
   Copies exactly codeUnitCount UTF-16 code units from source to destination (rep movsw in the original).
   It appends no terminator, so the number formatter can splice digit runs into a larger buffer.
*/
void WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,uint16_t *source,uint16_t *destination)

{
  if (codeUnitCount != 0) {
    for (; codeUnitCount != 0; codeUnitCount--) {
      *destination = *source;
      source++;
      destination++;
    }
  }
  return;
}

