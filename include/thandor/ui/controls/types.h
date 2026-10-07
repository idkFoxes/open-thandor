/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TYPES_H
#define THANDOR_UI_CONTROLS_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/ui/text/types.h>

union WidePathBuffer256;
struct UiPageStackControl;
struct UiNodeBase;
struct UiSelectableControl;
struct UiNumericTextControl;
struct UiNodeVtable;
struct UiPointerListControl;
struct UiTextEditControl;
struct UiTextListControl;
struct UiListControl;
struct UiScrollableControl;
struct UiRootNode;
struct RichTextExtent;
struct UiTextButtonControl;
struct UiSpriteButtonControl;
struct UiSpriteButtonDrawOffsets;
struct UiImageControl;
struct UiFramedTextButtonControl;
struct UiTooltipState;
struct UiRequiredTextEditControl;
struct UiSelectionGeometryControl;
struct GraphicsTextureSourceAsset;
struct RuntimeModelFactionPrefix;
struct UiRootCallbacks;

using GraphicsSubresourceIndex = uint32_t;

union WidePathBuffer256 {
    uint16_t codeUnits[256]; 
    uint32_t firstTwoCodeUnits; 
};

enum {
    UI_TEXT_EDIT_VALUE_VALID=1,
    UI_TEXT_EDIT_DRAW_FRAMED_CHROME=8,
    UI_TEXT_EDIT_CARET_VISIBLE_PHASE=16,
    UI_TEXT_EDIT_OVERWRITE_MODE=32,
    UI_TEXT_EDIT_POINTER_SELECTION_ACTIVE=64,
    UI_TEXT_EDIT_READ_ONLY=128,
    UI_TEXT_EDIT_DRAW_TILED_INTERIOR=256,
    UI_TEXT_EDIT_ACTION_ON_ENTER_ONLY=512,
    UI_TEXT_EDIT_PLAY_INTERACTION_SOUND=1024
};
using UiTextEditStateFlags = int;

enum {
    UI_SCROLL_HORIZONTAL_BAR_AT_TOP=1,
    UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM=2,
    UI_SCROLL_VERTICAL_BAR_AT_LEFT=4,
    UI_SCROLL_VERTICAL_BAR_AT_RIGHT=8,
    UI_SCROLL_PRIMARY_INTERACTION_ACTIVE=8192,
    UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE=65536,
    UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE=131072,
    UI_SCROLL_HORIZONTAL_THUMB_ACTIVE=262144,
    UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE=524288,
    UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE=1048576,
    UI_SCROLL_VERTICAL_DECREMENT_ACTIVE=16777216,
    UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE=33554432,
    UI_SCROLL_VERTICAL_THUMB_ACTIVE=67108864,
    UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE=134217728,
    UI_SCROLL_VERTICAL_INCREMENT_ACTIVE=268435456
};
using UiScrollableStateFlags = int;

enum {
    UI_NODE_PREFERRED_FOCUS_TARGET=2,
    UI_NODE_HAS_KEYBOARD_FOCUS=4,
    UI_NODE_SUPPRESSED=8,
    UI_NODE_FALLBACK_FOCUS_TARGET=32,
    UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS=64,
    UI_NODE_REPEAT_OR_DOUBLE_CLICK=128,
    UI_NODE_TOOLTIP_ELIGIBLE=256,
    UI_NODE_TOOLTIP_REFERENCE_DIRECT_UTF16=512
};
using UiNodeFlags = int;

enum {
    UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE=1,
    UI_SELECTABLE_SELECTED_OR_CHECKED=2,
    UI_SELECTABLE_TOGGLE_ON_ACTIVATION=16,
    UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION=8192
};
using UiSelectableStateFlags = int;

enum {
    UI_TEXT_LIST_TYPE_SEARCH_ENABLED=1,
    UI_TEXT_LIST_DEFERRED_ACTION_PENDING=2,
    UI_TEXT_LIST_SELECTION_CONFIRMED=4,
    UI_TEXT_LIST_PLAY_SELECTION_SOUND=8
};
using UiTextListStateFlags = int;

enum {
    UI_REQUIRED_TEXT_VALUE_VALID=1,
    UI_REQUIRED_TEXT_DRAW_FRAMED_CHROME=8,
    UI_REQUIRED_TEXT_CARET_VISIBLE_PHASE=16,
    UI_REQUIRED_TEXT_OVERWRITE_MODE=32,
    UI_REQUIRED_TEXT_POINTER_SELECTION_ACTIVE=64,
    UI_REQUIRED_TEXT_READ_ONLY=128,
    UI_REQUIRED_TEXT_DRAW_TILED_INTERIOR=256,
    UI_REQUIRED_TEXT_ACTION_ON_ENTER_ONLY=512,
    UI_REQUIRED_TEXT_PLAY_INTERACTION_SOUND=1024,
    UI_REQUIRED_TEXT_ESCAPE_CLEARS_AND_QUEUES_ACTION=2048
};
using UiRequiredTextEditStateFlags = int;

