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

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct SpatialSoundSlot SpatialSoundSlot, *PSpatialSoundSlot;
typedef struct DirectSoundVoiceSet DirectSoundVoiceSet;
typedef struct IDirectSoundBuffer IDirectSoundBuffer;

using SpatialSoundGainQ15 = uint32_t;

using SpatialSoundMaximumDistanceQ12 = uint32_t;

using AudioMixerGainQ15 = uint32_t;

/* Runtime only (native pointers): the sound code reads voiceSet through DirectSoundVoiceSet ** slot pointers. */
struct SpatialSoundSlot {
    struct DirectSoundVoiceSet *voiceSet; 
    struct IDirectSoundBuffer *activeVoice; 
    SpatialSoundGainQ15 desiredLeftGainQ15; 
    SpatialSoundGainQ15 desiredRightGainQ15; 
};

#endif /* THANDOR_AUDIO_SPATIAL_TYPES_H */
