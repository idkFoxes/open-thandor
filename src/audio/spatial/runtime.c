/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/spatial/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/spatial/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: audio/spatial/runtime. */

/* Address: 0x0050B5D0.
   Allocates the pool of SPATIAL_SOUND_SLOT_COUNT 0x10-byte spatial sound slots (0x1000 bytes) and zeroes it,
   so every slot starts without a voice set. Returns true on success; false with the allocator's error in
   *outError when the arena is exhausted.
*/
bool SpatialSoundPool_Init(uint32_t *outError)

{
  SpatialSoundSlot *clearCursor;
  int dwordsRemaining;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot),(void **)&clearCursor);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  g_SpatialSoundSlots = clearCursor;
  /* REP STOSD over the whole pool: the cursor advances one dword (to the next field) per step */
  for (dwordsRemaining = SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot) / 4; dwordsRemaining != 0;
       dwordsRemaining--) {
    clearCursor->voiceSet = NULL;
    clearCursor = (SpatialSoundSlot *)&clearCursor->activeVoice;
  }
  return true;
}


/* Address: 0x0050B600.
   Places the sound listener at the camera: g_SpatialSoundListenerTransform becomes the rotation built from
   the camera's view angles composed with a translation by -origin, i.e. world space to listener space, which
   the positioned-sound functions use to get distance and azimuth. Called directly by the frontend camera
   control setup in ui/frontend/runtime.c (no callback table).
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
  /* world-to-local: identity basis (Q28) with the translation -origin. The basis is written through the
     separate dword globals (g_SpatialSoundListenerWorldToLocalBasisRC = basisRowR[C]): as
     g_SpatialSoundListenerWorldToLocal.basisRow* members the compiler merges the stores
     into SSE stores. */
  g_SpatialSoundListenerWorldToLocal.translation.x = -originX;
  g_SpatialSoundListenerWorldToLocal.translation.y = -originY;
  g_SpatialSoundListenerWorldToLocal.translation.z = -originZ;
  g_SpatialSoundListenerWorldToLocalBasis00 = Q28_ONE;
  g_SpatialSoundListenerWorldToLocalBasis01 = 0;
  g_SpatialSoundListenerWorldToLocalBasis02 = 0;
  g_SpatialSoundListenerWorldToLocalBasis10 = 0;
  g_SpatialSoundListenerWorldToLocalBasis11 = Q28_ONE;
  g_SpatialSoundListenerWorldToLocalBasis12 = 0;
  g_SpatialSoundListenerWorldToLocalBasis20 = 0;
  g_SpatialSoundListenerWorldToLocalBasis21 = 0;
  g_SpatialSoundListenerWorldToLocalBasis22 = Q28_ONE;
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform,
             &g_SpatialSoundListenerWorldToLocal,&g_SpatialSoundListenerRotation);
  return;
}


/* Shared gain computation of the positioned one-shot and looping sounds. Transforms worldPosition into
   listener space (g_SpatialSoundRelative*), attenuates volumeQ15 with the distance and pans it by the
   azimuth. Returns false when the position is not closer than maximumDistanceQ12 or the attenuated gain is
   not above SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15. Otherwise stores two channel gains, each clamped to
   SPATIAL_SOUND_GAIN_Q15_FULL: *firstGainQ15 is the reduced one for an azimuth in the first half turn and
   *secondGainQ15 the reduced one in the second half turn; reverse stereo swaps the two. The callers map
   them to left/right differently (see SpatialSound_UpdateDesiredPositionedGains). */
