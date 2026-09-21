/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/codec/sam.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_CODEC_SAM_H
#define THANDOR_AUDIO_CODEC_SAM_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/codec/sam. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00417700 */
PreservedEaxEdxRegisterPair64 __fastcall
SoundCoefficientTransform_ApplyCosineBanksMmx
          (dword preservedIncomingEcx,dword preservedIncomingEdx,short *outputSamples,
          SoundCoefficientBlock256 *coefficientBlock);

/* 0x00418560 */
void __thandor_void_preserve_eax_ecx_edx
SoundSample_DecodeCoefficientBlockToPcmMmx(short *outputStereoPcm,short *coefficients);

/* 0x004193D0 */
void __thandor_void_preserve_eax_ecx_edx
SoundSample_DecodeCoefficientBlockToMonoPcmMmx(short *outputMonoPcm,short *coefficients);

/* 0x0041A430 */
dword __thandor_eax_preserve_ecx_edx
SoundSample_DecodePackedCoefficientBlock(short *outputCoefficients,byte *encodedBlock);


/* 0x0041A320 */
dword __thandor_eax_preserve_ecx_edx
SoundSample_EncodePackedCoefficientBlock(byte *encodedBlock,short *inputCoefficients);

#endif /* THANDOR_AUDIO_CODEC_SAM_H */
