/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/image.cpp
 */

/* Own translation unit: uses the real Windows SDK headers, not the game's type headers. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thandor/platform/bootstrap/image.h>
#include <thandor/core/ptr32.h>

/* Instruction, frame and stack pointer of a CONTEXT and the StackWalk64 machine type of this (x64) build. */
#define CRASH_MACHINE_TYPE IMAGE_FILE_MACHINE_AMD64
#define CONTEXT_PC(context) ((context).Rip)
#define CONTEXT_FP(context) ((context).Rbp)
#define CONTEXT_SP(context) ((context).Rsp)

/* A register or stack value of a captured context taken as an address in this process. */
static const void *register_address(DWORD64 value)
{
    return reinterpret_cast<const void *>(static_cast<uintptr_t>(value)); /* the value is a native address */
}

/* The first code byte of a function, to measure its offset from the image base. */
static const BYTE *code_bytes(void (*function)())
{
    return reinterpret_cast<const BYTE *>(reinterpret_cast<uintptr_t>(function)); /* code address as bytes */
}

static void executable_directory(char *out, size_t capacity)
{
    char *slash;
    GetModuleFileNameA(nullptr, out, (DWORD)capacity);
    slash = strrchr(out, '\\');
    if (slash != nullptr) {
        slash[1] = '\0';
    }
}

void Thandor_Log(const char *format, ...)
{
    char path[MAX_PATH];
    FILE *out;
    va_list args;
    executable_directory(path, sizeof path);
    strncat(path, "thandor.log", sizeof path - strlen(path) - 1);
    if ((out = fopen(path, "a")) == nullptr) {
        return;
    }
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fputc('\n', out);
    fclose(out);
}

/* `module+0xoffset` of a code address: the fallback without a symbol (addr2line or tools/data/symbolize.py
   symbolizes the offset offline, docs/BUILDING.md). 0 when no module contains the address. */
static int module_offset(char *out, size_t capacity, DWORD64 address)
{
    HMODULE module;
    char moduleName[MAX_PATH];
    const char *base;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            static_cast<LPCSTR>(register_address(address)), &module) ||
        !GetModuleFileNameA(module, moduleName, sizeof moduleName)) {
        return 0;
    }
    base = strrchr(moduleName, '\\');
    snprintf(out, capacity, "%s+0x%llX", base ? base + 1 : moduleName,
             (unsigned long long)(address - (DWORD64)reinterpret_cast<uintptr_t>(module)));
    return 1;
}

/*
Symbol table of a GCC build (no PDB for dbghelp): thandor.sym next to the executable, written after the link by
cmake/symbol_table.cmake from `nm -C --defined-only -n` - a first line "<address> A __ImageBase" (the link-time
image base), then the code symbols "<address> <T|t|W|w> <demangled name>" sorted by address.
Thandor_InstallCrashHandler reads it once into memory allocated then; the lookups of the crash, hang and watchdog
logs only read that memory (no allocation or file access while the process is crashing). A table that does not
belong to this executable (Thandor_InstallCrashHandler not found at its own address) is dropped.
*/
struct SymbolTableEntry {
    uint32_t rva;        /* offset from the image base */
    uint32_t nameOffset; /* into g_SymbolTableText, zero-terminated */
};

static char *g_SymbolTableText;
static SymbolTableEntry *g_SymbolTableEntries;
static uint32_t g_SymbolTableCount;
/* image offsets of the executable sections: only code addresses get a name (not data words on the stack) */
static uint32_t g_SymbolTableCodeStart;
static uint32_t g_SymbolTableCodeEnd;

