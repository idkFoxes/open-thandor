/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/campaign_progress.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_SESSION_CAMPAIGN_PROGRESS_H
#define THANDOR_GAMEPLAY_SESSION_CAMPAIGN_PROGRESS_H

#include <stdint.h>
#include <array>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

/* Campaign progress (docs/plans/campaign_map.md 4.4): the won and unlocked missions of each campaign and the
   carry-over snapshots for starting a mission that takes over units, kept in the text file
   save\campaign_progress.txt. Native memory and native file calls only (no arena). The model holds what the file
   says after the load-time checks of 4.4; the checks that need the loaded campaign (level ids are records of
   the campaign, a carry target is the successor of its source, unit positions inside the level) are made when a
   section is applied. Every container is sorted, so formatting the same progress always gives the same text. */

/* The first line of the file is CAMPAIGN_PROGRESS_MAGIC, one blank and the version. */
inline constexpr std::string_view CAMPAIGN_PROGRESS_MAGIC = "open-thandor campaign progress";
inline constexpr int CAMPAIGN_PROGRESS_VERSION = 1;
/* Campaign keys (the .cgn base name): 1..31 characters [A-Za-z0-9_] */
inline constexpr size_t CAMPAIGN_PROGRESS_NAME_MAX_LENGTH = 31;
/* End selections 0..7 (CampaignLevelRecord.successorLevelIds) and factions 0..7 */
inline constexpr int CAMPAIGN_PROGRESS_SELECTION_COUNT = 8;
inline constexpr int CAMPAIGN_PROGRESS_FACTION_COUNT = 8;
/* Technology mask dwords per faction in g_OldUnitSecondaryTable, and the whole table */
inline constexpr int CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION = 8;
inline constexpr int CAMPAIGN_PROGRESS_TECH_WORD_COUNT =
          CAMPAIGN_PROGRESS_FACTION_COUNT * CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION;
/* At most this many units per snapshot (OLD_UNIT_PRIMARY_RECORD_CAPACITY) */
inline constexpr int CAMPAIGN_PROGRESS_UNIT_CAPACITY = 0x200;
/* The parser logs at most this many ignored items one by one, then one line with the rest */
inline constexpr uint32_t CAMPAIGN_PROGRESS_LOG_LIMIT = 16;

/* fingerprint <crc32> <records>: identifies the campaign records the section was recorded with */
struct CampaignProgressFingerprint {
  uint32_t crc32;
  int32_t recordCount;

  bool operator==(const CampaignProgressFingerprint &) const = default;
};

/* done <levelId> <selection> lines of one level: every end selection ever won (bit s = selection s) and the
   selection of the last win (written last) */
struct CampaignProgressWins {
  uint8_t selectionMask;
  uint8_t lastSelection;

  bool operator==(const CampaignProgressWins &) const = default;
};

/* unit <asset> <faction> <x> <y> <rotation>: dwords [0..4] of one g_OldUnitPrimaryTable record. The asset id
   is any int32 (no numeric range exists, plan 4.4 "Army asset ids"); the rotation is the engine's AngleTurn32,
   kept as written so a restore gives the chain's exact value. */
struct CampaignProgressUnit {
  int32_t assetId;
  int32_t faction;
  int32_t worldXQ12;
  int32_t worldYQ12;
  uint32_t rotation;

  bool operator==(const CampaignProgressUnit &) const = default;
};

/* carry <target> from <source> sel <s> units <n>, n unit lines and the tech lines: the old-unit tables as
   OldUnitRuntime_RebuildScenarioReplayTables left them after the last win of source with selection s.
   techMasks is laid out like g_OldUnitSecondaryTable (faction * 8 + word); missing factions are 0. */
struct CampaignProgressCarry {
  int32_t sourceLevelId;
  int32_t selection;
  std::vector<CampaignProgressUnit> units;
  std::array<uint32_t,CAMPAIGN_PROGRESS_TECH_WORD_COUNT> techMasks;

  bool operator==(const CampaignProgressCarry &) const = default;
};

/* [campaign <name>] ... end */
struct CampaignProgressSection {
  std::string name;
  std::optional<CampaignProgressFingerprint> fingerprint;
  std::map<int32_t,CampaignProgressWins> wins;          /* by level id */
  std::set<int32_t> unlockedLevelIds;                    /* rule 3 of 4.3 only; successors of wins are derived */
  std::map<int32_t,CampaignProgressCarry> carries;      /* by target level id */

  bool operator==(const CampaignProgressSection &) const = default;
};

/* All sections, sorted by name, each name at most once. */
struct CampaignProgress {
  std::vector<CampaignProgressSection> campaigns;

  bool operator==(const CampaignProgress &) const = default;
};

/* What a parse did, for the log and the self-test. */
struct CampaignProgressParseReport {
  bool headerValid;          /* false: wrong magic or version, nothing was read */
  uint32_t ignoredItems;     /* malformed or unknown lines, dropped sections and snapshots */
};

