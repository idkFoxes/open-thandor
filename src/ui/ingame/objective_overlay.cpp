/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/objective_overlay.cpp
 * Project code (not in the original game)
 */

/* The objective overlay of the in-game UI (include/thandor/ui/ingame/objective_overlay.h, docs/plans/
   objective_overlay.md section 4).

   Why a run-time linked node and not a template node: a node appended to the in-game template would have to grow
   InGameUiImage and the parallel InGameRuntimeRoot (InGameSession_CreateRoot copies sizeof(InGameRuntimeRoot) / 4
   dwords of the template), move the root's reserved tail, and change the layout asserts and the uitemplate golden -
   for one box that needs no template data. The node here lives in module storage (below 2 GB like all game data,
   so the 32-bit links hold it), is linked into each new root after UiRootStack_Push relocated the tree (the
   relocation walk stops at the first sibling that is already relocated, so it must not see this node) and is
   unlinked before the root is freed. The template, the root layout and every original offset stay untouched; with
   the overlay hidden the frame is the original one.

   All drawing goes through the framebuffer slots (fill, win.gfx frame blits, the rich-text line) in logical
   pixels, so the GPU renderers record it and scale it with the UI scale like every other control: the box keeps
   its place and size relative to the UI at UI scale 1, 2 and 3. */

#include <thandor/ui/ingame/objective_overlay.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <thandor/thandor.h>
#include <thandor/gameplay/session/mission_objective.h>
#include <thandor/platform/bootstrap/image.h>

