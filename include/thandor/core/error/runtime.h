/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/error/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_ERROR_RUNTIME_H
#define THANDOR_CORE_ERROR_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/error/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00575890 */
void __cdecl ErrorSystem_Init(void);

/* 0x00407F50 */
bool __thandor_cf_preserve_eax_ecx_edx ErrorRuntime_CallbackAlwaysFailCf(UiRootNode *root);

/* 0x00407F60 */
int __thandor_eax_preserve_ecx_edx ErrorRuntime_CallbackReturnCode8(UiRootNode *root);

/* 0x00407F70 */
void __thandor_preserve_eax FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode);

/* 0x00407F90 */
FatalErrorEaxCf5 __thandor_eax_cf_io_preserve_ecx_edx
FatalErrorRuntime_DispatchPendingErrorCf(dword errorOrValue,bool carryIn);

/* 0x00408090 */
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void);

/* 0x0041BC50 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FatalError_CopyNarrowToUtf16Cf(TextOutputCapacityBytes capacityBytes,word *destination,byte *source);

/* 0x005758D0 */
FatalErrorEaxCf5 __thandor_eax_cf_io_preserve_ecx_edx
FatalError_Exit(dword errorOrValue,bool carryIn);

/* 0x0041BB00 */
int FatalError_CopyRichTextToNarrowCf (TextOutputCapacityBytes capacityBytes,byte *destination,word *source);

#endif /* THANDOR_CORE_ERROR_RUNTIME_H */
