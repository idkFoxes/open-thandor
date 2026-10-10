/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/campaignprogress_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=campaignprogress: the campaign progress model and file format
   (src/gameplay/session/campaign_progress.cpp) on fixed texts: a round trip of a model with every item kind
   (text and file), a truncated file, a wrong version and a wrong magic, garbage lines, a snapshot with more than
   0x200 units, bad asset ids (non-numeric and outside int32), repeated sections and unknown keys. The file cases
   run in a fresh directory under %TEMP% (the current directory without TEMP), which is removed afterwards; a
   rejected file must be renamed to ".bad". No game data is needed. One line per failed check, then
   "campaignprogress: <n> of <m> checks failed". The parser's own log lines for the broken inputs are expected. */

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

static bool CampaignProgressTest_WriteText(const CampaignProgressTestPath *path, const std::string &text)
{
    FILE *file = fopen(path->narrow, "wb");
    bool written;

    if (file == nullptr) {
        return false;
    }
    written = fwrite(text.data(), 1, text.size(), file) == text.size();
    return fclose(file) == 0 && written;
}

/* The parse of text; *outIgnored gets the number of ignored items. */
static CampaignProgress CampaignProgressTest_Parse(const std::string &text, bool *outHeaderValid,
                                                   uint32_t *outIgnored)
{
    CampaignProgress progress;
    CampaignProgressParseReport report = CampaignProgress_Parse(text, &progress);

    *outHeaderValid = report.headerValid;
    *outIgnored = report.ignoredItems;
    return progress;
}

/* A model with every item kind: two campaigns, a level won with two selections where the last one is not the
   highest, unlocked ids over more than one formatted line, a full snapshot of 0x200 units with extreme values
   and a small one with technology masks. */
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

    CampaignProgress_GetOrAddSection(progress, "luke_2"); /* empty, sorted between the two */
    return progress;
}

static void CampaignProgressTest_RoundTrip()
{
    CampaignProgress model = CampaignProgressTest_Model();
    std::string text = CampaignProgress_Format(model);
    bool headerValid;
    uint32_t ignored;
    CampaignProgress parsed = CampaignProgressTest_Parse(text, &headerValid, &ignored);
    const CampaignProgressSection *hansolo;

    CampaignProgressTest_Check(model.campaigns.size() == 3 && model.campaigns[0].name == "hansolo" &&
                               model.campaigns[1].name == "luke_2" && model.campaigns[2].name == "tutorial",
                               "round trip: sections sorted by name");
    CampaignProgressTest_Check(headerValid && ignored == 0, "round trip: parses without ignored items");
    CampaignProgressTest_Check(parsed == model, "round trip: parsed model equals the formatted one");
    CampaignProgressTest_Check(CampaignProgress_Format(parsed) == text, "round trip: formatting again is identical");
    hansolo = CampaignProgress_FindSection(parsed, "hansolo");
    CampaignProgressTest_Check(hansolo != nullptr && hansolo->wins.at(5).selectionMask == 0x6 &&
                               hansolo->wins.at(5).lastSelection == 1, "round trip: last win selection kept");
    CampaignProgressTest_Check(text.find("unlocked 2 3 4") != std::string::npos &&
                               text.find("unit 0x80000000 0 ") != std::string::npos &&
                               text.find("unit 0xFFFFFFFF 1 ") != std::string::npos &&
                               text.find("fingerprint 9f3a12c4 26\n") != std::string::npos,
                               "round trip: text items as documented");
    CampaignProgressTest_Check(CampaignProgress_FindSection(parsed, "nimm2") == nullptr,
                               "round trip: missing section not found");

    /* CR LF line ends, a byte order mark, comments and tabs */
    {
        std::string crlf = "\xEF\xBB\xBF";
        for (char character : text) {
            if (character == '\n') {
                crlf += " # comment\r";
            }
            crlf += character == ' ' ? '\t' : character;
        }
        CampaignProgress parsedCrlf = CampaignProgressTest_Parse(crlf, &headerValid, &ignored);
        CampaignProgressTest_Check(headerValid && ignored == 0 && parsedCrlf == model,
                                   "round trip: CR LF, BOM, comments and tabs");
    }
}

