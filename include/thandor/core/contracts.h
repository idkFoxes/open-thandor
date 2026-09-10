#ifndef THANDOR_CORE_CONTRACTS_H
#define THANDOR_CORE_CONTRACTS_H

/*
Core contracts shared by the curated submodules.
Q12: 0x1000 == 1.0. Q4 resource values use 16 units per displayed unit. Q5 research time uses 32 units per displayed unit.
FieldGridCell is 0x80 bytes. FLD flags/material at +0x50 and runtime occupancy at +0x70 are separate namespaces.
LEV file offsets and the loaded LevelAsset overlay are separate representations.
Faction runtime index, frontend player index, player-runtime ID, ARM ID, MDL ID and TEC ID are separate identity domains.
*/

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;
typedef int Q12;
typedef unsigned int UQ12;

typedef enum CuratedAssetMagic {
    CURATED_ASSET_MAGIC_TEC = 0x00636574,
    CURATED_ASSET_MAGIC_FNC = 0x00636E66,
    CURATED_ASSET_MAGIC_FLD = 0x00646C66,
    CURATED_ASSET_MAGIC_EFF = 0x00666665,
    CURATED_ASSET_MAGIC_PAL = 0x006C6170,
    CURATED_ASSET_MAGIC_MDL = 0x006C646D,
    CURATED_ASSET_MAGIC_SAM = 0x006D6173,
    CURATED_ASSET_MAGIC_FLM = 0x006D6C66,
    CURATED_ASSET_MAGIC_ROM = 0x006D6F72,
    CURATED_ASSET_MAGIC_ARM = 0x006D7261,
    CURATED_ASSET_MAGIC_SPR = 0x00727073,
    CURATED_ASSET_MAGIC_STR = 0x00727473,
    CURATED_ASSET_MAGIC_SHT = 0x00746873,
    CURATED_ASSET_MAGIC_LEV = 0x0076656C,
    CURATED_ASSET_MAGIC_GFX = 0x00786667
} CuratedAssetMagic;

typedef struct CuratedPckArchiveHeader {
    byte magic[4];
    dword archiveSize;
    dword version;
    dword format;
    dword packedDateTimeTriplets[6];
    byte reserved28[8];
    word producerName[32];
    word sourceName[32];
    dword entryCount;
    byte reservedB4[332];
} CuratedPckArchiveHeader; /* 0x200 */

typedef struct CuratedPckEntryHeader {
    word path[246];
    dword runtimePayloadOffset;
    dword unpackedSize;
    dword typeTag;
    dword packedSize;
    dword compressionMethod; /* 0 Huffman/RLE, 1 stored, 2 field-grid expansion */
} CuratedPckEntryHeader; /* 0x200 */

typedef enum CuratedFieldCellFlags {
    CURATED_FIELD_MATERIAL_ID_MASK             = 0x000000FF,
    CURATED_FIELD_RANDOM_VARIANT_MASK          = 0x00000700,
    CURATED_FIELD_XENITE_SUPPORT               = 0x00000800,
    CURATED_FIELD_TRITIUM_SUPPORT              = 0x00001000,
    CURATED_FIELD_RESOURCE_SUPPORT_MASK        = 0x00001800,
    CURATED_FIELD_FIRST_COLUMN_BOUNDARY        = 0x00002000,
    CURATED_FIELD_FIRST_ROW_BOUNDARY           = 0x00004000,
    CURATED_FIELD_INIT_CLEARED_8000            = 0x00008000,
    CURATED_FIELD_CONNECTED_REGION_VISITED     = 0x00010000,
    CURATED_FIELD_LAST_COLUMN_BOUNDARY         = 0x08000000,
    CURATED_FIELD_VISUAL_CLEARABLE_10000000    = 0x10000000,
    CURATED_FIELD_FLUID_RECEIVER_EXCLUDED      = 0x20000000,
    CURATED_FIELD_FLUID_SOURCE_EXCLUDED        = 0x40000000,
    CURATED_FIELD_LAST_ROW_BOUNDARY            = 0x80000000
} CuratedFieldCellFlags;

