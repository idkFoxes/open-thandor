#ifndef THANDOR_ASSETS_RESOURCE_RUNTIME_H
#define THANDOR_ASSETS_RESOURCE_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/resource/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0040E2E0 */
dword ResourceRegistration_OpenSourceCf(void *packagePath);

/* 0x0040F000 */
undefined8 __fastcall Resource_Load(undefined4 param_1,undefined4 param_2,word *path);

/* 0x0040F1D0 */
undefined4 Resource_Release(void *allocation);

/* 0x0050E890 */
ResourceRegistrationImagePair ResourceRegistration_SelectDomainPair (ResourceRegistrationRuntimeImageSerializedScalarViewDC *runtimeImage);

/* 0x00513020 */
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain0Pair(void);

/* 0x0051E2B0 */
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain1Pair(void);

/* 0x0052B6D0 */
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain2Pair(void);

/* 0x00532B00 */
void ResourceRegistration_ResolveRuntimeRecord(ResourceRegistrationRuntimeImage *runtimeImage);

#endif /* THANDOR_ASSETS_RESOURCE_RUNTIME_H */
