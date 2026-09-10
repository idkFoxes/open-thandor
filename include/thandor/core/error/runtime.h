#ifndef THANDOR_CORE_ERROR_RUNTIME_H
#define THANDOR_CORE_ERROR_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/error/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00575890 */
void __cdecl ErrorSystem_Init(void);

/* 0x00407F50 */
void ErrorRuntime_CallbackAlwaysFailCf(void);

/* 0x00407F60 */
undefined4 ErrorRuntime_CallbackReturnCode8(void);

/* 0x00407F70 */
undefined4 FatalErrorDialog_DismissAndPopRoot(UiRootNode *param_1);

/* 0x00407F90 */
void __cdecl FatalErrorRuntime_DispatchPendingErrorCf(void);

/* 0x00408090 */
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void);

/* 0x0041BC50 */
int FatalError_CopyNarrowToUtf16Cf(uint param_1,ushort *param_2,byte *param_3);

/* 0x005758D0 */
void __cdecl FatalError_Exit(void);

/* 0x0041BB00 */
int FatalError_CopyRichTextToNarrowCf (TextOutputCapacityBytes capacityBytes,byte *destination,word *source);

#endif /* THANDOR_CORE_ERROR_RUNTIME_H */
