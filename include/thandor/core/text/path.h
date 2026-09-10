#ifndef THANDOR_CORE_TEXT_PATH_H
#define THANDOR_CORE_TEXT_PATH_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/text/path. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040F240 */
dword WidePath_GetExtensionCode(word *path);

/* 0x0040F2B0 */
undefined4 WidePath_SetExtensionCode(PackedFileExtensionCode32 extensionCode,word *path);

/* 0x0040F320 */
undefined8 WidePath_SplitParentAndLeaf(word *leafOut,word *parentOut,word *path);

/* 0x0040F3C0 */
undefined4 WidePath_CombineDirectoryAndLeaf(word *destination,word *leaf,word *directory);

/* 0x00531170 */
undefined8 WidePath_ParseTrailingNumberBeforeExtensionRegs(word *path);

#endif /* THANDOR_CORE_TEXT_PATH_H */