/* Cuts of the model's text. */
static void CampaignProgressTest_Truncated()
{
    CampaignProgress model = CampaignProgressTest_Model();
    std::string text = CampaignProgress_Format(model);
    bool headerValid;
    uint32_t ignored;
    size_t cut;

    /* in the middle of the hansolo snapshot (unit lines): hansolo dropped, nothing else read yet */
    cut = text.find("unit 0x00000141");
    CampaignProgress parsed = CampaignProgressTest_Parse(text.substr(0, cut + 7), &headerValid, &ignored);
    CampaignProgressTest_Check(headerValid && parsed.campaigns.empty() && ignored >= 1,
                               "truncated in a snapshot: the open section is dropped");

    /* after "[campaign tutorial]\ndone 1 1\n": hansolo and luke_2 kept, tutorial dropped */
    cut = text.find("[campaign tutorial]");
    parsed = CampaignProgressTest_Parse(text.substr(0, cut + 29), &headerValid, &ignored);
    CampaignProgressTest_Check(headerValid && parsed.campaigns.size() == 2 &&
                               CampaignProgress_FindSection(parsed, "tutorial") == nullptr &&
                               CampaignProgress_FindSection(parsed, "hansolo") != nullptr &&
                               *CampaignProgress_FindSection(parsed, "hansolo") == model.campaigns[0] &&
                               ignored == 1, "truncated in the last section: earlier sections kept");

    /* just before the final "end" */
    parsed = CampaignProgressTest_Parse(text.substr(0, text.size() - 4), &headerValid, &ignored);
    CampaignProgressTest_Check(headerValid && parsed.campaigns.size() == 2 && ignored == 1,
                               "truncated before end: the last section is dropped");

    /* only the header line, without its line end */
    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1", &headerValid, &ignored);
    CampaignProgressTest_Check(headerValid && parsed.campaigns.empty() && ignored == 0, "header only: empty");

    /* a section header in the open section: the open one is dropped, the next one read */
    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1\n[campaign a]\ndone 1 1\n"
                                        "[campaign b]\ndone 2 1\nend\n", &headerValid, &ignored);
    CampaignProgressTest_Check(parsed.campaigns.size() == 1 && parsed.campaigns[0].name == "b" && ignored == 1,
                               "section without end before the next one: dropped");
}

static void CampaignProgressTest_Versions()
{
    static const char *const rejected[] = {
        "open-thandor campaign progress 2\n[campaign a]\nend\n",
        "open-thandor campaign progress 0\n",
        "open-thandor campaign progress x\n",
        "open-thandor campaign progress\n",
        "open-thandor campaign progress 1 extra\n",
        "Open-Thandor campaign progress 1\n",
        "[campaign a]\nend\n",
        "\nopen-thandor campaign progress 1\n",
        "",
        "\x01\x02\xFF\xFE garbage\n",
    };
    char label[64];
    bool headerValid;
    uint32_t ignored;
    size_t index;

    for (index = 0; index < sizeof rejected / sizeof rejected[0]; index++) {
        CampaignProgress parsed = CampaignProgressTest_Parse(rejected[index], &headerValid, &ignored);
        snprintf(label, sizeof label, "wrong header %u rejected", static_cast<unsigned>(index));
        CampaignProgressTest_Check(!headerValid && parsed.campaigns.empty(), label);
    }
    CampaignProgressTest_Parse("open-thandor  campaign\tprogress 0x1\r\n", &headerValid, &ignored);
    CampaignProgressTest_Check(headerValid, "header with other blanks and a hex version accepted");
}

