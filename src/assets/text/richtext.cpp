/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/richtext.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/richtext.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

uint32_t g_RichTextRuntimeBufferUsedWords = 0;

uint8_t *g_FontRuntimeBuffer = nullptr;

/* Walks one command stream (without following nested streams) and points every nested-stream command
   (0x18/0x19) whose selector matches at replacementPayload, so a text can have its placeholders bound to
   concrete sub-streams at run time.
*/
void RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,uint16_t *stream)

{
  uint16_t commandCodeUnit;
  uint16_t *commandCursor;

  while ((commandCodeUnit = *(commandCursor = stream)) != 0) {
    stream = commandCursor + 1;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        stream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        stream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        /* payload: stream pointer at commandCursor + 1, selector at commandCursor + 3 */
        stream = commandCursor + RICHTEXT_RECORD_UNITS_NESTED;
        if (selector == static_cast<int>(Thandor_LoadU32(commandCursor + 3))) { /* 2-byte aligned payload dword */
          THANDOR_PTR32_AT(void, commandCursor + 1) = replacementPayload;
        }
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        stream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
  }
}

/* Walks one command stream (without following nested streams) and sets the texture source of every inline
   image command to textureSource, so a text's icons can be bound to the texture they are drawn from.
*/
void RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,uint16_t *stream)