/* true when name is a valid campaign key (1..31 characters [A-Za-z0-9_]) */
bool CampaignProgress_IsValidName(std::string_view name);

/* The section of that campaign, or nullptr. */
const CampaignProgressSection *CampaignProgress_FindSection(const CampaignProgress &progress,std::string_view name);

/* The section of that campaign, added in sorted position (empty) when it is missing. */
CampaignProgressSection &CampaignProgress_GetOrAddSection(CampaignProgress &progress,std::string_view name);

/* Records a win of levelId with that end selection (0..7; others are ignored): adds the selection and makes it
   the last one. */
void CampaignProgressSection_RecordWin(CampaignProgressSection &section,int32_t levelId,int32_t selection);

/* Parses the text of a progress file into *outProgress (emptied first). A wrong header (magic or version)
   leaves it empty and returns headerValid false. Otherwise every malformed or unknown item is ignored with one
   log line ("campaign progress: ..."): a bad line, a section with a bad or repeated name or without "end", a
   snapshot with a bad header, a bad unit or tech line or a wrong number of unit lines. */
CampaignProgressParseReport CampaignProgress_Parse(std::string_view text,CampaignProgress *outProgress);

/* The file text of progress (version CAMPAIGN_PROGRESS_VERSION). Sections with an invalid name are left out
   (one log line). */
std::string CampaignProgress_Format(const CampaignProgress &progress);

/* Loads the progress file at path. A missing file gives empty progress. A file that cannot be read (over the
   native read limit) or whose header is wrong is renamed to the same name with the extension ".bad" (replacing
   an older one), with one log line, and gives empty progress, so the next write cannot overwrite it. */
CampaignProgress CampaignProgress_LoadFile(uint16_t *path);

/* Writes progress to path through FileSystem_WriteFileReplacing. Returns 0 or its error (one log line); the old
   file stays on failure. */
uint32_t CampaignProgress_SaveFile(uint16_t *path,const CampaignProgress &progress);

/* The only entry points to the stored progress (the storage may change; callers never build its path). Today the
   file save\campaign_progress.txt under the executable directory, or OPEN_THANDOR_CAMPAIGN_PROGRESS=<path> in a
   dev-tools build. LoadCurrent behaves like CampaignProgress_LoadFile. SaveCurrent writes like
   CampaignProgress_SaveFile and creates the missing directory first when the write fails (one log line on a
   final failure; the game goes on). */
CampaignProgress CampaignProgress_LoadCurrent();
void CampaignProgress_SaveCurrent(const CampaignProgress &progress);

/* --- Session hooks (src/gameplay/session/campaign_progress_session.cpp, plan 4.3, 4.5, 7.5) --- */

struct CampaignAsset;
struct CampaignLevelRecord;

/* The win rule of plan 4.3 for a session of level levelId (whose record is levelRecord) that ended through the
   end trigger with that end selection and movie variant: the success movie (variant != 0), a selection 0..7 and
   a successor that is not the level itself (a retry). */
bool CampaignProgress_IsWin(const CampaignLevelRecord &levelRecord,int32_t levelId,uint32_t endMovieSelectionIndex,
                            uint32_t endMovieVariantIndex);

/* The carry-over snapshot of plan 4.5: recordCount (at most CAMPAIGN_PROGRESS_UNIT_CAPACITY are taken) primary
   records of 8 dwords, of which dwords [0..4] are kept, and the 8 x 8 dwords of the secondary table. */
CampaignProgressCarry CampaignProgress_TakeCarry(int32_t sourceLevelId,int32_t selection,const uint32_t *primaryTable,
                                                 uint32_t recordCount,const uint32_t *secondaryTable);

/* Called once when a session ends through the end trigger (the END_MOVIE branch of
   InGameRuntime_RunSessionUntilExit), after OldUnitRuntime_RebuildScenarioReplayTables. Outside a campaign it does
   nothing. For a win (CampaignProgress_IsWin) of the campaign's current level it records the level as done with
   that selection and, when the selection carries units over (carryOverMask bit set, skipMask bit clear), the
   old-unit tables as the snapshot for the successor; then it rewrites the progress file. Every peer of a network
   game records its own result (plan 7.4). Only reads game state; native memory and files only. */
void CampaignProgress_RecordSessionEnd(const CampaignAsset *campaign,uint32_t endMovieSelectionIndex,
                                       uint32_t endMovieVariantIndex);

/* Called once when a session of a campaign level has started (new or loaded): unlock rule 3 of plan 4.3. Adds the
   campaign's current level to the explicitly unlocked levels unless it is already unlocked another way (the base
   campaign's first level, a done level or the successor of a recorded win); writes the file only on a change.
   campaign nullptr (not a campaign) does nothing. */
void CampaignProgress_NoteLevelStarted(const CampaignAsset *campaign);

#endif /* THANDOR_GAMEPLAY_SESSION_CAMPAIGN_PROGRESS_H */
