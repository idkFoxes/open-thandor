# Call graph: `ui/frontend/results`

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00517020` **FrontendResultsTable_DrawColumnSequenceByType** — local: `FrontendResultsTable_DrawColourColumn`, `FrontendResultsTable_DrawEconomyColumn`, `FrontendResultsTable_DrawMilitaryColumn`, `FrontendResultsTable_DrawPointsColumn`, `FrontendResultsTable_DrawPlayerColumn`, `FrontendResultsTable_DrawFactionColumn`, `FrontendResultsTable_DrawFormattedFactionFieldColumn`; cross: `TextResource_Resolve` → `assets/text/resources`
- `0x00517FB0` **FrontendResultsTable_HitTestAlwaysNone**
- `0x005177F0` **FrontendResultsTable_DrawColourColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x005178B0` **FrontendResultsTable_DrawFactionColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x00517960` **FrontendResultsTable_DrawFormattedFactionFieldColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`
- `0x00517A30` **FrontendResultsTable_DrawPointsColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`
- `0x00517B10` **FrontendResultsTable_DrawEconomyColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`
- `0x00517BF0` **FrontendResultsTable_DrawMilitaryColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`
- `0x00517CD0` **FrontendResultsTable_DrawPlayerColumn** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
