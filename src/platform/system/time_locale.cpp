/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/time_locale.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/system/time_locale.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(4) LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate = nullptr;

THANDOR_ALIGN(8) LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime = nullptr;

/* the periodic timers, installed by SdlPlatform_InstallTimersAndPump (SDL timers instead of the original's WinMM
   timeSetEvent timers) */
THANDOR_ALIGN(8) TimerRegisterPeriodicProc *g_TimerRegisterPeriodic = nullptr;

THANDOR_ALIGN(4) TimerUnregisterPeriodicProc *g_TimerUnregisterPeriodic = nullptr;

THANDOR_ALIGN(4) LocaleCopyDefaultComputerLabelUtf16Proc *g_LocaleCopyDefaultComputerLabelUtf16 = nullptr;

static uint8_t g_LocaleInfoScratch[16] = {};

static LocaleSystemState g_LocaleSystemState = {};

LocaleFormatCurrentDateUtf16Proc *g_LocaleFormatCurrentDateUtf16 = nullptr;

LocaleFormatTimeFieldsUtf16Proc *g_LocaleFormatTimeFieldsUtf16 = nullptr;

LocaleFormatCurrentTimeUtf16Proc *g_LocaleFormatCurrentTimeUtf16 = nullptr;

LocaleGetTelephoneCountryCodeProc *g_LocaleGetDefaultTelephoneCountryCode = nullptr;

/* GetLocalTime fills localTime, the first member of g_LocaleSystemState (Win32SystemTime16 is SYSTEMTIME) */
static inline LPSYSTEMTIME Locale_LocalTimeSlot()
{
  return reinterpret_cast<LPSYSTEMTIME>(&g_LocaleSystemState); /* same address as .localTime */
}

/* The UTF-16 formatters return byte lengths, and the date/time writers step their cursors by those bytes. */
static inline uint8_t *Utf16_Bytes(uint16_t *text)
{
  return reinterpret_cast<uint8_t *>(text); /* byte view of the UTF-16 output */
}

static inline uint16_t *Utf16_AdvanceBytes(uint16_t *text,uint32_t byteCount)
{
  return reinterpret_cast<uint16_t *>(Utf16_Bytes(text) + byteCount); /* back to code units after a byte step */
}

/* Installs the date/time/locale services in their function pointers and
   caches the user's locale settings (language id, number separators, date/time separators and order,
   AM/PM designators) in g_LocaleSystemState for the date and number formatters.
   Numeric fields are parsed from the GetLocaleInfoA text; string fields are widened to UTF-16.
*/
void Locale_Init()

