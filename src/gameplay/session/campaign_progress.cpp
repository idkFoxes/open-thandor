/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/campaign_progress.cpp
 * Project code (not in the original game)
 */

#include <thandor/gameplay/session/campaign_progress.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <span>
#include <thandor/thandor.h>
#include <thandor/assets/package/codec.h>
#include <thandor/platform/bootstrap/image.h>

/* Campaign progress file (docs/plans/campaign_map.md 4.4), save\kampagne.dat: a PCK package in the game's own
   format, built and read here with native memory only (the package API allocates from the arena and uses
   g_PackageScratchBuffer). Layout:

     +0x000  PckArchiveHeader: "pck\0", archiveSize (the file size), version 1, format 0x10000, timestamps 0,
             producer and source name "open-thandor", entryCount at +0xB0
     then per entry a PckEntryHeader (lowercase UTF-16 path, runtimePayloadOffset 0, unpackedSize, typeTag = first
             payload dword, packedSize, compressionMethod 0) directly followed by packedSize bytes of the payload
             packed with PckCodec_EncodeHuffmanRle

   The entries are progress.bin and one carry_<name>_<target>.hex per snapshot, in the order of the sections and
   their targets. The payload layouts are in campaign_progress.h. Load-time checks only look at the file itself; the
   checks against the campaign are made when a section is applied. Any failed check rejects the whole file. */

/* ".bad", the extension of a rejected file */
static constexpr std::array<uint16_t,4> CAMPAIGN_PROGRESS_BAD_EXTENSION = {'b','a','d',0};
/* PckArchiveHeader values of every stock archive */
static constexpr uint32_t CAMPAIGN_PROGRESS_PCK_VERSION = 1;
static constexpr uint32_t CAMPAIGN_PROGRESS_PCK_FORMAT = 0x10000;
/* producerName and sourceName of the archive header */
static constexpr std::string_view CAMPAIGN_PROGRESS_PRODUCER = "open-thandor";
/* code units of PckEntryHeader.path */
static constexpr size_t CAMPAIGN_PROGRESS_PATH_UNITS = sizeof(PckEntryHeader::path) / sizeof(PckEntryHeader::path[0]);
/* encoder output room per payload byte, plus the frequency table and slack (see CampaignProgress_PackPayload) */
static constexpr size_t CAMPAIGN_PROGRESS_PACK_BYTES_PER_BYTE = 4;
static constexpr size_t CAMPAIGN_PROGRESS_PACK_SLACK_BYTES = 0x40;
/* the smallest packed size PckCodec_DecodeHuffmanRle accepts */
static constexpr uint32_t CAMPAIGN_PROGRESS_MIN_PACKED_BYTES = PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 4;

static_assert(sizeof(PckArchiveHeader) == PCK_ENTRY_HEADER_BYTES && sizeof(PckEntryHeader) == PCK_ENTRY_HEADER_BYTES);
static_assert(CAMPAIGN_PROGRESS_CARRY_BYTES % 16 != 0); /* never padded, never equal to a packed size */

/* --- Model --- */

bool CampaignProgress_IsValidName(std::string_view name)

{
  if (name.empty() || name.size() > CAMPAIGN_PROGRESS_NAME_MAX_LENGTH) {
    return false;
  }
  return std::all_of(name.begin(),name.end(),[](char character) {
    return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') ||
           (character >= '0' && character <= '9') || character == '_';
  });
}

static bool CampaignProgress_SectionNameLess(const CampaignProgressSection &section,std::string_view name)

{
  return std::string_view(section.name) < name;
}

const CampaignProgressSection *CampaignProgress_FindSection(const CampaignProgress &progress,std::string_view name)

{
  auto found = std::lower_bound(progress.campaigns.begin(),progress.campaigns.end(),name,
                                CampaignProgress_SectionNameLess);
  if (found == progress.campaigns.end() || found->name != name) {
    return nullptr;
  }
  return &*found;
}

CampaignProgressSection &CampaignProgress_GetOrAddSection(CampaignProgress &progress,std::string_view name)

{
  auto found = std::lower_bound(progress.campaigns.begin(),progress.campaigns.end(),name,
                                CampaignProgress_SectionNameLess);
  if (found == progress.campaigns.end() || found->name != name) {
    found = progress.campaigns.insert(found,CampaignProgressSection{});
    found->name = std::string(name);
  }
  return *found;
}

void CampaignProgressSection_RecordWin(CampaignProgressSection &section,int32_t levelId,int32_t selection)

{
  CampaignProgressWins *wins;

  if (selection < 0 || selection >= CAMPAIGN_PROGRESS_SELECTION_COUNT) {
    return;
  }
  wins = &section.wins[levelId]; /* a new entry starts with no selection */
  wins->selectionMask = static_cast<uint8_t>(wins->selectionMask | (1u << selection));
  wins->lastSelection = static_cast<uint8_t>(selection);
}

/* --- Payloads --- */

/* CRC32 table (IEEE polynomial, reflected) */
static constexpr std::array<uint32_t,256> CampaignProgress_MakeCrc32Table()

{
  std::array<uint32_t,256> table{};

  for (uint32_t index = 0; index < 256; index++) {
    uint32_t value = index;
    for (int bit = 0; bit < 8; bit++) {
      value = (value & 1) != 0 ? 0xEDB88320u ^ (value >> 1) : value >> 1;
    }
    table[index] = value;
  }
  return table;
}

static constexpr std::array<uint32_t,256> CAMPAIGN_PROGRESS_CRC32_TABLE = CampaignProgress_MakeCrc32Table();

uint32_t CampaignProgress_Crc32(std::span<const uint8_t> bytes)

