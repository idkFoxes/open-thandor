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

typedef struct _OVERLAPPED _OVERLAPPED, *P_OVERLAPPED;
typedef union _union_518 _union_518, *P_union_518;
typedef struct _struct_519 _struct_519, *P_struct_519;
typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES, *P_SECURITY_ATTRIBUTES;
typedef struct _SYSTEMTIME _SYSTEMTIME, *P_SYSTEMTIME;
typedef struct _COORD _COORD, *P_COORD;
typedef struct _SMALL_RECT _SMALL_RECT, *P_SMALL_RECT;
typedef struct _CONSOLE_SCREEN_BUFFER_INFO _CONSOLE_SCREEN_BUFFER_INFO, *P_CONSOLE_SCREEN_BUFFER_INFO;
typedef struct _CONSOLE_READCONSOLE_CONTROL _CONSOLE_READCONSOLE_CONTROL, *P_CONSOLE_READCONSOLE_CONTROL;
typedef struct HWND__ HWND__, *PHWND__;
typedef struct Win32SystemTime16 Win32SystemTime16, *PWin32SystemTime16;
typedef struct LocaleSystemState LocaleSystemState, *PLocaleSystemState;
typedef struct _FILETIME _FILETIME;
typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA;

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

typedef struct _OVERLAPPED *LPOVERLAPPED;

using PTHREAD_START_ROUTINE = DWORD (__stdcall *)(LPVOID);

using LPTHREAD_START_ROUTINE = PTHREAD_START_ROUTINE;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;

typedef struct _WIN32_FIND_DATAA *LPWIN32_FIND_DATAA;

typedef struct _SYSTEMTIME *LPSYSTEMTIME;

using SHORT = short;

struct _COORD {
    SHORT X;
    SHORT Y;
};

typedef struct _COORD COORD;

struct _SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
};

typedef struct _SMALL_RECT SMALL_RECT;

struct _CONSOLE_SCREEN_BUFFER_INFO {
    COORD dwSize;
    COORD dwCursorPosition;
    WORD wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
};

typedef struct _CONSOLE_READCONSOLE_CONTROL *PCONSOLE_READCONSOLE_CONTROL;

using ULONG = uint32_t;

struct _CONSOLE_READCONSOLE_CONTROL {
    ULONG nLength;
    ULONG nInitialChars;
    ULONG dwCtrlWakeupMask;
    ULONG dwControlKeyState;
};

typedef struct _CONSOLE_SCREEN_BUFFER_INFO *PCONSOLE_SCREEN_BUFFER_INFO;

using va_list = char *;

typedef struct HWND__ *HWND;

using UINT = uint32_t;

using LONG = long;

struct HWND__ {
    int unused;
};

using LPCSTR = CHAR *;

using PLONG = LONG *;

using LPSTR = CHAR *;

using LCID = DWORD;

enum {
    LOCALE_COUNTRY_GENERIC=0,
    LOCALE_COUNTRY_USA=1,
    LOCALE_COUNTRY_CANADA=2,
    LOCALE_COUNTRY_NETHERLANDS=31,
    LOCALE_COUNTRY_BELGIUM=32,
    LOCALE_COUNTRY_FRANCE=33,
    LOCALE_COUNTRY_SPAIN=34,
    LOCALE_COUNTRY_ITALY=39,
    LOCALE_COUNTRY_GREAT_BRITAIN=44,
    LOCALE_COUNTRY_DENMARK=45,
    LOCALE_COUNTRY_GERMANY=49
};
using LocaleTelephoneCountryCode = int;

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

using HLOCAL = HANDLE;

using LPLONG = long *;

typedef struct _FILETIME *LPFILETIME;

using FARPROC = int (*)();

using LPWORD = WORD *;

using LPCVOID = void *;
using LocaleCopyDefaultComputerLabelUtf16Proc = void (uint16_t * destination);
using LocaleFormatCurrentDateUtf16Proc = uint32_t (uint16_t * destination);
using LocaleFormatCurrentTimeUtf16Proc = uint32_t (uint16_t * destination);
using LocaleFormatTimeFieldsUtf16Proc = uint32_t (uint32_t hour, uint32_t minute, uint16_t * destination);
using LocaleGetPackedCurrentDateProc = uint32_t ();
using LocaleGetPackedCurrentTimeProc = uint32_t ();
using LocaleGetTelephoneCountryCodeProc = uint32_t ();
using TimerCallbackProc = void __cdecl ();
using TimerRegisterPeriodicProc = void (uint32_t frequencyHz, TimerCallbackProc * callback);
using TimerUnregisterPeriodicProc = void (TimerCallbackProc * callback);
using Win32PumpMessagesProc = void __cdecl ();

#endif /* THANDOR_PLATFORM_SYSTEM_TYPES_H */
