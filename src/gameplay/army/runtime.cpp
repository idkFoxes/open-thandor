/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

__declspec(align(4)) void *g_ArmyRuntimeRebaseBaseMinusOne = 0;

ArmyRuntimeSlot *g_ArmyRuntimeSlots = 0;

ArmyGraphicsBinding g_ArmyGraphicsBindings[8] = {0};

/* uint32_t[24] depth-bin/occupancy class per model runtime class (0x88/0x90/0xA0/0xC0; 0x90 = structure), copied to ArmyRuntimeSlot.depthBinClass; gameplay/army runtime and placement */
const uint32_t g_ArmyRuntimeDepthBinClassByModelClass[24] = {
    /*  0 */ 136, 192, 192, 192, 144, 136, 136, 136, 136, 136, 136, 144, 136, 144, 144, 144,
    /* 16 */ 144, 192, 160, 192, 144, 192, 144, 144};

/* uint32_t[8]: preview army asset id per pointer mode (0 = none); gameplay/input/world.c */
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

static ModelRuntimeNode *g_ArmyPreviewModelNode = 0;

/* uint64_t[256] MMX qword per alpha a: three 16-bit lanes (a * 0x101) >> 4, alpha lane 0; PMULHW premultiply of the 2x2 downsample in the army preview (gameplay/army/runtime.c) */
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

/* uint64_t[256] MMX qword per average alpha a: three lanes ~0x3FF0/a (reciprocal), fourth lane a; un-premultiplies the averaged army preview pixel (gameplay/army/runtime.c) */
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

/* Implementation ownership: gameplay/army/runtime. */

/* Poses the aircraft body (the root's first child) on its vertical arc: the arc position (classState7C) gives the
   height above heightBaseQ12 (arcCoefficient * ticks * position^2) and the pitch, then the arc position advances
   by one movement step for this batch of ticks. Returns that step. */
static int ArmyAircraft_PoseOnVerticalArc(ModelRuntimeClass21UpdateView *modelRuntime,
          ArmyRuntimeClassUpdate21DefinitionView *definition,int arcCoefficient,Q12 heightBaseQ12)
{
  ModelRuntimeNode *bodyNode;
  uint32_t arcPosition;
  int64_t product64;
  uint32_t pitchAngle;
  int stepQ12;

  arcPosition = (modelRuntime->classLinkState).classState7C;
  product64 = (int64_t)(int)arcPosition * (int64_t)(int)arcPosition;
  bodyNode = modelRuntime->rootModelNode->childNodes[0];
  product64 = (int64_t)(int)(arcCoefficient * g_InGameSimulationStepTicks) *
           (int64_t)(int)FIXED_PRODUCT_SHR(product64,Q12_SHIFT);
  (bodyNode->modelPayload).localRotationAngle0 = FIXED_ANGLE16_HALF_TURN;
  (bodyNode->modelPayload).localTranslationZQ12 = FIXED_PRODUCT_SHR(product64,Q12_SHIFT) + heightBaseQ12;
  product64 = (int64_t)(int)(arcPosition * g_InGameSimulationStepTicks) * (int64_t)arcCoefficient;
  pitchAngle = FixedMath_Atan2Angle16(FIXED_PRODUCT_SHR(product64,11),Q12_ONE);
  stepQ12 = definition->movementStepQ12 * g_InGameSimulationStepTicks;
  (modelRuntime->classLinkState).classState7C = (modelRuntime->classLinkState).classState7C + stepQ12;
  (bodyNode->modelPayload).localRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN - pitchAngle & FIXED_ANGLE16_MASK;
  return stepQ12;
}

/* One tick batch of the take-off or landing arc over the home pad: the body follows the vertical arc above the
   parked height (classState80) and the aircraft moves one step along its heading. */
static void ArmyAircraft_FlyPadArc(ModelRuntimeClass21UpdateView *modelRuntime,
          ArmyRuntimeClassUpdate21DefinitionView *definition)
{
  int stepQ12;
  ModelRuntimeNode *rootNode;
  FixedSinCos sinCosStep;

  stepQ12 = ArmyAircraft_PoseOnVerticalArc
                      (modelRuntime,definition,definition->arcCoefficient,
                       (Q12)(modelRuntime->classLinkState).classState80);
  rootNode = modelRuntime->rootModelNode;
  sinCosStep = FixedMath_SinCosScaled((rootNode->modelPayload).worldRotationAngle2,stepQ12);
  (rootNode->worldTransform).translation.x = (rootNode->worldTransform).translation.x + sinCosStep.cosValue;
  (rootNode->worldTransform).translation.y = (rootNode->worldTransform).translation.y + sinCosStep.sinValue;
}

/* Parked on the pad: takes off once the hangar is ready (or there is no pad), is removed when the hangar is idle
   or closed, otherwise rides on the pad's lift (copies its height). */