{
  uint32_t state = 0xFFFFFFFFu;

  for (uint8_t value : bytes) {
    state = CAMPAIGN_PROGRESS_CRC32_TABLE[(state ^ value) & 0xFFu] ^ (state >> 8);
  }
  return state ^ 0xFFFFFFFFu;
}

static void CampaignProgress_PutU32(std::vector<uint8_t> *bytes,uint32_t value)

{
  for (int shift = 0; shift < 32; shift += 8) {
    bytes->push_back(static_cast<uint8_t>(value >> shift));
  }
}

static void CampaignProgress_StoreU32(uint8_t *destination,uint32_t value)

{
  for (int index = 0; index < 4; index++) {
    destination[index] = static_cast<uint8_t>(value >> (index * 8));
  }
}

static uint32_t CampaignProgress_LoadU32(const uint8_t *source)

{
  return static_cast<uint32_t>(source[0]) | static_cast<uint32_t>(source[1]) << 8 |
         static_cast<uint32_t>(source[2]) << 16 | static_cast<uint32_t>(source[3]) << 24;
}

/* A new payload: the tag, the version and the open size and CRC32 fields (CampaignProgress_SealPayload). */
static std::vector<uint8_t> CampaignProgress_BeginPayload(uint32_t tag)

{
  std::vector<uint8_t> payload;

  CampaignProgress_PutU32(&payload,tag);
  CampaignProgress_PutU32(&payload,CAMPAIGN_PROGRESS_VERSION);
  CampaignProgress_PutU32(&payload,0);
  CampaignProgress_PutU32(&payload,0);
  return payload;
}

void CampaignProgress_SealPayload(std::vector<uint8_t> *payload)

{
  const size_t dataBytes = payload->size();

  CampaignProgress_StoreU32(payload->data() + 8,static_cast<uint32_t>(dataBytes));
  CampaignProgress_StoreU32(payload->data() + 12,CampaignProgress_Crc32(std::span<const uint8_t>(*payload).subspan
                                                   (CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES)));
  if (dataBytes % 16 == 0) {
    payload->insert(payload->end(),CAMPAIGN_PROGRESS_PAYLOAD_PAD_BYTES,0);
  }
}

std::string CampaignProgress_CarryEntryPath(std::string_view name,int32_t targetLevelId)

{
  std::string path = "carry_";

  for (char character : name) {
    path.push_back((character >= 'A' && character <= 'Z') ? static_cast<char>(character + ('a' - 'A')) : character);
  }
  path += '_';
  path += std::to_string(targetLevelId);
  path += ".hex";
  return path;
}

/* A snapshot the file can hold (CampaignProgress_DecodeCarry accepts it); else one log line. */
static bool CampaignProgress_CarryWritable(const CampaignProgressSection &section,int32_t targetLevelId,
                                           const CampaignProgressCarry &carry)

{
  const char *reason = nullptr;

  if (targetLevelId < 1 || carry.sourceLevelId < 1) {
    reason = "level id not positive";
  }
  else if (carry.selection < 0 || carry.selection >= CAMPAIGN_PROGRESS_SELECTION_COUNT) {
    reason = "selection not 0..7";
  }
  else if (carry.units.size() > static_cast<size_t>(CAMPAIGN_PROGRESS_UNIT_CAPACITY)) {
    reason = "more than 0x200 units";
  }
  else if (std::any_of(carry.units.begin(),carry.units.end(),[](const CampaignProgressUnit &unit) {
             return unit.faction < 0 || unit.faction >= CAMPAIGN_PROGRESS_FACTION_COUNT; })) {
    reason = "a unit faction not 0..7";
  }
  if (reason != nullptr) {
    Thandor_Log("campaign progress: %s snapshot for level %d not written: %s",section.name.c_str(),targetLevelId,
                reason);
    return false;
  }
  return true;
}

/* carry_<name>_<target>.hex: the header, then the oldunit.hex layout. */
static std::vector<uint8_t> CampaignProgress_EncodeCarry(int32_t targetLevelId,const CampaignProgressCarry &carry)

{
  std::vector<uint8_t> payload = CampaignProgress_BeginPayload(CAMPAIGN_PROGRESS_CARRY_TAG);
  size_t record;
  size_t dword;

  payload.reserve(CAMPAIGN_PROGRESS_CARRY_BYTES);
  CampaignProgress_PutU32(&payload,static_cast<uint32_t>(targetLevelId));
  CampaignProgress_PutU32(&payload,static_cast<uint32_t>(carry.sourceLevelId));
  CampaignProgress_PutU32(&payload,static_cast<uint32_t>(carry.selection));
  CampaignProgress_PutU32(&payload,0);
  CampaignProgress_PutU32(&payload,static_cast<uint32_t>(carry.units.size()));
  for (record = 0; record < static_cast<size_t>(CAMPAIGN_PROGRESS_UNIT_CAPACITY); record++) {
    if (record < carry.units.size()) {
      const CampaignProgressUnit &unit = carry.units[record];
      CampaignProgress_PutU32(&payload,static_cast<uint32_t>(unit.assetId));
      CampaignProgress_PutU32(&payload,static_cast<uint32_t>(unit.faction));
      CampaignProgress_PutU32(&payload,static_cast<uint32_t>(unit.worldXQ12));
      CampaignProgress_PutU32(&payload,static_cast<uint32_t>(unit.worldYQ12));
      CampaignProgress_PutU32(&payload,unit.rotation);
      dword = 5;
    }
    else {
      dword = 0;
    }
    for (; dword < CAMPAIGN_PROGRESS_CARRY_RECORD_DWORDS; dword++) {
      CampaignProgress_PutU32(&payload,0);
    }
  }
  for (uint32_t word : carry.techMasks) {
    CampaignProgress_PutU32(&payload,word);
  }
  CampaignProgress_SealPayload(&payload);
  return payload;
}

