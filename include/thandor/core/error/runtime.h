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
/* Generic failure code returned with CF set by many helpers (package mount/lookup, PCK codec, text copies,
   runtime pools); InGameRuntime_RunSessionUntilExit returns it when the UI root stack runs empty */
#define FATAL_ERROR_GENERAL_FAILURE 0x14
/* Out of memory: the arena allocation failed (Package_LoadEntry, Resource_Load and the FileSystem whole-file
   loaders then leave the requested byte count in g_FatalErrorDetail1Utf16); the package loaders also return
   it for entries whose packed size exceeds PACKAGE_SCRATCH_BUFFER_BYTES */
#define FATAL_ERROR_OUT_OF_MEMORY 0x05
/* Network socket setup/send failed (NetworkFallback_OpenAndBindUdpSocket, NetworkFallback_SendDatagram); the
   WSAGetLastError code is left in g_PackageLastErrorPath */
#define FATAL_ERROR_NETWORK_SOCKET 0x2A
/* DirectSound_CreateSampleVoiceSet: the asset is not a 'sam' of format version 0x10000 (a failing
   secondary-buffer step there and in DirectSound_CreatePcmVoiceSet returns FATAL_ERROR_DIRECTSOUND_SETUP) */
#define FATAL_ERROR_SOUND_SAMPLE_INVALID 0x4A
/* Arena heap (core/memory/allocator): no free block is large enough (ArenaHeap_Alloc,
   ArenaHeap_AllocLargestFreeBlock; the largest free payload size is left in g_PackageLastErrorPath).
   A corrupt block chain returns ARENA_HEAP_FAILURE_SENTINEL_0x13 instead. */
#define FATAL_ERROR_ARENA_EXHAUSTED 0x12
/* Win32 file layer (platform/filesystem/win32, the g_FileSystem* table); the path is left in
   g_PackageLastErrorPath. Named after the operations that return them. */
#define FATAL_ERROR_FILE_ACCESS_FAILED 0x01 /* CreateFileA, DeleteFileA, MoveFileA, CopyFileA or GetFileTime failed */
#define FATAL_ERROR_FILE_READ_FAILED 0x06 /* ReadFile transferred fewer bytes than requested */
#define FATAL_ERROR_FILE_WRITE_INCOMPLETE 0x07 /* WriteFile succeeded but wrote fewer bytes than requested */
#define FATAL_ERROR_FILE_WRITE_FAILED 0x08 /* WriteFile failed; also CreateDirectoryA */
#define FATAL_ERROR_FILE_SEEK_FAILED 0x09 /* SetFilePointer failed */
#define FATAL_ERROR_SET_DIRECTORY_FAILED 0x0A /* SetCurrentDirectoryA failed */
#define FATAL_ERROR_REMOVE_DIRECTORY_FAILED 0x0B /* RemoveDirectoryA failed */
/* Level loading (gameplay/session/level.c); the level path is left in g_PackageLastErrorPath */
#define FATAL_ERROR_LEVEL_ASSET_INVALID 0x39 /* not a 'lev' asset of converter version 0x70001 */
#define FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES 0x3A /* the EFF/SHT/MDL/ARM lists name 0x200 or more files */
#define FATAL_ERROR_TECHNOLOGY_ASSET_INVALID 0x4F /* the level's technology file is not a 'tec' asset of
                                                     converter version 0x20000 */

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
