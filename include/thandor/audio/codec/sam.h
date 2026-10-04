/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/audio/codec/sam.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_AUDIO_CODEC_SAM_H
#define THANDOR_AUDIO_CODEC_SAM_H

#include <thandor/audio/codec/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Submodule: audio/codec/sam. */

/* A .sam block: 256 transform coefficients <-> 256 PCM samples per channel. */
#define SAM_BLOCK_SAMPLE_COUNT 256
/* The MMX cosine transforms compute 4 outputs (one qword, four 256-entry cosine rows) per loop pass. */
#define SAM_MMX_OUTPUTS_PER_PASS 4

/* CosineDerivedLookupTables_Init: two 256x256 tables of shorts (the .sam codec's cosine transform) */
#define COSINE_DERIVED_TABLE_ORDER 256

#define COSINE_DERIVED_TABLE_ANGLE_STEP 0x40 /* pi/512 in angle16 units */

#define COSINE_DERIVED_INV_SQRT2_Q12 2896 /* 1/sqrt(2) in Q12: row 0 of the first table */

#define COSINE_DERIVED_INV_SQRT2_Q14 11585 /* 1/sqrt(2) in Q14: entry 0 of each row of the second table */

/* Two 32-bit MMX lanes as one qword (high lane in the upper half), as PUNPCKLDQ builds them. */
#define SAM_PACK_LANE_PAIR(highLane, lowLane) ((uint64_t)(uint32_t)(highLane) << 32 | (uint32_t)(lowLane))

/* Functions are grouped by semantic ownership. */

void SoundCoefficientTransform_ApplyCosineBanksMmx(short *outputMonoPcm,SoundCoefficientBlock *coefficientBlock);

void SoundSample_DecodeCoefficientBlockToPcmMmx(short *outputStereoPcm,short *coefficients);

uint32_t SoundSample_DecodePackedCoefficientBlock(short *outputCoefficients,uint8_t *encodedBlock);

/* Returns 0, or the allocation error (the tables stay unset; no sample may be decoded then). */
uint32_t __cdecl CosineDerivedLookupTables_Init();

/* the two cosine matrices of the .sam codec (CosineDerivedLookupTables_Init) */
extern short *g_CosineDerivedLookupAllocation;
extern short *g_CosineDerivedLookupSecondTable;
extern const uint64_t g_SoundDecodeMmxWordLaneMask0;
extern const uint64_t g_SoundDecodeMmxWordLaneMask1;
extern const uint64_t g_SoundDecodeMmxWordLaneMask2;
extern const uint64_t g_SoundDecodeMmxWordLaneMask3;

#endif /* THANDOR_AUDIO_CODEC_SAM_H */
