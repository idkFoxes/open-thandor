/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/dialogs/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_DIALOGS_TYPES_H
#define THANDOR_UI_DIALOGS_TYPES_H

#include <stdint.h>
#include <stddef.h> /* offsetof */
#include <thandor/core/contracts.h> /* THANDOR_STATIC_ASSERT */
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/ui/controls/types.h>

typedef struct UiDisplayModeSelectionActionHandlerTable UiDisplayModeSelectionActionHandlerTable, *PUiDisplayModeSelectionActionHandlerTable;
typedef struct UiRootCallbacks UiRootCallbacks;

struct UiDisplayModeSelectionActionHandlerTable {
    Ptr32<void (struct UiNodeBase *)> handlers[20]; 
};

using DisplayModeScratchWord = uint32_t;

/* Class fields a template stores behind a node's UiNodeBase. A template keeps only the fields up to the next
   node of the original image; the class fields after them (a button's activationSound, a slider's clickSound)
   overlap the next node there and are set at runtime. One struct per control class, named like the class. */

/* Text buttons (g_UiTextButtonControlVtable, g_UiFramedTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable): UiSelectableControl fields, then text and style. */
typedef struct UiTextButtonTemplateFields {
    UiSelectableStateFlags stateFlags; /* UI_BUTTON_* (and UI_ADAPTER_TEXT_BUTTON_*) bits */
    UiActionId actionId;
    UiTextResourceId textResourceId;
    UiPackedTextStyle packedTextStyle;
} UiTextButtonTemplateFields;

/* Single-line labels (g_UiFocusProxyControlVtable, UiSingleLineTextControl). */
typedef struct UiLabelTemplateFields {
    uint32_t labelFlags; /* UI_LABEL_* */
    Ptr32<UiNodeBase> focusChild; /* link */
    uint32_t textResourceId; /* a TextResourceId, or (UI_LABEL_TEXT_IS_STREAM) a command stream set at runtime */
    UiPackedTextStyle styleOverride;
} UiLabelTemplateFields;

/* Range sliders (g_UiRangeSliderControlVtable, UiRangeSliderControl). */
typedef struct UiRangeSliderTemplateFields {
    uint32_t sliderFlags; /* UI_RANGE_SLIDER_* */
    int32_t minimumValue;
    int32_t maximumValue;
    int32_t value;
    int32_t stepValue;
    UiActionId actionId;
} UiRangeSliderTemplateFields;

/* Resizable windows (g_UiResizableWindowControlVtable, UiResizableWindowControl). */
typedef struct UiResizableWindowTemplateFields {
    UiRootFlags rootFlags; /* UI_ROOT_* */
    Ptr32<struct UiRootCallbacks> callbacks;
    Ptr32<struct UiRootNode> previousRoot;
    UiTextResourceId titleTextResourceId;
    uint32_t field5C;
    int32_t restoredLeft;
    int32_t restoredTop;
    int32_t restoredRight;
    int32_t restoredBottom;
    int32_t dragAnchorXOrPendingRight;
    int32_t dragAnchorYOrPendingBottom;
} UiResizableWindowTemplateFields;

/* The display settings dialog's applyButton: a framed text button whose tail holds the selected mode tuple and
   colour bias/scale, then the same six values as they were when the dialog opened (UiDisplaySettingsApplyButton). */
typedef struct UiDisplaySettingsApplyButtonTemplateFields {
    UiTextButtonTemplateFields button;
    int32_t selectedWidth;
    int32_t selectedHeight;
    uint32_t selectedBitsPerPixel;
    uint32_t selectedAdapterIndex;
    int32_t selectedColorBiasQ16;
    int32_t selectedColorScaleQ16;
    int32_t originalWidth;
    int32_t originalHeight;
    uint32_t originalBitsPerPixel;
    uint32_t originalAdapterIndex;
    int32_t originalColorBiasQ16;
    int32_t originalColorScaleQ16;
} UiDisplaySettingsApplyButtonTemplateFields;

/* The display settings dialog's colorBiasValueText label: its tail holds the number buffers of both readouts
   (UiDisplaySettingsValueReadout). */
typedef struct UiDisplaySettingsReadoutTemplateFields {
    UiLabelTemplateFields label;
    uint16_t colorScaleTextUtf16[16];
    uint16_t colorBiasTextUtf16[16];
} UiDisplaySettingsReadoutTemplateFields;

/* The dwords in front of a display settings option button (DisplaySettingsUiImage <button>_prefix):
   its mode value(s), then (as in front of every template node) the tooltip text id. Read back by the
   option actions (UiDisplayModeAction_Update*Selection). */
