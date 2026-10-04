/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/audio.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: audio. One SDL audio stream (22050 Hz, 16-bit, stereo, as DirectSound's primary buffer) whose
   callback mixes every playing voice. The g_Sound* slots behave like the DirectSound ones (audio/backend/runtime.c):
   a voice set holds one decoded .sam sample and up to eight voices playing it (the first idle voice plays, a new one
   is added while fewer than eight exist), one-shot or looping; a stopped voice keeps its position and resumes there,
   one that played to its end starts again from the beginning. The two Q15 channel gains become DirectSound's
   volume and pan through the same attenuation table (1/100 dB) and are applied as DirectSound applies them.
   Voice sets and voices are opaque handles for the game: a voice set is an arena block of the DirectSoundVoiceSet
   size (the arena layout stays as with DirectSound), a voice is a pointer to the backend's own voice object.
   The mixer is locked against the game's threads (main and timer threads) with a mutex. */

#include <thandor/platform/sdl3/sdl_objects.h>

#include <SDL3/SDL_init.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <mutex>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

namespace {

constexpr int kSampleRate = 22050;
constexpr int kChannels = 2;
constexpr std::size_t kSamplesPerDecodedBlock = SOUND_SAMPLE_DECODED_BLOCK_BYTES / sizeof(int16_t);
constexpr std::size_t kVoicesPerSet = DIRECTSOUND_VOICES_PER_SET;

/* One decoded sample: interleaved 16-bit stereo PCM. */
struct Sample {
  std::vector<int16_t> pcm;
  std::size_t FrameCount() const noexcept { return pcm.size() / kChannels; }
};

struct Voice {
  std::shared_ptr<const Sample> sample;
  std::size_t positionFrames = 0;
  bool playing = false;
  bool looping = false;
  float leftGain = 1.0f;
  float rightGain = 1.0f;
};

struct VoiceSet {
  DirectSoundVoiceSet *handle = nullptr; /* the arena block the game holds */
  std::shared_ptr<const Sample> sample;
  std::array<std::unique_ptr<Voice>, kVoicesPerSet> voices;
};

/* The game's voice handle for a voice, and back. */
IDirectSoundBuffer *HandleOf(Voice *voice) noexcept
{
  return reinterpret_cast<IDirectSoundBuffer *>(voice);
}

class Mixer {
public:
  std::mutex mutex;
  thandor::sdl3::AudioStreamPtr stream;
  std::unordered_map<DirectSoundVoiceSet *, std::unique_ptr<VoiceSet>> voiceSets;
  std::unordered_set<const Voice *> liveVoices;
  std::vector<float> accumulator;
  std::vector<int16_t> output;

  /* The live voice behind a game handle; nullptr for NULL, stale or foreign handles. Needs the mutex. */
  Voice *VoiceOf(IDirectSoundBuffer *handle)
  {
    auto *voice = reinterpret_cast<Voice *>(handle);
    return liveVoices.contains(voice) ? voice : nullptr;
  }

  VoiceSet *VoiceSetOf(DirectSoundVoiceSet *handle)
  {
    const auto found = voiceSets.find(handle);
    return (found != voiceSets.end()) ? found->second.get() : nullptr;
  }

  /* Mixes frameCount frames of every playing voice into output. */
  void Mix(std::size_t frameCount)
  {
    accumulator.assign(frameCount * kChannels, 0.0f);
    {
      const std::scoped_lock lock(mutex);
      for (auto &entry : voiceSets) {
        for (auto &voicePointer : entry.second->voices) {
          if (voicePointer && voicePointer->playing) {
            MixVoice(*voicePointer, frameCount);
          }
        }
      }
    }
    output.resize(frameCount * kChannels);
    std::transform(accumulator.begin(), accumulator.end(), output.begin(), [](float value) {
      return static_cast<int16_t>(std::clamp(std::lround(value), long{INT16_MIN}, long{INT16_MAX}));
    });
  }

private:
  void MixVoice(Voice &voice, std::size_t frameCount)
  {
    const std::span<const int16_t> pcm(voice.sample->pcm);
    const std::size_t sampleFrames = voice.sample->FrameCount();
    for (std::size_t frame = 0; frame < frameCount; frame++) {
      if (voice.positionFrames >= sampleFrames) {
        voice.positionFrames = 0;
        if (!voice.looping) {
          voice.playing = false; /* played to its end: stopped at the beginning, as a DirectSound buffer */
          return;
        }
        if (sampleFrames == 0) {
          return;
        }
      }
      accumulator[frame * kChannels] += pcm[voice.positionFrames * kChannels] * voice.leftGain;
      accumulator[frame * kChannels + 1] += pcm[voice.positionFrames * kChannels + 1] * voice.rightGain;
      voice.positionFrames++;
    }
  }
};

std::unique_ptr<Mixer> s_mixer;
/* the 256-entry voice-set registry in the arena, as DirectSound_Init allocates it */
DirectSoundVoiceSet **s_registry = nullptr;

void SDLCALL MixCallback(void * /*userdata*/, SDL_AudioStream *stream, int additionalAmount, int /*totalAmount*/)
{
  if ((additionalAmount <= 0) || !s_mixer) {
    return;
  }
  const auto frameCount = static_cast<std::size_t>(additionalAmount) / (kChannels * sizeof(int16_t));
  s_mixer->Mix(frameCount);
  SDL_PutAudioStreamData(stream, s_mixer->output.data(),
                         static_cast<int>(s_mixer->output.size() * sizeof(int16_t)));
}

/* A DirectSound attenuation (1/100 dB) as a linear factor; -10000 is silence. */
float LinearGain(int32_t attenuation) noexcept
{
  attenuation = std::max(attenuation, int32_t{-10000});
  return (attenuation <= -10000) ? 0.0f : std::pow(10.0f, static_cast<float>(attenuation) / 2000.0f);
}

/* DirectSound_ApplyChannelGains: the louder channel's attenuation is the volume, left - right attenuation the pan;
   DirectSound then attenuates the left channel by a positive pan and the right one by a negative pan. */
void ApplyChannelGains(uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, Voice &voice) noexcept
{
  const int32_t leftAttenuation = g_DirectSoundGainAttenuation[std::min<uint32_t>(leftChannelGainQ15 >> 8, 128)];
  const int32_t rightAttenuation = g_DirectSoundGainAttenuation[std::min<uint32_t>(rightChannelGainQ15 >> 8, 128)];
  const int32_t volume = (rightChannelGainQ15 < leftChannelGainQ15) ? leftAttenuation : rightAttenuation;
  const int32_t pan = leftAttenuation - rightAttenuation;
  voice.leftGain = LinearGain(volume - std::max(pan, int32_t{0}));
  voice.rightGain = LinearGain(volume + std::min(pan, int32_t{0}));
}

/* DirectSound_FailVoiceSet: the failing stage as text in g_PackageLastErrorPath, then the error code. */
uint32_t FailVoiceSet(int32_t failedStage, uint32_t errorCode) noexcept
{
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR, 0, 10, 1, failedStage, g_PackageLastErrorPath);
  return errorCode;
}

Bool8 PlayVoiceSet(uint32_t leftChannelGainQ15, uint32_t rightChannelGainQ15, DirectSoundVoiceSet *handle,
                   bool looping, IDirectSoundBuffer **outVoice)
{
  if (outVoice != nullptr) {
    *outVoice = nullptr;
  }
  if ((handle == nullptr) || !s_mixer) {
    return false;
  }
  const std::scoped_lock lock(s_mixer->mutex);
  VoiceSet *voiceSet = s_mixer->VoiceSetOf(handle);
  if (voiceSet == nullptr) {
    return false;
  }
  Voice *voice = nullptr;
  for (std::size_t slot = 0; slot < kVoicesPerSet; slot++) {
    if (!voiceSet->voices[slot]) {
      /* DuplicateSoundBuffer of voice 0, rewound */
      voiceSet->voices[slot] = std::make_unique<Voice>();
      voiceSet->voices[slot]->sample = voiceSet->sample;
      voice = voiceSet->voices[slot].get();
      s_mixer->liveVoices.insert(voice);
      handle->voices[slot] = HandleOf(voice);
      break;
    }
    if (!voiceSet->voices[slot]->playing) {
      voice = voiceSet->voices[slot].get();
      break;
    }
  }
  if (voice == nullptr) {
    return false; /* all eight voices busy */
  }
  voice->playing = true; /* from the current position */
  voice->looping = looping;
  ApplyChannelGains(leftChannelGainQ15, rightChannelGainQ15, *voice);
  if (outVoice != nullptr) {
    *outVoice = HandleOf(voice);
  }
  return true;
}

} // namespace

