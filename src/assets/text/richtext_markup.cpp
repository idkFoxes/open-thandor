/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/richtext_markup.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/richtext_markup.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>

/* Module data. */

static uint16_t g_Txt2strUnknownCharacterErrorUtf16[62] = {'e', 'r', 'r', 'o', 'r', ':', ' ', 'T', 'X', 'T', '2', 'S', 'T', 'R', ':', ' ', 'u', 'n', 'k', 'n', 'o', 'w', 'n', ' ', 'c', 'h', 'a', 'r', 'a', 'c', 't', 'e', 'r', ' ', 'a', 't', ':', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0}; /* L"error: TXT2STR: unknown character at:                        " */

/* Outcome of one parsing step of the TXT2STR markup (see RichTextMarkup_ParseAndBuildStringAsset). */
enum {
  RICHTEXT_MARKUP_PARSE_CONTINUE,         /* keep reading */
  RICHTEXT_MARKUP_PARSE_END,              /* '#.' reached */
  RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE,     /* the output buffer or the tag table is full */
  RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER
};
using RichTextMarkupParseResult = int;

/* Parser state of RichTextMarkup_ParseAndBuildStringAsset. tagStarts/tagKeys receive the (string start, key)
   pair of every '#<' in order (the original pushes these pairs on the machine stack). */
struct RichTextMarkupParser {
  uint8_t *markupCursor;
  uint16_t *outputCursor;
  uint32_t remainingCapacityBytes;
  uint32_t codeUnitBias;
  int32_t key;
  Bool8 insideTag;
  uint32_t tagCount;
  uint16_t **tagStarts;
  int32_t *tagKeys;
};

/* Appends one code unit to the output; false when no more than 2 bytes are left. */
static Bool8 RichTextMarkup_EmitCodeUnit(RichTextMarkupParser *parser,uint16_t codeUnit)
{
  if (parser->remainingCapacityBytes <= 2) {
    return false;
  }
  parser->remainingCapacityBytes -= 2;
  *parser->outputCursor++ = codeUnit;
  return true;
}

/* A text byte: emitted with the current code page bias inside a tag, ignored outside. */
static Bool8 RichTextMarkup_EmitTextByte(RichTextMarkupParser *parser,uint32_t markupByte)
{
  if (!parser->insideTag) {
    return true;
  }
  return RichTextMarkup_EmitCodeUnit(parser,(uint16_t)(markupByte + parser->codeUnitBias));
}

/* '#>': ends the string with a NUL terminator, padded to a dword boundary. */
static Bool8 RichTextMarkup_TerminateString(RichTextMarkupParser *parser)
{
  if (((uint32_t)(uintptr_t)parser->outputCursor & 2) == 0) {
    if (parser->remainingCapacityBytes <= 4) {
      return false;
    }
    parser->remainingCapacityBytes -= 4;
    parser->outputCursor[0] = 0;
    parser->outputCursor[1] = 0;
    parser->outputCursor += 2;
    return true;
  }
  return RichTextMarkup_EmitCodeUnit(parser,0);
}

/* Handles the escape after a '#' (the cursor points behind the '#'). */
static RichTextMarkupParseResult RichTextMarkup_ParseEscape(RichTextMarkupParser *parser)
{
  uint32_t markupByte;
  uint8_t lineEndByte;
  uint8_t *digits;

  markupByte = *parser->markupCursor++;
  switch (markupByte) {
  case '\n':
  case '\r':
    /* line continuation: skip the line end */
    while (*parser->markupCursor < ' ') {
      lineEndByte = *parser->markupCursor++;
      if ((lineEndByte != '\n') && (lineEndByte != '\r')) {
        return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
      }
    }
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '!':
    parser->codeUnitBias = RICHTEXT_MARKUP_COMMAND_BIAS; /* '@'..'_' become RICHTEXT_COMMAND_FLAG | 0x00..0x1F */
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '#':
    /* a literal '#', handled like any text byte */
    if (!RichTextMarkup_EmitTextByte(parser,markupByte)) {
      return RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE;
    }
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '-':
    /* no inside-tag test in the original */
    if (!RichTextMarkup_EmitCodeUnit(parser,RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_SOFT_HYPHEN)) {
      return RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE;
    }
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
    /* '#ddd': three decimal digits, the key of the following tags */
    digits = parser->markupCursor;
    parser->key = (int32_t)(markupByte - '0') * 10;
    if ((digits[0] < '0') || ('9' < digits[0])) {
      return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
    }
    parser->key = parser->key + (digits[0] - '0');
    if ((digits[1] < '0') || ('9' < digits[1])) {
      return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
    }
    parser->key = parser->key * 10 + (digits[1] - '0');
    parser->markupCursor += 2;
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '<':
    if (parser->insideTag) {
      return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
    }
    /* the string starts here and gets the current key; one entry stays free for the end sentinel */
    if (parser->tagCount >= RICHTEXT_MARKUP_TAG_LIMIT - 1) {
      return RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE;
    }
    parser->tagStarts[parser->tagCount] = parser->outputCursor;
    parser->tagKeys[parser->tagCount] = parser->key;
    parser->tagCount++;
    parser->insideTag = true;
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '>':
    if (!parser->insideTag) {
      return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
    }
    parser->insideTag = false;
    if (!RichTextMarkup_TerminateString(parser)) {
      return RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE;
    }
    return RICHTEXT_MARKUP_PARSE_CONTINUE;
  case '.':
    if ((parser->tagCount == 0) || parser->insideTag) {
      return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
    }
    return RICHTEXT_MARKUP_PARSE_END;
  default:
    if (('@' <= markupByte) && (markupByte <= '~')) {
      /* '#@'..'#~': code page select, following bytes are emitted + (c - '@') * 0x80 */
      parser->codeUnitBias =
           markupByte * RICHTEXT_MARKUP_CODE_PAGE_UNITS - '@' * RICHTEXT_MARKUP_CODE_PAGE_UNITS;
      return RICHTEXT_MARKUP_PARSE_CONTINUE;
    }
    return RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
  }
}

