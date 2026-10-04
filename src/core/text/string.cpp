/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/string.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/text/string.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(8) WideNumberFormatUtf16Proc *g_WideNumberFormatUtf16 = THANDOR_FN(WideNumber_FormatUtf16);

static WideNumberFormatState g_WideNumberFormatState = {
    .decimalSeparatorLength = 1,
    .groupSeparatorLength = 1,
    .positiveSignLength = 1,
    .negativeSignLength = 1,
    .hexPrefixLength = 2,
    .decimalSeparator = {44},
    .groupSeparator = {46},
    .positiveSign = {43},
    .negativeSign = {45},
    .hexPrefix = {'0', 'x'},
    .spacePadding = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
    .zeroPadding = {48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48},
    .digitAlphabet = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70}};

/* Implementation ownership: core/text/string. */

/* Formats a 32-bit number as UTF-16 text with the locale strings of g_WideNumberFormatState and returns the
   length written in bytes (without the NUL that WIDE_FORMAT_WRITE_TERMINATOR adds). Decimal mode prints
   value / denominator with optional sign, padding to integerDigitLimit, one group separator before the last
   three digits and up to fractionalDigits fraction digits (denominator must not be 0); hexadecimal mode prints
   prefix, 2/4/6/8 digits without leading zero bytes, and suffix.
*/
uint32_t WideNumber_FormatUtf16(WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
          WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator,
          WideNumberSignedValue32 value,uint16_t *destination)

{
  uint32_t integerPart;
  uint32_t fractionRemainder;
  uint64_t scaledRemainder;
  uint32_t hexBits;
  uint32_t hexDigit;
  uint32_t digitCount;
  uint32_t leadingDigitCount;
  WideNumberFormatCodeUnitCount signLength;
  uint16_t *signText;
  uint16_t *paddingText;
  uint16_t *digitText;
  uint16_t *destinationCursor;

  if ((flags & WIDE_FORMAT_HEXADECIMAL) != 0) {
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexPrefixLength,g_WideNumberFormatState.hexPrefix,destination);
    destinationCursor = destination + g_WideNumberFormatState.hexPrefixLength;
    /* drop leading zero bytes, keeping at least one byte (two digits) */
    hexBits = (uint32_t)value;
    for (digitCount = 8; (hexBits & 0xff000000U) == 0 && digitCount > 2; digitCount -= 2) {
      hexBits = hexBits << 8;
    }
    for (; digitCount != 0; digitCount--) {
      hexDigit = hexBits >> 28;
      hexBits = hexBits << 4;
      *destinationCursor = g_WideNumberFormatState.digitAlphabet[hexDigit];
      destinationCursor++;
    }
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexSuffixLength,g_WideNumberFormatState.hexSuffix,destinationCursor);
    destinationCursor = destinationCursor + g_WideNumberFormatState.hexSuffixLength;
  }
  else {
    /* without a sign zero code units are copied, so signText is never read */
    signText = nullptr;
    signLength = 0;
    if ((flags & WIDE_FORMAT_SIGNED_VALUE) != 0) {
      if (value < 0) {
        signText = g_WideNumberFormatState.negativeSign;
        signLength = g_WideNumberFormatState.negativeSignLength;
        /* the original negates with wrap-around (INT_MIN stays 0x80000000, printed as 2147483648) */
        value = (WideNumberSignedValue32)(0u - (uint32_t)value);
      }
      else if ((flags & WIDE_FORMAT_SHOW_PLUS_SIGN) != 0) {
        signText = g_WideNumberFormatState.positiveSign;
        signLength = g_WideNumberFormatState.positiveSignLength;
      }
    }
    WideText_CopyCodeUnits(signLength,signText,destination);
    destinationCursor = destination + signLength;
    integerPart = (uint32_t)value / denominator;
    fractionRemainder = (uint32_t)value % denominator;
    /* the integer digits are built backwards in the scratch array just before digitAlphabet */
    digitText = g_WideNumberFormatState.digitAlphabet;
    digitCount = 0;
    do {
      digitText--;
      *digitText = (uint16_t)('0' + integerPart % 10);
      integerPart = integerPart / 10;
      digitCount++;
    } while (integerPart != 0);
    /* only the integerDigitLimit most significant digits are printed */
    if (integerDigitLimit < digitCount) {
      digitCount = integerDigitLimit;
    }
    if ((flags & (WIDE_FORMAT_PAD_WITH_SPACE|WIDE_FORMAT_PAD_WITH_ZERO)) != 0) {
      if ((flags & WIDE_FORMAT_PAD_WITH_ZERO) != 0) {
        paddingText = g_WideNumberFormatState.zeroPadding;
      }
      else {
        paddingText = g_WideNumberFormatState.spacePadding;
      }
      WideText_CopyCodeUnits(integerDigitLimit - digitCount,paddingText,destinationCursor);
      destinationCursor = destinationCursor + (integerDigitLimit - digitCount);
    }
    /* a single group separator, before the last three digits */
    if (((flags & WIDE_FORMAT_GROUP_THOUSANDS) != 0) && (3 < digitCount)) {
      leadingDigitCount = digitCount - 3;
      WideText_CopyCodeUnits(leadingDigitCount,digitText,destinationCursor);
      destinationCursor = destinationCursor + leadingDigitCount;
      digitText = digitText + leadingDigitCount;
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.groupSeparatorLength,g_WideNumberFormatState.groupSeparator,
                 destinationCursor);
      destinationCursor = destinationCursor + g_WideNumberFormatState.groupSeparatorLength;
      digitCount = 3;
    }
    WideText_CopyCodeUnits(digitCount,digitText,destinationCursor);
    destinationCursor = destinationCursor + digitCount;
    if (((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0) || (denominator != 1)) {
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.decimalSeparatorLength,
                 g_WideNumberFormatState.decimalSeparator,destinationCursor);
      destinationCursor = destinationCursor + g_WideNumberFormatState.decimalSeparatorLength;
      /* long division: at least one fraction digit, then stop early once the fraction is exact */
      if (fractionalDigits != 0) {
        do {
          scaledRemainder = (uint64_t)fractionRemainder * 10;
          *destinationCursor = (uint16_t)('0' + scaledRemainder / denominator);
          fractionRemainder = (uint32_t)(scaledRemainder % denominator);
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
  }
  if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
    *destinationCursor = 0;
  }
  return (uint8_t *)destinationCursor - (uint8_t *)destination;
}


