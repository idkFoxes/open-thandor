/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/time_locale.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/system/time_locale.h>
#include <thandor/thandor.h>

/* Module data. */

__declspec(align(4)) LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate = 0;

__declspec(align(8)) LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime = 0;

__declspec(align(8)) TimerRegisterPeriodicProc *g_TimerRegisterPeriodic = 0;

__declspec(align(4)) TimerUnregisterPeriodicProc *g_TimerUnregisterPeriodic = 0;

__declspec(align(4)) LocaleCopyDefaultComputerLabelUtf16Proc *g_LocaleCopyDefaultComputerLabelUtf16 = 0;

static LocaleFormatDateFieldsUtf16Proc *g_LocaleFormatDateFieldsUtf16 = 0;

static CpuDetectFeaturesProc *g_CPUDetectFeatures = 0;

static TimerSystemState g_TimerSystemState = {0};

static uint8_t g_LocaleInfoScratch[16] = {0};

static LocaleSystemState g_LocaleSystemState = {0};

LocaleFormatCurrentDateUtf16Proc *g_LocaleFormatCurrentDateUtf16 = 0;

LocaleFormatTimeFieldsUtf16Proc *g_LocaleFormatTimeFieldsUtf16 = 0;

LocaleFormatCurrentTimeUtf16Proc *g_LocaleFormatCurrentTimeUtf16 = 0;

LocaleGetTelephoneCountryCodeProc *g_LocaleGetDefaultTelephoneCountryCode = 0;

/* Implementation ownership: platform/system/time_locale. */

/* Stops every periodic timer: unregisters each callback still present in the 32 slots (which also
   kills its WinMM timer).
*/
void TimerSystem_Shutdown(void)

{
  uint32_t callbackSlotByteOffset;

  /* the slots are walked by byte offset, as the WinMM dwUser values are */
  callbackSlotByteOffset = 0;
  do {
    if (TIMER_CALLBACK_AT(callbackSlotByteOffset) != NULL) {
      TimerSystem_UnregisterPeriodic(TIMER_CALLBACK_AT(callbackSlotByteOffset));
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset < sizeof g_TimerSystemState.callbacks);
  return;
}


/* Detects the CPU features, installs the date/time/locale services in their function pointers and
   caches the user's locale settings (language id, number separators, date/time separators and order,
   AM/PM designators) in g_LocaleSystemState for the date and number formatters.
   Numeric fields are parsed from the GetLocaleInfoA text; string fields are widened to UTF-16.
*/
void Locale_Init(void)

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
  /* The string copies pass an output capacity of LOCALE_STRING_COPY_CAPACITY_BYTES (8 UTF-16 units),
     although every string field of g_LocaleSystemState holds 16 units. */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ILANGUAGE,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.languageIdentifierDigits =
       Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.decimalSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_STHOUSAND,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.thousandsSeparator,g_LocaleInfoScratch);
  /* LOCALE_SGROUPING is text like "3;0"; only the leading group size is kept */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SGROUPING,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.digitGroupingSize = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SNEGATIVESIGN,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.negativeSign,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SDATE,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.dateSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_STIME,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.timeSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_S1159,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.amDesignator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_S2359,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.pmDesignator,g_LocaleInfoScratch);
  /* 0 = month-day-year, 1 = day-month-year, 2 = year-month-day */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ILDATE,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.longDateOrder = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  /* 0 = 12-hour clock with AM/PM, 1 = 24-hour clock */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ITIME,(LPSTR)g_LocaleInfoScratch,sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.timeFormat24Hour = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
}


/* Installs the WinMM periodic-timer services and the Win32 message pump in their function pointers.
   It cannot fail: the original always reports success, which ProcessEntry relies on.
*/
void __cdecl TimerSystem_Init(void)

{
  g_TimerRegisterPeriodic = (TimerRegisterPeriodicProc *)TimerSystem_RegisterPeriodic;
  g_TimerUnregisterPeriodic = (TimerUnregisterPeriodicProc *)TimerSystem_UnregisterPeriodic;
  g_Win32PumpMessages = Win32_PumpMessages;
}

