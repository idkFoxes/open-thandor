/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/terrain/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* filled at startup by GraphicsLighting_BuildPackedLookupTable */
__declspec(align(16)) uint64_t g_PackedLightingLookupTable[512] = {0};

__declspec(align(16)) uint64_t g_TerrainOccupancyMmxSignBiasBytes = 0x8080808080808080ull;

__declspec(align(8)) uint64_t g_TerrainOccupancyMmxClearBits1And2Mask = 0xF9F9F9F9F9F9F9F9ull;

__declspec(align(16)) uint64_t g_TerrainOccupancyMmxAllBitsMask = 0xFFFFFFFFFFFFFFFFull;

__declspec(align(8)) uint64_t g_TerrainOccupancyMmxPackedScale0280 = 0x280028002800280ull;

__declspec(align(16)) uint64_t g_TerrainOccupancyMmxPersistentWeights = 0x20000200200002ull;

__declspec(align(8)) uint64_t g_TerrainOccupancyMmxCurrentWeights = 0x40000400400004ull;

__declspec(align(16)) uint64_t g_FieldGridOccupancyMmxHighBitMask = 0x8080808080808080ull;

__declspec(align(4)) GraphicsTextureSetLoadPackageProc *g_GraphicsTextureSetLoadPackage = (void *)GraphicsTextureSet_LoadPackage;

__declspec(align(16)) GraphicsTextureSetReleasePackageProc *g_GraphicsTextureSetReleasePackage = (void *)GraphicsTextureSet_ReleasePackage;

__declspec(align(4)) GraphicsPaletteAssetLoadPackageProc *g_GraphicsPaletteAssetLoadPackage = (void *)GraphicsPaletteAsset_LoadPackage;

__declspec(align(16)) FieldGridInterpolationCallbackTable5 g_FieldGridInterpolationCallbacks5 = {
    .callbacks = {
        /* 0 */ (void *)FieldGrid_InterpolateTerrainHeight,
        /* 1 */ (void *)FieldGrid_InterpolateWaterSurfaceHeight,
        /* 2 */ (void *)FieldGrid_InterpolateTerrainHeight,
        /* 3 */ (void *)FieldGrid_InterpolateTerrainHeight,
        /* 4 */ (void *)FieldGrid_InterpolateTopSurfaceHeight
    }
};

