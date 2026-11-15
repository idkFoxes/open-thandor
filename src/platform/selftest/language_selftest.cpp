/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/language_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=language: the run-time language switch (TextResource_SelectLanguage). Decodes the text pages
   the game loads at startup (error, winclass and the eight pages of Game_LoadCoreAssets) from the packages in the
   current directory (mount order patch01, patch00, level, daten, ...; the first entry of a path wins) and binds them
   with TextResourcePage_BindAsset, as TextResourcePage_Load does. Then switches to every language menue.str offers,
   to auto and to a country without a block, and back to the language of the start, and checks:
     - every id of every page that the German block has resolves (no miss, no sentinel); ids that are empty where
       the German text is not are counted and logged (a gap of the data: the Italian level.str has 27);
     - the menu page is bound to the chosen language's block, the project texts are German for German only;
     - CoreAssets_BindLanguageTexts and every switch allocate and free nothing (allocation counter of g_MemoryApi);
     - back at the start language every id resolves to the same text address as before the switches;
     - for every language, every text of the rebound pages equals (code unit by code unit, the pointer slots of the
       nested-stream records aside) the text of a fresh copy of the page bound with that language requested from
       the start, i.e. the prepared blocks are what the original's load would have prepared.
   One line per failed check (the first few of each kind), one line per language and a summary.
   Without the packages it logs "language: skipped" (game data is not in the repository). */

#include <stdio.h>
#include <string.h>

#include <memory>
#include <new>
#include <string>
#include <vector>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>

namespace {

uint32_t s_LanguageTestAllocations = 0;
uint32_t s_LanguageTestFrees = 0;

uint32_t LanguageTest_Alloc(uint32_t bytes, void **outPayload)
{
  void *payload = ::operator new(bytes, std::nothrow);
  if (payload == nullptr) {
    return FATAL_ERROR_ARENA_EXHAUSTED;
  }
  s_LanguageTestAllocations++;
  *outPayload = payload;
  return 0;
}

uint32_t LanguageTest_Free(void *memory)
{
  s_LanguageTestFrees++;
  ::operator delete(memory);
  return 0;
}

struct LanguageTestPage {
  TextResourcePageIndex pageIndex;
  const char *path; /* lower case, as compared with the package entry paths */
  uint32_t stringCount;  /* ids that resolve with the German block bound */
  bool loaded;
};

/* Finds the entry path in the packages (first package and entry wins) and decodes it into a g_MemoryApi block. */
TextResourceAssetHeader *LanguageTest_LoadEntry(const char *const *packages, size_t packageCount, const char *wanted,
                                                uint8_t *packed, uint32_t *outByteCount)
{
  for (size_t packageIndex = 0; packageIndex < packageCount; packageIndex++) {
    FILE *pck = fopen(packages[packageIndex], "rb");
    if (pck == nullptr) {
      continue;
    }
    long position = PCK_ENTRY_HEADER_BYTES;
    for (;;) {
      PckEntryHeader header;
      if ((fseek(pck, position, SEEK_SET) != 0) || (fread(&header, sizeof header, 1, pck) != 1)) {
        break;
      }
      if ((header.packedSize == 0) || (header.packedSize > PACKAGE_SCRATCH_BUFFER_BYTES) ||
          ((uint32_t)header.compressionMethod >= sizeof g_PckDecoderTable / sizeof g_PckDecoderTable[0])) {
        break;
      }
      position += PCK_ENTRY_HEADER_BYTES + (long)header.packedSize;
      std::string path;
      for (int k = 0; k < PCK_ENTRY_PATH_UNITS && header.path[k] != 0; k++) {
        const uint16_t unit = header.path[k];
        path += (char)((unit >= 'A' && unit <= 'Z') ? unit + ('a' - 'A') : (unit < 0x80 ? unit : '?'));
      }
      if ((path != wanted) || (g_PckDecoderTable[header.compressionMethod] == nullptr)) {
        continue;
      }
      void *unpacked = nullptr;
      if ((fread(packed, 1, header.packedSize, pck) != header.packedSize) ||
          (g_MemoryApi.alloc(header.unpackedSize + 4, &unpacked) != 0)) {
        fclose(pck);
        return nullptr;
      }
      if (!g_PckDecoderTable[header.compressionMethod](header.unpackedSize, static_cast<uint8_t *>(unpacked), header.packedSize, packed,
                                                       nullptr, nullptr)) {
        g_MemoryApi.free(unpacked);
        fclose(pck);
        return nullptr;
      }
      fclose(pck);
      *outByteCount = header.unpackedSize;
      return static_cast<TextResourceAssetHeader *>(unpacked);
    }
    fclose(pck);
  }
  return nullptr;
}

/* The id of string index of a page: compact for page 0 (8-bit index), else extended (16-bit index). */
TextResourceId LanguageTest_Id(TextResourcePageIndex pageIndex, uint32_t index)
{
  return (pageIndex == 0) ? index : ((pageIndex << 16) | index);
}

/* The scratch page the fresh copies are bound to (unused by the game). */
constexpr TextResourcePageIndex LANGUAGE_TEST_SCRATCH_PAGE = 0x7E;

/* Whether two prepared rich-text streams are the same: every code unit up to the terminator, except the pointer
   slots of nested-stream records (patched at run time). */
bool LanguageTest_SameText(const uint16_t *left, const uint16_t *right)
{
  for (;;) {
    const uint16_t codeUnit = *left;
    int recordUnits = 1;

    if (codeUnit != *right) {
      return false;
    }
    if (codeUnit == 0) {
      return true;
    }
    if ((short)codeUnit < 0) {
      switch (codeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        recordUnits = RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        recordUnits = RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        if ((left[3] != right[3]) || (left[4] != right[4])) { /* the selector; units 1..2 are the pointer slot */
          return false;
        }
        left += RICHTEXT_RECORD_UNITS_NESTED;
        right += RICHTEXT_RECORD_UNITS_NESTED;
        continue;
      case RICHTEXT_OP_INLINE_IMAGE:
        recordUnits = RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        break;
      }
    }
    for (int unit = 1; unit < recordUnits; unit++) {
      if (left[unit] != right[unit]) {
        return false;
      }
    }
    left += recordUnits;
    right += recordUnits;
  }
}

uint32_t LanguageTest_IndexLimit(TextResourcePageIndex pageIndex)
{
  return (pageIndex == 0) ? 0x100 : 0x10000;
}

} // namespace

