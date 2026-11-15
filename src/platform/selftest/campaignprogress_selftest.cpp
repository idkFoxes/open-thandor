/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/campaignprogress_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=campaignprogress: the campaign progress model and the kampagne.dat package format
   (src/gameplay/session/campaign_progress.cpp) on fixed models: a round trip of a model with every item kind (bytes
   and file), the PCK and payload layout, a padded payload, truncated and extended files, a bad archive magic and
   version, bad payload tags and versions, CRC32 mismatches, oversize counts, values outside their ranges, unsorted
   and repeated sections and lists, missing and extra entries, the items the encoder leaves out, the win rule and
   the carry-over snapshot take of the session hooks. The file cases run in a fresh directory under %TEMP% (the
   current directory without TEMP), which is removed afterwards; a rejected file must be renamed to ".bad". No game
   data is needed. One line per failed check, then "campaignprogress: <n> of <m> checks failed". The decoder's own
   log lines for the broken inputs are expected. */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

constexpr auto CAMPAIGNPROGRESS_TEST_PATH_BYTES = 0x200;
/* progress.bin offsets of the test model: the section count and the first section's fields */
constexpr size_t CAMPAIGNPROGRESS_TEST_SECTION_COUNT = 16;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_NAME = 20;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_FLAGS = 52;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_RECORDS = 60;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_WIN_COUNT = 64;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_WIN = 68;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_UNLOCKED_COUNT = 84;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_UNLOCKED = 88;
constexpr size_t CAMPAIGNPROGRESS_TEST_FIRST_CARRY_COUNT = 168;
constexpr size_t CAMPAIGNPROGRESS_TEST_SECOND_NAME = 176;
/* carry payload offsets */
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_TARGET = 16;
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_SOURCE = 20;
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_SELECTION = 24;
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_RESERVED = 28;
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_COUNT = 32;
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_RECORDS = 36;
constexpr size_t CAMPAIGNPROGRESS_TEST_CARRY_TECH = CAMPAIGNPROGRESS_TEST_CARRY_RECORDS + 0x200 * 32;
/* PckEntryHeader fields */
constexpr size_t CAMPAIGNPROGRESS_TEST_ENTRY_UNPACKED = 0x1F0;
constexpr size_t CAMPAIGNPROGRESS_TEST_ENTRY_TAG = 0x1F4;
constexpr size_t CAMPAIGNPROGRESS_TEST_ENTRY_PACKED = 0x1F8;
constexpr size_t CAMPAIGNPROGRESS_TEST_ENTRY_METHOD = 0x1FC;

/* A path under the test directory, as UTF-16 for the loader and as narrow text for the checks. */
struct CampaignProgressTestPath {
    char narrow[CAMPAIGNPROGRESS_TEST_PATH_BYTES];
    uint16_t wide[THANDOR_PATH_CAPACITY];
};

static unsigned s_CampaignProgressChecks;
static unsigned s_CampaignProgressFailures;

static void CampaignProgressTest_Check(bool passed, const char *what)
{
    s_CampaignProgressChecks++;
    if (!passed) {
        s_CampaignProgressFailures++;
        Thandor_Log("campaignprogress FAILED: %s", what);
    }
}

static void CampaignProgressTest_MakePath(CampaignProgressTestPath *path, const char *directory, const char *leaf)
{
    snprintf(path->narrow, sizeof path->narrow, "%s\\%s", directory, leaf);
    Text_CopyNarrowToUtf16(sizeof path->wide, path->wide, reinterpret_cast<uint8_t *>(path->narrow));
}

static bool CampaignProgressTest_Exists(const CampaignProgressTestPath *path)
{
    _WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA(path->narrow, &findData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }
    FindClose(findHandle);
    return true;
}

static bool CampaignProgressTest_WriteBytes(const CampaignProgressTestPath *path, const std::vector<uint8_t> &bytes)
{
    FILE *file = fopen(path->narrow, "wb");
    bool written;

    if (file == nullptr) {
        return false;
    }
    written = fwrite(bytes.data(), 1, bytes.size(), file) == bytes.size();
    return fclose(file) == 0 && written;
}

static uint32_t CampaignProgressTest_Get(const std::vector<uint8_t> &bytes, size_t offset)
{
    uint32_t value = 0;

    if (offset + 4 <= bytes.size()) {
        memcpy(&value, bytes.data() + offset, 4);
    }
    return value;
}

static void CampaignProgressTest_Put(std::vector<uint8_t> *bytes, size_t offset, uint32_t value)
{
    if (offset + 4 <= bytes->size()) {
        memcpy(bytes->data() + offset, &value, 4);
    }
}

/* Stores value at offset of a sealed payload and seals it again (size, CRC32 and padding stay valid). */
static void CampaignProgressTest_PatchSealed(std::vector<uint8_t> *payload, size_t offset, uint32_t value)
{
    payload->resize(CampaignProgressTest_Get(*payload, 8)); /* drop the padding */
    CampaignProgressTest_Put(payload, offset, value);
    CampaignProgress_SealPayload(payload);
}

