/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/custom_maps.cpp
 */

/* Not in the original: custom maps in maps\<name>\ (see custom_maps.h, docs/custom_maps.md). Native memory and
   std::filesystem only: the arena (g_MemoryApi) must not see the scan, or the arena order of a later session would
   shift; without a maps folder the scan returns before it allocates anything. */

#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <vector>
#include <thandor/assets/scenario/custom_maps.h>
#include <thandor/assets/scenario/custom_map_validate.h>
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
/* Size limits of the files read (a stock .lev has 6 to 22 KiB, an expanded .fld up to 2.4 MiB). */
static constexpr uintmax_t CUSTOM_MAP_LEV_LIMIT_BYTES = 0x100000;
static constexpr uintmax_t CUSTOM_MAP_FLD_LIMIT_BYTES = 0x4000000;
static constexpr uintmax_t CUSTOM_MAP_INI_LIMIT_BYTES = 0x10000;
/* Code units of a title (without the leading style unit) and of a description. */
static constexpr size_t CUSTOM_MAP_TITLE_UNITS = 64;
static constexpr size_t CUSTOM_MAP_DESCRIPTION_UNITS = 1024;
static constexpr size_t CUSTOM_MAP_LIST_TITLE_UNITS = 1 + CUSTOM_MAP_TITLE_UNITS + 24;

/* The title and description ids of the custom title indices (custom_maps.h). */
static constexpr uint32_t CUSTOM_MAP_TITLE_ID_FIRST = TEXT_ID_LEVEL_TITLE_BASE + CUSTOM_MAP_TITLE_INDEX_BASE;
static constexpr uint32_t CUSTOM_MAP_DESCRIPTION_ID_FIRST =
    TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * CUSTOM_MAP_TITLE_INDEX_BASE;
static_assert(CUSTOM_MAP_TITLE_ID_FIRST == 0x2500);
static_assert(CUSTOM_MAP_TITLE_ID_FIRST + CUSTOM_MAP_TITLE_INDEX_SLOTS <= 0x2D00,
              "the custom titles stay on the unused compact text pages 0x25..0x2C");

/* The map.ini languages: the telephone country codes of the language choice and their key suffixes. Index 0 of the
   text arrays is the plain key (title, description). */
static constexpr int CUSTOM_MAP_LANGUAGE_COUNT = 5;
static const struct {
  uint32_t countryCode;
  const char *suffix;
} g_CustomMapLanguages[CUSTOM_MAP_LANGUAGE_COUNT] = {
    {49, "de"}, {44, "en"}, {33, "fr"}, {39, "it"}, {34, "es"}};

struct CustomMapEntry {
  std::string name;   /* the folder name (ASCII, as found) */
  std::string lowerName;
  uint32_t titleIndex = 0;
  ScenarioCatalogDisplayRecord record{}; /* the list row: the .lev's catalog record with our identifier and index */
  /* per language (0 = plain key): the title with a leading style unit (the callers overwrite the first unit), the
     description; empty vectors fall back to index 0 */
  std::vector<uint16_t> titles[1 + CUSTOM_MAP_LANGUAGE_COUNT];
  std::vector<uint16_t> descriptions[1 + CUSTOM_MAP_LANGUAGE_COUNT];
  std::array<uint16_t,CUSTOM_MAP_LIST_TITLE_UNITS> listTitle{}; /* the list row's title text, see RefreshLanguage */
};

/* The result of checking the files of one folder, kept while their sizes and write times stay the same. */
struct CustomMapFileCheck {
  uintmax_t levBytes = 0;
  uintmax_t fldBytes = 0;
  std::filesystem::file_time_type levTime{};
  std::filesystem::file_time_type fldTime{};
  bool valid = false;
  std::string reason;
  std::array<uint8_t,SCENARIO_CATALOG_RECORD_SIZE> record{};
};

/* Module data. The previous scan's entries stay alive one more scan: list rows may still point at them. */
static std::vector<std::unique_ptr<CustomMapEntry>> s_CustomMaps;
static std::vector<std::unique_ptr<CustomMapEntry>> s_RetiredCustomMaps;
static std::array<CustomMapEntry *,CUSTOM_MAP_TITLE_INDEX_SLOTS> s_CustomMapBySlot{};
static std::map<std::string,CustomMapFileCheck> s_CustomMapFileChecks;
static std::vector<Ptr32<ScenarioCatalogDisplayRecord>> s_CustomMapListRows;
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

