/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/audio/codec/sam.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/audio/codec/sam.h>
#include <thandor/thandor.h>

/* Module data. */

const uint64_t g_SoundDecodeMmxWordLaneMask0 = 0xFFFFull;

const uint64_t g_SoundDecodeMmxWordLaneMask1 = 0xFFFF0000ull;

const uint64_t g_SoundDecodeMmxWordLaneMask2 = 0xFFFF00000000ull;

const uint64_t g_SoundDecodeMmxWordLaneMask3 = 0xFFFF000000000000ull;

/* the two cosine matrices of the .sam codec, set by CosineDerivedLookupTables_Init */
short *g_CosineDerivedLookupAllocation = nullptr;

short *g_CosineDerivedLookupSecondTable = nullptr;

/* Implementation ownership: audio/codec/sam. */

/* Mono variant of SoundSample_DecodeCoefficientBlockToPcmMmx: the same inverse cosine transform of 256
   coefficients with g_CosineDerivedLookupSecondTable (each sample is bits 16..31 of the wrapping 256-tap dot
   product x32, doubled with saturation), written as 256 mono 16-bit samples (0x200 bytes). Nothing in the
   executable calls it or stores its address in a function-pointer table.
*/
void SoundCoefficientTransform_ApplyCosineBanksMmx(short *outputMonoPcm,SoundCoefficientBlock *coefficientBlock)

