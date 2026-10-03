/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/original_code.c
 * Project code (not in the original game)
 */

/* Own translation unit: uses the real Windows SDK headers, not generated/types.h. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>

#include <thandor/platform/debug/original_code.h>

#define ORIGINAL_IMAGE_NAME "thandor_original.exe"

/* Developer tools: see original_code.h. (Moved from platform/bootstrap/image.c.) */
void *Thandor_LoadOriginalCodeCopy(unsigned address, unsigned size)
{
    char path[MAX_PATH];
    char *slash;
    HANDLE file;
    DWORD fileSize;
    DWORD read;
    unsigned char *data;
    unsigned char *copy = NULL;
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS32 *nt;
    IMAGE_SECTION_HEADER *section;
    unsigned i;

    /* the directory of the executable */
    GetModuleFileNameA(NULL, path, (DWORD)sizeof path);
    slash = strrchr(path, '\\');
    if (slash != NULL) {
        slash[1] = '\0';
    }
    strcat_s(path, sizeof path, ORIGINAL_IMAGE_NAME);
    file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        return NULL;
    }
    fileSize = GetFileSize(file, NULL);
    data = (unsigned char *)HeapAlloc(GetProcessHeap(), 0, fileSize);
    if (data == NULL || !ReadFile(file, data, fileSize, &read, NULL) || read != fileSize) {
        CloseHandle(file);
        return NULL;
    }
    CloseHandle(file);
    dos = (IMAGE_DOS_HEADER *)data;
    nt = (IMAGE_NT_HEADERS32 *)(data + dos->e_lfanew);
    section = IMAGE_FIRST_SECTION(nt);
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
        unsigned rva = address - nt->OptionalHeader.ImageBase;
        if (rva >= section->VirtualAddress && rva + size <= section->VirtualAddress + section->SizeOfRawData) {
            copy = (unsigned char *)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (copy != NULL) {
                memcpy(copy, data + section->PointerToRawData + (rva - section->VirtualAddress), size);
            }
            break;
        }
    }
    HeapFree(GetProcessHeap(), 0, data);
    return copy;
}
