/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/image.cpp
 */

/* Own translation unit: uses the real Windows SDK headers, not generated/types.h. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thandor/platform/bootstrap/image.h>
#include <thandor/core/ptr32.h>

/* Instruction, frame and stack pointer of a CONTEXT and the StackWalk64 machine type of this build. The
   crash logs were written for x86; the x64 build logs the same through Rip/Rbp/Rsp (5f). */
#if defined(_M_IX86)
#define CRASH_MACHINE_TYPE IMAGE_FILE_MACHINE_I386
#define CONTEXT_PC(context) ((context).Eip)
#define CONTEXT_FP(context) ((context).Ebp)
#define CONTEXT_SP(context) ((context).Esp)
#elif defined(_M_X64)
#define CRASH_MACHINE_TYPE IMAGE_FILE_MACHINE_AMD64
#define CONTEXT_PC(context) ((context).Rip)
#define CONTEXT_FP(context) ((context).Rbp)
#define CONTEXT_SP(context) ((context).Rsp)
#else
#error "crash handler: unsupported architecture"
#endif

static void executable_directory(char *out, size_t capacity)
{
    char *slash;
    GetModuleFileNameA(NULL, out, (DWORD)capacity);
    slash = strrchr(out, '\\');
    if (slash != NULL) {
        slash[1] = '\0';
    }
}

void Thandor_Log(const char *format, ...)
{
    char path[MAX_PATH];
    FILE *out;
    va_list args;
    executable_directory(path, sizeof path);
    strcat_s(path, sizeof path, "thandor.log");
    if (fopen_s(&out, path, "a") != 0) {
        return;
    }
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fputc('\n', out);
    fclose(out);
}

/* Crash log: raw stack words below REBUILT_IMAGE_BASE + this are symbolized as code addresses (upper bound
   of the rebuilt executable's image) */
#define CRASH_LOG_REBUILT_IMAGE_SPAN 0x400000u
static void log_stack_thread(FILE *out, CONTEXT *start, HANDLE thread)
{
    HANDLE process = GetCurrentProcess();
    CONTEXT context = *start;
    STACKFRAME64 frame;
    int depth;
    static int initialized;

    if (!initialized) {
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(process, NULL, TRUE);
        initialized = 1;
    }
    memset(&frame, 0, sizeof frame);
    frame.AddrPC.Offset = CONTEXT_PC(context);
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = CONTEXT_FP(context);
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = CONTEXT_SP(context);
    frame.AddrStack.Mode = AddrModeFlat;
    for (depth = 0; depth < 48; depth++) {
        char buffer[sizeof(SYMBOL_INFO) + 256];
        SYMBOL_INFO *symbol = (SYMBOL_INFO *)buffer;
        IMAGEHLP_LINE64 line;
        DWORD64 displacement = 0;
        DWORD lineDisplacement = 0;
        if (!StackWalk64(CRASH_MACHINE_TYPE, process, thread, &frame, &context, NULL,
                         SymFunctionTableAccess64, SymGetModuleBase64, NULL) || frame.AddrPC.Offset == 0) {
            break;
        }
        memset(buffer, 0, sizeof buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;
        line.SizeOfStruct = sizeof line;
        if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
            fprintf(out, "  %2d  %08llX  %s+0x%llX", depth, frame.AddrPC.Offset, symbol->Name, displacement);
        } else {
            fprintf(out, "  %2d  %08llX  ?", depth, frame.AddrPC.Offset);
        }
        if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line)) {
            fprintf(out, "  (%s:%lu)", line.FileName, line.LineNumber);
        }
        fprintf(out, "\n");
    }
}

