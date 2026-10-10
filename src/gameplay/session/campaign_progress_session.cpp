/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/campaign_progress_session.cpp
 * Project code (not in the original game)
 */

/* The session hooks of the campaign progress (docs/plans/campaign_map.md 4.3, 4.5, 7.5): a win recorded when a
   session ends through the end trigger, the carry-over snapshot taken from the old-unit tables the original
   carry-over just rebuilt, and the levels unlocked by starting them. Everything here only reads game state and
   uses native memory and native file calls: the arena (g_MemoryApi) and the simulation never see it, so the
   arena order and every later session stay as they were (plan section 3).

   The progress section of a campaign is keyed by the .cgn base name of its base campaign (plan 5.6): the
   catalog row with the loaded campaign's fingerprint that CampaignCatalog_GroupByFingerprint picks as the base.
   The loaded campaign (a .cgn, a network bundle or the campagne entry of a save) carries no name, so the catalog
   rows' .cgn files are read natively and fingerprinted (cached per process). */

#include <thandor/gameplay/session/campaign_progress.h>

#include <stdlib.h>
#include <string.h>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/frontend/campaign_level_info.h>
#include <thandor/ui/frontend/campaign_map.h>

/* code units of ScenarioCatalogDisplayRecord.identifier (the .cgn base name of a campaign row) */
static constexpr size_t CAMPAIGN_PROGRESS_ROW_NAME_UNITS =
    sizeof(ScenarioCatalogDisplayRecord::identifier) / sizeof(ScenarioCatalogDisplayRecord::identifier[0]);
/* dwords per g_OldUnitPrimaryTable record and the ones a snapshot keeps (asset id, faction, X, Y, rotation) */
static constexpr uint32_t CAMPAIGN_PROGRESS_PRIMARY_RECORD_DWORDS = 8;

/* What a catalog row's .cgn gives the grouping; nullopt in the cache when it cannot be read. */
struct CampaignProgressRowInfo {
  CampaignFingerprint fingerprint;
  int32_t firstLevelId;
};

/* The base campaign of a loaded campaign: its section name and its first level. */
struct CampaignProgressBase {
  std::string name;
  int32_t firstLevelId;
};

/* Module data: the fingerprint of each catalog .cgn read so far, keyed by its lowercased path. The .cgn files do
   not change while the game runs. */
static std::map<std::u16string,std::optional<CampaignProgressRowInfo>> s_CampaignProgressRowCache;

bool CampaignProgress_IsWin(const CampaignLevelRecord &levelRecord,int32_t levelId,uint32_t endMovieSelectionIndex,
                            uint32_t endMovieVariantIndex)

{
  return endMovieVariantIndex != 0 && endMovieSelectionIndex < CAMPAIGN_PROGRESS_SELECTION_COUNT &&
         levelRecord.successorLevelIds[endMovieSelectionIndex] != levelId;
}

CampaignProgressCarry CampaignProgress_TakeCarry(int32_t sourceLevelId,int32_t selection,const uint32_t *primaryTable,
                                                 uint32_t recordCount,const uint32_t *secondaryTable)

{
  CampaignProgressCarry carry{};
  const uint32_t *record;
  uint32_t recordIndex;

  carry.sourceLevelId = sourceLevelId;
  carry.selection = selection;
  if (recordCount > static_cast<uint32_t>(CAMPAIGN_PROGRESS_UNIT_CAPACITY)) {
    recordCount = CAMPAIGN_PROGRESS_UNIT_CAPACITY;
  }
  carry.units.reserve(recordCount);
  for (recordIndex = 0; recordIndex < recordCount; recordIndex++) {
    record = primaryTable + recordIndex * CAMPAIGN_PROGRESS_PRIMARY_RECORD_DWORDS;
    carry.units.push_back(CampaignProgressUnit{static_cast<int32_t>(record[0]),static_cast<int32_t>(record[1]),
                                               static_cast<int32_t>(record[2]),static_cast<int32_t>(record[3]),
                                               record[4]});
  }
  for (size_t word = 0; word < carry.techMasks.size(); word++) {
    carry.techMasks[word] = secondaryTable[word];
  }
  return carry;
}

/* The record of levelId (the first one, like the engine's walks), or nullptr. */
static const CampaignLevelRecord *CampaignProgress_FindRecord(const CampaignAsset *campaign,int32_t levelId)

{
  for (int32_t recordIndex = 0; recordIndex < campaign->levelRecordCount; recordIndex++) {
    if (campaign->levels[recordIndex].levelId == levelId) {
      return &campaign->levels[recordIndex];
    }
  }
  return nullptr;
}

/* The fingerprint and first level of the .cgn of one catalog row (cached), nullopt when it cannot be read. */
static std::optional<CampaignProgressRowInfo> CampaignProgress_ReadRow(const uint16_t *rowName)

