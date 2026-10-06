/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/preview.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/preview.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* uint32_t[8]: preview army asset id per pointer mode (0 = none); ui/ingame/world_input.cpp */
const uint32_t g_InGamePointerModePreviewArmyIds[8] = {0, 240, 242, 240, 244, 240, 242, 240};

/* Origin X/Y/Z, projection scale, view angles 0/1, projection shift; passed whole to
   g_GraphicsOffscreenRenderModelListToTextureSource */
static GraphicsOffscreenViewParameters g_ArmyPreviewViewParameters = {
    0, /* originX */
    0, /* originY */
    0, /* originZ */
    0, /* projectionScale */
    0, /* viewAngle0 */
    0, /* viewAngle1 */
    0, /* projectionShift */
};

static AngleTurn32 g_ArmyPreviewAuxiliaryOrientation[2] = {
    0,
    0,
};

/* horizontalExtent = primary colour ARGB, verticalExtent = secondary colour ARGB; passed whole
   to g_GraphicsOffscreenRenderModelListToTextureSource, which forwards them as the scene colour pairs of
   Graphics_SetSceneBoundsAndColors */
static GraphicsOffscreenSceneExtents g_ArmyPreviewSceneExtents = {
    0, /* horizontalExtent (primary colour ARGB) */
    0, /* verticalExtent (secondary colour ARGB) */
};

static ModelRuntimeNode *g_ArmyPreviewModelNode = nullptr;