namespace {

/* Placement (logical pixels): left gap to the world view's edge, gap below the message lines / status lines. */
constexpr int32_t OVERLAY_LEFT_GAP = 8;
constexpr int32_t OVERLAY_TOP_GAP = 6;
/* Room between the frame and the text, and between the two lines. */
constexpr int32_t OVERLAY_PADDING_X = 6;
constexpr int32_t OVERLAY_PADDING_Y = 3;
constexpr int32_t OVERLAY_LINE_GAP = 2;
/* Widest box: OVERLAY_MAX_WIDTH, but at most half the world view (small software-renderer modes). */
constexpr int32_t OVERLAY_MAX_WIDTH = 480;
constexpr int32_t OVERLAY_MIN_WIDTH = 160;
/* Background inside the frame: dark grey at 63 % (the world stays visible through it). */
constexpr PackedArgb32 OVERLAY_BACKGROUND_ARGB = 0xA0101010;
/* The frame: the thin button frame of win.gfx; its pressed variant while the box is held down. */
constexpr GraphicsSubresourceIndex OVERLAY_FRAME = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
constexpr GraphicsSubresourceIndex OVERLAY_FRAME_PRESSED =
    UI_WINDOW_SUBRESOURCE_BUTTON_FRAME + UI_WINDOW_FRAME_PIECE_COUNT;
/* UI action of the mission objectives button (InGameMissionHelpPage_Toggle). */
constexpr UiActionId INGAME_ACTION_MISSION_HELP = 0x101F;
/* The label in front of line 1 (German texts, else English, like the other project texts). */
constexpr std::array<uint16_t, 7> OVERLAY_LABEL_GERMAN = {u'Z', u'i', u'e', u'l', u':', u' ', 0};
constexpr std::array<uint16_t, 12> OVERLAY_LABEL_ENGLISH = {u'O', u'b', u'j', u'e', u'c', u't', u'i', u'v', u'e', u':',
                                                            u' ', 0};
/* The ellipsis: U+2026 when the font has it, else Windows-1252 0x85, else three full stops. */
constexpr uint16_t ELLIPSIS_UNICODE = 0x2026;
constexpr uint16_t ELLIPSIS_CP1252 = 0x85;

using OverlayLine = std::array<uint16_t, OBJECTIVE_OVERLAY_LINE_UNITS>;

/* The lines as set, and their fitted copies (cut with an ellipsis) for the last box width. */
struct OverlayLayout {
    bool valid = false;
    int32_t maxTextWidth = 0; /* the width the copies were fitted to */
    bool german = true;       /* the label language they were made for */
    OverlayLine label{};
    int32_t labelWidth = 0;
    OverlayLine fitted1{};
    OverlayLine fitted2{};
    int32_t width1 = 0;
    int32_t width2 = 0;
    int32_t lineHeight = 0;
};

struct OverlayState {
    OverlayLine line1{};
    OverlayLine line2{};
    bool visible = true;
    bool pressed = false;
    InGameRuntimeRoot *root = nullptr; /* the root the node is linked into */
    OverlayLayout layout;
};

OverlayState s_Overlay;

/* The objective update (ObjectiveOverlay_Update): what line 1 was built for, and when the progress is due. */
constexpr std::chrono::milliseconds OVERLAY_REFRESH_INTERVAL{500};
struct OverlayUpdateState {
    uint32_t attachGeneration = 0; /* ObjectiveOverlay_AttachToRoot calls */
    bool built = false;
    uint32_t builtGeneration = 0;
    const InGameLevelConditionStorage *builtStorage = nullptr;
    uint32_t builtFaction = 0;
    uint32_t builtLanguage = 0;
    bool enabled = true;
    bool sample = false; /* OPEN_THANDOR_OBJECTIVE_SAMPLE set the lines: no update */
    std::chrono::steady_clock::time_point nextRefresh{};
    MissionObjective objective;
};
OverlayUpdateState s_Update;

/* The node: covers the world view area (anchors 0..1); the box is computed at draw and hit-test time. */
UiNodeBase s_OverlayNode = {
    .nextSibling = UI_NODE_NONE, .firstChild = UI_NODE_NONE, .parent = UI_NODE_NONE,
    .vtable = THANDOR_PTR(&g_InGameObjectiveOverlayVtable),
    .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000};

/* Copies text into line: control characters become spaces, rich-text command units '?'. */
void Overlay_CopyLine(OverlayLine &line, const uint16_t *text)
{
    size_t count = 0;
    if (text != nullptr) {
        for (; count + 1 < line.size() && text[count] != 0; count++) {
            uint16_t unit = text[count];
            line[count] = unit < 0x20 ? u' ' : unit >= 0x8000 ? u'?' : unit;
        }
    }
    std::fill(line.begin() + static_cast<std::ptrdiff_t>(count), line.end(), 0);
}

size_t Overlay_Length(const OverlayLine &line)
{
    size_t count = 0;
    while (count < line.size() && line[count] != 0) {
        count++;
    }
    return count;
}

int32_t Overlay_TextWidth(UiPackedTextStyle style, const uint16_t *text)
{
    return static_cast<int32_t>(RichTextCommandStream_MeasureLine(style, const_cast<uint16_t *>(text)).widthPixels);
}

/* fitted = text, or its longest prefix (trailing blanks dropped) plus the ellipsis that fits maxWidth. Returns the
   width of fitted. */
int32_t Overlay_FitLine(OverlayLine &fitted, const OverlayLine &text, UiPackedTextStyle style, int32_t maxWidth)
{
    fitted = text;
    int32_t width = Overlay_TextWidth(style, fitted.data());
    if (width <= maxWidth) {
        return width;
    }
    std::array<uint16_t, 4> ellipsis{};
    if (FontGlyph_GetLogicalSizeForStyle(style, ELLIPSIS_UNICODE, nullptr) != 0) {
        ellipsis = {ELLIPSIS_UNICODE, 0, 0, 0};
    } else if (FontGlyph_GetLogicalSizeForStyle(style, ELLIPSIS_CP1252, nullptr) != 0) {
        ellipsis = {ELLIPSIS_CP1252, 0, 0, 0};
    } else {
        ellipsis = {u'.', u'.', u'.', 0};
    }
    size_t ellipsisLength = 0;
    while (ellipsis[ellipsisLength] != 0) {
        ellipsisLength++;
    }
    size_t keep = Overlay_Length(text);
    while (keep > 0) {
        keep--;
        while (keep > 0 && text[keep - 1] == u' ') {
            keep--;
        }
        if (keep + ellipsisLength + 1 > fitted.size()) {
            continue;
        }
        std::copy_n(text.begin(), keep, fitted.begin());
        std::copy_n(ellipsis.begin(), ellipsisLength + 1, fitted.begin() + static_cast<std::ptrdiff_t>(keep));
        width = Overlay_TextWidth(style, fitted.data());
        if (width <= maxWidth) {
            return width;
        }
    }
    std::copy_n(ellipsis.begin(), ellipsisLength + 1, fitted.begin());
    return Overlay_TextWidth(style, fitted.data());
}

/* The fitted lines for the widest text the box allows (cached until the lines, the width or the language change). */
const OverlayLayout &Overlay_Layout(int32_t maxBoxWidth)
{
    OverlayLayout &layout = s_Overlay.layout;
    bool german = TextResource_ProjectTextsAreGerman();
    if (layout.valid && layout.maxTextWidth == maxBoxWidth && layout.german == german) {
        return layout;
    }
    layout.valid = true;
    layout.maxTextWidth = maxBoxWidth;
    layout.german = german;
    layout.label.fill(0);
    if (german) {
        std::copy(OVERLAY_LABEL_GERMAN.begin(), OVERLAY_LABEL_GERMAN.end(), layout.label.begin());
    } else {
        std::copy(OVERLAY_LABEL_ENGLISH.begin(), OVERLAY_LABEL_ENGLISH.end(), layout.label.begin());
    }
    RichTextExtent labelExtent = RichTextCommandStream_MeasureLine(g_UiTextStyleSelected, layout.label.data());
    layout.labelWidth = static_cast<int32_t>(labelExtent.widthPixels);
    layout.lineHeight = static_cast<int32_t>(labelExtent.heightPixels);
    int32_t textRoom = std::max<int32_t>(0, maxBoxWidth - 2 * OVERLAY_PADDING_X - layout.labelWidth);
    layout.width1 = Overlay_FitLine(layout.fitted1, s_Overlay.line1, g_UiTextStyleNormal, textRoom);
    layout.width2 = Overlay_FitLine(layout.fitted2, s_Overlay.line2, g_UiTextStyleNormal, textRoom);
    return layout;
}

InGameUiImage *Overlay_Image()
{
    if (s_Overlay.root == nullptr || s_Overlay.root != g_InGameRuntimeRoot) {
        return nullptr;
    }
    return InGameUi_Image(s_Overlay.root);
}

/* Bottom of the faction status lines at the top left of the world view (worldViewWrappedStatusText) while they are
   drawn, else the world view's top. */
int32_t Overlay_StatusTextBottom(InGameUiImage *image)
{
    UiWrappedTextControl *status = UiNode_As<UiWrappedTextControl>(&image->worldViewWrappedStatusText);
    const UiNodeBase &base = status->base;
    bool drawn = !Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS) &&
                 ((status->labelFlags & UI_WORLD_TEXT_PAUSED_ONLY) == 0 ||
                  Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED)) &&
                 !Any(base.nodeFlags & UI_NODE_SUPPRESSED);
    if (!drawn) {
        return base.top;
    }
    /* the text as UiWrappedTextControl_DrawClipped takes it: a command stream, else a text resource id */
    uint16_t *text = status->text;
    if ((status->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
        text = TextResource_Resolve(static_cast<TextResourceId>(Thandor_PointerToU32(status->text)));
    }
    if (text == nullptr || text[0] == 0) {
        return base.top;
    }
    RichTextExtent extent =
        RichTextCommandStream_MeasureWrappedBlock(g_UiTextStyleNormal, text, std::max<int32_t>(1, base.layoutWidth));
    return base.top + static_cast<int32_t>(extent.heightPixels);
}

