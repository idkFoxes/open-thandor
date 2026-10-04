/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_TYPES_H
#define THANDOR_GAMEPLAY_AI_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/gameplay/faction/types.h>
#include <thandor/ui/ingame/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct AiKnowledgeParameters AiKnowledgeParameters, *PAiKnowledgeParameters;
typedef struct AiKnowledgeDataImage AiKnowledgeDataImage, *PAiKnowledgeDataImage;
typedef struct AiScoredSiteWorkspaceEntry AiScoredSiteWorkspaceEntry, *PAiScoredSiteWorkspaceEntry;
typedef struct AiTerrainFeatureWorkspaceEntry AiTerrainFeatureWorkspaceEntry, *PAiTerrainFeatureWorkspaceEntry;
typedef struct AiRuntimeWorkspaceEntry AiRuntimeWorkspaceEntry, *PAiRuntimeWorkspaceEntry;
typedef struct AiTargetWorkspaceEntry AiTargetWorkspaceEntry, *PAiTargetWorkspaceEntry;
typedef struct AiLinkedDefinitionListView AiLinkedDefinitionListView, *PAiLinkedDefinitionListView;
typedef struct AiArmyScoreWeights AiArmyScoreWeights, *PAiArmyScoreWeights;
typedef struct AiTechnologyPlanningCandidate AiTechnologyPlanningCandidate, *PAiTechnologyPlanningCandidate;
typedef struct MdlDefinitionSemanticPrefix MdlDefinitionSemanticPrefix, *PMdlDefinitionSemanticPrefix;
typedef struct AiSecondaryWorkspaceDistanceSelection AiSecondaryWorkspaceDistanceSelection, *PAiSecondaryWorkspaceDistanceSelection;
typedef struct AiGeneralSiteDistanceSelection AiGeneralSiteDistanceSelection, *PAiGeneralSiteDistanceSelection;
typedef struct AiStrategicClassSelection AiStrategicClassSelection, *PAiStrategicClassSelection;
typedef struct AiStructureWorkspaceEntry AiStructureWorkspaceEntry, *PAiStructureWorkspaceEntry;
typedef struct FieldGridCell FieldGridCell;
typedef struct ModelRuntimeNode ModelRuntimeNode;

typedef uint32_t AiPlanningPhaseIndex;

