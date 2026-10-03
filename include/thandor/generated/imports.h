/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * Import prototypes of the Windows functions the game calls (to be replaced by the SDK headers).
 */

#ifndef THANDOR_GENERATED_IMPORTS_H
#define THANDOR_GENERATED_IMPORTS_H

#include <thandor/generated/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* KERNEL32.DLL */
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE hObject);
__declspec(dllimport) BOOL __stdcall CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
__declspec(dllimport) BOOL __stdcall CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
__declspec(dllimport) HANDLE __stdcall CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
__declspec(dllimport) HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) HANDLE __stdcall CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
__declspec(dllimport) HANDLE __stdcall CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName);
__declspec(dllimport) HANDLE __stdcall CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
__declspec(dllimport) BOOL __stdcall DeleteFileA(LPCSTR lpFileName);
__declspec(dllimport) BOOL __stdcall DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) void __stdcall ExitProcess(UINT uExitCode);
__declspec(dllimport) void __stdcall ExitThread(DWORD dwExitCode);
__declspec(dllimport) BOOL __stdcall FileTimeToDosDateTime(FILETIME * lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime);
__declspec(dllimport) BOOL __stdcall FindClose(HANDLE hFindFile);
__declspec(dllimport) HANDLE __stdcall FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
__declspec(dllimport) BOOL __stdcall FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
__declspec(dllimport) DWORD __stdcall FormatMessageA(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPSTR lpBuffer, DWORD nSize, va_list * Arguments);
__declspec(dllimport) LPSTR __stdcall GetCommandLineA(void);
__declspec(dllimport) BOOL __stdcall GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize);
__declspec(dllimport) BOOL __stdcall GetConsoleScreenBufferInfo(HANDLE hConsoleOutput, PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo);
__declspec(dllimport) DWORD __stdcall GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
__declspec(dllimport) HANDLE __stdcall GetCurrentProcess(void);
__declspec(dllimport) HANDLE __stdcall GetCurrentThread(void);
__declspec(dllimport) BOOL __stdcall GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters);
__declspec(dllimport) UINT __stdcall GetDriveTypeA(LPCSTR lpRootPathName);
__declspec(dllimport) DWORD __stdcall GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
__declspec(dllimport) BOOL __stdcall GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
__declspec(dllimport) DWORD __stdcall GetLastError(void);
__declspec(dllimport) void __stdcall GetLocalTime(LPSYSTEMTIME lpSystemTime);
__declspec(dllimport) int __stdcall GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData);
__declspec(dllimport) DWORD __stdcall GetLogicalDrives(void);
__declspec(dllimport) HMODULE __stdcall GetModuleHandleA(LPCSTR lpModuleName);
__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
__declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
__declspec(dllimport) UINT __stdcall GetTempFileNameA(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPSTR lpTempFileName);
__declspec(dllimport) DWORD __stdcall GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer);
__declspec(dllimport) LCID __stdcall GetUserDefaultLCID(void);
__declspec(dllimport) BOOL __stdcall GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
__declspec(dllimport) LPVOID __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
__declspec(dllimport) HANDLE __stdcall HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
__declspec(dllimport) BOOL __stdcall HeapDestroy(HANDLE hHeap);
__declspec(dllimport) BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) LPVOID __stdcall HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
__declspec(dllimport) HLOCAL __stdcall LocalFree(HLOCAL hMem);
__declspec(dllimport) LPVOID __stdcall MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap);
__declspec(dllimport) BOOL __stdcall MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
__declspec(dllimport) BOOL __stdcall ReadConsoleA(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumberOfCharsRead, PCONSOLE_READCONSOLE_CONTROL pInputControl);
__declspec(dllimport) BOOL __stdcall ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
__declspec(dllimport) BOOL __stdcall RemoveDirectoryA(LPCSTR lpPathName);
__declspec(dllimport) DWORD __stdcall ResumeThread(HANDLE hThread);
__declspec(dllimport) BOOL __stdcall SetConsoleCursorPosition(HANDLE hConsoleOutput, COORD dwCursorPosition);
__declspec(dllimport) BOOL __stdcall SetCurrentDirectoryA(LPCSTR lpPathName);
__declspec(dllimport) BOOL __stdcall SetEndOfFile(HANDLE hFile);
__declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
__declspec(dllimport) BOOL __stdcall SetPriorityClass(HANDLE hProcess, DWORD dwPriorityClass);
__declspec(dllimport) BOOL __stdcall SetThreadPriority(HANDLE hThread, int nPriority);
__declspec(dllimport) DWORD __stdcall SuspendThread(HANDLE hThread);
__declspec(dllimport) BOOL __stdcall UnmapViewOfFile(LPCVOID lpBaseAddress);
__declspec(dllimport) BOOL __stdcall WriteConsoleA(HANDLE hConsoleOutput, void * lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

/* USER32.DLL */
__declspec(dllimport) HWND __stdcall CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
__declspec(dllimport) LRESULT __stdcall DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
__declspec(dllimport) BOOL __stdcall DestroyWindow(HWND hWnd);
__declspec(dllimport) LRESULT __stdcall DispatchMessageA(MSG * lpMsg);
__declspec(dllimport) HWND __stdcall FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);
__declspec(dllimport) HWND __stdcall GetDesktopWindow(void);
__declspec(dllimport) SHORT __stdcall GetKeyState(int nVirtKey);
__declspec(dllimport) int __stdcall GetSystemMetrics(int nIndex);
__declspec(dllimport) HCURSOR __stdcall LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
__declspec(dllimport) HICON __stdcall LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName);
__declspec(dllimport) UINT __stdcall MapVirtualKeyA(UINT uCode, UINT uMapType);
__declspec(dllimport) int __stdcall MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
__declspec(dllimport) DWORD __stdcall MsgWaitForMultipleObjects(DWORD nCount, HANDLE * pHandles, BOOL fWaitAll, DWORD dwMilliseconds, DWORD dwWakeMask);
__declspec(dllimport) BOOL __stdcall PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
__declspec(dllimport) ATOM __stdcall RegisterClassA(WNDCLASSA * lpWndClass);
__declspec(dllimport) HCURSOR __stdcall SetCursor(HCURSOR hCursor);
__declspec(dllimport) BOOL __stdcall ShowWindow(HWND hWnd, int nCmdShow);
__declspec(dllimport) BOOL __stdcall TranslateMessage(MSG * lpMsg);
__declspec(dllimport) BOOL __stdcall UpdateWindow(HWND hWnd);

#ifdef __cplusplus
}
#endif

#endif /* THANDOR_GENERATED_IMPORTS_H */
