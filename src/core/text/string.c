/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/string.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/text/string.h>

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
dword __thandor_eax_preserve_ecx_edx
WideNumber_FormatUtf16
          (WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
          WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator,
          WideNumberSignedValue32 value,word *destination)

{
  ulonglong uVar1;
  uint uVar2;
  word *source;
  WideNumberFormatCodeUnitCount WVar3;
  uint uVar4;
  UiTextCodeUnitCount codeUnitCount;
  uint uVar5;
  word *signText;
  word *source_00;
  word *pwVar6;
  
  WVar3 = g_WideNumberFormatState.hexPrefixLength;
  if ((flags & WIDE_FORMAT_HEXADECIMAL) == 0) {
    WVar3 = 0;
    if (((flags & WIDE_FORMAT_SIGNED_VALUE) != 0) &&
       (((flags & WIDE_FORMAT_SHOW_PLUS_SIGN) != 0 || (value < 0)))) {
      if (value < 0) {
        signText = g_WideNumberFormatState.negativeSign;
        value = -value;
        WVar3 = g_WideNumberFormatState.negativeSignLength;
      }
      else {
        signText = g_WideNumberFormatState.positiveSign;
        WVar3 = g_WideNumberFormatState.positiveSignLength;
      }
    }
    WideText_CopyCodeUnits(WVar3,signText,destination);
    pwVar6 = destination + WVar3;
    uVar2 = (uint)value / denominator;
    uVar5 = (uint)value % denominator;
    source = g_WideNumberFormatState.digitAlphabet;
    uVar4 = 0;
    do {
      uVar1 = (ulonglong)uVar2;
      uVar2 = uVar2 / 10;
      source = source + -1;
      uVar4 = uVar4 + 1;
      *source = (short)(uVar1 % 10) + 0x30;
    } while (uVar2 != 0);
    source_00 = g_WideNumberFormatState.spacePadding;
    if (integerDigitLimit < uVar4) {
      uVar4 = integerDigitLimit;
    }
    if ((flags & (WIDE_FORMAT_PAD_WITH_SPACE|WIDE_FORMAT_PAD_WITH_ZERO)) != 0) {
      if ((flags & WIDE_FORMAT_PAD_WITH_ZERO) != 0) {
        source_00 = g_WideNumberFormatState.zeroPadding;
      }
      WideText_CopyCodeUnits(integerDigitLimit - uVar4,source_00,pwVar6);
      pwVar6 = pwVar6 + (integerDigitLimit - uVar4);
    }
    if (((flags & WIDE_FORMAT_GROUP_THOUSANDS) != 0) && (3 < uVar4)) {
      codeUnitCount = uVar4 - 3;
      WideText_CopyCodeUnits(codeUnitCount,source,pwVar6);
      WVar3 = g_WideNumberFormatState.groupSeparatorLength;
      source = source + codeUnitCount;
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.groupSeparatorLength,g_WideNumberFormatState.groupSeparator
                 ,pwVar6 + codeUnitCount);
      pwVar6 = pwVar6 + codeUnitCount + WVar3;
      uVar4 = 3;
    }
    WideText_CopyCodeUnits(uVar4,source,pwVar6);
    WVar3 = g_WideNumberFormatState.decimalSeparatorLength;
    pwVar6 = pwVar6 + uVar4;
    if (((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0) || (denominator != 1)) {
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.decimalSeparatorLength,
                 g_WideNumberFormatState.decimalSeparator,pwVar6);
      pwVar6 = pwVar6 + WVar3;
      if (fractionalDigits != 0) {
        do {
          uVar1 = (ulonglong)uVar5;
          uVar5 = (uint)((uVar1 * 10) % (ulonglong)denominator);
          *pwVar6 = (short)((uVar1 * 10) / (ulonglong)denominator) + 0x30;
          pwVar6 = pwVar6 + 1;
          fractionalDigits = fractionalDigits - 1;
          if (fractionalDigits == 0)
          goto WideNumber_FormatUtf16_FinalizeDecimalOutputWithOptionalTerminator;
        } while (uVar5 != 0);
        if ((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0) {
          WideText_CopyCodeUnits(fractionalDigits,g_WideNumberFormatState.zeroPadding,pwVar6);
          pwVar6 = pwVar6 + fractionalDigits;
        }
      }
    }
WideNumber_FormatUtf16_FinalizeDecimalOutputWithOptionalTerminator:
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *pwVar6 = 0;
    }
  }
  else {
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexPrefixLength,g_WideNumberFormatState.hexPrefix,destination
              );
    pwVar6 = destination + WVar3;
    for (uVar4 = 8; ((value & 0xff000000U) == 0 && (2 < uVar4)); uVar4 = uVar4 - 2) {
      value = value << 8;
    }
    do {
      uVar2 = (uint)value >> 0x1c;
      value = value << 4;
      *pwVar6 = g_WideNumberFormatState.digitAlphabet[uVar2];
      WVar3 = g_WideNumberFormatState.hexSuffixLength;
      pwVar6 = pwVar6 + 1;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexSuffixLength,g_WideNumberFormatState.hexSuffix,pwVar6);
    pwVar6 = pwVar6 + WVar3;
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *pwVar6 = 0;
    }
  }
  return (int)pwVar6 - (int)destination;
}


