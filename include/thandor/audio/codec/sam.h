#ifndef THANDOR_AUDIO_CODEC_SAM_H
#define THANDOR_AUDIO_CODEC_SAM_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/codec/sam. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00417700 */
PreservedEaxEdxRegisterPair64 __fastcall SoundCoefficientTransform_ApplyCosineBanksMmx (undefined4 preservedIncomingEcx,undefined4 preservedIncomingEdx,short *outputSamples, SoundCoefficientBlock256 *coefficientBlock);

/* 0x00418560 */
void SoundSample_DecodeCoefficientBlockToPcmMmx(short *outputStereoPcm,short *coefficients);

/* 0x004193D0 */
void SoundSample_DecodeCoefficientBlockToMonoPcmMmx(short *outputMonoPcm,short *coefficients);

/* 0x0041A430 */
dword SoundSample_DecodePackedCoefficientBlock(short *outputCoefficients,byte *encodedBlock);

#endif /* THANDOR_AUDIO_CODEC_SAM_H */
