# Call graph: `ui/controls/lists`

54 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004BBE60` **UiTimedListControl_HandleKeyboardNavigationCf** — local: `UiScrollableControl_QueryContentSizeRegs`, `UiTimedListTree_CountRecordArrayAndNestedChildren`, `UiScrollableControl_ClampOffsetsToViewport`; cross: `UiNode_DefaultKeyboardEventMoveFocusNextCf` → `ui/controls/input`
- `0x004BB100` **UiListControl_HandleKeyboardNavigationCf** — local: `UiScrollableControl_QueryContentSizeRegs`, `UiScrollableControl_ClampOffsetsToViewport`; cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_DefaultKeyboardEventMoveFocusNextCf` → `ui/controls/input`
- `0x004BB480` **UiPointerList_RefreshSelectionAndQueueAction** — local: `UiPointerList_GetSelectedIndexVariantBCf`, `UiPointerList_SelectIndexVariantB`; cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004B87A0` **UiScrollableControl_BeginPrimaryScrollInteraction** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B8A20` **UiScrollableControl_EndPrimaryScrollInteraction** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B8BF0` **UiScrollableControl_UpdatePrimaryScrollDrag** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B8F90` **UiScrollableControl_UpdateSecondaryScrollDrag** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9070` **UiScrollableControl_TickAutoScroll** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B90E0` **UiScrollableControl_HandlePointerWheel** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BA500` **UiPointerList_SelectIndexVariantA** — local: `UiScrollableControl_ClampOffsetsToViewport`
- `0x004BB020` **UiListControl_SelectRowFromPointer** — local: `UiScrollableControl_ClampOffsetsToViewport`; cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004BB7A0` **UiPointerList_SortByDwordPairFieldDescending** — local: `UiScrollableControl_ClampOffsetsToViewport`
- `0x004BB8A0` **UiPointerList_SortByDwordFieldAscending** — local: `UiScrollableControl_ClampOffsetsToViewport`
- `0x004BBCD0` **UiTimedListControl_SelectRowFromPointer** — local: `UiTimedListControl_SelectRecordAndScrollIntoView`; cross: `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`, `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004B11C0` **UiNodeList_UnsuppressActionId**
- `0x004B1200` **UiNodeList_SuppressActionId**
- `0x004B2550` **UiSelectableControl_KeyboardEventCf** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`, `UiNode_DefaultKeyboardEventMoveFocusNextCf` → `ui/controls/input`
- `0x004B26E0` **UiSelectableControl_SuppressIfActionId** — cross: `UiKeyboardFocus_ReleaseNode` → `ui/controls/input`
- `0x004B2710` **UiSelectableControl_UnsuppressIfActionId** — cross: `UiKeyboardFocus_AcquireIfNone` → `ui/controls/input`
- `0x004B2D30` **UiSelectableGroup_NoneVisibleSelectedCf**
- `0x004B2D70` **UiSelectableGroup_NoneSelectedCf**
- `0x004B2DA0` **UiSelectableGroup_SelectExclusive** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B2DE0` **UiSelectableControl_IsSelectedCf**
- `0x004B2E10` **UiSelectableControl_SetSelected** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B4920` **UiPageStack_ActivePageNotInListCf**
- `0x004B7970` **UiScrollableControl_RelocateChildren** — cross: `UiContainer_RelocateChildren` → `ui/controls/layout`
- `0x004B79D0` **UiScrollableControl_DrawFrameContentAndScrollbars** — cross: `UiWindow_BlitTiledHorizontalEdge` → `ui/controls/layout`, `UiWindow_BlitTiledVerticalEdge` → `ui/controls/layout`, `UiWindow_BlitTiledInterior` → `ui/controls/layout`, `UiContainer_DrawIntersectingChildren` → `ui/controls/layout`
- `0x004B8310` **UiScrollableControl_RebuildViewportAndScrollbars**
- `0x004B8AC0` **UiScrollableControl_QueryPointerRegion**
- `0x004B8B10` **UiScrollableControl_BeginSecondaryScrollInteraction**
- `0x004B8BC0` **UiScrollableControl_EndSecondaryScrollInteraction**
- `0x004B9000` **UiScrollableControl_HitTestContentAndScrollbars** — cross: `UiContainer_HitTestChildren` → `ui/controls/layout`
- `0x004BA4E0` **UiPointerList_GetRowSlotsVariantA**
- `0x004BA560` **UiPointerList_GetSelectedIndexVariantACf**
- `0x004BADE0` **UiListControl_DrawRowsAndSelection** — cross: `UiWindow_BlitTiledHorizontalEdge` → `ui/controls/layout`, `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x004BB310` **UiListControl_TickActivationPulse** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004BB350` **UiListControl_UnsuppressIfActionId** — cross: `UiContainer_UnsuppressActionId` → `ui/controls/layout`
- `0x004BB380` **UiListControl_SuppressIfActionId** — cross: `UiContainer_SuppressActionId` → `ui/controls/layout`
- `0x004BB3B0` **UiPointerList_InitializeColumnLayout** — cross: `FontGlyph_GetLogicalSizeForStyleRegs` → `assets/text/resources`
- `0x004BB460` **UiPointerList_GetRowSlotsVariantB**
- `0x004BB9E0` **UiTimedListControl_RelocateChildren** — cross: `UiContainer_RelocateChildren` → `ui/controls/layout`
- `0x004BBA00` **UiTimedListControl_DrawRowsAndSelection** — cross: `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`, `UiWindow_BlitTiledHorizontalEdge` → `ui/controls/layout`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x004BC180` **UiTimedListControl_TickActionDelay** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004BC460` **UiListOffsetControl_RelocateAndApplyDeferredOffset** — cross: `UiContainer_RelocateChildren` → `ui/controls/layout`
- `0x00516580` **UiCatalogEntryControl_DrawClipped** — cross: `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x00516B90` **UiCatalogEntryControl_PointerMove** — cross: `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`
- `0x00516C50` **UiCatalogEntryControl_NonRightRelease** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BC360` **UiTimedListControl_SelectRecordAndScrollIntoView** — local: `UiTimedListTree_CountRecordArrayAndNestedChildren`, `UiScrollableControl_ClampOffsetsToViewport`
- `0x004BB4E0` **UiPointerList_SelectIndexVariantB** — local: `UiScrollableControl_ClampOffsetsToViewport`
- `0x004BB540` **UiPointerList_GetSelectedIndexVariantBCf**
- `0x004B9460` **UiScrollableControl_QueryContentSizeRegs**
- `0x004BC310` **UiTimedListTree_CountRecordArrayAndNestedChildren**
- `0x004B9170` **UiScrollableControl_RefreshChildAndScrollThumbs**
- `0x004B9490` **UiScrollableControl_ClampOffsetsToViewport** — local: `UiScrollableControl_RefreshChildAndScrollThumbs`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