{
  uint16_t name[CAMPAIGN_PROGRESS_ROW_NAME_UNITS + 1] = {};
  uint16_t path[THANDOR_PATH_CAPACITY] = {};
  std::u16string key;

  memcpy(name,rowName,CAMPAIGN_PROGRESS_ROW_NAME_UNITS * sizeof(uint16_t));
  /* level\<name>.cgn as FrontendScenarioSession_LoadOrRequestCampaignBundle builds it */
  WidePath_CombineDirectoryAndLeaf(path,name,g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_CGN,path,THANDOR_PATH_CAPACITY);
  for (const uint16_t *unit = path; *unit != 0; unit++) {
    key.push_back(static_cast<char16_t>((*unit >= 'A' && *unit <= 'Z') ? *unit + ('a' - 'A') : *unit));
  }
  auto cached = s_CampaignProgressRowCache.find(key);
  if (cached != s_CampaignProgressRowCache.end()) {
    return cached->second;
  }
  std::optional<CampaignProgressRowInfo> info;
  std::optional<std::vector<uint8_t>> bytes = CampaignLevelInfo_ReadEntryNative(path);
  if (bytes.has_value()) {
    const CampaignAsset *asset = reinterpret_cast<const CampaignAsset *>(bytes->data());
    if (CampaignAsset_Fits(asset,static_cast<uint32_t>(bytes->size()))) {
      info = CampaignProgressRowInfo{CampaignAsset_Fingerprint(asset),asset->firstLevelId};
    }
  }
  if (!info.has_value()) {
    Thandor_Log("campaign progress: catalog campaign \"%ls\" cannot be read, not grouped",
                reinterpret_cast<const wchar_t *>(path));
  }
  s_CampaignProgressRowCache.emplace(key,info);
  return info;
}

/* The base campaign (plan 5.6) of the loaded campaign among the rows of g_ScenarioCatalog, or nullopt (one log
   line) when no row has its fingerprint or the base row's name is not a valid section name. */
static std::optional<CampaignProgressBase> CampaignProgress_FindBase(const CampaignAsset *campaign)

{
  const CampaignFingerprint fingerprint = CampaignAsset_Fingerprint(campaign);
  std::vector<CampaignGroupCandidate> candidates;
  std::vector<const ScenarioCatalogDisplayRecord *> rows;
  const ScenarioCatalogDisplayRecord *row;
  uint32_t rowIndex;

  if (g_ScenarioCatalog != nullptr) {
    row = Thandor_At<ScenarioCatalogDisplayRecord>(g_ScenarioCatalog,g_ScenarioCatalog->campaignRecordsOffset);
    for (rowIndex = 0; rowIndex < g_ScenarioCatalog->campaignRecordCount; rowIndex++) {
      std::optional<CampaignProgressRowInfo> info = CampaignProgress_ReadRow(row->identifier);
      if (info.has_value()) {
        candidates.push_back(CampaignGroupCandidate{info->fingerprint,info->firstLevelId,
                                                    static_cast<int32_t>(row->titleTextResourceId)});
        rows.push_back(row);
      }
      row = Thandor_At<ScenarioCatalogDisplayRecord>(row,SCENARIO_CATALOG_RECORD_SIZE);
    }
  }
  for (const CampaignGroup &group : CampaignCatalog_GroupByFingerprint(candidates)) {
    if (!(candidates[group.baseIndex].fingerprint == fingerprint)) {
      continue;
    }
    CampaignProgressBase base;
    base.firstLevelId = candidates[group.baseIndex].firstLevelId;
    for (size_t unit = 0; unit < CAMPAIGN_PROGRESS_ROW_NAME_UNITS && rows[group.baseIndex]->identifier[unit] != 0;
         unit++) {
      const uint16_t character = rows[group.baseIndex]->identifier[unit];
      base.name.push_back(character < 0x80 ? static_cast<char>(character) : '?');
    }
    if (!CampaignProgress_IsValidName(base.name)) {
      Thandor_Log("campaign progress: campaign name \"%s\" is not a valid key, progress not recorded",
                  base.name.c_str());
      return std::nullopt;
    }
    return base;
  }
  Thandor_Log("campaign progress: no catalog campaign has fingerprint %08x %d, progress not recorded",
              fingerprint.crc32,fingerprint.recordCount);
  return std::nullopt;
}

/* The section of the base campaign in progress, with its fingerprint set when it had none; nullptr (one log line)
   when the section was recorded with other campaign records (it is kept as it is, plan 4.4). */
static CampaignProgressSection *CampaignProgress_SectionForWrite(CampaignProgress &progress,
                                                                 const CampaignProgressBase &base,
                                                                 const CampaignAsset *campaign,bool *outChanged)

