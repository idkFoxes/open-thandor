/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/campaign_start.cpp
 * Project code (not in the original game)
 */

#include <thandor/gameplay/session/campaign_start.h>

#include <thandor/thandor.h>
#include <thandor/gameplay/session/campaign_carryover.h>
#include <thandor/ui/frontend/campaign_level_info.h>
#include <thandor/ui/frontend/campaign_map.h>
#include <thandor/ui/ingame/savegame_page.h>
#include <thandor/world/terrain/field_sampling.h>

/* Starting a campaign at a mission from the campaign map (docs/plans/campaign_map.md 7.3, 7.4, 4.5). */

/* "campaign_progress.txt", the leaf of the progress file in save\ (4.4) */
static uint16_t g_CampaignProgressFileLeafUtf16[] = {'c','a','m','p','a','i','g','n','_','p','r','o','g','r','e',
                                                     's','s','.','t','x','t',0};

/* dwords per g_OldUnitPrimaryTable record (0x20 bytes) */
static constexpr int CAMPAIGN_START_PRIMARY_RECORD_DWORDS = 8;

static_assert(CAMPAIGN_PROGRESS_UNIT_CAPACITY == OLD_UNIT_PRIMARY_RECORD_CAPACITY);
static_assert(CAMPAIGN_PROGRESS_TECH_WORD_COUNT * 4 == OLD_UNIT_SECONDARY_TABLE_BYTES);
static_assert(CAMPAIGN_PROGRESS_SELECTION_COUNT == CAMPAIGN_END_SELECTION_COUNT);

void CampaignStart_ProgressFilePath(uint16_t *outPath)

{
  WidePath_CombineDirectoryAndLeafBounded(outPath,THANDOR_PATH_CAPACITY,g_SaveDirectoryUtf16,
                                          g_ExecutableDirectoryUtf16);
  WidePath_CombineDirectoryAndLeafBounded(outPath,THANDOR_PATH_CAPACITY,g_CampaignProgressFileLeafUtf16,outPath);
}

const CampaignProgressSection *CampaignProgress_FindSectionOfCampaign(const CampaignProgress &progress,
                                                                      const CampaignAsset *campaign,
                                                                      std::string_view preferredName)

{
  const CampaignFingerprint fingerprint = CampaignAsset_Fingerprint(campaign);
  const CampaignProgressSection *found = nullptr;

  for (const CampaignProgressSection &section : progress.campaigns) {
    if (!section.fingerprint.has_value() || section.fingerprint->crc32 != fingerprint.crc32 ||
        section.fingerprint->recordCount != fingerprint.recordCount) {
      continue;
    }
    if (section.name == preferredName) {
      return &section;
    }
    if (found == nullptr) {
      found = &section;
    }
  }
  return found;
}

const CampaignLevelRecord *CampaignAsset_FindLevelRecord(const CampaignAsset *campaign,int32_t levelId)

{
  for (int32_t index = 0; index < campaign->levelRecordCount; index++) {
    if (campaign->levels[index].levelId == levelId) {
      return &campaign->levels[index];
    }
  }
  return nullptr;
}

/* carryOverMask bit selection set and skipMask bit selection clear: the selection takes units over */
static bool CampaignLevelRecord_SelectionCarries(const CampaignLevelRecord &record,int32_t selection)

{
  return ((record.carryOverMask >> selection) & 1) != 0 && ((record.skipMask >> selection) & 1) == 0;
}

bool CampaignAsset_IsCarryTarget(const CampaignAsset *campaign,int32_t levelId)

{
  for (int32_t index = 0; index < campaign->levelRecordCount; index++) {
    const CampaignLevelRecord &record = campaign->levels[index];
    if (record.levelId == levelId) {
      continue;
    }
    for (int32_t selection = 0; selection < CAMPAIGN_END_SELECTION_COUNT; selection++) {
      if (record.successorLevelIds[selection] == levelId && CampaignLevelRecord_SelectionCarries(record,selection)) {
        return true;
      }
    }
  }
  return false;
}

bool CampaignProgressSection_IsLevelDone(const CampaignProgressSection &section,int32_t levelId)

{
  return section.wins.find(levelId) != section.wins.end();
}

bool CampaignProgress_IsLevelUnlocked(const CampaignProgressSection *section,const CampaignAsset *campaign,
                                      int32_t levelId)

{
  if (levelId == campaign->firstLevelId) {
    return true;
  }
  if (section == nullptr) {
    return false;
  }
  if (section->unlockedLevelIds.count(levelId) != 0) {
    return true;
  }
  for (const auto &[wonLevelId,wins] : section->wins) {
    const CampaignLevelRecord *record = CampaignAsset_FindLevelRecord(campaign,wonLevelId);
    if (record == nullptr) {
      continue;
    }
    for (int32_t selection = 0; selection < CAMPAIGN_END_SELECTION_COUNT; selection++) {
      if (((wins.selectionMask >> selection) & 1) != 0 && record->successorLevelIds[selection] == levelId) {
        return true;
      }
    }
  }
  return false;
}

const CampaignProgressCarry *CampaignProgressSection_FindValidCarry(const CampaignProgressSection &section,
                                                                    const CampaignAsset *campaign,int32_t levelId)

{
  auto found = section.carries.find(levelId);
  if (found == section.carries.end()) {
    return nullptr;
  }
  const CampaignProgressCarry &carry = found->second;
  const CampaignLevelRecord *source = CampaignAsset_FindLevelRecord(campaign,carry.sourceLevelId);
  if (source == nullptr || carry.sourceLevelId == levelId || carry.selection < 0 ||
      carry.selection >= CAMPAIGN_END_SELECTION_COUNT || source->successorLevelIds[carry.selection] != levelId ||
      !CampaignLevelRecord_SelectionCarries(*source,carry.selection) ||
      carry.units.size() > (size_t)CAMPAIGN_PROGRESS_UNIT_CAPACITY) {
    return nullptr;
  }
  return &carry;
}

