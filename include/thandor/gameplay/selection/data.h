/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/data.h
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_DATA_H
#define THANDOR_GAMEPLAY_SELECTION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsTextureSourceLoadPackageAssetProc *g_GraphicsTextureSourceLoadPackageAsset;

extern uint32_t g_UiCommandModeGColorVariantLimit; /* uint32_t ARGB mask applied to terrain vertex diffuse colours (0x00FFFFFF raw, other value in masked command mode); its alpha byte also switches overlay/projection paths */

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerRuntimeBlockPointers[256]; /* indexed by player runtime id (0..254, FrontendTransfer_FindLowestFreePlayerRuntimeId), 0x400 bytes in the original */

extern GraphicsTextureSourceAsset *g_SelectionPanelTextureSource;

extern GraphicsTextureSourceAsset *g_InfoPanelTextureSource;

extern void *g_SelectionPanelData;

extern void *g_InfoPanelData;

extern UiPackedTextStyle g_SelectionPanelNumberTextStyle; /* UiPackedTextStyle 0x01000000 (font 1, palette 0, left aligned) used to measure and draw the numbers in the selection panel (gameplay/selection/runtime.c) */

extern ModelProjectedBoundsPixels g_ModelProjectedBoundsPixels;

extern SelectionInfoEntitySlots *g_SelectionInfoEntitySlots;

extern uint16_t u_gfx_panel_select_gfx_0052ce18[21];

extern uint16_t u_gfx_panel_info_gfx_0052ce42[19];

extern uint16_t u_gfx_panel_select_dat_0052ce68[21];

extern uint16_t u_gfx_panel_info_dat_0052ce92[19];

extern uint16_t g_SelectionPanelNumberScratchUtf16[16];

extern GraphicsTextureSourceBlitProc *g_SelectionPanelBlitOpaque;

extern GraphicsTextureSourceTiledBlitProc *g_SelectionPanelBlitClipped;

extern EffectRuntimeSlot *g_InGameOwnedEntityTransientEffectMarkers[32];

extern uint32_t g_InGameOwnedEntityTransientEffectMarkerCount;

extern EffectRuntimeSlot *g_InGameCommandTargetTransientEffectMarkers[128];

extern uint32_t g_InGameCommandTargetTransientEffectMarkerCount;

extern int32_t g_InGamePendingPlacementArmyAsset;

extern GameEntityRuntime *g_InGamePlacementPreviewArmyRuntime;

extern GameEntityRuntime *g_InGameCommandPreviewArmyRuntime;

extern uint32_t g_InGameCommandPreviewArmyAssetId;

extern code *g_InGamePointerModeHandlers[8];

#endif
