/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/time_locale.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/system/time_locale.h>

/* Implementation ownership: platform/system/time_locale. */

/* Address: 0x005867B0.
   Ownership: platform/system/time_locale.
   Purpose: Walks all 32 timer callback slots and unregisters every non-null callback. The unregister service
   clears the callback and calls timeKillEvent for the paired WinMM timer ID.
   Local calls: TimerSystem_UnregisterPeriodic.
*/
void __thandor_preserve_eax TimerSystem_Shutdown(void)

{
  uint callbackSlotByteOffset;
  
  callbackSlotByteOffset = 0;
  do {
    if (*(int *)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) != 0) {
      TimerSystem_UnregisterPeriodic
                (*(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset)
                );
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset < 0x80);
  return;
}


/* Address: 0x00586BA0.
   Ownership: platform/system/time_locale.
   Purpose: Detects CPU features, installs nine locale/system services, queries LOCALE_USER_DEFAULT fields through
   GetLocaleInfoA, converts locale strings to UTF-16, and records date/time ordering flags.
   Local calls: Locale_ParseUnsignedDecimalAscii.
   Cross-module calls: CPU_DetectFeatures [platform/bootstrap/runtime], Text_CopyNarrowToUtf16Cf
   [core/text/string].
*/
void __thandor_void_preserve_eax_ecx_edx Locale_Init(void)

{
  CPU_DetectFeatures();
  g_LocaleFormatDateFieldsUtf16 = Locale_FormatDateFieldsUtf16;
  g_LocaleFormatCurrentDateUtf16 = Locale_FormatCurrentDateUtf16;
  g_LocaleGetPackedCurrentDate = Locale_GetPackedCurrentDate;
  g_LocaleFormatTimeFieldsUtf16 = Locale_FormatTimeFieldsUtf16;
  g_LocaleFormatCurrentTimeUtf16 = Locale_FormatCurrentTimeUtf16;
  g_LocaleGetPackedCurrentTime = Locale_GetPackedCurrentTime;
  g_LocaleGetDefaultTelephoneCountryCode = Locale_GetDefaultTelephoneCountryCode;
  g_LocaleCopyDefaultComputerLabelUtf16 = Locale_CopyDefaultComputerLabelUtf16;
  g_CPUDetectFeatures = CPU_DetectFeatures;
  GetLocaleInfoA(0x400,1,(LPSTR)g_LocaleInfoScratch,0x10);
  g_LocaleSystemState.languageIdentifierDigits =
       Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0xe,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.decimalSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0xf,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.thousandsSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x10,(LPSTR)g_LocaleInfoScratch,0x10);
  g_LocaleSystemState.negativeNumberFormat = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x51,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.numberGrouping,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x1d,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.dateSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x1e,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.timeSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x28,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.amDesignator,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x29,(LPSTR)g_LocaleInfoScratch,0x10);
  Text_CopyNarrowToUtf16Cf(0x10,g_LocaleSystemState.pmDesignator,g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x22,(LPSTR)g_LocaleInfoScratch,0x10);
  g_LocaleSystemState.longDateOrder = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(0x400,0x23,(LPSTR)g_LocaleInfoScratch,0x10);
  g_LocaleSystemState.timeFormat24Hour = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  return;
}


/* Address: 0x00402F70.
   Ownership: platform/system/time_locale.
   Purpose: Maps a telephone country code to the engine packed ASCII region tag used by locale-dependent resource
   selection. Verified mappings include generic, Germany, Great Britain, France, Denmark, Italy, Belgium, Canada,
   Netherlands, Spain, and USA, with a dash fallback.
*/
LocaleRegionTagPacked __thandor_eax_preserve_ecx_edx
Locale_MapTelephoneCountryCodeToRegionTagPacked(LocaleTelephoneCountryCode countryCode)

