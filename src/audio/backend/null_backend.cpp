/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/backend/null_backend.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/backend/null_backend.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: audio/backend/null_backend. */

/* Silent-backend stub in slot g_SoundCreateSampleVoiceSet (until SdlAudio_Init switches the slots to the SDL3
   audio backend, and for good when there is no audio device). Always succeeds (returns 0) with the dummy voice set 0xFFFFFFFF in
   *outVoiceSet, so callers holding a sample keep a non-NULL handle even without sound.
*/
uint32_t SoundBackendDisabled_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet)

{
  *outVoiceSet = (DirectSoundVoiceSet *)(intptr_t)-1;
  return 0;
}

/* Silent-backend stub in slot g_SoundReleaseSampleVoiceSet: nothing to release.
*/
void SoundBackendDisabled_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)

{
  return;
}

/* Silent-backend stub in slot g_SoundPlayOneShot: plays nothing and reports success (returns true)
   with a NULL voice in *outVoice; the original never writes the voice, so its callers store an unrelated
   leftover value of their own as the voice (e.g. a random effect index or a music gain). Those handles only ever go back to the silent stubs (the backend is chosen once at startup)
   or through a NULL test before one (Movie_Rewind), so NULL here behaves the same.
*/
Bool8 SoundBackendDisabled_PlayOneShot
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice)

{
  if (outVoice != nullptr) {
    *outVoice = nullptr;
  }
  return true;
}

/* Silent-backend stub in slot g_SoundPlayLooping: plays nothing and reports success (returns true)
   with a NULL voice in *outVoice; the original never writes the voice (callers store an unrelated leftover
   value instead: a music gain, or the non-zero gain). As for
   SoundBackendDisabled_PlayOneShot those values only reach the silent stubs again, so NULL behaves the same
   (the spatial pool merely calls this stub again instead of the gain stub on the next frame).
*/
Bool8 SoundBackendDisabled_PlayLooping
          (SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          DirectSoundVoiceSet *voiceSet,IDirectSoundBuffer **outVoice)

{
  if (outVoice != nullptr) {
    *outVoice = nullptr;
  }
  return true;
}

/* Silent-backend stub in slot g_SoundStopVoice: nothing plays, so nothing to stop.
*/
void SoundBackendDisabled_StopVoice(IDirectSoundBuffer *voice)

{
  return;
}

/* Silent-backend stub in slot g_SoundIsVoicePlaying: always returns true, meaning the voice is not
   playing.
*/
Bool8 SoundBackendDisabled_IsVoicePlaying(IDirectSoundBuffer *voice)

{
  return true;
}

/* Silent-backend stub in slot g_SoundSetVoiceGains: ignores the new left/right gains.
*/
void SoundBackendDisabled_SetVoiceGains(SpatialSoundGainQ15 leftChannelGainQ15,SpatialSoundGainQ15 rightChannelGainQ15,
          IDirectSoundBuffer *voice)

{
  return;
}

/* Stop-all entry of the disabled sound backend (the initial value of g_SoundStopAllVoices until
   SdlAudio_Init installs SdlAudio_StopAllVoices): there are no voices, so it does nothing.
*/
void SoundBackendDisabled_StopAllVoices()

{
}
