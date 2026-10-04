/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/dialogs/display_settings.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/dialogs/display_settings.h>
#include <thandor/thandor.h>

/* Module data. */

static UiRootCallbacks g_UiDisplaySettingsRootCallbacks = {
    .vetoClose = UI_SLOT(UiRootCallbacks_Free),
    .frameUpdate = UI_SLOT(UiDisplaySettingsRoot_RefreshModeSelection),
    .method08 = UI_SLOT(UiModalDialogRoot_BlockMissedPointerPress),
    .pointerMissPolicy = UI_SLOT(UiModalDialogRoot_BlockMissedPointerMotion)};

/* the display settings dialog, copied and linked by
   UiDisplaySettings_OpenAndPopulateModeSelection. */
DisplaySettingsUiImage g_UiDisplaySettingsRootTemplate = {
    .displaySettingsWindow = { /* g_UiResizableWindowControlVtable */
        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = DISPLAY_SETTINGS_LINK(cancelButton), .parent = UI_TEMPLATE_NO_LINK,
        .vtable = THANDOR_PTR(&g_UiResizableWindowControlVtable),
        .leftOffset = -216, .topOffset = -144, .rightOffset = 216, .bottomOffset = 144,
        .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET | 0x1},
    .displaySettingsWindow_fields = {.rootFlags = UI_ROOT_TILED_BACKGROUND | UI_ROOT_FRAME | UI_ROOT_TITLE_BAR, .titleTextResourceId = TEXT_ID_DISPLAY_SETTINGS_TITLE},
    .cancelButton = { /* g_UiFramedTextButtonControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(applyButton), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
        .leftOffset = 16, .topOffset = 232, .rightOffset = 112, .bottomOffset = 256,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_PREFERRED_FOCUS_TARGET},
    .cancelButton_fields = {.stateFlags = 0x8, .actionId = UI_DISPLAY_MODE_ACTION_CANCEL, .textResourceId = TEXT_ID_CANCEL},
    .applyButton = { /* g_UiFramedTextButtonControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionHeading), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
        .leftOffset = 128, .topOffset = 232, .rightOffset = 240, .bottomOffset = 256,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .applyButton_fields = {.button = {.stateFlags = UI_BUTTON_FRAME_INSET, .actionId = UI_DISPLAY_MODE_ACTION_APPLY, .textResourceId = TEXT_ID_OK}},
    /* not in the original: the colour depth heading and its four buttons are not linked into the dialog any more
       (32-bit colour only); their nodes stay as unused template data, so the image keeps its layout */
    .resolutionHeading = { /* g_UiFocusProxyControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(adapterHeading), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 160, .topOffset = 8, .rightOffset = 248, .bottomOffset = 28,
        .layoutWidth = -1, .layoutHeight = -1},
    .resolutionHeading_fields = {.textResourceId = TEXT_ID_DISPLAY_RESOLUTION_HEADING},
    .colorDepthHeading = { /* g_UiFocusProxyControlVtable */
        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 24, .topOffset = 8, .rightOffset = 120, .bottomOffset = 28,
        .layoutWidth = -1, .layoutHeight = -1},
    .colorDepthHeading_fields = {.textResourceId = TEXT_ID_DISPLAY_COLOR_DEPTH_HEADING},
    .adapterHeading = { /* g_UiFocusProxyControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption1), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 24, .topOffset = 112, .rightOffset = 120, .bottomOffset = 132,
        .layoutWidth = -1, .layoutHeight = -1},
    .adapterHeading_fields = {.textResourceId = TEXT_ID_DISPLAY_ADAPTER_HEADING},
    .colorDepthOption1_prefix = {0},
    .colorDepthOption1 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorDepthOption2), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 28, .rightOffset = 120, .bottomOffset = 48,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorDepthOption1_fields = {.stateFlags = UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER | UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH, .textResourceId = TEXT_ID_DISPLAY_COLOR_DEPTH_OPTION},
    .colorDepthOption2_prefix = {0},
    .colorDepthOption2 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorDepthOption3), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 48, .rightOffset = 120, .bottomOffset = 68,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorDepthOption2_fields = {.stateFlags = UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER | UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH + 1, .textResourceId = TEXT_ID_DISPLAY_COLOR_DEPTH_OPTION},
    .colorDepthOption3_prefix = {0},
    .colorDepthOption3 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorDepthOption4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 68, .rightOffset = 120, .bottomOffset = 88,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorDepthOption3_fields = {.stateFlags = UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER | UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH + 2, .textResourceId = TEXT_ID_DISPLAY_COLOR_DEPTH_OPTION},
    .colorDepthOption4_prefix = {0},
    .colorDepthOption4 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 88, .rightOffset = 120, .bottomOffset = 108,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorDepthOption4_fields = {.stateFlags = UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER | UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_COLOR_DEPTH + 3, .textResourceId = TEXT_ID_DISPLAY_COLOR_DEPTH_OPTION},
    .resolutionOption1_prefix = {0},
    .resolutionOption1 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption2), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 28, .rightOffset = 248, .bottomOffset = 48,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption1_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption2_prefix = {0},
    .resolutionOption2 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption3), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 48, .rightOffset = 248, .bottomOffset = 68,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption2_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 1, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption3_prefix = {0},
    .resolutionOption3 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 68, .rightOffset = 248, .bottomOffset = 88,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption3_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 2, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption4_prefix = {0},
    .resolutionOption4 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption5), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 88, .rightOffset = 248, .bottomOffset = 108,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption4_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 3, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption5_prefix = {0},
    .resolutionOption5 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption6), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 108, .rightOffset = 248, .bottomOffset = 128,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption5_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 4, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption6_prefix = {0},
    .resolutionOption6 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption7), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 128, .rightOffset = 248, .bottomOffset = 148,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption6_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 5, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption7_prefix = {0},
    .resolutionOption7 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(resolutionOption8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 148, .rightOffset = 248, .bottomOffset = 168,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption7_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 6, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .resolutionOption8_prefix = {0},
    .resolutionOption8 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(adapterOption1), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 152, .topOffset = 168, .rightOffset = 248, .bottomOffset = 188,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .resolutionOption8_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + 7, .textResourceId = TEXT_ID_DISPLAY_RESOLUTION_OPTION},
    .adapterOption1_prefix = {0},
    .adapterOption1 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(adapterOption2), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 132, .rightOffset = 144, .bottomOffset = 152,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET},
    .adapterOption1_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED | UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER, .textResourceId = TEXT_ID_DISPLAY_ADAPTER_OPTION},
    .adapterOption2_prefix = {0},
    .adapterOption2 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(adapterOption3), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 152, .rightOffset = 144, .bottomOffset = 172,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET},
    .adapterOption2_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED | UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + 1, .textResourceId = TEXT_ID_DISPLAY_ADAPTER_OPTION},
    .adapterOption3_prefix = {0},
    .adapterOption3 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(adapterOption4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 172, .rightOffset = 144, .bottomOffset = 192,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .adapterOption3_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED | UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + 2, .textResourceId = TEXT_ID_DISPLAY_ADAPTER_OPTION},
    .adapterOption4_prefix = {0},
    .adapterOption4 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(adapterOption5), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 192, .rightOffset = 144, .bottomOffset = 212,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .adapterOption4_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED | UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + 3, .textResourceId = TEXT_ID_DISPLAY_ADAPTER_OPTION},
    .adapterOption5_prefix = {0},
    .adapterOption5 = { /* g_UiGraphicsAdapterTextButtonVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorScaleSliderFrame), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiGraphicsAdapterTextButtonVtable),
        .leftOffset = 16, .topOffset = 212, .rightOffset = 144, .bottomOffset = 232,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_SUPPRESSED | UI_NODE_FALLBACK_FOCUS_TARGET},
    .adapterOption5_fields = {.stateFlags = UI_BUTTON_HIDDEN_WHILE_SUPPRESSED | UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME, .actionId = UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + 4, .textResourceId = TEXT_ID_DISPLAY_ADAPTER_OPTION},
    .colorScaleSliderFrame = { /* g_UiFocusProxyControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorBiasSliderFrame), .firstChild = DISPLAY_SETTINGS_LINK(colorScaleSlider), .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 256, .topOffset = 8, .rightOffset = 336, .bottomOffset = 208,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorScaleSliderFrame_fields = {.labelFlags = UI_LABEL_CENTER_X, .focusChild = DISPLAY_SETTINGS_LINK(colorScaleSlider), .textResourceId = TEXT_ID_DISPLAY_COLOR_SCALE},
    .colorScaleSlider = { /* g_UiRangeSliderControlVtable */
        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(colorScaleSliderFrame),
        .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
        .leftOffset = -8, .topOffset = 24, .rightOffset = 8,
        .leftAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x80000000,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorScaleSlider_fields = {.sliderFlags = UI_RANGE_SLIDER_VERTICAL, .minimumValue = 32768, .maximumValue = 0x20000, .value = 0, .stepValue = 2048, .actionId = UI_ACTION_NONE},
    .colorBiasSliderFrame = { /* g_UiFocusProxyControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorScaleValueText), .firstChild = DISPLAY_SETTINGS_LINK(colorBiasSlider), .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 336, .topOffset = 8, .rightOffset = 416, .bottomOffset = 208,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorBiasSliderFrame_fields = {.labelFlags = UI_LABEL_CENTER_X, .focusChild = DISPLAY_SETTINGS_LINK(colorBiasSlider), .textResourceId = TEXT_ID_DISPLAY_COLOR_BIAS},
    .colorBiasSlider = { /* g_UiRangeSliderControlVtable */
        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(colorBiasSliderFrame),
        .vtable = THANDOR_PTR(&g_UiRangeSliderControlVtable),
        .leftOffset = -8, .topOffset = 24, .rightOffset = 8,
        .leftAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x80000000,
        .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = UI_NODE_FALLBACK_FOCUS_TARGET},
    .colorBiasSlider_fields = {.sliderFlags = UI_RANGE_SLIDER_VERTICAL, .minimumValue = -4194304, .maximumValue = 0x400000, .value = 0, .stepValue = 0x20000, .actionId = UI_ACTION_NONE},
    .colorScaleValueText = { /* g_UiFocusProxyControlVtable */
        .nextSibling = DISPLAY_SETTINGS_LINK(colorBiasValueText), .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 256, .topOffset = 208, .rightOffset = 336, .bottomOffset = 228,
        .layoutWidth = -1, .layoutHeight = -1},
    .colorScaleValueText_fields = {.labelFlags = UI_LABEL_CENTER_X | UI_LABEL_TEXT_IS_STREAM},
    .colorBiasValueText = { /* g_UiFocusProxyControlVtable */
        .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = DISPLAY_SETTINGS_LINK(displaySettingsWindow),
        .vtable = THANDOR_PTR(&g_UiFocusProxyControlVtable),
        .leftOffset = 336, .topOffset = 208, .rightOffset = 416, .bottomOffset = 228,
        .layoutWidth = -1, .layoutHeight = -1},
    .colorBiasValueText_fields = {.label = {.labelFlags = UI_LABEL_CENTER_X | UI_LABEL_TEXT_IS_STREAM}}
};
THANDOR_STATIC_ASSERT(sizeof(DisplaySettingsUiImage) == 0xBD4, "DisplaySettingsUiImage size");

static UiDisplayModeSelectionActionHandlerTable g_UiDisplayModeSelectionActionHandlers20 = {
    .handlers = {
        /*  0 */ THANDOR_FN(UiDisplayModeAction_ApplyPendingMode),
        /*  1 */ THANDOR_FN(nullptr), /* the original's colour depth buttons, gone (32-bit colour only) */
        /*  2 */ THANDOR_FN(nullptr), /* the original's colour depth buttons, gone (32-bit colour only) */
        /*  3 */ THANDOR_FN(nullptr), /* the original's colour depth buttons, gone (32-bit colour only) */
        /*  4 */ THANDOR_FN(nullptr), /* the original's colour depth buttons, gone (32-bit colour only) */
        /*  5 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /*  6 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /*  7 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /*  8 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /*  9 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /* 10 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /* 11 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /* 12 */ THANDOR_FN(UiDisplayModeAction_UpdateResolutionSelection),
        /* 13 */ THANDOR_FN(UiDisplayModeAction_RevertAndReopenSettings),
        /* 14 */ THANDOR_FN(UiDisplayModeAction_CancelAndRebuildPixelPacking),
        /* 15 */ THANDOR_FN(UiDisplayModeAction_UpdateAdapterSelection),
        /* 16 */ THANDOR_FN(UiDisplayModeAction_UpdateAdapterSelection),
        /* 17 */ THANDOR_FN(UiDisplayModeAction_UpdateAdapterSelection),
        /* 18 */ THANDOR_FN(UiDisplayModeAction_UpdateAdapterSelection),
        /* 19 */ THANDOR_FN(UiDisplayModeAction_UpdateAdapterSelection)
    }};

/* the ascending list of distinct values (resolutions,
   adapters) that UiDisplaySettings_OpenAndPopulateModeSelection sorts in, 0xFFFFFFFF = empty */
static DisplayModeScratchWord g_UiDisplayModeDistinctValueScratch[8] = {0};

static UiRootCallbacks g_UiFourValueDialogRootCallbacks = {
    .vetoClose = UI_SLOT(UiRootCallbacks_Free),
    .frameUpdate = UI_SLOT(UiFourValueDialog_TickCountdownAndRequestClose),
    .method08 = UI_SLOT(UiModalDialogRoot_BlockMissedPointerPress),
    .pointerMissPolicy = UI_SLOT(UiModalDialogRoot_BlockMissedPointerMotion)};

static FourValueDialogUiImage g_UiFourValueDialogTemplateImage = {
        { /* +0000 confirmModeDialogPanel g_UiPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x58), .parent = UI_TEMPLATE_NO_LINK,
            .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
            .leftOffset = -128, .topOffset = -48, .rightOffset = 128, .bottomOffset = 48,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x21},
        {
            0x00000003},
        { /* +0058 revertButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .leftOffset = 16, .topOffset = -32, .rightOffset = 112, .bottomOffset = -8,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x2},
        {
            0x00000008, 0x0000020D, 0x00000101},
        { /* +00B4 keepModeButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x110), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .leftOffset = 128, .topOffset = -32, .rightOffset = 240, .bottomOffset = -8,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00000100},
        { /* +0110 countdownMessageText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .leftOffset = 8, .topOffset = 8, .rightOffset = -8, .bottomOffset = -40,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0x00000000, 0x00000109, 0x00000000, 0x0000000F, 0x00000014},
};

/* Implementation ownership: ui/dialogs/display_settings. */

/* frameUpdate of g_UiDisplaySettingsRootCallbacks (the display settings dialog): when the colour bias or
   colour scale slider has moved, stores the new values, rebuilds the pixel packing tables at once (a live
   preview), refreshes which mode buttons are available and rewrites the two number readouts.
*/
void UiDisplaySettingsRoot_RefreshModeSelection(UiRootNode *root)

{
  UiAnchorFractionQ31 colorBiasQ16;
  UiAnchorFractionQ31 colorScaleQ16;
  UiDisplaySettingsApplyButton *applyButton;

  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  /* the slider values (UiRangeSliderControl.value) */
  colorBiasQ16 = ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorBiasSlider))->value;
  colorScaleQ16 = ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorScaleSlider))->value;
  if ((colorBiasQ16 != applyButton->selectedColorBiasQ16) ||
      (colorScaleQ16 != applyButton->selectedColorScaleQ16)) {
    applyButton->selectedColorBiasQ16 = colorBiasQ16;
    applyButton->selectedColorScaleQ16 = colorScaleQ16;
    g_SoftwareBuildPixelPackTables(colorScaleQ16,colorBiasQ16);
    UiDisplayModeSelection_RefreshEnumeratedOptions
              (applyButton->selectedAdapterIndex,
               (FrontendColorDepthBits)applyButton->selectedBitsPerPixel,
               applyButton->selectedHeight,
               applyButton->selectedWidth,&root->base);
    UiDisplaySettingsRoot_FormatColorReadouts(root);
  }
  return;
}

