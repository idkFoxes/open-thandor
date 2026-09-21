/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_RUNTIME_H
#define THANDOR_PLATFORM_BOOTSTRAP_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/bootstrap/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00585D40 */
void __cdecl ProcessEntry(void);

/* 0x00512E70 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx GameData_ResetDefaults(void);

/* 0x00512F60 */
bool __thandor_cf_preserve_eax_ecx_edx GameData_LoadExternalTables(void);

/* 0x00573BC0 */
DynApiResolveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DynAPI_Resolve(void **destination,HINSTANCE module,char *procedureName);

/* 0x00573C50 */
DynDllLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx DynDLL_Load(char *moduleName);

/* 0x00573CD0 */
dword DynDLL_Unload(char *moduleName);

/* 0x00573D40 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
BootstrapApi_ResolveBindingByDestination(void **destination);

/* 0x00573EB0 */
void __thandor_void_preserve_eax_ecx_edx DynDLL_UnloadAll(void);

/* 0x00585F50 */
LRESULT MainWindowProc(HWND hwnd,Win32WindowMessageId message,WPARAM wParam,LPARAM lParam);

/* 0x00587370 */
dword __cdecl CPU_DetectFeatures(void);

/* 0x00573070 */
void __cdecl Game_Run(void);

/* 0x0050BB10 */
void __cdecl GameRuntime_InitializeSpatialAudioAndRenderingCf(void);

/* 0x00573140 */
dword __cdecl Game_LoadCoreAssets(void);

/* 0x005739D0 */
void __thandor_void_preserve_eax_ecx_edx Game_PlayIntroMovies(void);

/* 0x00573DB0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx DynAPI_Bootstrap(void);

/* 0x00586110 */
CommandLineFindOptionEbxCf5 __thandor_ebx_cf_preserve_eax_ecx_edx
CommandLine_FindOption(CommandLineOptionLengthBytes length,char *option);

/* 0x00586170 */
void __thandor_void_preserve_eax_ecx_edx CommandLine_Parse(void);

#endif /* THANDOR_PLATFORM_BOOTSTRAP_RUNTIME_H */
