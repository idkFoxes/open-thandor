/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/misc.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_MISC_H
#define THANDOR_UI_CONTROLS_MISC_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/misc. */

/* Action ids of the display settings dialog (g_UiDisplaySettingsRootTemplate); action 0x200 + n runs
   g_UiDisplayModeSelectionActionHandlers20[n]. UiDisplayModeSelection_RefreshEnumeratedOptions suppresses
   the option buttons whose mode was not enumerated. */
#define UI_DISPLAY_MODE_ACTION_APPLY 0x200
#define UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH 0x201 /* 4 colour-depth buttons */
#define UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION 0x205 /* 8 resolution buttons */
#define UI_DISPLAY_MODE_ACTION_REVERT 0x20D /* "keep the new mode?" dialog: back to the previous mode */
#define UI_DISPLAY_MODE_ACTION_CANCEL 0x20E
#define UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER 0x20F /* 5 adapter buttons */
/* The display settings dialog's applyButton (a framed text button, g_UiNodeVtable_004B1D80) with extra fields in
   its tail: the selected mode tuple and colour bias/scale, then the same six values as they were when the
   dialog opened. 0x8C bytes. */
typedef struct UiDisplaySettingsApplyButton {
    UiSelectableControl selectable;
    UiTextResourceId textResourceId;
    UiPackedTextStyle packedTextStyle;
    int32_t selectedWidth;            /* +0x5C */
    int32_t selectedHeight;           /* +0x60 */
    uint32_t selectedBitsPerPixel;    /* +0x64 */
    uint32_t selectedAdapterIndex;    /* +0x68 */
    int32_t selectedColorBiasQ16;     /* +0x6C */
    int32_t selectedColorScaleQ16;    /* +0x70 */
    int32_t originalWidth;            /* +0x74 */
    int32_t originalHeight;           /* +0x78 */
    uint32_t originalBitsPerPixel;    /* +0x7C */
    uint32_t originalAdapterIndex;    /* +0x80 */
    int32_t originalColorBiasQ16;     /* +0x84 */
    int32_t originalColorScaleQ16;    /* +0x88 */
} UiDisplaySettingsApplyButton;
/* The display settings dialog's colorBiasValueText label; its tail holds the number buffers of both readouts
   (written by UiRuntime_FormatSignedValues140And144). 0x9C bytes. */
typedef struct UiDisplaySettingsValueReadout {
    UiSingleLineTextControl label;
    uint16_t colorScaleTextUtf16[16]; /* +0x5C, shown by colorScaleValueText */
    uint16_t colorBiasTextUtf16[16];  /* +0x7C */
} UiDisplaySettingsValueReadout;
/* The dwords in front of a display settings option button, ((UiDisplayModeOptionPrefix *)button)[-1]: its mode
   value(s), then (as in front of every template node) the tooltip text id. Read back by the option actions. */
typedef struct UiDisplayModeOptionPrefix {
    int32_t resolutionHeight; /* -0xC: resolution buttons only */
    int32_t modeValue;        /* -0x8: bits per pixel, resolution width or adapter index */
    uint32_t tooltipTextResourceId; /* -0x4 */
} UiDisplayModeOptionPrefix;
/* The "keep the new display mode?" dialog's countdownMessageText (a wrapped text, g_UiListOffsetControlVtable)
   with extra fields in its tail, see UiRuntime_OpenFourValueDialog. 0x98 bytes. */
typedef struct UiFourValueDialogCountdownText {
    UiWrappedTextControl text;
    int32_t countdown;                /* +0x5C: the number shown */
    int32_t stepTicks;                /* +0x60: frame updates left until the number counts down */
    int32_t previousWidth;            /* +0x64 */
    int32_t previousHeight;           /* +0x68 */
    int32_t previousBitsPerPixel;     /* +0x6C */
    int32_t previousAdapterIndex;     /* +0x70 */
    uint16_t countdownTextUtf16[18];  /* +0x74: UTF-16 digits of the countdown */
} UiFourValueDialogCountdownText;
/* Frame updates per step of the countdown in the "keep the new display mode?" dialog
   (UiFourValueDialog_TickCountdownAndRequestClose). */
#define UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS 20
/* Pointer coordinate far outside every control, used by UiImageControl_NonRightDrag for the synthetic events
   it sends when the pointer moves from one child to another (the new child's press, the old child's drag and
   release), so that no pixel test of theirs hits. */
#define UI_POINTER_FAR_OUTSIDE 0x70000000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00422910 */
void UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root);

/* 0x00423C40 */
void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode);

/* 0x00423C80 */
void UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode);

/* 0x00423CB0 */
void UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode);

/* 0x00424590 */
void UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings(UiNodeBase *sourceNode);

/* 0x004BC8B0 */
void UiImageControl_NonRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

/* 0x004BCB50 */
void UiImageControl_TickHover(UiImageControl *control);

/* 0x00423B30 */
void UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode);

/* 0x00423C00 */
void UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode);

/* 0x004242D0 */
void UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root);

/* 0x004B3F40 */
void UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiRangeSliderControl *control);

/* 0x004B41C0 */
void UiRangeSliderControl_BeginThumbDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

/* 0x004B4280 */
void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiRangeSliderControl *control);

/* 0x004B45F0 */
void UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control);

/* 0x004B4620 */
void UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control);

/* 0x004BC5C0 */
void UiImageControl_DrawClipped(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImageControl *control);

/* 0x004BC6E0 */
void UiImageControl_NonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

/* 0x004BC7E0 */
void UiImageControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

/* 0x004BD2A0 */
void ModelNodeRuntime_RefreshStateTint(ModelRuntimeNode *modelNode);

/* 0x00517E30 */
void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiHorizontalGaugeControl *control);

/* 0x00423600 */
void UiDisplaySettings_OpenAndPopulateModeSelection(void);

/* 0x004BC9B0 */
UiNodeBase * UiImageControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

/* 0x00423D70 */
void UiDisplayModeSelection_RefreshEnumeratedOptions
          (FrontendDisplayAdapterIndex adapterIndex,UiNodeBase *selectedModeValue,
          FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
          UiNodeBase *displaySettingsRoot);

#endif /* THANDOR_UI_CONTROLS_MISC_H */