{
  MmxPackedValue64 coefficientQuadHigh;
  short *cosineRowCursor;
  MmxPackedValue64 coefficientPairProducts91;
  MmxPackedValue64 coefficientPairProducts93;
  MmxPackedValue64 coefficientPairProducts92;
  MmxPackedValue64 coefficientPairProducts95;
  MmxPackedValue64 coefficientPairProducts94;
  MmxPackedValue64 coefficientPairProducts97;
  MmxPackedValue64 coefficientPairProducts96;
  MmxPackedValue64 coefficientPairProducts84;
  MmxPackedValue64 coefficientPairProducts83;
  MmxPackedValue64 coefficientPairProducts86;
  MmxPackedValue64 coefficientPairProducts85;
  MmxPackedValue64 coefficientPairProducts88;
  MmxPackedValue64 coefficientPairProducts87;
  MmxPackedValue64 coefficientPairProducts90;
  MmxPackedValue64 coefficientPairProducts89;
  MmxPackedValue64 coefficientPairProducts107;
  MmxPackedValue64 coefficientPairProducts106;
  MmxPackedValue64 coefficientPairProducts109;
  MmxPackedValue64 coefficientPairProducts108;
  MmxPackedValue64 coefficientPairProducts111;
  MmxPackedValue64 coefficientPairProducts110;
  MmxPackedValue64 coefficientPairProducts113;
  MmxPackedValue64 coefficientPairProducts112;
  MmxPackedValue64 coefficientPairProducts99;
  MmxPackedValue64 coefficientPairProducts98;
  MmxPackedValue64 coefficientPairProducts101;
  MmxPackedValue64 coefficientPairProducts100;
  MmxPackedValue64 coefficientPairProducts103;
  MmxPackedValue64 coefficientPairProducts102;
  MmxPackedValue64 coefficientPairProducts105;
  MmxPackedValue64 coefficientPairProducts104;
  MmxPackedValue64 coefficientPairProducts122;
  MmxPackedValue64 packedOutputWords;
  MmxPackedValue64 saturatedOutputWords;
  MmxPackedValue64 coefficientQuadLow;
  MmxPackedValue64 coefficientPairProducts124;
  MmxPackedValue64 coefficientPairProducts123;
  MmxPackedValue64 coefficientPairProducts126;
  MmxPackedValue64 coefficientPairProducts125;
  MmxPackedValue64 coefficientPairProducts128;
  MmxPackedValue64 coefficientPairProducts127;
  MmxPackedValue64 coefficientPairProducts115;
  MmxPackedValue64 coefficientPairProducts114;
  MmxPackedValue64 coefficientPairProducts117;
  MmxPackedValue64 coefficientPairProducts116;
  MmxPackedValue64 coefficientPairProducts119;
  MmxPackedValue64 coefficientPairProducts118;
  MmxPackedValue64 coefficientPairProducts121;
  MmxPackedValue64 coefficientPairProducts120;
  MmxPackedValue64 coefficientPairProducts28;
  MmxPackedValue64 coefficientPairProducts27;
  MmxPackedValue64 coefficientPairProducts30;
  MmxPackedValue64 coefficientPairProducts29;
  MmxPackedValue64 coefficientPairProducts32;
  MmxPackedValue64 coefficientPairProducts31;
  MmxPackedValue64 coefficientPairProducts34;
  MmxPackedValue64 coefficientPairProducts33;
  MmxPackedValue64 coefficientPairProducts20;
  MmxPackedValue64 coefficientPairProducts19;
  MmxPackedValue64 coefficientPairProducts22;
  MmxPackedValue64 coefficientPairProducts21;
  MmxPackedValue64 coefficientPairProducts24;
  MmxPackedValue64 coefficientPairProducts23;
  MmxPackedValue64 coefficientPairProducts26;
  MmxPackedValue64 coefficientPairProducts25;
  MmxPackedValue64 coefficientPairProducts44;
  MmxPackedValue64 coefficientPairProducts43;
  MmxPackedValue64 coefficientPairProducts46;
  MmxPackedValue64 coefficientPairProducts45;
  MmxPackedValue64 coefficientPairProducts48;
  MmxPackedValue64 coefficientPairProducts47;
  MmxPackedValue64 coefficientPairProducts50;
  MmxPackedValue64 coefficientPairProducts49;
  MmxPackedValue64 coefficientPairProducts36;
  MmxPackedValue64 coefficientPairProducts35;
  MmxPackedValue64 coefficientPairProducts38;
  MmxPackedValue64 coefficientPairProducts37;
  MmxPackedValue64 coefficientPairProducts40;
  MmxPackedValue64 coefficientPairProducts39;
  MmxPackedValue64 coefficientPairProducts42;
  MmxPackedValue64 coefficientPairProducts41;
  MmxPackedValue64 coefficientPairProducts60;
  MmxPackedValue64 coefficientPairProducts59;
  MmxPackedValue64 coefficientPairProducts62;
  MmxPackedValue64 coefficientPairProducts61;
  MmxPackedValue64 coefficientPairProducts64;
  MmxPackedValue64 coefficientPairProducts63;
  MmxPackedValue64 coefficientPairProducts66;
  MmxPackedValue64 coefficientPairProducts65;
  MmxPackedValue64 coefficientPairProducts52;
  MmxPackedValue64 coefficientPairProducts51;
  MmxPackedValue64 coefficientPairProducts54;
  MmxPackedValue64 coefficientPairProducts53;
  MmxPackedValue64 coefficientPairProducts56;
  MmxPackedValue64 coefficientPairProducts55;
  MmxPackedValue64 coefficientPairProducts58;
  MmxPackedValue64 coefficientPairProducts57;
  MmxPackedValue64 coefficientPairProducts76;
  MmxPackedValue64 coefficientPairProducts75;
  MmxPackedValue64 coefficientPairProducts78;
  MmxPackedValue64 coefficientPairProducts77;
  MmxPackedValue64 coefficientPairProducts80;
  MmxPackedValue64 coefficientPairProducts79;
  MmxPackedValue64 coefficientPairProducts82;
  MmxPackedValue64 coefficientPairProducts81;
  MmxPackedValue64 coefficientPairProducts68;
  MmxPackedValue64 coefficientPairProducts67;
  MmxPackedValue64 coefficientPairProducts70;
  MmxPackedValue64 coefficientPairProducts69;
  MmxPackedValue64 coefficientPairProducts72;
  MmxPackedValue64 coefficientPairProducts71;
  MmxPackedValue64 coefficientPairProducts74;
  MmxPackedValue64 coefficientPairProducts73;
  MmxPackedValue64 coefficientPairProducts202;
  MmxPackedValue64 coefficientPairProducts201;
  MmxPackedValue64 coefficientPairProducts204;
  MmxPackedValue64 coefficientPairProducts203;
  MmxPackedValue64 coefficientPairProducts206;
  MmxPackedValue64 coefficientPairProducts205;
  MmxPackedValue64 coefficientPairProducts208;
  MmxPackedValue64 coefficientPairProducts207;
  MmxPackedValue64 coefficientPairProducts194;
  MmxPackedValue64 coefficientPairProducts193;
  MmxPackedValue64 coefficientPairProducts196;
  MmxPackedValue64 coefficientPairProducts195;
  MmxPackedValue64 coefficientPairProducts198;
  MmxPackedValue64 coefficientPairProducts197;
  MmxPackedValue64 coefficientPairProducts200;
  MmxPackedValue64 coefficientPairProducts199;
  MmxPackedValue64 coefficientPairProducts218;
  MmxPackedValue64 coefficientPairProducts217;
  MmxPackedValue64 coefficientPairProducts220;
  MmxPackedValue64 coefficientPairProducts219;
  MmxPackedValue64 coefficientPairProducts222;
  MmxPackedValue64 coefficientPairProducts221;
  MmxPackedValue64 coefficientPairProducts224;
  MmxPackedValue64 coefficientPairProducts223;
  MmxPackedValue64 coefficientPairProducts210;
  MmxPackedValue64 coefficientPairProducts209;
  MmxPackedValue64 coefficientPairProducts212;
  MmxPackedValue64 coefficientPairProducts211;
  MmxPackedValue64 coefficientPairProducts214;
  MmxPackedValue64 coefficientPairProducts213;
  MmxPackedValue64 coefficientPairProducts216;
  MmxPackedValue64 coefficientPairProducts215;
  MmxPackedValue64 coefficientPairProducts234;
  MmxPackedValue64 coefficientPairProducts233;
  MmxPackedValue64 coefficientPairProducts236;
  MmxPackedValue64 coefficientPairProducts235;
  MmxPackedValue64 coefficientPairProducts238;
  MmxPackedValue64 coefficientPairProducts237;
  MmxPackedValue64 coefficientPairProducts240;
  MmxPackedValue64 coefficientPairProducts239;
  MmxPackedValue64 coefficientPairProducts226;
  MmxPackedValue64 coefficientPairProducts225;
  MmxPackedValue64 coefficientPairProducts228;
  MmxPackedValue64 coefficientPairProducts227;
  MmxPackedValue64 coefficientPairProducts230;
  MmxPackedValue64 coefficientPairProducts229;
  MmxPackedValue64 coefficientPairProducts232;
  MmxPackedValue64 coefficientPairProducts231;
  MmxPackedValue64 coefficientPairProducts250;
  MmxPackedValue64 coefficientPairProducts249;
  MmxPackedValue64 coefficientPairProducts252;
  MmxPackedValue64 coefficientPairProducts251;
  MmxPackedValue64 coefficientPairProducts254;
  MmxPackedValue64 coefficientPairProducts253;
  MmxPackedValue64 coefficientPairProducts256;
  MmxPackedValue64 coefficientPairProducts255;
  MmxPackedValue64 coefficientPairProducts242;
  MmxPackedValue64 coefficientPairProducts241;
  MmxPackedValue64 coefficientPairProducts244;
  MmxPackedValue64 coefficientPairProducts243;
  MmxPackedValue64 coefficientPairProducts246;
  MmxPackedValue64 coefficientPairProducts245;
  MmxPackedValue64 coefficientPairProducts248;
  MmxPackedValue64 coefficientPairProducts247;
  MmxPackedValue64 coefficientPairProducts138;
  MmxPackedValue64 coefficientPairProducts137;
  MmxPackedValue64 coefficientPairProducts140;
  MmxPackedValue64 coefficientPairProducts139;
  MmxPackedValue64 coefficientPairProducts142;
  MmxPackedValue64 coefficientPairProducts141;
  MmxPackedValue64 coefficientPairProducts144;
  MmxPackedValue64 coefficientPairProducts143;
  MmxPackedValue64 coefficientPairProducts130;
  MmxPackedValue64 coefficientPairProducts129;
  MmxPackedValue64 coefficientPairProducts132;
  MmxPackedValue64 coefficientPairProducts131;
  MmxPackedValue64 coefficientPairProducts134;
  MmxPackedValue64 coefficientPairProducts133;
  MmxPackedValue64 coefficientPairProducts136;
  MmxPackedValue64 coefficientPairProducts135;
  MmxPackedValue64 coefficientPairProducts154;
  MmxPackedValue64 coefficientPairProducts153;
  MmxPackedValue64 coefficientPairProducts156;
  MmxPackedValue64 coefficientPairProducts155;
  MmxPackedValue64 coefficientPairProducts158;
  MmxPackedValue64 coefficientPairProducts157;
  MmxPackedValue64 coefficientPairProducts160;
  MmxPackedValue64 coefficientPairProducts159;
  MmxPackedValue64 coefficientPairProducts146;
  MmxPackedValue64 coefficientPairProducts145;
  MmxPackedValue64 coefficientPairProducts148;
  MmxPackedValue64 coefficientPairProducts147;
  MmxPackedValue64 coefficientPairProducts150;
  MmxPackedValue64 coefficientPairProducts149;
  MmxPackedValue64 coefficientPairProducts152;
  MmxPackedValue64 coefficientPairProducts151;
  MmxPackedValue64 coefficientPairProducts170;
  MmxPackedValue64 coefficientPairProducts169;
  MmxPackedValue64 coefficientPairProducts172;
  MmxPackedValue64 coefficientPairProducts171;
  MmxPackedValue64 coefficientPairProducts174;
  MmxPackedValue64 coefficientPairProducts173;
  MmxPackedValue64 coefficientPairProducts176;
  MmxPackedValue64 coefficientPairProducts175;
  MmxPackedValue64 coefficientPairProducts162;
  MmxPackedValue64 coefficientPairProducts161;
  MmxPackedValue64 coefficientPairProducts164;
  MmxPackedValue64 coefficientPairProducts163;
  MmxPackedValue64 coefficientPairProducts166;
  MmxPackedValue64 coefficientPairProducts165;
  MmxPackedValue64 coefficientPairProducts168;
  MmxPackedValue64 coefficientPairProducts167;
  MmxPackedValue64 coefficientPairProducts186;
  MmxPackedValue64 coefficientPairProducts185;
  MmxPackedValue64 coefficientPairProducts188;
  MmxPackedValue64 coefficientPairProducts187;
  MmxPackedValue64 coefficientPairProducts190;
  MmxPackedValue64 coefficientPairProducts189;
  MmxPackedValue64 coefficientPairProducts192;
  MmxPackedValue64 coefficientPairProducts191;
  MmxPackedValue64 coefficientPairProducts178;
  MmxPackedValue64 coefficientPairProducts177;
  MmxPackedValue64 coefficientPairProducts180;
  MmxPackedValue64 coefficientPairProducts179;
  MmxPackedValue64 coefficientPairProducts182;
  MmxPackedValue64 coefficientPairProducts181;
  MmxPackedValue64 coefficientPairProducts184;
  MmxPackedValue64 coefficientPairProducts183;
  MmxPackedValue64 coefficientPairProducts10;
  MmxPackedValue64 coefficientPairProducts9;
  MmxPackedValue64 coefficientPairProducts12;
  MmxPackedValue64 coefficientPairProducts11;
  MmxPackedValue64 coefficientPairProducts14;
  MmxPackedValue64 coefficientPairProducts13;
  MmxPackedValue64 coefficientPairProducts16;
  MmxPackedValue64 coefficientPairProducts15;
  MmxPackedValue64 coefficientPairProducts2;
  MmxPackedValue64 coefficientPairProducts1;
  MmxPackedValue64 coefficientPairProducts4;
  MmxPackedValue64 coefficientPairProducts3;
  MmxPackedValue64 coefficientPairProducts6;
  MmxPackedValue64 coefficientPairProducts5;
  MmxPackedValue64 coefficientPairProducts8;
  MmxPackedValue64 coefficientPairProducts7;
  MmxPackedValue64 coefficientPairProducts18;
  MmxPackedValue64 coefficientPairProducts17;
  int bank0HighLaneSum;
  int bank1HighLaneSum;
  int bank2LowLaneSum;
  int bank3LowLaneSum;
  int outputPassesRemaining;
  
  outputPassesRemaining = SAM_BLOCK_SAMPLE_COUNT / SAM_MMX_OUTPUTS_PER_PASS;
  cosineRowCursor = g_CosineDerivedLookupSecondTable;
  do {
    /* One pass = output samples m..m+3: all 256 coefficients, 8 at a time, are multiplied pairwise (PMADDWD)
       with the cosine rows at cosineRowCursor + 0, + 0x100, + 0x200 and + 0x300. */
    coefficientQuadLow = *(MmxPackedValue64 *)coefficientBlock->coefficients;
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 4);
    coefficientPairProducts46 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)cosineRowCursor);
    coefficientPairProducts252 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x100));
    coefficientPairProducts45 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 4));
    coefficientPairProducts251 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x104));
    coefficientPairProducts91 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x200));
    coefficientPairProducts124 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x204));
    coefficientPairProducts48 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x300));
    coefficientPairProducts254 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x304));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xc);
    coefficientPairProducts47 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 8));
    coefficientPairProducts253 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x108));
    coefficientPairProducts50 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xc));
    coefficientPairProducts256 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x10c));
    coefficientPairProducts93 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x208));
    coefficientPairProducts123 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x20c));
    coefficientPairProducts49 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x308));
    coefficientPairProducts255 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x30c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x10);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x14);
    coefficientPairProducts36 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x10));
    coefficientPairProducts242 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x110));
    coefficientPairProducts35 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x14));
    coefficientPairProducts241 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x114));
    coefficientPairProducts92 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x210));
    coefficientPairProducts126 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x214));
    coefficientPairProducts38 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x310));
    coefficientPairProducts244 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x314));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x18);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x1c);
    coefficientPairProducts37 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x18));
    coefficientPairProducts243 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x118));
    coefficientPairProducts40 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c));
    coefficientPairProducts246 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x11c));
    coefficientPairProducts95 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x218));
    coefficientPairProducts125 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x21c));
    coefficientPairProducts39 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x318));
    coefficientPairProducts245 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x31c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x20);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x24);
    coefficientPairProducts42 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x20));
    coefficientPairProducts248 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x120));
    coefficientPairProducts41 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x24));
    coefficientPairProducts247 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x124));
    coefficientPairProducts94 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x220));
    coefficientPairProducts128 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x224));
    coefficientPairProducts60 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 800));
    coefficientPairProducts138 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x324));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x28);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x2c);
    coefficientPairProducts59 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x28));
    coefficientPairProducts137 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x128));
    coefficientPairProducts62 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c));
    coefficientPairProducts140 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 300));
    coefficientPairProducts97 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x228));
    coefficientPairProducts127 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x22c));
    coefficientPairProducts61 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x328));
    coefficientPairProducts139 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x32c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x30);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x34);
    coefficientPairProducts64 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x30));
    coefficientPairProducts142 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x130));
    coefficientPairProducts63 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x34));
    coefficientPairProducts141 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x134));
    coefficientPairProducts96 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x230));
    coefficientPairProducts115 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x234));
    coefficientPairProducts66 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x330));
    coefficientPairProducts144 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x334));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x38);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x3c);
    coefficientPairProducts65 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x38));
    coefficientPairProducts143 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x138));
    coefficientPairProducts52 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c));
    coefficientPairProducts130 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x13c));
    coefficientPairProducts84 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x238));
    coefficientPairProducts114 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x23c));
    coefficientPairProducts51 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x338));
    coefficientPairProducts129 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x33c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x40);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x44);
    coefficientPairProducts54 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x40));
    coefficientPairProducts132 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x140));
    coefficientPairProducts53 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x44));
    coefficientPairProducts131 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x144));
    coefficientPairProducts83 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x240));
    coefficientPairProducts117 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x244));
    coefficientPairProducts56 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x340));
    coefficientPairProducts134 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x344));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x48);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x4c);
    coefficientPairProducts55 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x48));
    coefficientPairProducts133 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x148));
    coefficientPairProducts58 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x4c));
    coefficientPairProducts136 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x14c));
    coefficientPairProducts86 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x248));
    coefficientPairProducts116 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x24c));
    coefficientPairProducts57 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x348));
    coefficientPairProducts135 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x34c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x50);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x54);
    coefficientPairProducts76 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x50));
    coefficientPairProducts154 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x150));
    coefficientPairProducts75 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x54));
    coefficientPairProducts153 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x154));
    coefficientPairProducts85 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x250));
    coefficientPairProducts119 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x254));
    coefficientPairProducts78 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x350));
    coefficientPairProducts156 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x354));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x58);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x5c);
    coefficientPairProducts77 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x58));
    coefficientPairProducts155 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x158));
    coefficientPairProducts80 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x5c));
    coefficientPairProducts158 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x15c));
    coefficientPairProducts88 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 600));
    coefficientPairProducts118 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x25c));
    coefficientPairProducts79 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x358));
    coefficientPairProducts157 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x35c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x60);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 100);
    coefficientPairProducts82 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x60));
    coefficientPairProducts160 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x160));
    coefficientPairProducts81 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 100));
    coefficientPairProducts159 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x164));
    coefficientPairProducts87 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x260));
    coefficientPairProducts121 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x264));
    coefficientPairProducts68 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x360));
    coefficientPairProducts146 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x364));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x68);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x6c);
    coefficientPairProducts67 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x68));
    coefficientPairProducts145 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x168));
    coefficientPairProducts70 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x6c));
    coefficientPairProducts148 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x16c));
    coefficientPairProducts90 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x268));
    coefficientPairProducts120 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x26c));
    coefficientPairProducts69 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x368));
    coefficientPairProducts147 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x36c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x70);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x74);
    coefficientPairProducts72 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x70));
    coefficientPairProducts150 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x170));
    coefficientPairProducts71 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x74));
    coefficientPairProducts149 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x174));
    coefficientPairProducts89 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x270));
    coefficientPairProducts28 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x274));
    coefficientPairProducts74 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x370));
    coefficientPairProducts152 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x374));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x78);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x7c);
    coefficientPairProducts73 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x78));
    coefficientPairProducts151 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x178));
    coefficientPairProducts202 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x7c));
    coefficientPairProducts170 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x17c));
    coefficientPairProducts107 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x278));
    coefficientPairProducts27 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x27c));
    coefficientPairProducts201 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x378));
    coefficientPairProducts169 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x37c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x80);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x84);
    coefficientPairProducts204 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x80));
    coefficientPairProducts172 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x180));
    coefficientPairProducts203 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x84));
    coefficientPairProducts171 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x184));
    coefficientPairProducts106 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x280));
    coefficientPairProducts30 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x284));
    coefficientPairProducts206 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x380));
    coefficientPairProducts174 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 900));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x88);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x8c);
    coefficientPairProducts205 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x88));
    coefficientPairProducts173 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x188));
    coefficientPairProducts208 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x8c));
    coefficientPairProducts176 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x18c));
    coefficientPairProducts109 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x288));
    coefficientPairProducts29 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x28c));
    coefficientPairProducts207 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x388));
    coefficientPairProducts175 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x38c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x90);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x94);
    coefficientPairProducts194 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x90));
    coefficientPairProducts162 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 400));
    coefficientPairProducts193 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x94));
    coefficientPairProducts161 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x194));
    coefficientPairProducts108 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x290));
    coefficientPairProducts32 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x294));
    coefficientPairProducts196 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x390));
    coefficientPairProducts164 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x394));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x98);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0x9c);
    coefficientPairProducts195 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x98));
    coefficientPairProducts163 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x198));
    coefficientPairProducts198 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x9c));
    coefficientPairProducts166 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x19c));
    coefficientPairProducts111 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x298));
    coefficientPairProducts31 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x29c));
    coefficientPairProducts197 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x398));
    coefficientPairProducts165 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x39c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xa0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xa4);
    coefficientPairProducts200 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xa0));
    coefficientPairProducts168 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1a0));
    coefficientPairProducts199 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xa4));
    coefficientPairProducts167 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1a4));
    coefficientPairProducts110 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2a0));
    coefficientPairProducts34 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2a4));
    coefficientPairProducts218 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3a0));
    coefficientPairProducts186 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3a4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xa8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xac);
    coefficientPairProducts217 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xa8));
    coefficientPairProducts185 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1a8));
    coefficientPairProducts220 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xac));
    coefficientPairProducts188 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1ac));
    coefficientPairProducts113 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2a8));
    coefficientPairProducts33 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2ac));
    coefficientPairProducts219 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3a8));
    coefficientPairProducts187 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3ac));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xb0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xb4);
    coefficientPairProducts222 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xb0));
    coefficientPairProducts190 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1b0));
    coefficientPairProducts221 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xb4));
    coefficientPairProducts189 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1b4));
    coefficientPairProducts112 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2b0));
    coefficientPairProducts20 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2b4));
    coefficientPairProducts224 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3b0));
    coefficientPairProducts192 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3b4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xb8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xbc);
    coefficientPairProducts223 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xb8));
    coefficientPairProducts191 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1b8));
    coefficientPairProducts210 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xbc));
    coefficientPairProducts178 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1bc));
    coefficientPairProducts99 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2b8));
    coefficientPairProducts19 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 700));
    coefficientPairProducts209 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3b8));
    coefficientPairProducts177 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3bc));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xc0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xc4);
    coefficientPairProducts212 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xc0));
    coefficientPairProducts180 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c0));
    coefficientPairProducts211 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xc4));
    coefficientPairProducts179 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c4));
    coefficientPairProducts98 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c0));
    coefficientPairProducts22 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c4));
    coefficientPairProducts214 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c0));
    coefficientPairProducts182 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 200);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xcc);
    coefficientPairProducts213 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 200));
    coefficientPairProducts181 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1c8));
    coefficientPairProducts216 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xcc));
    coefficientPairProducts184 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1cc));
    coefficientPairProducts101 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2c8));
    coefficientPairProducts21 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2cc));
    coefficientPairProducts215 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3c8));
    coefficientPairProducts183 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3cc));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xd0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xd4);
    coefficientPairProducts234 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xd0));
    coefficientPairProducts10 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1d0));
    coefficientPairProducts233 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xd4));
    coefficientPairProducts9 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1d4));
    coefficientPairProducts100 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2d0));
    coefficientPairProducts24 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2d4));
    coefficientPairProducts236 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3d0));
    coefficientPairProducts12 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3d4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xd8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xdc);
    coefficientPairProducts235 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xd8));
    coefficientPairProducts11 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1d8));
    coefficientPairProducts238 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xdc));
    coefficientPairProducts14 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1dc));
    coefficientPairProducts103 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2d8));
    coefficientPairProducts23 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2dc));
    coefficientPairProducts237 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3d8));
    coefficientPairProducts13 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3dc));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xe0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xe4);
    coefficientPairProducts240 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xe0));
    coefficientPairProducts16 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1e0));
    coefficientPairProducts239 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xe4));
    coefficientPairProducts15 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1e4));
    coefficientPairProducts102 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2e0));
    coefficientPairProducts26 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2e4));
    coefficientPairProducts226 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3e0));
    coefficientPairProducts2 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3e4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xe8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xec);
    coefficientPairProducts225 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xe8));
    coefficientPairProducts1 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1e8));
    coefficientPairProducts228 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xec));
    coefficientPairProducts4 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1ec));
    coefficientPairProducts105 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2e8));
    coefficientPairProducts25 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2ec));
    coefficientPairProducts227 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 1000));
    coefficientPairProducts3 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3ec));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xf0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xf4);
    coefficientPairProducts230 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xf0));
    coefficientPairProducts6 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1f0));
    coefficientPairProducts229 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xf4));
    coefficientPairProducts5 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 500));
    coefficientPairProducts104 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2f0));
    coefficientPairProducts44 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2f4));
    coefficientPairProducts232 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3f0));
    coefficientPairProducts8 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3f4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xf8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficientBlock->coefficients + 0xfc);
    coefficientPairProducts231 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0xf8));
    coefficientPairProducts7 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x1f8));
    coefficientPairProducts250 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0xfc));
    coefficientPairProducts18 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x1fc));
    /* Horizontal sums: the original accumulates each row in one MMX register (PADDD, 32-bit wrapping) and
       adds its two 32-bit lanes together at the end; bankN{High,Low}LaneSum is the partial sum of one lane of
       row N. */
    bank0HighLaneSum = (int)(coefficientPairProducts46 >> 0x20) + (int)(coefficientPairProducts45 >> 0x20) +
            (int)(coefficientPairProducts47 >> 0x20) + (int)(coefficientPairProducts50 >> 0x20) +
            (int)(coefficientPairProducts36 >> 0x20) + (int)(coefficientPairProducts35 >> 0x20) +
            (int)(coefficientPairProducts37 >> 0x20) + (int)(coefficientPairProducts40 >> 0x20) +
            (int)(coefficientPairProducts42 >> 0x20) + (int)(coefficientPairProducts41 >> 0x20) +
            (int)(coefficientPairProducts59 >> 0x20) + (int)(coefficientPairProducts62 >> 0x20) +
            (int)(coefficientPairProducts64 >> 0x20) + (int)(coefficientPairProducts63 >> 0x20) +
            (int)(coefficientPairProducts65 >> 0x20) + (int)(coefficientPairProducts52 >> 0x20) +
            (int)(coefficientPairProducts54 >> 0x20) + (int)(coefficientPairProducts53 >> 0x20) +
            (int)(coefficientPairProducts55 >> 0x20) + (int)(coefficientPairProducts58 >> 0x20) +
            (int)(coefficientPairProducts76 >> 0x20) + (int)(coefficientPairProducts75 >> 0x20) +
            (int)(coefficientPairProducts77 >> 0x20) + (int)(coefficientPairProducts80 >> 0x20) +
            (int)(coefficientPairProducts82 >> 0x20) + (int)(coefficientPairProducts81 >> 0x20) +
            (int)(coefficientPairProducts67 >> 0x20) + (int)(coefficientPairProducts70 >> 0x20) +
            (int)(coefficientPairProducts72 >> 0x20) + (int)(coefficientPairProducts71 >> 0x20) +
            (int)(coefficientPairProducts73 >> 0x20) + (int)(coefficientPairProducts202 >> 0x20) +
            (int)(coefficientPairProducts204 >> 0x20) + (int)(coefficientPairProducts203 >> 0x20) +
            (int)(coefficientPairProducts205 >> 0x20) + (int)(coefficientPairProducts208 >> 0x20) +
            (int)(coefficientPairProducts194 >> 0x20) + (int)(coefficientPairProducts193 >> 0x20) +
            (int)(coefficientPairProducts195 >> 0x20) + (int)(coefficientPairProducts198 >> 0x20) +
            (int)(coefficientPairProducts200 >> 0x20) + (int)(coefficientPairProducts199 >> 0x20) +
            (int)(coefficientPairProducts217 >> 0x20) + (int)(coefficientPairProducts220 >> 0x20) +
            (int)(coefficientPairProducts222 >> 0x20) + (int)(coefficientPairProducts221 >> 0x20) +
            (int)(coefficientPairProducts223 >> 0x20) + (int)(coefficientPairProducts210 >> 0x20) +
            (int)(coefficientPairProducts212 >> 0x20) + (int)(coefficientPairProducts211 >> 0x20) +
            (int)(coefficientPairProducts213 >> 0x20) + (int)(coefficientPairProducts216 >> 0x20) +
            (int)(coefficientPairProducts234 >> 0x20) + (int)(coefficientPairProducts233 >> 0x20) +
            (int)(coefficientPairProducts235 >> 0x20) + (int)(coefficientPairProducts238 >> 0x20) +
            (int)(coefficientPairProducts240 >> 0x20) + (int)(coefficientPairProducts239 >> 0x20) +
            (int)(coefficientPairProducts225 >> 0x20) + (int)(coefficientPairProducts228 >> 0x20) +
            (int)(coefficientPairProducts230 >> 0x20) + (int)(coefficientPairProducts229 >> 0x20) +
            (int)(coefficientPairProducts231 >> 0x20) + (int)(coefficientPairProducts250 >> 0x20);
    bank1HighLaneSum = (int)(coefficientPairProducts252 >> 0x20) + (int)(coefficientPairProducts251 >> 0x20) +
            (int)(coefficientPairProducts253 >> 0x20) + (int)(coefficientPairProducts256 >> 0x20) +
            (int)(coefficientPairProducts242 >> 0x20) + (int)(coefficientPairProducts241 >> 0x20) +
            (int)(coefficientPairProducts243 >> 0x20) + (int)(coefficientPairProducts246 >> 0x20) +
            (int)(coefficientPairProducts248 >> 0x20) + (int)(coefficientPairProducts247 >> 0x20) +
            (int)(coefficientPairProducts137 >> 0x20) + (int)(coefficientPairProducts140 >> 0x20) +
            (int)(coefficientPairProducts142 >> 0x20) + (int)(coefficientPairProducts141 >> 0x20) +
            (int)(coefficientPairProducts143 >> 0x20) + (int)(coefficientPairProducts130 >> 0x20) +
            (int)(coefficientPairProducts132 >> 0x20) + (int)(coefficientPairProducts131 >> 0x20) +
            (int)(coefficientPairProducts133 >> 0x20) + (int)(coefficientPairProducts136 >> 0x20) +
            (int)(coefficientPairProducts154 >> 0x20) + (int)(coefficientPairProducts153 >> 0x20) +
            (int)(coefficientPairProducts155 >> 0x20) + (int)(coefficientPairProducts158 >> 0x20) +
            (int)(coefficientPairProducts160 >> 0x20) + (int)(coefficientPairProducts159 >> 0x20) +
            (int)(coefficientPairProducts145 >> 0x20) + (int)(coefficientPairProducts148 >> 0x20) +
            (int)(coefficientPairProducts150 >> 0x20) + (int)(coefficientPairProducts149 >> 0x20) +
            (int)(coefficientPairProducts151 >> 0x20) + (int)(coefficientPairProducts170 >> 0x20) +
            (int)(coefficientPairProducts172 >> 0x20) + (int)(coefficientPairProducts171 >> 0x20) +
            (int)(coefficientPairProducts173 >> 0x20) + (int)(coefficientPairProducts176 >> 0x20) +
            (int)(coefficientPairProducts162 >> 0x20) + (int)(coefficientPairProducts161 >> 0x20) +
            (int)(coefficientPairProducts163 >> 0x20) + (int)(coefficientPairProducts166 >> 0x20) +
            (int)(coefficientPairProducts168 >> 0x20) + (int)(coefficientPairProducts167 >> 0x20) +
            (int)(coefficientPairProducts185 >> 0x20) + (int)(coefficientPairProducts188 >> 0x20) +
            (int)(coefficientPairProducts190 >> 0x20) + (int)(coefficientPairProducts189 >> 0x20) +
            (int)(coefficientPairProducts191 >> 0x20) + (int)(coefficientPairProducts178 >> 0x20) +
            (int)(coefficientPairProducts180 >> 0x20) + (int)(coefficientPairProducts179 >> 0x20) +
            (int)(coefficientPairProducts181 >> 0x20) + (int)(coefficientPairProducts184 >> 0x20) +
            (int)(coefficientPairProducts10 >> 0x20) + (int)(coefficientPairProducts9 >> 0x20) +
            (int)(coefficientPairProducts11 >> 0x20) + (int)(coefficientPairProducts14 >> 0x20) +
            (int)(coefficientPairProducts16 >> 0x20) + (int)(coefficientPairProducts15 >> 0x20) +
            (int)(coefficientPairProducts1 >> 0x20) + (int)(coefficientPairProducts4 >> 0x20) +
            (int)(coefficientPairProducts6 >> 0x20) + (int)(coefficientPairProducts5 >> 0x20) +
            (int)(coefficientPairProducts7 >> 0x20) + (int)(coefficientPairProducts18 >> 0x20);
    coefficientPairProducts122 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x2f8));
    coefficientPairProducts43 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x2fc));
    coefficientPairProducts249 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineRowCursor + 0x3f8));
    coefficientPairProducts17 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineRowCursor + 0x3fc));
    bank2LowLaneSum = (int)coefficientPairProducts91 + (int)coefficientPairProducts124 +
            (int)coefficientPairProducts93 + (int)coefficientPairProducts123 +
            (int)coefficientPairProducts92 + (int)coefficientPairProducts126 +
            (int)coefficientPairProducts95 + (int)coefficientPairProducts125 +
            (int)coefficientPairProducts94 + (int)coefficientPairProducts128 +
            (int)coefficientPairProducts97 + (int)coefficientPairProducts127 +
            (int)coefficientPairProducts96 + (int)coefficientPairProducts115 +
            (int)coefficientPairProducts84 + (int)coefficientPairProducts114 +
            (int)coefficientPairProducts83 + (int)coefficientPairProducts117 +
            (int)coefficientPairProducts86 + (int)coefficientPairProducts116 +
            (int)coefficientPairProducts85 + (int)coefficientPairProducts119 +
            (int)coefficientPairProducts88 + (int)coefficientPairProducts118 +
            (int)coefficientPairProducts87 + (int)coefficientPairProducts121 +
            (int)coefficientPairProducts90 + (int)coefficientPairProducts120 +
            (int)coefficientPairProducts89 + (int)coefficientPairProducts28 +
            (int)coefficientPairProducts107 + (int)coefficientPairProducts27 +
            (int)coefficientPairProducts106 + (int)coefficientPairProducts30 +
            (int)coefficientPairProducts109 + (int)coefficientPairProducts29 +
            (int)coefficientPairProducts108 + (int)coefficientPairProducts32 +
            (int)coefficientPairProducts111 + (int)coefficientPairProducts31 +
            (int)coefficientPairProducts110 + (int)coefficientPairProducts34 +
            (int)coefficientPairProducts113 + (int)coefficientPairProducts33 +
            (int)coefficientPairProducts112 + (int)coefficientPairProducts20 +
            (int)coefficientPairProducts99 + (int)coefficientPairProducts19 +
            (int)coefficientPairProducts98 + (int)coefficientPairProducts22 +
            (int)coefficientPairProducts101 + (int)coefficientPairProducts21 +
            (int)coefficientPairProducts100 + (int)coefficientPairProducts24 +
            (int)coefficientPairProducts103 + (int)coefficientPairProducts23 +
            (int)coefficientPairProducts102 + (int)coefficientPairProducts26 +
            (int)coefficientPairProducts105 + (int)coefficientPairProducts25 +
            (int)coefficientPairProducts104 + (int)coefficientPairProducts44 +
            (int)coefficientPairProducts122 + (int)coefficientPairProducts43;
    bank3LowLaneSum = (int)coefficientPairProducts48 + (int)coefficientPairProducts254 +
            (int)coefficientPairProducts49 + (int)coefficientPairProducts255 +
            (int)coefficientPairProducts38 + (int)coefficientPairProducts244 +
            (int)coefficientPairProducts39 + (int)coefficientPairProducts245 +
            (int)coefficientPairProducts60 + (int)coefficientPairProducts138 +
            (int)coefficientPairProducts61 + (int)coefficientPairProducts139 +
            (int)coefficientPairProducts66 + (int)coefficientPairProducts144 +
            (int)coefficientPairProducts51 + (int)coefficientPairProducts129 +
            (int)coefficientPairProducts56 + (int)coefficientPairProducts134 +
            (int)coefficientPairProducts57 + (int)coefficientPairProducts135 +
            (int)coefficientPairProducts78 + (int)coefficientPairProducts156 +
            (int)coefficientPairProducts79 + (int)coefficientPairProducts157 +
            (int)coefficientPairProducts68 + (int)coefficientPairProducts146 +
            (int)coefficientPairProducts69 + (int)coefficientPairProducts147 +
            (int)coefficientPairProducts74 + (int)coefficientPairProducts152 +
            (int)coefficientPairProducts201 + (int)coefficientPairProducts169 +
            (int)coefficientPairProducts206 + (int)coefficientPairProducts174 +
            (int)coefficientPairProducts207 + (int)coefficientPairProducts175 +
            (int)coefficientPairProducts196 + (int)coefficientPairProducts164 +
            (int)coefficientPairProducts197 + (int)coefficientPairProducts165 +
            (int)coefficientPairProducts218 + (int)coefficientPairProducts186 +
            (int)coefficientPairProducts219 + (int)coefficientPairProducts187 +
            (int)coefficientPairProducts224 + (int)coefficientPairProducts192 +
            (int)coefficientPairProducts209 + (int)coefficientPairProducts177 +
            (int)coefficientPairProducts214 + (int)coefficientPairProducts182 +
            (int)coefficientPairProducts215 + (int)coefficientPairProducts183 +
            (int)coefficientPairProducts236 + (int)coefficientPairProducts12 +
            (int)coefficientPairProducts237 + (int)coefficientPairProducts13 +
            (int)coefficientPairProducts226 + (int)coefficientPairProducts2 +
            (int)coefficientPairProducts227 + (int)coefficientPairProducts3 +
            (int)coefficientPairProducts232 + (int)coefficientPairProducts8 +
            (int)coefficientPairProducts249 + (int)coefficientPairProducts17;
    /* Each row total x32 (PSLLD 5); its bits 16..31 go to word lane N of the output qword (lane masks 0..3). */
    packedOutputWords =
         SAM_PACK_LANE_PAIR(bank0HighLaneSum * 0x20,
                  (bank0HighLaneSum + (int)coefficientPairProducts46 + (int)coefficientPairProducts45 +
                           (int)coefficientPairProducts47 + (int)coefficientPairProducts50 +
                           (int)coefficientPairProducts36 + (int)coefficientPairProducts35 +
                           (int)coefficientPairProducts37 + (int)coefficientPairProducts40 +
                           (int)coefficientPairProducts42 + (int)coefficientPairProducts41 +
                           (int)coefficientPairProducts59 + (int)coefficientPairProducts62 +
                           (int)coefficientPairProducts64 + (int)coefficientPairProducts63 +
                           (int)coefficientPairProducts65 + (int)coefficientPairProducts52 +
                           (int)coefficientPairProducts54 + (int)coefficientPairProducts53 +
                           (int)coefficientPairProducts55 + (int)coefficientPairProducts58 +
                           (int)coefficientPairProducts76 + (int)coefficientPairProducts75 +
                           (int)coefficientPairProducts77 + (int)coefficientPairProducts80 +
                           (int)coefficientPairProducts82 + (int)coefficientPairProducts81 +
                           (int)coefficientPairProducts67 + (int)coefficientPairProducts70 +
                           (int)coefficientPairProducts72 + (int)coefficientPairProducts71 +
                           (int)coefficientPairProducts73 + (int)coefficientPairProducts202 +
                           (int)coefficientPairProducts204 + (int)coefficientPairProducts203 +
                           (int)coefficientPairProducts205 + (int)coefficientPairProducts208 +
                           (int)coefficientPairProducts194 + (int)coefficientPairProducts193 +
                           (int)coefficientPairProducts195 + (int)coefficientPairProducts198 +
                           (int)coefficientPairProducts200 + (int)coefficientPairProducts199 +
                           (int)coefficientPairProducts217 + (int)coefficientPairProducts220 +
                           (int)coefficientPairProducts222 + (int)coefficientPairProducts221 +
                           (int)coefficientPairProducts223 + (int)coefficientPairProducts210 +
                           (int)coefficientPairProducts212 + (int)coefficientPairProducts211 +
                           (int)coefficientPairProducts213 + (int)coefficientPairProducts216 +
                           (int)coefficientPairProducts234 + (int)coefficientPairProducts233 +
                           (int)coefficientPairProducts235 + (int)coefficientPairProducts238 +
                           (int)coefficientPairProducts240 + (int)coefficientPairProducts239 +
                           (int)coefficientPairProducts225 + (int)coefficientPairProducts228 +
                           (int)coefficientPairProducts230 + (int)coefficientPairProducts229 +
                           (int)coefficientPairProducts231 + (int)coefficientPairProducts250) * 0x20
                 ) >> 0x10 & g_SoundDecodeMmxWordLaneMask0 |
         SAM_PACK_LANE_PAIR(bank1HighLaneSum * 0x20,
                  (bank1HighLaneSum + (int)coefficientPairProducts252 + (int)coefficientPairProducts251 +
                           (int)coefficientPairProducts253 + (int)coefficientPairProducts256 +
                           (int)coefficientPairProducts242 + (int)coefficientPairProducts241 +
                           (int)coefficientPairProducts243 + (int)coefficientPairProducts246 +
                           (int)coefficientPairProducts248 + (int)coefficientPairProducts247 +
                           (int)coefficientPairProducts137 + (int)coefficientPairProducts140 +
                           (int)coefficientPairProducts142 + (int)coefficientPairProducts141 +
                           (int)coefficientPairProducts143 + (int)coefficientPairProducts130 +
                           (int)coefficientPairProducts132 + (int)coefficientPairProducts131 +
                           (int)coefficientPairProducts133 + (int)coefficientPairProducts136 +
                           (int)coefficientPairProducts154 + (int)coefficientPairProducts153 +
                           (int)coefficientPairProducts155 + (int)coefficientPairProducts158 +
                           (int)coefficientPairProducts160 + (int)coefficientPairProducts159 +
                           (int)coefficientPairProducts145 + (int)coefficientPairProducts148 +
                           (int)coefficientPairProducts150 + (int)coefficientPairProducts149 +
                           (int)coefficientPairProducts151 + (int)coefficientPairProducts170 +
                           (int)coefficientPairProducts172 + (int)coefficientPairProducts171 +
                           (int)coefficientPairProducts173 + (int)coefficientPairProducts176 +
                           (int)coefficientPairProducts162 + (int)coefficientPairProducts161 +
                           (int)coefficientPairProducts163 + (int)coefficientPairProducts166 +
                           (int)coefficientPairProducts168 + (int)coefficientPairProducts167 +
                           (int)coefficientPairProducts185 + (int)coefficientPairProducts188 +
                           (int)coefficientPairProducts190 + (int)coefficientPairProducts189 +
                           (int)coefficientPairProducts191 + (int)coefficientPairProducts178 +
                           (int)coefficientPairProducts180 + (int)coefficientPairProducts179 +
                           (int)coefficientPairProducts181 + (int)coefficientPairProducts184 +
                           (int)coefficientPairProducts10 + (int)coefficientPairProducts9 +
                           (int)coefficientPairProducts11 + (int)coefficientPairProducts14 +
                           (int)coefficientPairProducts16 + (int)coefficientPairProducts15 +
                           (int)coefficientPairProducts1 + (int)coefficientPairProducts4 +
                           (int)coefficientPairProducts6 + (int)coefficientPairProducts5 +
                           (int)coefficientPairProducts7 + (int)coefficientPairProducts18) * 0x20) &
         g_SoundDecodeMmxWordLaneMask1 |
         SAM_PACK_LANE_PAIR((bank3LowLaneSum + (int)(coefficientPairProducts48 >> 0x20) +
                           (int)(coefficientPairProducts254 >> 0x20) +
                           (int)(coefficientPairProducts49 >> 0x20) +
                           (int)(coefficientPairProducts255 >> 0x20) +
                           (int)(coefficientPairProducts38 >> 0x20) +
                           (int)(coefficientPairProducts244 >> 0x20) +
                           (int)(coefficientPairProducts39 >> 0x20) +
                           (int)(coefficientPairProducts245 >> 0x20) +
                           (int)(coefficientPairProducts60 >> 0x20) +
                           (int)(coefficientPairProducts138 >> 0x20) +
                           (int)(coefficientPairProducts61 >> 0x20) +
                           (int)(coefficientPairProducts139 >> 0x20) +
                           (int)(coefficientPairProducts66 >> 0x20) +
                           (int)(coefficientPairProducts144 >> 0x20) +
                           (int)(coefficientPairProducts51 >> 0x20) +
                           (int)(coefficientPairProducts129 >> 0x20) +
                           (int)(coefficientPairProducts56 >> 0x20) +
                           (int)(coefficientPairProducts134 >> 0x20) +
                           (int)(coefficientPairProducts57 >> 0x20) +
                           (int)(coefficientPairProducts135 >> 0x20) +
                           (int)(coefficientPairProducts78 >> 0x20) +
                           (int)(coefficientPairProducts156 >> 0x20) +
                           (int)(coefficientPairProducts79 >> 0x20) +
                           (int)(coefficientPairProducts157 >> 0x20) +
                           (int)(coefficientPairProducts68 >> 0x20) +
                           (int)(coefficientPairProducts146 >> 0x20) +
                           (int)(coefficientPairProducts69 >> 0x20) +
                           (int)(coefficientPairProducts147 >> 0x20) +
                           (int)(coefficientPairProducts74 >> 0x20) +
                           (int)(coefficientPairProducts152 >> 0x20) +
                           (int)(coefficientPairProducts201 >> 0x20) +
                           (int)(coefficientPairProducts169 >> 0x20) +
                           (int)(coefficientPairProducts206 >> 0x20) +
                           (int)(coefficientPairProducts174 >> 0x20) +
                           (int)(coefficientPairProducts207 >> 0x20) +
                           (int)(coefficientPairProducts175 >> 0x20) +
                           (int)(coefficientPairProducts196 >> 0x20) +
                           (int)(coefficientPairProducts164 >> 0x20) +
                           (int)(coefficientPairProducts197 >> 0x20) +
                           (int)(coefficientPairProducts165 >> 0x20) +
                           (int)(coefficientPairProducts218 >> 0x20) +
                           (int)(coefficientPairProducts186 >> 0x20) +
                           (int)(coefficientPairProducts219 >> 0x20) +
                           (int)(coefficientPairProducts187 >> 0x20) +
                           (int)(coefficientPairProducts224 >> 0x20) +
                           (int)(coefficientPairProducts192 >> 0x20) +
                           (int)(coefficientPairProducts209 >> 0x20) +
                           (int)(coefficientPairProducts177 >> 0x20) +
                           (int)(coefficientPairProducts214 >> 0x20) +
                           (int)(coefficientPairProducts182 >> 0x20) +
                           (int)(coefficientPairProducts215 >> 0x20) +
                           (int)(coefficientPairProducts183 >> 0x20) +
                           (int)(coefficientPairProducts236 >> 0x20) +
                           (int)(coefficientPairProducts12 >> 0x20) +
                           (int)(coefficientPairProducts237 >> 0x20) +
                           (int)(coefficientPairProducts13 >> 0x20) +
                           (int)(coefficientPairProducts226 >> 0x20) +
                           (int)(coefficientPairProducts2 >> 0x20) +
                           (int)(coefficientPairProducts227 >> 0x20) +
                           (int)(coefficientPairProducts3 >> 0x20) +
                           (int)(coefficientPairProducts232 >> 0x20) +
                           (int)(coefficientPairProducts8 >> 0x20) +
                           (int)(coefficientPairProducts249 >> 0x20) +
                           (int)(coefficientPairProducts17 >> 0x20)) * 0x20,bank3LowLaneSum * 0x20) &
         g_SoundDecodeMmxWordLaneMask3 |
         SAM_PACK_LANE_PAIR((bank2LowLaneSum + (int)(coefficientPairProducts91 >> 0x20) +
                           (int)(coefficientPairProducts124 >> 0x20) +
                           (int)(coefficientPairProducts93 >> 0x20) +
                           (int)(coefficientPairProducts123 >> 0x20) +
                           (int)(coefficientPairProducts92 >> 0x20) +
                           (int)(coefficientPairProducts126 >> 0x20) +
                           (int)(coefficientPairProducts95 >> 0x20) +
                           (int)(coefficientPairProducts125 >> 0x20) +
                           (int)(coefficientPairProducts94 >> 0x20) +
                           (int)(coefficientPairProducts128 >> 0x20) +
                           (int)(coefficientPairProducts97 >> 0x20) +
                           (int)(coefficientPairProducts127 >> 0x20) +
                           (int)(coefficientPairProducts96 >> 0x20) +
                           (int)(coefficientPairProducts115 >> 0x20) +
                           (int)(coefficientPairProducts84 >> 0x20) +
                           (int)(coefficientPairProducts114 >> 0x20) +
                           (int)(coefficientPairProducts83 >> 0x20) +
                           (int)(coefficientPairProducts117 >> 0x20) +
                           (int)(coefficientPairProducts86 >> 0x20) +
                           (int)(coefficientPairProducts116 >> 0x20) +
                           (int)(coefficientPairProducts85 >> 0x20) +
                           (int)(coefficientPairProducts119 >> 0x20) +
                           (int)(coefficientPairProducts88 >> 0x20) +
                           (int)(coefficientPairProducts118 >> 0x20) +
                           (int)(coefficientPairProducts87 >> 0x20) +
                           (int)(coefficientPairProducts121 >> 0x20) +
                           (int)(coefficientPairProducts90 >> 0x20) +
                           (int)(coefficientPairProducts120 >> 0x20) +
                           (int)(coefficientPairProducts89 >> 0x20) +
                           (int)(coefficientPairProducts28 >> 0x20) +
                           (int)(coefficientPairProducts107 >> 0x20) +
                           (int)(coefficientPairProducts27 >> 0x20) +
                           (int)(coefficientPairProducts106 >> 0x20) +
                           (int)(coefficientPairProducts30 >> 0x20) +
                           (int)(coefficientPairProducts109 >> 0x20) +
                           (int)(coefficientPairProducts29 >> 0x20) +
                           (int)(coefficientPairProducts108 >> 0x20) +
                           (int)(coefficientPairProducts32 >> 0x20) +
                           (int)(coefficientPairProducts111 >> 0x20) +
                           (int)(coefficientPairProducts31 >> 0x20) +
                           (int)(coefficientPairProducts110 >> 0x20) +
                           (int)(coefficientPairProducts34 >> 0x20) +
                           (int)(coefficientPairProducts113 >> 0x20) +
                           (int)(coefficientPairProducts33 >> 0x20) +
                           (int)(coefficientPairProducts112 >> 0x20) +
                           (int)(coefficientPairProducts20 >> 0x20) +
                           (int)(coefficientPairProducts99 >> 0x20) +
                           (int)(coefficientPairProducts19 >> 0x20) +
                           (int)(coefficientPairProducts98 >> 0x20) +
                           (int)(coefficientPairProducts22 >> 0x20) +
                           (int)(coefficientPairProducts101 >> 0x20) +
                           (int)(coefficientPairProducts21 >> 0x20) +
                           (int)(coefficientPairProducts100 >> 0x20) +
                           (int)(coefficientPairProducts24 >> 0x20) +
                           (int)(coefficientPairProducts103 >> 0x20) +
                           (int)(coefficientPairProducts23 >> 0x20) +
                           (int)(coefficientPairProducts102 >> 0x20) +
                           (int)(coefficientPairProducts26 >> 0x20) +
                           (int)(coefficientPairProducts105 >> 0x20) +
                           (int)(coefficientPairProducts25 >> 0x20) +
                           (int)(coefficientPairProducts104 >> 0x20) +
                           (int)(coefficientPairProducts44 >> 0x20) +
                           (int)(coefficientPairProducts122 >> 0x20) +
                           (int)(coefficientPairProducts43 >> 0x20)) * 0x20,bank2LowLaneSum * 0x20) >> 0x10 &
         g_SoundDecodeMmxWordLaneMask2;
    /* doubled with signed saturation (PADDSW) */
    saturatedOutputWords = paddsw(packedOutputWords,packedOutputWords);
    cosineRowCursor = cosineRowCursor + SAM_MMX_OUTPUTS_PER_PASS * SAM_BLOCK_SAMPLE_COUNT;
    *(MmxPackedValue64 *)outputMonoPcm = saturatedOutputWords;
    outputPassesRemaining--;
    outputMonoPcm = outputMonoPcm + SAM_MMX_OUTPUTS_PER_PASS;
  } while (outputPassesRemaining != 0);
}