/* Compiles the markup into tagged rich-text strings until '#.', a full buffer or an invalid character. */
static RichTextMarkupParseResult RichTextMarkup_ParseMarkup(RichTextMarkupParser *parser)
{
  RichTextMarkupParseResult result;
  uint32_t markupByte;

  result = RICHTEXT_MARKUP_PARSE_CONTINUE;
  while (result == RICHTEXT_MARKUP_PARSE_CONTINUE) {
    markupByte = *parser->markupCursor++;
    switch (markupByte) {
    /* control characters other than tab, LF and CR, and DEL */
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
    case 11: case 12: case 14: case 15:
    case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
    case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
    case 127:
      result = RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER;
      break;
    case '\t':
    case '\n':
      break;
    case '\r':
      if (parser->insideTag &&
          !RichTextMarkup_EmitCodeUnit(parser,RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK)) {
        result = RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE;
      }
      break;
    case '#':
      result = RichTextMarkup_ParseEscape(parser);
      break;
    default:
      if (!RichTextMarkup_EmitTextByte(parser,markupByte)) {
        result = RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE;
      }
      break;
    }
  }
  return result;
}

/* Index of the first tag whose key is not grouped yet (grouped strings and the sentinel carry key -1), or
   tagCount when every string is grouped. */
static uint32_t RichTextMarkup_FirstUngroupedTag(const int32_t *tagKeys,uint32_t tagCount)
{
  uint32_t index;

  for (index = 0; (index < tagCount) && (tagKeys[index] == -1); index++) {
  }
  return index;
}

/* Writes the string groups at *assetCursorInOut: per key (in the order of its first string) a group header
   {size, string count, key, 0}, the string offsets (relative to the group header) and the strings. String k
   ends at tagStarts[k + 1]. Returns false when remainingCapacityBytes runs out; otherwise advances
   *assetCursorInOut behind the last group and stores the group count. */