static void CampaignProgressTest_GarbageLines()
{
    static const char *const lines[] = {
        "open-thandor campaign progress 1",
        "stray line outside",                  /* 1: outside a section */
        "\x01\x02\x03\xFF\xFE",                /* 2: outside a section */
        "[campaign hansolo]",
        "done 2 1",
        "done x 1",                            /* 3: non-numeric */
        "done 3 8",                            /* 4: selection out of range */
        "done 0 1",                            /* 5: level id not positive */
        "done 4 1 9",                          /* 6: extra token */
        "done 5",                              /* 7: missing token */
        "unlocked 6 7 -1",                     /* 8: one bad id drops the line */
        "unlocked",                            /* 9: no ids */
        "unlocked 8 0x9",
        "fingerprint zz 26",                   /* 10 */
        "fingerprint 12345678 26",
        "fingerprint 87654321 26",             /* 11: second */
        "",                                    /* replaced by a line with a NUL byte: 12 */
        "unit 1 1 1 1 1",                      /* 13: outside a snapshot */
        "tech 1 0 0 0 0 0 0 0 0",              /* 14: outside a snapshot */
        "carry 9 from 8 sel 1 units",          /* 15: missing count */
        "unit 1 1 1 1 1",                      /* skipped with it */
        "carry 9 from 8 sel 9 units 0",        /* 16: selection out of range */
        "carry 10 from 9 sel 1 units 1",
        "unit 1 9 0 0 0",                      /* 17: faction 9 */
        "carry 11 from 10 sel 1 units 1",
        "unit 1 1 0 0 0",
        "tech 1 0 0 0 0 0 0 0 0",
        "tech 1 0 0 0 0 0 0 0 0",              /* 18: faction repeated */
        "carry 12 from 11 sel 1 units 2",
        "unit 1 1 0 0 0",
        "done 13 1",                           /* 19: ends the snapshot with 1 of 2 units */
        "carry 14 from 13 sel 1 units 1",
        "unit 7 1 0 0 0",
        "tech 2 1 2 3 4 5 6 7 0xFFFFFFFF",
        "tech 3 0 0 0 0 0 0 0 4294967296",     /* 20: word over 32 bits */
        "carry 15 from 14 sel 1 units 1",
        "unit 7 1 0 0 0",
        "tech 2 1 2 3 4 5 6 7 4294967295",
        "carry 15 from 14 sel 2 units 0",      /* 21 (at end): second snapshot for 15 */
        "end extra",                           /* 22: tokens after end */
        "end",                                 /* 23: outside a section */
    };
    std::string text;
    bool headerValid;
    uint32_t ignored;
    const CampaignProgressSection *section;
    std::array<uint32_t, CAMPAIGN_PROGRESS_TECH_WORD_COUNT> expectedTech{};
    int word;

    for (const char *line : lines) {
        if (line[0] == 0) {
            text += std::string("\x80\x81\x82 \0 \xC3\xA4", 8);
        }
        text += line;
        text += '\n';
    }
    CampaignProgress parsed = CampaignProgressTest_Parse(text, &headerValid, &ignored);
    section = CampaignProgress_FindSection(parsed, "hansolo");
    CampaignProgressTest_Check(headerValid && parsed.campaigns.size() == 1 && section != nullptr,
                               "garbage lines: the section is kept");
    if (section == nullptr) {
        return;
    }
    CampaignProgressTest_Check(section->wins.size() == 2 && section->wins.contains(2) && section->wins.contains(13),
                               "garbage lines: only the good done lines");
    CampaignProgressTest_Check(section->unlockedLevelIds == std::set<int32_t>{8, 9},
                               "garbage lines: only the good unlocked line");
    CampaignProgressTest_Check(section->fingerprint.has_value() && section->fingerprint->crc32 == 0x12345678u,
                               "garbage lines: the first good fingerprint");
    for (word = 0; word < CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION; word++) {
        expectedTech[2 * CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION + word] = word == 7 ? 0xFFFFFFFFu :
                                                                           static_cast<uint32_t>(word + 1);
    }
    CampaignProgressTest_Check(section->carries.size() == 1 && section->carries.contains(15) &&
                               section->carries.at(15).selection == 1 &&
                               section->carries.at(15).units.size() == 1 &&
                               section->carries.at(15).techMasks == expectedTech,
                               "garbage lines: only the good snapshot, the first one for its target");
    CampaignProgressTest_Check(ignored == 23, "garbage lines: every bad item counted once");
}