/* Synthesizes one SAM block: transforms 256 decoded coefficients into 256 PCM samples with the MMX cosine
   tables (each sample is bits 16..31 of the wrapping 256-tap dot product x32, doubled with saturation) and
   writes them as a 0x400-byte interleaved stereo 16-bit block with the same value on both channels. No state
   is kept between blocks.
*/
void SoundSample_DecodeCoefficientBlockToPcmMmx(short *outputStereoPcm,short *coefficients)

{
  MmxPackedValue64 coefficientQuadLow;
  MmxPackedValue64 coefficientQuadHigh;
  short *cosineBankCursor;
  uint16_t outputWord1;
  MmxPackedValue64 mm0PackedValue0;
  MmxPackedValue64 mm0PackedValue1;
  MmxPackedValue64 mm0PackedValue2;
  MmxPackedValue64 mm0PackedValue3;
  MmxPackedValue64 mm0PackedValue4;
  MmxPackedValue64 mm0PackedValue5;
  MmxPackedValue64 mm0PackedValue6;
  MmxPackedValue64 mm0PackedValue7;
  MmxPackedValue64 mm0PackedValue8;
  MmxPackedValue64 mm0PackedValue9;
  MmxPackedValue64 mm0PackedValue10;
  MmxPackedValue64 mm0PackedValue11;
  MmxPackedValue64 mm0PackedValue12;
  MmxPackedValue64 mm0PackedValue13;
  MmxPackedValue64 mm0PackedValue14;
  MmxPackedValue64 mm0PackedValue15;
  MmxPackedValue64 mm0PackedValue16;
  MmxPackedValue64 mm0PackedValue17;
  MmxPackedValue64 mm0PackedValue18;
  MmxPackedValue64 mm0PackedValue19;
  MmxPackedValue64 mm0PackedValue20;
  MmxPackedValue64 mm0PackedValue21;
  MmxPackedValue64 mm0PackedValue22;
  MmxPackedValue64 mm0PackedValue23;
  MmxPackedValue64 mm0PackedValue24;
  MmxPackedValue64 mm0PackedValue25;
  MmxPackedValue64 mm0PackedValue26;
  MmxPackedValue64 mm0PackedValue27;
  MmxPackedValue64 mm0PackedValue28;
  MmxPackedValue64 mm0PackedValue29;
  MmxPackedValue64 mm0PackedValue30;
  MmxPackedValue64 mm0PackedValue31;
  uint64_t packedOutputWords;
  MmxPackedValue64 mm0PackedValue32;
  uint16_t outputWord2;
  uint16_t outputWord3;
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
  int outputGroupsRemaining;
  
  outputGroupsRemaining = 64; /* 64 groups of 4 samples, one cosine bank of 4 x 256 taps per group */
  cosineBankCursor = g_CosineDerivedLookupSecondTable;
  do {
    coefficientQuadLow = *(MmxPackedValue64 *)coefficients;
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 4);
    mm2PackedValue0 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)cosineBankCursor);
    mm3PackedValue0 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x100));
    mm2PackedValue1 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 4));
    mm3PackedValue1 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x104));
    mm0PackedValue0 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x200));
    mm1PackedValue0 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x204));
    mm2PackedValue2 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x300));
    mm3PackedValue2 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x304));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xc);
    mm2PackedValue3 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 8));
    mm3PackedValue3 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x108));
    mm2PackedValue4 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xc));
    mm3PackedValue4 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x10c));
    mm0PackedValue1 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x208));
    mm1PackedValue1 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x20c));
    mm2PackedValue5 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x308));
    mm3PackedValue5 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x30c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x10);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x14);
    mm2PackedValue6 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x10));
    mm3PackedValue6 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x110));
    mm2PackedValue7 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x14));
    mm3PackedValue7 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x114));
    mm0PackedValue2 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x210));
    mm1PackedValue2 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x214));
    mm2PackedValue8 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x310));
    mm3PackedValue8 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x314));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x18);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x1c);
    mm2PackedValue9 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x18));
    mm3PackedValue9 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x118));
    mm2PackedValue10 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1c));
    mm3PackedValue10 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x11c));
    mm0PackedValue3 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x218));
    mm1PackedValue3 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x21c));
    mm2PackedValue11 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x318));
    mm3PackedValue11 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x31c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x20);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x24);
    mm2PackedValue12 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x20));
    mm3PackedValue12 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x120));
    mm2PackedValue13 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x24));
    mm3PackedValue13 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x124));
    mm0PackedValue4 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x220));
    mm1PackedValue4 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x224));
    mm2PackedValue14 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 800));
    mm3PackedValue14 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x324));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x28);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x2c);
    mm2PackedValue15 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x28));
    mm3PackedValue15 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x128));
    mm2PackedValue16 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2c));
    mm3PackedValue16 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 300));
    mm0PackedValue5 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x228));
    mm1PackedValue5 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x22c));
    mm2PackedValue17 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x328));
    mm3PackedValue17 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x32c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x30);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x34);
    mm2PackedValue18 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x30));
    mm3PackedValue18 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x130));
    mm2PackedValue19 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x34));
    mm3PackedValue19 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x134));
    mm0PackedValue6 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x230));
    mm1PackedValue6 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x234));
    mm2PackedValue20 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x330));
    mm3PackedValue20 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x334));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x38);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x3c);
    mm2PackedValue21 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x38));
    mm3PackedValue21 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x138));
    mm2PackedValue22 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3c));
    mm3PackedValue22 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x13c));
    mm0PackedValue7 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x238));
    mm1PackedValue7 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x23c));
    mm2PackedValue23 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x338));
    mm3PackedValue23 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x33c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x40);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x44);
    mm2PackedValue24 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x40));
    mm3PackedValue24 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x140));
    mm2PackedValue25 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x44));
    mm3PackedValue25 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x144));
    mm0PackedValue8 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x240));
    mm1PackedValue8 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x244));
    mm2PackedValue26 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x340));
    mm3PackedValue26 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x344));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x48);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x4c);
    mm2PackedValue27 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x48));
    mm3PackedValue27 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x148));
    mm2PackedValue28 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x4c));
    mm3PackedValue28 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x14c));
    mm0PackedValue9 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x248));
    mm1PackedValue9 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x24c));
    mm2PackedValue29 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x348));
    mm3PackedValue29 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x34c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x50);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x54);
    mm2PackedValue30 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x50));
    mm3PackedValue30 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x150));
    mm2PackedValue31 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x54));
    mm3PackedValue31 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x154));
    mm0PackedValue10 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x250));
    mm1PackedValue10 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x254));
    mm2PackedValue32 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x350));
    mm3PackedValue32 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x354));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x58);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x5c);
    mm2PackedValue33 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x58));
    mm3PackedValue33 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x158));
    mm2PackedValue34 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x5c));
    mm3PackedValue34 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x15c));
    mm0PackedValue11 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 600));
    mm1PackedValue11 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x25c));
    mm2PackedValue35 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x358));
    mm3PackedValue35 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x35c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x60);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 100);
    mm2PackedValue36 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x60));
    mm3PackedValue36 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x160));
    mm2PackedValue37 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 100));
    mm3PackedValue37 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x164));
    mm0PackedValue12 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x260));
    mm1PackedValue12 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x264));
    mm2PackedValue38 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x360));
    mm3PackedValue38 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x364));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x68);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x6c);
    mm2PackedValue39 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x68));
    mm3PackedValue39 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x168));
    mm2PackedValue40 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x6c));
    mm3PackedValue40 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x16c));
    mm0PackedValue13 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x268));
    mm1PackedValue13 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x26c));
    mm2PackedValue41 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x368));
    mm3PackedValue41 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x36c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x70);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x74);
    mm2PackedValue42 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x70));
    mm3PackedValue42 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x170));
    mm2PackedValue43 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x74));
    mm3PackedValue43 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x174));
    mm0PackedValue14 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x270));
    mm1PackedValue14 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x274));
    mm2PackedValue44 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x370));
    mm3PackedValue44 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x374));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x78);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x7c);
    mm2PackedValue45 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x78));
    mm3PackedValue45 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x178));
    mm2PackedValue46 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x7c));
    mm3PackedValue46 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x17c));
    mm0PackedValue15 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x278));
    mm1PackedValue15 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x27c));
    mm2PackedValue47 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x378));
    mm3PackedValue47 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x37c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x80);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x84);
    mm2PackedValue48 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x80));
    mm3PackedValue48 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x180));
    mm2PackedValue49 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x84));
    mm3PackedValue49 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x184));
    mm0PackedValue16 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x280));
    mm1PackedValue16 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x284));
    mm2PackedValue50 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x380));
    mm3PackedValue50 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 900));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x88);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x8c);
    mm2PackedValue51 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x88));
    mm3PackedValue51 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x188));
    mm2PackedValue52 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x8c));
    mm3PackedValue52 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x18c));
    mm0PackedValue17 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x288));
    mm1PackedValue17 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x28c));
    mm2PackedValue53 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x388));
    mm3PackedValue53 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x38c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x90);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x94);
    mm2PackedValue54 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x90));
    mm3PackedValue54 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 400));
    mm2PackedValue55 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x94));
    mm3PackedValue55 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x194));
    mm0PackedValue18 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x290));
    mm1PackedValue18 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x294));
    mm2PackedValue56 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x390));
    mm3PackedValue56 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x394));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0x98);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0x9c);
    mm2PackedValue57 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x98));
    mm3PackedValue57 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x198));
    mm2PackedValue58 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x9c));
    mm3PackedValue58 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x19c));
    mm0PackedValue19 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x298));
    mm1PackedValue19 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x29c));
    mm2PackedValue59 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x398));
    mm3PackedValue59 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x39c));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xa0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xa4);
    mm2PackedValue60 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xa0));
    mm3PackedValue60 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1a0));
    mm2PackedValue61 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xa4));
    mm3PackedValue61 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1a4));
    mm0PackedValue20 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2a0));
    mm1PackedValue20 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2a4));
    mm2PackedValue62 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3a0));
    mm3PackedValue62 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3a4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xa8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xac);
    mm2PackedValue63 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xa8));
    mm3PackedValue63 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1a8));
    mm2PackedValue64 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xac));
    mm3PackedValue64 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1ac));
    mm0PackedValue21 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2a8));
    mm1PackedValue21 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2ac));
    mm2PackedValue65 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3a8));
    mm3PackedValue65 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3ac));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xb0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xb4);
    mm2PackedValue66 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xb0));
    mm3PackedValue66 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1b0));
    mm2PackedValue67 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xb4));
    mm3PackedValue67 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1b4));
    mm0PackedValue22 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2b0));
    mm1PackedValue22 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2b4));
    mm2PackedValue68 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3b0));
    mm3PackedValue68 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3b4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xb8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xbc);
    mm2PackedValue69 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xb8));
    mm3PackedValue69 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1b8));
    mm2PackedValue70 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xbc));
    mm3PackedValue70 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1bc));
    mm0PackedValue23 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2b8));
    mm1PackedValue23 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 700));
    mm2PackedValue71 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3b8));
    mm3PackedValue71 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3bc));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xc0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xc4);
    mm2PackedValue72 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xc0));
    mm3PackedValue72 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1c0));
    mm2PackedValue73 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xc4));
    mm3PackedValue73 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1c4));
    mm0PackedValue24 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2c0));
    mm1PackedValue24 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2c4));
    mm2PackedValue74 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3c0));
    mm3PackedValue74 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3c4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 200);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xcc);
    mm2PackedValue75 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 200));
    mm3PackedValue75 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1c8));
    mm2PackedValue76 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xcc));
    mm3PackedValue76 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1cc));
    mm0PackedValue25 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2c8));
    mm1PackedValue25 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2cc));
    mm2PackedValue77 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3c8));
    mm3PackedValue77 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3cc));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xd0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xd4);
    mm2PackedValue78 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xd0));
    mm3PackedValue78 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1d0));
    mm2PackedValue79 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xd4));
    mm3PackedValue79 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1d4));
    mm0PackedValue26 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2d0));
    mm1PackedValue26 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2d4));
    mm2PackedValue80 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3d0));
    mm3PackedValue80 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3d4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xd8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xdc);
    mm2PackedValue81 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xd8));
    mm3PackedValue81 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1d8));
    mm2PackedValue82 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xdc));
    mm3PackedValue82 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1dc));
    mm0PackedValue27 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2d8));
    mm1PackedValue27 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2dc));
    mm2PackedValue83 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3d8));
    mm3PackedValue83 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3dc));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xe0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xe4);
    mm2PackedValue84 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xe0));
    mm3PackedValue84 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1e0));
    mm2PackedValue85 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xe4));
    mm3PackedValue85 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1e4));
    mm0PackedValue28 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2e0));
    mm1PackedValue28 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2e4));
    mm2PackedValue86 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3e0));
    mm3PackedValue86 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3e4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xe8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xec);
    mm2PackedValue87 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xe8));
    mm3PackedValue87 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1e8));
    mm2PackedValue88 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xec));
    mm3PackedValue88 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1ec));
    mm0PackedValue29 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2e8));
    mm1PackedValue29 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2ec));
    mm2PackedValue89 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 1000));
    mm3PackedValue89 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3ec));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xf0);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xf4);
    mm2PackedValue90 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xf0));
    mm3PackedValue90 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1f0));
    mm2PackedValue91 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xf4));
    mm3PackedValue91 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 500));
    mm0PackedValue30 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2f0));
    mm1PackedValue30 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2f4));
    mm2PackedValue92 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3f0));
    mm3PackedValue92 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3f4));
    coefficientQuadLow = *(MmxPackedValue64 *)(coefficients + 0xf8);
    coefficientQuadHigh = *(MmxPackedValue64 *)(coefficients + 0xfc);
    mm2PackedValue93 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0xf8));
    mm3PackedValue93 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x1f8));
    mm2PackedValue94 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0xfc));
    mm3PackedValue94 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x1fc));
    bank0HighLaneSum = (int)((uint64_t)mm2PackedValue0 >> 0x20) + (int)((uint64_t)mm2PackedValue1 >> 0x20) +
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
    mm0PackedValue31 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x2f8));
    mm1PackedValue31 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x2fc));
    mm2PackedValue95 = pmaddwd(coefficientQuadLow,*(MmxPackedValue64 *)(cosineBankCursor + 0x3f8));
    mm3PackedValue95 = pmaddwd(coefficientQuadHigh,*(MmxPackedValue64 *)(cosineBankCursor + 0x3fc));
    bank2LowLaneSum = (int)mm0PackedValue0 + (int)mm1PackedValue0 + (int)mm0PackedValue1 +
             (int)mm1PackedValue1 + (int)mm0PackedValue2 + (int)mm1PackedValue2 +
             (int)mm0PackedValue3 + (int)mm1PackedValue3 + (int)mm0PackedValue4 +
             (int)mm1PackedValue4 + (int)mm0PackedValue5 + (int)mm1PackedValue5 +
             (int)mm0PackedValue6 + (int)mm1PackedValue6 + (int)mm0PackedValue7 +
             (int)mm1PackedValue7 + (int)mm0PackedValue8 + (int)mm1PackedValue8 +
             (int)mm0PackedValue9 + (int)mm1PackedValue9 + (int)mm0PackedValue10 +
             (int)mm1PackedValue10 + (int)mm0PackedValue11 + (int)mm1PackedValue11 +
             (int)mm0PackedValue12 + (int)mm1PackedValue12 + (int)mm0PackedValue13 +
             (int)mm1PackedValue13 + (int)mm0PackedValue14 + (int)mm1PackedValue14 +
             (int)mm0PackedValue15 + (int)mm1PackedValue15 + (int)mm0PackedValue16 +
             (int)mm1PackedValue16 + (int)mm0PackedValue17 + (int)mm1PackedValue17 +
             (int)mm0PackedValue18 + (int)mm1PackedValue18 + (int)mm0PackedValue19 +
             (int)mm1PackedValue19 + (int)mm0PackedValue20 + (int)mm1PackedValue20 +
             (int)mm0PackedValue21 + (int)mm1PackedValue21 + (int)mm0PackedValue22 +
             (int)mm1PackedValue22 + (int)mm0PackedValue23 + (int)mm1PackedValue23 +
             (int)mm0PackedValue24 + (int)mm1PackedValue24 + (int)mm0PackedValue25 +
             (int)mm1PackedValue25 + (int)mm0PackedValue26 + (int)mm1PackedValue26 +
             (int)mm0PackedValue27 + (int)mm1PackedValue27 + (int)mm0PackedValue28 +
             (int)mm1PackedValue28 + (int)mm0PackedValue29 + (int)mm1PackedValue29 +
             (int)mm0PackedValue30 + (int)mm1PackedValue30 + (int)mm0PackedValue31 +
             (int)mm1PackedValue31;
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
    packedOutputWords = SAM_PACK_LANE_PAIR(bank0HighLaneSum * 0x20,
                     (bank0HighLaneSum + (int)mm2PackedValue0 + (int)mm2PackedValue1 + (int)mm2PackedValue3 +
                              (int)mm2PackedValue4 + (int)mm2PackedValue6 + (int)mm2PackedValue7 +
                              (int)mm2PackedValue9 + (int)mm2PackedValue10 + (int)mm2PackedValue12 +
                              (int)mm2PackedValue13 + (int)mm2PackedValue15 + (int)mm2PackedValue16
                              + (int)mm2PackedValue18 + (int)mm2PackedValue19 +
                              (int)mm2PackedValue21 + (int)mm2PackedValue22 + (int)mm2PackedValue24
                              + (int)mm2PackedValue25 + (int)mm2PackedValue27 +
                              (int)mm2PackedValue28 + (int)mm2PackedValue30 + (int)mm2PackedValue31
                              + (int)mm2PackedValue33 + (int)mm2PackedValue34 +
                              (int)mm2PackedValue36 + (int)mm2PackedValue37 + (int)mm2PackedValue39
                              + (int)mm2PackedValue40 + (int)mm2PackedValue42 +
                              (int)mm2PackedValue43 + (int)mm2PackedValue45 + (int)mm2PackedValue46
                              + (int)mm2PackedValue48 + (int)mm2PackedValue49 +
                              (int)mm2PackedValue51 + (int)mm2PackedValue52 + (int)mm2PackedValue54
                              + (int)mm2PackedValue55 + (int)mm2PackedValue57 +
                              (int)mm2PackedValue58 + (int)mm2PackedValue60 + (int)mm2PackedValue61
                              + (int)mm2PackedValue63 + (int)mm2PackedValue64 +
                              (int)mm2PackedValue66 + (int)mm2PackedValue67 + (int)mm2PackedValue69
                              + (int)mm2PackedValue70 + (int)mm2PackedValue72 +
                              (int)mm2PackedValue73 + (int)mm2PackedValue75 + (int)mm2PackedValue76
                              + (int)mm2PackedValue78 + (int)mm2PackedValue79 +
                              (int)mm2PackedValue81 + (int)mm2PackedValue82 + (int)mm2PackedValue84
                              + (int)mm2PackedValue85 + (int)mm2PackedValue87 +
                              (int)mm2PackedValue88 + (int)mm2PackedValue90 + (int)mm2PackedValue91
                              + (int)mm2PackedValue93 + (int)mm2PackedValue94) * 0x20) >> 0x10 &
            g_SoundDecodeMmxWordLaneMask0 |
            SAM_PACK_LANE_PAIR(bank1HighLaneSum * 0x20,
                     (bank1HighLaneSum + (int)mm3PackedValue0 + (int)mm3PackedValue1 + (int)mm3PackedValue3 +
                              (int)mm3PackedValue4 + (int)mm3PackedValue6 + (int)mm3PackedValue7 +
                              (int)mm3PackedValue9 + (int)mm3PackedValue10 + (int)mm3PackedValue12 +
                              (int)mm3PackedValue13 + (int)mm3PackedValue15 + (int)mm3PackedValue16
                              + (int)mm3PackedValue18 + (int)mm3PackedValue19 +
                              (int)mm3PackedValue21 + (int)mm3PackedValue22 + (int)mm3PackedValue24
                              + (int)mm3PackedValue25 + (int)mm3PackedValue27 +
                              (int)mm3PackedValue28 + (int)mm3PackedValue30 + (int)mm3PackedValue31
                              + (int)mm3PackedValue33 + (int)mm3PackedValue34 +
                              (int)mm3PackedValue36 + (int)mm3PackedValue37 + (int)mm3PackedValue39
                              + (int)mm3PackedValue40 + (int)mm3PackedValue42 +
                              (int)mm3PackedValue43 + (int)mm3PackedValue45 + (int)mm3PackedValue46
                              + (int)mm3PackedValue48 + (int)mm3PackedValue49 +
                              (int)mm3PackedValue51 + (int)mm3PackedValue52 + (int)mm3PackedValue54
                              + (int)mm3PackedValue55 + (int)mm3PackedValue57 +
                              (int)mm3PackedValue58 + (int)mm3PackedValue60 + (int)mm3PackedValue61
                              + (int)mm3PackedValue63 + (int)mm3PackedValue64 +
                              (int)mm3PackedValue66 + (int)mm3PackedValue67 + (int)mm3PackedValue69
                              + (int)mm3PackedValue70 + (int)mm3PackedValue72 +
                              (int)mm3PackedValue73 + (int)mm3PackedValue75 + (int)mm3PackedValue76
                              + (int)mm3PackedValue78 + (int)mm3PackedValue79 +
                              (int)mm3PackedValue81 + (int)mm3PackedValue82 + (int)mm3PackedValue84
                              + (int)mm3PackedValue85 + (int)mm3PackedValue87 +
                              (int)mm3PackedValue88 + (int)mm3PackedValue90 + (int)mm3PackedValue91
                              + (int)mm3PackedValue93 + (int)mm3PackedValue94) * 0x20) &
            g_SoundDecodeMmxWordLaneMask1 |
            SAM_PACK_LANE_PAIR((bank3LowLaneSum + (int)((uint64_t)mm2PackedValue2 >> 0x20) +
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
                               (int)((uint64_t)mm3PackedValue95 >> 0x20)) * 0x20,bank3LowLaneSum * 0x20) &
            g_SoundDecodeMmxWordLaneMask3 |
            SAM_PACK_LANE_PAIR((bank2LowLaneSum + (int)((uint64_t)mm0PackedValue0 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue0 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue1 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue1 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue2 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue2 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue3 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue3 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue4 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue4 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue5 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue5 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue6 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue6 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue7 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue7 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue8 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue8 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue9 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue9 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue10 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue10 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue11 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue11 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue12 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue12 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue13 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue13 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue14 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue14 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue15 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue15 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue16 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue16 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue17 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue17 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue18 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue18 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue19 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue19 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue20 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue20 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue21 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue21 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue22 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue22 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue23 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue23 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue24 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue24 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue25 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue25 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue26 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue26 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue27 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue27 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue28 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue28 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue29 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue29 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue30 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue30 >> 0x20) +
                               (int)((uint64_t)mm0PackedValue31 >> 0x20) +
                               (int)((uint64_t)mm1PackedValue31 >> 0x20)) * 0x20,bank2LowLaneSum * 0x20) >>
            0x10 & g_SoundDecodeMmxWordLaneMask2;
    mm0PackedValue32 = paddsw(packedOutputWords,packedOutputWords);
    cosineBankCursor = cosineBankCursor + 0x400;
    outputWord1 = (uint16_t)((uint64_t)mm0PackedValue32 >> 0x10);
    outputWord3 = (uint16_t)((uint64_t)mm0PackedValue32 >> 0x30);
    outputWord2 = (uint16_t)((uint64_t)mm0PackedValue32 >> 0x20);
    /* punpcklwd/punpckhwd with itself: every word becomes a left/right stereo pair (w * 0x10001 = w:w). */
    *(uint64_t *)outputStereoPcm =
         ((uint64_t)((uint32_t)outputWord1 * 0x10001) << 32) | (uint32_t)(uint16_t)mm0PackedValue32 * 0x10001;
    *(uint64_t *)(outputStereoPcm + 4) =
         ((uint64_t)((uint32_t)outputWord3 * 0x10001) << 32) | (uint32_t)outputWord2 * 0x10001;
    outputGroupsRemaining--;
    outputStereoPcm = outputStereoPcm + 8;
  } while (outputGroupsRemaining != 0);
  return;
}