static bool SpatialSound_ComputePositionedGains(SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,uint32_t volumeQ15,
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
            ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
             (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
  lengthAngles = FixedMath_VectorToAnglesAndLength
                    (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
  azimuth = lengthAngles.azimuthAngle;
  distanceQ12 = lengthAngles.lengthQ12;
  if (distanceQ12 >= maximumDistanceQ12) {
    return false;
  }
  /* attenuation: volume * cos(distance / maximum * quarter turn), the << 14 maps the ratio to
     0..FIXED_ANGLE16_QUARTER_TURN */
  scaledProduct = (int64_t)
           g_FixedCosQ28[(int)(((uint64_t)distanceQ12 << 14) / (uint64_t)maximumDistanceQ12)] *
           (int64_t)(int)volumeQ15;
  attenuatedGainQ15 = FIXED_PRODUCT_SHR(scaledProduct, 28);
  if (!(SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15 < (int)attenuatedGainQ15)) {
    return false;
  }
  /* pan: one channel keeps the full gain, the other gets gain * (1 + cos(2 * azimuth)) / 2 */
  if (azimuth < FIXED_ANGLE16_HALF_TURN) {
    firstQ15 = (uint32_t)((uint64_t)
                   ((int64_t)(g_FixedCosQ28[azimuth * 2] + Q28_ONE) *
                   (int64_t)(int)(attenuatedGainQ15 << 3)) >> 32);
    secondQ15 = attenuatedGainQ15;
  }
  else {
    /* the original reads [azimuth * 8 + 0x4046A0] (k_SpatialSoundStereoCosineSecondHalfBaseBias, the folded
       address g_FixedCosQ28 - 0x8000 * 8); written against g_FixedCosQ28 itself, because with generated
       image data 0x4046A0 lies in another object and would not reach the cosine table */
    scaledProduct = (int64_t)
            (g_FixedCosQ28[(azimuth - FIXED_ANGLE16_HALF_TURN) * 2] + Q28_ONE) *
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


/* Address: 0x0050B6E0.
   Plays a sound effect once at a world position: the gain (scaled by the effects volume) fades out with
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
  if ((voiceSetRef == NULL) || (volumeQ15 == 0)) {
    return;
  }
  /* the one-shot plays the first-half-reduced gain on the left channel */
  if (SpatialSound_ComputePositionedGains(maximumDistanceQ12,volumeQ15,worldPosition,&leftGainQ15,&rightGainQ15)) {
    g_SoundPlayOneShot(leftGainQ15,rightGainQ15,*voiceSetRef,NULL);
  }
}


/* Address: 0x0050B7D0.
   Looping counterpart of SpatialSound_PlayPositionedOneShot: computes the same distance attenuation and
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
  if ((slot == NULL) || (volumeQ15 == 0)) {
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


/* Address: 0x0050B8C0.
   Creates a voice set for the 'sam' asset and gives it the first free spatial-sound slot, silent and not
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
    return NULL;
  }
  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet == NULL) {
      slotCursor->voiceSet = voiceSet;
      slotCursor->desiredLeftGainQ15 = 0;
      slotCursor->desiredRightGainQ15 = 0;
      slotCursor->activeVoice = NULL;
      return slotCursor;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  g_SoundReleaseSampleVoiceSet(voiceSet);
  return NULL;
}


/* Address: 0x0050B940.
   PCM counterpart of SpatialSoundSlot_CreateFromSampleAsset: creates a voice set for raw PCM data and gives
   it the first free spatial-sound slot, silent and not playing. Returns the slot (never NULL); NULL when the
   voice set cannot be created or all slots are taken (the voice set is released again). The original's
   error value (voice-set error or FATAL_ERROR_GENERAL_FAILURE) is dropped like in the sample-asset variant.
   Nothing in this code base calls it and no callback-table slot references it.
*/
SpatialSoundSlot *SpatialSoundSlot_CreateFromPcm
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  DirectSoundVoiceSet *voiceSet;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;

  if (g_SoundCreatePcmVoiceSet(bufferByteCount,sampleRateHz,bitsPerSample,channelCount,pcmData,&voiceSet) != 0) {
    return NULL;
  }
  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet == NULL) {
      slotCursor->voiceSet = voiceSet;
      slotCursor->desiredLeftGainQ15 = 0;
      slotCursor->desiredRightGainQ15 = 0;
      slotCursor->activeVoice = NULL;
      return slotCursor;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  g_SoundReleasePcmVoiceSet(voiceSet);
  return NULL;
}


/* Address: 0x0050B9D0.
   Releases the sample voice set of a slot from SpatialSoundSlot_CreateFromSampleAsset and clears the slot
   (all four dwords), which makes it free again. A NULL slot is ignored.
*/
void SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot)

{
  int slotEntriesRemaining;
  
  slotEntriesRemaining = sizeof(SpatialSoundSlot) / 4;
  if (slot != NULL) {
    g_SoundReleaseSampleVoiceSet(slot->voiceSet);
    for (; slotEntriesRemaining != 0; slotEntriesRemaining--) {
      slot->voiceSet = NULL;
      slot = (SpatialSoundSlot *)&slot->activeVoice;
    }
  }
  return;
}


/* Address: 0x0050BA00.
   Releases the PCM voice set of a slot from SpatialSoundSlot_CreateFromPcm and clears the slot (all four
   dwords), which makes it free again. A NULL slot is ignored. Nothing in this code base calls it and no
   callback-table slot references it.
*/
void SpatialSoundSlot_ReleasePcm(SpatialSoundSlot *slot)

{
  int slotEntriesRemaining;
  
  slotEntriesRemaining = sizeof(SpatialSoundSlot) / 4;
  if (slot != NULL) {
    g_SoundReleasePcmVoiceSet(slot->voiceSet);
    /* REP STOSD: the cursor advances one dword (to the next field) per step */
    for (; slotEntriesRemaining != 0; slotEntriesRemaining--) {
      slot->voiceSet = NULL;
      slot = (SpatialSoundSlot *)&slot->activeVoice;
    }
  }
  return;
}


/* Address: 0x0050BA30.
   Start of a frame's positioned-sound pass: sets the desired gains of every used slot to 0, so that only the
   sounds whose gains are set again this frame keep playing when SpatialSoundPool_ApplyDesiredGains runs.
*/
void SpatialSoundPool_ClearDesiredGains(void)

{
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  
  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != NULL) {
      slotCursor->desiredLeftGainQ15 = 0;
      slotCursor->desiredRightGainQ15 = 0;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return;
}


/* Address: 0x0050BA60.
   End of a frame's positioned-sound pass: for every used slot, starts a looping voice when it has gains but
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
    if (slotCursor->voiceSet != NULL) {
      existingVoice = slotCursor->activeVoice;
      if (existingVoice == NULL) {
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
        slotCursor->activeVoice = NULL;
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

