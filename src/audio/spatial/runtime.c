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

  allocResult = g_MemoryApi.alloc(0x1000);
  allocationFailed = allocResult.failed;
  clearCursor = (SpatialSoundSlot *)allocResult.payloadOrError;
  if (!allocationFailed) {
    g_SpatialSoundSlots = clearCursor;
    /* REP STOSD over 0x400 dwords: the cursor advances one dword (to the next field) per step */
    for (dwordsRemaining = 0x400; dwordsRemaining != 0; dwordsRemaining--) {
      clearCursor->voiceSet = NULL;
      clearCursor = (SpatialSoundSlot *)&clearCursor->activeVoice;
    }
    allocationFailed = false;
  }
  return allocationFailed ? StatusValue_Fail(allocResult.payloadOrError) : StatusValue_Ok(0);
}


/* Address: 0x0050B600.
   Ownership: audio/spatial/runtime.
   Purpose: Handles spatial sound rebuild listener transform from pose.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_Compose
   [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_RebuildListenerTransformFromPose
          (AngleTurn32 viewAngle1,AngleTurn32 viewAngle0,GraphicsWorldCoordinateQ12 originZ,
          GraphicsWorldCoordinateQ12 originY,GraphicsWorldCoordinateQ12 originX)

{
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)THANDOR_ADDR(g_SpatialSoundListenerRotation,0),0x4000 - viewAngle0 & 0xffff,viewAngle1,0xc000);
  uRam0050b574 = 0;
  uRam0050b578 = 0;
  uRam0050b57c = 0;
  iRam0050b5a4 = -originX;
  iRam0050b5a8 = -originY;
  iRam0050b5ac = -originZ;
  uRam0050b580 = 0x10000000;
  uRam0050b584 = 0;
  uRam0050b588 = 0;
  uRam0050b58c = 0;
  uRam0050b590 = 0x10000000;
  uRam0050b594 = 0;
  uRam0050b598 = 0;
  uRam0050b59c = 0;
  uRam0050b5a0 = 0x10000000;
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform,
             (GraphicsFixedMatrix3x4 *)THANDOR_ADDR(g_SpatialSoundListenerWorldToLocal,0),(GraphicsFixedMatrix3x4 *)THANDOR_ADDR(g_SpatialSoundListenerRotation,0));
  return;
}


/* Address: 0x0050B6E0.
   Plays a sound effect once at a world position: the gain (scaled by the effects volume) fades out with
   the listener distance along a quarter cosine up to maximumDistanceQ12 and is panned by the azimuth
   around the listener (sides swapped with reverse stereo). Nothing plays when the voice set is missing,
   the position is out of range or the attenuated gain is not above SPATIAL_SOUND_MIN_AUDIBLE_GAIN_Q15.
*/
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_PlayPositionedOneShot
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef)

{
  int64_t scaledProduct;
  uint32_t distanceOrPannedGainQ15;
  uint32_t azimuthOrRightGainQ15;
  uint32_t volumeOrLeftGainQ15;
  FixedLengthAnglesEaxEcxEdx12 lengthAngles;

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
   Ownership: audio/spatial/runtime.
   Purpose: Computes the same positional attenuation and writes desired left/right Q15 gains into a persistent
   SpatialSoundSlot for the pool update pass. Four stack arguments are authoritative from RET 0x10; EAX/EDX are
   restored and do not carry a semantic return.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], FixedMath_VectorToAnglesAndLength3Regs
   [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_UpdateDesiredPositionedGains
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot)

