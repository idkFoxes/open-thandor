/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/custom_maps.cpp
 */

/* Not in the original: custom maps, the map editor projects maps\<name>.thmap (see custom_maps.h,
   docs/custom_maps.md). Native memory and std::filesystem only: the arena (g_MemoryApi) must not see the scan, or the
   arena order of a later session would shift; without a maps folder the scan returns before it allocates anything.
   The active project's files reach the game through Package_LoadEntryWithSize / Package_LoadEntryIntoBuffer
   (CustomMaps_FindServedAsset), which copy them into the arena exactly as they copy a loose file. */

#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <vector>
#include <thandor/assets/scenario/custom_maps.h>
#include <thandor/assets/scenario/custom_map_validate.h>
#include <thandor/assets/scenario/custom_project.h>
#include <thandor/thandor.h>
#include <thandor/core/settings/original_mode.h>
#include <thandor/assets/package/runtime.h>
#include <thandor/core/text/path.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/filesystem/win32.h>
#include <thandor/ui/frontend/scenario_selection.h>

static_assert(sizeof(ScenarioCatalogDisplayRecord) == SCENARIO_CATALOG_RECORD_SIZE);

/* The .lev offsets the scan reads (docs/level_format.md). */
static constexpr size_t CUSTOM_MAP_LEV_MAGIC = 0x000;
static constexpr size_t CUSTOM_MAP_LEV_SIZE = 0x004;
static constexpr size_t CUSTOM_MAP_LEV_CONVERTER = 0x00C;
static constexpr size_t CUSTOM_MAP_LEV_FIELD_PATH = 0x0B0;
static constexpr size_t CUSTOM_MAP_LEV_RECORD = 0x100;
static constexpr size_t CUSTOM_MAP_LEV_MIN_BYTES = 0x800; /* the fixed prefix every level has */
/* Code units WidePath_SetExtensionCode needs from the field grid path's terminator on (".fld" + terminator), as
   FrontendLevelAsset_LoadedImageFits demands. */
static constexpr size_t CUSTOM_MAP_PATH_EXTENSION_UNITS = 5;
/* Size limit of the level entry (a stock .lev has 6 to 22 KiB). */
static constexpr size_t CUSTOM_MAP_LEV_LIMIT_BYTES = 0x100000;
/* Code units of a title (without the leading style unit), of a description line and of the author. */
static constexpr size_t CUSTOM_MAP_TITLE_UNITS = 64;
static constexpr size_t CUSTOM_MAP_DESCRIPTION_UNITS = 1024;
static constexpr size_t CUSTOM_MAP_AUTHOR_UNITS = 64;
static constexpr size_t CUSTOM_MAP_LIST_TITLE_UNITS = 1 + CUSTOM_MAP_TITLE_UNITS + 24;
/* Lines of a level's description block (TEXT_ID_LEVEL_DESCRIPTION_STRIDE). */
static constexpr size_t CUSTOM_MAP_DESCRIPTION_LINES = TEXT_ID_LEVEL_DESCRIPTION_STRIDE;

/* The title and description ids of the custom title indices (custom_maps.h). */
static constexpr uint32_t CUSTOM_MAP_TITLE_ID_FIRST = TEXT_ID_LEVEL_TITLE_BASE + CUSTOM_MAP_TITLE_INDEX_BASE;
static constexpr uint32_t CUSTOM_MAP_DESCRIPTION_ID_FIRST =
    TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * CUSTOM_MAP_TITLE_INDEX_BASE;
static_assert(CUSTOM_MAP_TITLE_ID_FIRST == 0x2500);
static_assert(CUSTOM_MAP_TITLE_ID_FIRST + CUSTOM_MAP_TITLE_INDEX_SLOTS <= 0x2D00,
              "the custom titles stay on the unused compact text pages 0x25..0x2C");

/* The languages of the language choice: their telephone country codes, which are also the STR locale block keys.
   Index 0 of the text arrays is the fallback (the STR block the editor prefers, or the display name). */
static constexpr int CUSTOM_MAP_LANGUAGE_COUNT = 5;
static constexpr uint32_t g_CustomMapLanguageCountries[CUSTOM_MAP_LANGUAGE_COUNT] = {49, 44, 33, 39, 34};
static constexpr int CUSTOM_MAP_LANGUAGE_GERMAN = 1;

/* The display texts of a map, per language (0 = fallback). Empty vectors fall back to index 0. */
struct CustomMapTexts {
  /* the title with a leading style unit (the callers overwrite the first unit) */
  std::vector<uint16_t> titles[1 + CUSTOM_MAP_LANGUAGE_COUNT];
  /* the lines of the description block (line 0 = description, with the author appended) */
  std::vector<uint16_t> lines[1 + CUSTOM_MAP_LANGUAGE_COUNT][CUSTOM_MAP_DESCRIPTION_LINES];
};

struct CustomMapEntry {
  std::string name;   /* the file stem (ASCII, as found) */
  std::string lowerName;
  uint32_t titleIndex = 0;
  ScenarioCatalogDisplayRecord record{}; /* the list row: the .lev's catalog record with our identifier and index */
  CustomMapTexts texts;
  std::array<uint16_t,CUSTOM_MAP_LIST_TITLE_UNITS> listTitle{}; /* the list row's title text, see RefreshLanguage */
};

/* The result of checking one project file, kept while its size and write time stay the same. */
struct CustomMapFileCheck {
  uintmax_t bytes = 0;
  std::filesystem::file_time_type time{};
  bool valid = false;
  std::string reason;
  std::array<uint8_t,SCENARIO_CATALOG_RECORD_SIZE> record{};
  CustomMapTexts texts;
};

/* The project of the custom map that is loaded or running: the files the game may load from it (normalized path:
   lowercase ASCII with '/'), the level under maps/<name>.lev. */
struct CustomMapActiveProject {
  std::string lowerName;
  uintmax_t bytes = 0;
  std::filesystem::file_time_type time{};
  std::map<std::string,std::vector<uint8_t>> served;
};

/* Module data. The previous scan's entries stay alive one more scan: list rows may still point at them. */
static std::vector<std::unique_ptr<CustomMapEntry>> s_CustomMaps;
static std::vector<std::unique_ptr<CustomMapEntry>> s_RetiredCustomMaps;
static std::array<CustomMapEntry *,CUSTOM_MAP_TITLE_INDEX_SLOTS> s_CustomMapBySlot{};
static std::map<std::string,CustomMapFileCheck> s_CustomMapFileChecks;
static std::vector<Ptr32<ScenarioCatalogDisplayRecord>> s_CustomMapListRows;
static std::unique_ptr<CustomMapActiveProject> s_CustomMapActive;
/* The catalog of the last CustomMaps_Rebuild and whether it skipped the scan for "Original Thandor": the next list
   build after the switch went off scans then (the frontend rebuilds the catalog only now and then). */