enum {
    TEC_000_BASIC_TECHNOLOGY=0,
    TEC_001_ARMS_FACTORIES=1,
    TEC_002_WEASEL=2,
    TEC_003_WEASEL_ARMOUR_PLUS_25_PERCENT=3,
    TEC_004_WEASEL_ARMOUR_PLUS_25=4,
    TEC_005_ARV_DINGO=5,
    TEC_006_ARV_DINGO_ARMOUR_PLUS_25_PERCENT=6,
    TEC_007_ARV_DINGO_ARMOUR_PLUS_20_PERCENT=7,
    TEC_008_AV_WOLF=8,
    TEC_009_AV_WOLF_ARMOUR_PLUS_25_PERCENT=9,
    TEC_010_AV_WOLF_ARMOUR_PLUS_20_PERCENT=10,
    TEC_011_PIONEER_VEHICLE=11,
    TEC_012_RESERVED=12,
    TEC_013_RESERVED=13,
    TEC_014_RESERVED=14,
    TEC_015_TANK_BADGER=15,
    TEC_016_TANK_BADGER_ARMOUR_PLUS_25_PERCENT=16,
    TEC_017_TANK_BADGER_ARMOUR_PLUS_20_PERCENT=17,
    TEC_018_PANTHER_TECHNOLOGY=18,
    TEC_019_TANK_PANTHER_ARMOUR_PLUS_25_PERCENT=19,
    TEC_020_TANK_PANTHER_ARMOUR_PLUS_25_PERCENT=20,
    TEC_021_GOLIATH_TECHNOLOGY=21,
    TEC_022_GOLIATH_ARMOUR_PLUS_30_PERCENT=22,
    TEC_023_GOLIATH_ARMOUR_PLUS_20_PERCENT=23,
    TEC_024_RESERVED=24,
    TEC_025_RESERVED=25,
    TEC_026_RESERVED=26,
    TEC_027_RESERVED=27,
    TEC_028_RESERVED=28,
    TEC_029_RESERVED=29,
    TEC_030_EMU_WALKER=30,
    TEC_031_EMU_ARMOUR_PLUS_30_PERCENT=31,
    TEC_032_EMU_ARMOUR_PLUS_25_PERCENT=32,
    TEC_033_ORANG_TECHNOLOGY=33,
    TEC_034_ORANG_ARMOUR_PLUS_25_PERCENT=34,
    TEC_035_ORANG_ARMOUR_PLUS_25_PERCENT=35,
    TEC_036_GORILLA_TECHNOLOGY=36,
    TEC_037_GORILLA_ARMOUR_PLUS_25_PERCENT=37,
    TEC_038_GORILLA_ARMOUR_PLUS_25_PERCENT=38,
    TEC_039_RESERVED=39,
    TEC_040_RESERVED=40,
    TEC_041_RESERVED=41,
    TEC_042_RESERVED=42,
    TEC_043_RESERVED=43,
    TEC_044_RESERVED=44,
    TEC_045_GLD_MINUS_1_MOSQUITO=45,
    TEC_046_GLD_MINUS_1_MOSQUITO_ARMOUR_PLUS_20_PERCENT=46,
    TEC_047_GLD_MINUS_1_MOSQUITO_ARMOUR_PLUS_25_PERCENT=47,
    TEC_048_GLD_MINUS_2_DRAGONFLY=48,
    TEC_049_GLD_MINUS_2_DRAGONFLY_ARMOUR_PLUS_25_PERCENT=49,
    TEC_050_GLD_MINUS_2_DRAGONFLY_ARMOUR_PLUS_20_PERCENT=50,
    TEC_051_GLD_MINUS_3_HORNET=51,
    TEC_052_GLD_MINUS_3_HORNET_ARMOUR_PLUS_25_PERCENT=52,
    TEC_053_GLD_MINUS_3_HORNET_ARMOUR_PLUS_20_PERCENT=53,
    TEC_054_BUMBLEBEE_SERIES=54,
    TEC_055_SPF_BUMBLEBEE_ARMOUR_PLUS_30_PERCENT=55,
    TEC_056_SPF_BUMBLEBEE_ARMOUR_PLUS_20_PERCENT=56,
    TEC_057_RESERVED=57,
    TEC_058_RESERVED=58,
    TEC_059_RESERVED=59,
    TEC_060_DOLPHIN_CLASS=60,
    TEC_061_DOLPHIN_CLASS_ARMOUR_PLUS_25_PERCENT=61,
    TEC_062_DOLPHIN_CLASS_ARMOUR_PLUS_25_PERCENT=62,
    TEC_063_HURRICANE_CLASS=63,
    TEC_064_HURRICANE_CLASS_ARMOUR_PLUS_30_PERCENT=64,
    TEC_065_HURRICANE_CLASS_ARMOUR_PLUS_20_PERCENT=65,
    TEC_066_TYPHOON_CLASS=66,
    TEC_067_TYPHOON_CLASS_ARMOUR_PLUS_25_PERCENT=67,
    TEC_068_TYPHOON_CLASS_ARMOUR_PLUS_25_PERCENT=68,
    TEC_069_LANDING_CRAFT=69,
    TEC_070_RESERVED=70,
    TEC_071_RESERVED=71,
    TEC_072_RESERVED=72,
    TEC_073_RESERVED=73,
    TEC_074_RESERVED=74,
    TEC_075_SP_CROW=75,
    TEC_076_SP_CROW_ARMOUR_PLUS_30_PERCENT=76,
    TEC_077_SP_CROW_ARMOUR_PLUS_20_PERCENT=77,
    TEC_078_B_MINUS_12_HAWK=78,
    TEC_079_B_MINUS_12_HAWK_ARMOUR_PLUS_25_PERCENT=79,
    TEC_080_B_MINUS_12_HAWK_ARMOUR_PLUS_25_PERCENT=80,
    TEC_081_B_MINUS_18_EAGLE=81,
    TEC_082_B_MINUS_18_EAGLE_ARMOUR_PLUS_30_PERCENT=82,
    TEC_083_B_MINUS_18_EAGLE_ARMOUR_PLUS_20_PERCENT=83,
    TEC_084_RESERVED=84,
    TEC_085_RESERVED=85,
    TEC_086_RESERVED=86,
    TEC_087_RESERVED=87,
    TEC_088_RESERVED=88,
    TEC_089_RESERVED=89,
    TEC_090_LASER=90,
    TEC_091_LASER_RANGE_PLUS_15_PERCENT=91,
    TEC_092_LASER_DAMAGE_PLUS_50_PERCENT=92,
    TEC_093_DUAL_LASER=93,
    TEC_094_DUAL_LASER_RANGE_PLUS_15=94,
    TEC_095_DUAL_LASER_DAMAGE_PLUS_50_PERCENT=95,
    TEC_096_LASER_GUN=96,
    TEC_097_LASER_GUN_RANGE_PLUS_5_PERCENT=97,
    TEC_098_LASER_GUN_DAMAGE_PLUS_50_PERCENT=98,
    TEC_099_LASER_TURRET=99,
    TEC_100_LASER_TURRET_RANGE_PLUS_10_PERCENT=100,
    TEC_101_LASER_TURRET_DAMAGE_PLUS_50_PERCENT=101,
    TEC_102_ION_GUN=102,
    TEC_103_ION_GUN_RANGE_PLUS_20_PERCENT=103,
    TEC_104_ION_GUN_DAMAGE_PLUS_50_PERCENT=104,
    TEC_105_ION_THROWER=105,
    TEC_106_ION_THROWER_RANGE_PLUS_20_PERCENT=106,
    TEC_107_ION_THROWER_DAMAGE_PLUS_50_PERCENT=107,
    TEC_108_ION_CANNON=108,
    TEC_109_ION_CANNON_RANGE_PLUS_15_PERCENT=109,
    TEC_110_ION_CANNON_DAMAGE_PLUS_50_PERCENT=110,
    TEC_111_RESERVED=111,
    TEC_112_RESERVED=112,
    TEC_113_RESERVED=113,
    TEC_114_RESERVED=114,
    TEC_115_RESERVED=115,
    TEC_116_RESERVED=116,
    TEC_117_RESERVED=117,
    TEC_118_RESERVED=118,
    TEC_119_RESERVED=119,
    TEC_120_DOUBLE_MG=120,
    TEC_121_DOUBLE_MG_RANGE_PLUS_25_PERCENT=121,
    TEC_122_DOUBLE_MG_DAMAGE_PLUS_50_PERCENT=122,
    TEC_123_AUTOMATIC_CANNON=123,
    TEC_124_AUTOMATIC_CANNON_RANGE_PLUS_20_PERCENT=124,
    TEC_125_AUTOMATIC_CANNON_DAMAGE_PLUS_50_PERCENT=125,
    TEC_126_CANNON=126,
    TEC_127_CANNON_RANGE_PLUS_15_PERCENT=127,
    TEC_128_CANNON_DAMAGE_PLUS_50_PERCENT=128,
    TEC_129_TWIN_CANNON=129,
    TEC_130_TWIN_CANNON_RANGE_PLUS_15_PERCENT=130,
    TEC_131_TWIN_CANNON_DAMAGE_PLUS_50_PERCENT=131,
    TEC_132_FLAK=132,
    TEC_133_FLAK_RANGE_PLUS_10_PERCENT=133,
    TEC_134_FLAK_DAMAGE_PLUS_50_PERCENT=134,
    TEC_135_ANTI_MINUS_AIRCRAFT_GUNS=135,
    TEC_136_ANTI_MINUS_AIRCRAFT_GUN_RANGE_PLUS_10_PERCENT=136,
    TEC_137_ANTI_MINUS_AIRCRAFT_GUN_DAMAGE_PLUS_50_PERCENT=137,
    TEC_138_MORTAR=138,
    TEC_139_MORTAR_RANGE_PLUS_10=139,
    TEC_140_MORTAR_DAMAGE_PLUS_50_PERCENT=140,
    TEC_141_RESERVED=141,
    TEC_142_RESERVED=142,
    TEC_143_RESERVED=143,
    TEC_144_RESERVED=144,
    TEC_145_RESERVED=145,
    TEC_146_RESERVED=146,
    TEC_147_RESERVED=147,
    TEC_148_RESERVED=148,
    TEC_149_RESERVED=149,
    TEC_150_ROCKET_LAUNCHER=150,
    TEC_151_ROCKET_LAUNCHER_RANGE_PLUS_10_PERCENT=151,
    TEC_152_ROCKET_LAUNCHER_DAMAGE_PLUS_50_PERCENT=152,
    TEC_153_ROCKET_BATTERY=153,
    TEC_154_ROCKET_BATTERY_RANGE_PLUS_10_PERCENT=154,
    TEC_155_ROCKET_BATTERY_DAMAGE_PLUS_50_PERCENT=155,
    TEC_156_ROCKET_FIRING_RAMP=156,
    TEC_157_ROCKET_FIRING_RAMP_RANGE_PLUS_10_PERCENT=157,
    TEC_158_ROCKET_FIRING_RAMP_DAMAGE_PLUS_50_PERCENT=158,
    TEC_159_GUIDED_MISSILES=159,
    TEC_160_GUIDED_MISSILES_RANGE_PLUS_15_PERCENT=160,
    TEC_161_GUIDED_MISSILES_DAMAGE_PLUS_50_PERCENT=161,
    TEC_162_HEAT_MINUS_SEEKING_MISSILES=162,
    TEC_163_HEAT_MINUS_SEEKING_MISSILES_RANGE_PLUS_10_PERCENT=163,
    TEC_164_HEAT_SEEKING_MISSILES_DAMAGE_PLUS_50_PERCENT=164,
    TEC_165_HOWITZER=165,
    TEC_166_HOWITZER_ROTATION_PLUS=166,
    TEC_167_HOWITZER_DAMAGE_PLUS=167,
    TEC_168_ARTILLERY=168,
    TEC_169_ARTILLERY_ROTATION_PLUS=169,
    TEC_170_ARTILLERY_DAMAGE_PLUS=170,
    TEC_171_RESERVED=171,
    TEC_172_RESERVED=172,
    TEC_173_RESERVED=173,
    TEC_174_RESERVED=174,
    TEC_175_RESERVED=175,
    TEC_176_RESERVED=176,
    TEC_177_RESERVED=177,
    TEC_178_RESERVED=178,
    TEC_179_RESERVED=179,
    TEC_180_FLAME_THROWER=180,
    TEC_181_FLAME_THROWER_RANGE_PLUS_10_PERCENT=181,
    TEC_182_FLAME_THROWER_DAMAGE=182,
    TEC_183_RADAR=183,
    TEC_184_RADAR_RANGE_PLUS_10_PERCENT=184,
    TEC_185_RADAR_RANGE_PLUS_10_PERCENT=185,
    TEC_186_LASER_COIL=186,
    TEC_187_LASER_COIL_RANGE_PLUS_10=187,
    TEC_188_LASER_COIL_DAMAGE_PLUS_50_PERCENT=188,
    TEC_189_AR_MINUS_M_SILO=189,
    TEC_190_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT=190,
    TEC_191_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT=191,
    TEC_192_REPAIR_MODULE=192,
    TEC_193_REPAIR_MODULE_PLUS=193,
    TEC_194_REPAIR_MODULE_PLUS_PLUS=194,
    TEC_195_RESERVED=195,
    TEC_196_RESERVED=196,
    TEC_197_RESERVED=197,
    TEC_198_RESERVED=198,
    TEC_199_RESERVED=199,
    TEC_200_SMALL_CALIBRE_TURRET=200,
    TEC_201_SMALL_CALIBRE_TURRET_ARMOUR_PLUS_25_PERCENT=201,
    TEC_202_SMALL_CALIBRE_TURRET_ARMOUR_PLUS_20_PERCENT=202,
    TEC_203_TURRET=203,
    TEC_204_TURRET_ARMOUR_PLUS_25_PERCENT=204,
    TEC_205_TURRET_ARMOUR_PLUS_20_PERCENT=205,
    TEC_206_LARGE_CALIBRE_TURRET=206,
    TEC_207_LARGE_CALIBRE_TURRET_ARMOUR_PLUS_25_PERCENT=207,
    TEC_208_LARGE_CALIBRE_TURRET_ARMOUR_PLUS_20_PERCENT=208,
    TEC_209_XENITE_MINE=209,
    TEC_210_IMPROVE_XENITE_MINE_1=210,
    TEC_211_IMPROVE_XENITE_MINE_2=211,
    TEC_212_TRITIUM_PUMP=212,
    TEC_213_IMPROVE_TRITIUM_PUMP_1=213,
    TEC_214_IMPROVE_TRITIUM_PUMP_2=214,
    TEC_215_TANK_OBSTACLE=215,
    TEC_216_WALL=216,
    TEC_217_HIGH_WALL=217,
    TEC_218_RESERVED=218,
    TEC_219_RESERVED=219,
    TEC_220_POWER_PLANT=220,
    TEC_221_RESERVED=221,
    TEC_222_RESERVED=222,
    TEC_223_RESERVED=223,
    TEC_224_RESERVED=224,
    TEC_225_RESERVED=225,
    TEC_226_RESERVED=226,
    TEC_227_RESERVED=227,
    TEC_228_RESERVED=228,
    TEC_229_RESERVED=229,
    TEC_230_RESERVED=230,
    TEC_231_RESERVED=231,
    TEC_232_RESERVED=232,
    TEC_233_RESERVED=233,
    TEC_234_RESERVED=234,
    TEC_235_RESERVED=235,
    TEC_236_RESERVED=236,
    TEC_237_RESERVED=237,
    TEC_238_RESERVED=238,
    TEC_239_RESERVED=239,
    TEC_240_RESERVED=240,
    TEC_241_RESERVED=241,
    TEC_242_RESERVED=242,
    TEC_243_RESERVED=243,
    TEC_244_RESERVED=244,
    TEC_245_RESERVED=245,
    TEC_246_RESERVED=246,
    TEC_247_RESERVED=247,
    TEC_248_RESERVED=248,
    TEC_249_RESERVED=249,
    TEC_250_RESERVED=250,
    TEC_251_RESERVED=251,
    TEC_252_RESERVED=252,
    TEC_253_RESERVED=253,
    TEC_254_RESERVED=254,
    TEC_255_RESERVED=255
};
typedef int PckTechnologyIdCatalog;