typedef struct UiDisplayModeOptionPrefix {
    int32_t resolutionHeight; /* -0xC: resolution buttons only */
    int32_t modeValue; /* -0x8: bits per pixel, resolution width or adapter index */
    uint32_t tooltipTextResourceId; /* -0x4 */
} UiDisplayModeOptionPrefix;
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
/* The <node>_prefix of the given type in front of a node the code only has as a pointer (the
   node of an action callback, a node chosen at runtime); with the node's name known,
   <TEMPLATE>_UI(root, <node>_prefix) names it directly. */
#define UI_TEMPLATE_NODE_PREFIX(type, node) (((type *)(uintptr_t)(node))[-1])
#pragma pack(push, 1)

/* g_FatalErrorUiRootTemplateImage: 3 UI nodes. FATAL_ERROR_UI(root, node) is the node in a copy of it (or a node's <node>_prefix),
   FATAL_ERROR_UI_FIELD(root, node, offset, type) a class field behind the UiNodeBase of the node. */
typedef struct FatalErrorUiImage {
    UiPanelControl fatalErrorPanel; /* +0000 g_UiPanelControlVtable: Panel root of the fatal error dialog; its leftOffset/rightOffset give the text wrap width. */
    UiListOffsetControl errorMessageText; /* +0058 g_UiListOffsetControlVtable: Rich text area showing the error (a UiWrappedTextControl); its leftOffset/rightOffset narrow the wrap width, its text is set in the template image. */
    UiNodeBase okButton; /* +00B4 g_UiFramedTextButtonControlVtable: Bottom-right button, action 1, text 0x100 (OK), closes the dialog. */
    UiTextButtonTemplateFields okButton_fields;
} FatalErrorUiImage;

/* g_UiDisplaySettingsRootTemplate: 29 UI nodes. DISPLAY_SETTINGS_UI(root, node) is the node in a copy of it (or a node's <node>_prefix),
   DISPLAY_SETTINGS_UI_FIELD(root, node, offset, type) a class field behind the UiNodeBase of the node. */
