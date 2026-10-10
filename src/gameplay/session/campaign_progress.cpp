/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/campaign_progress.cpp
 * Project code (not in the original game)
 */

#include <thandor/gameplay/session/campaign_progress.h>

#include <stdarg.h>
#include <stdio.h>
#include <algorithm>
#include <span>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Campaign progress file (docs/plans/campaign_map.md 4.4), version 1. UTF-8 text, one item per line, "#" starts a
   comment, blanks and tabs separate the tokens, CR LF or LF line ends, an optional UTF-8 byte order mark:

     open-thandor campaign progress 1
     [campaign hansolo]
     fingerprint 9f3a12c4 26
     done 5 1
     done 5 2
     unlocked 2 3 4
     carry 9 from 8 sel 1 units 37
     unit 0x00000041 1 1234567 -234567 512
     ...
     tech 0 0 0 0 0 0 0 0 0
     ...
     end

   Numbers are decimal or "0x" plus 1..8 hex digits (a 32-bit pattern); the fingerprint CRC is plain hex. A
   level id is a positive int32, a selection and a faction 0..7, a unit count 0..0x200. The n unit lines and the
   tech lines (each faction at most once) of a snapshot follow its carry line directly. Load-time checks only
   look at the file itself; the checks against the campaign are made when a section is applied. */

/* "\xEF\xBB\xBF", skipped at the start of the file */
static constexpr std::string_view CAMPAIGN_PROGRESS_UTF8_BOM = "\xEF\xBB\xBF";
/* level ids per unlocked line when formatting */
static constexpr size_t CAMPAIGN_PROGRESS_UNLOCKED_PER_LINE = 16;
/* the longest token quoted in a log line */
static constexpr size_t CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS = 24;
/* ".bad", the extension of a rejected file */
static constexpr std::array<uint16_t,4> CAMPAIGN_PROGRESS_BAD_EXTENSION = {'b','a','d',0};

/* Parser state: the open section and the open carry snapshot. */
struct CampaignProgressParser {
  CampaignProgress *progress;
  CampaignProgressParseReport report;
  uint32_t lineNumber;
  bool inSection;
  bool skippingSection;          /* bad or repeated name: lines up to "end" are dropped silently */
  CampaignProgressSection section;
  bool inCarry;
  bool skippingCarry;            /* a dropped snapshot: its remaining unit and tech lines are dropped silently */
  int32_t carryTarget;
  uint32_t carryUnitCount;
  uint32_t carryFactionsSeen;    /* bit f: a tech line of faction f was read */
  CampaignProgressCarry carry;
};

/* Logs one ignored item ("campaign progress: line <n>: ..."); after CAMPAIGN_PROGRESS_LOG_LIMIT items only counts
   them (CampaignProgress_Parse logs the rest in one line). */
static void CampaignProgressParser_Ignore(CampaignProgressParser *parser,const char *format,...)

{
  char message[160];
  va_list arguments;

  parser->report.ignoredItems++;
  if (parser->report.ignoredItems > CAMPAIGN_PROGRESS_LOG_LIMIT) {
    return;
  }
  va_start(arguments,format);
  vsnprintf(message,sizeof message,format,arguments);
  va_end(arguments);
  Thandor_Log("campaign progress: line %u: %s",parser->lineNumber,message);
}

/* A token as printable text for a log line: at most CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS characters, anything
   outside printable ASCII as '?'. */
static const char *CampaignProgress_LogToken(std::string_view token,char (&buffer)[CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS + 4])

{
  size_t length = std::min(token.size(),CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS);
  size_t index;

  for (index = 0; index < length; index++) {
    unsigned char character = static_cast<unsigned char>(token[index]);
    buffer[index] = (character >= 0x20 && character < 0x7F) ? static_cast<char>(character) : '?';
  }
  if (token.size() > length) {
    buffer[length++] = '.';
    buffer[length++] = '.';
    buffer[length++] = '.';
  }
  buffer[length] = 0;
  return buffer;
}

/* Splits a line (comment removed) at blanks and tabs. */
static std::vector<std::string_view> CampaignProgress_Tokens(std::string_view line)

{
  std::vector<std::string_view> tokens;
  size_t position = 0;
  size_t start;

  line = line.substr(0,line.find('#'));
  while (position < line.size()) {
    while (position < line.size() && (line[position] == ' ' || line[position] == '\t')) {
      position++;
    }
    start = position;
    while (position < line.size() && line[position] != ' ' && line[position] != '\t') {
      position++;
    }
    if (position > start) {
      tokens.push_back(line.substr(start,position - start));
    }
  }
  return tokens;
}

