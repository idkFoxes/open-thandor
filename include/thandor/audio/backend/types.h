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

/* Voice-set creation from a loaded .sam of sampleByteCount bytes: 0 on success (*outVoiceSet written), else an
   error code (*outVoiceSet untouched). */
using SoundCreateSampleVoiceSetProc = uint32_t (SoundSampleAsset * sampleAsset, uint32_t sampleByteCount,
                                               SoundVoiceSet * * outVoiceSet);
/* Play a voice of the set: true when it plays; the voice (NULL on failure and from the silent backend) goes to
   *outVoice unless outVoice is NULL. */
using SoundPlayVoiceProc = bool (uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, SoundVoiceSet * voiceSet, SoundVoice * * outVoice);
using SoundReleaseSampleVoiceSetProc = void (SoundVoiceSet * voiceSet);
using SoundStopVoiceProc = void (SoundVoice * voice);
using SoundIsVoiceFinishedProc = bool (SoundVoice * voice);
using SoundStopAllVoicesProc = void ();
using SoundSetVoiceGainsProc = void (uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, SoundVoice * voice);

#endif /* THANDOR_AUDIO_BACKEND_TYPES_H */