/* True when the level file of record exists with a valid header (the campaign map's node rule, 5.1), so the
   bundle load below cannot fail on it. */
static bool CampaignStart_LevelFileExists(const CampaignAsset *campaign,const CampaignLevelRecord *record)

{
  const std::vector<CampaignLevelFileInfo> infos = CampaignLevelInfo_Read(campaign);
  const size_t recordIndex = (size_t)(record - campaign->levels);

  return recordIndex < infos.size() && infos[recordIndex].fileExists && infos[recordIndex].headerValid;
}

CampaignStartDecision CampaignStart_Prepare(const CampaignAsset *campaign,int32_t startLevelId,bool networked,
                                            std::string_view preferredName)

{
  CampaignStartDecision decision{0,std::nullopt};
  uint16_t progressPath[THANDOR_PATH_CAPACITY];

  if (startLevelId == 0) {
    return decision;
  }
  const CampaignLevelRecord *record = CampaignAsset_FindLevelRecord(campaign,startLevelId);
  if (record == nullptr) {
    Thandor_Log("campaign start: level %d is not a record of the campaign, starting at its first level %d",
                startLevelId,campaign->firstLevelId);
    return decision;
  }
  if (!CampaignStart_LevelFileExists(campaign,record)) {
    Thandor_Log("campaign start: level %d has no valid level file, starting at the first level %d",startLevelId,
                campaign->firstLevelId);
    return decision;
  }
  const bool carryTarget = CampaignAsset_IsCarryTarget(campaign,startLevelId);
  if (networked && carryTarget) {
    Thandor_Log("campaign start: level %d takes units over and cannot be started in a network game, starting at "
                "the first level %d",startLevelId,campaign->firstLevelId);
    return decision;
  }
  CampaignStart_ProgressFilePath(progressPath);
  const CampaignProgress progress = CampaignProgress_LoadFile(progressPath);
  const CampaignProgressSection *section = CampaignProgress_FindSectionOfCampaign(progress,campaign,preferredName);
  if (!(section != nullptr && CampaignProgressSection_IsLevelDone(*section,startLevelId)) &&
      !CampaignProgress_IsLevelUnlocked(section,campaign,startLevelId)) {
    Thandor_Log("campaign start: level %d is neither done nor unlocked in the progress, starting at the first "
                "level %d",startLevelId,campaign->firstLevelId);
    return decision;
  }
  if (carryTarget) {
    const CampaignProgressCarry *carry =
        section != nullptr ? CampaignProgressSection_FindValidCarry(*section,campaign,startLevelId) : nullptr;
    if (carry == nullptr) {
      Thandor_Log("campaign start: level %d takes units over and the progress has no valid snapshot for it, "
                  "starting at the first level %d",startLevelId,campaign->firstLevelId);
      return decision;
    }
    decision.carry = *carry;
  }
  decision.levelId = startLevelId;
  return decision;
}

bool CampaignStart_RestoreSnapshot(const CampaignProgressCarry &carry,FieldGridAsset *fieldGrid)

{
  FixedVectorQ12 point;
  uint32_t *record;
  size_t index;

  if (fieldGrid == nullptr) {
    Thandor_Log("campaign start: no field grid, the carry-over snapshot is not restored");
    return false;
  }
  if (carry.units.size() > (size_t)CAMPAIGN_PROGRESS_UNIT_CAPACITY) {
    Thandor_Log("campaign start: snapshot with %u units, not restored",(unsigned)carry.units.size());
    return false;
  }
  for (index = 0; index < carry.units.size(); index++) {
    const CampaignProgressUnit &unit = carry.units[index];
    /* the merge places record [2] as the node's translation.x and [3] as translation.y; the field functions take
       (worldY = translation.y, worldX = translation.x), as the placement contact callbacks pass them */
    if (unit.faction < 0 || unit.faction >= CAMPAIGN_PROGRESS_FACTION_COUNT ||
        !FieldGrid_GetNearestTerrainPoint(unit.worldYQ12,unit.worldXQ12,fieldGrid,&point)) {
      Thandor_Log("campaign start: snapshot unit %u (faction %d at %d, %d) lies outside the level, the snapshot "
                  "is not restored",(unsigned)index,unit.faction,unit.worldXQ12,unit.worldYQ12);
      return false;
    }
  }
  record = g_OldUnitPrimaryTable;
  for (const CampaignProgressUnit &unit : carry.units) {
    record[0] = (uint32_t)unit.assetId;
    record[1] = (uint32_t)unit.faction;
    record[2] = (uint32_t)unit.worldXQ12;
    record[3] = (uint32_t)unit.worldYQ12;
    record[4] = unit.rotation;
    record[5] = 0;
    record[6] = 0;
    record[7] = 0;
    record += CAMPAIGN_START_PRIMARY_RECORD_DWORDS;
  }
  g_OldUnitRecordCount = (OldUnitRecordCount)carry.units.size();
  for (index = 0; index < carry.techMasks.size(); index++) {
    g_OldUnitSecondaryTable[index] = carry.techMasks[index];
  }
  Thandor_Log("campaign start: restored the carry-over snapshot from level %d (selection %d): %u units",
              carry.sourceLevelId,carry.selection,(unsigned)carry.units.size());
  return true;
}
