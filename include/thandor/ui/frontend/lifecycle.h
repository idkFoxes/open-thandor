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
#include <thandor/core/memory/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

bool Frontend_Init(RomRecordId initialRomRecordId,uint32_t *outError);

void FrontendMenu_BindSharedResources(FrontendRootResourceSlots *frontendUiState);

void FrontendRuntime_ShutdownAndReleaseResources();

extern uint32_t g_FrontendRuntimeFlags;
extern GraphicsTextureSet *g_FrontendCentralTextureSet;
extern GraphicsPaletteAsset *g_FrontendCentralPaletteAsset;
extern GraphicsTextureSourceAsset *g_FrontendMenuTextureSource;

extern RuntimeSpinLockValue g_FrontendStateTickSpinLock;

extern SoundVoiceSet *g_FrontendMusicVoiceSet;
extern uint16_t g_FrontendMusic00SamPathUtf16[18];

void FrontendMusic_StartMenuMusic();

void FrontendVersionLabel_Draw();

#endif /* THANDOR_UI_FRONTEND_LIFECYCLE_H */
