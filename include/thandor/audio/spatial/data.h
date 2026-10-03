/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/spatial/data.h
 */

#ifndef THANDOR_AUDIO_SPATIAL_DATA_H
#define THANDOR_AUDIO_SPATIAL_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsFixedMatrix3x4 g_SpatialSoundListenerTransform; /* 0050B520 g_SpatialSoundListenerTransform */

extern GraphicsFixedMatrix3x4 g_SpatialSoundListenerRotation; /* 0050B550 g_SpatialSoundListenerRotation */

extern GraphicsFixedMatrix3x4 g_SpatialSoundListenerWorldToLocal; /* 0050B580 g_SpatialSoundListenerWorldToLocal */

extern GraphicsFixedVec3 g_SpatialSoundRelative; /* 0050B5B0 g_SpatialSoundRelative: sound position in the listener's frame */

extern AudioMixerGainQ15 g_SoundEffectsGainQ15; /* 0050B5BC g_SoundEffectsGainQ15 */

extern SpatialSoundSlot *g_SpatialSoundSlots; /* 0050B5C4 g_SpatialSoundSlots */

#endif