/* The WinMM timer procedure every periodic timer of TimerSystem_RegisterPeriodic runs through, on WinMM's
   timer thread: dwUser (slotOffset) is the byte offset of the timer's callbacks[] slot, and a valid, occupied
   slot's engine callback is called without arguments.
*/
void __stdcall WinMM_TimerDispatchCallback
          (WinMmTimerId timerId,uint32_t message,TimerCallbackSlotByteOffset slotOffset,
          uint32_t reserved1,uint32_t reserved2)

{
  /* slotOffset is the byte offset of the callbacks[] entry */
  if ((slotOffset & 3) == 0 && slotOffset < sizeof g_TimerSystemState.callbacks &&
      TIMER_CALLBACK_AT(slotOffset) != NULL) {
    TIMER_CALLBACK_AT(slotOffset)();
  }
  return;
}


/* Starts a periodic timer: puts callback into the first free of the 32 slots and has WinMM call it
   frequencyHz times per second (period 1000 / frequencyHz ms, truncated) through
   WinMM_TimerDispatchCallback, on WinMM's timer thread. With all slots taken the request is ignored.
*/
void TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback)

{
  WinMmTimerPeriodMilliseconds intervalMilliseconds;
  WinMmTimerId winmmTimerId;
  TimerCallbackSlotByteOffset callbackSlotByteOffset;

  intervalMilliseconds = (WinMmTimerPeriodMilliseconds)(1000 / (uint64_t)frequencyHz);
  callbackSlotByteOffset = 0;
  do {
    if (TIMER_CALLBACK_AT(callbackSlotByteOffset) == NULL) {
      TIMER_CALLBACK_AT(callbackSlotByteOffset) = callback;
      /* dwUser is the slot's byte offset */
      winmmTimerId = ((BootstrapTimeSetEventProc)g_BootstrapApiBindings[BOOTSTRAP_API_TIME_SET_EVENT].destination)
                               (intervalMilliseconds,0,WinMM_TimerDispatchCallback,
                                callbackSlotByteOffset,TIME_PERIODIC);
      /* the ID is stored at the callback's slot index, not at intervalMilliseconds: the ID pairs with
         the callback slot (see TimerSystem_UnregisterPeriodic). */
      TIMER_WINMM_ID_AT(callbackSlotByteOffset) = winmmTimerId;
      return;
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset < sizeof g_TimerSystemState.callbacks);
  return;
}


/* Writes a date as UTF-16 text in the user's order (LOCALE_ILDATE: 0 month-day-year, 1 day-month-year,
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
                      ((uint16_t *)((uint8_t *)destination + appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)destination + appendByteLength + separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,outputCursor);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((uint8_t *)destination + appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)destination + appendByteLength + separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,outputCursor);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  else {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((uint8_t *)destination + appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)destination + appendByteLength + separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,outputCursor);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,outputCursor);
    completedByteOffset = (int)outputCursor + appendByteLength;
  }
  return completedByteOffset - (int)destination;
}

/* Writes today's local date like Locale_FormatDateFieldsUtf16 (user's order and separator). Returns the
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
                   ((uint16_t *)((uint8_t *)destination + currentAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    outputCursor = (uint16_t *)((uint8_t *)destination + currentAppendByteLength + separatorByteLength)
    ;
    branchAppendByteLength =
         g_WideNumberFormatUtf16
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                    outputCursor);
    branchSeparatorByteLength =
         Utf16_CopyAndReturnByteLength
                   ((uint16_t *)((uint8_t *)outputCursor + branchAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    branchOutputCursor =
         (uint16_t *)((uint8_t *)outputCursor + branchAppendByteLength + branchSeparatorByteLength);
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
                      ((uint16_t *)((uint8_t *)destination + fieldByteLength),g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((uint8_t *)destination + fieldByteLength + dateSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month
                       ,fieldCursor);
    fieldCursor = (uint16_t *)((uint8_t *)fieldCursor + fieldByteLength);
    fieldByteLength = Utf16_CopyAndReturnByteLength(fieldCursor,g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((uint8_t *)fieldCursor + fieldByteLength);
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
                      ((uint16_t *)((uint8_t *)destination + fieldByteLength),g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((uint8_t *)destination + fieldByteLength + dateSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month
                       ,fieldCursor);
    fieldCursor = (uint16_t *)((uint8_t *)fieldCursor + fieldByteLength);
    fieldByteLength = Utf16_CopyAndReturnByteLength(fieldCursor,g_LocaleSystemState.dateSeparator);
    fieldCursor = (uint16_t *)((uint8_t *)fieldCursor + fieldByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                       fieldCursor);
    completedByteOffset = (int)fieldCursor + fieldByteLength;
  }
  return completedByteOffset - (int)destination;
}


/* Returns today's local date packed as (year << 16) | (month << 8) | day, so packed dates compare in
   calendar order.
*/
uint32_t Locale_GetPackedCurrentDate(void)

{
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  return (uint32_t)g_LocaleSystemState.localTime.day | ((uint32_t)g_LocaleSystemState.localTime.month << 8) |
         ((uint32_t)g_LocaleSystemState.localTime.year << 16);
}


/* Writes hour:minute as UTF-16 text with the user's time separator. 24-hour locales get both fields
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
    /* the designator is selected by the hour; note the original picks the field named pmDesignator
       for hours below 12. */
    designatorText = g_LocaleSystemState.pmDesignator;
    if (11 < hour) {
      designatorText = g_LocaleSystemState.amDesignator;
      hour = hour - 12;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((uint8_t *)destination + appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((uint8_t *)destination + appendByteLength + separatorByteLength);
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength((uint16_t *)((uint8_t *)outputCursor + appendByteLength),designatorText);
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
                      ((uint16_t *)((uint8_t *)outputCursor + appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength + separatorByteLength);
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

/* Writes the current local time like Locale_FormatTimeFieldsUtf16 (same padding and the same swapped
   AM/PM designators). Returns the byte length without the terminator.
*/
uint32_t Locale_FormatCurrentTimeUtf16(uint16_t *destination)

{
  uint32_t hour;
  uint32_t minute;
  uint32_t hourByteLength;
  uint32_t appendByteLength;
  uint32_t separatorByteLength;
  uint16_t *designatorText;
  uint16_t *outputCursor;
  uint8_t *outputEnd;

  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  if (g_LocaleSystemState.timeFormat24Hour == 0) {
    hour = (uint32_t)g_LocaleSystemState.localTime.hour;
    /* designator selection as in Locale_FormatTimeFieldsUtf16 (pmDesignator for hours below 12) */
    designatorText = g_LocaleSystemState.pmDesignator;
    if (11 < hour) {
      designatorText = g_LocaleSystemState.amDesignator;
      hour = hour - 12;
    }
    hourByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((uint8_t *)destination + hourByteLength),
                       g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((uint8_t *)destination + hourByteLength + separatorByteLength);
    minute = (uint32_t)g_LocaleSystemState.localTime.minute;
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((uint8_t *)outputCursor + appendByteLength),designatorText);
    outputEnd = (uint8_t *)outputCursor + appendByteLength + separatorByteLength;
  }
  else {
    hour = (uint32_t)g_LocaleSystemState.localTime.hour;
    outputCursor = destination;
    if (hour < 10) {
      destination[0] = '0';
      destination[1] = 0;
      outputCursor = destination + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,hour,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      ((uint16_t *)((uint8_t *)outputCursor + appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = (uint16_t *)((uint8_t *)outputCursor + appendByteLength + separatorByteLength);
    minute = (uint32_t)g_LocaleSystemState.localTime.minute;
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    outputEnd = (uint8_t *)outputCursor + appendByteLength;
  }
  return (uint32_t)(outputEnd - (uint8_t *)destination);
}


/* Returns the current local time packed as (hour << 16) | (minute << 8) | second.
*/
uint32_t Locale_GetPackedCurrentTime(void)

{
  GetLocalTime((LPSYSTEMTIME)&g_LocaleSystemState);
  return (uint32_t)g_LocaleSystemState.localTime.second |
         ((uint32_t)g_LocaleSystemState.localTime.minute << 8) |
         ((uint32_t)g_LocaleSystemState.localTime.hour << 16);
}


/* Guesses the player's telephone country code from the Windows user language: English 44, German 49,
   French 33, Italian 39, Spanish 34, Russian 7, anything else 0.
*/
uint32_t Locale_GetDefaultTelephoneCountryCode(void)

{
  LCID userLocaleId;
  uint32_t primaryLanguageId;
  uint32_t telephoneCountryCode;

  userLocaleId = GetUserDefaultLCID();
  primaryLanguageId = userLocaleId & LOCALE_PRIMARY_LANGUAGE_MASK;
  if (primaryLanguageId == LANG_ENGLISH) {
    telephoneCountryCode = LOCALE_COUNTRY_GREAT_BRITAIN;
  }
  else if (primaryLanguageId == LANG_GERMAN) {
    telephoneCountryCode = LOCALE_COUNTRY_GERMANY;
  }
  else if (primaryLanguageId == LANG_FRENCH) {
    telephoneCountryCode = LOCALE_COUNTRY_FRANCE;
  }
  else if (primaryLanguageId == LANG_ITALIAN) {
    telephoneCountryCode = LOCALE_COUNTRY_ITALY;
  }
  else if (primaryLanguageId == LANG_SPANISH) {
    telephoneCountryCode = LOCALE_COUNTRY_SPAIN;
  }
  else if (primaryLanguageId == LANG_RUSSIAN) {
    telephoneCountryCode = 7;
  }
  else {
    telephoneCountryCode = LOCALE_COUNTRY_GENERIC;
  }
  return telephoneCountryCode;
}


/* Copies the default computer label (L"Computer", or the machine name FileSystem_Init put there) to
   destination: always the whole 0x40-byte buffer including its zero padding.
*/
void Locale_CopyDefaultComputerLabelUtf16(uint16_t *destination)

{
  int copyDwordsRemaining;
  uint16_t *sourceCursor;

  sourceCursor = g_DefaultComputerLabelUtf16;
  for (copyDwordsRemaining = sizeof g_DefaultComputerLabelUtf16 / 4; copyDwordsRemaining != 0;
       copyDwordsRemaining--) {
    *(uint32_t *)destination = *(uint32_t *)sourceCursor; /* two UTF-16 units per dword */
    sourceCursor = sourceCursor + 2;
    destination = destination + 2;
  }
  return;
}


/* Stops the periodic timer of callback: clears its slot (the first match) and kills the paired WinMM
   timer; the stale timer id stays in the table. An unknown callback is ignored.
*/
void TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback)

{
  int callbackSlotByteOffset;

  callbackSlotByteOffset = 0;
  do {
    if (TIMER_CALLBACK_AT(callbackSlotByteOffset) == callback) {
      TIMER_CALLBACK_AT(callbackSlotByteOffset) = NULL;
      ((BootstrapTimeKillEventProc)g_BootstrapApiBindings[BOOTSTRAP_API_TIME_KILL_EVENT].destination)
                (TIMER_WINMM_ID_AT(callbackSlotByteOffset));
      return;
    }
    callbackSlotByteOffset = callbackSlotByteOffset + 4;
  } while (callbackSlotByteOffset != (int)sizeof g_TimerSystemState.callbacks);
  return;
}


/* Parses the leading decimal digits of a GetLocaleInfoA number field ("1", "3;0", ...); stops at the
   first non-digit. No sign, whitespace or overflow handling.
*/
uint32_t Locale_ParseUnsignedDecimalAscii(uint8_t *text)

{
  /* every digit '0'..'9' continues the parse (the locale's day-month order and 24-hour flag depend on it;
     ending at the first digit would always read them as 0, US format). */
  uint32_t parsedValue = 0;

  while ((*text >= '0') && (*text < '9' + 1)) {
    parsedValue = parsedValue * 10 + (uint32_t)(*text - '0');
    text++;
  }
  return parsedValue;
}
