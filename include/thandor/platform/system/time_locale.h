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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005867B0 */
void TimerSystem_Shutdown(void);

/* 0x00586BA0 */
void Locale_Init(void);

/* 0x00586790 */
void __cdecl TimerSystem_Init(void);

/* 0x005867E0 */
void __stdcall WinMM_TimerDispatchCallback
          (WinMmTimerId timerId,uint32_t message,TimerCallbackSlotByteOffset slotOffset,
          uint32_t reserved1,uint32_t reserved2);

/* 0x00586820 */
void TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback);

/* 0x00586DD0 */
uint32_t Locale_FormatDateFieldsUtf16 (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month, LocaleCalendarDayStack32 day,uint16_t *destination);

/* 0x00586F10 */
uint32_t Locale_FormatCurrentDateUtf16(uint16_t *destination);

/* 0x00587080 */
uint32_t Locale_GetPackedCurrentDate(void);

/* 0x005870C0 */
uint32_t Locale_FormatTimeFieldsUtf16 (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,uint16_t *destination);

/* 0x005871B0 */
uint32_t Locale_FormatCurrentTimeUtf16(uint16_t *destination);

/* 0x005872B0 */
uint32_t Locale_GetPackedCurrentTime(void);

/* 0x005872F0 */
uint32_t Locale_GetDefaultTelephoneCountryCode(void);

/* 0x00587350 */
void Locale_CopyDefaultComputerLabelUtf16(uint16_t *destination);

/* 0x00586880 */
void TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback);

/* 0x00586B70 */
uint32_t Locale_ParseUnsignedDecimalAscii(uint8_t *text);

#endif /* THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H */