/* The table entry of the function containing image offset rva (the last symbol at or below it); NULL if none. */
static const SymbolTableEntry *symbol_table_find(uint64_t rva)
{
    uint32_t low = 0;
    uint32_t high = g_SymbolTableCount;
    if (g_SymbolTableCount == 0 || rva < g_SymbolTableCodeStart || rva >= g_SymbolTableCodeEnd ||
        rva < g_SymbolTableEntries[0].rva) {
        return nullptr;
    }
    while (high - low > 1) {
        uint32_t middle = low + (high - low) / 2;
        if (g_SymbolTableEntries[middle].rva <= rva) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return &g_SymbolTableEntries[low];
}

/* "Function+0xNN [thandor.exe+0xOFFSET]" of a code address in this executable from thandor.sym; 0 when the
   table is not loaded or does not cover the address. */
static int symbol_table_name(char *out, size_t capacity, DWORD64 address)
{
    const SymbolTableEntry *entry;
    DWORD64 base = (DWORD64)reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
    if (g_SymbolTableCount == 0 || address < base || (entry = symbol_table_find(address - base)) == nullptr) {
        return 0;
    }
    snprintf(out, capacity, "%s+0x%llX [thandor.exe+0x%llX]", g_SymbolTableText + entry->nameOffset,
             (unsigned long long)(address - base - entry->rva), (unsigned long long)(address - base));
    return 1;
}

static void symbol_table_drop()
{
    if (g_SymbolTableEntries != nullptr) {
        VirtualFree(g_SymbolTableEntries, 0, MEM_RELEASE);
    }
    if (g_SymbolTableText != nullptr) {
        VirtualFree(g_SymbolTableText, 0, MEM_RELEASE);
    }
    g_SymbolTableEntries = nullptr;
    g_SymbolTableText = nullptr;
    g_SymbolTableCount = 0;
}

static void symbol_table_load()
{
    char path[MAX_PATH];
    HANDLE file;
    LARGE_INTEGER size;
    DWORD bytesRead = 0;
    uint32_t lineCount = 0;
    unsigned long long imageBase = 0;
    int haveImageBase = 0;
    char *cursor;
    const BYTE *module = reinterpret_cast<const BYTE *>(GetModuleHandleA(nullptr)); /* the handle is the image base */
    /* the PE headers of the loaded image, read in place */
    const IMAGE_NT_HEADERS *headers = reinterpret_cast<const IMAGE_NT_HEADERS *>(
        module + reinterpret_cast<const IMAGE_DOS_HEADER *>(module)->e_lfanew);
    const IMAGE_SECTION_HEADER *section = IMAGE_FIRST_SECTION(headers);
    const SymbolTableEntry *self;
    WORD sectionIndex;

    executable_directory(path, sizeof path);
    if (strlen(path) + sizeof "thandor.sym" > sizeof path) {
        return;
    }
    strcat(path, "thandor.sym");
    file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }
    if (GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart < 0x4000000) {
        g_SymbolTableText = static_cast<char *>(
            VirtualAlloc(nullptr, (SIZE_T)size.QuadPart + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (g_SymbolTableText != nullptr &&
            (!ReadFile(file, g_SymbolTableText, (DWORD)size.QuadPart, &bytesRead, nullptr) ||
             bytesRead != (DWORD)size.QuadPart)) {
            bytesRead = 0;
        }
    }
    CloseHandle(file);
    if (bytesRead == 0) {
        symbol_table_drop();
        return;
    }
    g_SymbolTableText[bytesRead] = '\0';
    for (cursor = g_SymbolTableText; *cursor != '\0'; cursor++) {
        lineCount += (*cursor == '\n');
    }
    g_SymbolTableEntries = static_cast<SymbolTableEntry *>(VirtualAlloc(
        nullptr, ((SIZE_T)lineCount + 1) * sizeof(SymbolTableEntry), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (g_SymbolTableEntries == nullptr) {
        symbol_table_drop();
        return;
    }
    g_SymbolTableCodeStart = headers->OptionalHeader.SizeOfImage;
    g_SymbolTableCodeEnd = 0;
    for (sectionIndex = 0; sectionIndex < headers->FileHeader.NumberOfSections; sectionIndex++, section++) {
        if ((section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0) {
            if (section->VirtualAddress < g_SymbolTableCodeStart) {
                g_SymbolTableCodeStart = section->VirtualAddress;
            }
            if (section->VirtualAddress + section->Misc.VirtualSize > g_SymbolTableCodeEnd) {
                g_SymbolTableCodeEnd = section->VirtualAddress + section->Misc.VirtualSize;
            }
        }
    }
    /* each line: <hex address> <type> <name>; names are terminated in place */
    cursor = g_SymbolTableText;
    while (*cursor != '\0') {
        char *end;
        char *name;
        char type;
        unsigned long long address = strtoull(cursor, &end, 16);
        char *lineEnd = strchr(cursor, '\n');
        if (lineEnd == nullptr) {
            lineEnd = cursor + strlen(cursor);
        }
        if (end != cursor && end[0] == ' ' && end[1] != '\0' && end[2] == ' ' && end + 3 <= lineEnd) {
            type = end[1];
            name = end + 3;
            char *nameEnd = lineEnd;
            if (nameEnd > name && nameEnd[-1] == '\r') {
                nameEnd--;
            }
            *nameEnd = '\0';
            if (type == 'A' && strcmp(name, "__ImageBase") == 0) {
                imageBase = address;
                haveImageBase = 1;
            } else if (haveImageBase && (type == 'T' || type == 't' || type == 'W' || type == 'w') &&
                       name[0] != '.' && address >= imageBase && address - imageBase < g_SymbolTableCodeEnd) {
                g_SymbolTableEntries[g_SymbolTableCount].rva = (uint32_t)(address - imageBase);
                g_SymbolTableEntries[g_SymbolTableCount].nameOffset = (uint32_t)(name - g_SymbolTableText);
                g_SymbolTableCount++;
            }
        }
        cursor = *lineEnd != '\0' ? lineEnd + 1 : lineEnd;
    }
    /* the table must name this very function at its own address, else it belongs to another build */
    self = symbol_table_find((uint64_t)(code_bytes(&Thandor_InstallCrashHandler) - module));
    if (self == nullptr || self->rva != (uint32_t)(code_bytes(&Thandor_InstallCrashHandler) - module) ||
        strncmp(g_SymbolTableText + self->nameOffset, "Thandor_InstallCrashHandler",
                sizeof "Thandor_InstallCrashHandler" - 1) != 0) {
        symbol_table_drop();
        Thandor_Log("thandor.sym does not belong to this thandor.exe (ignored)");
    }
}

/* dbghelp is single-threaded, and the hang detector, the dev watchdog, Thandor_LogStack / Thandor_SymbolName
   (any thread) and the crash filter all use it: one process-wide lock around every dbghelp call (SymSetOptions,
   SymInitialize, SymFromAddr, StackWalk64, SymGetLineFromAddr64). It is a spin lock on the owning thread id
   instead of a std::mutex or CRITICAL_SECTION because the crash filter must never block on it: a thread can
   fault while it holds the lock (inside a stack walk of Thandor_LogStack), or hold it while another thread
   crashes. A CRITICAL_SECTION is recursive, so the faulting owner would re-enter dbghelp in its broken state,
   and try_lock on a std::mutex the caller owns is undefined; the owner id lets the filter see "held by me" and
   skip at once, and a bounded wait covers another holder. Contention is rare (diagnostic threads only). */
static volatile LONG g_dbghelpOwner; /* thread id of the holder, 0 = free (no thread has id 0) */
constexpr DWORD DBGHELP_CRASH_WAIT_MS = 2000;

static void dbghelp_lock()
{
    LONG self = static_cast<LONG>(GetCurrentThreadId());
    while (InterlockedCompareExchange(&g_dbghelpOwner, self, 0) != 0) {
        Sleep(1);
    }
}

/* Crash filter: false at once when the calling thread already holds the lock, else after waitMs without it. */
static bool dbghelp_try_lock(DWORD waitMs)
{
    LONG self = static_cast<LONG>(GetCurrentThreadId());
    DWORD start = GetTickCount();
    for (;;) {
        LONG owner = InterlockedCompareExchange(&g_dbghelpOwner, self, 0);
        if (owner == 0) {
            return true;
        }
        if (owner == self || GetTickCount() - start >= waitMs) {
            return false;
        }
        Sleep(1);
    }
}

static void dbghelp_unlock()
{
    InterlockedExchange(&g_dbghelpOwner, 0);
}

/* SymInitialize once for Thandor_SymbolName and log_stack_thread (the crash filter calls it itself); call with
   the lock held. */
static void dbghelp_initialize()
{
    static bool initialized;
    if (!initialized) {
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(GetCurrentProcess(), nullptr, TRUE);
        initialized = true;
    }
}

/* Name of a code address for the logs: thandor.sym (GCC build), else dbghelp (the PDB of an MSVC build;
   SymInitialize done and the dbghelp lock held by the caller; skipped when useDbghelp is false), else
   module+offset, else "?". */
static void code_address_name(char *out, size_t capacity, HANDLE process, DWORD64 address, bool useDbghelp)
{
    char buffer[sizeof(SYMBOL_INFO) + 256];
    SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO *>(buffer); /* dbghelp's variable-length record in the byte buffer */
    DWORD64 displacement = 0;
    if (symbol_table_name(out, capacity, address)) {
        return;
    }
    memset(buffer, 0, sizeof buffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;
    if (useDbghelp && SymFromAddr(process, address, &displacement, symbol)) {
        snprintf(out, capacity, "%s+0x%llX", symbol->Name, (unsigned long long)displacement);
    } else if (!module_offset(out, capacity, address)) {
        snprintf(out, capacity, "?");
    }
}

/* Hang detector and dev watchdog: what is taken of the main thread while it is suspended - its context and a raw
   copy of the top of its stack. The suspended thread may hold the process heap lock, the loader lock or dbghelp's
   lock, so between SuspendThread and ResumeThread nothing may allocate, take a user-mode lock or call dbghelp
   (only GetThreadContext, VirtualQuery and ReadProcessMemory, which are plain system calls, into this fixed
   buffer); the stack walk and the symbol lookups run on the copies after the thread is resumed. */
constexpr auto STACK_SNAPSHOT_BYTES = 0x10000u;
struct StackSnapshot {
    CONTEXT context;
    DWORD64 stackStart; /* address of stack[0]: the stack pointer of the captured context */
    SIZE_T stackBytes;  /* bytes copied, 0 when the stack could not be read */
    BYTE stack[STACK_SNAPSHOT_BYTES];
};

/* Suspends thread, copies its context and up to STACK_SNAPSHOT_BYTES of its stack from the stack pointer up
   (bounded by the committed region), resumes it. false when the context could not be read. */
static bool capture_thread(HANDLE thread, StackSnapshot *snapshot)
{
    MEMORY_BASIC_INFORMATION region;
    bool captured;
    SuspendThread(thread);
    memset(&snapshot->context, 0, sizeof snapshot->context);
    snapshot->context.ContextFlags = CONTEXT_FULL;
    snapshot->stackStart = 0;
    snapshot->stackBytes = 0;
    captured = GetThreadContext(thread, &snapshot->context) != FALSE;
    if (captured && VirtualQuery(register_address(CONTEXT_SP(snapshot->context)), &region, sizeof region) != 0 &&
        region.State == MEM_COMMIT) {
        const BYTE *start = static_cast<const BYTE *>(register_address(CONTEXT_SP(snapshot->context)));
        const BYTE *end = static_cast<const BYTE *>(region.BaseAddress) + region.RegionSize;
        SIZE_T bytes = static_cast<SIZE_T>(end - start);
        SIZE_T copied = 0;
        if (bytes > sizeof snapshot->stack) {
            bytes = sizeof snapshot->stack;
        }
        /* ReadProcessMemory reports an unreadable page instead of faulting */
        if (ReadProcessMemory(GetCurrentProcess(), start, snapshot->stack, bytes, &copied) != FALSE) {
            snapshot->stackStart = CONTEXT_SP(snapshot->context);
            snapshot->stackBytes = copied;
        }
    }
    ResumeThread(thread);
    return captured;
}

/* The snapshot whose stack copy log_stack_thread's walk on this thread reads (nullptr: read live memory). */
static thread_local const StackSnapshot *t_walkSnapshot;

/* StackWalk64 memory reader: reads inside the copied stack range come from the snapshot (the thread has run on
   since), everything else (code bytes, unwind data, a stack deeper than the copy) from the process. */
static BOOL CALLBACK read_walk_memory(HANDLE process, DWORD64 address, PVOID buffer, DWORD size, LPDWORD read)
{
    const StackSnapshot *snapshot = t_walkSnapshot;
    SIZE_T copied = 0;
    BOOL ok;
    if (snapshot != nullptr && address >= snapshot->stackStart &&
        address - snapshot->stackStart <= snapshot->stackBytes &&
        size <= snapshot->stackBytes - (address - snapshot->stackStart)) {
        memcpy(buffer, snapshot->stack + (address - snapshot->stackStart), size);
        *read = size;
        return TRUE;
    }
    ok = ReadProcessMemory(process, register_address(address), buffer, size, &copied);
    *read = static_cast<DWORD>(copied);
    return ok;
}

/* Crash log: raw stack qwords below REBUILT_IMAGE_BASE + this are symbolized as code addresses (upper bound
   of the rebuilt executable's image) */
constexpr auto CRASH_LOG_REBUILT_IMAGE_SPAN = 0x400000u;
/* snapshot: the stack copy of a suspended-and-resumed thread (hang detector, watchdog), read instead of the live
   stack; nullptr for the calling thread's own stack. */
static void log_stack_thread(FILE *out, const CONTEXT *start, HANDLE thread, const StackSnapshot *snapshot)
{
    HANDLE process = GetCurrentProcess();
    CONTEXT context = *start;
    STACKFRAME64 frame;
    int depth;

    dbghelp_lock();
    dbghelp_initialize();
    memset(&frame, 0, sizeof frame);
    frame.AddrPC.Offset = CONTEXT_PC(context);
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = CONTEXT_FP(context);
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = CONTEXT_SP(context);
    frame.AddrStack.Mode = AddrModeFlat;
    t_walkSnapshot = snapshot;
    for (depth = 0; depth < 48; depth++) {
        char name[MAX_PATH + 320];
        IMAGEHLP_LINE64 line;
        DWORD lineDisplacement = 0;
        if (!StackWalk64(CRASH_MACHINE_TYPE, process, thread, &frame, &context,
                         snapshot != nullptr ? read_walk_memory : nullptr, SymFunctionTableAccess64,
                         SymGetModuleBase64, nullptr) || frame.AddrPC.Offset == 0) {
            break;
        }
        line.SizeOfStruct = sizeof line;
        code_address_name(name, sizeof name, process, frame.AddrPC.Offset, true);
        fprintf(out, "  %2d  %08llX  %s", depth, frame.AddrPC.Offset, name);
        if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line)) {
            fprintf(out, "  (%s:%lu)", line.FileName, line.LineNumber);
        }
        fprintf(out, "\n");
    }
    t_walkSnapshot = nullptr;
    dbghelp_unlock();
}

constexpr auto SYMBOL_NAME_CAPACITY = MAX_PATH + 320;

/* The name of address into out. useDbghelp: the caller holds the dbghelp lock and has called SymInitialize;
   false names through thandor.sym / module+offset only. */
static const char *symbol_name(char *out, size_t capacity, const void *address, bool useDbghelp)
{
    code_address_name(out, capacity, GetCurrentProcess(), (DWORD64)reinterpret_cast<uintptr_t>(address),
                      useDbghelp);
    if (strcmp(out, "?") == 0) {
        snprintf(out, capacity, "%p", address);
    }
    return out;
}

/* The buffer is per thread: the caller reads it after the lock is released (valid until the thread's next
   call). */
const char *Thandor_SymbolName(const void *address)
{
    static thread_local char name[SYMBOL_NAME_CAPACITY];
    dbghelp_lock();
    dbghelp_initialize();
    symbol_name(name, sizeof name, address, true);
    dbghelp_unlock();
    return name;
}

void Thandor_LogStack(const char *reason, unsigned value)
{
    char path[MAX_PATH];
    FILE *out;
    CONTEXT context;

    executable_directory(path, sizeof path);
    strncat(path, "thandor.log", sizeof path - strlen(path) - 1);
    if ((out = fopen(path, "a")) == nullptr) {
        return;
    }
    fprintf(out, "%s 0x%08X\n", reason, value);
    memset(&context, 0, sizeof context);
    context.ContextFlags = CONTEXT_CONTROL;
    /* x64: the stack walk unwinds through the unwind tables, not a frame pointer */
    RtlCaptureContext(&context);
    log_stack_thread(out, &context, GetCurrentThread(), nullptr);
    fclose(out);
}

/* core/ptr32.h: a pointer of 2 GB or more was stored in a 32-bit field of an original layout. */
void Thandor_Ptr32Overflow(uintptr_t value)
{
    Thandor_Log("Ptr32: pointer 0x%llX does not fit a 32-bit field", (unsigned long long)value);
    Thandor_LogStack("Ptr32 overflow stack", (unsigned)value);
    ExitProcess(0xF5);
}

/* First thing in the crash filter: registers and raw stack words to crash_raw.log using only
   kernel32/user32 calls, so a corrupted CRT heap or stack cannot stop it. Symbolize the code
   addresses offline with the build's thandor.pdb or a /MAP map file. */
static void raw_crash_dump(EXCEPTION_POINTERS *info)
{
    char path[MAX_PATH];
    char line[256];
    char *slash;
    HANDLE file;
    DWORD written;
    const CONTEXT *c = info->ContextRecord;
    const DWORD *stack = static_cast<const DWORD *>(register_address(CONTEXT_SP(*c)));
    SYSTEMTIME now;
    int i;
    int n;

    if (GetModuleFileNameA(nullptr, path, MAX_PATH) == 0) {
        return;
    }
    slash = path;
    for (i = 0; path[i] != 0; i++) {
        if (path[i] == '\\') slash = path + i + 1;
    }
    /* a long game directory can leave too little room after it for the (longer than thandor.exe) name */
    if ((size_t)(path + sizeof path - slash) < sizeof "crash_raw.log") {
        return;
    }
    lstrcpyA(slash, "crash_raw.log");
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }
    GetLocalTime(&now);
    n = wsprintfA(line, "\r\n==== %04u-%02u-%02u %02u:%02u:%02u ====\r\nexception %08lX at %08lX info %08lX %08lX\r\n",
                  now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
                  info->ExceptionRecord->ExceptionCode,
                  (DWORD)reinterpret_cast<uintptr_t>(info->ExceptionRecord->ExceptionAddress),
                  (DWORD)info->ExceptionRecord->ExceptionInformation[0],
                  (DWORD)info->ExceptionRecord->ExceptionInformation[1]);
    WriteFile(file, line, n, &written, nullptr);
    /* x64: each register as high and low dword (wsprintf has no 64-bit format) */
    n = wsprintfA(line, "rip=%08lX%08lX rax=%08lX%08lX rbp=%08lX%08lX rsp=%08lX%08lX\r\n",
                  (DWORD)(c->Rip >> 32), (DWORD)c->Rip, (DWORD)(c->Rax >> 32), (DWORD)c->Rax,
                  (DWORD)(c->Rbp >> 32), (DWORD)c->Rbp, (DWORD)(c->Rsp >> 32), (DWORD)c->Rsp);
    WriteFile(file, line, n, &written, nullptr);
    for (i = 0; i < 512; i += 8) {
        if (!Thandor_IsReadable(stack + i, 32)) {
            break;
        }
        n = wsprintfA(line, "  +%03X: %08lX %08lX %08lX %08lX %08lX %08lX %08lX %08lX\r\n", i * 4,
                      stack[i], stack[i + 1], stack[i + 2], stack[i + 3], stack[i + 4], stack[i + 5],
                      stack[i + 6], stack[i + 7]);
        WriteFile(file, line, n, &written, nullptr);
    }
    CloseHandle(file);
}

/* The symbolized frames of the crashed thread (names and lines from the PDB of an MSVC build through dbghelp,
   names from thandor.sym in a GCC build, module+offset otherwise). */
static void walk_crash_stack(FILE *out, HANDLE process, HANDLE thread, CONTEXT *context)
{
    STACKFRAME64 frame;
    int depth;
    memset(&frame, 0, sizeof frame);
    frame.AddrPC.Offset = CONTEXT_PC(*context);
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = CONTEXT_FP(*context);
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = CONTEXT_SP(*context);
    frame.AddrStack.Mode = AddrModeFlat;
    for (depth = 0; depth < 64; depth++) {
        char name[MAX_PATH + 320];
        IMAGEHLP_LINE64 line;
        DWORD lineDisplacement = 0;
        if (!StackWalk64(CRASH_MACHINE_TYPE, process, thread, &frame, context, nullptr,
                         SymFunctionTableAccess64, SymGetModuleBase64, nullptr) || frame.AddrPC.Offset == 0) {
            break;
        }
        line.SizeOfStruct = sizeof line;
        code_address_name(name, sizeof name, process, frame.AddrPC.Offset, true);
        fprintf(out, "%2d  %08llX  %s", depth, frame.AddrPC.Offset, name);
        if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line)) {
            fprintf(out, "  (%s:%lu)", line.FileName, line.LineNumber);
        }
        fprintf(out, "\n");
    }
}

static LONG WINAPI crash_filter(EXCEPTION_POINTERS *info)
{
    char path[MAX_PATH];
    FILE *out;
    raw_crash_dump(info);
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    CONTEXT context = *info->ContextRecord;

    executable_directory(path, sizeof path);
    strncat(path, "crash.log", sizeof path - strlen(path) - 1);
    if ((out = fopen(path, "a")) == nullptr) {
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
        fprintf(out, "%s address 0x%08llX\n",
                info->ExceptionRecord->ExceptionInformation[0] == 8 ? "execute" :
                info->ExceptionRecord->ExceptionInformation[0] == 1 ? "write" : "read",
                (unsigned long long)info->ExceptionRecord->ExceptionInformation[1]);
    }
    fprintf(out, "rax=%016llX rbx=%016llX rcx=%016llX rdx=%016llX\nrsi=%016llX rdi=%016llX rbp=%016llX rsp=%016llX\n",
            context.Rax, context.Rbx, context.Rcx, context.Rdx, context.Rsi, context.Rdi, context.Rbp,
            context.Rsp);
    fprintf(out, "module base 0x%p\n\n", static_cast<void *>(GetModuleHandleA(nullptr)));
    fflush(out);
    /* Raw stack qwords first (the same 0x180 bytes as the earlier 96 dwords): the stack walk below can fault
       on a corrupted stack. */
    {
        const uint64_t *stack = static_cast<const uint64_t *>(register_address(CONTEXT_SP(context)));
        int i;
        fprintf(out, "stack:");
        for (i = 0; i < 48 && Thandor_IsReadable(stack + i, 8); i++) {
            fprintf(out, "%s%016llX", (i % 4) ? " " : "\n  ", (unsigned long long)stack[i]);
        }
        fprintf(out, "\n\n");
        fflush(out);
    }

    /* dbghelp only with its lock: not when this thread faulted while holding it, and not after
       DBGHELP_CRASH_WAIT_MS while another thread holds it (then names from thandor.sym / module+offset only and
       no stack walk). */
    const bool useDbghelp = dbghelp_try_lock(DBGHELP_CRASH_WAIT_MS);
    if (useDbghelp) {
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(process, nullptr, TRUE);
    }
    /* Code addresses among the raw stack qwords (return addresses of the frames; x64 pushes 8-byte return
       addresses, so the earlier 4-byte scan also matched halves of other values). */
    {
        const uint64_t *stack = static_cast<const uint64_t *>(register_address(CONTEXT_SP(context)));
        /* on the stack, not Thandor_SymbolName's thread_local buffer: a first thread_local access can allocate
           (emutls), and the heap may be what is corrupted */
        char name[SYMBOL_NAME_CAPACITY];
        int i;
        for (i = 0; i < 48 && Thandor_IsReadable(stack + i, 8); i++) {
            if (stack[i] >= REBUILT_IMAGE_CODE_START && stack[i] < REBUILT_IMAGE_BASE + CRASH_LOG_REBUILT_IMAGE_SPAN) {
                fprintf(out, "  [rsp+%03X] %s\n", i * 8,
                        symbol_name(name, sizeof name, register_address(stack[i]), useDbghelp));
            }
        }
        fprintf(out, "\n");
        fflush(out);
    }
    if (useDbghelp) {
#ifdef _MSC_VER
        __try {
            walk_crash_stack(out, process, thread, &context);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            fprintf(out, "(stack walk faulted)\n");
        }
#else
        /* GCC has no __try: StackWalk64 reads the stack through ReadProcessMemory, which reports a bad address
           instead of faulting. */
        walk_crash_stack(out, process, thread, &context);
#endif
        dbghelp_unlock();
    } else {
        fprintf(out, "(stack walk skipped: dbghelp in use)\n");
    }
    fclose(out);
    return EXCEPTION_CONTINUE_SEARCH;
}

static HANDLE g_watchedThread;

/* Stack reserve of the hang detector and the dev watchdog (instead of 0 = the executable's 32 MiB reserve, which
   only the pathing recursion on the main thread needs). Their 64 KiB StackSnapshot is static, not on the stack; what
   remains is a MAX_PATH path, the symbol name buffers (under 1 KiB) and dbghelp (SymInitialize, StackWalk64,
   SymFromAddr, SymGetLineFromAddr64), which is built for the 1 MiB default reserve of MSVC executables. */
constexpr SIZE_T DIAGNOSTIC_THREAD_STACK_RESERVE_BYTES = 0x100000;

#ifdef THANDOR_DEV_TOOLS
/* Diagnostics (developer tools): with OPEN_THANDOR_WATCHDOG=<seconds>, log the main thread's stack at that
   interval. */
static DWORD WINAPI watchdog_thread(void *parameter)
{
    DWORD interval = static_cast<DWORD>(reinterpret_cast<uintptr_t>(parameter));
    static StackSnapshot snapshot; /* this thread's own: filled only while the main thread is suspended */
    for (;;) {
        char path[MAX_PATH];
        FILE *out;
        Sleep(interval * 1000);
        executable_directory(path, sizeof path);
        strncat(path, "thandor.log", sizeof path - strlen(path) - 1);
        if ((out = fopen(path, "a")) == nullptr) {
            continue;
        }
        if (capture_thread(g_watchedThread, &snapshot)) {
            fprintf(out, "watchdog: main thread at %s\n",
                    Thandor_SymbolName(register_address(CONTEXT_PC(snapshot.context))));
            log_stack_thread(out, &snapshot.context, g_watchedThread, &snapshot);
        }
        fclose(out);
    }
}

static void start_watchdog()
{
    char seconds[16];
    if (GetEnvironmentVariableA("OPEN_THANDOR_WATCHDOG", seconds, sizeof seconds) != 0 && atoi(seconds) > 0) {
        /* the thread parameter carries the period in seconds as its value */
        CreateThread(nullptr, DIAGNOSTIC_THREAD_STACK_RESERVE_BYTES, watchdog_thread,
                     reinterpret_cast<void *>(static_cast<uintptr_t>(atoi(seconds))), STACK_SIZE_PARAM_IS_A_RESERVATION,
                     nullptr);
    }
}
#else
#define start_watchdog() ((void)0)
#endif

volatile long g_ThandorFrameHeartbeat;

/* Once frames are being presented, a stall of HANG_SECONDS logs three stack samples of the main
   thread (one second apart) to hang.log, so an endless loop shows which code it spins in. */
constexpr auto HANG_SECONDS = 4;

static DWORD WINAPI hang_detector_thread(void *parameter)
{
    long last = 0;
    int stalled = 0;
    int reported = 0;
    static StackSnapshot snapshot; /* this thread's own: filled only while the main thread is suspended */
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
            executable_directory(path, sizeof path);
            strncat(path, "hang.log", sizeof path - strlen(path) - 1);
            if ((out = fopen(path, "a")) == nullptr) {
                continue;
            }
            if (reported == 0) {
                SYSTEMTIME time;
                GetLocalTime(&time);
                fprintf(out, "\n==== %04u-%02u-%02u %02u:%02u:%02u no frame for %d s ====\n", time.wYear,
                        time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, stalled);
            }
            if (capture_thread(g_watchedThread, &snapshot)) {
                fprintf(out, "sample %d: main thread at %s\n", reported + 1,
                        Thandor_SymbolName(register_address(CONTEXT_PC(snapshot.context))));
                log_stack_thread(out, &snapshot.context, g_watchedThread, &snapshot);
            }
            fclose(out);
            reported++;
        }
    }
}

