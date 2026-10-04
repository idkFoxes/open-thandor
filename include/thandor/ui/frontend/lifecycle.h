/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/lifecycle.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_LIFECYCLE_H
#define THANDOR_UI_FRONTEND_LIFECYCLE_H

#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/lifecycle. */

/* Functions are grouped by semantic ownership. */

Bool8 Frontend_Init(RomRecordId initialRomRecordId,uint32_t *outError);

void FrontendMenu_BindSharedResources(FrontendRootResourceSlots *frontendUiState);

void FrontendRuntime_ShutdownAndReleaseResources();

extern uint32_t g_FrontendRuntimeFlags;
extern uintptr_t g_FrontendCentralTextureSet;
extern uintptr_t g_FrontendCentralPaletteAsset;
extern GraphicsTextureSourceAsset *g_FrontendMenuTextureSource;

extern uint32_t g_FrontendStateTickSpinLock;

extern DirectSoundVoiceSet *g_FrontendMusicVoiceSet;
extern uint16_t g_FrontendMusic00SamPathUtf16[18];

void FrontendMusic_StartMenuMusic();

#endif /* THANDOR_UI_FRONTEND_LIFECYCLE_H */