typedef int AiCandidateScore32;

typedef int FactionRuntimeRecordByteOffset;

typedef uint32_t ArmyPlacementContext;

typedef uint32_t FactionImageByteOffset;

typedef int AiSourceClassCount;

typedef uint32_t AiCandidateEntryKind;

struct AiKnowledgeParameters {
    uint32_t resource136DeficitScoreNumerator; 
    uint32_t specialSite14aBaseWeight; 
    uint32_t specialSite14cBaseWeight; 
    uint32_t structure14bBaseWeight; 
    uint32_t structure14dBaseWeight; 
    uint32_t workspace08Id14aDerivedWeight; 
    uint32_t workspace08OtherDerivedWeight; 
    uint32_t strategicClass12dBaseWeight; 
    uint32_t strategicClass12fTo132BaseWeight; 
    uint32_t armyVariantABaseWeight; 
    uint32_t armyVariantBUnexploredTerrainWeightCoefficient; 
    uint32_t armyVariantCBaseWeight; 
    uint32_t strategicClass141To143BaseWeight; 
    uint32_t workspace12BestCandidateBaseWeight; 
    uint32_t unknownParameterDwords14_15[2]; 
    uint32_t strategic12dAnd141To143AdditionalPlanningCapacity; 
    uint32_t strategic12fTo132AdditionalPlanningCapacity; 
    uint32_t unknownParameterDword18; 
    uint32_t resource136DeficitScoreDenominator; 
    uint32_t structure14bPrerequisite14aCountLimit; 
    uint32_t structure14dPrerequisite14cCountLimit; 
    uint32_t structure14bCountGapLimit; 
    uint32_t structure14dCountGapLimit; 
    XeniteAmountQ4 factionScaledTechnologyMinimumXeniteQ4;
    uint32_t unknownParameterDwords25_27[3];
    uint32_t specialSiteSeparationQuantumQ12; 
    uint32_t unknownParameterDwords29_31[3]; 
    uint32_t generalSiteMinimumAxisSeparationQ12;
    uint32_t generalSitePrimaryDistanceCapQ12; // General site score: (cap - distance to the primary workspace, >= 0) * generalSitePrimaryDistanceCoefficient.
    uint32_t generalSiteSecondaryDistanceCapQ12;
    uint32_t workspace05DistanceBiasQ12;
    uint32_t generalSiteVisibleHostileDistanceCapQ12; // General site score: min(distance to the nearest visible hostile, cap) * generalSiteVisibleHostileDistanceCoefficient.
    uint32_t generalSitePrimaryDistanceCoefficient;
    uint32_t generalSiteSecondaryDistanceCoefficient;
    uint32_t workspace05DistanceScaleQ12;
    uint32_t generalSiteVisibleHostileDistanceCoefficient;
    uint32_t unknownParameterDwords41_47[7];
    uint32_t flaggedSiteMinimumAxisSeparationQ12;
    uint32_t flaggedSitePrimaryDistanceCapQ12; // Flagged-site counterparts (+0xC4..+0xE0) of the general-site parameters; AiSiteCandidate_AddFlaggedCellIfSeparated reads them from the wrong base.
    uint32_t flaggedSiteSecondaryDistanceCapQ12;
    uint32_t factionAnchorDistanceBiasQ12;
    uint32_t flaggedSiteVisibleHostileDistanceCapQ12;
    uint32_t flaggedSitePrimaryDistanceCoefficient;
    uint32_t flaggedSiteSecondaryDistanceCoefficient;
    uint32_t factionAnchorDistanceScaleQ12;
    uint32_t flaggedSiteVisibleHostileDistanceCoefficient;
    uint32_t unknownParameterDwords57_66[10];
    uint32_t secondaryWorkspaceDistanceBiasQ12; 
    uint32_t unknownParameterDwords68_70[3]; 
    uint32_t secondaryWorkspaceDistanceScaleQ12; 
    uint32_t unknownParameterDwords72_79[8]; 
    uint32_t terrainFeatureMinimumAxisSeparationQ12; 
    uint32_t placementClearancePaddingQ12; 
    uint32_t specialClass12SecondaryWorkspaceDistanceThresholdQ12; 
    uint32_t specialClass12EntityDistanceBiasQ12; 
    uint32_t specialClass12Workspace02NearDistanceThresholdQ12; 
    uint32_t specialClass12Workspace08Field0cCoefficient; 
    uint32_t specialClass12SecondaryWorkspaceShortfallCoefficient; 
    uint32_t specialClass12EntityDistanceCoefficient; 
    uint32_t specialClass12Workspace02NearDistanceCoefficient; 
    uint32_t specialSiteMinimumWorkspaceDistanceQ12; 
    uint32_t unknownParameterDwords90_115[26];
    uint32_t strategicClass141Pressure2Coefficient; // AiStrategicClass_SelectPressureWeightedBuilding: building 0x141 score weight of AI pressure value 2 (+1).
    uint32_t strategicClass141Pressure3Coefficient;
    uint32_t strategicClass141Pressure4Coefficient;
    uint32_t unknownParameterDword119;
    uint32_t strategicClass142Pressure2Coefficient; // Same for building 0x142.
    uint32_t strategicClass142Pressure3Coefficient;
    uint32_t strategicClass142Pressure4Coefficient;
    uint32_t unknownParameterDword123;
    uint32_t strategicClass143Pressure2Coefficient; // Same for building 0x143.
    uint32_t strategicClass143Pressure3Coefficient;
    uint32_t strategicClass143Pressure4Coefficient;
    uint32_t unknownParameterDword127;
};