/* uint64_t[256] MMX qword per alpha a: three 16-bit lanes (a * 0x101) >> 4, alpha lane 0; PMULHW premultiply of the 2x2 downsample in the army preview */
static const uint64_t g_ArmyPreviewAlphaPremultiplyMmxLut256[256] = {
    /*   0 */ 0, 0x1000100010ull, 0x2000200020ull, 0x3000300030ull, 0x4000400040ull, 0x5000500050ull, 0x6000600060ull, 0x7000700070ull,
    /*   8 */ 0x8000800080ull, 0x9000900090ull, 0xA000A000A0ull, 0xB000B000B0ull, 0xC000C000C0ull, 0xD000D000D0ull, 0xE000E000E0ull, 0xF000F000F0ull,
    /*  16 */ 0x10101010101ull, 0x11101110111ull, 0x12101210121ull, 0x13101310131ull, 0x14101410141ull, 0x15101510151ull, 0x16101610161ull, 0x17101710171ull,
    /*  24 */ 0x18101810181ull, 0x19101910191ull, 0x1A101A101A1ull, 0x1B101B101B1ull, 0x1C101C101C1ull, 0x1D101D101D1ull, 0x1E101E101E1ull, 0x1F101F101F1ull,
    /*  32 */ 0x20202020202ull, 0x21202120212ull, 0x22202220222ull, 0x23202320232ull, 0x24202420242ull, 0x25202520252ull, 0x26202620262ull, 0x27202720272ull,
    /*  40 */ 0x28202820282ull, 0x29202920292ull, 0x2A202A202A2ull, 0x2B202B202B2ull, 0x2C202C202C2ull, 0x2D202D202D2ull, 0x2E202E202E2ull, 0x2F202F202F2ull,
    /*  48 */ 0x30303030303ull, 0x31303130313ull, 0x32303230323ull, 0x33303330333ull, 0x34303430343ull, 0x35303530353ull, 0x36303630363ull, 0x37303730373ull,
    /*  56 */ 0x38303830383ull, 0x39303930393ull, 0x3A303A303A3ull, 0x3B303B303B3ull, 0x3C303C303C3ull, 0x3D303D303D3ull, 0x3E303E303E3ull, 0x3F303F303F3ull,
    /*  64 */ 0x40404040404ull, 0x41404140414ull, 0x42404240424ull, 0x43404340434ull, 0x44404440444ull, 0x45404540454ull, 0x46404640464ull, 0x47404740474ull,
    /*  72 */ 0x48404840484ull, 0x49404940494ull, 0x4A404A404A4ull, 0x4B404B404B4ull, 0x4C404C404C4ull, 0x4D404D404D4ull, 0x4E404E404E4ull, 0x4F404F404F4ull,
    /*  80 */ 0x50505050505ull, 0x51505150515ull, 0x52505250525ull, 0x53505350535ull, 0x54505450545ull, 0x55505550555ull, 0x56505650565ull, 0x57505750575ull,
    /*  88 */ 0x58505850585ull, 0x59505950595ull, 0x5A505A505A5ull, 0x5B505B505B5ull, 0x5C505C505C5ull, 0x5D505D505D5ull, 0x5E505E505E5ull, 0x5F505F505F5ull,
    /*  96 */ 0x60606060606ull, 0x61606160616ull, 0x62606260626ull, 0x63606360636ull, 0x64606460646ull, 0x65606560656ull, 0x66606660666ull, 0x67606760676ull,
    /* 104 */ 0x68606860686ull, 0x69606960696ull, 0x6A606A606A6ull, 0x6B606B606B6ull, 0x6C606C606C6ull, 0x6D606D606D6ull, 0x6E606E606E6ull, 0x6F606F606F6ull,
    /* 112 */ 0x70707070707ull, 0x71707170717ull, 0x72707270727ull, 0x73707370737ull, 0x74707470747ull, 0x75707570757ull, 0x76707670767ull, 0x77707770777ull,
    /* 120 */ 0x78707870787ull, 0x79707970797ull, 0x7A707A707A7ull, 0x7B707B707B7ull, 0x7C707C707C7ull, 0x7D707D707D7ull, 0x7E707E707E7ull, 0x7F707F707F7ull,
    /* 128 */ 0x80808080808ull, 0x81808180818ull, 0x82808280828ull, 0x83808380838ull, 0x84808480848ull, 0x85808580858ull, 0x86808680868ull, 0x87808780878ull,
    /* 136 */ 0x88808880888ull, 0x89808980898ull, 0x8A808A808A8ull, 0x8B808B808B8ull, 0x8C808C808C8ull, 0x8D808D808D8ull, 0x8E808E808E8ull, 0x8F808F808F8ull,
    /* 144 */ 0x90909090909ull, 0x91909190919ull, 0x92909290929ull, 0x93909390939ull, 0x94909490949ull, 0x95909590959ull, 0x96909690969ull, 0x97909790979ull,
    /* 152 */ 0x98909890989ull, 0x99909990999ull, 0x9A909A909A9ull, 0x9B909B909B9ull, 0x9C909C909C9ull, 0x9D909D909D9ull, 0x9E909E909E9ull, 0x9F909F909F9ull,
    /* 160 */ 0xA0A0A0A0A0Aull, 0xA1A0A1A0A1Aull, 0xA2A0A2A0A2Aull, 0xA3A0A3A0A3Aull, 0xA4A0A4A0A4Aull, 0xA5A0A5A0A5Aull, 0xA6A0A6A0A6Aull, 0xA7A0A7A0A7Aull,
    /* 168 */ 0xA8A0A8A0A8Aull, 0xA9A0A9A0A9Aull, 0xAAA0AAA0AAAull, 0xABA0ABA0ABAull, 0xACA0ACA0ACAull, 0xADA0ADA0ADAull, 0xAEA0AEA0AEAull, 0xAFA0AFA0AFAull,
    /* 176 */ 0xB0B0B0B0B0Bull, 0xB1B0B1B0B1Bull, 0xB2B0B2B0B2Bull, 0xB3B0B3B0B3Bull, 0xB4B0B4B0B4Bull, 0xB5B0B5B0B5Bull, 0xB6B0B6B0B6Bull, 0xB7B0B7B0B7Bull,
    /* 184 */ 0xB8B0B8B0B8Bull, 0xB9B0B9B0B9Bull, 0xBAB0BAB0BABull, 0xBBB0BBB0BBBull, 0xBCB0BCB0BCBull, 0xBDB0BDB0BDBull, 0xBEB0BEB0BEBull, 0xBFB0BFB0BFBull,
    /* 192 */ 0xC0C0C0C0C0Cull, 0xC1C0C1C0C1Cull, 0xC2C0C2C0C2Cull, 0xC3C0C3C0C3Cull, 0xC4C0C4C0C4Cull, 0xC5C0C5C0C5Cull, 0xC6C0C6C0C6Cull, 0xC7C0C7C0C7Cull,
    /* 200 */ 0xC8C0C8C0C8Cull, 0xC9C0C9C0C9Cull, 0xCAC0CAC0CACull, 0xCBC0CBC0CBCull, 0xCCC0CCC0CCCull, 0xCDC0CDC0CDCull, 0xCEC0CEC0CECull, 0xCFC0CFC0CFCull,
    /* 208 */ 0xD0D0D0D0D0Dull, 0xD1D0D1D0D1Dull, 0xD2D0D2D0D2Dull, 0xD3D0D3D0D3Dull, 0xD4D0D4D0D4Dull, 0xD5D0D5D0D5Dull, 0xD6D0D6D0D6Dull, 0xD7D0D7D0D7Dull,
    /* 216 */ 0xD8D0D8D0D8Dull, 0xD9D0D9D0D9Dull, 0xDAD0DAD0DADull, 0xDBD0DBD0DBDull, 0xDCD0DCD0DCDull, 0xDDD0DDD0DDDull, 0xDED0DED0DEDull, 0xDFD0DFD0DFDull,
    /* 224 */ 0xE0E0E0E0E0Eull, 0xE1E0E1E0E1Eull, 0xE2E0E2E0E2Eull, 0xE3E0E3E0E3Eull, 0xE4E0E4E0E4Eull, 0xE5E0E5E0E5Eull, 0xE6E0E6E0E6Eull, 0xE7E0E7E0E7Eull,
    /* 232 */ 0xE8E0E8E0E8Eull, 0xE9E0E9E0E9Eull, 0xEAE0EAE0EAEull, 0xEBE0EBE0EBEull, 0xECE0ECE0ECEull, 0xEDE0EDE0EDEull, 0xEEE0EEE0EEEull, 0xEFE0EFE0EFEull,
    /* 240 */ 0xF0F0F0F0F0Full, 0xF1F0F1F0F1Full, 0xF2F0F2F0F2Full, 0xF3F0F3F0F3Full, 0xF4F0F4F0F4Full, 0xF5F0F5F0F5Full, 0xF6F0F6F0F6Full, 0xF7F0F7F0F7Full,
    /* 248 */ 0xF8F0F8F0F8Full, 0xF9F0F9F0F9Full, 0xFAF0FAF0FAFull, 0xFBF0FBF0FBFull, 0xFCF0FCF0FCFull, 0xFDF0FDF0FDFull, 0xFEF0FEF0FEFull, 0xFFF0FFF0FFFull};