{
  LocaleRegionTagPacked packedRegionTag;
  
  if (countryCode == LOCALE_COUNTRY_GENERIC) {
    packedRegionTag = LOCALE_REGION_TAG_GENERIC;
  }
  else if (countryCode == LOCALE_COUNTRY_GERMANY) {
    packedRegionTag = LOCALE_REGION_TAG_GERMANY;
  }
  else if (countryCode == LOCALE_COUNTRY_GREAT_BRITAIN) {
    packedRegionTag = LOCALE_REGION_TAG_GREAT_BRITAIN;
  }
  else if (countryCode == LOCALE_COUNTRY_FRANCE) {
    packedRegionTag = LOCALE_REGION_TAG_FRANCE;
  }
  else if (countryCode == LOCALE_COUNTRY_DENMARK) {
    packedRegionTag = LOCALE_REGION_TAG_DENMARK;
  }
  else if (countryCode == LOCALE_COUNTRY_ITALY) {
    packedRegionTag = LOCALE_REGION_TAG_ITALY;
  }
  else if (countryCode == LOCALE_COUNTRY_BELGIUM) {
    packedRegionTag = LOCALE_REGION_TAG_BELGIUM;
  }
  else if (countryCode == LOCALE_COUNTRY_CANADA) {
    packedRegionTag = LOCALE_REGION_TAG_CANADA;
  }
  else if (countryCode == LOCALE_COUNTRY_NETHERLANDS) {
    packedRegionTag = LOCALE_REGION_TAG_NETHERLANDS;
  }
  else if (countryCode == LOCALE_COUNTRY_SPAIN) {
    packedRegionTag = LOCALE_REGION_TAG_SPAIN;
  }
  else if (countryCode == LOCALE_COUNTRY_USA) {
    packedRegionTag = LOCALE_REGION_TAG_USA;
  }
  else {
    packedRegionTag = LOCALE_REGION_TAG_FALLBACK_DASH;
  }
  return packedRegionTag;
}


/* Address: 0x00586790.
   Ownership: platform/system/time_locale.
   Purpose: Installs TimerSystem_RegisterPeriodic, TimerSystem_UnregisterPeriodic, and Win32_PumpMessages into the
   three fixed runtime service slots. CF is cleared.
*/
void __cdecl TimerSystem_Init(void)

{
  g_TimerRegisterPeriodic = TimerSystem_RegisterPeriodic;
  g_TimerUnregisterPeriodic = TimerSystem_UnregisterPeriodic;
  g_Win32PumpMessages = Win32_PumpMessages;
  return;
}

/* Address: 0x005867E0.
   Ownership: platform/system/time_locale.
   Purpose: WinMM TIMECALLBACK used by timeSetEvent. The dwUser value is a byte offset into
   TimerSystemState.callbacks. Aligned offsets below 0x80 dispatch the corresponding non-null engine callback
   without arguments. Typed parameters: p0 timerId→WinMmTimerId_V331, p2
   slotOffset→TimerCallbackSlotByteOffset_V331. Nearby but non-identical semantic domains were explicitly deferred.
*/
void __thandor_void_preserve_eax_ecx_edx
WinMM_TimerDispatchCallback
          (WinMmTimerId timerId,dword message,TimerCallbackSlotByteOffset slotOffset,
          dword callbackData0,dword callbackData1)

{
  if ((((slotOffset & 3) == 0) && (slotOffset < 0x80)) &&
     (*(int *)((int)g_TimerSystemState.callbacks + slotOffset) != 0)) {
    (**(code **)((int)g_TimerSystemState.callbacks + slotOffset))();
  }
  return;
}


/* Address: 0x00586820.
   Ownership: platform/system/time_locale.
   Purpose: Registers an engine callback in the first free one of 32 slots. The requested frequency is converted
   with integer division periodMs=1000/frequencyHz. timeSetEvent is called with resolution 0,
   WinMM_TimerDispatchCallback, the slot byte offset as dwUser, and TIME_PERIODIC. The returned timer ID is stored
   in the paired ID array. A full table silently leaves the request unregistered.
*/
void __thandor_void_preserve_eax_ecx_edx
TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback)

{
  WinMmTimerPeriodMilliseconds intervalMilliseconds;
  WinMmTimerId winmmTimerId;
  TimerCallbackSlotByteOffset callbackSlotSearchByteOffset;
  
  intervalMilliseconds = (WinMmTimerPeriodMilliseconds)(1000 / (ulonglong)frequencyHz);
  callbackSlotSearchByteOffset = 0;
  do {
    if (*(int *)((int)g_TimerSystemState.callbacks + callbackSlotSearchByteOffset) == 0) {
      *(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotSearchByteOffset) =
           callback;
      winmmTimerId = (*(code *)g_BootstrapApiBindings[2].destination)
                               (intervalMilliseconds,0,WinMM_TimerDispatchCallback,
                                callbackSlotSearchByteOffset,1,callbackSlotSearchByteOffset);
      *(WinMmTimerId *)((int)g_TimerSystemState.winmmTimerIds + intervalMilliseconds) = winmmTimerId
      ;
      return;
    }
    callbackSlotSearchByteOffset = callbackSlotSearchByteOffset + 4;
  } while (callbackSlotSearchByteOffset < 0x80);
  return;
}