/* Handler of the eight resolution buttons (actions 0x205..0x20C, g_UiDisplayModeSelectionActionHandlers20[5..12])
   of the display settings dialog: selects the button's resolution (height 12 bytes and width 8 bytes before
   the button), keeps the selected adapter and bit depth and refreshes the available buttons.
*/
void UiDisplayModeAction_UpdateResolutionSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            ((FrontendDisplayAdapterIndex)applyButton->selectedAdapterIndex,
             (FrontendColorDepthBits)applyButton->selectedBitsPerPixel,
             DISPLAY_MODE_OPTION_PREFIX(sourceNode).resolutionHeight,
             DISPLAY_MODE_OPTION_PREFIX(sourceNode).modeValue,displaySettingsRoot);
  return;
}

/* Handler of the five adapter buttons (actions 0x20F..0x213, g_UiDisplayModeSelectionActionHandlers20[15..19])
   of the display settings dialog: selects the button's adapter (the dword 8 bytes before the button), keeps
   the selected resolution and bit depth and refreshes the available buttons.
*/
void UiDisplayModeAction_UpdateAdapterSelection(UiNodeBase *sourceNode)

{
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  UiDisplayModeSelection_RefreshEnumeratedOptions
            (DISPLAY_MODE_OPTION_PREFIX(sourceNode).modeValue,
             (FrontendColorDepthBits)applyButton->selectedBitsPerPixel,
             (FrontendDisplayDimensionPixels)applyButton->selectedHeight,
             (FrontendDisplayDimensionPixels)applyButton->selectedWidth,displaySettingsRoot);
  return;
}

