/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/resources.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/resources.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
static uint16_t g_ProjectText30[] = {PROJECT_TEXT_BRIGHT,'3','0',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectText60[] = {PROJECT_TEXT_BRIGHT,'6','0',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectText120[] = {PROJECT_TEXT_BRIGHT,'1','2','0',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectText144[] = {PROJECT_TEXT_BRIGHT,'1','4','4',' ','B','i','l','d','e','r','/','s',0};
static uint16_t g_ProjectTextVSync[] = {PROJECT_TEXT_BRIGHT,'V','S','y','n','c',0};
static uint16_t g_ProjectTextNoteSoftware[] = {'3','D','-','K','a','n','t','e','n',' ','u','n','d',' ','U','I','-','S','k','a','l','i','e','r','u','n','g',' ','w','i','r','k','e','n',' ','n','u','r',' ','m','i','t',' ','V','u','l','k','a','n',' ','o','d','e','r',' ','D','i','r','e','c','t','X',' ','1','2',0};
static uint16_t g_ProjectTextNoteUiScale[] = {'D','i','e',' ','U','I','-','S','k','a','l','i','e','r','u','n','g',' ','g','i','l','t',' ','a','b',' ','d','e','m',' ','n',0x00E4,'c','h','s','t','e','n',' ','A','n','w','e','n','d','e','n',' ','u','n','t','e','r',' ','A','n','z','e','i','g','e',0};
static uint16_t *const g_ProjectTexts[TEXT_ID_PROJECT_COUNT] = {
    g_ProjectTextAnzeigemodus, g_ProjectTextFenster, g_ProjectTextVollbildfenster, g_ProjectTextVollbild,
    g_ProjectTextAnzeige, g_ProjectTextAnzeigeeinstellungen, g_ProjectTextErweitert, g_ProjectTextErweiterteEinstellungen,
    g_ProjectText3DKanten, g_ProjectTextGlatt, g_ProjectTextOriginal, g_ProjectTextUiSkalierung,
    g_ProjectTextAuto, g_ProjectText1x, g_ProjectText2x, g_ProjectText3x,
    g_ProjectTextBildratenbegrenzung, g_ProjectTextAus, g_ProjectText30, g_ProjectText60,
    g_ProjectText120, g_ProjectText144, g_ProjectTextVSync, g_ProjectTextNoteSoftware,
    g_ProjectTextNoteUiScale};

/* Loads the level's own text page (the .str entry of a level package) as page 0x30 and makes its title,
   description and 14 further description lines reachable under the global ids the frontend uses for that
   level: TEXT_ID_LEVEL_TITLE_BASE + title index and TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE *
   title index (+1..14).
   Returns true when the page cannot be loaded or one of the strings is missing, false on success.
*/
Bool8 TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path)

{
  uint16_t *resolvedText;
  int lineIndex;
  Bool8 failed;

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
        do {
          if (!TextResource_TryResolve(lineIndex + TEXT_ID_LEVEL_PAGE_EXTRA_LINES,&resolvedText)) {
            return true;
          }
          TextResourceOverride_Register
                    (levelTitleIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE + (TEXT_ID_LEVEL_DESCRIPTION_BASE + 1) + lineIndex,
                     resolvedText);
          lineIndex--;
        } while (-1 < lineIndex);
        failed = false;
      }
    }
  }
  return failed;
}

/* Returns the first locale block of a 'str' asset whose country code is countryCode, or NULL. The blocks follow
   the 0x200-byte asset header; block + blockSizeBytes is the next block.
   Original quirk: the first block is always checked and the count is only tested after stepping, so a block
   count of 0 wraps and keeps scanning past the asset (TextResourceAsset_HasValidBlocks rejects that before). */
