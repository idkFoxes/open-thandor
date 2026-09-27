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
#define RICHTEXT_RECORD_UNITS_LITERAL_COLOR 9
#define RICHTEXT_RECORD_UNITS_INLINE_VALUE 3
#define RICHTEXT_RECORD_UNITS_NESTED 5 /* RICHTEXT_OP_CALL_NESTED and RICHTEXT_OP_JUMP_NESTED */
#define RICHTEXT_RECORD_UNITS_INLINE_IMAGE 5
/* The 0x18 return resumes 8 bytes (the four payload code units) after the saved position. */
#define RICHTEXT_NESTED_PAYLOAD_BYTES 8
/* Code units of g_FontRuntimeBuffer that RichTextCommandStream_FlattenNestedToRuntimeBuffer fills (the terminator
   is written behind them). */
#define RICHTEXT_RUNTIME_BUFFER_UNITS 0x2000

/* UiPackedTextStyle fields as the rich-text interpreters decode them. */
#define TEXT_STYLE_ALIGN_RIGHT 0x1 /* the line ends at the given x */
#define TEXT_STYLE_ALIGN_CENTER 0x2 /* the line is centred on the given x (ignored with ALIGN_RIGHT) */
#define TEXT_STYLE_PALETTE_SHIFT 16 /* bits 16-18: colour/shadow palette entry */
#define TEXT_STYLE_FONT_SHIFT 24 /* bits 24-26: font index */
#define TEXT_STYLE_INDEX_MASK 7

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041D300 */
RichTextExtentRegs __thandor_eax_edx_cf_preserve_ecx
RichTextCommandStream_MeasureWrappedBlockRegs
          (uint32_t packedStyle,uint16_t *commandStream,UiPixelExtent maximumWidth);

/* 0x0041D7C0 */
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_DrawWrappedBlock
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,uint32_t packedStyle,uint16_t *commandStream,
          UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX);

/* 0x0041D4A0 */
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPackedTextStyle packedStyle,uint16_t *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX);

/* 0x0041B100 */
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,uint16_t *stream);

/* 0x0041B200 */
void __thandor_void_preserve_eax_ecx
RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,uint16_t *stream);

/* 0x0041B300 */
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_FindNthCommandPayloadPair
          (RichTextCommandOrdinal commandOrdinal,RichTextCommandPayload32 payloadValue,
          uint16_t *commandStream);

/* 0x0041B420 */
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchNestedStreamPointerPayloads
          (RichTextNestedStreamPointerValue32 nestedStreamPointerValue,uint16_t *commandStream);

/* 0x0041B520 */
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchOpcode1APayloadPair
          (RichTextOpcode1APayloadValue32 opcode1APayloadValue,
          RichTextCommandPayload32 leadingPayloadValue,uint16_t *commandStream);

/* 0x0041B620 */
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchInlinePayloads
          (RichTextInlinePayloadValue32 inlinePayloadValue,uint16_t *commandStream);

/* 0x0041B720 */
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_FindNthCommandFlagsPair(int commandOrdinal,uint32_t flagBits,uint32_t *commandStream);

/* 0x0041B840 */
RichTextCommandQueryResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_QueryNthCommandFlags(int commandOrdinal,uint16_t *commandStream);

/* 0x0041B950 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_CopyToNarrow
          (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source);

/* 0x0041BCB0 */
RichTextAssetResult __thandor_eax_cf_preserve_edx
RichTextMarkup_ParseAndBuildStringAsset(uint8_t *markupBytes);

/* 0x0041C8D0 */
RichTextCopyResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_CopyExpanded
          (TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint16_t *source);

/* 0x0041CF30 */
RichTextExtentRegs __thandor_eax_edx_cf_preserve_ecx
RichTextCommandStream_MeasureRegs(UiPackedTextStyle packedStyle,uint16_t *commandStream);

/* 0x0041D0F0 */
WrappedLineResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_MeasureNextWrappedLine(UiPixelExtent maximumWidth);

/* 0x0041D9F0 */
WrappedLineResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_DrawNextWrappedLine
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX);

/* 0x0041D840 */
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_FlattenNestedToRuntimeBuffer(uint16_t *commandStream);

#endif /* THANDOR_ASSETS_TEXT_RICHTEXT_H */