const char *Thandor_SymbolName(const void *address)
{
    static char name[256];
    char buffer[sizeof(SYMBOL_INFO) + 256];
    SYMBOL_INFO *symbol = (SYMBOL_INFO *)buffer;
    DWORD64 displacement = 0;
    static int initialized;
    if (!initialized) {
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(GetCurrentProcess(), NULL, TRUE);
        initialized = 1;
    }
    memset(buffer, 0, sizeof buffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;
    if (SymFromAddr(GetCurrentProcess(), (DWORD64)(uintptr_t)address, &displacement, symbol)) {
        sprintf_s(name, sizeof name, "%s+0x%llX", symbol->Name, displacement);
    } else {
        sprintf_s(name, sizeof name, "%p", address);
    }
    return name;
}

void Thandor_LogStack(const char *reason, unsigned value)
{
    char path[MAX_PATH];
    FILE *out;
    CONTEXT context;

    executable_directory(path, sizeof path);
    strcat_s(path, sizeof path, "thandor.log");
    if (fopen_s(&out, path, "a") != 0) {
        return;
    }
    fprintf(out, "%s 0x%08X\n", reason, value);
    memset(&context, 0, sizeof context);
    context.ContextFlags = CONTEXT_CONTROL;
#if defined(_M_IX86)
    /* RtlCaptureContext reads the caller's return address through EBP, which the optimized
       build does not keep as a frame pointer (EBP may be 0). Capture ESP/EBP/EIP directly. */
    {
        DWORD espValue;
        DWORD ebpValue;
        DWORD eipValue;
        __asm {
            mov espValue, esp
            mov ebpValue, ebp
            call here
        here:
            pop eax
            mov eipValue, eax
        }
        context.Esp = espValue;
        context.Ebp = ebpValue;
        context.Eip = eipValue;
    }
#else
    /* x64: the stack walk unwinds through the unwind tables, not a frame pointer */
    RtlCaptureContext(&context);
#endif
    log_stack_thread(out, &context, GetCurrentThread());
    fclose(out);
}

#if defined(_WIN64)
/* core/ptr32.h: a pointer of 2 GB or more was stored in a 32-bit field of an original layout. */
void Thandor_Ptr32Overflow(uintptr_t value)
{
    Thandor_Log("Ptr32: pointer 0x%llX does not fit a 32-bit field", (unsigned long long)value);
    Thandor_LogStack("Ptr32 overflow stack", (unsigned)value);
    ExitProcess(0xF5);
}
#endif

/* First thing in the crash filter: registers and raw stack words to crash_raw.log using only
   kernel32/user32 calls, so a corrupted CRT heap or stack cannot stop it. Symbolize the code
   addresses offline with build-x86/thandor.map. */
static void raw_crash_dump(EXCEPTION_POINTERS *info)
{
    char path[MAX_PATH];
    char line[256];
    char *slash;
    HANDLE file;
    DWORD written;
    const CONTEXT *c = info->ContextRecord;
    const DWORD *stack = (const DWORD *)(uintptr_t)CONTEXT_SP(*c);
    SYSTEMTIME now;
    int i;
    int n;

    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) {
        return;
    }
    slash = path;
    for (i = 0; path[i] != 0; i++) {
        if (path[i] == '\\') slash = path + i + 1;
    }
    lstrcpyA(slash, "crash_raw.log");
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }
    GetLocalTime(&now);
    n = wsprintfA(line, "\r\n==== %04u-%02u-%02u %02u:%02u:%02u ====\r\nexception %08lX at %08lX info %08lX %08lX\r\n",
                  now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
                  info->ExceptionRecord->ExceptionCode,
                  (DWORD)(uintptr_t)info->ExceptionRecord->ExceptionAddress,
                  (DWORD)info->ExceptionRecord->ExceptionInformation[0],
                  (DWORD)info->ExceptionRecord->ExceptionInformation[1]);
    WriteFile(file, line, n, &written, NULL);
#if defined(_M_IX86)
    n = wsprintfA(line, "eip=%08lX eax=%08lX ebx=%08lX ecx=%08lX edx=%08lX esi=%08lX edi=%08lX ebp=%08lX esp=%08lX\r\n",
                  c->Eip, c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
#else
    /* x64: each register as high and low dword (wsprintf has no 64-bit format) */
    n = wsprintfA(line, "rip=%08lX%08lX rax=%08lX%08lX rbp=%08lX%08lX rsp=%08lX%08lX\r\n",
                  (DWORD)(c->Rip >> 32), (DWORD)c->Rip, (DWORD)(c->Rax >> 32), (DWORD)c->Rax,
                  (DWORD)(c->Rbp >> 32), (DWORD)c->Rbp, (DWORD)(c->Rsp >> 32), (DWORD)c->Rsp);
#endif
    WriteFile(file, line, n, &written, NULL);
    for (i = 0; i < 512; i += 8) {
        if (!Thandor_IsReadable(stack + i, 32)) {
            break;
        }
        n = wsprintfA(line, "  +%03X: %08lX %08lX %08lX %08lX %08lX %08lX %08lX %08lX\r\n", i * 4,
                      stack[i], stack[i + 1], stack[i + 2], stack[i + 3], stack[i + 4], stack[i + 5],
                      stack[i + 6], stack[i + 7]);
        WriteFile(file, line, n, &written, NULL);
    }
    CloseHandle(file);
}

