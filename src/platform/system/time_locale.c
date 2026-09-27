/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/time_locale.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/system/time_locale.h>
#include <thandor/thandor.h>

/* Implementation ownership: platform/system/time_locale. */

/* Address: 0x005867B0.
   Stops every periodic timer: unregisters each callback still present in the 32 slots (which also
   kills its WinMM timer).
*/
void __thandor_preserve_eax TimerSystem_Shutdown(void)

{
  uint32_t callbackSlotByteOffset;

  /* the slots are walked by byte offset, as the WinMM dwUser values are */
  callbackSlotByteOffset = 0;
  do {
    if (*(int *)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) != 0) {
      TimerSystem_UnregisterPeriodic
                (*(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset));
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset < sizeof g_TimerSystemState.callbacks);
  return;
}


/* Address: 0x00586BA0.
   Detects the CPU features, installs the date/time/locale services in their function pointers and
   caches the user's locale settings (language id, number separators, date/time separators and order,
   AM/PM designators) in g_LocaleSystemState for the date and number formatters.
   Numeric fields are parsed from the GetLocaleInfoA text; string fields are widened to UTF-16.
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
  /* The string copies pass an output capacity of 0x10 bytes (8 UTF-16 units), although every
     string field of g_LocaleSystemState holds 16 units. */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ILANGUAGE,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.languageIdentifierDigits =
       Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.decimalSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_STHOUSAND,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.thousandsSeparator,g_LocaleInfoScratch);
  /* LOCALE_SGROUPING is text like "3;0"; only the leading group size is kept */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SGROUPING,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.digitGroupingSize = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SNEGATIVESIGN,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.negativeSign,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SDATE,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.dateSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_STIME,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.timeSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_S1159,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.amDesignator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_S2359,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(0x10,g_LocaleSystemState.pmDesignator,g_LocaleInfoScratch);
  /* 0 = month-day-year, 1 = day-month-year, 2 = year-month-day */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ILDATE,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.longDateOrder = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  /* 0 = 12-hour clock with AM/PM, 1 = 24-hour clock */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ITIME,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.timeFormat24Hour = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
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
   Installs the WinMM periodic-timer services and the Win32 message pump in their function pointers.
   It cannot fail: the original returns with CF clear, which ProcessEntry relies on.
*/
void __cdecl TimerSystem_Init(void)

{
  g_TimerRegisterPeriodic = (TimerRegisterPeriodicProc *)TimerSystem_RegisterPeriodic;
  g_TimerUnregisterPeriodic = (TimerUnregisterPeriodicProc *)TimerSystem_UnregisterPeriodic;
  g_Win32PumpMessages = Win32_PumpMessages;
}

/* Address: 0x005867E0.
   The WinMM timer procedure every periodic timer of TimerSystem_RegisterPeriodic runs through, on WinMM's
   timer thread: dwUser (slotOffset) is the byte offset of the timer's callbacks[] slot, and a valid, occupied
   slot's engine callback is called without arguments.
*/
void __stdcall WinMM_TimerDispatchCallback
          (WinMmTimerId timerId,uint32_t message,TimerCallbackSlotByteOffset slotOffset,
          uint32_t reserved1,uint32_t reserved2)

{
  if ((((slotOffset & 3) == 0) && (slotOffset < sizeof g_TimerSystemState.callbacks)) &&
     (*(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + slotOffset) != NULL)) {
    /* slotOffset is the byte offset of the callbacks[] entry */
    (**(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + slotOffset))();
  }
  return;
}


/* Address: 0x00586820.
   Starts a periodic timer: puts callback into the first free of the 32 slots and has WinMM call it
   frequencyHz times per second (period 1000 / frequencyHz ms, truncated) through
   WinMM_TimerDispatchCallback, on WinMM's timer thread. With all slots taken the request is ignored.
*/
void __thandor_void_preserve_eax_ecx_edx
TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback)