static int CampaignProgress_HexDigit(char character)

{
  if (character >= '0' && character <= '9') {
    return character - '0';
  }
  if (character >= 'a' && character <= 'f') {
    return character - 'a' + 10;
  }
  if (character >= 'A' && character <= 'F') {
    return character - 'A' + 10;
  }
  return -1;
}

/* 1..8 hex digits without a prefix. */
static bool CampaignProgress_ParseHexDigits(std::string_view token,uint32_t *outValue)

{
  uint32_t value = 0;

  if (token.empty() || token.size() > 8) {
    return false;
  }
  for (char character : token) {
    int digit = CampaignProgress_HexDigit(character);
    if (digit < 0) {
      return false;
    }
    value = (value << 4) | static_cast<uint32_t>(digit);
  }
  *outValue = value;
  return true;
}

/* A number token: "0x" plus 1..8 hex digits (*outIsHex true, the 32-bit pattern in *outValue) or decimal with
   an optional '-' and at most 10 digits. */
static bool CampaignProgress_ParseNumber(std::string_view token,int64_t *outValue,bool *outIsHex)

{
  uint32_t hexValue;
  bool negative = false;
  int64_t value = 0;

  if (token.size() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X')) {
    if (!CampaignProgress_ParseHexDigits(token.substr(2),&hexValue)) {
      return false;
    }
    *outValue = hexValue;
    *outIsHex = true;
    return true;
  }
  if (!token.empty() && token[0] == '-') {
    negative = true;
    token.remove_prefix(1);
  }
  if (token.empty() || token.size() > 10) {
    return false;
  }
  for (char character : token) {
    if (character < '0' || character > '9') {
      return false;
    }
    value = value * 10 + (character - '0');
  }
  *outValue = negative ? -value : value;
  *outIsHex = false;
  return true;
}

/* Any int32: decimal in range, or a hex 32-bit pattern. */
static bool CampaignProgress_ParseInt32(std::string_view token,int32_t *outValue)

{
  int64_t value;
  bool isHex;

  if (!CampaignProgress_ParseNumber(token,&value,&isHex)) {
    return false;
  }
  if (isHex) {
    *outValue = static_cast<int32_t>(static_cast<uint32_t>(value));
    return true;
  }
  if (value < INT32_MIN || value > INT32_MAX) {
    return false;
  }
  *outValue = static_cast<int32_t>(value);
  return true;
}

/* Any uint32: decimal 0..0xFFFFFFFF or hex. */
static bool CampaignProgress_ParseUint32(std::string_view token,uint32_t *outValue)

{
  int64_t value;
  bool isHex;

  if (!CampaignProgress_ParseNumber(token,&value,&isHex) || value < 0 || value > UINT32_MAX) {
    return false;
  }
  *outValue = static_cast<uint32_t>(value);
  return true;
}

/* An int32 in [minimum, maximum]. */
static bool CampaignProgress_ParseRange(std::string_view token,int32_t minimum,int32_t maximum,int32_t *outValue)

{
  int32_t value;

  if (!CampaignProgress_ParseInt32(token,&value) || value < minimum || value > maximum) {
    return false;
  }
  *outValue = value;
  return true;
}

static bool CampaignProgress_ParseLevelId(std::string_view token,int32_t *outValue)

{
  return CampaignProgress_ParseRange(token,1,INT32_MAX,outValue);
}

static bool CampaignProgress_ParseSelection(std::string_view token,int32_t *outValue)

{
  return CampaignProgress_ParseRange(token,0,CAMPAIGN_PROGRESS_SELECTION_COUNT - 1,outValue);
}

static bool CampaignProgress_ParseFaction(std::string_view token,int32_t *outValue)

{
  return CampaignProgress_ParseRange(token,0,CAMPAIGN_PROGRESS_FACTION_COUNT - 1,outValue);
}

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

/* Closes an open snapshot before any line that is not a unit or tech line, and at "end": keeps it when all its
   unit lines were read, else drops it. Ends a silent skip of a dropped snapshot. */
static void CampaignProgressParser_FinishCarry(CampaignProgressParser *parser)

