/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/spatial/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/spatial/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(16) int32_t g_ReverseStereoMask = 0;

static GraphicsFixedMatrix3x4 g_SpatialSoundListenerTransform = {0};

static GraphicsFixedMatrix3x4 g_SpatialSoundListenerRotation = {0};

static GraphicsFixedMatrix3x4 g_SpatialSoundListenerWorldToLocal = {0};

/* sound position in the listener's frame */
static GraphicsFixedVec3 g_SpatialSoundRelative = {0};

static SpatialSoundSlot *g_SpatialSoundSlots = nullptr;

AudioMixerGainQ15 g_SoundEffectsGainQ15 = 32768;

/* Implementation ownership: audio/spatial/runtime. */

/* Allocates the pool of SPATIAL_SOUND_SLOT_COUNT 0x10-byte spatial sound slots (0x1000 bytes) and zeroes it,
   so every slot starts without a voice set. Returns true on success; false with the allocator's error in
   *outError when the arena is exhausted.
*/
Bool8 SpatialSoundPool_Init(uint32_t *outError)

{
  SpatialSoundSlot *clearCursor;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot),(void **)&clearCursor);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  g_SpatialSoundSlots = clearCursor;
  /* the whole pool is zeroed */
  memset(clearCursor,0,SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot));
  return true;
}


/* Places the sound listener at the camera: g_SpatialSoundListenerTransform becomes the rotation built from
   the camera's view angles composed with a translation by -origin, i.e. world space to listener space, which
   the positioned-sound functions use to get distance and azimuth. Called directly by the frontend camera
   control setup in ui/frontend/menu_room.cpp (no callback table).
*/
void SpatialSound_RebuildListenerTransformFromPose
          (AngleTurn32 viewAngle1,AngleTurn32 viewAngle0,GraphicsWorldCoordinateQ12 originZ,
          GraphicsWorldCoordinateQ12 originY,GraphicsWorldCoordinateQ12 originX)

{
  FixedTransform_BuildRotationBasis
            (&g_SpatialSoundListenerRotation,FIXED_ANGLE16_QUARTER_TURN - viewAngle0 & FIXED_ANGLE16_MASK,viewAngle1,
             FIXED_ANGLE16_HALF_TURN + FIXED_ANGLE16_QUARTER_TURN);
  g_SpatialSoundListenerRotation.translation.x = 0;
  g_SpatialSoundListenerRotation.translation.y = 0;
  g_SpatialSoundListenerRotation.translation.z = 0;
  /* world-to-local: identity basis (Q28) with the translation -origin. */
  g_SpatialSoundListenerWorldToLocal.translation.x = -originX;
  g_SpatialSoundListenerWorldToLocal.translation.y = -originY;
  g_SpatialSoundListenerWorldToLocal.translation.z = -originZ;
  g_SpatialSoundListenerWorldToLocal.basisRow0[0] = Q28_ONE;
  g_SpatialSoundListenerWorldToLocal.basisRow0[1] = 0;
  g_SpatialSoundListenerWorldToLocal.basisRow0[2] = 0;
  g_SpatialSoundListenerWorldToLocal.basisRow1[0] = 0;
  g_SpatialSoundListenerWorldToLocal.basisRow1[1] = Q28_ONE;
  g_SpatialSoundListenerWorldToLocal.basisRow1[2] = 0;
  g_SpatialSoundListenerWorldToLocal.basisRow2[0] = 0;
  g_SpatialSoundListenerWorldToLocal.basisRow2[1] = 0;
  g_SpatialSoundListenerWorldToLocal.basisRow2[2] = Q28_ONE;
  FixedTransform_Compose
            (&g_SpatialSoundListenerTransform,
             &g_SpatialSoundListenerWorldToLocal,&g_SpatialSoundListenerRotation);
  return;
}


/* Shared gain computation of the positioned one-shot and looping sounds. Transforms worldPosition into
   listener space (g_SpatialSoundRelative),attenuates volumeQ15 with the distance and pans it by the
   azimuth. Returns false when the position is not closer than maximumDistanceQ12 or the attenuated gain is
   not above SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15. Otherwise stores two channel gains, each clamped to
   SPATIAL_SOUND_GAIN_Q15_FULL: *firstGainQ15 is the reduced one for an azimuth in the first half turn and
   *secondGainQ15 the reduced one in the second half turn; reverse stereo swaps the two. The callers map
   them to left/right differently (see SpatialSound_UpdateDesiredPositionedGains). */