/* Unpacks one SAM block into 256 signed 16-bit coefficients (inverse of
   SoundSample_EncodePackedCoefficientBlock). Each coefficient is prefix-coded from the low bits of a 32-bit
   little-endian bit accumulator (prefix bits listed from bit 0): 0 -> zero (1 bit), 1,0 -> 3-bit value (5 bits),
   1,1,0 -> 6-bit value (9 bits), 1,1,1 -> 12-bit value (15 bits). Returns the encoded byte count consumed,
   rounded DOWN to a multiple of 4.
*/
uint32_t SoundSample_DecodePackedCoefficientBlock(short *outputCoefficients,uint8_t *encodedBlock)

{
  uint16_t refillWord;
  int refillDword;
  uint32_t fieldBits;
  uint32_t bitAccumulator;
  uint8_t refillShift;
  uint32_t availableBitCount;
  uint16_t *inputCursor;
  int coefficientsRemaining;

  coefficientsRemaining = SAM_BLOCK_SAMPLE_COUNT;
  bitAccumulator = *(uint32_t *)encodedBlock;
  availableBitCount = 32;
  inputCursor = (uint16_t *)(encodedBlock + 4);
  do {
    /* the value fields are sign-extended by shifting them to the top of a 32-bit int and back */
    if ((bitAccumulator & 1) == 0) {
      *outputCoefficients = 0;
      availableBitCount = availableBitCount - 1;
      bitAccumulator = bitAccumulator >> 1;
    }
    else if ((bitAccumulator & 2) == 0) {
      fieldBits = bitAccumulator >> 2;
      bitAccumulator = bitAccumulator >> 5;
      availableBitCount = availableBitCount - 5;
      *outputCoefficients = (short)((int)(fieldBits << 29) >> 29);
    }
    else if ((bitAccumulator & 4) == 0) {
      fieldBits = bitAccumulator >> 3;
      bitAccumulator = bitAccumulator >> 9;
      availableBitCount = availableBitCount - 9;
      *outputCoefficients = (short)((int)(fieldBits << 26) >> 26);
    }
    else {
      fieldBits = bitAccumulator >> 3;
      bitAccumulator = bitAccumulator >> 15;
      availableBitCount = availableBitCount - 15;
      *outputCoefficients = (short)((int)(fieldBits << 20) >> 20);
    }
    /* refill whole bytes above the remaining bits so that at least 25 bits are available again */
    refillShift = (uint8_t)availableBitCount;
    if (availableBitCount < 9) {
      refillDword = *(int *)inputCursor;
      inputCursor = (uint16_t *)((uintptr_t)inputCursor + 3);
      availableBitCount = availableBitCount + 24;
      bitAccumulator = bitAccumulator | refillDword << (refillShift & 0x1f);
    }
    else if (availableBitCount < 17) {
      refillWord = *inputCursor;
      inputCursor = inputCursor + 1;
      availableBitCount = availableBitCount + 16;
      bitAccumulator = bitAccumulator | (uint32_t)refillWord << (refillShift & 0x1f);
    }
    else if (availableBitCount < 25) {
      refillWord = *inputCursor;
      inputCursor = (uint16_t *)((uintptr_t)inputCursor + 1);
      availableBitCount = availableBitCount + 8;
      bitAccumulator = bitAccumulator | (uint32_t)(uint8_t)refillWord << (refillShift & 0x1f);
    }
    outputCoefficients++;
    coefficientsRemaining--;
  } while (coefficientsRemaining != 0);
  if (availableBitCount == 32) {
    inputCursor = (uint16_t *)((uintptr_t)inputCursor - 1);
  }
  else if (availableBitCount < 24) {
    inputCursor = (uint16_t *)((uintptr_t)inputCursor + 1);
  }
  return (uint32_t)(((uintptr_t)inputCursor & ~(uintptr_t)3) - (uintptr_t)encodedBlock);
}