static const ScenarioCatalogHeader *s_CustomMapsCatalog = nullptr;
static bool s_CustomMapsScanSkipped = false;

/* the texts of a title index without a listed map (a save of a removed map) */
static uint16_t g_CustomMapFallbackTitleGerman[] = {RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1,
    'E','i','g','e','n','e',' ','K','a','r','t','e',0};
static uint16_t g_CustomMapFallbackTitleEnglish[] = {RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1,
    'C','u','s','t','o','m',' ','m','a','p',0};
static uint16_t g_CustomMapEmptyText[] = {0};

/* The hint row (CustomMaps_ListRows): its record and texts. s_CustomMapsRejected counts the .thmap files the last
   scan skipped (bad name, an original level's name, a failed check). */
static ScenarioCatalogDisplayRecord s_CustomMapHintRecord{};
static std::array<uint16_t,64> s_CustomMapHintTitle{};
static std::array<uint16_t,320> s_CustomMapHintDescription{};
static std::array<uint16_t,32> s_CustomMapHintLink{};
static uint32_t s_CustomMapsRejected = 0;

/* ---------------------------------------------------------------- small helpers */

static char CustomMaps_LowerAscii(char c)
{
  return ((c >= 'A') && (c <= 'Z')) ? (char)(c - 'A' + 'a') : c;
}

static uint16_t CustomMaps_LowerUnit(uint16_t c)
{
  return ((c >= 'A') && (c <= 'Z')) ? (uint16_t)(c - 'A' + 'a') : c;
}

static std::string CustomMaps_Lower(const std::string &text)
{
  std::string lower = text;
  for (char &c : lower) {
    c = CustomMaps_LowerAscii(c);
  }
  return lower;
}

/* 1..CUSTOM_MAP_NAME_MAX_UNITS characters A-Z a-z 0-9 _ - (the convention of tools/data/custom_map.py) */
static bool CustomMaps_IsValidName(const std::wstring &name)
{
  if (name.empty() || (name.size() > (size_t)CUSTOM_MAP_NAME_MAX_UNITS)) {
    return false;
  }
  for (wchar_t c : name) {
    if (!(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_') ||
          (c == '-'))) {
      return false;
    }
  }
  return true;
}

static uint32_t CustomMaps_LoadU32(const std::vector<uint8_t> &bytes,size_t offset)
{
  uint32_t value;

  std::memcpy(&value,bytes.data() + offset,sizeof value);
  return value;
}

/* the executable directory as a std::filesystem path (g_ExecutableDirectoryUtf16 is UTF-16, as wchar_t here) */
static std::filesystem::path CustomMaps_ExecutableDirectory()
{
  return std::filesystem::path(reinterpret_cast<const wchar_t *>(g_ExecutableDirectoryUtf16));
}

static std::filesystem::path CustomMaps_ProjectPath(const std::string &name)
{
  return CustomMaps_ExecutableDirectory() / L"maps" / std::filesystem::path(name + ".thmap");
}

/* ---------------------------------------------------------------- texts */

/* Appends the code points of UTF-8 text as display code units: Latin-1 stays, every other character becomes '?',
   control characters are dropped. False for malformed UTF-8. */
static bool CustomMaps_AppendUtf8(const std::string &text,size_t limit,std::vector<uint16_t> *out)
{
  size_t index = 0;
  size_t appended = 0;

  while (index < text.size()) {
    const uint8_t lead = (uint8_t)text[index];
    uint32_t codePoint;
    size_t length;
    if (lead < 0x80) {
      codePoint = lead;
      length = 1;
    }
    else if ((lead & 0xE0) == 0xC0) {
      codePoint = lead & 0x1F;
      length = 2;
    }
    else if ((lead & 0xF0) == 0xE0) {
      codePoint = lead & 0x0F;
      length = 3;
    }
    else if ((lead & 0xF8) == 0xF0) {
      codePoint = lead & 0x07;
      length = 4;
    }
    else {
      return false;
    }
    if (index + length > text.size()) {
      return false;
    }
    for (size_t follow = 1; follow < length; follow++) {
      const uint8_t unit = (uint8_t)text[index + follow];
      if ((unit & 0xC0) != 0x80) {
        return false;
      }
      codePoint = codePoint << 6 | (unit & 0x3F);
    }
    if ((codePoint < 0x20) || ((codePoint >= 0x7F) && (codePoint < 0xA0))) {
      codePoint = 0; /* dropped */
    }
    else if (codePoint > 0xFF) {
      codePoint = '?'; /* the game fonts have Latin-1 only */
    }
    if ((codePoint != 0) && (appended < limit)) {
      out->push_back((uint16_t)codePoint);
      appended++;
    }
    index += length;
  }
  return true;
}

/* Appends the units of an STR string (raw UTF-16, up to its terminator) as display code units: Latin-1 characters
   stay, others become '?', control characters are dropped. Of the rich-text commands only the colour, save/restore,
   fixed space, soft hyphen and line break commands stay (with formatting; without, a line break becomes a space and
   the rest is dropped); every command with a payload (a literal colour, an inline value, a nested stream or image
   pointer) is dropped with its payload, so no pointer of the file reaches the text renderer. */
static void CustomMaps_AppendStrUnits(const std::vector<uint16_t> &units,bool formatting,size_t limit,
                                      std::vector<uint16_t> *out)
{
  size_t appended = 0;

  for (size_t index = 0; (index < units.size()) && (units[index] != 0) && (appended < limit); index++) {
    uint16_t unit = units[index];
    if ((unit & RICHTEXT_COMMAND_FLAG) != 0) {
      const int opcode = unit & RICHTEXT_OPCODE_MASK;
      if (opcode == RICHTEXT_OP_LITERAL_COLOR) {
        index += RICHTEXT_RECORD_UNITS_LITERAL_COLOR - 1;
        continue;
      }
      if ((opcode >= RICHTEXT_OP_INLINE_VALUE_0) && (opcode <= RICHTEXT_OP_INLINE_VALUE_2)) {
        index += RICHTEXT_RECORD_UNITS_INLINE_VALUE - 1;
        continue;
      }
      if ((opcode == RICHTEXT_OP_CALL_NESTED) || (opcode == RICHTEXT_OP_JUMP_NESTED)) {
        index += RICHTEXT_RECORD_UNITS_NESTED - 1;
        continue;
      }
      if (opcode == RICHTEXT_OP_INLINE_IMAGE) {
        index += RICHTEXT_RECORD_UNITS_INLINE_IMAGE - 1;
        continue;
      }
      const bool kept = (opcode <= RICHTEXT_OP_RESTORE_COLOR) || (opcode == RICHTEXT_OP_FIXED_SPACE) ||
                        (opcode == RICHTEXT_OP_SOFT_HYPHEN) || (opcode == RICHTEXT_OP_LINE_BREAK);
      if (!kept) {
        continue;
      }
      if (!formatting) {
        if (opcode != RICHTEXT_OP_LINE_BREAK) {
          continue;
        }
        unit = ' ';
      }
      else {
        unit = (uint16_t)(RICHTEXT_COMMAND_FLAG | opcode);
      }
    }
    else if (unit == '\n') {
      unit = formatting ? (uint16_t)(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK) : (uint16_t)' ';
    }
    else if ((unit < 0x20) || ((unit >= 0x7F) && (unit < 0xA0))) {
      continue;
    }
    else if (unit > 0xFF) {
      unit = '?';
    }
    out->push_back(unit);
    appended++;
  }
}