/* The file bytes of entries, or empty when the package cannot be written. */
static std::vector<uint8_t> CampaignProgressTest_Package(const std::vector<CampaignProgressEntry> &entries)
{
    std::optional<std::vector<uint8_t>> bytes = CampaignProgress_WritePackage(entries);

    return bytes.has_value() ? *bytes : std::vector<uint8_t>();
}

/* true when the bytes are rejected; the reason must contain expected (when not nullptr). */
static bool CampaignProgressTest_Rejects(const std::vector<uint8_t> &bytes, const char *expected)
{
    CampaignProgress progress;
    std::string reason;

    progress.campaigns.resize(1); /* must be emptied */
    if (CampaignProgress_Decode(bytes, &progress, &reason) || !progress.campaigns.empty()) {
        return false;
    }
    return expected == nullptr || reason.find(expected) != std::string::npos;
}

static bool CampaignProgressTest_EntriesReject(const std::vector<CampaignProgressEntry> &entries, const char *expected)
{
    CampaignProgress progress;
    std::string reason;

    if (CampaignProgress_DecodeEntries(entries, &progress, &reason)) {
        return false;
    }
    if (expected != nullptr && reason.find(expected) == std::string::npos) {
        Thandor_Log("campaignprogress: rejected for \"%s\", expected \"%s\"", reason.c_str(), expected);
        return false;
    }
    /* the same entries through a package as well */
    return CampaignProgressTest_Rejects(CampaignProgressTest_Package(entries), expected);
}

/* A model with every item kind: two campaigns, a level won with two selections where the last one is not the
   highest, 20 unlocked ids, a full snapshot of 0x200 units with extreme values and a small one with technology
   masks, and an empty section. */
static CampaignProgress CampaignProgressTest_Model()
{
    CampaignProgress progress;
    CampaignProgressCarry full{};
    CampaignProgressCarry small{};
    int index;

    /* each section is filled before the next insert (an insert may move the vector) */
    CampaignProgressSection &hansolo = CampaignProgress_GetOrAddSection(progress, "hansolo");
    hansolo.fingerprint = CampaignProgressFingerprint{0x9F3A12C4u, 26};
    CampaignProgressSection_RecordWin(hansolo, 2, 1);
    CampaignProgressSection_RecordWin(hansolo, 5, 2);
    CampaignProgressSection_RecordWin(hansolo, 5, 1); /* last win: selection 1 */
    for (index = 2; index <= 21; index++) {
        hansolo.unlockedLevelIds.insert(index);
    }
    full.sourceLevelId = 8;
    full.selection = 1;
    for (index = 0; index < CAMPAIGN_PROGRESS_UNIT_CAPACITY; index++) {
        CampaignProgressUnit unit;
        unit.assetId = index == 0 ? INT32_MIN : index == 1 ? -1 : 0x41 + index;
        unit.faction = index % CAMPAIGN_PROGRESS_FACTION_COUNT;
        unit.worldXQ12 = index == 2 ? INT32_MIN : 1234567 + index * 4096;
        unit.worldYQ12 = index == 2 ? INT32_MAX : -234567 - index;
        unit.rotation = index == 3 ? 0xFFFFFFFFu : static_cast<uint32_t>(index * 0x80);
        full.units.push_back(unit);
    }
    full.techMasks[0] = 1;
    full.techMasks[CAMPAIGN_PROGRESS_TECH_WORD_COUNT - 1] = 0x80000000u;
    hansolo.carries[9] = full;

    CampaignProgressSection &tutorial = CampaignProgress_GetOrAddSection(progress, "tutorial");
    CampaignProgressSection_RecordWin(tutorial, 1, 1);
    small.sourceLevelId = 1;
    small.selection = 1;
    small.units.push_back(CampaignProgressUnit{0x1234, 1, 4096, 8192, 0x4000});
    small.techMasks[8] = 0xDEADBEEFu;
    tutorial.carries[2] = small;

    CampaignProgress_GetOrAddSection(progress, "luke_2"); /* empty */
    return progress;
}