__declspec(align(16)) TerrainProjectedRowSpan g_TerrainProjectedRowSpans[260] = {
    /*   0 */ {0},
    /*   1 */ {0},
    /*   2 */ {0},
    /*   3 */ {0},
    /*   4 */ {0},
    /*   5 */ {0},
    /*   6 */ {0},
    /*   7 */ {0},
    /*   8 */ {0},
    /*   9 */ {0},
    /*  10 */ {0},
    /*  11 */ {0},
    /*  12 */ {0},
    /*  13 */ {0},
    /*  14 */ {0},
    /*  15 */ {0},
    /*  16 */ {0},
    /*  17 */ {0},
    /*  18 */ {0},
    /*  19 */ {0},
    /*  20 */ {0},
    /*  21 */ {0},
    /*  22 */ {0},
    /*  23 */ {0},
    /*  24 */ {0},
    /*  25 */ {0},
    /*  26 */ {0},
    /*  27 */ {0},
    /*  28 */ {0},
    /*  29 */ {0},
    /*  30 */ {0},
    /*  31 */ {0},
    /*  32 */ {0},
    /*  33 */ {0},
    /*  34 */ {0},
    /*  35 */ {0},
    /*  36 */ {0},
    /*  37 */ {0},
    /*  38 */ {0},
    /*  39 */ {0},
    /*  40 */ {0},
    /*  41 */ {0},
    /*  42 */ {0},
    /*  43 */ {0},
    /*  44 */ {0},
    /*  45 */ {0},
    /*  46 */ {0},
    /*  47 */ {0},
    /*  48 */ {0},
    /*  49 */ {0},
    /*  50 */ {0},
    /*  51 */ {0},
    /*  52 */ {0},
    /*  53 */ {0},
    /*  54 */ {0},
    /*  55 */ {0},
    /*  56 */ {0},
    /*  57 */ {0},
    /*  58 */ {0},
    /*  59 */ {0},
    /*  60 */ {0},
    /*  61 */ {0},
    /*  62 */ {0},
    /*  63 */ {0},
    /*  64 */ {0},
    /*  65 */ {0},
    /*  66 */ {0},
    /*  67 */ {0},
    /*  68 */ {0},
    /*  69 */ {0},
    /*  70 */ {0},
    /*  71 */ {0},
    /*  72 */ {0},
    /*  73 */ {0},
    /*  74 */ {0},
    /*  75 */ {0},
    /*  76 */ {0},
    /*  77 */ {0},
    /*  78 */ {0},
    /*  79 */ {0},
    /*  80 */ {0},
    /*  81 */ {0},
    /*  82 */ {0},
    /*  83 */ {0},
    /*  84 */ {0},
    /*  85 */ {0},
    /*  86 */ {0},
    /*  87 */ {0},
    /*  88 */ {0},
    /*  89 */ {0},
    /*  90 */ {0},
    /*  91 */ {0},
    /*  92 */ {0},
    /*  93 */ {0},
    /*  94 */ {0},
    /*  95 */ {0},
    /*  96 */ {0},
    /*  97 */ {0},
    /*  98 */ {0},
    /*  99 */ {0},
    /* 100 */ {0},
    /* 101 */ {0},
    /* 102 */ {0},
    /* 103 */ {0},
    /* 104 */ {0},
    /* 105 */ {0},
    /* 106 */ {0},
    /* 107 */ {0},
    /* 108 */ {0},
    /* 109 */ {0},
    /* 110 */ {0},
    /* 111 */ {0},
    /* 112 */ {0},
    /* 113 */ {0},
    /* 114 */ {0},
    /* 115 */ {0},
    /* 116 */ {0},
    /* 117 */ {0},
    /* 118 */ {0},
    /* 119 */ {0},
    /* 120 */ {0},
    /* 121 */ {0},
    /* 122 */ {0},
    /* 123 */ {0},
    /* 124 */ {0},
    /* 125 */ {0},
    /* 126 */ {0},
    /* 127 */ {0},
    /* 128 */ {0},
    /* 129 */ {0},
    /* 130 */ {0},
    /* 131 */ {0},
    /* 132 */ {0},
    /* 133 */ {0},
    /* 134 */ {0},
    /* 135 */ {0},
    /* 136 */ {0},
    /* 137 */ {0},
    /* 138 */ {0},
    /* 139 */ {0},
    /* 140 */ {0},
    /* 141 */ {0},
    /* 142 */ {0},
    /* 143 */ {0},
    /* 144 */ {0},
    /* 145 */ {0},
    /* 146 */ {0},
    /* 147 */ {0},
    /* 148 */ {0},
    /* 149 */ {0},
    /* 150 */ {0},
    /* 151 */ {0},
    /* 152 */ {0},
    /* 153 */ {0},
    /* 154 */ {0},
    /* 155 */ {0},
    /* 156 */ {0},
    /* 157 */ {0},
    /* 158 */ {0},
    /* 159 */ {0},
    /* 160 */ {0},
    /* 161 */ {0},
    /* 162 */ {0},
    /* 163 */ {0},
    /* 164 */ {0},
    /* 165 */ {0},
    /* 166 */ {0},
    /* 167 */ {0},
    /* 168 */ {0},
    /* 169 */ {0},
    /* 170 */ {0},
    /* 171 */ {0},
    /* 172 */ {0},
    /* 173 */ {0},
    /* 174 */ {0},
    /* 175 */ {0},
    /* 176 */ {0},
    /* 177 */ {0},
    /* 178 */ {0},
    /* 179 */ {0},
    /* 180 */ {0},
    /* 181 */ {0},
    /* 182 */ {0},
    /* 183 */ {0},
    /* 184 */ {0},
    /* 185 */ {0},
    /* 186 */ {0},
    /* 187 */ {0},
    /* 188 */ {0},
    /* 189 */ {0},
    /* 190 */ {0},
    /* 191 */ {0},
    /* 192 */ {0},
    /* 193 */ {0},
    /* 194 */ {0},
    /* 195 */ {0},
    /* 196 */ {0},
    /* 197 */ {0},
    /* 198 */ {0},
    /* 199 */ {0},
    /* 200 */ {0},
    /* 201 */ {0},
    /* 202 */ {0},
    /* 203 */ {0},
    /* 204 */ {0},
    /* 205 */ {0},
    /* 206 */ {0},
    /* 207 */ {0},
    /* 208 */ {0},
    /* 209 */ {0},
    /* 210 */ {0},
    /* 211 */ {0},
    /* 212 */ {0},
    /* 213 */ {0},
    /* 214 */ {0},
    /* 215 */ {0},
    /* 216 */ {0},
    /* 217 */ {0},
    /* 218 */ {0},
    /* 219 */ {0},
    /* 220 */ {0},
    /* 221 */ {0},
    /* 222 */ {0},
    /* 223 */ {0},
    /* 224 */ {0},
    /* 225 */ {0},
    /* 226 */ {0},
    /* 227 */ {0},
    /* 228 */ {0},
    /* 229 */ {0},
    /* 230 */ {0},
    /* 231 */ {0},
    /* 232 */ {0},
    /* 233 */ {0},
    /* 234 */ {0},
    /* 235 */ {0},
    /* 236 */ {0},
    /* 237 */ {0},
    /* 238 */ {0},
    /* 239 */ {0},
    /* 240 */ {0},
    /* 241 */ {0},
    /* 242 */ {0},
    /* 243 */ {0},
    /* 244 */ {0},
    /* 245 */ {0},
    /* 246 */ {0},
    /* 247 */ {0},
    /* 248 */ {0},
    /* 249 */ {0},
    /* 250 */ {0},
    /* 251 */ {0},
    /* 252 */ {0},
    /* 253 */ {0},
    /* 254 */ {0},
    /* 255 */ {0},
    /* 256 */ {0},
    /* 257 */ {0},
    /* 258 */ {0},
    /* 259 */ {.firstColumn = -1869574000, .endColumnExclusive = -1869574000}};

