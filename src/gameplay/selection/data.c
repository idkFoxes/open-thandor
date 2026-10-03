/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/gameplay/selection/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 004A8F3C g_GraphicsTextureSourceLoadPackageAsset */
__declspec(align(4)) GraphicsTextureSourceLoadPackageAssetProc *g_GraphicsTextureSourceLoadPackageAsset = (void *)GraphicsTextureSource_LoadPackageAsset;

/* 00503A8C g_UiCommandModeGColorVariantLimit: uint32_t ARGB mask applied to terrain vertex diffuse colours (0x00FFFFFF raw, other value in masked command mode); its alpha byte also switches overlay/projection paths */
__declspec(align(4)) uint32_t g_UiCommandModeGColorVariantLimit = 16777215;

/* 00514960 g_SelectionPlayerRuntimeBlockPointers: indexed by player runtime id (0..254) */
__declspec(align(16)) SelectionPlayerRuntimeBlock *g_SelectionPlayerRuntimeBlockPointers[256] = {0};

/* 0052CDF0 g_SelectionPanelTextureSource */
__declspec(align(16)) GraphicsTextureSourceAsset *g_SelectionPanelTextureSource = 0;

/* 0052CDF4 g_InfoPanelTextureSource */
__declspec(align(4)) GraphicsTextureSourceAsset *g_InfoPanelTextureSource = 0;

/* 0052CDF8 g_SelectionPanelData */
__declspec(align(8)) void *g_SelectionPanelData = 0;

/* 0052CDFC g_InfoPanelData */
__declspec(align(4)) void *g_InfoPanelData = 0;

/* 0052CE00 g_SelectionPanelNumberTextStyle: UiPackedTextStyle 0x01000000 (font 1, palette 0, left aligned) used to measure and draw the numbers in the selection panel (gameplay/selection/runtime.c) */
__declspec(align(16)) UiPackedTextStyle g_SelectionPanelNumberTextStyle = 16777216;

/* 0052CE04 g_ModelProjectedBoundsPixels */
__declspec(align(4)) ModelProjectedBoundsPixels g_ModelProjectedBoundsPixels = {0};

/* 0052CE14 g_SelectionInfoEntitySlots */
__declspec(align(4)) SelectionInfoEntitySlots *g_SelectionInfoEntitySlots = 0;

/* 0052CE18 u_gfx_panel_select_gfx_0052ce18 */
__declspec(align(8)) uint16_t u_gfx_panel_select_gfx_0052ce18[21] = L"gfx\\panel\\select.gfx";

/* 0052CE42 u_gfx_panel_info_gfx_0052ce42 */
__declspec(align(4)) uint16_t u_gfx_panel_info_gfx_0052ce42[19] = L"gfx\\panel\\info.gfx";

/* 0052CE68 u_gfx_panel_select_dat_0052ce68 */
__declspec(align(8)) uint16_t u_gfx_panel_select_dat_0052ce68[21] = L"gfx\\panel\\select.dat";

/* 0052CE92 u_gfx_panel_info_dat_0052ce92 */
__declspec(align(4)) uint16_t u_gfx_panel_info_dat_0052ce92[19] = L"gfx\\panel\\info.dat";

/* 0052CEB8 g_SelectionPanelNumberScratchUtf16 */
__declspec(align(8)) uint16_t g_SelectionPanelNumberScratchUtf16[16] = {0};

/* 0052CED8 g_SelectionPanelBlitOpaque */
__declspec(align(8)) GraphicsTextureSourceBlitProc *g_SelectionPanelBlitOpaque = 0;

/* 0052CEDC g_SelectionPanelBlitClipped */
__declspec(align(4)) GraphicsTextureSourceTiledBlitProc *g_SelectionPanelBlitClipped = 0;

/* 00562E48 g_InGameOwnedEntityTransientEffectMarkers */
__declspec(align(8)) EffectRuntimeSlot *g_InGameOwnedEntityTransientEffectMarkers[32] = {0};

/* 00562EC8 g_InGameOwnedEntityTransientEffectMarkerCount */
__declspec(align(8)) uint32_t g_InGameOwnedEntityTransientEffectMarkerCount = 0;

/* 00562ECC g_InGameCommandTargetTransientEffectMarkers */
__declspec(align(4)) EffectRuntimeSlot *g_InGameCommandTargetTransientEffectMarkers[128] = {0};

/* 005630CC g_InGameCommandTargetTransientEffectMarkerCount */
__declspec(align(4)) uint32_t g_InGameCommandTargetTransientEffectMarkerCount = 0;

/* 00563710 g_InGamePendingPlacementArmyAsset */
__declspec(align(16)) int32_t g_InGamePendingPlacementArmyAsset = 0;

/* 00563714 g_InGamePlacementPreviewArmyRuntime */
__declspec(align(4)) GameEntityRuntime *g_InGamePlacementPreviewArmyRuntime = 0;

/* 00563724 g_InGameCommandPreviewArmyRuntime */
__declspec(align(4)) GameEntityRuntime *g_InGameCommandPreviewArmyRuntime = 0;

/* 00563740 g_InGameCommandPreviewArmyAssetId */
__declspec(align(16)) uint32_t g_InGameCommandPreviewArmyAssetId = 0;

/* 00563748 g_InGamePointerModeHandlers */
__declspec(align(8)) code *g_InGamePointerModeHandlers[8] = {
    /* 0 */ 0,
    /* 1 */ (void *)InGameSelection_SetAircraftPadTargetLane1,
    /* 2 */ (void *)InGameSelection_SetAircraftPadTargetLane2,
    /* 3 */ (void *)SelectionMarkerCoordinates_ApplyType3,
    /* 4 */ (void *)SelectionMarkerCoordinates_ApplyType4,
    /* 5 */ (void *)SelectionMarkerCoordinates_ApplyType5,
    /* 6 */ (void *)SelectionMarkerCoordinates_ApplyType6,
    /* 7 */ (void *)SelectionMarkerCoordinates_ApplyType7};
