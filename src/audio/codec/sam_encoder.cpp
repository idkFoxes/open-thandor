/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/codec/sam_encoder.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/codec/sam_encoder.h>
#include <thandor/thandor.h>

/* Implementation ownership: audio/codec/sam_encoder. */

/* Forward cosine transform of the .sam codec (the encoder side; the decoders run the transposed matrix in the
   other direction): turns 256 mono 16-bit PCM samples into 256 coefficients,
   coefficient u = sum over k of (sample[k] >> 4) * cos((2k+1) * u * pi / 512) with g_CosineDerivedLookupAllocation
   (Q12), bits 16..31 of the wrapping 32-bit sum, >> 3. Nothing in the executable calls it or stores its
   address in a function-pointer table.
*/
void SoundSample_TransformPcmBlockToCoefficientsMmx(short *outputCoefficients,short *inputPcm)

{
  short *cosineRowCursor;
  MmxPackedValue64 mm0PackedValue0;
  MmxPackedValue64 mm0PackedValue1;
  MmxPackedValue64 mm0PackedValue2;
  MmxPackedValue64 mm0PackedValue3;
  MmxPackedValue64 mm0PackedValue34;
  MmxPackedValue64 mm0PackedValue4;
  MmxPackedValue64 mm0PackedValue35;
  MmxPackedValue64 mm0PackedValue5;
  MmxPackedValue64 mm0PackedValue36;
  MmxPackedValue64 mm0PackedValue6;
  MmxPackedValue64 mm0PackedValue37;
  MmxPackedValue64 mm0PackedValue7;
  MmxPackedValue64 mm0PackedValue38;
  MmxPackedValue64 mm0PackedValue8;
  MmxPackedValue64 mm0PackedValue39;
  MmxPackedValue64 mm0PackedValue9;
  MmxPackedValue64 mm0PackedValue40;
  MmxPackedValue64 mm0PackedValue10;
  MmxPackedValue64 mm0PackedValue41;
  MmxPackedValue64 mm0PackedValue11;
  MmxPackedValue64 mm0PackedValue42;
  MmxPackedValue64 mm0PackedValue12;
  MmxPackedValue64 mm0PackedValue43;
  MmxPackedValue64 mm0PackedValue13;
  MmxPackedValue64 mm0PackedValue44;
  MmxPackedValue64 mm0PackedValue14;
  MmxPackedValue64 mm0PackedValue45;
  MmxPackedValue64 mm0PackedValue15;
  MmxPackedValue64 mm0PackedValue46;
  MmxPackedValue64 mm0PackedValue16;
  MmxPackedValue64 mm0PackedValue47;
  MmxPackedValue64 mm0PackedValue17;
  MmxPackedValue64 mm0PackedValue48;
  MmxPackedValue64 mm0PackedValue18;
  MmxPackedValue64 mm0PackedValue49;
  MmxPackedValue64 mm0PackedValue19;
  MmxPackedValue64 mm0PackedValue50;
  MmxPackedValue64 mm0PackedValue20;
  MmxPackedValue64 mm0PackedValue51;
  MmxPackedValue64 mm0PackedValue21;
  MmxPackedValue64 mm0PackedValue52;
  MmxPackedValue64 mm0PackedValue22;
  MmxPackedValue64 mm0PackedValue53;
  MmxPackedValue64 mm0PackedValue23;
  MmxPackedValue64 mm0PackedValue54;
  MmxPackedValue64 mm0PackedValue24;
  MmxPackedValue64 mm0PackedValue55;
  MmxPackedValue64 mm0PackedValue25;
  MmxPackedValue64 mm0PackedValue56;
  MmxPackedValue64 mm0PackedValue26;
  MmxPackedValue64 mm0PackedValue57;
  MmxPackedValue64 mm0PackedValue27;
  MmxPackedValue64 mm0PackedValue58;
  MmxPackedValue64 mm0PackedValue28;
  MmxPackedValue64 mm0PackedValue59;
  MmxPackedValue64 mm0PackedValue29;
  MmxPackedValue64 mm0PackedValue60;
  MmxPackedValue64 mm0PackedValue30;
  MmxPackedValue64 mm0PackedValue61;
  MmxPackedValue64 mm0PackedValue31;
  MmxPackedValue64 mm0PackedValue62;
  MmxPackedValue64 mm0PackedValue32;
  MmxPackedValue64 mm0PackedValue63;
  MmxPackedValue64 mm0PackedValue33;
  MmxPackedValue64 mm0PackedValue64;
  MmxPackedValue64 mm1PackedValue0;
  MmxPackedValue64 mm1PackedValue1;
  MmxPackedValue64 mm1PackedValue2;
  MmxPackedValue64 mm1PackedValue3;
  MmxPackedValue64 mm1PackedValue4;
  MmxPackedValue64 mm1PackedValue5;
  MmxPackedValue64 mm1PackedValue6;
  MmxPackedValue64 mm1PackedValue7;
  MmxPackedValue64 mm1PackedValue8;
  MmxPackedValue64 mm1PackedValue9;
  MmxPackedValue64 mm1PackedValue10;
  MmxPackedValue64 mm1PackedValue11;
  MmxPackedValue64 mm1PackedValue12;
  MmxPackedValue64 mm1PackedValue13;
  MmxPackedValue64 mm1PackedValue14;
  MmxPackedValue64 mm1PackedValue15;
  MmxPackedValue64 mm1PackedValue16;
  MmxPackedValue64 mm1PackedValue17;
  MmxPackedValue64 mm1PackedValue18;
  MmxPackedValue64 mm1PackedValue19;
  MmxPackedValue64 mm1PackedValue20;
  MmxPackedValue64 mm1PackedValue21;
  MmxPackedValue64 mm1PackedValue22;
  MmxPackedValue64 mm1PackedValue23;
  MmxPackedValue64 mm1PackedValue24;
  MmxPackedValue64 mm1PackedValue25;
  MmxPackedValue64 mm1PackedValue26;
  MmxPackedValue64 mm1PackedValue27;
  MmxPackedValue64 mm1PackedValue28;
  MmxPackedValue64 mm1PackedValue29;
  MmxPackedValue64 mm1PackedValue30;
  MmxPackedValue64 mm1PackedValue31;
  MmxPackedValue64 mm1PackedValue32;
  MmxPackedValue64 mm1PackedValue33;
  MmxPackedValue64 mm1PackedValue34;
  MmxPackedValue64 mm1PackedValue35;
  MmxPackedValue64 mm1PackedValue36;
  MmxPackedValue64 mm1PackedValue37;
  MmxPackedValue64 mm1PackedValue38;
  MmxPackedValue64 mm1PackedValue39;
  MmxPackedValue64 mm1PackedValue40;
  MmxPackedValue64 mm1PackedValue41;
  MmxPackedValue64 mm1PackedValue42;
  MmxPackedValue64 mm1PackedValue43;
  MmxPackedValue64 mm1PackedValue44;
  MmxPackedValue64 mm1PackedValue45;
  MmxPackedValue64 mm1PackedValue46;
  MmxPackedValue64 mm1PackedValue47;
  MmxPackedValue64 mm1PackedValue48;
  MmxPackedValue64 mm1PackedValue49;
  MmxPackedValue64 mm1PackedValue50;
  MmxPackedValue64 mm1PackedValue51;
  MmxPackedValue64 mm1PackedValue52;
  MmxPackedValue64 mm1PackedValue53;
  MmxPackedValue64 mm1PackedValue54;
  MmxPackedValue64 mm1PackedValue55;
  MmxPackedValue64 mm1PackedValue56;
  MmxPackedValue64 mm1PackedValue57;
  MmxPackedValue64 mm1PackedValue58;
  MmxPackedValue64 mm1PackedValue59;
  MmxPackedValue64 mm1PackedValue60;
  MmxPackedValue64 mm1PackedValue61;
  MmxPackedValue64 mm1PackedValue62;
  MmxPackedValue64 mm1PackedValue63;
  MmxPackedValue64 mm2PackedValue0;
  MmxPackedValue64 mm2PackedValue1;
  MmxPackedValue64 mm2PackedValue2;
  MmxPackedValue64 mm2PackedValue3;
  MmxPackedValue64 mm2PackedValue4;
  MmxPackedValue64 mm2PackedValue5;
  MmxPackedValue64 mm2PackedValue6;
  MmxPackedValue64 mm2PackedValue7;
  MmxPackedValue64 mm2PackedValue8;
  MmxPackedValue64 mm2PackedValue9;
  MmxPackedValue64 mm2PackedValue10;
  MmxPackedValue64 mm2PackedValue11;
  MmxPackedValue64 mm2PackedValue12;
  MmxPackedValue64 mm2PackedValue13;
  MmxPackedValue64 mm2PackedValue14;
  MmxPackedValue64 mm2PackedValue15;
  MmxPackedValue64 mm2PackedValue16;
  MmxPackedValue64 mm2PackedValue17;
  MmxPackedValue64 mm2PackedValue18;
  MmxPackedValue64 mm2PackedValue19;
  MmxPackedValue64 mm2PackedValue20;
  MmxPackedValue64 mm2PackedValue21;
  MmxPackedValue64 mm2PackedValue22;
  MmxPackedValue64 mm2PackedValue23;
  MmxPackedValue64 mm2PackedValue24;
  MmxPackedValue64 mm2PackedValue25;
  MmxPackedValue64 mm2PackedValue26;
  MmxPackedValue64 mm2PackedValue27;
  MmxPackedValue64 mm2PackedValue28;
  MmxPackedValue64 mm2PackedValue29;
  MmxPackedValue64 mm2PackedValue30;
  MmxPackedValue64 mm2PackedValue31;
  MmxPackedValue64 mm2PackedValue32;
  MmxPackedValue64 mm2PackedValue33;
  MmxPackedValue64 mm2PackedValue34;
  MmxPackedValue64 mm2PackedValue35;
  MmxPackedValue64 mm2PackedValue36;
  MmxPackedValue64 mm2PackedValue37;
  MmxPackedValue64 mm2PackedValue38;
  MmxPackedValue64 mm2PackedValue39;
  MmxPackedValue64 mm2PackedValue40;
  MmxPackedValue64 mm2PackedValue41;
  MmxPackedValue64 mm2PackedValue42;
  MmxPackedValue64 mm2PackedValue43;
  MmxPackedValue64 mm2PackedValue44;
  MmxPackedValue64 mm2PackedValue45;
  MmxPackedValue64 mm2PackedValue46;
  MmxPackedValue64 mm2PackedValue47;
  MmxPackedValue64 mm2PackedValue48;
  MmxPackedValue64 mm2PackedValue49;
  MmxPackedValue64 mm2PackedValue50;
  MmxPackedValue64 mm2PackedValue51;
  MmxPackedValue64 mm2PackedValue52;
  MmxPackedValue64 mm2PackedValue53;
  MmxPackedValue64 mm2PackedValue54;
  MmxPackedValue64 mm2PackedValue55;
  MmxPackedValue64 mm2PackedValue56;
  MmxPackedValue64 mm2PackedValue57;
  MmxPackedValue64 mm2PackedValue58;
  MmxPackedValue64 mm2PackedValue59;
  MmxPackedValue64 mm2PackedValue60;
  MmxPackedValue64 mm2PackedValue61;
  MmxPackedValue64 mm2PackedValue62;
  MmxPackedValue64 mm2PackedValue63;
  MmxPackedValue64 mm2PackedValue64;
  MmxPackedValue64 mm2PackedValue65;
  MmxPackedValue64 mm2PackedValue66;
  MmxPackedValue64 mm2PackedValue67;
  MmxPackedValue64 mm2PackedValue68;
  MmxPackedValue64 mm2PackedValue69;
  MmxPackedValue64 mm2PackedValue70;
  MmxPackedValue64 mm2PackedValue71;
  MmxPackedValue64 mm2PackedValue72;
  MmxPackedValue64 mm2PackedValue73;
  MmxPackedValue64 mm2PackedValue74;
  MmxPackedValue64 mm2PackedValue75;
  MmxPackedValue64 mm2PackedValue76;
  MmxPackedValue64 mm2PackedValue77;
  MmxPackedValue64 mm2PackedValue78;
  MmxPackedValue64 mm2PackedValue79;
  MmxPackedValue64 mm2PackedValue80;
  MmxPackedValue64 mm2PackedValue81;
  MmxPackedValue64 mm2PackedValue82;
  MmxPackedValue64 mm2PackedValue83;
  MmxPackedValue64 mm2PackedValue84;
  MmxPackedValue64 mm2PackedValue85;
  MmxPackedValue64 mm2PackedValue86;
  MmxPackedValue64 mm2PackedValue87;
  MmxPackedValue64 mm2PackedValue88;
  MmxPackedValue64 mm2PackedValue89;
  MmxPackedValue64 mm2PackedValue90;
  MmxPackedValue64 mm2PackedValue91;
  MmxPackedValue64 mm2PackedValue92;
  MmxPackedValue64 mm2PackedValue93;
  MmxPackedValue64 mm2PackedValue94;
  MmxPackedValue64 mm2PackedValue95;
  MmxPackedValue64 mm3PackedValue0;
  MmxPackedValue64 mm3PackedValue1;
  MmxPackedValue64 mm3PackedValue2;
  MmxPackedValue64 mm3PackedValue3;
  MmxPackedValue64 mm3PackedValue4;
  MmxPackedValue64 mm3PackedValue5;
  MmxPackedValue64 mm3PackedValue6;
  MmxPackedValue64 mm3PackedValue7;
  MmxPackedValue64 mm3PackedValue8;
  MmxPackedValue64 mm3PackedValue9;
  MmxPackedValue64 mm3PackedValue10;
  MmxPackedValue64 mm3PackedValue11;
  MmxPackedValue64 mm3PackedValue12;
  MmxPackedValue64 mm3PackedValue13;
  MmxPackedValue64 mm3PackedValue14;
  MmxPackedValue64 mm3PackedValue15;
  MmxPackedValue64 mm3PackedValue16;
  MmxPackedValue64 mm3PackedValue17;
  MmxPackedValue64 mm3PackedValue18;
  MmxPackedValue64 mm3PackedValue19;
  MmxPackedValue64 mm3PackedValue20;
  MmxPackedValue64 mm3PackedValue21;
  MmxPackedValue64 mm3PackedValue22;
  MmxPackedValue64 mm3PackedValue23;
  MmxPackedValue64 mm3PackedValue24;
  MmxPackedValue64 mm3PackedValue25;
  MmxPackedValue64 mm3PackedValue26;
  MmxPackedValue64 mm3PackedValue27;
  MmxPackedValue64 mm3PackedValue28;
  MmxPackedValue64 mm3PackedValue29;
  MmxPackedValue64 mm3PackedValue30;
  MmxPackedValue64 mm3PackedValue31;
  MmxPackedValue64 mm3PackedValue32;
  MmxPackedValue64 mm3PackedValue33;
  MmxPackedValue64 mm3PackedValue34;
  MmxPackedValue64 mm3PackedValue35;
  MmxPackedValue64 mm3PackedValue36;
  MmxPackedValue64 mm3PackedValue37;
  MmxPackedValue64 mm3PackedValue38;
  MmxPackedValue64 mm3PackedValue39;
  MmxPackedValue64 mm3PackedValue40;
  MmxPackedValue64 mm3PackedValue41;
  MmxPackedValue64 mm3PackedValue42;
  MmxPackedValue64 mm3PackedValue43;
  MmxPackedValue64 mm3PackedValue44;
  MmxPackedValue64 mm3PackedValue45;
  MmxPackedValue64 mm3PackedValue46;
  MmxPackedValue64 mm3PackedValue47;
  MmxPackedValue64 mm3PackedValue48;
  MmxPackedValue64 mm3PackedValue49;
  MmxPackedValue64 mm3PackedValue50;
  MmxPackedValue64 mm3PackedValue51;
  MmxPackedValue64 mm3PackedValue52;
  MmxPackedValue64 mm3PackedValue53;
  MmxPackedValue64 mm3PackedValue54;
  MmxPackedValue64 mm3PackedValue55;
  MmxPackedValue64 mm3PackedValue56;
  MmxPackedValue64 mm3PackedValue57;
  MmxPackedValue64 mm3PackedValue58;
  MmxPackedValue64 mm3PackedValue59;
  MmxPackedValue64 mm3PackedValue60;
  MmxPackedValue64 mm3PackedValue61;
  MmxPackedValue64 mm3PackedValue62;
  MmxPackedValue64 mm3PackedValue63;
  MmxPackedValue64 mm3PackedValue64;
  MmxPackedValue64 mm3PackedValue65;
  MmxPackedValue64 mm3PackedValue66;
  MmxPackedValue64 mm3PackedValue67;
  MmxPackedValue64 mm3PackedValue68;
  MmxPackedValue64 mm3PackedValue69;
  MmxPackedValue64 mm3PackedValue70;
  MmxPackedValue64 mm3PackedValue71;
  MmxPackedValue64 mm3PackedValue72;
  MmxPackedValue64 mm3PackedValue73;
  MmxPackedValue64 mm3PackedValue74;
  MmxPackedValue64 mm3PackedValue75;
  MmxPackedValue64 mm3PackedValue76;
  MmxPackedValue64 mm3PackedValue77;
  MmxPackedValue64 mm3PackedValue78;
  MmxPackedValue64 mm3PackedValue79;
  MmxPackedValue64 mm3PackedValue80;
  MmxPackedValue64 mm3PackedValue81;
  MmxPackedValue64 mm3PackedValue82;
  MmxPackedValue64 mm3PackedValue83;
  MmxPackedValue64 mm3PackedValue84;
  MmxPackedValue64 mm3PackedValue85;
  MmxPackedValue64 mm3PackedValue86;
  MmxPackedValue64 mm3PackedValue87;
  MmxPackedValue64 mm3PackedValue88;
  MmxPackedValue64 mm3PackedValue89;
  MmxPackedValue64 mm3PackedValue90;
  MmxPackedValue64 mm3PackedValue91;
  MmxPackedValue64 mm3PackedValue92;
  MmxPackedValue64 mm3PackedValue93;
  MmxPackedValue64 mm3PackedValue94;
  MmxPackedValue64 mm3PackedValue95;
  int bank0HighLaneSum;
  int bank1HighLaneSum;
  int bank2LowLaneSum;
  int bank3LowLaneSum;
  int outputPassesRemaining;
  
  outputPassesRemaining = SAM_BLOCK_SAMPLE_COUNT / SAM_MMX_OUTPUTS_PER_PASS;
  cosineRowCursor = g_CosineDerivedLookupAllocation;
  do {
    /* One pass = coefficients u..u+3: all 256 samples, 8 at a time and pre-scaled >> 4 for headroom, are
       multiplied pairwise (PMADDWD) with the cosine rows at cosineRowCursor + 0, + 0x100, + 0x200 and + 0x300. */
    mm0PackedValue0 = psraw(*(MmxPackedValue64 *)inputPcm,4);
    mm1PackedValue0 = psraw(*(MmxPackedValue64 *)(inputPcm + 4),4);
    mm2PackedValue0 = pmaddwd(mm0PackedValue0,*(MmxPackedValue64 *)cosineRowCursor);
    mm3PackedValue0 = pmaddwd(mm0PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 0x100));
    mm2PackedValue1 = pmaddwd(mm1PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 4));
    mm3PackedValue1 = pmaddwd(mm1PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 0x104));
    mm0PackedValue1 = pmaddwd(mm0PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 0x200));
    mm1PackedValue1 = pmaddwd(mm1PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 0x204));
    mm2PackedValue2 = pmaddwd(mm0PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 0x300));
    mm3PackedValue2 = pmaddwd(mm1PackedValue0,*(MmxPackedValue64 *)(cosineRowCursor + 0x304));
    mm0PackedValue2 = psraw(*(MmxPackedValue64 *)(inputPcm + 8),4);
    mm1PackedValue2 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xc),4);
    mm2PackedValue3 = pmaddwd(mm0PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 8));
    mm3PackedValue3 = pmaddwd(mm0PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0x108));
    mm2PackedValue4 = pmaddwd(mm1PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0xc));
    mm3PackedValue4 = pmaddwd(mm1PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0x10c));
    mm0PackedValue3 = pmaddwd(mm0PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0x208));
    mm1PackedValue3 = pmaddwd(mm1PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0x20c));
    mm2PackedValue5 = pmaddwd(mm0PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0x308));
    mm3PackedValue5 = pmaddwd(mm1PackedValue2,*(MmxPackedValue64 *)(cosineRowCursor + 0x30c));
    mm0PackedValue34 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x10),4);
    mm1PackedValue4 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x14),4);
    mm2PackedValue6 = pmaddwd(mm0PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x10));
    mm3PackedValue6 = pmaddwd(mm0PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x110));
    mm2PackedValue7 = pmaddwd(mm1PackedValue4,*(MmxPackedValue64 *)(cosineRowCursor + 0x14));
    mm3PackedValue7 = pmaddwd(mm1PackedValue4,*(MmxPackedValue64 *)(cosineRowCursor + 0x114));
    mm0PackedValue4 = pmaddwd(mm0PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x210));
    mm1PackedValue5 = pmaddwd(mm1PackedValue4,*(MmxPackedValue64 *)(cosineRowCursor + 0x214));
    mm2PackedValue8 = pmaddwd(mm0PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x310));
    mm3PackedValue8 = pmaddwd(mm1PackedValue4,*(MmxPackedValue64 *)(cosineRowCursor + 0x314));
    mm0PackedValue35 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x18),4);
    mm1PackedValue6 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x1c),4);
    mm2PackedValue9 = pmaddwd(mm0PackedValue35,*(MmxPackedValue64 *)(cosineRowCursor + 0x18));
    mm3PackedValue9 = pmaddwd(mm0PackedValue35,*(MmxPackedValue64 *)(cosineRowCursor + 0x118));
    mm2PackedValue10 = pmaddwd(mm1PackedValue6,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c));
    mm3PackedValue10 = pmaddwd(mm1PackedValue6,*(MmxPackedValue64 *)(cosineRowCursor + 0x11c));
    mm0PackedValue5 = pmaddwd(mm0PackedValue35,*(MmxPackedValue64 *)(cosineRowCursor + 0x218));
    mm1PackedValue7 = pmaddwd(mm1PackedValue6,*(MmxPackedValue64 *)(cosineRowCursor + 0x21c));
    mm2PackedValue11 = pmaddwd(mm0PackedValue35,*(MmxPackedValue64 *)(cosineRowCursor + 0x318));
    mm3PackedValue11 = pmaddwd(mm1PackedValue6,*(MmxPackedValue64 *)(cosineRowCursor + 0x31c));
    mm0PackedValue36 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x20),4);
    mm1PackedValue8 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x24),4);
    mm2PackedValue12 = pmaddwd(mm0PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x20));
    mm3PackedValue12 = pmaddwd(mm0PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x120));
    mm2PackedValue13 = pmaddwd(mm1PackedValue8,*(MmxPackedValue64 *)(cosineRowCursor + 0x24));
    mm3PackedValue13 = pmaddwd(mm1PackedValue8,*(MmxPackedValue64 *)(cosineRowCursor + 0x124));
    mm0PackedValue6 = pmaddwd(mm0PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x220));
    mm1PackedValue9 = pmaddwd(mm1PackedValue8,*(MmxPackedValue64 *)(cosineRowCursor + 0x224));
    mm2PackedValue14 = pmaddwd(mm0PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x320));
    mm3PackedValue14 = pmaddwd(mm1PackedValue8,*(MmxPackedValue64 *)(cosineRowCursor + 0x324));
    mm0PackedValue37 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x28),4);
    mm1PackedValue10 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x2c),4);
    mm2PackedValue15 = pmaddwd(mm0PackedValue37,*(MmxPackedValue64 *)(cosineRowCursor + 0x28));
    mm3PackedValue15 = pmaddwd(mm0PackedValue37,*(MmxPackedValue64 *)(cosineRowCursor + 0x128));
    mm2PackedValue16 = pmaddwd(mm1PackedValue10,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c));
    mm3PackedValue16 = pmaddwd(mm1PackedValue10,*(MmxPackedValue64 *)(cosineRowCursor + 0x12c));
    mm0PackedValue7 = pmaddwd(mm0PackedValue37,*(MmxPackedValue64 *)(cosineRowCursor + 0x228));
    mm1PackedValue11 = pmaddwd(mm1PackedValue10,*(MmxPackedValue64 *)(cosineRowCursor + 0x22c));
    mm2PackedValue17 = pmaddwd(mm0PackedValue37,*(MmxPackedValue64 *)(cosineRowCursor + 0x328));
    mm3PackedValue17 = pmaddwd(mm1PackedValue10,*(MmxPackedValue64 *)(cosineRowCursor + 0x32c));
    mm0PackedValue38 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x30),4);
    mm1PackedValue12 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x34),4);
    mm2PackedValue18 = pmaddwd(mm0PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x30));
    mm3PackedValue18 = pmaddwd(mm0PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x130));
    mm2PackedValue19 = pmaddwd(mm1PackedValue12,*(MmxPackedValue64 *)(cosineRowCursor + 0x34));
    mm3PackedValue19 = pmaddwd(mm1PackedValue12,*(MmxPackedValue64 *)(cosineRowCursor + 0x134));
    mm0PackedValue8 = pmaddwd(mm0PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x230));
    mm1PackedValue13 = pmaddwd(mm1PackedValue12,*(MmxPackedValue64 *)(cosineRowCursor + 0x234));
    mm2PackedValue20 = pmaddwd(mm0PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x330));
    mm3PackedValue20 = pmaddwd(mm1PackedValue12,*(MmxPackedValue64 *)(cosineRowCursor + 0x334));
    mm0PackedValue39 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x38),4);
    mm1PackedValue14 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x3c),4);
    mm2PackedValue21 = pmaddwd(mm0PackedValue39,*(MmxPackedValue64 *)(cosineRowCursor + 0x38));
    mm3PackedValue21 = pmaddwd(mm0PackedValue39,*(MmxPackedValue64 *)(cosineRowCursor + 0x138));
    mm2PackedValue22 = pmaddwd(mm1PackedValue14,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c));
    mm3PackedValue22 = pmaddwd(mm1PackedValue14,*(MmxPackedValue64 *)(cosineRowCursor + 0x13c));
    mm0PackedValue9 = pmaddwd(mm0PackedValue39,*(MmxPackedValue64 *)(cosineRowCursor + 0x238));
    mm1PackedValue15 = pmaddwd(mm1PackedValue14,*(MmxPackedValue64 *)(cosineRowCursor + 0x23c));
    mm2PackedValue23 = pmaddwd(mm0PackedValue39,*(MmxPackedValue64 *)(cosineRowCursor + 0x338));
    mm3PackedValue23 = pmaddwd(mm1PackedValue14,*(MmxPackedValue64 *)(cosineRowCursor + 0x33c));
    mm0PackedValue40 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x40),4);
    mm1PackedValue16 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x44),4);
    mm2PackedValue24 = pmaddwd(mm0PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x40));
    mm3PackedValue24 = pmaddwd(mm0PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x140));
    mm2PackedValue25 = pmaddwd(mm1PackedValue16,*(MmxPackedValue64 *)(cosineRowCursor + 0x44));
    mm3PackedValue25 = pmaddwd(mm1PackedValue16,*(MmxPackedValue64 *)(cosineRowCursor + 0x144));
    mm0PackedValue10 = pmaddwd(mm0PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x240));
    mm1PackedValue17 = pmaddwd(mm1PackedValue16,*(MmxPackedValue64 *)(cosineRowCursor + 0x244));
    mm2PackedValue26 = pmaddwd(mm0PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x340));
    mm3PackedValue26 = pmaddwd(mm1PackedValue16,*(MmxPackedValue64 *)(cosineRowCursor + 0x344));
    mm0PackedValue41 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x48),4);
    mm1PackedValue18 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x4c),4);
    mm2PackedValue27 = pmaddwd(mm0PackedValue41,*(MmxPackedValue64 *)(cosineRowCursor + 0x48));
    mm3PackedValue27 = pmaddwd(mm0PackedValue41,*(MmxPackedValue64 *)(cosineRowCursor + 0x148));
    mm2PackedValue28 = pmaddwd(mm1PackedValue18,*(MmxPackedValue64 *)(cosineRowCursor + 0x4c));
    mm3PackedValue28 = pmaddwd(mm1PackedValue18,*(MmxPackedValue64 *)(cosineRowCursor + 0x14c));
    mm0PackedValue11 = pmaddwd(mm0PackedValue41,*(MmxPackedValue64 *)(cosineRowCursor + 0x248));
    mm1PackedValue19 = pmaddwd(mm1PackedValue18,*(MmxPackedValue64 *)(cosineRowCursor + 0x24c));
    mm2PackedValue29 = pmaddwd(mm0PackedValue41,*(MmxPackedValue64 *)(cosineRowCursor + 0x348));
    mm3PackedValue29 = pmaddwd(mm1PackedValue18,*(MmxPackedValue64 *)(cosineRowCursor + 0x34c));
    mm0PackedValue42 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x50),4);
    mm1PackedValue20 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x54),4);
    mm2PackedValue30 = pmaddwd(mm0PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x50));
    mm3PackedValue30 = pmaddwd(mm0PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x150));
    mm2PackedValue31 = pmaddwd(mm1PackedValue20,*(MmxPackedValue64 *)(cosineRowCursor + 0x54));
    mm3PackedValue31 = pmaddwd(mm1PackedValue20,*(MmxPackedValue64 *)(cosineRowCursor + 0x154));
    mm0PackedValue12 = pmaddwd(mm0PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x250));
    mm1PackedValue21 = pmaddwd(mm1PackedValue20,*(MmxPackedValue64 *)(cosineRowCursor + 0x254));
    mm2PackedValue32 = pmaddwd(mm0PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x350));
    mm3PackedValue32 = pmaddwd(mm1PackedValue20,*(MmxPackedValue64 *)(cosineRowCursor + 0x354));
    mm0PackedValue43 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x58),4);
    mm1PackedValue22 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x5c),4);
    mm2PackedValue33 = pmaddwd(mm0PackedValue43,*(MmxPackedValue64 *)(cosineRowCursor + 0x58));
    mm3PackedValue33 = pmaddwd(mm0PackedValue43,*(MmxPackedValue64 *)(cosineRowCursor + 0x158));
    mm2PackedValue34 = pmaddwd(mm1PackedValue22,*(MmxPackedValue64 *)(cosineRowCursor + 0x5c));
    mm3PackedValue34 = pmaddwd(mm1PackedValue22,*(MmxPackedValue64 *)(cosineRowCursor + 0x15c));
    mm0PackedValue13 = pmaddwd(mm0PackedValue43,*(MmxPackedValue64 *)(cosineRowCursor + 0x258));
    mm1PackedValue23 = pmaddwd(mm1PackedValue22,*(MmxPackedValue64 *)(cosineRowCursor + 0x25c));
    mm2PackedValue35 = pmaddwd(mm0PackedValue43,*(MmxPackedValue64 *)(cosineRowCursor + 0x358));
    mm3PackedValue35 = pmaddwd(mm1PackedValue22,*(MmxPackedValue64 *)(cosineRowCursor + 0x35c));
    mm0PackedValue44 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x60),4);
    mm1PackedValue24 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x64),4);
    mm2PackedValue36 = pmaddwd(mm0PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x60));
    mm3PackedValue36 = pmaddwd(mm0PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x160));
    mm2PackedValue37 = pmaddwd(mm1PackedValue24,*(MmxPackedValue64 *)(cosineRowCursor + 0x64));
    mm3PackedValue37 = pmaddwd(mm1PackedValue24,*(MmxPackedValue64 *)(cosineRowCursor + 0x164));
    mm0PackedValue14 = pmaddwd(mm0PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x260));
    mm1PackedValue25 = pmaddwd(mm1PackedValue24,*(MmxPackedValue64 *)(cosineRowCursor + 0x264));
    mm2PackedValue38 = pmaddwd(mm0PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x360));
    mm3PackedValue38 = pmaddwd(mm1PackedValue24,*(MmxPackedValue64 *)(cosineRowCursor + 0x364));
    mm0PackedValue45 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x68),4);
    mm1PackedValue26 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x6c),4);
    mm2PackedValue39 = pmaddwd(mm0PackedValue45,*(MmxPackedValue64 *)(cosineRowCursor + 0x68));
    mm3PackedValue39 = pmaddwd(mm0PackedValue45,*(MmxPackedValue64 *)(cosineRowCursor + 0x168));
    mm2PackedValue40 = pmaddwd(mm1PackedValue26,*(MmxPackedValue64 *)(cosineRowCursor + 0x6c));
    mm3PackedValue40 = pmaddwd(mm1PackedValue26,*(MmxPackedValue64 *)(cosineRowCursor + 0x16c));
    mm0PackedValue15 = pmaddwd(mm0PackedValue45,*(MmxPackedValue64 *)(cosineRowCursor + 0x268));
    mm1PackedValue27 = pmaddwd(mm1PackedValue26,*(MmxPackedValue64 *)(cosineRowCursor + 0x26c));
    mm2PackedValue41 = pmaddwd(mm0PackedValue45,*(MmxPackedValue64 *)(cosineRowCursor + 0x368));
    mm3PackedValue41 = pmaddwd(mm1PackedValue26,*(MmxPackedValue64 *)(cosineRowCursor + 0x36c));
    mm0PackedValue46 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x70),4);
    mm1PackedValue28 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x74),4);
    mm2PackedValue42 = pmaddwd(mm0PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x70));
    mm3PackedValue42 = pmaddwd(mm0PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x170));
    mm2PackedValue43 = pmaddwd(mm1PackedValue28,*(MmxPackedValue64 *)(cosineRowCursor + 0x74));
    mm3PackedValue43 = pmaddwd(mm1PackedValue28,*(MmxPackedValue64 *)(cosineRowCursor + 0x174));
    mm0PackedValue16 = pmaddwd(mm0PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x270));
    mm1PackedValue29 = pmaddwd(mm1PackedValue28,*(MmxPackedValue64 *)(cosineRowCursor + 0x274));
    mm2PackedValue44 = pmaddwd(mm0PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x370));
    mm3PackedValue44 = pmaddwd(mm1PackedValue28,*(MmxPackedValue64 *)(cosineRowCursor + 0x374));
    mm0PackedValue47 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x78),4);
    mm1PackedValue30 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x7c),4);
    mm2PackedValue45 = pmaddwd(mm0PackedValue47,*(MmxPackedValue64 *)(cosineRowCursor + 0x78));
    mm3PackedValue45 = pmaddwd(mm0PackedValue47,*(MmxPackedValue64 *)(cosineRowCursor + 0x178));
    mm2PackedValue46 = pmaddwd(mm1PackedValue30,*(MmxPackedValue64 *)(cosineRowCursor + 0x7c));
    mm3PackedValue46 = pmaddwd(mm1PackedValue30,*(MmxPackedValue64 *)(cosineRowCursor + 0x17c));
    mm0PackedValue17 = pmaddwd(mm0PackedValue47,*(MmxPackedValue64 *)(cosineRowCursor + 0x278));
    mm1PackedValue31 = pmaddwd(mm1PackedValue30,*(MmxPackedValue64 *)(cosineRowCursor + 0x27c));
    mm2PackedValue47 = pmaddwd(mm0PackedValue47,*(MmxPackedValue64 *)(cosineRowCursor + 0x378));
    mm3PackedValue47 = pmaddwd(mm1PackedValue30,*(MmxPackedValue64 *)(cosineRowCursor + 0x37c));
    mm0PackedValue48 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x80),4);
    mm1PackedValue32 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x84),4);
    mm2PackedValue48 = pmaddwd(mm0PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x80));
    mm3PackedValue48 = pmaddwd(mm0PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x180));
    mm2PackedValue49 = pmaddwd(mm1PackedValue32,*(MmxPackedValue64 *)(cosineRowCursor + 0x84));
    mm3PackedValue49 = pmaddwd(mm1PackedValue32,*(MmxPackedValue64 *)(cosineRowCursor + 0x184));
    mm0PackedValue18 = pmaddwd(mm0PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x280));
    mm1PackedValue33 = pmaddwd(mm1PackedValue32,*(MmxPackedValue64 *)(cosineRowCursor + 0x284));
    mm2PackedValue50 = pmaddwd(mm0PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x380));
    mm3PackedValue50 = pmaddwd(mm1PackedValue32,*(MmxPackedValue64 *)(cosineRowCursor + 0x384));
    mm0PackedValue49 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x88),4);
    mm1PackedValue34 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x8c),4);
    mm2PackedValue51 = pmaddwd(mm0PackedValue49,*(MmxPackedValue64 *)(cosineRowCursor + 0x88));
    mm3PackedValue51 = pmaddwd(mm0PackedValue49,*(MmxPackedValue64 *)(cosineRowCursor + 0x188));
    mm2PackedValue52 = pmaddwd(mm1PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x8c));
    mm3PackedValue52 = pmaddwd(mm1PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x18c));
    mm0PackedValue19 = pmaddwd(mm0PackedValue49,*(MmxPackedValue64 *)(cosineRowCursor + 0x288));
    mm1PackedValue35 = pmaddwd(mm1PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x28c));
    mm2PackedValue53 = pmaddwd(mm0PackedValue49,*(MmxPackedValue64 *)(cosineRowCursor + 0x388));
    mm3PackedValue53 = pmaddwd(mm1PackedValue34,*(MmxPackedValue64 *)(cosineRowCursor + 0x38c));
    mm0PackedValue50 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x90),4);
    mm1PackedValue36 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x94),4);
    mm2PackedValue54 = pmaddwd(mm0PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x90));
    mm3PackedValue54 = pmaddwd(mm0PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x190));
    mm2PackedValue55 = pmaddwd(mm1PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x94));
    mm3PackedValue55 = pmaddwd(mm1PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x194));
    mm0PackedValue20 = pmaddwd(mm0PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x290));
    mm1PackedValue37 = pmaddwd(mm1PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x294));
    mm2PackedValue56 = pmaddwd(mm0PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x390));
    mm3PackedValue56 = pmaddwd(mm1PackedValue36,*(MmxPackedValue64 *)(cosineRowCursor + 0x394));
    mm0PackedValue51 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x98),4);
    mm1PackedValue38 = psraw(*(MmxPackedValue64 *)(inputPcm + 0x9c),4);
    mm2PackedValue57 = pmaddwd(mm0PackedValue51,*(MmxPackedValue64 *)(cosineRowCursor + 0x98));
    mm3PackedValue57 = pmaddwd(mm0PackedValue51,*(MmxPackedValue64 *)(cosineRowCursor + 0x198));
    mm2PackedValue58 = pmaddwd(mm1PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x9c));
    mm3PackedValue58 = pmaddwd(mm1PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x19c));
    mm0PackedValue21 = pmaddwd(mm0PackedValue51,*(MmxPackedValue64 *)(cosineRowCursor + 0x298));
    mm1PackedValue39 = pmaddwd(mm1PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x29c));
    mm2PackedValue59 = pmaddwd(mm0PackedValue51,*(MmxPackedValue64 *)(cosineRowCursor + 0x398));
    mm3PackedValue59 = pmaddwd(mm1PackedValue38,*(MmxPackedValue64 *)(cosineRowCursor + 0x39c));
    mm0PackedValue52 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xa0),4);
    mm1PackedValue40 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xa4),4);
    mm2PackedValue60 = pmaddwd(mm0PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0xa0));
    mm3PackedValue60 = pmaddwd(mm0PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0x1a0));
    mm2PackedValue61 = pmaddwd(mm1PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0xa4));
    mm3PackedValue61 = pmaddwd(mm1PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x1a4));
    mm0PackedValue22 = pmaddwd(mm0PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0x2a0));
    mm1PackedValue41 = pmaddwd(mm1PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x2a4));
    mm2PackedValue62 = pmaddwd(mm0PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0x3a0));
    mm3PackedValue62 = pmaddwd(mm1PackedValue40,*(MmxPackedValue64 *)(cosineRowCursor + 0x3a4));
    mm0PackedValue53 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xa8),4);
    mm1PackedValue42 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xac),4);
    mm2PackedValue63 = pmaddwd(mm0PackedValue53,*(MmxPackedValue64 *)(cosineRowCursor + 0xa8));
    mm3PackedValue63 = pmaddwd(mm0PackedValue53,*(MmxPackedValue64 *)(cosineRowCursor + 0x1a8));
    mm2PackedValue64 = pmaddwd(mm1PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0xac));
    mm3PackedValue64 = pmaddwd(mm1PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x1ac));
    mm0PackedValue23 = pmaddwd(mm0PackedValue53,*(MmxPackedValue64 *)(cosineRowCursor + 0x2a8));
    mm1PackedValue43 = pmaddwd(mm1PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x2ac));
    mm2PackedValue65 = pmaddwd(mm0PackedValue53,*(MmxPackedValue64 *)(cosineRowCursor + 0x3a8));
    mm3PackedValue65 = pmaddwd(mm1PackedValue42,*(MmxPackedValue64 *)(cosineRowCursor + 0x3ac));
    mm0PackedValue54 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xb0),4);
    mm1PackedValue44 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xb4),4);
    mm2PackedValue66 = pmaddwd(mm0PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0xb0));
    mm3PackedValue66 = pmaddwd(mm0PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0x1b0));
    mm2PackedValue67 = pmaddwd(mm1PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0xb4));
    mm3PackedValue67 = pmaddwd(mm1PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x1b4));
    mm0PackedValue24 = pmaddwd(mm0PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0x2b0));
    mm1PackedValue45 = pmaddwd(mm1PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x2b4));
    mm2PackedValue68 = pmaddwd(mm0PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0x3b0));
    mm3PackedValue68 = pmaddwd(mm1PackedValue44,*(MmxPackedValue64 *)(cosineRowCursor + 0x3b4));
    mm0PackedValue55 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xb8),4);
    mm1PackedValue46 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xbc),4);
    mm2PackedValue69 = pmaddwd(mm0PackedValue55,*(MmxPackedValue64 *)(cosineRowCursor + 0xb8));
    mm3PackedValue69 = pmaddwd(mm0PackedValue55,*(MmxPackedValue64 *)(cosineRowCursor + 0x1b8));
    mm2PackedValue70 = pmaddwd(mm1PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0xbc));
    mm3PackedValue70 = pmaddwd(mm1PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x1bc));
    mm0PackedValue25 = pmaddwd(mm0PackedValue55,*(MmxPackedValue64 *)(cosineRowCursor + 0x2b8));
    mm1PackedValue47 = pmaddwd(mm1PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x2bc));
    mm2PackedValue71 = pmaddwd(mm0PackedValue55,*(MmxPackedValue64 *)(cosineRowCursor + 0x3b8));
    mm3PackedValue71 = pmaddwd(mm1PackedValue46,*(MmxPackedValue64 *)(cosineRowCursor + 0x3bc));
    mm0PackedValue56 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xc0),4);
    mm1PackedValue48 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xc4),4);
    mm2PackedValue72 = pmaddwd(mm0PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0xc0));
    mm3PackedValue72 = pmaddwd(mm0PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c0));
    mm2PackedValue73 = pmaddwd(mm1PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0xc4));
    mm3PackedValue73 = pmaddwd(mm1PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c4));
    mm0PackedValue26 = pmaddwd(mm0PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c0));
    mm1PackedValue49 = pmaddwd(mm1PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c4));
    mm2PackedValue74 = pmaddwd(mm0PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c0));
    mm3PackedValue74 = pmaddwd(mm1PackedValue48,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c4));
    mm0PackedValue57 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xc8),4);
    mm1PackedValue50 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xcc),4);
    mm2PackedValue75 = pmaddwd(mm0PackedValue57,*(MmxPackedValue64 *)(cosineRowCursor + 0xc8));
    mm3PackedValue75 = pmaddwd(mm0PackedValue57,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c8));
    mm2PackedValue76 = pmaddwd(mm1PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0xcc));
    mm3PackedValue76 = pmaddwd(mm1PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x1cc));
    mm0PackedValue27 = pmaddwd(mm0PackedValue57,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c8));
    mm1PackedValue51 = pmaddwd(mm1PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x2cc));
    mm2PackedValue77 = pmaddwd(mm0PackedValue57,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c8));
    mm3PackedValue77 = pmaddwd(mm1PackedValue50,*(MmxPackedValue64 *)(cosineRowCursor + 0x3cc));
    mm0PackedValue58 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xd0),4);
    mm1PackedValue52 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xd4),4);
    mm2PackedValue78 = pmaddwd(mm0PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0xd0));
    mm3PackedValue78 = pmaddwd(mm0PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0x1d0));
    mm2PackedValue79 = pmaddwd(mm1PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0xd4));
    mm3PackedValue79 = pmaddwd(mm1PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0x1d4));
    mm0PackedValue28 = pmaddwd(mm0PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0x2d0));
    mm1PackedValue53 = pmaddwd(mm1PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0x2d4));
    mm2PackedValue80 = pmaddwd(mm0PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0x3d0));
    mm3PackedValue80 = pmaddwd(mm1PackedValue52,*(MmxPackedValue64 *)(cosineRowCursor + 0x3d4));
    mm0PackedValue59 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xd8),4);
    mm1PackedValue54 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xdc),4);
    mm2PackedValue81 = pmaddwd(mm0PackedValue59,*(MmxPackedValue64 *)(cosineRowCursor + 0xd8));
    mm3PackedValue81 = pmaddwd(mm0PackedValue59,*(MmxPackedValue64 *)(cosineRowCursor + 0x1d8));
    mm2PackedValue82 = pmaddwd(mm1PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0xdc));
    mm3PackedValue82 = pmaddwd(mm1PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0x1dc));
    mm0PackedValue29 = pmaddwd(mm0PackedValue59,*(MmxPackedValue64 *)(cosineRowCursor + 0x2d8));
    mm1PackedValue55 = pmaddwd(mm1PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0x2dc));
    mm2PackedValue83 = pmaddwd(mm0PackedValue59,*(MmxPackedValue64 *)(cosineRowCursor + 0x3d8));
    mm3PackedValue83 = pmaddwd(mm1PackedValue54,*(MmxPackedValue64 *)(cosineRowCursor + 0x3dc));
    mm0PackedValue60 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xe0),4);
    mm1PackedValue56 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xe4),4);
    mm2PackedValue84 = pmaddwd(mm0PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0xe0));
    mm3PackedValue84 = pmaddwd(mm0PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0x1e0));
    mm2PackedValue85 = pmaddwd(mm1PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0xe4));
    mm3PackedValue85 = pmaddwd(mm1PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0x1e4));
    mm0PackedValue30 = pmaddwd(mm0PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0x2e0));
    mm1PackedValue57 = pmaddwd(mm1PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0x2e4));
    mm2PackedValue86 = pmaddwd(mm0PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0x3e0));
    mm3PackedValue86 = pmaddwd(mm1PackedValue56,*(MmxPackedValue64 *)(cosineRowCursor + 0x3e4));
    mm0PackedValue61 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xe8),4);
    mm1PackedValue58 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xec),4);
    mm2PackedValue87 = pmaddwd(mm0PackedValue61,*(MmxPackedValue64 *)(cosineRowCursor + 0xe8));
    mm3PackedValue87 = pmaddwd(mm0PackedValue61,*(MmxPackedValue64 *)(cosineRowCursor + 0x1e8));
    mm2PackedValue88 = pmaddwd(mm1PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0xec));
    mm3PackedValue88 = pmaddwd(mm1PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0x1ec));
    mm0PackedValue31 = pmaddwd(mm0PackedValue61,*(MmxPackedValue64 *)(cosineRowCursor + 0x2e8));
    mm1PackedValue59 = pmaddwd(mm1PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0x2ec));
    mm2PackedValue89 = pmaddwd(mm0PackedValue61,*(MmxPackedValue64 *)(cosineRowCursor + 0x3e8));
    mm3PackedValue89 = pmaddwd(mm1PackedValue58,*(MmxPackedValue64 *)(cosineRowCursor + 0x3ec));
    mm0PackedValue62 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xf0),4);
    mm1PackedValue60 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xf4),4);
    mm2PackedValue90 = pmaddwd(mm0PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0xf0));
    mm3PackedValue90 = pmaddwd(mm0PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0x1f0));
    mm2PackedValue91 = pmaddwd(mm1PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0xf4));
    mm3PackedValue91 = pmaddwd(mm1PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0x1f4));
    mm0PackedValue32 = pmaddwd(mm0PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0x2f0));
    mm1PackedValue61 = pmaddwd(mm1PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0x2f4));
    mm2PackedValue92 = pmaddwd(mm0PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0x3f0));
    mm3PackedValue92 = pmaddwd(mm1PackedValue60,*(MmxPackedValue64 *)(cosineRowCursor + 0x3f4));
    mm0PackedValue63 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xf8),4);
    mm1PackedValue62 = psraw(*(MmxPackedValue64 *)(inputPcm + 0xfc),4);
    mm2PackedValue93 = pmaddwd(mm0PackedValue63,*(MmxPackedValue64 *)(cosineRowCursor + 0xf8));
    mm3PackedValue93 = pmaddwd(mm0PackedValue63,*(MmxPackedValue64 *)(cosineRowCursor + 0x1f8));
    mm2PackedValue94 = pmaddwd(mm1PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0xfc));
    mm3PackedValue94 = pmaddwd(mm1PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0x1fc));
    /* Horizontal sums (32-bit wrapping); bankN{High,Low}LaneSum is the partial sum of one lane of row N.
       The mmNPackedValueM names are the MMX registers of the original. */
    bank0HighLaneSum =(int)((uint64_t)mm2PackedValue0 >> 0x20) + (int)((uint64_t)mm2PackedValue1 >> 0x20) +
            (int)((uint64_t)mm2PackedValue3 >> 0x20) + (int)((uint64_t)mm2PackedValue4 >> 0x20) +
            (int)((uint64_t)mm2PackedValue6 >> 0x20) + (int)((uint64_t)mm2PackedValue7 >> 0x20) +
            (int)((uint64_t)mm2PackedValue9 >> 0x20) + (int)((uint64_t)mm2PackedValue10 >> 0x20) +
            (int)((uint64_t)mm2PackedValue12 >> 0x20) + (int)((uint64_t)mm2PackedValue13 >> 0x20)
            + (int)((uint64_t)mm2PackedValue15 >> 0x20) +
            (int)((uint64_t)mm2PackedValue16 >> 0x20) + (int)((uint64_t)mm2PackedValue18 >> 0x20)
            + (int)((uint64_t)mm2PackedValue19 >> 0x20) +
            (int)((uint64_t)mm2PackedValue21 >> 0x20) + (int)((uint64_t)mm2PackedValue22 >> 0x20)
            + (int)((uint64_t)mm2PackedValue24 >> 0x20) +
            (int)((uint64_t)mm2PackedValue25 >> 0x20) + (int)((uint64_t)mm2PackedValue27 >> 0x20)
            + (int)((uint64_t)mm2PackedValue28 >> 0x20) +
            (int)((uint64_t)mm2PackedValue30 >> 0x20) + (int)((uint64_t)mm2PackedValue31 >> 0x20)
            + (int)((uint64_t)mm2PackedValue33 >> 0x20) +
            (int)((uint64_t)mm2PackedValue34 >> 0x20) + (int)((uint64_t)mm2PackedValue36 >> 0x20)
            + (int)((uint64_t)mm2PackedValue37 >> 0x20) +
            (int)((uint64_t)mm2PackedValue39 >> 0x20) + (int)((uint64_t)mm2PackedValue40 >> 0x20)
            + (int)((uint64_t)mm2PackedValue42 >> 0x20) +
            (int)((uint64_t)mm2PackedValue43 >> 0x20) + (int)((uint64_t)mm2PackedValue45 >> 0x20)
            + (int)((uint64_t)mm2PackedValue46 >> 0x20) +
            (int)((uint64_t)mm2PackedValue48 >> 0x20) + (int)((uint64_t)mm2PackedValue49 >> 0x20)
            + (int)((uint64_t)mm2PackedValue51 >> 0x20) +
            (int)((uint64_t)mm2PackedValue52 >> 0x20) + (int)((uint64_t)mm2PackedValue54 >> 0x20)
            + (int)((uint64_t)mm2PackedValue55 >> 0x20) +
            (int)((uint64_t)mm2PackedValue57 >> 0x20) + (int)((uint64_t)mm2PackedValue58 >> 0x20)
            + (int)((uint64_t)mm2PackedValue60 >> 0x20) +
            (int)((uint64_t)mm2PackedValue61 >> 0x20) + (int)((uint64_t)mm2PackedValue63 >> 0x20)
            + (int)((uint64_t)mm2PackedValue64 >> 0x20) +
            (int)((uint64_t)mm2PackedValue66 >> 0x20) + (int)((uint64_t)mm2PackedValue67 >> 0x20)
            + (int)((uint64_t)mm2PackedValue69 >> 0x20) +
            (int)((uint64_t)mm2PackedValue70 >> 0x20) + (int)((uint64_t)mm2PackedValue72 >> 0x20)
            + (int)((uint64_t)mm2PackedValue73 >> 0x20) +
            (int)((uint64_t)mm2PackedValue75 >> 0x20) + (int)((uint64_t)mm2PackedValue76 >> 0x20)
            + (int)((uint64_t)mm2PackedValue78 >> 0x20) +
            (int)((uint64_t)mm2PackedValue79 >> 0x20) + (int)((uint64_t)mm2PackedValue81 >> 0x20)
            + (int)((uint64_t)mm2PackedValue82 >> 0x20) +
            (int)((uint64_t)mm2PackedValue84 >> 0x20) + (int)((uint64_t)mm2PackedValue85 >> 0x20)
            + (int)((uint64_t)mm2PackedValue87 >> 0x20) +
            (int)((uint64_t)mm2PackedValue88 >> 0x20) + (int)((uint64_t)mm2PackedValue90 >> 0x20)
            + (int)((uint64_t)mm2PackedValue91 >> 0x20) +
            (int)((uint64_t)mm2PackedValue93 >> 0x20) + (int)((uint64_t)mm2PackedValue94 >> 0x20);
    bank1HighLaneSum = (int)((uint64_t)mm3PackedValue0 >> 0x20) + (int)((uint64_t)mm3PackedValue1 >> 0x20) +
            (int)((uint64_t)mm3PackedValue3 >> 0x20) + (int)((uint64_t)mm3PackedValue4 >> 0x20) +
            (int)((uint64_t)mm3PackedValue6 >> 0x20) + (int)((uint64_t)mm3PackedValue7 >> 0x20) +
            (int)((uint64_t)mm3PackedValue9 >> 0x20) + (int)((uint64_t)mm3PackedValue10 >> 0x20) +
            (int)((uint64_t)mm3PackedValue12 >> 0x20) + (int)((uint64_t)mm3PackedValue13 >> 0x20)
            + (int)((uint64_t)mm3PackedValue15 >> 0x20) +
            (int)((uint64_t)mm3PackedValue16 >> 0x20) + (int)((uint64_t)mm3PackedValue18 >> 0x20)
            + (int)((uint64_t)mm3PackedValue19 >> 0x20) +
            (int)((uint64_t)mm3PackedValue21 >> 0x20) + (int)((uint64_t)mm3PackedValue22 >> 0x20)
            + (int)((uint64_t)mm3PackedValue24 >> 0x20) +
            (int)((uint64_t)mm3PackedValue25 >> 0x20) + (int)((uint64_t)mm3PackedValue27 >> 0x20)
            + (int)((uint64_t)mm3PackedValue28 >> 0x20) +
            (int)((uint64_t)mm3PackedValue30 >> 0x20) + (int)((uint64_t)mm3PackedValue31 >> 0x20)
            + (int)((uint64_t)mm3PackedValue33 >> 0x20) +
            (int)((uint64_t)mm3PackedValue34 >> 0x20) + (int)((uint64_t)mm3PackedValue36 >> 0x20)
            + (int)((uint64_t)mm3PackedValue37 >> 0x20) +
            (int)((uint64_t)mm3PackedValue39 >> 0x20) + (int)((uint64_t)mm3PackedValue40 >> 0x20)
            + (int)((uint64_t)mm3PackedValue42 >> 0x20) +
            (int)((uint64_t)mm3PackedValue43 >> 0x20) + (int)((uint64_t)mm3PackedValue45 >> 0x20)
            + (int)((uint64_t)mm3PackedValue46 >> 0x20) +
            (int)((uint64_t)mm3PackedValue48 >> 0x20) + (int)((uint64_t)mm3PackedValue49 >> 0x20)
            + (int)((uint64_t)mm3PackedValue51 >> 0x20) +
            (int)((uint64_t)mm3PackedValue52 >> 0x20) + (int)((uint64_t)mm3PackedValue54 >> 0x20)
            + (int)((uint64_t)mm3PackedValue55 >> 0x20) +
            (int)((uint64_t)mm3PackedValue57 >> 0x20) + (int)((uint64_t)mm3PackedValue58 >> 0x20)
            + (int)((uint64_t)mm3PackedValue60 >> 0x20) +
            (int)((uint64_t)mm3PackedValue61 >> 0x20) + (int)((uint64_t)mm3PackedValue63 >> 0x20)
            + (int)((uint64_t)mm3PackedValue64 >> 0x20) +
            (int)((uint64_t)mm3PackedValue66 >> 0x20) + (int)((uint64_t)mm3PackedValue67 >> 0x20)
            + (int)((uint64_t)mm3PackedValue69 >> 0x20) +
            (int)((uint64_t)mm3PackedValue70 >> 0x20) + (int)((uint64_t)mm3PackedValue72 >> 0x20)
            + (int)((uint64_t)mm3PackedValue73 >> 0x20) +
            (int)((uint64_t)mm3PackedValue75 >> 0x20) + (int)((uint64_t)mm3PackedValue76 >> 0x20)
            + (int)((uint64_t)mm3PackedValue78 >> 0x20) +
            (int)((uint64_t)mm3PackedValue79 >> 0x20) + (int)((uint64_t)mm3PackedValue81 >> 0x20)
            + (int)((uint64_t)mm3PackedValue82 >> 0x20) +
            (int)((uint64_t)mm3PackedValue84 >> 0x20) + (int)((uint64_t)mm3PackedValue85 >> 0x20)
            + (int)((uint64_t)mm3PackedValue87 >> 0x20) +
            (int)((uint64_t)mm3PackedValue88 >> 0x20) + (int)((uint64_t)mm3PackedValue90 >> 0x20)
            + (int)((uint64_t)mm3PackedValue91 >> 0x20) +
            (int)((uint64_t)mm3PackedValue93 >> 0x20) + (int)((uint64_t)mm3PackedValue94 >> 0x20);
    mm0PackedValue33 = pmaddwd(mm0PackedValue63,*(MmxPackedValue64 *)(cosineRowCursor + 0x2f8));
    mm1PackedValue63 = pmaddwd(mm1PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0x2fc));
    mm2PackedValue95 = pmaddwd(mm0PackedValue63,*(MmxPackedValue64 *)(cosineRowCursor + 0x3f8));
    mm3PackedValue95 = pmaddwd(mm1PackedValue62,*(MmxPackedValue64 *)(cosineRowCursor + 0x3fc));
    bank2LowLaneSum = (int)mm0PackedValue1 + (int)mm1PackedValue1 + (int)mm0PackedValue3 +
            (int)mm1PackedValue3 + (int)mm0PackedValue4 + (int)mm1PackedValue5 +
            (int)mm0PackedValue5 + (int)mm1PackedValue7 + (int)mm0PackedValue6 +
            (int)mm1PackedValue9 + (int)mm0PackedValue7 + (int)mm1PackedValue11 +
            (int)mm0PackedValue8 + (int)mm1PackedValue13 + (int)mm0PackedValue9 +
            (int)mm1PackedValue15 + (int)mm0PackedValue10 + (int)mm1PackedValue17 +
            (int)mm0PackedValue11 + (int)mm1PackedValue19 + (int)mm0PackedValue12 +
            (int)mm1PackedValue21 + (int)mm0PackedValue13 + (int)mm1PackedValue23 +
            (int)mm0PackedValue14 + (int)mm1PackedValue25 + (int)mm0PackedValue15 +
            (int)mm1PackedValue27 + (int)mm0PackedValue16 + (int)mm1PackedValue29 +
            (int)mm0PackedValue17 + (int)mm1PackedValue31 + (int)mm0PackedValue18 +
            (int)mm1PackedValue33 + (int)mm0PackedValue19 + (int)mm1PackedValue35 +
            (int)mm0PackedValue20 + (int)mm1PackedValue37 + (int)mm0PackedValue21 +
            (int)mm1PackedValue39 + (int)mm0PackedValue22 + (int)mm1PackedValue41 +
            (int)mm0PackedValue23 + (int)mm1PackedValue43 + (int)mm0PackedValue24 +
            (int)mm1PackedValue45 + (int)mm0PackedValue25 + (int)mm1PackedValue47 +
            (int)mm0PackedValue26 + (int)mm1PackedValue49 + (int)mm0PackedValue27 +
            (int)mm1PackedValue51 + (int)mm0PackedValue28 + (int)mm1PackedValue53 +
            (int)mm0PackedValue29 + (int)mm1PackedValue55 + (int)mm0PackedValue30 +
            (int)mm1PackedValue57 + (int)mm0PackedValue31 + (int)mm1PackedValue59 +
            (int)mm0PackedValue32 + (int)mm1PackedValue61 + (int)mm0PackedValue33 +
            (int)mm1PackedValue63;
    bank3LowLaneSum = (int)mm2PackedValue2 + (int)mm3PackedValue2 + (int)mm2PackedValue5 +
            (int)mm3PackedValue5 + (int)mm2PackedValue8 + (int)mm3PackedValue8 +
            (int)mm2PackedValue11 + (int)mm3PackedValue11 + (int)mm2PackedValue14 +
            (int)mm3PackedValue14 + (int)mm2PackedValue17 + (int)mm3PackedValue17 +
            (int)mm2PackedValue20 + (int)mm3PackedValue20 + (int)mm2PackedValue23 +
            (int)mm3PackedValue23 + (int)mm2PackedValue26 + (int)mm3PackedValue26 +
            (int)mm2PackedValue29 + (int)mm3PackedValue29 + (int)mm2PackedValue32 +
            (int)mm3PackedValue32 + (int)mm2PackedValue35 + (int)mm3PackedValue35 +
            (int)mm2PackedValue38 + (int)mm3PackedValue38 + (int)mm2PackedValue41 +
            (int)mm3PackedValue41 + (int)mm2PackedValue44 + (int)mm3PackedValue44 +
            (int)mm2PackedValue47 + (int)mm3PackedValue47 + (int)mm2PackedValue50 +
            (int)mm3PackedValue50 + (int)mm2PackedValue53 + (int)mm3PackedValue53 +
            (int)mm2PackedValue56 + (int)mm3PackedValue56 + (int)mm2PackedValue59 +
            (int)mm3PackedValue59 + (int)mm2PackedValue62 + (int)mm3PackedValue62 +
            (int)mm2PackedValue65 + (int)mm3PackedValue65 + (int)mm2PackedValue68 +
            (int)mm3PackedValue68 + (int)mm2PackedValue71 + (int)mm3PackedValue71 +
            (int)mm2PackedValue74 + (int)mm3PackedValue74 + (int)mm2PackedValue77 +
            (int)mm3PackedValue77 + (int)mm2PackedValue80 + (int)mm3PackedValue80 +
            (int)mm2PackedValue83 + (int)mm3PackedValue83 + (int)mm2PackedValue86 +
            (int)mm3PackedValue86 + (int)mm2PackedValue89 + (int)mm3PackedValue89 +
            (int)mm2PackedValue92 + (int)mm3PackedValue92 + (int)mm2PackedValue95 +
            (int)mm3PackedValue95;
    cosineRowCursor = cosineRowCursor + SAM_MMX_OUTPUTS_PER_PASS * SAM_BLOCK_SAMPLE_COUNT;
    /* bits 16..31 of each row total into word lane N (lane masks 0..3), then >> 3 per word (PSRAW) */
    mm0PackedValue64 =
         psraw(SAM_PACK_LANE_PAIR(bank0HighLaneSum,bank0HighLaneSum + (int)mm2PackedValue0 + (int)mm2PackedValue1 +
                                      (int)mm2PackedValue3 + (int)mm2PackedValue4 +
                                      (int)mm2PackedValue6 + (int)mm2PackedValue7 +
                                      (int)mm2PackedValue9 + (int)mm2PackedValue10 +
                                      (int)mm2PackedValue12 + (int)mm2PackedValue13 +
                                      (int)mm2PackedValue15 + (int)mm2PackedValue16 +
                                      (int)mm2PackedValue18 + (int)mm2PackedValue19 +
                                      (int)mm2PackedValue21 + (int)mm2PackedValue22 +
                                      (int)mm2PackedValue24 + (int)mm2PackedValue25 +
                                      (int)mm2PackedValue27 + (int)mm2PackedValue28 +
                                      (int)mm2PackedValue30 + (int)mm2PackedValue31 +
                                      (int)mm2PackedValue33 + (int)mm2PackedValue34 +
                                      (int)mm2PackedValue36 + (int)mm2PackedValue37 +
                                      (int)mm2PackedValue39 + (int)mm2PackedValue40 +
                                      (int)mm2PackedValue42 + (int)mm2PackedValue43 +
                                      (int)mm2PackedValue45 + (int)mm2PackedValue46 +
                                      (int)mm2PackedValue48 + (int)mm2PackedValue49 +
                                      (int)mm2PackedValue51 + (int)mm2PackedValue52 +
                                      (int)mm2PackedValue54 + (int)mm2PackedValue55 +
                                      (int)mm2PackedValue57 + (int)mm2PackedValue58 +
                                      (int)mm2PackedValue60 + (int)mm2PackedValue61 +
                                      (int)mm2PackedValue63 + (int)mm2PackedValue64 +
                                      (int)mm2PackedValue66 + (int)mm2PackedValue67 +
                                      (int)mm2PackedValue69 + (int)mm2PackedValue70 +
                                      (int)mm2PackedValue72 + (int)mm2PackedValue73 +
                                      (int)mm2PackedValue75 + (int)mm2PackedValue76 +
                                      (int)mm2PackedValue78 + (int)mm2PackedValue79 +
                                      (int)mm2PackedValue81 + (int)mm2PackedValue82 +
                                      (int)mm2PackedValue84 + (int)mm2PackedValue85 +
                                      (int)mm2PackedValue87 + (int)mm2PackedValue88 +
                                      (int)mm2PackedValue90 + (int)mm2PackedValue91 +
                                      (int)mm2PackedValue93 + (int)mm2PackedValue94) >> 0x10 &
               g_SoundDecodeMmxWordLaneMask0 |
               SAM_PACK_LANE_PAIR(bank1HighLaneSum,bank1HighLaneSum + (int)mm3PackedValue0 + (int)mm3PackedValue1 +
                                      (int)mm3PackedValue3 + (int)mm3PackedValue4 +
                                      (int)mm3PackedValue6 + (int)mm3PackedValue7 +
                                      (int)mm3PackedValue9 + (int)mm3PackedValue10 +
                                      (int)mm3PackedValue12 + (int)mm3PackedValue13 +
                                      (int)mm3PackedValue15 + (int)mm3PackedValue16 +
                                      (int)mm3PackedValue18 + (int)mm3PackedValue19 +
                                      (int)mm3PackedValue21 + (int)mm3PackedValue22 +
                                      (int)mm3PackedValue24 + (int)mm3PackedValue25 +
                                      (int)mm3PackedValue27 + (int)mm3PackedValue28 +
                                      (int)mm3PackedValue30 + (int)mm3PackedValue31 +
                                      (int)mm3PackedValue33 + (int)mm3PackedValue34 +
                                      (int)mm3PackedValue36 + (int)mm3PackedValue37 +
                                      (int)mm3PackedValue39 + (int)mm3PackedValue40 +
                                      (int)mm3PackedValue42 + (int)mm3PackedValue43 +
                                      (int)mm3PackedValue45 + (int)mm3PackedValue46 +
                                      (int)mm3PackedValue48 + (int)mm3PackedValue49 +
                                      (int)mm3PackedValue51 + (int)mm3PackedValue52 +
                                      (int)mm3PackedValue54 + (int)mm3PackedValue55 +
                                      (int)mm3PackedValue57 + (int)mm3PackedValue58 +
                                      (int)mm3PackedValue60 + (int)mm3PackedValue61 +
                                      (int)mm3PackedValue63 + (int)mm3PackedValue64 +
                                      (int)mm3PackedValue66 + (int)mm3PackedValue67 +
                                      (int)mm3PackedValue69 + (int)mm3PackedValue70 +
                                      (int)mm3PackedValue72 + (int)mm3PackedValue73 +
                                      (int)mm3PackedValue75 + (int)mm3PackedValue76 +
                                      (int)mm3PackedValue78 + (int)mm3PackedValue79 +
                                      (int)mm3PackedValue81 + (int)mm3PackedValue82 +
                                      (int)mm3PackedValue84 + (int)mm3PackedValue85 +
                                      (int)mm3PackedValue87 + (int)mm3PackedValue88 +
                                      (int)mm3PackedValue90 + (int)mm3PackedValue91 +
                                      (int)mm3PackedValue93 + (int)mm3PackedValue94) &
               g_SoundDecodeMmxWordLaneMask1 |
               SAM_PACK_LANE_PAIR(bank3LowLaneSum + (int)((uint64_t)mm2PackedValue2 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue2 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue5 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue5 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue8 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue8 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue11 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue11 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue14 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue14 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue17 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue17 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue20 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue20 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue23 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue23 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue26 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue26 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue29 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue29 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue32 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue32 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue35 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue35 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue38 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue38 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue41 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue41 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue44 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue44 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue47 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue47 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue50 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue50 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue53 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue53 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue56 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue56 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue59 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue59 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue62 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue62 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue65 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue65 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue68 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue68 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue71 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue71 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue74 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue74 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue77 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue77 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue80 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue80 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue83 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue83 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue86 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue86 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue89 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue89 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue92 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue92 >> 0x20) +
                                (int)((uint64_t)mm2PackedValue95 >> 0x20) +
                                (int)((uint64_t)mm3PackedValue95 >> 0x20),bank3LowLaneSum) &
               g_SoundDecodeMmxWordLaneMask3 |
               SAM_PACK_LANE_PAIR(bank2LowLaneSum + (int)((uint64_t)mm0PackedValue1 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue1 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue3 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue3 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue4 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue5 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue5 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue7 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue6 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue9 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue7 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue11 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue8 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue13 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue9 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue15 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue10 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue17 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue11 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue19 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue12 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue21 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue13 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue23 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue14 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue25 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue15 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue27 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue16 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue29 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue17 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue31 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue18 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue33 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue19 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue35 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue20 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue37 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue21 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue39 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue22 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue41 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue23 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue43 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue24 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue45 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue25 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue47 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue26 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue49 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue27 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue51 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue28 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue53 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue29 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue55 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue30 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue57 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue31 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue59 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue32 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue61 >> 0x20) +
                                (int)((uint64_t)mm0PackedValue33 >> 0x20) +
                                (int)((uint64_t)mm1PackedValue63 >> 0x20),bank2LowLaneSum) >> 0x10 &
               g_SoundDecodeMmxWordLaneMask2,3);
    outputPassesRemaining--;
    *(MmxPackedValue64 *)outputCoefficients = mm0PackedValue64;
    outputCoefficients = outputCoefficients + SAM_MMX_OUTPUTS_PER_PASS;
  } while (outputPassesRemaining != 0);
}

