/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/motion/data.h
 */

#ifndef THANDOR_WORLD_MOTION_DATA_H
#define THANDOR_WORLD_MOTION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern Q12 g_WorldMotionPositionMagnitudeInputScaleQ12;

extern uint32_t k_CameraScreenDeltaDistanceScaleQ16;

extern Q12 g_WorldMotionDistanceInputScaleQ12;

extern AngleTurn32 g_WorldMotionHeadingInputScale;

extern AngleTurn32 g_WorldMotionPitchInputScale;

extern AngleTurn32 g_WorldMotionAlternateMinimumPitchAngle;

extern AngleTurn32 g_WorldMotionAlternateMaximumPitchAngle;

extern UQ12 g_WorldMotionAlternateMinimumDistanceQ12;

extern UQ12 g_WorldMotionAlternateMaximumDistanceQ12;

#endif
