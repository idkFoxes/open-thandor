/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/campaign_level_info.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_CAMPAIGN_LEVEL_INFO_H
#define THANDOR_UI_FRONTEND_CAMPAIGN_LEVEL_INFO_H

#include <cstdint>
#include <vector>
#include <thandor/assets/scenario/catalog.h>
#include <thandor/ui/frontend/campaign_map.h>

/* open-thandor (campaign map, docs/plans/campaign_map.md 5.1, 5.2, 5.4): what the map needs from the level file
   of one campaign level record. Read natively (no arena) and cached per level file. */
struct CampaignLevelFileInfo {
    int32_t levelId = 0;          /* CampaignLevelRecord.levelId of the record */
    bool fileExists = false;      /* level\<levelFileName>.lev found in a mounted package or as a loose file */
    bool headerValid = false;     /* the LEV header passes the loader checks; titleTextIndex is meaningful */
    bool triggersValid = false;   /* the level script passes InGameLevelScript_Validate; winSelectionMask is
                                     meaningful (otherwise the map falls back to all selections 1..7, 5.2) */
    int32_t titleTextIndex = 0;   /* LevelAssetHeader.titleTextResourceIndex (LEV +0x170) */
    uint32_t winSelectionMask = 0; /* OR of 1 << endMovieSelectionIndex (0..7) over the active end triggers
                                      with movieVariantSelector 1 (the win rule of 4.3) */
};

/* One entry per level record of campaign, in record order. The caller passes a campaign that passed
   CampaignAsset_Fits (levelRecordCount records are readable); nullptr or a record count below 1 gives an empty
   result. Each level file is read once per process (package entry through Package_FindEntryAcrossMounts and
   Package_DecodeEntryInto, compression methods 0 and 1 only, else the loose file through
   FileSystem_ReadWholeFileNative) and then served from the cache. Uses native memory only, never g_MemoryApi.
   A dev-tools build logs every result when OPEN_THANDOR_CAMPAIGN_LEVEL_INFO_DUMP is set (compare with
   python tools/data/cgn.py dump <level dir> --triggers). */
std::vector<CampaignLevelFileInfo> CampaignLevelInfo_Read(const CampaignAsset *campaign);

/* Forgets every cached level (e.g. after the mounted packages changed). */
void CampaignLevelInfo_ClearCache();

/* The graph view of one read result (CampaignGraph_Build input): a level counts as existing only with a valid
   header; an unreadable script gives CAMPAIGN_WIN_SELECTIONS_UNKNOWN (all selections 1..7 win, 5.2). */
inline CampaignLevelInfo CampaignLevelFileInfo_ToGraphInfo(const CampaignLevelFileInfo &file) {
    CampaignLevelInfo info{};
    info.fileExists = file.fileExists && file.headerValid;
    info.titleTextIndex = file.headerValid ? file.titleTextIndex : -1;
    info.winSelectionMask = file.triggersValid ? file.winSelectionMask : CAMPAIGN_WIN_SELECTIONS_UNKNOWN;
    return info;
}

#endif /* THANDOR_UI_FRONTEND_CAMPAIGN_LEVEL_INFO_H */