typedef struct CuratedFieldGridCell {
    byte runtime00_07[8];
    dword triangle0NormalAngles;
    byte runtime0C_3F[52];
    Q12 worldX;
    Q12 worldY;
    Q12 terrainHeight;
    Q12 waterSurfaceDelta;
    dword flagsAndMaterial;
    dword persistedAux54;
    byte runtime58_6F[24];
    qword occupancyMask;
    dword triangle1NormalAngles;
    dword runtime7C;
} CuratedFieldGridCell; /* 0x80 */

typedef struct CuratedFieldGridHeader200 {
    byte generatedAssetCommonPrefix[0xB0];
    dword fieldFlags;
    dword runtimeStateFlags;
    dword gridWidth;
    dword gridHeight;
    byte reservedC0_FF[0x40];
    word sourcePathUtf16[128];
} CuratedFieldGridHeader200; /* 0x200, cells follow */

typedef struct CuratedTerrainDirectionRecord {
    dword scaledSinCosA;
    dword scaledSinCosAHigh;
    dword scaledSinCosB;
    dword angleAccumulator;
    dword scaleA;
    dword scaleB;
    word rateA;
    word rateB;
    dword reserved1C;
} CuratedTerrainDirectionRecord; /* 0x20 */

typedef struct CuratedLevelAssetPathOffsets {
    dword levelPathOffset;
    dword groundTextureBasePathOffset;
    dword surfaceTextureBasePathOffset;
    dword skyTextureBasePathOffset;
    dword armyTextureBasePathOffset;
    dword shotTextureBasePathOffset;
    dword effectTextureBasePathOffset;
    dword endingMovieBasePathOffset;
    dword soundBasePathOffset;
    dword technologyPathOffset;
} CuratedLevelAssetPathOffsets; /* file +0xB0..+0xD7 */

typedef struct CuratedLevelPlayerSlotRecord {
    Q12 startCameraXQ12;
    Q12 startCameraYQ12;
    Q12 startCameraZQ12;
    dword startCameraMagnitudeQ12;
    dword packedHeadingLow16PitchHigh16;
    dword startXeniteQ4;
    dword startTritiumQ4;
    dword aiClassOrMode;
} CuratedLevelPlayerSlotRecord; /* 0x20, seven records at file +0x200 */

typedef struct CuratedTechnologyRecord {
    dword prerequisiteMasks[8];
    dword xeniteCostQ4;
    dword energyCostQ4;
    dword researchDurationQ5;
    dword dependencyTechnologyIndex;
    dword completionMessageResourceId;
    dword category;
    dword baseCandidateScore;
    dword reserved3C;
} CuratedTechnologyRecord; /* 0x40 */

typedef struct CuratedGameEntityTechnologyPayload {
    dword appliedResearchDurationQ5;
    dword entityValue24;
    dword energyCostQ4;
    dword xeniteCostQ4;
    byte reserved10_FB[0xEC];
} CuratedGameEntityTechnologyPayload; /* 0xFC class payload */