/* The STR strings of the project in the block for country (0 = the fallback block), or nullptr. */
static const std::vector<std::vector<uint16_t>> *CustomMaps_ProjectStrings(const CustomProject &project,
                                                                           uint32_t country)
{
  auto block = project.texts.find(country);
  if (block == project.texts.end()) {
    block = project.texts.find(0);
  }
  return (block == project.texts.end()) ? nullptr : &block->second;
}

/* Builds the display texts of a project: the title from display_name, else STR string 0 (without its leading
   space), else the file stem; the description block line k from STR string k + 1; the author as a line of the
   description. */
static void CustomMaps_BuildTexts(const std::string &name,const CustomProject &project,CustomMapTexts *texts)
{
  std::vector<uint16_t> author;
  if (!project.author.empty() && !CustomMaps_AppendUtf8(project.author,CUSTOM_MAP_AUTHOR_UNITS,&author)) {
    author.clear();
  }
  std::vector<uint16_t> displayName;
  if (!project.displayName.empty() && !CustomMaps_AppendUtf8(project.displayName,CUSTOM_MAP_TITLE_UNITS,&displayName)) {
    displayName.clear();
  }
  for (int language = 0; language <= CUSTOM_MAP_LANGUAGE_COUNT; language++) {
    const uint32_t country = (language == 0) ? 0 : g_CustomMapLanguageCountries[language - 1];
    const std::vector<std::vector<uint16_t>> *strings = CustomMaps_ProjectStrings(project,country);
    std::vector<uint16_t> &title = texts->titles[language];
    if (language == 0) {
      title.push_back(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1); /* overwritten by the callers */
      if (!displayName.empty()) {
        title.insert(title.end(),displayName.begin(),displayName.end());
      }
      else if (strings != nullptr) {
        std::vector<uint16_t> text;
        CustomMaps_AppendStrUnits((*strings)[0],false,CUSTOM_MAP_TITLE_UNITS + 8,&text);
        size_t first = 0;
        while ((first < text.size()) && (text[first] == ' ')) {
          first++;
        }
        title.insert(title.end(),text.begin() + first,text.begin() + std::min(text.size(),first + CUSTOM_MAP_TITLE_UNITS));
      }
      if (title.size() == 1) {
        for (char c : name) {
          title.push_back((uint8_t)c);
        }
      }
      title.push_back(0);
    }
    else if (displayName.empty() && (strings != nullptr)) {
      std::vector<uint16_t> text;
      CustomMaps_AppendStrUnits((*strings)[0],false,CUSTOM_MAP_TITLE_UNITS + 8,&text);
      size_t first = 0;
      while ((first < text.size()) && (text[first] == ' ')) {
        first++;
      }
      if (first < text.size()) {
        title.push_back(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1);
        title.insert(title.end(),text.begin() + first,text.begin() + std::min(text.size(),first + CUSTOM_MAP_TITLE_UNITS));
        title.push_back(0);
      }
    }
    for (size_t line = 0; line < CUSTOM_MAP_DESCRIPTION_LINES; line++) {
      std::vector<uint16_t> &text = texts->lines[language][line];
      if ((strings != nullptr) && (line + 1 < strings->size())) {
        CustomMaps_AppendStrUnits((*strings)[line + 1],true,CUSTOM_MAP_DESCRIPTION_UNITS,&text);
      }
      if ((line == 0) && !author.empty()) {
        static const char labelGerman[] = "Autor: ";
        static const char labelEnglish[] = "Author: ";
        if (!text.empty()) {
          text.push_back(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK);
          text.push_back(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK);
        }
        for (const char *c = (language == CUSTOM_MAP_LANGUAGE_GERMAN) ? labelGerman : labelEnglish; *c != '\0'; c++) {
          text.push_back((uint8_t)*c);
        }
        text.insert(text.end(),author.begin(),author.end());
      }
      if (!text.empty()) {
        text.push_back(0);
      }
    }
  }
}

/* The text array index of the current text language (0 = the fallback). */
static int CustomMaps_LanguageIndex()
{
  const uint32_t countryCode = TextResource_ActiveLanguage();

  for (int language = 0; language < CUSTOM_MAP_LANGUAGE_COUNT; language++) {
    if (g_CustomMapLanguageCountries[language] == countryCode) {
      return language + 1;
    }
  }
  return 0;
}

static uint16_t *CustomMaps_Title(CustomMapEntry *entry)
{
  const int language = CustomMaps_LanguageIndex();

  return (entry->texts.titles[language].empty() ? entry->texts.titles[0] : entry->texts.titles[language]).data();
}

/* Line line of the description block: the language's line, else the fallback's, else the description. */
static uint16_t *CustomMaps_DescriptionLine(CustomMapEntry *entry,size_t line)
{
  const int language = CustomMaps_LanguageIndex();

  for (size_t candidate : {line,(size_t)0}) {
    for (int index : {language,0}) {
      std::vector<uint16_t> &text = entry->texts.lines[index][candidate];
      if (!text.empty()) {
        return text.data();
      }
    }
  }
  return g_CustomMapEmptyText;
}

/* ---------------------------------------------------------------- the checks of one project */

/* The scan's own checks of the level (before CustomMap_Validate): the bytes FrontendLevelAsset_LoadedImageFits and
   the field grid load use, and the field grid path, which must be the project's field entry without ".fld"
   (level\<x>: the map can load no field grid but its own). */