static LONG WINAPI crash_filter(EXCEPTION_POINTERS *info)
{
    char path[MAX_PATH];
    FILE *out;
    raw_crash_dump(info);
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    CONTEXT context = *info->ContextRecord;
    STACKFRAME64 frame;
    int depth;

    executable_directory(path, sizeof path);
    strcat_s(path, sizeof path, "crash.log");
    if (fopen_s(&out, path, "a") != 0) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    {
        SYSTEMTIME now;
        GetLocalTime(&now);
        fprintf(out, "\n==== %04u-%02u-%02u %02u:%02u:%02u ====\n", now.wYear, now.wMonth, now.wDay,
                now.wHour, now.wMinute, now.wSecond);
    }
    fprintf(out, "exception 0x%08lX at 0x%p\n", info->ExceptionRecord->ExceptionCode,
            info->ExceptionRecord->ExceptionAddress);
    if (info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        fprintf(out, "%s address 0x%08IX\n",
                info->ExceptionRecord->ExceptionInformation[0] == 8 ? "execute" :
                info->ExceptionRecord->ExceptionInformation[0] == 1 ? "write" : "read",
                info->ExceptionRecord->ExceptionInformation[1]);
    }
#if defined(_M_IX86)
    fprintf(out, "eax=%08lX ebx=%08lX ecx=%08lX edx=%08lX esi=%08lX edi=%08lX ebp=%08lX esp=%08lX\n\n",
            context.Eax, context.Ebx, context.Ecx, context.Edx, context.Esi, context.Edi, context.Ebp,
            context.Esp);
#else
    fprintf(out, "rax=%016llX rbx=%016llX rcx=%016llX rdx=%016llX\nrsi=%016llX rdi=%016llX rbp=%016llX rsp=%016llX\n\n",
            context.Rax, context.Rbx, context.Rcx, context.Rdx, context.Rsi, context.Rdi, context.Rbp,
            context.Rsp);
#endif
    fflush(out);
    /* Raw stack words first: the stack walk below can fault on a corrupted stack. */
    {
        const DWORD *stack = (const DWORD *)(uintptr_t)CONTEXT_SP(context);
        int i;
        fprintf(out, "stack:");
        for (i = 0; i < 96 && Thandor_IsReadable(stack + i, 4); i++) {
            fprintf(out, "%s%08lX", (i % 8) ? " " : "\n  ", stack[i]);
        }
        fprintf(out, "\n\n");
        fflush(out);
    }

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    SymInitialize(process, NULL, TRUE);
    /* Code addresses among the raw stack words (return addresses of the frames). */
    {
        const DWORD *stack = (const DWORD *)(uintptr_t)CONTEXT_SP(context);
        int i;
        for (i = 0; i < 96 && Thandor_IsReadable(stack + i, 4); i++) {
            if (stack[i] >= REBUILT_IMAGE_CODE_START && stack[i] < REBUILT_IMAGE_BASE + CRASH_LOG_REBUILT_IMAGE_SPAN) {
                fprintf(out, "  [esp+%03X] %s\n", i * 4,
                        Thandor_SymbolName((const void *)(uintptr_t)stack[i]));
            }
        }
        fprintf(out, "\n");
        fflush(out);
    }
    __try {
    memset(&frame, 0, sizeof frame);
    frame.AddrPC.Offset = CONTEXT_PC(context);
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = CONTEXT_FP(context);
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = CONTEXT_SP(context);
    frame.AddrStack.Mode = AddrModeFlat;
    for (depth = 0; depth < 64; depth++) {
        char buffer[sizeof(SYMBOL_INFO) + 256];
        SYMBOL_INFO *symbol = (SYMBOL_INFO *)buffer;
        IMAGEHLP_LINE64 line;
        DWORD64 displacement = 0;
        DWORD lineDisplacement = 0;
        if (!StackWalk64(CRASH_MACHINE_TYPE, process, thread, &frame, &context, NULL,
                         SymFunctionTableAccess64, SymGetModuleBase64, NULL) || frame.AddrPC.Offset == 0) {
            break;
        }
        memset(buffer, 0, sizeof buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;
        line.SizeOfStruct = sizeof line;
        if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
            fprintf(out, "%2d  %08llX  %s+0x%llX", depth, frame.AddrPC.Offset, symbol->Name, displacement);
        } else {
            HMODULE module;
            char moduleName[MAX_PATH];
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   (LPCSTR)(uintptr_t)frame.AddrPC.Offset, &module) &&
                GetModuleFileNameA(module, moduleName, sizeof moduleName)) {
                const char *base = strrchr(moduleName, '\\');
                fprintf(out, "%2d  %08llX  %s+0x%llX", depth, frame.AddrPC.Offset,
                        base ? base + 1 : moduleName,
                        frame.AddrPC.Offset - (DWORD64)(uintptr_t)module);
            } else {
                fprintf(out, "%2d  %08llX  ?", depth, frame.AddrPC.Offset);
            }
        }
        if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line)) {
            fprintf(out, "  (%s:%lu)", line.FileName, line.LineNumber);
        }
        fprintf(out, "\n");
    }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        fprintf(out, "(stack walk faulted)\n");
    }
    fclose(out);
    return EXCEPTION_CONTINUE_SEARCH;
}

