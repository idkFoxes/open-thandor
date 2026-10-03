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
/* Functions are grouped by semantic ownership. */

/* A .sam block: 256 transform coefficients <-> 256 PCM samples per channel. */
#define SAM_BLOCK_SAMPLE_COUNT 256
/* The MMX cosine transforms compute 4 outputs (one qword, four 256-entry cosine rows) per loop pass. */
#define SAM_MMX_OUTPUTS_PER_PASS 4

void SoundCoefficientTransform_ApplyCosineBanksMmx(short *outputMonoPcm,SoundCoefficientBlock *coefficientBlock);

void SoundSample_DecodeCoefficientBlockToPcmMmx(short *outputStereoPcm,short *coefficients);

void SoundSample_TransformPcmBlockToCoefficientsMmx(short *outputCoefficients,short *inputPcm);

uint32_t SoundSample_DecodePackedCoefficientBlock(short *outputCoefficients,uint8_t *encodedBlock);


uint32_t SoundSample_EncodePackedCoefficientBlock(uint8_t *encodedBlock,short *inputCoefficients);

#endif /* THANDOR_AUDIO_CODEC_SAM_H */