/* uint64_t[256] MMX qword per average alpha a: three lanes ~0x3FF0/a (reciprocal), fourth lane a; un-premultiplies the averaged army preview pixel */
static const uint64_t g_ArmyPreviewAverageAlphaReciprocalMmxLut256[256] = {
    /*   0 */ 0, 0x13FF03FF03FF0ull, 0x21FF81FF81FF8ull, 0x3155015501550ull, 0x40FFC0FFC0FFCull, 0x50CC90CC90CC9ull, 0x60AA80AA80AA8ull, 0x7092209220922ull,
    /*   8 */ 0x807FE07FE07FEull, 0x9071A071A071Aull, 0xA066406640664ull, 0xB05D005D005D0ull, 0xC055405540554ull, 0xD04EB04EB04EBull, 0xE049104910491ull, 0xF044304430443ull,
    /*  16 */ 0x1003FF03FF03FFull, 0x1103C203C203C2ull, 0x12038D038D038Dull, 0x13035D035D035Dull, 0x14033203320332ull, 0x15030B030B030Bull, 0x1602E802E802E8ull, 0x1702C702C702C7ull,
    /*  24 */ 0x1802AA02AA02AAull, 0x19028E028E028Eull, 0x1A027502750275ull, 0x1B025E025E025Eull, 0x1C024802480248ull, 0x1D023402340234ull, 0x1E022102210221ull, 0x1F021002100210ull,
    /*  32 */ 0x2001FF01FF01FFull, 0x2101F001F001F0ull, 0x2201E101E101E1ull, 0x2301D301D301D3ull, 0x2401C601C601C6ull, 0x2501BA01BA01BAull, 0x2601AE01AE01AEull, 0x2701A301A301A3ull,
    /*  40 */ 0x28019901990199ull, 0x29018F018F018Full, 0x2A018501850185ull, 0x2B017C017C017Cull, 0x2C017401740174ull, 0x2D016B016B016Bull, 0x2E016301630163ull, 0x2F015C015C015Cull,
    /*  48 */ 0x30015501550155ull, 0x31014E014E014Eull, 0x32014701470147ull, 0x33014001400140ull, 0x34013A013A013Aull, 0x35013401340134ull, 0x36012F012F012Full, 0x37012901290129ull,
    /*  56 */ 0x38012401240124ull, 0x39011F011F011Full, 0x3A011A011A011Aull, 0x3B011501150115ull, 0x3C011001100110ull, 0x3D010C010C010Cull, 0x3E010801080108ull, 0x3F010301030103ull,
    /*  64 */ 0x4000FF00FF00FFull, 0x4100FB00FB00FBull, 0x4200F800F800F8ull, 0x4300F400F400F4ull, 0x4400F000F000F0ull, 0x4500ED00ED00EDull, 0x4600E900E900E9ull, 0x4700E600E600E6ull,
    /*  72 */ 0x4800E300E300E3ull, 0x4900E000E000E0ull, 0x4A00DD00DD00DDull, 0x4B00DA00DA00DAull, 0x4C00D700D700D7ull, 0x4D00D400D400D4ull, 0x4E00D100D100D1ull, 0x4F00CF00CF00CFull,
    /*  80 */ 0x5000CC00CC00CCull, 0x5100CA00CA00CAull, 0x5200C700C700C7ull, 0x5300C500C500C5ull, 0x5400C200C200C2ull, 0x5500C000C000C0ull, 0x5600BE00BE00BEull, 0x5700BC00BC00BCull,
    /*  88 */ 0x5800BA00BA00BAull, 0x5900B700B700B7ull, 0x5A00B500B500B5ull, 0x5B00B300B300B3ull, 0x5C00B100B100B1ull, 0x5D00B000B000B0ull, 0x5E00AE00AE00AEull, 0x5F00AC00AC00ACull,
    /*  96 */ 0x6000AA00AA00AAull, 0x6100A800A800A8ull, 0x6200A700A700A7ull, 0x6300A500A500A5ull, 0x6400A300A300A3ull, 0x6500A200A200A2ull, 0x6600A000A000A0ull, 0x67009E009E009Eull,
    /* 104 */ 0x68009D009D009Dull, 0x69009B009B009Bull, 0x6A009A009A009Aull, 0x6B009800980098ull, 0x6C009700970097ull, 0x6D009600960096ull, 0x6E009400940094ull, 0x6F009300930093ull,
    /* 112 */ 0x70009200920092ull, 0x71009000900090ull, 0x72008F008F008Full, 0x73008E008E008Eull, 0x74008D008D008Dull, 0x75008B008B008Bull, 0x76008A008A008Aull, 0x77008900890089ull,
    /* 120 */ 0x78008800880088ull, 0x79008700870087ull, 0x7A008600860086ull, 0x7B008500850085ull, 0x7C008400840084ull, 0x7D008200820082ull, 0x7E008100810081ull, 0x7F008000800080ull,
    /* 128 */ 0x80007F007F007Full, 0x81007E007E007Eull, 0x82007D007D007Dull, 0x83007C007C007Cull, 0x84007C007C007Cull, 0x85007B007B007Bull, 0x86007A007A007Aull, 0x87007900790079ull,
    /* 136 */ 0x88007800780078ull, 0x89007700770077ull, 0x8A007600760076ull, 0x8B007500750075ull, 0x8C007400740074ull, 0x8D007400740074ull, 0x8E007300730073ull, 0x8F007200720072ull,
    /* 144 */ 0x90007100710071ull, 0x91007000700070ull, 0x92007000700070ull, 0x93006F006F006Full, 0x94006E006E006Eull, 0x95006D006D006Dull, 0x96006D006D006Dull, 0x97006C006C006Cull,
    /* 152 */ 0x98006B006B006Bull, 0x99006A006A006Aull, 0x9A006A006A006Aull, 0x9B006900690069ull, 0x9C006800680068ull, 0x9D006800680068ull, 0x9E006700670067ull, 0x9F006600660066ull,
    /* 160 */ 0xA0006600660066ull, 0xA1006500650065ull, 0xA2006500650065ull, 0xA3006400640064ull, 0xA4006300630063ull, 0xA5006300630063ull, 0xA6006200620062ull, 0xA7006200620062ull,
    /* 168 */ 0xA8006100610061ull, 0xA9006000600060ull, 0xAA006000600060ull, 0xAB005F005F005Full, 0xAC005F005F005Full, 0xAD005E005E005Eull, 0xAE005E005E005Eull, 0xAF005D005D005Dull,
    /* 176 */ 0xB0005D005D005Dull, 0xB1005C005C005Cull, 0xB2005B005B005Bull, 0xB3005B005B005Bull, 0xB4005A005A005Aull, 0xB5005A005A005Aull, 0xB6005900590059ull, 0xB7005900590059ull,
    /* 184 */ 0xB8005800580058ull, 0xB9005800580058ull, 0xBA005800580058ull, 0xBB005700570057ull, 0xBC005700570057ull, 0xBD005600560056ull, 0xBE005600560056ull, 0xBF005500550055ull,
    /* 192 */ 0xC0005500550055ull, 0xC1005400540054ull, 0xC2005400540054ull, 0xC3005300530053ull, 0xC4005300530053ull, 0xC5005300530053ull, 0xC6005200520052ull, 0xC7005200520052ull,
    /* 200 */ 0xC8005100510051ull, 0xC9005100510051ull, 0xCA005100510051ull, 0xCB005000500050ull, 0xCC005000500050ull, 0xCD004F004F004Full, 0xCE004F004F004Full, 0xCF004F004F004Full,
    /* 208 */ 0xD0004E004E004Eull, 0xD1004E004E004Eull, 0xD2004D004D004Dull, 0xD3004D004D004Dull, 0xD4004D004D004Dull, 0xD5004C004C004Cull, 0xD6004C004C004Cull, 0xD7004C004C004Cull,
    /* 216 */ 0xD8004B004B004Bull, 0xD9004B004B004Bull, 0xDA004B004B004Bull, 0xDB004A004A004Aull, 0xDC004A004A004Aull, 0xDD004A004A004Aull, 0xDE004900490049ull, 0xDF004900490049ull,
    /* 224 */ 0xE0004900490049ull, 0xE1004800480048ull, 0xE2004800480048ull, 0xE3004800480048ull, 0xE4004700470047ull, 0xE5004700470047ull, 0xE6004700470047ull, 0xE7004600460046ull,
    /* 232 */ 0xE8004600460046ull, 0xE9004600460046ull, 0xEA004500450045ull, 0xEB004500450045ull, 0xEC004500450045ull, 0xED004500450045ull, 0xEE004400440044ull, 0xEF004400440044ull,
    /* 240 */ 0xF0004400440044ull, 0xF1004300430043ull, 0xF2004300430043ull, 0xF3004300430043ull, 0xF4004300430043ull, 0xF5004200420042ull, 0xF6004200420042ull, 0xF7004200420042ull,
    /* 248 */ 0xF8004200420042ull, 0xF9004100410041ull, 0xFA004100410041ull, 0xFB004100410041ull, 0xFC004000400040ull, 0xFD004000400040ull, 0xFE004000400040ull, 0xFF004000400040ull};

