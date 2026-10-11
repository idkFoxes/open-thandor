/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/resources.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/resources.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/scenario/catalog.h>
#include <thandor/assets/text/richtext.h>
#include <thandor/core/text/string.h>

#include <cstdio>

/* Module data. */

static TextResourcePageBinding g_TextResourcePageBindings[256] = {};

TextResourceOverrideTable *g_TextResourceOverrides = nullptr;

static uint16_t g_EmptyTextResourceUtf16[2] = {};

/* UTF-16 rich-text stream L"-" (code unit '-' plus terminator) that unresolved nested-stream records point to */
static uint16_t g_MissingTextResourceFallbackStream[2] = {0x002D, 0x0000};

/* Not in the original: the texts of TEXT_ID_PROJECT_BASE.. (UTF-16 rich-text streams). The choices start with the
   opcode "colour from palette entry 3" (0x8000 | RICHTEXT_OP_COLOR_PALETTE_3) as the adapter and colour depth
   choices of the display settings page do; the group title is plain like the original titles. */
#define PROJECT_TEXT_BRIGHT (0x8000 | RICHTEXT_OP_COLOR_PALETTE_3)
static uint16_t g_ProjectTextAnzeigemodus[] = {'A','n','z','e','i','g','e','m','o','d','u','s',':',0};
static uint16_t g_ProjectTextFenster[] = {PROJECT_TEXT_BRIGHT,'F','e','n','s','t','e','r',0};
static uint16_t g_ProjectTextVollbildfenster[] = {PROJECT_TEXT_BRIGHT,'V','o','l','l','b','i','l','d','f','e','n','s','t','e','r',0};
static uint16_t g_ProjectTextVollbild[] = {PROJECT_TEXT_BRIGHT,'V','o','l','l','b','i','l','d',0};
/* the options page's display button and title, and the advanced settings page (texts and choices) */
static uint16_t g_ProjectTextAnzeige[] = {'A','n','z','e','i','g','e',0};
static uint16_t g_ProjectTextAnzeigeeinstellungen[] = {'A','n','z','e','i','g','e','e','i','n','s','t','e','l','l','u','n','g','e','n',0};
static uint16_t g_ProjectTextErweitert[] = {'E','r','w','e','i','t','e','r','t',0};
static uint16_t g_ProjectTextErweiterteEinstellungen[] = {'E','r','w','e','i','t','e','r','t','e',' ','E','i','n','s','t','e','l','l','u','n','g','e','n',0};
static uint16_t g_ProjectText3DKanten[] = {'3','D','-','K','a','n','t','e','n',':',0};
static uint16_t g_ProjectTextGlatt[] = {PROJECT_TEXT_BRIGHT,'G','l','a','t','t',0};
static uint16_t g_ProjectTextOriginal[] = {PROJECT_TEXT_BRIGHT,'O','r','i','g','i','n','a','l',0};
static uint16_t g_ProjectTextUiSkalierung[] = {'U','I','-','S','k','a','l','i','e','r','u','n','g',':',0};
static uint16_t g_ProjectTextAuto[] = {PROJECT_TEXT_BRIGHT,'A','u','t','o',0};
static uint16_t g_ProjectText1x[] = {PROJECT_TEXT_BRIGHT,'1','x',0};
static uint16_t g_ProjectText2x[] = {PROJECT_TEXT_BRIGHT,'2','x',0};
static uint16_t g_ProjectText3x[] = {PROJECT_TEXT_BRIGHT,'3','x',0};
static uint16_t g_ProjectTextBildratenbegrenzung[] = {'B','i','l','d','r','a','t','e','n','b','e','g','r','e','n','z','u','n','g',':',0};
static uint16_t g_ProjectTextAus[] = {PROJECT_TEXT_BRIGHT,'A','u','s',0};
static uint16_t g_ProjectText60[] = {PROJECT_TEXT_BRIGHT,'6','0',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectText120[] = {PROJECT_TEXT_BRIGHT,'1','2','0',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectText144[] = {PROJECT_TEXT_BRIGHT,'1','4','4',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectTextVSync[] = {PROJECT_TEXT_BRIGHT,'V','S','y','n','c',0};
static uint16_t g_ProjectTextNoteSoftware[] = {'3','D','-','K','a','n','t','e','n',' ','u','n','d',' ','U','I','-','S','k','a','l','i','e','r','u','n','g',' ','w','i','r','k','e','n',' ','n','u','r',' ','m','i','t',' ','V','u','l','k','a','n',' ','o','d','e','r',' ','D','i','r','e','c','t','X',' ','1','2',0};
static uint16_t g_ProjectTextNoteUiScale[96] = {}; /* TextResource_SetUiScaleNote */
/* the campaign map: status line templates ("%u" mission number, "%s" mission title), the legend item and the short
   campaign names (plain: the controls choose the colour), and the two filled-in lines
   (TextResource_SetCampaignMapLine) */
static uint16_t g_ProjectTextCampaignDone[] = {'A','b','g','e','s','c','h','l','o','s','s','e','n',' ','-',' ','e','r','n','e','u','t',' ','s','p','i','e','l','b','a','r','.',0};
static uint16_t g_ProjectTextCampaignDoneCarry[] = {'A','b','g','e','s','c','h','l','o','s','s','e','n',' ','-',' ','e','r','n','e','u','t',' ','s','p','i','e','l','b','a','r','.',' ',0x00DC,'b','e','r','n','i','m','m','t',' ','E','i','n','h','e','i','t','e','n',' ','a','u','s',' ','M','i','s','s','i','o','n',' ','%','u',' ','(','%','s',')','.',0};
static uint16_t g_ProjectTextCampaignUnlockedFirst[] = {'F','r','e','i','g','e','s','c','h','a','l','t','e','t',0};
static uint16_t g_ProjectTextCampaignUnlocked[] = {'F','r','e','i','g','e','s','c','h','a','l','t','e','t',' ','-',' ','f','o','l','g','t',' ','a','u','f',' ','M','i','s','s','i','o','n',' ','%','u',' ','(','%','s',')',0};
static uint16_t g_ProjectTextCampaignUnlockedBranch[] = {'F','r','e','i','g','e','s','c','h','a','l','t','e','t',' ','-',' ','a','l','t','e','r','n','a','t','i','v','e','r',' ','Z','w','e','i','g',' ','n','a','c','h',' ','M','i','s','s','i','o','n',' ','%','u',' ','(','%','s',')',0};
static uint16_t g_ProjectTextCampaignLocked[] = {'G','e','s','p','e','r','r','t',' ','-',' ','f','o','l','g','t',' ','a','u','f',' ','M','i','s','s','i','o','n',' ','%','u',' ','(','%','s',')',0};
static uint16_t g_ProjectTextCampaignNeedsCarry[] = {'B','e','n',0x00F6,'t','i','g','t',' ','d','i','e',' ','E','i','n','h','e','i','t','e','n',' ','a','u','s',' ','M','i','s','s','i','o','n',' ','%','u',' ','(','%','s',')',' ','-',' ','M','i','s','s','i','o','n',' ','%','u',' ','g','e','w','i','n','n','e','n','.',0};
static uint16_t g_ProjectTextCampaignNetworkCarry[] = {'I','m',' ','N','e','t','z','w','e','r','k','s','p','i','e','l',' ','n','u','r',' ',0x00FC,'b','e','r',' ','M','i','s','s','i','o','n',' ','%','u',' ','e','r','r','e','i','c','h','b','a','r','.',0};
static uint16_t g_ProjectTextCampaignMissionHeading[] = {'M','i','s','s','i','o','n',' ','%','u',':',' ','%','s',0};
static uint16_t g_ProjectTextCampaignLegendCarry[] = {'E','i','n','h','e','i','t','e','n',0x00FC,'b','e','r','n','a','h','m','e',0};
static uint16_t g_ProjectTextCampaignTutorial[] = {'T','u','t','o','r','i','a','l',0};
static uint16_t g_ProjectTextCampaignInvasion[] = {'D','i','e',' ','I','n','v','a','s','i','o','n',0};
static uint16_t g_ProjectTextCampaignKyrrdon[] = {'K','y','r','r','d','o','n',0};
static uint16_t g_ProjectTextCampaignPiraten[] = {'P','i','r','a','t','e','n',0};
/* the .cgn base names of the stock campaigns (TextResource_CampaignShortName) */
static uint16_t g_CampaignFileNameTutorial[] = {'t','u','t','o','r','i','a','l',0};
static uint16_t g_CampaignFileNameHansolo[] = {'h','a','n','s','o','l','o',0};
static uint16_t g_CampaignFileNameNimm2[] = {'n','i','m','m','2',0};
static uint16_t g_CampaignFileNameLuke[] = {'l','u','k','e',0};
static uint16_t g_ProjectTextCampaignHeadingLine[128] = {};
static uint16_t g_ProjectTextCampaignStatusLine[160] = {};
/* the campaign button labels (TextResource_SetCampaignButtonLabel) */
static uint16_t g_ProjectTextCampaignButtonLabels[TEXT_ID_CAMPAIGN_BUTTON_LABEL_COUNT][TEXT_ID_CAMPAIGN_BUTTON_LABEL_UNITS] = {};
/* the language choice of the advanced settings page: box title, the "from Windows" choice (bright like the other
   choices) and the language button's label (TextResource_SetLanguageButtonLabel) */
static uint16_t g_ProjectTextSprache[] = {'S','p','r','a','c','h','e',':',0};
static uint16_t g_ProjectTextAutomatisch[] = {PROJECT_TEXT_BRIGHT,'A','u','t','o','m','a','t','i','s','c','h',0};
static uint16_t g_ProjectTextLanguageButtonLabel[TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS] = {};
static uint16_t *const g_ProjectTexts[TEXT_ID_PROJECT_COUNT] = {
    g_ProjectTextAnzeigemodus, g_ProjectTextFenster, g_ProjectTextVollbildfenster, g_ProjectTextVollbild,
    g_ProjectTextAnzeige, g_ProjectTextAnzeigeeinstellungen, g_ProjectTextErweitert, g_ProjectTextErweiterteEinstellungen,
    g_ProjectText3DKanten, g_ProjectTextGlatt, g_ProjectTextOriginal, g_ProjectTextUiSkalierung,
    g_ProjectTextAuto, g_ProjectText1x, g_ProjectText2x, g_ProjectText3x,
    g_ProjectTextBildratenbegrenzung, g_ProjectTextAus, g_ProjectText60, g_ProjectText120,
    g_ProjectText144, g_ProjectTextVSync, g_ProjectTextNoteSoftware, g_ProjectTextNoteUiScale,
    g_ProjectTextCampaignDone, g_ProjectTextCampaignDoneCarry, g_ProjectTextCampaignUnlockedFirst, g_ProjectTextCampaignUnlocked,
    g_ProjectTextCampaignUnlockedBranch, g_ProjectTextCampaignLocked, g_ProjectTextCampaignNeedsCarry, g_ProjectTextCampaignNetworkCarry,
    g_ProjectTextCampaignMissionHeading, g_ProjectTextCampaignLegendCarry, g_ProjectTextCampaignTutorial, g_ProjectTextCampaignInvasion,
    g_ProjectTextCampaignKyrrdon, g_ProjectTextCampaignPiraten, g_ProjectTextCampaignHeadingLine, g_ProjectTextCampaignStatusLine,
    g_ProjectTextCampaignButtonLabels[0], g_ProjectTextCampaignButtonLabels[1], g_ProjectTextCampaignButtonLabels[2],
    g_ProjectTextCampaignButtonLabels[3], g_ProjectTextCampaignButtonLabels[4], g_ProjectTextCampaignButtonLabels[5],
    g_ProjectTextSprache, g_ProjectTextAutomatisch, g_ProjectTextLanguageButtonLabel};

/* Not in the original: the language names (TEXT_ID_LANGUAGE_NAME_BASE..), each in its own language, bright like the
   choices; the same in every language. */
static uint16_t g_LanguageNameDeutsch[] = {PROJECT_TEXT_BRIGHT,'D','e','u','t','s','c','h',0};
static uint16_t g_LanguageNameEnglish[] = {PROJECT_TEXT_BRIGHT,'E','n','g','l','i','s','h',0};
static uint16_t g_LanguageNameFrancais[] = {PROJECT_TEXT_BRIGHT,'F','r','a','n',0x00E7,'a','i','s',0};
static uint16_t g_LanguageNameItaliano[] = {PROJECT_TEXT_BRIGHT,'I','t','a','l','i','a','n','o',0};
static uint16_t g_LanguageNameEspanol[] = {PROJECT_TEXT_BRIGHT,'E','s','p','a',0x00F1,'o','l',0};
static uint16_t *const g_LanguageNames[TEXT_ID_LANGUAGE_NAME_COUNT] = {
    g_LanguageNameDeutsch, g_LanguageNameEnglish, g_LanguageNameFrancais, g_LanguageNameItaliano, g_LanguageNameEspanol};
/* the telephone country codes of g_LanguageNames */
static const LocaleTelephoneCountryCode g_LanguageNameCountryCodes[TEXT_ID_LANGUAGE_NAME_COUNT] = {
    LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY, LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN,
    LocaleTelephoneCountryCode::LOCALE_COUNTRY_FRANCE, LocaleTelephoneCountryCode::LOCALE_COUNTRY_ITALY,
    LocaleTelephoneCountryCode::LOCALE_COUNTRY_SPAIN};

/* Not in the original: the pathing checkbox of the advanced settings page (TEXT_ID_ADVANCED_PATHING), bright like
   the VSync checkbox. */
static uint16_t g_ProjectTextVerbesserteWegfindung[] = {
    PROJECT_TEXT_BRIGHT,'V','e','r','b','e','s','s','e','r','t','e',' ','W','e','g','f','i','n','d','u','n','g',0};
static uint16_t g_ProjectTextImprovedPathfinding[] = {
    PROJECT_TEXT_BRIGHT,'I','m','p','r','o','v','e','d',' ','p','a','t','h','f','i','n','d','i','n','g',0};

TextResourceId TextResource_LanguageNameId(uint32_t countryCode)
{
  for (int index = 0; index < TEXT_ID_LANGUAGE_NAME_COUNT; index++) {
    if (static_cast<uint32_t>(g_LanguageNameCountryCodes[index]) == countryCode) {
      return TEXT_ID_LANGUAGE_NAME_BASE + index;
    }
  }
  return TEXT_RESOURCE_ID_NONE;
}

TextResourceId TextResource_SetLanguageButtonLabel(const uint16_t *text)
{
  size_t length = 0;

  if (text != nullptr) {
    for (; (text[length] != 0) && (length + 1 < TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS); length++) {
      g_ProjectTextLanguageButtonLabel[length] = text[length];
    }
  }
  g_ProjectTextLanguageButtonLabel[length] = 0;
  return TEXT_ID_LANGUAGE_BUTTON_LABEL;
}

void TextResource_SetUiScaleNote(const char *text)
{
  size_t index = 0;

  for (; (text[index] != '\0') && (index + 1 < sizeof(g_ProjectTextNoteUiScale) / sizeof(uint16_t)); index++) {
    g_ProjectTextNoteUiScale[index] = static_cast<unsigned char>(text[index]);
  }
  g_ProjectTextNoteUiScale[index] = 0;
}

TextResourceId TextResource_SetCampaignMapLine(TextResourceId lineId,TextResourceId templateId,uint32_t missionNumber,
                                               uint16_t *missionTitle)
{
  uint16_t *line;
  size_t capacity;
  size_t length = 0;
  char title[96];
  char number[12];
  const char *titleText = title;
  const char *insert;
  const uint16_t *source;

  if (lineId == TEXT_ID_CAMPAIGN_MAP_HEADING_LINE) {
    line = g_ProjectTextCampaignHeadingLine;
    capacity = sizeof(g_ProjectTextCampaignHeadingLine) / sizeof(uint16_t);
  }
  else if (lineId == TEXT_ID_CAMPAIGN_MAP_STATUS_LINE) {
    line = g_ProjectTextCampaignStatusLine;
    capacity = sizeof(g_ProjectTextCampaignStatusLine) / sizeof(uint16_t);
  }
  else {
    return lineId;
  }
  title[0] = '\0';
  if (missionTitle != nullptr) {
    RichTextCommandStream_CopyToNarrow(sizeof(title),reinterpret_cast<uint8_t *>(title),missionTitle); /* cut on overflow */
  }
  while (*titleText == ' ') {
    titleText++;
  }
  snprintf(number,sizeof(number),"%u",missionNumber);
  for (source = TextResource_Resolve(templateId); (*source != 0) && (length + 1 < capacity); source++) {
    if ((source[0] == '%') && ((source[1] == 'u') || (source[1] == 's'))) {
      for (insert = (source[1] == 'u') ? number : titleText; (*insert != '\0') && (length + 1 < capacity); insert++) {
        line[length++] = static_cast<unsigned char>(*insert);
      }
      source++;
      continue;
    }
    line[length++] = *source;
  }
  line[length] = 0;
  return lineId;
}

TextResourceId TextResource_SetCampaignButtonLabel(int buttonIndex,const uint16_t *text)
{
  size_t length = 0;

  if ((buttonIndex < 0) || (buttonIndex >= TEXT_ID_CAMPAIGN_BUTTON_LABEL_COUNT)) {
    return TEXT_RESOURCE_ID_NONE;
  }
  uint16_t *label = g_ProjectTextCampaignButtonLabels[buttonIndex];
  if (text != nullptr) {
    for (; (text[length] != 0) && (length + 1 < TEXT_ID_CAMPAIGN_BUTTON_LABEL_UNITS); length++) {
      label[length] = text[length];
    }
  }
  label[length] = 0;
  return TEXT_ID_CAMPAIGN_BUTTON_LABEL_0 + buttonIndex;
}

/* the German campaign title page is bound (its locale block is the one TextResourcePage_Load chose) */
static bool TextResource_CampaignTitlesAreGerman()
{
  const TextResourceLocaleBlockPrefix *localeBlock =
      g_TextResourcePageBindings[(TEXT_ID_CAMPAIGN_TITLE_BASE >> 8) & 0xff].selectedLocaleBlock;

  return (localeBlock != nullptr) && (localeBlock->countryCode == LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);
}

TextResourceId TextResource_CampaignShortName(uint16_t *cgnBaseName)
{
  static const struct {
    uint16_t *fileName;
    TextResourceId textId;
  } shortNames[] = {
      {g_CampaignFileNameTutorial, TEXT_ID_CAMPAIGN_SHORT_NAME_TUTORIAL},
      {g_CampaignFileNameHansolo, TEXT_ID_CAMPAIGN_SHORT_NAME_HANSOLO},
      {g_CampaignFileNameNimm2, TEXT_ID_CAMPAIGN_SHORT_NAME_NIMM2},
      {g_CampaignFileNameLuke, TEXT_ID_CAMPAIGN_SHORT_NAME_LUKE}};

  if ((cgnBaseName == nullptr) || !TextResource_CampaignTitlesAreGerman()) {
    return TEXT_RESOURCE_ID_NONE;
  }
  for (const auto &shortName : shortNames) {
    if (Utf16String_CompareAsciiCaseInsensitiveFlags(shortName.fileName,cgnBaseName) == 0) {
      return shortName.textId;
    }
  }
  return TEXT_RESOURCE_ID_NONE;
}

/* Loads the level's own text page (the .str entry of a level package) as page 0x30 and makes its title,
   description and 14 further description lines reachable under the global ids the frontend uses for that
   level: TEXT_ID_LEVEL_TITLE_BASE + title index and TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE *
   title index (+1..14).
   Returns true when the page cannot be loaded or one of the strings is missing, false on success.
*/
bool TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path)

{
  uint16_t *resolvedText;
  int lineIndex;
  bool failed;

  failed = !TextResourcePage_Load(TEXT_RESOURCE_PAGE_LEVEL,path,nullptr);
  if (!failed) {
    failed = !TextResource_TryResolve(TEXT_ID_LEVEL_PAGE_TITLE,&resolvedText);
    if (!failed) {
      TextResourceOverride_Register(levelTitleIndex + TEXT_ID_LEVEL_TITLE_BASE,resolvedText);
      failed = !TextResource_TryResolve(TEXT_ID_LEVEL_PAGE_DESCRIPTION,&resolvedText);
      if (!failed) {
        lineIndex = TEXT_LEVEL_EXTRA_LINE_COUNT - 1;
        TextResourceOverride_Register(levelTitleIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE + TEXT_ID_LEVEL_DESCRIPTION_BASE,
                                      resolvedText);
        /* the extra lines are registered from the last one down */
        for (; -1 < lineIndex; lineIndex--) {
          if (!TextResource_TryResolve(lineIndex + TEXT_ID_LEVEL_PAGE_EXTRA_LINES,&resolvedText)) {
            return true;
          }
          TextResourceOverride_Register
                    (levelTitleIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE + (TEXT_ID_LEVEL_DESCRIPTION_BASE + 1) + lineIndex,
                     resolvedText);
        }
        failed = false;
      }
    }
  }
  return failed;
}

/* Returns the first locale block of a 'str' asset whose country code is countryCode, or NULL. The blocks follow
   the 0x200-byte asset header; block + blockSizeBytes is the next block.
   The first block is always checked and the count is only tested after stepping; the count is >= 1 because
   TextResourceAsset_HasValidBlocks rejects a block count of 0 before. */
static TextResourceLocaleBlockPrefix *TextResourceAsset_FindLocaleBlock
          (TextResourceAssetHeader *asset,LocaleTelephoneCountryCode countryCode)
{
  TextResourceLocaleBlockPrefix *block;
  AssetRecordCount remainingBlocks;

  remainingBlocks = (asset->localeCountHeader).localeBlockCount;
  block = Asset_RecordAfter<TextResourceLocaleBlockPrefix>(asset);
  /* a do-while: remainingBlocks >= 1 because TextResourceAsset_HasValidBlocks rejects a count of 0 before */
  do {
    if (block->countryCode == countryCode) {
      return block;
    }
    block = Asset_RecordAt<TextResourceLocaleBlockPrefix>(block,block->blockSizeBytes);
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return nullptr;
}

/* Not in the original: true when a loaded 'str' asset of byteCount bytes has its header, at least one locale
   block and every block prefix TextResourceAsset_FindLocaleBlock reads inside the asset. The original trusts
   the asset; checked here because level packages (user maps) carry their own .str entry. */
static bool TextResourceAsset_HasValidBlocks(const TextResourceAssetHeader *asset,uint32_t byteCount)
{
  uint64_t blockOffset;
  AssetRecordCount remainingBlocks;

  if (byteCount < sizeof(TextResourceAssetHeader)) {
    return false;
  }
  remainingBlocks = (asset->localeCountHeader).localeBlockCount;
  if (remainingBlocks == 0) {
    return false;
  }
  blockOffset = sizeof(TextResourceAssetHeader);
  for (; remainingBlocks != 0; remainingBlocks--) {
    if (blockOffset + sizeof(TextResourceLocaleBlockPrefix) > byteCount) {
      return false;
    }
    blockOffset += Asset_RecordAt<TextResourceLocaleBlockPrefix>(asset,blockOffset)->blockSizeBytes;
  }
  return true;
}

/* Not in the original: true when the selected block's string offset table and every string start lie inside
   the asset (assetEnd is one past its last whole code unit), with room for at least one code unit. */
static bool TextResourceLocaleBlock_HasValidStrings(const TextResourceLocaleBlockPrefix *localeBlock,
                                                    const uint16_t *assetEnd)
{
  uint64_t blockBytes;
  const uint32_t *stringOffsets;
  uint32_t stringIndex;

  blockBytes = (uint64_t)Asset_ByteDistance(assetEnd,localeBlock);
  if ((blockBytes < sizeof(TextResourceLocaleBlockPrefix)) ||
      ((uint64_t)localeBlock->stringCount * sizeof(uint32_t) > blockBytes - sizeof(TextResourceLocaleBlockPrefix))) {
    return false;
  }
  stringOffsets = Asset_RecordAfter<uint32_t>(localeBlock);
  for (stringIndex = 0; stringIndex < localeBlock->stringCount; stringIndex++) {
    if ((uint64_t)stringOffsets[stringIndex] + sizeof(uint16_t) > blockBytes) {
      return false;
    }
  }
  return true;
}

/* Not in the original: logs and releases a malformed 'str' asset for TextResourcePage_Load and returns false
   with TEXT_RESOURCE_MISSING_SENTINEL_0x33 in *outLocaleBlockOrError (as for a non-'str' asset). The page
   binding is left unchanged. */
static bool TextResourcePage_RejectAsset(TextResourcePageIndex pageIndex,uint16_t *path,
                                          TextResourceAssetHeader *allocation,const char *reason,
                                          uintptr_t *outLocaleBlockOrError)
{
  Thandor_Log("text page 0x%02X \"%ls\": malformed asset rejected (%s)", pageIndex, reinterpret_cast<wchar_t *>(path), reason); /* UTF-16 for %ls */
  Resource_Release(allocation);
  if (outLocaleBlockOrError != nullptr) {
    *outLocaleBlockOrError = TEXT_RESOURCE_MISSING_SENTINEL_0x33;
  }
  return false;
}

/* Returns the number held by a command record's four UTF-16 decimal digits d0..d3 (recordStart[1..4], the two
   payload dwords): d0*1000 + d1*100 + d2*10 + d3. */
static uint32_t RichTextRecord_ParseDecimalDigits(const uint16_t *recordStart)
{
  uint32_t highDigits;
  uint32_t lowDigits;

  highDigits = Thandor_LoadU32(recordStart + 1); /* the payload dwords are only 2-byte aligned */
  lowDigits = Thandor_LoadU32(recordStart + 3);
  return ((lowDigits >> 16 & 0xf) + (lowDigits & 0xf) * 10) + (highDigits >> 16 & 0xf) * 100 +
         (highDigits & 0xf) * 1000;
}

/* Not in the original: the prepared (usable) locale blocks of each loaded page in asset order, so that
   TextResource_SelectLanguage can rebind a page without reading its asset again. Filled by
   TextResourcePage_BindAsset together with the page binding. */
inline constexpr int TEXT_RESOURCE_PAGE_LOCALE_BLOCKS_MAX = 8;
struct TextResourcePageLocaleBlocks {
  TextResourceLocaleBlockPrefix *blocks[TEXT_RESOURCE_PAGE_LOCALE_BLOCKS_MAX];
  LocaleTelephoneCountryCode countryCodes[TEXT_RESOURCE_PAGE_LOCALE_BLOCKS_MAX];
  int count;
};
static TextResourcePageLocaleBlocks s_TextResourcePageLocaleBlocks[256];

/* the language of TextResource_SelectLanguage; until its first call the thandor.ini setting applies */
static bool s_TextResourceLanguageSelected = false;
static uint32_t s_TextResourceLanguage = TEXT_RESOURCE_LANGUAGE_AUTO;

uint32_t TextResource_SelectedLanguage()
{
  return s_TextResourceLanguageSelected ? s_TextResourceLanguage : g_LocaleCountryCodeOverride;
}

/* The country code a page is bound to: the selected one, for auto the Windows default. */
static LocaleTelephoneCountryCode TextResource_RequestedCountryCode()
{
  const uint32_t selected = TextResource_SelectedLanguage();

  if (selected != TEXT_RESOURCE_LANGUAGE_AUTO) {
    return static_cast<LocaleTelephoneCountryCode>(selected);
  }
  /* the self-tests run before Locale_Init set the function pointer */
  return (g_LocaleGetDefaultTelephoneCountryCode != nullptr) ? g_LocaleGetDefaultTelephoneCountryCode()
                                                             : Locale_GetDefaultTelephoneCountryCode();
}

/* Prepares every string of a locale block for run time: the nested-stream and image records' decimal digits become
   a binary number and the pointer slot (see the switch). walkEnd bounds the walk (one past the last code unit a
   string may use). Returns nullptr on success, else the reason the block is malformed (the block may then be
   partly prepared). On success *outReached is one past the last code unit read (string table and strings). */
static const char *TextResourceLocaleBlock_Prepare(TextResourceLocaleBlockPrefix *localeBlock,uint16_t *walkEnd,
                                                   uint16_t **outReached)
{
  uint16_t codeUnit;
  uint32_t decimalValue;
  uint32_t *stringOffsets;
  TextResourceStringCount remainingStrings;
  uint16_t *recordStart;
  uint16_t *textCursor;
  int stringIndex;
  uint16_t *reached;

  if (!TextResourceLocaleBlock_HasValidStrings(localeBlock,walkEnd)) {
    return "string table";
  }
  /* the string offsets (relative to the block) follow the 16-byte block prefix */
  stringOffsets = Asset_RecordAfter<uint32_t>(localeBlock);
  reached = reinterpret_cast<uint16_t *>(stringOffsets + localeBlock->stringCount);
  stringIndex = 0;
  for (remainingStrings = localeBlock->stringCount; remainingStrings != 0; remainingStrings--) {
    textCursor = Asset_RecordAt<uint16_t>(localeBlock,stringOffsets[stringIndex]);
    /* The original walks to the terminator; bounded here because a string without one, or a record cut off by
       the asset end, would be read and rewritten past the asset. */
    for (;;) {
      if (textCursor >= walkEnd) {
        return "unterminated string";
      }
      if (*textCursor == 0) {
        break;
      }
      recordStart = textCursor;
      codeUnit = *recordStart;
      textCursor = recordStart + 1;
      if (((short)codeUnit < 0) && (walkEnd - recordStart < RICHTEXT_RECORD_UNITS_NESTED)) {
        /* the nested-stream and image records read and rewrite their whole payload */
        switch(codeUnit & RICHTEXT_OPCODE_MASK) {
        case RICHTEXT_OP_CALL_NESTED:
        case RICHTEXT_OP_JUMP_NESTED:
        case RICHTEXT_OP_INLINE_IMAGE:
          return "truncated record";
        }
      }
      if ((short)codeUnit < 0) {
        /* The converter stores the nested-stream selector and the image subresource as four UTF-16 decimal
           digits d0..d3 filling both payload dwords. They become a binary number (d0*1000 + d1*100 + d2*10
           + d3) in the second dword, and the first dword becomes the pointer slot: the missing-text stream
           for nested streams, NULL for images (bound later by RichTextCommandStream_BindTextureSource). The
           digits are parsed before the pointer slot overwrites the high ones. */
        switch(codeUnit & RICHTEXT_OPCODE_MASK) {
        case RICHTEXT_OP_LITERAL_COLOR:
          textCursor = recordStart + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
          break;
        case RICHTEXT_OP_INLINE_VALUE_0:
        case RICHTEXT_OP_INLINE_VALUE_1:
        case RICHTEXT_OP_INLINE_VALUE_2:
          textCursor = recordStart + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
          break;
        case RICHTEXT_OP_CALL_NESTED:
        case RICHTEXT_OP_JUMP_NESTED:
          decimalValue = RichTextRecord_ParseDecimalDigits(recordStart);
          THANDOR_PTR32_AT(void, recordStart + 1) = g_MissingTextResourceFallbackStream;
          Thandor_StoreU32(recordStart + 3,decimalValue);
          textCursor = recordStart + RICHTEXT_RECORD_UNITS_NESTED;
          break;
        case RICHTEXT_OP_INLINE_IMAGE:
          decimalValue = RichTextRecord_ParseDecimalDigits(recordStart);
          Thandor_StoreU32(recordStart + 1,0);
          Thandor_StoreU32(recordStart + 3,decimalValue);
          textCursor = recordStart + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
          break;
        }
      }
    }
    if (textCursor + 1 > reached) {
      reached = textCursor + 1; /* the terminator included */
    }
    stringIndex++;
  }
  *outReached = reached;
  return nullptr;
}

/* One past the last whole code unit of a locale block (its blockSizeBytes), cut to the asset end. */
static uint16_t *TextResourceLocaleBlock_End(TextResourceLocaleBlockPrefix *localeBlock,uint16_t *assetEnd)
{
  const uint64_t available = (uint64_t)Asset_ByteDistance(assetEnd,localeBlock);
  const uint64_t blockBytes = (localeBlock->blockSizeBytes < available) ? localeBlock->blockSizeBytes : available;

  return Asset_RecordAt<uint16_t>(localeBlock,(uint32_t)(blockBytes & ~(uint64_t)1));
}

/* The page's usable block for countryCode, else its Great Britain block, else its first usable block (nullptr for a
   page without usable blocks): the rule TextResourcePage_BindAsset applies to the asset's blocks. */
static TextResourceLocaleBlockPrefix *TextResourcePage_ChooseLocaleBlock(const TextResourcePageLocaleBlocks *page,
                                                                         LocaleTelephoneCountryCode countryCode)
{
  int index;

  if (page->count == 0) {
    return nullptr;
  }
  for (index = 0; index < page->count; index++) {
    if (page->countryCodes[index] == countryCode) {
      return page->blocks[index];
    }
  }
  for (index = 0; index < page->count; index++) {
    if (page->countryCodes[index] == LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN) {
      return page->blocks[index];
    }
  }
  return page->blocks[0];
}

/* Loads a 'str' text asset as page pageIndex and binds it (TextResourcePage_BindAsset): picks the locale block of
   the configured (or system) country, else the Great Britain block, else the first one, binds it, and prepares
   every string's command records for run time (TextResourceLocaleBlock_Prepare). Returns true on success and
   stores the selected block's address in *outLocaleBlockOrError; returns false and stores the package error there,
   or TEXT_RESOURCE_MISSING_SENTINEL_0x33 for a non-'str' or malformed asset (which is released; the page binding
   stays unchanged). outLocaleBlockOrError may be NULL.
   Not in the original: the other locale blocks are prepared as well, so TextResource_SelectLanguage can switch to
   them.
*/
bool TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path,uintptr_t *outLocaleBlockOrError)

{
  TextResourceAssetHeader *allocation;
  uint32_t loadErrorCode;
  uint32_t byteCount;

  allocation = static_cast<TextResourceAssetHeader *>(Package_LoadEntryWithSize(path,&byteCount,&loadErrorCode));
  if (allocation == nullptr) {
    Thandor_Log("text page 0x%02X \"%ls\": load failed 0x%08X", pageIndex, reinterpret_cast<wchar_t *>(path), /* UTF-16 for %ls */
                loadErrorCode);
    if (outLocaleBlockOrError != nullptr) {
      *outLocaleBlockOrError = loadErrorCode;
    }
    return false;
  }
  return TextResourcePage_BindAsset(pageIndex,path,allocation,byteCount,outLocaleBlockOrError);
}

bool TextResourcePage_BindAsset(TextResourcePageIndex pageIndex,uint16_t *path,TextResourceAssetHeader *allocation,
                                uint32_t byteCount,uintptr_t *outLocaleBlockOrError)
{
  TextResourceLocaleBlockPrefix *localeBlock;
  TextResourceLocaleBlockPrefix *block;
  LocaleTelephoneCountryCode countryCode;
  AssetRecordCount remainingBlocks;
  TextResourcePageLocaleBlocks usable{};
  uint16_t *assetEnd;
  uint16_t *reached = nullptr;
  const char *reason;
  bool selectedContained;
  bool selectedListed;

  /* not in the original: an asset shorter than the header is treated as a non-'str' asset */
  if ((byteCount < sizeof(TextResourceAssetHeader)) ||
      ((allocation->localeCountHeader).common.magic != ASSET_MAGIC_STR)) {
    Resource_Release(allocation);
    if (outLocaleBlockOrError != nullptr) {
      *outLocaleBlockOrError = TEXT_RESOURCE_MISSING_SENTINEL_0x33;
    }
    return false;
  }
  if (!TextResourceAsset_HasValidBlocks(allocation,byteCount)) {
    return TextResourcePage_RejectAsset(pageIndex,path,allocation,"locale blocks",outLocaleBlockOrError);
  }
  countryCode = TextResource_RequestedCountryCode();
  /* the block for the country code, else the Great Britain block, else the first block */
  localeBlock = TextResourceAsset_FindLocaleBlock(allocation,countryCode);
  if (localeBlock == nullptr) {
    localeBlock = TextResourceAsset_FindLocaleBlock(allocation,LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN);
    if (localeBlock == nullptr) {
      localeBlock = Asset_RecordAfter<TextResourceLocaleBlockPrefix>(allocation);
    }
  }
  /* The original binds the page here, before the walk; bound after the walk here so that a rejected asset leaves
     the binding unchanged (nothing reads it during the walk). */
  assetEnd = Asset_RecordAt<uint16_t>(allocation,byteCount & ~1u);
  reason = TextResourceLocaleBlock_Prepare(localeBlock,assetEnd,&reached);
  if (reason != nullptr) {
    return TextResourcePage_RejectAsset(pageIndex,path,allocation,reason,outLocaleBlockOrError);
  }
  /* Not in the original: the other blocks are prepared too, each walked within its own extent (blockSizeBytes),
     and listed when that succeeds; a malformed one is skipped (logged) and the asset kept. The extents of
     consecutive blocks are disjoint, so no string is prepared twice; only when the selected block's strings
     reach beyond its own extent (which the original allows) the others stay unprepared and unlisted. */
  selectedContained = (reached <= TextResourceLocaleBlock_End(localeBlock,assetEnd));
  selectedListed = false;
  block = Asset_RecordAfter<TextResourceLocaleBlockPrefix>(allocation);
  for (remainingBlocks = (allocation->localeCountHeader).localeBlockCount;
       (remainingBlocks != 0) && (usable.count < TEXT_RESOURCE_PAGE_LOCALE_BLOCKS_MAX); remainingBlocks--) {
    if (block == localeBlock) {
      usable.blocks[usable.count] = block;
      usable.countryCodes[usable.count++] = block->countryCode;
      selectedListed = true;
    }
    else if (selectedContained) {
      reason = TextResourceLocaleBlock_Prepare(block,TextResourceLocaleBlock_End(block,assetEnd),&reached);
      if (reason == nullptr) {
        usable.blocks[usable.count] = block;
        usable.countryCodes[usable.count++] = block->countryCode;
      }
      else {
        Thandor_Log("text page 0x%02X \"%ls\": locale block %d skipped (%s)", pageIndex,
                    reinterpret_cast<wchar_t *>(path), static_cast<int>(block->countryCode), reason); /* UTF-16 for %ls */
      }
    }
    /* TextResourceAsset_HasValidBlocks checked every block prefix */
    if (remainingBlocks > 1) {
      block = Asset_RecordAt<TextResourceLocaleBlockPrefix>(block,block->blockSizeBytes);
    }
  }
  if (!selectedListed) {
    /* the selected block lies beyond the first TEXT_RESOURCE_PAGE_LOCALE_BLOCKS_MAX: it alone is listed */
    usable = TextResourcePageLocaleBlocks{};
    usable.blocks[0] = localeBlock;
    usable.countryCodes[0] = localeBlock->countryCode;
    usable.count = 1;
  }
  g_TextResourcePageBindings[pageIndex].selectedLocaleBlock = localeBlock;
  g_TextResourcePageBindings[pageIndex].asset = allocation;
  s_TextResourcePageLocaleBlocks[pageIndex] = usable;
  if (outLocaleBlockOrError != nullptr) {
    *outLocaleBlockOrError = (uintptr_t)localeBlock;
  }
  return true;
}

void TextResource_SelectLanguage(uint32_t countryCode)
{
  LocaleTelephoneCountryCode requested;
  TextResourceLocaleBlockPrefix *localeBlock;
  size_t pageIndex;

  s_TextResourceLanguage = countryCode;
  s_TextResourceLanguageSelected = true;
  requested = TextResource_RequestedCountryCode();
  for (pageIndex = 0; pageIndex < sizeof(g_TextResourcePageBindings) / sizeof(g_TextResourcePageBindings[0]);
       pageIndex++) {
    if (g_TextResourcePageBindings[pageIndex].asset == nullptr) {
      continue;
    }
    localeBlock = TextResourcePage_ChooseLocaleBlock(&s_TextResourcePageLocaleBlocks[pageIndex],requested);
    if (localeBlock != nullptr) {
      g_TextResourcePageBindings[pageIndex].selectedLocaleBlock = localeBlock;
    }
  }
}

uint32_t TextResource_ActiveLanguage()
{
  const TextResourceLocaleBlockPrefix *localeBlock = g_TextResourcePageBindings[GAME_TEXT_PAGE_MENUE].selectedLocaleBlock;

  if (localeBlock != nullptr) {
    return static_cast<uint32_t>(localeBlock->countryCode);
  }
  return static_cast<uint32_t>(TextResource_RequestedCountryCode());
}

int TextResource_AvailableLanguages(uint32_t *outCodes,int capacity)
{
  static const LocaleTelephoneCountryCode order[TEXT_RESOURCE_LANGUAGE_MAX] = {
      LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY, LocaleTelephoneCountryCode::LOCALE_COUNTRY_GREAT_BRITAIN,
      LocaleTelephoneCountryCode::LOCALE_COUNTRY_FRANCE, LocaleTelephoneCountryCode::LOCALE_COUNTRY_ITALY,
      LocaleTelephoneCountryCode::LOCALE_COUNTRY_SPAIN};
  const TextResourcePageLocaleBlocks *page = &s_TextResourcePageLocaleBlocks[GAME_TEXT_PAGE_MENUE];
  int count = 0;
  int index;

  for (LocaleTelephoneCountryCode countryCode : order) {
    for (index = 0; index < page->count; index++) {
      if (page->countryCodes[index] == countryCode) {
        if (count < capacity) {
          outCodes[count++] = static_cast<uint32_t>(countryCode);
        }
        break;
      }
    }
  }
  return count;
}

bool TextResource_ProjectTextsAreGerman()
{
  return TextResource_ActiveLanguage() == static_cast<uint32_t>(LocaleTelephoneCountryCode::LOCALE_COUNTRY_GERMANY);
}

/* Makes resourceId resolve to text (checked by TextResource_Resolve before the locale blocks) by storing the
   pair in the first override entry whose id is zero. Without an override table, or when it is full, nothing
   is registered.
*/
void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text)

{
  uint32_t overrideIndex;

  if (g_TextResourceOverrides == nullptr) {
    return;
  }
  /* the first entry with a zero id; its text pointer is the entry of the same index in textPointers (the
     original scans the id array as dwords and writes the pointer TEXT_RESOURCE_OVERRIDE_CAPACITY dwords
     further on) */
  for (overrideIndex = 0; overrideIndex < TEXT_RESOURCE_OVERRIDE_CAPACITY; overrideIndex++) {
    if (g_TextResourceOverrides->resourceIds[overrideIndex] == 0) {
      g_TextResourceOverrides->resourceIds[overrideIndex] = resourceId;
      g_TextResourceOverrides->textPointers[overrideIndex] = text;
      return;
    }
  }
}

/* Not in the original: the texts of TEXT_ID_KEY_BINDING_BASE.. (the key binding list, ui/core/key_bindings.h) in
   German and English, in TEXT_ID_KEYS_* order: X(name, German, English). Umlauts are Latin-1 code units, as in the
   campaign map texts above (the game fonts have them). */
#define THANDOR_KEY_BINDING_TEXTS(X) \
    X(TITLE, u"Tastenbelegung", u"Key bindings") \
    X(SECTION_GENERAL, u"Allgemein im Spiel", u"In the game") \
    X(SECTION_UNITS, u"Einheiten und Gruppen", u"Units and groups") \
    X(SECTION_CAMERA, u"Kamera", u"Camera") \
    X(SECTION_MENUS, u"Men\u00FCs und Filme", u"Menus and movies") \
    X(GAME_MENU, u"Spielmen\u00FC \u00F6ffnen", u"Open the game menu") \
    X(OBJECTIVES, u"Missionsziele ein/aus", u"Mission objectives on/off") \
    X(SAVE, u"Spiel speichern (nur Einzelspieler)", u"Save the game (single player only)") \
    X(QUIT_MENU, u"Beenden-Men\u00FC \u00F6ffnen", u"Open the quit menu") \
    X(LEAVE_GAME, u"Spiel verlassen (nicht als Spielleiter)", u"Leave the game (not as host)") \
    X(PAUSE, u"Pause ein/aus", u"Pause on/off") \
    X(FASTER, u"Spielgeschwindigkeit erh\u00F6hen", u"Increase the game speed") \
    X(SLOWER, u"Spielgeschwindigkeit verringern", u"Decrease the game speed") \
    X(SIDE_PANEL, u"Seitenleiste aus-/einblenden", u"Hide/show the side panel") \
    X(CHAT_LINE, u"Nachrichtenzeile \u00F6ffnen", u"Open the message line") \
    X(MESSAGE_WINDOW, u"Nachrichtenfenster ein/aus (nur Netzwerkspiel)", u"Message window on/off (network games only)") \
    X(SCORE_OVERVIEW, u"Punkte\u00FCbersicht der Parteien ein/aus", u"Faction score overview on/off") \
    X(INFO_LINE, u"Infozeile weiterschalten (Bildrate, Kamera, Speicher)", u"Cycle the info line (frame rate, camera, memory)") \
    X(SCREENSHOT, u"Bildschirmfoto speichern", u"Save a screenshot") \
    X(NOTIFICATION_GO, u"Zur aktuellen Meldung springen", u"Go to the current message") \
    X(NOTIFICATION_BACK, u"Von der Meldung zur\u00FCckkehren", u"Return from the message") \
    X(GROUP_RECALL, u"Gruppe ausw\u00E4hlen", u"Select a group") \
    X(GROUP_ADD_TO_SELECTION, u"Gruppe zur Auswahl hinzuf\u00FCgen", u"Add a group to the selection") \
    X(GROUP_STORE, u"Auswahl als Gruppe speichern", u"Store the selection as a group") \
    X(GROUP_ADD_SELECTION, u"Auswahl zur Gruppe hinzuf\u00FCgen", u"Add the selection to a group") \
    X(STOP, u"Anhalten", u"Stop") \
    X(STOP_MOVEMENT, u"Nur Bewegung anhalten", u"Stop moving only") \
    X(CANCEL_ATTACK, u"Angriff abbrechen", u"Cancel the attack") \
    X(SELF_DESTRUCT, u"Auswahl selbst zerst\u00F6ren", u"Self-destruct the selection") \
    X(SELECT_AIRPORTS, u"Alle eigenen Flugh\u00E4fen ausw\u00E4hlen", u"Select all own airports") \
    X(UPGRADE_PAGE, u"Ausbauseite der Einheit ein/aus", u"Unit upgrade page on/off") \
    X(CAMERA_SCROLL, u"Kamera verschieben", u"Scroll the camera") \
    X(CAMERA_TILT, u"Kamera neigen", u"Tilt the camera") \
    X(CAMERA_ROTATE, u"Kamera drehen", u"Rotate the camera") \
    X(CAMERA_ZOOM, u"Heranzoomen / Wegzoomen", u"Zoom in / out") \
    X(CAMERA_SELECTION, u"Kamera auf die Auswahl", u"Camera to the selection") \
    X(CAMERA_HQ, u"Kamera zum Hauptquartier", u"Camera to the headquarters") \
    X(CAMERA_NOTIFICATION, u"Kamera zum Ort der letzten Meldung", u"Camera to the last message's location") \
    X(CAMERA_FREE, u"Freie Kamera ein/aus (ohne Neigungs- und Zoomgrenzen)", u"Free camera on/off (no tilt and zoom limits)") \
    X(MENU_LEAVE, u"Zur\u00FCck zum Hauptmen\u00FC (Kampagne oder Netzwerkspiel verlassen)", u"Back to the main menu (leave the campaign or network game)") \
    X(UI_NEXT_CONTROL, u"Zum n\u00E4chsten Bedienelement", u"Move to the next control") \
    X(UI_PRESS_BUTTON, u"Ausgew\u00E4hlte Schaltfl\u00E4che dr\u00FCcken", u"Press the focused button") \
    X(UI_CONFIRM_CANCEL, u"Dialog best\u00E4tigen / abbrechen", u"Confirm / cancel a dialog") \
    X(UI_LIST, u"In Listen bl\u00E4ttern", u"Move through lists") \
    X(UI_SLIDER, u"Schieberegler verstellen (mit Strg bis zum Anschlag)", u"Adjust a slider (with Ctrl to its end)") \
    X(INTRO_SKIP_ALL, u"Alle Intro-Filme \u00FCberspringen", u"Skip all intro movies") \
    X(INTRO_SKIP, u"Intro-Film \u00FCberspringen (auch per Mausklick)", u"Skip the intro movie (also a mouse click)") \
    X(END_MOVIE_SKIP, u"Endfilm \u00FCberspringen", u"Skip the end movie") \
    X(END_MOVIE_SCREENSHOT, u"Bildschirmfoto im Endfilm", u"Screenshot during the end movie")

/* a mutable UTF-16 copy of a u"" literal (TextResource_TryResolve hands out uint16_t *) */
template <size_t N>
struct KeyBindingTextUtf16 {
    uint16_t units[N];
    constexpr KeyBindingTextUtf16(const char16_t (&text)[N]) : units{}
    {
        for (size_t i = 0; i < N; i++) {
            units[i] = static_cast<uint16_t>(text[i]);
        }
    }
};
enum KeyBindingTextIndex : int {
#define THANDOR_KEY_BINDING_TEXT_INDEX(name, german, english) KEY_BINDING_TEXT_INDEX_##name,
    THANDOR_KEY_BINDING_TEXTS(THANDOR_KEY_BINDING_TEXT_INDEX)
    KEY_BINDING_TEXT_INDEX_COUNT
};
#define THANDOR_KEY_BINDING_TEXT_DEFINE(name, german, english) \
    static KeyBindingTextUtf16 g_KeyBindingTextGerman_##name{german}; \
    static KeyBindingTextUtf16 g_KeyBindingTextEnglish_##name{english}; \
    static_assert(TEXT_ID_KEYS_##name - TEXT_ID_KEY_BINDING_BASE == KEY_BINDING_TEXT_INDEX_##name, \
                  "THANDOR_KEY_BINDING_TEXTS is in TEXT_ID_KEYS_* order");
THANDOR_KEY_BINDING_TEXTS(THANDOR_KEY_BINDING_TEXT_DEFINE)
#define THANDOR_KEY_BINDING_TEXT_GERMAN(name, german, english) g_KeyBindingTextGerman_##name.units,
#define THANDOR_KEY_BINDING_TEXT_ENGLISH(name, german, english) g_KeyBindingTextEnglish_##name.units,
static uint16_t *const g_KeyBindingTextsGerman[KEY_BINDING_TEXT_INDEX_COUNT] = {
    THANDOR_KEY_BINDING_TEXTS(THANDOR_KEY_BINDING_TEXT_GERMAN)};
static uint16_t *const g_KeyBindingTextsEnglish[KEY_BINDING_TEXT_INDEX_COUNT] = {
    THANDOR_KEY_BINDING_TEXTS(THANDOR_KEY_BINDING_TEXT_ENGLISH)};
static_assert(KEY_BINDING_TEXT_INDEX_COUNT == TEXT_ID_KEY_BINDING_COUNT, "one text per TEXT_ID_KEYS_* id");
static bool g_KeyBindingTextsEnglishSelected = false;

void TextResource_SetKeyBindingTextsEnglish(bool english)
{
  g_KeyBindingTextsEnglishSelected = english;
}

/* Looks up the text of a resource id: TEXT_RESOURCE_ID_NONE gives the shared empty string, then the runtime
   override table is searched, then the bound locale block of the id's page (compact or extended id, see
   resources.h). Stores the text in *outText and returns true when found; a missing text stores
   TEXT_RESOURCE_MISSING_SENTINEL_0x33 (the pointer value 0x33, not a real string) there and returns false.
*/
bool TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText)