static void CampaignProgressTest_RoundTrip()
{
    CampaignProgress model = CampaignProgressTest_Model();
    std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
    std::optional<std::vector<uint8_t>> bytes = CampaignProgress_Encode(model);
    std::optional<std::vector<uint8_t>> again;
    CampaignProgress decoded;
    std::string reason;
    const CampaignProgressSection *hansolo;

    CampaignProgressTest_Check(model.campaigns.size() == 3 && model.campaigns[0].name == "hansolo" &&
                               model.campaigns[1].name == "luke_2" && model.campaigns[2].name == "tutorial",
                               "round trip: sections sorted by name");
    CampaignProgressTest_Check(entries.size() == 3 && entries[0].path == "progress.bin" &&
                               entries[1].path == "carry_hansolo_9.hex" && entries[2].path == "carry_tutorial_2.hex",
                               "round trip: entry paths and order");
    if (!bytes.has_value() || entries.size() != 3) {
        CampaignProgressTest_Check(false, "round trip: encode");
        return;
    }
    CampaignProgressTest_Check(CampaignProgress_Decode(*bytes, &decoded, &reason) && decoded == model,
                               "round trip: decoded model equals the encoded one");
    again = CampaignProgress_Encode(decoded);
    CampaignProgressTest_Check(again.has_value() && *again == *bytes, "round trip: encoding again is identical");
    hansolo = CampaignProgress_FindSection(decoded, "hansolo");
    CampaignProgressTest_Check(hansolo != nullptr && hansolo->wins.at(5).selectionMask == 0x6 &&
                               hansolo->wins.at(5).lastSelection == 1, "round trip: last win selection kept");
    CampaignProgressTest_Check(CampaignProgress_FindSection(decoded, "nimm2") == nullptr,
                               "round trip: missing section not found");

    /* empty progress */
    CampaignProgress empty;
    std::optional<std::vector<uint8_t>> emptyBytes = CampaignProgress_Encode(empty);
    decoded.campaigns.resize(2);
    CampaignProgressTest_Check(emptyBytes.has_value() && CampaignProgress_Decode(*emptyBytes, &decoded, &reason) &&
                               decoded.campaigns.empty(), "round trip: empty progress");

    /* a progress.bin of 80 data bytes (one section "a", one unlocked id) gets 4 padding bytes */
    CampaignProgress padded;
    CampaignProgress_GetOrAddSection(padded, "a").unlockedLevelIds.insert(7);
    std::vector<CampaignProgressEntry> paddedEntries = CampaignProgress_EncodeEntries(padded);
    std::optional<std::vector<uint8_t>> paddedBytes = CampaignProgress_Encode(padded);
    CampaignProgressTest_Check(paddedEntries[0].payload.size() == 84 &&
                               CampaignProgressTest_Get(paddedEntries[0].payload, 8) == 80 &&
                               CampaignProgressTest_Get(paddedEntries[0].payload, 80) == 0,
                               "padded payload: data size 80, 84 bytes");
    CampaignProgressTest_Check(paddedBytes.has_value() && CampaignProgress_Decode(*paddedBytes, &decoded, &reason) &&
                               decoded == padded, "padded payload: round trip");
    paddedEntries[0].payload[83] = 1;
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(paddedEntries, "size does not match"),
                               "padded payload: a nonzero padding byte is rejected");
}

/* The PCK archive and entry headers and the payload layouts. */
static void CampaignProgressTest_Layout()
{
    CampaignProgress model = CampaignProgressTest_Model();
    std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
    std::vector<uint8_t> bytes = CampaignProgressTest_Package(entries);
    std::vector<CampaignProgressEntry> read;
    std::string reason;
    size_t offset = 0x200;
    size_t index;
    bool headersGood = true;
    const char *const name = "progress.bin";

    CampaignProgressTest_Check(bytes.size() > 0x400 && memcmp(bytes.data(), "pck\0", 4) == 0 &&
                               CampaignProgressTest_Get(bytes, 4) == bytes.size() &&
                               CampaignProgressTest_Get(bytes, 8) == 1 && CampaignProgressTest_Get(bytes, 12) == 0x10000 &&
                               CampaignProgressTest_Get(bytes, 0xB0) == 3 && bytes[0x100] == 0,
                               "layout: archive header");
    for (index = 0; index < 12 && offset + 0x200 <= bytes.size(); index++) {
        headersGood = headersGood && bytes[offset + index * 2] == static_cast<uint8_t>(name[index]) &&
                      bytes[offset + index * 2 + 1] == 0;
    }
    for (index = 0; index < entries.size() && offset + 0x200 <= bytes.size(); index++) {
        uint32_t unpacked = CampaignProgressTest_Get(bytes, offset + CAMPAIGNPROGRESS_TEST_ENTRY_UNPACKED);
        uint32_t packed = CampaignProgressTest_Get(bytes, offset + CAMPAIGNPROGRESS_TEST_ENTRY_PACKED);
        headersGood = headersGood && CampaignProgressTest_Get(bytes, offset + 0x1EC) == 0 &&
                      unpacked == entries[index].payload.size() &&
                      CampaignProgressTest_Get(bytes, offset + CAMPAIGNPROGRESS_TEST_ENTRY_TAG) ==
                          CampaignProgressTest_Get(entries[index].payload, 0) &&
                      CampaignProgressTest_Get(bytes, offset + CAMPAIGNPROGRESS_TEST_ENTRY_METHOD) == 0 &&
                      packed % 16 == 0 && packed != unpacked && packed >= 0x104;
        offset += 0x200 + packed;
    }
    CampaignProgressTest_Check(headersGood && offset == bytes.size(), "layout: entry headers");
    CampaignProgressTest_Check(CampaignProgress_ReadPackage(bytes, &read, &reason) && read == entries,
                               "layout: the package gives the entries back");
    CampaignProgressTest_Check(CampaignProgressTest_Get(entries[0].payload, 0) == CAMPAIGN_PROGRESS_TAG &&
                               CampaignProgressTest_Get(entries[0].payload, 4) == CAMPAIGN_PROGRESS_VERSION &&
                               CampaignProgressTest_Get(entries[0].payload, CAMPAIGNPROGRESS_TEST_SECTION_COUNT) == 3,
                               "layout: progress.bin header");

    /* the oldunit.hex part of the hansolo snapshot */
    const std::vector<uint8_t> &carry = entries[1].payload;
    const CampaignProgressCarry &full = model.campaigns[0].carries.at(9);
    bool recordsGood = carry.size() == CAMPAIGN_PROGRESS_CARRY_BYTES && carry.size() == 0x4124 &&
                       CampaignProgressTest_Get(carry, 0) == CAMPAIGN_PROGRESS_CARRY_TAG &&
                       CampaignProgressTest_Get(carry, CAMPAIGNPROGRESS_TEST_CARRY_TARGET) == 9 &&
                       CampaignProgressTest_Get(carry, CAMPAIGNPROGRESS_TEST_CARRY_SOURCE) == 8 &&
                       CampaignProgressTest_Get(carry, CAMPAIGNPROGRESS_TEST_CARRY_SELECTION) == 1 &&
                       CampaignProgressTest_Get(carry, CAMPAIGNPROGRESS_TEST_CARRY_COUNT) == 0x200;
    for (index = 0; recordsGood && index < full.units.size(); index++) {
        size_t record = CAMPAIGNPROGRESS_TEST_CARRY_RECORDS + index * 32;
        recordsGood = CampaignProgressTest_Get(carry, record) == static_cast<uint32_t>(full.units[index].assetId) &&
                      CampaignProgressTest_Get(carry, record + 4) == static_cast<uint32_t>(full.units[index].faction) &&
                      CampaignProgressTest_Get(carry, record + 8) == static_cast<uint32_t>(full.units[index].worldXQ12) &&
                      CampaignProgressTest_Get(carry, record + 12) == static_cast<uint32_t>(full.units[index].worldYQ12) &&
                      CampaignProgressTest_Get(carry, record + 16) == full.units[index].rotation &&
                      CampaignProgressTest_Get(carry, record + 20) == 0 && CampaignProgressTest_Get(carry, record + 28) == 0;
    }
    recordsGood = recordsGood && CampaignProgressTest_Get(carry, CAMPAIGNPROGRESS_TEST_CARRY_TECH) == 1 &&
                  CampaignProgressTest_Get(carry, CAMPAIGNPROGRESS_TEST_CARRY_TECH + 63 * 4) == 0x80000000u;
    CampaignProgressTest_Check(recordsGood, "layout: snapshot in the oldunit.hex layout");
}