uint32_t SdlAudio_Init(void)
{
  if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    /* no audio device: the game runs silent, as DirectSound_Init without a device */
    Thandor_Log("SDL audio: %s (running silent)", SDL_GetError());
    return 0;
  }
  const SDL_AudioSpec spec{SDL_AUDIO_S16, kChannels, kSampleRate};
  auto mixer = std::make_unique<Mixer>();
  mixer->stream.reset(SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, MixCallback, nullptr));
  if (!mixer->stream) {
    Thandor_Log("SDL audio: no playback stream: %s (running silent)", SDL_GetError());
    return 0;
  }
  void *registryPayload = nullptr;
  const uint32_t registryAllocError =
      g_MemoryApi.alloc(DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY * sizeof(DirectSoundVoiceSet *), &registryPayload);
  if (registryAllocError != 0) {
    return registryAllocError;
  }
  s_registry = static_cast<DirectSoundVoiceSet **>(registryPayload);
  std::fill_n(s_registry, DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY, nullptr);
  SDL_AudioStream *stream = mixer->stream.get();
  s_mixer = std::move(mixer);
  g_SoundCreateSampleVoiceSet = SdlAudio_CreateSampleVoiceSet;
  g_SoundReleaseSampleVoiceSet = SdlAudio_ReleaseSampleVoiceSet;
  g_SoundPlayOneShot = SdlAudio_PlayOneShot;
  g_SoundPlayLooping = SdlAudio_PlayLooping;
  g_SoundStopVoice = SdlAudio_StopVoice;
  g_SoundStopAllVoices = SdlAudio_StopAllVoices;
  g_SoundIsVoicePlaying = SdlAudio_IsVoicePlaying;
  g_SoundSetVoiceGains = SdlAudio_SetVoiceGains;
  CosineDerivedLookupTables_Init();
  SDL_ResumeAudioStreamDevice(stream);
  return 0;
}

