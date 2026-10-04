/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/spatial/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_SPATIAL_TYPES_H
#define THANDOR_AUDIO_SPATIAL_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */

/* Types (split out by tools/dev/split_types.py). */

typedef struct SpatialSoundSlot SpatialSoundSlot, *PSpatialSoundSlot;
typedef struct SoundVoiceSet SoundVoiceSet;
typedef struct SoundVoice SoundVoice;

using SpatialSoundGainQ15 = uint32_t;

using SpatialSoundMaximumDistanceQ12 = uint32_t;

using AudioMixerGainQ15 = uint32_t;

/* Runtime only (native pointers): the sound code reads voiceSet through SoundVoiceSet ** slot pointers.
   The desired gains are named after the channel they are played on; the right gain comes first in memory
   (the original's slot order, where its names were the other way round). */
struct SpatialSoundSlot {
    struct SoundVoiceSet *voiceSet; 
    struct SoundVoice *activeVoice; 
    SpatialSoundGainQ15 desiredRightGainQ15; 
    SpatialSoundGainQ15 desiredLeftGainQ15; 
};

#endif /* THANDOR_AUDIO_SPATIAL_TYPES_H */