/* A snapshot header with n units, n unit lines and a done line after it. */
static std::string CampaignProgressTest_CarryText(int units, const char *assetToken)
{
    std::string text = "open-thandor campaign progress 1\n[campaign c]\n";
    char line[96];
    int index;

    snprintf(line, sizeof line, "carry 2 from 1 sel 1 units %d\n", units);
    text += line;
    for (index = 0; index < units; index++) {
        snprintf(line, sizeof line, "unit %s 1 0 0 0\n", index == units - 1 ? assetToken : "0x41");
        text += line;
    }
    text += "done 1 1\nend\n";
    return text;
}

static void CampaignProgressTest_UnitCounts()
{
    bool headerValid;
    uint32_t ignored;
    CampaignProgress parsed;
    const CampaignProgressSection *section;

    parsed = CampaignProgressTest_Parse(CampaignProgressTest_CarryText(CAMPAIGN_PROGRESS_UNIT_CAPACITY, "5"),
                                        &headerValid, &ignored);
    section = CampaignProgress_FindSection(parsed, "c");
    CampaignProgressTest_Check(section != nullptr && ignored == 0 && section->carries.contains(2) &&
                               section->carries.at(2).units.size() == CAMPAIGN_PROGRESS_UNIT_CAPACITY,
                               "0x200 units accepted");

    parsed = CampaignProgressTest_Parse(CampaignProgressTest_CarryText(CAMPAIGN_PROGRESS_UNIT_CAPACITY + 1, "5"),
                                        &headerValid, &ignored);
    section = CampaignProgress_FindSection(parsed, "c");
    CampaignProgressTest_Check(section != nullptr && section->carries.empty() && section->wins.contains(1) &&
                               ignored == 1, "0x201 units: snapshot dropped with one item, the rest kept");

    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1\n[campaign c]\n"
                                        "carry 2 from 1 sel 1 units 1\nunit 1 1 0 0 0\nunit 1 1 0 0 0\nend\n",
                                        &headerValid, &ignored);
    section = CampaignProgress_FindSection(parsed, "c");
    CampaignProgressTest_Check(section != nullptr && section->carries.empty() && ignored == 1,
                               "more unit lines than the count: snapshot dropped");

    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1\n[campaign c]\n"
                                        "carry 2 from 1 sel 1 units 0\nend\n", &headerValid, &ignored);
    section = CampaignProgress_FindSection(parsed, "c");
    CampaignProgressTest_Check(section != nullptr && section->carries.contains(2) && ignored == 0 &&
                               section->carries.at(2).techMasks == std::array<uint32_t, CAMPAIGN_PROGRESS_TECH_WORD_COUNT>{},
                               "0 units and no tech lines: an empty snapshot");
}

static void CampaignProgressTest_AssetIds()
{
    static const char *const accepted[] = {"0", "-1", "2147483647", "-2147483648", "0x0", "0xFFFFFFFF",
                                           "0x80000000", "0xffff", "65535"};
    static const char *const rejected[] = {"abc", "2147483648", "-2147483649", "0x100000000", "0x", "1x2",
                                           "4294967295", "--1", "0x-1", "99999999999", "+5", "1.0"};
    static const int32_t acceptedValues[] = {0, -1, INT32_MAX, INT32_MIN, 0, -1, INT32_MIN, 0xFFFF, 65535};
    char label[96];
    bool headerValid;
    uint32_t ignored;
    size_t index;

    for (index = 0; index < sizeof accepted / sizeof accepted[0]; index++) {
        CampaignProgress parsed = CampaignProgressTest_Parse(CampaignProgressTest_CarryText(1, accepted[index]),
                                                             &headerValid, &ignored);
        const CampaignProgressSection *section = CampaignProgress_FindSection(parsed, "c");
        snprintf(label, sizeof label, "asset id %s accepted", accepted[index]);
        CampaignProgressTest_Check(section != nullptr && ignored == 0 && section->carries.contains(2) &&
                                   section->carries.at(2).units[0].assetId == acceptedValues[index], label);
    }
    for (index = 0; index < sizeof rejected / sizeof rejected[0]; index++) {
        CampaignProgress parsed = CampaignProgressTest_Parse(CampaignProgressTest_CarryText(3, rejected[index]),
                                                             &headerValid, &ignored);
        const CampaignProgressSection *section = CampaignProgress_FindSection(parsed, "c");
        snprintf(label, sizeof label, "asset id %s drops the snapshot", rejected[index]);
        CampaignProgressTest_Check(section != nullptr && section->carries.empty() && section->wins.contains(1) &&
                                   ignored == 1, label);
    }
}