{
  if (parser->inCarry) {
    parser->inCarry = false;
    if (parser->carry.units.size() != parser->carryUnitCount) {
      CampaignProgressParser_Ignore(parser,"snapshot for level %d has %u of %u unit lines, dropped",
                                    parser->carryTarget,static_cast<unsigned>(parser->carry.units.size()),
                                    parser->carryUnitCount);
    }
    else if (parser->section.carries.contains(parser->carryTarget)) {
      CampaignProgressParser_Ignore(parser,"second snapshot for level %d, dropped",parser->carryTarget);
    }
    else {
      parser->section.carries[parser->carryTarget] = std::move(parser->carry);
    }
  }
  parser->skippingCarry = false;
}

/* Drops the open snapshot (one log line); its remaining unit and tech lines are skipped silently. */
static void CampaignProgressParser_DropCarry(CampaignProgressParser *parser,const char *reason)

{
  CampaignProgressParser_Ignore(parser,"snapshot for level %d dropped: %s",parser->carryTarget,reason);
  parser->inCarry = false;
  parser->skippingCarry = true;
}

/* A unit or tech line inside a section. */
static void CampaignProgressParser_CarryLine(CampaignProgressParser *parser,
                                             const std::vector<std::string_view> &tokens)

{
  CampaignProgressUnit unit;
  int32_t faction;
  uint32_t words[CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION];
  int32_t rotation;
  int index;

  if (!parser->inCarry) {
    if (!parser->skippingCarry) {
      CampaignProgressParser_Ignore(parser,"%s line outside a snapshot",tokens[0] == "unit" ? "unit" : "tech");
    }
    return;
  }
  if (tokens[0] == "unit") {
    if (parser->carryFactionsSeen != 0 || parser->carry.units.size() >= parser->carryUnitCount) {
      CampaignProgressParser_DropCarry(parser,"more unit lines than its count");
      return;
    }
    /* the rotation is an AngleTurn32: a negative int32 is taken as its 32-bit pattern */
    if (tokens.size() != 6 || !CampaignProgress_ParseInt32(tokens[1],&unit.assetId) ||
        !CampaignProgress_ParseFaction(tokens[2],&unit.faction) ||
        !CampaignProgress_ParseInt32(tokens[3],&unit.worldXQ12) ||
        !CampaignProgress_ParseInt32(tokens[4],&unit.worldYQ12)) {
      CampaignProgressParser_DropCarry(parser,"bad unit line");
      return;
    }
    if (!CampaignProgress_ParseUint32(tokens[5],&unit.rotation)) {
      if (!CampaignProgress_ParseInt32(tokens[5],&rotation)) {
        CampaignProgressParser_DropCarry(parser,"bad unit line");
        return;
      }
      unit.rotation = static_cast<uint32_t>(rotation);
    }
    parser->carry.units.push_back(unit);
    return;
  }
  if (parser->carry.units.size() != parser->carryUnitCount) {
    CampaignProgressParser_DropCarry(parser,"too few unit lines before its tech lines");
    return;
  }
  if (tokens.size() != 2 + CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION ||
      !CampaignProgress_ParseFaction(tokens[1],&faction)) {
    CampaignProgressParser_DropCarry(parser,"bad tech line");
    return;
  }
  if ((parser->carryFactionsSeen & (1u << faction)) != 0) {
    CampaignProgressParser_DropCarry(parser,"faction repeated in its tech lines");
    return;
  }
  for (index = 0; index < CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION; index++) {
    if (!CampaignProgress_ParseUint32(tokens[2 + index],&words[index])) {
      CampaignProgressParser_DropCarry(parser,"bad tech line");
      return;
    }
  }
  parser->carryFactionsSeen |= 1u << faction;
  std::copy(std::begin(words),std::end(words),
            parser->carry.techMasks.begin() + faction * CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION);
}

/* carry <target> from <source> sel <s> units <n> */
static void CampaignProgressParser_CarryHeader(CampaignProgressParser *parser,
                                               const std::vector<std::string_view> &tokens)