static TextResourceLocaleBlockPrefix *TextResourceAsset_FindLocaleBlock
          (TextResourceAssetHeader *asset,LocaleTelephoneCountryCode countryCode)
{
  TextResourceLocaleBlockPrefix *block;
  AssetRecordCount remainingBlocks;

  remainingBlocks = (asset->localeCountHeader).localeBlockCount;
  block = (TextResourceLocaleBlockPrefix *)(asset + 1);
  do {
    if (block->countryCode == countryCode) {
      return block;
    }
    block = (TextResourceLocaleBlockPrefix *)((uint8_t *)block + block->blockSizeBytes);
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
  do {
    if (blockOffset + sizeof(TextResourceLocaleBlockPrefix) > byteCount) {
      return false;
    }
    blockOffset += ((const TextResourceLocaleBlockPrefix *)((const uint8_t *)asset + blockOffset))->blockSizeBytes;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return true;
}

/* Not in the original: true when the selected block's string offset table and every string start lie inside
   the asset (assetEnd is one past its last whole code unit), with room for at least one code unit. */
static bool TextResourceLocaleBlock_HasValidStrings(const TextResourceLocaleBlockPrefix *localeBlock,
                                                    const uint8_t *assetEnd)
{
  uint64_t blockBytes;
  const uint32_t *stringOffsets;
  uint32_t stringIndex;

  blockBytes = (uint64_t)(assetEnd - (const uint8_t *)localeBlock);
  if ((blockBytes < sizeof(TextResourceLocaleBlockPrefix)) ||
      ((uint64_t)localeBlock->stringCount * sizeof(uint32_t) > blockBytes - sizeof(TextResourceLocaleBlockPrefix))) {
    return false;
  }
  stringOffsets = (const uint32_t *)(localeBlock + 1);
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
static Bool8 TextResourcePage_RejectAsset(TextResourcePageIndex pageIndex,uint16_t *path,
                                          TextResourceAssetHeader *allocation,const char *reason,
                                          uintptr_t *outLocaleBlockOrError)
{
  Thandor_Log("text page 0x%02X \"%ls\": malformed asset rejected (%s)", pageIndex, (wchar_t *)path, reason);
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

  highDigits = *(const uint32_t *)(recordStart + 1);
  lowDigits = *(const uint32_t *)(recordStart + 3);
  return ((lowDigits >> 16 & 0xf) + (lowDigits & 0xf) * 10) + (highDigits >> 16 & 0xf) * 100 +
         (highDigits & 0xf) * 1000;
}

/* Loads a 'str' text asset as page pageIndex: picks the locale block of the configured (or system) country,
   else the Great Britain block, else the first one, binds it, and prepares every string's command records for
   run time (see the switch). Returns true on success and stores the selected block's address in
   *outLocaleBlockOrError; returns false and stores the package error there, or
   TEXT_RESOURCE_MISSING_SENTINEL_0x33 for a non-'str' or malformed asset (which is released; the page binding
   stays unchanged). outLocaleBlockOrError may be NULL.
*/
Bool8 TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path,uintptr_t *outLocaleBlockOrError)

{
  uint16_t codeUnit;
  uint32_t decimalValue;
  TextResourceAssetHeader *allocation;
  TextResourceLocaleBlockPrefix *localeBlock;
  uint32_t *stringOffsets;
  LocaleTelephoneCountryCode countryCode;
  TextResourceStringCount remainingStrings;
  uint16_t *recordStart;
  uint16_t *textCursor;
  int stringIndex;
  uint32_t loadErrorCode;
  uint32_t byteCount;
  uint16_t *assetEnd;

  allocation = (TextResourceAssetHeader *)Package_LoadEntryWithSize(path,&byteCount,&loadErrorCode);
  if (allocation == nullptr) {
    Thandor_Log("text page 0x%02X \"%ls\": load failed 0x%08X", pageIndex, (wchar_t *)path,
                loadErrorCode);
    if (outLocaleBlockOrError != nullptr) {
      *outLocaleBlockOrError = loadErrorCode;
    }
    return false;
  }
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
  countryCode = g_LocaleCountryCodeOverride;
  if (g_LocaleCountryCodeOverride == 0) {
    countryCode = g_LocaleGetDefaultTelephoneCountryCode();
  }
  /* the block for the country code, else the Great Britain block, else the first block */
  localeBlock = TextResourceAsset_FindLocaleBlock(allocation,countryCode);
  if (localeBlock == nullptr) {
    localeBlock = TextResourceAsset_FindLocaleBlock(allocation,LOCALE_COUNTRY_GREAT_BRITAIN);
    if (localeBlock == nullptr) {
      localeBlock = (TextResourceLocaleBlockPrefix *)(allocation + 1);
    }
  }
  /* The original binds the page here, before the walk; bound after the walk here so that a rejected asset leaves
     the binding unchanged (nothing reads it during the walk). */
  assetEnd = (uint16_t *)((uint8_t *)allocation + (byteCount & ~1u));
  if (!TextResourceLocaleBlock_HasValidStrings(localeBlock,(uint8_t *)assetEnd)) {
    return TextResourcePage_RejectAsset(pageIndex,path,allocation,"string table",outLocaleBlockOrError);
  }
  /* the string offsets (relative to the block) follow the 16-byte block prefix */
  stringOffsets = (uint32_t *)(localeBlock + 1);
  stringIndex = 0;
  for (remainingStrings = localeBlock->stringCount; remainingStrings != 0; remainingStrings--) {
    textCursor = (uint16_t *)((uint8_t *)localeBlock + stringOffsets[stringIndex]);
    /* The original walks to the terminator; bounded here because a string without one, or a record cut off by
       the asset end, would be read and rewritten past the asset. */
    for (;;) {
      if (textCursor >= assetEnd) {
        return TextResourcePage_RejectAsset(pageIndex,path,allocation,"unterminated string",outLocaleBlockOrError);
      }
      if (*textCursor == 0) {
        break;
      }
      recordStart = textCursor;
      codeUnit = *recordStart;
      textCursor = recordStart + 1;
      if (((short)codeUnit < 0) && (assetEnd - recordStart < RICHTEXT_RECORD_UNITS_NESTED)) {
        /* the nested-stream and image records read and rewrite their whole payload */
        switch(codeUnit & RICHTEXT_OPCODE_MASK) {
        case RICHTEXT_OP_CALL_NESTED:
        case RICHTEXT_OP_JUMP_NESTED:
        case RICHTEXT_OP_INLINE_IMAGE:
          return TextResourcePage_RejectAsset(pageIndex,path,allocation,"truncated record",outLocaleBlockOrError);
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
          *(uint32_t *)(recordStart + 3) = decimalValue;
          textCursor = recordStart + RICHTEXT_RECORD_UNITS_NESTED;
          break;
        case RICHTEXT_OP_INLINE_IMAGE:
          decimalValue = RichTextRecord_ParseDecimalDigits(recordStart);
          *(uint32_t *)(recordStart + 1) = 0;
          *(uint32_t *)(recordStart + 3) = decimalValue;
          textCursor = recordStart + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
          break;
        }
      }
    }
    stringIndex++;
  }
  g_TextResourcePageBindings[pageIndex].selectedLocaleBlock = localeBlock;
  g_TextResourcePageBindings[pageIndex].asset = allocation;
  if (outLocaleBlockOrError != nullptr) {
    *outLocaleBlockOrError = (uintptr_t)localeBlock;
  }
  return true;
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

/* Looks up the text of a resource id: TEXT_RESOURCE_ID_NONE gives the shared empty string, then the runtime
   override table is searched, then the bound locale block of the id's page (compact or extended id, see
   resources.h). Stores the text in *outText and returns true when found; a missing text stores
   TEXT_RESOURCE_MISSING_SENTINEL_0x33 (the pointer value 0x33, not a real string) there and returns false.
*/
Bool8 TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText)

{
  static bool loggedMissingText; /* open-thandor diagnostics: the first missing id only */
  TextResourceLocaleBlockPrefix *localeBlock;
  uint32_t overrideIndex;
  uint32_t pageIndex;

  if (resourceId == TEXT_RESOURCE_ID_NONE) {
    *outText = (uint16_t *)THANDOR_ADDR(g_EmptyTextResourceUtf16,0);
    return true;
  }
  if ((resourceId >= TEXT_ID_PROJECT_BASE) && (resourceId < TEXT_ID_PROJECT_BASE + TEXT_ID_PROJECT_COUNT)) {
    *outText = g_ProjectTexts[resourceId - TEXT_ID_PROJECT_BASE];
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
    *outText = (uint16_t *)(uintptr_t)TEXT_RESOURCE_MISSING_SENTINEL_0x33;
    return false;
  }
  if ((resourceId & 0xff0000) == 0) {
    /* compact id: page << 8 | 8-bit index; the string offsets follow the 16-byte block prefix */
    localeBlock = g_TextResourcePageBindings[pageIndex].selectedLocaleBlock;
    if ((localeBlock != nullptr) &&
       ((resourceId & 0xff) < localeBlock->stringCount)) {
      /* the string offsets are relative to the block */
      *outText = (uint16_t *)((uint8_t *)localeBlock + ((uint32_t *)(localeBlock + 1))[resourceId & 0xff]);
      return true;
    }
  }
  else {
    /* extended id: page << 16 | 16-bit index */
    localeBlock = g_TextResourcePageBindings[pageIndex].selectedLocaleBlock;
    if ((localeBlock != nullptr) &&
       ((resourceId & 0xffff) < localeBlock->stringCount)) {
      *outText = (uint16_t *)((uint8_t *)localeBlock + ((uint32_t *)(localeBlock + 1))[resourceId & 0xffff]);
      return true;
    }
  }
  /* logged once: a missing id is resolved again on every frame that draws it */
  if (!loggedMissingText) {
    loggedMissingText = true;
    Thandor_Log("text resource 0x%08X missing (page binding %p; further missing ids not logged)", resourceId,
                (void *)g_TextResourcePageBindings[pageIndex].selectedLocaleBlock);
  }
  *outText = (uint16_t *)(uintptr_t)TEXT_RESOURCE_MISSING_SENTINEL_0x33;
  return false;
}

/* Returns the text of a resource id (see TextResource_TryResolve); a missing text gives
   TEXT_RESOURCE_MISSING_SENTINEL_0x33, which callers use like any other text pointer. */
uint16_t *TextResource_Resolve(TextResourceId resourceId)

{
  uint16_t *text;

  TextResource_TryResolve(resourceId,&text);
  return text;
}
