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
   campaign row with the loaded campaign's fingerprint that CampaignCatalog_GroupByFingerprint picks as the base.
   The loaded campaign (a .cgn, a network bundle or the campagne entry of a save) carries no name, so the campaign
   rows' .cgn files are read natively and fingerprinted (cached per process).

   The campaign rows are read natively from their source files (level\campagne.dat plus the add-on files
   level\campagne00..99.dat merged by name, as ScenarioCatalog_Rebuild builds the catalog's campaign section),
   never from g_ScenarioCatalog: in a session that block is not always a catalog. The in-game save page frees it
   and puts its own save-row list there (InGameSaveGamePage_RebuildCatalog, original behaviour; the frontend
   rebuilds the catalog later), so after the player had opened that page a walk of the catalog's campaign rows
   built .cgn paths from garbage names (rich-text command units among them) and crashed in the path conversion.
   Row names are also checked (plain file names, CampaignProgress_RowNameIsFileName) before a path is built. */

#include <thandor/gameplay/session/campaign_progress.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
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
/* the most campaign rows the catalog could hold (its whole capacity in records) */
static constexpr size_t CAMPAIGN_PROGRESS_MAX_ROWS = SCENARIO_CATALOG_CAPACITY / SCENARIO_CATALOG_RECORD_STRIDE;
static_assert(sizeof(ScenarioCatalogDisplayRecord) == SCENARIO_CATALOG_RECORD_SIZE);
/* dwords per g_OldUnitPrimaryTable record and the ones a snapshot keeps (asset id, faction, X, Y, rotation) */
static constexpr uint32_t CAMPAIGN_PROGRESS_PRIMARY_RECORD_DWORDS = 8;

/* What a catalog row's .cgn gives the grouping; nullopt in the cache when it cannot be read. */
struct CampaignProgressRowInfo {
  CampaignFingerprint fingerprint;
  int32_t firstLevelId;
};

/* One campaign row of level\campagne.dat (after the add-on merge): its file name and title index (+0x50). */
struct CampaignProgressCatalogRow {
  uint16_t identifier[CAMPAIGN_PROGRESS_ROW_NAME_UNITS];
  int32_t titleIndex;
};

/* The base campaign of a loaded campaign: its section name and its first level. */
struct CampaignProgressBase {
  std::string name;
  int32_t firstLevelId;
};

/* Module data: the fingerprint of each catalog .cgn read so far, keyed by its lowercased path. The .cgn files do
   not change while the game runs. */
static std::map<std::u16string,std::optional<CampaignProgressRowInfo>> s_CampaignProgressRowCache;
/* Module data: the campaign rows, read once per process (the .dat files do not change while the game runs). */
static std::optional<std::vector<CampaignProgressCatalogRow>> s_CampaignProgressCatalogRows;

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

/* Tells whether a campaign row's name (up to CAMPAIGN_PROGRESS_ROW_NAME_UNITS units, zero-terminated or not) is a
   plain file name that may go into a path: not empty, printable ASCII or Latin-1 letters only (the units the path
   layer's narrow conversion, RichTextCommandStream_CopyToNarrow, copies as they are: no rich-text command units
   0x8000.., no control characters), no directory separator, drive colon or wildcard. */
static bool CampaignProgress_RowNameIsFileName(const uint16_t *rowName)

{
  size_t unit;

  for (unit = 0; unit < CAMPAIGN_PROGRESS_ROW_NAME_UNITS && rowName[unit] != 0; unit++) {
    const uint16_t character = rowName[unit];
    if (character < 0x20 || (character >= 0x7F && character < 0xA0) || character > 0xFF || character == '\\' ||
        character == '/' || character == ':' || character == '*' || character == '?' || character == '"' ||
        character == '<' || character == '>' || character == '|') {
      return false;
    }
  }
  return unit != 0;
}

/* The campaign row of one 0x100-byte campagne*.dat record at bytes + offset. */
static CampaignProgressCatalogRow CampaignProgress_RowAt(const std::vector<uint8_t> &bytes,size_t offset)

{
  ScenarioCatalogDisplayRecord record;
  CampaignProgressCatalogRow row{};

  memcpy(&record,bytes.data() + offset,sizeof record);
  memcpy(row.identifier,record.identifier,sizeof row.identifier);
  row.titleIndex = static_cast<int32_t>(record.titleTextResourceId);
  return row;
}

/* Merges the records of one campagne00..99.dat into rows by name the way ScenarioCatalog_MergeRecordsByName does:
   the whole 0x40-byte identifier compared, a match overwritten, a new name appended; a file without a whole record,
   or no rows yet, skips the file. */
static void CampaignProgress_MergeAddOnRows(std::vector<CampaignProgressCatalogRow> &rows,
                                            const std::vector<uint8_t> &bytes)

{
  const size_t recordCount = bytes.size() / SCENARIO_CATALOG_RECORD_SIZE;

  if (recordCount == 0 || rows.empty()) {
    return;
  }
  for (size_t recordIndex = 0; recordIndex < recordCount; recordIndex++) {
    const CampaignProgressCatalogRow row = CampaignProgress_RowAt(bytes,recordIndex * SCENARIO_CATALOG_RECORD_SIZE);
    auto existing = std::find_if(rows.begin(),rows.end(),[&row](const CampaignProgressCatalogRow &other) {
      return memcmp(other.identifier,row.identifier,sizeof row.identifier) == 0;
    });
    if (existing != rows.end()) {
      *existing = row;
    }
    else if (rows.size() < CAMPAIGN_PROGRESS_MAX_ROWS) {
      rows.push_back(row);
    }
  }
}

/* Reads a game file named by an ASCII path the native way (CampaignLevelInfo_ReadEntryNative). */
static std::optional<std::vector<uint8_t>> CampaignProgress_ReadDataFile(const char *asciiPath)

{
  uint16_t path[THANDOR_PATH_CAPACITY] = {};

  for (size_t unit = 0; asciiPath[unit] != 0 && unit + 1 < THANDOR_PATH_CAPACITY; unit++) {
    path[unit] = static_cast<uint8_t>(asciiPath[unit]);
  }
  return CampaignLevelInfo_ReadEntryNative(path);
}

/* The campaign rows of the catalog, read natively once per process: level\campagne.dat (package entry or loose
   file) and the add-on files level\campagne00..99.dat merged by name, in the order ScenarioCatalog_Rebuild gives
   the catalog's campaign section. */
static const std::vector<CampaignProgressCatalogRow> &CampaignProgress_CatalogRows()

{
  if (s_CampaignProgressCatalogRows.has_value()) {
    return *s_CampaignProgressCatalogRows;
  }
  std::vector<CampaignProgressCatalogRow> rows;
  std::optional<std::vector<uint8_t>> bytes = CampaignProgress_ReadDataFile("level\\campagne.dat");
  if (bytes.has_value()) {
    const size_t recordCount = std::min(bytes->size() / SCENARIO_CATALOG_RECORD_SIZE,CAMPAIGN_PROGRESS_MAX_ROWS);
    for (size_t recordIndex = 0; recordIndex < recordCount; recordIndex++) {
      rows.push_back(CampaignProgress_RowAt(*bytes,recordIndex * SCENARIO_CATALOG_RECORD_SIZE));
    }
  }
  if (rows.empty()) {
    Thandor_Log("campaign progress: level\\campagne.dat cannot be read or holds no campaign");
  }
  else {
    for (int addOn = 0; addOn < 100; addOn++) {
      char addOnPath[32];
      snprintf(addOnPath,sizeof addOnPath,"level\\campagne%02d.dat",addOn);
      std::optional<std::vector<uint8_t>> addOnBytes = CampaignProgress_ReadDataFile(addOnPath);
      if (addOnBytes.has_value()) {
        CampaignProgress_MergeAddOnRows(rows,*addOnBytes);
      }
    }
  }
  s_CampaignProgressCatalogRows = std::move(rows);
  return *s_CampaignProgressCatalogRows;
}

/* The fingerprint and first level of the .cgn of one catalog row (cached), nullopt when it cannot be read or its
   name is not a plain file name (CampaignProgress_RowNameIsFileName). */
static std::optional<CampaignProgressRowInfo> CampaignProgress_ReadRow(const uint16_t *rowName)

{
  uint16_t name[CAMPAIGN_PROGRESS_ROW_NAME_UNITS + 1] = {};
  uint16_t path[THANDOR_PATH_CAPACITY] = {};
  std::u16string key;

  if (!CampaignProgress_RowNameIsFileName(rowName)) {
    Thandor_Log("campaign progress: a campaign row without a plain file name skipped");
    return std::nullopt;
  }
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

/* The base campaign (plan 5.6) of the loaded campaign among the campaign rows (CampaignProgress_CatalogRows), or
   nullopt (one log line) when no row has its fingerprint or the base row's name is not a valid section name. */
static std::optional<CampaignProgressBase> CampaignProgress_FindBase(const CampaignAsset *campaign)

{
  const CampaignFingerprint fingerprint = CampaignAsset_Fingerprint(campaign);
  std::vector<CampaignGroupCandidate> candidates;
  std::vector<const CampaignProgressCatalogRow *> rows;

  for (const CampaignProgressCatalogRow &row : CampaignProgress_CatalogRows()) {
    std::optional<CampaignProgressRowInfo> info = CampaignProgress_ReadRow(row.identifier);
    if (info.has_value()) {
      candidates.push_back(CampaignGroupCandidate{info->fingerprint,info->firstLevelId,row.titleIndex});
      rows.push_back(&row);
    }
  }
  for (const CampaignGroup &group : CampaignCatalog_GroupByFingerprint(candidates)) {
    if (!(candidates[group.baseIndex].fingerprint == fingerprint)) {
      continue;
    }
    CampaignProgressBase base;
    base.firstLevelId = candidates[group.baseIndex].firstLevelId;
    for (size_t unit = 0; unit < CAMPAIGN_PROGRESS_ROW_NAME_UNITS && rows[group.baseIndex]->identifier[unit] != 0 &&
                            rows[group.baseIndex]->identifier[unit] != '.';
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

/* The section of the base campaign in progress, with its fingerprint set when it had none. A section recorded with
   other campaign records (the campaign data changed, plan 4.4) is reset first: its wins, unlocked levels and
   snapshots are dropped and the new fingerprint is stored (one log line). Never nullptr. */
static CampaignProgressSection *CampaignProgress_SectionForWrite(CampaignProgress &progress,
                                                                 const CampaignProgressBase &base,
                                                                 const CampaignAsset *campaign,bool *outChanged)

{
  const CampaignFingerprint fingerprint = CampaignAsset_Fingerprint(campaign);
  const CampaignProgressFingerprint expected{fingerprint.crc32,fingerprint.recordCount};
  CampaignProgressSection &section = CampaignProgress_GetOrAddSection(progress,base.name);

  if (section.fingerprint.has_value() && !(*section.fingerprint == expected)) {
    Thandor_Log("campaign progress: section \"%s\" was recorded with other campaign records (fingerprint %08x %d, "
                "now %08x %d); reset",base.name.c_str(),section.fingerprint->crc32,section.fingerprint->recordCount,
                expected.crc32,expected.recordCount);
    section = CampaignProgressSection{};
    section.name = base.name;
  }
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