{
  int32_t target;
  int32_t source;
  int32_t selection;
  int32_t unitCount;

  if (tokens.size() != 8 || tokens[2] != "from" || tokens[4] != "sel" || tokens[6] != "units" ||
      !CampaignProgress_ParseLevelId(tokens[1],&target) || !CampaignProgress_ParseLevelId(tokens[3],&source) ||
      !CampaignProgress_ParseSelection(tokens[5],&selection)) {
    CampaignProgressParser_Ignore(parser,"bad carry line, snapshot dropped");
    parser->skippingCarry = true;
    return;
  }
  if (!CampaignProgress_ParseRange(tokens[7],0,CAMPAIGN_PROGRESS_UNIT_CAPACITY,&unitCount)) {
    CampaignProgressParser_Ignore(parser,"snapshot for level %d: unit count not 0..%d, dropped",target,
                                  CAMPAIGN_PROGRESS_UNIT_CAPACITY);
    parser->skippingCarry = true;
    return;
  }
  parser->inCarry = true;
  parser->carryTarget = target;
  parser->carryUnitCount = static_cast<uint32_t>(unitCount);
  parser->carryFactionsSeen = 0;
  parser->carry = CampaignProgressCarry{};
  parser->carry.sourceLevelId = source;
  parser->carry.selection = selection;
  parser->carry.units.reserve(static_cast<size_t>(unitCount));
}

/* "[campaign <name>]": closes an open section (dropped: it has no "end") and opens the next one. */
static void CampaignProgressParser_SectionHeader(CampaignProgressParser *parser,
                                                 const std::vector<std::string_view> &tokens)

{
  char logBuffer[CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS + 4];
  std::string_view name;

  if (parser->inSection && !parser->skippingSection) {
    CampaignProgressParser_Ignore(parser,"section \"%s\" has no end, dropped",parser->section.name.c_str());
  }
  parser->inSection = true;
  parser->skippingSection = true;
  parser->inCarry = false;
  parser->skippingCarry = false;
  parser->section = CampaignProgressSection{};
  if (tokens.size() != 2 || tokens[1].size() < 2 || tokens[1].back() != ']') {
    CampaignProgressParser_Ignore(parser,"bad section line, section skipped");
    return;
  }
  name = tokens[1].substr(0,tokens[1].size() - 1);
  if (!CampaignProgress_IsValidName(name)) {
    CampaignProgressParser_Ignore(parser,"bad campaign name \"%s\", section skipped",
                                  CampaignProgress_LogToken(name,logBuffer));
    return;
  }
  if (CampaignProgress_FindSection(*parser->progress,name) != nullptr) {
    CampaignProgressParser_Ignore(parser,"second section \"%s\" skipped",CampaignProgress_LogToken(name,logBuffer));
    return;
  }
  parser->skippingSection = false;
  parser->section.name = std::string(name);
}

/* An item inside an open (not skipped) section, other than unit and tech lines. */
static void CampaignProgressParser_SectionItem(CampaignProgressParser *parser,
                                               const std::vector<std::string_view> &tokens)

{
  char logBuffer[CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS + 4];
  std::string_view key = tokens[0];
  CampaignProgressFingerprint fingerprint;
  std::vector<int32_t> levelIds;
  int32_t levelId;
  int32_t selection;
  size_t index;

  if (key == "end") {
    if (tokens.size() != 1) {
      CampaignProgressParser_Ignore(parser,"tokens after end ignored");
    }
    CampaignProgress_GetOrAddSection(*parser->progress,parser->section.name) = std::move(parser->section);
    parser->section = CampaignProgressSection{};
    parser->inSection = false;
    return;
  }
  if (key == "fingerprint") {
    if (tokens.size() != 3 || !CampaignProgress_ParseHexDigits(tokens[1],&fingerprint.crc32) ||
        !CampaignProgress_ParseRange(tokens[2],0,INT32_MAX,&fingerprint.recordCount)) {
      CampaignProgressParser_Ignore(parser,"bad fingerprint line");
    }
    else if (parser->section.fingerprint.has_value()) {
      CampaignProgressParser_Ignore(parser,"second fingerprint line");
    }
    else {
      parser->section.fingerprint = fingerprint;
    }
    return;
  }
  if (key == "done") {
    if (tokens.size() != 3 || !CampaignProgress_ParseLevelId(tokens[1],&levelId) ||
        !CampaignProgress_ParseSelection(tokens[2],&selection)) {
      CampaignProgressParser_Ignore(parser,"bad done line");
      return;
    }
    CampaignProgressSection_RecordWin(parser->section,levelId,selection);
    return;
  }
  if (key == "unlocked") {
    for (index = 1; index < tokens.size(); index++) {
      if (!CampaignProgress_ParseLevelId(tokens[index],&levelId)) {
        break;
      }
      levelIds.push_back(levelId);
    }
    if (tokens.size() < 2 || index != tokens.size()) {
      CampaignProgressParser_Ignore(parser,"bad unlocked line");
      return;
    }
    parser->section.unlockedLevelIds.insert(levelIds.begin(),levelIds.end());
    return;
  }
  if (key == "carry") {
    CampaignProgressParser_CarryHeader(parser,tokens);
    return;
  }
  CampaignProgressParser_Ignore(parser,"unknown item \"%s\"",CampaignProgress_LogToken(key,logBuffer));
}