{
  const CampaignFingerprint fingerprint = CampaignAsset_Fingerprint(campaign);
  const CampaignProgressFingerprint expected{fingerprint.crc32,fingerprint.recordCount};
  const CampaignProgressSection *existing = CampaignProgress_FindSection(progress,base.name);

  if (existing != nullptr && existing->fingerprint.has_value() && !(*existing->fingerprint == expected)) {
    Thandor_Log("campaign progress: section \"%s\" was recorded with other campaign records (fingerprint %08x %d, "
                "now %08x %d); kept, nothing recorded",base.name.c_str(),existing->fingerprint->crc32,
                existing->fingerprint->recordCount,expected.crc32,expected.recordCount);
    return nullptr;
  }
  CampaignProgressSection &section = CampaignProgress_GetOrAddSection(progress,base.name);
  if (!section.fingerprint.has_value()) {
    section.fingerprint = expected;
    *outChanged = true;
  }
  return &section;
}

/* The level ids a recorded win of section unlocks (successorLevelIds of every selection won), plan 4.3. */
static bool CampaignProgress_IsSuccessorOfWin(const CampaignProgressSection &section,const CampaignAsset *campaign,
                                              int32_t levelId)

{
  for (const auto &[wonLevelId,wins] : section.wins) {
    const CampaignLevelRecord *record = CampaignProgress_FindRecord(campaign,wonLevelId);
    if (record == nullptr) {
      continue;
    }
    for (int selection = 0; selection < CAMPAIGN_PROGRESS_SELECTION_COUNT; selection++) {
      if ((wins.selectionMask & (1u << selection)) != 0 && record->successorLevelIds[selection] == levelId) {
        return true;
      }
    }
  }
  return false;
}

void CampaignProgress_RecordSessionEnd(const CampaignAsset *campaign,uint32_t endMovieSelectionIndex,
                                       uint32_t endMovieVariantIndex)

{
  const CampaignLevelRecord *record;
  int32_t levelId;
  int32_t successorLevelId;
  int32_t selection;
  bool changed = false;
  bool carried = false;

  if (campaign == nullptr) {
    return;
  }
  levelId = campaign->currentLevelId;
  record = CampaignProgress_FindRecord(campaign,levelId);
  if (record == nullptr || !CampaignProgress_IsWin(*record,levelId,endMovieSelectionIndex,endMovieVariantIndex)) {
    return; /* a loss or a retry records nothing */
  }
  std::optional<CampaignProgressBase> base = CampaignProgress_FindBase(campaign);
  if (!base.has_value()) {
    return;
  }
  CampaignProgress progress = CampaignProgress_LoadCurrent();
  CampaignProgressSection *section = CampaignProgress_SectionForWrite(progress,*base,campaign,&changed);
  if (section == nullptr) {
    return;
  }
  selection = static_cast<int32_t>(endMovieSelectionIndex);
  CampaignProgressSection_RecordWin(*section,levelId,selection);
  /* the snapshot of 4.5: only when this selection carries units into a level of the campaign, as the rebuild that
     just ran wrote the tables for it (carry bit set, skip bit clear) */
  successorLevelId = record->successorLevelIds[selection];
  if (((record->carryOverMask >> selection) & 1) != 0 && ((record->skipMask >> selection) & 1) == 0 &&
      successorLevelId > 0 && CampaignProgress_FindRecord(campaign,successorLevelId) != nullptr &&
      g_OldUnitPrimaryTable != nullptr && g_OldUnitSecondaryTable != nullptr) {
    section->carries[successorLevelId] = CampaignProgress_TakeCarry(levelId,selection,g_OldUnitPrimaryTable,
                                                                   g_OldUnitRecordCount,g_OldUnitSecondaryTable);
    carried = true;
  }
  Thandor_Log("campaign progress: %s level %d won with selection %d%s",base->name.c_str(),levelId,selection,
              carried ? ", carry-over snapshot taken" : "");
  if (carried) {
    Thandor_Log("campaign progress: snapshot for level %d holds %u units",successorLevelId,
                static_cast<unsigned>(section->carries[successorLevelId].units.size()));
  }
  CampaignProgress_SaveCurrent(progress);
}

void CampaignProgress_NoteLevelStarted(const CampaignAsset *campaign)

{
  int32_t levelId;
  bool changed = false;

  if (campaign == nullptr) {
    return;
  }
  levelId = campaign->currentLevelId;
  if (levelId <= 0 || CampaignProgress_FindRecord(campaign,levelId) == nullptr) {
    return;
  }
  std::optional<CampaignProgressBase> base = CampaignProgress_FindBase(campaign);
  if (!base.has_value()) {
    return;
  }
  CampaignProgress progress = CampaignProgress_LoadCurrent();
  CampaignProgressSection *section = CampaignProgress_SectionForWrite(progress,*base,campaign,&changed);
  if (section == nullptr) {
    return;
  }
  if (levelId != base->firstLevelId && !section->wins.contains(levelId) &&
      !section->unlockedLevelIds.contains(levelId) && !CampaignProgress_IsSuccessorOfWin(*section,campaign,levelId)) {
    section->unlockedLevelIds.insert(levelId);
    Thandor_Log("campaign progress: %s level %d unlocked by its start",base->name.c_str(),levelId);
    changed = true;
  }
  if (changed) {
    CampaignProgress_SaveCurrent(progress);
  }
}