static std::filesystem::path CustomMaps_Folder(const std::string &name)
{
  return CustomMaps_ExecutableDirectory() / L"maps" / std::filesystem::path(name);
}

/* Reads a whole file of at most limit bytes; false with the reason otherwise. */
static bool CustomMaps_ReadFile(const std::filesystem::path &path,uintmax_t limit,std::vector<uint8_t> *outBytes,
                                std::string *outReason)
{
  std::error_code error;
  const uintmax_t size = std::filesystem::file_size(path,error);

  if (error) {
    *outReason = path.filename().string() + " missing";
    return false;
  }
  if (size > limit) {
    *outReason = path.filename().string() + " too large (" + std::to_string(size) + " bytes)";
    return false;
  }
  std::ifstream stream(path,std::ios::binary);
  outBytes->assign((size_t)size,0);
  if (!stream || !stream.read(reinterpret_cast<char *>(outBytes->data()),(std::streamsize)size)) {
    *outReason = path.filename().string() + " cannot be read";
    return false;
  }
  return true;
}

/* ---------------------------------------------------------------- the checks of one folder */

/* The scan's own checks of the .lev (before CustomMap_Validate): the bytes FrontendLevelAsset_LoadedImageFits and the
   field grid load use, and the field grid path maps\<name>\<name> (no other directory: the map must not reach files
   outside its folder). */
static bool CustomMaps_CheckLevel(const std::string &name,const std::vector<uint8_t> &lev,std::string *outReason)
{
  if (lev.size() < CUSTOM_MAP_LEV_MIN_BYTES) {
    *outReason = "level file shorter than 0x800 bytes";
    return false;
  }
  if ((CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_MAGIC) != ASSET_MAGIC_LEV) ||
      (CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_CONVERTER) != PCK_CONVERTER_LEV_00070001)) {
    *outReason = "not a level file (magic or converter version)";
    return false;
  }
  if (CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_SIZE) > lev.size()) {
    *outReason = "level header size larger than the file";
    return false;
  }
  const uint32_t pathOffset = CustomMaps_LoadU32(lev,CUSTOM_MAP_LEV_FIELD_PATH);
  if ((pathOffset > 0xffff) || (pathOffset % 2 != 0) || (pathOffset >= lev.size())) {
    *outReason = "field grid path offset outside the level";
    return false;
  }
  const std::string expected = "maps\\" + name + "\\" + name;
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

/* Checks the folder's .lev and .fld, from the cache while their sizes and write times are unchanged. */
static const CustomMapFileCheck &CustomMaps_CheckFiles(const std::string &name)
{
  const std::filesystem::path folder = CustomMaps_Folder(name);
  const std::filesystem::path levPath = folder / (name + ".lev");
  const std::filesystem::path fldPath = folder / (name + ".fld");
  std::error_code error;
  CustomMapFileCheck check;

  check.levBytes = std::filesystem::file_size(levPath,error);
  if (!error) {
    check.levTime = std::filesystem::last_write_time(levPath,error);
  }
  std::error_code fldError;
  check.fldBytes = std::filesystem::file_size(fldPath,fldError);
  if (!fldError) {
    check.fldTime = std::filesystem::last_write_time(fldPath,fldError);
  }
  auto cached = s_CustomMapFileChecks.find(CustomMaps_Lower(name));
  if (!error && !fldError && (cached != s_CustomMapFileChecks.end()) &&
      (cached->second.levBytes == check.levBytes) && (cached->second.fldBytes == check.fldBytes) &&
      (cached->second.levTime == check.levTime) && (cached->second.fldTime == check.fldTime)) {
    return cached->second;
  }
  std::vector<uint8_t> lev;
  std::vector<uint8_t> fld;
  if (CustomMaps_ReadFile(levPath,CUSTOM_MAP_LEV_LIMIT_BYTES,&lev,&check.reason) &&
      CustomMaps_CheckLevel(name,lev,&check.reason) &&
      CustomMaps_ReadFile(fldPath,CUSTOM_MAP_FLD_LIMIT_BYTES,&fld,&check.reason)) {
    char reason[256];
    reason[0] = '\0';
    /* the game's own field grid check first (it logs its reason), then the full custom map check */
    if (!FieldGrid_ValidateLoadedImage(reinterpret_cast<const FieldGridAsset *>(fld.data()),(uint32_t)fld.size())) {
      check.reason = "field grid rejected (see the line above)";
    }
    else if (CustomMap_Validate(lev.data(),lev.size(),fld.data(),fld.size(),reason,sizeof reason)) {
      check.valid = true;
      std::memcpy(check.record.data(),lev.data() + CUSTOM_MAP_LEV_RECORD,check.record.size());
    }
    else {
      reason[sizeof reason - 1] = '\0';
      check.reason = (reason[0] != '\0') ? reason : "rejected by the map check";
    }
  }
  return s_CustomMapFileChecks[CustomMaps_Lower(name)] = check;
}