{
  /* the original first ran the CPU feature detection here and also stored it and Locale_FormatDateFieldsUtf16
     in slots that nothing called */
  g_LocaleFormatCurrentDateUtf16 = Locale_FormatCurrentDateUtf16;
  g_LocaleGetPackedCurrentDate = Locale_GetPackedCurrentDate;
  g_LocaleFormatTimeFieldsUtf16 = Locale_FormatTimeFieldsUtf16;
  g_LocaleFormatCurrentTimeUtf16 = Locale_FormatCurrentTimeUtf16;
  g_LocaleGetPackedCurrentTime = Locale_GetPackedCurrentTime;
  g_LocaleGetDefaultTelephoneCountryCode = Locale_GetDefaultTelephoneCountryCode;
  g_LocaleCopyDefaultComputerLabelUtf16 = Locale_CopyDefaultComputerLabelUtf16;
  /* GetLocaleInfoA writes its text into the byte scratch g_LocaleInfoScratch (hence the casts to LPSTR).
     The string copies pass an output capacity of LOCALE_STRING_COPY_CAPACITY_BYTES (8 UTF-16 units),
     although every string field of g_LocaleSystemState holds 16 units. */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ILANGUAGE,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.languageIdentifierDigits =
       Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.decimalSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_STHOUSAND,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.thousandsSeparator,g_LocaleInfoScratch);
  /* LOCALE_SGROUPING is text like "3;0"; only the leading group size is kept */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SGROUPING,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.digitGroupingSize = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SNEGATIVESIGN,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.negativeSign,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SDATE,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.dateSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_STIME,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.timeSeparator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_S1159,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.amDesignator,g_LocaleInfoScratch);
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_S2359,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  Text_CopyNarrowToUtf16(LOCALE_STRING_COPY_CAPACITY_BYTES,g_LocaleSystemState.pmDesignator,g_LocaleInfoScratch);
  /* 0 = month-day-year, 1 = day-month-year, 2 = year-month-day */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ILDATE,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.longDateOrder = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
  /* 0 = 12-hour clock with AM/PM, 1 = 24-hour clock */
  GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_ITIME,reinterpret_cast<LPSTR>(g_LocaleInfoScratch),sizeof g_LocaleInfoScratch);
  g_LocaleSystemState.timeFormat24Hour = Locale_ParseUnsignedDecimalAscii(g_LocaleInfoScratch);
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
  intptr_t completedByteOffset;
  
  if (g_LocaleSystemState.longDateOrder == 0) {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      (Utf16_AdvanceBytes(destination,appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,appendByteLength),separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,outputCursor);
    outputCursor = Utf16_AdvanceBytes(outputCursor,appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(outputCursor,appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,outputCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(outputCursor) + appendByteLength;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      (Utf16_AdvanceBytes(destination,appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,appendByteLength),separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,outputCursor);
    outputCursor = Utf16_AdvanceBytes(outputCursor,appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(outputCursor,appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,outputCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(outputCursor) + appendByteLength;
  }
  else {
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,year,destination);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      (Utf16_AdvanceBytes(destination,appendByteLength),g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,appendByteLength),separatorByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,month,outputCursor);
    outputCursor = Utf16_AdvanceBytes(outputCursor,appendByteLength);
    appendByteLength = Utf16_CopyAndReturnByteLength(outputCursor,g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(outputCursor,appendByteLength);
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,day,outputCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(outputCursor) + appendByteLength;
  }
  return (uint32_t)(completedByteOffset - reinterpret_cast<intptr_t>(destination));
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
  intptr_t completedByteOffset;
  uint16_t *fieldCursor;
  
  GetLocalTime(Locale_LocalTimeSlot());
  if (g_LocaleSystemState.longDateOrder == 0) {
    currentAppendByteLength =
         g_WideNumberFormatUtf16
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month,
                    destination);
    separatorByteLength =
         Utf16_CopyAndReturnByteLength
                   (Utf16_AdvanceBytes(destination,currentAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,currentAppendByteLength),separatorByteLength)
    ;
    branchAppendByteLength =
         g_WideNumberFormatUtf16
                   (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                    outputCursor);
    branchSeparatorByteLength =
         Utf16_CopyAndReturnByteLength
                   (Utf16_AdvanceBytes(outputCursor,branchAppendByteLength),
                    g_LocaleSystemState.dateSeparator);
    branchOutputCursor =
         Utf16_AdvanceBytes(Utf16_AdvanceBytes(outputCursor,branchAppendByteLength),branchSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.year,
                       branchOutputCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(branchOutputCursor) + fieldByteLength;
  }
  else if (g_LocaleSystemState.longDateOrder == 1) {
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                       destination);
    dateSeparatorByteLength = Utf16_CopyAndReturnByteLength
                      (Utf16_AdvanceBytes(destination,fieldByteLength),g_LocaleSystemState.dateSeparator);
    fieldCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,fieldByteLength),dateSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month
                       ,fieldCursor);
    fieldCursor = Utf16_AdvanceBytes(fieldCursor,fieldByteLength);
    fieldByteLength = Utf16_CopyAndReturnByteLength(fieldCursor,g_LocaleSystemState.dateSeparator);
    fieldCursor = Utf16_AdvanceBytes(fieldCursor,fieldByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.year,
                       fieldCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(fieldCursor) + fieldByteLength;
  }
  else {
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.year,
                       destination);
    dateSeparatorByteLength = Utf16_CopyAndReturnByteLength
                      (Utf16_AdvanceBytes(destination,fieldByteLength),g_LocaleSystemState.dateSeparator);
    fieldCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,fieldByteLength),dateSeparatorByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.month
                       ,fieldCursor);
    fieldCursor = Utf16_AdvanceBytes(fieldCursor,fieldByteLength);
    fieldByteLength = Utf16_CopyAndReturnByteLength(fieldCursor,g_LocaleSystemState.dateSeparator);
    fieldCursor = Utf16_AdvanceBytes(fieldCursor,fieldByteLength);
    fieldByteLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(uint32_t)g_LocaleSystemState.localTime.day,
                       fieldCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(fieldCursor) + fieldByteLength;
  }
  return (uint32_t)(completedByteOffset - reinterpret_cast<intptr_t>(destination));
}