static bool CustomMaps_CheckLevel(const CustomProject &project,std::string *outReason)
{
  const std::vector<uint8_t> &lev = project.lev;

  if (lev.size() < CUSTOM_MAP_LEV_MIN_BYTES) {
    *outReason = "level shorter than 0x800 bytes";
    return false;
  }
  if (lev.size() > CUSTOM_MAP_LEV_LIMIT_BYTES) {
    *outReason = "level larger than 1 MiB";
    return false;
  }
  if ((CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_MAGIC) != ASSET_MAGIC_LEV) ||
      (CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_CONVERTER) != PCK_CONVERTER_LEV_00070001)) {
    *outReason = "not a level (magic or converter version)";
    return false;
  }
  if (CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_SIZE) > lev.size()) {
    *outReason = "level header size larger than the level";
    return false;
  }
  const uint32_t pathOffset = CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_FIELD_PATH);
  if ((pathOffset > 0xffff) || (pathOffset % 2 != 0) || (pathOffset >= lev.size())) {
    *outReason = "field grid path offset outside the level";
    return false;
  }
  /* "level/<x>.fld" -> "level\<x>" */
  std::string expected = project.fieldEntry.substr(0,project.fieldEntry.size() - 4);
  std::replace(expected.begin(),expected.end(),'/','\\');
  const size_t unitsAvailable = (lev.size() - pathOffset) / 2;
  size_t unitIndex = 0;
  for (; unitIndex < unitsAvailable; unitIndex++) {
    uint16_t unit;
    std::memcpy(&unit,lev.data() + pathOffset + 2 * unitIndex,sizeof unit);
    if (unit == 0) {
      break;
    }
    if ((unitIndex >= expected.size()) ||
        (CustomMaps_LowerUnit(unit) != (uint16_t)(uint8_t)CustomMaps_LowerAscii(expected[unitIndex]))) {
      *outReason = "field grid path is not " + expected;
      return false;
    }
  }
  if ((unitIndex != expected.size()) || (unitsAvailable - unitIndex < CUSTOM_MAP_PATH_EXTENSION_UNITS)) {
    *outReason = "field grid path is not " + expected + " (terminated, with room for the extension)";
    return false;
  }
  return true;
}

/* Reads and checks the project maps\<name>.thmap into *check (size and write time as given); on success also
   *outProject when not nullptr. */
static void CustomMaps_LoadAndCheck(const std::string &name,CustomMapFileCheck *check,CustomProject *outProject)
{
  CustomProject project;

  check->valid = false;
  if (check->bytes > CustomProject_MaxFileBytes()) {
    check->reason = "file too large (" + std::to_string(check->bytes) + " bytes, at most " +
                    std::to_string(CustomProject_MaxFileBytes()) + ")";
    return;
  }
  if (!CustomProject_Load(CustomMaps_ProjectPath(name),&project,&check->reason) ||
      !CustomMaps_CheckLevel(project,&check->reason)) {
    return;
  }
  char reason[256];
  reason[0] = '\0';
  /* the game's own field grid check first (it logs its reason), then the full custom map check */
  if (!FieldGrid_ValidateLoadedImage(reinterpret_cast<const FieldGridAsset *>(project.fld.data()),
                                     (uint32_t)std::min<size_t>(project.fld.size(),UINT32_MAX))) {
    check->reason = "field grid rejected (see the line above)";
    return;
  }
  if (!CustomMap_Validate(project.lev.data(),project.lev.size(),project.fld.data(),project.fld.size(),reason,
                          sizeof reason)) {
    reason[sizeof reason - 1] = '\0';
    check->reason = (reason[0] != '\0') ? reason : "rejected by the map check";
    return;
  }
  check->valid = true;
  check->reason.clear();
  std::memcpy(check->record.data(),project.lev.data() + CUSTOM_MAP_LEV_RECORD,check->record.size());
  check->texts = CustomMapTexts{};
  CustomMaps_BuildTexts(name,project,&check->texts);
  if (outProject != nullptr) {
    *outProject = std::move(project);
  }
}

/* Size and write time of maps\<name>.thmap; false when it is missing. */
static bool CustomMaps_StatProject(const std::string &name,uintmax_t *outBytes,std::filesystem::file_time_type *outTime)
{
  const std::filesystem::path path = CustomMaps_ProjectPath(name);
  std::error_code error;

  *outBytes = std::filesystem::file_size(path,error);
  if (!error) {
    *outTime = std::filesystem::last_write_time(path,error);
  }
  return !error;
}

/* Checks maps\<name>.thmap, from the cache while its size and write time are unchanged. */
static const CustomMapFileCheck &CustomMaps_CheckFile(const std::string &name)
{
  CustomMapFileCheck check;

  if (!CustomMaps_StatProject(name,&check.bytes,&check.time)) {
    check.reason = name + ".thmap missing";
    return s_CustomMapFileChecks[CustomMaps_Lower(name)] = check;
  }
  auto cached = s_CustomMapFileChecks.find(CustomMaps_Lower(name));
  if ((cached != s_CustomMapFileChecks.end()) && (cached->second.bytes == check.bytes) &&
      (cached->second.time == check.time)) {
    return cached->second;
  }
  CustomMaps_LoadAndCheck(name,&check,nullptr);
  return s_CustomMapFileChecks[CustomMaps_Lower(name)] = check;
}

/* ---------------------------------------------------------------- the scan */

/* FNV-1a of the lowercased name: the preferred title slot */
static uint32_t CustomMaps_PreferredSlot(const std::string &lowerName)
{
  uint32_t hash = 0x811C9DC5u;

  for (char c : lowerName) {
    hash = (hash ^ (uint8_t)c) * 0x01000193u;
  }
  return hash % CUSTOM_MAP_TITLE_INDEX_SLOTS;
}

/* Whether name is taken by a stock level: a record of the catalog's level section ("<name>.") or a level\<name>.lev
   in a mounted package or loose next to the executable. */
static bool CustomMaps_CollidesWithStockLevel(const ScenarioCatalogHeader *catalog,const std::string &lowerName)
{
  if (catalog != nullptr) {
    const ScenarioCatalogRecord *record =
        reinterpret_cast<const ScenarioCatalogRecord *>(reinterpret_cast<const uint8_t *>(catalog) +
                                                        catalog->levelRecordsOffset);
    for (uint32_t recordIndex = 0; recordIndex < catalog->levelRecordCount; recordIndex++, record++) {
      size_t unitIndex = 0;
      for (; unitIndex < lowerName.size(); unitIndex++) {
        if (CustomMaps_LowerUnit(record->identifier[unitIndex]) != (uint8_t)lowerName[unitIndex]) {
          break;
        }
      }
      if ((unitIndex == lowerName.size()) &&
          ((record->identifier[unitIndex] == '.') || (record->identifier[unitIndex] == 0))) {
        return true;
      }
    }
  }
  const std::string stockPath = "level\\" + lowerName + ".lev";
  uint16_t lookupPath[THANDOR_PATH_CAPACITY] = {};
  for (size_t unitIndex = 0; unitIndex < stockPath.size(); unitIndex++) {
    lookupPath[unitIndex] = (uint8_t)stockPath[unitIndex];
  }
  EngineFileHandle fileHandle = 0;
  if (Package_FindEntryAcrossMounts(lookupPath,&fileHandle) != nullptr) { /* lowercases lookupPath in place */
    return true;
  }
  std::error_code error;
  return std::filesystem::exists(CustomMaps_ExecutableDirectory() / std::filesystem::path(stockPath),error);
}

