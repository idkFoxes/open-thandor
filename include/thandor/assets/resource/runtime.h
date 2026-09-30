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
bool InGameSaveGame_CreatePackage(void *packagePath,EngineFileHandle *outHandle);

/* 0x0040F000 */
bool Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode);

/* 0x0040F1D0 */
void Resource_Release(void *resourceBuffer);

/* 0x0050E890 */
ResourceRegistrationImagePair InGameSaveGame_PrepareRegistrationRecords (ResourceRegistrationRuntimeImageSavedView *runtimeImage);

/* 0x00513020 */
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareFactionImage(void);

/* 0x0051E2B0 */
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareEffectSlots(void);

/* 0x0052B6D0 */
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareShotSlots(void);

/* 0x00532B00 */
void InGameSaveGame_StoreCameraAsPlayerStart(ResourceRegistrationRuntimeImage *runtimeImage);

#endif /* THANDOR_ASSETS_RESOURCE_RUNTIME_H */