struct AiKnowledgeDataImage {
    struct AiKnowledgeParameters parameters; 
};

struct AiScoredSiteWorkspaceEntry {
    Q12 cellWorldXQ12; 
    Q12 cellWorldYQ12; 
    int score; 
    Ptr32<struct FieldGridCell> cell; 
};

struct AiTerrainFeatureWorkspaceEntry {
    Ptr32<struct FieldGridCell> cell; 
    uint32_t unresolved04; 
    PckArmyAssetIdCatalog armyAssetId; 
    int priority; 
};

struct AiRuntimeWorkspaceEntry {
    /* the model runtime (runtimePayload) of a MODEL node of the world owner list, NULL for a pending asset;
       its owning army is modelRuntime->ownerArmyRuntimeOrSavedOffset (AiPlanning_RebuildFactionWorkspaces) */
    Ptr32<struct ModelRuntimeSlot> modelRuntime; 
    PckArmyAssetIdCatalog armyAssetId; 
};

struct AiTargetWorkspaceEntry {
    Q12 worldXQ12; 
    Q12 worldYQ12; 
    Ptr32<struct ModelRuntimeSlot> modelRuntime; /* copied from a workspace-02 entry */
    Ptr32<struct ModelRuntimeNode> modelNode; /* its root model node */
};

