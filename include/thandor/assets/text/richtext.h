/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/richtext.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_RICHTEXT_H
#define THANDOR_ASSETS_TEXT_RICHTEXT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/text/richtext. */

/* Rich-text command streams (the engine's UTF-16 text in 'str' assets): a zero code unit ends the stream, a
   positive code unit is a glyph (the font's subresource index, i.e. the character), and a code unit with bit 15
   set is a command whose low five bits are the opcode. Some commands carry payload code units; the
   RICHTEXT_RECORD_UNITS_* values are the full record lengths including the command code unit. The opcodes are
   taken from the interpreters in richtext.c and TextResourcePage_Load. */
#define RICHTEXT_COMMAND_FLAG 0x8000
#define RICHTEXT_OPCODE_MASK 0x1F
#define RICHTEXT_OP_COLOR_PALETTE_0 0x00 /* 0x00..0x03: colour and shadow from palette entry 0..3 */
#define RICHTEXT_OP_COLOR_PALETTE_1 0x01
#define RICHTEXT_OP_COLOR_PALETTE_2 0x02
#define RICHTEXT_OP_COLOR_PALETTE_3 0x03
#define RICHTEXT_OP_SAVE_COLOR 0x04
#define RICHTEXT_OP_RESTORE_COLOR 0x05
#define RICHTEXT_OP_LITERAL_COLOR 0x06 /* eight payload code units, one ARGB nibble each */
#define RICHTEXT_OP_SELECT_FONT_FIRST 0x08 /* 0x08..0x0F: the font index is the opcode & 0xF */
#define RICHTEXT_OP_FIXED_SPACE 0x10 /* a space glyph that is no wrap opportunity */
#define RICHTEXT_OP_SOFT_HYPHEN 0x11
#define RICHTEXT_OP_LINE_BREAK 0x12
#define RICHTEXT_OP_INLINE_VALUE_0 0x14 /* 0x14..0x16: one 32-bit payload (flags/variant in its low bits) */
#define RICHTEXT_OP_INLINE_VALUE_1 0x15
#define RICHTEXT_OP_INLINE_VALUE_2 0x16
#define RICHTEXT_OP_CALL_NESTED 0x18 /* payload: nested stream pointer, 32-bit selector; returns afterwards */
#define RICHTEXT_OP_JUMP_NESTED 0x19 /* payload as 0x18, but continues in the nested stream without return */
#define RICHTEXT_OP_INLINE_IMAGE 0x1A /* payload: texture source pointer, 32-bit subresource */
/* Opcodes no interpreter handles (skipped as a single code unit; flattening drops them). 0x17 is the fourth
   inline-value variant, which carries no payload. */
#define RICHTEXT_OP_UNUSED_07 0x07
#define RICHTEXT_OP_UNUSED_13 0x13
#define RICHTEXT_OP_UNUSED_17 0x17
#define RICHTEXT_OP_UNUSED_1B 0x1B
#define RICHTEXT_OP_UNUSED_1C 0x1C
#define RICHTEXT_OP_UNUSED_1D 0x1D
#define RICHTEXT_OP_UNUSED_1E 0x1E
#define RICHTEXT_OP_UNUSED_1F 0x1F
/* RICHTEXT_OP_LITERAL_COLOR payload: the low nibble of each payload code unit is one hex digit of the ARGB
   value. The interpreters shift each digit in at bits 28-31 and the value right by one digit per step. */
#define RICHTEXT_COLOR_DIGIT_BITS 4
#define RICHTEXT_COLOR_DIGIT_SHIFT 28
#define RICHTEXT_RECORD_UNITS_LITERAL_COLOR 9
#define RICHTEXT_RECORD_UNITS_INLINE_VALUE 3
#define RICHTEXT_RECORD_UNITS_NESTED 5 /* RICHTEXT_OP_CALL_NESTED and RICHTEXT_OP_JUMP_NESTED */
#define RICHTEXT_RECORD_UNITS_INLINE_IMAGE 5
/* The 0x18 return resumes 8 bytes (the four payload code units) after the saved position. */
#define RICHTEXT_NESTED_PAYLOAD_BYTES 8
/* Code units of g_FontRuntimeBuffer that RichTextCommandStream_FlattenNestedToRuntimeBuffer fills (the terminator
   is written behind them). */
#define RICHTEXT_RUNTIME_BUFFER_UNITS 0x2000

/* TXT2STR markup (RichTextMarkup_ParseAndBuildStringAsset): '#@'..'#~' select code page (c - '@'), which adds
   (c - '@') * RICHTEXT_MARKUP_CODE_PAGE_UNITS to the following bytes; '#!' makes '@'..'_' command code units. */
#define RICHTEXT_MARKUP_CODE_PAGE_UNITS 0x80
#define RICHTEXT_MARKUP_COMMAND_BIAS (RICHTEXT_COMMAND_FLAG - '@') /* '@' + bias = RICHTEXT_COMMAND_FLAG | 0 */
/* Code unit of the "TXT2STR: unknown character" message where the byte offset is written (+0x4C). */
#define RICHTEXT_MARKUP_ERROR_OFFSET_UNIT 38

/* UiPackedTextStyle fields as the rich-text interpreters decode them. */
#define TEXT_STYLE_ALIGN_RIGHT 0x1 /* the line ends at the given x */
#define TEXT_STYLE_ALIGN_CENTER 0x2 /* the line is centred on the given x (ignored with ALIGN_RIGHT) */
#define TEXT_STYLE_PALETTE_SHIFT 16 /* bits 16-18: colour/shadow palette entry */
#define TEXT_STYLE_FONT_SHIFT 24 /* bits 24-26: font index */
#define TEXT_STYLE_INDEX_MASK 7

/* Functions are grouped by semantic ownership. */

RichTextExtent RichTextCommandStream_MeasureWrappedBlock
          (uint32_t packedStyle,uint16_t *commandStream,UiPixelExtent maximumWidth);

void RichTextCommandStream_DrawWrappedBlock
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,uint32_t packedStyle,uint16_t *commandStream,
          UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX);

bool RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPackedTextStyle packedStyle,uint16_t *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX);

void RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,uint16_t *stream);

void RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,uint16_t *stream);

bool RichTextCommandStream_CopyToNarrow
          (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source);

bool RichTextMarkup_ParseAndBuildStringAsset(uint8_t *markupBytes,void **outAsset,uint32_t *outError);

bool RichTextCommandStream_CopyExpanded
          (TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint16_t *source,
           uint32_t *outBytesWritten);

RichTextExtent RichTextCommandStream_MeasureLine(UiPackedTextStyle packedStyle,uint16_t *commandStream);

bool RichTextCommandStream_MeasureNextWrappedLine(UiPixelExtent maximumWidth,UiPixelExtent *lineHeight);

bool RichTextCommandStream_DrawNextWrappedLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX,UiPixelExtent *lineAdvance);

void RichTextCommandStream_FlattenNestedToRuntimeBuffer(uint16_t *commandStream);

#endif /* THANDOR_ASSETS_TEXT_RICHTEXT_H */
