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
   so every slot starts without a voice set. CF set with the allocator's error when the arena is exhausted.
*/
StatusResult SpatialSoundPool_Init(void)

{
  SpatialSoundSlot *clearCursor;
  int dwordsRemaining;
  bool allocationFailed;
  ArenaAllocResult allocResult;

  allocResult = g_MemoryApi.alloc(SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot));
  allocationFailed = allocResult.failed;
  clearCursor = (SpatialSoundSlot *)allocResult.payloadOrError;
  if (!allocationFailed) {
    g_SpatialSoundSlots = clearCursor;
    /* REP STOSD over the whole pool: the cursor advances one dword (to the next field) per step */
    for (dwordsRemaining = SPATIAL_SOUND_SLOT_COUNT * sizeof(SpatialSoundSlot) / 4; dwordsRemaining != 0;
         dwordsRemaining--) {
      clearCursor->voiceSet = NULL;
      clearCursor = (SpatialSoundSlot *)&clearCursor->activeVoice;
    }
    allocationFailed = false;
  }
  return allocationFailed ? StatusValue_Fail(allocResult.payloadOrError) : StatusValue_Ok(0);
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
            (&g_SpatialSoundListenerRotation,FIXED_ANGLE16_QUARTER_TURN - viewAngle0 & 0xffff,viewAngle1,
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


/* Address: 0x0050B6E0.
   Plays a sound effect once at a world position: the gain (scaled by the effects volume) fades out with
   the listener distance along a quarter cosine up to maximumDistanceQ12 and is panned by the azimuth
   around the listener (sides swapped with reverse stereo). Nothing plays when the voice set is missing,
   the position is out of range or the attenuated gain is not above SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15.
*/
void SpatialSound_PlayPositionedOneShot(SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef)

{
  int64_t scaledProduct;
  uint32_t distanceOrPannedGainQ15;
  uint32_t azimuthOrRightGainQ15;
  uint32_t volumeOrLeftGainQ15;
  FixedLengthAzimuthElevation lengthAngles;

  volumeOrLeftGainQ15 = gainQ15 * g_SoundEffectsGainQ15 >> 15;
  if ((voiceSetRef != NULL) && (volumeOrLeftGainQ15 != 0)) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
               (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
    lengthAngles = FixedMath_VectorToAnglesAndLength3Regs
                      (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
    azimuthOrRightGainQ15 = lengthAngles.azimuthAngle;
    distanceOrPannedGainQ15 = lengthAngles.lengthQ12;
    /* attenuation: volume * cos(distance / maximum * quarter turn), the << 14 maps the ratio to
       0..FIXED_ANGLE16_QUARTER_TURN */
    if ((distanceOrPannedGainQ15 < maximumDistanceQ12) &&
       (scaledProduct = (int64_t)
                g_FixedCosQ28
                [(int)(((uint64_t)distanceOrPannedGainQ15 << 0xe) / (uint64_t)maximumDistanceQ12)] *
                (int64_t)(int)volumeOrLeftGainQ15,
       volumeOrLeftGainQ15 = (int)((uint64_t)scaledProduct >> 0x20) << 4 | (uint32_t)scaledProduct >> 0x1c,
       SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15 < (int)volumeOrLeftGainQ15)) {
      /* pan: one channel keeps the full gain, the other gets gain * (1 + cos(2 * azimuth)) / 2 */
      if (azimuthOrRightGainQ15 < FIXED_ANGLE16_HALF_TURN) {
        distanceOrPannedGainQ15 = (uint32_t)((uint64_t)
                       ((int64_t)(g_FixedCosQ28[azimuthOrRightGainQ15 * 2] + Q28_ONE) *
                       (int64_t)(int)(volumeOrLeftGainQ15 << 3)) >> 0x20);
        azimuthOrRightGainQ15 = volumeOrLeftGainQ15;
      }
      else {
        /* k_SpatialSoundStereoCosineSecondHalfBaseBias lies 0x8000 * 8 bytes before g_FixedCosQ28, so this
           reads g_FixedCosQ28[(azimuth - FIXED_ANGLE16_HALF_TURN) * 2] */
        scaledProduct = (int64_t)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + azimuthOrRightGainQ15 * 8) + Q28_ONE) *
                (int64_t)(int)volumeOrLeftGainQ15;
        azimuthOrRightGainQ15 = (uint32_t)scaledProduct >> 0x1d | (int)((uint64_t)scaledProduct >> 0x20) << 3;
        distanceOrPannedGainQ15 = volumeOrLeftGainQ15;
      }
      volumeOrLeftGainQ15 = distanceOrPannedGainQ15;
      if (g_ReverseStereoMask != 0) {
        volumeOrLeftGainQ15 = azimuthOrRightGainQ15;
        azimuthOrRightGainQ15 = distanceOrPannedGainQ15;
      }
      if (SPATIAL_SOUND_GAIN_Q15_FULL < (int)azimuthOrRightGainQ15) {
        azimuthOrRightGainQ15 = SPATIAL_SOUND_GAIN_Q15_FULL;
      }
      if (SPATIAL_SOUND_GAIN_Q15_FULL < (int)volumeOrLeftGainQ15) {
        volumeOrLeftGainQ15 = SPATIAL_SOUND_GAIN_Q15_FULL;
      }
      g_SoundPlayOneShot(volumeOrLeftGainQ15,azimuthOrRightGainQ15,*voiceSetRef);
    }
  }
  return;
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
  int64_t scaledProduct;
  uint32_t distanceOrPannedGainQ15;
  uint32_t azimuthOrLeftGainQ15;
  uint32_t volumeOrRightGainQ15;
  FixedLengthAzimuthElevation lengthAngles;
  
  volumeOrRightGainQ15 = gainQ15 * g_SoundEffectsGainQ15 >> 15;
  if ((slot != NULL) && (volumeOrRightGainQ15 != 0)) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
               (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
    lengthAngles = FixedMath_VectorToAnglesAndLength3Regs
                      (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
    azimuthOrLeftGainQ15 = lengthAngles.azimuthAngle;
    distanceOrPannedGainQ15 = lengthAngles.lengthQ12;
    /* attenuation: volume * cos(distance / maximum * quarter turn), the << 14 maps the ratio to
       0..FIXED_ANGLE16_QUARTER_TURN */
    if ((distanceOrPannedGainQ15 < maximumDistanceQ12) &&
       (scaledProduct = (int64_t)
                g_FixedCosQ28
                [(int)(((uint64_t)distanceOrPannedGainQ15 << 0xe) / (uint64_t)maximumDistanceQ12)] *
                (int64_t)(int)volumeOrRightGainQ15,
       volumeOrRightGainQ15 = (int)((uint64_t)scaledProduct >> 0x20) << 4 | (uint32_t)scaledProduct >> 0x1c,
       SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15 < (int)volumeOrRightGainQ15)) {
      /* pan: one channel keeps the full gain, the other gets gain * (1 + cos(2 * azimuth)) / 2 */
      if (azimuthOrLeftGainQ15 < FIXED_ANGLE16_HALF_TURN) {
        distanceOrPannedGainQ15 = (uint32_t)((uint64_t)
                       ((int64_t)(g_FixedCosQ28[azimuthOrLeftGainQ15 * 2] + Q28_ONE) *
                       (int64_t)(int)(volumeOrRightGainQ15 << 3)) >> 0x20);
        azimuthOrLeftGainQ15 = volumeOrRightGainQ15;
      }
      else {
        /* k_SpatialSoundStereoCosineSecondHalfBaseBias lies 0x8000 * 8 bytes before g_FixedCosQ28, so this
           reads g_FixedCosQ28[(azimuth - FIXED_ANGLE16_HALF_TURN) * 2] */
        scaledProduct = (int64_t)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + azimuthOrLeftGainQ15 * 8) + Q28_ONE) *
                (int64_t)(int)volumeOrRightGainQ15;
        azimuthOrLeftGainQ15 = (uint32_t)scaledProduct >> 0x1d | (int)((uint64_t)scaledProduct >> 0x20) << 3;
        distanceOrPannedGainQ15 = volumeOrRightGainQ15;
      }
      volumeOrRightGainQ15 = distanceOrPannedGainQ15;
      if (g_ReverseStereoMask != 0) {
        volumeOrRightGainQ15 = azimuthOrLeftGainQ15;
        azimuthOrLeftGainQ15 = distanceOrPannedGainQ15;
      }
      if (SPATIAL_SOUND_GAIN_Q15_FULL < (int)azimuthOrLeftGainQ15) {
        azimuthOrLeftGainQ15 = SPATIAL_SOUND_GAIN_Q15_FULL;
      }
      if (SPATIAL_SOUND_GAIN_Q15_FULL < (int)volumeOrRightGainQ15) {
        volumeOrRightGainQ15 = SPATIAL_SOUND_GAIN_Q15_FULL;
      }
      slot->desiredLeftGainQ15 = azimuthOrLeftGainQ15;
      slot->desiredRightGainQ15 = volumeOrRightGainQ15;
    }
  }
  return;
}


