#ifndef THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H
#define THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/system/time_locale. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005867B0 */
void __cdecl TimerSystem_Shutdown(void);

/* 0x00586BA0 */
void __cdecl Locale_Init(void);

/* 0x00402F70 */
undefined * Locale_MapTelephoneCountryCodeToRegionTagPacked(LocaleTelephoneCountryCode countryCode);

/* 0x00586790 */
void __cdecl TimerSystem_Init(void);

/* 0x005867E0 */
void WinMM_TimerDispatchCallback (WinMmTimerId timerId,dword message,TimerCallbackSlotByteOffset slotOffset, dword callbackData0,dword callbackData1);

/* 0x00586820 */
void TimerSystem_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback);

/* 0x00586DD0 */
dword Locale_FormatDateFieldsUtf16 (LocaleCalendarYearStack32 year,LocaleCalendarMonthStack32 month, LocaleCalendarDayStack32 day,word *destination);

/* 0x00586F10 */
dword Locale_FormatCurrentDateUtf16(word *destination);

/* 0x00587080 */
dword __cdecl Locale_GetPackedCurrentDate(void);

/* 0x005870C0 */
dword Locale_FormatTimeFieldsUtf16 (LocaleClockHourStack32 hour,LocaleClockMinuteStack32 minute,word *destination);

/* 0x005871B0 */
dword Locale_FormatCurrentTimeUtf16(word *destination);

/* 0x005872B0 */
dword __cdecl Locale_GetPackedCurrentTime(void);

/* 0x005872F0 */
dword __cdecl Locale_GetDefaultTelephoneCountryCode(void);

/* 0x00587350 */
void Locale_CopyDefaultComputerLabelUtf16(word *destination);

/* 0x00586880 */
void TimerSystem_UnregisterPeriodic(TimerCallbackProc *callback);

/* 0x00586B70 */
dword Locale_ParseUnsignedDecimalAscii(byte *text);

#endif /* THANDOR_PLATFORM_SYSTEM_TIME_LOCALE_H */
