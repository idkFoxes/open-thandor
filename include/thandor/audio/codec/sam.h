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

/* A .sam block: 256 transform coefficients <-> 256 PCM samples per channel. */
#define SAM_BLOCK_SAMPLE_COUNT 256
/* The MMX cosine transforms compute 4 outputs (one qword, four 256-entry cosine rows) per loop pass. */
#define SAM_MMX_OUTPUTS_PER_PASS 4

/* 0x00417700 */
PreservedEaxEdxRegisterPair64 __fastcall
SoundCoefficientTransform_ApplyCosineBanksMmx
          (uint32_t preservedIncomingEcx,uint32_t preservedIncomingEdx,short *outputMonoPcm,
          SoundCoefficientBlock256 *coefficientBlock);

/* 0x00418560 */
void __thandor_void_preserve_eax_ecx_edx
SoundSample_DecodeCoefficientBlockToPcmMmx(short *outputStereoPcm,short *coefficients);

/* 0x004193D0 */
void __thandor_void_preserve_eax_ecx_edx
SoundSample_TransformPcmBlockToCoefficientsMmx(short *outputCoefficients,short *inputPcm);

/* 0x0041A430 */
uint32_t __thandor_eax_preserve_ecx_edx
SoundSample_DecodePackedCoefficientBlock(short *outputCoefficients,uint8_t *encodedBlock);


/* 0x0041A320 */
uint32_t __thandor_eax_preserve_ecx_edx
SoundSample_EncodePackedCoefficientBlock(uint8_t *encodedBlock,short *inputCoefficients);

#endif /* THANDOR_AUDIO_CODEC_SAM_H */
