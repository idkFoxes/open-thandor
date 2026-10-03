/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/motion/data.h
 */

#ifndef THANDOR_WORLD_MOTION_DATA_H
#define THANDOR_WORLD_MOTION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern Q12 g_WorldMotionPositionMagnitudeInputScaleQ12; /* 0050BAE2 g_WorldMotionPositionMagnitudeInputScaleQ12 */

extern uint32_t k_CameraScreenDeltaDistanceScaleQ16; /* 0050BAE6 k_CameraScreenDeltaDistanceScaleQ16 */

extern Q12 g_WorldMotionDistanceInputScaleQ12; /* 0050BAEA g_WorldMotionDistanceInputScaleQ12 */

extern AngleTurn32 g_WorldMotionHeadingInputScale; /* 0050BAEE g_WorldMotionHeadingInputScale */

extern AngleTurn32 g_WorldMotionPitchInputScale; /* 0050BAF2 g_WorldMotionPitchInputScale */

extern AngleTurn32 g_WorldMotionAlternateMinimumPitchAngle; /* 0050BAFA g_WorldMotionAlternateMinimumPitchAngle */

extern AngleTurn32 g_WorldMotionAlternateMaximumPitchAngle; /* 0050BAFE g_WorldMotionAlternateMaximumPitchAngle */

extern UQ12 g_WorldMotionAlternateMinimumDistanceQ12; /* 0050BB02 g_WorldMotionAlternateMinimumDistanceQ12 */

extern UQ12 g_WorldMotionAlternateMaximumDistanceQ12; /* 0050BB06 g_WorldMotionAlternateMaximumDistanceQ12 */

#endif