struct AiLinkedDefinitionListView {
    uint8_t unresolved00_07[8]; 
    uint32_t childListCount; 
    ModelLinkedDefinitionListAddress32 childList0Address; 
    ModelLinkedDefinitionListAddress32 childList1Address; 
    uint8_t unresolved14_1F[12]; 
    PckModelDefinitionIdCatalog definitionIds[8]; 
};

struct AiArmyScoreWeights {
    int pressureCategoryWeights[8]; 
    int definitionValue60Weight; 
    int definitionValue0CWeight; 
    int armyRecord74Weight; 
    int armyRecord78Weight; 
    int armyRecord70Weight; 
    int nonzeroDefinition18Bonus; 
    int baseScore; 
};

typedef int AiTechnologyCandidateScore;

enum {
    AI_TECHNOLOGY_SCORE_DEFAULT_ZERO=0,
    AI_TECHNOLOGY_SCORE_FACTION_SCALED=1,
    AI_TECHNOLOGY_SCORE_BASE_VALUE_KIND2=2,
    AI_TECHNOLOGY_SCORE_RUNTIME_CLASS_COMPATIBLE=3,
    AI_TECHNOLOGY_SCORE_BASE_VALUE_KIND4=4,
    AI_TECHNOLOGY_SCORE_CATEGORY_COMPATIBLE=5
};
typedef int AiTechnologyCandidateScoreKind;