/* Address: 0x00586DD0.
   Ownership: platform/system/time_locale.
   Purpose: Formats year, month, and day into destination using LocaleSystemState.longDateOrder and dateSeparator.
   Order 0 is month/day/year, order 1 is day/month/year, and other values are year/month/day. Returns output byte
   length excluding the final UTF-16 terminator.
   Cross-module calls: Utf16_CopyAndReturnByteLength [core/text/string].
*/
dword Locale_FormatDateFieldsUtf16
                (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month,
                LocaleCalendarDayStack32 day,word *destination)

{
  dword dVar1;
  dword dVar2;
  word *pwVar3;
  int iVar4;
  
  if (g_LocaleSystemState.longDateOrder == 0) {
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + dVar1),g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)((int)destination + dVar1) + dVar2);
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,pwVar3);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = Utf16_CopyAndReturnByteLength(pwVar3,g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,pwVar3);
    iVar4 = (int)pwVar3 + dVar1;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + dVar1),g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)((int)destination + dVar1) + dVar2);
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,pwVar3);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = Utf16_CopyAndReturnByteLength(pwVar3,g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,pwVar3);
    iVar4 = (int)pwVar3 + dVar1;
  }
  else {
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + dVar1),g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)((int)destination + dVar1) + dVar2);
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,pwVar3);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = Utf16_CopyAndReturnByteLength(pwVar3,g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,pwVar3);
    iVar4 = (int)pwVar3 + dVar1;
  }
  return iVar4 - (int)destination;
}

/* Address: 0x00586F10.
   Ownership: platform/system/time_locale.
   Purpose: Calls GetLocalTime and formats the current year, month, and day using the localized date order and
   separator. Returns output byte length excluding the final UTF-16 terminator.
   Cross-module calls: Utf16_CopyAndReturnByteLength [core/text/string].
*/
dword Locale_FormatCurrentDateUtf16(word *destination)

{
  dword currentAppendByteLength;
  dword separatorByteLength;
  dword branchAppendByteLength;
  dword branchSeparatorByteLength;
  dword dVar1;
  dword dVar2;
  word *outputCursor;
  word *branchOutputCursor;
  int completedByteOffset;
  word *pwVar3;
  
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.longDateOrder == 0) {
    currentAppendByteLength =
         (*g_WideNumberFormatUtf16)
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.month,
                    destination);
    separatorByteLength =
         Utf16_CopyAndReturnByteLength
                   ((word *)((int)destination + currentAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    outputCursor = (word *)((int)((int)destination + currentAppendByteLength) + separatorByteLength)
    ;
    branchAppendByteLength =
         (*g_WideNumberFormatUtf16)
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.day,
                    outputCursor);
    branchSeparatorByteLength =
         Utf16_CopyAndReturnByteLength
                   ((word *)((int)outputCursor + branchAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    branchOutputCursor =
         (word *)((int)((int)outputCursor + branchAppendByteLength) + branchSeparatorByteLength);
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.year,
                       branchOutputCursor);
    completedByteOffset = (int)branchOutputCursor + dVar1;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.day,
                       destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + dVar1),g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)((int)destination + dVar1) + dVar2);
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.month
                       ,pwVar3);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = Utf16_CopyAndReturnByteLength(pwVar3,g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.year,
                       pwVar3);
    completedByteOffset = (int)pwVar3 + dVar1;
  }
  else {
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.year,
                       destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + dVar1),g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)((int)destination + dVar1) + dVar2);
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.month
                       ,pwVar3);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = Utf16_CopyAndReturnByteLength(pwVar3,g_LocaleSystemState.dateSeparator);
    pwVar3 = (word *)((int)pwVar3 + dVar1);
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint)g_LocaleSystemState.localTime.day,
                       pwVar3);
    completedByteOffset = (int)pwVar3 + dVar1;
  }
  return completedByteOffset - (int)destination;
}


/* Address: 0x00587080.
   Ownership: platform/system/time_locale.
   Purpose: Calls GetLocalTime and returns (year << 16) | (month << 8) | day.
*/
dword __thandor_eax_preserve_ecx_edx Locale_GetPackedCurrentDate(void)

{
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  return (uint)g_LocaleSystemState.localTime.day | (uint)g_LocaleSystemState.localTime.month << 8 |
         (uint)g_LocaleSystemState.localTime.year << 0x10;
}


