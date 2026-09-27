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

/* The two fatal-error handlers, installed by ErrorSystem_Init. Both take a value and a failure flag:
   without the flag they return the value unchanged; with it they treat the value as an error code or
   message and handle it.
   FatalError_ExitIfFailed: always FatalError_Exit, which shows the message box, shuts down and exits.
   FatalError_ReportIfFailed: FatalError_Exit until the UI error state exists
   (ErrorRuntime_InstallUiHandlerAndAllocateState), then FatalErrorRuntime_DispatchPendingError, which
   shows the error in a modal in-game dialog and returns. */
#define FatalError_ExitIfFailed(valueOrError, failed) (g_FatalErrorExitHandler((valueOrError), (failed)))
#define FatalError_ReportIfFailed(valueOrError, failed) (g_FatalErrorReportHandler((valueOrError), (failed)))

/* Error codes handed to the fatal-error dispatcher (FatalError_ExitIfFailed); the code selects
   the message text. Named as they are found. */
#define FATAL_ERROR_CPU_WITHOUT_MMX 0x51 /* ProcessEntry: CPUID reports no MMX (see CPU_DetectFeatures) */
/* DLL binding (DynAPI_Bootstrap, DynAPI_Resolve, DynDLL_Load); the DLL/procedure name is left in
   g_PackageLastErrorPath */
#define FATAL_ERROR_LOADER_MODULE_MISSING 0x0F /* the module of LoadLibraryA itself is not mapped */
#define FATAL_ERROR_DLL_PROCEDURE_MISSING 0x10 /* GetProcAddress failed */
#define FATAL_ERROR_DLL_LOAD_FAILED 0x11 /* LoadLibraryA failed */
/* Subsystem startup (Graphics_Init, DirectSound_Init); names follow the failing step */
#define FATAL_ERROR_DIRECTDRAW_NO_ADAPTER 0x17 /* DirectDrawEnumerateA failed or listed no adapter */
#define FATAL_ERROR_DIRECTDRAW_NO_DISPLAY_MODE 0x18 /* no adapter reported a usable display mode */
#define FATAL_ERROR_DIRECTSOUND_SETUP 0x29 /* primary buffer setup failed; the stage number is left in
                                              g_PackageLastErrorPath */
#define FATAL_ERROR_DIRECTINPUT_SETUP 0x25 /* DirectInputMouse_Init: DirectInputCreateA or a mouse-device
                                              setup call failed; the stage number (0..4) is left in
                                              g_PackageLastErrorPath */

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00575890 */
void __cdecl ErrorSystem_Init(void);

/* 0x00407F50 */
bool __thandor_cf_preserve_eax_ecx_edx ErrorRuntime_CallbackAlwaysFail(UiRootNode *root);

/* 0x00407F60 */
int __thandor_eax_preserve_ecx_edx ErrorRuntime_CallbackReturnCode8(UiRootNode *root);

/* 0x00407F70 */
void __thandor_preserve_eax FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode);

/* 0x00407F90 */
FatalErrorCheckResult __thandor_eax_cf_io_preserve_ecx_edx
FatalErrorRuntime_DispatchPendingError(uint32_t errorOrValue,bool carryIn);

/* 0x00408090 */
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void);

/* 0x0041BC50 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
FatalError_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source);

/* 0x005758D0 */
FatalErrorCheckResult __thandor_eax_cf_io_preserve_ecx_edx
FatalError_Exit(uint32_t errorOrValue,bool carryIn);

/* 0x0041BB00 */
int FatalError_CopyRichTextToNarrow (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source);

#endif /* THANDOR_CORE_ERROR_RUNTIME_H */
