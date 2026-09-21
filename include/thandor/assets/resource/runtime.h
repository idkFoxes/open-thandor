/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/resource/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_RESOURCE_RUNTIME_H
#define THANDOR_ASSETS_RESOURCE_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/resource/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040E2E0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ResourceRegistration_OpenSourceCf(void *packagePath);

/* 0x0040F000 */
ResourceLoadEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx Resource_Load(word *path);

/* 0x0040F1D0 */
void __thandor_void_preserve_eax_ecx_edx Resource_Release(void *allocation);

/* 0x0050E890 */
ResourceRegistrationImagePair ResourceRegistration_SelectDomainPair (ResourceRegistrationRuntimeImageSerializedScalarViewDC *runtimeImage);

/* 0x00513020 */
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain0Pair(void);

/* 0x0051E2B0 */
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain1Pair(void);

/* 0x0052B6D0 */
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain2Pair(void);

/* 0x00532B00 */
void __thandor_void_preserve_eax_ecx_edx
ResourceRegistration_ResolveRuntimeRecord(ResourceRegistrationRuntimeImage *runtimeImage);

#endif /* THANDOR_ASSETS_RESOURCE_RUNTIME_H */