/* Packs 256 transform coefficients into one SAM block, the exact format SoundSample_DecodePackedCoefficientBlock
   reads: prefix codes in a little-endian bit stream (prefix bits from bit 0): 0 -> zero (1 bit), 1,0 -> 3-bit
   value -4..3 (5 bits), 1,1,0 -> 6-bit value -32..31 (9 bits), 1,1,1 -> 12-bit value (15 bits, clamped to
   -2048..2047). Lossy: -1 and +1 are stored as zero. Returns the bytes written, rounded up to a multiple of 4.
   Nothing in the executable calls it or stores its address in a function-pointer table.
*/
uint32_t SoundSample_EncodePackedCoefficientBlock(uint8_t *encodedBlock,short *inputCoefficients)

{
  uint32_t coefficientValue;
  uint8_t bitShift;
  uint32_t pendingBitCount;
  uint32_t bitAccumulator;
  uint32_t *outputCursor;
  int coefficientsRemaining;
  
  coefficientsRemaining = SAM_BLOCK_SAMPLE_COUNT;
  pendingBitCount = 0;
  bitAccumulator = 0;
  outputCursor = (uint32_t *)encodedBlock;
  do {
    coefficientValue = (uint32_t)*inputCoefficients;
    /* -1, 0 and +1 all become the 1-bit zero code (a 0 bit, nothing to OR in) */
    if (((int)coefficientValue < -1) || (1 < (int)coefficientValue)) {
      bitShift = (uint8_t)pendingBitCount;
      if (((int)coefficientValue < -4) || (3 < (int)coefficientValue)) {
        if (((int)coefficientValue < -32) || (31 < (int)coefficientValue)) {
          if ((int)coefficientValue < 2048) {
            if ((int)coefficientValue < -2048) {
              coefficientValue = 0x800; /* -2048 as 12 bits */
            }
            else {
              coefficientValue = coefficientValue & 0xfff;
            }
          }
          else {
            coefficientValue = 0x7ff; /* 2047 */
          }
          pendingBitCount = pendingBitCount + 15;
          bitAccumulator = bitAccumulator | (coefficientValue * 8 + 7) << (bitShift & 0x1f);
        }
        else {
          pendingBitCount = pendingBitCount + 9;
          bitAccumulator = bitAccumulator | ((coefficientValue & 0x3f) * 8 + 3) << (bitShift & 0x1f);
        }
      }
      else {
        pendingBitCount = pendingBitCount + 5;
        bitAccumulator = bitAccumulator | ((coefficientValue & 7) * 4 + 1) << (bitShift & 0x1f);
      }
    }
    else {
      pendingBitCount = pendingBitCount + 1;
    }
    /* flush the complete bytes; at most 7 + 15 = 22 bits are pending here, so the 24-bit branch never runs */
    if (pendingBitCount < 24) {
      if (pendingBitCount < 16) {
        if (7 < pendingBitCount) {
          *(char *)outputCursor = (char)bitAccumulator;
          bitAccumulator = bitAccumulator >> 8;
          outputCursor = (uint32_t *)((uintptr_t)outputCursor + 1);
          pendingBitCount = pendingBitCount - 8;
        }
      }
      else {
        *(short *)outputCursor = (short)bitAccumulator;
        bitAccumulator = bitAccumulator >> 16;
        outputCursor = (uint32_t *)((uintptr_t)outputCursor + 2);
        pendingBitCount = pendingBitCount - 16;
      }
    }
    else {
      *outputCursor = bitAccumulator;
      bitAccumulator = bitAccumulator >> 24;
      outputCursor = (uint32_t *)((uintptr_t)outputCursor + 3);
      pendingBitCount = pendingBitCount - 24;
    }
    inputCoefficients++;
    coefficientsRemaining--;
  } while (coefficientsRemaining != 0);
  /* write out the last partial byte (fewer than 8 bits remain after the flush above) */
  if (pendingBitCount < 8) {
    if (pendingBitCount != 0) {
      *(char *)outputCursor = (char)bitAccumulator;
      outputCursor = (uint32_t *)((uintptr_t)outputCursor + 1);
    }
  }
  else {
    *(short *)outputCursor = (short)bitAccumulator;
    outputCursor = (uint32_t *)((uintptr_t)outputCursor + 2);
  }
  /* bytes written, rounded up to a dword */
  return (uint32_t)((((uintptr_t)outputCursor + 3U) & ~(uintptr_t)3) - (uintptr_t)encodedBlock);
}
