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
   Ownership: audio/spatial/runtime.
   Purpose: Allocates and zeroes 0x1000 bytes, exactly 256 SpatialSoundSlot records. CF reports allocation failure.
*/
StatusValueEaxCf5 SpatialSoundPool_Init(void)

{
  SpatialSoundSlot *spatialSoundStorageCursor;
  int allocationDwordsRemaining;
  bool allocationFailed;
  ArenaAllocEaxCf5 allocResult;
  
  allocResult = (*g_MemoryApi.alloc)(0x1000);
  allocationFailed = allocResult.carry;
  spatialSoundStorageCursor = (SpatialSoundSlot *)allocResult.eax;
  if (!allocationFailed) {
    g_SpatialSoundSlots = spatialSoundStorageCursor;
    for (allocationDwordsRemaining = 0x400; allocationDwordsRemaining != 0;
        allocationDwordsRemaining = allocationDwordsRemaining + -1) {
      spatialSoundStorageCursor->voiceSet = (DirectSoundVoiceSet *)0x0;
      spatialSoundStorageCursor = (SpatialSoundSlot *)&spatialSoundStorageCursor->activeVoice;
    }
    allocationFailed = false;
  }
  return allocationFailed ? StatusValue_Fail(allocResult.eax) : StatusValue_Ok(0);
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
   Ownership: audio/spatial/runtime.
   Purpose: Transforms a world position into listener space, applies distance attenuation and stereo panning,
   respects reverse-stereo state, clamps Q15 gains, and starts a one-shot voice.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], FixedMath_VectorToAnglesAndLength3Regs
   [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
SpatialSound_PlayPositionedOneShot
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef)

{
  longlong scaledProduct;
  uint distanceOrPannedGain;
  uint azimuthOrLeftGainQ15;
  uint volumeOrRightGainQ15;
  FixedLengthAnglesEaxEcxEdx12 lengthAngles;
  
  volumeOrRightGainQ15 = gainQ15 * g_SoundEffectsGainQ15 >> 0xf;
  if ((voiceSetRef != (DirectSoundVoiceSet **)0x0) && (volumeOrRightGainQ15 != 0)) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
               (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
    lengthAngles = FixedMath_VectorToAnglesAndLength3Regs
                      (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
    azimuthOrLeftGainQ15 = lengthAngles.azimuthAngle;
    distanceOrPannedGain = lengthAngles.lengthQ12;
    if ((distanceOrPannedGain < maximumDistanceQ12) &&
       (scaledProduct = (longlong)
                g_FixedCosQ28
                [(int)(CONCAT44(distanceOrPannedGain >> 0x12,distanceOrPannedGain << 0xe) / (ulonglong)maximumDistanceQ12)] *
                (longlong)(int)volumeOrRightGainQ15,
       volumeOrRightGainQ15 = (int)((ulonglong)scaledProduct >> 0x20) << 4 | (uint)scaledProduct >> 0x1c, 0x100 < (int)volumeOrRightGainQ15)) {
      if (azimuthOrLeftGainQ15 < 0x8000) {
        distanceOrPannedGain = (uint)((ulonglong)
                       ((longlong)(g_FixedCosQ28[azimuthOrLeftGainQ15 * 2] + 0x10000000) *
                       (longlong)(int)(volumeOrRightGainQ15 << 3)) >> 0x20);
        azimuthOrLeftGainQ15 = volumeOrRightGainQ15;
      }
      else {
        scaledProduct = (longlong)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + azimuthOrLeftGainQ15 * 8) + 0x10000000) *
                (longlong)(int)volumeOrRightGainQ15;
        azimuthOrLeftGainQ15 = (uint)scaledProduct >> 0x1d | (int)((ulonglong)scaledProduct >> 0x20) << 3;
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
      (*g_SoundPlayOneShot)(volumeOrRightGainQ15,azimuthOrLeftGainQ15,*voiceSetRef);
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
  longlong scaledProduct;
  uint distanceOrPannedGain;
  uint azimuthOrLeftGainQ15;
  uint volumeOrRightGainQ15;
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
       (scaledProduct = (longlong)
                g_FixedCosQ28
                [(int)(CONCAT44(distanceOrPannedGain >> 0x12,distanceOrPannedGain << 0xe) / (ulonglong)maximumDistanceQ12)] *
                (longlong)(int)volumeOrRightGainQ15,
       volumeOrRightGainQ15 = (int)((ulonglong)scaledProduct >> 0x20) << 4 | (uint)scaledProduct >> 0x1c, 0x100 < (int)volumeOrRightGainQ15)) {
      if (azimuthOrLeftGainQ15 < 0x8000) {
        distanceOrPannedGain = (uint)((ulonglong)
                       ((longlong)(g_FixedCosQ28[azimuthOrLeftGainQ15 * 2] + 0x10000000) *
                       (longlong)(int)(volumeOrRightGainQ15 << 3)) >> 0x20);
        azimuthOrLeftGainQ15 = volumeOrRightGainQ15;
      }
      else {
        scaledProduct = (longlong)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + azimuthOrLeftGainQ15 * 8) + 0x10000000) *
                (longlong)(int)volumeOrRightGainQ15;
        azimuthOrLeftGainQ15 = (uint)scaledProduct >> 0x1d | (int)((ulonglong)scaledProduct >> 0x20) << 3;
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
   Ownership: audio/spatial/runtime.
   Purpose: Creates a DirectSound sample voice set, claims the first free slot in the 256-entry spatial pool,
   stores the voice set, clears activeVoice and both desired gains, and returns the slot in EAX with CF clear. A
   full pool releases the new voice set and returns error 0x14 with CF set.
*/
SpatialSoundSlotEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset)

{
  SpatialSoundSlot *voiceSetOrError;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  SoundCreateSampleVoiceSetEaxCf5 createResult;
  SpatialSoundSlotEaxCf5 failureResult;
  SpatialSoundSlotEaxCf5 successResult;
  
  createResult = (*g_SoundCreateSampleVoiceSet)(sampleAsset);
  voiceSetOrError = (SpatialSoundSlot *)createResult.eax;
  if (!createResult.carry) {
    slotsRemaining = 0x100;
    slotCursor = g_SpatialSoundSlots;
    do {
      if (slotCursor->voiceSet == (DirectSoundVoiceSet *)0x0) {
        slotCursor->voiceSet = (DirectSoundVoiceSet *)voiceSetOrError;
        slotCursor->desiredLeftGainQ15 = 0;
        slotCursor->desiredRightGainQ15 = 0;
        slotCursor->activeVoice = (IDirectSoundBuffer *)0x0;
        successResult.carry = false;
        successResult.soundSlot = slotCursor;
        return successResult;
      }
      slotCursor = slotCursor + 1;
      slotsRemaining = slotsRemaining + -1;
    } while (slotsRemaining != 0);
    (*g_SoundReleaseSampleVoiceSet)((DirectSoundVoiceSet *)voiceSetOrError);
    voiceSetOrError = (SpatialSoundSlot *)0x14;
  }
  failureResult.carry = true;
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
  SoundCreatePcmVoiceSetEaxCf5 createResult;
  
  createResult = (*g_SoundCreatePcmVoiceSet)
                    (bufferByteCount,sampleRateHz,bitsPerSample,channelCount,pcmData);
  voiceSetOrError = (SpatialSoundSlot *)createResult.eax;
  if (!createResult.carry) {
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
    (*g_SoundReleasePcmVoiceSet)((DirectSoundVoiceSet *)voiceSetOrError);
    voiceSetOrError = (SpatialSoundSlot *)0x14;
  }
  return voiceSetOrError;
}


/* Address: 0x0050B9D0.
   Ownership: audio/spatial/runtime.
   Purpose: Releases the slot's sample voice set through DirectSound_ReleaseSampleVoiceSet and clears all four slot
   dwords. Null is accepted.
*/
void __thandor_void_preserve_eax_ecx SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot)

