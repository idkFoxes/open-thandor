/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/dialogs/display_settings.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_DIALOGS_DISPLAY_SETTINGS_H
#define THANDOR_UI_DIALOGS_DISPLAY_SETTINGS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/dialogs/display_settings. */

/* Action ids of the display settings dialog (g_UiDisplaySettingsRootTemplate); action 0x200 + n runs
   g_UiDisplayModeSelectionActionHandlers20[n]. UiDisplayModeSelection_RefreshEnumeratedOptions suppresses
   the option buttons whose mode was not enumerated. */
#define UI_DISPLAY_MODE_ACTION_APPLY 0x200
#define UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH 0x201 /* 4 colour-depth buttons */
#define UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION 0x205 /* 8 resolution buttons */
#define UI_DISPLAY_MODE_ACTION_REVERT 0x20D /* "keep the new mode?" dialog: back to the previous mode */
#define UI_DISPLAY_MODE_ACTION_CANCEL 0x20E
#define UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER 0x20F /* 5 adapter buttons */
/* g_UiActionHandlerPages page of these action ids (0x200 >> 8), installed by the dialog when it opens. */
#define UI_DISPLAY_MODE_ACTION_HANDLER_PAGE 2
/* Texts of the display settings dialog (text resource ids). */
#define TEXT_ID_OK 0x100
#define TEXT_ID_CANCEL 0x101
#define TEXT_ID_DISPLAY_COLOR_DEPTH_OPTION 0x106 /* colour depth button, formats the bit depth */
#define TEXT_ID_DISPLAY_RESOLUTION_OPTION 0x107 /* resolution button, formats width and height */
#define TEXT_ID_DISPLAY_SETTINGS_TITLE 0x108
#define TEXT_ID_DISPLAY_COLOR_SCALE 0x10A /* colour scale slider column (likely contrast) */
#define TEXT_ID_DISPLAY_COLOR_BIAS 0x10B /* colour bias slider column (likely brightness) */
#define TEXT_ID_DISPLAY_RESOLUTION_HEADING 0x10C
#define TEXT_ID_DISPLAY_COLOR_DEPTH_HEADING 0x10D
#define TEXT_ID_DISPLAY_ADAPTER_HEADING 0x10F
#define TEXT_ID_DISPLAY_ADAPTER_OPTION 0x110 /* adapter button, formats the adapter number */
/* Display settings pages: distinct display modes are sorted as width * UI_DISPLAY_MODE_WIDTH_SCALE + height
   (the height is the low word); unused candidate slots hold UI_DISPLAY_MODE_NONE */
#define UI_DISPLAY_MODE_WIDTH_SCALE 0x10000
#define UI_DISPLAY_MODE_HEIGHT_MASK 0xffff
#define UI_DISPLAY_MODE_NONE 0xffffffffu
/* The display settings dialog's applyButton (a framed text button, g_UiFramedTextButtonControlVtable) with extra fields in
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
   (written by UiDisplaySettingsRoot_FormatColorReadouts). 0x9C bytes. */
typedef struct UiDisplaySettingsValueReadout {
    UiSingleLineTextControl label;
    uint16_t colorScaleTextUtf16[16]; /* +0x5C, shown by colorScaleValueText */
    uint16_t colorBiasTextUtf16[16];  /* +0x7C */
} UiDisplaySettingsValueReadout;
/* UiDisplayModeOptionPrefix (the mode values in front of each display settings option button,
   DISPLAY_SETTINGS_UI(root, <button>_prefix)) is generated with the template: thandor/generated/ui_templates.h. */
/* The UiDisplayModeOptionPrefix in front of an option button the code has only as a node pointer. */
#define DISPLAY_MODE_OPTION_PREFIX(button) UI_TEMPLATE_NODE_PREFIX(UiDisplayModeOptionPrefix,button)
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
/* Rich text of that dialog's countdownMessageText; selector 0 is the countdown number. */
#define TEXT_ID_DISPLAY_MODE_KEEP_COUNTDOWN 0x109
/* Pointer coordinate far outside every control, used by UiImageControl_NonRightDrag for the synthetic events
   it sends when the pointer moves from one child to another (the new child's press, the old child's drag and
   release), so that no pixel test of theirs hits. */
#define UI_POINTER_FAR_OUTSIDE 0x70000000

/* Functions are grouped by semantic ownership. */

void UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root);

void UiDisplayModeAction_UpdateColorDepthSelection(UiNodeBase *sourceNode);

void UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode);

void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode);

void UiDisplayModeAction_RevertAndReopenSettings(UiNodeBase *sourceNode);

void UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode);

void UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode);

void UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root);

void UiDisplaySettings_OpenAndPopulateModeSelection(void);

void UiDisplayModeSelection_RefreshEnumeratedOptions
          (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits selectedBitsPerPixel,
          FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
          UiNodeBase *displaySettingsRoot);

extern DisplaySettingsUiImage g_UiDisplaySettingsRootTemplate;

void UiDisplaySettingsRoot_FormatColorReadouts(void *root);

void UiRuntime_OpenFourValueDialog(UiPixelCoordinate previousAdapterIndex,UiPixelCoordinate previousBitsPerPixel,
          UiPixelCoordinate previousHeight,UiPixelCoordinate previousWidth);

#endif /* THANDOR_UI_DIALOGS_DISPLAY_SETTINGS_H */