/* entries 0..255 the shaded colour ramp (originally
   g_TerrainLightingColorRampArgb256), entries 256..512 the lit half; indexed by the signed dot
   product -256..256 from entry 256 */
__declspec(align(16)) PackedArgb32 g_TerrainDirectionalLightColorLut[513] = {0};

__declspec(align(4)) uint32_t g_TerrainDirectionalLightSecondaryColor = 0;

/* Q28 unit vector */
__declspec(align(8)) GraphicsFixedVec3 g_TerrainLightDirection = {0};

__declspec(align(4)) uint8_t *g_TerrainByteClampLookup = 0;

__declspec(align(8)) TerrainDirectionRecord g_TerrainDirectionRecordTable256[256] = {0};

__declspec(align(4)) GraphicsTextureSet *g_TerrainPrimaryTextureSet = 0;

__declspec(align(8)) void *g_TerrainSoilPacketTablePayload = 0;

__declspec(align(4)) void *g_TerrainSurfacePacketTablePayload = 0;

__declspec(align(4)) GraphicsPaletteAsset *g_TerrainPrimaryPalette = 0;

__declspec(align(16)) TerrainMaterialSuffixEntry g_TerrainMaterialTextureSuffixLettersUtf16AtoZ[26] = {
    /*  0 */ {.lowercaseLetterUtf16 = 97},
    /*  1 */ {.lowercaseLetterUtf16 = 98},
    /*  2 */ {.lowercaseLetterUtf16 = 99},
    /*  3 */ {.lowercaseLetterUtf16 = 100},
    /*  4 */ {.lowercaseLetterUtf16 = 101},
    /*  5 */ {.lowercaseLetterUtf16 = 102},
    /*  6 */ {.lowercaseLetterUtf16 = 103},
    /*  7 */ {.lowercaseLetterUtf16 = 104},
    /*  8 */ {.lowercaseLetterUtf16 = 105},
    /*  9 */ {.lowercaseLetterUtf16 = 106},
    /* 10 */ {.lowercaseLetterUtf16 = 107},
    /* 11 */ {.lowercaseLetterUtf16 = 108},
    /* 12 */ {.lowercaseLetterUtf16 = 109},
    /* 13 */ {.lowercaseLetterUtf16 = 110},
    /* 14 */ {.lowercaseLetterUtf16 = 111},
    /* 15 */ {.lowercaseLetterUtf16 = 112},
    /* 16 */ {.lowercaseLetterUtf16 = 113},
    /* 17 */ {.lowercaseLetterUtf16 = 114},
    /* 18 */ {.lowercaseLetterUtf16 = 115},
    /* 19 */ {.lowercaseLetterUtf16 = 116},
    /* 20 */ {.lowercaseLetterUtf16 = 117},
    /* 21 */ {.lowercaseLetterUtf16 = 118},
    /* 22 */ {.lowercaseLetterUtf16 = 119},
    /* 23 */ {.lowercaseLetterUtf16 = 120},
    /* 24 */ {.lowercaseLetterUtf16 = 121},
    /* 25 */ {.lowercaseLetterUtf16 = 122}};

__declspec(align(8)) int32_t g_TerrainHeightBandMaximumDelta = 1024;

__declspec(align(4)) int32_t g_TerrainHeightBandMinimumDelta = -1024;

