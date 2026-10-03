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

/* The timer tables are walked by byte offset (the offset is the WinMM dwUser value); one slot of each */
#define TIMER_CALLBACK_AT(byteOffset) \
  (*(TimerCallbackProc **)((uint8_t *)g_TimerSystemState.callbacks + (byteOffset)))
#define TIMER_WINMM_ID_AT(byteOffset) (*(WinMmTimerId *)((uint8_t *)g_TimerSystemState.winmmTimerIds + (byteOffset)))
/* Locale_Init: output capacity passed for every locale string (8 UTF-16 units, half of each 16-unit field) */
#define LOCALE_STRING_COPY_CAPACITY_BYTES 0x10
/* Primary language bits the original keeps from GetUserDefaultLCID (9 bits; PRIMARYLANGID keeps 10, 0x3ff) */
#define LOCALE_PRIMARY_LANGUAGE_MASK 0x1ff
/* Functions are grouped by semantic ownership. */

void TimerSystem_Shutdown(void);

void Locale_Init(void);

void __cdecl TimerSystem_Init(void);

void __stdcall WinMM_TimerDispatchCallback
          (WinMmTimerId timerId,uint32_t message,TimerCallbackSlotByteOffset slotOffset,
          uint32_t reserved1,uint32_t reserved2);

void TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback);

uint32_t Locale_FormatDateFieldsUtf16 (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month, LocaleCalendarDayStack32 day,uint16_t *destination);

uint32_t Locale_FormatCurrentDateUtf16(uint16_t *destination);

uint32_t Locale_GetPackedCurrentDate(void);

uint32_t Locale_FormatTimeFieldsUtf16 (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,uint16_t *destination);

uint32_t Locale_FormatCurrentTimeUtf16(uint16_t *destination);

uint32_t Locale_GetPackedCurrentTime(void);

uint32_t Locale_GetDefaultTelephoneCountryCode(void);

void Locale_CopyDefaultComputerLabelUtf16(uint16_t *destination);

void TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback);

uint32_t Locale_ParseUnsignedDecimalAscii(uint8_t *text);

#endif /* THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H */
