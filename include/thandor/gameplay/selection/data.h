/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/data.h
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_DATA_H
#define THANDOR_GAMEPLAY_SELECTION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsTextureSourceLoadPackageAssetProc *g_GraphicsTextureSourceLoadPackageAsset; /* 004A8F3C g_GraphicsTextureSourceLoadPackageAsset */

extern uint32_t g_UiCommandModeGColorVariantLimit; /* 00503A8C g_UiCommandModeGColorVariantLimit: uint32_t ARGB mask applied to terrain vertex diffuse colours (0x00FFFFFF raw, other value in masked command mode); its alpha byte also switches overlay/projection paths */

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerRuntimeBlockPointers[256]; /* 00514960 g_SelectionPlayerRuntimeBlockPointers: indexed by player runtime id (0..254, FrontendTransfer_FindLowestFreePlayerRuntimeId), 0x400 bytes in the original */

extern GraphicsTextureSourceAsset *g_SelectionPanelTextureSource; /* 0052CDF0 g_SelectionPanelTextureSource */

extern GraphicsTextureSourceAsset *g_InfoPanelTextureSource; /* 0052CDF4 g_InfoPanelTextureSource */

extern void *g_SelectionPanelData; /* 0052CDF8 g_SelectionPanelData */

extern void *g_InfoPanelData; /* 0052CDFC g_InfoPanelData */

extern UiPackedTextStyle g_SelectionPanelNumberTextStyle; /* 0052CE00 g_SelectionPanelNumberTextStyle: UiPackedTextStyle 0x01000000 (font 1, palette 0, left aligned) used to measure and draw the numbers in the selection panel (gameplay/selection/runtime.c) */

extern ModelProjectedBoundsPixels g_ModelProjectedBoundsPixels; /* 0052CE04 g_ModelProjectedBoundsPixels */

extern SelectionInfoEntitySlots *g_SelectionInfoEntitySlots; /* 0052CE14 g_SelectionInfoEntitySlots */

extern uint16_t u_gfx_panel_select_gfx_0052ce18[21]; /* 0052CE18 u_gfx_panel_select_gfx_0052ce18 */

extern uint16_t u_gfx_panel_info_gfx_0052ce42[19]; /* 0052CE42 u_gfx_panel_info_gfx_0052ce42 */

extern uint16_t u_gfx_panel_select_dat_0052ce68[21]; /* 0052CE68 u_gfx_panel_select_dat_0052ce68 */

extern uint16_t u_gfx_panel_info_dat_0052ce92[19]; /* 0052CE92 u_gfx_panel_info_dat_0052ce92 */

extern uint16_t g_SelectionPanelNumberScratchUtf16[16]; /* 0052CEB8 g_SelectionPanelNumberScratchUtf16 */

extern GraphicsTextureSourceBlitProc *g_SelectionPanelBlitOpaque; /* 0052CED8 g_SelectionPanelBlitOpaque */

extern GraphicsTextureSourceTiledBlitProc *g_SelectionPanelBlitClipped; /* 0052CEDC g_SelectionPanelBlitClipped */

extern EffectRuntimeSlot *g_InGameOwnedEntityTransientEffectMarkers[32]; /* 00562E48 g_InGameOwnedEntityTransientEffectMarkers */

extern uint32_t g_InGameOwnedEntityTransientEffectMarkerCount; /* 00562EC8 g_InGameOwnedEntityTransientEffectMarkerCount */

extern EffectRuntimeSlot *g_InGameCommandTargetTransientEffectMarkers[128]; /* 00562ECC g_InGameCommandTargetTransientEffectMarkers */

extern uint32_t g_InGameCommandTargetTransientEffectMarkerCount; /* 005630CC g_InGameCommandTargetTransientEffectMarkerCount */

extern int32_t g_InGamePendingPlacementArmyAsset; /* 00563710 g_InGamePendingPlacementArmyAsset */

extern GameEntityRuntime *g_InGamePlacementPreviewArmyRuntime; /* 00563714 g_InGamePlacementPreviewArmyRuntime */

extern GameEntityRuntime *g_InGameCommandPreviewArmyRuntime; /* 00563724 g_InGameCommandPreviewArmyRuntime */

extern uint32_t g_InGameCommandPreviewArmyAssetId; /* 00563740 g_InGameCommandPreviewArmyAssetId */

extern code *g_InGamePointerModeHandlers[8]; /* 00563748 g_InGamePointerModeHandlers */

#endif