{
  WinMmTimerPeriodMilliseconds intervalMilliseconds;
  WinMmTimerId winmmTimerId;
  TimerCallbackSlotByteOffset callbackSlotByteOffset;

  intervalMilliseconds = (WinMmTimerPeriodMilliseconds)(1000 / (uint64_t)frequencyHz);
  callbackSlotByteOffset = 0;
  do {
    if (*(int *)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) == 0) {
      *(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) = callback;
      /* binding 2 is timeSetEvent; dwUser is the slot's byte offset */
      winmmTimerId = ((BootstrapTimeSetEventProc)g_BootstrapApiBindings[2].destination)
                               (intervalMilliseconds,0,WinMM_TimerDispatchCallback,
                                callbackSlotByteOffset,TIME_PERIODIC);
      /* Ghidra showed a stale 6th argument and indexed the ID array by intervalMilliseconds;
         the ID pairs with the callback slot (see TimerSystem_UnregisterPeriodic). */
      *(WinMmTimerId *)((int)g_TimerSystemState.winmmTimerIds + callbackSlotByteOffset) = winmmTimerId;
      return;
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset < sizeof g_TimerSystemState.callbacks);
  return;
}


/* Address: 0x00586DD0.
   Writes a date as UTF-16 text in the user's order (LOCALE_ILDATE: 0 month-day-year, 1 day-month-year,
   else year-month-day) with the user's date separator, without zero padding. Returns the byte length
   without the terminator.
*/
uint32_t Locale_FormatDateFieldsUtf16
                (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month,
                LocaleCalendarDayStack32 day,uint16_t *destination)

{
  uint32_t appendByteLength;
  uint32_t separatorByteLength;
  uint16_t *outputCursor;
  int completedByteOffset;
  
  if (g_LocaleSystemState.longDateOrder == 0) {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)((int)destination + appendByteLength) + separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,outputCursor);
    outputCursor = (uint16_t *)((int)outputCursor + appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)outputCursor + appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)((int)destination + appendByteLength) + separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,outputCursor);
    outputCursor = (uint16_t *)((int)outputCursor + appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)outputCursor + appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  else {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)((int)destination + appendByteLength) + separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,outputCursor);
    outputCursor = (uint16_t *)((int)outputCursor + appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)outputCursor + appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  return completedByteOffset - (int)destination;
}

/* Address: 0x00586F10.
   Writes today's local date like Locale_FormatDateFieldsUtf16 (user's order and separator). Returns the
   byte length without the terminator.
*/
uint32_t Locale_FormatCurrentDateUtf16(uint16_t *destination)

{
  uint32_t currentAppendByteLength;
  uint32_t separatorByteLength;
  uint32_t branchAppendByteLength;
  uint32_t branchSeparatorByteLength;
  uint32_t fieldByteLength;
  uint32_t dateSeparatorByteLength;
  uint16_t *outputCursor;
  uint16_t *branchOutputCursor;
  int completedByteOffset;
  uint16_t *fieldCursor;
  
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.longDateOrder == 0) {
    currentAppendByteLength =
         g_WideNumberFormatUtf16
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month,
                    destination);
    separatorByteLength =
         Utf16_CopyAndReturnByteLength
                   ((uint16_t *)((int)destination + currentAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((int)((int)destination + currentAppendByteLength) + separatorByteLength)
    ;
    branchAppendByteLength =
         g_WideNumberFormatUtf16
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                    outputCursor);
    branchSeparatorByteLength =
         Utf16_CopyAndReturnByteLength
                   ((uint16_t *)((int)outputCursor + branchAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    branchOutputCursor =
         (uint16_t *)((int)((int)outputCursor + branchAppendByteLength) + branchSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.year,
                       branchOutputCursor);
    completedByteOffset = (int)branchOutputCursor + fieldByteLength;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                       destination);
    dateSeparatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + fieldByteLength),g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((int)((int)destination + fieldByteLength) + dateSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month
                       ,fieldCursor);
    fieldCursor = (uint16_t *)((int)fieldCursor + fieldByteLength);
    fieldByteLength = Utf16_CopyAndReturnByteLength(fieldCursor,g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((int)fieldCursor + fieldByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.year,
                       fieldCursor);
    completedByteOffset = (int)fieldCursor + fieldByteLength;
  }
  else {
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.year,
                       destination);
    dateSeparatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + fieldByteLength),g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((int)((int)destination + fieldByteLength) + dateSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month
                       ,fieldCursor);
    fieldCursor = (uint16_t *)((int)fieldCursor + fieldByteLength);
    fieldByteLength = Utf16_CopyAndReturnByteLength(fieldCursor,g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((int)fieldCursor + fieldByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                       fieldCursor);
    completedByteOffset = (int)fieldCursor + fieldByteLength;
  }
  return completedByteOffset - (int)destination;
}