static const uint64_t g_ArmyPreviewDownsampleAlphaRoundingBiasMmx = 0x100000000000000ull;

/* One 16-bit MMX lane per pixel byte: PUNPCKLBW mm,mm duplicates each byte into a word, PSRLW 4 scales it. */
#define ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, byteIndex) \
  ((uint64_t)((((pixel) >> ((byteIndex) * 8)) & 0xffu) * ARMY_PREVIEW_BYTE_TO_WORD_REPEAT >> 4) << ((byteIndex) * 16))
#define ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixel) \
  (ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 3) | ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 2) | \
   ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 1) | ARMY_PREVIEW_UNPACK_BYTE_LANE(pixel, 0))

/* One output pixel of the preview downsampling (MMX in the original): the four ARGB pixels of a 2x2 block are
   premultiplied by their alpha, summed per channel with the rounding bias and scaled by the reciprocal of their
   average alpha. */
static uint32_t ArmyPreview_AverageAlphaWeighted2x2(uint32_t pixelTopLeft,uint32_t pixelTopRight,
          uint32_t pixelBottomLeft,uint32_t pixelBottomRight)
{
  uint64_t topLeftLanes;
  uint64_t topRightLanes;
  uint64_t bottomLeftLanes;
  uint64_t bottomRightLanes;
  uint64_t alphaReciprocal;
  uint16_t channelSum0;
  uint16_t channelSum1;
  uint16_t channelSum2;
  uint16_t channelSum3;
  uint16_t channelClamp0;
  uint16_t channelClamp1;
  uint16_t channelClamp2;
  uint16_t channelClamp3;

  /* PUNPCKLBW mm,mm; PSRLW mm,4: each pixel byte b becomes the 16-bit lane (b * 0x101) >> 4. */
  topLeftLanes =
       pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelTopLeft),
              g_ArmyPreviewAlphaPremultiplyMmxLut256[pixelTopLeft >> 24]);
  topRightLanes =
       pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelTopRight),
              g_ArmyPreviewAlphaPremultiplyMmxLut256[pixelTopRight >> 24]);
  bottomLeftLanes =
       pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelBottomLeft),
              g_ArmyPreviewAlphaPremultiplyMmxLut256[pixelBottomLeft >> 24]);
  bottomRightLanes =
       pmulhw(ARMY_PREVIEW_UNPACK_PIXEL_LANES(pixelBottomRight),
              g_ArmyPreviewAlphaPremultiplyMmxLut256[pixelBottomRight >> 24]);
  alphaReciprocal =
       g_ArmyPreviewAverageAlphaReciprocalMmxLut256
       [((pixelTopLeft >> 24) + (pixelTopRight >> 24) + (pixelBottomLeft >> 24) + (pixelBottomRight >> 24)) >> 2];
  channelSum0 = ((short)topLeftLanes + (short)topRightLanes + (short)bottomLeftLanes + (short)bottomRightLanes +
                 (short)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx) * (short)alphaReciprocal;
  channelSum1 = ((short)(topLeftLanes >> 16) + (short)(topRightLanes >> 16) + (short)(bottomLeftLanes >> 16) +
                 (short)(bottomRightLanes >> 16) +
                 (short)((uint64_t)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 16)) *
                (short)(alphaReciprocal >> 16);
  channelSum2 = ((short)(topLeftLanes >> 32) + (short)(topRightLanes >> 32) + (short)(bottomLeftLanes >> 32) +
                 (short)(bottomRightLanes >> 32) +
                 (short)((uint64_t)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 32)) *
                (short)(alphaReciprocal >> 32);
  channelSum3 = ((short)(topLeftLanes >> 48) + (short)(topRightLanes >> 48) + (short)(bottomLeftLanes >> 48) +
                 (short)(bottomRightLanes >> 48) +
                 (short)((uint64_t)g_ArmyPreviewDownsampleAlphaRoundingBiasMmx >> 48)) *
                (short)(alphaReciprocal >> 48);
  channelClamp0 = channelSum0 >> 8;
  channelClamp1 = channelSum1 >> 8;
  channelClamp2 = channelSum2 >> 8;
  channelClamp3 = channelSum3 >> 8;
  /* PSRLW 8 leaves every lane <= 0xFF, so PACKUSWB never saturates: it just packs the low bytes. */
  return (uint32_t)(uint8_t)channelClamp3 << 24 | (uint32_t)(uint8_t)channelClamp2 << 16 |
         (uint32_t)(uint8_t)channelClamp1 << 8 | (uint32_t)(uint8_t)channelClamp0;
}

