/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/layout.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_LAYOUT_H
#define THANDOR_UI_INGAME_LAYOUT_H

#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/layout. */

/* Index of the display-size digit in "gfx\panel\panel0.gfx" and "gfx\panel\diagram0.gfx" */
#define INGAME_PANEL_GFX_PATH_VARIANT_DIGIT 15
#define INGAME_DIAGRAM_GFX_PATH_VARIANT_DIGIT 17

/* Functions are grouped by semantic ownership. */

Bool8 InGameUiRuntime_InitializeControlTreeResources(UiRootNode *inGameRoot,uint32_t *outError);

extern int32_t g_InGamePanelTextureSubresource02Width;
extern int32_t g_InGamePanelTextureSubresource27Width;
extern int32_t g_InGamePanelTextureSubresource28Width;
extern int32_t g_InGamePanelTextureSubresource34Width;
extern int32_t g_InGamePanelTextureSubresource26Height;
extern int32_t g_InGamePanelTextureSubresource31Height;
extern int32_t g_InGamePanelTextureSubresource34Height;

extern DirectSoundVoiceSet *g_UiButtonSoundVoiceSets7[7];

extern GraphicsTextureSourceAsset *g_InGamePanelTextureSource;

extern GraphicsTextureSourceAsset *g_InGameDiagramTextureSource;
extern GraphicsTextureSourceAsset *g_InGameTechnologyTextureSource;
extern GraphicsTextureSourceAsset *g_InGameWindowTextureSource;
extern uint16_t g_GfxPanelPanel0GfxPathUtf16[21];
extern int32_t g_InGamePanelTextureSubresource19Width;
extern int32_t g_InGamePanelTextureSubresource20Width;
extern int32_t g_InGamePanelTextureSubresource32Width;
extern int32_t g_InGamePanelTextureSubresource18Height;
extern int32_t g_InGamePanelTextureSubresource23Height;
extern int32_t g_InGamePanelTextureSubresource32Height;

#endif /* THANDOR_UI_INGAME_LAYOUT_H */