/* One section of progress.bin; targets are the snapshots written for it. */
static void CampaignProgress_EncodeSection(std::vector<uint8_t> *payload,const CampaignProgressSection &section,
                                           const std::vector<int32_t> &targets)

{
  std::vector<std::pair<int32_t,CampaignProgressWins>> wins;
  std::vector<int32_t> unlocked;
  size_t index;

  for (index = 0; index < CAMPAIGN_PROGRESS_NAME_FIELD_BYTES; index++) {
    payload->push_back(index < section.name.size() ? static_cast<uint8_t>(section.name[index]) : 0);
  }
  CampaignProgress_PutU32(payload,section.fingerprint.has_value() ? CAMPAIGN_PROGRESS_FLAG_FINGERPRINT : 0);
  CampaignProgress_PutU32(payload,section.fingerprint.has_value() ? section.fingerprint->crc32 : 0);
  CampaignProgress_PutU32(payload,section.fingerprint.has_value() ?
                                  static_cast<uint32_t>(std::max(section.fingerprint->recordCount,0)) : 0);
  for (const auto &[levelId,levelWins] : section.wins) {
    if (levelId < 1 || (levelWins.selectionMask & (1u << (levelWins.lastSelection & 7))) == 0 ||
        levelWins.lastSelection >= CAMPAIGN_PROGRESS_SELECTION_COUNT) {
      Thandor_Log("campaign progress: %s win of level %d not written (level id or selections)",
                  section.name.c_str(),levelId);
      continue;
    }
    wins.emplace_back(levelId,levelWins);
  }
  CampaignProgress_PutU32(payload,static_cast<uint32_t>(wins.size()));
  for (const auto &[levelId,levelWins] : wins) {
    CampaignProgress_PutU32(payload,static_cast<uint32_t>(levelId));
    payload->push_back(levelWins.selectionMask);
    payload->push_back(levelWins.lastSelection);
    payload->push_back(0);
    payload->push_back(0);
  }
  for (int32_t levelId : section.unlockedLevelIds) {
    if (levelId < 1) {
      Thandor_Log("campaign progress: %s unlocked level %d not written",section.name.c_str(),levelId);
      continue;
    }
    unlocked.push_back(levelId);
  }
  CampaignProgress_PutU32(payload,static_cast<uint32_t>(unlocked.size()));
  for (int32_t levelId : unlocked) {
    CampaignProgress_PutU32(payload,static_cast<uint32_t>(levelId));
  }
  CampaignProgress_PutU32(payload,static_cast<uint32_t>(targets.size()));
  for (int32_t levelId : targets) {
    CampaignProgress_PutU32(payload,static_cast<uint32_t>(levelId));
  }
}

std::vector<CampaignProgressEntry> CampaignProgress_EncodeEntries(const CampaignProgress &progress)

{
  std::vector<CampaignProgressEntry> entries(1);
  std::vector<const CampaignProgressSection *> sections;
  std::vector<uint8_t> &main = entries[0].payload;

  for (const CampaignProgressSection &section : progress.campaigns) {
    if (!CampaignProgress_IsValidName(section.name)) {
      Thandor_Log("campaign progress: a section with an invalid name (%u characters) not written",
                  static_cast<unsigned>(section.name.size()));
      continue;
    }
    sections.push_back(&section);
  }
  /* the model keeps its sections sorted; sorted again so the file is valid whatever the caller did */
  std::sort(sections.begin(),sections.end(),[](const CampaignProgressSection *left,const CampaignProgressSection *right) {
    return left->name < right->name;
  });
  sections.erase(std::unique(sections.begin(),sections.end(),[](const CampaignProgressSection *left,
                                                               const CampaignProgressSection *right) {
    return left->name == right->name;
  }),sections.end());
  entries[0].path = std::string(CAMPAIGN_PROGRESS_MAIN_ENTRY);
  main = CampaignProgress_BeginPayload(CAMPAIGN_PROGRESS_TAG);
  CampaignProgress_PutU32(&main,static_cast<uint32_t>(sections.size()));
  for (const CampaignProgressSection *section : sections) {
    std::vector<int32_t> targets;
    for (const auto &[targetLevelId,carry] : section->carries) {
      if (CampaignProgress_CarryWritable(*section,targetLevelId,carry)) {
        targets.push_back(targetLevelId);
        entries.push_back(CampaignProgressEntry{CampaignProgress_CarryEntryPath(section->name,targetLevelId),
                                                CampaignProgress_EncodeCarry(targetLevelId,carry)});
      }
    }
    CampaignProgress_EncodeSection(&entries[0].payload,*section,targets);
  }
  CampaignProgress_SealPayload(&entries[0].payload);
  return entries;
}

/* Reads a payload front to back; the first failed check keeps its reason. */
struct CampaignProgressReader {
  const std::vector<uint8_t> *payload;
  size_t position;
  size_t end;               /* the data byte count of the header */
  const char *error;

  /* the reason of the first failed check */
  const char *Reason() const { return error != nullptr ? error : "malformed"; }

  bool Fail(const char *reason)
  {
    if (error == nullptr) {
      error = reason;
    }
    return false;
  }

  bool Has(size_t byteCount) const { return error == nullptr && byteCount <= end - position; }

  bool U32(uint32_t *outValue,const char *what)
  {
    if (!Has(4)) {
      return Fail(what);
    }
    *outValue = CampaignProgress_LoadU32(payload->data() + position);
    position += 4;
    return true;
  }