/* Builds the two 256x256 cosine matrices of the .sam sound codec in one 0x40000-byte allocation (called by
   SdlAudio_Init, as by the original's DirectSound_Init). The first (g_CosineDerivedLookupAllocation, Q12) has row u, entry k =
   cos((2k+1) * u * pi / 512), row 0 being 1/sqrt(2); the second (g_CosineDerivedLookupSecondTable, Q14) is
   its transpose, row m, entry k = cos(k * (2m+1) * pi / 512), entry 0 being 1/sqrt(2). Angles are 16-bit
   (65536 = full turn), so 0x40 is pi/512. Returns 0, or the allocation error; on allocation failure the
   pointers stay unset. The original returned nothing and its caller decoded with the unset tables; the
   result lets SdlAudio_Init run silent instead.
*/
uint32_t __cdecl CosineDerivedLookupTables_Init()

{
  short *outputCursor;
  int entriesRemainingInRow;
  uint32_t angleStep16;
  uint32_t secondAngleStep16;
  int entriesRemaining;
  uint32_t angleIndex16;
  uint32_t secondAngleIndex16;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(2 * COSINE_DERIVED_TABLE_ORDER * COSINE_DERIVED_TABLE_ORDER * sizeof(short),(void **)&outputCursor);
  if (allocError != 0) {
    return allocError;
  }
  else {
    g_CosineDerivedLookupAllocation = outputCursor;
    /* row 0: 256 entries of 1/sqrt(2) in Q12, written as 128 pairs */
    for (entriesRemainingInRow = COSINE_DERIVED_TABLE_ORDER / 2; entriesRemainingInRow != 0; entriesRemainingInRow--) {
      outputCursor[0] = COSINE_DERIVED_INV_SQRT2_Q12;
      outputCursor[1] = COSINE_DERIVED_INV_SQRT2_Q12;
      outputCursor = outputCursor + 2;
    }
    /* rows 1..255: angleStep16 = u * 0x40, entries at the odd multiples (2k+1) * angleStep16 */
    angleIndex16 = COSINE_DERIVED_TABLE_ANGLE_STEP;
    angleStep16 = COSINE_DERIVED_TABLE_ANGLE_STEP;
    entriesRemaining = COSINE_DERIVED_TABLE_ORDER;
    do {
      do {
        *outputCursor = (short)((uint32_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + angleIndex16] >> 16); /* Q28 -> Q12 */
        outputCursor++;
        angleIndex16 = (angleIndex16 + angleStep16 * 2) & FIXED_ANGLE16_MASK;
        entriesRemaining--;
      } while (entriesRemaining != 0);
      angleStep16 = angleStep16 + COSINE_DERIVED_TABLE_ANGLE_STEP;
      entriesRemaining = COSINE_DERIVED_TABLE_ORDER;
      angleIndex16 = angleStep16 & FIXED_ANGLE16_MASK;
    } while (angleStep16 < COSINE_DERIVED_TABLE_ORDER * COSINE_DERIVED_TABLE_ANGLE_STEP);
    /* rows m = 0..255: secondAngleStep16 = (2m+1) * 0x40, entries at k * secondAngleStep16 */
    secondAngleIndex16 = 0;
    entriesRemaining = COSINE_DERIVED_TABLE_ORDER;
    secondAngleStep16 = COSINE_DERIVED_TABLE_ANGLE_STEP;
    g_CosineDerivedLookupSecondTable = outputCursor;
    do {
      do {
        if (entriesRemaining == COSINE_DERIVED_TABLE_ORDER) {
          *outputCursor = COSINE_DERIVED_INV_SQRT2_Q14; /* entry 0 */
        }
        else {
          *outputCursor = (short)(g_FixedSineQ28[FIXED_SINE_TABLE_COS + secondAngleIndex16] >> 14); /* Q28 -> Q14 */
        }
        outputCursor++;
        secondAngleIndex16 = (secondAngleIndex16 + secondAngleStep16) & FIXED_ANGLE16_MASK;
        entriesRemaining--;
      } while (entriesRemaining != 0);
      secondAngleStep16 = secondAngleStep16 + 2 * COSINE_DERIVED_TABLE_ANGLE_STEP;
      entriesRemaining = COSINE_DERIVED_TABLE_ORDER;
      secondAngleIndex16 = 0;
    } while (secondAngleStep16 < 2 * COSINE_DERIVED_TABLE_ORDER * COSINE_DERIVED_TABLE_ANGLE_STEP);
  }
  return 0;
}
