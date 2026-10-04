/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/codec/sam_encoder.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_CODEC_SAM_ENCODER_H
#define THANDOR_AUDIO_CODEC_SAM_ENCODER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/codec/sam_encoder. */

/* Functions are grouped by semantic ownership. */

void SoundSample_TransformPcmBlockToCoefficientsMmx(short *outputCoefficients,short *inputPcm);

uint32_t SoundSample_EncodePackedCoefficientBlock(uint8_t *encodedBlock,short *inputCoefficients);

#endif /* THANDOR_AUDIO_CODEC_SAM_ENCODER_H */