static Bool8 RichTextMarkup_WriteStringGroups
          (uint16_t **tagStarts,int32_t *tagKeys,uint32_t tagCount,uint32_t remainingCapacityBytes,
           uint32_t **assetCursorInOut,uint32_t *outGroupCount)
{
  uint32_t *assetCursor;
  uint32_t *groupHeader;
  uint32_t *offsetCursor;
  uint32_t groupCount;
  int32_t groupKey;
  uint32_t spanBytes;
  uint32_t dwordCount;
  uint32_t *copySource;
  uint32_t first;
  uint32_t index;

  assetCursor = *assetCursorInOut;
  groupCount = 0;
  for (first = RichTextMarkup_FirstUngroupedTag(tagKeys,tagCount); first != tagCount;
       first = RichTextMarkup_FirstUngroupedTag(tagKeys,tagCount)) {
    groupKey = tagKeys[first];
    if (remainingCapacityBytes <= sizeof(TextResourceLocaleBlockPrefix)) {
      return false;
    }
    remainingCapacityBytes -= sizeof(TextResourceLocaleBlockPrefix);
    /* groupHeader is a TextResourceLocaleBlockPrefix: [0] block size, [1] string count, [2] key */
    groupHeader = assetCursor;
    groupHeader[2] = (uint32_t)groupKey;
    groupHeader[0] = sizeof(TextResourceLocaleBlockPrefix);
    groupHeader[1] = 0;
    for (index = first; index < tagCount; index++) {
      if (tagKeys[index] == groupKey) {
        groupHeader[1]++;
        spanBytes = (uint32_t)Asset_ByteDistance(tagStarts[index + 1],tagStarts[index]) + 4;
        groupHeader[0] += spanBytes;
        if (remainingCapacityBytes <= spanBytes) {
          return false;
        }
        remainingCapacityBytes -= spanBytes;
      }
    }
    groupCount++;
    offsetCursor = groupHeader + sizeof(TextResourceLocaleBlockPrefix) / sizeof(uint32_t);
    assetCursor = offsetCursor + groupHeader[1];
    for (index = 0; index < tagCount; index++) {
      if (tagKeys[index] == groupKey) {
        tagKeys[index] = -1;
        *offsetCursor++ = (uint32_t)Asset_ByteDistance(assetCursor,groupHeader);
        copySource = reinterpret_cast<uint32_t *>(tagStarts[index]); /* the UTF-16 string copied as dwords */
        for (dwordCount = (uint32_t)Asset_ByteDistance(tagStarts[index + 1],tagStarts[index]) >> 2;
             dwordCount != 0; dwordCount--) {
          *assetCursor++ = *copySource++;
        }
      }
    }
  }
  *assetCursorInOut = assetCursor;
  *outGroupCount = groupCount;
  return true;
}

/* Frees the string buffer and fails with FATAL_ERROR_GENERAL_FAILURE (arena space exhausted). */
static Bool8 RichTextMarkup_FreeBufferAndFail(uint16_t *memory,uint32_t *outError)
{
  g_MemoryApi.free(memory);
  *outError = FATAL_ERROR_GENERAL_FAILURE;
  return false;
}

/* Frees the string buffer and fails with the "TXT2STR: unknown character" message, which gets the byte offset
   after the offending character(s) (markupCursor - markupBytes) written in at code unit
   RICHTEXT_MARKUP_ERROR_OFFSET_UNIT. */
static Bool8 RichTextMarkup_ReportInvalidCharacter
          (uint16_t *memory,uint8_t *markupBytes,uint8_t *markupCursor,uint32_t *outError)
{
  g_MemoryApi.free(memory);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)(markupCursor - markupBytes),
             &g_Txt2strUnknownCharacterErrorUtf16[RICHTEXT_MARKUP_ERROR_OFFSET_UNIT]);
  *outError = (uint32_t)(uintptr_t)g_Txt2strUnknownCharacterErrorUtf16;
  return false;
}

/* Leftover of the TXT2STR converter (no caller and no function-pointer table entry in src/): compiles text
   markup into a 'str' string asset. Text between '#<' and '#>'
   becomes one NUL-terminated, dword-padded rich-text string keyed by the last '#ddd' number (text outside is
   ignored); further '#' escapes select a code page ('#@'..'#~'), raw command codes ('#!'), a soft hyphen
   ('#-', emitted also outside a tag), a literal '#' ('##') or a line continuation, a CR inside a tag is a line
   break, and '#.' ends the input and groups the strings by key: a 0x200-byte header, then per key (in the order
   of the key's first string) a 0x10-byte group header {size, string count, key, 0}, the string offsets
   (relative to the group header) and the strings.
   Returns true with the new asset in *outAsset on success. On failure returns false with the error value in
   *outError: for an invalid character (control byte, unknown escape, misplaced tag) the address of the
   formatted "TXT2STR: unknown character" message (with its byte offset), when the arena has no free block the
   allocator's error, when the arena space runs out FATAL_ERROR_GENERAL_FAILURE. On success the original also
   returns the asset size and asset + 0x100; they are dropped (there is no caller).
*/
Bool8 RichTextMarkup_ParseAndBuildStringAsset(uint8_t *markupBytes,void **outAsset,uint32_t *outError)

