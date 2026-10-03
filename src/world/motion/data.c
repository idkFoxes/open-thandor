/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/motion/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/motion/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(4)) Q12 g_WorldMotionPositionMagnitudeInputScaleQ12 = 16 /* 0.003906 */;

__declspec(align(4)) uint32_t k_CameraScreenDeltaDistanceScaleQ16 = 32;

__declspec(align(4)) Q12 g_WorldMotionDistanceInputScaleQ12 = 64 /* 0.015625 */;

__declspec(align(4)) AngleTurn32 g_WorldMotionHeadingInputScale = 16;

__declspec(align(4)) AngleTurn32 g_WorldMotionPitchInputScale = 16;

__declspec(align(4)) AngleTurn32 g_WorldMotionAlternateMinimumPitchAngle = 0xFFFFC400;

__declspec(align(4)) AngleTurn32 g_WorldMotionAlternateMaximumPitchAngle = 0xFFFFF600;

__declspec(align(4)) UQ12 g_WorldMotionAlternateMinimumDistanceQ12 = 32768;

__declspec(align(4)) UQ12 g_WorldMotionAlternateMaximumDistanceQ12 = 131072;