/* Cut and extended files, and a package whose last entry was removed cleanly. */
static void CampaignProgressTest_Truncated()
{
    CampaignProgress model = CampaignProgressTest_Model();
    std::vector<uint8_t> bytes = CampaignProgressTest_Package(CampaignProgress_EncodeEntries(model));
    const size_t cuts[] = {0, 1, 4, 0x1FF, 0x200, 0x201, 0x3FF, 0x400, 0x500, bytes.size() / 2, bytes.size() - 16,
                           bytes.size() - 1};
    char label[64];

    for (size_t cut : cuts) {
        snprintf(label, sizeof label, "truncated at %u: rejected", static_cast<unsigned>(cut));
        CampaignProgressTest_Check(CampaignProgressTest_Rejects(std::vector<uint8_t>(bytes.begin(),
                                                                                     bytes.begin() + cut), nullptr),
                                   label);
    }
    std::vector<uint8_t> longer = bytes;
    longer.push_back(0);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(longer, "magic, version"), "one byte more: rejected");
    CampaignProgressTest_Put(&longer, 4, static_cast<uint32_t>(longer.size()));
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(longer, "behind the last entry"),
                               "one byte more with a matching archive size: rejected");

    /* the last entry (the tutorial snapshot) cut off with the archive header fixed: a missing snapshot */
    std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
    entries.pop_back();
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(entries, "missing or out of order"),
                               "missing snapshot entry: rejected");
}

