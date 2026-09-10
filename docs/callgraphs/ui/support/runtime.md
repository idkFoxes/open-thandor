# Call graph: `ui/support/runtime`

6 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0050F220` **RecentTextHistory_SortAndBuildPointerList** — local: `RecentTextHistory_SwapSlots`
- `0x0050F130` **RecentTextHistory_Insert** — cross: `RichTextCommandStream_CopyExpandedCf` → `assets/text/richtext`
- `0x0050F2E0` **RecentTextHistory_RemoveOldest**
- `0x00548EC0` **CreditsScreen_Open** — cross: `UiFrame_FlushInputAndResetPendingTicks` → `ui/controls/layout`, `SoftwareMaskBuffer_Clear` → `graphics/backend/software`, `UiPageStack_SetActiveIndex` → `ui/controls/layout`
- `0x0054D5D0` **PcxPreview_Load64x64PaletteAndPixelsCf** — cross: `WidePath_CombineDirectoryAndLeaf` → `core/text/path`, `WidePath_SetExtensionCode` → `core/text/path`, `Resource_Load` → `assets/resource/runtime`, `Resource_Release` → `assets/resource/runtime`
- `0x0050F1A0` **RecentTextHistory_SwapSlots**