/* int32_t minimum (triangle1NormalAngles >> 16) for the auxiliary height/placement scans in world/terrain/height.c (0x3000) */
__declspec(align(16)) int32_t g_TerrainAuxHeightMinimum = 12288;

__declspec(align(16)) int32_t g_TerrainHeightDeltaScaleByStepQ12[256] = {
    /*   0 */ 4096, 3277, 2731, 2341, 2048, 1820, 1638, 1489, 1365, 1260, 1170, 1092, 1024, 964, 910, 862,
    /*  16 */ 819, 780, 745, 712, 683, 655, 630, 607, 585, 565, 546, 529, 512, 496, 482, 468,
    /*  32 */ 455, 443, 431, 420, 410, 400, 390, 381, 372, 364, 356, 349, 341, 334, 328, 321,
    /*  48 */ 315, 309, 303, 298, 293, 287, 282, 278, 273, 269, 264, 260, 256, 252, 248, 245,
    /*  64 */ 241, 237, 234, 231, 228, 224, 221, 218, 216, 213, 210, 207, 205, 202, 200, 197,
    /*  80 */ 195, 193, 191, 188, 186, 184, 182, 180, 178, 176, 174, 172, 171, 169, 167, 165,
    /*  96 */ 164, 162, 161, 159, 158, 156, 155, 153, 152, 150, 149, 148, 146, 145, 144, 142,
    /* 112 */ 141, 140, 139, 138, 137, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125,
    /* 128 */ 124, 123, 122, 121, 120, 120, 119, 118, 117, 116, 115, 115, 114, 113, 112, 111,
    /* 144 */ 111, 110, 109, 109, 108, 107, 106, 106, 105, 104, 104, 103, 102, 102, 101, 101,
    /* 160 */ 100, 99, 99, 98, 98, 97, 96, 96, 95, 95, 94, 94, 93, 93, 92, 92,
    /* 176 */ 91, 91, 90, 90, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84,
    /* 192 */ 84, 83, 83, 82, 82, 82, 81, 81, 80, 80, 80, 79, 79, 78, 78, 78,
    /* 208 */ 77, 77, 77, 76, 76, 76, 75, 75, 74, 74, 74, 73, 73, 73, 72, 72,
    /* 224 */ 72, 72, 71, 71, 71, 70, 70, 70, 69, 69, 69, 69, 68, 68, 68, 67,
    /* 240 */ 67, 67, 67, 66, 66, 66, 66, 65, 65, 65, 65, 64, 64, 64, 64, 63};

__declspec(align(16)) uint32_t g_TerrainScanRowStrideBytes = 0;

__declspec(align(4)) uint32_t g_TerrainScanStepLimit = 0;

__declspec(align(8)) TerrainScanSelectorUnion g_TerrainScanSharedSelectorValue = {0};

__declspec(align(4)) uint32_t g_TerrainScanReferenceHeight = 0;

__declspec(align(16)) TerrainClassPlacementAndOverlayCallbackTable10 g_TerrainClassPlacementAndOverlayCallbacks10 = {
    .placementTests = {
        /* 0 */ (void *)TerrainHeightBand_TestAroundWorldPoint,
        /* 1 */ (void *)TerrainAuxHeightThreshold_TestAroundWorldPoint,
        /* 2 */ (void *)TerrainHeightBand_TestAroundWorldPoint,
        /* 3 */ (void *)TerrainHeightBand_TestAroundWorldPoint,
        /* 4 */ (void *)TerrainHeightBand_TestAroundWorldPoint
    },
    .overlayCallbacks = {
        /* 0 */ (void *)FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint,
        /* 1 */ (void *)FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint,
        /* 2 */ (void *)FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint,
        /* 3 */ (void *)FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint,
        /* 4 */ (void *)FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
    }};

__declspec(align(16)) TerrainCompositeTextureRuntime *g_TerrainCompositeTexture = 0;

__declspec(align(4)) InGameRuntimeRoot *g_InGameRuntimeRoot = 0;

__declspec(align(16)) GraphicsTextureSourceAsset *g_InGamePanelTextureSource = 0;

__declspec(align(4)) uint32_t g_TerrainMaterialEditFieldGrid = 0;

__declspec(align(16)) uint32_t g_TerrainMaterialEditDeltaBuffer = 0;

__declspec(align(4)) uint32_t g_TerrainMaterialEditReferenceMaterialByte = 0;

__declspec(align(8)) uint32_t g_TerrainMaterialEditReplacementMaterialByte = 0;