/* Archive and payload magic, version, tag and CRC32. */
static void CampaignProgressTest_Headers()
{
    CampaignProgress model = CampaignProgressTest_Model();
    std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
    std::vector<uint8_t> bytes = CampaignProgressTest_Package(entries);
    std::vector<uint8_t> changed;
    std::vector<CampaignProgressEntry> patched;

    changed = bytes;
    changed[0] = 'P';
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "magic"), "archive magic: rejected");
    changed = bytes;
    CampaignProgressTest_Put(&changed, 8, 2);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "magic, version"), "archive version 2: rejected");
    changed = bytes;
    CampaignProgressTest_Put(&changed, 0xB0, 0x7FFFFFFF);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "entry count"), "huge entry count: rejected");
    changed = bytes;
    CampaignProgressTest_Put(&changed, 0x200 + CAMPAIGNPROGRESS_TEST_ENTRY_METHOD, 1);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "compression method"), "method 1: rejected");
    changed = bytes;
    CampaignProgressTest_Put(&changed, 0x200 + CAMPAIGNPROGRESS_TEST_ENTRY_UNPACKED, 0x7FFFFFFF);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "unpacked size"), "huge unpacked size: rejected");
    changed = bytes;
    CampaignProgressTest_Put(&changed, 0x200 + CAMPAIGNPROGRESS_TEST_ENTRY_PACKED, 0x7FFFFFF0);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "packed"), "huge packed size: rejected");
    changed = bytes;
    CampaignProgressTest_Put(&changed, 0x200 + CAMPAIGNPROGRESS_TEST_ENTRY_TAG, 0x00636574);
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "type tag"), "entry type tag: rejected");
    changed = bytes;
    changed[0x200] = 'P'; /* uppercase path */
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, "lowercase"), "uppercase entry path: rejected");
    changed = bytes;
    changed[0x200 + 0x200 + 0x100 + 4] ^= 0x5A; /* the bit stream of progress.bin */
    CampaignProgressTest_Check(CampaignProgressTest_Rejects(changed, nullptr), "damaged packed bytes: rejected");

    patched = entries;
    CampaignProgressTest_PatchSealed(&patched[0].payload, 4, 2);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "another version"),
                               "progress.bin version 2: rejected");
    patched = entries;
    CampaignProgressTest_PatchSealed(&patched[0].payload, 0, CAMPAIGN_PROGRESS_CARRY_TAG);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "wrong type tag"),
                               "progress.bin with the carry tag: rejected");
    patched = entries;
    CampaignProgressTest_PatchSealed(&patched[1].payload, 4, 0);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "another version"),
                               "snapshot version 0: rejected");

    /* edited without a new CRC32 */
    patched = entries;
    CampaignProgressTest_Put(&patched[0].payload, CAMPAIGNPROGRESS_TEST_FIRST_WIN, 3);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "CRC32"), "progress.bin edited: CRC32 mismatch");
    patched = entries;
    CampaignProgressTest_Put(&patched[2].payload, CAMPAIGNPROGRESS_TEST_CARRY_RECORDS, 0x41);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "CRC32"), "snapshot edited: CRC32 mismatch");
    patched = entries;
    CampaignProgressTest_Put(&patched[1].payload, 12, CampaignProgressTest_Get(patched[1].payload, 12) ^ 1);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "CRC32"), "stored CRC32 changed: mismatch");
    patched = entries;
    CampaignProgressTest_Put(&patched[0].payload, 8, CampaignProgressTest_Get(patched[0].payload, 8) - 4);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, "size does not match"),
                               "progress.bin data size changed: rejected");
}

/* Counts larger than the bytes left, values outside their ranges, unsorted and repeated lists, all with a valid
   CRC32. */