typedef struct DisplaySettingsUiImage {
    UiResizableWindowControl displaySettingsWindow; /* +0000 g_UiResizableWindowControlVtable: Centered resizable window root of the display settings dialog; parent of all other nodes. */
    UiNodeBase cancelButton; /* +0078 g_UiFramedTextButtonControlVtable: Bottom-left button, action 0x20E, text 0x101 (Cancel); restores the previous pixel-pack color tables. */
    UiTextButtonTemplateFields cancelButton_fields;
    UiDisplaySettingsApplyButton applyButton; /* +00D4 g_UiFramedTextButtonControlVtable: Bottom button, action 0x200, text 0x100 (OK); its extra fields hold the selected and original mode tuple plus color bias/scale, suppressed while nothing changed. */
    UiFocusProxyControl resolutionHeading; /* +0160 g_UiFocusProxyControlVtable: Label text 0x10C above the resolution button column. */
    UiFocusProxyControl colorDepthHeading; /* +01BC g_UiFocusProxyControlVtable: Label text 0x10D above the color depth button column. */
    UiFocusProxyControl adapterHeading; /* +0218 g_UiFocusProxyControlVtable: Label text 0x10F above the graphics adapter button column. */
    UiDisplayModeOptionPrefix colorDepthOption1_prefix; /* +0274 */
    UiNodeBase colorDepthOption1; /* +0280 g_UiGraphicsAdapterTextButtonVtable: First color depth option button (action 0x201, group 0x480). */
    UiTextButtonTemplateFields colorDepthOption1_fields;
    UiDisplayModeOptionPrefix colorDepthOption2_prefix; /* +02DC */
    UiNodeBase colorDepthOption2; /* +02E8 g_UiGraphicsAdapterTextButtonVtable: Second color depth option button (action 0x202). */
    UiTextButtonTemplateFields colorDepthOption2_fields;
    UiDisplayModeOptionPrefix colorDepthOption3_prefix; /* +0344 */
    UiNodeBase colorDepthOption3; /* +0350 g_UiGraphicsAdapterTextButtonVtable: Third color depth option button (action 0x203). */
    UiTextButtonTemplateFields colorDepthOption3_fields;
    UiDisplayModeOptionPrefix colorDepthOption4_prefix; /* +03AC */
    UiNodeBase colorDepthOption4; /* +03B8 g_UiGraphicsAdapterTextButtonVtable: Fourth color depth option button (action 0x204). */
    UiTextButtonTemplateFields colorDepthOption4_fields;
    UiDisplayModeOptionPrefix resolutionOption1_prefix; /* +0414 */
    UiNodeBase resolutionOption1; /* +0420 g_UiGraphicsAdapterTextButtonVtable: First resolution option button (action 0x205, group 0x400). */
    UiTextButtonTemplateFields resolutionOption1_fields;
    UiDisplayModeOptionPrefix resolutionOption2_prefix; /* +047C */
    UiNodeBase resolutionOption2; /* +0488 g_UiGraphicsAdapterTextButtonVtable: Second resolution option button (action 0x206). */
    UiTextButtonTemplateFields resolutionOption2_fields;
    UiDisplayModeOptionPrefix resolutionOption3_prefix; /* +04E4 */
    UiNodeBase resolutionOption3; /* +04F0 g_UiGraphicsAdapterTextButtonVtable: Third resolution option button (action 0x207). */
    UiTextButtonTemplateFields resolutionOption3_fields;
    UiDisplayModeOptionPrefix resolutionOption4_prefix; /* +054C */
    UiNodeBase resolutionOption4; /* +0558 g_UiGraphicsAdapterTextButtonVtable: Fourth resolution option button (action 0x208). */
    UiTextButtonTemplateFields resolutionOption4_fields;
    UiDisplayModeOptionPrefix resolutionOption5_prefix; /* +05B4 */
    UiNodeBase resolutionOption5; /* +05C0 g_UiGraphicsAdapterTextButtonVtable: Fifth resolution option button (action 0x209). */
    UiTextButtonTemplateFields resolutionOption5_fields;
    UiDisplayModeOptionPrefix resolutionOption6_prefix; /* +061C */
    UiNodeBase resolutionOption6; /* +0628 g_UiGraphicsAdapterTextButtonVtable: Sixth resolution option button (action 0x20A). */
    UiTextButtonTemplateFields resolutionOption6_fields;
    UiDisplayModeOptionPrefix resolutionOption7_prefix; /* +0684 */
    UiNodeBase resolutionOption7; /* +0690 g_UiGraphicsAdapterTextButtonVtable: Seventh resolution option button (action 0x20B). */
    UiTextButtonTemplateFields resolutionOption7_fields;
    UiDisplayModeOptionPrefix resolutionOption8_prefix; /* +06EC */
    UiNodeBase resolutionOption8; /* +06F8 g_UiGraphicsAdapterTextButtonVtable: Eighth resolution option button (action 0x20C). */
    UiTextButtonTemplateFields resolutionOption8_fields;
    UiDisplayModeOptionPrefix adapterOption1_prefix; /* +0754 */
    UiNodeBase adapterOption1; /* +0760 g_UiGraphicsAdapterTextButtonVtable: First graphics adapter option button (action 0x20F, group 0xC00). */
    UiTextButtonTemplateFields adapterOption1_fields;
    UiDisplayModeOptionPrefix adapterOption2_prefix; /* +07BC */
    UiNodeBase adapterOption2; /* +07C8 g_UiGraphicsAdapterTextButtonVtable: Second graphics adapter option button (action 0x210). */
    UiTextButtonTemplateFields adapterOption2_fields;
    UiDisplayModeOptionPrefix adapterOption3_prefix; /* +0824 */
    UiNodeBase adapterOption3; /* +0830 g_UiGraphicsAdapterTextButtonVtable: Third graphics adapter option button (action 0x211). */
    UiTextButtonTemplateFields adapterOption3_fields;
    UiDisplayModeOptionPrefix adapterOption4_prefix; /* +088C */
    UiNodeBase adapterOption4; /* +0898 g_UiGraphicsAdapterTextButtonVtable: Fourth graphics adapter option button (action 0x212). */
    UiTextButtonTemplateFields adapterOption4_fields;
    UiDisplayModeOptionPrefix adapterOption5_prefix; /* +08F4 */
    UiNodeBase adapterOption5; /* +0900 g_UiGraphicsAdapterTextButtonVtable: Fifth graphics adapter option button (action 0x213). */
    UiTextButtonTemplateFields adapterOption5_fields;
    UiFocusProxyControl colorScaleSliderFrame; /* +095C g_UiFocusProxyControlVtable: Framed column (text 0x10A) holding the color scale slider, likely contrast. */
    UiNodeBase colorScaleSlider; /* +09B8 g_UiRangeSliderControlVtable: Vertical range slider for the pixel-pack color scale (Q16, 0.5 to 2.0). */
    UiRangeSliderTemplateFields colorScaleSlider_fields;
    UiFocusProxyControl colorBiasSliderFrame; /* +0A1C g_UiFocusProxyControlVtable: Framed column (text 0x10B) holding the color bias slider, likely brightness. */
    UiNodeBase colorBiasSlider; /* +0A78 g_UiRangeSliderControlVtable: Vertical range slider for the pixel-pack color bias (Q16, -64 to +64). */
    UiRangeSliderTemplateFields colorBiasSlider_fields;
    UiFocusProxyControl colorScaleValueText; /* +0ADC g_UiFocusProxyControlVtable: Text readout below the color scale slider. */
    UiDisplaySettingsValueReadout colorBiasValueText; /* +0B38 g_UiFocusProxyControlVtable: Text readout below the color bias slider; its tail holds both number buffers (0xB94 scale, 0xBB4 bias) written by UiDisplaySettingsRoot_FormatColorReadouts. */
} DisplaySettingsUiImage;
#define DISPLAY_SETTINGS_UI(root, node) (&((DisplaySettingsUiImage *)(uintptr_t)(root))->node)
/* A link to node `node` of the template (its offset in the template, made a pointer when the copy is linked). */
#define DISPLAY_SETTINGS_LINK(node) UI_TEMPLATE_LINK(offsetof(DisplaySettingsUiImage, node))
/* The node offsets of the original template image: the field structs must keep them. */
/* The template layouts are the original 32-bit images on both architectures (pointer fields are Ptr32). */
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, displaySettingsWindow) == 0x0, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, cancelButton) == 0x78, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, applyButton) == 0xD4, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionHeading) == 0x160, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorDepthHeading) == 0x1BC, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, adapterHeading) == 0x218, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorDepthOption1) == 0x280, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorDepthOption2) == 0x2E8, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorDepthOption3) == 0x350, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorDepthOption4) == 0x3B8, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption1) == 0x420, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption2) == 0x488, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption3) == 0x4F0, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption4) == 0x558, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption5) == 0x5C0, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption6) == 0x628, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption7) == 0x690, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, resolutionOption8) == 0x6F8, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, adapterOption1) == 0x760, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, adapterOption2) == 0x7C8, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, adapterOption3) == 0x830, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, adapterOption4) == 0x898, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, adapterOption5) == 0x900, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorScaleSliderFrame) == 0x95C, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorScaleSlider) == 0x9B8, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorBiasSliderFrame) == 0xA1C, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorBiasSlider) == 0xA78, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorScaleValueText) == 0xADC, "DisplaySettingsUiImage layout");
THANDOR_STATIC_ASSERT(offsetof(DisplaySettingsUiImage, colorBiasValueText) == 0xB38, "DisplaySettingsUiImage layout");