/* The box in logical pixels when the overlay is shown, else false. */
bool Overlay_Box(UiPixelRect *box, const OverlayLayout **outLayout)
{
    InGameUiImage *image = Overlay_Image();
    if (image == nullptr || !s_Overlay.visible || s_Overlay.line1[0] == 0 || !ObjectiveOverlay_Enabled()) {
        return false;
    }
    if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS) ||
        s_Overlay.root->activeEndMovieRuntime != nullptr) {
        return false;
    }
    uint32_t windowPage =
        UiPageStack_ActivePageIndex(UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack));
    if (windowPage >= INGAME_WINDOW_PAGE_GAME_MENU && windowPage <= INGAME_WINDOW_PAGE_MISSION_HELP) {
        return false;
    }
    const UiNodeBase &area = s_OverlayNode;
    int32_t maxBoxWidth = std::min(OVERLAY_MAX_WIDTH, std::max(OVERLAY_MIN_WIDTH, area.layoutWidth / 2));
    GraphicsTextureLogicalSize corner = g_GraphicsTextureSourceGetLogicalSize(OVERLAY_FRAME, g_UiWindowTextureSource);
    int32_t frameX = static_cast<int32_t>(corner.logicalWidthPixels);
    int32_t frameY = static_cast<int32_t>(corner.logicalHeightPixels);
    const OverlayLayout &layout = Overlay_Layout(maxBoxWidth - 2 * frameX);
    int32_t textWidth = layout.labelWidth + std::max(layout.width1, layout.width2);
    int32_t lineCount = s_Overlay.line2[0] != 0 ? 2 : 1;
    int32_t width = textWidth + 2 * (OVERLAY_PADDING_X + frameX);
    int32_t height = lineCount * layout.lineHeight + (lineCount - 1) * OVERLAY_LINE_GAP +
                     2 * (OVERLAY_PADDING_Y + frameY);
    int32_t top = std::max(image->messageHistoryPanel.base.bottom, Overlay_StatusTextBottom(image)) + OVERLAY_TOP_GAP;
    box->left = area.left + OVERLAY_LEFT_GAP;
    box->top = std::max(area.top, top);
    box->right = box->left + width;
    box->bottom = box->top + height;
    if (box->bottom > area.bottom || box->right > area.right) {
        return false; /* a tiny world view: no room */
    }
    if (outLayout != nullptr) {
        *outLayout = &layout;
    }
    return true;
}