/* Address: 0x0050B8C0.
   Creates a voice set for the 'sam' asset and gives it the first free spatial-sound slot, silent and not
   playing, so that SpatialSound_UpdateDesiredPositionedGains can drive it as a looping positioned sound.
   Returns the slot with CF clear; CF set with the voice-set error, or FATAL_ERROR_GENERAL_FAILURE when the
   pool is full (the new voice set is released again).
*/
SpatialSoundSlotResult SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset)

{
  SpatialSoundSlot *voiceSetOrError;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  SampleVoiceSetResult createResult;
  SpatialSoundSlotResult failureResult;
  SpatialSoundSlotResult successResult;
  
  createResult = g_SoundCreateSampleVoiceSet(sampleAsset);
  voiceSetOrError = (SpatialSoundSlot *)createResult.voiceSet;
  if (!createResult.failed) {
    slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
    slotCursor = g_SpatialSoundSlots;
    do {
      if (slotCursor->voiceSet == NULL) {
        slotCursor->voiceSet = (DirectSoundVoiceSet *)voiceSetOrError;
        slotCursor->desiredLeftGainQ15 = 0;
        slotCursor->desiredRightGainQ15 = 0;
        slotCursor->activeVoice = NULL;
        successResult.failed = false;
        successResult.soundSlot = slotCursor;
        return successResult;
      }
      slotCursor++;
      slotsRemaining--;
    } while (slotsRemaining != 0);
    g_SoundReleaseSampleVoiceSet((DirectSoundVoiceSet *)voiceSetOrError);
    voiceSetOrError = (SpatialSoundSlot *)FATAL_ERROR_GENERAL_FAILURE;
  }
  failureResult.failed = true;
  failureResult.soundSlot = voiceSetOrError;
  return failureResult;
}