enum {
    UI_LIST_DEFERRED_ACTION_PENDING=2,
    UI_LIST_SELECTION_CONFIRMED=4,
    UI_LIST_PLAY_SELECTION_SOUND=8
};
using UiListStateFlags = int;

enum {
    UI_NUMERIC_TEXT_VALUE_VALID=1,
    UI_NUMERIC_TEXT_SIGNED_VALUE=2,
    UI_NUMERIC_TEXT_HEXADECIMAL_FORMAT=4,
    UI_NUMERIC_TEXT_DRAW_FRAMED_CHROME=8,
    UI_NUMERIC_TEXT_CARET_VISIBLE_PHASE=16,
    UI_NUMERIC_TEXT_OVERWRITE_MODE=32,
    UI_NUMERIC_TEXT_POINTER_SELECTION_ACTIVE=64,
    UI_NUMERIC_TEXT_READ_ONLY=128,
    UI_NUMERIC_TEXT_DRAW_TILED_INTERIOR=256,
    UI_NUMERIC_TEXT_ACTION_ON_ENTER_ONLY=512,
    UI_NUMERIC_TEXT_PLAY_INTERACTION_SOUND=1024
};
using UiNumericTextEditStateFlags = int;

using UiPointerWheelDelta = int;

struct UiGridDimensions {
    uint32_t columnCount;
    uint32_t rowCount;
};

using UiTextCodeUnitIndex = uint32_t;

using UiTextCodeUnitCount = uint32_t;

using GraphicsCursorFrameIndex = uint32_t;

using UiNumericValue32 = int;

using UiPageCount = uint32_t;

using UiSerializedRelocationDelta = int;

using UiPixelCoordinate = int;

using UiKeyboardStateMask = uint32_t;

using UiKeyboardEventCode = uint32_t;

using UiNodeFlagMask = uint32_t;

using UiActionId = int;

using UiAnchorFractionQ31 = uint32_t;

using UiPixelOffset = uint32_t;

struct UiNodeBase {
    Ptr32<struct UiNodeBase> nextSibling; 
    Ptr32<struct UiNodeBase> firstChild; 
    Ptr32<struct UiNodeBase> parent; 
    Ptr32<struct UiNodeVtable> vtable; 
    int32_t left; 
    int32_t top; 
    int32_t right; 
    int32_t bottom; 
    int32_t leftOffset; 
    int32_t topOffset; 
    int32_t rightOffset; 
    int32_t bottomOffset; 
    UiAnchorFractionQ31 leftAnchorQ31; 
    UiAnchorFractionQ31 topAnchorQ31; 
    UiAnchorFractionQ31 rightAnchorQ31; 
    UiAnchorFractionQ31 bottomAnchorQ31; 
    int32_t layoutWidth; 
    int32_t layoutHeight; 
    UiNodeFlags nodeFlags; 
};

struct UiNumericTextControl {
    struct UiNodeBase base; 
    UiNumericTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    UiNumericValue32 currentValue; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    Ptr32<struct SoundVoiceSet> activationSound; 
    uint16_t textBuffer[16]; 
    UiNumericValue32 minimumValue; 
    UiNumericValue32 maximumValue; 
};

struct UiSelectableControl {
    struct UiNodeBase base;
    UiSelectableStateFlags stateFlags;
    UiActionId actionId;
};

/* How UiSelectableControl_KeyboardEvent sees its controls: the third dword after the selectable part is
   the activation sound, played when stateFlags bit 0x80 is set (activationSound of UiTextButtonControl and
   UiFramedTextButtonControl, keyboardActivationSound of UiImageControl). The two dwords before it differ
   per subclass. */
struct UiSoundSelectableControl;
struct UiSoundSelectableControl {
    struct UiSelectableControl selectable;
    uint32_t subclassFields[2];
    Ptr32<struct SoundVoiceSet> activationSound;
};

