/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/system/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_SYSTEM_TYPES_H
#define THANDOR_PLATFORM_SYSTEM_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/platform/bootstrap/types.h>

struct _OVERLAPPED;
union _union_518;
struct _struct_519;
struct _SECURITY_ATTRIBUTES;
struct _SYSTEMTIME;
struct HWND__;
struct Win32SystemTime16;
struct LocaleSystemState;
struct _FILETIME;
struct _WIN32_FIND_DATAA;

using DWORD = uint32_t;

using LCTYPE = DWORD;

using ULONG_PTR = uintptr_t; /* pointer-sized, as in the Windows SDK */

using HANDLE = void *;

using PVOID = void *;

struct _struct_519 {
    DWORD Offset;
    DWORD OffsetHigh;
};

union _union_518 {
    struct _struct_519 s;
    PVOID Pointer;
};

struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union _union_518 u;
    HANDLE hEvent;
};

using LPVOID = void *;

using BOOL = int;

struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
};

using WORD = uint16_t;

struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
};

using CHAR = char;

using LPOVERLAPPED = struct _OVERLAPPED *;

using ThreadStartRoutineFunction = DWORD __stdcall (LPVOID); /* function type of a thread entry (__stdcall, as in the SDK) */

using PTHREAD_START_ROUTINE = ThreadStartRoutineFunction *;

using LPTHREAD_START_ROUTINE = PTHREAD_START_ROUTINE;

using LPSECURITY_ATTRIBUTES = struct _SECURITY_ATTRIBUTES *;

using LPWIN32_FIND_DATAA = struct _WIN32_FIND_DATAA *;

using LPSYSTEMTIME = struct _SYSTEMTIME *;

using va_list = char *;

using HWND = struct HWND__ *;

using UINT = uint32_t;

using LONG = long;

struct HWND__ {
    int unused;
};

using LPCSTR = const CHAR *; /* const as in the Windows SDK (the imports are extern "C": same symbols) */

using PLONG = LONG *;

using LPSTR = CHAR *;

using LCID = DWORD;

/* Telephone country code that selects a text resource locale block (TextResourceLocaleBlockPrefix.countryCode,
   a file dword) and the thandor.ini language_country_code override. Other values are valid (no block matches). */
enum class LocaleTelephoneCountryCode : int {
    LOCALE_COUNTRY_GENERIC=0,
    LOCALE_COUNTRY_USA=1,
    LOCALE_COUNTRY_CANADA=2,
    LOCALE_COUNTRY_RUSSIA=7,
    LOCALE_COUNTRY_NETHERLANDS=31,
    LOCALE_COUNTRY_BELGIUM=32,
    LOCALE_COUNTRY_FRANCE=33,
    LOCALE_COUNTRY_SPAIN=34,
    LOCALE_COUNTRY_ITALY=39,
    LOCALE_COUNTRY_GREAT_BRITAIN=44,
    LOCALE_COUNTRY_DENMARK=45,
    LOCALE_COUNTRY_GERMANY=49
};

using Win32CalendarYear16 = uint16_t;

using LocaleCalendarDayStack32 = uint32_t;

using Win32CalendarMonth16 = uint16_t;

using Win32CalendarDay16 = uint16_t;

using Win32Millisecond16 = uint16_t;

using LocaleClockHourStack32 = uint32_t;

using Win32Hour16 = uint16_t;

using Win32DayOfWeek16 = uint16_t;

using LocaleClockMinuteStack32 = uint32_t;

using Win32Second16 = uint16_t;

using LocaleCalendarMonthStack32 = uint32_t;

using LocaleCalendarYearStack32 = uint32_t;

using Win32Minute16 = uint16_t;

struct Win32SystemTime16 {
    Win32CalendarYear16 year; 
    Win32CalendarMonth16 month; 
    Win32DayOfWeek16 dayOfWeek; 
    Win32CalendarDay16 day; 
    Win32Hour16 hour; 
    Win32Minute16 minute; 
    Win32Second16 second; 
    Win32Millisecond16 milliseconds; 
};

struct LocaleSystemState {
    struct Win32SystemTime16 localTime; 
    uint32_t languageIdentifierDigits; 
    uint16_t decimalSeparator[16]; 
    uint16_t thousandsSeparator[16]; 
    uint16_t negativeSign[16];
    uint32_t digitGroupingSize;
    uint16_t dateSeparator[16]; 
    uint16_t timeSeparator[16]; 
    uint32_t longDateOrder; 
    uint32_t timeFormat24Hour; 
    uint16_t amDesignator[16]; 
    uint16_t pmDesignator[16]; 
};

using SIZE_T = ULONG_PTR;

using LPDWORD = DWORD *;

using HMODULE = HINSTANCE;

using LPLONG = long *;

using LPFILETIME = struct _FILETIME *;

using FARPROC = int (*)();

using LPWORD = WORD *;

using LPCVOID = void *;
using LocaleCopyDefaultComputerLabelUtf16Proc = void (uint16_t * destination);
using LocaleFormatCurrentDateUtf16Proc = uint32_t (uint16_t * destination);
using LocaleFormatCurrentTimeUtf16Proc = uint32_t (uint16_t * destination);
using LocaleFormatTimeFieldsUtf16Proc = uint32_t (uint32_t hour, uint32_t minute, uint16_t * destination);
using LocaleGetPackedCurrentDateProc = uint32_t ();
using LocaleGetPackedCurrentTimeProc = uint32_t ();
using LocaleGetTelephoneCountryCodeProc = LocaleTelephoneCountryCode ();
using TimerCallbackProc = void ();
using TimerRegisterPeriodicProc = void (uint32_t frequencyHz, TimerCallbackProc * callback);
using TimerUnregisterPeriodicProc = void (TimerCallbackProc * callback);
using PlatformPumpEventsProc = void ();

#endif /* THANDOR_PLATFORM_SYSTEM_TYPES_H */
