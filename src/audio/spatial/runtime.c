#include <thandor/audio/spatial/runtime.h>

/* Implementation ownership: audio/spatial/runtime. */

/* Address: 0x0050B5D0.
   Ownership: audio/spatial/runtime.
   Purpose: Allocates and zeroes 0x1000 bytes, exactly 256 SpatialSoundSlot records. CF reports allocation failure.
*/
void SpatialSoundPool_Init(void)

{
  SpatialSoundSlot *spatialSoundStorageCursor;
  int allocationDwordsRemaining;
  undefined1 in_CF;
  
  spatialSoundStorageCursor = (*g_MemoryApi.alloc)(0x1000);
  if (!(bool)in_CF) {
    g_SpatialSoundSlots = spatialSoundStorageCursor;
    for (allocationDwordsRemaining = 0x400; allocationDwordsRemaining != 0;
        allocationDwordsRemaining = allocationDwordsRemaining + -1) {
      spatialSoundStorageCursor->voiceSet = (DirectSoundVoiceSet *)0x0;
      spatialSoundStorageCursor = (SpatialSoundSlot *)&spatialSoundStorageCursor->activeVoice;
    }
  }
  return;
}

/* Address: 0x0050B600.
   Ownership: audio/spatial/runtime.
   Purpose: Handles spatial sound rebuild listener transform from pose.
   Cross-module calls: FixedTransform_BuildRotationBasis [core/math/fixed], FixedTransform_Compose
   [core/math/fixed].
*/
undefined8
SpatialSound_RebuildListenerTransformFromPose
          (AngleTurn32 param_1,int param_2,int param_3,int param_4,int param_5)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)0x50b550,0x4000U - param_2 & 0xffff,param_1,0xc000);
  uRam0050b574 = 0;
  uRam0050b578 = 0;
  uRam0050b57c = 0;
  iRam0050b5a4 = -param_5;
  iRam0050b5a8 = -param_4;
  iRam0050b5ac = -param_3;
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
             (GraphicsFixedMatrix3x4 *)0x50b580,(GraphicsFixedMatrix3x4 *)0x50b550);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0050B6E0.
   Ownership: audio/spatial/runtime.
   Purpose: Transforms a world position into listener space, applies distance attenuation and stereo panning,
   respects reverse-stereo state, clamps Q15 gains, and starts a one-shot voice.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], FixedMath_VectorToAnglesAndLength3Regs
   [core/math/fixed].
*/
undefined8
SpatialSound_PlayPositionedOneShot
          (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
          GraphicsFixedVec3 *worldPosition,DirectSoundVoiceSet **voiceSetRef)