/* Revert action (UI_DISPLAY_MODE_ACTION_REVERT, g_UiDisplayModeSelectionActionHandlers20[13]) of the "keep
   the new display mode?" dialog, from its button or from the expired countdown: closes the dialog, switches
   back to the previous display mode stored in it (a failure is fatal), lays out every open root for the
   restored framebuffer size and opens the display settings dialog again. The name is misleading: nothing is
   applied.
*/
void UiDisplayModeAction_RevertAndReopenSettings(UiNodeBase *sourceNode)

{
  int64_t scaledAnchor;
  UiRootNode *root;
  uint32_t modeError;
  uint32_t adapterIndex;
  uint32_t bitsPerPixel;
  uint32_t modeHeight;
  uint32_t modeWidth;
  UiFourValueDialogCountdownText *countdownText;

  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  countdownText = (UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText);
  modeWidth = countdownText->previousWidth;
  modeHeight = countdownText->previousHeight;
  bitsPerPixel = countdownText->previousBitsPerPixel;
  adapterIndex = countdownText->previousAdapterIndex;
  UiRootStack_Pop(root);
  g_CursorVisibilityToken--;
  UiFrame_ProcessAndPresentWithLockTransition();
  if (!g_GraphicsSetDisplayMode(adapterIndex,bitsPerPixel,modeHeight,modeWidth,&modeError)) {
    FatalError_ExitIfFailed(modeError,true);
  }
  /* the loop of UiRootStack_Relayout, inlined: edge = framebuffer size * anchor (Q31) + offset */
  root = g_UiRootNode;
  do {
    scaledAnchor = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).rightAnchorQ31;
    (root->base).right =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).rightOffset;
    scaledAnchor = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).bottomAnchorQ31;
    (root->base).bottom =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).bottomOffset;
    scaledAnchor = (uint64_t)g_FramebufferWidth * (uint64_t)(root->base).leftAnchorQ31;
    (root->base).left =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).leftOffset;
    scaledAnchor = (uint64_t)g_FramebufferHeight * (uint64_t)(root->base).topAnchorQ31;
    (root->base).top =
         (FIXED_PRODUCT_SHR(scaledAnchor, 31)) + (root->base).topOffset;
    (*((root->base).vtable)->layout)(&root->base);
    root = root->previousRoot;
  } while (root != (UiRootNode *)UI_NODE_NONE);
  g_CursorVisibilityToken++;
  UiDisplaySettings_OpenAndPopulateModeSelection();
  return;
}