/* Turns the .lev's catalog record into the list row: identifier "<name>.", title index, the three column text
   indices kept inside the stock ranges (docs/level_format.md). */
static void CustomMaps_BuildRecord(CustomMapEntry *entry,const CustomMapFileCheck &check)
{
  ScenarioCatalogDisplayRecord &record = entry->record;

  std::memcpy(&record,check.record.data(),sizeof record);
  std::memset(record.identifier,0,sizeof record.identifier);
  for (size_t unitIndex = 0; unitIndex < entry->name.size(); unitIndex++) {
    record.identifier[unitIndex] = (uint8_t)entry->name[unitIndex];
  }
  record.identifier[entry->name.size()] = '.';
  record.scenarioTextResourceId = entry->titleIndex;
  if (record.titleTextResourceId > 7) {
    record.titleTextResourceId = 0;
  }
  if (record.subtitleTextResourceId > 4) {
    record.subtitleTextResourceId = 0;
  }
  if (record.modeTextResourceId > 4) {
    record.modeTextResourceId = 0;
  }
}

/* Whether path's extension is ".thmap" (case ignored). */
static bool CustomMaps_IsProjectFile(const std::filesystem::path &path)
{
  std::wstring extension = path.extension().wstring();
  for (wchar_t &c : extension) {
    c = (wchar_t)CustomMaps_LowerUnit((uint16_t)c);
  }
  return extension == L".thmap";
}

static void CustomMaps_RefreshHintTexts();

void CustomMaps_Rebuild(const ScenarioCatalogHeader *catalog)
{
  const std::filesystem::path mapsFolder = CustomMaps_ExecutableDirectory() / L"maps";
  std::error_code error;

  CustomMaps_Deactivate(); /* the scenario selection: no custom map is loaded now */
  s_CustomMapBySlot.fill(nullptr);
  s_RetiredCustomMaps = std::move(s_CustomMaps);
  s_CustomMaps.clear();
  s_CustomMapsCatalog = catalog;
  s_CustomMapsRejected = 0;
  s_CustomMapsScanSkipped = !OtFeature_Active(OT_FEATURE_CUSTOM_MAPS);
  if (s_CustomMapsScanSkipped) {
    return; /* "Original Thandor": the original lists no user maps (a loaded custom map save stays playable) */
  }
  if (!std::filesystem::is_directory(mapsFolder,error)) {
    return;
  }
  /* the project names, sorted (case ignored) so that the slot probing does not depend on the directory order */
  std::vector<std::wstring> projectNames;
  for (std::filesystem::directory_iterator it(mapsFolder,error), end; !error && (it != end); it.increment(error)) {
    std::error_code typeError;
    if (it->is_directory(typeError)) {
      Thandor_Log("custom map folder \"%ls\" ignored: custom maps are map editor projects maps\\<name>.thmap",
                  it->path().filename().wstring().c_str());
    }
    else if (it->is_regular_file(typeError) && CustomMaps_IsProjectFile(it->path())) {
      projectNames.push_back(it->path().stem().wstring());
    }
  }
  if (error) {
    Thandor_Log("custom maps: reading the maps folder failed (%s)",error.message().c_str());
  }
  std::sort(projectNames.begin(),projectNames.end(),[](const std::wstring &left,const std::wstring &right) {
    return std::lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),
                                        [](wchar_t a,wchar_t b) { return CustomMaps_LowerUnit(a) < CustomMaps_LowerUnit(b); });
  });
  uint32_t skippedForLimit = 0;
  for (const std::wstring &projectName : projectNames) {
    if (!CustomMaps_IsValidName(projectName)) {
      Thandor_Log("custom map \"%ls.thmap\" skipped: the name must be 1..%d characters A-Z a-z 0-9 _ -",
                  projectName.c_str(),CUSTOM_MAP_NAME_MAX_UNITS);
      s_CustomMapsRejected++;
      continue;
    }
    std::string name(projectName.begin(),projectName.end()); /* ASCII, checked above */
    std::string lowerName = CustomMaps_Lower(name);
    if (CustomMaps_CollidesWithStockLevel(catalog,lowerName)) {
      Thandor_Log("custom map \"%s\" skipped: an original level has this name",name.c_str());
      s_CustomMapsRejected++;
      continue;
    }
    if (s_CustomMaps.size() >= (size_t)CUSTOM_MAP_LIMIT) {
      skippedForLimit++;
      Thandor_Log("custom map \"%s\" skipped: more than %d custom maps",name.c_str(),CUSTOM_MAP_LIMIT);
      continue;
    }
    const CustomMapFileCheck &check = CustomMaps_CheckFile(name);
    if (!check.valid) {
      Thandor_Log("custom map \"%s\" skipped: %s",name.c_str(),check.reason.c_str());
      s_CustomMapsRejected++;
      continue;
    }
    auto entry = std::make_unique<CustomMapEntry>();
    entry->name = name;
    entry->lowerName = lowerName;
    uint32_t slot = CustomMaps_PreferredSlot(lowerName);
    while (s_CustomMapBySlot[slot] != nullptr) {
      slot = (slot + 1) % CUSTOM_MAP_TITLE_INDEX_SLOTS;
    }
    s_CustomMapBySlot[slot] = entry.get();
    entry->titleIndex = CUSTOM_MAP_TITLE_INDEX_BASE + slot;
    entry->texts = check.texts;
    CustomMaps_BuildRecord(entry.get(),check);
    s_CustomMaps.push_back(std::move(entry));
  }
  Thandor_Log("custom maps: %u listed%s",(unsigned)s_CustomMaps.size(),
              (skippedForLimit != 0) ? " (limit reached, see above)" : "");
  CustomMaps_RefreshHintTexts(); /* the rejected count */
}

uint32_t CustomMaps_Count()
{
  return (uint32_t)s_CustomMaps.size();
}

const char *CustomMaps_ListedName(uint32_t index)
{
  return (index < s_CustomMaps.size()) ? s_CustomMaps[index]->name.c_str() : nullptr;
}

