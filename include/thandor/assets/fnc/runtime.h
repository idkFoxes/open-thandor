#ifndef THANDOR_ASSETS_FNC_RUNTIME_H
#define THANDOR_ASSETS_FNC_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/fnc/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041A610 */
dword FncModule_GetBindingModeCf(FncModuleHeader *module);

/* 0x0041A640 */
int * FncModule_LoadAndRelocateCf(FncModuleHeader *serializedModule);

/* 0x0041A710 */
undefined8 FncModule_GetExportByIndexCf(FncExportIndex exportIndex,FncModuleHeader *module);

#endif /* THANDOR_ASSETS_FNC_RUNTIME_H */
