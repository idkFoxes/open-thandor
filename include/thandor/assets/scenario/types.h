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

union Utf16DecimalDigitPair4;
struct ScenarioCampaignDataPathTemplate2A;
struct ScenarioLevelDataPathTemplate24;
struct ScenarioCatalogHeader;
struct ScenarioCatalogRecord;
struct ScenarioCatalogSaveRecord;

union Utf16DecimalDigitPair4 {
    uint16_t codeUnits[2]; 
    uint32_t packedDigits; 
};

using ScenarioCatalogByteOffset = uint32_t;

using ScenarioCatalogSourceByteCount = uint32_t;

using ScenarioCatalogRecordCount = uint32_t;
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
    uint32_t autosaveMarker; // +0x74 open-thandor: INGAME_SAVE_AUTOSAVE_MARKER in an autosave (InGameSavePackageHeader.autosaveMarker), else 0.
    uint8_t metadata78_8F[24];
    int campaignTitleTextId; // Stored as the campaign title index, negative = not a campaign save; ScenarioCatalog_Rebuild adds TEXT_ID_CAMPAIGN_TITLE_BASE.
    uint16_t autosaveFileLeaf[22]; // +0x94 open-thandor, in the catalog only: an autosave's file name while identifier holds its list text (Autosave_PrepareCatalogRecord); 0 in the file.
    uint16_t timestampText[24]; // +0xC0 "date, time" (InGameSavePackageHeader.dateTimeTextUtf16)
    uint32_t packedDate; // +0xF0 (year << 16) | (month << 8) | day; the lists sort by the pair packedDate, packedTime
    uint32_t packedTime; // +0xF4 (hour << 16) | (minute << 8) | second
    uint8_t metadataF8_FF[8];
};

#endif /* THANDOR_ASSETS_SCENARIO_TYPES_H */
