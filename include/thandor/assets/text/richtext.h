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
          UiPixelCoordinate drawX,UiPixelCoordinate baselineY);

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