  bool Int32(int32_t *outValue,const char *what)
  {
    uint32_t value;

    if (!U32(&value,what)) {
      return false;
    }
    *outValue = static_cast<int32_t>(value);
    return true;
  }

  /* an int32 in [minimum, maximum] */
  bool Range(int32_t minimum,int32_t maximum,int32_t *outValue,const char *what)
  {
    return Int32(outValue,what) && (*outValue >= minimum && *outValue <= maximum ? true : Fail(what));
  }

  /* a count of elements of elementBytes each that fit in the bytes left */
  bool Count(size_t elementBytes,uint32_t *outCount,const char *what)
  {
    return U32(outCount,what) && (*outCount <= (end - position) / elementBytes ? true : Fail(what));
  }
};

/* Checks the payload header (tag, version, data size, padding, CRC32) and opens a reader behind it. */
static bool CampaignProgress_OpenPayload(const std::vector<uint8_t> &payload,uint32_t tag,CampaignProgressReader *reader)

{
  uint32_t dataBytes;

  *reader = CampaignProgressReader{&payload,0,payload.size(),nullptr};
  if (payload.size() < CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES) {
    return reader->Fail("payload shorter than its header");
  }
  if (CampaignProgress_LoadU32(payload.data()) != tag) {
    return reader->Fail("payload has the wrong type tag");
  }
  if (CampaignProgress_LoadU32(payload.data() + 4) != CAMPAIGN_PROGRESS_VERSION) {
    return reader->Fail("payload has another version");
  }
  dataBytes = CampaignProgress_LoadU32(payload.data() + 8);
  if (dataBytes < CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES ||
      payload.size() != dataBytes + (dataBytes % 16 == 0 ? CAMPAIGN_PROGRESS_PAYLOAD_PAD_BYTES : 0) ||
      std::any_of(payload.begin() + dataBytes,payload.end(),[](uint8_t value) { return value != 0; })) {
    return reader->Fail("payload size does not match its header");
  }
  if (CampaignProgress_LoadU32(payload.data() + 12) !=
      CampaignProgress_Crc32(std::span<const uint8_t>(payload.data(),dataBytes).subspan
                             (CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES))) {
    return reader->Fail("payload CRC32 mismatch (damaged or edited)");
  }
  reader->position = CAMPAIGN_PROGRESS_PAYLOAD_HEADER_BYTES;
  reader->end = dataBytes;
  return true;
}

/* A list of positive level ids, ascending without repeats. */
static bool CampaignProgress_ReadLevelIds(CampaignProgressReader *reader,std::vector<int32_t> *outIds,
                                          const char *what)

{
  uint32_t count;
  int32_t levelId;

  if (!reader->Count(4,&count,what)) {
    return false;
  }
  for (uint32_t index = 0; index < count; index++) {
    if (!reader->Range(1,INT32_MAX,&levelId,what)) {
      return false;
    }
    if (!outIds->empty() && levelId <= outIds->back()) {
      return reader->Fail(what);
    }
    outIds->push_back(levelId);
  }
  return true;
}

/* One section of progress.bin; the snapshot targets go to *outTargets. */
static bool CampaignProgress_DecodeSection(CampaignProgressReader *reader,CampaignProgressSection *outSection,
                                           std::vector<int32_t> *outTargets)

{
  const uint8_t *nameField;
  size_t nameLength;
  uint32_t flags;
  uint32_t crc32;
  int32_t recordCount;
  uint32_t winCount;
  int32_t levelId;
  int32_t previousLevelId = 0;
  std::vector<int32_t> unlocked;

  if (!reader->Has(CAMPAIGN_PROGRESS_NAME_FIELD_BYTES)) {
    return reader->Fail("section cut short");
  }
  nameField = reader->payload->data() + reader->position;
  nameLength = 0;
  while (nameLength < CAMPAIGN_PROGRESS_NAME_FIELD_BYTES && nameField[nameLength] != 0) {
    nameLength++;
  }
  outSection->name.assign(reinterpret_cast<const char *>(nameField),nameLength);
  if (!CampaignProgress_IsValidName(outSection->name) ||
      std::any_of(nameField + nameLength,nameField + CAMPAIGN_PROGRESS_NAME_FIELD_BYTES,
                  [](uint8_t value) { return value != 0; })) {
    return reader->Fail("bad campaign name");
  }
  reader->position += CAMPAIGN_PROGRESS_NAME_FIELD_BYTES;
  if (!reader->U32(&flags,"section cut short") || !reader->U32(&crc32,"section cut short") ||
      !reader->Range(0,INT32_MAX,&recordCount,"bad fingerprint record count")) {
    return false;
  }
  if ((flags & ~CAMPAIGN_PROGRESS_FLAG_FINGERPRINT) != 0 ||
      (flags == 0 && (crc32 != 0 || recordCount != 0))) {
    return reader->Fail("bad section flags");
  }
  if (flags != 0) {
    outSection->fingerprint = CampaignProgressFingerprint{crc32,recordCount};
  }
  if (!reader->Count(CAMPAIGN_PROGRESS_WIN_BYTES,&winCount,"bad win count")) {
    return false;
  }
  for (uint32_t index = 0; index < winCount; index++) {
    if (!reader->Range(1,INT32_MAX,&levelId,"bad win level id")) {
      return false;
    }
    const uint8_t *win = reader->payload->data() + reader->position;
    if (levelId <= previousLevelId) {
      return reader->Fail("wins not sorted or repeated");
    }
    if (win[1] >= CAMPAIGN_PROGRESS_SELECTION_COUNT || (win[0] & (1u << win[1])) == 0 || win[2] != 0 ||
        win[3] != 0) {
      return reader->Fail("bad win selections");
    }
    outSection->wins[levelId] = CampaignProgressWins{win[0],win[1]};
    reader->position += 4;
    previousLevelId = levelId;
  }
  if (!CampaignProgress_ReadLevelIds(reader,&unlocked,"bad unlocked level list") ||
      !CampaignProgress_ReadLevelIds(reader,outTargets,"bad snapshot target list")) {
    return false;
  }
  outSection->unlockedLevelIds.insert(unlocked.begin(),unlocked.end());
  return true;
}