/* Apply action (UI_DISPLAY_MODE_ACTION_APPLY, g_UiDisplayModeSelectionActionHandlers20[0]) of the display
   settings dialog: closes the dialog and, when the selected mode differs from the current one, switches to
   it. If the switch fails, the current mode is restored (a failure there is fatal) and the error is
   reported; otherwise every root is laid out again and the "keep the new display mode?" dialog opens with
   the previous mode, which it restores unless the player confirms.
*/
void UiDisplayModeAction_ApplyPendingMode(UiNodeBase *sourceNode)

{
  uint32_t pendingBitsPerPixel;
  uint32_t pendingAdapterIndex;
  UiRootNode *root;
  uint32_t pendingWidth;
  uint32_t pendingHeight;
  uint32_t currentBitsPerPixel;
  uint32_t pendingModeError;
  uint32_t restoreModeError;
  uint32_t currentAdapterIndex;
  uint32_t currentWidth;
  uint32_t currentHeight;
  UiDisplaySettingsApplyButton *applyButton;

  root = (UiRootNode *)UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  pendingWidth = applyButton->selectedWidth;
  pendingHeight = applyButton->selectedHeight;
  pendingBitsPerPixel = applyButton->selectedBitsPerPixel;
  /* the original: the RGB bits of the pixel format (24 in a 32-bit mode); 32-bit colour only now */
  currentBitsPerPixel = PERSISTENT_DEFAULT_BITS_PER_PIXEL;
  pendingAdapterIndex = applyButton->selectedAdapterIndex;
  UiRootStack_Pop(root);
  if (pendingWidth == g_FramebufferWidth && pendingHeight == g_FramebufferHeight &&
      pendingBitsPerPixel == currentBitsPerPixel && pendingAdapterIndex == g_ActiveGraphicsAdapterIndex) {
    return;
  }
  g_CursorVisibilityToken--;
  UiFrame_ProcessAndPresentWithLockTransition();
  /* the current mode, read before the switch changes it */
  currentAdapterIndex = g_ActiveGraphicsAdapterIndex;
  currentHeight = g_FramebufferHeight;
  currentWidth = g_FramebufferWidth;
  if (!g_GraphicsSetDisplayMode(pendingAdapterIndex,pendingBitsPerPixel,pendingHeight,pendingWidth,
                                &pendingModeError)) {
    if (!g_GraphicsSetDisplayMode(currentAdapterIndex,currentBitsPerPixel,currentHeight,currentWidth,
                                  &restoreModeError)) {
      FatalError_ExitIfFailed(restoreModeError,true);
    }
    g_CursorVisibilityToken++;
    FatalError_ReportIfFailed(pendingModeError,true);
    return;
  }
  UiRootStack_Relayout();
  g_CursorVisibilityToken++;
  UiRuntime_OpenFourValueDialog(currentAdapterIndex,currentBitsPerPixel,currentHeight,currentWidth);
  return;
}