static void CampaignProgressParser_Line(CampaignProgressParser *parser,std::string_view line)

{
  char logBuffer[CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS + 4];
  std::vector<std::string_view> tokens = CampaignProgress_Tokens(line);

  if (tokens.empty()) {
    return;
  }
  if (tokens[0] == "[campaign") {
    CampaignProgressParser_SectionHeader(parser,tokens);
    return;
  }
  if (!parser->inSection) {
    CampaignProgressParser_Ignore(parser,"\"%s\" outside a campaign section",
                                  CampaignProgress_LogToken(tokens[0],logBuffer));
    return;
  }
  if (parser->skippingSection) {
    if (tokens[0] == "end") {
      parser->inSection = false;
    }
    return;
  }
  if (tokens[0] == "unit" || tokens[0] == "tech") {
    CampaignProgressParser_CarryLine(parser,tokens);
    return;
  }
  CampaignProgressParser_FinishCarry(parser);
  CampaignProgressParser_SectionItem(parser,tokens);
}

/* The first line: the magic text and version CAMPAIGN_PROGRESS_VERSION. */
static bool CampaignProgress_HeaderValid(std::string_view line)

{
  std::vector<std::string_view> tokens = CampaignProgress_Tokens(line);
  std::vector<std::string_view> magic = CampaignProgress_Tokens(CAMPAIGN_PROGRESS_MAGIC);
  int32_t version;

  if (tokens.size() != magic.size() + 1 || !std::equal(magic.begin(),magic.end(),tokens.begin())) {
    return false;
  }
  return CampaignProgress_ParseInt32(tokens.back(),&version) && version == CAMPAIGN_PROGRESS_VERSION;
}

CampaignProgressParseReport CampaignProgress_Parse(std::string_view text,CampaignProgress *outProgress)

{
  CampaignProgressParser parser{};
  size_t lineEnd;
  std::string_view line;
  bool first = true;

  outProgress->campaigns.clear();
  parser.progress = outProgress;
  if (text.starts_with(CAMPAIGN_PROGRESS_UTF8_BOM)) {
    text.remove_prefix(CAMPAIGN_PROGRESS_UTF8_BOM.size());
  }
  while (!text.empty()) {
    lineEnd = text.find('\n');
    line = text.substr(0,lineEnd);
    text.remove_prefix(lineEnd == std::string_view::npos ? text.size() : lineEnd + 1);
    if (!line.empty() && line.back() == '\r') {
      line.remove_suffix(1);
    }
    parser.lineNumber++;
    if (first) {
      first = false;
      if (!CampaignProgress_HeaderValid(line)) {
        return parser.report; /* headerValid false */
      }
      parser.report.headerValid = true;
      continue;
    }
    CampaignProgressParser_Line(&parser,line);
  }
  if (!parser.report.headerValid) {
    return parser.report; /* empty text */
  }
  if (parser.inSection && !parser.skippingSection) {
    CampaignProgressParser_Ignore(&parser,"section \"%s\" has no end (truncated file), dropped",
                                  parser.section.name.c_str());
  }
  if (parser.report.ignoredItems > CAMPAIGN_PROGRESS_LOG_LIMIT) {
    Thandor_Log("campaign progress: %u more ignored items not logged",
                parser.report.ignoredItems - CAMPAIGN_PROGRESS_LOG_LIMIT);
  }
  return parser.report;
}

static void CampaignProgress_Append(std::string *text,const char *format,...)

{
  char line[160];
  va_list arguments;

  va_start(arguments,format);
  vsnprintf(line,sizeof line,format,arguments);
  va_end(arguments);
  text->append(line);
}

/* A tech word: "0" or "0x" and 8 hex digits. */
static void CampaignProgress_AppendTechWord(std::string *text,uint32_t word)

{
  if (word == 0) {
    text->append(" 0");
  }
  else {
    CampaignProgress_Append(text," 0x%08X",word);
  }
}

static void CampaignProgress_FormatSection(std::string *text,const CampaignProgressSection &section)