void SdlAudio_Shutdown(void)
{
  if (s_mixer) {
    s_mixer->stream.reset(); /* closes the device; the callback has ended */
    s_mixer.reset();
  }
  g_MemoryApi.free(s_registry);
  s_registry = nullptr;
  /* back to the silent backend */
  g_SoundCreateSampleVoiceSet = SoundBackendDisabled_CreateSampleVoiceSet;
  g_SoundReleaseSampleVoiceSet = SoundBackendDisabled_ReleaseSampleVoiceSet;
  g_SoundPlayOneShot = SoundBackendDisabled_PlayOneShot;
  g_SoundPlayLooping = SoundBackendDisabled_PlayLooping;
  g_SoundStopVoice = SoundBackendDisabled_StopVoice;
  g_SoundStopAllVoices = SoundBackendDisabled_StopAllVoices;
  g_SoundIsVoicePlaying = SoundBackendDisabled_IsVoicePlaying;
  g_SoundSetVoiceGains = SoundBackendDisabled_SetVoiceGains;
}

uint32_t SdlAudio_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet)
{
  if ((sampleAsset->magic != ASSET_MAGIC_SAM) || (sampleAsset->formatVersion != SOUND_SAMPLE_FORMAT_VERSION)) {
    return FailVoiceSet(DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER, FATAL_ERROR_SOUND_SAMPLE_INVALID);
  }
  const std::size_t blockCount = sampleAsset->decodedBlockCount;
  if (blockCount == 0) {
    return FailVoiceSet(DIRECTSOUND_VOICE_STAGE_CREATE_BUFFER, FATAL_ERROR_DIRECTSOUND_SETUP); /* empty buffer */
  }
  /* decode every packed block (they follow the 0x200-byte header) */
  auto sample = std::make_shared<Sample>();
  sample->pcm.resize(blockCount * kSamplesPerDecodedBlock);
  std::array<short, 256> coefficients{};
  auto *encodedBlock = reinterpret_cast<uint8_t *>(sampleAsset + 1);
  for (std::size_t block = 0; block < blockCount; block++) {
    const uint32_t encodedBlockSize = SoundSample_DecodePackedCoefficientBlock(coefficients.data(), encodedBlock);
    SoundSample_DecodeCoefficientBlockToPcmMmx(sample->pcm.data() + block * kSamplesPerDecodedBlock,
                                               coefficients.data());
    encodedBlock += encodedBlockSize;
  }
  void *voiceSetPayload = nullptr;
  const uint32_t voiceSetAllocError = g_MemoryApi.alloc(sizeof(DirectSoundVoiceSet), &voiceSetPayload);
  if (voiceSetAllocError != 0) {
    return FailVoiceSet(DIRECTSOUND_VOICE_STAGE_FILL, voiceSetAllocError);
  }
  auto *handle = static_cast<DirectSoundVoiceSet *>(voiceSetPayload);
  std::fill(std::begin(handle->voices), std::end(handle->voices), nullptr);
  auto voiceSet = std::make_unique<VoiceSet>();
  voiceSet->handle = handle;
  voiceSet->sample = sample;
  voiceSet->voices[0] = std::make_unique<Voice>();
  voiceSet->voices[0]->sample = sample;
  handle->voices[0] = HandleOf(voiceSet->voices[0].get());
  if (s_mixer) {
    const std::scoped_lock lock(s_mixer->mutex);
    s_mixer->liveVoices.insert(voiceSet->voices[0].get());
    s_mixer->voiceSets.emplace(handle, std::move(voiceSet));
  }
  /* the first free registry slot (a full registry is not an error) */
  if (s_registry != nullptr) {
    const std::span<DirectSoundVoiceSet *> registry(s_registry, DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY);
    const auto freeSlot = std::find(registry.begin(), registry.end(), nullptr);
    if (freeSlot != registry.end()) {
      *freeSlot = handle;
    }
  }
  *outVoiceSet = handle;
  return 0;
}