static HANDLE g_watchedThread;

#ifdef THANDOR_DEV_TOOLS
/* Diagnostics (developer tools): with OPEN_THANDOR_WATCHDOG=<seconds>, log the main thread's stack at that
   interval. */
static DWORD WINAPI watchdog_thread(void *parameter)
{
    DWORD interval = (DWORD)(uintptr_t)parameter;
    for (;;) {
        char path[MAX_PATH];
        FILE *out;
        CONTEXT context;
        Sleep(interval * 1000);
        executable_directory(path, sizeof path);
        strcat_s(path, sizeof path, "thandor.log");
        if (fopen_s(&out, path, "a") != 0) {
            continue;
        }
        SuspendThread(g_watchedThread);
        memset(&context, 0, sizeof context);
        context.ContextFlags = CONTEXT_FULL;
        if (GetThreadContext(g_watchedThread, &context)) {
            fprintf(out, "watchdog: main thread at %s\n", Thandor_SymbolName((void *)(uintptr_t)CONTEXT_PC(context)));
            log_stack_thread(out, &context, g_watchedThread);
        }
        ResumeThread(g_watchedThread);
        fclose(out);
    }
}

static void start_watchdog(void)
{
    char seconds[16];
    if (GetEnvironmentVariableA("OPEN_THANDOR_WATCHDOG", seconds, sizeof seconds) != 0 && atoi(seconds) > 0) {
        CreateThread(NULL, 0, watchdog_thread, (void *)(uintptr_t)atoi(seconds), 0, NULL);
    }
}
#else
#define start_watchdog() ((void)0)
#endif

volatile long g_ThandorFrameHeartbeat;

/* Once frames are being presented, a stall of HANG_SECONDS logs three stack samples of the main
   thread (one second apart) to hang.log, so an endless loop shows which code it spins in. */
#define HANG_SECONDS 4

static DWORD WINAPI hang_detector_thread(void *parameter)
{
    long last = 0;
    int stalled = 0;
    int reported = 0;
    (void)parameter;
    for (;;) {
        long now;
        Sleep(1000);
        now = g_ThandorFrameHeartbeat;
        if (now != last || now == 0) {
            last = now;
            stalled = 0;
            reported = 0;
            continue;
        }
        if (++stalled < HANG_SECONDS || reported >= 3) {
            continue;
        }
        {
            char path[MAX_PATH];
            FILE *out;
            CONTEXT context;
            executable_directory(path, sizeof path);
            strcat_s(path, sizeof path, "hang.log");
            if (fopen_s(&out, path, "a") != 0) {
                continue;
            }
            if (reported == 0) {
                SYSTEMTIME time;
                GetLocalTime(&time);
                fprintf(out, "\n==== %04u-%02u-%02u %02u:%02u:%02u no frame for %d s ====\n", time.wYear,
                        time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, stalled);
            }
            SuspendThread(g_watchedThread);
            memset(&context, 0, sizeof context);
            context.ContextFlags = CONTEXT_FULL;
            if (GetThreadContext(g_watchedThread, &context)) {
                fprintf(out, "sample %d: main thread at %s\n", reported + 1,
                        Thandor_SymbolName((void *)(uintptr_t)CONTEXT_PC(context)));
                log_stack_thread(out, &context, g_watchedThread);
            }
            ResumeThread(g_watchedThread);
            fclose(out);
            reported++;
        }
    }
}

void Thandor_InstallCrashHandler(void)
{
    SetUnhandledExceptionFilter(crash_filter);
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_watchedThread, 0, FALSE,
                    DUPLICATE_SAME_ACCESS);
    CreateThread(NULL, 0, hang_detector_thread, NULL, 0, NULL);
    start_watchdog();
}

int Thandor_IsReadable(const void *address, unsigned size)
{
    MEMORY_BASIC_INFORMATION mbi;
    const char *p = (const char *)address;
    const char *end = p + size;
    while (p < end) {
        if (VirtualQuery(p, &mbi, sizeof mbi) == 0 || mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) {
            return 0;
        }
        p = (const char *)mbi.BaseAddress + mbi.RegionSize;
    }
    return 1;
}

unsigned Thandor_TickCount(void)
{
    return GetTickCount();
}

void Thandor_SleepMs(unsigned milliseconds)
{
    Sleep(milliseconds);
}

int Thandor_DirectoryExistsW(const unsigned short *path)
{
    DWORD attributes = GetFileAttributesW((const wchar_t *)path);
    return (attributes != INVALID_FILE_ATTRIBUTES) && ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

void Thandor_GetExecutablePathA(char *out, unsigned capacity)
{
    DWORD length = GetModuleFileNameA(NULL, out, capacity);
    if (length == 0 || length >= capacity) {
        out[0] = 0;
    }
}
