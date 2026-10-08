/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * Import prototypes of the Windows functions the game calls (to be replaced by the SDK headers).
 */

#ifndef THANDOR_GENERATED_IMPORTS_H
#define THANDOR_GENERATED_IMPORTS_H

#include <thandor/core/types.h>
#include <thandor/platform/filesystem/types.h>
#include <thandor/platform/system/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* KERNEL32.DLL */
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE hObject);
__declspec(dllimport) BOOL __stdcall CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
__declspec(dllimport) BOOL __stdcall CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
__declspec(dllimport) HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) HANDLE __stdcall CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName);
__declspec(dllimport) HANDLE __stdcall CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
__declspec(dllimport) BOOL __stdcall DeleteFileA(LPCSTR lpFileName);
__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(UINT uExitCode);
__declspec(dllimport) BOOL __stdcall FileTimeToDosDateTime(FILETIME * lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime);
__declspec(dllimport) BOOL __stdcall FindClose(HANDLE hFindFile);
__declspec(dllimport) HANDLE __stdcall FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
__declspec(dllimport) BOOL __stdcall FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
__declspec(dllimport) LPSTR __stdcall GetCommandLineA(void);
__declspec(dllimport) BOOL __stdcall GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize);
__declspec(dllimport) DWORD __stdcall GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
__declspec(dllimport) HANDLE __stdcall GetCurrentProcess(void);
__declspec(dllimport) HANDLE __stdcall GetCurrentThread(void);
__declspec(dllimport) BOOL __stdcall GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters);
__declspec(dllimport) UINT __stdcall GetDriveTypeA(LPCSTR lpRootPathName);
__declspec(dllimport) DWORD __stdcall GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
__declspec(dllimport) BOOL __stdcall GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
__declspec(dllimport) void __stdcall GetLocalTime(LPSYSTEMTIME lpSystemTime);
__declspec(dllimport) int __stdcall GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData);
__declspec(dllimport) DWORD __stdcall GetLogicalDrives(void);
__declspec(dllimport) HMODULE __stdcall GetModuleHandleA(LPCSTR lpModuleName);
__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
__declspec(dllimport) LCID __stdcall GetUserDefaultLCID(void);
__declspec(dllimport) BOOL __stdcall GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
__declspec(dllimport) LPVOID __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
__declspec(dllimport) HANDLE __stdcall HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
__declspec(dllimport) BOOL __stdcall HeapDestroy(HANDLE hHeap);
__declspec(dllimport) BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) BOOL __stdcall MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
__declspec(dllimport) BOOL __stdcall MoveFileExA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags);
__declspec(dllimport) BOOL __stdcall ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
__declspec(dllimport) BOOL __stdcall RemoveDirectoryA(LPCSTR lpPathName);
__declspec(dllimport) DWORD __stdcall ResumeThread(HANDLE hThread);
__declspec(dllimport) BOOL __stdcall SetCurrentDirectoryA(LPCSTR lpPathName);
__declspec(dllimport) BOOL __stdcall SetEndOfFile(HANDLE hFile);
__declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
__declspec(dllimport) BOOL __stdcall SetPriorityClass(HANDLE hProcess, DWORD dwPriorityClass);
__declspec(dllimport) BOOL __stdcall SetThreadPriority(HANDLE hThread, int nPriority);
__declspec(dllimport) DWORD __stdcall SuspendThread(HANDLE hThread);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

/* USER32.DLL */
__declspec(dllimport) BOOL __stdcall DestroyWindow(HWND hWnd);
__declspec(dllimport) HWND __stdcall FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);
__declspec(dllimport) int __stdcall MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
__declspec(dllimport) DWORD __stdcall MsgWaitForMultipleObjects(DWORD nCount, HANDLE * pHandles, BOOL fWaitAll, DWORD dwMilliseconds, DWORD dwWakeMask);

#ifdef __cplusplus
}
#endif

#endif /* THANDOR_GENERATED_IMPORTS_H */
