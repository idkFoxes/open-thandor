/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/system/time_locale.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H
#define THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/system/time_locale. */

/* Locale_Init: output capacity passed for every locale string (8 UTF-16 units, half of each 16-unit field) */
#define LOCALE_STRING_COPY_CAPACITY_BYTES 0x10
/* Primary language bits the original keeps from GetUserDefaultLCID (9 bits; PRIMARYLANGID keeps 10, 0x3ff) */
#define LOCALE_PRIMARY_LANGUAGE_MASK 0x1ff
/* Functions are grouped by semantic ownership. */

void Locale_Init(void);

uint32_t Locale_FormatDateFieldsUtf16 (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month, LocaleCalendarDayStack32 day,uint16_t *destination);

uint32_t Locale_FormatCurrentDateUtf16(uint16_t *destination);

uint32_t Locale_GetPackedCurrentDate(void);

uint32_t Locale_FormatTimeFieldsUtf16 (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,uint16_t *destination);

uint32_t Locale_FormatCurrentTimeUtf16(uint16_t *destination);

uint32_t Locale_GetPackedCurrentTime(void);

uint32_t Locale_GetDefaultTelephoneCountryCode(void);

void Locale_CopyDefaultComputerLabelUtf16(uint16_t *destination);

uint32_t Locale_ParseUnsignedDecimalAscii(uint8_t *text);

extern LocaleFormatCurrentDateUtf16Proc *g_LocaleFormatCurrentDateUtf16;
extern LocaleFormatTimeFieldsUtf16Proc *g_LocaleFormatTimeFieldsUtf16;
extern LocaleFormatCurrentTimeUtf16Proc *g_LocaleFormatCurrentTimeUtf16;
extern LocaleGetTelephoneCountryCodeProc *g_LocaleGetDefaultTelephoneCountryCode;

extern LocaleCopyDefaultComputerLabelUtf16Proc *g_LocaleCopyDefaultComputerLabelUtf16;

extern TimerRegisterPeriodicProc *g_TimerRegisterPeriodic;
extern TimerUnregisterPeriodicProc *g_TimerUnregisterPeriodic;

extern LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate;
extern LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime;

#endif /* THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H */
