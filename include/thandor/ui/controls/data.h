/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/data.h
 */

#ifndef THANDOR_UI_CONTROLS_DATA_H
#define THANDOR_UI_CONTROLS_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern pointer g_Utf16StringCompareAsciiCaseInsensitiveFlags; /* 004027BC g_Utf16StringCompareAsciiCaseInsensitiveFlags */

extern FileSystemEnumerateDirectoryOrVolumeEntriesProc *g_FileSystemEnumerateDirectoryOrVolumeEntries; /* 0040B214 g_FileSystemEnumerateDirectoryOrVolumeEntries */

extern FileSystemValidateDos83Proc *g_FileSystemValidateDos83Path; /* 0040B218 g_FileSystemValidateDos83Path */

extern uint8_t g_UiTimedListDriveLetters[32]; /* 0040F530 g_UiTimedListDriveLetters */

extern WidePathBuffer256 g_UiTimedListRecordPathScratch; /* 0040F550 g_UiTimedListRecordPathScratch */

extern WidePathBuffer256 g_UiTimedListCombinedPathScratch; /* 0040F750 g_UiTimedListCombinedPathScratch */

extern WidePathBuffer256 g_UiTimedListSecondaryPathScratch; /* 0040F950 g_UiTimedListSecondaryPathScratch */

extern WidePathBuffer256 g_UiTimedListHierarchyPathScratch; /* 0040FB50 g_UiTimedListHierarchyPathScratch */

extern WidePathBuffer256 g_UiTimedListHierarchyParentPathScratch; /* 0040FD50 g_UiTimedListHierarchyParentPathScratch */

extern uint16_t g_WildcardAllFilesUtf16[4]; /* 0040FF50 g_WildcardAllFilesUtf16 */

extern uint16_t g_UiTimedListDriveWildcardUtf16[7]; /* 0040FF58 g_UiTimedListDriveWildcardUtf16 */

extern uint32_t g_CursorUseOverridePosition; /* 00416818 g_CursorUseOverridePosition */

extern UiPointerWheelDelta g_CursorWheelDelta; /* 00416830 g_CursorWheelDelta */

extern SoundPlayVoiceProc *g_SoundPlayOneShot; /* 00417348 g_SoundPlayOneShot */

extern SoftwareBgraWordLanes g_UiScalerSecondPixelWeights[256]; /* 0041F720 g_UiScalerSecondPixelWeights */

extern SoftwareBgraWordLanes g_UiScalerFirstPixelWeights[256]; /* 00420720 g_UiScalerFirstPixelWeights */

extern UiNodeVtable g_UiGraphicsAdapterTextButtonVtable; /* 00422720 g_UiGraphicsAdapterTextButtonVtable */

extern uint16_t g_GraphicsAdapterFormatScratch0Utf16[16]; /* 00422768 g_GraphicsAdapterFormatScratch0Utf16 */

extern uint16_t g_GraphicsAdapterFormatScratch1Utf16[16]; /* 00422788 g_GraphicsAdapterFormatScratch1Utf16 */

extern DisplaySettingsUiImage g_UiDisplaySettingsRootTemplate; /* 004229B4 g_UiDisplaySettingsRootTemplate */

extern UiDisplayModeSelectionActionHandlerTable g_UiDisplayModeSelectionActionHandlers20; /* 00423588 g_UiDisplayModeSelectionActionHandlers20 */

/* 004235D8 g_UiDisplayModeDistinctValueScratch: the ascending list of distinct values (bit depths,
   resolutions, adapters) that UiDisplaySettings_OpenAndPopulateModeSelection sorts in, 0xFFFFFFFF = empty */
extern DisplayModeScratchWord g_UiDisplayModeDistinctValueScratch[8];

extern SoftwareFramebufferAccess *g_FramebufferAccess; /* 004A8E70 g_FramebufferAccess */

extern uint32_t g_FramebufferRowStrideBytes; /* 004A8E84 g_FramebufferRowStrideBytes */

extern uint32_t g_FramebufferHeight; /* 004A8E8C g_FramebufferHeight */

