/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SCENARIO_CATALOG_H
#define THANDOR_ASSETS_SCENARIO_CATALOG_H

#include <thandor/assets/package/types.h>
#include <thandor/assets/scenario/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Scenario catalog (g_ScenarioCatalog): ScenarioCatalogHeader followed by the level, campaign and save
   records (0x100 bytes each). Every counted record advances the following section offsets and
   g_ScenarioCatalogUsedBytes by 0x104, although the records are packed 0x100 apart. */
inline constexpr int SCENARIO_CATALOG_CAPACITY = 0x30000;
inline constexpr int SCENARIO_CATALOG_HEADER_SIZE = 0x18;
inline constexpr int SCENARIO_CATALOG_RECORD_SIZE = 0x100;
inline constexpr int SCENARIO_CATALOG_RECORD_STRIDE = 0x104;
/* Text resource ids: a level title is 0x2230 + its title index, a campaign title 0x2220 + its index (the
   level text page aliases, see TextResourcePage_LoadCompatibilityAliases). */
inline constexpr int TEXT_ID_LEVEL_TITLE_BASE = 0x2230;
inline constexpr int TEXT_ID_CAMPAIGN_TITLE_BASE = 0x2220;

/* Campaign asset (level\<name>.cgn, g_FrontendLoadedCampaignAsset): a 0x200-byte header followed by
   levelRecordCount level records of 0x180 bytes. The original walks the records with a cursor that starts at
   the asset base and advances by 0x180, reading the fields at cursor + 0x200 + field offset; the port keeps
   that cursor and reads levels[0] of a CampaignAsset view at the cursor (= record i of the real asset). */
typedef struct CampaignLevelRecord {
    int32_t successorLevelIds[8];        /* +0x000 next level id per end selection (g_EndMovieSelectionIndex), <0 = end */
    int32_t endMovieNumbersVariant[8];   /* +0x020 end movie number per end selection, nonzero g_EndMovieVariantIndex */
    int32_t endMovieNumbers[8];          /* +0x040 end movie number per end selection, g_EndMovieVariantIndex 0 */
    int32_t exitZoneCenterX[8];          /* +0x060 per faction: carry-over exit zone centre X (Q12) */
    int32_t exitZoneCenterY[8];          /* +0x080 per faction: exit zone centre Y */
    int32_t exitZoneRadius[8];           /* +0x0A0 per faction: exit zone radius (<= 0 = none) */
    int32_t exitZoneDestinationX[8];     /* +0x0C0 per faction: where the carried units appear, X */
    int32_t exitZoneDestinationY[8];     /* +0x0E0 per faction: destination Y */
    int32_t levelId;                     /* +0x100 */
    uint32_t carryOverMask;              /* +0x104 one carry-over bit per end selection */
    uint32_t skipMask;                   /* +0x108 one skip bit per end selection */
    uint16_t levelFileName[58];          /* +0x10C UTF-16 level file name (level\<name>.lev) */
} CampaignLevelRecord;

typedef struct CampaignAsset {
    AssetMagic magic;               /* +0x00 */
    PckDecodedByteCount decodedSizeBytes; /* +0x04 allocation/decoded size of the whole asset */
    uint8_t reserved08_B3[0xac];
    int32_t firstLevelId;                /* +0xB4 */
    int32_t levelRecordCount;            /* +0xB8 */
    uint8_t reservedBC_C3[8];
    int32_t currentLevelId;              /* +0xC4 */
    uint8_t reservedC8_1FF[0x138];
    CampaignLevelRecord levels[1];       /* +0x200, levelRecordCount records */
} CampaignAsset;

void ScenarioCatalog_Rebuild();

void ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
                                                 uint32_t unusedArg3);

ScenarioCatalogRecordCount ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords,
          ScenarioCatalogRecordCount maxRecordCount);

extern ScenarioCatalogHeader *g_ScenarioCatalog;
extern uint32_t g_ScenarioCatalogUsedBytes;
extern uint16_t g_SaveSvePatternUtf16[11];

#endif /* THANDOR_ASSETS_SCENARIO_CATALOG_H */
