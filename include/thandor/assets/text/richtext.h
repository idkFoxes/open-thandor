#ifndef THANDOR_ASSETS_TEXT_RICHTEXT_H
#define THANDOR_ASSETS_TEXT_RICHTEXT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/text/richtext. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041D300 */
RichTextExtentRegs RichTextCommandStream_MeasureWrappedBlockRegs (dword packedStyle,word *commandStream,UiPixelExtent maximumWidth);

/* 0x0041D7C0 */
void RichTextCommandStream_DrawWrappedBlockCf (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,dword packedStyle,word *commandStream, UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX);

/* 0x0041D4A0 */
undefined8 RichTextCommandStream_DrawSingleLine (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPackedTextStyle packedStyle,word *commandStream, UiPixelCoordinate drawX,UiPixelCoordinate baselineY);

/* 0x0041B100 */
undefined8 RichTextCommandStream_PatchPayloadBySelector (RichTextCommandSelector selector,void *replacementPayload,word *stream);

/* 0x0041B200 */
void RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,word *stream);

/* 0x0041B300 */
undefined8 __fastcall RichTextCommandStream_FindNthCommandPayloadPair (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,ushort *param_5);

/* 0x0041B420 */
undefined4 RichTextCommandStream_FindNestedStreamPointer(undefined4 param_1,ushort *param_2);

/* 0x0041B520 */
undefined4 RichTextCommandStream_FindOpcode1APayloadPair(undefined4 param_1,undefined4 param_2,ushort *param_3);

/* 0x0041B620 */
undefined4 RichTextCommandStream_FindInlinePayloadPair(undefined4 param_1,ushort *param_2);

/* 0x0041B720 */
undefined8 __fastcall RichTextCommandStream_FindNthCommandFlagsPair (undefined4 param_1,undefined4 param_2,int param_3,uint param_4,uint *param_5);

/* 0x0041B840 */
ulonglong __fastcall RichTextCommandStream_QueryNthCommandFlags (undefined4 param_1,uint param_2,int param_3,ushort *param_4);

/* 0x0041B950 */
int RichTextCommandStream_CopyToNarrowCf (TextOutputCapacityBytes capacityBytes,byte *destination,word *source);

/* 0x0041BCB0 */
undefined8 __fastcall RichTextMarkup_ParseAndBuildStringAsset(undefined4 param_1,undefined4 param_2,byte *param_3);

/* 0x0041C8D0 */
int RichTextCommandStream_CopyExpandedCf (TextOutputCapacityBytes capacityBytes,word *destination,word *source);

/* 0x0041CF30 */
RichTextExtentRegs RichTextCommandStream_MeasureRegs(UiPackedTextStyle packedStyle,word *commandStream);

/* 0x0041D0F0 */
undefined8 RichTextCommandStream_MeasureNextWrappedLineCf(UiPixelExtent maximumWidth);

/* 0x0041D9F0 */
undefined8 RichTextCommandStream_DrawNextWrappedLineCf (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelExtent maximumWidth,UiPixelCoordinate drawY, UiPixelCoordinate drawX);

/* 0x0041D840 */
void RichTextCommandStream_FlattenNestedToRuntimeBuffer(word *commandStream);

#endif /* THANDOR_ASSETS_TEXT_RICHTEXT_H */
