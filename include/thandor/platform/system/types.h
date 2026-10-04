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

/* Types (split from generated/types.h by tools/dev/split_types.py). */

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

typedef uint32_t DWORD;

typedef DWORD LCTYPE;

typedef uintptr_t ULONG_PTR; /* pointer-sized, as in the Windows SDK */

typedef void *HANDLE;

typedef void *PVOID;

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

typedef void *LPVOID;

typedef int BOOL;

struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
};

typedef uint16_t WORD;

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

typedef char CHAR;

typedef struct _OVERLAPPED *LPOVERLAPPED;

typedef DWORD (__stdcall *PTHREAD_START_ROUTINE)(LPVOID);

typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;

typedef struct _WIN32_FIND_DATAA *LPWIN32_FIND_DATAA;

typedef struct _SYSTEMTIME *LPSYSTEMTIME;

typedef short SHORT;

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

typedef uint32_t ULONG;

struct _CONSOLE_READCONSOLE_CONTROL {
    ULONG nLength;
    ULONG nInitialChars;
    ULONG dwCtrlWakeupMask;
    ULONG dwControlKeyState;
};

typedef struct _CONSOLE_SCREEN_BUFFER_INFO *PCONSOLE_SCREEN_BUFFER_INFO;

typedef char *va_list;

typedef struct HWND__ *HWND;

typedef uint32_t UINT;

typedef long LONG;

struct HWND__ {
    int unused;
};

typedef CHAR *LPCSTR;

typedef LONG *PLONG;

typedef CHAR *LPSTR;

typedef DWORD LCID;

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
typedef int LocaleTelephoneCountryCode;

typedef uint16_t Win32CalendarYear16;

typedef uint32_t LocaleCalendarDayStack32;

typedef uint16_t Win32CalendarMonth16;

typedef uint16_t Win32CalendarDay16;

typedef uint16_t Win32Millisecond16;

typedef uint32_t LocaleClockHourStack32;

typedef uint16_t Win32Hour16;

typedef uint16_t Win32DayOfWeek16;

typedef uint32_t LocaleClockMinuteStack32;

typedef uint16_t Win32Second16;

typedef uint32_t LocaleCalendarMonthStack32;

typedef uint32_t LocaleCalendarYearStack32;

typedef uint16_t Win32Minute16;

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

typedef ULONG_PTR SIZE_T;

typedef DWORD *LPDWORD;

typedef HINSTANCE HMODULE;

typedef HANDLE HLOCAL;

typedef long *LPLONG;

typedef struct _FILETIME *LPFILETIME;

typedef int (*FARPROC)(void);

typedef WORD *LPWORD;

typedef void *LPCVOID;
typedef uint32_t __cdecl CpuDetectFeaturesProc(void);
typedef void LocaleCopyDefaultComputerLabelUtf16Proc(uint16_t * destination);
typedef uint32_t LocaleFormatCurrentDateUtf16Proc(uint16_t * destination);
typedef uint32_t LocaleFormatCurrentTimeUtf16Proc(uint16_t * destination);
typedef uint32_t LocaleFormatDateFieldsUtf16Proc(uint32_t year, uint32_t month, uint32_t day, uint16_t * destination);
typedef uint32_t LocaleFormatTimeFieldsUtf16Proc(uint32_t hour, uint32_t minute, uint16_t * destination);
typedef uint32_t LocaleGetPackedCurrentDateProc(void);
typedef uint32_t LocaleGetPackedCurrentTimeProc(void);
typedef uint32_t LocaleGetTelephoneCountryCodeProc(void);
typedef void __cdecl TimerCallbackProc(void);
typedef void TimerRegisterPeriodicProc(uint32_t frequencyHz, TimerCallbackProc * callback);
typedef void TimerUnregisterPeriodicProc(TimerCallbackProc * callback);
typedef void __cdecl Win32PumpMessagesProc(void);

#endif /* THANDOR_PLATFORM_SYSTEM_TYPES_H */
