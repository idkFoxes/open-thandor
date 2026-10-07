/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/codec/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_CODEC_TYPES_H
#define THANDOR_AUDIO_CODEC_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */

struct SoundCoefficientBlock;

struct SoundCoefficientBlock {
    short coefficients[256]; 
};

using MmxPackedValue64 = uint64_t;

#endif /* THANDOR_AUDIO_CODEC_TYPES_H */
