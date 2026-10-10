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
#include <span>
#include <string>
#include <string_view>
#include <vector>

/* Campaign progress (docs/plans/campaign_map.md 4.4): the won and unlocked missions of each campaign and the
   carry-over snapshots for starting a mission that takes over units, kept in save\kampagne.dat. The file is a PCK
   package in the game's own format (archive header, then per entry a 0x200-byte entry header and the payload
   packed with the Huffman/RLE codec, compression method 0), written and read natively here (no package API, no
   arena, main thread only: the codec has static workspaces). Its entries:

     progress.bin                    the sections (names, fingerprints, wins, unlocked levels, snapshot targets)
     carry_<name>_<target>.hex       one per snapshot: a small header, then the oldunit.hex layout (record count,
                                     0x200 primary records of 8 dwords, 64 technology dwords)

   Every payload starts with CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES: its type tag (also the entry's typeTag), the
   version, the byte count of the data and the CRC32 of the data behind that header, so a damaged or hand-edited
   file is detected. The model holds what the file says after the load-time checks of 4.4; the checks that need
   the loaded campaign (level ids are records of the campaign, a carry target is the successor of its source, unit
   positions inside the level) are made when a section is applied. Every container is sorted, so encoding the same
   progress always gives the same bytes. */

/* Version of every payload of the file */
inline constexpr uint32_t CAMPAIGN_PROGRESS_VERSION = 1;
/* Type tags (first payload dword, three lowercase letters like the stock tags): "kpg" progress.bin, "kco" carry */
inline constexpr uint32_t CAMPAIGN_PROGRESS_TAG = 'k' | 'p' << 8 | 'g' << 16;
inline constexpr uint32_t CAMPAIGN_PROGRESS_CARRY_TAG = 'k' | 'c' << 8 | 'o' << 16;
/* Payload header: tag, version, data byte count (header included), CRC32 of the data behind the header. A payload
   whose data byte count is a multiple of 16 gets CAMPAIGN_PROGRESS_PAYLOAD_PAD_BYTES zero bytes behind it, so its
   unpacked size never equals the packed size (always a multiple of 16; tools/data/pck.py takes an entry of equal
   sizes as stored). */
inline constexpr size_t CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES = 16;
inline constexpr size_t CAMPAIGN_PROGRESS_PAYLOAD_PAD_BYTES = 4;
/* progress.bin: the header, the section count, then per section the name (CAMPAIGN_PROGRESS_NAME_FIELD_BYTES, zero
   padded), flags (bit 0: has a fingerprint), fingerprint CRC32 and record count (both 0 without one), the win count
   and wins (levelId, selectionMask byte, lastSelection byte, 2 zero bytes), the unlocked count and level ids, the
   snapshot count and target level ids. Every list is sorted ascending without repeats, the sections by name. */
inline constexpr std::string_view CAMPAIGN_PROGRESS_MAIN_ENTRY = "progress.bin";
inline constexpr size_t CAMPAIGN_PROGRESS_NAME_FIELD_BYTES = 32;
inline constexpr uint32_t CAMPAIGN_PROGRESS_FLAG_FINGERPRINT = 1;
inline constexpr size_t CAMPAIGN_PROGRESS_WIN_BYTES = 8;
/* carry_<name>_<target>.hex: the header (tag, version, size, CRC32, target, source, selection, 0), then the
   oldunit.hex layout: record count, CAMPAIGN_PROGRESS_UNIT_CAPACITY records of 8 dwords (dwords [0..4] as in
   CampaignProgressUnit, [5..7] and unused records 0), CAMPAIGN_PROGRESS_TECH_WORD_COUNT technology dwords */
inline constexpr size_t CAMPAIGN_PROGRESS_CARRY_HEADER_BYTES = 0x20;
inline constexpr size_t CAMPAIGN_PROGRESS_CARRY_RECORD_DWORDS = 8;
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
/* The whole carry entry payload (0x4124 bytes) */
inline constexpr size_t CAMPAIGN_PROGRESS_CARRY_BYTES =
          CAMPAIGN_PROGRESS_CARRY_HEADER_BYTES + 4 +
          CAMPAIGN_PROGRESS_UNIT_CAPACITY * CAMPAIGN_PROGRESS_CARRY_RECORD_DWORDS * 4 + CAMPAIGN_PROGRESS_TECH_WORD_COUNT * 4;