/* Address: 0x005870C0.
   Ownership: platform/system/time_locale.
   Purpose: Formats hour and minute with two-digit zero padding and the localized time separator. When LOCALE_ITIME
   is zero, hours 12-23 are reduced by 12 and the localized AM/PM designator is appended; hour zero and hour twelve
   therefore format as 00 AM and 00 PM in this implementation. Returns output byte length excluding the final
   terminator.
   Cross-module calls: Utf16_CopyAndReturnByteLength [core/text/string].
*/
dword Locale_FormatTimeFieldsUtf16
                (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,word *destination)

{
  dword dVar1;
  dword dVar2;
  word *source;
  word *pwVar3;
  int iVar4;
  
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.timeFormat24Hour == 0) {
    if (0xb < hour) {
      hour = hour - 0xc;
    }
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + dVar1),g_LocaleSystemState.timeSeparator);
    pwVar3 = (word *)((int)((int)destination + dVar1) + dVar2);
    if (minute < 10) {
      pwVar3[0] = 0x30;
      pwVar3[1] = 0;
      pwVar3 = pwVar3 + 1;
    }
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,pwVar3);
    dVar2 = Utf16_CopyAndReturnByteLength((word *)((int)pwVar3 + dVar1),source);
    iVar4 = (int)((int)pwVar3 + dVar1) + dVar2;
  }
  else {
    pwVar3 = destination;
    if (hour < 10) {
      destination[0] = 0x30;
      destination[1] = 0;
      pwVar3 = destination + 1;
    }
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,pwVar3);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)pwVar3 + dVar1),g_LocaleSystemState.timeSeparator);
    pwVar3 = (word *)((int)((int)pwVar3 + dVar1) + dVar2);
    if (minute < 10) {
      pwVar3[0] = 0x30;
      pwVar3[1] = 0;
      pwVar3 = pwVar3 + 1;
    }
    dVar1 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,pwVar3);
    iVar4 = (int)pwVar3 + dVar1;
  }
  return iVar4 - (int)destination;
}

/* Address: 0x005871B0.
   Ownership: platform/system/time_locale.
   Purpose: Calls GetLocalTime and formats the current hour and minute with the localized time separator and
   optional AM/PM designator. Returns output byte length excluding the final terminator.
   Cross-module calls: Utf16_CopyAndReturnByteLength [core/text/string].
*/
dword Locale_FormatCurrentTimeUtf16(word *destination)

{
  uint uVar1;
  dword currentAppendByteLength;
  dword dVar2;
  dword dVar3;
  word *source;
  word *outputCursor;
  int completedByteOffset;
  word *pwVar4;
  Win32Hour16 localHour;
  Win32Minute16 localMinute;
  
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.timeFormat24Hour == 0) {
    uVar1 = (uint)g_LocaleSystemState.localTime.hour;
    if (0xb < uVar1) {
      uVar1 = uVar1 - 0xc;
    }
    currentAppendByteLength =
         (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar1,destination);
    dVar2 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)destination + currentAppendByteLength),
                       g_LocaleSystemState.timeSeparator);
    outputCursor = (word *)((int)((int)destination + currentAppendByteLength) + dVar2);
    uVar1 = (uint)g_LocaleSystemState.localTime.minute;
    if (uVar1 < 10) {
      outputCursor[0] = 0x30;
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    dVar2 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar1,outputCursor);
    dVar3 = Utf16_CopyAndReturnByteLength((word *)((int)outputCursor + dVar2),source);
    completedByteOffset = (int)((int)outputCursor + dVar2) + dVar3;
  }
  else {
    uVar1 = (uint)g_LocaleSystemState.localTime.hour;
    pwVar4 = destination;
    if (uVar1 < 10) {
      destination[0] = 0x30;
      destination[1] = 0;
      pwVar4 = destination + 1;
    }
    dVar2 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar1,pwVar4);
    dVar3 = Utf16_CopyAndReturnByteLength
                      ((word *)((int)pwVar4 + dVar2),g_LocaleSystemState.timeSeparator);
    pwVar4 = (word *)((int)((int)pwVar4 + dVar2) + dVar3);
    uVar1 = (uint)g_LocaleSystemState.localTime.minute;
    if (uVar1 < 10) {
      pwVar4[0] = 0x30;
      pwVar4[1] = 0;
      pwVar4 = pwVar4 + 1;
    }
    dVar2 = (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,uVar1,pwVar4);
    completedByteOffset = (int)pwVar4 + dVar2;
  }
  return completedByteOffset - (int)destination;
}


