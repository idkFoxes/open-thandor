/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SCENARIO_TYPES_H
#define THANDOR_ASSETS_SCENARIO_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */

/* Types (split out by tools/dev/split_types.py). */

typedef union Utf16DecimalDigitPair4 Utf16DecimalDigitPair4, *PUtf16DecimalDigitPair4;
typedef struct ScenarioCampaignDataPathTemplate2A ScenarioCampaignDataPathTemplate2A, *PScenarioCampaignDataPathTemplate2A;
typedef struct ScenarioLevelDataPathTemplate24 ScenarioLevelDataPathTemplate24, *PScenarioLevelDataPathTemplate24;
typedef struct ScenarioCatalogHeader ScenarioCatalogHeader, *PScenarioCatalogHeader;
typedef struct ScenarioCatalogRecord ScenarioCatalogRecord, *PScenarioCatalogRecord;
typedef struct ScenarioCatalogSaveRecord ScenarioCatalogSaveRecord, *PScenarioCatalogSaveRecord;

union Utf16DecimalDigitPair4 {
    uint16_t codeUnits[2]; 
    uint32_t packedDigits; 
};

typedef uint32_t ScenarioCatalogByteOffset;

typedef uint32_t ScenarioCatalogSourceByteCount;

typedef uint32_t ScenarioCatalogRecordCount;
#pragma pack(push, 1) /* packed layout: no alignment padding */
struct ScenarioCampaignDataPathTemplate2A {
    uint16_t prefixCodeUnits[14]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    uint16_t suffixCodeUnits[5]; 
};
#pragma pack(pop)
#pragma pack(push, 1) /* packed layout: no alignment padding */
struct ScenarioLevelDataPathTemplate24 {
    uint16_t prefixCodeUnits[11]; 
    union Utf16DecimalDigitPair4 decimalDigits; 
    uint16_t suffixCodeUnits[5]; 
};
#pragma pack(pop)

struct ScenarioCatalogHeader {
    ScenarioCatalogByteOffset levelRecordsOffset; 
    ScenarioCatalogByteOffset campaignRecordsOffset; 
    ScenarioCatalogByteOffset saveRecordsOffset; 
    ScenarioCatalogRecordCount levelRecordCount; 
    ScenarioCatalogRecordCount campaignRecordCount; 
    ScenarioCatalogRecordCount saveRecordCount; 
};

struct ScenarioCatalogRecord {
    uint16_t identifier[32]; 
    uint8_t metadata40_BF[128]; 
    uint16_t timestampText[32]; 
};

struct ScenarioCatalogSaveRecord {
    uint16_t identifier[32];
    uint8_t metadata40_6F[48];
    int levelTitleTextId; // Stored as the level title index; ScenarioCatalog_Rebuild adds TEXT_ID_LEVEL_TITLE_BASE.
    uint8_t metadata74_8F[28];
    int campaignTitleTextId; // Stored as the campaign title index, negative = not a campaign save; ScenarioCatalog_Rebuild adds TEXT_ID_CAMPAIGN_TITLE_BASE.
    uint8_t metadata94_BF[44];
    uint16_t timestampText[32];
};

#endif /* THANDOR_ASSETS_SCENARIO_TYPES_H */
