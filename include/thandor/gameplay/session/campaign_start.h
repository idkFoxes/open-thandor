/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/campaign_start.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_SESSION_CAMPAIGN_START_H
#define THANDOR_GAMEPLAY_SESSION_CAMPAIGN_START_H

#include <stdint.h>
#include <optional>
#include <string_view>
#include <thandor/assets/scenario/catalog.h>
#include <thandor/gameplay/session/campaign_progress.h>

struct FieldGridAsset;

/* Starting a campaign at a mission from the campaign map (docs/plans/campaign_map.md 7.3, 7.4, 4.5). The start
   level travels in the first argument dword of FRONTEND_COMMAND_LOAD_CAMPAIGN
   (FrontendScenarioSession_LoadOrRequestCampaignBundle(playerRuntimeId,startLevelId,0,selectedRecordIndex)); 0 is
   the original behaviour. Everything here uses native memory and native file calls only (no arena). */

/* The progress section that belongs to a loaded campaign: the section whose fingerprint equals the campaign's
   (CampaignAsset_Fingerprint); a section named preferredName wins over others with the same fingerprint. A
   section without a fingerprint or with another one is never applied (4.4). nullptr when there is none. The
   caller has checked the campaign with CampaignAsset_Fits. */
const CampaignProgressSection *CampaignProgress_FindSectionOfCampaign(const CampaignProgress &progress,
                                                                      const CampaignAsset *campaign,
                                                                      std::string_view preferredName);

/* The record of levelId in the campaign (the first one, as the engine's record lookup), or nullptr. */
const CampaignLevelRecord *CampaignAsset_FindLevelRecord(const CampaignAsset *campaign,int32_t levelId);

/* True when levelId is the target of a carry-over: some other record has successorLevelIds[s] == levelId with
   carryOverMask bit s set and skipMask bit s clear (the double-line edges of the map, read from the records). */
bool CampaignAsset_IsCarryTarget(const CampaignAsset *campaign,int32_t levelId);

/* True when the section records a win of levelId (any selection). */
bool CampaignProgressSection_IsLevelDone(const CampaignProgressSection &section,int32_t levelId);

/* True when levelId is unlocked (4.3): the campaign's firstLevelId, the successor of a recorded win for one of its
   won selections, or listed in the section's unlocked levels. section may be nullptr (no progress: only the first
   level). Does not check that levelId is a record of the campaign. */
bool CampaignProgress_IsLevelUnlocked(const CampaignProgressSection *section,const CampaignAsset *campaign,
                                      int32_t levelId);

/* The carry-over snapshot for starting levelId directly, when the section has one that fits the campaign
   (4.4): its source is a record whose successorLevelIds[selection] is levelId with carryOverMask bit selection
   set and skipMask bit selection clear, and it holds at most CAMPAIGN_PROGRESS_UNIT_CAPACITY units. The unit
   positions are checked only at restore (CampaignStart_RestoreSnapshot, against the loaded level). nullptr
   otherwise. */
const CampaignProgressCarry *CampaignProgressSection_FindValidCarry(const CampaignProgressSection &section,
                                                                    const CampaignAsset *campaign,int32_t levelId);

/* What CampaignStart_Prepare decided. */
struct CampaignStartDecision {
  int32_t levelId;                             /* the accepted start level; 0: keep the campaign's firstLevelId */
  std::optional<CampaignProgressCarry> carry;  /* local game only: the snapshot to restore after the reset */
};

/* Checks a requested start level of the loaded campaign (host or local game; 7.3, 7.4) and reads what its start
   needs from the progress file:
   - startLevelId 0: nothing is read, the result is {0, none} (original behaviour);
   - the level must be a record of the campaign whose level file exists, and done or unlocked in the local
     progress (the section of CampaignProgress_FindSectionOfCampaign with preferredName);
   - a carry target (CampaignAsset_IsCarryTarget) cannot be started in a network game (a host-only snapshot would
     desynchronise the peers), and in a local game only with a valid snapshot, which is returned for the restore.
   A rejected level gives {0, none} with one log line ("campaign start: ..."). */
CampaignStartDecision CampaignStart_Prepare(const CampaignAsset *campaign,int32_t startLevelId,bool networked,
                                            std::string_view preferredName);

/* Writes a snapshot back into the old-unit tables, as OldUnitRuntime_RebuildScenarioReplayTables leaves them after
   the source level's win: the record count, dwords [0..4] of each primary record (dwords [5..7] 0) and the
   secondary (technology) table. Called after OldUnitRuntime_ResetPendingTables; the session start then merges
   the tables (OldUnitRuntime_MergeMasksAndReplayRecords) as in the chain. Every unit must lie on the level's
   field grid (FieldGrid_GetNearestTerrainPoint with the merge's coordinate order); otherwise, or without a
   grid, nothing is written (one log line) and false is returned. */
bool CampaignStart_RestoreSnapshot(const CampaignProgressCarry &carry,FieldGridAsset *fieldGrid);

/* The progress file path, <exe dir>\save\campaign_progress.txt, into a THANDOR_PATH_CAPACITY buffer. */
void CampaignStart_ProgressFilePath(uint16_t *outPath);

#endif /* THANDOR_GAMEPLAY_SESSION_CAMPAIGN_START_H */