/* Address: 0x0050B940.
   PCM counterpart of SpatialSoundSlot_CreateFromSampleAsset: creates a voice set for raw PCM data and gives
   it the first free spatial-sound slot, silent and not playing. CF clear with the slot; CF set with the
   voice-set error, or with FATAL_ERROR_GENERAL_FAILURE when all slots are taken (the voice set is released
   again). Nothing in this code base calls it and no callback-table slot references it.
*/
SpatialSoundSlotResult SpatialSoundSlot_CreateFromPcm
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  SpatialSoundSlot *voiceSetOrError;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  PcmVoiceSetResult createResult;
  SpatialSoundSlotResult failureResult;
  SpatialSoundSlotResult successResult;
  
  createResult = g_SoundCreatePcmVoiceSet
                    (bufferByteCount,sampleRateHz,bitsPerSample,channelCount,pcmData);
  voiceSetOrError = (SpatialSoundSlot *)createResult.voiceSet;
  if (!createResult.failed) {
    slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
    slotCursor = g_SpatialSoundSlots;
    do {
      if (slotCursor->voiceSet == NULL) {
        slotCursor->voiceSet = (DirectSoundVoiceSet *)voiceSetOrError;
        slotCursor->desiredLeftGainQ15 = 0;
        slotCursor->desiredRightGainQ15 = 0;
        slotCursor->activeVoice = NULL;
        successResult.failed = false;
        successResult.soundSlot = slotCursor;
        return successResult;
      }
      slotCursor++;
      slotsRemaining--;
    } while (slotsRemaining != 0);
    g_SoundReleasePcmVoiceSet((DirectSoundVoiceSet *)voiceSetOrError);
    voiceSetOrError = (SpatialSoundSlot *)FATAL_ERROR_GENERAL_FAILURE;
  }
  failureResult.failed = true;
  failureResult.soundSlot = voiceSetOrError;
  return failureResult;
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
  SoundPlayResult playResult;
  
  slotsRemaining = SPATIAL_SOUND_SLOT_COUNT;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != NULL) {
      existingVoice = slotCursor->activeVoice;
      if (existingVoice == NULL) {
        if (slotCursor->desiredLeftGainQ15 != 0 || slotCursor->desiredRightGainQ15 != 0) {
          playResult = g_SoundPlayLooping
                            (slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,
                             slotCursor->voiceSet);
          activeVoice = playResult.soundBuffer;
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