static void CampaignProgressTest_Ranges()
{
    CampaignProgress model = CampaignProgressTest_Model();
    const std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
    struct Patch {
        size_t entry;
        size_t offset;
        uint32_t value;
        const char *expected;
        const char *label;
    };
    const Patch patches[] = {
        {0, CAMPAIGNPROGRESS_TEST_SECTION_COUNT, 0xFFFFFFFFu, "section count", "section count 0xFFFFFFFF"},
        {0, CAMPAIGNPROGRESS_TEST_SECTION_COUNT, 4, "cut short", "one section more than stored"},
        {0, CAMPAIGNPROGRESS_TEST_SECTION_COUNT, 2, "extra bytes", "one section less than stored"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN_COUNT, 0x20000000u, "win count", "win count 0x20000000"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_UNLOCKED_COUNT, 0xFFFFFFFFu, "unlocked", "unlocked count 0xFFFFFFFF"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_CARRY_COUNT, 0x40000000u, "snapshot target", "snapshot count 0x40000000"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN, 0, "level id", "win level id 0"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN, 0x80000000u, "level id", "win level id negative"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN + 4, 0x0802, "selections", "win last selection 8"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN + 4, 0x0201, "selections", "win last selection not in its mask"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN + 4, 0x01000102, "selections", "win reserved bytes not 0"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_WIN + 8, 1, "sorted", "wins not sorted"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_UNLOCKED + 4, 2, "unlocked", "unlocked id repeated"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_UNLOCKED, 0, "unlocked", "unlocked id 0"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_FLAGS, 2, "flags", "unknown section flag"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_RECORDS, 0x80000000u, "record count", "negative fingerprint record count"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_NAME, 0x2D61, "name", "campaign name with '-'"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_NAME + 8, 0x41, "name", "bytes behind the name terminator"},
        {0, CAMPAIGNPROGRESS_TEST_FIRST_NAME, 0, "name", "empty campaign name"},
        {0, CAMPAIGNPROGRESS_TEST_SECOND_NAME, 0x6B756C41, "sorted or repeated", "sections not sorted"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_COUNT, 0x201, "unit count", "snapshot of 0x201 units"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_COUNT, 0xFFFFFFFFu, "unit count", "snapshot unit count 0xFFFFFFFF"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_COUNT, 0, "unused record", "unit behind the count"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_RECORDS + 4, 8, "faction", "unit faction 8"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_RECORDS + 4, 0xFFFFFFFFu, "faction", "unit faction -1"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_RECORDS + 20, 1, "unused record", "unit dword 5 not 0"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_RECORDS + 32 * 0x1FF + 28, 1, "unused record", "last unused record not 0"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_TARGET, 3, "target", "snapshot target not its entry"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_SOURCE, 0, "source", "snapshot source level 0"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_SELECTION, 8, "selection", "snapshot selection 8"},
        {1, CAMPAIGNPROGRESS_TEST_CARRY_RESERVED, 1, "reserved", "snapshot reserved dword"},
    };
    char label[96];

    for (const Patch &patch : patches) {
        std::vector<CampaignProgressEntry> patched = entries;
        CampaignProgressTest_PatchSealed(&patched[patch.entry].payload, patch.offset, patch.value);
        snprintf(label, sizeof label, "%s: rejected", patch.label);
        CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(patched, patch.expected), label);
    }
    /* the second section renamed to "hansolo": a repeated section */
    std::vector<CampaignProgressEntry> repeated = entries;
    CampaignProgressTest_PatchSealed(&repeated[0].payload, CAMPAIGNPROGRESS_TEST_SECOND_NAME, 0x736E6168);   /* "hans" */
    CampaignProgressTest_PatchSealed(&repeated[0].payload, CAMPAIGNPROGRESS_TEST_SECOND_NAME + 4, 0x006F6C6F); /* "olo" */
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(repeated, "sorted or repeated"),
                               "repeated section: rejected");

    /* sanity: the unchanged entries decode, so every rejection above comes from its patch */
    CampaignProgress decoded;
    std::string reason;
    CampaignProgressTest_Check(CampaignProgress_DecodeEntries(entries, &decoded, &reason) && decoded == model,
                               "ranges: unchanged entries decode");

    /* a snapshot payload one dword longer */
    std::vector<CampaignProgressEntry> longer = entries;
    longer[1].payload.resize(CAMPAIGN_PROGRESS_CARRY_BYTES);
    longer[1].payload.insert(longer[1].payload.end(), 4, 0);
    CampaignProgress_SealPayload(&longer[1].payload);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(longer, "wrong size"), "snapshot too long: rejected");

    /* entries in the wrong order, an extra entry, an entry under another name */
    std::vector<CampaignProgressEntry> reordered = entries;
    std::swap(reordered[1], reordered[2]);
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(reordered, "out of order"),
                               "snapshot entries swapped: rejected");
    std::vector<CampaignProgressEntry> extra = entries;
    extra.push_back(entries[1]);
    extra.back().path = "carry_zzz_1.hex";
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(extra, "no section names"), "extra entry: rejected");
    std::vector<CampaignProgressEntry> renamed = entries;
    renamed[0].path = "progress.dat";
    CampaignProgressTest_Check(CampaignProgressTest_EntriesReject(renamed, "first entry"),
                               "progress.bin under another name: rejected");
}

/* Items the file cannot hold are left out by the encoder, so the file stays loadable. */
static void CampaignProgressTest_EncoderDrops()
{
    CampaignProgress model;
    CampaignProgress expected;
    CampaignProgressCarry tooMany{};
    CampaignProgressCarry badFaction{};
    CampaignProgressCarry good{};
    CampaignProgress decoded;
    std::string reason;

    tooMany.sourceLevelId = 1;
    tooMany.units.resize(CAMPAIGN_PROGRESS_UNIT_CAPACITY + 1);
    badFaction.sourceLevelId = 1;
    badFaction.units.push_back(CampaignProgressUnit{1, 9, 0, 0, 0});
    good.sourceLevelId = 1;
    good.units.push_back(CampaignProgressUnit{1, 7, 0, 0, 0});
    CampaignProgress_GetOrAddSection(model, "bad-name").unlockedLevelIds.insert(3);
    CampaignProgress_GetOrAddSection(model, "Mixed_Case").carries[5] = good;
    CampaignProgressSection &section = CampaignProgress_GetOrAddSection(model, "c");
    section.carries[2] = tooMany;
    section.carries[3] = badFaction;
    section.carries[4] = good;
    section.carries[0] = good;
    section.unlockedLevelIds = {-5, 0, 6};
    section.wins[0] = CampaignProgressWins{1, 0};
    section.wins[7] = CampaignProgressWins{0, 0}; /* no selection */
    section.wins[8] = CampaignProgressWins{4, 2};

    CampaignProgress_GetOrAddSection(expected, "Mixed_Case").carries[5] = good;
    CampaignProgressSection &kept = CampaignProgress_GetOrAddSection(expected, "c");
    kept.carries[4] = good;
    kept.unlockedLevelIds = {6};
    kept.wins[8] = CampaignProgressWins{4, 2};
    std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
    std::optional<std::vector<uint8_t>> bytes = CampaignProgress_Encode(model);
    CampaignProgressTest_Check(bytes.has_value() && CampaignProgress_Decode(*bytes, &decoded, &reason) &&
                               decoded == expected, "encoder: invalid names and items left out, the rest loadable");
    CampaignProgressTest_Check(entries.size() == 3 && entries[1].path == "carry_mixed_case_5.hex" &&
                               entries[2].path == "carry_c_4.hex", "encoder: entry paths lowercased");
}