struct AiTechnologyPlanningCandidate {
    PckTechnologyIdCatalog technologyId00; 
    Ptr32<struct ModelRuntimeSlot> sourceModelRuntime04; /* the own structure's model runtime (workspace 00 entry) */
    AiTechnologyCandidateScoreKind scoreKind08; 
    uint32_t reserved0C; 
};

typedef uint32_t AiTechnologyPlanningCandidateCount;

typedef uint32_t AiTechnologyCategoryMask;

struct MdlDefinitionSemanticPrefix {
    AssetRecordByteCount byteSize; 
    uint32_t nameTextOffset; 
    PckModelDefinitionIdCatalog definitionId; 
    Q12 classSpeedQ12; 
    uint32_t yawMaxVelocityTurn16; 
    uint32_t pitchMaxVelocityTurn16; 
    uint32_t accelerationPerTick; // ModelDefinition.accelerationPerTick
    uint32_t yawAccelerationTurn16; 
    uint32_t pitchAccelerationTurn16; 
    int pitchMinimumTurn16; 
    int pitchMaximumTurn16; 
    PckShotDefinitionIdCatalog shotDefinitionId; 
    MdlReloadTicks reloadTicks; 
    uint32_t recoilRotationStepTurn16; 
    uint32_t recoilDurationOrIntershotTicks; 
    Q12 recoilTranslationQ12; 
    uint32_t actionVector0; 
    uint32_t actionVector1; 
    uint32_t visibilityRadius; // ModelDefinition.visibilityRadius
    ModelRuntimeClassId runtimeClassId; 
    uint32_t aimHeightOffsetQ12; // ModelDefinition.aimHeightOffsetQ12
    Q12 placementHeightOffsetQ12; // ModelDefinition.placementHeightOffsetQ12
    uint32_t waterEmitterEffectId; // ModelDefinition.waterEmitterEffectDefinitionReference (serialized id)
    uint32_t targetClassIndex; // ModelDefinition.targetClassIndex (indexes per-class shot impact effects and damage)
    uint32_t maximumHealth; // ModelDefinition.maximumHealth
    uint32_t rootNodeOffset; 
    uint32_t modelFlags; // ModelDefinition.modelFlags
    uint32_t unknown6C; 
    uint32_t visibilityHeightOffset; // ModelDefinition.visibilityHeightOffset
    uint32_t destroyedReplacementArmyAssetId; // ModelDefinition.destroyedReplacementArmyAssetId
    uint32_t positionedSoundGainQ15; 
    Q12 positionedSoundMaximumDistanceQ12; 
};

struct AiSecondaryWorkspaceDistanceSelection {
    AiCandidateScore32 score;
    Ptr32<struct AiTargetWorkspaceEntry> selectedEntry;
};
struct AiGeneralSiteDistanceSelection {
    AiCandidateScore32 score;
    Ptr32<struct AiScoredSiteWorkspaceEntry> selectedEntry;
};
struct AiStrategicClassSelection {
    uint32_t existingCountOrPressure;
    RuntimeToken selectedRuntimeToken;
};

struct AiStructureWorkspaceEntry {
    Ptr32<struct ModelRuntimeSlot> runtimeSlotAddressOrZero; // Nullable ModelRuntimeSlot (runtimePayload of a MODEL owner-list node, as AiRuntimeWorkspaceEntry.modelRuntime); runtime-only AI workspace.
    PckArmyAssetIdCatalog armyAssetId; // ARM registry identity.
};

typedef uint32_t AiCommandGenerationRightShiftBits;

/* Function-signature types used for function pointers, callbacks and method tables. */
typedef AiTechnologyCandidateScore AiTechnologyCandidateScoreCallback(FactionRuntimeIndex factionIndex, PckTechnologyIdCatalog technologyId, WorldRuntimeContext * worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_TYPES_H */