/* A carry entry for targetLevelId into *outCarry. */
static bool CampaignProgress_DecodeCarry(const std::vector<uint8_t> &payload,int32_t targetLevelId,
                                         CampaignProgressCarry *outCarry,const char **outError)

{
  CampaignProgressReader reader;
  int32_t storedTarget;
  uint32_t reserved;
  int32_t unitCount;
  uint32_t dwords[CAMPAIGN_PROGRESS_CARRY_RECORD_DWORDS];

  if (!CampaignProgress_OpenPayload(payload,CAMPAIGN_PROGRESS_CARRY_TAG,&reader) ||
      reader.end != CAMPAIGN_PROGRESS_CARRY_BYTES) {
    *outError = reader.error != nullptr ? reader.error : "snapshot has the wrong size";
    return false;
  }
  if (reader.Int32(&storedTarget,"snapshot cut short") && storedTarget != targetLevelId) {
    reader.Fail("snapshot target does not match its entry");
  }
  reader.Range(1,INT32_MAX,&outCarry->sourceLevelId,"bad snapshot source level");
  reader.Range(0,CAMPAIGN_PROGRESS_SELECTION_COUNT - 1,&outCarry->selection,"bad snapshot selection");
  if (reader.U32(&reserved,"snapshot cut short") && reserved != 0) {
    reader.Fail("snapshot reserved dword not 0");
  }
  reader.Range(0,CAMPAIGN_PROGRESS_UNIT_CAPACITY,&unitCount,"snapshot unit count not 0..0x200");
  for (int32_t record = 0; reader.error == nullptr && record < CAMPAIGN_PROGRESS_UNIT_CAPACITY; record++) {
    for (uint32_t &dword : dwords) {
      reader.U32(&dword,"snapshot cut short");
    }
    if (record < unitCount) {
      if (dwords[1] >= static_cast<uint32_t>(CAMPAIGN_PROGRESS_FACTION_COUNT)) {
        reader.Fail("snapshot unit faction not 0..7");
      }
      outCarry->units.push_back(CampaignProgressUnit{static_cast<int32_t>(dwords[0]),static_cast<int32_t>(dwords[1]),
                                                     static_cast<int32_t>(dwords[2]),static_cast<int32_t>(dwords[3]),
                                                     dwords[4]});
    }
    if (std::any_of(dwords + (record < unitCount ? 5 : 0),std::end(dwords),[](uint32_t value) { return value != 0; })) {
      reader.Fail("snapshot unused record dwords not 0");
    }
  }
  for (uint32_t &word : outCarry->techMasks) {
    reader.U32(&word,"snapshot cut short");
  }
  if (reader.error == nullptr && reader.position != reader.end) {
    reader.Fail("snapshot has extra bytes");
  }
  *outError = reader.error;
  return reader.error == nullptr;
}

static bool CampaignProgress_Reject(CampaignProgress *outProgress,std::string *outReason,const std::string &reason)

{
  outProgress->campaigns.clear();
  if (outReason != nullptr) {
    *outReason = reason;
  }
  return false;
}

bool CampaignProgress_DecodeEntries(const std::vector<CampaignProgressEntry> &entries,CampaignProgress *outProgress,
                                    std::string *outReason)

{
  CampaignProgressReader reader;
  uint32_t sectionCount;
  size_t nextEntry = 1;
  const char *error = nullptr;

  outProgress->campaigns.clear();
  if (entries.empty() || entries[0].path != CAMPAIGN_PROGRESS_MAIN_ENTRY) {
    return CampaignProgress_Reject(outProgress,outReason,"progress.bin is not the first entry");
  }
  if (!CampaignProgress_OpenPayload(entries[0].payload,CAMPAIGN_PROGRESS_TAG,&reader)) {
    return CampaignProgress_Reject(outProgress,outReason,std::string("progress.bin: ") + reader.Reason());
  }
  /* a section takes at least its name and six dwords */
  if (!reader.Count(CAMPAIGN_PROGRESS_NAME_FIELD_BYTES + 6 * 4,&sectionCount,"bad section count")) {
    return CampaignProgress_Reject(outProgress,outReason,std::string("progress.bin: ") + reader.Reason());
  }
  outProgress->campaigns.reserve(sectionCount);
  for (uint32_t index = 0; index < sectionCount; index++) {
    CampaignProgressSection section;
    std::vector<int32_t> targets;
    if (!CampaignProgress_DecodeSection(&reader,&section,&targets)) {
      return CampaignProgress_Reject(outProgress,outReason,std::string("progress.bin: ") + reader.Reason());
    }
    if (!outProgress->campaigns.empty() && section.name <= outProgress->campaigns.back().name) {
      return CampaignProgress_Reject(outProgress,outReason,"progress.bin: sections not sorted or repeated");
    }
    for (int32_t targetLevelId : targets) {
      CampaignProgressCarry carry{};
      if (nextEntry >= entries.size() ||
          entries[nextEntry].path != CampaignProgress_CarryEntryPath(section.name,targetLevelId)) {
        return CampaignProgress_Reject(outProgress,outReason,"a snapshot entry is missing or out of order");
      }
      if (!CampaignProgress_DecodeCarry(entries[nextEntry].payload,targetLevelId,&carry,&error)) {
        return CampaignProgress_Reject(outProgress,outReason,entries[nextEntry].path + ": " + error);
      }
      section.carries.emplace(targetLevelId,std::move(carry));
      nextEntry++;
    }
    outProgress->campaigns.push_back(std::move(section));
  }
  if (reader.position != reader.end) {
    return CampaignProgress_Reject(outProgress,outReason,"progress.bin has extra bytes");
  }
  if (nextEntry != entries.size()) {
    return CampaignProgress_Reject(outProgress,outReason,"the package has entries no section names");
  }
  return true;
}