/* LoadFile / SaveFile: a round trip, a missing file, rejected files renamed to ".bad". */
static void CampaignProgressTest_Files(const char *directory)
{
    CampaignProgressTestPath target;
    CampaignProgressTestPath temporary;
    CampaignProgressTestPath bad;
    CampaignProgressTestPath noExtension;
    CampaignProgressTestPath noExtensionBad;
    CampaignProgress model = CampaignProgressTest_Model();
    CampaignProgress loaded;
    std::vector<uint8_t> versionTwo;

    CampaignProgressTest_MakePath(&target, directory, "kampagne.dat");
    CampaignProgressTest_MakePath(&temporary, directory, "kampagne.dat.tmp");
    CampaignProgressTest_MakePath(&bad, directory, "kampagne.bad");
    CampaignProgressTest_MakePath(&noExtension, directory, "progress");
    CampaignProgressTest_MakePath(&noExtensionBad, directory, "progress.bad");

    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&bad),
                               "missing file: empty progress, no .bad");

    CampaignProgressTest_Check(CampaignProgress_SaveFile(target.wide, model) == 0, "file: save");
    CampaignProgressTest_Check(!CampaignProgressTest_Exists(&temporary), "file: no .tmp left");
    {
        std::optional<std::vector<uint8_t>> saved = FileSystem_ReadWholeFileNative(target.wide);
        CampaignProgressTest_Check(saved.has_value() && saved->size() > 0x400 && memcmp(saved->data(), "pck\0", 4) == 0 &&
                                   *saved == CampaignProgress_Encode(model).value_or(std::vector<uint8_t>()),
                                   "file: the saved bytes are the PCK package");
        if (saved.has_value()) {
            versionTwo = *saved;
        }
    }
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded == model && CampaignProgressTest_Exists(&target) &&
                               !CampaignProgressTest_Exists(&bad), "file: load gives the saved model");

    /* a newer archive version: renamed to .bad, an older .bad is replaced */
    CampaignProgressTest_WriteBytes(&bad, std::vector<uint8_t>{'o', 'l', 'd'});
    CampaignProgressTest_Put(&versionTwo, 8, 2);
    CampaignProgressTest_WriteBytes(&target, versionTwo);
    loaded = CampaignProgress_LoadFile(target.wide);
    {
        std::optional<std::vector<uint8_t>> badBytes = FileSystem_ReadWholeFileNative(bad.wide);
        CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                                   badBytes.has_value() && *badBytes == versionTwo,
                                   "wrong version: empty progress, file kept as .bad");
    }

    /* a damaged byte inside a payload */
    {
        std::vector<CampaignProgressEntry> entries = CampaignProgress_EncodeEntries(model);
        entries[0].payload[CAMPAIGNPROGRESS_TEST_FIRST_WIN] ^= 1;
        CampaignProgressTest_WriteBytes(&target, CampaignProgressTest_Package(entries));
        loaded = CampaignProgress_LoadFile(target.wide);
        CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                                   CampaignProgressTest_Exists(&bad), "CRC32 mismatch: kept as .bad");
    }

    /* an empty file and the old text format are rejected as well */
    CampaignProgressTest_WriteBytes(&target, std::vector<uint8_t>());
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                               CampaignProgressTest_Exists(&bad), "empty file: kept as .bad");
    {
        const char text[] = "open-thandor campaign progress 1\n[campaign a]\nend\n";
        CampaignProgressTest_WriteBytes(&target, std::vector<uint8_t>(text, text + sizeof text - 1));
        loaded = CampaignProgress_LoadFile(target.wide);
        CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                                   CampaignProgressTest_Exists(&bad), "text file: kept as .bad");
    }

    /* over the native read limit */
    CampaignProgressTest_WriteBytes(&target, std::vector<uint8_t>(FILESYSTEM_NATIVE_READ_LIMIT_BYTES + 1, 0));
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                               CampaignProgressTest_Exists(&bad), "file over the size limit: kept as .bad");

    /* a name without an extension gets ".bad" appended */
    CampaignProgressTest_WriteBytes(&noExtension, std::vector<uint8_t>{'g', 'a', 'r', 'b', 'a', 'g', 'e'});
    loaded = CampaignProgress_LoadFile(noExtension.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&noExtension) &&
                               CampaignProgressTest_Exists(&noExtensionBad), "no extension: \".bad\" appended");

    /* empty progress is a valid file that stays */
    CampaignProgressTest_Check(CampaignProgress_SaveFile(target.wide, CampaignProgress{}) == 0, "file: save empty");
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && CampaignProgressTest_Exists(&target),
                               "empty progress: the file stays");

    DeleteFileA(target.narrow);
    DeleteFileA(temporary.narrow);
    DeleteFileA(bad.narrow);
    DeleteFileA(noExtension.narrow);
    DeleteFileA(noExtensionBad.narrow);
}