extern GraphicsFramebufferPresentProc *g_GraphicsFramebufferPresent; /* 004A8EE0 g_GraphicsFramebufferPresent */

extern GraphicsFramebufferBeginAccessProc *g_GraphicsFramebufferBeginAccess; /* 004A8EEC g_GraphicsFramebufferBeginAccess */

extern GraphicsFramebufferEndAccessProc *g_GraphicsFramebufferEndAccess; /* 004A8EF0 g_GraphicsFramebufferEndAccess */

extern GraphicsTextureSourceGetLogicalSizeProc *g_GraphicsTextureSourceGetLogicalSize; /* 004A8EF4 g_GraphicsTextureSourceGetLogicalSize */

extern GraphicsTextureSourceTestOpaquePixelProc *g_GraphicsTextureSourceTestOpaquePixel; /* 004A8EF8 g_GraphicsTextureSourceTestOpaquePixel */

extern GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitSourceAlpha; /* 004A8EFC g_GraphicsTextureSourceBlitSourceAlpha */

extern GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledSourceAlpha; /* 004A8F00 g_GraphicsTextureSourceBlitTiledSourceAlpha */

extern GraphicsTextureSourceBlitModulatedSourceAlphaProc *g_GraphicsTextureSourceBlitModulatedSourceAlpha; /* 004A8F18 g_GraphicsTextureSourceBlitModulatedSourceAlpha */

extern GraphicsFramebufferFillRectArgbProc *g_GraphicsFramebufferFillRectArgb; /* 004A8F30 g_GraphicsFramebufferFillRectArgb */

extern RuntimeSpinLockValue *g_UiRuntimeFrameLock; /* 004AE984 g_UiRuntimeFrameLock */

extern UiRuntimePostUnlockCallbackProc *g_UiRuntimePostUnlockCallback; /* 004AE988 g_UiRuntimePostUnlockCallback */

extern UiTooltipState g_UiTooltipState; /* 004AF1E0 g_UiTooltipState */

extern uint32_t g_UiPendingFrameTicks; /* 004AF1F0 g_UiPendingFrameTicks */

extern uint32_t g_UiInvalidationSuppressed; /* 004AF208 g_UiInvalidationSuppressed */

extern UiRootNode *g_UiRootNode; /* 004B0E30 g_UiRootNode */

extern GraphicsTextureSourceAsset *g_UiWindowTextureSource; /* 004B0E34 g_UiWindowTextureSource */

extern GraphicsTextureSourceAsset *g_UiWindowClassTextureSource; /* 004B0E38 g_UiWindowClassTextureSource */

extern UiNodeBase *g_UiPointerCaptureTarget; /* 004B0E3C g_UiPointerCaptureTarget */

extern UiNodeBase *g_UiKeyboardFocusNode; /* 004B0E40 g_UiKeyboardFocusNode */

extern AudioMixerGainQ15 g_UiSoundGainQ15; /* 004B0E44 g_UiSoundGainQ15 */

extern UiFrameDelayFrames g_UiTooltipDelayFrames; /* 004B0E48 g_UiTooltipDelayFrames */

extern uint32_t g_UiTooltipTextStyle; /* 004B0E4C g_UiTooltipTextStyle */

extern int32_t g_UiScrollWheelDefaultStep; /* 004B0E50 g_UiScrollWheelDefaultStep: int32_t, 14: pixels scrolled per mouse-wheel step in a scrollable control whose child is not a list (UiScrollableControl wheel handler, src/ui/controls/lists.c). */

extern int32_t g_UiScrollWheelListStep; /* 004B0E54 g_UiScrollWheelListStep: int32_t, 15: pixels per mouse-wheel step when the scrollable control's child is a list/text list/timed list control (src/ui/controls/lists.c). */

extern int32_t g_UiRangeSliderDragScale; /* 004B0E58 g_UiRangeSliderDragScale: int32_t, 1: multiplier of wheelDelta * stepValue when the mouse wheel moves a range slider (src/ui/controls/input.c). */