/* --- Package --- */

/* text (ASCII) into a zero-terminated UTF-16 field of capacity units */
static void CampaignProgress_CopyUtf16(uint16_t *destination,size_t capacity,std::string_view text)

{
  for (size_t index = 0; index < capacity; index++) {
    destination[index] = index < text.size() ? static_cast<uint8_t>(text[index]) : 0;
  }
  destination[capacity - 1] = 0;
}

/* payload packed with method 0 into *outPacked. The encoder fills its whole capacity with zeros and may report up
   to 0x1F bytes past what it wrote (the slack the decoder's dword reads need), so it is given 0x20 bytes less
   than the buffer. */
static bool CampaignProgress_PackPayload(const std::vector<uint8_t> &payload,std::vector<uint8_t> *outPacked)

{
  uint32_t packedBytes = 0;
  uint32_t errorCode = 0;
  const size_t capacity = PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + CAMPAIGN_PROGRESS_PACK_BYTES_PER_BYTE * payload.size() +
                          CAMPAIGN_PROGRESS_PACK_SLACK_BYTES;
  std::vector<uint8_t> source(payload);

  outPacked->assign(capacity,0);
  if (payload.empty() || payload.size() > CAMPAIGN_PROGRESS_ENTRY_LIMIT_BYTES ||
      !PckCodec_EncodeHuffmanRle(static_cast<PckOutputCapacityBytes>(capacity - 0x20),outPacked->data(),
                                 static_cast<PckDecodedByteCount>(source.size()),source.data(),&packedBytes,
                                 &errorCode) ||
      packedBytes < CAMPAIGN_PROGRESS_MIN_PACKED_BYTES || packedBytes > capacity) {
    return false;
  }
  outPacked->resize(packedBytes);
  return true;
}

std::optional<std::vector<uint8_t>> CampaignProgress_WritePackage(const std::vector<CampaignProgressEntry> &entries)

{
  std::vector<uint8_t> file(PCK_ENTRY_HEADER_BYTES,0);
  PckArchiveHeader archive{};
  std::vector<uint8_t> packed;

  for (const CampaignProgressEntry &entry : entries) {
    PckEntryHeader header{};
    if (entry.path.size() >= CAMPAIGN_PROGRESS_PATH_UNITS || !CampaignProgress_PackPayload(entry.payload,&packed)) {
      Thandor_Log("campaign progress: entry \"%s\" (%u bytes) cannot be packed, nothing written",entry.path.c_str(),
                  static_cast<unsigned>(entry.payload.size()));
      return std::nullopt;
    }
    CampaignProgress_CopyUtf16(header.path,CAMPAIGN_PROGRESS_PATH_UNITS,entry.path);
    header.runtimePayloadOffset = 0;
    header.unpackedSize = static_cast<PckDecodedByteCount>(entry.payload.size());
    header.typeTag = static_cast<PckAssetTypeTag>(entry.payload.size() >= 4 ? CampaignProgress_LoadU32(entry.payload.data())
                                                                             : 0);
    header.packedSize = static_cast<PckStoredByteCount>(packed.size());
    header.compressionMethod = PCK_COMPRESSION_HUFFMAN_RLE;
    const uint8_t *headerBytes = reinterpret_cast<const uint8_t *>(&header);
    file.insert(file.end(),headerBytes,headerBytes + sizeof header);
    file.insert(file.end(),packed.begin(),packed.end());
  }
  archive.magic[0] = 'p';
  archive.magic[1] = 'c';
  archive.magic[2] = 'k';
  archive.magic[3] = 0;
  archive.archiveSize = static_cast<PckArchiveByteCount>(file.size());
  archive.version = CAMPAIGN_PROGRESS_PCK_VERSION;
  archive.format = CAMPAIGN_PROGRESS_PCK_FORMAT;
  CampaignProgress_CopyUtf16(archive.producerName,std::size(archive.producerName),CAMPAIGN_PROGRESS_PRODUCER);
  CampaignProgress_CopyUtf16(archive.sourceName,std::size(archive.sourceName),CAMPAIGN_PROGRESS_PRODUCER);
  archive.entryCount = static_cast<PckEntryCount>(entries.size());
  memcpy(file.data(),&archive,sizeof archive);
  return file;
}

static bool CampaignProgress_PackageError(std::string *outReason,const char *reason,size_t entryIndex)

{
  char text[96];

  if (outReason != nullptr) {
    snprintf(text,sizeof text,"package entry %u: %s",static_cast<unsigned>(entryIndex),reason);
    *outReason = text;
  }
  return false;
}

bool CampaignProgress_ReadPackage(std::span<const uint8_t> bytes,std::vector<CampaignProgressEntry> *outEntries,
                                  std::string *outReason)