static void CampaignProgressTest_RemoveAll(const char *directory)
{
    static const char *const leaves[] = {"kampagne.dat", "kampagne.dat.tmp", "kampagne.bad", "progress",
                                         "progress.bad"};
    char path[CAMPAIGNPROGRESS_TEST_PATH_BYTES];

    for (const char *leaf : leaves) {
        snprintf(path, sizeof path, "%s\\%s", directory, leaf);
        DeleteFileA(path);
    }
    RemoveDirectoryA(directory);
}

/* The win rule of plan 4.3 and the snapshot take of 4.5 (the session hooks' pure parts). */
static void CampaignProgressTest_SessionRules()
{
    CampaignLevelRecord record{};
    record.levelId = 16; /* hansolo m15: loss continues to m17, the win to m16 */
    record.successorLevelIds[0] = 18;
    record.successorLevelIds[1] = 17;
    CampaignProgressTest_Check(CampaignProgress_IsWin(record, 16, 1, 1), "win rule: success movie, next level");
    CampaignProgressTest_Check(!CampaignProgress_IsWin(record, 16, 0, 0), "win rule: m15 loss going forward");
    record.successorLevelIds[0] = 16;
    CampaignProgressTest_Check(!CampaignProgress_IsWin(record, 16, 0, 1), "win rule: successor is the level itself");
    CampaignProgressTest_Check(!CampaignProgress_IsWin(record, 16, 8, 1), "win rule: selection outside 0..7");
    record.successorLevelIds[2] = 0;
    CampaignProgressTest_Check(CampaignProgress_IsWin(record, 16, 2, 1), "win rule: finale (successor 0)");

    std::vector<uint32_t> primary(8 * (CAMPAIGN_PROGRESS_UNIT_CAPACITY + 2), 0);
    std::vector<uint32_t> secondary(CAMPAIGN_PROGRESS_TECH_WORD_COUNT, 0);
    for (size_t index = 0; index < primary.size(); index++) {
        primary[index] = (uint32_t)index * 7u + 0x80000000u;
    }
    for (size_t index = 0; index < secondary.size(); index++) {
        secondary[index] = (uint32_t)index + 1;
    }
    CampaignProgressCarry carry = CampaignProgress_TakeCarry(8, 1, primary.data(), 3, secondary.data());
    CampaignProgressTest_Check(carry.sourceLevelId == 8 && carry.selection == 1 && carry.units.size() == 3,
                               "snapshot: header and count");
    CampaignProgressTest_Check(carry.units.size() == 3 && (uint32_t)carry.units[2].assetId == primary[16] &&
                                   (uint32_t)carry.units[2].faction == primary[17] &&
                                   (uint32_t)carry.units[2].worldXQ12 == primary[18] &&
                                   (uint32_t)carry.units[2].worldYQ12 == primary[19] &&
                                   carry.units[2].rotation == primary[20],
                               "snapshot: dwords [0..4] of a record");
    CampaignProgressTest_Check(carry.techMasks[0] == 1 && carry.techMasks[CAMPAIGN_PROGRESS_TECH_WORD_COUNT - 1] ==
                                   (uint32_t)CAMPAIGN_PROGRESS_TECH_WORD_COUNT,
                               "snapshot: secondary table");
    carry = CampaignProgress_TakeCarry(8, 1, primary.data(), CAMPAIGN_PROGRESS_UNIT_CAPACITY + 2, secondary.data());
    CampaignProgressTest_Check(carry.units.size() == (size_t)CAMPAIGN_PROGRESS_UNIT_CAPACITY,
                               "snapshot: count clamped to the table capacity");
}

void Thandor_SelfTestCampaignProgress()
{
    const char *tempRoot;
    char directory[CAMPAIGNPROGRESS_TEST_PATH_BYTES - 0x40]; /* leaves room for the leaf names */

    s_CampaignProgressChecks = 0;
    s_CampaignProgressFailures = 0;
    CampaignProgressTest_RoundTrip();
    CampaignProgressTest_Layout();
    CampaignProgressTest_Truncated();
    CampaignProgressTest_Headers();
    CampaignProgressTest_Ranges();
    CampaignProgressTest_EncoderDrops();
    CampaignProgressTest_SessionRules();

    tempRoot = getenv("TEMP");
    if (tempRoot == nullptr || tempRoot[0] == 0) {
        tempRoot = ".";
    }
    if (snprintf(directory, sizeof directory, "%s\\open-thandor-campaignprogress", tempRoot) >=
        (int)sizeof directory) {
        CampaignProgressTest_Check(false, "TEMP path too long for the file cases");
    }
    else {
        CampaignProgressTest_RemoveAll(directory); /* an aborted run */
        if (CreateDirectoryA(directory, nullptr) == 0) {
            CampaignProgressTest_Check(false, "cannot create the test directory");
        }
        else {
            CampaignProgressTest_Files(directory);
            CampaignProgressTest_RemoveAll(directory);
        }
    }
    Thandor_Log("campaignprogress: %u of %u checks failed", s_CampaignProgressFailures, s_CampaignProgressChecks);
}