/* Address: 0x00403010.
   Ownership: core/text/string.
   Purpose: Compares two UTF-16 streams while folding ASCII A-Z/a-z only. EAX and EDX are restored; ordering is
   returned through flags.
*/
CompareFlagsCfZf2 __thandor_void_preserve_eax_ecx_edx
Utf16String_CompareAsciiCaseInsensitiveFlags(word *rightText,word *leftText)

{
  ushort leftCodeUnit;
  ushort uVar1;
  ushort rightCodeUnit;
  CompareFlagsCfZf2 CVar2;
  
  do {
    do {
      leftCodeUnit = *leftText;
      rightCodeUnit = *rightText;
      leftText = leftText + 1;
      rightText = rightText + 1;
      uVar1 = rightCodeUnit;
      if ((leftCodeUnit == 0) || (uVar1 = leftCodeUnit, rightCodeUnit == 0)) {
        CVar2.zero = uVar1 == 0;
        CVar2.carry = false;
        return CVar2;
      }
    } while (leftCodeUnit == rightCodeUnit);
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
  return (CompareFlagsCfZf2)((ushort)(leftCodeUnit < rightCodeUnit) << 8);
}


/* Address: 0x0041BAA0.
   Ownership: core/text/string.
   Purpose: Copies a NUL-terminated narrow string into a bounded UTF-16 destination by zero-extending each byte.
   Capacity is measured in destination bytes; CF reports error 0x14. Typed parameters: p0
   capacityBytes→TextOutputCapacityBytes_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Text_CopyNarrowToUtf16Cf(TextOutputCapacityBytes capacityBytes,word *destination,byte *source)

{
  dword remainingCapacityBytes;
  bool capacityExhausted;
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  byte sourceByte;
  
  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      SVar2.carry = true;
      SVar2.valueOrError = 0x14;
      return SVar2;
    }
    *destination = (ushort)sourceByte;
    source = source + 1;
    destination = destination + 1;
  } while (sourceByte != 0);
  SVar1.valueOrError = capacityBytes - remainingCapacityBytes;
  SVar1.carry = false;
  return SVar1;
}


/* Address: 0x00586DA0.
   Ownership: core/text/string.
   Purpose: Copies a NUL-terminated UTF-16 string including its terminator. Returns the copied byte count excluding
   the two-byte terminator.
*/
dword Utf16_CopyAndReturnByteLength(word *destination,word *source)

{
  dword completedByteLength;
  word copiedCodeUnit;
  dword nextByteLength;
  
  nextByteLength = 0;
  do {
    completedByteLength = nextByteLength;
    copiedCodeUnit = *source;
    *destination = copiedCodeUnit;
    source = source + 1;
    destination = destination + 1;
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
WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,word *source,word *destination)

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

