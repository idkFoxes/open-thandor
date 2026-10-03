/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/shots/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0052AF40 g_ShotTextureSet */
__declspec(align(16)) GraphicsTextureSet *g_ShotTextureSet = 0;

/* 0052AF44 g_ShotPalette */
__declspec(align(4)) GraphicsPaletteAsset *g_ShotPalette = 0;

/* 0052AF48 g_ShotRuntimeSlots */
__declspec(align(8)) ShotRuntimeSlot *g_ShotRuntimeSlots = 0;

/* 0052AF4C g_ShotRuntimeRebaseBaseMinusOne */
__declspec(align(4)) uint8_t *g_ShotRuntimeRebaseBaseMinusOne = 0;