/* Cancel action (UI_DISPLAY_MODE_ACTION_CANCEL, g_UiDisplayModeSelectionActionHandlers20[14]) of the display
   settings dialog: closes it and rebuilds the pixel packing tables from the colour bias and scale the
   dialog opened with, undoing the slider preview.
*/
void UiDisplayModeAction_CancelAndRebuildPixelPacking(UiNodeBase *sourceNode)

{
  int32_t colorBiasQ16;
  int32_t colorScaleQ16;
  UiNodeBase *displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton;

  displaySettingsRoot = UiNode_GetRoot(sourceNode);
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(displaySettingsRoot,applyButton);
  colorBiasQ16 = applyButton->originalColorBiasQ16;
  colorScaleQ16 = applyButton->originalColorScaleQ16;
  UiRootStack_Pop((UiRootNode *)sourceNode); /* the button, not the root, as in the original */
  g_SoftwareBuildPixelPackTables(colorScaleQ16,colorBiasQ16);
  return;
}

/* frameUpdate of g_UiFourValueDialogRootCallbacks (the "keep the new display mode?" dialog): every
   UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS frame updates the shown countdown drops by one; at zero the revert
   action is queued, otherwise the new number is written into the message.
*/
void UiFourValueDialog_TickCountdownAndRequestClose(UiRootNode *root)

{
  int32_t *stepTicksField;
  int32_t *countdownField;
  UiFourValueDialogCountdownText *countdownText;

  countdownText = (UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText);
  stepTicksField = &countdownText->stepTicks;
  *stepTicksField = *stepTicksField - 1;
  if (*stepTicksField == 0) {
    countdownText->stepTicks =
         UI_DISPLAY_MODE_COUNTDOWN_STEP_TICKS;
    countdownField = &countdownText->countdown;
    *countdownField = *countdownField - 1;
    if (*countdownField == 0) {
      UiActionQueue_Enqueue(UI_DISPLAY_MODE_ACTION_REVERT,root);
    }
    else {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                 countdownText->countdown,
                 countdownText->countdownTextUtf16);
    }
  }
  return;
}

/* Inserts value into the ascending list candidates[0..candidateCount-1] (UI_DISPLAY_MODE_NONE marks empty
   slots) unless it is already listed; the largest entry falls off the end. Used by the display settings
   dialog (UiDisplaySettings_OpenAndPopulateModeSelection) and the frontend display settings page
   (FrontendDisplaySettingsAction_OpenPageAndListModes). */
void UiDisplayModeCandidates_InsertSortedUnique
          (DisplayModeScratchWord *candidates,uint32_t candidateCount,DisplayModeScratchWord value)
{
  uint32_t candidateIndex;
  DisplayModeScratchWord displacedValue;

  for (candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++) {
    if (value == candidates[candidateIndex]) {
      return;
    }
  }
  for (candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++) {
    if (value < candidates[candidateIndex]) {
      displacedValue = candidates[candidateIndex];
      candidates[candidateIndex] = value;
      value = displacedValue;
    }
  }
}