void Thandor_InstallCrashHandler()
{
    symbol_table_load();
    SetUnhandledExceptionFilter(crash_filter);
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_watchedThread, 0, FALSE,
                    DUPLICATE_SAME_ACCESS);
    CreateThread(nullptr, DIAGNOSTIC_THREAD_STACK_RESERVE_BYTES, hang_detector_thread, nullptr,
                 STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr);
    start_watchdog();
}

int Thandor_IsReadable(const void *address, unsigned size)
{
    MEMORY_BASIC_INFORMATION mbi;
    const char *p = static_cast<const char *>(address);
    const char *end = p + size;
    while (p < end) {
        if (VirtualQuery(p, &mbi, sizeof mbi) == 0 || mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) {
            return 0;
        }
        p = static_cast<const char *>(mbi.BaseAddress) + mbi.RegionSize;
    }
    return 1;
}

unsigned Thandor_TickCount()
{
    return GetTickCount();
}

void Thandor_SleepMs(unsigned milliseconds)
{
    Sleep(milliseconds);
}

int Thandor_DirectoryExistsW(const unsigned short *path)
{
    DWORD attributes = GetFileAttributesW(reinterpret_cast<const wchar_t *>(path)); /* UTF-16; wchar_t is 16 bits on Windows */
    return (attributes != INVALID_FILE_ATTRIBUTES) && ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

void Thandor_GetExecutablePathA(char *out, unsigned capacity)
{
    DWORD length = GetModuleFileNameA(nullptr, out, capacity);
    if (length == 0 || length >= capacity) {
        out[0] = 0;
    }
}