bool Overlay_Contains(const UiPixelRect &box, UiPixelCoordinate x, UiPixelCoordinate y)
{
    return x >= box.left && x < box.right && y >= box.top && y < box.bottom;
}

void Overlay_Invalidate()
{
    if (Overlay_Image() != nullptr) {
        UiNode_InvalidateRoot(&s_OverlayNode);
    }
}

UiPixelRect Overlay_Intersect(UiPixelRect a, UiPixelRect b)
{
    return UiPixelRect{std::max(a.left, b.left), std::max(a.top, b.top), std::min(a.right, b.right),
                       std::min(a.bottom, b.bottom)};
}

} // namespace

/* drawClipped of g_InGameObjectiveOverlayVtable: the background, the frame, "Ziel:" in the highlight colour, the
   objective and the progress line below it, aligned with the objective. */
void InGameObjectiveOverlay_DrawClipped(UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,
                                        UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft, UiNodeBase *node)
{
    UiPixelRect box{};
    const OverlayLayout *layout = nullptr;
    if (!Overlay_Box(&box, &layout)) {
        return;
    }
    UiPixelRect clip = Overlay_Intersect(UiPixelRect{clipLeft, clipTop, clipRight, clipBottom}, box);
    if (clip.left >= clip.right || clip.top >= clip.bottom || g_GraphicsFramebufferBeginAccess()) {
        return;
    }
    GraphicsTextureLogicalSize corner = g_GraphicsTextureSourceGetLogicalSize(OVERLAY_FRAME, g_UiWindowTextureSource);
    int32_t frameX = static_cast<int32_t>(corner.logicalWidthPixels);
    int32_t frameY = static_cast<int32_t>(corner.logicalHeightPixels);
    g_GraphicsFramebufferFillRectArgb(clip.bottom, clip.right, clip.top, clip.left, box.bottom - 1, box.right - 1,
                                      box.top + 1, box.left + 1, OVERLAY_BACKGROUND_ARGB, g_FramebufferAccess);
    UiFramedFrame_Draw(box, s_Overlay.pressed ? OVERLAY_FRAME_PRESSED : OVERLAY_FRAME, clip);
    int32_t textLeft = box.left + frameX + OVERLAY_PADDING_X;
    int32_t textRight = box.right - frameX - OVERLAY_PADDING_X;
    int32_t lineTop = box.top + frameY + OVERLAY_PADDING_Y;
    int32_t textClipRight = std::min(clip.right, textRight);
    RichTextCommandStream_DrawSingleLine(clip.bottom, textClipRight, clip.top, clip.left, g_UiTextStyleSelected,
                                         const_cast<uint16_t *>(layout->label.data()), lineTop, textLeft);
    RichTextCommandStream_DrawSingleLine(clip.bottom, textClipRight, clip.top, clip.left, g_UiTextStyleNormal,
                                         const_cast<uint16_t *>(layout->fitted1.data()), lineTop,
                                         textLeft + layout->labelWidth);
    if (s_Overlay.line2[0] != 0) {
        RichTextCommandStream_DrawSingleLine(clip.bottom, textClipRight, clip.top, clip.left, g_UiTextStyleNormal,
                                             const_cast<uint16_t *>(layout->fitted2.data()),
                                             lineTop + layout->lineHeight + OVERLAY_LINE_GAP,
                                             textLeft + layout->labelWidth);
    }
    g_GraphicsFramebufferEndAccess();
    (void)node;
}

