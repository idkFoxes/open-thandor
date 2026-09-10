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
dword WideNumber_FormatUtf16
                (WideNumberFormatFlags flags,WideNumberFractionalDigitCount fractionalDigits,
                WideNumberIntegerDigitLimit integerDigitLimit,WideNumberDenominator32 denominator,
                WideNumberSignedValue32 value,word *destination)

{
  ulonglong uVar1;
  uint extraout_EAX;
  uint uVar2;
  word *extraout_EAX_00;
  word *extraout_EAX_01;
  word *source;
  uint extraout_EAX_02;
  WideNumberFormatCodeUnitCount codeUnitCount;
  int extraout_ECX;
  uint uVar3;
  uint extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  uint uVar4;
  int extraout_EDX;
  int extraout_EDX_00;
  word *unaff_ESI;
  word *source_00;
  word *pwVar5;
  
  if ((flags & WIDE_FORMAT_HEXADECIMAL) == 0) {
    codeUnitCount = 0;
    if (((flags & WIDE_FORMAT_SIGNED_VALUE) != 0) &&
       (((flags & WIDE_FORMAT_SHOW_PLUS_SIGN) != 0 || (value < 0)))) {
      if (value < 0) {
        unaff_ESI = g_WideNumberFormatState.negativeSign;
        codeUnitCount = g_WideNumberFormatState.negativeSignLength;
      }
      else {
        unaff_ESI = g_WideNumberFormatState.positiveSign;
        codeUnitCount = g_WideNumberFormatState.positiveSignLength;
      }
    }
    WideText_CopyCodeUnits(codeUnitCount,unaff_ESI,destination);
    pwVar5 = destination + extraout_ECX;
    uVar2 = extraout_EAX / denominator;
    uVar4 = extraout_EAX % denominator;
    source = g_WideNumberFormatState.digitAlphabet;
    uVar3 = 0;
    do {
      uVar1 = (ulonglong)uVar2;
      uVar2 = uVar2 / 10;
      source = source + -1;
      uVar3 = uVar3 + 1;
      *source = (short)(uVar1 % 10) + 0x30;
    } while (uVar2 != 0);
    source_00 = g_WideNumberFormatState.spacePadding;
    if (integerDigitLimit < uVar3) {
      uVar3 = integerDigitLimit;
    }
    if ((flags & (WIDE_FORMAT_PAD_WITH_SPACE|WIDE_FORMAT_PAD_WITH_ZERO)) != 0) {
      if ((flags & WIDE_FORMAT_PAD_WITH_ZERO) != 0) {
        source_00 = g_WideNumberFormatState.zeroPadding;
      }
      WideText_CopyCodeUnits(integerDigitLimit - uVar3,source_00,pwVar5);
      pwVar5 = pwVar5 + extraout_EDX;
      source = extraout_EAX_00;
      uVar3 = extraout_ECX_00;
    }
    if (((flags & WIDE_FORMAT_GROUP_THOUSANDS) != 0) && (3 < uVar3)) {
      WideText_CopyCodeUnits(uVar3 - 3,source,pwVar5);
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.groupSeparatorLength,g_WideNumberFormatState.groupSeparator
                 ,pwVar5 + extraout_ECX_01);
      pwVar5 = pwVar5 + extraout_ECX_01 + extraout_EDX_00;
      uVar3 = 3;
      source = extraout_EAX_01;
    }
    WideText_CopyCodeUnits(uVar3,source,pwVar5);
    pwVar5 = pwVar5 + extraout_ECX_02;
    if (((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0) || (denominator != 1)) {
      WideText_CopyCodeUnits
                (g_WideNumberFormatState.decimalSeparatorLength,
                 g_WideNumberFormatState.decimalSeparator,pwVar5);
      pwVar5 = pwVar5 + extraout_ECX_03;
      if (fractionalDigits != 0) {
        do {
          uVar1 = (ulonglong)uVar4;
          uVar4 = (uint)((uVar1 * 10) % (ulonglong)denominator);
          *pwVar5 = (short)((uVar1 * 10) / (ulonglong)denominator) + 0x30;
          pwVar5 = pwVar5 + 1;
          fractionalDigits = fractionalDigits - 1;
          if (fractionalDigits == 0)
          goto WideNumber_FormatUtf16_FinalizeDecimalOutputWithOptionalTerminator;
        } while (uVar4 != 0);
        if ((flags & WIDE_FORMAT_FIXED_FRACTION_WIDTH) != 0) {
          WideText_CopyCodeUnits(fractionalDigits,g_WideNumberFormatState.zeroPadding,pwVar5);
          pwVar5 = pwVar5 + extraout_ECX_04;
        }
      }
    }
WideNumber_FormatUtf16_FinalizeDecimalOutputWithOptionalTerminator:
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *pwVar5 = 0;
    }
  }
  else {
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexPrefixLength,g_WideNumberFormatState.hexPrefix,destination
              );
    pwVar5 = destination + extraout_ECX_05;
    uVar3 = extraout_EAX_02;
    for (uVar2 = 8; ((uVar3 & 0xff000000) == 0 && (2 < uVar2)); uVar2 = uVar2 - 2) {
      uVar3 = uVar3 << 8;
    }
    do {
      uVar4 = uVar3 >> 0x1c;
      uVar3 = uVar3 << 4;
      *pwVar5 = g_WideNumberFormatState.digitAlphabet[uVar4];
      pwVar5 = pwVar5 + 1;
      uVar2 = uVar2 - 1;
    } while (uVar2 != 0);
    WideText_CopyCodeUnits
              (g_WideNumberFormatState.hexSuffixLength,g_WideNumberFormatState.hexSuffix,pwVar5);
    pwVar5 = pwVar5 + extraout_ECX_06;
    if ((flags & WIDE_FORMAT_WRITE_TERMINATOR) != 0) {
      *pwVar5 = 0;
    }
  }
  return (int)pwVar5 - (int)destination;
}

/* Address: 0x00403010.
   Ownership: core/text/string.
   Purpose: Compares two UTF-16 streams while folding ASCII A-Z/a-z only. EAX and EDX are restored; ordering is
   returned through flags.
*/
undefined8 Utf16String_CompareAsciiCaseInsensitiveFlags(word *rightText,word *leftText)

{
  ushort leftCodeUnit;
  undefined4 in_EAX;
  ushort rightCodeUnit;
  undefined4 in_EDX;
  
  do {
    do {
      leftCodeUnit = *leftText;
      rightCodeUnit = *rightText;
      leftText = leftText + 1;
      rightText = rightText + 1;
      if ((leftCodeUnit == 0) || (rightCodeUnit == 0)) {
        return CONCAT44(in_EDX,in_EAX);
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
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0041BAA0.
   Ownership: core/text/string.
   Purpose: Copies a NUL-terminated narrow string into a bounded UTF-16 destination by zero-extending each byte.
   Capacity is measured in destination bytes; CF reports error 0x14. Typed parameters: p0
   capacityBytes→TextOutputCapacityBytes_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
int Text_CopyNarrowToUtf16Cf(TextOutputCapacityBytes capacityBytes,word *destination,byte *source)

{
  dword remainingCapacityBytes;
  bool capacityExhausted;
  byte sourceByte;
  
  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      return 0x14;
    }
    *destination = (ushort)sourceByte;
    source = source + 1;
    destination = destination + 1;
  } while (sourceByte != 0);
  return capacityBytes - remainingCapacityBytes;
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
void WideText_CopyCodeUnits(UiTextCodeUnitCount codeUnitCount,word *source,word *destination)

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