/* Opens the display settings dialog (only when more than one display mode was enumerated): copies
   g_UiDisplaySettingsRootTemplate to the heap, records the current mode and colour bias/scale as both the
   selected and the original values, installs its action handlers and pushes it. The option buttons are then
   labelled with the enumerated values in ascending order: up to 8 resolutions and 5 adapters (a sorted insert
   into the g_UiDisplayModeDistinctValueScratch slots, 0xFFFFFFFF = empty); the original also listed up to 4
   distinct bit depths (32-bit colour only now).
   Reopened by UiDisplayModeAction_RevertAndReopenSettings. The original also reports a failed
   allocation; that caller ignores it.
*/
void UiDisplaySettings_OpenAndPopulateModeSelection()

{
  uint32_t framebufferWidth;
  uint32_t framebufferHeight;
  UiRootFlags activeAdapterIndex;
  int32_t colorScaleQ16;
  int32_t colorBiasQ16;
  UiRootNode *root;
  int copyCount;
  UiNodeFlags colorDepthBits;
  GraphicsDisplayModeCount remainingModes;
  uint32_t *copyCursor;
  const uint32_t *templateCursor;
  GraphicsDisplayMode *displayMode;
  UiDisplaySettingsApplyButton *applyButton;

  if (g_GraphicsDisplayModeCount <= 1) {
    return;
  }
  if (g_MemoryApi.alloc(sizeof(DisplaySettingsUiImage),(void **)&root) != 0) {
    return;
  }
  /* copy the template, one dword per step */
  templateCursor = (const uint32_t *)&g_UiDisplaySettingsRootTemplate;
  copyCursor = (uint32_t *)root;
  for (copyCount = sizeof(DisplaySettingsUiImage) / 4; copyCount != 0; copyCount--) {
    *copyCursor = *templateCursor;
    templateCursor = templateCursor + 1;
    copyCursor = copyCursor + 1;
  }
  activeAdapterIndex = g_ActiveGraphicsAdapterIndex;
  framebufferHeight = g_FramebufferHeight;
  framebufferWidth = g_FramebufferWidth;
  colorDepthBits = PERSISTENT_DEFAULT_BITS_PER_PIXEL; /* the original: the pixel format's RGB bits */
  /* applyButton tail: the selected mode tuple and the original one start out equal */
  applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  applyButton->selectedWidth = framebufferWidth;
  applyButton->selectedHeight = framebufferHeight;
  applyButton->selectedBitsPerPixel = colorDepthBits;
  applyButton->selectedAdapterIndex = activeAdapterIndex;
  applyButton->originalWidth = framebufferWidth;
  applyButton->originalHeight = framebufferHeight;
  applyButton->originalBitsPerPixel = colorDepthBits;
  applyButton->originalAdapterIndex = activeAdapterIndex;
  colorBiasQ16 = g_SoftwareColorBiasQ16;
  colorScaleQ16 = g_SoftwareColorScaleQ16;
  applyButton->selectedColorBiasQ16 = colorBiasQ16;
  applyButton->selectedColorScaleQ16 = colorScaleQ16;
  applyButton->originalColorBiasQ16 = colorBiasQ16;
  applyButton->originalColorScaleQ16 = colorScaleQ16;
  ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorBiasSlider))->value = colorBiasQ16;
  ((UiRangeSliderControl *)DISPLAY_SETTINGS_UI(root,colorScaleSlider))->value = colorScaleQ16;
  /* The two readouts show the number buffers kept in the tail of colorBiasValueText. */
  ((UiSingleLineTextControl *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->text =
       ((UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->colorBiasTextUtf16;
  ((UiSingleLineTextControl *)DISPLAY_SETTINGS_UI(root,colorScaleValueText))->text =
       ((UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText))->colorScaleTextUtf16;
  UiDisplaySettingsRoot_FormatColorReadouts(root);
  UiActionHandlers_SetPage(UI_DISPLAY_MODE_ACTION_HANDLER_PAGE,
                           (UiActionHandlerPage *)&g_UiDisplayModeSelectionActionHandlers20);
  UiRootStack_Push(&g_UiDisplaySettingsRootCallbacks,root);

  /* distinct resolutions, keyed width << 16 | height so that they sort by width, then height */
  g_UiDisplayModeDistinctValueScratch[0] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[1] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[2] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[3] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[4] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[5] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[6] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[7] = UI_DISPLAY_MODE_NONE;
  displayMode = g_GraphicsDisplayModes;
  for (remainingModes = g_GraphicsDisplayModeCount; remainingModes != 0; remainingModes--) {
    UiDisplayModeCandidates_InsertSortedUnique
              (g_UiDisplayModeDistinctValueScratch,8,
               displayMode->width * UI_DISPLAY_MODE_WIDTH_SCALE + displayMode->height);
    displayMode = displayMode + 1;
  }
  /* Resolution buttons: width at -8, height at -0xC. */
  DISPLAY_SETTINGS_UI(root,resolutionOption1_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[0] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption1_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[0] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption2_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[1] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption2_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[1] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption3_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[2] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption3_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[2] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption4_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[3] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption4_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[3] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption5_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[4] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption5_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[4] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption6_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[5] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption6_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[5] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption7_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[6] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption7_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[6] & UI_DISPLAY_MODE_HEIGHT_MASK;
  DISPLAY_SETTINGS_UI(root,resolutionOption8_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[7] >> 16;
  DISPLAY_SETTINGS_UI(root,resolutionOption8_prefix)->resolutionHeight = g_UiDisplayModeDistinctValueScratch[7] & UI_DISPLAY_MODE_HEIGHT_MASK;

  /* distinct adapters */
  g_UiDisplayModeDistinctValueScratch[0] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[1] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[2] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[3] = UI_DISPLAY_MODE_NONE;
  g_UiDisplayModeDistinctValueScratch[4] = UI_DISPLAY_MODE_NONE;
  displayMode = g_GraphicsDisplayModes;
  for (remainingModes = g_GraphicsDisplayModeCount; remainingModes != 0; remainingModes--) {
    UiDisplayModeCandidates_InsertSortedUnique(g_UiDisplayModeDistinctValueScratch,5,displayMode->adapterIndex);
    displayMode = displayMode + 1;
  }
  /* Adapter buttons: adapter index at -8. */
  DISPLAY_SETTINGS_UI(root,adapterOption1_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[0];
  DISPLAY_SETTINGS_UI(root,adapterOption2_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[1];
  DISPLAY_SETTINGS_UI(root,adapterOption3_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[2];
  DISPLAY_SETTINGS_UI(root,adapterOption4_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[3];
  DISPLAY_SETTINGS_UI(root,adapterOption5_prefix)->modeValue = g_UiDisplayModeDistinctValueScratch[4];
  UiDisplayModeSelection_RefreshEnumeratedOptions
            (g_ActiveGraphicsAdapterIndex,colorDepthBits,g_FramebufferHeight,g_FramebufferWidth,(UiNodeBase *)root);
  UiRootStack_InvalidateAll();
  return;
}

/* Refreshes the display settings dialog for a selected mode (adapterIndex, bit depth, height, width): every
   resolution and adapter button whose combination with the other selected values was not
   enumerated (GraphicsDisplayMode_IsEnumerated) is suppressed, the others are enabled; the buttons matching
   the selection are selected in their groups; the tuple is stored as the selected one, and the apply button
   is suppressed while it and the colour bias/scale equal the values the dialog opened with.
*/
void UiDisplayModeSelection_RefreshEnumeratedOptions
          (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits selectedBitsPerPixel,
          FrontendDisplayDimensionPixels modeHeight,FrontendDisplayDimensionPixels modeWidth,
          UiNodeBase *displaySettingsRoot)

{
  /* Every option button is a 0x68-byte node; the dwords just before each button hold its mode
     value(s) (DISPLAY_MODE_OPTION_PREFIX). */
  static const unsigned sizeButtons[8] = {offsetof(DisplaySettingsUiImage,resolutionOption1),
      offsetof(DisplaySettingsUiImage,resolutionOption2),offsetof(DisplaySettingsUiImage,resolutionOption3),
      offsetof(DisplaySettingsUiImage,resolutionOption4),offsetof(DisplaySettingsUiImage,resolutionOption5),
      offsetof(DisplaySettingsUiImage,resolutionOption6),offsetof(DisplaySettingsUiImage,resolutionOption7),
      offsetof(DisplaySettingsUiImage,resolutionOption8)};
  static const unsigned adapterButtons[5] = {offsetof(DisplaySettingsUiImage,adapterOption1),
      offsetof(DisplaySettingsUiImage,adapterOption2),offsetof(DisplaySettingsUiImage,adapterOption3),
      offsetof(DisplaySettingsUiImage,adapterOption4),offsetof(DisplaySettingsUiImage,adapterOption5)};
  uint8_t *root = (uint8_t *)displaySettingsRoot;
  UiDisplaySettingsApplyButton *applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  uint32_t bitsPerPixel = selectedBitsPerPixel;
  void *selected = nullptr;
  Bool8 modeMissing; /* GraphicsDisplayMode_IsEnumerated returns true when the mode was not enumerated */
  int i;

  /* the original first refreshed the four colour depth buttons (gone, 32-bit colour only) */
  for (i = 0; i < 8; i++) {
    uint32_t height = DISPLAY_MODE_OPTION_PREFIX(root + sizeButtons[i]).resolutionHeight;
    uint32_t width = DISPLAY_MODE_OPTION_PREFIX(root + sizeButtons[i]).modeValue;
    modeMissing = GraphicsDisplayMode_IsEnumerated(adapterIndex,bitsPerPixel,height,width);
    if (modeMissing) {
      UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_RESOLUTION + i,displaySettingsRoot);
    }
    if ((modeWidth == width) && (modeHeight == height)) {
      selected = root + sizeButtons[i];
    }
  }
  UiSelectableGroup_SelectExclusive(8,(UiNodeBase *)selected,
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption8),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption7),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption6),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption5),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption4),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption3),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption2),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,resolutionOption1));
  for (i = 0; i < 5; i++) {
    uint32_t adapter = DISPLAY_MODE_OPTION_PREFIX(root + adapterButtons[i]).modeValue;
    modeMissing = GraphicsDisplayMode_IsEnumerated(adapter,bitsPerPixel,modeHeight,modeWidth);
    if (modeMissing) {
      UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + i,displaySettingsRoot);
    }
    else {
      UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_FIRST_ADAPTER + i,displaySettingsRoot);
    }
    if (adapterIndex == adapter) {
      selected = root + adapterButtons[i];
    }
  }
  UiSelectableGroup_SelectExclusive(5,(UiNodeBase *)selected,
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption5),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption4),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption3),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption2),
      DISPLAY_SETTINGS_UI(displaySettingsRoot,adapterOption1));
  applyButton->selectedWidth = modeWidth;
  applyButton->selectedHeight = modeHeight;
  applyButton->selectedBitsPerPixel = bitsPerPixel;
  applyButton->selectedAdapterIndex = adapterIndex;
  if ((modeWidth == (uint32_t)applyButton->originalWidth) &&
      (modeHeight == (uint32_t)applyButton->originalHeight) &&
      (bitsPerPixel == applyButton->originalBitsPerPixel) &&
      (adapterIndex == applyButton->originalAdapterIndex) &&
      (applyButton->selectedColorBiasQ16 ==
       applyButton->originalColorBiasQ16) &&
      (applyButton->selectedColorScaleQ16 ==
       applyButton->originalColorScaleQ16)) {
    UiNodeList_SuppressActionId(UI_DISPLAY_MODE_ACTION_APPLY,displaySettingsRoot);
  }
  else {
    UiNodeList_UnsuppressActionId(UI_DISPLAY_MODE_ACTION_APPLY,displaySettingsRoot);
  }
  return;
}

