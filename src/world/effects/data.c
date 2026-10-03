/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/effects/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0051DBC0 g_EffectTextureSet */
__declspec(align(16)) GraphicsTextureSet *g_EffectTextureSet = 0;

/* 0051DBC4 g_EffectPalette */
__declspec(align(4)) GraphicsPaletteAsset *g_EffectPalette = 0;

/* 0051DBC8 g_EffectRuntimeSlots */
__declspec(align(8)) EffectRuntimeSlot *g_EffectRuntimeSlots = 0;