static Bool8 SpatialSound_ComputePositionedGains(SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,uint32_t volumeQ15,
          GraphicsFixedVec3 *worldPosition,uint32_t *firstGainQ15,uint32_t *secondGainQ15)

{
  int64_t scaledProduct;
  uint32_t distanceQ12;
  uint32_t azimuth;
  uint32_t attenuatedGainQ15;
  uint32_t firstQ15;
  uint32_t secondQ15;
  uint32_t swappedQ15;
  FixedLengthAzimuthElevation lengthAngles;

  FixedTransform_ApplyPoint
            (&g_SpatialSoundRelative,worldPosition,
             &g_SpatialSoundListenerTransform);
  lengthAngles = FixedMath_VectorToAnglesAndLength
                    (g_SpatialSoundRelative.y,g_SpatialSoundRelative.x,g_SpatialSoundRelative.z);
  azimuth = lengthAngles.azimuthAngle;
  distanceQ12 = lengthAngles.lengthQ12;
  if (distanceQ12 >= maximumDistanceQ12) {
    return false;
  }
  /* attenuation: volume * cos(distance / maximum * quarter turn), the << 14 maps the ratio to
     0..FIXED_ANGLE16_QUARTER_TURN */
  scaledProduct = (int64_t)
           g_FixedSineQ28[FIXED_SINE_TABLE_COS + (int)(((uint64_t)distanceQ12 << 14) / (uint64_t)maximumDistanceQ12)] *
           (int64_t)(int)volumeQ15;
  attenuatedGainQ15 = FIXED_PRODUCT_SHR(scaledProduct, 28);
  if (!(SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15 < (int)attenuatedGainQ15)) {
    return false;
  }
  /* pan: one channel keeps the full gain, the other gets gain * (1 + cos(2 * azimuth)) / 2 */
  if (azimuth < FIXED_ANGLE16_HALF_TURN) {
    firstQ15 = (uint32_t)((uint64_t)
                   ((int64_t)(g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuth * 2] + Q28_ONE) *
                   (int64_t)(int)(attenuatedGainQ15 << 3)) >> 32);
    secondQ15 = attenuatedGainQ15;
  }
  else {
    /* the original reads [azimuth * 8 + 0x4046A0] (k_SpatialSoundStereoCosineSecondHalfBaseBias, the folded
       address of the cosine part of g_FixedSineQ28 - 0x8000 * 8); written against the cosine part itself,
       because with generated image data 0x4046A0 lies in another object and would not reach the table */
    scaledProduct = (int64_t)
            (g_FixedSineQ28[FIXED_SINE_TABLE_COS + (azimuth - FIXED_ANGLE16_HALF_TURN) * 2] + Q28_ONE) *
            (int64_t)(int)attenuatedGainQ15;
    secondQ15 = FIXED_PRODUCT_SHR(scaledProduct,29);
    firstQ15 = attenuatedGainQ15;
  }
  if (g_ReverseStereoMask != 0) {
    swappedQ15 = firstQ15;
    firstQ15 = secondQ15;
    secondQ15 = swappedQ15;
  }
  if (SPATIAL_SOUND_GAIN_Q15_FULL < (int)secondQ15) {
    secondQ15 = SPATIAL_SOUND_GAIN_Q15_FULL;
  }
  if (SPATIAL_SOUND_GAIN_Q15_FULL < (int)firstQ15) {
    firstQ15 = SPATIAL_SOUND_GAIN_Q15_FULL;
  }
  *firstGainQ15 = firstQ15;
  *secondGainQ15 = secondQ15;
  return true;
}


/* Plays a sound effect once at a world position: the gain (scaled by the effects volume) fades out with
   the listener distance along a quarter cosine up to maximumDistanceQ12 and is panned by the azimuth
   around the listener (sides swapped with reverse stereo). Nothing plays when the voice set is missing,
   the position is out of range or the attenuated gain is not above SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15.
*/
void SpatialSound_PlayPositionedOneShot(SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef)

{
  uint32_t volumeQ15;
  uint32_t leftGainQ15;
  uint32_t rightGainQ15;

  volumeQ15 = gainQ15 * g_SoundEffectsGainQ15 >> 15;
  if ((voiceSetRef == nullptr) || (volumeQ15 == 0)) {
    return;
  }
  /* the one-shot plays the first-half-reduced gain on the left channel */
  if (SpatialSound_ComputePositionedGains(maximumDistanceQ12,volumeQ15,worldPosition,&leftGainQ15,&rightGainQ15)) {
    g_SoundPlayOneShot(leftGainQ15,rightGainQ15,*voiceSetRef,nullptr);
  }
}