/* ---------------------------------------------------------------- the list */

/* Writes text (ASCII) behind leadingUnit (0: none) into destination, cut to its size. */
template <size_t N>
static void CustomMaps_SetHintText(std::array<uint16_t,N> &destination,uint16_t leadingUnit,const std::string &text)
{
  size_t length = 0;

  if (leadingUnit != 0) {
    destination[length++] = leadingUnit;
  }
  for (size_t index = 0; (index < text.size()) && (length + 1 < N); index++) {
    destination[length++] = (uint8_t)text[index];
  }
  destination[length] = 0;
}

/* The hint row's texts in the current language (German for German, English otherwise, as the list suffix). */
static void CustomMaps_RefreshHintTexts()
{
  const bool german = TextResource_ProjectTextsAreGerman();
  std::string rejected;

  if (s_CustomMapsRejected == 1) {
    rejected = german ? " (1 abgelehnt, siehe thandor.log)" : " (1 file rejected, see thandor.log)";
  }
  else if (s_CustomMapsRejected > 1) {
    rejected = " (" + std::to_string(s_CustomMapsRejected) +
               (german ? " abgelehnt, siehe thandor.log)" : " files rejected, see thandor.log)");
  }
  /* palette 2 (dark grey): the row is no map */
  CustomMaps_SetHintText(s_CustomMapHintTitle,RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_2,
                         german ? "Eigene Karten: keine gefunden" : "Custom maps: none found");
  /* with the rejected files the editor's name is left out (the link below names it), so the text keeps to two
     lines in the description box at 640x480 and the link above the buttons */
  CustomMaps_SetHintText(s_CustomMapHintDescription,0,
                         german ? "Keine eigenen Karten gefunden" + rejected +
                                      (rejected.empty() ? ". Lege .thmap-Dateien des Thandor Map Editors"
                                                        : ". Lege .thmap-Dateien") +
                                      " in den Ordner maps neben thandor.exe."
                                : "No custom maps found" + rejected +
                                      (rejected.empty() ? ". Put Thandor Map Editor .thmap files" : ". Put .thmap files") +
                                      " into the maps folder next to thandor.exe.");
  CustomMaps_SetHintText(s_CustomMapHintLink,0,german ? "Map Editor auf GitHub" : "Map Editor on GitHub");
}

void CustomMaps_RefreshLanguage()
{
  CustomMaps_RefreshHintTexts();

  const bool german = TextResource_ProjectTextsAreGerman();
  static const char markerGerman[] = " (eigene Karte)";
  static const char markerEnglish[] = " (custom map)";
  const char *marker = german ? markerGerman : markerEnglish;

  for (const std::unique_ptr<CustomMapEntry> &entry : s_CustomMaps) {
    std::array<uint16_t,CUSTOM_MAP_LIST_TITLE_UNITS> &listTitle = entry->listTitle;
    const uint16_t *title = CustomMaps_Title(entry.get());
    size_t length = 0;
    /* palette 0, as the stock rows show in a local game (FrontendScenarioList_LevelAvailabilityMarker) */
    listTitle[length++] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
    for (title++; (*title != 0) && (length + 1 < listTitle.size()); title++) {
      listTitle[length++] = *title;
    }
    for (const char *markerChar = marker; (*markerChar != '\0') && (length + 1 < listTitle.size()); markerChar++) {
      listTitle[length++] = (uint8_t)*markerChar;
    }
    listTitle[length] = 0;
  }
}

bool CustomMaps_IsHintRecord(const void *record)
{
  return record == &s_CustomMapHintRecord;
}

Ptr32<ScenarioCatalogDisplayRecord> *CustomMaps_ListRows(Ptr32<ScenarioCatalogDisplayRecord> *stockRows,
                                                         uint32_t stockCount,uint32_t *outCount,bool *outHintRow)
{
  *outCount = stockCount;
  *outHintRow = false;
  if (!OtFeature_Active(OT_FEATURE_CUSTOM_MAPS)) {
    return stockRows; /* "Original Thandor" was switched on since the last scan: the original list */
  }
  if (s_CustomMapsScanSkipped && (s_CustomMapsCatalog != nullptr)) {
    CustomMaps_Rebuild(s_CustomMapsCatalog); /* switched off since a skipped scan */
  }
  if (s_CustomMapsScanSkipped) {
    return stockRows;
  }
  CustomMaps_RefreshLanguage();
  if (s_CustomMaps.empty()) {
    /* the hint row behind the stock rows (custom_maps.h); identifier unit 0x01: no -KARTE name (every unit >= ' ')
       equals it, and no level path is ever built from it */
    ScenarioCatalogDisplayRecord &record = s_CustomMapHintRecord;
    std::memset(&record,0,sizeof record);
    record.identifier[0] = 0x01;
    record.scenarioTextResourceId = TEXT_ID_CUSTOM_MAPS_HINT_TITLE;
    record.titleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.titleResolvedText = g_CustomMapEmptyText;
    record.subtitleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.subtitleResolvedText = g_CustomMapEmptyText;
    record.scenarioDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.scenarioResolvedText = s_CustomMapHintTitle.data();
    record.modeDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.modeResolvedText = g_CustomMapEmptyText;
    s_CustomMapListRows.assign(stockRows,stockRows + stockCount);
    s_CustomMapListRows.push_back(&record);
    *outHintRow = true;
    return s_CustomMapListRows.data();
  }
  s_CustomMapListRows.assign(stockRows,stockRows + stockCount);
  for (const std::unique_ptr<CustomMapEntry> &entry : s_CustomMaps) {
    ScenarioCatalogDisplayRecord &record = entry->record;
    /* the column texts as the stock rows have them (ScenarioCatalog_RebuildLevelRecordListPage) */
    record.titleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.titleResolvedText = TextResource_Resolve(record.titleTextResourceId + TEXT_ID_LEVEL_COLUMN50_BASE);
    record.subtitleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.subtitleResolvedText = TextResource_Resolve(record.subtitleTextResourceId + TEXT_ID_LEVEL_COLUMN60_BASE);
    record.scenarioDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.scenarioResolvedText = entry->listTitle.data();
    record.modeDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
    record.modeResolvedText = TextResource_Resolve(record.modeTextResourceId + TEXT_ID_LEVEL_COLUMN80_BASE);
    s_CustomMapListRows.push_back(&record);
  }
  *outCount = (uint32_t)s_CustomMapListRows.size();
  return s_CustomMapListRows.data();
}

/* ---------------------------------------------------------------- paths and the loaded level */

