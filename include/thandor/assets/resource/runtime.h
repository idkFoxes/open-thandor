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
StatusResult ResourceRegistration_OpenSource(void *packagePath);

/* 0x0040F000 */
ResourceLoadResult Resource_Load(uint16_t *path);

/* 0x0040F1D0 */
void Resource_Release(void *resourceBuffer);

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