/* ---------------------------------------------------------------- map.ini */

/* Appends the code points of UTF-8 text as display code units: Latin-1 stays, every other character becomes '?',
   control characters are dropped; with lineBreaks the two characters \n become a rich-text line break. False for
   malformed UTF-8. */
static bool CustomMaps_AppendUtf8(const std::string &text,bool lineBreaks,size_t limit,std::vector<uint16_t> *out)
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
    if (lineBreaks && (codePoint == '\\') && (index + 1 < text.size()) && (text[index + 1] == 'n')) {
      codePoint = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK; /* not a character: no Latin-1 filter */
      length = 2;
    }
    else if ((codePoint < 0x20) || ((codePoint >= 0x7F) && (codePoint < 0xA0))) {
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

static std::string CustomMaps_Trim(const std::string &text)
{
  size_t first = 0;
  size_t last = text.size();

  while ((first < last) && ((text[first] == ' ') || (text[first] == '\t'))) {
    first++;
  }
  while ((last > first) && ((text[last - 1] == ' ') || (text[last - 1] == '\t') || (text[last - 1] == '\r'))) {
    last--;
  }
  std::string trimmed = text.substr(first,last - first);
  if ((trimmed.size() >= 2) && (trimmed.front() == '"') && (trimmed.back() == '"')) {
    trimmed = trimmed.substr(1,trimmed.size() - 2);
  }
  return trimmed;
}

/* The keys of the [map] section of maps\<name>\map.ini (lowercased keys; empty when the file is missing or not
   readable, which is logged). */
static std::map<std::string,std::string> CustomMaps_ReadIni(const std::string &name)
{
  std::map<std::string,std::string> keys;
  std::vector<uint8_t> bytes;
  std::string reason;

  if (!CustomMaps_ReadFile(CustomMaps_Folder(name) / L"map.ini",CUSTOM_MAP_INI_LIMIT_BYTES,&bytes,&reason)) {
    Thandor_Log("custom map \"%s\": %s, the folder name is its title",name.c_str(),reason.c_str());
    return keys;
  }
  std::string text(bytes.begin(),bytes.end());
  if (text.compare(0,3,"\xEF\xBB\xBF") == 0) {
    text.erase(0,3);
  }
  bool inMapSection = false;
  bool foundMapSection = false;
  size_t lineStart = 0;
  while (lineStart <= text.size()) {
    size_t lineEnd = text.find('\n',lineStart);
    if (lineEnd == std::string::npos) {
      lineEnd = text.size();
    }
    const std::string line = CustomMaps_Trim(text.substr(lineStart,lineEnd - lineStart));
    lineStart = lineEnd + 1;
    if (line.empty() || (line[0] == ';') || (line[0] == '#')) {
      continue;
    }
    if (line[0] == '[') {
      inMapSection = CustomMaps_Lower(line) == "[map]";
      foundMapSection = foundMapSection || inMapSection;
      continue;
    }
    const size_t equals = line.find('=');
    if (!inMapSection || (equals == std::string::npos)) {
      continue;
    }
    keys[CustomMaps_Lower(CustomMaps_Trim(line.substr(0,equals)))] = CustomMaps_Trim(line.substr(equals + 1));
  }
  if (!foundMapSection) {
    Thandor_Log("custom map \"%s\": map.ini has no [map] section, the folder name is its title",name.c_str());
  }
  return keys;
}

/* Fills the entry's titles and descriptions from map.ini; the plain title falls back to the folder name. */
static void CustomMaps_ReadTexts(CustomMapEntry *entry)
{
  const std::map<std::string,std::string> keys = CustomMaps_ReadIni(entry->name);

  for (int language = 0; language <= CUSTOM_MAP_LANGUAGE_COUNT; language++) {
    const std::string suffix = (language == 0) ? "" : std::string(".") + g_CustomMapLanguages[language - 1].suffix;
    auto title = keys.find("title" + suffix);
    std::vector<uint16_t> &titleText = entry->titles[language];
    if ((title != keys.end()) && !title->second.empty()) {
      titleText.push_back(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1); /* overwritten by the callers */
      if (!CustomMaps_AppendUtf8(title->second,false,CUSTOM_MAP_TITLE_UNITS,&titleText) || (titleText.size() == 1)) {
        Thandor_Log("custom map \"%s\": title%s in map.ini is not usable UTF-8 text, ignored",entry->name.c_str(),
                    suffix.c_str());
        titleText.clear();
      }
      else {
        titleText.push_back(0);
      }
    }
    auto description = keys.find("description" + suffix);
    std::vector<uint16_t> &descriptionText = entry->descriptions[language];
    if ((description != keys.end()) && !description->second.empty()) {
      if (!CustomMaps_AppendUtf8(description->second,true,CUSTOM_MAP_DESCRIPTION_UNITS,&descriptionText)) {
        Thandor_Log("custom map \"%s\": description%s in map.ini is not UTF-8, ignored",entry->name.c_str(),
                    suffix.c_str());
        descriptionText.clear();
      }
      else {
        descriptionText.push_back(0);
      }
    }
  }
  if (entry->titles[0].empty()) {
    entry->titles[0].push_back(RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1);
    for (char c : entry->name) {
      entry->titles[0].push_back((uint8_t)c);
    }
    entry->titles[0].push_back(0);
  }
}

/* The text array index of the current text language (0 = the plain key). */
static int CustomMaps_LanguageIndex()
{
  const uint32_t countryCode = TextResource_ActiveLanguage();

  for (int language = 0; language < CUSTOM_MAP_LANGUAGE_COUNT; language++) {
    if (g_CustomMapLanguages[language].countryCode == countryCode) {
      return language + 1;
    }
  }
  return 0;
}

static uint16_t *CustomMaps_Title(CustomMapEntry *entry)
{
  const int language = CustomMaps_LanguageIndex();

  return (entry->titles[language].empty() ? entry->titles[0] : entry->titles[language]).data();
}

static uint16_t *CustomMaps_Description(CustomMapEntry *entry)
{
  const int language = CustomMaps_LanguageIndex();

  if (!entry->descriptions[language].empty()) {
    return entry->descriptions[language].data();
  }
  return entry->descriptions[0].empty() ? g_CustomMapEmptyText : entry->descriptions[0].data();
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

void CustomMaps_Rebuild(const ScenarioCatalogHeader *catalog)
{
  const std::filesystem::path mapsFolder = CustomMaps_ExecutableDirectory() / L"maps";
  std::error_code error;

  s_CustomMapBySlot.fill(nullptr);
  s_RetiredCustomMaps = std::move(s_CustomMaps);
  s_CustomMaps.clear();
  s_CustomMapsCatalog = catalog;
  s_CustomMapsScanSkipped = !OtFeature_Active(OT_FEATURE_CUSTOM_MAPS);
  if (s_CustomMapsScanSkipped) {
    return; /* "Original Thandor": the original lists no user maps (a loaded custom map save stays playable) */
  }
  if (!std::filesystem::is_directory(mapsFolder,error)) {
    return;
  }
  /* the folder names, sorted (case ignored) so that the slot probing does not depend on the directory order */
  std::vector<std::wstring> folderNames;
  for (std::filesystem::directory_iterator it(mapsFolder,error), end; !error && (it != end); it.increment(error)) {
    std::error_code typeError;
    if (it->is_directory(typeError)) {
      folderNames.push_back(it->path().filename().wstring());
    }
  }
  if (error) {
    Thandor_Log("custom maps: reading the maps folder failed (%s)",error.message().c_str());
  }
  std::sort(folderNames.begin(),folderNames.end(),[](const std::wstring &left,const std::wstring &right) {
    return std::lexicographical_compare(left.begin(),left.end(),right.begin(),right.end(),
                                        [](wchar_t a,wchar_t b) { return CustomMaps_LowerUnit(a) < CustomMaps_LowerUnit(b); });
  });
  uint32_t skippedForLimit = 0;
  for (const std::wstring &folderName : folderNames) {
    if (!CustomMaps_IsValidName(folderName)) {
      Thandor_Log("custom map folder \"%ls\" skipped: the name must be 1..%d characters A-Z a-z 0-9 _ -",
                  folderName.c_str(),CUSTOM_MAP_NAME_MAX_UNITS);
      continue;
    }
    std::string name(folderName.begin(),folderName.end()); /* ASCII, checked above */
    std::string lowerName = CustomMaps_Lower(name);
    if (CustomMaps_CollidesWithStockLevel(catalog,lowerName)) {
      Thandor_Log("custom map \"%s\" skipped: an original level has this name",name.c_str());
      continue;
    }
    if (s_CustomMaps.size() >= (size_t)CUSTOM_MAP_LIMIT) {
      skippedForLimit++;
      Thandor_Log("custom map \"%s\" skipped: more than %d custom maps",name.c_str(),CUSTOM_MAP_LIMIT);
      continue;
    }
    const CustomMapFileCheck &check = CustomMaps_CheckFiles(name);
    if (!check.valid) {
      Thandor_Log("custom map \"%s\" skipped: %s",name.c_str(),check.reason.c_str());
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
    CustomMaps_ReadTexts(entry.get());
    CustomMaps_BuildRecord(entry.get(),check);
    s_CustomMaps.push_back(std::move(entry));
  }
  Thandor_Log("custom maps: %u listed%s",(unsigned)s_CustomMaps.size(),
              (skippedForLimit != 0) ? " (limit reached, see above)" : "");
}

uint32_t CustomMaps_Count()
{
  return (uint32_t)s_CustomMaps.size();
}

/* ---------------------------------------------------------------- the list */

void CustomMaps_RefreshLanguage()
{
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

Ptr32<ScenarioCatalogDisplayRecord> *CustomMaps_ListRows(Ptr32<ScenarioCatalogDisplayRecord> *stockRows,
                                                         uint32_t stockCount,uint32_t *outCount)
{
  *outCount = stockCount;
  if (!OtFeature_Active(OT_FEATURE_CUSTOM_MAPS)) {
    return stockRows; /* "Original Thandor" was switched on since the last scan: the original list */
  }
  if (s_CustomMapsScanSkipped && (s_CustomMapsCatalog != nullptr)) {
    CustomMaps_Rebuild(s_CustomMapsCatalog); /* switched off since a skipped scan */
  }
  if (s_CustomMaps.empty()) {
    return stockRows;
  }
  CustomMaps_RefreshLanguage();
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

/* Writes "maps\<name>\<name>." into destination. */
static void CustomMaps_WriteLevelPathStem(uint16_t *destination,const std::string &name)
{
  const std::string path = "maps\\" + name + "\\" + name + ".";
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

/* The map name of a level path maps\<name>\<name>.lev (the two names equal, case ignored). */
static bool CustomMaps_NameOfLevelPath(const uint16_t *levelPath,std::string *outName)
{
  const uint16_t *rest;
  std::string folderName;
  std::string fileName;

  if (!CustomMaps_StartsWithMapsFolder(levelPath,&rest) ||
      !CustomMaps_NameAt(rest,CUSTOM_MAP_NAME_MAX_UNITS + 1,"\\/",&folderName)) {
    return false;
  }
  rest += folderName.size();
  if ((*rest != '\\') && (*rest != '/')) {
    return false;
  }
  if (!CustomMaps_NameAt(rest + 1,CUSTOM_MAP_NAME_MAX_UNITS + 1,".",&fileName) ||
      (CustomMaps_Lower(fileName) != CustomMaps_Lower(folderName))) {
    return false;
  }
  *outName = folderName;
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
    const CustomMapFileCheck &check = CustomMaps_CheckFiles(name);
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

/* ---------------------------------------------------------------- texts */

bool CustomMaps_TryResolveText(uint32_t resourceId,uint16_t **outText)
{
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
    /* every line of the block (description, faction tasks, briefings) shows the description */
    CustomMapEntry *entry =
        s_CustomMapBySlot[(resourceId - CUSTOM_MAP_DESCRIPTION_ID_FIRST) / TEXT_ID_LEVEL_DESCRIPTION_STRIDE];
    *outText = (entry != nullptr) ? CustomMaps_Description(entry) : g_CustomMapEmptyText;
    return true;
  }
  return false;
}