{
  /* The original pushes a (string start, key) pair per '#<' on the machine stack (and an (end of output, -1)
     sentinel at '#.'); tagStarts/tagKeys hold those pairs in push order, so string k ends at tagStarts[k + 1].
     More than RICHTEXT_MARKUP_TAG_LIMIT - 1 tags (the original is limited only by its stack) fail like an
     arena overflow. */
  static uint16_t *tagStarts[RICHTEXT_MARKUP_TAG_LIMIT];
  static int32_t tagKeys[RICHTEXT_MARKUP_TAG_LIMIT];
  uint32_t arenaError;
  uint32_t largestBlockSize;
  uint16_t *memory;
  void *block;
  RichTextMarkupParser parser;
  RichTextMarkupParseResult parseResult;
  uint32_t remainingCapacityBytes;
  uint32_t *asset;
  TextResourceAssetHeader *header;
  AssetBuildTimestampSet *timestamps;
  uint32_t *assetCursor;
  uint32_t groupCount;
  uint32_t index;
  uint32_t assetSize;

  arenaError = g_MemoryApi.allocLargestFreeBlock(&block,&largestBlockSize);
  if (arenaError != 0) {
    *outError = arenaError;
    return false;
  }
  memory = static_cast<uint16_t *>(block);
  parser.remainingCapacityBytes = largestBlockSize;
  parser.markupCursor = markupBytes;
  parser.outputCursor = memory;
  parser.key = 0;
  parser.codeUnitBias = 0;
  parser.insideTag = false;
  parser.tagCount = 0;
  parser.tagStarts = tagStarts;
  parser.tagKeys = tagKeys;
  parseResult = RichTextMarkup_ParseMarkup(&parser);
  if (parseResult == RICHTEXT_MARKUP_PARSE_INVALID_CHARACTER) {
    return RichTextMarkup_ReportInvalidCharacter(memory,markupBytes,parser.markupCursor,outError);
  }
  if (parseResult == RICHTEXT_MARKUP_PARSE_OUT_OF_SPACE) {
    return RichTextMarkup_FreeBufferAndFail(memory,outError);
  }
  /* '#.' reached: the end sentinel, its start is the end of the last string */
  tagStarts[parser.tagCount] = parser.outputCursor;
  tagKeys[parser.tagCount] = -1;
  parser.tagCount++;
  if (g_MemoryApi.shrinkInPlace((uint32_t)Asset_ByteDistance(parser.outputCursor,memory),memory) != 0) {
    return RichTextMarkup_FreeBufferAndFail(memory,outError);
  }
  if (g_MemoryApi.allocLargestFreeBlock(&block,&largestBlockSize) != 0) {
    return RichTextMarkup_FreeBufferAndFail(memory,outError);
  }
  asset = static_cast<uint32_t *>(block);
  if (largestBlockSize <= sizeof(TextResourceAssetHeader)) {
    g_MemoryApi.free(asset);
    return RichTextMarkup_FreeBufferAndFail(memory,outError);
  }
  remainingCapacityBytes = largestBlockSize - sizeof(TextResourceAssetHeader);

  for (index = 0; index < sizeof(TextResourceAssetHeader) / sizeof(uint32_t); index++) {
    asset[index] = 0;
  }
  assetCursor = asset + sizeof(TextResourceAssetHeader) / sizeof(uint32_t);
  if (!RichTextMarkup_WriteStringGroups
         (tagStarts,tagKeys,parser.tagCount,remainingCapacityBytes,&assetCursor,&groupCount)) {
    g_MemoryApi.free(asset);
    return RichTextMarkup_FreeBufferAndFail(memory,outError);
  }
  g_MemoryApi.free(memory);
  assetSize = (uint32_t)Asset_ByteDistance(assetCursor,asset);
  g_MemoryApi.shrinkInPlace(assetSize,asset);
  header = reinterpret_cast<TextResourceAssetHeader *>(asset); /* the dword buffer filled above is the asset */
  header->localeCountHeader.localeBlockCount = groupCount; /* +0xB0 */
  header->localeCountHeader.common.allocationSizeBytes = assetSize;
  header->localeCountHeader.common.magic = ASSET_MAGIC_STR;
  header->localeCountHeader.common.formatVersion = 1;
  header->localeCountHeader.common.converterVersion = 0;
  timestamps = &header->localeCountHeader.common.buildMetadata.timestamps;
  timestamps->timeValue0 = g_LocaleGetPackedCurrentTime();
  timestamps->timeValue1 = timestamps->timeValue0;
  timestamps->timeValue2 = timestamps->timeValue0;
  timestamps->dateValue0 = g_LocaleGetPackedCurrentDate();
  timestamps->dateValue1 = timestamps->dateValue0;
  timestamps->dateValue2 = timestamps->dateValue0;
  g_LocaleCopyDefaultComputerLabelUtf16(header->localeCountHeader.common.buildMetadata.names.producerName);
  g_LocaleCopyDefaultComputerLabelUtf16(header->localeCountHeader.common.buildMetadata.names.sourceName);
  *outAsset = asset;
  return true;
}