/* Renders the picture of an army type for the in-game panels: spawns a temporary army of armyAssetId for
   factionIndex, turns it to a fixed three-quarter view, frames its bounds and renders it off screen at twice the
   requested size, then destroys the army and downsamples the image 2x2 -> 1 with alpha weighting (MMX) into a
   previewWidth x previewHeight texture. Returns the texture, or NULL when creating the army or rendering failed.
*/
GraphicsTextureResource *ArmyRuntime_RenderPreviewTexture
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *rootNode;
  GameEntityRuntime *previewArmy;
  int boundsSpanY;
  GameEntityRuntime *previewTexture;
  int boundsSpanZ;
  int maxBoundsSpan;
  uint32_t *sourcePixels;
  uint32_t *destinationPixels;
  GraphicsPixelDimension remainingRows;
  GraphicsPixelDimension remainingColumns;
  int halvedWidth;
  int halvedHeight;
  uint32_t allocationSize;

  /* a temporary army at world position (ARMY_PREVIEW_WORLD_POSITION_Q12 on both axes) */
  previewArmy = ModelView_Cast<GameEntityRuntime>(ArmyRuntime_CreateInstanceFromAsset
                     (1,0,ARMY_PREVIEW_WORLD_POSITION_Q12,ARMY_PREVIEW_WORLD_POSITION_Q12,factionIndex,armyAssetId,
                      worldRuntime,nullptr));
  if (previewArmy == nullptr) {
    return nullptr;
  }
  rootNode = (previewArmy->common).ownership.modelNode;
  /* armies of runtime class 13 lose their fourth child node */
  if ((((previewArmy->common).ownership.modelRuntime()->definitionOrSavedId.
        runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) && (3 < rootNode->childCount)) &&
     (rootNode->childNodes[3] != nullptr)) {
    WorldRuntime_UnlinkOwnerListNode(ModelView_Cast<WorldOwnerListNode>(rootNode->childNodes[3]));
    rootNode->childNodes[3] = nullptr;
  }
  /* angles are 16-bit turns: 45 and 67.5 degrees */
  (rootNode->modelPayload).worldRotationAngle2 = FIXED_ANGLE16_EIGHTH_TURN;
  (rootNode->modelPayload).worldRotationAngle1 = 3 * FIXED_ANGLE16_FULL_TURN / 16;
  rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
  rootNode->tintArgb = 0xffffffff;
  ModelNodeRuntime_RebuildTransformsFromRoot(rootNode);
  /* start the bounds at the root position; the recursion widens them over all nodes */
  g_ModelBoundsMinimumX = (rootNode->worldTransform).translation.x;
  g_ModelBoundsMinimumY = (rootNode->worldTransform).translation.y;
  g_ModelBoundsMinimumZ = (rootNode->worldTransform).translation.z;
  g_ModelBoundsMaximumX = g_ModelBoundsMinimumX;
  g_ModelBoundsMaximumY = g_ModelBoundsMinimumY;
  g_ModelBoundsMaximumZ = g_ModelBoundsMinimumZ;
  ModelNodeRuntime_AccumulateTransformedBoundsRecursive(rootNode);
  boundsSpanY = g_ModelBoundsMaximumY - g_ModelBoundsMinimumY;
  boundsSpanZ = g_ModelBoundsMaximumZ - g_ModelBoundsMinimumZ;
  maxBoundsSpan = boundsSpanZ;
  if (boundsSpanZ < boundsSpanY) {
    maxBoundsSpan = boundsSpanY;
  }
  /* camera centred on the bounds in Y and Z, backed off by four times the larger span in X */
  g_ArmyPreviewViewParameters.originY = (boundsSpanY + g_ModelBoundsMinimumY * 2) >> 1;
  g_ArmyPreviewViewParameters.originZ = (boundsSpanZ + g_ModelBoundsMinimumZ * 2) >> 1;
  g_ArmyPreviewAuxiliaryOrientation[0] = ARMY_PREVIEW_AUXILIARY_ORIENTATION0_ANGLE16;
  g_ArmyPreviewAuxiliaryOrientation[1] = ARMY_PREVIEW_AUXILIARY_ORIENTATION1_ANGLE16;
  g_ArmyPreviewViewParameters.originX = g_ModelBoundsMaximumX + maxBoundsSpan * 4;
  /* the scene "extents" are the two scene colours */
  g_ArmyPreviewSceneExtents.horizontalExtent = (GraphicsSceneExtentFixed)ARMY_PREVIEW_PRIMARY_COLOR_ARGB;
  g_ArmyPreviewSceneExtents.verticalExtent = (GraphicsSceneExtentFixed)ARMY_PREVIEW_SECONDARY_COLOR_ARGB;
  g_ArmyPreviewViewParameters.projectionScale = Q12_ONE / 2;
  g_ArmyPreviewViewParameters.viewAngle0 = ARMY_PREVIEW_VIEW_ANGLE0;
  g_ArmyPreviewViewParameters.viewAngle1 = 0;
  g_ArmyPreviewViewParameters.projectionShift = 4;
  g_ArmyPreviewModelNode = rootNode;
  /* the texture is handled through the GameEntityRuntime view below (header fields by entity field names) */
  previewTexture = reinterpret_cast<GameEntityRuntime *>(g_GraphicsOffscreenRenderModelListToTextureSource
                     (&g_ArmyPreviewSceneExtents,
                      g_ArmyPreviewAuxiliaryOrientation,
                      &g_ArmyPreviewViewParameters,
                      previewHeight * 2,previewWidth * 2,1,
                      &g_ArmyPreviewModelNode));
  if (previewTexture == nullptr) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,previewArmy);
    return nullptr;
  }
  /* the texture is typed as GameEntityRuntime here: its pixels start at +0x220 and the header fields
     rewritten below (+0x200/+0x204 and +0x218/+0x21C) hold its width and height; +0x04 is the
     allocation size */
  sourcePixels = reinterpret_cast<uint32_t *>(&previewTexture[1].common.commandTarget.targetWorldXQ12); /* +0x220 */
  ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,previewArmy);
  /* downsample in place: each output pixel averages a 2x2 block of the double-size image */
  destinationPixels = sourcePixels;
  remainingRows = previewHeight;
  do {
    remainingColumns = previewWidth;
    do {
      *destinationPixels = ArmyPreview_AverageAlphaWeighted2x2
                             (sourcePixels[0],sourcePixels[1],sourcePixels[previewWidth * 2],
                              sourcePixels[previewWidth * 2 + 1]);
      sourcePixels = sourcePixels + 2;
      destinationPixels = destinationPixels + 1;
      remainingColumns = remainingColumns - 1;
    } while (remainingColumns != 0);
    sourcePixels = sourcePixels + previewWidth * 2; /* skip the second source row of the pair */
    remainingRows = remainingRows - 1;
  } while (remainingRows != 0);
  /* halve the stored sizes and shrink the allocation to the 0x220-byte header plus 32-bit pixels */
  halvedWidth = (int)previewTexture[1].common.commandFlags >> 1;
  halvedHeight = (int)previewTexture[1].common.commandTarget.targetEntity >> 1; /* 32-bit format field: GraphicsTextureSourceAsset header +0x204 (GameEntityRuntime view) */
  previewTexture[1].common.commandFlags = (GameEntityCommandFlags)halvedWidth;
  previewTexture[1].common.commandTarget.targetEntity = Thandor_U32ToPointer<GameEntityRuntime>(halvedHeight); /* 32-bit format field: GraphicsTextureSourceAsset header +0x204 (GameEntityRuntime view) */
  previewTexture[1].common.ownership.definitionOrClassRecord = Thandor_U32ToPointer(halvedWidth); /* 32-bit format field: GraphicsTextureSourceAsset header +0x218 (GameEntityRuntime view) */
  previewTexture[1].common.ownership.modelNode = Thandor_U32ToPointer<ModelRuntimeNode>(halvedHeight); /* 32-bit format field: GraphicsTextureSourceAsset header +0x21C (GameEntityRuntime view) */
  allocationSize = (uint32_t)(halvedWidth * halvedHeight * 4 + ARMY_PREVIEW_TEXTURE_HEADER_BYTES);
  (previewTexture->common).ownership.modelNode = Thandor_U32ToPointer<ModelRuntimeNode>(allocationSize); /* 32-bit format field: GraphicsTextureSourceAsset header +0x04 (GameEntityRuntime view) */
  g_MemoryApi.shrinkInPlace(allocationSize,previewTexture);
  return reinterpret_cast<GraphicsTextureResource *>(previewTexture); /* back from the entity view */
}