/* (also among the previous scan's entries: list rows built before the last scan still point at them) */
static CustomMapEntry *CustomMaps_FindByRecord(const void *record)
{
  for (const std::vector<std::unique_ptr<CustomMapEntry>> *entries : {&s_CustomMaps,&s_RetiredCustomMaps}) {
    for (const std::unique_ptr<CustomMapEntry> &entry : *entries) {
      if (record == &entry->record) {
        return entry.get();
      }
    }
  }
  return nullptr;
}

static CustomMapEntry *CustomMaps_FindByName(const std::string &lowerName)
{
  for (const std::unique_ptr<CustomMapEntry> &entry : s_CustomMaps) {
    if (entry->lowerName == lowerName) {
      return entry.get();
    }
  }
  return nullptr;
}

/* Writes "maps\<name>." into destination (the level path without its extension; nothing of that name exists on
   disk: the level is served from the project, CustomMaps_FindServedAsset). */
static void CustomMaps_WriteLevelPathStem(uint16_t *destination,const std::string &name)
{
  const std::string path = "maps\\" + name + ".";
  size_t unitIndex = 0;

  for (; unitIndex < path.size(); unitIndex++) {
    destination[unitIndex] = (uint8_t)path[unitIndex];
  }
  destination[unitIndex] = 0;
}

bool CustomMaps_LevelPathOfRecord(uint16_t *destination,const void *record)
{
  CustomMapEntry *entry = CustomMaps_FindByRecord(record);

  if (entry == nullptr) {
    return false;
  }
  CustomMaps_WriteLevelPathStem(destination,entry->name);
  return true;
}

/* True when text (terminated) starts with "maps\" (case ignored); *outRest then points behind it. */
static bool CustomMaps_StartsWithMapsFolder(const uint16_t *text,const uint16_t **outRest)
{
  static const char prefix[] = "maps\\";

  for (size_t unitIndex = 0; prefix[unitIndex] != '\0'; unitIndex++) {
    const uint16_t unit = (text[unitIndex] == '/') ? '\\' : CustomMaps_LowerUnit(text[unitIndex]);
    if (unit != (uint8_t)prefix[unitIndex]) {
      return false;
    }
  }
  *outRest = text + sizeof prefix - 1;
  return true;
}

/* The name at text up to the first unit of stops (or the terminator), when it is a valid map name. */
static bool CustomMaps_NameAt(const uint16_t *text,size_t maxUnits,const char *stops,std::string *outName)
{
  std::wstring name;

  for (size_t unitIndex = 0; (unitIndex < maxUnits) && (text[unitIndex] != 0); unitIndex++) {
    if ((text[unitIndex] < 0x80) && (std::strchr(stops,(char)text[unitIndex]) != nullptr)) {
      break;
    }
    name.push_back((wchar_t)text[unitIndex]);
  }
  if (!CustomMaps_IsValidName(name)) {
    return false;
  }
  outName->assign(name.begin(),name.end());
  return true;
}

bool CustomMaps_IsCustomIdentifier(const uint16_t *identifier)
{
  const uint16_t *rest;

  return CustomMaps_StartsWithMapsFolder(identifier,&rest);
}

bool CustomMaps_LevelPathOfIdentifier(uint16_t *destination,const uint16_t *identifier)
{
  const uint16_t *rest;
  std::string name;

  if (!CustomMaps_StartsWithMapsFolder(identifier,&rest) || !CustomMaps_NameAt(rest,CUSTOM_MAP_NAME_MAX_UNITS + 1,".",&name)) {
    return false;
  }
  CustomMaps_WriteLevelPathStem(destination,name);
  return true;
}

/* The map name of a level path maps\<name>.lev. */
static bool CustomMaps_NameOfLevelPath(const uint16_t *levelPath,std::string *outName)
{
  const uint16_t *rest;
  std::string name;
  static const char extension[] = ".lev";

  if (!CustomMaps_StartsWithMapsFolder(levelPath,&rest) ||
      !CustomMaps_NameAt(rest,CUSTOM_MAP_NAME_MAX_UNITS + 1,".\\/",&name)) {
    return false;
  }
  rest += name.size();
  for (size_t unitIndex = 0; unitIndex < sizeof extension; unitIndex++) {
    if (CustomMaps_LowerUnit(rest[unitIndex]) != (uint8_t)extension[unitIndex]) {
      return false;
    }
  }
  *outName = name;
  return true;
}

void CustomMaps_PrepareLoadedLevel(FrontendLoadedLevelAsset *level,const uint16_t *levelPath)
{
  std::string name;

  if ((level == nullptr) || !CustomMaps_NameOfLevelPath(levelPath,&name)) {
    return;
  }
  const CustomMapEntry *entry = CustomMaps_FindByName(CustomMaps_Lower(name));
  const std::string identifier = "maps\\" + (entry != nullptr ? entry->name : name) + ".";
  uint16_t *levelName = level->header.levelFileNameUtf16;
  std::memset(levelName,0,sizeof level->header.levelFileNameUtf16);
  for (size_t unitIndex = 0; unitIndex < identifier.size(); unitIndex++) {
    levelName[unitIndex] = (uint8_t)identifier[unitIndex];
  }
  /* a map no longer listed (removed since the scan) gets its preferred slot: the fallback title */
  level->header.titleTextResourceIndex =
      (entry != nullptr) ? entry->titleIndex
                         : CUSTOM_MAP_TITLE_INDEX_BASE + CustomMaps_PreferredSlot(CustomMaps_Lower(name));
}

bool CustomMaps_LevelPathPlayable(const uint16_t *levelPath)
{
  std::string name;

  if (CustomMaps_NameOfLevelPath(levelPath,&name)) {
    const CustomMapFileCheck &check = CustomMaps_CheckFile(name);
    if (!check.valid) {
      Thandor_Log("custom map \"%s\" cannot be loaded again: %s",name.c_str(),check.reason.c_str());
    }
    return check.valid;
  }
  uint16_t lookupPath[THANDOR_PATH_CAPACITY] = {};
  size_t unitIndex = 0;
  for (; (unitIndex + 1 < THANDOR_PATH_CAPACITY) && (levelPath[unitIndex] != 0); unitIndex++) {
    lookupPath[unitIndex] = levelPath[unitIndex];
  }
  EngineFileHandle fileHandle = 0;
  if (Package_FindEntryAcrossMounts(lookupPath,&fileHandle) != nullptr) { /* lowercases lookupPath in place */
    return true;
  }
  const std::filesystem::path relative(std::wstring(reinterpret_cast<const wchar_t *>(levelPath),unitIndex));
  std::error_code error;
  if (std::filesystem::exists(CustomMaps_ExecutableDirectory() / relative,error) ||
      std::filesystem::exists(relative,error)) {
    return true;
  }
  Thandor_Log("level \"%ls\" cannot be loaded again: not found",reinterpret_cast<const wchar_t *>(levelPath));
  return false;
}