static void ArmyAircraft_UpdateParked(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *rootNode;
  Q12 padDeckHeightQ12;

  definition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  if ((homeModelRuntime == NULL) || ((homeModelRuntime->classState).classStateB0 == ARMY_PAD_HANGAR_READY)) {
    (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_TAKING_OFF;
    (modelRuntime->classLinkState).classState7C = 0;
    (modelRuntime->classLinkState).classState64 = definition->phaseInitial;
    (modelRuntime->classLinkState).classState68 = definition->phaseDuration;
  }
  else if (((homeModelRuntime->classState).classStateB0 == ARMY_PAD_HANGAR_IDLE) ||
          ((homeModelRuntime->classState).classStateB0 == ARMY_PAD_HANGAR_CLOSED)) {
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
               THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelRuntime = (ModelRuntimeSlot *)modelRuntime },
               (rootNode->modelPayload).worldRotationAngle2,
               (rootNode->modelPayload).worldRotationAngle1,
               (rootNode->modelPayload).worldRotationAngle0,
               (rootNode->worldTransform).translation.z,
               (rootNode->worldTransform).translation.y,
               (rootNode->worldTransform).translation.x,definition->removalEffect,worldRuntime);
    (modelRuntime->class21State).stateFlags =
         (modelRuntime->class21State).stateFlags | ARMY_MODEL_STATE_DESTRUCTION_STARTED;
  }
  else {
    padDeckHeightQ12 = (((homeModelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
                       localTranslationZQ12;
    (rootNode->childNodes[0]->modelPayload).localTranslationZQ12 = padDeckHeightQ12;
    (modelRuntime->classLinkState).classState80 = padDeckHeightQ12;
  }
}

/* Touch-down on the home pad: the hangar starts lowering (with its sound). The pad's health, scaled to the
   aircraft's maximum health, is compared with the aircraft's health: when the aircraft has less, the pad takes
   half the difference as damage. */
static void ArmyAircraft_TouchDownOnPad(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeSlot *homeDefinitionSlot;
  int scaledPadHealth;
  uint32_t healthDifference;

  definition = modelRuntime->modelDefinition;
  homeDefinitionSlot = (ModelRuntimeSlot *)(homeModelRuntime->definitionOrSavedId).savedIdOrOffset;
  (homeModelRuntime->classState).classStateB0 = ARMY_PAD_HANGAR_LOWERING;
  ModelRuntime_PlayDefinitionSecondaryOneShotSound(homeModelRuntime,worldRuntime);
  scaledPadHealth = (int)(((int64_t)(int)homeModelRuntime->health * (int64_t)definition->maximumHealth) /
                          (int64_t)(homeDefinitionSlot->classLinkState).modelLinkOrState.signedScalarState);
  healthDifference = scaledPadHealth - modelRuntime->health;
  if (healthDifference != 0 && (int)modelRuntime->health <= scaledPadHealth) {
    ArmyRuntime_ApplyDamageAndPropagateToParent(healthDifference >> 1,homeModelRuntime);
  }
}

/* Landing arc onto the pad; when the landing countdown (classState68) runs out the aircraft is parked again. */
static void ArmyAircraft_UpdateLanding(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyAircraft_FlyPadArc(modelRuntime,modelRuntime->modelDefinition);
  (modelRuntime->classLinkState).classState68 = (modelRuntime->classLinkState).classState68 - 1;
  if ((int)(modelRuntime->classLinkState).classState68 < 0) {
    (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_PARKED;
    if (homeModelRuntime != NULL) {
      ArmyAircraft_TouchDownOnPad(worldRuntime,modelRuntime,homeModelRuntime);
    }
  }
}

/* Take-off arc from the pad: the hangar starts lowering when the countdown classState64 reaches 0; when the
   countdown classState68 runs out the aircraft leaves the map and starts its approach. */
static void ArmyAircraft_UpdateTakingOff(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ModelRuntimeNode *rootNode;

  ArmyAircraft_FlyPadArc(modelRuntime,modelRuntime->modelDefinition);
  rootNode = modelRuntime->rootModelNode;
  (modelRuntime->classLinkState).classState64 = (modelRuntime->classLinkState).classState64 - 1;
  if (((modelRuntime->classLinkState).classState64 == 0) && (homeModelRuntime != NULL)) {
    (homeModelRuntime->classState).classStateB0 = ARMY_PAD_HANGAR_LOWERING;
    ModelRuntime_PlayDefinitionSecondaryOneShotSound(homeModelRuntime,worldRuntime);
  }
  (modelRuntime->classLinkState).classState68 = (modelRuntime->classLinkState).classState68 - 1;
  if ((int)(modelRuntime->classLinkState).classState68 < 0) {
    (rootNode->worldTransform).translation.x = ARMY_AIRCRAFT_OFF_MAP_X_Q12;
    (rootNode->worldTransform).translation.y = ARMY_AIRCRAFT_OFF_MAP_Y_Q12;
    (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_APPROACH;
  }
}

/* Approach: as soon as ArmyRuntime_TestWorldPointAllowedDefault rejects the target point (classState70/74), the
   aircraft is placed travelStepCount movement steps behind it on the attack heading (classState78) and starts
   the attack run (drop countdown armyLinkOrState = travelStepCount, run length classState68 = twice that). The
   run height is the highest terrain sampled every 10 movement steps along the run, plus half a unit and twice
   the parked height (classState80). */
static void ArmyAircraft_UpdateApproach(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *rootNode;
  Bool8 pointAllowed;
  AngleTurn32 attackHeading;
  FixedSinCos sinCosStep;
  uint32_t travelSteps;
  uint32_t sampleCountdown;
  Q12 sampleXQ12;
  Q12 sampleYQ12;
  int maxTerrainHeightQ12;
  Q12 terrainHeightQ12;
  uint32_t parkedHeightQ12;

  definition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  pointAllowed = ArmyRuntime_TestWorldPointAllowedDefault
                     (definition->worldPointAllowedContext,
                      (modelRuntime->classLinkState).classState74,
                      (modelRuntime->classLinkState).classState70);
  if (pointAllowed) {
    return;
  }
  attackHeading = (modelRuntime->classLinkState).classState78;
  sinCosStep = FixedMath_SinCosScaled
                     (attackHeading ^ FIXED_ANGLE16_HALF_TURN,definition->movementStepQ12 * definition->travelStepCount);
  (rootNode->worldTransform).translation.x = sinCosStep.cosValue + (modelRuntime->classLinkState).classState70;
  (rootNode->worldTransform).translation.y = sinCosStep.sinValue + (modelRuntime->classLinkState).classState74;
  (rootNode->modelPayload).worldRotationAngle2 = attackHeading;
  travelSteps = definition->travelStepCount;
  (modelRuntime->classLinkState).armyLinkOrState.classState = travelSteps;
  sampleCountdown = travelSteps * 2;
  (modelRuntime->classLinkState).classState68 = sampleCountdown;
  (modelRuntime->classLinkState).classState7C = (0 - travelSteps) * definition->movementStepQ12;
  (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_ATTACK_RUN;
  sinCosStep = FixedMath_SinCosScaled(attackHeading,definition->movementStepQ12 * 10);
  sampleXQ12 = (rootNode->worldTransform).translation.x;
  sampleYQ12 = (rootNode->worldTransform).translation.y;
  maxTerrainHeightQ12 = 0;
  parkedHeightQ12 = (modelRuntime->classLinkState).classState80;
  do {
    FieldGrid_InterpolateTopSurfaceHeight(sampleYQ12,sampleXQ12,worldRuntime->fieldGrid,&terrainHeightQ12);
    if (maxTerrainHeightQ12 < terrainHeightQ12) {
      maxTerrainHeightQ12 = terrainHeightQ12;
    }
    sampleXQ12 = sampleXQ12 + sinCosStep.cosValue;
    sampleYQ12 = sampleYQ12 + sinCosStep.sinValue;
    sampleCountdown = sampleCountdown - 10;
  } while ((int)sampleCountdown >= 0);
  (modelRuntime->class21State).trajectoryTerrainReferenceHeightQ12 =
       maxTerrainHeightQ12 + Q12_ONE / 2 + parkedHeightQ12 * 2;
}

/* Releases one model-point effect of the attack run when the drop countdown (armyLinkOrState) sits on a mark: the marks
   are modelPointStep apart from three steps before the target point (model point 7) to three steps after it
   (model point 1). Nothing is dropped when modelPointStep is 0. */
static void ArmyAircraft_DropModelPointEffectAtMark(WorldRuntimeContext *worldRuntime,
          ModelRuntimeClass21UpdateView *modelRuntime,ArmyRuntimeClassUpdate21DefinitionView *definition)
{
  ModelRuntimeNode *bodyNode;
  Q12 dropZQ12;
  void *modelPointTable;
  uint32_t countdownMark;
  ModelAttachmentOrdinal modelPointOrdinal;

  bodyNode = modelRuntime->rootModelNode->childNodes[0];
  countdownMark = definition->modelPointStep * -3;
  dropZQ12 = (bodyNode->worldTransform).translation.z - 2 * Q12_ONE;
  if (definition->modelPointStep == 0) {
    return;
  }
  modelPointTable = (void *)((MdlSerializedNodeHeader *)definition->rootNode)->childSerializedOffsets[0];
  for (modelPointOrdinal = 7; modelPointOrdinal != 0; modelPointOrdinal = modelPointOrdinal - 1) {
    if (countdownMark == (modelRuntime->classLinkState).armyLinkOrState.classState) {
      ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
                (0,dropZQ12,(bodyNode->worldTransform).translation.y,(bodyNode->worldTransform).translation.x,
                 modelPointOrdinal,definition->modelPointEffectId,bodyNode,modelPointTable,worldRuntime);
      return;
    }
    countdownMark = countdownMark + definition->modelPointStep;
  }
}

/* Attack run: flies along the heading at the run height (trajectoryTerrainReferenceHeightQ12, measured from the
   terrain below), then per tick counts down the drop countdown (armyLinkOrState) and the run (classState68,
   leaving the map when it runs out), plays the
   mapped terrain sound and drops the model-point effects at their marks. */
static void ArmyAircraft_UpdateAttackRun(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *rootNode;
  FixedSinCos sinCosStep;
  Q12 terrainHeightQ12;
  InGameSimulationStepBatchTicks remainingTicks;

  definition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  sinCosStep = FixedMath_SinCosScaled
                     ((rootNode->modelPayload).worldRotationAngle2,
                      g_InGameSimulationStepTicks * definition->movementStepQ12);
  (rootNode->worldTransform).translation.x = (rootNode->worldTransform).translation.x + sinCosStep.cosValue;
  (rootNode->worldTransform).translation.y = (rootNode->worldTransform).translation.y + sinCosStep.sinValue;
  FieldGrid_InterpolateTopSurfaceHeight
            ((rootNode->worldTransform).translation.y,
             (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid,&terrainHeightQ12);
  ArmyAircraft_PoseOnVerticalArc
            (modelRuntime,definition,definition->verticalArcCoefficient,
             (modelRuntime->class21State).trajectoryTerrainReferenceHeightQ12 - terrainHeightQ12);
  remainingTicks = g_InGameSimulationStepTicks;
  do {
    (modelRuntime->classLinkState).armyLinkOrState.classState =
         (modelRuntime->classLinkState).armyLinkOrState.classState - 1; /* the drop countdown */
    (modelRuntime->classLinkState).classState68 = (modelRuntime->classLinkState).classState68 - 1;
    if ((int)(modelRuntime->classLinkState).classState68 < 0) {
      (rootNode->worldTransform).translation.x = ARMY_AIRCRAFT_OFF_MAP_X_Q12;
      (rootNode->worldTransform).translation.y = ARMY_AIRCRAFT_OFF_MAP_Y_Q12;
      (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_RETURNING;
    }
    ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
              (modelRuntime->ownerArmyRuntime->factionIndex,
               (rootNode->worldTransform).translation.y,
               (rootNode->worldTransform).translation.x,
               definition->terrainSoundAssetIndex,worldRuntime);
    ArmyAircraft_DropModelPointEffectAtMark(worldRuntime,modelRuntime,definition);
    remainingTicks = remainingTicks - 1;
  } while (remainingTicks != 0);
}

/* Returning (off the map): once the home pad's hangar is idle and ArmyRuntime_TestWorldPointAllowedDefault
   rejects the pad position, the hangar starts opening (with its sound) and the aircraft is placed phaseDuration
   movement steps behind the pad on the pad's heading to fly its landing arc. */
static void ArmyAircraft_TryStartLanding(WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime,
          ModelRuntimeSlot *homeModelRuntime)
{
  ArmyRuntimeClassUpdate21DefinitionView *definition;
  ModelRuntimeNode *padNode;
  ModelRuntimeNode *aircraftNode;
  Bool8 pointAllowed;
  AngleTurn32 padHeading;
  FixedSinCos sinCosStep;
  uint32_t landingSteps;

  definition = modelRuntime->modelDefinition;
  padNode = (homeModelRuntime->rootModelNodeOrSavedOffset).modelNode;
  if ((homeModelRuntime->classState).classStateB0 != ARMY_PAD_HANGAR_IDLE) {
    return;
  }
  pointAllowed = ArmyRuntime_TestWorldPointAllowedDefault
                     (definition->worldPointAllowedContext,
                      (padNode->worldTransform).translation.y,
                      (padNode->worldTransform).translation.x);
  if (pointAllowed) {
    return;
  }
  (homeModelRuntime->classState).classStateB0 = ARMY_PAD_HANGAR_OPENING;
  ModelRuntime_PlayDefinitionPrimaryOneShotSound(homeModelRuntime,worldRuntime);
  padHeading = (padNode->modelPayload).worldRotationAngle2;
  sinCosStep = FixedMath_SinCosScaled
                     (padHeading ^ FIXED_ANGLE16_HALF_TURN,definition->movementStepQ12 * definition->phaseDuration);
  aircraftNode = modelRuntime->rootModelNode;
  (aircraftNode->worldTransform).translation.x = sinCosStep.cosValue + (padNode->worldTransform).translation.x;
  (aircraftNode->worldTransform).translation.y = sinCosStep.sinValue + (padNode->worldTransform).translation.y;
  (aircraftNode->modelPayload).worldRotationAngle2 = padHeading;
  landingSteps = definition->phaseDuration;
  (modelRuntime->classLinkState).classState68 = landingSteps;
  (modelRuntime->classLinkState).classState7C = (0 - landingSteps) * definition->movementStepQ12;
  (modelRuntime->class21State).behaviorState = ARMY_AIRCRAFT_STATE_LANDING;
}

/* Runtime update of the aircraft class (MODEL_RUNTIME_CLASS_21_AIRCRAFT), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[21] (called by
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive). State machine in behaviorState around the
   aircraft's home pad (classLinkState.modelLinkOrState): 1 parked on the pad, 3 take-off arc and off the map,
   4/5 re-entry along the attack heading at a height above the highest terrain on the path, dropping its
   model-point effects on a countdown, 6 off the map again, 2 landing arc back onto the pad. Afterwards the model
   is re-seated on the terrain and its transforms, emitters and damage effect are refreshed.
*/

void ArmyRuntimeClass_UpdateAircraft
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime)

{
  ModelRuntimeSlot *homeModelRuntime;
  ModelRuntimeNode *modelNode;
  Q12 childHeightZ;
  ModelDefinition *semanticDefinition;
  uint32_t behaviorState;
  AngleTurn32 savedAngle0;
  AngleTurn32 savedAngle1;

  homeModelRuntime = (modelRuntime->classLinkState).modelLinkOrState.modelRuntime;
  modelNode = modelRuntime->rootModelNode;
  switch((modelRuntime->class21State).behaviorState) {
  case ARMY_AIRCRAFT_STATE_PARKED:
    ArmyAircraft_UpdateParked(worldRuntime,modelRuntime,homeModelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_LANDING:
    ArmyAircraft_UpdateLanding(worldRuntime,modelRuntime,homeModelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_TAKING_OFF:
    ArmyAircraft_UpdateTakingOff(worldRuntime,modelRuntime,homeModelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_APPROACH: /* line up behind the target point (classState70/74) on heading classState78 */
    ArmyAircraft_UpdateApproach(worldRuntime,modelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_ATTACK_RUN: /* releases the model-point effects when the drop countdown hits a mark */
    ArmyAircraft_UpdateAttackRun(worldRuntime,modelRuntime);
    break;
  case ARMY_AIRCRAFT_STATE_RETURNING:
    if (homeModelRuntime != NULL) {
      ArmyAircraft_TryStartLanding(worldRuntime,modelRuntime,homeModelRuntime);
      break;
    }
    ModelRuntimeHierarchy_MarkDestroyedRecursive(worldRuntime,modelRuntime->ownerArmyRuntime);
    /* no home pad left: falls through */
  case ARMY_AIRCRAFT_STATE_NO_PAD:
    if (homeModelRuntime == NULL) {
      childHeightZ = Q12_ONE;
    }
    else {
      childHeightZ = (((homeModelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0]->modelPayload).
               localTranslationZQ12;
    }
    (modelNode->childNodes[0]->modelPayload).localTranslationZQ12 = childHeightZ;
  }
  modelNode = modelRuntime->rootModelNode;
  savedAngle0 = (modelNode->modelPayload).worldRotationAngle0;
  savedAngle1 = (modelNode->modelPayload).worldRotationAngle1;
  (*g_ArmyPlacementContactKindDispatchTable.callbacks
    [((ModelDefinition *)modelRuntime->modelDefinition)->
     placementContactKindIndex])
            (((ModelDefinition *)modelRuntime->modelDefinition)->
             placementHeightOffsetQ12,(modelNode->worldTransform).translation.y,
             (modelNode->worldTransform).translation.x,modelNode,worldRuntime);
  behaviorState = (modelRuntime->class21State).behaviorState;
  (modelNode->modelPayload).worldRotationAngle1 = savedAngle1;
  (modelNode->modelPayload).worldRotationAngle0 = savedAngle0;
  if ((behaviorState != ARMY_AIRCRAFT_STATE_NO_PAD) && (behaviorState != ARMY_AIRCRAFT_STATE_PARKED)) {
    ArmyRuntime_UpdateTimedShotAndEffectEmitters
              (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
  }
  semanticDefinition = (ModelDefinition *)modelRuntime->modelDefinition;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNode);
  ModelNodeRuntime_UpdateDepthBinMasks(semanticDefinition->footprintRadius,modelNode);
  return;
}


/* Takes the first queued pad asset (asset flag 8) the faction can pay for out of its secondary asset queue: the
   Xenite is paid, the build interval and the asset's Energy load (held while building) are stored in the pad, and
   the pad starts producing. Builds one asset at a time. */
static void ArmyPad_StartBuildingFirstAffordableAsset(ModelRuntimeLinkedChildSpawnAndBuildView *padRuntime,
          FactionRuntimeIndex factionIndex)
{
  FactionArmyAssetCount remainingAssetCount;
  uint32_t *queueEntry;
  ArmyAssetRecord *candidateAsset;
  uint32_t buildTicks;
  uint32_t selectedAssetValue;
  PckArmyAssetIdCatalog secondaryAssetId;

  remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  queueEntry = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  for (; remainingAssetCount != 0; remainingAssetCount = remainingAssetCount - 1, queueEntry = queueEntry + 1) {
    /* queued asset record: build ticks (buildTicks), Xenite cost (xeniteCostQ4), Energy load
       (energyLoadQ4), the sums of its model definitions' build metrics */
    candidateAsset = (ArmyAssetRecord *)*queueEntry;
    if (((candidateAsset->flags & ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD) == 0) ||
       (g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 < candidateAsset->xeniteCostQ4)) {
      continue;
    }
    g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - candidateAsset->xeniteCostQ4;
    buildTicks = candidateAsset->buildTicks;
    selectedAssetValue = candidateAsset->energyLoadQ4;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
      buildTicks = (buildTicks >> 4) + 1;
    }
    secondaryAssetId = candidateAsset->registryId;
    (padRuntime->linkedChildBuildState).secondaryArmyAssetBuildRequiredTicks = buildTicks;
    (padRuntime->linkedChildBuildState).selectedSecondaryArmyAssetValue = selectedAssetValue;
    (padRuntime->linkedChildBuildState).selectedSecondaryArmyAssetId = secondaryAssetId;
    padRuntime->energyLoadQ4 = padRuntime->energyLoadQ4 + selectedAssetValue;
    (padRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks = 0;
    g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount =
         g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount - 1;
    /* remove the entry: shift the rest of the queue down by one */
    do {
      *queueEntry = queueEntry[1];
      queueEntry = queueEntry + 1;
      remainingAssetCount = remainingAssetCount - 1;
    } while (remainingAssetCount != 0);
    padRuntime->secondaryArmyAssetBuildState = 1;
    padRuntime->linkedChildRuntimeFlags = padRuntime->linkedChildRuntimeFlags | ARMY_MODEL_STATE_PRODUCING;
    return;
  }
}

/* Plays one hangar sound of the aircraft pad (an index into the world's sound asset workspace; 0 = none) at the
   pad, unless the pad's terrain cell has mask bits 0/1 set. */
static void ArmyPadHangar_PlaySound(WorldRuntimeContext *worldRuntime,ModelDefinitionLinkedChildStateView *padDefinition,
          ModelRuntimeNode *padNode,SoundAssetIndex soundAssetIndex)
{
  DirectSoundVoiceSet **soundVoiceSet;

  if ((soundAssetIndex == 0) || (worldRuntime->dwordArrayCount <= soundAssetIndex) ||
     (worldRuntime->dwordArray == NULL)) {
    return;
  }
  soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
  if (soundVoiceSet == NULL) {
    return;
  }
  if (!TerrainGrid_TestProjectedCellMaskBits01
                ((padNode->worldTransform).translation.y,(padNode->worldTransform).translation.x,worldRuntime)) {
    SpatialSound_PlayPositionedOneShot
              (padDefinition->positionedSoundMaximumDistanceQ12,padDefinition->positionedSoundGainQ15,
               &(padNode->worldTransform).translation,soundVoiceSet);
  }
}

/* Consumes one pending launch of a linked aircraft slot and tries to create the aircraft. On success the hangar
   starts opening (with its transition sound) and true is returned. */
static Bool8 ArmyPadHangar_TryLaunchPendingAircraft(WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView *padRuntime,uint8_t *pendingSpawnCount,
          ModelRuntimeLinkedChildSpawnInheritedState *inheritedState,PckArmyAssetIdCatalog linkedArmyAssetId,
          ModelDefinitionLinkedChildStateView *padDefinition,ModelRuntimeNode *padNode)
{
  Bool8 spawnFailed;

  *pendingSpawnCount = *pendingSpawnCount - 1;
  spawnFailed = ArmyRuntimeSpawner_CreateLinkedChildInstance
                          (inheritedState->inheritedValue78,inheritedState->inheritedValue74,
                           inheritedState->inheritedValue70,linkedArmyAssetId,worldRuntime,
                           (ArmyRuntimeLinkedChildMaskSlotView *)padRuntime);
  if (spawnFailed) {
    return false;
  }
  padRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_OPENING;
  ArmyPadHangar_PlaySound(worldRuntime,padDefinition,padNode,padDefinition->linkedChildTransitionSoundAssetIndex);
  return true;
}

/* Runtime update of the aircraft home pad class (22), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[22]. While the pad is being dismantled it
   passes the dismantling on to every aircraft (class 21) based on it. It builds one queued secondary army asset
   (asset flag 8) at a time: Xenite is paid up front, the asset's Energy load is held while building, and the
   finished id goes into a free slot of completedSecondaryArmyAssetIds (with a notification for the active
   faction). Then it drives the hangar transition in linkedChildTransitionState (1 open, 2 lift, 4 lower,
   5 close, 6/0 idle) and launches pending linked aircraft.
*/

void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode(WorldRuntimeContext *worldRuntime,
          ModelRuntimeLinkedChildSpawnAndBuildView *modelRuntime)

{
  ModelRuntimeSlot *ownerPayload;
  FactionRuntimeIndex factionIndex;
  Q12 translationLimitQ12;
  uint32_t completedAssetValue;
  PckArmyAssetIdCatalog secondaryAssetId;
  ModelRuntimeNode *modelNodeRuntime;
  ModelRuntimeNode *childNode;
  ModelDefinition *linkedModelDefinition;
  int reverseSlotIndex;
  uint32_t elapsedTicks;
  uint32_t energyLoadBefore;
  WorldOwnerListNode *ownerNode;
  ArmyAssetRecordPrefix *assetRecord;
  ModelDefinitionRecordPrefix *selectedDefinition;
  GraphicsFixedVec3 loweredChildPosition;
  Q12 translationStep;
  InGameNotificationMovieId notificationMovieId;
  ModelDefinitionLinkedChildStateView *linkedChildDefinition;
  ArmyRuntimeSlot *ownerArmyRuntime;

  if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_DISMANTLING) != 0) {
    /* every aircraft (class 21) based on this pad (its classLinkState.modelLinkOrState) that is not dismantling
       already */
    ownerNode = worldRuntime->ownerListHead;
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        ownerPayload = (ModelRuntimeSlot *)ownerNode->runtimePayload;
        if ((ownerPayload->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
             MODEL_RUNTIME_CLASS_21_AIRCRAFT) &&
           (modelRuntime == (ModelRuntimeLinkedChildSpawnAndBuildView *)
                            ownerPayload->classLinkState.modelLinkOrState.modelRuntime) &&
           ((ownerPayload->classState.stateFlags & ARMY_MODEL_STATE_DISMANTLING) == 0)) {
          ModelRuntimeHierarchy_MarkDestroyedRecursive
                    (worldRuntime,ownerPayload->ownerArmyRuntimeOrSavedOffset.armyRuntime);
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != NULL);
  }
  switch(modelRuntime->secondaryArmyAssetBuildState) {
  case 0:
    if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) {
        factionIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
        if ((modelRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount <
            modelRuntime->modelDefinition->linkedChildSlotCapacity) {
          ArmyPad_StartBuildingFirstAffordableAsset(modelRuntime,factionIndex);
        }
      }
    }
    else if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
    }
    break;
  case 1:
    if ((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      (modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks =
           (modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks + g_InGameSimulationStepTicks;
      ownerArmyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters
                (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
      elapsedTicks = (modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildElapsedTicks;
      ArmyRuntime_UpdateAnimatedModelSubnodes
                (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
      if ((modelRuntime->linkedChildBuildState).secondaryArmyAssetBuildRequiredTicks <= elapsedTicks)
      {
        factionIndex = ownerArmyRuntime->factionIndex;
        completedAssetValue = (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetValue;
        (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetValue = 0;
        modelRuntime->secondaryArmyAssetBuildState = 0;
        modelRuntime->linkedChildRuntimeFlags =
             modelRuntime->linkedChildRuntimeFlags & ~ARMY_MODEL_STATE_PRODUCING;
        energyLoadBefore = modelRuntime->energyLoadQ4;
        modelRuntime->energyLoadQ4 = energyLoadBefore - completedAssetValue;
        secondaryAssetId = (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetId;
        (modelRuntime->linkedChildBuildState).selectedSecondaryArmyAssetId = 0;
        if (completedAssetValue <= energyLoadBefore) {
          /* the last free slot of completedSecondaryArmyAssetIds (scanning down from the capacity) */
          for (reverseSlotIndex = modelRuntime->modelDefinition->linkedChildSlotCapacity - 1; -1 < reverseSlotIndex;
               reverseSlotIndex = reverseSlotIndex - 1) {
            if (modelRuntime->completedSecondaryArmyAssetIds[reverseSlotIndex] == 0) {
              break;
            }
          }
          if (reverseSlotIndex < 0) {
            reverseSlotIndex = 0; /* no free slot: overwrite the first */
          }
          modelRuntime->completedSecondaryArmyAssetIds[reverseSlotIndex] = secondaryAssetId;
          g_GameFactionRuntimeImage.records[factionIndex].relationCounterA =
               g_GameFactionRuntimeImage.records[factionIndex].relationCounterA + 1;
          (modelRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount =
               (modelRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount + 1;
          (modelRuntime->linkedChildBuildState).classState70 =
               (modelRuntime->linkedChildBuildState).classState70 + 1;
          if (factionIndex == worldRuntime->activeFactionRuntimeIndex) {
            /* Original quirk: the lookup status is not checked (an unknown id leaves the error code in
               assetRecord) */
            ArmyAssetRegistry_FindById(secondaryAssetId,&assetRecord);
            selectedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                               (ownerArmyRuntime->factionIndex,assetRecord->rootNodeOffsetOrPointer);
            linkedModelDefinition = (ModelDefinition *)selectedDefinition;
            linkedModelDefinition->builtCount = linkedModelDefinition->builtCount + 1;
            notificationMovieId = linkedModelDefinition->firstBuiltNotificationMovieId;
            if (linkedModelDefinition->builtCount != 1) {
              notificationMovieId = linkedModelDefinition->nextBuiltNotificationMovieId;
            }
            InGameNotificationQueue_InsertPriorityRecord
                      (NOTIFICATION_PAYLOAD_NONE,0,(worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                       translation.y,
                       (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                       translation.x,3,notificationMovieId);
          }
        }
      }
    }
  }
  linkedChildDefinition = modelRuntime->modelDefinition;
  modelNodeRuntime = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
  switch(modelRuntime->linkedChildTransitionState) {
  case ARMY_PAD_HANGAR_OPENING: /* scroll the hatch texture */
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV +
         linkedChildDefinition->linkedChildTextureVStepPerTick * g_InGameSimulationStepTicks;
    if (ARMY_DOOR_TEXTURE_OPEN_V - 1 < modelNodeRuntime->primaryTextureOffsetV) {
      modelNodeRuntime->primaryTextureOffsetV = ARMY_DOOR_TEXTURE_OPEN_V;
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_LIFTING;
      ArmyPadHangar_PlaySound(worldRuntime,linkedChildDefinition,modelNodeRuntime,
                              linkedChildDefinition->linkedChildTransitionEndSoundAssetIndex);
    }
    break;
  case ARMY_PAD_HANGAR_LIFTING: /* the platform is child 0 */
    childNode = modelNodeRuntime->childNodes[0];
    translationLimitQ12 = linkedChildDefinition->linkedChildTranslationLimitQ12;
    (childNode->modelPayload).localTranslationZQ12 =
         (childNode->modelPayload).localTranslationZQ12 +
         linkedChildDefinition->linkedChildTranslationStepQ12PerTick * g_InGameSimulationStepTicks;
    if (translationLimitQ12 < (childNode->modelPayload).localTranslationZQ12) {
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_READY;
      (childNode->modelPayload).localTranslationZQ12 = translationLimitQ12;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    break;
  case ARMY_PAD_HANGAR_LOWERING:
    translationStep = linkedChildDefinition->linkedChildTranslationStepQ12PerTick;
    /* (0, 0, 0) when the model has no such point; the result is not checked */
    ModelLookupTable_GetPackedPointPosition
              (0,1,(modelNodeRuntime->modelPayload).modelResource,&loweredChildPosition);
    childNode = modelNodeRuntime->childNodes[0];
    (childNode->modelPayload).localTranslationZQ12 =
         (childNode->modelPayload).localTranslationZQ12 - translationStep * g_InGameSimulationStepTicks;
    if ((childNode->modelPayload).localTranslationZQ12 < (int)loweredChildPosition.z) {
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_CLOSING;
      (childNode->modelPayload).localTranslationZQ12 = loweredChildPosition.z;
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    if (modelRuntime->linkedChildTransitionState == ARMY_PAD_HANGAR_CLOSING) {
      ArmyPadHangar_PlaySound(worldRuntime,linkedChildDefinition,modelNodeRuntime,
                              linkedChildDefinition->linkedChildTransitionSoundAssetIndex);
    }
    break;
  case ARMY_PAD_HANGAR_CLOSING:
    modelNodeRuntime->primaryTextureOffsetV =
         modelNodeRuntime->primaryTextureOffsetV -
         linkedChildDefinition->linkedChildTextureVStepPerTick * g_InGameSimulationStepTicks;
    if (modelNodeRuntime->primaryTextureOffsetV < 1) {
      modelNodeRuntime->primaryTextureOffsetV = 0;
      modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_CLOSED;
    }
    break;
  case ARMY_PAD_HANGAR_CLOSED:
    modelRuntime->linkedChildTransitionState = ARMY_PAD_HANGAR_IDLE;
    /* fall through: the idle hangar launches the next pending aircraft */
  case ARMY_PAD_HANGAR_IDLE:
    if (((modelRuntime->linkedChildRuntimeFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) &&
       !ArmyRuntime_TestWorldPointAllowedDefault
                  (linkedChildDefinition->visibilityRadius,(modelNodeRuntime->worldTransform).translation.y,
                   (modelNodeRuntime->worldTransform).translation.x)) {
      /* the first pending slot whose aircraft can be created opens the hangar */
      if (((modelRuntime->linkedChildPendingSpawnCounts).slot0 != 0) &&
         ArmyPadHangar_TryLaunchPendingAircraft
                   (worldRuntime,modelRuntime,&(modelRuntime->linkedChildPendingSpawnCounts).slot0,
                    &modelRuntime->linkedChildSpawnInheritedState[0],g_InGamePointerModePreviewArmyIds[1],
                    linkedChildDefinition,modelNodeRuntime)) {
        break;
      }
      if (((modelRuntime->linkedChildPendingSpawnCounts).slot1 != 0) &&
         ArmyPadHangar_TryLaunchPendingAircraft
                   (worldRuntime,modelRuntime,&(modelRuntime->linkedChildPendingSpawnCounts).slot1,
                    &modelRuntime->linkedChildSpawnInheritedState[1],g_InGamePointerModePreviewArmyIds[2],
                    linkedChildDefinition,modelNodeRuntime)) {
        break;
      }
      if ((modelRuntime->linkedChildPendingSpawnCounts).slot2 != 0) {
        ArmyPadHangar_TryLaunchPendingAircraft
                  (worldRuntime,modelRuntime,&(modelRuntime->linkedChildPendingSpawnCounts).slot2,
                   &modelRuntime->linkedChildSpawnInheritedState[2],g_InGamePointerModePreviewArmyIds[4],
                   linkedChildDefinition,modelNodeRuntime);
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Takes the first queued secondary asset whose flags match the factory definition's buildable mask
   (classParameterC4) and which the faction can pay for: the Xenite is paid, the build interval and the asset's
   Energy load (held while building) are stored in the factory, and the factory starts building. */
static void ArmyUnitFactory_StartBuildingFirstAffordableAsset(ModelRuntimeUpdateView *modelRuntime,
          FactionRuntimeIndex factionIndex)
{
  FactionArmyAssetCount remainingAssetCount;
  uint32_t *queueEntry;
  ArmyAssetRecord *candidateAsset;
  uint32_t xeniteCostQ4;
  uint32_t buildTicks;
  uint32_t energyLoadQ4;
  PckArmyAssetIdCatalog selectedAssetId;

  remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount;
  queueEntry = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
  for (; remainingAssetCount != 0; remainingAssetCount = remainingAssetCount - 1, queueEntry = queueEntry + 1) {
    candidateAsset = (ArmyAssetRecord *)*queueEntry;
    if ((candidateAsset->flags & modelRuntime->modelDefinition->classParameterC4) == 0) {
      continue;
    }
    xeniteCostQ4 = candidateAsset->xeniteCostQ4;
    if (g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 < xeniteCostQ4) {
      continue;
    }
    g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
         g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - xeniteCostQ4;
    buildTicks = candidateAsset->buildTicks;
    energyLoadQ4 = candidateAsset->energyLoadQ4;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
      buildTicks = (buildTicks >> 4) + 1;
    }
    selectedAssetId = candidateAsset->registryId;
    (modelRuntime->classLinkState).classState68 = buildTicks;
    (modelRuntime->classLinkState).classState74 = energyLoadQ4;
    (modelRuntime->classLinkState).modelLinkOrState.classState = (uint32_t)selectedAssetId;
    (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 + energyLoadQ4;
    (modelRuntime->classLinkState).classState64 = 0;
    g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount =
         g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount - 1;
    /* remove the entry: shift the rest of the queue down by one */
    do {
      *queueEntry = queueEntry[1];
      queueEntry = queueEntry + 1;
      remainingAssetCount = remainingAssetCount - 1;
    } while (remainingAssetCount != 0);
    (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_BUILDING;
    (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags | ARMY_MODEL_STATE_PRODUCING;
    return;
  }
}

/* Plays the factory definition's one-shot sound at the factory, unless the factory's terrain cell has mask bits
   0/1 set. */
static void ArmyUnitFactory_PlayPrimarySound(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime,
          ModelDefinition *factoryDefinition)
{
  uint32_t soundIndex;
  ModelRuntimeNode *rootNode;
  DirectSoundVoiceSet **soundVoiceSet;
  GraphicsFixedVec3 *translationVec;

  soundIndex = factoryDefinition->primarySoundIndex;
  if ((soundIndex == 0) || (worldRuntime->dwordArrayCount <= soundIndex) || (worldRuntime->dwordArray == NULL)) {
    return;
  }
  rootNode = modelRuntime->rootModelNode;
  /* Original quirk: the voice set is read from rootNode + index * 4, not from
     worldRuntime->dwordArray, which is only tested for NULL. */
  soundVoiceSet = *(DirectSoundVoiceSet ***)((uint8_t *)rootNode + soundIndex * 4);
  translationVec = &(rootNode->worldTransform).translation;
  if (soundVoiceSet == NULL) {
    return;
  }
  if (!TerrainGrid_TestProjectedCellMaskBits01((rootNode->worldTransform).translation.y,translationVec->x,
                                               worldRuntime)) {
    SpatialSound_PlayPositionedOneShot
              (factoryDefinition->positionedSoundMaximumDistanceQ12,factoryDefinition->positionedSoundGainQ15,
               translationVec,soundVoiceSet);
  }
}

/* The build is done: creates the army at the spawn point (lookup key 0/5), heading towards the exit point (lookup
   key 1/5), links it to the factory, releases the held Energy load, starts opening the door and, for the active
   faction, queues the "army created" notification. */
static void ArmyUnitFactory_CreateBuiltArmy(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime,
          ModelRuntimeNode *rootNode)
{
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint exitPoint;
  ModelWorldPoint spawnPoint;
  uint32_t exitXQ12;
  uint32_t exitYQ12;
  uint32_t spawnHeading;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ArmyRuntimeSlot *createdArmyRuntime;
  ModelRuntimeSlot *createdModelRuntime;
  ModelRuntimeNode *createdNode;
  ModelDefinition *createdDefinition;
  ModelDefinition *factoryDefinition;
  FactionRuntimeIndex createdFactionIndex;
  uint32_t heldEnergyLoadQ4;
  uint32_t viewPitchAngle;
  int nodeHeading;
  InGameNotificationMovieId notificationMovieId;
  ArmyAssetRecordPrefix *unusedArmyAsset;

  if (!ModelLookupTable_FindPackedPoint(1,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
    return;
  }
  exitPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
  exitYQ12 = exitPoint.yQ12;
  exitXQ12 = exitPoint.xQ12;
  if (!ModelLookupTable_FindPackedPoint(0,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
    return;
  }
  spawnPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
  spawnHeading = FixedMath_Atan2Angle16(exitYQ12 - spawnPoint.yQ12,exitXQ12 - spawnPoint.xQ12);
  ownerArmyRuntime = modelRuntime->ownerArmyRuntime;
  createdArmyRuntime = ArmyRuntime_CreateInstanceFromAsset
                     (4,spawnHeading,spawnPoint.yQ12,spawnPoint.xQ12,ownerArmyRuntime->factionIndex,
                      (modelRuntime->classLinkState).modelLinkOrState.classState,worldRuntime,NULL);
  if (createdArmyRuntime == NULL) {
    return;
  }
  g_GameFactionRuntimeImage.records[ownerArmyRuntime->factionIndex].relationCounterA =
       g_GameFactionRuntimeImage.records[ownerArmyRuntime->factionIndex].relationCounterA + 1;
  factoryDefinition = modelRuntime->modelDefinition;
  createdArmyRuntime->movementStateFlags = createdArmyRuntime->movementStateFlags |
                                           (ARMY_MOVEMENT_MIRROR_TARGET | ARMY_MOVEMENT_LOCKED);
  ArmyUnitFactory_PlayPrimarySound(worldRuntime,modelRuntime,factoryDefinition);
  heldEnergyLoadQ4 = (modelRuntime->classLinkState).classState74;
  (modelRuntime->classLinkState).armyLinkOrState.armyRuntime = createdArmyRuntime;
  createdFactionIndex = createdArmyRuntime->factionIndex;
  (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_OPENING;
  (modelRuntime->classLinkState).classState74 = 0;
  (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 - heldEnergyLoadQ4;
  createdModelRuntime = createdArmyRuntime->modelRuntimeOrSavedOffset.modelRuntime;
  createdArmyRuntime->movementStateFlags = createdArmyRuntime->movementStateFlags | ARMY_MOVEMENT_LOCKED;
  /* the new army links back to this factory until it has left (ARMY_FACTORY_STATE_WAITING_EXIT) */
  createdModelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime = (ModelRuntimeSlot *)modelRuntime;
  if (createdFactionIndex != worldRuntime->activeFactionRuntimeIndex) {
    return;
  }
  /* result unused (the lookup only records an unknown id in g_PackageLastErrorPath) */
  ArmyAssetRegistry_FindById((modelRuntime->classLinkState).modelLinkOrState.classState,&unusedArmyAsset);
  viewPitchAngle = (worldRuntime->motion).pitchAngle;
  createdNode = createdModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  createdDefinition = createdModelRuntime->definitionOrSavedId.runtimeDefinition;
  nodeHeading = createdNode->modelPayload.worldRotationAngle2;
  createdDefinition->builtCount = createdDefinition->builtCount + 1;
  notificationMovieId = createdDefinition->firstBuiltNotificationMovieId;
  if (createdDefinition->builtCount != 1) {
    notificationMovieId = createdDefinition->nextBuiltNotificationMovieId;
  }
  InGameNotificationQueue_InsertPriorityRecord
            (ARMY_CREATED,0,viewPitchAngle,
             nodeHeading + ARMY_FACTORY_NOTIFICATION_HEADING_OFFSET_ANGLE16 & FIXED_ANGLE16_MASK,
             createdNode->worldTransform.translation.y,createdNode->worldTransform.translation.x,2,
             notificationMovieId);
}

/* Runtime update of the unit factory class (13), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[13]. On the first update it stores the
   factory's exit point (model lookup key 1/5). It builds one queued secondary army asset whose flags match the
   definition's mask (Xenite paid up front, Energy load held while building), creates the army at the spawn
   point, opens the door, sends the army out to the exit point, waits until it has left and closes the door.
*/
void ArmyRuntimeClass_UpdateUnitFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelRuntimeNode *rootNode;
  ModelDefinition *factoryDefinition;
  ModelRuntimeSlot *linkedModelRuntime;
  uint32_t behaviorState;
  uint32_t elapsedTicks;
  ModelPackedPointRecord *packedPoint;
  ModelWorldPoint localPoint;
  ArmyRuntimeSlot *linkedArmyRuntime;

  rootNode = modelRuntime->rootModelNode;
  if (((modelRuntime->classState).classStateBC & 1) != 0) {
    if (ModelLookupTable_FindPackedPoint(1,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
      (modelRuntime->classLinkState).classState78 = localPoint.xQ12;
      (modelRuntime->classLinkState).classState7C = localPoint.yQ12;
      (modelRuntime->classState).classStateBC = (modelRuntime->classState).classStateBC & ~1u;
    }
  }
  behaviorState = (modelRuntime->classState).behaviorState;
  factoryDefinition = modelRuntime->modelDefinition;
  if ((3 < rootNode->childCount) && (rootNode->childNodes[3] != NULL)) {
    WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)rootNode->childNodes[3]);
    rootNode->childNodes[3] = NULL;
  }
  switch(behaviorState) {
  case ARMY_FACTORY_STATE_IDLE: /* start the first affordable queued asset this factory can build */
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) {
        ArmyUnitFactory_StartBuildingFirstAffordableAsset(modelRuntime,modelRuntime->ownerArmyRuntime->factionIndex);
      }
    }
    else if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case ARMY_FACTORY_STATE_BUILDING: /* when done create the army at the spawn point (lookup keys 1/5 and 0/5 give
                                       its heading) */
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      (modelRuntime->classLinkState).classState64 =
           (modelRuntime->classLinkState).classState64 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      elapsedTicks = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= elapsedTicks) {
        ArmyUnitFactory_CreateBuiltArmy(worldRuntime,modelRuntime,rootNode);
      }
    }
    break;
  case ARMY_FACTORY_STATE_OPENING: /* then send the new army out to the point in classState78/7C */
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV +
         factoryDefinition->movementSpeed * g_InGameSimulationStepTicks;
    if (ARMY_DOOR_TEXTURE_OPEN_V - 1 < rootNode->primaryTextureOffsetV) {
      rootNode->primaryTextureOffsetV = ARMY_DOOR_TEXTURE_OPEN_V;
      (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_WAITING_EXIT;
      if (ModelLookupTable_FindPackedPoint(1,5,(rootNode->modelPayload).modelResource,&packedPoint)) {
        linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState.armyRuntime;
        localPoint = ModelNodeRuntime_TransformLocalPoint(packedPoint,rootNode);
        linkedModelRuntime = (linkedArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
        ArmyRuntime_StartMoveCommandWithAuxiliaryValues
                  ((modelRuntime->classLinkState).classState7C,
                   (modelRuntime->classLinkState).classState78,localPoint.yQ12,localPoint.xQ12,
                   (ArmyMovementRuntime *)linkedArmyRuntime);
        (linkedModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime =
             (ModelRuntimeSlot *)modelRuntime;
      }
    }
    break;
  case ARMY_FACTORY_STATE_WAITING_EXIT:
    linkedArmyRuntime = (modelRuntime->classLinkState).armyLinkOrState.armyRuntime;
    if ((linkedArmyRuntime == NULL) ||
       ((ModelRuntimeUpdateView *)
        (((linkedArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).
        linkedArmyRuntimeOrSavedOffset.modelRuntime != modelRuntime)) {
      factoryDefinition = modelRuntime->modelDefinition;
      (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_CLOSING;
      (modelRuntime->classLinkState).armyLinkOrState.armyRuntime = NULL;
      ArmyUnitFactory_PlayPrimarySound(worldRuntime,modelRuntime,factoryDefinition);
    }
    break;
  case ARMY_FACTORY_STATE_CLOSING:
    rootNode->primaryTextureOffsetV =
         rootNode->primaryTextureOffsetV -
         factoryDefinition->movementSpeed * g_InGameSimulationStepTicks;
    if (rootNode->primaryTextureOffsetV < 1) {
      rootNode->primaryTextureOffsetV = 0;
      (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_IDLE;
      (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags & ~ARMY_MODEL_STATE_PRODUCING;
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Runtime update of production class 11, reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[11]. Builds one queued secondary army asset
   with flag 0x10 at a time: its Xenite cost (xeniteCostQ4) is paid once up front, its Energy load
   (energyLoadQ4) is held on the building while it is built. The finished asset is appended to the faction's
   primary asset list (at most 64 entries, from where it is placed) and, for the active faction, the command grid
   is rebuilt and a notification is queued.
*/
void ArmyRuntimeClass_UpdateStructureFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  uint32_t assetEnergyValue;
  ModelRuntimeSlotLinkOrState selectedAssetLink;
  int factionIndex;
  ArmyAssetRecord *candidateAsset;
  int activeFactionIndex;
  AngleTurn32 headingAngle;
  ModelDefinition *linkedModelDefinition;
  FactionArmyAssetCount remainingAssetCount;
  uint32_t buildTicks;
  uint32_t elapsedTicks;
  uint32_t heldEnergyLoad;
  uint32_t primaryAssetCount;
  uint32_t linkedRootNodeOffset;
  uint32_t pitchAngle;
  uint32_t *queueSlot;
  uint32_t lookupError;
  ArmyAssetRecordPrefix *assetRecord;
  InGameNotificationMovieId notificationMovieId;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ModelRuntimeNode *rootNode;

  switch((modelRuntime->classState).behaviorState) {
  case ARMY_FACTORY_STATE_IDLE: /* start the first affordable queued asset with flag 0x10 */
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) == 0) {
      if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_BUILD_BLOCKING_MASK) == 0) {
        factionIndex = modelRuntime->ownerArmyRuntime->factionIndex;
        queueSlot = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetPointersOrIds;
        for (remainingAssetCount = g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount; remainingAssetCount != 0;
            remainingAssetCount = remainingAssetCount - 1) {
          candidateAsset = (ArmyAssetRecord *)*queueSlot;
          if (((candidateAsset->flags & ARMY_ASSET_FLAG_BUILT_BY_CLASS11) != 0) &&
             (candidateAsset->xeniteCostQ4 <= g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4))
          {
            g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
                 g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 - candidateAsset->xeniteCostQ4;
            buildTicks = candidateAsset->buildTicks;
            assetEnergyValue = candidateAsset->energyLoadQ4;
            if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
              buildTicks = (buildTicks >> 4) + 1;
            }
            selectedAssetLink = *(ModelRuntimeSlotLinkOrState *)&candidateAsset->registryId;
            (modelRuntime->classLinkState).classState68 = buildTicks;
            (modelRuntime->classLinkState).classState74 = assetEnergyValue;
            (modelRuntime->classLinkState).modelLinkOrState = selectedAssetLink;
            (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 + assetEnergyValue;
            (modelRuntime->classLinkState).classState64 = 0;
            g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount =
                 g_GameFactionRuntimeImage.records[factionIndex].secondaryArmyAssetCount - 1;
            /* Remove the entry from the queue. Original quirk: it shifts remainingAssetCount entries, i.e. it
               also copies the slot just behind the last queued entry. */
            do {
              *queueSlot = queueSlot[1];
              queueSlot = queueSlot + 1;
              remainingAssetCount = remainingAssetCount - 1;
            } while (remainingAssetCount != 0);
            (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_BUILDING;
            (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags | ARMY_MODEL_STATE_PRODUCING;
            break;
          }
          queueSlot = queueSlot + 1;
        }
      }
    }
    else if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
    }
    break;
  case ARMY_FACTORY_STATE_BUILDING:
    if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
      ownerArmyRuntime = modelRuntime->ownerArmyRuntime;
      (modelRuntime->classLinkState).classState64 =
           (modelRuntime->classLinkState).classState64 + g_InGameSimulationStepTicks;
      ArmyRuntime_UpdateTimedShotAndEffectEmitters(worldRuntime,modelRuntime);
      elapsedTicks = (modelRuntime->classLinkState).classState64;
      ArmyRuntime_UpdateAnimatedModelSubnodes(worldRuntime,modelRuntime);
      if ((modelRuntime->classLinkState).classState68 <= elapsedTicks) {
        factionIndex = ownerArmyRuntime->factionIndex;
        heldEnergyLoad = (modelRuntime->classLinkState).classState74;
        (modelRuntime->classLinkState).classState74 = 0;
        (modelRuntime->classState).behaviorState = ARMY_FACTORY_STATE_IDLE;
        (modelRuntime->classState).stateFlags = (modelRuntime->classState).stateFlags & ~ARMY_MODEL_STATE_PRODUCING;
        (modelRuntime->classState).energyLoadQ4 = (modelRuntime->classState).energyLoadQ4 - heldEnergyLoad;
        lookupError = ArmyAssetRegistry_FindById
                           ((modelRuntime->classLinkState).modelLinkOrState.classState,&assetRecord);
        (modelRuntime->classLinkState).modelLinkOrState.modelRuntime = NULL;
        if (lookupError == 0) {
          primaryAssetCount = g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount;
          if (primaryAssetCount < 64) {
            activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
            /* appended to the faction's primary asset list */
            g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[primaryAssetCount] =
                 (uint32_t)assetRecord;
            g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount =
                 g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount + 1;
            if (activeFactionIndex == ownerArmyRuntime->factionIndex) {
              linkedRootNodeOffset = assetRecord->rootNodeOffsetOrPointer;
              InGameArmyStock_RebuildGrid((UiNodeBase *)worldRuntime);
              linkedModelDefinition = (ModelDefinition *)ModelDefinition_SelectFactionUnlockedLinkedDefinition
                                 (ownerArmyRuntime->factionIndex,linkedRootNodeOffset);
              rootNode = modelRuntime->rootModelNode;
              pitchAngle = (worldRuntime->motion).pitchAngle;
              headingAngle = (rootNode->modelPayload).worldRotationAngle2;
              linkedModelDefinition->builtCount = linkedModelDefinition->builtCount + 1;
              notificationMovieId = linkedModelDefinition->firstBuiltNotificationMovieId;
              if (linkedModelDefinition->builtCount != 1) {
                notificationMovieId = linkedModelDefinition->nextBuiltNotificationMovieId;
              }
              InGameNotificationQueue_InsertPriorityRecord
                        (ARMY_CREATED,0,pitchAngle,headingAngle + ARMY_PRODUCTION_NOTIFICATION_HEADING_OFFSET_ANGLE16 & FIXED_ANGLE16_MASK,
                         (rootNode->worldTransform).translation.y,
                         (rootNode->worldTransform).translation.x,3,notificationMovieId);
            }
          }
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Runtime update of the resource extractor class (14), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[14]. While it has health left and is active,
   it stamps its faction and resource-field selector into the grid cell under it (inner cells only) and, when
   the cell carries the matching resource-field support bit, registers itself there and runs its emitters and
   animation.
*/
void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView *modelRuntime)

{
  int cellColumn;
  int cellRow;
  int cellIndex;
  FieldCellPackedFlagsAndMaterial supportFlagMask;
  FieldGridCoordinates gridCoordinates;
  FieldGridAsset *fieldGrid;

  if ((1 < (int)modelRuntime->health) &&
     (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0)) {
    gridCoordinates = FieldGrid_WorldToGridQ12
                      ((modelRuntime->rootModelNode->worldTransform).translation.y,
                       (modelRuntime->rootModelNode->worldTransform).translation.x);
    /* grid coordinates rounded to the nearest cell */
    cellColumn = ((gridCoordinates.columnQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
    cellRow = ((gridCoordinates.rowQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
    fieldGrid = worldRuntime->fieldGrid;
    if ((0 < cellColumn) && (0 < cellRow)) {
      if ((cellColumn + 1 < (int)fieldGrid->gridWidth) && (cellRow + 1 < (int)fieldGrid->gridHeight)) {
        cellIndex = cellRow * fieldGrid->gridWidth + cellColumn;
        supportFlagMask = FIELD_CELL_XENITE_SUPPORT << ((uint8_t)modelRuntime->modelDefinition->resourceFieldSupportSelector & 31
                         );
        /* claim the cell: faction << 13, the support bit, claimedCellTag << 24 */
        fieldGrid->cells[cellIndex].resourceExtractionDescriptor =
             modelRuntime->ownerArmyRuntime->factionIndex << 13 | supportFlagMask |
             modelRuntime->modelDefinition->claimedCellTag << 24;
        if ((fieldGrid->cells[cellIndex].flagsAndMaterial & supportFlagMask) != 0) {
          /* the cell supports this extractor: register it (as a saved offset) and run its emitters */
          fieldGrid->cells[cellIndex].armyRuntimeSavedOffset =
               (int)modelRuntime - g_ModelRuntimeRebaseDelta;
          ArmyRuntime_UpdateTimedShotAndEffectEmitters
                    (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
          ArmyRuntime_UpdateAnimatedModelSubnodes
                    (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
        }
      }
    }
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Class command of the structure classes (the ten non-default class slots that share it; session conditions
   use it to tell structures from units). It hits every class-0/class-12 model standing inside the structure's
   footprint with 0x100000 impact damage, stamps the structure's ground height into the field grid (unless
   class-state bit 0x20 is set), and when every model of the same owner within reach is an idle class-18
   model, marks the last of them (flag 8) and spawns its army-from-model completion effect.
*/
void ArmyRuntime_ClassCommandHandlerGroupA(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *ownNode;
  ModelRuntimeNode *armyModelNode;
  ModelRuntimeNode *completionNode;
  ModelRuntimeSlot *candidateModelRuntime;
  ModelRuntimeSlot *completionModelRuntime;
  int64_t deltaYSquared;
  int64_t remainingRadiusSquared;
  uint32_t impactAngle;
  uint32_t candidateSupportRadius;
  int proximityRadius;
  int deltaX;
  int deltaY;
  ModelRuntimeNode *scanNode;
  EffectDefinition *completionEffect;
  int blockingCount;

  /* crush every class-0/class-12 model standing inside the structure */
  ownNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
    for (scanNode = (ModelRuntimeNode *)worldRuntime->ownerListHead; scanNode != NULL;
        scanNode = (ModelRuntimeNode *)(scanNode->common).nextNode) {
      if (scanNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      candidateModelRuntime = (scanNode->runtimePayload).modelRuntime;
      if ((ownNode != scanNode) &&
         ((candidateModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_00 ||
          (candidateModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_12))))
      {
        /* Damage the candidate unless the class-13 test misses and the attachment proximity test hits. */
        if (ArmyRuntime_TestArmyNearFactoryExit(candidateModelRuntime,modelRuntime) ||
            !ArmyRuntime_TestModelAttachmentProximity(candidateModelRuntime,modelRuntime)) {
          impactAngle = FixedMath_Atan2Angle16
                            ((scanNode->worldTransform).translation.y -
                             (ownNode->worldTransform).translation.y,
                             (scanNode->worldTransform).translation.x -
                             (ownNode->worldTransform).translation.x);
          ArmyRuntime_ApplyImpactDamageAndFinalizeState(impactAngle,ARMY_CRUSH_IMPACT_DAMAGE,candidateModelRuntime);
        }
      }
    }
  }
  /* stamp the structure's ground height (root node read again after the damage calls, as in the original) */
  ownNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if ((modelRuntime->definitionOrSavedId.runtimeDefinition->modelFlags & MODEL_DEFINITION_FLAG_DRAW_BEFORE_TERRAIN) == 0) {
    FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
              (modelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius,
               (ownNode->worldTransform).translation.z,(ownNode->worldTransform).translation.y
               ,(ownNode->worldTransform).translation.x,worldRuntime->fieldGrid);
  }
  /* count the models of the same faction within reach of the owning army's model node; idle class-18 models
     do not block, the last of them is the one to complete */
  armyModelNode = modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->modelNodeRuntime;
  completionModelRuntime = NULL;
  blockingCount = 0;
  for (scanNode = (ModelRuntimeNode *)worldRuntime->ownerListHead; scanNode != NULL;
      scanNode = (ModelRuntimeNode *)(scanNode->common).nextNode) {
    if ((scanNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) || (scanNode == armyModelNode)) {
      continue;
    }
    candidateModelRuntime = (scanNode->runtimePayload).modelRuntime;
    candidateSupportRadius = candidateModelRuntime->definitionOrSavedId.runtimeDefinition->supportRadius;
    if ((modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex !=
         candidateModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) ||
        (candidateSupportRadius == 0)) {
      continue;
    }
    /* reach = own definition placementFlags + candidate supportRadius; inside when reach^2 - dx^2 - dy^2 >= 0 in
       64 bits */
    proximityRadius =
         modelRuntime->definitionOrSavedId.runtimeDefinition->placementFlags + candidateSupportRadius;
    deltaX = (armyModelNode->worldTransform).translation.x - (scanNode->worldTransform).translation.x;
    remainingRadiusSquared =
         (int64_t)proximityRadius * (int64_t)proximityRadius - (int64_t)deltaX * (int64_t)deltaX;
    if (remainingRadiusSquared < 0) {
      continue;
    }
    deltaY = (armyModelNode->worldTransform).translation.y - (scanNode->worldTransform).translation.y;
    deltaYSquared = (int64_t)deltaY * (int64_t)deltaY;
    if (remainingRadiusSquared - deltaYSquared < 0) {
      continue;
    }
    if ((candidateModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_18) &&
       (((candidateModelRuntime->classState).stateFlags &
         (ARMY_RUNTIME_FLAG_DESTROYED | ARMY_MODEL_STATE_DISMANTLING)) == 0)) {
      completionModelRuntime = candidateModelRuntime;
    }
    else {
      blockingCount = blockingCount + 1;
    }
  }
  if ((blockingCount == 0) && (completionModelRuntime != NULL)) {
    completionNode = completionModelRuntime->rootModelNodeOrSavedOffset.modelNode;
    (completionModelRuntime->classState).stateFlags =
         (completionModelRuntime->classState).stateFlags | ARMY_RUNTIME_FLAG_DESTROYED;
    if (EffectDefinitionRegistry_FindById
                      ((PckEffectDefinitionIdCatalog)
                       completionModelRuntime->definitionOrSavedId.runtimeDefinition->classParameterC4,
                       &completionEffect) == 0) {
      EffectRuntimePool_CreateInstanceFromDefinition
                (EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL,
                 THANDOR_COMPOUND(EffectRuntimeOwnerReference){
                   .armyRuntime = completionModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime },
                 (completionNode->modelPayload).worldRotationAngle2,
                 (completionNode->modelPayload).worldRotationAngle1,
                 (completionNode->modelPayload).worldRotationAngle0,
                 (completionNode->worldTransform).translation.z,
                 (completionNode->worldTransform).translation.y,
                 (completionNode->worldTransform).translation.x,completionEffect,worldRuntime);
    }
  }
}


/* Army entry of the terrainStateRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that
   table): re-registers the owning army's terrain occupancy flags and refreshes the state tint of the model.
*/
void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime)

{
  ArmyRuntime_InitializeTerrainOccupancyFlags
            (worldRuntime,
             (ArmyRuntimeSlot *)
             ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime);
  ModelNodeRuntime_UpdateStateTintRecursive(modelNodeRuntime);
  return;
}


/* Army entry of the audioRefresh phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table):
   runs the class sound callbacks (classMethodD) over the model hierarchy of the owner-list node, starting at its
   model runtime (runtimePayload).
*/
void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode)

{
  ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(worldRuntime,(ModelRuntimeSlot *)ownerNode->runtimePayload);
  return;
}


/* Army entry of the primaryUpdate phase of g_RuntimeMaintenanceCallbackPhases (only reached through that table,
   once per simulation step and owner-list node): updates the army's model hierarchy, lets the AI pick targets
   for non-neutral factions, drops a timed-out target command, clears the LOCKED movement flag once nothing
   links to the model any more and counts down movementRetryCountdown.
*/
void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode)

{
  ArmyCommandGeneration *commandGenerationField;
  ModelRuntimeSlot *modelRuntime;
  ArmyRuntimeSlot *armyRuntime;
  int ownerFactionIndex;
  
  modelRuntime = (ModelRuntimeSlot *)ownerNode->runtimePayload;
  armyRuntime = (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  ownerFactionIndex = armyRuntime->factionIndex;
  ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,modelRuntime);
  if (ownerFactionIndex != 0) {
    AiCombatDecision_UpdateTargetAssignment(worldRuntime,armyRuntime);
  }
  commandGenerationField = &armyRuntime->commandGeneration;
  *commandGenerationField = *commandGenerationField - g_InGameSimulationStepTicks;
  if ((int)*commandGenerationField < 0) {
    armyRuntime->commandModeFlags =
         armyRuntime->commandModeFlags & ~(ARMY_COMMAND_MODE_INTERRUPTED | ARMY_COMMAND_MODE_AI_COMBAT_TARGET);
  }
  if (((armyRuntime->movementStateFlags & ARMY_MOVEMENT_LOCKED) != 0) &&
     ((modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime == NULL)) {
    armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & ~ARMY_MOVEMENT_LOCKED;
  }
  if (armyRuntime->movementRetryCountdown != 0) {
    armyRuntime->movementRetryCountdown = armyRuntime->movementRetryCountdown - 1;
  }
  return;
}


/* Level start: allocates and zeroes the 0x48000-byte army runtime pool, loads the army graphics (texture set and
   palette, "<graphicsBasePath><suffix>.gfx/.pal") of slot 0 and of every existing faction, and renders the two
   panel preview textures of every army asset that has a selection panel entry. The movie schedule is ticked in between, since this
   runs behind the level-loading movie. Returns true on success (*outError = 0); on an allocation or graphics
   load error returns false with that error in *outError. A failed preview render is skipped silently.
*/
Bool8 ArmyRuntime_InitializePoolAndGraphics(void *ownerContext,uint16_t *graphicsBasePath,uint32_t *outError)

{
  uint16_t pathChar;
  uint32_t factionGraphicsVariant;
  ArmyAssetRecordPrefix *armyAsset;
  ArmyRuntimeSlot *armyPool;
  uint32_t *poolDwordCursor;
  GraphicsPaletteAsset *paletteAsset;
  GraphicsTextureSourceAsset *textureSourceAsset;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPixelDimension previewHeight;
  int remainingCount;
  int factionSuffixChar;
  Bool8 loadFactionGraphics;
  int frontendPlayerRuntimeId;
  ArmyAssetRecordPrefix **registryCursor;
  uint16_t *pathCursor;
  uint16_t *pathEnd;
  uint32_t allocError;
  GraphicsTextureResource *previewTexture;

  allocError = g_MemoryApi.alloc(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),(void **)&armyPool);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  /* base - 1: the rebase value for saved offsets, see ArmyRuntimePool_RebaseAfterLoad */
  g_ArmyRuntimeRebaseBaseMinusOne = (uint8_t *)armyPool - 1;
  g_ArmyRuntimeSlots = armyPool;
  poolDwordCursor = (uint32_t *)armyPool;
  for (remainingCount = ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot) / 4; remainingCount != 0; remainingCount--) {
    *poolDwordCursor = 0;
    poolDwordCursor++;
  }
  /* find the end of graphicsBasePath (at most 32 code units); pathEnd ends up on the terminator (or on the
     last of the 32 code units) and the suffix digit is written there */
  pathCursor = graphicsBasePath;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    pathChar = *pathCursor;
    pathCursor++;
    if (pathChar == 0) {
      break;
    }
  }
  pathEnd = pathCursor - 1;
  g_MoviePlaybackScheduleSpan = 26;
  for (frontendPlayerRuntimeId = 0; frontendPlayerRuntimeId < ARMY_GRAPHICS_BINDING_COUNT;
       frontendPlayerRuntimeId++) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    factionSuffixChar = '0';
    /* Slot 0 always loads the "0" graphics; the other slots load theirs (suffix 0-9/A-Z from the faction's
       graphics variant) only while the faction exists. */
    loadFactionGraphics = frontendPlayerRuntimeId == 0;
    if (!loadFactionGraphics) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frontendPlayerRuntimeId] != 0) {
        /* the faction's colour index selects its graphics */
        factionGraphicsVariant = g_GameFactionRuntimeImage.records[frontendPlayerRuntimeId].colorIndex;
        g_MoviePlaybackScheduleCounter--;
        if (factionGraphicsVariant < 10) {
          factionSuffixChar = factionGraphicsVariant + '0';
        }
        else {
          factionSuffixChar = factionGraphicsVariant + ('A' - 10);
        }
        loadFactionGraphics = true;
      }
    }
    if (loadFactionGraphics) {
      *(int *)pathEnd = factionSuffixChar; /* the suffix and a terminator in one dword */
      WidePath_SetExtensionCode(ASSET_MAGIC_GFX,graphicsBasePath);
      textureSourceAsset = (GraphicsTextureSourceAsset *)Package_LoadEntry(graphicsBasePath,outError);
      if (textureSourceAsset == NULL) {
        return false;
      }
      ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
                (frontendPlayerRuntimeId,(ArmyGraphicsAssetAddress32)textureSourceAsset);
      loadedTextureSet = g_GraphicsCreateTextureSet(textureSourceAsset,outError);
      if (loadedTextureSet == NULL) {
        LOCK();
        UNLOCK();
        Resource_Release(textureSourceAsset);
        return false;
      }
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ArmyGraphicsBindings[frontendPlayerRuntimeId].textureSet = loadedTextureSet;
      WidePath_SetExtensionCode(ASSET_MAGIC_PAL,graphicsBasePath);
      paletteAsset = g_GraphicsPaletteAssetLoadPackage(graphicsBasePath,outError);
      if (paletteAsset == NULL) {
        return false;
      }
      g_ArmyGraphicsBindings[frontendPlayerRuntimeId].paletteAsset = paletteAsset;
    }
    MoviePlayback_AdvanceScheduledFrameAndTick();
  }
  pathEnd[0] = 0;
  pathEnd[1] = 0;
  /* Preview textures for army assets with a non-zero selection detail variant (bits 1-7 of
     armyAsset[1].selectionDetailTemplateVariantIndex): the panel size one (subresource 34) into
     armyAsset[1].rootNodeOffsetOrPointer, a third of the subresource-2 width into armyAsset[1].registryId. */
  registryCursor = g_ArmyAssetRecordRegistry;
  for (remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT; remainingCount != 0; remainingCount--) {
    armyAsset = *registryCursor;
    if ((armyAsset != NULL) &&
       ((armyAsset[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_PRODUCTION_MASK) != 0)) {
      previewTexture = ArmyRuntime_RenderPreviewTexture
                         (g_InGamePanelTextureSubresource34Height,
                          g_InGamePanelTextureSubresource34Width,
                          ((WorldRuntimeContext *)ownerContext)->activeFactionRuntimeIndex,armyAsset->registryId,
                          (WorldRuntimeContext *)ownerContext);
      if (previewTexture != NULL) {
        armyAsset[1].rootNodeOffsetOrPointer = (uint32_t)previewTexture;
        previewHeight =
             (GraphicsPixelDimension)
             ((uint64_t)(int64_t)g_InGamePanelTextureSubresource02Width / 3);
        previewTexture = ArmyRuntime_RenderPreviewTexture
                           (previewHeight,previewHeight,
                            ((WorldRuntimeContext *)ownerContext)->activeFactionRuntimeIndex,armyAsset->registryId,
                            (WorldRuntimeContext *)ownerContext);
        if (previewTexture != NULL) {
          armyAsset[1].registryId = (PckArmyAssetIdCatalog)previewTexture;
        }
      }
    }
    registryCursor++;
  }
  *outError = 0;
  return true;
}


/* Runtime update of class 12, reached only through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[12]:
   runs the emitters and moves the model vertically by the definition's step per tick, subtracting the step
   from the distance counter in classLinkState.modelLinkOrState; once that counter exceeds the model's own
   height (bounds Z1 - Z0) the whole model hierarchy is destroyed, e.g. a wreck that has sunk out of sight.
*/

void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy
          (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelResource *rootModelResource;
  int verticalStepQ12;
  int modelHeightQ12;
  int remainingClassDistanceQ12;

  ArmyRuntime_UpdateTimedShotAndEffectEmitters
            (worldRuntime,(ModelRuntimeUpdateView *)modelRuntime);
  modelNodeRuntime = modelRuntime->rootModelNode;
  rootModelResource = (modelNodeRuntime->modelPayload).modelResource;
  verticalStepQ12 =
       modelRuntime->modelDefinition->verticalTranslationStepQ12PerTick *
       g_InGameSimulationStepTicks;
  (modelNodeRuntime->worldTransform).translation.z += verticalStepQ12;
  /* the class link field (modelLinkOrState) holds the distance counter here */
  (modelRuntime->classLinkState).modelLinkOrState.signedScalarState -= verticalStepQ12;
  modelHeightQ12 = rootModelResource->localBoundsZ1Q12 - rootModelResource->localBoundsZ0Q12;
  remainingClassDistanceQ12 = (modelRuntime->classLinkState).modelLinkOrState.signedScalarState;
  modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 1;
  if (modelHeightQ12 < remainingClassDistanceQ12) {
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  }
  else {
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  return;
}


/* World owner-list callback: for a model node, clears its runtime flags 0x4 and 0x8, re-registers the owning
   army's terrain occupancy and refreshes the node's state tint. Second pass after
   ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback when a level's armies are set up.
*/
void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    node->runtimeFlags = node->runtimeFlags & ~(TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE | TERRAIN_OCCUPANCY_FLAG_PRESENT);
    /* the army runtime at +8 of the node's payload */
    ArmyRuntime_InitializeTerrainOccupancyFlags
              (armyContext,*(ArmyRuntimeSlot **)((int)node->runtimePayload + 8));
    ModelNodeRuntime_RefreshStateTint((ModelRuntimeNode *)node);
  }
}


/* Called by the weapon code (combat/movement) after a shot has been fired: stores the launch heading and the
   weapon definition's two post-launch values as the army's action vector, but only when both of those values
   are nonzero; otherwise the previous vector is kept.
*/
void ArmyRuntime_SetNonzeroActionVector
          (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime)

{
  if ((actionVector1 != 0) && (actionVector2 != 0)) {
    armyRuntime->actionVector1Q12 = actionVector1;
    armyRuntime->actionVector0Q12 = actionVector0;
    armyRuntime->actionVector2Q12 = actionVector2;
  }
  return;
}


/* Gives the army a new target army (NULL clears the command). A move started by target following is
   ended first; the command is stamped with the standard generation, or generation 0 when cleared.
*/
void ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration standardGeneration;

  standardGeneration = g_ArmyCommandGenerationStandard;
  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  if (targetArmyRuntime == NULL) {
    armyRuntime->commandModeFlags = 0;
    armyRuntime->commandGeneration = 0;
  }
  else {
    armyRuntime->commandModeFlags = ARMY_COMMAND_MODE_TARGET_ARMY;
    armyRuntime->commandGeneration = standardGeneration;
  }
  armyRuntime->commandTargetArmyRuntime = targetArmyRuntime;
  return;
}


/* Gives the army a target position command (commandCoordinate0-2Q12): ends a move started by target
   following, drops any target army and stamps the standard command generation.
*/
void ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinate2Q12,Q12 coordinate1Q12,Q12 coordinate0Q12,ArmyRuntimeSlot *armyRuntime)

{
  ArmyCommandGeneration commandGeneration;

  if ((armyRuntime->movementStateFlags & ARMY_MOVEMENT_TARGET_FOLLOWING) != 0) {
    if ((armyRuntime->commandModeFlags & ARMY_COMMAND_MODE_AI_COMBAT_TARGET) == 0) {
      ArmyRuntime_ResetMovementStatePreserveQueuedTarget((ArmyMovementRuntime *)armyRuntime);
    }
    else {
      ArmyRuntime_ResetMovementStateFromCurrentPosition((ArmyMovementRuntime *)armyRuntime);
    }
  }
  armyRuntime->commandModeFlags = ARMY_COMMAND_MODE_TARGET_POSITION;
  armyRuntime->commandCoordinate0Q12 = coordinate0Q12;
  armyRuntime->commandCoordinate1Q12 = coordinate1Q12;
  armyRuntime->commandCoordinate2Q12 = coordinate2Q12;
  commandGeneration = g_ArmyCommandGenerationStandard;
  armyRuntime->commandTargetArmyRuntime = NULL;
  armyRuntime->movementStateFlags = armyRuntime->movementStateFlags & ~ARMY_MOVEMENT_TARGET_FOLLOWING;
  armyRuntime->commandGeneration = commandGeneration;
  return;
}


/* Resolves the world point a shooter at sourceWorld*Q12 aims its shot at: the explicit target position of the
   command, or the target entity's model (the flying body of an aircraft) raised by its definition's aim height.
   A moving target is led along its heading by the distance it covers during the shot's flight time, unless it
   stands still within that lead range. A target entity the shooter's faction can no longer see is dropped from
   the command. Returns true with the point in *outAimPoint, or false (and *outAimPoint zeroed) when there is
   nothing to aim at.
*/
Bool8
ArmyRuntime_ResolveShotAimPoint
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState,GraphicsFixedVec3 *outAimPoint)

{
  int *targetDefinitionRecord;
  ModelDefinition *targetDefinition;
  ArmyRuntimeSlot *targetArmy;
  int64_t deltaYSquared;
  int64_t remainingRangeSquared;
  uint32_t visibilityMask;
  uint32_t targetDistance;
  uint32_t rampUpLeadTime;
  uint32_t targetHeading;
  int leadDistance;
  Q12 trackedX;
  Q12 trackedY;
  int deltaX;
  int deltaY;
  int aimWorldX;
  int aimWorldY;
  int aimWorldZ;
  ModelRuntimeNode *targetNode;
  Q12 shotLeadSpeedQ12;
  FixedDirection leadDirection;
  GameEntityRuntime *targetEntity;

  /* On failure the original leaves the aim point undefined (left-over intermediate values). All three callers
     ignore the coordinates on failure, so the point is zeroed. */
  outAimPoint->x = 0;
  outAimPoint->y = 0;
  outAimPoint->z = 0;
  if (((targetState->common).commandTarget.targetFlags & 1) == 0) {
    if (((targetState->common).commandTarget.targetFlags & 2) != 0) {
      outAimPoint->x = (targetState->common).commandTarget.targetWorldXQ12;
      outAimPoint->y = (targetState->common).commandTarget.targetWorldYQ12;
      outAimPoint->z = (targetState->common).commandTarget.targetWorldZQ12;
      return true;
    }
  }
  else {
    targetEntity = (targetState->common).commandTarget.targetEntity;
    if (targetEntity != NULL) {
      /* bit 1 of the shooter faction's 2-bit field: the target is visible to that faction */
      visibilityMask = 2u << ((uint8_t)((targetState->common).ownership.ownerIndex * 2) & 31);
      targetNode = (targetEntity->common).ownership.modelNode;
      if (((targetEntity->common).damageState.factionVisibilityBits1C & visibilityMask) != 0) {
        if (((ModelRuntimeSlot *)(targetEntity->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
            runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
          targetNode = targetNode->childNodes[0];
        }
        aimWorldX = (targetNode->worldTransform).translation.x;
        targetDefinitionRecord = (int *)(targetEntity->common).ownership.definitionOrClassRecord;
        targetDefinition = (ModelDefinition *)*targetDefinitionRecord;
        aimWorldY = (targetNode->worldTransform).translation.y;
        aimWorldZ = (targetNode->worldTransform).translation.z + targetDefinition->aimHeightOffsetQ12;
        targetArmy = (ArmyRuntimeSlot *)targetDefinitionRecord[2];
        if ((targetDefinition->accelerationPerTick != 0) && ((targetArmy->movementStateFlags & 4) == 0)) {
          targetDistance = FixedMath_Length3(aimWorldZ - sourceWorldZQ12,aimWorldY - sourceWorldYQ12,
                                             aimWorldX - sourceWorldXQ12);
          shotLeadSpeedQ12 = ShotDefinition_GetLeadSpeed(shotDefinition);
          leadDistance = (int)(((int64_t)(int)targetDistance * (int64_t)targetDefinition->movementSpeed) /
                               (int64_t)shotLeadSpeedQ12);
          rampUpLeadTime = ShotDefinition_ComputeRampUpLeadTime(shotDefinition);
          leadDistance = leadDistance + rampUpLeadTime * targetDefinition->movementSpeed;
          targetHeading = FixedMath_Atan2Angle16
                            (targetArmy->movementPosition1Q12 -
                             targetArmy->modelNodeRuntime->worldTransform.translation.y,
                             targetArmy->movementPosition0Q12 -
                             targetArmy->modelNodeRuntime->worldTransform.translation.x);
          leadDirection = FixedMath_DirectionFromAnglesScaled(0,targetHeading,leadDistance);
          targetEntity = (targetState->common).commandTarget.targetEntity;
          trackedX = (targetEntity->common).damageState.trackedCoordinate0Q12;
          if (trackedX == (targetEntity->common).pathCoordinate0Q12) {
            deltaX = trackedX - (targetNode->worldTransform).translation.x;
            remainingRangeSquared = ((int64_t)(int)leadDirection.y * (int64_t)(int)leadDirection.y +
                                     (int64_t)(int)leadDirection.x * (int64_t)(int)leadDirection.x) -
                                    (int64_t)deltaX * (int64_t)deltaX;
            if (-1 < remainingRangeSquared) {
              trackedY = (targetEntity->common).damageState.trackedCoordinate1Q12;
              if (trackedY == (targetEntity->common).pathCoordinate1Q12) {
                deltaY = trackedY - (targetNode->worldTransform).translation.y;
                deltaYSquared = (int64_t)deltaY * (int64_t)deltaY;
                if (-1 < remainingRangeSquared - deltaYSquared) {
                  /* The target is standing still within lead range: aim at its path position directly. */
                  outAimPoint->x = (targetEntity->common).pathCoordinate0Q12;
                  outAimPoint->y = (targetEntity->common).pathCoordinate1Q12;
                  outAimPoint->z =
                       (((targetEntity->common).ownership.modelNode)->worldTransform).translation.z +
                       ((ModelRuntimeSlot *)(targetEntity->common).ownership.definitionOrClassRecord)->
                       definitionOrSavedId.runtimeDefinition->aimHeightOffsetQ12;
                  return true;
                }
              }
            }
          }
          aimWorldZ = leadDirection.z + aimWorldZ;
          aimWorldY = leadDirection.y + aimWorldY;
          aimWorldX = leadDirection.x + aimWorldX;
        }
        outAimPoint->x = aimWorldX;
        outAimPoint->y = aimWorldY;
        outAimPoint->z = aimWorldZ;
        return true;
      }
      (targetState->common).commandTarget.targetEntity = NULL;
      (targetState->common).commandTarget.targetFlags = 0;
    }
  }
  return false;
}


/* World owner-list callback: for a model node of an owned army, adds the army's projected terrain occlusion
   (its depthBinClass mask in the byte of every faction whose nibble in the owner's packed relation states has bit 3
   set) around the node, and marks occupancy bit 2 around it when the active faction's nibble has bit 3 set. First pass of the occupancy rebuild; see
   ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback.
*/
void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *node)

{
  ArmyRuntimeSlot *ownerArmy;
  uint32_t relationStates;
  int activeFactionIndex;
  int factionIndex;
  uint64_t visibilityMask;
  uint64_t factionMaskByte;
  Q12 worldXQ12;
  Q12 worldYQ12;
  int ownerFactionIndex;
  FieldGridAsset *fieldGrid;

  if (node->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
    return;
  }
  /* node->worldXQ12 goes to the callees' worldYQ12 and node->worldYQ12 to their worldXQ12,
     as in the original; one of the two namings is swapped. */
  worldYQ12 = node->worldXQ12;
  ownerArmy = ((ModelRuntimeSlot *)node->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  worldXQ12 = node->worldYQ12;
  /* faction 0 = none */
  if (ownerArmy->factionIndex == 0) {
    return;
  }
  ownerFactionIndex = ownerArmy->factionIndex;
  fieldGrid = worldRuntime->fieldGrid;
  relationStates = g_GameFactionRuntimeImage.records[ownerFactionIndex].packedRelationStates;
  factionMaskByte = (uint64_t)(uint32_t)ownerArmy->depthBinClass;
  /* one nibble per faction in relationStates, one byte per faction in the 64-bit mask (faction 7 in the top
     byte); faction 0 (the lowest byte) is not tested and stays zero */
  visibilityMask = 0;
  for (factionIndex = 7; factionIndex >= 1; factionIndex--) {
    visibilityMask = visibilityMask << 8;
    if ((relationStates & FACTION_RELATION_PACKED(FACTION_RELATION_STATE_ALLIED,factionIndex)) != 0) {
      visibilityMask = visibilityMask | factionMaskByte;
    }
  }
  activeFactionIndex = worldRuntime->activeFactionRuntimeIndex;
  TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint
            (visibilityMask << 8,ownerArmy->visibilityRadius,node->worldZQ12 + ownerArmy->visibilityHeightOffset,
             worldXQ12,worldYQ12,worldRuntime->fieldGrid);
  if ((relationStates >> ((uint8_t)(activeFactionIndex << 2) & 31) & FACTION_RELATION_STATE_ALLIED) != 0) {
    TerrainOccupancyBit2_MarkAroundWorldPoint
              (ownerArmy->occupancyMarkRadius,worldXQ12,worldYQ12,ownerFactionIndex,fieldGrid);
  }
}


/* Tests the army's summed weapon damage against target class 0 (targetClassShotDamage[0], stateOrTechnologyId)
   for zero, i.e. an unarmed army (returns true when it is zero); used by
   ArmyRuntime_ResetMovementStateFromModel to decide whether a targeted command is dropped.
*/
Bool8 ArmyRuntime_TestHasNoWeaponDamage(ArmyRuntimeSlot *armyRuntime)

{
  return armyRuntime->stateOrTechnologyId == 0;
}


/* Tests the army's summed weapon damage against target class 0 (targetClassShotDamage[0], stateOrTechnologyId)
   for being non-negative (returns true when it is >= 0). No C code calls it directly.
*/
Bool8 ArmyRuntime_TestWeaponDamageNonnegative(ArmyRuntimeSlot *armyRuntime)

{
  return -1 < armyRuntime->stateOrTechnologyId;
}


/* Runs the placement-validation handler of the army's runtime class
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation, indexed by the definition's
   runtimeClassId) for the army in *armyRuntimeHolder and returns its acceptance.
*/
Bool8 ArmyRuntimeNode_DispatchTypedCallback(ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime)

{
  Bool8 accepted;

  /* the view's first field (modelDefinition) is the army's model runtime pointer */
  accepted = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation
            [((ModelRuntimePlacementValidationView *)*armyRuntimeHolder)->modelDefinition->
             runtimeClassId])
                    (worldRuntime,(ModelRuntimePlacementValidationView *)*armyRuntimeHolder);
  return accepted;
}


/* Runs the class-command handler of the runtime class of the army's model runtime (the definition's
   runtimeClassId) with that model runtime; the AI planners call it to start the class-specific behaviour of the armies
   they create or re-task.
*/
void ArmyRuntime_DispatchClassCommand(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime)

{
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
    [armyRuntime->modelRuntimeOrSavedOffset.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId])
            (worldRuntime,armyRuntime->modelRuntimeOrSavedOffset.modelRuntime);
  return;
}


/* Counterpart of ArmyRuntime_InitializePoolAndGraphics: frees the army runtime pool, releases every faction's
   army texture set and palette, frees the two preview textures (armyAsset[1].rootNodeOffsetOrPointer and
   .registryId) of every registered army asset and clears the asset registry.
*/
void ArmyRuntime_ShutdownPoolAndGraphics(void)

{
  ArmyAssetRecordPrefix *armyAsset;
  ArmyGraphicsBinding *graphicsBinding;
  int bindingIndex;
  int registryIndex;

  g_MemoryApi.free(g_ArmyRuntimeSlots);
  g_ArmyRuntimeSlots = NULL;
  for (bindingIndex = 0; bindingIndex < ARMY_GRAPHICS_BINDING_COUNT; bindingIndex++) {
    graphicsBinding = &g_ArmyGraphicsBindings[bindingIndex];
    if (graphicsBinding->textureSet != NULL) {
      g_GraphicsTextureSetReleasePackage(graphicsBinding->textureSet);
      graphicsBinding->textureSet = NULL;
    }
    if (graphicsBinding->paletteAsset != NULL) {
      g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(graphicsBinding->paletteAsset);
      graphicsBinding->paletteAsset = NULL;
    }
  }
  for (registryIndex = 0; registryIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; registryIndex++) {
    armyAsset = g_ArmyAssetRecordRegistry[registryIndex];
    if (armyAsset != NULL) {
      /* the two preview textures stored in the record that follows the prefix */
      g_MemoryApi.free((void *)armyAsset[1].rootNodeOffsetOrPointer);
      g_MemoryApi.free((void *)armyAsset[1].registryId);
      g_ArmyAssetRecordRegistry[registryIndex] = NULL;
    }
  }
}


/* Savegame writing (called by the in-game save in ui/ingame/runtime): turns the four pointers of every used
   army slot (model runtime, model node, command target, assigned target) into offsets and zeroes the unused
   slots, so the pool can be written as it is (the caller then writes g_ArmyRuntimeSlots,
   ARMY_RUNTIME_SLOT_COUNT slots); ArmyRuntimePool_RebaseAfterLoad is the counterpart.
*/
void ArmyRuntimePool_ConvertPointersToOffsetsForSave(void)

{
  uint32_t assignedTargetOffset;
  ModelRuntimeSlot *savedModelRuntimeOffset;
  ArmyRuntimeSlot *savedTargetOffset;
  ArmyRuntimeSlot *slot;
  uint32_t *slotWords;
  int slotIndex;
  int wordIndex;

  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = &g_ArmyRuntimeSlots[slotIndex];
    if (slot->modelNodeRuntime == NULL) {
      /* an unused slot is zeroed dword by dword */
      slotWords = (uint32_t *)slot;
      for (wordIndex = 0; wordIndex < (int)(sizeof(ArmyRuntimeSlot) / 4); wordIndex++) {
        slotWords[wordIndex] = 0;
      }
      continue;
    }
    savedModelRuntimeOffset = (ModelRuntimeSlot *)
             ((int)(slot->modelRuntimeOrSavedOffset).modelRuntime - g_ModelRuntimeRebaseDelta);
    savedTargetOffset = slot->commandTargetArmyRuntime;
    if (savedTargetOffset != NULL) {
      savedTargetOffset = (ArmyRuntimeSlot *)((int)savedTargetOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    slot->modelNodeRuntime =
         (ModelRuntimeNode *)((int)slot->modelNodeRuntime - (int)g_RuntimeObjectRebaseBaseMinusOne);
    assignedTargetOffset = slot->assignedTargetArmyRuntime;
    (slot->modelRuntimeOrSavedOffset).modelRuntime = savedModelRuntimeOffset;
    if (assignedTargetOffset != 0) {
      assignedTargetOffset = assignedTargetOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    slot->commandTargetArmyRuntime = savedTargetOffset;
    slot->assignedTargetArmyRuntime = assignedTargetOffset;
  }
}


/* After a savegame load: turns the saved offsets in every used army slot (model node != 0) back into
   pointers, the counterpart of ArmyRuntimePool_ConvertPointersToOffsetsForSave. Model runtime
   (modelRuntimeOrSavedOffset) and model node (modelNodeRuntime) are rebased by their pools' deltas; the army
   references (commandTargetArmyRuntime, assignedTargetArmyRuntime) are saved as pointer - (pool base - 1), so 0
   stays NULL.
*/
void ArmyRuntimePool_RebaseAfterLoad(void)

{
  uint32_t savedAssignedTargetOffset;
  void *rebasedModelRuntime;
  int slotIndex;
  ArmyRuntimeSlot *rebasedCommandTarget;
  ArmyRuntimeSlot *slot;

  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = &g_ArmyRuntimeSlots[slotIndex];
    if (slot->modelNodeRuntime == NULL) {
      continue;
    }
    /* modelRuntime + g_ModelRuntimeRebaseDelta */
    rebasedModelRuntime = (uint8_t *)(slot->modelRuntimeOrSavedOffset).modelRuntime + g_ModelRuntimeRebaseDelta;
    rebasedCommandTarget = NULL;
    if (slot->commandTargetArmyRuntime != NULL) {
      rebasedCommandTarget =
           (ArmyRuntimeSlot *)((int)slot->commandTargetArmyRuntime + (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    /* modelNodeRuntime + g_RuntimeObjectRebaseBaseMinusOne */
    slot->modelNodeRuntime =
         (ModelRuntimeNode *)(g_RuntimeObjectRebaseBaseMinusOne + (int)slot->modelNodeRuntime);
    savedAssignedTargetOffset = slot->assignedTargetArmyRuntime;
    (slot->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)rebasedModelRuntime;
    if (savedAssignedTargetOffset != 0) {
      savedAssignedTargetOffset = savedAssignedTargetOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    slot->commandTargetArmyRuntime = rebasedCommandTarget;
    slot->assignedTargetArmyRuntime = savedAssignedTargetOffset;
  }
}


/* One looping sound of ArmyRuntimeClass_UpdateGroundPositionedSounds and
   ArmyRuntimeClass_UpdateWaterPositionedSounds (slot index soundSlotIndex in the world's
   sound slot array; 0, out of range or an empty slot is ignored) kept at the model's root position, only where
   the active faction's cell bits 0/1 are set. */
static void ArmyRuntimeClass_UpdateGroundLoopSoundAtModel(WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot *modelRuntime,ModelDefinition *definition,uint32_t soundSlotIndex)
{
  SpatialSoundSlot *soundSlot;
  GraphicsFixedVec3 *worldPosition;
  Bool8 cellMasked;

  if ((soundSlotIndex == 0) || (soundSlotIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == NULL)) {
    return;
  }
  soundSlot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
  if (soundSlot == NULL) {
    return;
  }
  worldPosition = &(modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation;
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y,worldPosition->x,
                     worldRuntime);
  if (!cellMasked) {
    SpatialSound_UpdateDesiredPositionedGains
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,worldPosition,
               soundSlot);
  }
}

/* Sound update of a moving ground army, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .classMethodD[1] and through ArmyRuntimeAudio_DispatchPositionedSoundVariant (classMethodD[18],
   every placement kind but water). While the model turns it keeps the turning sound (definition
   turningLoopSoundSlotIndex) at the model's position; while it turns or drives it keeps the movement
   sound (movingLoopSoundSlotIndex) there; both only
   where the active faction's cell bits 0/1 are set (TerrainGrid_TestProjectedCellMaskBits01). Identical to
   ArmyRuntimeClass_UpdateWaterPositionedSounds except for the reload below.
*/

void ArmyRuntimeClass_UpdateGroundPositionedSounds(WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((modelRuntime->movementControl).turnVelocityAngle16 == 0) {
    if ((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    ArmyRuntimeClass_UpdateGroundLoopSoundAtModel
              (worldRuntime,modelRuntime,definition,definition->turningLoopSoundSlotIndex);
    /* Reload (the original skips this after a masked cell; nothing in between writes it). */
    definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  }
  ArmyRuntimeClass_UpdateGroundLoopSoundAtModel
            (worldRuntime,modelRuntime,definition,definition->movingLoopSoundSlotIndex);
}


/* Empty sound update of class 3, reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[3].
*/
void ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  return;
}


/* Depth-first over a model runtime tree (the attached child model runtimes, null slots skipped):
   the last node whose definition has class MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR, or null.
   Part of ArmyRuntimeClass_SelectProjectileTargetNode (the original walks the tree inline). */
static uint8_t *ArmyRuntimeClass_FindLastClass10Node(uint8_t *node)
{
  uint8_t *found = NULL;
  int i;
  if (((ModelRuntimeSlot *)node)->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
      MODEL_RUNTIME_CLASS_10_CONTINUOUS_RADAR) {
    found = node;
  }
  for (i = 0; i < (int)((ModelRuntimeSlot *)node)->attachmentCount; i++) {
    uint8_t *child = (uint8_t *)((ModelRuntimeSlot *)node)->attachments[i].childModelRuntimeOrSavedOffset;
    if (child != NULL) {
      uint8_t *match = ArmyRuntimeClass_FindLastClass10Node(child);
      if (match != NULL) {
        found = match;
      }
    }
  }
  return found;
}

/* Owner-list callback of ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (passed to
   WorldRuntime_ForEachOwnerListNode). For a model of another, non-neutral faction within the shot's
   selection range it stores the last radar node (class 10) of that model as the target
   (timedTargetLinkState.selectedTargetModelRuntime), unless that node is destroyed; for a shot of the same shot
   definition it records that one is still in flight (timedTargetLinkState.matchingActiveShotRuntime).
*/

void ArmyRuntimeClass_SelectProjectileTargetNode(ModelRuntimeTimedTargetProjectileView *modelRuntime,
          WorldOwnerListNode *candidateNode)

{
  int64_t deltaYSquared;
  int64_t remainingRangeSquared;
  int deltaX;
  int deltaY;
  int selectionRange;
  int candidateFactionIndex;
  ModelRuntimeSlot *targetModelRuntime;

  if (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    selectionRange = ((modelRuntime->modelDefinition->shotDefinitionReference).definition)->
            mode2SelectionRangeQ12;
    deltaX = candidateNode->worldXQ12 - (modelRuntime->rootModelNode->worldTransform).translation.x;
    remainingRangeSquared = (int64_t)selectionRange * (int64_t)selectionRange - (int64_t)deltaX * (int64_t)deltaX;
    if (remainingRangeSquared < 0) {
      return;
    }
    deltaY = candidateNode->worldYQ12 - (modelRuntime->rootModelNode->worldTransform).translation.y;
    deltaYSquared = (int64_t)deltaY * (int64_t)deltaY;
    /* the original tests the sign of the high dword of remainingRangeSquared - deltaYSquared; both are
       non-negative, so this is a plain comparison */
    if (remainingRangeSquared < deltaYSquared) {
      return;
    }
    candidateFactionIndex =
         ((ModelRuntimeSlot *)candidateNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
    if ((candidateFactionIndex == modelRuntime->ownerArmyRuntime->factionIndex) || (candidateFactionIndex == 0)) {
      return;
    }
    /* pick the last node, depth-first, whose definition has class 10 (runtimeClassId) */
    targetModelRuntime = (ModelRuntimeSlot *)ArmyRuntimeClass_FindLastClass10Node((uint8_t *)candidateNode->runtimePayload);
    if ((targetModelRuntime != NULL) && (((targetModelRuntime->classState).stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0)) {
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime = targetModelRuntime;
    }
  }
  else if ((candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          ((modelRuntime->modelDefinition->shotDefinitionReference).definition ==
           (ShotDefinition *)(((ModelRuntimeSlot *)candidateNode->runtimePayload)->definitionOrSavedId).definition)) {
    (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime = (ShotRuntimeSlot *)candidateNode->runtimePayload;
  }
}


/* Runtime update of class 20 (a launcher that fires at enemy radar), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[20]. When its reload countdown has run out it
   shows the loaded missile (mesh group bit 0) and scans the world with ArmyRuntimeClass_SelectProjectileTargetNode;
   if a radar target is in range and none of its own shots is still in flight, it hides the missile, restarts the
   reload and fires from its attachment points at the target's position.
*/

void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects
          (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView *modelRuntime)

{
  ModelDefinitionTimedTargetProjectileView *timedTargetDefinition;
  ModelRuntimeNode *rootNode;
  ModelRuntimeNode *targetRootNode;
  ModelRuntimeSlot *selectedTarget;
  Q12 targetWorldXQ12;
  Q12 targetWorldYQ12;
  Q12 targetWorldZQ12;
  int reloadCountdownTicks;

  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_INACTIVE_MASK) == 0) {
    timedTargetDefinition = modelRuntime->modelDefinition;
    reloadCountdownTicks = (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks -
            g_InGameSimulationStepTicks;
    rootNode = modelRuntime->rootModelNode;
    (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks = reloadCountdownTicks;
    if (reloadCountdownTicks < 1) {
      (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks = 0;
      /* show the loaded missile */
      (rootNode->modelPayload).meshGroupMask |= 1;
      (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime = NULL;
      (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime = NULL;
      /* signature differs: the callback's context is ModelRuntimeTimedTargetProjectileView *, the slot's void * */
      WorldRuntime_ForEachOwnerListNode
                (modelRuntime,(WorldRuntimeNodeTraversalCallback *)ArmyRuntimeClass_SelectProjectileTargetNode,
                 worldRuntime);
      selectedTarget = (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime;
      if (((modelRuntime->timedTargetLinkState).matchingActiveShotRuntime ==
           NULL) && (selectedTarget != NULL)) {
        targetRootNode = (selectedTarget->rootModelNodeOrSavedOffset).modelNode;
        targetWorldXQ12 = (targetRootNode->worldTransform).translation.x;
        targetWorldYQ12 = (targetRootNode->worldTransform).translation.y;
        targetWorldZQ12 = (targetRootNode->worldTransform).translation.z;
        rootNode = modelRuntime->rootModelNode;
        (modelRuntime->timedTargetState).targetProjectileReloadCountdownTicks +=
             (timedTargetDefinition->timedTargetParameters).reloadTicks;
        /* hide the missile and fire */
        rootNode->runtimeFlags = rootNode->runtimeFlags | 1;
        (rootNode->modelPayload).meshGroupMask &= ~1u;
        ModelRuntime_EmitProjectilesFromAttachmentPoints
                  ((ShotTargetModelReference)
                   (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime,targetWorldZQ12
                   ,targetWorldYQ12,targetWorldXQ12,(timedTargetDefinition->shotDefinitionReference).definition,
                   rootNode,(MdlSerializedNodeHeader *)timedTargetDefinition->rootNodeOffsetOrPointer,
                   worldRuntime);
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
  (modelRuntime->timedTargetLinkState).selectedTargetModelRuntime = NULL;
  (modelRuntime->timedTargetLinkState).matchingActiveShotRuntime = NULL;
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Sound update of a moving army on water, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .classMethodD[19] and through ArmyRuntimeAudio_DispatchPositionedSoundVariant (classMethodD[18],
   placement kind 1 = water surface). Same as ArmyRuntimeClass_UpdateGroundPositionedSounds: the turning sound
   (definition turningLoopSoundSlotIndex) while turning, the movement sound (movingLoopSoundSlotIndex) while
   turning or driving, only where the active faction's cell bits 0/1 are set.
*/

void ArmyRuntimeClass_UpdateWaterPositionedSounds(WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  if ((modelRuntime->movementControl).turnVelocityAngle16 == 0) {
    if ((modelRuntime->movementControl).movementAdvancePerTickQ12 == 0) {
      return;
    }
  }
  else {
    ArmyRuntimeClass_UpdateGroundLoopSoundAtModel
              (worldRuntime,modelRuntime,definition,definition->turningLoopSoundSlotIndex);
  }
  ArmyRuntimeClass_UpdateGroundLoopSoundAtModel
            (worldRuntime,modelRuntime,definition,definition->movingLoopSoundSlotIndex);
}


/* Segment meter of the selection panel (called directly by gameplay/selection/runtime with a model runtime):
   filled segments from linkedChildBuildState.completedSecondaryArmyAssetCount of the passed runtime (the
   completed linked assets of a class-22 pad), total segments from its definition's linkedChildSlotCapacity.
*/
ArmySegmentMeter ArmyRuntime_GetLinkedChildSlotMeter(ModelRuntimeLinkedChildSpawnAndBuildView *linkedChildRuntime)

{
  ArmySegmentMeter slotMeter;

  slotMeter.totalSegments = linkedChildRuntime->modelDefinition->linkedChildSlotCapacity;
  slotMeter.filledSegments = (linkedChildRuntime->linkedChildBuildState).completedSecondaryArmyAssetCount;
  return slotMeter;
}


/* Returns which of the three linked-child asset ids (g_InGamePointerModePreviewArmyIds[1]/[2]/[4] as bits 1/2/4)
   occur among the army's 13 attachment asset-id slots (completedSecondaryArmyAssetIds); the selection panel ORs these
   masks over all selected armies.
*/
int ArmyRuntime_GetAttachmentEffectVariantMask(ModelRuntimeLinkedChildSpawnAndBuildView *linkedChildRuntime)

{
  int attachmentAssetId;
  int attachmentEffectSlotsRemaining;
  uint32_t variantMask;

  variantMask = 0;
  attachmentEffectSlotsRemaining = 13;
  do {
    /* completedSecondaryArmyAssetIds[0] of the current window; the runtime pointer itself moves one dword per
       slot below */
    attachmentAssetId = linkedChildRuntime->completedSecondaryArmyAssetIds[0];
    if (attachmentAssetId == g_InGamePointerModePreviewArmyIds[1]) {
      variantMask = variantMask | 1;
    }
    if (attachmentAssetId == g_InGamePointerModePreviewArmyIds[2]) {
      variantMask = variantMask | 2;
    }
    if (attachmentAssetId == g_InGamePointerModePreviewArmyIds[4]) {
      variantMask = variantMask | 4;
    }
    linkedChildRuntime = (ModelRuntimeLinkedChildSpawnAndBuildView *)((uint8_t *)linkedChildRuntime + 4);
    attachmentEffectSlotsRemaining--;
  } while (attachmentEffectSlotsRemaining != 0);
  return variantMask;
}


/* Keeps the model's looping sound (its definition's loopingSoundSlotIndex) at the model's position while
   state flag 1 (switched off) is clear and the active faction's cell bits 0/1 are set there. Reached through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD[10], [14] and [16] and through
   ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled (classMethodD[4]).
*/
void ArmyRuntime_UpdateLoopingPositionedSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelDefinition *definition;
  uint32_t soundSlotIndex;
  SpatialSoundSlot *slot;
  Bool8 cellMasked;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  soundSlotIndex = definition->loopingSoundSlotIndex;
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) != 0) || (soundSlotIndex == 0) ||
      (soundSlotIndex >= worldRuntime->dwordArrayCount) || (worldRuntime->dwordArray == NULL)) {
    return;
  }
  slot = (SpatialSoundSlot *)worldRuntime->dwordArray[soundSlotIndex];
  if (slot == NULL) {
    return;
  }
  worldPosition = &(modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation;
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y,
                     worldPosition->x,worldRuntime);
  if (!cellMasked) {
    SpatialSound_UpdateDesiredPositionedGains
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,worldPosition,slot);
  }
}


/* The deployment one-shot sound of ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState at the root
   node's position, only where the active faction's cell bits 0/1 are set. */
static void ArmyRuntimeClass_PlayVerticalDeploymentSound(WorldRuntimeContext *worldRuntime,
          ModelDefinitionVerticalDeploymentView *deploymentDefinition,ModelRuntimeNode *rootNode)
{
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **soundVoiceSet;
  Bool8 cellMasked;

  soundAssetIndex = deploymentDefinition->deploymentSoundAssetIndex;
  if ((soundAssetIndex == 0) || (soundAssetIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == NULL)) {
    return;
  }
  soundVoiceSet = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
  if (soundVoiceSet == NULL) {
    return;
  }
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,worldRuntime);
  if (!cellMasked) {
    SpatialSound_PlayPositionedOneShot
              (deploymentDefinition->positionedSoundMaximumDistanceQ12,deploymentDefinition->positionedSoundGainQ15,
               &(rootNode->worldTransform).translation,soundVoiceSet);
  }
}

/* Runtime update of class 23 (a platform that armies of its faction can dock on, see
   ArmyRuntime_HandleCollisionPartner), reached only through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[23]. While an army is linked (bit 0 of
   classState.behaviorState, re-checked against the army's expanded radius every 8 ticks) the child platform node
   moves by the step per tick until the travel (deploymentState.deploymentTravelQ12) reaches the definition's
   limit; without a linked army it moves back until the travel is 0. A positioned one-shot sound marks the start
   of either movement.
*/

void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState
          (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView *modelRuntime)

{
  ModelDefinitionVerticalDeploymentView *deploymentDefinition;
  ModelRuntimeSlot *linkedModelRuntime;
  int travelLimit;
  int travelStep;
  Bool8 linkedStillInRange;
  ModelRuntimeNode *rootNode;
  ModelRuntimeNode *platformNode;

  deploymentDefinition = modelRuntime->modelDefinition;
  rootNode = modelRuntime->rootModelNode;
  linkedModelRuntime = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime;
  travelLimit = deploymentDefinition->deploymentTravelLimitQ12;
  if (((modelRuntime->classState).behaviorState & 1) == 0) {
    /* no army linked (bit 0 clear): move back until the travel is 0 */
    if ((modelRuntime->deploymentState).deploymentTravelQ12 == 0) {
      return;
    }
    if ((modelRuntime->deploymentState).deploymentTravelQ12 <= travelLimit) {
      ArmyRuntimeClass_PlayVerticalDeploymentSound(worldRuntime,deploymentDefinition,rootNode);
    }
    platformNode = rootNode->childNodes[0];
    travelStep = deploymentDefinition->verticalDeploymentStepQ12PerTick;
    (modelRuntime->classState).behaviorState &= ~2u;
    travelStep = travelStep * g_InGameSimulationStepTicks;
    if (platformNode != NULL) {
      (platformNode->modelPayload).localTranslationZQ12 += travelStep;
      (modelRuntime->deploymentState).deploymentTravelQ12 += travelStep;
      ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
    }
    return;
  }
  /* docking requested or army linked (bit 0): when the retry countdown has run out, bit 0 is dropped and only
     restored while the linked army is still within range */
  if ((modelRuntime->deploymentState).collisionRetryCountdown != 0) {
    (modelRuntime->deploymentState).collisionRetryCountdown -= 1;
  }
  (modelRuntime->classState).behaviorState |= 2;
  if ((modelRuntime->deploymentState).collisionRetryCountdown == 0) {
    (modelRuntime->classState).behaviorState &= ~1u;
    if (linkedModelRuntime != NULL) {
      (modelRuntime->deploymentState).collisionRetryCountdown = 8;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
      linkedStillInRange = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                             (deploymentDefinition->footprintRadius,
                              (rootNode->worldTransform).translation.y,
                              (rootNode->worldTransform).translation.x,linkedModelRuntime);
      if (linkedStillInRange) {
        (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = linkedModelRuntime;
        (modelRuntime->classState).behaviorState |= 1;
      }
    }
  }
  /* move towards the travel limit */
  if (travelLimit < (modelRuntime->deploymentState).deploymentTravelQ12) {
    (modelRuntime->classState).behaviorState &= ~2u;
    if ((modelRuntime->deploymentState).deploymentTravelQ12 == 0) {
      ArmyRuntimeClass_PlayVerticalDeploymentSound(worldRuntime,deploymentDefinition,rootNode);
    }
    if (rootNode->childNodes[0] != NULL) {
      travelStep = deploymentDefinition->verticalDeploymentStepQ12PerTick * g_InGameSimulationStepTicks;
      (rootNode->childNodes[0]->modelPayload).localTranslationZQ12 -= travelStep;
      (modelRuntime->deploymentState).deploymentTravelQ12 -= travelStep;
      ModelNodeRuntime_RebuildTransformsFromRoot(modelRuntime->rootModelNode);
    }
  }
}


/* Fires a shot of the weapon code (called directly by gameplay/army/combat): looks up the launch point
   (packed key attachmentSelectorOrdinal << 4 | 2) in the sprite model of definitionNode, transforms it by the
   freshly rebuilt modelNode and creates the projectile from there towards the target point. Returns true
   when the model has no such launch point.
*/
Bool8 ArmyRuntime_ResolveShotLaunchFromModelAttachment
          (ShotTargetModelReference targetModelReference,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime)

{
  ModelResource *spriteModelResource;
  Q12 launchWorldYQ12;
  Q12 launchWorldZQ12;
  ModelPackedLookupTableEntryCount remainingEntries;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint launchPoint;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  spriteModelResource = (definitionNode->spriteAssetReference).modelResource;
  remainingEntries = spriteModelResource->packedLookupTableEntryCount;
  localPointRecord =
       (ModelPackedPointRecord *)((uint8_t *)spriteModelResource + spriteModelResource->packedLookupTableRelativeOffset);
  /* find the attachment point record (key = selector << 4 | 2); none -> fail */
  while (remainingEntries != 0 && localPointRecord->packedLookupKey != (attachmentSelectorOrdinal << 4 | 2)) {
    localPointRecord = localPointRecord + 1;
    remainingEntries = remainingEntries - 1;
  }
  if (remainingEntries == 0) {
    return true;
  }
  launchPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,modelNode);
  launchWorldZQ12 = launchPoint.zQ12;
  launchWorldYQ12 = launchPoint.yQ12;
  ShotRuntimePool_CreateProjectileFromDefinition
            (targetModelReference,
             (ArmyRuntimeSlot *)((modelNode->runtimePayload).armyRuntime)->linkedEntityRuntime,
             targetWorldXQ12,targetWorldYQ12,targetWorldZQ12,launchWorldZQ12,launchWorldYQ12,
             launchPoint.xQ12,shotDefinition,worldRuntime);
  return false;
}


/* Accelerates a moving model (called directly by the movement class updates in gameplay/army/movement): the
   speed limit is the definition's movementSpeed. While the pitch (worldRotationAngle1) is below the first class threshold it is cut to 5/16, unless
   the pitch is at least the second threshold: then it stays full, or 5/8 when angle2 - angle0 lies between a
   quarter and three quarters of a turn. The advance per tick grows by accelerationPerTick up to that
   limit (and drops to it at once). When the model starts from standstill its start sound plays where the active faction's cell bits 0/1 are set.
*/
void ArmyRuntime_UpdateActivationMetricAndPlayStartSound(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  int previousAdvance;
  ModelDefinition *definition;
  AngleTurn32 rotationAngle1;
  DirectSoundVoiceSet **voiceSetRef;
  uint32_t headingDelta;
  uint32_t speedLimit;
  uint32_t currentAdvance;
  uint32_t acceleratedAdvance;
  uint32_t newAdvance;
  uint32_t startSoundSlotIndex;
  Bool8 cellMasked;
  ModelRuntimeNode *rootNode;

  rootNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  rotationAngle1 = (rootNode->modelPayload).worldRotationAngle1;
  speedLimit = definition->movementSpeed;
  if ((int)rotationAngle1 < (int)definition->traversalSecondaryThreshold) {
    headingDelta = (rootNode->modelPayload).worldRotationAngle2 -
            (rootNode->modelPayload).worldRotationAngle0 & FIXED_ANGLE16_MASK;
    speedLimit = speedLimit * 5 >> 4;
    if ((int)definition->runtimeValue24 <= (int)rotationAngle1) {
      speedLimit = definition->movementSpeed;
      if ((FIXED_ANGLE16_QUARTER_TURN < headingDelta) && (headingDelta < 3 * FIXED_ANGLE16_QUARTER_TURN)) {
        speedLimit = speedLimit * 5 >> 3;
      }
    }
  }
  /* accelerate towards the limit; at or above it the advance drops to it at once */
  newAdvance = speedLimit;
  currentAdvance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  if (currentAdvance < speedLimit) {
    acceleratedAdvance = currentAdvance + definition->accelerationPerTick;
    if (acceleratedAdvance < speedLimit) {
      newAdvance = acceleratedAdvance;
    }
  }
  /* atomic exchange, as in the original */
  LOCK();
  previousAdvance = (modelRuntime->movementControl).movementAdvancePerTickQ12;
  (modelRuntime->movementControl).movementAdvancePerTickQ12 = newAdvance;
  UNLOCK();
  if ((previousAdvance == 0) && (newAdvance != 0)) {
    startSoundSlotIndex = definition->moveStartSoundSlotIndex;
    if ((startSoundSlotIndex != 0) &&
       ((startSoundSlotIndex < worldRuntime->dwordArrayCount && (worldRuntime->dwordArray != NULL)))) {
      voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[startSoundSlotIndex];
      worldPosition = &(rootNode->worldTransform).translation;
      if (voiceSetRef != NULL) {
        cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                          ((rootNode->worldTransform).translation.y,worldPosition->x,worldRuntime)
        ;
        if (!cellMasked) {
          SpatialSound_PlayPositionedOneShot
                    (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,
                     worldPosition,voiceSetRef);
        }
      }
    }
  }
  return;
}


/* Reacts to the model a moving model has run into (called directly by the movement code in
   gameplay/army/movement with the mover's model runtime and position, Y before X). A free class-23 platform of
   the same faction is told to dock (behaviorState bit 0, collision retry countdown 0x20) and, once it is ready
   (bit 1), the two model runtimes are linked to each other (classState.linkedArmyRuntimeOrSavedOffset); a model
   of class 0 is run over and takes impact damage 0x100000 from the direction of the collision.
*/
void ArmyRuntime_HandleCollisionPartner(ModelRuntimeSlot *currentModelRuntime,Q12 currentWorldYQ12,Q12 currentWorldXQ12,
          ModelRuntimeSlot *collisionPartnerModelRuntime,WorldRuntimeContext *worldRuntime)

{
  uint32_t impactAngle;
  ModelRuntimeVerticalDeploymentView *platformRuntime;

  if (collisionPartnerModelRuntime == NULL) {
    return;
  }
  if (collisionPartnerModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
      MODEL_RUNTIME_CLASS_23) {
    platformRuntime = (ModelRuntimeVerticalDeploymentView *)collisionPartnerModelRuntime;
    if ((platformRuntime->ownerArmyRuntime->factionIndex ==
         currentModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) &&
       ((platformRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime == NULL)) {
      (platformRuntime->classState).behaviorState |= 1;
      (platformRuntime->deploymentState).collisionRetryCountdown = 32;
      if (((platformRuntime->classState).behaviorState & 2U) != 0) {
        (platformRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime = currentModelRuntime;
        (currentModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime =
             collisionPartnerModelRuntime;
      }
    }
  }
  else if (collisionPartnerModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId ==
           MODEL_RUNTIME_CLASS_00) {
    impactAngle = FixedMath_Atan2Angle16
                            ((collisionPartnerModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).
                             translation.y - currentWorldYQ12,
                             (collisionPartnerModelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).
                             translation.x - currentWorldXQ12);
    ArmyRuntime_ApplyImpactDamageAndFinalizeState
              (impactAngle,ARMY_CRUSH_IMPACT_DAMAGE,collisionPartnerModelRuntime);
  }
}


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
  previewArmy = (GameEntityRuntime *)ArmyRuntime_CreateInstanceFromAsset
                     (1,0,ARMY_PREVIEW_WORLD_POSITION_Q12,ARMY_PREVIEW_WORLD_POSITION_Q12,factionIndex,armyAssetId,
                      worldRuntime,NULL);
  if (previewArmy == NULL) {
    return NULL;
  }
  rootNode = (previewArmy->common).ownership.modelNode;
  /* armies of runtime class 13 lose their fourth child node */
  if (((((ModelRuntimeSlot *)(previewArmy->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
        runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) && (3 < rootNode->childCount)) &&
     (rootNode->childNodes[3] != NULL)) {
    WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)rootNode->childNodes[3]);
    rootNode->childNodes[3] = NULL;
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
  previewTexture = (GameEntityRuntime *)g_GraphicsOffscreenRenderModelListToTextureSource
                     (&g_ArmyPreviewSceneExtents,
                      g_ArmyPreviewAuxiliaryOrientation,
                      &g_ArmyPreviewViewParameters,
                      previewHeight * 2,previewWidth * 2,1,
                      &g_ArmyPreviewModelNode);
  if (previewTexture == NULL) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,previewArmy);
    return NULL;
  }
  /* the texture is typed as GameEntityRuntime here: its pixels start at +0x220 and the header fields
     rewritten below (+0x200/+0x204 and +0x218/+0x21C) hold its width and height; +0x04 is the
     allocation size */
  sourcePixels = (uint32_t *)&previewTexture[1].common.commandTarget.targetWorldXQ12;
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
  halvedHeight = (int)previewTexture[1].common.commandTarget.targetEntity >> 1;
  previewTexture[1].common.commandFlags = (GameEntityCommandFlags)halvedWidth;
  previewTexture[1].common.commandTarget.targetEntity = (GameEntityRuntime *)halvedHeight;
  previewTexture[1].common.ownership.definitionOrClassRecord = THANDOR_PTR(halvedWidth);
  previewTexture[1].common.ownership.modelNode = (ModelRuntimeNode *)halvedHeight;
  allocationSize = (uint32_t)(halvedWidth * halvedHeight * 4 + ARMY_PREVIEW_TEXTURE_HEADER_BYTES);
  (previewTexture->common).ownership.modelNode = (ModelRuntimeNode *)allocationSize;
  g_MemoryApi.shrinkInPlace(allocationSize,previewTexture);
  return (GraphicsTextureResource *)previewTexture;
}


/* Per-step update of one model runtime and, recursively, its attached children (called by
   ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers, the primaryUpdate entry of g_RuntimeMaintenanceCallbackPhases).
   Runs the class callback runtimeUpdate[class] of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes, then the
   shared bookkeeping: health regeneration, dismantling (Xenite refund while health drains, destruction effect
   at zero), the attachment channel ticks of a model without health, and technology research (Xenite paid once
   when affordable, the Energy load held until the technology is unlocked).
*/
void ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelDefinition *definition;
  uint32_t previousHealth;
  uint32_t healthLimit;
  uint32_t clampedHealth;
  uint32_t healthDrain;
  uint32_t linkedAssetRefund;
  ModelRuntimeNode *rootNode;
  int factionIndex;
  int researchProgress;
  int researchEnergyLoad;
  int energyRequirement;
  uint32_t ticksRemaining;
  uint32_t attachmentCount;
  uint32_t attachmentIndex;
  uint32_t parentStateFlags;
  ModelRuntimeSlot *childModelRuntime;

  definition = (ModelDefinition *)(modelRuntime->definitionOrSavedId).savedIdOrOffset;
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[definition->runtimeClassId]
            (worldRuntime,modelRuntime);
  /* every 4 ticks health regenerates by healthRegenerationPerStep up to 3/4 of the definition's
     health (maximumHealth), or decays down to it while bit 0 is set */
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DISMANTLING) == 0) {
    (modelRuntime->classState).healthRegenerationDelayTicks -= g_InGameSimulationStepTicks;
    if ((int)(modelRuntime->classState).healthRegenerationDelayTicks < 0) {
      (modelRuntime->classState).healthRegenerationDelayTicks = 4;
      previousHealth = modelRuntime->health;
      healthLimit = (definition->maximumHealth * 3) >> 2;
      if (previousHealth != 0) {
        if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
          if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_NO_REGENERATION) == 0) {
            clampedHealth = previousHealth + definition->healthRegenerationPerStep;
            if ((int)healthLimit < (int)clampedHealth) {
              clampedHealth = healthLimit;
            }
            if ((int)modelRuntime->health < (int)clampedHealth) {
              modelRuntime->health = clampedHealth;
            }
          }
        }
        else {
          clampedHealth = previousHealth - definition->healthRegenerationPerStep;
          if ((int)clampedHealth < (int)healthLimit) {
            clampedHealth = healthLimit;
          }
          if ((int)clampedHealth < (int)modelRuntime->health) {
            modelRuntime->health = clampedHealth;
          }
        }
      }
    }
  }
  (modelRuntime->classState).dismantleTickCountdown -= g_InGameSimulationStepTicks;
  if ((int)(modelRuntime->classState).dismantleTickCountdown < 0) {
    (modelRuntime->classState).dismantleTickCountdown += 12;
    if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DISMANTLING) != 0) &&
        (definition->xeniteValueQ4 != 0) && (definition->maximumHealth != 0) &&
        (0 < (int)modelRuntime->health)) {
      /* dismantling, every 12 ticks: refund 1/32 of the Xenite value and drain 1/16 of the health */
      factionIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
      g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + (definition->xeniteValueQ4 >> 5);
      healthDrain = definition->maximumHealth >> 4;
      if (definition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        /* a pad also refunds its unlaunched linked assets */
        linkedAssetRefund = ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
                              ((ArmyRuntimeLinkedChildMaskSlotView *)modelRuntime);
        g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
             g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 + linkedAssetRefund;
      }
      previousHealth = modelRuntime->health;
      modelRuntime->health -= healthDrain;
      if ((int)previousHealth <= (int)healthDrain) { /* signed compare of the old health */
        modelRuntime->health = 0;
        (modelRuntime->classState).stateFlags ^= (ARMY_MODEL_STATE_DISMANTLING | ARMY_MODEL_STATE_DISMANTLED);
        rootNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                   THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelRuntime = modelRuntime },
                   (rootNode->modelPayload).worldRotationAngle2,
                   (rootNode->modelPayload).worldRotationAngle1,
                   (rootNode->modelPayload).worldRotationAngle0,(rootNode->worldTransform).translation.z,
                   (rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,
                   definition->removalEffectDefinitionReference.definition,worldRuntime);
        /* Setting it also skips the attachment tick loop below (the original jumps past it). */
        (modelRuntime->classState).stateFlags |= ARMY_MODEL_STATE_DESTRUCTION_STARTED;
      }
    }
  }
  /* health gone (and not already exploding): tick the attachment channels once per simulation tick
     (only when a linked model runtime exists) */
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0) &&
      ((int)modelRuntime->health < 1) &&
      ((modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime != NULL)) {
    /* Original quirk: the body runs once before the counter is tested, so a step of 0 ticks wraps around. */
    ticksRemaining = g_InGameSimulationStepTicks;
    do {
      ArmyRuntime_ProcessReadyAttachmentChannels(worldRuntime,modelRuntime);
      modelRuntime->destructionEffectTimers[0] = modelRuntime->destructionEffectTimers[0] - 1;
      modelRuntime->destructionEffectTimers[1] = modelRuntime->destructionEffectTimers[1] - 1;
      modelRuntime->destructionEffectTimers[2] = modelRuntime->destructionEffectTimers[2] - 1;
      modelRuntime->destructionEffectTimers[3] = modelRuntime->destructionEffectTimers[3] - 1;
      modelRuntime->destructionEffectTimers[4] = modelRuntime->destructionEffectTimers[4] - 1;
      modelRuntime->destructionEffectTimers[5] = modelRuntime->destructionEffectTimers[5] - 1;
      modelRuntime->destructionEffectTimers[6] = modelRuntime->destructionEffectTimers[6] - 1;
      modelRuntime->destructionEffectTimers[7] = modelRuntime->destructionEffectTimers[7] - 1;
      ticksRemaining--;
    } while (ticksRemaining != 0);
  }
  /* research progress */
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCHING) != 0) &&
      (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0)) {
    researchProgress = modelRuntime->researchElapsedTicks + g_InGameSimulationStepTicks;
    modelRuntime->researchElapsedTicks = researchProgress;
    if (modelRuntime->researchDurationTicks <= researchProgress) {
      Technology_UnlockForFaction
                ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.y,
                 (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                 translation.x,modelRuntime->researchTechnologyId,
                 ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex);
      researchEnergyLoad = modelRuntime->researchEnergyLoadQ4;
      (modelRuntime->classState).stateFlags &= ~ARMY_MODEL_STATE_RESEARCHING;
      (modelRuntime->classState).energyLoadQ4 -= researchEnergyLoad;
    }
  }
  /* queued research starts once its Xenite cost can be paid */
  if ((((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RESEARCH_UNPAID) != 0) &&
     (((modelRuntime->classState).stateFlags & (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_PRODUCING)) == 0)) {
    factionIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex;
    energyRequirement = modelRuntime->researchEnergyLoadQ4;
    if (modelRuntime->researchXeniteCostQ4 <=
        (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4) {
      g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 =
           g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4 -
           modelRuntime->researchXeniteCostQ4;
      modelRuntime->researchXeniteCostQ4 = 0;
      (modelRuntime->classState).stateFlags ^= (ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID);
      (modelRuntime->classState).energyLoadQ4 += energyRequirement;
    }
  }
  /* recurse into the attached child models, passing bit 8 on to them */
  attachmentCount = modelRuntime->attachmentCount;
  parentStateFlags = (modelRuntime->classState).stateFlags;
  for (attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++) {
    childModelRuntime = modelRuntime->attachments[attachmentIndex].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != NULL) {
      (childModelRuntime->classState).stateFlags |= parentStateFlags & 8;
      ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive(worldRuntime,childModelRuntime);
    }
  }
}


/* Launches one linked asset of a class-22 pad (called directly by
   ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode): finds a not yet launched slot of
   completedSecondaryArmyAssetIds holding linkedArmyAssetId, creates that army on the pad, marks the slot as used
   and links the new aircraft to the pad (classLinkState.modelLinkOrState, state 1 = parked) with the attack
   point and heading (classState70..78) and the pad's platform height; its health is scaled by the pad's health.
   Returns true when no slot matches or the creation fails.
*/
Bool8 ArmyRuntimeSpawner_CreateLinkedChildInstance
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  ArmyRuntimeLinkedChildSlotMaskState *slotMaskState;
  ModelRuntimeSlot *childModelRuntime;
  ModelRuntimeNode *parentRootNode;
  int remainingSlots;
  uint32_t slotBit;
  const Q12 *slotAssetId;
  ArmyRuntimeSlot *createdArmy;
  ModelRuntimeNode *modelNode;

  /* the pad's slots hold one 32-bit asset id each, from completedSecondaryArmyAssetIds on (this view's
     movementTarget0Q12; classParameterC4 slots). Original quirk: slot 0
     is tested even with 0 slots, and the count then runs below 0 instead of stopping. */
  slotBit = 1;
  remainingSlots = ((ModelDefinition *)armyRuntime->definitionOrAsset)->classParameterC4;
  slotAssetId = &armyRuntime->movementTarget0Q12;
  while ((linkedArmyAssetId != *slotAssetId ||
         (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit)
          != 0))) {
    slotAssetId = slotAssetId + 1;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots - 1;
    if (remainingSlots == 0) {
      return true;
    }
  }
  modelNode = armyRuntime->modelNodeRuntime;
  createdArmy = ArmyRuntime_CreateInstanceFromAsset
                    (0,(modelNode->modelPayload).worldRotationAngle2,
                     (modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                     linkedArmyAssetId,worldRuntime,NULL);
  if (createdArmy == NULL) {
    return true;
  }
  childModelRuntime = createdArmy->modelRuntimeOrSavedOffset.modelRuntime;
  slotMaskState = &(armyRuntime->articulatedContact).linkedChildSlotMaskState;
  slotMaskState->linkedChildSlotMask = slotMaskState->linkedChildSlotMask | slotBit;
  armyRuntime->fallbackWorldYQ12 = armyRuntime->fallbackWorldYQ12 - 1;
  /* the aircraft's home pad, state 1 = parked (behaviorState), attack point and heading (classState70..78) */
  childModelRuntime->classLinkState.modelLinkOrState.modelRuntime = (ModelRuntimeSlot *)armyRuntime;
  childModelRuntime->classState.behaviorState = ARMY_AIRCRAFT_STATE_PARKED;
  modelNode = childModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  childModelRuntime->classLinkState.classState70 = inheritedValue70;
  childModelRuntime->classLinkState.classState74 = inheritedValue74;
  parentRootNode = armyRuntime->modelNodeRuntime;
  childModelRuntime->classLinkState.classState78 = inheritedValue78;
  (modelNode->childNodes[0]->modelPayload).localTranslationZQ12 =
       (parentRootNode->childNodes[0]->modelPayload).localTranslationZQ12;
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
  childModelRuntime->health =
       (int)(((int64_t)armyRuntime->actionVector2Q12 *
             (int64_t)(int)childModelRuntime->definitionOrSavedId.runtimeDefinition->maximumHealth)
            / (int64_t)(int)((ModelDefinition *)armyRuntime->definitionOrAsset)->maximumHealth);
  return false;
}


/* Returns true when the candidate model is within the combined radius (definition footprintRadiusCopy) of the
   source model and of every attached child model of the source (attachments, attachmentCount); false as soon as
   one of them is out of reach.
*/
Bool8 ArmyRuntime_TestModelAttachmentProximity(ModelRuntimeSlot *candidateModelRuntime,ModelRuntimeSlot *sourceModelRuntime)

{
  ModelDefinition *candidateDefinition;
  ModelRuntimeSlot *childModelRuntime;
  int remainingAttachments;
  Bool8 baseWithinRadius;
  Bool8 childWithinRadius;

  candidateDefinition = candidateModelRuntime->definitionOrSavedId.runtimeDefinition;
  remainingAttachments = sourceModelRuntime->attachmentCount;
  /* The candidate radius is candidateDefinition->footprintRadiusCopy. It is read through a pointer-typed field
     at the same offset (attachments[3].childModelRuntimeOrSavedOffset of a ModelRuntimeSlot view): a uint32_t
     read schedules the loads and the spill of the radius differently. */
  baseWithinRadius = ArmyRuntime_TestPositionDistanceWithinCombinedRadius
                    ((UQ12)((ModelRuntimeSlot *)candidateDefinition)->attachments[3].childModelRuntimeOrSavedOffset,
                     sourceModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadiusCopy,
                     candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode,
                     sourceModelRuntime->rootModelNodeOrSavedOffset.modelNode);
  if (!baseWithinRadius) {
    return false;
  }
  for (; remainingAttachments != 0; remainingAttachments--) {
    childModelRuntime = sourceModelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != NULL) {
      childWithinRadius = ArmyRuntime_TestPositionDistanceWithinCombinedRadius
                            ((UQ12)((ModelRuntimeSlot *)candidateDefinition)->attachments[3].
                                   childModelRuntimeOrSavedOffset,
                             childModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadiusCopy,
                             candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode,
                             childModelRuntime->rootModelNodeOrSavedOffset.modelNode);
      if (!childWithinRadius) {
        return false;
      }
    }
    /* the original advances the source pointer itself by one attachment descriptor (0x20), so
       attachments[0] walks the attachments */
    sourceModelRuntime =
         (ModelRuntimeSlot *)((uint8_t *)sourceModelRuntime + sizeof(ModelRuntimeAttachmentDescriptor));
  }
  return true;
}


/* Removes an army for good: destroys its model hierarchy, drops every reference to it (player selections, the
   world selection, owned-model links of other nodes, each player's primary selection, the faction group
   tables), frees its runtime slot (model node = NULL) and rebuilds the in-game catalog grids and the
   selection detail panel.
*/
void ArmyRuntime_DestroyInstanceAndRefreshUi(WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *modelRuntime;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  SelectionPlayerRuntimeBlock *playerSelectionBlock;

  modelRuntime = (ModelRuntimeSlot *)(entityRuntime->common).ownership.definitionOrClassRecord;
  if (modelRuntime != NULL) {
    (entityRuntime->common).ownership.definitionOrClassRecord = NULL;
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime);
  }
  SelectionPlayerBlocks_RemovePointer(entityRuntime);
  if (entityRuntime == (worldRuntime->selection).selectedEntity) {
    (worldRuntime->selection).selectedEntity = NULL;
  }
  WorldRuntime_ForEachOwnerListNode
            (entityRuntime,WorldRuntimeNode_ClearOwnedModelReferencesCallback,worldRuntime);
  /* drop it as each player's primary selection (the loop body runs at least once, as in the original) */
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    playerSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[playerBlockCursor->playerRuntimeId];
    if (entityRuntime == (GameEntityRuntime *)playerSelectionBlock->placedArmyToken) {
      playerSelectionBlock->placedArmyToken = 0;
    }
    playerBlockCursor++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(entityRuntime);
  (entityRuntime->common).ownership.modelNode = NULL; /* marks the army slot free */
  InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  InGameSelectionDetailPanel_Rebuild();
}


/* Group-A command check: returns true when the source model is of class 13 and the candidate model is
   within its radius + 0xC00 (0.75 in Q12) of the source model's anchor point (model lookup entry (1,5),
   transformed to world space), measured in x/y.
*/
Bool8 ArmyRuntime_TestArmyNearFactoryExit(ModelRuntimeSlot *candidateModelRuntime,ModelRuntimeSlot *sourceModelRuntime)

{
  uint32_t candidateRadius;
  ModelRuntimeNode *sourceNode;
  uint32_t anchorDistance;
  ModelPackedPointRecord *anchorRecord;
  ModelWorldPoint anchorPoint;
  ModelRuntimeNode *candidateNode;

  if (sourceModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_13) {
    return false;
  }
  /* the candidate definition's radius (footprintRadiusCopy) */
  candidateRadius = candidateModelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadiusCopy;
  sourceNode = sourceModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  candidateNode = candidateModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if (!ModelLookupTable_FindPackedPoint(1,5,(sourceNode->modelPayload).modelResource,&anchorRecord)) {
    return false;
  }
  anchorPoint = ModelNodeRuntime_TransformLocalPoint(anchorRecord,sourceNode);
  anchorDistance = FixedMath_Length2(anchorPoint.yQ12 - (candidateNode->worldTransform).translation.y,
                                     anchorPoint.xQ12 - (candidateNode->worldTransform).translation.x);
  return (int)anchorDistance <= (int)(candidateRadius + 3 * Q12_ONE / 4);
}


/* Air-raid alert (called directly by ArmyRuntimeClass_UpdateAircraft on every tick of an attack
   run): when the aircraft of factionIndex is hostile to the active faction and flies over a grid cell with bit
   0x10 in the active faction's byte of the cell's occupancyMask, the sound soundAssetIndex is played
   unpositioned at the effects gain, at most once per 16 ticks of the faction's relationTransitionTick.
*/
void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint(FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext)

{
  DirectSoundVoiceSet **voiceSetRef;
  FieldGridDimension gridWidthCells;
  uint32_t activeFactionIndex;
  int cellColumn;
  uint32_t projectedRow;
  int cellRow;
  Bool8 capabilityClear;
  FieldGridAsset *fieldGrid;

  if ((soundAssetIndex == 0) || (worldContext->dwordArray == NULL) ||
      (soundAssetIndex >= worldContext->dwordArrayCount)) {
    return;
  }
  voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[soundAssetIndex];
  if (voiceSetRef == NULL) {
    return;
  }
  fieldGrid = worldContext->fieldGrid;
  /* world point -> grid cell (fixed-point projection, rounded) */
  projectedRow = FIXED_MUL_SHR(worldYQ12,FIELD_GRID_WORLD_Y_TO_ROW_Q20,Q20_SHIFT + 1);
  gridWidthCells = fieldGrid->gridWidth;
  cellColumn = (int)((FIXED_MUL_SHR(worldXQ12,FIELD_GRID_WORLD_X_TO_COLUMN_Q20,Q20_SHIFT) - projectedRow) + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if (cellColumn < 0) {
    return;
  }
  cellRow = (int)(projectedRow * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((cellRow < 0) || (cellColumn >= (int)gridWidthCells) || (cellRow >= (int)fieldGrid->gridHeight)) {
    return;
  }
  activeFactionIndex = worldContext->activeFactionRuntimeIndex;
  if (factionIndex == activeFactionIndex) {
    return;
  }
  capabilityClear = GameFactionRuntime_TestCapabilityBitClear(activeFactionIndex,factionIndex);
  /* the active faction's byte of the cell's occupancy mask */
  if ((capabilityClear) &&
      ((((uint8_t *)&fieldGrid->cells[gridWidthCells * cellRow + cellColumn].occupancyMask)[activeFactionIndex] &
        ARMY_DEPTH_BIN_STRUCTURE_BIT) != 0) &&
      (16 < g_GameFactionRuntimeImage.records[activeFactionIndex].relationTransitionTick)) {
    g_GameFactionRuntimeImage.records[activeFactionIndex].relationTransitionTick = 0;
    g_SoundPlayOneShot(g_SoundEffectsGainQ15,g_SoundEffectsGainQ15,*voiceSetRef,NULL);
  }
}


/* Xenite refund for the not yet launched linked assets of a class-22 pad that is being dismantled (called
   directly by ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive): sums xeniteValueQ4 of the
   faction's model definition of every linked asset whose slot bit is still clear, divided by 32 (the same
   rate as the pad's own refund).
*/
uint32_t ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric(ArmyRuntimeLinkedChildMaskSlotView *armyRuntime)

{
  FactionRuntimeIndex factionIndex;
  int remainingSlots;
  uint32_t metricSum;
  uint32_t slotBit;
  int slotIndex;
  Q12 *linkedAssetIds;
  ArmyAssetRecordPrefix *assetRecord;
  ModelDefinitionRecordPrefix *selectedDefinition;

  metricSum = 0;
  factionIndex = (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex;
  slotBit = 1;
  slotIndex = 0;
  remainingSlots = ((ModelDefinition *)armyRuntime->definitionOrAsset)->classParameterC4;
  /* the linked asset ids are consecutive dwords starting at movementTarget0Q12 */
  linkedAssetIds = &armyRuntime->movementTarget0Q12;
  do {
    if (((armyRuntime->articulatedContact).linkedChildSlotMaskState.linkedChildSlotMask & slotBit) == 0) {
      if (ArmyAssetRegistry_FindById(linkedAssetIds[slotIndex],&assetRecord) == 0) {
        selectedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                          (factionIndex,assetRecord->rootNodeOffsetOrPointer);
        metricSum = metricSum + ((ModelDefinition *)selectedDefinition)->xeniteValueQ4;
      }
    }
    slotIndex++;
    slotBit = slotBit * 2;
    remainingSlots = remainingSlots - 1;
  } while (remainingSlots != 0);
  return metricSum >> 5;
}


/* Plays the one-shot sound secondarySoundIndex of the model's definition at the model's position, with the
   definition's range and gain (positionedSoundMaximumDistanceQ12/positionedSoundGainQ15), where the active
   faction's cell bits 0/1 are set. Called directly by ArmyRuntimeClass_UpdateAircraft for the home pad when an
   aircraft lands or takes off (the pad's platform sound).
*/
void ModelRuntime_PlayDefinitionSecondaryOneShotSound(ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime)

{
  ModelDefinition *definition;
  ModelRuntimeNode *rootNode;
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **voiceSetRef;
  Bool8 cellMasked;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  rootNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  soundAssetIndex = definition->secondarySoundIndex;
  if ((soundAssetIndex == 0) || (soundAssetIndex >= worldRuntime->dwordArrayCount) ||
      (worldRuntime->dwordArray == NULL)) {
    return;
  }
  voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundAssetIndex];
  if (voiceSetRef == NULL) {
    return;
  }
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,worldRuntime);
  if (!cellMasked) {
    SpatialSound_PlayPositionedOneShot
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,
               &(rootNode->worldTransform).translation,voiceSetRef);
  }
}


/* Plays the one-shot sound primarySoundIndex of the model's definition at the model's position, with the
   definition's range and gain (positionedSoundMaximumDistanceQ12/positionedSoundGainQ15), where the active
   faction's cell bits 0/1 are set. Called directly by ArmyRuntimeClass_UpdateAircraft when a returning aircraft
   opens its home pad. Despite the name no effect is spawned; it is the hatch sound class 22 plays itself in
   ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode.
*/
void ModelRuntime_PlayDefinitionPrimaryOneShotSound(ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldContext)

{
  ModelDefinition *definition;
  ModelRuntimeNode *rootNode;
  uint32_t soundAssetIndex;
  DirectSoundVoiceSet **voiceSetRef;
  Bool8 cellMasked;

  definition = modelRuntime->definitionOrSavedId.runtimeDefinition;
  rootNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  soundAssetIndex = definition->primarySoundIndex;
  if ((soundAssetIndex == 0) || (soundAssetIndex >= worldContext->dwordArrayCount) ||
      (worldContext->dwordArray == NULL)) {
    return;
  }
  voiceSetRef = (DirectSoundVoiceSet **)worldContext->dwordArray[soundAssetIndex];
  if (voiceSetRef == NULL) {
    return;
  }
  cellMasked = TerrainGrid_TestProjectedCellMaskBits01
                    ((rootNode->worldTransform).translation.y,(rootNode->worldTransform).translation.x,worldContext);
  if (!cellMasked) {
    SpatialSound_PlayPositionedOneShot
              (definition->positionedSoundMaximumDistanceQ12,definition->positionedSoundGainQ15,
               &(rootNode->worldTransform).translation,voiceSetRef);
  }
}


/* Tests whether two model nodes are closer in the XY plane than the sum of their radii (collision/contact test
   of two armies or attachments): returns false when dx^2 + dy^2 <= (candidateRadius + sourceRadius)^2, true
   otherwise, in 64-bit Q24 arithmetic.
*/
Bool8 ArmyRuntime_TestPositionDistanceWithinCombinedRadius
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidateModelNode,
          void *sourceModelNode)

{
  int axisDeltaXQ12;
  int axisDeltaYQ12;
  int64_t remainingRadiusSquaredAfterXQ24;
  int64_t yDistanceSquaredQ24;

  axisDeltaXQ12 =
       ((ModelRuntimeNode *)sourceModelNode)->worldTransform.translation.x -
       ((ModelRuntimeNode *)candidateModelNode)->worldTransform.translation.x;
  remainingRadiusSquaredAfterXQ24 =
       (int64_t)(int)(sourceRadiusQ12 + candidateRadiusQ12) *
       (int64_t)(int)(sourceRadiusQ12 + candidateRadiusQ12) -
       (int64_t)axisDeltaXQ12 * (int64_t)axisDeltaXQ12;
  if (remainingRadiusSquaredAfterXQ24 < 0) {
    return true;
  }
  axisDeltaYQ12 = ((ModelRuntimeNode *)sourceModelNode)->worldTransform.translation.y -
                  ((ModelRuntimeNode *)candidateModelNode)->worldTransform.translation.y;
  yDistanceSquaredQ24 = (int64_t)axisDeltaYQ12 * (int64_t)axisDeltaYQ12;
  /* the original tests the sign of the high dword of remainder - dy^2 (high dwords minus the borrow); both
     values are below 2^62 in magnitude, so this is a plain 64-bit comparison */
  if (remainingRadiusSquaredAfterXQ24 >= yDistanceSquaredQ24) {
    return false;
  }
  return true;
}


/* Bomb release of an aircraft on its attack run (called directly by ArmyRuntimeClass_UpdateAircraft):
   scores every intact model (stateFlags bit 8 clear) of another, non-neutral faction with AiCombatTarget_EvaluateCandidateScore. If the
   best one is within 1.0 (Q12) of its radius from the given point, every shot aims at it; otherwise each shot
   aims at the given point shifted by its launch point's offset from the aircraft. One shot of
   effectDefinitionId (despite the name a shot definition) is created from every launch point with packed key
   modelPointOrdinal << 4 | 2.
*/
void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  ArmyRuntimeSlot *candidateArmyRuntime;
  ModelRuntimeSlot *candidateModelRuntime;
  Q12 candidateWorldZ;
  uint32_t candidateScore;
  uint32_t candidateDistance;
  int deltaY;
  uint32_t currentBestScore;
  int deltaX;
  int candidateRadius;
  ModelResource *spriteModelResource;
  int remainingEntries;
  WorldOwnerListNode *nodeCursor;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint localPoint;
  WorldOwnerListNode *bestCandidateNode;
  uint32_t pointOffsetMask;

  bestCandidateNode = NULL;
  nodeCursor = worldContext->ownerListHead;
  pointOffsetMask = UINT32_MAX;
  currentBestScore = 0;
  sourceArmyRuntime =
       ((ModelRuntimeNode *)sourceRuntime)->runtimePayload.modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  do {
    if ((nodeCursor->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (nodeCursor != sourceRuntime)) {
      candidateModelRuntime = (ModelRuntimeSlot *)nodeCursor->runtimePayload;
      candidateArmyRuntime = candidateModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if (((candidateModelRuntime->classState.stateFlags & ARMY_RUNTIME_FLAG_DESTROYED) == 0) &&
          (candidateArmyRuntime->factionIndex != 0) &&
          (candidateArmyRuntime->factionIndex != sourceArmyRuntime->factionIndex)) {
        sourceArmyRuntime->weaponRangeQ12 =
             (((candidateArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionOrSavedId).
             runtimeDefinition->footprintRadius;
        candidateScore = AiCombatTarget_EvaluateCandidateScore
                          (currentBestScore,1,UINT32_MAX,UINT32_MAX,candidateArmyRuntime,
                           sourceArmyRuntime);
        if (currentBestScore < candidateScore) {
          currentBestScore = candidateScore;
          bestCandidateNode = nodeCursor;
        }
      }
    }
    nodeCursor = nodeCursor->nextNode;
  } while (nodeCursor != NULL);
  if (bestCandidateNode != NULL) {
    candidateWorldZ = bestCandidateNode->worldZQ12;
    deltaX = bestCandidateNode->worldXQ12 - worldXQ12;
    deltaY = bestCandidateNode->worldYQ12 - worldYQ12;
    candidateRadius =
         ((ModelRuntimeSlot *)bestCandidateNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->
         footprintRadius;
    candidateDistance = FixedMath_Length2(deltaY,deltaX);
    if ((int)(candidateDistance - candidateRadius) < Q12_ONE + 1) {
      pointOffsetMask = 0;
      worldXQ12 = deltaX + worldXQ12;
      worldYQ12 = deltaY + worldYQ12;
      worldZQ12 = candidateWorldZ;
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)sourceRuntime);
  spriteModelResource = ((MdlSerializedNodeHeader *)modelPointTable)->spriteAssetReference.modelResource;
  localPointRecord =
       (ModelPackedPointRecord *)
       ((uint8_t *)spriteModelResource + spriteModelResource->packedLookupTableRelativeOffset);
  for (remainingEntries = spriteModelResource->packedLookupTableEntryCount;
      remainingEntries != 0; remainingEntries = remainingEntries - 1) {
    if (localPointRecord->packedLookupKey == (modelPointOrdinal << 4 | 2)) {
      localPoint = ModelNodeRuntime_TransformLocalPoint(localPointRecord,(ModelRuntimeNode *)sourceRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (effectFlags,
                 ((ModelRuntimeNode *)sourceRuntime)->runtimePayload.modelRuntime->ownerArmyRuntimeOrSavedOffset.
                 armyRuntime,worldZQ12,
                 (localPoint.yQ12 - ((ModelRuntimeNode *)sourceRuntime)->worldTransform.translation.y &
                 pointOffsetMask) + worldYQ12,
                 (localPoint.xQ12 - ((ModelRuntimeNode *)sourceRuntime)->worldTransform.translation.x &
                 pointOffsetMask) + worldXQ12,
                 localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,(ShotDefinition *)effectDefinitionId,worldContext);
    }
    localPointRecord = localPointRecord + 1;
  }
  return;
}


/* One tick of a model whose health is gone (called directly by
   ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive, which then counts the eight channel timers
   destructionEffectTimers down). For every channel i whose timer is 0 it spawns the channel's
   effect (definition (&destructionEffect0)[2 * i]) at every model point with packed key i << 4 | 3 of the root
   model, using the root's orientation, and, when the definition has a child model, at those of child node 0 with
   a fixed orientation. Also sets its own health to 0 and clears health and link (linkedModelRuntimeOrSavedOffset)
   of every attached child model.
*/
void ArmyRuntime_ProcessReadyAttachmentChannels(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeSlot *childModelRuntime;
  uint32_t channelIndex;
  uint32_t remainingAttachments;
  int remainingRecords;
  ModelResource *rootModelResource;
  ModelResource *childModelResource;
  MdlSerializedNodeHeader *rootNodeHeader;
  MdlSerializedNodeHeader *childNodeHeader;
  ModelPackedPointRecord *pointRecord;
  ModelWorldPoint localPoint;
  ModelRuntimeNode *modelNode;

  /* root model: the effects use the root node's orientation */
  rootModelResource =
       (ModelResource *)((MdlSerializedNodeHeader *)
                         (modelRuntime->definitionOrSavedId).runtimeDefinition->rootNodeOffsetOrPointer)->
       spriteAssetReference.modelResource;
  modelRuntime->health = 0;
  for (channelIndex = 0; channelIndex < 8; channelIndex = channelIndex + 1) {
    if (modelRuntime->destructionEffectTimers[channelIndex] != 0) {
      continue;
    }
    pointRecord =
         (ModelPackedPointRecord *)
         ((uint8_t *)rootModelResource + rootModelResource->packedLookupTableRelativeOffset);
    for (remainingRecords = rootModelResource->packedLookupTableEntryCount; remainingRecords != 0;
        remainingRecords = remainingRecords - 1) {
      if (channelIndex * 16 + 3 == pointRecord->packedLookupKey) {
        localPoint = ModelNodeRuntime_TransformLocalPoint
                          (pointRecord,(modelRuntime->rootModelNodeOrSavedOffset).modelNode);
        modelNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                   *(EffectRuntimeOwnerReference *)
                    &modelRuntime->linkedModelRuntimeOrSavedOffset,
                   (modelNode->modelPayload).worldRotationAngle2,
                   (modelNode->modelPayload).worldRotationAngle1,
                   (modelNode->modelPayload).worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,
                   /* the definition's eight {effect, value} pairs from destructionEffect0 */
                   (&(modelRuntime->definitionOrSavedId).runtimeDefinition->destructionEffect0)
                   [channelIndex * 2].definition,worldRuntime);
      }
      pointRecord = pointRecord + 1;
    }
  }
  /* child model 0 (when its node flags' low nibble is 0): the effects use a fixed orientation */
  rootNodeHeader =
       (MdlSerializedNodeHeader *)(modelRuntime->definitionOrSavedId).runtimeDefinition->rootNodeOffsetOrPointer;
  if (rootNodeHeader->childCount != 0) {
    childNodeHeader = (MdlSerializedNodeHeader *)rootNodeHeader->childSerializedOffsets[0];
    childModelResource = (ModelResource *)childNodeHeader->spriteAssetReference.modelResource;
    if ((childNodeHeader->nodeFlags & 0xf) == 0) {
      for (channelIndex = 0; channelIndex < 8; channelIndex = channelIndex + 1) {
        if (modelRuntime->destructionEffectTimers[channelIndex] != 0) {
          continue;
        }
        pointRecord =
             (ModelPackedPointRecord *)
             ((uint8_t *)childModelResource + childModelResource->packedLookupTableRelativeOffset);
        for (remainingRecords = childModelResource->packedLookupTableEntryCount; remainingRecords != 0;
            remainingRecords = remainingRecords - 1) {
          if ((channelIndex * 16 + 3 == pointRecord->packedLookupKey) &&
              (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childCount != 0)) {
            modelNode = ((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->childNodes[0];
            if (modelNode != NULL) {
              localPoint = ModelNodeRuntime_TransformLocalPoint(pointRecord,modelNode);
              EffectRuntimePool_CreateInstanceFromDefinition
                        (EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY,
                         *(EffectRuntimeOwnerReference *)
                          &modelRuntime->linkedModelRuntimeOrSavedOffset,0,FIXED_ANGLE16_QUARTER_TURN,0,localPoint.zQ12,
                         localPoint.yQ12,localPoint.xQ12,
                         (&(modelRuntime->definitionOrSavedId).runtimeDefinition->destructionEffect0)
                         [channelIndex * 2].definition,worldRuntime);
            }
          }
          pointRecord = pointRecord + 1;
        }
      }
    }
  }
  /* modelRuntime advances by one 0x20-byte attachment record per iteration */
  for (remainingAttachments = modelRuntime->attachmentCount; remainingAttachments != 0; remainingAttachments = remainingAttachments - 1) {
    childModelRuntime = modelRuntime->attachments[0].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != NULL) {
      childModelRuntime->health = 0;
      (childModelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = NULL;
    }
    modelRuntime = (ModelRuntimeSlot *)((uint8_t *)modelRuntime + sizeof(ModelRuntimeAttachmentDescriptor));
  }
  return;
}


/* Runs the sound callback classMethodD[class] of g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes for a model
   runtime and recursively for its attached child models (called by
   ArmyRuntimeMaintenance_DispatchClassMethodDRecursive, the audioRefresh entry of
   g_RuntimeMaintenanceCallbackPhases).
*/
void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  uint32_t childCount;
  uint32_t childIndex;
  ModelRuntimeSlot *childModelRuntime;

  /* the attachment count is read before the class callback runs */
  childCount = modelRuntime->attachmentCount;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD
    [modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId])
            (worldRuntime,modelRuntime);
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    childModelRuntime = modelRuntime->attachments[childIndex].childModelRuntimeOrSavedOffset;
    if (childModelRuntime != NULL) {
      ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(worldRuntime,childModelRuntime);
    }
  }
}


/* Recomputes the army's derived combat figures shown on selection from its model hierarchy (after creation or
   a change of attachments): the maxima occupancyMarkRadius, visibilityRadius, visibilityHeightOffset and the
   shot selection range weaponRangeQ12 are cleared, as are the eight per-target-class damage sums
   targetClassShotDamage, then
   ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics adds up every node of the model.
*/
void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime)

{
  int targetClassIndex;

  armyRuntime->occupancyMarkRadius = 0;
  armyRuntime->visibilityRadius = 0;
  armyRuntime->visibilityHeightOffset = 0;
  armyRuntime->weaponRangeQ12 = 0;
  for (targetClassIndex = 7; targetClassIndex >= 0; targetClassIndex--) {
    armyRuntime->targetClassShotDamage[targetClassIndex] = 0;
  }
  if ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime != NULL) {
    ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics
              ((int *)(armyRuntime->modelRuntimeOrSavedOffset).modelRuntime);
  }
}


/* Stub of a world point test (called directly by the aircraft and pad updates, slots 21 and 22): always
   returns false, so the callers' `!result` branches are always taken.
*/
Bool8 ArmyRuntime_TestWorldPointAllowedDefault(uint32_t allowedContext,uint32_t worldYQ12,uint32_t worldXQ12)

{
  return false;
}


/* The first free army slot (model node NULL), or NULL when there is no army pool or no free slot. */
static ArmyRuntimeSlot *ArmyRuntimePool_FindFreeSlot(void)

{
  ArmyRuntimeSlot *armyRuntime;
  uint32_t armySlotsRemaining;

  armyRuntime = g_ArmyRuntimeSlots;
  if (armyRuntime == NULL) {
    return NULL;
  }
  for (armySlotsRemaining = ARMY_RUNTIME_SLOT_COUNT; armySlotsRemaining != 0; armySlotsRemaining--) {
    if (armyRuntime->modelNodeRuntime == NULL) {
      return armyRuntime;
    }
    armyRuntime++;
  }
  return NULL;
}


/* The registered army asset record with this id, or NULL when there is none. */
static ArmyAssetRecordPrefix *ArmyAssetRegistry_FindRecordById(PckArmyAssetIdCatalog armyAssetId)

{
  ArmyAssetRecordPrefix **registryCursor;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix *armyAssetRecord;

  registryCursor = g_ArmyAssetRecordRegistry;
  for (registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    armyAssetRecord = *registryCursor;
    if ((armyAssetRecord != NULL) && (armyAssetRecord->registryId == armyAssetId)) {
      return armyAssetRecord;
    }
    registryCursor++;
  }
  return NULL;
}


/* Stores the error in *outError (when outError is not NULL) and returns the failure result NULL. */
static ArmyRuntimeSlot *ArmyRuntime_FailCreateInstance(uint32_t error,uint32_t *outError)

{
  if (outError != NULL) {
    *outError = error;
  }
  return NULL;
}


/* Creates an army (unit or building) of an army asset for a faction at a world point: takes the first free
   army slot, creates the faction's model (and its linked child models) with the faction's army graphics, links
   it into the world, places it on the terrain and initialises occupancy, tint and selection metrics. Returns
   the army slot (never NULL), or NULL on failure with the error in *outError (when outError is not NULL):
   FATAL_ERROR_GENERAL_FAILURE (no army pool or no free slot), FATAL_ERROR_ARMY_ID_NOT_FOUND (the id is left in
   g_PackageLastErrorPath) or the model creation error.
   Original quirk: when creating the linked child models fails, the error is the value of worldYQ12 (a stale
   value the original never replaced by an error code); kept.
*/
ArmyRuntimeSlot *ArmyRuntime_CreateInstanceFromAsset
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyAssetRecordPrefix *armyAssetRecord;
  ModelDefinitionRecordPrefix *selectedDefinition;
  GraphicsTextureSet *textureSet;
  GraphicsPaletteAsset *paletteAsset;
  uint32_t anchorScoreWeight;
  PckArmyAssetIdCatalog secondaryWorkspaceScoreWeight;
  GameEntityRuntime *linkedEntity;
  uint32_t rootNodeReference;
  PckModelDefinitionIdCatalog modelDefinitionId;
  uint32_t modelCreateError;
  ModelRuntimeSlot *createdModelRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  Bool8 childCreateFailed;
  ModelDefinition *definition;

  armyRuntime = ArmyRuntimePool_FindFreeSlot();
  if (armyRuntime == NULL) {
    return ArmyRuntime_FailCreateInstance(FATAL_ERROR_GENERAL_FAILURE,outError);
  }
  armyAssetRecord = ArmyAssetRegistry_FindRecordById(armyAssetId);
  if (armyAssetRecord == NULL) {
    /* the asset id as decimal text (base 10, at least one digit) for the error message */
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armyAssetId,g_PackageLastErrorPath);
    return ArmyRuntime_FailCreateInstance(FATAL_ERROR_ARMY_ID_NOT_FOUND,outError);
  }
  armyRuntime->armyAssetId = armyAssetId;
  if (((creationFlags & ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION) != 0) &&
      (factionIndex == worldRuntime->activeFactionRuntimeIndex)) {
    selectedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                      (factionIndex,armyAssetRecord->rootNodeOffsetOrPointer);
    ((ModelDefinition *)selectedDefinition)->builtCount++;
  }
  /* the graphics bindings exist for faction slots 0-7 only */
  if (7 < (uint32_t)factionIndex) {
    factionIndex = 7;
  }
  armyRuntime->factionIndex = factionIndex;
  if ((creationFlags & ARMY_CREATE_UNLOCK_TECHNOLOGY) != 0) {
    ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
              (factionIndex,(ModelDefinitionHierarchyNodeAddress32)armyAssetRecord);
  }
  textureSet = g_ArmyGraphicsBindings[factionIndex].textureSet;
  paletteAsset = g_ArmyGraphicsBindings[factionIndex].paletteAsset;
  anchorScoreWeight = armyAssetRecord[7].selectionDetailTemplateVariantIndex;
  secondaryWorkspaceScoreWeight = armyAssetRecord[7].registryId;
  armyRuntime->aiSiteScoreWeight = armyAssetRecord[7].byteSize;
  armyRuntime->aiFactionAnchorScoreWeight = anchorScoreWeight;
  armyRuntime->aiSecondaryWorkspaceScoreWeight = secondaryWorkspaceScoreWeight;
  linkedEntity = (GameEntityRuntime *)armyAssetRecord[1].byteSize;
  armyRuntime->occupancyMarkRadius = 0;
  armyRuntime->visibilityRadius = 0;
  armyRuntime->visibilityHeightOffset = 0;
  armyRuntime->aiUnitFlags = 0;
  rootNodeReference = armyAssetRecord->rootNodeOffsetOrPointer;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
  armyRuntime->linkedEntityRuntime = linkedEntity;
  (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = NULL;
  armyRuntime->aiUnitState = 0;
  modelDefinitionId = ModelDefinition_SelectFactionUnlockedLinkedId(factionIndex,rootNodeReference);
  modelCreateError = ModelRuntimePool_CreateInstanceByDefinitionId
                     (paletteAsset,textureSet,armyRuntime,modelDefinitionId,worldRuntime,
                      &createdModelRuntime);
  if (modelCreateError != 0) {
    return ArmyRuntime_FailCreateInstance(modelCreateError,outError);
  }
  modelNodeRuntime = createdModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = createdModelRuntime;
  armyRuntime->modelNodeRuntime = modelNodeRuntime;
  /* worldYQ12 goes to translation.x and worldXQ12 to translation.y throughout, as in the original; the
     parameter names are swapped relative to the node fields */
  (modelNodeRuntime->worldTransform).translation.x = worldYQ12;
  (modelNodeRuntime->worldTransform).translation.y = worldXQ12;
  (modelNodeRuntime->worldTransform).translation.z = 0;
  armyRuntime->movementRetryCountdown = 0;
  armyRuntime->fallbackWorldYQ12 = worldYQ12;
  armyRuntime->fallbackWorldXQ12 = worldXQ12;
  armyRuntime->movementTarget0Q12 = worldYQ12;
  armyRuntime->movementTarget1Q12 = worldXQ12;
  (modelNodeRuntime->modelPayload).worldRotationAngle0 = 0;
  (modelNodeRuntime->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 = orientationAngle;
  armyRuntime->commandTargetArmyRuntime = NULL;
  armyRuntime->assignedTargetArmyRuntime = 0;
  armyRuntime->commandCoordinate0Q12 = 0;
  armyRuntime->commandCoordinate1Q12 = 0;
  armyRuntime->commandCoordinate2Q12 = 0;
  armyRuntime->commandModeFlags = 0;
  armyRuntime->commandGeneration = 0;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
  (armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.coordinateOrTargetQ12 = worldYQ12;
  (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.coordinateOrTargetQ12 = worldXQ12;
  armyRuntime->movementPosition0Q12 = worldYQ12;
  armyRuntime->movementPosition1Q12 = worldXQ12;
  armyRuntime->movementStateFlags = 0;
  armyRuntime->actionVector1Q12 = 0;
  armyRuntime->terrainOccupancyMask0 = 0;
  armyRuntime->terrainOccupancyMask1 = 0;
  armyRuntime->runtimeState40 = 0;
  childCreateFailed = ModelNodeRuntime_InstantiateLinkedChildrenRecursive
                    (factionIndex,paletteAsset,textureSet,
                     (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime,rootNodeReference,worldRuntime);
  if (childCreateFailed) {
    /* Original quirk: the error is worldYQ12 (see above). */
    return ArmyRuntime_FailCreateInstance((uint32_t)worldYQ12,outError);
  }
  WorldRuntime_LinkOwnerListNode((WorldOwnerListNode *)modelNodeRuntime);
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
  /* terrain contact by the definition's contact kind; depth class by its model class; depth radius from the
     definition */
  definition = (ModelDefinition *)
               (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime->definitionOrSavedId.runtimeDefinition;
  g_ArmyPlacementContactKindDispatchTable.callbacks[definition->placementContactKindIndex]
            (definition->placementHeightOffsetQ12,(modelNodeRuntime->worldTransform).translation.y,
             (modelNodeRuntime->worldTransform).translation.x,modelNodeRuntime,worldRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  armyRuntime->depthBinClass =
       (ModelRuntimeClassId)g_ArmyRuntimeDepthBinClassByModelClass[definition->runtimeClassId];
  ModelNodeRuntime_UpdateDepthBinMasks(definition->footprintRadius,modelNodeRuntime);
  ArmyRuntime_InitializeTerrainOccupancyFlags(worldRuntime,armyRuntime);
  ModelNodeRuntime_RefreshStateTint(modelNodeRuntime);
  ArmyRuntime_RebuildDerivedSelectionMetrics(armyRuntime);
  return armyRuntime;
}


/* Sets up the terrain occupancy of a newly placed army: classifies the field-grid neighbourhood of its model
   node (within the model definition's footprintRadius), lets TerrainOccupancyMask_ResolveRuntimeClassFlags
   derive the two occupancy masks and the node's occupancy flags (0x4, 0x8, 0x1000) from it, and forces node flag 0x1000
   when the model runtime has flag 0x200 set in classState.stateFlags.
*/
void ArmyRuntime_InitializeTerrainOccupancyFlags
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint32_t occupancyRuntimeFlags;
  TerrainOccupancyResolvedMasks resolvedMasks;
  ModelRuntimeNode *modelNode;
  ModelRuntimeSlot *modelRuntime;

  armyRuntime->terrainOccupancyMask0 =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionOrSavedId).runtimeDefinition->
                  footprintRadius,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                  worldRuntime->fieldGrid);
  modelNode = armyRuntime->modelNodeRuntime;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNode->runtimeFlags,armyRuntime->terrainOccupancyMask1,
                     armyRuntime->terrainOccupancyMask0,
                     (char)worldRuntime->activeFactionRuntimeIndex);
  occupancyRuntimeFlags = resolvedMasks.runtimeFlags;
  /* replace node flags 0x4, 0x8 and 0x1000 by the resolved ones */
  modelNode->runtimeFlags = modelNode->runtimeFlags & ~(MODEL_NODE_FLAG_FORCE_TRANSPARENT | TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE | TERRAIN_OCCUPANCY_FLAG_PRESENT);
  armyRuntime->terrainOccupancyMask0 = resolvedMasks.primaryOccupancyMask;
  armyRuntime->terrainOccupancyMask1 = resolvedMasks.secondaryOccupancyMask;
  modelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  modelNode->runtimeFlags = modelNode->runtimeFlags | occupancyRuntimeFlags;
  if ((modelRuntime->classState.stateFlags & ARMY_MODEL_STATE_DISMANTLED) != 0) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | MODEL_NODE_FLAG_FORCE_TRANSPARENT;
  }
  return;
}


/* Idle animation of a building model: child nodes 0 and 1 spin (localRotationAngle2 += definition
   animatedChild0RotationStep / animatedChild1RotationStep per tick), child node 2 bobs in Z by
   animatedChild2BobStep per tick between runtimeValue24 and runtimeValue28, reversing at the
   limits (stateFlags bit 2 = direction). While bit 0 is set the animation stands still; bit 4 follows bit 0,
   and each change of it rebuilds the owner's selection metrics. Reached through
   g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[10] and directly from the class updates 11,
   13, 14, 22 and gameplay/army/combat.
*/
void ArmyRuntime_UpdateAnimatedModelSubnodes(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  uint32_t *classStateField;
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinition *animationDefinition;
  Q12 updatedChildTranslationZQ12;
  int bobStep;
  ModelRuntimeNode *animatedChildNode;
  int childTranslationZQ12;
  ModelRuntimeNode *animatedNode;
  
  if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_SWITCHED_OFF) == 0) {
    if (((modelRuntime->classState).stateFlags & 4) != 0) {
      classStateField = &(modelRuntime->classState).stateFlags;
      *classStateField = *classStateField ^ 4;
      ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
    }
    modelNodeRuntime = modelRuntime->rootModelNode;
    animationDefinition = modelRuntime->modelDefinition;
    animatedChildNode = modelNodeRuntime->childNodes[0];
    if (modelNodeRuntime->childCount != 0) {
      if (animatedChildNode != NULL) {
        (animatedChildNode->modelPayload).localRotationAngle2 =
             animationDefinition->animatedChild0RotationStep * g_InGameSimulationStepTicks +
             (animatedChildNode->modelPayload).localRotationAngle2 & FIXED_ANGLE16_MASK;
        animatedChildNode->runtimeFlags = animatedChildNode->runtimeFlags | 1;
      }
      animatedNode = modelNodeRuntime->childNodes[1];
      if (1 < modelNodeRuntime->childCount) {
        if (animatedNode != NULL) {
          (animatedNode->modelPayload).localRotationAngle2 =
               animationDefinition->animatedChild1RotationStep * g_InGameSimulationStepTicks +
               (animatedNode->modelPayload).localRotationAngle2 & FIXED_ANGLE16_MASK;
          animatedNode->runtimeFlags = animatedNode->runtimeFlags | 1;
        }
        animatedNode = modelNodeRuntime->childNodes[2];
        if ((2 < modelNodeRuntime->childCount) && (animatedNode != NULL)) {
          childTranslationZQ12 = (animatedNode->modelPayload).localTranslationZQ12;
          bobStep = animationDefinition->animatedChild2BobStep * g_InGameSimulationStepTicks;
          if (((modelRuntime->classState).stateFlags & 2) == 0) {
            updatedChildTranslationZQ12 = childTranslationZQ12 - bobStep;
            if (updatedChildTranslationZQ12 < (int)animationDefinition->runtimeValue24) {
              updatedChildTranslationZQ12 = animationDefinition->runtimeValue24;
              classStateField = &(modelRuntime->classState).stateFlags;
              *classStateField = *classStateField ^ 2;
            }
          }
          else {
            updatedChildTranslationZQ12 = childTranslationZQ12 + bobStep;
            if ((int)animationDefinition->runtimeValue28 < updatedChildTranslationZQ12) {
              updatedChildTranslationZQ12 = animationDefinition->runtimeValue28;
              classStateField = &(modelRuntime->classState).stateFlags;
              *classStateField = *classStateField ^ 2;
            }
          }
          (animatedNode->modelPayload).localTranslationZQ12 = updatedChildTranslationZQ12;
          animatedNode->runtimeFlags = animatedNode->runtimeFlags | 1;
        }
      }
    }
    ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  }
  else if (((modelRuntime->classState).stateFlags & 4) == 0) {
    classStateField = &(modelRuntime->classState).stateFlags;
    *classStateField = *classStateField ^ 4;
    ArmyRuntime_RebuildDerivedSelectionMetrics(modelRuntime->ownerArmyRuntime);
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Picks the model point of the timed effect emitter: the model (the first child for aircraft) has effect points
   with packed keys n << 4 | 6; one of them is chosen in turn (definition modelFlags bit 0) or at random and
   transformed to world space. Returns false when the model has no such point (the caller then uses the root
   position). */
static Bool8 ArmyEmitter_FindEffectPoint(ModelRuntimeUpdateView *modelRuntime,ModelDefinition *emitterDefinition,
          ModelWorldPoint *outWorldPoint)
{
  MdlSerializedNodeHeader *serializedNode;
  ModelResource *modelResource;
  int remainingRecords;
  uint32_t *pointRecordCursor;
  uint32_t pointCount;
  uint32_t pointSelector;
  ModelPackedPointRecord *emitterPoint;
  ModelRuntimeNode *modelNode;

  serializedNode = (MdlSerializedNodeHeader *)emitterDefinition->rootNodeOffsetOrPointer;
  if (emitterDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    serializedNode = (MdlSerializedNodeHeader *)serializedNode->childSerializedOffsets[0];
  }
  modelResource = (ModelResource *)serializedNode->spriteAssetReference.modelResource;
  remainingRecords = modelResource->packedLookupTableEntryCount;
  if (remainingRecords == 0) {
    return false;
  }
  pointCount = 0;
  pointRecordCursor = (uint32_t *)((uint8_t *)modelResource + modelResource->packedLookupTableRelativeOffset);
  /* count the effect points: highest n + 1 of the packed keys n << 4 | 6 */
  for (; remainingRecords != 0; remainingRecords = remainingRecords - 1, pointRecordCursor = pointRecordCursor + 4) {
    if (((*pointRecordCursor & 0xf) == 6) && (pointCount <= *pointRecordCursor >> 4)) {
      pointCount = (*pointRecordCursor >> 4) + 1;
    }
  }
  if (pointCount == 0) {
    return false;
  }
  pointSelector = (modelRuntime->classState).effectEmitterPointIndex;
  if ((emitterDefinition->modelFlags & 1) == 0) {
    pointSelector = g_RandomGeneratorState.next();
  }
  if (!ModelLookupTable_FindPackedPoint
            (pointSelector % pointCount,6,serializedNode->spriteAssetReference.modelResource,&emitterPoint)) {
    return false;
  }
  modelNode = modelRuntime->rootModelNode;
  if (emitterDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_21_AIRCRAFT) {
    modelNode = modelNode->childNodes[0];
  }
  *outWorldPoint = ModelNodeRuntime_TransformLocalPoint(emitterPoint,modelNode);
  return true;
}

/* Timed emitters of an army model, reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes
   .runtimeUpdate[0] and [16] and directly from most class updates (here, gameplay/army/combat and movement).
   Timer classState.shotEmitterTimerTicks: fires the definition's shot (emitterShotDefinitionReference) straight
   ahead (1.0 along the root's orientation) and restarts at shotEmitterIntervalTicks plus a random part below
   shotEmitterRandomTicks. Timer classState.effectEmitterTimerTicks: spawns the land
   (emitterEffectDefinitionReference) or, where FieldGrid_GetNearestWaterDelta is positive, the water
   (waterEmitterEffectDefinitionReference) effect at a model point with key n << 4 | 6 (random, or in turn when
   definition modelFlags bit 0 is set; the root position when there is none) and restarts at
   effectEmitterIntervalTicks plus a random part below effectEmitterRandomTicks.
*/
void ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  ModelDefinition *emitterDefinition;
  int previousTimerTicks;
  uint32_t randomValue;
  uint32_t randomTicks;
  ModelRuntimeNode *modelNode;
  FixedDirection launchDirection;
  GraphicsWorldCoordinateQ12 launchWorldZQ12;
  GraphicsWorldCoordinateQ12 launchWorldYQ12;
  GraphicsWorldCoordinateQ12 launchWorldXQ12;
  ShotDefinition *shotDefinition;
  ModelWorldPoint emitterPoint;
  uint32_t worldX;
  uint32_t worldY;
  uint32_t worldZQ12;
  int32_t waterDelta;
  EffectDefinition *effectDefinition;
  AngleTurn32 orientationAngle0;
  AngleTurn32 orientationAngle1;
  AngleTurn32 orientationAngle2;

  emitterDefinition = modelRuntime->modelDefinition;
  previousTimerTicks = (int)(modelRuntime->classState).shotEmitterTimerTicks;
  (modelRuntime->classState).shotEmitterTimerTicks =
       (modelRuntime->classState).shotEmitterTimerTicks - g_InGameSimulationStepTicks;
  /* the shot timer has reached 0 or below (signed compare of the old value with the step) */
  if (previousTimerTicks <= (int)g_InGameSimulationStepTicks &&
     ((emitterDefinition->emitterShotDefinitionReference).definition != (ShotDefinition *)(intptr_t)-1)) {
    randomTicks = 0;
    if (emitterDefinition->shotEmitterRandomTicks != 0) {
      randomValue = g_RandomGeneratorState.next();
      randomTicks = randomValue % emitterDefinition->shotEmitterRandomTicks;
    }
    shotDefinition = (emitterDefinition->emitterShotDefinitionReference).definition;
    modelNode = modelRuntime->rootModelNode;
    (modelRuntime->classState).shotEmitterTimerTicks = randomTicks + emitterDefinition->shotEmitterIntervalTicks;
    launchWorldXQ12 = (modelNode->worldTransform).translation.x;
    launchWorldYQ12 = (modelNode->worldTransform).translation.y;
    launchWorldZQ12 = (modelNode->worldTransform).translation.z;
    launchDirection = FixedMath_DirectionFromAnglesScaled
                       ((modelNode->modelPayload).worldRotationAngle1,
                        (modelNode->modelPayload).worldRotationAngle0,Q12_ONE);
    ShotRuntimePool_CreateProjectileFromDefinition
              (0,modelRuntime->ownerArmyRuntime,launchDirection.z + launchWorldZQ12,
               launchDirection.y + launchWorldYQ12,launchDirection.x + launchWorldXQ12,launchWorldZQ12,
               launchWorldYQ12,launchWorldXQ12,shotDefinition,worldRuntime);
  }
  emitterDefinition = modelRuntime->modelDefinition;
  previousTimerTicks = (int)(modelRuntime->classState).effectEmitterTimerTicks;
  (modelRuntime->classState).effectEmitterTimerTicks =
       (modelRuntime->classState).effectEmitterTimerTicks - g_InGameSimulationStepTicks;
  /* the effect timer has reached 0 or below (signed) and there is an effect to emit */
  if (previousTimerTicks <= (int)g_InGameSimulationStepTicks &&
      ((emitterDefinition->emitterEffectDefinitionReference).definition != NULL ||
       (emitterDefinition->waterEmitterEffectDefinitionReference).definition != NULL)) {
    randomTicks = 0;
    if (emitterDefinition->effectEmitterRandomTicks != 0) {
      randomValue = g_RandomGeneratorState.next();
      randomTicks = randomValue % emitterDefinition->effectEmitterRandomTicks;
    }
    (modelRuntime->classState).effectEmitterTimerTicks = randomTicks + emitterDefinition->effectEmitterIntervalTicks;
    if (ArmyEmitter_FindEffectPoint(modelRuntime,emitterDefinition,&emitterPoint)) {
      worldZQ12 = emitterPoint.zQ12;
      worldY = emitterPoint.yQ12;
      worldX = emitterPoint.xQ12;
    }
    else {
      modelNode = modelRuntime->rootModelNode;
      worldX = (modelNode->worldTransform).translation.x;
      worldY = (modelNode->worldTransform).translation.y;
      worldZQ12 = (modelNode->worldTransform).translation.z;
    }
    effectDefinition = (emitterDefinition->emitterEffectDefinitionReference).definition;
    waterDelta = FieldGrid_GetNearestWaterDelta(worldY,worldX,worldRuntime->fieldGrid);
    if (0 < waterDelta) {
      effectDefinition = (emitterDefinition->waterEmitterEffectDefinitionReference).definition;
    }
    if ((effectDefinition == NULL) ||
       ((effectDefinition->transitionPrefix).transitionKind !=
        EFFECT_TRANSITION_INTEGRATE_LINEAR_MOTION_AND_SHADING_POSITION)) {
      modelNode = modelRuntime->rootModelNode;
      orientationAngle2 = (modelNode->modelPayload).worldRotationAngle0;
      orientationAngle1 = (modelNode->modelPayload).worldRotationAngle1;
      orientationAngle0 = (modelNode->modelPayload).worldRotationAngle2;
    }
    else {
      randomValue = g_RandomGeneratorState.next();
      orientationAngle0 = randomValue & FIXED_ANGLE16_MASK;
      orientationAngle1 = FIXED_ANGLE16_QUARTER_TURN - (randomValue >> 20);
      orientationAngle2 = orientationAngle0;
    }
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = NULL },orientationAngle0,
               orientationAngle1,orientationAngle2,worldZQ12,worldY,worldX,effectDefinition,worldRuntime);
    (modelRuntime->classState).effectEmitterPointIndex = (modelRuntime->classState).effectEmitterPointIndex + 1;
  }
  ArmyRuntime_EmitDamageThresholdEffect(worldRuntime,(ModelRuntimeSlot *)modelRuntime);
  return;
}


/* Class vtables. */

ArmyRuntimeOrderHandlerMatrix11x24 g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes = {
    .runtimeUpdate = {
        /*  0 */ THANDOR_FN(ArmyRuntime_UpdateTimedShotAndEffectEmitters),
        /*  1 */ THANDOR_FN(ArmyRuntimeClass_UpdateGroundMovement),
        /*  2 */ THANDOR_FN(ArmyRuntimeClass_UpdateGroundMovementCollisionAndTrackAnimation),
        /*  3 */ THANDOR_FN(ArmyRuntimeClass_UpdateArticulatedMovement),
        /*  4 */ THANDOR_FN(ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage),
        /*  5 */ THANDOR_FN(ArmyRuntimeClass_NoOpTickUpdateForClass5),
        /*  6 */ THANDOR_FN(ArmyRuntimeClass_NoOpTickUpdateForClass6),
        /*  7 */ THANDOR_FN(ArmyRuntimeClass_UpdateSingleBarrelTurret),
        /*  8 */ THANDOR_FN(ArmyRuntimeClass_UpdateTwinBarrelTurret),
        /*  9 */ THANDOR_FN(ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments),
        /* 10 */ THANDOR_FN(ArmyRuntime_UpdateAnimatedModelSubnodes),
        /* 11 */ THANDOR_FN(ArmyRuntimeClass_UpdateStructureFactory),
        /* 12 */ THANDOR_FN(ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy),
        /* 13 */ THANDOR_FN(ArmyRuntimeClass_UpdateUnitFactory),
        /* 14 */ THANDOR_FN(ArmyRuntimeClass_UpdateGridBoundEffectsAndModels),
        /* 15 */ THANDOR_FN(ArmyRuntimeClass_UpdateTransformAndDamageEffect),
        /* 16 */ THANDOR_FN(ArmyRuntime_UpdateTimedShotAndEffectEmitters),
        /* 17 */ THANDOR_FN(ArmyRuntimeClass_UpdateMovementBankingAndChildAnimation),
        /* 18 */ THANDOR_FN(ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement),
        /* 19 */ THANDOR_FN(ArmyRuntimeClass_UpdateWaterSurfaceMovement),
        /* 20 */ THANDOR_FN(ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects),
        /* 21 */ THANDOR_FN(ArmyRuntimeClass_UpdateAircraft),
        /* 22 */ THANDOR_FN(ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode),
        /* 23 */ THANDOR_FN(ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState)
    },
    .classMethodD = {
        /*  0 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpD),
        /*  1 */ THANDOR_FN(ArmyRuntimeClass_UpdateGroundPositionedSounds),
        /*  2 */ THANDOR_FN(ArmyRuntimeAudio_UpdateTrackedTurnAndMoveSounds),
        /*  3 */ THANDOR_FN(ArmyRuntimeClass_NoOpUpdate),
        /*  4 */ THANDOR_FN(ArmyRuntimeAudio_UpdateLoopingSoundWhenEnabled),
        /*  5 */ THANDOR_FN(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  6 */ THANDOR_FN(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  7 */ THANDOR_FN(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  8 */ THANDOR_FN(ArmyRuntimeAudio_UpdateTurretTurnSound),
        /*  9 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 10 */ THANDOR_FN(ArmyRuntime_UpdateLoopingPositionedSound),
        /* 11 */ THANDOR_FN(ArmyRuntimeAudio_UpdateStructureFactorySound),
        /* 12 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 13 */ THANDOR_FN(ArmyRuntimeAudio_UpdateUnitFactorySounds),
        /* 14 */ THANDOR_FN(ArmyRuntime_UpdateLoopingPositionedSound),
        /* 15 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 16 */ THANDOR_FN(ArmyRuntime_UpdateLoopingPositionedSound),
        /* 17 */ THANDOR_FN(ArmyRuntimeAudio_UpdateGliderTurnAndMoveSounds),
        /* 18 */ THANDOR_FN(ArmyRuntimeAudio_DispatchPositionedSoundVariant),
        /* 19 */ THANDOR_FN(ArmyRuntimeClass_UpdateWaterPositionedSounds),
        /* 20 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpD),
        /* 21 */ THANDOR_FN(ArmyRuntimeAudio_UpdateAssetProjectedSound),
        /* 22 */ THANDOR_FN(ArmyRuntimeAudio_UpdateLinkedChildPadSounds),
        /* 23 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpD)
    },
    .modelUnrebase = {
        /*  0 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  1 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  2 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  3 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  4 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  5 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  6 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  7 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  8 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /*  9 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 10 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 11 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 12 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 13 */ THANDOR_FN(ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C),
        /* 14 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 15 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 16 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 17 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 18 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 19 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 20 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 21 */ THANDOR_FN(ModelRuntimeSlot_UnrebaseClassModelLinkOffset60),
        /* 22 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC),
        /* 23 */ THANDOR_FN(UnifiedRuntimeDefault_OneArgNoOpC)
    },
    .modelRebaseOrLoadRepair = {
        /*  0 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  1 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  2 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  3 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  4 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  5 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  6 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  7 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  8 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /*  9 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 10 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 11 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 12 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 13 */ THANDOR_FN(ModelRuntimeSlot_RebaseClassArmyLinkOffset6C),
        /* 14 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 15 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 16 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 17 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 18 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 19 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 20 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 21 */ THANDOR_FN(ModelRuntimeSlot_RebaseClassModelLinkOffset60),
        /* 22 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp),
        /* 23 */ THANDOR_FN(ModelRuntimeSlotPointerRebase_NoOp)
    },
    .modelClassInitialize = {
        /*  0 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  1 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  2 */ THANDOR_FN(ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices),
        /*  3 */ THANDOR_FN(ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming),
        /*  4 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  5 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  6 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  7 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  8 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /*  9 */ THANDOR_FN(ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters),
        /* 10 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /* 11 */ THANDOR_FN(ModelRuntimeSlotClassInit_ResetStructureFactoryBuild),
        /* 12 */ THANDOR_FN(ModelRuntimeSlotClassInit_ClearField60),
        /* 13 */ THANDOR_FN(ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0),
        /* 14 */ THANDOR_FN(ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3),
        /* 15 */ THANDOR_FN(ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1),
        /* 16 */ THANDOR_FN(ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity),
        /* 17 */ THANDOR_FN(ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform),
        /* 18 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /* 19 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /* 20 */ THANDOR_FN(ModelRuntimeSlotClassInit_NoOp),
        /* 21 */ THANDOR_FN(ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset),
        /* 22 */ THANDOR_FN(ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation),
        /* 23 */ THANDOR_FN(ModelRuntimeSlotClassInit_ClearFields60AndB8)
    },
    .modelReleaseOrCommit = {
        /*  0 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  1 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  2 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  3 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  4 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  5 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  6 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  7 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  8 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /*  9 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 10 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 11 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 12 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 13 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 14 */ THANDOR_FN(ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation),
        /* 15 */ THANDOR_FN(ArmyPlacement_ReleaseFactionCapacity),
        /* 16 */ THANDOR_FN(ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity),
        /* 17 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 18 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 19 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 20 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 21 */ THANDOR_FN(ArmyPlacement_ReleaseClassStateReservation),
        /* 22 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB),
        /* 23 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgNoOpB)
    },
    .placementValidation = {
        /*  0 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  1 */ THANDOR_FN(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /*  2 */ THANDOR_FN(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /*  3 */ THANDOR_FN(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /*  4 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /*  5 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  6 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  7 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  8 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /*  9 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /* 10 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /* 11 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /* 12 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /* 13 */ THANDOR_FN(ArmyPlacement_TestModelTerrainAndRuntimeClearance),
        /* 14 */ THANDOR_FN(ArmyPlacement_TestGridOccupancyMask),
        /* 15 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /* 16 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /* 17 */ THANDOR_FN(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /* 18 */ THANDOR_FN(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /* 19 */ THANDOR_FN(ArmyPlacement_TestGridRuntimeAndFieldBlocking),
        /* 20 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /* 21 */ THANDOR_FN(UnifiedRuntimeDefault_TwoArgSuccess),
        /* 22 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime),
        /* 23 */ THANDOR_FN(ArmyPlacementCollision_TestCurrentRuntime)
    },
    .placementAssetClassDispatch = {
        /*  0 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /*  1 */ THANDOR_FN(ArmyPlacement_CanPlaceMobileUnit),
        /*  2 */ THANDOR_FN(ArmyPlacement_CanPlaceMobileUnit),
        /*  3 */ THANDOR_FN(ArmyPlacement_CanPlaceMobileUnit),
        /*  4 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /*  5 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /*  6 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /*  7 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /*  8 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /*  9 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /* 10 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /* 11 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /* 12 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /* 13 */ THANDOR_FN(ArmyPlacement_CanPlaceAnchoredModel),
        /* 14 */ THANDOR_FN(ArmyPlacement_CanPlaceResourceExtractor),
        /* 15 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /* 16 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /* 17 */ THANDOR_FN(ArmyPlacement_CanPlaceMobileUnit),
        /* 18 */ THANDOR_FN(ArmyPlacement_CanPlaceMobileUnit),
        /* 19 */ THANDOR_FN(ArmyPlacement_CanPlaceMobileUnit),
        /* 20 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /* 21 */ THANDOR_FN(ArmyPlacement_CanPlaceAnywhere),
        /* 22 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding),
        /* 23 */ THANDOR_FN(ArmyPlacement_CanPlaceBuilding)
    },
    .classCommand = {
        /*  0 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  1 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  2 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  3 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  4 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /*  5 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  6 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  7 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  8 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /*  9 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /* 10 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 11 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 12 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /* 13 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 14 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 15 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 16 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 17 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /* 18 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /* 19 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /* 20 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 21 */ THANDOR_FN(ArmyRuntimeClassCommand_NoOp),
        /* 22 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA),
        /* 23 */ THANDOR_FN(ArmyRuntime_ClassCommandHandlerGroupA)
    },
    .gridInfluenceAdd = {
        /*  0 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /*  1 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /*  2 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /*  3 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /*  4 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /*  5 */ THANDOR_FN(GridInfluence_AddNoOp),
        /*  6 */ THANDOR_FN(GridInfluence_AddNoOp),
        /*  7 */ THANDOR_FN(GridInfluence_AddNoOp),
        /*  8 */ THANDOR_FN(GridInfluence_AddNoOp),
        /*  9 */ THANDOR_FN(GridInfluence_AddNoOp),
        /* 10 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 11 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 12 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 13 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 14 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 15 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 16 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 17 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /* 18 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /* 19 */ THANDOR_FN(GridInfluence_AddHighDistanceBands),
        /* 20 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 21 */ THANDOR_FN(GridInfluence_AddNoOp),
        /* 22 */ THANDOR_FN(GridInfluence_AddLowDistanceBands),
        /* 23 */ THANDOR_FN(GridInfluence_AddHighDistanceBands)
    },
    .gridInfluenceRemove = {
        /*  0 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /*  1 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /*  2 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /*  3 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /*  4 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /*  5 */ THANDOR_FN(GridInfluence_RemoveNoOp),
        /*  6 */ THANDOR_FN(GridInfluence_RemoveNoOp),
        /*  7 */ THANDOR_FN(GridInfluence_RemoveNoOp),
        /*  8 */ THANDOR_FN(GridInfluence_RemoveNoOp),
        /*  9 */ THANDOR_FN(GridInfluence_RemoveNoOp),
        /* 10 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 11 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 12 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 13 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 14 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 15 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 16 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 17 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /* 18 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /* 19 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands),
        /* 20 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 21 */ THANDOR_FN(GridInfluence_RemoveNoOp),
        /* 22 */ THANDOR_FN(GridInfluence_RemoveLowDistanceBands),
        /* 23 */ THANDOR_FN(GridInfluence_RemoveHighDistanceBands)
    }};

RuntimeMaintenanceCallbackPhasesTyped g_RuntimeMaintenanceCallbackPhases = {
    .primaryUpdate = {.army = THANDOR_FN(ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers), .shot = THANDOR_FN(ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects), .effect = THANDOR_FN(EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions)},
    .terrainStateRefresh = {.army = THANDOR_FN(ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint), .shot = THANDOR_FN(ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint), .effect = THANDOR_FN(EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint)},
    .occupancyRebuild = {.army = THANDOR_FN(ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback), .shot = THANDOR_FN(ShotRuntimeMaintenance_OccupancyRebuildNoOp), .effect = THANDOR_FN(EffectRuntimeMaintenance_OccupancyRebuildNoOp)},
    .audioRefresh = {.army = THANDOR_FN(ArmyRuntimeMaintenance_DispatchClassMethodDRecursive), .shot = THANDOR_FN(ShotRuntimeMaintenance_UpdateHierarchyProjectedSound), .effect = THANDOR_FN(EffectRuntimeMaintenance_AudioRefreshNoOp)}};