{
  static bool loggedMissingText; /* open-thandor diagnostics: the first missing id only */
  TextResourceLocaleBlockPrefix *localeBlock;
  uint32_t overrideIndex;
  uint32_t pageIndex;

  if (resourceId == TEXT_RESOURCE_ID_NONE) {
    *outText = g_EmptyTextResourceUtf16;
    return true;
  }
  if ((resourceId >= TEXT_ID_PROJECT_BASE) && (resourceId < TEXT_ID_PROJECT_BASE + TEXT_ID_PROJECT_COUNT)) {
    /* not in the original: English for every language but German where there is an English text */
    *outText = TextResource_ProjectTextsAreGerman() ? nullptr : TextResource_ProjectTextEnglish(resourceId - TEXT_ID_PROJECT_BASE);
    if (*outText == nullptr) {
      *outText = g_ProjectTexts[resourceId - TEXT_ID_PROJECT_BASE];
    }
    return true;
  }
  if (resourceId == TEXT_ID_ADVANCED_PATHING) {
    /* not in the original */
    *outText = TextResource_ProjectTextsAreGerman() ? g_ProjectTextVerbesserteWegfindung : g_ProjectTextImprovedPathfinding;
    return true;
  }
  if ((resourceId >= TEXT_ID_LANGUAGE_NAME_BASE) && (resourceId < TEXT_ID_LANGUAGE_NAME_BASE + TEXT_ID_LANGUAGE_NAME_COUNT)) {
    *outText = g_LanguageNames[resourceId - TEXT_ID_LANGUAGE_NAME_BASE]; /* not in the original */
    return true;
  }
  if ((resourceId >= TEXT_ID_KEY_BINDING_BASE) && (resourceId < TEXT_ID_KEY_BINDING_BASE + TEXT_ID_KEY_BINDING_COUNT)) {
    *outText = (g_KeyBindingTextsEnglishSelected ? g_KeyBindingTextsEnglish
                                                 : g_KeyBindingTextsGerman)[resourceId - TEXT_ID_KEY_BINDING_BASE];
    return true;
  }
  if (g_TextResourceOverrides != nullptr) {
    /* the first entry with this id; its text is the entry of the same index in textPointers */
    for (overrideIndex = 0; overrideIndex < TEXT_RESOURCE_OVERRIDE_CAPACITY; overrideIndex++) {
      if (resourceId == g_TextResourceOverrides->resourceIds[overrideIndex]) {
        *outText = g_TextResourceOverrides->textPointers[overrideIndex];
        return true;
      }
    }
  }
  /* The original tests only bits 16-23 and indexes the 256 page bindings with the whole shifted id; bounded here
     because ids come from save files and the network: ids with bits 24-31 set are missing. */
  pageIndex = ((resourceId & 0xff0000) == 0) ? (uint32_t)resourceId >> 8 : (uint32_t)resourceId >> 16;
  if (pageIndex >= sizeof(g_TextResourcePageBindings) / sizeof(g_TextResourcePageBindings[0])) {
    Thandor_Log("text resource 0x%08X missing (page out of range)", resourceId);
    *outText = Thandor_U32ToPointer<uint16_t>(TEXT_RESOURCE_MISSING_SENTINEL_0x33);
    return false;
  }
  if ((resourceId & 0xff0000) == 0) {
    /* compact id: page << 8 | 8-bit index; the string offsets follow the 16-byte block prefix */
    localeBlock = g_TextResourcePageBindings[pageIndex].selectedLocaleBlock;
    if ((localeBlock != nullptr) &&
       ((resourceId & 0xff) < localeBlock->stringCount)) {
      /* the string offsets are relative to the block */
      *outText = Asset_RecordAt<uint16_t>(localeBlock,Asset_RecordAfter<uint32_t>(localeBlock)[resourceId & 0xff]);
      return true;
    }
  }
  else {
    /* extended id: page << 16 | 16-bit index */
    localeBlock = g_TextResourcePageBindings[pageIndex].selectedLocaleBlock;
    if ((localeBlock != nullptr) &&
       ((resourceId & 0xffff) < localeBlock->stringCount)) {
      *outText = Asset_RecordAt<uint16_t>(localeBlock,Asset_RecordAfter<uint32_t>(localeBlock)[resourceId & 0xffff]);
      return true;
    }
  }
  /* logged once: a missing id is resolved again on every frame that draws it */
  if (!loggedMissingText) {
    loggedMissingText = true;
    Thandor_Log("text resource 0x%08X missing (page binding %p; further missing ids not logged)", resourceId,
                static_cast<void *>(g_TextResourcePageBindings[pageIndex].selectedLocaleBlock.get()));
  }
  *outText = Thandor_U32ToPointer<uint16_t>(TEXT_RESOURCE_MISSING_SENTINEL_0x33);
  return false;
}

/* Returns the text of a resource id (see TextResource_TryResolve); a missing text gives the shared empty string.
   The original returns TEXT_RESOURCE_MISSING_SENTINEL_0x33 there, which every caller dereferences like any other
   text pointer (copy, measure, draw, nested-stream payload); bounded here because ids come from user maps, model
   files and savegames (e.g. an out-of-range model nameTextIndex), and no caller tests for the sentinel. The miss
   is logged by TextResource_TryResolve. */
uint16_t *TextResource_Resolve(TextResourceId resourceId)

{
  uint16_t *text;

  if (!TextResource_TryResolve(resourceId,&text)) {
    return g_EmptyTextResourceUtf16;
  }
  return text;
}