/* Writes the two number readouts of the display settings dialog (root is a copy of
   g_UiDisplaySettingsRootTemplate): the selected colour bias (applyButton selectedColorBiasQ16, Q16, -64..+64)
   divided by 64.0, i.e. -1.000..+1.000, and the colour scale (applyButton selectedColorScaleQ16, Q16,
   0.5..2.0) as a plain value, both signed with up to 3 fraction digits into the number buffers in the tail of
   colorBiasValueText (colorBiasTextUtf16 for the bias, colorScaleTextUtf16 for the scale). Called when the dialog opens and by
   UiDisplaySettingsRoot_RefreshModeSelection.
*/
void UiDisplaySettingsRoot_FormatColorReadouts(void *root)

{
  UiDisplaySettingsApplyButton *applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  UiDisplaySettingsValueReadout *readout =
       (UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText);

  /* fractionalDigits 3, integerDigitLimit 10; the denominators are 64.0 and 1.0 in Q16 */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,3,10,64 << 16,applyButton->selectedColorBiasQ16,
             readout->colorBiasTextUtf16);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,3,10,1 << 16,applyButton->selectedColorScaleQ16,
             readout->colorScaleTextUtf16);
  return;
}

/* Opens the "keep the new display mode?" dialog after UiDisplayModeAction_ApplyPendingMode switched modes:
   copies g_UiFourValueDialogTemplateImage to the heap, points the countdown text (text 0x109) at its number
   buffer, prints the starting seconds there and stores the previous mode tuple, which the revert action
   0x20D (button or countdown expiry) restores. The original also reports a failed allocation to the caller;
   no caller looks at it.
*/
void UiRuntime_OpenFourValueDialog(UiPixelCoordinate previousAdapterIndex,UiPixelCoordinate previousBitsPerPixel,
          UiPixelCoordinate previousHeight,UiPixelCoordinate previousWidth)