/* Compares two NUL-terminated UTF-16 strings, ignoring the case of ASCII letters only, and returns the order
   of leftText relative to rightText: -1 when less, 0 when equal, 1 when greater. A string that ends first
   compares as greater (1), equal (0) only when both end together. (The original returned the order in CPU
   flags; the name keeps "Flags" because the generated image data refers to it.)
*/
int Utf16String_CompareAsciiCaseInsensitiveFlags(uint16_t *rightText,uint16_t *leftText)

{
  uint16_t leftCodeUnit;
  uint16_t rightCodeUnit;

  do {
    leftCodeUnit = *leftText;
    rightCodeUnit = *rightText;
    leftText++;
    rightText++;
    if (leftCodeUnit == 0) {
      return rightCodeUnit == 0 ? 0 : 1;
    }
    if (rightCodeUnit == 0) {
      return 1;
    }
    /* Fold only when one side is an upper-case letter ('A'..'Z', 0x41..0x5A) and the other a lower-case one
       ('a'..'z', 0x61..0x7A), so equal code units are never changed; the lower-case test on leftCodeUnit has
       no upper bound in the original either, which is harmless because rightCodeUnit is then an upper-case
       letter. */
    if (leftCodeUnit >= 'A' && leftCodeUnit <= 'Z') {
      if (rightCodeUnit >= 'a' && rightCodeUnit <= 'z') {
        leftCodeUnit = leftCodeUnit | TEXT_ASCII_LOWER_CASE_BIT;
      }
    }
    else if (leftCodeUnit >= 'a' && rightCodeUnit >= 'A' && rightCodeUnit <= 'Z') {
      rightCodeUnit = rightCodeUnit | TEXT_ASCII_LOWER_CASE_BIT;
    }
  } while (leftCodeUnit == rightCodeUnit);
  return leftCodeUnit < rightCodeUnit ? -1 : 1;
}


/* Widens a NUL-terminated 8-bit string to UTF-16 (each byte zero-extended) into a buffer of capacityBytes
   bytes. Returns the bytes written including the terminator (always at least 2); if the string does not
   fit it is cut off and terminated, and 0 is returned (the original reported FATAL_ERROR_GENERAL_FAILURE
   as a failure).
*/
uint32_t Text_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source)

{
  uint16_t *destinationStart;
  uint32_t remainingCapacityBytes;
  Bool8 capacityExhausted;
  uint8_t sourceByte;

  destinationStart = destination;
  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    /* fails once the remaining capacity would reach zero or below, so one unit always stays unused */
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      /* The original writes destination[-1] here, before the buffer when nothing was copied yet
         (capacityBytes <= 2); bounded here because of that: an empty result when there is room for it. */
      if (destination != destinationStart) {
        destination[-1] = 0;
      }
      else if (capacityBytes >= 2) {
        destination[0] = 0;
      }
      return 0;
    }
    *destination = (uint16_t)sourceByte;
    source++;
    destination++;
  } while (sourceByte != 0);
  return capacityBytes - remainingCapacityBytes;
}


/* Copies a NUL-terminated UTF-16 string including its terminator and returns its length in bytes, without
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

/* Copies exactly codeUnitCount UTF-16 code units from source to destination (rep movsw in the original).
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

