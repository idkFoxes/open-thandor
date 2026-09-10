# Call graph: `assets/text/richtext`

18 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0041D300` **RichTextCommandStream_MeasureWrappedBlockRegs** — local: `RichTextCommandStream_FlattenNestedToRuntimeBuffer`, `RichTextCommandStream_MeasureNextWrappedLineCf`
- `0x0041D7C0` **RichTextCommandStream_DrawWrappedBlockCf** — local: `RichTextCommandStream_FlattenNestedToRuntimeBuffer`, `RichTextCommandStream_DrawNextWrappedLineCf`
- `0x0041D4A0` **RichTextCommandStream_DrawSingleLine** — local: `RichTextCommandStream_MeasureRegs`; cross: `FontGlyph_DrawBottomAligned` → `assets/text/resources`
- `0x0041B100` **RichTextCommandStream_PatchPayloadBySelector**
- `0x0041B200` **RichTextCommandStream_BindTextureSource**
- `0x0041B300` **RichTextCommandStream_FindNthCommandPayloadPair**
- `0x0041B420` **RichTextCommandStream_FindNestedStreamPointer**
- `0x0041B520` **RichTextCommandStream_FindOpcode1APayloadPair**
- `0x0041B620` **RichTextCommandStream_FindInlinePayloadPair**
- `0x0041B720` **RichTextCommandStream_FindNthCommandFlagsPair**
- `0x0041B840` **RichTextCommandStream_QueryNthCommandFlags**
- `0x0041B950` **RichTextCommandStream_CopyToNarrowCf**
- `0x0041BCB0` **RichTextMarkup_ParseAndBuildStringAsset**
- `0x0041C8D0` **RichTextCommandStream_CopyExpandedCf**
- `0x0041CF30` **RichTextCommandStream_MeasureRegs** — cross: `FontGlyph_GetLogicalSizeActiveRegs` → `assets/text/resources`
- `0x0041D0F0` **RichTextCommandStream_MeasureNextWrappedLineCf** — cross: `FontGlyph_GetLogicalSizeActiveRegs` → `assets/text/resources`
- `0x0041D9F0` **RichTextCommandStream_DrawNextWrappedLineCf** — cross: `FontGlyph_GetLogicalSizeActiveRegs` → `assets/text/resources`, `FontGlyph_DrawVerticallyCentered` → `assets/text/resources`
- `0x0041D840` **RichTextCommandStream_FlattenNestedToRuntimeBuffer**