{
  int slotEntriesRemaining;
  
  slotEntriesRemaining = 4;
  if (slot != (SpatialSoundSlot *)0x0) {
    (*g_SoundReleaseSampleVoiceSet)(slot->voiceSet);
    for (; slotEntriesRemaining != 0; slotEntriesRemaining = slotEntriesRemaining + -1) {
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
    (*g_SoundReleasePcmVoiceSet)(slot->voiceSet);
    for (; slotEntriesRemaining != 0; slotEntriesRemaining = slotEntriesRemaining + -1) {
      slot->voiceSet = (DirectSoundVoiceSet *)0x0;
      slot = (SpatialSoundSlot *)&slot->activeVoice;
    }
  }
  return;
}


/* Address: 0x0050BA30.
   Ownership: audio/spatial/runtime.
   Purpose: Clears desiredLeftGainQ15 and desiredRightGainQ15 in every occupied spatial-sound slot. activeVoice is
   left intact until SpatialSoundPool_ApplyDesiredGains processes the zero-gain request.
*/
void __thandor_void_preserve_eax_ecx SpatialSoundPool_ClearDesiredGains(void)

{
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  
  slotsRemaining = 0x100;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != (DirectSoundVoiceSet *)0x0) {
      slotCursor->desiredLeftGainQ15 = 0;
      slotCursor->desiredRightGainQ15 = 0;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return;
}


/* Address: 0x0050BA60.
   Ownership: audio/spatial/runtime.
   Purpose: Walks all 256 slots. Zero desired gains stop and clear an active voice. Nonzero desired gains update an
   active voice or start a one-shot voice from voiceSet and store the returned activeVoice pointer.
*/
void __thandor_void_preserve_eax_ecx_edx SpatialSoundPool_ApplyDesiredGains(void)

{
  IDirectSoundBuffer *existingVoice;
  IDirectSoundBuffer *activeVoice;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  SoundPlayVoiceEaxCf5 playResult;
  
  slotsRemaining = 0x100;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != (DirectSoundVoiceSet *)0x0) {
      existingVoice = slotCursor->activeVoice;
      if (existingVoice == (IDirectSoundBuffer *)0x0) {
        if (slotCursor->desiredLeftGainQ15 != 0 || slotCursor->desiredRightGainQ15 != 0) {
          playResult = (*g_SoundPlayLooping)
                            (slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,
                             slotCursor->voiceSet);
          activeVoice = playResult.eax;
          slotCursor->activeVoice = activeVoice;
        }
      }
      else if (slotCursor->desiredLeftGainQ15 == 0 && slotCursor->desiredRightGainQ15 == 0) {
        (*g_SoundStopVoice)(existingVoice);
        slotCursor->activeVoice = (IDirectSoundBuffer *)0x0;
      }
      else {
        (*g_SoundSetVoiceGains)(slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,existingVoice)
        ;
      }
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return;
}

