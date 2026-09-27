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
/* Engine path buffers hold at most 256 UTF-16 code units (0x200 bytes) including the terminator, e.g.
   g_FileSystemCombinedPathScratchUtf16 and g_ExecutableDirectoryUtf16. */
#define WIDE_PATH_MAX_CODE_UNITS 0x100

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040F240 */
uint32_t WidePath_GetExtensionCode(uint16_t *path);

/* 0x0040F2B0 */
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,uint16_t *path);

/* 0x0040F320 */
bool __thandor_cf_preserve_eax_ecx_edx
WidePath_SplitParentAndLeaf(uint16_t *leafOut,uint16_t *parentOut,uint16_t *path);

/* 0x0040F3C0 */
void __thandor_void_preserve_eax_ecx_edx
WidePath_CombineDirectoryAndLeaf(uint16_t *destination,uint16_t *leaf,uint16_t *directory);

/* 0x00531170 */
uint32_t __thandor_preserve_eax_edx WidePath_ParseTrailingNumberBeforeExtensionRegs(uint16_t *path);

#endif /* THANDOR_CORE_TEXT_PATH_H */