{
  longlong lVar1;
  undefined4 in_EAX;
  uint uVar2;
  uint extraout_ECX;
  undefined4 in_EDX;
  uint arg1;
  uint uVar3;
  FixedLengthElevationEdxEax8 FVar4;
  
  uVar3 = gainQ15 * g_SoundEffectsGainQ15 >> 0xf;
  if ((voiceSetRef != (DirectSoundVoiceSet **)0x0) && (uVar3 != 0)) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
               (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
    FVar4 = FixedMath_VectorToAnglesAndLength3Regs
                      (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
    uVar2 = (uint)FVar4;
    if ((uVar2 < maximumDistanceQ12) &&
       (lVar1 = (longlong)
                g_FixedCosQ28
                [(int)(CONCAT44(uVar2 >> 0x12,uVar2 << 0xe) / (ulonglong)maximumDistanceQ12)] *
                (longlong)(int)uVar3,
       uVar3 = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c, 0x100 < (int)uVar3)) {
      if (extraout_ECX < 0x8000) {
        uVar2 = (uint)((ulonglong)
                       ((longlong)(g_FixedCosQ28[extraout_ECX * 2] + 0x10000000) *
                       (longlong)(int)(uVar3 << 3)) >> 0x20);
        arg1 = uVar3;
      }
      else {
        lVar1 = (longlong)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + extraout_ECX * 8) +
                0x10000000) * (longlong)(int)uVar3;
        arg1 = (uint)lVar1 >> 0x1d | (int)((ulonglong)lVar1 >> 0x20) << 3;
        uVar2 = uVar3;
      }
      uVar3 = uVar2;
      if (g_ReverseStereoMask != 0) {
        uVar3 = arg1;
        arg1 = uVar2;
      }
      if (0x8000 < (int)arg1) {
        arg1 = 0x8000;
      }
      if (0x8000 < (int)uVar3) {
        uVar3 = 0x8000;
      }
      (*g_SoundPlayOneShot)(uVar3,arg1,*voiceSetRef);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0050B7D0.
   Ownership: audio/spatial/runtime.
   Purpose: Computes the same positional attenuation and writes desired left/right Q15 gains into a persistent
   SpatialSoundSlot for the pool update pass. Four stack arguments are authoritative from RET 0x10; EAX/EDX are
   restored and do not carry a semantic return.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], FixedMath_VectorToAnglesAndLength3Regs
   [core/math/fixed].
*/
void SpatialSound_UpdateDesiredPositionedGains
               (SpatialSoundMaximumDistanceQ12 maximumDistanceQ12,SpatialSoundGainQ15 gainQ15,
               GraphicsFixedVec3 *worldPosition,SpatialSoundSlot *slot)

{
  longlong lVar1;
  uint uVar2;
  uint extraout_ECX;
  uint uVar3;
  uint uVar4;
  FixedLengthElevationEdxEax8 FVar5;
  
  uVar4 = gainQ15 * g_SoundEffectsGainQ15 >> 0xf;
  if ((slot != (SpatialSoundSlot *)0x0) && (uVar4 != 0)) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_SpatialSoundRelativeX,worldPosition,
               (GraphicsFixedMatrix3x4 *)&g_SpatialSoundListenerTransform);
    FVar5 = FixedMath_VectorToAnglesAndLength3Regs
                      (g_SpatialSoundRelativeY,g_SpatialSoundRelativeX,g_SpatialSoundRelativeZ);
    uVar2 = (uint)FVar5;
    if ((uVar2 < maximumDistanceQ12) &&
       (lVar1 = (longlong)
                g_FixedCosQ28
                [(int)(CONCAT44(uVar2 >> 0x12,uVar2 << 0xe) / (ulonglong)maximumDistanceQ12)] *
                (longlong)(int)uVar4,
       uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c, 0x100 < (int)uVar4)) {
      if (extraout_ECX < 0x8000) {
        uVar2 = (uint)((ulonglong)
                       ((longlong)(g_FixedCosQ28[extraout_ECX * 2] + 0x10000000) *
                       (longlong)(int)(uVar4 << 3)) >> 0x20);
        uVar3 = uVar4;
      }
      else {
        lVar1 = (longlong)
                (*(int *)(&k_SpatialSoundStereoCosineSecondHalfBaseBias + extraout_ECX * 8) +
                0x10000000) * (longlong)(int)uVar4;
        uVar3 = (uint)lVar1 >> 0x1d | (int)((ulonglong)lVar1 >> 0x20) << 3;
        uVar2 = uVar4;
      }
      uVar4 = uVar2;
      if (g_ReverseStereoMask != 0) {
        uVar4 = uVar3;
        uVar3 = uVar2;
      }
      if (0x8000 < (int)uVar3) {
        uVar3 = 0x8000;
      }
      if (0x8000 < (int)uVar4) {
        uVar4 = 0x8000;
      }
      slot->desiredLeftGainQ15 = uVar3;
      slot->desiredRightGainQ15 = uVar4;
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
SpatialSoundSlot * SpatialSoundSlot_CreateFromSampleAsset(SoundSampleAsset *sampleAsset)

{
  SpatialSoundSlot *arg0;
  int slotsRemaining;
  SpatialSoundSlot *slotCursor;
  undefined1 in_CF;
  
  arg0 = (SpatialSoundSlot *)(*g_SoundCreateSampleVoiceSet)(sampleAsset);
  if (!(bool)in_CF) {
    slotsRemaining = 0x100;
    slotCursor = g_SpatialSoundSlots;
    do {
      if (slotCursor->voiceSet == (DirectSoundVoiceSet *)0x0) {
        slotCursor->voiceSet = (DirectSoundVoiceSet *)arg0;
        slotCursor->desiredLeftGainQ15 = 0;
        slotCursor->desiredRightGainQ15 = 0;
        slotCursor->activeVoice = (IDirectSoundBuffer *)0x0;
        return slotCursor;
      }
      slotCursor = slotCursor + 1;
      slotsRemaining = slotsRemaining + -1;
    } while (slotsRemaining != 0);
    (*g_SoundReleaseSampleVoiceSet)((DirectSoundVoiceSet *)arg0);
    arg0 = (SpatialSoundSlot *)0x14;
  }
  return arg0;
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
  SpatialSoundSlot *arg0;
  int iVar1;
  SpatialSoundSlot *pSVar2;
  undefined1 in_CF;
  
  arg0 = (SpatialSoundSlot *)
         (*g_SoundCreatePcmVoiceSet)
                   (bufferByteCount,sampleRateHz,bitsPerSample,channelCount,pcmData);
  if (!(bool)in_CF) {
    iVar1 = 0x100;
    pSVar2 = g_SpatialSoundSlots;
    do {
      if (pSVar2->voiceSet == (DirectSoundVoiceSet *)0x0) {
        pSVar2->voiceSet = (DirectSoundVoiceSet *)arg0;
        pSVar2->desiredLeftGainQ15 = 0;
        pSVar2->desiredRightGainQ15 = 0;
        pSVar2->activeVoice = (IDirectSoundBuffer *)0x0;
        return pSVar2;
      }
      pSVar2 = pSVar2 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    (*g_SoundReleasePcmVoiceSet)((DirectSoundVoiceSet *)arg0);
    arg0 = (SpatialSoundSlot *)0x14;
  }
  return arg0;
}

/* Address: 0x0050B9D0.
   Ownership: audio/spatial/runtime.
   Purpose: Releases the slot's sample voice set through DirectSound_ReleaseSampleVoiceSet and clears all four slot
   dwords. Null is accepted.
*/
void SpatialSoundSlot_ReleaseSample(SpatialSoundSlot *slot)

{
  int extraout_ECX;
  int slotEntriesRemaining;
  
  if (slot != (SpatialSoundSlot *)0x0) {
    (*g_SoundReleaseSampleVoiceSet)(slot->voiceSet);
    for (slotEntriesRemaining = extraout_ECX; slotEntriesRemaining != 0;
        slotEntriesRemaining = slotEntriesRemaining + -1) {
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
void SpatialSoundSlot_ReleasePcm(SpatialSoundSlot *slot)

{
  int extraout_ECX;
  int slotEntriesRemaining;
  
  if (slot != (SpatialSoundSlot *)0x0) {
    (*g_SoundReleasePcmVoiceSet)(slot->voiceSet);
    for (slotEntriesRemaining = extraout_ECX; slotEntriesRemaining != 0;
        slotEntriesRemaining = slotEntriesRemaining + -1) {
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
void SpatialSoundPool_ClearDesiredGains(void)

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
void SpatialSoundPool_ApplyDesiredGains(void)

{
  IDirectSoundBuffer *arg2;
  IDirectSoundBuffer *activeVoice;
  int slotsRemaining;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  SpatialSoundSlot *slotCursor;
  
  slotsRemaining = 0x100;
  slotCursor = g_SpatialSoundSlots;
  do {
    if (slotCursor->voiceSet != (DirectSoundVoiceSet *)0x0) {
      arg2 = slotCursor->activeVoice;
      if (arg2 == (IDirectSoundBuffer *)0x0) {
        if (slotCursor->desiredLeftGainQ15 != 0 || slotCursor->desiredRightGainQ15 != 0) {
          activeVoice = (*g_SoundPlayLooping)
                                  (slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,
                                   slotCursor->voiceSet);
          slotCursor->activeVoice = activeVoice;
          slotsRemaining = extraout_ECX_01;
        }
      }
      else if (slotCursor->desiredLeftGainQ15 == 0 && slotCursor->desiredRightGainQ15 == 0) {
        (*g_SoundStopVoice)(arg2);
        slotCursor->activeVoice = (IDirectSoundBuffer *)0x0;
        slotsRemaining = extraout_ECX;
      }
      else {
        (*g_SoundSetVoiceGains)(slotCursor->desiredRightGainQ15,slotCursor->desiredLeftGainQ15,arg2)
        ;
        slotsRemaining = extraout_ECX_00;
      }
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return;
}