/* Address: 0x00587080.
   Returns today's local date packed as (year << 16) | (month << 8) | day, so packed dates compare in
   calendar order.
*/
uint32_t __thandor_eax_preserve_ecx_edx Locale_GetPackedCurrentDate(void)

{
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  return (uint32_t)g_LocaleSystemState.localTime.day | (uint32_t)g_LocaleSystemState.localTime.month << 8 |
         (uint32_t)g_LocaleSystemState.localTime.year << 16;
}


/* Address: 0x005870C0.
   Writes hour:minute as UTF-16 text with the user's time separator. 24-hour locales get both fields
   zero-padded; 12-hour locales (LOCALE_ITIME 0) get an unpadded hour 0..11 and a designator appended.
   The designator is swapped in the original: hours below 12 get the S2359 (PM) text, the others the
   S1159 (AM) text, and noon shows as 0. Returns the byte length without the terminator.
*/
uint32_t Locale_FormatTimeFieldsUtf16
                (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,uint16_t *destination)

{
  uint32_t appendByteLength;
  uint32_t separatorByteLength;
  uint16_t *designatorText;
  uint16_t *outputCursor;
  int completedByteOffset;

  /* the local time is fetched but not used */
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.timeFormat24Hour == 0) {
    /* The decompiler dropped the designator selection (ECX in the original); note the original
       picks the field exported as pmDesignator for hours below 12. */
    designatorText = g_LocaleSystemState.pmDesignator;
    if (11 < hour) {
      designatorText = g_LocaleSystemState.amDesignator;
      hour = hour - 12;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((int)((int)destination + appendByteLength) + separatorByteLength);
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength((uint16_t *)((int)outputCursor + appendByteLength),designatorText);
    completedByteOffset = (int)((int)outputCursor + appendByteLength) + separatorByteLength;
  }
  else {
    outputCursor = destination;
    if (hour < 10) {
      destination[0] = '0';
      destination[1] = 0;
      outputCursor = destination + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)outputCursor + appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((int)((int)outputCursor + appendByteLength) + separatorByteLength);
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  return completedByteOffset - (int)destination;
}

/* Address: 0x005871B0.
   Writes the current local time like Locale_FormatTimeFieldsUtf16 (same padding and the same swapped
   AM/PM designators). Returns the byte length without the terminator.
*/
uint32_t Locale_FormatCurrentTimeUtf16(uint16_t *destination)

{
  uint32_t hourOrMinute;
  uint32_t currentAppendByteLength;
  uint32_t appendByteLength;
  uint32_t separatorByteLength;
  uint16_t *designatorText;
  uint16_t *outputCursor;
  int completedByteOffset;
  uint16_t *timeCursor;

  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.timeFormat24Hour == 0) {
    hourOrMinute = (uint32_t)g_LocaleSystemState.localTime.hour;
    /* See Locale_FormatTimeFieldsUtf16: the designator selection was lost in decompilation. */
    designatorText = g_LocaleSystemState.pmDesignator;
    if (11 < hourOrMinute) {
      designatorText = g_LocaleSystemState.amDesignator;
      hourOrMinute = hourOrMinute - 12;
    }
    currentAppendByteLength =
         g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hourOrMinute,destination);
    appendByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)destination + currentAppendByteLength),
                       g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((int)((int)destination + currentAppendByteLength) + appendByteLength);
    hourOrMinute = (uint32_t)g_LocaleSystemState.localTime.minute;
    if (hourOrMinute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hourOrMinute,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength((uint16_t *)((int)outputCursor + appendByteLength),designatorText);
    completedByteOffset = (int)((int)outputCursor + appendByteLength) + separatorByteLength;
  }
  else {
    hourOrMinute = (uint32_t)g_LocaleSystemState.localTime.hour;
    timeCursor = destination;
    if (hourOrMinute < 10) {
      destination[0] = '0';
      destination[1] = 0;
      timeCursor = destination + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hourOrMinute,timeCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((int)timeCursor + appendByteLength),g_LocaleSystemState.timeSeparator);
    timeCursor = (uint16_t *)((int)((int)timeCursor + appendByteLength) + separatorByteLength);
    hourOrMinute = (uint32_t)g_LocaleSystemState.localTime.minute;
    if (hourOrMinute < 10) {
      timeCursor[0] = '0';
      timeCursor[1] = 0;
      timeCursor = timeCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hourOrMinute,timeCursor);
    completedByteOffset = (int)timeCursor + appendByteLength;
  }
  return completedByteOffset - (int)destination;
}


