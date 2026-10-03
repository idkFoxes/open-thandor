/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/spatial/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/audio/spatial/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) GraphicsFixedMatrix3x4 g_SpatialSoundListenerTransform = {0};

__declspec(align(16)) GraphicsFixedMatrix3x4 g_SpatialSoundListenerRotation = {0};

__declspec(align(16)) GraphicsFixedMatrix3x4 g_SpatialSoundListenerWorldToLocal = {0};

/* sound position in the listener's frame */
__declspec(align(16)) GraphicsFixedVec3 g_SpatialSoundRelative = {0};

__declspec(align(4)) AudioMixerGainQ15 g_SoundEffectsGainQ15 = 32768;

__declspec(align(4)) SpatialSoundSlot *g_SpatialSoundSlots = 0;