void Thandor_SelfTestLanguage()
{
  static const char *packages[] = {"PATCH01.PCK", "PATCH00.PCK", "LEVEL.PCK", "DATEN.PCK",
                                   "MODELLE.PCK", "GRAPHIK.PCK", "SOUND.PCK", "ENGINE.PCK"};
  LanguageTestPage pages[] = {
      {0, "texte\\error.str", 0, false},       {1, "texte\\winclass.str", 0, false},
      {GAME_TEXT_PAGE_NETERROR, "texte\\neterror.str", 0, false},
      {GAME_TEXT_PAGE_HELP, "texte\\help.str", 0, false},     {GAME_TEXT_PAGE_HILFE, "texte\\hilfe.str", 0, false},
      {GAME_TEXT_PAGE_MENUE, "texte\\menue.str", 0, false},   {GAME_TEXT_PAGE_TECHNO, "texte\\techno.str", 0, false},
      {GAME_TEXT_PAGE_LEVEL, "texte\\level.str", 0, false},   {GAME_TEXT_PAGE_INHALT, "texte\\inhalt.str", 0, false},
      {GAME_TEXT_PAGE_TASTATUR, "texte\\tastatur.str", 0, false}};
  constexpr uint32_t GERMANY = static_cast<uint32_t>(LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);
  constexpr uint32_t NO_BLOCK_COUNTRY = static_cast<uint32_t>(LocaleTelephoneCountryCode::LOCALE_COUNTRY_RUSSIA);
  uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
  uint32_t (*savedFree)(void *) = g_MemoryApi.free;
  unsigned failures = 0;
  unsigned checks = 0;
  unsigned loadedPages = 0;

  g_MemoryApi.alloc = LanguageTest_Alloc;
  g_MemoryApi.free = LanguageTest_Free;
  {
    auto packedBuffer = std::make_unique_for_overwrite<uint8_t[]>(PACKAGE_SCRATCH_BUFFER_BYTES);
    for (LanguageTestPage &page : pages) {
      uint32_t byteCount = 0;
      TextResourceAssetHeader *asset =
          LanguageTest_LoadEntry(packages, sizeof packages / sizeof packages[0], page.path, packedBuffer.get(),
                                 &byteCount);
      if (asset == nullptr) {
        continue;
      }
      uint16_t pathUtf16[64] = {};
      for (size_t index = 0; (page.path[index] != 0) && (index + 1 < 64); index++) {
        pathUtf16[index] = static_cast<uint8_t>(page.path[index]);
      }
      uintptr_t error = 0;
      page.loaded = TextResourcePage_BindAsset(page.pageIndex, pathUtf16, asset, byteCount, &error);
      if (page.loaded) {
        loadedPages++;
      }
      else {
        Thandor_Log("language: %s FAILED to bind (0x%IX)", page.path, error);
        failures++;
      }
    }
  }
  if (!pages[5].loaded) {
    g_MemoryApi.alloc = savedAlloc;
    g_MemoryApi.free = savedFree;
    Thandor_Log("language: skipped (menue.str not found in the packages of the current directory, %u pages)",
                loadedPages);
    return;
  }

  const uint32_t startLanguage = TextResource_SelectedLanguage();
  const uint32_t startActive = TextResource_ActiveLanguage();
  uint32_t languages[TEXT_RESOURCE_LANGUAGE_MAX];
  const int languageCount = TextResource_AvailableLanguages(languages, TEXT_RESOURCE_LANGUAGE_MAX);
  {
    char list[64] = {};
    size_t used = 0;
    for (int index = 0; index < languageCount; index++) {
      used += (size_t)snprintf(list + used, sizeof list - used, "%s%u", index ? "," : "", languages[index]);
    }
    Thandor_Log("language: %u pages, start language %u (active %u), available: %s", loadedPages, startLanguage,
                startActive, list);
  }

  /* the text addresses at the start language */
  std::vector<std::vector<uint16_t *>> startTexts(sizeof pages / sizeof pages[0]);
  for (size_t pageSlot = 0; pageSlot < sizeof pages / sizeof pages[0]; pageSlot++) {
    if (!pages[pageSlot].loaded) {
      continue;
    }
    for (uint32_t index = 0; index < LanguageTest_IndexLimit(pages[pageSlot].pageIndex); index++) {
      uint16_t *text;
      if (!TextResource_TryResolve(LanguageTest_Id(pages[pageSlot].pageIndex, index), &text)) {
        break;
      }
      startTexts[pageSlot].push_back(text);
    }
  }

  const uint32_t allocationsBefore = s_LanguageTestAllocations;
  const uint32_t freesBefore = s_LanguageTestFrees;
  bool germanAvailable = false;
  for (int index = 0; index < languageCount; index++) {
    germanAvailable = germanAvailable || (languages[index] == GERMANY);
  }
  if (!germanAvailable) {
    Thandor_Log("language: no German block in menue.str, the German reference is the first language");
  }
  /* the reference: which ids the German (else the first) block has, and which of them have text */
  TextResource_SelectLanguage(germanAvailable ? GERMANY : ((languageCount > 0) ? languages[0] : startLanguage));
  std::vector<std::vector<bool>> referenceHasText(sizeof pages / sizeof pages[0]);
  for (size_t pageSlot = 0; pageSlot < sizeof pages / sizeof pages[0]; pageSlot++) {
    if (!pages[pageSlot].loaded) {
      continue;
    }
    for (uint32_t index = 0; index < LanguageTest_IndexLimit(pages[pageSlot].pageIndex); index++) {
      uint16_t *text;
      if (!TextResource_TryResolve(LanguageTest_Id(pages[pageSlot].pageIndex, index), &text)) {
        break;
      }
      referenceHasText[pageSlot].push_back(text[0] != 0);
    }
    pages[pageSlot].stringCount = (uint32_t)referenceHasText[pageSlot].size();
  }

  std::vector<uint32_t> sequence(languages, languages + languageCount);
  sequence.push_back(TEXT_RESOURCE_LANGUAGE_AUTO);
  sequence.push_back(NO_BLOCK_COUNTRY);
  for (uint32_t language : sequence) {
    unsigned missing = 0;
    unsigned empty = 0;
    unsigned resolved = 0;

    TextResource_SelectLanguage(language);
    CoreAssets_BindLanguageTexts();
    const uint32_t active = TextResource_ActiveLanguage();
    checks++;
    if ((language != TEXT_RESOURCE_LANGUAGE_AUTO) && (language != NO_BLOCK_COUNTRY) && (active != language)) {
      Thandor_Log("language %u: FAILED, menue.str bound to %u", language, active);
      failures++;
    }
    if (language == NO_BLOCK_COUNTRY) {
      /* the fallback: Great Britain when there is a block for it, else the first block */
      bool britain = false;
      for (int index = 0; index < languageCount; index++) {
        britain = britain || (languages[index] == static_cast<uint32_t>(LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN));
      }
      if (britain && (active != static_cast<uint32_t>(LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN))) {
        Thandor_Log("language %u: FAILED, fallback bound %u instead of 44", language, active);
        failures++;
      }
    }
    checks++;
    uint16_t *projectText = TextResource_Resolve(TEXT_ID_DISPLAY_MODE_KIND_TITLE);
    const bool projectGerman = (projectText[0] == 'A'); /* "Anzeigemodus:" / "Display mode:" */
    if (projectGerman != (active == GERMANY) || (TextResource_ProjectTextsAreGerman() != (active == GERMANY))) {
      Thandor_Log("language %u: FAILED, project texts %s with %u bound", language,
                  projectGerman ? "German" : "English", active);
      failures++;
    }
    for (size_t pageSlot = 0; pageSlot < sizeof pages / sizeof pages[0]; pageSlot++) {
      if (!pages[pageSlot].loaded) {
        continue;
      }
      for (uint32_t index = 0; index < pages[pageSlot].stringCount; index++) {
        const TextResourceId id = LanguageTest_Id(pages[pageSlot].pageIndex, index);
        uint16_t *text = nullptr;
        checks++;
        if (!TextResource_TryResolve(id, &text) ||
            (text == Thandor_U32ToPointer<uint16_t>(TEXT_RESOURCE_MISSING_SENTINEL_0x33))) {
          if (missing++ < 4) {
            Thandor_Log("language %u: FAILED, id 0x%08X (%s) missing", language, id, pages[pageSlot].path);
          }
          continue;
        }
        resolved++;
        if (referenceHasText[pageSlot][index] && (text[0] == 0)) {
          if (empty++ < 4) {
            Thandor_Log("language %u: id 0x%08X (%s) empty where the German text is not", language, id,
                        pages[pageSlot].path);
          }
        }
      }
    }
    failures += missing;
    Thandor_Log("language %u: bound %u, %u ids resolved, %u missing, %u empty where the German text is not",
                language, active, resolved, missing, empty);
  }

  /* back to the start language: the same text addresses as before */
  TextResource_SelectLanguage(startLanguage);
  CoreAssets_BindLanguageTexts();
  unsigned moved = 0;
  for (size_t pageSlot = 0; pageSlot < sizeof pages / sizeof pages[0]; pageSlot++) {
    for (uint32_t index = 0; index < startTexts[pageSlot].size(); index++) {
      uint16_t *text = nullptr;
      checks++;
      if (!TextResource_TryResolve(LanguageTest_Id(pages[pageSlot].pageIndex, index), &text) ||
          (text != startTexts[pageSlot][index])) {
        if (moved++ < 4) {
          Thandor_Log("language: FAILED, id 0x%08X differs after switching back",
                      LanguageTest_Id(pages[pageSlot].pageIndex, index));
        }
      }
    }
  }
  failures += moved;
  checks++;
  if ((s_LanguageTestAllocations != allocationsBefore) || (s_LanguageTestFrees != freesBefore)) {
    Thandor_Log("language: FAILED, the switches allocated %u and freed %u blocks",
                s_LanguageTestAllocations - allocationsBefore, s_LanguageTestFrees - freesBefore);
    failures++;
  }

  /* every language against a fresh copy bound with that language requested from the start */
  {
    auto packedBuffer = std::make_unique_for_overwrite<uint8_t[]>(PACKAGE_SCRATCH_BUFFER_BYTES);
    for (int languageIndex = 0; languageIndex < languageCount; languageIndex++) {
      const uint32_t language = languages[languageIndex];
      unsigned different = 0;
      unsigned compared = 0;

      TextResource_SelectLanguage(language);
      for (LanguageTestPage &page : pages) {
        if (!page.loaded) {
          continue;
        }
        uint32_t byteCount = 0;
        uint16_t pathUtf16[2] = {'-', 0};
        TextResourceAssetHeader *asset =
            LanguageTest_LoadEntry(packages, sizeof packages / sizeof packages[0], page.path, packedBuffer.get(),
                                   &byteCount);
        if ((asset == nullptr) ||
            !TextResourcePage_BindAsset(LANGUAGE_TEST_SCRATCH_PAGE, pathUtf16, asset, byteCount, nullptr)) {
          Thandor_Log("language %u: FAILED to bind a fresh copy of %s", language, page.path);
          failures++;
          continue;
        }
        for (uint32_t index = 0; index < LanguageTest_IndexLimit(page.pageIndex); index++) {
          uint16_t *text;
          uint16_t *fresh;
          const bool found = TextResource_TryResolve(LanguageTest_Id(page.pageIndex, index), &text);
          const bool freshFound = TextResource_TryResolve(LanguageTest_Id(LANGUAGE_TEST_SCRATCH_PAGE, index), &fresh);
          if (!found && !freshFound) {
            break;
          }
          checks++;
          compared++;
          if ((found != freshFound) || !LanguageTest_SameText(text, fresh)) {
            if (different++ < 4) {
              Thandor_Log("language %u: FAILED, id 0x%08X (%s) differs from a fresh load", language,
                          LanguageTest_Id(page.pageIndex, index), page.path);
            }
          }
        }
        g_MemoryApi.free(asset); /* the scratch binding is not read again until the next bind */
      }
      failures += different;
      Thandor_Log("language %u: %u texts compared with a fresh load, %u differ", language, compared, different);
    }
    TextResource_SelectLanguage(startLanguage);
  }

  g_MemoryApi.alloc = savedAlloc;
  g_MemoryApi.free = savedFree;
  if (failures == 0) {
    Thandor_Log("language: ok, %u checks, %d languages", checks, languageCount);
  }
  else {
    Thandor_Log("language: %u of %u checks FAILED", failures, checks);
  }
}