static void CampaignProgressTest_Sections()
{
    bool headerValid;
    uint32_t ignored;
    CampaignProgress parsed;

    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1\n"
                                        "[campaign hansolo]\ndone 2 1\nend\n"
                                        "[campaign hansolo]\ndone 3 1\ncarry 4 from 3 sel 1 units 0\nend\n"
                                        "[campaign tutorial]\ndone 1 1\nend\n",
                                        &headerValid, &ignored);
    CampaignProgressTest_Check(parsed.campaigns.size() == 2 && ignored == 1 &&
                               CampaignProgress_FindSection(parsed, "hansolo")->wins.size() == 1 &&
                               CampaignProgress_FindSection(parsed, "hansolo")->wins.contains(2) &&
                               CampaignProgress_FindSection(parsed, "hansolo")->carries.empty() &&
                               CampaignProgress_FindSection(parsed, "tutorial") != nullptr,
                               "duplicate section: the first is kept, the second skipped up to its end");

    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1\n"
                                        "[campaign bad-name]\ndone 2 1\nend\n"
                                        "[campaign ]\nend\n"
                                        "[campaign]\nend\n"                                    /* two items outside a section */
                                        "[campaign abcdefghijklmnopqrstuvwxyz012345]\nend\n" /* 32 characters */
                                        "[campaign abcdefghijklmnopqrstuvwxyz01234]\nend\n"  /* 31 */
                                        "[campaign x] extra\nend\n"
                                        "[campaign y\nend\n"
                                        "[other z]\n",
                                        &headerValid, &ignored);
    CampaignProgressTest_Check(parsed.campaigns.size() == 1 &&
                               parsed.campaigns[0].name == "abcdefghijklmnopqrstuvwxyz01234" && ignored == 8,
                               "bad campaign names skipped");
}