/* hitTest of g_InGameObjectiveOverlayVtable: the node inside the shown box, nothing elsewhere (the world view below
   gets the pointer there). */
UiNodeBase *InGameObjectiveOverlay_HitTest(UiPixelCoordinate pointerY, UiPixelCoordinate pointerX, UiNodeBase *node)
{
    UiPixelRect box{};
    if (!Overlay_Box(&box, nullptr) || !Overlay_Contains(box, pointerX, pointerY)) {
        return UI_NODE_NONE;
    }
    return node;
}

/* nonRightPress of g_InGameObjectiveOverlayVtable: shows the box pressed (the release opens the mission help). */
void InGameObjectiveOverlay_NonRightPress(UiPointerWheelDelta wheelDelta, UiPixelCoordinate pointerY,
                                          UiPixelCoordinate pointerX, UiNodeBase *node)
{
    (void)wheelDelta;
    (void)pointerY;
    (void)pointerX;
    s_Overlay.pressed = true;
    UiNode_InvalidateRoot(node);
}

/* nonRightRelease of g_InGameObjectiveOverlayVtable: a release over the box opens the mission help as F1 does
   (the objectives button selected, its click sound, then its action 0x101F, InGameMissionHelpPage_Toggle). */
void InGameObjectiveOverlay_NonRightRelease(UiPointerWheelDelta wheelDelta, UiPixelCoordinate pointerY,
                                            UiPixelCoordinate pointerX, UiNodeBase *node)
{
    (void)wheelDelta;
    bool wasPressed = s_Overlay.pressed;
    s_Overlay.pressed = false;
    UiNode_InvalidateRoot(node);
    UiPixelRect box{};
    InGameUiImage *image = Overlay_Image();
    if (!wasPressed || image == nullptr || !Overlay_Box(&box, nullptr) || !Overlay_Contains(box, pointerX, pointerY)) {
        return;
    }
    UiSpriteButtonControl *button = &image->missionObjectivesButton;
    UiSelectableControl_SetSelected(1, &button->selectable);
    if (Any(button->selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) && button->activationSound != nullptr) {
        g_SoundPlayOneShot(g_UiSoundGainQ15, g_UiSoundGainQ15, button->activationSound, nullptr);
    }
    UiActionQueue_Enqueue(INGAME_ACTION_MISSION_HELP, &button->selectable.base);
}