typedef struct CuratedAiKnowledgeParameters {
    dword resource136DeficitScoreNumerator;                    /* 0 */
    dword specialSite14aBaseWeight;                            /* 1 */
    dword specialSite14cBaseWeight;                            /* 2 */
    dword structure14bBaseWeight;                              /* 3 */
    dword structure14dBaseWeight;                              /* 4 */
    dword workspace08Id14aDerivedWeight;                       /* 5 */
    dword workspace08OtherDerivedWeight;                       /* 6 */
    dword strategicClass12dBaseWeight;                         /* 7 */
    dword strategicClass12fTo132BaseWeight;                    /* 8 */
    dword armyVariantABaseWeight;                              /* 9 */
    dword armyVariantBUnexploredTerrainWeightCoefficient;      /* 10 */
    dword armyVariantCBaseWeight;                              /* 11 */
    dword strategicClass141To143BaseWeight;                    /* 12 */
    dword workspace12BestCandidateBaseWeight;                  /* 13 */
    dword unknown14_15[2];
    dword strategic12dAnd141To143AdditionalPlanningCapacity;   /* 16 */
    dword strategic12fTo132AdditionalPlanningCapacity;         /* 17 */
    dword unknown18;
    dword resource136DeficitScoreDenominator;                  /* 19 */
    dword structure14bPrerequisite14aCountLimit;               /* 20 */
    dword structure14dPrerequisite14cCountLimit;               /* 21 */
    dword structure14bCountGapLimit;                           /* 22 */
    dword structure14dCountGapLimit;                           /* 23 */
    dword unknown24_27[4];
    dword specialSiteSeparationQuantumQ12;                     /* 28 */
    dword unknown29_31[3];
    dword generalSiteMinimumAxisSeparationQ12;                 /* 32 */
    dword unknown33;
    dword generalSiteSecondaryDistanceCapQ12;                  /* 34 */
    dword workspace05DistanceBiasQ12;                          /* 35 */
    dword unknown36_37[2];
    dword generalSiteSecondaryDistanceCoefficient;             /* 38 */
    dword workspace05DistanceScaleQ12;                         /* 39 */
    dword unknown40_47[8];
    dword flaggedSiteMinimumAxisSeparationQ12;                 /* 48 */
    dword unknown49_50[2];
    dword factionAnchorDistanceBiasQ12;                        /* 51 */
    dword unknown52_54[3];
    dword factionAnchorDistanceScaleQ12;                       /* 55 */
    dword unknown56_66[11];
    dword secondaryWorkspaceDistanceBiasQ12;                   /* 67 */
    dword unknown68_70[3];
    dword secondaryWorkspaceDistanceScaleQ12;                  /* 71 */
    dword unknown72_79[8];
    dword terrainFeatureMinimumAxisSeparationQ12;              /* 80 */
    dword placementClearancePaddingQ12;                        /* 81 */
    dword specialClass12SecondaryWorkspaceDistanceThresholdQ12;/* 82 */
    dword specialClass12EntityDistanceBiasQ12;                 /* 83 */
    dword specialClass12Workspace02NearDistanceThresholdQ12;   /* 84 */
    dword specialClass12Workspace08Field0cCoefficient;         /* 85 */
    dword specialClass12SecondaryWorkspaceShortfallCoefficient;/* 86 */
    dword specialClass12EntityDistanceCoefficient;             /* 87 */
    dword specialClass12Workspace02NearDistanceCoefficient;    /* 88 */
    dword specialSiteMinimumWorkspaceDistanceQ12;              /* 89 */
    dword unknown90_119[30];
    dword strategicClass141Weight;                             /* 120 */
    dword strategicClass142Weight;                             /* 121 */
    dword strategicClass143Weight;                             /* 122 */
    dword unknown123_127[5];
} CuratedAiKnowledgeParameters; /* 0x200 */

typedef struct CuratedUiTransferPacketHeader {
    dword packedTypeAndUnitCount;
    dword sequenceToken;
    dword senderContext;
    dword xorChecksum;
} CuratedUiTransferPacketHeader; /* 0x10 */

typedef struct CuratedUiCommandQueueRecord {
    dword packedCommandAndPlayerId;
    dword payloadDword04;
    dword payloadDword08;
    dword payloadDword0C;
} CuratedUiCommandQueueRecord; /* 0x10 */

typedef struct CuratedFrontendCommandPacketRecord {
    CuratedUiTransferPacketHeader header;
    CuratedUiCommandQueueRecord command;
} CuratedFrontendCommandPacketRecord; /* 0x20 */

typedef enum CuratedSimulationWheelPhase {
    CURATED_SIM_PHASE_FACTION_TECH_LIGHTING = 0,
    CURATED_SIM_PHASE_FLUID_FORWARD = 1,
    CURATED_SIM_PHASE_AI_CAPACITY_A = 2,
    CURATED_SIM_PHASE_TERRAIN_CLAMP_CALLBACKS = 3,
    CURATED_SIM_PHASE_INFLUENCE_REFRESH = 4,
    CURATED_SIM_PHASE_FLUID_REVERSE = 5,
    CURATED_SIM_PHASE_AI_CAPACITY_B = 6,
    CURATED_SIM_PHASE_OCCUPANCY_REBUILD = 7
} CuratedSimulationWheelPhase;

#endif /* THANDOR_CORE_CONTRACTS_H */
