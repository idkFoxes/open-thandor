/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/backend/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_BACKEND_TYPES_H
#define THANDOR_AUDIO_BACKEND_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

/* Types (split out by tools/dev/split_types.py). */

/* Voice-set creation: 0 on success (*outVoiceSet written), else an error code (*outVoiceSet untouched). */
using SoundCreateSampleVoiceSetProc = uint32_t (SoundSampleAsset * sampleAsset, SoundVoiceSet * * outVoiceSet);
/* Play a voice of the set: true when it plays; the voice (NULL on failure and from the silent backend) goes to
   *outVoice unless outVoice is NULL. */
using SoundPlayVoiceProc = Bool8 (uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, SoundVoiceSet * voiceSet, SoundVoice * * outVoice);
using SoundReleaseSampleVoiceSetProc = void (SoundVoiceSet * voiceSet);
using SoundStopVoiceProc = void (SoundVoice * voice);
using SoundIsVoiceFinishedProc = Bool8 (SoundVoice * voice);
using SoundStopAllVoicesProc = void ();
using SoundSetVoiceGainsProc = void (uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, SoundVoice * voice);

#endif /* THANDOR_AUDIO_BACKEND_TYPES_H */
