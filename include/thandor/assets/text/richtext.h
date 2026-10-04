/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/richtext.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_RICHTEXT_H
#define THANDOR_ASSETS_TEXT_RICHTEXT_H

#include <thandor/assets/text/types.h>
#include <thandor/core/text/types.h>
#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/core/contracts.h>

/* Rich-text command streams (the engine's UTF-16 text in 'str' assets): a zero code unit ends the stream, a
   positive code unit is a glyph (the font's subresource index, i.e. the character), and a code unit with bit 15
   set is a command whose low five bits are the opcode. Some commands carry payload code units; the
   RICHTEXT_RECORD_UNITS_* values are the full record lengths including the command code unit. The opcodes are
   taken from the interpreters in assets/text/richtext.cpp, ui/text/richtext_render.cpp and TextResourcePage_Load. */
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
   is written behind them, or over the last one when all are used: the allocation holds exactly these units). */
#define RICHTEXT_RUNTIME_BUFFER_UNITS 0x2000

/* Depth of the machine-stack return chains the original keeps for nested (0x18) streams. */
#define RICHTEXT_NESTING_LIMIT 64

void RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,uint16_t *stream);

void RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,uint16_t *stream);

Bool8 RichTextCommandStream_CopyToNarrow
          (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source);

Bool8 RichTextCommandStream_CopyExpanded
          (TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint16_t *source,
           uint32_t *outBytesWritten);

void RichTextCommandStream_FlattenNestedToRuntimeBuffer(uint16_t *commandStream);

extern uint8_t *g_FontRuntimeBuffer;
extern uint32_t g_RichTextRuntimeBufferUsedWords;

/* FatalError_CopyRichTextToNarrow: nested rich-text streams it follows at most (deeper nesting cuts the text) */
#define FATAL_ERROR_RICHTEXT_NESTING_MAX 64

int FatalError_CopyRichTextToNarrow (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source);

#endif /* THANDOR_ASSETS_TEXT_RICHTEXT_H */