{
  uint16_t *countdownNumberBuffer;
  UiRootNode *root;
  int remainingDwords;
  uint32_t *templateCursor;
  uint32_t *copyCursor;
  uint32_t allocError;
  uint16_t *resolvedText;
  UiFourValueDialogCountdownText *countdownText;

  allocError = g_MemoryApi.alloc(sizeof(g_UiFourValueDialogTemplateImage),(void **)&root);
  if (allocError != 0) {
    root = (UiRootNode *)(uintptr_t)allocError;
  }
  else {
    /* copy the 0x1A4-byte template, one dword per step */
    templateCursor = (uint32_t *)&g_UiFourValueDialogTemplateImage;
    copyCursor = (uint32_t *)root;
    countdownText = (UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText);
    for (remainingDwords = sizeof(g_UiFourValueDialogTemplateImage) / 4; remainingDwords != 0; remainingDwords--) {
      *copyCursor = *templateCursor;
      templateCursor++;
      copyCursor++;
    }
    countdownNumberBuffer = countdownText->countdownTextUtf16;
    resolvedText = TextResource_Resolve(TEXT_ID_DISPLAY_MODE_KEEP_COUNTDOWN);
    RichTextCommandStream_PatchPayloadBySelector(0,countdownNumberBuffer,resolvedText);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,countdownText->countdown,countdownNumberBuffer);
    countdownText->previousWidth = previousWidth;
    countdownText->previousHeight = previousHeight;
    countdownText->previousBitsPerPixel = previousBitsPerPixel;
    countdownText->previousAdapterIndex = previousAdapterIndex;
    UiRootStack_Push(&g_UiFourValueDialogRootCallbacks,root);
    UiRootStack_InvalidateAll();
    return;
  }
  return;
}