static void CampaignProgressTest_UnknownKeys()
{
    bool headerValid;
    uint32_t ignored;
    CampaignProgress parsed;
    const CampaignProgressSection *section;

    parsed = CampaignProgressTest_Parse("open-thandor campaign progress 1\n"
                                        "# a comment\n"
                                        "future_global 1 2 3\n"
                                        "[campaign hansolo]\n"
                                        "future_key 7\n"
                                        "done 2 1\n"
                                        "carry 3 from 2 sel 1 units 1\n"
                                        "unit 1 1 0 0 0\n"
                                        "future_after_units 1\n" /* closes the snapshot first: it is kept */
                                        "Done 4 1\n"             /* keys are case-sensitive */
                                        "end\n",
                                        &headerValid, &ignored);
    section = CampaignProgress_FindSection(parsed, "hansolo");
    CampaignProgressTest_Check(headerValid && section != nullptr && ignored == 4 && section->wins.size() == 1 &&
                               section->carries.contains(3), "unknown keys ignored, known items kept");
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

    CampaignProgressTest_MakePath(&target, directory, "campaign_progress.txt");
    CampaignProgressTest_MakePath(&temporary, directory, "campaign_progress.txt.tmp");
    CampaignProgressTest_MakePath(&bad, directory, "campaign_progress.bad");
    CampaignProgressTest_MakePath(&noExtension, directory, "progress");
    CampaignProgressTest_MakePath(&noExtensionBad, directory, "progress.bad");

    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&bad),
                               "missing file: empty progress, no .bad");

    CampaignProgressTest_Check(CampaignProgress_SaveFile(target.wide, model) == 0, "file: save");
    CampaignProgressTest_Check(!CampaignProgressTest_Exists(&temporary), "file: no .tmp left");
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded == model && CampaignProgressTest_Exists(&target) &&
                               !CampaignProgressTest_Exists(&bad), "file: load gives the saved model");

    /* a newer version: renamed to .bad, an older .bad is replaced */
    CampaignProgressTest_WriteText(&bad, "older bad file");
    CampaignProgressTest_WriteText(&target, "open-thandor campaign progress 2\n[campaign a]\nend\n");
    loaded = CampaignProgress_LoadFile(target.wide);
    {
        std::optional<std::vector<uint8_t>> badBytes = FileSystem_ReadWholeFileNative(bad.wide);
        CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                                   badBytes.has_value() && badBytes->size() > 31 && (*badBytes)[31] == '2',
                                   "wrong version: empty progress, file kept as .bad");
    }

    /* an empty file is rejected as well */
    CampaignProgressTest_WriteText(&target, "");
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                               CampaignProgressTest_Exists(&bad), "empty file: kept as .bad");

    /* over the native read limit */
    CampaignProgressTest_WriteText(&target, std::string(FILESYSTEM_NATIVE_READ_LIMIT_BYTES + 1, '#'));
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&target) &&
                               CampaignProgressTest_Exists(&bad), "file over the size limit: kept as .bad");

    /* a name without an extension gets ".bad" appended */
    CampaignProgressTest_WriteText(&noExtension, "garbage");
    loaded = CampaignProgress_LoadFile(noExtension.wide);
    CampaignProgressTest_Check(loaded.campaigns.empty() && !CampaignProgressTest_Exists(&noExtension) &&
                               CampaignProgressTest_Exists(&noExtensionBad), "no extension: \".bad\" appended");

    /* a good file with ignored items is not renamed */
    CampaignProgressTest_WriteText(&target, "open-thandor campaign progress 1\nnonsense\n[campaign a]\nend\n");
    loaded = CampaignProgress_LoadFile(target.wide);
    CampaignProgressTest_Check(loaded.campaigns.size() == 1 && CampaignProgressTest_Exists(&target),
                               "ignored items: the file stays");

    DeleteFileA(target.narrow);
    DeleteFileA(temporary.narrow);
    DeleteFileA(bad.narrow);
    DeleteFileA(noExtension.narrow);
    DeleteFileA(noExtensionBad.narrow);
}

static void CampaignProgressTest_RemoveAll(const char *directory)
{
    static const char *const leaves[] = {"campaign_progress.txt", "campaign_progress.txt.tmp",
                                         "campaign_progress.bad", "progress", "progress.bad"};
    char path[CAMPAIGNPROGRESS_TEST_PATH_BYTES];

    for (const char *leaf : leaves) {
        snprintf(path, sizeof path, "%s\\%s", directory, leaf);
        DeleteFileA(path);
    }
    RemoveDirectoryA(directory);
}

void Thandor_SelfTestCampaignProgress()
{
    const char *tempRoot;
    char directory[CAMPAIGNPROGRESS_TEST_PATH_BYTES - 0x40]; /* leaves room for the leaf names */

    s_CampaignProgressChecks = 0;
    s_CampaignProgressFailures = 0;
    CampaignProgressTest_RoundTrip();
    CampaignProgressTest_Truncated();
    CampaignProgressTest_Versions();
    CampaignProgressTest_GarbageLines();
    CampaignProgressTest_UnitCounts();
    CampaignProgressTest_AssetIds();
    CampaignProgressTest_Sections();
    CampaignProgressTest_UnknownKeys();

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