extern UiFrameDelayFrames g_UiTimedListActionDelayFrames; /* 004B0E5C g_UiTimedListActionDelayFrames */

extern UiFrameDelayFrames g_UiListActivationPulseFrames; /* 004B0E60 g_UiListActivationPulseFrames: UiFrameDelayFrames, 8: frames of the activation pulse after Enter on a list/text list before its action is queued (src/ui/controls/lists.c, text.c). */

extern int32_t g_UiResizableWindowTitleTextTopOffset; /* 004B0E64 g_UiResizableWindowTitleTextTopOffset: int32_t, 5: pixels from the window top to the title text line of a resizable window (src/ui/controls/layout.c). */

extern UiPackedTextStyle g_UiResizableWindowTitleTextStyle; /* 004B0E68 g_UiResizableWindowTitleTextStyle: UiPackedTextStyle, 2: packed rich-text style of the resizable window title (src/ui/controls/layout.c). */

extern int32_t g_UiWindowMoveHandleWidth; /* 004B0E6C g_UiWindowMoveHandleWidth: int32_t, 19 (0x13): height in pixels of the top strip that drags a movable root window (src/ui/controls/layout.c). */

extern int32_t g_UiWindowResizeBorderThickness; /* 004B0E70 g_UiWindowResizeBorderThickness */

extern UiPackedTextStyle g_UiTextStyleSelected; /* 004B0E74 g_UiTextStyleSelected: UiPackedTextStyle, 0x10000 (palette byte 1): text style of the selected/highlighted row or item (src/ui/controls/text.c). */

extern uint32_t g_UiTextStyleNormal; /* 004B0E78 g_UiTextStyleNormal */

extern UiPackedTextStyle g_UiTextStyleDisabled; /* 004B0E7C g_UiTextStyleDisabled: UiPackedTextStyle, 0x20000 (palette byte 2): text style of disabled items (src/ui/controls/text.c). */

extern uint32_t g_UiTextStyleAlternate; /* 004B0E80 g_UiTextStyleAlternate */

extern uint32_t g_UiWindowTitleTextStyle; /* 004B0E88 g_UiWindowTitleTextStyle */

extern int32_t g_UiWindowFrameInset; /* 004B0E8C g_UiWindowFrameInset */

extern uint32_t g_UiListTextStyle; /* 004B0E90 g_UiListTextStyle */

extern uint32_t g_UiTextEditActiveTextStyle; /* 004B0E98 g_UiTextEditActiveTextStyle */

extern uint32_t g_UiTextEditInactiveTextStyle; /* 004B0E9C g_UiTextEditInactiveTextStyle */

extern uint32_t g_UiTextEditDisabledTextStyle; /* 004B0EA0 g_UiTextEditDisabledTextStyle */

extern UiFrameDelayFrames g_UiTextEditCaretBlinkPhaseStep; /* 004B0EA4 g_UiTextEditCaretBlinkPhaseStep: UiFrameDelayFrames, 8: frames per caret blink phase of a focused text edit, reloaded into the counter byte of editStateFlags (src/ui/controls/text.c). */

extern int32_t g_UiHorizontalGaugeLabelTopInset; /* 004B0EA8 g_UiHorizontalGaugeLabelTopInset: int32_t, 4: pixels from the gauge top to its label line (src/ui/controls/layout.c). */

extern uint32_t g_UiHorizontalGaugeLabelTextStyle; /* 004B0EAC g_UiHorizontalGaugeLabelTextStyle */

extern uint16_t g_UiWindowClassTexturePathUtf16[20]; /* 004B0EB8 g_UiWindowClassTexturePathUtf16 */

extern uint16_t g_UiWindowClassTextPathUtf16[19]; /* 004B0EE0 g_UiWindowClassTextPathUtf16 */

extern uint16_t g_UiWindowTexturePathUtf16[15]; /* 004B0F06 g_UiWindowTexturePathUtf16 */

extern UiPointerCaptureButton g_UiPointerCaptureButton; /* 004B0F24 g_UiPointerCaptureButton */