/* Address: 0x005872B0.
   Returns the current local time packed as (hour << 16) | (minute << 8) | second.
*/
uint32_t __thandor_eax_preserve_ecx_edx Locale_GetPackedCurrentTime(void)

{
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  return (uint32_t)g_LocaleSystemState.localTime.second |
         (uint32_t)g_LocaleSystemState.localTime.minute << 8 |
         (uint32_t)g_LocaleSystemState.localTime.hour << 16;
}


/* Address: 0x005872F0.
   Guesses the player's telephone country code from the Windows user language: English 44, German 49,
   French 33, Italian 39, Spanish 34, Russian 7, anything else 0.
*/
uint32_t __thandor_eax_preserve_ecx_edx Locale_GetDefaultTelephoneCountryCode(void)

{
  LCID userLocaleId;
  uint32_t primaryLanguageId;
  uint32_t telephoneCountryCode;

  userLocaleId = GetUserDefaultLCID();
  primaryLanguageId = userLocaleId & 0x1ff; /* PRIMARYLANGID would mask 0x3ff */
  if (primaryLanguageId == LANG_ENGLISH) {
    telephoneCountryCode = 44;
  }
  else if (primaryLanguageId == LANG_GERMAN) {
    telephoneCountryCode = 49;
  }
  else if (primaryLanguageId == LANG_FRENCH) {
    telephoneCountryCode = 33;
  }
  else if (primaryLanguageId == LANG_ITALIAN) {
    telephoneCountryCode = 39;
  }
  else if (primaryLanguageId == LANG_SPANISH) {
    telephoneCountryCode = 34;
  }
  else if (primaryLanguageId == LANG_RUSSIAN) {
    telephoneCountryCode = 7;
  }
  else {
    telephoneCountryCode = 0;
  }
  return telephoneCountryCode;
}


/* Address: 0x00587350.
   Copies the default computer label (L"Computer", or the machine name FileSystem_Init put there) to
   destination: always the whole 0x40-byte buffer including its zero padding.
*/
void __thandor_void_preserve_eax_ecx_edx Locale_CopyDefaultComputerLabelUtf16(uint16_t *destination)

{
  int copyDwordsRemaining;
  uint16_t *sourceCursor;

  sourceCursor = g_DefaultComputerLabelUtf16;
  for (copyDwordsRemaining = 0x10; copyDwordsRemaining != 0; copyDwordsRemaining--) {
    *(uint32_t *)destination = *(uint32_t *)sourceCursor; /* two UTF-16 units per dword */
    sourceCursor = sourceCursor + 2;
    destination = destination + 2;
  }
  return;
}


/* Address: 0x00586880.
   Stops the periodic timer of callback: clears its slot (the first match) and kills the paired WinMM
   timer; the stale timer id stays in the table. An unknown callback is ignored.
*/
void __thandor_void_preserve_eax_ecx_edx TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback)

{
  int callbackSlotByteOffset;

  callbackSlotByteOffset = 0;
  do {
    if (*(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) ==
        callback) {
      *(TimerCallbackProc **)((int)g_TimerSystemState.callbacks + callbackSlotByteOffset) = NULL;
      /* binding 3 is timeKillEvent */
      ((BootstrapTimeKillEventProc)g_BootstrapApiBindings[3].destination)
                (*(uint32_t *)((int)g_TimerSystemState.winmmTimerIds + callbackSlotByteOffset));
      return;
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset != (int)sizeof g_TimerSystemState.callbacks);
  return;
}


/* Address: 0x00586B70.
   Parses the leading decimal digits of a GetLocaleInfoA number field ("1", "3;0", ...); stops at the
   first non-digit. No sign, whitespace or overflow handling.
*/
uint32_t Locale_ParseUnsignedDecimalAscii(uint8_t *text)

{
  /* The decompiled loop tested (c - '0') > 0x2f instead of "no borrow", so every digit ended the
     parse and the locale's day-month order and 24-hour flag always read as 0 (US format). */
  uint32_t parsedValue = 0;

  while ((*text >= '0') && (*text < '9' + 1)) {
    parsedValue = parsedValue * 10 + (uint32_t)(*text - '0');
    text++;
  }
  return parsedValue;
}
