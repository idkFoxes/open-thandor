/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/codec/sam_encoder.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_CODEC_SAM_ENCODER_H
#define THANDOR_AUDIO_CODEC_SAM_ENCODER_H

#include <thandor/core/contracts.h>

void SoundSample_TransformPcmBlockToCoefficientsMmx(short *outputCoefficients,short *inputPcm);

uint32_t SoundSample_EncodePackedCoefficientBlock(uint8_t *encodedBlock,short *inputCoefficients);

#endif /* THANDOR_AUDIO_CODEC_SAM_ENCODER_H */