/* g_UiFourValueDialogTemplateImage: 4 UI nodes. FOUR_VALUE_DIALOG_UI(root, node) is the node in a copy of it (or a node's <node>_prefix),
   FOUR_VALUE_DIALOG_UI_FIELD(root, node, offset, type) a class field behind the UiNodeBase of the node. */
typedef struct FourValueDialogUiImage {
    UiPanelControl confirmModeDialogPanel; /* +0000 g_UiPanelControlVtable: Centered panel root of the confirm-new-display-mode dialog. */
    UiNodeBase revertButton; /* +0058 g_UiFramedTextButtonControlVtable: Bottom-left button, action 0x20D, text 0x101 (Cancel); also enqueued when the countdown reaches zero to restore the previous mode. */
    UiTextButtonTemplateFields revertButton_fields;
    UiNodeBase keepModeButton; /* +00B4 g_UiFramedTextButtonControlVtable: Bottom-right button, text 0x100 (OK), keeps the new display mode. */
    UiTextButtonTemplateFields keepModeButton_fields;
    UiListOffsetControl countdownMessageText; /* +0110 g_UiListOffsetControlVtable: Rich text 0x109 with the countdown seconds (+0x5C), tick counter (+0x60), previous mode tuple (+0x64..+0x70) and number buffer (+0x74). */
    uint32_t countdownMessageText_trailing[14]; /* +016C: template dwords behind the control */
} FourValueDialogUiImage;
#define FOUR_VALUE_DIALOG_UI(root, node) (&((FourValueDialogUiImage *)(uintptr_t)(root))->node)
#pragma pack(pop)

#endif /* THANDOR_UI_DIALOGS_TYPES_H */