/* ---------------------------------------------------------------- the active project */

void CustomMaps_Deactivate()
{
  if (s_CustomMapActive != nullptr) {
    Thandor_Log("custom map \"%s\": project files released",s_CustomMapActive->lowerName.c_str());
    s_CustomMapActive.reset();
  }
}

/* Makes maps\<name>.thmap the active project (again read and checked unless it is active and unchanged). */
static bool CustomMaps_ActivateName(const std::string &name)
{
  const std::string lowerName = CustomMaps_Lower(name);
  uintmax_t bytes = 0;
  std::filesystem::file_time_type time{};

  if (!CustomMaps_StatProject(name,&bytes,&time)) {
    CustomMaps_Deactivate();
    s_CustomMapFileChecks.erase(lowerName);
    Thandor_Log("custom map \"%s\" cannot be loaded: %s.thmap missing",name.c_str(),name.c_str());
    return false;
  }
  if ((s_CustomMapActive != nullptr) && (s_CustomMapActive->lowerName == lowerName) &&
      (s_CustomMapActive->bytes == bytes) && (s_CustomMapActive->time == time)) {
    return true;
  }
  CustomMaps_Deactivate();
  CustomMapFileCheck check;
  CustomProject project;
  check.bytes = bytes;
  check.time = time;
  CustomMaps_LoadAndCheck(name,&check,&project);
  s_CustomMapFileChecks[lowerName] = check;
  if (!check.valid) {
    Thandor_Log("custom map \"%s\" cannot be loaded: %s",name.c_str(),check.reason.c_str());
    return false;
  }
  auto active = std::make_unique<CustomMapActiveProject>();
  active->lowerName = lowerName;
  active->bytes = bytes;
  active->time = time;
  active->served = std::move(project.servedAssets);
  active->served.insert_or_assign("maps/" + lowerName + ".lev",std::move(project.lev));
  active->served.insert_or_assign(project.fieldEntry,std::move(project.fld));
  Thandor_Log("custom map \"%s\": %s project, %u files served (field grid %s)",name.c_str(),
              project.zip ? "ZIP" : "legacy PCK",(unsigned)active->served.size(),project.fieldEntry.c_str());
  s_CustomMapActive = std::move(active);
  return true;
}

bool CustomMaps_ActivateLevelPath(const uint16_t *levelPath)
{
  std::string name;

  if ((levelPath == nullptr) || !CustomMaps_NameOfLevelPath(levelPath,&name)) {
    CustomMaps_Deactivate();
    return true;
  }
  return CustomMaps_ActivateName(name);
}

bool CustomMaps_ActivateIdentifier(const uint16_t *identifier)
{
  const uint16_t *rest;
  std::string name;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL ||
      !CustomMaps_StartsWithMapsFolder(identifier,&rest) ||
      !CustomMaps_NameAt(rest,CUSTOM_MAP_NAME_MAX_UNITS + 1,".",&name)) {
    CustomMaps_Deactivate();
    return true;
  }
  return CustomMaps_ActivateName(name);
}

bool CustomMaps_FindServedAsset(const uint16_t *path,const uint8_t **outData,uint32_t *outBytes)
{
  if ((s_CustomMapActive == nullptr) || (path == nullptr)) {
    return false;
  }
  std::string key;
  for (size_t unitIndex = 0; path[unitIndex] != 0; unitIndex++) {
    const uint16_t unit = path[unitIndex];
    if ((unit >= 0x80) || (unitIndex >= THANDOR_PATH_CAPACITY)) {
      return false;
    }
    key.push_back((unit == '\\') ? '/' : CustomMaps_LowerAscii((char)unit));
  }
  const auto served = s_CustomMapActive->served.find(key);
  if (served == s_CustomMapActive->served.end()) {
    return false;
  }
  *outData = served->second.data();
  *outBytes = (uint32_t)served->second.size();
  return true;
}

bool CustomMaps_CheckProjectForTest(const std::string &name,std::string *outReason)
{
  const CustomMapFileCheck &check = CustomMaps_CheckFile(name);

  *outReason = check.reason;
  return check.valid;
}

/* ---------------------------------------------------------------- texts */

bool CustomMaps_TryResolveText(uint32_t resourceId,uint16_t **outText)
{
  if ((resourceId >= TEXT_ID_CUSTOM_MAPS_HINT_BASE) &&
      (resourceId < TEXT_ID_CUSTOM_MAPS_HINT_BASE + TEXT_ID_CUSTOM_MAPS_HINT_RESERVED)) {
    if (s_CustomMapHintTitle[0] == 0) {
      CustomMaps_RefreshHintTexts(); /* before the first list build */
    }
    *outText = (resourceId == TEXT_ID_CUSTOM_MAPS_HINT_TITLE)         ? s_CustomMapHintTitle.data()
               : (resourceId == TEXT_ID_CUSTOM_MAPS_HINT_DESCRIPTION) ? s_CustomMapHintDescription.data()
               : (resourceId == TEXT_ID_CUSTOM_MAPS_HINT_LINK)        ? s_CustomMapHintLink.data()
                                                                      : g_CustomMapEmptyText;
    return true;
  }
  if ((resourceId >= CUSTOM_MAP_TITLE_ID_FIRST) && (resourceId < CUSTOM_MAP_TITLE_ID_FIRST + CUSTOM_MAP_TITLE_INDEX_SLOTS)) {
    CustomMapEntry *entry = s_CustomMapBySlot[resourceId - CUSTOM_MAP_TITLE_ID_FIRST];
    if (entry != nullptr) {
      *outText = CustomMaps_Title(entry);
    }
    else {
      *outText = TextResource_ProjectTextsAreGerman() ? g_CustomMapFallbackTitleGerman : g_CustomMapFallbackTitleEnglish;
    }
    return true;
  }
  if ((resourceId >= CUSTOM_MAP_DESCRIPTION_ID_FIRST) &&
      (resourceId < CUSTOM_MAP_DESCRIPTION_ID_FIRST + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * CUSTOM_MAP_TITLE_INDEX_SLOTS)) {
    const uint32_t offset = resourceId - CUSTOM_MAP_DESCRIPTION_ID_FIRST;
    CustomMapEntry *entry = s_CustomMapBySlot[offset / TEXT_ID_LEVEL_DESCRIPTION_STRIDE];
    *outText = (entry != nullptr) ? CustomMaps_DescriptionLine(entry,offset % TEXT_ID_LEVEL_DESCRIPTION_STRIDE)
                                  : g_CustomMapEmptyText;
    return true;
  }
  return false;
}