{
  int64_t scaledProduct;
  uint32_t distanceOrPannedGain;
  uint32_t azimuthOrLeftGainQ15;
  uint32_t volumeOrRightGainQ15;
  FixedLengthAnglesEaxEcxEdx12 lengthAngles;
  
  volumeOrRightGainQ15 = gainQ15 * g_SoundEffectsGainQ15 >> 0xf;
  if ((slot != (SpatialSoundSlot *)0x0) && (volumeOrRightGainQ15 != 0)) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
               (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
    lengthAngles = FixedMath_VectorToAnglesAndLength3Regs
                      (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
    azimuthOrLeftGainQ15 = lengthAngles.azimuthAngle;
    distanceOrPannedGain = lengthAngles.lengthQ12;
    if ((distanceOrPannedGain < maximumDistanceQ12) &&
       (scaledProduct = (int64_t)
                g_FixedCosQ28
                [(int)(((uint64_t)distanceOrPannedGain << 0xe) / (uint64_t)maximumDistanceQ12)] *
                (int64_t)(int)volumeOrRightGainQ15,
       volumeOrRightGainQ15 = (int)((uint64_t)scaledProduct >> 0x20) << 4 | (uint32_t)scaledProduct >> 0x1c, 0x100 < (int)volumeOrRightGainQ15)) {
      if (azimuthOrLeftGainQ15 < 0x8000) {
        distanceOrPannedGain = (uint32_t)((uint64_t)
                       ((int64_t)(g_FixedCosQ28[azimuthOrLeftGainQ15 * 2] + 0x10000000) *
                       (int64_t)(int)(volumeOrRightGainQ15 << 3)) >> 0x20);
        azimuthOrLeftGainQ15 = volumeOrRightGainQ15;
      }
      else {
        scaledProduct = (int64_t)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + azimuthOrLeftGainQ15 * 8) + 0x10000000) *
                (int64_t)(int)volumeOrRightGainQ15;
        azimuthOrLeftGainQ15 = (uint32_t)scaledProduct >> 0x1d | (int)((uint64_t)scaledProduct >> 0x20) << 3;
        distanceOrPannedGain = volumeOrRightGainQ15;
      }
      volumeOrRightGainQ15 = distanceOrPannedGain;
      if (g_ReverseStereoMask != 0) {
        volumeOrRightGainQ15 = azimuthOrLeftGainQ15;
        azimuthOrLeftGainQ15 = distanceOrPannedGain;
      }
      if (0x8000 < (int)azimuthOrLeftGainQ15) {
        azimuthOrLeftGainQ15 = 0x8000;
      }
      if (0x8000 < (int)volumeOrRightGainQ15) {
        volumeOrRightGainQ15 = 0x8000;
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
SpatialSoundSlotResult __thandor_eax_cf_preserve_ecx_edx
SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset)

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
   Ownership: audio/spatial/runtime.
   Purpose: A full pool releases the voice set and returns error 0x14 with CF set.
*/
SpatialSoundSlot *
SpatialSoundSlot_CreateFromPcm
          (AudioBufferByteCount bufferByteCount,AudioSampleRateHz sampleRateHz,
          AudioBitsPerSampleStack32 bitsPerSample,AudioChannelCountStack32 channelCount,
          void *pcmData)

{
  SpatialSoundSlot *voiceSetOrError;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  PcmVoiceSetResult createResult;
  
  createResult = g_SoundCreatePcmVoiceSet
                    (bufferByteCount,sampleRateHz,bitsPerSample,channelCount,pcmData);
  voiceSetOrError = (SpatialSoundSlot *)createResult.voiceSet;
  if (!createResult.failed) {
    slotsRemaining = 0x100;
    slotCursor = g_SpatialSoundSlots;
    do {
      if (slotCursor->voiceSet == (DirectSoundVoiceSet *)0x0) {
        slotCursor->voiceSet = (DirectSoundVoiceSet *)voiceSetOrError;
        slotCursor->desiredLeftGainQ15 = 0;
        slotCursor->desiredRightGainQ15 = 0;
        slotCursor->activeVoice = (IDirectSoundBuffer *)0x0;
        return slotCursor;
      }
      slotCursor = slotCursor + 1;
      slotsRemaining = slotsRemaining + -1;
    } while (slotsRemaining != 0);
    g_SoundReleasePcmVoiceSet((DirectSoundVoiceSet *)voiceSetOrError);
    voiceSetOrError = (SpatialSoundSlot *)0x14;
  }
  return voiceSetOrError;
}


/* Address: 0x0050B9D0.
   Releases the sample voice set of a slot from SpatialSoundSlot_CreateFromSampleAsset and clears the slot
   (all four dwords), which makes it free again. A NULL slot is ignored.
*/
void __thandor_void_preserve_eax_ecx SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot)

{
  int slotEntriesRemaining;
  
  slotEntriesRemaining = 4;
  if (slot != NULL) {
    g_SoundReleaseSampleVoiceSet(slot->voiceSet);
    for (; slotEntriesRemaining != 0; slotEntriesRemaining--) {
      slot->voiceSet = (DirectSoundVoiceSet *)0x0;
      slot = (SpatialSoundSlot *)&slot->activeVoice;
    }
  }
  return;
}


/* Address: 0x0050BA00.
   Ownership: audio/spatial/runtime.
   Purpose: Releases the slot's PCM voice set through DirectSound_ReleasePcmVoiceSet and clears all four slot
   dwords. Null is accepted.
*/
void __thandor_void_preserve_eax_ecx SpatialSoundSlot_ReleasePcm(SpatialSoundSlot *slot)

{
  int slotEntriesRemaining;
  
  slotEntriesRemaining = 4;
  if (slot != (SpatialSoundSlot *)0x0) {
    g_SoundReleasePcmVoiceSet(slot->voiceSet);
    for (; slotEntriesRemaining != 0; slotEntriesRemaining = slotEntriesRemaining + -1) {
      slot->voiceSet = (DirectSoundVoiceSet *)0x0;
      slot = (SpatialSoundSlot *)&slot->activeVoice;
    }
  }
  return;
}


/* Address: 0x0050BA30.
   Start of a frame's positioned-sound pass: sets the desired gains of every used slot to 0, so that only the
   sounds whose gains are set again this frame keep playing when SpatialSoundPool_ApplyDesiredGains runs.
*/
void __thandor_void_preserve_eax_ecx SpatialSoundPool_ClearDesiredGains(void)

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
void __thandor_void_preserve_eax_ecx_edx SpatialSoundPool_ApplyDesiredGains(void)

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

