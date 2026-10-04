/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/resource/savegame_prepare.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_RESOURCE_SAVEGAME_PREPARE_H
#define THANDOR_ASSETS_RESOURCE_SAVEGAME_PREPARE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/resource/savegame_prepare. */

/* Functions are grouped by semantic ownership. */

Bool8 InGameSaveGame_CreatePackage(void *packagePath,EngineFileHandle *outHandle);

ResourceRegistrationImagePair InGameSaveGame_PrepareRegistrationRecords (ResourceRegistrationRuntimeImageSavedView *runtimeImage);

ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareFactionImage(void);

ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareEffectSlots(void);

ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareShotSlots(void);

void InGameSaveGame_StoreCameraAsPlayerStart(ResourceRegistrationRuntimeImage *runtimeImage);

extern uint8_t *g_EffectRuntimeRebaseBaseMinusOne;
extern uint8_t *g_RuntimeObjectRebaseBaseMinusOne;

#endif /* THANDOR_ASSETS_RESOURCE_SAVEGAME_PREPARE_H */