/* Largest unpacked payload accepted from a file */
inline constexpr uint32_t CAMPAIGN_PROGRESS_ENTRY_LIMIT_BYTES = 0x1000000;

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

/* One package entry: its path (lowercase ASCII) and its unpacked payload. */
struct CampaignProgressEntry {
  std::string path;
  std::vector<uint8_t> payload;

  bool operator==(const CampaignProgressEntry &) const = default;
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

/* CRC32 (IEEE, as zlib) of bytes */
uint32_t CampaignProgress_Crc32(std::span<const uint8_t> bytes);

/* Completes a payload whose header tail is still open: stores the data byte count (the payload size) and the CRC32
   at +8 / +12 and appends CAMPAIGN_PROGRESS_PAYLOAD_PAD_BYTES zero bytes when that count is a multiple of 16. */
void CampaignProgress_SealPayload(std::vector<uint8_t> *payload);

/* "carry_<name lowercased>_<target>.hex" */
std::string CampaignProgress_CarryEntryPath(std::string_view name,int32_t targetLevelId);

/* The entries of progress: progress.bin, then the snapshots of each section by target. Sections with an invalid
   name and items the file cannot hold (a level id < 1, a selection or faction outside 0..7, more than 0x200 units,
   a win without a selection) are left out with one log line each. */
std::vector<CampaignProgressEntry> CampaignProgress_EncodeEntries(const CampaignProgress &progress);

/* The progress the entries hold. false with the reason in *outReason (*outProgress emptied) when any check fails:
   the entry paths, order and count, a payload's tag, version, sizes, padding or CRC32, a count larger than the bytes
   left, a name, level id, selection or faction outside its range, a list not sorted or repeated (a repeated
   section), reserved bytes or unused records not 0. outReason may be nullptr. */
bool CampaignProgress_DecodeEntries(const std::vector<CampaignProgressEntry> &entries,CampaignProgress *outProgress,
                                    std::string *outReason);

/* The PCK package of entries (Huffman/RLE packed, timestamps 0); nullopt (one log line) when the codec fails. */
std::optional<std::vector<uint8_t>> CampaignProgress_WritePackage(const std::vector<CampaignProgressEntry> &entries);

/* The entries of a PCK package as CampaignProgress_WritePackage writes it; false with the reason in *outReason when
   the archive or an entry header is malformed, an entry does not unpack or its type tag is not its first dword.
   outReason may be nullptr. */
bool CampaignProgress_ReadPackage(std::span<const uint8_t> bytes,std::vector<CampaignProgressEntry> *outEntries,
                                  std::string *outReason);

/* The file bytes of progress (EncodeEntries, then WritePackage); nullopt when the codec fails. */
std::optional<std::vector<uint8_t>> CampaignProgress_Encode(const CampaignProgress &progress);

/* The progress held by file bytes (ReadPackage, then DecodeEntries); false with the reason when anything is wrong. */
bool CampaignProgress_Decode(std::span<const uint8_t> bytes,CampaignProgress *outProgress,std::string *outReason);

/* Loads the progress file at path. A missing file gives empty progress. A file that cannot be read (over the
   native read limit) or that CampaignProgress_Decode rejects is renamed to the same name with the extension ".bad" (replacing
   an older one), with one log line, and gives empty progress, so the next write cannot overwrite it. */
CampaignProgress CampaignProgress_LoadFile(uint16_t *path);

/* Writes progress to path through FileSystem_WriteFileReplacing. Returns 0 or its error (one log line; or
   FATAL_ERROR_GENERAL_FAILURE when it cannot be encoded); the old file stays on failure. */
uint32_t CampaignProgress_SaveFile(uint16_t *path,const CampaignProgress &progress);

/* The only entry points to the stored progress (the storage may change; callers never build its path). Today the
   file save\kampagne.dat under the executable directory, or OPEN_THANDOR_CAMPAIGN_PROGRESS=<path> in a
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
