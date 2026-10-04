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

/* Types (split from generated/types.h by tools/dev/split_types.py). */

/* Voice-set creation: 0 on success (*outVoiceSet written), else an error code (*outVoiceSet untouched). */
typedef uint32_t SoundCreateSampleVoiceSetProc(SoundSampleAsset * sampleAsset, DirectSoundVoiceSet * * outVoiceSet);
/* Play a voice of the set: true when it plays; the voice (NULL on failure and from the silent backend) goes to
   *outVoice unless outVoice is NULL. */
typedef Bool8 SoundPlayVoiceProc(uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, DirectSoundVoiceSet * voiceSet, IDirectSoundBuffer * * outVoice);
typedef void SoundReleaseSampleVoiceSetProc(DirectSoundVoiceSet * voiceSet);
typedef void SoundStopVoiceProc(IDirectSoundBuffer * voice);
typedef Bool8 SoundIsVoicePlayingProc(IDirectSoundBuffer * voice);
typedef void SoundStopAllVoicesProc();
typedef void SoundSetVoiceGainsProc(uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, IDirectSoundBuffer * voice);

#endif /* THANDOR_AUDIO_BACKEND_TYPES_H */
