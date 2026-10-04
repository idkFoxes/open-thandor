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

static TextResourcePageBinding g_TextResourcePageBindings[256] = {0};

TextResourceOverrideTable *g_TextResourceOverrides = 0;

static uint16_t g_EmptyTextResourceUtf16[2] = {0};

/* UTF-16 rich-text stream L"-" (code unit '-' plus terminator) that unresolved nested-stream records point to */
static uint16_t g_MissingTextResourceFallbackStream[2] = {0x002D, 0x0000};

/* Implementation ownership: assets/text/resources. */

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

  failed = !TextResourcePage_Load(TEXT_RESOURCE_PAGE_LEVEL,path,NULL);
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
   count of 0 wraps and keeps scanning past the asset. */
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
  return NULL;
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
   TEXT_RESOURCE_MISSING_SENTINEL_0x33 for a non-'str' asset (which is released). outLocaleBlockOrError may be
   NULL.
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

  allocation = (TextResourceAssetHeader *)Package_LoadEntry(path,&loadErrorCode);
  if (allocation == NULL) {
    Thandor_Log("text page 0x%02X \"%ls\": load failed 0x%08X", pageIndex, (wchar_t *)path,
                loadErrorCode);
    if (outLocaleBlockOrError != NULL) {
      *outLocaleBlockOrError = loadErrorCode;
    }
    return false;
  }
  if ((allocation->localeCountHeader).common.magic != ASSET_MAGIC_STR) {
    Resource_Release(allocation);
    if (outLocaleBlockOrError != NULL) {
      *outLocaleBlockOrError = TEXT_RESOURCE_MISSING_SENTINEL_0x33;
    }
    return false;
  }
  countryCode = g_LocaleCountryCodeOverride;
  if (g_LocaleCountryCodeOverride == 0) {
    countryCode = g_LocaleGetDefaultTelephoneCountryCode();
  }
  /* the block for the country code, else the Great Britain block, else the first block */
  localeBlock = TextResourceAsset_FindLocaleBlock(allocation,countryCode);
  if (localeBlock == NULL) {
    localeBlock = TextResourceAsset_FindLocaleBlock(allocation,LOCALE_COUNTRY_GREAT_BRITAIN);
    if (localeBlock == NULL) {
      localeBlock = (TextResourceLocaleBlockPrefix *)(allocation + 1);
    }
  }
  g_TextResourcePageBindings[pageIndex].selectedLocaleBlock = localeBlock;
  g_TextResourcePageBindings[pageIndex].asset = allocation;
  /* the string offsets (relative to the block) follow the 16-byte block prefix */
  stringOffsets = (uint32_t *)(localeBlock + 1);
  stringIndex = 0;
  for (remainingStrings = localeBlock->stringCount; remainingStrings != 0; remainingStrings--) {
    textCursor = (uint16_t *)((uint8_t *)localeBlock + stringOffsets[stringIndex]);
    while (*textCursor != 0) {
      recordStart = textCursor;
      codeUnit = *recordStart;
      textCursor = recordStart + 1;
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
  if (outLocaleBlockOrError != NULL) {
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

  if (g_TextResourceOverrides == NULL) {
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
  return;
}

/* Looks up the text of a resource id: TEXT_RESOURCE_ID_NONE gives the shared empty string, then the runtime
   override table is searched, then the bound locale block of the id's page (compact or extended id, see
   resources.h). Stores the text in *outText and returns true when found; a missing text stores
   TEXT_RESOURCE_MISSING_SENTINEL_0x33 (the pointer value 0x33, not a real string) there and returns false.
*/
Bool8 TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText)

{
  TextResourceLocaleBlockPrefix *localeBlock;
  uint32_t overrideIndex;

  if (resourceId == TEXT_RESOURCE_ID_NONE) {
    *outText = (uint16_t *)THANDOR_ADDR(g_EmptyTextResourceUtf16,0);
    return true;
  }
  if (g_TextResourceOverrides != NULL) {
    /* the first entry with this id; its text is the entry of the same index in textPointers */
    for (overrideIndex = 0; overrideIndex < TEXT_RESOURCE_OVERRIDE_CAPACITY; overrideIndex++) {
      if (resourceId == g_TextResourceOverrides->resourceIds[overrideIndex]) {
        *outText = g_TextResourceOverrides->textPointers[overrideIndex];
        return true;
      }
    }
  }
  if ((resourceId & 0xff0000) == 0) {
    /* compact id: page << 8 | 8-bit index; the string offsets follow the 16-byte block prefix */
    localeBlock = g_TextResourcePageBindings[resourceId >> 8].selectedLocaleBlock;
    if ((localeBlock != NULL) &&
       ((resourceId & 0xff) < localeBlock->stringCount)) {
      /* the string offsets are relative to the block */
      *outText = (uint16_t *)((uint8_t *)localeBlock + ((uint32_t *)(localeBlock + 1))[resourceId & 0xff]);
      return true;
    }
  }
  else {
    /* extended id: page << 16 | 16-bit index */
    localeBlock = g_TextResourcePageBindings[resourceId >> 16].selectedLocaleBlock;
    if ((localeBlock != NULL) &&
       ((resourceId & 0xffff) < localeBlock->stringCount)) {
      *outText = (uint16_t *)((uint8_t *)localeBlock + ((uint32_t *)(localeBlock + 1))[resourceId & 0xffff]);
      return true;
    }
  }
  Thandor_Log("text resource 0x%08X missing (page binding %p)", resourceId,
              (void *)g_TextResourcePageBindings[(resourceId & 0xff0000) == 0 ? resourceId >> 8 : resourceId >> 16].selectedLocaleBlock);
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