void SdlAudio_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet)
{
  if ((voiceSet == nullptr) || !s_mixer) {
    return;
  }
  {
    const std::scoped_lock lock(s_mixer->mutex);
    const auto found = s_mixer->voiceSets.find(voiceSet);
    if (found == s_mixer->voiceSets.end()) {
      return;
    }
    for (const auto &voice : found->second->voices) {
      s_mixer->liveVoices.erase(voice.get());
    }
    s_mixer->voiceSets.erase(found);
  }
  g_MemoryApi.free(voiceSet);
  if (s_registry != nullptr) {
    const std::span<DirectSoundVoiceSet *> registry(s_registry, DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY);
    const auto slot = std::find(registry.begin(), registry.end(), voiceSet);
    if (slot != registry.end()) {
      *slot = nullptr;
    }
  }
}

Bool8 SdlAudio_PlayOneShot(uint32_t leftChannelGainQ15,uint32_t rightChannelGainQ15,DirectSoundVoiceSet *voiceSet,
                           IDirectSoundBuffer **outVoice)
{
  return PlayVoiceSet(leftChannelGainQ15, rightChannelGainQ15, voiceSet, false, outVoice);
}

Bool8 SdlAudio_PlayLooping(uint32_t leftChannelGainQ15,uint32_t rightChannelGainQ15,DirectSoundVoiceSet *voiceSet,
                           IDirectSoundBuffer **outVoice)
{
  return PlayVoiceSet(leftChannelGainQ15, rightChannelGainQ15, voiceSet, true, outVoice);
}

void SdlAudio_StopVoice(IDirectSoundBuffer *voice)
{
  if (!s_mixer) {
    return;
  }
  const std::scoped_lock lock(s_mixer->mutex);
  if (Voice *stopped = s_mixer->VoiceOf(voice)) {
    stopped->playing = false; /* the position stays, as with IDirectSoundBuffer::Stop */
  }
}

Bool8 SdlAudio_IsVoicePlaying(IDirectSoundBuffer *voice)
{
  if (!s_mixer) {
    return true;
  }
  const std::scoped_lock lock(s_mixer->mutex);
  const Voice *queried = s_mixer->VoiceOf(voice);
  return (queried == nullptr) || !queried->playing;
}

void SdlAudio_SetVoiceGains(uint32_t leftChannelGainQ15,uint32_t rightChannelGainQ15,IDirectSoundBuffer *voice)
{
  if (!s_mixer) {
    return;
  }
  const std::scoped_lock lock(s_mixer->mutex);
  if (Voice *changed = s_mixer->VoiceOf(voice)) {
    ApplyChannelGains(leftChannelGainQ15, rightChannelGainQ15, *changed);
  }
}

void SdlAudio_StopAllVoices(void)
{
  /* every voice of every voice set in the registry */
  if (!s_mixer || (s_registry == nullptr)) {
    return;
  }
  const std::scoped_lock lock(s_mixer->mutex);
  for (DirectSoundVoiceSet *handle : std::span<DirectSoundVoiceSet *>(s_registry, DIRECTSOUND_VOICE_SET_REGISTRY_CAPACITY)) {
    if (VoiceSet *voiceSet = (handle != nullptr) ? s_mixer->VoiceSetOf(handle) : nullptr) {
      for (const auto &voice : voiceSet->voices) {
        if (voice) {
          voice->playing = false;
        }
      }
    }
  }
}
