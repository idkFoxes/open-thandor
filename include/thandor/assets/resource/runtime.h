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
/* Functions are grouped by semantic ownership. */

bool InGameSaveGame_CreatePackage(void *packagePath,EngineFileHandle *outHandle);

bool Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode);

void Resource_Release(void *resourceBuffer);

ResourceRegistrationImagePair InGameSaveGame_PrepareRegistrationRecords (ResourceRegistrationRuntimeImageSavedView *runtimeImage);

ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareFactionImage(void);

ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareEffectSlots(void);

ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareShotSlots(void);

void InGameSaveGame_StoreCameraAsPlayerStart(ResourceRegistrationRuntimeImage *runtimeImage);

#endif /* THANDOR_ASSETS_RESOURCE_RUNTIME_H */
