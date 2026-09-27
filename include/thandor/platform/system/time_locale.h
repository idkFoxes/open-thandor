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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005867B0 */
void __thandor_preserve_eax TimerSystem_Shutdown(void);

/* 0x00586BA0 */
void __thandor_void_preserve_eax_ecx_edx Locale_Init(void);

/* 0x00402F70 */
LocaleRegionTagPacked __thandor_eax_preserve_ecx_edx
Locale_MapTelephoneCountryCodeToRegionTagPacked(LocaleTelephoneCountryCode countryCode);

/* 0x00586790 */
void __cdecl TimerSystem_Init(void);

/* 0x005867E0 */
void __stdcall WinMM_TimerDispatchCallback
          (WinMmTimerId timerId,uint32_t message,TimerCallbackSlotByteOffset slotOffset,
          uint32_t reserved1,uint32_t reserved2);

/* 0x00586820 */
void __thandor_void_preserve_eax_ecx_edx
TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback);

/* 0x00586DD0 */
uint32_t Locale_FormatDateFieldsUtf16 (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month, LocaleCalendarDayStack32 day,uint16_t *destination);

/* 0x00586F10 */
uint32_t Locale_FormatCurrentDateUtf16(uint16_t *destination);

/* 0x00587080 */
uint32_t __thandor_eax_preserve_ecx_edx Locale_GetPackedCurrentDate(void);

/* 0x005870C0 */
uint32_t Locale_FormatTimeFieldsUtf16 (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,uint16_t *destination);

/* 0x005871B0 */
uint32_t Locale_FormatCurrentTimeUtf16(uint16_t *destination);

/* 0x005872B0 */
uint32_t __thandor_eax_preserve_ecx_edx Locale_GetPackedCurrentTime(void);

/* 0x005872F0 */
uint32_t __thandor_eax_preserve_ecx_edx Locale_GetDefaultTelephoneCountryCode(void);

/* 0x00587350 */
void __thandor_void_preserve_eax_ecx_edx Locale_CopyDefaultComputerLabelUtf16(uint16_t *destination);

/* 0x00586880 */
void __thandor_void_preserve_eax_ecx_edx TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback);

/* 0x00586B70 */
uint32_t Locale_ParseUnsignedDecimalAscii(uint8_t *text);

#endif /* THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H */