/* Frees the cached preview texture of every registered army record and re-renders the previews of the editor's
   unit- and object-placement selections into their image panels on the in-game UI root. Needed because the
   previews are drawn in the unit-placement owner faction's colours: called from the in-game keyboard dispatch
   table g_InGameKeyboardDispatchRecords (commands 0x10012 / 0x1001A) after
   g_UiCommandModeGOwnerFactionIndex was cycled.
*/
void ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(uint32_t uiRootAddress)

{
  uintptr_t resolvedTexture;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecord *registeredRecord;

  registryCursor = g_ArmyAssetRecordRegistry;
  for (registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    registeredRecord = ModelView_Cast<ArmyAssetRecord>(*registryCursor);
    if (registeredRecord != nullptr) {
      g_MemoryApi.free(Thandor_U32ToPointer<void>(registeredRecord->previewTexture)); /* 32-bit format field: ArmyAssetRecord.previewTexture (+0x20) */
      registeredRecord->previewTexture = 0;
    }
    registryCursor++;
  }
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
  ((UiImagePanelControl *)INGAME_UI(uiRootAddress,unitPlacementPreviewImage))->textureSource =
       reinterpret_cast<GraphicsTextureSourceAsset *>(resolvedTexture);
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
  ((UiImagePanelControl *)INGAME_UI(uiRootAddress,objectPlacementPreviewImage))->textureSource =
       reinterpret_cast<GraphicsTextureSourceAsset *>(resolvedTexture);
}