struct UiNodeVtable {
    Ptr32<void (UiSerializedRelocationDelta, struct UiNodeBase *)> relocate; 
    Ptr32<void (struct UiNodeBase *)> method04; // Common one-argument no-op callback; concrete UiNode vtables use UiNode_DefaultMethod04_NoOp.
    Ptr32<void (UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight, UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft, struct UiNodeBase *node)> drawClipped; // Clip rectangle bottom/right first (UiFrame_Draw, UiContainer_DrawIntersectingChildren), as in the texture-source blits.
    Ptr32<void (struct UiNodeBase *)> layout; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> nonRightPress; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> nonRightRelease; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> rightPress; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> rightRelease; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> nonRightDrag; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> rightDrag; 
    Ptr32<GraphicsCursorFrameIndex (UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> pointerMove; 
    Ptr32<UiNodeBase * (UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> hitTest; 
    Ptr32<Bool8 (UiKeyboardStateMask, UiKeyboardEventCode, struct UiNodeBase *)> keyboardEvent;
    Ptr32<void (UiNodeFlagMask, UiNodeFlagMask, struct UiNodeBase *)> applyFlags; 
    Ptr32<void (UiActionId, struct UiNodeBase *)> suppressActionId; 
    Ptr32<void (UiActionId, struct UiNodeBase *)> unsuppressActionId; 
    Ptr32<void (struct UiNodeBase *)> tick; 
    Ptr32<void (UiPointerWheelDelta, UiPixelCoordinate, UiPixelCoordinate, struct UiNodeBase *)> pointerWheel; 
};

struct UiPageStackControl {
    struct UiNodeBase base; 
    UiPageCount pageCount; 
    Ptr32<struct UiNodeBase> pages; 
};

/* Page stack of a template image with its page slots spelled out (g_UiLayoutContainerControlVtable, step 13 U1):
   pageCount followed by PageCount page links (template offsets, UI_TEMPLATE_NO_LINK for an empty page; pointers
   after relocation), 0x50 + 4 * PageCount bytes. The template instances have 1, 2, 3, 4, 8, 9 and 13 pages.
   The class methods (container.cpp) take the one-slot view UiPageStackControl and index &pages[i]; reach it
   with UiLayoutContainerControl_AsPageStack. */
template <UiPageCount PageCount>
struct UiLayoutContainerControl {
    struct UiNodeBase base;
    UiPageCount pageCount; // Equal to PageCount in every template instance.
    Ptr32<struct UiNodeBase> pages[PageCount];
};

/* The UiPageStackControl view of a UiLayoutContainerControl<N> (same prefix: base, pageCount, first page slot). */
template <UiPageCount PageCount>
inline UiPageStackControl *UiLayoutContainerControl_AsPageStack(UiLayoutContainerControl<PageCount> *control)

{
  return reinterpret_cast<UiPageStackControl *>(control);
}

using GraphicsSubresourceEndIndex = uint32_t;

using UiTextResourceId = uint32_t;

using UiListRowCount = uint32_t;

/* The first 0x64 bytes of a UiListControl (g_UiListControlVtable); pages embed it with the column tail in
   the following reserved bytes, and methods that need columnCount/columns view it as UiListControl. */
struct UiPointerListControl {
    struct UiNodeBase base; 
    UiListStateFlags listStateFlags; 
    Ptr32<Ptr32<void>> rowSlots; 
    UiListRowCount rowCount; 
    UiPixelExtent rowHeight; 
    UiActionId actionId; 
    Ptr32<Ptr32<void>> selectedRowSlot; 
};

using UiPackedTextStyle = uint32_t;

using UiPageIndex = uint32_t;

using UiPointerButtonMask = uint32_t;

using UiFrameCount = uint32_t;

using SerializedImageRelocationDelta = int;

using UiFrameDelayFrames = uint32_t;

using UiPointerListFieldByteOffset = uint32_t;

using GraphicsSubresourceOffset = uint32_t;

struct UiScrollableViewportSize {
    UiPixelExtent width;
    UiPixelExtent height;
};

using UiControlCount = uint32_t;

struct UiTextEditControl {
    struct UiNodeBase base; 
    UiTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    uint32_t bufferCapacityCodeUnits; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    Ptr32<struct SoundVoiceSet> activationSound; 
    uint16_t textBuffer[10]; 
};

struct UiTextListControl {
    struct UiNodeBase base; 
    UiTextListStateFlags listStateFlags; 
    Ptr32<Ptr32<uint16_t>> rowTextSlots; 
    UiListRowCount rowCount; 
    UiPixelExtent rowHeight; 
    UiActionId actionId; 
    Ptr32<Ptr32<uint16_t>> selectedRowSlot; 
    Ptr32<struct SoundVoiceSet> activationSound; 
};

/* One column of a UiListControl: the rich-text string at rowRecord + rowTextOffset is drawn in a column
   of width pixels; a negative width is a right-aligned column of -width pixels. */
struct UiListColumn;
struct UiListColumn {
    int32_t width;
    uint32_t rowTextOffset;
};

/* Multi-column text list (g_UiListControlVtable): rowSlots points at rowCount row records whose
   column strings are drawn per columns[]. columns is variable length (columnCount entries: 2, 3 or 5 in
   the templates, instance sizes 0x7C..0x98); three templates carry one more dword (0x21DB..0x21DD) after
   the columns that no list method reads. UiPointerListControl is the 0x64-byte prefix of this class. */
struct UiListControl {
    struct UiNodeBase base;
    UiListStateFlags listStateFlags;
    Ptr32<Ptr32<void>> rowSlots;
    UiListRowCount rowCount;
    UiPixelExtent rowHeight;
    UiActionId actionId;
    Ptr32<Ptr32<void>> selectedRowSlot;
    uint32_t columnCount;
    Ptr32<struct SoundVoiceSet> activationSound;
    struct UiListColumn columns[1];
};

/* The UiPointerListControl view of a UiListControl (same first 0x64 bytes); the UiPointerList_* methods take it. */
inline UiPointerListControl *UiListControl_AsPointerList(UiListControl *control)

{
  return reinterpret_cast<UiPointerListControl *>(control);
}

struct UiScrollableControl {
    struct UiNodeBase base; 
    UiScrollableStateFlags scrollStateFlags; 
    UiPixelExtent viewportWidth; 
    UiPixelExtent viewportHeight; 
    UiPixelExtent contentWidth; 
    UiPixelExtent contentHeight; 
    UiPixelOffset contentOriginX; 
    UiPixelOffset contentOriginY; 
    UiPixelOffset scrollOffsetX; 
    UiPixelOffset scrollOffsetY; 
    UiPixelCoordinate pointerAnchorX; 
    UiPixelCoordinate pointerAnchorY; 
    UiPixelCoordinate horizontalThumbLeft; 
    UiPixelCoordinate verticalThumbTop; 
    UiPixelCoordinate horizontalThumbRight; 
    UiPixelCoordinate verticalThumbBottom; 
    UiPixelOffset autoScrollStepX; 
    UiPixelOffset autoScrollStepY; 
};

enum {
    UI_ROOT_DISABLE_POINTER_HIT_TEST=256 
};
using UiRootFlags = int;

struct UiRootNode {
    struct UiNodeBase base;
    UiRootFlags rootFlags;
    Ptr32<struct UiRootCallbacks> callbacks;
    Ptr32<struct UiRootNode> previousRoot;
};

struct UiPanelControl;
/* Panel (g_UiPanelControlVtable): the plain root-capable container; rootFlags bit 0x1 tiled background, 0x2 frame, 0x200 alternate background/frame. */
struct UiPanelControl {
    struct UiRootNode root;
};

struct UiResizableWindowControl;
/* Resizable window (g_UiResizableWindowControlVtable): root-capable window with title bar, close/maximize buttons, move and resize. */
struct UiResizableWindowControl {
    struct UiRootNode root; // rootFlags: 0x1 tiled interior, 0x2 frame, 0x4 title bar, 0x8 close button, 0x10 maximize button, 0x20 movable, 0x40 resizable, 0x80 maximized, 0x800/0x1000 close/maximize pressed, 0x2000 moving, 0x4000 resizing, 0x80000/0x100000 close/maximize armed, 0xFF000000 resized edges.
    UiTextResourceId titleTextResourceId;
    uint32_t field5C; // Not read by any window method; zero in the template.
    int32_t restoredLeft; // Rectangle saved on maximize and restored on un-maximize.
    int32_t restoredTop;
    int32_t restoredRight;
    int32_t restoredBottom;
    int32_t dragAnchorXOrPendingRight; // Move: pointer x relative to left at grab time. Resize: new right edge being applied.
    int32_t dragAnchorYOrPendingBottom; // Move: pointer y relative to top at grab time. Resize: new bottom edge being applied.
};

struct UiTitledWindowControl;
/* Titled box (g_UiTitledWindowControlVtable): framed group box with a title text in its top edge. */
struct UiTitledWindowControl {
    struct UiNodeBase base;
    uint32_t titleFlags; // Bit 0x1: title centered (otherwise at the left corner).
    UiTextResourceId titleTextResourceId;
};

struct RichTextExtent {
    uint32_t widthPixels; 
    uint32_t heightPixels; 
};

struct UiTextButtonControl {
    struct UiSelectableControl selectable; 
    UiTextResourceId textResourceId; 
    UiPackedTextStyle packedTextStyle;
    Ptr32<struct SoundVoiceSet> activationSound; // Click sound played on activation (UI_BUTTON_PLAY_ACTIVATION_SOUND); set by the UI initialisers from g_UiButtonSoundVoiceSets7.
};

/* Text button that formats two numbers into rich-text payload selectors 0 and 1 of its text before drawing
   (g_UiNumericPairTextButtonVtable; e.g. the display-resolution options: width, height). 0x68 bytes. */
struct UiNumericPairTextButton;
struct UiNumericPairTextButton {
    struct UiTextButtonControl base;
    int32_t firstValue;
    int32_t secondValue;
};

/* Text button that patches two payload pointers into rich-text payload selectors 0 and 1 of its text before
   drawing (g_UiPayloadPairTextButtonVtable; the display-adapter options). 0x68 bytes. */
struct UiPayloadPairTextButton;
struct UiPayloadPairTextButton {
    struct UiTextButtonControl base;
    Ptr32<void> firstPayload;
    Ptr32<void> secondPayload;
};

struct UiSpriteButtonDrawOffsets {
    int8_t normalX; 
    int8_t normalY; 
    int8_t selectedX; 
    int8_t selectedY; 
};

struct UiSpriteButtonControl {
    struct UiSelectableControl selectable; 
    Ptr32<struct GraphicsTextureSourceAsset> primaryTextureSource; 
    uint32_t normalSubresourceStartOrDescriptor; 
    struct UiSpriteButtonDrawOffsets drawOffsets; 
    GraphicsSubresourceIndex selectedSubresourceStart; 
    GraphicsSubresourceEndIndex normalSubresourceEndExclusive; 
    GraphicsSubresourceEndIndex selectedSubresourceEndExclusive; 
    GraphicsSubresourceOffset animationFrameOffset; 
    Ptr32<struct SoundVoiceSet> activationSound; // Click sound played on activation; set by the UI initialisers from g_UiButtonSoundVoiceSets7.
    Ptr32<struct GraphicsTextureSourceAsset> alternateTextureSource;
};

struct UiImageControl {
    struct UiSelectableControl selectable; 
    Ptr32<struct GraphicsTextureSourceAsset> textureSource; 
    GraphicsSubresourceIndex normalSubresource; 
    Ptr32<struct SoundVoiceSet> keyboardActivationSound; // Sound of a keyboard activation (read through UiSoundSelectableControl.activationSound).
    GraphicsSubresourceIndex alternateSubresource;
    Ptr32<struct UiNodeBase> activeChild;
    Ptr32<struct SoundVoiceSet> pointerActivationSound; // Sound played when the image opens/closes on a click; set by the UI initialisers from g_UiButtonSoundVoiceSets7.
};

/* Image/movie surface that queues one action on left and one on right click (g_UiImageActionControlVtable).
   letterboxWidth is only read with UI_IMAGE_ACTION_STRETCH set; most template instances end before it (0x64
   bytes, the end movie view is 0x68). */
struct UiImageActionControl;
struct UiImageActionControl {
    struct UiNodeBase base;
    uint32_t displayFlags; /* 1: stretch to the node, 2: Enter queues primaryActionId, 4: letterbox to letterboxWidth */
    GraphicsCursorFrameIndex cursorFrame;
    Ptr32<struct GraphicsTextureSourceAsset> textureSource;
    GraphicsSubresourceIndex subresource;
    UiActionId primaryActionId;
    UiActionId secondaryActionId;
    int32_t letterboxWidth;
};

/* A UiImageActionControl node of a template image that ends before letterboxWidth (0x64 bytes; the frontend's
   briefingImage): the class methods read letterboxWidth only with displayFlags bit 4, which such a node never
   sets. Same prefix as UiImageActionControl. */
struct UiImageActionTemplateNode {
    struct UiNodeBase base;
    uint32_t displayFlags;
    GraphicsCursorFrameIndex cursorFrame;
    Ptr32<struct GraphicsTextureSourceAsset> textureSource;
    GraphicsSubresourceIndex subresource;
    UiActionId primaryActionId;
    UiActionId secondaryActionId;
};

/* Framed text box drawing lineCount rich-text lines; left clicks queue actionId while it has lines
   (g_UiConditionalActionControlVtable). textLines is variable length: 1, 5 or 8 slots in the templates. */
struct UiConditionalActionControl;
struct UiConditionalActionControl {
    struct UiNodeBase base;
    uint32_t field4C;
    GraphicsCursorFrameIndex cursorFrame;
    UiActionId actionId;
    uint32_t lineCount;
    Ptr32<uint16_t> textLines[1];
};

/* A UiConditionalActionControl of a template image with its text line slots spelled out (step 13 U3):
   0x5C + 4 * LineSlots bytes, same prefix. The class methods (buttons.cpp) take the one-slot view; reach it
   with UiConditionalActionTextBox_AsControl. */
template <uint32_t LineSlots>
struct UiConditionalActionTextBox {
    struct UiNodeBase base;
    uint32_t field4C;
    GraphicsCursorFrameIndex cursorFrame;
    UiActionId actionId;
    uint32_t lineCount;
    Ptr32<uint16_t> textLines[LineSlots];
};

/* The UiConditionalActionControl view of a UiConditionalActionTextBox<N> (same prefix and first line slot). */
template <uint32_t LineSlots>
inline UiConditionalActionControl *UiConditionalActionTextBox_AsControl(UiConditionalActionTextBox<LineSlots> *control)
{
  return reinterpret_cast<UiConditionalActionControl *>(control);
}

/* A results chart of the end-of-game results screen (g_FrontendResultsTableVtable; the in-game template's
   resultsChart1..3) with its column type list spelled out (step 13 X9e): 0x64 + 4 * ColumnSlots bytes. The class
   methods (ui/frontend/results.cpp) get it through the vtable as the one-slot view
   FrontendResultsColumnSequenceControl (ui/frontend/types.h, same prefix). */
struct FrontendResultsFactionWeightPair;
template <uint32_t ColumnSlots>
struct FrontendResultsTable {
    struct UiNodeBase base;
    uint32_t modeFlags; /* +0x4C bit 0 (FRONTEND_RESULTS_MODE_GRAPH): faction-weight graph instead of the table */
    Ptr32<void (UiPixelCoordinate, UiPixelCoordinate, UiPixelCoordinate, struct FrontendResultsFactionWeightPair *)>
        factionWeightRaster; /* +0x50 graph mode: draws one pixel column */
    uint32_t columnTypeCount; /* +0x54 used entries of columnTypes */
    uint32_t rowCount; /* +0x58 rows (active factions), set by the end-of-game results screen */
    int headerBaselineOffsetPixels; /* +0x5C (UiPixelMetric) */
    int rowAdvancePixels; /* +0x60 (UiPixelMetric) */
    uint32_t columnTypes[ColumnSlots]; /* +0x64 FRONTEND_RESULTS_COLUMN_* */
};

struct UiFramedTextButtonControl {
    struct UiSelectableControl selectable; 
    UiTextResourceId textResourceId; 
    UiPackedTextStyle packedTextStyle;
    Ptr32<struct SoundVoiceSet> activationSound; // Click sound played on activation (UI_BUTTON_PLAY_ACTIVATION_SOUND); set by the UI initialisers from g_UiButtonSoundVoiceSets7.
};

struct UiWindowControl;
/* Framed icon-and-text button (g_UiWindowControlVtable); shares the UiFramedTextButtonControl input methods, which only rely on the UiSelectableControl prefix. No template instance; the size past 0x68 is unknown. */
struct UiWindowControl {
    struct UiSelectableControl selectable;
    Ptr32<struct GraphicsTextureSourceAsset> iconTextureSource;
    GraphicsSubresourceIndex iconSubresource;
    struct UiSpriteButtonDrawOffsets iconDrawOffsets; // Offset of the dimmed icon copy drawn under the icon: normal pair, or selected pair when selected.
    UiTextResourceId textResourceId;
    UiPackedTextStyle packedTextStyle;
};

struct UiTooltipState {
    UiFrameCount countdownFrames; 
    Ptr32<struct UiNodeBase> targetNode; 
    UiPixelCoordinate pointerX; 
    UiPixelCoordinate pointerY; 
};

enum /* UiPointerCaptureButton, stored in 1 byte(s) */ {
    UI_POINTER_CAPTURE_LEFT=0,
    UI_POINTER_CAPTURE_MIDDLE=1,
    UI_POINTER_CAPTURE_RIGHT=2,
    UI_POINTER_CAPTURE_NONE=255
};
using UiPointerCaptureButton = uint8_t;

struct UiRequiredTextEditControl {
    struct UiNodeBase base; 
    UiRequiredTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    UiTextCodeUnitCount bufferCapacityCodeUnits; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    Ptr32<struct SoundVoiceSet> activationSound; 
    uint16_t textBuffer[10]; 
};

struct UiSelectionGeometryControl {
    struct UiNodeBase base; // Common UI-node prefix.
    uint32_t reserved4C; // Serialized/runtime slot not required by the two recovered methods; intentionally unresolved.
    Q12 sourceOriginXQ12; // Source-space X (texture column, Q12) shown at the control's centre; the sampling transform steps its column from it. The minimap stores the clicked grid column here on recentre.
    Q12 sourceOriginYQ12; // Source-space Y (texture row, Q12) shown at the control's centre; the minimap stores the clicked grid row here on recentre.
    Q12 sampleScaleQ12; // Scale multiplied by the fixed sine/cosine tables before the sampling transform.
    AngleTurn32 rotationAngle; // Fixed-turn angle indexing the global Q28 sine/cosine tables.
    Ptr32<struct GraphicsTextureSourceAsset> textureSource; // Texture-source asset. DrawClipped resolves its GraphicsTextureSourceEntry table and samples direct-color pixels.
    UiActionId actionId; // Action enqueued by ConvertPointerAndEnqueueAction after writing the transformed source coordinates.
    Q12 selectedSourceXQ12; // Pointer position transformed into source-space X (texture column; the minimap's grid column) before action dispatch.
    Q12 selectedSourceYQ12; // Pointer position transformed into source-space Y (texture row; the minimap's grid row) before action dispatch.
};

/* Single-line rich-text label (g_UiSingleLineTextControlVtable, g_UiCommandVisibilitySingleLineTextVtable) that
   optionally forwards focus and pointer/keyboard input to one child (a slider or button it frames). 0x5C bytes;
   template extents beyond 0x5C are unrelated data placed after the node. */
struct UiSingleLineTextControl;
struct UiSingleLineTextControl {
    struct UiNodeBase base;
    uint32_t labelFlags; // 1 center X, 2 align right, 4 center Y, 8 align bottom, 0x10 text is a command stream (else a TextResourceId), 0x20 text pointer still needs relocation, 0x40 hide while suppressed, 0x100/0x200 keep the style override bytes 3/2, 0x400 pointer-wheel forwarding in progress, 0x800/0x1000 command-visibility conditions, 0x8000 do not forward navigation keys 0x30.
    Ptr32<struct UiNodeBase> focusChild; // Child that receives focus and forwarded input; relocated, may be null.
    Ptr32<uint16_t> text; // Rich-text command stream, or a TextResourceId when labelFlags & 0x10 is clear.
    UiPackedTextStyle styleOverride; // Packed style bits OR-ed over g_UiTextStyleNormal (top two bytes used).
};

/* Wrapped multi-line rich-text block (g_UiWrappedTextControlVtable, g_UiCommandVisibilityWrappedTextVtable).
   0x5C bytes; template extents beyond 0x5C are unrelated data placed after the node (e.g. countdown state). */
struct UiWrappedTextControl;
struct UiWrappedTextControl {
    struct UiNodeBase base;
    uint32_t labelFlags; // 0x10 text is a command stream (else a TextResourceId), 0x20 text pointer still needs relocation, 0x40 keep wrapWidth (else it follows layoutWidth), 0x100/0x200 keep the style override bytes 3/2, 0x800 command-visibility condition.
    UiPixelExtent wrapWidth; // Maximum line width for wrapping.
    Ptr32<uint16_t> text; // Rich-text command stream, or a TextResourceId when labelFlags & 0x10 is clear.
    UiPackedTextStyle styleOverride; // Packed style bits OR-ed over g_UiTextStyleNormal (top two bytes used).
};

/* Control types of the template vtables whose methods already work on one of the structs above (step 13 U1).
   The template images (InGameUiImage, FrontendUiImage, the dialog images) name each node by its vtable; these
   names give every such vtable its control type, so a member `UiNodeBase x; uint32_t x_fields[N];` can become
   `UiFocusProxyControl x;` with the same bytes. A template member with more dwords than the type (e.g. 5 instead
   of the 4 of UiSingleLineTextControl) is followed by data that belongs to the next node (mostly its tooltip text
   id) and keeps those dwords as a separate trailing member.
   - g_UiFocusProxyControlVtable: a single-line label that forwards focus, pointer, wheel, keyboard and tick to its
     focusChild (UiSingleLineTextControl_* handlers, focus_proxy.cpp); 0x5C bytes, 4 dwords after UiNodeBase.
   - g_UiListOffsetControlVtable: a wrapped multi-line text (UiWrappedTextControl_RelocateAndApplyDeferredOffset,
     UiWrappedTextControl_DrawClipped); 0x5C bytes, 4 dwords after UiNodeBase. */
using UiFocusProxyControl = UiSingleLineTextControl;
using UiListOffsetControl = UiWrappedTextControl;

/* Horizontal or vertical range slider with a draggable thumb (g_UiRangeSliderControlVtable); every value change
   enqueues actionId. 0x68 bytes; the display-settings template allocates only 0x64 bytes for its two color
   sliders, which never set sliderFlags & 4 and so never touch clickSound. */
struct UiRangeSliderControl;
struct UiRangeSliderControl {
    struct UiNodeBase base;
    uint32_t sliderFlags; // 1 vertical (else horizontal), 2 thumb drag in progress, 4 play clickSound on press/release/key step, 8 reversed direction.
    int32_t minimumValue;
    int32_t maximumValue;
    int32_t value; // Current value, kept within minimumValue..maximumValue.
    int32_t stepValue; // Increment per arrow key; per wheel notch it is scaled by g_UiRangeSliderDragScale.
    UiActionId actionId; // Enqueued on every value change; also the id matched by suppress/unsuppress.
    Ptr32<struct SoundVoiceSet> clickSound; // Set at runtime; may be null.
};

/* Horizontal progress gauge drawing a framed fill for value within minimumValue..maximumValue, optionally with a
   centered percent label. Only g_UiTransferProgressGaugeVtable uses it (the original's plain gauge vtable had no
   instance and was removed); it first reloads the range from the network transfer mailbox (file-transfer
   progress). 0x5C bytes. */
struct UiHorizontalGaugeControl;
struct UiHorizontalGaugeControl {
    struct UiNodeBase base;
    uint32_t gaugeFlags; // 1 draw the percent label.
    uint32_t minimumValue;
    uint32_t maximumValue; // Compared unsigned with value.
    uint32_t value;
};

/* Panel drawing one texture subresource aligned (or stretched) inside its layout box, optionally over a drop shadow,
   and hit-testing the texture's opaque pixels (g_UiImagePanelControlVtable). Neither the texture nor the children
   are drawn while textureSource is null. 0x5C bytes. */
struct UiImagePanelControl;
struct UiImagePanelControl {
    struct UiNodeBase base;
    uint32_t panelFlags; // 1 center X, 2 align right, 4 center Y, 8 align bottom, 0x10 draw a drop shadow, 0x20 never hit, 0x40 hit the whole box (skip the opaque-pixel test), 0x80 stretch the texture over the layout box.
    int8_t shadowOffsetX; // Drop-shadow offset from the texture position (panelFlags & 0x10).
    int8_t shadowOffsetY;
    uint16_t reserved52;
    Ptr32<struct GraphicsTextureSourceAsset> textureSource;
    GraphicsSubresourceIndex subresource;
};

/* Image panel that also draws the metrics (portrait, health, value) of one selected entity over its texture
   (g_UiArmyMetricsPanelVtable; shares UiImagePanelControl_HitTestAlignedTextureAndChildren). Its draw ignores the
   drop-shadow and stretch flags. 0x60 bytes. */
struct UiArmyMetricsPanel;
struct UiArmyMetricsPanel {
    struct UiImagePanelControl base;
    Ptr32<struct RuntimeModelFactionPrefix> entity; // Drawn by SelectionPanel_RenderArmyRuntimeMetrics; null draws only the texture.
};

/* Panel filling its box with a solid ARGB color, or tiling a texture subresource over it
   (g_UiFillPanelControlVtable). 0x5C bytes. */
struct UiFillPanelControl;
struct UiFillPanelControl {
    struct UiNodeBase base;
    uint32_t fillFlags; // 1 tile horizontally, 2 tile vertically (else one tile), 0x10 draw a drop shadow under each tile.
    int8_t shadowOffsetX; // Drop-shadow offset from each tile (fillFlags & 0x10).
    int8_t shadowOffsetY;
    uint16_t reserved52;
    Ptr32<struct GraphicsTextureSourceAsset> textureSource; // Null: fill the box with the color instead.
    GraphicsSubresourceIndex subresourceOrFillArgb; // Subresource of textureSource; the ARGB fill color while textureSource is null.
};

/* Panel drawing a nine-slice texture frame: four corners, four tiled edges and a tiled center
   (g_UiNineSlicePanelControlVtable). 0x5C bytes; a template extent of 0x60 includes the next node's tooltip id. */
struct UiNineSlicePanelControl;
struct UiNineSlicePanelControl {
    struct UiNodeBase base;
    uint32_t field4C; // Not read by the class methods; 0 in every template.
    Ptr32<struct GraphicsTextureSourceAsset> textureSource;
    GraphicsSubresourceIndex firstFrameSubresource; // Eight consecutive subresources: +0 top-left, +1 top-right, +2 top edge, +3 left edge, +4 right edge, +5 bottom-left, +6 bottom-right, +7 bottom edge.
    GraphicsSubresourceIndex centerSubresource; // Tiled over the interior.
};

/* Software-rendered view blending two subresources of a texture through a per-pixel mask, enqueueing actionId on a
   press or key (g_UiSoftwareTexturePreviewControlVtable; the frontend movie view). The mask animation
   (SoftwareMaskBuffer_*) addresses the same object as SoftwareMaskRuntimeView. 0x6C bytes. */
struct UiSoftwareTexturePreviewControl;
struct UiSoftwareTexturePreviewControl {
    struct UiNodeBase base;
    uint32_t field4C; // Not read by the class methods; 0 in the template.
    Ptr32<struct GraphicsTextureSourceAsset> textureSource; // Nothing is drawn while null.
    GraphicsSubresourceIndex outgoingSubresource; // Blend source B (SoftwareMaskRuntimeView.outgoingSubresource).
    GraphicsSubresourceIndex incomingSubresource; // Blend source A (SoftwareMaskRuntimeView.incomingSubresource).
    UiActionId actionId; // Enqueued on primary/secondary press and on every key except 0x10002 (which moves focus).
    Ptr32<uint64_t> blendFactorPixels; // Per-pixel blend mask (SoftwareMaskRuntimeView.maskPixels).
    Ptr32<uint64_t> blendedSourcePixels;
    int32_t tickCounter; // Mask animation tick (SoftwareMaskRuntimeView.tickCounter).
};

/* Resource gauge (g_UiFormattedContainerVtable; xenite, tritium and energy on the in-game resource panel): a
   three-part bar filled to currentValue with a marker at limitValue, both scaled against initialScaleRange
   (multiplied by 4 until every value fits); the fill texture variant follows currentValue / limitValue. The values
   are also formatted into the node's tooltip text (payload selectors 0, 1 and 2), whose TextResourceId is the dword
   in front of the node. 0x94 bytes; with gaugeFlags & 2 the node continues as UiFormattedContainerWithMarker. */
struct UiFormattedContainer;
struct UiFormattedContainer {
    struct UiNodeBase base;
    uint32_t gaugeFlags; // 1 two-sided variant scale (below 40% and above 85% both use the late variants; else they rise from 80%), 2 has the markerValue tail (UiFormattedContainerWithMarker).
    int32_t currentValue;
    int32_t limitValue; // Nothing but the texts is drawn while 0.
    Ptr32<struct GraphicsTextureSourceAsset> textureSource;
    GraphicsSubresourceIndex firstFrameSubresource; // +0/+1/+2 empty bar left/middle/right, +3..+0x14 six fill variants of three parts each, +0x15 marker.
    uint16_t currentValueTextUtf16[12]; // Tooltip payload 0.
    uint16_t limitValueTextUtf16[12]; // Tooltip payload 1.
    int32_t initialScaleRange;
};

/* Resource gauge with a second marker (gaugeFlags & 2; the energy gauge). 0xB0 bytes. */
struct UiFormattedContainerWithMarker;
struct UiFormattedContainerWithMarker {
    struct UiFormattedContainer base;
    int32_t markerValue; // Second marker; also caps the fill-percentage divisor when smaller than limitValue.
    uint16_t markerValueTextUtf16[12]; // Tooltip payload 2.
};

/* The two handlers of the root-stack action page: UiRootStack_Pop, FatalErrorDialog_DismissAndPopRoot. */
struct UiRootStackActionHandlerPage2;
struct UiRootStackActionHandlerPage2 {
    Ptr32<void (void *)> handlers[2];
};
using UiRootPointerMissPolicyCallback = int (UiRootNode * root);

#endif /* THANDOR_UI_CONTROLS_TYPES_H */