/* Returns today's local date packed as (year << 16) | (month << 8) | day, so packed dates compare in
   calendar order.
*/
uint32_t Locale_GetPackedCurrentDate()

{
  GetLocalTime(Locale_LocalTimeSlot());
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
  intptr_t completedByteOffset;

  /* the local time is fetched but not used */
  GetLocalTime(Locale_LocalTimeSlot());
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
                      (Utf16_AdvanceBytes(destination,appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,appendByteLength),separatorByteLength);
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength(Utf16_AdvanceBytes(outputCursor,appendByteLength),designatorText);
    completedByteOffset = (reinterpret_cast<intptr_t>(outputCursor) + appendByteLength) + separatorByteLength;
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
                      (Utf16_AdvanceBytes(outputCursor,appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(outputCursor,appendByteLength),separatorByteLength);
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    completedByteOffset = reinterpret_cast<intptr_t>(outputCursor) + appendByteLength;
  }
  return (uint32_t)(completedByteOffset - reinterpret_cast<intptr_t>(destination));
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

  GetLocalTime(Locale_LocalTimeSlot());
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
                      (Utf16_AdvanceBytes(destination,hourByteLength),
                       g_LocaleSystemState.timeSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(destination,hourByteLength),separatorByteLength);
    minute = (uint32_t)g_LocaleSystemState.localTime.minute;
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    separatorByteLength = Utf16_CopyAndReturnByteLength
                      (Utf16_AdvanceBytes(outputCursor,appendByteLength),designatorText);
    outputEnd = Utf16_Bytes(outputCursor) + appendByteLength + separatorByteLength;
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
                      (Utf16_AdvanceBytes(outputCursor,appendByteLength),g_LocaleSystemState.timeSeparator);
    outputCursor = Utf16_AdvanceBytes(Utf16_AdvanceBytes(outputCursor,appendByteLength),separatorByteLength);
    minute = (uint32_t)g_LocaleSystemState.localTime.minute;
    if (minute < 10) {
      outputCursor[0] = '0';
      outputCursor[1] = 0;
      outputCursor = outputCursor + 1;
    }
    appendByteLength = g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,minute,outputCursor);
    outputEnd = Utf16_Bytes(outputCursor) + appendByteLength;
  }
  return (uint32_t)(outputEnd - Utf16_Bytes(destination));
}


/* Returns the current local time packed as (hour << 16) | (minute << 8) | second.
*/
uint32_t Locale_GetPackedCurrentTime()

{
  GetLocalTime(Locale_LocalTimeSlot());
  return (uint32_t)g_LocaleSystemState.localTime.second |
         ((uint32_t)g_LocaleSystemState.localTime.minute << 8) |
         ((uint32_t)g_LocaleSystemState.localTime.hour << 16);
}


/* Guesses the player's telephone country code from the Windows user language: English 44, German 49,
   French 33, Italian 39, Spanish 34, Russian 7, anything else 0.
*/
uint32_t Locale_GetDefaultTelephoneCountryCode()

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
    Thandor_StoreU32(destination,Thandor_LoadU32(sourceCursor)); /* two UTF-16 units per dword */
    sourceCursor = sourceCursor + 2;
    destination = destination + 2;
  }
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