/* Returns the preview texture of a registered army asset for the editor's placement panels. The texture is
   cached in the record (previewTexture); on the first request it is rendered in the unit-placement
   owner faction's colours (faction 0 for ids from 400 up). Returns 0 for an unknown id or a failed render.
   Called directly by the in-game keyboard dispatch handlers (g_InGameKeyboardDispatchRecords),
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState and
   ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected.
*/
uintptr_t ArmyAssetRegistry_ResolveOrCreatePreviewTexture(uint32_t armyAssetRegistryId)

{
  ArmyAssetRecord *registeredRecord;
  int slotIndex;
  GraphicsTextureResource *previewTexture;
  FactionRuntimeIndex factionIndex;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    registeredRecord = ModelView_Cast<ArmyAssetRecord>(g_ArmyAssetRecordRegistry[slotIndex]);
    if (registeredRecord == nullptr || armyAssetRegistryId != registeredRecord->registryId) {
      continue;
    }
    if (registeredRecord->previewTexture != 0) {
      return registeredRecord->previewTexture;
    }
    factionIndex = g_UiCommandModeGOwnerFactionIndex;
    if (ARMY_ASSET_NEUTRAL_PREVIEW_FIRST_ID - 1 < armyAssetRegistryId) {
      factionIndex = 0;
    }
    previewTexture = ArmyRuntime_RenderPreviewTexture
                      (INGAME_UI(g_InGameRuntimeRoot,modePreviewPageStack)->layoutHeight,
                       INGAME_UI(g_InGameRuntimeRoot,modePreviewPageStack)->layoutHeight,
                       factionIndex,armyAssetRegistryId,&g_InGameRuntimeRoot->worldRuntime);
    if (previewTexture == nullptr) {
      return 0;
    }
    registeredRecord->previewTexture = Thandor_PointerToU32(previewTexture); /* 32-bit format field: ArmyAssetRecord.previewTexture (+0x20) */
    return (uintptr_t)previewTexture;
  }
  return 0; /* unknown id */
}