/* Looping counterpart of SpatialSound_PlayPositionedOneShot: computes the same distance attenuation and
   azimuth panning and stores the result as the slot's desired gains, which SpatialSoundPool_ApplyDesiredGains
   turns into start/stop/gain updates at the end of the frame. An out-of-range or inaudible sound leaves the
   gains at the 0 that SpatialSoundPool_ClearDesiredGains set, so it stops. Called directly by the army and shot
   sound updates (gameplay/army, world/shots; no callback table).
*/
void SpatialSound_UpdateDesiredPositionedGains
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot)

{
  uint32_t volumeQ15;
  uint32_t leftGainQ15;
  uint32_t rightGainQ15;

  volumeQ15 = gainQ15 * g_SoundEffectsGainQ15 >> 15;
  if ((slot == nullptr) || (volumeQ15 == 0)) {
    return;
  }
  /* Original quirk: the channels are mapped the other way round than in SpatialSound_PlayPositionedOneShot
     (the first-half-reduced gain goes to the right channel here), so looping and one-shot sounds at the
     same position pan to opposite sides (unless the play callback's left/right parameter names are swapped). */
  if (SpatialSound_ComputePositionedGains(maximumDistanceQ12,volumeQ15,worldPosition,&rightGainQ15,&leftGainQ15)) {
    slot->desiredLeftGainQ15 = leftGainQ15;
    slot->desiredRightGainQ15 = rightGainQ15;
  }
}


/* Creates a voice set for the 'sam' asset and gives it the first free spatial-sound slot, silent and not
   playing, so that SpatialSound_UpdateDesiredPositionedGains can drive it as a looping positioned sound.
   Returns the slot (never NULL, it lies in the pool); NULL when the voice set cannot be created or the pool
   is full (the new voice set is released again). The original's error value (voice-set error or
   FATAL_ERROR_GENERAL_FAILURE) was never read by a caller.
*/
SpatialSoundSlot *SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset)

{
  DirectSoundVoiceSet *voiceSet;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;

  if (g_SoundCreateSampleVoiceSet(sampleAsset,&voiceSet) != 0) {
    return nullptr;
  }
  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet == nullptr) {
      slotCursor->voiceSet = voiceSet;
      slotCursor->desiredLeftGainQ15 = 0;
      slotCursor->desiredRightGainQ15 = 0;
      slotCursor->activeVoice = nullptr;
      return slotCursor;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  g_SoundReleaseSampleVoiceSet(voiceSet);
  return nullptr;
}


/* Releases the sample voice set of a slot from SpatialSoundSlot_CreateFromSampleAsset and clears the slot
   (all four fields), which makes it free again. A NULL slot is ignored.
*/
void SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot)

{
  if (slot != nullptr) {
    g_SoundReleaseSampleVoiceSet(slot->voiceSet);
    memset(slot,0,sizeof(SpatialSoundSlot));
  }
  return;
}


/* Start of a frame's positioned-sound pass: sets the desired gains of every used slot to 0, so that only the
   sounds whose gains are set again this frame keep playing when SpatialSoundPool_ApplyDesiredGains runs.
*/
void SpatialSoundPool_ClearDesiredGains(void)

{
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  
  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != nullptr) {
      slotCursor->desiredLeftGainQ15 = 0;
      slotCursor->desiredRightGainQ15 = 0;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return;
}


/* End of a frame's positioned-sound pass: for every used slot, starts a looping voice when it has gains but
   is not playing, stops the voice when both gains are 0, and otherwise updates the voice's gains.
*/
void SpatialSoundPool_ApplyDesiredGains(void)

{
  IDirectSoundBuffer *existingVoice;
  IDirectSoundBuffer *activeVoice;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;

  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != nullptr) {
      existingVoice = slotCursor->activeVoice;
      if (existingVoice == nullptr) {
        if (slotCursor->desiredLeftGainQ15 != 0 || slotCursor->desiredRightGainQ15 != 0) {
          /* the voice is stored whether or not it plays (NULL on failure) */
          g_SoundPlayLooping
                    (slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,
                     slotCursor->voiceSet,&activeVoice);
          slotCursor->activeVoice = activeVoice;
        }
      }
      else if (slotCursor->desiredLeftGainQ15 == 0 && slotCursor->desiredRightGainQ15 == 0) {
        g_SoundStopVoice(existingVoice);
        slotCursor->activeVoice = nullptr;
      }
      else {
        g_SoundSetVoiceGains(slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,existingVoice);
      }
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return;
}