{
  uint16_t *commandCursor;
  uint16_t *streamCursor;
  uint16_t commandCodeUnit;

  streamCursor = stream;
  while ((commandCodeUnit = *(commandCursor = streamCursor)) != 0) {
    streamCursor = commandCursor + 1;
    if ((short)commandCodeUnit < 0) { /* RICHTEXT_COMMAND_FLAG set */
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_NESTED;
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        /* the texture source pointer is the first payload dword, right after the command */
        THANDOR_PTR32_AT(GraphicsTextureSourceAsset, streamCursor) = textureSource;
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
  }
}

/* Converts a rich-text command stream into a NUL-terminated 8-bit string (for Win32 text such as message boxes):
   follows nested streams, turns the fixed-space and line-break commands into ' ' and CR LF, drops all other
   commands and every code unit above 0xFF. Returns true when the whole text fit; on overflow (or nesting deeper
   than RICHTEXT_NESTING_LIMIT) the output is cut and terminated and false is returned. (The original also
   returned the byte count including the terminator, which no caller reads.)
*/
Bool8 RichTextCommandStream_CopyToNarrow
          (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source)

{
  uint16_t *readCursor;
  uint32_t remainingCapacityBytes;
  uint16_t *commandCursor;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  uint16_t codeUnit;

  nestedDepth = 0;
  remainingCapacityBytes = capacityBytes;
  readCursor = source;
  /* Runs until the terminator of the outermost stream (the terminator of a nested stream returns to the
     caller stream). */
  while ((*readCursor != 0) || (nestedDepth != 0)) {
    commandCursor = readCursor;
    codeUnit = *commandCursor;
    readCursor = commandCursor + 1;
    if (codeUnit == 0) {
      readCursor = Asset_RecordAt<uint16_t>(nestedReturnStack[--nestedDepth],RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    if ((short)codeUnit < 0) {
      switch(codeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        readCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_FIXED_SPACE:
        remainingCapacityBytes--;
        if (remainingCapacityBytes == 0) {
          destination[-1] = 0;
          return false;
        }
        *destination = ' ';
        destination++;
        break;
      case RICHTEXT_OP_LINE_BREAK:
        /* CR LF needs two bytes and must still leave room for one more */
        if (remainingCapacityBytes <= 2) {
          destination[-1] = 0;
          return false;
        }
        remainingCapacityBytes = remainingCapacityBytes - 2;
        destination[0] = '\r';
        destination[1] = '\n';
        destination += 2;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
        if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
          destination[-1] = 0;
          return false;
        }
        nestedReturnStack[nestedDepth++] = readCursor;
        readCursor = THANDOR_PTR32_AT(uint16_t, readCursor);
        break;
      case RICHTEXT_OP_JUMP_NESTED:
        readCursor = THANDOR_PTR32_AT(uint16_t, readCursor);
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
    else if ((codeUnit & 0xff00) == 0) {
      remainingCapacityBytes--;
      if (remainingCapacityBytes == 0) {
        destination[-1] = 0;
        return false;
      }
      *destination = (uint8_t)codeUnit;
      destination++;
    }
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return true;
  }
  destination[-1] = 0;
  return false;
}

/* Copies a rich-text command stream into a bounded buffer with every nested stream (0x18/0x19) inlined, so the
   copy no longer depends on the streams it referenced. Commands are normalised to RICHTEXT_COMMAND_FLAG | opcode;
   payload records are copied unchanged. Returns true when the whole text fit and stores the byte count without
   the terminator in *outBytesWritten (may be NULL); on overflow (or nesting deeper than RICHTEXT_NESTING_LIMIT)
   the output is cut and terminated, *outBytesWritten is left untouched and false is returned.
*/
Bool8 RichTextCommandStream_CopyExpanded
          (TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint16_t *source,
           uint32_t *outBytesWritten)

{
  uint16_t commandCodeUnit;
  uint32_t recordUnits;
  uint32_t unitIndex;
  uint16_t *nextSource;
  uint16_t *destinationCursor;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;

  nestedDepth = 0;
  destinationCursor = destination;
  /* Runs until the terminator of the outermost stream (the terminator of a nested stream returns to the
     caller stream). On every overflow the last written code unit is replaced by the terminator. */
  while ((*source != 0) || (nestedDepth != 0)) {
    commandCodeUnit = *source;
    if (commandCodeUnit == 0) {
      source = Asset_RecordAt<uint16_t>(nestedReturnStack[--nestedDepth],RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    nextSource = source + 1;
    if ((short)commandCodeUnit >= 0) {
      /* a glyph: copied as is (the capacity must stay above zero for the terminator) */
      if (capacityBytes <= 2) {
        destinationCursor[-1] = 0;
        return false;
      }
      capacityBytes = capacityBytes - 2;
      *destinationCursor = commandCodeUnit;
      destinationCursor++;
      source = nextSource;
      continue;
    }
    switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
    default:
      if (capacityBytes <= 2) {
        destinationCursor[-1] = 0;
        return false;
      }
      capacityBytes = capacityBytes - 2;
      *destinationCursor = commandCodeUnit & RICHTEXT_OPCODE_MASK | RICHTEXT_COMMAND_FLAG;
      destinationCursor++;
      source = nextSource;
      continue;
    case RICHTEXT_OP_CALL_NESTED:
      if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
        destinationCursor[-1] = 0;
        return false;
      }
      nestedReturnStack[nestedDepth++] = nextSource;
      source = THANDOR_PTR32_AT(uint16_t, nextSource);
      continue;
    case RICHTEXT_OP_JUMP_NESTED:
      source = THANDOR_PTR32_AT(uint16_t, nextSource);
      continue;
    case RICHTEXT_OP_LITERAL_COLOR:
      recordUnits = RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      recordUnits = RICHTEXT_RECORD_UNITS_INLINE_VALUE;
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      recordUnits = RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      break;
    }
    /* a command with payload: the whole record is copied unchanged */
    if (capacityBytes <= recordUnits * 2) {
      destinationCursor[-1] = 0;
      return false;
    }
    capacityBytes = capacityBytes - recordUnits * 2;
    for (unitIndex = 0; unitIndex < recordUnits; unitIndex++) {
      destinationCursor[unitIndex] = source[unitIndex];
    }
    destinationCursor = destinationCursor + recordUnits;
    source = source + recordUnits;
  }
  if (1 < (int)capacityBytes) {
    *destinationCursor = 0;
    if (outBytesWritten != nullptr) {
      *outBytesWritten = (uint32_t)Asset_ByteDistance(destinationCursor,destination);
    }
    return true;
  }
  destinationCursor[-1] = 0;
  return false;
}

/* A nested-stream command (0x18/0x19) of RichTextCommandStream_FlattenNestedToRuntimeBuffer whose pointer is
   NULL: the original follows it and reads address 0; skipped here (the cursor moves behind the payload) because
   the pointers are patched in at run time (RichTextCommandStream_PatchPayloadBySelector) and may be unset.
   Logged once. */
static void RichTextCommandStream_SkipNullNestedStream(uint16_t **commandStream)
{
  static Bool8 s_logged = false;

  if (!s_logged) {
    s_logged = true;
    Thandor_Log("RichTextCommandStream_FlattenNestedToRuntimeBuffer: NULL nested stream skipped");
  }
  *commandStream = Asset_RecordAt<uint16_t>(*commandStream,RICHTEXT_NESTED_PAYLOAD_BYTES);
}

/* Copies commandStream into g_FontRuntimeBuffer with every nested stream inlined, so the line measuring and
   drawing code can walk one flat stream: glyphs and most commands are copied, literal colours and inline images
   with their payload, nested-stream commands are followed instead of copied and the reserved and inline-value
   commands are dropped. Output beyond RICHTEXT_RUNTIME_BUFFER_UNITS is discarded; the read position
   g_RichTextRuntimeBufferUsedWords is reset to the start.
*/
void RichTextCommandStream_FlattenNestedToRuntimeBuffer(uint16_t *commandStream)

{
  uint16_t commandCodeUnit;
  uint32_t remainingWords;
  int nestedDepth;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  uint16_t *commandCursor;
  uint16_t *outputCursor;
  
  remainingWords = RICHTEXT_RUNTIME_BUFFER_UNITS;
  nestedDepth = 0;
  outputCursor = reinterpret_cast<uint16_t *>(g_FontRuntimeBuffer); /* the byte buffer holds the flattened UTF-16 stream */
  /* Runs until the terminator of the outermost stream (the terminator of a nested stream returns to the
     caller stream). */
  while ((*commandStream != 0) || (nestedDepth != 0)) {
    commandCursor = commandStream;
    commandCodeUnit = *commandCursor;
    commandStream = commandCursor + 1;
    if (commandCodeUnit == 0) {
      nestedDepth--;
      /* resume behind the nested-stream command's payload */
      commandStream = Asset_RecordAt<uint16_t>(nestedReturnStack[nestedDepth],RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    /* Plain code units take the same path as command 0 (copy one word). */
    switch(((short)commandCodeUnit < 0) ? (commandCodeUnit & RICHTEXT_OPCODE_MASK) : 0) {
    default:
      if (remainingWords != 0) {
        *outputCursor = commandCodeUnit;
        remainingWords--;
        outputCursor++;
      }
      break;
    case RICHTEXT_OP_LITERAL_COLOR:
      /* without room for the whole record only the command code unit is skipped */
      if (RICHTEXT_RECORD_UNITS_LITERAL_COLOR < remainingWords) {
        *outputCursor = commandCodeUnit;
        remainingWords = remainingWords - RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        /* payload dwords, 2-byte aligned */
        Thandor_StoreU32(outputCursor + 1,Thandor_LoadU32(commandStream));
        Thandor_StoreU32(outputCursor + 3,Thandor_LoadU32(commandCursor + 3));
        Thandor_StoreU32(outputCursor + 5,Thandor_LoadU32(commandCursor + 5));
        Thandor_StoreU32(outputCursor + 7,Thandor_LoadU32(commandCursor + 7));
        outputCursor = outputCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        commandStream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      }
      break;
    /* dropped; only the command code unit is skipped, so an inline value's payload units follow as code
       units of their own */
    case RICHTEXT_OP_UNUSED_07:
    case RICHTEXT_OP_UNUSED_13:
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
    case RICHTEXT_OP_UNUSED_17:
    case RICHTEXT_OP_UNUSED_1B:
    case RICHTEXT_OP_UNUSED_1C:
    case RICHTEXT_OP_UNUSED_1D:
    case RICHTEXT_OP_UNUSED_1E:
    case RICHTEXT_OP_UNUSED_1F:
      break;
    case RICHTEXT_OP_CALL_NESTED:
      if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
        break;
      }
      if (THANDOR_PTR32_AT(uint16_t, commandStream) == nullptr) {
        RichTextCommandStream_SkipNullNestedStream(&commandStream);
        break;
      }
      nestedReturnStack[nestedDepth] = commandStream;
      nestedDepth++;
      /* fall through: enter the nested stream */
    case RICHTEXT_OP_JUMP_NESTED:
      if (THANDOR_PTR32_AT(uint16_t, commandStream) == nullptr) {
        RichTextCommandStream_SkipNullNestedStream(&commandStream);
        break;
      }
      commandStream = THANDOR_PTR32_AT(uint16_t, commandStream);
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      if (RICHTEXT_RECORD_UNITS_INLINE_IMAGE < remainingWords) {
        *outputCursor = commandCodeUnit;
        remainingWords = remainingWords - RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        /* payload dwords, 2-byte aligned */
        Thandor_StoreU32(outputCursor + 1,Thandor_LoadU32(commandStream));
        Thandor_StoreU32(outputCursor + 3,Thandor_LoadU32(commandCursor + 3));
        outputCursor = outputCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        commandStream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
  }
  if (remainingWords == 0) {
    /* The original writes the terminator behind a completely filled buffer; bounded here because that unit lies
       past the 16 KiB allocation: the terminator replaces the last unit instead. */
    outputCursor--;
  }
  *outputCursor = 0;
  g_RichTextRuntimeBufferUsedWords = 0;
}

/* Failure tail of FatalError_CopyRichTextToNarrow: terminates the cut output on the last byte written
   (destination points behind it) and reports the failure. */
static int FatalError_TerminateCutNarrowText(uint8_t *destination)
{
  destination[-1] = 0;
  return FATAL_ERROR_GENERAL_FAILURE;
}

/* Converts a rich-text command stream into plain narrow text for the fatal-error MessageBoxA: glyphs below
   0x100 are copied as bytes, fixed spaces become ' ', line breaks CR LF, nested streams are followed and
   every other command is skipped. Returns the bytes written including the terminator, or
   FATAL_ERROR_GENERAL_FAILURE (output cut and terminated) when capacityBytes runs out.
*/
int FatalError_CopyRichTextToNarrow
              (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source)

{
  uint16_t *command;
  uint16_t *record;
  uint16_t *operand;
  uint16_t codeUnit;
  uint32_t remainingCapacityBytes;
  Bool8 newlineCapacityUnderflow;
  uint16_t *nestedReturnStack[FATAL_ERROR_RICHTEXT_NESTING_MAX]; /* return points of nested texts (the original keeps them on its call stack) */
  int nestedDepth;

  nestedDepth = 0;
  remainingCapacityBytes = capacityBytes;
  command = source;
  /* a 0 code unit ends the current stream; the outermost one ends the text */
  while (*command != 0 || nestedDepth != 0) {
    codeUnit = *command;
    if (codeUnit == 0) {
      /* end of a nested stream: continue behind the payload of its call record */
      command = Asset_RecordAt<uint16_t>(nestedReturnStack[--nestedDepth],RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    record = command;
    operand = record + 1;
    /* by default a code unit (glyph or unknown command) is one unit long */
    command = operand;
    if ((short)codeUnit < 0) {
      switch(codeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        command = record + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_FIXED_SPACE:
        remainingCapacityBytes--;
        if (remainingCapacityBytes == 0) {
          return FatalError_TerminateCutNarrowText(destination);
        }
        *destination = ' ';
        destination++;
        break;
      case RICHTEXT_OP_LINE_BREAK:
        newlineCapacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (newlineCapacityUnderflow || remainingCapacityBytes == 0) {
          return FatalError_TerminateCutNarrowText(destination);
        }
        destination[0] = '\r';
        destination[1] = '\n';
        destination = destination + 2;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        command = record + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
        if (nestedDepth == FATAL_ERROR_RICHTEXT_NESTING_MAX) {
          return FatalError_TerminateCutNarrowText(destination);
        }
        nestedReturnStack[nestedDepth++] = operand;
        command = THANDOR_PTR32_AT(uint16_t, operand);
        break;
      case RICHTEXT_OP_JUMP_NESTED:
        command = THANDOR_PTR32_AT(uint16_t, operand);
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        command = record + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        break;
      }
    }
    else if ((codeUnit & 0xff00) == 0) { /* only glyphs that fit a narrow character */
      remainingCapacityBytes--;
      if (remainingCapacityBytes == 0) {
        return FatalError_TerminateCutNarrowText(destination);
      }
      *destination = (uint8_t)codeUnit;
      destination++;
    }
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return capacityBytes - (remainingCapacityBytes - 1);
  }
  return FatalError_TerminateCutNarrowText(destination);
}