{
  PckArchiveHeader archive;
  PckEntryHeader header;
  size_t offset = PCK_ENTRY_HEADER_BYTES;
  size_t pathLength;
  std::vector<uint8_t> packed;

  outEntries->clear();
  if (bytes.size() < PCK_ENTRY_HEADER_BYTES) {
    if (outReason != nullptr) {
      *outReason = "shorter than a package header";
    }
    return false;
  }
  memcpy(&archive,bytes.data(),sizeof archive);
  if (memcmp(archive.magic,"pck",4) != 0 || archive.version != CAMPAIGN_PROGRESS_PCK_VERSION ||
      archive.format != CAMPAIGN_PROGRESS_PCK_FORMAT || archive.archiveSize != bytes.size() ||
      archive.entryCount == 0 || archive.entryCount > (bytes.size() - offset) / PCK_ENTRY_HEADER_BYTES) {
    if (outReason != nullptr) {
      *outReason = "not a progress package (magic, version, format, size or entry count)";
    }
    return false;
  }
  for (size_t index = 0; index < archive.entryCount; index++) {
    if (bytes.size() - offset < PCK_ENTRY_HEADER_BYTES) {
      return CampaignProgress_PackageError(outReason,"header cut short",index);
    }
    memcpy(&header,bytes.data() + offset,sizeof header);
    offset += PCK_ENTRY_HEADER_BYTES;
    CampaignProgressEntry entry;
    for (pathLength = 0; pathLength < CAMPAIGN_PROGRESS_PATH_UNITS && header.path[pathLength] != 0; pathLength++) {
      const uint16_t unit = header.path[pathLength];
      if (unit < 0x20 || unit >= 0x7F || (unit >= 'A' && unit <= 'Z')) {
        return CampaignProgress_PackageError(outReason,"path not lowercase ASCII",index);
      }
      entry.path.push_back(static_cast<char>(unit));
    }
    if (pathLength == 0 || pathLength == CAMPAIGN_PROGRESS_PATH_UNITS) {
      return CampaignProgress_PackageError(outReason,"bad path",index);
    }
    if (header.runtimePayloadOffset != 0 || header.compressionMethod != PCK_COMPRESSION_HUFFMAN_RLE) {
      return CampaignProgress_PackageError(outReason,"payload offset or compression method not 0",index);
    }
    if (header.packedSize < CAMPAIGN_PROGRESS_MIN_PACKED_BYTES || header.packedSize > bytes.size() - offset ||
        header.unpackedSize < 4 || header.unpackedSize > CAMPAIGN_PROGRESS_ENTRY_LIMIT_BYTES) {
      return CampaignProgress_PackageError(outReason,"bad packed or unpacked size",index);
    }
    /* the decoder takes non-const pointers: a copy of the packed bytes */
    packed.assign(bytes.begin() + offset,bytes.begin() + offset + header.packedSize);
    entry.payload.assign(header.unpackedSize,0);
    if (!PckCodec_DecodeHuffmanRle(header.unpackedSize,entry.payload.data(),header.packedSize,packed.data(),nullptr,
                                   nullptr)) {
      return CampaignProgress_PackageError(outReason,"payload does not unpack",index);
    }
    if (static_cast<uint32_t>(header.typeTag) != CampaignProgress_LoadU32(entry.payload.data())) {
      return CampaignProgress_PackageError(outReason,"type tag is not the first payload dword",index);
    }
    offset += header.packedSize;
    outEntries->push_back(std::move(entry));
  }
  if (offset != bytes.size()) {
    if (outReason != nullptr) {
      *outReason = "bytes behind the last entry";
    }
    outEntries->clear();
    return false;
  }
  return true;
}

std::optional<std::vector<uint8_t>> CampaignProgress_Encode(const CampaignProgress &progress)

{
  return CampaignProgress_WritePackage(CampaignProgress_EncodeEntries(progress));
}

bool CampaignProgress_Decode(std::span<const uint8_t> bytes,CampaignProgress *outProgress,std::string *outReason)

{
  std::vector<CampaignProgressEntry> entries;

  if (!CampaignProgress_ReadPackage(bytes,&entries,outReason)) {
    outProgress->campaigns.clear();
    return false;
  }
  return CampaignProgress_DecodeEntries(entries,outProgress,outReason);
}

/* --- File --- */

/* Renames the file at path to the same name with the extension ".bad" (one log line with the reason). */
static void CampaignProgress_KeepAsBad(uint16_t *path,const char *reason)

{
  uint16_t badPath [THANDOR_PATH_CAPACITY];
  size_t length = 0;
  size_t extension;
  size_t index;

  while (length < THANDOR_PATH_CAPACITY && path[length] != 0) {
    badPath[length] = path[length];
    length++;
  }
  /* the dot of the leaf's extension, else the end of the path (".bad" is appended) */
  extension = length;
  for (index = length; index > 0; index--) {
    if (path[index - 1] == '\\' || path[index - 1] == '/') {
      break;
    }
    if (path[index - 1] == '.') {
      extension = index - 1;
      break;
    }
  }
  if (extension + 1 + CAMPAIGN_PROGRESS_BAD_EXTENSION.size() > THANDOR_PATH_CAPACITY) {
    Thandor_Log("campaign progress: \"%ls\" %s; path too long to keep it as .bad, ignored",
                reinterpret_cast<wchar_t *>(path),reason); /* UTF-16 for %ls */
    return;
  }
  badPath[extension] = '.';
  std::copy(CAMPAIGN_PROGRESS_BAD_EXTENSION.begin(),CAMPAIGN_PROGRESS_BAD_EXTENSION.end(),badPath + extension + 1);
  if (Win32File_Replace(badPath,path) != 0) {
    Thandor_Log("campaign progress: \"%ls\" %s; renaming it to .bad failed, ignored",
                reinterpret_cast<wchar_t *>(path),reason);
    return;
  }
  Thandor_Log("campaign progress: \"%ls\" %s; kept as \"%ls\", progress starts empty",
              reinterpret_cast<wchar_t *>(path),reason,reinterpret_cast<wchar_t *>(badPath));
}