UiNodeVtable g_InGameObjectiveOverlayVtable = {
    .relocate = UI_SLOT(UiContainer_RelocateChildren),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(InGameObjectiveOverlay_DrawClipped),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(InGameObjectiveOverlay_NonRightPress),
    .nonRightRelease = UI_SLOT(InGameObjectiveOverlay_NonRightRelease),
    .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(InGameObjectiveOverlay_HitTest),
    .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
    .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
    .tick = UI_SLOT(UiNode_DefaultTick),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

void ObjectiveOverlay_SetLines(const uint16_t *line1, const uint16_t *line2)
{
    OverlayLine copy1{};
    OverlayLine copy2{};
    Overlay_CopyLine(copy1, line1);
    Overlay_CopyLine(copy2, line2);
    if (copy1 == s_Overlay.line1 && copy2 == s_Overlay.line2) {
        return;
    }
    s_Overlay.line1 = copy1;
    s_Overlay.line2 = copy2;
    s_Overlay.layout.valid = false;
    Overlay_Invalidate();
}

void ObjectiveOverlay_SetVisible(bool visible)
{
    if (s_Overlay.visible != visible) {
        s_Overlay.visible = visible;
        Overlay_Invalidate();
    }
}

void ObjectiveOverlay_AttachToRoot(InGameRuntimeRoot *root)
{
    if (root == nullptr) {
        return;
    }
    InGameUiImage *image = InGameUi_Image(root);
    UiNodeBase &worldView = image->worldView.base;
    s_Overlay.line1.fill(0);
    s_Overlay.line2.fill(0);
    s_Overlay.layout.valid = false;
    s_Overlay.visible = true;
    s_Overlay.pressed = false;
    s_Overlay.root = root;
    s_Update.attachGeneration++;
    s_Update.built = false;
    s_Update.sample = false;
    /* sibling right after the world view: drawn above it, below the windows; hit-tested before it */
    s_OverlayNode.parent = worldView.parent;
    s_OverlayNode.nextSibling = worldView.nextSibling;
    s_OverlayNode.firstChild = UI_NODE_NONE;
    s_OverlayNode.nodeFlags = UiNodeFlags{};
    worldView.nextSibling = &s_OverlayNode;
    /* laid out like the world view (the whole world view area) until the next root layout does it */
    UiNodeBase &area = image->worldViewArea.base;
    s_OverlayNode.left = area.left;
    s_OverlayNode.top = area.top;
    s_OverlayNode.right = area.right;
    s_OverlayNode.bottom = area.bottom;
    s_OverlayNode.layoutWidth = area.right - area.left;
    s_OverlayNode.layoutHeight = area.bottom - area.top;
#ifdef THANDOR_DEV_TOOLS
    /* test aid: sample lines (screenshots of the overlay before the objective model feeds it) */
    if (const char *sample = std::getenv("OPEN_THANDOR_OBJECTIVE_SAMPLE")) {
        static constexpr char16_t SAMPLE_LINE1[] = u"Zerst\u00F6re die feindliche Basis";
        static constexpr char16_t SAMPLE_LINE2[] = u"Xenit 3200 / 5000";
        static constexpr char16_t SAMPLE_LONG[] =
            u"Halte die Stellung am Pass, bis die Verst\u00E4rkung aus dem Norden eintrifft und die Br\u00FCcke "
            u"gesichert ist";
        auto units = [](const char16_t *text) { return reinterpret_cast<const uint16_t *>(text); };
        s_Update.sample = sample[0] != 0 && sample[0] != '0';
        if (std::strcmp(sample, "long") == 0) {
            ObjectiveOverlay_SetLines(units(SAMPLE_LONG), units(SAMPLE_LINE2));
        } else if (std::strcmp(sample, "1line") == 0) {
            ObjectiveOverlay_SetLines(units(SAMPLE_LINE1), nullptr);
        } else if (sample[0] != 0 && sample[0] != '0') {
            ObjectiveOverlay_SetLines(units(SAMPLE_LINE1), units(SAMPLE_LINE2));
        }
    }
#endif
}

void ObjectiveOverlay_DetachFromRoot(InGameRuntimeRoot *root)
{
    if (root == nullptr || s_Overlay.root != root) {
        return;
    }
    InGameUiImage *image = InGameUi_Image(root);
    /* unlink from the world view's sibling chain (the node stays right after the world view) */
    for (UiNodeBase *node = &image->worldView.base; node != UI_NODE_NONE; node = node->nextSibling) {
        if (node->nextSibling.get() == &s_OverlayNode) {
            node->nextSibling = s_OverlayNode.nextSibling;
            break;
        }
    }
    s_OverlayNode.nextSibling = UI_NODE_NONE;
    s_OverlayNode.parent = UI_NODE_NONE;
    s_Overlay.root = nullptr;
    s_Overlay.pressed = false;
    s_Overlay.line1.fill(0);
    s_Overlay.line2.fill(0);
    s_Overlay.layout.valid = false;
    s_Update.built = false;
    s_Update.objective = MissionObjective{};
}

bool ObjectiveOverlay_Enabled()
{
    return PersistentSettings_Read(PERSISTENT_OBJECTIVE_OVERLAY_ON, PERSISTENT_SETTING_OBJECTIVE_OVERLAY) !=
           PERSISTENT_OBJECTIVE_OVERLAY_OFF;
}

bool ObjectiveOverlay_ToggleSetting()
{
    bool shown = !ObjectiveOverlay_Enabled();
    PersistentSettings_WriteObjectiveOverlay(shown);
    Overlay_Invalidate(); /* ObjectiveOverlay_Update fills the lines with the next frame */
    return shown;
}

void ObjectiveOverlay_Update()
{
    InGameRuntimeRoot *root = s_Overlay.root;
    if (root == nullptr || root != g_InGameRuntimeRoot) {
        return;
    }
    bool editor = Any((root->worldRuntime).runtimeFlags & INGAME_WORLD_FLAG_EDITOR);
    ObjectiveOverlay_SetVisible(!editor);
    bool enabled = ObjectiveOverlay_Enabled();
    auto now = std::chrono::steady_clock::now();
    if (enabled != s_Update.enabled) {
        s_Update.enabled = enabled;
        s_Update.nextRefresh = now; /* switched on: fresh values at once */
        Overlay_Invalidate();
    }
    if (!enabled || editor || s_Update.sample) {
        return;
    }
    const InGameLevelConditionStorage *storage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
    uint32_t faction = static_cast<uint32_t>((root->worldRuntime).activeFactionRuntimeIndex);
    uint32_t language = Game_LanguageGeneration();
    if (!s_Update.built || s_Update.builtGeneration != s_Update.attachGeneration || s_Update.builtStorage != storage ||
        s_Update.builtFaction != faction || s_Update.builtLanguage != language) {
        s_Update.objective = MissionObjective_Build(storage, faction);
        s_Update.built = true;
        s_Update.builtGeneration = s_Update.attachGeneration;
        s_Update.builtStorage = storage;
        s_Update.builtFaction = faction;
        s_Update.builtLanguage = language;
        s_Update.nextRefresh = now;
    }
    if (now < s_Update.nextRefresh) {
        return;
    }
    s_Update.nextRefresh = now + OVERLAY_REFRESH_INTERVAL;
    std::array<uint16_t, MISSION_OBJECTIVE_LINE_UNITS> progress{};
    bool hasProgress = s_Update.objective.source != MissionObjectiveSource::NONE &&
                       MissionObjective_FormatProgress(&s_Update.objective, progress.data(), progress.size());
    ObjectiveOverlay_SetLines(s_Update.objective.line1, hasProgress ? progress.data() : nullptr);
}
