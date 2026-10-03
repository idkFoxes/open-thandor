/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/motion/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/motion/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0050BAE2 g_WorldMotionPositionMagnitudeInputScaleQ12 */
__declspec(align(4)) Q12 g_WorldMotionPositionMagnitudeInputScaleQ12 = 16 /* 0.003906 */;

/* 0050BAE6 k_CameraScreenDeltaDistanceScaleQ16 */
__declspec(align(4)) uint32_t k_CameraScreenDeltaDistanceScaleQ16 = 32;

/* 0050BAEA g_WorldMotionDistanceInputScaleQ12 */
__declspec(align(4)) Q12 g_WorldMotionDistanceInputScaleQ12 = 64 /* 0.015625 */;

/* 0050BAEE g_WorldMotionHeadingInputScale */
__declspec(align(4)) AngleTurn32 g_WorldMotionHeadingInputScale = 16;

/* 0050BAF2 g_WorldMotionPitchInputScale */
__declspec(align(4)) AngleTurn32 g_WorldMotionPitchInputScale = 16;

/* 0050BAFA g_WorldMotionAlternateMinimumPitchAngle */
__declspec(align(4)) AngleTurn32 g_WorldMotionAlternateMinimumPitchAngle = 0xFFFFC400;

/* 0050BAFE g_WorldMotionAlternateMaximumPitchAngle */
__declspec(align(4)) AngleTurn32 g_WorldMotionAlternateMaximumPitchAngle = 0xFFFFF600;

/* 0050BB02 g_WorldMotionAlternateMinimumDistanceQ12 */
__declspec(align(4)) UQ12 g_WorldMotionAlternateMinimumDistanceQ12 = 32768;

/* 0050BB06 g_WorldMotionAlternateMaximumDistanceQ12 (followed by 0x90 code filler up to 0050BB10) */
__declspec(align(4)) UQ12 g_WorldMotionAlternateMaximumDistanceQ12 = 131072;