/* Address: 0x005872B0.
   Ownership: platform/system/time_locale.
   Purpose: Calls GetLocalTime and returns (hour << 16) | (minute << 8) | second.
*/
dword __thandor_eax_preserve_ecx_edx Locale_GetPackedCurrentTime(void)

{
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  return (uint)g_LocaleSystemState.localTime.second |
         (uint)g_LocaleSystemState.localTime.minute << 8 |
         (uint)g_LocaleSystemState.localTime.hour << 0x10;
}


/* Address: 0x005872F0.
   Ownership: platform/system/time_locale.
   Purpose: Masks GetUserDefaultLCID with 0x1FF and maps English, German, French, Italian, Spanish, and Russian
   primary-language values to telephone country codes 44, 49, 33, 39, 34, and 7. Other values return zero.
*/
dword __thandor_eax_preserve_ecx_edx Locale_GetDefaultTelephoneCountryCode(void)

{
  LCID userLocaleId;
  uint primaryLanguageId;
  dword telephoneCountryCode;
  
  userLocaleId = GetUserDefaultLCID();
  primaryLanguageId = userLocaleId & 0x1ff;
  if (primaryLanguageId == 9) {
    telephoneCountryCode = 0x2c;
  }
  else if (primaryLanguageId == 7) {
    telephoneCountryCode = 0x31;
  }
  else if (primaryLanguageId == 0xc) {
    telephoneCountryCode = 0x21;
  }
  else if (primaryLanguageId == 0x10) {
    telephoneCountryCode = 0x27;
  }
  else if (primaryLanguageId == 10) {
    telephoneCountryCode = 0x22;
  }
  else if (primaryLanguageId == 0x19) {
    telephoneCountryCode = 7;
  }
  else {
    telephoneCountryCode = 0;
  }
  return telephoneCountryCode;
}


/* Address: 0x00587350.
   Ownership: platform/system/time_locale.
   Purpose: Copies exactly 0x40 bytes from the fixed UTF-16 label L"Computer", including trailing zero padding,
   into destination.
*/
void __thandor_void_preserve_eax_ecx_edx Locale_CopyDefaultComputerLabelUtf16(word *destination)

{
  int copyDwordsRemaining;
  word *sourceCursor;
  
  sourceCursor = g_DefaultComputerLabelUtf16;
  for (copyDwordsRemaining = 0x10; copyDwordsRemaining != 0;
      copyDwordsRemaining = copyDwordsRemaining + -1) {
    *(undefined4 *)destination = *(undefined4 *)sourceCursor;
    sourceCursor = sourceCursor + 2;
    destination = destination + 2;
  }
  return;
}


/* Address: 0x00586880.
   Ownership: platform/system/time_locale.
   Purpose: Finds the first timer slot whose callback pointer matches, clears that callback, and calls
   timeKillEvent with the paired timer ID. The stored timer ID is left unchanged. A missing callback is silently
   ignored.
*/
void __thandor_void_preserve_eax_ecx_edx TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback)

{
  int callbackSlotByteOffset;
  
  callbackSlotByteOffset = 0;
  do {
    if (*(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) ==
        callback) {
      *(undefined4 *)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) = 0;
      (*(code *)g_BootstrapApiBindings[3].destination)
                (*(undefined4 *)((int)g_TimerSystemState.winmmTimerIds + callbackSlotByteOffset));
      return;
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset != 0x80);
  return;
}


/* Address: 0x00586B70.
   Ownership: platform/system/time_locale.
   Purpose: Parses consecutive ASCII digits until NUL or the first non-digit. No sign, whitespace, hexadecimal, or
   overflow handling is performed.
*/
dword Locale_ParseUnsignedDecimalAscii(byte *text)

{
  dword parsedValue;
  uint digitValue;
  uint currentAsciiCode;
  
  digitValue = 0;
  parsedValue = 0;
  currentAsciiCode = digitValue;
  for (; ((currentAsciiCode._1_3_ = (undefined3)(currentAsciiCode >> 8),
          currentAsciiCode = CONCAT31(currentAsciiCode._1_3_,*text), currentAsciiCode != 0 &&
          (currentAsciiCode = currentAsciiCode - 0x30, 0x2f < currentAsciiCode)) &&
         (currentAsciiCode < 10)); text = text + 1) {
    parsedValue = parsedValue * 10 + currentAsciiCode;
  }
  return parsedValue;
}