extern UiImageControl * g_UiImageControlHoverTarget; /* 004B0F28 g_UiImageControlHoverTarget */

extern UiNodeVtable g_UiSpriteButtonControlVtable; /* 004B15D0 g_UiSpriteButtonControlVtable */

extern UiNodeVtable g_UiNodeVtable_004B1D80; /* 004B1D80 g_UiNodeVtable_004B1D80 (framed text button) */

extern UiNodeVtable g_UiWindowControlVtable; /* 004B2740 g_UiWindowControlVtable */

extern UiNodeVtable g_UiNodeVtable_004B2CE0; /* 004B2CE0 g_UiNodeVtable_004B2CE0 (text button) */

extern UiNodeVtable g_UiTitledWindowControlVtable; /* 004B33D0 g_UiTitledWindowControlVtable */

extern UiNodeVtable g_UiImagePanelControlVtable; /* 004B3770 g_UiImagePanelControlVtable */

extern UiNodeVtable g_UiFillPanelControlVtable; /* 004B3A50 g_UiFillPanelControlVtable */

extern UiNodeVtable g_UiHorizontalGaugeControlVtable; /* 004B3C20 g_UiHorizontalGaugeControlVtable */

extern uint16_t g_UiWindowPercentTextUtf16[5]; /* 004B3C68 g_UiWindowPercentTextUtf16 */

extern UiNodeVtable g_UiRangeSliderControlVtable; /* 004B3EF0 g_UiRangeSliderControlVtable */

extern UiNodeVtable g_UiLayoutContainerControlVtable; /* 004B4650 g_UiLayoutContainerControlVtable */

extern UiNodeVtable g_UiPanelControlVtable; /* 004B4950 g_UiPanelControlVtable */

extern UiNodeVtable g_UiResizableWindowControlVtable; /* 004B4CC0 g_UiResizableWindowControlVtable */

extern UiNodeVtable g_UiNumericTextEditControlVtable; /* 004B58A0 g_UiNumericTextEditControlVtable */

extern UiNodeVtable g_UiPathTextEditControlVtable; /* 004B6800 g_UiPathTextEditControlVtable */

extern UiNodeVtable g_UiRequiredTextEditControlVtable; /* 004B7050 g_UiRequiredTextEditControlVtable */

extern UiNodeVtable g_UiScrollableControlVtable; /* 004B7920 g_UiScrollableControlVtable */

extern UiNodeVtable g_UiFocusProxyControlVtable; /* 004B9530 g_UiFocusProxyControlVtable */

extern UiNodeVtable g_UiTextListControlVtable; /* 004B9E40 g_UiTextListControlVtable */

extern UiNodeVtable g_UiListControlVtable; /* 004BA590 g_UiListControlVtable */

extern uint16_t g_UiPointerListExpandedLeftTextUtf16[512]; /* 004BA5D8 g_UiPointerListExpandedLeftTextUtf16: 1 KiB expansion scratch of UiPointerList_CompareExpandedText */

extern uint16_t g_UiPointerListExpandedRightTextUtf16[512]; /* 004BA9D8 g_UiPointerListExpandedRightTextUtf16: 1 KiB expansion scratch of UiPointerList_CompareExpandedText */

extern UiNodeVtable g_UiTimedListControlVtable; /* 004BB990 g_UiTimedListControlVtable */

extern UiNodeVtable g_UiListOffsetControlVtable; /* 004BC410 g_UiListOffsetControlVtable */

extern UiNodeVtable g_UiNodeVtable_004BC570; /* 004BC570 g_UiNodeVtable_004BC570 */

extern UiNodeVtable g_UiNineSlicePanelControlVtable; /* 004BCC30 g_UiNineSlicePanelControlVtable */

extern UiNodeVtable g_UiImageActionControlVtable; /* 00514FC0 g_UiImageActionControlVtable */

extern UiNodeVtable g_UiConditionalActionControlVtable; /* 00515290 g_UiConditionalActionControlVtable */