CampaignProgress CampaignProgress_LoadFile(uint16_t *path)

{
  CampaignProgress progress;
  std::optional<std::vector<uint8_t>> bytes = FileSystem_ReadWholeFileNative(path);
  std::string reason;
  void *handle;

  if (!bytes.has_value()) {
    /* missing (first run), or present but unreadable or over the size limit */
    if (Win32File_Open(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,path,&handle) == 0) {
      Win32File_Close(handle);
      CampaignProgress_KeepAsBad(path,"cannot be read or is too large");
    }
    return progress;
  }
  if (!CampaignProgress_Decode(*bytes,&progress,&reason)) {
    reason = "is not a valid version 1 progress file (" + reason + ")";
    CampaignProgress_KeepAsBad(path,reason.c_str());
    progress.campaigns.clear();
  }
  return progress;
}

uint32_t CampaignProgress_SaveFile(uint16_t *path,const CampaignProgress &progress)

{
  std::optional<std::vector<uint8_t>> bytes = CampaignProgress_Encode(progress);
  uint32_t writeError;

  if (!bytes.has_value()) {
    return FATAL_ERROR_GENERAL_FAILURE; /* CampaignProgress_WritePackage logged it */
  }
  writeError = FileSystem_WriteFileReplacing(path,*bytes);
  if (writeError != 0) {
    Thandor_Log("campaign progress: writing \"%ls\" failed (error 0x%08X), the old file is kept",
                reinterpret_cast<wchar_t *>(path),writeError);
  }
  return writeError;
}

/* "kampagne.dat", the leaf of the progress file in the save directory */
static constexpr uint16_t CAMPAIGN_PROGRESS_FILE_LEAF[] = {'k','a','m','p','a','g','n','e','.','d','a','t',0};

/* The progress file: save\kampagne.dat under the executable directory. A dev-tools build takes
   OPEN_THANDOR_CAMPAIGN_PROGRESS=<path> instead when it is set. pathCapacity is in code units. */
static void CampaignProgress_GetFilePath(uint16_t *outPath,size_t pathCapacity)

{
  uint16_t leaf[THANDOR_PATH_CAPACITY];

#ifdef THANDOR_DEV_TOOLS
  /* test aid: OPEN_THANDOR_CAMPAIGN_PROGRESS=<path> (plan 4.4, 10) */
  const char *overridePath = getenv("OPEN_THANDOR_CAMPAIGN_PROGRESS");
  if (overridePath != nullptr && overridePath[0] != 0 && strlen(overridePath) < pathCapacity) {
    Text_CopyNarrowToUtf16(static_cast<TextOutputCapacityBytes>(pathCapacity * sizeof(uint16_t)),outPath,
                           reinterpret_cast<uint8_t *>(const_cast<char *>(overridePath)));
    return;
  }
#endif
  WidePath_CombineDirectoryAndLeaf(leaf,const_cast<uint16_t *>(CAMPAIGN_PROGRESS_FILE_LEAF),g_SaveDirectoryUtf16);
  WidePath_CombineDirectoryAndLeafBounded(outPath,pathCapacity,leaf,g_ExecutableDirectoryUtf16);
}

/* Writes progress to path; when that fails, creates the file's directory (as InGameSaveGame_OpenNewPackage does
   for the saves) and tries once more through CampaignProgress_SaveFile, which logs a second failure. */
static void CampaignProgress_WriteCreatingDirectory(uint16_t *path,const CampaignProgress &progress)

{
  uint16_t directory[THANDOR_PATH_CAPACITY];
  size_t length = 0;
  size_t separator = 0;
  std::optional<std::vector<uint8_t>> bytes = CampaignProgress_Encode(progress);

  if (!bytes.has_value()) {
    return; /* CampaignProgress_WritePackage logged it; the old file stays */
  }
  if (FileSystem_WriteFileReplacing(path,*bytes) == 0) {
    return;
  }
  while (length + 1 < THANDOR_PATH_CAPACITY && path[length] != 0) {
    directory[length] = path[length];
    if (path[length] == '\\' || path[length] == '/') {
      separator = length;
    }
    length++;
  }
  if (separator != 0) {
    directory[separator] = 0;
    g_FileSystemCreateDirectoryRecursive(FileSystemCreateDirectoryFlags::FILESYSTEM_CREATE_DIRECTORY_RECURSIVE,
                                         directory);
  }
  if (FileSystem_WriteFileReplacing(path,*bytes) != 0) {
    Thandor_Log("campaign progress: writing \"%ls\" failed, the old file is kept",reinterpret_cast<wchar_t *>(path));
  }
}

CampaignProgress CampaignProgress_LoadCurrent()

{
  uint16_t path[THANDOR_PATH_CAPACITY];

  CampaignProgress_GetFilePath(path,THANDOR_PATH_CAPACITY);
  return CampaignProgress_LoadFile(path);
}

void CampaignProgress_SaveCurrent(const CampaignProgress &progress)

{
  uint16_t path[THANDOR_PATH_CAPACITY];

  CampaignProgress_GetFilePath(path,THANDOR_PATH_CAPACITY);
  CampaignProgress_WriteCreatingDirectory(path,progress);
}
