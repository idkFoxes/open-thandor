/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/fnc/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_FNC_RUNTIME_H
#define THANDOR_ASSETS_FNC_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/fnc/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041A610 */
dword FncModule_GetBindingModeCf(FncModuleHeader *module);

/* 0x0041A640 */
FncModuleLoadEaxCf5 FncModule_LoadAndRelocateCf(FncModuleHeader *serializedModule);

/* 0x0041A710 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FncModule_GetExportByIndexCf(FncExportIndex exportIndex,FncModuleHeader *module);

#endif /* THANDOR_ASSETS_FNC_RUNTIME_H */