extern UiNodeVtable g_UiNumericPairTextButtonVtable; /* 00515610 g_UiNumericPairTextButtonVtable */

extern uint16_t g_UiNumericPairFirstValueScratchUtf16[16]; /* 00515658 g_UiNumericPairFirstValueScratchUtf16 */

extern uint16_t g_UiNumericPairSecondValueScratchUtf16[16]; /* 00515678 g_UiNumericPairSecondValueScratchUtf16 */

extern UiNodeVtable g_UiPayloadPairTextButtonVtable; /* 00515730 g_UiPayloadPairTextButtonVtable */

extern UiNodeVtable g_UiFormattedContainerVtable; /* 005157E0 g_UiFormattedContainerVtable */

extern UiNodeVtable g_UiSelectionGeometryControlVtable; /* 00515C70 g_UiSelectionGeometryControlVtable */

extern uint16_t g_UiCatalogEntryRichTextScratchUtf16[16]; /* 00516510 g_UiCatalogEntryRichTextScratchUtf16 */

extern UiNodeVtable g_UiNodeVtable_00516530; /* 00516530 g_UiNodeVtable_00516530 */

extern UiNodeVtable g_UiArmyMetricsPanelVtable; /* 00516CC0 g_UiArmyMetricsPanelVtable */

extern UiNodeVtable g_UiTransferProgressGaugeVtable; /* 00517DE0 g_UiTransferProgressGaugeVtable: UiHorizontalGaugeControl subclass of the transfer progress gauge; followed by 0x90 code alignment fill */

extern UiNodeVtable g_UiCommandVisibilityWrappedTextVtable; /* 00517F10 g_UiCommandVisibilityWrappedTextVtable; followed by 0x90 code alignment fill */

extern UiNodeVtable g_UiCommandVisibilitySingleLineTextVtable; /* 00517FC0 g_UiCommandVisibilitySingleLineTextVtable; followed by 0x90 code alignment fill */

extern UiNodeVtable g_UiSoftwareTexturePreviewControlVtable; /* 00518C90 g_UiSoftwareTexturePreviewControlVtable; followed by 0x90 code alignment fill */

extern uint32_t g_UiCatalogGroup48ColumnCount; /* 00562648 g_UiCatalogGroup48ColumnCount */

extern uint32_t g_UiCatalogGroup42ColumnCount; /* 0056264C g_UiCatalogGroup42ColumnCount */

extern int32_t *g_UiCatalogGroup48OffsetTables[9]; /* 00562654 g_UiCatalogGroup48OffsetTables */

extern int32_t *g_UiCatalogGroup42OffsetTables[7]; /* 00562678 g_UiCatalogGroup42OffsetTables */

extern int32_t g_UiCatalogGroup48OffsetsDefault[48]; /* 005626A8 g_UiCatalogGroup48OffsetsDefault */

extern int32_t g_UiCatalogGroup48Offsets5Columns[48]; /* 00562768 g_UiCatalogGroup48Offsets5Columns */

extern int32_t g_UiCatalogGroup48Offsets6Columns[48]; /* 00562828 g_UiCatalogGroup48Offsets6Columns */

extern int32_t g_UiCatalogGroup48Offsets7Columns[48]; /* 005628E8 g_UiCatalogGroup48Offsets7Columns */

extern int32_t g_UiCatalogGroup48Offsets8Columns[48]; /* 005629A8 g_UiCatalogGroup48Offsets8Columns */

extern int32_t g_UiCatalogGroup42OffsetsDefault[42]; /* 00562A68 g_UiCatalogGroup42OffsetsDefault */

extern int32_t g_UiCatalogGroup42Offsets5Columns[42]; /* 00562B10 g_UiCatalogGroup42Offsets5Columns */

extern int32_t g_UiCatalogGroup42Offsets6Columns[42]; /* 00562BB8 g_UiCatalogGroup42Offsets6Columns */

extern UiFrameRefreshCountdownFrames g_DirectInputMouseRefreshCountdown; /* 00576C24 g_DirectInputMouseRefreshCountdown */

#endif
