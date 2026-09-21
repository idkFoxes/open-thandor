/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/text/path.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_TEXT_PATH_H
#define THANDOR_CORE_TEXT_PATH_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/text/path. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040F240 */
dword WidePath_GetExtensionCode(word *path);

/* 0x0040F2B0 */
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,word *path);

/* 0x0040F320 */
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SplitParentAndLeaf(word *leafOut,word *parentOut,word *path);

/* 0x0040F3C0 */
void __thandor_void_preserve_eax_ecx_edx
WidePath_CombineDirectoryAndLeaf(word *destination,word *leaf,word *directory);

/* 0x00531170 */
dword __thandor_preserve_eax_edx WidePath_ParseTrailingNumberBeforeExtensionRegs(word *path);

#endif /* THANDOR_CORE_TEXT_PATH_H */