{
  size_t inLine = 0;
  int selection;
  int faction;
  int word;

  CampaignProgress_Append(text,"[campaign %s]\n",section.name.c_str());
  if (section.fingerprint.has_value()) {
    CampaignProgress_Append(text,"fingerprint %08x %d\n",section.fingerprint->crc32,
                            section.fingerprint->recordCount);
  }
  /* the selection of the last win is written last, so the parser takes it as the last one again */
  for (const auto &[levelId,wins] : section.wins) {
    for (selection = 0; selection < CAMPAIGN_PROGRESS_SELECTION_COUNT; selection++) {
      if ((wins.selectionMask & (1u << selection)) != 0 && selection != wins.lastSelection) {
        CampaignProgress_Append(text,"done %d %d\n",levelId,selection);
      }
    }
    CampaignProgress_Append(text,"done %d %d\n",levelId,static_cast<int>(wins.lastSelection));
  }
  for (int32_t levelId : section.unlockedLevelIds) {
    text->append(inLine == 0 ? "unlocked" : "");
    CampaignProgress_Append(text," %d",levelId);
    if (++inLine == CAMPAIGN_PROGRESS_UNLOCKED_PER_LINE) {
      text->append("\n");
      inLine = 0;
    }
  }
  if (inLine != 0) {
    text->append("\n");
  }
  for (const auto &[targetLevelId,carry] : section.carries) {
    CampaignProgress_Append(text,"carry %d from %d sel %d units %u\n",targetLevelId,carry.sourceLevelId,
                            carry.selection,static_cast<unsigned>(carry.units.size()));
    for (const CampaignProgressUnit &unit : carry.units) {
      CampaignProgress_Append(text,"unit 0x%08X %d %d %d %u\n",static_cast<uint32_t>(unit.assetId),unit.faction,
                              unit.worldXQ12,unit.worldYQ12,unit.rotation);
    }
    for (faction = 0; faction < CAMPAIGN_PROGRESS_FACTION_COUNT; faction++) {
      CampaignProgress_Append(text,"tech %d",faction);
      for (word = 0; word < CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION; word++) {
        CampaignProgress_AppendTechWord(text,carry.techMasks[faction * CAMPAIGN_PROGRESS_TECH_WORDS_PER_FACTION + word]);
      }
      text->append("\n");
    }
  }
  text->append("end\n");
}

std::string CampaignProgress_Format(const CampaignProgress &progress)

{
  char logBuffer[CAMPAIGN_PROGRESS_LOG_TOKEN_CHARS + 4];
  std::string text;

  CampaignProgress_Append(&text,"%.*s %d\n",static_cast<int>(CAMPAIGN_PROGRESS_MAGIC.size()),
                          CAMPAIGN_PROGRESS_MAGIC.data(),CAMPAIGN_PROGRESS_VERSION);
  for (const CampaignProgressSection &section : progress.campaigns) {
    if (!CampaignProgress_IsValidName(section.name)) {
      Thandor_Log("campaign progress: section with the invalid name \"%s\" not written",
                  CampaignProgress_LogToken(section.name,logBuffer));
      continue;
    }
    CampaignProgress_FormatSection(&text,section);
  }
  return text;
}

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
  void *handle;

  if (!bytes.has_value()) {
    /* missing (first run), or present but unreadable or over the size limit */
    if (Win32File_Open(FileSystemOpenFlags::FILESYSTEM_OPEN_NONE,path,&handle) == 0) {
      Win32File_Close(handle);
      CampaignProgress_KeepAsBad(path,"cannot be read or is too large");
    }
    return progress;
  }
  if (!CampaignProgress_Parse(std::string_view(reinterpret_cast<const char *>(bytes->data()),bytes->size()),
                              &progress).headerValid) {
    CampaignProgress_KeepAsBad(path,"is not a version 1 progress file");
    progress.campaigns.clear();
  }
  return progress;
}

uint32_t CampaignProgress_SaveFile(uint16_t *path,const CampaignProgress &progress)

{
  std::string text = CampaignProgress_Format(progress);
  uint32_t writeError;

  writeError = FileSystem_WriteFileReplacing
                 (path,std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(text.data()),text.size()));
  if (writeError != 0) {
    Thandor_Log("campaign progress: writing \"%ls\" failed (error 0x%08X), the old file is kept",
                reinterpret_cast<wchar_t *>(path),writeError);
  }
  return writeError;
}
